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

#ifndef FREEFUNCLIBS_H
#define FREEFUNCLIBS_H

#include <string>
#include <vector>

#include <tfunctions.h>
#include <tspecials.h>

using std::string;
using std::vector;

namespace FreeFunc
{

//Free libraries
class Libs : public TSpecial
{
    public:
	Libs( string src );
	~Libs();

	void modLoad( );	

	TBDS::SName 	BD();
	
	TElem &elLib()	{ return(lb_el); }
	TElem &elFnc()	{ return(fnc_el); }
	TElem &elFncIO(){ return(fncio_el); }
	
	int &parseRes( ){ return parse_res; }
	vector<string> &freeLibList() { return free_libs; }
	bool avoid( const string &lib );

    protected:
	void cntrCmd_( const string &a_path, XMLNode *opt, int cmd );
	AutoHD<TCntrNode> ctrAt1( const string &a_path );

    private:
	void modConnect(  );
	void loadBD();
        void saveBD();

    private:
	vector<string>	free_libs;
	TElem   	lb_el;
	TElem   	fnc_el;
	TElem   	fncio_el;
	TBDS::SName     m_bd;
	int     	parse_res;	//Syntax analisator
};


} //End namespace StatFunc

#endif //FREEFUNCLIBS_H

