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

#ifndef TPROTOCOLS_H
#define TPROTOCOLS_H

#define  VER_PROT 1    //ProtocolS type modules version

#include <string>

#include "tgrpmodule.h"

using std::string;

//================================================================
//=========== TProtocolIn ========================================
//================================================================
class TProtocol;

class TProtocolIn : public TCntrNode
{
    /** Public methods: */
    public:
	TProtocolIn( const string &name, TProtocol *owner );
	virtual ~TProtocolIn();
	
	string &name(){ return(m_name); }
	
	// process input messages
	virtual bool mess( const string &request, string &answer, const string &sender )
	{ answer = ""; }
	
	//Owner
	TProtocol &owner(){ return *(TProtocol *)nodePrev(); }
	
    protected:
	string nodeName(){ return m_name; }
	
    private:    
	string            m_name;
};

//================================================================
//=========== TProtocol ==========================================
//================================================================
class TProtocol: public TModule
{
    /** Public methods: */
    public:
	TProtocol( );
	virtual ~TProtocol();

	// Input protocols
	void list( vector<string> &list )	{ chldList(m_pr,list); }
        bool openStat( const string &name )	{ return chldAvoid(m_pr,name); } 
	void open( const string &name );
	void close( const string &name );
	AutoHD<TProtocolIn> at( const string &name )
	{ return chldAt(m_pr,name); }
	
    private:
	virtual TProtocolIn *in_open( const string &name )
	{throw TError(nodePath().c_str(),"Function 'in_open' no support!"); }		
	
    private:
	int	m_pr;
};

//================================================================
//=========== TProtocolS =========================================
//================================================================

class TProtocolS : public TSubSYS
{
    /** Public methods: */
    public:
	TProtocolS( TSYS *app );
	~TProtocolS( );
    
	int subVer( ) { return(VER_PROT); }
	void subLoad( );

	string optDescr( );	

    /** Private methods: */
    private:
	//================== Controll functions ========================
	void cntrCmd_( const string &a_path, XMLNode *opt, TCntrNode::Command cmd );
};

#endif // TPROTOCOLS_H
