#include "PdFloatSolution.h"

#include"SimpleFilter.hpp"
#include"WinUtils.h"
#include"ComputeWeightSimple.h"
#include"SyncObs.h"
#include"KalmanSolver.h"
#include"KalmanSolverFB.h"

#include"TropoEquations.h"
#include"ClockBiasEquations.h"
#include"PositionEquations.h"
#include"InterSystemBias.h"
#include"InterFrequencyBiases.h"
#include"AmbiguitiesEquations.h"
#include"SNRCatcher.h"
#include"IonoEquations.h"
#include"IonoStochasticModel.h"
#include"PrefitResCatcher.h"
#include"NumSatFilter.h"
#include"CatcherStatistic.h"

#include"LinearCombinations.hpp"
#include"LICSDetector.hpp"
#include"MWCSDetector.hpp"
#include"Decimate.hpp"
#include"BasicModel.hpp"
#include"NeillTropModel.hpp"
#include"PowerSum.hpp"
#include"ComputeTropModel.hpp"
#include"LinearCombinations.hpp"
#include"DeltaOp.hpp"
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
#include"MJD.hpp"
#include"IonexModel.hpp"

#include<memory>

using namespace std;
using namespace gpstk;

namespace pod
{

    PdFloatSolution::PdFloatSolution(GnssDataStore_sptr data_ptr)
        :GnssSolution(data_ptr,50.0)
    { }
    PdFloatSolution::PdFloatSolution(GnssDataStore_sptr data_ptr, double max_sigma)
        : GnssSolution(data_ptr, max_sigma)
    { }
    PdFloatSolution::~PdFloatSolution()
    { }

