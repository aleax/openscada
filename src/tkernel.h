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

#include "xml.h"
#include "tcontr.h"

using std::string;
using std::vector;

class    TUIS;
class    TArhiveS;
class    TBDS;
class    TControllerS;
class    TProtocolS;
class    TTransportS;
class    TSpecialS;
class    TParamS;
class    TModSchedul;
class    TSequrity;

class TKernel : public TContr 
{
    /** Public methods: */
    public:
	TKernel( const string &name );
	~TKernel(  );

	TUIS         &UI()     { return (*ui); }
	TArhiveS     &Arhive() { return (*arhive); } 
	TBDS         &BD()     { return (*bd); }
	TControllerS &Controller() { return (*controller); }
	TProtocolS   &Protocol()   { return (*protocol); }
	TTransportS  &Transport()  { return (*transport); }
	TSpecialS    &Special()    { return (*special); }
	TParamS      &Param()      { return (*param); }
	TModSchedul  &ModSchedul() { return (*modschedul); }
	TSequrity    &Sequrity()   { return (*sequrity); }
	
	// Run server
	int run(  );
	// Get optio from generic config file.
	void UpdateOpt();
	// Update comand line option
    	void CheckCommandLine(bool mode = false );
	// Print comand line options! 
	string opt_descr( );
    	// Get XML kernel node
    	XMLNode *XMLCfgNode();
	// Kernel name
    	string &Name() { return( m_name ); }
        //================== Message functions ========================
	void m_put( const string &categ, int level, char *fmt,  ... );
	void m_put_s( const string &categ, int level, const string &mess );

    /**Attributes: */
    public:
	string DefBDType;
	string DefBDName;    
	
    /** Private attributes: */
    private:
        //================== Controll functions ========================
	void ctr_fill_info( XMLNode *inf );
	void ctr_din_get_( const string &a_path, XMLNode *opt );
	void ctr_din_set_( const string &a_path, XMLNode *opt );
	void ctr_cmd_go_( const string &a_path, XMLNode *fld, XMLNode *rez );
	TContr &ctr_at( const string &br );
    /** Private methods: */
    private:    
	string       m_name;
	
	TUIS         *ui;
	TArhiveS     *arhive;
	TBDS         *bd;
	TControllerS *controller;
	TProtocolS   *protocol;
	TTransportS  *transport;
	TSpecialS    *special;
	TParamS      *param;
	TModSchedul  *modschedul;
	TSequrity    *sequrity;

	bool         s_run;

	static const char *o_name;
	static const char *i_cntr;
};

#endif // TKERNEL_H
