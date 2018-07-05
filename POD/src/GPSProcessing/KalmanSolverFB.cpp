#include "KalmanSolverFB.h"
#include"PowerSum.hpp"

using namespace gpstk;
using namespace std;

namespace pod
{
    //set of all possible TypeID for code pseudorange postfit residuals 
    const std::set<gpstk::TypeID> KalmanSolverFB::codeResTypes
    {
        TypeID::postfitC,
        TypeID::postfitP1,
        TypeID::postfitP2,
        TypeID::postfitPC,
    };

    //set of all possible TypeID for  carrier phase postfit residuals 
    const std::set<gpstk::TypeID> KalmanSolverFB::phaseResTypes
    {
        TypeID::postfitL,
        TypeID::postfitL1,
        TypeID::postfitL2,
        TypeID::postfitLC,
    };



    KalmanSolverFB::KalmanSolverFB()
        :firstIteration(true), processedMeasurements(0), rejectedMeasurements(0)
    {
    }

    KalmanSolverFB::KalmanSolverFB(eqComposer_sptr eqs)
        : firstIteration(true), processedMeasurements(0), rejectedMeasurements(0)
    {
        solver = KalmanSolver(eqs);
    }

    KalmanSolverFB::~KalmanSolverFB()
    {
    }

    gpstk::gnssRinex & KalmanSolverFB::Process(gpstk::gnssRinex & gRin)
    {
        solver.Process(gRin);


        // Before returning, store the results for a future iteration
        if (firstIteration)
        {
            // Create a new gnssRinex structure with just the data we need
            //gnssRinex gBak(gData.extractTypeID(keepTypeSet));

            // Store observation data
            ObsData.push_back(gRin);

            // Update the number of processed measurements
            processedMeasurements += gRin.numSats();

        }

        return gRin;
    }


    bool KalmanSolverFB::lastProcess(gpstk::gnssRinex & gRin)
    {

        // Keep processing while 'ObsData' is not empty
        if (!(ObsData.empty()))
        {

            // Get the first data epoch in 'ObsData' and process it. The
            // result will be stored in 'gData'
            gRin = solver.Process(ObsData.front());
            // gData = ObsData.front();
            // Remove the first data epoch in 'ObsData', freeing some
            // memory and preparing for next epoch
            ObsData.pop_front();
            return true;
        }
        else
        {
            return false;
        }
    }

    void KalmanSolverFB::reProcess(int cycles)
    {
        // Check number of cycles. The minimum allowed is "1".
        if (cycles < 1)
        {
            cycles = 1;
        }
        firstIteration = false;
        // Backwards iteration. We must do this at least once
        for (auto rpos = ObsData.rbegin(); rpos != ObsData.rend(); ++rpos)
        {
            solver.Process((*rpos));
        }

        // If 'cycles > 1', let's do the other iterations
        for (int i = 0; i < (cycles - 1); i++)
        {

            // Forwards iteration
            for (auto pos = ObsData.begin(); pos != ObsData.end(); ++pos)
            {
                solver.Process((*pos));
            }

            // Backwards iteration.
            for (auto rpos = ObsData.rbegin(); rpos != ObsData.rend(); ++rpos)
            {
                solver.Process((*rpos));
            }

        }  // End of 'for (int i=0; i<(cycles-1), i++)'
        return;
    }
    void KalmanSolverFB::reProcess()
    {
        firstIteration = false;
        // Backwards iteration. We must do this at least once
        for (auto rpos = ObsData.rbegin(); rpos != ObsData.rend(); ++rpos)
            solver.Process((*rpos));

        for (size_t cycle = 0; cycle < cyclesNumber; cycle++)
        {
            for (auto &it : ObsData)
            {
                checkLimits(it, cycle);
                solver.Process(it);
            }

            for (auto rpos = ObsData.rbegin(); rpos != ObsData.rend(); ++rpos)
                solver.Process((*rpos));
        }
    }
    KalmanSolverFB& KalmanSolverFB::setLimits(const std::list<double>& codeLims, const std::list<double>& phaseLims)
    {
        size_t i = 0;
        tresholds.codeLimits.resize(codeLims.size());
        for (auto val : codeLims)
            tresholds.codeLimits[i] = val;

        i = 0;
        tresholds.phaseLimits.resize(phaseLims.size());
        for (auto val : phaseLims)
            tresholds.phaseLimits[i] = val;

        return *this;
    }

    double KalmanSolverFB::getLimit(const gpstk::TypeID& type, int cycleNumber)
    {
        if (codeResTypes.find(type) != codeResTypes.end())
            if (cycleNumber < tresholds.codeLimits.size())
                return tresholds.codeLimits[cycleNumber];
        if (phaseResTypes.find(type) != phaseResTypes.end())
            if (cycleNumber < tresholds.phaseLimits.size())
                return tresholds.phaseLimits[cycleNumber];

        string msg = "Can't get observables treshold for type: '"
            + TypeID::tStrings[type.type] +
            "' with reprocess cycle number: '"
            + StringUtils::asString(cycleNumber) + "'.";

        InvalidRequest e(msg);
        GPSTK_THROW(e);

    }
    void KalmanSolverFB::checkLimits(gnssRinex& gData, int cycleNumber)
    {
        // Set to store rejected satellites
        SatIDSet satRejectedSet;

        // Let's check limits
        for (const auto& type : solver.eqComposer().residTypes())
        {
            double limit = getLimit(type, cycleNumber);
            for (const auto& it : gData.body)
            {
                // Check postfit values and mark satellites as rejected
                if (std::abs(it.second.at(type)) > limit)
                    satRejectedSet.insert(it.first);

            }
        }
        // Update the number of rejected measurements
        rejectedMeasurements += satRejectedSet.size();

        // Remove satellites with missing data
        gData.removeSatID(satRejectedSet);

    }

}
