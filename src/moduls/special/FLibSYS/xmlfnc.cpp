
//OpenSCADA system module Special.FLibSYS file: xmlfnc.cpp
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
	AutoHD<XMLNodeObj> xnd = val->getO(1);
	if(xnd.freeStat()) { val->setS(0,_("1:Request is not object!")); return; }
	xnd.at().toXMLNode(req);
	string path = req.attr("path");
	if(val->getS(2).empty())
	{
	    req.setAttr("user", val->user());
	    SYS->cntrCmd(&req);
	}
	else
	{
	    req.setAttr("path", "/"+val->getS(2)+path);
	    SYS->transport().at().cntrIfCmd(req, "xmlCntrReq");
	    req.setAttr("path", path);
	}
	xnd.at().fromXMLNode(req);
	val->setS(0, "0");
    }catch(TError err){ val->setS(0, TSYS::strMess(_("1:Request error: %s"),err.mess.c_str())); }
}
