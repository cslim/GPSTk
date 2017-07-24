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
        PODSolution(ConfDataReader & confReader, string dir);
        virtual ~PODSolution(){};

    protected:
        virtual void PRProcess() override;
        virtual bool PPPprocess() override;
    };
}


#endif // !POD_POD_SOLUTION_H