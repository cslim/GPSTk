#ifndef POD_PR_SOLVER_H
#define POD_PR_SOLVER_H

#include"TropModel.hpp"
#include"PRSolverBase.h"
#include"IonoModelStore.hpp"

using namespace std;
using namespace gpstk;

namespace POD
{
    class PRSolver : public PRSolverBase
    {
    public:

        PRSolver(TropModel &tropo) :PRSolverBase(), tropo(&tropo)
        {};
        virtual ~PRSolver()
        {
        };
        TropModel *tropo;

        string virtual getName() override
        {
            return "PRSolver";
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



    private:

    };

}

#endif // !POD_PR_SOLVER_H