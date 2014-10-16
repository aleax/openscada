
//OpenSCADA system module DAQ.BlockCalc file: block.cpp
/***************************************************************************
 *   Copyright (C) 2005-2014 by Roman Savochenko, <rom_as@oscada.org>      *
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
    TCntrNode(iown), TValFunc(iid+"_block",NULL), TConfig(&((TipContr &)iown->owner()).blockE()), m_enable(false), m_process(false),
    m_id(cfg("ID")), m_to_en(cfg("EN").getBd()), m_to_prc(cfg("PROC").getBd()), mOutLnkWrChs(cfg("LNK_OUT_WR_CH").getBd()),
    id_freq(-1), id_start(-1), id_stop(-1)
{
    m_id = iid;
}

Block::~Block( )
{
    if(enable()) setEnable(false);
}

TCntrNode &Block::operator=( TCntrNode &node )
{
    Block *src_n = dynamic_cast<Block*>(&node);
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
    if(flag) {
	//Delete block from BD
	string tbl = owner().DB()+"."+owner().cfg("BLOCK_SH").getS();
	SYS->db().at().dataDel(tbl,mod->nodePath()+owner().cfg("BLOCK_SH").getS(),*this,true);

	//Delete block's IO from BD
	TConfig cfg(&owner().owner().blockIOE());
	tbl = tbl+"_io";
	cfg.cfg("BLK_ID").setS(id(), true);		//Delete all block id records
	SYS->db().at().dataDel(tbl,mod->nodePath()+owner().cfg("BLOCK_SH").getS()+"_io",cfg);
    }
}

Contr &Block::owner( )	{ return *(Contr*)nodePrev(); }

string Block::name( )
{
    string rez = cfg("NAME").getS();
    return rez.size() ? rez : id();
}

void Block::load_( )
{
    if(!SYS->chkSelDB(owner().DB())) throw TError();

    string bd = owner().DB()+"."+owner().cfg("BLOCK_SH").getS();
    SYS->db().at().dataGet(bd,mod->nodePath()+owner().cfg("BLOCK_SH").getS(),*this);

    //Load IO config
    loadIO();
}

void Block::save_( )
{
    string bd = owner().DB()+"."+owner().cfg("BLOCK_SH").getS();
    SYS->db().at().dataSet(bd,mod->nodePath()+owner().cfg("BLOCK_SH").getS(),*this);

    //Save IO config
    saveIO();
}

void Block::loadIO( const string &blk_db, const string &blk_id, bool force )
{
    string bd_tbl, bd;
    if(!func()) return;
    if(owner().startStat() && !force) { modif(true); return; }	//Load/reload IO context only allow for stoped controlers for prevent throws

    TConfig cfg(&mod->blockIOE());
    cfg.cfg("BLK_ID").setS((blk_id.size())?blk_id:id());
    if(blk_db.empty()) {
	bd_tbl	= owner().cfg("BLOCK_SH").getS()+"_io";
	bd	= owner().DB()+"."+bd_tbl;
    }
    else {
	bd	= blk_db+"_io";
	bd_tbl	= TSYS::strSepParse(bd,2,'.');
    }

    for(int i_ln = 0; i_ln < ioSize(); i_ln++) {
	if(i_ln >= (int)m_lnk.size()) {
	    m_lnk.push_back(SLnk());
	    m_lnk[i_ln].tp = FREE;
	}

	cfg.cfg("ID").setS(func()->io(i_ln)->id());
	if(!SYS->db().at().dataGet(bd,mod->nodePath()+bd_tbl,cfg,false,true)) continue;
	//Value
	setS(i_ln,cfg.cfg("VAL").getS());
	//Configuration of link
	setLink(i_ln, SET, (LnkT)cfg.cfg("TLNK").getI(), cfg.cfg("LNK").getS());
    }
}

void Block::saveIO( )
{
    if(!enable()) return;

    TConfig cfg(&mod->blockIOE());
    cfg.cfg("BLK_ID").setS(id());
    string bd_tbl = owner().cfg("BLOCK_SH").getS()+"_io";
    string bd = owner().DB()+"."+bd_tbl;

    for(unsigned i_ln = 0; i_ln < m_lnk.size(); i_ln++)
	try {
	    cfg.cfg("ID").setS(func()->io(i_ln)->id());
	    cfg.cfg("TLNK").setI(m_lnk[i_ln].tp);				//Type link
	    cfg.cfg("LNK").setS((m_lnk[i_ln].tp == FREE)?"":m_lnk[i_ln].lnk);	//Link
	    cfg.cfg("VAL").setS(getS(i_ln));					//Value

	    SYS->db().at().dataSet(bd,mod->nodePath()+bd_tbl,cfg);
	}
	catch(TError err) {
	    mess_err(err.cat.c_str(),"%s",err.mess.c_str());
	    mess_err(nodePath().c_str(),_("Block link '%s' save error."),func()->io(i_ln)->id().c_str());
	}
}

void Block::setEnable( bool val )
{
    //Enable
    if(val && !m_enable) {
	if(!func()) {
	    if(!dynamic_cast<TFunction *>(&SYS->nodeAt(wFunc(),0,'.').at()))
		throw TError(nodePath().c_str(),_("Node '%s' is not function."),wFunc().c_str());
	    setFunc((TFunction *)&SYS->nodeAt(wFunc(),0,'.').at());
	    // Init system attributes identifiers
	    id_freq  = func()->ioId("f_frq");
	    id_start = func()->ioId("f_start");
	    id_stop  = func()->ioId("f_stop");
	}
	// Init links
	loadIO("", "", true);
    }
    //Disable
    else if(!val && m_enable) {
	if(process()) setProcess(false);
	// Save IO config
	//saveIO();

	// Clean IO
	for(unsigned i_ln = 0; i_ln < m_lnk.size(); i_ln++) setLink(i_ln, SET, FREE);
	m_lnk.clear();

	// Free func
	setFunc(NULL);
	id_freq = id_start = id_stop = -1;
    }
    m_enable = val;
}

void Block::setProcess( bool val )
{
    if(val && !enable()) setEnable(true);

    //Connect links
    if(val && !process()) {
	for(unsigned i_ln = 0; i_ln < m_lnk.size(); i_ln++) setLink(i_ln, INIT);
	if(owner().startStat()) calc(true, false, 0);
	owner().blkProc(id(), val);
    }
    //Disconnect links
    if(!val && process()) {
	owner().blkProc(id(), val);
	if(owner().startStat()) calc(false, true, 0);
	for(unsigned i_ln = 0; i_ln < m_lnk.size(); i_ln++) setLink(i_ln, DEINIT);
    }
    m_process = val;
}

Block::LnkT Block::link( unsigned iid )
{
    if(iid >= m_lnk.size()) throw TError(nodePath().c_str(),_("Link %d error!"),iid);
    return m_lnk[iid].tp;
}

bool Block::linkActive( unsigned iid )
{
    ResAlloc res(lnk_res,false);
    if(iid >= m_lnk.size()) throw TError(nodePath().c_str(),_("Link %d error!"),iid);

    switch(m_lnk[iid].tp) {
	case I_LOC: case I_GLB:	return !m_lnk[iid].iblk->w_bl.freeStat();
	case I_PRM:		return !m_lnk[iid].aprm->freeStat();
	default: return false;
    }
    return false;
}

void Block::setLink( unsigned iid, LnkCmd cmd, LnkT lnk, const string &vlnk )
{
    ResAlloc res(lnk_res, true);
    if(iid >= m_lnk.size()) throw TError(nodePath().c_str(),_("Link %d error!"),iid);

    //Change type link
    if(cmd == SET) {
	if(lnk != m_lnk[iid].tp) {
	    // Free old structures
	    switch(m_lnk[iid].tp) {
		case I_LOC: case I_GLB:	case O_LOC: case O_GLB: delete m_lnk[iid].iblk;	break;
		case I_PRM: case O_PRM: delete m_lnk[iid].aprm;				break;
		default: break;
	    }

	    // Make new structures
	    switch(lnk) {
		case I_LOC: case I_GLB:	case O_LOC: case O_GLB: m_lnk[iid].iblk = new SLIBlk();	break;
		case I_PRM: case O_PRM: m_lnk[iid].aprm = new AutoHD<TVal>;			break;
		default: break;
	    }
	    m_lnk[iid].tp = lnk;
	}
	m_lnk[iid].lnk = vlnk;
    }
    //Connect new link and init
    if(cmd == INIT || (cmd == SET && process())) {
	string lo1 = TSYS::strSepParse(m_lnk[iid].lnk,0,'.');
	string lo2 = TSYS::strSepParse(m_lnk[iid].lnk,1,'.');
	string lo3 = TSYS::strSepParse(m_lnk[iid].lnk,2,'.');
	string lo4 = TSYS::strSepParse(m_lnk[iid].lnk,3,'.');

	switch(m_lnk[iid].tp) {
	    case I_LOC: case O_LOC:
		m_lnk[iid].iblk->w_bl.free();
		if(owner().blkPresent(lo1) && owner().blkAt(lo1).at().ioId(lo2) >= 0) {
		    m_lnk[iid].iblk->w_bl = owner().blkAt(lo1);
		    m_lnk[iid].iblk->w_id = m_lnk[iid].iblk->w_bl.at().ioId(lo2);
		}
		break;
	    case I_GLB: case O_GLB:
		m_lnk[iid].iblk->w_bl.free();
		if(owner().owner().present(lo1) &&
		    ((Contr &)owner().owner().at(lo1).at()).blkPresent(lo2) &&
		    ((Contr &)owner().owner().at(lo1).at()).blkAt(lo2).at().ioId(lo3) >= 0)
		{
		    m_lnk[iid].iblk->w_bl = ((Contr &)owner().owner().at(lo1).at()).blkAt(lo2);
		    m_lnk[iid].iblk->w_id = m_lnk[iid].iblk->w_bl.at().ioId(lo3);
		}
		break;
	    case I_PRM: case O_PRM:
		*m_lnk[iid].aprm = SYS->daq().at().attrAt(m_lnk[iid].lnk, '.', true);
		if(m_lnk[iid].aprm->freeStat()) *m_lnk[iid].aprm = SYS->nodeAt(m_lnk[iid].lnk, 0, '.', 0, true);
		break;
	    default: break;
	}
    }
    //Disconnect
    if(cmd == DEINIT)
	switch(m_lnk[iid].tp) {
	    case I_LOC: case I_GLB: case O_LOC: case O_GLB: m_lnk[iid].iblk->w_bl.free();	break;
	    case I_PRM: case O_PRM: m_lnk[iid].aprm->free();					break;
	    default: break;
	}
}

void Block::calc( bool first, bool last, double frq )
{
    setMdfChk(outLnkWrChs());

    //Set fixed system attributes
    if(id_freq >= 0)	setR(id_freq, frq);
    if(id_start >= 0)	setB(id_start, first);
    if(id_stop >= 0)	setB(id_stop, last);

    //Get values from input links
    lnk_res.resRequestR( );
    try {
	// Get input links
	for(unsigned i_ln = 0; i_ln < m_lnk.size(); i_ln++)
	    switch(m_lnk[i_ln].tp) {
		case I_LOC: case I_GLB:
		    if(m_lnk[i_ln].iblk->w_bl.freeStat()) break;
		    if(m_lnk[i_ln].iblk->w_bl.at().enable()) {
			//  Early disconnected link init try
			if(m_lnk[i_ln].iblk->w_id == -100) {
			    lnk_res.resRelease( );
			    try{ setLink(i_ln, INIT); } catch(...) { setLink(i_ln, DEINIT); }
			    lnk_res.resRequestR( );
			    if( m_lnk[i_ln].iblk->w_bl.freeStat() ) break;
			}
			//  Use link
			set(i_ln,m_lnk[i_ln].iblk->w_bl.at().get(m_lnk[i_ln].iblk->w_id));
		    }
		    //  Check for link disable need
		    else m_lnk[i_ln].iblk->w_id = -100;
		    break;
		case I_PRM:
		    if(m_lnk[i_ln].aprm->freeStat()) break;
		    set(i_ln, m_lnk[i_ln].aprm->at().get());
		    break;
		default: break;
	    }
    }
    catch(TError err) {
	err_cnt++;
	lnk_res.resRelease();
	mess_err(err.cat.c_str(),"%s",err.mess.c_str());
	throw TError(nodePath().c_str(),_("Error reading block's '%s' links."),id().c_str());
    }
    lnk_res.resRelease( );

    //Calc function
    try {
	TValFunc::calc( );
	modif();
    }
    catch(TError err) { err_cnt++; throw; }

    //Put values to output links
    lnk_res.resRequestR();
    try {
	for(unsigned i_ln = 0; i_ln < m_lnk.size(); i_ln++)
	    switch(m_lnk[i_ln].tp) {
		case O_LOC: case O_GLB:
		    if(m_lnk[i_ln].iblk->w_bl.freeStat() || (outLnkWrChs() && !ioMdf(i_ln))) break;
		    if(m_lnk[i_ln].iblk->w_bl.at().enable()) {
			//Early disconnected link init try
			if(m_lnk[i_ln].iblk->w_id == -100) {
			    lnk_res.resRelease();
			    try{ setLink(i_ln, INIT); } catch(...) { setLink(i_ln, DEINIT); }
			    lnk_res.resRequestR();
			    if(m_lnk[i_ln].iblk->w_bl.freeStat()) break;
			}
			//Use link
			m_lnk[i_ln].iblk->w_bl.at().set(m_lnk[i_ln].iblk->w_id,get(i_ln));
		    }
		    //Check for link disable need
		    else m_lnk[i_ln].iblk->w_id = -100;
		    break;
		case O_PRM:
		    if(m_lnk[i_ln].aprm->freeStat() || (outLnkWrChs() && !ioMdf(i_ln))) break;
		    m_lnk[i_ln].aprm->at().set(get(i_ln));
		    break;
		default: break;
	    }
    }
    catch(TError err) {
	err_cnt++;
	lnk_res.resRelease();
	throw TError(nodePath().c_str(),_("Error writing block's '%s' links."),id().c_str());
    }
    lnk_res.resRelease( );
    err_cnt = 0;
}

void Block::cntrCmdProc( XMLNode *opt )
{
    //Service commands process
    string a_path = opt->attr("path");
    if(a_path.substr(0,6) == "/serv/") {
	if(a_path == "/serv/attr") {
	    if(!enable() || !func()) throw TError(nodePath().c_str(),_("Block disabled or error."));
	    if(ctrChkNode(opt,"get",RWRWR_,"root",SDAQ_ID,SEC_RD) )
		for(int i_a = 0; i_a < ioSize(); i_a++)
		    opt->childAdd("a")->setAttr("id",func()->io(i_a)->id())->setText(getS(i_a));
	    if(ctrChkNode(opt,"set",RWRWR_,"root",SDAQ_ID,SEC_WR))
		for(unsigned i_a = 0; i_a < opt->childSize(); i_a++) {
		    int io_id = -1;
		    if(opt->childGet(i_a)->name() != "a" || (io_id=ioId(opt->childGet(i_a)->attr("id"))) < 0) continue;
		    setS(io_id,opt->childGet(i_a)->text());
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
		ctrMkNode("fld",opt,-1,"/blck/st/en",_("Enable"),RWRWR_,"root",SDAQ_ID,1,"tp","bool");
		if(owner().startStat())
		    ctrMkNode("fld",opt,-1,"/blck/st/prc",_("Process"),RWRWR_,"root",SDAQ_ID,1,"tp","bool");
	    }
	    if(ctrMkNode("area",opt,-1,"/blck/cfg",_("Configuration"))) {
		ctrMkNode("fld",opt,-1,"/blck/cfg/id",_("Id"),R_R_R_,"root",SDAQ_ID,1,"tp","str");
		ctrMkNode("fld",opt,-1,"/blck/cfg/name",_("Name"),RWRWR_,"root",SDAQ_ID,2,"tp","str","len",OBJ_NM_SZ);
		ctrMkNode("fld",opt,-1,"/blck/cfg/descr",_("Description"),RWRWR_,"root",SDAQ_ID,3,"tp","str","cols","90","rows","4");
		ctrMkNode("fld",opt,-1,"/blck/cfg/toen",_("To enable"),RWRWR_,"root",SDAQ_ID,1,"tp","bool");
		ctrMkNode("fld",opt,-1,"/blck/cfg/toprc",_("To process"),RWRWR_,"root",SDAQ_ID,1,"tp","bool");
		ctrMkNode("fld",opt,-1,"/blck/cfg/prior",_("Prior block"),RWRWR_,"root",SDAQ_ID,4,"tp","str","dest","sel_ed","select","/blck/cfg/blks",
		    "help",_("Priority block(s) calculate before this block. Blocks list is separated by symbol ';'."));
		ctrMkNode("fld",opt,-1,"/blck/cfg/outLnkWrChs",cfg("LNK_OUT_WR_CH").fld().descr().c_str(),RWRWR_,"root",SDAQ_ID,1,"tp","bool");
		ctrMkNode("fld",opt,-1,"/blck/cfg/func",_("Function"),(!func())?RWRWR_:R_R_R_,"root",SDAQ_ID,3,"tp","str","dest","sel_ed","select","/blck/cfg/fncs");
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
			if(ioHide(id) && !s2i(TBDS::genDBGet(owner().nodePath()+"showHide","0",opt->attr("user")))) continue;
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

			if(ioHide(id) && !s2i(TBDS::genDBGet(owner().nodePath()+"showHide","0",opt->attr("user")))) continue;

			// Add link's type
			ctrMkNode("fld",opt,-1,(string("/lnk/io/1|")+list[i_io]).c_str(),
			    func()->io(id)->name().c_str(),RWRWR_,"root",SDAQ_ID,3,"tp","dec","dest","select","select",(ioFlg(id)&(IO::Output|IO::Return))?"/lnk/otp":"/lnk/itp");
			if(m_lnk[id].tp != FREE)
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
    else if(a_path == "/blck/cfg/id" && ctrChkNode(opt))	opt->setText(id());
    else if(a_path == "/blck/cfg/name") {
	if(ctrChkNode(opt,"get",RWRWR_,"root",SDAQ_ID,SEC_RD))	opt->setText(name());
	if(ctrChkNode(opt,"set",RWRWR_,"root",SDAQ_ID,SEC_WR))	setName(opt->text());
    }
    else if(a_path == "/blck/cfg/descr") {
	if(ctrChkNode(opt,"get",RWRWR_,"root",SDAQ_ID,SEC_RD))	opt->setText(descr());
	if(ctrChkNode(opt,"set",RWRWR_,"root",SDAQ_ID,SEC_WR))	setDescr(opt->text());
    }
    else if(a_path == "/blck/cfg/toen") {
	if(ctrChkNode(opt,"get",RWRWR_,"root",SDAQ_ID,SEC_RD))	opt->setText(toEnable() ? "1" : "0");
	if(ctrChkNode(opt,"set",RWRWR_,"root",SDAQ_ID,SEC_WR))	setToEnable(s2i(opt->text()));
    }
    else if(a_path == "/blck/cfg/toprc") {
	if(ctrChkNode(opt,"get",RWRWR_,"root",SDAQ_ID,SEC_RD))	opt->setText(toProcess() ? "1" : "0");
	if(ctrChkNode(opt,"set",RWRWR_,"root",SDAQ_ID,SEC_WR))	setToProcess(s2i(opt->text()));
    }
    else if(a_path == "/blck/cfg/prior") {
	if(ctrChkNode(opt,"get",RWRWR_,"root",SDAQ_ID,SEC_RD))	opt->setText(prior());
	if(ctrChkNode(opt,"set",RWRWR_,"root",SDAQ_ID,SEC_WR))	setPrior(opt->text());
    }
    else if(a_path == "/blck/cfg/outLnkWrChs") {
	if(ctrChkNode(opt,"get",RWRWR_,"root",SDAQ_ID,SEC_RD))	opt->setText(i2s(outLnkWrChs()));
	if(ctrChkNode(opt,"set",RWRWR_,"root",SDAQ_ID,SEC_WR))	setOutLnkWrChs(s2i(opt->text()));
    }
    else if(a_path == "/blck/cfg/blks" && ctrChkNode(opt)) {
	vector<string> list;
	owner().blkList(list);
	opt->childAdd("el")->setText("");
	for(unsigned i_b = 0; i_b < list.size(); i_b++)
	    if(list[i_b] != id()) opt->childAdd("el")->setText(list[i_b]);
    }
    else if(a_path == "/blck/cfg/func") {
	if(ctrChkNode(opt,"get",RWRWR_,"root",SDAQ_ID,SEC_RD)) {
	    opt->setText(wFunc());
	    try{ if(dynamic_cast<TFunction*>(&SYS->nodeAt(wFunc(),0,'.').at())) opt->setText(opt->text()+" (+)"); }
	    catch(TError) { }
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
	for(unsigned i_a=0; i_a < list.size(); i_a++)
	    opt->childAdd("el")->setText(c_path+list[i_a]);
    }
    else if((a_path == "/lio/show/hide" || a_path == "/lnk/show/hide") && enable()) {
	if(ctrChkNode(opt,"get",RWRWR_,"root",SDAQ_ID,SEC_RD))	opt->setText(TBDS::genDBGet(owner().nodePath()+"showHide","0",opt->attr("user")));
	if(ctrChkNode(opt,"set",RWRWR_,"root",SDAQ_ID,SEC_WR))	TBDS::genDBSet(owner().nodePath()+"showHide",opt->text(),opt->attr("user"));
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
	    string lnk = m_lnk[id].lnk;

	    switch(lev) {
		case '1':	opt->setText(i2s(m_lnk[id].tp));	break;
		case '2':
		    opt->setText(lnk);
		    try {
			switch(m_lnk[id].tp) {
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
		    }
		    catch(TError) { }
		    break;
		case '3':
		    if(m_lnk[id].tp == I_PRM || m_lnk[id].tp == O_PRM) SYS->daq().at().ctrListPrmAttr(opt, lnk, false, '.');
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

			switch(m_lnk[id].tp) {
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
			for(unsigned i_a=0; i_a < list.size(); i_a++)
			    opt->childAdd("el")->setText(c_path+list[i_a]);
		    }
	    }
	}
	if(ctrChkNode(opt,"set",RWRWR_,"root",SDAQ_ID,SEC_WR)) {
	    char lev = TSYS::pathLev(a_path,2)[0];
	    int id = ioId(TSYS::pathLev(a_path,2).substr(2));
	    if(lev == '1')	setLink(id, SET, (Block::LnkT)s2i(opt->text()));
	    else if(lev == '2')	setLink(id, SET, m_lnk[id].tp, TSYS::strParse(opt->text(),0," "));
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
