#ifndef POD_GNSS_SOLUTION_H
#define POD_GNSS_SOLUTION_H
#include"SolverLMS.hpp"
#include"RequireObservables.hpp"

#include"GnssDataStore.hpp"
#include"GnssEpochMap.h"
#include"EquationComposer.h"
#include"ComputeLinear.hpp"
#include"ProcessLinear.h"
#include"SQLiteAdapter.h"

namespace pod
{
    //base class for all GNSS post processing  classes
    class GnssSolution
    {
    public: static  std::ostream& printMsg(const gpstk::CommonTime& time, const char* msg);

#pragma region Constructors

    public: GnssSolution(GnssDataStore_sptr dataStore, double maxsigma );
    public: virtual ~GnssSolution();

#pragma endregion


#pragma region Methods

    public: virtual void process() = 0;

    public: virtual GnssEpochMap& getData()
    {
        return gMap;
    };

    public: virtual std::string  fileName() const = 0;

    public: virtual SlnType desiredSlnType() const = 0;

    public: virtual double getMaxSigma() const
            { return maxSigma; }
    
    public: virtual GnssSolution& setMaxSigma(double sigma) 
            { maxSigma = sigma; return (*this); }

    public: virtual GnssSolution& setConfigData(GnssDataStore_sptr dataStore)
    {
        data = dataStore; return (*this);
    };

    protected: virtual  gpstk::ConfDataReader& confReader()
    {
        return *(data->confReader);
    }

    protected: virtual GnssDataStore::ProcessOpts & opts()
    {
        return data->opts;
    }

    protected: virtual void updateRequaredObs() = 0;

    protected: virtual int computeApprPos(
                           const gpstk::gnssRinex & gRin,
                           const gpstk::XvtStore<gpstk::SatID>& Eph,
                           gpstk::Position& pos);

    protected: virtual void printSolution(std::ofstream& os,
                            const gpstk::SolverLMS& solver,
                            const gpstk::CommonTime& time,
                            GnssEpoch& gEpoch);

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

               // object to compute prefit residuals 
    protected: gpstk::ComputeLinear oMinusC;

               //equation System composer
    protected: eqComposer_sptr Equations;

               //number of forward-backward cycles
    protected: int forwardBackwardCycles;

               //object to compute linear combinations
    protected: ProcessLinear computeLinear;
               
               //max sigma 
    protected: double maxSigma;

#pragma endregion

    };
    typedef std::unique_ptr<GnssSolution> GnssSolution_uptr;
}
#endif // !POD_GNSS_SOLUTION_H
