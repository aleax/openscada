#ifndef TMESSAGE_H
#define TMESSAGE_H

#include <string>
using std::string;
#include <exception>
using std::exception;

class TMessage
{

/** Public methods: */
public:
    TMessage(  );

    ~TMessage(  );

    int Start(  );	

    int Sconv(const char *fromCH, const char *toCH, string & buf);
    int SconvIn(const char *fromCH, string & buf);
    int SconvOut(const char *toCH, string & buf);
    void SetCharset(string charset) { IOCharSet = charset; }
    void SetDLevel(int level)       { d_level   = level; }
    void SetLogDir(int dir)         { log_dir   = dir; }

    void put( int level, char * fmt,  ... );
    
/** Private methods: */
 private:
    static void sighandler( int signal );
    
/**Attributes: */
private:
    int    stop_signal;
    string IOCharSet;      //Internal charset
    int    d_level;        //Debug level
    int    log_dir;        //Log direction
};

extern TMessage *Mess;

#endif // TMESSAGE_H
