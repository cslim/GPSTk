#include <iostream>

#include "SystemTime.hpp"
#include"CommonTime.hpp"
#include"CivilTime.hpp"
#include"OrbitSim.h"
#include"SphericalHarmonicsModel.h"
#include"Rinex3ObsStream.hpp"
#include"Rinex3ObsData.hpp"
#include"Rinex3EphemerisStore.hpp"
#include"Exception.hpp"
#include"DataStructures.hpp"
#include"Rinex3EphemerisStore.hpp"

#include "PrSmoother.h"

#include"Solution.h"
#include"Action.h"
#include"PPPSolution.h"

using namespace std;
using namespace gpstk;
using namespace pod;

void testRinNav(char* path)
{
    Rinex3EphemerisStore nrin;
    nrin.loadFile(path);
    auto &sid = SatID(2, SatID::SatelliteSystem::systemGPS);
    CommonTime t0 = nrin.getInitialTime(sid);
    CommonTime te = nrin.getFinalTime(sid);
    nrin.SearchNear();
    nrin.getXvt(sid, t0);

}
void testRinParse(char* path)
{

    // Create the input file stream
    Rinex3ObsStream rin(path);

    // Create the output file stream
    ofstream rout("_cng2182a_.17o", ios::out);
    gnssRinex  gRin;
    // Read the RINEX header
    Rinex3ObsHeader head;    //RINEX header object
    rin >> head;
    // rout.header = head;

    //   rout << head;
    auto obsT = head.obsTypeList;
    // Loop over all data epochs
    Rinex3ObsData data;   //RINEX data object
    while (rin >> data)
    {
        rout << data.time << endl;
        for (auto &ot : obsT)
        {
            for (auto &it : data.obs)
            {
                auto val = data.getObs(it.first, ot, head);

                rout << it.first << " " << it.first.id << " ";
                //for (auto &it1 : it.second)
                //    cout << it1.;
                //cout << endl;
            }
        }
        rout << endl;
        /*for (auto &it : gRin.getTypeID())
        {
        cout << it << " ";
        }
        cout << endl;*/

        //gRin.body.dump(cout);
    }
    rout.close();
}

void testPod(char * path)
{
    auto t = clock();
    Action a(path);
    auto gMap = a.process();
    auto t2 = clock();
    cout << "process complete ";
    cout << (std::clock() - t) / (double)CLOCKS_PER_SEC << endl;
    cout <<"epochs "<< gMap.data.size() << endl;
    // ofstream f("dump.txt");
    // gMap.dump(f);

    //f.close();

}

void codeSmoother(const char * iPath, int window)
{
    list<TypeID> ids{ TypeID::C1,TypeID::P1,TypeID::P2 };
    PrSmoother prsm(ids, window);
    prsm.smooth(iPath);

}

int main(int argc, char* argv[])
{
    //Solution sol(argv[1]);
    //sol.chekObs();
    //codeSmoother(argv[1], atoi(argv[2]));
    //testgpstk
    //testRinNav(argv[1]);

    //testRinParse(argv[1]);
    testPod(argv[1]);
    system("pause");
    return 0;
}


