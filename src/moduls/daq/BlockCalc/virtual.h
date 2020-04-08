
//OpenSCADA module DAQ.BlockCalc file: virtual.h
/***************************************************************************
 *   Copyright (C) 2005-2018 by Roman Savochenko, <rom_as@oscada.org>      *
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

#ifndef VIRTUAL_H
#define VIRTUAL_H

#include <string>
#include <vector>

#include <ttypedaq.h>
#include <tparamcontr.h>

#include "block.h"

#undef _
#define _(mess) mod->I18N(mess)

using std::string;
using std::vector;
using namespace OSCADA;

namespace Virtual
{

//*************************************************
//* Prm: parameters for access to data            *
//*      of calculate blocks                      *
//************************************************* 
class Contr;

class Prm : public TParamContr
{
    public:
	//Public methods
	Prm( string name, TTypeParam *tp_prm );
	~Prm( );

	void enable( );
	void disable( );

	Contr &owner( ) const;

    protected:
	//Protected methods
	void cntrCmdProc( XMLNode *opt );	//Control interface command process

    private:
        //Private methods
	void postEnable( int flag );

	void vlGet( TVal &vo );
	void vlSet( TVal &vo, const TVariant &vl, const TVariant &pvl );
	void vlArchMake( TVal &vo );

	//Private attributes
	TElem  v_el;				//Values elem
};

//*************************************************
//* Contr: Blocks and parameters container        *
//*************************************************
class TpContr;

class Contr: public TController
{
    friend class Block;
    friend class Prm;

    public:
	//Public methods
	Contr( string name_c, const string &daq_db, ::TElem *cfgelem );
	~Contr( );

	TCntrNode &operator=( const TCntrNode &node );

	string getStatus( );

	TParamContr *ParamAttach( const string &name, int type );
	double period( )			{ return mPer; }
	string cron( )				{ return cfg("SCHEDULE").getS(); }
	int iterate( )				{ return mIter; }

	// Scheme's functions
	void blkList( vector<string> &ls ) const	{ chldList(mBl, ls); }
	bool blkPresent( const string &id ) const	{ return chldPresent(mBl, id); }
	string blkAdd( const string &id );
	void blkDel( const string &id )			{ chldDel(mBl, id); }
	AutoHD<Block> blkAt( const string &id ) const	{ return chldAt(mBl, id); }

	TpContr &owner( ) const;

	void redntDataUpdate( );

    protected:
	//Protected methods
	void load_( );
	void enable_( );
	void disable_( );
	void start_( );
	void stop_( );
	void cntrCmdProc( XMLNode *opt );	//Control interface command process

	// Process stat
	void blkProc( const string & id, bool val );

	void postDisable(int flag);

    private:
	//Private methods
	static void *Task( void *contr );

	//Private attributes
	bool	prcSt,		//Calc status
		callSt,		//Calc now stat
		endrunReq;	//Endrun calc request
	int64_t	&mPrior,	//Process data task priority
		&mIter;		//Iteration into clock

	int	mBl;
	vector< AutoHD<Block> > calcBlks;	// Calc blocks HD
	double	mPer;

	ResMtx	calcRes;	// Resource for process block
	ResRW	hdRes;
};

//************************************************
//* TpContr - BlockCalc type controller         *
//************************************************
class TpContr: public TTypeDAQ
{
    public:
	//Public methods
	TpContr( string name );
	~TpContr( );

	TController *ContrAttach( const string &name, const string &daq_db );

	TElem &blockE( )	{ return blkEl; }
	TElem &blockIOE( )	{ return blkioEl; }

	AutoHD<Contr> at( const string &name, const string &who = "" )	{ return TTypeDAQ::at(name,who); }
	void copy( const string &src, const string &dst );

    protected:
	//Protected methods
	void load_( );

	bool redntAllow( )	{ return true; }

    private:
	//Private methods
	void postEnable( int flag );
	void preDisable( int flag );

	//Private attributes
	TElem	blkEl;
	TElem	blkioEl;
};

extern TpContr *mod;

} //End namespace virtual

#endif //VIRTUAL_H
