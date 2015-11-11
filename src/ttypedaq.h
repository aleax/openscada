
//OpenSCADA system file: ttypedaq.h
/***************************************************************************
 *   Copyright (C) 2003-2014 by Roman Savochenko, <rom_as@oscada.org>      *
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

#ifndef TTIPCONTROLLER_H
#define TTIPCONTROLLER_H

#include <string>
#include <vector>

#include "tcontroller.h"

using std::string;
using std::vector;

namespace OSCADA
{

//************************************************
//* TTypeDAQ                                     *
//************************************************
class TTypeParam;

class TTypeDAQ : public TModule, public TElem
{
    public:
	//Public methods
	TTypeDAQ( const string &id );
	virtual ~TTypeDAQ( );

	string objName( );

	string DAQPath( );

	void modStart( );
	void modStop( );

	// Controllers
	void list( vector<string> &list )	{ chldList(mCntr,list); }
	bool present( const string &name )	{ return chldPresent(mCntr,name); }
	void add( const string &name, const string &daq_db = "*.*" );
	void del( const string &name )		{ chldDel(mCntr,name); }
	AutoHD<TController> at( const string &name, const string &who = "" )
	{ return chldAt(mCntr,name); }

	// Parameter types (DB structure)
	bool tpPrmPresent( const string &name_t );
	int tpPrmToId( const string &name_t );
	int tpParmAdd( const char *id, const char *n_db, const char *name, bool isPrmCntr = false );
	int tpParmAdd( TTypeParam *tp );
	unsigned tpPrmSize( )			{ return( paramt.size()); }
	TTypeParam &tpPrmAt( unsigned id );

	// Compile functions support API
	virtual bool compileFuncLangs( vector<string> *ls = NULL )	{ return false; }
	virtual void compileFuncSynthHighl( const string &lang, XMLNode &shgl )	{ }
	virtual string compileFunc( const string &lang, TFunction &fnc_cfg, const string &prog_text, const string &usings = "", int maxCalcTm = 0 );

	// Redundancy
	virtual bool redntAllow( )		{ return false; }

	TDAQS &owner( );

    protected:
	//Protected methods
	void postEnable( int flag );
	void cntrCmdProc( XMLNode *opt );	//Control interface command process
	virtual TController *ContrAttach( const string &name, const string &daq_db );

    private:
	//Private attributes
	vector<TTypeParam *>	paramt;		// List type parameter and Structure configurations of parameter.
	int	mCntr;
};

}

#endif // TTIPCONTROLLER_H
