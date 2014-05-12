
//OpenSCADA system module Archive.FSArch file: mess.cpp
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

#include <sys/time.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <dirent.h>
#include <errno.h>
#include <string.h>

#include <tsys.h>
#include "base.h"
#include "mess.h"

using namespace FSArch;

//************************************************
//* FSArch::ModMArch - Messages archivator       *
//************************************************
ModMArch::ModMArch( const string &iid, const string &idb, TElem *cf_el ) :
    TMArchivator(iid,idb,cf_el),
    mUseXml(false), mMaxSize(1024), mNumbFiles(30), mTimeSize(30), mChkTm(60), mPackTm(10), mPackInfoFiles(false), mPrevDbl(false),
    tmCalc(0.0), mLstCheck(0)
{

}

ModMArch::~ModMArch( )
{
    try{ stop(); }catch(...){ }
}

void ModMArch::load_( )
{
    TMArchivator::load_();

    try
    {
	XMLNode prmNd;
	string  vl;
	prmNd.load(cfg("A_PRMS").getS());
	vl = prmNd.attr("XML");		if(!vl.empty()) setUseXML(atoi(vl.c_str()));
	vl = prmNd.attr("MSize");	if(!vl.empty()) setMaxSize(atoi(vl.c_str()));
	vl = prmNd.attr("NFiles");	if(!vl.empty()) setNumbFiles(atoi(vl.c_str()));
	vl = prmNd.attr("TmSize");	if(!vl.empty()) setTimeSize(atoi(vl.c_str()));
	vl = prmNd.attr("PackTm");	if(!vl.empty()) setPackTm(atoi(vl.c_str()));
	vl = prmNd.attr("CheckTm");	if(!vl.empty()) setCheckTm(atoi(vl.c_str()));
	vl = prmNd.attr("PackInfoFiles"); if(!vl.empty()) setPackInfoFiles(atoi(vl.c_str()));
	vl = prmNd.attr("PrevDbl");	if(!vl.empty()) setPrevDbl(atoi(vl.c_str()));
    } catch(...){ }

}

void ModMArch::save_( )
{
    XMLNode prmNd("prms");
    prmNd.setAttr("XML", i2s(useXML()));
    prmNd.setAttr("MSize", i2s(maxSize()));
    prmNd.setAttr("NFiles", i2s(numbFiles()));
    prmNd.setAttr("TmSize", i2s(timeSize()));
    prmNd.setAttr("PackTm", i2s(packTm()));
    prmNd.setAttr("CheckTm", i2s(checkTm()));
    prmNd.setAttr("PackInfoFiles", i2s(packInfoFiles()));
    prmNd.setAttr("PrevDbl", i2s(prevDbl()));
    cfg("A_PRMS").setS(prmNd.save(XMLNode::BrAllPast));

    TMArchivator::save_();
}

void ModMArch::start( )
{
    //First scan dir
    checkArchivator();
    TMArchivator::start();
}

void ModMArch::stop( )
{
    TMArchivator::stop();

    //Clear archive files list
    ResAlloc res(mRes, true);
    while(arh_s.size())
    {
	delete arh_s[0];
	arh_s.pop_front();
    }
}

time_t ModMArch::begin( )
{
    ResAlloc res(mRes, false);
    for(int i_arh = arh_s.size()-1; i_arh >= 0; i_arh--)
	if(!arh_s[i_arh]->err()) return arh_s[i_arh]->begin();

    return 0;
}

time_t ModMArch::end( )
{
    ResAlloc res(mRes, false);
    for(unsigned i_arh = 0; i_arh < arh_s.size(); i_arh++)
	if(!arh_s[i_arh]->err()) return arh_s[i_arh]->end();

    return 0;
}

bool ModMArch::put( vector<TMess::SRec> &mess )
{
    int64_t t_cnt = TSYS::curTime();

    ResAlloc res(mRes, false);

    if(!run_st) throw TError(nodePath().c_str(),_("Archive is not started!"));

    for(unsigned i_m = 0; i_m < mess.size(); i_m++)
    {
	if(!chkMessOK(mess[i_m].categ,mess[i_m].level)) continue;
	int i_arh;
	for(i_arh = 0; i_arh < (int)arh_s.size(); i_arh++)
	    if(!arh_s[i_arh]->err() && mess[i_m].time >= arh_s[i_arh]->begin())
	    {
		if(mess[i_m].time > arh_s[i_arh]->end() &&
		    ((mMaxSize && i_arh == 0 && arh_s[i_arh]->size() > mMaxSize*1024) ||
		    (mess[i_m].time >= arh_s[i_arh]->begin()+mTimeSize*24*60*60))) break;
		try{ arh_s[i_arh]->put(mess[i_m]); }
		catch(TError err)
		{ mess_err(err.cat.c_str(),err.mess.c_str()); continue; }
		i_arh = -1;
		break;
	    }
	//If going a new data then create new file
	if(i_arh >= 0)
	{
	    res.request(true);
	    time_t f_beg = mess[i_m].time;
	    if(i_arh < (int)arh_s.size() && f_beg > arh_s[i_arh]->end() && (f_beg-arh_s[i_arh]->end()) < (mTimeSize*24*60*60/3))
		f_beg = arh_s[i_arh]->end()+1;
	    if(i_arh && arh_s[i_arh-1]->begin() > f_beg && (arh_s[i_arh-1]->begin()-f_beg) < (mTimeSize*24*60*60*2/3))
		f_beg = arh_s[i_arh-1]->begin()-mTimeSize*24*60*60;
	    // Create new Archive
	    string f_name = TSYS::time2str(f_beg,"/%F %T.msg");
	    try
	    {
		MFileArch *f_obj = new MFileArch(addr()+f_name, f_beg, this, Mess->charset(), useXML());
		if(i_arh == (int)arh_s.size()) arh_s.push_back(f_obj);
		else if(i_arh < (int)arh_s.size()) arh_s.insert(arh_s.begin()+i_arh,f_obj);
		else return true;
	    }
	    catch(TError err)
	    {
		mess_crit(nodePath().c_str(),_("Error create new archive file '%s'!"),(addr()+f_name).c_str() );
		return false;
	    }
	    // Allow parallel read access
	    res.request(false);
	    arh_s[i_arh]->put(mess[i_m]);
	}
    }
    tmCalc = TSYS::curTime()-t_cnt;

    return true;
}

