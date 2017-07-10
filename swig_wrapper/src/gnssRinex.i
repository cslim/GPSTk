// additional methods to ::gnssRinex class
%extend gpstk::gnssRinex {
  
         bool   readRinex3ObsStream( gpstk::Rinex3ObsStream& strm) {
         Rinex3ObsData rod;
         strm >> rod;
		 
         // If the header hasn't been read, read it...
         if(!strm.headerRead) strm >> strm.header;

         // Clear out this object
         Rinex3ObsHeader& roh = strm.header;
         // Fill data
         (*($self)).header.source.type = SatIDsystem2SourceIDtype(roh.fileSysSat);
         (*($self)).header.source.sourceName = roh.markerName;
         (*($self)).header.antennaType = roh.antType;
         (*($self)).header.antennaPosition = roh.antennaPosition;
         (*($self)).header.epochFlag = rod.epochFlag;
         (*($self)).header.epoch = rod.time;
		 

         (*($self)).body = satTypeValueMapFromRinex3ObsData(roh, rod);
       return !strm.eof();
   }
   
   gpstk::satTypeValueMap& getBody(){
	   return (*($self)).body;
   }

         std::map<gpstk::SatID, gpstk::typeValueMap>  getBodyAsMap()
      {
          return (std::map<gpstk::SatID, gpstk::typeValueMap>)((*($self)).body);
      }
   /*
      gpstk::RinexObsStream&  readRinexObsStream( gpstk::RinexObsStream& strm) {
     RinexObsData rod;
         strm >> rod;

         // Fill data
         (*($self)).header.source.type = SatIDsystem2SourceIDtype(roh.fileSysSat);
         (*($self)).header.source.sourceName = roh.markerName;
         (*($self)).header.antennaType = roh.antType;
         (*($self)).header.antennaPosition = roh.antennaPosition;
         (*($self)).header.epochFlag = rod.epochFlag;
         (*($self)).header.epoch = rod.time;

         (*($self)).body = satTypeValueMapFromRinexObsData(roh, rod);
       return strm;
   }
   */
}
