#pragma once
#include "ProcessingClass.hpp"
#include"LinearCombination.h"
#include"StochasticModel.hpp"

namespace pod
{
    class IonoEstimator :
        public gpstk::ProcessingClass
    {
    public:
        

        IonoEstimator();
        virtual ~IonoEstimator() {};

        virtual gpstk::gnssSatTypeValue& Process(gpstk::gnssSatTypeValue& gData);


        virtual gpstk::gnssRinex& Process(gpstk::gnssRinex& gData)
            throw(gpstk::ProcessingException);

        /// Returns a string identifying this object.
        virtual std::string getClassName(void) const override;

        IonoEstimator& clear();

        IonoEstimator& reset(const gpstk::SatID& sv);


    private:
        
        bool feed(const gpstk::SatID & sv, gpstk::gnssRinex& gData);
        struct KalmanData
        {
            KalmanData()
                :delay(.0), bias(.0),
                q11(1e4), q12(.0), q22(1e9)
            {};

            //state
            double delay;
            double bias;

            //variance-covariance
            double q11, q12, q22;
        };

        struct FilterData
        {
            FilterData()
            {};

            //state
            KalmanData state;
            //delay stochastic model - random walk model
            RandomWalkModel rWalkModel;
        };

        std::map<SatID, FilterData> filterData;

        CodeIonoDelayL1 lcIonoCode;

        PhaseIonoDelayL1 lcIonoPhase;

        gpstk::PhaseAmbiguityModel biasStochModel;

        class Initializer
        {
        public:
            Initializer();
            ~Initializer() {};
        };

        static Initializer initializer;
        static Matrix<double> W;
        static Matrix<double> H;
    public:
        static double maxGap;
    };
}

