
//OpenSCADA system file: ttipdaq.cpp
/***************************************************************************
 *   Copyright (C) 2003-2010 by Roman Savochenko                           *
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


#include <stdio.h>

#include "tsys.h"
#include "tmodule.h"
#include "tmess.h"
#include "tbds.h"
#include "tdaqs.h"
#include "ttiparam.h"
#include "ttipdaq.h"

using namespace OSCADA;

//*************************************************
//* TTipDAQ                                       *
//*************************************************
TTipDAQ::TTipDAQ( const string &id ) : TModule(id)
{
    m_cntr = grpAdd("cntr_");

    fldAdd( new TFld("ID",_("ID"),TFld::String,TCfg::Key|TFld::NoWrite,"20") );
    fldAdd( new TFld("NAME",_("Name"),TFld::String,TCfg::TransltText,"50") );
    fldAdd( new TFld("DESCR",_("Description"),TFld::String,TFld::FullText|TCfg::TransltText,"300") );
    fldAdd( new TFld("ENABLE",_("To enable"),TFld::Boolean,0,"1","0") );
    fldAdd( new TFld("START",_("To start"),TFld::Boolean,0,"1","0") );
}

TTipDAQ::~TTipDAQ( )
{
    nodeDelAll();

    while(paramt.size())
    {
	delete paramt[0];
	paramt.erase(paramt.begin());
    }
}

TDAQS &TTipDAQ::owner( )	{ return *(TDAQS*)nodePrev(); }

void TTipDAQ::postEnable( int flag )
{
    TModule::postEnable(flag);

    if( redntAllow( ) )
    {
	fldAdd( new TFld("REDNT",_("Redundant"),TFld::Integer,TFld::Selected,"1","0",
	    (TSYS::int2str(TController::Off)+";"+TSYS::int2str(TController::Asymmetric)/*+";"+TSYS::int2str(TController::Symmetric)*/).c_str(),
	    _("Off;Asymmetric"/*;Symmetric"*/)) );
	fldAdd( new TFld("REDNT_RUN",_("Preferable run"),TFld::String,0,"20","<high>") );
    }
}

void TTipDAQ::modStart( )
{
    vector<string> lst;
    //> Start all controllers
    list(lst);
    for(unsigned i_l = 0; i_l < lst.size(); i_l++)
	if(at(lst[i_l]).at().toStart())
	    try{ at(lst[i_l]).at().start(); }
	    catch(TError err)
	    {
		mess_err(err.cat.c_str(),"%s",err.mess.c_str());
		mess_err(nodePath().c_str(),_("Start controller <%s> error."),(modId()+"."+lst[i_l]).c_str());
	    }
}

void TTipDAQ::modStop( )
{
    vector<string> lst;
    //> Stop all controllers
    list(lst);
    for(unsigned i_l = 0; i_l < lst.size(); i_l++)
        at(lst[i_l]).at().stop( );
}

void TTipDAQ::add( const string &name, const string &daq_db )
{
    chldAdd(m_cntr, ContrAttach(name,daq_db));
}

TTipParam &TTipDAQ::tpPrmAt( unsigned id )
{
    if(id >= paramt.size() || id < 0)
	throw TError(nodePath().c_str(),_("Id of parameter type error!"));
    return *paramt[id];
}

bool TTipDAQ::tpPrmPresent( const string &name_t )
{
    for(unsigned i_t=0; i_t < paramt.size(); i_t++)
	if(paramt[i_t]->name == name_t) return true;
    return false;
}

int TTipDAQ::tpParmAdd( const char *id, const char *n_db, const char *name )
{
    if( tpPrmPresent(id) )	return tpPrmToId(id);

    //> Add type
    int i_t = paramt.size();
    paramt.push_back( new TTipParam(id,name,n_db) );
    //> Add structure fields
    paramt[i_t]->fldAdd( new TFld("SHIFR",_("ID"),TFld::String,TCfg::Key|TFld::NoWrite,"20") );
    paramt[i_t]->fldAdd( new TFld("NAME",_("Name"),TFld::String,TCfg::TransltText,"50") );
    paramt[i_t]->fldAdd( new TFld("DESCR",_("Description"),TFld::String,TFld::FullText|TCfg::TransltText,"200") );
    paramt[i_t]->fldAdd( new TFld("EN",_("To enable"),TFld::Boolean,TCfg::NoVal,"1","0") );

    return i_t;
}

int TTipDAQ::tpPrmToId( const string &name_t)
{
    for(unsigned i_t=0; i_t < paramt.size(); i_t++)
	if(paramt[i_t]->name == name_t) return i_t;
    throw TError(nodePath().c_str(),_("Parameter type is not present."));
}

TController *TTipDAQ::ContrAttach( const string &name, const string &daq_db )
{
    throw TError(nodePath().c_str(),_("Error attach new controller %s."),name.c_str());
}

string TTipDAQ::compileFunc( const string &lang, TFunction &fnc_cfg, const string &prog_text, const string &usings )
{
    throw TError(nodePath().c_str(),_("Module doesn't support the function for compilation programming languages."));
}

void TTipDAQ::cntrCmdProc( XMLNode *opt )
{
    //> Get page info
    if(opt->name() == "info")
    {
	TModule::cntrCmdProc(opt);
	ctrMkNode("grp",opt,-1,"/br/cntr_",_("Controller"),RWRWR_,"root",SDAQ_ID,2,"idm","1","idSz","20");
	if(ctrMkNode("area",opt,0,"/tctr",_("Controllers")))
	    ctrMkNode("list",opt,-1,"/tctr/ctr",_("Controllers"),RWRWR_,"root",SDAQ_ID,5,"tp","br","idm","1","s_com","add,del","br_pref","cntr_","idSz","20");
	return;
    }
    //> Process command to page
    string a_path = opt->attr("path");
    if(a_path == "/br/cntr_" || a_path == "/tctr/ctr")
    {
	if(ctrChkNode(opt,"get",RWRWR_,"root",SDAQ_ID,SEC_RD))
	{
	    vector<string> c_list;
	    list(c_list);
	    for(unsigned i_a=0; i_a < c_list.size(); i_a++)
		opt->childAdd("el")->setAttr("id",c_list[i_a])->setText(at(c_list[i_a]).at().name());
	}
	if(ctrChkNode(opt,"add",RWRWR_,"root",SDAQ_ID,SEC_WR))
	{
	    string vid = TSYS::strEncode(opt->attr("id"),TSYS::oscdID);
	    add(vid); at(vid).at().setName(opt->text());
	}
	if(ctrChkNode(opt,"del",RWRWR_,"root",SDAQ_ID,SEC_WR))	chldDel(m_cntr,opt->attr("id"),-1,1);
    }
    else TModule::cntrCmdProc(opt);
}
