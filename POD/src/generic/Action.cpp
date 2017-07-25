#include"Action.h"
#include"Solution.h"

namespace pod
{
    ///
    std::vector<gpstk::gnssRinex> Action:: process()
    {
        std::string  var = this->path;
        Solution sol(var.c_str());


        std::vector<gpstk::gnssRinex> gData;
        gData.push_back(gpstk::gnssRinex());
        gData.push_back(gpstk::gnssRinex());
        return gData;
    }
}
