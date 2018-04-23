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
        virtual ~PPPSolution() {};


        void process() override;

    protected:
        virtual bool processCore() override;
        NeillTropModel tropModel;
        virtual void updateRequaredObs() override;

    protected: virtual void printSolution(ofstream& of, const SolverLMS& solver, const CommonTime& time, GnssEpoch& gEpoch) override;

    protected: virtual void updateNomPos(const CommonTime & t, Position & pos) override;



    };
}

#endif // !POD_PPP_SOLUTION_H