
//OpenSCADA system file: tmodule.h
/***************************************************************************
 *   Copyright (C) 2003-2010 by Roman Savochenko                           *
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

#ifndef TMODULE_H
#define TMODULE_H

#include <string>
#include <vector>

#include "tmess.h"
#include "tcntrnode.h"

using std::string;
using std::vector;

namespace OSCADA
{

//*************************************************
//* TModule                                       *
//*************************************************
class TSubSYS;

class TModule : public TCntrNode
{
    friend class TSubSYS;
    public:
	//Data
	//*****************************************
	//* SAt: Attach module struct             *
	class SAt
	{
	    public:
		//Methods
		SAt( )	{ }
		SAt( const string &iid, const string &itype = "", int itver = 0 ) :
		    id(iid), type(itype), t_ver(itver)	{ }

		bool operator==( const TModule::SAt &amst ) const
		{
		    if( amst.id == id && amst.type == type && amst.t_ver == t_ver ) return true;
		    return false;
		}

		//Attributes
		string id;          //id module
		string type;        //Type module
		int    t_ver;       //Type version module
	};

	//*****************************************
	//* ExpFunc                               *
	class ExpFunc
	{
	    public:
		ExpFunc( const string &iprot, const string &idscr, void (TModule::*iptr)() ) :
		    prot(iprot), dscr(idscr), ptr(iptr)	{ };
		string prot;		//Prototip
		string dscr;		//Description
		void (TModule::*ptr)();	//Adress
	};

	//Methods
	TModule( const string &id );
	virtual ~TModule(  );

	string	objName( );

	string	modId( )		{ return mId.c_str(); }
	string	modName( );

	virtual void modStart( )	{ }
	virtual void modStop( )		{ }

	virtual void   modInfo( vector<string> &list );
	virtual string modInfo( const string &name );

	virtual void perSYSCall( unsigned int cnt )	{ }

	//> Export functions
	void modFuncList( vector<string> &list );
	bool modFuncPresent( const string &prot );
	ExpFunc &modFunc( const string &prot );
	void modFunc( const string &prot, void (TModule::**offptr)() );

	//> Translate functions
	const char *I18N( const char *mess );

	TSubSYS &owner( );

    protected:
	//Methods
	void postEnable( int flag );
	void cntrCmdProc( XMLNode *opt );	//Control interface command process

	//> Reg export function
	void modFuncReg( ExpFunc *func ){ m_efunc.push_back(func); }

	//Attributes
	string mName;				// Name
	string mDescr;				// Describe
	string mType;				// Type
	string mVers;				// Version
	string mAuthor;				// Author
	string mLicense;			// License
	string mSource;				// Source of module (SO, in build, ....)

    private:
	//Methods
	const char *nodeName( )	{ return mId.c_str(); }

	//Attributes
	const string	mId;		// Identificator
	string		lc_id;		// Locale id. For gettext.
	vector<ExpFunc *> m_efunc;	// Export function list

	static const char *l_info[];	// list of the present info options
};

}

#endif // TMODULE_H
