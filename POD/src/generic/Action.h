#ifndef POD_ACTION_H
#define POD_ACTION_H
#include"DataStructures.hpp"

namespace pod
{
    class Action
    {
    public :
        Action() {}
        std::vector<gpstk::gnssRinex> process();

        //std::vector<gpstk::gnssRinex> getData();
        

    };
}

#endif // !POD_ACTION_H
