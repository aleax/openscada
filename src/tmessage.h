
#ifndef TMESSAGE_H
#define TMESSAGE_H

#include <string>

class TMessage
{

/** Public methods: */
public:
    TMessage(  );

    ~TMessage(  );

    int Start(  );	

    int Sconv(const char *fromCH, const char *toCH, string & buf);

    void put( int level, char * fmt,  ... );
/** Private methods: */
 private:
    static void sighandler( int signal );
/**Attributes: */
private:
    int stop_signal;
      
};

#endif // TMESSAGE_H
