#pragma once
#include "EquationBase.h"
#include"StochasticModel.hpp"

namespace pod
{
    class InterFrequencyBiases :
        public EquationBase
    {
    private:
        static std::map< gpstk::SatID::SatelliteSystem, gpstk::TypeID> ss2ifb;
        static std::map<gpstk::TypeID, gpstk::SatID::SatelliteSystem> ifb2ss;

    public:
        InterFrequencyBiases();
        virtual ~InterFrequencyBiases() {};

        virtual  void Prepare(gpstk::gnssRinex& gData);

        virtual void updateEquationTypes(gpstk::gnssRinex& gData, gpstk::TypeIDSet& eq)  override;

        virtual gpstk::TypeIDSet getEquationTypes() const override
        {
            return types;
        }

        virtual void updatePhi(gpstk::Matrix<double>& Phi, int& index) const override;

        virtual void updateQ(gpstk::Matrix<double>& Q, int& index) const override;

        virtual void defStateAndCovariance(gpstk::Vector<double>& x, gpstk::Matrix<double>& P, int& index) const override;

        virtual int getNumUnknowns() const override;

        virtual InterFrequencyBiases& setStochasicModel(const gpstk::SatID::SatelliteSystem& system, gpstk::StochasticModel_uptr newModel);

    
    private:

        std::map< gpstk::TypeID, gpstk::StochasticModel_uptr> stochasticModels;

        //current set of satellite systems
        gpstk::TypeIDSet types;

        class Initilizer
        {
        public: Initilizer();
        };
        static Initilizer IfbSingleton;
    };
}
