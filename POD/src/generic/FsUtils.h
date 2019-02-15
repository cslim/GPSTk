#ifndef POD_FS_UTILS_H
#define POD_FS_UTILS_H


#include<string>
#include<list>
#include<filesystem>

namespace pod
{
    class FsUtils
    {
    public:
        static void  getAllFilesInDir(const std::string &dir, std::list<std::string> &files);
        static void  getAllFilesInDir(const std::string &dir, std::list<std::experimental::filesystem::path> &files);

        static void  getAllFilesInDir(const std::string &dir, const std::string &ext, std::list<std::string> &files);
        static void  getAllFilesInDir(const std::string &dir, const std::string &ext, std::list<std::experimental::filesystem::path> &files);
    };
}

#endif // ! POD_FS_UTILS_H

