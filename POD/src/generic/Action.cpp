#include"Action.h"
#include"Solution.h"

namespace pod
{
    ///
    GnssEpochMap Action:: process()
    {
        std::string  var = this->path;

        Solution sol(var.c_str());

        sol.process();
        pod::GnssEpochMap map = sol.getData();

        return map;
    }
}
