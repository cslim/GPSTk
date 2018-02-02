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

        virtual gpstk::gnssSatTypeValue& Process(gpstk::gnssSatTypeValue& gData) override;

        virtual gpstk::gnssRinex& Process(gpstk::gnssRinex& gData) override;

        gpstk::satTypeValueMap& Process(gpstk::satTypeValueMap& gData);

        virtual std::string getClassName(void) const override { return "ComputeWeightSimple"; }

    };
}

