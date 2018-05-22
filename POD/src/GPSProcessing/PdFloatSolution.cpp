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
#include"AmbiguitySdEquations.h"

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

#include<memory>

using namespace std;
using namespace gpstk;

namespace pod
{

    PdFloatSolution::PdFloatSolution(GnssDataStore_sptr data_ptr)
        :GnssSolution(data_ptr,50.0)
    { }

    PdFloatSolution::~PdFloatSolution()
    { }

    void  PdFloatSolution::process()
    {
        updateRequaredObs();

        SimpleFilter CodeFilter(TypeIDSet{ codeL1,TypeID::P2 });
        SimpleFilter SNRFilter(TypeID::S1, confReader().getValueAsInt("SNRmask"), DBL_MAX);
        // Object to remove eclipsed satellites
        EclipsedSatFilter eclipsedSV;

        Triple pos;
        int i = 0;
        for (auto& it : confReader().getListValueAsDouble("nominalPosition", data->SiteBase))
            pos[i++] = it;
        Position refPos(pos);

        // basic model object for ref. station
        BasicModel modelRef;
        modelRef.setDefaultEphemeris(data->SP3EphList);
        modelRef.setDefaultObservable(codeL1);
        modelRef.setMinElev(opts().maskEl);

        // basic model object for rover has the same settings as BasicModel for ref. station
        BasicModel modelRover(modelRef);
        modelRef.rxPos = refPos;

        gnssRinex gRin, gRef;
        SyncObs sync(data->getObsFiles(data->SiteBase), gRin);

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

        // Object to compute gravitational delay effects
        GravitationalDelay grDelayBase(refPos);
        GravitationalDelay grDelayRover;

#pragma region CS detectors

        // Objects to mark cycle slips
        LICSDetector2 markCSLI2Base, markCSLI2Rover; // Checks LI cycle slips
        markCSLI2Base.setSatThreshold(confReader().getValueAsDouble("LISatThreshold"));
        markCSLI2Rover.setSatThreshold(confReader().getValueAsDouble("LISatThreshold"));
        MWCSDetector  markCSMW2Base, markCSMW2Rover;          // Checks Merbourne-Wubbena cycle slips
        markCSMW2Base.setMaxNumLambdas(confReader().getValueAsDouble("MWNLambdas"));
        markCSMW2Rover.setMaxNumLambdas(confReader().getValueAsDouble("MWNLambdas"));

        // Object to keep track of satellite arcs
        SatArcMarker markArcBase(TypeID::CSL1, true, 31.0);
        SatArcMarker markArcRover(TypeID::CSL1, true, 31.0);

#pragma endregion

#pragma region prepare ANTEX reader

        string antxfile = data->genericFilesDirectory;
        antxfile += confReader().getValue("antexFile");

        AntexReader antexReader;
        antexReader.open(antxfile);

#pragma endregion

#pragma region correct observable

        CorrectObservables corrBase(data->SP3EphList);
        corrBase.setNominalPosition(refPos);

        CorrectObservables corrRover(data->SP3EphList);

        // Vector from monument to antenna ARP [UEN], in meters
        //for base
        Triple offsetARP;
        i = 0;
        for (auto &it : confReader().getListValueAsDouble("offsetARP", data->SiteBase))
            offsetARP[i++] = it;
        corrBase.setMonument(offsetARP);

        //for rover
        i = 0;
        for (auto &it : confReader().getListValueAsDouble("offsetARP", data->SiteRover))
            offsetARP[i++] = it;
        corrRover.setMonument(offsetARP);

        Antenna baseAnt(antexReader.getAntenna(confReader().getValue("antennaModel", data->SiteBase)));
        corrBase.setAntenna(baseAnt);
        corrBase.setUseAzimuth(confReader().getValueAsBoolean("useAzim", data->SiteBase));

        Antenna roverAnt(antexReader.getAntenna(confReader().getValue("antennaModel", data->SiteRover)));
        corrRover.setAntenna(roverAnt);
        corrRover.setUseAzimuth(confReader().getValueAsBoolean("useAzim", data->SiteRover));


#pragma endregion

        // Objects to compute tidal effects
        SolidTides solid;
        PoleTides pole;
        // Configure ocean loading model
        OceanLoading ocean;
        ocean.setFilename(data->genericFilesDirectory + confReader().getValue("oceanLoadingFile"));

        ComputeWindUp windupBase(data->SP3EphList, refPos, data->genericFilesDirectory + confReader().getValue("satDataFile"));
        ComputeWindUp windupRover(data->SP3EphList, refPos, data->genericFilesDirectory + confReader().getValue("satDataFile"));

        ComputeSatPCenter svPcenterBase(refPos);
        svPcenterBase.setAntexReader(antexReader);
        ComputeSatPCenter svPcenterRover;
        svPcenterRover.setAntexReader(antexReader);

        ProcessLinear linearIonoFree;
        linearIonoFree.add(make_unique<PCCombimnation>());
        linearIonoFree.add(make_unique<LCCombimnation>());
        //Compute single differenceses opreator
        DeltaOp delta;
        for (const auto& it : Equations->measTypes())
            delta.addDiffType(it);

        KalmanSolver solver(Equations);
        KalmanSolverFB solverFb(Equations);
        if (forwardBackwardCycles > 0)
            solverFb.setCodeLims(confReader().getListValueAsDouble("codeLimList"));

        ofstream ostream;
        ostream.open(data->workingDir + "\\" + fileName(), ios::out);

        bool firstTime = true;
        //
        for (auto &obsFile : data->getObsFiles(data->SiteRover))
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

            //read all epochs
            while (rin >> gRin)
            {
                if (gRin.body.size() == 0)
                {
                    printMsg(gRin.header.epoch, "Empty epoch record in Rinex file");
                    continue;
                }

                const auto& t = gRin.header.epoch;

                //keep only satellites from satellites systems selecyted for processing
                gRin.keepOnlySatSyst(opts().systems);

                //keep only types used for processing
               // gRin.keepOnlyTypeID(requireObs.getRequiredType());

                //compute approximate position
                if (firstTime)
                {
                   /* if (computeApprPos(gRin, data->SP3EphList, nominalPos))
                        continue;*/
                    i = 0;
                    for (auto& it : confReader().getListValueAsDouble("nominalPosition", data->SiteRover))
                        pos[i++] = it;
                    nominalPos = Position(pos);

                    cout << "Baseline: " << setprecision(4) << (nominalPos - refPos).mag() / 1000 << " km" << endl;
                    firstTime = false;
                }

                grDelayRover.setNominalPosition(nominalPos);
                
                tropoRovPtr.setAllParameters(t, nominalPos);
                tropoBasePtr.setAllParameters(t, refPos);

                modelRover.rxPos = nominalPos;
                corrRover.setNominalPosition(nominalPos);
                windupRover.setNominalPosition(nominalPos);
                svPcenterRover.setNominalPosition(nominalPos);


                gRin >> requireObs;
                gRin >> CodeFilter;
                gRin >> SNRFilter;

                if (gRin.body.size() == 0)
                {
                    printMsg(gRef.header.epoch, "Rover receiver: all SV has been rejected.");

                    continue;
                }
                gRin >> computeLinear;
                gRin >> markCSLI2Rover;
                gRin >> markCSMW2Rover;
                gRin >> markArcRover;

                auto eop = data->eopStore.getEOP(MJD(t).mjd, IERSConvention::IERS2010);
                pole.setXY(eop.xp, eop.yp);

                try
                {
                    gRef >> sync;
                    //keep only satellites from satellites systems selecyted for processing
                    gRef.keepOnlySatSyst(opts().systems);

                    //keep only types used for processing
                    gRef.keepOnlyTypeID(requireObs.getRequiredType());

                    gRef >> requireObs;
                    gRef >> CodeFilter;
                    gRef >> SNRFilter;

                    if (gRef.body.size() == 0)
                    {
                        printMsg(gRef.header.epoch, "Reference receiver: all SV has been rejected.");
                        continue;
                    }

                    gRef >> computeLinear;
                    gRef >> markCSLI2Base;
                    gRef >> markCSMW2Base;
                    gRef >> markArcBase;

                    if (decimateData.check(gRef))
                        continue;

                    gRef >> modelRef;
                    gRef >> eclipsedSV;
                    gRef >> grDelayBase;
                    gRef >> svPcenterBase;

                    Triple tides(solid.getSolidTide(t, refPos) + ocean.getOceanLoading(data->SiteBase, t) + pole.getPoleTide(t, refPos));
                    corrBase.setExtraBiases(tides);
                    
                    gRef >> corrBase;
                    gRef >> windupBase;
                    data->ionoCorrector.setNominalPosition(refPos);
                    
                    //gRef >> data->ionoCorrector;
                    gRef >> computeTropoBase;
                    gRef >> linearIonoFree;
                    gRef >> oMinusC;

                    delta.setRefData(gRef.body);
                }
                catch (SynchronizeException &e)
                {
                    break;
                }

                gRin >> modelRover;
                gRin >> eclipsedSV;
                gRin >> grDelayRover;
                gRin >> svPcenterRover;

                Triple tides(solid.getSolidTide(t, nominalPos) + ocean.getOceanLoading(data->SiteRover, t) + pole.getPoleTide(t, nominalPos));
                corrRover.setExtraBiases(tides);
                gRin >> corrRover;
                
                gRin >> windupRover;
                gRin >> computeTropoRover;
                gRin >> linearIonoFree;
                gRin >> oMinusC;
                gRin >> delta;

                if (forwardBackwardCycles > 0)
                {
                    gRin >> solverFb;
                }
                else
                {
                    gRin >> solver;
                    GnssEpoch ep(gRin);
                    // updateNomPos(solverFB);
                    printSolution(ostream, solver, t, ep);
                    gMap.data.insert(std::make_pair(t, ep));
                }
            }
        }
        if (forwardBackwardCycles > 0)
        {
            cout << "Fw-Bw part started" << endl;
            solverFb.reProcess();
            gnssRinex gRin;
            cout << "Last process part started" << endl;
            while (solverFb.lastProcess(gRin))
            {
                GnssEpoch ep(gRin);
                //updateNomPos(solverFB);
                printSolution(ostream, solverFb, gRin.header.epoch, ep);
                gMap.data.insert(std::make_pair(gRin.header.epoch, ep));
            }
            cout << "Measurments rejected: " << solverFb.rejectedMeasurements << endl;
        }
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

      
        Equations->residTypes() = TypeIDList{ TypeID::postfitC, TypeID::postfitL };

