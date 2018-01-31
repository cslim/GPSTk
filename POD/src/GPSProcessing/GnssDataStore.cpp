#include"GnssDataStore.hpp"
#include"FsUtils.h"
#include"Rinex3NavStream.hpp"

#include"CodeProcSvData.h"
using namespace std;
using namespace gpstk;

namespace pod
{
    bool GnssDataStore::initReader(const char* path)
    {
        try
        {
            // Try to open the provided configuration file
            confReader->open(path);
        }
        catch (...)
        {
            cerr << "Problem opening file "
                << path
                << endl;
            cerr << "Maybe it doesn't exist or you don't have proper "
                << "read permissions." << endl;

            exit(-1);

        }  // End of 'try-catch' block

           // If a given variable is not found in the provided section, then
           // 'confReader' will look for it in the 'DEFAULT' section.
        confReader->setFallback2Default(true);

        return true;
    }
    void  GnssDataStore::LoadData(const char* path)
    {
        try
        {
            initReader(path);
            workingDir = fs::path(path).parent_path().string();
            opts.isSpaceborneRcv = confReader->fetchListValueAsBoolean("IsSpaceborneRcv");
            
            opts.maskEl = confReader->getValueAsDouble("ElMask");

            opts.maskSNR = confReader->getValueAsDouble("SNRmask");

            opts.systems.insert(SatID::SatelliteSystem::systemGPS);
            bool useGLN = confReader->getValueAsBoolean("useGLN");
            if (useGLN)
                opts.systems.insert(SatID::SatelliteSystem::systemGlonass);

            cout << "Used Sat. Systems: ";
            for (auto& ss : opts.systems)
                cout << SatID::convertSatelliteSystemToString(ss) << " ";
            cout << endl;

            //set BCE files direcory 
            bceDir = confReader->getValue("RinexNavFilesDir");
            //set generic files direcory 
            string subdir = confReader->getValue("GenericFilesDir");
            genericFilesDirectory = workingDir + "\\" + subdir + "\\";

            subdir = confReader->getValue("RinesObsDir");
            FsUtils::getAllFilesInDir(workingDir + "\\" + subdir, rinexObsFiles);

            cout << "Ephemeris Loading... ";
            cout << loadEphemeris() << endl;

            opts.isComputeApprPos = confReader->getValueAsBoolean("calcApprPos");
            apprPosFile = confReader->getValue("apprPosFile");

            opts.dynamics = (Dynamics)confReader->getValueAsInt("Dynamics");

            //load clock data from RINEX clk files, if required
            if (confReader->getValueAsBoolean("UseRinexClock"))
            {
                cout << "Rinex clock data Loading... ";
                cout << loadClocks() << endl;
            }

            cout << "IonoModel Loading... ";
            cout << loadIono() << endl;

            cout << "Load Glonass FCN data... ";
            cout << loadFcn() << endl;

            cout << "Load Earth orientation data... ";
            cout << loadEOPData() << endl;

        }
        catch (const Exception& e)
        {
            cout << "Failed to load input data. An error has occured: " << e.what() << endl;
            exit(-1);
        }
        catch (const std::exception& e)
        {
            cout << "failed to load input data: An error has occured: " << e.what() << endl;
            exit(-1);
        }
    }

