
//OpenSCADA system module Archive.FSArch file: val.cpp
/***************************************************************************
 *   Copyright (C) 2003-2014 by Roman Savochenko                           *
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

#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <dirent.h>
#include <math.h>
#include <string.h>

#include <tsys.h>
#include <tmess.h>
#include "base.h"
#include "val.h"

using namespace FSArch;

//*************************************************
//* FSArch::ModVArch - Value archivator           *
//*************************************************
ModVArch::ModVArch( const string &iid, const string &idb, TElem *cf_el ) :
    TVArchivator(iid,idb,cf_el), chkANow(false),
    time_size(800), mNumbFiles(100), mMaxCapacity(0), round_proc(0.01), mChkTm(60), mPackTm(10), mPackInfoFiles(false), mLstCheck(0)
{

}

ModVArch::~ModVArch( )
{
    try{ stop(); }catch(...){}
}

double ModVArch::curCapacity( )
{
    double fsz = 0;
    ResAlloc res(archRes,false);
    for(map<string,TVArchEl*>::iterator iel = archEl.begin(); iel != archEl.end(); ++iel)
	fsz += ((ModVArchEl *)iel->second)->size();
    return fsz;
}

void ModVArch::setValPeriod( double iper )
{
    TVArchivator::setValPeriod(iper);

    time_size = vmax(0.2, 1e3*valPeriod());
}

void ModVArch::load_( )
{
    TVArchivator::load_();

    try
    {
	XMLNode prmNd;
	string  vl;
	prmNd.load(cfg("A_PRMS").getS());
	vl = prmNd.attr("TmSize");	if(!vl.empty()) setFileTimeSize(atof(vl.c_str()));
	vl = prmNd.attr("NFiles");	if(!vl.empty()) setNumbFiles(atoi(vl.c_str()));
	vl = prmNd.attr("MaxCapacity"); if(!vl.empty()) setMaxCapacity(atof(vl.c_str()));
	vl = prmNd.attr("Round"); 	if(!vl.empty()) setRoundProc(atof(vl.c_str()));
	vl = prmNd.attr("PackTm");	if(!vl.empty()) setPackTm(atoi(vl.c_str()));
	vl = prmNd.attr("CheckTm");	if(!vl.empty()) setCheckTm(atoi(vl.c_str()));
	vl = prmNd.attr("PackInfoFiles"); if(!vl.empty()) setPackInfoFiles(atoi(vl.c_str()));
    } catch(...){ }

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

void ModVArch::start( )
{
    //> Start getting data cycle
    TVArchivator::start();

    //> First scan dir. Load and connect archive files
    try{ checkArchivator(true); } catch( TError err ) { stop( ); throw; }
}

void ModVArch::stop( )
{
    //> Stop getting data cicle an detach archives
    TVArchivator::stop();
}

bool ModVArch::filePrmGet( const string &anm, string *archive, TFld::Type *vtp, int64_t *abeg, int64_t *aend, int64_t *aper )
{
    char buf[21]; buf[20] = 0;
    bool unpck = false;
    string a_fnm = anm;
    if(mod->filePack(anm))
    {
	bool infoOK = false;
	//>> Get archive info from info file
	int hd = open((anm+".info").c_str(), O_RDONLY);
	if(hd >= 0)
	{
	    char ibuf[80];
	    int rsz = read(hd,ibuf,sizeof(ibuf));
	    if(rsz > 0 && rsz < (int)sizeof(ibuf))
	    {
		ibuf[rsz] = 0;
		int64_t tBeg, tEnd, tPer;
		int tVTp;
		if(sscanf(ibuf,"%llx %llx %20s %llx %d",&tBeg,&tEnd,buf,&tPer,&tVTp) == 5)
		{
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

	//> Get file info from DB
	if(!infoOK)
	{
	    TConfig c_el(&mod->packFE());
	    c_el.cfg("FILE").setS(anm);
	    if(SYS->db().at().dataGet(mod->filesDB(),mod->nodePath()+"Pack/",c_el))
	    {
		if(abeg)	*abeg = strtoll(c_el.cfg("BEGIN").getS().c_str(),NULL,16);
		if(aend)	*aend = strtoll(c_el.cfg("END").getS().c_str(),NULL,16);
		if(archive)	*archive = c_el.cfg("PRM1").getS();
		if(aper)	*aper = strtoll(c_el.cfg("PRM2").getS().c_str(),NULL,16);
		if(vtp)		*vtp  = (TFld::Type)atoi(c_el.cfg("PRM3").getS().c_str());
		infoOK = true;
	    }
	}

	if(infoOK) return true;

	try
	{
	    mess_info(nodePath().c_str(), _("Unpack '%s' for information get."),anm.c_str());
	    a_fnm = mod->unPackArch(anm,false);
	}
	catch(TError){ return false; }
	unpck = true;
    }
    //> Get params from file
    int hd = open(a_fnm.c_str(), O_RDONLY);
    if(hd <= 0)	return false;
    //>> Read Header
    VFileArch::FHead head;
    int r_len = read(hd, &head, sizeof(VFileArch::FHead));
    close(hd);
    if(r_len < (int)sizeof(VFileArch::FHead) || VFileArch::afl_id != head.f_tp || head.term != 0x55) return false;
    //>> Check to archive present
    if(archive)	{ strncpy(buf,head.archive,20); *archive = buf; }
    if(abeg)	*abeg = head.beg;
    if(aend)	*aend = head.end;
    if(aper)	*aper = head.period;
    if(vtp)	*vtp  = (TFld::Type)(head.vtp|(head.vtpExt<<4));

    if(unpck)
    {
	remove(a_fnm.c_str());

	if(!packInfoFiles())
	{
	    //>> Write info to DB
	    TConfig c_el(&mod->packFE());
	    c_el.cfg("FILE").setS(anm);
	    c_el.cfg("BEGIN").setS(ll2s(head.beg,TSYS::Hex));
	    c_el.cfg("END").setS(ll2s(head.end,TSYS::Hex));
	    strncpy(buf, head.archive, 20);
	    c_el.cfg("PRM1").setS(buf);
	    c_el.cfg("PRM2").setS(ll2s(head.period,TSYS::Hex));
	    c_el.cfg("PRM3").setS(i2s(head.vtp|(head.vtpExt<<4)));
	    SYS->db().at().dataSet(mod->filesDB(), mod->nodePath()+"Pack/", c_el);
	}
	else if((hd=open((anm+".info").c_str(),O_WRONLY|O_CREAT|O_TRUNC,0666)) > 0)
	{
	    //>> Write info to info file
	    string si = TSYS::strMess("%llx %llx %s %llx %d", head.beg, head.end, buf, head.period, head.vtp|(head.vtpExt<<4));
	    bool fOK = (write(hd,si.data(),si.size()) == (int)si.size());
	    close(hd);
	    if(!fOK) return false;
	}
    }

    return true;
}

void ModVArch::checkArchivator( bool now )
{
    if(!startStat())	return;

    chkANow = true;

    bool isTm = time(NULL) > (mLstCheck+checkTm()*60);
    if(now || isTm)
    {
	//> Find archive files for no present archives and create it.
	struct stat file_stat;
	dirent scan_dirent, *scan_rez = NULL;

	//>> Open/create new directory
	DIR *IdDir = opendir(addr().c_str());
	if(IdDir == NULL)
	{
	    if(mkdir(addr().c_str(),0777)) throw TError(nodePath().c_str(), _("Can not create directory '%s'."), addr().c_str());
	    IdDir = opendir(addr().c_str());
	}

	//>> Scan opened directory
	while(readdir_r(IdDir,&scan_dirent,&scan_rez) == 0 && scan_rez)
	{
	    if(strcmp(scan_rez->d_name,"..") == 0 || strcmp(scan_rez->d_name,".") == 0) continue;

	    string	ArhNm;
	    TFld::Type	ArhTp;
	    string NameArhFile = addr()+"/"+scan_rez->d_name;

	    stat(NameArhFile.c_str(),&file_stat);
	    if((file_stat.st_mode&S_IFMT) != S_IFREG || access(NameArhFile.c_str(),F_OK|R_OK) != 0) continue;

	    //>> Check for info files
	    if(NameArhFile.compare(NameArhFile.size()-4,4,".val") != 0 && NameArhFile.compare(NameArhFile.size()-7,7,".val.gz") != 0) continue;
	    if(!filePrmGet(NameArhFile,&ArhNm,&ArhTp,NULL,NULL,NULL))	continue;

	    //>>> Check to archive present
	    AutoHD<TVArchive> varch;
	    if(owner().owner().valPresent(ArhNm)) varch = owner().owner().valAt(ArhNm);
	    else
	    {
		//>>>> Add no present archive
		owner().owner().valAdd(ArhNm);
		varch = owner().owner().valAt(ArhNm);
		varch.at().setToStart(true);
		varch.at().setValType(ArhTp);
		varch.at().start();
	    }
	    //>>> Check for attached
	    if(!varch.at().archivatorPresent(workId()))	varch.at().archivatorAttach(workId());
	    //>>> Try connect new file
	    ResAlloc res(archRes, false);
	    map<string,TVArchEl*>::iterator iel = archEl.find(ArhNm);
	    if(iel != archEl.end()) ((ModVArchEl *)iel->second)->fileAdd(NameArhFile);
	}

	closedir(IdDir);
	now = true;
    }

    //>> Scan files of attached archives
    ResAlloc res(archRes, false);
    for(map<string,TVArchEl*>::iterator iel = archEl.begin(); iel != archEl.end(); ++iel)
	((ModVArchEl *)iel->second)->checkArchivator(now, maxCapacity() && (curCapacity()/1048576) > maxCapacity());

    chkANow = false;
    if(isTm)	mLstCheck = time(NULL);
}

void ModVArch::expArch(const string &arch_nm, time_t beg, time_t end, const string &file_tp, const string &file_nm)
{
    int64_t buf_sz = 100000;
    int64_t buf_per = (int64_t)(valPeriod()*1e6);
    int64_t c_tm;

    TValBuf buf( TFld::Real, buf_sz, buf_per, true, true );
    beg = vmax(beg, SYS->archive().at().valAt(arch_nm).at().begin(workId())/1000000);
    end = vmin(end, SYS->archive().at().valAt(arch_nm).at().end(workId())/1000000);

    if(file_tp == "wav")
    {
	//> Export to wav
	struct
	{
	    char riff[4];
	    int  filesize;
	    char rifftype[4];
	} rif;
	struct
	{
	    char chunk_id[4];
	    int  chunksize;
	} chnk;
	struct
	{
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
	wv_form.wBitsPerSample=32;

	int hd = open((file_nm+"."+file_tp).c_str(), O_RDWR|O_CREAT|O_TRUNC, 0666);
	if(hd == -1) return;
	bool fOK = true;

	fOK = fOK && (write(hd,&rif,sizeof(rif)) == sizeof(rif));
	fOK = fOK && (write(hd,&chnk,sizeof(chnk)) == sizeof(chnk));
	fOK = fOK && (write(hd,&wv_form,sizeof(wv_form)) == sizeof(wv_form));
	strncpy(chnk.chunk_id, "data", 4);
	chnk.chunksize = 0;				//Set temporary size buf.realSize()*sizeof(float);
	off_t sz_pos = lseek(hd, 0, SEEK_CUR);
	fOK = fOK && (write(hd,&chnk,sizeof(chnk)) == sizeof(chnk));

	//> Calc overage and scale of value
	float c_val, v_over = 0, v_max = -1e30, v_min = 1e30;

	c_tm = (int64_t)beg*1000000;
	while(c_tm < (int64_t)end*1000000)
	{
	    int64_t end_tm = c_tm+buf_sz*buf_per;
	    end_tm = vmin(end_tm, (int64_t)end*1000000);
	    SYS->archive().at().valAt(arch_nm).at().getVals(buf, c_tm, end_tm, workId());

	    //>> Check scale
	    for( ; c_tm <= buf.end(); c_tm += buf_per)
	    {
		c_val = buf.getR(&c_tm, true);
		if(c_val == EVAL_REAL)	continue;
		v_max = vmax(c_val, v_max);
		v_min = vmin(c_val, v_min);
	    }
	    v_over = (v_max+v_min)/2;
	}
	buf.clear();

	//> Transver value
	int val_cnt = 0;
	c_tm = (int64_t)beg*1000000;
	while(fOK && c_tm && c_tm < (int64_t)end*1000000)
	{
	    int64_t end_tm = c_tm+buf_sz*buf_per-buf_per;
	    end_tm = vmin(end_tm, (int64_t)end*1000000);
	    SYS->archive().at().valAt(arch_nm).at().getVals(buf, c_tm, end_tm, workId());

	    for( ; fOK && c_tm <= buf.end(); c_tm += buf_per, val_cnt++)
	    {
		c_val = buf.getR(&c_tm, true);
		if(!c_tm) break;
		if(c_val == EVAL_REAL)	c_val = v_over;
		c_val = 2.*(c_val-v_over)/(v_max-v_min);
		fOK = fOK && (write(hd,&c_val,sizeof(float)) == sizeof(float));
	    }
	}

	//> Write value count
	lseek(hd, sz_pos, SEEK_SET);
	chnk.chunksize = val_cnt*sizeof(float);
	fOK = fOK && (write(hd,&chnk,sizeof(chnk)) == sizeof(chnk));

	close(hd);
    }
    else
    {
	char c_val[40];
	int hd = open((file_nm+"."+file_tp).c_str(), O_RDWR|O_CREAT|O_TRUNC, 0666);
	if(hd == -1) return;
	bool fOK = true;

	c_tm = (int64_t)beg*1000000;
	while(fOK && c_tm && c_tm < (int64_t)end*1000000)
	{
	    int64_t end_tm = c_tm + buf_sz*buf_per - buf_per;
	    end_tm = vmin(end_tm, (int64_t)end*1000000);
	    SYS->archive().at().valAt(arch_nm).at().getVals(buf, c_tm, end_tm, workId());

	    for( ; fOK && c_tm <= buf.end(); c_tm += buf_per)
	    {
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
    if(!owner().owner().SubStarting) v_el->checkArchivator(true);

    return v_el;
}

void ModVArch::cntrCmdProc( XMLNode *opt )
{
    //> Get page info
    if(opt->name() == "info")
    {
	TVArchivator::cntrCmdProc(opt);
	ctrMkNode("fld",opt,-1,"/prm/st/fsz",_("Full archives size"),R_R_R_,"root",SARH_ID,1,"tp","str");
	ctrMkNode("fld",opt,-1,"/prm/cfg/ADDR",EVAL_STR,startStat()?R_R_R_:RWRWR_,"root",SARH_ID,3,
	    "dest","sel_ed","select","/prm/cfg/dirList","help",_("Path to directory for archivator's of values files."));
	if(ctrMkNode("area",opt,-1,"/prm/add",_("Additional options"),R_R_R_,"root",SARH_ID))
	{
	    ctrMkNode("fld",opt,-1,"/prm/add/tm",_("Archive's file time size (hours)"),RWRWR_,"root",SARH_ID,2,"tp","real","help",
		_("The parameter is set automatically when you change the frequency values by archiver\n"
		  "and generally proportional to the frequency values of the archiver.\n"
		  "Attention! Large archive files will be processed long by long unpacking gzip-files\n"
		  "and the primary indexing, when accessing to parts of deep in the archives of history."));
	    ctrMkNode("fld",opt,-1,"/prm/add/fn",_("Maximum files number for one archive"),RWRWR_,"root",SARH_ID,2,"tp","dec","help",
		_("Limits the maximum number of archive files and share with the size of single file\n"
		  "determines the size of archive on disk. Completely remove this restriction can be set to zero."));
	    ctrMkNode("fld",opt,-1,"/prm/add/maxCpct",_("Maximum capacity by all archives (MB)"),RWRWR_,"root",SARH_ID,2,"tp","real","help",
		_("Sets a limit on the maximum amount of disk space occupied by all archive files by archiver.\n"
		  "Testing is done by periodicity checking the archives, which resulted in, on exceeding the limit,\n"
		  "removes the oldest files of all archives. Completely remove this restriction can be set to zero."));
	    ctrMkNode("fld",opt,-1,"/prm/add/round",_("Numeric values rounding (%)"),RWRWR_,"root",SARH_ID,2,"tp","real","help",
		_("Sets the percentage of boundary difference values of parameters integer and real types\n"
		  "where they are considered identical and will be archived as a single value\n"
		  "through sequential packaging. Allows well-packaged slightly changing parameters\n"
		  "which outside certainty. Disable this property can be set to zero."));
	    ctrMkNode("fld",opt,-1,"/prm/add/pcktm",_("Pack files timeout (min)"),RWRWR_,"root",SARH_ID,2,"tp","dec","help",
		_("Sets the time after which, in the absence of appeals, archive files will be packaged in gzip archiver.\n"
		 "Set to zero for disable packing by gzip."));
	    ctrMkNode("fld",opt,-1,"/prm/add/tmout",_("Check archives period (min)"),RWRWR_,"root",SARH_ID,2,"tp","dec","help",
		_("Sets the frequency of checking the archives for the emergence or delete new files\n"
		  "in a directory of archives, as well as exceeding the limits and removing old archive files."));
	    ctrMkNode("fld",opt,-1,"/prm/add/pack_info_fl",_("Use info files for packed archives"),RWRWR_,"root",SARH_ID,2,"tp","bool","help",
		_("Specifies whether to create files with information about the packed archive files by gzip-archiver.\n"
		  "When copying files of archive on another station, these info files can speed up the target station\n"
		  "process of first run by eliminating the need to decompress by gzip-archives in order to obtain information."));
	    ctrMkNode("comm",opt,-1,"/prm/add/chk_nw",_("Check archivator directory now"),RWRW__,"root",SARH_ID,1,"help",
		_("The command, which allows you to immediately start checking the archives,\n"
		  "for example, after manual changes to the directory archiver."));
	}
	ctrMkNode("list",opt,-1,"/arch/arch/3",_("Files size"),R_R_R_,"root",SARH_ID,1,"tp","str");
	if(ctrMkNode("comm",opt,-1,"/arch/exp",_("Export"),RWRW__,"root",SARH_ID))
	{
	    ctrMkNode("fld",opt,-1,"/arch/exp/arch",_("Archive"),RWRW__,"root",SARH_ID,3,"tp","str","dest","select","select","/arch/lst");
	    ctrMkNode("fld",opt,-1,"/arch/exp/beg",_("Begin"),RWRW__,"root",SARH_ID,1,"tp","time");
	    ctrMkNode("fld",opt,-1,"/arch/exp/end",_("End"),RWRW__,"root",SARH_ID,1,"tp","time");
	    ctrMkNode("fld",opt,-1,"/arch/exp/tfl",_("Type"),RWRW__,"root",SARH_ID,3,"tp","str","dest","select","select","/arch/tpflst");
	    ctrMkNode("fld",opt,-1,"/arch/exp/file",_("To file"),RWRW__,"root",SARH_ID,1,"tp","str");
	}
	return;
    }
    //> Process command to page
    string a_path = opt->attr("path");
    if(a_path == "/prm/cfg/dirList" && ctrChkNode(opt))	TSYS::ctrListFS(opt, addr());
    else if(a_path == "/prm/st/fsz" && ctrChkNode(opt))	opt->setText(TSYS::cpct2str(curCapacity()));
    else if(a_path == "/prm/add/tm")
    {
	if(ctrChkNode(opt,"get",RWRWR_,"root",SARH_ID,SEC_RD))	opt->setText(r2s(fileTimeSize(),6));
	if(ctrChkNode(opt,"set",RWRWR_,"root",SARH_ID,SEC_WR))	setFileTimeSize(atof(opt->text().c_str()));
    }
    else if(a_path == "/prm/add/fn")
    {
	if(ctrChkNode(opt,"get",RWRWR_,"root",SARH_ID,SEC_RD))	opt->setText(i2s(numbFiles()));
	if(ctrChkNode(opt,"set",RWRWR_,"root",SARH_ID,SEC_WR))	setNumbFiles(atoi(opt->text().c_str()));
    }
    else if(a_path == "/prm/add/maxCpct")
    {
	if(ctrChkNode(opt,"get",RWRWR_,"root",SARH_ID,SEC_RD))	opt->setText(r2s(maxCapacity(),6));
	if(ctrChkNode(opt,"set",RWRWR_,"root",SARH_ID,SEC_WR))	setMaxCapacity(atof(opt->text().c_str()));
    }
    else if(a_path == "/prm/add/round")
    {
	if(ctrChkNode(opt,"get",RWRWR_,"root",SARH_ID,SEC_RD))	opt->setText(r2s(roundProc(),6));
	if(ctrChkNode(opt,"set",RWRWR_,"root",SARH_ID,SEC_WR))	setRoundProc(atof(opt->text().c_str()));
    }
    else if(a_path == "/prm/add/pcktm")
    {
	if(ctrChkNode(opt,"get",RWRWR_,"root",SARH_ID,SEC_RD))	opt->setText(i2s(packTm()));
	if(ctrChkNode(opt,"set",RWRWR_,"root",SARH_ID,SEC_WR))	setPackTm(atoi(opt->text().c_str()));
    }
    else if(a_path == "/prm/add/tmout")
    {
	if(ctrChkNode(opt,"get",RWRWR_,"root",SARH_ID,SEC_RD))	opt->setText(i2s(checkTm()));
	if(ctrChkNode(opt,"set",RWRWR_,"root",SARH_ID,SEC_WR))	setCheckTm(atoi(opt->text().c_str()));
    }
    else if(a_path == "/prm/add/pack_info_fl")
    {
	if(ctrChkNode(opt,"get",RWRWR_,"root",SARH_ID,SEC_RD))	opt->setText(i2s(packInfoFiles()));
	if(ctrChkNode(opt,"set",RWRWR_,"root",SARH_ID,SEC_WR))	setPackInfoFiles(atoi(opt->text().c_str()));
    }
    else if(a_path == "/prm/add/chk_nw" && ctrChkNode(opt,"set",RWRW__,"root",SARH_ID,SEC_WR))	checkArchivator(true);
    else if(a_path == "/arch/arch" && ctrChkNode(opt))
    {
	//>> Fill Archives table
	XMLNode *n_arch = ctrMkNode("list",opt,-1,"/arch/arch/0","");
	XMLNode *n_per  = ctrMkNode("list",opt,-1,"/arch/arch/1","");
	XMLNode *n_size = ctrMkNode("list",opt,-1,"/arch/arch/2","");
	XMLNode *f_size = ctrMkNode("list",opt,-1,"/arch/arch/3","");

	ResAlloc res(archRes,false);
	for(map<string,TVArchEl*>::iterator iel = archEl.begin(); iel != archEl.end(); ++iel)
	{
	    if(n_arch)	n_arch->childAdd("el")->setText(iel->second->archive().id());
	    if(n_per)	n_per->childAdd("el")->setText(r2s((double)iel->second->archive().period()/1e6,6));
	    if(n_size)	n_size->childAdd("el")->setText(i2s(iel->second->archive().size()));
	    if(f_size)	f_size->childAdd("el")->setText(TSYS::cpct2str((double)((ModVArchEl *)iel->second)->size()));
	}
    }
    else if(a_path == "/arch/lst" && ctrChkNode(opt))
    {
	vector<string> a_ls;
	archiveList(a_ls);
	for(unsigned i_el = 0; i_el < a_ls.size(); i_el++)
	    opt->childAdd("el")->setText(a_ls[i_el]);
    }
    else if(a_path == "/arch/tpflst" && ctrChkNode(opt))
    {
	opt->childAdd("el")->setText("ascii");
	opt->childAdd("el")->setText("wav");
    }
    else if(a_path == "/arch/exp" && ctrChkNode(opt,"set",RWRW__,"root",SARH_ID,SEC_WR))
	expArch(ctrId(opt,"arch")->text(),
		atoi(ctrId(opt,"beg")->text().c_str()),
		atoi(ctrId(opt,"end")->text().c_str()),
		ctrId(opt,"tfl")->text(),
		ctrId(opt,"file")->text());
    else TVArchivator::cntrCmdProc(opt);
}

//*************************************************
//* FSArch::ModVArchEl - Value archive element    *
//*************************************************
ModVArchEl::ModVArchEl( TVArchive &iachive, TVArchivator &iarchivator ) :
    TVArchEl(iachive,iarchivator), realEnd(0)
{

}

ModVArchEl::~ModVArchEl( )
{
    //> Clear a files list
    ResAlloc res(mRes,true);
    while(arh_f.size())
    {
	delete arh_f[0];
	arh_f.pop_front();
    }
    res.release();
}

void ModVArchEl::fullErase()
{
    //> Remove archive's files
    ResAlloc res(mRes,true);
    while(arh_f.size())
    {
	arh_f[0]->delFile();
	delete arh_f[0];
	arh_f.pop_front();
    }
    res.release();
}

int ModVArchEl::size()
{
    int rez = 0;
    ResAlloc res(mRes,false);
    for(unsigned i_arh = 0; i_arh < arh_f.size(); i_arh++)
	rez += arh_f[i_arh]->size();

    return rez;
}

void ModVArchEl::checkArchivator( bool now, bool cpctLim )
{
    if(now && !archivator().chkANow)
    {
	//> Scan directory for find new files and deleted files
	struct stat file_stat;
	dirent scan_dirent, *scan_rez = NULL;

	//>> Open archive derictory
	DIR *IdDir = opendir(archivator().addr().c_str());
	if(IdDir == NULL) return;

	//>> Check to allow files
	while(readdir_r(IdDir,&scan_dirent,&scan_rez) == 0 && scan_rez)
	{
	    if(strcmp(scan_rez->d_name,"..") == 0 || strcmp(scan_rez->d_name,".") == 0)	continue;

	    string ArhNm;
	    string NameArhFile = archivator().addr()+"/"+scan_rez->d_name;

	    stat(NameArhFile.c_str(),&file_stat);
	    if((file_stat.st_mode&S_IFMT) != S_IFREG || access(NameArhFile.c_str(),F_OK|R_OK) != 0)	continue;
	    if(!archivator().filePrmGet(NameArhFile,&ArhNm,NULL,NULL,NULL,NULL) || archive().id() != ArhNm)	continue;
	    fileAdd(NameArhFile);
	}

	closedir(IdDir);
    }

    ResAlloc res(mRes,true);
    //>> Check file count for delete old files
    if(now && !mod->noArchLimit && ((((ModVArch &)archivator()).numbFiles() && arh_f.size() > ((ModVArch &)archivator()).numbFiles()) || cpctLim))
	for(unsigned i_arh = 1; i_arh < arh_f.size(); )
	{
	    if(!(arh_f.size() > ((ModVArch &)archivator()).numbFiles() || cpctLim))	break;
	    else if(!arh_f[i_arh]->err())
	    {
		string f_nm = arh_f[i_arh]->name();
		delete arh_f[i_arh];
		arh_f.erase(arh_f.begin() + i_arh);
		remove(f_nm.c_str());
		remove((f_nm+".info").c_str());
		if(cpctLim) break;
		continue;
	    }
	    i_arh++;
	}

    //> Check the archive's files for pack
    res.request(false);
    for(unsigned i_arh = 0; i_arh < arh_f.size(); i_arh++)
	arh_f[i_arh]->check();
}

void ModVArchEl::fileAdd( const string &file )
{
    //> Check to present archive files
    ResAlloc res(mRes,false);
    for(unsigned i_arh = 0; i_arh < arh_f.size(); i_arh++)
	if(arh_f[i_arh]->name() == file) return;
    res.release();

    //> Attach a new archive file
    VFileArch *f_arh = new VFileArch(this);
    f_arh->attach(file);

    //>> Broken archives delete. Oldest archives to up.
    if(f_arh->err()) delete f_arh;
    else
    {
	res.request(true);
	int i_arh;
	for(i_arh = (int)arh_f.size()-1; i_arh >= 0; i_arh--)
	    if(arh_f[i_arh]->err() || f_arh->begin() >= arh_f[i_arh]->begin())
	    {
		arh_f.insert(arh_f.begin()+i_arh+1,f_arh);
		break;
	    }
	if(i_arh < 0) arh_f.push_front(f_arh);
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
    for(unsigned i_a = 0; i_a < arh_f.size(); i_a++)
    {
	if(arh_f[i_a]->err()) continue;
	lstFile = arh_f[i_a];
	if(curTm <= lstFile->end())
	{
	    if(!realEnd) realEnd = lstFile->endData();
	    break;
	}
    }
    if(lstFile && !realEnd) realEnd = lstFile->endData();

    return realEnd;
}

int64_t ModVArchEl::begin()
{
    ResAlloc res(mRes,false);
    for(unsigned i_a = 0; i_a < arh_f.size(); i_a++)
	if(!arh_f[i_a]->err())
	    return arh_f[i_a]->begin();

    return 0;
}

void ModVArchEl::getValsProc( TValBuf &buf, int64_t ibeg, int64_t iend )
{
    //> Request by single values for most big buffer period
    if(buf.period()/100 > (int64_t)(archivator().valPeriod()*1e6))
    {
	ibeg = (ibeg/buf.period())*buf.period();
	for(int64_t ctm; ibeg <= iend; ibeg += buf.period())
	{
	    ctm = ibeg;
	    TVariant vl = getValProc(&ctm, false);
	    buf.set(vl, ibeg);
	}
	return;
    }

    ResAlloc res(mRes, false);
    for(unsigned i_a = 0; i_a < arh_f.size(); i_a++)
	if(ibeg > iend) break;
	else if(!arh_f[i_a]->err() && ibeg <= arh_f[i_a]->end() && iend >= arh_f[i_a]->begin())
	{
	    for( ; ibeg < arh_f[i_a]->begin(); ibeg += arh_f[i_a]->period()) buf.setI(EVAL_INT,ibeg);
	    arh_f[i_a]->getVals(buf, ibeg, vmin(iend,arh_f[i_a]->end()));
	    ibeg = arh_f[i_a]->end()+arh_f[i_a]->period();
	}
    for( ; ibeg <= iend; ibeg += (int64_t)(archivator().valPeriod()*1e6)) buf.setI(EVAL_INT, ibeg);
}

TVariant ModVArchEl::getValProc( int64_t *tm, bool up_ord )
{
    int64_t itm = tm ? *tm : SYS->curTime();
    int64_t per;
    ResAlloc res(mRes,false);
    for(unsigned i_a = 0; i_a < arh_f.size(); i_a++)
	if(!arh_f[i_a]->err() && (
		(up_ord && itm <= arh_f[i_a]->end() && itm > arh_f[i_a]->begin()-arh_f[i_a]->period()) ||
		(!up_ord && itm < arh_f[i_a]->end()+arh_f[i_a]->period() && itm >= arh_f[i_a]->begin())))
	{
	    if(tm) { per = arh_f[i_a]->period(); *tm = (itm/per)*per+((up_ord&&itm%per)?per:0); }
	    return arh_f[i_a]->getVal(up_ord?arh_f[i_a]->maxPos()-(arh_f[i_a]->end()-itm)/arh_f[i_a]->period():(itm-arh_f[i_a]->begin())/arh_f[i_a]->period());
	}
    if(tm) { per = (int64_t)(archivator().valPeriod()*1e6); *tm = (itm>=begin()||itm<=end()) ? (itm/per)*per+((up_ord&&itm%per)?per:0) : 0; }
    return EVAL_REAL;
}

bool ModVArchEl::setValsProc( TValBuf &buf, int64_t beg, int64_t end )
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
    for(unsigned i_a = 0; i_a < arh_f.size(); i_a++)
	if(!arh_f[i_a]->err() && beg <= end)
	{
	    // Create new file for old data
	    if(beg < arh_f[i_a]->begin())
	    {
		//  Calc file limits
		int64_t n_end, n_beg;	//New file end position
		if((arh_f[i_a]->begin()-beg) > f_sz) n_end = beg+f_sz;
		else n_end = arh_f[i_a]->begin()-v_per;
		n_beg = vmax(b_prev,n_end-f_sz);

		//  Create file name
		char c_buf[30];
		time_t tm = n_beg/1000000;
		struct tm tm_tm;
		localtime_r(&tm,&tm_tm);
		strftime(c_buf,sizeof(c_buf)," %F %T.val",&tm_tm);
		string AName = archivator().addr()+"/"+archive().id()+c_buf;

		arh_f.insert(arh_f.begin()+i_a, new VFileArch(AName,n_beg,n_end,v_per,archive().valType(true),this));
	    }

	    // Insert values to archive
	    if(beg <= arh_f[i_a]->end() && end >= arh_f[i_a]->begin())
	    {
		int64_t n_end = (end > arh_f[i_a]->end())?arh_f[i_a]->end():end;
		res.release();
		arh_f[i_a]->setVals(buf,beg,n_end);
		beg = n_end+v_per;
		res.request(true);
	    }
	    b_prev = arh_f[i_a]->end()+v_per;
	}
    //> Create new file for new data
    while(end >= beg)
    {
	char c_buf[30];
	time_t tm = beg/1000000;
	struct tm tm_tm;
	localtime_r(&tm,&tm_tm);
	strftime(c_buf,sizeof(c_buf)," %F %T.val",&tm_tm);
	string AName = archivator().addr()+"/"+archive().id()+c_buf;

	int64_t n_end = beg+f_sz;
	arh_f.push_back(new VFileArch(AName,beg,n_end,v_per,archive().valType(true),this));
	n_end = (end > n_end)?n_end:end;

	res.release();
	arh_f[arh_f.size()-1]->setVals(buf,beg,n_end);
	beg = n_end+v_per;
    }

    realEnd = vmax(realEnd, end);

    return true;
}

//*************************************************
//* FSArch::VFileArch - Value archivator file     *
//*************************************************
string VFileArch::afl_id = "OpenSCADA Val Arch.";

VFileArch::VFileArch( ModVArchEl *owner ) :
    mSize(0), mTp(TFld::Real), mBeg(0), mEnd(0), mPer(1000000), mErr(true), mPack(false),
    fixVl(true), vSize(sizeof(double)), mpos(0), mOwner(owner)
{
    cach_pr_rd.pos = cach_pr_rd.off = cach_pr_wr.pos = cach_pr_wr.off = 0;
    mAcces = time(NULL);
}

VFileArch::VFileArch( const string &iname, int64_t ibeg, int64_t iend, int64_t iper, TFld::Type itp, ModVArchEl *iowner) :
    mName(iname), mSize(0), mTp(itp), mBeg(ibeg), mEnd(iend), mPer(iper), mErr(false), mPack(false), mOwner(iowner)
{
    char buf[1000];
    cach_pr_rd.pos = cach_pr_rd.off = cach_pr_wr.pos = cach_pr_wr.off = 0;

    //> Round-up begin and end position to period
    mBeg = (mBeg/mPer)*mPer;
    mEnd = (mEnd/mPer)*mPer + mPer;

    //> Open/create new archive file
    int hd = open(name().c_str(), O_RDWR|O_CREAT|O_TRUNC, 0666);
    if(hd <= 0) throw TError(owner().archivator().nodePath().c_str(),_("Can not create file: '%s'!"),name().c_str());
    bool fOK = true;

    //> Prepare and write the file archive header
    FHead head;
    memset(&head, 0, sizeof(FHead));
    strncpy(head.f_tp, afl_id.c_str(), sizeof(head.f_tp));
    strncpy(head.archive, owner().archive().id().c_str(), sizeof(head.archive));
    head.beg = begin();
    head.end = end();
    head.period = period();
    head.vtp = type()&TFld::GenMask;
    head.vtpExt = type()>>4;
    head.hgrid = owner().archive().hardGrid();
    head.hres = owner().archive().highResTm();
    head.term = 0x55;
    fOK = (write(hd,&head,sizeof(FHead)) == sizeof(FHead));

    //> Create bit table and init first value
    mpos = (end()-begin())/period();
    switch(type())
    {
	case TFld::String:
	{
	    fixVl = false;
	    vSize = sizeof(char);
	    string s_val = EVAL_STR;
	    eVal  = s_val;

	    //Init pack table
	    int i_buf = 0;
	    for(int i_bt = 0; i_bt < mpos*vSize; i_bt++, i_buf++)
	    {
		if(i_buf == sizeof(buf)) { fOK = fOK && (write(hd,buf,i_buf) == i_buf); i_buf = 0; }
		buf[i_buf] = i_bt ? 0 : strlen(EVAL_STR);
	    }
	    if(i_buf)	fOK = fOK && (write(hd,buf,i_buf) == i_buf);

	    fOK = fOK && (write(hd,s_val.c_str(),s_val.size()) == (int)s_val.size());
	    break;
	}
	case TFld::Int16:
	{
	    fixVl = true;
	    vSize = sizeof(int16_t);
	    int16_t s_val = EVAL_INT16;
	    eVal.assign((char*)&s_val, vSize);

	    int i_buf = 0;
	    for(int i_bt = 0; i_bt < (mpos/8+(bool)(mpos%8)); i_bt++, i_buf++)
	    {
		if(i_buf == sizeof(buf)) { fOK = fOK && (write(hd,buf,i_buf) == i_buf); i_buf = 0; }
		buf[i_buf] = i_bt ? 0 : 1;
	    }
	    if(i_buf)	fOK = fOK && (write(hd,buf,i_buf) == i_buf);

	    fOK = fOK && (write(hd,&s_val,sizeof(s_val)) == sizeof(s_val));
	    break;
	}
	case TFld::Int32:
	{
	    fixVl = true;
	    vSize = sizeof(int32_t);
	    int32_t s_val = EVAL_INT32;
	    eVal.assign((char*)&s_val, vSize);

	    int i_buf = 0;
	    for(int i_bt = 0; i_bt < (mpos/8+(bool)(mpos%8)); i_bt++, i_buf++)
	    {
		if(i_buf == sizeof(buf)) { fOK = fOK && (write(hd,buf,i_buf) == i_buf); i_buf = 0; }
		buf[i_buf] = i_bt ? 0 : 1;
	    }
	    if(i_buf)	fOK = fOK && (write(hd,buf,i_buf) == i_buf);

	    fOK = fOK && (write(hd,&s_val,sizeof(s_val)) == sizeof(s_val));
	    break;
	}
	case TFld::Int64:
	{
	    fixVl = true;
	    vSize = sizeof(int64_t);
	    int64_t s_val = EVAL_INT64;
	    eVal.assign((char*)&s_val, vSize);

	    int i_buf = 0;
	    for(int i_bt = 0; i_bt < (mpos/8+(bool)(mpos%8)); i_bt++, i_buf++)
	    {
		if(i_buf == sizeof(buf)) { fOK = fOK && (write(hd,buf,i_buf) == i_buf); i_buf = 0; }
		buf[i_buf] = i_bt ? 0 : 1;
	    }
	    if(i_buf)	fOK = fOK && (write(hd,buf,i_buf) == i_buf);

	    fOK = fOK && (write(hd,&s_val,sizeof(s_val)) == sizeof(s_val));
	    break;
	}
	case TFld::Float:
	{
	    fixVl = true;
	    vSize = sizeof(float);
	    float s_val = TSYS::floatLE(EVAL_RFlt);
	    eVal.assign((char*)&s_val, vSize);

	    int i_buf = 0;
	    for(int i_bt = 0; i_bt < (mpos/8+(bool)(mpos%8)); i_bt++, i_buf++)
	    {
		if(i_buf == sizeof(buf)) { fOK = fOK && (write(hd,buf,i_buf) == i_buf); i_buf = 0; }
		buf[i_buf] = i_bt ? 0 : 1;
	    }
	    if(i_buf)	fOK = fOK && (write(hd,buf,i_buf) == i_buf);

	    fOK = fOK && (write(hd,&s_val,sizeof(s_val)) == sizeof(s_val));
	    break;
	}
	case TFld::Double:
	{
	    fixVl = true;
	    vSize = sizeof(double);
	    double s_val = TSYS::doubleLE(EVAL_RDbl);
	    eVal.assign((char*)&s_val, vSize);

	    int i_buf = 0;
	    for(int i_bt = 0; i_bt < (mpos/8+(bool)(mpos%8)); i_bt++, i_buf++)
	    {
		if(i_buf == sizeof(buf)) { fOK = fOK && (write(hd,buf,i_buf) == i_buf); i_buf = 0; }
		buf[i_buf] = i_bt ? 0 : 1;
	    }
	    if(i_buf)	fOK = fOK && (write(hd,buf,i_buf) == i_buf);

	    fOK = fOK && (write(hd,&s_val,sizeof(s_val)) == sizeof(s_val));
	    break;
	}
	case TFld::Boolean:
	{
	    fixVl = true;
	    vSize = sizeof(char);
	    char s_val = EVAL_BOOL;
	    eVal.assign((char*)&s_val,vSize);

	    int i_buf = 0;
	    for( int i_bt = 0; i_bt < (mpos/8+(bool)(mpos%8)); i_bt++, i_buf++ )
	    {
		if( i_buf == sizeof(buf) ) { fOK = fOK && (write(hd,buf,i_buf) == i_buf); i_buf = 0; }
		buf[i_buf] = i_bt ? 0 : 1;
	    }
	    if(i_buf)	fOK = fOK && (write(hd,buf,i_buf) == i_buf);

	    fOK = fOK && (write(hd,&s_val,sizeof(s_val)) == sizeof(s_val));
	    break;
	}
	default: break;
    }
    mSize = lseek(hd,0,SEEK_END);
    close(hd);
    mAcces = time(NULL);
}

VFileArch::~VFileArch( )	{ }

void VFileArch::delFile()
{
    ResAlloc res(mRes,true);
    remove(name().c_str());
    remove((name()+(isPack()?".info":".gz.info")).c_str());
    mErr = true;
}

void VFileArch::attach( const string &name )
{
    try
    {
	ResAlloc res(mRes, true);

	mName = name;
	mAcces = time(NULL);

	mPack = mod->filePack(mName);
	mErr  = !owner().archivator().filePrmGet(mName, NULL, &mTp, &mBeg, &mEnd, &mPer);

	//> Init values type parameters
	switch(type())
	{
	    case TFld::String:
	    {
		fixVl = false;
		vSize = sizeof(char);
		eVal = EVAL_STR;
		break;
	    }
	    case TFld::Int16:
	    {
		fixVl = true;
		vSize = sizeof(int16_t);
		int s_val = EVAL_INT16;
		eVal.assign((char*)&s_val, vSize);
		break;
	    }
	    case TFld::Int32:
	    {
		fixVl = true;
		vSize = sizeof(int32_t);
		int s_val = EVAL_INT32;
		eVal.assign((char*)&s_val, vSize);
		break;
	    }
	    case TFld::Int64:
	    {
		fixVl = true;
		vSize = sizeof(int64_t);
		int64_t s_val = EVAL_INT64;
		eVal.assign((char*)&s_val, vSize);
		break;
	    }
	    case TFld::Float:
	    {
		fixVl = true;
		vSize = sizeof(float);
		float s_val = TSYS::floatLE(EVAL_RFlt);
		eVal.assign((char*)&s_val, vSize);
		break;
	    }
	    case TFld::Double:
	    {
		fixVl = true;
		vSize = sizeof(double);
		double s_val = TSYS::doubleLE(EVAL_RDbl);
		eVal.assign((char*)&s_val, vSize);
		break;
	    }
	    case TFld::Boolean:
	    {
		fixVl = true;
		vSize = sizeof(char);
		char s_val = EVAL_BOOL;
		eVal.assign((char*)&s_val,vSize);
		break;
	    }
	    default: break;
	}

	//> Load previous val check
	bool load_prev = false;
	int64_t cur_tm = TSYS::curTime();
	if( cur_tm >= begin() && cur_tm <= end() && period() > 10000000 )
	{ owner().prev_tm = cur_tm; load_prev = true; }

	//> Check and prepare last archive files
	//>> Get file size
	int hd = open(mName.c_str(),O_RDWR);
	if( hd == -1 )	throw TError(owner().archivator().nodePath().c_str(),_("Archive file '%s' no opened!"),name.c_str());
	mSize = lseek(hd,0,SEEK_END);
	mpos = (end()-begin())/period();
	if(!mPack && cur_tm >= begin() && cur_tm <= end()) repairFile(hd);
	close(hd);
	res.release();

	//>> Load previous value
	if(load_prev)
	    switch(type())
	    {
		case TFld::Int16:
		{
		    int16_t tval = getVal((cur_tm-begin())/period()).getI();
		    owner().prev_val.assign((char*)&tval, vSize);
		    break;
		}
		case TFld::Int32:
		{
		    int32_t tval = getVal((cur_tm-begin())/period()).getI();
		    owner().prev_val.assign((char*)&tval, vSize);
		    break;
		}
		case TFld::Int64:
		{
		    int64_t tval = getVal((cur_tm-begin())/period()).getI();
		    owner().prev_val.assign((char*)&tval, vSize);
		    break;
		}
		case TFld::Float:
		{
		    float tval = getVal((cur_tm-begin())/period()).getR();
		    owner().prev_val.assign((char*)&tval, vSize);
		    break;
		}
		case TFld::Double:
		{
		    double tval = getVal((cur_tm-begin())/period()).getR();
		    owner().prev_val.assign((char*)&tval, vSize);
		    break;
		}
		default: break;
	    }
    }
    catch(TError err)
    {
	mess_err(err.cat.c_str(), "%s", err.mess.c_str());
	mess_err(mod->nodePath().c_str(), _("Attach file '%s' error."), name.c_str());
	mErr = true;
    }
}

void VFileArch::check( )
{
    //> Check for pack archive file
    ResAlloc res(mRes, false);
    if(!err() && !isPack() && owner().archivator().packTm() && (time(NULL) > mAcces + owner().archivator().packTm()*60))
    {
	res.request(true);
	mName = mod->packArch(name());
	mPack = true;

	//>> Get file size
	int hd = open(mName.c_str(), O_RDONLY);
	if(hd > 0) { mSize = lseek(hd, 0, SEEK_END); close(hd);	}

	if(!owner().archivator().packInfoFiles())
	{
	    //>> Write info to DB
	    TConfig c_el(&mod->packFE());
	    c_el.cfg("FILE").setS(mName);
	    c_el.cfg("BEGIN").setS(ll2s(begin(),TSYS::Hex));
	    c_el.cfg("END").setS(ll2s(end(),TSYS::Hex));
	    c_el.cfg("PRM1").setS(owner().archive().id());
	    c_el.cfg("PRM2").setS(ll2s(period(),TSYS::Hex));
	    c_el.cfg("PRM3").setS(i2s(type()));
	    SYS->db().at().dataSet(mod->filesDB(),mod->nodePath()+"Pack/",c_el);
	}
	else if((hd=open((mName+".info").c_str(),O_WRONLY|O_CREAT|O_TRUNC,0666)) > 0)
	{
	    //>> Write info to info file
	    string si = TSYS::strMess("%llx %llx %s %llx %d",begin(),end(),owner().archive().id().c_str(),period(),type());
	    if(write(hd,si.data(),si.size()) != (int)si.size())
		mess_err(mod->nodePath().c_str(), _("Write to '%s' error!"), (mName+".info").c_str());
	    close(hd);
	}
    }
}

int64_t VFileArch::endData( )
{
    if(getVal(mpos).getS() != EVAL_STR) return end();

    ResAlloc res(mRes, false);
    if(mErr) throw TError(owner().archivator().nodePath().c_str(),_("Archive file error!"));

    if(mPack)
    {
	res.request(true);
	try{ mName = mod->unPackArch(mName); } catch(TError){ mErr = true; throw; }
	mPack = false;
	res.request(false);
    }

    //> Open archive file
    int hd = open(name().c_str(), O_RDONLY);
    if(hd <= 0) { mErr = true; return end(); }

    //> Find last value offset
    res.request(true);
    int last_off = calcVlOff(hd,mpos);
    int curPos = mpos;
    for(int d_win = curPos/2; d_win > 3; d_win /= 2)
	if(calcVlOff(hd,curPos-d_win) == last_off) curPos -= d_win;
    while(curPos > 0 && calcVlOff(hd,curPos) == last_off) curPos--;
    res.request(false);

    //> Free file resource and close file
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
    if(mErr) throw TError(owner().archivator().nodePath().c_str(),_("Archive file error!"));

    //> Get values block character
    vpos_beg = vmax(0, (beg-begin())/period());
    if(vpos_beg > mpos)	return;
    vpos_end = vmin(mpos, (end-begin())/period());
    if(vpos_end < 0)	return;
    //if( (vpos_end-vpos_beg) > (TArchiveS::max_req_vals-buf.realSize()) )
    //	vpos_end = vpos_beg+TArchiveS::max_req_vals-buf.realSize();
    if(vpos_beg > vpos_end)	return;

    if(mPack)
    {
	res.request(true);
	try{ mName = mod->unPackArch(mName); } catch(TError){ mErr = true; throw; }
	mPack = false;
	res.request(false);
    }

    //> Open archive file
    int hd = open(name().c_str(), O_RDONLY);
    if(hd <= 0) { mErr = true; return; }
    bool fOK = true;

    res.request(true);
    voff_beg = calcVlOff(hd, vpos_beg, &vlen_beg);
    res.request(false);

    //> Get the pack index block and the value block
    if(fixVl)
    {
	//>> Get index block
	int i_beg = sizeof(FHead) + vpos_beg/8;
	int i_end = sizeof(FHead) + vpos_end/8+1;
	lseek(hd, i_beg, SEEK_SET);
	pid_b = (char*)malloc(i_end-i_beg);
	if((fOK=(read(hd,pid_b,i_end-i_beg) == (i_end-i_beg))))
	{
	    //>> Calc end offset
	    voff_end = voff_beg;
	    for(int i_pos = vpos_beg+1; i_pos <= vpos_end; i_pos++)
		voff_end += vSize*(bool)((0x01<<(i_pos%8))&pid_b[(i_pos/8)-(vpos_beg/8)]);
	    //>> Get value block
	    i_beg = voff_beg;
	    i_end = voff_end+vSize;
	    lseek(hd, i_beg, SEEK_SET);
	    val_b = (char*)malloc(i_end-i_beg);
	    fOK = (read(hd,val_b,i_end-i_beg) == (i_end-i_beg));
	}
    }
    else
    {
	//>> Get index block
	int i_beg = sizeof(FHead) + vpos_beg*vSize;
	int i_end = sizeof(FHead) + vpos_end*vSize+vSize;
	lseek(hd, i_beg, SEEK_SET);
	pid_b = (char*)malloc(i_end-i_beg);
	if((fOK=(read(hd,pid_b,i_end-i_beg) == (i_end-i_beg))))
	{
	    //>> Calc end offset
	    voff_end = voff_beg;
	    vlen_end = vlen_beg;
	    for(int i_pos = (vpos_beg+1); i_pos <= vpos_end; i_pos++)
	    {
		int pk_vl = 0;
		for(int i_e = 0; i_e < vSize; i_e++)
		    pk_vl += pid_b[vSize*(i_pos-vpos_beg)+i_e]<<(8*i_e);
		if(pk_vl)
		{
		    voff_end += vlen_end;
		    vlen_end = pk_vl;
		}
	    }
	    //>> Get value block
	    i_beg = voff_beg;
	    i_end = voff_end+vlen_end;
	    lseek(hd, i_beg, SEEK_SET);
	    val_b = (char*)malloc(i_end-i_beg);
	    fOK = (read(hd,val_b,i_end-i_beg) == (i_end-i_beg));
	}
    }

    //> Free file resource and close file
    close(hd);
    mAcces = time(NULL);
    res.release();

    //> Process value block
    int pid_off = vpos_beg;
    voff_end -= voff_beg;
    voff_beg = 0;
    while(fOK)
    {
	switch(type())
	{
	    case TFld::Boolean:
		buf.setB((char)*(val_b+voff_beg),begin()+(int64_t)vpos_beg*period());	break;
	    case TFld::Int16:
	    {
		int16_t vl = *(int16_t*)(val_b+voff_beg);
		buf.setI((vl==EVAL_INT16)?EVAL_INT:vl, begin()+(int64_t)vpos_beg*period());
		break;
	    }
	    case TFld::Int32:
	    {
		int32_t vl = *(int32_t*)(val_b+voff_beg);
		buf.setI((vl==EVAL_INT32)?EVAL_INT:vl, begin()+(int64_t)vpos_beg*period());
		break;
	    }
	    case TFld::Int64:
	    {
		int64_t vl = *(int64_t*)(val_b+voff_beg);
		buf.setI((vl==EVAL_INT64)?EVAL_INT:vl, begin()+(int64_t)vpos_beg*period());
		break;
	    }
	    case TFld::Float:
	    {
		float vl = TSYS::floatLErev(*(float*)(val_b+voff_beg));
		buf.setR((vl<=EVAL_RFlt)?EVAL_REAL:vl, begin()+(int64_t)vpos_beg*period());
		break;
	    }
	    case TFld::Double:
	    {
		double vl = TSYS::doubleLErev(*(double*)(val_b+voff_beg));
		buf.setR((vl<=EVAL_RDbl)?EVAL_REAL:vl, begin()+(int64_t)vpos_beg*period());
		break;
	    }
	    case TFld::String:
		buf.setS(string(val_b+voff_beg,vlen_beg),begin()+(int64_t)vpos_beg*period());	break;
	    default: break;
	}
	vpos_beg++;
	if(vpos_beg > vpos_end)	break;
	if(fixVl) voff_beg += vSize*(bool)(pid_b[(vpos_beg/8)-(pid_off/8)]&(0x01<<(vpos_beg%8)));
	else
	{
	    int pk_vl = 0;
	    for(int i_e = 0; i_e < vSize; i_e++) pk_vl += pid_b[vSize*(vpos_beg-pid_off)+i_e]<<(8*i_e);
	    if(pk_vl)
	    {
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
    if(mErr) throw TError(owner().archivator().nodePath().c_str(), _("Archive file error!"));

    mAcces = time(NULL);

    if(mPack)
    {
	res.request(true);
	try{ mName = mod->unPackArch(mName); } catch(TError){ mErr = true; throw; }
	mPack = false;
	res.request(false);
    }

    //> Open archive file
    int hd = open(name().c_str(), O_RDONLY);
    if(hd <= 0) { mErr = true; return EVAL_REAL; }

    res.request(true);
    switch(type())
    {
	case TFld::Boolean:
	{
	    char rez = *(char*)getValue(hd, calcVlOff(hd,vpos), sizeof(char)).c_str();
	    close(hd);
	    return rez;
	}
	case TFld::Int16:
	{
	    int16_t rez = *(int16_t*)getValue(hd, calcVlOff(hd,vpos), sizeof(int16_t)).c_str();
	    close(hd);
	    return (rez==EVAL_INT16) ? (int64_t)EVAL_INT : rez;
	}
	case TFld::Int32:
	{
	    int32_t rez = *(int32_t*)getValue(hd, calcVlOff(hd,vpos), sizeof(int32_t)).c_str();
	    close(hd);
	    return (rez==EVAL_INT32) ? (int64_t)EVAL_INT : rez;
	}
	case TFld::Int64:
	{
	    int64_t rez = *(int64_t*)getValue(hd, calcVlOff(hd,vpos), sizeof(int64_t)).c_str();
	    close(hd);
	    return (rez==EVAL_INT64) ? (int64_t)EVAL_INT : rez;
	}
	case TFld::Float:
	{
	    float rez = TSYS::floatLErev(*(float*)getValue(hd, calcVlOff(hd,vpos), sizeof(float)).c_str());
	    close(hd);
	    return (rez<=EVAL_RFlt) ? EVAL_REAL : rez;
	}
	case TFld::Double:
	{
	    double rez = TSYS::doubleLErev(*(double*)getValue(hd, calcVlOff(hd,vpos), sizeof(double)).c_str());
	    close(hd);
	    return (rez<=EVAL_RDbl) ? EVAL_REAL : rez;
	}
	case TFld::String:
	{
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

void VFileArch::setVals( TValBuf &buf, int64_t ibeg, int64_t iend )
{
    int vpos_beg, vpos_end;
    string val_b, value, value_first, value_end;       //Set value

    ResAlloc res(mRes, false);
    if(mErr) throw TError(owner().archivator().nodePath().c_str(), _("Archive file error!"));

    ibeg = vmax(ibeg, begin());
    iend = vmin(iend, end());
    if(ibeg > iend)	return;

    mAcces = time(NULL);

    if(mPack)
    {
	res.request(true);
	try{ mName = mod->unPackArch(mName); } catch(TError){ mErr = true; throw; }
	mPack = false;
	res.request(false);
    }

    //Init pack index buffer
    vpos_beg = (ibeg-begin())/period();
    vpos_end = (iend-begin())/period();

    string pid_b(fixVl ? (vpos_end/8)-(vpos_beg/8)+1 : vSize*(vpos_end-vpos_beg+1), '\0');

    //Reserve memory for values buffer
    val_b.reserve(fixVl ? vSize*(vpos_end-vpos_beg+1)/2 : 10*(vpos_end-vpos_beg+1));

    //> Get values, make value buffer and init the pack index table
    vpos_beg = 0;
    vpos_end = -1;
    while(ibeg <= iend)
    {
	//>> Get value and put it to file
	switch(type())
	{
	    case TFld::Boolean:
	    {
		char tval = buf.getB(&ibeg, true);
		value.assign(&tval, vSize);
		break;
	    }
	    case TFld::Int16: case TFld::Int32: case TFld::Int64:
	    {
		int64_t tval = buf.getI(&ibeg, true);
		if(((ModVArch&)owner().archivator()).roundProc() && vpos_end >= 0)
		{
		    int64_t vprev = *(int64_t*)value_end.c_str();
		    if(((vprev > 0 && tval > 0) || (vprev < 0 && tval < 0)) &&
			    100.*(double)abs(vprev-tval)/(double)vmax(abs(vprev),abs(tval)) <= ((ModVArch&)owner().archivator()).roundProc())
			tval = vprev;
		}
		switch(type())
		{
		    case TFld::Int16: { int16_t vl = (tval==EVAL_INT)?EVAL_INT16:tval; value.assign((char*)&vl, vSize); }	break;
		    case TFld::Int32: { int32_t vl = (tval==EVAL_INT)?EVAL_INT32:tval; value.assign((char*)&vl, vSize); }	break;
		    case TFld::Int64: { int64_t vl = (tval==EVAL_INT)?EVAL_INT64:tval; value.assign((char*)&vl, vSize); }	break;
		    default: break;
		}
		break;
	    }
	    case TFld::Float: case TFld::Double:
	    {
		double tval = buf.getR(&ibeg, true);
		if(((ModVArch&)owner().archivator()).roundProc() && vpos_end >= 0)
		{
		    double vprev = *(double*)value_end.c_str();
		    if(((vprev > 0 && tval > 0) || (vprev < 0 && tval < 0)) &&
			    100.*fabs(vprev-tval)/vmax(fabs(vprev),fabs(tval)) <= ((ModVArch&)owner().archivator()).roundProc())
			tval = vprev;
		}
		switch(type())
		{
		    case TFld::Float:  { float vl = TSYS::floatLE((tval==EVAL_REAL)?EVAL_RFlt:tval); value.assign((char*)&vl, vSize); }		break;
		    case TFld::Double: { double vl = TSYS::doubleLE((tval==EVAL_REAL)?EVAL_RDbl:tval); value.assign((char*)&vl, vSize); }	break;
		    default: break;
		}
		break;
	    }
	    case TFld::String:
	    {
		value = buf.getS(&ibeg, true);
		if(!value.size()) value = " ";
		if((int)value.size() >= (1<<(vSize*8)))
		    value.erase((1<<(vSize*8))-1);
		break;
	    }
	    default: break;
	}

	int pos_cur = (vmin(ibeg,iend)-begin())/period();		//> Border add to end for prevent index buffer overlap
	if(vpos_end < 0) { vpos_beg = pos_cur; vpos_end = vpos_beg-1; }	//> First call
	int pos_i = vpos_end;
	while(pos_i < pos_cur)
	{
	    pos_i++;
	    string wr_val = (pos_i==pos_cur) ? value : eVal;
	    if(vpos_end < vpos_beg || wr_val != value_end)
	    {
		if(fixVl)
		{
		    int b_n = (pos_i/8)-(vpos_beg/8);
		    pid_b[b_n] = pid_b[b_n]|(0x01<<(pos_i%8));
		    val_b.append(wr_val);
		}
		else
		{
		    int v_sz = wr_val.size();
		    for(int v_psz = 0; v_psz < vSize; v_psz++)
			pid_b[vSize*(pos_i-vpos_beg)+v_psz] = *(((char *)&v_sz)+v_psz);
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
    //> Open archive file
    int hd = open(name().c_str(), O_RDWR);
    if(hd <= 0) { mErr = true; return; }
    bool fOK = true;

    //> Get block geometry from file
    int foff_beg_len, foff_beg, foff_begprev_len, foff_begprev = 0, foff_end_len, foff_end, foff_endnext_len, foff_endnext = 0;
    if(vpos_beg) foff_begprev = calcVlOff(hd, vpos_beg-1, &foff_begprev_len, true);
    foff_beg = calcVlOff(hd, vpos_beg, &foff_beg_len, true);
    foff_end = calcVlOff(hd, vpos_end, &foff_end_len, true);
    if(vpos_beg < mpos) foff_endnext = calcVlOff(hd, vpos_end+1, &foff_endnext_len, true);

    //> Checking and adaptation border
    if(fixVl)
    {
	//>> Check begin border
	if(vpos_beg)
	{
	    if(getValue(hd,foff_begprev,vSize) == value_first)
	    {
		pid_b[0] &= ~(0x01<<(vpos_beg%8));
		if(getPkVal(hd,vpos_beg))	foff_beg -= vSize;
	    }
	    else if(!getPkVal(hd,vpos_beg))	foff_beg += vSize;
	}
	//>> Check end border
	if(vpos_end < mpos)
	{
	    if(getValue(hd,foff_endnext,vSize) == value_end)
	    {
		if(getPkVal(hd,vpos_end+1))
		{
		    foff_end += 2*vSize;
		    setPkVal(hd, vpos_end+1, 0);
		}
		else foff_end += vSize;
	    }
	    else
	    {
		if(getPkVal(hd,vpos_end+1)) foff_end += vSize;
		else setPkVal(hd,vpos_end+1, 1);
	    }
	}
	//>> Merge begin and end pack values
 	char tmp_pb;
	lseek(hd, sizeof(FHead)+vpos_beg/8, SEEK_SET);
	fOK = fOK && (read(hd,&tmp_pb,1) == 1);
	pid_b[0] |= (~(0xFF<<(vpos_beg%8)))&tmp_pb;
	lseek(hd, sizeof(FHead)+vpos_end/8, SEEK_SET);
	fOK = fOK && (read(hd,&tmp_pb,1) == 1);
	pid_b[vpos_end/8-vpos_beg/8] |= (0xFE<<(vpos_end%8))&tmp_pb;
    }
    else
    {
	//>> Check begin border
	if(vpos_beg)
	{
	    if(getValue(hd,foff_begprev,foff_begprev_len) == value_first)
	    {
		for(int i_sz = 0; i_sz < vSize; i_sz++) pid_b[i_sz] = 0;
		if(getPkVal(hd,vpos_beg))	foff_beg -= foff_begprev_len;
	    }
	    else if(!getPkVal(hd,vpos_beg)) foff_beg += foff_begprev_len;
	}
	//>> Check end border
	if(vpos_end < mpos)
	{
	    if(getValue(hd,foff_endnext,foff_endnext_len) == value_end)
	    {
		if(getPkVal(hd,vpos_end+1))
		{
		    foff_end += foff_end_len+foff_endnext_len;
		    setPkVal(hd,vpos_end+1,0);
		}
		else foff_end += foff_end_len;
	    }
	    else
	    {
		if(getPkVal(hd,vpos_end+1)) foff_end += foff_end_len;
		else setPkVal(hd,vpos_end+1,foff_endnext_len);
	    }
	}
    }
    //> Write pack id buffer and value buffer
    int pid_b_sz;
    if(fixVl)
    {
	lseek(hd, sizeof(FHead)+vpos_beg/8, SEEK_SET);
	pid_b_sz = vpos_end/8 - vpos_beg/8 + 1;
    }
    else
    {
	lseek(hd, sizeof(FHead)+vSize*vpos_beg, SEEK_SET);
	pid_b_sz = vSize*(vpos_end-vpos_beg+1);
    }
    if((int)pid_b.size() < pid_b_sz) mess_err(mod->nodePath().c_str(),_("Warning! Allocated buffer size %d for indexes lesser for used %d."),pid_b.size(),pid_b_sz);
    fOK = fOK && (write(hd,pid_b.data(),pid_b_sz) == pid_b_sz);
    moveTail(hd, foff_end, foff_end+(val_b.size()-(foff_end-foff_beg)));
    lseek(hd, foff_beg, SEEK_SET);
    fOK = fOK && (write(hd,val_b.data(),val_b.size()) == (int)val_b.size());

    //> Check for write to end correct
    if(fOK && mod->copyErrValFiles && fixVl && iend > owner().end() && iend < end() && !((mSize-foff_end) == vSize ||
		((mSize-foff_end) == 0 && (int)val_b.size() >= vSize && val_b.compare(val_b.size()-vSize,vSize,eVal) == 0)))
	mess_err(mod->nodePath().c_str(), _("Write data block to archive file '%s' error. Will structure break. mSize=%d, foff_end=%d, vSize=%d"),name().c_str(),mSize,foff_end,vSize);

    //> Drop cache
    cacheDrop(vpos_beg);
    cacheSet(vpos_end, foff_beg+val_b.size()-value_end.size(), value_end.size(), true, true);

    mSize = lseek(hd, 0, SEEK_END);

    close(hd);
}

string VFileArch::getValue( int hd, int voff, int vsz )
{
    bool fOK = true;
    string get_vl;
    get_vl.reserve(10);

    lseek(hd, voff, SEEK_SET);
    if((fOK=(read(hd,&tbt,1)==1)))
    {
	get_vl.assign((char *)&tbt,sizeof(char));
	for(int i_vs = 0; fOK && i_vs < (vsz-1); i_vs++)
	    if((fOK=(read(hd,&tbt,1)==1)))
		get_vl.append((char *)&tbt,sizeof(char));
    }

    if(!fOK) mess_err(mod->nodePath().c_str(), _("Access to file '%s' error!"), mName.c_str());

    return get_vl;
}

int VFileArch::calcVlOff( int hd, int vpos, int *vsz, bool wr )
{
    bool fOK = true;
    int b_sz = 0, i_bf = 0;
    char buf[4096];
    int voff;

    if(fixVl)
    {
	int cach_pos = vpos;
	int cach_off = cacheGet(cach_pos);
	if(cach_off) voff = cach_off;
	else voff = sizeof(FHead)+mpos/8+(bool)(mpos%8);
	if(cach_pos == vpos) return voff;

	cach_pos++;
	int i_ps = cach_pos;

	for(int n_pos = 0; fOK && i_ps <= vpos; i_ps = n_pos)
	{
	    //> Fast algorithm for big blocks
	    //if(!((i_ps%8) || ((i_ps/8)%4)) && (i_ps/32) < (vpos/32))
	    if(!((i_ps%8) || (i_bf%4)) && (vpos/8 - i_ps/8) >= 4)
	    {
		//> Buffer check for refresh
		if((i_bf+4) > b_sz)
		{
		    lseek(hd, sizeof(FHead)+i_ps/8, SEEK_SET);
		    b_sz = vmin((vpos/8)-(i_ps/8)+1, (int)sizeof(buf));
		    fOK = (read(hd,&buf,b_sz) == b_sz);
		    i_bf = 0;
		}
		//> Count
		uint32_t vw = *(uint32_t*)(buf+i_bf);	//TSYS::getUnalign32(buf+i_bf);
		vw -= ((vw>>1)&0x55555555);
		vw = (vw&0x33333333) + ((vw>>2)&0x33333333);
		voff += vSize * ((((vw+(vw>>4))&0xF0F0F0F)*0x1010101)>>24);
		n_pos = i_ps + 32; i_bf += 4;
		//> Update cache
		if(i_ps && (i_ps%VAL_CACHE_POS) == 0) cacheSet(i_ps+31, voff, 0, false, wr);
	    }
	    //> Simple algorithm
	    else
	    {
		//> Buffer check for refresh
		if(i_bf >= b_sz)
		{
		    lseek(hd, sizeof(FHead)+i_ps/8, SEEK_SET);
		    b_sz = vmin((vpos/8)-(i_ps/8)+1, (int)sizeof(buf));
		    fOK = (read(hd,&buf,b_sz) == b_sz);
		    i_bf = 0;
		}
		//> Count
		voff += vSize * (0x01&(buf[i_bf]>>(i_ps%8)));
		n_pos = i_ps+1;
		if((n_pos%8) == 0) i_bf++;
		//> Update cache
		if(i_ps == vpos) cacheSet(i_ps, voff, 0, i_ps==vpos, wr);
	    }
	}
    }
    else
    {
	int cach_pos = vpos;
	int lst_pk_vl;
	int cach_off = cacheGet(cach_pos, &lst_pk_vl);
	if(cach_off)	{ voff = cach_off; cach_pos++; }
	else voff = sizeof(FHead)+mpos*vSize;
	lseek(hd, sizeof(FHead)+cach_pos*vSize, SEEK_SET);

	for(int i_ps = cach_pos; fOK && i_ps <= vpos; i_ps++)
	{
	    int pk_vl = 0;
	    for(int i_e = 0; i_e < vSize; ++i_e)
	    {
		if(++i_bf >= b_sz)
		{
		    b_sz = vmin(vSize*(vpos-i_ps+1), (int)sizeof(buf));
		    fOK = (read(hd,&buf,b_sz) == b_sz);
		    i_bf = 0;
		}
		pk_vl += buf[i_bf]<<(8*i_e);
	    }
	    if(pk_vl)
	    {
		if(i_ps) voff += lst_pk_vl;
		lst_pk_vl = pk_vl;
	    }
	    //> Update cache
	    if((i_ps-cach_pos && !((i_ps-cach_pos)%VAL_CACHE_POS)) || i_ps == vpos)
		cacheSet(i_ps, voff, lst_pk_vl, i_ps==vpos, wr);
	}
	if(vsz) *vsz = lst_pk_vl;
    }

    return voff;
}

void VFileArch::setValue( int hd, int voff, const string &val )
{
    lseek(hd, voff, SEEK_SET);
    if(write(hd,val.c_str(),val.size()) != (int)val.size())
	mess_err(mod->nodePath().c_str(), _("Write to file '%s' error!"), mName.c_str());
}

void VFileArch::moveTail( int hd, int old_st, int new_st )
{
    bool fOK = true;
    char buf[STR_BUF_LEN];

    if(old_st == new_st) return;
    if(new_st > old_st && fOK)
    {
	//> Move down (insert)
	int beg_cur;
	int mv_beg = old_st;
	int mv_end = lseek(hd,0,SEEK_END);
	if(mv_end <= mv_beg)	return;
	do
	{
	    beg_cur = ((mv_end-mv_beg) >= (int)sizeof(buf)) ? mv_end-sizeof(buf) : mv_beg;
	    lseek(hd,beg_cur,SEEK_SET);
	    fOK = fOK && (read(hd,buf,mv_end-beg_cur) == (mv_end-beg_cur));
	    lseek(hd, beg_cur+(new_st-old_st), SEEK_SET);
	    fOK = fOK && (write(hd,buf,mv_end-beg_cur) == (mv_end-beg_cur));
	    mv_end -= sizeof(buf);
	}
	while(beg_cur != mv_beg && fOK);
    }
    else
    {
	//> Move up (erase)
	int end_cur;
	int mv_beg = old_st;
	int mv_end = lseek(hd, 0, SEEK_END);
	if(mv_end > mv_beg && fOK)
	{
	    do
	    {
		end_cur = ((mv_end-mv_beg) >= (int)sizeof(buf)) ? mv_beg+sizeof(buf) : mv_end;
		lseek(hd, mv_beg, SEEK_SET);
		fOK = fOK && (read(hd,buf,end_cur-mv_beg) == (end_cur-mv_beg));
		lseek(hd, mv_beg-(old_st-new_st), SEEK_SET);
		fOK = fOK && (write(hd,buf,end_cur-mv_beg) == (end_cur-mv_beg));
		mv_beg += sizeof(buf);
	    }
	    while(end_cur != mv_end && fOK);
	}

	//> Truncate tail
	fOK = fOK && (ftruncate(hd,mv_end-(old_st-new_st)) == 0);
    }
}

int VFileArch::getPkVal( int hd, int vpos )
{
    bool fOK = true;
    int pk_val = 0;

    if(fixVl)
    {
	lseek(hd, sizeof(FHead)+vpos/8, SEEK_SET);
	fOK = (read(hd,&tbt,1) == 1);
	pk_val = ((0x01<<(vpos%8))&tbt)?1:0;
    }
    else
    {
	lseek(hd, sizeof(FHead)+vpos*vSize, SEEK_SET);
	for(int v_psz = 0; fOK && v_psz < vSize; v_psz++)
	{
	    fOK = (read(hd,&tbt,1) == 1);
	    pk_val += tbt<<(v_psz*8);
	}
    }

    return pk_val;
}

void VFileArch::setPkVal( int hd, int vpos, int vl )
{
    bool fOK = true;
    if(fixVl)
    {
	lseek(hd, sizeof(FHead)+vpos/8, SEEK_SET);
	fOK = (read(hd,&tbt,1) == 1);
	tbt = vl ? ((0x01<<(vpos%8))|tbt) : (~(0x01<<(vpos%8))&tbt);
	lseek(hd, -1, SEEK_CUR);
	fOK = fOK && (write(hd,&tbt,1) == 1);
    }
    else
    {
	lseek(hd, sizeof(FHead)+vpos*vSize, SEEK_SET);
	for(int v_psz = 0; fOK && v_psz < vSize; v_psz++)
	    fOK = (write(hd, ((char *)&vl)+v_psz, 1) == 1);
    }
}

void VFileArch::repairFile( int hd )
{
    int v_sz;
    if(!mPack)
    {
	bool fOK = true;
	int f_sz = lseek(hd, 0, SEEK_END);
	int f_off = calcVlOff(hd, mpos, &v_sz);
	if(fixVl)
	{
	    int dt = f_sz-f_off-vSize;
	    if(!dt) return;
	    mess_err(owner().archivator().nodePath().c_str(),
		_("Error archive file structure: '%s'. Margin = %d byte. Will try fix it!"),name().c_str(),dt);
	    //> Copy error file
	    if(mod->copyErrValFiles)
	    {
		char cpBuf[4096];
		int ehd = open((name()+".err").c_str(), O_WRONLY|O_CREAT|O_TRUNC, 0666);
		if(ehd < 0) mess_err(owner().archivator().nodePath().c_str(), _("Error open new archive file for copy."));
		else
		{
		    lseek(hd, 0, SEEK_SET);
		    for(int rs = 0; fOK && (rs=read(hd, cpBuf, sizeof(cpBuf))) > 0; )
			fOK = (write(ehd,cpBuf,rs) == rs);
		    close(ehd);
		}
	    }
	    //> Fix file
	    if(dt > 0 && fOK)
	    {
		mSize = f_off + vSize;
		if((fOK=(ftruncate(hd,mSize)==0))) setValue(hd, f_off, eVal);
	    }
	    else
	    {
		f_sz = f_off - vSize*((f_off-f_sz)/vSize);
		while(f_sz <= f_off) { setValue(hd,f_sz,eVal); f_sz += vSize; }
	    }
	}
	else
	{
	    //In progress !!!!
	}
    }
}

int VFileArch::cacheGet( int &pos, int *vsz )
{
    CacheEl rez = {0,0,0};
    for(int i_p = (int)cache.size()-1; i_p >= 0; i_p--)
	if(pos >= cache[i_p].pos) { rez = cache[i_p]; break; }

    if(pos >= cach_pr_rd.pos && cach_pr_rd.pos > rez.pos)	rez = cach_pr_rd;
    if(pos >= cach_pr_wr.pos && cach_pr_wr.pos > rez.pos)	rez = cach_pr_wr;

    pos = rez.pos;
    if(vsz) *vsz = rez.vsz;
    return rez.off;
}

void VFileArch::cacheSet( int pos, int off, int vsz, bool last, bool wr  )
{
    CacheEl el = { pos, off, vsz };

    if(!last)
    {
	for(unsigned i_p = 0; i_p < cache.size(); i_p++)
	    if(el.pos == cache[i_p].pos)	{ cache[i_p] = el; return; }
	    else if(el.pos < cache[i_p].pos)	{ cache.insert(cache.begin()+i_p,el); return; }
	cache.push_back(el);
    }
    else if(wr) cach_pr_wr = el;
    else cach_pr_rd = el;
}

void VFileArch::cacheDrop( int pos )
{
    for(unsigned i_p = 0; i_p < cache.size(); )
    {
	if(cache[i_p].pos >= pos) { cache.erase(cache.begin()+i_p); continue; }
	i_p++;
    }
    if(cach_pr_rd.pos >= pos)	cach_pr_rd.off = cach_pr_rd.pos = cach_pr_rd.vsz = 0;
    if(cach_pr_wr.pos >= pos)	cach_pr_wr.off = cach_pr_wr.pos = cach_pr_wr.vsz = 0;
}
