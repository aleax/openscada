#ifndef TSEQURITY_H
#define TSEQURITY_H

#include "tbds.h"
#include "thd.h"
#include "tcontr.h"
#include "tconfig.h"

#define SEQ_RD 0x01
#define SEQ_WR 0x02
#define SEQ_XT 0x04

class TKernel;
class XMLNode;

class TSequrity;

class TUser : public TContr, public TConfig 
{
    /** Public methods: */
    public:
	TUser( TSequrity *owner, string name, unsigned id);
	~TUser(  );
	
	string   &Name()  { return(m_name); }
        int      &Id()    { return(m_id); }
	string   &Descr() { return(m_lname); }
	bool     Auth( string pass ){ return( (m_pass == pass)?true:false ); }
	
	void Name( string name )  { m_name = name; }
	void Id( unsigned id )    { m_id = id; }
	void Descr( string name ) { m_lname = name; }
	void Pass( string pass )  { m_pass = pass; }
	
	TSequrity &Owner(){ return(*m_owner); }
	//================== Controll functions ========================
	void ctr_cmd_go( string a_path, XMLNode *fld, XMLNode *rez );
    private:	    
	//================== Controll functions ========================
	void ctr_fill_info( XMLNode *inf );
	void ctr_din_get_( string a_path, XMLNode *opt );
	void ctr_din_set_( string a_path, XMLNode *opt );
    /** Private atributes: */
    private:	
       	TSequrity *m_owner;

	string    &m_name;
	string    &m_lname;
	string    &m_pass;
	int       &m_id;
	
	static const char   *i_cntr;
};

class TGroup : public TContr, public TConfig
{
    /** Public methods: */
    public:
	TGroup( TSequrity *owner, string name, unsigned id );
	~TGroup(  );

	string &Name()    { return(m_name); }
        unsigned Id()     { return(m_id); }
	string   &Descr() { return(m_lname); }
	
	void Name( string name )  { m_name = name; }
	void Id( unsigned id )    { m_id = id; }
	void Descr( string name ) { m_lname = name; }

	bool user( string name );
	
	TSequrity &Owner(){ return(*m_owner); }
    private:	    
	//================== Controll functions ========================
	void ctr_fill_info( XMLNode *inf );
	void ctr_din_get_( string a_path, XMLNode *opt );
	void ctr_din_set_( string a_path, XMLNode *opt );
    /** Private atributes: */
    private:
	string    &m_name;
	string    &m_lname;
	string    &m_usrs;
	int       &m_id;

       	TSequrity *m_owner;
	
	static const char   *i_cntr;
};

class TSequrity : public TContr, public TConfigElem
{
    /** Public methods: */
    public:
	TSequrity( TKernel *app );    
	~TSequrity(  );

	string Name(){ return(s_name); }
	
	void Init( );

	bool access( string user, char mode, int owner, int group, int access );
	
	string usr( int id );
	// Avoid users list
	void usr_list( vector<string> &list ) 
	{ m_hd_usr.obj_list( list ); }
	// Add user
	void usr_add( string name );
	// Del user
	void usr_del( string name ) 
	{ delete (TUser *)m_hd_usr.obj_del( name ); }
	// Attach to user
	unsigned usr_att( string name )
	{ return( m_hd_usr.hd_att( name ) ); }	
	// Detach from user
	void usr_det( unsigned hd )
	{ m_hd_usr.hd_det( hd ); }
	// Get attached object
        TUser &usr_at( unsigned hd )
	{ return( *(TUser *)m_hd_usr.hd_at( hd ) ); }

	
	string grp( int id );
	// Avoid groups list
	void grp_list( vector<string> &list ) 
	{ m_hd_grp.obj_list( list ); }
	// Add group
	void grp_add( string name );
	// Del group
	void grp_del( string name ) 
	{ delete (TGroup *)m_hd_grp.obj_del( name ); }
	// Attach to group
	unsigned grp_att( string name )
	{ return( m_hd_grp.hd_att( name ) ); }	
	// Detach from group
	void grp_det( unsigned hd )
	{ m_hd_grp.hd_det( hd ); }
	// Get attached object
        TGroup &grp_at( unsigned hd )
	{ return( *(TGroup *)m_hd_grp.hd_at( hd ) ); }	
	
	// Get XML section node
	XMLNode *XMLCfgNode();
	
	string opt_descr( );
	void CheckCommandLine(  );
	void UpdateOpt( );
	
	void LoadBD( );
	void UpdateBD( );	
	
	TKernel &Owner() const { return(*owner); }
	//================== Controll functions ========================
	void ctr_cmd_go( string a_path, XMLNode *fld, XMLNode *rez );
    public:
    /** Private methods: */
    private:
        unsigned usr_id_f();
        unsigned grp_id_f();
	//================== Controll functions ========================
	void ctr_fill_info( XMLNode *inf );
	void ctr_din_get_( string a_path, XMLNode *opt );
	void ctr_din_set_( string a_path, XMLNode *opt );
	unsigned ctr_att( string br );    
	void     ctr_det( string br, unsigned hd );
	TContr  &ctr_at( string br, unsigned hd );
    private:
        THD                 m_hd_usr; 
        THD                 m_hd_grp; 

	unsigned            hd_res;   
	TKernel             *owner;	

	SBDS                m_bd;
	
	static SCfgFld      gen_elem[]; //Generic BD elements
	
	static const char   *i_cntr;
	static const char   *o_name;
	static const char   *s_name;
};

#endif // TSEQURITY_H

