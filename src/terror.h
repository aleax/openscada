
#ifndef TERROR_H
#define TERROR_H

#include <string>
#include <exception>


using std::string;
using std::exception;

class TError
{
public:
    TError( string descr );
    TError( char *fmt, ... );
    string what() { return(err); }
public:
    string  err;
};


#endif // TERROR_H
