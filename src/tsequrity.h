#ifndef TSEQURITY_H
#define TSEQURITY_H

#include "thd.h"
#include "tcontr.h"
//#include "tconfig.h"

#define SEQ_RD 0x01
#define SEQ_WR 0x02
#define SEQ_XT 0x04

class TKernel;
class XMLNode;

class TSequrity;

class TUser
{
    /** Public methods: */
    public:
	TUser( TSequrity *owner, string name, unsigned id);
	~TUser(  );
	
	string   &Name()  { return(m_name); }
        unsigned Id()     { return(m_id); }
	string   &Descr() { return(m_l_name); }
	bool     Auth( string pass ){ return( (m_pass == pass)?true:false ); }
	
	//void Name( string name );
	//void Id( unsigned id );
	void Descr( string name ) { m_l_name = name; }
	void Pass( string pass )  { m_pass = pass; }
	
	TSequrity &Owner(){ return(*m_owner); }
    /** Public atributes: */
    private:
	unsigned m_id;          //Individual user identificator
	string   m_name;        //User name
	string   m_l_name;      //Long user name(description)
	string   m_pass;        //User password
	
       	TSequrity *m_owner;
};

class TGroup
{
    /** Public methods: */
    public:
	TGroup( TSequrity *owner, string name, unsigned id );
	~TGroup(  );

	string &Name(){ return(m_name); }
        unsigned Id(){ return(m_id); }
	string   &Descr() { return(m_l_name); }
	
	void Descr( string name ) { m_l_name = name; }
	
	TSequrity &Owner(){ return(*m_owner); }
    /** Public atributes: */
    private:
	unsigned m_id;          //Individual group identificator
	string   m_name;        //Group name
	string   m_l_name;      //Long group name(description)
	vector<string> m_users; //Users in group

       	TSequrity *m_owner;
};

class TSequrity : public TContr//, public TConfig
{
    /** Public methods: */
    public:
	TSequrity( TKernel *app );    
	~TSequrity(  );

	bool access( string user, char mode, int owner, int group, int access );
	
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
	
	TKernel &Owner() const { return(*owner); }
    public:
    /** Private methods: */
    private:
        unsigned usr_id_f();
        unsigned grp_id_f();
	//================== Controll functions ========================
	void ctr_fill_info( XMLNode *inf );
    private:
        THD                 m_hd_usr; 
        THD                 m_hd_grp; 

	unsigned            hd_res;   
	TKernel             *owner;	
	
	static const char   *i_cntr;
	static const char   *o_name;
};

#endif // TSEQURITY_H

