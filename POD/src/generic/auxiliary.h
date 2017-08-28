#ifndef POD_AUXILIARY_H
#define POD_AUXILIARY_H

#include<string>
#include<list>
using namespace std;
namespace pod
{
    class auxiliary
    {
    public:
        static void  auxiliary::getAllFilesInDir(const string &dir, list<string> &files);
    };
}
#endif // ! POD_AUXILIARY_H

