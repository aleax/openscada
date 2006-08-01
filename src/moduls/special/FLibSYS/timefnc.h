
//OpenSCADA system module Special.FLibTime file: timefnc.h
/***************************************************************************
 *   Copyright (C) 2005-2006 by Roman Savochenko                           *
 *   rom_as@fromru.com                                                     *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
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

#include <string>
#include <vector>

#include <tfunction.h>

#include "statfunc.h"

using std::string;
using std::vector;

namespace FLibSYS
{

//------------------------------------------------------------------------------------
//date
//------------------------------------------------------------------------------------
class TmDate : public TFunction
{
    public:
	TmDate() : TFunction("tmDate")
	{
	    ioAdd( new IO("fullsec",mod->I18N("Full seconds"),IO::Integer,IO::Input,"0") );
	    ioAdd( new IO("sec",mod->I18N("Seconds"),IO::Integer,IO::Output,"0") );
	    ioAdd( new IO("min",mod->I18N("Minutes"),IO::Integer,IO::Output,"0") );
	    ioAdd( new IO("hour",mod->I18N("Hours"),IO::Integer,IO::Output,"0") );
	    ioAdd( new IO("mday",mod->I18N("Day of the month"),IO::Integer,IO::Output,"0") );
	    ioAdd( new IO("month",mod->I18N("Month"),IO::Integer,IO::Output,"0") );
	    ioAdd( new IO("year",mod->I18N("Year"),IO::Integer,IO::Output,"0") );
	    ioAdd( new IO("wday",mod->I18N("Day of the week"),IO::Integer,IO::Output,"0") );
	    ioAdd( new IO("yday",mod->I18N("Day of the year"),IO::Integer,IO::Output,"0") );
	    ioAdd( new IO("isdst",mod->I18N("Daylight saving time"),IO::Integer,IO::Output,"0") );
	}
	
	string name()	{ return mod->I18N("Date"); }
	string descr()	{ return mod->I18N("Full date."); }

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

//------------------------------------------------------------------------------------
//time
//------------------------------------------------------------------------------------
class TmTime : public TFunction
{
    public:
	TmTime() : TFunction("tmTime")
	{
	    ioAdd( new IO("sec",mod->I18N("Seconds"),IO::Integer,IO::Return,"0") );
	}
	
	string name()	{ return mod->I18N("Time"); }
	string descr()	{ return mod->I18N("Full time (since 01.01.1970)."); }

	void calc( TValFunc *val )
	{
	    val->setI(0,time(NULL));
	}
};

//------------------------------------------------------------------------------------
//ctime
//------------------------------------------------------------------------------------
class TmCtime : public TFunction
{
    public:
	TmCtime() : TFunction("tmCtime")
	{
	    ioAdd( new IO("val",mod->I18N("Full string date"),IO::String,IO::Return,"") );
	    ioAdd( new IO("sec",mod->I18N("Seconds"),IO::Integer,IO::Input,"0") );
	}
	
	string name()	{ return mod->I18N("CTime"); }
	string descr()	{ return mod->I18N("Full string time."); }

	void calc( TValFunc *val )
	{
	    time_t tm_t = val->getI(1);
	    val->setS(0,ctime(&tm_t));
	}
};

}

#endif //TIMEFNC_H

