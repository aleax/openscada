
//OpenSCADA module UI.WebCfgD file: web_cfg.h
/***************************************************************************
 *   Copyright (C) 2008-2022 by Roman Savochenko, <roman@oscada.org>       *
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
#define _(mess) mod->I18N(mess).c_str()

using namespace OSCADA;

namespace WebCfgD
{

//*************************************************
//* SSess                                         *
//*************************************************
struct SSess
{
    //Methods
    SSess( const string &iurl, const string &isender, const string &iuser,
		vector<string> &ivars, const string &icontent );

    //Attributes
    string	url,		//request URL
		page,
		sender,		//request sender
		user,		//sesion user
		content,	//Contain
		lang;		//Language

    vector<string>	vars;	//request vars
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

	void modStart( )	{ runSt = true; }
	void modStop( )		{ runSt = false; }

    private:
	//Methods
	string pgCreator( TProtocolIn *iprt, const string &cnt, const string &rcode = "", const string &httpattrs = "",
	    const string &htmlHeadEls = "", const string &forceTmplFile = "" );

	static bool compareHosts( const TTransportS::ExtHost &v1, const TTransportS::ExtHost &v2 );

	void HTTP_GET( const string &url, string &page, vector<string> &vars, const string &user, TProtocolIn *iprt );
	void HTTP_POST( const string &url, string &page, vector<string> &vars, const string &user, TProtocolIn *iprt );

	// Controll system requests
	int cntrIfCmd( XMLNode &node, const string &user );

	void   modInfo( vector<string> &list );
	string modInfo( const string &name );
	string trMessReplace( const string &tsrc );

	void imgConvert( SSess &ses, string &vl );

	void cntrCmdProc( XMLNode *opt );	//Control interface command process
};

extern TWEB *mod;
}

#endif //WEB_CFG_H
