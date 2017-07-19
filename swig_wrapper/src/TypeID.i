%extend gpstk::TypeID {
    static gpstk::TypeID::ValueType gpstk::TypeID::ToTypeID(
                               const gpstk::RinexObsType& rot, 
                               const gpstk::RinexSatID& sat )
    {
     return gpstk::ConvertToTypeID(rot,sat);
    }
    static gpstk::TypeID::ValueType gpstk::TypeID::ToTypeID(
                               const gpstk::RinexObsID& roi, 
                               const gpstk::RinexSatID& sat )
    {
     return gpstk::ConvertToTypeID(roi,sat);
    }
	static std::string gpstk::TypeID::asString(gpstk::TypeID::ValueType val)
    {
      auto it = gpstk::TypeID::tStrings.find(val);

      if(it != gpstk::TypeID::tStrings.end())
      {
         return it->second;
      }
      else
      {
        return gpstk::TypeID::tStrings.at(gpstk::TypeID::ValueType::Unknown);
      }
    }

};

