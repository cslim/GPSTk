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
//  Dagoberto Salazar - gAGE ( http://www.gage.es ). 2011
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
* @file GeoEphemerisStore.cpp
* Get GLONASS broadcast ephemeris data information
*/

#include "GeoEphemerisStore.hpp"
#include "TimeString.hpp"

using namespace std;

namespace gpstk
{


    // Add ephemeris information from a Rinex3NavData object.
    bool GeoEphemerisStore::addEphemeris(const Rinex3NavData& data)
    {

        // If enabled, check SV health before entering here (health = 0 -> OK)
        if ((data.health == 0) || (!checkHealthFlag))
        {
            // Get a GloEphemeris object from Rinex3NavData object
            GeoEphemeris gloEphem(data);

            CommonTime t(data.time);
            t.setTimeSystem(TimeSystem::GPS);   // must be GPS time

            SatID sat(data.sat);
            pe[sat][t] = gloEphem; // find or add entry

            if (t < initialTime)
                initialTime = t;
            else if (t > finalTime)
                finalTime = t;

            return true;

        }  // End of 'if( (data.health == 0) || (!checkHealthFlag) )'

        return false;

    }  // End of method 'GeoEphemerisStore::addEphemeris()'


       /* Returns the position, velocity and clock offset of the indicated
       * satellite in ECEF coordinates (meters) at the indicated time,
       * in the PZ-90 ellipsoid.
       *
       *  @param[in] sat   Satellite's identifier
       *  @param[in] epoch Time to look up
       *
       *  @return the Xvt of the object at the indicated time
       *
       *  @throw InvalidRequest If the request can not be completed for any
       *  reason, this is thrown. The text may have additional information
       *  as to why the request failed.
       */
    Xvt GeoEphemerisStore::getXvt(const SatID& sat,
        const CommonTime& epoch) const
    {
        // TD is this too strict?
        if (epoch.getTimeSystem() != initialTime.getTimeSystem())
        {
            InvalidRequest e(string("Requested time system is not GLONASS time"));
            GPSTK_THROW(e);
        }

        // Check that the given epoch is within the available time limits.
        // We have to add a margin of 15 minutes (900 seconds).
        if (epoch <  (initialTime - validInterval) ||
            epoch >(finalTime + validInterval))
        {
            InvalidRequest e("Requested time is out of boundaries for satellite "
                + StringUtils::asString(sat));
            GPSTK_THROW(e);
        }

        // Look for the satellite in the 'pe' (EphMap) data structure.
        GeoEphMap::const_iterator svmap = pe.find(sat);

        // If satellite was not found, issue an exception
        if (svmap == pe.end())
        {
            InvalidRequest e("Ephemeris for satellite  "
                + StringUtils::asString(sat) + " not found.");
            GPSTK_THROW(e);
        }

        // Let's take the second part of the EphMap
        const TimeGeoMap& sem = svmap->second;

        // Look for 'i': the first element whose key >= epoch.
        TimeGeoMap::const_iterator i = sem.lower_bound(epoch);;

        // Values to be returned will be stored here
        Xvt sv;

        // If we reached the end, the requested time is beyond the last
        // ephemeris record, but it may still be within the allowable time
        // span, so we can use the last record.
        if (i == sem.end())
        {
            i = --i;
        }

        // If key > (epoch+900), we must use the previous record if possible.
        if ((i->first > (epoch + validInterval)) && (i != sem.begin()))
        {
            i = --i;
        }

        // Check that the given epoch is within the available time limits for
        // this specific satellite, with a margin of 15 minutes (900 seconds).
        if (epoch <  (i->first - validInterval) ||
            epoch >= (i->first + validInterval))
        {
            InvalidRequest e("Requested time is out of boundaries for satellite "
                + StringUtils::asString(sat));
            GPSTK_THROW(e);
        }

        // We now have the proper reference data record. Let's use it
        GeoEphemeris data(i->second);

        data.setIntegrationStep(step);
        // Compute the satellite position, velocity and clock offset
        sv = data.svXvt(epoch);

        // We are done, let's return
        return sv;

    }; // End of method 'GeoEphemerisStore::getXvt()'


