#include"Action.h"

namespace pod
{
    ///
    std::vector<gpstk::gnssRinex> process()
    {
        std::vector<gpstk::gnssRinex> gData;
        gData.push_back(gpstk::gnssRinex());
        gData.push_back(gpstk::gnssRinex());
        return gData;
    }
}
