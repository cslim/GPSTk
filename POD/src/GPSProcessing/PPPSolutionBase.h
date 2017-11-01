#ifndef POD_PPP_SOLUTION_BASE_H
#define POD_PPP_SOLUTION_BASE_H


#include"SolverPPP.hpp"
#include"CodeSolverLEO.h"
#include "ComputeDOP.hpp"
#include"SP3EphemerisStore.hpp"
#include"ConfDataReader.hpp"
#include"GnssEpochMap.h"

using namespace gpstk;
namespace pod
{
    class PPPSolutionBase 
    {

    public:
        static PPPSolutionBase * Factory(bool isSpaceborne, ConfDataReader & confReader, const string& dir);
        static void printModel(ofstream& modelfile, const gnssRinex& gData, int   precision);

        PPPSolutionBase(ConfDataReader & confReader,string workingDir);

        virtual ~PPPSolutionBase();

        bool LoadData();
        bool loadEphemeris();
        bool loadIono();
        bool loadFcn();
        bool loadClocks();
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


        bool loadApprPos(std::string path);
      
        void printSolution(
            ofstream& outfile,
            const SolverPPP& solver,
            const CommonTime& time0,
            const CommonTime& time,
            const ComputeDOP& cDOP,
            GnssEpoch &   gEpoch,
            double dryTropo,
            vector<PowerSum> &stats,
            int   precision,
            const Position &nomXYZ
        );

        void printStats(ofstream& outfile, const vector<PowerSum> &stats);
        
        string genFilesDir;
        string workingDir;
        string bceDir;

        bool calcApprPos = true;
        string apprPosFile;


        uchar maskSNR;
        double maskEl;

        //
        int DoY = 0;
        //
        Position nominalPos;

        // Configuration file reader
        ConfDataReader* confReader;

        // object to handle precise ephemeris and clocks
        SP3EphemerisStore SP3EphList;
        //
        CodeSolverBase *solverPR;
        IonoModelStore ionoStore;
        list<string> rinexObsFiles;
        map<CommonTime, Xvt, std::less<CommonTime>> apprPos;
        SatSystSet systems;
        GnssEpochMap gMap;

    };
}
#endif // !POD_PPP_SOLUTION_BASE_H