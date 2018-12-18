#pragma once
#include"ITypeValueMap.h"
#include"Matrix.hpp"
#include"satTypeValueMap.hpp"

namespace gpstk
{   


    class SatTypePtrMap : public std::map<SatID,  std::shared_ptr<ITypeValueMap> >
    {

    public:

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

		/// Returns a SatIDSet with all the satellites present in this object.
		SatSystSet getSatSystems() const;

        /// Returns a Vector with all the satellites present in this object.
        Vector<SatID> getVectorOfSatID() const;


        /// Returns a TypeIDSet with all the data types present in
        /// this object.  This does not imply that all satellites have
        /// these types.
        TypeIDSet getTypeID() const;


        /// Returns a SatTypePtrMap with only this satellite.
        /// @param satellite Satellite to be extracted.
        SatTypePtrMap extractSatID(const SatID& satellite) const;


        /// Returns a SatTypePtrMap with only one satellite, identified
        /// by the given parameters.
        /// @param p Satellite PRN number.
        /// @param p System the satellite belongs to.
        SatTypePtrMap extractSatID(const int& p,
            const SatID::SatelliteSystem& s) const;


        /// Returns a SatTypePtrMap with only these satellites.
        /// @param satSet Set (SatIDSet) containing the satellites to
        ///               be extracted.
        SatTypePtrMap extractSatID(const SatIDSet& satSet) const;

        /// Returns a SatTypePtrMap with only these Satellite system.
        /// @param satSet Set (SatSystSet) containing the Satellite system to
        ///               be extracted.
        SatTypePtrMap extractSatSyst(const SatSystSet& sustSet) const;
        SatTypePtrMap extractSatSyst(SatID::SatelliteSystem s) const;

		SatTypePtrMap& keepOnlySatSyst(const SatSystSet& satSystSet);
		SatTypePtrMap& keepOnlySatSyst(SatID::SatelliteSystem s);

        /// Modifies this object, keeping only this satellite.
        /// @param satellite Satellite to be kept.
        SatTypePtrMap& keepOnlySatID(const SatID& satellite);


        /// Modifies this object, keeping only this satellite.
        /// @param p Satellite PRN number.
        /// @param p System the satellite belongs to.
        SatTypePtrMap& keepOnlySatID(const int& p,
            const SatID::SatelliteSystem& s);


        /// Modifies this object, keeping only these satellites.
        /// @param satSet Set (SatIDSet) containing the satellites to be kept.
        SatTypePtrMap& keepOnlySatID(const SatIDSet& satSet);


        /// Returns a SatTypePtrMap with only this type of value.
        /// @param type Type of value to be extracted.
		satTypeValueMap extractTypeID(const TypeID & typeSet) const;

        /// Returns a SatTypePtrMap with only these types of data.
        /// @param typeSet Set (TypeIDSet) containing the types of data
        ///                to be extracted.
		satTypeValueMap extractTypeID(const TypeIDSet& typeSet) const;


        /// Modifies this object, keeping only this type of data.
        /// @param type Type of value to be kept.
        SatTypePtrMap& keepOnlyTypeID(const TypeID& type);


        /// Modifies this object, keeping only these types of data.
        /// @param typeSet Set (TypeIDSet) containing the types of data
        ///                to be kept.
        SatTypePtrMap& keepOnlyTypeID(const TypeIDSet& typeSet);


        /// Modifies this object, removing this satellite.
        /// @param satellite Satellite to be removed.
		SatTypePtrMap& removeSatID(const SatID& satellite);


        /// Modifies this object, removing these satellites.
        /// @param satSet Set (SatIDSet) containing the satellites
        ///               to be removed.
        SatTypePtrMap& removeSatID(const SatIDSet& satSet);

        SatTypePtrMap& removeSatSyst(const SatSystSet& satSet);

        SatTypePtrMap& removeSatSyst(SatID::SatelliteSystem syst);

        /// Modifies this object, removing this type of data.
        /// @param type Type of value to be removed.
        SatTypePtrMap& removeTypeID(const TypeID& type);


        /// Modifies this object, removing these types of data.
        /// @param typeSet Set (TypeIDSet) containing the types of data
        ///                to be kept.
        SatTypePtrMap& removeTypeID(const TypeIDSet& typeSet);


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
        SatTypePtrMap& insertTypeIDVector(const TypeID& type,
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
        SatTypePtrMap& insertMatrix(const TypeIDSet& typeSet,
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
        virtual ~SatTypePtrMap() {};
        
    };


}

