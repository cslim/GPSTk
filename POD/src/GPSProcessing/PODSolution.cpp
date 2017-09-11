#include"PODSolution.h"
#include"PPPSolverLEO.h"
#include"PPPSolverLEOFwBw.h"

#include"XYZ2NEU.hpp"
#include"RequireObservables.hpp"
//
#include"SimpleFilter.hpp"
// Class to detect cycle slips using LI combination
#include "LICSDetector2.hpp"

// Class to detect cycle slips using the Melbourne-Wubbena combination
#include "MWCSDetector.hpp"

// Class to compute the effect of solid tides
#include "SolidTides.hpp"

// Class to compute the effect of ocean loading
#include "OceanLoading.hpp"

// Class to compute the effect of pole tides
#include "PoleTides.hpp"

// Class to correct observables
#include "CorrectObservables.hpp"

// Class to compute the effect of wind-up
#include "ComputeWindUp.hpp"

// Class to compute the effect of satellite antenna phase center
#include "ComputeSatPCenter.hpp"

// Class to compute the tropospheric data
#include "ComputeTropModel.hpp"

// Class to compute linear combinations
#include "ComputeLinear.hpp"

// This class pre-defines several handy linear combinations
#include "LinearCombinations.hpp"

// Class to compute Dilution Of Precision values
#include "ComputeDOP.hpp"

// Class to keep track of satellite arcs
#include "SatArcMarker.hpp"

// Class to compute gravitational delays
#include "GravitationalDelay.hpp"

// Class to align phases with code measurements
#include "PhaseCodeAlignment.hpp"

// Compute statistical data
#include "PowerSum.hpp"

// Used to delete satellites in eclipse
#include "EclipsedSatFilter.hpp"

// Used to decimate data. This is important because RINEX observation
// data is provided with a 30 s sample rate, whereas SP3 files provide
// satellite clock information with a 900 s sample rate.
#include "Decimate.hpp"

#include"BasicModel.hpp"
using namespace gpstk;
namespace pod
{
    PODSolution::PODSolution(ConfDataReader & confReader, const string& dir):
        PPPSolutionBase (confReader,dir)
    {
        solverPR = new CodeSolverLEO();
    }

