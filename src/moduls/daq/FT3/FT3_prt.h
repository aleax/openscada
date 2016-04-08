//OpenSCADA system module Protocol.FT3 file: FT3_prt.h
/***************************************************************************
 *   Copyright (C) 2011-2015 by Maxim Kochetkov                            *
 *   fido_max@inbox.ru                                                     *
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

#ifndef FT3_PRT_H
#define FT3_PRT_H

#include <stdint.h>
#include <stdarg.h>

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
#define PRT_ID		"FT3"
#define PRT_NAME	_("FT3")
#define PRT_TYPE	SPRT_ID
#define PRT_SUBVER	SPRT_VER
#define PRT_MVER	"0.2.1"
#define PRT_AUTORS	_("Maxim Kochetkov")
#define PRT_DESCR	_("Allow realisation of FT3 protocols.")
#define PRT_LICENSE	"GPL2"
//*************************************************

namespace FT3
{

//*************************************************
//* TProtIn                                       *
//*************************************************
    class TProt;
//class NodeBlock;

    class TProtIn: public TProtocolIn
    {
    public:
	//Methods
	TProtIn(string name);
	~TProtIn();

	bool mess(const string &request, string &answer/*, const string &sender */);

	TProt &owner();

    public:
	//Attributes
	string req_buf;
    };

//*************************************************
//* TProt                                         *
//*************************************************
    class TProt: public TProtocol
    {
    public:
	//Methods
	//!!! Constructor for root module object.
	TProt(string name);
	//!!! Destructor for root module object.
	~TProt();
	//> Special FT3 protocol's functions
	uint16_t CRC(const char *data, uint16_t length);
	void MakePacket(string &pdu, tagMsg * msg);
	bool VerCRC(string &pdu, int l);
	uint16_t VerifyPacket(string &pdu);
	uint16_t ParsePacket(string &pdu, tagMsg * msg);
	uint16_t Len(uint8_t l);

    protected:
	//Methods
	//!!! Inherited (virtual) load and save object's node methods. Call from OpenSCADA kernel.
	void load_();
	void save_();

    private:
	//Methods
	//!!! Module's comandline options for print help function.
	string optDescr();
	//!!! Main subsystem API function for self modules input protocol object creation.
	TProtocolIn *in_open(const string &name);

	//!!! OpenSCADA control interface comands process virtual function.
	void cntrCmdProc(XMLNode *opt);	//Control interface command process


    };

    extern TProt *modPrt;
} //End namespace FT3

#endif //FT3_PRT_H
