
//OpenSCADA system file: tsys.h
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

#ifndef TSYS_H
#define TSYS_H

//- Programm constants -
#define PACKAGE_LICENSE	"GPL v2"
#define PACKAGE_DESCR	"Open Supervisory Control And Data Acquisition"
#define PACKAGE_AUTHOR	"Roman Savochenko"
#define PACKAGE_SITE	"http://oscada.org.ua"

//- Hardware independent types -
typedef char si8;
typedef short si16;
typedef int si32;
typedef long long si64;
typedef unsigned char ui8;
typedef unsigned short ui16;
typedef unsigned int ui32;
typedef unsigned long long ui64;

//- Other system's constants -
#define TO_FREE		NULL	// Object free
#define STR_BUF_LEN	3000	// Len of string buffers (no string class)
#define STD_WAIT_DELAY	100	// Standart wait dalay (ms)
#define STD_WAIT_TM	10	// Standart timeouts length (s)

#include <math.h>
#include <stdio.h>

#include <string>
#include <vector>

#include "tbds.h"
#include "tuis.h"
#include "tarchives.h"
#include "tdaqs.h"
#include "tprotocols.h"
#include "ttransports.h"
#include "tspecials.h"
#include "tmodschedul.h"
#include "tsecurity.h"

#define __func__ __PRETTY_FUNCTION__

#define vmin(a,b) ((a) < (b) ? (a) : (b))
#define vmax(a,b) ((a) > (b) ? (a) : (b))

using std::string;
using std::vector;

//*************************************************
//* TSYS					  *
//*************************************************
class TSYS : public TCntrNode
{

    public:
	//Data
	enum Code	{ PathEl, HttpURL, Html, JavaSc, SQL, Custom, base64, FormatPrint, ID };
	enum IntView	{ Dec, Oct, Hex };
	enum Errors
	{
	    //- DB errors code -
	    DBInit = 1,		//init DB error
	    DBConn,		//connect to DB is error
	    DBInernal,		//internal error
	    DBRequest,		//request to DB is error
	    DBOpen,		//open DB is error
	    DBOpenTable,	//open table error
	    DBClose,		//close DB or table error
	    DBTableEmpty,	//table is empty
	    DBRowNoPresent,	//no present of requested row
	    DBReadOnly		//read only db
	};

	//Public methods
	TSYS( int argi, char ** argb, char **env );
	~TSYS( );

	int  start( );
	void stop( );

	int stopSignal( )	{ return mStopSignal; }

        //- Programms options -
	string id( )		{ return mId; }
	string name( )		{ return mName; }
	string user( )		{ return mUser; }	//Run user name

	void list( vector<string> &list )	{ chldList(mSubst,list); }
	bool present( const string &name )	{ return chldPresent(mSubst,name); }
	void add( TSubSYS *sub )		{ chldAdd(mSubst,sub); }
	void del( const string &name )		{ chldDel(mSubst,name); }
	AutoHD<TSubSYS> at( const string &name ){ return chldAt(mSubst,name); }

	AutoHD<TUIS>		ui( )		{ return at("UI"); }
	AutoHD<TArchiveS>	archive( )	{ return at("Archive"); }
	AutoHD<TBDS>		db( )		{ return at("BD"); }
	AutoHD<TDAQS>		daq( )		{ return at("DAQ"); }
	AutoHD<TProtocolS>	protocol( )	{ return at("Protocol"); }
	AutoHD<TTransportS>	transport( )	{ return at("Transport"); }
	AutoHD<TSpecialS>	special( )	{ return at("Special"); }
	AutoHD<TModSchedul>	modSchedul( )	{ return at("ModSched"); }
	AutoHD<TSecurity>	security( )	{ return at("Security"); }

	string workDir( );
	string icoDir( )	{ return mIcoDir; }
	string modDir( )	{ return mModDir; }
	void setWorkDir( const string &wdir );
	void setIcoDir( const string &idir )	{ mIcoDir = idir; modif(); }
	void setModDir( const string &mdir )	{ mModDir = mdir; modif(); }

	//- Config file functions -
	string cfgFile( )	{ return mConfFile; }
	XMLNode &cfgRoot( )	{ return rootN; }

