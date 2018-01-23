#include "SQLiteAdapter.h"
#include <boost/format.hpp>
using namespace std;
using boost::format;
namespace pod
{
    void SQLiteAdapter::initialize(const fs::path &path)
    {

        if (!fs::exists(path))
            createDb(path);
        else
            
        fileName = path.string();
        string dbString = (format("Data Source =%1%;Version=3;") %fileName).str();
       
    };

    void SQLiteAdapter::createDb(const fs::path &path)
    {
        int rc = sqlite3_open(path.string().c_str(), &this->db);
        if (rc)
        {
            auto excStr = format("Error opening SQLite3 database:%1%") % sqlite3_errmsg(db);
            sqlite3_close(db);
            DBOUT(excStr);
            std::exception ex;
            ex.what = excStr;
            throw ex;          
        }
    }

    void SQLiteAdapter::setPragmas()
    {
        list<string> pragmas;
        pragmas.push_back("PRAGMA temp_store = \"%2%\";");
        pragmas.push_back("PRAGMA journal_mode = \"OFF\";");
        pragmas.push_back((format("PRAGMA schema_version = \"%0%\";")%SCHEMA_VERSION).str());

        for (auto & it : pragmas)
            tryExecuteNonQuery(it);
    }

    void SQLiteAdapter::tryExecuteNonQuery(const string & sql)
    {
        tryExecuteNonQuery(sql.c_str());
    }

    void SQLiteAdapter::tryExecuteNonQuery(const char * sql)
    {
        char *error;
        int rc = sqlite3_exec(db, sql, NULL, NULL, &error);
        if (rc)
            errorHandler(rc, error, __FUNCTION__);
    }

    int SQLiteAdapter::tryExecuteNonQueryAndGetRowId(const string & sql)
    {
        return tryExecuteNonQueryAndGetRowId(sql.c_str());
    }

    int SQLiteAdapter::tryExecuteNonQueryAndGetRowId(const char * sql)
    {
        char *error;
        int rc = sqlite3_exec(db, sql, NULL, NULL, &error);
        if (rc)
            errorHandler(rc, error, __FUNCTION__);

        return sqlite3_last_insert_rowid(db);
    }

    void SQLiteAdapter::errorHandler(int errorCode, char *error, const char *methodName)
    {
        DBOUT("An Error has occured.\r\nWhere: ");
        DBOUT(methodName);
        DBOUT("\r\nEroor code: ");
        DBOUT(errorCode);
        DBOUT("\r\nEroor message: ");
        DBOUT(error);
        std::exception ex;
        ex.what = error;
        sqlite3_free(error);
        throw ex;
    }


}
