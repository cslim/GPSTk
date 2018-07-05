#pragma once
#include"StochasticModel.hpp"
namespace pod
{
    class IonoStochasticModel :
        public gpstk::RandomWalkModel
    {
    public:
        IonoStochasticModel() {};
        
        IonoStochasticModel(double qp,
            const gpstk::CommonTime& prevTime = gpstk::CommonTime::BEGINNING_OF_TIME,
            const gpstk::CommonTime& currentTime = gpstk::CommonTime::BEGINNING_OF_TIME)
            : RandomWalkModel(qp, prevTime, prevTime) {};

        virtual ~IonoStochasticModel() {};

        /** This method provides the stochastic model with all the available
        *  information and takes appropriate actions.
        *
        * @param sat        Satellite.
        * @param gData      Data object holding the data.
        *
        */
        virtual void Prepare(const  gpstk::SatID& sat, gpstk::gnssRinex& gData) override;

        virtual double getQ() const override;
        virtual double getPhi() const override;
    private:
        double el;
        bool csFlag;

    };
}

