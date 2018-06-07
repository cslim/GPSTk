#pragma once
#include "SolverLMS.hpp"
#include"SimpleKalmanFilter.hpp"
#include"EquationComposer.h"

using namespace gpstk;

namespace pod
{

    class KalmanSolver :
        public gpstk::SolverLMS
    {
    public:
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

        double getSolution(const TypeID& type) const override;

        double getVariance(const TypeID& type) const override;

    protected:
        virtual void fixAmbiguities(gnssRinex& gData);
        virtual void storeAmbiguities(gnssRinex& gData) const;
        virtual int check(gnssRinex& gData);
        virtual gnssRinex& reject(gnssRinex& gData, const TypeIDSet& typeOfResid);

        //virtual int Compute(const Vector<double>& prefitResiduals,
        //    const Matrix<double>& designMatrix,
        //    const Matrix<double>& rMatrix)
        //    throw(InvalidSolver);


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

        /// General Kalman filter object
        //SimpleKalmanFilter kFilter;

        ///object to prepare Matrix for filter
        eqComposer_sptr equations;

    };
}
