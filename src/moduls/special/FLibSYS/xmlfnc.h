
//OpenSCADA system module Special.FLibSYS file: xmlfnc.h
/***************************************************************************
 *   Copyright (C) 2009-2014 by Roman Savochenko, <rom_as@oscada.org>      *
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

#ifndef XMLFNC_H
#define XMLFNC_H

#include <tfunction.h>
#include <tvariant.h>

#include "statfunc.h"

namespace FLibSYS
{

//*************************************************
//* XML node object creation                      *
//*************************************************
class xmlNode : public TFunction
{
    public:
	xmlNode( ) : TFunction("xmlNode",SSPC_ID) {
	    ioAdd(new IO("rez",_("Result"),IO::Object,IO::Return));
	    ioAdd(new IO("name",_("Name"),IO::String,IO::Default));
	}

	string name( )	{ return _("XML: Node"); }
	string descr( )	{ return _("XML node object creation."); }

	void calc( TValFunc *val )	{ val->setO(0, new XMLNodeObj(val->getS(1))); }
};

//*************************************************
//* Control request                               *
//*************************************************
class xmlCntrReq : public TFunction
{
    public:
	xmlCntrReq( ) : TFunction("xmlCntrReq", SSPC_ID) {
	    ioAdd(new IO("rez",_("Result"),IO::String,IO::Return));
	    ioAdd(new IO("req",_("Request"),IO::Object,IO::Default));
	    ioAdd(new IO("stat",_("Station"),IO::String,IO::Default));
	}

	string name( )	{ return _("XML: Control request"); }
	string descr( )	{ return _("Control request to system by XML."); }

	void calc( TValFunc *val );
};

}

#endif //XMLFNC_H
