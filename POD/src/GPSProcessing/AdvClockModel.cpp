#include"AdvClockModel.h"

using namespace gpstk;

namespace pod
{

    Matrix<double> AdvClockModel::getQ() const
    {
        double dt = isFirstTime?30.0: std::abs(currentTime - previousTime);
        Matrix<double> res(2, 2);

        double dt2 = dt*dt;
        double dt3 = dt2*dt;

        res(0, 0) = q1*dt+q2*dt3/3.0;
        res(0, 1) = q2*dt2 / 2.0;
        res(1, 0) = res(0, 1);
        res(1, 1) = q2*dt;

        isFirstTime = false;
        
        return res;
    }

    Matrix<double> AdvClockModel::getPhi() const
    {
        double dt = isFirstTime ? 30.0 : std::abs(currentTime - previousTime);
        Matrix<double> res(2, 2);

        res(0, 0) = 1.0;
        res(0, 1) = dt;
        res(1, 0) = 0;
        res(1, 1) = 1.0;
        
        return res;
    }
    void AdvClockModel::Prepare(const CommonTime & ct)
    {

        // Update previous epoch
        setPreviousTime(currentTime);

        setCurrentTime(ct);

        return;
    }
    void AdvClockModel::Prepare(const SatID& sat, const gnssSatTypeValue& gData)
    {

        // Update previous epoch
        setPreviousTime(currentTime);

        setCurrentTime(gData.header.epoch);

        return;
    }

    void AdvClockModel::Prepare(const SatID& sat, const gnssRinex& gData)
    {

        // Update previous epoch
        setPreviousTime(currentTime);

        setCurrentTime(gData.header.epoch);

        return;
    }
}