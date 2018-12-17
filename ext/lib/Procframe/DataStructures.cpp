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
 * @file DataStructures.cpp
 * Set of several data structures to be used by other GPSTk classes.
 */

#include "DataStructures.hpp"

using namespace gpstk::StringUtils;
using namespace std;


namespace gpstk
{
      ////// gnssSatValue //////


      // Returns a gnssSatValue with only this satellite.
      // @param satellite Satellite to be extracted.
   gnssSatValue gnssSatValue::extractSatID(const SatID& satellite) const
   {

      gnssSatValue result;
      result.header = (*this).header;
      result.body = (*this).body.extractSatID(satellite);

      return result;

   }  // End of method 'gnssSatValue::extractSatID()'



      // Returns a gnssSatValue with only one satellite, identified by
      // the given parameters.
      // @param p Satellite PRN number.
      // @param p System the satellite belongs to.
   gnssSatValue gnssSatValue::extractSatID( const int& p,
                                       const SatID::SatelliteSystem& s ) const
   {

      SatID tempSatellite(p, s);  // We build a temporary SatID object

      return extractSatID(tempSatellite);

   }  // End of method 'gnssSatValue::extractSatID()'



      // Returns a gnssSatValue with only these satellites.
      // @param satSet Set (SatIDSet) containing the satellites
      //               to be extracted.
   gnssSatValue gnssSatValue::extractSatID(const SatIDSet& satSet) const
   {

      gnssSatValue result;
      result.header = (*this).header;
      result.body = (*this).body.extractSatID(satSet);

      return result;

   }  // End of method 'gnssSatValue::extractSatID()'



      // Modifies this object, keeping only this satellite.
      // @param satellite Satellite to be kept.
   gnssSatValue& gnssSatValue::keepOnlySatID(const SatID& satellite)
   {

      SatIDSet satSet;
      satSet.insert(satellite);

      return keepOnlySatID(satSet);

   }  // End of method 'gnssSatValue::keepOnlySatID()'



      // Modifies this object, keeping only this satellite.
      // @param p Satellite PRN number.
      // @param p System the satellite belongs to.
   gnssSatValue& gnssSatValue::keepOnlySatID( const int& p,
                                              const SatID::SatelliteSystem& s )
   {

      SatID tempSatellite(p, s);  // We build a temporary SatID object

      return keepOnlySatID(tempSatellite);

   }  // End of method 'gnssSatValue::keepOnlySatID()'



      // Modifies this object, keeping only these satellites.
      // @param satSet Set (SatIDSet) containing the satellites to be kept.
   gnssSatValue& gnssSatValue::keepOnlySatID(const SatIDSet& satSet)
   {

      satValueMap svMap ( (*this).body.extractSatID(satSet) );
      (*this).body = svMap;

      return (*this);

   }  // End of method 'gnssSatValue::keepOnlySatID()'



      // Modifies this object, removing these satellites.
      // @param satSet Set (SatIDSet) containing the satellites
      //               to be removed.
   gnssSatValue& gnssSatValue::removeSatID(const SatIDSet& satSet)
   {

      for( SatIDSet::const_iterator pos = satSet.begin();
           pos != satSet.end();
           ++pos )
      {
         (*this).body.erase(*pos);
      }

         return (*this);

   }  // End of method 'gnssSatValue::removeSatID()'




      ////// gnssTypeValue //////


      // Returns a gnssTypeValue with only this type of data.
      // @param type Type of value to be extracted.
   gnssTypeValue gnssTypeValue::extractTypeID(const TypeID& type) const
   {

      gnssTypeValue result;
      result.header = (*this).header;
      result.body = (*this).body.extractTypeID(type);

      return result;

   }  // End of method 'gnssTypeValue::extractTypeID()'



      // Returns a gnssTypeValue with only these types of data.
      // @param typeSet Set (TypeIDSet) containing the types of data
      //                to be extracted.
   gnssTypeValue gnssTypeValue::extractTypeID(const TypeIDSet& typeSet) const
   {

      gnssTypeValue result;
      result.header = (*this).header;
      result.body = (*this).body.extractTypeID(typeSet);

      return result;

   }  // End of method 'gnssTypeValue::extractTypeID()'



      // Modifies this object, keeping only this type of data.
      // @param type Type of value to be kept.
   gnssTypeValue& gnssTypeValue::keepOnlyTypeID(const TypeID& type)
   {

      TypeIDSet typeSet;
      typeSet.insert(type);

      return keepOnlyTypeID(typeSet);

   }  // End of method 'gnssTypeValue::keepOnlyTypeID()'



      // Modifies this object, keeping only these types of data.
      // @param typeSet Set (TypeIDSet) containing the types of data
      //                to be kept.
   gnssTypeValue& gnssTypeValue::keepOnlyTypeID(const TypeIDSet& typeSet)
   {

      typeValueMap tvMap( (*this).body.extractTypeID(typeSet) );
      (*this).body = tvMap;

      return (*this);

   }  // End of method 'gnssTypeValue::keepOnlyTypeID()'



      // Modifies this object, removing these types of data.
      // @param typeSet Set (TypeIDSet) containing the types of data
      //                to be kept.
   gnssTypeValue& gnssTypeValue::removeTypeID(const TypeIDSet& typeSet)
   {

      for( TypeIDSet::const_iterator pos = typeSet.begin();
           pos != typeSet.end();
           ++pos )
      {
         (*this).body.erase(*pos);
      }

      return (*this);

   }  // End of method 'gnssTypeValue::removeTypeID()'




      ////// gnssSatTypeValue //////

      // Returns a getSatSystems with only this satellite.
      // @param satellite Satellite to be extracted.
   SatSystSet gnssSatTypeValue::getSatSystems() const
   {
       typedef decltype(body)::value_type elm;
       SatSystSet res;
       for_each(body.cbegin(), body.cend(), [&res](const elm& e) { res.insert(e.first.system); });
       return res;
   }

      // Returns a gnssSatTypeValue with only this satellite.
      // @param satellite Satellite to be extracted.
   gnssSatTypeValue gnssSatTypeValue::extractSatID(const SatID& satellite)
      const
   {
      gnssSatTypeValue result;
      result.header = (*this).header;
      result.body = (*this).body.extractSatID(satellite);

      return result;
   }  // End of method 'gnssSatTypeValue::extractSatID()'



      // Returns a gnssSatTypeValue with only one satellite, identified
      // by the given parameters.
      // @param p Satellite PRN number.
      // @param p System the satellite belongs to.
   gnssSatTypeValue gnssSatTypeValue::extractSatID( const int& p,
                                       const SatID::SatelliteSystem& s ) const
   {

      SatID tempSatellite(p, s);  // We build a temporary SatID object

      return extractSatID(tempSatellite);

   }  // End of method 'gnssSatTypeValue::extractSatID()'



      // Returns a gnssSatTypeValue with only these satellites.
      // @param satSet Set (SatIDSet) containing the satellites
      //               to be extracted.
   gnssSatTypeValue gnssSatTypeValue::extractSatID(const SatIDSet& satSet) const
   {

      gnssSatTypeValue result;
      result.header = (*this).header;
      result.body = (*this).body.extractSatID(satSet);

      return result;

   }  // End of method 'gnssSatTypeValue::extractSatID()'



      // Modifies this object, keeping only this satellite.
      // @param satellite Satellite to be kept.
   gnssSatTypeValue& gnssSatTypeValue::keepOnlySatID(const SatID& satellite)
   {

      SatIDSet satSet;
      satSet.insert(satellite);

      return keepOnlySatID(satSet);

   }  // End of method 'gnssSatTypeValue::keepOnlySatID()'



      // Modifies this object, keeping only this satellite.
      // @param p Satellite PRN number.
      // @param p System the satellite belongs to.
   gnssSatTypeValue& gnssSatTypeValue::keepOnlySatID( const int& p,
                                             const SatID::SatelliteSystem& s )
   {

      SatID tempSatellite(p, s);  // We build a temporary SatID object

      return keepOnlySatID(tempSatellite);

   }  // End of method 'gnssSatTypeValue::keepOnlySatID()'



      // Modifies this object, keeping only these satellites.
      // @param satSet Set (SatIDSet) containing the satellites to be kept.
   gnssSatTypeValue& gnssSatTypeValue::keepOnlySatID(const SatIDSet& satSet)
   {

      satTypeValueMap stvMap( (*this).body.extractSatID(satSet) );
      (*this).body = stvMap;

      return (*this);

   }  // End of method 'gnssSatTypeValue::keepOnlySatID()'



