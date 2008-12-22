
//OpenSCADA system module UI.WebCfgD file: web_cfg.h
/***************************************************************************
 *   Copyright (C) 2008 by Roman Savochenko                                *
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

#ifndef WEB_CFG_H
#define WEB_CFG_H

#include <tuis.h>

#undef _
#define _(mess) mod->I18N(mess)

namespace WebCfgD
{

//*************************************************
//* SAuth                                         *
//*************************************************
class SAuth
{
    public:
	SAuth( ) : tAuth(0), name("")	{ }
	SAuth( string inm, time_t itAuth ) : tAuth(itAuth), name(inm)	{ }

	time_t tAuth;
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
	string	url;			//request URL
	string	page;
	string	sender;			//request sender
	string	user;			//sesion user
	string	content;		//Contain

	vector<string>		vars;	//request vars
	map<string,string>	cnt;	//Parsed contain
	map<string,string>	prm;	//URL parameters
	map<string,string>	files;	//Post files
};

//*************************************************
//* TWEB                                          *
//*************************************************
class TWEB: public TUI
{
    public:
	//Data
	enum MessLev		{ Info, Warning, Error };

	//Methods
	TWEB( string name );
	~TWEB( );

	int authTime( )				{ return mTAuth; }
	string CSStables( )			{ return mCSStables; }

	void setAuthTime( int vl )		{ mTAuth = vl; modif(); }
	void setCSStables( const string &vl )	{ mCSStables = vl; modif(); }

	void modStart( );
	void modStop( );

    protected:
	//Methods
	void load_( );
	void save_( );

    private:
	//Methods
	string httpHead( const string &rcode, int cln, const string &cnt_tp = "text/html", const string &addattr = "" );
	string pgHead( string head_els = "" );
	string pgTail( );

	void HttpGet( const string &url, string &page, const string &sender, vector<string> &vars );
	void getAbout( SSess &ses );
	void getAuth( SSess &ses );
	string getCookie( string name, vector<string> &vars );

	void HttpPost( const string &url, string &page, const string &sender, vector<string> &vars, const string &contain );
	//- Post message dialog -
	void messPost( string &page, const string &cat, const string &mess, MessLev type = Info );
	//- Sesion manipulation function -
	int sesOpen( string name );
	void sesCheck( SSess &ses );

	//- Get form content for name -
	string cntGet( SSess &ses, const string &nm );

	//- Controll system requests -
	int cntrIfCmd( XMLNode &node, const string &user );

	string optDescr( );
	string modInfo( const string &name );
	void   modInfo( vector<string> &list );

	void cntrCmdProc( XMLNode *opt );	//Control interface command process

	//Attributes
	Res		mRes;
	map<int,SAuth>	mAuth;
	int		mTAuth;			//Time of sesion life (minutes)
	time_t		lst_ses_chk;		//Last time of sessions check
	string		mCSStables;		//CSS tables
};

extern TWEB *mod;
}

#endif //WEB_CFG_H
