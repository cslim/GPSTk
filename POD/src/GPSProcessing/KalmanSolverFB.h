#pragma once
#include "KalmanSolver.h"
#include"ProcessingList.hpp"
#include"RinexEpoch.h"
#include"UsedInPvtMarker.hpp"

namespace pod
{
    class KalmanSolverFB :
        public KalmanSolver
    {

    public:
        KalmanSolverFB();
        KalmanSolverFB(eqComposer_sptr eqs);

        virtual ~KalmanSolverFB();

        virtual std::string getClassName() const override
        {
            return "KalmanSolverFB";
        }

		virtual EquationComposer& eqComposer() override
		{
			return solver.eqComposer();
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

		//return sqrt(vpv/(n-p)) value
		virtual double getPhaseSigma() const
		{
			return solver.getPhaseSigma();
		}

		//return sqrt(vpv/(n-p)) value
		virtual double getCodeSigma() const
		{
			return solver.getCodeSigma();
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

		virtual gpstk::ProcessingList& ReProcList()
		{
			return reProcList;
		}

		virtual const gpstk::ProcessingList& ReProcList() const
		{
			return reProcList;
		}

		virtual UsedInPvtMarker& UsedSvMarker()
		{
			return usedSvMarker;
		}

		virtual const UsedInPvtMarker& UsedSvMarker() const
		{
			return usedSvMarker;
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

        gpstk::IRinex & Process(gpstk::IRinex & gRin);

        //last forward process cycle
        bool lastProcess(gpstk::IRinex & gRin);

        //Reprocess the data stored during a previous 'Process()' call.
        void reProcess(void);

        
    private:
		
		gpstk::IRinex & ReProcessOneEpoch(gpstk::IRinex & gRin);

        //This method checks the residuals and modifies 'gData' accordingly.
        void checkLimits(gpstk::IRinex& gData, size_t cycleNumber);

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

		void updateCurrList();

		void addData(gpstk::IRinex & gRin);

		//all observational data
		std::list < std::list<gpstk::irinex_uptr>> obsData;
		
		//current list of epoches
		std::list < std::list<gpstk::irinex_uptr>>::iterator currList;

	    //internal kalman solver object, which do main part of real work
        KalmanSolver solver;

        //number of forward-backward cycles
        size_t cyclesNumber;
		
		//current  forward-backward cycle
        size_t currCycle;

		//
		gpstk::ProcessingList reProcList;

		//
		UsedInPvtMarker usedSvMarker;

#pragma endregion


    };
}

