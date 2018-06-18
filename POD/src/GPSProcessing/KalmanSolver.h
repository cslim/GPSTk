#pragma once
#include "SolverBase.hpp"
#include"SimpleKalmanFilter.hpp"
#include"EquationComposer.h"

using namespace gpstk;

namespace pod
{

    class KalmanSolver :
        public gpstk::SolverBase, public gpstk::ProcessingClass
    {
    public:
        //maximum time interval without data
        static double maxGap;

        KalmanSolver();
        KalmanSolver(eqComposer_sptr eqs);
        virtual ~KalmanSolver();

        virtual gnssSatTypeValue& Process(gnssSatTypeValue& gData)
            throw(ProcessingException);

        virtual gnssRinex& Process(gnssRinex& gData)
            throw(ProcessingException);

        /// Returns a string identifying this object.
        virtual std::string getClassName(void) const
        { return "KalmanSolver"; }
       
        //virtual gnssEquationDefinition getDefaultEqDefinition() const override
        //{
        //    return gnssEquationDefinition(equations->measTypes(), equations->currentUnkNowns());
        //}

        virtual EquationComposer& eqComposer() 
        {
            return *equations;
        }

        virtual double getSolution(const FilterParameter& type) const ;
        virtual double getVariance(const FilterParameter& type) const ;

    protected:

        virtual void fixAmbiguities(gnssRinex& gData);
        virtual void storeAmbiguities(gnssRinex& gData) const;
        virtual int check(gnssRinex& gData);
        virtual gnssRinex& reject(gnssRinex& gData, const TypeIDSet& typeOfResid);

        virtual void reset()
        {
            equations->clearData();
        }
        //virtual int Compute(const Vector<double>& prefitResiduals,
        //    const Matrix<double>& designMatrix,
        //    const Matrix<double>& rMatrix)
        //    throw(InvalidSolver);
        CommonTime t_pre = CommonTime::BEGINNING_OF_TIME;

        bool firstTime;

        ///state transition matrix
        Matrix<double> phiMatrix;

        ///process noise matrix
        Matrix<double> qMatrix;

        /// Geometry matrix
        Matrix<double> hMatrix;

        /// weights matrix
        Matrix<double> weigthMatrix;

        /// Measurements vector (Prefit-residuals)
        Vector<double> measVector;

        ///object to prepare Matrix for filter
        eqComposer_sptr equations;

    };
}