        requireObs.addRequiredType(codeL1);
        requireObs.addRequiredType(TypeID::L1);
        requireObs.addRequiredType(TypeID::L2);
        requireObs.addRequiredType(TypeID::LLI1);
        requireObs.addRequiredType(TypeID::LLI2);
        requireObs.addRequiredType(TypeID::P2);
        requireObs.addRequiredType(TypeID::S1);
        
        ///
        if (data->ionoCorrector.getType() == IonoModelType::DualFreq )
        {
            Equations->measTypes() = TypeIDList{ TypeID::prefitC,TypeID::prefitL };

            oMinusC.addLinear(comm.pcPrefit);
            oMinusC.addLinear(comm.lcPrefit);
        }
        else
        {
            Equations->measTypes() = TypeIDList{ TypeID::prefitC,TypeID::prefitL1 };

            oMinusC.addLinear(useC1 ? comm.c1Prefit : comm.p1Prefit);
            oMinusC.addLinear(comm.l1Prefit);
        }
    }

    void PdFloatSolution::configureSolver()
    {
        Equations->clear();
        
        //tropo
        //double qPrime = confReader().getValueAsDouble("tropoQ");
        //Equations->addEquation( make_unique<TropoEquations>(qPrime));

        // White noise stochastic models
        auto  coord = make_unique<PositionEquations>();

        double sigma = confReader().getValueAsDouble("posSigma");
        if (opts().dynamics == GnssDataStore::Dynamics::Static)
        {
            coord->setStochasicModel(make_shared<StochasticModel>());
        }
        else  if (opts().dynamics == GnssDataStore::Dynamics::Kinematic)
        {
            coord->setStochasicModel(make_shared<WhiteNoiseModel>(sigma));
        }
        else if (opts().dynamics == GnssDataStore::Dynamics::RandomWalk)
        {
            for (const auto& it : coord->getEquationTypes())
                coord->setStochasicModel(it, make_shared<RandomWalkModel>(sigma));
        }

        //add position equations
        Equations->addEquation(std::move(coord));

        Equations->addEquation(make_unique<ClockBiasEquations>());
        auto bias = make_unique<InterSystemBias>();
        bias->setStochasicModel(SatID::systemGlonass, make_unique<WhiteNoiseModel>());
        
        Equations->addEquation(/*std::move(bias)*/std::make_unique<InterSystemBias>());
        
        
        if(data->ionoCorrector.getType() == IonoModelType::DualFreq)
            Equations->addEquation(std::make_unique<AmbiguitySdEquations>(AmbiguitySdEquations::L1L2_IF));
        else 
            Equations->addEquation(std::make_unique<AmbiguitySdEquations>(AmbiguitySdEquations::L1));

        forwardBackwardCycles = confReader().getValueAsInt("forwardBackwardCycles");
    }

    void PdFloatSolution::printSolution(std::ofstream& os, const SolverLMS& solver, const CommonTime& time, GnssEpoch& gEpoch)
    {
        GnssSolution::printSolution(os, solver, time, gEpoch);
        const auto & params = Equations->currentUnknowns();
        const auto & it_tropo = params.find(TypeID::wetMap);
        
        if (it_tropo != params.end())
        {
            //double wetMap = solver.getSolution(TypeID::wetMap) + 0.1 + this->tropModel.dry_zenith_delay();
            gEpoch.slnData.insert(std::make_pair(TypeID::recZTropo, solver.getSolution(TypeID::wetMap)));
        }
        auto ambType = (data->ionoCorrector.getType() == IonoModelType::DualFreq) ? TypeID::BLC : TypeID::BL1;

        const auto svs = gEpoch.satData.getSatID();
        
        int i(0);
        for (const auto & it : svs)
        {
            gEpoch.satData[it][ambType] = solver.Solution()(params.size() + i);
            ++i;
        }
    }
}
