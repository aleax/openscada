#ifndef TARHIVES_H
#define TARHIVES_H

#include <string>
#include <vector>

#include "terror.h"
#include "thd.h"
#include "tbds.h"
#include "tmodule.h"
#include "tconfig.h"
#include "tmessage.h"
#include "tgrpmodule.h"

using std::string;
using std::vector;

//================================================================
//=========== TArhiveMess ========================================
//================================================================
class TTipArhive;

class TArhiveMess
{
    public:
	TArhiveMess(string name, string addr, string categoris, TTipArhive *owner );
	virtual ~TArhiveMess();

	virtual void put( vector<SBufRec> &mess ){ };
        virtual void get( time_t b_tm, time_t e_tm, vector<SBufRec> &mess, string category = "", char level = 0 ) { };
	
	string &Name() { return(m_name); }
	string &Addr() { return(m_addr); }
	vector<string> &Categ() { return(m_categ); }
	
	TTipArhive &Owner() { return(*m_owner); }
    protected:
	string         m_name;
	string         m_addr;
	string         m_cat_o;
    private:
	vector<string> m_categ;
	TTipArhive     *m_owner;

	static const char *o_name;
};

//================================================================
//=========== TArhiveVal =========================================
//================================================================
class TArhiveVal
{
    public:
	TArhiveVal(string name, string bd, TTipArhive *owner ) 
	    : m_name(name), m_bd(bd), m_owner(owner) { }
	virtual ~TArhiveVal();

	string &Name() { return(m_name); }
	
	TTipArhive &Owner() { return(*m_owner); }
    protected:
	string  m_name;
	string  m_bd;
    private:
	TTipArhive *m_owner;
	
	static const char *o_name;
};

//================================================================
//=========== TTipArhive =========================================
//================================================================

class TTipArhive: public TModule
{
    /** Public methods: */
    public:
    	TTipArhive( );
	virtual ~TTipArhive();
		
	/*
	 * Avoid message list
	 */
	void mess_list( vector<string> &list ) 
	{ m_hd_mess.hd_obj_list( list ); }
	/*
	 * Add message arhive
	 */
	void mess_add( string name, string addr ,string categories );
	/*
	 * Del message arhive
	 */
	void mess_del( string name ) 
	{ delete (TArhiveMess *)m_hd_mess.hd_obj_del( name ); }
	/*
	 * Attach to message arhive
	 * Return arhive header
	 */
	unsigned mess_att( string name )
	{ return( m_hd_mess.hd_att( name ) ); }	
	/*
	 * Detach from message arhive
	 */
	void mess_det( unsigned hd )
	{ m_hd_mess.hd_det( hd ); }
	/*
	 * Get attached object
	 */
        TArhiveMess &mess_at( unsigned hd )
	{ return( *(TArhiveMess *)m_hd_mess.hd_at( hd ) ); }
	
	/*
	 * Avoid message list
	 */
	void val_list( vector<string> &list )
	{ m_hd_val.hd_obj_list( list ); }
	/*
	 * Add message arhive
	 */
	void val_add( string name, string bd );
	/*
	 * Del message arhive
	 */
        void val_del( string name )
	{ delete (TArhiveVal *)m_hd_val.hd_obj_del( name ); }
	/*
	 * Attach to message arhive
	 * Return arhive header
	 */
        unsigned val_att( string name )
	{ return( m_hd_val.hd_att( name ) ); }
	/*
	 * Detach from message arhive
	 */
        void val_det( unsigned hd )
	{ m_hd_val.hd_det( hd ); }
	/*
	 * Get attached object
	 */
        TArhiveVal &val_at( unsigned hd )
	{ return( *(TArhiveVal *)m_hd_val.hd_at( hd ) ); }
    /** Public atributes:: */
    public:
    /** Public atributes:: */
    private:
	virtual TArhiveMess *AMess(string name, string addr, string categories )
	{ throw TError("%s: Message arhiv no support!",o_name); }
	virtual TArhiveVal  *AVal(string name, string bd )
	{ throw TError("%s: Value arhiv no support!",o_name); }
    /** Private atributes:: */
    private:
        //unsigned hd_res;
	
        THD    m_hd_mess; 
        THD    m_hd_val; 
	
	static const char *o_name;
};

//================================================================
//================ TArhiveS ======================================
//================================================================

class SArhS
{
    public:
	SArhS( string m_tp, string m_obj ) : tp(m_tp), obj(m_obj) { }
    	string tp;
	string obj;
};

struct SHDArh
{
    unsigned h_tp;
    unsigned h_obj;
};

class TArhiveS : public TGRPModule, public TConfig 
{
    /** Public methods: */
    public:
	TArhiveS( TKernel *app );

	~TArhiveS(  );
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
	
    	TTipArhive &gmd_at( unsigned hd )     { return( (TTipArhive &)TGRPModule::gmd_at(hd) ); }
	TTipArhive &operator[]( unsigned hd ) { return( gmd_at(hd) ); }

	/*
	 * Avoid message list
	 */
	void mess_list( vector<SArhS> &list );
	/*
	 * Add message arhive
	 */
	void mess_add( SArhS arh, string addr ,string categories );
	/*
	 * Del message arhive
	 */
	void mess_del( SArhS arh );
	/*
	 * Attach to message arhive
	 * Return arhive header
	 */
	SHDArh mess_att( SArhS arh );
	/*
	 * Detach from message arhive
	 */
	void mess_det( SHDArh &hd );
	/*
	 * Get attached object
	 */
        TArhiveMess &mess_at( SHDArh &hd ) { return( gmd_at( hd.h_tp ).mess_at( hd.h_obj ) ); }
	
	/*
	 * Avoid message list
	 */
	void val_list( vector<SArhS> &list );
	/*
	 * Add message arhive
	 */
	void val_add( SArhS arh, string bd );
	/*
	 * Del message arhive
	 */
	void val_del( SArhS arh );
	/*
	 * Attach to message arhive
	 * Return arhive header
	 */
	SHDArh val_att( SArhS arh );
	/*
	 * Detach from message arhive
	 */
	void val_det( SHDArh &hd );
	/*
	 * Get attached object
	 */
        TArhiveVal &val_at( SHDArh &hd ) { return( gmd_at( hd.h_tp ).val_at( hd.h_obj ) ); }

	void gmd_Start( );
	void gmd_Stop( );

	void gmd_CheckCommandLine( );
	void gmd_UpdateOpt();

	SBDS &GenB() { return(m_bd); }
    /** Privateds: */
    private:
	void pr_opt_descr( FILE * stream );

	static void *MessArhTask(void *param);
	
	void gmd_del( string name );    
    /** Private atributes: */
    private:	
	SBDS   m_bd;

	int       m_mess_per;       //Mmessage arhiving period
	pthread_t m_mess_pthr;
	bool      m_mess_r_stat;
	bool      m_mess_r_endrun;
	//unsigned  hd_res;

	static SCfgFld    gen_elem[];  
	
	static const char *o_name;
};

#endif // TARHIVES_H
