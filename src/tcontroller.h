
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
	//Public Data
	enum Redundant
	{
	    Off		= 0,
	    Asymmetric	= 1,
	    Symmetric	= 2
	};

	//Public methods
	TController( const string &name_c, const string &daq_db, TElem *cfgelem );
	virtual ~TController(  );

	TCntrNode &operator=( TCntrNode &node );

	const string &id( )	{ return mId; }
	string workId( );
	string name( );
	string descr( )		{ return mDescr; }
	virtual string getStatus( );

	string DB( )		{ return mDB; }
	string tbl( );
	string fullDB( )	{ return DB()+'.'+tbl(); }

	void setName( const string &nm )	{ mName = nm; }
	void setDescr( const string &dscr )	{ mDescr = dscr; }
	void setDB( const string &idb )		{ mDB = idb; modifG(); }

	bool toEnable( )	{ return mAEn; }
	bool toStart( )		{ return mAStart; }
	bool enableStat( )	{ return en_st; }
	bool startStat( )	{ return run_st; }

	void start( );
	void stop( );
	void enable( );
	void disable( );

	//> Parameters
	void list( vector<string> &list )	{ chldList(mPrm,list); }
	bool present( const string &name )	{ return chldPresent(mPrm,name); }
	void add( const string &name, unsigned type );
	void del( const string &name, bool full = false )	{ chldDel(mPrm,name,-1,full); }
	AutoHD<TParamContr> at( const string &name, const string &who = "th_contr" )	{ return chldAt(mPrm,name); }

	//> Redundance
	bool redntUse( )			{ return mRedntUse; }
	void setRedntUse( bool vl );
	Redundant redntMode( );
	void setRedntMode( Redundant vl );
	string redntRun( );
	void setRedntRun( const string &vl );
	virtual void redntDataUpdate( bool firstArchiveSync = false );

	TTipDAQ &owner( );

    protected:
	//Protected attributes
	bool	en_st;
	bool	run_st;

	//Methods
	//> User methods
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
	string nodeName( )       { return mId; }

	void LoadParmCfg(  );

	//Private attributes
	string	&mId;
	string	&mName;
	string	&mDescr;
	bool	&mAEn;
	bool	&mAStart;

	string	mDB;

	unsigned mPrm		: 2;
	unsigned mRedntUse	: 1;
	unsigned mRedntFirst	: 1;
};


#endif // TCONTROLLER_H
