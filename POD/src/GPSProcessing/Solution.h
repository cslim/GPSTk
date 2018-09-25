#ifndef POD_SOLUTION_H
#define POD_SOLUTION_H

#include"BasicFramework.hpp"
#include"ConfDataReader.hpp"

#include"GnssEpochMap.h"
#include"CustomSolution.h"


namespace pod
{
    class Solution 
        : public gpstk::BasicFramework
    {
       
    public:
        Solution(const char* path);
       virtual ~Solution()
        {
           
        }
        virtual void process();
        void chekObs();
        void saveToDb();
        void saveStatistic();
        GnssEpochMap  getData()
        {
            return solver.getData();
        };

    protected:
        
        GnssDataStore_sptr data;

        gpstk::CommandOptionWithArg confFile;

        // Configuration file reader
        gpstk::ConfDataReader confReader;

        CustomSolution solver;

    };
}

#endif // !POD_SOLUTION_H
