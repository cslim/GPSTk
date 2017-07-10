//Let's rename relational  operators 
%define RELATIONAL_OPERATORS(CLASSNAME)
%rename(EqEq) gpstk::## CLASSNAME ##::operator==(const CLASSNAME &)const;
%rename(NoEq) gpstk::## CLASSNAME ##::operator!=(const CLASSNAME &)const;
%rename(Lt) gpstk::## CLASSNAME ##::operator<(const CLASSNAME&)const;
%rename(LtEq) gpstk::## CLASSNAME ##::operator<=(const CLASSNAME&)const;
%rename(Gt) gpstk::## CLASSNAME ##::operator>(const CLASSNAME&)const;
%rename(GtEq) gpstk::## CLASSNAME ##::operator>=(const CLASSNAME&)const;

%enddef


%define EQUALS_DUMMY(CLASSNAME)
%extend gpstk:: ## CLASSNAME ##  { 
   gpstk::##CLASSNAME##::operator==(const gpstk::CLASSNAME&) const
   {
   return false;
   }
}
%enddef

EQUALS_DUMMY(SatID)
EQUALS_DUMMY(RinexDatum)
EQUALS_DUMMY(RinexObsType)
EQUALS_DUMMY(RinexObsID)
EQUALS_DUMMY(Rinex3ObsData)

