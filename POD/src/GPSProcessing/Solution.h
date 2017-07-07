#ifndef POD_SOLUTION_H
#define POD_SOLUTION_H

#include"BasicFramework.hpp"
#include"ConfDataReader.hpp"
#include"PPPSolutionBase.h"
using namespace gpstk;
namespace POD
{
    class Solution : public BasicFramework
    {
       
    public:
        Solution(char* path);
       virtual ~Solution()
        {
            delete solver;
        }
        virtual void process();

    protected:

        bool loadConfig(char* path);

        CommandOptionWithArg confFile;

        // Configuration file reader
        ConfDataReader confReader;

        PPPSolutionBase* solver;

    };
}

#endif // !POD_SOLUTION_H
