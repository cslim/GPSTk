#pragma once
#include "GnssSolution.h"
namespace pod
{
    class PppFloatSolution :
        public GnssSolution
    {
    public:
        PppFloatSolution(GnssDataStore_sptr data_ptr);
        PppFloatSolution(GnssDataStore_sptr data_ptr, double  max_sigma);
        virtual ~PppFloatSolution() {};

        virtual std::string  fileName() const override
        {
            return  opts().SiteRover +"_" +slnType2Str.at(desiredSlnType());
        }

        virtual SlnType desiredSlnType() const override
        {
            return SlnType::PPP_Float;
        }

        virtual void process() override;

    protected:
        virtual void updateRequaredObs() override;

        void configureSolver();

        ProcessLinear OminusC;

    };
}
