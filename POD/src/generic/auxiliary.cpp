#include"auxiliary.h"
#include<filesystem>

namespace fs = std::experimental::filesystem;
namespace pod
{
    void  auxiliary::getAllFilesInDir(const string &dir, list<string> &files)
    {
        files.clear();
        fs::path p(dir);
        if (!fs::exists(p))
        {
            string  message = "directory: " + dir + " doesn't exist.";
            throw std::exception(message.c_str());
        }
         

        for (auto &p : fs::directory_iterator(dir))
            files.push_back(p.path().string());
    }
}
