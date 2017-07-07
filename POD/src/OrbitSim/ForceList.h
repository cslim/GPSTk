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
* @file ForceList.hpp
* ForceList is a countainer for force models.
*/

#ifndef POD_FORCE_MODEL_LIST_H
#define POD_FORCE_MODEL_LIST_H


#include"Vector.hpp"
#include"Matrix.hpp"
#include "Force.h"
#include"Spacecraft.hpp"

#include <list>
#include <memory>

namespace POD
{
    typedef std::unique_ptr<Force> ForceUniquePtr;
      /// @ingroup GeoDynamics 
      //@{
      
      /**
       * ForceList is a countainer for force models. It allows the simulation to encapsulate
       * a number of different force models and obtain their combined effect on a spacecraft. 
       */
   class ForceList : public Force
   {
   public:

         /// Default constructor
      ForceList();

         /// Default destructor
      virtual ~ForceList() {};

         /**
         * Adds a generic force to the list
         * @param f Object which implements the Force interface
         */
      void addForce(ForceUniquePtr pForce)
      { forceList.push_back(std::move(pForce)); };


         /// interface implementation for the 'Force'
      virtual Vector<double> getDerivatives(const Epoch &t, Spacecraft& sc);
      

         /// return the force model name
      virtual std::string modelName() const
      { return "ForceList"; };


         /// return the force model index
      virtual int forceIndex() const
      { return FMI_LIST; }

         /// Show the exist force model
      void printForce(std::ostream& s);

	  void clear();
   protected:

         /// List of forces

      std::list<ForceUniquePtr> forceList;


   }; // End of class 'ForceList'

      // @}

}  // End of namespace 'gpstk'

#endif   // POD_FORCE_MODEL_LIST_H
