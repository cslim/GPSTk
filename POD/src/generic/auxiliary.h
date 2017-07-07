#ifndef POD_AUXILIARY_H
#define POD_AUXILIARY_H

#include<string>
#include<list>
using namespace std;
namespace POD
{
    class auxiliary
    {
    public:
        static int getAllFiles(string &subDir, list<string> &files);

    };
}
#endif // ! POD_AUXILIARY_H

