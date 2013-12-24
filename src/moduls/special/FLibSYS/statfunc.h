
//OpenSCADA system module Special.FLibSYS file: statfunc.h
/***************************************************************************
 *   Copyright (C) 2005-2010 by Roman Savochenko                           *
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

#ifndef STATFUNC_H
#define STATFUNC_H

#include <string>
#include <vector>

#include <tspecials.h>
#include <tarchval.h>

#undef _
#define _(mess) mod->I18N(mess)

using std::string;
using std::vector;
using namespace OSCADA;

namespace FLibSYS
{

//*************************************************
//* Lib: Complex1 functions library               *
//*************************************************
class Lib : public TSpecial
{
    public:
	//Methods
	Lib( string src );
	~Lib( );

	void modStart( );
	void modStop( );

	void list( vector<string> &ls ) 	{ chldList(mFnc,ls); }
	bool present( const string &id )	{ return chldPresent(mFnc,id); }
	AutoHD<TFunction> at( const string &id ){ return chldAt(mFnc,id); }
	void reg( TFunction *fnc )		{ chldAdd(mFnc,fnc); }
	void unreg( const char *id )		{ chldDel(mFnc,id); }

	TVariant objFuncCall( const string &id, vector<TVariant> &prms, const string &user );

    private:
	//Methods
	void postEnable( int flag );
	void cntrCmdProc( XMLNode *opt );       //Control interface command process

	//Attributes
	int	mFnc;
};

extern Lib *mod;

} //End namespace FLibSYS

#endif //STATFUNC_H
