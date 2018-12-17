#pragma once
#include "EquationBase.h"
#include"StochasticModel.hpp"

namespace pod 
{
    class PositionEquations :
        public EquationBase
    {
    public:
        //PositionEquations();
        PositionEquations(double posSigma = 100.0);
        virtual ~PositionEquations() {};

        virtual PositionEquations& setStochasicModel(gpstk::StochasticModel_sptr newModel);

        virtual PositionEquations& setStochasicModel(FilterParameter, gpstk::StochasticModel_sptr newModel);
       
        virtual ParametersSet getParameters() const override
        {
            return types;
        }

        virtual void Prepare(gpstk::IRinex& gData);

        virtual void updateH(const gpstk::IRinex& gData, const gpstk::TypeIDSet& types, gpstk::Matrix<double>& H, int& col_0) override;

        virtual void updatePhi(gpstk::Matrix<double>& Phi, int& index) const override;

        virtual void updateQ(gpstk::Matrix<double>& Q, int& index) const override;

        virtual void defStateAndCovariance(gpstk::Vector<double>& x, gpstk::Matrix<double>& P, int& index) const override;

        virtual int getNumUnknowns() const override;

    protected:

        std::map<FilterParameter, gpstk::StochasticModel_sptr> stochasticModels;

        ParametersSet types;

    };
}
