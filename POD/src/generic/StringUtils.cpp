#include"StringUtils.h"
#include<regex>

using namespace gpstk;
using namespace std;
namespace pod
{
     std::string StringUtils::formatTime(const CommonTime &t)
     {
         const char* fmt = "%Y-%02m-%02d %02H:%02M:%05.3f";
         return CivilTime(t).printf(fmt);
     }
	 
	 bool StringUtils::tryParseTime(const std::string&  str, gpstk::CommonTime& time)
	 {
		 time = CommonTime::BEGINNING_OF_TIME;
		 const char* pattern = "([\\d]{4})-([\\d]{2})-([\\d]{2}) ([\\d]{2}):([\\d]{2}):([\\d.]{5,6})";
		 std::cmatch res;
		 std::regex rgx(pattern);
		 bool b = std::regex_search(str.c_str(), res, rgx);

		 if (b && res.size() >= 6)
		 {
			 int y = std::stoi(res[1]);
			 int m = std::stoi(res[2]);
			 int d = std::stoi(res[3]);
			 int h = std::stoi(res[4]);
			 int min = std::stoi(res[5]);
			 int sec = std::stod(res[6]);
			 time = CivilTime(y, m, d, h, min, sec, TimeSystem::Any);
			 return true;
		 }
		 return false;

	 }

	 std::vector<std::string> StringUtils::split(const std::string&  str, const std::string&  delim)
	 {
		 std::vector<std::string> res;
		 std::string token;
		 size_t last = 0; 
		 size_t next = 0; 
		 while ((next = str.find(delim, last)) != string::npos)
		 { 
			 auto word = str.substr(last, next - last);
			 if (!word.empty())
				 res.push_back(word);
			 last = next + 1; 
		 } 
		 auto word = str.substr(last, next - last);
		 if (!word.empty())
			 res.push_back(word);
		 return res;

	 }

}
