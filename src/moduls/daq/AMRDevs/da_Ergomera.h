
//OpenSCADA system module DAQ.AMRDevs file: da_Ergomera.h
/***************************************************************************
 *   Copyright (C) 2010-2014 by Roman Savochenko, <rom_as@oscada.org>      *
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

#ifndef DA_ERGOMERA_H
#define DA_ERGOMERA_H

#include "da.h"

#define MaxLenReq 200

namespace AMRDevs
{

//*************************************************
//* Ergomera                                      *
//*************************************************
class Ergomera: public DA
{
    public:
	//Methods
	Ergomera( TMdPrm *prm );
	~Ergomera( );

	void getVals( );

	bool cntrCmdProc( XMLNode *opt );

    private:
	//Data
	class SDataRec
	{
	    public:
		SDataRec( int ioff, int v_rez ) : off(ioff)
		{ val.assign(v_rez,0); err.setVal(_("11:Value not gathered.")); }

		int	off;			//Data block start offset
		string	val;			//Data block values kadr
		ResString	err;		//Acquisition error text
	};

	//Methods
	void regVal( int reg );			//Register value for acquisition
	string modBusReq( string &pdu );
	int64_t getValR( int addr, ResString &err );

	//Attributes
	int	devAddr;			//Device address
	string	mAttrs;
	bool	mMerge;
	vector<SDataRec>	acqBlks;	//Acquisition data blocks for registers
	float	numReg;
};

} //End namespace

#endif //DA_ERGOMERA_H
