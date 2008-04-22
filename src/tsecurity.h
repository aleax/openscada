
//OpenSCADA system file: tsecurity.h
/***************************************************************************
 *   Copyright (C) 2003-2007 by Roman Savochenko                           *
 *   rom_as@fromru.com                                                     *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; version 2 of the License.               *
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

#define SEQ_XT 0x01
#define SEQ_WR 0x02
#define SEQ_RD 0x04

//*************************************************
//* TUser                                         *
//*************************************************
class TSecurity;

class TUser : public TCntrNode, public TConfig 
{
    public:
	//Methods
	TUser( const string &name, const string &db, TElem *el );
	~TUser( );

	TCntrNode &operator=( TCntrNode &node );

	const string	&name( ) 	{ return m_name; }
	const string	&lName( )	{ return m_lname; }
	const string	&picture( )	{ return m_pict; }	
	bool sysItem( )			{ return m_sysIt; }	

	bool auth( const string &pass );
	
	string DB( )            { return m_db; }
        string tbl( );
        string fullDB( )        { return DB()+'.'+tbl(); }
	
	void setName( const string &nm )	{ m_name = nm; }
	void setLName( const string &nm )	{ m_lname = nm; }
	void setPicture( const string &pct )	{ m_pict = pct; }
	void setPass( const string &n_pass );
	void setSysItem( bool vl )		{ m_sysIt = vl; }

	void load( );
	void save( );
	
	TSecurity &owner( )	{ return *(TSecurity*)nodePrev(); }
	
    private:	
	//Methods    
	string nodeName( )	{ return m_name; }
	void cntrCmdProc( XMLNode *opt );       //Control interface command process
	void postDisable( int flag );     	//Delete all DB if flag 1

	//Attributes	
	string	&m_name;
	string	&m_lname;
	string	&m_pass;
	string	&m_pict;
	string	m_db;
	bool	m_sysIt;
};

//*************************************************
//* TGroup                                        *
//*************************************************
class TGroup : public TCntrNode, public TConfig
{
    public:
	//Methods
	TGroup( const string &name, const string &db, TElem *el );
	~TGroup( );

	TCntrNode &operator=( TCntrNode &node );

	const string &name( )  	{ return m_name; }
	const string &lName( ) 	{ return m_lname; }
	bool sysItem( )		{ return m_sysIt; }
	
	string DB( )            { return m_db; }
        string tbl( );
        string fullDB( )        { return DB()+'.'+tbl(); }
	
	void setName( const string &nm )	{ m_name = nm; }
	void setLName( const string &nm )	{ m_lname = nm; }
	void setSysItem( bool vl )		{ m_sysIt = vl; }
		
	bool user( const string &name );
	void userAdd( const string &name );
	void userDel( const string &name );

	void load( );
	void save( );
	
	TSecurity &owner( )	{ return *(TSecurity*)nodePrev(); }
	
    private:	 
	//Methods   
	string nodeName( )	{ return m_name; }
	void cntrCmdProc( XMLNode *opt );       //Control interface command process
	void postDisable( int flag );     	//Delete all DB if flag 1
	
	//Attributes
	string	&m_name;
	string	&m_lname;
	string	&m_usrs;
	string  m_db;
	bool    m_sysIt;
};

//*************************************************
//* TSecurity                                     *
//*************************************************
class TSecurity : public TSubSYS
{
    public:
	//Methods
	TSecurity( );    
	~TSecurity( );

	void subLoad( );
	void subSave( );

	char access( const string &user, char mode, const string &owner, const string &group, int access );
	
	//- Users -
	void usrList( vector<string> &list )		{ chldList(m_usr,list); }
	void usrGrpList( const string &name, vector<string> &list );
	bool usrPresent( const string &name ) 		{ return chldPresent(m_usr,name); }
	void usrAdd( const string &name, const string &db = "*.*" );
	void usrDel( const string &name, bool complete = false );
	AutoHD<TUser> usrAt( const string &name )	{ return chldAt(m_usr,name); }
	
	//- Groups -
	void grpList( vector<string> &list ) 		{ chldList(m_grp,list); }
	bool grpPresent( const string &name )		{ return chldPresent(m_grp,name); }
	void grpAdd( const string &name, const string &db = "*.*" );
	void grpDel( const string &name, bool complete = false );
	AutoHD<TGroup> grpAt( const string &name )	{ return chldAt(m_grp,name); }
	
    private:
	//Methods
	string optDescr( );	
	void cntrCmdProc( XMLNode *opt );       //Control interface command process
	
	void postEnable( int flag );
	
	//Attributes
	int	m_usr, m_grp;
	
	TElem	user_el;
	TElem	grp_el;

	unsigned	hd_res;   
};

#endif // TSECURITY_H

