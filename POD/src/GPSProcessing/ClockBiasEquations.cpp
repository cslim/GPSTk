#include "ClockBiasEquations.h"

using namespace gpstk;

namespace pod
{
    ClockBiasEquations::ClockBiasEquations() 
        : type(TypeID::cdt), stochModel(std::make_unique<WhiteNoiseModel>())
    {
    }
    
    ClockBiasEquations& ClockBiasEquations::setStochasicModel( StochasticModel_sptr newModel)
    {
        stochModel = newModel;
        return *this;
    }

    void ClockBiasEquations::Prepare(gnssRinex & gData)
    {

        stochModel->Prepare(SatID::dummy, gData);
    }
    
    void ClockBiasEquations::updateEquationTypes(gpstk::gnssRinex& gData,TypeIDSet & eqTypes) 
    {
        eqTypes.insert(type);
    }
    
    void ClockBiasEquations::updatePhi( Matrix<double>& Phi, int & index) const
    {
        Phi(index, index) = stochModel->getPhi();
        ++index;
    }
    
    void ClockBiasEquations::updateQ( Matrix<double>& Q, int & index) const
    {
        Q(index, index) = stochModel->getQ();
        ++index;
    }
    
    void ClockBiasEquations::defStateAndCovariance( Vector<double>& x,  Matrix<double>& P, int & index) const
    {
        x(index) = 0;
        P(index, index) = 1e9;
        ++index;
    }
    
    int ClockBiasEquations::getNumUnknowns() const
    {
        return 1;
    }
}
