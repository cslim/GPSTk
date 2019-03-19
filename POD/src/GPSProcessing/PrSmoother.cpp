#include "PrSmoother.h"
#include<filesystem>

#include"CommonTime.hpp"
#include"Rinex3ObsStream.hpp"
#include"Rinex3ObsData.hpp"
#include"Exception.hpp"
#include"DataStructures.hpp"
#include"CodeSmoother.hpp"
#include"OneFreqCSDetector.hpp"
#include"LICSDetector2.hpp"

namespace fs = std::experimental::filesystem;

typedef std::map<TypeID, int> band_stat ;
typedef std::map<SatID, std::map<TypeID, int>> sv_stat;
typedef std::pair<TypeID, int> band_stat_pair;
namespace pod
{
    PrSmoother::PrSmoother() : window(100), codes(std::list<TypeID>(TypeID::C1))
    {
    }

    PrSmoother::PrSmoother(const std::list<TypeID>& tList, int l) : window(l), codes(tList)
    {
    }

    PrSmoother::~PrSmoother()
    {
    }

    void PrSmoother::smooth(const char * path)
    {

        fs::path iPath(path);
        fs::path oPath(iPath);

        fs::path stem = oPath.stem();
        stem += "_sm";
        fs::path ext = iPath.extension();

        oPath.replace_filename(stem);
        oPath.replace_extension(ext);
        fs::path sPath = iPath;
        sPath.replace_extension("out");

        std::cout << "Rinex file whith raw PR: " << iPath << std::endl;

        std::list<CodeSmoother> smList;
        // We MUST mark cycle slips
        std::list<OneFreqCSDetector> csList;

		std::cout << "Obs. currParameters for smoothing: " << std::endl;
        for (auto &it : codes)
        {
			std::cout << TypeID::tStrings[it.type] << std::endl;
            smList.push_back(CodeSmoother(it, window));
            csList.push_back(OneFreqCSDetector(it));
        }


        RinexObsStream rin(iPath.string());
        RinexObsStream rout(oPath.string(), std::ios::out);
		std::ofstream fStat(sPath, std::ios::out);

        RinexObsHeader head;
        RinexEpoch gRin;


        rin >> head;
        rout << head;
        sv_stat stat;
        while (rin >> gRin)
        {
            for (auto &it : csList)
                gRin >> it;

            bool isEpochFirstTime = true;
            for (auto &it : gRin.getBody())
            {
                bool isSVFirstTime = true;
                for (auto& it1 : csList)
                {
                    auto csType = it1.getResultType();
                    double CS = it.second->get_value().getValue(csType);
                    if (CS > 0)
                    {
                        if (isEpochFirstTime)
                        {
                            fStat << CivilTime(gRin.getHeader().epoch) << " " << std::endl;
                            isEpochFirstTime = false;
                        }

                        if (isSVFirstTime)
                        {
                            fStat << it.first << " ";
                            isSVFirstTime = false;
                        }

                        auto s_it = stat.find(it.first);
                        if (s_it == stat.end())
                            stat.emplace(it.first, band_stat({ band_stat_pair(csType, 1) }));
                        else
                        {
                            auto b_it = (*s_it).second.find(csType);
                            if (b_it == (*s_it).second.end())
                                (*s_it).second.emplace(csType, 1);
                            else
                                b_it->second++;
                        }
                        fStat << TypeID::tStrings[csType.type] << " ";
                    }
                }
                if (!isSVFirstTime)
                    fStat << std::endl;
            }
            if (!isEpochFirstTime)
                fStat << std::endl;
            for (auto &it : smList)
                gRin >> it;

            rout << gRin;
        }


		std::cout << "Rinex file whith smoothed PR: " << oPath << std::endl;
		std::cout << "File for CS statistic PR: " << sPath << std::endl;


        for (auto &it : stat)
        {
            fStat << it.first << " ";
            for (auto &it1 : it.second)
            {
                fStat << it1.first << " " << it1.second << " ";
            }
            fStat << std::endl;
        }

        rin.close();
        rout.close();
        fStat.close();
    }
}