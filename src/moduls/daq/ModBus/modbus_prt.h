
//OpenSCADA system module Protocol.ModBus file: modbus_prt.h
/***************************************************************************
 *   Copyright (C) 2009 by Roman Savochenko                                *
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

#ifndef MODBUS_PRT_H
#define MODBUS_PRT_H

#include <string>
#include <map>

#include <tprotocols.h>

using std::string;
using std::map;

//*************************************************
//* Protocol modul info!                          *
#define PRT_ID		"ModBus"
#define PRT_NAME	"ModBus"
#define PRT_TYPE	"Protocol"
#define PRT_SUBVER	VER_PROT
#define PRT_MVER	"0.1.0"
#define PRT_AUTORS	"Roman Savochenko"
#define PRT_DESCR	"Allow realisation of ModBus protocols. Supported Modbus/TCP, Modbus/RTU and Modbus/ASCII protocols."
#define PRT_LICENSE	"GPL"
//*************************************************

namespace ModBus
{

//*************************************************
//* TProtIn                                       *
//*************************************************
class TProtIn: public TProtocolIn
{
    public:
	//Methods
	TProtIn( string name );
	~TProtIn( );

	bool mess( const string &request, string &answer, const string &sender );
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

	void outMess( XMLNode &io, TTransportOut &tro );

	//> Special modbus protocol's functions
	ui16	CRC16( const string &mbap );
	ui8	LRC( const string &mbap );
	string	DataToASCII( const string &in );
	string	ASCIIToData( const string &in );

	//> Protocol
	int prtLen( )	{ return mPrtLen; }
	void setPrtLen( int vl );
	void pushPrtMess( const string &vl );

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
	static ui8 CRCHi[];
	static ui8 CRCLo[];

	//Methods
	string optDescr( );
	TProtocolIn *in_open( const string &name );

	void cntrCmdProc( XMLNode *opt );	//Control interface command process

};

extern TProt *modPrt;
} //End namespace ModBus

#endif //MODBUS_PRT_H
