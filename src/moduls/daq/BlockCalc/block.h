
//OpenSCADA system module DAQ.BlockCalc file: block.h
/***************************************************************************
 *   Copyright (C) 2005-2016 by Roman Savochenko, <rom_as@oscada.org>      *
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
using namespace OSCADA;

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
	// Link types:
	//  DIS   - Disable IO
	//  FREE  - Free link or constant
	//  I_LOC - Input interblocks local link
	//  I_GLB - Input interblocks global link
	//  I_PRM - input parameter link
	//  O_LOC - Input interblocks local link
	//  O_GLB - Input interblocks global link
	//  O_PRM - output parameter link
	enum LnkT { FREE, I_LOC, I_GLB, I_PRM, O_PRM, O_LOC, O_GLB };
	//> Link comands
	enum LnkCmd { INIT, DEINIT, SET };

	//Methods
	Block( const string &iid, Contr *iown );
	~Block( );

	TCntrNode &operator=( TCntrNode &node );

	// Block's parameters
	string id( )		{ return mId; }
	string name( );
	string descr( ) 	{ return cfg("DESCR").getS(); }
	int    errCnt( )	{ return mErrCnt; }
	bool   enable( )	{ return mEnable; }
	bool   process( )	{ return mProcess; }
	bool   toEnable( )	{ return mToEn; }
	bool   toProcess( )	{ return mToPrc; }
	string prior( )		{ return cfg("PRIOR").getS(); }
	string wFunc( )		{ return cfg("FUNC").getS(); }
	bool outLnkWrChs( )	{ return mOutLnkWrChs; }

	void setName( const string &name )	{ cfg("NAME").setS(name); }
	void setDescr( const string &dscr )	{ cfg("DESCR").setS(dscr); }
	void setEnable( bool val );
	void setProcess( bool val );
	void setToEnable( bool val )		{ mToEn = val; modif(); }
	void setToProcess( bool val )		{ mToPrc = val; modif(); }
	void setPrior( const string &vl )	{ cfg("PRIOR").setS(vl); }
	void setWFunc( const string &vl )	{ cfg("FUNC").setS(vl); }
	void setOutLnkWrChs( bool val )		{ mOutLnkWrChs = val; modif(); }

	// Link IO
	LnkT link( unsigned id );
	bool linkActive( unsigned id );	
	void setLink( unsigned id, LnkCmd cmd, LnkT lnk = FREE, const string &vlnk = "" );

	// Calc block
	void calc( bool first, bool last, double frq );

	Contr &owner( );

    protected:
	//Attributes
	void load_( );
	void save_( );

	void loadIO( const string &blk_db = "", const string &blk_id = "", bool force = false );
	void saveIO( );

	const char *nodeName( )	{ return mId.getSd(); }
	void cntrCmdProc( XMLNode *opt );	//Control interface command process

	void preDisable( int flag );
	void postDisable( int flag );		//Delete all DB if flag 1
	bool cfgChange( TCfg &co, const TVariant &pc )	{ modif(); return true; }

	TVariant objFuncCall( const string &id, vector<TVariant> &prms, const string &user );

    private:
	//Data
	// Define input interblock link structure
	class SLIBlk {
	    public:
		SLIBlk( ) : wId(-1) { }

		AutoHD<Block>	wBl;	//Block AutoHD
		int		wId;	//IO index
	};

	//> Define link structures
	struct SLnk {
	    LnkT tp;			//Link type
	    string lnk;			//Link
	    union {
		SLIBlk		*iblk;	//Input interblock link structure
		AutoHD<TVal>	*aprm;	//Parameter atribute link structure
	    };
	};

	//Attributes
	vector<SLnk>	mLnk;
	bool		mEnable,
			mProcess;	//Processing block

	TCfg		&mId;		//Block id
	char		&mToEn, &mToPrc,
			&mOutLnkWrChs;	//Write to output links only at changes

	Res		lnkRes;		//Link resource
	int		mErrCnt;
	int		idFreq, idStart, idStop;	//Fixed system attributes identifiers
};

} //End namespace virtual

#endif //BLOCK_H
