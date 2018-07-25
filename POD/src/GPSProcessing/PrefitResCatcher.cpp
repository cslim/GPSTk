#include "PrefitResCatcher.h"
#include"WinUtils.h"

using namespace gpstk;
namespace pod
{
    bool getDetection(const Vector<double> sample, int excl, double maxRatio, double& ratio)
    {
        int s = sample.size();
        double avg(0);
        for (size_t i = 0; i < s; i++)
            if (i != excl)
                avg += sample[i];
        avg /= s - 1;

        double resExcl = abs(avg - sample[excl]);
        double resAvg(0);

        for (size_t i = 0; i < s; i++)
            if (i != excl)
                resAvg += abs(avg - sample[i]);
        resAvg /= s - 1;

        ratio = resExcl / resAvg;

        return(ratio > maxRatio);
    }

    gnssSatTypeValue & PrefitResCatcher::Process(gnssSatTypeValue & gData)
    {
        // TODO: insert return statement here
        throw UnimplementedException("pod::PrefitResCatcher::Process(gnssSatTypeValue & gData) has not been implemented.");
    }

    gnssRinex & PrefitResCatcher::Process(gnssRinex & gData)
    {
        
        for (auto && tid:resTypes)
        {
            int s = gData.body.size();
            auto svs = gData.getVectorOfSatID();
            auto values = gData.getVectorOfTypeID(tid);
            double ratio(0);
            for (size_t i = 0; i < values.size(); i++)
            {
                if (getDetection(values, i, 100, ratio))
                {
                    DBOUT_LINE(getClassName()<<" "<<svs[i]<<" "<<tid<<" "<<ratio)
                    gData.removeSatID(svs[i]);
                    break;
                }
            }
        }
        return gData;
    }

    std::string PrefitResCatcher::getClassName(void) const
    {
        return "pod::PrefitResCatcher";
    }
}