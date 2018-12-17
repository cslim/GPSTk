#ifndef POD_PPP_SOLUTION_BASE_H
#define POD_PPP_SOLUTION_BASE_H

#include<memory>

#include"SolverPPP.hpp"
#include"CodeSolverLEO.h"
#include"ComputeDOP.hpp"
#include"SP3EphemerisStore.hpp"
#include"ConfDataReader.hpp"
#include"GnssEpochMap.h"
#include"EOPStore.hpp"
#include"CorrectCodeBiases.hpp"
#include"GnssDataStore.hpp"
#include"GnssSolution.h"


namespace pod
{
    class PPPSolutionBase : public GnssSolution
    {

    public: PPPSolutionBase(GnssDataStore_sptr data);

    public: virtual ~PPPSolutionBase();

#pragma region Methods
    public: virtual SlnType desiredSlnType() const override
    {
        return SlnType::PPP_Float;
    }
    public: virtual std::string  fileName() const override
    {
        return opts().SiteRover + "_ppp_float.txt";
    }

    protected: virtual void PRProcess();

    protected: virtual bool processCore() = 0;

    protected: virtual void mapSNR(gpstk::IRinex& value);

    protected: virtual double mapSNR(double value) { return value; };

    protected: virtual void updateNomPos(const gpstk::CommonTime& time, gpstk::Position& nominalPos) = 0;

#pragma endregion

#pragma region Fields

               //pointer to object for code solution 
    protected: unique_ptr<CodeSolverBase> solverPR;

#pragma endregion

    };
}
#endif // !POD_PPP_SOLUTION_BASE_H