#ifndef POD_PPP_SOLUTION_BASE_H
#define POD_PPP_SOLUTION_BASE_H


#include"SolverWMS.hpp"
#include"PRSolverLEO.h"
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
        bool loadClocks();

        void checkObservable(const string & path);

        void process();

        GnssEpochMap & getData()
        {
            return gMap;
        };

    protected:
        virtual void PRProcess() = 0;
        virtual bool PPPprocess() = 0;

        bool loadApprPos(std::string path);
      

        void printSolution(ofstream& outfile,
            const SolverLMS& solver,
            const CommonTime& time0,
            const CommonTime& time,
            const ComputeDOP& cDOP,
            bool  useNEU,
            GnssEpoch &   gEpoch,
            double dryTropo,
            vector<PowerSum> &stats,
            int   precision,
            const Position &nomXYZ);

        void printStats(ofstream& outfile, const vector<PowerSum> &stats);
        
        string genFilesDir;
        string workingDir;

        bool calcApprPos = true;
        string apprPosFile;


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

       GnssEpochMap gMap;

    };
}
#endif // !POD_PPP_SOLUTION_BASE_H