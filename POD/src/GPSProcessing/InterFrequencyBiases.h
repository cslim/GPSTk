#pragma once
#include "EquationBase.h"
#include"StochasticModel.hpp"

namespace pod
{
    class InterFrequencyBiases :
        public EquationBase
    {
    private:
        static std::map< gpstk::SatID::SatelliteSystem, FilterParameter> ss2ifb;
        static std::map<FilterParameter, gpstk::SatID::SatelliteSystem> ifb2ss;
        static const  gpstk::TypeIDSet l2Types;

    public:
        InterFrequencyBiases();
        virtual ~InterFrequencyBiases() {};

        virtual  void Prepare(gpstk::gnssRinex& gData);

        void updateH(const gpstk::gnssRinex& gData, const gpstk::TypeIDSet& types, gpstk::Matrix<double>& H, int& col_0);

        virtual ParametersSet getParameters() const override
        {
            return types;
        }

        virtual void updatePhi(gpstk::Matrix<double>& Phi, int& index) const override;

        virtual void updateQ(gpstk::Matrix<double>& Q, int& index) const override;

        virtual void defStateAndCovariance(gpstk::Vector<double>& x, gpstk::Matrix<double>& P, int& index) const override;

        virtual int getNumUnknowns() const override;

        virtual InterFrequencyBiases& setStochasicModel(const gpstk::SatID::SatelliteSystem& system, gpstk::StochasticModel_uptr newModel);

    
    private:

        std::map< FilterParameter, gpstk::StochasticModel_uptr> stochasticModels;

        //current set of satellite systems
        ParametersSet types;

        class Initilizer
        {
        public: Initilizer();
        };
        static Initilizer IfbSingleton;
    };
}
