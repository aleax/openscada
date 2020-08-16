
//OpenSCADA module Protocol.SelfSystem file: self.h
/***************************************************************************
 *   Copyright (C) 2007-2020 by Roman Savochenko, <roman@oscada.org>       *
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

#ifndef SELF_H
#define SELF_H

#include <tprotocols.h>

#undef _
#define _(mess) mod->I18N(mess)

#define ERR_NO		"0"
#define ERR_AUTH	"1"
#define ERR_PRC		"2"
#define ERR_CMD		"3"

#define EMPTY_PASS	"<EMPTY>"

using namespace OSCADA;

namespace SelfPr
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
	//Attributes
	string reqBuf;
};

//*************************************************
//* TProt                                         *
//*************************************************
class TProt: public TProtocol
{
    public:
	//Data
	//*****************************************
	//* SAuth                                 *
	class SAuth
	{
	    public:
		//Methods
		SAuth( ) : tAuth(0) { }
		SAuth( time_t itm, const string &inm, const string &isrc = "" ) : tAuth(itm), name(inm), src(isrc) { }

		//Attributes
		time_t	tAuth;
		string	name, src, pHash;
	};

	//Methods
	TProt( string name );
	~TProt( );

	int authTime( )			{ return mTAuth; }
	int singleUserHostLimit( )	{ return mSingleUserHostLimit; }
	int comprLev( )			{ return mComprLev; }
	int comprBrd( )			{ return mComprBrd; }

	void setAuthTime( int vl )	{ mTAuth = vmax(1,vl); modif(); }
	void setSingleUserHostLimit( int vl )	{ mSingleUserHostLimit = vmax(1,vmin(10000,vl)); modif(); }
	void setComprLev( int vl )	{ mComprLev = vmax(-1,vmin(9,vl)); modif(); }
	void setComprBrd( int vl )	{ mComprBrd = vmax(10,vl); modif(); }

	int sesOpen( const string &user, const string &pass, const string &src = "" );
	void sesClose( int idSes );
	SAuth sesGet( int idSes );
	void sesSet( int idSes, const SAuth &auth );

	void outMess( XMLNode &io, TTransportOut &tro );

    protected:
	void load_( );
	void save_( );

    private:
	//Methods
	void cntrCmdProc( XMLNode *opt );
	TProtocolIn *in_open( const string &name );

	//Attributes
	map<int, SAuth>	mAuth;
	int		mTAuth, mComprLev, mComprBrd, mSingleUserHostLimit;

	ResMtx		authRes;
};

extern TProt *mod;
} //End namespace SelfPr

#endif //SELF_H
