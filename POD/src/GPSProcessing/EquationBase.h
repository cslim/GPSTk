#pragma once
#include"DataStructures.hpp"

namespace pod
{
    class EquationBase
    {
    public:
        EquationBase()
        {};
        virtual ~EquationBase() {};

        virtual  void Prepare(gpstk::gnssRinex& gData) = 0;

        virtual void updateEquationTypes(gpstk::gnssRinex& gData, gpstk::TypeIDSet& eqTypes) = 0;
        
        virtual  gpstk::TypeIDSet getEquationTypes() const = 0;

        virtual void updatePhi(gpstk::Matrix<double>& Phi, int& index) const = 0;

        virtual void updateQ(gpstk::Matrix<double>& Q, int& index) const = 0;

        virtual void defStateAndCovariance(gpstk::Vector<double>& x, gpstk::Matrix<double>& P, int& index) const = 0;

        //virtual void updateH(gpstk::Matrix<double>& H, int& index) = 0;

        virtual int getNumUnknowns() const = 0;

    };
}
