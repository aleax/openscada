
//OpenSCADA system file: tarchval.h
/***************************************************************************
 *   Copyright (C) 2006-2014 by Roman Savochenko, <rom_as@oscada.org>      *
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
//* values types: bool, integer (16, 32, 64),     *
//*               real (float, double) or string. *
//*************************************************
class TValBuf
{
    public:
	//Public methods
	TValBuf( );
	TValBuf( TFld::Type vtp, int isz, int64_t ipr, bool ihgrd = false, bool ihres = false );
	virtual ~TValBuf( );

	TValBuf &operator=( TValBuf &src );

	void clear( );

	TFld::Type valType( bool full = false )	{ return full ? mValTp : (TFld::Type)(mValTp&TFld::GenMask); }
	bool hardGrid( )	{ return mHrdGrd; }
	bool highResTm( )	{ return mHgResTm; }
	int size( )		{ return mSize; }
	int realSize( );
	unsigned int evalCnt( )	{ return mEvalCnt; }

	int64_t begin( )	{ return mBeg; }
	int64_t end( )		{ return mEnd; }
	int64_t period( )	{ return mPer; }

	bool vOK( int64_t ibeg, int64_t iend );

	void setValType( TFld::Type vl );
	void setHardGrid( bool vl );
	void setHighResTm( bool vl );
	void setSize( int vl );
	void setPeriod( int64_t vl );

	// Get value
	virtual void getVals( TValBuf &buf, int64_t beg = 0, int64_t end = 0 );
	TVariant get( int64_t *tm = NULL, bool up_ord = false );
	virtual string getS( int64_t *tm = NULL, bool up_ord = false );
	virtual double getR( int64_t *tm = NULL, bool up_ord = false );
	virtual int64_t getI( int64_t *tm = NULL, bool up_ord = false );
	virtual char   getB( int64_t *tm = NULL, bool up_ord = false );

	// Set value
	virtual void setVals( TValBuf &buf, int64_t beg = 0, int64_t end = 0 );
	void set( const TVariant &value, int64_t tm = 0 );
	virtual void setS( const string &value, int64_t tm = 0 );
	virtual void setR( double value, int64_t tm = 0 );
	virtual void setI( int64_t value, int64_t tm = 0 );
	virtual void setB( char value, int64_t tm = 0 );

	void makeBuf( TFld::Type v_tp, int isz, int64_t ipr, bool hd_grd, bool hg_res );	//Create new or change buffer mode (all data into buffer will lost)

    private:
	//Private data and attributes
	template <class TpVal> class TBuf
	{
	    friend class TValBuf;
	    public:
		//Public methods
		TBuf( TpVal eval, int &isz, int64_t &ipr, bool &ihgrd, bool &ihres,
		    int64_t &iend, int64_t &ibeg, unsigned int &iEvalCnt );
		~TBuf( );

		void clear( );

		int realSize( );

		TpVal get( int64_t *tm = NULL, bool up_ord = false );
		void  set( TpVal value, int64_t tm = 0 );

		// Create new or change buffer mode (all data into buffer will lost)
		void makeBuf( int isz, int64_t ipr, bool hd_grd, bool hg_res );

	    private:
		//Private attributes
		bool	&hg_res_tm,
			&hrd_grd;
		int64_t	&end, &beg,
			&per;
		int	&size,
			cur;		//Curent position.
		TpVal	eval;		//Error element value
		unsigned int &mEvalCnt;

		struct SLw  { time_t tm; TpVal val; };
		struct SHg  { int64_t tm; TpVal val; };
		union {
		    vector<TpVal>	*grid;
		    vector<SLw>		*tm_low;
		    vector<SHg>		*tm_high;
		}buf;
	};

	Res		bRes;		//Access resource
	TFld::Type	mValTp;		//Store values type
	union {
	    TBuf<char>		*bl;
	    TBuf<int16_t>	*i16;
	    TBuf<int32_t>	*i32;
	    TBuf<int64_t>	*i64;
	    TBuf<float>		*rFlt;
	    TBuf<double>	*rDbl;
	    TBuf<string>	*str;
	} buf;

	bool	mHgResTm,		//High resolution time use (microseconds)
		mHrdGrd;		//Set hard griding. It mode no support the zero periodic.
	int64_t	mEnd, mBeg,		//Time of buffer begin and end (64 bit usec)
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
	enum SrcMode { SaveCur = -1, Passive = 0, PassiveAttr, ActiveAttr };
	enum CombMode { MovAver = 0, LastVal, MinVal, MaxVal };

	//Public methods
	TVArchive( const string &id, const string &db, TElem *cf_el );
	~TVArchive( );

	TCntrNode &operator=( TCntrNode &node );

	// Base functions
	string	id( )		{ return mId; }
	string	name( );
	string	dscr( )		{ return cfg("DESCR").getS(); }
	SrcMode	srcMode( )	{ return (TVArchive::SrcMode)mSrcMode.getI(); }
	string	srcData( )	{ return mSource; }
	CombMode combMode( )	{ return (TVArchive::CombMode)mCombMode.getI(); }
	AutoHD<TVal> srcPAttr( bool force = false, const string &ipath = "" );
	bool toStart( )  	{ return mStart; }
	bool startStat( )	{ return runSt; }

	string DB( )		{ return mDB; }
	string tbl( );
	string fullDB( )	{ return DB()+'.'+tbl(); }

	int64_t end( const string &arch = BUF_ARCH_NM );
	int64_t begin( const string &arch = BUF_ARCH_NM );
	int64_t period( const string &arch = BUF_ARCH_NM );
	TFld::Type valType( bool full = false )	{ return TValBuf::valType(full); }
	bool hardGrid( )	{ return TValBuf::hardGrid(); }
	bool highResTm( )	{ return TValBuf::highResTm(); }
	int size( )		{ return TValBuf::size(); }

	void setName( const string &inm )	{ cfg("NAME").setS(inm); }
	void setDscr( const string &idscr )	{ cfg("DESCR").setS(idscr); }
	void setSrcMode( SrcMode vl = SaveCur, const string &isrc = "<*>", bool noex = false );
	void setCombMode( CombMode vl )		{ mCombMode = (int)vl; }
	void setToStart( bool vl )		{ mStart = vl; modif(); }

	void setDB( const string &idb )		{ mDB = idb; modifG(); }

	void setValType( TFld::Type vl );
	void setHardGrid( bool vl );
	void setHighResTm( bool vl );
	void setSize( int vl );
	void setPeriod( int64_t vl );

	// Service
	void start( );
	void stop( bool full_del = false );

	// Get value
	TVariant getVal( int64_t *tm = NULL, bool up_ord = false, const string &arch = "", bool onlyLocal = false );
	void getVals( TValBuf &buf, int64_t beg = 0, int64_t end = 0,
		const string &arch = "", int limit = 100000, bool onlyLocal = false );
	void setVals( TValBuf &buf, int64_t beg, int64_t end, const string &arch );

	// Active get data from atribute
	void getActiveData( );

	// Phisical archivator's functions
	void archivatorList( vector<string> &ls );
	bool archivatorPresent( const string &arch );
	void archivatorAttach( const string &arch );
	void archivatorDetach( const string &arch, bool full = false, bool toModify = true );
	void archivatorSort( );

	string makeTrendImg( int64_t beg, int64_t end, const string &arch,
	    int hsz = 650, int vsz = 230, double valmax = 0, double valmin = 0, string *tp = NULL );

	TArchiveS &owner( );

	void cntrCmdProc( XMLNode *opt );       //Control interface command process

    protected:
	//Protected methods
	void preDisable( int flag );
	void postDisable( int flag );
	bool cfgChange( TCfg &co, const TVariant &pc );

	void load_( );
	void save_( );

	TVariant objFuncCall( const string &id, vector<TVariant> &prms, const string &user );

    private:
	//Private methods
	void setUpBuf( );
	const char *nodeName( )	{ return mId.getSd(); }

	//Private attributes
	Res	aRes;
	bool	runSt;
	string	mDB;

	TCfg	&mId,		//ID
		&mVType,	//Value type (int, real, bool, string)
		&mSrcMode,	//Source mode
		&mSource,	//Source
		&mCombMode,	//Data combining mode (Moving average, Single, Minimum, Maximum)
		&mBPer,		//Buffer period
		&mBSize;	//Buffer size
	char	&mStart,	//Starting flag
		&mBHGrd,	//Buffer use hard time griding
		&mBHRes;	//Buffer use high time resolution
	// Mode params
	AutoHD<TVal>	pattr_src;
	// Phisical archive's elements
	vector<TVArchEl*> arch_el;	//Links
};

//*************************************************
//* TVArchivator                                  *
//*************************************************
class TTipArchivator;

class TVArchivator : public TCntrNode, public TConfig
{
    friend void TVArchive::archivatorAttach( const string &arch );
    friend void TVArchive::archivatorDetach( const string &arch, bool full = false, bool toModify = true );

    public:
	//Public methods
	TVArchivator( const string &id, const string &db, TElem *cf_el );
	~TVArchivator( );

	TCntrNode &operator=( TCntrNode &node );

	string	id( )		{ return mId; }
	string	workId( );
	string	name( );
	string	dscr( )		{ return cfg("DESCR").getS(); }
	string	addr( )		{ return cfg("ADDR").getS(); }
	double	valPeriod( )	{ return mVPer; }
	int	archPeriod( )	{ return mAPer; }
	int	selPrior( )	{ return mSelPrior; }

	bool toStart( )		{ return mStart; }
	bool startStat( )	{ return runSt; }

	string DB( )		{ return mDB; }
	string tbl( );
	string fullDB( )	{ return DB()+'.'+tbl(); }

	void setName( const string &inm )	{ cfg("NAME").setS(inm); }
	void setDscr( const string &idscr )	{ cfg("DESCR").setS(idscr); }
	void setAddr( const string &vl )	{ cfg("ADDR").setS(vl); }
	void setValPeriod( double per )		{ mVPer = per; }
	void setArchPeriod( int per )		{ mAPer = per; }
	void setSelPrior( int vl )		{ mSelPrior = std::max(0,std::min(1000,vl)); modif(); }
	void setToStart( bool vl )		{ mStart = vl; modif(); }

	void setDB( const string &idb )		{ mDB = idb; modif(); }

	virtual void start( );
	virtual void stop( bool full_del = false );

	TVariant objFuncCall( const string &id, vector<TVariant> &prms, const string &user );

	// Place archive functions
	void archiveList( vector<string> &ls );
	bool archivePresent( const string &iid );

	TTipArchivator &owner( );

    protected:
	//Protected methods
	// Protected place archive functions
	TVArchEl *archivePlace( TVArchive &item );
	void archiveRemove( const string &id, bool full = false );

	virtual TVArchEl *getArchEl( TVArchive &arch );

	void cntrCmdProc( XMLNode *opt );	//Control interface command process
	void postEnable( int flag );
	void preDisable( int flag );
	void postDisable( int flag );		//Delete all DB if flag 1
	bool cfgChange( TCfg &co, const TVariant &pc );

	void load_( );
	void save_( );

	//Protected attributes
	Res	archRes;
	bool	runSt, endrunReq;
	// Phisical elements storing
	map<string,TVArchEl*> archEl;

    private:
	//Private methods
	const char *nodeName( )		{ return mId.getSd(); }
	static void *Task( void *param );	//Process task

	//Private attributes
	TCfg	&mId,		//Var arch id
		&mVPer,		//Value period (sec)
		&mAPer;		//Archivation period
	char	&mStart;	//Var arch starting flag
	int64_t	&mSelPrior;	//Selection priority
	string	mDB;
	// Archivate process
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
	TVArchEl( TVArchive &iarchive, TVArchivator &iarchivator );
	virtual ~TVArchEl( );
	virtual void fullErase( )	{ }

	virtual int64_t end( )		{ return 0; }		//Archive end
	virtual int64_t begin( )	{ return 0; }		//Archive begin
	int64_t lastGet( )		{ return mLastGet; }	//Last getted value time

	TVariant getVal( int64_t *tm, bool up_ord, bool onlyLocal = false );
	void getVals( TValBuf &buf, int64_t beg = 0, int64_t end = 0, bool onlyLocal = false );
	void setVals( TValBuf &buf, int64_t beg = 0, int64_t end = 0 );

	TVArchive &archive( );
	TVArchivator &archivator( );

    protected:
	//Protected methods
	virtual TVariant getValProc( int64_t *tm, bool up_ord );
	virtual void getValsProc( TValBuf &buf, int64_t beg, int64_t end )	{ }
	virtual bool setValsProc( TValBuf &buf, int64_t beg, int64_t end )	{ return false; }

	// Previous averaging value
	int64_t	prev_tm;
	string	prev_val;

    private:
	//Private attributes
	TVArchive	&mArchive;
	TVArchivator	&mArchivator;

	int64_t	mLastGet;
};

}

#endif // TArchVal_H
