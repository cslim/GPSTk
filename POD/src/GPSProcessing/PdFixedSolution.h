#pragma once
#include"PdFloatSolution.h"

namespace pod
{
    class PdFixedSolution :
        public PdFloatSolution
    {
    public:
    public:
        PdFixedSolution(GnssDataStore_sptr data_ptr);
        virtual ~PdFixedSolution();

        virtual std::string  fileName() const override
        {
            return data->SiteBase + "-" + data->SiteRover + "_pd_fixed.txt";
        }

        virtual SlnType desiredSlnType() const override
        {
            return SlnType::PD_Fixed;
        }

    };
}

