#include "KalmanSolverFB.h"
#include"PowerSum.hpp"

using namespace gpstk;
using namespace std;

namespace pod
{
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
        for (int i = 0; i<(cycles - 1); i++)
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

        for (auto limit : codeLimits)
        {
            for (auto &it : ObsData)
            {
                checkLimits(it, *solver.eqComposer().residTypes().begin(), limit);

                solver.Process(it);
            }

            for (auto rpos = ObsData.rbegin(); rpos != ObsData.rend(); ++rpos)
                solver.Process((*rpos));
        }
    }

    void KalmanSolverFB::checkLimits(gnssRinex& gData, const TypeID& type, double limit)
    {
        // Set to store rejected satellites
        SatIDSet satRejectedSet;

        // Let's check limits
        for (const auto& it :gData.body)
        {
            // Check postfit values and mark satellites as rejected
            if (std::abs(it.second.at(type)) > limit)
                satRejectedSet.insert(it.first);
        }

           // Update the number of rejected measurements
        rejectedMeasurements += satRejectedSet.size();

        // Remove satellites with missing data
        gData.removeSatID(satRejectedSet);

    }

   


}
