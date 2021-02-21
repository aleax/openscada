
//OpenSCADA module Archive.FSArch file: val.cpp
/***************************************************************************
 *   Copyright (C) 2003-2021 by Roman Savochenko, <roman@oscada.org>       *
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

#include <sys/types.h>
#include <sys/stat.h>
#include <stddef.h>
#include <fcntl.h>
#include <dirent.h>
#include <errno.h>
#include <math.h>
#include <string.h>

#include <algorithm>

#include <tsys.h>
#include <tmess.h>
#include "base.h"
#include "val.h"

using namespace FSArch;

//*************************************************
//* FSArch::ModVArch - Value archiver             *
//*************************************************
ModVArch::ModVArch( const string &iid, const string &idb, TElem *cf_el ) :
    TVArchivator(iid,idb,cf_el), chkANow(false), infoTbl(dataRes()),
    time_size(800), mNumbFiles(100), mMaxCapacity(0), round_proc(0.01), mChkTm(60), mPackTm(10), mPackInfoFiles(false), mLstCheck(0)
{
    setSelPrior(1000);
    if(SYS->prjNm().size()) setAddr("ARCHIVES/VAL/"+iid);
}

ModVArch::~ModVArch( )
{
    try { stop(); } catch(...) { }
}

TCntrNode &ModVArch::operator=( const TCntrNode &node )
{
    TVArchivator::operator=(node);
    load_();

    return *this;
}

double ModVArch::curCapacity( )
{
    double fsz = 0;
    ResAlloc res(archRes, false);
    for(map<string,TVArchEl*>::iterator iel = archEl.begin(); iel != archEl.end(); ++iel)
	fsz += ((ModVArchEl *)iel->second)->size();
    return fsz;
}

void ModVArch::load_( )
{
    //TVArchivator::load_();

    try {
	XMLNode prmNd;
	string  vl;
	prmNd.load(cfg("A_PRMS").getS());
	vl = prmNd.attr("TmSize");	if(!vl.empty()) setFileTimeSize(s2r(vl));
	vl = prmNd.attr("NFiles");	if(!vl.empty()) setNumbFiles(s2i(vl));
	vl = prmNd.attr("MaxCapacity"); if(!vl.empty()) setMaxCapacity(s2r(vl));
	vl = prmNd.attr("Round"); 	if(!vl.empty()) setRoundProc(s2r(vl));
	vl = prmNd.attr("PackTm");	if(!vl.empty()) setPackTm(s2i(vl));
	vl = prmNd.attr("CheckTm");	if(!vl.empty()) setCheckTm(s2i(vl));
	vl = prmNd.attr("PackInfoFiles"); if(!vl.empty()) setPackInfoFiles(s2i(vl));
    } catch(...) { }
}

void ModVArch::save_( )
{
    XMLNode prmNd("prms");
    prmNd.setAttr("TmSize", r2s(fileTimeSize()));
    prmNd.setAttr("NFiles", i2s(numbFiles()));
    prmNd.setAttr("MaxCapacity", r2s(maxCapacity()));
    prmNd.setAttr("Round", r2s(roundProc()));
    prmNd.setAttr("PackTm", i2s(packTm()));
    prmNd.setAttr("CheckTm", i2s(checkTm()));
    prmNd.setAttr("PackInfoFiles", i2s(packInfoFiles()));
    cfg("A_PRMS").setS(prmNd.save(XMLNode::BrAllPast));

    TVArchivator::save_();
}

bool ModVArch::cfgChange( TCfg &co, const TVariant &pc )
{
    if(co.name() == "V_PER") setFileTimeSize(1e3*valPeriod());
    else if(co.name() == "ADDR" && startStat())	return false;

    return TVArchivator::cfgChange(co, pc);
}

void ModVArch::start( )
{
    if(!startStat()) {
	//Open/create the archive directory
	DIR *IdDir = opendir(addr().c_str());
	if(IdDir == NULL && mkdir(addr().c_str(),SYS->permCrtFiles(true)))
	    throw err_sys(_("Cannot create the archive directory '%s'."), addr().c_str());
	closedir(IdDir);

	//Checking the archiver folders for duplicates
	string dbl = "";
	MtxAlloc res(mod->enRes(), true);
	const char *fLock = "fsArchLock";
	int hd = open((addr()+"/"+fLock).c_str(), O_CREAT|O_TRUNC|O_WRONLY, SYS->permCrtFiles());
	if(hd >= 0) {
	    write(hd, "1", 1);
	    vector<string> ls;
	    mod->valList(ls);
	    for(unsigned iL = 0; iL < ls.size() && dbl.empty(); iL++) {
		AutoHD<TVArchivator> vAt = mod->valAt(ls[iL]);
		if(vAt.at().id() == id() || !vAt.at().startStat())	continue;
		int hd1 = open((vAt.at().addr()+"/"+fLock).c_str(), O_RDONLY);
		if(hd1 >= 0 || vAt.at().addr() == addr()) {
		    dbl = vAt.at().addr();
		    if(hd1 >= 0) close(hd1);
		}
	    }
	    close(hd);
	    remove((addr()+"/"+fLock).c_str());
	}
	res.unlock();
	if(dbl.size()) { stop(); throw err_sys(_("The value archiver '%s' uses the same folder '%s' as an other archiver."), id().c_str(), addr().c_str()); }
    }

    //Create and/or update the SQLite info file, special for the archiver and placed with main files of the archiver
    if(!startStat() && packInfoFiles()) {
	infoTbl = "";
	try {
	    AutoHD<TTypeBD> SQLite = SYS->db().at().at("SQLite");
	    // Search for the info table
	    vector<string> ls;
	    SQLite.at().list(ls);
	    for(unsigned iL = 0; iL < ls.size() && infoTbl.empty(); iL++)
		if(SQLite.at().at(ls[iL]).at().addr() == addr()+"/info.db")
		    infoTbl = "SQLite."+ls[iL]+"."+TSYS::strParse(mod->filesDB(),2,".");
	    if(infoTbl.empty()) {
		string iDBnm = MOD_ID "_Val_"+id()+"_info";
		while(true) {
		    AutoHD<TBD> infoDB = SQLite.at().at((iDBnm=SQLite.at().open(iDBnm)));
		    if(infoDB.at().addr().size()) { iDBnm = TSYS::strLabEnum(iDBnm); continue; }
		    infoDB.at().setName(TSYS::strMess(_("%s: Val: %s: information"),MOD_ID,id().c_str()));
		    infoDB.at().setDscr(TSYS::strMess(_("Local information DB for the value archiver '%s'. "
			"Created automatically then don't modify, save and remove it!"),id().c_str()));
		    infoDB.at().setAddr(addr()+"/info.db");
		    infoDB.at().enable();
		    infoDB.at().modifClr();
		    infoTbl = "SQLite."+iDBnm+"."+TSYS::strParse(mod->filesDB(),2,".");
		    break;
		}
	    }
	} catch(TError&) { }
    }

    //Start getting data cycle
    TVArchivator::start();

    //First scan dir. Load and connect archive files
    try { checkArchivator(true); } catch(TError&) { stop(); throw; }
}

void ModVArch::stop( bool full_del )
{
    bool curSt = startStat();

    //Stop getting data cicle an detach archives
    TVArchivator::stop(full_del);

    if(curSt)	infoTbl = "";
}

bool ModVArch::filePrmGet( const string &anm, string *archive, TFld::Type *vtp, int64_t *abeg, int64_t *aend, int64_t *aper )
{
    int bufSz = limArchID_SZ;
    char buf[bufSz+1]; buf[bufSz] = 0;
    bool unpck = false;
    string a_fnm = anm;
    if(mod->filePack(anm)) {
	bool infoOK = false;

	//Get archive info from info file
	int hd = open((anm+".info").c_str(), O_RDONLY);
	if(hd >= 0) {
	    int ibufSz = 80;
	    char ibuf[ibufSz+1]; ibuf[ibufSz] = 0;
	    int rsz = read(hd, ibuf, ibufSz);
	    if(rsz > 0 && rsz < ibufSz) {
		ibuf[rsz] = 0;
		int64_t tBeg, tEnd, tPer;
		int tVTp;
		if(sscanf(ibuf,("%llx %llx %"+i2s(limArchID_SZ)+"s %llx %d").c_str(),&tBeg,&tEnd,buf,&tPer,&tVTp) == 5) {
		    if(abeg)	*abeg = tBeg;
		    if(aend)	*aend = tEnd;
		    if(archive)	*archive = buf;
		    if(aper)	*aper = tPer;
		    if(vtp)	*vtp = (TFld::Type)tVTp;
		    infoOK = true;
		}
	    }
	    close(hd);
	}

	//Get file info from DB
	if(!infoOK) {
	    TConfig cEl(&mod->packFE());
	    cEl.cfg("FILE").setS(anm);
	    if(SYS->db().at().dataGet((infoTbl.size()?infoTbl:mod->filesDB()),mod->nodePath()+"Pack/",cEl,false,true)) {
		if(abeg)	*abeg = strtoll(cEl.cfg("BEGIN").getS().c_str(),NULL,16);
		if(aend)	*aend = strtoll(cEl.cfg("END").getS().c_str(),NULL,16);
		if(archive)	*archive = cEl.cfg("PRM1").getS().substr(0, limArchID_SZ);
		if(aper)	*aper = strtoll(cEl.cfg("PRM2").getS().c_str(),NULL,16);
		if(vtp)		*vtp  = (TFld::Type)s2i(cEl.cfg("PRM3").getS());
		infoOK = true;
	    }
	}

	if(infoOK) return true;

	try {
	    mess_sys(TMess::Info, _("Unpacking '%s' for information."), anm.c_str());
	    a_fnm = mod->unPackArch(anm, false);
	} catch(TError&) { return false; }
	unpck = true;
    }
    //Get params from file
    int hd = open(a_fnm.c_str(), O_RDONLY);
    if(hd <= 0)	return false;
    // Read Header
    VFileArch::FHead head;
    int r_len = read(hd, &head, sizeof(VFileArch::FHead));
    close(hd);
    if(r_len < (int)sizeof(VFileArch::FHead) || VFileArch::afl_id != head.f_tp || head.term != 0x55) return false;
    string aId = getArchiveID(head, TSYS::pathLevEnd(a_fnm,0));
    // Check to archive present
    if(archive)	*archive = aId;
    if(abeg)	*abeg = head.beg;
    if(aend)	*aend = head.end;
    if(aper)	*aper = head.period;
    if(vtp)	*vtp  = (TFld::Type)(head.vtp|(head.vtpExt<<4));

    if(unpck) {
	if(mess_lev() == TMess::Debug) mess_sys(TMess::Debug, "Remove file '%s' after the unpack.", a_fnm.c_str());
	remove(a_fnm.c_str());

	if(!packInfoFiles() || infoTbl.size()) {
	    // Write info to DB
	    TConfig cEl(&mod->packFE());
	    cEl.cfg("FILE").setS(anm);
	    cEl.cfg("BEGIN").setS(ll2s(head.beg,TSYS::Hex));
	    cEl.cfg("END").setS(ll2s(head.end,TSYS::Hex));
	    cEl.cfg("PRM1").setS(aId);
	    cEl.cfg("PRM2").setS(ll2s(head.period,TSYS::Hex));
	    cEl.cfg("PRM3").setS(i2s(head.vtp|(head.vtpExt<<4)));
	    SYS->db().at().dataSet((infoTbl.size()?infoTbl:mod->filesDB()), mod->nodePath()+"Pack/", cEl, false, true);
	}
	else if((hd=open((anm+".info").c_str(),O_WRONLY|O_CREAT|O_TRUNC,SYS->permCrtFiles())) > 0) {
	    // Write info to info file
	    string si = TSYS::strMess("%llx %llx %s %llx %d", head.beg, head.end, aId.c_str(), head.period, head.vtp|(head.vtpExt<<4));
	    bool fOK = (write(hd,si.data(),si.size()) == (int)si.size());
	    close(hd);
	    if(!fOK) return false;
	}
    }

    return true;
}

void ModVArch::checkArchivator( bool now, bool toLimits )
{
    if(!startStat())	return;

    chkANow = true;

    bool isTm = time(NULL) > (mLstCheck+checkTm()*60);

    //Present files of attached archives check.
    //!!!! Moved to the top for early capacity limits check
    ResAlloc res(archRes, false);
    if(maxCapacity() > 1 && (curCapacity()/1048576) > maxCapacity()) {
	vector< pair<int,ModVArchEl*> > sortEls;
	for(map<string,TVArchEl*>::iterator iel = archEl.begin(); iel != archEl.end(); ++iel)
	    sortEls.push_back(pair<int,ModVArchEl*>(((ModVArchEl*)iel->second)->files.size(),(ModVArchEl*)iel->second));
	sort(sortEls.begin(), sortEls.end());
	for(vector< pair<int,ModVArchEl*> >::reverse_iterator iel = sortEls.rbegin(); iel != sortEls.rend(); ++iel)
	    iel->second->checkArchivator(now || isTm || toLimits, maxCapacity() > 1 && (curCapacity()/1048576) > maxCapacity());
    }
    else for(map<string,TVArchEl*>::iterator iel = archEl.begin(); iel != archEl.end(); ++iel)
	((ModVArchEl*)iel->second)->checkArchivator(now || isTm || toLimits, false /*maxCapacity() > 1 && (curCapacity()/1048576) > maxCapacity()*/);
    res.unlock();

    //Archivator's folder check for new files attach and present files pack needs
    if(now || isTm) {
	// Open/create the archive directory
	DIR *IdDir = opendir(addr().c_str());
	if(IdDir == NULL) throw err_sys(_("The archive directory '%s' is not present."), addr().c_str());

	// Find archive files for no present archives and create it.
	struct stat file_stat;
	dirent	*scan_rez = NULL,
		*scan_dirent = (dirent*)malloc(offsetof(dirent,d_name) + NAME_MAX + 1);

	// Scan opened directory
	while(readdir_r(IdDir,scan_dirent,&scan_rez) == 0 && scan_rez) {
	    if(strcmp(scan_rez->d_name,"..") == 0 || strcmp(scan_rez->d_name,".") == 0 || strcmp(scan_rez->d_name,"info.db") == 0) continue;

	    string	ArhNm;
	    TFld::Type	ArhTp;
	    string NameArhFile = addr()+"/"+scan_rez->d_name;

	    stat(NameArhFile.c_str(), &file_stat);
	    if((file_stat.st_mode&S_IFMT) != S_IFREG || access(NameArhFile.c_str(),F_OK|R_OK) != 0) continue;

	    // Remove for empty files mostly after wrong or limited FSs
	    if(file_stat.st_size == 0) { remove(NameArhFile.c_str()); remove((NameArhFile+".info").c_str()); continue; }

	    // Pass for info and other impropper files
	    if(NameArhFile.compare(NameArhFile.size()-4,4,".val") != 0 && NameArhFile.compare(NameArhFile.size()-7,7,".val.gz") != 0) continue;

	    // Pass for files wich have not the proper header
	    if(!filePrmGet(NameArhFile,&ArhNm,&ArhTp,NULL,NULL,NULL))	continue;

	    //  Check for the archive presenting
	    AutoHD<TVArchive> varch;
	    if(owner().owner().valPresent(ArhNm)) varch = owner().owner().valAt(ArhNm);
	    else {
		//   Add no present archive
		owner().owner().valAdd(ArhNm);
		varch = owner().owner().valAt(ArhNm);
		varch.at().setToStart(true);
		varch.at().setValType(ArhTp);
		//varch.at().start();
	    }
	    //  Check for archive's start state and it starts early for propper redundancy sync
	    if(!varch.at().startStat() && varch.at().toStart())
		try { varch.at().start(); } catch(TError&) { continue; }	//!!!! Pass wrong archives
	    //  Check for attached
	    if(!varch.at().archivatorPresent(workId()))	varch.at().archivatorAttach(workId());
	    //  Try connect new file
	    ResAlloc res1(archRes, false);
	    map<string,TVArchEl*>::iterator iel = archEl.find(ArhNm);
	    if(iel != archEl.end()) ((ModVArchEl *)iel->second)->fileAdd(NameArhFile);
	}

	free(scan_dirent);
	closedir(IdDir);
	now = true;

	//Check for not presented files into the info table
	if(infoTbl.size() && isTm) {
	    TConfig cEl(&mod->packFE());
	    cEl.cfgViewAll(false);

	    for(int fldCnt = 0; SYS->db().at().dataSeek(infoTbl,mod->nodePath()+"Pack",fldCnt++,cEl); )
		if(stat(cEl.cfg("FILE").getS().c_str(),&file_stat) != 0 || (file_stat.st_mode&S_IFMT) != S_IFREG) {
		    if(!SYS->db().at().dataDel(infoTbl,mod->nodePath()+"Pack",cEl,true,false,true))	break;
		    fldCnt--;
		}
	}
    }

    chkANow = false;
    if(isTm)	mLstCheck = time(NULL);
}

