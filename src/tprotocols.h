
#ifndef TPROTOCOLS_H
#define TPROTOCOLS_H

#include "tkernel.h"
#include "tmessage.h"
#include "tmodule.h"
#include "tgrpmodule.h"

#include <string>
using std::string;

//================================================================
//=========== TProtocol ==========================================
//================================================================
class TProtocolS;

class TProtocol: public TModule
{
    /** Public methods: */
    public:
	TProtocol( );
	virtual ~TProtocol();
	
	/*
	 * process input messages
	 */
	virtual void in_mess(string &request, string &answer )
	{ answer = ""; }
    private:
	static const char *o_name;
};

//================================================================
//=========== TProtocolS =========================================
//================================================================

class TProtocolS : public TGRPModule
{

/** Public methods: */
public:
    TProtocolS( TKernel *app );

    TProtocol &at_tp( unsigned id ) { return( (TProtocol &)gmd_at(id) ); }
    TProtocol &operator[]( unsigned id ) { return( at_tp(id) ); }

    void gmd_CheckCommandLine( );
    void gmd_UpdateOpt();

    /** Private methods: */
private:
    void pr_opt_descr( FILE * stream );
    
    /** Private atributes: */
private:
    static const char *o_name;
};

#endif // TPROTOCOLS_H
