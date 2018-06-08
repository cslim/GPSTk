#include "Ambiguity.h"
using namespace gpstk;
namespace pod
{
    System2SatSetMap Ambiguity::get_sv_by_ss(const std::set<Ambiguity>& ambs)
    {
        System2SatSetMap svs_by_ss;
        SatIDSet svs;
        for (const auto& amb : ambs)
        {
            svs.insert(amb.sv);
            svs_by_ss[amb.sv.system].insert(amb.sv);
        }
        return svs_by_ss;

    }
    
    SatIDSet Ambiguity::get_all_sv(const std::set<Ambiguity>& ambs)
    {
        SatIDSet svs;
        for (const auto& amb : ambs)
            svs.insert(amb.sv);

        return svs;
    }

    SatSystSet Ambiguity::get_all_ss(const std::set<Ambiguity>& ambs)
    {
        SatSystSet sss;
        for (const auto& amb : ambs)
            sss.insert(amb.sv.system);

        return sss;
    }

    TypeIDSet Ambiguity::get_all_types(const std::set<Ambiguity>& ambs)
    {
        TypeIDSet types;
        for (const auto& amb : ambs)
            types.insert(amb.type);

        return types;
    }
}