
//OpenSCADA system module DAQ.System file: da_cpu.cpp
/***************************************************************************
 *   Copyright (C) 2005-2007 by Roman Savochenko                           *
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

#include <sys/times.h>

#include <tsys.h>

#include "os_contr.h"
#include "da_cpu.h"

using namespace SystemCntr;

//*************************************************
//* CPU                                           *
//*************************************************
CPU::CPU( )
{
    //- CPU value structure -
    fldAdd( new TFld("load",_("Load (%)"),TFld::Real,TFld::NoWrite,"",TSYS::real2str(EVAL_REAL).c_str()) );
    fldAdd( new TFld("sys",_("System (%)"),TFld::Real,TFld::NoWrite,"",TSYS::real2str(EVAL_REAL).c_str()) );
    fldAdd( new TFld("user",_("User (%)"),TFld::Real,TFld::NoWrite,"",TSYS::real2str(EVAL_REAL).c_str()) );
    fldAdd( new TFld("idle",_("Idle (%)"),TFld::Real,TFld::NoWrite,"",TSYS::real2str(EVAL_REAL).c_str()) );
}

CPU::~CPU( )
{

}

void CPU::init( TMdPrm *prm )
{
    char buf[256];
    //- Create config -
    TCfg &t_cf = prm->cfg("SUBT");
    t_cf.fld().setDescr("");

    //t_fl.selValI().push_back(-1); t_fl.selNm().push_back("OpenSCADA");

    //- Init start value -
    FILE *f = fopen("/proc/stat","r");
    if( f == NULL ) return;

    string cpuLs, cpuLsNm;
    while( fgets(buf,sizeof(buf),f) != NULL )
    {
	int n_cpu;
	if( sscanf(buf,"cpu%d",&n_cpu) )
	{
	    if( !isdigit(buf[3]) )
	    {
		cpuLs=cpuLs+"gen;";
		cpuLsNm=cpuLsNm+_("General")+";";
		c_vls.push_back(tval());
	    }
	    else
	    {
		cpuLs=cpuLs+TSYS::int2str(n_cpu)+";";
		cpuLsNm=cpuLsNm+TSYS::int2str(n_cpu)+";";
		c_vls.push_back(tval());
	    }
	}
    }
    t_cf.fld().setValues(cpuLs);
    t_cf.fld().setSelNames(cpuLsNm);

    fclose(f);
    try{ t_cf.getSEL(); }
    catch(...){ t_cf.setS("gen"); }
}

void CPU::getVal( TMdPrm *prm )
{
    long unsigned user,nice,sys,idle,iowait;
    float sum;

    string trg = prm->cfg("SUBT").getS();

    /*if( trg == "OpenSCADA" )
    {
	struct tms p_tm;
	clock_t cur_tm = times(&p_tm);
	sum  = cur_tm - m_nice;
	user = p_tm.tms_utime + p_tm.tms_cutime - m_user;
	sys  = p_tm.tms_stime + p_tm.tms_cstime - m_sys;

	prm->vlAt("value").at().setR( 100.0*(float(user + sys))/sum,NULL,true);
	prm->vlAt("sys").at().setR( 100.0*(float(sys))/sum,NULL,true);
	prm->vlAt("user").at().setR( 100.0*(float(user))/sum,NULL,true);
	prm->vlAt("idle").at().setR( 100.0*(float(sum - user - sys))/sum,NULL,true);

	m_nice = cur_tm;
	m_user = p_tm.tms_utime + p_tm.tms_cutime;
	m_sys  = p_tm.tms_stime + p_tm.tms_cstime;

	return;
    }*/

    //- File /proc/stat scan -
    int n_el;	//CPU number
    int n = 0;
    char buf[256];
    FILE *f = fopen("/proc/stat","r");
    if( f == NULL ) return;
    while( fgets(buf,sizeof(buf),f) != NULL )
    {
	if( trg == "gen" )
	{
	    n = sscanf(buf,"cpu %lu %lu %lu %lu %lu\n",&user,&nice,&sys,&idle,&iowait);
	    n_el=0;
	}
	else if( isdigit(trg[0]) )
	{
	    n = sscanf(buf,(string("cpu")+trg+" %lu %lu %lu %lu %lu\n").c_str(),&user,&nice,&sys,&idle,&iowait);
	    n_el=atoi(trg.c_str())+1;
	}
	if( n )
	{
	    if( n == 5 ) idle += iowait;
	    sum = (float)(user+nice+sys+idle-c_vls[n_el].user-c_vls[n_el].nice-c_vls[n_el].sys-c_vls[n_el].idle);
	    prm->vlAt("load").at().setR( 100.0*(float(user+sys-c_vls[n_el].user-c_vls[n_el].sys))/sum,0,true);
	    prm->vlAt("sys").at().setR( 100.0*(float(sys-c_vls[n_el].sys))/sum,0,true);
	    prm->vlAt("user").at().setR( 100.0*(float(user-c_vls[n_el].user))/sum,0,true);
	    prm->vlAt("idle").at().setR( 100.0*(float(idle-c_vls[n_el].idle))/sum,0,true);
	    c_vls[n_el].user = user; 
	    c_vls[n_el].nice = nice; 
	    c_vls[n_el].sys  = sys; 
	    c_vls[n_el].idle = idle;
	    break;
	}
    }
    fclose(f);
}

void CPU::makeActiveDA( TMdContr *a_cntr )
{
    char buf[256];

    FILE *f = fopen("/proc/stat","r");
    if( f == NULL ) return;

    //- Check for allow CPU -
    while( fgets(buf,sizeof(buf),f) != NULL )
    {
	int n_cpu;
	if( sscanf(buf,"cpu%d",&n_cpu) )
	{
	    if( !isdigit(buf[3]) )
	    {
		if(!a_cntr->present("CPULoad"))
		{
		    a_cntr->add("CPULoad",0);
		    a_cntr->at("CPULoad").at().setName(_("Full CPU Load"));
		    a_cntr->at("CPULoad").at().autoC(true);
		    a_cntr->at("CPULoad").at().cfg("TYPE").setS(id());
		    a_cntr->at("CPULoad").at().cfg("SUBT").setS("gen");
		    a_cntr->at("CPULoad").at().cfg("EN").setB(true);
		}
	    }
	    else
	    {
		string ncpu = "CPU"+TSYS::int2str(n_cpu)+"Load";
		if(!a_cntr->present(ncpu))
		{
		    a_cntr->add(ncpu,0);
		    a_cntr->at(ncpu).at().setName(_("CPU Load :")+TSYS::int2str(n_cpu));
		    a_cntr->at(ncpu).at().autoC(true);
		    a_cntr->at(ncpu).at().cfg("TYPE").setS(id());
		    a_cntr->at(ncpu).at().cfg("SUBT").setS(TSYS::int2str(n_cpu));
		    a_cntr->at(ncpu).at().cfg("EN").setB(true);
		}
	    }
	}
    }
    fclose(f);
}
