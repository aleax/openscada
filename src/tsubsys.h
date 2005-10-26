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

#ifndef TGRPMODULE_H
#define TGRPMODULE_H

#include <string>
#include <vector>

#include "tmodule.h"

using std::string;
using std::vector;

class TModSchedul;
class TSYS;

class TSubSYS : public TCntrNode
{
    /** Public methods: */
    public:
	TSubSYS( char *id, char *name, bool modi = false );
	virtual ~TSubSYS(  );
	
	string subId()		{ return m_id; }
	string subName();
	
	bool subModule()	{ return m_mod_sys; }	//Module subsystem
	
	virtual int subVer( )	{ return 0; }		//Type/grp module version
	
	//Start procedures	
	virtual void subLoad( );
	virtual void subSave( );
	virtual void subStart( );
	virtual void subStop( );
    
	// Modules
	void modList( vector<string> &list )	{ chldList(m_mod,list); }
        bool modPresent( const string &name )	{ return chldPresent(m_mod,name); }
	void modAdd( TModule *modul );
	void modDel( const string &name );
        AutoHD<TModule> modAt( const string &name )
	{ return chldAt(m_mod,name); }           
	
	TSYS &owner()		{ return *(TSYS *)nodePrev(); }

    /** Protected methods: */
    protected:
        //================== Controll functions ========================
	void cntrCmd_( const string &a_path, XMLNode *opt, TCntrNode::Command cmd );
	AutoHD<TCntrNode> ctrAt( const string &br );

    /** Private methods: */
    private:
	string nodeName()       { return subId(); }
	
    /** Private Attributes: */
    private:
	bool	m_mod_sys;
	int	m_mod;
	
	string	m_id;		//Id
	string	m_name;		//Name
};

#endif // TGRPMODULE_H
