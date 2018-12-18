#include "SatTypePtrMap.h"
#include<algorithm>

using namespace std;
namespace gpstk
{
    /* Returns the total number of data elements in the map.
    * This method DOES NOT suppose that all the satellites have
    * the same number of type values.
    */
    size_t SatTypePtrMap::numElements() const
    {

        size_t numEle(0);
        for (auto &&it : *this)
            numEle += it.second->get_value().size();
        return numEle;

    }  // End of method 'SatTypePtrMap::numElements()'



       // Returns a SatIDSet with all the satellites present in this object.
    SatIDSet SatTypePtrMap::getSatID() const
    {

        SatIDSet satSet;

        for (auto &&it : *this)
            satSet.insert(it.first);

        return satSet;

    }  // End of method 'SatTypePtrMap::getSatID()'


	  // Returns a getSatSystems with only this satellite.
	  // @param satellite Satellite to be extracted.
	SatSystSet SatTypePtrMap::getSatSystems() const
	{	
		SatSystSet res;
		for (auto && it:*this)
			res.insert(it.first.system);
		return res;
	}


       // Returns a Vector with all the satellites present in this object.
    Vector<SatID> SatTypePtrMap::getVectorOfSatID() const
    {

        std::vector<SatID> temp;

        for (auto &&it : *this)
            temp.push_back(it.first);

        Vector<SatID> result;
        result = temp;

        return result;

    }  // End of method 'SatTypePtrMap::getVectorOfSatID()'



       // Returns a TypeIDSet with all the data types present in
       // this object. This does not imply that all satellites have these types.
    TypeIDSet SatTypePtrMap::getTypeID() const
    {

        TypeIDSet typeSet;

        for (auto &&it : *this)
            for (auto &&it1 : it.second->get_value())
                typeSet.insert(it1.first);
        return typeSet;

    }  // End of method 'SatTypePtrMap::getTypeID()'



       // Returns a SatTypePtrMap with only this satellite.
       // @param satellite Satellite to be extracted.
    SatTypePtrMap SatTypePtrMap::extractSatID(const SatID& satellite) const
    {

        SatIDSet satSet;
        satSet.insert(satellite);

        return extractSatID(satSet);

    }  // End of method 'SatTypePtrMap::extractSatID()'



       // Returns a SatTypePtrMap with only one satellite, identified
       // by the given parameters.
       // @param p Satellite PRN number.
       // @param p System the satellite belongs to.
    SatTypePtrMap SatTypePtrMap::extractSatID(const int& p,
        const SatID::SatelliteSystem& s) const
    {

        SatID tempSatellite(p, s);  // We build a temporary SatID object

        return extractSatID(tempSatellite);

    }  // End of method 'SatTypePtrMap::extractSatID()'



       // Returns a SatTypePtrMap with only these satellites.
       // @param satSet Set (SatIDSet) containing the satellites to
       //               be extracted.
    SatTypePtrMap SatTypePtrMap::extractSatID(const SatIDSet& satSet) const
    {
        SatTypePtrMap stvMap;

        for (auto&& sv : satSet)
        {
            auto itObs = find(sv);
            if (itObs != end())
                stvMap.emplace(sv, itObs->second);
        }

        return stvMap;

    }  // End of method 'SatTypePtrMap::extractSatID()'

    SatTypePtrMap SatTypePtrMap::extractSatSyst(const SatSystSet& systSet) const
    {
        SatTypePtrMap stvMap;

        for (auto it = begin(); it != end(); ++it)
        {

            if (systSet.find(it->first.system) != systSet.end())
                stvMap.emplace(*it);
        }

        return stvMap;

    }  // End of method 'SatTypePtrMap::extractSatID()'

	SatTypePtrMap SatTypePtrMap::extractSatSyst(SatID::SatelliteSystem ss) const
	{
		SatSystSet sset{ ss };
		return extractSatSyst(sset);
	}


	SatTypePtrMap& SatTypePtrMap::keepOnlySatSyst(const SatSystSet& satSystSet)
	{
		SatTypePtrMap stvMap(extractSatSyst(satSystSet));
		(*this) = stvMap;

		return (*this);

	}

	SatTypePtrMap& SatTypePtrMap::keepOnlySatSyst(SatID::SatelliteSystem ss) 
	{
		SatSystSet sset{ ss };
		keepOnlySatSyst(sset);
		return *this;
	}

       // Modifies this object, keeping only this satellite.
       // @param satellite Satellite to be kept.
    SatTypePtrMap& SatTypePtrMap::keepOnlySatID(const SatID& satellite)
    {

        SatIDSet satSet;
        satSet.insert(satellite);

        return keepOnlySatID(satSet);

    }  // End of method 'SatTypePtrMap::keepOnlySatID()'



