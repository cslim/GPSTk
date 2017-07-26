#include"Action.h"
#include"Solution.h"

namespace pod
{
    ///
    std::vector<gpstk::gnssRinex> Action:: process()
    {
        std::string  var = this->path;

        Solution sol(var.c_str());

        sol.process();
        auto list = sol.getData();
        std::vector<gpstk::gnssRinex> vect(list.size());
        int i = 0;
        for (auto &it : list)
        {
            vect[i] = it;
            i++;
        }
        return vect;
    }
}
