#ifndef POD_GNSS_SOLUTION_H
#define POD_GNSS_SOLUTION_H
#include"GnssDataStore.hpp"
#include"GnssEpochMap.h"
#include"SolverLMS.hpp"

namespace pod
{
    //base class for all GNSS post processing  classes
    class GnssSolution
    {
#pragma region Constructors

    public: GnssSolution(GnssDataStore_sptr data);
    public: virtual ~GnssSolution();

#pragma endregion


#pragma region Methods

    public: virtual void process()=0;

    public: GnssEpochMap& getData()
    {
        return gMap;
    };

    protected: gpstk::ConfDataReader& confReader()
    {
        return *(data->confReader);
    }

    protected:GnssDataStore::ProcessOpts & opts()
    {
        return data->opts;
    }
              //Print current solution  to a file end fill the parameter "gEpoch" 
    protected: virtual void printSolution(
        std::ofstream& outfile,
        const gpstk::SolverLMS& solver,
        const gpstk::CommonTime& time,
        GnssEpoch&   gEpoch,
        double dryTropo,
        int   precision,
        const gpstk::Position& nomXYZ
    ) = 0;

#pragma endregion


#pragma region Fields

              //Input processing data and configuration
    protected:GnssDataStore_sptr data;

              //nominal position
    protected:gpstk::Position nominalPos;

              // processing result
    protected:GnssEpochMap gMap;

#pragma endregion

    };
}
#endif // !POD_GNSS_SOLUTION_H
