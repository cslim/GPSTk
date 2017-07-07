#ifndef POD_PR_SOLVER_LEO_H
#define POD_PR_SOLVER_LEO_H

#include"PRSolverBase.h"
#include"IonoModelStore.hpp"
using namespace gpstk;

namespace POD
{

    class PRSolverLEO : public PRSolverBase
    {
    public:
        PRSolverLEO() : PRSolverBase()
        {};
        virtual ~PRSolverLEO()
        {};
        string virtual getName() override
        {
            return "PRSolverLEO";
        };

        virtual int  solve(
            const CommonTime &t,
            const Matrix<double> &SVP,
            vector<bool> &useSat,
            Matrix<double>& Cov,
            Vector<double>& Resid,
            IonoModelStore &iono
        ) override;

    protected:

        virtual int catchSatByResid(
            const CommonTime &t,
            const Matrix<double> &SVP,
            vector<bool> &useSat,
            IonoModelStore &iono) override;

    };
}
#endif // !POD_PR_SOLVER_LEO_H