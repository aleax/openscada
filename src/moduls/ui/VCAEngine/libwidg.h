
//OpenSCADA system module UI.VCSEngine file: libwidg.h
/***************************************************************************
 *   Copyright (C) 2006 by Roman Savochenko
 *   rom_as@diyaorg.dp.ua
 *
 *   This program is free software; you can redistribute it and/or modify
 *   it under the terms of the GNU General Public License as published by
 *   the Free Software Foundation; either version 2 of the License, or
 *   (at your option) any later version.
 *
 *   This program is distributed in the hope that it will be useful,
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *   GNU General Public License for more details.
 *
 *   You should have received a copy of the GNU General Public License
 *   along with this program; if not, write to the
 *   Free Software Foundation, Inc.,
 *   59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
 ***************************************************************************/
		  
#ifndef LIBWDG_H
#define LIBWDG_H

#include <tcntrnode.h>
#include <tconfig.h>
		  
namespace VCA
{

class Widget;

class LibWdg : public TCntrNode, public TConfig
{
    public:
        LibWdg( const string &id, const string &name, const string &lib_db = "*.*" );
	~LibWdg();
			
        const string &id( )	{ return m_id; }		//Identifier
        string name( );						//Name
        string descr( ) 	{ return m_descr; }		//Description
	string ico( )		{ return m_ico; }		//Icon
	string user( );						//Library user
	string grp( );						//Library group
	short permit( )		{ return m_permit; }		//Permition for access to library
	
	string DB( )		{ return work_lib_db; }		//Current library DB
        string tbl( )    	{ return m_dbt; }		//Table of storing library data
	string fullDB( )	{ return DB()+'.'+tbl(); }	//Full address to library data storage ( DB()+"."+tbl() )
	
	void name( const string &it )	{ m_name = it; }
	void descr( const string &it )	{ m_descr = it; }
	void ico( const string &it )	{ m_ico = it; }
	void user( const string &it );
	void grp( const string &it )	{ m_grp = it; }
	void permit( short it )		{ m_permit = it; }
	void tbl( const string &it )	{ m_dbt = it; }
	
        void load( );
        void save( );
	
	//- Enable stat -
	bool enable( )			{ return m_enable; }
        void enable( bool val );				
	
	//- Widgets -
        void list( vector<string> &ls ) 	{ chldList(m_wdg,ls); }
        bool present( const string &id )	{ return chldPresent(m_wdg,id); }
        AutoHD<Widget> at( const string &id );
        void add( const string &id, const string &name, const string &orig );
        void del( const string &id, bool full = false )	{ chldDel(m_wdg,id,-1,full); }

    protected:
	//Methods
        string nodeName()       { return m_id; }
        void cntrCmdProc( XMLNode *opt );       //Control interface command process
	
	void preDisable( int flag );	
        void postDisable( int flag );
	
	//Attributes
	int     m_wdg;
	
    private:
        string  &m_id, &m_name, &m_descr, &m_dbt, &m_user, &m_grp, &m_ico, work_lib_db;
	int	&m_permit;
	bool    m_enable;
};
		  
}

#endif //LIBWDG_H
