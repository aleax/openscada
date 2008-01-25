
//OpenSCADA system module UI.WebVision file: web_vision.h
/***************************************************************************
 *   Copyright (C) 2007 by Roman Savochenko                                *
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

#ifndef WEB_VISION_H
#define WEB_VISION_H

#include <string>
#include <vector>
#include <map>

#include <tuis.h>

#include "vca_sess.h"

#undef _
#define _(mess) mod->I18N(mess)

#define rgb(r,g,b) (((r)<<16)+((g)<<8)+(b))

using std::string;
using std::vector;
using std::map;

namespace WebVision
{

//*************************************************
//* SAuth                                         *
//*************************************************
class SAuth
{
    public:
	SAuth( ) :	t_auth(0), name("")	{ }
	SAuth( string inm, time_t it_auth ) : t_auth(it_auth), name(inm)	{ }

	time_t t_auth;
	string name;
};

//*************************************************
//* SSess                                         *
//*************************************************
class SSess
{
    public:
        //Methods
        SSess( const string &iurl, const string &ipage, const string &isender,
        	vector<string> &ivars, const string &icontent );
							
        //Attributes
        string url;             //request URL
        string page;
        string sender;          //request sender
        string user;            //sesion user
        string content;         //Contain

        vector<string> vars;    //request vars
	map< string, string >	cnt;	//Parsed contain
	map< string, string >	prm;	//URL parameters    
	
	vector<string> mess;	//no interrupt messages
};

//************************************************
//* TWEB                                         *
//************************************************
class TWEB: public TUI
{
    public:
	//Data
	enum MessLev	{ Info, Warning, Error };
	
	//Methods
	TWEB( string name );
	~TWEB( );

        time_t authTime( )				{ return m_t_auth; }

        //- VCA sessions -
        void vcaSesList( vector<string> &list )		{ chldList(id_vcases,list); }
        bool vcaSesPresent( const string &name )	{ return chldPresent(id_vcases,name); }
        void vcaSesAdd( const string &name );
        void vcaSesDel( const string &name )		{ chldDel(id_vcases,name); }
	AutoHD<VCASess> vcaSesAt( const string &name )	{ return chldAt(id_vcases,name); }

	void modLoad( );
	void modSave( );
        void modStart( );
	void modStop( );			
    
	//- Web process methods -
	void HttpGet( const string &url, string &page, const string &sender, vector<string> &vars );
	void getAbout( SSess &ses );
	void getAuth( SSess &ses );
	string getCookie( string name, vector<string> &vars );
	
	void HttpPost( const string &url, string &page, const string &sender, vector<string> &vars, const string &contain );	
	    
	string optDescr( );
	string modInfo( const string &name );
	void   modInfo( vector<string> &list );
 
	void cntrCmdProc( XMLNode *opt );       	//Control interface command process
	
	string httpHead( const string &rcode, int cln = 0, const string &cnt_tp = "text/html", const string &addattr = "" );
	string pgHead( string head_els = "" );
	string pgTail( );
	
	int cntrIfCmd( XMLNode &node, const string &user, bool VCA = true );
	
	int colorParse( const string &clr );
	
        //Attributes
	string		VCAjs;				//Main page JavaScript programm VCA.js	
	
    private:
	//Methods
	static void TaskSessCheck( union sigval obj );	//Sessions check task
	
	//- Sesion manipulation function -
        int sesOpen( string name );
        void sesCheck( SSess &ses );
	
	//- Post message dialog -
	void messPost( string &page, const string &cat, const string &mess, MessLev type = Info );
	
        //Attributes
        Res             m_res;
	map< int, SAuth >	m_auth;
        int             m_t_auth;               	//Time of sesion life (minutes)
	timer_t 	chkSessTm;        		//Check session's timer
	bool		chck_st;        		//Check session's status
	int     	id_vcases;			//VCA session's container identifier
	string          m_CSStables;            	//CSS tables
	map<string, int> colors;			//Named colors
};
    
extern TWEB *mod;
}
#endif //WEB_VISION_H 
