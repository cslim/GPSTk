
%extend gpstk::RinexMetData {
   double _lookup_data(int x) {
      return $self->data[static_cast<RinexMetHeader::RinexMetType>(x)];
   }
}


