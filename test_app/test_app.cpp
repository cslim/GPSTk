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

using namespace std;
using namespace gpstk;
using namespace POD;

int main(int argc, char* argv[])
{
    // Create the input file stream
    Rinex3ObsStream rin("drbn083f0.15o");

    // Create the output file stream
    ofstream rout("drbn083f_.15o", ios::out);
    gnssRinex  gRin;
    // Read the RINEX header
    Rinex3ObsHeader head;    //RINEX header object
    rin >> head;
   // rout.header = head;

   //   rout << head;

    // Loop over all data epochs
    Rinex3ObsData data;   //RINEX data object
    while (rin >> data)
    {
        
        for (auto &it : data.obs)
        {
            cout << it.first << endl;
            for (auto &it1 : it.second)
                cout << it1.asString();
            cout << endl;
        }
        /*for (auto &it : gRin.getTypeID())
        {
            cout << it << " ";
        }
        cout << endl;*/

        //gRin.body.dump(cout);
    }
    rout.close();
    return 0;
}
