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
//  Dagoberto Salazar - gAGE ( http://www.gage.es ). 2007, 2008, 2009, 2011
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
 * @file BasicModel.cpp
 * This is a class to compute the basic parts of a GNSS model, i.e.:
 * Geometric distance, relativity correction, satellite position and
 * velocity at transmission time, satellite elevation and azimuth, etc.
 */

#include "BasicModel.hpp"

namespace gpstk
{


      // Returns a string identifying this object.
   std::string BasicModel::getClassName() const
   { return "BasicModel"; }



      /* Explicit constructor taking as input reference
       * station coordinates.
       *
       * Those coordinates may be Cartesian (X, Y, Z in meters) or Geodetic
       * (Latitude, Longitude, Altitude), but defaults to Cartesian.
       *
       * Also, a pointer to GeoidModel may be specified, but default is
       * NULL (in which case WGS84 values will be used).
       *
       * @param aRx   first coordinate [ X(m), or latitude (degrees N) ]
       * @param bRx   second coordinate [ Y(m), or longitude (degrees E) ]
       * @param cRx   third coordinate [ Z, height above ellipsoid or
       *              radius, in meters ]
       * @param s     coordinate system (default is Cartesian, may be set
       *              to Geodetic).
       * @param ell   pointer to EllipsoidModel
       * @param frame Reference frame associated with this position
       */
   BasicModel::BasicModel( const double& aRx,
                           const double& bRx,
                           const double& cRx,
                           Position::CoordinateSystem s,
                           EllipsoidModel *ell,
                           ReferenceFrame frame )
       : minElev(10.0), defaultObservable(TypeID::C1), useTGD(false), addTGD(false),
       useCdtDot(false), isFirstTime(false), currTime(CommonTime::END_OF_TIME),
       prevTime(CommonTime::BEGINNING_OF_TIME), defInterval(30)
   {

      pDefaultEphemeris = NULL;
      setInitialRxPosition( aRx, bRx, cRx, s, ell, frame );

   }  // End of 'BasicModel::BasicModel()'


      // Explicit constructor, taking as input a Position object
      // containing reference station coordinates.
   BasicModel::BasicModel(const Position& RxCoordinates)
       : minElev(10.0), defaultObservable(TypeID::C1), useTGD(false), addTGD(false),
       useCdtDot(false), isFirstTime(false), currTime(CommonTime::END_OF_TIME),
       prevTime(CommonTime::BEGINNING_OF_TIME), defInterval(30)
   {

      pDefaultEphemeris = NULL;
          
      setInitialRxPosition(RxCoordinates);

   }  // End of 'BasicModel::BasicModel()'



      /* Explicit constructor, taking as input reference station
       * coordinates, ephemeris to be used, default observable
       * and whether TGD will be computed or not.
       *
       * @param RxCoordinates Reference station coordinates.
       * @param dEphemeris    EphemerisStore object to be used by default.
       * @param dObservable   Observable type to be used by default.
       * @param applyTGD      Whether or not C1 observable will be
       *                      corrected from TGD effect or not.
       *
       */
   BasicModel::BasicModel( const Position& RxCoordinates,
                           XvtStore<SatID>& dEphemeris,
                           const TypeID& dObservable,
                           const bool& applyTGD,
                           const bool& isaddTGD)
       : minElev(10.0), defaultObservable(dObservable), useTGD(applyTGD), addTGD(isaddTGD),
       useCdtDot(false), isFirstTime(false), currTime(CommonTime::END_OF_TIME),
       prevTime(CommonTime::BEGINNING_OF_TIME), defInterval(30)
   {

      setInitialRxPosition(RxCoordinates);
      setDefaultEphemeris(dEphemeris);
    
   }  // End of 'BasicModel::BasicModel()'



