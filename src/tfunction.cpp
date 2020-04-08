
//OpenSCADA file: tfunction.cpp
/***************************************************************************
 *   Copyright (C) 2003-2019 by Roman Savochenko, <rom_as@oscada.org>      *
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

#include <math.h>

#include <tsys.h>
#include <tmess.h>
#include "tfunction.h"

using namespace OSCADA;

//*************************************************
//* Function abstract object                      *
//*************************************************
TFunction::TFunction( const string &iid, const char *igrp, const string &istor ) :
    mId(iid), mStor(istor), runSt(false), beStart(false), mTVal(NULL), grp(igrp)
{
    if(mess_lev() == TMess::Debug) SYS->cntrIter(objName(), 1);
}

TFunction::TFunction( const TFunction &src ) : runSt(false), beStart(false), mTVal(NULL), grp(NULL)
{
    operator=(src);

    if(mess_lev() == TMess::Debug) SYS->cntrIter(objName(), 1);
}

TFunction::~TFunction( )
{
    for(unsigned iIO = 0; iIO < mIO.size(); iIO++) delete mIO[iIO];

    if(mess_lev() == TMess::Debug) SYS->cntrIter(objName(), -1);
}

TFunction &TFunction::operator=( const TFunction &func )
{
    //Copy IO
    // Clear no present IO
    for(int iIO = 0; iIO < ioSize(); )
	if(func.ioId(io(iIO)->id()) < 0) ioDel(iIO);
	else iIO++;
    // Update present and create new IO
    for(int iIO = 0; iIO < func.ioSize(); iIO++) {
	int dst_io = ioId(func.io(iIO)->id());
	if(dst_io < 0)
	    dst_io = ioIns(new IO(func.io(iIO)->id().c_str(),func.io(iIO)->name().c_str(),func.io(iIO)->type(),func.io(iIO)->flg(),
		func.io(iIO)->def().c_str(),func.io(iIO)->hide(),func.io(iIO)->rez().c_str()), iIO);
	else *io(dst_io) = *func.io(iIO);
	if(dst_io != iIO && !use()) ioMove(dst_io, iIO);
    }

    if(mId.empty()) mId = func.id();
    mStor = func.stor();

    return *this;
}

void TFunction::preDisable( int flag )
{
    if(mTVal) { delete mTVal; mTVal = NULL; }
    if(used.size()) {
	string mess;
	for(unsigned i = 0; i < used.size(); i++)
	    mess += used[i]->vfName()+", ";
	throw err_sys(_("Function used: %s"), mess.c_str());
    }
}

void TFunction::setId( const string &vl )
{
    if(!nodePrev(true)) mId = vl;
}

int TFunction::ioSize( ) const	{ return mIO.size(); }

IO *TFunction::io( int iid ) const
{
    if(iid >= (int)mIO.size()) throw err_sys(_("Index %d is broken!"), iid);
    return mIO[iid];
}

int TFunction::ioId( const string &id ) const
{
    for(int iIO = 0; iIO < (int)mIO.size(); iIO++)
	if(mIO[iIO]->id() == id) return iIO;

    return -1;
}

void TFunction::setStart( bool val )
{
    if(!val && mTVal) { delete mTVal; mTVal = NULL; }

    runSt = val;
}

void TFunction::ioList( vector<string> &list )
{
    for(unsigned iIO = 0; iIO < mIO.size(); iIO++)
	list.push_back(mIO[iIO]->id());
}

int TFunction::ioAdd( IO *io )
{
    preIOCfgChange();
    mIO.push_back(io);
    io->owner = this;
    postIOCfgChange();
    modif();

    return mIO.size()-1;
}

int TFunction::ioIns( IO *io, int pos )
{
    if(pos < 0 || pos > (int)mIO.size()) pos = mIO.size();

    preIOCfgChange();
    mIO.insert(mIO.begin()+pos,io);
    io->owner = this;
    postIOCfgChange();
    modif();

    return pos;
}

void TFunction::ioDel( int pos )
{
    if(pos < 0 || pos >= (int)mIO.size()) throw err_sys(_("Error removing IO %d."), pos);

    preIOCfgChange();
    mIO.erase(mIO.begin()+pos);
    postIOCfgChange();
    modif();
}

void TFunction::ioMove( int pos, int to )
{
    if(pos < 0 || pos >= (int)mIO.size() || to < 0 || to >= (int)mIO.size())
	throw err_sys(_("Error moving IO from %d to %d."), pos, to);

    preIOCfgChange();
    IO *io = mIO[to];
    mIO[to] = mIO[pos];
    mIO[pos] = io;
    postIOCfgChange();
    modif();
}

void TFunction::preIOCfgChange( )
{
    //Previous stop
    beStart = startStat();
    if(beStart) {
	setStart(false);
	if(mTVal) { delete mTVal; mTVal = NULL; }
    }

    //Main process
    string blk_lst;
    for(unsigned i = 0; i < used.size(); i++)
	if(used[i]->blk()) blk_lst += used[i]->vfName()+",";
    if(blk_lst.size())
	throw err_sys(_("Changes are not possible because the function is used: %s"), blk_lst.c_str());

    for(unsigned i = 0; i < used.size(); i++) used[i]->preIOCfgChange();
}

void TFunction::postIOCfgChange( )
{
    //Start for restore
    if(beStart) setStart(true);

    for(unsigned i = 0; i < used.size(); i++) used[i]->postIOCfgChange();
}

void TFunction::valAtt( TValFunc *vfnc )
{
    MtxAlloc res(dataRes(), true);
    for(unsigned i = 0; i < used.size(); i++)
	if(used[i] == vfnc)
	    throw err_sys(_("The value '%s' is already connected!"), vfnc->vfName().c_str());
    used.push_back(vfnc);
}

void TFunction::valDet( TValFunc *vfnc )
{
    dataRes().lock();
    for(unsigned i = 0; i < used.size() ;i++)
	if(used[i] == vfnc)
	{ used.erase(used.begin()+i); break; }
    dataRes().unlock();
}

TVariant TFunction::objFuncCall( const string &iid, vector<TVariant> &prms, const string &user )
{
    // ElTp call(ElTp prm1, ...) - the function call
    //  prm{N} - {N} parameter to the function.
    if(iid == "call") {
	if(!startStat()) return TVariant();
	TValFunc vfnc("JavaLikeObjFuncCalc",this);

	//  Get return position
	int r_pos, i_p, p_p;
	for(r_pos = 0; r_pos < vfnc.func()->ioSize(); r_pos++)
	    if(vfnc.ioFlg(r_pos)&IO::Return) break;
	//  Process parameters
	for(i_p = p_p = 0; true; i_p++) {
	    p_p = (i_p>=r_pos) ? i_p+1 : i_p;
	    if(p_p >= vfnc.func()->ioSize()) break;
	    //   Set default value
	    if(i_p >= (int)prms.size()) { vfnc.setS(p_p,vfnc.func()->io(p_p)->def()); continue; }
	    vfnc.set(p_p,prms[i_p]);
	}
	//  Make calc
	vfnc.calc(user);
	//  Process outputs
	for(i_p = 0; i_p < (int)prms.size(); i_p++) {
	    p_p = (i_p>=r_pos) ? i_p+1 : i_p;
	    if(p_p >= vfnc.func()->ioSize()) break;
	    if(!(vfnc.ioFlg(p_p)&IO::Output))   continue;
	    prms[i_p] = vfnc.get(p_p);
	    prms[i_p].setModify();
	}
	//  Set return
	if(r_pos < vfnc.func()->ioSize()) return vfnc.get(r_pos);
	return TVariant();
    }

    return TCntrNode::objFuncCall(iid, prms, user);
}

void TFunction::cntrCmdProc( XMLNode *opt )
{
    //Get page info
    if(opt->name() == "info") {
	TCntrNode::cntrCmdProc(opt);
	ctrMkNode("oscada_cntr",opt,-1,"/",_("Function: ")+name(),R_R_R_,"root",grp);
	if(ctrMkNode("area",opt,-1,"/func",_("Function"))) {
	    if(ctrMkNode("area",opt,-1,"/func/st",_("State"))) {
		ctrMkNode("fld",opt,-1,"/func/st/st",_("Accessing"),RWRWR_,"root",grp,1,"tp","bool");
		ctrMkNode("fld",opt,-1,"/func/st/use",_("Used"),R_R_R_,"root",grp,1,"tp","dec");
	    }
	    if(ctrMkNode("area",opt,-1,"/func/cfg",_("Configuration"))) {
		ctrMkNode("fld",opt,-1,"/func/cfg/ID",_("Identifier"),R_R_R_,"root",grp,1,"tp","str");
		ctrMkNode("fld",opt,-1,"/func/cfg/NAME",_("Name"),R_R_R_,"root",grp,1,"tp","str");
		ctrMkNode("fld",opt,-1,"/func/cfg/DESCR",_("Description"),R_R_R_,"root",grp,3,"tp","str","cols","90","rows","5");
	    }
	}
	if(ctrMkNode("area",opt,-1,"/io",_("IO")))
	    if(ctrMkNode("table",opt,-1,"/io/io",_("IO"),R_R_R_,"root",grp,1,"rows","5")) {
		ctrMkNode("list",opt,-1,"/io/io/0",_("Identifier"),R_R_R_,"root",grp,1,"tp","str");
		ctrMkNode("list",opt,-1,"/io/io/1",_("Name"),R_R_R_,"root",grp,1,"tp","str");
		ctrMkNode("list",opt,-1,"/io/io/2",_("Type"),R_R_R_,"root",grp,1,"tp","str");
		ctrMkNode("list",opt,-1,"/io/io/3",_("Mode"),R_R_R_,"root",grp,1,"tp","str");
		ctrMkNode("list",opt,-1,"/io/io/4",_("Hide"),R_R_R_,"root",grp,1,"tp","bool");
		ctrMkNode("list",opt,-1,"/io/io/5",_("Default"),R_R_R_,"root",grp,1,"tp","str");
	    }
	if(ctrMkNode("area",opt,-1,"/exec",_("Execute"),RWRW__,"root",grp)) {
	    ctrMkNode("fld",opt,-1,"/exec/en",_("Enabled"),RWRW__,"root",grp,1,"tp","bool");
	    // Add test form
	    if(mTVal) {
		if(ctrMkNode("area",opt,-1,"/exec/io",_("IO")))
		    for(int iIO = 0; iIO < ioSize(); iIO++) {
			if(mIO[iIO]->hide()) continue;
			XMLNode *nd = ctrMkNode("fld",opt,-1,("/exec/io/"+io(iIO)->id()).c_str(),io(iIO)->name(),RWRW__,"root",grp);
			if(nd) {
			    switch(io(iIO)->type()) {
				case IO::String:
				    nd->setAttr("tp","str");
				    if(io(iIO)->flg()&IO::FullText) nd->setAttr("cols","100")->setAttr("rows","4");
				    break;
				case IO::Integer:	nd->setAttr("tp","dec");	break;
				case IO::Real:		nd->setAttr("tp","real");	break;
				case IO::Boolean:	nd->setAttr("tp","bool");	break;
				case IO::Object:	nd->setAttr("tp","str")->setAttr("cols","100")->setAttr("rows","4");	break;
			    }
			}
		    }
		// Add Calc button and Calc time
		ctrMkNode("fld",opt,-1,"/exec/n_clc",_("Number of calculations"),RWRW__,"root",grp,1,"tp","dec");
		ctrMkNode("fld",opt,-1,"/exec/tm",_("Spent time"),R_R___,"root",grp,1,"tp","str");
		ctrMkNode("comm",opt,-1,"/exec/calc",_("Execute"),RWRW__,"root",grp);
	    }
	}
        return;
    }

    //Process command to page
    string a_path = opt->attr("path");
    if(a_path == "/func/st/st") {
	if(ctrChkNode(opt,"get",RWRWR_,"root",grp,SEC_RD))	opt->setText(runSt?"1":"0");
	if(ctrChkNode(opt,"set",RWRWR_,"root",grp,SEC_WR))	setStart(s2i(opt->text()));
    }
    else if(a_path == "/func/st/use" && ctrChkNode(opt))	opt->setText(i2s(used.size()));
    else if(a_path == "/func/cfg/ID" && ctrChkNode(opt))	opt->setText(id());
    else if(a_path == "/func/cfg/NAME" && ctrChkNode(opt))	opt->setText(name());
    else if(a_path == "/func/cfg/DESCR" && ctrChkNode(opt))	opt->setText(descr());
    else if(a_path == "/io/io" && ctrChkNode(opt,"get",R_R_R_,"root",grp,SEC_RD)) {
	XMLNode *n_id	= ctrMkNode("list",opt,-1,"/io/io/0","");
	XMLNode *n_nm	= ctrMkNode("list",opt,-1,"/io/io/1","");
	XMLNode *n_type	= ctrMkNode("list",opt,-1,"/io/io/2","");
	XMLNode *n_mode	= ctrMkNode("list",opt,-1,"/io/io/3","");
	XMLNode *n_hide	= ctrMkNode("list",opt,-1,"/io/io/4","");
	XMLNode *n_def	= ctrMkNode("list",opt,-1,"/io/io/5","");
	//XMLNode *n_vect	= ctrId(opt,"6");
	for(int iIO = 0; iIO < ioSize(); iIO++) {
	    string tmp_str;
	    if(n_id)	n_id->childAdd("el")->setText(io(iIO)->id());
	    if(n_nm)	n_nm->childAdd("el")->setText(io(iIO)->name());
	    if(n_type) {
		switch(io(iIO)->type()) {
		    case IO::String:	tmp_str = _("String");	break;
		    case IO::Integer:	tmp_str = _("Integer");	break;
		    case IO::Real:	tmp_str = _("Real");	break;
		    case IO::Boolean:	tmp_str = _("Bool");	break;
		    case IO::Object:	tmp_str = _("Object");	break;
		}
		n_type->childAdd("el")->setText(tmp_str);
	    }
	    if(n_mode) {
		if(io(iIO)->flg()&IO::Return)		tmp_str = _("Return");
		else if(io(iIO)->flg()&IO::Output)	tmp_str = _("Output");
		else					tmp_str = _("Input");
		n_mode->childAdd("el")->setText(tmp_str);
	    }
	    if(n_hide)	n_hide->childAdd("el")->setText(io(iIO)->hide()?"1":"0");
	    if(n_def)	n_def->childAdd("el")->setText(io(iIO)->def());
	}
    }
    else if(a_path == "/exec/en") {
	if(ctrChkNode(opt,"get",RWRW__,"root",grp,SEC_RD))	opt->setText(mTVal?"1":"0");
	if(ctrChkNode(opt,"set",RWRW__,"root",grp,SEC_WR)) {
	    bool to_en_exec = s2i(opt->text());
	    if(to_en_exec && !mTVal)	mTVal = new TValFunc(id()+"_exec",this);
	    if(!to_en_exec && mTVal)	{ delete mTVal; mTVal = NULL; }
	}
    }
    else if(a_path == "/exec/n_clc" && mTVal) {
	if(ctrChkNode(opt,"get",RWRW__,"root",grp,SEC_RD))	opt->setText(TBDS::genDBGet(nodePath()+"ntCalc","1",opt->attr("user")));
	if(ctrChkNode(opt,"set",RWRW__,"root",grp,SEC_WR))	TBDS::genDBSet(nodePath()+"ntCalc",opt->text(),opt->attr("user"));
    }
    else if(a_path == "/exec/tm" && mTVal && ctrChkNode(opt,"get",R_R___,"root",grp,SEC_RD))
	opt->setText(tm2s(1e-6*SYS->cntrGet(nodePath('.'))));
    else if(a_path.substr(0,8) == "/exec/io" && mTVal) {
	string io_id = TSYS::pathLev(a_path,2);
	for(unsigned iIO = 0; iIO < mIO.size(); iIO++)
	    if(io_id == io(iIO)->id()) {
		if(ctrChkNode(opt,"get",RWRW__,"root",grp,SEC_RD))
		    opt->setText((mTVal->ioType(iIO)==IO::Real) ? r2s(mTVal->getR(iIO),6) : mTVal->getS(iIO));
		if(ctrChkNode(opt,"set",RWRW__,"root",grp,SEC_WR))
		    mTVal->setS(iIO,opt->text());
		break;
	    }
    }
    else if(a_path == "/exec/calc" && mTVal && ctrChkNode(opt,"set",RWRW__,"root",grp,SEC_WR)) {
	int n_tcalc = s2i(TBDS::genDBGet(nodePath()+"ntCalc","1",opt->attr("user")));
	string wuser = opt->attr("user");
	time_t tm_lim = SYS->sysTm()+STD_WAIT_TM;
	int64_t t_cnt = TSYS::curTime();
	for(int iC = 0; iC < n_tcalc && SYS->sysTm() < tm_lim; iC++)
	    mTVal->calc(wuser);
	SYS->cntrSet(nodePath('.'), TSYS::curTime()-t_cnt);
    }
    else TCntrNode::cntrCmdProc(opt);
}

//*************************************************
//* IO                                            *
//*************************************************
IO::IO( const char *iid, const char *iname, IO::Type itype,  unsigned iflgs, const char *idef, bool ihide, const char *irez ) :
    mId(iid), mName(iname), mType(itype), mFlg(iflgs), mDef(idef), mHide(ihide), mRez(irez), owner(NULL)
{

}

IO::IO( const IO &src ) : mType(Real), mFlg(0), mHide(false), owner(NULL)
{
    operator=(src);
}

IO &IO::operator=( const IO &iio )
{
    setId(iio.id());
    setName(iio.name());
    setType(iio.type());
    setFlg(iio.flg());
    setDef(iio.def());
    setHide(iio.hide());
    setRez(iio.rez());

    return *this;
}

void IO::setId( const string &val )
{
    if(mId == val) return;
    owner->preIOCfgChange();
    mId = val;
    owner->postIOCfgChange();
}

void IO::setName( const string &val )	{ mName = val; }

void IO::setType( Type val )
{
    if(mType == val) return;
    owner->preIOCfgChange();
    mType = val;
    owner->postIOCfgChange();
}

void IO::setFlg( unsigned val )
{
    if(mFlg == val) return;
    owner->preIOCfgChange();
    mFlg = val;
    owner->postIOCfgChange();
}

void IO::setDef( const string &val )
{
    if(mDef == val) return;
    //owner->preIOCfgChange();
    mDef = val;
    //owner->postIOCfgChange();
}

void IO::setHide( bool val )
{
    if(mHide == val) return;
    //owner->preIOCfgChange();
    mHide = val;
    //owner->postIOCfgChange();
}

void IO::setRez( const string &val )
{
    if(mRez == val) return;
    mRez = val;
}

//*************************************************
//* TValFunc                                      *
//*************************************************
TValFunc::TValFunc( const string &iname, TFunction *ifunc, bool iblk, const string &iuser ) :
    exCtx(NULL), mName(iname), mUser(iuser), mBlk(iblk), mMdfChk(false), mPrgCh(false), mFunc(NULL)
{
    setFunc(ifunc);
}

TValFunc::~TValFunc( )
{
    if(mFunc) funcDisConnect();
}

void TValFunc::setFunc( TFunction *ifunc, bool att_det )
{
    if(mFunc)	funcDisConnect(att_det);
    if(ifunc) {
	mFunc = ifunc;
	if(att_det) {
	    mFunc->AHDConnect();
	    mFunc->valAtt(this);
	}
	for(int iVl = 0; iVl < mFunc->ioSize(); iVl++) {
	    SVl val;
	    val.tp = mFunc->io(iVl)->type();
	    val.mdf = false;
	    string def = mFunc->io(iVl)->def();
	    if(mFunc->io(iVl)->flg()&IO::Selectable)	def = TSYS::strLine(def, 0);
	    switch(val.tp) {
		case IO::String:	val.val.s = new string(def);	break;
		case IO::Integer:	val.val.i = s2ll(def);		break;
		case IO::Real:		val.val.r = s2r(def);		break;
		case IO::Boolean:	val.val.b = s2i(def);		break;
		case IO::Object:	val.val.o = new AutoHD<TVarObj>(new TVarObj);	break;
	    }
	    mVal.push_back(val);
	}
    }
}

void TValFunc::funcDisConnect( bool det )
{
    ctxClear();

    if(mFunc) {
	for(unsigned iVl = 0; iVl < mVal.size(); iVl++)
	    switch( mVal[iVl].tp ) {
		case IO::String: delete mVal[iVl].val.s;	break;
		case IO::Object: delete mVal[iVl].val.o;	break;
	    }

	mVal.clear();
	if(det) {
	    mFunc->valDet(this);
	    mFunc->AHDDisConnect();
	    mFunc = NULL;
	}
    }
}

int TValFunc::ioId( const string &iid )
{
    if(!mFunc)	throw TError("ValFnc", _("IO '%s' is not present!"), iid.c_str());

    return mFunc->ioId(iid);
}

void TValFunc::ioList( vector<string> &list )
{
    if(!mFunc)	throw TError("ValFnc", _("Function is not attached!"));

    return mFunc->ioList(list);
}

int TValFunc::ioSize( ) const
{
    if(!mFunc)	throw TError("ValFnc", _("Function is not attached!"));

    return mFunc->ioSize();
}

TVariant TValFunc::get( unsigned id )
{
    if(id >= mVal.size())	throw TError("ValFnc", _("%s: Error with ID or IO %d!"), "getS()", id);
    switch(mVal[id].tp) {
	case IO::Integer:	return getI(id);
	case IO::Real:		return getR(id);
	case IO::Boolean:	return getB(id);
	case IO::String:	return getS(id);
	case IO::Object:	return getO(id);
    }

    return EVAL_STR;
}

string TValFunc::getS( unsigned id )
{
    if(id >= mVal.size()) throw TError("ValFnc", _("%s: Error with ID or IO %d!"),"getS()", id);
    switch(mVal[id].tp) {
	case IO::Integer: { int64_t tvl = getI(id);return (tvl!=EVAL_INT) ? ll2s(tvl) : EVAL_STR; }
	case IO::Real:	  { double tvl = getR(id); return (tvl!=EVAL_REAL) ? r2s(tvl) : EVAL_STR; }
	case IO::Boolean: { char tvl = getB(id);   return (tvl!=EVAL_BOOL) ? i2s((bool)tvl) : EVAL_STR; }
	case IO::Object:  return getO(id).at().getStrXML();
	case IO::String: {
	    mRes.lock();
	    string tvl(mVal[id].val.s->data(), mVal[id].val.s->size());
	    mRes.unlock();
	    return tvl;
	}
    }

    return EVAL_STR;
}

int64_t TValFunc::getI( unsigned id )
{
    if(id >= mVal.size()) throw TError("ValFnc", _("%s: Error with ID or IO %d!"), "getI()", id);
    switch(mVal[id].tp) {
	case IO::String:  { string tvl = getS(id); return (tvl!=EVAL_STR) ? s2ll(tvl) : EVAL_INT; }
	case IO::Real:	  { double tvl = getR(id); return (tvl!=EVAL_REAL) ? (int64_t)tvl : EVAL_INT; }
	case IO::Boolean: { char tvl = getB(id);   return (tvl!=EVAL_BOOL) ? (bool)tvl : EVAL_INT; }
	case IO::Integer: return mVal[id].val.i;
    }

    return EVAL_INT;
}

double TValFunc::getR( unsigned id )
{
    if(id >= mVal.size()) throw TError("ValFnc", _("%s: Error with ID or IO %d!"), "getR()", id);
    switch(mVal[id].tp) {
	case IO::String:  { string tvl = getS(id); return (tvl!=EVAL_STR) ? s2r(tvl) : EVAL_REAL; }
	case IO::Integer: { int64_t tvl = getI(id);return (tvl!=EVAL_INT) ? tvl : EVAL_REAL; }
	case IO::Boolean: { char tvl = getB(id);   return (tvl!=EVAL_BOOL) ? (bool)tvl : EVAL_REAL; }
	case IO::Real:    return mVal[id].val.r;
    }

    return EVAL_REAL;
}

char TValFunc::getB( unsigned id )
{
    if(id >= mVal.size()) throw TError("ValFnc", _("%s: Error with ID or IO %d!"), "getB()", id);
    switch(mVal[id].tp) {
	case IO::String:  { string tvl = getS(id); return (tvl!=EVAL_STR) ? (bool)s2i(tvl) : EVAL_BOOL; }
	case IO::Integer: { int64_t tvl = getI(id);return (tvl!=EVAL_INT) ? (bool)tvl : EVAL_BOOL; }
	case IO::Real:    { double tvl = getR(id); return (tvl!=EVAL_REAL) ? (bool)tvl : EVAL_BOOL; }
	case IO::Boolean:  return mVal[id].val.b;
    }

    return EVAL_BOOL;
}

AutoHD<TVarObj> TValFunc::getO( unsigned id )
{
    if(id >= mVal.size()) throw TError("ValFnc", _("%s: Error with ID or IO %d!"), "getO()", id);
    if(mVal[id].tp != IO::Object) throw TError("ValFnc", _("Error getting object from non-object IO %d!"), id);
    mRes.lock();
    AutoHD<TVarObj> rez = *mVal[id].val.o;
    mRes.unlock();

    return rez;
}

void TValFunc::set( unsigned id, const TVariant &val )
{
    if(id >= mVal.size())	throw TError("ValFnc", _("%s: Error with ID or IO %d!"), "setS()", id);
    switch(mVal[id].tp) {
	case IO::Integer:	setI(id, val.getI());	break;
	case IO::Real:		setR(id, val.getR());	break;
	case IO::Boolean:	setB(id, val.getB());	break;
	case IO::String:	setS(id, val.getS());	break;
	case IO::Object:	setO(id, val.getO());	break;
    }
}

void TValFunc::setS( unsigned id, const string &val )
{
    if(id >= mVal.size())	throw TError("ValFnc", _("%s: Error with ID or IO %d!"), "setS()", id);
    switch(mVal[id].tp) {
	case IO::Integer:	setI(id, (val!=EVAL_STR) ? s2ll(val) : EVAL_INT);	break;
	case IO::Real:		setR(id, (val!=EVAL_STR) ? s2r(val) : EVAL_REAL);	break;
	case IO::Boolean:	setB(id, (val!=EVAL_STR) ? (bool)s2i(val) : EVAL_BOOL);	break;
	case IO::Object:
	    setO(id, (val!=EVAL_STR) ? TVarObj::parseStrXML(val,NULL,*mVal[id].val.o) : new TEValObj());
	    break;
	case IO::String:
	    mRes.lock();
	    if(mdfChk() && val != *mVal[id].val.s) mVal[id].mdf = true;
	    mVal[id].val.s->assign(val.data(), val.size());;
	    mRes.unlock();
	    break;
    }
}

void TValFunc::setI( unsigned id, int64_t val )
{
    if(id >= mVal.size())	throw TError("ValFnc", _("%s: Error with ID or IO %d!"), "setI()", id);
    switch(mVal[id].tp) {
	case IO::String:	setS(id, (val!=EVAL_INT) ? ll2s(val) : EVAL_STR);	break;
	case IO::Real:		setR(id, (val!=EVAL_INT) ? val : EVAL_REAL);		break;
	case IO::Boolean:	setB(id, (val!=EVAL_INT) ? (bool)val : EVAL_BOOL);	break;
	case IO::Object:	if(val == EVAL_INT) setO(id, new TEValObj());		break;
	case IO::Integer:
	    if(mdfChk() && val != mVal[id].val.i) mVal[id].mdf = true;
	    mVal[id].val.i = val;
	    break;
    }
}

void TValFunc::setR( unsigned id, double val )
{
    if(id >= mVal.size())	throw TError("ValFnc", _("%s: Error with ID or IO %d!"), "setR()", id);
    switch(mVal[id].tp) {
	case IO::String:	setS(id, (val!=EVAL_REAL) ? r2s(val) : EVAL_STR);	break;
	case IO::Integer:	setI(id, (val!=EVAL_REAL) ? (int64_t)val : EVAL_INT);	break;
	case IO::Boolean:	setB(id, (val!=EVAL_REAL) ? (bool)val : EVAL_BOOL);	break;
	case IO::Object:	if(val == EVAL_REAL) setO(id, new TEValObj());		break;
	case IO::Real:
	    if(isnan(val)) val = 0;	//Check for 'Not a Number'
	    if(mdfChk() && val != mVal[id].val.r) mVal[id].mdf = true;
	    mVal[id].val.r = val;
	    break;
    }
}

void TValFunc::setB( unsigned id, char val )
{
    if(id >= mVal.size())	throw TError("ValFnc", _("%s: Error with ID or IO %d!"), "setB()", id);
    switch(mVal[id].tp) {
	case IO::String:	setS(id, (val!=EVAL_BOOL) ? i2s((bool)val) : EVAL_STR);	break;
	case IO::Integer:	setI(id, (val!=EVAL_BOOL) ? (bool)val : EVAL_INT);	break;
	case IO::Real:		setR(id, (val!=EVAL_BOOL) ? (bool)val : EVAL_REAL);	break;
	case IO::Object:	if(val == EVAL_BOOL) setO(id, new TEValObj());		break;
	case IO::Boolean:
	    if(mdfChk() && val != mVal[id].val.b) mVal[id].mdf = true;
	    mVal[id].val.b = val;
	    break;
    }
}

void TValFunc::setO( unsigned id, AutoHD<TVarObj> val )
{
    if(id >= mVal.size()) throw TError("ValFnc", _("%s: Error with ID or IO %d!"), "setO()", id);
    switch(mVal[id].tp) {
	case IO::String:	setS(id, val.at().getStrXML());	break;
	case IO::Integer: case IO::Real: case IO::Boolean:
				setB(id, true);			break;
	case IO::Object:
	    mRes.lock();
	    if(mdfChk() && !(val == *mVal[id].val.o)) mVal[id].mdf = true;
	    *mVal[id].val.o = val;
	    mRes.unlock();
	    return;
    }
}

void TValFunc::setMdfChk( bool set )
{
    mMdfChk = set;
    if(set)
	for(unsigned i_v = 0; i_v < mVal.size(); i_v++)
	    mVal[i_v].mdf = false;
}

void TValFunc::calc( const string &user )
{
    if(!mFunc) return;
    if(!user.empty()) mUser = user;
    mFunc->calc(this);
}

void TValFunc::preIOCfgChange( )	{ setFunc(NULL, false); }

void TValFunc::postIOCfgChange( )	{ setFunc(mFunc, false); }

TValFunc *TValFunc::ctxGet( int key )
{
    map<int,TValFunc* >::iterator vc = vctx.find(key);

    return (vc == vctx.end()) ? NULL : vc->second;
}

void TValFunc::ctxSet( int key, TValFunc *val )
{
    map<int,TValFunc* >::iterator vc = vctx.find(key);
    if(vc == vctx.end()) vctx[key] = val;
    else {
	if(vc->second) delete vc->second;
	vc->second = val;
    }
}

void TValFunc::ctxClear( )
{
    for(map<int,TValFunc* >::iterator ivcx = vctx.begin(); ivcx != vctx.end(); ivcx++)
	delete ivcx->second;
    vctx.clear();
}

//***********************************************************
//* TFuncArgObj                                             *
//*   The function arguments access object.                 *
//***********************************************************
TVariant TFuncArgsObj::propGet( const string &id )
{
    int apos = -1;

    if(!vf.func())	return EVAL_REAL;
    if(id == "length")	return vf.ioSize();
    if(id.size() && isdigit(id[0])) apos = s2i(id);
    if(apos < 0 || apos >= vf.ioSize()) apos = vf.ioId(id);
    if(apos != -1)	return vf.get(apos);
    return EVAL_REAL;
}

void TFuncArgsObj::propSet( const string &id, TVariant val )
{
    int apos = -1;

    if(!vf.func()) return;
    if(id.size() && isdigit(id[0])) apos = s2i(id);
    if(apos < 0 || apos >= vf.ioSize()) apos = vf.ioId(id);
    if(apos != -1) vf.set(apos, val);
}

string TFuncArgsObj::getStrXML( const string &oid )
{
    string nd("<TFuncArgsObj");
    if(!oid.empty()) nd = nd + " p='" + oid + "'";
    nd = nd + ">\n";
    for(int iIO = 0; vf.func() && iIO < vf.ioSize(); iIO++)
	switch(vf.ioType(iIO)) {
	    case IO::String:	nd += "<str id='"+vf.func()->io(iIO)->id()+"'>"+TSYS::strEncode(vf.getS(iIO),TSYS::Html)+"</str>\n"; break;
	    case IO::Integer:	nd += "<int id='"+vf.func()->io(iIO)->id()+"'>"+vf.getS(iIO)+"</int>\n"; break;
	    case IO::Real:	nd += "<real id='"+vf.func()->io(iIO)->id()+"'>"+vf.getS(iIO)+"</real>\n"; break;
	    case IO::Boolean:	nd += "<bool id='"+vf.func()->io(iIO)->id()+"'>"+vf.getS(iIO)+"</bool>\n"; break;
	    case IO::Object:	nd += vf.getO(iIO).at().getStrXML(vf.func()->io(iIO)->id()); break;
	}
    nd += "</TFuncArgsObj>\n";

    return nd;
}
