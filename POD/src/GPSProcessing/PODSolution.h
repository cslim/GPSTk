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

	protected:  void printSolution(std::ofstream& of, const SolverLMS& solver, const CommonTime& time, GnssEpoch& gEpoch) ;

    };
}

#endif // !POD_POD_SOLUTION_H