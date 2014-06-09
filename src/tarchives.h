
//OpenSCADA system file: tarchives.h
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

#ifndef TARCHIVES_H
#define TARCHIVES_H

#define SARH_VER	6		//ArchiveS type modules version
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
//* Message archivator                           *
//************************************************

//************************************************
//* TMArchivator                                 *
//************************************************
class TTipArchivator;

class TMArchivator : public TCntrNode, public TConfig
{
    friend class TArchiveS;

    public:
	//Public methods
	TMArchivator( const string &id, const string &db, TElem *cf_el );

	TCntrNode &operator=( TCntrNode &node );

	string	id( )		{ return mId; }
	string	workId( );
	string	name( );
	string	dscr( )		{ return cfg("DESCR").getS(); }
	bool	toStart( )	{ return m_start; }
	bool	startStat( )	{ return run_st; }
	string	addr( )		{ return cfg("ADDR").getS(); }
	int	level( )	{ return mLevel; }
	void	categ( vector<string> &list );

	string	DB( )		{ return m_db; }
	string	tbl( );
	string	fullDB( )	{ return DB()+'.'+tbl(); }

	void setName( const string &vl )	{ cfg("NAME").setS(vl); }
	void setDscr( const string &vl )	{ cfg("DESCR").setS(vl); }
	void setToStart( bool vl )		{ m_start = vl; modif(); }
	void setAddr( const string &vl )	{ cfg("ADDR").setS(vl); }
	void setLevel( int lev )		{ mLevel = lev; }

	void setDB( const string &idb )		{ m_db = idb; modifG(); }

	virtual void start( );
	virtual void stop( );

	virtual time_t begin( )	{ return 0; }
	virtual time_t end( )	{ return 0; }
	virtual bool put( vector<TMess::SRec> &mess )	{ return false; };
	virtual void get( time_t b_tm, time_t e_tm, vector<TMess::SRec> &mess, const string &category = "", char level = 0, time_t upTo = 0 )	{ };

	TTipArchivator &owner( );

    protected:
	//Protected methods
	void cntrCmdProc( XMLNode *opt );	//Control interface command process
	void postEnable( int flag );
	void preDisable( int flag );
	void postDisable( int flag );		//Delete all DB if flag 1
	bool cfgChange( TCfg &cfg )	{ modif(); return true; }

	void load_( );
	void save_( );

	TVariant objFuncCall( const string &id, vector<TVariant> &prms, const string &user );

	//> Check messages criteries
	bool chkMessOK( const string &icateg, int8_t ilvl );

	//Protected atributes
	bool	run_st;
	int	messHead;			//Last read and archived head of messages buffer

    private:
	//Private methods
	const char *nodeName( )	{ return mId.getSd(); }

	//Private attributes
	TCfg	&mId,		//Mess arch id
		&mLevel;	//Mess arch level
	char	&m_start;	//Mess arch starting flag
	string	m_db;
};

//************************************************
//* Archive subsystem                            *
//************************************************

//************************************************
//* TTipArchivator                               *
//************************************************
class TArchiveS;
class TVArchivator;

class TTipArchivator: public TModule
{
    public:
	//Public methods
	TTipArchivator( const string &id );
	virtual ~TTipArchivator( );

	//> Messages
	void messList( vector<string> &list )	{ chldList(mMess,list); }
	bool messPresent( const string &iid )	{ return chldPresent(mMess,iid); }
	void messAdd( const string &iid, const string &idb = "*.*" );
	void messDel( const string &iid, bool full = false )	{ chldDel(mMess,iid,-1,full); }
	AutoHD<TMArchivator> messAt( const string &iid )	{ return chldAt(mMess,iid); }

	//> Values
	void valList( vector<string> &list )	{ chldList(mVal,list); }
	bool valPresent( const string &iid )	{ return chldPresent(mVal,iid); }
	void valAdd( const string &iid, const string &idb = "*.*" );
	void valDel( const string &iid, bool full = false )	{ chldDel(mVal,iid,-1,full); }
	AutoHD<TVArchivator> valAt( const string &iid )		{ return chldAt(mVal,iid); }

	TArchiveS &owner( );

    protected:
	//Protected methods
	void cntrCmdProc( XMLNode *opt );	//Control interface command process

	virtual TMArchivator *AMess( const string &id, const string &db )
	{ throw TError(nodePath().c_str(),_("Message archive no support!")); }
	virtual TVArchivator *AVal( const string &id, const string &db )
	{ throw TError(nodePath().c_str(),_("Value archive no support!")); }

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
	int valPeriod( );
	int valPrior( )		{ return mValPrior; }

	void setMessPeriod( int ivl )	{ mMessPer = ivl; modif(); }
	void setValPeriod( int ivl )	{ mValPer = ivl; modif(); }
	void setValPrior( int ivl );
	void setToUpdate( )		{ toUpdate = true; }

	void subStart( );
	void subStop( );

	void perSYSCall( unsigned int cnt );

	//> Value archives functions
	void valList( vector<string> &list )			{ chldList(mAval,list); }
	bool valPresent( const string &iid )			{ return chldPresent(mAval,iid); }
	void valAdd( const string &iid, const string &idb = "*.*" );
	void valDel( const string &iid, bool db = false )	{ chldDel(mAval,iid,-1,db); }
	AutoHD<TVArchive> valAt( const string &iid )		{ return chldAt(mAval,iid); }

	void setActMess( TMArchivator *a, bool val );
	void setActVal( TVArchive *a, bool val );

	//> Archivators
	AutoHD<TTipArchivator> at( const string &name )		{ return modAt(name); }

	//> Message archive function
	void messPut( time_t tm, int utm, const string &categ, int8_t level, const string &mess );
	void messPut( const vector<TMess::SRec> &recs );
	void messGet( time_t b_tm, time_t e_tm, vector<TMess::SRec> & recs, const string &category = "",
	    int8_t level = TMess::Debug, const string &arch = "", time_t upTo = 0 );
	time_t messBeg( const string &arch = "" );
	time_t messEnd( const string &arch = "" );

	TElem &messE( )		{ return elMess; }
	TElem &valE( ) 		{ return elVal; }
	TElem &aValE( )		{ return elAval; }

	//Public attributes
	bool	SubStarting;

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
	TElem	elMess,			//Message archivator's DB elements
		elVal,			//Value archivator's DB elements
		elAval;			//Value archives DB elements

	//> Messages archiving
	char	bufErr;			//Buffer error
	int	mMessPer;		//Message archiving period
	bool	prcStMess;		//Process messages flag
	//> Messages buffer
	pthread_mutex_t	mRes;		//Mess access resource
	unsigned headBuf;		//Head of messages buffer
	vector<TMess::SRec> mBuf;	//Messages buffer
	map<string,TMess::SRec> mAlarms;//Alarms buffer

	//> Value archiving
	pthread_mutex_t	vRes;		//Value access resource
	int	mValPer;		//Value archiving period
	int	mValPrior;		//Value archive task priority
	bool	prcStVal;		//Process value flag
	bool	endrunReqVal;		//Endrun value request
	bool	toUpdate;
	int	mAval;

	vector<AutoHD<TMArchivator> >	actMess;
	vector<AutoHD<TVArchive> >	actVal;
};

}

#endif // TARCHIVES_H
