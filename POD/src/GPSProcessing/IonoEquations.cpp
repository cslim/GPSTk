#include "IonoEquations.h"

using namespace gpstk;
namespace pod
{
    IonoEquations::IonoEquations()
       // : types( gpstk::TypeID::ionoMap }),
       : pStochasticModel(std::make_unique<gpstk::RandomWalkModel>())
    {};
    IonoEquations::IonoEquations(double qPrime)
    //: types({ gpstk::TypeID::ionoMap }),
        :pStochasticModel(std::make_unique<gpstk::RandomWalkModel>(qPrime))
    {};

    void IonoEquations::Prepare(gpstk::gnssRinex & gData)
    {
        pStochasticModel->Prepare(SatID::dummy, gData);
    }
    void IonoEquations::updateH(const gpstk::gnssRinex& gData, const gpstk::TypeIDSet& types, gpstk::Matrix<double>& H, int& col_0)
    {

    }


    void IonoEquations::updatePhi(gpstk::Matrix<double>& Phi, int & index) const
    {
        Phi(index, index) = pStochasticModel->getPhi();
        ++index;
    }

    void IonoEquations::updateQ(gpstk::Matrix<double>& Q, int & index) const
    {
        Q(index, index) = pStochasticModel->getQ();
        ++index;
    }

    void IonoEquations::defStateAndCovariance(gpstk::Vector<double>& x, gpstk::Matrix<double>& P, int & index) const
    {
        x(index) = 0.0;
        P(index, index) = 99.0;

        ++index;
    }

    int IonoEquations::getNumUnknowns() const
    {
        return types.size();
    }
}
