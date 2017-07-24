#ifndef POD_PR_SOLVER_H
#define POD_PR_SOLVER_H

#include"TropModel.hpp"
#include"PRSolverBase.h"
#include"IonoModelStore.hpp"

namespace pod
{
    class PRSolver : public PRSolverBase
    {
    public:

        PRSolver(gpstk::TropModel &tropo) :PRSolverBase(), tropo(&tropo)
        {};
        virtual ~PRSolver()
        {
        };
        gpstk::TropModel *tropo;

        std::string virtual getName() override
        {
            return "PRSolver";
        };

        virtual int  solve(
            const gpstk::CommonTime &t,
            const gpstk::Matrix<double> &SVP,
            std::vector<bool> &useSat,
            gpstk::Matrix<double>& Cov,
            gpstk::Vector<double>& Resid,
            gpstk::IonoModelStore &iono
        ) override;

    protected:

        virtual int catchSatByResid(
            const gpstk::CommonTime &t,
            const gpstk::Matrix<double> &SVP,
            std::vector<bool> &useSat,
            gpstk::IonoModelStore &iono) override;

    };

}

#endif // !POD_PR_SOLVER_H