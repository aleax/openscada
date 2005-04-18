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

#include "tbds.h"
#include "tconfig.h"
#include "tgrpmodule.h"
#include "ttipcontroller.h"

using std::string;

class TController;
class TTipController;

class TControllerS : public TGRPModule, public TElem
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
    
	TControllerS( TKernel *app );
	~TControllerS(  );

	int gmdVer( ) { return(VER_CNTR); }
	// Init All controller's modules    
	void gmdInit( );
	// Init and start all configured controllers. 
	virtual void gmdStart(  );                                         
	// Stop and deinit all configured controllers. 
	virtual void gmdStop( );

	// Load/Reload all BD and update internal controllers structure!
	void loadBD( );
	// Update all BD from current to external BD.
	void saveBD( );    
	
        void gmdCheckCommandLine( );
	void gmdUpdateOpt();		       
    
	TBDS::SName BD();
	
	string optDescr( );
	
    /** Private methods: */
    private:
	//================== Controll functions ========================
	void cntrCmd_( const string &a_path, XMLNode *opt, int cmd );
	
    /** Private atributes: */
    private:
	TBDS::SName	m_bd;
    
	static const char *o_name;
};

#endif // TCONTROLLERS_H
