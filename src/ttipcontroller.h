/***************************************************************************
 *   Copyright (C) 2004 by Roman Savochenko                                *
 *   rom_as@fromru.com                                                     *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
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

class TTipParam;

class TTipController : public TModule, public TElem
{
    /** Public methods: */
    public:
	TTipController( );
	virtual ~TTipController();
    
	// Controllers
	void list( vector<string> &list )	{ chldList(m_cntr,list); }
	bool avoid( const string &name )	{ return chldAvoid(m_cntr,name); }
	void add( const string &name, const TBDS::SName &bd );
	void del( const string &name )		{ chldDel(m_cntr,name); }
	AutoHD<TController> at( const string &name, const string &how = "" )
	{ return chldAt(m_cntr,name); }
	
	unsigned tpPrmToId( const string &name_t );
	unsigned tpPrmSize( ) { return( paramt.size()); }
	TTipParam &tpPrmAt( unsigned id )
	{ if(id >= paramt.size()) throw TError("%s: id of param type error!",o_name); return( *paramt[id]); }
	int tpParmAdd( const char *id, const char *n_db, const char *name );
	
    /** Protected methods: */
    protected: 
	virtual TController *ContrAttach( const string &name, const TBDS::SName &bd )
	{ throw TError("%s: Error controller %s attach!",o_name,name.c_str()); }
	//================== Controll functions ========================
	void cntrCmd_( const string &a_path, XMLNode *opt, int cmd );
	AutoHD<TCntrNode> ctrAt1( const string &br );
    
    /** Private atributes: */
    private:    
	vector<TTipParam *>   paramt;  // List type parameter and Structure configs of parameter.
	int	m_cntr;

	static const char *o_name;
};

#endif // TTIPCONTROLLER_H

