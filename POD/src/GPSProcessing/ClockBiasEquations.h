#pragma once
#include "EquationBase.h"
#include"StochasticModel.hpp"

namespace pod
{
    class ClockBiasEquations :
        public EquationBase
    {
    public:
        ClockBiasEquations();
        virtual ~ClockBiasEquations(){};

        virtual ClockBiasEquations& setStochasicModel(gpstk::StochasticModel_sptr newModel);

        // Inherited via EquationBase
        virtual void Prepare(gpstk::gnssRinex & gData) override;

        virtual void updateEquationTypes(gpstk::gnssRinex& gData, gpstk::TypeIDSet & eqTypes) override;

        virtual gpstk::TypeIDSet getEquationTypes() const override
        {
            return gpstk::TypeIDSet({ type });
        }
        virtual void updatePhi(gpstk::Matrix<double>& Phi, int & index) const override;
        
        virtual void updateQ(gpstk::Matrix<double>& Q, int & index) const override;
        
        virtual void defStateAndCovariance(gpstk::Vector<double>& x, gpstk::Matrix<double>& P, int & index) const override;
        
        virtual int getNumUnknowns() const override;
    protected:

        gpstk::TypeID type;
        gpstk::StochasticModel_sptr stochModel;
    };
}

