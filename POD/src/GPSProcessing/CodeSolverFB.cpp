#include "CodeSolverFB.h"
#include"PowerSum.hpp"

using namespace gpstk;
using namespace std;

namespace pod
{
    CodeSolverFB::CodeSolverFB():firstIteration(true), processedMeasurements(0), rejectedMeasurements(0)
    {
    }
    
    CodeSolverFB::~CodeSolverFB()
    {
    }

    gpstk::gnssRinex & CodeSolverFB::Process(gpstk::gnssRinex & gRin)
    {
        CodeKalmanSolver::Process(gRin);


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


    bool CodeSolverFB::lastProcess(gpstk::gnssRinex & gRin)
    {

        // Keep processing while 'ObsData' is not empty
        if (!(ObsData.empty()))
        {

            // Get the first data epoch in 'ObsData' and process it. The
            // result will be stored in 'gData'
            gRin = Process(ObsData.front());
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

    void CodeSolverFB::reProcess(int cycles)
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
            CodeKalmanSolver::Process((*rpos));
        }

        // If 'cycles > 1', let's do the other iterations
        for (int i = 0; i<(cycles - 1); i++)
        {

            // Forwards iteration
            for (auto pos = ObsData.begin(); pos != ObsData.end(); ++pos)
            {
                CodeKalmanSolver::Process((*pos));
            }

            // Backwards iteration.
            for (auto rpos = ObsData.rbegin(); rpos != ObsData.rend(); ++rpos)
            {
                CodeKalmanSolver::Process((*rpos));
            }

        }  // End of 'for (int i=0; i<(cycles-1), i++)'
        return;
    }
    void CodeSolverFB::reProcess()
    {
        firstIteration = false;
        // Backwards iteration. We must do this at least once
        for (auto rpos = ObsData.rbegin(); rpos != ObsData.rend(); ++rpos)
            CodeKalmanSolver::Process((*rpos));

        for (auto limit : codeLimits)
        {

            for (auto &it : ObsData)
            {
                checkLimits(it, limit);
                //updateWeights(it);
                CodeKalmanSolver::Process(it);
            }

            for (auto rpos = ObsData.rbegin(); rpos != ObsData.rend(); ++rpos)
                CodeKalmanSolver::Process((*rpos));
        }
    }

    void CodeSolverFB::checkLimits(gnssRinex& gData, double codeLimit)
    {
        // Set to store rejected satellites
        SatIDSet satRejectedSet;

        // Let's check limits
        for (auto it = gData.body.begin(); it != gData.body.end(); ++it)
        {
            // Check postfit values and mark satellites as rejected
            if (std::abs((*it).second(TypeID::postfitC)) > codeLimit)
                satRejectedSet.insert((*it).first);
        } 

           // Update the number of rejected measurements
        rejectedMeasurements += satRejectedSet.size();

        // Remove satellites with missing data
        gData.removeSatID(satRejectedSet);

    }

    void CodeSolverFB::updateWeights(gnssRinex& gData)
    {
        PowerSum ps;
        for (auto it : gData.body)
            ps.add(it.second[TypeID::postfitC]);

        double sigma = sqrt(ps.variance());

        for (auto &it : gData.body)
        {
            double res = it.second[TypeID::postfitC];

            if (::abs(res) > sigma)
                it.second[TypeID::weight] = sigma*sigma / res / res;
            else
                it.second[TypeID::weight] = 1;
        }
    }
}
