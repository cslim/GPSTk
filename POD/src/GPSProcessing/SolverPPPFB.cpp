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
//  Dagoberto Salazar - gAGE ( http://www.gage.es ). 2008, 2009, 2011
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
 * @file SolverPPPFB.cpp
 * Class to compute the PPP solution in forwards-backwards mode.
 */

#include "SolverPPPFB.hpp"
#include"LICSDetector2.hpp"
#include"MWCSDetector.hpp"
#include"SatArcMarker.hpp"

namespace pod
{

      // Returns a string identifying this object.
   std::string SolverPPPFB::getClassName() const
   { return "SolverPPPFB"; }


      /* Common constructor.
       *
       * @param useNEU   If true, will compute dLat, dLon, dH coordinates;
       *                 if false (the default), will compute dx, dy, dz.
       */
   SolverPPPFB::SolverPPPFB(
       bool isUseAdvClkModel,
       double  tropoQ,
       double posSigma,
       double clkSigma,
       double weightFactor
   )
       :SolverPPP(isUseAdvClkModel, tropoQ, posSigma, clkSigma, weightFactor), firstIteration(true)
   {

       // Initialize the counter of processed measurements
       processedMeasurements = 0;

       // Initialize the counter of rejected measurements
       rejectedMeasurements = 0;

       // Indicate the TypeID's that we want to keep
       keepTypeSet.insert(TypeID::wetMap);

       keepTypeSet.insert(TypeID::dx);
       keepTypeSet.insert(TypeID::dy);
       keepTypeSet.insert(TypeID::dz);


       keepTypeSet.insert(TypeID::cdt);
       keepTypeSet.insert(TypeID::recCdtdot);
       keepTypeSet.insert(TypeID::recISB_GLN);
       
       keepTypeSet.insert(TypeID::L1);
       keepTypeSet.insert(TypeID::L2);
       keepTypeSet.insert(TypeID::C1);
       keepTypeSet.insert(TypeID::P1);
       keepTypeSet.insert(TypeID::P2);

       keepTypeSet.insert(TypeID::prefitC);
       keepTypeSet.insert(TypeID::prefitL);
       keepTypeSet.insert(TypeID::weight);
       keepTypeSet.insert(TypeID::CSL1);
       keepTypeSet.insert(TypeID::CSL2);
       keepTypeSet.insert(TypeID::satArc);


   }  // End of 'SolverPPPFB::SolverPPPFB()'

      /* Returns a reference to a gnnsRinex object after solving
       * the previously defined equation system.
       *
       * @param gData     Data object holding the data.
       */
   IRinex& SolverPPPFB::Process(IRinex& gData)
      throw(ProcessingException)
   {

      try
      {

         SolverPPP::Process(gData);


            // Before returning, store the results for a future iteration
         if(firstIteration)
         {
               // Create a new gnssRinex structure with just the data we need
            //gnssRinex gBak(gData.extractTypeID(keepTypeSet));

               // Store observation data
            ObsData.push_back(gData.clone());

            // Update the number of processed measurements
            processedMeasurements += gData.getBody().numSats();

         }

         return gData;

      }
      catch(Exception& u)
      {
            // Throw an exception if something unexpected happens
         ProcessingException e( getClassName() + ":"
                                + u.what() );

         GPSTK_THROW(e);

      }

   }  // End of method 'SolverPPPFB::Process()'



      /* Reprocess the data stored during a previous 'Process()' call.
       *
       * @param cycles     Number of forward-backward cycles, 1 by default.
       *
       * \warning The minimum number of cycles allowed is "1". In fact, if
       * you introduce a smaller number, 'cycles' will be set to "1".
       */
   void SolverPPPFB::ReProcess(int cycles)
      throw(ProcessingException)
   {

         // Check number of cycles. The minimum allowed is "1".
      if (cycles < 1)
      {
         cycles = 1;
      }

         // This will prevent further storage of input data when calling
         // method 'Process()'
      firstIteration = false;

      try
      {

            // Backwards iteration. We must do this at least once
         for ( auto rpos = ObsData.rbegin(); rpos != ObsData.rend(); ++rpos)
         {

            SolverPPP::Process( **rpos );

         }

            // If 'cycles > 1', let's do the other iterations
         for (int i=0; i<(cycles-1); i++)
         {

               // Forwards iteration
            for (auto pos = ObsData.begin(); pos != ObsData.end(); ++pos)
            {
               SolverPPP::Process(**pos);
            }

               // Backwards iteration.
            for (auto rpos = ObsData.rbegin(); rpos != ObsData.rend(); ++rpos)
            {
               SolverPPP::Process( **rpos );
            }

         }  // End of 'for (int i=0; i<(cycles-1), i++)'


         return;

      }
      catch(Exception& u)
      {
            // Throw an exception if something unexpected happens
         ProcessingException e( getClassName() + ":" + u.what() );

         GPSTK_THROW(e);

      }

   }  // End of method 'SolverPPPFB::ReProcess()'


