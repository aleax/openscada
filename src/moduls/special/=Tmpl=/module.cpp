
//!!! Module name, file name and module's license. Change for your need.
//OpenSCADA system module Special.Tmpl file: module.cpp
/***************************************************************************
 *   Copyright (C) 2012 by MyName MyFamily                                 *
 *   my@email.org                                                          *
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

//!!! System's includings. Add need for your module includings.

//!!! OpenSCADA module's API includings. Add need for your module includings.
#include <tsys.h>

//!!! Self your module's includings. Add need for your module includings.
#include "module.h"

//!!! Module's meta-information. Change for your module.
//*************************************************
//* Modul info!                                   *
#define MOD_ID		"Tmpl"
#define MOD_NAME	_("Special template")
#define MOD_TYPE	SSPC_ID
#define VER_TYPE	SSPC_VER
#define SUB_TYPE	"LIB"
#define MOD_VER		"0.0.1"
#define AUTHORS		_("MyName MyFamily")
#define DESCRIPTION	_("Special subsystem template module.")
#define LICENSE		"MyLicense"
//*************************************************

ModTmpl::Lib *ModTmpl::mod;

//!!! Required section for binding OpenSCADA kernel's to this module. Gives information and create module's root object.
//!!! Not remove this section!
extern "C"
{
#ifdef MOD_INCL
    TModule::SAt spec_Tmpl_module( int n_mod )
#else
    TModule::SAt module( int n_mod )
#endif
    {
	if( n_mod==0 )	return TModule::SAt(MOD_ID,MOD_TYPE,VER_TYPE);
	return TModule::SAt("");
    }

#ifdef MOD_INCL
    TModule *spec_Tmpl_attach( const TModule::SAt &AtMod, const string &source )
#else
    TModule *attach( const TModule::SAt &AtMod, const string &source )
#endif
    {
	if( AtMod == TModule::SAt(MOD_ID,MOD_TYPE,VER_TYPE) )
	    return new ModTmpl::Lib( source );
	return NULL;
    }
}

//!!! Include for default call into your module's namespace.
using namespace ModTmpl;

//*************************************************
//* Lib:					  *
//*************************************************
//!!! Constructor for module's root object. Append into for your need.
Lib::Lib( string src ) : TSpecial(MOD_ID)
{
    //!!! Init shortcut to module's root object. No change it.
    mod		= this;

    //!!! Loading module's meta-information into root object. No change it.
    mName	= MOD_NAME;
    mType	= MOD_TYPE;
    mVers	= MOD_VER;
    mAuthor	= AUTHORS;
    mDescr	= DESCRIPTION;
    mLicense	= LICENSE;
    mSource	= src;

    m_fnc = grpAdd("fnc_");
}

//!!! Destructor for module's root object. Append into for your need.
Lib::~Lib()
{

}

//!!! Module's post enable call. Add your module objects initialize code.
void Lib::postEnable( int flag )
{
    TModule::postEnable( flag );

    if( flag&TCntrNode::NodeRestore )	return;

    //!!! Place here your code for internal objects initialize.
    //> Reg functions
    reg( new MathAcos() );
    reg( new MathAsin() );

    //> Enable functions
    vector<string> lst;
    list(lst);
    for( int i_l = 0; i_l < lst.size(); i_l++ )
	at(lst[i_l]).at().setStart(true);
}

//!!! Module's comandline options the print help function. Add your module commandline parameters info.
string Lib::optDescr( )
{
    char buf[STR_BUF_LEN];
    snprintf(buf,sizeof(buf),_(
	"======================= The module <%s:%s> options =======================\n"
	"---------- Parameters of the module section '%s' in config-file ----------\n\n"),
	MOD_TYPE,MOD_ID,nodePath().c_str());

    return buf;
}

//!!! Module's start call. Place code for internal objects start.
void Lib::modStart( )
{
    vector<string> lst;
    list(lst);
    for( int i_l = 0; i_l < lst.size(); i_l++ )
	at(lst[i_l]).at().setStart(true);
    run_st = true;
}

//!!! Module's stop call. Place code for internal objects stop.
void Lib::modStop( )
{
    vector<string> lst;
    list(lst);
    for( int i_l = 0; i_l < lst.size(); i_l++ )
	at(lst[i_l]).at().setStart(false);
    run_st = false;
}

//!!! OpenSCADA control interface comands process virtual function.
//!!! For example, process access from standard configurators of OpenSCADA to individual module's parameters.
//!!! Modify for self needs
void Lib::cntrCmdProc( XMLNode *opt )
{
    //> Get page info
    if(opt->name() == "info")
    {
	TSpecial::cntrCmdProc(opt);
	ctrMkNode("fld",opt,-1,"/help/g_help",_("Options help"),R_R___,"root",SSPC_ID,3,"tp","str","cols","90","rows","5");
	ctrMkNode("grp",opt,-1,"/br/fnc_",_("Function"),R_R_R_,"root",SSPC_ID,1,"idm","1");
	ctrMkNode("list",opt,-1,"/prm/func",_("Functions"),R_R_R_,"root",SSPC_ID,3,"tp","br","idm","1","br_pref","fnc_");
	return;
    }

    //> Process command to page
    string a_path = opt->attr("path");
    if((a_path == "/br/fnc_" || a_path == "/prm/func") && ctrChkNode(opt))
    {
	vector<string> lst;
	list(lst);
	for(unsigned i_f=0; i_f < lst.size(); i_f++)
	    opt->childAdd("el")->setAttr("id",lst[i_f])->setText(at(lst[i_f]).at().name());
    }
    if(a_path == "/help/g_help" && ctrChkNode(opt,"get",R_R___,"root",SSPC_ID))	opt->setText(optDescr());
    else TSpecial::cntrCmdProc(opt);
}
