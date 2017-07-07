#include "Solution.h"
#include "EarthRotation.h"
#include"OrbitSim.h"
using namespace POD;
int main(int argc, char *argv[])
{
    //POD:: Solution sol(argv[1]);
    //sol.process();


    OrbitSim::runTest();
   
    /*
    CivilTime ct(2012, 10, 10, 2, 3, 1, TimeSystem::GPS);
    CommonTime epoch = (CommonTime)ct;
    Matrix<double> C2E = ERP.getJ2k2ECEF(epoch);
    cout << C2E << endl;
    */

    return 0;
}