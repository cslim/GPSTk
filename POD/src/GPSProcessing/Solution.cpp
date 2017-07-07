#include"Solution.h"
#include <direct.h>
#include<windows.h>
namespace POD
{

    Solution::Solution(char* path) :
        BasicFramework("POD",
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
        this->solver = PPPSolutionBase::Factory(isSpace, confReader);

    }

    bool Solution::loadConfig(char* path)
    {
        // Check if the user provided a configuration file name
        if (confFile.getCount() > 0)
        {
            // Enable exceptions
            confReader.exceptions(ios::failbit);

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

        }
        else
        {
            try
            {
                // Try to open default configuration file
                confReader.open("config.txt");
            }
            catch (...)
            {

                cerr << "Problem opening default configuration file 'pppconf_my.txt'"
                    << endl;
                cerr << "Maybe it doesn't exist or you don't have proper read "
                    << "permissions. Try providing a configuration file with "
                    << "option '-c'."
                    << endl;

                exit(-1);

            }  // End of 'try-catch' block

        }  // End of 'if ( confFile.getCount() > 0 )'

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
}