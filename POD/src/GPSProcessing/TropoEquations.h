#pragma once
#include "EquationBase.h"
#include"StochasticModel.hpp"

namespace pod
{
    class TropoEquations :
        public EquationBase
    {
    public:
        TropoEquations() 
            :type(gpstk::TypeID::wetMap),
            pStochasticModel(std::make_unique<gpstk::RandomWalkModel>())
        {};
        TropoEquations(double qPrime) ;
        virtual ~TropoEquations() {};

#pragma region Inherited via EquationBase

        virtual void Prepare(gpstk::IRinex & gData) override;

        virtual void updateH(const gpstk::IRinex& gData, const gpstk::TypeIDSet& types, gpstk::Matrix<double>& H, int& col_0) override;
        
        virtual  ParametersSet getParameters() const override
        {
            return ParametersSet{ type };
        }
        
        virtual void updatePhi(gpstk::Matrix<double>& Phi, int & index) const override;

        virtual void updateQ(gpstk::Matrix<double>& Q, int & index) const override;

        virtual void defStateAndCovariance(gpstk::Vector<double>& x, gpstk::Matrix<double>& P, int & index) const override;

        virtual int getNumUnknowns() const override;

#pragma endregion

        TropoEquations& setModel( gpstk::StochasticModel_uptr model)
        { pStochasticModel = std::move(model); return *this; }

        gpstk::IStochasticModel* getModel() const
        { return pStochasticModel.get(); }

#pragma region Fields

        gpstk::StochasticModel_uptr pStochasticModel;

        FilterParameter type;

#pragma endregion

    };
}

