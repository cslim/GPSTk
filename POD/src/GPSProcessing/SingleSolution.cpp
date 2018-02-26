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
#include"PRSolution2.hpp"
#include"Bancroft.hpp"
#include"MWCSDetector.hpp"


#include<memory>

using namespace std;
using namespace gpstk;

namespace pod
{
    const double  SingleSolution::MAX_SIGMA_SINGLE = 50;

    SingleSolution::SingleSolution(GnssDataStore_sptr data_ptr): 
        GnssSolution(data_ptr), 
        codeSmWindowSize(600),
        Equations(make_shared<EquationComposer>())
    {
    }

    //
    void SingleSolution::process()
    {
        updateRequaredObs();

        SimpleFilter PRFilter;

        PRFilter.addFilteredType(codeL1);
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
                if (gRin.body.size() == 0  )
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
                gRin >> linearCombinations;

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
        if (numSats >= 4 && sigma < MAX_SIGMA_SINGLE)
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

        forwardBackwardCycles = confReader().getValueAsInt("forwardBackwardCycles");

    }

    void SingleSolution::updateRequaredObs()
    {
        LinearCombinations comm;
        bool useC1 = confReader().getValueAsBoolean("useC1");
        linearCombinations.setUseC1(useC1);
        
        configureSolver();

        if (useC1)
        {
            codeL1 = TypeID::C1;
            oMinusC.addLinear(comm.c1Prefit);
            Equations->measType() = TypeID::prefitC;
        }
        else
        {
            codeL1 = TypeID::P1;
            Equations->measType() = TypeID::prefitP1;
            oMinusC.addLinear(comm.p1Prefit);
        }

        requireObs.addRequiredType(codeL1);

        if (data->ionoCorrector.getType() == IonoModelType::DualFreq && opts().isSmoothCode)
        {
            requireObs.addRequiredType(TypeID::P2);
            requireObs.addRequiredType(TypeID::L1);
            requireObs.addRequiredType(TypeID::L2);
            requireObs.addRequiredType(TypeID::LLI1);
            requireObs.addRequiredType(TypeID::LLI2);

            codeSmoother.addSmoother(CodeSmoother(codeL1));
            codeSmoother.addSmoother(CodeSmoother(TypeID::P2));

            codeSmoother.addScMarker(make_unique<LICSDetector>());
            codeSmoother.addScMarker(make_unique<MWCSDetector>());

            linearCombinations.add(make_unique<LICombimnation>());
            linearCombinations.add(make_unique<MWoubenna>());
        }
        else if (data->ionoCorrector.getType() == IonoModelType::DualFreq && !opts().isSmoothCode)
        {
            requireObs.addRequiredType(TypeID::P2);
        }
        //
        else if (data->ionoCorrector.getType() != IonoModelType::DualFreq && opts().isSmoothCode)
        {
            requireObs.addRequiredType(TypeID::L1);
            requireObs.addRequiredType(TypeID::LLI1);

            codeSmoother.addSmoother(CodeSmoother(codeL1));
            codeSmoother.addScMarker(make_unique<OneFreqCSDetector>());
        }
        requireObs.addRequiredType(TypeID::C1);
        requireObs.addRequiredType(TypeID::S1);
    }

    int SingleSolution::computeApprPos(
        const gpstk::gnssRinex & gRin,
        const gpstk::XvtStore<gpstk::SatID>& Eph,
        gpstk::Position& pos)
    {
        auto svs = gRin.getVectorOfSatID().toStdVector();
        auto meas = gRin.getVectorOfTypeID(codeL1).toStdVector();
        Matrix<double> svp;
        if (PRSolution2::PrepareAutonomousSolution(gRin.header.epoch, svs, meas, Eph, svp))
            return -1;
       
        Bancroft ban;
        Vector<double> res;
        if (ban.Compute(svp, res))
            return -2;
        pos = Position(res(0), res(1), res(2));

        return 0;
    }

    void SingleSolution::printSolution(std::ofstream& os, const gpstk::SolverLMS& solver, const gpstk::CommonTime& time, GnssEpoch& gEpoch)
    {
        PowerSum psum;
        for (auto it : solver.PostfitResiduals())
            psum.add(it);
        double sigma = sqrt(psum.variance());

        Position newPos;
        newPos[0] = nominalPos.X() + solver.getSolution(TypeID::dx);    // dx    - #4
        newPos[1] = nominalPos.Y() + solver.getSolution(TypeID::dy);    // dy    - #5
        newPos[2] = nominalPos.Z() + solver.getSolution(TypeID::dz);    // dz    - #6

        double cdt = solver.getSolution(TypeID::cdt);

        double varX = solver.getVariance(TypeID::dx);     // Cov dx    - #8
        double varY = solver.getVariance(TypeID::dy);     // Cov dy    - #9
        double varZ = solver.getVariance(TypeID::dz);     // Cov dz    - #10
        double stDev3D = sqrt(varX + varY + varZ);

        int numSats = gEpoch.satData.size();
        int slnType = (sigma < MAX_SIGMA_SINGLE && numSats >= 4) ? 1 : 0;

        gEpoch.slnData.insert(make_pair(TypeID::recSlnType, slnType));

        gEpoch.slnData.insert(make_pair(TypeID::recX, newPos.X()));
        gEpoch.slnData.insert(make_pair(TypeID::recY, newPos.Y()));
        gEpoch.slnData.insert(make_pair(TypeID::recZ, newPos.Z()));

        gEpoch.slnData.insert(make_pair(TypeID::recStDev3D, stDev3D));
        gEpoch.slnData.insert(make_pair(TypeID::recCdt, cdt));
        gEpoch.slnData.insert(make_pair(TypeID::sigma, sigma));

        const auto& types = solver.getDefaultEqDefinition().body;
        if (types.find(TypeID::recISB_GLN) != types.end())
            gEpoch.slnData.insert(make_pair(TypeID::recISB_GLN, solver.getSolution(TypeID::recISB_GLN)));

        os << setprecision(6) << CivilTime(time).printf("%02Y %02m %02d %02H %02M %02S %P") << " "<< slnType <<" ";
        os << setprecision(10) << newPos.X() << "  " << newPos.Y() << "  " << newPos.Z() << "  " << cdt << " ";
        os << setprecision(3) << sigma << " " << stDev3D << " " << numSats;
        os << endl;
    };
}