
//OpenSCADA module DAQ.BlockCalc file: block.cpp
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
#include <tmess.h>
#include <math.h>

#include "virtual.h"
#include "block.h"

using namespace Virtual;

//*************************************************
//* Block: Function block                         *
//*************************************************
Block::Block( const string &iid, Contr *iown ) :
    TCntrNode(iown), TValFunc(iid+"_block",NULL), TConfig(&((TpContr&)iown->owner()).blockE()), mEnable(false), mProcess(false),
    mId(cfg("ID")), mToEn(cfg("EN").getBd()), mToPrc(cfg("PROC").getBd()), mOutLnkWrChs(cfg("LNK_OUT_WR_CH").getBd()),
    idFreq(-1), idStart(-1), idStop(-1)
{
    mId = iid;
}

Block::~Block( )
{
    if(enable()) setEnable(false);
}

TCntrNode &Block::operator=( const TCntrNode &node )
{
    const Block *src_n = dynamic_cast<const Block*>(&node);
    if(!src_n) return *this;

    //Copy parameters
    exclCopy(*src_n, "ID;");

    //Copy IO and links
    if(src_n->enable()) {
	setEnable(true);
	loadIO(src_n->owner().DB()+"."+src_n->owner().cfg("BLOCK_SH").getS(), src_n->id(), true);
    }

    return *this;
}

void Block::preDisable( int flag )
{
    if(process()) setProcess(false);
}

void Block::postDisable( int flag )
{
    if(flag&NodeRemove) {
	//Delete block from BD
	string tbl = owner().DB()+"."+owner().cfg("BLOCK_SH").getS();
	TBDS::dataDel(tbl, mod->nodePath()+owner().cfg("BLOCK_SH").getS(), *this, TBDS::UseAllKeys);

	//Delete block's IO from BD
	TConfig cfg(&owner().owner().blockIOE());
	tbl = tbl+"_io";
	cfg.cfg("BLK_ID").setS(id(), true);		//Delete all block id records
	TBDS::dataDel(tbl, mod->nodePath()+owner().cfg("BLOCK_SH").getS()+"_io", cfg);
    }
}

Contr &Block::owner( ) const	{ return *(Contr*)nodePrev(); }

string Block::name( )
{
    string rez = cfg("NAME").getS();
    return rez.size() ? rez : id();
}

void Block::load_( TConfig *icfg )
{
    if(!SYS->chkSelDB(owner().DB())) throw TError();

    if(icfg) *(TConfig*)this = *icfg;
    else TBDS::dataGet(owner().DB()+"."+owner().cfg("BLOCK_SH").getS(), mod->nodePath()+owner().cfg("BLOCK_SH").getS(), *this);

    //Load IO config
    loadIO();
}

void Block::save_( )
{
    string bd = owner().DB()+"."+owner().cfg("BLOCK_SH").getS();
    TBDS::dataSet(bd, mod->nodePath()+owner().cfg("BLOCK_SH").getS(), *this);

    //Save IO config
    saveIO();
}

void Block::loadIO( const string &blk_db, const string &blk_id, bool force )
{
    string bd_tbl, bd;
    if(!func()) return;
    if(owner().startStat() && !force) { modif(true); return; }	//Load/reload IO context only allow for stoped controlers for prevent throws

    TConfig cfg(&mod->blockIOE());
    cfg.cfg("BLK_ID").setS((blk_id.size())?blk_id:id(), TCfg::ForceUse);
    if(blk_db.empty()) {
	bd_tbl	= owner().cfg("BLOCK_SH").getS()+"_io";
	bd	= owner().DB()+"."+bd_tbl;
    }
    else {
	bd	= blk_db+"_io";
	bd_tbl	= TSYS::strSepParse(bd,2,'.');
    }

    //Links first init
    while(ioSize() > (int)mLnk.size()) { mLnk.push_back(SLnk()); mLnk.back().tp = FREE; }

    //IO values loading and links set, by seek
    for(int fldCnt = 0; TBDS::dataSeek(bd,mod->nodePath()+bd_tbl,fldCnt++,cfg,TBDS::UseCache); ) {
	int io = func()->ioId(cfg.cfg("ID").getS());
	if(io < 0) continue;
	setS(io, cfg.cfg("VAL").getS());
	setLink(io, SET, (LnkT)cfg.cfg("TLNK").getI(), cfg.cfg("LNK").getS());
    }
}

