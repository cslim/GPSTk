#pragma once
#include "EquationBase.h"
#include"StochasticModel.hpp"
#include <type_traits>
#include<memory>

namespace pod
{

    class IonoEquations :
        public EquationBase
    {
        typedef gpstk::StochasticModel_uptr (IonoEquations::*StochModelInitialazer)(double qprime) ;
        static const double SQR_L1_WL_GPS;
    public:
        IonoEquations();
        IonoEquations(double qPrime);
        ~IonoEquations(){};

        /* update state of equations with new observational data */
        virtual  void Prepare(gpstk::gnssRinex& gData) override;

        /*Check, if unknown parameters currently observable, if so,
        put the corresponding TypeID into 'TypeIDSet'
        */
        virtual void updateH(const gpstk::gnssRinex& gData,  const gpstk::TypeIDSet& types, gpstk::Matrix<double>& H, int& col_0) override;

        /* return set of TypeID, corresponding unknown parameters  for given equations */
        virtual  ParametersSet getParameters() const override
        {
            return currParameters;
        }

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

        /* return number of unknowns
        */
        virtual int getNumUnknowns() const override;

        template<class T , typename = std::enable_if_t<std::is_base_of<gpstk::StochasticModel, T>::value> >
        IonoEquations& setStocModel()
        {
            if (std::is_same<T, gpstk::StochasticModel>::value)
                stModelInitializer = &IonoEquations::constantModel;
            else if(std::is_same<T, gpstk::RandomWalkModel>::value)
                stModelInitializer = &IonoEquations::rWalkModel;
            else if (std::is_same<T, gpstk::WhiteNoiseModel>::value)
                stModelInitializer = &IonoEquations::whiteNoiseModel;
            
            return *this;
        }

        IonoEquations& setSigma(double sigma)
        {
            this->sigma = sigma;
            return *this;
        }

    private:

        gpstk::StochasticModel_uptr constantModel(double sigma);
        gpstk::StochasticModel_uptr rWalkModel(double qPrime);
        gpstk::StochasticModel_uptr whiteNoiseModel(double sigma);

#pragma region Fields

        gpstk::TypeID eqType;
        
        //current parameters - iono delay along lines of sight for each satellites
        ParametersSet currParameters;

        StochModelInitialazer stModelInitializer;

        std::map<gpstk::SatID, gpstk::StochasticModel_uptr> stochModels;

        double sigma;

        static std::map<gpstk::TypeID, int> obsType2Band;
        static std::map<gpstk::TypeID, int> obsType2Sign;

#pragma endregion
    };
}

