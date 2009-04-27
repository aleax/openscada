
//OpenSCADA system module DAQ.BlockCalc file: block.h
/***************************************************************************
 *   Copyright (C) 2005-2008 by Roman Savochenko                           *
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

#ifndef BLOCK_H
#define BLOCK_H

#include <string>
#include <vector>

#include <tfunction.h>

using std::string;
using std::vector;

namespace Virtual
{

//*************************************************
//* Block: Function block                         *
//*************************************************
class Contr;

class Block : public TCntrNode, public TValFunc, public TConfig
{
    public:
	//Data
	//- Link types: -
	// DIS   - Disable IO
	// FREE  - Free link or constant
	// I_LOC - Input interblocks local link
	// I_GLB - Input interblocks global link
	// I_PRM - input parameter link
	// O_LOC - Input interblocks local link
	// O_GLB - Input interblocks global link
	// O_PRM - output parameter link
	enum LnkT { FREE, I_LOC, I_GLB, I_PRM, O_PRM, O_LOC, O_GLB };
	//- Link comands -
	enum LnkCmd { INIT, DEINIT, SET };

	//Methods
	Block( const string &iid, Contr *iown );
	~Block( );

	TCntrNode &operator=( TCntrNode &node );

	//> Block's parameters
	const string &id( )	{ return m_id; }
	string name( );
	string descr( ) 	{ return m_descr; }
	int    errCnt( )	{ return err_cnt; }
	bool   enable( )	{ return m_enable; }
	bool   process( )	{ return m_process; }
	bool   toEnable( )	{ return m_to_en; }
	bool   toProcess( )	{ return m_to_prc; }
	string wFunc( )		{ return m_func; }

	void setName( const string &name )	{ m_name = name; modif(); }
	void setDescr( const string &dscr )	{ m_descr = dscr; modif(); }
	void setEnable( bool val );
	void setProcess( bool val );
	void setToEnable( bool val )		{ m_to_en = val; modif(); }
	void setToProcess( bool val )		{ m_to_prc = val; modif(); }
	void setWFunc( const string &vl )	{ m_func = vl; modif(); }

	//> Link IO
	LnkT link( unsigned id );
	bool linkActive( unsigned id );	
	void setLink( unsigned id, LnkCmd cmd, LnkT lnk = FREE, const string &vlnk = "" );

	//> Calc block
	void calc( bool first, bool last );

	Contr &owner( );

    protected:
	//Attributes
	void load_( );
	void save_( );

	void loadIO( const string &blk_db = "", const string &blk_id = "" );
	void saveIO( );

	string nodeName( )	{ return m_id; }
	void cntrCmdProc( XMLNode *opt );	//Control interface command process

	void preDisable( int flag );
	void postDisable( int flag );		//Delete all DB if flag 1

    private:
	//Data
	//- Define input interblock link structure -
	struct SLIBlk
	{
	    AutoHD<Block>	w_bl;	//Block AutoHD
	    unsigned		w_id;	//IO index
	};

	//- Define link structures -
	struct SLnk
	{
	    LnkT tp;			//Link type
	    string lnk;			//Link
	    union
	    {
		SLIBlk		*iblk;	//Input interblock link structure
		AutoHD<TVal>	*aprm;	//Parameter atribute link structure
	    };
	};

	//Attributes
	vector<SLnk>	m_lnk;
	bool		m_enable,
			m_process;	//Processing block

	string 		&m_id,		//Block id
			&m_name,	//Block name
			&m_descr,	//Block description
			&m_func;	//Associated function
	bool		&m_to_en, &m_to_prc;

	Res		lnk_res;		//Link resource
	int		err_cnt;
	int		id_freq, id_start, id_stop;	//Fixed system attributes identifiers
};

} //End namespace virtual

#endif //BLOCK_H
