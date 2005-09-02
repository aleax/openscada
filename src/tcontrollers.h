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

#ifndef TCONTROLLERS_H
#define TCONTROLLERS_H

#define  VER_CNTR 1    //ControllerS type modules version

#include <string>

#include "tconfig.h"
#include "tsubsys.h"
#include "ttipcontroller.h"

using std::string;

class TController;
class TTipController;

class TControllerS : public TSubSYS, public TElem
{
    /** Public methods: */
    public:
	class SName
	{
	    public:
        	SName( const char *m_tp, const char *m_obj ) : tp(m_tp), obj(m_obj) { }
        	string tp;
		string obj;
	};    
    
	TControllerS( TSYS *app );
	~TControllerS(  );
	
	void preDisable(int flag);

	int subVer( ) { return(VER_CNTR); }
	void subLoad( );
	void subSave( );
	void subStart(  );
	void subStop( );
	
	TBDS::SName BD();
	
	string optDescr( );
	
    /** Private methods: */
    private:
	//================== Controll functions ========================
	void cntrCmd_( const string &a_path, XMLNode *opt, TCntrNode::Command cmd );
	
    /** Private atributes: */
    private:
	TBDS::SName	m_bd;
};

#endif // TCONTROLLERS_H
