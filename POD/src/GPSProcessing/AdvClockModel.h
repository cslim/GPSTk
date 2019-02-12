#ifndef POD_ADV_CLOCK_MODEL_H
#define POD_ADV_CLOCK_MODEL_H

#include"StochasticModel.hpp"
#include"EquationBase.h"

namespace pod
{
    // linear clock model 
    class AdvClockModel: public EquationBase
    {
    public:
        AdvClockModel() :
			q1(1e-29), q2(1e-30),
			previousTime(gpstk::CommonTime::BEGINNING_OF_TIME),
            currentTime(gpstk::CommonTime::END_OF_TIME),
			types(ParametersSet{FilterParameter(gpstk::TypeID::recCdt),FilterParameter(gpstk::TypeID::recCdtdot) }),
			isFirstTime(true), dt(DBL_MAX)
		{};

        AdvClockModel(double q1_, double q2_) :
            q1(q1_), q2(q2_),
			previousTime(gpstk::CommonTime::BEGINNING_OF_TIME), 
			currentTime(gpstk::CommonTime::END_OF_TIME),
			types(ParametersSet{ FilterParameter(gpstk::TypeID::recCdt),FilterParameter(gpstk::TypeID::recCdtdot) }),
			isFirstTime(true), dt(DBL_MAX)
		{};
       
        AdvClockModel(double q1_, double q2_, const gpstk::CommonTime & t1, const gpstk::CommonTime & t2) :
            q1(q1_), q2(q2_),
			previousTime(t1), currentTime(t2),
			types(ParametersSet{ FilterParameter(gpstk::TypeID::recCdt),FilterParameter(gpstk::TypeID::recCdtdot) }),
			isFirstTime(true), dt(DBL_MAX)
		{};

       virtual ~AdvClockModel() {};

       /** Set the value of previous epoch
       *
       * @param prevTime   Value of previous epoch
       *
       */
       virtual AdvClockModel& setPreviousTime(const gpstk::CommonTime& prevTime)
       {
           previousTime = prevTime; return (*this);
       }

       /** Set the value of current epoch
       *
       * @param currTime   Value of current epoch
       *
       */
       virtual AdvClockModel& setCurrentTime(const gpstk::CommonTime& currTime)
       {
           currentTime = currTime; return (*this);
       }

       virtual AdvClockModel& setQ1(double q1_)
       {
           q1 = q1_; return (*this);
       }

       virtual AdvClockModel& setQ2(double q2_)
       {
           q2 = q2_; return (*this);
       }

	   // Inherited via EquationBase
	   virtual void Prepare(gpstk::IRinex & gData) override;

	   virtual ParametersSet getParameters() const override;

	   virtual void updatePhi(gpstk::Matrix<double>& Phi, int & index) const override;

	   virtual void updateQ(gpstk::Matrix<double>& Q, int & index) const override;

	   virtual void defStateAndCovariance(gpstk::Vector<double>& x, gpstk::Matrix<double>& P, int & index) const override;

	   virtual void updateH(const gpstk::IRinex & gData, const gpstk::TypeIDSet & types, gpstk::Matrix<double>& H, int & col_0) override;

	   virtual int getNumUnknowns() const override;


    private:

		ParametersSet types;
		
		double dt;
        
		///the diffusion coefficients, denoting the impact
        /// of white frequency noise
        double q1;

        ///the diffusion coefficients, denoting the impact
        /// of random walk frequency noise
        double q2;

        /// Epoch of previous measurement
        gpstk::CommonTime previousTime;

        /// Epoch of current measurement
        gpstk::CommonTime currentTime;

        mutable bool isFirstTime;


	};
}
#endif // !POD_ADV_CLOCK_MODEL_H