	string workDB( )	{ return mWorkDB; }
	string selDB( )		{ return mSelDB; }
	bool chkSelDB( const string& wDB );
	void setWorkDB( const string &wdb )	{ mWorkDB = wdb; modifG(); }
	void setSelDB( const string &vl )	{ mSelDB = vl; }
	bool saveAtExit( )	{ return mSaveAtExit; }
	void setSaveAtExit( bool vl )		{ mSaveAtExit = vl; modif(); }
	int  savePeriod( )	{ return mSavePeriod; }
	void setSavePeriod( int vl )		{ mSavePeriod = vl; modif(); }

	//- Get system options from DB -
	bool sysOptCfg( )	{ return mSysOptCfg; }

	string optDescr( );	//print comand line options

	static void sighandler( int signal );

        //- Short time dimensions -
	unsigned long long sysClk( )		{ return mSysclc; }
	void clkCalc( )
	{
	    unsigned long long st_pnt = shrtCnt( );
	    usleep(100000);
	    mSysclc = 10*(shrtCnt( )-st_pnt);
	}
	unsigned long long shrtCnt( )
	{
	    unsigned int cntl, cnth;
	    asm volatile("rdtsc; movl %%eax,%0; movl %%edx,%1;":"=r"(cntl),"=r"(cnth)::"%eax","%edx");
	    return ((unsigned long long)cnth<<32)+cntl;
	}
	static long HZ( );

	//Public system static methods
	//- Current system time (usec) -
	static long long curTime( );

	static void taskSleep( long long per );

	//- Wait event with timeout support -
	static bool eventWait( bool &m_mess_r_stat, bool exempl, const string &loc, time_t time = 0 );

	//- Convert value to string -
	static string int2str( int val, IntView view = Dec );
	static string uint2str( unsigned val, IntView view = Dec );
	static string ll2str( long long val, IntView view = Dec );
	static string real2str( double val, int prec = 15, char tp = 'g' );
	static double realRound( double val, int dig = 0, bool toint = false )
	{
	    double rez = floor(val*pow(10,dig)+0.5)/pow(10,dig);
	    if( toint ) return floor(rez+0.5);
	    return rez;
	}

	//Adress convertors
	static string addr2str( void *addr );
	static void *str2addr( const string &str );

	//- Path and string parse -
	static string fNameFix( const string &fname );
	static string strNoSpace( const string &val );
	static string strSepParse( const string &str, int level, char sep, int *off = NULL );
	static string pathLev( const string &path, int level, bool encode = true, int *off = NULL );
	static string path2sepstr( const string &path, char sep = '.' );
	static string sepstr2path( const string &str, char sep = '.' );
	static string strEncode( const string &in, Code tp, const string &symb = " \t\n");
	static string strDecode( const string &in, Code tp = Custom );
	static string strMess( const char *fmt, ... );

	static string strCompr( const string &in, int lev = -1 );
	static string strUncompr( const string &in );

	//Public attributes
	static bool finalKill;	//Final object's kill flag. For dead requsted resources
	const int argc;		//Comand line seting counter.
	const char **argv;	//Comand line seting buffer.
	const char **envp;	//System environment.

    protected:
	//Protected methods
	void load_( );
	void save_( );

    private:
	//Private methods
	string nodeName( )	{ return id(); }
	bool cfgFileLoad( );
	void cfgPrmLoad( );
	void cfgFileScan( bool first = false );
	void cntrCmdProc( XMLNode *opt );	// Control interface command process

	static unsigned char getBase64Code( unsigned char asymb );

	//Private attributes
	string	mUser,		// A owner user name!
	 	mConfFile,	// Config file name
		mId,		// Station id
		mName,		// Station name
		mIcoDir,	// Icons directory
		mModDir;	// Modules directory

	bool	mSysOptCfg;	// Get system options from config only

	string	mWorkDB,mSelDB;	// Work and selected DB
	bool	mSaveAtExit;	// Save at exit
	int	mSavePeriod;	// Save period (s) for periodic system saving to DB

	XMLNode rootN;		// Root of the config file tree
	string	rootCfgFl;	// Root node's config file name
	time_t	rootFlTm;	// Root node's config file's modify time

	int	mStopSignal,	// Stop station signal
		mSubst;		// Subsystem tree id

	unsigned long long mSysclc;
};

extern TSYS *SYS;

#endif // TSYS_H
