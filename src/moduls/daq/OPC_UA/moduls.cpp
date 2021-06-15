
//OpenSCADA module DAQ.OPC_UA file: moduls.cpp
/***************************************************************************
 *   Copyright (C) 2009-2015 by Roman Savochenko, <roman@oscada.org>       *
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

#include "mod_daq.h"
#include "mod_prt.h"

extern "C"
{
#ifdef MOD_INCL
    TModule::SAt daq_OPC_UA_module( int nMod )
#else
    TModule::SAt module( int nMod )
#endif
    {
	if(nMod == 0)		return TModule::SAt(PRT_ID, PRT_TYPE, PRT_SUBVER);
	else if(nMod == 1)	return TModule::SAt(DAQ_ID, DAQ_TYPE, DAQ_SUBVER);

	return TModule::SAt("");
    }

#ifdef MOD_INCL
    TModule *daq_OPC_UA_attach( const TModule::SAt &AtMod, const string &source )
#else
    TModule *attach( const TModule::SAt &AtMod, const string &source )
#endif
    {
	if(AtMod == TModule::SAt(DAQ_ID,DAQ_TYPE,DAQ_SUBVER))		return new OPC_UA::TTpContr(source);
	else if(AtMod == TModule::SAt(PRT_ID,PRT_TYPE,PRT_SUBVER))	return new OPC_UA::TProt(source);
	return NULL;
    }
}
