#include"PPPSolutionBase.h"

#include<list>
#include<set>
#include <direct.h>
#include<windows.h>
#include<regex>

#include"GnssEpochMap.h"
#include"PPPSolution.h"
#include"PODSolution.h"
#include"auxiliary.h"


#include "Rinex3NavHeader.hpp"
#include "Rinex3NavData.hpp"
#include "Rinex3NavStream.hpp"

namespace pod
{
    PPPSolutionBase* PPPSolutionBase::Factory(bool isSpaceborne, ConfDataReader & reader, const string &  dir)
    {
        if (isSpaceborne)
            return new PODSolution(reader, dir);
        else
            return new PPPSolution(reader, dir);
    }

    PPPSolutionBase::PPPSolutionBase(ConfDataReader & cReader,string dir ) : confReader(&cReader), workingDir(dir)
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
    void PPPSolutionBase::mapSNR(gnssRinex & gRin)
    {
        for (auto &it1 : gRin.body)
        {
            auto  ts1 = TypeID(TypeID::S1);
            auto  ts2 = TypeID(TypeID::S2);
           
            auto s1 = it1.second.find(ts1);
            if (s1 != it1.second.end())
                s1->second =  mapSNR(s1->second);

            auto s2 = it1.second.find(ts1);
            if (s2 != it1.second.end())
                s2->second = mapSNR(s2->second);
        }
    }
    bool  PPPSolutionBase::LoadData()
    {
        try
        {
            //set generic files direcory 
            string subdir = confReader->fetchListValue("GenericFilesDir");
            genFilesDir = workingDir + "\\" + subdir + "\\";

            subdir = confReader->fetchListValue("RinesObsDir");
            auxiliary::getAllFilesInDir(workingDir+"\\"+ subdir, rinexObsFiles);

            cout << "Ephemeris Loading... ";
            cout << loadEphemeris() << endl;
            cout << "Clocks Loading... ";
            //cout << loadClocks() << endl;
            cout << "IonoModel Loading... ";
            cout << loadIono() << endl;

            calcApprPos = confReader->fetchListValueAsBoolean("calcApprPos");
            apprPosFile = confReader->fetchListValue("apprPosFile");

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
        auxiliary::getAllFilesInDir(workingDir+"\\"+ subdir, files);

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
        auxiliary::getAllFilesInDir(workingDir+"\\" + subdir, files);

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
        auxiliary::getAllFilesInDir(workingDir+"\\"+ subdir, files);

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
    
    void PPPSolutionBase::checkObservable()
    {
        string subdir = confReader->fetchListValue("RinesObsDir");
        auxiliary::getAllFilesInDir(workingDir + "\\" + subdir, rinexObsFiles);

        ofstream os(workingDir+ "\\ObsStatisic.out");

        std::set<SatID::SatelliteSystem> systems;
        systems.insert(SatID::SatelliteSystem::systemGPS);
        bool useGLN = confReader->fetchListValueAsBoolean("useGLN");
        if (useGLN)
            systems.insert(SatID::SatelliteSystem::systemGlonass);


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

            while (rin >> rod)
            {
                if (rod.epochFlag == 0 || rod.epochFlag == 1)  // Begin usable data
                {
                    int NumC1(0), NumP1(0), NumP2(0), NumBadCNo1(0);
                    os << setprecision(12) << (CivilTime)rod.time << " " ;
                    int nGPS = 0, nGLN = 0;
                    for (auto &it : rod.obs)
                    {
                        if (systems.find(it.first.system) == systems.end()) continue;
                        if (it.first.system == SatID::SatelliteSystem::systemGPS)
                            nGPS++;
                        if (it.first.system == SatID::SatelliteSystem::systemGlonass)
                            nGLN++;

                        auto &ids = CodeProcSvData::obsTypes[it.first.system];

                        double C1 = rod.getObs(it.first, ids[TypeID::C1], roh).data;

                        int CNoL1 = rod.getObs(it.first, ids[TypeID::S1], roh).data;
                        if (CNoL1 < 30) NumBadCNo1++;

                        double P1 = rod.getObs(it.first, ids[TypeID::P1], roh).data;
                        if (P1 > 0.0)  NumP1++;

                        double P2 = rod.getObs(it.first, ids[TypeID::P2], roh).data;
                        if (P2 > 0.0)  NumP2++;
                    }

                    os <<nGPS<<" "<<nGLN<<" "<< NumBadCNo1 << " " << NumP1 << " " << NumP2 << endl;
                }
            }

        }
    }

    void PPPSolutionBase::PRProcess()
    {
        NeillTropModel NeillModel = solverPR->initTropoModel(nominalPos, DoY);

        int badSol = 0;
        apprPos.clear();

        cout << "solverType " << solverPR->getName() << endl;

        solverPR->maskEl = 5;
        solverPR->maskSNR = 30;
        solverPR->ionoType = (CodeIonoCorrType)confReader->fetchListValueAsInt("CodeIonoCorrType");

        std::set<SatID::SatelliteSystem> systems;
        systems.insert(SatID::SatelliteSystem::systemGPS);
        bool useGLN = confReader->fetchListValueAsBoolean("useGLN");
        if (useGLN)
            systems.insert(SatID::SatelliteSystem::systemGlonass);

        ofstream os;
        string outPath = workingDir + "\\" + apprPosFile;
        // string outDbg= workingDir + "\\code_dbg.txt";
        // solverPR->dbg.open(outDbg);
        // solverPR->dbg << setprecision(12);
         os.open(outPath);
       
        //decimation
        int sampl(1);
        double tol(0.1);

        for (auto obsFile : rinexObsFiles)
        {
            cout << obsFile << endl;
            try 
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
                CommonTime ct0 = roh.firstObs;
                // Let's process all lines of observation data, one by one
                while (rin >> rod)
                {
                    double dt = rod.time - ct0;
                    GPSWeekSecond gpst = static_cast<GPSWeekSecond>(rod.time);

                    if (fmod(gpst.getSOW(), sampl) > tol) continue;

                    int GoodSats = 0;
                    int res = 0;

                    CodeProcSvData svData;
                    
                    ///
                    solverPR->selectObservables(rod, roh, systems, CodeProcSvData::obsTypes, svData);
                    
                    for (auto &it : svData.data)
                        it.second.snr = mapSNR(it.second.snr);
                    
                    svData.applyCNoMask(solverPR->maskSNR);
                    GoodSats = svData.getNumUsedSv();

                    os  << setprecision(6);
                    os  << CivilTime(gpst).printf("%02Y %02m %02d %02H %02M %02S %P") << " "<<dt<< " ";

                    if (GoodSats > 4)
                    {
                        solverPR->prepare(rod.time, SP3EphList, svData);
                        res = solverPR->solve(rod.time, ionoStore, svData);
                        os << res;
                    }
                    else
                        res = 1;

                    os << *solverPR<<" "<<svData << endl;
                    if (res == 0)
                    {
                        Xvt xvt;
                        xvt.x = Triple(solverPR->Sol(0), solverPR->Sol(1), solverPR->Sol(2));
                        xvt.clkbias = solverPR->Sol(3);
                        apprPos.insert(pair<CommonTime, Xvt>(rod.time, xvt));
                    }
                    else
                    {
                        solverPR->Sol = 0.0;
                        badSol++;
                    }
                }
                rin.close();
            }
            catch (Exception& e)
            {
                cerr << e << endl;
                GPSTK_RETHROW(e);
            }
            catch (...)
            {
                cerr << "Caught an unexpected exception." << endl;
            }
            cout << "Number of bad solutions " << badSol << endl;
        }
    }
    //
    void PPPSolutionBase::process()
    {

        if (calcApprPos)
        {
            PRProcess();
        }
        else
        {
            string appr_pos_file = workingDir + "\\" + apprPosFile;
            cout << "Approximate Positions loading from \n"+appr_pos_file+"\n... ";
            
            int i = loadApprPos(appr_pos_file);
        }

        PPPprocess();
    }