void ModMArch::get( time_t b_tm, time_t e_tm, vector<TMess::SRec> &mess, const string &category, char level, time_t upTo )
{
    if(e_tm <= b_tm) return;
    if(!run_st) throw TError(nodePath().c_str(), _("Archive is not started!"));
    if(!upTo) upTo = time(NULL)+STD_INTERF_TM;

    ResAlloc res(mRes, false);
    for(int i_arh = arh_s.size()-1; i_arh >= 0 && time(NULL) < upTo; i_arh--)
    {
	if(!arh_s[i_arh]->err() &&
		!((b_tm < arh_s[i_arh]->begin() && e_tm < arh_s[i_arh]->begin()) ||
		   (b_tm > arh_s[i_arh]->end() && e_tm > arh_s[i_arh]->end())))
	    arh_s[i_arh]->get(b_tm, e_tm, mess, category, level, upTo);
    }
}

void ModMArch::checkArchivator( bool now )
{
    if(now || time(NULL) > mLstCheck + checkTm()*60)
    {
	struct stat file_stat;
	dirent scan_dirent, *scan_rez = NULL;

	DIR *IdDir = opendir(addr().c_str());
	if(IdDir == NULL)
	{
	    if(mkdir(addr().c_str(),0777))
		throw TError(nodePath().c_str(),_("Can not create directory '%s'."),addr().c_str());
	    IdDir = opendir(addr().c_str());
	}
	//Clean scan flag
	ResAlloc res(mRes, false);
	for(unsigned i_arh = 0; i_arh < arh_s.size(); i_arh++) arh_s[i_arh]->scan = false;
	res.release();

	while(readdir_r(IdDir,&scan_dirent,&scan_rez) == 0 && scan_rez)
	{
	    if(strcmp(scan_rez->d_name,"..") == 0 || strcmp(scan_rez->d_name,".") == 0) continue;
	    string NameArhFile = addr() + "/" + scan_rez->d_name;
	    stat(NameArhFile.c_str(), &file_stat);
	    if((file_stat.st_mode&S_IFMT) != S_IFREG || access(NameArhFile.c_str(),F_OK|R_OK) != 0) continue;
	    // Check for info files
	    if(NameArhFile.compare(NameArhFile.size()-4,4,".msg") != 0 && NameArhFile.compare(NameArhFile.size()-7,7,".msg.gz") != 0) continue;
	    // Check all files
	    unsigned i_arh;
	    res.request(false);
	    for(i_arh = 0; i_arh < arh_s.size(); i_arh++)
		if(arh_s[i_arh]->name() == NameArhFile)
		{
		    arh_s[i_arh]->scan = true;
		    res.release();
		    break;
		}
	    // Registre new archive file
	    if(i_arh >= arh_s.size())
	    {
		res.release();

		MFileArch *f_arh = new MFileArch(this);
		f_arh->attach(NameArhFile, false);
		f_arh->scan = true;

		res.request(true);
		//  Oldest and broken archives to down
		if(f_arh->err()) arh_s.push_back(f_arh);
		else
		{
		    for(i_arh = 0; i_arh < arh_s.size(); i_arh++)
			if(arh_s[i_arh]->err() || f_arh->begin() >= arh_s[i_arh]->begin())
			{
			    arh_s.insert(arh_s.begin()+i_arh, f_arh);
			    break;
			}
		    if(i_arh >= arh_s.size()) arh_s.push_back(f_arh);
		}
		res.release();
	    }
	}

	closedir(IdDir);

	//Check deleting Archives
	res.request(true);
	for(unsigned i_arh = 0; i_arh < arh_s.size(); )
	    if(!arh_s[i_arh]->scan)
	    {
		delete arh_s[i_arh];
		arh_s.erase(arh_s.begin() + i_arh);
	    }
	    else i_arh++;
	res.release();

	//Check file count and delete odd files
	if(mNumbFiles && !mod->noArchLimit)
	{
	    int f_cnt = 0;	//Work files number
	    for(unsigned i_arh = 0; i_arh < arh_s.size(); i_arh++)
		if(!arh_s[i_arh]->err()) f_cnt++;
	    if(f_cnt > mNumbFiles)
	    {
		// Delete oldest files
		for(int i_arh = arh_s.size()-1; i_arh >= 0; i_arh--)
		    if(f_cnt <= mNumbFiles)	break;
		    else if(!arh_s[i_arh]->err())
		    {
			string f_nm = arh_s[i_arh]->name();	//Deleted file name
			delete arh_s[i_arh];
			arh_s.erase(arh_s.begin() + i_arh);
			remove(f_nm.c_str());
			remove((f_nm+".info").c_str());
			f_cnt--;
		    }
	    }
	}
	mLstCheck = time(NULL);
    }

    //Call files checking
    ResAlloc res(mRes, false);
    for(unsigned i_arh = 0; i_arh < arh_s.size(); i_arh++)
	arh_s[i_arh]->check();
}

