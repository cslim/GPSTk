#pragma once
#include"GnssEpochMap.h"
#include"GnssSolution.h"
#include<iostream>

namespace pod
{
    class ComputeStatistic
    {

    public:

        ComputeStatistic(SlnType st, gpstk::TypeIDSet tIDs)
            :slnType(st), types(tIDs)
        {};
        ~ComputeStatistic() {};
        
        SlnType getSlnType()
        {
            return slnType;
        }

        gpstk::TypeIDSet getTypeIDSet()
        {
            return types;
        }

        void compute(const GnssEpochMap& data, gpstk::Vector<double>& sln, gpstk::Matrix<double>& cov);


    private:

        SlnType slnType;
        gpstk::TypeIDSet types;
    };
}

