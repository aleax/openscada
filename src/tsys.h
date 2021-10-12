
//OpenSCADA file: tsys.h
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

#ifndef TSYS_H
#define TSYS_H

//Program constants
#define PACKAGE_LICENSE	"GPL v2"
#define PACKAGE_DESCR	_("Open Supervisory Control And Data Acquisition")
#define PACKAGE_AUTHOR	_("Roman Savochenko")
#define PACKAGE_SITE	"http://oscada.org"

//Other global constants
#define NSTR_BUF_LEN	50	// Length of string buffers for numbers
#define MESS_LEV_MAX	99	// Message maximum level
#define DAQ_APER_FRQ	1000000	// Frequency of representing the aperiodic invokes, like to f_start, of the periodic processes in Hz
#define RECURS_DET_HOPS	20	// Hops/depth number of the recursion detection

// ????[API,v1.0] Remove - left for the compatibility
#define OBJ_ID_SZ	"20"
#define OBJ_NM_SZ	"100"
#define ARCH_ID_SZ	"70"
#define STD_WAIT_DELAY	100
#define USER_FILE_LIMIT	1048576
#define USER_ITS_LIMIT	1000000
#define STD_CACHE_LIM	100
#define STR_BUF_LEN	10000
#define STD_WAIT_TM	5
#define STD_INTERF_TM	7
#define SERV_TASK_PER	10

#define ARCH_BUF	"<buffer>"
#define ARCH_ALRM	"<alarms>"
#define ARCH_ALRM_CH	"<alarmsChange>"
#define ARCH_NOALRM	"<noalarms>"
#define DB_CFG		"<cfg>"
#define DB_NULL		"<NULL>"

#include <signal.h>
#include <unistd.h>
#include <stdint.h>
#include <math.h>
#include <stdio.h>

#include <string>
#include <vector>

#define __func__ __PRETTY_FUNCTION__

#define vmin(a,b) ((a) < (b) ? (a) : (b))
#define vmax(a,b) ((a) > (b) ? (a) : (b))

#ifndef UINT16_MAX
# define UINT16_MAX (65535)
#endif

#ifdef HAVE_CONFIG_H
# include "config.h"
#endif
#include "tbds.h"
#include "tuis.h"
#include "tarchives.h"
#include "tdaqs.h"
#include "tprotocols.h"
#include "ttransports.h"
#include "tspecials.h"
#include "tmodschedul.h"
#include "tsecurity.h"

using std::string;
using std::vector;

namespace OSCADA
{

//Global configurable Limits
extern uint8_t	limObjID_SZ;	//[20..50] ID size of the OpenSCADA objects.
extern uint8_t	limObjNm_SZ;	//[100...200] NAME size of the OpenSCADA objects.
extern uint8_t	limArchID_SZ;	//[50...90] ID size of the value archive objects, limObjID_SZ + 1.5*limObjID_SZ.
extern int	limUserFile_SZ;	//[1MB...*10MB...1000MB] The files size limit at loading and processing in the userspace
				//	and the part size of the big files transferring.
extern int	limUserIts_N;	//[1000...*1000000...1000000000] The limit on count of creating user items, like to array items.
extern unsigned	limCacheIts_N;	//[*100...100000] The limit on count of the caching items.

//Global configurable parameters
extern int	prmStrBuf_SZ;	//[1000...*10000...1000000] Length of string buffers, no string class
extern float	prmWait_DL;	//[0.001...*0.1...1] Quantum of the waiting time cycles, seconds
extern uint8_t	prmWait_TM;	//[*5...10] Standard waiting timeout length, seconds
extern uint8_t	prmInterf_TM;	//[*7...15] Time of waiting for the interface reaction, seconds
extern uint8_t	prmServTask_PER;//[1...*10...120] Service task period, seconds

//*************************************************
//* TSYS					  *
//*************************************************
class TSYS : public TCntrNode
{
    friend class TMess;