void Block::saveIO( )
{
    if(!enable()) return;

    TConfig cfg(&mod->blockIOE());
    cfg.cfg("BLK_ID").setS(id());
    string bd_tbl = owner().cfg("BLOCK_SH").getS()+"_io";
    string bd = owner().DB()+"."+bd_tbl;

    for(unsigned iLn = 0; iLn < mLnk.size(); iLn++)
	try {
	    cfg.cfg("ID").setS(func()->io(iLn)->id());
	    cfg.cfg("TLNK").setI(mLnk[iLn].tp);				//Type link
	    cfg.cfg("LNK").setS((mLnk[iLn].tp == FREE)?"":mLnk[iLn].lnk);	//Link
	    cfg.cfg("VAL").setS(getS(iLn));					//Value

	    TBDS::dataSet(bd, mod->nodePath()+bd_tbl, cfg);
	} catch(TError &err) {
	    mess_err(err.cat.c_str(),"%s",err.mess.c_str());
	    mess_err(nodePath().c_str(),_("Block link '%s' save error."),func()->io(iLn)->id().c_str());
	}
}

void Block::setEnable( bool val )
{
    //Enable
    if(val && !mEnable) {
	if(!func()) {
	    if(!dynamic_cast<TFunction *>(&SYS->nodeAt(wFunc(),0,'.').at()))
		throw TError(nodePath().c_str(),_("Node '%s' is not function."),wFunc().c_str());
	    setFunc((TFunction *)&SYS->nodeAt(wFunc(),0,'.').at());
	    // Init system attributes identifiers
	    idFreq  = func()->ioId("f_frq");
	    idStart = func()->ioId("f_start");
	    idStop  = func()->ioId("f_stop");
	    int idThis = func()->ioId("this");
	    if(idThis >= 0) setO(idThis, new TCntrNodeObj(AutoHD<TCntrNode>(this),"root"));
	}
	// Init links
	loadIO("", "", true);
    }
    //Disable
    else if(!val && mEnable) {
	if(process()) setProcess(false);
	// Save IO config
	//saveIO();

	// Clean IO
	for(unsigned iLn = 0; iLn < mLnk.size(); iLn++) setLink(iLn, SET, FREE);
	mLnk.clear();

	// Free func
	setFunc(NULL);
	idFreq = idStart = idStop = -1;
    }
    mEnable = val;
}

void Block::setProcess( bool val )
{
    if(val && !enable()) setEnable(true);

    //Connect links
    if(val && !process()) {
	for(unsigned iLn = 0; iLn < mLnk.size(); iLn++) setLink(iLn, INIT);
	if(owner().startStat()) calc(true, false, 0);
	owner().blkProc(id(), val);
    }
    //Disconnect links
    if(!val && process()) {
	owner().blkProc(id(), val);
	if(owner().startStat()) calc(false, true, 0);
	for(unsigned iLn = 0; iLn < mLnk.size(); iLn++) setLink(iLn, DEINIT);
    }
    mProcess = val;
}

Block::LnkT Block::link( unsigned iid )
{
    if(iid >= mLnk.size()) throw TError(nodePath().c_str(),_("Link %d error!"),iid);
    return mLnk[iid].tp;
}

bool Block::linkActive( unsigned iid )
{
    ResAlloc res(lnkRes,false);
    if(iid >= mLnk.size()) throw TError(nodePath().c_str(),_("Link %d error!"),iid);

    switch(mLnk[iid].tp) {
	case I_LOC: case I_GLB:	return !mLnk[iid].iblk->wBl.freeStat();
	case I_PRM:		return !mLnk[iid].aprm->freeStat();
	default: return false;
    }
    return false;
}

