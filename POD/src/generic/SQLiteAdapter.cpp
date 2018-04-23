#include "SQLiteAdapter.h"
#include<boost\format.hpp>
#include"WinUtils.h"
using namespace std;
using namespace gpstk;
using boost::format;
namespace pod
{
    void SQLiteAdapter::testSQLite(const char* path2obs, const char* path2GlnNav)
    {
        SatID::loadGloFcn(path2GlnNav);

        namespace fs = std::experimental::filesystem;
        Rinex3ObsStream rin(path2obs);
        gnssRinex  gRin;

        std::ostringstream ss;
        auto t = std::chrono::system_clock::to_time_t(std::chrono::system_clock::now());
        ss << std::ctime(&t);

        GnssEpochMap eMap;
        eMap.title = ss.str();

        Rinex3ObsHeader header;
        // Read the RINEX header
        rin >> header;
        // Read the RINEX data epoch by epoch
        while (rin >> gRin)
        {
            auto ge = GnssEpoch(gRin);
            ge.slnData[TypeID::recSlnType] =0;
            eMap.data.insert(make_pair(gRin.header.epoch, ge));
        }

        eMap.updateMetadata();
        fs::path dbPath(path2obs);
        dbPath.replace_extension("db");

        string  str = dbPath.string();
        char * cstr = new char[str.length() + 1];

        std::strcpy(cstr, str.c_str());
        SQLiteAdapter dbAdapter(cstr);

        dbAdapter.addNewFile(eMap);
    }

#pragma region Create methods

    void SQLiteAdapter::initialize()
    {
        sqlite3_initialize();

        int rc = sqlite3_open_v2(fileName.c_str(), &db, SQLITE_OPEN_READWRITE | SQLITE_OPEN_CREATE, NULL);
        if (rc)
        {
            auto excStr = (boost::format("Error opening SQLite3 database:%1%") % sqlite3_errmsg(db)).str();
            errorHandler(rc, const_cast<char*>(excStr.c_str()));
        }

        // string dbString = (boost::format("Data Source =%1%;Version=3;") % fileName).str();
        create();
    };

    void SQLiteAdapter::setPragmas()
    {
        list<string> pragmas;
        pragmas.push_back("PRAGMA temp_store = \"2\";");

        pragmas.push_back("PRAGMA journal_mode = \"OFF\";");
        pragmas.push_back((boost::format("PRAGMA schema_version = \"%1%\";") % SCHEMA_VERSION).str());
        
        //tryExecuteNonQuery("BEGIN TRANSACTION;");
        for (auto & it : pragmas)
            tryExecuteNonQuery(it);

        //tryExecuteNonQuery("COMMIT;");

    }

    void SQLiteAdapter::create()
    {
        setPragmas();
        sqlite3_exec(db, createSchemaCommand.c_str(), NULL, NULL, NULL);
       // tryExecuteNonQuery(createSchemaCommand.c_str());
    }

#pragma endregion

#pragma region insert methods

    void SQLiteAdapter::addNewFile(const pod::GnssEpochMap & eMap)
    {
        char* sql = "INSERT INTO `GnssObsFile`(`FullName`,`Title`) VALUES( @Name, @Title);";
        sqlite3_stmt *comm;
        sqlite3_prepare_v2(db, sql, -1, &comm, NULL);
        sqlite3_bind_text(comm, 1, eMap.title.c_str(), -1, 0);
        sqlite3_bind_text(comm, 2, eMap.title.c_str(), -1, 0);

        lastFileID =  tryExecuteNonQueryAndGetRowId(comm);

        // fill the  SV metadata
        tryExecuteNonQuery("BEGIN TRANSACTION;");
        for (const auto& it : eMap.svs)
        {
            //add sv to table of all SV
           int svid  = addSV(it);

        }
        tryExecuteNonQuery("COMMIT;");

        // fill the  TypeId's metadata
        tryExecuteNonQuery("BEGIN TRANSACTION;");
        for (const auto it : eMap.types)
        {
            char* sql = "INSERT INTO `TypeIDsByFiles`(`FileId`,`TypeId`) VALUES( @FileId, @TypeId);";
            sqlite3_stmt *comm;
            sqlite3_prepare_v2(db, sql, -1, &comm, NULL);
            sqlite3_bind_int(comm, 1, lastFileID);
            sqlite3_bind_int(comm, 2, it.type);
            tryExecuteNonQuery(comm);
        }
        tryExecuteNonQuery("COMMIT;");

        for (auto& it : eMap.data)
            addNewEpoch(it);

        finalizeTransactionsSequence();
    }