void ModVArch::expArch( const string &arch_nm, time_t beg, time_t end, const string &file_tp, const string &file_nm )
{
    int64_t buf_sz = 100000;
    int64_t buf_per = (int64_t)(valPeriod()*1e6);
    int64_t c_tm;

    TValBuf buf(TFld::Real, buf_sz, buf_per, true, true);
    beg = vmax(beg, SYS->archive().at().valAt(arch_nm).at().begin(workId())/1000000);
    end = vmin(end, SYS->archive().at().valAt(arch_nm).at().end(workId())/1000000);

    if(file_tp == "wav") {
	//Export to wav
	struct {
	    char riff[4];
	    int  filesize;
	    char rifftype[4];
	} rif;
	struct {
	    char chunk_id[4];
	    int  chunksize;
	} chnk;
	struct {
	    short wFormatTag;
	    short nChannels;
	    int   nSamplesPerSec;
	    int   nAvgBytesPerSec;
	    short nBlockAlign;
	    short wBitsPerSample;
	} wv_form;

	strncpy(rif.riff, "RIFF", 4);
	rif.filesize = buf.realSize()*sizeof(float)+sizeof(rif)+2*sizeof(chnk)+sizeof(wv_form);
	strncpy(rif.rifftype,"WAVE",4);
	strncpy(chnk.chunk_id,"fmt ",4);
	chnk.chunksize = sizeof(wv_form);
	wv_form.wFormatTag = 3;
	wv_form.nChannels = 1;
	wv_form.nSamplesPerSec = 1000000/buf.period();
	wv_form.nAvgBytesPerSec = wv_form.nSamplesPerSec;
	wv_form.nBlockAlign = 4;
	wv_form.wBitsPerSample = 32;

	int hd = open((file_nm+"."+file_tp).c_str(), O_RDWR|O_CREAT|O_TRUNC, SYS->permCrtFiles());
	if(hd == -1) return;
	bool fOK = true;

	fOK = fOK && (write(hd,&rif,sizeof(rif)) == sizeof(rif));
	fOK = fOK && (write(hd,&chnk,sizeof(chnk)) == sizeof(chnk));
	fOK = fOK && (write(hd,&wv_form,sizeof(wv_form)) == sizeof(wv_form));
	strncpy(chnk.chunk_id, "data", 4);
	chnk.chunksize = 0;				//Set temporary size buf.realSize()*sizeof(float);
	off_t sz_pos = lseek(hd, 0, SEEK_CUR);
	fOK = fOK && (write(hd,&chnk,sizeof(chnk)) == sizeof(chnk));

	//Calc overage and scale of value
	float c_val, v_over = 0, v_max = -1e30, v_min = 1e30;

	c_tm = (int64_t)beg*1000000;
	while(c_tm < (int64_t)end*1000000) {
	    int64_t end_tm = c_tm+buf_sz*buf_per;
	    end_tm = vmin(end_tm, (int64_t)end*1000000);
	    SYS->archive().at().valAt(arch_nm).at().getVals(buf, c_tm, end_tm, workId());

	    // Check scale
	    for( ; c_tm <= buf.end(); c_tm += buf_per) {
		c_val = buf.getR(&c_tm, true);
		if(c_val == EVAL_REAL)	continue;
		v_max = vmax(c_val, v_max);
		v_min = vmin(c_val, v_min);
	    }
	    v_over = (v_max+v_min)/2;
	}
	buf.clear();

	//Transver value
	int val_cnt = 0;
	c_tm = (int64_t)beg*1000000;
	while(fOK && c_tm && c_tm < (int64_t)end*1000000) {
	    int64_t end_tm = c_tm+buf_sz*buf_per-buf_per;
	    end_tm = vmin(end_tm, (int64_t)end*1000000);
	    SYS->archive().at().valAt(arch_nm).at().getVals(buf, c_tm, end_tm, workId());

	    for( ; fOK && c_tm <= buf.end(); c_tm += buf_per, val_cnt++) {
		c_val = buf.getR(&c_tm, true);
		if(!c_tm) break;
		if(c_val == EVAL_REAL)	c_val = v_over;
		c_val = 2.*(c_val-v_over)/(v_max-v_min);
		fOK = fOK && (write(hd,&c_val,sizeof(float)) == sizeof(float));
	    }
	}

	//Write value count
	lseek(hd, sz_pos, SEEK_SET);
	chnk.chunksize = val_cnt*sizeof(float);
	fOK = fOK && (write(hd,&chnk,sizeof(chnk)) == sizeof(chnk));

	close(hd);
    }
    else {
	char c_val[40];
	int hd = open((file_nm+"."+file_tp).c_str(), O_RDWR|O_CREAT|O_TRUNC, SYS->permCrtFiles());
	if(hd == -1) return;
	bool fOK = true;

	c_tm = (int64_t)beg*1000000;
	while(fOK && c_tm && c_tm < (int64_t)end*1000000) {
	    int64_t end_tm = c_tm + buf_sz*buf_per - buf_per;
	    end_tm = vmin(end_tm, (int64_t)end*1000000);
	    SYS->archive().at().valAt(arch_nm).at().getVals(buf, c_tm, end_tm, workId());

	    for( ; fOK && c_tm <= buf.end(); c_tm += buf_per) {
		sprintf(c_val, "%g\n", buf.getR(&c_tm,true));
		if(!c_tm) break;
		fOK = fOK && (write(hd,c_val,strlen(c_val)) == (int)strlen(c_val));
	    }
	}
	close(hd);
    }
}

TVArchEl *ModVArch::getArchEl( TVArchive &arch )
{
    ModVArchEl *v_el = new ModVArchEl(arch, *this);
    if(!owner().owner().subStarting) v_el->checkArchivator(true);

    return v_el;
}

string ModVArch::getArchiveID( const VFileArch::FHead &head, const string &fNm )
{
    string resID = string(head.archive, sizeof(head.archive)).c_str();
    if(head.archive_add[0])
	resID += string(head.archive_add, sizeof(head.archive_add)).c_str();
    //Getting the complete id from file name
    if(head.archive_add[sizeof(head.archive_add)-1]) {
	TArrayObj *tArr;
	if((tArr=TRegExp("^(.+) [0-9]{4}-[0-9]{2}-[0-9]{2} [0-9]{2}\\.[0-9]{2}\\.[0-9]{2}\\.val").match(fNm))) {
	    if(tArr->size() >= 2 && tArr->arGet(1).getS().find(resID) == 0 && tArr->arGet(1).getS().size() > resID.size())
		resID = tArr->arGet(1).getS();
	    delete tArr;
	}
    }
    return resID;
}

