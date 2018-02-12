
#pragma region gpstk includes
#include"SP3EphemerisStore.hpp"
#include"XYZ2NEU.hpp"
#include"RequireObservables.hpp"
#include"SimpleFilter.hpp"
#include "LICSDetector2.hpp"
#include "MWCSDetector.hpp"
#include "SolidTides.hpp"
#include "OceanLoading.hpp"
#include "PoleTides.hpp"
#include "CorrectObservables.hpp"
#include "ComputeWindUp.hpp"
#include "ComputeSatPCenter.hpp"
#include "ComputeTropModel.hpp"
#include "ComputeLinear.hpp"
#include "LinearCombinations.hpp"
#include "ComputeDOP.hpp"
#include "SatArcMarker.hpp"
#include "GravitationalDelay.hpp"
#include "PhaseCodeAlignment.hpp"
#include "PowerSum.hpp"
#include "EclipsedSatFilter.hpp"
#include "Decimate.hpp"
#include"ProcessingList.hpp"
#include"BasicModel.hpp"

#pragma endregion

//pod includes
#include "SolverPPP.hpp"
#include"SolverPPPFB.hpp"
#include "PPPSolution.h"
#include"CodeSolver.h"
#include"GnssEpochMap.h"
#include"LinearCombination.h"
#include"ProcessLinear.h"
#include"FsUtils.h"

namespace pod
{
    PPPSolution::PPPSolution(GnssDataStore_sptr gnssData)
        :PPPSolutionBase (gnssData)
    {
        nominalPos.asECEF();
        int i = 0;
        for (auto& it: confReader().getListValueAsDouble("nominalPosition"))
            nominalPos[i++] = it;
     
       opts().DoY = confReader().getValueAsInt("dayOfYear");

        //initialize troposhperic model
        tropModel = NeillTropModel(nominalPos.getAltitude(), nominalPos.getGeodeticLatitude(),opts().DoY);

        solverPR = unique_ptr<CodeSolverBase>(new CodeSolver(tropModel, data));
        fName = "ppp_sln.txt";
    }
    