int ModMArch::size( )
{
    int rez = 0;
    ResAlloc res(mRes, false);
    for(unsigned i_arh = 0; i_arh < arh_s.size(); i_arh++)
	rez += arh_s[i_arh]->size();

    return rez;
}

void ModMArch::cntrCmdProc( XMLNode *opt )
{
    //Get page info
    if(opt->name() == "info")
    {
	TMArchivator::cntrCmdProc(opt);
	ctrMkNode("fld",opt,-1,"/prm/st/fsz",_("Archivator files size"),R_R_R_,"root",SARH_ID,1,"tp","str");
	ctrMkNode("fld",opt,-1,"/prm/st/tarch",_("Archiving time"),R_R_R_,"root",SARH_ID,1,"tp","str");
	ctrMkNode("fld",opt,-1,"/prm/cfg/ADDR",EVAL_STR,startStat()?R_R_R_:RWRWR_,"root",SARH_ID,3,
	    "dest","sel_ed","select","/prm/cfg/dirList","help",_("Path to directory for archivator's of messages files."));
	if(ctrMkNode("area",opt,-1,"/prm/add",_("Additional options"),R_R_R_,"root",SARH_ID))
	{
	    ctrMkNode("fld",opt,-1,"/prm/add/xml",_("XML archive files"),RWRWR_,"root",SARH_ID,2,"tp","bool","help",
		_("Enables archiving of messages in XML-format files, rather than plain text.\n"
		  "The use of XML-format archiving requires more RAM because is needed full download file,\n"
		  "XML-parsing and storing in memory at the time of use."));
	    ctrMkNode("fld",opt,-1,"/prm/add/sz",_("Maximum archive file size (kB)"),RWRWR_,"root",SARH_ID,2,"tp","dec","help",
		_("Sets a limit on the size of one archive file.\n"
		  "Disable the restriction can be by setting the parameter to zero."));
	    ctrMkNode("fld",opt,-1,"/prm/add/fl",_("Maximum files number"),RWRWR_,"root",SARH_ID,2,"tp","dec","help",
		_("Limits the maximum number of archive files and share with the size of single file\n"
		  "determines the size of archive on disk. Completely remove this restriction can be set to zero."));
	    ctrMkNode("fld",opt,-1,"/prm/add/len",_("File's time size (days)"),RWRWR_,"root",SARH_ID,2,"tp","dec","help",
		_("Sets a limit on the size of a single archive file on time."));
	    ctrMkNode("fld",opt,-1,"/prm/add/pcktm",_("Pack files timeout (min)"),RWRWR_,"root",SARH_ID,2,"tp","dec","help",
		_("Sets the time after which, in the absence of appeals, archive files will be packaged in gzip archiver.\n"
		 "Set to zero for disable packing by gzip."));
	    ctrMkNode("fld",opt,-1,"/prm/add/tm",_("Check archives period (min)"),RWRWR_,"root",SARH_ID,2,"tp","dec","help",
		_("Sets the frequency of checking the archives for the emergence or delete new files\n"
		  "in a directory of archives, as well as exceeding the limits and removing old archive files."));
	    ctrMkNode("fld",opt,-1,"/prm/add/pack_info_fl",_("Use info files for packed archives"),RWRWR_,"root",SARH_ID,2,"tp","bool","help",
		_("Specifies whether to create files with information about the packed archive files by gzip-archiver.\n"
		  "When copying files of archive on another station, these info files can speed up the target station\n"
		  "process of first run by eliminating the need to decompress by gzip-archives in order to obtain information."));
	    ctrMkNode("fld",opt,-1,"/prm/add/prev_dbl",_("Prevent duplicates"),RWRWR_,"root",SARH_ID,2,"tp","bool","help",
		_("Enables checks for duplicate messages at the time put a message in the archive.\n"
		  "If there is a duplicate the message does not fit into the archive.\n"
		  "This feature some increases the recording time to archive, but in cases of\n"
		  "placing messages in the archive by past time from external sources it allows to eliminate duplication."));
	    ctrMkNode("comm",opt,-1,"/prm/add/chk_nw",_("Check archivator directory now"),RWRW__,"root",SARH_ID,1,"help",
		_("The command, which allows you to immediately start checking the archives,\n"
		  "for example, after manual changes to the directory archiver."));
	}
	return;
    }

    //Process command to page
    string a_path = opt->attr("path");
    if(a_path == "/prm/cfg/dirList" && ctrChkNode(opt))		TSYS::ctrListFS(opt, addr());
    else if(a_path == "/prm/st/fsz" && ctrChkNode(opt))		opt->setText(TSYS::cpct2str(size()));
    else if(a_path == "/prm/st/tarch" && ctrChkNode(opt))	opt->setText(TSYS::time2str(tmCalc));
    else if(a_path == "/prm/add/xml")
    {
	if(ctrChkNode(opt,"get",RWRWR_,"root",SARH_ID,SEC_RD))	opt->setText(useXML() ? "1" : "0");
	if(ctrChkNode(opt,"set",RWRWR_,"root",SARH_ID,SEC_WR))	setUseXML(atoi(opt->text().c_str()));
    }
    else if(a_path == "/prm/add/sz")
    {
	if(ctrChkNode(opt,"get",RWRWR_,"root",SARH_ID,SEC_RD))	opt->setText(i2s(maxSize()));
	if(ctrChkNode(opt,"set",RWRWR_,"root",SARH_ID,SEC_WR))	setMaxSize(atoi(opt->text().c_str()));
    }
    else if(a_path == "/prm/add/fl")
    {
	if(ctrChkNode(opt,"get",RWRWR_,"root",SARH_ID,SEC_RD))	opt->setText(i2s(numbFiles()));
	if(ctrChkNode(opt,"set",RWRWR_,"root",SARH_ID,SEC_WR))	setNumbFiles(atoi(opt->text().c_str()));
    }
    else if(a_path == "/prm/add/len")
    {
	if(ctrChkNode(opt,"get",RWRWR_,"root",SARH_ID,SEC_RD))	opt->setText(i2s(timeSize()));
	if(ctrChkNode(opt,"set",RWRWR_,"root",SARH_ID,SEC_WR))	setTimeSize(atoi(opt->text().c_str()));
    }
    else if(a_path == "/prm/add/pcktm")
    {
	if(ctrChkNode(opt,"get",RWRWR_,"root",SARH_ID,SEC_RD))	opt->setText(i2s(packTm()));
	if(ctrChkNode(opt,"set",RWRWR_,"root",SARH_ID,SEC_WR))	setPackTm(atoi(opt->text().c_str()));
    }
    else if(a_path == "/prm/add/tm")
    {
	if(ctrChkNode(opt,"get",RWRWR_,"root",SARH_ID,SEC_RD))	opt->setText(i2s(checkTm()));
	if(ctrChkNode(opt,"set",RWRWR_,"root",SARH_ID,SEC_WR))	setCheckTm(atoi(opt->text().c_str()));
    }
    else if(a_path == "/prm/add/pack_info_fl")
    {
	if(ctrChkNode(opt,"get",RWRWR_,"root",SARH_ID,SEC_RD))	opt->setText(i2s(packInfoFiles()));
	if(ctrChkNode(opt,"set",RWRWR_,"root",SARH_ID,SEC_WR))	setPackInfoFiles(atoi(opt->text().c_str()));
    }
    else if(a_path == "/prm/add/prev_dbl")
    {
	if(ctrChkNode(opt,"get",RWRWR_,"root",SARH_ID,SEC_RD))	opt->setText(i2s(prevDbl()));
	if(ctrChkNode(opt,"set",RWRWR_,"root",SARH_ID,SEC_WR))	setPrevDbl(atoi(opt->text().c_str()));
    }
    else if(a_path == "/prm/add/chk_nw" && ctrChkNode(opt,"set",RWRW__,"root",SARH_ID,SEC_WR))	checkArchivator(true);
    else TMArchivator::cntrCmdProc(opt);
}

