
//OpenSCADA system file: tsubsys.cpp
/***************************************************************************
 *   Copyright (C) 2003-2007 by Roman Savochenko                           *
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

#include <stdarg.h>
#include <unistd.h>

#include "tsys.h"
#include "tmess.h"
#include "tsubsys.h"

//*************************************************
//* TSubSYS                                       *
//*************************************************
TSubSYS::TSubSYS( char *id, char *name, bool modi ) : 
	m_id(id), m_name(name), m_mod_sys(modi), m_mod(-1)
{
    if(subModule()) m_mod = grpAdd("mod_");
}

TSubSYS::~TSubSYS(  )
{
    nodeDelAll();
}

string TSubSYS::subName()
{
    return m_name.size()?_(m_name.c_str()):m_id;
}    

void TSubSYS::modList( vector<string> &list )    
{ 
    if( !subModule() ) throw TError(nodePath().c_str(),_("No modules subsystem!"));
    chldList(m_mod,list);
}

bool TSubSYS::modPresent( const string &name )
{
    if( !subModule() ) throw TError(nodePath().c_str(),_("No modules subsystem!")); 
    return chldPresent(m_mod,name); 
}	

void TSubSYS::modAdd( TModule *modul )
{
    if( !subModule() ) throw TError(nodePath().c_str(),_("No modules subsystem!"));
    if( chldPresent(m_mod,modul->modId()) ) return;
    chldAdd(m_mod,modul);
#if OSC_DEBUG 
    vector<string> list;
    modul->modInfo( list );
    for( unsigned i_opt = 0; i_opt < list.size(); i_opt++)
    	mess_info(nodePath().c_str(),"-> %s: %s",_(list[i_opt].c_str()),modul->modInfo(list[i_opt]).c_str());
#endif
}

void TSubSYS::modDel( const string &name )
{
    if( !subModule() ) throw TError(nodePath().c_str(),_("No modules subsystem!"));
    mess_info(nodePath().c_str(),_("Disconnect modul <%s>!"),name.c_str());
    chldDel(m_mod,name);
}

AutoHD<TModule> TSubSYS::modAt( const string &name )
{
    if( !subModule() ) throw TError(nodePath().c_str(),_("No modules subsystem!")); 
    return chldAt(m_mod,name); 
}

void TSubSYS::subLoad( ) 
{
    if( !subModule() )	return;
    vector<string> list;
    modList(list);
    for(unsigned i_m=0; i_m < list.size(); i_m++)
	try{ modAt(list[i_m]).at().modLoad( ); }
	catch(TError err) 
	{ 
	    mess_err(err.cat.c_str(),"%s",err.mess.c_str());
	    mess_err(nodePath().c_str(),_("Load module '%s' is error."),list[i_m].c_str());
	}
}

void TSubSYS::subSave( )
{
    if( !subModule() )  return;
    vector<string> list;
    modList(list);
    for(unsigned i_m=0; i_m < list.size(); i_m++)
	try{ modAt(list[i_m]).at().modSave( ); }
	catch(TError err) 
	{ 
	    mess_err(err.cat.c_str(),"%s",err.mess.c_str());
	    mess_err(nodePath().c_str(),_("Save module '%s' is error."),list[i_m].c_str());
	}	
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
    for(unsigned i_m=0; i_m < list.size(); i_m++)
	try{ modAt(list[i_m]).at().modStart( ); }
	catch(TError err)
        { 
	    mess_err(err.cat.c_str(),"%s",err.mess.c_str()); 
	    mess_err(nodePath().c_str(),_("Start module '%s' is error."),list[i_m].c_str());	    
	}
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
	    mess_err(nodePath().c_str(),_("Stop module '%s' is error."),list[i_m].c_str());
	}	
}		

void TSubSYS::cntrCmdProc( XMLNode *opt )
{
    //Get page info
    if( opt->name() == "info" )
    {
	ctrMkNode("oscada_cntr",opt,-1,"/",_("Subsystem: ")+subName());
	ctrMkNode("branches",opt,-1,"/br","",0444);
	if(TUIS::icoPresent(subId()))	ctrMkNode("img",opt,-1,"/ico","",0444);
	if( subModule() )
	{
	    ctrMkNode("grp",opt,-1,"/br/mod_",_("Module"),0444,"root","root",1,"list","/mod/br");
	    if(ctrMkNode("area",opt,-1,"/mod",_("Modules"),0444,"root","root"))
		ctrMkNode("list",opt,-1,"/mod/br",_("Modules"),0444,"root","root",3,"tp","br","idm","1","br_pref","mod_");
	}
	ctrMkNode("area",opt,-1,"/help",_("Help"));
	return;
    }
    //Process command to page
    string a_path = opt->attr("path");
    if( a_path == "/ico" && ctrChkNode(opt) )
    {
        string itp;
        opt->setText(TSYS::strEncode(TUIS::icoGet(subId(),&itp),TSYS::base64));
        opt->setAttr("tp",itp);	
    }
    else if( subModule() )
    { 
	if( a_path == "/mod/br" && ctrChkNode(opt,"get",0444,"root","root",SEQ_RD) )
	{
	    vector<string> list;
	    modList(list);
	    for( unsigned i_a=0; i_a < list.size(); i_a++ )
		opt->childAdd("el")->setAttr("id",list[i_a])->setText(modAt(list[i_a]).at().modName());
	}
    }
}
