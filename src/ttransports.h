
#ifndef TTRANSPORTS_H
#define TTRANSPORTS_H

#include <string>
using std::string;

#include "tmodule.h"
#include "tgrpmodule.h"

//================================================================
//=========== TTransportIn =======================================
//================================================================
class TTransportIn
{
    public:
	TTransportIn(){ };
	virtual ~TTransportIn();

    private:

	static const char *o_name;
};

//================================================================
//=========== TTransportOut ======================================
//================================================================
class TTransportOut
{
    public:
	TTransportOut(){ };
	virtual ~TTransportOut();

    private:

	static const char *o_name;
};

//================================================================
//=========== TTipTransport ======================================
//================================================================
class TTransportS;

class TTipTransport: public TModule
{
/** Public methods: */
    public:
    	TTipTransport( ) { };
	virtual ~TTipTransport();

	//int OpenIn(string name, string address );
	//int OpenOut(string name, string address );

	//TTransportIn  *atIn( unsigned int id );
	//TTransportOut *atOut( unsigned int id );

/** Public atributes:: */
    public:
	TTransportS *owner;
/** Private atributes:: */
    private:
	static const char *o_name;
};


//================================================================
//=========== TTransportS ========================================
//================================================================
class TTransportS : public TGRPModule
{

/** Public methods: */
public:
    TTransportS( TKernel *app );
    ~TTransportS();

    void CheckCommandLine( char **argv, int argc );
    void UpdateOpt();

/** Private methods: */
private:
    void pr_opt_descr( FILE * stream );
    virtual int AddM( TModule *modul );
    virtual int DelM( int hd );	    

/** Private atributes: */
private:
    vector< TTipTransport *>     TTransport;
    
    static const char     *o_name;
    static const char     *n_opt;    
};

#endif // TTRANSPORTS_H
