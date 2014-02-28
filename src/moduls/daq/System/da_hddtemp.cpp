
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
    fldAdd(new TFld("disk",_("Name"),TFld::String,TFld::NoWrite));
    fldAdd(new TFld("ed",_("Measure unit"),TFld::String,TFld::NoWrite));
    fldAdd(new TFld("t",_("Temperature"),TFld::Integer,TFld::NoWrite,"0"));
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

    //> Create Configuration
    c_subt.fld().setDescr(_("Disk"));

    vector<string> list;
    dList(list);
    string dls;
    for(unsigned i_l = 0; i_l < list.size(); i_l++)
	dls += list[i_l]+";";
    c_subt.fld().setValues(dls);
    c_subt.fld().setSelNames(dls);

    if(list.size() && !TRegExp("(^|;)"+c_subt.getS()+";").test(dls)) c_subt.setS(list[0]);
}

void Hddtemp::dList( vector<string> &list )
{
    try
    {
	string val = getHDDTemp(), c_el;
	list.clear();
	for(int p_cnt = 0; (c_el=TSYS::strSepParse(val,p_cnt+1,'|')).size(); p_cnt += 5)
	    list.push_back(c_el);
    }
    catch( TError err ) { /*mess_err(err.cat.c_str(),"%s",err.mess.c_str());*/ }
}

void Hddtemp::getVal( TMdPrm *prm )
{
    bool devOK = false;
    try
    {
	string	dev = prm->cfg("SUBT").getS(),
		val = getHDDTemp(),
		c_el, t_vl;
	for(int p_cnt = 0; (c_el=TSYS::strSepParse(val,p_cnt+1,'|')).size(); p_cnt += 5)
	    if(c_el == dev)
	    {
		prm->vlAt("disk").at().setS(parseName(TSYS::strSepParse(val,p_cnt+2,'|')), 0, true);
		t_vl = TSYS::strSepParse(val,p_cnt+3, '|');
		prm->vlAt("t").at().setI(((t_vl=="NA")?EVAL_INT:atoi(t_vl.c_str())), 0, true);
		prm->vlAt("ed").at().setS(TSYS::strSepParse(val,p_cnt+4,'|'), 0, true);
		devOK = true;
		break;
	    }
    }
    catch(TError err) { mess_err(err.cat.c_str(),"%s",err.mess.c_str()); }

    if(devOK) prm->daErr = "";
    else if(!prm->daErr.getVal().size())
    {
        prm->setEval();
        prm->daErr = _("10:Device is not available.");
    }
}

string Hddtemp::getHDDTemp( )
{
    string val;
    char buf[20];

    ResAlloc res(m_res, true);
    //> Check connect and start
    if(!SYS->transport().at().at(t_tr).at().outPresent(n_tr)) SYS->transport().at().at(t_tr).at().outAdd(n_tr);
    AutoHD<TTransportOut> tr = SYS->transport().at().at(t_tr).at().outAt(n_tr);

    ResAlloc resN(tr.at().nodeRes(), true);
    tr.at().setName(_("Parameter Hddtemp"));
    tr.at().setAddr("TCP:127.0.0.1:7634");
    tr.at().setTimings("5:0.01");

    if(tr.at().startStat()) tr.at().stop();
    tr.at().start();

    //> Request
    int len;
    do{
        len = tr.at().messIO(NULL, 0, buf, sizeof(buf), 0, true);
        val.append(buf,len);
    }while(len);

    tr.at().stop();

    return val;
}

void Hddtemp::makeActiveDA( TMdContr *aCntr )
{
    string ap_nm = "Temperature_hd";

    vector<string> list;
    dList(list);
    try
    {
	for(unsigned i_hd = 0; i_hd < list.size(); i_hd++)
	{
	    vector<string> pLs;
	    // Find propper parameter's object
	    aCntr->list(pLs);

	    unsigned i_p;
	    for(i_p = 0; i_p < pLs.size(); i_p++)
	    {
		AutoHD<TMdPrm> p = aCntr->at(pLs[i_p]);
		if(p.at().cfg("TYPE").getS() == id() && p.at().cfg("SUBT").getS() == list[i_hd])	break;
	    }
	    if(i_p < pLs.size()) continue;

	    string hddprm = ap_nm+TSYS::int2str(i_hd);
	    while(aCntr->present(hddprm)) hddprm = TSYS::strLabEnum(hddprm);
	    aCntr->add(hddprm,0);
	    AutoHD<TMdPrm> dprm = aCntr->at(hddprm);
	    dprm.at().setName(_("HD temperature: ")+TSYS::int2str(i_hd));
	    dprm.at().autoC(true);
	    dprm.at().cfg("TYPE").setS(id());
	    dprm.at().cfg("SUBT").setS(list[i_hd]);
	    dprm.at().cfg("EN").setB(true);
	    if(aCntr->enableStat()) dprm.at().enable();
	}
    }
    catch(TError err) { /*mess_err(err.cat.c_str(),"%s",err.mess.c_str());*/ }
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