      // Returns a gnssSatTypeValue with only this type of data.
      // @param type Type of value to be extracted.
   gnssSatTypeValue gnssSatTypeValue::extractTypeID(const TypeID& type) const
   {

      gnssSatTypeValue result;
      result.header = (*this).header;
      result.body = (*this).body.extractTypeID(type);

      return result;

   }  // End of method 'gnssSatTypeValue::extractTypeID()'



      // Returns a gnssSatTypeValue with only these types of data.
      // @param typeSet Set (TypeIDSet) containing the types of data
      //                to be extracted.
   gnssSatTypeValue gnssSatTypeValue::extractTypeID(const TypeIDSet& typeSet)
      const
   {

      gnssSatTypeValue result;
      result.header = (*this).header;
      result.body = (*this).body.extractTypeID(typeSet);

      return result;

   }  // End of method 'gnssSatTypeValue::extractTypeID()'



      // Modifies this object, keeping only this type of data.
      // @param type Type of value to be kept.
   gnssSatTypeValue& gnssSatTypeValue::keepOnlyTypeID(const TypeID& type)
   {

      TypeIDSet typeSet;
      typeSet.insert(type);

      return keepOnlyTypeID(typeSet);

   }  // End of method 'gnssSatTypeValue::keepOnlyTypeID()'



      // Modifies this object, keeping only these types of data.
      // @param typeSet Set (TypeIDSet) containing the types of data
      //                to be kept.
   gnssSatTypeValue& gnssSatTypeValue::keepOnlyTypeID(const TypeIDSet& typeSet)
   {

      satTypeValueMap stvMap( (*this).body.extractTypeID(typeSet) );
      (*this).body = stvMap;

      return (*this);

   }  // End of method 'gnssSatTypeValue::keepOnlyTypeID()'



      // Modifies this object, removing these satellites.
      // @param satSet Set (SatIDSet) containing the satellites
      //               to be removed.
   gnssSatTypeValue& gnssSatTypeValue::removeSatID(const SatIDSet& satSet)
   {

      for( SatIDSet::const_iterator pos = satSet.begin();
           pos != satSet.end();
           ++pos )
      {
         (*this).body.erase(*pos);
      }

      return (*this);

   }  // End of method 'gnssSatTypeValue::removeSatID()'



      // Modifies this object, removing these types of data
      // @param typeSet Set (TypeIDSet) containing the types of data
      //                to be kept.
   gnssSatTypeValue& gnssSatTypeValue::removeTypeID(const TypeIDSet& typeSet)
   {

      for( TypeIDSet::const_iterator pos = typeSet.begin();
           pos != typeSet.end();
           ++pos )
      {
         (*this).body.removeTypeID(*pos);
      }

      return (*this);

   }  // End of method 'gnssSatTypeValue::removeTypeID()'




      ////// gnssRinex //////


      // Returns a gnssRinex with only this satellite.
      // @param satellite Satellite to be extracted.
   gnssRinex gnssRinex::extractSatID(const SatID& satellite) const
   {

      gnssRinex result;
      result.header = (*this).header;
      result.body = (*this).body.extractSatID(satellite);

      return result;

   }  // End of method 'gnssRinex::extractSatID()'



      // Returns a gnssRinex with only one satellite, identified by
      // the given parameters.
      // @param p Satellite PRN number.
      // @param p System the satellite belongs to.
   gnssRinex gnssRinex::extractSatID( const int& p,
                                      const SatID::SatelliteSystem& s ) const
   {

      SatID tempSatellite(p, s);  // We build a temporary SatID object

      return extractSatID(tempSatellite);

   }  // End of method 'gnssRinex::extractSatID()'



      // Returns a gnssRinex with only these satellites.
      // @param satSet Set (SatIDSet) containing the satellites
      //               to be extracted.
   gnssRinex gnssRinex::extractSatID(const SatIDSet& satSet) const
   {

      gnssRinex result;
      result.header = (*this).header;
      result.body = (*this).body.extractSatID(satSet);

      return result;

   }  // End of method 'gnssRinex::extractSatID()'

   gnssRinex gnssRinex::extractSatSyst(const SatSystSet& satSet) const
   {

       gnssRinex result;
       result.header = (*this).header;
       result.body = (*this).body.extractSatSyst(satSet);

       return result;

   }  // End of method 'gnssRinex::extractSatID()'



      // Modifies this object, keeping only this satellite.
      // @param satellite Satellite to be kept.
   gnssRinex& gnssRinex::keepOnlySatID(const SatID& satellite)
   {

      SatIDSet satSet;
      satSet.insert(satellite);

      return keepOnlySatID(satSet);

   }  // End of method 'gnssRinex::keepOnlySatID()'



      // Modifies this object, keeping only this satellite.
      // @param p Satellite PRN number.
      // @param p System the satellite belongs to.
   gnssRinex& gnssRinex::keepOnlySatID( const int& p,
                                        const SatID::SatelliteSystem& s )
   {

      SatID tempSatellite(p, s);  // We build a temporary SatID object

      return keepOnlySatID(tempSatellite);

   }  // End of method 'gnssRinex::keepOnlySatID()'



      // Modifies this object, keeping only these satellites.
      // @param satSet Set (SatIDSet) containing the satellites to be kept.
   gnssRinex& gnssRinex::keepOnlySatID(const SatIDSet& satSet)
   {

      satTypeValueMap stvMap( (*this).body.extractSatID(satSet) );
      (*this).body = stvMap;

      return (*this);

   }  // End of method 'gnssRinex::keepOnlySatID()'

   gnssRinex& gnssRinex::keepOnlySatSystems(SatID::SatelliteSystem satSyst)
   {
       SatSystSet satSet;
       satSet.insert(satSyst);
       return keepOnlySatSystems(satSet);
   }

   gnssRinex& gnssRinex::keepOnlySatSystems(const SatSystSet& satSet)
   {

       satTypeValueMap stvMap((*this).body.extractSatSyst(satSet));
       (*this).body = stvMap;

       return (*this);

   }  // End of method 'gnssRinex::keepOnlySatID()'


      // Returns a gnssRinex with only this type of data.
      // @param type Type of value to be extracted.
   gnssRinex gnssRinex::extractTypeID(const TypeID& type) const
   {

      gnssRinex result;
      result.header = (*this).header;
      result.body = (*this).body.extractTypeID(type);

      return result;

   }  // End of method 'gnssRinex::extractTypeID()'



      // Returns a gnssRinex with only these types of data.
      // @param typeSet Set (TypeIDSet) containing the types of data
      //                to be extracted.
   gnssRinex gnssRinex::extractTypeID(const TypeIDSet& typeSet) const
   {

      gnssRinex result;
      result.header = (*this).header;
      result.body = (*this).body.extractTypeID(typeSet);

      return result;

   }  // End of method 'gnssRinex::extractTypeID()'



      // Modifies this object, keeping only this type of data.
      // @param type Type of value to be kept.
   gnssRinex& gnssRinex::keepOnlyTypeID(const TypeID& type)
   {

      TypeIDSet typeSet;
      typeSet.insert(type);

      return keepOnlyTypeID(typeSet);

   }  // End of method 'gnssRinex::keepOnlyTypeID()'



      // Modifies this object, keeping only these types of data.
      // @param typeSet Set (TypeIDSet) containing the types of data
      //                to be kept.
   gnssRinex& gnssRinex::keepOnlyTypeID(const TypeIDSet& typeSet)
   {

      satTypeValueMap stvMap( (*this).body.extractTypeID(typeSet) );
      (*this).body = stvMap;

      return (*this);

   }  // End of method 'gnssRinex::keepOnlyTypeID()'





      //// Some other handy data structures
	/* Returns the data value (double) corresponding to provided SourceID,
	   * SatID and TypeID.
	   *
	   * @param source        Source to be looked for.
	   * @param satellite     Satellite to be looked for.
	   * @param type          Type to be looked for.
	   */
   double sourcePtrDataMap::getValue(const SourceID& source,
	   const SatID& satellite,
	   const TypeID& type) const
	   throw(SourceIDNotFound, SatIDNotFound, TypeIDNotFound)
   {

	   // Look for the SourceID
	   auto itObs = find(source);
	   if (itObs != (*this).end())
	   {

		   return (*itObs).second.getValue(satellite, type);
	   }
	   else
	   {
		   GPSTK_THROW(SourceIDNotFound("SourceID not found in map"));
	   }

   }  // End of method 'sourceDataMap::getValue()'

