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
using namespace std;
namespace fs = std::experimental::filesystem;

typedef std::map<TypeID, int> band_stat ;
typedef std::map<SatID, std::map<TypeID, int>> sv_stat;
typedef std::pair<TypeID, int> band_stat_pair;

PrSmoother::PrSmoother(): window(100), codes(std::list<TypeID>(TypeID::C1))
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

    cout << "Rinex file whith raw PR: " << iPath << endl;

    cout << "Obs. types for smoothing: " << endl;

    for (auto &it : codes)
        cout << TypeID::tStrings[it.type] << endl;

    RinexObsStream rin(iPath.string());
    RinexObsStream rout(oPath.string(), ios::out);
    ofstream fStat(sPath, ios::out);

    RinexObsHeader head;
    gnssRinex gRin;

    // We MUST mark cycle slips
    int smL = 100;
    std::list<CodeSmoother> smList;
    for (auto &it : codes)
        smList.push_back(CodeSmoother(it, window));

    std::list<OneFreqCSDetector> csList;
    for (auto &it : codes)
        csList.push_back(OneFreqCSDetector(it));

    rin >> head;
    rout << head;
    sv_stat stat;
    while (rin >> gRin)
    {
        for (auto &it : csList)
            gRin >> it;

        bool isEpochFirstTime = true;
        for (auto &it : gRin.body)
        {
            bool isSVFirstTime = true;
            for (auto& it1 : csList)
            {

                auto csType = it1.getResultType();
                double CS = it.second.getValue(csType);
                if (CS > 0)
                {
                    if (isEpochFirstTime)
                    {
                        fStat << CivilTime(gRin.header.epoch) << " " << endl;
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
                fStat << endl;
        }
        if (!isEpochFirstTime)
            fStat << endl;
        for (auto &it : smList)
            gRin >> it;

        rout << gRin;
    }


    cout << "Rinex file whith smoothed PR: " << oPath << endl;
    cout << "File for CS statistic PR: " << sPath << endl;


    for (auto &it : stat)
    {
        fStat << it.first <<" ";
        for (auto &it1 : it.second)
        {
            fStat << it1.first << " " << it1.second<<" ";
        }
        fStat << endl;
    }

    rin.close();
    rout.close();
    fStat.close();
}