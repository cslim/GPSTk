#pragma once
#include"TypeID.hpp"
#include"SatID.hpp"
#include"DataStructures.hpp"
#include"RinexEpoch.h"

#include<set>
namespace pod
{
    typedef std::map<gpstk::SatID::SatelliteSystem, gpstk::SatIDSet> System2SatSetMap;


    class FilterParameter
    {
    public:

        static System2SatSetMap get_sv_by_ss(const std::set<FilterParameter>& parameters);
        
        static gpstk::SatIDSet get_all_sv(const std::set<FilterParameter>& parameters);

        static gpstk::SatSystSet get_all_ss(const std::set<FilterParameter>& parameters);
        
        static gpstk::TypeIDSet get_all_types(const std::set<FilterParameter>& parameters);

        FilterParameter()
            :type(gpstk::TypeID::Unknown), sv(gpstk::SatID::dummy)
        { };

        FilterParameter(const gpstk::TypeID &obsType, const gpstk::SatID &sat) 
            :type(obsType), sv(sat)
        { };

        FilterParameter(const gpstk::TypeID &obsType)
            :type(obsType), sv(gpstk::SatID::dummy)
        { };

        FilterParameter(const FilterParameter& parameter)
            :type(parameter.type), sv(parameter.sv)
        { };

        ~FilterParameter() 
        {};

        FilterParameter& operator = (const FilterParameter& other)
        {
            // check for self-assignment
            if (&other == this)
                return *this;

            this->sv = other.sv;
            this->type = other.type;

            return *this;
        }

        inline bool operator == (const FilterParameter& parameter) const
        {
            return (this->type == parameter.type && this->sv == parameter.sv);
        }

		std::string toString() const;

        gpstk::TypeID type;

        gpstk::SatID sv;

    };

    inline bool operator < (const FilterParameter& parameter1, const FilterParameter& parameter2)
    {
        //first, compare the types
        //then, compare the satellites
        if (parameter1.type == parameter2.type)
            return parameter1.sv < parameter2.sv;
        else
            return parameter1.type < parameter2.type;
    }
    inline std::ostream& operator<<(std::ostream& os, const FilterParameter& par)
    {
		os << par.type;
		if (par.sv.isValid())
			os << ' ' << par.sv;

        return os;
    }

    typedef std::set<FilterParameter> ParametersSet;
}

