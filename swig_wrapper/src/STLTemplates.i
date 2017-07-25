
//////////////////////////////////////////////////
//            std::vector wraps
//////////////////////////////////////////////////
//%template(std_vector_SatID)      std::vector<gpstk::SatID>;
%template(vector_double)     std::vector<double>;
%template(vector_int)        std::vector<int>;
%template(vector_string)     std::vector<std::string>;

SWIG_STD_VECTOR_ENHANCED(RinexDatum)
%template(vector_RinexDatum) std::vector<gpstk::RinexDatum>;
SWIG_STD_VECTOR_ENHANCED(RinexSatID)
%template(vector_RinexSatID) std::vector<gpstk::RinexSatID>;
SWIG_STD_VECTOR_ENHANCED(SatID)
%template(std_vector_SatID) std::vector<gpstk::SatID>;

SWIG_STD_VECTOR_ENHANCED(RinexObsID)
%template(vector_RinexObsID) std::vector<gpstk::RinexObsID>;
SWIG_STD_VECTOR_ENHANCED(RinexObsType)
%template(vector_RinexObsType) std::vector<gpstk::RinexObsType>;
SWIG_STD_VECTOR_ENHANCED(Rinex3ObsData)
%template(vector_Rinex3ObsData) std::vector<gpstk::Rinex3ObsData>;

%template(vector_gnssRinex) std::vector< gpstk::gnssRinex >;

/////////////////////////////////////////////////////////////////
//             std::map wraps
/////////////////////////////////////////////////////////////////
//%template()     std::pair<swig::SwigPtr_PyObject, swig::SwigPtr_PyObject>;
//%template(cmap) std::map<swig::SwigPtr_PyObject, swig::SwigPtr_PyObject>;

%template(map_string_double) std::map<std::string, double>;
%template(map_string_int)    std::map<std::string, int>;
%template(map_string_char)   std::map<std::string, char>;

%template(map_double_string) std::map<double, std::string>;
%template(map_double_int)    std::map<double, int>;

%template(map_int_char)   std::map<int, char>;
%template(map_int_double) std::map<int, double>;
%template(map_int_string) std::map<int, std::string>;

%template(map_char_string) std::map<char, std::string>;
%template(map_char_int)    std::map<char, int>;


// AKA RinexObsData::RinexObsTypeMap
%template(map_RinexObsType_RinexDatum) std::map<gpstk::RinexObsType, gpstk::RinexDatum>;

// AKA RinexObsData::RinexSatMap
%template(map_RinexSatID_RinexObsTypeMap) std::map<gpstk::SatID, std::map<gpstk::RinexObsType, gpstk::RinexDatum> >;

// the type of Rinex3ObsHeader::mapObsTypes
%template(map_string_vector_RinexObsID) std::map<std::string, std::vector<gpstk::RinexObsID> >;

%template(map_RinexSatID_vector_RinexDatum) std::map<gpstk::RinexSatID, std::vector<gpstk::RinexDatum> >;

// These need to be instantiated so the SvObsEpoch and ObsEpoch classes will work correctly
%template(map_SvObsEpoch) std::map< gpstk::ObsID, double >;
%template(map_ObsEpoch) std::map<gpstk::SatID, gpstk::SvObsEpoch>;

// AKA ObsEpochMap
%template(ObsEpochMap) std::map<gpstk::CommonTime, gpstk::ObsEpoch>;


// These are needed to properly interpret an ObsID instance
%template(map_ObservationType_string) std::map<enum gpstk::ObsID::ObservationType, std::string>;
%template(map_CarrierBand_string) std::map<enum gpstk::ObsID::CarrierBand, std::string>;
%template(map_TrackingCode_string) std::map<enum gpstk::ObsID::TrackingCode, std::string>;
%template(map_char_ObservationType) std::map<char, enum gpstk::ObsID::ObservationType>;
%template(map_char_CarrierBand) std::map<char, enum gpstk::ObsID::CarrierBand>;
%template(map_char_TrackingCode) std::map<char, enum gpstk::ObsID::TrackingCode>;
%template(map_ObservationType_char) std::map<enum gpstk::ObsID::ObservationType,char>;
%template(map_CarrierBand_char) std::map<enum gpstk::ObsID::CarrierBand,char>;
%template(map_TrackingCode_char) std::map<enum gpstk::ObsID::TrackingCode,char>;


// Map holding TypeID with corresponding numeric value.
%template(map_typeID_double)    std::map<gpstk::TypeID, double>;

// Map holding SatID with corresponding numeric value.
%template(map_SatID_double)    std::map<gpstk::SatID, double>;
%template(map_SatID_typeValueMap)    std::map<gpstk::SatID, gpstk::typeValueMap>;

%template(map_SatID_map_type_double)    std::map<gpstk::SatID, std::map<gpstk::TypeID, double>>;

// ValidType wraps
%template(validtype_float) gpstk::ValidType<float>;
%template(validtype_double) gpstk::ValidType<double>;
%template(validtype_char) gpstk::ValidType<char>;
%template(validtype_short) gpstk::ValidType<short>;
%template(validtype_int) gpstk::ValidType<int>;
%template(validtype_long) gpstk::ValidType<long>;
%template(validtype_uchar) gpstk::ValidType<unsigned char>;
%template(validtype_ushort) gpstk::ValidType<unsigned short>;
%template(validtype_uint) gpstk::ValidType<unsigned int>;
%template(validtype_ulong) gpstk::ValidType<unsigned long>;

//////////////////////////////////////////////////
//             std::set wraps
//////////////////////////////////////////////////
%template(set_int) std::set<int>;
%template(set_double) std::set<double>;
%template(set_string) std::set<std::string>;

// Set containing TypeID objects.
%template(set_TypeID) std::set<gpstk::TypeID>;

// Set containing SatID objects.
%template(set_SatID) std::set<gpstk::SatID>;

// Set containing SourceID objects.
%template(set_SourceID) std::set<gpstk::SourceID>;
//////////////////////////////////////////////////
//             std::list wraps
//////////////////////////////////////////////////



