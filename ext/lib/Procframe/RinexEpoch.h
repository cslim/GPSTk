#pragma once
#include"DataStructures.hpp"
#include"DataHeaders.hpp"
#include"SatTypePtrMap.h"
#include<memory>

namespace gpstk
{
   
    typedef SatID::SatelliteSystem GpstkSatSystem;

    class IRinex
    {
    public:
		friend std::istream&  operator>>(std::istream& i, IRinex& f)
		{
			return f.read(i);
		}
		
		friend std::ostream&  operator<<(std::ostream& i, IRinex& f)
		{
			return f.print(i);
		}

		virtual std::unique_ptr<IRinex> clone() const =0;
		virtual std::istream& read(std::istream&) = 0;
		virtual std::ostream& print(std::ostream&) = 0;
        virtual sourceEpochRinexHeader& getHeader() = 0;
        virtual SatTypePtrMap& getBody() = 0;
		virtual const SatTypePtrMap& getBody() const = 0;
    };

    class RinexEpoch : public IRinex
    {

    public:

        RinexEpoch( );
        RinexEpoch(const gnssRinex & gRin);

        virtual ~RinexEpoch();

        void resetCurrData();

        SatTypePtrMap& getBody()
        {
            return currData;
        };

		virtual std::unique_ptr<IRinex> clone() const override
		{
			return std::make_unique<RinexEpoch>(*this);
		}

        const SatTypePtrMap& getBody() const
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

		std::istream& read(std::istream& i)
		{
			return i >> rinex;
		}

		std::ostream& print(std::ostream& i)
		{
			return i << rinex;
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


