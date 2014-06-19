
//OpenSCADA system file: tsubsys.cpp
/***************************************************************************
 *   Copyright (C) 2003-2014 by Roman Savochenko, <rom_as@oscada.org>      *
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

#include <stdarg.h>
#include <unistd.h>

#include "tsys.h"
#include "tmess.h"
#include "tsubsys.h"

using namespace OSCADA;

//*************************************************
//* TSubSYS                                       *
//*************************************************
TSubSYS::TSubSYS( const char *id, const char *name, bool modi ) : mStart(false), m_mod_sys(modi), m_mod(-1), m_id(id), m_name(name)
{
    if(subModule()) m_mod = grpAdd("mod_");
}

TSubSYS::~TSubSYS(  )
{
    nodeDelAll();
}

string TSubSYS::objName( )	{ return TCntrNode::objName()+":TSubSYS"; }

TSYS &TSubSYS::owner( )		{ return *(TSYS*)nodePrev(); }

string TSubSYS::subName( )	{ return m_name.size()?_(m_name.c_str()):m_id; }

void TSubSYS::modList( vector<string> &list )
{
    if( !subModule() ) throw TError(nodePath().c_str(),_("The subsystem is not modular!"));
    chldList(m_mod,list);
}

bool TSubSYS::modPresent( const string &name )
{
    if( !subModule() ) throw TError(nodePath().c_str(),_("The subsystem is not modular!")); 
    return chldPresent(m_mod,name);
}

void TSubSYS::modAdd( TModule *modul )
{
    if(!subModule()) throw TError(nodePath().c_str(), _("The subsystem is not modular!"));
    if(chldPresent(m_mod,modul->modId())) return;
    chldAdd(m_mod, modul);
    if(mess_lev() == TMess::Debug)
    {
	vector<string> list;
	modul->modInfo(list);
	for(unsigned i_opt = 0; i_opt < list.size(); i_opt++)
	    mess_debug(nodePath().c_str(), "-> %s: %s", _(list[i_opt].c_str()), modul->modInfo(list[i_opt]).c_str());
    }
}

void TSubSYS::modDel( const string &name )
{
    if( !subModule() ) throw TError(nodePath().c_str(),_("The subsystem is not modular!"));
    mess_info(nodePath().c_str(),_("Disconnect module '%s'!"),name.c_str());
    chldDel(m_mod,name);
}

AutoHD<TModule> TSubSYS::modAt( const string &name )
{
    if( !subModule() ) throw TError(nodePath().c_str(),_("The subsystem is not modular!"));
    return chldAt(m_mod,name);
}

void TSubSYS::subStart( )
{
    if( !SYS->security().at().grpPresent(subId()) )
    {
	SYS->security().at().grpAdd(subId());
	SYS->security().at().grpAt(subId()).at().setLName(subName());
	SYS->security().at().grpAt(subId()).at().setSysItem(true);
	SYS->security().at().grpAt(subId()).at().userAdd("root");
    }

    if( !subModule() )	return;
    vector<string> list;
    modList(list);
    for(unsigned i_m = 0; i_m < list.size(); i_m++)
	try { modAt(list[i_m]).at().modStart();	}
	catch(TError err)
	{
	    mess_err(err.cat.c_str(),"%s",err.mess.c_str());
	    mess_err(nodePath().c_str(),_("Start module '%s' error."),list[i_m].c_str());
	}

    mStart = true;
}

void TSubSYS::subStop( )
{
    if( !subModule() )	return;
    vector<string> list;
    modList(list);
    for(unsigned i_m=0; i_m < list.size(); i_m++)
	try{ modAt(list[i_m]).at().modStop( ); }
	catch(TError err)
	{
	    mess_err(err.cat.c_str(),"%s",err.mess.c_str());
	    mess_err(nodePath().c_str(),_("Stop module '%s' error."),list[i_m].c_str());
	}

    mStart = false;
}

void TSubSYS::perSYSCall( unsigned int cnt )
{
    if(!subModule()) return;
    vector<string> list;
    modList(list);
    for(unsigned i_m = 0; i_m < list.size(); i_m++)
	try{ modAt(list[i_m]).at().perSYSCall(cnt); }
	catch(TError err) { mess_err(err.cat.c_str(),"%s",err.mess.c_str()); }
}

void TSubSYS::cntrCmdProc( XMLNode *opt )
{
    //Get page info
    if(opt->name() == "info")
    {
	TCntrNode::cntrCmdProc(opt);
	ctrMkNode("oscada_cntr",opt,-1,"/",_("Subsystem: ")+subName(),R_R_R_);
	ctrMkNode("branches",opt,-1,"/br","",R_R_R_);
	if(TUIS::icoGet(subId(),NULL,true).size()) ctrMkNode("img",opt,-1,"/ico","",R_R_R_);
	if(subModule())
	{
	    ctrMkNode("grp",opt,-1,"/br/mod_",_("Module"),R_R_R_,"root","root",1,"idm","1");
	    if(ctrMkNode("area",opt,-1,"/mod",_("Modules"),R_R_R_,"root","root"))
		ctrMkNode("list",opt,-1,"/mod/br",_("Modules"),R_R_R_,"root","root",3,"tp","br","idm","1","br_pref","mod_");
	}
	return;
    }
    //Process command to page
    string a_path = opt->attr("path");
    if(a_path == "/ico" && ctrChkNode(opt))
    {
	string itp;
	opt->setText(TSYS::strEncode(TUIS::icoGet(subId(),&itp),TSYS::base64));
	opt->setAttr("tp",itp);
    }
    else if(subModule() && (a_path == "/br/mod_" || a_path == "/mod/br") && ctrChkNode(opt,"get",R_R_R_,"root","root",SEC_RD))
    {
	vector<string> list;
	modList(list);
	for(unsigned i_a=0; i_a < list.size(); i_a++)
	    opt->childAdd("el")->setAttr("id",list[i_a])->setText(modAt(list[i_a]).at().modName());
    }
    else TCntrNode::cntrCmdProc(opt);
}
