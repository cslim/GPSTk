#ifndef POD_AUXILIARY_H
#define POD_AUXILIARY_H


#include<string>
#include<list>
#include<filesystem>

namespace pod
{
    namespace fs = std::experimental::filesystem;
    class FsUtils
    {
    public:
        static void  getAllFilesInDir(const std::string &dir, std::list<std::string> &files);
        static void  getAllFilesInDir(const std::string &dir, std::list<fs::path> &files);

        static void  getAllFilesInDir(const std::string &dir, const std::string &ext, std::list<std::string> &files);
        static void  getAllFilesInDir(const std::string &dir, const std::string &ext, std::list<fs::path> &files);
    };
}
#endif // ! POD_AUXILIARY_H

