
//OpenSCADA system file: tmodule.cpp
/***************************************************************************
 *   Copyright (C) 2003-2009 by Roman Savochenko                           *
 *   rom_as@oscada.org, rom_as@fromru.com                                  *
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

#include <sys/types.h>
#include <sys/stat.h>
#include <stdarg.h>
#include <unistd.h>
#include <dlfcn.h>
#include <string.h>
#include <libintl.h>

#include "tsys.h"
#include "terror.h"
#include "tmess.h"
#include "tsubsys.h"
#include "tmodule.h"

//*************************************************
//* TModule                                       *
//*************************************************
const char *TModule::l_info[] =
    {"Module","Name","Type","Source","Version","Author","Description","License"};

TModule::TModule( )
{

}

TModule::~TModule(  )
{
    //- Clean export function list -
    for(int i=0; i < m_efunc.size(); i++)
        delete m_efunc[i];
}

string TModule::modName()
{
    return I18Ns(mName);
}

void TModule::postEnable( int flag )
{
    if( flag&TCntrNode::NodeRestore )	return;

    mess_info(nodePath().c_str(),_("Connect module!"));

    lc_id = string("oscd_")+mId;
    bindtextdomain(lc_id.c_str(),LOCALEDIR);
}

TSubSYS &TModule::owner( )	{ return *(TSubSYS*)nodePrev(); }

void TModule::modFuncList( vector<string> &list )
{
    list.clear();
    for(int i=0; i < m_efunc.size(); i++)
	list.push_back(m_efunc[i]->prot);
}

bool TModule::modFuncPresent( const string &prot )
{
    for(int i=0; i < m_efunc.size(); i++)
	if( m_efunc[i]->prot == prot )
	    return true;
    return false;
}

TModule::ExpFunc &TModule::modFunc( const string &prot )
{
    for(int i=0; i < m_efunc.size(); i++)
	if( m_efunc[i]->prot == prot ) return *m_efunc[i];
    throw TError(nodePath().c_str(),_("Function <%s> is not present in the module!"),prot.c_str());
}

void TModule::modFunc( const string &prot, void (TModule::**offptr)() )
{
    *offptr = modFunc(prot).ptr;
}

void TModule::modInfo( vector<string> &list )
{
    for( int i_opt = 0; i_opt < sizeof(l_info)/sizeof(char *); i_opt++ )
	list.push_back( l_info[i_opt] );
}

string TModule::modInfo( const string &name )
{
    string info;

    if( name == l_info[0] )      info=mId;
    else if( name == l_info[1] ) info=I18Ns(mName);
    else if( name == l_info[2] ) info=I18Ns(mType);
    else if( name == l_info[3] ) info=mSource;
    else if( name == l_info[4] ) info=I18Ns(mVers);
    else if( name == l_info[5] ) info=I18Ns(mAutor);
    else if( name == l_info[6] ) info=I18Ns(mDescr);
    else if( name == l_info[7] ) info=I18Ns(mLicense);

    return info;
}

void TModule::cntrCmdProc( XMLNode *opt )
{
    //- Get page info -
    if( opt->name() == "info" )
    {
	TCntrNode::cntrCmdProc(opt);
	ctrMkNode("oscada_cntr",opt,-1,"/",_("Module: ")+modId(),0444);
	ctrMkNode("branches",opt,-1,"/br","",0444);
	if(TUIS::icoPresent(owner().subId()+"."+modId())) ctrMkNode("img",opt,-1,"/ico","",0444);
	if(ctrMkNode("area",opt,-1,"/help",_("Help")))
	    if(ctrMkNode("area",opt,-1,"/help/m_inf",_("Module information")))
	    {
		vector<string> list;
		modInfo(list);
		for( int i_l = 0; i_l < list.size(); i_l++)
		    ctrMkNode("fld",opt,-1,(string("/help/m_inf/")+list[i_l]).c_str(),I18Ns(list[i_l]),0444,"root","root",1,"tp","str");
	    }
	return;
    }

    //- Process command to page -
    string a_path = opt->attr("path");
    if( a_path == "/ico" && ctrChkNode(opt) )
    {
	string itp;
	opt->setText(TSYS::strEncode(TUIS::icoGet(owner().subId()+"."+modId(),&itp),TSYS::base64));
	opt->setAttr("tp",itp);
    }
    else if( a_path.substr(0,11) == "/help/m_inf" && ctrChkNode(opt) )
	opt->setText(modInfo(TSYS::pathLev(a_path,2)));
    else TCntrNode::cntrCmdProc(opt);
}

const char *TModule::I18N( const char *mess )
{
    const char *rez = Mess->I18N(mess,lc_id.c_str());
    if( !strcmp(mess,rez) ) rez = _(mess);
    return rez;
}

string TModule::I18Ns( const string &mess ) 
{
    return I18N(mess.c_str());
}


