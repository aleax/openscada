
//OpenSCADA system file: tdaqs.h
/***************************************************************************
 *   Copyright (C) 2003-2007 by Roman Savochenko                           *
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

#ifndef TDAQS_H
#define TDAQS_H

#define  VER_CNTR 1    //ControllerS type modules version

#include <string>

#include "tprmtmpl.h"
#include "tsubsys.h"
#include "ttipdaq.h"

using std::string;

//*************************************************
//* TDAQS                                         *
//*************************************************
class TController;
class TTipDAQ;

class TDAQS : public TSubSYS
{
    public:
	//Public methods
	TDAQS( );
	~TDAQS( );
	
	int subVer( )	{ return VER_CNTR; }
	void subLoad( );
	void subSave( );
	void subStart(  );
	void subStop( );
	
	AutoHD<TTipDAQ> at( const string &name )	{ return modAt(name); }
	
        //- Parameter's templates library -
	string tmplLibTable( )				{ return "ParamTemplLibs"; }
        void tmplLibList( vector<string> &list )    	{ chldList(m_tmplib,list); }
	bool tmplLibPresent( const string &id )   	{ return chldPresent(m_tmplib,id); }
	void tmplLibReg( TPrmTmplLib *lib )		{ chldAdd(m_tmplib,lib); }
        void tmplLibUnreg( const string &id, int flg = 0 )	{ chldDel(m_tmplib,id,-1,flg); }
        AutoHD<TPrmTmplLib> tmplLibAt( const string &id )	{ return chldAt(m_tmplib,id); }								
	
	TElem &elLib( )	{ return lb_el; }
	TElem &tplE( ) 	{ return el_tmpl; }
        TElem &tplIOE( ){ return el_tmpl_io; }
	
	TElem &errE( )	{ return el_err; }	//Error atributes structure
	
    private:
	//Private methods
	string optDescr( );
    
	void cntrCmdProc( XMLNode *opt );       //Control interface command process
	
	//Private attributes
	TElem   el_err, lb_el, el_tmpl, el_tmpl_io;
	int	m_tmplib;
};

#endif // TDAQS_H
