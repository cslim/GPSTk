#pragma once
#include "ProcessingClass.hpp"
namespace pod
{
    class ComputeWeightSimple :
        public gpstk::ProcessingClass
    {
    public:

        static const double elev0;

        static const double sin0;

        static const double glnSigmaFactor;


        ComputeWeightSimple();
        virtual ~ComputeWeightSimple();


        virtual gpstk::IRinex& Process(gpstk::IRinex& gData) override;

        gpstk::SatTypePtrMap& Process(gpstk::SatTypePtrMap& gData);

        virtual std::string getClassName(void) const override { return "ComputeWeightSimple"; }

    };
}

