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
		typedef std::map<gpstk::CommonTime, EquationComposer::FilterState> filterHistory;

	protected:
		//set of all possible TypeID for code pseudorange postfit residuals 
		static const std::set<gpstk::TypeID> codeResTypes;

		//set of all possible TypeID for  carrier phase postfit residuals 
		static const std::set<gpstk::TypeID> phaseResTypes;

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
		{
			return "KalmanSolver";
		}

		virtual EquationComposer& eqComposer()
		{
			return *equations;
		}

		//return sqrt(vpv/(n-p)) value
		virtual double getSigma() const
		{
			return sigma;
		}

		virtual double getPhaseSigma() const
		{
			return phaseSigma;
		}

		virtual double getCodeSigma() const
		{
			return codeSigma;
		}

		//return minimum number of satellites requared for state esimation
		virtual double getMinSatNumber() const
		{
			return minSatNumber;
		}

		//return isValid
		virtual bool getValid() const
		{
			return isValid;
		}

		//set minimum number of satellites requared for state esimation
		virtual KalmanSolver& setMinSatNumber(int value)
		{
			minSatNumber = value;
			return *this;
		}

		// get solution 
		virtual double getSolution(const FilterParameter& type) const;

		// get solution variance
		virtual double getVariance(const FilterParameter& type) const;

		virtual const EquationComposer::FilterState & getState() const
		{
			return equations->getState();
		}

		virtual KalmanSolver & setState(const EquationComposer::FilterState & newState)
		{
			equations->setState(newState);
			return *this;
		}
		bool getResetState()
		{
			return isReset;
		}

		bool ResetIfRequared(const gpstk::CommonTime& t, const filterHistory& data);

		//filter states, processed so far will be used in case of filer reset
		std::map<gpstk::CommonTime, EquationComposer::FilterState> FilterData;

	protected:

		double getSigma(const gpstk::TypeIDSet& types) const;

		int getUnknownIndex(const FilterParameter& parameter) const;

		//resolve carrier  phase ambiguities ot integer values
		virtual void fixAmbiguities(gpstk::IRinex& gData);

		//check phase data integrity 
		int checkPhase(gpstk::IRinex& gData);

		//reject bad observation using residuals value
		virtual gpstk::IRinex& reject(gpstk::IRinex& gData, const gpstk::TypeIDSet& typeOfResid);

		virtual void reset()
		{
			equations->clearData();
		}

		gpstk::CommonTime t_pre = gpstk::CommonTime::BEGINNING_OF_TIME;

		bool firstTime;

		// Minimum satellites number required for state computation
		size_t minSatNumber;

		// State transition matrix
		gpstk::Matrix<double> phiMatrix;

		// Process noise matrix
		gpstk::Matrix<double> qMatrix;

		// Geometry matrix (derivative of observations wrt state)
		gpstk::Matrix<double> hMatrix;

		// weights matrix
		gpstk::Matrix<double> weigthMatrix;

		// Measurements vector (prefit-residuals)
		gpstk::Vector<double> measVector;

		//Weight unit error (sqrt(vpv/(n-p)))
		double sigma;

		double phaseSigma;

		double codeSigma;

		//object to prepare h, phi, q  matrices for filter
		eqComposer_sptr equations;

		// Indicator of current filter state validity
		bool isValid;

		// Indicate, if reset occurred on current filter step
		bool isReset;
	};
}
