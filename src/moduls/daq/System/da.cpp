
//OpenSCADA module DAQ.System file: da.cpp
/***************************************************************************
 *   Copyright (C) 2023-2024 by Roman Savochenko, <roman@oscada.org>       *
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
#include "da.h"

using namespace SystemCntr;

//*************************************************
//* DA                                            *
//*************************************************
void DA::init( TMdPrm *prm, bool update )
{
    if(isDynamic() && !update) {
	prm->daData = new TElem();
	prm->vlElemAtt((TElem*)prm->daData);
    }

    //Creating the configuration
    TCfg &cSubt = prm->cfg("SUBT");
    vector<string> list;
    dList(list, prm);

    cSubt.setView(hasSubTypes());
    if(list.empty()) return;

    if(!update) cSubt.fld().setDescr("");

    string mpls, mplsNm, tVl;
    for(unsigned iL = 0; iL < list.size(); iL++) {
	int off = 0;
	mpls += (tVl=TSYS::strParse(TSYS::strParse(list[iL],0,";",&off),0,SEP_SUBID)) + ";";
	mplsNm += ((off < list[iL].size()) ? list[iL].substr(off) : tVl) + ";";
    }
    MtxAlloc res(prm->dataRes(), true);
    if(mpls == mplsNm) { cSubt.fld().setValues(""); cSubt.fld().setSelNames(mpls); }
    else { cSubt.fld().setValues(mpls); cSubt.fld().setSelNames(mplsNm); }
    res.unlock();

    if(!update && !TRegExp("(^|;)"+cSubt.getS()+";").test(mpls)) cSubt.setS("");	//!!!!: Subtype reseting after the type change
}

void DA::deInit( TMdPrm *prm )
{
    if(!prm->daData) return;

    prm->vlElemDet((TElem*)prm->daData);
    delete (TElem*)prm->daData;
    prm->daData = NULL;
}

void DA::makeActiveDA( TMdContr *aCntr, const string &dIdPref, const string &dNmPref )
{
    vector<string> list, pLs;

    dList(list);
    aCntr->list(pLs);

    //No subtypes
    if(!hasSubTypes()) {
	//Find for propper parameter's object
	unsigned iP;
	for(iP = 0; iP < pLs.size(); ++iP) {
	    AutoHD<TMdPrm> p = aCntr->at(pLs[iP]);
	    if(p.at().cfg("TYPE").getS() == id()) break;
	}
	if(iP >= pLs.size()) {
	    string itprm = dIdPref.size() ? dIdPref : "Sys";
	    while(aCntr->present(itprm)) itprm = TSYS::strLabEnum(itprm);
	    aCntr->add(itprm, 0);
	    AutoHD<TMdPrm> dprm = aCntr->at(itprm);
	    dprm.at().setName(dNmPref.size() ? dNmPref.c_str() : _("System"));
	    dprm.at().autoC(true);
	    dprm.at().cfg("TYPE").setS(id());
	    dprm.at().cfg("EN").setB(true);
	    if(aCntr->enableStat()) dprm.at().enable();
	}

	return;
    }

    //Subtypes checking
    for(unsigned iIt = 0; iIt < list.size(); ++iIt) {
	int off = 0;
	string itId = TSYS::strParse(list[iIt], 0, ";", &off), itId2, itNm;
	if((itNm=TSYS::strParse(itId,0,SEP_SUBID)) != itId) { itId2 = TSYS::strParse(itId,1,SEP_SUBID); itId = itNm; }
	else itId2 = itId;
	itNm = (off < list[iIt].size()) ? list[iIt].substr(off) : itId;

	aCntr->list(pLs);

	//Find for propper parameter's object
	unsigned iP;
	for(iP = 0; iP < pLs.size(); ++iP) {
	    AutoHD<TMdPrm> p = aCntr->at(pLs[iP]);
	    if(p.at().cfg("TYPE").getS() == id() && p.at().cfg("SUBT").getS() == TSYS::strParse(itId,0," "))	break;
	}
	if(iP < pLs.size()) continue;

	string itprm = dIdPref + TSYS::strEncode(itId2, TSYS::oscdID);
	while(aCntr->present(itprm)) itprm = TSYS::strLabEnum(itprm);
	aCntr->add(itprm, 0);
	AutoHD<TMdPrm> dprm = aCntr->at(itprm);
	dprm.at().setName((dNmPref.size()?dNmPref+": ":"") + itNm);
	dprm.at().autoC(true);
	dprm.at().cfg("TYPE").setS(id());
	dprm.at().cfg("SUBT").setS(itId);
	dprm.at().cfg("EN").setB(true);
	if(aCntr->enableStat()) dprm.at().enable();
    }
}
