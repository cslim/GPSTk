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
        ClockBiasEquations(double sigma);
        virtual ~ClockBiasEquations(){};

        virtual ClockBiasEquations& setStochasicModel(gpstk::StochasticModel_sptr newModel);

        // Inherited via EquationBase
        virtual void Prepare(gpstk::gnssRinex & gData) override;
        virtual void updateH(const gpstk::gnssRinex& gData, const gpstk::TypeIDSet& types, gpstk::Matrix<double>& H, int& col_0) override;
      
        virtual ParametersSet getParameters() const override
        {
            return ParametersSet({ type });
        }
        virtual void updatePhi(gpstk::Matrix<double>& Phi, int & index) const override;
        
        virtual void updateQ(gpstk::Matrix<double>& Q, int & index) const override;
        
        virtual void defStateAndCovariance(gpstk::Vector<double>& x, gpstk::Matrix<double>& P, int & index) const override;
        
        virtual int getNumUnknowns() const override;
    protected:

        FilterParameter type;
        gpstk::StochasticModel_sptr stochModel;
    };
}

