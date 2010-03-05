
//OpenSCADA system module Protocol.UserProtocol file: user_prt.h
/***************************************************************************
 *   Copyright (C) 2010 by Roman Savochenko                                *
 *   rom_as@oscada.org, rom_as@fromru.com                                  *
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

	bool mess( const string &request, string &answer, const string &sender );

	TProt &owner( );

    private:
	//Attributes
	AutoHD<UserPrt> up;
	TValFunc funcV;
};

//*************************************************
//* UserProtocol::UserPrt:                        *
//*************************************************
class UserPrt : public TCntrNode, public TConfig
{
    public:
	//Methods
	UserPrt( const string &iid, const string &db, TElem *el );
	~UserPrt( );

	TCntrNode &operator=( TCntrNode &node );

	const string &id( )	{ return mId; }
	string name( );
	string descr( )		{ return mDscr; }
	bool toEnable( )	{ return mAEn; }
	bool enableStat( )	{ return mEn; }
	string inTransport( );
	string inProgLang( );
	string inProg( );
	string outTransport( );
	string outProgLang( );
	string outProg( );
	string workInProg( )	{ return mWorkInProg; }
	string workOutProg( )	{ return mWorkOutProg; }

	string getStatus( );

	string DB( )		{ return mDB; }
	string tbl( );
	string fullDB( )	{ return DB()+'.'+tbl(); }

	void setName( const string &name )	{ mName = name; modif(); }
	void setDescr( const string &idsc )	{ mDscr = idsc; modif(); }
	void setToEnable( bool vl )		{ mAEn = vl; modif(); }
	void setEnable( bool vl );
	void setInTransport( const string &il );
	void setInProgLang( const string &ilng );
	void setInProg( const string &iprg );
	void setOutTransport( const string &il );
	void setOutProgLang( const string &ilng );
	void setOutProg( const string &iprg );

	void setDB( const string &vl )		{ mDB = vl; modifG(); }

	TProt &owner( );

	//Attributes
	float	cntInReq, cntOutReq;

    protected:
	//Methods
	void load_( );
	void save_( );

	bool cfgChange( TCfg &cfg );

    private:
	//Methods
	string nodeName( )	{ return mId; }

	void cntrCmdProc( XMLNode *opt );	//Control interface command process

	void postDisable( int flag );		//Delete all DB if flag 1

	//Attributes
	string	&mId, &mName, &mDscr;
	bool	&mAEn, mEn;
	string	mDB, mWorkInProg, mWorkOutProg;
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

	//> User protocol's functions
	void uPrtList( vector<string> &ls )	{ chldList(mPrtU,ls); }
	bool uPrtPresent( const string &id )	{ return chldPresent(mPrtU,id); }
	void uPrtAdd( const string &id, const string &db = "*.*" );
	void uPrtDel( const string &id )	{ chldDel(mPrtU,id); }
	AutoHD<UserPrt> uPrtAt( const string &id )	{ return chldAt(mPrtU,id); }

	TElem &uPrtEl( )	{ return mUPrtEl; }

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

	TElem	mUPrtEl;
};

extern TProt *mod;

} //End namespace UserProtocol

#endif //USER_PRT_H