void Block::setLink( unsigned iid, LnkCmd cmd, LnkT lnk, const string &vlnk )
{
    ResAlloc res(lnkRes, true);
    if(iid >= mLnk.size()) throw TError(nodePath().c_str(),_("Link %d error!"),iid);

    //Change type link
    if(cmd == SET) {
	if(lnk != mLnk[iid].tp) {
	    // Free old structures
	    switch(mLnk[iid].tp) {
		case I_LOC: case I_GLB:	case O_LOC: case O_GLB: delete mLnk[iid].iblk;	break;
		case I_PRM: case O_PRM: delete mLnk[iid].aprm;				break;
		default: break;
	    }

	    // Make new structures
	    switch(lnk) {
		case I_LOC: case I_GLB:	case O_LOC: case O_GLB: mLnk[iid].iblk = new SLIBlk();	break;
		case I_PRM: case O_PRM: mLnk[iid].aprm = new AutoHD<TVal>;			break;
		default: break;
	    }
	    mLnk[iid].tp = lnk;
	}
	mLnk[iid].lnk = vlnk;
    }
    //Connect new link and init
    if(cmd == INIT || (cmd == SET && process())) {
	string lo1 = TSYS::strSepParse(mLnk[iid].lnk,0,'.');
	string lo2 = TSYS::strSepParse(mLnk[iid].lnk,1,'.');
	string lo3 = TSYS::strSepParse(mLnk[iid].lnk,2,'.');
	string lo4 = TSYS::strSepParse(mLnk[iid].lnk,3,'.');

	switch(mLnk[iid].tp) {
	    case I_LOC: case O_LOC:
		mLnk[iid].iblk->wBl.free();
		if(owner().blkPresent(lo1) && owner().blkAt(lo1).at().ioId(lo2) >= 0) {
		    mLnk[iid].iblk->wBl = owner().blkAt(lo1);
		    mLnk[iid].iblk->wId = mLnk[iid].iblk->wBl.at().ioId(lo2);
		}
		break;
	    case I_GLB: case O_GLB:
		mLnk[iid].iblk->wBl.free();
		if(owner().owner().present(lo1) &&
		    ((Contr &)owner().owner().at(lo1).at()).blkPresent(lo2) &&
		    ((Contr &)owner().owner().at(lo1).at()).blkAt(lo2).at().ioId(lo3) >= 0)
		{
		    mLnk[iid].iblk->wBl = ((Contr &)owner().owner().at(lo1).at()).blkAt(lo2);
		    mLnk[iid].iblk->wId = mLnk[iid].iblk->wBl.at().ioId(lo3);
		}
		break;
	    case I_PRM: case O_PRM:
		*mLnk[iid].aprm = SYS->daq().at().attrAt(mLnk[iid].lnk, '.', true);
		if(mLnk[iid].aprm->freeStat()) *mLnk[iid].aprm = SYS->nodeAt(mLnk[iid].lnk, 0, '.', 0, true);
		break;
	    default: break;
	}
    }
    //Disconnect
    if(cmd == DEINIT)
	switch(mLnk[iid].tp) {
	    case I_LOC: case I_GLB: case O_LOC: case O_GLB: mLnk[iid].iblk->wBl.free();	break;
	    case I_PRM: case O_PRM: mLnk[iid].aprm->free();					break;
	    default: break;
	}
}

void Block::calc( bool first, bool last, double frq )
{
    //Set fixed system attributes
    if(idFreq >= 0)	setR(idFreq, frq);
    if(idStart >= 0)	setB(idStart, first);
    if(idStop >= 0)	setB(idStop, last);

    //Get values from input links
    lnkRes.resRequestR();
    try {
	// Get input links
	for(unsigned iLn = 0; iLn < mLnk.size(); iLn++)
	    switch(mLnk[iLn].tp) {
		case I_LOC: case I_GLB:
		    if(mLnk[iLn].iblk->wBl.freeStat()) break;
		    if(mLnk[iLn].iblk->wBl.at().enable()) {
			//  Early disconnected link init try
			if(mLnk[iLn].iblk->wId == -100) {
			    lnkRes.resRelease();
			    try{ setLink(iLn, INIT); } catch(...) { setLink(iLn, DEINIT); }
			    lnkRes.resRequestR();
			    if(mLnk[iLn].iblk->wBl.freeStat()) break;
			}
			//  Use link
			set(iLn,mLnk[iLn].iblk->wBl.at().get(mLnk[iLn].iblk->wId));
		    }
		    //  Check for link disable need
		    else mLnk[iLn].iblk->wId = -100;
		    break;
		case I_PRM:
		    if(mLnk[iLn].aprm->freeStat()) break;
		    set(iLn, mLnk[iLn].aprm->at().get());
		    break;
		default: break;
	    }
    } catch(TError &err) {
	mErrCnt++;
	lnkRes.resRelease();
	mess_err(err.cat.c_str(),"%s",err.mess.c_str());
	throw TError(nodePath().c_str(),_("Error reading block's '%s' links."),id().c_str());
    }
    lnkRes.resRelease( );

    //Calc function
    try {
	setMdfChk(outLnkWrChs());
	TValFunc::calc();
	if(SYS->modifCalc()) modif();
    } catch(TError &err) { mErrCnt++; throw; }

    //Put values to output links
    lnkRes.resRequestR();
    try {
	for(unsigned iLn = 0; iLn < mLnk.size(); iLn++)
	    switch(mLnk[iLn].tp) {
		case O_LOC: case O_GLB:
		    if(mLnk[iLn].iblk->wBl.freeStat() || (outLnkWrChs() && !ioMdf(iLn))) break;
		    if(mLnk[iLn].iblk->wBl.at().enable()) {
			//Early disconnected link init try
			if(mLnk[iLn].iblk->wId == -100) {
			    lnkRes.resRelease();
			    try{ setLink(iLn, INIT); } catch(...) { setLink(iLn, DEINIT); }
			    lnkRes.resRequestR();
			    if(mLnk[iLn].iblk->wBl.freeStat()) break;
			}
			//Use link
			mLnk[iLn].iblk->wBl.at().set(mLnk[iLn].iblk->wId,get(iLn));
		    }
		    //Check for link disable need
		    else mLnk[iLn].iblk->wId = -100;
		    break;
		case O_PRM:
		    if(mLnk[iLn].aprm->freeStat() || (outLnkWrChs() && !ioMdf(iLn))) break;
		    mLnk[iLn].aprm->at().set(get(iLn));
		    break;
		default: break;
	    }
    } catch(TError &err) {
	mErrCnt++;
	lnkRes.resRelease();
	throw TError(nodePath().c_str(),_("Error writing block's '%s' links."),id().c_str());
    }
    lnkRes.resRelease();
    mErrCnt = 0;
}

