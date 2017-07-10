
//Let's rename binnary operators 
% define BINARY_OPERATORS_HELPER(CLASSNAME)
%rename(EqEq) gpstk::## CLASSNAME ##::operator==(const ## CLASSNAME ##&)const;
%rename(NoEq) gpstk::## CLASSNAME ##::operator!=(const ## CLASSNAME ##&)const;
%rename(Lt) gpstk::## CLASSNAME ##::operator<(const ## CLASSNAME ##&)const;
%rename(LtEq) gpstk::## CLASSNAME ##::operator<=(const ## CLASSNAME ##&)const;
%rename(Gt) gpstk::## CLASSNAME ##::operator>(const ## CLASSNAME ##&)const;
%rename(GtEq) gpstk::## CLASSNAME ##::operator>=(const ## CLASSNAME ##&)const;

%enddef