    void SQLiteAdapter::addObsData(const pair<TypeID, double> & typeValuePair)
    {
        char* sql = "INSERT INTO `RinexTypePairs`(`Type`,`Value`) VALUES (@Type, @Value);";
        sqlite3_stmt *comm;
        sqlite3_prepare_v2(db, sql, -1, &comm, NULL);
        sqlite3_bind_int(comm, 1, (int)typeValuePair.first.type);
        sqlite3_bind_double(comm, 2, typeValuePair.second);

        lastTypeValuePairID = tryExecuteNonQueryAndGetRowId(comm);

        ++obsItemCounter;
    }

    void SQLiteAdapter::addSlnData(const gpstk::typeValueMap&  slnData)
    {
        for(auto& it : slnData)
        {
            addObsData(it);
            char* sql = "INSERT INTO `SlnDataItems`(`EpochID`,`DataID`) VALUES (@EpochID, @DataID);";
            sqlite3_stmt *comm;
            int rc =  sqlite3_prepare_v2(db, sql, -1, &comm, NULL);
            
            sqlite3_bind_int(comm, 1, lastEpochID);
            sqlite3_bind_int64(comm, 2, lastTypeValuePairID);
            tryExecuteNonQuery(comm);
        }
    }

    void SQLiteAdapter::addSvData(const gpstk::satTypeValueMap& svData)
    {
        for (auto& svIt : svData)
        {
            SatID satId = svIt.first;
            for (auto& it : svIt.second)
            {
                addObsData(it);
            
                char* sql = "INSERT INTO `SvDataItems`(`SV`,`DataID`, `EpochID`) VALUES ((SELECT ID FROM SVS WHERE SVID = @SVID AND SSID = @SSID), @DataID, @EpochID);";
                sqlite3_stmt *comm;
                int rc  =  sqlite3_prepare_v2(db, sql, -1, &comm, NULL);
                sqlite3_bind_int(comm, 1, satId.id);
                sqlite3_bind_int(comm, 2, (int)satId.system);
                sqlite3_bind_int64(comm, 3, lastTypeValuePairID);
                sqlite3_bind_int(comm, 4, lastEpochID);

                tryExecuteNonQuery(comm);
            }
        }
    }

    void SQLiteAdapter::addNewEpoch(const  std::pair<CommonTime, pod::GnssEpoch>& epoch)
    {
        updateTransaction();
        char* sql = "INSERT INTO `Epochs`(`Time`,`FileID`,'OccupationID') VALUES(@time, @FileID, @OccupationID);";
        sqlite3_stmt *comm;
        sqlite3_prepare_v2(db, sql, -1, &comm, NULL);
        
        string occId = formatTime(epoch.first);
        sqlite3_bind_text(comm, 1, occId.c_str(), -1, 0);
        sqlite3_bind_int(comm,  2, lastFileID);
        sqlite3_bind_text(comm, 3, "", - 1, 0);

        lastEpochID = tryExecuteNonQueryAndGetRowId(comm);

        addSlnData(epoch.second.slnData);
        //TypeIDSet typeSet;// { TypeID::postfitC };
        //typeSet.insert(TypeID::postfitC);
        
       addSvData(epoch.second.satData.extractTypeID(requaredTypes));
    }

