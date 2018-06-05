#include "RefSatHandler.h"
#include"WinUtils.h"

using namespace gpstk;

namespace pod
{
    RefSatHandler::RefSatHandler()
    {
    }

    SatIDSet RefSatHandler::getRefSats(gpstk::gnssRinex & gData)
    {
        SatIDSet results;
        SatSystSet satSystems;
        
        auto it_pre = gData.body.end();
        for (const auto& sv : gData.body)
            satSystems.insert(sv.first.system);
        

        for (const auto& ss : satSystems)
        {
            double maxEl = DBL_MIN;
            SatID sv_maxEl;
            for (const auto& sv : gData.body)
                if (sv.first.system == ss)
                {
                    double ei = sv.second.at(TypeID::elevation);
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
    Matrix<double> RefSatHandler::getSD2DDMatrix(gpstk::gnssRinex & gData, const AmbiguitySet& ambs)
    {
        //get reference satellites for each satellite systems
        const auto refSVs = getRefSats(gData);


        std::list<int> indeces;
        auto curr_sv = refSVs.begin();
        std::map<SatID::SatelliteSystem, SatIDSet> svs_by_ss;
        SatIDSet svs;
        for (const auto& amb : ambs)
        {
            svs.insert(amb.sv);
            svs_by_ss[amb.sv.system].insert(amb.sv);
        }

        //prepare single to double differences transition matrix
        Matrix<double> SD2DD(svs.size() - refSVs.size(), svs.size(), .0);

        int n(0);
        int k(0);
        for (const auto& sv : svs_by_ss)
        {
            int ref_sv_index = std::distance(sv.second.begin(), sv.second.find(*curr_sv++));
            int num_svs = sv.second.size();

            for (size_t j = n; j < n + num_svs - 1; j++)
            {
                SD2DD(j, ref_sv_index+n+k) = -1.0;
                if (j+k < ref_sv_index + n+k) SD2DD(j, j+k) = 1.0;
                else    SD2DD(j, j + 1+k) = 1.0;
               
            }
           
            n += num_svs - 1;
            k++;
        }
        return SD2DD;
    }

}