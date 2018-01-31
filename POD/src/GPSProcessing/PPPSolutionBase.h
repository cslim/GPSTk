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

using namespace gpstk;
typedef   std::shared_ptr< pod::GnssDataStore> GnssDataStore_sptr;
namespace pod
{
    class PPPSolutionBase 
    {
    public:

        static PPPSolutionBase* Factory(GnssDataStore_sptr data);

        PPPSolutionBase(GnssDataStore_sptr data);

        virtual ~PPPSolutionBase();

        void process();

        GnssEpochMap& getData()
        {
            return gMap;
        };
        
        ConfDataReader& confReader()
        {
            return *(data->confReader);
        }

        GnssDataStore::ProcessOpt & opts()
        {
            return (data->opts);
        }

    protected:
        void PRProcess();
        
        virtual bool processCore() = 0;

        void mapSNR(gnssRinex& value);

        virtual double mapSNR(double value) { return value; };

        void updateNomPos(const CommonTime& time, Position& nominalPos);

      
        void printSolution(
            ofstream& outfile,
            const SolverPPP& solver,
            const CommonTime& time,
            const ComputeDOP& cDOP,
            GnssEpoch&   gEpoch,
            double dryTropo,
            int   precision,
            const Position& nomXYZ
        );


#pragma region Fields
        
        //Input processing data and configuration
        GnssDataStore_sptr data;

        //nominal position
        Position nominalPos;

        //pointer to object for code solution 
        unique_ptr<CodeSolverBase> solverPR;
        
        // processing result
        GnssEpochMap gMap;
#pragma endregion

    };
}
#endif // !POD_PPP_SOLUTION_BASE_H