      /* Returns a satTypeValueMap object, adding the new data generated when
       * calling a modeling object.
       *
       * @param time      Epoch.
       * @param gData     Data object holding the data.
       */
   SatTypePtrMap& BasicModel::Process( const CommonTime& time,
                                         SatTypePtrMap& gData )
      throw(ProcessingException)
   {
       
      
      try
      {
          prevTime = currTime;
          currTime = time;
          double dt = isFirstTime ? defInterval : std::abs(currTime - prevTime);
         
          SatIDSet satRejectedSet;
          int numGLN(0);

            // Loop through all the satellites
         for(auto stv = gData.begin(); stv != gData.end(); ++stv )
         {
               // Scalar to hold temporal value
            double observable( (*stv).second->get_value()(defaultObservable) );

               // A lot of the work is done by a CorrectedEphemerisRange object
            CorrectedEphemerisRange cerange;

            try
            {
                  // Compute most of the parameters
               cerange.ComputeAtTransmitTime( time,
                                              observable,
                                              rxPos,
                                              (*stv).first,
                                              *(getDefaultEphemeris()) );
            }
            catch(InvalidRequest& e)
            {
#if _DEBUG
				//std::cout << e << std::endl;
#endif // DEBUG

                  // If some problem appears, then schedule this satellite
                  // for removal
               satRejectedSet.insert( (*stv).first );

               continue;    // Skip this SV if problems arise

            }
            double el = rxPos.elevationGeodetic(cerange.svPosVel);
               // Let's test if satellite has enough elevation over horizon
            if (el < minElev )
            {

                  // Mark this satellite if it doesn't have enough elevation
               satRejectedSet.insert( (*stv).first );

               continue;

            }

           

               // Now we have to add the new values to the data structure
            (*stv).second->get_value()[TypeID::dtSat] = cerange.svclkbias;

               // Now, lets insert the geometry matrix
            (*stv).second->get_value()[TypeID::dx] = cerange.cosines[0];
            (*stv).second->get_value()[TypeID::dy] = cerange.cosines[1];
            (*stv).second->get_value()[TypeID::dz] = cerange.cosines[2];

            (*stv).second->get_value()[TypeID::dSatX] = -cerange.cosines[0];
            (*stv).second->get_value()[TypeID::dSatY] = -cerange.cosines[1];
            (*stv).second->get_value()[TypeID::dSatZ] = -cerange.cosines[2];

               // When using pseudorange method, this is 1.0
            (*stv).second->get_value()[TypeID::cdt] = 1.0;

            //Glonass-spacific bias(ISB)
            double cdtGLO (0.0);

            if (stv->first.system == SatID::SatelliteSystem::systemGlonass)
            {
                cdtGLO = 1;
                numGLN++;
            }
            if (useCdtDot)
                (*stv).second->get_value()[TypeID::recCdtdot] = dt;

            (*stv).second->get_value()[TypeID::recISB_GLN] = cdtGLO;
               // Now we have to add the new values to the data structure
            (*stv).second->get_value()[TypeID::rho] = cerange.rawrange;
            (*stv).second->get_value()[TypeID::rel] = -cerange.relativity;
            (*stv).second->get_value()[TypeID::elevation] = cerange.elevationGeodetic;
            (*stv).second->get_value()[TypeID::azimuth] = cerange.azimuthGeodetic;

               // Let's insert satellite position at transmission time
            (*stv).second->get_value()[TypeID::satX] = cerange.svPosVel.x[0];
            (*stv).second->get_value()[TypeID::satY] = cerange.svPosVel.x[1];
            (*stv).second->get_value()[TypeID::satZ] = cerange.svPosVel.x[2];

               // Let's insert satellite velocity at transmission time
            (*stv).second->get_value()[TypeID::satVX] = cerange.svPosVel.v[0];
            (*stv).second->get_value()[TypeID::satVY] = cerange.svPosVel.v[1];
            (*stv).second->get_value()[TypeID::satVZ] = cerange.svPosVel.v[2];

               // Let's insert receiver position 
            (*stv).second->get_value()[TypeID::recX] = rxPos.X();
            (*stv).second->get_value()[TypeID::recY] = rxPos.Y();
            (*stv).second->get_value()[TypeID::recZ] = rxPos.Z();

               // Let's insert receiver velocity 
            (*stv).second->get_value()[TypeID::recVX] = 0.0;
            (*stv).second->get_value()[TypeID::recVY] = 0.0;
            (*stv).second->get_value()[TypeID::recVZ] = 0.0;

            if (addTGD)
            {
                // Computing Total Group Delay (TGD - meters), if possible
                double tempTGD = getTGDCorrections(time, (*pDefaultEphemeris), (*stv).first);

                // Apply correction to C1 observable, if appropriate
                if (useTGD)
                {
                    // Look for C1
                    if ((*stv).second->get_value().find(TypeID::C1) != (*stv).second->get_value().end())
                    {
                        (*stv).second->get_value()[TypeID::C1] =
                            (*stv).second->get_value()[TypeID::C1] - tempTGD;
                    };
                };

                (*stv).second->get_value()[TypeID::instC1] = tempTGD;
            }
         } // End of loop for(stv = gData.begin()...

            // Remove satellites with missing data
         gData.removeSatID(satRejectedSet);
       
         if (numGLN < 2)
         {
             gData.removeSatSyst(SatID::SatelliteSystem::systemGlonass);
             gData.removeTypeID(TypeID::recISB_GLN);
         }

         isFirstTime = false;
         rejectedSatsTable[time] = satRejectedSet;
         return gData;

      }   // End of try...
      catch(Exception& u)
      {
            // Throw an exception if something unexpected happens
         ProcessingException e( getClassName() + ":" + u.what() );

         GPSTK_THROW(e);

      }

   }  // End of method 'BasicModel::Process()'



