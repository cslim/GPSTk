#include"typeValueMap.hpp"

namespace gpstk
{
	
      ////// typeValueMap //////


      // Returns a TypeIDSet with all the data types present in
      // this object.
   TypeIDSet typeValueMap::getTypeID() const
   {

      TypeIDSet typeSet;

      for( typeValueMap::const_iterator pos = (*this).begin();
           pos != (*this).end();
           ++pos )
      {
         typeSet.insert( (*pos).first );
      }

      return typeSet;

   }  // End of method 'typeValueMap::getTypeID()'



      // Returns a typeValueMap with only this type of data.
      // @param type Type of value to be extracted.
   typeValueMap typeValueMap::extractTypeID(const TypeID& type) const
   {

      TypeIDSet typeSet;
      typeSet.insert(type);

      return extractTypeID(typeSet);

   }  // End of method 'typeValueMap::extractTypeID()'



      // Returns a typeValueMap with only these types of data.
      // @param typeSet Set (TypeIDSet) containing the types of data to
      //                be extracted.
   typeValueMap typeValueMap::extractTypeID(const TypeIDSet& typeSet) const
   {

      typeValueMap tvMap;

      for( TypeIDSet::const_iterator pos = typeSet.begin();
           pos != typeSet.end();
           ++pos )
      {

         typeValueMap::const_iterator itObs( (*this).find(*pos) );
         if( itObs != (*this).end() )
         {
            tvMap[ (*itObs).first ] = (*itObs).second;
         };
      }

      return tvMap;

   }  // End of method 'typeValueMap::extractTypeID()'



      // Modifies this object, keeping only this type of data.
      // @param type Type of value to be kept.
   typeValueMap& typeValueMap::keepOnlyTypeID(const TypeID& type)
   {

      TypeIDSet typeSet;
      typeSet.insert(type);

      return (keepOnlyTypeID(typeSet));

   }  // End of method 'typeValueMap::keepOnlyTypeID()'



      // Modifies this object, keeping only these types of data.
      // @param typeSet Set (TypeIDSet) containing the types of data
      //                to be kept.
   typeValueMap& typeValueMap::keepOnlyTypeID(const TypeIDSet& typeSet)
   {

      typeValueMap tvMap( (*this).extractTypeID(typeSet) );
      (*this) = tvMap;

      return (*this);

   }  // End of method 'typeValueMap::keepOnlyTypeID()'



      // Modifies this object, removing these types of data.
      // @param typeSet Set (TypeIDSet) containing the types of data
      //                to be kept.
   typeValueMap& typeValueMap::removeTypeID(const TypeIDSet& typeSet)
   {

      for( TypeIDSet::const_iterator pos = typeSet.begin();
           pos != typeSet.end();
           ++pos )
      {
         (*this).erase(*pos);
      }

      return (*this);

   }  // End of method 'typeValueMap::removeTypeID()'



      /* Returns the data value (double) corresponding to provided type.
       *
       * @param type       Type of value to be looked for.
       */
   double typeValueMap::getValue(const TypeID& type) const
      throw(TypeIDNotFound)
   {

      typeValueMap::const_iterator itObs( (*this).find(type) );
      if ( itObs != (*this).end() )
      {
         return (*itObs).second;
      }
      else
      {
         GPSTK_THROW(TypeIDNotFound("TypeID not found in map"));
      }

   }  // End of method 'typeValueMap::getValue()'



      // Returns a reference to the data value (double) with
      // corresponding type.
      // @param type Type of value to be looked for.
   double& typeValueMap::operator()(const TypeID& type)
      throw(TypeIDNotFound)
   {

      typeValueMap::iterator itObs ( (*this).find(type) );

      if ( itObs != (*this).end() )
      {
         return (*itObs).second;
      }
      else
      {
         GPSTK_THROW(TypeIDNotFound("TypeID not found in map"));
      }

   }  // End of method 'typeValueMap::operator()'

   std::ostream& operator<<(std::ostream& s, const typeValueMap& obj)
   {
	   for (auto&& it : obj)
		   s << it.first << " " << it.second;   
	   s << std::endl;
	   return s;
   }
}