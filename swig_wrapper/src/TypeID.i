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
	static gpstk::TypeID::ValueType gpstk::TypeID::Parse(std::string name)
	{
	      auto it = gpstk::TypeID::fStrings.find(name);

      if(it != gpstk::TypeID::fStrings.end())
      {
         return it->second;
      }
      else
      {
         InvalidRequest e("There are no  TypeID name as '"
            + name + "'.");
         GPSTK_THROW(e);
      }
	}

};

