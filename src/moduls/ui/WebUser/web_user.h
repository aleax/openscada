
//OpenSCADA module UI.WebUser file: web_user.h
/***************************************************************************
 *   Copyright (C) 2010-2018 by Roman Savochenko, <rom_as@oscada.org>      *
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

#ifndef WEB_USER_H
#define WEB_USER_H

#include <tuis.h>

#undef _
#define _(mess) mod->I18N(mess)

using namespace OSCADA;

namespace WebUser
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
	string	lang;			//Language

	vector<XMLNode>		cnt;	//Parsed contain
	map<string,string>	vars;	//request vars
	map<string,string>	prm;	//URL parameters
};

//*************************************************
//* UserPage                                      *
//*************************************************
class TWEB;

class UserPg : public TCntrNode, public TConfig, public TPrmTempl::Impl
{
    public:
	//Methods
	UserPg( const string &iid, const string &db, TElem *el );
	~UserPg( );

	TCntrNode &operator=( const TCntrNode &node );

	string	id( )			{ return mId; }
	string	name( );
	string	descr( )		{ return cfg("DESCR").getS(); }
	bool	toEnable( )		{ return mAEn; }
	bool	enableStat( ) const	{ return mEn; }
	string	progLang( );
	string	prog( );
	int	timeStamp( )		{ return mTimeStamp; }

	string	getStatus( );

	string	DB( ) const		{ return mDB; }
	string	tbl( ) const;
	string	fullDB( ) const	{ return DB()+'.'+tbl(); }

	void setName( const string &name )	{ cfg("NAME").setS(name); }
	void setDescr( const string &idsc )	{ cfg("DESCR").setS(idsc); }
	void setToEnable( bool vl )		{ mAEn = vl; modif(); }
	void setEnable( bool vl );
	void setProgLang( const string &ilng );
	void setProg( const string &iprg );

	void setDB( const string &vl )		{ mDB = vl; modifG(); }

	void HTTP( const string &req, SSess &s, TProtocolIn *iprt );

	TWEB &owner( ) const;

	//Attributes
	float	cntReq;
	bool	isDAQTmpl;	// Using the DAQ-template

    protected:
	//Methods
	void load_( TConfig *cfg );
	void loadIO( );
	void save_( );
	void saveIO( );

	bool cfgChange( TCfg &co, const TVariant &pc );

    private:
	//Methods
	const char *nodeName( ) const	{ return mId.getSd(); }

	void cntrCmdProc( XMLNode *opt );	//Control interface command process

	void postDisable( int flag );		//Delete all DB if flag 1

	//Attributes
	TCfg	&mId;
	char	&mAEn, mEn;
	int64_t	&mTimeStamp;
	string	mDB;

	int	ioRez, ioHTTPreq, ioUrl, ioPage, ioSender, ioUser,
		ioHTTPvars, ioURLprms, ioCnts, ioThis, ioPrt;

	bool	chkLnkNeed;	//Check lnk need flag

	ResRW	cfgRes;
	ResMtx	reqRes;
};

//*************************************************
//* TWEB                                          *
//*************************************************
class TWEB: public TUI
{
    public:
	//Methods
	TWEB( string name );
	~TWEB( );

	string defPg( )		{ return mDefPg; }
	void setDefPg( const string &pg )	{ mDefPg = pg; modif(); }

	void modStart( );
	void modStop( );

	// User page's functions
	void uPgList( vector<string> &ls ) const	{ chldList(mPgU,ls); }
	bool uPgPresent( const string &id ) const	{ return chldPresent(mPgU,id); }
	string uPgAdd( const string &id, const string &db = "*.*" );
	void uPgDel( const string &id )			{ chldDel(mPgU,id); }
	AutoHD<UserPg> uPgAt( const string &id ) const	{ return chldAt(mPgU, id); }

	TElem &uPgEl( )		{ return mUPgEl; }
	TElem &uPgIOEl( )	{ return mUPgIOEl; }

	string httpHead( const string &rcode, int cln, const string &cnt_tp = "text/html", const string &addattr = "" );

    protected:
	//Methods
	void load_( );
	void save_( );

    private:
	//Methods
	void colontDown( SSess &ses );

	string pgCreator( TProtocolIn *iprt, const string &cnt, const string &rcode = "", const string &httpattrs = "",
		const string &htmlHeadEls = "", const string &forceTmplFile = "", const string &lang = "" );
	bool pgAccess( TProtocolIn *iprt, const string &URL );

	void HTTP_GET( const string &url, string &page, vector<string> &vars, const string &user, TProtocolIn *iprt );
	void HTTP_POST( const string &url, string &page, vector<string> &vars, const string &user, TProtocolIn *iprt );

	void cntrCmdProc( XMLNode *opt );	//Control interface command process

	void   modInfo( vector<string> &list );
	string modInfo( const string &name );

	//Attributes
	string	mDefPg;
	int	mPgU;

	TElem	mUPgEl, mUPgIOEl;
};

extern TWEB *mod;
}

#endif //WEB_USER_H
