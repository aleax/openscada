
//OpenSCADA system file: tcontroller.h
/***************************************************************************
 *   Copyright (C) 2003-2008 by Roman Savochenko                           *
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

#ifndef TCONTROLLER_H
#define TCONTROLLER_H

#include <time.h>
#include <string>

#include "tbds.h"
#include "tparamcontr.h"

using std::string;

//*************************************************
//* TController                                   *
//*************************************************
class TTipDAQ;

class TController : public TCntrNode, public TConfig
{
    public:
	//Public methods
	TController( const string &name_c, const string &daq_db, TElem *cfgelem );
	virtual ~TController(  );

	TCntrNode &operator=( TCntrNode &node );

	const string &id( )	{ return m_id; }
	string name( );
	string descr( )		{ return m_descr; }
	virtual string getStatus( );

	string DB( )		{ return m_db; }
	string tbl( );
	string fullDB( )	{ return DB()+'.'+tbl(); }

	void setName( const string &nm )	{ m_name = nm; }
	void setDescr( const string &dscr )	{ m_descr = dscr; }
	void setDB( const string &idb )		{ m_db = idb; modifG(); }

	bool toEnable( )	{ return m_aen; }
	bool toStart( )		{ return m_astart; }
	bool enableStat( )	{ return en_st; }
	bool startStat( )	{ return run_st; }

	void start( );
	void stop( );
	void enable( );
	void disable( );

	//> Parameters
	void list( vector<string> &list )	{ chldList(m_prm,list); }
	bool present( const string &name )	{ return chldPresent(m_prm,name); }
	void add( const string &name, unsigned type );
	void del( const string &name, bool full = false )	{ chldDel(m_prm,name,-1,full); }
	AutoHD<TParamContr> at( const string &name, const string &who = "th_contr" )
	{ return chldAt(m_prm,name); }

	TTipDAQ &owner( )	{ return *(TTipDAQ *)nodePrev(); }

    protected:
	//Protected attributes
	bool	en_st;
	bool	run_st;

	//Methods
	//- User methods -
	void load_( );
	void save_( );
	virtual void enable_( )		{ }
	virtual void disable_( )	{ }
	virtual void start_( )		{ }
	virtual void stop_( )		{ }

	virtual TParamContr *ParamAttach( const string &name, int type );

	void cntrCmdProc( XMLNode *opt );       //Control interface command process

	bool cfgChange( TCfg &cfg );

	void preDisable( int flag );		//Disable if delete
	void postDisable( int flag );		//Delete all DB if flag 1

    private:
	//Private methods
	string nodeName( )       { return m_id; }

	void LoadParmCfg(  );

	//Private attributes
	string	&m_id;
	string	&m_name;
	string	&m_descr;
	bool	&m_aen;
	bool	&m_astart;

	string	m_db;
	int	m_prm;
};


#endif // TCONTROLLER_H
