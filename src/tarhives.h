#ifndef TARHIVES_H
#define TARHIVES_H

#include <string>
#include <vector>

#include "terror.h"
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
        virtual void GetMess( time_t b_tm, time_t e_tm, vector<SBufRec> &mess, string category = "", char level = 0 ) { };
	virtual string GetCodePage( ) { return(Mess->GetCharset()); }
	virtual void SetCodePage( string codepage ) { }
	
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

	string Name() { return(m_name); }
	
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
	
	unsigned      OpenMess( string name, string addr, string categoris );
	void          CloseMess( unsigned int id );
	TArhiveMess   *atMess( unsigned int id );
	
	unsigned      OpenVal( string name, string bd );
	void          CloseVal( unsigned int id );
	TArhiveVal    *atVal( unsigned int id );

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
        unsigned hd_res;
	
        vector< TArhiveMess * > m_mess;
        vector< TArhiveVal * >  m_val;
	
	static const char *o_name;
};

//================================================================
//================ TArhiveS ======================================
//================================================================
struct SArhive
{
    bool     use;
    unsigned type;
    unsigned obj;
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
	
    	TTipArhive &at_tp( unsigned id )      { return( (TTipArhive &)gmd_at(id) ); }
	TTipArhive &operator[]( unsigned id ) { return( at_tp(id) ); }

	int MessOpen( string name, string t_name, string addr ,string categories );
	void MessClose( unsigned int id );
	unsigned MessNameToId( string name );
	TArhiveMess *Mess_at( unsigned int id );
	void MessList( vector<string> &list );
    
	int ValOpen( string name, string t_name, string bd );
	void ValClose( unsigned int id );
	unsigned ValNameToId( string name );
	TArhiveVal *Val_at( unsigned int id );
	void ValList( vector<string> &list );

	void gmd_Start( );
	void gmd_Stop( );

	void gmd_CheckCommandLine( );
	void gmd_UpdateOpt();
    /** Privateds: */
    private:
	void pr_opt_descr( FILE * stream );

	static void *MessArhTask(void *param);
	
	void gmd_DelM( unsigned hd );    
    /** Private atributes: */
    private:
	vector< SArhive > m_mess;
	vector< SArhive > m_val;
	
    	string t_bd;
	string n_bd;
	string n_tb;

	int       m_mess_per;       //Mmessage arhiving period
	pthread_t m_mess_pthr;
	bool      m_mess_r_stat;
	bool      m_mess_r_endrun;
	unsigned  hd_res;

	static SCfgFld    gen_elem[];  
	
	static const char *o_name;
};

#endif // TARHIVES_H
