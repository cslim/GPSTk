#include "GnssSolution.h"
using namespace std;
using namespace gpstk;

namespace pod
{
     std::ostream& GnssSolution:: printMsg(const gpstk::CommonTime& time, const char* msg)
     {
        return( cout << "Epoch: " << CivilTime(time) << " "<< msg << endl);
     }
   
    GnssSolution::GnssSolution(GnssDataStore_sptr gnssData):data(gnssData)
    {
    }

    GnssSolution::~GnssSolution()
    {
    }
}
