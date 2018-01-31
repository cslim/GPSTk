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

        this->solver = PPPSolutionBase::Factory(data);
        cout << this->solver << endl;
    }

    void Solution::process()
    {
        solver->process();
    }

    void Solution::chekObs()
    {
        data->checkObservable();
    }
}