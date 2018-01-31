#ifndef POD_WINUTIPS_H
#define POD_WINUTIPS_H

#include<Windows.h>
#include <iostream>

///print to Output window
#define DBOUT( s )            \
{                             \
   std::ostringstream os_;    \
   os_ << s;                   \
   OutputDebugString( os_.str().c_str() );  \
}
#define DBOUT_LINE( s )            \
{                             \
   std::ostringstream os_;    \
   os_ << s;                   \
   OutputDebugString( os_.str().c_str() );  \
 OutputDebugString( "\r\n" );\
}

#endif // !POD_WINUTIPS_H

