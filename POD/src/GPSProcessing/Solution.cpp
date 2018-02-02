#include"Solution.h"
#include <direct.h>
#include<windows.h>
#include "Shlwapi.h"

#include"FsUtils.h"

namespace pod
{

    Solution::Solution(const char* path) :
        BasicFramework("pod",
                       "discr"),
        // Option initialization. "true" means a mandatory option
        confFile(CommandOption::stdType,
                 'c',
                 "conffile",
                 " [-c|--conffile]    Name of configuration file ('config.txt' by default).",
                 false)
    {
        data.reset(new GnssDataStore(confReader));
        data->LoadData(path);

        solver.setConfigData(data);

    }

    void Solution::process()
    {
        solver.process();

    }
    void Solution::saveToDb()
    {

        auto gMap = solver.getData();
        gMap.updateMetadata();

        fs::path dbPath(data->workingDir + "\\" + solver.fileName());

        dbPath.replace_extension("db");

        //delete curtrent db file if exists
        string cmd = "del \"" + dbPath.string() + "\"";
        system(cmd.c_str());

        SQLiteAdapter db(dbPath.string());
        gMap.title = confReader.filename;
        db.addNewFile(gMap);
    }

    void Solution::chekObs()
    {
        data->checkObservable();
    }
}