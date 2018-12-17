#pragma once
#include"DataStructures.hpp"
#include"FilterParameter.h"

namespace pod
{
    ///select satellites with maximum elevation angle for each satellite system
    class RefSatHandler
    {
    public:
         RefSatHandler() {};
        ~RefSatHandler() {};
        
        // return satellites set, contains satellites with maximum elevation for each satellite system
        gpstk::SatIDSet  getRefSats(gpstk::IRinex & gRin);
        
        //return DD to SD transition matrix
        gpstk::Matrix<double> getSD2DDMatrix(
            gpstk::IRinex & gData,
            const  gpstk::SatIDSet& svs,
            const  gpstk::SatSystSet& ss,
            gpstk::SatIDSet &refSvSet);
    };
}

