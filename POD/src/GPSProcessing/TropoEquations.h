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
            :types({ gpstk::TypeID::wetMap }),
            pStochasticModel(std::make_unique<gpstk::RandomWalkModel>())
        {};
        TropoEquations(double qPrime) ;
        virtual ~TropoEquations() {};

#pragma region Inherited via EquationBase

        virtual void Prepare(gpstk::gnssRinex & gData) override;

        virtual void updateEquationTypes(gpstk::gnssRinex & gData, gpstk::TypeIDSet & eqTypes) override;

        virtual gpstk::TypeIDSet getEquationTypes() const override;

        virtual void updatePhi(gpstk::Matrix<double>& Phi, int & index) const override;

        virtual void updateQ(gpstk::Matrix<double>& Q, int & index) const override;

        virtual void defStateAndCovariance(gpstk::Vector<double>& x, gpstk::Matrix<double>& P, int & index) const override;

        virtual int getNumUnknowns() const override;

#pragma endregion

        TropoEquations& setModel( gpstk::StochasticModel_uptr model)
        { pStochasticModel = std::move(model); return *this; }

        gpstk::StochasticModel getModel() const
        { return  *pStochasticModel; }

#pragma region Fields

        gpstk::StochasticModel_uptr pStochasticModel;

        gpstk::TypeIDSet types;

#pragma endregion

    };
}

