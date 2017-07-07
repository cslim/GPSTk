#include"CentralMassModel.h"

using namespace std; 

namespace POD
{

    CentralMassModel::CentralMassModel(const GravityModelData &gData)
        :GravityModel(gData)
    {
        gmData_.modelName = "Earth as point mass";
    }

       /** Call the relevant methods to compute the acceleration.
       * @param utc Time reference class
       * @param rb  Reference body class
       * @param sc  Spacecraft parameters and state
       * @return the acceleration [m/s^s]
       */
    void CentralMassModel::doCompute(Epoch time,  Spacecraft& sc)
    {
        Vector<double> R = sc.R();
        double  r = sqrt(R(0)*R(0) + R(1)*R(1) + R(2)*R(2));
        double  r2 = r*r;
        double  r3 = r2*r;
        double  r5 = r2*r3;
         
        a = -gmData_.GM*R / r3;
        
        // da_dr
        double m = gmData_.GM / r5;
        da_dr(0, 0) = 3 * R[0] * R[0] - r2;	    da_dr(0, 1) = 3 * R[0] * R[1];			da_dr(0, 2) = 3 * R[0] * R[2];
        da_dr(1, 0) = 3 * R[0] * R[1];			da_dr(1, 1) = 3 * R[1] * R[1] - r2;	    da_dr(1, 2) = 3 * R[1] * R[2];
        da_dr(2, 0) = 3 * R[0] * R[2];			da_dr(2, 1) = 3 * R[2] * R[1];		    da_dr(2, 2) = 3 * R[2] * R[2] - r2;

        da_dr = m*da_dr;

        //da_dv
        da_dv.resize(3, 3, 0.0);

        //da_dp

    }

}