      /* Reprocess the data stored during a previous 'Process()' call.
       *
       * This method will reprocess data trimming satellites whose postfit
       * residual is bigger than the limits indicated by limitsCodeList and
       * limitsPhaseList.
       */
   void SolverPPPFB::ReProcess( void )
      throw(ProcessingException)
   {

         // Let's use a copy of the lists
      std::list<double> codeList( limitsCodeList );
      std::list<double> phaseList( limitsPhaseList );

         // Get maximum size
      size_t maxSize( codeList.size() );
      if( maxSize < phaseList.size() ) maxSize = phaseList.size();

         // This will prevent further storage of input data when calling
         // method 'Process()'
      firstIteration = false;

      try
      {

            // Backwards iteration. We must do this at least once
		  for (auto rpos = ObsData.rbegin(); rpos != ObsData.rend(); ++rpos)
			  SolverPPP::Process(**rpos);


            // If both sizes are '0', let's return
         if (maxSize == 0) return;

            // We will store the limits here. By default we use very big values
         double codeLimit( 1000000.0 );
         double phaseLimit( 1000000.0 );

            // If 'maxSize > 0', let's do the other iterations
         for (size_t i = 0; i < maxSize; i++)
         {

               // Update current limits, if available
            if( codeList.size() > 0 )
            {
                  // Get the first element from the list
               codeLimit = codeList.front();

                  // Delete the first element from the list
               codeList.pop_front();
            }

            if( phaseList.size() > 0 )
            {
                  // Get the first element from the list
               phaseLimit = phaseList.front();

                  // Delete the first element from the list
               phaseList.pop_front();
            }


               // Forwards iteration
            for (auto pos = ObsData.begin(); pos != ObsData.end(); ++pos)
            {
                  // Let's check limits
               checkLimits( **pos, codeLimit, phaseLimit );
               
               if ((*pos)->getBody().size() == 0)
               {
                   Exception e("Rejected all satellites at reprocessing part, check phase and code limits");
                   GPSTK_THROW(e);
               }
                   
                  // Process data
			   SolverPPP::Process(**pos);
            }

               // Backwards iteration.
            for (auto rpos = ObsData.rbegin(); rpos != ObsData.rend(); ++rpos)
            {
                  // Let's check limits
               checkLimits( **rpos, codeLimit, phaseLimit );

                  // Process data
			   SolverPPP::Process(**rpos);
            }

         }  // End of 'for (int i=0; i<(cycles-1), i++)'

         return;

      }
      catch(Exception& u)
      {
            // Throw an exception if something unexpected happens
         ProcessingException e( getClassName() + ":" + u.what() );

         GPSTK_THROW(e);

      }

   }  // End of method 'SolverPPPFB::ReProcess()'




      /* Process the data stored during a previous 'ReProcess()' call, one
       * item at a time, and always in forward mode.
       *
       * @param gData      Data object that will hold the resulting data.
       *
       * @return FALSE when all data is processed, TRUE otherwise.
       */
   bool SolverPPPFB::LastProcess(IRinex& gData)
      throw(ProcessingException)
   {
      try
      {
            // Keep processing while 'ObsData' is not empty
         if( !(ObsData.empty()) )
         {

               // Get the first data epoch in 'ObsData' and process it. The
               // result will be stored in 'gData'
			 gData = SolverPPP::Process(*ObsData.front());
            // gData = ObsData.front();
               // Remove the first data epoch in 'ObsData', freeing some
               // memory and preparing for next epoch
            ObsData.pop_front();

            // Update some inherited fields
            solution = SolverPPP::solution;
            covMatrix = SolverPPP::covMatrix;
            postfitResiduals = SolverPPP::postfitResiduals;

 /*           solution = sols.front(); 
            sols.pop_front();
            covMatrix =covs.front();
            covs.pop_front();
            postfitResiduals = ress.front();
            ress.pop_front();*/

               // If everything is fine so far, then results should be valid
            valid = true;

            return true;

         }
         else
         {
               // There are no more data
            return false;

         }  // End of 'if( !(ObsData.empty()) )'

      }
      catch(Exception& u)
      {
            // Throw an exception if something unexpected happens
         ProcessingException e( getClassName() + ":" + u.what() );

         GPSTK_THROW(e);

      }

   }  // End of method 'SolverPPPFB::LastProcess()'



      // This method checks the limits and modifies 'gData' accordingly.
   void SolverPPPFB::checkLimits( IRinex& gData,
                                  double codeLimit,
                                  double phaseLimit )
   {

         // Set to store rejected satellites
      SatIDSet satRejectedSet;

         // Let's check limits
      for( auto it = gData.getBody().begin(); it != gData.getBody().end(); ++it )
      {

            // Check postfit values and mark satellites as rejected
         if( std::abs((*it).second->get_value()( TypeID::postfitC )) > codeLimit )
         {
            satRejectedSet.insert( (*it).first );
         }

         if( std::abs((*it).second->get_value()( TypeID::postfitL )) > phaseLimit )
         {
            satRejectedSet.insert( (*it).first );
         }

      }  // End of 'for( satTypeValueMap::iterator it = gds.body.begin();...'


         // Update the number of rejected measurements
      rejectedMeasurements += satRejectedSet.size();

         // Remove satellites with missing data
      gData.getBody().removeSatID(satRejectedSet);

      return;

   }  // End of method 'SolverPPPFB::checkLimits()'


}  // End of namespace gpstk
