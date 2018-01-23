#ifndef POD_SQLITE_ADAPTER
#define POD_SQLITE_ADAPTER


#include <ios>
#include <iostream>
#include<filesystem>
#include<string>
#include "sqlite3.h"
#include"auxiliary.h"

namespace fs = std::experimental::filesystem;

namespace pod
{
    class SQLiteAdapter
    {
#pragma region static meembers
        const unsigned  SCHEMA_VERSION = 1;

#pragma endregion

    public: SQLiteAdapter(char * path)
    {

    }

    private: SQLiteAdapter()
    {

    }

    private: void initialize(char * path)
    {
        initialize(fs::path(path));
    }

    private: void initialize(const fs::path& path);

    private: void createDb(const fs::path& path);
    private: void setPragmas();
    private: void tryExecuteNonQuery(const string & sql);
    private: void tryExecuteNonQuery(const char * sql);

    private: int tryExecuteNonQueryAndGetRowId(const string& sql);
    private: int tryExecuteNonQueryAndGetRowId(const char * sql);

    private: inline void errorHandler(int errorCode, char *error, const char *methodName);

    public: ~SQLiteAdapter()
    {
        sqlite3_close(db);
    }

#pragma region Fields
    private: std::string fileName;
    private: sqlite3 *db;
#pragma endregion

    private: const  std::string createSchemaCommand =
        "BEGIN TRANSACTION;"
        "CREATE TABLE IF NOT EXISTS `SvDataItems` ("
        "	`SV`	INTEGER NOT NULL,"
        "	`DataID`	INTEGER NOT NULL,"
        "	`EpochID`	INTEGER NOT NULL,"
        "	FOREIGN KEY(`SV`) REFERENCES `SVS`(`USI`),"
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
        "	`USI`	INTEGER NOT NULL,"
        "	`SatID`	INTEGER NOT NULL,"
        "	`SatSystemId`	INTEGER NOT NULL,"
        "	PRIMARY KEY(`USI`)"
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
        "	FOREIGN KEY(`SV`) REFERENCES `SVS`(`USI`),"
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
        "CREATE TABLE IF NOT EXISTS `ColorSv` ("
        "	`USI`	INTEGER NOT NULL,"
        "	`FileID`	INTEGER NOT NULL,"
        "	`Color`	INTEGER NOT NULL,"
        "	FOREIGN KEY(`FileID`) REFERENCES `GnssObsFile`(`ID`)"
        ");"
        "CREATE TABLE IF NOT EXISTS `ColorGen` ("
        "	`FileID`	INTEGER NOT NULL UNIQUE,"
        "	`Color`	INTEGER NOT NULL,"
        "	FOREIGN KEY(`FileID`) REFERENCES `GnssObsFile`(`ID`)"
        ");"
        "CREATE UNIQUE INDEX IF NOT EXISTS `SlnTypesColors_Type_File_Unique` ON `ColorsSt` ("
        "	`SolType`,"
        "	`FileID`"
        ");"
        "CREATE UNIQUE INDEX IF NOT EXISTS `SatColor_SV_File_Unuque` ON `ColorSv` ("
        "	`FileID`,"
        "	`USI`"
        ");"
        "CREATE UNIQUE INDEX IF NOT EXISTS `SkySector_Unique` ON `SkySectors` ("
        "	`A0`	ASC,"
        "	`A1`,"
        "	`E0`,"
        "	`E1`"
        ");"
        "COMMIT;";

    };
}

#endif // !POD_SQLITE_ADAPTER

