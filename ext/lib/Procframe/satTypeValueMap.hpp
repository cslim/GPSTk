#pragma once
#include"SatID.hpp"
#include"typeValueMap.hpp"
#include"SatValueMap.hpp"
#include"Matrix.hpp"

namespace gpstk
{


	/// Thrown when the number of data values and the number of
	/// corresponding types does not match.
	/// @ingroup exceptiongroup
	NEW_EXCEPTION_CLASS(NumberOfTypesMismatch, gpstk::Exception);


	/// Thrown when the number of data values and the number of
	/// corresponding satellites does not match.
	/// @ingroup exceptiongroup
	NEW_EXCEPTION_CLASS(NumberOfSatsMismatch, gpstk::Exception);


	/// Map holding SatID with corresponding typeValueMap.
	struct satTypeValueMap : std::map<SatID, typeValueMap>
	{

		/// Returns the number of available satellites.
		size_t numSats() const
		{
			return (*this).size();
		}


		/** Returns the total number of data elements in the map.
		 * This method DOES NOT suppose that all the satellites have
		 * the same number of type values.
		 */
		size_t numElements() const;


		/// Returns a SatIDSet with all the satellites present in this object.
		SatIDSet getSatID() const;


		/// Returns a Vector with all the satellites present in this object.
		Vector<SatID> getVectorOfSatID() const;


		/// Returns a TypeIDSet with all the data types present in
		/// this object.  This does not imply that all satellites have
		/// these types.
		TypeIDSet getTypeID() const;


		/// Returns a satTypeValueMap with only this satellite.
		/// @param satellite Satellite to be extracted.
		satTypeValueMap extractSatID(const SatID& satellite) const;


		/// Returns a satTypeValueMap with only one satellite, identified
		/// by the given parameters.
		/// @param p Satellite PRN number.
		/// @param p System the satellite belongs to.
		satTypeValueMap extractSatID(const int& p,
			const SatID::SatelliteSystem& s) const;


		/// Returns a satTypeValueMap with only these satellites.
		/// @param satSet Set (SatIDSet) containing the satellites to
		///               be extracted.
		satTypeValueMap extractSatID(const SatIDSet& satSet) const;

		/// Returns a satTypeValueMap with only these Satellite system.
		/// @param satSet Set (SatSystSet) containing the Satellite system to
		///               be extracted.
		satTypeValueMap extractSatSyst(const SatSystSet& sustSet) const;

		/// Modifies this object, keeping only this satellite.
		/// @param satellite Satellite to be kept.
		satTypeValueMap& keepOnlySatID(const SatID& satellite);


		/// Modifies this object, keeping only this satellite.
		/// @param p Satellite PRN number.
		/// @param p System the satellite belongs to.
		satTypeValueMap& keepOnlySatID(const int& p,
			const SatID::SatelliteSystem& s);


		/// Modifies this object, keeping only these satellites.
		/// @param satSet Set (SatIDSet) containing the satellites to be kept.
		satTypeValueMap& keepOnlySatID(const SatIDSet& satSet);


		/// Returns a satTypeValueMap with only this type of value.
		/// @param type Type of value to be extracted.
		satTypeValueMap extractTypeID(const TypeID& type) const;


		/// Returns a satTypeValueMap with only these types of data.
		/// @param typeSet Set (TypeIDSet) containing the types of data
		///                to be extracted.
		satTypeValueMap extractTypeID(const TypeIDSet& typeSet) const;


		/// Modifies this object, keeping only this type of data.
		/// @param type Type of value to be kept.
		satTypeValueMap& keepOnlyTypeID(const TypeID& type);


		/// Modifies this object, keeping only these types of data.
		/// @param typeSet Set (TypeIDSet) containing the types of data
		///                to be kept.
		satTypeValueMap& keepOnlyTypeID(const TypeIDSet& typeSet);


