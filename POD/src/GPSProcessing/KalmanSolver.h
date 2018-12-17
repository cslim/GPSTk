#pragma once
#include "SolverBase.hpp"
#include"SimpleKalmanFilter.hpp"
#include"EquationComposer.h"

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


        virtual gpstk::IRinex& Process(gpstk::IRinex& gData)
            throw(gpstk::ProcessingException);

        // Returns a string identifying this object.
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
        virtual void fixAmbiguities(gpstk::IRinex& gData);
        
        //check data integrity 
        virtual int check(gpstk::IRinex& gData);

        int checkPhase(gpstk::IRinex& gData);

        //reject bad observation using residuals value
        virtual gpstk::IRinex& reject(gpstk::IRinex& gData, const gpstk::TypeIDSet& typeOfResid);

        virtual void reset()
        {
            equations->clearData();
        }

        gpstk::CommonTime t_pre = gpstk::CommonTime::BEGINNING_OF_TIME;

        bool firstTime;

        //indicate, is current filter state valid
        bool isValid_;

        //minimum number of satellites requared for state esimation
        size_t minSatNumber;

        //state transition matrix
        gpstk::Matrix<double> phiMatrix;

        //process noise matrix
        gpstk::Matrix<double> qMatrix;

        // Geometry matrix
        gpstk::Matrix<double> hMatrix;

        // weights matrix
        gpstk::Matrix<double> weigthMatrix;

        // Measurements vector (Prefit-residuals)
        gpstk::Vector<double> measVector;

        //sqrt(vpv/(n-p))
        double sigma;

        //object to prepare Matrix for filter
        eqComposer_sptr equations;

    };
}