void ModVArch::cntrCmdProc( XMLNode *opt )
{
    //Get page info
    if(opt->name() == "info") {
	TVArchivator::cntrCmdProc(opt);
	ctrMkNode("fld",opt,-1,"/prm/st/fsz",_("Overall size of the archiver files"),R_R_R_,"root",SARH_ID,1,"tp","str");
	ctrMkNode("fld",opt,-1,"/prm/cfg/ADDR",EVAL_STR,startStat()?R_R_R_:RWRWR_,"root",SARH_ID,3,
	    "dest","sel_ed","select","/prm/cfg/dirList","help",_("Path to a directory for files to the value archiver."));
	ctrRemoveNode(opt,"/prm/cfg/A_PRMS");
	if(ctrMkNode("area",opt,-1,"/prm/add",_("Additional options"),R_R_R_,"root",SARH_ID)) {
	    ctrMkNode("fld",opt,-1,"/prm/add/tm",_("Time size of the archive files, hours"),RWRWR_,"root",SARH_ID,2,"tp","real","help",
		_("The parameter is set automatically when changing the periodicity of the values of the archiver "
		  "and in general proportional to the periodicity of the values of the archiver.\n"
		  "WARNING! Large archive files will be longer processed due to the long unpacking of "
		  "gzip files and primary indexing, while accessing archives is deeply in the history."));
	    ctrMkNode("fld",opt,-1,"/prm/add/fn",_("Maximum number of files per archive"),RWRWR_,"root",SARH_ID,2,"tp","dec","help",
		_("Limits the maximum number for files of the archive and additional with the size of single file "
		  "it determines the size of archive on disk.\nCompletely removing this restriction can be performed by setting the parameter to zero."));
	    ctrMkNode("fld",opt,-1,"/prm/add/maxCpct",_("Maximum size of all archives, MB"),RWRWR_,"root",SARH_ID,2,"tp","real","help",
		_("Sets limit to maximum amount of the disk space of all arhive's files of the archiver.\n"
		  "The check is carried out with periodicity of checking the archives, as a result, when the limit is exceeded, "
		  "the oldest files of all archives are removed.\nTo completely remove this restriction you can set it to zero."));
	    ctrMkNode("fld",opt,-1,"/prm/add/round",_("Rounding for numeric values (%)"),RWRWR_,"root",SARH_ID,2,"tp","real","help",
		_("Sets the maximum percentage difference between the values of the parameters of the integer and the real types "
		  "at which they are considered the same and are arranged in the archive as one value, through successive packaging.\n"
		  "Allows you to pack lightly changing values beyond the limits of validity.\n"
		  "To disable this property you can it set to zero."));
	    ctrMkNode("fld",opt,-1,"/prm/add/pcktm",_("Timeout packaging archive files, minutes"),RWRWR_,"root",SARH_ID,2,"tp","dec","help",
		_("Sets the time after which, in the absence of requests, the archive file will be packaged in a gzip archive.\n"
		  "Set to zero for disabling the packing by gzip."));
	    ctrMkNode("fld",opt,-1,"/prm/add/tmout",_("Period of the archives checking, minutes"),RWRWR_,"root",SARH_ID,2,"tp","dec","help",
		_("Sets the periodicity of checking the archives for the appearance or deletion files into the archive folder, "
		  "as well as exceeding the limits and removing old archives files."));
	    ctrMkNode("fld",opt,-1,"/prm/add/pack_info_fl",_("Use info file for packaged archives"),RWRWR_,"root",SARH_ID,2,"tp","bool","help",
		_("Specifies whether to create a file with information about the packed archive files by gzip-archiver.\n"
		  "When copying the files of archive to another station, this info file can speed up the target station "
		  "process of first run by eliminating the need to decompress by gzip-archiver in order to obtain the information."));
	    ctrMkNode("comm",opt,-1,"/prm/add/chk_nw",_("Check now for the directory of the archiver"),RWRW__,"root",SARH_ID,1,"help",
		_("The command, which allows you to immediately start for checking the archives, "
		  "for example, after some manual changes into the directory of the archiver."));
	}
	ctrMkNode("list",opt,-1,"/arch/arch/4",_("Files size"),R_R_R_,"root",SARH_ID,1,"tp","str");
	if(ctrMkNode("comm",opt,-1,"/arch/exp",_("Export"),RWRW__,"root",SARH_ID)) {
	    ctrMkNode("fld",opt,-1,"/arch/exp/arch",_("Archive"),RWRW__,"root",SARH_ID,3,"tp","str","dest","select","select","/arch/lst");
	    ctrMkNode("fld",opt,-1,"/arch/exp/beg",_("Begin"),RWRW__,"root",SARH_ID,1,"tp","time");
	    ctrMkNode("fld",opt,-1,"/arch/exp/end",_("End"),RWRW__,"root",SARH_ID,1,"tp","time");
	    ctrMkNode("fld",opt,-1,"/arch/exp/tfl",_("Type"),RWRW__,"root",SARH_ID,3,"tp","str","dest","select","select","/arch/tpflst");
	    ctrMkNode("fld",opt,-1,"/arch/exp/file",_("To file"),RWRW__,"root",SARH_ID,1,"tp","str");
	}
	if(ctrMkNode("area",opt,-1,"/files",_("Files"),R_R___,"root",SARH_ID))
	    if(ctrMkNode("table",opt,-1,"/files/files",_("Files"),R_R___,"root",SARH_ID)) {
		ctrMkNode("list",opt,-1,"/files/files/arch",_("Archive"),R_R___,"root",SARH_ID,1,"tp","str");
		ctrMkNode("list",opt,-1,"/files/files/nm",_("Name"),R_R___,"root",SARH_ID,1,"tp","str");
		ctrMkNode("list",opt,-1,"/files/files/beg",_("Begin"),R_R___,"root",SARH_ID,1,"tp","str");
		ctrMkNode("list",opt,-1,"/files/files/end",_("End"),R_R___,"root",SARH_ID,1,"tp","str");
		ctrMkNode("list",opt,-1,"/files/files/per",_("Period, us"),R_R___,"root",SARH_ID,1,"tp","dec");
		ctrMkNode("list",opt,-1,"/files/files/tp",_("Type"),R_R___,"root",SARH_ID,1,"tp","str");
		ctrMkNode("list",opt,-1,"/files/files/sz",_("Size"),R_R___,"root",SARH_ID,1,"tp","str");
		ctrMkNode("list",opt,-1,"/files/files/pack",_("Pack"),R_R___,"root",SARH_ID,1,"tp","bool");
		ctrMkNode("list",opt,-1,"/files/files/err",_("Error"),R_R___,"root",SARH_ID,1,"tp","bool");
	    }
	return;
    }
    //Process command to page
    string a_path = opt->attr("path");
    if(a_path == "/prm/cfg/dirList" && ctrChkNode(opt))	TSYS::ctrListFS(opt, addr());
    else if(a_path == "/prm/st/fsz" && ctrChkNode(opt))	opt->setText(TSYS::cpct2str(curCapacity()));
    else if(a_path == "/prm/add/tm") {
	if(ctrChkNode(opt,"get",RWRWR_,"root",SARH_ID,SEC_RD))	opt->setText(r2s(fileTimeSize(),6));
	if(ctrChkNode(opt,"set",RWRWR_,"root",SARH_ID,SEC_WR))	setFileTimeSize(s2r(opt->text()));
    }
    else if(a_path == "/prm/add/fn") {
	if(ctrChkNode(opt,"get",RWRWR_,"root",SARH_ID,SEC_RD))	opt->setText(i2s(numbFiles()));
	if(ctrChkNode(opt,"set",RWRWR_,"root",SARH_ID,SEC_WR))	setNumbFiles(s2i(opt->text()));
    }
    else if(a_path == "/prm/add/maxCpct") {
	if(ctrChkNode(opt,"get",RWRWR_,"root",SARH_ID,SEC_RD))	opt->setText(r2s(maxCapacity(),6));
	if(ctrChkNode(opt,"set",RWRWR_,"root",SARH_ID,SEC_WR))	setMaxCapacity(s2r(opt->text()));
    }
    else if(a_path == "/prm/add/round") {
	if(ctrChkNode(opt,"get",RWRWR_,"root",SARH_ID,SEC_RD))	opt->setText(r2s(roundProc(),6));
	if(ctrChkNode(opt,"set",RWRWR_,"root",SARH_ID,SEC_WR))	setRoundProc(s2r(opt->text()));
    }
    else if(a_path == "/prm/add/pcktm") {
	if(ctrChkNode(opt,"get",RWRWR_,"root",SARH_ID,SEC_RD))	opt->setText(i2s(packTm()));
	if(ctrChkNode(opt,"set",RWRWR_,"root",SARH_ID,SEC_WR))	setPackTm(s2i(opt->text()));
    }
    else if(a_path == "/prm/add/tmout") {
	if(ctrChkNode(opt,"get",RWRWR_,"root",SARH_ID,SEC_RD))	opt->setText(i2s(checkTm()));
	if(ctrChkNode(opt,"set",RWRWR_,"root",SARH_ID,SEC_WR))	setCheckTm(s2i(opt->text()));
    }
    else if(a_path == "/prm/add/pack_info_fl") {
	if(ctrChkNode(opt,"get",RWRWR_,"root",SARH_ID,SEC_RD))	opt->setText(i2s(packInfoFiles()));
	if(ctrChkNode(opt,"set",RWRWR_,"root",SARH_ID,SEC_WR))	setPackInfoFiles(s2i(opt->text()));
    }
    else if(a_path == "/prm/add/chk_nw" && ctrChkNode(opt,"set",RWRW__,"root",SARH_ID,SEC_WR))	checkArchivator(true);
    else if(a_path == "/arch/arch" && ctrChkNode(opt)) {
	// Fill Archives table
	XMLNode *n_arch = ctrMkNode("list",opt,-1,"/arch/arch/0","");
	XMLNode *n_per  = ctrMkNode("list",opt,-1,"/arch/arch/1","");
	XMLNode *n_size = ctrMkNode("list",opt,-1,"/arch/arch/2","");
	XMLNode *n_lstRd = ctrMkNode("list",opt,-1,"/arch/arch/3","");
	XMLNode *f_size = ctrMkNode("list",opt,-1,"/arch/arch/4","");

	ResAlloc res(archRes, false);
	for(map<string,TVArchEl*>::iterator iel = archEl.begin(); iel != archEl.end(); ++iel) {
	    if(n_arch)	n_arch->childAdd("el")->setText(iel->second->archive().id());
	    if(n_per)	n_per->childAdd("el")->setText(r2s((double)iel->second->archive().period()/1e6,6));
	    if(n_size)	n_size->childAdd("el")->setText(i2s(iel->second->archive().size()));
	    if(n_lstRd)	n_lstRd->childAdd("el")->setText(atm2s(iel->second->mLastGet*1e-6)+"."+i2s(iel->second->mLastGet%1000000));
	    if(f_size)	f_size->childAdd("el")->setText(TSYS::cpct2str((double)((ModVArchEl *)iel->second)->size()));
	}
    }
    else if(a_path == "/arch/lst" && ctrChkNode(opt)) {
	vector<string> a_ls;
	archiveList(a_ls);
	for(unsigned i_el = 0; i_el < a_ls.size(); i_el++)
	    opt->childAdd("el")->setText(a_ls[i_el]);
    }
    else if(a_path == "/arch/tpflst" && ctrChkNode(opt)) {
	opt->childAdd("el")->setText("ascii");
	opt->childAdd("el")->setText("wav");
    }
    else if(a_path == "/arch/exp" && ctrChkNode(opt,"set",RWRW__,"root",SARH_ID,SEC_WR))
	expArch(ctrId(opt,"arch")->text(),
		s2i(ctrId(opt,"beg")->text()),
		s2i(ctrId(opt,"end")->text()),
		ctrId(opt,"tfl")->text(),
		ctrId(opt,"file")->text());
    else if(a_path == "/files/files" && ctrChkNode(opt,"get",R_R___,"root",SARH_ID,SEC_RD)) {
	XMLNode *rwArch	= ctrMkNode("list",opt,-1,"/files/files/arch","",R_R___,"root",SARH_ID);
	XMLNode *rwNm	= ctrMkNode("list",opt,-1,"/files/files/nm","",R_R___,"root",SARH_ID);
	XMLNode *rwBeg	= ctrMkNode("list",opt,-1,"/files/files/beg","",R_R___,"root",SARH_ID);
	XMLNode *rwEnd	= ctrMkNode("list",opt,-1,"/files/files/end","",R_R___,"root",SARH_ID);
	XMLNode *rwPer	= ctrMkNode("list",opt,-1,"/files/files/per","",R_R___,"root",SARH_ID);
	XMLNode *rwTp	= ctrMkNode("list",opt,-1,"/files/files/tp","",R_R___,"root",SARH_ID);
	XMLNode *rwSz	= ctrMkNode("list",opt,-1,"/files/files/sz","",R_R___,"root",SARH_ID);
	XMLNode *rwPack	= ctrMkNode("list",opt,-1,"/files/files/pack","",R_R___,"root",SARH_ID);
	XMLNode *rwErr	= ctrMkNode("list",opt,-1,"/files/files/err","",R_R___,"root",SARH_ID);

	ResAlloc res(archRes, false);
	for(map<string,TVArchEl*>::iterator iel = archEl.begin(); iel != archEl.end(); ++iel) {
	    ModVArchEl *el = dynamic_cast<ModVArchEl*>(iel->second);
	    if(!el) continue;
	    ResAlloc res1(el->mRes, false);
	    for(unsigned iF = 0; iF < el->files.size(); iF++) {
		if(rwArch)	rwArch->childAdd("el")->setText(el->archive().id());
		if(rwNm)	rwNm->childAdd("el")->setText(el->files[iF]->name());
		if(rwBeg)	rwBeg->childAdd("el")->setText(atm2s(el->files[iF]->begin()/1000000)+"."+i2s(el->files[iF]->begin()%1000000));
		if(rwEnd)	rwEnd->childAdd("el")->setText(atm2s(el->files[iF]->end()/1000000)+"."+i2s(el->files[iF]->end()%1000000));
		if(rwPer)	rwPer->childAdd("el")->setText(ll2s(el->files[iF]->period()));
		if(rwTp) {
		    string stp = _("Unknown");
		    switch(el->files[iF]->type()) {
			case TFld::Boolean:	stp = _("Boolean");	break;
			case TFld::Int16:	stp = _("Int16");	break;
			case TFld::Int32:	stp = _("Int32");	break;
			case TFld::Int64:	stp = _("Int64");	break;
			case TFld::Float:	stp = _("Float");	break;
			case TFld::Double:	stp = _("Double");	break;
			case TFld::String:	stp = _("String");	break;
			default: break;
		    }
		    rwTp->childAdd("el")->setText(stp);
		}
		if(rwSz)	rwSz->childAdd("el")->setText(TSYS::cpct2str(el->files[iF]->size()));
		if(rwPack)	rwPack->childAdd("el")->setText(i2s(el->files[iF]->isPack()));
		if(rwErr)	rwErr->childAdd("el")->setText(i2s(el->files[iF]->err()));
	    }
	}
    }
    else TVArchivator::cntrCmdProc(opt);
}

