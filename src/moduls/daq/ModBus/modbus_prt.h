
//OpenSCADA module Protocol.ModBus file: modbus_prt.h
/***************************************************************************
 *   Copyright (C) 2008-2022 by Roman Savochenko, <roman@oscada.org>       *
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

#ifndef MODBUS_PRT_H
#define MODBUS_PRT_H

#include <stdint.h>

#include <string>
#include <map>

#include <tprotocols.h>

#undef _
#define _(mess) modPrt->I18N(mess).c_str()
#undef trS
#define trS(mess) modPrt->I18N(mess,mess_PreSave)

using std::string;
using std::map;
using namespace OSCADA;

//*************************************************
//* Protocol modul info!                          *
#define PRT_ID		"ModBus"
#define PRT_NAME	trS("Server ModBus")
#define PRT_TYPE	SPRT_ID
#define PRT_SUBVER	SPRT_VER
#define PRT_MVER	"2.9.8"
#define PRT_AUTHORS	trS("Roman Savochenko")
#define PRT_DESCR	trS("Provides implementation of ModBus protocols. ModBus/TCP, ModBus/RTU and ModBus/ASCII protocols are supported.")
#define PRT_LICENSE	"GPL2"
//*************************************************

#define MODBUS_FRM_LIM	1000

namespace ModBus
{

//*************************************************
//* TProtIn                                       *
//*************************************************
class TProt;

class TProtIn: public TProtocolIn
{
    public:
	//Methods
	TProtIn( string name );
	~TProtIn( );

	bool mess( const string &request, string &answer );

	TProt &owner( ) const;

    public:
	//Attributes
	string req_buf;
};

//*************************************************
//* Node: ModBus input protocol node.             *
//*************************************************
class Node : public TFunction, public TConfig
{
    public:
	// Addition flags for IO
	enum IONodeFlgs {
	    IsLink	= 0x10,	//Link to subsystem's "DAQ" data, for compatibility the old configurations
	};
	// Node modes
	enum NodeModes {
	    MD_DATA	= 0,	//Data
	    MD_GT_ND	= 1,	//Gate node
	    MD_GT_NET	= 2	//Gate network
	};

	//Methods
	Node( const string &iid, const string &db, TElem *el );
	~Node( );

	TCntrNode &operator=( const TCntrNode &node );

	string id( )		{ return mId; }
	string name( );
	string descr( )		{ return mDscr; }
	string stor( ) const	{ return DB(); }
	bool toEnable( )	{ return mAEn; }
	bool enableStat( ) const{ return mEn; }
	int addr( ) const;
	string inTransport( );
	string prt( );
	NodeModes mode( );
	int64_t timeStamp( )	{ return mTimeStamp; }

	double period( )	{ return mPer; }
	string progLang( );
	bool progTr( )		{ return cfg("DT_PR_TR"); }
	string prog( );

	string getStatus( );

	string DB( bool qTop = false ) const	{ return storage(mDB, qTop); }
	string tbl( ) const;
	string fullDB( bool qTop = false ) const{ return DB(qTop)+'.'+tbl(); }

	void setName( const string &name )	{ mName = name; }
	void setDescr( const string &idsc )	{ mDscr = idsc; }
	void setToEnable( bool vl )		{ mAEn = vl; modif(); }
	void setEnable( bool vl );
	void setProgLang( const string &ilng );
	void setProgTr( bool vl )		{ cfg("DT_PR_TR") = vl; }
	void setProg( const string &iprg );

	void setDB( const string &vl, bool qTop = false )	{ setStorage(mDB, vl, qTop); if(!qTop) modifG(); }

	bool req( const string &tr, const string &prt, unsigned char node, string &pdu );

	TProt &owner( ) const;

    protected:
	//Methods
	void load_( TConfig *cfg );
	void save_( );

    private:
	//Data
	class SIO {
	    public:
		SIO( ) : id(-1), pos(-1), sTp(0)	{ }
		SIO( int iid, char isTp = 0, int ipos = 0 ) : id(iid), pos(ipos), sTp(isTp)	{ }

		int id, pos;
		char sTp;
	};
	class SData : public TPrmTempl::Impl {
	    public:
		SData( TCntrNode *iobj, string name_c ) : TPrmTempl::Impl(iobj, name_c.c_str()),
		    rReg(0), wReg(0), rCoil(0), wCoil(0), rCoilI(0), rRegI(0), chkLnkNeed(false) { }

		map<int,SIO> regR, regW, coilR, coilW, coilI, regI;
		float	rReg, wReg, rCoil, wCoil, rCoilI, rRegI;
		bool	chkLnkNeed;	//Check lnk need flag
	};

	//Methods
	const char *nodeName( ) const	{ return mId.getSd(); }

	void loadIO( );
	void saveIO( );

	void cntrCmdProc( XMLNode *opt );	//Control interface command process

	void postEnable( int flag );
	void postDisable( int flag );		//Delete all DB if flag 1
	bool cfgChange( TCfg &cfg, const TVariant &pc );
	void regCR( int id, const SIO &val, const string &tp, bool wr = false );

	static void *Task( void *icntr );

	//Attributes
	ResRW	nRes;
	SData	*data;
	bool	isDAQTmpl;	// DAQ template used
	TCfg	&mId, &mName, &mDscr;
	double	&mPer;
	char	&mAEn, mEn;
	int64_t	&mTimeStamp;
	string	mDB;

	bool	prcSt, endRun;

	float	cntReq;
};

//*************************************************
//* TProt                                         *
//*************************************************
class TProt: public TProtocol
{
    public:
	//Data
	enum ModBusFunc	{
	    FC_MULT_COILS	= 0x01,
	    FC_MULT_COILS_IN	= 0x02,
	    FC_MULT_REGS	= 0x03,
	    FC_MULT_REGS_IN	= 0x04,
	    FC_SING_COILS_WR	= 0x05,
	    FC_SING_REGS_WR	= 0x06,
	    FC_MULT_COILS_WR	= 0x0F,
	    FC_MULT_REGS_WR	= 0x10
	};

	//Methods
	TProt( string name );
	~TProt( );

	void modStart( );
	void modStop( );

	// Node's functions
	void nList( vector<string> &ls ) const		{ chldList(mNode, ls); }
	bool nPresent( const string &id ) const		{ return chldPresent(mNode, id); }
	string nAdd( const string &id, const string &db = "*.*" );
	void nDel( const string &id )			{ chldDel(mNode, id); }
	AutoHD<Node> nAt( const string &id ) const	{ return chldAt(mNode, id); }

	void outMess( XMLNode &io, TTransportOut &tro );

	// Special modbus protocol's functions
	uint16_t CRC16( const string &mbap );
	uint8_t	LRC( const string &mbap );
	string	DataToASCII( const string &in );
	string	ASCIIToData( const string &in );

	// Protocol
	int prtLen( )		{ return mPrtLen; }
	void setPrtLen( int vl );
	void pushPrtMess( const string &vl );

	TElem &nodeEl( )	{ return mNodeEl; }
	TElem &nodeIOEl( )	{ return mNodeIOEl; }

    protected:
	//Methods
	void load_( );
	void save_( );

    private:
	//Attribute
	// Protocol
	int	mPrtLen;
	deque<string>	mPrt;

	// Special modbus protocol's attributes
	static uint8_t CRCHi[];
	static uint8_t CRCLo[];

	//Methods
	TProtocolIn *in_open( const string &name );

	void cntrCmdProc( XMLNode *opt );	//Control interface command process

	//Attributes
	int	mNode;

	TElem	mNodeEl, mNodeIOEl;
};

extern TProt *modPrt;
} //End namespace ModBus

#endif //MODBUS_PRT_H
