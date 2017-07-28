#ifndef POD_GNSS_EPOCH_H
#define POD_GNSS_EPOCH_H
#include<iostream>

#include"DataStructures.hpp"
#include"CommonTime.hpp"

using namespace gpstk;

namespace pod
{
    struct GnssEpoch
    {

        GnssEpoch();

        GnssEpoch(const satTypeValueMap&  sData);
        GnssEpoch(const gnssRinex& gRin);
        ~GnssEpoch();

        satTypeValueMap satData;

        typeValueMap slnData;

        std::ostream& GnssEpoch::dump(std::ostream& s, int precision = 4);
  
    };

    struct GnssEpochMap
    {

        GnssEpochMap();

        ~GnssEpochMap();

        Rinex3ObsHeader header;
        std::map<CommonTime, GnssEpoch> data;
        std::ostream& dump(std::ostream& s, int precision = 4);
    };


}

#endif // !POD_GNSS_EPOCH_H
