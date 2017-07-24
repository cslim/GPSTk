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
        static int getAllFiles(const string &dir, const string &subDir, list<string> &files);
        static bool getDirectory(const string& path, string& directory);
    };
}
#endif // ! POD_AUXILIARY_H

