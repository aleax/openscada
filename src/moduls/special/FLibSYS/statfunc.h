
//OpenSCADA system module Special.FLibSYS file: statfunc.h
/***************************************************************************
 *   Copyright (C) 2005-2007 by Roman Savochenko                           *
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

#ifndef STATFUNC_H
#define STATFUNC_H

#include <string>
#include <vector>

#include <tspecials.h>

using std::string;
using std::vector;

namespace FLibSYS
{

//** Complex1 functions library **
class Lib : public TSpecial
{
    public:
	//Methods
	Lib( string src );
	~Lib();
	
	void modStart( );
        void modStop( );		
	
	void list( vector<string> &ls ) 	{ chldList(m_fnc,ls); }
	bool present( const string &id )	{ return chldPresent(m_fnc,id); }
	AutoHD<TFunction> at( const string &id )	{ return chldAt(m_fnc,id); }
	void reg( TFunction *fnc )		{ chldAdd(m_fnc,fnc); }
        void unreg( const char *id )		{ chldDel(m_fnc,id); }
	
	//- Value archives and buffers access methods -
	int varchOpen( const string &inm );
	int varchBufOpen( TFld::Type vtp, int isz, int ipr, bool ihgrd, bool ihres );	
	void varchClose( int id );
	bool isArch( int id );
	AutoHD<TVArchive> varch( int id );
	TValBuf *vbuf( int id );
	void varchFree( );
	
    private:
	//Methods
	void postEnable( );
	void cntrCmdProc( XMLNode *opt );       //Control interface command process
	
	//Attributes    
	int  m_fnc;
	
	//- Value archive resources -
	int varch_res;	
	struct SVarch
	{
	    bool isArch;
	    union
	    {
		AutoHD<TVArchive> *arch;
		TValBuf	*buf;
	    };
	};
	
	vector<SVarch>	varch_lst;
	//int  aval_res, vbf_res;
	//vector< AutoHD<TVArchive> > aval_id_lst;
	//vector<TValBuf*>	vbf_id_lst;
};

extern Lib *mod;

} //End namespace FLibSYS

#endif //STATFUNC_H