    bool  PPPSolution::processCore()
    {
        string stationName = confReader().fetchListValue("stationName");

        updateRequaredObs();
        
        // This object will check that code observations are within
        // reasonable limits
        SimpleFilter PRFilter; 
        PRFilter.setFilteredType(TypeID::P2);
        PRFilter.addFilteredType(codeL1);

        SimpleFilter SNRFilter(TypeID::S1, 30, DBL_MAX);

        // IMPORTANT NOTE:
        // It turns out that some receivers don't correct their clocks
        // from drift.
        // When this happens, their code observations may drift well beyond
        // what it is usually expected from a pseudorange. In turn, this
        // effect causes that "SimpleFilter" objects start to reject a lot of
        // satellites.
        // Thence, the "filterCode" option allows you to deactivate the
        // "SimpleFilter" object that filters out C1, P1 and P2, in case you
        // need to.
        //bool filterCode(confReader().getValueAsBoolean("filterCode"));

        // Object to compute linear combinations for cycle slip detection
        ProcessLinear linear1;

        linear1.add(make_unique<PDelta>());
        linear1.add(make_unique<MWoubenna>());

        linear1.add(make_unique<LDelta>());
        linear1.add(make_unique<LICombimnation>());
        
        // Objects to mark cycle slips
        LICSDetector2 markCSLI2;         // Checks LI cycle slips
        markCSLI2.setSatThreshold(confReader().getValueAsDouble("LISatThreshold"));
        MWCSDetector  markCSMW;          // Checks Merbourne-Wubbena cycle slips
        markCSMW.setMaxNumLambdas(confReader().getValueAsDouble("MWNLambdas"));

        // Object to keep track of satellite arcs
        SatArcMarker markArc;
        markArc.setDeleteUnstableSats(true);
        markArc.setUnstablePeriod(31.0);
        
        // Object to decimate data
        Decimate decimateData(
            confReader().getValueAsDouble("decimationInterval"),
            confReader().getValueAsDouble("decimationTolerance"),
            data->SP3EphList.getInitialTime());
     
        // Declare a basic modeler
        //BasicModel basic(Position(0.0, 0.0, 0.0), SP3EphList);
        BasicModel basic(nominalPos, data->SP3EphList);
        // Set the minimum elevation
        basic.setMinElev(opts().maskEl);

        basic.setDefaultObservable(codeL1);

        // Object to remove eclipsed satellites
        EclipsedSatFilter eclipsedSV;

        // Object to compute gravitational delay effects
        GravitationalDelay grDelay(nominalPos);

        // Vector from monument to antenna ARP [UEN], in meters
        Triple offsetARP;
        int i = 0;
        for(auto &it:confReader().getListValueAsDouble("offsetARP"))
            offsetARP[i++] = it;

        AntexReader antexReader;
        Antenna receiverAntenna;

        // Feed Antex reader object with Antex file
        string afile = data->genericFilesDirectory;
        afile += confReader().getValue("antexFile");

        antexReader.open(afile);

        // Get receiver antenna parameters
        receiverAntenna = antexReader.getAntenna(confReader().getValue("antennaModel"));

        // Object to compute satellite antenna phase center effect
        ComputeSatPCenter svPcenter(nominalPos);

        // Feed 'ComputeSatPCenter' object with 'AntexReader' object
        svPcenter.setAntexReader(antexReader);

        // Declare an object to correct observables to monument
        CorrectObservables corr(data->SP3EphList);

        corr.setMonument(offsetARP);

        // Check if we want to use Antex patterns
        bool usepatterns(confReader().getValueAsBoolean("usePCPatterns"));
        if (usepatterns)
        {
            corr.setAntenna(receiverAntenna);
            // Should we use elevation/azimuth patterns or just elevation?
            corr.setUseAzimuth(confReader().getValueAsBoolean("useAzim"));
        }

        // Object to compute wind-up effect
        ComputeWindUp windup(data->SP3EphList, nominalPos, data->genericFilesDirectory + confReader().getValue("satDataFile"));
      
        // Declare a NeillTropModel object, setting its parameters
        NeillTropModel neillTM(nominalPos.getAltitude(), nominalPos.getGeodeticLatitude(), opts().DoY);

        // Object to compute the tropospheric data
        ComputeTropModel computeTropo(neillTM);

        // Object to compute ionosphere-free combinations to be used
        // as observables in the PPP processing
        ProcessLinear linear2;
        linear2.add(make_unique<PCCombimnation>());
        linear2.add(make_unique<LCCombimnation>());

        // Add to processing list
        // Declare a simple filter object to screen PC
        SimpleFilter pcFilter;
        pcFilter.setFilteredType(TypeID::PC);

        // IMPORTANT NOTE:
        // Like in the "filterCode" case, the "filterPC" option allows you to
        // deactivate the "SimpleFilter" object that filters out PC, in case
        // you need to.

        // Object to align phase with code measurements
        PhaseCodeAlignment phaseAlign;

        LinearCombinations comb;
        // Object to compute prefit-residuals
        ComputeLinear linear3(comb.pcPrefit);
        linear3.addLinear(comb.lcPrefit);

        // Declare a base-changing object: From ECEF to North-East-Up (NEU)
        XYZ2NEU baseChange(nominalPos);

        // Object to compute DOP values
        ComputeDOP cDOP;

        double tropoQ(confReader().getValueAsDouble("tropoQ"));
        double posSigma(confReader().getValueAsDouble("posSigma"));
        double clkSigma(confReader().getValueAsDouble("clkSigma"));
        double weightFactor(confReader().getValueAsDouble("weightFactor"));
       
        //estimate receiver linear clock drift parameters together with  clock offset?
        bool useAdvClkModel = confReader().getValueAsBoolean("useAdvClkModel");
        basic.useClkDrift(useAdvClkModel);
        
        // Declare solver objects
        SolverPPP   pppSolver (useAdvClkModel,tropoQ, posSigma, clkSigma, weightFactor);
        SolverPPPFB fbpppSolver(useAdvClkModel, tropoQ, posSigma, clkSigma, weightFactor);
       
        list<double> phaselims = confReader().getListValueAsDouble("phaseLimlist");
        fbpppSolver.setPhaseList(phaselims);
        list<double> codelims = confReader().getListValueAsDouble("codeLimList");
        fbpppSolver.setCodeList(codelims);
        int cycles(std::max<int>(phaselims.size(), codelims.size()));
        cout <<"cycles "<< cycles << endl;

        if (useAdvClkModel)
        {
            double q1clk(confReader().getValueAsDouble("q1Clk"));
            double q2clk(confReader().getValueAsDouble("q2Clk"));
            AdvClockModel mod(q1clk, q2clk);
            pppSolver.setAdvClkModel(mod);
            fbpppSolver.setAdvClkModel(mod);
        }

        // White noise stochastic models
        WhiteNoiseModel wnM(100.0);

        if (opts().dynamics == GnssDataStore:: Dynamics::Kinematic)
        {
            fbpppSolver.setCoordinatesModel(&wnM);
            pppSolver.setCoordinatesModel(&wnM);
        }

           // Objects to compute tidal effects
        SolidTides solid;

        // Configure ocean loading model
        OceanLoading ocean;
        ocean.setFilename(data->genericFilesDirectory + confReader().getValue("oceanLoadingFile"));

        // This is the GNSS data structure that will hold all the
        // GNSS-related information
        gnssRinex gRin;

        #pragma region Output streams

        // Prepare for printing
        int prec(4);

        ofstream outfile;
        outfile.open(data->workingDir + "\\" + fileName(), ios::out);

        #pragma endregion

        i = 1;
        cout << "First forward processing part started." << endl;
        for (auto& obsFile : data->rinexObsFiles)
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

            //set def. interval for basic model object
            basic.setDefaultInterval(roh.interval);

            // Let's process all lines of observation data, one by one
            while (rin >> gRin)
            {
                //work around for post header comments
                if (gRin.body.size() == 0) continue;
                //
                gRin.keepOnlySatSyst(opts().systems);

                /// update current time and nominal position
                CommonTime time(gRin.header.epoch);
                updateNomPos(gRin.header.epoch, nominalPos);

                /// compute pole tide displacment
                auto eop = data->eopStore.getEOP(MJD(time).mjd, IERSConvention::IERS2010);
                PoleTides pole;
                pole.setXY(eop.xp, eop.yp);

                basic.rxPos = nominalPos;
                grDelay.setNominalPosition(nominalPos);
                svPcenter.setNominalPosition(nominalPos);
                windup.setNominalPosition(nominalPos);
                XYZ2NEU baseChange(nominalPos);

                // Compute solid, oceanic and pole tides effects at this epoch
                Triple tides(solid.getSolidTide(time, nominalPos) + ocean.getOceanLoading(stationName, time) + pole.getPoleTide(time, nominalPos));

                // Update observable correction object with tides information
                corr.setExtraBiases(tides);
                corr.setNominalPosition(nominalPos);

                try
                {
                    gRin >> requireObs;
                    gRin >> PRFilter;
                    gRin >> SNRFilter;
                    gRin >> linear1;
                    gRin >> markCSLI2;
                    gRin >> markCSMW;
                    gRin >> markArc;
                    gRin >> decimateData;
                    gRin >> basic;
                    gRin >> eclipsedSV;
                    gRin >> grDelay;
                    gRin >> svPcenter;
                    gRin >> corr;
                    gRin >> windup;
                    gRin >> computeTropo;
                    gRin >> linear2;
                    gRin >> pcFilter;
                    // gRin >> phaseAlign;
                    gRin >> linear3;
                    gRin >> baseChange;
                    gRin >> cDOP;

                    if (cycles < 1)
                        gRin >> pppSolver;
                    else
                        gRin >> fbpppSolver;
                }
                catch (DecimateEpoch& d)
                {
                    // If we catch a DecimateEpoch exception, just continue.
                    continue;
                }
                ++i;

                // Check what type of solver we are using
                if (cycles < 1)
                {
                    GnssEpoch ep(gRin);
                    CommonTime time(gRin.header.epoch);

                    // Let's print to output file the results of this epoch
                    printSolution(outfile, pppSolver, time,  ep);

                    //add epoch to results
                    gMap.data.insert(pair<CommonTime, GnssEpoch>(time, ep));
                } 
            } 

            rin.close();
        }
        cout << "First forward processing part finished." << endl;

