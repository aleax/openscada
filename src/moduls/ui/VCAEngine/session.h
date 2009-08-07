
//OpenSCADA system module UI.VCAEngine file: session.h
/***************************************************************************
 *   Copyright (C) 2007-2008 by Roman Savochenko                           *
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

#ifndef SESSION_H
#define SESSION_H

#include <stdint.h>

#include <tcntrnode.h>
#include <tconfig.h>

#include "widget.h"

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

	string ico( );
	const string &id( )	{ return mId; }			//Identifier
	string projNm( )	{ return mPrjnm; }		//Project's name
	string user( )		{ return mUser; }		//Open session user
	string owner( )		{ return mOwner; }		//Source project owner
	string grp( )		{ return mGrp; }		//Source project group
	short  permit( )	{ return mPermit; }		//Permition for access to source project
	int    period( )	{ return vmax(1,mPer); }	//Process period (ms)
	double calcTm( )	{ return tm_calc; }		//Calc session time
	bool   enable( )	{ return mEnable; }		//Enable stat
	bool   start( )		{ return mStart; }		//Start stat
	bool   backgrnd( )	{ return mBackgrnd; }		//Background session execution
	int    connects( )	{ return mConnects; }		//Connections counter
	unsigned calcClk( )	{ return mCalcClk; }		//Calc clock
	AutoHD<Project> parent( );

	void setProjNm( const string &it )	{ mPrjnm = it; }
	void setUser( const string &it );
	void setPeriod( int val )		{ mPer = val; }
	void setEnable( bool val );
	void setStart( bool val );
	void setBackgrnd( bool val )		{ mBackgrnd = val; }
	void connect( )				{ mConnects++; }
	void disconnect( )			{ if(mConnects>0) mConnects--; }

	//- Pages -
	void list( vector<string> &ls ) 	{ chldList(mPage,ls); }
	bool present( const string &id )	{ return chldPresent(mPage,id); }
	AutoHD<SessPage> at( const string &id );
	void add( const string &id, const string &parent = "" );
	void del( const string &id, bool full = false )	{ chldDel(mPage,id,-1,full); }

	vector<string> &openList( )		{ return mOpen; }
	void openReg( const string &id );
	void openUnreg( const string &id );

	Res &eventRes( )			{ return mEvRes; }

	void uiComm( const string &com, const string &prm, SessWdg *src = NULL );

	//- Alarms process -
	void alarmSet( const string &wpath, const string &alrm );	//Alarm set
	int  alarmStat( );						//Alarm status
	void alarmQuittance( const string &wpath, uint8_t quit_tmpl );	//Alarm quittance send

	//Attributes
	AutoHD<TSecurity> sec;

    protected:
	//Methods
	string nodeName( )	{ return mId; }
	void cntrCmdProc( XMLNode *opt );				//Control interface command process

	void postEnable( int flag );
	void preDisable( int flag );

    private:
	//Data
	class Alarm
	{
	    public:
		//Methods
		Alarm( const string &ipath, uint8_t ilev, uint8_t itp, const string &icat, const string &imess, const string &itpArg = "", unsigned iclc = 0 ) :
		    path(ipath), lev(ilev), tp(itp), cat(icat), mess(imess), tpArg(itpArg), clc(iclc)	{ }
		Alarm( const string &path, const string &alrm, unsigned clc );
		Alarm( ) : lev(0), tp(0), qtp(0)	{ }

		//Attributes
		uint8_t	    lev,	//Level
			    tp,		//Type
			    qtp;	//Quitance type
		string	    path,	//Widget path
			    cat,	//Category
			    mess,	//Message
			    tpArg;	//Type argument
		unsigned    clc;	//Clock
	};

	//Methods
	static void *Task( void *contr );

	//Attributes
	int	mPage;
	string	mId, mPrjnm, mUser, mOwner, mGrp;
	int	mPer, mPermit;
	bool	mEnable, mStart, endrun_req;	//Enabled, Started and endrun stats
	bool	mBackgrnd;			//Backgrounded execution of a session
	int	mConnects;			//Connections counter

	pthread_t	calcPthr;		//Calc pthread
	unsigned	mCalcClk;		//Calc clock
	float		tm_calc;		//Scheme's calc time
	float		rez_calc;
	AutoHD<Project>	mParent;
	Res		mCalcRes;		//Calc resource
	Res		mEvRes;			//Event access resource

	vector<string>	mOpen;

	Res		mAlrmRes;		//Alarms resource
	vector<Alarm>	mAlrm;			//Alarms queue
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

	//> Main parameters
	string path( );
	string ownerFullId( bool contr = false );
	string type( )		{ return "SessWidget"; }
	string ico( );
	string owner( );
	string grp( );
	short  permit( );
	string calcLang( );
	string calcProg( );
	int    calcPer( );
	bool   process( )	{ return mProc; }		//Process stat

	void setEnable( bool val );
	virtual void setProcess( bool val );

	virtual void prcElListUpdate( );
	virtual void calc( bool first, bool last );
	void getUpdtWdg( const string &path, unsigned int tm, vector<string> &els );

	//> Include widgets
	void wdgAdd( const string &wid, const string &name, const string &parent, bool force = false );	//Implicit widget's creating on inherit
	AutoHD<SessWdg> wdgAt( const string &wdg );

	//> Events process
	void eventAdd( const string &ev );
	string eventGet( bool clear = false );

	//> Alarms process
	virtual void alarmSet( bool isSet = false );
	virtual void alarmQuittance( uint8_t quit_tmpl, bool isSet = false );

	//> Access to mime resource
	string resourceGet( const string &id, string *mime = NULL );

	SessWdg  *ownerSessWdg( bool base = false );
	SessPage *ownerPage( );
	Session  *ownerSess( )	{ return mSess; }

	void inheritAttr( const string &attr = "" );

    protected:
	//Methods
	void postEnable( int flag );

	bool cntrCmdServ( XMLNode *opt );
	bool cntrCmdGeneric( XMLNode *opt );
	bool cntrCmdAttributes( XMLNode *opt );
	void cntrCmdProc( XMLNode *opt );			//Control interface command process
	bool attrChange( Attr &cfg, TVariant prev );

	unsigned int modifVal( Attr &cfg );
	bool modifChk( unsigned int tm );

    private:
	//Attributes
	char		mProc	: 1;
	char		inLnkGet: 1;
	
	string		mWorkProg;
	unsigned int	mMdfClc;
	Res		mCalcRes;

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
	string type( )          { return "SessPage"; }

	void setEnable( bool val );
	void setProcess( bool val );

	void calc( bool first, bool last );

	AutoHD<Page> parent( );

	//- Pages -
	void pageList( vector<string> &ls )			{ chldList(mPage,ls); }
	bool pagePresent( const string &id )			{ return chldPresent(mPage,id); }
	AutoHD<SessPage> pageAt( const string &id );
	void pageAdd( const string &id, const string &parent = "" );
	void pageDel( const string &id, bool full = false )	{ chldDel(mPage,id,-1,full); }

	//- Alarms process -
	void alarmSet( bool isSet = false );
	void alarmQuittance( uint8_t quit_tmpl, bool isSet = false );

    protected:
	//Methods
	bool cntrCmdGeneric( XMLNode *opt );

	bool attrChange( Attr &cfg, TVariant prev );

    private:
	//Attributes
	int	mPage;		//Pages container identifier
};

}

#endif //SESSION_H
