/***************************************************************************
 *   Copyright (C) 2004 by Roman Savochenko                                *
 *   rom_as@fromru.com                                                     *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 *   This program is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 *                                                                         *
 *   You should have received a copy of the GNU General Public License     *
 *   along with this program; if not, write to the                         *
 *   Free Software Foundation, Inc.,                                       *
 *   59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.             *
 ***************************************************************************/

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
	TUser( TSequrity *owner, const string &name, unsigned id, TElem *el );
	~TUser(  );
	
	string   &Name()  { return(m_name); }
        int      &Id()    { return(m_id); }
	string   &Descr() { return(m_lname); }
	string   &Grp()   { return(m_grp); }
	bool     Auth( const string &pass )
	{ return( (m_pass == pass)?true:false ); }
	
	void Name( const string &name )  { m_name = name; }
	void Id( unsigned id )    	 { m_id = id; }
	void Descr( const string &name ) { m_lname = name; }
	void Pass( const string &pass )  { m_pass = pass; }
	void Grp( const string &grp )    { m_grp = grp; }

	void Load();
	void Save();
	
	TSequrity &Owner(){ return(*m_owner); }
    private:	    
	//================== Controll functions ========================
	void ctr_fill_info( XMLNode *inf );
	void ctr_din_get_( const string &a_path, XMLNode *opt );
	void ctr_din_set_( const string &a_path, XMLNode *opt );
	void ctr_cmd_go_( const string &a_path, XMLNode *fld, XMLNode *rez );
    /** Private atributes: */
    private:	
       	TSequrity *m_owner;

	string    &m_name;
	string    &m_lname;
	string    &m_pass;
	string    &m_grp;
	int       &m_id;
	
	static const char   *i_cntr;
};

class TGroup : public TContr, public TConfig
{
    /** Public methods: */
    public:
	TGroup( TSequrity *owner, const string &name, unsigned id, TElem *el );
	~TGroup(  );

	string &Name()  { return(m_name); }
        int    &Id()    { return(m_id); }
	string &Descr() { return(m_lname); }
	
	void Name( const string &name )  { m_name = name; }
	void Id( unsigned id )    	 { m_id = id; }
	void Descr( const string &name ) { m_lname = name; }

	void Load();
	void Save();
	
	bool user( const string &name );
	
	TSequrity &Owner(){ return(*m_owner); }
    private:	    
	//================== Controll functions ========================
	void ctr_fill_info( XMLNode *inf );
	void ctr_din_get_( const string &a_path, XMLNode *opt );
	void ctr_din_set_( const string &a_path, XMLNode *opt );
	void ctr_cmd_go_( const string &a_path, XMLNode *fld, XMLNode *rez );
    /** Private atributes: */
    private:
	string    &m_name;
	string    &m_lname;
	string    &m_usrs;
	int       &m_id;

       	TSequrity *m_owner;
	
	static const char   *i_cntr;
};

class TSequrity : public TContr
{
    /** Public methods: */
    public:
	TSequrity( TKernel *app );    
	~TSequrity(  );

	string Name();
	
	void Init( );

	bool access( const string &user, char mode, int owner, int group, int access );
	
	string usr( int id );
	// Avoid users list
	void usr_list( vector<string> &list ) 
	{ m_hd_usr.obj_list( list ); }
	// Add user
	void usr_add( const string &name );
	// Del user
	void usr_del( const string &name ) 
	{ delete (TUser *)m_hd_usr.obj_del( name ); }
        // User
	AutoHD<TUser> usr_at( const string &name )
	{ AutoHD<TUser> obj( name, m_hd_usr ); return obj; }       	
	
	string grp( int id );
	// Avoid groups list
	void grp_list( vector<string> &list ) 
	{ m_hd_grp.obj_list( list ); }
	// Add group
	void grp_add( const string &name );
	// Del group
	void grp_del( const string &name ) 
	{ delete (TGroup *)m_hd_grp.obj_del( name ); }
        // Group
	AutoHD<TGroup> grp_at( const string &name )
	{ AutoHD<TGroup> obj( name, m_hd_grp ); return obj; }       
	
	// Get XML section node
	XMLNode *XMLCfgNode();
	
	string opt_descr( );
	void CheckCommandLine(  );
	void UpdateOpt( );
	
	void LoadBD( );
	void UpdateBD( );	
	
	TKernel &Owner() const { return(*owner); }

	SBDS &BD_user(){ return(m_bd_usr); }
	SBDS &BD_grp() { return(m_bd_grp); }
    public:
    /** Private methods: */
    private:
        unsigned usr_id_f();
        unsigned grp_id_f();
	//================== Controll functions ========================
	void ctr_fill_info( XMLNode *inf );
	void ctr_din_get_( const string &a_path, XMLNode *opt );
	void ctr_din_set_( const string &a_path, XMLNode *opt );
	void ctr_cmd_go_( const string &a_path, XMLNode *fld, XMLNode *rez );
	AutoHD<TContr> ctr_at1( const string &br );
    private:
        THD                 m_hd_usr; 
        THD                 m_hd_grp; 
	
	TElem               user_el;
	TElem               grp_el;

	unsigned            hd_res;   
	TKernel             *owner;	

	SBDS                m_bd_usr;
	SBDS                m_bd_grp;
	
	static const char   *i_cntr;
	static const char   *o_name;
	static const char   *s_name;
};

#endif // TSEQURITY_H

