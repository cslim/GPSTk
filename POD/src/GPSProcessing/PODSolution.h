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

        PODSolution(ConfDataReader & confReader, const string& dir);
        virtual ~PODSolution(){};
    
    protected:
        virtual bool PPPprocess() override;
        virtual double mapSNR(double  value) override;
    };
}

#endif // !POD_POD_SOLUTION_H