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

        virtual PositionEquations& setStochasicModel(gpstk::TypeID, gpstk::StochasticModel_sptr newModel);
       
        virtual gpstk::TypeIDSet getEquationTypes() const override
        {
            return types;
        }

        virtual void Prepare(gpstk::gnssRinex& gData);

        virtual void updateEquationTypes(gpstk::gnssRinex& gData, gpstk::TypeIDSet& eq) override;

        virtual void updatePhi(gpstk::Matrix<double>& Phi, int& index) const override;

        virtual void updateQ(gpstk::Matrix<double>& Q, int& index) const override;

        virtual void defStateAndCovariance(gpstk::Vector<double>& x, gpstk::Matrix<double>& P, int& index) const override;

        virtual int getNumUnknowns() const override;

    protected:

        std::map<gpstk::TypeID, gpstk::StochasticModel_sptr> stochasticModels;

        gpstk::TypeIDSet types;

    };
}