//*************************************************
//* FSArch::ModVArchEl - Value archive element    *
//*************************************************
ModVArchEl::ModVArchEl( TVArchive &iachive, TVArchivator &iarchivator ) : TVArchEl(iachive,iarchivator), mChecked(false), realEnd(0)
{

}

ModVArchEl::~ModVArchEl( )
{
    //Clear a files list
    ResAlloc res(mRes, true);
    while(files.size()) {
	delete files[0];
	files.pop_front();
    }
    res.release();
}

void ModVArchEl::fullErase( )
{
    //Remove files of the archive
    ResAlloc res(mRes, true);
    while(files.size()) {
	files[0]->delFile();
	delete files[0];
	files.pop_front();
    }
    res.release();
}

int ModVArchEl::size( )
{
    int rez = 0;
    ResAlloc res(mRes, false);
    for(unsigned iArh = 0; iArh < files.size(); iArh++)
	rez += files[iArh]->size();

    return rez;
}

void ModVArchEl::checkArchivator( bool now, bool cpctLim )
{
    if(now && !archivator().chkANow) {
	// Open archive derictory
	DIR *IdDir = opendir(archivator().addr().c_str());
	if(IdDir == NULL) return;

	//Scan directory for find new files and deleted files
	struct stat file_stat;
	dirent	*scan_rez = NULL,
		*scan_dirent = (dirent*)malloc(offsetof(dirent,d_name) + NAME_MAX + 1);

	// Check to allow files
	while(readdir_r(IdDir,scan_dirent,&scan_rez) == 0 && scan_rez) {
	    if(strcmp(scan_rez->d_name,"..") == 0 || strcmp(scan_rez->d_name,".") == 0)	continue;

	    string ArhNm;
	    string NameArhFile = archivator().addr()+"/"+scan_rez->d_name;

	    stat(NameArhFile.c_str(),&file_stat);
	    if((file_stat.st_mode&S_IFMT) != S_IFREG || access(NameArhFile.c_str(),F_OK|R_OK) != 0)	continue;
	    if(!archivator().filePrmGet(NameArhFile,&ArhNm,NULL,NULL,NULL,NULL) || archive().id() != ArhNm)	continue;
	    fileAdd(NameArhFile);
	}

	free(scan_dirent);
	closedir(IdDir);
    }
    if(now) mChecked = true;

    ResAlloc res(mRes, true);
    //Check file count for delete old files
    if(now && !mod->noArchLimit && ((((ModVArch &)archivator()).numbFiles() && files.size() > ((ModVArch &)archivator()).numbFiles()) || cpctLim))
	for(int iArh = 0; iArh < (int)files.size()-1; ) {	//Up to last fresh
	    if(!(files.size() > ((ModVArch &)archivator()).numbFiles() || cpctLim))	break;
	    else if(!files[iArh]->err()) {
		if(mess_lev() == TMess::Debug) archive().mess_sys(TMess::Debug, "Remove file '%s' by limit.", files[iArh]->name().c_str());
		files[iArh]->delFile();
		delete files[iArh];
		files.erase(files.begin() + iArh);

		if(cpctLim) break;
		continue;
	    }
	    iArh++;
	}

    //Check the archive's files for pack
    res.request(false);
    for(unsigned iArh = 0; iArh < files.size(); iArh++) files[iArh]->check();
}

void ModVArchEl::fileAdd( const string &file )
{
    //Check to present archive files
    ResAlloc res(mRes, false);
    for(unsigned iArh = 0; iArh < files.size(); iArh++)
	if(files[iArh]->name() == file) return;
    res.release();

    //Attach a new archive file
    VFileArch *f_arh = new VFileArch(this);
    f_arh->attach(file);

    // Broken archives delete. Oldest archives to up.
    if(f_arh->err()) delete f_arh;
    else {
	res.request(true);
	int iArh;
	for(iArh = (int)files.size()-1; iArh >= 0; iArh--)
	    if(files[iArh]->err() || f_arh->begin() >= files[iArh]->begin()) {
		files.insert(files.begin()+iArh+1,f_arh);
		break;
	    }
	if(iArh < 0) files.push_front(f_arh);
	realEnd = 0;	//Reset real end position
    }
}

int64_t ModVArchEl::end( )
{
    if(realEnd) return realEnd;

    //Real end first calculation
    ResAlloc res(mRes, false);
    int64_t curTm = TSYS::curTime();
    VFileArch *lstFile = NULL;
    for(unsigned iA = 0; iA < files.size(); iA++) {
	if(files[iA]->err()) continue;
	lstFile = files[iA];
	if(curTm <= lstFile->end()) {
	    if(!realEnd) realEnd = lstFile->endData();
	    break;
	}
    }
    if(lstFile && !realEnd) realEnd = lstFile->endData();

    return realEnd;
}

int64_t ModVArchEl::begin( )
{
    ResAlloc res(mRes,false);
    for(unsigned iA = 0; iA < files.size(); iA++)
	if(!files[iA]->err())
	    return files[iA]->begin();

    return 0;
}

void ModVArchEl::getValsProc( TValBuf &buf, int64_t ibeg, int64_t iend )
{
    //Request by single values for most big buffer period
    if(buf.period()/100 > (int64_t)(archivator().valPeriod()*1e6)) {
	ibeg = (ibeg/buf.period())*buf.period();
	for(int64_t ctm; ibeg <= iend; ibeg += buf.period()) {
	    ctm = ibeg;
	    TVariant vl = getValProc(&ctm, false);
	    buf.set(vl, ibeg);
	}
	return;
    }

    ResAlloc res(mRes, false);
    for(unsigned iA = 0; iA < files.size(); iA++)
	if(ibeg > iend) break;
	else if(!files[iA]->err() && ibeg <= files[iA]->end() && iend >= files[iA]->begin()) {
	    for( ; ibeg < files[iA]->begin(); ibeg += files[iA]->period()) buf.setI(EVAL_INT,ibeg);
	    files[iA]->getVals(buf, ibeg, vmin(iend,files[iA]->end()));
	    ibeg = files[iA]->end()+files[iA]->period();
	}
    for( ; ibeg <= iend; ibeg += (int64_t)(archivator().valPeriod()*1e6)) buf.setI(EVAL_INT, ibeg);
}

TVariant ModVArchEl::getValProc( int64_t *tm, bool up_ord )
{
    int64_t itm = tm ? *tm : SYS->curTime();
    int64_t per;
    ResAlloc res(mRes, false);
    for(unsigned iA = 0; iA < files.size(); iA++)
	if(!files[iA]->err() && (
		(up_ord && itm <= files[iA]->end() && itm > files[iA]->begin()-files[iA]->period()) ||
		(!up_ord && itm < files[iA]->end()+files[iA]->period() && itm >= files[iA]->begin())))
	{
	    if(tm) { per = files[iA]->period(); *tm = (itm/per)*per+((up_ord&&itm%per)?per:0); }
	    return files[iA]->getVal(up_ord?files[iA]->maxPos()-(files[iA]->end()-itm)/files[iA]->period():(itm-files[iA]->begin())/files[iA]->period());
	}
    if(tm) { per = (int64_t)(archivator().valPeriod()*1e6); *tm = (itm>=begin()||itm<=end()) ? (itm/per)*per+((up_ord&&itm%per)?per:0) : 0; }
    return EVAL_REAL;
}

int64_t ModVArchEl::setValsProc( TValBuf &buf, int64_t beg, int64_t end, bool toAccum )
{
    //Check border
    if(!buf.vOK(beg,end)) return false;
    beg = vmax(beg, buf.begin());
    end = vmin(end, buf.end());
    int64_t b_prev = 0;
    int64_t f_sz = (int64_t)(((ModVArch&)archivator()).fileTimeSize()*3600e6);
    int64_t v_per = (int64_t)(archivator().valPeriod()*1e6);
    beg = (beg/v_per)*v_per;
    end = (end/v_per)*v_per;

    //Put values to files
    ResAlloc res(mRes, true);
    bool wrOK = true, wrCurOK = false;	//check for all writes OK else repeate
    for(unsigned iA = 0; iA < files.size(); iA++)
	if(!files[iA]->err() && beg <= end) {
	    // Create new file for old data
	    if(beg < files[iA]->begin()) {
		if(!mChecked)	return 0;	//Wait for checking

		//  Calc file limits
		int64_t n_end, n_beg;	//New file end position
		if((files[iA]->begin()-beg) > f_sz) n_end = beg+f_sz;
		else n_end = files[iA]->begin()-v_per;
		n_beg = vmax(b_prev, n_end-f_sz);

		//  Create file name
		string AName = archivator().addr() + "/" + archive().id() + atm2s(n_beg/1000000," %F %H.%M.%S.val");
		files.insert(files.begin()+iA, new VFileArch(AName,n_beg,n_end,v_per,archive().valType(true),this));
		//Remove new error created file mostly by store space lack
		if(files[iA]->err()) {
		    files[iA]->delFile();
		    delete files[iA];
		    files.erase(files.begin()+iA);
		    return 0;
		}
	    }

	    // Insert values to the archive
	    if(beg <= files[iA]->end() && end >= files[iA]->begin()) {
		int64_t n_end = (end > files[iA]->end())?files[iA]->end():end;
		res.release();
		if((wrCurOK=files[iA]->setVals(buf,beg,n_end))) realEnd = vmax(realEnd, n_end);
		wrOK = wrOK && wrCurOK;
		beg = n_end + v_per;
		res.request(true);
	    }
	    b_prev = files[iA]->end() + v_per;
	}
    //Create new file for new data
    while(end >= beg) {
	if(!mChecked)	return 0;	//Wait for checking

	int64_t n_end = beg + f_sz;
	string AName = archivator().addr() + "/" + archive().id() + atm2s(beg/1000000," %F %H.%M.%S.val");
	files.push_back(new VFileArch(AName,beg,n_end,v_per,archive().valType(true),this));
	//Remove new error created file mostly by store space lack
	if(files.back()->err()) {
	    files.back()->delFile();
	    delete files.back();
	    files.pop_back();
	    return 0;
	}
	n_end = (end > n_end) ? n_end : end;

	res.release();
	if((wrCurOK=files[files.size()-1]->setVals(buf,beg,n_end))) realEnd = vmax(realEnd, n_end);
	wrOK = wrOK && wrCurOK;
	beg = n_end + v_per;
    }

    return wrOK ? end : 0;
}

