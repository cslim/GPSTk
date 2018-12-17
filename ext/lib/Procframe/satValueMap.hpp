#pragma once
#include"SatID.hpp"
#include"Vector.hpp"


namespace gpstk
{
	/// Thrown when attempting to access a value and the corresponding SatID
	/// does not exist in the map.
	/// @ingroup exceptiongroup
	NEW_EXCEPTION_CLASS(SatIDNotFound, gpstk::Exception);



	/// Map holding SatID with corresponding numeric value.
	struct satValueMap : std::map<SatID, double>
	{

		/// Returns the number of satellites available.
		size_t numSats() const
		{
			return (*this).size();
		}


		/// Returns a SatIDSet with all the satellites present in this object.
		SatIDSet getSatID() const;


		/// Returns a Vector with all the satellites present in this object.
		Vector<SatID> getVectorOfSatID() const;


		/// Returns a satValueMap with only this satellite.
		/// @param satellite Satellite to be extracted.
		satValueMap extractSatID(const SatID& satellite) const;


		/// Returns a satValueMap with only one satellite, identified by
		/// the given parameters.
		/// @param p Satellite PRN number.
		/// @param p System the satellite belongs to.
		satValueMap extractSatID(const int& p,
			const SatID::SatelliteSystem& s) const;


		/// Returns a satValueMap with only these satellites.
		/// @param satSet Set (SatIDSet) containing the satellites to
		///               be extracted.
		satValueMap extractSatID(const SatIDSet& satSet) const;


		/// Modifies this object, keeping only this satellite.
		/// @param satellite Satellite to be kept.
		satValueMap& keepOnlySatID(const SatID& satellite);


		/// Modifies this object, keeping only this satellite.
		/// @param p Satellite PRN number.
		/// @param p System the satellite belongs to.
		satValueMap& keepOnlySatID(const int& p,
			const SatID::SatelliteSystem& s);


		/// Modifies this object, keeping only these satellites.
		/// @param satSet Set (SatIDSet) containing the satellites to be kept.
		satValueMap& keepOnlySatID(const SatIDSet& satSet);


		/// Modifies this object, removing this satellite.
		/// @param satellite Satellite to be removed.
		satValueMap& removeSatID(const SatID& satellite)
		{
			(*this).erase(satellite); return (*this);
		}


		/// Modifies this object, removing the given satellites.
		/// @param satSet Set (SatIDSet) containing the satellites to
		///               be removed.
		satValueMap& removeSatID(const SatIDSet& satSet);


		/** Returns the data value (double) corresponding to provided SatID.
		 *
		 * @param satellite     Satellite to be looked for.
		 */
		double getValue(const SatID& satellite) const
			throw(SatIDNotFound);


		/// Returns a reference to the data value (double) with
		/// corresponding SatID.
		/// @param satellite Satellite to be looked for.
		double& operator()(const SatID& satellite)
			throw(SatIDNotFound);


		/// Destructor.
		virtual ~satValueMap() {};

	};  // End of 'satValueMap'


}