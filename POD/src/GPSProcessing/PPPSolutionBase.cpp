#include"PPPSolutionBase.h"

#include<list>
#include<set>
#include <direct.h>
#include<windows.h>
#include<regex>

#include"GnssEpochMap.h"
#include"PPPSolution.h"
#include"PODSolution.h"
#include"FsUtils.h"


#include "Rinex3NavHeader.hpp"
#include "Rinex3NavData.hpp"
#include "Rinex3NavStream.hpp"
#include"GnssDataStore.hpp"

namespace pod
{
    PPPSolutionBase* PPPSolutionBase::Factory(GnssDataStore_sptr data)
    {
        if (data->opts.isSpaceborneRcv)
            return new PODSolution(data);
        else
            return new PPPSolution(data);
    }

    PPPSolutionBase::PPPSolutionBase(GnssDataStore_sptr procData ) :GnssSolution(procData)
    {
    } 
    PPPSolutionBase:: ~PPPSolutionBase()
    {
        solverPR.release();
    }

    void PPPSolutionBase::mapSNR(gnssRinex & gRin)
    {
        for (auto &it1 : gRin.body)
        {
            auto  ts1 = TypeID(TypeID::S1);
            auto  ts2 = TypeID(TypeID::S2);
           
            auto s1 = it1.second.find(ts1);
            if (s1 != it1.second.end())
                s1->second =  mapSNR(s1->second);

            auto s2 = it1.second.find(ts1);
            if (s2 != it1.second.end())
                s2->second = mapSNR(s2->second);
        }
    }

    void PPPSolutionBase::PRProcess()
    {
        NeillTropModel NeillModel = solverPR->initTropoModel(nominalPos, opts().DoY);

        data->apprPos.clear();

        cout << "solverType " << solverPR->getName() << endl;

        solverPR->maskEl = 5;
        solverPR->ionoType = (CodeIonoCorrType)confReader().getValueAsInt("CodeIonoCorrType");

        ofstream os;
        string outPath = data->workingDir + "\\" + data->apprPosFile;
        os.open(outPath);
       
        //decimation
        int sampl(1);
        double tol(0.1);

        for (auto obsFile : data->rinexObsFiles)
        {
            int badSol(0);
            cout << obsFile << endl;
            try 
            {
                //Input observation file stream
                Rinex3ObsStream rin;
                // Open Rinex observations file in read-only mode
                rin.open(obsFile, std::ios::in);

                rin.exceptions(ios::failbit);
                Rinex3ObsHeader roh;
                Rinex3ObsData rod;

                //read the header
                rin >> roh;
                CommonTime ct0 = roh.firstObs;
                CommonTime Tpre( CommonTime::BEGINNING_OF_TIME);
                Tpre.setTimeSystem(TimeSystem::Any);
                // Let's process all lines of observation data, one by one
                while (rin >> rod)
                {
                    //work around for post header comments
                    if (std::abs(rod.time - Tpre) <= CommonTime::eps) continue;
                    Tpre = rod.time;
                    double dt = rod.time - ct0;
                    GPSWeekSecond gpst = static_cast<GPSWeekSecond>(rod.time);

                    if (fmod(gpst.getSOW(), sampl) > tol) continue;

                    int GoodSats = 0;
                    int res = 0;
                    CodeProcSvData svData;
                    ///
                    solverPR->selectObservables(rod, roh, opts().systems, CodeProcSvData::obsTypes, svData);
                    
                    for (auto &it : svData.data)
                        it.second.snr = mapSNR(it.second.snr);
                    
                    svData.applyCNoMask(solverPR->maskSNR);
                    GoodSats = svData.getNumUsedSv();

                    os  << setprecision(6);
                    os  << CivilTime(gpst).printf("%02Y %02m %02d %02H %02M %02S %P") << " "<<dt<< " ";

                    if (GoodSats >= 4)
                    {
                        try
                        {
                            solverPR->prepare(rod.time, data->SP3EphList, svData);
                            res = solverPR->solve(rod.time, data->ionoStore, svData);
                            os << res;
                        }
                        catch (Exception &e)
                        {
                            GPSTK_RETHROW(e)
                        }
                    }
                    else
                        res = -1;

                    os << *solverPR<<" "<<svData << endl;

                    if (res == 0)
                    {
                        Xvt xvt;
                        xvt.x = Triple(solverPR->Sol(0), solverPR->Sol(1), solverPR->Sol(2));
                        xvt.clkbias = solverPR->Sol(3);
                        data->apprPos.insert(pair<CommonTime, Xvt>(rod.time, xvt));
                    }
                    else
                    {
                        solverPR->Sol = 0.0;
                        badSol++;
                    }
                }
                rin.close();
            }
            catch (Exception& e)
            {
                cerr << e << endl;
                GPSTK_RETHROW(e);
            }
            catch (...)
            {
                cerr << "Caught an unexpected exception." << endl;
            }
            cout << "Number of bad solutions for file " << badSol << endl;
        }
    }
    //
    void PPPSolutionBase::process()
    {

        if (opts().isComputeApprPos)
        {
            PRProcess();
        }
        else
        {
            cout << "Approximate Positions loading from \n" + data->workingDir + "\\" + data->apprPosFile + "\n... ";

            data->loadApprPos();
            cout << "\nComplete." << endl;
        }
        try
        {
            processCore();

            gMap.updateMetadata();
        }
        catch (ConfigurationException &conf_exp)
        {
            cerr << conf_exp.what() << endl;
            throw;
        }
        catch (Exception &gpstk_e)
        {
            GPSTK_RETHROW(gpstk_e);
        }
        catch (std::exception &std_e)
        {
            cerr << std_e.what() << endl;
            throw;
        }
    }

