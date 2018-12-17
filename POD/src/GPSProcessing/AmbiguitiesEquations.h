#pragma once
#include "EquationBase.h"
#include"StochasticModel.hpp"

namespace pod
{
    class AmbiguitiesEquations :
        public EquationBase
    {
         
    public:
        static const double sigma;

        AmbiguitiesEquations()
            :type(gpstk::TypeID::BL1), obsType(typeMap[gpstk::TypeID::BL1])
        {
            stochModel.setSigma(sigma);
        };

        AmbiguitiesEquations(const gpstk::TypeID& obsType)
            :type(obsType), obsType(typeMap[obsType])
        {
            stochModel.setSigma(sigma);
        };

        virtual ~AmbiguitiesEquations() {};

        // Inherited via EquationBase
        virtual void Prepare(gpstk::IRinex & gData) override;

        virtual void updatePhi(gpstk::Matrix<double>& Phi, int & index) const override;

        virtual void updateQ(gpstk::Matrix<double>& Q, int & index) const override;

        virtual void defStateAndCovariance(gpstk::Vector<double>& x, gpstk::Matrix<double>& P, int & index) const override;

        virtual int getNumUnknowns() const override;

        virtual  ParametersSet getParameters() const override;

        virtual void updateH(const gpstk::IRinex& gData, const gpstk::TypeIDSet& types, gpstk::Matrix<double>& H, int& col_0) override;

        virtual ParametersSet getAmbSet() const override
        {
            return getParameters();
        }

    private:

        /// type ID of unknown values 
        gpstk::TypeID type;

        /// type ID of carrier phase measurements (or combination of measurements) 
        gpstk::TypeID obsType;
        
        ///salellites set to be preocessed
        gpstk::SatIDSet satSet;

        ///current satellites set (subset of 'satSet')
        gpstk::SatIDSet svsInView;

        ///cycle slip flags for satellites in 'satSet' 
        std::map<gpstk::SatID, bool> csFlags;

        /// phase ambiguity stochasic model
        mutable gpstk::PhaseAmbiguityModel stochModel;

        static std::map< gpstk::TypeID, gpstk::TypeID> typeMap;

        class Initializer
        {
        public:
            Initializer();
        };
        static Initializer initializer;
    };
}
