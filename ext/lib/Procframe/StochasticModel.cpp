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
//  Dagoberto Salazar - gAGE ( http://www.gage.es ). 2007, 2008
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
 * @file StochasticModel.cpp
 * Base class to define stochastic models, plus implementations
 * of common ones.
 */

#include "StochasticModel.hpp"


namespace gpstk
{


      // Get element of the process noise matrix Q
   double RandomWalkModel::getQ() const
   {

         // Compute current variance
      double variance(qprime*std::abs(currentTime - previousTime));

         // Return variance
      return variance;

   }  // End of method 'PhaseAmbiguityModel::getQ()'


      /* This method provides the stochastic model with all the available
       * information and takes appropriate actions.
       *
       * @param sat        Satellite.
       * @param gData      Data object holding the data.
       *
       */
   void RandomWalkModel::Prepare( const SatID& sat,
                                  IRinex& gData )
   {

         // Update previous epoch
      setPreviousTime(currentTime);

      setCurrentTime(gData.getHeader().epoch);

      return;

   }  // End of method 'RandomWalkModel::Prepare()'


      /* Set the value of process spectral density for ALL current sources.
       *
       * @param qp         Process spectral density: d(variance)/d(time) or
       *                   d(sigma*sigma)/d(time).
       *
       * \warning Beware of units: Process spectral density units are
       * sigma*sigma/time, while other models take plain sigma as input.
       * Sigma units are usually given in meters, but time units MUST BE
       * in SECONDS.
       *
       * \warning By default, process spectral density for zenital wet
       * tropospheric delay is set to 3e-8 m*m/s (equivalent to about
       * 1.0 cm*cm/h).
       *
       */
   TropoRandomWalkModel& TropoRandomWalkModel::setQprime(double qp)
   {

         // Look at each source being currently managed
      for( auto &&it :tmData )
         it.second.qprime = qp;

      return (*this);

   }  // End of method 'TropoRandomWalkModel::setQprime()'




      /* This method provides the stochastic model with all the available
       *  information and takes appropriate actions.
       *
       * @param sat        Satellite.
       * @param gData      Data object holding the data.
       *
       */
   void TropoRandomWalkModel::Prepare( const SatID& sat,
                                       IRinex& gData )
   {

         // First, get current source
      SourceID source( gData.getHeader().source );

         // Second, let's update current epoch for this source
      setCurrentTime(source, gData.getHeader().epoch );

         // Third, compute Q value
      computeQ(sat, gData.getBody(), source);

         // Fourth, prepare for next iteration updating previous epoch
      setPreviousTime(source, tmData[source].currentTime);

      return;

   }  // End of method 'TropoRandomWalkModel::Prepare()'



      /* This method computes the right variance value to be returned
       *  by method 'getQ()'.
       *
       * @param sat        Satellite.
       * @param data       Object holding the data.
       * @param source     Object holding the source of data.
       *
       */
   void TropoRandomWalkModel::computeQ( const SatID& sat,
                                        SatTypePtrMap& data,
                                        SourceID& source )
   {

         // Compute current variance
      variance = tmData[ source ].qprime
                 * std::abs( tmData[ source ].currentTime
                           - tmData[ source ].previousTime );

      return;

   }  // End of method 'TropoRandomWalkModel::computeQ()'



}  // End of namespace gpstk
