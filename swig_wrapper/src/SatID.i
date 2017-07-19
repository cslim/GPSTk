%newobject gpstk::SatID::SatID(int p); //Change the ownership of the variable p in the SatID program

%extend gpstk::SatID { //Attach following values to the structure SatID
   SatID(int p) {
      gpstk::SatID helper;
      return new gpstk::SatID(p, gpstk::SatID::systemGPS);
   }

   gpstk::SatID::operator==(const gpstk::SatID& right) const
   {
    return (((*($self)).system == right.system) && (((*($self)).id == right.id)); 
   }
};

