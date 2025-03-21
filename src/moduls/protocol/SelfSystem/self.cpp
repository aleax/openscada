
//OpenSCADA module Protocol.SelfSystem file: self.cpp
/***************************************************************************
 *   Copyright (C) 2007-2025 by Roman Savochenko, <roman@oscada.org>       *
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

#include <string.h>
#include <string>

#include <tsys.h>
#include <tmess.h>
#include <tmodule.h>
#include "self.h"

//*************************************************
//* Modul info!                                   *
#define MOD_ID		"SelfSystem"
#define MOD_NAME	trS("Own protocol of OpenSCADA")
#define MOD_TYPE	SPRT_ID
#define VER_TYPE	SPRT_VER
#define MOD_VER		"2.0.5"
#define AUTHORS		trS("Roman Savochenko")
#define DESCRIPTION	trS("Provides own OpenSCADA protocol based at XML and the control interface of OpenSCADA.")
#define LICENSE		"GPL2"
//*************************************************

SelfPr::TProt *SelfPr::mod;

extern "C"
{
#ifdef MOD_INCL
    TModule::SAt prot_SelfSystem_module( int n_mod )
#else
    TModule::SAt module( int n_mod )
#endif
    {
	if(n_mod == 0)	return TModule::SAt(MOD_ID,MOD_TYPE,VER_TYPE);
	return TModule::SAt("");
    }

#ifdef MOD_INCL
    TModule *prot_SelfSystem_attach( const TModule::SAt &AtMod, const string &source )
#else
    TModule *attach( const TModule::SAt &AtMod, const string &source )
#endif
    {
	if(AtMod == TModule::SAt(MOD_ID,MOD_TYPE,VER_TYPE)) return new SelfPr::TProt(source);
	return NULL;
    }
}

using namespace SelfPr;

//*************************************************
//* TProt                                         *
//*************************************************
TProt::TProt( string name ) : TProtocol(MOD_ID),
    mTAuth(DEF_SessTimeLife), mComprLev(DEF_ComprLev), mComprBrd(DEF_ComprBrd), mSingleUserHostLimit(DEF_SingleUserHostLimit)
{
    mod = this;

    modInfoMainSet(MOD_NAME, MOD_TYPE, MOD_VER, AUTHORS, DESCRIPTION, LICENSE, name);
}

TProt::~TProt( )
{

}

string TProt::comprLevLab( )	{ return _("Compression level and lower limit"); }

string TProt::authTimeLab( )	{ return _("Life time of the authentication session, minutes"); }

string TProt::singleUserHostLimitLab( )	{ return _("Limit of the connections for one user and host"); }

string TProt::comprLevHelp( )
{
    return _("ZLib compression level:\n"
	     "  -1  - optimal speed-size;\n"
	     "  0   - disable;\n"
	     "  1-9 - direct level.");
}

string TProt::comprBrdHelp( )	{ return _("Limit in bytes."); }

int TProt::sesOpen( const string &user, const string &pass, const string &src, int iauthTime, int isingleUserHostLimit )
{
    string pHash;
    if(!SYS->security().at().usrPresent(user) || !SYS->security().at().usrAt(user).at().auth(pass,&pHash)) return -1;

    MtxAlloc res(authRes, true);

    //Check sesions for close old and reuse more other
    unsigned i_oCnt = 0;
    map<int, SAuth>::iterator aOldI = mAuth.end();
    for(map<int, SAuth>::iterator aI = mAuth.begin(); aI != mAuth.end(); )
	if(time(NULL) > (aI->second.tAuth+authTime()*60*100) ||			//Very long unused (100x from global authTime())
		(aI->second.name == user && aI->second.src == src && time(NULL) > (aI->second.tAuth+iauthTime*60)))	//Long unused
	    mAuth.erase(aI++);
	else {
	    if(aI->second.name == user && aI->second.src == src) {		//Many opened
		if(aOldI == mAuth.end() || aI->second.tAuth < aOldI->second.tAuth) aOldI = aI;
		++i_oCnt;
	    }
	    ++aI;
	}

    if((int)i_oCnt > isingleUserHostLimit && aOldI != mAuth.end()) {
	if(mess_lev() == TMess::Debug)
	    mess_debug(nodePath().c_str(), _("Connections from the user '%s' and the source '%s' reached to the limit %d. Erasing spare!"),
		user.c_str(), TSYS::strLine(src,0).c_str(), isingleUserHostLimit);
	mAuth.erase(aOldI);
    }

    //New session ID generation
    int idSes = rand();
    while(mAuth.find(idSes) != mAuth.end()) idSes = rand();

    //Make new sesion
    mAuth[idSes] = TProt::SAuth(time(NULL), user, src);
    mAuth[idSes].pHash = pHash;

    return idSes;
}

void TProt::sesClose( int idSes )
{
    MtxAlloc res(authRes, true);
    mAuth.erase(idSes);
}

TProt::SAuth TProt::sesGet( int idSes, int iauthTime )
{
    MtxAlloc res(authRes, true);
    map<int, SAuth>::iterator aI = mAuth.find(idSes);
    if(aI != mAuth.end()) {
	time_t cur_tm = time(NULL);
	if(cur_tm > (aI->second.tAuth+iauthTime*60)) mAuth.erase(aI);
	else {
	    aI->second.tAuth = cur_tm;
	    return aI->second;
	}
    }

    return TProt::SAuth();
}

void TProt::sesSet( int idSes, const SAuth &auth )
{
    MtxAlloc res(authRes, true);
    mAuth[idSes] = auth;
}

void TProt::load_( )
{
    //Load parameters from command line

    //Load parameters from config-file
    setAuthTime(s2i(TBDS::genPrmGet(nodePath()+"SessTimeLife",i2s(DEF_SessTimeLife))));
    setComprLev(s2i(TBDS::genPrmGet(nodePath()+"ComprLev",i2s(DEF_ComprLev))));
    setComprBrd(s2i(TBDS::genPrmGet(nodePath()+"ComprBrd",i2s(DEF_ComprBrd))));
    setSingleUserHostLimit(s2i(TBDS::genPrmGet(nodePath()+"SingleUserHostLimit",i2s(DEF_SingleUserHostLimit))));
}

void TProt::save_( )
{
    TBDS::genPrmSet(nodePath()+"SessTimeLife",i2s(authTime()));
    TBDS::genPrmSet(nodePath()+"ComprLev",i2s(comprLev()));
    TBDS::genPrmSet(nodePath()+"ComprBrd",i2s(comprBrd()));
    TBDS::genPrmSet(nodePath()+"SingleUserHostLimit",i2s(singleUserHostLimit()));
}

TProtocolIn *TProt::in_open( const string &name )	{ return new TProtIn(name); }

void TProt::outMess( XMLNode &io, TTransportOut &tro )
{
    char buf[prmStrBuf_SZ];
    string req, resp, header;
    int rez, resp_len, off;
    string reqNm = io.name();

    MtxAlloc res(tro.reqRes(), true);
    if(tro.isNetwork())	tro.setTimings("20:2", true);

    bool isDir = s2i(io.attr("rqDir"));	io.attrDel("rqDir");
    bool authForce = s2i(io.attr("rqAuthForce")); io.attrDel("rqAuthForce");
    int conTm = s2i(io.attr("conTm"));	io.attrDel("conTm");
    string user = io.attr("rqUser");	io.attrDel("rqUser");
    user = TSYS::strEncode(user, TSYS::Custom, " \r\n\t%");
    string pass = io.attr("rqPass");	io.attrDel("rqPass");
    if(!pass.size())	pass = EMPTY_PASS;
    pass = TSYS::strEncode(pass, TSYS::Custom, " \r\n\t%");
    //int64_t seq = tro.conPrm("seq").getI();
    //seq = (seq != EVAL_INT && seq < 1000) ? seq+1 : 0;
    //tro.setConPrm("seq", seq); io.setAttr("rqSeq", i2s(seq));
    string data = io.save();
    io.clear();

    try {
	int reqTrs = tro.attempts(), iTr = 0;
	int comprL = vmax(-1, vmin(9,(int)tro.conPrm("ComprLev",comprLev(),(MOD_ID "\n")+TProt::comprLevLab()+"\n"+TProt::comprLevHelp())));
	int comprB = vmax(10, (int)tro.conPrm("ComprBrd",comprBrd(),(MOD_ID "\n\n")+TProt::comprBrdHelp()));

	// Compressing the data
	bool reqCompr = (comprL && (int)data.size() > comprB);
	if(reqCompr) data = TSYS::strCompr(data, comprL);

	string errTr;
	for(iTr = 0; iTr < reqTrs; ++iTr) {
	    int64_t authID = tro.conPrm("auth"+user);
	    //Opening the Session
	    if(!isDir && (authID == EVAL_INT || authForce)) {
		req = "SES_OPEN " + user + " " + pass + "\x0A";

		if(mess_lev() == TMess::Debug) mess_debug(nodePath().c_str(), _("SES_OPEN request: %d"), req.size());

		// Requesting
		resp_len = tro.messIO(req.data(), req.size(), buf, sizeof(buf), conTm);
		resp.assign(buf, resp_len);

		// Waiting the tail
		while(resp.size() && resp[resp.size()-1] != '\x0A' && resp.size() < H_LIM && (resp_len=tro.messIO(NULL,0,buf,sizeof(buf))))
		    resp.append(buf, resp_len);
		if(resp.empty()) { errTr = _("no response."); continue; }
		else if(resp[resp.size()-1] != '\x0A' || resp.size() < 5) { errTr = _("wrong or not completed response."); continue; }

		if(mess_lev() == TMess::Debug) mess_debug(nodePath().c_str(), _("SES_OPEN response: %d"), resp_len);

		off = 0;
		if(TSYS::strParse(resp,0," ",&off) == "REZ") {
		    if((rez=s2i(TSYS::strParse(resp,0," ",&off))) > 0 || off >= (int)resp.size()) {
			if(rez == atoi(ERR_AUTH)) { io.setAttr("rez", i2s(rez))->setText(resp.substr(off)); break; }
			throw TError(nodePath().c_str(), (i2s(TError::Tr_ErrResponse)+":"+_("Station %s error: %s.")).c_str(), tro.id().c_str(), resp.substr(off).c_str());
		    }
		    tro.conPrm("auth"+user, (authID=s2i(resp.substr(off))));
		} else throw TError(nodePath().c_str(), (i2s(TError::Tr_ErrResponse)+":"+_("Station %s error: %s.")).c_str(), tro.id().c_str(), _("error the header format."));
	    }

	    //The same Request
	    if(isDir)	req = "REQDIR " + user + " " + pass + " " + i2s(data.size()*(reqCompr?-1:1)) + "\x0A" + data;
	    else	req = "REQ " + i2s(authID) + " " + i2s(data.size()*(reqCompr?-1:1)) + "\x0A" + data;

	    if(mess_lev() == TMess::Debug) mess_debug(nodePath().c_str(), _("REQ send: %d"), req.size());

	    // Requesting
	    resp_len = tro.messIO(req.data(), req.size(), buf, sizeof(buf), conTm);
	    resp.assign(buf, resp_len);

	    // Waiting the header
	    while(resp.size() && ((header=TSYS::strLine(resp,0)).size() >= resp.size() || resp[header.size()] != '\x0A') &&
		    resp.size() < H_LIM && (resp_len=tro.messIO(NULL,0,buf,sizeof(buf))))
		resp.append(buf, resp_len);
	    if(resp.empty()) { errTr = _("no response."); continue; }
	    else if(resp[header.size()] != '\x0A' || header.size() < 5) { errTr = _("wrong or not completed response."); continue; }

	    if(mess_lev() == TMess::Debug) mess_debug(nodePath().c_str(), _("REQ first response %d"), resp.size());

	    // Processing the header
	    off = 0;
	    if(TSYS::strParse(header,0," ",&off) != "REZ")
		throw TError(nodePath().c_str(), (i2s(TError::Tr_ErrResponse)+":"+_("Station %s error: %s.")).c_str(), tro.id().c_str(), _("error the header format."));
	    if((rez=s2i(TSYS::strParse(header,0," ",&off))) == atoi(ERR_AUTH)) {
		tro.conPrm("auth"+user, (authID=EVAL_INT));
		if(isDir) { io.setAttr("rez", i2s(rez))->setText(header.substr(off)); break; }
		else { errTr = _("error authentication."); if(!iTr) reqTrs++; continue; }	//Reauth and one additional try for reauth
	    }
	    if(rez > 0 || off >= (int)header.size())
		throw TError(nodePath().c_str(), (i2s(TError::Tr_ErrResponse)+":"+_("Station %s error: %s.")).c_str(), tro.id().c_str(), header.substr(off).c_str());

	    int head_end = header.size() + 1,
		resp_size = s2i(header.substr(off));

	    // Waiting the whole response
	    while((int)resp.size() < (abs(resp_size)+head_end) && (resp_len=tro.messIO(NULL,0,buf,sizeof(buf))))
		resp.append(buf, resp_len);
	    if((int)resp.size() < (abs(resp_size)+head_end)) { errTr = _("not completed response."); continue; }

	    if(mess_lev() == TMess::Debug) mess_debug(nodePath().c_str(), _("REQ full response %d"), resp.size());

	    if(resp_size < 0) io.load(TSYS::strUncompr(resp.substr(head_end)), XMLNode::LD_NoTxtSpcRemEnBeg);
	    else io.load(resp.substr(head_end), XMLNode::LD_NoTxtSpcRemEnBeg);

	    if(reqNm != io.name())
	    { errTr = TSYS::strMess(_("the response '%s' is not suitable to the request '%s'."), io.name().c_str(), reqNm.c_str()); continue; }
	    //if(s2i(io.attr("rqSeq")) != seq)
	    //{ errTr = TSYS::strMess(_("The response sequence %d is not suitable to the request one %d."),s2i(io.attr("rqSeq")),(int)seq); iTr++; continue; }

	    return;
	}

	if(iTr >= reqTrs) throw TError(nodePath().c_str(), (i2s(TError::Tr_ErrResponse)+":"+_("Station %s error: %s.")).c_str(), tro.id().c_str(), errTr.c_str());

    } catch(TError &err) {
	//Appending the error code
	if(!s2i(err.mess)) err.mess = i2s(TError::Tr_ErrDevice)+":"+_("Host error: ")+err.mess;

	if(mess_lev() == TMess::Debug) mess_debug(nodePath().c_str(), _("Error the request: %s"), err.mess.c_str());

	tro.stop();

	io.setAttr("err", err.mess);
	throw TError(nodePath(), err.mess);
    }
}

void TProt::cntrCmdProc( XMLNode *opt )
{
    //Get page info
    if(opt->name() == "info") {
	TProtocol::cntrCmdProc(opt);
	if(ctrMkNode("area",opt,1,"/prm",_("Parameters"))) {
	    if(ctrMkNode("area",opt,1,"/prm/st",_("State")))
		ctrMkNode("list",opt,-1,"/prm/st/auths",_("Active authentication sessions"),R_R_R_,"root",SPRT_ID);
	    if(ctrMkNode("area",opt,1,"/prm/cfg",_("Module options"))) {
		ctrMkNode("fld",opt,-1,"/prm/cfg/lf_tm",authTimeLab(),RWRWR_,"root",SPRT_ID,2,"tp","dec","min","1");
		ctrMkNode("fld",opt,-1,"/prm/cfg/sUserHostLim",singleUserHostLimitLab(),RWRWR_,"root",SPRT_ID,1,"tp","dec");
		ctrMkNode("fld",opt,-1,"/prm/cfg/compr",comprLevLab(),RWRWR_,"root",SPRT_ID,4,"tp","dec","min","-1","max","9","help",comprLevHelp().c_str());
		ctrMkNode("fld",opt,-1,"/prm/cfg/comprBrd","",RWRWR_,"root",SPRT_ID,3,"tp","dec","min","10","help",comprBrdHelp().c_str());
	    }
	}
	return;
    }

    //Process command to page
    string a_path = opt->attr("path");
    if(a_path == "/prm/st/auths" && ctrChkNode(opt)) {
	MtxAlloc res(authRes, true);
	for(map<int,SAuth>::iterator authEl = mAuth.begin(); authEl != mAuth.end(); ++authEl)
	    opt->childAdd("el")->setText(TSYS::strMess("%s %s(%s)",
		atm2s(authEl->second.tAuth).c_str(),authEl->second.name.c_str(),authEl->second.src.c_str()));
    }
    else if(a_path == "/prm/cfg/lf_tm") {
	if(ctrChkNode(opt,"get",RWRWR_,"root",SPRT_ID,SEC_RD))	opt->setText(i2s(authTime()));
	if(ctrChkNode(opt,"set",RWRWR_,"root",SPRT_ID,SEC_WR))	setAuthTime(s2i(opt->text()));
    }
    else if(a_path == "/prm/cfg/sUserHostLim") {
	if(ctrChkNode(opt,"get",RWRWR_,"root",SPRT_ID,SEC_RD))	opt->setText(i2s(singleUserHostLimit()));
	if(ctrChkNode(opt,"set",RWRWR_,"root",SPRT_ID,SEC_WR))	setSingleUserHostLimit(s2i(opt->text()));
    }
    else if(a_path == "/prm/cfg/compr") {
	if(ctrChkNode(opt,"get",RWRWR_,"root",SPRT_ID,SEC_RD))	opt->setText(i2s(comprLev()));
	if(ctrChkNode(opt,"set",RWRWR_,"root",SPRT_ID,SEC_WR))	setComprLev(s2i(opt->text()));
    }
    else if(a_path == "/prm/cfg/comprBrd") {
	if(ctrChkNode(opt,"get",RWRWR_,"root",SPRT_ID,SEC_RD))	opt->setText(i2s(comprBrd()));
	if(ctrChkNode(opt,"set",RWRWR_,"root",SPRT_ID,SEC_WR))	setComprBrd(s2i(opt->text()));
    }
    else TProtocol::cntrCmdProc(opt);
}

//*************************************************
//* TProtIn                                       *
//*************************************************
TProtIn::TProtIn( string name ) : TProtocolIn(name)
{

}

TProtIn::~TProtIn( )
{

}

bool TProtIn::mess( const string &request, string &answer )
{
    int64_t d_tm = 0;
    int ses_id = -1;
    int req_sz = 0;
    char user[256] = "", pass[256] = "";
    string user_, pass_;

    reqBuf += request;

    if(mess_lev() == TMess::Debug) d_tm = TSYS::curTime();

    //Checking for the header completeness
    string header = TSYS::strLine(reqBuf, 0);
    if(header.size() >= reqBuf.size() || reqBuf[header.size()] != '\x0A') {
	if(header.size() > H_LIM) reqBuf.clear();	//Similar to flooding - clearing the buffer
	return true;
    }

    int authTime = vmax(1, (int)srcTr().at().conPrm("SessTimeLife",((TProt&)owner()).authTime(),
							(MOD_ID "\n")+TProt::authTimeLab()));

    //Processing the request
    if(header.find("SES_OPEN") == 0) {
	int singleUserHostLimit = srcTr().at().conPrm("SingleUserHostLimit",((TProt&)owner()).singleUserHostLimit(),
							(MOD_ID "\n")+TProt::singleUserHostLimitLab());

	sscanf(header.c_str(), "SES_OPEN %255s %255s", user, pass);
	user_ = TSYS::strDecode(user, TSYS::Custom);
	pass_ = TSYS::strDecode(pass, TSYS::Custom);
	if(pass_ == EMPTY_PASS) pass_ = "";
	if((ses_id=mod->sesOpen(user_,pass_,TSYS::strLine(srcAddr(),0),authTime,singleUserHostLimit)) < 0)
	    answer = "REZ " ERR_AUTH " Error authentication: wrong user or password.\x0A";
	else answer = "REZ " ERR_NO " " + i2s(ses_id) + "\x0A";
    }
    else if(header.find("SES_CLOSE") == 0) {
	sscanf(header.c_str(), "SES_CLOSE %d", &ses_id);
	mod->sesClose(ses_id);
	answer = "REZ 0\x0A";
    }
    else if(header.find("REQ") == 0) {
	if(mess_lev() == TMess::Debug) {
	    mess_debug(nodePath().c_str(), _("Request received: '%s': %d, time: %g ms."),
		header.c_str(), reqBuf.size(), 1e-3*(TSYS::curTime()-d_tm));
	    d_tm = TSYS::curTime();
	}

	int rez = 0;
	if((rez=sscanf(header.c_str(),"REQ %d %d",&ses_id,&req_sz)) != 2 && (rez=sscanf(header.c_str(),"REQDIR %255s %255s %d",user,pass,&req_sz)) != 3)
	{ answer = "REZ " ERR_CMD " Command format error.\x0A"; reqBuf.clear(); return false; }

	// Waiting the tail before any other checkings
	if(reqBuf.size() < (header.size()+1+abs(req_sz))) return true;

	// Checking the authentication
	TProt::SAuth auth;
	if(rez == 2) auth = mod->sesGet(ses_id, authTime);
	else if(rez == 3) {
	    user_ = TSYS::strDecode(user, TSYS::Custom);
	    pass_ = TSYS::strDecode(pass, TSYS::Custom);
	    if(pass_ == EMPTY_PASS) pass_ = "";
	    if(SYS->security().at().usrPresent(user_) && SYS->security().at().usrAt(user_).at().auth(pass_,&auth.pHash))
	    { auth.tAuth = 1; auth.name = user_; }
	}
	if(!auth.tAuth) { answer = "REZ " ERR_AUTH " Error authentication: session is not valid.\x0A"; reqBuf.clear(); return false; }

	try {
	    int comprL = vmax(-1, vmin(9,(int)srcTr().at().conPrm("ComprLev",((TProt&)owner()).comprLev(),
					    (MOD_ID "\n")+TProt::comprLevLab()+"\n"+TProt::comprLevHelp())));
	    int comprB = vmax(10, (int)srcTr().at().conPrm("ComprBrd",((TProt&)owner()).comprBrd(),
					    (MOD_ID "\n\n")+TProt::comprBrdHelp()));

	    //Decompressing the request
	    if(req_sz < 0) reqBuf.replace(header.size()+1, abs(req_sz), TSYS::strUncompr(reqBuf.substr(header.size()+1)));

	    //Processing the request
	    XMLNode req_node;
	    req_node.load(reqBuf.substr(header.size()+1), XMLNode::LD_NoTxtSpcRemEnBeg);

	    // Check for reforward requests
	    string host = req_node.attr("reforwardHost");
	    if(host.size()) {
		if(mess_lev() == TMess::Debug) {
		    mess_debug(nodePath().c_str(), _("Request checked and redirected: '%s': %d, time: %g ms."),
			header.c_str(), reqBuf.size(), 1e-3*(TSYS::curTime()-d_tm));
		    d_tm = TSYS::curTime();
		}
		req_node.setAttr("path", "/"+host+req_node.attr("path"))->setAttr("reforwardHost", "");
		try { SYS->transport().at().cntrIfCmd(req_node, "Reforward", auth.name); }
		catch(TError &err) {
		    req_node.childClear();
		    req_node.setAttr("mcat",err.cat)->setAttr("rez",i2s(TError::Tr_Connect))->setText(err.mess);
		}
	    }
	    else {
		req_node.setAttr("user", auth.name);

		if(mess_lev() == TMess::Debug) {
		    mess_debug(nodePath().c_str(), _("Request unpacked and loaded: '%s': %d, time: %g ms."),
			header.c_str(), reqBuf.size(), 1e-3*(TSYS::curTime()-d_tm));
		    d_tm = TSYS::curTime();
		}

		req_node.setAttr("remoteSrcAddr", TSYS::strLine(srcAddr(),0));
		SYS->cntrCmd(&req_node);
		req_node.attrDel("remoteSrcAddr");
		if(auth.pHash.size()) {
		    req_node.setAttr("pHash", auth.pHash);
		    if(ses_id >= 0) { auth.pHash = ""; mod->sesSet(ses_id, auth); }
		}

		if(mess_lev() == TMess::Debug) {
		    mess_debug(nodePath().c_str(), _("Request processed: '%s', time: %g ms."), header.c_str(), 1e-3*(TSYS::curTime()-d_tm));
		    d_tm = TSYS::curTime();
		}
	    }

	    string resp = req_node.save(XMLNode::MissTagEnc|XMLNode::MissAttrEnc)+"\n";

	    //Compress respond
	    bool respCompr = ((req_sz < 0 || comprL) && (int)resp.size() > comprB);
	    if(respCompr) resp = TSYS::strCompr(resp, comprL?comprL:-1);

	    if(mess_lev() == TMess::Debug)
		mess_debug(nodePath().c_str(), _("Response saved to a stream and packed: '%s': %d, time: %g ms."),
		    header.c_str(), resp.size(), 1e-3*(TSYS::curTime()-d_tm));

	    answer = "REZ " ERR_NO " " + i2s(resp.size()*(respCompr?-1:1)) + "\x0A" + resp;
	} catch(TError &err) { answer = "REZ " ERR_PRC " " + err.cat + ":" + err.mess + "\x0A"; }
    } else answer = "REZ " ERR_CMD " Error the command format.\x0A";

    reqBuf.clear();

    return false;
}
