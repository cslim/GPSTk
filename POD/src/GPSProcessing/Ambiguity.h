#pragma once
#include"TypeID.hpp"
#include"SatID.hpp"
#include"DataStructures.hpp"

#include<set>
namespace pod
{
    typedef std::map<gpstk::SatID::SatelliteSystem, gpstk::SatIDSet> System2SatSetMap;


    struct Ambiguity
    {
    public:
       // static bool operator < (const Ambiguity& amb1, const Ambiguity& amb2);


        static System2SatSetMap get_sv_by_ss(const std::set<Ambiguity>& ambs);
        static gpstk::SatIDSet get_all_sv(const std::set<Ambiguity>& ambs);
        static gpstk::SatSystSet get_all_ss(const std::set<Ambiguity>& ambs);
        static gpstk::TypeIDSet get_all_types(const std::set<Ambiguity>& ambs);

        Ambiguity(const gpstk::TypeID &obsType, const gpstk::SatID &sat) 
            :type(obsType), sv(sat)
        { };

        Ambiguity(const Ambiguity& amb)
            :type(amb.type), sv(amb.sv)
        { };

        ~Ambiguity() 
        {};

        Ambiguity operator = (const Ambiguity& amb)
        {
            return Ambiguity(amb);
        }

        inline bool operator == (const Ambiguity& amb)
        {
            return (this->type == amb.type && this->sv == sv);
        }

        gpstk::TypeID type;

        gpstk::SatID sv;
    };

    inline bool operator < (const Ambiguity& amb1, const Ambiguity& amb2)
    {
        //first, compare the types
        //then, compare the satellites
        if (amb1.type == amb2.type)
            return amb1.sv < amb2.sv;
        else
            return amb1.type < amb2.type;
    }

    typedef std::set<Ambiguity> AmbiguitySet;
}

