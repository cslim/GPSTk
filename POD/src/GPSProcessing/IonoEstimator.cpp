#include "IonoEstimator.h"
#include"WinUtils.h"
using namespace gpstk;
using namespace std;
namespace pod
{
    Matrix<double> IonoEstimator::H;
    Matrix<double> IonoEstimator::W;
    double IonoEstimator::maxGap = 3600;

    IonoEstimator::Initializer IonoEstimator::initializer;

    IonoEstimator::Initializer::Initializer()
    {
        //initialize weigth matrix
        IonoEstimator::W = Matrix<double>(2, 2, .0);
        W(0, 0) = 1.0;
        W(1, 1) = 1e4;

        //initialize design matrix
        IonoEstimator::H = Matrix<double>(2, 2, .0);
        H(0, 0) = 1.0;
        H(1, 0) = -1.0;
        H(0, 1) = 0;
        H(1, 1) = 1;
    }

    IonoEstimator::IonoEstimator()
       {}

    gpstk::gnssSatTypeValue& IonoEstimator::Process(gpstk::gnssSatTypeValue& gData)
        throw(gpstk::ProcessingException)
    {
        gpstk::UnimplementedException e(
            "Vitrual method:\n"
            "'gpstk::gnssSatTypeValue& Process(gpstk::gnssSatTypeValue& gData)'\n"
            "has not been implemented in class:\n"
            "'pod::IonoEstimator'.");

        GPSTK_THROW(e)

            return gData;
    };

    gnssRinex& IonoEstimator::Process(gnssRinex& gData)
        throw(ProcessingException)
    {

        try
        {
            SatIDSet satRejectedSet;

            for (auto& it : gData.body)
            {
                try
                {
                    bool b = feed(it.first, gData);
                    if(!b)
                        satRejectedSet.insert(it.first);
                }
                catch (gpstk::Exception &e)
                {
                    DBOUT_LINE(e)
                    satRejectedSet.insert(it.first);
              
                }
                catch (std::exception &e)
                {
                    DBOUT_LINE(e.what());
                    satRejectedSet.insert(it.first);

                }
                catch (...)
                {
                    satRejectedSet.insert(it.first);
                }
            }

            // Remove satellites with missing data
            gData.removeSatID(satRejectedSet);

            return gData;
        }
        catch (Exception& u)
        {
            // Throw an exception if something unexpected happens
            ProcessingException e(getClassName() + ":"
                + u.what());

            GPSTK_THROW(e);

        }

    }  // End of method 'IonoEstimator::Process()'


       ///
    bool IonoEstimator::feed(const gpstk::SatID & sv, gnssRinex &gRin)
    {
        double ionoCode(.0);
        if (!lcIonoCode.getCombination(sv, gRin.body[sv], ionoCode)) return false;

        double ionoPhase(.0);
        if (!lcIonoPhase.getCombination(sv, gRin.body[sv], ionoPhase)) return false;

        //get reference to current sv data
        auto & data = filterData[sv];

        double dt = abs(data.rWalkModel.getCurrentTime() - data.rWalkModel.getPreviousTime());
        if (dt > maxGap)
        {
            reset(sv);
            cout << "reset for " << sv << endl;
        }

        //update stochastic models
        data.rWalkModel.Prepare(sv, gRin);
        biasStochModel.Prepare(sv, gRin);

        //
        Matrix<double> Cov(2, 2, .0), Q(2, 2, .0), Phi(2, 2, .0);
        Vector<double> state(2, .0), meas(2, .0);

        meas(0) = ionoCode;
        meas(1) = ionoPhase;
        Cov(0, 0) = data.state.q11;
        Cov(1, 0) = Cov(0, 1) = data.state.q12;
        Cov(1, 1) = data.state.q22;

        state(0) = data.state.delay;
        state(1) = data.state.bias;

        Q(0, 0) = data.rWalkModel.getQ();
        Q(1, 1) = biasStochModel.getQ();
        Phi(0, 0) = data.rWalkModel.getPhi();
        Phi(1, 1) = biasStochModel.getPhi();

        auto hMatrixTr = transpose(H);
        auto PhiTr = transpose(Phi);
        auto hTrTimesW = hMatrixTr*W;

        //predict
        auto Pminus = Phi*Cov*PhiTr + Q;
        auto xminus = Phi*state;

        //correct
        auto invPminus = inverseChol(Pminus);
        Cov = inverseChol(hTrTimesW*H + invPminus);
        state = Cov*(hTrTimesW*meas + (invPminus*xminus));

        //auto postfitResiduals = meas - H * state;

        data.state.q11 = Cov(0, 0);
        data.state.q22 = Cov(1, 1);
        data.state.q12 = Cov(0, 1);

        data.state.delay = state(0);
        data.state.bias = state(1);

        gRin.body[sv][TypeID::ionoL1] = data.state.delay;
        
        return true;
    }

       // Returns a string identifying this object.
    std::string IonoEstimator::getClassName() const
    {
        return "IonoEstimator";
    }

    IonoEstimator& IonoEstimator::clear()
    {
        filterData.clear();
        return *this;
    };

    IonoEstimator& IonoEstimator::reset(const SatID& sv)
    {
        filterData[sv].state = KalmanData();
        return *this;
    };
}