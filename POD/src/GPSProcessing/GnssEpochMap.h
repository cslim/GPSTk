#ifndef POD_GNSS_EPOCH_H
#define POD_GNSS_EPOCH_H

#include"DataStructures.hpp"
#include"CommonTime.hpp"

namespace pod
{
    struct GnssEpoch
    {

        GnssEpoch();

        GnssEpoch(const gpstk::satTypeValueMap&  sData);
        GnssEpoch(const gpstk::gnssRinex& gRin);
        ~GnssEpoch();

        gpstk::satTypeValueMap satData;

        gpstk::typeValueMap slnData;

        std::ostream& GnssEpoch::dump(std::ostream& s, int precision = 4);
  
    };

    struct GnssEpochMap
    {

        GnssEpochMap();

        ~GnssEpochMap();

        //dump object to a stream
        std::ostream& dump(std::ostream& s, int precision = 4);
        
        void updateMetadata();
        std::string title;
        //all sv in data 
        std::set<gpstk::SatID> svs;

        gpstk::Rinex3ObsHeader header;
        std::map<gpstk::CommonTime, GnssEpoch> data;

    };


}

#endif // !POD_GNSS_EPOCH_H
