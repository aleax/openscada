
//OpenSCADA system file: tsecurity.h
/***************************************************************************
 *   Copyright (C) 2003-2006 by Roman Savochenko                           *
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

#ifndef TSECURITY_H
#define TSECURITY_H

#include "tbds.h"

#define SEQ_RD 0x01
#define SEQ_WR 0x02
#define SEQ_XT 0x04

class TSYS;
class XMLNode;

class TSecurity;

class TUser : public TCntrNode, public TConfig 
{
    public:
	//Methods
	TUser( const string &name, const string &idb, unsigned id, TElem *el );
	~TUser(  );
	
	string   &name()  	{ return(m_name); }
	string   &lName()	{ return(m_lname); }
        int      &id()    	{ return(m_id); }
	string   &grp()   	{ return(m_grp); }
	bool     auth( const string &n_pass )
	{ return( (m_pass == n_pass)?true:false ); }
	
	void name( const string &nm )	{ m_name = nm; }
	void lName( const string &nm )	{ m_lname = nm; }
	void id( unsigned n_id )	{ m_id = n_id; }
	void pass( const string &n_pass )	{ m_pass = n_pass; }
	void grp( const string &nm_grp )	{ m_grp = nm_grp; }

	void load( );
	void save( );
	
	string BD();
	
	TSecurity &owner()	{ return *(TSecurity*)nodePrev(); }
	
    private:	
	//Methods    
	string nodeName()	{ return m_name; }
	void cntrCmd_( const string &a_path, XMLNode *opt, TCntrNode::Command cmd );	
	void postDisable(int flag);     //Delete all DB if flag 1

	//Attributes	
	string	&m_name;
	string	&m_lname;
	string	&m_pass;
	string	&m_grp;
	int 	&m_id;
	string	m_bd;
};

class TGroup : public TCntrNode, public TConfig
{
    public:
	//Methods
	TGroup( const string &name, const string &idb, unsigned id, TElem *el );
	~TGroup(  );

	string &name()  { return(m_name); }
	string &lName() { return(m_lname); }
        int    &id()    { return(m_id); }
	
	void name( const string &nm )    { m_name = nm; }
	void lName( const string &nm )   { m_lname = nm; }
	void id( unsigned n_id )         { m_id = n_id; }
	
	bool user( const string &name );

	void load( );
	void save( );
	
	string BD();
	
	TSecurity &owner(){ return *(TSecurity*)nodePrev(); }
	
    private:	 
	//Methods   
	string nodeName(){ return m_name; }
	void cntrCmd_( const string &a_path, XMLNode *opt, TCntrNode::Command cmd );	
	void postDisable(int flag);     //Delete all DB if flag 1
	
	//Attributes
	string	&m_name;
	string	&m_lname;
	string	&m_usrs;
	int	&m_id;
	string  m_bd;
};

class TSecurity : public TSubSYS
{
    public:
	//Methods
	TSecurity( );    
	~TSecurity( );

	void subLoad( );
	void subSave( );

	bool access( const string &user, char mode, int owner, int group, int access );
	
	//- Users -
	string usr( int id );
	int usr( const string &sid );
	void usrList( vector<string> &list )	{ chldList(m_usr,list); }
	bool usrPresent( const string &name ) 	{ return chldPresent(m_usr,name); }
	int usrAdd( const string &name, const string &idb = "*.*" );
	void usrDel( const string &name ) 	{ chldDel(m_usr,name); }
	AutoHD<TUser> usrAt( const string &name )
	{ return chldAt(m_usr,name); }
	
	//- Groups -
	string grp( int id );
	int grp( const string &sid );
	void grpList( vector<string> &list ) 	{ chldList(m_grp,list); }
	bool grpPresent( const string &name )	{ return chldPresent(m_grp,name); }
	int grpAdd( const string &name, const string &idb = "*.*" );
	void grpDel( const string &name ) 	{ chldDel(m_grp,name); }
	AutoHD<TGroup> grpAt( const string &name )
	{ return chldAt(m_grp,name); }
	
	string optDescr( );
	
    private:
	//Methods
        unsigned usr_id_f();
        unsigned grp_id_f();
	void cntrCmd_( const string &a_path, XMLNode *opt, TCntrNode::Command cmd );
	
	void postEnable();
	
	//Attributes
	int	m_usr, m_grp;
	
	TElem	user_el;
	TElem	grp_el;

	unsigned	hd_res;   
};

#endif // TSECURITY_H

