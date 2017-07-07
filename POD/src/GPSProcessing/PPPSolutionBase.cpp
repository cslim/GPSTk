#include"PPPSolutionBase.h"

#include<list>
#include <direct.h>
#include<windows.h>
#include<regex>

#include"PPPSolution.h"
#include"PODSolution.h"
#include"auxiliary.h"

#include "Rinex3NavHeader.hpp"
#include "Rinex3NavData.hpp"
#include "Rinex3NavStream.hpp"

#define DBG
namespace POD
{
    PPPSolutionBase* PPPSolutionBase::Factory(bool isSpaceborne, ConfDataReader & reader)
    {
        if (isSpaceborne)
            return new PODSolution(reader);
        else
            return new PPPSolution(reader);
    }
    PPPSolutionBase::PPPSolutionBase(ConfDataReader & cReader) : confReader(&cReader)
    {
        maskEl = confReader->fetchListValueAsDouble("ElMask");
        maskSNR = confReader->fetchListValueAsInt("SNRmask");

        cout << "mask El " << maskEl << endl;
        cout << "mask SNR " << (int)maskSNR << endl;
    } 
    PPPSolutionBase:: ~PPPSolutionBase()
    {
        delete solverPR;
    }

    bool  PPPSolutionBase::LoadData()
    {
        try
        {
            //get application dir
            char current_work_dir[_MAX_FNAME];
            _getcwd(current_work_dir, sizeof(current_work_dir));
            string s_dir(current_work_dir);
            //set generic files direcory 
            string subdir = confReader->fetchListValue("GenericFilesDir");
            genFilesDir = s_dir + "\\" + subdir + "\\";

            subdir = confReader->fetchListValue("RinesObsDir");
            auxiliary::getAllFiles(subdir, rinexObsFiles);

            cout << "Ephemeris Loading... ";
            cout << loadEphemeris() << endl;
            cout << "Clocks Loading... ";
            cout << loadClocks() << endl;
            cout << "IonoModel Loading... ";
            cout << loadIono() << endl;

            return true;
        }
        catch (const std::exception&)
        {
            cout << "failed to load input data" << endl;
            return false;
        }
    }
    
    //
    bool PPPSolutionBase::loadEphemeris()
    {
        // Set flags to reject satellites with bad or absent positional
        // values or clocks
        SP3EphList.clear();
        SP3EphList.rejectBadPositions(true);
        SP3EphList.rejectBadClocks(true);

        list<string> files;
        string subdir = confReader->fetchListValue("EphemerisDir");
        auxiliary::getAllFiles(subdir, files);

        for (auto file : files)
        {
            // Try to load each ephemeris file
            try
            {

                SP3EphList.loadFile(file);
            }
            catch (FileMissingException& e)
            {
                // If file doesn't exist, issue a warning
                cerr << "SP3 file '" << file << "' doesn't exist or you don't "
                    << "have permission to read it. Skipping it." << endl;

                return false;
            }
        }
        return true;
    }
    //reading clock
    bool PPPSolutionBase::loadClocks()
    {
        list<string> files;
        string subdir = confReader->fetchListValue("RinexClockDir");
        auxiliary::getAllFiles(subdir, files);

        for (auto file : files)
        {
            // Try to load each ephemeris file
            try
            {
                SP3EphList.loadRinexClockFile(file);
            }
            catch (FileMissingException& e)
            {
                // If file doesn't exist, issue a warning
                cerr << "Rinex clock file '" << file << "' doesn't exist or you don't "
                    << "have permission to read it. Skipping it." << endl;
                return false;
            }
        }//   while ((ClkFile = confReader->fetchListValue("rinexClockFiles", station)) != "")
        return true;
    }

