#include "PositionEquations.h"

using namespace gpstk;
using namespace std;

namespace pod
{
    //PositionEquations::PositionEquations()
    //    :types({ TypeID::dx,TypeID::dy,TypeID::dz })

    //{
    //    for (const auto &it : types)
    //        stochasticModels[it] = std::make_unique<WhiteNoiseModel>(100);
    //}
    PositionEquations::PositionEquations(double sigma)
        :types({ TypeID::dx,TypeID::dy,TypeID::dz })
    {
        for (const auto &it : types)
            stochasticModels[it] = std::make_unique<WhiteNoiseModel>(sigma);
    }
    
    PositionEquations& PositionEquations::setStochasicModel( StochasticModel_sptr newModel)
    {
        for(auto& it: stochasticModels)
            it.second= newModel;
        return *this;
    }

    PositionEquations& PositionEquations::setStochasicModel(TypeID id, StochasticModel_sptr newModel)
    {
        stochasticModels[id] = newModel;
        return *this;
    }

    void PositionEquations::Prepare(gpstk::gnssRinex& gData)
    {
        //do nothing
        for (const auto& it : stochasticModels)
            it.second->Prepare(SatID::dummy, gData);

    }

    void PositionEquations::updateEquationTypes(gpstk::gnssRinex& gData, gpstk::TypeIDSet& eq) 
    {
        for (const auto& it : types)
            eq.insert(it);
    }

    void PositionEquations::updatePhi(gpstk::Matrix<double>& Phi, int& index) const
    {
      
        for (const auto &it : types)
        {
            Phi(index, index) = stochasticModels.at(it)->getPhi();
            ++index;
        }
    }

    void PositionEquations::updateQ(gpstk::Matrix<double>& Q, int& index) const
    {

        for (const auto &it : types)
        {
            Q(index, index) = stochasticModels.at(it)->getQ();
            ++index;
        }
    }

    void PositionEquations::defStateAndCovariance(gpstk::Vector<double>& x, gpstk::Matrix<double>& P, int& index) const
    {
        for (const auto &it : types)
        {
            x(index) = 0;
            P(index, index) = 1e9;
            ++index;
        }
    }

    int PositionEquations::getNumUnknowns() const
    {
        return types.size();
    }
}