      /* Method to set the initial (a priori) position of receiver.
       * @return
       *  0 if OK
       *  -1 if problems arose
       */
   int BasicModel::setInitialRxPosition( const double& aRx,
                                         const double& bRx,
                                         const double& cRx,
                                         Position::CoordinateSystem s,
                                         EllipsoidModel *ell,
                                         ReferenceFrame frame )
   {

      try
      {
         Position rxpos( aRx, bRx, cRx, s, ell, frame );
         setInitialRxPosition(rxpos);
         return 0;
      }
      catch(GeometryException& e)
      {
         return -1;
      }

   }  // End of method 'BasicModel::setInitialRxPosition()'



      // Method to set the initial (a priori) position of receiver.
   int BasicModel::setInitialRxPosition(const Position& RxCoordinates)
   {

      try
      {
         rxPos = RxCoordinates;
         return 0;
      }
      catch(GeometryException& e)
      {
         return -1;
      }

   }  // End of method 'BasicModel::setInitialRxPosition()'



      // Method to set the initial (a priori) position of receiver.
   int BasicModel::setInitialRxPosition()
   {
      try
      {
         Position rxpos(0.0, 0.0, 0.0, Position::Cartesian, NULL);
         setInitialRxPosition(rxpos);
         return 0;
      }
      catch(GeometryException& e)
      {
         return -1;
      }

   }  // End of method 'BasicModel::setInitialRxPosition()'



      // Method to get TGD corrections.
   double BasicModel::getTGDCorrections( CommonTime Tr,
                                         const XvtStore<SatID>& Eph,
                                         SatID sat )
      throw()
   {

      try
      {
         const GPSEphemerisStore& bce =
                                 dynamic_cast<const GPSEphemerisStore&>(Eph);

         bce.findEphemeris(sat,Tr);

         //return ( bce.findEphemeris(sat,Tr).getTgd() * C_MPS );
         return ( bce.findEphemeris(sat,Tr).Tgd * C_MPS );
      }
      catch(...)
      {
         return 0.0;
      }

   }  // End of method 'BasicModel::getTGDCorrections()'


}  // End of namespace gpstk
