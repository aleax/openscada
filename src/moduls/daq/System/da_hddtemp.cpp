
//OpenSCADA module DAQ.System file: da_hddtemp.cpp
/***************************************************************************
 *   Copyright (C) 2005-2023 by Roman Savochenko, <roman@oscada.org>       *
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
Hddtemp::Hddtemp( ) : tTr("Sockets"), nTr("HDDTemp")
{
    //HDD value structure
    fldAdd(new TFld("disk",trS("Name"),TFld::String,TFld::NoWrite));
    fldAdd(new TFld("ed",trS("Measure unit"),TFld::String,TFld::NoWrite));
    fldAdd(new TFld("t",trS("Temperature"),TFld::Integer,TFld::NoWrite,"0"));
}

Hddtemp::~Hddtemp( )
{
    try {
	if(((TTypeTransport &)SYS->transport().at().modAt(tTr).at()).outPresent(nTr))
	    ((TTypeTransport &)SYS->transport().at().modAt(tTr).at()).outDel(nTr);
    } catch(TError &err) { }
}

void Hddtemp::dList( vector<string> &list, TMdPrm *prm )
{
    try {
	string val = getHDDTemp(), cEl;
	list.clear();
	for(int pCnt = 0; (cEl=TSYS::strSepParse(val,pCnt+1,'|')).size(); pCnt += 5)
	    list.push_back(cEl);
    }
    catch(TError &err) { 
	/*mess_err(err.cat.c_str(),"%s",err.mess.c_str());*/
	if(SYS->transport().at().at(tTr).at().outPresent(nTr)) SYS->transport().at().at(tTr).at().outDel(nTr);
    }
}

void Hddtemp::getVal( TMdPrm *prm )
{
    bool devOK = false;
    try {
	string	dev = prm->cfg("SUBT").getS(),
		val = getHDDTemp(),
		cEl, tVl;
	for(int pCnt = 0; (cEl=TSYS::strSepParse(val,pCnt+1,'|')).size(); pCnt += 5)
	    if(cEl == dev) {
		prm->vlAt("disk").at().setS(parseName(TSYS::strSepParse(val,pCnt+2,'|')), 0, true);
		tVl = TSYS::strSepParse(val,pCnt+3, '|');
		prm->vlAt("t").at().setI(((tVl=="NA")?EVAL_INT:s2i(tVl)), 0, true);
		prm->vlAt("ed").at().setS(TSYS::strSepParse(val,pCnt+4,'|'), 0, true);
		devOK = true;
		break;
	    }
    } catch(TError &err) { mess_err(err.cat.c_str(),"%s",err.mess.c_str()); }

    if(devOK) prm->daErr = "";
    else if(!prm->daErr.getVal().size()) {
	prm->setEval();
	prm->daErr = _("10:Device is not available.");
    }
}

string Hddtemp::getHDDTemp( )
{
    string val;
    char buf[20];

    ResAlloc res(mRes, true);
    //Check connect and start
    if(!SYS->transport().at().at(tTr).at().outPresent(nTr)) SYS->transport().at().at(tTr).at().outAdd(nTr);
    AutoHD<TTransportOut> tr = SYS->transport().at().at(tTr).at().outAt(nTr);

    MtxAlloc resN(tr.at().reqRes(), true);
    tr.at().setName(_("Parameter Hddtemp"));
    tr.at().setAddr("127.0.0.1:7634");
    tr.at().setTimings("5:0.01");
    tr.at().modifClr();

    if(tr.at().startStat()) tr.at().stop();
    tr.at().start();

    //Request
    int len;
    do {
	len = tr.at().messIO(NULL, 0, buf, sizeof(buf));
	val.append(buf,len);
    }while(len);

    tr.at().stop();

    return val;
}

string Hddtemp::parseName( const string &val )
{
    int beg = -1, end = -1;

    for(unsigned iS = 0; iS < val.size(); iS++)
	if(val[iS] != ' ' &&  val[iS] != '\t' && isalnum(val[iS])) {
	    if(beg < 0) beg = iS;
	    end = iS;
	}

    return (beg >= 0) ? val.substr(beg,end-beg+1) : "";
}
