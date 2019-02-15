#include "Rinex3EphemerisStore.hpp"
#include"Solution.h"
#include"Action.h"

#include <iostream>
#include<filesystem>

using namespace std;
using namespace gpstk;
using namespace pod;
namespace fs = std::experimental::filesystem;

void testRinNav(char* path)
{
    Rinex3EphemerisStore nrin;
    cout<< nrin.loadFile(path) << endl;
    auto &sid = SatID(1, SatID::SatelliteSystem::systemGPS);
    CommonTime t0 = nrin.getInitialTime(sid);
    CommonTime te = nrin.getFinalTime(sid);
    nrin.SearchNear();
    auto t = t0 + (te - t0) / 2;
    nrin.dump(cout);
   
   // nrin.getXvt(sid, t);
}

void testRinParse(char* path1)
{
	fs::path path("D:\\_temp\\RINEX\\N183012a__.19O");
    // Create the input file stream
	Rinex3ObsStream rin(path.string());
	
    // Create the output file stream
    ofstream rout(path.append("_"), ios::out);
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
   
    sol.saveStatistic();
    sol.saveToDb();

    cout << "inserting to db complete ";
    cout << (std::clock() - t2) / (double)CLOCKS_PER_SEC << endl;

    //ofstream f("dump.txt");
    //gMap.dump(f);
    //f.close();
}

int main(int argc, char* argv[])
{
	cout << "Build: \n" << __DATE__" " << __TIME__ << endl << endl;
    //cout << CivilTime(CommonTime::BEGINNING_OF_TIME) << endl;
    //Solution sol(argv[1]);
    //sol.chekObs();
    //codeSmoother(argv[1], atoi(argv[2]));
    //testRinParse(argv[1]);
    //SQLiteAdapter:: testSQLite(argv[1], argv[2]);
    //test();

    //testRinParse(argv[1]);
    cout << argv[1] << endl;

    testPod(argv[1]);
    //system("pause");
    return 0;
}
