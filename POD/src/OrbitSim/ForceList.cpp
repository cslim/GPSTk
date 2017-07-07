
#include "ForceList.h"

namespace POD
{

   ForceList::ForceList()
   {
     clear();
   }

      // interface implementation for the 'ForceModel'
   Vector<double> ForceList::getDerivatives(const Epoch & t,  Spacecraft& sc)
   {
	   const int np = 0; //= setFMT.size(); //getNP();

      a.resize(3,0.0);
      da_dr.resize(3,3,0.0);
      da_dv.resize(3,3,0.0);
      da_dp.resize(3,np,0.0);

      da_dcd.resize(3,1,0.0);
      da_dcr.resize(3,1,0.0);
      
	  for (auto & it : forceList)
      {
         it->doCompute(t,sc);

         a       += it->getAccel();
         da_dr   += it->partialR();
         da_dv   += it->partialV();
            //da_dp   += (*it)->partialP();

            //cout<<(*it)->modelName()<<endl;
         
         da_dcd   += it->partialCd();
         da_dcr   += it->partialCr();
         
      }
      
         // declare a counter
      int i = 0;  


         /* Transition Matrix (6+np)*(6+np)
            |                           |
            | dr_dr0   dr_dv0   dr_dp0  |
            |                           |
       phi= | dv_dr0   dv_dv0   dv_dp0  |
            |                           |
            | 0         0          I    |
            |                           |
         */
      Matrix<double> phi = sc.getTransitionMatrix();

         /* A Matrix (6+np)*(6+np)
            |                           |
            | 0         I      0        |
            |                           |
        A = | da_dr      da_dv   da_dp  |
            |                           |
            | 0         0      0        |
            |                           |
         */
      Matrix<double> A = getAMatrix();
      
         /* dphi Matrix
            |                          |
            | dv_dr0   dv_dv0   dv_dp0 |
            |                          |
     dphi = | da_dr0   da_dv0   da_dp0 |
            |                          |
            | 0         0         0    |
            |                          |

            da_dr0 = da_dr*dr_dr0 + da_dv*dv_dr0

            da_dv0 = da_dr*dr_dv0 + da_dv*dv_dv0

            da_dp0 = da_dr*dr_dp0 + da_dv*dv_dp0 + da_dp0;
         */
      Matrix<double> dphi = A * phi;


      Vector<double> r = sc.R();
      Vector<double> v = sc.V();
      
      Vector<double> dy(42+6*np,0.0);   //////////////////////////////////////////////////////////////////////////

      dy(0) = v(0);      // v
      dy(1) = v(1);
      dy(2) = v(2);
      dy(3) = a(0);      // a
      dy(4) = a(1);
      dy(5) = a(2);
      
      for(int i=0;i<3;i++)
      {
         for(int j=0;j<3;j++)
         {
            dy(6+i*3+j) = dphi(i,j);                // dv_dr0
            dy(15+i*3+j) = dphi(i,j+3);             // dv_dv0
            dy(24+3*np+i*3+j) = dphi(i+3,j);        // da_dr0
            dy(33+3*np+i*3+j) = dphi(i+3,j+3);      // da_dv0   
         }
         for(int k=0;k<np;k++)
         {
            dy(24+i*np+k) = dphi(i,i*np+k);         // dv_dp0
            dy(42+3*np+i*np+k) = dphi(i+3,i*np+k);  // da_dp0
         }
      }
      return dy;

   }  // End of method 'ForceList::getDerivatives()'


   void ForceList::printForce(std::ostream& s)
   {
         // a counter
      int i(1);

      for(auto &  it : forceList)
      {
         s << std::setw(3) << i << " "
           << it->forceIndex() << " "
           << it->modelName() << std::endl;

         i++;
      }

   }  // End of method 'ForceList::printForceModel()'
   void ForceList::clear()
   {
	   forceList.clear();
   }
}  // End of namespace 'POD'