	  /* Get a set with all the SourceID's in this data structure.
	   *
	   * @warning If current 'sourceDataMap' is big, this could be a very
	   * costly operation.
	   */
   SourceIDSet sourcePtrDataMap::getSourceIDSet(void) const
   {
	   // SourceID set to be returned
	   SourceIDSet toReturn;

	   // Then, iterate through corresponding 'sourceDataMap'
	   for (auto&& it : *this)
		   toReturn.insert(it.first);


	   return toReturn;

   }  // End of method 'sourceDataMap::getSourceIDSet()'



	  /* Get a set with all the SatID's in this data structure.
	   *
	   * @warning If current 'sourceDataMap' is big, this could be a very
	   * costly operation.
	   */
   SatIDSet sourcePtrDataMap::getSatIDSet(void) const
   {

	   // SatID set to be returned
	   SatIDSet toReturn;

	   // Then, iterate through corresponding 'sourceDataMap'
	   for (auto&& sdmIter : *this)
		   // Finally, iterate through corresponding 'satTypeValueMap'
		   for (auto && stvmIter : sdmIter.second)
			   // Add current SatID to 'toReturn'
			   toReturn.insert(stvmIter.first);


	   return toReturn;

   }  // End of method 'sourceDataMap::getSatIDSet()'

      /* Returns the data value (double) corresponding to provided SourceID,
       * SatID and TypeID.
       *
       * @param source        Source to be looked for.
       * @param satellite     Satellite to be looked for.
       * @param type          Type to be looked for.
       */
   double sourceDataMap::getValue( const SourceID& source,
                                   const SatID& satellite,
                                   const TypeID& type ) const
      throw( SourceIDNotFound, SatIDNotFound, TypeIDNotFound )
   {

         // Look for the SourceID
      sourceDataMap::const_iterator itObs( (*this).find(source) );
      if( itObs != (*this).end() )
      {
         return (*itObs).second.getValue( satellite, type );
      }
      else
      {
         GPSTK_THROW(SourceIDNotFound("SourceID not found in map"));
      }

   }  // End of method 'sourceDataMap::getValue()'



      /* Get a set with all the SourceID's in this data structure.
       *
       * @warning If current 'sourceDataMap' is big, this could be a very
       * costly operation.
       */
   SourceIDSet sourceDataMap::getSourceIDSet( void ) const
   {

         // SourceID set to be returned
      SourceIDSet toReturn;

         // Then, iterate through corresponding 'sourceDataMap'
      for( sourceDataMap::const_iterator sdmIter = (*this).begin();
           sdmIter != (*this).end();
           ++sdmIter )
      {

            // Add current SourceID to 'toReturn'
         toReturn.insert( (*sdmIter).first );

      }  // End of 'for( sourceDataMap::const_iterator sdmIter = ...'

      return toReturn;

   }  // End of method 'sourceDataMap::getSourceIDSet()'



      /* Get a set with all the SatID's in this data structure.
       *
       * @warning If current 'sourceDataMap' is big, this could be a very
       * costly operation.
       */
   SatIDSet sourceDataMap::getSatIDSet( void ) const
   {

         // SatID set to be returned
      SatIDSet toReturn;

         // Then, iterate through corresponding 'sourceDataMap'
      for( sourceDataMap::const_iterator sdmIter = (*this).begin();
           sdmIter != (*this).end();
           ++sdmIter )
      {

            // Finally, iterate through corresponding 'satTypeValueMap'
         for( satTypeValueMap::const_iterator stvmIter =
                                                   (*sdmIter).second.begin();
              stvmIter != (*sdmIter).second.end();
              stvmIter++ )
         {

               // Add current SatID to 'toReturn'
            toReturn.insert( (*stvmIter).first );

         }  // End of 'for( satTypeValueMap::const_iterator stvmIter = ...'

      }  // End of 'for( sourceDataMap::const_iterator sdmIter = ...'

      return toReturn;

   }  // End of method 'sourceDataMap::getSatIDSet()'



      /* Adds 'gnssSatTypeValue' object data to this structure.
       *
       * @param gds     gnssSatTypeValue object containing data to be added.
       */
   gnssDataMap& gnssDataMap::addGnssSatTypeValue( const gnssSatTypeValue& gds )
   {

         // Declare an object for temporal storage of data
      sourceDataMap sdMap;

         // Fill with data
      sdMap[ gds.header.source ] = gds.body;

         // Introduce data into this GDS
      (*this).insert( pair<const CommonTime, sourceDataMap>( gds.header.epoch, sdMap ) );

         // Return curren GDS.
      return (*this);

   }  // End of method 'gnssDataMap::addGnssSatTypeValue()'



      /* Adds 'gnssRinex' object data to this structure.
       *
       * @param gds     gnssRinex object containing data to be added.
       */
   gnssDataMap& gnssDataMap::addGnssRinex( const gnssRinex& gds )
   {

         // Declare an object for temporal storage of data
      sourceDataMap sdMap;

         // Fill with data
      sdMap[ gds.header.source ] = gds.body;

         // Introduce data into this GDS
      (*this).insert( pair<const CommonTime, sourceDataMap>( gds.header.epoch, sdMap ) );

         // Return current GDS.
      return (*this);

   }  // End of method 'gnssDataMap::addGnssRinex()'



      /* Returns a 'gnssRinex' object corresponding to given SourceID.
       *
       * @param source     SourceID object.
       *
       * @warning Returned data will correspond to first matching SourceID,
       * if it exists.
       */
   gnssRinex gnssDataMap::getGnssRinex( const SourceID& source ) const
   {

         // Get first data set
      gnssDataMap gdMap( (*this).frontEpoch() );

         // Declare a gnssRinex object to be returned
      gnssRinex toReturn;

         // We'll need a flag
      bool found(false);

         // Look into the data structure
      for( gnssDataMap::const_iterator it = gdMap.begin();
           it != gdMap.end() && !found;
           ++it )
      {

            // Look for SourceID
         sourceDataMap::const_iterator iter( (*it).second.find( source ) );
         if( iter != (*it).second.end() )
         {
            toReturn.body = (*iter).second;
            toReturn.header.source = (*iter).first;
            toReturn.header.epoch = (*it).first;
            toReturn.header.epochFlag = 0;
            found = true;
         }

      }  // End of 'for( gnssDataMap::const_iterator it = gdMap.begin(); ...'

         // Return GDS
      return toReturn;

   }  // End of method 'gnssDataMap::addGnssRinex()'



      /* Adds 'gnssDataMap' object data to this structure.
       *
       * @param gds     gnssDataMap object containing data to be added.
       */
   gnssDataMap& gnssDataMap::addGnssDataMap( const gnssDataMap& gds )
   {
         // Iterate through all items in the gnssDataMap
      for( gnssDataMap::const_iterator it = gds.begin();
           it != gds.end();
           ++it )
      {
         const CommonTime& time(it->first);
         const sourceDataMap& sourceMap(it->second);

         for(sourceDataMap::const_iterator itsrc = sourceMap.begin();
             itsrc != sourceMap.end();
             ++itsrc)
         {
            gnssSatTypeValue stv;
            stv.header.epoch = time;
            stv.header.source = itsrc->first;
            stv.body = itsrc->second;

            addGnssSatTypeValue(stv);

         }  // loop in the sources

      }  // End of 'for( gnssDataMap::const_iterator it = gdMap.begin(); ...'

      return (*this);
   }


      // Returns a copy of the first element in the map.
   gnssDataMap gnssDataMap::front( void ) const
   {

         // Object to be returned
      gnssDataMap toReturn;

         // First check that structure isn't empty
      if( !( (*this).empty() ) )
      {

            // Get first element
         gnssDataMap::const_iterator firstIt( (*this).begin() );

            // Insert first element in 'toReturn' object
         toReturn.insert(*firstIt);

      }

      return toReturn;

   }  // End of method 'gnssDataMap::front()'



      // Removes the first element in the map.
   void gnssDataMap::pop_front( void )
   {

         // First check that structure isn't empty
      if( !( (*this).empty() ) )
      {

            // Define an iterator pointing to the beginning of this map
         gnssDataMap::iterator pos( (*this).begin() );

            // It is advisable to avoid sawing off the branch we are sitting on
         (*this).erase( pos++ );

      }

      return;

   }  // End of method 'gnssDataMap::pop_front()'