//*************************************************
//* FSArch::VFileArch - Value archivator file     *
//*************************************************
string VFileArch::afl_id = "OpenSCADA Val Arch.";

VFileArch::VFileArch( ModVArchEl *owner ) :
    dtRes(true), mName(dtRes), mSize(0), mTp(TFld::Real), mBeg(0), mEnd(0), mPer(1000000), mErr(true), mPack(false),
    fixVl(true), vSize(sizeof(double)), mpos(0), intoRep(false), mOwner(owner)
{
    cach_pr_rd.pos = cach_pr_rd.off = cach_pr_wr.pos = cach_pr_wr.off = 0;
    mAcces = time(NULL);
}

VFileArch::VFileArch( const string &iname, int64_t ibeg, int64_t iend, int64_t iper, TFld::Type itp, ModVArchEl *iowner) :
    dtRes(true), mName(dtRes), mSize(0), mTp(itp), mBeg(ibeg), mEnd(iend), mPer(iper), mErr(false), mPack(false), intoRep(false), mOwner(iowner)
{
    mName = iname;
    char buf[1000];
    cach_pr_rd.pos = cach_pr_rd.off = cach_pr_wr.pos = cach_pr_wr.off = 0;

    //Round-up begin and end position to period
    mBeg = (mBeg/mPer)*mPer;
    mEnd = (mEnd/mPer)*mPer + mPer;

    //Open/create new archive file
    int hd = open(name().c_str(), O_RDWR|O_CREAT|O_TRUNC, SYS->permCrtFiles());
    if(hd <= 0) {
	owner().archivator().mess_sys(TMess::Error, _("Error creating a file '%s': %s(%d)."), name().c_str(), strerror(errno), errno);
	mErr = true;
	return;
    }

    //Prepare and write the file archive header
    FHead head;
    memset(&head, 0, sizeof(FHead));
    strncpy(head.f_tp, afl_id.c_str(), sizeof(head.f_tp));
    strncpy(head.archive, owner().archive().id().c_str(), sizeof(head.archive));
    if(owner().archive().id().size() > sizeof(head.archive))
	strncpy(head.archive_add, owner().archive().id().c_str()+sizeof(head.archive), sizeof(head.archive_add));
    head.beg = begin();
    head.end = end();
    head.period = period();
    head.vtp = type()&TFld::GenMask;
    head.vtpExt = type()>>4;
    head.hgrid = owner().archive().hardGrid();
    head.hres = owner().archive().highResTm();
    head.term = 0x55;
    bool fOK = (write(hd,&head,sizeof(FHead)) == sizeof(FHead));

    //Create bit table and init first value
    mpos = (end()-begin())/period();
    switch(type()) {
	case TFld::String: {
	    fixVl = false;
	    vSize = sizeof(char);
	    string s_val = EVAL_STR;
	    eVal  = s_val;

	    //Init pack table
	    int i_buf = 0;
	    for(int i_bt = 0; i_bt < mpos*vSize; i_bt++, i_buf++) {
		if(i_buf == sizeof(buf)) { fOK = fOK && (write(hd,buf,i_buf) == i_buf); i_buf = 0; }
		buf[i_buf] = i_bt ? 0 : strlen(EVAL_STR);
	    }
	    if(i_buf)	fOK = fOK && (write(hd,buf,i_buf) == i_buf);

	    fOK = fOK && (write(hd,s_val.c_str(),s_val.size()) == (int)s_val.size());
	    break;
	}
	case TFld::Int16: {
	    fixVl = true;
	    vSize = sizeof(int16_t);
	    int16_t s_val = EVAL_INT16;
	    eVal.assign((char*)&s_val, vSize);

	    int i_buf = 0;
	    for(int i_bt = 0; i_bt < (mpos/8+(bool)(mpos%8)); i_bt++, i_buf++) {
		if(i_buf == sizeof(buf)) { fOK = fOK && (write(hd,buf,i_buf) == i_buf); i_buf = 0; }
		buf[i_buf] = i_bt ? 0 : 1;
	    }
	    if(i_buf)	fOK = fOK && (write(hd,buf,i_buf) == i_buf);

	    fOK = fOK && (write(hd,&s_val,sizeof(s_val)) == sizeof(s_val));
	    break;
	}
	case TFld::Int32: {
	    fixVl = true;
	    vSize = sizeof(int32_t);
	    int32_t s_val = EVAL_INT32;
	    eVal.assign((char*)&s_val, vSize);

	    int i_buf = 0;
	    for(int i_bt = 0; i_bt < (mpos/8+(bool)(mpos%8)); i_bt++, i_buf++) {
		if(i_buf == sizeof(buf)) { fOK = fOK && (write(hd,buf,i_buf) == i_buf); i_buf = 0; }
		buf[i_buf] = i_bt ? 0 : 1;
	    }
	    if(i_buf)	fOK = fOK && (write(hd,buf,i_buf) == i_buf);

	    fOK = fOK && (write(hd,&s_val,sizeof(s_val)) == sizeof(s_val));
	    break;
	}
	case TFld::Int64: {
	    fixVl = true;
	    vSize = sizeof(int64_t);
	    int64_t s_val = EVAL_INT64;
	    eVal.assign((char*)&s_val, vSize);

	    int i_buf = 0;
	    for(int i_bt = 0; i_bt < (mpos/8+(bool)(mpos%8)); i_bt++, i_buf++) {
		if(i_buf == sizeof(buf)) { fOK = fOK && (write(hd,buf,i_buf) == i_buf); i_buf = 0; }
		buf[i_buf] = i_bt ? 0 : 1;
	    }
	    if(i_buf)	fOK = fOK && (write(hd,buf,i_buf) == i_buf);

	    fOK = fOK && (write(hd,&s_val,sizeof(s_val)) == sizeof(s_val));
	    break;
	}
	case TFld::Float: {
	    fixVl = true;
	    vSize = sizeof(float);
	    float s_val = TSYS::floatLE(EVAL_RFlt);
	    eVal.assign((char*)&s_val, vSize);

	    int i_buf = 0;
	    for(int i_bt = 0; i_bt < (mpos/8+(bool)(mpos%8)); i_bt++, i_buf++) {
		if(i_buf == sizeof(buf)) { fOK = fOK && (write(hd,buf,i_buf) == i_buf); i_buf = 0; }
		buf[i_buf] = i_bt ? 0 : 1;
	    }
	    if(i_buf)	fOK = fOK && (write(hd,buf,i_buf) == i_buf);

	    fOK = fOK && (write(hd,&s_val,sizeof(s_val)) == sizeof(s_val));
	    break;
	}
	case TFld::Double: {
	    fixVl = true;
	    vSize = sizeof(double);
	    double s_val = TSYS::doubleLE(EVAL_RDbl);
	    eVal.assign((char*)&s_val, vSize);

	    int i_buf = 0;
	    for(int i_bt = 0; i_bt < (mpos/8+(bool)(mpos%8)); i_bt++, i_buf++) {
		if(i_buf == sizeof(buf)) { fOK = fOK && (write(hd,buf,i_buf) == i_buf); i_buf = 0; }
		buf[i_buf] = i_bt ? 0 : 1;
	    }
	    if(i_buf)	fOK = fOK && (write(hd,buf,i_buf) == i_buf);

	    fOK = fOK && (write(hd,&s_val,sizeof(s_val)) == sizeof(s_val));
	    break;
	}
	case TFld::Boolean: {
	    fixVl = true;
	    vSize = sizeof(char);
	    char s_val = EVAL_BOOL;
	    eVal.assign((char*)&s_val,vSize);

	    int i_buf = 0;
	    for(int i_bt = 0; i_bt < (mpos/8+(bool)(mpos%8)); i_bt++, i_buf++) {
		if(i_buf == sizeof(buf)) { fOK = fOK && (write(hd,buf,i_buf) == i_buf); i_buf = 0; }
		buf[i_buf] = i_bt ? 0 : 1;
	    }
	    if(i_buf)	fOK = fOK && (write(hd,buf,i_buf) == i_buf);

	    fOK = fOK && (write(hd,&s_val,sizeof(s_val)) == sizeof(s_val));
	    break;
	}
	default: break;
    }
    mSize = lseek(hd, 0, SEEK_END);
    close(hd);
    mAcces = time(NULL);
    mErr = !fOK;
}

VFileArch::~VFileArch( )	{ }

void VFileArch::delFile( )
{
    ResAlloc res(mRes, true);
    remove(name().c_str());
    remove((name()+(isPack()?".info":".gz.info")).c_str());
    mErr = true;
}

void VFileArch::attach( const string &iname )
{
    try {
	ResAlloc res(mRes, true);

	mName = iname;
	mAcces = time(NULL);

	mPack = mod->filePack(name());
	mErr  = !owner().archivator().filePrmGet(name(), NULL, &mTp, &mBeg, &mEnd, &mPer);
	if(mErr)
	    throw owner().archivator().err_sys(_("Error reading parameters of the archive file '%s'!"), name().c_str());
	if(period() <= 0)
	    throw owner().archivator().err_sys(_("Error parameters of the archive file '%s'!"), name().c_str());

	//Init values type parameters
	switch(type()) {
	    case TFld::String: {
		fixVl = false;
		vSize = sizeof(char);
		eVal = EVAL_STR;
		break;
	    }
	    case TFld::Int16: {
		fixVl = true;
		vSize = sizeof(int16_t);
		int s_val = EVAL_INT16;
		eVal.assign((char*)&s_val, vSize);
		break;
	    }
	    case TFld::Int32: {
		fixVl = true;
		vSize = sizeof(int32_t);
		int s_val = EVAL_INT32;
		eVal.assign((char*)&s_val, vSize);
		break;
	    }
	    case TFld::Int64: {
		fixVl = true;
		vSize = sizeof(int64_t);
		int64_t s_val = EVAL_INT64;
		eVal.assign((char*)&s_val, vSize);
		break;
	    }
	    case TFld::Float: {
		fixVl = true;
		vSize = sizeof(float);
		float s_val = TSYS::floatLE(EVAL_RFlt);
		eVal.assign((char*)&s_val, vSize);
		break;
	    }
	    case TFld::Double: {
		fixVl = true;
		vSize = sizeof(double);
		double s_val = TSYS::doubleLE(EVAL_RDbl);
		eVal.assign((char*)&s_val, vSize);
		break;
	    }
	    case TFld::Boolean: {
		fixVl = true;
		vSize = sizeof(char);
		char s_val = EVAL_BOOL;
		eVal.assign((char*)&s_val, vSize);
		break;
	    }
	    default: break;
	}

	//Load previous val check
	bool load_prev = false;
	int64_t cur_tm = TSYS::curTime();
	if(cur_tm >= begin() && cur_tm <= end() && period() > 10000000) { owner().prevTm = cur_tm; load_prev = true; }

	//Check and prepare last archive files
	// Get file size
	int hd = open(name().c_str(), O_RDWR);
	if(hd == -1)	throw owner().archivator().err_sys(_("Archive file '%s' is not opened!"), name().c_str());
	mSize = lseek(hd, 0, SEEK_END);
	mpos = (end()-begin())/period();
	if(!mPack && cur_tm >= begin() && cur_tm <= end()) repairFile(hd);
	close(hd);
	res.release();

	// Load previous value
	if(load_prev && owner().prevVal == EVAL_REAL)
	    switch(type()) {
		case TFld::Int16: case TFld::Int32: case TFld::Int64: case TFld::Float: case TFld::Double:
		    owner().prevVal = getVal((cur_tm-begin())/period()).getR();
		    break;
		default: break;
	    }
    } catch(TError &err) {
	mess_err(err.cat.c_str(), "%s", err.mess.c_str());
	mod->mess_sys(TMess::Error, _("Error attaching the file '%s'."), name().c_str());
	mErr = true;
    }
}

