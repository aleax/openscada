
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
class TTipTransport;

class TTransportIn
{
    public:
	TTransportIn(string name, string address, string prot, TTipTransport *owner ) 
	    : m_name(name), m_address(address), m_prot(prot), m_owner(owner) { }
	virtual ~TTransportIn();

	string Name() { return(m_name); }
	
	TTipTransport &Owner() { return(*m_owner); }
    protected:
	string  m_name;
	string  m_address;
	string  m_prot;
    private:
	TTipTransport *m_owner;
	
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

	unsigned      OpenIn(string name, string address, string prot );
	void          CloseIn( unsigned int id );
	TTransportIn  *atIn( unsigned int id );

	unsigned      OpenOut(string name, string address );
	void          CloseOut( unsigned int id );
	TTransportOut *atOut( unsigned int id );

/** Public atributes:: */
    public:
/** Public atributes:: */
    private:
	virtual TTransportIn  *In(string name, string address, string prot )
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
    void gmd_Init( );
    /*
     * Load/Reload all BD and update internal controllers structure!
     */
    void LoadBD( );
    /*
     * Update all BD from current to external BD.
     */
    void UpdateBD( );

    TTipTransport &at_tp( unsigned id ) { return( (TTipTransport &)gmd_at(id) ); }
    TTipTransport &operator[]( unsigned id ) { return( at_tp(id) ); }
    
    int OpenIn( string name, string t_name, string address, string prot );
    void CloseIn( unsigned int id );
    unsigned NameInToId( string name );
    TTransportIn *at_in( unsigned int id );
    void ListIn( vector<string> &list );
    
    int OpenOut( string name, string t_name, string address );
    void CloseOut( unsigned int id );
    unsigned NameOutToId( string name );
    TTransportOut *at_out( unsigned int id );
    void ListOut( vector<string> &list );
    
    void gmd_CheckCommandLine( );
    void gmd_UpdateOpt();

/** Private methods: */
private:
    void pr_opt_descr( FILE * stream );

/** Private atributes: */
private:
    vector< STransp >        TranspIn;
    vector< STransp >        TranspOut;

    static SCfgFld        gen_elem[]; //Generic BD elements

    string t_bd;
    string n_bd;
    string n_tb;
    
    static const char     *o_name;
};

#endif // TTRANSPORTS_H
