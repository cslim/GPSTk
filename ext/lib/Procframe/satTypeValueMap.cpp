#include"satTypeValueMap.hpp"

namespace gpstk
{

	////// satTypeValueMap //////


	/* Returns the total number of data elements in the map.
	 * This method DOES NOT suppose that all the satellites have
	 * the same number of type values.
	 */
	size_t satTypeValueMap::numElements() const
	{

		size_t numEle(0);

		for (satTypeValueMap::const_iterator it = (*this).begin();
			it != (*this).end();
			++it)
		{
			numEle = numEle + (*it).second.size();
		}

		return numEle;

	}  // End of method 'satTypeValueMap::numElements()'



	   // Returns a SatIDSet with all the satellites present in this object.
	SatIDSet satTypeValueMap::getSatID() const
	{

		SatIDSet satSet;

		for (satTypeValueMap::const_iterator pos = (*this).begin();
			pos != (*this).end();
			++pos)
		{
			satSet.insert((*pos).first);
		}

		return satSet;

	}  // End of method 'satTypeValueMap::getSatID()'



	   // Returns a Vector with all the satellites present in this object.
	Vector<SatID> satTypeValueMap::getVectorOfSatID() const
	{

		std::vector<SatID> temp;

		for (satTypeValueMap::const_iterator pos = (*this).begin();
			pos != (*this).end();
			++pos)
		{
			temp.push_back((*pos).first);
		}

		Vector<SatID> result;
		result = temp;

		return result;

	}  // End of method 'satTypeValueMap::getVectorOfSatID()'



	   // Returns a TypeIDSet with all the data types present in
	   // this object. This does not imply that all satellites have these types.
	TypeIDSet satTypeValueMap::getTypeID() const
	{

		TypeIDSet typeSet;

		for (satTypeValueMap::const_iterator pos = (*this).begin();
			pos != (*this).end();
			++pos)
		{

			for (typeValueMap::const_iterator it = (*pos).second.begin();
				it != (*pos).second.end();
				++it)
			{
				typeSet.insert((*it).first);
			}

		}

		return typeSet;

	}  // End of method 'satTypeValueMap::getTypeID()'



	   // Returns a satTypeValueMap with only this satellite.
	   // @param satellite Satellite to be extracted.
	satTypeValueMap satTypeValueMap::extractSatID(const SatID& satellite) const
	{

		SatIDSet satSet;
		satSet.insert(satellite);

		return extractSatID(satSet);

	}  // End of method 'satTypeValueMap::extractSatID()'



	   // Returns a satTypeValueMap with only one satellite, identified
	   // by the given parameters.
	   // @param p Satellite PRN number.
	   // @param p System the satellite belongs to.
	satTypeValueMap satTypeValueMap::extractSatID(const int& p,
		const SatID::SatelliteSystem& s) const
	{

		SatID tempSatellite(p, s);  // We build a temporary SatID object

		return extractSatID(tempSatellite);

	}  // End of method 'satTypeValueMap::extractSatID()'



	   // Returns a satTypeValueMap with only these satellites.
	   // @param satSet Set (SatIDSet) containing the satellites to
	   //               be extracted.
	satTypeValueMap satTypeValueMap::extractSatID(const SatIDSet& satSet) const
	{

		satTypeValueMap stvMap;

		for (SatIDSet::const_iterator pos = satSet.begin();
			pos != satSet.end();
			++pos)
		{
			satTypeValueMap::const_iterator itObs((*this).find(*pos));
			if (itObs != (*this).end())
			{
				stvMap[(*itObs).first] = (*itObs).second;
			};
		}

		return stvMap;

	}  // End of method 'satTypeValueMap::extractSatID()'

	satTypeValueMap satTypeValueMap::extractSatSyst(const SatSystSet& systSet) const
	{

		satTypeValueMap stvMap;

		for (const auto&it : *this)
		{
			if (systSet.find(it.first.system) != systSet.end())
				stvMap[it.first] = it.second;
		}

		return stvMap;

	}  // End of method 'satTypeValueMap::extractSatID()'

	   // Modifies this object, keeping only this satellite.
	   // @param satellite Satellite to be kept.
	satTypeValueMap& satTypeValueMap::keepOnlySatID(const SatID& satellite)
	{

		SatIDSet satSet;
		satSet.insert(satellite);

		return keepOnlySatID(satSet);

	}  // End of method 'satTypeValueMap::keepOnlySatID()'



	   // Modifies this object, keeping only this satellite.
	   // @param p Satellite PRN number.
	   // @param p System the satellite belongs to.
	satTypeValueMap& satTypeValueMap::keepOnlySatID(const int& p,
		const SatID::SatelliteSystem& s)
	{

		SatID tempSatellite(p, s);  // We build a temporary SatID object

		return keepOnlySatID(tempSatellite);

	}  // End of method 'satTypeValueMap::keepOnlySatID()'



