#ifndef POD_ACTION_H
#define POD_ACTION_H
#include"DataStructures.hpp"

namespace pod
{
    class Action
    {
    public :
        Action(std::string pathToConfig):path(pathToConfig) {}
        std::vector<gpstk::gnssRinex> process();

        //std::vector<gpstk::gnssRinex> getData();
    protected:
        std::string path;

    };
}

#endif // !POD_ACTION_H
