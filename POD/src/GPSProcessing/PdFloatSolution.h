#pragma once
#include "GnssSolution.h"
namespace pod
{
    class PdFloatSolution :
        public GnssSolution
    {
    public:
        PdFloatSolution(GnssDataStore_sptr data_ptr);
        PdFloatSolution(GnssDataStore_sptr data_ptr, double  max_sigma);
        virtual ~PdFloatSolution();

        virtual std::string  fileName() const override
        {
            return opts().SiteBase + "-" + opts().SiteRover + "_" + slnType2Str.at(desiredSlnType());;
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