    // Method to print solution values
    void PPPSolutionBase::printSolution(ofstream& outfile,
        const SolverLMS& solver,
        const CommonTime& time,
        GnssEpoch &   gEpoch,
        double dryTropo,
        int   precision,
        const Position &nomXYZ)
    {
        // Prepare for printing
        outfile << fixed << setprecision(precision);

        // Print results
        outfile << static_cast<YDSTime>(time).year << "-";   // Year           - #1
        outfile << static_cast<YDSTime>(time).doy << "-";    // DayOfYear      - #2
        outfile << static_cast<YDSTime>(time).sod << "  ";   // SecondsOfDay   - #3
        outfile << setprecision(6) << (static_cast<YDSTime>(time).doy + static_cast<YDSTime>(time).sod / 86400.0) << "  " << setprecision(precision);

        // We add 0.1 meters to 'wetMap' because 'NeillTropModel' sets a
        // nominal value of 0.1 m. Also to get the total we have to add the
        // dry tropospheric delay value
        // ztd - #7
        double wetMap = solver.getSolution(TypeID::wetMap) + 0.1 + dryTropo;

        gEpoch.slnData.insert(pair<TypeID, double>(TypeID::recZTropo, wetMap));


        double x = nomXYZ.X() + solver.getSolution(TypeID::dx);    // dx    - #4
        double y = nomXYZ.Y() + solver.getSolution(TypeID::dy);    // dy    - #5
        double z = nomXYZ.Z() + solver.getSolution(TypeID::dz);    // dz    - #6

        gEpoch.slnData.insert(pair<TypeID, double>(TypeID::recX, x));
        gEpoch.slnData.insert(pair<TypeID, double>(TypeID::recY, y));
        gEpoch.slnData.insert(pair<TypeID, double>(TypeID::recZ, z));

        double varX = solver.getVariance(TypeID::dx);     // Cov dx    - #8
        double varY = solver.getVariance(TypeID::dy);     // Cov dy    - #9
        double varZ = solver.getVariance(TypeID::dz);     // Cov dz    - #10
        double sigma = sqrt(varX + varY + varZ);

        double cdt = solver.getSolution(TypeID::cdt);
        gEpoch.slnData.insert(pair<TypeID, double>(TypeID::recCdt, cdt));

        //
        outfile << x << "  " << y << "  " << z << "  " << cdt << " ";

        auto defeq = solver.getDefaultEqDefinition();

        auto itcdtGLO = defeq.body.find(TypeID::recCdtGLO);
        if (defeq.body.find(TypeID::recCdtGLO) != defeq.body.end())
        {
            double cdtGLO = solver.getSolution(TypeID::recCdtGLO);
            gEpoch.slnData.insert(pair<TypeID, double>(TypeID::recCdtGLO, cdtGLO));

            outfile << cdtGLO << " ";
        }

        if (defeq.body.find(TypeID::recCdtdot) != defeq.body.end())
        {
            double recCdtdot = solver.getSolution(TypeID::recCdtdot);
            gEpoch.slnData.insert(pair<TypeID, double>(TypeID::recCdtdot, recCdtdot));

            outfile <<setprecision(12) << recCdtdot << " ";
        }

        gEpoch.slnData.insert(pair<TypeID, double>(TypeID::sigma, sigma));
        outfile << setprecision(6) <<wetMap << "  " << sigma << "  ";

        gEpoch.slnData.insert(pair<TypeID, double>(TypeID::recSlnType, 16));

        outfile << gEpoch.satData.size() << endl;    // Number of satellites - #12

        return;

    }  // End of method 'ex9::printSolution()'
    
    void PPPSolutionBase::updateNomPos(const CommonTime& time, Position &nominalPos)
    {
        if (opts().dynamics != GnssDataStore::Dynamics::Static)
        {
            auto it_pos = data->apprPos.find(time);
            if (it_pos != data->apprPos.end())
                nominalPos = it_pos->second;
        }
    }
}