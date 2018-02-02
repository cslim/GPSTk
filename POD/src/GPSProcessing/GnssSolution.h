#ifndef POD_GNSS_SOLUTION_H
#define POD_GNSS_SOLUTION_H
#include"SolverLMS.hpp"
#include"RequireObservables.hpp"

#include"GnssDataStore.hpp"
#include"GnssEpochMap.h"
#include"SQLiteAdapter.h"

namespace pod
{
    //base class for all GNSS post processing  classes
    class GnssSolution
    {
   

#pragma region Constructors

    public: GnssSolution(GnssDataStore_sptr dataStore);
    public: virtual ~GnssSolution();

#pragma endregion


#pragma region Methods

    public: virtual void process() = 0;

    public: virtual GnssEpochMap& getData()
    {
        return gMap;
    };

    public: virtual std::string&  fileName()
    {
        return fName;
    };
 
    public: virtual GnssSolution& setConfigData(GnssDataStore_sptr dataStore)
    {
        data = dataStore;
        return (*this);
    };

    protected: virtual  gpstk::ConfDataReader& confReader()
    {
        return *(data->confReader);
    }

    protected: virtual GnssDataStore::ProcessOpts & opts()
    {
        return data->opts;
    }
              //Print current solution  to a file end fill the parameter "gEpoch" 
    protected: virtual void printSolution(std::ofstream& of, const gpstk::SolverLMS& solver, const gpstk::CommonTime& time, GnssEpoch& gEpoch) =0;

    protected: virtual void updateRequaredObs() = 0;
#pragma endregion


#pragma region Fields

              // Input processing data and configuration
    protected:GnssDataStore_sptr data;

              // Nominal position
    protected:gpstk::Position nominalPos;

              // Processing result
    protected:GnssEpochMap gMap;
              
              // This object will filter out satellites, which doesn't meet  predefined required observables set 
    protected:gpstk::RequireObservables requireObs;

              // L1 code measurements used for position computation (typical C1 and P1)
    protected:gpstk::TypeID codeL1;

              //number of decimal places for output
    protected: int outputPrec = 3;

              // output file name
    protected: std::string fName;

              //adapter for SQLite3 database
    //protected: SQLiteAdapter db;

#pragma endregion

    };
    typedef std::unique_ptr<GnssSolution> GnssSolution_uptr;
}
#endif // !POD_GNSS_SOLUTION_H
