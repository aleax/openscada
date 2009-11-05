
//OpenSCADA system module Special.FLibSYS file: xmlfnc.cpp
/***************************************************************************
 *   Copyright (C) 2009 by Roman Savochenko                                *
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

#include "xmlfnc.h"

using namespace FLibSYS;

//*************************************************
//* Control request                               *
//*************************************************
void xmlCntrReq::calc( TValFunc *val )
{
    try
    {
	XMLNode req;
	if( !dynamic_cast<XMLNodeObj*>(val->getO(1)) ) { val->setS(0,_("1:Request is not object!")); return; }
	((XMLNodeObj*)val->getO(1))->toXMLNode(req);
	string path = req.attr("path");
	if( val->getS(2).empty() )
	{
	    req.setAttr("user",val->user());
	    SYS->cntrCmd(&req);
	}
	else
	{
	    req.setAttr("path",val->getS(2)+"/"+path);
	    SYS->transport().at().cntrIfCmd(req,"xmlCntrReq",val->user());
	    req.setAttr("path",path);
	}
	((XMLNodeObj*)val->getO(1))->fromXMLNode(req);
	val->setS(0,"0");
    }catch(TError err){ val->setS(0,TSYS::strMess(_("1:Request error: %s"),err.mess.c_str())); }
}

