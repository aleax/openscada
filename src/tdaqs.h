
//OpenSCADA system file: tdaqs.h
/***************************************************************************
 *   Copyright (C) 2003-2006 by Roman Savochenko                           *
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

#ifndef TDAQS_H
#define TDAQS_H

#define  VER_CNTR 1    //ControllerS type modules version

#include <string>

#include "tconfig.h"
#include "tsubsys.h"
#include "ttipdaq.h"

using std::string;

class TController;
class TTipDAQ;

class TDAQS : public TSubSYS
{
    public:
	TDAQS( );
	~TDAQS( );
	
	int subVer( )	{ return(VER_CNTR); }
	void subLoad( );
	void subSave( );
	void subStart(  );
	void subStop( );
	
	AutoHD<TTipDAQ> at( const string &name )
        { return modAt(name); }
	
	TElem &errE()	{ return el_err; }	//Error atributes structure
	
    private:
	//Methods
	string optDescr( );
	void preDisable(int flag);
    
	void cntrCmd_( const string &a_path, XMLNode *opt, TCntrNode::Command cmd );
	
	//Attributes
	TElem   	el_err;
};

#endif // TDAQS_H
