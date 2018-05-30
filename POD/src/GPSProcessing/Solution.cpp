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
        try
        {
            solver.process();
        }
        catch (gpstk::Exception & e)
        {
            cerr << "An exception has occured. Processing stopped." << endl;
            cerr << e.getLocation() << endl;
            cerr << e.getText() << endl;

            GPSTK_RETHROW(e);
        }
    }

    void Solution::saveToDb()
    {
        auto fName = solver.fileName();
        auto& gMap = solver.getData();
        gMap.title = fName;
        gMap.updateMetadata();

        fs::path dbPath(data->workingDir + "\\" + fName);
        dbPath.replace_extension("db");

        //delete curtrent solution database file, if exists
        string cmd = "del \"" + dbPath.string() + "\"";
        system(cmd.c_str());

        //insert solution data into DB 
        SQLiteAdapter db(dbPath.string());
        db.addNewFile(gMap);
    }

    void Solution::chekObs()
    {
        data->checkObservable();
    }
}