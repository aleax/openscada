
#ifndef TTRANSPORTS_H
#define TTRANSPORTS_H

#define  VER_TR 1    //TransportS type modules version

#include <string>

#include "tbds.h"
#include "tconfig.h"
#include "tmodule.h"
#include "tgrpmodule.h"

using std::string;
//================================================================
//=========== TTransportIn =======================================
//================================================================
class TTipTransport;

class TTransportIn : public TContr, public TConfig
{
    public:
	TTransportIn(string name, TTipTransport *owner );
	virtual ~TTransportIn();

	virtual void start(){};
	virtual void stop(){};

	string &Name() { return(m_name); }
	
	TTipTransport &Owner() { return(*m_owner); }
    protected:
	//================== Controll functions ========================
	void ctr_fill_info( XMLNode *inf );
	void ctr_din_get_( string a_path, XMLNode *opt );
	void ctr_din_set_( string a_path, XMLNode *opt );
	void ctr_cmd_go_( string a_path, XMLNode *fld, XMLNode *rez );
    protected:
	string  &m_name;
	string  &m_lname;
	string  &m_addr;
	string  &m_prot;

	bool    run_st;
    private:
	TTipTransport *m_owner;
	
	static const char *o_name;
	static const char *i_cntr;
};

//================================================================
//=========== TTransportOut ======================================
//================================================================
class TTransportOut : public TContr, public TConfig
{
    public:
	TTransportOut(string name, TTipTransport *owner );
	virtual ~TTransportOut();
	
	virtual void start(){};
	virtual void stop(){};
	
	virtual int IOMess(char *obuf, int len_ob, char *ibuf = NULL, int len_ib = 0, int time = 0 )
	{ return(0); }
	
	string &Name() { return(m_name); }
	
	TTipTransport &Owner() { return(*m_owner); }
    protected:
	//================== Controll functions ========================
	void ctr_fill_info( XMLNode *inf );
	void ctr_din_get_( string a_path, XMLNode *opt );
	void ctr_din_set_( string a_path, XMLNode *opt );
	void ctr_cmd_go_( string a_path, XMLNode *fld, XMLNode *rez );
    protected:
	string  &m_name;
	string  &m_lname;
	string  &m_addr;
	
	bool    run_st;
    private:
	TTipTransport *m_owner;
	
	static const char *o_name;
	static const char *i_cntr;
};

//================================================================
//=========== TTipTransport ======================================
//================================================================
class TTipTransport: public TModule
{
/** Public methods: */
    public:
    	TTipTransport( );
	virtual ~TTipTransport();

	// Avoid input transports list
	void in_list( vector<string> &list )
	{ m_hd_in.obj_list( list ); }
    	// Add input transport
	void in_add( string name );
	// Del input transport
	void in_del( string name )
	{ delete (TTransportIn *)m_hd_in.obj_del( name ); }
	/*
	 * Attach to input transport
	 * Return input transport header
	 */
	unsigned in_att( string name )
	{ return( m_hd_in.hd_att( name ) ); }
	// Detach from input transport
	void in_det( unsigned hd )
	{ m_hd_in.hd_det( hd ); }
	// Get attached input transport
	TTransportIn &in_at( unsigned hd )
	{ return( *(TTransportIn *)m_hd_in.hd_at( hd ) ); }											    

	// Avoid output transports list
	void out_list( vector<string> &list )
	{ m_hd_out.obj_list( list ); }
    	// Add output transport
	void out_add( string name );
	// Del output transport
	void out_del( string name )
	{ delete (TTransportOut *)m_hd_out.obj_del( name ); }
	/*
	 * Attach to output transport
	 * Return output transport header
	 */
	unsigned out_att( string name )
	{ return( m_hd_out.hd_att( name ) ); }
	// Detach from output transport
	void out_det( unsigned hd )
	{ m_hd_out.hd_det( hd ); }
	// Get attached output transport
	TTransportOut &out_at( unsigned hd )
	{ return( *(TTransportOut *)m_hd_out.hd_at( hd ) ); }											    

/** Public atributes:: */
    public:
	
