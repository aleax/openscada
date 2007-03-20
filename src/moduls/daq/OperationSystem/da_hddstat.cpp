
//OpenSCADA system module DAQ.OperationSystem file: da_hddstat.cpp
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
#include "da_hddstat.h"

using namespace SystemCntr;

//======================================================================
//==== HddStat
//======================================================================
HddStat::HddStat( )
{
    fldAdd( new TFld("rd",_("Read (Kb)"),TFld::Real,TFld::NoWrite,"",TSYS::real2str(EVAL_REAL).c_str()) );
    fldAdd( new TFld("wr",_("Write (Kb)"),TFld::Real,TFld::NoWrite,"",TSYS::real2str(EVAL_REAL).c_str()) );
}

HddStat::~HddStat( )
{

}

void HddStat::init( TMdPrm *prm )
{
    TCfg &c_subt = prm->cfg("SUBT");
    
    //Create Config
    c_subt.fld().setDescr(_("Disk(part)"));

    vector<string> list;
    dList(list,true);
    string dls;
    for( int i_l = 0; i_l < list.size(); i_l++ )
	dls=dls+list[i_l]+";";
    c_subt.fld().setValues(dls);
    c_subt.fld().setSelNames(dls);    
	
    try{ c_subt.getSEL(); }
    catch(...)
    {
	if( list.size() ) c_subt.setS(list[0]);    
    }
}

void HddStat::dList( vector<string> &list, bool part )
{    
    int major, minor;
    char name[11];
    char buf[256];
    
    FILE *f = fopen("/proc/partitions","r");
    if( f == NULL ) return;
	
    while( fgets(buf,sizeof(buf),f) != NULL )
    {
	if( sscanf(buf,"%d %d %*d %10s",&major,&minor,name) != 3 ) continue;
	if( !part && minor != 0 ) continue;
	if( !strncmp(name,"md",2) )	continue;
	list.push_back(name);	
    }
    fclose(f);
}

void HddStat::getVal( TMdPrm *prm )
{    
    unsigned int rd,rd1,wr,wr1;
    char sc_pat[50], buf[256];
    
    string dev = prm->cfg("SUBT").getS();
    FILE *f = fopen("/proc/diskstats","r");
    if(f)
    {	
	//major minor name rio rmerge rsect ruse wio wmerge wsect wuse running use aveq
        //--or for a partition--
	//major minor name rio rsect wio wsect
	snprintf(sc_pat,sizeof(sc_pat),"%%*d %%*d %s %%*d %%lu %%lu %%lu %%*d %%*d %%lu",dev.c_str());
	while( fgets(buf,sizeof(buf),f) != NULL )
	{	
	    int n = sscanf(buf,sc_pat,&rd,&rd1,&wr,&wr1);
            if( !n ) continue;
	    if(n == 4)
    	    {
                rd = rd1;
            	wr = wr1;
            }
	    prm->vlAt("rd").at().setR((double)rd/2.0,0,true);
	    prm->vlAt("wr").at().setR((double)wr/2.0,0,true);
	    break;
	}
	fclose(f);
	return;
    }
    f = fopen("/proc/partitions","r");
    if(f)
    {
	//major minor #blocks name rio rmerge rsect ruse wio wmerge wsect wuse running use aveq					                                
	snprintf(sc_pat,sizeof(sc_pat),"%%*d %%*d %%*d %s %%*d %%*d %%lu %%*d %%*d %%*d %%lu",dev.c_str());
	while( fgets(buf,sizeof(buf),f) != NULL )
	    if( sscanf(buf,sc_pat,&rd,&wr) == 2 ) break;
	prm->vlAt("rd").at().setR((double)rd/2.0,0,true);
        prm->vlAt("wr").at().setR((double)wr/2.0,0,true);		    	
	fclose(f);
	return;    
    }	
}

void HddStat::setEVAL( TMdPrm *prm )
{
    prm->vlAt("rd").at().setR(EVAL_REAL,0,true);
    prm->vlAt("wr").at().setR(EVAL_REAL,0,true);
}

void HddStat::makeActiveDA( TMdContr *a_cntr )
{
    string ap_nm = "Statistic_";
	
    vector<string> list;
    dList(list);
    for( int i_hd = 0; i_hd < list.size(); i_hd++ )
    {
        string hddprm = ap_nm+list[i_hd];
        if(!a_cntr->present(hddprm))
        {
            a_cntr->add(hddprm,0);
	    a_cntr->at(hddprm).at().setName(_("HD statistic: ")+list[i_hd]);
	    a_cntr->at(hddprm).at().autoC(true);
            a_cntr->at(hddprm).at().cfg("TYPE").setS(id());
    	    a_cntr->at(hddprm).at().cfg("SUBT").setS(list[i_hd]);
            a_cntr->at(hddprm).at().cfg("EN").setB(true);
        }
    }
}

