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

#include "thd.h"
#include "tkernel.h"
#include "tmessage.h"
#include "tmodule.h"
#include "tgrpmodule.h"

using std::string;



//================================================================
//=========== TProtocolIn ========================================
//================================================================
class TProtocol;

class TProtocolIn
{
    /** Public methods: */
    public:
	TProtocolIn( string name, TProtocol *owner );
	virtual ~TProtocolIn();
	
	// Name of used transport process/pthread
	string &Name(){ return(m_name); }
	//Owner
	TProtocol &Owner(){ return( *m_owner ); }
       	// Input message no complit, wait left message 
	bool wait() { return( m_wait ); }
	// process input messages
	virtual bool mess(string &request, string &answer, string sender )
	{ answer = ""; }
    protected:
	bool              m_wait;    
	
    private:    
	string            m_name;
	TProtocol         *m_owner;
    
	static const char *o_name;
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

	// List opened input object protocols
	void list( vector<string> &list ) { m_hd.obj_list( list ); }
	// Open input protocol.
	unsigned open( string name );
    	// Close input protocol.
	void close( unsigned hd );
	// Get input protocol object.	
	TProtocolIn &at(unsigned hd) { return( *(TProtocolIn *)m_hd.hd_at( hd ) ); }
	TProtocolIn &operator[](unsigned hd ) { return(at(hd)); }	
	// process input messages
	//virtual void in_mess(string &request, string &answer )
	//{ answer = ""; }
    private:
	virtual TProtocolIn *in_open( string name )
	{throw TError("(%s) Function 'in_open' no support!",o_name); }		
	
    private:
	THD               m_hd;
    
	static const char *o_name;
};

//================================================================
//=========== TProtocolS =========================================
//================================================================

class TProtocolS : public TGRPModule
{
    /** Public methods: */
    public:
	TProtocolS( TKernel *app );
    
	int gmd_Ver( ) { return(VER_PROT); }

	TProtocol &gmd_at( unsigned hd )     { return( (TProtocol &)TGRPModule::gmd_at(hd) ); }
	TProtocol &operator[]( unsigned hd ) { return( gmd_at(hd) ); }

	void gmd_CheckCommandLine( );
	void gmd_UpdateOpt();

    /** Private methods: */
    private:
	string opt_descr( );
    
	//================== Controll functions ========================
	void ctr_fill_info( XMLNode *inf );
	void ctr_din_get_( string a_path, XMLNode *opt );
    /** Private atributes: */
    private:
	static const char *i_cntr;
	static const char *o_name;
};

#endif // TPROTOCOLS_H
