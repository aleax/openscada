#ifndef TARHIVES_H
#define TARHIVES_H

#include <string>
#include <vector>

#include "terror.h"
#include "tmodule.h"
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
	TArhiveMess(string name, string categor ) : m_name(name) { };
	virtual ~TArhiveMess();
	
	string Name() { return(m_name); }
	
	TTipArhive &Owner() { return(*m_owner); }
    private:
	string         m_name;
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
	
	unsigned      OpenMess( string name, string categoris );
	void          CloseMess( unsigned int id );
	TArhiveMess   *atMess( unsigned int id );
	
	unsigned      OpenVal( string name, string bd );
	void          CloseVal( unsigned int id );
	TArhiveVal    *atVal( unsigned int id );

    /** Public atributes:: */
    public:
    /** Public atributes:: */
    private:
	virtual TArhiveMess *Mess(string name, string categories )
	{ throw TError("%s: Message arhiv no support!",o_name); }
	virtual TArhiveVal  *Val(string name, string bd )
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

class TArhiveS : public TGRPModule
{

    /** Public methods: */
    public:
	TArhiveS( TKernel *app );

	~TArhiveS(  );
	
    	TTipArhive &at_tp( unsigned id )      { return( (TTipArhive &)gmd_at(id) ); }
	TTipArhive &operator[]( unsigned id ) { return( at_tp(id) ); }

	int MessOpen( string name, string t_name, string categories );
	void MessClose( unsigned int id );
	unsigned MessNameToId( string name );
	TArhiveMess *Mess_at( unsigned int id );
	void MessList( vector<string> &list );
    
	int ValOpen( string name, string t_name, string bd );
	void ValClose( unsigned int id );
	unsigned ValNameToId( string name );
	TArhiveVal *Val_at( unsigned int id );
	void ValList( vector<string> &list );

	void gmd_CheckCommandLine( );
	void gmd_UpdateOpt();
    /** Privateds: */
    private:
	void pr_opt_descr( FILE * stream );
    
    /** Private atributes: */
    private:
	vector< SArhive > m_mess;
	vector< SArhive > m_val;
	
	static const char *o_name;
};

#endif // TARHIVES_H
