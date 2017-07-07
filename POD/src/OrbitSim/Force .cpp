
#include "Force.h"

namespace POD
{


         /// Default constructor
	Force:: Force()
      {         
         a.resize(3,0.0);
         da_dr.resize(3,3,0.0);
         da_dv.resize(3,3,0.0);
         da_dp.resize(3,0,0.0);      // default np = 0;

         da_dcd.resize(3,1,0.0);
         da_dcr.resize(3,1,0.0);
      }

         /// Default destructor
      	Force:: ~Force(){}


         /// this is the real one to do computation
       void Force:: doCompute(Epoch t, Spacecraft& sc)
      {
         a.resize(3,0.0);
         da_dr.resize(3,3,0.0);
         da_dv.resize(3,3,0.0);
         da_dp.resize(3,0,0.0);      // default np = 0;

         da_dcd.resize(3,1,0.0);
         da_dcr.resize(3,1,0.0);

      }
         
      
     
         /// get A Matrix
      Matrix<double> Force::getAMatrix() const
      {
            /* A Matrix
            |                        |
            | 0         I      0     |
            |                        |
         A =| da_dr    da_dv   da_dp |
            |                        |
            | 0         0      0     |
            |                        |
            */

         const int np = da_dp.cols();

         gpstk::Matrix<double> A(6+np,6+np,0.0);

         A(0,3) = 1.0;
         A(1,4) = 1.0;
         A(2,5) = 1.0;

         // da_dr
         A(3,0) = da_dr(0,0);
         A(3,1) = da_dr(0,1);
         A(3,2) = da_dr(0,2);
         A(4,0) = da_dr(1,0);
         A(4,1) = da_dr(1,1);
         A(4,2) = da_dr(1,2);
         A(5,0) = da_dr(2,0);
         A(5,1) = da_dr(2,1);
         A(5,2) = da_dr(2,2);

         // da_dv
         A(3,3) = da_dv(0,0);
         A(3,4) = da_dv(0,1);
         A(3,5) = da_dv(0,2);
         A(4,3) = da_dv(1,0);
         A(4,4) = da_dv(1,1);
         A(4,5) = da_dv(1,2);
         A(5,3) = da_dv(2,0);
         A(5,4) = da_dv(2,1);
         A(5,5) = da_dv(2,2);

         // da_dp
         for(int i=0;i<np;i++)
         {
            A(3,6+i) = da_dp(0,i);
            A(4,6+i) = da_dp(1,i);
            A(5,6+i) = da_dp(2,i);
         }

         return A;

      }  // End of method 'getAMatrix()'


} 


