#include "InterSystemBias.h"


using namespace gpstk;

namespace pod
{
    std::map< SatID::SatelliteSystem, FilterParameter> InterSystemBias::ss2isb;
    std::map< FilterParameter, SatID::SatelliteSystem> InterSystemBias::isb2ss;
    
    // 
    const TypeIDSet InterSystemBias::l1Types{ TypeID::prefitC, TypeID::prefitL1,TypeID::prefitPC, TypeID::prefitLC };

    InterSystemBias::Initilizer InterSystemBias::IsbSingleton;

    InterSystemBias::Initilizer:: Initilizer()
    {
        ss2isb[SatID::SatelliteSystem::systemGlonass] = FilterParameter(TypeID::recISB_GLN);
        //ss2isb[SatID::SatelliteSystem::systemGalileo] = FilterParameter(TypeID::recISB_GAL);
        ss2isb[SatID::SatelliteSystem::systemBeiDou] = FilterParameter(TypeID::recISB_BDS);

        for (const auto& it : ss2isb)
            isb2ss[it.second] = it.first; 
    }

    InterSystemBias::InterSystemBias()
    {
        for (auto& it : isb2ss)
            stochasticModels[it.first] = std::make_unique<ConstantModel>();
    }

    void InterSystemBias::Prepare(IRinex& gData)
    {
        //update current set of Satellite systems
        types.clear();
        for (const auto& it : gData.getBody())
            if (it.first.system != SatID::SatelliteSystem::systemGPS)
                types.insert(ss2isb[it.first.system]);
        
        for (const auto &ss : types)
            stochasticModels[ss]->Prepare(SatID::dummy, gData);
    }
    
    void InterSystemBias::updateH(const gpstk::IRinex& gData, const gpstk::TypeIDSet& obsTypes, gpstk::Matrix<double>& H, int& col_0)
    {
        auto currentSatSet = gData.getBody().getSatID();
        int row(0);
        for (const auto& obs : obsTypes)
        {
            if (l1Types.find(obs) == l1Types.end())
            {
                row += currentSatSet.size();
                continue;
            }

            for (const auto& sv : currentSatSet)
            {
                if (sv.system != SatID::SatelliteSystem::systemGPS)
                {
                    auto it = types.find(ss2isb[sv.system]);
                    int j = std::distance(types.begin(), it);
                    H(row, col_0 + j) = 1;
                }
                row++;
            }
        }
        col_0 += types.size();
    }

    InterSystemBias& InterSystemBias::setStochasicModel(
        const SatID::SatelliteSystem& system,
        StochasticModel_uptr newModel)
    {
        stochasticModels[ss2isb.at(system)] = std::move(newModel);
        return *this;
    }

    void InterSystemBias::updatePhi(gpstk::Matrix<double>& Phi, int& index) const
    {
        for (const auto &ss: types)
        {
            Phi(index, index) = stochasticModels.at(ss)->getPhi();
            ++index;
        }
    }

    void InterSystemBias::updateQ(gpstk::Matrix<double>& Q, int& index) const
    {
        for (const auto &ss : types)
        {
            Q(index, index) = stochasticModels.at(ss)->getQ();
            ++index;
        }
    }

    int InterSystemBias::getNumUnknowns() const
    {
       return types.size();
    }

    void InterSystemBias:: defStateAndCovariance(gpstk::Vector<double>& x, gpstk::Matrix<double>& P, int& index) const
    {
        for (const auto &ss : types)
        {
            x(index) = 0;
            P(index, index) = 1e9;
            ++index;
        }
    }
}