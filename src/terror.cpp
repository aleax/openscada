#include <stdarg.h>
#include <stdio.h>

#include "terror.h"

TError::TError( string descr )
{
    err = descr;
}

TError::TError( char *fmt, ... )
{
    char str[1024];                  //!!!!
    va_list argptr;

    va_start(argptr,fmt);
    vsnprintf(str,sizeof(str),fmt,argptr);
    va_end(argptr);

    err = str;
//    App->Mess->put(7,(char *)desc.c_str());
}        

