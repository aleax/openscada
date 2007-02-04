
//OpenSCADA system module DAQ.OperationSystem file: da_netstat.cpp
/***************************************************************************
 *   Copyright (C) 2005-2006 by Roman Savochenko                           *
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

#include <sys/ioctl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <errno.h>

#include <tsys.h>

#include "os_contr.h"
#include "da_netstat.h"

using namespace SystemCntr;

//======================================================================
//==== HddStat
//======================================================================
NetStat::NetStat( )
{
    fldAdd( new TFld("rcv",_("Receive (Kb)"),TFld::Integer,TFld::NoWrite,"",TSYS::int2str(EVAL_INT).c_str()) );
    fldAdd( new TFld("trns",_("Transmit (Kb)"),TFld::Integer,TFld::NoWrite,"",TSYS::int2str(EVAL_INT).c_str()) );
}

NetStat::~NetStat( )
{

}

void NetStat::init( TMdPrm *prm )
{
    TCfg &c_subt = prm->cfg("SUBT");
    
    //Create Config
    c_subt.fld().descr(_("Interface"));

    vector<string> list;
    dList(list,true);
    string ifls;
    for( int i_l = 0; i_l < list.size(); i_l++ )
	ifls=ifls+list[i_l]+";";
    c_subt.fld().values(ifls);
    c_subt.fld().selNames(ifls);	
	
    try{ c_subt.getSEL(); }
    catch(...)
    {
	if( list.size() ) c_subt.setS(list[0]);    
    }
}

void NetStat::dList( vector<string> &list, bool part )
{    
    unsigned int rcv, trns;
    char name[11] = "",
	 buf[256] = "";
    
    FILE *f = fopen("/proc/net/dev","r");
    if( f == NULL ) return;
	
    while( fgets(buf,sizeof(buf),f) != NULL )
    {
	for(int i=0; i < sizeof(buf); i++ )
            if( buf[i] == ':' ) buf[i] = ' ';			
	if( sscanf(buf,"%10s %lu %*d %*d %*d %*d %*d %*d %*d %lu",name,&rcv,&trns) != 3 ) continue;
	list.push_back(name);	
    }
    fclose(f);
}

void NetStat::getVal( TMdPrm *prm )
{    
    unsigned int rcv, trns;
    char sc_pat[50] = "", 
	 buf[256] = "";
    
    string dev = prm->cfg("SUBT").getS();
    FILE *f = fopen("/proc/net/dev","r");
    if(f)
    {	
	snprintf(sc_pat,sizeof(sc_pat)," %s %%lu %%*d %%*d %%*d %%*d %%*d %%*d %%*d %%lu",dev.c_str());
	while( fgets(buf,sizeof(buf),f) != NULL )
	{	
	    for(int i=0; i < sizeof(buf); i++ ) 
		if( buf[i] == ':' ) buf[i] = ' ';
	    int n = sscanf(buf,sc_pat,&rcv,&trns);
            if( !n ) continue;
	    prm->vlAt("rcv").at().setI(rcv/1024,0,true);
	    prm->vlAt("trns").at().setI(trns/1024,0,true);
	    break;
	}
	fclose(f);
	return;
    }
}

void NetStat::setEVAL( TMdPrm *prm )
{
    prm->vlAt("rcv").at().setI(EVAL_INT,0,true);
    prm->vlAt("trns").at().setI(EVAL_INT,0,true);
}

void NetStat::makeActiveDA( TMdContr *a_cntr )
{
    string ap_nm = "Interface_";
	
    vector<string> list;
    dList(list);
    for( int i_hd = 0; i_hd < list.size(); i_hd++ )
    {
        string intprm = ap_nm+list[i_hd];
        if(!a_cntr->present(intprm))
        {
            a_cntr->add(intprm,0);
	    a_cntr->at(intprm).at().name(_("Interface statistic: ")+list[i_hd]);
	    a_cntr->at(intprm).at().autoC(true);
            a_cntr->at(intprm).at().cfg("TYPE").setS(id());
    	    a_cntr->at(intprm).at().cfg("SUBT").setS(list[i_hd]);
            a_cntr->at(intprm).at().cfg("EN").setB(true);
        }
    }
}

