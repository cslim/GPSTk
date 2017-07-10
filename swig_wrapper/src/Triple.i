// C++ extensions:
%extend gpstk::Triple {
   double At(unsigned int i) {
      // element [] access
      return $self->theArray[i];
   }
   
   //----------------------------------------
   // Alternate implementation of indexing
   // Remove this comment block after testing
   //----------------------------------------
   //    double __getitem__(unsigned int i) {
   //       return (*($self))[i];
   //    }

   gpstk::Triple scale(double scalar) {
      // since operator* can't be wrapped
      return Triple(scalar * $self->theArray[0],
                    scalar * $self->theArray[1],
                    scalar * $self->theArray[2]);
   }
   int Length() {
      return 3;
   }
 
   std::string ToString() {
      std::ostringstream ss;
      size_t i;
      for(i = 0; i < $self->size() - 1; i++)
         ss << (*($self))[i] << ", ";
      ss << (*($self))[i];
      return ss.str();
   }
 
   
   
   
};

