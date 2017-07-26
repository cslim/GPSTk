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
            for (auto &it1 : it.body)
            {
                auto  ts1 = TypeID(TypeID::S1);
                auto  ts2 = TypeID(TypeID::S2);

                double s1 = it1.second.getValue(ts1);
                it1.second.at(ts1) = 20.0*log10(s1);

                double s2 = it1.second.getValue(ts2);
                it1.second.at(ts2) = 20.0*log10(s2);
            }

            vect[i] = it;
            i++;
        }
        return vect;
    }
}
