#ifndef POD_GNSS_EPOCH_H
#define POD_GNSS_EPOCH_H

#include"DataStructures.hpp"
#include"CommonTime.hpp"
#include"Position.hpp"

namespace pod
{
    struct GnssEpoch
    {

        GnssEpoch();

        GnssEpoch(const gpstk::satTypeValueMap&  sData);
        GnssEpoch(const gpstk::SatTypePtrMap&  stpMap);
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

        //all solution types
        std::set<int> slnTypes;

        //all typeIDs 
        gpstk::TypeIDSet types;

        gpstk::Rinex3ObsHeader header;

        std::map<gpstk::CommonTime, GnssEpoch> data;

        std::map<gpstk::CommonTime, GnssEpoch>::iterator begin() 
        {
            return data.begin();
        };

        std::map<gpstk::CommonTime, GnssEpoch>::iterator end()
        {
            return data.end();
        };

        std::map<gpstk::CommonTime, GnssEpoch>::const_iterator begin() const
        {
            return data.begin();
        };

        std::map<gpstk::CommonTime, GnssEpoch>::const_iterator end() const
        {
            return data.end();
        };
        
        std::map<gpstk::CommonTime, GnssEpoch>::reverse_iterator rbegin() 
        {
            return data.rbegin();
        };

        std::map<gpstk::CommonTime, GnssEpoch>::const_reverse_iterator rbegin() const
        {
            return data.rbegin();
        };

        std::map<gpstk::CommonTime, GnssEpoch>::reverse_iterator rend()
        {
            return data.rend();
        };

        std::map<gpstk::CommonTime, GnssEpoch>::const_reverse_iterator rend() const
        {
            return data.rend();
        };

        gpstk::CommonTime getInitialTime() const
        {
            if (data.size() == 0)
                GPSTK_THROW(gpstk::InvalidRequest("GnssEpochMap objects contais no elements"));

            return begin()->first;
        }

        gpstk::CommonTime getFinalTime() const
        {
            if (data.size() == 0)
                GPSTK_THROW(gpstk::InvalidRequest("GnssEpochMap objects contais no elements"));

            return rbegin()->first;
        }

        size_t size() const
        {
            return data.size();
        }

    protected: void updateTypes(const gpstk::TypeIDSet & types);
    };

} 



#endif // !POD_GNSS_EPOCH_H
