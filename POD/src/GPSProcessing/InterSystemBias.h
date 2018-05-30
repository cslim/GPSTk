#pragma once
#include "EquationBase.h"
#include "StochasticModel.hpp"
#include<memory>

namespace pod
{

    /**This is a class to modify the GNSS data stucture: to
    * add ISB componenst of receiver clock offset
    *
    * The "InterSystemBias" object will visit every satellite in
    * the GNSS data structure and will try to add ISB model terms
    * to it.
    *
    * When used with the ">>" operator, this class returns the same
    * incoming data structure with the ISB data inserted along their
    * corresponding satellites. Be warned that if a GDS contains
    * less then two satellites of some satellite system its will
    * be summarily deleted from the data structure.
    */
    class InterSystemBias :
        public  EquationBase
    {
    private:
        static std::map< gpstk::SatID::SatelliteSystem, gpstk::TypeID> ss2isb;
        static std::map<gpstk::TypeID, gpstk::SatID::SatelliteSystem> isb2ss;

    public:

        InterSystemBias();
        virtual ~InterSystemBias() {};

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

        virtual InterSystemBias& setStochasicModel(const gpstk::SatID::SatelliteSystem& system, gpstk::StochasticModel_uptr newModel);


    private:

        std::map< gpstk::TypeID, gpstk::StochasticModel_uptr> stochasticModels;
        
        //current set of satellite systems
        gpstk::TypeIDSet types;

        class Initilizer
        {
        public: Initilizer();
        };
        static Initilizer IsbSingleton;

    };
}

