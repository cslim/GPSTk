#ifndef POD_POD_SOLUTION_H
#define POD_POD_SOLUTION_H

#include "ConfDataReader.hpp"
#include"PPPSolutionBase.h"
using namespace gpstk;
namespace pod
{
    class PODSolution : public PPPSolutionBase
    {
    public:

        ///Map GRACE C/No(Volts???) to dB*Hz by empirical relation
        static void mapSNR(gnssRinex & gRin);

        PODSolution(ConfDataReader & confReader, const string& dir);
        virtual ~PODSolution(){};
    
    protected:
        virtual void PRProcess() override;
        virtual bool PPPprocess() override;
    };
}

#endif // !POD_POD_SOLUTION_H