#ifndef POD_POD_SOLUTION_H
#define POD_POD_SOLUTION_H

#include"PPPSolutionBase.h"
using namespace gpstk;
namespace pod
{
    class PODSolution : public PPPSolutionBase
    {


    public: PODSolution(GnssDataStore_sptr confData);
    public: virtual ~PODSolution() {};

    public: void process() override;

    protected: virtual bool processCore() override;

    protected: virtual double mapSNR(double  value) override;

    protected: virtual void updateRequaredObs() override;

    protected: virtual void printSolution(ofstream& of, const SolverLMS& solver, const CommonTime& time, GnssEpoch& gEpoch) override;

    protected: virtual void updateNomPos(const CommonTime& t, Position &pos) override;

    };
}

#endif // !POD_POD_SOLUTION_H