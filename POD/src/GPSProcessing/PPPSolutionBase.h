#ifndef POD_PPP_SOLUTION_BASE_H
#define POD_PPP_SOLUTION_BASE_H


#include"SolverWMS.hpp"
#include"PRSolverLEO.h"
#include "ComputeDOP.hpp"
#include"SP3EphemerisStore.hpp"
#include"ConfDataReader.hpp"

using namespace gpstk;
namespace POD
{
    class PPPSolutionBase 
    {

    public:
        static PPPSolutionBase * Factory(bool isSpaceborne, ConfDataReader & confReader);

        PPPSolutionBase(ConfDataReader & confReader);

        virtual ~PPPSolutionBase();

        bool LoadData();
        bool loadEphemeris();
        bool loadIono();
        bool loadClocks();

        void checkObservable(const string & path);

        void process();

    protected:
        virtual void PRProcess()= 0;
        virtual bool PPPprocess() = 0;

        bool loadApprPos(std::string path);

        void printModel(ofstream& modelfile,
            const gnssRinex& gData,
            int   precision);

        void printSolution(ofstream& outfile,
            const SolverLMS& solver,
            const CommonTime& time0,
            const CommonTime& time,
            const ComputeDOP& cDOP,
            bool  useNEU,
            int   numSats,
            double dryTropo,
            vector<PowerSum> &stats,
            int   precision,
            const Position &nomXYZ);

        void printStats(ofstream& outfile, const vector<PowerSum> &stats);

        string genFilesDir;

        uchar maskSNR;
        double maskEl;

        const char * L1CCodeID = "C1" ;
        const char * L1PCodeID = "C1W";
        const char * L2CodeID  = "C2W";
        const char * L1CNo     = "S1C";
        //
        int DoY = 0;
        //
        Position nominalPos;

        // Configuration file reader
        ConfDataReader* confReader;

        // object to handle precise ephemeris and clocks
        SP3EphemerisStore SP3EphList;
        //
        PRSolverBase *solverPR;
        IonoModelStore ionoStore;
        list<string> rinexObsFiles;
        map<CommonTime, Xvt, std::less<CommonTime>> apprPos;
    };
}
#endif // !POD_PPP_SOLUTION_BASE_H