        // Now decide what to do: If solver was a 'forwards-only' version,
        // then we are done and should continue with next station.
        if (cycles < 1)
        {
            outfile.close();
            // We are done with this station. Let's show a message
            cout << "Processing finished for station: '" << stationName <<"'." << endl;

            return true;
        }

        // Now, let's do 'forwards-backwards' cycles
        try
        {
            cout << "fw-bw bart begin..." << endl;
            fbpppSolver.ReProcess();
        }
        catch (Exception& e)
        {
            // If problems arose, issue an message and skip receiver
            cerr << "Exception at reprocessing phase: " << e << endl;
            cerr << "Station '" << stationName << "'." << endl;

            // Close output file for this station
            outfile.close();

            GPSTK_RETHROW(e);

        }  // End of 'try-catch' block
    
           // Reprocess is over. Let's finish with the last processing		
           // Loop over all data epochs, again, and print results
        while (fbpppSolver.LastProcess(gRin))
        {
            // update current time and nominal position
            GnssEpoch ep(gRin);
            updateNomPos(gRin.header.epoch,nominalPos);
            printSolution(outfile, fbpppSolver, gRin.header.epoch, ep);
            gMap.data.insert(pair<CommonTime, GnssEpoch>(gRin.header.epoch, ep));

        }  // End of 'while( fbpppSolver.LastProcess(gRin) )'

