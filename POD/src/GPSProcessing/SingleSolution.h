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
        static const double MAX_SIGMA_SINGLE;

        SingleSolution(GnssDataStore_sptr data_ptr);
        virtual ~SingleSolution() {};

    public: virtual std::string  fileName() const
    {
        return data->SiteRover + "_auto.txt";
    }

    public: virtual void process() override;

    protected: virtual void updateRequaredObs() override;

    protected: virtual void printSolution(
        std::ofstream& of,
        const gpstk::SolverLMS& solver,
        const gpstk::CommonTime& time,
        GnssEpoch& gEpoch) override;

    protected: virtual int computeApprPos(
        const gpstk::gnssRinex & gRin,
        const gpstk::XvtStore<gpstk::SatID>& Eph,
        gpstk::Position& pos);

    protected: virtual void configureSolver();

    protected: void updateNomPos(KalmanSolver &solver);


               //code smoother
    protected: CodeSmoother2 codeSmoother;

               //object to compute linear combinations
    protected: ProcessLinear linearCombinations;

               // Maximum size of filter window, in seconds.
    protected: int codeSmWindowSize;

               // object to compute prefit residuals 
    protected: gpstk::ComputeLinear oMinusC;

    //           // object to compute code based solution via Kalman filtering
    //protected: std::shared_ptr<KalmanSolver> solver;

    protected: eqComposer_sptr Equations;

               //number of forward-backward cycles
    protected: int forwardBackwardCycles;

    };
}
#endif //!POD_SINGLE_SOLUTION_H


