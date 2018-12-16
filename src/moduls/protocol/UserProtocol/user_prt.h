
//OpenSCADA module Protocol.UserProtocol file: user_prt.h
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

#ifndef USER_PRT_H
#define USER_PRT_H

#include <string>
#include <map>

#include <tprotocols.h>

#undef _
#define _(mess) mod->I18N(mess)

using std::string;
using std::map;
using namespace OSCADA;

namespace UserProtocol
{
class TProt;
class UserPrt;

//*************************************************
//* UserProtocol::TProtIn                         *
//*************************************************
class TProtIn: public TProtocolIn
{
    public:
	//Methods
	TProtIn( string name );
	~TProtIn( );

	unsigned waitReqTm( );

	void setSrcTr( TTransportIn *vl );

	bool mess( const string &request, string &answer );

	TProt &owner( ) const;

	//Attributes
	string	req;

    private:
	//Attributes
	//TValFunc funcV;
	AutoHD<UserPrt> up;
};

//*************************************************
//* UserProtocol::UserPrt:                        *
//*************************************************
class UserPrt : public TCntrNode, public TConfig, public TPrmTempl::Impl
{
    public:
	//Methods
	UserPrt( const string &iid, const string &db, TElem *el );
	~UserPrt( );

	TCntrNode &operator=( const TCntrNode &node );

	string id( )		{ return mId; }
	string name( );
	string descr( )		{ return cfg("DESCR").getS(); }
	bool toEnable( )	{ return mAEn; }
	bool enableStat( ) const{ return mEn; }
	bool progTr( )		{ return cfg("PR_TR"); }
	string DAQTmpl( )	{ return cfg("DAQTmpl"); }
	unsigned waitReqTm( )	{ return mWaitReqTm; }
	string inProgLang( );
	string inProg( );
	string outProgLang( );
	string outProg( );
	string workOutProg( )	{ return mWorkOutProg; }
	int64_t timeStamp( )	{ return mTimeStamp; }

	string getStatus( );

	string DB( ) const		{ return mDB; }
	string tbl( ) const;
	string fullDB( ) const	{ return DB()+'.'+tbl(); }

	void setName( const string &name )	{ cfg("NAME").setS(name); }
	void setDescr( const string &idsc )	{ cfg("DESCR").setS(idsc); }
	void setToEnable( bool vl )		{ mAEn = vl; modif(); }
	void setEnable( bool vl );
	void setProgTr( bool vl )		{ cfg("PR_TR") = vl; }
	void setDAQTmpl( const string &vl )	{ cfg("DAQTmpl").setS(vl); }
	void setWaitReqTm( unsigned vl )	{ mWaitReqTm = vmax(0,vmin(100000,vl)); modif(); }
	void setInProgLang( const string &ilng );
	void setInProg( const string &iprg );
	void setOutProgLang( const string &ilng );
	void setOutProg( const string &iprg );

	void setDB( const string &vl )		{ mDB = vl; modifG(); }

	bool inMess( const string &reqst, string &answer, TProtIn *prt );
	void outMess( XMLNode &io, TTransportOut &tro );

	TProt &owner( ) const;

	//Attributes
	float	cntInReq, cntOutReq;

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
	int64_t	&mWaitReqTm, &mTimeStamp;
	string	mDB, mWorkOutProg;

	//bool	isDAQTmpl;	// DAQ template used for the input part
	int	ioRez, ioReq, ioAnsw, ioSend, ioTr, ioIO;

	bool	chkLnkNeed;	//Check lnk need flag

	ResRW	inCfgRes;
	ResMtx	inReqRes;
};

//*************************************************
//* UserProtocol::TProt                           *
//*************************************************
class TProt: public TProtocol
{
    public:
	//Methods
	TProt( string name );
	~TProt( );

	void modStart( );
	void modStop( );

	void itemListIn( vector<string> &ls, const string &curIt = "" );

	// User protocol's functions
	void uPrtList( vector<string> &ls ) const		{ chldList(mPrtU, ls); }
	bool uPrtPresent( const string &id ) const		{ return chldPresent(mPrtU, id); }
	string uPrtAdd( const string &id, const string &db = "*.*" );
	void uPrtDel( const string &id )			{ chldDel(mPrtU, id); }
	AutoHD<UserPrt> uPrtAt( const string &id ) const	{ return chldAt(mPrtU, id); }

	TElem &uPrtEl( )	{ return mUPrtEl; }
	TElem &uPrtIOEl( )	{ return mUPrtIOEl; }

	void outMess( XMLNode &io, TTransportOut &tro );

    protected:
	//Methods
	void load_( );
	void save_( );

    private:
	//Methods
	TProtocolIn *in_open( const string &name );

	void cntrCmdProc( XMLNode *opt );	//Control interface command process

	//Attributes
	int	mPrtU;

	TElem	mUPrtEl, mUPrtIOEl;
};

extern TProt *mod;

} //End namespace UserProtocol

#endif //USER_PRT_H