    public:
	//Data
	enum Code	{ PathEl, HttpURL, Html, JavaSc, SQL, Custom, base64, FormatPrint, oscdID, Bin, Reverse, ShieldSimb, ToLower, Limit };
	enum IntView	{ Dec, Oct, Hex };

	// Task structure
	class STask
	{
	    public:
		//Data
		enum Flgs	{ Detached = 0x01, FinishTask = 0x02 };

		//Methods
		STask( ) : thr(0), policy(0), phase(0), prior(0), tid(0), flgs(0), tm_beg(0), tm_end(0), tm_per(0), tm_pnt(0),
		    cycleLost(0), lagMax(0), consMax(0)	{ }
		STask( pthread_t ithr, char ipolicy, char iprior ) :
		    thr(ithr), policy(ipolicy), phase(0), prior(iprior), tid(0), flgs(0), tm_beg(0), tm_end(0), tm_per(0), tm_pnt(0),
		    cycleLost(0), lagMax(0), consMax(0)	{ }

		float consumpt( ) const	{ return tm_beg ? 1e-9*(tm_end-tm_beg) : 0; }
		float period( ) const	{ return tm_beg ? 1e-9*(tm_per-tm_beg) : 0; }

		//Attributes
		string		path;
		pthread_t	thr;
		uint8_t		policy, phase;
		int16_t		prior;
		pid_t		tid;
		ResString	cpuSet;
		void *(*task) (void *);
		void		*taskArg;
		unsigned	flgs;
		int64_t		tm_beg, tm_end, tm_per, tm_pnt, cycleLost, lagMax, consMax;
	};

	// Remote station structure
	class SStat {
	    public:
		SStat( int8_t ilev, bool iisLive = false, float icnt = 0 ) :
		    lev(ilev), isLive(iisLive), isLivePrev(iisLive), cnt(icnt) { }
		SStat( ) : lev(-1), isLive(false), isLivePrev(false), cnt(0) { }

		bool isActive( )	{ return (lev >= 0 && isLive); }

		int8_t	lev;
		bool	isLive, isLivePrev;
		float	cnt;

		ResMtx	reqM;
	};

	//Public methods
	TSYS( int argi, char **argb, char **env );
	~TSYS( );

	// Station statuses
	bool	isRunning( )	{ return mRunning; }
	bool	isFinalKill( )	{ return mFinalKill; }
	int	stopSignal( )	{ return mStopSignal; }

	void	unload( );

	int	start( );
	void	stop( int sig = SIGUSR1 );	// SIGUSR2 used for reloading from other project

	// Program options
	string	id( )		{ return mId.c_str(); }
	string	name( )		{ return mName; }
	void setName( const string &vl )	{ mName = vl; modif(); }
	string	user( )		{ return mUser; }	//Run user name
	string	host( );

	void	list( vector<string> &list ) const	{ chldList(mSubst, list); }
	bool	present( const string &name ) const	{ return chldPresent(mSubst, name); }
	void	add( TSubSYS *sub )			{ chldAdd(mSubst, sub); }
	void	del( const string &name )		{ chldDel(mSubst, name); }
	AutoHD<TSubSYS> at( const string &name ) const	{ return chldAt(mSubst, name); }

	AutoHD<TUIS>		ui( )		{ return at("UI"); }
	AutoHD<TArchiveS>	archive( )	{ return at("Archive"); }
	AutoHD<TBDS>		db( )		{ return at("BD"); }
	AutoHD<TDAQS>		daq( )		{ return at("DAQ"); }
	AutoHD<TProtocolS>	protocol( )	{ return at("Protocol"); }
	AutoHD<TTransportS>	transport( )	{ return at("Transport"); }
	AutoHD<TSpecialS>	special( )	{ return at("Special"); }
	AutoHD<TModSchedul>	modSchedul( )	{ return at("ModSched"); }
	AutoHD<TSecurity>	security( )	{ return at("Security"); }

	string	workDir( );
	string	modDir( )	{ return mModDir; }
	string	icoDir( )	{ return mIcoDir; }
	string	docDir( )	{ return mDocDir; }
	void	setWorkDir( const string &wdir, bool init = false );
	void	setModDir( const string &mdir, bool init = false );
	void	setIcoDir( const string &idir, bool init = false );
	void	setDocDir( const string &idir, bool init = false );

