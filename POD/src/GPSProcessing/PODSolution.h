#ifndef POD_POD_SOLUTION_H
#define POD_POD_SOLUTION_H

#include"PPPSolutionBase.h"
using namespace gpstk;
namespace pod
{
    class PODSolution : public PPPSolutionBase
    {
    public:

        PODSolution(GnssDataStore_sptr confData);
        virtual ~PODSolution(){};
    
    protected:
        virtual bool processCore() override;
        virtual double mapSNR(double  value) override;
    };
}

#endif // !POD_POD_SOLUTION_H