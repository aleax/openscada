
#ifndef TPROTOCOLS_H
#define TPROTOCOLS_H

#define  VER_PROT 1    //ProtocolS type modules version

#include <string>

#include "tkernel.h"
#include "tmessage.h"
#include "tmodule.h"
#include "tgrpmodule.h"

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

    int gmd_Ver( ) { return(VER_PROT); }

    TProtocol &gmd_at( unsigned hd )     { return( (TProtocol &)TGRPModule::gmd_at(hd) ); }
    TProtocol &operator[]( unsigned hd ) { return( gmd_at(hd) ); }

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
