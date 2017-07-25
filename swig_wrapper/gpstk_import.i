%module swig_wrapper
%{
   #include "gpstk_swig.hpp"
	
	using namespace gpstk;
%}
%include<stl.i>
%include "std_set.i"
//%include"std_list.i"
//%include "operator_overload.i"
%include"gpstk_macros.i"


// Ignores on things we can't wrap
%ignore *::dump;  // takes a stream as a parameter
%ignore operator<<;
%ignore operator>>;
%ignore *::operator<<;
%ignore *::operator>>;
%ignore *::operator=;
%ignore *::operator++;
%ignore *::operator--;

%include "ValidType.hpp"
%include "typemaps.i"
%include "STLTemplates.i"
//%include "STLHelpers.i"

%include "exception.i"
//%rename(str) gpstk::Exception::what() const;
%include "Exception.hpp"
%include "FFStreamError.hpp"
%include "GPSTkException.i"


// =============================================================
//  Section 2: Time classes
// =============================================================
%rename (toString) *::operator std::string() const;
%rename(toCommonTime) *::convertToCommonTime() const;
%ignore *::operator CommonTime() const;

%ignore gpstk::TimeSystem::TimeSystem(int i);
%ignore gpstk::TimeSystem::getTimeSystem();


RELATIONAL_OPERATORS(TimeSystem)
%include "TimeSystem.hpp"
%include "TimeSystem.i"
RELATIONAL_OPERATORS(TimeTag)
%include "TimeTag.hpp"
%include "TimeConstants.hpp"

 // Long doubles are used in several interfaces but
 // swig really doesn't handle them
%apply double { long double };

%ignore gpstk::CommonTime::get;  // takes non-const values as parameters for output
RELATIONAL_OPERATORS(CommonTime)
%include "CommonTime.hpp"
RELATIONAL_OPERATORS(Week)
%include "Week.hpp"
RELATIONAL_OPERATORS(WeekSecond)
%include "WeekSecond.hpp"
%feature("notabstract") UnixTime;
RELATIONAL_OPERATORS(UnixTime)
%include "UnixTime.hpp"
%feature("notabstract") SystemTime;
RELATIONAL_OPERATORS(SystemTime)
%include "SystemTime.hpp"
%feature("notabstract") ANSITime;
RELATIONAL_OPERATORS(ANSITime)
%include "ANSITime.hpp"
%feature("notabstract") CivilTime;
%ignore gpstk::CivilTime::MonthNames;
%ignore gpstk::CivilTime::MonthAbbrevNames;
RELATIONAL_OPERATORS(CivilTime)
%include "CivilTime.hpp"
RELATIONAL_OPERATORS(GPSZcount)
%include "GPSZcount.hpp"
RELATIONAL_OPERATORS(GPSWeek)
%include "GPSWeek.hpp"
%feature("notabstract") GPSWeekSecond;
RELATIONAL_OPERATORS(GPSWeekSecond)
%include "GPSWeekSecond.hpp"
%feature("notabstract") GPSWeekZcount;
RELATIONAL_OPERATORS(GPSWeekZcount)
%include "GPSWeekZcount.hpp"
%feature("notabstract") JulianDate;
RELATIONAL_OPERATORS(JulianDate)
%include "JulianDate.hpp"
%feature("notabstract") BDSWeekSecond;
RELATIONAL_OPERATORS(BDSWeekSecond)
%include "BDSWeekSecond.hpp"
%feature("notabstract") GALWeekSecond;
RELATIONAL_OPERATORS(GALWeekSecond)
%include "GALWeekSecond.hpp"
%feature("notabstract") QZSWeekSecond;
RELATIONAL_OPERATORS(QZSWeekSecond)
%include "QZSWeekSecond.hpp"
%feature("notabstract") MJD;
RELATIONAL_OPERATORS(MJD)
%include "MJD.hpp"
%feature("notabstract") YDSTime;
RELATIONAL_OPERATORS(YDSTime)
%include "YDSTime.hpp"
%ignore gpstk::scanTime(TimeTag& btime, const std::string& str, const std::string& fmt);
%ignore gpstk::scanTime(CommonTime& btime, const std::string& str, const std::string& fmt);
%ignore gpstk::mixedScanTime(CommonTime& btime, const std::string& str, const std::string& fmt);

%include "TimeString.hpp"
%include "TimeString.i"
%include "TimeSystemCorr.hpp"


// =============================================================
//  Section 3: General/Utils classes
// =============================================================
// Utils stuff
%include "geometry.hpp"
%include "gps_constants.hpp"
RELATIONAL_OPERATORS(SatID)
%include "SatID.hpp"
%include "SatID.i"
%include "ObsIDInitializer.hpp"
RELATIONAL_OPERATORS(ObsID)
%include "ObsID.hpp"
//%include "ObsID.i"
%ignore gpstk::SV_ACCURACY_GLO_INDEX;  // wrapper added in GPS_URA.i
%include "GNSSconstants.hpp"
%ignore gpstk::Triple::operator[](const size_t index);
%ignore gpstk::Triple::operator()(const size_t index);
%ignore gpstk::Triple::operator*(double right, const Triple& rhs);
%ignore gpstk::Triple::theArray;
%include "Triple.hpp"
%include "Triple.i"

