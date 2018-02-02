#include"CodeSolverBase.h"
#include<math.h>
#include"Matrix.hpp"
#include"FsUtils.h"
#include"WinUtils.h"

namespace pod
{
    double CodeSolverBase::eps = 1e-3;
    GPSEllipsoid CodeSolverBase::ellGPS;

    CodeSolverBase::CodeSolverBase(GnssDataStore_sptr data) :
        maskEl(data->opts.maskEl), maskSNR(data->opts.maskSNR), maxIter(15), Sol(5),
        sigmaMax(25), ionoType(ComputeIonoModel::DualFreq), RMS3D(DBL_MAX), PDOP(DBL_MAX), sigma(0)
    {
       
        Sol = 0.0;
    };
    void CodeSolverBase::refreshSolution(Vector<double> &Sol, Vector<double> &dSol)
    {
        if (Sol.size() == dSol.size())
        {
            Sol += dSol;
            return;
        }
        int K = dSol.size();
        Vector<double> Sol1(K);

        int m = min(Sol.size(), Sol1.size());

        for (size_t i = 0; i < m; i++)
            Sol1[i] = Sol[i] + dSol[i];

        for (size_t j = m; j < dSol.size(); j++)
            Sol1[j] += dSol[j];

        Sol = Sol1;
    }

    void  CodeSolverBase::selectObservables(
        const Rinex3ObsData &rod,
        const Rinex3ObsHeader& roh,
        const set<SatID::SatelliteSystem> &systems,
        const ObsTypes & typeMap,
        CodeProcSvData & svData,
        bool isApplyRCO 
    )
    {
        // Let's compute an useful constant (also found in "GNSSconstants.hpp")
        const double gamma = (L1_FREQ_GPS / L2_FREQ_GPS)*(L1_FREQ_GPS / L2_FREQ_GPS);
        // Apply editing criteria
        if (rod.epochFlag == 0 || rod.epochFlag == 1)  // Begin usable data
        {
            for (auto &it : rod.obs)
            {
                if (systems.find(it.first.system) == systems.end())
                    continue;
                const auto &ids = typeMap.at(it.first.system);

                double C1(0.0);
                char S1(0);
                try
                {
                    C1 = rod.getObs(it.first, ids.at(TypeID::C1),roh).data;
                    
                }
                catch (gpstk::Exception &e)
                {
                    continue;
                }
                try
                {
                    S1 = rod.getObs(it.first, ids.at(TypeID::S1), roh).data;
                }
                catch (gpstk::Exception &e)
                {

                }

                double ionocorr(0.0);
                if (ionoType == ComputeIonoModel::IonoModelType::DualFreq)
                {
                    double P2(0.0);
                    try
                    {
                        P2 = rod.getObs(it.first, ids.at(TypeID::P2), roh).data;
                    }
                    catch (gpstk::Exception &e)
                    {
                        continue;
                    }
                    if (P2 == 0) continue;
                    ionocorr = 1.0 / (1.0 - gamma) * (C1 - P2);
                    C1 -= ionocorr;
                }
              
                if (isApplyRCO) C1 -= rod.clockOffset*C_MPS;
                
                CodeProcSvData::SvDataItem svDatai;
                svDatai.pr = C1;
                svDatai.snr = S1;
                svData.tryAdd(it.first, svDatai);
            }
        }
    }

    void CodeSolverBase::prepare(
        const CommonTime &t,
        const XvtStore<SatID>& Eph,
        CodeProcSvData & svData)
    {
        CommonTime tx;

        for (auto it = svData.data.rbegin();it!= svData.data.rend();  ++it)
        {
            if (it->second.use)
            {
                // transmit time
                Xvt PVT;
                // first estimate of transmit time
                tx = t;
                tx -= it->second.pr / C_MPS;

                // get ephemeris range, etc
                try
                {
                    PVT = Eph.getXvt(it->first, tx);
                }
                catch (InvalidRequest& e)
                {
                    svData.tryRemove(it->first);
                    continue;
                }

                tx -= PVT.clkbias + PVT.relcorr;

                // SVP = {SV position at transmit time}, raw range + clk + rel
                for (int l = 0; l < 3; l++)
                {
                    it->second.pos[l] = PVT.x[l];
                }
                it->second.pr = it->second.pr + C_MPS * (PVT.clkbias + PVT.relcorr);
            }
          
        }
    }

