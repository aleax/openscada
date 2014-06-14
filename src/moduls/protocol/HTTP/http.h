
//OpenSCADA system module Protocol.HTTP file: http.h
/***************************************************************************
 *   Copyright (C) 2003-2014 by Roman Savochenko, <rom_as@oscada.org>      *
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

#ifndef HTTP_H
#define HTTP_H

#include <string>
#include <map>

#include <tprotocols.h>

#undef _
#define _(mess) mod->I18N(mess)

#define cntBnd "asdfeeeeqqwe34QWEasd4rserv$WERwar34q2fsdFWEra345tfSDFasefavt5rtaew@#"

using std::string;
using std::map;
using namespace OSCADA;

namespace PrHTTP
{

//*************************************************
//* TProtIn                                       *
//*************************************************
class TProtIn: public TProtocolIn
{
    public:
	//Methods
	TProtIn( string name );
	~TProtIn( );

	bool mess( const string &request, string &answer );

    private:
	//Methods
	string getIndex( const string &user, const string &sender );
	string getAuth( const string& url = "/", const string &mess = "" );
	void getCnt( const vector<string> &vars, const string &content, map<string,string> &cnt );

	string httpHead( const string &rcode, int cln, const string &addattr = "", bool defCtx = true );
	string pgHead( const string &head_els = "" );
	string pgTail( );
	string pgTmpl( const string &cnt, const string &head_els = "" );

	//Attributes
	bool m_nofull;
	string m_buf;
};

//*************************************************
//* TProt                                         *
//*************************************************
class TProt: public TProtocol
{
    public:
	//Methods
	TProt( string name );
	~TProt( );

	int authTime( )			{ return mTAuth; }
	string tmpl( )			{ return mTmpl; }

	void setAuthTime( int vl )	{ mTAuth = vmax(1,vl); modif(); }
	void setTmpl( const string &vl ){ mTmpl = vl; modif(); }

	void outMess( XMLNode &io, TTransportOut &tro );

	//> Auth session manipulation functions
	int sesOpen( const string &name );
	void sesClose( int sid );
	string sesCheck( int sid );

	//> Auto-login
	string autoLogGet( const string &sender );

	Res &nodeRes( )			{ return nRes; }

    protected:
	//Methods
	void load_( );
	void save_( );

    private:
	//Data
	//*************************************************
	//* SAuth                                         *
	//*************************************************
	class SAuth
	{
	    public:
		//Methods
		SAuth( ) : tAuth(0), name("")	{ }
		SAuth( string inm, time_t itAuth ) : tAuth(itAuth), name(inm)	{ }

		//Attributes
		time_t tAuth;
		string name;
	};

	//*************************************************
	//* SAutoLogin                                    *
	//*************************************************
	class SAutoLogin
	{
	    public:
		//Methods
		SAutoLogin( ) : addrs(""), user("")	{ }
		SAutoLogin(string iaddrs, string iuser) : addrs(iaddrs), user(iuser)	{ }

		//Attributes
		string addrs, user;
	};

	//Methods
	string optDescr( );
	TProtocolIn *in_open( const string &name );

	void cntrCmdProc( XMLNode *opt );	//Control interface command process

	//Attributes
	map<int,SAuth>	mAuth;
	int		mTAuth;
	ResString	mTmpl;
	time_t		lst_ses_chk;

	vector<SAutoLogin>	mALog;

	Res		nRes;
};

extern TProt *mod;
} //End namespace PrHTTP

#endif //HTTP_H