void Block::cntrCmdProc( XMLNode *opt )
{
    //Service commands process
    string a_path = opt->attr("path");
    if(a_path.find("/serv/") == 0) {
	if(a_path == "/serv/attr") {
	    if(!enable() || !func()) throw TError(nodePath(), _("Block disabled or error."));
	    if(ctrChkNode(opt,"get",RWRWR_,"root",SDAQ_ID,SEC_RD) )
		for(int iA = 0; iA < ioSize(); iA++)
		    opt->childAdd("a")->setAttr("id",func()->io(iA)->id())->setText(getS(iA));
	    if(ctrChkNode(opt,"set",RWRWR_,"root",SDAQ_ID,SEC_WR))
		for(unsigned iA = 0; iA < opt->childSize(); iA++) {
		    int io_id = -1;
		    if(opt->childGet(iA)->name() != "a" || (io_id=ioId(opt->childGet(iA)->attr("id"))) < 0) continue;
		    setS(io_id, opt->childGet(iA)->text());
		}
	}
	else TCntrNode::cntrCmdProc(opt);
	return;
    }

    //Get page info
    if(opt->name() == "info") {
	TCntrNode::cntrCmdProc(opt);
	ctrMkNode("oscada_cntr",opt,-1,"/",_("Block: ")+id(),RWRWR_,"root",SDAQ_ID);
	if(ctrMkNode("area",opt,-1,"/blck",_("Block"))) {
	    if(owner().enableStat() && ctrMkNode("area",opt,-1,"/blck/st",_("State"))) {
		ctrMkNode("fld",opt,-1,"/blck/st/en",_("Enabled"),RWRWR_,"root",SDAQ_ID,1,"tp","bool");
		if(owner().startStat())
		    ctrMkNode("fld",opt,-1,"/blck/st/prc",_("Process"),RWRWR_,"root",SDAQ_ID,1,"tp","bool");
	    }
	    if(ctrMkNode("area",opt,-1,"/blck/cfg",_("Configuration"))) {
		TConfig::cntrCmdMake(opt,"/blck/cfg",0,"root",SDAQ_ID,RWRWR_);
		ctrMkNode("fld",opt,-1,"/blck/cfg/PRIOR",EVAL_STR,RWRWR_,"root",SDAQ_ID,3,"dest","sel_ed","select","/blck/cfg/blks",
		    "help",_("Priority block(s) calculate before this block. Blocks list is separated by symbol ';'."));
		ctrMkNode("fld",opt,-1,"/blck/cfg/FUNC",EVAL_STR,func()?R_R_R_:RWRWR_,"root",SDAQ_ID,2,"dest","sel_ed","select","/blck/cfg/fncs");
		ctrMkNode("comm",opt,-1,"/blck/cfg/func_lnk",_("Go to function"),wFunc().empty()?0:RWRW__,"root",SDAQ_ID,1,"tp","lnk");
	    }
	}
	if(enable()) {
	    if(ctrMkNode("area",opt,-1,"/lio",_("IO"))) {
		ctrMkNode("area",opt,-1,"/lio/show",_("Show"));
		ctrMkNode("fld",opt,-1,"/lio/show/hide",_("Hidden"),RWRWR_,"root",SDAQ_ID,1,"tp","bool");
		if(ctrMkNode("area",opt,-1,"/lio/io",_("IO"))) {
		    vector<string> list;
		    ioList(list);
		    for(unsigned i_io = 0; i_io < list.size(); i_io++) {
			int id = ioId(list[i_io]);
			if(ioHide(id) && !s2i(TBDS::genPrmGet(owner().nodePath()+"showHide",DEF_showHide,opt->attr("user")))) continue;
			const char *tip = "str";
			switch(ioType(id)) {
			    case IO::String:	tip = "str";	break;
			    case IO::Integer:	tip = "dec";	break;
			    case IO::Real:	tip = "real";	break;
			    case IO::Boolean:	tip = "bool";	break;
			    case IO::Object:	tip = "str";	break;
			}
			ctrMkNode("fld",opt,-1,(string("/lio/io/")+list[i_io]).c_str(),
				func()->io(id)->name().c_str(),linkActive(id)?R_R_R_:RWRWR_,"root",SDAQ_ID,1,"tp",tip);
		    }
		}
	    }
	    if(ctrMkNode("area",opt,-1,"/lnk",_("Links"))) {
		ctrMkNode("area",opt,-1,"/lnk/show",_("Show"));
		ctrMkNode("fld",opt,-1,"/lnk/show/hide",_("Hidden"),RWRWR_,"root",SDAQ_ID,1,"tp","bool");
		if(ctrMkNode("area",opt,-1,"/lnk/io",_("IO"))) {
		    vector<string> list;
		    ioList(list);
		    // Put IO links
		    for(unsigned i_io = 0; i_io < list.size(); i_io++) {
			int id = ioId(list[i_io]);

			if(ioHide(id) && !s2i(TBDS::genPrmGet(owner().nodePath()+"showHide",DEF_showHide,opt->attr("user")))) continue;

			// Add link's type
			ctrMkNode("fld",opt,-1,(string("/lnk/io/1|")+list[i_io]).c_str(),
			    func()->io(id)->name().c_str(),RWRWR_,"root",SDAQ_ID,3,"tp","dec","dest","select","select",(ioFlg(id)&(IO::Output|IO::Return))?"/lnk/otp":"/lnk/itp");
			if(mLnk[id].tp != FREE)
			    ctrMkNode("fld",opt,-1,(string("/lnk/io/2|")+list[i_io]).c_str(),"",RWRWR_,"root",SDAQ_ID,3,"tp","str","dest","sel_ed","select",(string("/lnk/io/3|")+list[i_io]).c_str());
		    }
		}
	    }
	}
	return;
    }
    //Process command to page
    if(a_path == "/blck/st/en") {
	if(ctrChkNode(opt,"get",RWRWR_,"root",SDAQ_ID,SEC_RD))	opt->setText(enable()?"1":"0");
	if(ctrChkNode(opt,"set",RWRWR_,"root",SDAQ_ID,SEC_WR))	setEnable(s2i(opt->text()));
    }
    else if(a_path == "/blck/st/prc") {
	if(ctrChkNode(opt,"get",RWRWR_,"root",SDAQ_ID,SEC_RD))	opt->setText(process()?"1":"0");
	if(ctrChkNode(opt,"set",RWRWR_,"root",SDAQ_ID,SEC_WR))	setProcess(s2i(opt->text()));
    }
    else if(a_path == "/blck/cfg/blks" && ctrChkNode(opt)) {
	vector<string> list;
	owner().blkList(list);
	opt->childAdd("el")->setText("");
	for(unsigned iB = 0; iB < list.size(); iB++)
	    if(list[iB] != id() && prior() != list[iB] && prior().find(";"+list[iB]) == string::npos && prior().find(list[iB]+";") == string::npos)
		opt->childAdd("el")->setText((prior().size()?prior()+";":"")+list[iB]);
    }
    else if(a_path == "/blck/cfg/FUNC") {
	if(ctrChkNode(opt,"get",RWRWR_,"root",SDAQ_ID,SEC_RD)) {
	    opt->setText(wFunc());
	    try{ if(dynamic_cast<TFunction*>(&SYS->nodeAt(wFunc(),0,'.').at())) opt->setText(opt->text()+" (+)"); }
	    catch(TError&) { }
	}
	if(!func() && ctrChkNode(opt,"set",RWRWR_,"root",SDAQ_ID,SEC_WR)) setWFunc(TSYS::strParse(opt->text(),0," "));
    }
    else if(a_path == "/blck/cfg/func_lnk" && ctrChkNode(opt,"get",R_R___,"root",SDAQ_ID,SEC_RD))
	opt->setText(TSYS::sepstr2path(wFunc(),'.'));
    else if(a_path == "/blck/cfg/fncs" && ctrChkNode(opt)) {
	vector<string> list;
	int c_lv = 0;
	string c_path = "", c_el;
	for(int c_off = 0; (c_el=TSYS::strSepParse(wFunc(),0,'.',&c_off)).size(); c_lv++) {
	    opt->childAdd("el")->setText(c_path);
	    c_path += c_lv ? "."+c_el : c_el;
	}
	opt->childAdd("el")->setText(c_path);
	if(c_lv != 0) c_path += ".";
	try { SYS->nodeAt(c_path,0,'.').at().nodeList(list); }	catch(...){ }
	for(unsigned iA = 0; iA < list.size(); iA++)
	    opt->childAdd("el")->setText(c_path+list[iA]);
    }
    else if(a_path.compare(0,9,"/blck/cfg") == 0) TConfig::cntrCmdProc(opt, TSYS::pathLev(a_path,2), "root", SDAQ_ID, RWRWR_);
    else if((a_path == "/lio/show/hide" || a_path == "/lnk/show/hide") && enable()) {
	if(ctrChkNode(opt,"get",RWRWR_,"root",SDAQ_ID,SEC_RD))	opt->setText(TBDS::genPrmGet(owner().nodePath()+"showHide",DEF_showHide,opt->attr("user")));
	if(ctrChkNode(opt,"set",RWRWR_,"root",SDAQ_ID,SEC_WR))	TBDS::genPrmSet(owner().nodePath()+"showHide",opt->text(),opt->attr("user"));
    }
    else if(a_path.substr(0,7) == "/lio/io" && enable()) {
	int id = ioId(TSYS::pathLev(a_path,2));
	if(ctrChkNode(opt,"get",RWRWR_,"root",SDAQ_ID,SEC_RD))
	    opt->setText((ioType(id)==IO::Real) ? r2s(getR(id),6) : getS(id));
	if(!linkActive(id) && ctrChkNode(opt,"set",RWRWR_,"root",SDAQ_ID,SEC_WR)) { setS(id,opt->text()); modif(); }
    }
    else if(a_path.substr(0,7) == "/lnk/io" && enable()) {
	if(ctrChkNode(opt,"get",RWRWR_,"root",SDAQ_ID,SEC_RD)) {
	    vector<string> list;
	    char lev = TSYS::pathLev(a_path,2)[0];
	    int id   = ioId(TSYS::pathLev(a_path,2).substr(2));
	    string lnk = mLnk[id].lnk;

	    switch(lev) {
		case '1':	opt->setText(i2s(mLnk[id].tp));	break;
		case '2':
		    opt->setText(lnk);
		    try {
			switch(mLnk[id].tp) {
			    case I_LOC: case O_LOC:
				if(owner().blkAt(TSYS::strParse(lnk,0,".")).at().ioId(TSYS::strParse(lnk,1,".")) >= 0)
				    opt->setText(opt->text()+" (+)");
				break;
			    case I_GLB: case O_GLB:
				if(((Contr&)owner().owner().at(TSYS::strParse(lnk,0,".")).at()).blkAt(TSYS::strParse(lnk,1,".")).at().ioId(TSYS::strParse(lnk,2,".")) >= 0)
				    opt->setText(opt->text()+" (+)");
				break;
			    case I_PRM: case O_PRM:
				if(!SYS->daq().at().attrAt(lnk,'.',true).freeStat()) opt->setText(opt->text()+" (+)");
				break;
			    default: break;
			}
		    } catch(TError&) { }
		    break;
		case '3':
		    if(mLnk[id].tp == I_PRM || mLnk[id].tp == O_PRM) SYS->daq().at().ctrListPrmAttr(opt, lnk, false, '.');
		    else {
			int c_lv = 0;
			string c_path = "", c_el;

			opt->childAdd("el")->setText(c_path);
			for(int c_off = 0; (c_el=TSYS::strSepParse(lnk,0,'.',&c_off)).size(); ++c_lv) {
			    c_path += c_lv ? "."+c_el : c_el;
			    opt->childAdd("el")->setText(c_path);
			}
			if(c_lv) c_path += ".";

			string prm0 = TSYS::strSepParse(lnk, 0, '.');
			string prm1 = TSYS::strSepParse(lnk, 1, '.');
			string prm2 = TSYS::strSepParse(lnk, 2, '.');

			switch(mLnk[id].tp) {
			    case I_LOC: case O_LOC:
				switch(c_lv) {
				    case 0: owner().blkList(list); break;
				    case 1:
					if(owner().blkPresent(prm0) && owner().blkAt(prm0).at().func())
					    owner().blkAt(prm0).at().ioList(list);
					break;
				}
				break;
			    case I_GLB: case O_GLB:
				switch(c_lv) {
				    case 0: owner().owner().list(list); break;
				    case 1:
					if(owner().owner().present(prm0))
					    ((Contr &)owner().owner().at(prm0).at()).blkList(list);
					break;
				    case 2:
					if(owner().owner().present(prm0) && ((Contr &)owner().owner().at(prm0).at()).blkPresent(prm1))
					    ((Contr&)owner().owner().at(prm0).at()).blkAt(prm1).at().ioList(list);
					break;
				}
				break;
			    default:	break;
			}
			for(unsigned iA = 0; iA < list.size(); iA++)
			    opt->childAdd("el")->setText(c_path+list[iA]);
		    }
	    }
	}
	if(ctrChkNode(opt,"set",RWRWR_,"root",SDAQ_ID,SEC_WR)) {
	    char lev = TSYS::pathLev(a_path,2)[0];
	    int id = ioId(TSYS::pathLev(a_path,2).substr(2));
	    if(lev == '1')	setLink(id, SET, (Block::LnkT)s2i(opt->text()));
	    else if(lev == '2')	setLink(id, SET, mLnk[id].tp, TSYS::strParse(opt->text(),0," "));
	    modif();
	}
    }
    else if(a_path == "/lnk/itp" && enable() && ctrChkNode(opt)) {
	opt->childAdd("el")->setAttr("id",i2s(Block::FREE))->setText(_("Free"));
	opt->childAdd("el")->setAttr("id",i2s(Block::I_LOC))->setText(_("Local"));
	opt->childAdd("el")->setAttr("id",i2s(Block::I_GLB))->setText(_("Global"));
	opt->childAdd("el")->setAttr("id",i2s(Block::I_PRM))->setText(_("Parameter"));
    }
    else if(a_path == "/lnk/otp" && enable() && ctrChkNode(opt)) {
	opt->childAdd("el")->setAttr("id",i2s(Block::FREE))->setText(_("Free"));
	opt->childAdd("el")->setAttr("id",i2s(Block::O_LOC))->setText(_("Local out"));
	opt->childAdd("el")->setAttr("id",i2s(Block::O_GLB))->setText(_("Global out"));
	opt->childAdd("el")->setAttr("id",i2s(Block::O_PRM))->setText(_("Parameter out"));
	opt->childAdd("el")->setAttr("id",i2s(Block::I_LOC))->setText(_("Local in"));
	opt->childAdd("el")->setAttr("id",i2s(Block::I_GLB))->setText(_("Global in"));
	opt->childAdd("el")->setAttr("id",i2s(Block::I_PRM))->setText(_("Parameter in"));
    }
    else TCntrNode::cntrCmdProc(opt);
}

TVariant Block::objFuncCall( const string &iid, vector<TVariant> &prms, const string &user_lang )
{
    //TCntrNodeObj cntr() - get the controller node
    if(iid == "cntr")	return new TCntrNodeObj(AutoHD<TCntrNode>(&owner()), user_lang);

    //Configuration functions call
    TVariant cfRez = objFunc(iid, prms, user_lang, RWRWR_, "root:" SDAQ_ID);
    if(!cfRez.isNull()) return cfRez;

    return TCntrNode::objFuncCall(iid, prms, user_lang);
}
