
//OpenSCADA system module DAQ.ICP_DAS file: da_LP_8x.h
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

#ifndef DA_LP_8X_H
#define DA_LP_8X_H

#include "da.h"

namespace ICP_DAS_DAQ
{

//*************************************************
//* da_LP_8x                                         *
//*************************************************
class da_LP_8x: public DA
{
    public:
	//Methods
	da_LP_8x( );
	~da_LP_8x( );

	string id( )	{ return "LP_8x"; }
	string name( )	{ return _("LP-8000, I-8000 parallel bus"); }

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
		DevFeature( unsigned iDI, unsigned iDO = 0 ) : DI(iDI), DO(iDO)	{ }
		DevFeature( ) : DI(0), DO(0)	{ }

		unsigned DI;    //[mode][cnls]	0x0002  cnls*8	mode:0-DI_8(32);1-DIO_DI_8(16)
		unsigned DO;    //[mode][cnls]  0x0002  cnls*8  mode:0-DO_8(32);1-DIO_DO_8(16)
	};

	class tval
	{
	    public:
		tval( ) : init(false), prmNum(8), fastPer(0), doVal(0)
		{ for(int ip = 0; ip < 16; ip++) cnlMode[ip] = 0; }

		DevFeature dev;

		//> I-8017
		bool	init;
		char	prmNum;
		float	fastPer;
		char	cnlMode[16];

		uint32_t doVal; //current values for DO channels
	};

	//Methods
	static void *fastTask( void *iprm );

	//Attributes
	map<string, DevFeature> devs;
};

} //End namespace

#endif //DA_LP_8X_H
