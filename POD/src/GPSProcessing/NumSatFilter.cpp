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



    IRinex & pod::NumSatFilter::Process(IRinex & gData)
    {
        auto svs = gData.getBody().getSatID();
        map<SatID::SatelliteSystem, int> counter;
        auto  & rejTableItem = rejectedSatsTable[gData.getHeader().epoch];
        for_each(svs.begin(), svs.end(), [&counter](const SatID& sv) {counter[sv.system]++; });
        SatSystSet ssset;
        for (auto&& it: counter)
        {
            if(it.second>=minSvNum)
                ssset.insert(it.first);
        }

        for (auto && sv : svs)
            if (ssset.find(sv.system) == ssset.end())
                rejTableItem.insert(sv);
        
        gData.getBody().keepOnlySatSyst(ssset);
        return gData;
    }

    std::string pod::NumSatFilter::getClassName(void) const
    {
        return "pod::NumSatFilter";
    }
}
