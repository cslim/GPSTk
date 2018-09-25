#pragma once
#include "KalmanSolver.h"
#include"ProcessingClass.hpp"

namespace pod
{
    class KalmanSolverFB :
        public KalmanSolver
    {
    private:
        //set of all possible TypeID for code pseudorange postfit residuals 
        static const std::set<gpstk::TypeID> codeResTypes;

        //set of all possible TypeID for  carrier phase postfit residuals 
        static const std::set<gpstk::TypeID> phaseResTypes;


    public:
        KalmanSolverFB();
        KalmanSolverFB(eqComposer_sptr eqs);

        virtual ~KalmanSolverFB();

        virtual std::string getClassName() const override
        {
            return "KalmanSolverFB";
        }

        // Solution
        virtual const  gpstk::Vector<double>& Solution() const
        {
            return solver.Solution();
        }

        virtual  gpstk::Vector<double>& Solution()
        {
            return solver.Solution();
        }

        // Postfit-residuals.
        virtual const gpstk::Vector<double>& PostfitResiduals() const override
        {
            return solver.PostfitResiduals();
        }

        //return postfit residuals vector
        virtual gpstk::Vector<double>& PostfitResiduals() override
        {
            return solver.PostfitResiduals();
        }

        //return current varince - covarince matrix
        virtual const gpstk::Matrix<double>& CovMatrix()const
        {
            return solver.CovMatrix();
        }

        //return current varince - covarince matrix
        virtual gpstk::Matrix<double>& CovMatrix()
        {
            return solver.CovMatrix();
        }

        //return sqrt(vpv/(n-p)) value
        virtual double getSigma() const
        {
            return solver.getSigma();
        }

        //return minimum number of satellites requared for state esimation
        virtual double getMinSatNumber() const
        {
            return solver.getMinSatNumber();
        }

        //return current solver  status 
        // true - solution valid
        // false - invalid
        virtual bool isValid() const
        {
            return solver.isValid();
        }

        //set minimum number of satellites requared for state esimation
        virtual KalmanSolverFB& setMinSatNumber(int value) override
        {
            solver.setMinSatNumber(value);
            return *this;
        }

        //get current value for given filter parameter
        double getSolution(const FilterParameter& type) const override
        {
            return solver.getSolution(type);
        }

        //get current varince value for given filter parameter
        double getVariance(const FilterParameter& type) const override
        {
            return solver.getVariance(type);
        }

        KalmanSolverFB& setLimits(const std::list<double>& codeLims, const std::list<double>& phaseLims);

        KalmanSolverFB& setCyclesNumber(size_t number)
        {
            cyclesNumber = number;
            return *this;
        }

        gpstk::gnssRinex & Process(gpstk::gnssRinex & gRin);

        //last forward process cycle
        bool lastProcess(gpstk::gnssRinex & gRin);

        //Reprocess the data stored during a previous 'Process()' call.
        void reProcess(int numCycles);

        //Reprocess the data stored during a previous 'Process()' call.
        void reProcess(void);

        
    private:
        //This method checks the residuals and modifies 'gData' accordingly.
        void checkLimits(gpstk::gnssRinex& gData, size_t cycleNumber);

        double getLimit(const gpstk::TypeID& type, size_t cycleNumber);

#pragma region Fields

        struct limits
        {
            std::vector<double> codeLimits;
            std::vector<double> phaseLimits;
        }tresholds;

    public:

        // Number of processed measurements.
        int processedMeasurements;

        //Number of measurements rejected because they were off limits.
        int rejectedMeasurements;

       
    private:

        //Number of measurements rejected because they were off limits.
        std::list<gpstk::gnssRinex> ObsData;

        // is current iteration first
        bool firstIteration;

        //internal kalman solver object, which do main part of real work
        KalmanSolver solver;

        //number of forward-backward cycles
        size_t cyclesNumber;

#pragma endregion


    };
}

