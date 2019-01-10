#pragma once
#include "EquationBase.h"
#include"StochasticModel.hpp"

namespace pod
{
    class TropoGradEquations :
        public EquationBase
    {
    public:
		TropoGradEquations()
			:types(
				{ FilterParameter(gpstk::TypeID::wetMap),
				FilterParameter(gpstk::TypeID::wetMapNorth),
				FilterParameter(gpstk::TypeID::wetMapEast)
				}),
			pStochModelZ(std::make_unique<gpstk::RandomWalkModel>()),
			pStochModelNorth(std::make_unique<gpstk::RandomWalkModel>()),
			pStochModelEast(std::make_unique<gpstk::RandomWalkModel>())
        {};

		TropoGradEquations(double qPrimeZ, double qPrimeN, double qPrimeE);

        virtual ~TropoGradEquations() {};

#pragma region Inherited via EquationBase

        virtual void Prepare(gpstk::IRinex & gData) override;

        virtual void updateH(const gpstk::IRinex& gData, const gpstk::TypeIDSet& types, gpstk::Matrix<double>& H, int& col_0) override;
        
        virtual  ParametersSet getParameters() const override
        { return types; }
        
        virtual void updatePhi(gpstk::Matrix<double>& Phi, int & index) const override;

        virtual void updateQ(gpstk::Matrix<double>& Q, int & index) const override;

        virtual void defStateAndCovariance(gpstk::Vector<double>& x, gpstk::Matrix<double>& P, int & index) const override;

        virtual int getNumUnknowns() const override;

#pragma endregion

        TropoGradEquations& setModel( gpstk::StochasticModel_uptr model)
        { pStochModelZ = std::move(model); return *this; }

        gpstk::IStochasticModel* getModel() const
        { return pStochModelZ.get(); }

#pragma region Fields

        gpstk::StochasticModel_uptr pStochModelZ;
        gpstk::StochasticModel_uptr pStochModelNorth;
        gpstk::StochasticModel_uptr pStochModelEast;

		ParametersSet types;

#pragma endregion

    };
}

