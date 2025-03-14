
//OpenSCADA module DAQ.JavaLikeCalc file: freelib.h
/***************************************************************************
 *   Copyright (C) 2005-2021 by Roman Savochenko, <roman@oscada.org>       *
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

#ifndef FREELIB_H
#define FREELIB_H

#include <string>
#include <vector>

#include <tcntrnode.h>

#include "freefunc.h"

using std::string;
using std::vector;

namespace JavaLikeCalc
{

//*************************************************
//* Lib: Free library                             *
//*************************************************
class TpContr;

class Lib : public TCntrNode, public TConfig
{
    public:
	//Methods
	Lib( const string &id, const string &name, const string &lib_db );
	~Lib( );

	TCntrNode &operator=( const TCntrNode &node );

	string id( ) const	{ return mId; }
	string name( );
	string descr( )		{ return cfg("DESCR").getS(); }
	bool startStat( ) const	{ return runSt; }

	string DB( bool qTop = false ) const	{ return storage(mDB, qTop); }
	string tbl( ) const			{ return cfg("DB").getS().empty() ? ("flb_"+id()) : cfg("DB").getS(); }
	string fullDB( bool qTop = false ) const { return DB(qTop)+'.'+tbl(); }

	void setName( const string &inm )	{ cfg("NAME").setS(inm); }
	void setDescr( const string &idscr )	{ cfg("DESCR").setS(idscr); }
	void setStart( bool val );
	void setDB( const string &vl, bool qTop = false ) { setStorage(mDB, vl, qTop); if(!qTop) modifG(); }
	void setFullDB( const string &idb );

	void list( vector<string> &ls ) const		{ chldList(mFnc, ls); }
	bool present( const string &id ) const		{ return chldPresent(mFnc, id); }
	AutoHD<Func> at( const string &id ) const	{ return chldAt(mFnc, id); }
	void add( const string &id, const string &name = "" );
	void del( const string &id );

	void copyFunc( const string &f_id, const string &l_id, const string &to_id, const string &to_name );

	TVariant objFuncCall( const string &id, vector<TVariant> &prms, const string &user_lang );

    protected:
	//Methods
	const char *nodeName( ) const	{ return mId.getSd(); }
	void cntrCmdProc( XMLNode *opt );	//Control interface command process

	void load_( TConfig *cfg );
	void save_( );

	void preDisable( int flag );
	void postDisable( int flag );
	bool cfgChange( TCfg &co, const TVariant &pc )	{ modif(); return true; }

    private:
	//Methods
	bool isStdStorAddr( ) const		{ return (tbl() == ("flb_"+id())); }	//????[v1.0] Remove

	//Attributes
	bool	runSt;
	int	mFnc;
	string	mDB;
	TCfg	&mId;
};

} //End namespace FreeLib

#endif //FREELIB_H
