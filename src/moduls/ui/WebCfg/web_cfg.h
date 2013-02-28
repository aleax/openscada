
//OpenSCADA system module UI.WebCfg file: web_cfg.h
/***************************************************************************
 *   Copyright (C) 2004-2013 by Roman Savochenko                           *
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

using namespace OSCADA;

namespace WebCfg
{

//*************************************************
//* SSess                                         *
//*************************************************
class SSess
{
    public:
	//Methods
	SSess( const string &iurl, const string &isender, const string &iuser, vector<string> &ivars, const string &icontent );

	//Attributes
	string	url;			//request URL
	string	page;
	string	sender;			//request sender
	string	user;			//sesion user
	string	content;		//Contain

	vector<string>		vars;	//request vars
	map<string,string>	cnt;	//Parsed contain
	map<string,string>	prm;	//URL parameters

	vector<string>		mess;	//no interrupt messages

	XMLNode	pg_info;	//page node
	XMLNode	*root;
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

	void modStart( );
	void modStop( );

    protected:
	//Methods
	void load_( );

    private:
	//Methods
	void colontDown( SSess &ses );

	string httpHead( const string &rcode, int cln, const string &cnt_tp = "text/html", const string &addattr = "" );
	string pgHead( string head_els = "" );
	string pgTail( );

	void HttpGet( const string &url, string &page, const string &sender, vector<string> &vars, const string &user );
	void getAbout( SSess &ses );
	void getHead( SSess &ses );
	void getArea( SSess &ses, XMLNode &node, string a_path );
	void getCmd( SSess &ses, XMLNode &node, string a_path ); 
	bool getVal( SSess &ses, XMLNode &node, string a_path, bool rd = true );
	string getCookie( string name, vector<string> &vars );

	void HttpPost( const string &url, string &page, const string &sender, vector<string> &vars, const string &user );
	int  postArea( SSess &ses, XMLNode &node, const string &prs_comm, int level = 0 );
	int  postVal( SSess &ses, XMLNode &node, string prs_path);
	bool valPrepare( SSess &ses, XMLNode &node, string prs_path, bool compare );
	int  postCmd( SSess &ses, XMLNode &node, string prs_path );
	int  postList( SSess &ses, XMLNode &node, string prs_path );
	int  postTable( SSess &ses, XMLNode &node, string prs_path );
	//> Post message dialog
	void messPost( string &page, const string &cat, const string &mess, MessLev type = Info );

	//> Get form content for name
	string cntGet( SSess &ses, const string &nm );

	//> Controll system requests
	int cntrIfCmd( XMLNode &node );

	string modInfo( const string &name );
	void   modInfo( vector<string> &list );

	//Attributes
	string		mCSStables;		//CSS tables
};

extern TWEB *mod;
}

#endif //WEB_CFG_H
