#include "CdDiffSolution.h"

#include"SimpleFilter.hpp"
#include"WinUtils.h"
#include"ComputeWeightSimple.h"
#include"SyncObs.h"
#include"PositionEquations.h"
#include"InterSystemBias.h"
#include"ClockBiasEquations.h"

#include"LICSDetector.hpp"
#include"MWCSDetector.hpp"
#include"OneFreqCSDetector.hpp"
#include"Decimate.hpp"
#include"BasicModel.hpp"
#include"NeillTropModel.hpp"
#include"PowerSum.hpp"
#include"ComputeMOPSWeights.hpp"
#include"ComputeTropModel.hpp"
#include"LinearCombinations.hpp"
#include"DeltaOp.hpp"

#include<memory>

using namespace gpstk;

namespace pod
{

    CdDiffSolution::CdDiffSolution(GnssDataStore_sptr data_ptr)
        : SingleSolution(data_ptr)
    {
    }

    CdDiffSolution::~CdDiffSolution()
    {
    }

    ///
    void CdDiffSolution::process()
    {
        updateRequaredObs();
        SimpleFilter CodeFilter(codeL1);
        if (data->ionoCorrector.getType() == ComputeIonoModel::DualFreq)
            CodeFilter.addFilteredType(TypeID::P2);

        SimpleFilter SNRFilter(TypeID::S1, confReader().getValueAsInt("SNRmask"), DBL_MAX);

        Triple pos;
        int i = 0;
        for (auto& it : confReader().getListValueAsDouble("nominalPosition",opts().SiteBase))
            pos[i++] = it;
        Position refPos(pos);

        // basic model object for ref. station
        BasicModel modelRef;
        modelRef.setDefaultEphemeris(data->SP3EphList);
        modelRef.setDefaultObservable(codeL1);
        modelRef.setMinElev(confReader().getValueAsInt("ElMask"));

        // basic model object for rover has the same settings as BasicModel for ref. station
        BasicModel modelRover(modelRef);

        modelRef.rxPos = refPos;

        RinexEpoch gRin, gRef;
        SyncObs sync(data->getObsFiles(opts().SiteBase), gRin);

        // Object to decimate data
        Decimate decimateData(
            confReader().getValueAsDouble("decimationInterval"),
            confReader().getValueAsDouble("decimationTolerance"),
            data->SP3EphList.getInitialTime());

        //troposhere modeling objects
        //for base 
        NeillTropModel tropoBase;
  
        ComputeTropModel computeTropoBase(tropoBase);
        
        //for rover
        NeillTropModel tropoRov;
        ComputeTropModel computeTropoRover(tropoRov);

        //
        //data->ionoCorrector.setNominalPosition(refPos);

        //
        ComputeWeightSimple w;
        
        //Compute single differenceses opreator
        DeltaOp delta;
 
        KalmanSolver solver(Equations);
        KalmanSolverFB solverFb(Equations);
        if (forwardBackwardCycles > 0)
        {
            solverFb.setCyclesNumber(forwardBackwardCycles);
            solverFb.setLimits(confReader().getListValueAsDouble("codeLimList"), confReader().getListValueAsDouble("phaseLimList"));
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

            //update code smoothers sampling rate, according to current rinex file sampling rate
            codeSmoother.setInterval(codeSmWindowSize / roh.interval);

            //read all epochs
            while (rin >> gRin)
            {
                if (gRin.getBody().size() == 0)
                {
                    printMsg(gRin.getHeader().epoch, "Empty epoch record in Rinex file");
                    continue;
                }

                const auto& t = gRin.getHeader().epoch;

                //keep only satellites from satellites systems selecyted for processing
                gRin.keepOnlySatSystems(opts().systems);

                //keep only types used for processing
                gRin.keepOnlyTypeID(requireObs.getRequiredType());

                //compute approximate position
               
					if (apprPos().getPosition(gRin, nominalPos))
						continue;

					if (firstTime)
					{
						std::cout << "Baseline: " << std::setprecision(4) << (nominalPos - refPos).mag() / 1000 << " km" << std::endl;
						firstTime = false;
					}
                
                tropoRov.setAllParameters(t, nominalPos);
                tropoBase.setAllParameters(t, refPos);
                modelRover.rxPos = nominalPos;

                gRin >> requireObs;
                gRin >> CodeFilter;
                gRin >> SNRFilter;
                gRin >> computeLinear;

                if (opts().isSmoothCode)
                    gRin >> codeSmoother;

                try
                {
                    gRef >> sync;
                    //keep only satellites from satellites systems selecyted for processing
                    gRef.keepOnlySatSystems(opts().systems);

                    //keep only types used for processing
                    gRef.keepOnlyTypeID(requireObs.getRequiredType());

                    gRef >> requireObs;
                    gRef >> CodeFilter;
                    gRef >> SNRFilter;
                    gRef >> computeLinear;

                    if (opts().isSmoothCode)
                    {
                        //update code smoother interval length
                        codeSmootherRef.setInterval(codeSmWindowSize / sync.getRefHeader().interval);
                        
                        //let's smooth the code
                        gRef >> codeSmootherRef;
                    }

                    if (gRef.getBody().size() == 0)
                    {
                        printMsg(gRef.getHeader().epoch, "Reference receiver: all SV has been rejected.");
                        continue;
                    }

                    if (decimateData.check(gRef))
                        continue;

                    gRef >> modelRef;
                    gRef >> computeTropoBase;
                    
                    data->ionoCorrector.setNominalPosition(refPos);
                    gRef >> data->ionoCorrector;

                    gRef >> oMinusC;

                    delta.setRefData(gRef.getBody());
                }
                catch (SynchronizeException &e)
                {
                    break;
                }
                
                gRin >> modelRover;
                gRin >> computeTropoRover;

                data->ionoCorrector.setNominalPosition(nominalPos);
                gRin >> data->ionoCorrector;
                
                gRin >> oMinusC;
                gRin >> delta;
                gRin >> w;
                
                if (forwardBackwardCycles > 0)
                {
					solverFb.setMinSatNumber(3 + gRin.getBody().getSatSystems().size());
                    gRin >> solverFb;
                }
                else
                {
					solver.setMinSatNumber(3 + gRin.getBody().getSatSystems().size());
                    gRin >> solver;
                    auto ep = opts().fullOutput? GnssEpoch(gRin.getBody()): GnssEpoch();
                    // updateNomPos(solverFB);
                    printSolution( solver, t, ep);
                    gMap.data.insert(std::make_pair(t, ep));
                }
            }
        }
        if (forwardBackwardCycles > 0)
        {
            std::cout << "Fw-Bw part started" << std::endl;
            solverFb.reProcess();
            RinexEpoch gRin;
            std::cout << "Last process part started" << std::endl;
            while (solverFb.lastProcess(gRin))
            {
                auto ep = opts().fullOutput ? GnssEpoch(gRin.getBody()) : GnssEpoch();
                //updateNomPos(solverFB);
                printSolution( solverFb, gRin.getHeader().epoch, ep);
                gMap.data.insert(std::make_pair(gRin.getHeader().epoch, ep));
            }
            std::cout << "Measurments rejected: " << solverFb.rejectedMeasurements << std::endl;
        }
    }
    void CdDiffSolution::configureSolver()
    {
        Equations->clearEquations();
        // White noise stochastic models
        auto  coord = std::make_unique<PositionEquations>();

        double sigma = confReader().getValueAsDouble("posSigma");
        if (opts().dynamics == GnssDataStore::Dynamics::Static)
        {
            coord->setStochasicModel(std::make_shared<ConstantModel>());
        }
        else  if (opts().dynamics == GnssDataStore::Dynamics::Kinematic)
        {
            coord->setStochasicModel(std::make_shared<WhiteNoiseModel>(sigma));
        }
        else if (opts().dynamics == GnssDataStore::Dynamics::RandomWalk)
        {
            for (const auto& it : coord->getParameters())
            {
                coord->setStochasicModel(it, std::make_shared<RandomWalkModel>(sigma));
            }
        }
        
        //add position equations
        Equations->addEquation(std::move(coord));
        
        Equations->addEquation(std::make_unique<ClockBiasEquations>());

        if (opts().systems.size() > 1)
            Equations->addEquation(/*std::move(bias)*/std::make_unique<InterSystemBias>());
        

        Equations->residTypes() = TypeIDSet{ TypeID::postfitC };
        forwardBackwardCycles = confReader().getValueAsInt("forwardBackwardCycles");

    }


