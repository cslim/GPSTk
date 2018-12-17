#ifndef POD_ADV_CLOCK_MODEL_H
#define POD_ADV_CLOCK_MODEL_H

#include"StochasticModel.hpp"
#include"CommonTime.hpp"

namespace pod
{
    // linear clock model 
    class AdvClockModel
    {
    public:
        AdvClockModel() : q1(1e-29), q2(1e-30), previousTime(gpstk::CommonTime::BEGINNING_OF_TIME),
            currentTime(gpstk::CommonTime::END_OF_TIME) {};

        AdvClockModel(double q1_, double q2_) :
            q1(q1_), q2(q2_), previousTime(gpstk::CommonTime::BEGINNING_OF_TIME), currentTime(gpstk::CommonTime::END_OF_TIME) {};
       
        AdvClockModel(double q1_, double q2_, const gpstk::CommonTime & t1, const gpstk::CommonTime & t2) :
            q1(q1_), q2(q2_), previousTime(t1), currentTime(t2) {};

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

       gpstk::Matrix<double> getQ() const;

       gpstk::Matrix<double> getPhi() const;

       void Prepare(const gpstk::CommonTime & ct);

       void Prepare(const gpstk::SatID& sat, const gpstk::IRinex& gData);

    private:

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