    int SQLiteAdapter::addSV(const gpstk::SatID & sv)
    {
        char* sql = "INSERT OR IGNORE INTO `SVS`(`SVID`,`SSID`) VALUES (@SVID, @SSID);";
        sqlite3_stmt *comm;
        sqlite3_prepare_v2(db, sql, -1, &comm, NULL);
        sqlite3_bind_int(comm, 1, sv.id);
        sqlite3_bind_int(comm, 2, (int)sv.system);

        return tryExecuteNonQueryAndGetRowId(comm);
    }

#pragma endregion

#pragma region service methods

    void SQLiteAdapter::tryExecuteNonQuery(sqlite3_stmt * comm)
    {
        char * zErrMsg;
        int OK_DONE = SQLITE_DONE | SQLITE_OK;
        int rc;
        while (rc = sqlite3_step(comm) == SQLITE_ROW)
        {
            DBOUT("\r\n---")
            DBOUT(sqlite3_column_text(comm, 0));
            DBOUT("---\r\n")
        }
        if ((rc | OK_DONE)  != OK_DONE)
        {
            
            sqlite3_finalize(comm);
            errorHandler(rc, "Can't execute SQL command.");
        }
        sqlite3_finalize(comm);
    }

    int SQLiteAdapter::tryExecuteNonQueryAndGetRowId(sqlite3_stmt * stmt)
    {
        tryExecuteNonQuery(stmt);
   
        return sqlite3_last_insert_rowid(db);
    }

    void SQLiteAdapter::tryExecuteNonQuery(const string & sql)
    {
        tryExecuteNonQuery(sql.c_str());
    }

    void SQLiteAdapter::tryExecuteNonQuery(const char * sql)
    {
       sqlite3_stmt *comm;
       int rc = sqlite3_prepare_v2(db, sql, -1, &comm, NULL);
       if (rc != SQLITE_OK)
       {
           sqlite3_finalize(comm);
           errorHandler(rc, "Can't compile SQL command.");
       }
        tryExecuteNonQuery(comm);
    }

    int SQLiteAdapter::tryExecuteNonQueryAndGetRowId(const string & sql)
    {
        return tryExecuteNonQueryAndGetRowId(sql.c_str());
    }

    int SQLiteAdapter::tryExecuteNonQueryAndGetRowId(const char * sql)
    {
        sqlite3_stmt *comm;
        int rc = sqlite3_prepare_v2(db, sql, -1, &comm, NULL);
        if (rc != SQLITE_OK)
        {
            sqlite3_finalize(comm);
            errorHandler(rc, "Can't compile SQL command.");
        }

        return tryExecuteNonQueryAndGetRowId(comm);
    }

    void SQLiteAdapter::errorHandler(int errorCode, char *error)
    {
        DBOUT("An Error has occured.\r\nEroor code:");
        DBOUT(errorCode);
        DBOUT("\r\nEroor message: ");
        DBOUT(error);
        std::exception ex(error);

       // sqlite3_free(error);
        throw ex;
    }

     void SQLiteAdapter::updateTransaction()
     {
         if (firstTime)
             tryExecuteNonQuery("BEGIN TRANSACTION;");

         firstTime = false;

         if (obsItemCounter > maxObsItemsPerTransaction)
         {
             tryExecuteNonQuery("COMMIT;");
             tryExecuteNonQuery("BEGIN TRANSACTION;");
             obsItemCounter = 0;
         }
     }
     
     void SQLiteAdapter::finalizeTransactionsSequence()
     {
         if (!firstTime)
             tryExecuteNonQuery("COMMIT;");
         firstTime = true;
     }
    
