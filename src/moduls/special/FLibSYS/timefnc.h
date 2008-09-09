
//OpenSCADA system module Special.FLibSYS file: timefnc.h
/***************************************************************************
 *   Copyright (C) 2005-2008 by Roman Savochenko                           *
 *   rom_as@fromru.com                                                     *
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
//* tmDate                                        *
//*************************************************
class tmDate : public TFunction
{
    public:
	tmDate( ) : TFunction("tmDate")
	{
	    ioAdd( new IO("fullsec",_("Full seconds"),IO::Integer,IO::Default,"0") );
	    ioAdd( new IO("sec",_("Seconds"),IO::Integer,IO::Output,"0") );
	    ioAdd( new IO("min",_("Minutes"),IO::Integer,IO::Output,"0") );
	    ioAdd( new IO("hour",_("Hours"),IO::Integer,IO::Output,"0") );
	    ioAdd( new IO("mday",_("Day of the month"),IO::Integer,IO::Output,"0") );
	    ioAdd( new IO("month",_("Month"),IO::Integer,IO::Output,"0") );
	    ioAdd( new IO("year",_("Year"),IO::Integer,IO::Output,"0") );
	    ioAdd( new IO("wday",_("Day of the week"),IO::Integer,IO::Output,"0") );
	    ioAdd( new IO("yday",_("Day of the year"),IO::Integer,IO::Output,"0") );
	    ioAdd( new IO("isdst",_("Daylight saving time"),IO::Integer,IO::Output,"0") );
	}

	string name( )	{ return _("Time: Date"); }
	string descr( )	{ return _("Full date."); }

	void calc( TValFunc *val )
	{
	    time_t tm_t = val->getI(0);
	    struct tm *tm_tm = localtime(&tm_t);

	    val->setI(1,tm_tm->tm_sec);
	    val->setI(2,tm_tm->tm_min);
	    val->setI(3,tm_tm->tm_hour);
	    val->setI(4,tm_tm->tm_mday);
	    val->setI(5,tm_tm->tm_mon);
	    val->setI(6,1900+tm_tm->tm_year);
	    val->setI(7,tm_tm->tm_wday);
	    val->setI(8,tm_tm->tm_yday);
	    val->setI(9,tm_tm->tm_isdst);
	}
};

//*************************************************
//* tmTime                                        *
//*************************************************
class tmTime : public TFunction
{
    public:
	tmTime( ) : TFunction("tmTime")
	{
	    ioAdd( new IO("sec",_("Seconds"),IO::Integer,IO::Return,"0") );
	}

	string name( )	{ return _("Time: Time"); }
	string descr( )	{ return _("Full time (since 01.01.1970)."); }

	void calc( TValFunc *val )
	{
	    val->setI(0,time(NULL));
	}
};

//*************************************************
//* tmCtime                                       *
//*************************************************
class tmCtime : public TFunction
{
    public:
	tmCtime( ) : TFunction("tmCtime")
	{
	    ioAdd( new IO("val",_("Full string date"),IO::String,IO::Return,"") );
	    ioAdd( new IO("sec",_("Seconds"),IO::Integer,IO::Default,"0") );
	}

	string name( )	{ return _("Time: String time"); }
	string descr( )	{ return _("Full string time."); }

	void calc( TValFunc *val )
	{
	    time_t tm_t = val->getI(1);
	    val->setS(0,ctime(&tm_t));
	}
};

//*************************************************
//* tmStr2Tm                                      *
//*************************************************
class tmStr2Tm : public TFunction
{
    public:
	tmStr2Tm( ) : TFunction("tmStrPTime")
	{
	    ioAdd( new IO("sec",_("Seconds"),IO::Integer,IO::Return,"0") );
	    ioAdd( new IO("str",_("Date string"),IO::String,IO::Default,"") );
	    ioAdd( new IO("form",_("Date format"),IO::String,IO::Default,"%Y-%m-%d %H:%M:%S") );
	}

	string name( )	{ return _("Time: String to time"); }
	string descr( )	{ return _("Convert a string representation of time to a time."); }

	void calc( TValFunc *val )
	{
	    struct tm stm;
	    stm.tm_isdst = -1;
	    strptime(val->getS(1).c_str(),val->getS(2).c_str(),&stm);
	    val->setI(0,mktime(&stm));
	}
};

}

#endif //TIMEFNC_H
