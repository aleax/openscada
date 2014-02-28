
//OpenSCADA system module DAQ.System file: da_ups.cpp
/***************************************************************************
 *   Copyright (C) 2014 by Roman Savochenko                                *
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

#include <tsys.h>

#include "os_contr.h"
#include "da_ups.h"

using namespace SystemCntr;

//*************************************************
//* UPS                                           *
//*************************************************
UPS::UPS( ) : tTr("Sockets"), nTr("sys_UPS")	{ }

UPS::~UPS( )	{ }

void UPS::init( TMdPrm *prm )
{
    prm->daData = new tval();
    prm->vlElemAtt(&((tval*)prm->daData)->els);

    TCfg &c_subt = prm->cfg("SUBT");
    c_subt.fld().setDescr(_("UPS"));
    c_subt.fld().setFlg(c_subt.fld().flg()|TFld::SelEdit);
    c_subt.setS("localhost:3493");

    string dls = upsList(c_subt.getS());
    c_subt.fld().setValues(dls);
    c_subt.fld().setSelNames(dls);
    if(dls.size()) c_subt.setS(TSYS::strParse(dls,0,";"));
}

void UPS::deInit( TMdPrm *prm )
{
    TCfg &c_subt = prm->cfg("SUBT");
    c_subt.fld().setFlg(c_subt.fld().flg()&~(TFld::SelEdit));

    prm->vlElemDet(&((tval*)prm->daData)->els);
    delete (tval*)prm->daData;
    prm->daData = NULL;
}

void UPS::cfgChange( TCfg &i_cfg )
{
    if(i_cfg.name() == "SUBT")
    {
	string dls = upsList(i_cfg.getS());
	i_cfg.fld().setValues(dls);
	i_cfg.fld().setSelNames(dls);
    }
}

string UPS::upsList( const string &addr )
{
    string rez, host, val, c_el;
    char name[51] = "", dscr[256] = "";

    try
    {
	if((host=TSYS::strParse(TSYS::strParse(addr,0," "),1,"@")).empty()) host = TSYS::strParse(addr,0," ");
	val = reqUPS(host,"LIST UPS\x0A");
	for(int off = 0, lstSec = false; (c_el=TSYS::strLine(val,0,&off)).size(); )
	    if(c_el == "BEGIN LIST UPS") lstSec = true;
	    else if(c_el == "END LIST UPS")	break;
	    else if(lstSec)
		if(sscanf(c_el.c_str(),"UPS %50s \"%255[^\"]s\"",name,dscr) == 2)
		    rez = rez + name+"@"+host+" ("+dscr+");";
    }
    catch(TError err) { /*mess_err(err.cat.c_str(),"%s",err.mess.c_str());*/ }

    return rez;
}

void UPS::getVal( TMdPrm *prm )
{
    vector<string> als;
    try
    {
	string c_el, addr = TSYS::strParse(prm->cfg("SUBT").getS(),0," ");
	int aOff = 0;
	string UPS = TSYS::strParse(addr, 0, "@", &aOff);
	char var[51] = "", vVal[256] = "";
	if(aOff < (int)addr.size())
	{
	    string val = reqUPS(addr,"LIST VAR "+UPS+"\x0A");
	    for(int off = 0, lstSec = false; (c_el=TSYS::strLine(val,0,&off)).size(); )
		if(c_el == "BEGIN LIST VAR "+UPS) lstSec = true;
		else if(c_el == "END LIST VAR "+UPS)	break;
		else if(lstSec)
		{
		    if(sscanf(c_el.c_str(),"VAR %*s %50s \"%255[^\"]s\"",var,vVal) != 2) continue;
		    string aid = TSYS::strEncode(var,TSYS::oscdID);
		    if(!prm->vlPresent(aid)) ((tval*)prm->daData)->els.fldAdd(new TFld(aid.c_str(),var,TFld::String,TFld::NoWrite));
		    prm->vlAt(aid).at().setS(vVal, 0, true);
		    als.push_back(aid);
		}
	}
    }
    catch(TError err) { mess_err(err.cat.c_str(),"%s",err.mess.c_str()); }

    if(als.size())
    {
	prm->daErr = "";
	//Check for delete DAQ parameter's attributes
	for(int i_p = 0; i_p < (int)fldSize(); i_p++)
	{
	    unsigned i_l;
	    for(i_l = 0; i_l < als.size(); i_l++)
		if(fldAt(i_p).name() == als[i_l])
		    break;
	    if(i_l >= als.size())
		try { fldDel(i_p); i_p--; }
		catch(TError err) { mess_warning(err.cat.c_str(),err.mess.c_str()); }
	}
    }
    else if(!prm->daErr.getVal().size())
    {
        prm->setEval();
        prm->daErr = _("10:Device is not available.");
    }
}

string UPS::reqUPS( const string &addr, const string &req )
{
    string val, host;
    char buf[1024];

    if((host=TSYS::strParse(TSYS::strParse(addr,0," "),1,"@")).empty()) host = TSYS::strParse(addr,0," ");

    //> Check connect and start
    if(!SYS->transport().at().at(tTr).at().outPresent(nTr)) SYS->transport().at().at(tTr).at().outAdd(nTr);
    AutoHD<TTransportOut> tr = SYS->transport().at().at(tTr).at().outAt(nTr);

    ResAlloc resN(tr.at().nodeRes(), true);
    tr.at().setName(_("UPS"));
    tr.at().setAddr("TCP:"+host);
    tr.at().setTimings("3:0.01");
    if(!tr.at().startStat()) tr.at().start();

    //> Request
    int resp_len = tr.at().messIO(req.data(), req.size(), buf, sizeof(buf), 0, true);
    val.assign(buf, resp_len);

    //> Wait tail
    while(resp_len)
    {
	try{ resp_len = tr.at().messIO(NULL, 0, buf, sizeof(buf), 0, true); } catch(TError err){ break; }
	val.append(buf, resp_len);
    }

    return val;
}

void UPS::makeActiveDA( TMdContr *aCntr )
{
    string list = upsList("localhost:3493"), uEl;
    try
    {
	//UPS list process
	for(int off = 0; (uEl=TSYS::strParse(list,0,";",&off)).size(); )
	{
	    vector<string> pLs;
	    // Find propper parameter's object
	    aCntr->list(pLs);

	    unsigned i_p;
	    for(i_p = 0; i_p < pLs.size(); i_p++)
	    {
		AutoHD<TMdPrm> p = aCntr->at(pLs[i_p]);
		if(p.at().cfg("TYPE").getS() == id() && TSYS::strParse(p.at().cfg("SUBT"),0," ") == TSYS::strParse(uEl,0," "))
		    break;
	    }
	    if(i_p < pLs.size()) continue;

	    string upsAprm = "UPS";
	    while(aCntr->present(upsAprm)) upsAprm = TSYS::strLabEnum(upsAprm);
	    aCntr->add(upsAprm, 0);
	    AutoHD<TMdPrm> dprm = aCntr->at(upsAprm);
	    dprm.at().setName(_("UPS: ")+uEl);
	    dprm.at().autoC(true);
	    dprm.at().cfg("TYPE").setS(id());
	    dprm.at().cfg("SUBT").setS(uEl);
	    dprm.at().cfg("EN").setB(true);
	    if(aCntr->enableStat()) dprm.at().enable();
	}
    }
    catch(TError err) { mess_err(err.cat.c_str(),"%s",err.mess.c_str()); }
}
