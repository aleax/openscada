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

#include "tcntrnode.h"
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

	TUIS         &UI()     	{ return (*ui); }
	TArchiveS    &Archive()	{ return (*archive); } 
	TBDS         &BD()     	{ return (*bd); }
	TControllerS &Controller()	{ return (*controller); }
	TProtocolS   &Protocol()   	{ return (*protocol); }
	TTransportS  &Transport()  	{ return (*transport); }
	TSpecialS    &Special()    	{ return (*special); }
	TParamS      &Param()      	{ return (*param); }
	TModSchedul  &ModSchedul() 	{ return (*modschedul); }
	TSequrity    &Sequrity()  	{ return (*sequrity); }
	TFunctionS   &func()	{ return (*m_func); }
	
	// Run server
	int run(  );
	// Get optio from generic config file.
	void updateOpt();
	// Update comand line option
    	void checkCommandLine(bool mode = false );
	// Print comand line options! 
	string optDescr( );
    	// Get XML kernel node
    	XMLNode *cfgNode();
	// Kernel name
    	string &name() { return( m_name ); }
        //================== Message functions ========================
	void mPut( const string &categ, int level, char *fmt,  ... );
	void mPutS( const string &categ, int level, const string &mess );

	//BD default prepare
	TBDS::SName nameDBPrep( const TBDS::SName &nbd );
	
	//Use individual DB
	bool genDB( ) { return m_genDB; }
	
    /**Attributes: */
    public:
	string DefBDType;
	string DefBDName;    
	
    /** Private attributes: */
    private:
	string nodeName(){ return name(); }
        //================== Controll functions ========================
	void ctrStat_( XMLNode *inf );
	void ctrDinGet_( const string &a_path, XMLNode *opt );
	void ctrDinSet_( const string &a_path, XMLNode *opt );
 	TCntrNode &ctrAt( const string &br );
    /** Private methods: */
    private:    
	string       m_name;
	
	TUIS         *ui;
	TArchiveS    *archive;
	TBDS         *bd;
	TControllerS *controller;
	TProtocolS   *protocol;
	TTransportS  *transport;
	TSpecialS    *special;
	TParamS      *param;
	TModSchedul  *modschedul;
	TSequrity    *sequrity;
	TFunctionS   *m_func;

	bool         s_run;
	bool	     m_genDB;

	static const char *o_name;
};

#endif // TKERNEL_H
