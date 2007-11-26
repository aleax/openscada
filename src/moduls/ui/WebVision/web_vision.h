
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

#undef _
#define _(mess) mod->I18N(mess)

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
	map< string, string >	cnt;	//Parsed contein
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

	void modLoad( );
	void modSave( );
    
    private:
	//Methods
	void HttpGet( const string &url, string &page, const string &sender, vector<string> &vars );
	void getAbout( SSess &ses );
	void getAuth( SSess &ses );
	string getCookie( string name, vector<string> &vars );
	
	void HttpPost( const string &url, string &page, const string &sender, vector<string> &vars, const string &contein );	
	    
	string optDescr( );
	string modInfo( const string &name );
	void   modInfo( vector<string> &list );
 
	void cntrCmdProc( XMLNode *opt );       //Control interface command process
	
	string httpHead( const string &rcode, int cln = 0, const string &cnt_tp = "text/html", const string &addattr = "" );
	string pgHead( string head_els = "" );
	string pgTail( );
	
	//- Sesion manipulation function -
        int sesOpen( string name );
        void sesCheck( SSess &ses );
	
	//- Post message dialog -
	void messPost( string &page, const string &cat, const string &mess, MessLev type = Info );
	
	int cntrIfCmd( XMLNode &node, SSess &ses );
	
        //Attributes
        Res             m_res;
	map< int, SAuth >	m_auth;
        int             m_t_auth;               //Time of sesion life (minutes)
	time_t		lst_ses_chk;		//Last time of sessions check
	string          m_CSStables;            //CSS tables
	string		m_VCAjs;		//Main page programm VCA.js
};    
    
extern TWEB *mod;
}
#endif //WEB_VISION_H 
