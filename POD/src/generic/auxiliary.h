#ifndef POD_AUXILIARY_H
#define POD_AUXILIARY_H

#include<string>
#include<list>
using namespace std;

#include <Windows.h>
#include <iostream>
#include <sstream>

#define DBOUT( s )            \
{                             \
   std::ostringstream os_;    \
   os_ << s;                   \
   OutputDebugString( os_.str().c_str() );  \
}

namespace pod
{

    class auxiliary
    {
    public:
        static void  auxiliary::getAllFilesInDir(const string &dir, list<string> &files);
    };
}
#endif // ! POD_AUXILIARY_H

