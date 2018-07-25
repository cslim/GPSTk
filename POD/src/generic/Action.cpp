#include"Action.h"
#include"Solution.h"

namespace pod
{
    ///
    GnssEpochMap Action:: process()
    {
        Solution sol(path.c_str());

        sol.process();

        return sol.getData();
    }
}
