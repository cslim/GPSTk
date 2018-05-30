#include "InterFrequencyBiases.h"

using namespace gpstk;

namespace pod
{
    std::map< SatID::SatelliteSystem, TypeID> InterFrequencyBiases::ss2ifb;
    std::map< TypeID, SatID::SatelliteSystem> InterFrequencyBiases::ifb2ss;

    InterFrequencyBiases::Initilizer InterFrequencyBiases::IfbSingleton;

    InterFrequencyBiases::Initilizer::Initilizer()
    {
        ss2ifb[SatID::SatelliteSystem::systemGPS] = TypeID::recIFB_GPS_L2;
        ss2ifb[SatID::SatelliteSystem::systemGlonass] = TypeID::recIFB_GLN_L2;
        ss2ifb[SatID::SatelliteSystem::systemGalileo] = TypeID::recIFB_GAL_E5;
        ss2ifb[SatID::SatelliteSystem::systemBeiDou] = TypeID::recIFB_BDS_B2;

        for (const auto& it : ss2ifb)
            ifb2ss[it.second] = it.first;
    }

    InterFrequencyBiases::InterFrequencyBiases()
    {
        for (auto& it : ifb2ss)
            stochasticModels[it.first] = std::make_unique<StochasticModel>();
    }

    void InterFrequencyBiases::Prepare(gnssRinex& gData)
    {
        types.clear();
        for (const auto& it : gData.body)
                types.insert(ss2ifb[it.first.system]);

        //fill the IFB data
        for (auto& it : gData.body)
            for (const auto& t : types)
                if (ss2ifb[it.first.system] == t)
                    it.second[t] = 1.0;
                else
                    it.second[t] = 0.0;

        for (const auto &ss : types)
            stochasticModels[ss]->Prepare(SatID::dummy, gData);
    }

    void InterFrequencyBiases::updateEquationTypes(gpstk::gnssRinex& gData, TypeIDSet& eq)
    {

        TypeIDSet availableTypes, typeToRemove;
        auto allSats = gData.getVectorOfSatID();

        //find all IFB types which can be estimated from current observations set (gData)
        for (const auto& it : allSats)
                availableTypes.insert(ss2ifb.at(it.system));

        //than, find the IFB types, which exist in current ISB set, but can't be observable 
        std::set_difference
        (
            types.begin(), types.end(),
            availableTypes.begin(), availableTypes.end(),
            std::inserter(typeToRemove, typeToRemove.begin())
        );
        //remove unobservable IFB types from current set of TypeID 
        for (const auto& it : typeToRemove)
            types.erase(it);

        //finally, let's add current IFB types to the overall equation set
        for (const auto& it : types)
            eq.insert(it);
    }

    InterFrequencyBiases& InterFrequencyBiases::setStochasicModel(
        const SatID::SatelliteSystem& system,
        StochasticModel_uptr newModel)
    {
        stochasticModels[ss2ifb.at(system)] = std::move(newModel);
        return *this;
    }

    void InterFrequencyBiases::updatePhi(gpstk::Matrix<double>& Phi, int& index) const
    {
        for (const auto &ss : types)
        {
            Phi(index, index) = stochasticModels.at(ss)->getPhi();
            ++index;
        }
    }

    void InterFrequencyBiases::updateQ(gpstk::Matrix<double>& Q, int& index) const
    {
        for (const auto &ss : types)
        {
            Q(index, index) = stochasticModels.at(ss)->getQ();
            ++index;
        }
    }

    int InterFrequencyBiases::getNumUnknowns() const
    {
        return types.size();
    }

    void InterFrequencyBiases::defStateAndCovariance(gpstk::Vector<double>& x, gpstk::Matrix<double>& P, int& index) const
    {
        for (const auto &ss : types)
        {
            x(index) = 0;
            P(index, index) = 1e9;
            ++index;
        }
    }
}