		/// Modifies this object, removing this satellite.
		/// @param satellite Satellite to be removed.
		satTypeValueMap& removeSatID(const SatID& satellite)
		{
			(*this).erase(satellite); return (*this);
		}


		/// Modifies this object, removing these satellites.
		/// @param satSet Set (SatIDSet) containing the satellites
		///               to be removed.
		satTypeValueMap& removeSatID(const SatIDSet& satSet);

		satTypeValueMap& removeSatSyst(const SatSystSet& satSet);

		satTypeValueMap& removeSatSyst(SatID::SatelliteSystem syst);

		/// Modifies this object, removing this type of data.
		/// @param type Type of value to be removed.
		satTypeValueMap& removeTypeID(const TypeID& type);


		/// Modifies this object, removing these types of data.
		/// @param typeSet Set (TypeIDSet) containing the types of data
		///                to be kept.
		satTypeValueMap& removeTypeID(const TypeIDSet& typeSet);


		/// Returns a GPSTk::Vector containing the data values with this type.
		/// @param type Type of value to be returned.
		/// This method returns zero if a given satellite does not have
		/// this type.
		Vector<double> getVectorOfTypeID(const TypeID& type) const;


		/// Returns a GPSTk::Matrix containing the data values in this set.
		/// @param typeSet  TypeIDSet of values to be returned.
		Matrix<double> getMatrixOfTypes(const TypeIDSet& typeSet) const;


		/** Modifies this object, adding one vector of data with this type,
		 *  one value per satellite.
		 *
		 * If type already exists, data is overwritten. If the number of
		 * values does not match with the number of satellites, a
		 * NumberOfSatsMismatch exception is thrown.
		 *
		 * Given that dataVector does not store information about the
		 * satellites the values correspond to, the user is held responsible
		 * for having the data values stored in dataVector in the proper
		 * order regarding the SatIDs in this object.
		 *
		 * @param type          Type of data to be added.
		 * @param dataVector    GPSTk Vector containing the data to be added.
		 */
		satTypeValueMap& insertTypeIDVector(const TypeID& type,
			const Vector<double> dataVector)
			throw(NumberOfSatsMismatch);


		/** Modifies this object, adding a matrix of data, one vector
		 *  per satellite.
		 *
		 * If types already exists, data is overwritten. If the number of
		 * rows in matrix does not match with the number of satellites, a
		 * NumberOfSatsMismatch exception is thrown. If the number of columns
		 * in matrix does not match with the number of types in typeSet, a
		 * NumberOfTypesMismatch exception is thrown.
		 *
		 * Given that dataMatrix does not store information about the
		 * satellites and types the values correspond to, the user is held
		 * responsible for having those data values stored in dataMatrix in
		 * the proper order regarding the SatIDs in this object and the
		 * provided typeSet.
		 *
		 * @param typeSet       Set (TypeIDSet) containing the types of data
		 *                      to be added.
		 * @param dataMatrix    GPSTk Matrix containing the data to be added.
		 */
		satTypeValueMap& insertMatrix(const TypeIDSet& typeSet,
			const Matrix<double> dataMatrix)
			throw(NumberOfSatsMismatch, NumberOfTypesMismatch);


		/** Returns the data value (double) corresponding to provided SatID
		 *  and TypeID.
		 *
		 * @param satellite     Satellite to be looked for.
		 * @param type          Type to be looked for.
		 */
		double getValue(const SatID& satellite,
			const TypeID& type) const
			throw(SatIDNotFound, TypeIDNotFound);


		/// Returns a reference to the typeValueMap with corresponding SatID.
		/// @param type Type of value to be look for.
		typeValueMap& operator()(const SatID& satellite)
			throw(SatIDNotFound);


		/// Convenience output method
		virtual std::ostream& dump(std::ostream& s,
			int mode = 0) const;


		/// Destructor.
		virtual ~satTypeValueMap() {};

	};  // End of 'satTypeValueMap'


}