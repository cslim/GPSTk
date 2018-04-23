#pragma once
#include "GnssSolution.h"
namespace pod
{
    class PdFloatSolution :
        public GnssSolution
    {
    public:
        PdFloatSolution(GnssDataStore_sptr data_ptr);
        virtual ~PdFloatSolution();

        virtual std::string  fileName() const override
        {
            return data->SiteBase + "-" + data->SiteRover + "_pd_float.txt";
        }
        
        virtual SlnType desiredSlnType() const override
        {
            return SlnType::PD_Float;
        }

        virtual void process() override;

    protected:
        virtual void updateRequaredObs() override;

        void configureSolver();

        ProcessLinear OminusC;

    };
}

