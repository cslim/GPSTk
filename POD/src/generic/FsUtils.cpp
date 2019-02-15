#include"FsUtils.h"
#include<regex>
#include<iostream>
#include<filesystem>

using namespace std;
namespace fs = std::experimental::filesystem;

namespace pod
{

    void  FsUtils::getAllFilesInDir(const string &dir, list<string> &files)
    {
        list<fs::path> paths;
        getAllFilesInDir(dir, paths);

        for (auto &p : paths)
            files.push_back(p.string());
    }

    void  FsUtils::getAllFilesInDir(const string &dir, list<fs::path> &files)
    {
        files.clear();
        fs::path p(dir);
        
        if (!fs::exists(p))
        {
            string  message = "directory: " + dir + " doesn't exist.";
			cerr << message << endl;
            throw std::exception(message.c_str());
        }

        for (auto &p : fs::directory_iterator(dir))
            files.push_back(p.path());
    }

    void  FsUtils::getAllFilesInDir(const string &dir, const string &ext, list<string> &files)
    {
        list<fs::path> paths;
        getAllFilesInDir(dir, ext, paths);

        for (auto &p : paths)
            files.push_back(p.string());
    }

    void  FsUtils::getAllFilesInDir(const string &dir, const string &ext, list<fs::path> &files )
    {
        regex rx(ext);
        int totalUrls = 0;
        list<fs::path> paths;
        getAllFilesInDir(dir, paths);
        for (auto &p : paths)
        {
            if (p.has_extension())
            {
                string exti = p.extension().string();
                if (regex_match(exti, rx))
                    files.push_back(p);
            }
        }
    }
}