	   // Modifies this object, keeping only these satellites.
	   // @param satSet Set (SatIDSet) containing the satellites to be kept.
	satTypeValueMap& satTypeValueMap::keepOnlySatID(const SatIDSet& satSet)
	{

		satTypeValueMap stvMap(extractSatID(satSet));
		(*this) = stvMap;

		return (*this);

	}  // End of method 'satTypeValueMap::keepOnlySatID()'



	   // Returns a satTypeValueMap with only this type of value.
	   // @param type Type of value to be extracted.
	satTypeValueMap satTypeValueMap::extractTypeID(const TypeID& type) const
	{

		TypeIDSet typeSet;
		typeSet.insert(type);

		return extractTypeID(typeSet);

	}  // End of method 'satTypeValueMap::extractTypeID()'



	   // Returns a satTypeValueMap with only these types of data.
	   // @param typeSet Set (TypeIDSet) containing the types of data
	   //                to be extracted.
	satTypeValueMap satTypeValueMap::extractTypeID(const TypeIDSet& typeSet)
		const
	{

		satTypeValueMap theMap;

		for (satTypeValueMap::const_iterator it = (*this).begin();
			it != (*this).end();
			++it)
		{

			typeValueMap tvMap((*it).second.extractTypeID(typeSet));
			if (tvMap.size() > 0)
			{
				theMap[(*it).first] = tvMap;
			};

		};

		return theMap;

	}  // End of method 'satTypeValueMap::extractTypeID()'



	   // Modifies this object, keeping only this type of data.
	   // @param type Type of value to be kept.
	satTypeValueMap& satTypeValueMap::keepOnlyTypeID(const TypeID& type)
	{

		TypeIDSet typeSet;
		typeSet.insert(type);

		return keepOnlyTypeID(typeSet);

	}  // End of method 'satTypeValueMap::keepOnlyTypeID()'



	   // Modifies this object, keeping only these types of data.
	   // @param typeSet Set (TypeIDSet) containing the types of data
	   //                to be kept.
	satTypeValueMap& satTypeValueMap::keepOnlyTypeID(const TypeIDSet& typeSet)
	{

		satTypeValueMap stvMap(extractTypeID(typeSet));
		(*this) = stvMap;

		return (*this);

	}  // End of method 'satTypeValueMap::keepOnlyTypeID()'



	   // Modifies this object, removing these satellites.
	   // @param satSet Set (SatIDSet) containing the satellites
	   //               to be removed.
	satTypeValueMap& satTypeValueMap::removeSatID(const SatIDSet& satSet)
	{

		for (SatIDSet::const_iterator pos = satSet.begin();
			pos != satSet.end();
			++pos)
		{
			(*this).erase(*pos);
		}

		return (*this);

	}  // End of method 'satTypeValueMap::removeSatID()'

	satTypeValueMap& satTypeValueMap::removeSatSyst(const SatSystSet& satSystSet)
	{
		auto satSet = this->getSatID();
		for (const auto& it : satSet)
		{
			if (satSystSet.find(it.system) != satSystSet.end())
				(*this).erase(it);
		}

		return (*this);

	}  // End of method 'satTypeValueMap::removeSatSyst()'

	satTypeValueMap& satTypeValueMap::removeSatSyst(SatID::SatelliteSystem syst)
	{
		SatSystSet ssset;
		ssset.insert(syst);
		return removeSatSyst(ssset);
	}

	// Modifies this object, removing this type of data.
	// @param type Type of value to be removed.
	satTypeValueMap& satTypeValueMap::removeTypeID(const TypeID& type)
	{

		for (satTypeValueMap::iterator it = (*this).begin();
			it != (*this).end();
			++it)
		{
			(*it).second.removeTypeID(type);
		}

		return (*this);

	}  // End of method 'satTypeValueMap::removeTypeID()'



	   // Modifies this object, removing these types of data.
	   // @param typeSet Set (TypeIDSet) containing the types of data
	   //                to be kept.
	satTypeValueMap& satTypeValueMap::removeTypeID(const TypeIDSet& typeSet)
	{

		for (TypeIDSet::const_iterator pos = typeSet.begin();
			pos != typeSet.end();
			++pos)
		{
			removeTypeID(*pos);
		}

		return (*this);

	}  // End of method 'satTypeValueMap::removeTypeID()'



