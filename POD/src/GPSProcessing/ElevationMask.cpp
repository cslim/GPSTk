#include"ElevationMask.hpp"
#include"Exception.hpp"
#include"boost/format.hpp"
using namespace gpstk;

namespace pod
{
	IRinex & ElevationMask::
		Process(IRinex & gData)
	{
		SatIDSet svRejSet;
		for (auto && it : gData.getBody())
		{
			auto it2 = it.second->get_value().find(TypeID::elevation);
			if (it2 != it.second->get_value().end())
			{
				if (it2->second < elevation)
					svRejSet.insert(it.first);
			}
			else
				svRejSet.insert(it.first);
		}
		
		gData.getBody().removeSatID(svRejSet);

		return gData;
	}

}
