#include "TropoEquations.h"
using namespace gpstk;
using namespace std;

namespace pod
{
    TropoEquations::TropoEquations(double qPrime)
        :types({ gpstk::TypeID::wetMap }),
        pStochasticModel(std::make_unique<RandomWalkModel>(qPrime))
    {}

    void TropoEquations::Prepare(gpstk::gnssRinex & gData)
    {
        pStochasticModel->Prepare(SatID::dummy, gData);
    }

    void TropoEquations::updateEquationTypes(gpstk::gnssRinex & gData, gpstk::TypeIDSet & eqTypes)
    {
        for (const auto& it : types)
            eqTypes.insert(it);
    }

    gpstk::TypeIDSet TropoEquations::getEquationTypes() const
    {
        return types;
    }
    void TropoEquations::updatePhi(gpstk::Matrix<double>& Phi, int & index) const
    {
        Phi(index, index) = pStochasticModel->getPhi();
        ++index;
    }

    void TropoEquations::updateQ(gpstk::Matrix<double>& Q, int & index) const
    {
        Q(index, index) = pStochasticModel->getQ();
        ++index;
    }

    void TropoEquations::defStateAndCovariance(gpstk::Vector<double>& x, gpstk::Matrix<double>& P, int & index) const
    {
        x(index) = 0.0;
        P(index, index) = 0.25;

        ++index;
    }

    int TropoEquations::getNumUnknowns() const
    {
        return types.size();
    }
}
