#pragma once
#include"TypeID.hpp"
#include"SatID.hpp"
#include"DataStructures.hpp"

#include<set>
namespace pod
{
    typedef std::map<gpstk::SatID::SatelliteSystem, gpstk::SatIDSet> System2SatSetMap;


    struct FilterParameter
    {
    public:
       // static bool operator < (const FilterParameter& amb1, const FilterParameter& amb2);


        static System2SatSetMap get_sv_by_ss(const std::set<FilterParameter>& ambs);
        static gpstk::SatIDSet get_all_sv(const std::set<FilterParameter>& ambs);
        static gpstk::SatSystSet get_all_ss(const std::set<FilterParameter>& ambs);
        static gpstk::TypeIDSet get_all_types(const std::set<FilterParameter>& ambs);

        FilterParameter(const gpstk::TypeID &obsType, const gpstk::SatID &sat) 
            :type(obsType), sv(sat)
        { };

        FilterParameter(const FilterParameter& amb)
            :type(amb.type), sv(amb.sv)
        { };

        ~FilterParameter() 
        {};

        FilterParameter operator = (const FilterParameter& amb)
        {
            return FilterParameter(amb);
        }

        inline bool operator == (const FilterParameter& amb)
        {
            return (this->type == amb.type && this->sv == sv);
        }

        gpstk::TypeID type;

        gpstk::SatID sv;
    };

    inline bool operator < (const FilterParameter& amb1, const FilterParameter& amb2)
    {
        //first, compare the types
        //then, compare the satellites
        if (amb1.type == amb2.type)
            return amb1.sv < amb2.sv;
        else
            return amb1.type < amb2.type;
    }

    typedef std::set<FilterParameter> ParametersSet;
}

