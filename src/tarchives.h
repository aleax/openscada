
//OpenSCADA system file: tarchives.h
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

#ifndef TARCHIVES_H
#define TARCHIVES_H

#define  VER_ARH 2    //ArchiveS type modules version

#include <time.h>

#include <string>
#include <vector>

#include "tbds.h"
#include "tarchval.h"

using std::string;
using std::vector;

//************************************************
//* Message archivator                           *
//************************************************

//************************************************
//* TMArchivator                                 *
//************************************************
class TTipArchivator;

class TMArchivator : public TCntrNode, public TConfig
{
    public:
	//Public methods
	TMArchivator( const string &id, const string &db, TElem *cf_el );

	TCntrNode &operator=( TCntrNode &node );

	const string &id( )	{ return m_id; }
	string workId( );
	string name( );
	string dscr( )		{ return m_dscr; }
	bool toStart( )		{ return m_start; }
	bool startStat( )	{ return run_st; }
	string addr( )		{ return m_addr; }
	int    level( )		{ return m_level; }
	void   categ( vector<string> &list );

	string DB( )		{ return m_db; }
	string tbl( );
	string fullDB( )	{ return DB()+'.'+tbl(); }

	void setName( const string &vl )	{ m_name = vl; modif(); }
	void setDscr( const string &vl )	{ m_dscr = vl; modif(); }
	void setToStart( bool vl )		{ m_start = vl; modif(); }
	void setAddr( const string &vl )	{ m_addr = vl; modif(); }
	void setLevel( int lev )		{ m_level = lev; modif(); }

	void setDB( const string &idb )		{ m_db = idb; modifG(); }

	virtual void start( )	{ };
	virtual void stop( )	{ };

	virtual time_t begin( )	{ return 0; }
	virtual time_t end( )	{ return 0; }
	virtual void put( vector<TMess::SRec> &mess ){ };
	virtual void get( time_t b_tm, time_t e_tm, vector<TMess::SRec> &mess, const string &category = "", char level = 0 ) { };

	TTipArchivator &owner( );

    protected:
	//Protected methods
	void cntrCmdProc( XMLNode *opt );	//Control interface command process
	void postEnable( int flag );
	void preDisable( int flag );
	void postDisable( int flag );		//Delete all DB if flag 1

	void load_( );
	void save_( );

	//- Check messages criteries -
	bool chkMessOK( const string &icateg, TMess::Type ilvl );

	//Protected atributes
	bool           run_st;

    private:
	//Private methods
	string nodeName( )	{ return m_id; }

	//Private attributes
	string	&m_id,		//Mess arch id
		&m_name,	//Mess arch name
		&m_dscr,	//Mess arch description
		&m_addr,	//Mess arch address
		&m_cat_o;	//Mess arch cetegory
	bool	&m_start;	//Mess arch starting flag
	int	&m_level;	//Mess arch level
	string	m_db;
};

//************************************************
//* Archive subsystem                            *
//************************************************

//************************************************
//* TTipArchivator                               *
//************************************************
class TArchiveS;

class TTipArchivator: public TModule
{
    public:
	//Public methods
	TTipArchivator( );
	virtual ~TTipArchivator( );

	//- Messages -
	void messList( vector<string> &list )	{ chldList(mMess,list); }
	bool messPresent( const string &iid )	{ return chldPresent(mMess,iid); }
	void messAdd( const string &iid, const string &idb = "*.*" );
	void messDel( const string &iid, bool full = false )	{ chldDel(mMess,iid,-1,full); }
	AutoHD<TMArchivator> messAt( const string &iid )	{ return chldAt(mMess,iid); }

	//- Values -
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
	{ throw TError(nodePath().c_str(),"Message arhiv no support!"); }
	virtual TVArchivator *AVal( const string &id, const string &db )
	{ throw TError(nodePath().c_str(),"Value arhiv no support!"); }	

    private:
	//Private attributes
	int	mMess, mVal;
};

//************************************************
//* TArchiveS                                    *
//************************************************
class TArchiveS : public TSubSYS
{
    public:
	//Public methods
	TArchiveS( );
	~TArchiveS( );

	int subVer( )		{ return VER_ARH; }

	int messPeriod( )	{ return mMessPer; }
	int valPeriod( );
	int valPrior( )		{ return mValPrior; }

	void setMessPeriod( int ivl );
	void setValPeriod( int ivl )	{ mValPer = ivl; modif(); }
	void setValPrior( int ivl )	{ mValPrior = ivl; modif(); }

	void subStart( );
	void subStop( );

	//> Value archives functions
	void valList( vector<string> &list )			{ chldList(mAval,list); }
	bool valPresent( const string &iid )			{ return chldPresent(mAval,iid); }
	void valAdd( const string &iid, const string &idb = "*.*" );
	void valDel( const string &iid, bool db = false )	{ chldDel(mAval,iid,-1,db); }
	AutoHD<TVArchive> valAt( const string &iid )		{ return chldAt(mAval,iid); }

	void setActValArch( const string &id, bool val );

	//> Archivators
	AutoHD<TTipArchivator> at( const string &name )		{ return modAt(name); }

	//> Message archive function
	void messPut( time_t tm, int utm, const string &categ, TMess::Type level, const string &mess );
	void messPut( const vector<TMess::SRec> &recs );
	void messGet( time_t b_tm, time_t e_tm, vector<TMess::SRec> & recs, const string &category = "",
		TMess::Type level = TMess::Debug, const string &arch = "" );
	time_t messBeg( const string &arch = "" );
	time_t messEnd( const string &arch = "" );

	TElem &messE( )		{ return elMess; }
	TElem &valE( ) 		{ return elVal; }
	TElem &aValE( )		{ return elAval; }

	//Public attributes
	static int max_req_mess;

    protected:
	//Protected methods
	void load_( );
	void save_( );

    private:
	//Private methods
	string optDescr( );

	static void ArhMessTask( union sigval obj );
	static void *ArhValTask( void *param );

	void cntrCmdProc( XMLNode *opt );       //Control interface command process

	int messBufLen( )	{ return mBuf.size(); }
	void setMessBufLen( int len );

	//Private attributes
	TElem		elMess,		//Message archivator's DB elements
			elVal,		//Value archivator's DB elements
			elAval;		//Value archives DB elements

	//> Messages archiving
	char	bufErr;			//Buffer error
	int	mMessPer;		//Message arhiving period
	timer_t	tmIdMess;		//Messages timer
	bool	prcStMess;		//Process messages flag
	//>> Messages buffer
	Res	mRes;			//Mess access resource
	unsigned headBuf,		//Head of messages buffer
		headLstread;		//Last read and archived head of messages buffer
	vector<TMess::SRec> mBuf;	//Messages buffer

	//> Value archiving
	Res	vRes;			//Value access resource
	int	mValPer;		//Value arhiving period
	int	mValPrior;		//Value archive task priority
	pthread_t mValPthr;		//Value get realtime pthread
	bool	prcStVal;		//Process value flag
	bool	endrunReqVal;		//Endrun value request
	int	mAval;

	vector< AutoHD<TVArchive> > actUpSrc;
};

#endif // TARCHIVES_H
