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
    fldAdd( new TFld("rcv",mod->I18N("Receive (Kb)"),TFld::Dec,FLD_NWR) );
    fldAdd( new TFld("trns",mod->I18N("Transmit (Kb)"),TFld::Dec,FLD_NWR) );
}

NetStat::~NetStat( )
{

}

void NetStat::init( TMdPrm *prm )
{
    TCfg &c_subt = prm->cfg("SUBT");
    
    //Create Config
    c_subt.fld().descr() = mod->I18N("Interface");
    c_subt.fld().selValI().clear();
    c_subt.fld().selNm().clear();    

    vector<string> list;
    dList(list,true);
    for( int i_l = 0; i_l < list.size(); i_l++ )
    {
	c_subt.fld().selValI().push_back(i_l);
	c_subt.fld().selNm().push_back(list[i_l]);
    }
    if( list.size() ) c_subt.setSEL(list[0]);    
}

void NetStat::dList( vector<string> &list, bool part )
{    
    unsigned int rcv, trns;
    char name[11];
    char buf[256];
    
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
    char sc_pat[50], buf[256];
    
    string dev = prm->cfg("SUBT").getSEL();
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
	    prm->vlAt("rcv").at().setI(rcv/1024,NULL,true);
	    prm->vlAt("trns").at().setI(trns/1024,NULL,true);
	    break;
	}
	fclose(f);
	return;
    }
}

void NetStat::makeActiveDA( TController *a_cntr )
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
            a_cntr->at(intprm).at().cfg("TYPE").setS(id());
    	    a_cntr->at(intprm).at().cfg("SUBT").setI(i_hd);
            a_cntr->at(intprm).at().cfg("EN").setB(true);
        }
    }
}