      // Returns the data corresponding to the first epoch in the map,
      // taking into account the 'tolerance' value.
   gnssDataMap gnssDataMap::frontEpoch( void ) const
   {

         // Declare structure to be returned
      gnssDataMap toReturn;

         // First check that structure isn't empty
      if( !( (*this).empty() ) )
      {

            // Get the 'CommonTime' of the first element
         CommonTime firstEpoch( (*(*this).begin()).first );

            // Find the position of the first element PAST
            // 'firstEpoch+tolerance'
         gnssDataMap::const_iterator endPos(
                                 (*this).upper_bound(firstEpoch+tolerance) );

            // Add values to 'toReturn'
         for( gnssDataMap::const_iterator it = (*this).begin();
              it != endPos;
              ++it )
         {
            toReturn.insert( (*it) );
         }

      }

      return toReturn;

   }  // End of method 'gnssDataMap::frontEpoch()'



      // Removes the first epoch in the map. Be aware that this method
      // takes into account 'tolerance', so more than just one epoch may be
      // removed if they are within 'tolerance' margin.
   void gnssDataMap::pop_front_epoch( void )
   {

         // First check that structure isn't empty
      if( !( (*this).empty() ) )
      {

            // Get the 'CommonTime' of the first element
         CommonTime firstEpoch( (*(*this).begin()).first );

            // Find the position of the first element PAST
            // 'firstEpoch+tolerance'
         gnssDataMap::iterator endPos(
                                 (*this).upper_bound(firstEpoch+tolerance) );

            // Remove values
         for( gnssDataMap::iterator pos = (*this).begin();
              pos != endPos; )
         {

               // It is advisable to avoid sawing off the branch we are
               // sitting on
            (*this).erase( pos++ );
         }

      }

      return;

   }  // End of method 'gnssDataMap::pop_front_epoch()'



      // Returns a copy of the last element in the map.
   gnssDataMap gnssDataMap::back( void ) const
   {

         // Object to be returned
      gnssDataMap toReturn;

         // First check that structure isn't empty
      if( !( (*this).empty() ) )
      {

            // Get last element
         gnssDataMap::const_reverse_iterator lastIt( (*this).rbegin() );

            // Insert the last element in 'toReturn' object
         toReturn.insert(*lastIt);

      }

      return toReturn;

   }  // End of method 'gnssDataMap::back()'



      // Removes the last element in the map.
   void gnssDataMap::pop_back( void )
   {

         // First check that structure isn't empty
      if( !( (*this).empty() ) )
      {

            // Define an iterator pointing to the end of this map
         gnssDataMap::iterator pos( (*this).end() );

            // Delete element
         (*this).erase( --pos );

      }

      return;

   }  // End of method 'gnssDataMap::pop_back()'



      // Returns the data corresponding to the last epoch in the map,
      // taking into account the 'tolerance' value.
   gnssDataMap gnssDataMap::backEpoch( void ) const
   {

         // Declare structure to be returned
      gnssDataMap toReturn;

         // First check that structure isn't empty
      if( !( (*this).empty() ) )
      {

            // Get the 'CommonTime' of the last element
         CommonTime lastEpoch( (*(--(*this).end())).first );

            // Find the position of the last element PAST
            // 'lastEpoch-tolerance'
         gnssDataMap::const_iterator beginPos(
                                 (*this).lower_bound(lastEpoch-tolerance) );

            // Add values to 'toReturn'
         for( gnssDataMap::const_iterator it = beginPos;
              it != (*this).end();
              ++it )
         {
            toReturn.insert( (*it) );
         }

      }

      return toReturn;

   }  // End of method 'gnssDataMap::backEpoch()'



      // Removes the last epoch in the map. Be aware that this method
      // takes into account 'tolerance', so more than just one epoch may be
      // removed if they are within 'tolerance' margin.
   void gnssDataMap::pop_back_epoch( void )
   {

         // First check that structure isn't empty
      if( !( (*this).empty() ) )
      {

            // Get the 'CommonTime' of the last element
         CommonTime lastEpoch( (*(--(*this).end())).first );

            // Find the position of the last element PAST
            // 'lastEpoch-tolerance'
         gnssDataMap::iterator beginPos(
                                 (*this).lower_bound(lastEpoch-tolerance) );

            // Remove values
         for( gnssDataMap::iterator pos = beginPos;
              pos != (*this).end(); )
         {

               // It is advisable to avoid sawing off the branch we are
               // sitting on
            (*this).erase( pos++ );
         }

      }

      return;

   }  // End of method 'gnssDataMap::pop_back_epoch()'



      /* Returns a 'gnssDataMap' with the data corresponding to provided
       * CommonTime, taking into account 'tolerance'.
       *
       * @param epoch         Epoch to be looked for.
       */
   gnssDataMap gnssDataMap::getDataFromEpoch( const CommonTime& epoch ) const
      throw( CommonTimeNotFound )
   {

         // Declare structure to be returned
      gnssDataMap toReturn;

         // First check that structure isn't empty
      if( !( (*this).empty() ) )
      {

            // Find the position of the first element whose key is not less than
            // 'epoch-tolerance'
         gnssDataMap::const_iterator beginPos(
                                 (*this).lower_bound(epoch-tolerance) );

            // Find the position of the first element PAST
            // 'epoch+tolerance'
         gnssDataMap::const_iterator endPos(
                                 (*this).upper_bound(epoch+tolerance) );

            // Add values to 'toReturn'
         for( gnssDataMap::const_iterator it = beginPos;
              it != endPos;
              ++it )
         {
            toReturn.insert( (*it) );
         }

      }
      else
      {
         GPSTK_THROW(CommonTimeNotFound("Data map is empty"));
      }

         // Check if 'toReturn' is empty
      if( toReturn.empty() )
      {
         GPSTK_THROW(CommonTimeNotFound("Epoch not found"));
      }

      return toReturn;

   }  // End of method 'gnssDataMap::getDataFromEpoch()'



      /* Returns the data value (double) corresponding to provided CommonTime,
       * SourceID, SatID and TypeID.
       *
       * @param epoch         Epoch to be looked for.
       * @param source        Source to be looked for.
       * @param satellite     Satellite to be looked for.
       * @param type          Type to be looked for.
       *
       * @warning If within (epoch +/- tolerance) more than one match exists,
       * then only the first one is returned.
       */
   double gnssDataMap::getValue( const CommonTime& epoch,
                                 const SourceID& source,
                                 const SatID& satellite,
                                 const TypeID& type ) const
      throw( CommonTimeNotFound, ValueNotFound )
   {

         // Look for the epoch (CommonTime) data
      gnssDataMap gdMap( getDataFromEpoch(epoch) );

         // Value to be returned
      double toReturn = 0.0;

         // We'll need a flag
      bool found(false);

         // Look into the data structure
      for( gnssDataMap::const_iterator it = gdMap.begin();
           it != gdMap.end() && !found;
           ++it )
      {

         try
         {
            toReturn = (*it).second.getValue( source, satellite, type );
            found = true;
         }
         catch(...)
         {
            continue;
         }

      }

         // Check if value was found
      if( !found )
      {
         GPSTK_THROW(ValueNotFound("Value not found"));
      }

      return toReturn;

   }  // End of method 'gnssDataMap::getValue()'



      /* Returns the data value (double) corresponding to the first epoch
       * in the data structure, given SourceID, SatID and TypeID.
       *
       * @param source        Source to be looked for.
       * @param satellite     Satellite to be looked for.
       * @param type          Type to be looked for.
       *
       * @warning If within first epoch (epoch +/- tolerance) more than one
       * match exists, then only the first one is returned.
       */
   double gnssDataMap::getValue( const SourceID& source,
                                 const SatID& satellite,
                                 const TypeID& type ) const
      throw( ValueNotFound )
   {

         // Look for the epoch (CommonTime) data
      gnssDataMap gdMap( frontEpoch() );

         // Value to be returned
      double toReturn = 0.0;

         // We'll need a flag
      bool found(false);

         // Look into the data structure
      for( gnssDataMap::const_iterator it = gdMap.begin();
           it != gdMap.end() && !found;
           ++it )
      {

         try
         {
            toReturn = (*it).second.getValue( source, satellite, type );
            found = true;
         }
         catch(...)
         {
            continue;
         }

      }

         // Check if value was found
      if( !found )
      {
         GPSTK_THROW(ValueNotFound("Value not found"));
      }

      return toReturn;

   }  // End of method 'gnssDataMap::getValue()'



