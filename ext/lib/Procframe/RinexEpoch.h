#pragma once
#include"DataStructures.hpp"
#include"DataHeaders.hpp"

#include<memory>

namespace gpstk
{
   
    typedef SatID::SatelliteSystem GpstkSatSystem;

    //template<typename T>
    class ITypeValueMap
    {
      
    public:
        virtual ~ITypeValueMap() {};
        virtual typeValueMap& get_value() = 0;
    };

    class TypeValueMapPtr: public ITypeValueMap
    {
    public:
        TypeValueMapPtr(typeValueMap * p) :ptr(p) {};
        TypeValueMapPtr(typeValueMap & p) :ptr(&p) {};

        virtual typeValueMap& get_value()
        {
            return *ptr;
        };

        std::shared_ptr<typeValueMap> ptr;
    };

    typedef std::map<SatID, std::unique_ptr<ITypeValueMap >> SatTypePtrMap;

    class IRinex
    {
    public:
        virtual SatTypePtrMap& get_value() = 0;
    };

    class RinexEpoch: public IRinex
    {

    public:

        friend std::istream& operator>>(std::istream& i, RinexEpoch& f);

        RinexEpoch( );
        RinexEpoch(const gnssRinex & gRin);

        virtual ~RinexEpoch();

        void resetCurrData();

        SatTypePtrMap& get_value()
        {
            return currData;
        };

        const SatTypePtrMap& get_value() const
        {
            return currData;
        };

        sourceEpochRinexHeader& getHeader()
        {
            return rinex.header;
        }

        const sourceEpochRinexHeader& getHeader() const
        {
            return rinex.header;
        }

        RinexEpoch extractSatID(const SatID& satellite) const;

        RinexEpoch extractSatID(const int& p, const GpstkSatSystem& s) const;

        RinexEpoch extractSatID(const SatIDSet& satSet) const;

        RinexEpoch extractSatSyst(const SatSystSet& satSet) const;

        RinexEpoch& keepOnlySatID(const SatID& satellite);

        RinexEpoch& keepOnlySatID(const int& p, const GpstkSatSystem& s);

        RinexEpoch& keepOnlySatID(const SatIDSet& satSet);

        RinexEpoch& keepOnlySatSystems(GpstkSatSystem satSyst);

        RinexEpoch& keepOnlySatSystems(const SatSystSet& satSet);

        RinexEpoch extractTypeID(const TypeID& type) const;

        RinexEpoch extractTypeID(const TypeIDSet& typeSet) const;

        RinexEpoch& keepOnlyTypeID(const TypeID& type);

        RinexEpoch& keepOnlyTypeID(const TypeIDSet& typeSet);

    protected:
        SatTypePtrMap currData;

        gnssRinex rinex;

    };
}


