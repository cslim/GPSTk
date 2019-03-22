#include "PppFloatSolution.h"

#include"SimpleFilter.hpp"
#include"LinearCombinations.hpp"
#include"LICSDetector.hpp"
#include"MWCSDetector.hpp"
#include"Decimate.hpp"
#include"BasicModel.hpp"
#include"ComputeWeightSimple.h"
#include"NeillTropModel.hpp"
#include"PowerSum.hpp"
#include"ComputeTropModel.hpp"
#include"LinearCombinations.hpp"
#include"EclipsedSatFilter.hpp"
#include"GravitationalDelay.hpp"
#include"LICSDetector2.hpp"
#include"MWCSDetector.hpp"
#include"SatArcMarker.hpp"
#include"AntexReader.hpp"
#include"ComputeSatPCenter.hpp"
#include"CorrectObservables.hpp"
#include"ComputeWindUp.hpp"
#include"SolidTides.hpp"
#include"OceanLoading.hpp"
#include"PoleTides.hpp"
#include"ElevationMask.hpp"

#include"MJD.hpp"
#include"IonexModel.hpp"

#include"TropoEquations.h"
#include"TropoGradEquations.h"
#include"TropoEquationsAdv.hpp"
#include"ClockBiasEquations.h"
#include"AdvClockModel.h"
#include"PositionEquations.h"
#include"InterSystemBias.h"
#include"InterFrequencyBiases.h"
#include"AmbiguitiesEquations.h"
#include"SNRCatcher.h"
#include"IonoEquations.h"
#include"IonoStochasticModel.h"
#include"KalmanSolverFB.h"
#include"PrefitResCatcher.h"
#include"UsedInPvtMarker.hpp"

#include"WinUtils.h"

using namespace gpstk;

namespace pod
{
    PppFloatSolution::PppFloatSolution(GnssDataStore_sptr data_ptr)
        :GnssSolution(data_ptr, 50.0)
    { }

    PppFloatSolution::PppFloatSolution(GnssDataStore_sptr data_ptr, double max_sigma)
        : GnssSolution(data_ptr, max_sigma)
    { }