void VFileArch::check( )
{
    //Check for pack archive file
    ResAlloc res(mRes, false);
    if(!err() && !mPack && owner().archivator().packTm() && (time(NULL) > mAcces + owner().archivator().packTm()*60)) {
	res.request(true);
	if(!mPack) mName = mod->packArch(name());
	mPack = true;

	// Get file size
	int hd = open(name().c_str(), O_RDONLY);
	if(hd > 0) { mSize = lseek(hd, 0, SEEK_END); close(hd);	}

	if(!owner().archivator().packInfoFiles() || owner().archivator().infoTbl.size()) {
	    // Write info to DB
	    TConfig cEl(&mod->packFE());
	    cEl.cfg("FILE").setS(mName);
	    cEl.cfg("BEGIN").setS(ll2s(begin(),TSYS::Hex));
	    cEl.cfg("END").setS(ll2s(end(),TSYS::Hex));
	    cEl.cfg("PRM1").setS(owner().archive().id());
	    cEl.cfg("PRM2").setS(ll2s(period(),TSYS::Hex));
	    cEl.cfg("PRM3").setS(i2s(type()));
	    SYS->db().at().dataSet((owner().archivator().infoTbl.size()?owner().archivator().infoTbl:mod->filesDB()),
		mod->nodePath()+"Pack/",cEl, false, true);
	}
	else if((hd=open((name()+".info").c_str(),O_WRONLY|O_CREAT|O_TRUNC,SYS->permCrtFiles())) > 0) {
	    // Write info to info file
	    string si = TSYS::strMess("%llx %llx %s %llx %d",begin(),end(),owner().archive().id().c_str(),period(),type());
	    if(write(hd,si.data(),si.size()) != (int)si.size())
		mod->mess_sys(TMess::Error, _("Error writing to '%s'!"), (name()+".info").c_str());
	    close(hd);
	}
    }
}

int64_t VFileArch::endData( )
{
    if(getVal(maxPos()).getS() != EVAL_STR) return end(); //vmin(SYS->curTime(), end());

    ResAlloc res(mRes, false);
    if(mErr) throw owner().archivator().err_sys(_("Error archive file!"));
    if(mPack) {
	res.request(true);
	try{ if(mPack) mName = mod->unPackArch(mName); } catch(TError&) { mErr = true; throw; }
	mPack = false;
	res.request(false);
    }

    //Open archive file
    int hd = open(name().c_str(), O_RDONLY);
    if(hd <= 0) { mErr = true; return end(); }

    //Find last value offset
    res.request(true);
    int vSz = 0, curPos = maxPos();
    for(int last_off = calcVlOff(hd,curPos,&vSz); curPos && getValue(hd,last_off,vSz) == eVal; last_off = calcVlOff(hd,curPos,&vSz)) {
	for(int d_win = curPos/2; d_win > 3; d_win /= 2)
	    if(calcVlOff(hd,curPos-d_win,&vSz) == last_off) curPos -= d_win;
	while(curPos > 0 && calcVlOff(hd,curPos,&vSz) == last_off) curPos--;
    }
    res.request(false);

    //Free file resource and close the file
    close(hd);
    mAcces = time(NULL);
    res.release();

    return begin() + (int64_t)curPos*period();
}

void VFileArch::getVals( TValBuf &buf, int64_t beg, int64_t end )
{
    int vpos_beg, vpos_end, voff_beg, vlen_beg, voff_end, vlen_end;
    char *pid_b, *val_b = NULL;

    ResAlloc res(mRes, false);
    if(mErr) throw owner().archivator().err_sys(_("Error archive file!"));

    //Get values block character
    vpos_beg = vmax(0, (beg-begin())/period());
    if(vpos_beg > mpos)	return;
    vpos_end = vmin(mpos, (end-begin())/period());
    if(vpos_end < 0)	return;
    //if((vpos_end-vpos_beg) > (TArchiveS::max_req_vals-buf.realSize()))
    //	vpos_end = vpos_beg+TArchiveS::max_req_vals-buf.realSize();
    if(vpos_beg > vpos_end)	return;

    if(mPack) {
	res.request(true);
	try{ if(mPack) mName = mod->unPackArch(mName); }
	catch(TError&) {
	    try {
		owner().archivator().checkArchivator(false, true);	// Try to remove some files by limits
		mName = mod->unPackArch(mName);
	    } catch(TError&) { mErr = true; throw; }
	}
	mPack = false;
	res.request(false);
    }

    //Open archive file
    int hd = open(name().c_str(), O_RDONLY);
    if(hd <= 0) { mErr = true; return; }
    bool fOK = true;

    res.request(true);
    voff_beg = calcVlOff(hd, vpos_beg, &vlen_beg);
    res.request(false);

    //Get the pack index block and the value block
    if(fixVl) {
	// Get index block
	int i_beg = sizeof(FHead) + vpos_beg/8;
	int i_end = sizeof(FHead) + vpos_end/8+1;
	lseek(hd, i_beg, SEEK_SET);
	pid_b = (char*)malloc(i_end-i_beg);
	if((fOK=(read(hd,pid_b,i_end-i_beg) == (i_end-i_beg)))) {
	    // Calc end offset
	    voff_end = voff_beg;
	    for(int iPos = vpos_beg+1; iPos <= vpos_end; iPos++)
		voff_end += vSize*(bool)((0x01<<(iPos%8))&pid_b[(iPos/8)-(vpos_beg/8)]);
	    // Get value block
	    i_beg = voff_beg;
	    i_end = voff_end+vSize;
	    lseek(hd, i_beg, SEEK_SET);
	    val_b = (char*)malloc(i_end-i_beg);
	    fOK = (read(hd,val_b,i_end-i_beg) == (i_end-i_beg));
	}
    }
    else {
	// Get index block
	int i_beg = sizeof(FHead) + vpos_beg*vSize;
	int i_end = sizeof(FHead) + vpos_end*vSize+vSize;
	lseek(hd, i_beg, SEEK_SET);
	pid_b = (char*)malloc(i_end-i_beg);
	if((fOK=(read(hd,pid_b,i_end-i_beg) == (i_end-i_beg)))) {
	    // Calc end offset
	    voff_end = voff_beg;
	    vlen_end = vlen_beg;
	    for(int iPos = (vpos_beg+1); iPos <= vpos_end; iPos++) {
		int pk_vl = 0;
		for(int i_e = 0; i_e < vSize; i_e++)
		    pk_vl += pid_b[vSize*(iPos-vpos_beg)+i_e]<<(8*i_e);
		if(pk_vl) {
		    voff_end += vlen_end;
		    vlen_end = pk_vl;
		}
	    }
	    // Get value block
	    i_beg = voff_beg;
	    i_end = voff_end + vlen_end;
	    lseek(hd, i_beg, SEEK_SET);
	    val_b = (char*)malloc(i_end-i_beg);
	    fOK = (read(hd,val_b,i_end-i_beg) == (i_end-i_beg));
	}
    }

    //Free file resource and close file
    close(hd);
    mAcces = time(NULL);
    res.release();

    //Process value block
    int pid_off = vpos_beg;
    voff_end -= voff_beg;
    voff_beg = 0;
    while(fOK) {
	switch(type()) {
	    case TFld::Boolean:
		buf.setB((char)*(val_b+voff_beg), begin()+(int64_t)vpos_beg*period());	break;
	    case TFld::Int16: {
		int16_t vl = *(int16_t*)(val_b+voff_beg);
		buf.setI((vl==EVAL_INT16)?EVAL_INT:vl, begin()+(int64_t)vpos_beg*period());
		break;
	    }
	    case TFld::Int32: {
		int32_t vl = *(int32_t*)(val_b+voff_beg);
		buf.setI((vl==EVAL_INT32)?EVAL_INT:vl, begin()+(int64_t)vpos_beg*period());
		break;
	    }
	    case TFld::Int64: {
		int64_t vl = *(int64_t*)(val_b+voff_beg);
		buf.setI((vl==EVAL_INT64)?EVAL_INT:vl, begin()+(int64_t)vpos_beg*period());
		break;
	    }
	    case TFld::Float: {
		float vl = TSYS::floatLErev(*(float*)(val_b+voff_beg));
		buf.setR((vl<=EVAL_RFlt)?EVAL_REAL:vl, begin()+(int64_t)vpos_beg*period());
		break;
	    }
	    case TFld::Double: {
		double vl = TSYS::doubleLErev(*(double*)(val_b+voff_beg));
		buf.setR((vl<=EVAL_RDbl)?EVAL_REAL:vl, begin()+(int64_t)vpos_beg*period());
		break;
	    }
	    case TFld::String:
		buf.setS(string(val_b+voff_beg,vlen_beg), begin()+(int64_t)vpos_beg*period());	break;
	    default: break;
	}
	vpos_beg++;
	if(vpos_beg > vpos_end)	break;
	if(fixVl) voff_beg += vSize*(bool)(pid_b[(vpos_beg/8)-(pid_off/8)]&(0x01<<(vpos_beg%8)));
	else {
	    int pk_vl = 0;
	    for(int i_e = 0; i_e < vSize; i_e++) pk_vl += pid_b[vSize*(vpos_beg-pid_off)+i_e]<<(8*i_e);
	    if(pk_vl) {
		voff_beg += vlen_beg;
		vlen_beg = pk_vl;
	    }
	}
    }
    free(pid_b);
    free(val_b);
}

TVariant VFileArch::getVal( int vpos )
{
    ResAlloc res(mRes, false);
    if(mErr) throw owner().archivator().err_sys(_("Error archive file!"));

    mAcces = time(NULL);

    if(mPack) {
	res.request(true);
	try { if(mPack) mName = mod->unPackArch(mName); }
	catch(TError&) {
	    try {
		owner().archivator().checkArchivator(false, true);	// Try to remove some files by limits
		mName = mod->unPackArch(mName);
	    } catch(TError&) { mErr = true; throw; }
	}
	mPack = false;
	res.request(false);
    }

    //Open archive file
    int hd = open(name().c_str(), O_RDONLY);
    if(hd <= 0) { mErr = true; return EVAL_REAL; }

    res.request(true);
    switch(type()) {
	case TFld::Boolean: {
	    char rez = *(char*)getValue(hd, calcVlOff(hd,vpos), sizeof(char)).c_str();
	    close(hd);
	    return rez;
	}
	case TFld::Int16: {
	    int16_t rez = *(int16_t*)getValue(hd, calcVlOff(hd,vpos), sizeof(int16_t)).c_str();
	    close(hd);
	    return (rez==EVAL_INT16) ? (int64_t)EVAL_INT : rez;
	}
	case TFld::Int32: {
	    int32_t rez = *(int32_t*)getValue(hd, calcVlOff(hd,vpos), sizeof(int32_t)).c_str();
	    close(hd);
	    return (rez==EVAL_INT32) ? (int64_t)EVAL_INT : rez;
	}
	case TFld::Int64: {
	    int64_t rez = *(int64_t*)getValue(hd, calcVlOff(hd,vpos), sizeof(int64_t)).c_str();
	    close(hd);
	    return (rez==EVAL_INT64) ? (int64_t)EVAL_INT : rez;
	}
	case TFld::Float: {
	    float rez = TSYS::floatLErev(*(float*)getValue(hd, calcVlOff(hd,vpos), sizeof(float)).c_str());
	    close(hd);
	    return (rez<=EVAL_RFlt) ? EVAL_REAL : rez;
	}
	case TFld::Double: {
	    double rez = TSYS::doubleLErev(*(double*)getValue(hd, calcVlOff(hd,vpos), sizeof(double)).c_str());
	    close(hd);
	    return (rez<=EVAL_RDbl) ? EVAL_REAL : rez;
	}
	case TFld::String: {
	    int v_sz;
	    int v_off = calcVlOff(hd, vpos, &v_sz);
	    string rez = getValue(hd, v_off, v_sz);
	    close(hd);
	    return rez;
	}
	default: break;
    }

    return EVAL_REAL;
}