//*************************************************
//* FSArch::MFileArch - Messages archivator file  *
//*************************************************
MFileArch::MFileArch( ModMArch *owner ) :
    scan(false), mXML(true), mSize(0), mChars("UTF-8"), mErr(false), mWrite(false), mLoad(false), mPack(false),
    mBeg(0), mEnd(0), mNode(NULL), mOwner(owner)
{
    cach_pr.tm = cach_pr.off = 0;
    mAcces = time(NULL);
}

MFileArch::MFileArch( const string &iname, time_t ibeg, ModMArch *iowner, const string &icharset, bool ixml ) :
    scan(false), mName(iname), mXML(ixml), mSize(0), mChars(icharset), mErr(false), mWrite(false), mLoad(false), mPack(false),
    mBeg(ibeg), mEnd(ibeg), mNode(NULL), mOwner(iowner)
{
    cach_pr.tm = cach_pr.off = 0;

    int hd = open(name().c_str(), O_RDWR|O_CREAT|O_TRUNC, 0666);
    if(hd <= 0) throw TError(owner().nodePath().c_str(),_("Can not create file: '%s'!"),name().c_str());
    bool fOK = true;

    if(xmlM())
    {
	//Prepare XML file
	mChars = "UTF-8";
	mNode = new XMLNode();

	mNode->clear()->setName(mod->modId())->
	    setAttr("Version", mod->modInfo("Version"))->
	    setAttr("Begin", i2s(mBeg,TSYS::Hex))->
	    setAttr("End", i2s(mEnd,TSYS::Hex));
	string x_cf = "<?xml version=\"1.0\" encoding=\"UTF-8\" ?>" + mNode->save(XMLNode::BrOpenPrev);
	fOK = (write(hd,x_cf.c_str(),x_cf.size()) == (int)x_cf.size());
    }
    else
    {
	//Prepare plain text file
	char s_buf[STR_BUF_LEN];
	snprintf(s_buf, sizeof(s_buf), "%s %s %s %8x %8x\n",
	    mod->modId().c_str(), mod->modInfo("Version").c_str(), mChars.c_str(), (unsigned int)mBeg, (unsigned int)mEnd);
	fOK = (write(hd,s_buf,strlen(s_buf)) == (int)strlen(s_buf));
    }
    close(hd);
    if(!fOK) throw TError(owner().nodePath().c_str(), _("Write to file '%s' error"), name().c_str());

    mLoad = true;
    mAcces = time(NULL);
}

