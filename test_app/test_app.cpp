
#include "CommonTime.hpp"
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
#include"SQLiteAdapter.h"

#include <iostream>
#include<filesystem>
#include<chrono>
#include <iostream>
#include <iomanip>


using namespace std;
using namespace gpstk;
using namespace pod;

void testRinNav(char* path)
{
    Rinex3EphemerisStore nrin;
    nrin.loadFile(path);
    auto &sid = SatID(2, SatID::SatelliteSystem::systemBeiDou);
    CommonTime t0 = nrin.getInitialTime(sid);
    CommonTime te = nrin.getFinalTime(sid);
    nrin.SearchNear();
    auto t = t0 + (te - t0) / 2;
    nrin.getXvt(sid, t);
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
    auto t1 = clock();
    Solution sol(path);
    sol.process();
    cout << "process complete ";
    auto t2 = clock();
    cout << (t2 - t1) / (double)CLOCKS_PER_SEC << endl;
    cout << "processed epochs: " << sol.getData().data.size() << endl;
    sol.saveToDb();

    cout << "inserting to db complete ";
    cout << (std::clock() - t2) / (double)CLOCKS_PER_SEC << endl;

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
    //cout << CivilTime(CommonTime::BEGINNING_OF_TIME) << endl;
    //Solution sol(argv[1]);
    //sol.chekObs();
    //codeSmoother(argv[1], atoi(argv[2]));
    //testRinNav(argv[1]);
    //SQLiteAdapter:: testSQLite(argv[1], argv[2]);
    //test();

    //testRinParse(argv[1]);
    testPod(argv[1]);
    //system("pause");
    return 0;
}