bool VFileArch::setVals( TValBuf &buf, int64_t ibeg, int64_t iend )
{
    int vpos_beg, vpos_end;
    string val_b, value, value_first, value_end = eVal;	//Set value

    ResAlloc res(mRes, false);
    if(mErr) return false;	//throw owner().archivator().err_sys(_("Error archive file!"));

    ibeg = vmax(ibeg, begin());
    iend = vmin(iend, end());
    if(ibeg > iend) return false;

    mAcces = time(NULL);

    if(mPack) {
	res.request(true);
	try { if(mPack) mName = mod->unPackArch(mName); }
	catch(TError&) {
	    try {
		owner().archivator().checkArchivator(false, true);	// Try to remove some files by limits
		mName = mod->unPackArch(mName);
	    } catch(TError&) { mErr = true; return false; /*throw;*/ }
	}
	mPack = false;
	res.request(false);
    }

    //Init pack index buffer
    vpos_beg = (ibeg-begin())/period();
    vpos_end = (iend-begin())/period();

    string pid_b(fixVl ? (vpos_end/8)-(vpos_beg/8)+1 : vSize*(vpos_end-vpos_beg+1), '\0');

    //Reserve memory for values buffer
    val_b.reserve(fixVl ? vSize*(vpos_end-vpos_beg+1)/2 : 10*(vpos_end-vpos_beg+1));

    //Get values, make value buffer and init the pack index table
    vpos_beg = 0;
    vpos_end = -1;
    double pRc = ((ModVArch&)owner().archivator()).roundProc();
    while(ibeg <= iend) {
	// Get value and put it to file
	switch(type()) {
	    case TFld::Boolean: {
		char tval = buf.getB(&ibeg, true);
		value.assign(&tval, vSize);
		break;
	    }
	    case TFld::Int16: case TFld::Int32: case TFld::Int64: {
		int64_t bval = buf.getI(&ibeg, true);
		switch(type()) {
		    case TFld::Int16: {
			int16_t vprev, tval = (bval==EVAL_INT) ? EVAL_INT16 : bval;
			if(pRc && vpos_end >= 0 && (vprev=*(int16_t*)value_end.c_str()) != EVAL_INT16 && tval != EVAL_INT16 &&
			    ((vprev > 0 && tval > 0) || (vprev < 0 && tval < 0)) &&
			    100*(double)abs(vprev-tval)/(double)vmax(abs(vprev),abs(tval)) <= pRc) tval = vprev;
			value.assign((char*)&tval, vSize);
			break;
		    }
		    case TFld::Int32: {
			int32_t vprev, tval = (bval==EVAL_INT) ? EVAL_INT32 : bval;
			if(pRc && vpos_end >= 0 && (vprev=*(int32_t*)value_end.c_str()) != EVAL_INT32 && tval != EVAL_INT32 &&
			    ((vprev > 0 && tval > 0) || (vprev < 0 && tval < 0)) &&
			    100*(double)abs(vprev-tval)/(double)vmax(abs(vprev),abs(tval)) <= pRc) tval = vprev;
			value.assign((char*)&tval, vSize);
			break;
		    }
		    case TFld::Int64: {
			int64_t vprev, tval = (bval==EVAL_INT) ? EVAL_INT64 : bval;
			if(pRc && vpos_end >= 0 && (vprev=*(int64_t*)value_end.c_str()) != EVAL_INT64 && tval != EVAL_INT64 &&
			    ((vprev > 0 && tval > 0) || (vprev < 0 && tval < 0)) &&
			    100*(double)abs(vprev-tval)/(double)vmax(abs(vprev),abs(tval)) <= pRc) tval = vprev;
			value.assign((char*)&tval, vSize);
			break;
		    }
		    default:	break;
		}
		break;
	    }
	    case TFld::Float: case TFld::Double: {
		double bval = buf.getR(&ibeg, true);
		switch(type()) {
		    case TFld::Float: {
			float vprev, tval = (bval==EVAL_REAL) ? EVAL_RFlt : bval;
			if(pRc && vpos_end >= 0 && (vprev=*(float*)value_end.c_str()) != EVAL_RFlt && tval != EVAL_RFlt &&
			    ((vprev > 0 && tval > 0) || (vprev < 0 && tval < 0)) &&
			    100*fabs(vprev-tval)/vmax(fabs(vprev),fabs(tval)) <= pRc) tval = vprev;
			tval = TSYS::floatLE(tval); value.assign((char*)&tval, vSize);
			break;
		    }
		    case TFld::Double: {
			double vprev, tval = (bval==EVAL_REAL) ? EVAL_RDbl : bval;
			if(pRc && vpos_end >= 0 && (vprev=*(double*)value_end.c_str()) != EVAL_RDbl && tval != EVAL_RDbl &&
			    ((vprev > 0 && tval > 0) || (vprev < 0 && tval < 0)) &&
			    100*fabs(vprev-tval)/vmax(fabs(vprev),fabs(tval)) <= pRc) tval = vprev;
			tval = TSYS::doubleLE(tval); value.assign((char*)&tval, vSize);
			break;
		    }
		    default:	break;
		}
		break;
	    }
	    case TFld::String: {
		value = buf.getS(&ibeg, true);
		if(!value.size()) value = " ";
		if((int)value.size() >= (1<<(vSize*8))) value.erase((1<<(vSize*8))-1);
		break;
	    }
	    default: break;
	}

	int pos_cur = (vmin(ibeg,iend)-begin())/period();		//Border added to end for prevent index buffer overlap
	if(vpos_end < 0) { vpos_beg = pos_cur; vpos_end = vpos_beg-1; }	//First call
	int pos_i = vpos_end;
	while(pos_i < pos_cur) {
	    pos_i++;
	    string wr_val = (pos_i==pos_cur) ? value : eVal;
	    if(vpos_end < vpos_beg || wr_val != value_end) {
		if(fixVl) {
		    pid_b[(pos_i/8)-(vpos_beg/8)] |= (0x01<<(pos_i%8));
		    val_b.append(wr_val);
		}
		else {
		    int v_sz = wr_val.size();
		    for(int v_psz = 0; v_psz < vSize; v_psz++)
			pid_b[vSize*(pos_i-vpos_beg)+v_psz] = *(((char*)&v_sz)+v_psz);
		    val_b.append(wr_val);
		}
		if(vpos_end < vpos_beg) value_first = wr_val;
		value_end = wr_val;
	    }
	    vpos_end = pos_i;
	}
	ibeg++;
    }

    res.request(true);
    //Open archive file
    int hd = open(name().c_str(), O_RDWR);
    if(hd <= 0) { mErr = true; return false; }
    bool fOK = true;

    //Get block geometry from file
    int foff_beg_len, foff_beg, foff_begprev_len, foff_begprev = 0, foff_end_len, foff_end, foff_endnext_len, foff_endnext = 0;
    if(vpos_beg) foff_begprev = calcVlOff(hd, vpos_beg-1, &foff_begprev_len, true);
    foff_beg = calcVlOff(hd, vpos_beg, &foff_beg_len, true);
    foff_end = calcVlOff(hd, vpos_end, &foff_end_len, true);
    if(vpos_beg < mpos) foff_endnext = calcVlOff(hd, vpos_end+1, &foff_endnext_len, true);

    //Checking and adaptation border
    if(fixVl) {
	// Check for begin border
	if(vpos_beg) {
	    if(getValue(hd,foff_begprev,vSize) == value_first) {
		pid_b[0] &= ~(0x01<<(vpos_beg%8));
		if(getPkVal(hd,vpos_beg))	foff_beg -= vSize;
	    }
	    else if(!getPkVal(hd,vpos_beg))	foff_beg += vSize;
	}
	// Check end border
	if(vpos_end < mpos) {
	    if(getValue(hd,foff_endnext,vSize) == value_end) {
		if(getPkVal(hd,vpos_end+1)) {
		    foff_end += 2*vSize;
		    setPkVal(hd, vpos_end+1, 0);
		}
		else foff_end += vSize;
	    }
	    else {
		if(getPkVal(hd,vpos_end+1)) foff_end += vSize;
		else setPkVal(hd,vpos_end+1, 1);
	    }
	}
	// Merge begin and end pack values
	char tmp_pb;
	lseek(hd, sizeof(FHead)+vpos_beg/8, SEEK_SET);
	fOK = fOK && (read(hd,&tmp_pb,1) == 1);
	pid_b[0] |= (~(0xFF<<(vpos_beg%8)))&tmp_pb;
	lseek(hd, sizeof(FHead)+vpos_end/8, SEEK_SET);
	fOK = fOK && (read(hd,&tmp_pb,1) == 1);
	pid_b[vpos_end/8-vpos_beg/8] |= (0xFE<<(vpos_end%8))&tmp_pb;	//0xFE mask by next value processing upper
    }
    else {
	// Check begin border
	if(vpos_beg) {
	    if(getValue(hd,foff_begprev,foff_begprev_len) == value_first) {
		for(int i_sz = 0; i_sz < vSize; i_sz++) pid_b[i_sz] = 0;
		if(getPkVal(hd,vpos_beg)) foff_beg -= foff_begprev_len;
	    }
	    else if(!getPkVal(hd,vpos_beg)) foff_beg += foff_begprev_len;
	}
	// Check end border
	if(vpos_end < mpos) {
	    if(getValue(hd,foff_endnext,foff_endnext_len) == value_end) {
		if(getPkVal(hd,vpos_end+1)) {
		    foff_end += foff_end_len+foff_endnext_len;
		    setPkVal(hd,vpos_end+1,0);
		}
		else foff_end += foff_end_len;
	    }
	    else {
		if(getPkVal(hd,vpos_end+1)) foff_end += foff_end_len;
		else setPkVal(hd,vpos_end+1,foff_endnext_len);
	    }
	}
    }
    //Write the values buffer before for writability and allowed store space detect
    moveTail(hd, foff_end, foff_end+(val_b.size()-(foff_end-foff_beg)));
    lseek(hd, foff_beg, SEEK_SET);
    fOK = fOK && (write(hd,val_b.data(),val_b.size()) == (int)val_b.size());

    //Write the pack id buffer
    int pid_b_sz;
    if(fixVl) {
	lseek(hd, sizeof(FHead)+vpos_beg/8, SEEK_SET);
	pid_b_sz = vpos_end/8 - vpos_beg/8 + 1;
    }
    else {
	lseek(hd, sizeof(FHead)+vSize*vpos_beg, SEEK_SET);
	pid_b_sz = vSize*(vpos_end-vpos_beg+1);
    }
    if((int)pid_b.size() < pid_b_sz)
	mod->mess_sys(TMess::Error, _("Warning! Allocated buffer size %d for indexes lesser for used %d."), pid_b.size(), pid_b_sz);
    fOK = fOK && (write(hd,pid_b.data(),pid_b_sz) == pid_b_sz);

    //Drop cache, before any owner().end() call by the cache using
    cacheDrop(vpos_beg);
    cacheSet(vpos_end, foff_beg+val_b.size()-value_end.size(), value_end.size(), true, true);

    //Check for write to end correct
    /*if(fOK && fixVl && iend > owner().end() && iend < end() && !((mSize-foff_end) == vSize ||
		((mSize-foff_end) == 0 && (int)val_b.size() >= vSize && val_b.compare(val_b.size()-vSize,vSize,eVal) == 0)))
	mof->mess_sys(TMess::Debug, _("Write data block to the archive file '%s' error. Will structure break. mSize=%d, foff_end=%d, vSize=%d"),name().c_str(),mSize,foff_end,vSize);*/

    mSize = lseek(hd, 0, SEEK_END);

    close(hd);

    if(!fOK) owner().archivator().mess_sys(TMess::Error, _("Error writing to the archive '%s' file '%s': %s(%d)"),
			owner().archive().id().c_str(), name().c_str(), strerror(errno), errno);

    return fOK;
}

string VFileArch::getValue( int hd, int voff, int vsz )
{
    bool fOK = true;
    string get_vl;
    get_vl.reserve(10);

    lseek(hd, voff, SEEK_SET);
    if((fOK=(read(hd,&tbt,1)==1))) {
	get_vl.assign((char *)&tbt,sizeof(char));
	for(int i_vs = 0; fOK && i_vs < (vsz-1); i_vs++)
	    if((fOK=(read(hd,&tbt,1)==1)))
		get_vl.append((char *)&tbt,sizeof(char));
    }

    if(!fOK) {
	mod->mess_sys(TMess::Error, _("Error reading the file '%s' for offset %d!"), name().c_str(), voff);
	if(!intoRep) repairFile(hd);
    }

    return get_vl;
}

