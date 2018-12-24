#include"UsedInPvtMarker.hpp"

using namespace gpstk;
namespace pod
{
	
	gpstk::IRinex& UsedInPvtMarker::
		Process(gpstk::IRinex& gRin)
	{
		markAsUsed(gRin.getBody());
		return gRin;
	}

	gpstk::SatTypePtrMap& UsedInPvtMarker::
		markAsUsed(gpstk::SatTypePtrMap& satData) const
	{
		for (auto &&it : satData)
			it.second->get_value()[type] = SatUsedStatus::UsedInPVT;

		return satData;
	}

	gpstk::SatTypePtrMap& UsedInPvtMarker::
		keepOnlyUsed(gpstk::SatTypePtrMap& satData) const
	{
		SatIDSet rejectedSats;
		for (auto &&it : satData)
		{
			auto it2 = it.second->get_value().find(type);
			if (it2 == it.second->get_value().end() || it2->second == SatUsedStatus::NotUsedInPVT)
				rejectedSats.insert(it.first);
			
		}

		satData.removeSatID(rejectedSats);
		return satData;
	}

	gpstk::SatTypePtrMap& UsedInPvtMarker::
		CleanScFlags(gpstk::SatTypePtrMap& satData) const
	{
		for (auto it = satData.begin(); it != satData.end(); ++it)
		{
			//reset CS flag, if this sv - epoch already has been rejected by CS catcher
			auto status = it->second->get_value().find(TypeID::satStatus);
			if (status != it->second->get_value().end() && status->second == SatUsedStatus::RejectedByCsCatcher)
				it->second->get_value()[TypeID::CSL1] = it->second->get_value()[TypeID::CSL2] = 0;
		}
		return satData;
	}

	gpstk::SatTypePtrMap& UsedInPvtMarker::
		CleanSatArcFlags(gpstk::SatTypePtrMap& satData) const
	{
		for(auto&& it :satData)
			it.second->get_value()[TypeID::satArc] = 0;

		return satData;
	}
}