	// Config-file functions
	string	cfgFile( )	{ return mConfFile; }
	XMLNode	&cfgRoot( )	{ return rootN; }
	XMLNode	*cfgNode( const string &path, bool create = false );
	void	modifCfg( bool chkPossibleWR = false );
	ResRW	&cfgRes( )	{ return mCfgRes; }

	string	workDB( )	{ return mWorkDB; }
	string	mainCPUs( )	{ return mMainCPUs; }
	bool	clockRT( )	{ return mClockRT; }
	int	taskInvPhs( )	{ return mTaskInvPhs; }
	bool	saveAtExit( )	{ return mSaveAtExit; }
	int	savePeriod( )	{ return mSavePeriod; }
	bool	modifCalc( )	{ return mModifCalc; }

	void	setWorkDB( const string &wdb )	{ mWorkDB = wdb; /*modifG();*/ }	//!!!! Do not mark all objects modification
											//     and to save/load all them use the force operations
	void	setMainCPUs( const string &vl );
	void	setClockRT( bool vl )		{ mClockRT = vl; modif(); }
	void	setTaskInvPhs( int vl );
	void	setSaveAtExit( bool vl )	{ mSaveAtExit = vl; modif(); }
	void	setSavePeriod( int vl )		{ mSavePeriod = vmax(0,vl); modif(); }
	void	setModifCalc( bool vl )		{ mModifCalc = vl; modif(); }

	//  Configuration loading
	string	selDB( )	{ return mSelDB; }
	void	setSelDB( const string &vl )	{ mSelDB = vl; }
	bool	chkSelDB( const string& wDB, bool isStrong = false );
	XMLNode *cfgCtx( bool last = false )	{ return last ? mCfgCtxLast : mCfgCtx; }
	void	setCfgCtx( XMLNode *vl, bool last = false ) {
	    if(last) mCfgCtxLast = vl;
	    else { mCfgCtx = vl; mCfgCtxLast = NULL; }
	}
	ResMtx &cfgLoadSaveM( )	{ return mCfgLoadSaveM; }

	static void sighandler( int signal, siginfo_t *siginfo, void *context );

	// Short time dimensions
	int	nCPU( )		{ return mN_CPU; }
	uint64_t sysClk( )	{ return mSysclc; }
	void	clkCalc( );
	uint64_t shrtCnt( ) {
#if defined (__i386__) || defined (__x86_64__)
	    unsigned int cntl, cnth;
	    asm volatile("rdtsc; movl %%eax,%0; movl %%edx,%1;":"=r"(cntl),"=r"(cnth)::"%eax","%edx");
	    return ((uint64_t)cnth<<32)+cntl;
#else
	    return 0;
#endif
	}
	static long HZ( );

	time_t	sysTm( ) volatile	{ return mSysTm ? mSysTm : time(NULL); }	//System time fast access, from updated cell
	static int64_t curTime( clockid_t clc = CLOCK_REALTIME );	//Current system time, microseconds
	static uint64_t curTimeN( clockid_t clc = CLOCK_REALTIME );	//Current system time, nanoseconds

	// Projects
	string prjUserDir( );
	bool prjCustMode( )			{ return mPrjCustMode; }
	void setPrjCustMode( bool vl )		{ mPrjCustMode = vl; }
	string prjNm( )				{ return mPrjNm; }
	void setPrjNm( const string &vl )	{ mPrjNm = vl; }

	bool prjSwitch( const string &prj, bool toCreate = false );

	int  prjLockUpdPer( );
	bool prjLock( const char *cmd );	//<cmd> variants: "hold", "free", "update"

