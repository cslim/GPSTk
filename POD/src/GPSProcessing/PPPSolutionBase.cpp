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

#include"GnssSolution.h"

using namespace gpstk;

namespace pod
{

    PPPSolutionBase::PPPSolutionBase(GnssDataStore_sptr procData ) :GnssSolution(procData,1.0)
    {
    } 
    PPPSolutionBase:: ~PPPSolutionBase()
    {
        solverPR.release();
    }

    void PPPSolutionBase::mapSNR(IRinex & gRin)
    {
        for (auto &it1 : gRin.getBody())
        {
            auto  ts1 = TypeID(TypeID::S1);
            auto  ts2 = TypeID(TypeID::S2);
           
            auto s1 = it1.second->get_value().find(ts1);
            if (s1 != it1.second->get_value().end())
                s1->second =  mapSNR(s1->second);

            auto s2 = it1.second->get_value().find(ts1);
            if (s2 != it1.second->get_value().end())
                s2->second = mapSNR(s2->second);
        }
    }

    void PPPSolutionBase::PRProcess()
    {
        NeillTropModel NeillModel = solverPR->initTropoModel(nominalPos, opts().DoY);

        data->apprPos.clear();

        cout << "solverType " << solverPR->getName() << endl;

        solverPR->maskEl = 5;
        solverPR->ionoType = data->ionoCorrector.getType();

        ofstream os;
        string outPath = opts().workingDir + "\\" + data->apprPosFile;
        os.open(outPath);
       
        //decimation
        int sampl(1);
        double tol(0.1);

        for (auto obsFile : data->getObsFiles(opts().SiteRover))
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
                            res = solverPR->solve(rod.time, data->bceIonoStore, svData);
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
   
  
}