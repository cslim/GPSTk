#include"FsUtils.h"
#include<regex>
#include<iostream>
#include<filesystem>

namespace fs = std::experimental::filesystem;

namespace pod
{

    void  FsUtils::getAllFilesInDir(const std::string &dir, std::list<std::string> &files)
    {
        std::list<fs::path> paths;
        getAllFilesInDir(dir, paths);

        for (auto &p : paths)
            files.push_back(p.string());
    }

    void  FsUtils::getAllFilesInDir(const std::string &dir, std::list<fs::path> &files)
    {
        files.clear();
        fs::path p(dir);
        
        if (!fs::exists(p))
        {
            std::string  message = "directory: " + dir + " doesn't exist.";
			std::cerr << message << std::endl;
            throw std::exception(message.c_str());
        }

        for (auto &p : fs::directory_iterator(dir))
            files.push_back(p.path());
    }

    void  FsUtils::getAllFilesInDir(const std::string &dir, const std::string &ext, std::list<std::string> &files)
    {
        std::list<fs::path> paths;
        getAllFilesInDir(dir, ext, paths);

        for (auto &p : paths)
            files.push_back(p.string());
    }

    void  FsUtils::getAllFilesInDir(const std::string &dir, const std::string &ext, std::list<fs::path> &files )
    {
		std::regex rx(ext);
        int totalUrls = 0;
        std::list<fs::path> paths;
        getAllFilesInDir(dir, paths);
        for (auto &p : paths)
        {
            if (p.has_extension())
            {
                std::string exti = p.extension().string();
                if (regex_match(exti, rx))
                    files.push_back(p);
            }
        }
    }
}
