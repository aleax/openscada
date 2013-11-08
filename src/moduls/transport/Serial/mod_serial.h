
//OpenSCADA system module Transport.Serial file: mod_serial.h
/***************************************************************************
 *   Copyright (C) 2009-2010 by Roman Savochenko                           *
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

#ifndef MOD_SERIAL_H
#define MOD_SERIAL_H

#include <pthread.h>

#include <ttransports.h>

#undef _
#define _(mess) mod->I18N(mess)

using namespace OSCADA;

namespace Serial
{

//************************************************
//* Serial::TTrIn				 *
//************************************************
class TTrIn: public TTransportIn
{
    public:
	TTrIn( string name, const string &idb, TElem *el );
	~TTrIn( );

	string getStatus( );

	string timings( )			{ return mTimings; }

	void setAddr( const string &addr );
	void setTimings( const string &vl );

	//> Modem functions
	int	mdmTm( )			{ return mMdmTm; }
	int	taskPrior( )			{ return mTaskPrior; }
	float	mdmPreInit( )			{ return mMdmPreInit; }
	float	mdmPostInit( )			{ return mMdmPostInit; }
	string	mdmInitStr1( )			{ return mMdmInitStr1; }
	string	mdmInitStr2( )			{ return mMdmInitStr2; }
	string	mdmInitResp( )			{ return mMdmInitResp; }
	string	mdmRingReq( )			{ return mMdmRingReq; }
	string	mdmRingAnswer( )		{ return mMdmRingAnswer; }
	string	mdmRingAnswerResp( )		{ return mMdmRingAnswerResp; }

	void	setTaskPrior( int vl )		{ mTaskPrior = vmax(-1,vmin(99,vl)); modif(); }
	void	setMdmTm( int vl )		{ mMdmTm = vmax(1,vmin(120,vl)); modif(); }
	void	setMdmPreInit( float vl )	{ mMdmPreInit = vmax(0,vmin(3,vl)); modif(); }
	void	setMdmPostInit( float vl )	{ mMdmPostInit = vmax(0.01,vmin(3,vl)); modif(); }
	void	setMdmInitStr1( const string &vl )	{ mMdmInitStr1 = vl; modif(); }
	void	setMdmInitStr2( const string &vl )	{ mMdmInitStr2 = vl; modif(); }
	void	setMdmInitResp( const string &vl )	{ mMdmInitResp = vl; modif(); }
	void	setMdmRingReq( const string &vl )	{ mMdmRingReq = vl; modif(); }
	void	setMdmRingAnswer( const string &vl )	{ mMdmRingAnswer = vl; modif(); }
	void	setMdmRingAnswerResp( const string &vl ){ mMdmRingAnswerResp = vl; modif(); }

	void start( );
	void stop( );

    protected:
	//Methods
	void load_( );
	void save_( );

    private:
	//Methods
	void connect( );
	static void *Task( void * );

	void cntrCmdProc( XMLNode *opt );	//Control interface command process

	//Attributes
	string	mTimings, mDevPort;
	int	fd;

	bool	endrun;				// Command for stop task

	uint64_t trIn, trOut;			// Traffic in and out counter
	float	tmMax;

	//> Modem properties
	int	mTaskPrior;			// Requests processing task prioritet
	int	mMdmTm;
	float	mMdmPreInit, mMdmPostInit;
	string	mMdmInitStr1, mMdmInitStr2, mMdmInitResp;
	string	mMdmRingReq, mMdmRingAnswer, mMdmRingAnswerResp;
	unsigned mMdmMode	:1;
	unsigned mMdmDataMode	:1;
	unsigned mRTSfc         :1;
};

//************************************************
//* Serial::TTrOut				 *
//************************************************
class TTrOut: public TTransportOut
{
    public:
	TTrOut( string name, const string &idb, TElem *el );
	~TTrOut( );

	string getStatus( );

	string timings( )			{ return mTimings; }

	void setAddr( const string &addr );
	void setTimings( const string &vl );

	//> Modem functions
	int	mdmTm( )			{ return mMdmTm; }
	int	mdmLifeTime( )			{ return mMdmLifeTime; }
	float	mdmPreInit( )			{ return mMdmPreInit; }
	float	mdmPostInit( )			{ return mMdmPostInit; }
	string	mdmInitStr1( )			{ return mMdmInitStr1; }
	string	mdmInitStr2( )			{ return mMdmInitStr2; }
	string	mdmInitResp( )			{ return mMdmInitResp; }
	string	mdmDialStr( )			{ return mMdmDialStr; }
	string	mdmCnctResp( )			{ return mMdmCnctResp; }
	string	mdmBusyResp( )			{ return mMdmBusyResp; }
	string	mdmNoCarResp( )			{ return mMdmNoCarResp; }
	string	mdmNoDialToneResp( )		{ return mMdmNoDialToneResp; }
	string  mdmExit( )			{ return mMdmExit; }
	string	mdmHangUp( )			{ return mMdmHangUp; }
	string	mdmHangUpResp( )		{ return mMdmHangUpResp; }

	void	setMdmTm( int vl )		{ mMdmTm = vmax(1,vmin(120,vl)); modif(); }
	void	setMdmLifeTime( int vl )	{ mMdmLifeTime = vmax(0,vmin(120,vl)); modif(); }
	void	setMdmPreInit( float vl )	{ mMdmPreInit = vmax(0,vmin(3,vl)); modif(); }
	void	setMdmPostInit( float vl )	{ mMdmPostInit = vmax(0.01,vmin(3,vl)); modif(); }
	void	setMdmInitStr1( const string &vl )	{ mMdmInitStr1 = vl; modif(); }
	void	setMdmInitStr2( const string &vl )	{ mMdmInitStr2 = vl; modif(); }
	void	setMdmInitResp( const string &vl )	{ mMdmInitResp = vl; modif(); }
	void	setMdmDialStr( const string &vl )	{ mMdmDialStr = vl; modif(); }
	void	setMdmCnctResp( const string &vl )	{ mMdmCnctResp = vl; modif(); }
	void	setMdmBusyResp( const string &vl )	{ mMdmBusyResp = vl; modif(); }
	void	setMdmNoCarResp( const string &vl )	{ mMdmNoCarResp = vl; modif(); }
	void	setMdmNoDialToneResp( const string &vl ){ mMdmNoDialToneResp = vl; modif(); }
	void	setMdmHangUp( const string &vl )	{ mMdmHangUp = vl; modif(); }
	void	setMdmExit( const string &vl )		{ mMdmExit = vl; modif(); }
	void	setMdmHangUpResp( const string &vl )	{ mMdmHangUpResp = vl; modif(); }

	void start( );
	void stop( );

	void check( );

	int messIO( const char *obuf, int len_ob, char *ibuf = NULL, int len_ib = 0, int time = 0, bool noRes = false );

    protected:
	//Methods
	void load_( );
	void save_( );

	TVariant objFuncCall( const string &id, vector<TVariant> &prms, const string &user );

    private:
	//Methods
	void cntrCmdProc( XMLNode *opt );	//Control interface command process

	//Attributes
	string	mDevPort, mTimings;
	int	fd;
	int64_t	mLstReqTm, mKeepAliveLstTm;

	uint64_t trIn, trOut;			// Traffic in and out counter and maximum respond timeout

	//> Modem properties
	int	mMdmTm, mMdmLifeTime;
	float	mMdmPreInit, mMdmPostInit;
	string	mMdmInitStr1, mMdmInitStr2, mMdmInitResp;
	string	mMdmDialStr, mMdmCnctResp, mMdmBusyResp, mMdmNoCarResp, mMdmNoDialToneResp;
	string	mMdmExit, mMdmHangUp, mMdmHangUpResp;
	unsigned mMdmMode	:1;
	unsigned mMdmDataMode	:1;
	unsigned mRTSfc		:1;	//Flow control by RTS signal for pure RS-485
};

//************************************************
//* Serial::TTr					*
//************************************************
class TTr: public TTipTransport
{
    public:
	TTr( string name );
	~TTr( );

	AutoHD<TTrIn> inAt( const string &name );
	AutoHD<TTrOut> outAt( const string &name );

	TTransportIn  *In( const string &name, const string &idb );
	TTransportOut *Out( const string &name, const string &idb );

	static void writeLine( int fd, const string &ln, bool noNewLn = false );
	static string expect( int fd, const string& expLst, int tm );

	bool devLock( const string &dn, bool check = false );
	void devUnLock( const string &dn );

	void perSYSCall( unsigned int cnt );

    protected:
	void load_( );

    private:
	//Methods
	void postEnable( int flag );

	//Attributes
	map<string,bool> mDevLock;
};

extern TTr *mod;
}

#endif //MOD_SERIAL_H
