
//OpenSCADA system module DAQ.OPC_UA file: mod_prt.h
/***************************************************************************
 *   Copyright (C) 2009 by Roman Savochenko                                *
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

#ifndef MOD_PRT_H
#define MOD_PRT_H

#include <stdint.h>

#include <string>
#include <map>

#include <tprotocols.h>

using std::string;
using std::map;

//*************************************************
//* Protocol modul info!                          *
#define PRT_ID		"OPC_UA"
#define PRT_NAME	"OPC UA"
#define PRT_TYPE	"Protocol"
#define PRT_SUBVER	VER_PROT
#define PRT_MVER	"0.0.1"
#define PRT_AUTOR	"Roman Savochenko"
#define PRT_DESCR	"Allow realisation of OPC UA protocol."
#define PRT_LICENSE	"GPL"
//*************************************************

namespace OPC_UA
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

	bool mess( const string &request, string &answer, const string &sender );

	TProt &owner( );

	//> Special
	string mkError( uint32_t errId, const string &err );

    public:
	//Attributes
	bool mNotFull;
	string mBuf;
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

	void outMess( XMLNode &io, TTransportOut &tro );

	//> Protocol's data process
	static int32_t iN( const string &rb, int &off, char vSz );
	static uint32_t iNu( const string &rb, int &off, char vSz );
	static const char *iVal( const string &buf, int &off, char vSz );
	static string iS( const string &buf, int &off );
	static string iSl( const string &buf, int &off, string *locale = NULL );
	static long long iTm( const string &buf, int &off );
	static int iNodeId( const string &buf, int &off, int *ns = NULL );

	static void oN( string &buf, int32_t val, char sz, int off = -1 );
	static void oNu( string &buf, uint32_t val, char sz, int off = -1 );
	static void oS( string &buf, const string &val );
	static void oNodeId( string &buf, int val, int ns = -1 );
	static void oTm( string &buf, long long val );

    protected:
	//Methods
	void load_( );
	void save_( );

    private:
	//Methods
	TProtocolIn *in_open( const string &name );
};

extern TProt *modPrt;

} //End namespace OPC_UA

#endif //MOD_PRT_H