    bool  PPPSolutionBase::loadIono()
    {
        list<string> files;
        string subdir = confReader->fetchListValue("RinexNavFilesDir");
        auxiliary::getAllFiles(subdir, files);

        for (auto file : files)
        {
            try
            {
                IonoModel iMod;
                Rinex3NavStream rNavFile;
                Rinex3NavHeader rNavHeader;

                rNavFile.open(file.c_str(), std::ios::in);
                rNavFile >> rNavHeader;

#pragma region try get the date

                CommonTime refTime = CommonTime::BEGINNING_OF_TIME;
                if (rNavHeader.fileAgency == "AIUB")
                {

                    for (auto it : rNavHeader.commentList)
                    {
                        int doy = -1, yr = -1;
                        std::tr1::cmatch res;
                        std::tr1::regex rxDoY("DAY [0-9]{3}"), rxY(" [0-9]{4}");
                        bool b = std::tr1::regex_search(it.c_str(), res, rxDoY);
                        if (b)
                        {
                            string sDay = res[0];
                            sDay = sDay.substr(sDay.size() - 4, 4);
                            doy = stoi(sDay);
                        }
                        if (std::tr1::regex_search(it.c_str(), res, rxY))
                        {
                            string sDay = res[0];
                            sDay = sDay.substr(sDay.size() - 5, 5);
                            yr = stoi(sDay);
                        }
                        if (doy > 0 && yr > 0)
                        {
                            refTime = YDSTime(yr, doy, 0, TimeSystem::GPS);
                            break;
                        }
                    }
                }
                else
                {
                    long week = rNavHeader.mapTimeCorr["GPUT"].refWeek;
                    if (week > 0)
                    {
                        GPSWeekSecond gpsws = GPSWeekSecond(week, 0);
                        refTime = gpsws.convertToCommonTime();
                    }
                }
#pragma endregion

                if (rNavHeader.valid & Rinex3NavHeader::validIonoCorrGPS)
                {
                    // Extract the Alpha and Beta parameters from the header
                    double* ionAlpha = rNavHeader.mapIonoCorr["GPSA"].param;
                    double* ionBeta = rNavHeader.mapIonoCorr["GPSB"].param;

                    // Feed the ionospheric model with the parameters
                    iMod.setModel(ionAlpha, ionBeta);
                }
                else
                {
                    cerr << "WARNING: Navigation file " << file
                        << " doesn't have valid ionospheric correction parameters." << endl;
                }

                ionoStore.addIonoModel(refTime, iMod);
            }
            catch (...)
            {
                cerr << "Problem opening file " << file << endl;
                cerr << "Maybe it doesn't exist or you don't have proper read "
                    << "permissions." << endl;
                exit(-1);
            }
        }
        return true;
    }
    
    void PPPSolutionBase::checkObservable(const string & path)
    {
        ofstream os("ObsStatisic.out");

        for (auto obsFile : rinexObsFiles)
        {

            //Input observation file stream
            Rinex3ObsStream rin;
            // Open Rinex observations file in read-only mode
            rin.open(obsFile, std::ios::in);

            rin.exceptions(ios::failbit);
            Rinex3ObsHeader roh;
            Rinex3ObsData rod;

            //read the header
            rin >> roh;
            int indexC1 = roh.getObsIndex(L1CCodeID);
            int indexCNoL1 = roh.getObsIndex(L1CNo);
            int indexP1 = roh.getObsIndex(L1PCodeID);
            int indexP2 = roh.getObsIndex(L2CodeID);
            while (rin >> rod)
            {
                if (rod.epochFlag == 0 || rod.epochFlag == 1)  // Begin usable data
                {
                    int NumC1(0), NumP1(0), NumP2(0), NumBadCNo1(0);
                    os << setprecision(12) << (CivilTime)rod.time << " " << rod.numSVs << " ";

                    for (auto it : rod.obs)
                    {
                        double C1 = rod.getObs(it.first, indexC1).data;

                        int CNoL1 = rod.getObs(it.first, indexCNoL1).data;
                        if (CNoL1 > 30) NumBadCNo1++;

                        double P1 = rod.getObs(it.first, indexP1).data;
                        if (P1 > 0.0)  NumP1++;

                        double P2 = rod.getObs(it.first, indexP2).data;
                        if (P2 > 0.0)  NumP2++;
                    }

                    os << NumBadCNo1 << " " << NumP1 << " " << NumP2 << endl;
                }
            }

        }
    }

    //
    void PPPSolutionBase::process()
    {
#ifndef DBG
        cout << "Approximate Positions loading... ";
        cout << loadApprPos("nomPos.in") << endl;
#else
        PRProcess();
#endif
        PPPprocess();
    }

