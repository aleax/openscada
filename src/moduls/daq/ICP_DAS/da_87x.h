
//OpenSCADA system module DAQ.ICP_DAS file: da_87x.h
/***************************************************************************
 *   Copyright (C) 2012-2014 by Roman Savochenko, <rom_as@oscada.org>      *
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

#ifndef DA_87X_H
#define DA_87X_H

#include "da.h"

namespace ICP_DAS_DAQ
{

//*************************************************
//* da_87x                                         *
//*************************************************
class da_87x: public DA
{
    public:
	//Data
	class DevFeature {
	    public:
		DevFeature( unsigned iAI, unsigned iAO = 0, unsigned iDI = 0, unsigned iDO = 0, unsigned iCNTR = 0 ) :
		    AI(iAI), AO(iAO), DI(iDI), DO(iDO), CNTR(iCNTR)	{ }
		DevFeature( ) : AI(0), AO(0), DI(0), DO(0), CNTR(0)	{ }

		unsigned AI;	//[mode][cnls]	0x0108	mode:0-#AA;1-$AA3,#AA
		unsigned AO;	//[cnls]	2	#AAN(Data),~AA0-5
		unsigned DI;	//[mode][cnls]	0x0002	mode:0-@AA(cnls*8);1-@AADI(cnls*8)
		unsigned DO;	//[mode][cnls]	0x0002	mode:0-@AA(Data)(cnls*8);1-@AADODD(cnls*8)
		unsigned CNTR;	//[cntrs]	8	#AAN

		string	aiTypes;
	};

	class tval {
	    public:
		tval( ) : doVal(0)	{ }

		DevFeature dev;
		uint32_t doVal;	//current values for DO channels
        };

	//Methods
	da_87x( );
	~da_87x( );

	string id( )	{ return "87x"; }
	string name( )	{ return _("I-8700, I-7000 serial bus"); }

	void tpList( TMdPrm *prm, vector<string> &tpl, vector<string> *ntpl = NULL );

	void enable( TMdPrm *prm, vector<string> &als );
	void disable( TMdPrm *prm );

	void getVal( TMdPrm *prm );
	void vlSet( TMdPrm *prm, TVal &vo, const TVariant &vl, const TVariant &pvl );

	DevFeature getDev( TMdPrm *prm, const string &nm );

	bool cntrCmdProc( TMdPrm *prm, XMLNode *opt );


	//Attributes
	map<string, DevFeature> devs;
};

} //End namespace

#endif //DA_87X_H
