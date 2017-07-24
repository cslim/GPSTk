#include"auxiliary.h"
#include<windows.h>
#include <direct.h>
namespace pod
{
    int  auxiliary::getAllFiles(const string &dir, const string &subDir, list<string> &files)
    {
        files.clear();

        //dir of interest
        string path = dir + "\\" + subDir + "\\*";

        WIN32_FIND_DATA FindFileData;
        HANDLE hf;

        hf = FindFirstFile(path.c_str(), &FindFileData);
        if (hf != INVALID_HANDLE_VALUE)
        {
            do
            {
                string fpath = dir + "\\" + subDir + "\\" + FindFileData.cFileName;
                files.push_back(fpath);
            } while (FindNextFile(hf, &FindFileData) != 0);
            FindClose(hf);
        }
        if (files.size() < 3) return 0;

        //удалим "." и ".."
        files.pop_front();
        files.pop_front();
        return 1;
    }
    
    bool auxiliary::getDirectory(const string& path, string& directory )
    {
        size_t found = path.find_last_of("/\\");
        if (found == string::npos) return false;
        if (found == path.size() - 1) return false;
        directory = path.substr(0, found);
        return true;
    }
}
