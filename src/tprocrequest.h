
#ifndef TPROCREQUEST_H
#define TPROCREQUEST_H

#include "tapplication.h"
#include "tmessage.h"
#include "tgrpmodule.h"

#include <string>

class TProcRequest : public TGRPModule
{

/** Public methods: */
public:
     TProcRequest(  );

    /**
      * 
      * @param BufReq
      *        Client's request
      */
    int Request( string BufReq, string BufReply );

    void Init(  );

    void Start(  );

    void CheckCommandLine(  );

    /** Private methods: */
private:
    void pr_opt_descr( FILE * stream );
    
    /** Private atributes: */
private:
};

#endif // TPROCREQUEST_H
