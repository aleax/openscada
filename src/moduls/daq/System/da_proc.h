
//OpenSCADA module DAQ.System file: da_proc.h
/***************************************************************************
 *   Copyright (C) 2024 by Roman Savochenko, <roman@oscada.org>            *
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

#ifndef DA_PROC_H
#define DA_PROC_H

#include "da.h"

#define SELF_PROC	"<SELF>"
#define PIDS_CACHE_UPD_TM 60

namespace SystemCntr
{

//*************************************************
//* Proc                                          *
//*************************************************
class Proc: public DA
{
    public:
	//Data
	// Cache object of the processed PIDs
	struct tval {
	    tval( ) : tmUpd(0)	{ }

	    time_t	tmUpd;
	    vector<int>	prcCache;
	};

	//Methods
	Proc( );

	string id( )	{ return "proc"; }
	string name( )	{ return _("Process"); }

	void init( TMdPrm *prm, bool update = false );
	void deInit( TMdPrm *prm );

	void cfgChange( TMdPrm *prm, TCfg &co, const TVariant &pc );

	void getVal( TMdPrm *prm );

	void dList( vector<string> &list, TMdPrm *prm = NULL );
	void makeActiveDA( TMdContr *aCntr, const string &dIdPref = "", const string &dNmPref = "" )
	{ }						//!!!! Don't create automatically
	//{ DA::makeActiveDA(aCntr, id(), name()); }

    private:
	//Methods
	string devRead( const string &file );
};

} //End namespace

#endif //DA_PROC_H
