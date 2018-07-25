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


        virtual EquationComposer& eqComposer() 
        {
            return *equations;
        }

        //return sqrt(vpv/(n-p)) value
        virtual double getSigma() const
        { return sigma; }

        //return minimum number of satellites requared for state esimation
        virtual double getMinSatNumber() const
        {
            return minSatNumber;
        }

        //return isValid
        virtual bool isValid() const
        {
            return isValid_;
        }

        //set minimum number of satellites requared for state esimation
        virtual KalmanSolver& setMinSatNumber(int value) 
        {
             minSatNumber = value;
             return *this;
        }

        // get solution variance
        virtual double getSolution(const FilterParameter& type) const;
        
        // get solution variance
        virtual double getVariance(const FilterParameter& type) const;

    protected:

        //resolve carrier  phase ambiguities ot integer values
        virtual void fixAmbiguities(gnssRinex& gData);
        
        //check data integrity 
        virtual int check(gnssRinex& gData);

        //reject bad observation using residuals value
        virtual gnssRinex& reject(gnssRinex& gData, const TypeIDSet& typeOfResid);

        virtual void reset()
        {
            equations->clearData();
        }

        CommonTime t_pre = CommonTime::BEGINNING_OF_TIME;

        bool firstTime;

        //current solution type
        bool isValid_;

        //minimum number of satellites requared for state esimation
        int minSatNumber;

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

        ///sqrt(vpv/(n-p))
        double  sigma;

        ///object to prepare Matrix for filter
        eqComposer_sptr equations;

    };
}
