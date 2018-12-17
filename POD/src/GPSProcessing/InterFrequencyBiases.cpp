#include "InterFrequencyBiases.h"

using namespace gpstk;

namespace pod
{
	std::map< SatID::SatelliteSystem, FilterParameter> InterFrequencyBiases::ss2ifb;
	std::map< FilterParameter, SatID::SatelliteSystem> InterFrequencyBiases::ifb2ss;

	const TypeIDSet InterFrequencyBiases::l2Types{ TypeID::prefitP2, TypeID::prefitL2 };

	InterFrequencyBiases::Initilizer InterFrequencyBiases::IfbSingleton;

	InterFrequencyBiases::Initilizer::Initilizer()
	{
		ss2ifb[SatID::SatelliteSystem::systemGPS] = FilterParameter(TypeID::recIFB_GPS_L2);
		ss2ifb[SatID::SatelliteSystem::systemGlonass] = FilterParameter(TypeID::recIFB_GLN_L2);
		ss2ifb[SatID::SatelliteSystem::systemGalileo] = FilterParameter(TypeID::recIFB_GAL_E5);
		ss2ifb[SatID::SatelliteSystem::systemBeiDou] = FilterParameter(TypeID::recIFB_BDS_B2);

		for (const auto& it : ss2ifb)
			ifb2ss.insert(std::make_pair(it.second, it.first));
	}

	InterFrequencyBiases::InterFrequencyBiases()
	{
		for (auto& it : ifb2ss)
			stochasticModels.insert(std::make_pair(it.first, std::make_unique<ConstantModel>()));
	}

	void InterFrequencyBiases::Prepare(IRinex& gData)
	{
		types.clear();
		for (const auto& it : gData.getBody())
			types.insert(ss2ifb[it.first.system]);

		//fill the IFB data
		//for (auto& it : gData.body)
		//    for (const auto& t : types)
		//        if (ss2ifb[it.first.system] == t)
		//            it.second[t] = 1.0;
		//        else
		//            it.second[t] = 0.0;

		for (const auto &ss : types)
			stochasticModels[ss]->Prepare(SatID::dummy, gData);
	}

	void InterFrequencyBiases::updateH(const gpstk::IRinex& gData, const gpstk::TypeIDSet& obsTypes, gpstk::Matrix<double>& H, int& col_0)
	{
		//ParametersSet availableTypes, typeToRemove;


		//find all IFB types which can be estimated from current observations set (gData)
		//for (const auto& it : svs)
		//    availableTypes.insert(ss2ifb.at(it.system));

		//than, find the IFB types, which exist in current ISB set, but can't be observable 
		//std::set_difference
		//(
		//    types.begin(), types.end(),
		//    availableTypes.begin(), availableTypes.end(),
		//    std::inserter(typeToRemove, typeToRemove.begin())
		//);

		//remove unobservable IFB types from current set of TypeID 
		//for (const auto& it : typeToRemove)
		//    types.erase(it);


		int row(0);
		auto currentSatSet = gData.getBody().getSatID();

		/*
		... | cdt(G2) | cdt(R2) |...| cdt(B2) |...
		*/
		for (const auto& type : obsTypes)
		{
			if (l2Types.find(type) == l2Types.end())
			{
				row += currentSatSet.size();
				continue;
			}

			for (const auto& sv : currentSatSet)
			{
				auto it = types.find(ss2ifb[sv.system]);
				int j = std::distance(types.begin(), it);
				H(row++, col_0 + j) = 1;
			}
		}
		col_0 += types.size();
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