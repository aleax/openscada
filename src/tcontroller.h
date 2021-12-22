
//OpenSCADA file: tcontroller.h
/***************************************************************************
 *   Copyright (C) 2003-2021 by Roman Savochenko, <roman@oscada.org>       *
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
class TTypeDAQ;

class TController : public TCntrNode, public TConfig
{
    friend class TParamContr;

    public:
	//Public Data
	enum Redundant {
	    Off		= 0,
	    Asymmetric	= 1,
	    OnlyAlarms	= 2,
	    // Command specific
	    Any		= 0
	};
	class RedntStkEl {
	    public:
	    RedntStkEl( const string &iaddr = "" ) : pos(0), addr(iaddr)	{ }

	    unsigned pos;
	    vector<string> ls;
	    string addr;
	};

	//Public methods
	TController( const string &name_c, const string &daq_db, TElem *cfgelem );
	virtual ~TController( );

	string objName( );

	string DAQPath( );

	TCntrNode &operator=( const TCntrNode &node );

	string id( )		{ return mId; }
	string workId( );
	string name( );
	string descr( );
	int64_t timeStamp( );
	virtual string getStatus( );

	string DB( bool qTop = false ) const	{ return storage(mDB, qTop); }
	string tbl( ) const;
	string fullDB( bool qTop = false ) const{ return DB(qTop)+'.'+tbl(); }

	void setName( const string &nm );
	void setDescr( const string &dscr );
	void setDB( const string &vl, bool qTop = false )	{ setStorage(mDB, vl, qTop); if(!qTop) modifG(); }

	bool toEnable( )		{ return mAEn; }
	bool toStart( )			{ return mAStart; }
	bool enableStat( ) const	{ return enSt; }
	bool startStat( ) const		{ return runSt; }
	int  messLev( )			{ return mMessLev; }

	void start( );
	void stop( );
	void enable( );
	void disable( );

	// Parameters
	void list( vector<string> &list ) const	{ chldList(mPrm, list); }
	bool present( const string &id ) const	{ return chldPresent(mPrm, id); }
	string add( const string &id, unsigned type );
	void del( const string &id, int flags = NodeNoFlg )	{ chldDel(mPrm, id, -1, flags); }
	AutoHD<TParamContr> at( const string &id, const string &who = "th_contr" ) const	{ return chldAt(mPrm, id); }

	// Redundancy
	//  In redundancy now
	bool redntUse( Redundant md = Asymmetric )	{ return mRdUse && (md == Any || redntMode() == md); }
	void setRedntUse( bool vl )		{ mRdUse = vl; }
	//  Mode of the archiver's redundancy, only enabling now
	Redundant redntMode( )	{ return (TController::Redundant)cfg("REDNT").getI(); }
	void setRedntMode( Redundant vl )	{ cfg("REDNT").setI(vl); }
	//  Redundancy condition: <high>, <low>, <optimal>, {ForceStation}
	string redntRun( )	{ return cfg("REDNT_RUN").getS(); }
	void setRedntRun( const string &vl )	{ cfg("REDNT_RUN").setS(vl); }
	virtual void redntDataUpdate( );

	virtual string catsPat( );	//Individual categories pattern for messages of the controller
	virtual void messSet( const string &mess, int lev, const string &type2Code = "OP", const string &prm = "", const string &cat = "" );
	void alarmSet( const string &mess, int lev = -TMess::Crit, const string &prm = "", bool force = false );

	TTypeDAQ &owner( ) const;

    protected:
	//Protected attributes
	bool	enSt, runSt;

	//Methods
	// User methods
	void load_( TConfig *cfg );
	void save_( );
	virtual void enable_( )		{ }
	virtual void disable_( )	{ }
	virtual void start_( )		{ }
	virtual void stop_( )		{ }

	virtual TParamContr *ParamAttach( const string &name, int type );

	void cntrCmdProc( XMLNode *opt );	//Control interface command process

	bool cfgChange( TCfg &co, const TVariant &pc );

	void preDisable( int flag );		//Disable if delete
	void postDisable( int flag );		//Delete all DB if flag 1

	TVariant objFuncCall( const string &id, vector<TVariant> &prms, const string &user_lang );

    private:
	//Private methods
	const char *nodeName( ) const		{ return mId.getSd(); }
	const char *nodeNameSYSM( ) const	{ return mId.getSd(); }

	void LoadParmCfg( );

	//Private attributes
	TCfg	&mId, &mMessLev;
	char	&mAEn, &mAStart;

	string	mDB;
	MtxString mRdSt;

	unsigned mPrm	 : 2;
	unsigned mRdUse	 : 1;
	unsigned mRdFirst: 1;
};

}

#endif // TCONTROLLER_H
