#include"ProcessLinear.h"
#include"LinearCombination.h"
#include<map>

using namespace std;
namespace pod
{
    gpstk::satTypeValueMap& ProcessLinear::Process(gpstk::satTypeValueMap& gData)
    {
        for (auto& sv : gData)
        {
            for (auto& lc : this->combs)
            {
                double value;
                if (lc->getCombination(sv.first, sv.second, value))
                {
                    sv.second.insert(make_pair(lc->getType(), value));
                }
            }
        }
        return gData;
    }

    gpstk::gnssRinex& ProcessLinear::Process(gpstk::gnssRinex& gData)
    {
        Process(gData.body);
        return gData;
    }

    gpstk::gnssSatTypeValue& ProcessLinear::Process(gpstk::gnssSatTypeValue& gData)
    {
        Process(gData.body);
        return gData;
    }
}
