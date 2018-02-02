#ifndef POD_SOLUTION_H
#define POD_SOLUTION_H

#include"BasicFramework.hpp"
#include"ConfDataReader.hpp"
#include"PPPSolutionBase.h"
#include"GnssEpochMap.h"
#include"CustomSolution.h"
using namespace gpstk;
namespace pod
{
    class Solution : public BasicFramework
    {
       
    public:
        Solution(const char* path);
       virtual ~Solution()
        {
           
        }
        virtual void process();
        void chekObs();
        void saveToDb();

        GnssEpochMap  getData()
        {
            return solver.getData();
        };

    protected:
        
        GnssDataStore_sptr data;

        CommandOptionWithArg confFile;

        // Configuration file reader
        ConfDataReader confReader;

        CustomSolution solver;

    };
}

#endif // !POD_SOLUTION_H
