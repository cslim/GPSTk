#include"satValueMap.hpp"

namespace gpstk
{

	////// satValueMap //////

	// Returns a SatIDSet with all the satellites present in this object.
	SatIDSet satValueMap::getSatID() const
	{

		SatIDSet satSet;

		for (satValueMap::const_iterator pos = (*this).begin();
			pos != (*this).end();
			++pos)
		{
			satSet.insert((*pos).first);
		}

		return satSet;

	}  // End of method 'satValueMap::getSatID()'



	   // Returns a Vector with all the satellites present in this object.
	Vector<SatID> satValueMap::getVectorOfSatID() const
	{

		std::vector<SatID> temp;

		for (satValueMap::const_iterator pos = (*this).begin();
			pos != (*this).end();
			++pos)
		{
			temp.push_back((*pos).first);
		}

		Vector<SatID> result;
		result = temp;

		return result;

	}  // End of method 'satValueMap::getVectorOfSatID()'



	   // Returns a satValueMap with only this satellite.
	   // @param satellite Satellite to be extracted.
	satValueMap satValueMap::extractSatID(const SatID& satellite) const
	{

		SatIDSet satSet;
		satSet.insert(satellite);

		return extractSatID(satSet);

	}  // End of method 'satValueMap::extractSatID()'



	   // Returns a satValueMap with only one satellite, identified by
	   // the given parameters.
	   // @param p Satellite PRN number.
	   // @param p System the satellite belongs to.
	satValueMap satValueMap::extractSatID(const int& p,
		const SatID::SatelliteSystem& s) const
	{

		SatID tempSatellite(p, s);  // We build a temporary SatID object

		return extractSatID(tempSatellite);

	}  // End of method 'satValueMap::extractSatID()'



	   // Returns a satValueMap with only these satellites.
	   // @param satSet Set (SatIDSet) containing the satellites to
	   //               be extracted.
	satValueMap satValueMap::extractSatID(const SatIDSet& satSet) const
	{

		satValueMap svMap;

		for (SatIDSet::const_iterator pos = satSet.begin();
			pos != satSet.end();
			++pos)
		{
			satValueMap::const_iterator itObs((*this).find(*pos));

			if (itObs != (*this).end())
			{
				svMap[(*itObs).first] = (*itObs).second;
			};
		}

		return svMap;

	}  // End of method 'satValueMap::extractSatID()'



	   // Modifies this object, keeping only this satellite.
	   // @param satellite Satellite to be kept.
	satValueMap& satValueMap::keepOnlySatID(const SatID& satellite)
	{

		SatIDSet satSet;
		satSet.insert(satellite);

		return keepOnlySatID(satSet);

	}  // End of method 'satValueMap::keepOnlySatID()'



	   // Modifies this object, keeping only this satellite.
	   // @param p Satellite PRN number.
	   // @param p System the satellite belongs to.
	satValueMap& satValueMap::keepOnlySatID(const int& p,
		const SatID::SatelliteSystem& s)
	{

		SatID tempSatellite(p, s);  // We build a temporary SatID object

		return keepOnlySatID(tempSatellite);

	}  // End of method 'satValueMap::keepOnlySatID()'



	   // Modifies this object, keeping only these satellites.
	   // @param satSet Set (SatIDSet) containing the satellites to be kept.
	satValueMap& satValueMap::keepOnlySatID(const SatIDSet& satSet)
	{

		satValueMap svMap = extractSatID(satSet);
		(*this) = svMap;

		return (*this);

	}  // End of method 'satValueMap::keepOnlySatID()'



	   // Modifies this object, removing these satellites.
	   // @param satSet Set (SatIDSet) containing the satellites to
	   //               be removed.
	satValueMap& satValueMap::removeSatID(const SatIDSet& satSet)
	{

		for (SatIDSet::const_iterator pos = satSet.begin();
			pos != satSet.end();
			++pos)
		{
			(*this).erase(*pos);
		}

		return (*this);

	}  // End of method 'satValueMap::removeSatID()'



	   /* Returns the data value (double) corresponding to provided SatID.
		*
		* @param satellite     Satellite to be looked for.
		*/
	double satValueMap::getValue(const SatID& satellite) const
		throw(SatIDNotFound)
	{

		satValueMap::const_iterator itObs((*this).find(satellite));
		if (itObs != (*this).end())
		{
			return (*itObs).second;
		}
		else
		{
			GPSTK_THROW(SatIDNotFound("SatID not found in map"));
		}

	}  // End of method 'satValueMap::getValue()'



	   // Returns a reference to the data value (double) with
	   // corresponding SatID.
	   // @param satellite Satellite to be looked for.
	double& satValueMap::operator()(const SatID& satellite)
		throw(SatIDNotFound)
	{

		satValueMap::iterator itObs((*this).find(satellite));

		if (itObs != (*this).end())
		{
			return (*itObs).second;
		}
		else
		{
			GPSTK_THROW(SatIDNotFound("SatID not found in map"));
		}

	}  // End of method 'satValueMap::operator()'


}