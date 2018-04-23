#ifndef POD_SINGLE_SOLUTION_H
#define POD_SINGLE_SOLUTION_H

#include"GnssSolution.h"
#include"CodeSmoother2.h"

#include"CodeSmoother.hpp"
#include"ComputeIonoModel.hpp"
#include"ComputeLinear.hpp"
#include"KalmanSolverFB.h"
#include"ProcessLinear.h"

namespace pod
{
    class SingleSolution : public GnssSolution
    {

    public:

        SingleSolution(GnssDataStore_sptr data_ptr);
        virtual ~SingleSolution() {};

    public: virtual std::string  fileName() const
    {
        return data->SiteRover + "_auto.txt";
    }

    public: virtual SlnType desiredSlnType() const override 
    { return SlnType::Standalone; }

    public: virtual void process() override;

    protected: virtual void updateRequaredObs() override;

    protected: virtual void configureSolver();

    protected: void updateNomPos(KalmanSolver &solver);

               //code smoother
    protected: CodeSmoother2 codeSmoother;

               // Maximum size of filter window, in seconds.
    protected: int codeSmWindowSize;

    };
}
#endif //!POD_SINGLE_SOLUTION_H