    int CodeSolverBase::solveInter(
        const gpstk::CommonTime &t,
        const gpstk::IonoModelStore &iono,
        CodeProcSvData & svsData,
        gpstk::Matrix<double>& Cov        
    )
    {
        this->iter = 0;
        double conv = DBL_MAX;
        while(true)
        {
            svsData.updateSolutionLength(Sol);
            Position rxPos(Sol(0), Sol(1), Sol(2));
          
            for (auto &it : svsData.data)
            {
                if (!it.second.use) continue;
                Triple &svp = (it.second.pos);

                double rho(0.0);
                // time of flight (sec)
                if (iter == 0)
                    rho = 0.070;             // initial guess: 70ms
                else
                    rho = RSS(svp[0] - Sol(0), svp[1] - Sol(1), svp[2] - Sol(2)) / C_MPS;

                // correct for earth rotation
                double  wt = ellGPS.angVelocity()*rho;             // radians
                double  svxyz[3];

                svxyz[0] = ::cos(wt)*svp[0] + ::sin(wt)*svp[1];
                svxyz[1] = -::sin(wt)*svp[0] + ::cos(wt)*svp[1];
                svxyz[2] = svp[2];

                double ioDel(0.0), tropoDel(0.0);
                if (conv < 1000)
                {
                    //
                    Position svPos(svp);
                    double elv = rxPos.elevationGeodetic(svPos);
                    it.second.el = elv;
                    if (elv < maskEl)
                    {
                        it.second.use = false;
                        continue;
                    }
                    if (ionoType == ComputeIonoModel::IonoModelType::Klobuchar)
                    {
                        double azm = rxPos.azimuth(svPos);
                        ioDel = iono.getCorrection(t, rxPos, elv, azm);
                    }

                    tropoDel = getTropoCorrection(rxPos, svPos, t);
                }

                // geometric range
                rho = RSS(svxyz[0] - Sol(0), svxyz[1] - Sol(1), svxyz[2] - Sol(2));

                it.second.alph[0] = (Sol(0) - svxyz[0]) / rho;           // x direction cosine
                it.second.alph[1] = (Sol(1) - svxyz[1]) / rho;           // y direction cosine
                it.second.alph[2] = (Sol(2) - svxyz[2]) / rho;           // z direction cosine
               
                // system specific correction
                double dts = svsData.appendResid(Sol, it.first.system);
                //corrected range residual
                it.second.resid = it.second.pr - rho - Sol(3) - ioDel - tropoDel - dts;
            }
            
            Matrix<double> A, W;
            Vector<double> b;
            int i = svsData.getEquations(A, W, b);
            if (i)
            {
                sigma = DBL_MAX;
                return i;
            }

            Matrix<double> AT = transpose(A);
            Cov = AT *W* A;
            
            DBOUT("\nW\n" <<W << endl;);
            DBOUT("\nA\n" << A << endl;);
            DBOUT("\nb\n"<<b << endl;);
            DBOUT(" " << Sol << endl<<endl;);

            try
            {
                Cov = inverseSVD(Cov);
            }
            catch (MatrixException& sme)
            {
                sigma = DBL_MAX;
                return -2;
            }
            //       T     -1 T
            //dX = (A W A )  A W b
            Vector<double> dX = Cov * AT* W* b;

            refreshSolution(Sol, dX);
           
            // test for convergence
            conv = norm(dX);
            this->iter++;
            if (conv < eps || this->iter >= maxIter)
            {
                calcSigma(rxPos, W, b, svsData);
                break;
            }        
        } 
        return 0;
    }

    void CodeSolverBase::calcSigma(
        const Position& rxPos,
        const Matrix<double> & W,
        const Vector<double> & b,
        const CodeProcSvData &svsData)
    {
        double h = rxPos.getAltitude();
        if (h > 1000000.0 || h < -200)
        {
            sigma = DBL_MAX;
            return ;
        }
        double vpv(0);
        auto pv = (W*b);
        for (size_t i = 0; i < b.size(); i++)
            vpv += b(i)*pv(i);
        int r = svsData.getNumUsedSv() - svsData.getParamNum();
        sigma = sqrt(vpv / r);
        
    }

    int CodeSolverBase::solve(
        const gpstk::CommonTime &t,
        const gpstk::IonoModelStore &iono,
        CodeProcSvData & svsData

    )
    {
        Matrix<double> Cov;
        solveInter(t, iono, svsData, Cov );

        calcStat(Cov);

        if (sigma > sigmaMax)
            return catchSatByResid(t, iono, svsData);

        return 0;
    }

    int CodeSolverBase::catchSatByResid(
        const CommonTime & t,
        const IonoModelStore & iono,
        CodeProcSvData & svsData
        )
    {
        Matrix<double> Cov;
        for (auto & it : svsData.data)
        {
            svsData.applyCNoMask(this->maskSNR);

            Sol = 0.0;
            if (!it.second.use) continue;
            
            it.second.use = false;
            solveInter(t, iono, svsData, Cov);

            calcStat(Cov);
            if (this->sigma < sigmaMax)
                return 0;
            else
                it.second.use = true;
        }
        return -1;
    }

    void CodeSolverBase::calcStat(const Matrix<double>& Cov)
    {
        RMS3D = 0, PDOP = 0;

        for (size_t i = 0; i < 3; i++)
        {
            RMS3D += sigma*sigma*Cov(i, i);
            PDOP += Cov(i, i);
        }
        PDOP = sqrt(PDOP);
        RMS3D = sqrt(RMS3D);
    }

    /// stream output for CodeSolverBase
    std::ostream& operator<<(std::ostream& os, 
        const CodeSolverBase& solver)
    {
        os << setprecision(10) << " ";
        for (size_t i = 0; i < solver.Sol.size(); i++)
            os << solver.Sol(i) << " ";

        os << solver.iter << " "  << setprecision(3) << solver.sigma << " " << solver.RMS3D << " " << solver.PDOP;
        return os;
    }
}