    // Method to print solution values
    void PPPSolutionBase::printSolution(ofstream& outfile,
        const SolverLMS& solver,
        const CommonTime& time0,
        const CommonTime& time,
        const ComputeDOP& cDOP,
        bool  useNEU,
        GnssEpoch &   gEpoch,
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
       
       // gEpoch.slnData.insert(pair<TypeID,double> (TypeID::dryTropo, dryTropo));
        gEpoch.slnData.insert(pair<TypeID, double>(TypeID::recZTropo, wetMap));

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

            gEpoch.slnData.insert(pair<TypeID, double>(TypeID::recX, x));
            gEpoch.slnData.insert(pair<TypeID, double>(TypeID::recY, y));
            gEpoch.slnData.insert(pair<TypeID, double>(TypeID::recZ, z));

            varX = solver.getVariance(TypeID::dx);     // Cov dx    - #8
            varY = solver.getVariance(TypeID::dy);     // Cov dy    - #9
            varZ = solver.getVariance(TypeID::dz);     // Cov dz    - #10
        }
        //
        double cdt = solver.getSolution(TypeID::cdt);
        gEpoch.slnData.insert(pair<TypeID, double>(TypeID::recCdt, cdt));

        double sigma = sqrt(varX + varY + varZ);
        gEpoch.slnData.insert(pair<TypeID, double>(TypeID::sigma, sigma));
        outfile << x << "  " << y << "  " << z << "  " << wetMap << "  " << sigma << "  ";
     
        gEpoch.slnData.insert(pair<TypeID, double>(TypeID::recSlnType, 16));

        //gEpoch.slnData.insert(pair<TypeID, double>(TypeID::recPDOP, cDOP.getPDOP()));
        outfile << gEpoch.satData.size() << endl;    // Number of satellites - #12

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

    bool PPPSolutionBase::loadApprPos(std::string path)
    {
        apprPos.clear();
        try
        {
            ifstream file(path);
            if (file.is_open())
            {
                unsigned int Y(0), m(0), d(0), D(0), M(0), S(0);
                double sow(0.0), x(0.0), y(0.0), z(0.0), rco(0.0),dt(0);
                int solType(0);
                string sTS;

                string line;
                while (file >> Y >> m >> d >> D >> M >> S >> sTS >> dt>> solType >> x >> y >> z >> rco)
                {
                    if (!solType)
                    {
                        CivilTime ct = CivilTime(Y, m, d, D, M, S, TimeSystem::GPS);
                        CommonTime time = static_cast<CommonTime> (ct);
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
                auto mess = "Can't load data from file: " + path;
                std::exception e(mess.c_str());
                throw e;
            }
        }
        catch (const std::exception& e)
        {
            cout << e.what() << endl;
            throw e;
        }
        return true;
    }
}