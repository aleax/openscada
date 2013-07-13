
//OpenSCADA system module UI.WebVision file: web_vision.h
/***************************************************************************
 *   Copyright (C) 2007-2010 by Roman Savochenko                           *
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

#define POS_PREC_DIG	3

using std::string;
using std::vector;
using std::map;
using namespace OSCADA;

namespace WebVision
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
	string url;		//request URL
	string page;
	string sender;		//request sender
	string user;		//sesion user
	string content;		//Contain

	vector<string>		vars;	//request vars
	map<string,string>	cnt;	//Parsed contain
	map<string,string>	prm;	//URL parameters
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

	time_t	sessTime( )				{ return mTSess; }
	int	sessLimit( )				{ return mSessLimit; }
	int	PNGCompLev( )				{ return mPNGCompLev; }
	string	CSStables( )				{ return mCSStables; }

	void setSessTime( time_t vl )			{ mTSess = vmax(1,vmin(24*60,vl)); modif(); }
	void setSessLimit( int vl )			{ mSessLimit = vmax(1,vmin(100,vl)); modif(); }
	void setPNGCompLev( int vl )			{ mPNGCompLev = vmax(-1,vmin(9,vl)); modif(); }
	void setCSStables( const string &vl )		{ mCSStables = vl; modif(); }

	//> VCA sessions
	void vcaSesList( vector<string> &list )		{ chldList(id_vcases,list); }
	bool vcaSesPresent( const string &name )	{ return chldPresent(id_vcases,name); }
	void vcaSesAdd( const string &name, bool isCreate );
	void vcaSesDel( const string &name )		{ chldDel(id_vcases,name); }
	AutoHD<VCASess> vcaSesAt( const string &name )	{ return chldAt(id_vcases,name); }

	//> Web process methods
	void HttpGet( const string &url, string &page, const string &sender, vector<string> &vars, const string &user );
	void getAbout( SSess &ses );

	void HttpPost( const string &url, string &page, const string &sender, vector<string> &vars, const string &user );

	string optDescr( );
	string modInfo( const string &name );
	void   modInfo( vector<string> &list );

	string httpHead( const string &rcode, int cln = 0, const string &cnt_tp = "text/html",
	    const string &addattr = "", const string &charset = Mess->charset() );
	string pgHead( const string &head_els = "", const string &title = "", const string &charset = Mess->charset() );
	string pgTail( );

	int cntrIfCmd( XMLNode &node, const string &user, bool VCA = true );

	void imgConvert(SSess &ses);
	int colorParse( const string &clr );
	static int colorResolve( gdImagePtr im, int clr );
	string trMessReplace( const string &tsrc );

	void perSYSCall( unsigned int cnt );

    protected:
	//Methods
	void load_( );
	void save_( );

	void modStart( );
        void modStop( );

	void cntrCmdProc( XMLNode *opt );		//Control interface command process

    private:
	//Methods
	//> Post message dialog
	void messPost( string &page, const string &cat, const string &mess, MessLev type = Info );

	//Attributes
	int		mTSess;				//Time of sesion life (minutes)
	int		mSessLimit;			//Sessions limit
	int		mPNGCompLev;			//PNG images compression level
	int		id_vcases;			//VCA session's container identifier
	string		mCSStables;			//CSS tables
	map<string,int> colors;				//Named colors
};

extern TWEB *mod;
}
#endif //WEB_VISION_H
