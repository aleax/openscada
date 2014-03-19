
//OpenSCADA system module DAQ.ICP_DAS file: da_ISA.h
/***************************************************************************
 *   Copyright (C) 2012-2014 by Roman Savochenko                           *
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

#ifndef DA_ISA_H
#define DA_ISA_H

#include <map>

#include "da.h"

using std::map;

namespace ICP_DAS_DAQ
{

//*************************************************
//* da_ISA                                         *
//*************************************************
class da_ISA: public DA
{
    public:
	//Methods
	da_ISA( );
	~da_ISA( );

	string id( )	{ return "ISA"; }
	string name( )	{ return _("ISA DAQ boards"); }
	string modType( const string &modTp );      //Module type, include dynamic ISA and other processing

	void tpList( TMdPrm *prm, vector<string> &tpl, vector<string> *ntpl = NULL );

	void enable( TMdPrm *prm, vector<string> &als );
	void disable( TMdPrm *prm );

	void getVal( TMdPrm *prm );
	void vlSet( TMdPrm *prm, TVal &vo, const TVariant &vl, const TVariant &pvl );

	bool cntrCmdProc( TMdPrm *prm, XMLNode *opt );

    private:
	//Data
	class DevFeature
	{
	    public:
		DevFeature( unsigned iAI, unsigned iAO = 0, unsigned iDIO = 0, unsigned iDI = 0, unsigned iDO = 0 ) :
		    AI(iAI), AO(iAO), DIO(iDIO), DI(iDI), DO(iDO), aiTm(200)	{ }
		DevFeature( ) : AI(0), AO(0), DIO(0), DI(0), DO(0), aiTm(200)	{ }

		unsigned AI;	//[MCR][cnls]	0x1108
		unsigned AO;	//[cnls]	2
		unsigned DIO;	//[cnls]	6	cnls*24
		unsigned DI;	//[mode][cnls]	0x0002	cnls*8	mode:0-IXISA_DI_A(N);1-IXISA_DIL(H);2-IXISA_DIO_A(N)
		unsigned DO;	//[mode][cnls]	0x0002	cnls*8	mode:0-IXISA_DO_A(N);1-IXISA_DOL(H);2-IXISA_DIO_A(N)

		int aiTm;	//Analog input settle timeout (us)
	};

	class tval
	{
	    public:
		tval( ) : devFd(-1)	{ }

		int	devFd;
		DevFeature dev;
	};

	//Attributes
	map<string, DevFeature>	devs;
};

} //End namespace

#endif //DA_ISA_H