MFileArch::~MFileArch( )
{
    check();	//Check XML-archive

    if(mNode) delete mNode;
}

void MFileArch::attach( const string &iname, bool full )
{
    FILE *f = NULL;
    char buf[STR_BUF_LEN];
    ResAlloc res(mRes, true);

    mErr = false;
    mLoad = false;
    mName = iname;
    mPack = mod->filePack(name());
    mAcces = time(NULL);

    try
    {
	//Check archive and unpack if want
	if(mPack)
	{
	    bool infoOK = false;
	    // Get archive info from info file
	    int hd = open((name()+".info").c_str(), O_RDONLY);
	    if(hd > 0)
	    {
		int rsz = read(hd, buf, sizeof(buf));
		if(rsz > 0 && rsz < (int)sizeof(buf))
		{
		    buf[rsz] = 0;
		    char bChars[100];
		    if(sscanf(buf,"%lx %lx %99s %hhd",&mBeg,&mEnd,bChars,&mXML) == 4) { mChars = bChars; infoOK = true; }
		}
		close(hd);
	    }

	    // Get archive info from DB
	    if(!infoOK)
	    {
		TConfig c_el(&mod->packFE());
		c_el.cfg("FILE").setS(name());
		if(SYS->db().at().dataGet(mod->filesDB(),mod->nodePath()+"Pack/",c_el))
		{
		    mBeg = strtol(c_el.cfg("BEGIN").getS().c_str(),NULL,16);
		    mEnd = strtol(c_el.cfg("END").getS().c_str(),NULL,16);
		    mChars = c_el.cfg("PRM1").getS();
		    mXML = atoi(c_el.cfg("PRM2").getS().c_str());
		    infoOK = true;
		}
	    }

	    if(infoOK && (!mXML || (mXML && !full)))
	    {
		//  Get file size
		int hd = open(name().c_str(), O_RDONLY);
		if(hd > 0)	{ mSize = lseek(hd,0,SEEK_END); close(hd); }
		return;
	    }

	    try { mName = mod->unPackArch(name()); } catch(TError){ mErr = true; return; }
	    mPack = false;
	}

	f = fopen(name().c_str(),"r");
	if(f == NULL) { mErr = true; return; }

	char s_char[100];
	//Check to plain text archive
	if(fgets(buf,sizeof(buf),f) == NULL)
	    throw TError(owner().nodePath().c_str(),_("File '%s' header error!"),name().c_str());
	string s_tmpl = mod->modId()+"%*s %99s %x %x";
	if(sscanf(buf,s_tmpl.c_str(),s_char,&mBeg,&mEnd) == 3)
	{
	    // Attach plain text archive file
	    mChars = s_char;
	    mXML = false;
	    mLoad = true;
	    fseek(f,0,SEEK_END);
	    mSize = ftell(f);

	    // Delete Node tree
	    if(mNode) delete mNode;
	    mNode = NULL;

	    fclose(f);
	}
	else
	{
	    if(!mNode) mNode = new XMLNode();
	    fseek(f, 0, SEEK_SET);
	    if(full)
	    {
		// Load and parse all file
		int r_cnt;
		string s_buf;

		// Read full file to buffer
		while((r_cnt=fread(buf,1,sizeof(buf),f))) s_buf.append(buf,r_cnt);
		fclose(f); f = NULL;

		// Parse full file
		mNode->load(s_buf);
		if(mNode->name() != mod->modId())
		{
		    mess_err(owner().nodePath().c_str(),_("Archive file: '%s' is not mine."),name().c_str());
		    mNode->clear();
		    mErr = true;
		    return;
		}
		mSize = s_buf.size();
		mWrite = false;
		mChars = "UTF-8";
		mBeg = strtol(mNode->attr("Begin").c_str(), (char **)NULL, 16);
		mEnd = strtol(mNode->attr("End").c_str(), (char **)NULL, 16);
		mLoad = true;
		mXML = true;
		return;
	    }
	    else
	    {
		// Process only archive header
		int c;
		string prm, val;

		do
		{
		    while((c=fgetc(f)) != '<' && c != EOF);
		    if(c == EOF)
		    {
			mess_err(owner().nodePath().c_str(),_("Archive '%s' file error."),name().c_str());
			mErr = true;
			fclose(f);
			return;
		    }
		    prm.clear();
		    while((c=fgetc(f)) != ' ' && c != '\t' && c != '>' && c != EOF) prm += c;
		    if(c == EOF)
		    {
			mess_err(owner().nodePath().c_str(),_("Archive '%s' file error."),name().c_str());
			mErr = true;
			fclose(f);
			return;
		    }
		} while(prm != mod->modId());
		// Go to
		while(true)
		{
		    prm.clear();
		    val.clear();
		    while((c=fgetc(f)) == ' ' || c == '\t');
		    if(c == '>' || c == EOF) break;
		    while(c != '=' && c != '>' && c != EOF)	{ prm += c; c = fgetc(f); }
		    while((c=fgetc(f)) != '"' && c != '>' && c != EOF);
		    while((c=fgetc(f)) != '"' && c != '>' && c != EOF)  val += c;

		    if(prm == "Begin")		mBeg = strtol(val.c_str(),NULL,16);
		    else if(prm == "End")	mEnd = strtol(val.c_str(),NULL,16);
		}
		fseek(f, 0, SEEK_END);
		mSize = ftell(f);
		fclose(f);
		mWrite = false;
		mLoad = false;
		mXML = true;
		return;
	    }
	}
    }
    catch(TError err)
    {
	mess_err(err.cat.c_str(), "%s", err.mess.c_str());
	if(mNode) delete mNode;
	mNode = NULL;
	mErr = true;
	if(f)	fclose(f);
    }
}