%ignore gpstk::ReferenceFrame::ReferenceFrame(int i);

%include "ReferenceFrame.hpp"
%include "EllipsoidModel.hpp"
%include "Xvt.hpp"

%ignore gpstk::Position::convertSphericalToCartesian(const Triple& tpr, Triple& xyz) throw();
%ignore gpstk::Position::convertCartesianToSpherical(const Triple& xyz, Triple& tpr) throw();
%ignore gpstk::Position::convertCartesianToGeodetic(const Triple& xyz, Triple& llh, const double A, const double eccSq) throw();
%ignore gpstk::Position::convertGeodeticToCartesian(const Triple&, llh, Triple& xyz, const double A, const double eccSq) throw();
%ignore gpstk::Position::convertCartesianToGeocentric(const Triple& xyz, Triple& llr) throw();
%ignore gpstk::Position::convertGeocentricToCartesian(const Triple& llr, Triple& xyz) throw();
%ignore gpstk::Position::convertGeocentricToGeodetic(const Triple& llr, Triple& geodeticllr, const double A, const double eccSq) throw();
%ignore gpstk::Position::convertGeodeticToGeocentric(const Triple& geodeticllh, Triple& llr, const double A, const double eccSq) throw();
%include "Position.hpp"
%include "Position.i"

%include "convhelp.hpp"
%include "Xv.hpp"
%ignore gpstk::VectorBase::operator[] (size_t i) const;
%ignore gpstk::VectorBase::operator() (size_t i) const;
%ignore gpstk::RefVectorBaseHelper::zeroTolerance;
%ignore gpstk::RefVectorBaseHelper::perator[] (size_t i);
%ignore gpstk::RefVectorBaseHelper::operator() (size_t i);
%ignore gpstk::RefVectorBaseHelper::zeroize();
%include "VectorBase.hpp"
%include "Vector.i"
%include "DataStatus.hpp"


// =============================================================
//  Section 4: "XvtStore and friends"
// =============================================================
// Renames on a few commonly used operators
%rename (toEngEphemeris) *::operator EngEphemeris() const;
%rename (toGalEphemeris) *::operator GalEphemeris() const;
%rename (toGloEphemeris) *::operator GloEphemeris() const;
%rename (toAlmOrbit) *::operator AlmOrbit() const;

%include "AstronomicalFunctions.hpp"
%include "PZ90Ellipsoid.hpp"
%include "WGS84Ellipsoid.hpp"

%include "XvtStore.hpp"
%template(XvtStore_SatID)  gpstk::XvtStore<gpstk::SatID>;
%template(XvtStore_string) gpstk::XvtStore<std::string>;

%include "gpstkplatform.h"
%include "FFStreamError.hpp"
%include "FileStore.hpp"
%include "FFData.hpp"
%include "EngNav.hpp"
%include "YumaBase.hpp"
// This is to silence warning about not knowing about the fstream base class
namespace std { class fstream {}; }
%include "FFStream.hpp"
%include "FFTextStream.hpp"
%include "AlmOrbit.hpp"
%include "YumaHeader.hpp"
%ignore gpstk::EngAlmanac::getUTC;
%include "EngAlmanac.hpp"

%include "OrbElemBase.hpp"
%include "OrbElem.hpp"
%include "OrbElemStore.hpp"
%include "AlmOrbit.hpp"
%include "YumaStream.hpp"
%include "YumaData.hpp"
%include "GPSAlmanacStore.hpp"
%template(FileStore_YumaHeader) gpstk::FileStore<gpstk::YumaHeader>;
%include "YumaAlmanacStore.hpp"

%include "SVNumXRef.hpp"

%include "RinexSatID.hpp"

%ignore gpstk::SV_ACCURACY_GPS_MIN_INDEX;
%ignore gpstk::SV_ACCURACY_GPS_NOMINAL_INDEX;
%ignore gpstk::SV_ACCURACY_GPS_MAX_INDEX;
%ignore gpstk::SV_CNAV_ACCURACY_GPS_MIN_INDEX;
%ignore gpstk::SV_CNAV_ACCURACY_GPS_NOM_INDEX;
%ignore gpstk::SV_CNAV_ACCURACY_GPS_MAX_INDEX;
%include "GPS_URA.hpp"
%include "GPS_URA.i"

%include "BrcClockCorrection.hpp"
%include "BrcKeplerOrbit.hpp"
%include "EphemerisRange.hpp"
// %include "EphReader.hpp"

