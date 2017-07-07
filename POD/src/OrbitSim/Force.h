//============================================================================
//
//  This file is part of GPSTk, the GPS Toolkit.
//
//  The GPSTk is free software; you can redistribute it and/or modify
//  it under the terms of the GNU Lesser General Public License as published
//  by the Free Software Foundation; either version 3.0 of the License, or
//  any later version.
//
//  The GPSTk is distributed in the hope that it will be useful,
//  but WITHOUT ANY WARRANTY; without even the implied warranty of
//  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//  GNU Lesser General Public License for more details.
//
//  You should have received a copy of the GNU Lesser General Public
//  License along with GPSTk; if not, write to the Free Software Foundation,
//  Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110, USA
//  
//  Copyright 2004, The University of Texas at Austin
//  Wei Yan - Chinese Academy of Sciences . 2009, 2010
//
//============================================================================

//============================================================================
//
//This software developed by Applied Research Laboratories at the University of
//Texas at Austin, under contract to an agency or agencies within the U.S. 
//Department of Defense. The U.S. Government retains all rights to use,
//duplicate, distribute, disclose, or release this software. 
//
//Pursuant to DoD Directive 523024 
//
// DISTRIBUTION STATEMENT A: This software has been approved for public 
//                           release, distribution is unlimited.
//
//=============================================================================

/**
* @file Force.hpp
* Force Model is a simple interface which allows uniformity among the various force
* models 
*/

#ifndef POD_FORCE_H
#define POD_FORCE_H

#include "Vector.hpp"
#include "Matrix.hpp"
#include"Epoch.hpp"
#include"Spacecraft.hpp"

using namespace gpstk;

namespace POD
{
      /// @ingroup GeoDynamics 
      //@{

      /**
       * Force Model is a simple interface which allows uniformity among the various force
       * models 
       */
   class Force
   {

   public:

         /// To identify every force model and make sure one type of
         /// force exist only one instance in the force model list.
         /// class indexed with FMI_BASE and FMI_LIST can't be added
         /// to the force model list
      enum ForceIndex
      {
         FMI_BASE       = 1000,  ///< For This class 'Force'

         FMI_GEOEARTH,           ///< Geopotential of Earth
         FMI_GEOSUN,             ///< Geopotential of Sun
         FMI_GEOMOON,            ///< Geopotential of Moon 
         FMI_DRAG,               ///< Atmospheric Drag
         FMI_SRP,                ///< Solar Radiation Pressure
         FMI_RELATIVE,           ///< Relative Effect   
         FMT_EMPIRICAL,          ///< Empirical Force 
         
         //... add more here

         FMI_LIST   = 2000      ///< For class 'ForceList'
      };

      enum ForceType
      {
         Cd,               // Coefficient of drag
         Cr               // Coefficient of Reflectivity
      };

         /// Default constructor
	  Force();
     

         /// Default destructor
	  virtual ~Force();

         /// this is the real one to do computation
	  virtual void doCompute(gpstk:: Epoch t, Spacecraft& sc);
  
         
         /// return the force model name
      virtual std::string modelName() const
      { return "Force"; }


         /// return the force model index
      virtual int forceIndex() const
      { return FMI_BASE; }


         /**
          * Return the acceleration
          * @return  acceleration
          */
      virtual Vector<double> getAccel() const
      { return a; }

         /**
          * Return the partial derivative of acceleration wrt position
          * @return Matrix containing the partial derivative of acceleration wrt velocity
          */
      virtual Matrix<double> partialR() const
      { return da_dr; }

         /**
          * Return the partial derivative of acceleration wrt velocity
          * @return Matrix containing the partial derivative of acceleration wrt velocity
          */
      virtual Matrix<double> partialV() const
      { return da_dv; }

         /**
          * Return the partial derivative of acceleration wrt velocity
          * @return Matrix containing the partial derivative of acceleration wrt velocity
          */
      virtual Matrix<double> partialP() const
      { return da_dp; }

         /**
          * Return the partial derivative of acceleration wrt velocity
          * @return Matrix containing the partial derivative of acceleration wrt cd
          */
      virtual Matrix<double> partialCd() const
      { return da_dcd; } 

         /**
          * Return the partial derivative of acceleration wrt velocity
          * @return Matrix containing the partial derivative of acceleration wrt cr
          */
      virtual Matrix<double> partialCr() const
      { return da_dcr; }

         /** return number of np
          */
      int getNP() const
      { return da_dp.cols(); }

         /// get A Matrix
	  Matrix<double> getAMatrix() const;
      

   protected:

         /// Acceleration
      Vector<double> a;         // 3
      
         /// Partial derivative of acceleration wrt position
      Matrix<double> da_dr;      // 3*3
      
         /// Partial derivative of acceleration wrt velocity
      Matrix<double> da_dv;      // 3*3
      
         /// Partial derivative of acceleration wrt dynamic parameters
      Matrix<double> da_dp;      // 3*np
         
         /// Partial derivative of acceleration wrt Cd
      Matrix<double> da_dcd;      // 3*1
         
         /// Partial derivative of acceleration wrt Cr
      Matrix<double> da_dcr;      // 3*1

      

   }; // End of 'class Force'

      /**
       * Stream output for CommonTime objects.  Typically used for debugging.
       * @param s stream to append formatted CommonTime to.
       * @param t CommonTime to append to stream \c s.
       * @return reference to \c s.
       */
   inline std::ostream& operator<<( std::ostream& s,
                                    const POD::Force& fm )
   {
      Vector<double> a = fm.getAccel();
      Matrix<double> da_dr = fm.partialR();
      Matrix<double> da_dv = fm.partialV();
      Matrix<double> da_dp = fm.partialP();

      s << "a [" << a.size() << "]\n{\n"
          << a << std::endl << "}\n\n";

      s << "da/dr [" << da_dr.rows() << "," << da_dr.cols() << "]\n{\n"
          << da_dr << std::endl << "}\n\n";

      s << "da/dv [" << da_dv.rows() << "," << da_dv.cols() << "]\n{\n"
          << da_dv << std::endl << "}\n\n";

      s << "da/dp [" << da_dp.rows() << "," << da_dp.cols() << "]\n{\n"
          << da_dp << std::endl << "}\n\n";

      Matrix<double> A = fm.getAMatrix();

      s << "A = [" << A.rows() << "," << A.cols() << "]\n{\n"
          << A << std::endl << "}\n\n";

      return s;

   }

      // @}

}  // End of namespace 'gpstk'


#endif  // POD_FORCE_MODEL_H