	   // Returns a GPSTk::Vector containing the data values with this type.
	   // @param type Type of value to be returned.
	   // This method returns zero if a given satellite does not have this type.
	Vector<double> satTypeValueMap::getVectorOfTypeID(const TypeID& type) const
	{
		Vector<double> result(this->size());
		int i = 0;
		for (const auto& it : *this)
		{
			const auto& itObs(it.second.find(type));
			result[i] = (itObs != (it).second.end()) ? (*itObs).second : 0.0;
			++i;
		}

		return result;

	}  // End of method 'satTypeValueMap::getVectorOfTypeID()'



	   // Returns a GPSTk::Matrix containing the data values in this set.
	   // @param typeSet  TypeIDSet of values to be returned.
	Matrix<double> satTypeValueMap::getMatrixOfTypes(const TypeIDSet& typeSet)
		const
	{

		// First, let's create a Matrix<double> of the proper size
		Matrix<double> tempMat((*this).numSats(), typeSet.size(), 0.0);

		size_t numRow(0), numCol(0);

		for (satTypeValueMap::const_iterator it = (*this).begin();
			it != (*this).end();
			++it)
		{
			numCol = 0;

			for (TypeIDSet::const_iterator pos = typeSet.begin();
				pos != typeSet.end();
				++pos)
			{

				typeValueMap::const_iterator itObs((*it).second.find(*pos));
				if (itObs != (*it).second.end())
				{
					tempMat(numRow, numCol) = (*itObs).second;
				}

				++numCol;
			}

			++numRow;

		}

		return tempMat;

	}  // End of method 'satTypeValueMap::getMatrixOfTypes()'



	   /* Modifies this object, adding one vector of data with this type,
		* one value per satellite.
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
	satTypeValueMap& satTypeValueMap::insertTypeIDVector(const TypeID& type,
		const Vector<double> dataVector)
		throw(NumberOfSatsMismatch)
	{

		if (dataVector.size() == (*this).numSats())
		{
			size_t pos = 0;

			for (satTypeValueMap::iterator it = (*this).begin();
				it != (*this).end();
				++it)
			{
				(*it).second[type] = dataVector[pos];
				++pos;
			}

			return (*this);

		}
		else
		{
			GPSTK_THROW(NumberOfSatsMismatch(" Number of data values in vector \
and number of satellites do not match"));
		}

	}  // End of method 'satTypeValueMap::insertTypeIDVector()'



	   /* Modifies this object, adding a matrix of data, one vector
		* per satellite.
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
	satTypeValueMap& satTypeValueMap::insertMatrix(const TypeIDSet& typeSet,
		const Matrix<double> dataMatrix)
		throw(NumberOfSatsMismatch, NumberOfTypesMismatch)
	{

		if (dataMatrix.rows() != (*this).numSats())
		{
			GPSTK_THROW(NumberOfSatsMismatch("Number of rows in matrix and \
number of satellites do not match"));
		}

		if (dataMatrix.cols() == typeSet.size())
		{

			size_t pos(0);

			for (satTypeValueMap::iterator it = (*this).begin();
				it != (*this).end();
				++it)
			{

				size_t idx(0);

				for (TypeIDSet::const_iterator itSet = typeSet.begin();
					itSet != typeSet.end();
					++itSet)
				{
					(*it).second[(*itSet)] = dataMatrix(pos, idx);
					++idx;
				}

				++pos;

			}

			return (*this);

		}
		else
		{
			GPSTK_THROW(NumberOfTypesMismatch("Number of data values per row \
in matrix and number of types do not match"));
		}

	}  // End of method 'satTypeValueMap::insertMatrix()'



	   /* Returns the data value (double) corresponding to provided SatID
		* and TypeID.
		*
		* @param satellite     Satellite to be looked for.
		* @param type          Type to be looked for.
		*/
	double satTypeValueMap::getValue(const SatID& satellite,
		const TypeID& type) const
		throw(SatIDNotFound, TypeIDNotFound)
	{

		satTypeValueMap::const_iterator itObs((*this).find(satellite));
		if (itObs != (*this).end())
		{
			return (*itObs).second.getValue(type);
		}
		else
		{
			GPSTK_THROW(SatIDNotFound("SatID not found in map"));
		}

	}  // End of method 'satTypeValueMap::getValue()'



	   // Returns a reference to the typeValueMap with corresponding SatID.
	   // @param type Type of value to be looked for.
	typeValueMap& satTypeValueMap::operator()(const SatID& satellite)
		throw(SatIDNotFound)
	{

		satTypeValueMap::iterator itObs((*this).find(satellite));
		if (itObs != (*this).end())
		{
			return (*itObs).second;
		}
		else
		{
			GPSTK_THROW(SatIDNotFound("SatID not found in map"));
		}

	}  // End of method 'satTypeValueMap::operator()'



}