    ///
    void CdDiffSolution::updateRequaredObs()
    {
        LinearCombinations comm;
        bool useC1 = confReader().getValueAsBoolean("useC1");
        computeLinear.setUseC1(useC1);
        
        configureSolver();

        if (useC1)
        {
            codeL1 = TypeID::C1;
			oMinusC.add(std::make_unique<PrefitC1>(false));
            Equations->measTypes() = { TypeID::prefitC };
        }
        else
        {
            codeL1 = TypeID::P1;
            oMinusC.add(std::make_unique<PrefitP1>(false));
            Equations->measTypes() = { TypeID::prefitP1 };
        }

        requireObs.addRequiredType(codeL1);
        requireObs.addRequiredType(TypeID::C1);
        requireObs.addRequiredType(TypeID::P2);
        requireObs.addRequiredType(TypeID::L1);
        requireObs.addRequiredType(TypeID::L2);
        requireObs.addRequiredType(TypeID::LLI1);
        requireObs.addRequiredType(TypeID::LLI2);
        requireObs.addRequiredType(TypeID::S1);

        if (opts().isSmoothCode)
        {
            codeSmoother.addSmoother(CodeSmoother(codeL1));
            codeSmoother.addSmoother(CodeSmoother(TypeID::P2));

            codeSmootherRef.addSmoother(CodeSmoother(codeL1));
            codeSmootherRef.addSmoother(CodeSmoother(TypeID::P2));

            // add linear combinations, requared  for CS detections 
            computeLinear.add(std::make_unique<LICombimnation>());
            computeLinear.add(std::make_unique<MWoubenna>());

            //define and add  CS markers
            codeSmoother.addScMarker(std::make_unique<LICSDetector>());
            codeSmoother.addScMarker(std::make_unique<MWCSDetector>());

            codeSmootherRef.addScMarker(std::make_unique<LICSDetector>());
            codeSmootherRef.addScMarker(std::make_unique<MWCSDetector>());
        }
    }
}