      /* Inserts a data value (double) at the provided CommonTime, SourceID,
       * SatID and TypeID, taking into account 'tolerance'.
       *
       * @param epoch         Epoch to be looked for.
       * @param source        Source to be looked for.
       * @param satellite     Satellite to be looked for.
       * @param type          Type of the new data.
       * @param value         Value to be inserted.
       */
   gnssDataMap& gnssDataMap::insertValue( const CommonTime& epoch,
                                          const SourceID& source,
                                          const SatID& satellite,
                                          const TypeID& type,
                                          double value )
      throw( CommonTimeNotFound, ValueNotFound )
   {

         // First check that structure isn't empty
      if( !( (*this).empty() ) )
      {

            // Find the position of the first element whose key is not less than
            // 'epoch-tolerance'
         gnssDataMap::iterator beginPos( (*this).lower_bound(epoch-tolerance) );

            // Find the position of the first element PAST
            // 'epoch+tolerance'
         gnssDataMap::iterator endPos( (*this).upper_bound(epoch+tolerance) );

            // Check if we found some match within current tolerance
         if( beginPos != endPos )
         {

               // We'll need a flag
            bool found(false);

               // Look into the range
            for( gnssDataMap::iterator it = beginPos;
                 it != endPos && !found;
                 ++it )
            {

                  // Look for the source
               sourceDataMap::iterator it2( (*it).second.find(source) );

               if( it2 != (*it).second.end() )
               {

                     // Look for the satellite
                  satTypeValueMap::iterator it3((*it2).second.find(satellite));

                  if( it3 != (*it2).second.end() )
                  {

                        // Insert type and value
                     (*it3).second[ type ] = value;

                        // Work is done, let's get out
                     found = true;

                  }  // End of 'if( it3 != (*it2).second.end() )'

               }  // End of 'if( it2 != (*it).second.end() )'

            }  // End of 'for( gnssDataMap::iterator it = beginPos ...'

               // Check if we found a proper place to insert value
            if( !found )
            {
               GPSTK_THROW( ValueNotFound("No proper place to insert value"));
            }

         }
         else
         {
               // No match found for CommonTime with current tolerance
            GPSTK_THROW(CommonTimeNotFound("Epoch not found within tolerance"));
         }

      }
      else
      {
         GPSTK_THROW(CommonTimeNotFound("Data map is empty"));
      }

      return (*this);

   }  // End of method 'gnssDataMap::insertValue()'



      /* Inserts a data value (double) in the first epoch of the data
       * structure with the given SourceID, SatID and TypeID.
       *
       * @param source        Source to be looked for.
       * @param satellite     Satellite to be looked for.
       * @param type          Type of the new data.
       * @param value         Value to be inserted.
       */
   gnssDataMap& gnssDataMap::insertValue( const SourceID& source,
                                          const SatID& satellite,
                                          const TypeID& type,
                                          double value )
      throw( ValueNotFound )
   {

         // We'll need a flag
      bool found(false);

         // Look into the data structure
      for( gnssDataMap::iterator it = (*this).begin();
           it != (*this).end() && !found;
           ++it )
      {

            // Look for the source
         sourceDataMap::iterator it2( (*it).second.find(source) );

         if( it2 != (*it).second.end() )
         {

               // Look for the satellite
            satTypeValueMap::iterator it3( (*it2).second.find(satellite) );

            if( it3 != (*it2).second.end() )
            {

                  // Insert type and value
               (*it3).second[ type ] = value;

                  // Work is done, let's get out
               found = true;

            }  // End of 'if( it3 != (*it2).second.end() )'

         }  // End of 'if( it2 != (*it).second.end() )'

      }  // End of 'for( gnssDataMap::iterator it = (*this).begin(); ...'

         // Check if we found a proper place to insert value
      if( !found )
      {
         GPSTK_THROW( ValueNotFound("No proper place to insert value"));
      }

      return (*this);

   }  // End of method 'gnssDataMap::insertValue()'



      /* Get a set with all the SourceID's in this data structure.
       *
       * @warning If current 'gnssDataMap' is big, this could be a very
       * costly operation.
       */
   SourceIDSet gnssDataMap::getSourceIDSet( void ) const
   {

         // SourceID set to be returned
      SourceIDSet toReturn;

         // Iterate through all items in the gnssDataMap
      for( gnssDataMap::const_iterator it = (*this).begin();
           it != (*this).end();
           ++it )
      {

            // Then, iterate through corresponding 'sourceDataMap'
         for( sourceDataMap::const_iterator sdmIter = (*it).second.begin();
              sdmIter != (*it).second.end();
              ++sdmIter )
         {

               // Add current SourceID to 'toReturn'
            toReturn.insert( (*sdmIter).first );

         }  // End of 'for( sourceDataMap::const_iterator sdmIter = ...'

      }  // End of 'for( gnssDataMap::const_iterator it = gdMap.begin(); ...'

      return toReturn;

   }  // End of method 'gnssDataMap::getSourceIDSet()'



      /* Get a set with all the SatID's in this data structure.
       *
       * @warning If current 'gnssDataMap' is big, this could be a very
       * costly operation.
       */
   SatIDSet gnssDataMap::getSatIDSet( void ) const
   {

         // SatID set to be returned
      SatIDSet toReturn;

         // Iterate through all items in the gnssDataMap
      for( gnssDataMap::const_iterator it = (*this).begin();
           it != (*this).end();
           ++it )
      {

            // Then, iterate through corresponding 'sourceDataMap'
         for( sourceDataMap::const_iterator sdmIter = (*it).second.begin();
              sdmIter != (*it).second.end();
              ++sdmIter )
         {

               // Finally, iterate through corresponding 'satTypeValueMap'
            for( satTypeValueMap::const_iterator stvmIter =
                                                      (*sdmIter).second.begin();
                 stvmIter != (*sdmIter).second.end();
                 stvmIter++ )
            {

                  // Add current SatID to 'toReturn'
               toReturn.insert( (*stvmIter).first );

            }  // End of 'for( satTypeValueMap::const_iterator stvmIter = ...'

         }  // End of 'for( sourceDataMap::const_iterator sdmIter = ...'

      }  // End of 'for( gnssDataMap::const_iterator it = gdMap.begin(); ...'

      return toReturn;

   }  // End of method 'gnssDataMap::getSatIDSet()'



      // Convenience output method
   std::ostream& gnssDataMap::dump( std::ostream& s,
                                    int mode ) const
   {

         // Iterate through all items in the gnssDataMap
      for( gnssDataMap::const_iterator it = (*this).begin();
           it != (*this).end();
           ++it )
      {

            // Then, iterate through corresponding 'sourceDataMap'
         for( sourceDataMap::const_iterator sdmIter = (*it).second.begin();
              sdmIter != (*it).second.end();
              ++sdmIter )
         {

               // Finally, iterate through corresponding 'satTypeValueMap'
            for( satTypeValueMap::const_iterator stvmIter =
                                                      (*sdmIter).second.begin();
                 stvmIter != (*sdmIter).second.end();
                 stvmIter++ )
            {

                  // Declare a 'YDSTime' object to ease printing
               YDSTime time( (*it).first );
                  // First, print year, Day-Of-Year and Seconds of Day
               s << time.year << " "
                 << time.doy << " "
                 << time.sod << " ";

                  // Second, print SourceID information
               s << (*sdmIter).first << " ";

                  // Third, print satellite (system and PRN)
               s << (*stvmIter).first << " ";

                  // Fourth, print type descriptions and numerical values
               for( typeValueMap::const_iterator itObs =
                                                   (*stvmIter).second.begin();
                    itObs != (*stvmIter).second.end();
                    itObs++ )
               {

                  s << (*itObs).first << " "
                    << (*itObs).second << " ";

               }  // End of 'for( typeValueMap::const_iterator itObs = ...'

                  // Add end-of-line
               s << endl;

            }  // End of 'for( satTypeValueMap::const_iterator stvmIter = ...'

         }  // End of 'for( sourceDataMap::const_iterator sdmIter = ...'

      }  // End of 'for( gnssDataMap::const_iterator it = gdMap.begin(); ...'

         // Let's return the 'std::ostream'
      return s;

   }  // End of method 'gnssDataMap::dump()'



      // Returns a gnssDataMap with only this source.
      // @param source Source to be extracted.
   gnssDataMap gnssDataMap::extractSourceID(const SourceID& source)
   {
      SourceIDSet sourceSet;
      sourceSet.insert(source);

      return extractSourceID(sourceSet);

   }  // End of method 'gnssDataMap::extractSourceID()'



