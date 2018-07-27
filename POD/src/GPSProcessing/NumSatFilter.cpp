#include "NumSatFilter.h"

using namespace gpstk;
using namespace std;

namespace pod
{

    std::map<SlnType, int> NumSatFilter:: minSatbySsForSt
    {
        { SlnType::Standalone,1 },
        { SlnType::CODE_DIFF,1 },
        { SlnType::PD_Float,1 },
        { SlnType::PD_Fixed,2 },
        { SlnType::PPP_Float,1 },
    };

    gnssSatTypeValue & pod::NumSatFilter::Process(gnssSatTypeValue & gData)
    {
        throw UnimplementedException( getClassName()+"::Process(gnssSatTypeValue & gData) has not been implemented.");
    }

    gnssRinex & pod::NumSatFilter::Process(gnssRinex & gData)
    {
        auto svs = gData.getSatID();
        map<SatID::SatelliteSystem, int> counter;

        for_each(svs.begin(), svs.end(), [&counter](const SatID& sv) {counter[sv.system]++; });
        SatSystSet ssset;
        for (auto&& it: counter)
        {
            if(it.second>=minSvNum)
                ssset.insert(it.first);
        }
        gData.keepOnlySatSystems(ssset);
        return gData;
    }

    std::string pod::NumSatFilter::getClassName(void) const
    {
        return "pod::NumSatFilter";
    }
}