    //
    bool GnssDataStore::loadEphemeris()
    {
        // Set flags to reject satellites with bad or absent positional
        // values or clocks
        SP3EphList.clear();
        SP3EphList.rejectBadPositions(true);
        SP3EphList.rejectBadClocks(true);

        list<string> files;
        string subdir = confReader->getValue("EphemerisDir");
        FsUtils::getAllFilesInDir(workingDir + "\\" + subdir, files);

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
                exit(-1);
            }
        }
        return files.size() > 0;
    }
    //reading clock data
    bool GnssDataStore::loadClocks()
    {
        list<string> files;
        string subdir = confReader->getValue("RinexClockDir");
        FsUtils::getAllFilesInDir(workingDir + "\\" + subdir, files);

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
                exit(-1);
            }
        }//   while ((ClkFile = confReader->fetchListValue("rinexClockFiles", station)) != "")
        return files.size() > 0;
    }

    bool  GnssDataStore::loadIono()
    {
        const string gpsObsExt = ".[\\d]{2}[nN]";
        list<string> files;

        FsUtils::getAllFilesInDir(workingDir + "\\" + bceDir, gpsObsExt, files);
        int i = 0;
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
                    i++;
                }
                else
                {
                    cerr << "WARNING: Navigation file " << file
                        << " doesn't have valid ionospheric correction parameters." << endl;
                    exit(-1);
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
        return  i > 0;
    }

    bool  GnssDataStore::loadFcn()
    {
        const string glnObsExt = ".[\\d]{2}[gG]";
        list<string> files;
        FsUtils::getAllFilesInDir(workingDir + "\\" + bceDir, glnObsExt, files);

        for (auto file : files)
        {
            try
            {
                SatID::loadGloFcn(file.c_str());
            }
            catch (...)
            {
                cerr << "Problem opening file " << file << endl;
                cerr << "Maybe it doesn't exist or you don't have proper read "
                    << "permissions." << endl;
                exit(-1);
            }
        }
        return   SatID::glonassFcn.size() > 0;
    }

    bool GnssDataStore::loadEOPData()
    {

        string iersEopFile = genericFilesDirectory;
        try
        {
            iersEopFile += confReader->getValue("IersEopFile");
        }
        catch (...)
        {
            cerr << "Problem get value from config: file \"IersEopFile\" " << endl;
            exit(-1);
        }

        try
        {
            eopStore.addIERSFile(iersEopFile);
        }
        catch (...)
        {
            cerr << "Problem opening file " << iersEopFile << endl;
            cerr << "Maybe it doesn't exist or you don't have proper read "
                << "permissions." << endl;
            exit(-1);
        }
        return eopStore.size() > 0;
    }

    bool GnssDataStore::loadCodeBiades()
    {
        string biasesFile = genericFilesDirectory;
        try
        {
            biasesFile += confReader->getValue("IersEopFile");
        }
        catch (...)
        {
            cerr << "Problem get value from config: file \"IersEopFile\" " << endl;
            exit(-1);
        }

        try
        {
            // DCBData.setDCBFile(biasesFile,);
        }
        catch (...)
        {
            cerr << "Problem opening file " << biasesFile << endl;
            cerr << "Maybe it doesn't exist or you don't have proper read "
                << "permissions." << endl;
            exit(-1);
        }
        return eopStore.size() > 0;
    }

    void GnssDataStore::checkObservable()
    {
        string subdir = confReader->getValue("RinesObsDir");
        FsUtils::getAllFilesInDir(workingDir + "\\" + subdir, rinexObsFiles);

        ofstream os(workingDir + "\\ObsStatisic.out");

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
                    os << setprecision(12) << (CivilTime)rod.time << " ";
                    int nGPS = 0, nGLN = 0;
                    for (auto &it : rod.obs)
                    {
                        if (opts.systems.find(it.first.system) == opts.systems.end()) continue;
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

                    os << nGPS << " " << nGLN << " " << NumBadCNo1 << " " << NumP1 << " " << NumP2 << endl;
                }
            }
        }
    }


    bool GnssDataStore::loadApprPos()
    {
        apprPos.clear();
        try
        {
            ifstream file(workingDir + "\\" + apprPosFile);
            if (file.is_open())
            {
                unsigned int Y(0), m(0), d(0), D(0), M(0), S(0);
                double sow(0.0), x(0.0), y(0.0), z(0.0), rco(0.0), dt(0);
                int solType(0);
                string sTS;

                string line;
                while (file >> Y >> m >> d >> D >> M >> S >> sTS >> dt >> solType >> x >> y >> z >> rco)
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
                auto mess = "Can't load data from file: " + apprPosFile;
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
