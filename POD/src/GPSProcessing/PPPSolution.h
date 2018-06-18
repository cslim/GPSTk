#ifndef POD_PPP_SOLUTION_H
#define POD_PPP_SOLUTION_H

#include"ConfDataReader.hpp"
#include "PPPSolutionBase.h"
#include"NeillTropModel.hpp"

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
        gpstk::NeillTropModel tropModel;
        virtual void updateRequaredObs() override;

    protected:  void printSolution(ofstream& of, const gpstk::SolverLMS& solver, const gpstk::CommonTime& time, GnssEpoch& gEpoch) ;

    protected: virtual void updateNomPos(const gpstk::CommonTime & t, gpstk::Position & pos) override;



    };
}

#endif // !POD_PPP_SOLUTION_H