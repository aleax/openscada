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

class TModule;
class TModSchedul;
class TKernel;

class TGRPModule : public TCntrNode
{
    /** Public methods: */
    public:
	TGRPModule( TKernel *app, char *id, char *name );
    
	virtual ~TGRPModule(  );
	
	string gmdId()		{ return m_id; }
	string gmdName();
	
	virtual int gmdVer( ) = 0;	// Type/grp module version
	
	//Start procedures	
	virtual void gmdLoad( );
	virtual void gmdStart( );
	virtual void gmdStop( );
    
	// Modules
	void gmdList( vector<string> &list )	{ chldList(m_mod,list); }
        bool gmdAvoid( const string &name )	{ return chldAvoid(m_mod,name); }
	void gmdAdd( TModule *modul );
	void gmdDel( const string &name );
        AutoHD<TModule> gmdAt( const string &name )
	{ return chldAt(m_mod,name); }           
	
	// Get XML section node
	XMLNode *gmdCfgNode();
	string cfgNodeName();
    
	//string &gmdModPath() { return(DirPath); }
	
	//================== Message functions ========================
        void mPut( const string &categ, TMess::Type level, char *fmt,  ... );
        void mPutS( const string &categ, TMess::Type level, const string &mess );			
    
	TKernel &owner() { return(*m_owner); }

    /** Protected methods: */
    protected:
        //================== Controll functions ========================
	void cntrCmd_( const string &a_path, XMLNode *opt, int cmd );
	AutoHD<TCntrNode> ctrAt1( const string &br );
	
    /** Protected Attributes: */
    protected:    
	//string          DirPath;
	
    /** Private Attributes: */
    private:
	TKernel		*m_owner;
	int		m_mod;
	
	//string          nameType;       //Id
	string          m_id;		//Id
	string          m_name;		//Name
};

#endif // TGRPMODULE_H
