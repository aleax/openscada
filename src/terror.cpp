#include <stdarg.h>

#include "tmessage.h"
#include "tapplication.h"
#include "terror.h"


TError::TError( string descr )
{
    err = descr;
}

TError::TError( char *fmt, ... )
{
    char str[256];                  //!!!!
    va_list argptr;

    va_start(argptr,fmt);
    vsprintf(str,fmt,argptr);
    va_end(argptr);

    err = str;
//    App->Mess->put(7,(char *)desc.c_str());
}        

