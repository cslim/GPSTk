#include"Solution.h"
#include <direct.h>
#include<windows.h>
#include "Shlwapi.h"
#include"auxiliary.h"
#include<filesystem>

namespace fs = std::experimental::filesystem;

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
        loadConfig(path);

        bool isSpace = confReader.fetchListValueAsBoolean("IsSpaceborneRcv");
        this->solver = PPPSolutionBase::Factory(isSpace, confReader, fs::path(path).parent_path().string());

    }

    bool Solution::loadConfig(const char* path)
    {
        try
        {
            // Try to open the provided configuration file
            confReader.open(path);
        }
        catch (...)
        {
            cerr << "Problem opening file "
                << confFile.getValue()[0]
                << endl;
            cerr << "Maybe it doesn't exist or you don't have proper "
                << "read permissions." << endl;

            exit(-1);

        }  // End of 'try-catch' block

        // If a given variable is not found in the provided section, then
        // 'confReader' will look for it in the 'DEFAULT' section.
        confReader.setFallback2Default(true);

        return true;
    }

    void Solution::process()
    {
        solver->LoadData();
        solver->process();
    }

    void Solution::chekObs()
    {
        solver->checkObservable();
    }
}