
#ifndef TPROTOCOLS_H
#define TPROTOCOLS_H

#include "tkernel.h"
#include "tmessage.h"
#include "tmodule.h"
#include "tgrpmodule.h"

#include <string>
using std::string;

//================================================================
//=========== TTipProtocol =======================================
//================================================================
class TProtocolS;

class TTipProtocol: public TModule
{
    /** Public methods: */
    public:
	TTipProtocol( );
	virtual ~TTipProtocol();
	
	/*
	 * process input messages
	 */
	virtual void in_mess(string &request, string &answer )
	{ answer = ""; }
    public:
	TProtocolS *owner;	
    private:
	static const char *o_name;
};

//================================================================
//=========== TProtocolS =========================================
//================================================================
class TTipProtocol;

class TProtocolS : public TGRPModule
{

/** Public methods: */
public:
    TProtocolS( TKernel *app );

    TTipProtocol *at_tp( unsigned id ) { return( (TTipProtocol *)gmd_at(id) ); }
    TTipProtocol *operator[]( unsigned id ) { return( at_tp(id) ); }

    void gmd_CheckCommandLine( );
    void gmd_UpdateOpt();

    /** Private methods: */
private:
    void pr_opt_descr( FILE * stream );
    virtual int  gmd_AddM( TModule *modul );
    virtual void gmd_DelM( unsigned hd );
    
    /** Private atributes: */
private:
};

#endif // TPROTOCOLS_H
