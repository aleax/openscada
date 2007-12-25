
//OpenSCADA system module UI.VCAEngine file: libwidg.h
/***************************************************************************
 *   Copyright (C) 2006-2007 by Roman Savochenko                           *
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

#ifndef LIBWDG_H
#define LIBWDG_H

#include <tcntrnode.h>
#include <tconfig.h>

#include "widget.h"

namespace VCA
{

//************************************************
//* WidgetLib: Widgets library                   *
//************************************************
class LWidget;
    
class WidgetLib : public TCntrNode, public TConfig
{
    public:
	//Methods
        WidgetLib( const string &id, const string &name, const string &lib_db = "*.*" );
	~WidgetLib( );

        const string &id( )	{ return m_id; }		//Identifier
        string name( );						//Name
        string descr( ) 	{ return m_descr; }		//Description
	string ico( )		{ return m_ico; }		//Icon
	string user( );						//Library user
	string grp( );						//Library group
	short  permit( )	{ return m_permit; }		//Permition for access to library

	string DB( )		{ return work_lib_db; }		//Current library DB
        string tbl( )    	{ return m_dbt; }		//Table of storing library data
	string fullDB( )	{ return DB()+'.'+tbl(); }	//Full address to library data storage ( DB()+"."+tbl() )

	void setName( const string &it )	{ m_name = it; }
	void setDescr( const string &it )	{ m_descr = it; }
	void setIco( const string &it )		{ m_ico = it; }
	void setUser( const string &it );
	void setGrp( const string &it )		{ m_grp = it; }
	void setPermit( short it )		{ m_permit = it; }
	void setTbl( const string &it )		{ m_dbt = it; }
	
	void setFullDB( const string &it );

        void load( );
        void save( );

	//- Enable stat -
	bool enable( )			{ return m_enable; }
        void setEnable( bool val );

        //- Mime data access -
	void mimeDataList( vector<string> &list );
        bool mimeDataGet( const string &id, string &mimeType, string *mimeData = NULL );
        void mimeDataSet( const string &id, const string &mimeType, const string &mimeData );
	void mimeDataDel( const string &id );

	//- Widgets -
        void list( vector<string> &ls ) 	{ chldList(m_wdg,ls); }
        bool present( const string &id )	{ return chldPresent(m_wdg,id); }
        AutoHD<LWidget> at( const string &id );
        void add( const string &id, const string &name, const string &orig = "" );
	void add( LWidget *iwdg );
        void del( const string &id, bool full = false )	{ chldDel(m_wdg,id,-1,full); }

    protected:
	//Methods
        string nodeName()       { return m_id; }
        void cntrCmdProc( XMLNode *opt );       //Control interface command process

	void postEnable( int flag );
	void preDisable( int flag );
        void postDisable( int flag );

	//Attributes
	int     m_wdg;

    private:
	//Attributes
        string  &m_id, &m_name, &m_descr, &m_dbt, &m_user, &m_grp, &m_ico, work_lib_db;
	int	&m_permit;
	bool    m_enable;
};

//************************************************
//* LWidget: Library stored widget               *
//************************************************
class CWidget;

class LWidget : public Widget, public TConfig
{
    public:
    //Methods
        LWidget( const string &id, const string &isrcwdg = "" );
        ~LWidget( );

	string path( );
        string ico( );
	string type( )          { return "LibWidget"; }
        string user( );
        string grp( );
        short  permit( )    	{ return m_permit; }
	string calcId( );
        string calcLang( );
        string calcProg( );
	int    calcPer( );

        void setIco( const string &iico )      { m_ico = iico; }
        void setUser( const string &iuser );
        void setGrp( const string &igrp )      { m_grp = igrp; }
        void setPermit( short iperm )          { m_permit = iperm; }
        void setCalcLang( const string &ilng );
        void setCalcProg( const string &iprg );
        void setCalcPer( int vl );
        void setParentNm( const string &isw );

        //- Storing -
        void load( );
	void loadIO( );
        void save( );
	void saveIO( );

        //- Include widgets -
        void wdgAdd( const string &wid, const string &name, const string &path );
        AutoHD<CWidget> wdgAt( const string &wdg );
	
	//- Data access -
        string resourceGet( const string &id, string *mime = NULL );

        WidgetLib &owner( );

    protected:
	//Methods
        void postDisable( int flag );
        void cntrCmdProc( XMLNode *opt );       //Control interface command process

    private:
	//Attributes
        string  &m_ico,         //Widget icon
                &m_user,        //Widget user
                &m_grp,         //Widget group
                &m_proc;        //Widget procedure
        int 	&m_permit,	//Widget permission
		&m_proc_per;	//Widget period
};

//************************************************
//* CWidget: Container stored widget             *
//************************************************
class CWidget : public Widget, public TConfig
{
    public:
	//Methods
        CWidget( const string &id, const string &isrcwdg = "" );
        ~CWidget( );

	//- Main parameters -
	string path( );
        string ico( );
	string type( )          { return "LibLink"; }
        string user( );
        string grp( );
        short  permit( );
	string calcId( );
        string calcLang( );
        string calcProg( );
	int    calcPer( );

        void setParentNm( const string &isw );

        //- Storing -
        void load( );
	void loadIO( );
        void save( );
	void saveIO( );

	//- Data access -
        string resourceGet( const string &id, string *mime = NULL );

        LWidget &owner( );

    protected:
	//Methods
        void postEnable( int flag );
	void preDisable( int flag );
        void postDisable( int flag );

        void cntrCmdProc( XMLNode *opt );       //Control interface command process
	
	//Attributes
	bool wdgIherited;
};

}

#endif //LIBWDG_H