    void  PppFloatSolution::process()
    {
        updateRequaredObs();

        BasicModel model;
        model.setDefaultEphemeris(data->SP3EphList);
        model.setDefaultObservable(codeL1);
        model.setMinElev(.0);
		
		ElevationMask elMask(opts().maskEl);

        SimpleFilter CodeFilter(TypeIDSet{ codeL1,TypeID::P2,TypeID::L1,TypeID::L2 });
        SimpleFilter SNRFilter(TypeID::S1, confReader().getValueAsInt("SNRmask"), DBL_MAX);
        // Object to remove eclipsed satellites
        EclipsedSatFilter eclipsedSV;

        RinexEpoch gRin;

        //Object to decimate data
        Decimate decimateData(confReader().getValueAsDouble("decimationInterval"),
            confReader().getValueAsDouble("decimationTolerance"),
            data->SP3EphList.getInitialTime());

		// Object to compute gravitational delay effects
		GravitationalDelay grDelayRover;

#pragma region troposhere modeling objects

        //for rover
        NeillTropModel tropoRovPtr;
		ComputeTropModel computeTropoRover(tropoRovPtr, true);

#pragma endregion

#pragma region CS detectors

        // Objects to mark cycle slips
        // Checks LI cycle slips
        LICSDetector2 markCSLI2Rover;
		markCSLI2Rover.setSatThreshold(confReader().getValueAsDouble("LISatThreshold"));
		
        // Checks Merbourne-Wubbena cycle slips
        MWCSDetector markCSMW2Rover;
		markCSMW2Rover.setMaxNumLambdas(confReader().getValueAsDouble("MWNLambdas"));
		
		// check sharp SNR drops 
        SNRCatcher snrCatcherL1Rover;
        PrefitResCatcher resCatcher(Equations->measTypes());

        // Object to keep track of satellite arcs
        SatArcMarker markArcRover(TypeID::CSL1, true, 31.0);

#pragma endregion

#pragma region prepare ANTEX reader

        std::string antxfile = opts().genericFilesDirectory;
        antxfile += confReader().getValue("antexFile");

        AntexReader antexReader;
        antexReader.open(antxfile);

#pragma endregion

#pragma region correct observable

        CorrectObservables corrRover(data->SP3EphList);

        // Vector from monument to antenna ARP [UEN], in meters
        Triple offsetARP;
        int i = 0;
        for (auto &it : confReader().getListValueAsDouble("offsetARP", opts().SiteRover))
            offsetARP[i++] = it;
        corrRover.setMonument(offsetARP);

        Antenna roverAnt(antexReader.getAntenna(confReader().getValue("antennaModel", opts().SiteRover)));
		corrRover.setUsePcv(confReader().getValueAsBoolean("usePCPatterns", opts().SiteRover));
        corrRover.setAntenna(roverAnt);
        corrRover.setUseAzimuth(confReader().getValueAsBoolean("useAzim", opts().SiteRover));

#pragma endregion

		ComputeWeightSimple computeWeightSimple(2);
        // Objects to compute tidal effects
        SolidTides solid;
        PoleTides pole;
        // Configure ocean loading model
        OceanLoading ocean;
        ocean.setFilename(opts().genericFilesDirectory + confReader().getValue("oceanLoadingFile"));

        ComputeWindUp windupRover(data->SP3EphList, opts().genericFilesDirectory + confReader().getValue("satDataFile"));

        ComputeSatPCenter svPcenterRover;
        svPcenterRover.setAntexReader(antexReader);

        ProcessLinear linearIonoFree;
        linearIonoFree.add(std::make_unique<PCCombimnation>());
        linearIonoFree.add(std::make_unique<LCCombimnation>());

		UsedInPvtMarker useMarker;
        KalmanSolver solver(Equations);
        KalmanSolverFB solverFb(Equations);

        if (forwardBackwardCycles > 0)
        {
            solverFb.setCyclesNumber(forwardBackwardCycles);
            solverFb.setLimits(confReader().getListValueAsDouble("codeLimList"), confReader().getListValueAsDouble("phaseLimList"));
			solverFb.ReProcList().push_back(markCSLI2Rover);
			solverFb.ReProcList().push_back(markCSMW2Rover);
			solverFb.ReProcList().push_back(markArcRover);
			solverFb.ReProcList().push_back(elMask);
        }

        bool firstTime = true;
        //
        for (auto &obsFile : data->getObsFiles(opts().SiteRover))
        {
            std::cout << obsFile << std::endl;
            //Input observation file stream
            Rinex3ObsStream rin;

            //Open Rinex observations file in read-only mode
            rin.open(obsFile, std::ios::in);

            rin.exceptions(std::ios::failbit);
            Rinex3ObsHeader roh;

            //read the header
            rin >> roh;
            gMap.header = roh;

            //read all epochs
            while (rin >> gRin)
            {
				//gRin.removeSatID(18, SatID::SatelliteSystem::systemGPS);
				if (decimateData.check(gRin))
					continue;

				if (gRin.getBody().size() == 0)
				{
					printMsg(gRin.getHeader().epoch, "Empty epoch record in Rinex file");
					continue;
				}

                const auto& t = gRin.getHeader().epoch;
#if _DEBUG
				bool b;

				CATCH_TIME(t, 2014, 12, 19, 0, 14, 15, b)
					if (b)
						DBOUT_LINE("catched")
#endif
                //keep only satellites from satellites systems selecyted for processing
                gRin.keepOnlySatSystems(opts().systems);

                //keep only types used for processing
                // gRin.keepOnlyTypeID(requireObs.getRequiredType());

                //get approximate position
				if (apprPos().getPosition(gRin, nominalPos))
					continue;
				//std::cout << nominalPos << std::endl;
                grDelayRover.setNominalPosition(nominalPos);

                tropoRovPtr.setAllParameters(t, nominalPos);

                corrRover.setNominalPosition(nominalPos);
                windupRover.setNominalPosition(nominalPos);
                svPcenterRover.setNominalPosition(nominalPos);
                model.rxPos = nominalPos;

                gRin >> requireObs;
                gRin >> CodeFilter;
                gRin >> SNRFilter;

                if (gRin.getBody().size() == 0)
                {
                    printMsg(gRin.getHeader().epoch, "Rover receiver: all SV has been rejected.");
                    continue;
                }
                gRin >> computeLinear;

                auto eop = data->eopStore.getEOP(MJD(t).mjd, IERSConvention::IERS2010);
                pole.setXY(eop.xp, eop.yp);

                gRin >> model;
                gRin >> eclipsedSV;
                gRin >> grDelayRover;
                gRin >> svPcenterRover;

                Triple tides(solid.getSolidTide(t, nominalPos) + ocean.getOceanLoading(opts().SiteRover, t) + pole.getPoleTide(t, nominalPos));
                corrRover.setExtraBiases(tides);
                gRin >> corrRover;

                gRin >> windupRover;
                gRin >> computeTropoRover;

                gRin >> linearIonoFree;
                gRin >> oMinusC;
                gRin >> resCatcher;
				gRin >> computeWeightSimple;

				gRin >> useMarker;
				gRin >> markCSLI2Rover;
				gRin >> markCSMW2Rover;
				gRin >> markArcRover;
				gRin >> elMask;
				//gRin >> snrCatcherL1Rover;


                //DBOUT_LINE(">>" << CivilTime(gRin.getHeader().epoch).asString());

                if (forwardBackwardCycles > 0)
                {
					solverFb.setMinSatNumber(4 /*+ gRin.getBody().getSatSystems().size()*/);
                    gRin >> solverFb;
                }
                else
                {
					solver.setMinSatNumber(4 /*+ gRin.getBody().getSatSystems().size()*/);
                    gRin >> solver;
                    auto ep = opts().fullOutput ? GnssEpoch(gRin.getBody()) : GnssEpoch();
                    // updateNomPos(solverFB);
                    printSolution( solver, t, ep);
                    gMap.data.insert(std::make_pair(t, ep));
                }
            }
        }
        if (forwardBackwardCycles > 0)
        {
			markCSLI2Rover.setIsReprocess(true);
			markCSMW2Rover.setIsReprocess(true);

            std::cout << "Fw-Bw part started" << std::endl;
            solverFb.reProcess();
            RinexEpoch gRin;
            std::cout << "Last process part started" << std::endl;

            while (solverFb.lastProcess(gRin))
            {
				//fill GnssEpoch by IRinex object data
                auto ep = opts().fullOutput ? GnssEpoch(gRin.getBody()) : GnssEpoch();
				
				//uptate nominal position
				apprPos().getPosition(gRin, nominalPos);

				//fill GnssEpoch by filter state data 
                printSolution( solverFb, gRin.getHeader().epoch, ep);
				
				//add epoch to map
                gMap.data.insert(std::make_pair(gRin.getHeader().epoch, ep));
            }
            std::cout << "Measurments rejected: " << solverFb.rejectedMeasurements << std::endl;
        }
    }

