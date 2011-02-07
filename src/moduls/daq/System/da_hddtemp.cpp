
//OpenSCADA system module DAQ.System file: da_hddtemp.cpp
/***************************************************************************
 *   Copyright (C) 2005-2008 by Roman Savochenko                           *
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

#include <tsys.h>

#include "os_contr.h"
#include "da_hddtemp.h"

using namespace SystemCntr;

//*************************************************
//* HddTemp                                       *
//*************************************************
Hddtemp::Hddtemp( ) : t_tr("Sockets"), n_tr("HDDTemp")
{
    //> HDD value structure
    fldAdd( new TFld("disk",_("Name"),TFld::String,TFld::NoWrite) );
    fldAdd( new TFld("ed",_("Measure unit"),TFld::String,TFld::NoWrite) );
    fldAdd( new TFld("t",_("Temperature"),TFld::Integer,TFld::NoWrite,"0") );
}

Hddtemp::~Hddtemp( )
{
    try
    {
	if(((TTipTransport &)SYS->transport().at().modAt(t_tr).at()).outPresent(n_tr))
	    ((TTipTransport &)SYS->transport().at().modAt(t_tr).at()).outDel(n_tr);
    }catch(TError err) { }
}

void Hddtemp::init( TMdPrm *prm )
{
    TCfg &c_subt = prm->cfg("SUBT");

    //- Create Config -
    c_subt.fld().setDescr(_("Disk"));

    vector<string> list;
    dList(list);
    string dls;
    for( unsigned i_l = 0; i_l < list.size(); i_l++ )
	dls = dls+list[i_l]+";";
    c_subt.fld().setValues(dls);
    c_subt.fld().setSelNames(dls);

    try{ c_subt.getSEL(); }
    catch(...)
    {
	if( list.size() ) c_subt.setS(list[0]);
    }
}

void Hddtemp::dList( vector<string> &list )
{
    try
    {
	string val = getHDDTemp( ), c_el;
	list.clear();
	for( int p_cnt = 0; (c_el=TSYS::strSepParse(val,p_cnt+1,'|')).size(); p_cnt+=5 )
	    list.push_back(c_el);
    }
    catch( TError err ) { mess_err(err.cat.c_str(),"%s",err.mess.c_str()); }
}

void Hddtemp::getVal( TMdPrm *prm )
{
    try
    {
	string	dev = prm->cfg("SUBT").getS(),
		val = getHDDTemp( ),
		c_el;
	for(int p_cnt = 0; (c_el=TSYS::strSepParse(val,p_cnt+1,'|')).size(); p_cnt+=5)
	    if(c_el == dev)
	    {
		prm->vlAt("disk").at().setS(parseName(TSYS::strSepParse(val,p_cnt+2,'|')), 0, true);
		prm->vlAt("t").at().setI(atoi(TSYS::strSepParse(val,p_cnt+3,'|').c_str()), 0, true);
		prm->vlAt("ed").at().setS(TSYS::strSepParse(val,p_cnt+4,'|'), 0, true);
		break;
	    }
    }
    catch(TError err) { mess_err(err.cat.c_str(),"%s",err.mess.c_str()); }
}

string Hddtemp::getHDDTemp( )
{
    string val;
    char buf[20];

    ResAlloc res(m_res,true);
    //- Check connect and start -
    if( !SYS->transport().at().at(t_tr).at().outPresent(n_tr) )
    {
	SYS->transport().at().at(t_tr).at().outAdd(n_tr);
	SYS->transport().at().at(t_tr).at().outAt(n_tr).at().setName(_("Parameter Hddtemp"));
	SYS->transport().at().at(t_tr).at().outAt(n_tr).at().setAddr("TCP:127.0.0.1:7634");
    }
    if( SYS->transport().at().at(t_tr).at().outAt(n_tr).at().startStat() )
	SYS->transport().at().at(t_tr).at().outAt(n_tr).at().stop();
    try{ SYS->transport().at().at(t_tr).at().outAt(n_tr).at().start(); }
    catch(TError err)
    {
	SYS->transport().at().at(t_tr).at().outDel(n_tr,true);
	throw;
    }

    //- Request -
    int len;
    do{
        len = SYS->transport().at().at(t_tr).at().outAt(n_tr).at().messIO(NULL,0,buf,sizeof(buf),1);
        val.append(buf,len);
    }while( len == sizeof(buf) );

    SYS->transport().at().at(t_tr).at().outAt(n_tr).at().stop();

    return val;
}

void Hddtemp::makeActiveDA( TMdContr *a_cntr )
{
    string ap_nm = "Temperature_hd";

    vector<string> list;
    dList(list);
    try
    {
	for( unsigned i_hd = 0; i_hd < list.size(); i_hd++ )
	{
	    string hddprm = ap_nm+TSYS::int2str(i_hd);
	    if(!a_cntr->present(hddprm))
	    {
		a_cntr->add(hddprm,0);
		a_cntr->at(hddprm).at().setName(_("HD temperature: ")+TSYS::int2str(i_hd));
		a_cntr->at(hddprm).at().autoC(true);
		a_cntr->at(hddprm).at().cfg("TYPE").setS(id());
		a_cntr->at(hddprm).at().cfg("SUBT").setS(list[i_hd]);
		a_cntr->at(hddprm).at().cfg("EN").setB(true);
	    }
	}
    }
    catch( TError err ) { mess_err(err.cat.c_str(),"%s",err.mess.c_str()); }
}

string Hddtemp::parseName( const string &val )
{
    int beg = -1, end = -1;

    for(unsigned i_s = 0; i_s < val.size(); i_s++)
	if(val[i_s] != ' ' &&  val[i_s] != '\t' && isalnum(val[i_s]))
	{
	    if(beg < 0) beg = i_s;
	    end = i_s;
	}

    return (beg>=0) ? val.substr(beg,end-beg+1) : "";
}
