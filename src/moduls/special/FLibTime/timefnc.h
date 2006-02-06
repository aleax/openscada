/***************************************************************************
 *   Copyright (C) 2005 by Roman Savochenko                                *
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

#ifndef STDMATH_H
#define STDMATH_H

#include <time.h>

#include <string>
#include <vector>

#include <tfunction.h>

#include "statfunc.h"

using std::string;
using std::vector;

namespace FLibTime
{

//------------------------------------------------------------------------------------
//date
//------------------------------------------------------------------------------------
class TmDate : public TFunction
{
    public:
	TmDate() : TFunction("date")
	{
	    ioAdd( new IO("sec",st_lib->I18N("Seconds"),IO::Integer,IO::Output,"0") );
	    ioAdd( new IO("min",st_lib->I18N("Minutes"),IO::Integer,IO::Output,"0") );
	    ioAdd( new IO("hour",st_lib->I18N("Hours"),IO::Integer,IO::Output,"0") );
	    ioAdd( new IO("mday",st_lib->I18N("Day of the month"),IO::Integer,IO::Output,"0") );
	    ioAdd( new IO("month",st_lib->I18N("Month"),IO::Integer,IO::Output,"0") );
	    ioAdd( new IO("year",st_lib->I18N("Year"),IO::Integer,IO::Output,"0") );
	    ioAdd( new IO("wday",st_lib->I18N("Day of the week"),IO::Integer,IO::Output,"0") );
	    ioAdd( new IO("yday",st_lib->I18N("Day of the year"),IO::Integer,IO::Output,"0") );
	}
	
	string name()	{ return st_lib->I18N("Date"); }
	string descr()	{ return st_lib->I18N("Full date."); }

	void calc( TValFunc *val )
	{
	    time_t tm_t = time(NULL);
	    struct tm *tm_tm = localtime(&tm_t);
	    
	    val->setI(0,tm_tm->tm_sec);
	    val->setI(1,tm_tm->tm_min);
	    val->setI(2,tm_tm->tm_hour);
	    val->setI(3,tm_tm->tm_mday);
	    val->setI(4,tm_tm->tm_mon);
	    val->setI(5,1900+tm_tm->tm_year);
	    val->setI(6,tm_tm->tm_wday);
	    val->setI(7,tm_tm->tm_yday);
	}
};

//------------------------------------------------------------------------------------
//time
//------------------------------------------------------------------------------------
class TmTime : public TFunction
{
    public:
	TmTime() : TFunction("time")
	{
	    ioAdd( new IO("sec",st_lib->I18N("Seconds"),IO::Integer,IO::Return,"0") );
	}
	
	string name()	{ return st_lib->I18N("Time"); }
	string descr()	{ return st_lib->I18N("Full time (since 01.01.1970)."); }

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
	TmCtime() : TFunction("ctime")
	{
	    ioAdd( new IO("val",st_lib->I18N("Full string date"),IO::String,IO::Return,"") );
	    ioAdd( new IO("sec",st_lib->I18N("Seconds"),IO::Integer,IO::Input,"0") );
	}
	
	string name()	{ return st_lib->I18N("CTime"); }
	string descr()	{ return st_lib->I18N("Full string time."); }

	void calc( TValFunc *val )
	{
	    time_t tm_t = val->getI(1);
	    val->setS(0,ctime(&tm_t));
	}
};

}

#endif //STDMATH_H

