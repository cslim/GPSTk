#pragma once
#include"ProcessingClass.hpp"
#include"GnssDataStore.hpp"

namespace pod
{
    class NumSatFilter
        :public gpstk::ProcessingClass
    {
    private:
        static std::map<SlnType, int> minSatbySsForSt;
    public:
        NumSatFilter(SlnType slnType) :minSvNum(minSatbySsForSt.at(slnType)) {};
        virtual ~NumSatFilter() {};

        // Inherited via ProcessingClass
        virtual gpstk::gnssSatTypeValue & Process(gpstk::gnssSatTypeValue & gData) override;

        virtual gpstk::gnssRinex & Process(gpstk::gnssRinex & gData) override;
        
        virtual std::string getClassName(void) const override;

        int minSvNum;

    };

}
