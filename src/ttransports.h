
#ifndef TTRANSPORTS_H
#define TTRANSPORTS_H

#include <string>
using std::string;

#include "tconfig.h"
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
	
	virtual int IOMess(char *obuf, int len_ob, char *ibuf = NULL, int len_ib = 0, int time = 0 )
	{ return(0); }
	
	string Name() { return(_name); }
    private:
	string  _name;
	string  _address;

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
	
        vector< TTransportIn * >  i_tr;
        vector< TTransportOut * > o_tr;
	
	static const char *o_name;
};


//================================================================
//=========== TTransportS ========================================
//================================================================

struct STransp
{
    bool     use;
    unsigned type_tr;
    unsigned tr;
};

class TTransportS : public TGRPModule, public TConfig
{

/** Public methods: */
public:
    TTransportS( TKernel *app );
    ~TTransportS();
    /*
     * Init All transport's modules
     */
    void gmd_InitAll( );
    /*
     * Load/Reload all BD and update internal controllers structure!
     */
    void LoadBD( );
    /*
     * Update all BD from current to external BD.
     */
    void UpdateBD( );

    TTipTransport *at_tp( string name ) { return(TTransport[gmd_NameToId(name)]); }
    
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
    
    void gmd_CheckCommandLine( );
    void gmd_UpdateOpt();

/** Private methods: */
private:
    void pr_opt_descr( FILE * stream );
    virtual int  gmd_AddM( TModule *modul );
    virtual void gmd_DelM( unsigned hd );	    

/** Private atributes: */
private:
    vector< TTipTransport *> TTransport;
    vector< STransp >        TranspIn;
    vector< STransp >        TranspOut;

    static SCfgFld        gen_elem[]; //Generic BD elements
    TConfigElem           gen_ecfg;

    string t_bd;
    string n_bd;
    string n_tb;
    
    static const char     *o_name;
    static const char     *n_opt;    
};

#endif // TTRANSPORTS_H
