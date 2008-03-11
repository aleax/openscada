
//OpenSCADA system module DAQ.BlockCalc file: virtual.h
/***************************************************************************
 *   Copyright (C) 2005-2007 by Roman Savochenko                           *
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

#ifndef VIRTUAL_H
#define VIRTUAL_H

#include <string>
#include <vector>

#include <ttipdaq.h>
#include <tparamcontr.h>

#include "block.h"

#undef _
#define _(mess) mod->I18N(mess)

using std::string;
using std::vector;

namespace Virtual
{

//*************************************************
//* Prm: parameters for access to data            *
//*      of calced blocks                         *
//************************************************* 
class Contr;
    
class Prm : public TParamContr
{
    public:
	//Public methods
     	Prm( string name, TTipParam *tp_prm );
	~Prm( );
    
	void enable( );
	void disable( );
	
	Contr &owner( )		{ return (Contr&)TParamContr::owner( ); }
	
    private:
        //Private methods
	void postEnable( int flag );
	
	void vlSet( TVal &val );
	void vlGet( TVal &val );
	void vlArchMake( TVal &val );
    
	//Private attributes
	TElem  v_el;				//Values elem
}; 

//*************************************************
//* Contr: Blocks and parameters container        *
//*************************************************
class TipContr;

class Contr: public TController
{
    friend class Block;    

    public:
    	//Public methods
	Contr( string name_c, const string &daq_db, ::TElem *cfgelem );
	~Contr( );

	void load( );
	void save( );
	void enable_( );
	void disable_( );	
	void start_( );
	void stop_( );
    
	TParamContr *ParamAttach( const string &name, int type );
	int period( )  				{ return m_per; }
	int iterate( ) 				{ return m_iter; }
	
	//- Scheme's functions -
        void blkList( vector<string> &ls )	{ chldList(m_bl,ls); }
        bool blkPresent( const string &id )    	{ return chldPresent(m_bl,id); }
        void blkAdd( const string &id );
        void blkDel( const string &id )    	{ chldDel(m_bl,id); }
        AutoHD<Block> blkAt( const string &id )	{ return chldAt(m_bl,id); }
	
	Res &res( )				{ return hd_res; }
    
	TipContr &owner( )			{ return (TipContr&)TController::owner( ); }
    
    protected:
	//Protected methods
	bool cfgChange( TCfg &cfg );
	void cntrCmdProc( XMLNode *opt );       //Control interface command process
	
	void loadV( );
	void saveV( );
	void freeV( );
	
        //- Process stat -
        void blkProc( const string & id, bool val );
	
	void postDisable(int flag);
    
    private:
	//Private methods
	static void *Task( void *contr );
	static void TaskDBSync(union sigval obj);
	
	//Private attributes
	bool	prc_st,      	// Calc status
		endrun_req,	// Endrun calc request
		sync_st;	// Sync DB status
	int	&m_per,  	// Clock period (ms)
		&m_prior,	// Process data task priority
		&m_iter,    	// Iteration into clock
		&m_dbper;	// DB period sync (s)
	
	pthread_t calcPthr;	// Calc pthread
	timer_t	sncDBTm;	// Sync DB timer
	
	int	m_bl;
	vector< AutoHD<Block> >	clc_blks;	// Calc blocks HD
	double	tm_calc;			// Scheme's calc time
	
	Res	hd_res;		// Resource for process block
};

//************************************************
//* TipContr - BlockCalc type controller         *
//************************************************
class TipContr: public TTipDAQ
{
    public:
	//Public methods
	TipContr( string name );
	~TipContr( );
	
	void modLoad( );
	
	TController *ContrAttach( const string &name, const string &daq_db );
	
	TElem &blockE( )	{ return blk_el; }
	TElem &blockIOE( )	{ return blkio_el; }

	AutoHD<Contr> at( const string &name, const string &who = "" )
	{ return TTipDAQ::at(name,who); }
 	void copy( const string &src, const string &dst );
	
    protected:
	//Protected methods
	void cntrCmdProc( XMLNode *opt );       //Control interface command process
    
    private:
	//Private methods
	void postEnable( int flag );
	void preDisable( int flag );
        string optDescr( );
	void loadBD( );
	void saveBD( );
	
        //Private attributes
	TElem	blk_el;
	TElem   blkio_el;	
};

extern TipContr *mod;

} //End namespace virtual

#endif //VIRTUAL_H

