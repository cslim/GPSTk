#ifndef POD_SQLITE_ADAPTER
#define POD_SQLITE_ADAPTER

#include"FsUtils.h"
#include"GnssEpochMap.h"
#include "sqlite3.h"

namespace pod
{
    class SQLiteAdapter
    {
#pragma region static meembers
    public: static void   testSQLite(const char *f1, const char *f2);
    public: static const unsigned  SCHEMA_VERSION;
    public: static const std::string createSchemaCommand;

#pragma endregion

#pragma region Create methods



    public: SQLiteAdapter(char * path) :firstTime(true), fileName(path)
    {
        initialize();
    }

    private: SQLiteAdapter()
    {
    }

    private: void initialize();

    private: void setPragmas();
    private: void create();

#pragma endregion

#pragma region Insert methods

    public:  void addNewFile(const pod::GnssEpochMap & eMap);
    private: void addObsData(const std::pair<gpstk::TypeID, double> & typeValuePair);
    private: void addSlnData(const gpstk::typeValueMap& slnData);
    private: void addSvData(const gpstk::satTypeValueMap& svData);
    private: void addNewEpoch(const std::pair<gpstk::CommonTime, pod::GnssEpoch>& epoch);
    private: void addSV(const gpstk::SatID& sv);

#pragma endregion

#pragma region service methods

    private: void tryExecuteNonQuery(const std::string & sql);
    private: void tryExecuteNonQuery(const char * sql);
    private: void tryExecuteNonQuery(sqlite3_stmt * stmt);

    private: int tryExecuteNonQueryAndGetRowId(const std::string& sql);
    private: int tryExecuteNonQueryAndGetRowId(const char * sql);
    private: int tryExecuteNonQueryAndGetRowId(sqlite3_stmt * stmt);

    private: inline void errorHandler(int errorCode, char *error);

    private: void updateTransaction();
    private: void finalizeTransactionsSequence();
    private: std::string formatTime(const gpstk::CommonTime &t);
#pragma endregion

    public: ~SQLiteAdapter()
    {
        sqlite3_close(db);
    }

#pragma region Fields

    private: char* fileName;
    private: sqlite3 *db;

    private: int  lastFileID;
    private: int  lastEpochID;
    private: long lastTypeValuePairID;

    private: bool  firstTime;
    private: int obsItemCounter = 0;
    private: int maxObsItemsPerTransaction = 100000;

#pragma endregion

    };
}

#endif // !POD_SQLITE_ADAPTER

