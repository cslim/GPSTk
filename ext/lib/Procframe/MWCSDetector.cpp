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
//  Dagoberto Salazar - gAGE ( http://www.gage.es ). 2007, 2008, 2011
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
 * @file MWCSDetector.cpp
 * This is a class to detect cycle slips using the Melbourne-Wubbena
 * combination.
 */

#include "MWCSDetector.hpp"


namespace gpstk
{


      // Returns a string identifying this object.
   std::string MWCSDetector::getClassName() const
   { return "MWCSDetector"; }



      /* Common constructor
       *
       * @param mLambdas      Maximum deviation allowed before declaring
       *                      cycle slip (in number of Melbourne-Wubbena
       *                      wavelenghts).
       * @param dtMax         Maximum interval of time allowed between two
       *                      successive epochs, in seconds.
       */
   MWCSDetector::MWCSDetector( double mLambdas,
                               double dtMax,
                               bool use )
      : CycleSlipDetector(TypeID::MWubbena, dtMax, use)
   {
      setMaxNumLambdas(mLambdas);
   }


      /* Method to set the maximum deviation allowed before declaring
       * cycle slip (in number of Melbourne-Wubbena wavelenghts).
       *
       * @param mLambdas     Maximum deviation allowed before declaring
       *                     cycle slip (in number of Melbourne-Wubbena
       *                     wavelenghts).
       */
   MWCSDetector& MWCSDetector::setMaxNumLambdas(const double& mLambdas)
   {

         // Don't allow number of lambdas less than or equal to 0
      if (mLambdas > 0.0)
      {
         maxNumLambdas = mLambdas;
      }
      else
      {
         maxNumLambdas = 10.0;
      }

      return (*this);

   }  // End of method 'MWCSDetector::setMaxNumLambdas()'



    



      /* Method that implements the Melbourne-Wubbena cycle slip
       *  detection algorithm
       *
       * @param epoch     Time of observations.
       * @param sat       SatID.
       * @param tvMap     Data structure of TypeID and values.
       * @param epochflag Epoch flag.
       * @param mw        Current MW observation value.
       * @param lli1      LLI1 index.
       * @param lli2      LLI2 index.
       */
   CycleSlipDetector::DetectionResult MWCSDetector::
	   getDetection( const CommonTime& epoch,
                                      const SatID& sat,
                                      typeValueMap& tvMap,
                                      const short& epochflag,
                                      const double& mw,
                                      const double& lli1,
                                      const double& lli2 )
   {

	   DetectionResult reportCS = DetectionResult::NotDetected;

         // Difference between current and former epochs, in sec
      double currentDeltaT(0.0);

         // Difference between current and former MW values
      double currentBias(0.0);

         // Limit to declare cycle slip based on lambdas (LambdaLW = 0.862 m)
      double lambdaLimit(maxNumLambdas*0.862);

      double tempLLI1(0.0);
      double tempLLI2(0.0);


         // Get the difference between current epoch and former epoch,
         // in seconds
      currentDeltaT = ( epoch - MWData[sat].formerEpoch );

         // Store current epoch as former epoch
      MWData[sat].formerEpoch = epoch;

         // Difference between current value of MW and average value
      currentBias = std::abs(mw - MWData[sat].meanMW);

         // Increment window size
      ++MWData[sat].windowSize;

         // Check if receiver already declared cycle slip or if too much time
         // has elapsed
         // Note: If tvMap(lliType1) or tvMap(lliType2) don't exist, then 0
         // will be used and those tests will pass
      if ( (tvMap(lliType1)==1.0) ||
           (tvMap(lliType1)==3.0) ||
           (tvMap(lliType1)==5.0) ||
           (tvMap(lliType1)==7.0) )
      {
         tempLLI1 = 1.0;
      }

      if ( (tvMap(lliType2)==1.0) ||
           (tvMap(lliType2)==3.0) ||
           (tvMap(lliType2)==5.0) ||
           (tvMap(lliType2)==7.0) )
      {
         tempLLI2 = 1.0;
      }

      if ( (epochflag==1)  ||
           (epochflag==6)  ||
           (tempLLI1==1.0) ||
           (tempLLI2==1.0) ||
           (currentDeltaT > deltaTMax && isReprocess) )
      {

            // We reset the filter with this
         MWData[sat].windowSize = 1;

         reportCS = DetectionResult::CsDetected;                // Report cycle slip
      }


      if (MWData[sat].windowSize > 1)
      {

            // Test for current bias bigger than lambda limit and for
            // current bias squared bigger than sigma squared limit
         if ( (currentBias > lambdaLimit) )
         {

               // We reset the filter with this
            MWData[sat].windowSize = 1;

            reportCS = DetectionResult::CsDetected; // Report cycle slip

         }
      }

         // Let's prepare for the next time
         // If a cycle-slip happened or just starting up
      if (MWData[sat].windowSize < 2)
      {
         MWData[sat].meanMW = mw;
      }
      else
      {
            // Compute average
         MWData[sat].meanMW += (mw - MWData[sat].meanMW) /
                               (static_cast<double>(MWData[sat].windowSize));
      }

	  return reportCS;

   }  // End of method 'MWCSDetector::getDetection()'


}  // End of namespace gpstk
