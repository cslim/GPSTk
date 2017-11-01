#ifndef POD_AUXILIARY_H
#define POD_AUXILIARY_H

#include<string>
#include<list>
#include<filesystem>

#include <Windows.h>
#include <iostream>
#include <sstream>

#define DBOUT( s )            \
{                             \
   std::ostringstream os_;    \
   os_ << s;                   \
   OutputDebugString( os_.str().c_str() );  \
}

namespace fs = std::experimental::filesystem;
namespace pod
{
    class auxiliary
    {
    public:
        static void  getAllFilesInDir(const std::string &dir, std::list<std::string> &files);
        static void  getAllFilesInDir(const std::string &dir, std::list<fs::path> &files);

        static void  getAllFilesInDir(const std::string &dir, const std::string &ext, std::list<std::string> &files);
        static void  getAllFilesInDir(const std::string &dir, const std::string &ext, std::list<fs::path> &files);
    };
}
#endif // ! POD_AUXILIARY_H

