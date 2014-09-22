
//OpenSCADA system module UI.WebCfgD file: web_cfg.h
/***************************************************************************
 *   Copyright (C) 2008-2014 by Roman Savochenko, <rom_as@oscada.org>      *
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

namespace WebCfgD
{

//*************************************************
//* SSess                                         *
//*************************************************
class SSess
{
    public:
	//Methods
	SSess( const string &iurl, const string &isender, const string &iuser,
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

	void modStart( );
	void modStop( );

    protected:
	//Methods
	void load_( );

    private:
	//Methods
	string httpHead( const string &rcode, int cln, const string &cnt_tp = "text/html",
	    const string &addattr = "", const string &charset = Mess->charset() );
	string pgHead( string head_els = "" );
	string pgTail( );

	void HttpGet( const string &url, string &page, const string &sender, vector<string> &vars, const string &user );
	void getAbout( SSess &ses );
	string getCookie( string name, vector<string> &vars );

	void HttpPost( const string &url, string &page, const string &sender, vector<string> &vars, const string &user );

	// Get form content for name
	string cntGet( SSess &ses, const string &nm );

	// Controll system requests
	int cntrIfCmd( XMLNode &node, const string &user );

	string modInfo( const string &name );
	void   modInfo( vector<string> &list );
	string trMessReplace( const string &tsrc );

	void imgConvert( SSess &ses );

	void cntrCmdProc( XMLNode *opt );	//Control interface command process
};

extern TWEB *mod;
}

#endif //WEB_CFG_H
