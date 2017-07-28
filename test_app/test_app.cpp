#include <iostream>
#include "SystemTime.hpp"
#include"CommonTime.hpp"
#include"CivilTime.hpp"
#include"OrbitSim.h"
#include"SphericalHarmonicsModel.h"
#include"Rinex3ObsStream.hpp"
#include"Rinex3ObsData.hpp"
#include"Exception.hpp"
#include"DataStructures.hpp"
#include"Solution.h"
#include"Action.h"
using namespace std;
using namespace gpstk;
using namespace pod;

void testgpstk()
{
    // Create the input file stream
    Rinex3ObsStream rin("cng2182a.17o");

    // Create the output file stream
    ofstream rout("cng2182a_.17o", ios::out);
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


int main(int argc, char* argv[])
{
    auto t = clock();
    Action a(argv[1]);
    auto gMap = a.process();
    auto t2 = clock();
    cout << "process complete ";
    cout << (std::clock() - t) / (double)CLOCKS_PER_SEC << endl;

    ofstream f("dump.txt");
    gMap.dump(f);

    f.close();
    return 0;
}
