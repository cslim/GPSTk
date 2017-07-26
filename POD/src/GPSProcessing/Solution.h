#ifndef POD_SOLUTION_H
#define POD_SOLUTION_H

#include"BasicFramework.hpp"
#include"ConfDataReader.hpp"
#include"PPPSolutionBase.h"
using namespace gpstk;
namespace pod
{
    class Solution : public BasicFramework
    {
       
    public:
        Solution(const char* path);
       virtual ~Solution()
        {
            delete solver;
        }
        virtual void process();
        
        std::list<gpstk::gnssRinex> & getData()
        {
            return solver->getData();
        };

    protected:
     
        bool loadConfig(const char* path);

        CommandOptionWithArg confFile;

        // Configuration file reader
        ConfDataReader confReader;

        PPPSolutionBase* solver;

    };
}

#endif // !POD_SOLUTION_H