	// Tasks control
	void taskCreate( const string &path, int priority, void *(*start_routine)(void *), void *arg, int wtm = 5, pthread_attr_t *pAttr = NULL, bool *startSt = NULL );
	void taskDestroy( const string &path, bool *endrunCntr = NULL, int wtm = 5, bool noSignal = false, pthread_cond_t *cv = NULL );
	void taskSendSIGALRM( const string &path );
	double taskUtilizTm( const string &path, bool max = false );
	static bool taskEndRun( );		// Check for the task endrun by signal SIGUSR1
	static const STask& taskDescr( );	// Get the current task control structure

	// Sleep task for period grid <per> on ns or to cron time.
	static int sysSleep( float tm );			//System sleep in seconds down to nanoseconds (1e-9)
	static void taskSleep( int64_t per, const string &cron = "", int64_t *lag = NULL );	//<per> in nanoseconds
	static time_t cron( const string &vl, time_t base = 0 );

	// Wait event with timeout support
	static bool eventWait( bool &m_mess_r_stat, bool exempl, const string &loc, time_t time = 0 );

	// Program counters
	bool	cntrEmpty( );
	double	cntrGet( const string &id );
	void	cntrSet( const string &id, double vl );
	void	cntrIter( const string &id, double vl );

	// Redundancy
	bool rdEnable( );		//any external host set
	bool rdActive( );		//any external host active
	void rdStList( vector<string> &ls );
	SStat rdSt( const string &id );
	map<string, SStat> rdSts( );
	int rdStLevel( )		{ return mRdStLevel; }
	void setRdStLevel( int vl )	{ mRdStLevel = vmin(255,vmax(0,vl)); modif(); }
	float rdTaskPer( )		{ return mRdTaskPer; }
	void setRdTaskPer( float vl )	{ mRdTaskPer = vmin(255,vmax(0.1,vl)); modif(); }
	int rdRestConnTm( )		{ return mRdRestConnTm; }
	void setRdRestConnTm( int vl )	{ mRdRestConnTm = vmin(255,vmax(0,vl)); modif(); }
	bool rdPrimCmdTr( )		{ return mRdPrimCmdTr; }
	void setRdPrimCmdTr( bool vl )	{ mRdPrimCmdTr = vl; modif(); }
	string rdStRequest( XMLNode &req, const string &st = "", bool toScan = false );	//Request to a remote station if <st> is empty or force

	// Convert value to string
	static string int2str( int val, IntView view = Dec );
	static string uint2str( unsigned val, IntView view = Dec );
	static string ll2str( long long val, IntView view = Dec );
	static string real2str( double val, int prec = 15, char tp = 'g' );
	static double realRound( double val, int dig = 0, bool toint = false ) {
	    double rez = floor(val*pow(10,dig)+0.5)/pow(10,dig);
	    return toint ? floor(rez+0.5) : rez;
	}
	static string atime2str( time_t tm, const string &format = "", bool gmt = false );
	static string time2str( double tm );
	static string cpct2str( double cnt );

	// Convert string to value
	static double str2real( const string &val );
	static time_t str2atime( const string &val, const string &format = "", bool gmt = false );

	// Adress convertors
	static string addr2str( void *addr );
	static void *str2addr( const string &str );

	// Path and string parse
	static string strTrim( const string &val, const string &cfg = " \n\t\r" );
	static string strSepParse( const string &str, int level, char sep, int *off = NULL );
	static string strParse( const string &str, int level, const string &sep, int *off = NULL, bool mergeSepSymb = false );
	static string strParseEnd( const string &str, int level, const string &sep, int *off = NULL, bool mergeSepSymb = false );
	static string strLine( const string &str, int level, int *off = NULL );
	static string pathLev( const string &path, int level, bool decode = true, int *offCmtbl = NULL, int *off = NULL );
	static string pathLevEnd( const string &path, int level, bool decode = true, int *off = NULL );
	static string path2sepstr( const string &path, char sep = '.' );
	static string sepstr2path( const string &str, char sep = '.' );
	static string strEncode( const string &in, Code tp, const string &opt1 = "" );
	static string strDecode( const string &in, Code tp = Custom, const string &opt1 = "" );
	static string strMess( const char *fmt, ... );
	static string strLabEnum( const string &base );

