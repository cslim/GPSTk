#include "FilterParameter.h"

using namespace gpstk;
namespace pod
{
    System2SatSetMap FilterParameter::get_sv_by_ss(const std::set<FilterParameter>& parameters)
    {
        System2SatSetMap svs_by_ss;
        SatIDSet svs;
        for (const auto& par : parameters)
        {
            svs.insert(par.sv);
            svs_by_ss[par.sv.system].insert(par.sv);
        }
        return svs_by_ss;

    }
    
    SatIDSet FilterParameter::get_all_sv(const std::set<FilterParameter>& parameters)
    {
        SatIDSet svs;
        for (const auto& par : parameters)
            svs.insert(par.sv);

        return svs;
    }

    SatSystSet FilterParameter::get_all_ss(const std::set<FilterParameter>& parameters)
    {
        SatSystSet sss;
        for (const auto& par : parameters)
            sss.insert(par.sv.system);

        return sss;
    }

    TypeIDSet FilterParameter::get_all_types(const std::set<FilterParameter>& parameters)
    {
        TypeIDSet types;
        for (const auto& par : parameters)
            types.insert(par.type);

        return types;
    }
	std::string FilterParameter::toString() const
	{
		std::ostringstream ss;
		ss << *this;
		return ss.str();
	}
}