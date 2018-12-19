#include"UsedInPvtMarker.hpp"

using namespace gpstk;
namespace pod
{

	gpstk::IRinex& UsedInPvtMarker::
		Process(gpstk::IRinex& gRin)
	{
		mark(gRin.getBody());
		return gRin;
	}

	gpstk::SatTypePtrMap& UsedInPvtMarker::
		mark(gpstk::SatTypePtrMap& satData) const
	{
		for (auto &&it : satData)
			it.second->get_value()[type] = 1;
		
		return satData;
	}

	gpstk::SatTypePtrMap& UsedInPvtMarker::
		keepOnlyUsed(gpstk::SatTypePtrMap& satData) const
	{
		SatIDSet rejectedSats;
		for (auto &&it : satData)
		{
			auto it2 = it.second->get_value().find(type);
			if (it2 == it.second->get_value().end())
				rejectedSats.insert(it.first);
		}
		satData.removeSatID(rejectedSats);
		return satData;
	}
}