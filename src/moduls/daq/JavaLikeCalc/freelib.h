
//OpenSCADA system module DAQ.JavaLikeCalc file: freelib.h
/***************************************************************************
 *   Copyright (C) 2005-2008 by Roman Savochenko                           *
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

#ifndef FREELIB_H
#define FREELIB_H

#include <string>
#include <vector>

#include <tcntrnode.h>
#include <freefunc.h>

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
	Lib( const char *id, const char *name, const string &lib_db );
	~Lib( );

	TCntrNode &operator=( TCntrNode &node );

	const string &id( )	{ return mId; }
	string name( );
	string descr( )		{ return mDescr; }
	bool startStat( )	{ return run_st; }

	string DB( )		{ return work_lib_db; }
	string tbl( )		{ return mDB; }
	string fullDB( )	{ return DB()+'.'+tbl(); }

	void setName( const string &inm )	{ mName = inm; modif(); }
	void setDescr( const string &idscr )	{ mDescr = idscr; modif(); }
	void setStart( bool val );
	void setFullDB( const string &idb );

	void list( vector<string> &ls )		{ chldList(mFnc,ls); }
	bool present( const string &id )	{ return chldPresent(mFnc,id); }
	AutoHD<Func> at( const string &id )	{ return chldAt(mFnc,id); }
	void add( const char *id, const char *name = "" );
	void del( const char *id );

	void copyFunc( const string &f_id, const string &l_id, const string &to_id, const string &to_name );

    protected:
	string nodeName( )	{ return mId; }
	void cntrCmdProc( XMLNode *opt );       //Control interface command process

	void load_( );
	void save_( );

	void preDisable( int flag );
	void postDisable( int flag );

    private:
	bool	run_st;
	int	mFnc;
	string	&mId, &mName, &mDescr, &mDB, work_lib_db;
};

} //End namespace FreeLib

#endif //FREELIB_H
