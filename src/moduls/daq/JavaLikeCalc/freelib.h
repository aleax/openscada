
//OpenSCADA system module DAQ.JavaLikeCalc file: freelib.h
/***************************************************************************
 *   Copyright (C) 2005-2014 by Roman Savochenko, <rom_as@oscada.org>      *
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
class TipContr;

class Lib : public TCntrNode, public TConfig
{
    public:
	Lib( const string &id, const string &name, const string &lib_db );
	~Lib( );

	TCntrNode &operator=( TCntrNode &node );

	string id( )		{ return mId; }
	string name( );
	string descr( )		{ return cfg("DESCR").getS(); }
	bool startStat( )	{ return run_st; }
	bool progTr( )		{ return mProgTr; }

	string DB( )		{ return work_lib_db; }
	string tbl( )		{ return cfg("DB").getS(); }
	string fullDB( )	{ return DB()+'.'+tbl(); }

	void setName( const string &inm )	{ cfg("NAME").setS(inm); }
	void setDescr( const string &idscr )	{ cfg("DESCR").setS(idscr); }
	void setStart( bool val );
	void setFullDB( const string &idb );
	void setProgTr( bool val )		{ mProgTr = val; modif(); }

	void list( vector<string> &ls )		{ chldList(mFnc,ls); }
	bool present( const string &id )	{ return chldPresent(mFnc,id); }
	AutoHD<Func> at( const string &id )	{ return chldAt(mFnc,id); }
	void add( const string &id, const string &name = "" );
	void del( const string &id );

	void copyFunc( const string &f_id, const string &l_id, const string &to_id, const string &to_name );

	TVariant objFuncCall( const string &id, vector<TVariant> &prms, const string &user );

    protected:
	const char *nodeName( )	{ return mId.getSd(); }
	void cntrCmdProc( XMLNode *opt );       //Control interface command process

	void load_( );
	void save_( );

	void preDisable( int flag );
	void postDisable( int flag );
	bool cfgChange( TCfg &co, const TVariant &pc )	{ modif(); return true; }

    private:
	bool	run_st;
	int	mFnc;
	string	work_lib_db;
	TCfg	&mId;
	char	&mProgTr;
};

} //End namespace FreeLib

#endif //FREELIB_H