        cout << "Processing finished for station: '" << stationName << "'." << endl;
        cout << "Num. of rejected meas. " << fbpppSolver.getRejectedMeasurements() << endl;
       
        outfile.close();

        return true;
    }

    void PPPSolution::updateRequaredObs()
    {
        codeL1 = confReader().getValueAsBoolean("useC1") ? TypeID::C1 : TypeID::P1;

        requireObs.addRequiredType(codeL1);
        requireObs.addRequiredType(TypeID::P2);
        requireObs.addRequiredType(TypeID::L1);
        requireObs.addRequiredType(TypeID::L2);
        requireObs.addRequiredType(TypeID::S1);
    }

    void PPPSolution::printSolution(ofstream& outfile, const SolverLMS& solver, const CommonTime& time, GnssEpoch& gEpoch)
    {
        // Prepare for printing
        outfile << fixed << setprecision(outputPrec);

        // Print results
        outfile << static_cast<YDSTime>(time).year << "-";   // Year           - #1
        outfile << static_cast<YDSTime>(time).doy << "-";    // DayOfYear      - #2
        outfile << static_cast<YDSTime>(time).sod << "  ";   // SecondsOfDay   - #3
        outfile << setprecision(6) << (static_cast<YDSTime>(time).doy + static_cast<YDSTime>(time).sod / 86400.0) << "  " << setprecision(outputPrec);

        // We add 0.1 meters to 'wetMap' because 'NeillTropModel' sets a
        // nominal value of 0.1 m. Also to get the total we have to add the
        // dry tropospheric delay value
        // ztd - #7
        double wetMap = solver.getSolution(TypeID::wetMap) + 0.1 + this->tropModel.dry_zenith_delay();

        gEpoch.slnData.insert(pair<TypeID, double>(TypeID::recZTropo, wetMap));


        double x = nominalPos.X() + solver.getSolution(TypeID::dx);    // dx    - #4
        double y = nominalPos.Y() + solver.getSolution(TypeID::dy);    // dy    - #5
        double z = nominalPos.Z() + solver.getSolution(TypeID::dz);    // dz    - #6

        gEpoch.slnData.insert(pair<TypeID, double>(TypeID::recX, x));
        gEpoch.slnData.insert(pair<TypeID, double>(TypeID::recY, y));
        gEpoch.slnData.insert(pair<TypeID, double>(TypeID::recZ, z));

        double varX = solver.getVariance(TypeID::dx);     // Cov dx    - #8
        double varY = solver.getVariance(TypeID::dy);     // Cov dy    - #9
        double varZ = solver.getVariance(TypeID::dz);     // Cov dz    - #10
        double sigma = sqrt(varX + varY + varZ);

        double cdt = solver.getSolution(TypeID::cdt);
        gEpoch.slnData.insert(pair<TypeID, double>(TypeID::recCdt, cdt));

        //
        outfile << x << "  " << y << "  " << z << "  " << cdt << " ";

        auto defeq = solver.getDefaultEqDefinition();

        auto itcdtGLO = defeq.body.find(TypeID::recCdtGLO);
        if (defeq.body.find(TypeID::recCdtGLO) != defeq.body.end())
        {
            double cdtGLO = solver.getSolution(TypeID::recCdtGLO);
            gEpoch.slnData.insert(pair<TypeID, double>(TypeID::recCdtGLO, cdtGLO));

            outfile << cdtGLO << " ";
        }

        if (defeq.body.find(TypeID::recCdtdot) != defeq.body.end())
        {
            double recCdtdot = solver.getSolution(TypeID::recCdtdot);
            gEpoch.slnData.insert(pair<TypeID, double>(TypeID::recCdtdot, recCdtdot));

            outfile << setprecision(12) << recCdtdot << " ";
        }

        gEpoch.slnData.insert(pair<TypeID, double>(TypeID::sigma, sigma));
        outfile << setprecision(6) << wetMap << "  " << sigma << "  ";

        gEpoch.slnData.insert(pair<TypeID, double>(TypeID::recSlnType, 16));

        outfile << gEpoch.satData.size() << endl;
    }
}