
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

	string Name() { return(_name); }
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
	
	string Name() { return(_name); }
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
	void          CloseIn( unsigned int id );
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

struct STransp
{
    int use;
    unsigned type_tr;
    unsigned tr;
};

class TTransportS : public TGRPModule
{

/** Public methods: */
public:
    TTransportS( TKernel *app );
    ~TTransportS();

    TTipTransport *at_tp( string name ) { return(TTransport[NameToId(name)]); }
    
    int OpenIn( string t_name, string tt_name, string address );
    void CloseIn( unsigned int id );
    unsigned NameInToId( string name );
    TTransportIn *at_in( unsigned int id );
    void ListIn( vector<string> &list );
    
    int OpenOut( string tt_name, string t_name, string address );
    void CloseOut( unsigned int id );
    unsigned NameOutToId( string name );
    TTransportOut *at_out( unsigned int id );
    void ListOut( vector<string> &list );
    
    void CheckCommandLine( );
    void UpdateOpt();

/** Private methods: */
private:
    void pr_opt_descr( FILE * stream );
    virtual int AddM( TModule *modul );
    virtual void DelM( unsigned hd );	    

/** Private atributes: */
private:
    vector< TTipTransport *> TTransport;
    vector< STransp >        TranspIn;
    vector< STransp >        TranspOut;
    
    static const char     *o_name;
    static const char     *n_opt;    
};

#endif // TTRANSPORTS_H
