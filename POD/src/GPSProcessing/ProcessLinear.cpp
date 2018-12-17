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

    gpstk::SatTypePtrMap& ProcessLinear::Process(gpstk::SatTypePtrMap& gData)
    {
        for (auto& sv : gData)
        {
            for (const auto& lc : this->combs)
            {
                double value;
				if (lc->getCombination(sv.first, sv.second->get_value(), value))
					sv.second->get_value().emplace(lc->getType(), value);
            }
        }
        return gData;
    }

    gpstk::IRinex& ProcessLinear::Process(gpstk::IRinex& gData)
    {
        Process(gData.getBody());
        return gData;
    }
}
