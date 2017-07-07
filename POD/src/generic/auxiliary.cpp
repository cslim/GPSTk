#include"auxiliary.h"
#include<windows.h>
#include <direct.h>
namespace POD
{
    int  auxiliary::getAllFiles(string &subDir, list<string> &files)
    {
        files.clear();

        //get application dir
        char current_work_dir[_MAX_FNAME];
        _getcwd(current_work_dir, sizeof(current_work_dir));
        string s_dir(current_work_dir);
        //dir of interest
        string path = s_dir + "\\" + subDir + "\\*";

        WIN32_FIND_DATA FindFileData;
        HANDLE hf;

        hf = FindFirstFile(path.c_str(), &FindFileData);
        if (hf != INVALID_HANDLE_VALUE)
        {
            do
            {
                string fpath = s_dir + "\\" + subDir + "\\" + FindFileData.cFileName;
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
}
