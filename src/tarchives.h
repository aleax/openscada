
//OpenSCADA file: tarchives.h
/***************************************************************************
 *   Copyright (C) 2003-2018 by Roman Savochenko, <rom_as@oscada.org>      *
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

#ifndef TARCHIVES_H
#define TARCHIVES_H

#define SARH_VER	20		//ArchiveS type modules version
#define SARH_ID		"Archive"

#include <string>
#include <vector>

#include "tbds.h"
#include "tarchval.h"

using std::string;
using std::vector;

namespace OSCADA
{

//************************************************
//* Message archiver                             *
//************************************************

//************************************************
//* TMArchivator                                 *
//************************************************
class TTypeArchivator;

class TMArchivator : public TCntrNode, public TConfig
{
    friend class TArchiveS;

    public:
	//Public methods
	TMArchivator( const string &id, const string &db, TElem *cf_el );

	TCntrNode &operator=( const TCntrNode &node );

	string	id( )			{ return mId; }
	string	workId( );
	string	name( );
	string	dscr( )			{ return cfg("DESCR").getS(); }
	bool	toStart( )		{ return mStart; }
	bool	startStat( ) const	{ return runSt; }
	string	addr( ) const		{ return cfg("ADDR").getS(); }
	int	level( )		{ return mLevel; }
	void	categ( vector<string> &list );

	string	DB( )		{ return mDB; }
	string	tbl( );
	string	fullDB( )	{ return DB()+'.'+tbl(); }

	void setName( const string &vl )	{ cfg("NAME").setS(vl); }
	void setDscr( const string &vl )	{ cfg("DESCR").setS(vl); }
	void setToStart( bool vl )		{ mStart = vl; modif(); }
	void setAddr( const string &vl )	{ cfg("ADDR").setS(vl); }
	void setLevel( int lev )		{ mLevel = lev; }

	void setDB( const string &idb )		{ mDB = idb; modifG(); }

	// Redundancy
	//  In redundancy now
	bool redntUse( )	{ return mRdUse; }
	void setRedntUse( bool vl )		{ mRdUse = vl; }
	//  Enabled the archiver's redundancy
	bool redntMode( )	{ return cfg("REDNT").getB(); }
	void setRedntMode( bool vl )		{ cfg("REDNT").setB(vl); }
	//  Redundancy condition: <high>, <low>, <optimal>, {ForceStation}
	string redntRun( )	{ return cfg("REDNT_RUN").getS(); }
	void setRedntRun( const string &vl )	{ cfg("REDNT_RUN").setS(vl); }
	virtual void redntDataUpdate( );

	virtual void start( );
	virtual void stop( );

	virtual time_t begin( )		{ return 0; }
	virtual time_t end( )		{ return 0; }
	virtual bool put( vector<TMess::SRec> &mess, bool force = false );	//<force> mostly used by redundancy to prevent cycling
	virtual time_t get( time_t bTm, time_t eTm, vector<TMess::SRec> &mess, const string &category = "", char level = 0, time_t upTo = 0 )
					{ return 0; };

	TTypeArchivator &owner( ) const;

    protected:
	//Protected methods
	void cntrCmdProc( XMLNode *opt );	//Control interface command process
	void postEnable( int flag );
	void preDisable( int flag );
	void postDisable( int flag );		//Delete all DB if flag 1
	bool cfgChange( TCfg &co, const TVariant &pc )	{ modif(); return true; }

	void load_( TConfig *cfg );
	void save_( );

	TVariant objFuncCall( const string &id, vector<TVariant> &prms, const string &user );

	// Check messages criteries
	bool chkMessOK( const string &icateg, int8_t ilvl );

	//Protected atributes
	bool	runSt;
	int	messHead;			//Last read and archived head of messages buffer

    private:
	//Private methods
	const char *nodeName( ) const	{ return mId.getSd(); }

	//Private attributes
	TCfg	&mId,		//Mess arch id
		&mLevel;	//Mess arch level
	char	&mStart;	//Mess arch starting flag
	string	mDB;

	unsigned mRdUse  : 1;
	unsigned mRdFirst: 1;
	time_t	mRdTm;
};

//************************************************
//* Archive subsystem                            *
//************************************************

//************************************************
//* TTypeArchivator                               *
//************************************************
class TArchiveS;
class TVArchivator;

class TTypeArchivator: public TModule
{
    public:
	//Public methods
	TTypeArchivator( const string &id );
	virtual ~TTypeArchivator( );

	// Messages
	void messList( vector<string> &list ) const		{ chldList(mMess, list); }
	bool messPresent( const string &id ) const		{ return chldPresent(mMess, id); }
	string messAdd( const string &id, const string &idb = "*.*" );
	void messDel( const string &id, bool full = false )	{ chldDel(mMess, id, -1, full); }
	AutoHD<TMArchivator> messAt( const string &id ) const	{ return chldAt(mMess, id); }

	// Values
	void valList( vector<string> &list ) const		{ chldList(mVal, list); }
	bool valPresent( const string &id ) const		{ return chldPresent(mVal, id); }
	string valAdd( const string &id, const string &idb = "*.*" );
	void valDel( const string &id, bool full = false )	{ chldDel(mVal, id, -1, full); }
	AutoHD<TVArchivator> valAt( const string &id ) const	{ return chldAt(mVal, id); }

	TArchiveS &owner( ) const;

    protected:
	//Protected methods
	void cntrCmdProc( XMLNode *opt );		//Control interface command process

	virtual TMArchivator *AMess( const string &id, const string &db )
	{ throw TError(nodePath().c_str(),_("Message archiver is not supported!")); }
	virtual TVArchivator *AVal( const string &id, const string &db )
	{ throw TError(nodePath().c_str(),_("Value archiver is not supported!")); }

    private:
	//Private attributes
	int	mMess, mVal;
};

//************************************************
//* TArchiveS                                    *
//************************************************
class TVArchive;

class TArchiveS : public TSubSYS
{
    public:
	//Public methods
	TArchiveS( );
	~TArchiveS( );

	int subVer( )		{ return SARH_VER; }

	int messPeriod( )	{ return mMessPer; }
	int valPeriod( )	{ return vmax(1, mValPer); }
	int valPrior( )		{ return mValPrior; }
	bool valForceCurTm( )	{ return mValForceCurTm; }

	void setMessPeriod( int ivl )	{ mMessPer = ivl; modif(); }
	void setValPeriod( int ivl )	{ mValPer = ivl; modif(); }
	void setValPrior( int ivl )	{ mValPrior = vmax(-1, vmin(199,ivl)); modif(); }
	void setValForceCurTm( bool vl ){ mValForceCurTm = vl; modif(); }
	void setToUpdate( )		{ toUpdate = true; }

	void unload( );

	void subStart( );
	void subStop( );

	void perSYSCall( unsigned int cnt );

	// Value archives functions
	void valList( vector<string> &list ) const		{ chldList(mAval, list); }
	bool valPresent( const string &id ) const		{ return chldPresent(mAval, id); }
	string valAdd( const string &id, const string &idb = "*.*" );
	void valDel( const string &id, bool db = false )	{ chldDel(mAval, id, -1, db); }
	AutoHD<TVArchive> valAt( const string &id ) const	{ return chldAt(mAval, id); }

	void setActMess( TMArchivator *a, bool val );
	void setActVal( TVArchive *a, bool val );

	// Archivers
	AutoHD<TTypeArchivator> at( const string &name ) const		{ return modAt(name); }

	// Message archive function
	void messPut( time_t tm, int utm, const string &categ, int8_t level, const string &mess, const string &arch = "", bool force = false );
	void messPut( const vector<TMess::SRec> &recs, const string &arch = "", bool force = false );
	time_t messGet( time_t bTm, time_t eTm, vector<TMess::SRec> &recs, const string &category = "",
	    int8_t level = TMess::Debug, const string &arch = "", time_t upTo = 0 );
	time_t messBeg( const string &arch = "" );
	time_t messEnd( const string &arch = "" );

	// Redundancy
	bool rdProcess( XMLNode *reqSt = NULL );
	float rdRestDtOverTm( )			{ return mRdRestDtOverTm; }	//In days
	void setRdRestDtOverTm( float vl )	{ mRdRestDtOverTm = vmin(356,vmax(0,vl)); modif(); }
	void rdActArchMList( vector<string> &ls, bool isRun = false );
	string rdStRequest( const string &arch, XMLNode &req, const string &prevSt = "", bool toRun = true );

	TElem &messE( )		{ return elMess; }
	TElem &valE( ) 		{ return elVal; }
	TElem &aValE( )		{ return elAval; }

	//Public attributes
	bool	subStarting;

    protected:
	//Protected methods
	void load_( );
	void save_( );

    private:
	//Private methods
	string optDescr( );

	static void *ArhMessTask( void *param );
	static void *ArhValTask( void *param );

	void cntrCmdProc( XMLNode *opt );       //Control interface command process
	TVariant objFuncCall( const string &id, vector<TVariant> &prms, const string &user );

	unsigned messBufLen( )	{ return mBuf.size(); }
	void setMessBufLen( unsigned len );

	//Private attributes
	TElem	elMess,			//Message archiver's DB elements
		elVal,			//Value archivator's DB elements
		elAval;			//Value archives DB elements

	// Messages archiving
	int	mMessPer;		//Message archiving period
	bool	prcStMess;		//Process messages flag
	//  Messages buffer
	ResMtx	mRes;			//Mess access resource
	unsigned headBuf;		//Head of messages buffer
	vector<TMess::SRec> mBuf;	//Messages buffer
	map<string,TMess::SRec> mAlarms;//Alarms buffer

	// Value archiving
	ResMtx	vRes;			//Value access resource
	int	mValPer,		//Value archiving period
		mValPrior,		//Value archive task priority
		mAval;

	bool	mValForceCurTm,		//Time of taken values force to current and overide it's source from
		prcStVal,		//Process value flag
		endrunReqVal,		//Endrun value request
		toUpdate;

	vector<AutoHD<TMArchivator> >	actMess;
	vector<AutoHD<TVArchive> >	actVal;

	// Redundancy
	ResRW	mRdRes;
	float	mRdRestDtOverTm;	//Overtime of the redundant history reload at start in hours
	map<string, map<string,bool> > mRdArchM;
	unsigned mRdFirst	: 1;
};

}

#endif // TARCHIVES_H
