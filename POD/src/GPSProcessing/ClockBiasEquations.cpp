#include "ClockBiasEquations.h"

using namespace gpstk;

namespace pod
{
    ClockBiasEquations::ClockBiasEquations() 
        : type( TypeID::cdt), stochModel(std::make_unique<WhiteNoiseModel>())
    {
    }

    ClockBiasEquations::ClockBiasEquations(double sigma)
        : type(TypeID::cdt), stochModel(std::make_unique<WhiteNoiseModel>(sigma))
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

    void ClockBiasEquations::updateH(const gpstk::gnssRinex& svs, const gpstk::TypeIDSet& types, gpstk::Matrix<double>& H, int& col_0)
    {
        for (size_t i = 0; i < H.rows(); i++)
            H(i, col_0) = 1.0;
        
        col_0++;
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
