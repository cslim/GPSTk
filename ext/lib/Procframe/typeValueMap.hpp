#pragma once 
#include<map>
#include<set>
#include"TypeID.hpp"

namespace gpstk
{

	/// Thrown when attempting to access a value and the corresponding TypeID
	/// does not exist in the map.
	/// @ingroup exceptiongroup
	NEW_EXCEPTION_CLASS(TypeIDNotFound, gpstk::Exception);

	typedef std::set<TypeID> TypeIDSet;

	struct typeValueMap : std::map<TypeID, double>
	{

		/// Returns the number of different types available.
		inline size_t numTypes() const
		{
			return (*this).size();
		}


		/// Returns a TypeIDSet with all the data types present in
		/// this object.
		TypeIDSet getTypeID() const;


		/// Returns a typeValueMap with only this type of data.
		/// @param type Type of value to be extracted.
		typeValueMap extractTypeID(const TypeID& type) const;


		/// Returns a typeValueMap with only these types of data.
		/// @param typeSet Set (TypeIDSet) containing the types of data to
		///                be extracted.
		typeValueMap extractTypeID(const TypeIDSet& typeSet) const;


		/// Modifies this object, keeping only this type of data.
		/// @param type Type of value to be kept.
		typeValueMap& keepOnlyTypeID(const TypeID& type);


		/// Modifies this object, keeping only these types of data.
		/// @param typeSet Set (TypeIDSet) containing the types of data
		///                to be kept.
		typeValueMap& keepOnlyTypeID(const TypeIDSet& typeSet);


		/// Modifies this object, removing this type of data.
		/// @param type Type of value to be removed.
		typeValueMap& removeTypeID(const TypeID& type)
		{
			(*this).erase(type); return (*this);
		}


		/// Modifies this object, removing these types of data.
		/// @param typeSet Set (TypeIDSet) containing the types of data
		///                to be kept.
		typeValueMap& removeTypeID(const TypeIDSet& typeSet);


		/** Returns the data value (double) corresponding to provided type.
		 *
		 * @param type       Type of value to be looked for.
		 */
		double getValue(const TypeID& type) const
			throw(TypeIDNotFound);


		/// Returns a reference to the data value (double) with
		/// corresponding type.
		/// @param type Type of value to be looked for.
		double& operator()(const TypeID& type)
			throw(TypeIDNotFound);


		/// Destructor.
		virtual ~typeValueMap() {};

	};  // End typeValueMap


}