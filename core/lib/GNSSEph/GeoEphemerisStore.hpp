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

/// @file GeoEphemerisStore.hpp
/// Get Geostationary SV broadcast ephemeris data information

#ifndef GPSTK_GEOEPHEMERISSTORE_HPP
#define GPSTK_GEOEPHEMERISSTORE_HPP

#include <iostream>
#include "XvtStore.hpp"
#include "GeoEphemeris.hpp"
#include "Rinex3NavData.hpp"
#include "WGS84Ellipsoid.hpp"
#include "Vector.hpp"
#include "YDSTime.hpp"
#include "TimeSystemCorr.hpp"

namespace gpstk
{
    /// @ingroup GNSSEph
    //@{

    // First, let's declare some useful type definitions

    typedef std::map<CommonTime, GeoEphemeris> TimeGeoMap;

    typedef std::map<SatID, TimeGeoMap> GeoEphMap;

    /**
    * This adds the interface to get Geostationary SV broadcast ephemeris information
    */
    class GeoEphemerisStore : public XvtStore<SatID>
    {
    public:

        /// Default constructor
        GeoEphemerisStore()
            : initialTime(CommonTime::END_OF_TIME),
            finalTime(CommonTime::BEGINNING_OF_TIME),
            step(1.0), checkHealthFlag(false)
        { };

        /** Common constructor
        *
        * @param rkStep     Runge-Kutta integration step in seconds.
        * @param checkHealth   Enable or disable the use of the health bit.
        */
        GeoEphemerisStore(double rkStep,
            double checkHealth)
            : initialTime(CommonTime::END_OF_TIME),
            finalTime(CommonTime::BEGINNING_OF_TIME),
            step(rkStep), checkHealthFlag(checkHealth), validInterval(900.0)
        { };

        /// Destructor
        virtual ~GeoEphemerisStore() {};

        /// Add ephemeris information from a Rinex3NavData object.
        /// @return true unless the data was not added.
        bool addEphemeris(const Rinex3NavData& data);

        /** Returns the position, velocity and clock offset of the indicated
        *  satellite in ECEF coordinates (meters) at the indicated time,
        *  in the WGS-84 ellipsoid.
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
        Xvt getXvt(const SatID& sat,
            const CommonTime& epoch) const;

        /// Get integration step for Runge-Kutta algorithm.
        double getIntegrationStep() const
        {
            return step;
        };

        /** Set integration step for Runge-Kutta algorithm.
        *
        * @param rkStep  Runge-Kutta integration step in seconds.
        */
        GeoEphemerisStore& setIntegrationStep(double rkStep)
        {
            step = rkStep; return (*this);
        };

        /// Get whether satellite health bit will be used or not.
        bool getCheckHealthFlag() const
        {
            return checkHealthFlag;
        };

        /** Set whether satellite health bit will be used or not.
        *
        * @param checkHealth   Enable or disable the use of the health bit.
        */
        GeoEphemerisStore& setCheckHealthFlag(bool checkHealth)
        {
            checkHealthFlag = checkHealth; return (*this);
        };

        /** A debugging function that outputs in human readable form,
        *  all data stored in this object.
        *
        * @param[in] s The stream to receive the output; defaults to cout
        * @param[in] detail The level of detail to provide
        *
        * @warning Geostationary SV position, velocity and acceleration
        * information are given in km, km/s and km/(s*s),
        * respectively.
        */
        virtual void dump(std::ostream& s = std::cout, short detail = 0) const;

        /** Edit the dataset, removing data outside the indicated
        * time interval
        *
        * @param[in] tmin   Defines the beginning of the time interval
        * @param[in] tmax   Defines the end of the time interval
        */
        virtual void edit(const CommonTime& tmin,
            const CommonTime& tmax = CommonTime::END_OF_TIME);

        /// Clear the dataset, meaning remove all data
        virtual void clear(void)
        {
            pe.clear();
            initialTime = CommonTime::END_OF_TIME;
            finalTime = CommonTime::BEGINNING_OF_TIME;
            return;
        };

