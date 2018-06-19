#pragma once
#include "EquationBase.h"
#include"StochasticModel.hpp"

namespace pod
{
    class AmbiguitySdEquations :
        public EquationBase
    {
    public:
        

        AmbiguitySdEquations():type(gpstk::TypeID::BL1){};
        AmbiguitySdEquations(const gpstk::TypeID& obsType) :type(obsType) {};

        virtual ~AmbiguitySdEquations() {};

        // Inherited via EquationBase
        virtual void Prepare(gpstk::gnssRinex & gData) override;

        virtual void updateEquationTypes(gpstk::gnssRinex & gData, gpstk::TypeIDSet & eqTypes) override;

        virtual gpstk::TypeIDSet getEquationTypes() const override;

        virtual void updatePhi(gpstk::Matrix<double>& Phi, int & index) const override;

        virtual void updateQ(gpstk::Matrix<double>& Q, int & index) const override;

        virtual void defStateAndCovariance(gpstk::Vector<double>& x, gpstk::Matrix<double>& P, int & index) const override;

        virtual int getNumUnknowns() const override;

        virtual  ParametersSet getAmbSet() const override;

        virtual void updateH(gpstk::gnssRinex& gData, gpstk::Matrix<double>& H, int& col_0, int& row_0) override;

    private:

        /// type ID of carrier phase measurements (or combination of measurements) 
        gpstk::TypeID type;

        ///salellites set to be preocessed
        gpstk::SatIDSet satSet;

        ///current satellites set (subset of 'satSet')
        gpstk::SatIDSet currentSatSet;

        ///cycle slip flags for satellites in 'satSet' 
        std::map<gpstk::SatID, bool> csFlags;

        /// phase ambiguity stochasic model
        mutable gpstk::PhaseAmbiguityModel stochModel;

    };
}