    void PppFloatSolution::updateRequaredObs()
    {
        LinearCombinations comm;
        bool useC1 = confReader().getValueAsBoolean("useC1");

        codeL1 = useC1 ? TypeID::C1 : TypeID::P1;
        computeLinear.setUseC1(useC1);
        computeLinear.add(std::make_unique<PDelta>());
        computeLinear.add(std::make_unique<MWoubenna>());

        computeLinear.add(std::make_unique<LDelta>());
        computeLinear.add(std::make_unique<LICombimnation>());

        configureSolver();

        requireObs.addRequiredType(codeL1);
        requireObs.addRequiredType(TypeID::P2);
        requireObs.addRequiredType(TypeID::L1);
        requireObs.addRequiredType(TypeID::L2);
        requireObs.addRequiredType(TypeID::LLI1);
        requireObs.addRequiredType(TypeID::LLI2);

        requireObs.addRequiredType(TypeID::S1);

        oMinusC.add(std::make_unique<PrefitPC>(true));
        oMinusC.add(std::make_unique<PrefitLC>());

        Equations->measTypes().insert(TypeID::prefitPC);
        Equations->measTypes().insert(TypeID::prefitLC);
        Equations->residTypes().insert(TypeID::postfitPC);
        Equations->residTypes().insert(TypeID::postfitLC);
    }

    void PppFloatSolution::configureSolver()
    {
        Equations->clearEquations();

        double qPrimeVert = confReader().getValueAsDouble("tropoQ1");
        double qPrimeHor = confReader().getValueAsDouble("tropoQ2");

		if (opts().tropoModelType == TropoModelType::Simple)
			Equations->addEquation(std::make_unique<TropoEquations>(qPrimeVert));

		else if (opts().tropoModelType == TropoModelType::SimpleWithGradients)
			Equations->addEquation(std::make_unique<TropoGradEquations>(qPrimeVert, qPrimeHor, qPrimeHor));

		else if (opts().tropoModelType == TropoModelType::Advanced)
			Equations->addEquation(std::make_unique<TropoEquationsAdv>(qPrimeVert, qPrimeHor));

		#pragma region Position stochastic model

		// White noise stochastic models
		auto  coord = std::make_unique<PositionEquations>();

		double posSigma = confReader().getValueAsDouble("posSigma");
		if (opts().dynamics == GnssDataStore::Dynamics::Static)
			coord->setStochasicModel(std::make_shared<ConstantModel>());

		else  if (opts().dynamics == GnssDataStore::Dynamics::Kinematic)
			coord->setStochasicModel(std::make_shared<WhiteNoiseModel>(posSigma));

		else if (opts().dynamics == GnssDataStore::Dynamics::RandomWalk)
			for (const auto& it : coord->getParameters())
				coord->setStochasicModel(it, std::make_shared<RandomWalkModel>(posSigma));

		//add position equations
		Equations->addEquation(std::move(coord));

		#pragma endregion

		if (confReader().getValueAsBoolean("useAdvClkModel"))
			Equations->addEquation(std::make_unique <AdvClockModel>(
				confReader().getValueAsDouble("q1Clk"),
				confReader().getValueAsDouble("q2Clk")));
		else
			Equations->addEquation(std::make_unique<ClockBiasEquations>());

        if (opts().systems.size() > 1)
            Equations->addEquation(std::make_unique<InterSystemBias>());

        Equations->addEquation(std::make_unique<AmbiguitiesEquations>(TypeID::BLC));

        forwardBackwardCycles = confReader().getValueAsInt("forwardBackwardCycles");
    }
}
