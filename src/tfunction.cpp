
//OpenSCADA system file: tfunction.cpp
/***************************************************************************
 *   Copyright (C) 2003-2014 by Roman Savochenko, <rom_as@oscada.org>      *
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
    mId(iid), mStor(istor), run_st(false), be_start(false), mTVal(NULL), grp(igrp)
{

}

TFunction::~TFunction( )
{
    for(unsigned i_io = 0; i_io < mIO.size(); i_io++) delete mIO[i_io];
}

TFunction &TFunction::operator=( TFunction &func )
{
    //Copy IO
    // Clear no present IO
    for(int i_io = 0; i_io < ioSize(); )
	if(func.ioId(io(i_io)->id()) < 0) ioDel(i_io);
	else i_io++;
    // Update present and create new IO
    for(int i_io = 0; i_io < func.ioSize(); i_io++) {
	int dst_io = ioId(func.io(i_io)->id());
	if(dst_io < 0)
	    dst_io = ioIns( new IO( func.io(i_io)->id().c_str(), func.io(i_io)->name().c_str(), func.io(i_io)->type(), func.io(i_io)->flg(),
		func.io(i_io)->def().c_str(), func.io(i_io)->hide(), func.io(i_io)->rez().c_str() ), i_io );
	else *io(dst_io) = *func.io(i_io);
	if(dst_io != i_io && !use()) ioMove(dst_io,i_io);
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
	throw TError(nodePath().c_str(),_("Function is used by: %s"),mess.c_str());
    }
}

void TFunction::setId( const string &vl )
{
    if(!nodePrev(true)) mId = vl;
}

int TFunction::ioSize( )	{ return mIO.size(); }

IO *TFunction::io( int iid )
{
    if(iid >= (int)mIO.size()) throw TError(nodePath().c_str(),_("Index %d is broken!"),iid);
    return mIO[iid];
}

int TFunction::ioId( const string &id )
{
    for(int i_io = 0; i_io < (int)mIO.size(); i_io++)
	if(mIO[i_io]->id() == id) return i_io;

    return -1;
}

void TFunction::ioList( vector<string> &list )
{
    for(unsigned i_io = 0; i_io < mIO.size(); i_io++)
	list.push_back(mIO[i_io]->id());
}

void TFunction::ioAdd( IO *io )
{
    preIOCfgChange();
    mIO.push_back(io);
    io->owner = this;
    postIOCfgChange();
}

int TFunction::ioIns( IO *io, int pos )
{
    if(pos < 0 || pos > (int)mIO.size()) pos = mIO.size();

    preIOCfgChange();
    mIO.insert(mIO.begin()+pos,io);
    io->owner = this;
    postIOCfgChange();

    return pos;
}

void TFunction::ioDel( int pos )
{
    if(pos < 0 || pos >= (int)mIO.size())
	throw TError(nodePath().c_str(),_("Delete IO '%d' error."),pos);

    preIOCfgChange();
    mIO.erase(mIO.begin()+pos);
    postIOCfgChange();
}

void TFunction::ioMove( int pos, int to )
{
    if(pos < 0 || pos >= (int)mIO.size() || to < 0 || to >= (int)mIO.size())
	throw TError(nodePath().c_str(),_("Move IO from %d to %d error."),pos,to);

    preIOCfgChange();
    IO *io = mIO[to];
    mIO[to] = mIO[pos];
    mIO[pos] = io;
    postIOCfgChange();
}

void TFunction::preIOCfgChange( )
{
    //Previous stop
    be_start = startStat();
    if(be_start) {
	setStart(false);
	if(mTVal) { delete mTVal; mTVal = NULL; }
    }

    //Main process
    string blk_lst;
    for(unsigned i = 0; i < used.size(); i++)
	if(used[i]->blk()) blk_lst += used[i]->vfName()+",";
    if(blk_lst.size())
	throw TError(nodePath().c_str(),_("Change is not permitted while function is used: %s"),blk_lst.c_str());

    for(unsigned i = 0; i < used.size(); i++) used[i]->preIOCfgChange();
}

void TFunction::postIOCfgChange( )
{
    //Start for restore
    if(be_start) setStart(true);

    for(unsigned i=0; i < used.size(); i++) used[i]->postIOCfgChange();
}

void TFunction::valAtt( TValFunc *vfnc )
{
    ResAlloc res(nodeRes(), true);
    for(unsigned i = 0; i < used.size(); i++)
	if(used[i] == vfnc)
	    throw TError(nodePath().c_str(), _("Value '%s' is already attached!"), vfnc->vfName().c_str());
    used.push_back(vfnc);
}

void TFunction::valDet( TValFunc *vfnc )
{
    ResAlloc res(nodeRes(), true);
    for(unsigned i = 0; i < used.size() ;i++)
	if(used[i] == vfnc)
	{ used.erase(used.begin()+i); break; }
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
		ctrMkNode("fld",opt,-1,"/func/cfg/id",_("Id"),R_R_R_,"root",grp,1,"tp","str");
		ctrMkNode("fld",opt,-1,"/func/cfg/name",_("Name"),R_R_R_,"root",grp,1,"tp","str");
		ctrMkNode("fld",opt,-1,"/func/cfg/descr",_("Description"),R_R_R_,"root",grp,3,"tp","str","cols","90","rows","5");
	    }
	}
	if(ctrMkNode("area",opt,-1,"/io",_("IO")))
	    if(ctrMkNode("table",opt,-1,"/io/io",_("IO"),R_R_R_,"root",grp)) {
		ctrMkNode("list",opt,-1,"/io/io/0",_("Id"),R_R_R_,"root",grp,1,"tp","str");
		ctrMkNode("list",opt,-1,"/io/io/1",_("Name"),R_R_R_,"root",grp,1,"tp","str");
		ctrMkNode("list",opt,-1,"/io/io/2",_("Type"),R_R_R_,"root",grp,1,"tp","str");
		ctrMkNode("list",opt,-1,"/io/io/3",_("Mode"),R_R_R_,"root",grp,1,"tp","str");
		ctrMkNode("list",opt,-1,"/io/io/4",_("Hide"),R_R_R_,"root",grp,1,"tp","bool");
		ctrMkNode("list",opt,-1,"/io/io/5",_("Default"),R_R_R_,"root",grp,1,"tp","str");
	    }
	if(ctrMkNode("area",opt,-1,"/exec",_("Execute"),RWRW__,"root",grp)) {
	    ctrMkNode("fld",opt,-1,"/exec/en",_("Enable"),RWRW__,"root",grp,1,"tp","bool");
	    // Add test form
	    if(mTVal) {
		if(ctrMkNode("area",opt,-1,"/exec/io",_("IO")))
		    for(int i_io = 0; i_io < ioSize(); i_io++) {
			if(mIO[i_io]->hide()) continue;
			XMLNode *nd = ctrMkNode("fld",opt,-1,("/exec/io/"+io(i_io)->id()).c_str(),io(i_io)->name(),RWRW__,"root",grp);
			if(nd) {
			    switch(io(i_io)->type()) {
				case IO::String:
				    nd->setAttr("tp","str");
				    if(io(i_io)->flg()&IO::FullText) nd->setAttr("cols","100")->setAttr("rows","4");
				    break;
				case IO::Integer:	nd->setAttr("tp","dec");	break;
				case IO::Real:		nd->setAttr("tp","real");	break;
				case IO::Boolean:	nd->setAttr("tp","bool");	break;
				case IO::Object:	nd->setAttr("tp","str")->setAttr("cols","100")->setAttr("rows","4");	break;
			    }
			}
		    }
		// Add Calc button and Calc time
		ctrMkNode("fld",opt,-1,"/exec/n_clc",_("Number runs"),RWRW__,"root",grp,1,"tp","dec");
		ctrMkNode("fld",opt,-1,"/exec/tm",_("Spent time"),R_R___,"root",grp,1,"tp","str");
		ctrMkNode("comm",opt,-1,"/exec/calc",_("Execute"),RWRW__,"root",grp);
	    }
	}
        return;
    }

    //Process command to page
    string a_path = opt->attr("path");
    if(a_path == "/func/st/st") {
	if(ctrChkNode(opt,"get",RWRWR_,"root",grp,SEC_RD))	opt->setText(run_st?"1":"0");
	if(ctrChkNode(opt,"set",RWRWR_,"root",grp,SEC_WR))	setStart(s2i(opt->text()));
    }
    else if(a_path == "/func/st/use" && ctrChkNode(opt))	{ ResAlloc res(nodeRes(),false); opt->setText(i2s(used.size())); }
    else if(a_path == "/func/cfg/id" && ctrChkNode(opt))	opt->setText(id());
    else if(a_path == "/func/cfg/name" && ctrChkNode(opt))	opt->setText(name());
    else if(a_path == "/func/cfg/descr" && ctrChkNode(opt))	opt->setText(descr());
    else if(a_path == "/io/io" && ctrChkNode(opt,"get",R_R_R_,"root",grp,SEC_RD)) {
	XMLNode *n_id	= ctrMkNode("list",opt,-1,"/io/io/0","");
	XMLNode *n_nm	= ctrMkNode("list",opt,-1,"/io/io/1","");
	XMLNode *n_type	= ctrMkNode("list",opt,-1,"/io/io/2","");
	XMLNode *n_mode	= ctrMkNode("list",opt,-1,"/io/io/3","");
	XMLNode *n_hide	= ctrMkNode("list",opt,-1,"/io/io/4","");
	XMLNode *n_def	= ctrMkNode("list",opt,-1,"/io/io/5","");
	//XMLNode *n_vect	= ctrId(opt,"6");
	for(int i_io = 0; i_io < ioSize(); i_io++) {
	    string tmp_str;
	    if(n_id)	n_id->childAdd("el")->setText(io(i_io)->id());
	    if(n_nm)	n_nm->childAdd("el")->setText(io(i_io)->name());
	    if(n_type) {
		switch(io(i_io)->type()) {
		    case IO::String:	tmp_str = _("String");	break;
		    case IO::Integer:	tmp_str = _("Integer");	break;
		    case IO::Real:	tmp_str = _("Real");	break;
		    case IO::Boolean:	tmp_str = _("Bool");	break;
		    case IO::Object:	tmp_str = _("Object");	break;
		}
		n_type->childAdd("el")->setText(tmp_str);
	    }
	    if(n_mode) {
		if(io(i_io)->flg()&IO::Return)		tmp_str = _("Return");
		else if(io(i_io)->flg()&IO::Output)	tmp_str = _("Output");
		else					tmp_str = _("Input");
		n_mode->childAdd("el")->setText(tmp_str);
	    }
	    if(n_hide)	n_hide->childAdd("el")->setText(io(i_io)->hide()?"1":"0");
	    if(n_def)	n_def->childAdd("el")->setText(io(i_io)->def());
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
	if(ctrChkNode(opt,"get",RWRW__,"root",grp,SEC_RD))	opt->setText(TBDS::genDBGet(nodePath()+"ntCalc","10",opt->attr("user")));
	if(ctrChkNode(opt,"set",RWRW__,"root",grp,SEC_WR))	TBDS::genDBSet(nodePath()+"ntCalc",opt->text(),opt->attr("user"));
    }
    else if(a_path == "/exec/tm" && mTVal && ctrChkNode(opt,"get",R_R___,"root",grp,SEC_RD))
	opt->setText(TSYS::time2str(SYS->cntrGet(nodePath('.'))));
    else if(a_path.substr(0,8) == "/exec/io" && mTVal) {
	string io_id = TSYS::pathLev(a_path,2);
	for(unsigned i_io = 0; i_io < mIO.size(); i_io++)
	    if(io_id == io(i_io)->id()) {
		if(ctrChkNode(opt,"get",RWRW__,"root",grp,SEC_RD))
		    opt->setText( (mTVal->ioType(i_io)==IO::Real) ? r2s(mTVal->getR(i_io),6) : mTVal->getS(i_io) );
		if(ctrChkNode(opt,"set",RWRW__,"root",grp,SEC_WR))
		    mTVal->setS(i_io,opt->text());
		break;
	    }
    }
    else if(a_path == "/exec/calc" && mTVal && ctrChkNode(opt,"set",RWRW__,"root",grp,SEC_WR)) {
	int n_tcalc = s2i(TBDS::genDBGet(nodePath()+"ntCalc","10",opt->attr("user")));
	string wuser = opt->attr("user");
	time_t tm_lim = SYS->sysTm()+STD_WAIT_TM;
	int64_t t_cnt = TSYS::curTime();
	for(int i_c = 0; i_c < n_tcalc && SYS->sysTm() < tm_lim; i_c++)
	    mTVal->calc(wuser);
	t_cnt = TSYS::curTime()-t_cnt;
	SYS->cntrSet(nodePath('.'), t_cnt);
    }
    else TCntrNode::cntrCmdProc(opt);
}

//*************************************************
//* IO                                            *
//*************************************************
IO::IO( const char *iid, const char *iname, IO::Type itype,  unsigned iflgs, const char *idef, bool ihide, const char *irez )
{
    mId		= iid;
    mName	= iname;
    mType	= itype;
    mFlg	= iflgs;
    mHide	= ihide;
    mDef	= idef;
    mRez	= irez;
}

IO &IO::operator=( IO &iio )
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
    exCtx(NULL), mName(iname), mUser(iuser), mBlk(iblk), mMdfChk(false), mFunc(NULL)
{
    pthread_mutex_init(&mRes, NULL);
    setFunc(ifunc);
}

TValFunc::~TValFunc( )
{
    if(mFunc) funcDisConnect();
    pthread_mutex_lock(&mRes);
    pthread_mutex_destroy(&mRes);
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
	for(int i_vl = 0; i_vl < mFunc->ioSize(); i_vl++) {
	    SVl val;
	    val.tp = mFunc->io(i_vl)->type();
	    val.mdf = false;
	    switch(val.tp) {
		case IO::String:	val.val.s = new string(mFunc->io(i_vl)->def());	break;
		case IO::Integer:	val.val.i = s2ll(mFunc->io(i_vl)->def());	break;
		case IO::Real:		val.val.r = s2r(mFunc->io(i_vl)->def());	break;
		case IO::Boolean:	val.val.b = s2i(mFunc->io(i_vl)->def());	break;
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
	for(unsigned i_vl = 0; i_vl < mVal.size(); i_vl++)
	    switch( mVal[i_vl].tp ) {
		case IO::String: delete mVal[i_vl].val.s;	break;
		case IO::Object: delete mVal[i_vl].val.o;	break;
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
    if(!mFunc)	throw TError("ValFnc",_("IO '%s' is not present!"),iid.c_str());

    return mFunc->ioId(iid);
}

void TValFunc::ioList( vector<string> &list )
{
    if(!mFunc)	throw TError("ValFnc",_("Function is not attached!"));

    return mFunc->ioList(list);
}

int TValFunc::ioSize( )
{
    if(!mFunc)	throw TError("ValFnc",_("Function is not attached!"));

    return mFunc->ioSize();
}

TVariant TValFunc::get( unsigned id )
{
    if(id >= mVal.size())	throw TError("ValFnc",_("%s: Id or IO %d error!"),"getS()",id);
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
    if(id >= mVal.size()) throw TError("ValFnc",_("%s: Id or IO %d error!"),"getS()",id);
    switch(mVal[id].tp) {
	case IO::Integer: { int64_t tvl = getI(id);return (tvl!=EVAL_INT) ? ll2s(tvl) : EVAL_STR; }
	case IO::Real:	  { double tvl = getR(id); return (tvl!=EVAL_REAL) ? r2s(tvl) : EVAL_STR; }
	case IO::Boolean: { char tvl = getB(id);   return (tvl!=EVAL_BOOL) ? i2s((bool)tvl) : EVAL_STR; }
	case IO::Object:  return getO(id).at().getStrXML();
	case IO::String: {
	    pthread_mutex_lock(&mRes);
	    string tvl(mVal[id].val.s->data(), mVal[id].val.s->size());
	    pthread_mutex_unlock(&mRes);
	    return tvl;
	}
    }

    return EVAL_STR;
}

int64_t TValFunc::getI( unsigned id )
{
    if(id >= mVal.size()) throw TError("ValFnc",_("%s: Id or IO %d error!"),"getI()",id);
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
    if(id >= mVal.size()) throw TError("ValFnc",_("%s: Id or IO %d error!"),"getR()",id);
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
    if(id >= mVal.size()) throw TError("ValFnc",_("%s: Id or IO %d error!"),"getB()",id);
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
    if(id >= mVal.size()) throw TError("ValFnc",_("%s: Id or IO %d error!"),"getO()",id);
    if(mVal[id].tp != IO::Object) throw TError("ValFnc",_("Get object from not object's IO %d error!"),id);
    pthread_mutex_lock(&mRes);
    AutoHD<TVarObj> rez = *mVal[id].val.o;
    pthread_mutex_unlock(&mRes);

    return rez;
}

void TValFunc::set( unsigned id, const TVariant &val )
{
    if(id >= mVal.size())	throw TError("ValFnc",_("%s: Id or IO %d error!"),"setS()",id);
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
    if(id >= mVal.size())	throw TError("ValFnc",_("%s: Id or IO %d error!"),"setS()",id);
    switch(mVal[id].tp) {
	case IO::Integer:	setI(id, (val!=EVAL_STR) ? s2ll(val) : EVAL_INT);	break;
	case IO::Real:		setR(id, (val!=EVAL_STR) ? s2r(val) : EVAL_REAL);	break;
	case IO::Boolean:	setB(id, (val!=EVAL_STR) ? (bool)s2i(val) : EVAL_BOOL);	break;
	case IO::Object:
	    setO(id, (val!=EVAL_STR) ? TVarObj::parseStrXML(val,NULL,*mVal[id].val.o) : new TEValObj());
	    break;
	case IO::String:
	    pthread_mutex_lock(&mRes);
	    if(mdfChk() && val != *mVal[id].val.s) mVal[id].mdf = true;
	    mVal[id].val.s->assign(val.data(), val.size());;
	    pthread_mutex_unlock(&mRes);
	    break;
    }
}

void TValFunc::setI( unsigned id, int64_t val )
{
    if(id >= mVal.size())	throw TError("ValFnc",_("%s: Id or IO %d error!"),"setI()",id);
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
    if(id >= mVal.size())	throw TError("ValFnc",_("%s: Id or IO %d error!"),"setR()",id);
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
    if(id >= mVal.size())	throw TError("ValFnc",_("%s: Id or IO %d error!"),"setB()",id);
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
    if(id >= mVal.size()) throw TError("ValFnc",_("%s: Id or IO %d error!"),"setO()",id);
    switch(mVal[id].tp) {
	case IO::String:	setS(id, val.at().getStrXML());	break;
	case IO::Integer: case IO::Real: case IO::Boolean:
				setB(id, true);			break;
	case IO::Object:
	    pthread_mutex_lock(&mRes);
	    if(mdfChk() && !(val == *mVal[id].val.o)) mVal[id].mdf = true;
	    *mVal[id].val.o = val;
	    pthread_mutex_unlock(&mRes);
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
    if(apos != -1) vf.set(apos,val);
}

string TFuncArgsObj::getStrXML( const string &oid )
{
    string nd("<TFuncArgsObj");
    if(!oid.empty()) nd = nd + " p='" + oid + "'";
    nd = nd + ">\n";
    for(int i_io = 0; vf.func() && i_io < vf.ioSize(); i_io++)
	switch(vf.ioType(i_io)) {
	    case IO::String:	nd += "<str id='"+vf.func()->io(i_io)->id()+"'>"+TSYS::strEncode(vf.getS(i_io),TSYS::Html)+"</str>\n"; break;
	    case IO::Integer:	nd += "<int id='"+vf.func()->io(i_io)->id()+"'>"+vf.getS(i_io)+"</int>\n"; break;
	    case IO::Real:	nd += "<real id='"+vf.func()->io(i_io)->id()+"'>"+vf.getS(i_io)+"</real>\n"; break;
	    case IO::Boolean:	nd += "<bool id='"+vf.func()->io(i_io)->id()+"'>"+vf.getS(i_io)+"</bool>\n"; break;
	    case IO::Object:	nd += vf.getO(i_io).at().getStrXML(vf.func()->io(i_io)->id()); break;
	}
    nd += "</TFuncArgsObj>\n";

    return nd;
}