    void  PdFloatSolution::process()
    {
        list<ProcessingClass*> plBase, plRov;
        updateRequaredObs();

        SimpleFilter CodePhaseFilterBase(TypeIDSet{ codeL1,TypeID::P2,TypeID::L1,TypeID::L2 });
        plBase.push_back(&CodePhaseFilterBase);
        SimpleFilter CodePhaseFilterRover(CodePhaseFilterBase);
        plRov.push_back(&CodePhaseFilterRover);

        SimpleFilter SNRFilterBase(TypeID::S1, confReader().getValueAsInt("SNRmask"), DBL_MAX);
        plBase.push_back(&SNRFilterBase);
        SimpleFilter SNRFilterRover(SNRFilterBase);
        plRov.push_back(&SNRFilterRover);

        // Object to remove eclipsed satellites
        EclipsedSatFilter eclipsedSV;

        Triple pos;
        int i = 0;
        for (auto& it : confReader().getListValueAsDouble("nominalPosition", opts().SiteBase))
            pos[i++] = it;
        Position refPos(pos);

        // basic model object for ref. station
        BasicModel modelRef;
        modelRef.setDefaultEphemeris(data->SP3EphList);
        modelRef.setDefaultObservable(codeL1);
        modelRef.setMinElev(opts().maskEl);
        plBase.push_back(&modelRef);
        // basic model object for rover has the same settings as BasicModel for ref. station
        BasicModel modelRover(modelRef);
        modelRef.rxPos = refPos;
        plRov.push_back(&modelRover);
        RinexEpoch gRin, gRef;
        SyncObs sync(data->getObsFiles(opts().SiteBase), gRin);

        //Object to decimate data
        Decimate decimateData(confReader().getValueAsDouble("decimationInterval"),
            confReader().getValueAsDouble("decimationTolerance"),
            data->SP3EphList.getInitialTime());

#pragma region troposhere modeling objects

        //for base 
        NeillTropModel tropoBasePtr;
        ComputeTropModel computeTropoBase(tropoBasePtr);

        //for rover
        NeillTropModel tropoRovPtr;
        ComputeTropModel computeTropoRover(tropoRovPtr);

#pragma endregion

        IonexModel ionoModel(nominalPos, data->ionexStore, TypeID::C1, false);

        // Object to compute gravitational delay effects
        GravitationalDelay grDelayBase(refPos);
        GravitationalDelay grDelayRover;

        plBase.push_back(&grDelayBase);
        plRov.push_back(&grDelayRover);

#pragma region Catcher objects 

        // Objects to mark cycle slips
        // Checks LI cycle slips
        LICSDetector2 markCSLI2Base, markCSLI2Rover; 
        markCSLI2Base.setSatThreshold(confReader().getValueAsDouble("LISatThreshold"));
        markCSLI2Rover.setSatThreshold(confReader().getValueAsDouble("LISatThreshold"));

        // Checks Merbourne-Wubbena cycle slips
        MWCSDetector  markCSMW2Base, markCSMW2Rover;        
        markCSMW2Base.setMaxNumLambdas(confReader().getValueAsDouble("MWNLambdas"));
        markCSMW2Rover.setMaxNumLambdas(confReader().getValueAsDouble("MWNLambdas"));

        plBase.push_back(&markCSLI2Base);
        plBase.push_back(&markCSMW2Base);
        plRov.push_back(&markCSLI2Rover);
        plRov.push_back(&markCSMW2Rover);


        // check sharp SNR drops 
        SNRCatcher snrCatcherL1Base(TypeID::S1, TypeID::CSL1,901.0,5,30);
        SNRCatcher snrCatcherL1Rover(TypeID::S1, TypeID::CSL1, 901.0, 5, 30);
        PrefitResCatcher resCatcher(Equations->measTypes());
        NumSatFilter minSatFilter(desiredSlnType());
        
        plBase.push_back(&snrCatcherL1Base);

        plRov.push_back(&snrCatcherL1Rover);
        plRov.push_back(&resCatcher);
        plRov.push_back(&minSatFilter);

        // Object to keep track of satellite arcs
        SatArcMarker markArcBase(TypeID::CSL1, true, 31.0);
        SatArcMarker markArcRover(TypeID::CSL1, true, 31.0);

#pragma endregion

#pragma region prepare ANTEX reader

        string antxfile = opts().genericFilesDirectory;
        antxfile += confReader().getValue("antexFile");

        AntexReader antexReader;
        antexReader.open(antxfile);

#pragma endregion

#pragma region correct observable

        CorrectObservables corrBase(data->SP3EphList);
        corrBase.setNominalPosition(refPos);

        CorrectObservables corrRover(data->SP3EphList);

        plBase.push_back(&corrBase);
        plRov.push_back(&corrRover);

        // Vector from monument to antenna ARP [UEN], in meters
        //for base
        Triple offsetARP;
        i = 0;
        for (auto &it : confReader().getListValueAsDouble("offsetARP", opts().SiteBase))
            offsetARP[i++] = it;
        corrBase.setMonument(offsetARP);

        //for rover
        i = 0;
        for (auto &it : confReader().getListValueAsDouble("offsetARP", opts().SiteRover))
            offsetARP[i++] = it;
        corrRover.setMonument(offsetARP);

        Antenna baseAnt(antexReader.getAntenna(confReader().getValue("antennaModel", opts().SiteBase)));
        corrBase.setAntenna(baseAnt);
        corrBase.setUseAzimuth(confReader().getValueAsBoolean("useAzim", opts().SiteBase));

        Antenna roverAnt(antexReader.getAntenna(confReader().getValue("antennaModel", opts().SiteRover)));
        corrRover.setAntenna(roverAnt);
        corrRover.setUseAzimuth(confReader().getValueAsBoolean("useAzim", opts().SiteRover));

#pragma endregion

        // Objects to compute tidal effects
        SolidTides solid;
        PoleTides pole;
        // Configure ocean loading model
        OceanLoading ocean;
        ocean.setFilename(opts().genericFilesDirectory + confReader().getValue("oceanLoadingFile"));

        ComputeWindUp windupBase(data->SP3EphList, refPos, opts().genericFilesDirectory + confReader().getValue("satDataFile"));
        ComputeWindUp windupRover(data->SP3EphList, refPos, opts().genericFilesDirectory + confReader().getValue("satDataFile"));
        
        plBase.push_back(&windupBase);
        plRov.push_back(&windupRover);

        ComputeSatPCenter svPcenterBase(refPos);
        svPcenterBase.setAntexReader(antexReader);

        ComputeSatPCenter svPcenterRover;
        svPcenterRover.setAntexReader(antexReader);

        plBase.push_back(&svPcenterBase);
        plRov.push_back(&svPcenterRover);

        ProcessLinear linearIonoFree;
        linearIonoFree.add(make_unique<PCCombimnation>());
        linearIonoFree.add(make_unique<LCCombimnation>());

        //Compute single differences opreator
        DeltaOp delta;

        //configure single differences operator with appropriate measurements types
        TypeIDSet diffTypeSet;
        for (auto&& it : Equations->measTypes())
            diffTypeSet.insert(it);
        delta.setDiffTypeSet(diffTypeSet);

        KalmanSolver solver(Equations);
        solver.setMinSatNumber(5);
        KalmanSolverFB solverFb(Equations);
        solverFb.setMinSatNumber(5);
        if (forwardBackwardCycles > 0)
        {
            solverFb.setCyclesNumber(forwardBackwardCycles);
            solverFb.setLimits(confReader().getListValueAsDouble("codeLimList"), confReader().getListValueAsDouble("phaseLimList"));
        }
 
        bool firstTime = true;
        //
        for (auto &obsFile : data->getObsFiles(opts().SiteRover))
        {
            cout << obsFile << endl;
            //Input observation file stream
            Rinex3ObsStream rin;

            //Open Rinex observations file in read-only mode
            rin.open(obsFile, std::ios::in);

            rin.exceptions(ios::failbit);
            Rinex3ObsHeader roh;

            //read the header
            rin >> roh;
            gMap.header = roh;
            
            Antenna ant_rov(antexReader.getAntenna(roh.antType));
            corrRover.setAntenna(ant_rov);

            //read all epochs
            while (rin >> gRin)
            {
               
                if (gRin.getBody().size() == 0)
                {
                    printMsg(gRin.getHeader().epoch, "Empty epoch record in Rinex file");
                    continue;
                }
                
                const auto& t = gRin.getHeader().epoch;
                bool b;
#if _DEBUG
                CATCH_TIME(t,2014,12,19,0,14,15,b)
                if(b)
                    DBOUT_LINE("catched")
#endif
                //keep only satellites from satellites systems selecyted for processing
                gRin.keepOnlySatSystems(opts().systems);
               
                //keep only types used for processing
                // gRin.keepOnlyTypeID(requireObs.getRequiredType());

                //compute approximate position
                if (firstTime)
                {
                   /* if (computeApprPos(gRin, data->SP3EphList, nominalPos))
                        continue;*/
                    i = 0;
                    for (auto& it : confReader().getListValueAsDouble("nominalPosition", opts().SiteRover))
                        pos[i++] = it;
                    nominalPos = Position(pos);

                    cout << "Baseline: " << setprecision(4) << (nominalPos - refPos).mag() / 1000 << " km" << endl;
                    firstTime = false;
                }

                grDelayRover.setNominalPosition(nominalPos);
                
                tropoRovPtr.setAllParameters(t, nominalPos);
                tropoBasePtr.setAllParameters(t, refPos);
                ionoModel.setInitialRxPosition(nominalPos);

                modelRover.rxPos = nominalPos;
                corrRover.setNominalPosition(nominalPos);
                windupRover.setNominalPosition(nominalPos);
                svPcenterRover.setNominalPosition(nominalPos);


                gRin >> requireObs;
                gRin >> CodePhaseFilterRover;
                gRin >> SNRFilterRover;

                if (gRin.getBody().size() == 0)
                {
                    printMsg(gRin.getHeader().epoch, "Rover receiver: all SV has been rejected.");
                    continue;
                }

                gRin >> computeLinear;
                gRin >> markCSLI2Rover;
                gRin >> markCSMW2Rover;
                gRin >> snrCatcherL1Rover;
                gRin >> markArcRover;

                auto eop = data->eopStore.getEOP(MJD(t).mjd, IERSConvention::IERS2010);
                pole.setXY(eop.xp, eop.yp);

                try
                {
                    gRef >> sync;
                    
                    Antenna ant_ref(antexReader.getAntenna(sync.getRefHeader().antType));
                    corrBase.setAntenna(ant_ref);
                   
                    //keep only satellites from satellites systems selecyted for processing
                    gRef.keepOnlySatSystems(opts().systems);

                    //keep only types used for processing
                    gRef.keepOnlyTypeID(requireObs.getRequiredType());

                    gRef >> requireObs;
                    gRef >> CodePhaseFilterBase;
                    gRef >> SNRFilterBase;

                    gRef >> computeLinear;
                    gRef >> markCSLI2Base;
                    gRef >> markCSMW2Base;
                    gRef >> snrCatcherL1Base;
                    gRef >> markArcBase;

                    if (decimateData.check(gRef))
                        continue;

                    gRef >> modelRef;
                    //gRef >> eclipsedSV;
                    gRef >> grDelayBase;
                    gRef >> svPcenterBase;

                    Triple tides(solid.getSolidTide(t, refPos) + ocean.getOceanLoading(opts().SiteBase, t) + pole.getPoleTide(t, refPos));
                    corrBase.setExtraBiases(tides);
                    
                    gRef >> corrBase;
                    gRef >> windupBase;
                    data->ionoCorrector.setNominalPosition(refPos);
                    

                    gRef >> computeTropoBase;
                   // gRef >> ionoFilterBase;
                    gRef >> linearIonoFree;
                    gRef >> oMinusC;

                    delta.setRefData(gRef.getBody());
                }
                catch (SyncNextRoverEpoch &e)
                {
                    continue;
                }
                catch (SynchronizeException &e)
                {
                    break;
                }

                gRin >> modelRover;
                //gRin >> eclipsedSV;
                gRin >> grDelayRover;
                gRin >> svPcenterRover;

                Triple tides(solid.getSolidTide(t, nominalPos) + ocean.getOceanLoading(opts().SiteRover, t) + pole.getPoleTide(t, nominalPos));
                corrRover.setExtraBiases(tides);
                gRin >> corrRover;
                
                gRin >> windupRover;
                gRin >> computeTropoRover;
                //gRin >> ionoFilterRover;
                gRin >> linearIonoFree;
                gRin >> oMinusC;
                gRin >> delta;
                gRin >> resCatcher;
                gRin >> minSatFilter;

                DBOUT_LINE(">>" << CivilTime(gRin.getHeader().epoch).asString());
                if (forwardBackwardCycles > 0)
                {
                    gRin >> solverFb;
                }
                else
                {
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
            cout << "Fw-Bw part started" << endl;
            solverFb.reProcess();
            RinexEpoch gRin;
            cout << "Last process part started" << endl;
            while (solverFb.lastProcess(gRin))
            {
                auto ep = opts().fullOutput ? GnssEpoch(gRin.getBody()) : GnssEpoch();
                //updateNomPos(solverFB);
                printSolution( solverFb, gRin.getHeader().epoch, ep);
                gMap.data.insert(std::make_pair(gRin.getHeader().epoch, ep));
            }
            cout << "Measurments rejected: " << solverFb.rejectedMeasurements << endl;
        }

        //CatcherStatistic statB(opts().workingDir+"\\"+opts().SiteBase+".cst");
        //statB.logStatistic(plBase);
        //CatcherStatistic statR(opts().workingDir+"\\"+opts().SiteRover+".cst");
        //statR.logStatistic(plRov);

    }

    void PdFloatSolution::updateRequaredObs()
    {
        LinearCombinations comm;
        bool useC1 = confReader().getValueAsBoolean("useC1");

        codeL1 = useC1 ? TypeID::C1 : TypeID::P1;
        computeLinear.setUseC1(useC1);
        computeLinear.add(make_unique<PDelta>());
        computeLinear.add(make_unique<MWoubenna>());

        computeLinear.add(make_unique<LDelta>());
        computeLinear.add(make_unique<LICombimnation>());

        configureSolver();

        requireObs.addRequiredType(codeL1);
        requireObs.addRequiredType(TypeID::P2);
        requireObs.addRequiredType(TypeID::L1);
        requireObs.addRequiredType(TypeID::L2);
        requireObs.addRequiredType(TypeID::LLI1);
        requireObs.addRequiredType(TypeID::LLI2);

        requireObs.addRequiredType(TypeID::S1);

        if (opts().carrierBands.find(CarrierBand::L1) != opts().carrierBands.end())
        {
            if (useC1)
                oMinusC.add(make_unique<PrefitC1>(true));
            else
                oMinusC.add(make_unique<PrefitP1>(true));

            oMinusC.add(make_unique<PrefitL1>());
            
            Equations->measTypes().insert(TypeID::prefitC);
            Equations->measTypes().insert(TypeID::prefitL1);
            Equations->residTypes().insert(TypeID::postfitC);
            Equations->residTypes().insert(TypeID::postfitL1);

        }
        if (opts().carrierBands.find(CarrierBand::L2) != opts().carrierBands.end())
        {
            oMinusC.add(make_unique<PrefitP2>(true));
            oMinusC.add(make_unique<PrefitL2>());

            Equations->measTypes().insert(TypeID::prefitP2);
            Equations->measTypes().insert(TypeID::prefitL2);
            Equations->residTypes().insert(TypeID::postfitP2);
            Equations->residTypes().insert(TypeID::postfitL2);
        }
    }

    void PdFloatSolution::configureSolver()
    {
        Equations->clearEquations();

        //tropo
        if (opts().computeTropo)
        {
            double qPrime = confReader().getValueAsDouble("tropoQ");
            Equations->addEquation(make_unique<TropoEquations>(qPrime));
        }
       //  Equations->addEquation(std::make_unique<IonoEquations>(confReader().getValueAsDouble("ionoQ")));
        // White noise stochastic models
        auto  coord = make_unique<PositionEquations>();

        double posSigma = confReader().getValueAsDouble("posSigma");
        if (opts().dynamics == GnssDataStore::Dynamics::Static)
        {
            coord->setStochasicModel(make_shared<ConstantModel>());
        }
        else  if (opts().dynamics == GnssDataStore::Dynamics::Kinematic)
        {
            coord->setStochasicModel(make_shared<WhiteNoiseModel>(posSigma));
        }
        else if (opts().dynamics == GnssDataStore::Dynamics::RandomWalk)
        {
            for (const auto& it : coord->getParameters())
                coord->setStochasicModel(it, make_shared<RandomWalkModel>(posSigma));
        }

        //add position equations
        Equations->addEquation(std::move(coord));

        Equations->addEquation(std::make_unique<ClockBiasEquations>(confReader().getValueAsDouble("clkSigma")));

        if (opts().systems.size() > 1)
            Equations->addEquation(std::make_unique<InterSystemBias>());

        if (opts().carrierBands.size() > 1)
            Equations->addEquation(std::make_unique<InterFrequencyBiases>());

        if (confReader().getValueAsBoolean("computeIono"))
        {
            auto  ionoEq = make_unique<IonoEquations>();

            int ionoModelType= confReader().getValueAsInt("ionoModelType");

            if (ionoModelType == 0)
                ionoEq->setStocModel<WhiteNoiseModel>();
            else if (ionoModelType == 1)
                ionoEq->setStocModel<IonoStochasticModel>();
            
            double ionoSigma = confReader().getValueAsDouble("ionoSigma");
            ionoEq->setSigma(ionoSigma);
            Equations->addEquation(std::move(ionoEq));
        }

        if (opts().carrierBands.find(CarrierBand::L1) != opts().carrierBands.end())
            Equations->addEquation(std::make_unique<AmbiguitiesEquations>(TypeID::BL1));

        if (opts().carrierBands.find(CarrierBand::L2) != opts().carrierBands.end())
            Equations->addEquation(std::make_unique<AmbiguitiesEquations>(TypeID::BL2));

        forwardBackwardCycles = confReader().getValueAsInt("forwardBackwardCycles");
    }
}
