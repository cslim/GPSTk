
#include "ARSimple.hpp"

namespace gpstk
{
   Vector<double> ARSimple::resolveIntegerAmbiguity( 
                                                 const Vector<double>& ambFloat, 
                                                 const Matrix<double>& ambCov )
      throw(ARException)
   {
         // check input
      if( ambFloat.size()!=ambCov.rows() || ambFloat.size()!=ambCov.cols() )
      {
         ARException e("The dimension of input does not match.");
         GPSTK_THROW(e);
      }

      const size_t n = ambFloat.size();

      Vector<double> ambFixed(n,0.0);
      for(int i = 0; i < ambFloat.size(); i++)
      {
         const double threshold = 3.0 * std::sqrt(ambCov(i,i));

         double lowerValue = ambFloat(i) - threshold;
         double upperValue = ambFloat(i) + threshold;

         if( std::abs(upperValue-lowerValue) <= 1.0 )
         {
            ambFixed(i) = double( std::round(ambFloat(i)) );
         }
         else
         {
            ambFixed(i) = ambFloat(i);       // keep it with float value
         }
      }

      return ambFixed;

   }  // End of method 'ARSimple::resolveIntegerAmbiguity()'

   
}   // End of namespace gpstk