    // Method to print solution values
    void PPPSolutionBase::printSolution(ofstream& outfile,
        const SolverLMS& solver,
        const CommonTime& time0,
        const CommonTime& time,
        const ComputeDOP& cDOP,
        bool  useNEU,
        int   numSats,
        double dryTropo,
        vector<PowerSum> &stats,
        int   precision,
        const Position &nomXYZ)
    {

        // Prepare for printing
        outfile << fixed << setprecision(precision);

        // Print results
        outfile << static_cast<YDSTime>(time).year << "-";   // Year           - #1
        outfile << static_cast<YDSTime>(time).doy << "-";    // DayOfYear      - #2
        outfile << static_cast<YDSTime>(time).sod << "  ";   // SecondsOfDay   - #3
        outfile << setprecision(6) << (static_cast<YDSTime>(time).doy + static_cast<YDSTime>(time).sod / 86400.0) << "  " << setprecision(precision);

        //calculate statistic
        double x(0), y(0), z(0), varX(0), varY(0), varZ(0);

        // We add 0.1 meters to 'wetMap' because 'NeillTropModel' sets a
        // nominal value of 0.1 m. Also to get the total we have to add the
        // dry tropospheric delay value
        // ztd - #7
        double wetMap = solver.getSolution(TypeID::wetMap) + 0.1 + dryTropo;

        if (useNEU)
        {
            x = solver.getSolution(TypeID::dLat);      // dLat  - #4
            y = solver.getSolution(TypeID::dLon);      // dLon  - #5
            z = solver.getSolution(TypeID::dH);        // dH    - #6

            varX = solver.getVariance(TypeID::dLat);   // Cov dLat  - #8
            varY = solver.getVariance(TypeID::dLon);   // Cov dLon  - #9
            varZ = solver.getVariance(TypeID::dH);     // Cov dH    - #10
        }
        else
        {

            x = nomXYZ.X() + solver.getSolution(TypeID::dx);    // dx    - #4
            y = nomXYZ.Y() + solver.getSolution(TypeID::dy);    // dy    - #5
            z = nomXYZ.Z() + solver.getSolution(TypeID::dz);    // dz    - #6

            varX = solver.getVariance(TypeID::dx);     // Cov dx    - #8
            varY = solver.getVariance(TypeID::dy);     // Cov dy    - #9
            varZ = solver.getVariance(TypeID::dz);     // Cov dz    - #10
        }
        //
        outfile << x << "  " << y << "  " << z << "  " << wetMap << "  ";
        outfile << sqrt(varX + varY + varZ) << "  ";

        outfile << numSats << endl;    // Number of satellites - #12

        double tConv(5400.0);

        double dt = time - time0;
        if (dt > tConv)
        {
            stats[0].add(x);
            stats[1].add(y);
            stats[2].add(z);
            stats[3].add(wetMap);
        }

        return;


    }  // End of method 'ex9::printSolution()'

    void PPPSolutionBase::printStats(ofstream& outfile,
        const vector<PowerSum> &stats)
    {
        Triple Aver, Var;
        double _3DRMS(0.0);

        for (size_t i = 0; i < 3; i++)
        {
            Aver[i] = stats[i].average();
            Var[i] = stats[i].variance();
            _3DRMS += Var[i];
            Var[i] = sqrt(Var[i]);
        }

        outfile << "Averege  " << Aver[0] << "  " << Aver[1] << "  " << Aver[2] << "  ";
        outfile << "St.Dev.  " << Var[0] << "  " << Var[1] << "  " << Var[2] << "  " << sqrt(_3DRMS) << endl;
    }

    // Method to print model values
    void PPPSolutionBase::printModel(ofstream& modelfile, const gnssRinex& gData, int   precision)
    {
        // Prepare for printing
        modelfile << fixed << setprecision(precision);

        // Get epoch out of GDS
        CommonTime time(gData.header.epoch);

        // Iterate through the GNSS Data Structure
        for (satTypeValueMap::const_iterator it = gData.body.begin();
            it != gData.body.end();
            it++)
        {
            // Print epoch
            modelfile << static_cast<YDSTime>(time).year << "  ";    // Year          #1
            modelfile << static_cast<YDSTime>(time).doy << "  ";    // DayOfYear      #2
            modelfile << static_cast<YDSTime>(time).sod << "  ";    // SecondsOfDay   #3

            // Print satellite information (Satellite system and ID number)
            modelfile << (*it).first << " ";             // System         #4
                                                         // ID number      #5

                                                         // Print model values
            for (typeValueMap::const_iterator itObs = (*it).second.begin();
                itObs != (*it).second.end();
                itObs++)
            {
                // Print type names and values
                modelfile << (*itObs).first << " ";
                modelfile << (*itObs).second << " ";

            }  // End of 'for( typeValueMap::const_iterator itObs = ...'

            modelfile << endl;

        }  // End for (it = gData.body.begin(); ... )

    }  // End of method 'ex9::printModel()'

    bool PPPSolutionBase::loadApprPos(std::string path)
    {
        apprPos.clear();
        try
        {
            ifstream file(path);
            if (file.is_open())
            {
                unsigned int W(0);
                double sow(0.0), x(0.0), y(0.0), z(0.0), rco(0.0);
                int solType(0);
                string sTS;

                string line;
                while (file >> W >> sow >> sTS >> solType >> x >> y >> z >> rco)
                {
                    if (!solType)
                    {
                        GPSWeekSecond gpst(W, sow, TimeSystem::GPS);
                        CommonTime time = static_cast<CommonTime> (gpst);
                        Xvt xvt;
                        xvt.x = Triple(x, y, z);
                        xvt.clkbias = rco;
                        apprPos.insert(pair<CommonTime, Xvt>(time, xvt));
                    }
                    string line;
                    getline(file, line);
                }
            }
            else
            {
                return false;
            }
        }
        catch (const std::exception& e)
        {
            cout << e.what() << endl;
            return false;
        }
        return true;
    }
}