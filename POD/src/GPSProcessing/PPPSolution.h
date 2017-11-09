#ifndef POD_PPP_SOLUTION_H
#define POD_PPP_SOLUTION_H

#include"ConfDataReader.hpp"
#include "PPPSolutionBase.h"
#include"NeillTropModel.hpp"

using namespace gpstk;
namespace pod
{
    class PPPSolution : public PPPSolutionBase
    {
    public:
        PPPSolution(ConfDataReader & confReader, string dir);
        virtual ~PPPSolution(){};

    protected:
        virtual bool PPPprocess() override;
        NeillTropModel tropModel;

    };
}

#endif // !POD_PPP_SOLUTION_H