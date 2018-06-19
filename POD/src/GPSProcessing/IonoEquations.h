#pragma once
#include "EquationBase.h"
#include"StochasticModel.hpp"

namespace pod
{

    class IonoEquations :
        public EquationBase
    {
    public:
        IonoEquations();
        IonoEquations(double qPrime);
        ~IonoEquations(){};

        /* update state of equations with new observational data */
        virtual  void Prepare(gpstk::gnssRinex& gData) override;

        /*Check, if unknown parameters currently observable, if so,
        put the corresponding TypeID into 'TypeIDSet'
        */
        virtual void updateEquationTypes(gpstk::gnssRinex& gData, gpstk::TypeIDSet& eqTypes) override;

        /* return set of TypeID, corresponding unknown parameters  for given equations */
        virtual  gpstk::TypeIDSet getEquationTypes() const override;


        /* Put the values in state tarnsition matrix, starting with specific index,
        index will be incremented inside this method
        */
        virtual void updatePhi(gpstk::Matrix<double>& Phi, int& index) const override;

        /*Put process noise components into corresponding matrix,
        starting with specific index, index will be incremented inside this method
        */
        virtual void updateQ(gpstk::Matrix<double>& Q, int& index) const override;

        /* Put default values of state vector and it's covariance into corresponding matrices,
        starting with specific index, index will be incremented inside this method
        */
        virtual void defStateAndCovariance(gpstk::Vector<double>& x, gpstk::Matrix<double>& P, int& index) const override;

        /* Put  partials of the measurements with respect to the unknowns into the design (geometry) matrix
        starting with specific indices, indices will be incremented inside this method
        */


        /* return number of unknowns
        */
        virtual int getNumUnknowns() const override;

#pragma region Fields

        gpstk::StochasticModel_uptr pStochasticModel;

        gpstk::TypeIDSet types;

#pragma endregion
    };
}

