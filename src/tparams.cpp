/***************************************************************************
 *   Copyright (C) 2004 by Roman Savochenko                                *
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
 
#include "xml.h"
#include "tmessage.h"
#include "tparamcontr.h"
#include "tparam.h"
#include "tparams.h"

const char *TParamS::o_name = "TParamS";
const char *TParamS::s_name = "Parameters";
 
TParamS::TParamS( TKernel *app ) : 
	TConfig(NULL), m_owner(app)
{
    m_prm = grpAdd();
    nodeEn();
    
    elem().fldAdd( new TFld("NAME","Name",T_STRING,"20") );
}

TParamS::~TParamS(  )
{

}

string TParamS::name()
{ 
    return(Mess->I18N((char *)s_name));
}

void TParamS::add( TControllerS::SName cntr, const string &param )
{
    if( chldAvoid(m_prm,param) ) at(param).at().reg( cntr, param );
    else	chldAdd(m_prm,new TParam( cntr, param, this )); 
}

void TParamS::del( TControllerS::SName cntr, const string &param )
{
    if( !at(param).at().unreg( cntr, param ) )
	chldDel(m_prm,param);
}

//==============================================================
//================== Controll functions ========================
//==============================================================
void TParamS::ctrStat_( XMLNode *inf )
{
    char *i_cntr = 
	"<oscada_cntr>"
	"</oscada_cntr>"; 

    inf->load(i_cntr);
    inf->text(name());
} 

