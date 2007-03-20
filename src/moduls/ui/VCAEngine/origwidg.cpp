
//OpenSCADA system module UI.VCAEngine file: origwidg.cpp
/***************************************************************************
 *   Copyright (C) 2006-2007 by Roman Savochenko
 *   rom_as@diyaorg.dp.ua
 *
 *   This program is free software; you can redistribute it and/or modify
 *   it under the terms of the GNU General Public License as published by
 *   the Free Software Foundation; either version 2 of the License, or
 *   (at your option) any later version.
 *
 *   This program is distributed in the hope that it will be useful,
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *   GNU General Public License for more details.
 *
 *   You should have received a copy of the GNU General Public License
 *   along with this program; if not, write to the
 *   Free Software Foundation, Inc.,
 *   59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
 ***************************************************************************/

#include <tsys.h>

#include "origwidg.h"
#include "vcaengine.h"
		  
using namespace VCA;

//*************************************************
//* Primitive widget template                     *
//*************************************************
PrWidget::PrWidget( const string &iid ) : LWidget(iid)
{ 

}
    
string PrWidget::ico( )
{
    if( LWidget::ico().size() )
        return LWidget::ico();
    if( TUIS::icoPresent("VCA.wdg_"+id()) )
        return TSYS::strEncode(TUIS::icoGet("VCA.wdg_"+id()),TSYS::base64);
    return "";
}

void PrWidget::cntrCmdProc( XMLNode *opt )
{
    //Get page info
    if( opt->name() == "info" )
    {
	Widget::cntrCmdProc(opt);
    
	ctrMkNode("fld",opt,-1,"/wdg/st/parent",_("Parent"),R_R_R_,user().c_str(),grp().c_str(),
		2,"tp","str","dest","");
	
        return;
    }
    //Process command to page
    string a_path = opt->attr("path");
    if( a_path == "/wdg/st/parent" && ctrChkNode(opt,"get",R_R_R_,user().c_str(),grp().c_str(),SEQ_RD) )
	opt->setText(parentNm());
    else Widget::cntrCmdProc(opt);	    
}

void PrWidget::preDisable(int flag)
{
    if( flag )
	throw TError(mod->nodePath().c_str(),_("Deleting of main primitive widgets is error."));
    
    Widget::preDisable(flag);
}
