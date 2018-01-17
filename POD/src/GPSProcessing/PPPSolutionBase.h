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

using namespace gpstk;
namespace pod
{
    class PPPSolutionBase 
    {
    public:
        enum Dynamics
        {
            Static = 0,
            Kinematic,
        }dynamics;

        static PPPSolutionBase * Factory(bool isSpaceborne, ConfDataReader & confReader, const string& dir);
        //static void printModel(ofstream& modelfile, const gnssRinex& gData, int   precision);

        PPPSolutionBase(ConfDataReader & confReader,string workingDir);

        virtual ~PPPSolutionBase();

        void LoadData();
        bool loadEphemeris();
        bool loadIono();
        bool loadFcn();
        bool loadClocks();
        bool loadEOPData();
        bool loadCodeBiades();

        void checkObservable();

        void process();

        GnssEpochMap & getData()
        {
            return gMap;
        };

    protected:
        void PRProcess();
        virtual bool PPPprocess() = 0;

        void mapSNR(gnssRinex & value);
        virtual double mapSNR(double value) { return value; };

        void updateNomPos(const CommonTime& time, Position &nominalPos);
        bool loadApprPos(std::string path);
      
        void printSolution(
            ofstream& outfile,
            const SolverPPP& solver,
            const CommonTime& time,
            const ComputeDOP& cDOP,
            GnssEpoch &   gEpoch,
            double dryTropo,
            int   precision,
            const Position &nomXYZ
        );

        string genFilesDir;
        string workingDir;
        string bceDir;

        bool useC1;
        uchar maskSNR;
        double maskEl;

        //
        int DoY = 0;

        //
        bool calcApprPos = true;
        string apprPosFile;
        Position nominalPos;
        map<CommonTime, Xvt, std::less<CommonTime>> apprPos;
        //
        
        // Configuration file reader
        ConfDataReader* confReader;

        // object to handle precise ephemeris and clocks
        SP3EphemerisStore SP3EphList;

        //Earth orintation parameters store
        EOPStore eopStore;
        
        CorrectCodeBiases DCBData;
        //pointer to object for code solution 
        unique_ptr<CodeSolverBase> solverPR;

        IonoModelStore ionoStore;
        list<string> rinexObsFiles;

        SatSystSet systems;       
        GnssEpochMap gMap;

    };
}
#endif // !POD_PPP_SOLUTION_BASE_H