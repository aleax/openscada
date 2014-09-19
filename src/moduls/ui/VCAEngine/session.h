
//OpenSCADA system module UI.VCAEngine file: session.h
/***************************************************************************
 *   Copyright (C) 2007-2014 by Roman Savochenko, <rom_as@oscada.org>      *
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

#ifndef SESSION_H
#define SESSION_H

#include <stdint.h>

#include <tcntrnode.h>
#include <tconfig.h>

#include "widget.h"

#define DIS_SES_TM 30*60

namespace VCA
{

//************************************************
//* Session: VCA session			 *
//************************************************
class SessPage;
class SessWdg;

class Session : public TCntrNode
{
    public:
	//Methods
	Session( const string &id, const string &proj = "" );
	~Session( );

	string	ico( );
	string	id( )		{ return mId.c_str(); }		//Identifier
	string	projNm( )	{ return mPrjnm; }		//Project's name
	string	user( )		{ return mUser; }		//Open session user
	string	owner( )	{ return mOwner; }		//Source project owner
	string	grp( )		{ return mGrp; }		//Source project group
	short	permit( )	{ return mPermit; }		//Permission for access to source project
	int	period( )	{ return vmax(1,mPer); }	//Process period (ms)
	double	calcTm( );					//Calculate session time
	bool	enable( )	{ return mEnable; }		//Enable stat
	bool	start( )	{ return mStart; }		//Start stat
	bool	backgrnd( )	{ return mBackgrnd; }		//Background session execution
	int	connects( )	{ return mConnects; }		//Connections counter
	time_t	reqTm( )	{ return mReqTm; }		//Last request time from client
	unsigned &calcClk( )	{ return mCalcClk; }		//Calc clock
	AutoHD<Project> parent( );
	int stlCurent( )	{ return mStyleIdW; }

	void setProjNm( const string &it )	{ mPrjnm = it; }
	void setUser( const string &it );
	void setPeriod( int val )		{ mPer = val; }
	void setEnable( bool val );
	void setStart( bool val );
	void setBackgrnd( bool val )		{ mBackgrnd = val; }
	void connect( )				{ mConnects++; }
	void disconnect( )			{ if(mConnects>0) mConnects--; }
	void stlCurentSet( int sid );

	bool modifChk( unsigned int tm, unsigned int iMdfClc );

	// Pages
	void list( vector<string> &ls ) 	{ chldList(mPage,ls); }
	bool present( const string &id )	{ return chldPresent(mPage,id); }
	AutoHD<SessPage> at( const string &id );
	void add( const string &id, const string &parent = "" );
	void del( const string &id, bool full = false )	{ chldDel(mPage,id,-1,full); }

	vector<string> openList( );
	void openReg( const string &id );
	void openUnreg( const string &id );

	pthread_mutex_t	&dataMtx( )		{ return dataM; }

	void uiComm( const string &com, const string &prm, SessWdg *src = NULL );

	string sessAttr( const string &idw, const string &id, bool onlyAllow = false );
	void sessAttrSet( const string &idw, const string &id, const string &val );

	// Alarms process
	void alarmSet( const string &wpath, const string &alrm );	//Alarm set
	int  alarmStat( );						//Alarm status
	void alarmQuittance( const string &wpath, uint8_t quit_tmpl );	//Alarm quittance send

	// Style
	string stlPropGet( const string &pid, const string &def = "" );
	bool stlPropSet( const string &pid, const string &vl );

	//Attributes
	AutoHD<TSecurity> sec;

    protected:
	//Methods
	const char *nodeName( )		{ return mId.c_str(); }
	void cntrCmdProc( XMLNode *opt );				//Control interface command process

	TVariant objFuncCall( const string &id, vector<TVariant> &prms, const string &user );

	void postEnable( int flag );
	void preDisable( int flag );

    private:
	//Data
	class Alarm
	{
	    public:
		//Methods
		Alarm(const string &ipath, uint8_t ilev, uint8_t itp, const string &icat, const string &imess,
			    const string &itpArg = "", unsigned iclc = 0 ) :
		    lev(ilev), tp(itp), path(ipath), cat(icat), mess(imess), tpArg(itpArg), clc(iclc)	{ }
		Alarm(const string &path, const string &alrm, unsigned clc);
		Alarm( ) : lev(0), tp(0), qtp(0)	{ }

		//Attributes
		uint8_t	    lev,	//Level
			    tp,		//Type
			    qtp;	//Quittance type
		string	    path,	//Widget path
			    cat,	//Category
			    mess,	//Message
			    tpArg;	//Type argument
		unsigned    clc;	//Clock
	};

	//Methods
	static void *Task( void *contr );

	//Attributes
	pthread_mutex_t	dataM,
			mCalcRes;		//Calc resource
	int	mPage;
	const string mId;
	string	mPrjnm, mOwner, mGrp;
	MtxString mUser;
	int	mPer, mPermit;
	bool	mEnable, mStart, endrun_req;	//Enabled, Started and endrun stats
	bool	mBackgrnd;			//Backgrounded execution of a session
	int	mConnects;			//Connections counter

	unsigned	mCalcClk;		//Calc clock
	time_t		mReqTm;
	AutoHD<Project>	mParent;

	vector<string>	mOpen;

	Res		mAlrmRes;		//Alarms resource
	vector<Alarm>	mAlrm;			//Alarms queue
	int		mAlrmSndPlay;		//Now played sound alarm

	// Styles
	int		mStyleIdW;
	map<string,string>	mStProp;	//Styles' properties
};

//************************************************
//* SessWdg: Widget of included to session page  *
//************************************************
class SessWdg : public Widget, public TValFunc
{
    public:
	//Methods
	SessWdg( const string &id, const string &parent, Session *sess );
	~SessWdg( );

	// Main parameters
	string	path( );
	string	ownerFullId( bool contr = false );
	string	type( )		{ return "SessWidget"; }
	string	ico( );
	string	calcLang( );
	string	calcProg( );
	string	calcProgStors( const string &attr = "" );
	int	calcPer( );
	bool	process( )	{ return mProc; }		//Process stat

	void setEnable( bool val );
	virtual void setProcess( bool val, bool lastFirstCalc = true );

	virtual void prcElListUpdate( );
	virtual void calc( bool first, bool last );
	void getUpdtWdg( const string &path, unsigned int tm, vector<string> &els );

	// Include widgets
	void wdgAdd( const string &wid, const string &name, const string &parent, bool force = false );	//Implicit widget's creating on inherit
	AutoHD<Widget> wdgAt( const string &wdg, int lev = -1, int off = 0 );
	void pgClose( );

	string sessAttr( const string &id, bool onlyAllow = false );
	void sessAttrSet( const string &id, const string &val );

	// Events process
	void eventAdd( const string &ev );
	string eventGet( bool clear = false );

	// Alarms process
	virtual void alarmSet( bool isSet = false );
	virtual void alarmQuittance( uint8_t quit_tmpl, bool isSet = false );

	// Access to mime resource
	string resourceGet( const string &id, string *mime = NULL );
	void resourceSet( const string &id, const string &data, const string &mime = "" );

	SessWdg  *ownerSessWdg( bool base = false );
	SessPage *ownerPage( );
	Session  *ownerSess( )	{ return mSess; }

	void inheritAttr( const string &attr = "" );

	void attrAdd( TFld *attr, int pos = -1, bool inher = false, bool forceMdf = false );

    protected:
	//Methods
	void postEnable( int flag );
	void preDisable( int flag );

	bool cntrCmdServ( XMLNode *opt );
	bool cntrCmdGeneric( XMLNode *opt );
	bool cntrCmdAttributes( XMLNode *opt, Widget *src = NULL );
	void cntrCmdProc( XMLNode *opt );			//Control interface command process
	bool attrChange( Attr &cfg, TVariant prev );

	TVariant objFuncCall( const string &id, vector<TVariant> &prms, const string &user );

	unsigned int modifVal( Attr &cfg );
	bool modifChk( unsigned int tm, unsigned int iMdfClc );

	//Attributes
	unsigned	mProc	: 1;
	unsigned	inLnkGet: 1;
	unsigned	mToEn	: 1;

    private:
	//Attributes
	string		mWorkProg;
	unsigned int	mMdfClc;
	unsigned int	&mCalcClk;
	pthread_mutex_t	mCalcRes;

	vector<string>	mWdgChldAct,	//Active childs widget's list
			mAttrLnkLs;	//Linked attributes list

	Session		*mSess;
};

//************************************************
//* SessPage: Session's page			 *
//************************************************
class SessPage : public SessWdg
{
    public:
	//Methods
	SessPage( const string &id, const string &page, Session *sess );
	~SessPage( );

	string path( );
	string type( )		{ return "SessPage"; }

	void setEnable( bool val, bool force = false );
	void setProcess( bool val, bool lastFirstCalc = true );

	void calc( bool first, bool last );

	AutoHD<Page> parent( );

	// Pages
	void pageList( vector<string> &ls )			{ chldList(mPage,ls); }
	bool pagePresent( const string &id )			{ return chldPresent(mPage,id); }
	AutoHD<SessPage> pageAt( const string &id );
	void pageAdd( const string &id, const string &parent = "" );
	void pageDel( const string &id, bool full = false )	{ chldDel(mPage,id,-1,full); }

	AutoHD<Widget> wdgAt( const string &wdg, int lev = -1, int off = 0 );

	// Alarms process
	void alarmSet( bool isSet = false );
	void alarmQuittance( uint8_t quit_tmpl, bool isSet = false );

	bool attrPresent( const string &attr );
	AutoHD<Attr> attrAt( const string &attr, int lev = -1 );

    protected:
	//Methods
	bool cntrCmdGeneric( XMLNode *opt );

	bool attrChange( Attr &cfg, TVariant prev );
	TVariant stlReq( Attr &a, const TVariant &vl, bool wr );

    private:
	//Attributes
	unsigned mPage		: 4;		//Pages container identifier
	unsigned mClosePgCom	: 1;
};

}

#endif //SESSION_H
