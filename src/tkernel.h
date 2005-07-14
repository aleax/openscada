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

#ifndef TKERNEL_H
#define TKERNEL_H

#include <stdio.h>

#include <string>
#include <vector>

#include "tbds.h"

using std::string;
using std::vector;

class    TUIS;
class    TArchiveS;
class    TControllerS;
class    TProtocolS;
class    TTransportS;
class    TSpecialS;
class    TParamS;
class    TModSchedul;
class    TSequrity;
class    TFunctionS;

class TKernel : public TCntrNode 
{
    /** Public methods: */
    public:
	TKernel( const string &name );
	~TKernel(  );

	TUIS         &ui()     	{ return (*m_ui); }
	TArchiveS    &archive()	{ return (*m_archive); } 
	TBDS         &db()     	{ return (*m_bd); }
	TControllerS &controller()	{ return (*m_controller); }
	TProtocolS   &protocol()   	{ return (*m_protocol); }
	TTransportS  &transport()  	{ return (*m_transport); }
	TSpecialS    &special()    	{ return (*m_special); }
	TParamS      &param()      	{ return (*m_param); }
	TModSchedul  &modSchedul() 	{ return (*m_modschedul); }
	TSequrity    &sequrity()  	{ return (*m_sequrity); }
	TFunctionS   &func()	{ return (*m_func); }
	
	//Load/Start
	void load( );
	void start( bool val );
	// Print comand line options! 
	string optDescr( );
    	// Get XML kernel node
	string cfgNodeName();
    	XMLNode *cfgNode();
	// Kernel name
    	string &name() { return( m_name ); }
        //================== Message functions ========================
	void mPut( const string &categ, TMess::Type level, char *fmt,  ... );
	void mPutS( const string &categ, TMess::Type level, const string &mess );

	//BD default prepare
	TBDS::SName nameDBPrep( const TBDS::SName &nbd );
	
	//Use individual DB
	bool genDB( ) { return m_genDB; }
	
    /** Private attributes: */
    private:
	string nodeName(){ return name(); }
        //================== Controll functions ========================
	void cntrCmd_( const string &a_path, XMLNode *opt, int cmd );
 	TCntrNode &ctrAt( const string &br );
	
    /** Private methods: */
    private:    
	string       m_name;
	
	string 	     DefBDType;
	string       DefBDName;		
	
	TUIS         *m_ui;
	TArchiveS    *m_archive;
	TBDS         *m_bd;
	TControllerS *m_controller;
	TProtocolS   *m_protocol;
	TTransportS  *m_transport;
	TSpecialS    *m_special;
	TParamS      *m_param;
	TModSchedul  *m_modschedul;
	TSequrity    *m_sequrity;
	TFunctionS   *m_func;

	bool         s_run;
	bool	     m_genDB;
};

#endif // TKERNEL_H
