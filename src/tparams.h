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

#ifndef TPARAMS_H
#define TPARAMS_H

#include <string>
#include <vector>

#include "thd.h"
#include "tconfig.h"
#include "terror.h"
#include "tcontr.h"

using std::string;
using std::vector;

class SCntrS;
class TParam;
class TParamContr;

class TParamS : public TConfig, public TContr  
{
    /** Public methods: */
    public:
	TParamS( TKernel *app );
    
	~TParamS(  );

        string Name();

	// Avoid parameters list
	void list( vector<string> &list )
	{ m_hd.obj_list( list ); }
	// Add parameter
	void add( SCntrS cntr, string param );
	// Del parameter
	void del( SCntrS cntr, string param );
	/*
	 * Attach to parameter
	 * Return parameter header
	 */
	unsigned att( string name, string how = "" )
	{ return( m_hd.hd_att( name, how ) ); }
	// Detach from parameter
    	void det( unsigned hd )
	{ m_hd.hd_det( hd ); }
	// Get attached parameter
	TParam &at( unsigned hd )
	{ return( *(TParam *)m_hd.hd_at( hd ) ); }
	TParam &operator[]( unsigned hd ){ return( at(hd) ); }
    
	TKernel &Owner() { return( *owner ); }
    
    //Start scheduler of params
    public:
    /** Private methods: */
    private:
	void ctr_fill_info( XMLNode *inf );
    /**Attributes: */
    private:
	THD               m_hd;
	
	TKernel           *owner;
    
	static SFld       gen_elem[];  
	static const char *i_cntr;
	static const char *o_name;
	static const char *s_name;
};

#endif // TPARAMS_H
