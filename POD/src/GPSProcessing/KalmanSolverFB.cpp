#include "KalmanSolverFB.h"
#include"PowerSum.hpp"
#include"WinUtils.h"

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
        :currCycle(0), processedMeasurements(0), rejectedMeasurements(0)
    {
    }

    KalmanSolverFB::KalmanSolverFB(eqComposer_sptr eqs)
        : currCycle(0), processedMeasurements(0), rejectedMeasurements(0)
    {
        solver = KalmanSolver(eqs);
    }

    KalmanSolverFB::~KalmanSolverFB()
    {
    }

    gpstk::IRinex & KalmanSolverFB::Process(gpstk::IRinex & gRin)
    {
        solver.Process(gRin);


        // Before returning, store the results for a future iteration
		if (currCycle == 0)
		{
			// Create a new gnssRinex structure with just the data we need
			//gnssRinex gBak(gData.extractTypeID(keepTypeSet));

			// Store observation data
			addData(gRin);

			// Update the number of processed measurements
			processedMeasurements += gRin.getBody().numSats();
		}

        return gRin;
    }


	bool KalmanSolverFB::lastProcess(gpstk::IRinex & gRin)
	{
		if (currList->empty())
		{
			obsData.pop_front();
			if (obsData.empty())
				return false;

			currList = obsData.begin();
		}

		// Get the first data epoch in 'ObsData' and process it. 
		// The result will be stored in 'gData'
		gRin = ReProcessOneEpoch(*currList->front());
		// gData = ObsData.front();
		// Remove the first data epoch in 'ObsData', freeing some
		// memory and preparing for next epoch
		currList->pop_front();
		return true;
	}

	void KalmanSolverFB::reProcess()
	{
		for (auto && oList : obsData)
		{
			// Backwards iteration. We must do this at least once
			for (auto rpos = oList.rbegin(); rpos != oList.rend(); ++rpos)
				ReProcessOneEpoch(**rpos);

			for (currCycle = 0; currCycle < cyclesNumber - 1; ++currCycle)
			{
				for (auto &it : oList)
					ReProcessOneEpoch(*it);

				for (auto rpos = oList.rbegin(); rpos != oList.rend(); ++rpos)
					ReProcessOneEpoch(**rpos);
			}
		}
		currList = obsData.begin();
	}

	gpstk::IRinex & KalmanSolverFB::ReProcessOneEpoch(gpstk::IRinex & gRin)
	{
		usedSvMarker.keepOnlyUsed(gRin.getBody());
		usedSvMarker.CleanSatArcFlags(gRin.getBody());
		usedSvMarker.CleanScFlags(gRin.getBody());
		checkLimits(gRin, currCycle);

		gRin >> reProcList;
		return	solver.Process(gRin);
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

    double KalmanSolverFB::getLimit(const gpstk::TypeID& type, size_t cycleNumber)
    {
        if (codeResTypes.find(type) != codeResTypes.end() 
			&& cycleNumber < tresholds.codeLimits.size())
                return tresholds.codeLimits[cycleNumber];

        if (phaseResTypes.find(type) != phaseResTypes.end()
			&& cycleNumber < tresholds.phaseLimits.size())
                return tresholds.phaseLimits[cycleNumber];

        string msg = "Can't get observables treshold for type: '"
            + TypeID::tStrings[type.type] +
            "' with reprocess cycle number: '"
            + StringUtils::asString(cycleNumber) + "'.";

        InvalidRequest e(msg);
        GPSTK_THROW(e);

    }

    void KalmanSolverFB::checkLimits(IRinex& gData, size_t cycleNumber)
    {
        // Set to store rejected satellites
        SatIDSet satRejectedSet;

        // Let's check limits
        for (auto&& type : solver.eqComposer().residTypes())
        {
            double limit = getLimit(type, cycleNumber);
			for (auto&& it : gData.getBody())
			{
				// Check postfit values and mark satellites as rejected
				auto itRes = it.second->get_value().find(type);
				if (itRes != it.second->get_value().end() && std::abs(itRes->second) > limit)
				{
					it.second->get_value().erase(type);
					satRejectedSet.insert(it.first);
				}
			}
        }

        // Update the number of rejected measurements
        rejectedMeasurements += satRejectedSet.size();

        // Remove satellites with missing data
        gData.getBody().removeSatID(satRejectedSet);
		DBOUT_LINE("rej. meas" << rejectedMeasurements)
    }

	void KalmanSolverFB::updateCurrList()
	{
		obsData.push_back(std::list<gpstk::irinex_uptr>());
		currList = --obsData.end();
	}

	void KalmanSolverFB::addData(gpstk::IRinex & gRin)
	{
		if (solver.isReset(gRin.getHeader().epoch))
			updateCurrList();
		
		currList->push_back(gRin.clone());
	}
}