      /// Returns a gnssDataMap with only these sources.
      /// @param sourceSet Set(SourceIDSet) containing the sources
      ///                  to be extracted.
   gnssDataMap gnssDataMap::extractSourceID(const SourceIDSet& sourceSet)
   {
      gnssDataMap dataMap;

      // Iterate through all items in the gnssDataMap
      for( gnssDataMap::const_iterator it = this->begin();
           it != this->end();
           ++it )
      {
         const CommonTime& time(it->first);
         const sourceDataMap& sourceMap(it->second);

         for(sourceDataMap::const_iterator itsrc = sourceMap.begin();
             itsrc != sourceMap.end();
             ++itsrc)
         {
            SourceIDSet::const_iterator itsrc2 = sourceSet.find(itsrc->first);
            if(itsrc2!=sourceSet.end())
            {
               gnssSatTypeValue gds;
               gds.header.epoch = time;
               gds.header.source = itsrc->first;
               gds.body = itsrc->second;

               dataMap.addGnssSatTypeValue(gds);
            }

         }  // loop in the sources

      }  // End of 'for( gnssDataMap::const_iterator it = gdMap.begin(); ...'

      return dataMap;

   }  // End of method 'gnssDataMap::extractSourceID()'


      // Modifies this object, keeping only this source.
      // @param source Source to be extracted.
   gnssDataMap& gnssDataMap::keepOnlySourceID(const SourceID& source)
   {
      (*this) = extractSourceID(source);
      return (*this);
   }


      // Modifies this object, keeping only these sources.
      // @param sourceSet Set(SourceIDSet) containing the sources
      //                  to be extracted.
   gnssDataMap& gnssDataMap::keepOnlySourceID(const SourceIDSet& sourceSet)
   {
      (*this) = extractSourceID(sourceSet);
      return (*this);
   }


      // Modifies this object, removing this source.
      // @param source Source to be removed.
   gnssDataMap& gnssDataMap::removeSourceID(const SourceID& source)
   {
      SourceIDSet sourceSet;
      sourceSet.insert(source);

      return removeSourceID(sourceSet);
   }


      // Modifies this object, keeping only these sources.
      // @param sourceSet Set(SourceIDSet) containing the sources
      //                  to be removed.
   gnssDataMap& gnssDataMap::removeSourceID(const SourceIDSet& sourceSet)
   {
      gnssDataMap dataMap;

      // Iterate through all items in the gnssDataMap
      for( gnssDataMap::const_iterator it = this->begin();
           it != this->end();
           ++it )
      {
         const CommonTime& time(it->first);
         const sourceDataMap& sourceMap(it->second);

         for(sourceDataMap::const_iterator itsrc = sourceMap.begin();
            itsrc != sourceMap.end();
            ++itsrc)
         {
            SourceIDSet::const_iterator itsrc2 = sourceSet.find(itsrc->first);
            if(itsrc2==sourceSet.end())
            {
               gnssSatTypeValue gds;
               gds.header.epoch = time;
               gds.header.source = itsrc->first;
               gds.body = itsrc->second;

               dataMap.addGnssSatTypeValue(gds);
            }

         }  // loop in the sources

      }  // End of 'for( gnssDataMap::const_iterator it = gdMap.begin(); ...'

      (*this) = dataMap;

      return (*this);

   }  // End of method 'gnssDataMap::removeSourceID()'


      // Returns a gnssDataMap with only this satellite.
      // @param sat Satellite to be extracted.
   gnssDataMap gnssDataMap::extractSatID(const SatID& sat)
   {
      SatIDSet satSet;
      satSet.insert(sat);

      return extractSatID(satSet);
   }


      // Returns a gnssDataMap with only these satellites.
      // @param satSet Set(SatIDSet) containing the satellite
      //               to be extracted.
   gnssDataMap gnssDataMap::extractSatID(const SatIDSet& satSet)
   {
      gnssDataMap dataMap;

      // Iterate through all items in the gnssDataMap
      for( gnssDataMap::const_iterator it = this->begin();
           it != this->end();
           ++it )
      {
         const CommonTime& time(it->first);
         const sourceDataMap& sourceMap(it->second);

         for(sourceDataMap::const_iterator itsrc = sourceMap.begin();
             itsrc != sourceMap.end();
             ++itsrc)
         {
            gnssSatTypeValue gds;
            gds.header.epoch = time;
            gds.header.source = itsrc->first;
            gds.body = itsrc->second;

            gds.body.keepOnlySatID(satSet);

            dataMap.addGnssSatTypeValue(gds);

         }  // loop in the sources

      }  // End of 'for( gnssDataMap::const_iterator it = gdMap.begin(); ...'

      return dataMap;

   }  // End of method 'gnssDataMap::extractSatID()'


      // Modifies this object, keeping only this satellite.
      // @param sat Satellite to be extracted.
   gnssDataMap& gnssDataMap::keepOnlySatID(const SatID& sat)
   {
      (*this) = extractSatID(sat);
      return (*this);
   }


      // Modifies this object, keeping only these satellites.
      // @param satSet Set(SatIDSet) containing the satellite
      //                  to be extracted.
   gnssDataMap& gnssDataMap::keepOnlySatID(const SatIDSet& satSet)
   {
      (*this) = extractSatID(satSet);
      return (*this);
   }

      // Modifies this object, removing this satellite.
      // @param sat Satellite to be removed.
   gnssDataMap& gnssDataMap::removeSatID(const SatID& sat)
   {
      SatIDSet satSet;
      satSet.insert(sat);

      return removeSatID(satSet);
   }


      // Modifies this object, keeping only these satellites.
      // @param satSet Set(SatIDSet) containing the satellites
      //               to be removed.
   gnssDataMap& gnssDataMap::removeSatID(const SatIDSet& satSet)
   {
      gnssDataMap dataMap;

      // Iterate through all items in the gnssDataMap
      for( gnssDataMap::const_iterator it = this->begin();
           it != this->end();
           ++it )
      {
         const CommonTime& time(it->first);
         const sourceDataMap& sourceMap(it->second);

         for(sourceDataMap::const_iterator itsrc = sourceMap.begin();
             itsrc != sourceMap.end();
             ++itsrc)
         {
            gnssSatTypeValue gds;
            gds.header.epoch = time;
            gds.header.source = itsrc->first;
            gds.body = itsrc->second;

            gds.body.removeSatID(satSet);

            dataMap.addGnssSatTypeValue(gds);

         }  // loop in the sources

      }  // End of 'for( gnssDataMap::const_iterator it = gdMap.begin(); ...'

      (*this) = dataMap;

      return (*this);

   }  // End of method 'gnssDataMap::removeSatID()'



      /// Returns a gnssDataMap with only this type.
      /// @param type Type to be extracted.
   gnssDataMap gnssDataMap::extractTypeID(const TypeID& type)
   {
      TypeIDSet typeSet;
      typeSet.insert(type);

      return extractTypeID(typeSet);
   }


      // Returns a gnssDataMap with only these satellites.
      // @param typeSet Set(TypeIDSet) containing the types
      //               to be extracted.
   gnssDataMap gnssDataMap::extractTypeID(const TypeIDSet& typeSet)
   {
      gnssDataMap dataMap;

      // Iterate through all items in the gnssDataMap
      for( gnssDataMap::const_iterator it = this->begin();
           it != this->end();
           ++it )
      {
         const CommonTime& time(it->first);
         const sourceDataMap& sourceMap(it->second);

         for(sourceDataMap::const_iterator itsrc = sourceMap.begin();
             itsrc != sourceMap.end();
             ++itsrc)
         {
            gnssSatTypeValue gds;
            gds.header.epoch = time;
            gds.header.source = itsrc->first;
            gds.body = itsrc->second;

            gds.body.keepOnlyTypeID(typeSet);

            dataMap.addGnssSatTypeValue(gds);

         }  // loop in the sources

      }  // End of 'for( gnssDataMap::const_iterator it = gdMap.begin(); ...'

      return dataMap;
   }


      // Modifies this object, keeping only this type.
      // @param type Type to be extracted.
   gnssDataMap& gnssDataMap::keepOnlyTypeID(const TypeID& type)
   {
      (*this) = extractTypeID(type);
      return (*this);
   }


      // Modifies this object, keeping only these types.
      // @param typeSet Set(TypeIDSet) containing the type
      //                  to be extracted.
   gnssDataMap& gnssDataMap::keepOnlyTypeID(const TypeIDSet& typeSet)
   {
      (*this) = extractTypeID(typeSet);
      return (*this);
   }


      // Modifies this object, removing this type.
      // @param type Type to be removed.
   gnssDataMap& gnssDataMap::removeTypeID(const TypeID& type)
   {
      TypeIDSet typeSet;
      typeSet.insert(type);

      return removeTypeID(typeSet);
   }