       /* A debugging function that outputs in human readable form,
       * all data stored in this object.
       *
       * @param[in] s      The stream to receive the output; defaults to cout
       * @param[in] detail The level of detail to provide
       *
       * @warning GLONASS position, velocity and acceleration information are
       * given in km, km/s and km/(s*s), respectively.
       */
    void GeoEphemerisStore::dump(std::ostream& s, short detail) const
    {
        static const string fmt("%04Y/%02m/%02d %02H:%02M:%02S %P");
        s << "Dump of GeoEphemerisStore:\n";

        //if(detail == 0 ) {
        s << " Span is " << (initialTime == CommonTime::END_OF_TIME
            ? "End_time" : printTime(initialTime, fmt))
            << " to " << (finalTime == CommonTime::BEGINNING_OF_TIME
                ? "Begin_time" : printTime(finalTime, fmt))
            << " with " << pe.size() << " entries; checkHealthFlag is "
            << (checkHealthFlag ? "T" : "F") << std::endl;

        //}
        //else
        if (detail > 0) {
            if (pe.size())

            // Iterate through all items in the 'pe' GeoEphMap
            for (GeoEphMap::const_iterator it = pe.begin(); it != pe.end(); ++it) {

                // Then, iterate through corresponding 'TimeGeoMap'
                for (TimeGeoMap::const_iterator tgmIter = (*it).second.begin();
                    tgmIter != (*it).second.end();
                    ++tgmIter)
                {

                    // First, print year, Day-Of-Year and Seconds of Day
                    s << printTime(tgmIter->first, fmt) << " ";

                    // Second, print SatID information
                    s << RinexSatID((*it).first) << " ";

                    // Third, print satellite ephemeris data
                    s << ((*tgmIter).second)<<endl;

                }  // End of 'for( TimeGeoMap::const_iterator tgmIter = ...'

            }  // End of 'for( GeoEphMap::const_iterator it = pe.begin(); ...'

        }  // End of 'else', i.e., detail != 0

        s << "  End of GeoEphemerisStore data." << std::endl;


    }; // End of method 'GeoEphemerisStore::dump()'


       /* Edit the dataset, removing data outside the indicated time interval
       *
       * @param[in] tmin   Defines the beginning of the time interval
       * @param[in] tmax   Defines the end of the time interval
       */
    void GeoEphemerisStore::edit(const CommonTime& tmin,
        const CommonTime& tmax)
    {

        // Create a working copy
        GeoEphMap bak;

        // Reset the initial and final times
        initialTime = CommonTime::END_OF_TIME;
        finalTime = CommonTime::BEGINNING_OF_TIME;

        // Iterate through all items in the 'bak' GeoEphMap
        for (GeoEphMap::const_iterator it = pe.begin();
            it != pe.end();
            ++it)
        {

            // Then, iterate through corresponding 'TimeGeoMap'
            for (TimeGeoMap::const_iterator tgmIter = (*it).second.begin();
                tgmIter != (*it).second.end();
                ++tgmIter)
            {

                CommonTime t((*tgmIter).first);

                // Check if the current record is within the given time interval
                if ((tmin <= t) && (t <= tmax))
                {

                    // If we are within the proper boundaries, let's add the data
                    GeoEphemeris data((*tgmIter).second);

                    SatID sat((*it).first);
                    bak[sat][t] = data;     // Add entry

                                            // Update 'initialTime' and 'finalTime', if necessary
                    if (t < initialTime)
                        initialTime = t;
                    else if (t > finalTime)
                        finalTime = t;

                }  // End of 'if ( ( (*tgmIter).first >= tmin ) && ...'

            }  // End of 'for( TimeGeoMap::const_iterator tgmIter = ...'

        }  // End of 'for( GeoEphMap::const_iterator it = pe.begin(); ...'

           // Update the data map before returning
        pe = bak;

        return;

    }; // End of method 'GeoEphemerisStore::edit()'


       // Determine the earliest time for which this object can successfully
       // determine the Xvt for any object.
       // @return The initial time
       // @throw InvalidRequest This is thrown if the object has no data.
    CommonTime GeoEphemerisStore::getInitialTime() const
    {
        return initialTime;

    }; // End of method 'GeoEphemerisStore::getInitialTime()'


       // Determine the latest time for which this object can successfully
       // determine the Xvt for any object.
       // @return The final time
       // @throw InvalidRequest This is thrown if the object has no data.
    CommonTime GeoEphemerisStore::getFinalTime() const
    {
        return finalTime;
    }; // End of method 'GeoEphemerisStore::getFinalTime()'

