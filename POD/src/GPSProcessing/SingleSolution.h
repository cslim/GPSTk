#ifndef POD_SINGLE_SOLUTION_H
#define POD_SINGLE_SOLUTION_H

#include "GnssSolution.h"
#include"CodeSmoother.hpp"
#include"ComputeIonoModel.hpp"
#include"ComputeLinear.hpp"
#include"CodeKalmanSolver.hpp"
#include"CodeSolverFB.h"
#include"ProcessLinear.h"

namespace pod
{
    class SingleSolution : public GnssSolution
    {

    public:
        static const double MAX_SIGMA;

        SingleSolution(GnssDataStore_sptr data_ptr);
        virtual ~SingleSolution();

    public: void process() override;

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
    
    
    protected: void updateNomPos(CodeKalmanSolver &solver);

               //cycle slip marker
    protected: std::unique_ptr<gpstk::ProcessingClass> csMarker;

               //code smoothers
    protected: std::list<gpstk::CodeSmoother> codeSmoothers;

               //object to compute linear combinations
    protected: ProcessLinear computeLinear;

               // Maximum size of filter window, in samples.
    protected: int codeSmWindowSize;

               // object to compute prefit residuals 
    protected: gpstk::ComputeLinear linear;

               // object to compute code based solution via Kalman filtering
    protected: gpstk::CodeKalmanSolver solver;

               // object to compute code based solution via Kalman filtering in forward-backward manner 
    protected: CodeSolverFB solverFB;

    protected: int forwardBackwardCycles;
    };
}
#endif //!POD_SINGLE_SOLUTION_H