        /// Return time system (NB assumed always to be GPS)
        virtual TimeSystem getTimeSystem(void) const
        {
            return TimeSystem::GPS;
        }

        /** Determine the earliest time for which this object can successfully
        *  determine the Xvt for any object.
        *
        * @return The initial time.
        *
        * @throw InvalidRequest This is thrown if the object has no data.
        */
        virtual CommonTime getInitialTime() const;

        /** Determine the latest time for which this object can successfully
        *  determine the Xvt for any object.
        *
        * @return The final time.
        *
        * @throw InvalidRequest This is thrown if the object has no data.
        */
        virtual CommonTime getFinalTime() const;

        /// Compute initial time for the given satellite
        CommonTime getInitialTime(const SatID& sat) const;

        /// Compute final time for the given satellite
        CommonTime getFinalTime(const SatID& sat) const;

        /// Return true if velocity data is present in the store. 
        /// Geostationary SV  ephemeris data always include velocity.
        virtual bool hasVelocity() const
        {
            return true;
        };

        /// Return true if the given SatID is present in the store
        virtual bool isPresent(const SatID& id) const;

        /// Return the number of satellites present in the store
        unsigned size(void) const
        {
            return pe.size();
        }

        /** Find the corresponding Geostationary SV ephemeris
        *for the given epoch.
        *
        * @param sat     SatID of satellite of interest.
        * @param epoch   time with which to search for ephemeris.
        *
        * @return a reference to the desired ephemeris.
        * @throw InvalidRequest object thrown when no ephemeris is found.
        */
        const GeoEphemeris& findEphemeris(const SatID& sat,
            const CommonTime& epoch) const;

        /** Find the corresponding Geostationary SV ephemeris 
        * for the given epoch.
        *
        * @param sat SatID of satellite of interest.
        * @param t time with which to search for ephemeris.
        *
        * @return a reference to the desired ephemeris.
        * @throw InvalidRequest object thrown when no ephemeris is found.
        *
        * @note This method is added for consistency with classes
        * GPSEphemerisStore.hpp and GalEphemerisStore.hpp, because in Geostationary SV
        * the returned ephemeris is ALWAYS the nearest one.
        */
        const GeoEphemeris& findUserEphemeris(const SatID& sat,
            const CommonTime& t) const
        {
            return findEphemeris(sat, t);
        };

        /** Find the corresponding Geostationary SV ephemeris 
        * for the given epoch.
        *
        * @param sat SatID of satellite of interest.
        * @param t time with which to search for ephemeris.
        *
        * @return a reference to the desired ephemeris.
        * @throw InvalidRequest object thrown when no ephemeris is found.
        *
        * @note This method is added for consistency with classes
        * GPSEphemerisStore.hpp and GalEphemerisStore.hpp, because in Geostationary SV
        * the returned ephemeris is ALWAYS the nearest one.
        */
        const GeoEphemeris& findNearEphemeris(const SatID& sat,
            const CommonTime& t) const
        {
            return findEphemeris(sat, t);
        };

        /// Add all ephemerides to an existing list<GeoEphemeris>.
        /// @return the number of ephemerides added.
        int addToList(std::list<GeoEphemeris>& v) const;

        void setValidInterval(double dt)
        {
            validInterval = dt;
        }

        double getValidInterval(void) const
        {
            return validInterval;
        }

    private:

        /// The map of SVs and Xvt's
        GeoEphMap pe;

        /// Earliest epoch of data available
        CommonTime initialTime;

        /// Latest epoch of data available
        CommonTime finalTime;

        /// Integration step for Runge-Kutta algorithm (1 second by default)
        double step;

        /// Flag signaling if satellites will be screened out according to
        /// their health bit (by default it is false)
        bool checkHealthFlag;

        ///
        double validInterval;

    };  // End of class 'GeoEphemerisStore'

        //@}

}  // End of namespace gpstk

#endif   // GPSTK_GEOEPHEMERISSTORE_HPP
