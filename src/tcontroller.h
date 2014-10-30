
//OpenSCADA system file: tcontroller.h
/***************************************************************************
 *   Copyright (C) 2003-2014 by Roman Savochenko, <rom_as@oscada.org>      *
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

namespace OSCADA
{

//*************************************************
//* TController                                   *
//*************************************************
class TTipDAQ;

class TController : public TCntrNode, public TConfig
{
    friend class TParamContr;

    public:
	//Public Data
	enum Redundant {
	    Off		= 0,
	    Asymmetric	= 1,
	    Symmetric	= 2
	};

	//Public methods
	TController( const string &name_c, const string &daq_db, TElem *cfgelem );
	virtual ~TController( );

	string objName( );

	string DAQPath( );

	TCntrNode &operator=( TCntrNode &node );

	string id( )		{ return mId; }
	string workId( );
	string name( );
	string descr( );
	virtual string getStatus( );

	string DB( )		{ return mDB; }
	string tbl( );
	string fullDB( )	{ return DB()+'.'+tbl(); }

	void setName( const string &nm );
	void setDescr( const string &dscr );
	void setDB( const string &idb )		{ mDB = idb; modifG(); }

	bool toEnable( )	{ return mAEn; }
	bool toStart( )		{ return mAStart; }
	bool enableStat( )	{ return enSt; }
	bool startStat( )	{ return runSt; }
	int  messLev( )		{ return mMessLev; }

	void start( );
	void stop( );
	void enable( );
	void disable( );

	// Parameters
	void list( vector<string> &list )	{ chldList(mPrm,list); }
	bool present( const string &name )	{ return chldPresent(mPrm,name); }
	void add( const string &name, unsigned type );
	void del( const string &name, bool full = false )	{ chldDel(mPrm,name,-1,full); }
	AutoHD<TParamContr> at( const string &name, const string &who = "th_contr" )	{ return chldAt(mPrm,name); }

	// Redundancy
	bool redntUse( )			{ return mRedntUse; }
	void setRedntUse( bool vl );
	Redundant redntMode( );
	void setRedntMode( Redundant vl );
	string redntRun( );
	void setRedntRun( const string &vl );
	virtual void redntDataUpdate( );

	virtual string catsPat( );	//Individual the controller messages' categories pattern
	void alarmSet( const string &mess, int lev = -TMess::Crit, const string &prm = "" );

	TTipDAQ &owner( );

    protected:
	//Protected attributes
	bool	enSt, runSt;

	//Methods
	// User methods
	void load_( );
	void save_( );
	virtual void enable_( )		{ }
	virtual void disable_( )	{ }
	virtual void start_( )		{ }
	virtual void stop_( )		{ }

	virtual TParamContr *ParamAttach( const string &name, int type );

	void cntrCmdProc( XMLNode *opt );       //Control interface command process

	bool cfgChange( TCfg &co, const TVariant &pc )	{ modif(); return true; }

	void preDisable( int flag );		//Disable if delete
	void postDisable( int flag );		//Delete all DB if flag 1

	TVariant objFuncCall( const string &id, vector<TVariant> &prms, const string &user );

    private:
	//Private methods
	const char *nodeName( )	{ return mId.getSd(); }

	void LoadParmCfg( );

	//Private attributes
	TCfg	&mId, &mMessLev;
	char	&mAEn, &mAStart;

	string	mDB;
	ResString mRedntSt;

	unsigned mPrm		: 2;
	unsigned mRedntUse	: 1;
};

}

#endif // TCONTROLLER_H