int VFileArch::calcVlOff( int hd, int vpos, int *vsz, bool wr, int *rvpos )
{
    bool fOK = true;
    int bSz = 0, iBf = 0, vOff;
    char buf[4096];

    if(fixVl) {
	int cachPos = vpos, cachOff = cacheGet(cachPos);
	vOff = cachOff ? cachOff : sizeof(FHead)+mpos/8+(bool)(mpos%8);
	if(vsz) *vsz = vSize;

	if(cachPos == vpos) return vOff;
	cachPos++;
	int iPs = cachPos;

	for(int n_pos = 0; fOK && iPs <= vpos; iPs = n_pos) {
	    //Fast algorithm for big blocks
	    //if(!((iPs%8) || ((iPs/8)%4)) && (iPs/32) < (vpos/32))
	    if(!((iPs%8) || (iBf%4)) && (vpos/8 - iPs/8) >= 4) {
		//Buffer check for refresh
		if((iBf+4) > bSz) {
		    lseek(hd, sizeof(FHead)+iPs/8, SEEK_SET);
		    bSz = vmin((vpos/8)-(iPs/8)+1, (int)sizeof(buf));
		    fOK = (read(hd,&buf,bSz) == bSz);
		    iBf = 0;
		}

		//Count
		uint32_t vw = *(uint32_t*)(buf+iBf);	//TSYS::getUnalign32(buf+iBf);
		vw -= ((vw>>1)&0x55555555);
		vw = (vw&0x33333333) + ((vw>>2)&0x33333333);
		vOff += vSize * ((((vw+(vw>>4))&0xF0F0F0F)*0x1010101)>>24);
		n_pos = iPs + 32; iBf += 4;
		//Update cache
		if(iPs && (iPs%VAL_CACHE_POS) == 0) cacheSet(iPs+31, vOff, 0, false, wr);
	    }
	    //Simple algorithm
	    else {
		//Buffer check for refresh
		if(iBf >= bSz) {
		    lseek(hd, sizeof(FHead)+iPs/8, SEEK_SET);
		    bSz = vmin((vpos/8)-(iPs/8)+1, (int)sizeof(buf));
		    fOK = (read(hd,&buf,bSz) == bSz);
		    iBf = 0;
		}

		//Count
		vOff += vSize * (0x01&(buf[iBf]>>(iPs%8)));
		n_pos = iPs+1;
		if((n_pos%8) == 0) iBf++;
		//Update cache
		if(iPs == vpos) cacheSet(iPs, vOff, 0, iPs==vpos, wr);
	    }
	}
    }
    else {
	int cachPos = vpos, lstPkPos = 0, lstPkVl, cachOff = cacheGet(cachPos, &lstPkVl);
	if(cachOff)	{ vOff = cachOff; cachPos++; }
	else vOff = sizeof(FHead) + mpos*vSize;
	lseek(hd, sizeof(FHead)+cachPos*vSize, SEEK_SET);

	if(mess_lev() == TMess::Debug) owner().archive().mess_sys(TMess::Debug, "Cache (start) pos %d(%d,%d) = %d", vpos, mpos, cachPos, vOff);

	for(int iPs = cachPos; fOK && iPs <= vmin(vpos,mpos-1); iPs++) {
	    int pkVl = 0;
	    for(int i_e = 0; i_e < vSize; ++i_e) {
		if(++iBf >= bSz) {
		    bSz = vmin(vSize*(vpos-iPs+1), (int)sizeof(buf));
		    fOK = (read(hd,&buf,bSz) == bSz);
		    iBf = 0;
		}
		pkVl += buf[iBf]<<(8*i_e);
	    }
	    if(pkVl) {
		if(iPs) vOff += lstPkVl;
		lstPkPos = iPs;
		lstPkVl = pkVl;
	    }
	    //Update cache
	    if(((iPs-cachPos) && !((iPs-cachPos)%VAL_CACHE_POS)) || iPs == vpos)
		cacheSet(iPs, vOff, lstPkVl, iPs==vpos, wr);
	}
	if(rvpos) *rvpos = lstPkPos;
	if(vsz) *vsz = lstPkVl;

	if(mess_lev() == TMess::Debug)
	    owner().archive().mess_sys(TMess::Debug, "Cache pos %d(%d,%d) = %d(%d)", vpos, lstPkPos, cachPos, vOff, lstPkVl);
    }

    return vOff;
}

void VFileArch::setValue( int hd, int voff, const string &val )
{
    lseek(hd, voff, SEEK_SET);
    if(write(hd,val.c_str(),val.size()) != (int)val.size())
	mod->mess_sys(TMess::Error, _("Error writing to the file '%s'!"), name().c_str());
}

void VFileArch::moveTail( int hd, int old_st, int new_st )
{
    bool fOK = true;
    char buf[prmStrBuf_SZ];

    if(old_st == new_st) return;
    if(new_st > old_st && fOK) {
	//Move down (insert)
	int beg_cur;
	int mv_beg = old_st;
	int mv_end = lseek(hd,0,SEEK_END);
	if(mv_end <= mv_beg)	return;
	do {
	    beg_cur = ((mv_end-mv_beg) >= (int)sizeof(buf)) ? mv_end-sizeof(buf) : mv_beg;
	    lseek(hd,beg_cur,SEEK_SET);
	    fOK = fOK && (read(hd,buf,mv_end-beg_cur) == (mv_end-beg_cur));
	    lseek(hd, beg_cur+(new_st-old_st), SEEK_SET);
	    fOK = fOK && (write(hd,buf,mv_end-beg_cur) == (mv_end-beg_cur));
	    mv_end -= sizeof(buf);
	}
	while(beg_cur != mv_beg && fOK);
    }
    else {
	//Move up (erase)
	int end_cur;
	int mv_beg = old_st;
	int mv_end = lseek(hd, 0, SEEK_END);
	if(mv_end > mv_beg && fOK) {
	    do {
		end_cur = ((mv_end-mv_beg) >= (int)sizeof(buf)) ? mv_beg+sizeof(buf) : mv_end;
		lseek(hd, mv_beg, SEEK_SET);
		fOK = fOK && (read(hd,buf,end_cur-mv_beg) == (end_cur-mv_beg));
		lseek(hd, mv_beg-(old_st-new_st), SEEK_SET);
		fOK = fOK && (write(hd,buf,end_cur-mv_beg) == (end_cur-mv_beg));
		mv_beg += sizeof(buf);
	    }
	    while(end_cur != mv_end && fOK);
	}

	//Truncate tail
	fOK = fOK && (ftruncate(hd,mv_end-(old_st-new_st)) == 0);
    }
}

int VFileArch::getPkVal( int hd, int vpos )
{
    bool fOK = true;
    int pk_val = 0;

    if(fixVl) {
	lseek(hd, sizeof(FHead)+vpos/8, SEEK_SET);
	fOK = (read(hd,&tbt,1) == 1);
	pk_val = ((0x01<<(vpos%8))&tbt)?1:0;
    }
    else {
	lseek(hd, sizeof(FHead)+vpos*vSize, SEEK_SET);
	for(int v_psz = 0; fOK && v_psz < vSize; v_psz++) {
	    fOK = (read(hd,&tbt,1) == 1);
	    pk_val += tbt<<(v_psz*8);
	}
    }

    return pk_val;
}

void VFileArch::setPkVal( int hd, int vpos, int vl )
{
    bool fOK = true;
    if(fixVl) {
	lseek(hd, sizeof(FHead)+vpos/8, SEEK_SET);
	fOK = (read(hd,&tbt,1) == 1);
	tbt = vl ? ((0x01<<(vpos%8))|tbt) : (~(0x01<<(vpos%8))&tbt);
	lseek(hd, -1, SEEK_CUR);
	fOK = fOK && (write(hd,&tbt,1) == 1);
    }
    else {
	lseek(hd, sizeof(FHead)+vpos*vSize, SEEK_SET);
	for(int v_psz = 0; fOK && v_psz < vSize; v_psz++)
	    fOK = (write(hd, ((char *)&vl)+v_psz, 1) == 1);
    }
}

void VFileArch::repairFile( int hd )
{
    if(mPack)	return;

    intoRep = true;

    cacheDrop(0);

    bool fOK = true;
    int vSz, lstMpos,
	fSz = lseek(hd, 0, SEEK_END),
	fOff = calcVlOff(hd, mpos, &vSz, false, &lstMpos),
	dt = fSz - fOff - vSz;
    string errsDir = owner().archivator().addr()+"/errors";

    //Check for the structure size
    if(dt) {
	owner().archivator().mess_sys(TMess::Error, _("Error archive file structure: '%s'. Margin = %d byte. Will try fix that!"), name().c_str(), dt);

	// Check to directory for errors present and accessible to write
	struct stat dStat;
	stat(errsDir.c_str(), &dStat);
	if((dStat.st_mode&S_IFMT) == S_IFDIR && access(errsDir.c_str(),F_OK|W_OK|X_OK) == 0) {
	    //  Copy the error file for observing next
	    char cpBuf[4096];
	    int ehd = open((errsDir+name().substr(name().rfind("/"))).c_str(), O_WRONLY|O_CREAT|O_TRUNC, SYS->permCrtFiles());
	    if(ehd < 0) owner().archivator().mess_sys(TMess::Error, _("Error openning/creating an archive file for the copy here: %s"), strerror(errno));
	    else {
		lseek(hd, 0, SEEK_SET);
		for(int rs = 0; fOK && (rs=read(hd, cpBuf, sizeof(cpBuf))) > 0; )
		    fOK = (write(ehd,cpBuf,rs) == rs);
		close(ehd);
	    }
	}

	// Fix file
	if(fOK) {
	    if(dt > 0) {
		mSize = fOff + vSz;
		fOK = (ftruncate(hd,mSize) == 0);
	    }
	    else {
		mSize = fOff + vSz;
		if(fixVl) {
		    int fillBlk = vSz*((mSize-fSz)/vSz + (bool)((mSize-fSz)%vSz));
		    for(int fOff = mSize-fillBlk; fOff < mSize; fOff += eVal.size()) setValue(hd, fOff, eVal);
		}
		else {
		    string fillBlk(mSize-fSz, 'R');
		    fOK = (write(hd,fillBlk.data(),fillBlk.size()) == (int)fillBlk.size());
		}
	    }

	    cacheDrop(0);

	    fOff = calcVlOff(hd, mpos, &vSz, false, &lstMpos);	//Update offset to end
	}
    }

    //Check to end value EVAL for actual archive file
    if(fOK && end() > TSYS::curTime() && getValue(hd,fOff,vSz) != eVal) {
	owner().archivator().mess_sys(TMess::Error, _("Last value for the actual archive file '%s' is not EVAL. Will try fix that!"), name().c_str());

	mSize = fOff + eVal.size();
	if((fOK=(ftruncate(hd,mSize)==0))) {
	    setValue(hd, fOff, eVal);
	    if(!fixVl) setValue(hd, sizeof(FHead)+lstMpos, string(1,eVal.size()));
	}
	cacheDrop(0);
    }

    intoRep = false;
}

int VFileArch::cacheGet( int &pos, int *vsz )
{
    CacheEl rez = {0, 0, 0};

    dtRes.lock();
    for(int iP = (int)cache.size()-1; iP >= 0; iP--)
	if(pos >= cache[iP].pos) { rez = cache[iP]; break; }

    if(pos >= cach_pr_rd.pos && cach_pr_rd.pos > rez.pos) rez = cach_pr_rd;
    if(pos >= cach_pr_wr.pos && cach_pr_wr.pos > rez.pos) rez = cach_pr_wr;
    dtRes.unlock();

    pos = rez.pos;
    if(vsz) *vsz = rez.vsz;
    return rez.off;
}

void VFileArch::cacheSet( int pos, int off, int vsz, bool last, bool wr  )
{
    CacheEl el = { pos, off, vsz };

    MtxAlloc res(dtRes, true);
    if(!last) {
	for(unsigned iP = 0; iP < cache.size(); iP++)
	    if(el.pos == cache[iP].pos)	{ cache[iP] = el; return; }
	    else if(el.pos < cache[iP].pos)	{ cache.insert(cache.begin()+iP,el); return; }
	cache.push_back(el);
    }
    else if(wr) cach_pr_wr = el;
    else cach_pr_rd = el;
}

void VFileArch::cacheDrop( int pos )
{
    dtRes.lock();
    for(unsigned iP = 0; iP < cache.size(); ) {
	if(cache[iP].pos >= pos) { cache.erase(cache.begin()+iP); continue; }
	iP++;
    }
    if(cach_pr_rd.pos >= pos)	cach_pr_rd.off = cach_pr_rd.pos = cach_pr_rd.vsz = 0;
    if(cach_pr_wr.pos >= pos)	cach_pr_wr.off = cach_pr_wr.pos = cach_pr_wr.vsz = 0;
    dtRes.unlock();
}
