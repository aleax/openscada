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

#ifndef TPARAMCONTR_H
#define TPARAMCONTR_H

#include <time.h>
#include <string>
using std::string;

#include "tcntrnode.h"
#include "tvalue.h"

//==============================================================================================
//Use for register into TControllers and make direct access to controller function for parameter
//and direct acces to parameter from controller
//==============================================================================================
class TController;
class TTipParam;

class TParamContr : public TConfig, public TValue
{
    /** Public methods: */
    public:
	TParamContr( const string &name, TTipParam *tpprm ); 
	virtual ~TParamContr();
	
    	const string &id()	{ return m_id; }
    	const string &name()  	{ return m_name; }
	const string &descr()	{ return m_descr; }
	
	void name( const string &inm ) 	{ m_name = inm; }
	void descr( const string &idsc ){ m_descr = idsc; }
	
	TTipParam &type()	{ return *tipparm; }

	bool toEnable()		{ return m_aen; }
	bool enableStat()	{ return m_en; }
	
    	virtual void enable();	// Enable parameter and open access to value	
    	virtual void disable();	// Disable parameter and close access to value
	void load( );
	void save( );	
	
    	bool operator==( TParamContr & PrmCntr )
	{ if( id() == PrmCntr.id() ) return true; return false; };
	
    	TParamContr &operator=( TParamContr & PrmCntr );

	TController &owner() 	{ return *(TController *)nodePrev(); }
	
    protected:    
	//================== Controll functions ========================
	void cntrCmd_( const string &a_path, XMLNode *opt, TCntrNode::Command cmd );
	
	void postEnable();
	void preDisable(int flag);
        void postDisable(int flag);
	
	void vlGet( TVal &val );
	
    private:
	string nodeName()       { return m_id; }	
	
    /**Attributes: */
    private:
        string	&m_id, &m_name,	&m_descr;
	bool	&m_aen, m_en, m_export;
	TElem	el_err;		//Error atributes
	
	TTipParam   *tipparm;
};

#endif // TPARAMCONTR_H
