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
#include "SatID.hpp"
#include "GNSSconstants.hpp"
#include"Rinex3NavStream.hpp"
#include"Rinex3NavData.hpp"
/**
* @file SatID.cpp
* gpstk::SatID - navigation system-independent representation of a satellite.
*/

namespace gpstk
{
    std::map<SatID, int> SatID::glonassFcn;

    SatID SatID::dummy;

    int SatID::getGloFcn() const
    {
        if (system != SatelliteSystem::systemGlonass)
            return 0;
        auto it = glonassFcn.find(*this);
        if (it == glonassFcn.end())
        {
            InvalidRequest ip("Unknown FCN for Glonass satellite: " + this->id);
            GPSTK_THROW(ip);
        }

        return it->second;
    }
    void SatID::loadGloFcn(const char * path)
    {
        Rinex3NavStream rNavFile;
        Rinex3NavHeader rNavHeader;

        rNavFile.open(path, std::ios::in);
        rNavFile >> rNavHeader;
        Rinex3NavData nm;
        while (rNavFile >> nm)
            SatID::glonassFcn[SatID(nm.sat)] = nm.freqNum;
    }
}