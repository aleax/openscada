
//OpenSCADA module Protocol.HTTP file: http.h
/***************************************************************************
 *   Copyright (C) 2003-2021 by Roman Savochenko, <roman@oscada.org>       *
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

#define cntBnd "----OpenSCADAFormBoundary4q2fsdFWEra345tf"
#define CtxTmplMark	"#####CONTEXT#####"

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

	string lang( );

    protected:
	//Methods
	TVariant objFuncCall( const string &id, vector<TVariant> &prms, const string &user );

    private:
	//Methods
	string getIndex( const string &user, const string &sender );
	string getAuth( const string& url = "/", const string &mess = "" );
	void getCnt( const vector<string> &vars, const string &content, map<string,string> &cnt );

	string pgCreator( const string &cnt, const string &rcode = "",
		const string &httpattrs = "", const string &htmlHeadEls = "", const string &forceTmplFile = "" );
	bool pgAccess( const string &URL );

	//Attributes
	bool	mNotFull, KeepAlive;
	int	sesId;
	string	mBuf,
		prms, user, prmLang, brLang;
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

	string deny( )			{ return mDeny; }
	string allow( )			{ return mAllow; }
	string tmpl( )			{ return mTmpl; }
	string tmplMainPage( )		{ return mTmplMainPage; }
	string authSessDB( )		{ return mAuthSessDB; }
	int spaceUID( )			{ return mSpaceUID; }
	string allowUsersAuth( )	{ return mAllowUsersAuth; }
	int authTime( )			{ return mTAuth; }
	string authSessTbl( )		{ return authSessDB().size() ? authSessDB()+".HTTP_AuthSessions" : ""; }

	void setDeny( const string &vl )	{ mDeny = vl; modif(); }
	void setAllow( const string &vl )	{ mAllow = vl; modif(); }
	void setTmpl( const string &vl )	{ mTmpl = vl; modif(); }
	void setTmplMainPage( const string &vl ){ mTmplMainPage = vl; modif(); }
	void setAuthSessDB( const string &vl);
	void setSpaceUID( int vl )		{ mSpaceUID = vmax(0, vmin(100,vl)); modif(); }
	void setAllowUsersAuth( const string &vl ) { mAllowUsersAuth = vl; modif(); }
	void setAuthTime( int vl )		{ mTAuth = vmax(1,vl); modif(); }

	void outMess( XMLNode &io, TTransportOut &tro );

	// Auth session manipulation functions
	int sesOpen( const string &name, const string &srcAddr = "", const string &userAgent = "" );
	void sesClose( int sid );
	string sesCheck( int sid, const string &chUser = "" );

	// Auto-login
	string autoLogGet( const string &sender );

	//Attributes
	MtxString cookieLab;

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
		SAuth( ) : tAuth(0)	{ }
		SAuth( string inm, time_t itAuth, const string &srcAddr, const string &userAgent ) :
		    tAuth(itAuth), name(inm), addr(srcAddr), agent(userAgent)	{ }

		//Attributes
		time_t tAuth;
		string name, addr, agent;
	};

	//*************************************************
	//* SAutoLogin                                    *
	//*************************************************
	class SAutoLogin
	{
	    public:
		//Methods
		SAutoLogin( ) : addrs(""), user("")	{ }
		SAutoLogin( string iaddrs, string iuser ) : addrs(iaddrs), user(iuser)	{ }

		bool operator==( const SAutoLogin &i ) { return (addrs == i.addrs && user == i.user); }

		//Attributes
		string addrs, user;
	};

	//Methods
	string optDescr( );
	TProtocolIn *in_open( const string &name );

	void cntrCmdProc( XMLNode *opt );	//Control interface command process

	//Attributes
	MtxString	mDeny, mAllow, mTmpl, mTmplMainPage, mAllowUsersAuth, mAuthSessDB;
	TElem		elAuth;		//Elements of the external authentication sessions
	map<int, SAuth>	mAuth;
	int		mTAuth;
	int		mSpaceUID;
	time_t		lstSesChk;

	vector<SAutoLogin> mALog;
	ResMtx		authM;
};

extern TProt *mod;
} //End namespace PrHTTP

#endif //HTTP_H