void MFileArch::put( TMess::SRec mess )
{
    if(mErr) throw TError(owner().nodePath().c_str(),_("Inserting message to an error Archive file!"));

    ResAlloc res(mRes, true);

    if(mPack)
    {
	try { mName = mod->unPackArch(name()); } catch(TError err) { mErr = true; throw; }
	mPack = false;
    }

    mAcces = time(NULL);

    if(!mLoad)
    {
	res.release(); attach(mName); res.request(true);
	if(mErr || !mLoad)
	{
	    mErr = true;
	    throw TError(owner().nodePath().c_str(),_("Archive file '%s' isn't attached!"),mName.c_str());
	}
    }

    if(xmlM())
    {
	unsigned i_ch;
	for(i_ch = 0; i_ch < mNode->childSize(); i_ch++)
	{
	    XMLNode *xIt = mNode->childGet(i_ch);
	    long xTm = strtol(xIt->attr("tm").c_str(),(char **)NULL,16);
	    if(xTm > mess.time)	break;
	    else if(owner().prevDbl() && xTm == mess.time && atoi(xIt->attr("tmu").c_str()) == mess.utime &&
		    xIt->attr("cat") == mess.categ && xIt->text() == mess.mess)
		return;
	}

	XMLNode *cl_node = mNode->childIns(i_ch,"m");
	cl_node->setAttr("tm", i2s(mess.time,TSYS::Hex))->
		 setAttr("tmu", i2s(mess.utime))->
		 setAttr("lv", i2s(mess.level))->
		 setAttr("cat", mess.categ)->
		 setText(mess.mess);
	if(mess.time > mEnd)
	{
	    mEnd = mess.time;
	    mNode->setAttr("End",i2s(mEnd,TSYS::Hex));
	}
	mWrite = true;
    }
    else
    {
	unsigned int tTm;
	long mv_beg = 0;
	char buf[STR_BUF_LEN];
	//Check to empty category and message
	if(!mess.categ.size())	mess.categ = " ";
	if(!mess.mess.size())	mess.mess = " ";
	//Open file
	FILE *f = fopen(mName.c_str(), "r+");
	if(f == NULL) { mErr = true; return; }
	bool fOK = true;

	//Prepare mess
	string s_buf = TSYS::strMess("%x:%d %d %s %s", (unsigned int)mess.time, mess.utime, mess.level,
	    Mess->codeConvOut(mChars,TSYS::strEncode(mess.categ,TSYS::Custom," \n\t%")).c_str(),
	    Mess->codeConvOut(mChars,TSYS::strEncode(mess.mess,TSYS::Custom," \n\t%")).c_str())+"\n";

	//Check for duples
	if(mess.time <= mEnd && owner().prevDbl())
	{
	    long c_off = cacheGet(mess.time);
	    if(c_off) fseek(f,c_off,SEEK_SET);
	    else fOK = (fgets(buf,sizeof(buf),f) != NULL);

	    while((fOK=fOK&&(fgets(buf,sizeof(buf),f)!=NULL)))
	    {
		sscanf(buf, "%x %*d", &tTm);
		if((int)tTm > mess.time) { mv_beg = ftell(f)-strlen(buf); break; }
		if((int)tTm == mess.time && s_buf == buf) { fclose(f); return; }
	    }
	    fseek(f, 0, SEEK_SET);
	}

	//Put message to end
	if(fOK && (mess.time >= mEnd))
	{
	    //Update header
	    if(mess.time != mEnd)
	    {
		mEnd = mess.time;
		snprintf(buf, sizeof(buf), "%s %s %s %8x %8x\n",
		    mod->modId().c_str(), mod->modInfo("Version").c_str(), mChars.c_str(), (unsigned int)mBeg, (unsigned int)mEnd);
		fOK = (fwrite(buf,strlen(buf),1,f) == 1);
	    }
	    //Put mess to end file
	    fseek(f,0,SEEK_END);
	    fOK = fOK && (fwrite(s_buf.data(),s_buf.size(),1,f) == 1);
	}
	//Put message to inwards
	else
	{
	    if(fOK && !mv_beg)
	    {
		// Get want position
		long c_off = cacheGet(mess.time);
		if(c_off) fseek(f,c_off,SEEK_SET);
		else fOK = (fgets(buf,sizeof(buf),f) != NULL);

		// Check mess records
		int pass_cnt = 0;
		time_t last_tm = 0;
		while(!mv_beg && (fOK=fOK&&(fgets(buf,sizeof(buf),f)!=NULL)))
		{
		    sscanf(buf,"%x %*d",&tTm);
		    if((int)tTm > mess.time) mv_beg = ftell(f)-strlen(buf);
		    //  Add too big position to cache
		    else if((pass_cnt++) > CACHE_POS && (int)tTm != last_tm)
		    {
			cacheSet(tTm,ftell(f)-strlen(buf));
			pass_cnt = 0;
		    }
		    last_tm = tTm;
		}
	    }
	    if(fOK && mv_beg)
	    {
		fseek(f, 0, SEEK_END);
		int mv_end = ftell(f);
		int beg_cur;
		do
		{
		    beg_cur = ((mv_end-mv_beg) >= (int)sizeof(buf)) ? mv_end-sizeof(buf) : mv_beg;
		    fseek(f, beg_cur, SEEK_SET);
		    fOK = fOK && (fread(buf, mv_end-beg_cur,1,f) == 1);
		    fseek(f, beg_cur+s_buf.size(), SEEK_SET);
		    fOK = fOK && (fwrite(buf,mv_end-beg_cur,1,f) == 1);
		    mv_end -= sizeof(buf);
		}
		while(fOK && beg_cur != mv_beg);
		//  Write a new message
		fseek(f, mv_beg, SEEK_SET);
		fOK = fOK && (fwrite(s_buf.c_str(),s_buf.size(),1,f) == 1);
		cacheUpdate(mess.time, s_buf.size());
		//  Put last value to cache
		cacheSet(mess.time, mv_beg, true);
	    }
	}
	fseek(f, 0, SEEK_END);
	mSize = ftell(f);
	fclose(f);
	if(!fOK) mess_err(mod->nodePath().c_str(), _("File access to '%s' error."), mName.c_str());
    }
}

