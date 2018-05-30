#include "InterSystemBias.h"

using namespace std;
using namespace gpstk;

namespace pod
{
    std::map< SatID::SatelliteSystem, TypeID> InterSystemBias::ss2isb;
    std::map< TypeID, SatID::SatelliteSystem> InterSystemBias::isb2ss;

    InterSystemBias::Initilizer InterSystemBias::IsbSingleton;

    InterSystemBias::Initilizer:: Initilizer()
    {
        ss2isb[SatID::SatelliteSystem::systemGlonass] = TypeID::recISB_GLN;
        //ss2isb[SatID::SatelliteSystem::systemGalileo] = TypeID::recISB_GAL;
        ss2isb[SatID::SatelliteSystem::systemBeiDou] =  TypeID::recISB_BDS;

        for (const auto& it : ss2isb)
            isb2ss[it.second] = it.first; 
    }

    InterSystemBias::InterSystemBias()
    {
        for (auto& it : isb2ss)
            stochasticModels[it.first] = std::make_unique<StochasticModel>();
    }

    void InterSystemBias::Prepare(gnssRinex& gData)
    {
        //counter of SV by satellite system
        //std::map<SatID::SatelliteSystem, int> SvNumberBySS;
        
        //fill the map Satellite System to number of SV
        //for (auto& it: gData.body)
        //{
        //    if (it.first.system != SatID::SatelliteSystem::systemGPS)
        //    {
        //        auto& cur = SvNumberBySS.find(it.first.system);
        //        if (cur == SvNumberBySS.end())
        //            SvNumberBySS[it.first.system] = 1;
        //        else
        //            cur->second++;
        //    }
        //}
        
        // fill the set of satellite systems to remove (with less then 2 entrees) 
        //SatSystSet SatSystToRemove;
        //for (auto& it : SvNumberBySS)
        //    if (it.second < 2)
        //        SatSystToRemove.insert(it.first);

        // remove the satellite system with less then 2 entrees from 'gData'
        //gData.body.removeSatSyst(SatSystToRemove);

        //remove ones from map Satellite System to number of SV
        //for (auto& it : SatSystToRemove)
        //    SvNumberBySS.erase(it);

        //update current set of Satellite systems
        types.clear();
        for (const auto& it : gData.body)
            if (it.first.system != SatID::SatelliteSystem::systemGPS)
                types.insert(ss2isb[it.first.system]);
         
        //fill the ISB data
        for (auto& it : gData.body)
            for (const auto& t : types)
                if (ss2isb[it.first.system] == t)
                    it.second[t] = 1.0;
                else
                    it.second[t] = 0.0;
        
        for (const auto &ss : types)
            stochasticModels[ss]->Prepare(SatID::dummy, gData);
    }
    
    void InterSystemBias::updateEquationTypes(gpstk::gnssRinex& gData, TypeIDSet& eq)
    {
        
        TypeIDSet availableTypes, typeToRemove;
        auto allSats = gData.getVectorOfSatID();
        
        //find all ISB types which can be estimated from current observations set (gData)
        for (const auto& it : allSats)
            if (it.system != SatID::systemGPS)
                availableTypes.insert(ss2isb.at(it.system));

        //than, find the ISB types, which exist in current ISB set, but can't be observable 
        std::set_difference
        (
            types.begin(), types.end(),
            availableTypes.begin(), availableTypes.end(),
            std::inserter(typeToRemove, typeToRemove.begin())
        );
        //remove unobservable ISB types from current set of TypeID 
        for (const auto& it : typeToRemove)
            types.erase(it);
        
        //finally, let's add current ISB types to the overall equation set
        for (const auto& it : types)
            eq.insert(it);
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