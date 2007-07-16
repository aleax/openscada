
//OpenSCADA system module UI.VCAEngine file: session.h
/***************************************************************************
 *   Copyright (C) 2007 by Roman Savochenko
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

#ifndef SESSION_H
#define SESSION_H

#include <tcntrnode.h>
#include <tconfig.h>

#include "widget.h"

namespace VCA
{

//************************************************
//* VCA session                              	 *
//************************************************
class SessPage;
    
class Session : public TCntrNode
{
    public:
        Session( const string &id, const string &proj = "" );
	~Session();

        string ico( );
        const string &id( )	{ return m_id; }		//Identifier
	string projNm( )	{ return m_prjnm; }		//Project's name
	string user( )		{ return m_user; }		//Open session user
	int    period( )	{ return m_per; }		//Process period (ms)
	double calcTm( )	{ return tm_calc; }		//Calc session time
	bool   enable( )	{ return m_enable; }		//Enable stat
  	bool   start( )		{ return m_start; }		//Start stat
	unsigned calcClk( )	{ return m_calcClk; }		//Calc clock
	AutoHD<Project> parent( );

	void setProjNm( const string &it )	{ m_prjnm = it; }
	void setUser( const string &it )	{ m_user = it; }
	void setPeriod( int val )		{ m_per = val; }
        void setEnable( bool val );
        void setStart( bool val );

	//- Pages -
        void list( vector<string> &ls ) 	{ chldList(m_page,ls); }
        bool present( const string &id )	{ return chldPresent(m_page,id); }
        AutoHD<SessPage> at( const string &id );
        void add( const string &id, const string &parent = "" );
        void del( const string &id, bool full = false )	{ chldDel(m_page,id,-1,full); }
	
	vector<string> &openList( )		{ return m_open; }
	void openReg( const string &id );
	void openUnreg( const string &id );
	
	Res &eventRes( )			{ return m_evRes; }
	
	void uiComm( const string &com, const string &prm, const string &src );

    protected:
	//Methods
        string nodeName()       { return m_id; }
        void cntrCmdProc( XMLNode *opt );       //Control interface command process

	void postEnable( int flag );
	void preDisable( int flag );

    private:
	//Methods
	static void *Task( void *contr );
    
	//Attributes
	int     m_page;
        string  m_id, m_prjnm, m_user;
	int	m_per;
	bool    m_enable, m_start, endrun_req;	//Enabled, Started and endrun stats
	
	pthread_t calcPthr;     		//Calc pthread
	unsigned  m_calcClk;			//Calc clock
	double  tm_calc;                        //Scheme's calc time
	AutoHD<Project> m_parent;
	Res 	m_evRes;			//Event access resource
	
	vector<string>	m_open;
};

//************************************************
//* Widget of included to session page           *
//************************************************
class SessWdg : public Widget, public TValFunc
{
    public:
    	//Methods
        SessWdg( const string &id, const string &parent = "" );
        ~SessWdg( );

	//- Main parameters -
	string path( );
	string ownerFullId( bool contr = false );
	string type( )          { return "SessWidget"; }
        string ico( );
        string user( );
        string grp( );
        short  permit( );
        string calcLang( );
        string calcProg( );
  	bool   process( )	{ return m_proc; }		//Process stat	

	void setEnable( bool val );
        virtual void setProcess( bool val );
	
	virtual void calc( bool first, bool last, unsigned clcClk );
	
        //- Include widgets -
        void wdgAdd( const string &wid, const string &name, const string &parent );	//Implicit widget's creating on inherit
        AutoHD<SessWdg> wdgAt( const string &wdg );
	
	//- Events process -
	void eventAdd( const string &ev );
	string eventGet( bool clear = false );
	
	//- Access to mime resource -
        string resourceGet( const string &id, string *mime = NULL );

        SessWdg  *ownerSessWdg( bool base = false );
	SessPage *ownerPage();
        Session	 *ownerSess();

    protected:
	void cntrCmdProc( XMLNode *opt );       //Control interface command process
	bool attrChange( Attr &cfg );
    
    private:
	//Data
	//- Link types -
	/*enum LnkT { Const, Param, LocAttr };
	//- Define link structures -
	struct SLnk
	{
	    LnkT tp;    //Link type
            string lnk; //Link value
            union
            {
	        AutoHD<TVal>    *aprm;  //Parameter atribute link structure
	    };
        };*/

	//Attributes
	bool	m_proc;
	string	work_prog;
	//vector<SLnk>	m_lnk;
};

//************************************************
//* Session's page                        	 *
//************************************************
class SessPage : public SessWdg
{
    public:
	//Methods
        SessPage( const string &id, const string &page = "" );
        ~SessPage();

	string path( );
	string type( )          { return "SessPage"; }

	void setEnable( bool val );
	void setProcess( bool val );

	void calc( bool first, bool last, unsigned clcClk );

	AutoHD<Page> parent( );

	//- Pages -
        void pageList( vector<string> &ls ) 	{ chldList(m_page,ls); }
        bool pagePresent( const string &id )	{ return chldPresent(m_page,id); }
        AutoHD<SessPage> pageAt( const string &id );
        void pageAdd( const string &id, const string &parent = "" );
        void pageDel( const string &id, bool full = false )	{ chldDel(m_page,id,-1,full); }
	
    protected:
	void cntrCmdProc( XMLNode *opt );       //Control interface command process
	
	bool attrChange( Attr &cfg );

    private:
	int     m_page;		//Pages container identifier
};

}

#endif //SESSION_H
