
//OpenSCADA system file: ttipdaq.h
/***************************************************************************
 *   Copyright (C) 2003-2008 by Roman Savochenko                           *
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

#ifndef TTIPCONTROLLER_H
#define TTIPCONTROLLER_H

#include <string>
#include <vector>

#include "tcontroller.h"

using std::string;
using std::vector;

//************************************************
//* TTipDAQ                                      *
//************************************************
class TTipParam;

class TTipDAQ : public TModule, public TElem
{
    public:
	TTipDAQ( );
	virtual ~TTipDAQ();

	void modStart( );
	void modStop( );

	//- Controllers -
	void list( vector<string> &list )	{ chldList(m_cntr,list); }
	bool present( const string &name )	{ return chldPresent(m_cntr,name); }
	void add( const string &name, const string &daq_db = "*.*" );
	void del( const string &name )		{ chldDel(m_cntr,name); }
	AutoHD<TController> at( const string &name, const string &who = "" )
	{ return chldAt(m_cntr,name); }

	//- Parameter types (DB structure) -
	bool tpPrmPresent( const string &name_t );
	int tpPrmToId( const string &name_t );
	int tpParmAdd( const char *id, const char *n_db, const char *name );
	unsigned tpPrmSize( )			{ return( paramt.size()); }
	TTipParam &tpPrmAt( unsigned id );

	//- Compile functions support API -
	virtual void compileFuncLangs( vector<string> &ls )	{ }
	virtual string compileFunc( const string &lang, TFunction &fnc_cfg, const string &prog_text, const string &usings = "" );

    protected:
	void cntrCmdProc( XMLNode *opt );       //Control interface command process
	virtual TController *ContrAttach( const string &name, const string &daq_db );

    private:
	vector<TTipParam *>   paramt;  // List type parameter and Structure configs of parameter.
	int	m_cntr;
};

#endif // TTIPCONTROLLER_H