    protected:
	//================== Controll functions ========================
	void ctr_fill_info( XMLNode *inf );
	void ctr_din_get_( string a_path, XMLNode *opt );
	void ctr_din_set_( string a_path, XMLNode *opt );
	unsigned ctr_att( string br );
	void     ctr_det( string br, unsigned hd );
	TContr  &ctr_at( string br, unsigned hd );
/** Public atributes:: */
    private:
	virtual TTransportIn  *In( string name )
	{ throw TError("(%s) Input transport no support!",o_name); }
	virtual TTransportOut *Out( string name )
	{ throw TError("(%s) Output transport no support!",o_name); }
/** Private atributes:: */
    private:
        //unsigned hd_res;
	
	THD    m_hd_in;
	THD    m_hd_out;
	
	static const char *o_name;
	static const char *i_cntr;
};

//================================================================
//=========== TTransportS ========================================
//================================================================

class STrS
{
    public:
	STrS( string m_tp, string m_obj ) : tp(m_tp), obj(m_obj) { }
    	string tp;
	string obj;
};

struct SHDTr
{
    unsigned h_tp;
    unsigned h_obj;
};

class TTransportS : public TGRPModule, public TConfigElem
{
    /** Public methods: */
    public:
     	TTransportS( TKernel *app );
	~TTransportS();

	int gmd_Ver( ) { return(VER_TR); }
	// Init All transport's modules
	void gmd_Init( );
	// Start all transports
	void gmd_Start( );
	// Stop all transports
	void gmd_Stop( );
	
	// Load/Reload all BD and update internal controllers structure!
	void LoadBD( );
	// Update all BD from current to external BD.
	void UpdateBD( );

	TTipTransport &gmd_at( unsigned hd ) { return( (TTipTransport &)TGRPModule::gmd_at(hd) ); }
	TTipTransport &operator[]( unsigned hd ) { return( gmd_at(hd) ); }

	// Avoid input transports list
	void in_list( vector<STrS> &list );
	// Add input transports 
	void in_add( STrS tr );
	// Del input transports
	void in_del( STrS tr );
	/*
	 * Attach to input transports
    	 * Return input transports header
	 */
	SHDTr in_att( STrS tr );
	// Detach from input transports
	void in_det( SHDTr &hd );
        // Get attached input transports
	TTransportIn &in_at( SHDTr &hd ) { return( gmd_at( hd.h_tp ).in_at( hd.h_obj ) ); }
	
	// Avoid output transports list
	void out_list( vector<STrS> &list );
	// Add output transport 
	void out_add( STrS tr );
	// Del output transport
	void out_del( STrS tr );
	/*
	 * Attach to output transport
    	 * Return output transports header
	 */
	SHDTr out_att( STrS tr );
	// Detach from output transport
	void out_det( SHDTr &hd );
        // Get attached output transports
	TTransportOut &out_at( SHDTr &hd ) { return( gmd_at( hd.h_tp ).out_at( hd.h_obj ) ); }
	
	void gmd_CheckCommandLine( );
	void gmd_UpdateOpt();
    /** Private methods: */
    private:
    	string opt_descr( );
	
	void gmd_del( string name );
	//================== Controll functions ========================
	void ctr_fill_info( XMLNode *inf );
	void ctr_din_get_( string a_path, XMLNode *opt );
	void ctr_din_set_( string a_path, XMLNode *opt );
	void ctr_cmd_go_( string a_path, XMLNode *fld, XMLNode *rez );
    /** Private atributes: */
    private:
	static SCfgFld        gen_elem[]; //Generic BD elements

	SBDS   m_bd;
    
	static const char *i_cntr;
	static const char *o_name;
};

#endif // TTRANSPORTS_H
