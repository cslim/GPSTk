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
        PPPSolution(GnssDataStore_sptr  confReader);
        virtual ~PPPSolution(){};

    protected:
        virtual bool processCore() override;
        NeillTropModel tropModel;
        virtual void updateRequaredObs() override;

    protected: virtual void printSolution( ofstream& of, const SolverLMS& solver, const CommonTime& time, GnssEpoch& gEpoch ) override;

    };
}

#endif // !POD_PPP_SOLUTION_H