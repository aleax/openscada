
//OpenSCADA system module Protocol.ModBus file: modbus_prt.h
/***************************************************************************
 *   Copyright (C) 2008-2014 by Roman Savochenko, <rom_as@oscada.org>      *
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
#define _(mess) modPrt->I18N(mess)

using std::string;
using std::map;
using namespace OSCADA;

//*************************************************
//* Protocol modul info!                          *
#define PRT_ID		"ModBus"
#define PRT_NAME	_("ModBUS")
#define PRT_TYPE	SPRT_ID
#define PRT_SUBVER	SPRT_VER
#define PRT_MVER	"0.6.4"
#define PRT_AUTHORS	_("Roman Savochenko")
#define PRT_DESCR	_("Allow realization of ModBus protocols. Supported Modbus/TCP, Modbus/RTU and Modbus/ASCII protocols.")
#define PRT_LICENSE	"GPL2"
//*************************************************

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

	TProt &owner( );

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
	//> Addition flags for IO
	enum IONodeFlgs
	{
	    IsLink	= 0x10,	//Link to subsystem's "DAQ" data
	    LockAttr	= 0x20	//Lock attribute
	};
	//> Node modes
	enum NodeModes
	{
	    MD_DATA = 0,	//Data
	    MD_GT_ND = 1,	//Gate node
	    MD_GT_NET = 2	//Gate network
	};

	//Methods
	Node( const string &iid, const string &db, TElem *el );
	~Node( );

	TCntrNode &operator=( TCntrNode &node );

	string id( )		{ return mId; }
	string name( );
	string descr( )		{ return mDscr; }
	bool toEnable( )	{ return mAEn; }
	bool enableStat( )	{ return mEn; }
	int addr( );
	string inTransport( );
	string prt( );
	NodeModes mode( );

	double period( )	{ return mPer; }
	string progLang( );
	string prog( );

	string getStatus( );

	string DB( )		{ return mDB; }
	string tbl( );
	string fullDB( )	{ return DB()+'.'+tbl(); }

	void setName( const string &name )	{ mName = name; }
	void setDescr( const string &idsc )	{ mDscr = idsc; }
	void setToEnable( bool vl )		{ mAEn = vl; modif(); }
	void setEnable( bool vl );
	void setProgLang( const string &ilng );
	void setProg( const string &iprg );

	void setDB( const string &vl )		{ mDB = vl; modifG(); }

	bool req( const string &tr, const string &prt, unsigned char node, string &pdu );

	TProt &owner( );

    protected:
	//Methods
	void load_( );
	void save_( );

    private:
	//Data
	class SIO
	{
	    public:
		SIO( ) : id(-1), pos(-1), sTp(0)	{ }
		SIO( int iid, char isTp = 0, int ipos = 0 ) : id(iid), pos(ipos), sTp(isTp)	{ }

		int id, pos;
		char sTp;
	};
	class SData
	{
	    public:
		SData( ) : rReg(0), wReg(0), rCoil(0), wCoil(0), rBit(0), rIReg(0)	{ }

		TValFunc	val;
		map<int,AutoHD<TVal> > lnk;
		map<int,SIO> regR, regW, coilR, coilW, bitR, regIR;
		float rReg, wReg, rCoil, wCoil, rBit, rIReg;
	};

	//Methods
	const char *nodeName( )	{ return mId.getSd(); }

	void cntrCmdProc( XMLNode *opt );	//Control interface command process

	void postEnable( int flag );
	void postDisable( int flag );		//Delete all DB if flag 1
	bool cfgChange( TCfg &cfg );
	void regCR( int id, const SIO &val, const string &tp, bool wr = false );

	static void *Task( void *icntr );

	//Attributes
	Res	nRes;
	SData	*data;
	TCfg	&mId, &mName, &mDscr;
	double	&mPer;
	char	&mAEn, mEn;
	string	mDB;

	bool	prcSt, endrunRun;

	float	tmProc, cntReq;
};

//*************************************************
//* TProt                                         *
//*************************************************
class TProt: public TProtocol
{
    public:
	//Methods
	TProt( string name );
	~TProt( );

	void modStart( );
	void modStop( );

	//> Node's functions
	void nList( vector<string> &ls )	{ chldList(mNode,ls); }
	bool nPresent( const string &id )	{ return chldPresent(mNode,id); }
	void nAdd( const string &id, const string &db = "*.*" );
	void nDel( const string &id )		{ chldDel(mNode,id); }
	AutoHD<Node> nAt( const string &id )	{ return chldAt(mNode,id); }

	void outMess( XMLNode &io, TTransportOut &tro );

	//> Special modbus protocol's functions
	uint16_t CRC16( const string &mbap );
	uint8_t	LRC( const string &mbap );
	string	DataToASCII( const string &in );
	string	ASCIIToData( const string &in );

	//> Protocol
	int prtLen( )		{ return mPrtLen; }
	void setPrtLen( int vl );
	void pushPrtMess( const string &vl );

	TElem &nodeEl( )	{ return mNodeEl; }
	TElem &nodeIOEl( )	{ return mNodeIOEl; }

	Res &nodeRes( )		{ return nRes; }

    protected:
	//Methods
	void load_( );
	void save_( );

    private:
	//Attribute
	//> Protocol
	int	mPrtLen;
	deque<string>	mPrt;

	//> Special modbus protocol's attributes
	static uint8_t CRCHi[];
	static uint8_t CRCLo[];

	//Methods
	TProtocolIn *in_open( const string &name );

	void cntrCmdProc( XMLNode *opt );	//Control interface command process

	//Attributes
	int	mNode;

	TElem	mNodeEl, mNodeIOEl;

	Res	nRes;
};

extern TProt *modPrt;
} //End namespace ModBus

#endif //MODBUS_PRT_H
