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
     }
}
