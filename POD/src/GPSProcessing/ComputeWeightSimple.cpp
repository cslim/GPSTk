#include "ComputeWeightSimple.h"
#include<math.h>
using namespace std;
using namespace gpstk;

namespace pod
{
     const double ComputeWeightSimple::elev0 = 30.0;

     const double ComputeWeightSimple::sin0 = (std::sin(elev0 * DEG_TO_RAD));

     //const double ComputeWeightSimple::glnSigmaFactor = 2;

    ComputeWeightSimple::~ComputeWeightSimple()
    {
    }

    IRinex& ComputeWeightSimple::Process(IRinex& gData)
    {
        Process(gData.getBody());
        return gData;
    }

    gpstk::SatTypePtrMap& ComputeWeightSimple::Process(gpstk::SatTypePtrMap& gData)
    {
        SatIDSet rejSatSet;
        for (auto& it: gData)
        {
            auto el = it.second->get_value().find(TypeID::elevation);
            if (el != it.second->get_value().end() && el->second >0)
            {
               
                double invsig = 1.0;
                if (el->second < elev0)
                    invsig = (::sin(el->second * DEG_TO_RAD) / sin0);
                    //invsig = 0.5/(::sin(el->second * DEG_TO_RAD) / sin0);

                double factor = (it.first.system == SatID::systemGlonass) ? glnSigmaFactor : 1;
                
                it.second->get_value()[TypeID::weight] = invsig*invsig/factor;
            }
            else
            {
                rejSatSet.insert(it.first);
            } 
        }
        
        return gData;
    }

}
