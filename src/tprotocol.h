
#ifndef TPROTOCOL_H
#define TPROTOCOL_H

#include "tapplication.h"
#include "tmessage.h"
#include "tgrpmodule.h"

#include <string>
using std::string;

class TProtocol : public TGRPModule
{

/** Public methods: */
public:
     TProtocol(  );

    /**
      * 
      * @param BufReq
      *        Client's request
      */
//    int Request( string BufReq, string BufReply );

//    void Start(  );

    void CheckCommandLine(  );
    void UpdateOpt();

    /** Private methods: */
private:
    void pr_opt_descr( FILE * stream );
    
    /** Private atributes: */
private:
};

#endif // TPROTOCOL_H
