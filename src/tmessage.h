
#ifndef TMESSAGE_H
#define TMESSAGE_H


class TMessage
{

/** Public methods: */
public:
    TMessage(  );

    ~TMessage(  );

    int Start(  );	
    void put( int level, char * fmt,  ... );
/** Private methods: */
 private:
    static void sighandler( int signal );
/**Attributes: */
private:
    int stop_signal;
      
};

#endif // TMESSAGE_H