       // Modifies this object, keeping only this satellite.
       // @param p Satellite PRN number.
       // @param p System the satellite belongs to.
    SatTypePtrMap& SatTypePtrMap::keepOnlySatID(const int& p,
        const SatID::SatelliteSystem& s)
    {

        SatID tempSatellite(p, s);  // We build a temporary SatID object

        return keepOnlySatID(tempSatellite);

    }  // End of method 'SatTypePtrMap::keepOnlySatID()'



       // Modifies this object, keeping only these satellites.
       // @param satSet Set (SatIDSet) containing the satellites to be kept.
    SatTypePtrMap& SatTypePtrMap::keepOnlySatID(const SatIDSet& satSet)
    {

        SatTypePtrMap stvMap(extractSatID(satSet));
        (*this) = stvMap;

        return (*this);

    }  // End of method 'SatTypePtrMap::keepOnlySatID()'



       // Returns a SatTypePtrMap with only this type of value.
       // @param type Type of value to be extracted.
	satTypeValueMap SatTypePtrMap::extractTypeID(const TypeID& type) const
    {

        TypeIDSet typeSet;
        typeSet.insert(type);

        return extractTypeID(typeSet);

    }  // End of method 'SatTypePtrMap::extractTypeID()'



       // Returns a SatTypePtrMap with only these types of data.
       // @param typeSet Set (TypeIDSet) containing the types of data
       //                to be extracted.
    satTypeValueMap SatTypePtrMap::extractTypeID(const TypeIDSet& typeSet) const
    {

		satTypeValueMap theMap;

		for (auto&& it: *this)
		{
			auto tvMap = it.second->get_value().extractTypeID(typeSet);
			if (tvMap.size() > 0)
				theMap[it.first] = tvMap;

		}

        return theMap;

    }  // End of method 'SatTypePtrMap::extractTypeID()'



       // Modifies this object, keeping only this type of data.
       // @param type Type of value to be kept.
    SatTypePtrMap& SatTypePtrMap::keepOnlyTypeID(const TypeID& type)
    {

        TypeIDSet typeSet;
        typeSet.insert(type);

        return keepOnlyTypeID(typeSet);

    }  // End of method 'SatTypePtrMap::keepOnlyTypeID()'



       // Modifies this object, keeping only these types of data.
       // @param typeSet Set (TypeIDSet) containing the types of data
       //                to be kept.
    SatTypePtrMap& SatTypePtrMap::keepOnlyTypeID(const TypeIDSet& typeSet)
    {
        for (auto it = begin(); it != end(); ++it)
        {
            it->second->get_value().keepOnlyTypeID(typeSet);
            /*auto typeMap = it->second->get_value();
            for (auto typeIt = typeMap.cbegin(); typeIt != typeMap.cend();)
            {
                if (typeSet.find(typeIt->first) == typeSet.end())
                    typeMap.erase(typeIt++);
                else
                    ++typeIt;
            }*/
        }

        return (*this);

    }  // End of method 'SatTypePtrMap::keepOnlyTypeID()'

	SatTypePtrMap& SatTypePtrMap::removeSatID(const SatID& satellite)
	{
		return removeSatID(SatIDSet{ satellite });
	}

       // Modifies this object, removing these satellites.
       // @param satSet Set (SatIDSet) containing the satellites
       //               to be removed.
    SatTypePtrMap& SatTypePtrMap::removeSatID(const SatIDSet& satSet)
    {
        for (auto&& it : satSet)
            (*this).erase(it);

        return (*this);

    }  // End of method 'SatTypePtrMap::removeSatID()'

    SatTypePtrMap& SatTypePtrMap::removeSatSyst(const SatSystSet& satSystSet)
    {
        for (auto it = begin(); it != end();)
        {
            if (satSystSet.find(it->first.system) != satSystSet.end())
                (*this).erase(it++);
            else
                ++it;
        }

        return (*this);

    }  // End of method 'SatTypePtrMap::removeSatSyst()'

    SatTypePtrMap& SatTypePtrMap::removeSatSyst(SatID::SatelliteSystem syst)
    {
        SatSystSet ssset;
        ssset.insert(syst);
        return removeSatSyst(ssset);
    }

	
    // Modifies this object, removing this type of data.
    // @param type Type of value to be removed.
    SatTypePtrMap& SatTypePtrMap::removeTypeID(const TypeID& type)
    {
        TypeIDSet typeSet{ type };
        removeTypeID(typeSet);

        return (*this);

    }  // End of method 'SatTypePtrMap::removeTypeID()'



       // Modifies this object, removing these types of data.
       // @param typeSet Set (TypeIDSet) containing the types of data
       //                to be kept.
    SatTypePtrMap& SatTypePtrMap::removeTypeID(const TypeIDSet& typeSet)
    {
        for (auto it = begin(); it != end(); ++it)
            (*it).second->get_value().removeTypeID(typeSet);

        return (*this);

    }  // End of method 'SatTypePtrMap::removeTypeID()'



