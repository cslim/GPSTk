#pragma once
#include"DataStructures.hpp"
#include"Ambiguity.h"

namespace pod
{
    ///select satellites with maximum elevation angle for each satellite system
    class RefSatHandler
    {
    public:
        RefSatHandler();
        ~RefSatHandler() {};
        
        // return satellites set, contains satellites with maximum elevation for each satellite system
        gpstk::SatIDSet  getRefSats(gpstk::gnssRinex & gRin);
        
        //return DD to SD transition matrix
        gpstk::Matrix<double> getSD2DDMatrix(gpstk::gnssRinex & gData, const AmbiguitySet& ambs, gpstk::SatIDSet &svSet);
    };
}

