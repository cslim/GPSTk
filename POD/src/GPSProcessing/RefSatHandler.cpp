#include "RefSatHandler.h"
#include"WinUtils.h"

using namespace gpstk;

namespace pod
{
    SatIDSet RefSatHandler::getRefSats(gpstk::IRinex & gData)
    {
        SatIDSet results;
        SatSystSet satSystems;

        auto it_pre = gData.getBody().end();
        for (const auto& sv : gData.getBody())
            satSystems.insert(sv.first.system);

        for (const auto& ss : satSystems)
        {
            double maxEl = DBL_MIN;
            SatID sv_maxEl;
            for (const auto& sv : gData.getBody())
                if (sv.first.system == ss)
                {
                    double ei = sv.second->get_value().at(TypeID::elevation);
                    if (ei > maxEl)
                    {
                        maxEl = ei;
                        sv_maxEl = sv.first;
                    }
                }

            results.insert(sv_maxEl);
        }
        return results;
    }

    //
    Matrix<double> RefSatHandler::getSD2DDMatrix(
        gpstk::IRinex & gData, 
        const SatIDSet& svs,
        const SatSystSet& ss,
        gpstk::SatIDSet &refSvSet)
    {
        //get reference satellites for each satellite systems
        refSvSet = getRefSats(gData);

        auto currRefSv = refSvSet.begin();
        std::map<SatID::SatelliteSystem, SatIDSet> svs_by_ss;

        for_each(svs.begin(), svs.end(), [&svs_by_ss](const SatID & sv) {svs_by_ss[sv.system].insert(sv); });

        //prepare single to double differences transition matrix
        Matrix<double> SD2DD(svs.size() - svs_by_ss.size(), svs.size(), .0);

        size_t n(0);
        size_t k(0);
        for (const auto& sv : svs_by_ss)
        {
            if (currRefSv == refSvSet.end()) break;
            int ref_sv_index = std::distance(sv.second.begin(), sv.second.find(*currRefSv++));
            int num_svs = sv.second.size();

            for (size_t j = n; j < n + num_svs - 1; j++)
            {
                SD2DD(j, ref_sv_index + n + k) = -1.0;
                if (j + k < ref_sv_index + n + k)
                    SD2DD(j, j + k) = 1.0;
                else 
                    SD2DD(j, j + 1 + k) = 1.0;
            }

            n += num_svs - 1;
            k++;
        }
        return SD2DD;
    }

}