
//OpenSCADA module DAQ.System file: da_cpu.h
/***************************************************************************
 *   Copyright (C) 2005-2025 by Roman Savochenko, <roman@oscada.org>       *
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

#ifndef DA_CPU_H
#define DA_CPU_H

#include "da.h"

namespace SystemCntr
{

//*************************************************
//* CPU                                           *
//*************************************************
class CPU: public DA
{
    public:
	//Data
	struct tval {
	    tval( long long unsigned iuser = 0, long long unsigned inice = 0, long long unsigned isys = 0, long long unsigned iidle = 0 ) :
		user(iuser), nice(inice), sys(isys), idle(iidle) { }
	    long long unsigned user;
	    long long unsigned nice;
	    long long unsigned sys;
	    long long unsigned idle;
	};

	//Methods
	CPU( );
	~CPU( );

	string id( )	{ return "CPU"; }
	string name( )	{ return _("CPU"); }

	void updGen( bool cntr = false );

	void getVal( TMdPrm *prm );
	void vlGet( TMdPrm *prm, TVal &val );
	void vlSet( TMdPrm *prm, TVal &vo, const TVariant &vl, const TVariant &pvl );

	void dList( vector<string> &list, TMdPrm *prm = NULL );
	void makeActiveDA( TMdContr *aCntr, const string &dIdPref = "", const string &dNmPref = "" )
	{ DA::makeActiveDA(aCntr, id(), name()); }

    private:
	//Methods
	bool devChkAccess( int cpuN, const string &file, const string &acs = "r" );
	string devRead( int cpuN, const string &file );

	//Attributes
#if defined (__i386__) || defined (__x86_64__)
	uint64_t rdtsc;
	time_t   rdtscTm;
#endif

	ResRW	mRes;

	vector<tval>	load;
};

} //End namespace

#endif //DA_CPU_H