     std::string SQLiteAdapter::formatTime(const CommonTime &t)
     {
         const char* fmt = "%Y-%02m-%02d %02H:%02M:%5.3f";
         return CivilTime(t).printf(fmt);
     }
#pragma endregion
    const  gpstk::TypeIDSet SQLiteAdapter::requaredTypes{
         TypeID::C1 ,
         TypeID::P2 ,
         TypeID::L1 ,
         TypeID::L2 ,
         TypeID::S1 ,
         TypeID::LI          ,
         TypeID::MWubbena          ,
         TypeID::rho               ,
         TypeID::dtSat             ,
         TypeID::elevation         ,
         TypeID::azimuth           ,
         TypeID::CSL1              ,
         TypeID::CSL2              ,
         TypeID::prefitC           ,
         TypeID::prefitL          ,
         TypeID::prefitL1          ,
         TypeID::postfitC          ,
         TypeID::postfitL         ,
         TypeID::postfitL1          ,
     };

     const unsigned SQLiteAdapter:: SCHEMA_VERSION = 1;

      const std::string SQLiteAdapter:: createSchemaCommand =
         "BEGIN TRANSACTION;"
         "CREATE TABLE IF NOT EXISTS `SvDataItems` ("
         "	`SV`	INTEGER NOT NULL,"
         "	`DataID`	INTEGER NOT NULL,"
         "	`EpochID`	INTEGER NOT NULL,"
         "	FOREIGN KEY(`SV`) REFERENCES `SVS`(`ID`),"
         "    FOREIGN KEY(`DataID`) REFERENCES `RinexTypePairs`(`ID`),"
         "	FOREIGN KEY(`EpochID`) REFERENCES `Epochs`(`ID`)"
         ");"
         "CREATE TABLE IF NOT EXISTS `SlnDataItems` ("
         "	`EpochID`	INTEGER,"
         "	`DataID`	INTEGER UNIQUE,"
         "	FOREIGN KEY(`EpochID`) REFERENCES `Epochs`(`ID`),"
         "	FOREIGN KEY(`DataID`) REFERENCES `RinexTypePairs`(`ID`)"
         ");"
         "CREATE TABLE IF NOT EXISTS `SatSystems` ("
         "	`ID`	INTEGER NOT NULL,"
         "	`Name`	TEXT NOT NULL UNIQUE,"
         "	PRIMARY KEY(`ID`)"
         ");"
         "CREATE TABLE IF NOT EXISTS `SVS` ("
         "	`ID`	INTEGER NOT NULL,"
         "	`SVID`	INTEGER NOT NULL,"
         "	`SSID`	INTEGER NOT NULL,"
          "	PRIMARY KEY(`ID`)"
         ");"
         "CREATE TABLE IF NOT EXISTS `Sites` ("
         "	`Name`	TEXT NOT NULL,"
         "	PRIMARY KEY(`Name`)"
         ");"
         "CREATE TABLE IF NOT EXISTS `RinexTypePairs` ("
         "	`ID`	INTEGER NOT NULL,"
         "	`Type`	TEXT NOT NULL,"
         "	`Value`	REAL NOT NULL,"
         "	PRIMARY KEY(`ID`)"
         ");"
         "CREATE TABLE IF NOT EXISTS `SkySectors` ("
         "	`ID`	INTEGER NOT NULL,"
         "	`A0`	REAL NOT NULL,"
         "	`A1`	REAL NOT NULL,"
         "	`E0`	REAL NOT NULL,"
         "	`E1`	REAL NOT NULL,"
         "	PRIMARY KEY(`ID`)"
         ");"
         "CREATE TABLE IF NOT EXISTS `MpProcResults` ("
         "	`ArcID`	INTEGER NOT NULL,"
         "	`SV`	INTEGER NOT NULL,"
         "	`Sector`INTEGER NOT NULL,"
         "	`Site`	TEXT NOT NULL,"
         "    FOREIGN KEY(`ArcID`) REFERENCES `MpArcs`(`ID`),"
      //   "	FOREIGN KEY(`SV`) REFERENCES `SVS`(`USI`),"
         "    FOREIGN KEY(`Sector`) REFERENCES `SkySectors`(`ID`),"
         "    FOREIGN KEY(`Site`) REFERENCES `Sites`(`Name`)"
         ");"
         "CREATE TABLE IF NOT EXISTS `MpArcs` ("
         "	`ID`	INTEGER NOT NULL,"
         "	`T1`	TEXT NOT NULL,"
         "	`T2`	TEXT NOT NULL,"
         "	`minEl`	REAL NOT NULL,"
         "	`maxEl`	REAL NOT NULL,"
         "	`H`	REAL NOT NULL,"
         "	`Count`	INTEGER NOT NULL,"
         "	PRIMARY KEY(`ID`)"
         ");"
         "CREATE TABLE IF NOT EXISTS `GnssObsFile` ("
         "	`ID`	INTEGER NOT NULL,"
         "	`FullName`	TEXT,"
         "	`Title`	TEXT,"
         "	PRIMARY KEY(`ID`)"
         ");"
         ""
         "CREATE TABLE IF NOT EXISTS `Epochs` ("
         "	`ID`	INTEGER NOT NULL,"
         "	`Time`	TEXT NOT NULL,"
         "	`FileID`	INTEGER NOT NULL,"
         "    `OccupationID`	TEXT,"
         "	FOREIGN KEY(`FileID`) REFERENCES `GnssObsFile`(`ID`),"
         "	PRIMARY KEY(`ID`)"
         ");"
         ""
         "CREATE TABLE IF NOT EXISTS `ColorsSt` ("
         "	`SolType`	INTEGER NOT NULL,"
         "	`FileID`	INTEGER NOT NULL,"
         "	`Color`	TEXT NOT NULL,"
         "	FOREIGN KEY(`FileID`) REFERENCES `GnssObsFile`(`ID`)"
         ");"
         "CREATE TABLE IF NOT EXISTS `ColorGen` ("
         "	`FileID`	INTEGER NOT NULL UNIQUE,"
         "	`Color`	INTEGER NOT NULL,"
         "	FOREIGN KEY(`FileID`) REFERENCES `GnssObsFile`(`ID`)"
         ");"
          "CREATE TABLE IF NOT EXISTS `ColorSv` ("
          "   `SVID`	INTEGER NOT NULL, "
          "   `SSID`	INTEGER NOT NULL, "
          "	`FileID`	INTEGER NOT NULL,"
          "	`Color`	INTEGER NOT NULL,"
          "	FOREIGN KEY(`SVID`) REFERENCES `SVS`(`SVID`),"
          "	FOREIGN KEY(`SSID`) REFERENCES `SVS`(`SSID`),"
          "	FOREIGN KEY(`FileID`) REFERENCES `GnssObsFile`(`ID`)"
          ");"
             "CREATE TABLE IF NOT EXISTS `TypeIDsByFiles` ("
             "	`FileId`	INTEGER NOT NULL,"
             "	`TypeId`	INTEGER NOT NULL,"
             "	FOREIGN KEY(`FileId`) REFERENCES `GnssObsFile`(`ID`)"
             ");"
         "CREATE UNIQUE INDEX IF NOT EXISTS `SlnTypesColors_Type_File_Unique` ON `ColorsSt` ("
         "	`SolType`,"
         "	`FileID`"
         ");"
         "CREATE UNIQUE INDEX IF NOT EXISTS `SatColor_SV_File_Unuque` ON `ColorSv` ("
         "	`FileID`,"
         "	`SVID`,"
         "	`SSID`"
         ");"
         "CREATE UNIQUE INDEX IF NOT EXISTS `SkySector_Unique` ON `SkySectors` ("
         "	`A0`	ASC,"
         "	`A1`,"
         "	`E0`,"
         "	`E1`"
         ");"
         "CREATE UNIQUE INDEX IF NOT EXISTS `SV_Unique` ON `SVS` ("
         "	`SVID`,"
         "	`SSID`"
         ");"
         "COMMIT;";

}