	static string strCompr( const string &in, int lev = -1 );
	static string strUncompr( const string &in );

	// Unaligned read from memory for some ARM and other
	static inline uint16_t getUnalign16( const void *p ) {
	    struct su16 { uint16_t x; } __attribute__((packed));
	    const struct su16 *ptr = (const struct su16 *)p;
	    return ptr->x;
	}
	static inline uint32_t getUnalign32( const void *p ) {
	    struct su32 { uint32_t x; } __attribute__((packed));
	    const struct su32 *ptr = (const struct su32 *)p;
	    return ptr->x;
	}
	static inline uint64_t getUnalign64( const void *p ) {
	    struct su64 { uint64_t x; } __attribute__((packed));
	    const struct su64 *ptr = (const struct su64 *)p;
	    return ptr->x;
	}
	static inline int getUnalignInt( const void *p ) {
	    struct suInt { int x; } __attribute__((packed));
	    const struct suInt *ptr = (const struct suInt *)p;
	    return ptr->x;
	}
	static inline float getUnalignFloat( const void *p ) {
	    struct sFloat64 { float x; } __attribute__((packed));
	    const struct sFloat64 *ptr = (const struct sFloat64 *)p;
	    return ptr->x;
	}
	static inline double getUnalignDbl( const void *p ) {
	    struct sDbl { double x; } __attribute__((packed));
	    const struct sDbl *ptr = (const struct sDbl *)p;
	    return ptr->x;
	}

	// Endian convert
	static uint16_t i16_LE( uint16_t in );
	static uint32_t i32_LE( uint32_t in );
	static uint64_t i64_LE( uint64_t in );
	static uint16_t i16_BE( uint16_t in );
	static uint32_t i32_BE( uint32_t in );
	static uint64_t i64_BE( uint64_t in );
	static float floatLE( float in );
	static float floatLErev( float in );
	static double doubleLE( double in );
	static double doubleLErev( double in );
	static float floatBE( float in );
	static float floatBErev( float in );
	static double doubleBE( double in );
	static double doubleBErev( double in );

	// Reentrant commandline processing
	string	optDescr( );	//get comand line options

	string	getCmdOpt( int &curPos, string *argVal = NULL );
	static string getCmdOpt_( int &curPos, string *argVal, int argc, char **argv );

	bool	cmdOptPresent( const string &opt );
	string	cmdOpt( const string &opt, const string &setVl = "" );

	int permCrtFiles( bool exec = false );

	ResMtx	*commonLock( const string &nm );

	// Control interface functions
	static void ctrListFS( XMLNode *nd, const string &fsBase, const string &fileExt = "" );	//Inline file system browsing

	TVariant objFuncCall( const string &id, vector<TVariant> &prms, const string &user );

	//Public attributes
	AutoHD<TModule>	mainThr;	//A module to call into the main thread


    protected:
	//Protected methods
	void load_( );
	void save_( );

    private:
	//Data
	enum MdfSYSFlds	{ MDF_WorkDir = 0x01, MDF_IcoDir = 0x02, MDF_ModDir = 0x04, MDF_LANG = 0x08, MDF_DocDir = 0x10 };

	//Private methods
	const char *nodeName( ) const		{ return mId.c_str(); }
	const char *nodeNameSYSM( ) const	{ return mName.c_str(); }
	void	cfgFileLoad( );
	void	cfgFileSave( );
	void	cfgPrmLoad( );
	void	cfgFileScan( bool first = false, bool up = false );
	void	cntrCmdProc( XMLNode *opt );	// Control interface command process

	static unsigned char getBase64Code( unsigned char asymb );

	static void *taskWrap( void *stas );

	static void *ServTask( void * );
	static void *HPrTask( void * );
	static void *RdTask( void * );

	//Private attributes
	const int argc;		// Comand line seting counter.
	const char **argv;	// Comand line seting buffer.
	const char **envp;	// System environment.

	// Station statuses
	bool	isLoaded, mRunning, isServPrc,
	    mFinalKill;		// Final object's kill flag. For dead requsted resources

