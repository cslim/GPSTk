#include "SingleSolution.h"

#include"SimpleFilter.hpp"
#include"WinUtils.h"
#include"ComputeWeightSimple.h"

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

#include<memory>
using namespace std;
using namespace gpstk;

namespace pod
{
    const double  SingleSolution::MAX_SIGMA = 50;

    SingleSolution::SingleSolution(GnssDataStore_sptr data_ptr)
        : GnssSolution(data_ptr), codeSmWindowSize(600)
    {
        fName = "auto_sln.txt";
    }

    SingleSolution::~SingleSolution()
    {
    }

    //
    void SingleSolution::process()
    {
        updateRequaredObs();

        SimpleFilter PRFilter;

        PRFilter.addFilteredType(codeL1);
        if (data->ionoCorrector.getType() == ComputeIonoModel::DualFreq)
            PRFilter.setFilteredType(TypeID::P2);

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
        model.setMinElev(5);

        //troposhere modeling object
        unique_ptr<NeillTropModel> uptrTropModel = make_unique<NeillTropModel>();
        ComputeTropModel computeTropo(*uptrTropModel);

        //
        ComputeWeightSimple w;

        // White noise stochastic models
        WhiteNoiseModel wnM(confReader().getValueAsDouble("posSigma"));

        if (opts().dynamics == GnssDataStore::Dynamics::Kinematic)
        {
            solverFB.setCoordinatesModel(&wnM);
            solver.setCoordinatesModel(&wnM);
        }

        ofstream ostream;
        ostream.open(data->workingDir + "\\" + fileName(), ios::out);

        bool firstTime = true;

        gnssRinex gRin;

        for (auto &obsFile : data->rinexObsFiles)
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
            if (opts().isSmoothCode)
                for (auto& it : codeSmoothers)
                    it.setMaxWindowSize(codeSmWindowSize / roh.interval);

            //read all epochs
            while (rin >> gRin)
            {
                //work around for post header comments 
                if (gRin.body.size() == 0  )
                {
                    printMsg(gRin.header.epoch, "Empty epoch record in Rinex file");
                    continue;
                }

                if (decimateData.check(gRin))
                    continue;

                auto& t = gRin.header.epoch;

                //keep only satellites from satellites systems selecyted for processing
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

                //update position 
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
                {
                    //first, mark cycle slips
                    gRin >> *csMarker;

                    //then, smooth pseudoranges
                    for (auto& it : codeSmoothers)
                        gRin >> it;
                }
                if (gRin.body.size() == 0)
                {
                    printMsg(gRin.header.epoch, "All SV has been rejected.");
                    continue;
                }
                gRin >> model;
                gRin >> computeTropo;
                gRin >> data->ionoCorrector;
                gRin >> linear;
                gRin >> w;

                if (forwardBackwardCycles > 0)
                {
                    gRin >> solverFB;
                    //updateNomPos(solverFB);
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
            solverFB.reProcess();
            gnssRinex gRin;
            cout << "Last process part started" << endl;
            while (solverFB.lastProcess(gRin))
            {
                GnssEpoch ep(gRin);
                //updateNomPos(solverFB);
                printSolution(ostream, solverFB, gRin.header.epoch, ep);
                gMap.data.insert(std::make_pair(gRin.header.epoch, ep));
            }
        }
    }

    void SingleSolution::updateNomPos(CodeKalmanSolver &solver)
    {
        PowerSum psum;
        for (auto it : solver.postfitResiduals)
            psum.add(it);
        double sigma = sqrt(psum.variance());

        int numSats = solver.postfitResiduals.size();
        Position newPos;
        if (numSats >= 4 && sigma < MAX_SIGMA)
        {
            newPos[0] = nominalPos.X() + solver.getSolution(TypeID::dx);    // dx    - #4
            newPos[1] = nominalPos.Y() + solver.getSolution(TypeID::dy);    // dy    - #5
            newPos[2] = nominalPos.Z() + solver.getSolution(TypeID::dz);    // dz    - #6

            nominalPos = newPos;
        }
    }

    void SingleSolution::updateRequaredObs()
    {
     
        LinearCombinations comm;
        bool useC1 = confReader().getValueAsBoolean("useC1");
        forwardBackwardCycles =  confReader().getValueAsInt("forwardBackwardCycles");
        if (forwardBackwardCycles > 0)
            solverFB.setCodeLims(confReader().getListValueAsDouble("codeLimList"));

        bool codeLimits = confReader().getValueAsBoolean("useC1");
        auto defeq = solver.getDefaultEqDefinition();
      
        if (useC1)
        {
            codeL1 = TypeID::C1;
            linear.addLinear(comm.c1Prefit);
            defeq.header = TypeID::prefitC;
        }
        else
        {
            codeL1 = TypeID::P1;
            defeq.header = TypeID::prefitP1;
            linear.addLinear(comm.p1Prefit);
        }

        solver.setDefaultEqDefinition(defeq);
        solverFB.setDefaultEqDefinition(defeq);
        requireObs.addRequiredType(codeL1);

        if (data->ionoCorrector.getType() == IonoModelType::DualFreq && opts().isSmoothCode)
        {
            requireObs.addRequiredType(TypeID::P2);
            requireObs.addRequiredType(TypeID::L1);
            requireObs.addRequiredType(TypeID::L2);
            requireObs.addRequiredType(TypeID::LLI1);
            requireObs.addRequiredType(TypeID::LLI2);

            codeSmoothers.push_back(CodeSmoother(codeL1));
            codeSmoothers.push_back(CodeSmoother(TypeID::P2));

            csMarker = std::make_unique<LICSDetector>();

            computeLinear.add(std::make_unique<LICombimnation>());
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

            codeSmoothers.push_back(CodeSmoother(codeL1));

            csMarker = std::make_unique<OneFreqCSDetector>();
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
        os << setprecision(6);
        os << CivilTime(time).printf("%02Y %02m %02d %02H %02M %02S %P") << " " << 0 << " ";
        PowerSum psum;
        for (auto it : solver.postfitResiduals)
            psum.add(it);
        double sigma = sqrt(psum.variance());

        int numSats = gEpoch.satData.size();
        Position newPos;
        double cdt = 0;
        if (numSats >= 4 && sigma < MAX_SIGMA)
        {
            newPos[0] = nominalPos.X() + solver.getSolution(TypeID::dx);    // dx    - #4
            newPos[1] = nominalPos.Y() + solver.getSolution(TypeID::dy);    // dy    - #5
            newPos[2] = nominalPos.Z() + solver.getSolution(TypeID::dz);    // dz    - #6

            gEpoch.slnData.insert(pair<TypeID, double>(TypeID::recX, newPos.X()));
            gEpoch.slnData.insert(pair<TypeID, double>(TypeID::recY, newPos.Y()));
            gEpoch.slnData.insert(pair<TypeID, double>(TypeID::recZ, newPos.Z()));

            cdt = solver.getSolution(TypeID::cdt);
            gEpoch.slnData.insert(pair<TypeID, double>(TypeID::recCdt, cdt));

        }

        os << setprecision(10) << nominalPos.X() << "  " << nominalPos.Y() << "  " << nominalPos.Z() << "  " << cdt << " ";

        double varX = solver.getVariance(TypeID::dx);     // Cov dx    - #8
        double varY = solver.getVariance(TypeID::dy);     // Cov dy    - #9
        double varZ = solver.getVariance(TypeID::dz);     // Cov dz    - #10
        double stDev3D = sqrt(varX + varY + varZ);
        gEpoch.slnData.insert(pair<TypeID, double>(TypeID::recStDev3D, stDev3D));

        gEpoch.slnData.insert(pair<TypeID, double>(TypeID::sigma, sigma));

        auto defeq = solver.getDefaultEqDefinition();

        auto itcdtGLO = defeq.body.find(TypeID::recCdtGLO);
        if (defeq.body.find(TypeID::recCdtGLO) != defeq.body.end())
        {
            double cdtGLO = solver.getSolution(TypeID::recCdtGLO);
            gEpoch.slnData.insert(pair<TypeID, double>(TypeID::recCdtGLO, cdtGLO));

            os << cdtGLO << " ";
        }
        //

        gEpoch.slnData.insert(make_pair(TypeID::recSlnType, (sigma < MAX_SIGMA && numSats >= 4) ? 1 : 0));

        // precision 
        os << setprecision(3);
        os << sigma << " " << stDev3D << " " << numSats;
        os << endl;
    };
}
