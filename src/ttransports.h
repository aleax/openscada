
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
	TTransportIn(string name, string address) 
	    : _name(name), _address(address) { };
	virtual ~TTransportIn();

    private:
	string  _name;
	string  _address;

	static const char *o_name;
};

//================================================================
//=========== TTransportOut ======================================
//================================================================
class TTransportOut
{
    public:
	TTransportOut(string name, string address) 
	    : _name(name), _address(address) { };
	virtual ~TTransportOut();

    private:
	string  _name;
	string  _address;

	static const char *o_name;
};

//================================================================
//=========== TTipTransport ======================================
//================================================================
struct STransIn
{
    int          use;
    TTransportIn *tr;
};

struct STransOut
{
    int           use;
    TTransportOut *tr;
};

class TTransportS;

class TTipTransport: public TModule
{
/** Public methods: */
    public:
    	TTipTransport( );
	virtual ~TTipTransport();

	unsigned      OpenIn(string name, string address );
	void          CloseIN( unsigned int id );
	TTransportIn  *atIn( unsigned int id );

	unsigned      OpenOut(string name, string address );
	void          CloseOut( unsigned int id );
	TTransportOut *atOut( unsigned int id );

/** Public atributes:: */
    public:
	TTransportS *owner;
/** Public atributes:: */
    private:
	virtual TTransportIn  *In(string name, string address )
	{ throw TError("%s: Input transport no support!",o_name); }
	virtual TTransportOut *Out(string name, string address )
	{ throw TError("%s: Output transport no support!",o_name); }
/** Private atributes:: */
    private:
        unsigned hd_res;
	
        vector< STransIn >  i_tr;
        vector< STransOut > o_tr;
	
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
    virtual void DelM( unsigned hd );	    

/** Private atributes: */
private:
    vector< TTipTransport *>     TTransport;
    
    static const char     *o_name;
    static const char     *n_opt;    
};

#endif // TTRANSPORTS_H