	// Station properties
	string	mUser,		// An owner user name!
	 	mConfFile,	// Config-file name
		mId,		// Station id
		mName,		// Station name
		mModDir,	// Modules directory
		mIcoDir,	// Icons directory
		mDocDir,	// Icons directory
		prjLockFile;	// Lock file of the project OpenSCADA

	MtxString mWorkDB, mSelDB,// Work and selected DB
		mMainCPUs;	// Main used processors set
	int	mTaskInvPhs;	// Number of phases of the task invoking
	bool	mSaveAtExit;	// Save at exit
	int	mSavePeriod;	// Save period (s) for periodic system saving to DB
	bool	mModifCalc;	// Set modification for the calculated objects

	XMLNode rootN;		// Root of the config-file tree
	string	rootCfgFl;	// Root node's config-file name
	time_t	rootFlTm;	// Root node's config-file's modify time
	unsigned rootModifCnt;	// Root tree modify counter, used for save tree to file detect
	unsigned sysModifFlgs;	// System fields modif flags

	int	mStopSignal,	// Stop station signal
		mSubst;		// Subsystem tree id

	map<string, STask>	mTasks;
	static pthread_key_t	sTaskKey;

	ResRW	taskRes, mCfgRes, mRdRes;

	int		mN_CPU;
	pthread_t	mainPthr;
	uint64_t	mSysclc;
	volatile time_t	mSysTm;
	bool		mClockRT;	//Used clock REALTIME, else it is MONOTONIC

	bool		mPrjCustMode;
	MtxString	mPrjNm;

	map<string, string>	mCmdOpts;	//Commandline options
	map<string, double>	mCntrs;		//Counters
	map<string, SStat>	mSt;		//Remote stations

	ResMtx	mCfgLoadSaveM;
	XMLNode	*mCfgCtx, *mCfgCtxLast;

	unsigned char	mRdStLevel,	//Current station level
			mRdRestConnTm;	//Redundant restore connection to reserve stations timeout in seconds
	float		mRdTaskPer;	//Redundant task period in seconds
	bool		mRdPrimCmdTr;	//Allow transfer local primary commands to redundant ones

	map<string, ResMtx*>	mCommonLocks;

	struct sigaction	sigActOrig;
};

//*************************************************
//* Global functions for OSCADA namespace         *
template <class fVal> fVal fmin( fVal a, fVal b ) { return (a < b) ? a : b; }
template <class fVal> fVal fmax( fVal a, fVal b ) { return (a > b) ? a : b; }

inline string i2s( int val, TSYS::IntView view = TSYS::Dec )	{ return TSYS::int2str(val, view); }
inline string u2s( unsigned val, TSYS::IntView view = TSYS::Dec ){ return TSYS::uint2str(val, view); }
inline string ll2s( long long val, TSYS::IntView view = TSYS::Dec ){ return TSYS::ll2str(val, view); }
inline string r2s( double val, int prec = 15, char tp = 'g' )	{ return TSYS::real2str(val, prec, tp); }
inline double rRnd( double val, int dig = 0, bool toint = false ){ return TSYS::realRound(val, dig, toint); }
inline string atm2s( time_t tm, const string &format = "", bool gmt = false )	{ return TSYS::atime2str(tm, format, gmt); }
inline string tm2s( double tm )					{ return TSYS::time2str(tm); }

inline int s2i( const string &val )		{ return atoi(val.c_str()); }
inline long s2l( const string &val, int base = 10 )		{ return strtol(val.c_str(), NULL, base); }
inline unsigned long s2u( const string &val, int base = 10 )	{ return strtoul(val.c_str(), NULL, base); }
inline long long s2ll( const string &val )	{ return atoll(val.c_str()); }
inline double s2r( const string &val )		{ return /*TSYS::str2real(val); }*/ atof(val.c_str()); }

inline string sTrm( const string &val, const string &cfg = " \n\t\r") { return TSYS::strTrim(val, cfg); }

extern TSYS *SYS;
}

#endif // TSYS_H