    bool PODSolution::PPPprocess()
    {
        int outInt(confReader->getValueAsInt("outputInterval"));

        // This object will check that all required observables are present
        RequireObservables requireObs;
        requireObs.addRequiredType(TypeID::P1);
        requireObs.addRequiredType(TypeID::P2);
        requireObs.addRequiredType(TypeID::L1);
        requireObs.addRequiredType(TypeID::L2);
        requireObs.addRequiredType(TypeID::S1);

        // This object will check that code observations are within
        // reasonable limits
        SimpleFilter PRFilter;
        PRFilter.addFilteredType(TypeID::P1);
        PRFilter.setFilteredType(TypeID::P2);

        SimpleFilter SNRFilter(TypeID::S1, (double)maskSNR, 1e7);

        // This object defines several handy linear combinations
        LinearCombinations comb;

        // Object to compute linear combinations for cycle slip detection
        ComputeLinear linear1;

        linear1.addLinear(comb.pdeltaCombination);
        linear1.addLinear(comb.mwubbenaCombination);

        linear1.addLinear(comb.ldeltaCombination);
        linear1.addLinear(comb.liCombination);

        // Objects to mark cycle slips
        LICSDetector2 markCSLI2;         // Checks LI cycle slips
                                         // markCSLI2.setSatThreshold(1);
       // Checks Merbourne-Wubbena cycle slips
        MWCSDetector  markCSMW(confReader->getValueAsDouble("MWNumLambdas"));          

        // Object to keep track of satellite arcs
        SatArcMarker markArc;
        markArc.setDeleteUnstableSats(true);
        markArc.setUnstablePeriod(61.0);

        // Object to decimate data
        double newSampling(confReader->getValueAsDouble("decimationInterval"));

        Decimate decimateData(
            newSampling,
            confReader->getValueAsDouble("decimationTolerance"),
            SP3EphList.getInitialTime());

        // Declare a basic modeler
        //BasicModel basic(Position(0.0, 0.0, 0.0), SP3EphList);
        BasicModel basic(nominalPos, SP3EphList);
        // Set the minimum elevation
        basic.setMinElev(maskEl);

        basic.setDefaultObservable(TypeID::P1);

        // Object to remove eclipsed satellites
        EclipsedSatFilter eclipsedSV;

        // Object to compute gravitational delay effects
        GravitationalDelay grDelay(nominalPos);

        // Vector from monument to antenna ARP [UEN], in meters
        double uARP(confReader->fetchListValueAsDouble("offsetARP"));
        double eARP(confReader->fetchListValueAsDouble("offsetARP"));
        double nARP(confReader->fetchListValueAsDouble("offsetARP"));
        Triple offsetARP(uARP, eARP, nARP);

        // Declare an object to correct observables to monument
        CorrectObservables corr(SP3EphList);
        corr.setMonument(offsetARP);

        // Feed Antex reader object with Antex file
        AntexReader antexReader;
        string afile = genFilesDir;
        afile += confReader->getValue("antexFile");
        antexReader.open(afile);

        // Object to compute satellite antenna phase center effect
        ComputeSatPCenter svPcenter(nominalPos);
        // Feed 'ComputeSatPCenter' object with 'AntexReader' object
        svPcenter.setAntexReader(antexReader);

#pragma region receiver antenna parameters

        bool useAntex(confReader->fetchListValueAsBoolean("useRcvAntennaModel"));
        Triple ofstL1(0.0, 0.0, 0.0), ofstL2(0.0, 0.0, 0.0);
        if (useAntex)
        {
            Antenna receiverAntenna;
            // Get receiver antenna parameters
            string aModel(confReader->getValue("antennaModel"));
            receiverAntenna = antexReader.getAntenna(aModel);

            // Check if we want to use Antex patterns
            bool usepatterns(confReader->getValueAsBoolean("usePCPatterns"));
            if (usepatterns)
            {
                corr.setAntenna(receiverAntenna);
                // Should we use elevation/azimuth patterns or just elevation?
                corr.setUseAzimuth(confReader->getValueAsBoolean("useAzim"));
            }
        }
        else
        {
            // Fill vector from antenna ARP to L1 phase center [UEN], in meters
            ofstL1[0] = confReader->fetchListValueAsDouble("offsetL1");
            ofstL1[1] = confReader->fetchListValueAsDouble("offsetL1");
            ofstL1[2] = confReader->fetchListValueAsDouble("offsetL1");

            // Vector from antenna ARP to L2 phase center [UEN], in meters
            ofstL2[0] = confReader->fetchListValueAsDouble("offsetL2");
            ofstL2[1] = confReader->fetchListValueAsDouble("offsetL2");
            ofstL2[2] = confReader->fetchListValueAsDouble("offsetL2");
            //
            corr.setL1pc(ofstL1);
            corr.setL2pc(ofstL2);
        }

#pragma endregion

        // Object to compute wind-up effect
        ComputeWindUp windup(SP3EphList, nominalPos, genFilesDir + confReader->getValue("satDataFile"));

        // Object to compute ionosphere-free combinations to be used
        // as observables in the PPP processing
        ComputeLinear linear2;
        linear2.addLinear(comb.pcCombination);
        linear2.addLinear(comb.lcCombination);

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

        // Object to compute prefit-residuals
        ComputeLinear linear3(comb.pcPrefit);
        linear3.addLinear(comb.lcPrefit);

        // Object to compute DOP values
        ComputeDOP cDOP;

        // White noise stochastic model
        WhiteNoiseModel wnM(1000.0);      // 100 m of sigma
                                          // Declare solver objects
        PPPSolverLEO   pppSolver(false);
        pppSolver.setCoordinatesModel(&wnM);
        PPPSolverLEOFwBw fbpppSolver(false);
        fbpppSolver.setCoordinatesModel(&wnM);

        // Get if we want 'forwards-backwards' or 'forwards' processing only
        int cycles(confReader->getValueAsInt("forwardBackwardCycles"));

        list<double> phaseLimits, codeLimits;
        double val(0);
        while ((val = confReader->fetchListValueAsDouble("codeLimits")) != 0.0)
            codeLimits.push_back(val);
        while ((val = confReader->fetchListValueAsDouble("phaseLimits")) != 0.0)
            phaseLimits.push_back(val);

        fbpppSolver.setPhaseList(phaseLimits);
        fbpppSolver.setCodeList(codeLimits);

        // This is the GNSS data structure that will hold all the
        // GNSS-related information
        gnssRinex gRin;

#pragma region Output streams

        // Prepare for printing
        int precision(4);

        ofstream outfile;
        outfile.open(workingDir +"\\PPP_sol.out", ios::out);

#pragma endregion

        //statistics for coorinates and tropo delay
        vector<PowerSum> stats(4);
        CommonTime time0;
        bool b = true;

        //// *** Now comes the REAL forwards processing part *** ////
        for (auto obsFile : rinexObsFiles)
        {
            cout << obsFile << endl;
            //Input observation file stream
            Rinex3ObsStream rin;
            // Open Rinex observations file in read-only mode
            rin.open(obsFile, std::ios::in);

            rin.exceptions(ios::failbit);
            Rinex3ObsHeader roh;
            Rinex3ObsData rod;

            auto it = apprPos.begin();
            //read the header
            rin >> roh;
            gMap.header = roh;

            // Loop over all data epochs
            while (rin >> gRin)
            {
                PPPSolutionBase::mapSNR(gRin);

                // Store current epoch
                CommonTime time(gRin.header.epoch);
//#ifdef DBG
//                nominalPos = it->second;
//                it++;
//#else
                nominalPos = apprPos.at(time);
//#endif // DEBUG

                ///update the nominal position in processing objects
                XYZ2NEU baseChange(nominalPos);
                basic.rxPos = nominalPos;
                grDelay.setNominalPosition(nominalPos);
                svPcenter.setNominalPosition(nominalPos);
                windup.setNominalPosition(nominalPos);
                corr.setNominalPosition(nominalPos);

                int csnum(0);
                try
                {
                    //  cout <<(YDSTime)time<< " "<< gRin.numSats();
                    gRin >> requireObs
                        >> PRFilter
                        >> SNRFilter
                        >> linear1;
                    //  gRin >> markCSLI2;

                    gRin >> markCSMW;
                    //csnum = getNumCS(gRin);

                    gRin >> markArc;
                    //cout  <<" "<<csnum<<  " " << gRin.numSats();
                    gRin >> decimateData
                        >> basic
                        >> eclipsedSV
                        >> grDelay
                        >> svPcenter;
                    //cout <<  " " << gRin.numSats() << " ";
                    gRin >> requireObs
                        >> corr
                        >> windup
                        >> linear2
                        >> pcFilter
                        >> phaseAlign
                        >> linear3
                        >> baseChange
                        >> cDOP;

                    if (cycles < 1)
                    {
                        gRin >> pppSolver;
                    }
                    else
                        gRin >> fbpppSolver;
                    
                    //   cout /*<<  " " << gRin.numSats()*/ << endl;
                }
                catch (DecimateEpoch& d)
                {
                    continue;
                }
                catch (Exception& e)
                {
                    cout << e.getText() << endl;
                    return false;
                }
                catch (...)
                {
                    return false;
                }


                // Check what type of solver we are using
                if (cycles < 1)
                {
                    GnssEpoch ep(gRin);
                    CommonTime time(gRin.header.epoch);
                    if (b)
                    {
                        time0 = time;

                        b = false;
                    }

                    // This is a 'forwards-only' filter. Let's print to output
                    // file the results of this epoch
                    double fm = fmod(((GPSWeekSecond)time).getSOW(), outInt);

                    if (fm < 0.1)
                        pppSolver.printSolution(outfile, time0, time, cDOP, ep,  0.0, stats, nominalPos);
                    gMap.data.insert(pair<CommonTime, GnssEpoch>(time, ep));
                }  // End of 'if ( cycles < 1 )'
             
            }  // End of 'while(rin >> gRin)'

            rin.close();
        }

        //// *** Forwards processing part is over *** ////

        // Now decide what to do: If solver was a 'forwards-only' version,
        // then we are done and should continue with next station.
        if (cycles < 1)
        {

            // Close output file for this station
            outfile.close();

            return true;
        }

        //// *** If we got here, it is a 'forwards-backwards' solver *** ////
        int i_c = 0;
        // Now, let's do 'forwards-backwards' cycles
        try
        {
            cout << "cycle # " << ++i_c;
            if (codeLimits.size() > 0 || phaseLimits.size() > 0)
                fbpppSolver.ReProcess();
            else
                fbpppSolver.ReProcess(cycles);

            cout << " rej. meas: " << fbpppSolver.getRejectedMeasurements() << endl;
        }
        catch (Exception& e)
        {
            // Close output file for this station
            outfile.close();

            return false;
        }

        // Reprocess is over. Let's finish with the last processing		
        // Loop over all data epochs, again, and print results
    
        while (fbpppSolver.LastProcess(gRin))
        {
            GnssEpoch ep(gRin);
            CommonTime time(gRin.header.epoch);

            if (b)
            {
                time0 = time;
                b = false;
            }
                    
            nominalPos = apprPos.at(time);
            double fm = fmod(((GPSWeekSecond)time).getSOW(), outInt);
            if (fm < 0.1)
                fbpppSolver.printSolution(outfile, time0, time, cDOP, ep, 0.0, stats, nominalPos);
            //add epoch to results
            gMap.data.insert(pair<CommonTime, GnssEpoch>(time, ep));
        }  // End of 'while( fbpppSolver.LastProcess(gRin) )'

           //print statistic
        printStats(outfile, stats);

        // We are done. Close and go for next station

        // Close output file for this station
        outfile.close();
    }




    double PODSolution::mapSNR(double value)
    {
       return 20.0*log10(value);
    }
}