// Ephemeris:
%include "OrbitEph.hpp"
%include "BDSEphemeris.hpp"
%include "EngEphemeris.hpp"
%include "GalEphemeris.hpp"
%include "GloEphemeris.hpp"
%include "GPSEphemeris.hpp"
%include "QZSEphemeris.hpp"

// RINEX format:
//%include "RinexSatID.hpp"
// RINEX obs:
%include "RinexObsBase.hpp"
%typemap(csbase) gpstk::RinexObsHeader::validBits "uint"
%include "RinexObsHeader.hpp"

%include "RinexObsData.hpp"
%include "RinexObsID.hpp"
%include "RinexObsStream.hpp"
// RINEX nav:
%include "RinexNavBase.hpp"
%typemap(csbase) gpstk::RinexNavHeader::validBits "uint"
%include "RinexNavHeader.hpp"
%include "RinexNavStream.hpp"
%include "RinexNavData.hpp"
// RINEX meteorological:
%include "RinexMetBase.hpp"
%typemap(csbase) gpstk::RinexMetHeader::validBits "uint"
%include "RinexMetHeader.hpp"
%include "RinexMetStream.hpp"

%ignore gpstk::RinexMetData::data;
%include "RinexMetData.hpp"
%include "RinexMetData.i"

 // RINEX 3 nav:
%include "Rinex3NavBase.hpp"
%include "RinexDatum.hpp"
%typemap(csbase) gpstk::Rinex3NavHeader::validBits "uint"
%include "Rinex3NavHeader.hpp"
%include "Rinex3NavStream.hpp"
%include "Rinex3NavData.hpp"
%include "OrbElemRinex.hpp"
// RINEX 3 clock/obs:
%include "Rinex3ClockBase.hpp"
%include "Rinex3ObsBase.hpp"
%typemap(csbase) gpstk::Rinex3ObsHeader::validBits "uint"
%include "Rinex3ObsHeader.hpp"

%include "Rinex3ObsData.hpp"
%include "Rinex3ObsStream.hpp"
%include "Rinex3ClockHeader.hpp"
%include "Rinex3ClockData.hpp"
%include "Rinex3ClockStream.hpp"
%include "Rinex3EphemerisStore.hpp"

// Ephemeris stores:
%include "OrbitEphStore.hpp"
%include "BDSEphemerisStore.hpp"
%include "GalEphemerisStore.hpp"
%include "GloEphemerisStore.hpp"
%include "GPSEphemerisStore.hpp"
%include "QZSEphemerisStore.hpp"
%template (FileStore_RinexNavHeader) gpstk::FileStore<gpstk::RinexNavHeader>;
%include "RinexEphemerisStore.hpp"

// SP3 format:
%include "TabularSatStore.hpp"
%include "ClockSatStore.hpp"
%include "SP3Base.hpp"
RELATIONAL_OPERATORS(SP3SatID)
%include "SP3SatID.hpp"
%include "SP3Header.hpp"
%include "SP3Data.hpp"
%include "SP3Stream.hpp"
%include "PositionSatStore.hpp"
%include "SP3EphemerisStore.hpp"
%include "RinexUtilities.hpp"

// SEM format:
%include "SEMBase.hpp"
%include "SEMHeader.hpp"
%include "SEMStream.hpp"
%include "SEMData.hpp"
%template(FileStore_SEMHeader) gpstk::FileStore<gpstk::SEMHeader>;
%include "SEMAlmanacStore.hpp"

%include "ObsEpochMap.hpp"
%include "WxObsMap.hpp"
%include "TropModel.hpp"
%include "PRSolution2.hpp"
%include "ExtractData.hpp"
%ignore gpstk::Expression::print(std::ostream& ostr) const;
%include "Expression.hpp"

// FIC format:
%include "FFBinaryStream.hpp"

%include "MoonPosition.hpp"
%include "SunPosition.hpp"
%include "PoleTides.hpp"
%include "SolidTides.hpp"

//////////////////////////////////
//GNSS data structures
//////////////////////////////////

RELATIONAL_OPERATORS(TypeID)
%rename (TypeID_Initializer) gpstk::TypeID::Initializer;
%include"TypeID.hpp"
%include"TypeID.i"
RELATIONAL_OPERATORS(SourceID)
%include"SourceID.hpp"
%include"DataHeaders.hpp"
%include"DataStructures.hpp"
%include"gnssRinex.i"
// GNSS data structure with source and epoch as header (common indexes) and satTypeValueMap as body.
%template(GnssDataSatTypeValue)    gpstk::gnssData<gpstk::sourceEpochHeader, std::map<gpstk::SatID, std::map<gpstk::TypeID, double>>>;

%include"Action.h"


// Geomatics:
%include "Geomatics.i"
%include "SunEarthSatGeometry.hpp"
%include"FileIO.i"




