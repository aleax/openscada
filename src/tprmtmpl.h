
//OpenSCADA system file: tprmtmpl.h
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

#ifndef TPRMTMPL_H
#define TPRMTMPL_H

#include <string>
#include <vector>

#include "tcntrnode.h"
#include "tconfig.h"
#include "tfunction.h"

using std::string;
using std::vector;

class TPrmTempl: public TCntrNode, public TConfig
{
    public:
	//Data
	enum AttrMode	{ NoAttr, ReadOnly, FullAccess };
	enum AccMode	{ Const, PublConst, Link };
    
	class SIOPrm
        {
	    public:
	    	SIOPrm(const string iid, AttrMode iattr, AccMode iaccs, const string ival ) :
			id(iid), attr(iattr), accs(iaccs), val(ival) { }
		    
            	string	 id;    
	    	AttrMode attr;	//Mirrore to attribut
            	AccMode	 accs;   //Access mode
	    	string	 val;
        };
    
	//Methods
	TPrmTempl( const string &id, const string &idb, TElem *cf_el );
	~TPrmTempl( );
	
	const string &id()	{ return m_id; }
        string name();
        string descr() 		{ return m_descr; }
	
	void name( const string &inm )  { m_name = inm; }
        void descr( const string &idsc ){ m_descr = idsc; }		

	void load( );
        void save( );
	
	bool enable()	{ return m_en; }	//Enable stat
	void enable( bool val );
	
	AutoHD<TFunction>	func();	//Associated function
	
	//- Atributes configs -
	void attrUp();
	void attrSave();
	int attrSize()	{ return m_io.size(); }
	int attrId( const string &id );
	SIOPrm &attr( int id );
	
	string BD();
	
	TParamS &owner()	{ return *(TParamS*)nodePrev(); }
	
    private:
	//Methods
	string nodeName()	{ return m_id; }
	void postDisable(int flag);
        void cntrCmd_( const string &a_path, XMLNode *opt, TCntrNode::Command cmd );		
	
	//Attributes
	bool	m_en;	//Enable stat
	string 	&m_id, &m_name, &m_descr, &m_func;
	string  m_bd;
	vector<SIOPrm>	m_io;
};

#endif // TPRMTMPL_H
