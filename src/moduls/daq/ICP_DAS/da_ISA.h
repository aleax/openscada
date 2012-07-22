
//OpenSCADA system module DAQ.ICP_DAS file: da_ISA.h
/***************************************************************************
 *   Copyright (C) 2012 by Roman Savochenko                                *
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
	void vlSet( TMdPrm *prm, TVal &valo, const TVariant &pvl );

	bool cntrCmdProc( TMdPrm *prm, XMLNode *opt );

    private:
        //Data
        class DevFeature
        {
	    public:
		DevFeature( unsigned iDIO ) : dio(iDIO)	{ }
		DevFeature( ) : dio(0)	{ }

		unsigned dio;	//[cnls][prts]	0x0603
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