      // Modifies this object, keeping only these types.
      // @param typeSet Set(TypeIDSet) containing the types
      //               to be removed.
   gnssDataMap& gnssDataMap::removeTypeID(const TypeIDSet& typeSet)
   {
      gnssDataMap dataMap;

      // Iterate through all items in the gnssDataMap
      for( gnssDataMap::const_iterator it = this->begin();
           it != this->end();
           ++it )
      {
         const CommonTime& time(it->first);
         const sourceDataMap& sourceMap(it->second);

         for(sourceDataMap::const_iterator itsrc = sourceMap.begin();
             itsrc != sourceMap.end();
             ++itsrc)
         {
            gnssSatTypeValue gds;
            gds.header.epoch = time;
            gds.header.source = itsrc->first;
            gds.body = itsrc->second;

            gds.body.removeTypeID(typeSet);

            dataMap.addGnssSatTypeValue(gds);

         }  // loop in the sources

      }  // End of 'for( gnssDataMap::const_iterator it = gdMap.begin(); ...'

      (*this) = dataMap;

      return (*this);

   }  // End of method 'gnssDataMap::removeTypeID()'


      /* Edit the dataset, removing data outside the indicated time
       *  interval.
       *
       * @param[in] tmin defines the beginning of the time interval
       * @param[in] tmax defines the end of the time interval
       */
   gnssDataMap& gnssDataMap::edit(CommonTime tmin, CommonTime tmax )
   {
      gnssDataMap dataMap;

      while(!this->empty())
      {
         gnssDataMap gds = this->frontEpoch();

         CommonTime time(gds.begin()->first);
         if( (time>=tmin) && (time<=tmax) ) dataMap.addGnssDataMap(gds);

         this->pop_front_epoch();
      }

      (*this) = dataMap;

      return (*this);
   }

      // Load data from a rinex observation file
   void gnssDataMap::loadObsFile(std::string obsFile)
   {
      try
      {
         RinexObsStream rin(obsFile);
         rin.exceptions(ios::failbit);

         gnssRinex gRin;
         while( rin >> gRin )
         {
            this->addGnssRinex(gRin);
         }

         rin.close();
      }
      catch(...)
      {
         Exception e("Failed to load obs file '"+obsFile+"'.");
         GPSTK_THROW(e);
      }

   }  // End of method 'gnssDataMap::loadObsFile()'


      ////// Other stuff //////



      // Convenience output method for structure satTypeValueMap
   std::ostream& satTypeValueMap::dump( std::ostream& s,
                                        int mode ) const
   {

      for( satTypeValueMap::const_iterator it = (*this).begin();
           it!= (*this).end();
           it++ )
      {

            // First, print satellite (system and PRN)
         s << (*it).first << " ";

         for( typeValueMap::const_iterator itObs = (*it).second.begin();
              itObs != (*it).second.end();
              itObs++ )
         {

            if (mode==1)
            {
               s << (*itObs).first << " ";
            }

            s << (*itObs).second << " ";

         }  // End of 'for( typeValueMap::const_iterator itObs = ...'

         s << endl;

      }  // End of 'for( satTypeValueMap::const_iterator it = ...'

         // Let's return the 'std::ostream'
      return s;

   }  // End of method 'satTypeValueMap::dump()'



      // stream output for satTypeValueMap
   std::ostream& operator<<( std::ostream& s,
                             const satTypeValueMap& stvMap )
   {

      stvMap.dump(s);
      return s;

   }  // End of 'operator<<'



      // stream output for gnssDataMap
   std::ostream& operator<<( std::ostream& s,
                             const gnssDataMap& gdsMap)
   {

      gdsMap.dump(s);
      return s;

   }  // End of 'operator<<'


      // Stream input for gnssRinex
   std::istream& operator >> (std::istream& i, gnssRinex& f)
   {
       if (Rinex3ObsStream::isRinex3ObsStream(i))     // Rinex3
       {
           Rinex3ObsStream& strm = dynamic_cast<Rinex3ObsStream&>(i);

           // If the header hasn't been read, read it...
           if (!strm.headerRead) strm >> strm.header;

           // Clear out this object
           Rinex3ObsHeader& roh = strm.header;

           Rinex3ObsData rod;
           strm >> rod;

           // Fill data
           f.header.source.type = SatIDsystem2SourceIDtype(roh.fileSysSat);
           f.header.source.sourceName = roh.markerName;
           f.header.antennaType = roh.antType;
           f.header.antennaPosition = roh.antennaPosition;
           f.header.epochFlag = rod.epochFlag;
           f.header.epoch = rod.time;

           f.body = satTypeValueMapFromRinex3ObsData(roh, rod);

           return i;
       }

       if (RinexObsStream::isRinexObsStream(i))    // Rinex2
       {
           try
           {
               RinexObsStream& strm = dynamic_cast<RinexObsStream&>(i);

               // If the header hasn't been read, read it...
               if (!strm.headerRead) strm >> strm.header;

               // Clear out this object
               RinexObsHeader& roh = strm.header;

               RinexObsData rod;
               strm >> rod;

               // Fill data
               f.header.source.type = SatIDsystem2SourceIDtype(roh.system);
               f.header.source.sourceName = roh.markerName;
               f.header.antennaType = roh.antType;
               f.header.antennaPosition = roh.antennaPosition;
               f.header.epochFlag = rod.epochFlag;
               f.header.epoch = rod.time;

               f.body = satTypeValueMapFromRinexObsData(roh, rod);

               return i;
           }
           catch (...)
           {
               return i;
           }
       }

       return i;

   }  // End of stream input for gnssRinex


   // Stream output for gnssRinex
   std::ostream& operator<<( std::ostream& s,
                             gnssRinex& f )
      throw(FFStreamError, gpstk::StringUtils::StringException)
   {
      FFStream* ffs = dynamic_cast<FFStream*>(&s);
      if(ffs)
      {
         try
         {
            RinexObsStream& strm = dynamic_cast<RinexObsStream&>(*ffs);

            // Clear out this object
            RinexObsHeader& hdr = strm.header;

            RinexObsData rod;

            rod.time = f.header.epoch;
            rod.epochFlag = f.header.epochFlag;
            rod.numSvs = f.numSats();
            rod.clockOffset = 0.0;
            rod.auxHeader = hdr;

            SatIDSet satSet = f.getSatID();
            for(SatIDSet::iterator itSat = satSet.begin();
               itSat != satSet.end();
               ++itSat)
            {
               vector<RinexObsType>::iterator obsTypeItr =
                  hdr.obsTypeList.begin();

               while (obsTypeItr != strm.header.obsTypeList.end())
               {
                  TypeID type = ConvertToTypeID( *obsTypeItr,
                     RinexSatID(itSat->id,itSat->system));

                  RinexDatum data;
                  data.data = f.body[*itSat][type];
                  data.ssi = 0;
                  data.lli = 0;

                  if( (type == TypeID::P1) || (type == TypeID::L1) )
                  {
                     if(type == TypeID::L1)
                     {
                        data.data /= L1_WAVELENGTH_GAL;
                        data.ssi = f.body[*itSat][TypeID::SSI1];
                     }

                     data.lli = f.body[*itSat][TypeID::LLI1];
                  }

                  if( (type == TypeID::P2) || (type == TypeID::L2))
                  {
                     if(type == TypeID::L2)
                     {
                        data.data /= L2_WAVELENGTH_GPS;
                        data.ssi = f.body[*itSat][TypeID::SSI2];
                     }

                     data.lli = f.body[*itSat][TypeID::LLI2];
                  }

                  if( (type == TypeID::C5) || (type == TypeID::L5))
                  {
                     if(type == TypeID::L5)
                     {
                        data.data /= L5_WAVELENGTH_GAL;
                        data.ssi = f.body[*itSat][TypeID::SSI5];
                     }

                     data.lli = f.body[*itSat][TypeID::LLI5];
                  }

                  if( (type == TypeID::C6) || (type == TypeID::L6) )
                  {
                     if(type == TypeID::L6)
                     {
                        data.data /= L6_WAVELENGTH_GAL;
                        data.ssi = f.body[*itSat][TypeID::SSI6];
                     }

                     data.lli = f.body[*itSat][TypeID::LLI6];
                  }

                  if( (type == TypeID::C7) || (type == TypeID::L7) )
                  {
                     if(type == TypeID::L7)
                     {
                        data.data /= L7_WAVELENGTH_GAL;
                        data.ssi = f.body[*itSat][TypeID::SSI7];
                     }

                     data.lli = f.body[*itSat][TypeID::LLI7];
                  }

                  if( (type == TypeID::C8) || (type == TypeID::L8))
                  {
                     if(type == TypeID::L8)
                     {
                        data.data /= L8_WAVELENGTH_GAL;
                        data.ssi = f.body[*itSat][TypeID::SSI8];
                     }

                     data.lli = f.body[*itSat][TypeID::LLI8];
                  }

                  if( (type == TypeID::D1) ||
                      (type == TypeID::S1) ||
                      (type == TypeID::C1) )
                  {
                     data.lli = f.body[*itSat][TypeID::LLI1];
                  }

                  if( (type == TypeID::D2) || (type == TypeID::S2) )
                  {
                     data.lli = f.body[*itSat][TypeID::LLI2];
                  }


                  rod.obs[*itSat][*obsTypeItr] = data;

                  obsTypeItr++;
               }
            }

            strm << rod;

            return s;

         }  // End of "try" block
         ////
         //// ATENTION: This part is VERY UGLY
         ////   Help from the guy who wrote
         ////        FFStream::tryFFStreamGet(FFData& rec)
         ////   will be very appreciated
         ////
         // EOF - do nothing - eof causes fail() to be set which
         // is handled by std::fstream
         catch (EndOfFile& e)
         {
            return s;
         }
         catch (...)
         {
            return s;
         }

      }  // End of block: "if (ffs)..."
      else
      {
         FFStreamError e("operator<< stream argument must be an FFStream");
         GPSTK_THROW(e);
      }

   }   // End of stream input for gnssRinex


