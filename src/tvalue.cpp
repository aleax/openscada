
//OpenSCADA file: tvalue.cpp
/***************************************************************************
 *   Copyright (C) 2003-2020 by Roman Savochenko, <roman@oscada.org>       *
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

#include "tsys.h"
#include "tmess.h"
#include "tarchval.h"
#include "tparamcontr.h"
#include "tcontroller.h"
#include "tvalue.h"

using namespace OSCADA;

//*************************************************
//* TValue                                        *
//*************************************************
TValue::TValue( ) : lCfg(0), mCfg(NULL)
{
    mVl = grpAdd("a_", true);

    if(mess_lev() == TMess::Debug) SYS->cntrIter(objName(), 1);
}

TValue::~TValue( )
{
    while(elem.size()) vlElemDet(elem[0]);

    if(mess_lev() == TMess::Debug) SYS->cntrIter(objName(), -1);
}

string TValue::objName( )	{ return TCntrNode::objName() + ":TValue"; }

string TValue::DAQPath( )	{ return nodeName(); }

TVal* TValue::vlNew( )		{ return new TVal(); }

void TValue::detElem( TElem *el ) { vlElemDet(el); }

void TValue::addFld( TElem *el, unsigned id_val )
{
    int iOff = lCfg;
    for(unsigned iE = 0; iE < elem.size(); iE++)
	if(elem[iE]->elName() == el->elName()) break;
	else iOff += elem[iE]->fldSize();

    TVal *vl = vlNew();
    vl->setFld(el->fldAt(id_val));
    chldAdd(mVl, vl, iOff+id_val);
}

void TValue::delFld( TElem *el, unsigned id_val )
{
    if(nodeMode() == TCntrNode::Enabled && chldPresent(mVl,el->fldAt(id_val).name()))
	chldDel(mVl, el->fldAt(id_val).name());
}

void TValue::setVlCfg( TConfig *cfg )
{
    vector<string> list;
    //Detach old configurations
    if(mCfg) {
	mCfg->cfgList(list);
	for(unsigned iCf = 0; iCf < list.size(); iCf++)
	    if(!(mCfg->cfg(list[iCf]).fld().flg()&TCfg::NoVal) && vlPresent(list[iCf])) {
		chldDel(mVl, list[iCf]);
		lCfg--;
	    }
	mCfg = NULL;
    }
    //Attach new config
    if(cfg) {
	cfg->cfgList(list);
	for(unsigned iCf = 0; iCf < list.size(); iCf++)
	    if(!(cfg->cfg(list[iCf]).fld().flg()&TCfg::NoVal) && !vlPresent(list[iCf])) {
		TVal *vl = vlNew();
		vl->setCfg(cfg->cfg(list[iCf]));
		chldAdd(mVl, vl, lCfg);
		lCfg++;
	    }
	mCfg = cfg;
    }
}

bool TValue::vlElemPresent( TElem *ValEl )
{
    for(unsigned iEl = 0; iEl < elem.size(); iEl++)
	if(elem[iEl] == ValEl) return true;
    return false;
}

void TValue::vlElemAtt( TElem *ValEl )
{
    ValEl->valAtt(this);
    for(unsigned iElem = 0; iElem < ValEl->fldSize(); iElem++)
	addFld(ValEl, iElem);
    elem.push_back(ValEl);
}

void TValue::vlElemDet( TElem *ValEl )
{
    for(unsigned iE = 0; iE < elem.size(); iE++)
	if(elem[iE] == ValEl) {
	    for(unsigned iElem = 0; iElem < elem[iE]->fldSize(); iElem++)
		delFld(elem[iE], iElem);
	    elem[iE]->valDet(this);
	    elem.erase(elem.begin()+iE);
	    return;
	}
}

TElem &TValue::vlElem( const string &name )
{
    for(unsigned iE = 0; iE < elem.size(); iE++)
	if(elem[iE]->elName() == name)
	    return *elem[iE];
    throw err_sys(_("Element '%s' is missing!"), name.c_str());
}

string TValue::chldAdd( int8_t igr, TCntrNode *node, int pos, bool noExp )
{
    string id = "";
    try {
	id = TCntrNode::chldAdd(igr, node, pos);
	if(igr == mVl) SYS->archive().at().setToUpdate();
    }
    catch(TError&) {
	if(!noExp) throw;
    }

    return id;
}

void TValue::cntrCmdProc( XMLNode *opt )
{
    vector<string> vLs;
    string a_path = opt->attr("path"), u = opt->attr("user"), l = opt->attr("lang");
    //Service commands process
    if(a_path == "/serv/attr") {	//Attributes access
	vlList(vLs);
	if(ctrChkNode(opt,"list",RWRWRW,"root",SDAQ_ID,SEC_RD)) {	//Full info attributes list
	    AutoHD<TVal> attr;
	    for(unsigned iEl = 0; iEl < vLs.size(); iEl++) {
		attr = vlAt(vLs[iEl]);
		opt->childAdd("el")->
		    setAttr("id", vLs[iEl])->
		    setAttr("nm", attr.at().fld().descr())->
		    setAttr("flg", i2s(attr.at().fld().flg()))->
		    setAttr("tp", i2s(attr.at().fld().type()))->
		    setAttr("vals", attr.at().fld().values())->
		    setAttr("names", attr.at().fld().selNames());
	    }
	}
	if(ctrChkNode(opt,"get",RWRWRW,"root",SDAQ_ID,SEC_RD)) {	//All attributes values
	    int64_t vtm;
	    string svl;
	    AutoHD<TVal> vl;
	    XMLNode *aNd;

	    //Put last attribute's values
	    bool sepReq = s2i(opt->attr("sepReq"));
	    bool hostTm = s2i(opt->attr("hostTm"));
	    int prcTm = s2i(opt->attr("prcTm"));
	    opt->setAttr("prcTm", i2s(SYS->sysTm()));
	    for(unsigned iEl = 0; iEl < vLs.size(); iEl++) {
		vl = vlAt(vLs[iEl]);
		if(!sepReq) {
		    vtm = 0; svl = vl.at().getS(&vtm);
		    aNd = opt->childAdd("el")->setAttr("id", vLs[iEl])->setText(svl);
		    if(!hostTm) aNd->setAttr("tm", ll2s(vtm));
		}
		if(!vl.at().isCfg() && (vl.at().fld().flg()&TVal::Dynamic) && vl.at().fld().len() >= prcTm)
		    opt->childAdd("del")->setAttr("id", vLs[iEl])->setAttr("name", vl.at().fld().descr())->
				setAttr("type", i2s(vl.at().fld().type()))->setAttr("flg", i2s(vl.at().fld().flg()))->
				setAttr("values", vl.at().fld().values())->setAttr("selNames", vl.at().fld().selNames());
	    }

	    //Archives requests process
	    for(int iA = 0; iA < (int)opt->childSize(); iA++) {
		aNd = opt->childGet(iA);
		if(aNd->name() == "del") continue;
		if(!sepReq && aNd->name() != "ael") break;
		if((aNd->name() != "el" && aNd->name() != "ael") || !vlPresent(aNd->attr("id")))
		{ opt->childDel(aNd); iA--; continue; }
		vl = vlAt(aNd->attr("id"));

		// Separated element request
		if(aNd->name() == "el") {
		    vtm = 0; svl = vl.at().getS(&vtm);
		    aNd->setText(svl);
		    if(!hostTm) aNd->setAttr("tm", ll2s(vtm));
		    continue;
		}

		// To archive of element request
		if(vl.at().arch().freeStat()) { opt->childDel(aNd); iA--; continue; }

		AutoHD<TVArchive> arch = vl.at().arch();
		int64_t vper = arch.at().period(BUF_ARCH_NM);
		int64_t reqBeg = s2ll(aNd->attr("tm"));	//!!!! Some spare request of the last requested value
							//     to prevent EVAL here at the connection lose
				//(s2ll(aNd->attr("tm"))/vper+1)*vper;
		int64_t vBufBeg = arch.at().begin(BUF_ARCH_NM);
		int64_t vbeg = vmax(reqBeg, vBufBeg);
		int64_t vend = arch.at().end(BUF_ARCH_NM);

		//  Longing to equivalent archivators
		if(vbeg == vBufBeg) {
		    vector<string> archLs;
		    arch.at().archivatorList(archLs);
		    for(unsigned iA1 = 0; iA1 < archLs.size(); iA1++)
			if(arch.at().period(archLs[iA1]) == vper)
			    vbeg = vmax(reqBeg, arch.at().begin(archLs[iA1]));
		}
		aNd->setAttr("tm", ll2s(vbeg))->setAttr("per", ll2s(vper));

		TValBuf buf(arch.at().valType(), 0, 0, false, true);
		arch.at().getVals(buf, vbeg, vend, "", (vend-vbeg)/vper, true);

		bool firstVal = true;
		string vl;
		for(vbeg = buf.begin(); vbeg <= buf.end(); vbeg++) {
		    vl = buf.getS(&vbeg, true);
		    if(firstVal && vl == EVAL_STR) continue;
		    if(firstVal && vl != EVAL_STR) { aNd->setAttr("tm", ll2s(vbeg)); firstVal = false; }
		    aNd->childAdd("v")->setText(vl);
		}

		if(!aNd->childSize()) { opt->childDel(aNd); iA--; }
	    }
	}
	if(ctrChkNode(opt,"set",RWRWR_,"root",SDAQ_ID,SEC_WR))		//Multi attributes set
	    for(unsigned iEl = 0; iEl < opt->childSize(); iEl++)
		vlAt(opt->childGet(iEl)->attr("id")).at().setS(opt->childGet(iEl)->text());
	return;
    }

    //Interface comands process
    // Info command process
    if(opt->name() == "info") {
	TCntrNode::cntrCmdProc(opt);
	ctrMkNode("oscada_cntr", opt, -1, "/", TSYS::strMess(_("Parameter: %s"),nodeName()), RWRWR_, "root", SDAQ_ID);
	if(ctrMkNode("area",opt,-1,"/val",_("Attributes"))) {
	    //  Add attributes list
	    vlList(vLs);
	    for(unsigned iEl = 0; iEl < vLs.size(); iEl++) {
		AutoHD<TVal> vl = vlAt(vLs[iEl]);
		XMLNode *n_e = vl.at().fld().cntrCmdMake(opt, "/val", -1, "root", SDAQ_ID, RWRWR_);
		if(n_e) {
		    string sType = _("Unknown");
		    switch(vl.at().fld().type()) {
			case TFld::String:	sType = _("String");	break;
			case TFld::Integer:	sType = _("Integer");	break;
			case TFld::Real:	sType = _("Real");	break;
			case TFld::Boolean:	sType = _("Boolean");	break;
			case TFld::Object:	sType = _("Object");	break;
			default: break;
		    }
		    if(vl.at().fld().flg()&TFld::Selectable) sType += _("-select");
		    n_e->setAttr("help",
			TSYS::strMess(_("Parameter attribute\n"
			    "  ID: '%s'\n"
			    "  Name: '%s'\n"
			    "  Type: '%s'\n"
			    "  Read only: %d"),
			    vl.at().fld().name().c_str(),vl.at().fld().descr().c_str(),sType.c_str(),(vl.at().fld().flg()&TFld::NoWrite)?1:0));
		    if(vl.at().fld().values().size())
			n_e->setAttr("help",n_e->attr("help")+_("\n  Values: ")+vl.at().fld().values());
		    if(vl.at().fld().selNames().size())
			n_e->setAttr("help",n_e->attr("help")+_("\n  Names for selection: ")+vl.at().fld().selNames());
		}
	    }
	}
	if(ctrMkNode("area",opt,-1,"/arch",_("Archiving"))) {
	    //  Archiving
	    if(ctrMkNode("table",opt,-1,"/arch/arch",_("Archiving"),RWRWR_,"root",SARH_ID,1,"key","atr")) {
		vector<string> vLs2;
		ctrMkNode("list", opt, -1, "/arch/arch/atr", _("Attribute"), R_R_R_, "root", SARH_ID, 1, "tp", "str");
		ctrMkNode("list", opt, -1, "/arch/arch/prc", _("Archiving"), RWRWR_, "root", SARH_ID, 1, "tp", "bool");
		SYS->archive().at().modList(vLs);
		for(unsigned i_ta = 0; i_ta < vLs.size(); i_ta++) {
		    SYS->archive().at().at(vLs[i_ta]).at().valList(vLs2);
		    for(unsigned iA = 0; iA < vLs2.size(); iA++) {
			string a_id = SYS->archive().at().at(vLs[i_ta]).at().valAt(vLs2[iA]).at().workId();
			ctrMkNode("list",opt,-1,("/arch/arch/"+a_id).c_str(),a_id,RWRWR_,"root",SARH_ID,1,"tp","bool");
		    }
		}
	    }
	}
	return;
    }
    // Process command to page
    if(a_path.compare(0,4,"/val") == 0) {
	if(a_path.compare(0,9,"/val/sel_") == 0 && ctrChkNode(opt)) {
	    AutoHD<TVal> vl = vlAt(TSYS::pathLev(a_path,1).substr(4));
	    for(unsigned iA = 0; iA < vl.at().fld().selNm().size(); iA++)
		opt->childAdd("el")->setText(vl.at().fld().selNm()[iA]);
	    return;
	}
	AutoHD<TVal> vl = vlAt(TSYS::pathLev(a_path,1));
	if(ctrChkNode(opt,"get",(vl.at().fld().flg()&TFld::NoWrite)?R_R_R_:RWRWR_,"root",SDAQ_ID,SEC_RD))
	    opt->setText((vl.at().fld().type()==TFld::Real) ?
		    ((vl.at().getR()==EVAL_REAL) ? EVAL_STR : r2s(vl.at().getR(),6)) :
		    ((Mess->translDyn() && vl.at().fld().type()==TFld::String) ? trLU(vl.at().getS(),l,u) :
		    vl.at().getS()));
	if(ctrChkNode(opt,"set",(vl.at().fld().flg()&TFld::NoWrite)?R_R_R_:RWRWR_,"root",SDAQ_ID,SEC_WR))
	    vl.at().setS((Mess->translDyn() && vl.at().fld().type() == TFld::String) ? trSetLU(vl.at().getS(),l,u,opt->text()) : opt->text());
    }
    else if(a_path == "/arch/arch") {
	if(ctrChkNode(opt,"get",RWRWR_,"root",SARH_ID,SEC_RD)) {
	    vector<string> vLs2;
	    // Prepare headers
	    ctrMkNode("list", opt, -1, "/arch/arch/atr", "", R_R_R_);
	    ctrMkNode("list", opt, -1, "/arch/arch/prc", "", RWRWR_);
	    SYS->archive().at().modList(vLs);
	    for(unsigned i_ta = 0; i_ta < vLs.size(); i_ta++) {
		SYS->archive().at().at(vLs[i_ta]).at().valList(vLs2);
		for(unsigned iA = 0; iA < vLs2.size(); iA++)
		    ctrMkNode("list", opt, -1,
			("/arch/arch/"+SYS->archive().at().at(vLs[i_ta]).at().valAt(vLs2[iA]).at().workId()).c_str(), "", RWRWR_);
	    }
	    // Fill table
	    vlList(vLs);
	    for(unsigned i_v = 0; i_v < vLs.size(); i_v++)
		for(unsigned iA = 0; iA < opt->childSize(); iA++) {
		    XMLNode *chld = opt->childGet(iA);
		    string c_id = chld->attr("id");
		    if(c_id=="atr")		chld->childAdd("el")->setText(vLs[i_v]);
		    else if(c_id=="prc")	chld->childAdd("el")->setText(vlAt(vLs[i_v]).at().arch().freeStat()?"0":"1");
		    else chld->childAdd("el")->
			setText(vlAt(vLs[i_v]).at().arch().freeStat() ? "0" : (vlAt(vLs[i_v]).at().arch().at().archivatorPresent(c_id)?"1":"0"));
		}
	}
	if(ctrChkNode(opt,"set",RWRWR_,"root",SARH_ID,SEC_WR)) {
	    bool v_get = s2i(opt->text());
	    string attr = opt->attr("key_atr");
	    string col  = opt->attr("col");

	    // Check for create new
	    if(v_get && vlAt(attr).at().arch().freeStat()) vlAt(attr).at().setArch();
	    // Check for delete archive
	    if(col == "prc" && !v_get && !vlAt(attr).at().arch().freeStat()) {
		string a_id = vlAt(attr).at().arch().at().id();
		SYS->archive().at().valDel(a_id, true);
		modif();
	    }
	    // Change archivator status
	    if(col != "prc" && !vlAt(attr).at().arch().freeStat()) {
		if(v_get)	vlAt(attr).at().arch().at().archivatorAttach(col);
		else		vlAt(attr).at().arch().at().archivatorDetach(col, true);
		modif();
		vlAt(attr).at().arch().at().modif();
	    }
	}
    }
    else TCntrNode::cntrCmdProc(opt);
}

//*************************************************
//* TVal                                          *
//*************************************************
TVal::TVal( ) : mCfg(false), mReqFlg(false), mResB1(false), mResB2(false), mTime(0)
{
    src.fld = NULL;
    modifClr();
}

TVal::TVal( TFld &fld ) : mCfg(false), mTime(0)
{
    src.fld = NULL;
    modifClr();

    setFld(fld);
}

TVal::TVal( TCfg &cfg ) : mCfg(false), mTime(0)
{
    src.fld = NULL;
    modifClr();

    setCfg(cfg);
}

TVal::~TVal( )
{
    if(!mCfg && src.fld->type() == TFld::String)delete val.s;
    if(!mCfg && src.fld->type() == TFld::Object)delete val.o;
    if(!mCfg && src.fld->flg()&TFld::SelfFld)	delete src.fld;
}

string TVal::objName( )	{ return TCntrNode::objName()+":TVal"; }

string TVal::DAQPath( )	{ return owner().DAQPath()+"."+TSYS::strEncode(name(),TSYS::Custom,". "); }

TValue &TVal::owner( ) const	{ return *(TValue*)nodePrev(); }

void TVal::setFld( TFld &fld )
{
    //Delete previous
    if(!mCfg && src.fld)
	switch(src.fld->type()) {
	    case TFld::String:	delete val.s;	break;
	    case TFld::Object:	delete val.o;	break;
	    default: break;
	}
    if(!mCfg && src.fld && (src.fld->flg()&TFld::SelfFld)) delete src.fld;

    //Chek for self field for dynamic elements
    if(fld.flg()&TFld::SelfFld) {
	src.fld = new TFld();
	*(src.fld) = fld;
    }
    else src.fld = &fld;

    switch(src.fld->type()) {
	case TFld::String:  val.s = new string(src.fld->def().empty() ? string(EVAL_STR) : src.fld->def());	break;
	case TFld::Integer: val.i = src.fld->def().empty() ? EVAL_INT : s2i(src.fld->def());	break;
	case TFld::Real:    val.r = src.fld->def().empty() ? EVAL_REAL : s2r(src.fld->def());	break;
	case TFld::Boolean: val.b = src.fld->def().empty() ? EVAL_BOOL : s2i(src.fld->def());	break;
	case TFld::Object:  val.o = new AutoHD<TVarObj>(new TEValObj);	break;
	default: break;
    }

    mCfg = false;
}

void TVal::setCfg( TCfg &cfg )
{
    //Delete previous
    if(!mCfg && src.fld && src.fld->type() == TFld::String)	delete val.s;
    if(!mCfg && src.fld && src.fld->flg()&TFld::SelfFld)	delete src.fld;

    //Set cfg
    src.cfg = &cfg;
    mCfg = true;
}

string TVal::name( )			{ return mCfg ? src.cfg->name().c_str() : src.fld->name().c_str(); }

bool TVal::dataActive( )		{ return owner().dataActive(); }

const char *TVal::nodeName( ) const	{ return mCfg ? src.cfg->name().c_str() : src.fld->name().c_str(); }

TFld &TVal::fld( )			{ return mCfg ? src.cfg->fld() : *src.fld; }

AutoHD<TVArchive> TVal::arch( )		{ return mArch; }

void TVal::setArch( const AutoHD<TVArchive> &vl )	{ mArch = vl; }

string TVal::setArch( const string &nm )
{
    string rez_nm = nm, n_nm, a_nm;
    //Make archive name
    if(rez_nm.empty()) {
	if(SYS->archive().at().autoIdMode() == TArchiveS::OnlyPrmId) rez_nm = owner().nodeName();
	else if(SYS->archive().at().autoIdMode() == TArchiveS::OnlyAttrId) rez_nm = name();
	else {
	    n_nm = owner().nodeName();
	    a_nm = n_nm+"_"+name();
	    if(a_nm.size() > 20) a_nm = (n_nm.substr(0,n_nm.size()-n_nm.size()*(a_nm.size()-19)/21)+"_"+name()).substr(0,20);
	    rez_nm = a_nm;
	}
    }
    a_nm = rez_nm;
    for(int p_cnt = 2; SYS->archive().at().valPresent(rez_nm); p_cnt++) {
	AutoHD<TVal> dattr = SYS->archive().at().valAt(rez_nm).at().srcPAttr();
	if(!dattr.freeStat() && dattr.at().DAQPath() == DAQPath()) break;
	rez_nm = a_nm + i2s(p_cnt);
	if((int)rez_nm.size() > s2i(ARCH_ID_SZ)) rez_nm = a_nm.substr(0,a_nm.size()-(rez_nm.size()-s2i(ARCH_ID_SZ)))+i2s(p_cnt);
    }
    //Create new archive
    rez_nm = SYS->archive().at().valAdd(rez_nm);
    SYS->archive().at().valAt(rez_nm).at().setValType(fld().type());
    SYS->archive().at().valAt(rez_nm).at().setSrcMode(TVArchive::PassiveAttr,DAQPath());
    SYS->archive().at().valAt(rez_nm).at().setToStart(true);
    SYS->archive().at().valAt(rez_nm).at().start();
    owner().vlArchMake(*this);
    owner().modif();

    return rez_nm;
}

string TVal::getSEL( int64_t *tm, bool sys )
{
    if(!(fld().flg()&TFld::Selectable))	throw TError("Val", _("Not selective type!"));
    switch(fld().type()) {
	case TFld::String:	return fld().selVl2Nm(getS(tm,sys));
	case TFld::Integer:	return fld().selVl2Nm(getI(tm,sys));
	case TFld::Real:	return fld().selVl2Nm(getR(tm,sys));
	case TFld::Boolean:	return fld().selVl2Nm(getB(tm,sys));
	default: break;
    }

    return EVAL_STR;
}

TVariant TVal::get( int64_t *tm, bool sys )
{
    switch(fld().type()) {
	case TFld::Integer:	return getI(tm, sys);
	case TFld::Real:	return getR(tm, sys);
	case TFld::Boolean:	return getB(tm, sys);
	case TFld::String:	return getS(tm, sys);
	case TFld::Object:	return getO(tm, sys);
	default: break;
    }

    return EVAL_STR;
}

string TVal::getS( int64_t *tm, bool sys )
{
    switch(fld().type()) {
	case TFld::Integer:	{ int64_t vl = getI(tm,sys); return (vl!=EVAL_INT)  ? ll2s(vl) : EVAL_STR; }
	case TFld::Real:	{ double vl = getR(tm,sys); return (vl!=EVAL_REAL) ? r2s(vl) : EVAL_STR; }
	case TFld::Boolean:	{ char vl = getB(tm,sys); return (vl!=EVAL_BOOL) ? i2s((bool)vl) : EVAL_STR; }
	case TFld::Object:	return (getO().at().objName()!="EVAL") ? getO().at().getStrXML() : EVAL_STR;
	case TFld::String: {
	    setReqFlg(true);
	    //Get from archive
	    if(tm && (*tm) && !mArch.freeStat() && *tm/mArch.at().period() < time()/mArch.at().period())
		return mArch.at().getVal(tm).getS();
	    //Get value from config
	    if(mCfg) {
		if(tm) *tm = TSYS::curTime();
		return src.cfg->getS();
	    }
	    //Get current value
	    if(fld().flg()&TVal::DirRead && !sys) owner().vlGet(*this);
	    if(tm) *tm = time();
	    dataRes().lock();
	    string rez(val.s->data(), val.s->size());
	    dataRes().unlock();
	    return rez;
	}
	default: break;
    }

    return EVAL_STR;
}

int64_t TVal::getI( int64_t *tm, bool sys )
{
    switch(fld().type()) {
	case TFld::String:	{ string vl = getS(tm,sys); return (vl!=EVAL_STR) ? s2ll(vl) : EVAL_INT; }
	case TFld::Real:	{ double vl = getR(tm,sys); return (vl!=EVAL_REAL) ? (int64_t)vl : EVAL_INT; }
	case TFld::Boolean:	{ char vl = getB(tm,sys); return (vl!=EVAL_BOOL) ? (bool)vl : EVAL_INT; }
	case TFld::Object:	return (getO().at().objName()!="EVAL") ? 1 : EVAL_INT;
	case TFld::Integer:
	    setReqFlg(true);
	    //Get from archive
	    if(tm && (*tm) && !mArch.freeStat() && *tm/mArch.at().period() < time()/mArch.at().period())
		return mArch.at().getVal(tm).getI();
	    //Get value from config
	    if(mCfg) {
		if(tm) *tm = TSYS::curTime();
		return src.cfg->getI();
	    }
	    //Get current value
	    if(fld().flg()&TVal::DirRead && !sys) owner().vlGet(*this);
	    if(tm) *tm = time();
	    return val.i;
	default: break;
    }

    return EVAL_INT;
}

double TVal::getR( int64_t *tm, bool sys )
{
    switch(fld().type()) {
	case TFld::String:	{ string vl = getS(tm,sys); return (vl!=EVAL_STR) ? s2r(vl) : EVAL_REAL; }
	case TFld::Integer:	{ int64_t vl = getI(tm,sys); return (vl!=EVAL_INT) ? vl : EVAL_REAL; }
	case TFld::Boolean:	{ char vl = getB(tm,sys); return (vl!=EVAL_BOOL) ? (bool)vl : EVAL_REAL; }
	case TFld::Object:	return (getO().at().objName()!="EVAL") ? 1 : EVAL_REAL;
	case TFld::Real:
	    setReqFlg(true);
	    //Get from archive
	    if(tm && (*tm) && !mArch.freeStat() && *tm/mArch.at().period() < time()/mArch.at().period())
		return mArch.at().getVal(tm).getR();
	    //Get value from config
	    if(mCfg) {
		if(tm) *tm = TSYS::curTime();
		return src.cfg->getR();
	    }
	    //Get current value
	    if(fld().flg()&TVal::DirRead && !sys) owner().vlGet(*this);
	    if(tm) *tm = time();
	    return val.r;
	default: break;
    }

    return EVAL_REAL;
}

char TVal::getB( int64_t *tm, bool sys )
{
    switch(fld().type()) {
	case TFld::String:	{ string vl = getS(tm,sys); return (vl!=EVAL_STR) ? (bool)s2i(vl) : EVAL_BOOL; }
	case TFld::Integer:	{ int64_t vl = getI(tm,sys); return (vl!=EVAL_INT) ? (bool)vl : EVAL_BOOL; }
	case TFld::Real:	{ double vl = getR(tm,sys); return (vl!=EVAL_REAL) ? (bool)vl : EVAL_BOOL; }
	case TFld::Object:	return (getO().at().objName()!="EVAL") ? true : EVAL_BOOL;
	case TFld::Boolean:
	    setReqFlg(true);
	    //Get from archive
	    if(tm && (*tm) && !mArch.freeStat() && *tm/mArch.at().period() < time()/mArch.at().period())
		return mArch.at().getVal(tm).getB();
	    //Get value from config
	    if(mCfg) {
		if(tm) *tm = TSYS::curTime();
		return src.cfg->getB();
	    }
	    //Get current value
	    if(fld().flg()&TVal::DirRead && !sys) owner().vlGet(*this);
	    if(tm) *tm = time();
	    return val.b;
	default: break;
    }

    return EVAL_BOOL;
}

AutoHD<TVarObj> TVal::getO( int64_t *tm, bool sys )
{
    if(fld().type() != TFld::Object) return new TEValObj;

    setReqFlg(true);
    //Get from archive. Get objects from archive did not support
    //Get value from config. Get object form config did not support
    if(mCfg) return new TEValObj;
    //Get current value
    if(fld().flg()&TVal::DirRead && !sys) owner().vlGet(*this);
    if(tm) *tm = time();
    dataRes().lock();
    AutoHD<TVarObj> rez = *val.o;
    dataRes().unlock();

    return rez;
}

void TVal::setSEL( const string &value, int64_t tm, bool sys )
{
    if(!(fld().flg()&TFld::Selectable))	throw TError("Val", _("Not selective type!"));
    switch(fld().type()) {
	case TFld::String:	setS(fld().selNm2VlS(value), tm, sys);	break;
	case TFld::Integer:	setI(fld().selNm2VlI(value), tm, sys);	break;
	case TFld::Real:	setR(fld().selNm2VlR(value), tm, sys);	break;
	case TFld::Boolean:	setB(fld().selNm2VlB(value), tm, sys);	break;
	default: break;
    }
}

void TVal::set( const TVariant &value, int64_t tm, bool sys )
{
    switch(fld().type()) {
	case TFld::Integer:	setI(value.getI(), tm, sys);	break;
	case TFld::Real:	setR(value.getR(), tm, sys);	break;
	case TFld::Boolean:	setB(value.getB(), tm, sys);	break;
	case TFld::String:	setS(value.getS(), tm, sys);	break;
	case TFld::Object:	setO(value.getO(), tm, sys);	break;
	default: break;
    }
}

void TVal::setS( const string &value, int64_t tm, bool sys )
{
    switch(fld().type()) {
	case TFld::Integer:	setI((value!=EVAL_STR) ? s2ll(value) : EVAL_INT, tm, sys);	break;
	case TFld::Real:	setR((value!=EVAL_STR) ? s2r(value) : EVAL_REAL, tm, sys);	break;
	case TFld::Boolean:	setB((value!=EVAL_STR) ? (bool)s2i(value) : EVAL_BOOL, tm, sys);	break;
	case TFld::Object:
	    setO((value!=EVAL_STR) ? TVarObj::parseStrXML(value,NULL,getO(NULL,true)) : new TEValObj(), tm, sys);
	    break;
	case TFld::String: {
	    //Set value to config
	    if(mCfg)	{ src.cfg->setS( value ); return; }
	    //Check to write
	    if(!sys && fld().flg()&TFld::NoWrite) return;
	    //Set current value and time
	    dataRes().lock();
	    string pvl = *val.s;
	    val.s->assign(value.data(), value.size());
	    dataRes().unlock();
	    mTime = tm;
	    if(!mTime) mTime = TSYS::curTime();
	    if(fld().flg()&TVal::DirWrite && !sys)	owner().vlSet(*this, value, pvl);
	    //Set to archive
	    if(!mArch.freeStat() && mArch.at().srcMode() == TVArchive::PassiveAttr)
		try{ mArch.at().setS(value,time()); }
		catch(TError &err) {
		    if(err.cod == TError::Arch_Val_OldBufVl) mArch.at().clear();
		    mess_sys(TMess::Error, _("Error writing a value to the archive: %s"), err.mess.c_str());
		}
	    break;
	}
	default: break;
    }
}

void TVal::setI( int64_t value, int64_t tm, bool sys )
{
    switch(fld().type()) {
	case TFld::String:	setS((value!=EVAL_INT) ? ll2s(value) : EVAL_STR, tm, sys);	break;
	case TFld::Real:	setR((value!=EVAL_INT) ? value : EVAL_REAL, tm, sys);		break;
	case TFld::Boolean:	setB((value!=EVAL_INT) ? (bool)value : EVAL_BOOL, tm, sys);	break;
	case TFld::Object:	if(value == EVAL_INT) setO(new TEValObj(), tm, sys);		break;
	case TFld::Integer: {
	    //Set value to config
	    if(mCfg)	{ src.cfg->setI(value); return; }
	    //Check to write
	    if(!sys && fld().flg()&TFld::NoWrite) return;
	    //Set current value and time
	    if(!(fld().flg()&TFld::Selectable) && fld().selValI()[1] > fld().selValI()[0] && value != EVAL_INT)
		value = vmin(fld().selValI()[1], vmax(fld().selValI()[0],value));
	    int pvl = val.i; val.i = value;
	    mTime = tm;
	    if(!mTime) mTime = TSYS::curTime();
	    if(fld().flg()&TVal::DirWrite && !sys) owner().vlSet(*this, value, pvl);
	    //Set to archive
	    if(!mArch.freeStat() && mArch.at().srcMode() == TVArchive::PassiveAttr)
		try{ mArch.at().setI(value,time()); }
		catch(TError &err) {
		    if(err.cod == TError::Arch_Val_OldBufVl) mArch.at().clear();
		    mess_sys(TMess::Error, _("Error writing a value to the archive: %s"), err.mess.c_str());
		}
	    break;
	}
	default: break;
    }
}

void TVal::setR( double value, int64_t tm, bool sys )
{
    switch(fld().type()) {
	case TFld::String:	setS((value!=EVAL_REAL) ? r2s(value) : EVAL_STR, tm, sys);	break;
	case TFld::Integer:	setI((value!=EVAL_REAL) ? (int64_t)value : EVAL_INT, tm, sys);	break;
	case TFld::Boolean:	setB((value!=EVAL_REAL) ? (bool)value : EVAL_BOOL, tm, sys);	break;
	case TFld::Object:	if(value == EVAL_REAL) setO(new TEValObj(), tm, sys);		break;
	case TFld::Real: {
	    //Set value to config
	    if(mCfg)	{ src.cfg->setR(value); return; }
	    //Check to write
	    if(!sys && fld().flg()&TFld::NoWrite) return;
	    //Set current value and time
	    if(!(fld().flg()&TFld::Selectable) && fld().selValR()[1] > fld().selValR()[0] && value != EVAL_REAL)
		value = vmin(fld().selValR()[1], vmax(fld().selValR()[0],value));
	    double pvl = val.r; val.r = value;
	    mTime = tm;
	    if(!mTime) mTime = TSYS::curTime();
	    if(fld().flg()&TVal::DirWrite && !sys) owner().vlSet(*this, value, pvl);
	    //Set to archive
	    if(!mArch.freeStat() && mArch.at().srcMode() == TVArchive::PassiveAttr)
		try{ mArch.at().setR(value, time()); }
		catch(TError &err) {
		    if(err.cod == TError::Arch_Val_OldBufVl) mArch.at().clear();
		    mess_sys(TMess::Error, _("Error writing a value to the archive: %s"), err.mess.c_str());
		}
	    break;
	}
	default: break;
    }
}

void TVal::setB( char value, int64_t tm, bool sys )
{
    switch(fld().type()) {
	case TFld::String:	setS((value!=EVAL_BOOL) ? i2s((bool)value) : EVAL_STR, tm, sys);break;
	case TFld::Integer:	setI((value!=EVAL_BOOL) ? (bool)value : EVAL_INT, tm, sys);	break;
	case TFld::Real:	setR((value!=EVAL_BOOL) ? (bool)value : EVAL_REAL, tm, sys);	break;
	case TFld::Object:	if(value == EVAL_BOOL) setO(new TEValObj(), tm, sys);		break;
	case TFld::Boolean: {
	    //Set value to config
	    if(mCfg)	{ src.cfg->setB(value); return; }
	    //Check to write
	    if(!sys && fld().flg()&TFld::NoWrite) return;
	    //Set current value and time
	    char pvl = val.b; val.b = value;
	    mTime = tm;
	    if(!mTime) mTime = TSYS::curTime();
	    if(fld().flg()&TVal::DirWrite && !sys) owner().vlSet(*this, value, pvl);
	    //Set to archive
	    if(!mArch.freeStat() && mArch.at().srcMode() == TVArchive::PassiveAttr)
		try{ mArch.at().setB(value,time()); }
		catch(TError &err) {
		    if(err.cod == TError::Arch_Val_OldBufVl) mArch.at().clear();
		    mess_sys(TMess::Error, _("Error writing a value to the archive: %s"), err.mess.c_str());
		}
	    break;
	}
	default: break;
    }
}

void TVal::setO( AutoHD<TVarObj> value, int64_t tm, bool sys )
{
    if(mCfg || fld().type() != TFld::Object) return;
    //Set value to config. Set object to config did not support
    //Check to write
    if(!sys && fld().flg()&TFld::NoWrite) return;
    //Set current value and time
    dataRes().lock();
    AutoHD<TVarObj> pvl = *val.o;
    *val.o = value;
    dataRes().unlock();
    mTime = tm;
    if(!mTime) mTime = TSYS::curTime();
    if(fld().flg()&TVal::DirWrite && !sys) owner().vlSet(*this, value, pvl);
    //Set to archive. Set object to archive did not support
}

TVariant TVal::objFuncCall( const string &iid, vector<TVariant> &prms, const string &user )
{
    // ElTp get(int tm = 0, int utm = 0, bool sys = false) - get attribute value at time <tm:utm> and system access flag <sys>.
    //  tm, utm - the time for requested value
    //  sys - system request, direct from object
    if(iid == "get") {
	try {
	    TVariant rez;
	    int64_t tm = 0;
	    if(prms.size() >= 1) tm = (int64_t)prms[0].getI()*1000000;
	    if(prms.size() >= 2) tm += prms[1].getI();
	    bool isSys = false;
	    if(prms.size() >= 3) isSys = prms[2].getB();
	    rez = get(&tm, isSys);
	    if(prms.size() >= 1) { prms[0].setI(tm/1000000); prms[0].setModify(); }
	    if(prms.size() >= 2) { prms[1].setI(tm%1000000); prms[1].setModify(); }

	    return rez;
	} catch(...){ }
	return EVAL_REAL;
    }
    // bool set(ElTp val, int tm = 0, int utm = 0, bool sys = false) - write value <val> to attribute with time label <tm:utm> and system
    //       access flag <sys>
    //  tm, utm - time for set value
    //  sys - system request, direct to the object
    if(iid == "set" && prms.size() >= 1) {
	try {
	    int64_t tm = 0;
	    if(prms.size() >= 2) tm = (int64_t)prms[1].getI()*1000000;
	    if(prms.size() >= 3) tm += prms[2].getI();
	    bool isSys = false;
	    if(prms.size() >= 4) isSys = prms[3].getB();
	    if(isSys && isCfg() && fld().flg()&TFld::NoWrite) return false;
	    set(prms[0], tm, isSys);
	    return false;
	} catch(...){ }
	return true;
    }
    // TCntrNodeObj arch() - get current archive object for value
    if(iid == "arch") {
	AutoHD<TVArchive> aobj = arch();
	if(aobj.freeStat()) return false;
	return new TCntrNodeObj(aobj, user);
    }
    // string descr() - get attribute description
    if(iid == "descr")	return fld().descr();
    // int time(int utm) - get last attribute's value time
    if(iid == "time") {
	if(prms.size() >= 1)	{ prms[0].setI((int)(time()%1000000)); prms[0].setModify(); }
	return (int)(time()/1000000);
    }
    // int len() - get field length
    if(iid == "len")	return fld().len();
    // int dec() - float dec
    if(iid == "dec")	return fld().dec();
    // int flg() - field's flags
    if(iid == "flg")	return (int)fld().flg();
    // string def() - default value
    if(iid == "def")	return fld().def();
    // string values() - allowed values list or range
    if(iid == "values")	return fld().values();
    // string selNames() - names of allowed values list
    if(iid == "selNames")	return fld().selNames();
    // string reserve() - reserve string property value
    if(iid == "reserve")return fld().reserve();
    // bool isCfg() - check to a configuration field
    if(iid == "isCfg")	return isCfg();

    return TCntrNode::objFuncCall(iid,prms,user);
}

void TVal::preDisable( int flag )
{
    if(!arch().freeStat()) arch().at().stop();
}

void TVal::cntrCmdProc( XMLNode *opt )
{
    string a_path = opt->attr("path");
    //Service commands process
    if(a_path == "/serv/val") {		//Values access
	if(ctrChkNode(opt,"info",RWRWRW,"root",SDAQ_ID,SEC_RD)) {	//Value's data information
	    if(!arch().freeStat()) arch().at().cntrCmdProc(opt);
	    else {
		opt->setAttr("end", "0")->setAttr("beg", "0")->setAttr("per", "0");
		opt->setAttr("vtp", i2s(fld().type()));
	    }
	}
	else if(ctrChkNode(opt,"get",RWRWRW,"root",SDAQ_ID,SEC_RD)) {	//Value's data request
	    if(!s2ll(opt->attr("tm_grnd")) && opt->attr("arch").empty()) {
		int64_t tm = s2ll(opt->attr("tm"));
		opt->setText(getS(&tm));
		opt->setAttr("tm", ll2s(tm));
	    }
	    else if(!arch().freeStat()) arch().at().cntrCmdProc(opt);
	    else throw err_sys(_("Attribute does not have an archive"));
	}
	else if(ctrChkNode(opt,"name",RWRWRW,"root",SDAQ_ID,SEC_RD)) {	//Archive name request
	    if(owner().vlPresent("NAME") && owner().vlAt("NAME").at().getS().size())
		opt->setText(owner().vlAt("NAME").at().getS()+"."+fld().descr());
	    /*else if(owner().vlPresent("SHIFR") && owner().vlAt("SHIFR").at().getS().size())
		opt->setText(owner().vlAt("SHIFR").at().getS()+"."+fld().descr());
	    else if(owner().vlPresent("ID") && owner().vlAt("ID").at().getS().size())
		opt->setText(owner().vlAt("ID").at().getS()+"."+fld().descr());*/
	    else fld().descr();
	}
	return;
    }

    //Interface comands process
    // Info command process
    if(opt->name() == "info") {
	TCntrNode::cntrCmdProc(opt);
	return;
    }
    // Process command to page
    TCntrNode::cntrCmdProc(opt);
}
