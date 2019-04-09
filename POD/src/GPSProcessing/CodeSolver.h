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
            const gpstk::Position &rxPos,
            const gpstk::Position &svPos,
            const gpstk::CommonTime &t) const override;

        virtual gpstk::NeillTropModel initTropoModel(const gpstk::Position &nominalPos, int DoY) override;
    protected:



    };

}

#endif // !POD_PR_SOLVER_H