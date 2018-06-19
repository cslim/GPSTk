#include "FilterParameter.h"
using namespace gpstk;
namespace pod
{
    System2SatSetMap FilterParameter::get_sv_by_ss(const std::set<FilterParameter>& ambs)
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
    
    SatIDSet FilterParameter::get_all_sv(const std::set<FilterParameter>& ambs)
    {
        SatIDSet svs;
        for (const auto& amb : ambs)
            svs.insert(amb.sv);

        return svs;
    }

    SatSystSet FilterParameter::get_all_ss(const std::set<FilterParameter>& ambs)
    {
        SatSystSet sss;
        for (const auto& amb : ambs)
            sss.insert(amb.sv.system);

        return sss;
    }

    TypeIDSet FilterParameter::get_all_types(const std::set<FilterParameter>& ambs)
    {
        TypeIDSet types;
        for (const auto& amb : ambs)
            types.insert(amb.type);

        return types;
    }
}