#include"ProcessLinear.h"
#include"LinearCombination.h"
#include<map>

using namespace std;
namespace pod
{
    void ProcessLinear::setUseC1(bool useC1)
    {
        this->useC1 = useC1;
        for (auto& lc : this->combs)
            lc->setUseC1(useC1);
    }

    gpstk::satTypeValueMap& ProcessLinear::Process(gpstk::satTypeValueMap& gData)
    {
        for (auto& sv : gData)
        {
            for (const auto& lc : this->combs)
            {
                double value;
                if (lc->getCombination(sv.first, sv.second, value))
                    sv.second.insert(make_pair(lc->getType(), value));
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
