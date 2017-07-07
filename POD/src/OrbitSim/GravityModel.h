/**
* @file GravityModel.hpp
* 
*/

#ifndef POD_GRAVITY_MODEL_H
#define POD_GRAVITY_MODEL_H
#include "Force.h"
#include"ForceModelData.h"

namespace POD
{

      /// @ingroup GeoDynamics 
      //@{


      /** This class computes the body fixed acceleration due to the harmonic 
       *  gravity field of the central body
       */
   class GravityModel : public Force
   {
   public:

        ///default constructor.
       GravityModel(const GravityModelData &gData);

         /// Default destructor
      virtual ~GravityModel() {};

         /// return the force model index
      virtual int forceIndex() const
      { return FMI_GEOEARTH; }


   protected:

       GravityModelData  gmData_;

   }; // End of namespace 'gpstk'

      // @}

}  // End of namespace 'gpstk'

#endif   // POD_GRAVITY_MODEL_H