void MFileArch::get( time_t b_tm, time_t e_tm, vector<TMess::SRec> &mess, const string &category, char level, time_t upTo )
{
    TMess::SRec b_rec;

    if(mErr) throw TError(owner().nodePath().c_str(),_("Getting messages from an error Archive file!"));

    ResAlloc res(mRes, false);
    if(!upTo) upTo = time(NULL) + STD_INTERF_TM;

    if(mPack)
    {
	try { mName = mod->unPackArch(name()); } catch(TError err) { mErr = true; throw; }
	mPack = false;
    }

    mAcces = time(NULL);

    if(!mLoad)
    {
	res.release(); attach(mName); res.request(false);
	if(mErr || !mLoad) throw TError(owner().nodePath().c_str(),_("Archive file isn't attached!"));
    }

    TRegExp re(category, "p");

    if(xmlM())
    {
	for(unsigned i_ch = 0; i_ch < mNode->childSize() && time(NULL) < upTo; i_ch++)
	{
	    //Find messages
	    b_rec.time  = strtol(mNode->childGet(i_ch)->attr("tm").c_str(), (char**)NULL, 16);
	    b_rec.utime  = atoi(mNode->childGet(i_ch)->attr("tmu").c_str());
	    b_rec.categ = mNode->childGet(i_ch)->attr("cat");
	    b_rec.level = (TMess::Type)atoi(mNode->childGet(i_ch)->attr("lv").c_str());
	    b_rec.mess  = mNode->childGet(i_ch)->text();
	    if(b_rec.time >= b_tm && b_rec.time <= e_tm && abs(b_rec.level) >= level && re.test(b_rec.categ))
	    {
		bool equal = false;
		int i_p = mess.size();
		for(int i_m = mess.size()-1; i_m >= 0; i_m--)
		{
		    if(FTM(mess[i_m]) > FTM(b_rec)) i_p = i_m;
		    else if(FTM(mess[i_m]) == FTM(b_rec) && b_rec.level == mess[i_m].level && b_rec.mess == mess[i_m].mess)
		    { equal = true; break; }
		    else if(FTM(mess[i_m]) < FTM(b_rec)) break;
		}
		if(!equal) mess.insert(mess.begin()+i_p, b_rec);
	    }
	}
    }
    else
    {
	char buf[STR_BUF_LEN];
	//Open file
	FILE *f = fopen(mName.c_str(), "r");
	if(f == NULL) { mErr = true; return; }

	//Get want position
	bool rdOK = true;
	long c_off = cacheGet(b_tm);
	if(c_off) fseek(f, c_off, SEEK_SET);
	else rdOK = (fgets(buf,sizeof(buf),f) != NULL);
	//Check mess records
	int pass_cnt = 0;
	time_t last_tm = 0;
	while((rdOK=(fgets(buf,sizeof(buf),f)!=NULL)) && time(NULL) < upTo)
	{
	    char stm[51]; int off = 0, bLev;
	    sscanf(buf, "%50s %d", stm, &bLev);
	    b_rec.level = (TMess::Type)bLev;
	    b_rec.time = strtol(TSYS::strSepParse(stm,0,':',&off).c_str(),NULL,16);
	    b_rec.utime = atoi(TSYS::strSepParse(stm,0,':',&off).c_str());
	    if(b_rec.time >= e_tm) break;
	    if(b_rec.time >= b_tm)
	    {
		if(abs(b_rec.level) < level) continue;
		char m_cat[1001], m_mess[1001];
		sscanf(buf,"%*x:%*d %*d %1000s %1000s",m_cat,m_mess);
		b_rec.categ = TSYS::strDecode(Mess->codeConvIn(mChars,m_cat),TSYS::HttpURL);
		b_rec.mess  = TSYS::strDecode(Mess->codeConvIn(mChars,m_mess),TSYS::HttpURL);
		if(!re.test(b_rec.categ)) continue;
		// Check to equal messages and inserting
		bool equal = false;
		int i_p = mess.size();
		for(int i_m = mess.size()-1; i_m >= 0; i_m--)
		    if(FTM(mess[i_m]) > FTM(b_rec)) i_p = i_m;
		    else if(FTM(mess[i_m]) == FTM(b_rec) && b_rec.level == mess[i_m].level && b_rec.mess == mess[i_m].mess)
		    { equal = true; break; }
		    else if(FTM(mess[i_m]) < FTM(b_rec)) break;
		if(!equal) mess.insert(mess.begin()+i_p,b_rec);
	    }
	    else if((pass_cnt++) > CACHE_POS && b_rec.time != last_tm)
	    {
		cacheSet(b_rec.time, ftell(f)-strlen(buf));
		pass_cnt = 0;
	    }
	    last_tm = b_rec.time;
	}
	fclose(f);
    }
}

