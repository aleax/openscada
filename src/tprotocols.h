
#ifndef TPROTOCOLS_H
#define TPROTOCOLS_H

#include "tkernel.h"
#include "tmessage.h"
#include "tgrpmodule.h"

#include <string>
using std::string;

class TProtocolS : public TGRPModule
{

/** Public methods: */
public:
     TProtocolS( TKernel *app );

    /**
      * 
      * @param BufReq
      *        Client's request
      */
//    int Request( string BufReq, string BufReply );

//    void Start(  );

    void CheckCommandLine( char **argv, int argc );
    void UpdateOpt();

    /** Private methods: */
private:
    void pr_opt_descr( FILE * stream );
    
    /** Private atributes: */
private:
};

#endif // TPROTOCOLS_H