      // Convenience function to convert from SatID system to SourceID type.
      // @param sid Satellite ID.
   SourceID::SourceType SatIDsystem2SourceIDtype(const SatID& sid)
   {

         // Select the right system the data came from
      switch(sid.system)
      {
         case SatID::systemGPS:
            return SourceID::GPS;
            break;
         case SatID::systemGalileo:
            return SourceID::Galileo;
            break;
         case SatID::systemGlonass:
            return SourceID::Glonass;
            break;
         case SatID::systemGeosync:
            return SourceID::Geosync;
            break;
         case SatID::systemLEO:
            return SourceID::LEO;
            break;
         case SatID::systemTransit:
            return SourceID::Transit;
            break;
         case SatID::systemMixed:
            return SourceID::Mixed;
            break;
         default:
            return SourceID::Unknown;
      }

   } // End SatIDsystem2SourceIDtype(const SatID& sid)



      // Convenience function to fill a satTypeValueMap with data
      // from RinexObsData.
      /// @param roh RinexObsHeader holding the data
      // @param rod RinexObsData holding the data.
   satTypeValueMap satTypeValueMapFromRinexObsData(
                            const RinexObsHeader& roh, const RinexObsData& rod)
   {

         // We need to declare a satTypeValueMap
      satTypeValueMap theMap;

         // Let's define the "it" iterator to visit the observations PRN map
         // RinexSatMap is a map from SatID to RinexObsTypeMap:
         //      std::map<SatID, RinexObsTypeMap>
      for( RinexObsData::RinexSatMap::const_iterator it = rod.obs.begin();
           it!= rod.obs.end();
           ++it )
      {
            // RinexObsTypeMap is a map from RinexObsType to RinexDatum:
            //   std::map<RinexObsType, RinexDatum>
            // The "second" field of a RinexSatMap (it) is a
            // RinexObsTypeMap (otmap)
         RinexObsData::RinexObsTypeMap otmap = (*it).second;
         SatID sat = (*it).first;

         typeValueMap tvMap;

         // Let's visit the RinexObsTypeMap (RinexObsType -> RinexDatum)
         for( RinexObsData::RinexObsTypeMap::const_iterator itObs = otmap.begin();
            itObs!= otmap.end();
            ++itObs )
         {

            RinexSatID rsat(sat.id,sat.system);

            TypeID type = ConvertToTypeID( itObs->first, rsat);

            const bool isPhase = IsCarrierPhase(itObs->first);
            const int n = GetCarrierBand(itObs->first);

            if(isPhase)
            {
               // TODO:: handle glonass data later(yanweigps)
               tvMap[ type ] = (*itObs).second.data*getWavelength(rsat,n);

               // n=1 2 5 6 7 8
               if(n==1)
               {
                  tvMap[TypeID::LLI1] = (*itObs).second.lli;
                  tvMap[TypeID::SSI1] = (*itObs).second.ssi;
               }
               else if(n==2)
               {
                  tvMap[TypeID::LLI2] = (*itObs).second.lli;
                  tvMap[TypeID::SSI2] = (*itObs).second.ssi;
               }
               else if(n==5)
               {
                  tvMap[TypeID::LLI5] = (*itObs).second.lli;
                  tvMap[TypeID::SSI5] = (*itObs).second.ssi;
               }
               else if(n==6)
               {
                  tvMap[TypeID::LLI6] = (*itObs).second.lli;
                  tvMap[TypeID::SSI6] = (*itObs).second.ssi;
               }
               else if(n==7)
               {
                  tvMap[TypeID::LLI7] = (*itObs).second.lli;
                  tvMap[TypeID::SSI7] = (*itObs).second.ssi;
               }
               else if(n==8)
               {
                  tvMap[TypeID::LLI8] = (*itObs).second.lli;
                  tvMap[TypeID::SSI8] = (*itObs).second.ssi;
               }
            }
            else
            {
               tvMap[ type ] = (*itObs).second.data;
            }

         }  // End of "for( itObs = otmap.

         theMap[sat] = tvMap;
      }

      return theMap;

   } // End FillsatTypeValueMapwithRinexObsData(const RinexObsData& rod)


      // Convenience function to fill a satTypeValueMap with data
      // from Rinex3ObsData.
      // @param roh Rinex3ObsHeader holding the data
      // @param rod Rinex3ObsData holding the data.
   satTypeValueMap satTypeValueMapFromRinex3ObsData(
       const Rinex3ObsHeader& roh, const Rinex3ObsData& rod)
   {
       // We need to declare a satTypeValueMap
       satTypeValueMap theMap;

       Rinex3ObsData::DataMap::const_iterator it;
       for (it = rod.obs.begin(); it != rod.obs.end(); it++)
       {
           RinexSatID sat(it->first);


           typeValueMap tvMap;

           map<std::string, std::vector<RinexObsID> > mapObsTypes(roh.mapObsTypes);
           const vector<RinexObsID> types = mapObsTypes[sat.toString().substr(0, 1)];
           try
           {
               for (size_t i = 0; i < types.size(); i++)
               {
                   TypeID type = ConvertToTypeID(types[i], sat);

                   const int n = GetCarrierBand(types[i]);

                   if (types[i].type == ObsID::otPhase)   // Phase
                   {
                       // TODO:: handle glonass data later(yanweigps)

                       int fcn = static_cast<SatID>(sat).getGloFcn();
                       tvMap[type] = it->second[i].data*getWavelength(sat, n, fcn);




                       // n=1 2 5 6 7 8
                       if (n == 1)
                       {
                           tvMap[TypeID::LLI1] = it->second[i].lli;
                           tvMap[TypeID::SSI1] = it->second[i].ssi;
                       }
                       else if (n == 2)
                       {
                           tvMap[TypeID::LLI2] = it->second[i].lli;
                           tvMap[TypeID::SSI2] = it->second[i].ssi;
                       }
                       else if (n == 5)
                       {
                           tvMap[TypeID::LLI5] = it->second[i].lli;
                           tvMap[TypeID::SSI5] = it->second[i].ssi;
                       }
                       else if (n == 6)
                       {
                           tvMap[TypeID::LLI6] = it->second[i].lli;
                           tvMap[TypeID::SSI6] = it->second[i].ssi;
                       }
                       else if (n == 7)
                       {
                           tvMap[TypeID::LLI7] = it->second[i].lli;
                           tvMap[TypeID::SSI7] = it->second[i].ssi;
                       }
                       else if (n == 8)
                       {
                           tvMap[TypeID::LLI8] = it->second[i].lli;
                           tvMap[TypeID::SSI8] = it->second[i].ssi;
                       }
                   }
                   else
                   {
                       tvMap[type] = it->second[i].data;
                   }
               }
           }
           catch (gpstk::InvalidRequest &e)
           {
               continue;
           }
           theMap[sat] = tvMap;
       }   // End loop over all the satellite

       return theMap;
   }

}  // End of namespace gpstk
