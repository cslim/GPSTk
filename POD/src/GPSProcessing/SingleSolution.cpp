#include "SingleSolution.h"

#include"SimpleFilter.hpp"
#include"WinUtils.h"
#include"ComputeWeightSimple.h"

#include"PositionEquations.h"
#include"InterSystemBias.h"
#include"ClockBiasEquations.h"

#include"LICSDetector.hpp"
#include"OneFreqCSDetector.hpp"
#include"Decimate.hpp"
#include"BasicModel.hpp"
#include"NeillTropModel.hpp"
#include"PowerSum.hpp"
#include"ComputeMOPSWeights.hpp"
#include"ComputeTropModel.hpp"
#include"LinearCombinations.hpp"
#include"MWCSDetector.hpp"

#include<memory>

using namespace std;
using namespace gpstk;

namespace pod
{

    SingleSolution::SingleSolution(GnssDataStore_sptr data_ptr)
        : GnssSolution(data_ptr,50.0), codeSmWindowSize(600)      
    { }

    //
    void SingleSolution::process()
    {
        updateRequaredObs();

        SimpleFilter PRFilter(codeL1);
        if (data->ionoCorrector.getType() == ComputeIonoModel::DualFreq)
            PRFilter.addFilteredType(TypeID::P2);

        SimpleFilter SNRFilter(TypeID::S1, 30, DBL_MAX);
        list<Position> nomPos;
        //nominalPos.asECEF();
        //int i = 0;
        //for (auto& it : confReader().getListValueAsDouble("nominalPosition"))
        //    nominalPos[i++] = it;

        // Object to decimate data
        Decimate decimateData(
            confReader().getValueAsDouble("decimationInterval"),
            confReader().getValueAsDouble("decimationTolerance"),
            data->SP3EphList.getInitialTime());

        // basic model object
        BasicModel model;
        model.setDefaultEphemeris(data->SP3EphList);
        model.setDefaultObservable(codeL1);
        model.setMinElev(confReader().getValueAsInt("ElMask"));

        //troposhere modeling object
        unique_ptr<NeillTropModel> uptrTropModel = make_unique<NeillTropModel>();
        ComputeTropModel computeTropo(*uptrTropModel);

        //
        ComputeWeightSimple w;
        KalmanSolver solver(Equations);
        KalmanSolverFB solverFb(Equations);
        if (forwardBackwardCycles > 0)
            solverFb.setCodeLims(confReader().getListValueAsDouble("codeLimList"));

        ofstream ostream;
        ostream.open(data->workingDir + "\\" + fileName(), ios::out);

        bool firstTime = true;

        gnssRinex gRin;

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

            //update code smoothers sampling rate, according to current rinex file sampling rate
            codeSmoother.setInterval(codeSmWindowSize / roh.interval);

            //read all epochs
            while (rin >> gRin)
            {
                //work around for post header comments 
                if (gRin.body.size() == 0)
                {
                    printMsg(gRin.header.epoch, "Empty epoch record in Rinex file");
                    continue;
                }

                auto& t = gRin.header.epoch;

                //keep only satellites from satellites systems selected for processing
                gRin.keepOnlySatSyst(opts().systems);

                //keep only types used for processing
                gRin.keepOnlyTypeID(requireObs.getRequiredType());

                //compute approximate position
                if (firstTime)
                {
                    if (computeApprPos(gRin, data->SP3EphList, nominalPos))
                        continue;
                    cout << setprecision(10) << nominalPos << endl;
                    firstTime = false;
                }

                //update approximate position 
                data->ionoCorrector.setNominalPosition(nominalPos);
                uptrTropModel->setAllParameters(t, nominalPos);
                model.rxPos = nominalPos;

                //filter out satellites with incomplete observables set 
                gRin >> requireObs;
                gRin >> PRFilter;
                gRin >> SNRFilter;
                gRin >> computeLinear;

                // smooth pseudoranges if required
                if (opts().isSmoothCode)
                    gRin >> codeSmoother;

                if (gRin.body.size() == 0)
                {
                    printMsg(gRin.header.epoch, "All SV has been rejected.");
                    continue;
                }

                if (decimateData.check(gRin))
                    continue;

                gRin >> model;
                gRin >> computeTropo;
                gRin >> data->ionoCorrector;
                gRin >> oMinusC;
                gRin >> w;

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
            cout << "measurments rejected: " << solverFb.rejectedMeasurements << endl;
        }
    }

    void SingleSolution::updateNomPos(KalmanSolver &solver)
    {
        PowerSum psum;
        for (auto it : solver.PostfitResiduals())
            psum.add(it);
        double sigma = sqrt(psum.variance());

        int numSats = solver.PostfitResiduals().size();
        Position newPos;
        if (numSats >= 4 && sigma < getMaxSigma())
        {
            newPos[0] = nominalPos.X() + solver.getSolution(TypeID::dx);    // dx    - #4
            newPos[1] = nominalPos.Y() + solver.getSolution(TypeID::dy);    // dy    - #5
            newPos[2] = nominalPos.Z() + solver.getSolution(TypeID::dz);    // dz    - #6

            nominalPos = newPos;
        }
    }

    void SingleSolution::configureSolver()
    {
        Equations->clear();
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
            {
                coord->setStochasicModel(it, make_shared<RandomWalkModel>(sigma));
            }
        }

        //add position equations
        Equations->addEquation(std::move(coord));

        Equations->addEquation(make_unique<ClockBiasEquations>());

        //auto bias = make_unique<InterSystemBias>();
        //bias->setStochasicModel(SatID::systemGlonass, make_unique<WhiteNoiseModel>());
        Equations->addEquation(make_unique<InterSystemBias>());
        Equations->residTypes() = TypeIDList{ TypeID::postfitC };
        forwardBackwardCycles = confReader().getValueAsInt("forwardBackwardCycles");
    }

    void SingleSolution::updateRequaredObs()
    {
        LinearCombinations comm;
        bool useC1 = confReader().getValueAsBoolean("useC1");
        computeLinear.setUseC1(useC1);
        
        configureSolver();

        if (useC1)
        {
            codeL1 = TypeID::C1;
            oMinusC.add(make_unique<PrefitC1>());
            Equations->measTypes() = TypeIDList{ TypeID::prefitC };
        }
        else
        {
            codeL1 = TypeID::P1;
            Equations->measTypes() = TypeIDList{ TypeID::prefitP1 };
            oMinusC.add(make_unique<PrefitP1>());
        }

        requireObs.addRequiredType(codeL1);

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

            // add linear combinations, requared  for CS detections 
            computeLinear.add(std::make_unique<LICombimnation>());
            computeLinear.add(std::make_unique<MWoubenna>());

            //define and add  CS markers
            codeSmoother.addScMarker(std::make_unique<LICSDetector>());
            codeSmoother.addScMarker(std::make_unique<MWCSDetector>());
        }
    }




}