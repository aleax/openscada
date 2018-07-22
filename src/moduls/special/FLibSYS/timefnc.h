
//OpenSCADA module Special.FLibSYS file: timefnc.h
/***************************************************************************
 *   Copyright (C) 2005-2016 by Roman Savochenko, <rom_as@oscada.org>      *
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

#ifndef TIMEFNC_H
#define TIMEFNC_H

#include <time.h>

#include <tfunction.h>
#include "statfunc.h"

namespace FLibSYS
{

//*************************************************
//* tmFStr                                        *
//*************************************************
class tmFStr : public TFunction
{
    public:
	tmFStr( ) : TFunction("tmFStr",SSPC_ID) {
	    ioAdd(new IO("val",_("Date and time string"),IO::String,IO::Return,""));
	    ioAdd(new IO("sec",_("Seconds"),IO::Integer,IO::Default,"0"));
	    ioAdd(new IO("form",_("Date and time format"),IO::String,IO::Default,"%Y-%m-%d %H:%M:%S"));
	}

	string name( )	{ return _("Time: String time"); }
	string descr( )	{ return _("Getting the time in the formatted string. Recording of the format corresponds to the POSIX-function \"strftime()\"."); }

	void calc( TValFunc *val ) {
	    time_t tm_t = val->getI(1);
	    struct tm tm_tm;
	    localtime_r(&tm_t,&tm_tm);
	    char buf[1000];
	    int rez = strftime( buf, sizeof(buf), val->getS(2).c_str(), &tm_tm );
	    val->setS(0,(rez>0)?string(buf,rez):"");
	}
};

//*************************************************
//* tmDate                                        *
//*************************************************
class tmDate : public TFunction
{
    public:
	tmDate( ) : TFunction("tmDate",SSPC_ID) {
	    ioAdd(new IO("fullsec",_("Full seconds"),IO::Integer,IO::Default,"0"));
	    ioAdd(new IO("sec",_("Seconds [0...59]"),IO::Integer,IO::Output,"0"));
	    ioAdd(new IO("min",_("Minutes [0...59]"),IO::Integer,IO::Output,"0"));
	    ioAdd(new IO("hour",_("Hours [0...23]"),IO::Integer,IO::Output,"0"));
	    ioAdd(new IO("mday",_("Day of the month [1...31]"),IO::Integer,IO::Output,"0"));
	    ioAdd(new IO("month",_("Month [0...11]"),IO::Integer,IO::Output,"0"));
	    ioAdd(new IO("year",_("Year, from 1900"),IO::Integer,IO::Output,"0"));
	    ioAdd(new IO("wday",_("Day of the week [0...6]"),IO::Integer,IO::Output,"0"));
	    ioAdd(new IO("yday",_("Day of the year [0...365]"),IO::Integer,IO::Output,"0"));
	    ioAdd(new IO("isdst",_("Daylight saving time"),IO::Integer,IO::Output,"0"));
	}

	string name( )	{ return _("Time: Date and time"); }
	string descr( )	{ return _("Full date and time in seconds, minutes, hours, etc., based on the absolute time in seconds from the epoch of 01-01-1970."); }

	void calc( TValFunc *val ) {
	    time_t tm_t = val->getI(0);
	    struct tm tm_tm;
	    localtime_r(&tm_t, &tm_tm);

	    val->setI(1, tm_tm.tm_sec);
	    val->setI(2, tm_tm.tm_min);
	    val->setI(3, tm_tm.tm_hour);
	    val->setI(4, tm_tm.tm_mday);
	    val->setI(5, tm_tm.tm_mon);
	    val->setI(6, 1900+tm_tm.tm_year);
	    val->setI(7, tm_tm.tm_wday);
	    val->setI(8, tm_tm.tm_yday);
	    val->setI(9, tm_tm.tm_isdst);
	}
};

//*************************************************
//* tmTime                                        *
//*************************************************
class tmTime : public TFunction
{
    public:
	tmTime( ) : TFunction("tmTime",SSPC_ID) {
	    ioAdd(new IO("sec",_("Seconds"),IO::Integer,IO::Return,"0"));
	    ioAdd(new IO("usec",_("Microseconds"),IO::Integer,IO::Output,"-1"));
	}

	string name( )	{ return _("Time: Time"); }
	string descr( )	{ return _("Getting the full time, in seconds since 01.01.1970, and in microseconds, if <usec> is installed in a non-negative value."); }

	void calc( TValFunc *val ) {
	    if(val->getI(1) < 0) val->setI(0, time(NULL));
	    else {
		int64_t tm = TSYS::curTime();
		val->setI(0, tm/1000000);
		val->setI(1, tm%1000000);
	    }
	}
};

//*************************************************
//* tmStr2Tm                                      *
//*************************************************
class tmStr2Tm : public TFunction
{
    public:
	tmStr2Tm( ) : TFunction("tmStrPTime",SSPC_ID) {
	    ioAdd(new IO("sec",_("Seconds"),IO::Integer,IO::Return,"0"));
	    ioAdd(new IO("str",_("Date and time string"),IO::String,IO::Default,""));
	    ioAdd(new IO("form",_("Date and time format"),IO::String,IO::Default,"%Y-%m-%d %H:%M:%S"));
	}

	string name( )	{ return _("Time: String to time"); }
	string descr( )	{ return _("Converting the string data and time, for the <form>, to the time in seconds since 01.01.1970."); }

	void calc( TValFunc *val ) {
	    struct tm stm;
	    stm.tm_isdst = -1;
	    strptime(val->getS(1).c_str(), val->getS(2).c_str(), &stm);
	    val->setI(0, mktime(&stm));
	}
};

//*************************************************
//* tmCron                                        *
//*************************************************
class tmCron : public TFunction
{
    public:
	tmCron( ) : TFunction("tmCron",SSPC_ID) {
	    ioAdd(new IO("res",_("Result, seconds"),IO::Integer,IO::Return,"0"));
	    ioAdd(new IO("str",_("CRON"),IO::String,IO::Default,"* * * * *"));
	    ioAdd(new IO("base",_("Base time, seconds"),IO::Integer,IO::Default,"0"));
	}

	string name( )	{ return _("Time: Schedule time at CRON"); }
	string descr( )	{ return _("Scheduling the time at the CRON standard, returning the planned time from the <base> time or from the current time, if the base is not specified."); }

	void calc( TValFunc *val )	{ val->setI(0, SYS->cron(val->getS(1),val->getI(2))); }
};

//*************************************************
//* tmSleep                                       *
//*************************************************
class tmSleep : public TFunction
{
    public:
	tmSleep( ) : TFunction("tmSleep",SSPC_ID) {
	    ioAdd(new IO("res",_("Result"),IO::Integer,IO::Return,"0"));
	    ioAdd(new IO("tm",_("Time, seconds"),IO::Real,IO::Default,"0"));
	}

	string name( )	{ return _("Time: sleep"); }
	string descr( )	{ return _("Short sleeping from nanoseconds and up to STD_INTERF_TM (5 seconds)."); }

	void calc( TValFunc *v )	{ v->setI(0, TSYS::sysSleep(vmin(STD_INTERF_TM,v->getR(1)))); }
};


}

#endif //TIMEFNC_H
