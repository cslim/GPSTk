#include "ComputeWeightSimple.h"
#include<math.h>
using namespace std;
using namespace gpstk;

namespace pod
{
     const double ComputeWeightSimple::elev0 = 30.0;

     const double ComputeWeightSimple::sin0 = (std::sin(elev0 * DEG_TO_RAD));

     const double ComputeWeightSimple::glnSigmaFactor = 1;

    ComputeWeightSimple::ComputeWeightSimple()
    {
    }


    ComputeWeightSimple::~ComputeWeightSimple()
    {
    }

    gnssSatTypeValue& ComputeWeightSimple::Process(gnssSatTypeValue& gData)
    {
        Process(gData.body);
        return gData;
    }

    gnssRinex& ComputeWeightSimple::Process(gnssRinex& gData)
    {
        Process(gData.body);
        return gData;
    }

    gpstk::satTypeValueMap& ComputeWeightSimple::Process(gpstk::satTypeValueMap& gData)
    {
        SatIDSet rejSatSet;
        for (auto& it: gData)
        {
            auto el = it.second.find(TypeID::elevation);
            if (el != it.second.end() && el->second >0)
            {
               
                double invsig = 1.0;
                if (el->second < elev0)
                    invsig = (::sin(el->second * DEG_TO_RAD) / sin0);

                double factor = (it.first.system == SatID::systemGlonass) ? glnSigmaFactor : 1;
                
                it.second[TypeID::weight] = invsig*invsig*factor;
            }
            else
            {
                rejSatSet.insert(it.first);
            } 
        }
        return gData;
    }

}
