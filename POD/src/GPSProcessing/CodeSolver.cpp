#include"CodeSolver.h"

namespace pod
{
  
     NeillTropModel CodeSolver::initTropoModel(const Position &nominalPos, int DoY)
    {
        return NeillTropModel(nominalPos.getAltitude(), nominalPos.getGeodeticLatitude(), DoY);
    }
     double CodeSolver::getTropoCorrection(
         const Position &rxPos,
         const Position &svPos,
         const CommonTime &t
     ) const
     {
         double height = rxPos.getHeight();
         if ( height < 10000.0 || height > -1000)
             return  tropo->correction(rxPos, svPos, t);
         else
         {
             auto e = gpstk::InvalidRequest();
             e.addLocation(ExceptionLocation(__FILE__, __FUNCTION__, __LINE__));
             e.addText("Invalid height for tropospheric correction computation");
             GPSTK_THROW(e)
         }
     }
}
