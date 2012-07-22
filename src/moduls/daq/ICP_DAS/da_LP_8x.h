
//OpenSCADA system module DAQ.ICP_DAS file: da_LP_8x.h
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
	void vlSet( TMdPrm *prm, TVal &valo, const TVariant &pvl );

	bool cntrCmdProc( TMdPrm *prm, XMLNode *opt );

    private:
	//Data
        class PrmsI8017
        {
            public:
                PrmsI8017( ) : init(false), prmNum(8), fastPer(0)
                { for( int ip = 0; ip < 8; ip++ ) cnlMode[ip] = 0; }

                bool    init;
                char    prmNum;
                float   fastPer;
                char    cnlMode[8];
        };

	//Methods
	static void *fastTask( void *iprm );
};

} //End namespace

#endif //DA_LP_8X_H