       // Returns a GPSTk::Vector containing the data values with this type.
       // @param type Type of value to be returned.
       // This method returns zero if a given satellite does not have this type.
    Vector<double> SatTypePtrMap::getVectorOfTypeID(const TypeID& type) const
    {
        Vector<double> result(this->size());
        int i = 0;
        for (auto&& it : *this)
        {
            auto itObs = it.second->get_value().find(type);
            result[i] = (itObs != it.second->get_value().end()) ? (*itObs).second : 0.0;
            ++i;
        }

        return result;

    }  // End of method 'SatTypePtrMap::getVectorOfTypeID()'



       // Returns a GPSTk::Matrix containing the data values in this set.
       // @param typeSet  TypeIDSet of values to be returned.
    Matrix<double> SatTypePtrMap::getMatrixOfTypes(const TypeIDSet& typeSet)
        const
    {

        // First, let's create a Matrix<double> of the proper size
        Matrix<double> tempMat((*this).numSats(), typeSet.size(), 0.0);

        size_t numRow(0), numCol(0);

        for (auto&& it : *this)
        {
            numCol = 0;

            for (auto pos = typeSet.begin(); pos != typeSet.end(); ++pos)
            {
                auto itObs = it.second->get_value().find(*pos);
                if (itObs != it.second->get_value().end())
                    tempMat(numRow, numCol) = (*itObs).second;

                ++numCol;
            }
            ++numRow;
        }

        return tempMat;

    }  // End of method 'SatTypePtrMap::getMatrixOfTypes()'



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
    SatTypePtrMap& SatTypePtrMap::insertTypeIDVector(const TypeID& type,
        const Vector<double> dataVector)
        throw(NumberOfSatsMismatch)
    {

        if (dataVector.size() == (*this).numSats())
        {
            size_t pos = 0;

            for (SatTypePtrMap::iterator it = (*this).begin();
                it != (*this).end();
                ++it)
            {
                (*it).second->get_value()[type] = dataVector[pos];
                ++pos;
            }

            return (*this);

        }
        else
        {
            GPSTK_THROW(NumberOfSatsMismatch(" Number of data values in vector \
and number of satellites do not match"));
        }

    }  // End of method 'SatTypePtrMap::insertTypeIDVector()'



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
    SatTypePtrMap& SatTypePtrMap::insertMatrix(const TypeIDSet& typeSet,
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

            for (SatTypePtrMap::iterator it = (*this).begin();
                it != (*this).end();
                ++it)
            {

                size_t idx(0);

                for (TypeIDSet::const_iterator itSet = typeSet.begin();
                    itSet != typeSet.end();
                    ++itSet)
                {
                    (*it).second->get_value()[(*itSet)] = dataMatrix(pos, idx);
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

    }  // End of method 'SatTypePtrMap::insertMatrix()'



       /* Returns the data value (double) corresponding to provided SatID
        * and TypeID.
        *
        * @param satellite     Satellite to be looked for.
        * @param type          Type to be looked for.
        */
    double SatTypePtrMap::getValue(const SatID& satellite,
        const TypeID& type) const
        throw(SatIDNotFound, TypeIDNotFound)
    {

        SatTypePtrMap::const_iterator itObs((*this).find(satellite));
        if (itObs != (*this).end())
        {
            return (*itObs).second->get_value().getValue(type);
        }
        else
        {
            GPSTK_THROW(SatIDNotFound("SatID not found in map"));
        }

    }  // End of method 'SatTypePtrMap::getValue()'



       // Returns a reference to the typeValueMap with corresponding SatID.
       // @param type Type of value to be looked for.
    typeValueMap& SatTypePtrMap::operator()(const SatID& satellite)
        throw(SatIDNotFound)
    {

        SatTypePtrMap::iterator itObs((*this).find(satellite));
        if (itObs != (*this).end())
        {
            return (*itObs).second->get_value();
        }
        else
        {
            GPSTK_THROW(SatIDNotFound("SatID not found in map"));
        }

    }  // End of method 'SatTypePtrMap::operator()'


	// Convenience output method for structure satTypeValueMap
	std::ostream& SatTypePtrMap::dump(std::ostream& s,
		int mode) const
	{

		for (auto it = (*this).begin();
			it != (*this).end();
			it++)
		{

			// First, print satellite (system and PRN)
			s << (*it).first << " ";

			for (auto itObs = (*it).second->get_value().begin();
				itObs != (*it).second->get_value().end();
				itObs++)
			{

				if (mode == 1)
				{
					s << (*itObs).first << " ";
				}

				s << (*itObs).second << " ";

			}  // End of 'for( typeValueMap::const_iterator itObs = ...'

			s << endl;

		}  // End of 'for( satTypeValueMap::const_iterator it = ...'

		   // Let's return the 'std::ostream'
		return s;

	}  // End of method 'satTypeValueMap::dump()'
}
