#include"PRSolverBase.h"
#include<math.h>
#include"Matrix.hpp"
namespace POD
{
    double PRSolverBase::eps = 1e-4;
    GPSEllipsoid PRSolverBase::ellGPS;

    void  PRSolverBase::selectObservables(
        const Rinex3ObsData &rod,
        int iL1PR,
        int iL2PR,
        int iL1SNR,
        vector<SatID> & PRNs,
        vector<double> &L1PRs,
        vector<uchar> & SNRs,
        bool isApplyRCO
    )
    {
        // Let's compute an useful constant (also found in "GNSSconstants.hpp")
        const double gamma = (L1_FREQ_GPS / L2_FREQ_GPS)*(L1_FREQ_GPS / L2_FREQ_GPS);
        // Apply editing criteria
        if (rod.epochFlag == 0 || rod.epochFlag == 1)  // Begin usable data
        {
            Rinex3ObsData::DataMap::const_iterator it;

            for (auto it : rod.obs)
            {
                double C1(0.0);
                char S1(0);
                try
                {
                    C1 = rod.getObs(it.first, iL1PR).data;
                    S1 = rod.getObs(it.first, iL1SNR).data;
                }
                catch (...)
                {
                    // Ignore this satellite if P1 is not found
                    continue;
                }

                double ionocorr(0.0);
                if (ionoType == PRIonoCorrType::IF)
                {
                    double P2(0.0);
                    try
                    {
                        P2 = rod.getObs(it.first, iL2PR).data;
                    }
                    catch (...)
                    {
                        continue;
                    }
                    if (P2 == 0) continue;
                    ionocorr = 1.0 / (1.0 - gamma) * (C1 - P2);
                    C1 -= ionocorr;
                }

                PRNs.push_back(it.first);
                SNRs.push_back(S1);
                if (isApplyRCO) C1 -= rod.clockOffset*C_MPS;
                L1PRs.push_back(C1);
            }
        }
    }

    void PRSolverBase::prepare(
        const CommonTime &t,
        vector<SatID> &IDs,
        const vector<double> &PRs,
        const XvtStore<SatID>& Eph,
        vector<bool> &useSat,
        Matrix<double> &SVP)
    {
        CommonTime tx;

        for (size_t i = 0; i < IDs.size(); i++)
        {
            if (!useSat[i]) continue;
            // transmit time
            Xvt PVT;
            // first estimate of transmit time
            tx = t;
            tx -= PRs[i] / C_MPS;
            // get ephemeris range, etc
            try
            {
                PVT = Eph.getXvt(IDs[i], tx);
            }
            catch (InvalidRequest& e)
            {
                useSat[i] = false;
                continue;
            }

            tx -= PVT.clkbias + PVT.relcorr;

            try
            {
                PVT = Eph.getXvt(IDs[i], tx);
            }
            catch (InvalidRequest& e)
            {
                ///Negate SatID because getXvt failed.
                IDs[i].id = -::abs(IDs[i].id);
                useSat[i] = false;
                continue;
            }
            // SVP = {SV position at transmit time}, raw range + clk + rel
            for (int l = 0; l < 3; l++)
            {
                SVP(i, l) = PVT.x[l];
            }
            SVP(i, 3) = PRs[i] + C_MPS * (PVT.clkbias + PVT.relcorr);
        }
    }

    void PRSolverBase::calcStat(Vector<double> resid, Matrix<double> Cov)
    {
        RMS3D = 0, PDOP = 0;
        double variance = ps.variance();
        this->sigma = sqrt(variance);

        for (size_t i = 0; i < 3; i++)
        {
            RMS3D += variance*Cov(i, i);
            PDOP += Cov(i, i);
        }
        PDOP = sqrt(PDOP);
        RMS3D = sqrt(RMS3D);
    }

    string PRSolverBase::printSolution(const vector<bool> &useSat)
    {
        std::ostringstream strs;
        strs << setprecision(12) << " " << Sol(0) << " " << Sol(1) << " " << Sol(2) << " " << Sol(3) / C_MPS << " " << iter << " " << useSat.size() << " " << ps.size() << " " << setprecision(3) << sigma << " " << RMS3D << " " << PDOP;
        return strs.str();
    }
}