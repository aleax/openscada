
//OpenSCADA system file: tarchval.h
/***************************************************************************
 *   Copyright (C) 2006-2010 by Roman Savochenko                           *
 *   rom_as@oscada.org, rom_as@fromru.com                                  *
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

#ifndef TArchVal_H
#define TArchVal_H

#include <string>
#include <vector>
#include <map>

#include "resalloc.h"
#include "tvariant.h"
#include "tsys.h"

using std::string;
using std::vector;
using std::map;

namespace OSCADA
{

//*************************************************
//* Value archivator                              *
//*************************************************

//*************************************************
//* TValBuf                                       *
//*   Value buffer object. Contain a massif of    *
//* values types: bool, decimal, real or string.  *
//*************************************************
class TValBuf
{
    public:
	//Public methods
	TValBuf( );
	TValBuf( TFld::Type vtp, int isz, long long ipr, bool ihgrd = false, bool ihres = false );
	virtual ~TValBuf( );

	TValBuf &operator=( TValBuf &src );

	void clear( );

	TFld::Type valType( )	{ return mValTp; }
	bool hardGrid( )	{ return mHrdGrd; }
	bool highResTm( )	{ return mHgResTm; }
	int size( )		{ return mSize; }
	int realSize( );
	unsigned int evalCnt( )	{ return mEvalCnt; }

	long long begin( )	{ return mBeg; }
	long long end( )	{ return mEnd; }
	long long period( )	{ return mPer; }

	bool vOK( long long ibeg, long long iend );

	void setValType( TFld::Type vl );
	void setHardGrid( bool vl );
	void setHighResTm( bool vl );
	void setSize( int vl );
	void setPeriod( long long vl );

	//> Get value
	virtual void getVals( TValBuf &buf, long long beg = 0, long long end = 0 );
	virtual string getS( long long *tm = NULL, bool up_ord = false );
	virtual double getR( long long *tm = NULL, bool up_ord = false );
	virtual int    getI( long long *tm = NULL, bool up_ord = false );
	virtual char   getB( long long *tm = NULL, bool up_ord = false );

	//> Set value
	virtual void setVals( TValBuf &buf, long long beg = 0, long long end = 0 );
	virtual void setS( const string &value, long long tm = 0 );
	virtual void setR( double value, long long tm = 0 );
	virtual void setI( int value, long long tm = 0 );
	virtual void setB( char value, long long tm = 0 );

	void makeBuf( TFld::Type v_tp, int isz, long long ipr, bool hd_grd, bool hg_res );	//Create new or change buffer mode (all data into buffer will lost)

    private:
	//Private data and attributes
	template <class TpVal> class TBuf
	{
	    friend class TValBuf;
	    public:
		//Public methods
		TBuf( TpVal eval, int &isz, long long &ipr, bool &ihgrd, bool &ihres,
		    long long& iend, long long& ibeg, unsigned int &iEvalCnt );
		~TBuf( );

		void clear( );

		int realSize( );

		TpVal get( long long *tm = NULL, bool up_ord = false );
		void  set( TpVal value, long long tm = 0 );

		//> Create new or change buffer mode (all data into buffer will lost)
		void makeBuf( int isz, long long ipr, bool hd_grd, bool hg_res );

	    private:
		//Private attributes
		bool	&hg_res_tm,
			&hrd_grd;
		long long &end, &beg,
			&per;
		int	&size,
			cur;		//Curent position.
		TpVal	eval;		//Error element value
		unsigned int &mEvalCnt;

		struct SLw  { time_t tm; TpVal val; };
		struct SHg  { long long tm; TpVal val; };
		union
		{
		    vector<TpVal>	*grid;
		    vector<SLw>		*tm_low;
		    vector<SHg>		*tm_high;
		}buf;
	};

	Res		bRes;		//Access resource
	TFld::Type	mValTp;		//Store values type
	union
	{
	    TBuf<char>	*bl;
	    TBuf<int>	*dec;
	    TBuf<double>*real;
	    TBuf<string>*str;
	} buf;

	bool	mHgResTm,		//High resolution time use (microseconds)
		mHrdGrd;		//Set hard griding. It mode no support the zero periodic.
	long long mEnd, mBeg,		//Time of buffer begin and end (64 bit usec)
		mPer;			//Periodic grid value (usec). If value = 0 then it no periodic buffer, error for time griding mode!
	int	mSize;			//Buffer size limit.
	unsigned int mEvalCnt;
};


//*************************************************
//* TVArchive                                     *
//*************************************************
class TVal;
class TArchiveS;
class TVArchEl;

class TVArchive : public TCntrNode, public TValBuf, public TConfig
{
    public:
	//Public data
	enum SrcMode { Passive, PassiveAttr, ActiveAttr };

	//Public methods
	TVArchive( const string &id, const string &db, TElem *cf_el );
	~TVArchive( );

	TCntrNode &operator=( TCntrNode &node );

	//> Base functions
	const string &id( )	{ return mId; }
	string name( );
	string dscr( )		{ return mDscr; }
	SrcMode srcMode( )	{ return (TVArchive::SrcMode)mSrcMode; }
	string  srcData( )	{ return mDSourc; }
	bool toStart( )  	{ return mStart; }
	bool startStat( )	{ return run_st; }

	string DB( )		{ return mDB; }
	string tbl( );
	string fullDB( )	{ return DB()+'.'+tbl(); }

	long long end( const string &arch = BUF_ARCH_NM );
	long long begin( const string &arch = BUF_ARCH_NM );
	long long period( const string &arch = BUF_ARCH_NM );
	TFld::Type valType( )	{ return TValBuf::valType(); }
	bool hardGrid( )	{ return TValBuf::hardGrid(); }
	bool highResTm( )	{ return TValBuf::highResTm(); }
	int size( )		{ return TValBuf::size(); }

	void setName( const string &inm )	{ mName = inm; modif(); }
	void setDscr( const string &idscr )	{ mDscr = idscr; modif(); }
	void setSrcMode( SrcMode vl, const string &isrc = "" );
	void setToStart( bool vl )		{ mStart = vl; modif(); }

	void setDB( const string &idb )		{ mDB = idb; modifG(); }

	void setValType( TFld::Type vl );
	void setHardGrid( bool vl );
	void setHighResTm( bool vl );
	void setSize( int vl );
	void setPeriod( long long vl );

	//> Service
	void start( );
	void stop( bool full_del = false );

	//> Get value
	TVariant getVal( long long *tm = NULL, bool up_ord = false, const string &arch = "", bool onlyLocal = false );
	void getVals( TValBuf &buf, long long beg = 0, long long end = 0,
		const string &arch = "", int limit = 100000, bool onlyLocal = false );
	void setVals( TValBuf &buf, long long beg, long long end, const string &arch );

	//> Active get data from atribute
	void getActiveData( );

	//> Phisical archivator's functions
	void archivatorList( vector<string> &ls );
	bool archivatorPresent( const string &arch );
	void archivatorAttach( const string &arch );
	void archivatorDetach( const string &arch, bool full = false );
	void archivatorSort( );

	string makeTrendImg( long long beg, long long end, const string &arch, int hsz = 650, int vsz = 230, double valmax = 0, double valmin = 0 );

	TArchiveS &owner( );

	void cntrCmdProc( XMLNode *opt );       //Control interface command process

    protected:
	//Protected methods
	void preDisable( int flag );
	void postDisable( int flag );

	void load_( );
	void save_( );

    private:
	//Private methods
	void setUpBuf( );
	const string &nodeName( )	{ return mId; }

	//Private attributes
	Res	a_res;
	bool	run_st;
	string	mDB;
	//> Base params
	string  &mId,		//ID
		&mName,		//Name
		&mDscr,		//Description
		&mDSourc,	//Data source (parameter atribute)
		&mArchs;	//Use archivators list (separated whith ';')
	bool	&mStart;	//Starting flag
	int	&mSrcMode;	//Source mode
	//> Buffer params
	int	&mVType;	//Value type (int, real, bool, string)
	double	&mBPer;		//Buffer period
	int	&mBSize;	//Buffer size
	bool	&mBHGrd,	//Buffer use hard time griding
		&mBHRes;	//Buffer use high time resolution
	//> Mode params
	AutoHD<TVal>	pattr_src;
	//> Phisical archive's elements
	vector<TVArchEl*> arch_el;	//Links
};


//*************************************************
//* TVArchivator                                  *
//*************************************************
class TTipArchivator;

class TVArchivator : public TCntrNode, public TConfig
{
    friend void TVArchive::archivatorAttach( const string &arch );
    friend void TVArchive::archivatorDetach( const string &arch, bool full = false );

    public:
	//Public methods
	TVArchivator( const string &id, const string &db, TElem *cf_el );
	~TVArchivator( );

	TCntrNode &operator=( TCntrNode &node );

	const string &id( )	{ return mId; }
	string workId( );
	string name( );
	string dscr( )		{ return mDscr; }
	string addr( )		{ return mAddr; }
	double valPeriod( )	{ return mVPer; }
	int    archPeriod( )	{ return mAPer; }

	bool toStart( )		{ return mStart; }
	bool startStat( )	{ return run_st; }

	string DB( )		{ return mDB; }
	string tbl( );
	string fullDB( )		{ return DB()+'.'+tbl(); }

	void setName( const string &inm )	{ mName = inm; modif(); }
	void setDscr( const string &idscr )	{ mDscr = idscr; modif(); }
	void setAddr( const string &vl )	{ mAddr = vl; modif(); }
	virtual void setValPeriod( double per );
	virtual void setArchPeriod( int per )	{ mAPer = (per?per:1); modif(); }
	void setToStart( bool vl )		{ mStart = vl; modif(); }

	void setDB( const string &idb )		{ mDB = idb; modif(); }

	virtual void start( );
	virtual void stop( bool full_del = false );

	//> Place archive functions
	void archiveList( vector<string> &ls );
	bool archivePresent( const string &iid );

	TTipArchivator &owner( );

    protected:
	//Protected methods
	//> Protected place archive functions
	TVArchEl *archivePlace( TVArchive &item );
	void archiveRemove( const string &id, bool full = false );

	virtual TVArchEl *getArchEl( TVArchive &arch );

	void cntrCmdProc( XMLNode *opt );	//Control interface command process
	void postEnable( int flag );
	void preDisable( int flag );
	void postDisable( int flag );		//Delete all DB if flag 1

	void load_( );
	void save_( );

	//Protected attributes
	Res	a_res;
	bool	run_st, endrunReq;
	//> Phisical elements storing
	map<string,TVArchEl*> archEl;

    private:
	//Private methods
	const string &nodeName( )	{ return mId; }
	static void *Task( void *param );	//Process task

	//Private attributes
	string	&mId,		//Var arch id
		&mName,		//Var arch name
		&mDscr,		//Var arch description
		&mAddr;		//Mess arch phisical address
	bool	&mStart;	//Var arch starting flag
	double	&mVPer;		//Value period (sec)
	int	&mAPer;		//Archivation period
	string	mDB;
	//> Archivate process
	double	tm_calc;	//Archiving time
};

//*************************************************
//* TVArchEl                                      *
//*************************************************
class TVArchEl
{
    friend class TVArchivator;

    public:
	//Public methods
	TVArchEl( TVArchive &iachive, TVArchivator &iarchivator );
	virtual ~TVArchEl( );
	virtual void fullErase( )	{ }

	virtual long long end( )	{ return 0; }		//Archive end
	virtual long long begin( )	{ return 0; }		//Archive begin
	long long lastGet( )		{ return mLastGet; }	//Last getted value time

	TVariant getVal( long long *tm, bool up_ord, bool onlyLocal = false );
	void getVals( TValBuf &buf, long long beg = 0, long long end = 0, bool onlyLocal = false );
	void setVals( TValBuf &buf, long long beg = 0, long long end = 0 );

	TVArchive &archive( );
	TVArchivator &archivator( );

    protected:
	//Protected methods
	virtual TVariant getValProc( long long *tm, bool up_ord );
	virtual void getValsProc( TValBuf &buf, long long beg, long long end )	{ }
	virtual void setValsProc( TValBuf &buf, long long beg, long long end )	{ }

	//> Previous averaging value
	long long prev_tm;
	string prev_val;

    private:
	//Private attributes
	TVArchive	&mArchive;
	TVArchivator	&mArchivator;

	long long mLastGet;
};

}

#endif // TArchVal_H