       // Compute initial time for the given satellite
    CommonTime GeoEphemerisStore::getInitialTime(const SatID& sat) const
    {
        CommonTime ret(CommonTime::END_OF_TIME);
        GeoEphMap::const_iterator svmap = pe.find(sat);
        if (svmap != pe.end()) {
            const TimeGeoMap& tgm = svmap->second;
            TimeGeoMap::const_iterator it = tgm.begin();
            ret = it->first;
        }
        return ret;
    }

    // Compute final time for the given satellite
    CommonTime GeoEphemerisStore::getFinalTime(const SatID& sat) const
    {
        CommonTime ret(CommonTime::BEGINNING_OF_TIME);
        GeoEphMap::const_iterator svmap = pe.find(sat);
        if (svmap != pe.end()) {
            const TimeGeoMap& tgm = svmap->second;
            TimeGeoMap::const_reverse_iterator rit = tgm.rbegin();
            ret = rit->first;
        }
        return ret;
    }

    /* Find the corresponding GLONASS ephemeris for the given epoch.
    *
    * @param sat SatID of satellite of interest.
    * @param t time with which to search for ephemeris.
    *
    * @return a reference to the desired ephemeris.
    * @throw InvalidRequest object thrown when no ephemeris is found.
    */
    const GeoEphemeris& GeoEphemerisStore::findEphemeris(const SatID& sat,
        const CommonTime& epoch) const
    {
        // Check that the given epoch is within the available time limits.
        // We have to add a margin of 15 minutes (900 seconds).
        if (epoch < (initialTime - validInterval) ||
            epoch >(finalTime + validInterval))
        {
            InvalidRequest e("Requested time is out of boundaries for satellite "
                + StringUtils::asString(sat));
            GPSTK_THROW(e);
        }

        // Look for the satellite in the 'pe' (EphMap) data structure.
        GeoEphMap::const_iterator svmap = pe.find(sat);

        // If satellite was not found, issue an exception
        if (svmap == pe.end())
        {
            InvalidRequest e("Ephemeris for satellite  "
                + StringUtils::asString(sat) + " not found.");
            GPSTK_THROW(e);
        }

        // Let's take the second part of the EphMap
        const TimeGeoMap& sem = svmap->second;

        // 'i' will be the first element whose key >= epoch.
        TimeGeoMap::const_iterator i = sem.lower_bound(epoch);

        // If we reached the end, the requested time is beyond the last
        // ephemeris record, but it may still be within the allowable time
        // span, so we can use the last record.
        if (i == sem.end())
        {
            i = --i;
        }

        // If key > (epoch+900), we must use the previous record if possible.
        if ((i->first > (epoch + validInterval)) && (i != sem.begin()))
        {
            i = --i;
        }

        // Check that the given epoch is within the available time limits for
        // this specific satellite, with a margin of 15 minutes (900 seconds).
        if (epoch < (i->first - validInterval) ||
            epoch >(i->first + validInterval))
        {
            InvalidRequest e("Requested time is out of boundaries for satellite "
                + StringUtils::asString(sat));
            GPSTK_THROW(e);
        }

        // We now have the proper reference data record. Let's return it
        return (i->second);

    }; // End of method 'GeoEphemerisStore::findEphemeris()'


       // Return true if the given SatID is present in the store
    bool GeoEphemerisStore::isPresent(const SatID& id) const
    {

        // Look for the satellite in the 'pe' (GeoEphMap) data structure.
        GeoEphMap::const_iterator svmap = pe.find(id);

        // If satellite was not found return false, else return true
        if (svmap == pe.end())
        {
            return false;
        }
        else
        {
            return true;
        }

    }; // End of method 'GeoEphemerisStore::isPresent(const SatID& id)'


    int GeoEphemerisStore::addToList(std::list<GeoEphemeris>& v) const
    {
        int n = 0;
        for (GeoEphMap::const_iterator it = pe.begin(); it != pe.end(); ++it)
        {
            for (TimeGeoMap::const_iterator tgmIter = (*it).second.begin();
                tgmIter != (*it).second.end(); ++tgmIter)
            {
                v.push_back(tgmIter->second);
                n++;
            }
        }
        return n;
    }


}  // End of namespace gpstk
