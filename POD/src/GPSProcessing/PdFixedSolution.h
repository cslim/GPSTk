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


        virtual SlnType desiredSlnType() const override
        {
            return SlnType::PD_Fixed;
        }

    };
}

