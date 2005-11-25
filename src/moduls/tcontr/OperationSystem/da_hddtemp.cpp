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

#include <tsys.h>
#include <resalloc.h>

#include "os_contr.h"
#include "da_hddtemp.h"

using namespace SystemCntr;

//======================================================================
//==== HddTemp
//======================================================================
Hddtemp::Hddtemp( ) : t_tr("Sockets"), n_tr("HDDTemp")
{
    m_res = ResAlloc::resCreate();    
    //HDD value structure
    fldAdd( new TFld("disk",mod->I18N("Name"),TFld::String,FLD_NWR) );
    fldAdd( new TFld("ed",mod->I18N("Measure unit"),TFld::String,FLD_NWR) );
    fldAdd( new TFld("t",mod->I18N("Temperature"),TFld::Dec,FLD_NWR,"3","0") );    
}

Hddtemp::~Hddtemp()
{
    if( ((TTipTransport &)SYS->transport().at().modAt(t_tr).at()).outPresent(n_tr) )
	((TTipTransport &)SYS->transport().at().modAt(t_tr).at()).outDel(n_tr);
    
    ResAlloc::resDelete(m_res);	
}

void Hddtemp::init( TMdPrm *prm )
{
    TCfg &c_subt = prm->cfg("SUBT");
    
    //Create Config
    c_subt.fld().descr() = mod->I18N("Disk");
    c_subt.fld().selValS().clear();
    c_subt.fld().selNm().clear();    

    vector<string> list;
    dList(list);
    for( int i_l = 0; i_l < list.size(); i_l++ )
    {
	c_subt.fld().selValS().push_back(list[i_l]);
	c_subt.fld().selNm().push_back(list[i_l]);
    }
    if( list.size() ) c_subt.setS(list[0]);
}

void Hddtemp::dList( vector<string> &list )
{    
    try 
    { 
	string val = getHDDTemp( );
	int p_cnt = 0;
	list.clear();
        while( TSYS::strSepParse(val,p_cnt+1,'|').size() )
        {
            list.push_back(TSYS::strSepParse(val,p_cnt+1,'|'));
            p_cnt+=5;
        }
    }
    catch( TError err ) { Mess->put(err.cat.c_str(),TMess::Error,err.mess.c_str()); }
}

void Hddtemp::getVal( TMdPrm *prm )
{    
    try 
    { 
       	string dev = prm->cfg("SUBT").getS();
	
	string val = getHDDTemp( );
	
	int p_cnt = 0;
        while( TSYS::strSepParse(val,p_cnt+1,'|').size() )
        {
	    if( TSYS::strSepParse(val,p_cnt+1,'|') == dev )
	    {
		prm->vlAt("disk").at().setS( TSYS::strSepParse(val,p_cnt+2,'|'), NULL, true );
		prm->vlAt("t").at().setI( atoi(TSYS::strSepParse(val,p_cnt+3,'|').c_str()), NULL, true );
		prm->vlAt("ed").at().setS( TSYS::strSepParse(val,p_cnt+4,'|'), NULL, true );
		break;
	    }	    
            p_cnt+=5;
	}
    }    
    catch( TError err ) { Mess->put(err.cat.c_str(),TMess::Error,err.mess.c_str()); }
}

string Hddtemp::getHDDTemp( )
{
    string val;
    char buf[20];
    
    ResAlloc res(m_res,true);
    //Check connect and start
    if( !((TTipTransport &)SYS->transport().at().modAt(t_tr).at()).outPresent(n_tr) )
    {
        ((TTipTransport &)SYS->transport().at().modAt(t_tr).at()).outAdd(n_tr);
        (((TTipTransport &)SYS->transport().at().modAt(t_tr).at()).outAt(n_tr)).at().lName() = mod->I18N("Parametr Hddtemp");
        (((TTipTransport &)SYS->transport().at().modAt(t_tr).at()).outAt(n_tr)).at().addr() = "TCP:127.0.0.1:7634";
        (((TTipTransport &)SYS->transport().at().modAt(t_tr).at()).outAt(n_tr)).at().toStart(true);
    }
    if( (((TTipTransport &)SYS->transport().at().modAt(t_tr).at()).outAt(n_tr)).at().startStat() )
	(((TTipTransport &)SYS->transport().at().modAt(t_tr).at()).outAt(n_tr)).at().stop();
    (((TTipTransport &)SYS->transport().at().modAt(t_tr).at()).outAt(n_tr)).at().start();
    
    //Request
    int len;
    do{	    
        len = (((TTipTransport &)SYS->transport().at().modAt(t_tr).at()).outAt(n_tr)).at().messIO(NULL,0,buf,sizeof(buf),1);
        val.append(buf,len);
    }while( len == sizeof(buf) );
    
    (((TTipTransport &)SYS->transport().at().modAt(t_tr).at()).outAt(n_tr)).at().stop();
    
    return val;    
}

void Hddtemp::makeActiveDA( TController *a_cntr )
{
    string ap_nm = "Temperature_hd";
    
    vector<string> list;
    dList(list);
    try
    {
	for( int i_hd = 0; i_hd < list.size(); i_hd++ )
	{   
	    string hddprm = ap_nm+TSYS::int2str(i_hd);
	    if(!a_cntr->present(hddprm))
	    {
		a_cntr->add(hddprm,0);
		a_cntr->at(hddprm).at().cfg("TYPE").setS(id());
		a_cntr->at(hddprm).at().cfg("SUBT").setS(list[i_hd]);
		a_cntr->at(hddprm).at().cfg("EN").setB(true);    
	    }
	}
    }
    catch( TError err ) { Mess->put(err.cat.c_str(),TMess::Error,err.mess.c_str()); }			    
}
