#ifndef POD_PR_SOLVER_H
#define POD_PR_SOLVER_H

#include"TropModel.hpp"
#include"CodeSolverBase.h"
#include"IonoModelStore.hpp"
#include"CodeProcSvData.h"

namespace pod
{
    class CodeSolver : public CodeSolverBase
    {
    public:

        CodeSolver(gpstk::TropModel &tropo, GnssDataStore_sptr data) :CodeSolverBase(data), tropo(&tropo)
        {};
        virtual ~CodeSolver()
        {
        };
        gpstk::TropModel *tropo;

        std::string virtual getName() override
        {
            return "CodeSolver";
        };
        virtual double getTropoCorrection(
            const Position &rxPos,
            const Position &svPos,
            const CommonTime &t) const override;

        virtual NeillTropModel initTropoModel(const Position &nominalPos, int DoY) override;
    protected:



    };

}

#endif // !POD_PR_SOLVER_H