void MFileArch::check( bool free )
{
    ResAlloc res(mRes, true);
    if(!mErr && mLoad && xmlM())
    {
	if(mWrite)
	{
	    int hd = open(mName.c_str(), O_RDWR|O_TRUNC);
	    if(hd > 0)
	    {
		string x_cf = mNode->save(XMLNode::XMLHeader|XMLNode::BrOpenPrev);
		mSize = x_cf.size();
		mWrite = !(write(hd,x_cf.c_str(),mSize) == mSize);
		if(mWrite) mess_err(mod->nodePath().c_str(), _("Write to '%s' error!"), mName.c_str());
		close(hd);
	    }
	}
	//Free memory of XML-archive after 10 minets
	if(time(NULL) > mAcces + owner().packTm()*30 || free)
	{
	    mNode->clear();
	    mLoad = false;
	}
    }
    //Check for pack archive file
    if(!mErr && !mPack && owner().packTm() && time(NULL) > (mAcces+owner().packTm()*60) && ((xmlM() && !mLoad) || !xmlM()))
    {
	mName = mod->packArch(name());
	mPack = true;
	// Get file size
	int hd = open(name().c_str(), O_RDONLY);
	if(hd > 0) { mSize = lseek(hd, 0, SEEK_END); close(hd); }

	if(!owner().packInfoFiles())
	{
	    // Write info to DB
	    TConfig c_el(&mod->packFE());
	    c_el.cfg("FILE").setS(name());
	    c_el.cfg("BEGIN").setS(ll2s(begin(),TSYS::Hex));
	    c_el.cfg("END").setS(ll2s(end(),TSYS::Hex));
	    c_el.cfg("PRM1").setS(charset());
	    c_el.cfg("PRM2").setS(i2s(xmlM()));
	    SYS->db().at().dataSet(mod->filesDB(),mod->nodePath()+"Pack/",c_el);
	}
	else if((hd=open((name()+".info").c_str(),O_WRONLY|O_CREAT|O_TRUNC,0666)) > 0)
	{
	    // Write info to info file
	    string si = TSYS::strMess("%lx %lx %s %d",begin(),end(),charset().c_str(),xmlM());
	    if(write(hd,si.data(),si.size()) != (int)si.size())
		mess_err(mod->nodePath().c_str(), _("Write to '%s' error!"), (name()+".info").c_str());
	    close(hd);
	}
    }
}

long MFileArch::cacheGet( time_t tm )
{
    CacheEl rez = {0,0};
    for(int i_c = cache.size()-1; i_c >= 0; i_c--)
	if(tm >= cache[i_c].tm) { rez = cache[i_c]; break; }
    if(tm >= cach_pr.tm && cach_pr.tm >= rez.tm) rez = cach_pr;

    return rez.off;
}

void MFileArch::cacheSet( time_t tm, long off, bool last )
{
    CacheEl el = { tm, off };

    if(!last)
    {
	for(unsigned i_c = 0; i_c < cache.size(); i_c++)
	    if(el.tm == cache[i_c].tm)		{ cache[i_c] = el; return; }
	    else if(el.tm < cache[i_c].tm)	{ cache.insert(cache.begin()+i_c,el); return; }
	cache.push_back(el);
    }
    else cach_pr = el;
}

void MFileArch::cacheUpdate( time_t tm, long v_add )
{
    for(unsigned i_c = 0; i_c < cache.size(); i_c++)
	if(cache[i_c].tm > tm) cache[i_c].off += v_add;
    if(cach_pr.tm > tm) cach_pr.off += v_add;
}
