#include"auxiliary.h"
#include<filesystem>

namespace fs = std::experimental::filesystem;
namespace pod
{
    void  auxiliary::getAllFilesInDir(const string &dir, list<string> &files)
    {
        files.clear();

        for (auto &p : fs::directory_iterator(dir))
            files.push_back(p.path().string());
    }
}
