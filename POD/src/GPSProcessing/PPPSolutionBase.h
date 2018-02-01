#ifndef POD_PPP_SOLUTION_BASE_H
#define POD_PPP_SOLUTION_BASE_H

#include<memory>

#include"SolverPPP.hpp"
#include"CodeSolverLEO.h"
#include "ComputeDOP.hpp"
#include"SP3EphemerisStore.hpp"
#include"ConfDataReader.hpp"
#include"GnssEpochMap.h"
#include"EOPStore.hpp"
#include"CorrectCodeBiases.hpp"
#include"GnssDataStore.hpp"
#include"GnssSolution.h"

using namespace gpstk;

namespace pod
{
    class PPPSolutionBase: public GnssSolution
    {
    public: static PPPSolutionBase* Factory(GnssDataStore_sptr data);

    public: PPPSolutionBase(GnssDataStore_sptr data);

    public: virtual ~PPPSolutionBase();

#pragma region Methods

    public: void process() override;

    protected: void PRProcess();

    protected: virtual bool processCore() = 0;

    protected: void mapSNR(gnssRinex& value);

    protected: virtual double mapSNR(double value) { return value; };

    protected: void updateNomPos(const CommonTime& time, Position& nominalPos);

    protected: virtual void printSolution(
        ofstream& outfile,
        const SolverLMS& solver,
        const CommonTime& time,
        GnssEpoch&   gEpoch,
        double dryTropo,
        int   precision,
        const Position& nomXYZ
    ) override;

#pragma endregion

#pragma region Fields

        //pointer to object for code solution 
    protected: unique_ptr<CodeSolverBase> solverPR;

#pragma endregion

    };
}
#endif // !POD_PPP_SOLUTION_BASE_H