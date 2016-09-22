
//OpenSCADA system file: tsys.cpp
/***************************************************************************
 *   Copyright (C) 2003-2016 by Roman Savochenko, <rom_as@oscada.org>      *
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

#include <features.h>
#include <byteswap.h>
#include <ieee754.h>
#include <syscall.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/stat.h>
#include <sys/time.h>
#include <sys/utsname.h>
#include <sys/resource.h>
#include <dirent.h>
#include <errno.h>
#include <string.h>
#include <fcntl.h>
#include <stddef.h>
#include <unistd.h>
#include <stdio.h>
#include <stdarg.h>
#include <stdlib.h>
#include <langinfo.h>
#include <zlib.h>

#include <algorithm>

#include "terror.h"
#include "tmess.h"
#include "tsys.h"

using namespace OSCADA;

//Continuously access variable
TMess	*OSCADA::Mess = NULL;
TSYS	*OSCADA::SYS = NULL;
bool TSYS::finalKill = false;
pthread_key_t TSYS::sTaskKey;

TSYS::TSYS( int argi, char ** argb, char **env ) : argc(argi), argv((const char **)argb), envp((const char **)env),
    mUser("root"), mConfFile(sysconfdir_full"/oscada.xml"), mId("EmptySt"), mName(_("Empty Station")),
    mModDir(oscd_moddir_full), mIcoDir("icons;"oscd_datadir_full"/icons"), mDocDir("docs;"oscd_datadir_full"/docs"),
    mWorkDB(DB_CFG), mSaveAtExit(false), mSavePeriod(0), rootModifCnt(0), sysModifFlgs(0), mStopSignal(-1), mN_CPU(1),
    mainPthr(0), mSysTm(time(NULL)), mClockRT(false), mRdStLevel(0), mRdRestConnTm(10), mRdTaskPer(1), mRdPrcTm(0), mRdPrimCmdTr(false)
{
    finalKill = false;
    SYS = this;		//Init global access value
    mSubst = grpAdd("sub_",true);
    nodeEn();
    mainPthr = pthread_self();
    pthread_key_create(&sTaskKey, NULL);

    Mess = new TMess();

    if(getenv("USER")) mUser = getenv("USER");

    //Init system clock
    clkCalc();

#if __GLIBC_PREREQ(2,4)
    //Multi CPU allow check
    cpu_set_t cpuset;
    CPU_ZERO(&cpuset);
    pthread_getaffinity_np(mainPthr, sizeof(cpu_set_t), &cpuset);
#if __GLIBC_PREREQ(2,6)
    mN_CPU = CPU_COUNT(&cpuset);
#endif
#endif

    //Set signal handlers
    struct sigaction sHdr;
    memset(&sHdr, 0, sizeof(sHdr));
    sHdr.sa_sigaction = sighandler;
    sHdr.sa_flags = SA_SIGINFO;
    sigaction(SIGINT, &sHdr, &sigActOrig);
    sigaction(SIGTERM, &sHdr, &sigActOrig);
    //sigaction(SIGCHLD, &sHdr, &sigActOrig);
    sigaction(SIGALRM, &sHdr, &sigActOrig);
    sigaction(SIGPIPE, &sHdr, &sigActOrig);
    //sigaction(SIGFPE, &sHdr, &sigActOrig);
    //sigaction(SIGSEGV, &sHdr, &sigActOrig);
    sigaction(SIGABRT, &sHdr, &sigActOrig);
    sigaction(SIGUSR1, &sHdr, &sigActOrig);
}

TSYS::~TSYS( )
{
    int mLev = Mess->messLevel();
    finalKill = true;

    //Delete all nodes in order
    del("ModSched");
    del("UI");
    del("Special");
    del("Archive");
    del("DAQ");
    del("Protocol");
    del("Transport");
    del("Security");
    del("BD");

    delete Mess;
    pthread_key_delete(sTaskKey);

    if(mLev == TMess::Debug) {
	string cntrsStr;
	dataRes().lock();
	for(map<string,double>::iterator icnt = mCntrs.begin(); icnt != mCntrs.end(); icnt++)
	    cntrsStr += TSYS::strMess("%s: %g\n",icnt->first.c_str(),icnt->second);
	dataRes().unlock();
	printf(_("System counters on exit: %s"), cntrsStr.c_str());
    }

    //Signal handlers restore
    sigaction(SIGINT, &sigActOrig, NULL);
    sigaction(SIGTERM, &sigActOrig, NULL);
    //sigaction(SIGCHLD, &sigActOrig, NULL);
    sigaction(SIGALRM, &sigActOrig, NULL);
    sigaction(SIGPIPE, &sigActOrig, NULL);
    //sigaction(SIGFPE, &sigActOrig, NULL);
    //sigaction(SIGSEGV, &sigActOrig, NULL);
    sigaction(SIGABRT, &sigActOrig, NULL);
    sigaction(SIGUSR1, &sigActOrig, NULL);
}

string TSYS::host( )
{
    utsname ubuf; uname(&ubuf);
    return ubuf.nodename;
}

string TSYS::workDir( )
{
    char buf[STR_BUF_LEN];
    return getcwd(buf,sizeof(buf));
}

void TSYS::setWorkDir( const string &wdir, bool init )
{
    if(wdir.empty() || workDir() == wdir) return;
    if(chdir(wdir.c_str()) != 0)
	mess_sys(TMess::Warning, _("Changing work directory to '%s' error: %s. Perhaps current directory already set correct to '%s'."),
	    wdir.c_str(), strerror(errno),workDir().c_str());
    else if(init) sysModifFlgs &= ~MDF_WorkDir;
    else { sysModifFlgs |= MDF_WorkDir; modif(); }
}

void TSYS::setIcoDir( const string &idir, bool init )
{
    mIcoDir = idir;
    if(init) sysModifFlgs &= ~MDF_IcoDir;
    else { sysModifFlgs |= MDF_IcoDir; modif(); }
}

void TSYS::setDocDir( const string &idir, bool init )
{
    mDocDir = idir;
    if(init) sysModifFlgs &= ~MDF_DocDir;
    else { sysModifFlgs |= MDF_DocDir; modif(); }
}

void TSYS::setModDir( const string &mdir, bool init )
{
    mModDir = mdir;
    if(init) sysModifFlgs &= ~MDF_ModDir;
    else { sysModifFlgs |= MDF_ModDir; modif(); }
}

XMLNode *TSYS::cfgNode( const string &path, bool create )
{
    string s_el, ndNm;

    XMLNode *t_node = &rootN;
    if(t_node->name() != "OpenSCADA") {
	if(!create) return NULL;
	t_node->setName("OpenSCADA");
    }

    for(int l_off = 0, nLev = 0; true; nLev++) {
	s_el = TSYS::pathLev(path, 0, true, &l_off);
	if(s_el.empty()) return t_node;
	bool ok = false;
	for(unsigned i_f = 0; !ok && i_f < t_node->childSize(); i_f++)
	    if(strcasecmp(t_node->childGet(i_f)->attr("id").c_str(),s_el.c_str()) == 0) {
		t_node = t_node->childGet(i_f);
		ok = true;
	    }
	if(!ok) {
	    if(!create)	return NULL;
	    ndNm = "prm";
	    switch(nLev) {
		case 0: ndNm = "station";	break;
		case 1: if(s_el.compare(0,4,"sub_") == 0) ndNm = "node";	break;
		case 2: if(s_el.compare(0,4,"mod_") == 0) ndNm = "node";	break;
	    }
	    if(ndNm == "prm") t_node = t_node->childIns(0,ndNm)->setAttr("id",s_el);
	    else t_node = t_node->childAdd(ndNm)->setAttr("id",s_el);
        }
    }
    return t_node;
}

void TSYS::modifCfg( bool chkPossibleWR )
{
    if(chkPossibleWR) {
	//Check config file for readonly
	if(access(mConfFile.c_str(),F_OK|W_OK) != 0)
	    throw err_sys(_("Read only access to file '%s'."), mConfFile.c_str());
    }
    else rootModifCnt++;
}

string TSYS::int2str( int val, TSYS::IntView view )
{
    char buf[NSTR_BUF_LEN];
    switch(view) {
	case TSYS::Oct:	snprintf(buf, sizeof(buf), "%o", (unsigned)val);	break;
	case TSYS::Hex:	snprintf(buf, sizeof(buf), "%x", (unsigned)val);	break;
	default: snprintf(buf, sizeof(buf), "%d", val);				break;
    }
    return buf;
}

string TSYS::uint2str( unsigned val, IntView view )
{
    char buf[NSTR_BUF_LEN];
    switch(view) {
	case TSYS::Oct:	snprintf(buf, sizeof(buf), "%o", val);	break;
	case TSYS::Hex:	snprintf(buf, sizeof(buf), "%x", val);	break;
	default: snprintf(buf, sizeof(buf), "%u", val);		break;
    }
    return buf;
}

string TSYS::ll2str( long long val, IntView view )
{
    char buf[NSTR_BUF_LEN];
    switch(view) {
	case TSYS::Oct:	snprintf(buf, sizeof(buf), "%llo", (long long unsigned int)val);	break;
	case TSYS::Hex:	snprintf(buf, sizeof(buf), "%llx", (long long unsigned int)val);	break;
	default: snprintf(buf, sizeof(buf), "%lld", (long long int)val);			break;
    }
    return buf;
}

string TSYS::real2str( double val, int prec, char tp )
{
    char buf[NSTR_BUF_LEN];
    prec = vmax(0, prec);
    switch(tp) {
	case 'g': snprintf(buf, sizeof(buf), "%.*g", prec, val);	break;
	case 'e': snprintf(buf, sizeof(buf), "%.*e", prec, val);	break;
	default: snprintf(buf, sizeof(buf), "%.*f", prec, val);		break;
    }
    return buf;
}

string TSYS::atime2str( time_t itm, const string &format )
{
    struct tm tm_tm;
    localtime_r(&itm, &tm_tm);
    char buf[100];
    int ret = strftime(buf, sizeof(buf), format.empty()?"%d-%m-%Y %H:%M:%S":format.c_str(), &tm_tm);
    return (ret > 0) ? string(buf,ret) : string("");
}

string TSYS::time2str( double tm )
{
    if(tm < 1e-12) return "0";
    int lev = 0;
    int days = (int)floor(tm/(24*60*60));
    int hours = (int)floor(tm/(60*60))%24;
    int mins = (int)floor(tm/(60))%60;
    double usec = 1e6 * (tm - days*24*60*60 - hours*60*60 - mins*60);

    string rez;
    if(days)		{ rez += i2s(days)+_("day"); lev = vmax(lev,6); }
    if(hours)		{ rez += (rez.size()?" ":"")+i2s(hours)+_("hour"); lev = vmax(lev,5); }
    if(mins && lev < 6)	{ rez += (rez.size()?" ":"")+i2s(mins)+_("min"); lev = vmax(lev,4); }
    if((1e-6*usec) > 0.5 && lev < 5)	{ rez += (rez.size()?" ":"")+r2s(1e-6*usec,3)+_("s"); lev = vmax(lev,3); }
    else if((1e-3*usec) > 0.5 && !lev)	{ rez += (rez.size()?" ":"")+r2s(1e-3*usec,4)+_("ms"); lev = vmax(lev,2); }
    else if(usec > 0.5 && !lev)		{ rez += (rez.size()?" ":"")+r2s(usec,4)+_("us"); lev = vmax(lev,1); }
    else if(!lev)	rez += (rez.size()?" ":"")+r2s(1e3*usec,4)+_("ns");

    return rez;
}

string TSYS::cpct2str( double cnt )
{
    if(cnt > 0.2*pow(2,80))	return r2s(cnt/pow(2,80),3,'g')+_("YiB");
    if(cnt > 0.2*pow(2,70))	return r2s(cnt/pow(2,70),3,'g')+_("ZiB");
    if(cnt > 0.2*pow(2,60))	return r2s(cnt/pow(2,60),3,'g')+_("EiB");
    if(cnt > 0.2*pow(2,50))	return r2s(cnt/pow(2,50),3,'g')+_("PiB");
    if(cnt > 0.2*pow(2,40))	return r2s(cnt/pow(2,40),3,'g')+_("TiB");
    if(cnt > 0.2*pow(2,30))	return r2s(cnt/pow(2,30),3,'g')+_("GiB");
    if(cnt > 0.2*pow(2,20))	return r2s(cnt/pow(2,20),3,'g')+_("MiB");
    if(cnt > 0.2*pow(2,10))	return r2s(cnt/pow(2,10),3,'g')+_("KiB");
    return r2s(cnt,3,'g')+_("B");
}

string TSYS::addr2str( void *addr )
{
    char buf[sizeof(void*)*2+3];
    snprintf(buf,sizeof(buf),"%p",addr);

    return buf;
}

void *TSYS::str2addr( const string &str )	{ return (void *)strtoul(str.c_str(),NULL,16); }

string TSYS::strNoSpace( const string &val )
{
    int beg = -1, end = -1;

    for(unsigned i_s = 0; i_s < val.size(); i_s++)
	if(val[i_s] != ' ' && val[i_s] != '\n' && val[i_s] != '\t') {
	    if(beg < 0) beg = i_s;
	    end = i_s;
	}

    return (beg>=0) ? val.substr(beg,end-beg+1) : "";
}

string TSYS::strMess( const char *fmt, ... )
{
    char str[STR_BUF_LEN];
    va_list argptr;

    va_start(argptr, fmt);
    vsnprintf(str, sizeof(str), fmt, argptr);
    va_end(argptr);

    return str;
}

string TSYS::strMess( unsigned len, const char *fmt, ... )
{
    if(len <= 0) return "";

    char str[len];
    va_list argptr;

    va_start(argptr, fmt);
    int lenRez = vsnprintf(str, sizeof(str), fmt, argptr);
    va_end(argptr);

    return (lenRez < (int)len) ? string(str) : string(str)+"...";
}

string TSYS::strLabEnum( const string &base )
{
    //Get number from end
    unsigned numbDig = base.size(), numbXDig = base.size();
    bool noDig = false;
    for(int i_c = base.size()-1; i_c >= 0; i_c--) {
	if(!noDig && isdigit(base[i_c])) numbDig = i_c; else noDig = true;
	if(!(isxdigit(base[i_c]) || (i_c && strncasecmp(base.c_str()+i_c-1,"0x",2) == 0))) break;
	else if(!isxdigit(base[i_c])) { numbXDig = i_c-1; break; }
    }

    //Process number and increment
    if(numbXDig < numbDig && (base.size()-numbXDig) > 2 && strncasecmp(base.c_str()+numbXDig,"0x",2) == 0)
	return base.substr(0, numbXDig) + "0x" + i2s(strtol(base.c_str()+numbXDig,NULL,16)+1, TSYS::Hex);
    if((base.size()-numbDig) > 1 && base[numbDig] == '0')
	return base.substr(0, numbDig) + "0" + i2s(strtol(base.c_str()+numbDig,NULL,8)+1, TSYS::Oct);
    return base.substr(0, numbDig) + i2s(strtol(base.c_str()+numbDig,NULL,0)+1);
}

string TSYS::optDescr( )
{
    utsname buf;
    uname(&buf);
    return TSYS::strMess(_(
	"***************************************************************************\n"
	"********** %s v%s (%s-%s). *********\n"
	"***************************************************************************\n\n"
	"===========================================================================\n"
	"==================== Generic options ======================================\n"
	"===========================================================================\n"
	"-h, --help		Info message about the system options.\n"
	"    --config=<file>	The station configuration file.\n"
	"    --station=<id>	The station identifier.\n"
	"    --statName=<name>	The station name.\n"
	"    --demon, --daemon	Start into the daemon mode.\n"
	"    --pidFile=<file>	The file for the programm process ID place here.\n"
	"    --coreDumpAllow	Set the limits for a core dump creation allow on the crash.\n"
	"    --messLev=<level>	Process messages <level> (0-7).\n"
	"    --log=<direct>	Direct messages to, by bitfield:\n"
	"			  0x1 - syslogd;\n"
	"			  0x2 - stdout;\n"
	"			  0x4 - stderr;\n"
	"			  0x8 - the messages archive.\n"
	"----------- The config-file station '%s' parameters -----------\n"
	"StName     <nm>	Station name.\n"
	"WorkDB     <Type.Name> Work DB (type and name).\n"
	"WorkDir    <path>	Work directory.\n"
	"ModDir     <path>	Modules directory.\n"
	"IcoDir     <path>	Icons directory.\n"
	"DocDir     <path>	Documents directory.\n"
	"MessLev    <level>     Messages <level> (0-7).\n"
	"SelDebCats <list>	Debug categories list (separated by ';').\n"
	"LogTarget  <direction> Direct messages to, by bitfield:\n"
	"			  0x1 - syslogd;\n"
	"			  0x2 - stdout;\n"
	"			  0x4 - stderr;\n"
	"			  0x8 - the messages archive.\n"
	"Lang       <lang>	Work-internal language, like \"en_US.UTF-8\".\n"
	"Lang2CodeBase <lang>	Base language for variable texts translation, two symbols code.\n"
	"MainCPUs   <list>	Main used CPUs list (separated by ':').\n"
	"ClockRT    <false>	Set for use REALTIME (else MONOTONIC) clock, some problematic with the system clock modification.\n"
	"SaveAtExit <true>	Save the system at exit.\n"
	"SavePeriod <sec>	Save the system period.\n"
	"RdStLevel  <lev>	Level of redundancy current station.\n"
	"RdTaskPer  <s>		Call period of the redundant task.\n"
	"RdRestConnTm <s>	Restore connection timeout of try to the \"dead\" reserve stations.\n"
	"RdStList   <list>	Redundant stations list, separated symbol ';' (st1;st2).\n"
	"RdPrimCmdTr <0|1>	Enable the primary commands transfering to the redundant stations.\n\n"),
	PACKAGE_NAME,VERSION,buf.sysname,buf.release,nodePath().c_str());
}

string TSYS::getCmdOpt( int &curPos, string *argVal )	{ return getCmdOpt_(curPos, argVal, argc, (char **)argv); }

string TSYS::getCmdOpt_( int &curPos, string *argVal, int argc, char **argv )
{
    size_t fPos;
    int argI = curPos&0xFF;
    int argIsh = (curPos>>8)&0xFF;
    if(argI >= argc) return "";
    for(int argLen = 0; argI < argc; argI++, argIsh = 0) {
	argLen = strlen(argv[argI]);
	if(argLen < 2 || argv[argI][0] != '-') continue;
	//Check for long: "--var", "--var=val" or "--var val"
	if(argv[argI][1] == '-') {
	    curPos = argI+1;
	    string rez = string(argv[argI]+2);
	    if((fPos=rez.find("=")) != string::npos) {
		if(argVal) *argVal = rez.substr(fPos+1);
		return rez.substr(0,fPos);
	    }
	    if(argVal) *argVal = ((argI+1) < argc && argv[argI+1][0] != '-') ? argv[argI+1] : "";
	    return rez;
	}
	//Check for short: "-v", "-v val", "-abcv" or "-abcv val"
	else {
	    if((argIsh+1) >= argLen) continue;
	    curPos = argI+((argIsh+1)<<8);
	    if(argVal) *argVal = ((argIsh+2) == argLen && (argI+1) < argc && argv[argI+1][0] != '-') ? argv[argI+1] : "";
	    return string(argv[argI]+argIsh+1,1);
	}
    }

    return "";
}

bool TSYS::cfgFileLoad( )
{
    bool cmd_help = false;

    //================ Load parameters from commandline =========================
    string argCom, argVl;
    for(int argPos = 0; (argCom=getCmdOpt(argPos,&argVl)).size(); )
	if(strcasecmp(argCom.c_str(),"h") == 0 || strcasecmp(argCom.c_str(),"help") == 0) {
	    fprintf(stdout,"%s",optDescr().c_str());
	    Mess->setMessLevel(7);
	    cmd_help = true;
	}
	else if(strcasecmp(argCom.c_str(),"config") == 0)	mConfFile = argVl;
	else if(strcasecmp(argCom.c_str(),"station") == 0)	mId = argVl;
	else if(strcasecmp(argCom.c_str(),"statname") == 0)	mName = argVl;

    //Load config-file
    int hd = open(mConfFile.c_str(), O_RDONLY);
    if(hd < 0) mess_sys(TMess::Error, _("Config-file '%s' error: %s"), mConfFile.c_str(), strerror(errno));
    else {
	bool fOK = true;
	string s_buf;
	int cf_sz = lseek(hd, 0, SEEK_END);
	if(cf_sz > 0) {
	    lseek(hd, 0, SEEK_SET);
	    char buf[STR_BUF_LEN];
	    for(int len = 0; (len=read(hd,buf,sizeof(buf))) > 0; ) s_buf.append(buf, len);
	    fOK = s_buf.size();
	}
	close(hd);
	if(!fOK) mess_sys(TMess::Error, _("Config-file '%s' load error."), mConfFile.c_str());

	try {
	    ResAlloc res(cfgRes(), true);
	    rootN.load(s_buf, true);
	    if(rootN.name() == "OpenSCADA") {
		XMLNode *stat_n = NULL;
		for(int i_st = rootN.childSize()-1; i_st >= 0; i_st--)
		    if(rootN.childGet(i_st)->name() == "station") {
			stat_n = rootN.childGet(i_st);
			if(stat_n->attr("id") == mId) break;
		    }
		if(stat_n && stat_n->attr("id") != mId) {
		    if(mId != "EmptySt")
			mess_sys(TMess::Warning, _("Station '%s' is not present in the config-file. Using configuration for station '%s'!"),
			    mId.c_str(), stat_n->attr("id").c_str());
		    mId	= stat_n->attr("id");
		}
		if(!stat_n)	rootN.clear();
	    }
	    else rootN.clear();
	    if(!rootN.childSize()) mess_sys(TMess::Error, _("Configuration '%s' error!"), mConfFile.c_str());
	    rootModifCnt = 0;
	} catch(TError &err) { mess_sys(TMess::Error, _("Load config-file error: %s"), err.mess.c_str()); }
    }

    return cmd_help;
}

void TSYS::cfgFileSave( )
{
    ResAlloc res(cfgRes(), true);
    if(!rootModifCnt) return;
    int hd = open(mConfFile.c_str(), O_CREAT|O_TRUNC|O_WRONLY, 0664);
    if(hd < 0) mess_sys(TMess::Error, _("Config-file '%s' error: %s"), mConfFile.c_str(), strerror(errno));
    else {
	string rezFile = rootN.save(XMLNode::XMLHeader);
	int rez = write(hd, rezFile.data(), rezFile.size());
	if(rez != (int)rezFile.size()) mess_sys(TMess::Error,_("Configuration '%s' write error. %s"), mConfFile.c_str(), ((rez<0)?strerror(errno):""));
	rootModifCnt = 0;
	rootFlTm = time(NULL);
	close(hd);
    }
}

void TSYS::cfgPrmLoad( )
{
    //System parameters
    mName = TBDS::genDBGet(nodePath()+"StName",name(),"root",TBDS::UseTranslate);
    mWorkDB = TBDS::genDBGet(nodePath()+"WorkDB",workDB(),"root",TBDS::OnlyCfg);
    setWorkDir(TBDS::genDBGet(nodePath()+"Workdir","","root",TBDS::OnlyCfg).c_str(), true);
    setModDir(TBDS::genDBGet(nodePath()+"ModDir",modDir(),"root",TBDS::OnlyCfg), true);
    setIcoDir(TBDS::genDBGet(nodePath()+"IcoDir",icoDir(),"root",TBDS::OnlyCfg), true);
    setDocDir(TBDS::genDBGet(nodePath()+"DocDir",docDir(),"root",TBDS::OnlyCfg), true);
    setMainCPUs(TBDS::genDBGet(nodePath()+"MainCPUs",mainCPUs()));
    setClockRT(s2i(TBDS::genDBGet(nodePath()+"ClockRT",i2s(clockRT()))));
    setSaveAtExit(s2i(TBDS::genDBGet(nodePath()+"SaveAtExit","0")));
    setSavePeriod(s2i(TBDS::genDBGet(nodePath()+"SavePeriod","0")));

    //Redundancy parameters
    setRdStLevel(s2i(TBDS::genDBGet(nodePath()+"RdStLevel",i2s(rdStLevel()))));
    setRdTaskPer(s2r(TBDS::genDBGet(nodePath()+"RdTaskPer",r2s(rdTaskPer()))));
    setRdRestConnTm(s2i(TBDS::genDBGet(nodePath()+"RdRestConnTm",i2s(rdRestConnTm()))));
    setRdPrimCmdTr(s2i(TBDS::genDBGet(nodePath()+"RdPrimCmdTr",i2s(rdPrimCmdTr()))));
    string stLs = TBDS::genDBGet(nodePath()+"RdStList"), stId;
    mRdRes.lock(true);
    for(int off = 0; (stId=TSYS::strSepParse(stLs,0,';',&off)).size(); )
	if(mSt.find(stId) == mSt.end()) mSt[stId] = SStat();
    mRdRes.unlock();
}

void TSYS::load_( )
{
    static bool first_load = true;

    bool cmd_help = cfgFileLoad();
    mess_sys(TMess::Info, _("Load!"));
    cfgPrmLoad();
    Mess->load();	//Messages load

    if(first_load) {
	//Create subsystems
	add(new TBDS());
	add(new TSecurity());
	add(new TTransportS());
	add(new TProtocolS());
	add(new TDAQS());
	add(new TArchiveS());
	add(new TSpecialS());
	add(new TUIS());
	add(new TModSchedul());

	//Load modules
	modSchedul().at().load();
	if(!modSchedul().at().loadLibS()) {
	    mess_sys(TMess::Error, _("No one module is loaded. Your configuration is broken!"));
	    stop();
	}

	//First DB subsystem load
	db().at().load();
	if(!cmd_help) modSchedul().at().modifG();	// For try reload from DB

	//Second load for load from generic DB
	Mess->load();
	cfgPrmLoad();
    }

    //Direct load subsystems and modules
    vector<string> lst;
    list(lst);
    for(unsigned i_a = 0; i_a < lst.size(); i_a++)
	try { at(lst[i_a]).at().load(); }
	catch(TError &err) {
	    mess_err(err.cat.c_str(), "%s", err.mess.c_str());
	    mess_sys(TMess::Error, _("Error load subsystem '%s'."), lst[i_a].c_str());
	}

    if(cmd_help) stop();
    first_load = false;
}

void TSYS::save_( )
{
    mess_sys(TMess::Info, _("Save!"));

    //System parameters
    TBDS::genDBSet(nodePath()+"StName", mName, "root", TBDS::UseTranslate);
    TBDS::genDBSet(nodePath()+"WorkDB", workDB(), "root", TBDS::OnlyCfg);
    if(sysModifFlgs&MDF_WorkDir)TBDS::genDBSet(nodePath()+"Workdir", workDir(), "root", TBDS::OnlyCfg);
    if(sysModifFlgs&MDF_ModDir)	TBDS::genDBSet(nodePath()+"ModDir", modDir(), "root", TBDS::OnlyCfg);
    if(sysModifFlgs&MDF_IcoDir)	TBDS::genDBSet(nodePath()+"IcoDir", icoDir(), "root", TBDS::OnlyCfg);
    if(sysModifFlgs&MDF_DocDir)	TBDS::genDBSet(nodePath()+"DocDir", docDir(), "root", TBDS::OnlyCfg);
    TBDS::genDBSet(nodePath()+"MainCPUs", mainCPUs());
    TBDS::genDBSet(nodePath()+"ClockRT", i2s(clockRT()));
    TBDS::genDBSet(nodePath()+"SaveAtExit", i2s(saveAtExit()));
    TBDS::genDBSet(nodePath()+"SavePeriod", i2s(savePeriod()));

    //Redundancy parameters
    TBDS::genDBSet(nodePath()+"RdStLevel", i2s(rdStLevel()), "root", TBDS::OnlyCfg);
    TBDS::genDBSet(nodePath()+"RdTaskPer", r2s(rdTaskPer()), "root", TBDS::OnlyCfg);
    TBDS::genDBSet(nodePath()+"RdRestConnTm", i2s(rdRestConnTm()), "root", TBDS::OnlyCfg);
    TBDS::genDBSet(nodePath()+"RdPrimCmdTr", i2s(rdPrimCmdTr()), "root", TBDS::OnlyCfg);
    mRdRes.lock(false);
    string stLs;
    for(map<string,TSYS::SStat>::iterator sit = mSt.begin(); sit != mSt.end(); sit++)
	stLs += sit->first+";";
    mRdRes.unlock();
    TBDS::genDBSet(nodePath()+"RdStList", stLs, "root", TBDS::OnlyCfg);

    Mess->save();	//Messages load
}

int TSYS::start( )
{
    //High priority service task and redundancy start
    taskCreate("SYS_HighPriority", 120, TSYS::HPrTask, this);
    taskCreate("SYS_Redundancy", 5, TSYS::RdTask, this);

    //Subsystems starting
    vector<string> lst;
    list(lst);

    mess_sys(TMess::Info, _("Start!"));
    for(unsigned i_a = 0; i_a < lst.size(); i_a++)
	try { at(lst[i_a]).at().subStart(); }
	catch(TError &err) {
	    mess_err(err.cat.c_str(), "%s", err.mess.c_str());
	    mess_sys(TMess::Error, _("Error start subsystem '%s'."), lst[i_a].c_str());
	}

    cfgFileScan(true);

    //Register user API translations into config
    Mess->translReg("", "uapi:"DB_CFG);

    mess_sys(TMess::Info, _("Final starting!"));

    unsigned int i_cnt = 1;
    mStopSignal = 0;
    while(!mStopSignal) {
	//CPU frequency calc (ten seconds)
	if(!(i_cnt%(10*1000/STD_WAIT_DELAY)))	clkCalc( );

	//Config-file change periodic check (ten seconds)
	if(!(i_cnt%(10*1000/STD_WAIT_DELAY)))	cfgFileScan( );

	//Periodic shared libraries checking
	if(modSchedul().at().chkPer() && !(i_cnt%(modSchedul().at().chkPer()*1000/STD_WAIT_DELAY)))
	    modSchedul().at().libLoad(modDir(),true);

	//Periodic changes saving to DB
	if(savePeriod() && !(i_cnt%(savePeriod()*1000/STD_WAIT_DELAY))) save();

	//Config-file save need check
	if(!(i_cnt%(10*1000/STD_WAIT_DELAY)))	cfgFileSave();

	//Call subsystems at 10s
	if(!(i_cnt%(10*1000/STD_WAIT_DELAY)))
	    for(unsigned i_a=0; i_a < lst.size(); i_a++)
		try { at(lst[i_a]).at().perSYSCall(i_cnt/(1000/STD_WAIT_DELAY)); }
		catch(TError &err) { mess_err(err.cat.c_str(), "%s", err.mess.c_str()); }

	sysSleep(STD_WAIT_DELAY*1e-3);
	i_cnt++;
    }

    mess_sys(TMess::Info, _("Stop!"));
    if(saveAtExit() || savePeriod()) save();
    cfgFileSave();
    for(int i_a = lst.size()-1; i_a >= 0; i_a--)
	try { at(lst[i_a]).at().subStop(); }
	catch(TError &err) {
	    mess_err(err.cat.c_str(), "%s", err.mess.c_str());
	    mess_sys(TMess::Error, _("Error stop subsystem '%s'."), lst[i_a].c_str());
	}

    //High priority service task and redundancy stop
    taskDestroy("SYS_Redundancy");
    taskDestroy("SYS_HighPriority");

    return mStopSignal;
}

void TSYS::stop( )	{ mStopSignal = SIGUSR1; }

bool TSYS::chkSelDB( const string& wDB,  bool isStrong )
{
    if(selDB().empty() && !isStrong) return true;
    if(SYS->selDB() == TBDS::realDBName(wDB)) return true;
    return false;
}

void TSYS::setMainCPUs( const string &vl )
{
    mMainCPUs = vl;

#if __GLIBC_PREREQ(2,4)
    if(nCPU() > 1) {
	cpu_set_t cpuset;
	CPU_ZERO(&cpuset);
	string sval;
	if(!vl.size()) for(int iCPU = 0; iCPU < nCPU(); iCPU++) CPU_SET(iCPU, &cpuset);
	else {
	    string prcVl;
	    for(int off = 0; (sval=TSYS::strParse(vl,0,":",&off)).size(); ) {
		if(s2i(sval) < nCPU()) CPU_SET(s2i(sval), &cpuset);
		prcVl += (prcVl.size()?":":"") + i2s(s2i(sval));
	    }
	    mMainCPUs = prcVl;
	}
	pthread_setaffinity_np(mainPthr, sizeof(cpu_set_t), &cpuset);
    }
#endif

    modif();
}

void TSYS::sighandler( int signal, siginfo_t *siginfo, void *context )
{
    switch(signal) {
	case SIGINT:
	    SYS->mStopSignal = signal;
	    break;
	case SIGTERM:
	    SYS->mess_sys(TMess::Warning, _("Termination signal is received. Stop the station!"));
	    SYS->mStopSignal = signal;
	    break;
	case SIGFPE:
	    SYS->mess_sys(TMess::Warning, _("Floating point exception is caught!"));
	    exit(1);
	    break;
	case SIGCHLD: {
	    int status;
	    pid_t pid = wait(&status);
	    if(!WIFEXITED(status) && pid > 0) SYS->mess_sys(TMess::Info, _("Free child process %d!"), pid);
	    break;
	}
	case SIGPIPE:
	    //mess_sys(TMess::Warning, _("Broken PIPE signal!"));
	    break;
	case SIGSEGV:
	    SYS->mess_sys(TMess::Emerg, _("Segmentation fault signal!"));
	    break;
	case SIGABRT:
	    SYS->mess_sys(TMess::Emerg, _("Program aborted!"));
	    break;
	case SIGALRM: case SIGUSR1: break;
	default:
	    SYS->mess_sys(TMess::Warning, _("Unknown signal %d!"), signal);
    }
}

void TSYS::clkCalc( )
{
    uint64_t st_pnt = shrtCnt();
    sysSleep(0.1);
    mSysclc = 10*(shrtCnt()-st_pnt);

    if(!mSysclc) {
	char buf[255];
	FILE *fp = NULL;
	//Try read file cat /sys/devices/system/cpu/cpu0/cpufreq/cpuinfo_cur_freq for current CPU frequency get
	if(!mSysclc && (fp=fopen("/sys/devices/system/cpu/cpu0/cpufreq/cpuinfo_cur_freq", "r"))) {
	    size_t rez = fread(buf, 1, sizeof(buf)-1, fp); buf[rez] = 0;
	    mSysclc = uint64_t(atof(buf)*1e3);
	    fclose(fp);
	}

	//Try read file cat /proc/cpuinfo for CPU frequency or BogoMIPS get
	if(!mSysclc && (fp=fopen("/proc/cpuinfo", "r"))) {
	    float frq;
	    while(fgets(buf,sizeof(buf),fp) != NULL)
		if(sscanf(buf,"cpu MHz : %f\n",&frq) || sscanf(buf,"bogomips : %f\n",&frq) || sscanf(buf,"BogoMIPS : %f\n",&frq))
		{
		    mSysclc = (uint64_t)(frq*1e6);
		    break;
		}
	    fclose(fp);
	}
    }
}

void TSYS::cfgFileScan( bool first, bool up )
{
    struct stat f_stat;

    if(stat(cfgFile().c_str(),&f_stat) != 0) return;
    if(!up && (rootCfgFl != cfgFile() || rootFlTm != f_stat.st_mtime)) up = true;
    rootCfgFl = cfgFile();
    rootFlTm = f_stat.st_mtime;

    if(up && !first) {
	modifG();
	setSelDB(DB_CFG);
	load();
	setSelDB("");
    }
}

int64_t TSYS::curTime( )
{
    timeval cur_tm;
    gettimeofday(&cur_tm, NULL);
    return (int64_t)cur_tm.tv_sec*1000000 + cur_tm.tv_usec;
}

bool TSYS::eventWait( bool &m_mess_r_stat, bool exempl, const string &loc, time_t tm )
{
    time_t t_tm, s_tm;

    t_tm = s_tm = time(NULL);
    while(m_mess_r_stat != exempl) {
	time_t c_tm = time(NULL);
	//Check timeout
	if(tm && (c_tm > s_tm+tm)) {
	    SYS->mess_sys(TMess::Crit, _("Timeouted !!!"));
	    return true;
	}
	//Make messages
	if(c_tm > t_tm+1) {	//1sec
	    t_tm = c_tm;
	    SYS->mess_sys(TMess::Crit, _("Wait event..."));
	}
	sysSleep(STD_WAIT_DELAY*1e-3);
    }
    return false;
}

string TSYS::strSepParse( const string &path, int level, char sep, int *off )
{
    int an_dir = off ? *off : 0;
    int t_lev = 0;
    size_t t_dir;

    if(an_dir >= (int)path.size()) return "";
    while(true) {
	t_dir = path.find(sep,an_dir);
	if(t_dir == string::npos) {
	    if(off) *off = path.size();
	    return (t_lev==level) ? path.substr(an_dir) : "";
	}
	else if(t_lev == level) {
	    if(off) *off = t_dir+1;
	    return path.substr(an_dir, t_dir-an_dir);
	}
	an_dir = t_dir+1;
	t_lev++;
    }
    return "";
}

string TSYS::strParse( const string &path, int level, const string &sep, int *off, bool mergeSepSymb )
{
    int an_dir = off ? *off : 0;
    int t_lev = 0;
    size_t t_dir;

    if(an_dir >= (int)path.size() || sep.empty()) return "";
    while(true) {
	t_dir = path.find(sep,an_dir);
	if(t_dir == string::npos) {
	    if(off) *off = path.size();
	    return (t_lev==level) ? path.substr(an_dir) : "";
	}
	else if(t_lev == level) {
	    if(off) *off = t_dir+sep.size();
	    return path.substr(an_dir,t_dir-an_dir);
	}
	if(mergeSepSymb && sep.size() == 1)
	    for(an_dir = t_dir; an_dir < (int)path.size() && path[an_dir] == sep[0]; ) an_dir++;
	else an_dir = t_dir+sep.size();
	t_lev++;
    }
    return "";
}

string TSYS::strLine( const string &str, int level, int *off )
{
    int an_dir = off ? *off : 0;
    int t_lev = 0, edLnSmbSz = 1;
    size_t t_dir;

    if(an_dir >= (int)str.size()) return "";
    while(true) {
	for(t_dir = an_dir; t_dir < str.size(); t_dir++)
	    if(str[t_dir] == '\x0D' || str[t_dir] == '\x0A')
	    { edLnSmbSz = (str[t_dir] == '\x0D' && ((t_dir+1) < str.size()) && str[t_dir+1] == '\x0A') ? 2 : 1; break; }
	if(t_dir >= str.size()) {
	    if(off) *off = str.size();
	    return (t_lev==level) ? str.substr(an_dir) : "";
	}
	else if(t_lev == level) {
	    if(off) *off = t_dir+edLnSmbSz;
	    return str.substr(an_dir,t_dir-an_dir);
	}
	an_dir = t_dir+edLnSmbSz;
	t_lev++;
    }
    return "";
}

string TSYS::pathLev( const string &path, int level, bool decode, int *off )
{
    int an_dir = off ? *off : 0;
    int t_lev = 0;
    size_t t_dir;

    //First separators pass
    while(an_dir < (int)path.size() && path[an_dir]=='/') an_dir++;
    if(an_dir >= (int)path.size()) return "";

    //Path level process
    while(true) {
	t_dir = path.find("/",an_dir);
	if(t_dir == string::npos) {
	    if(off) *off = path.size();
	    return (t_lev == level) ? (decode ? TSYS::strDecode(path.substr(an_dir),TSYS::PathEl) : path.substr(an_dir)) : "";
	}
	else if(t_lev == level) {
	    if(off) *off = t_dir;
	    return decode ? TSYS::strDecode(path.substr(an_dir,t_dir-an_dir),TSYS::PathEl) : path.substr(an_dir,t_dir-an_dir);
	}
	an_dir = t_dir;
	t_lev++;
	while(an_dir < (int)path.size() && path[an_dir]=='/') an_dir++;
    }
}

string TSYS::path2sepstr( const string &path, char sep )
{
    string rez, curv;
    int off = 0;
    while(!(curv=TSYS::pathLev(path,0,false,&off)).empty()) rez += curv+sep;
    if(!rez.empty()) rez.resize(rez.size()-1);

    return rez;
}

string TSYS::sepstr2path( const string &str, char sep )
{
    string rez, curv;
    int off = 0;
    while(!(curv=TSYS::strSepParse(str,0,sep,&off)).empty()) rez += "/"+curv;

    return rez;
}

string TSYS::strEncode( const string &in, TSYS::Code tp, const string &opt1 )
{
    int iSz;
    string sout;

    switch(tp) {
	case TSYS::PathEl:
	    sout = in;
	    for(iSz = 0; iSz < (int)sout.size(); iSz++)
		switch(sout[iSz]) {
		    case '/': sout.replace(iSz,1,"%2f"); iSz += 2; break;
		    case '%': sout.replace(iSz,1,"%25"); iSz += 2; break;
		}
	    break;
	case TSYS::HttpURL: {
	    char buf[4];
	    sout = in;
	    for(iSz = 0; iSz < (int)sout.size(); iSz++)
		switch(sout[iSz]) {
		    case '%': sout.replace(iSz,1,"%25"); iSz += 2; break;
		    case ' ': sout.replace(iSz,1,"%20"); iSz += 2; break;
		    case '\t': sout.replace(iSz,1,"%09"); iSz += 2; break;
		    default:
			if(sout[iSz]&0x80) {
			    snprintf(buf,sizeof(buf),"%%%02X",(unsigned char)sout[iSz]);
			    sout.replace(iSz,1,buf);
			    iSz += 2;
			    break;
			}
		}
	    break;
	}
	case TSYS::Html:
	    sout.reserve(in.size()+10);
	    for(iSz = 0; iSz < (int)in.size(); iSz++)
		switch(in[iSz]) {
		    case '>':	sout += "&gt;";		break;
		    case '<':	sout += "&lt;";		break;
		    case '"':	sout += "&quot;";	break;
		    case '&':	sout += "&amp;";	break;
		    case '\'':	sout += "&apos;";	break;
		    default:	sout += in[iSz];
		}
	    break;
	case TSYS::JavaSc:
	    sout.reserve(in.size()+10);
	    for(iSz = 0; iSz < (int)in.size(); iSz++)
		switch(in[iSz]) {
		    case '\n':	sout += "\\n";	break;
		    default:	sout += in[iSz];
		}
	    break;
	case TSYS::SQL:
	    if(!opt1.size()) {
		sout.reserve(in.size()+10);
		for(iSz = 0; iSz < (int)in.size(); iSz++)
		    switch(in[iSz]) {
			case '\'':	sout += "\\'";	break;
			case '\"':	sout += "\\\"";	break;
			case '`':	sout += "\\`";	break;
			case '\\':	sout += "\\\\";	break;
			default:	sout += in[iSz];
		    }
	    } else {
		//By doubling method
		sout = in;
		for(unsigned iSz = 0; iSz < sout.size(); iSz++)
		    for(unsigned iSmb = 0; iSmb < opt1.size(); iSmb++)
			if(sout[iSz] == opt1[iSmb])
			    sout.replace(iSz++, 1, 2, opt1[iSmb]);
	    }
	    break;
	case TSYS::Custom: {
	    sout.reserve(in.size()+10);
	    char buf[4];
	    for(iSz = 0; iSz < (int)in.size(); iSz++) {
		unsigned i_smb;
		for(i_smb = 0; i_smb < opt1.size(); i_smb++)
		    if(in[iSz] == opt1[i_smb]) {
			snprintf(buf,sizeof(buf),"%%%02X",(unsigned char)in[iSz]);
			sout += buf;
			break;
		    }
		if(i_smb >= opt1.size()) sout += in[iSz];
	    }
	    break;
	}
	case TSYS::base64: {
	    sout.reserve(in.size()+in.size()/4+in.size()/57+10);
	    const char *base64alph = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";
	    for(iSz = 0; iSz < (int)in.size(); iSz += 3) {
		if(iSz && !(iSz%57))	sout.push_back('\n');
		sout.push_back(base64alph[(unsigned char)in[iSz]>>2]);
		if((iSz+1) >= (int)in.size()) {
		    sout.push_back(base64alph[((unsigned char)in[iSz]&0x03)<<4]);
		    sout += "==";
		}
		else {
		    sout.push_back(base64alph[(((unsigned char)in[iSz]&0x03)<<4)|((unsigned char)in[iSz+1]>>4)]);
		    if((iSz+2) >= (int)in.size()) {
			sout.push_back(base64alph[((unsigned char)in[iSz+1]&0x0F)<<2]);
			sout.push_back('=');
		    }
		    else {
			sout.push_back(base64alph[(((unsigned char)in[iSz+1]&0x0F)<<2)|((unsigned char)in[iSz+2]>>6)]);
			sout.push_back(base64alph[(unsigned char)in[iSz+2]&0x3F]);
		    }
		}
	    }
	    break;
	}
	case TSYS::FormatPrint:
	    sout = in;
	    for(iSz = 0; iSz < (int)sout.size(); iSz++)
		if(sout[iSz] == '%') { sout.replace(iSz,1,"%%"); iSz++; }
	    break;
	case TSYS::oscdID:
	    sout.reserve(in.size());
	    for(iSz = 0; iSz < (int)in.size(); iSz++)
		switch(in[iSz]) {
		    case ' ': case '/': case '\\': case '&': case '(':
		    case ')': case '[': case ']': case '!': case '~':
		    case '`': case '@': case '%': case '^': case '-':
		    case '+': case '=': case '*': case '{': case '}':
		    case ':': case ';': case '"': case '\'': case '<':
		    case '>': case '?': case '.': case ',':
			sout+="_";	break;
		    default:	sout += in[iSz];
		}
	    break;
	case TSYS::Bin: {
	    string svl, evl;
	    sout.reserve(in.size()/2);
	    for(unsigned iCh = 0; iCh < in.size(); ++iCh)
		if(isxdigit(in[iCh])) {
		    sout += (char)strtol(in.substr(iCh,2).c_str(),NULL,16); iCh++;
		}
	    break;
	}
	case TSYS::Reverse:
	    for(iSz = in.size()-1; iSz >= 0; iSz--) sout += in[iSz];
	    break;
	case TSYS::ShieldSimb:
	    sout.reserve(in.size());
	    for(iSz = 0; iSz < (int)in.size(); iSz++)
		if(in[iSz] == '\\' && iSz < ((int)in.size()-1)) {
		    switch(in[iSz+1]) {
			case 'a':	sout += '\a';	break;
			case 'b':	sout += '\b';	break;
			case 'f':	sout += '\f';	break;
			case 'n':	sout += '\n';	break;
			case 'r':	sout += '\r';	break;
			case 't':	sout += '\t';	break;
			case 'v':	sout += '\v';	break;
			case 'x': case 'X':
			    if((iSz+3) < (int)in.size() && isxdigit(in[iSz+2]) && isxdigit(in[iSz+3]))
			    { sout += (char)strtol(in.substr(iSz+2,2).c_str(),NULL,16); iSz += 2; }
			    else sout += in[iSz+1];
			    break;
			default:
			    if((iSz+3) < (int)in.size() && in[iSz+1] >= '0' && in[iSz+1] <= '7' &&
							    in[iSz+2] >= '0' && in[iSz+2] <= '7' &&
							    in[iSz+3] >= '0' && in[iSz+3] <= '7')
			    { sout += (char)strtol(in.substr(iSz+1,3).c_str(),NULL,8); iSz += 2; }
			    else sout += in[iSz+1];
		    }
		    iSz++;
		}
		else sout += in[iSz];
	    break;
    }
    return sout;
}

unsigned char TSYS::getBase64Code( unsigned char asymb )
{
    switch(asymb) {
	case 'A'...'Z':	return asymb-(unsigned char)'A';
	case 'a'...'z':	return 26+asymb-(unsigned char)'a';
	case '0'...'9':	return 52+asymb-(unsigned char)'0';
	case '+':	return 62;
	case '/':	return 63;
    }
    return 0;
}

string TSYS::strDecode( const string &in, TSYS::Code tp, const string &opt1 )
{
    unsigned iSz;
    string sout;

    switch(tp) {
	case TSYS::PathEl: case TSYS::HttpURL: case TSYS::Custom:
	    sout.reserve(in.size());
	    for(iSz = 0; iSz < in.size(); iSz++)
		switch(in[iSz]) {
		    case '%':
			if(iSz+2 < in.size()) {
			    sout += (char)strtol(in.substr(iSz+1,2).c_str(),NULL,16);
			    iSz += 2;
			}else sout += in[iSz];
			break;
		    default: sout += in[iSz];
		}
	    break;
	case TSYS::base64:
	    sout.reserve(in.size());
	    for(iSz = 0; iSz < in.size(); ) {
		if(in[iSz] == '\n')	iSz += sizeof('\n');
		if((iSz+3) < in.size())
		    if(in[iSz+1] != '=') {
			char w_code1 = TSYS::getBase64Code(in[iSz+1]);
			sout.push_back((TSYS::getBase64Code(in[iSz])<<2)|(w_code1>>4));
			if(in[iSz+2] != '=') {
			    char w_code2 = TSYS::getBase64Code(in[iSz+2]);
			    sout.push_back((w_code1<<4)|(w_code2>>2));
			    if(in[iSz+3] != '=')
				sout.push_back((w_code2<<6)|TSYS::getBase64Code(in[iSz+3]));
			}
		    }
		iSz += 4;
	    }
	    break;
	case TSYS::Bin: {
	    sout.reserve(in.size()*2);
	    char buf[3+opt1.size()];
	    for(iSz = 0; iSz < in.size(); iSz++) {
		snprintf(buf, sizeof(buf), "%s%02X", (iSz&&opt1.size())?(((iSz)%16)?opt1.c_str():"\n"):"", (unsigned char)in[iSz]);
		sout += buf;
	    }
	    break;
	}
	default: sout = in;	break;
    }

    return sout;
}

string TSYS::strCompr( const string &in, int lev )
{
    if(in.empty())	return "";

    z_stream strm;
    strm.zalloc = Z_NULL;
    strm.zfree = Z_NULL;
    strm.opaque = Z_NULL;

    if(deflateInit(&strm,lev) != Z_OK) return "";

    uLongf comprLen = deflateBound(&strm, in.size());
    char out[comprLen];

    strm.next_in = (Bytef*)in.data();
    strm.avail_in = (uInt)in.size();
    strm.next_out = (Bytef*)out;
    strm.avail_out = comprLen;

    if(deflate(&strm, Z_FINISH) != Z_STREAM_END) {
	deflateEnd(&strm);
	return "";
    }

    comprLen = strm.total_out;

    deflateEnd(&strm);

    return string(out, comprLen);
}

string TSYS::strUncompr( const string &in )
{
    int ret;
    z_stream strm;
    unsigned char out[STR_BUF_LEN];
    string rez;

    if(in.empty())	return "";

    strm.zalloc = Z_NULL;
    strm.zfree = Z_NULL;
    strm.opaque = Z_NULL;

    if(inflateInit(&strm) != Z_OK)	return "";

    strm.avail_in = in.size();
    strm.next_in = (Bytef*)in.data();
    do {
	strm.avail_out = sizeof(out);
	strm.next_out = out;
	ret = inflate(&strm,Z_NO_FLUSH);
	if(ret == Z_STREAM_ERROR || ret == Z_NEED_DICT || ret == Z_DATA_ERROR || ret == Z_MEM_ERROR)
	    break;
	rez.append((char*)out,sizeof(out)-strm.avail_out);
    } while(strm.avail_out == 0);

    inflateEnd(&strm);

    if(ret != Z_STREAM_END)	return "";

    return rez;
}

uint16_t TSYS::i16_LE( uint16_t in )
{
#if __BYTE_ORDER == __BIG_ENDIAN
    return bswap_16(in);
#endif

    return in;
}

uint32_t TSYS::i32_LE( uint32_t in )
{
#if __BYTE_ORDER == __BIG_ENDIAN
    return bswap_32(in);
#endif

    return in;
}

uint64_t TSYS::i64_LE( uint64_t in )
{
#if __BYTE_ORDER == __BIG_ENDIAN
    return bswap_64(in);
#endif

    return in;
}

uint16_t TSYS::i16_BE( uint16_t in )
{
#if __BYTE_ORDER == __LITTLE_ENDIAN
    return bswap_16(in);
#endif

    return in;
}

uint32_t TSYS::i32_BE( uint32_t in )
{
#if __BYTE_ORDER == __LITTLE_ENDIAN
    return bswap_32(in);
#endif

    return in;
}

uint64_t TSYS::i64_BE( uint64_t in )
{
#if __BYTE_ORDER == __LITTLE_ENDIAN
    return bswap_64(in);
#endif

    return in;
}

float TSYS::floatLE( float in )
{
#if __BYTE_ORDER == __BIG_ENDIAN
    ieee754_float ieee754_be;
    union ieee754_le {
	float f;
	struct {
	    unsigned int mantissa:23;
	    unsigned int exponent:8;
	    unsigned int negative:1;
	} ieee;
    } ieee754_le;

    ieee754_be.f = in;
    ieee754_le.ieee.mantissa	= ieee754_be.ieee.mantissa;
    ieee754_le.ieee.exponent	= ieee754_be.ieee.exponent;
    ieee754_le.ieee.negative	= ieee754_be.ieee.negative;

    return ieee754_le.f;
#endif

    return in;
}

float TSYS::floatLErev( float in )
{
#if __BYTE_ORDER == __BIG_ENDIAN
    ieee754_float ieee754_be;
    union ieee754_le {
	float f;
	struct {
	    unsigned int mantissa:23;
	    unsigned int exponent:8;
	    unsigned int negative:1;
	} ieee;
    } ieee754_le;

    ieee754_le.f = in;
    ieee754_be.ieee.mantissa	= ieee754_le.ieee.mantissa;
    ieee754_be.ieee.exponent	= ieee754_le.ieee.exponent;
    ieee754_be.ieee.negative	= ieee754_le.ieee.negative;

    return ieee754_be.f;
#endif

    return in;
}

double TSYS::doubleLE( double in )
{
#if __BYTE_ORDER == __BIG_ENDIAN || __FLOAT_WORD_ORDER == __BIG_ENDIAN
    ieee754_double ieee754_be;
    union ieee754_le {
	double d;
	struct {
	    unsigned int mantissa1:32;
	    unsigned int mantissa0:20;
	    unsigned int exponent:11;
	    unsigned int negative:1;
	} ieee;
    } ieee754_le;

    ieee754_be.d = in;
    ieee754_le.ieee.mantissa0	= ieee754_be.ieee.mantissa0;
    ieee754_le.ieee.mantissa1	= ieee754_be.ieee.mantissa1;
    ieee754_le.ieee.exponent	= ieee754_be.ieee.exponent;
    ieee754_le.ieee.negative	= ieee754_be.ieee.negative;

    return ieee754_le.d;
#endif

    return in;
}

double TSYS::doubleLErev( double in )
{
#if __BYTE_ORDER == __BIG_ENDIAN || __FLOAT_WORD_ORDER == __BIG_ENDIAN
    ieee754_double ieee754_be;
    union ieee754_le {
	double d;
	struct {
	    unsigned int mantissa1:32;
	    unsigned int mantissa0:20;
	    unsigned int exponent:11;
	    unsigned int negative:1;
	} ieee;
    } ieee754_le;

    ieee754_le.d = in;
    ieee754_be.ieee.mantissa0	= ieee754_le.ieee.mantissa0;
    ieee754_be.ieee.mantissa1	= ieee754_le.ieee.mantissa1;
    ieee754_be.ieee.exponent	= ieee754_le.ieee.exponent;
    ieee754_be.ieee.negative	= ieee754_le.ieee.negative;

    return ieee754_be.d;
#endif

    return in;
}

float TSYS::floatBE( float in )
{
#if __BYTE_ORDER == __LITTLE_ENDIAN
    ieee754_float ieee754_le;
    union ieee754_be {
	float f;
	struct {
	    unsigned int negative:1;
	    unsigned int exponent:8;
	    unsigned int mantissa:23;
	} ieee;
    } ieee754_be;

    ieee754_le.f = in;
    ieee754_be.ieee.mantissa	= ieee754_le.ieee.mantissa;
    ieee754_be.ieee.exponent	= ieee754_le.ieee.exponent;
    ieee754_be.ieee.negative	= ieee754_le.ieee.negative;

    return ieee754_be.f;
#endif

    return in;
}

float TSYS::floatBErev( float in )
{
#if __BYTE_ORDER == __LITTLE_ENDIAN
    ieee754_float ieee754_le;
    union ieee754_be {
	float f;
	struct {
	    unsigned int negative:1;
	    unsigned int exponent:8;
	    unsigned int mantissa:23;
	} ieee;
    } ieee754_be;

    ieee754_be.f = in;
    ieee754_le.ieee.mantissa	= ieee754_be.ieee.mantissa;
    ieee754_le.ieee.exponent	= ieee754_be.ieee.exponent;
    ieee754_le.ieee.negative	= ieee754_be.ieee.negative;

    return ieee754_le.f;
#endif

    return in;
}

double TSYS::doubleBE( double in )
{
#if __BYTE_ORDER == __LITTLE_ENDIAN
    ieee754_double ieee754_le;
    union ieee754_be {
	double d;
	struct {
	    unsigned int negative:1;
	    unsigned int exponent:11;
	    unsigned int mantissa0:20;
	    unsigned int mantissa1:32;
	} ieee;
    } ieee754_be;

    ieee754_le.d = in;
    ieee754_be.ieee.mantissa0	= ieee754_le.ieee.mantissa0;
    ieee754_be.ieee.mantissa1	= ieee754_le.ieee.mantissa1;
    ieee754_be.ieee.exponent	= ieee754_le.ieee.exponent;
    ieee754_be.ieee.negative	= ieee754_le.ieee.negative;

    return ieee754_be.d;
#endif

    return in;
}

double TSYS::doubleBErev( double in )
{
#if __BYTE_ORDER == __LITTLE_ENDIAN
    ieee754_double ieee754_le;
    union ieee754_be {
	double d;
	struct {
	    unsigned int negative:1;
	    unsigned int exponent:11;
	    unsigned int mantissa0:20;
	    unsigned int mantissa1:32;
	} ieee;
    } ieee754_be;

    ieee754_be.d = in;
    ieee754_le.ieee.mantissa0	= ieee754_be.ieee.mantissa0;
    ieee754_le.ieee.mantissa1	= ieee754_be.ieee.mantissa1;
    ieee754_le.ieee.exponent	= ieee754_be.ieee.exponent;
    ieee754_le.ieee.negative	= ieee754_be.ieee.negative;

    return ieee754_le.d;
#endif

    return in;
}

long TSYS::HZ( )	{ return sysconf(_SC_CLK_TCK); }

bool TSYS::cntrEmpty( )
{
    MtxAlloc res(dataRes(), true);
    return mCntrs.empty();
}

double TSYS::cntrGet( const string &id )
{
    MtxAlloc res(dataRes(), true);
    map<string,double>::iterator icnt = mCntrs.find(id);
    if(icnt == mCntrs.end())	return 0;
    return icnt->second;
}

void TSYS::cntrSet( const string &id, double vl )
{
    MtxAlloc res(dataRes(), true);
    mCntrs[id] = vl;
}

void TSYS::cntrIter( const string &id, double vl )
{
    MtxAlloc res(dataRes(), true);
    mCntrs[id] += vl;
}

bool TSYS::rdEnable( )	{ return mSt.size(); }

bool TSYS::rdActive( )
{
    ResAlloc res(mRdRes, false);
    for(map<string,TSYS::SStat>::iterator sit = mSt.begin(); sit != mSt.end(); sit++)
	if(sit->second.isLive) return true;
    return false;
}

void TSYS::rdStList( vector<string> &ls )
{
    ResAlloc res(mRdRes, false);
    ls.clear();
    for(map<string,TSYS::SStat>::iterator sti = mSt.begin(); sti != mSt.end(); sti++)
	ls.push_back(sti->first);
}

TSYS::SStat TSYS::rdSt( const string &id )
{
    ResAlloc res(mRdRes, false);
    TSYS::SStat rez;
    map<string,TSYS::SStat>::iterator iSt = mSt.find(id);
    if(iSt != mSt.end()) rez = iSt->second;
    return rez;
}

map<string, TSYS::SStat> TSYS::rdSts( )
{
    ResAlloc res(mRdRes, false);
    map<string, TSYS::SStat> rez = mSt;
    return rez;
}

string TSYS::rdStRequest( XMLNode &req, const string &st, bool toScan )
{
    bool prevPresent = false;
    string lcPath = req.attr("path");

    ResAlloc res(mRdRes, false);
    //map<string, TSYS::SStat>::iterator sit = st.size() ? mSt.find(st) : mSt.begin();
    //if(sit == mSt.end()) return "";

    //Same request
    for(map<string, TSYS::SStat>::iterator sit = mSt.begin(); sit != mSt.end(); ++sit) {
	if(!sit->second.isLive) continue;
	if(toScan && st.size() && !prevPresent) {
	    if(sit->first == st) prevPresent = true;
	    continue;
	}
	try {
	    req.setAttr("path", "/"+sit->first+lcPath);
	    SYS->transport().at().cntrIfCmd(req, "redundant");
	    sit->second.cnt++;
	    return sit->first;
	} catch(TError &err) {
	    sit->second.isLive = false;
	    sit->second.cnt = rdRestConnTm();
	    sit->second.lev = 0;
	}
    }

    return "";
}

void TSYS::taskCreate( const string &path, int priority, void *(*start_routine)(void *), void *arg, int wtm, pthread_attr_t *pAttr, bool *startSt )
{
    int detachStat = 0;
    pthread_t procPthr;
    pthread_attr_t locPAttr, *pthr_attr;
    map<string,STask>::iterator ti;

    ResAlloc res(taskRes, true);
    for(time_t c_tm = time(NULL); (ti=mTasks.find(path)) != mTasks.end(); ) {
	//Remove created and finished but not destroyed task
	if(ti->second.flgs&STask::FinishTask && !(ti->second.flgs&STask::Detached)) {
	    pthread_join(ti->second.thr, NULL);
	    mTasks.erase(ti);
	    continue;
	}
	res.release();
	//Error by this active task present
	if(time(NULL) >= (c_tm+wtm)) throw err_sys(_("Task '%s' already present!"), path.c_str());
	sysSleep(0.01);
	res.request(true);
    }
    STask &htsk = mTasks[path];
    htsk.path = path;
    htsk.task = start_routine;
    htsk.taskArg = arg;
    htsk.flgs = 0;
    htsk.thr = 0;
    htsk.prior = priority%100;
    res.release();

    if(pAttr) pthr_attr = pAttr;
    else {
	pthr_attr = &locPAttr;
	pthread_attr_init(pthr_attr);
    }
    pthread_attr_setinheritsched(pthr_attr, PTHREAD_EXPLICIT_SCHED);
    struct sched_param prior;
    prior.sched_priority = 0;

    int policy = SCHED_OTHER;
#if __GLIBC_PREREQ(2,4)
    if(priority < 0)	policy = SCHED_BATCH;
#endif
    if(priority > 0)	policy = SCHED_RR;
    if(priority >= 100)	policy = SCHED_FIFO;
    pthread_attr_setschedpolicy(pthr_attr, policy);
    prior.sched_priority = vmax(sched_get_priority_min(policy),vmin(sched_get_priority_max(policy),priority%100));
    pthread_attr_setschedparam(pthr_attr, &prior);

    try {
	pthread_attr_getdetachstate(pthr_attr,&detachStat);
	if(detachStat == PTHREAD_CREATE_DETACHED) htsk.flgs |= STask::Detached;
	int rez = pthread_create(&procPthr, pthr_attr, taskWrap, &htsk);
	if(rez == EPERM) {
	    mess_sys(TMess::Warning, _("No permission for create a real-time policy for '%s'. Default thread is created!"), path.c_str());
	    policy = SCHED_OTHER;
	    pthread_attr_setschedpolicy(pthr_attr, policy);
	    prior.sched_priority = 0;
	    pthread_attr_setschedparam(pthr_attr,&prior);
	    rez = pthread_create(&procPthr, pthr_attr, taskWrap, &htsk);
	}
	if(!pAttr) pthread_attr_destroy(pthr_attr);

	if(rez) throw err_sys(1, _("Task creation error %d."), rez);

	//Wait for thread structure initialization finish for not detachable tasks
	while(!(htsk.flgs&STask::Detached) && !htsk.thr) TSYS::sysSleep(1e-3); //sched_yield(); !!! don't use for hard realtime systems with high priority
	//Wait for start status
	for(time_t c_tm = time(NULL); !(htsk.flgs&STask::Detached) && startSt && !(*startSt); ) {
	    if(time(NULL) >= (c_tm+wtm)) throw err_sys(_("Task '%s' start timeouted!"), path.c_str());
	    sysSleep(STD_WAIT_DELAY*1e-3);
	}
    } catch(TError &err) {
	if(err.cod) {		//Remove info for pthread_create() but left for other by possible start later
	    res.request(true);
	    mTasks.erase(path);
	    res.release();
	}
	throw;
    }
}

void TSYS::taskDestroy( const string &path, bool *endrunCntr, int wtm, bool noSignal, pthread_cond_t *cv )
{
    map<string,STask>::iterator it;
    ResAlloc res(taskRes, false);
    if(mTasks.find(path) == mTasks.end()) return;
    res.release();

    if(endrunCntr) *endrunCntr = true;

    //Wait for task stop and SIGALRM send repeat
    time_t t_tm, s_tm;
    t_tm = s_tm = time(NULL);
    bool first = true;
    res.request(true);
    while((it=mTasks.find(path)) != mTasks.end() && !(it->second.flgs&STask::FinishTask)) {
	if(first) pthread_kill(it->second.thr, SIGUSR1);	//User's termination signal, check for it by function taskEndRun()
	if(!noSignal) pthread_kill(it->second.thr, SIGALRM);	//Sleep, select and other system calls termination
	if(cv) pthread_cond_signal(cv);
	res.release();

	time_t c_tm = time(NULL);
	//Check timeout
	if(wtm && (c_tm > (s_tm+wtm))) {
	    mess_sys(TMess::Crit, _("Task '%s' timeouted !!!"), path.c_str());
	    throw err_sys(_("Task '%s' is not stopped!"), path.c_str());
	}
	//Make messages
	if(c_tm > t_tm+1) {	//1sec
	    t_tm = c_tm;
	    mess_sys(TMess::Info, _("Wait event of task '%s' ..."), path.c_str());
	}
	sysSleep(STD_WAIT_DELAY*1e-3);
	first = false;
	res.request(true);
    }
    if(it != mTasks.end()) {
	if(!(it->second.flgs&STask::Detached)) pthread_join(it->second.thr, NULL);
	mTasks.erase(it);
    }
}

double TSYS::taskUtilizTm( const string &path, bool max )
{
    ResAlloc res(taskRes, false);
    map<string,STask>::iterator it = mTasks.find(path);
    if(it == mTasks.end()) return 0;
    if(max) return 1e-9*it->second.consMax;
    int64_t tm_beg = 0, tm_end = 0, tm_per = 0;
    for(int i_tr = 0; tm_beg == tm_per && i_tr < 2; i_tr++) {
	tm_beg = it->second.tm_beg;
	tm_end = it->second.tm_end;
	tm_per = it->second.tm_per;
    }
    if(tm_beg && tm_beg < tm_per) return 1e-9*(tm_end-tm_beg);

    return 0;
}

bool TSYS::taskEndRun( )
{
    sigset_t sigset;
    return sigpending(&sigset) == 0 && sigismember(&sigset,SIGUSR1);
}

const TSYS::STask& TSYS::taskDescr( )
{
    STask *stsk = (STask*)pthread_getspecific(sTaskKey);
    if(stsk) return *stsk;
    throw SYS->err_sys(_("It isn't OpenSCADA task!"));
}

void *TSYS::taskWrap( void *stas )
{
    //Get temporary task structure
    STask *tsk = (STask *)stas;
    pthread_setspecific(TSYS::sTaskKey, tsk);

    //Store call parameters
    void *(*wTask) (void *) = tsk->task;
    void *wTaskArg = tsk->taskArg;

    //Get current policy and priority
    int policy;
    struct sched_param param;
    pthread_getschedparam(pthread_self(), &policy, &param);
    tsk->policy = policy;
    //tsk->prior = param.sched_priority;	//!!!! Commented for nice

#if __GLIBC_PREREQ(2,4)
    //Load and init CPU set
    if(SYS->nCPU() > 1 && !(tsk->flgs&STask::Detached)) {
	tsk->cpuSet = TBDS::genDBGet(SYS->nodePath()+"CpuSet:"+tsk->path);
	cpu_set_t cpuset;
	CPU_ZERO(&cpuset);
	string sval;
	bool cpuSetOK = false;
	for(int off = 0; (sval=TSYS::strParse(tsk->cpuSet,0,":",&off)).size(); cpuSetOK = true)
	    if(s2i(sval) < SYS->nCPU()) CPU_SET(s2i(sval), &cpuset);
	if(cpuSetOK) pthread_setaffinity_np(pthread_self(), sizeof(cpu_set_t), &cpuset);
    }
    else if(SYS->nCPU() > 1 && (tsk->flgs & STask::Detached)) tsk->cpuSet = "NA";
#endif

    //Final set for init finish indicate
    tsk->tid = syscall(SYS_gettid);
    // Set nice level without realtime if it no permitted
    if(tsk->policy != SCHED_RR && tsk->prior > 0 && setpriority(PRIO_PROCESS,tsk->tid,-tsk->prior/5) != 0) tsk->prior = 0;
    tsk->thr = pthread_self();		//Task creation finish

    //Signal SIGUSR1 BLOCK for internal checking to endrun by taskEndRun()
    sigset_t mask;
    sigemptyset(&mask);
    sigaddset(&mask, SIGUSR1);
    pthread_sigmask(SIG_BLOCK, &mask, NULL);

    //Call work task
    void *rez = NULL;
    try { rez = wTask(wTaskArg); }
    catch(TError &err) {
	mess_err(err.cat.c_str(), "%s", err.mess.c_str());
	SYS->mess_sys(TMess::Error, _("Task %u unexpected terminated by exception."), tsk->thr);
    }
    //???? The code cause: FATAL: exception not rethrown
    //catch(...)	{ mess_sys(TMess::Error, _("Task %u unexpected terminated by unknown exception."), tsk->thr); }

    //Mark for task finish
    tsk->flgs |= STask::FinishTask;

    //> Remove task object for detached
    if(tsk->flgs & STask::Detached)	SYS->taskDestroy(tsk->path, NULL);

    return rez;
}

void *TSYS::HPrTask( void *icntr )
{
    while(!TSYS::taskEndRun()) {
	SYS->mSysTm = time(NULL);
	TSYS::taskSleep(1000000000);
    }

    return NULL;
}

void *TSYS::RdTask( void *param )
{
    XMLNode req("CntrReqs");
    vector<string> subLs;
    SYS->list(subLs);

    while(!TSYS::taskEndRun())
    try {
	int64_t wTm = SYS->curTime();

	//Update wait time for dead stations and process connections to stations
	ResAlloc res(SYS->mRdRes, false);
	for(map<string,TSYS::SStat>::iterator sit = SYS->mSt.begin(); sit != SYS->mSt.end(); sit++) {
	    // Live stations and connect to new station process
	    if(sit->second.isLive || (!sit->second.isLive && sit->second.cnt <= 0)) {
		// Prepare request to a remote station
		req.clear()->setAttr("path", "/"+sit->first);
		req.childAdd("st")->setAttr("path","/%2fserv%2fredundant");
		for(unsigned iSub = 0; iSub < subLs.size(); iSub++)
		    req.childAdd("st")->setAttr("path","/"+subLs[iSub]+"/%2fserv%2fredundant");
		try {
		    if(SYS->transport().at().cntrIfCmd(req,"redundant")) continue;
		    sit->second.lev = s2i(req.childGet(0)->attr("StLevel"));
		    sit->second.isLive = true;

		    // State request
		    for(int iL = 0, iReq = 1; iL < (int)subLs.size() && iReq < (int)req.childSize(); iL++, iReq++) {
			XMLNode *subPrt = req.childGet(iReq);
			subPrt->setAttr("StId", sit->first);
			if(!s2i(subPrt->attr("inProc")) || !SYS->at(subLs[iL]).at().rdProcess(subPrt))
			    subLs.erase(subLs.begin()+(iL--));
		    }
		} catch(TError &err) {
		    sit->second.isLive = false;
		    sit->second.lev = 0;
		    sit->second.cnt = SYS->rdRestConnTm();
		    continue;
		}
	    }
	    // Reconnect counter process
	    if(!sit->second.isLive && sit->second.cnt > 0) sit->second.cnt -= SYS->rdTaskPer();
	    if(sit->second.isLive != sit->second.isLivePrev) {
		if(sit->second.isLive) SYS->mess_sys(TMess::Notice, _("Redundant station '%s' up."),
						SYS->transport().at().extHostGet("*",sit->first).name.c_str());
		else SYS->mess_sys(TMess::Warning, _("Redundant station '%s' down."),
						SYS->transport().at().extHostGet("*",sit->first).name.c_str());
		sit->second.isLivePrev = sit->second.isLive;
	    }
	}
	res.release();

	//Call to main service request
	for(int iL = 0; iL < (int)subLs.size(); iL++)
	    if(!SYS->at(subLs[iL]).at().rdProcess())
		subLs.erase(subLs.begin()+(iL--));

	SYS->mRdPrcTm = SYS->curTime()-wTm;

	//Wait to next iteration
	TSYS::taskSleep((int64_t)(SYS->rdTaskPer()*1e9));
    } catch(TError &err) { mess_err(err.cat.c_str(), "%s", err.mess.c_str()); }

    return NULL;
}

int TSYS::sysSleep( float tm )
{
    struct timespec sp_tm;
    sp_tm.tv_sec = (time_t)tm;
    sp_tm.tv_nsec = (long int)(1e9*(tm-floorf(tm)));
    return nanosleep(&sp_tm, NULL);
}

void TSYS::taskSleep( int64_t per, const string &icron, int64_t *lag )
{
    struct timespec sp_tm;
    STask *stsk = (STask*)pthread_getspecific(sTaskKey);

    if(icron.empty()) {
	if(!per) per = 1000000000ll;
	clockid_t clkId = SYS->clockRT() ? CLOCK_REALTIME : CLOCK_MONOTONIC;
	clock_gettime(clkId, &sp_tm);
	int64_t cur_tm = (int64_t)sp_tm.tv_sec*1000000000ll + sp_tm.tv_nsec,
		pnt_tm = (cur_tm/per + 1)*per,
		wake_tm = 0;
	do {
	    sp_tm.tv_sec = pnt_tm/1000000000ll; sp_tm.tv_nsec = pnt_tm%1000000000ll;
	    if(clock_nanosleep(clkId,TIMER_ABSTIME,&sp_tm,NULL)) return;
	    clock_gettime(clkId, &sp_tm);
	    wake_tm = (int64_t)sp_tm.tv_sec*1000000000ll + sp_tm.tv_nsec;
	} while(wake_tm < pnt_tm);

	if(stsk) {
	    if(stsk->tm_pnt) stsk->cycleLost += vmax(0, pnt_tm/per-stsk->tm_pnt/per-1);
	    if(lag) *lag = stsk->tm_pnt ? wake_tm-stsk->tm_pnt-per : 0;
	    stsk->tm_beg = stsk->tm_per;
	    stsk->tm_end = cur_tm;
	    stsk->tm_per = wake_tm;
	    stsk->tm_pnt = pnt_tm;
	    stsk->lagMax = vmax(stsk->lagMax, stsk->tm_per - stsk->tm_pnt);
	    if(stsk->tm_beg) stsk->consMax = vmax(stsk->consMax, stsk->tm_end - stsk->tm_beg);
	}
    }
    else {
	int64_t end_tm = curTime();
	time_t cur_tm = end_tm/1000000,
	       cron_tm = cron(icron, cur_tm);
	while(time(NULL) < cron_tm && sysSleep(1) == 0) {
	    time_t dt = time(NULL) - cur_tm;
	    if(dt/60) {
		dt = 60*(dt/60);
		SYS->mess_sys(TMess::Debug, _("System clock changed to '%s'. Correction the cron '%s' target!"), tm2s(dt).c_str(), icron.c_str());
		cron_tm += dt;
		end_tm += 1000000ll*dt;
		if(stsk) stsk->tm_per += 1000000000ll*dt;
	    }
	    cur_tm = time(NULL);
	}
	if(stsk) {
	    stsk->tm_beg = stsk->tm_per;
	    stsk->tm_end = 1000ll*end_tm;
	    stsk->tm_per = 1000ll*curTime();
	    stsk->tm_pnt = 1000000000ll*cron_tm;
	    stsk->lagMax = vmax(stsk->lagMax, stsk->tm_per - stsk->tm_pnt);
	    if(stsk->tm_beg) stsk->consMax = vmax(stsk->consMax, stsk->tm_end - stsk->tm_beg);
	}
    }
}

time_t TSYS::cron( const string &vl, time_t base )
{
    string cronEl, tEl;
    int vbeg, vend, vstep, vm;

    time_t ctm = base?base:time(NULL);
    struct tm ttm;
    localtime_r(&ctm,&ttm);
    ttm.tm_sec = 0;

reload:
    bool isReload = false;

    //Minutes check
    cronEl = TSYS::strSepParse(vl,0,' ');
    vm = 200;
    for(int eoff = 0; (tEl=TSYS::strSepParse(cronEl,0,',',&eoff)).size(); ) {
	vbeg = vend = -1; vstep = 0;
	sscanf(tEl.c_str(),"%d-%d/%d",&vbeg,&vend,&vstep);
	if(vbeg < 0) { sscanf(tEl.c_str(),"*/%d",&vstep); vbeg=0; vend=59; }
	if(vend < 0) vm = vmin(vm,vbeg+((ttm.tm_min>=vbeg)?60:0));
	else if((vbeg=vmax(0,vbeg)) < (vend=vmin(59,vend))) {
	    if(ttm.tm_min < vbeg) vm = vmin(vm,vbeg);
	    else if((vstep>1 && ttm.tm_min >= (vbeg+((vend-vbeg)/vstep)*vstep)) || (vstep <= 0 && ttm.tm_min >= vend))
		vm = vmin(vm,vbeg+60);
	    else if(vstep>1 ) vm = vmin(vm, vbeg + vstep*(((ttm.tm_min+1)-vbeg)/vstep + ((((ttm.tm_min+1)-vbeg)%vstep)?1:0)));
	    else vm = vmin(vm, ttm.tm_min+1);
	}
	if(vm == ttm.tm_min+1) break;
    }
    ttm.tm_min = vm;
    mktime(&ttm);

    //Hours check
    cronEl = TSYS::strSepParse(vl,1,' ');
    vm = 200;
    for(int eoff = 0; (tEl=TSYS::strSepParse(cronEl,0,',',&eoff)).size(); ) {
	vbeg = vend = -1; vstep = 0;
	sscanf(tEl.c_str(),"%d-%d/%d",&vbeg,&vend,&vstep);
	if(vbeg < 0) { sscanf(tEl.c_str(),"*/%d",&vstep); vbeg=0; vend=23; }
	if(vend < 0) vm = vmin(vm,vbeg+((ttm.tm_hour>vbeg)?24:0));
	else if((vbeg=vmax(0,vbeg)) < (vend=vmin(23,vend))) {
	    if(ttm.tm_hour < vbeg) vm = vmin(vm,vbeg);
	    else if((vstep>1 && ttm.tm_hour > (vbeg+((vend-vbeg)/vstep)*vstep)) || (vstep <= 0 && ttm.tm_hour > vend))
		vm = vmin(vm,vbeg+24);
	    else if(vstep>1 ) vm = vmin(vm, vbeg + vstep*((ttm.tm_hour-vbeg)/vstep + (((ttm.tm_hour-vbeg)%vstep)?1:0)));
	    else vm = vmin(vm, ttm.tm_hour);
	}
	if(vm == ttm.tm_hour) break;
    }
    isReload = (vm != 200 && ttm.tm_hour!=vm);
    ttm.tm_hour = vm;
    mktime(&ttm);
    if(isReload)	{ ttm.tm_min = -1; goto reload; }

    //Day check
    cronEl = TSYS::strSepParse(vl,2,' ');
    string cronElw = TSYS::strSepParse(vl,4,' ');
    vm = 200;
    if(cronEl != "*")
	for(int eoff = 0; (tEl=TSYS::strSepParse(cronEl,0,',',&eoff)).size(); ) {
	    vbeg = vend = -1; vstep = 0;
	    sscanf(tEl.c_str(),"%d-%d/%d",&vbeg,&vend,&vstep);
	    if(vbeg < 0) { sscanf(tEl.c_str(),"*/%d",&vstep); vbeg=1; vend=31; }
	    if(vend < 0) vm = vmin(vm,vbeg+((ttm.tm_mday>vbeg)?31:0));
	    else if((vbeg=vmax(1,vbeg)) < (vend=vmin(31,vend))) {
		if(ttm.tm_mday < vbeg) vm = vmin(vm,vbeg);
		else if((vstep>1 && ttm.tm_mday > (vbeg+((vend-vbeg)/vstep)*vstep)) || (vstep <= 0 && ttm.tm_mday > vend))
		    vm = vmin(vm,vbeg+31);
		else if(vstep>1 ) vm = vmin(vm, vbeg + vstep*((ttm.tm_mday-vbeg)/vstep + (((ttm.tm_mday-vbeg)%vstep)?1:0)));
		else vm = vmin(vm, ttm.tm_mday);
	    }
	    if(vm == ttm.tm_mday) break;
	}
    if(cronEl == "*" || (cronElw != "*" && !cronElw.empty()))
	for(int eoff = 0; (tEl=TSYS::strSepParse(cronElw,0,',',&eoff)).size(); ) {
	    vbeg = vend = -1; vstep = 0;
	    sscanf(tEl.c_str(),"%d-%d/%d",&vbeg,&vend,&vstep);
	    if(vbeg < 0) { sscanf(tEl.c_str(),"*/%d",&vstep); vbeg=0; vend=6; }
	    if(vend < 0) vm = vmin(vm, ttm.tm_mday - ttm.tm_wday + vbeg+((ttm.tm_wday>vbeg)?7:0));
	    else if((vbeg=vmax(0,vbeg)) < (vend=vmin(6,vend))) {
		if(ttm.tm_wday < vbeg) vm = vmin(vm, ttm.tm_mday - ttm.tm_wday + vbeg);
		else if((vstep>1 && ttm.tm_wday > (vbeg+((vend-vbeg)/vstep)*vstep)) || (vstep <= 0 && ttm.tm_wday > vend))
		    vm = vmin(vm, ttm.tm_mday - ttm.tm_wday + vbeg+7);
		else if(vstep>1) vm = vmin(vm, ttm.tm_mday - ttm.tm_wday + vbeg + vstep*((ttm.tm_wday-vbeg)/vstep + (((ttm.tm_wday-vbeg)%vstep)?1:0)));
		else vm = vmin(vm, ttm.tm_mday);
	    }
	    if(vm == ttm.tm_mday) break;
	}
    isReload = (vm!=200 && ttm.tm_mday!=vm);
    if(vm <= 31) ttm.tm_mday = vm;
    else { ttm.tm_mday = vm-31; ttm.tm_mon++; }
    mktime(&ttm);
    if(isReload)	{ ttm.tm_min = -1; ttm.tm_hour = 0; goto reload; }

    //Month check
    cronEl = TSYS::strSepParse(vl,3,' ');
    vm = 200;
    for(int eoff = 0; (tEl=TSYS::strSepParse(cronEl,0,',',&eoff)).size(); ) {
	vbeg = vend = -1; vstep = 0;
	sscanf(tEl.c_str(),"%d-%d/%d",&vbeg,&vend,&vstep);
	if(vbeg < 0) { sscanf(tEl.c_str(),"*/%d",&vstep); vbeg=1; vend=12; }
	if(vend < 0) vm = vmin(vm,vbeg+(((ttm.tm_mon+1)>vbeg)?12:0));
	else if((vbeg=vmax(1,vbeg)) < (vend=vmin(12,vend))) {
	    if((ttm.tm_mon+1) < vbeg) vm = vmin(vm,vbeg);
	    else if((vstep>1 && (ttm.tm_mon+1) > (vbeg+((vend-vbeg)/vstep)*vstep)) || (vstep <= 0 && (ttm.tm_mon+1) > vend))
		vm = vmin(vm,vbeg+12);
	    else if(vstep>1) vm = vmin( vm, vbeg + vstep*(((ttm.tm_mon+1)-vbeg)/vstep + ((((ttm.tm_mon+1)-vbeg)%vstep)?1:0)));
	    else vm = vmin(vm, ttm.tm_mon+1);
	}
	if(vm == (ttm.tm_mon+1)) break;
    }
    isReload = (vm!=200 && ttm.tm_mon!=(vm-1));
    ttm.tm_mon = vm-1;
    mktime(&ttm);
    if(isReload)	{ ttm.tm_min = -1; ttm.tm_hour = 0; ttm.tm_mday = 1; goto reload; }

    return mktime(&ttm);
}

TVariant TSYS::objFuncCall( const string &iid, vector<TVariant> &prms, const string &user )
{
    bool alt1 = false;
    // int message(string cat, int level, string mess) - formation of the system message <mess> with the category <cat>, level <level>
    //  cat - message category
    //  level - message level
    //  mess - message text
    if(iid == "message" && prms.size() >= 3)	{ message(prms[0].getS().c_str(), (TMess::Type)prms[1].getI(), "%s", prms[2].getS().c_str()); return 0; }
    // int mess{Debug,Info,Note,Warning,Err,Crit,Alert,Emerg}(string cat, string mess) -
    //		formation of the system message <mess> with the category <cat> and the appropriate level
    //  cat - message category
    //  mess - message text
    if(iid == "messDebug" && prms.size() >= 2)	{ mess_debug(prms[0].getS().c_str(), "%s", prms[1].getS().c_str()); return 0; }
    if(iid == "messInfo" && prms.size() >= 2)	{ mess_info(prms[0].getS().c_str(), "%s", prms[1].getS().c_str()); return 0; }
    if(iid == "messNote" && prms.size() >= 2)	{ mess_note(prms[0].getS().c_str(), "%s", prms[1].getS().c_str()); return 0; }
    if(iid == "messWarning" && prms.size() >= 2){ mess_warning(prms[0].getS().c_str(), "%s", prms[1].getS().c_str()); return 0; }
    if(iid == "messErr" && prms.size() >= 2)	{ mess_err(prms[0].getS().c_str(), "%s", prms[1].getS().c_str()); return 0; }
    if(iid == "messCrit" && prms.size() >= 2)	{ mess_crit(prms[0].getS().c_str(), "%s", prms[1].getS().c_str()); return 0; }
    if(iid == "messAlert" && prms.size() >= 2)	{ mess_alert(prms[0].getS().c_str(), "%s", prms[1].getS().c_str()); return 0; }
    if(iid == "messEmerg" && prms.size() >= 2)	{ mess_emerg(prms[0].getS().c_str(), "%s", prms[1].getS().c_str()); return 0; }
    // string system(string cmd, bool noPipe = false) - calls the console commands <cmd> of OS returning the result by the channel
    //  cmd - command text
    //  noPipe - pipe result disable for background call
    if(iid == "system" && prms.size() >= 1) {
	if(prms.size() >= 2 && prms[1].getB()) return system(prms[0].getS().c_str());
	FILE *fp = popen(prms[0].getS().c_str(), "r");
	if(!fp) return string("");

	char buf[STR_BUF_LEN];
	string rez;
	for(int r_cnt = 0; (r_cnt=fread(buf,1,sizeof(buf),fp)) || !feof(fp); )
	    rez.append(buf,r_cnt);

	pclose(fp);
	return rez;
    }
    // string fileRead( string file ) - Return <file> content by string.
    if(iid == "fileRead" && prms.size() >= 1) {
	char buf[STR_BUF_LEN];
	string rez;
	int hd = open(prms[0].getS().c_str(),O_RDONLY);
	if(hd >= 0) {
	    for(int len = 0; (len=read(hd,buf,sizeof(buf))) > 0; ) rez.append(buf,len);
	    close(hd);
	}
	return rez;
    }
    // int fileWrite( string file, string str, bool append = false ) - Write <str> to <file>, remove presented or <append>.
    //	  Return wrote bytes count.
    if(iid == "fileWrite" && prms.size() >= 2) {
	int wcnt = 0, wflags = O_WRONLY|O_CREAT|O_TRUNC;
	string val = prms[1].getS();
	if(prms.size() >= 3 && prms[2].getB()) wflags = O_WRONLY|O_CREAT|O_APPEND;
	int hd = open(prms[0].getS().c_str(), wflags, 0664);
	if(hd >= 0) {
	    wcnt = write(hd,val.data(),val.size());
	    close(hd);
	}
	return wcnt;
    }
    // int fileRemove( string file ) - Remove <file>.
    //	  Return the removing result.
    if(iid == "fileRemove" && prms.size()) return remove(prms[0].getS().c_str());
    // XMLNodeObj XMLNode(string name = "") - creation of the XML node object with the name <name>
    //  name - XML node name
    if(iid == "XMLNode") return new XMLNodeObj((prms.size()>=1) ? prms[0].getS() : "");
    // string cntrReq(XMLNodeObj req, string stat = "") - request of the control interface to the system via XML
    //  req - request's XML node
    //  stat - remote OpenSCADA-station for request
    if(iid == "cntrReq" && prms.size() >= 1) {
	XMLNode req;
	AutoHD<XMLNodeObj> xnd = prms[0].getO();
	if(xnd.freeStat()) return string(_("1:Request is not object!"));
	xnd.at().toXMLNode(req);
	string path = req.attr("path");
	if(prms.size() < 2 || prms[1].getS().empty()) {
	    req.setAttr("user",user);
	    cntrCmd(&req);
	}
	else {
	    req.setAttr("path","/"+prms[1].getS()+path);
	    transport().at().cntrIfCmd(req, "cntrReq");
	    req.setAttr("path",path);
	}
	xnd.at().fromXMLNode(req);
	return string("0");
    }
    // int sleep(real tm, int ntm = 0) - call for task sleep to <tm> seconds and <ntm> nanoseconds.
    //  tm - wait time in seconds (precised up to nanoseconds), up to STD_INTERF_TM(5 seconds)
    //  ntm - wait time part in nanoseconds
    if(iid == "sleep" && prms.size() >= 1) {
	struct timespec sp_tm;
	sp_tm.tv_sec = prms[0].getI();
	sp_tm.tv_nsec = 1000000000l*(prms[0].getR()-sp_tm.tv_sec) + ((prms.size()>=2)?prms[1].getI():0);
	sp_tm.tv_sec = vmin(STD_INTERF_TM, sp_tm.tv_sec);
	return nanosleep(&sp_tm, NULL);
    }
    // int time(int usec) - returns the absolute time in seconds from the epoch of 1/1/1970 and in microseconds, if <usec> is specified
    //  usec - microseconds of time
    if(iid == "time") {
	if(prms.empty()) return (int64_t)time(NULL);
	int64_t tm = curTime();
	prms[0].setI(tm%1000000); prms[0].setModify();
	return (int64_t)(tm/1000000);
    }
    // int {localtime|gmtime}(int fullsec, int sec, int min, int hour, int mday, int month, int year, int wday, int yday, int isdst)
    //      - returns the full date based on the absolute time in seconds <fullsec> from the epoch 1.1.1970
    //  fullsec - source time ins seconds from the epoch 1.1.1970
    //  sec - seconds
    //  min - minutes
    //  hour - hours
    //  mday - days of the month
    //  month - months
    //  year - years
    //  wday - days in the week
    //  yday - days in the year
    //  isdst - sign of summer time
    if((iid == "localtime" || (alt1=(iid=="gmtime"))) && prms.size() >= 2) {
	time_t tm_t = prms[0].getI();
	struct tm tm_tm;
	if(alt1) gmtime_r(&tm_t, &tm_tm);
	else localtime_r(&tm_t, &tm_tm);

	prms[1].setI(tm_tm.tm_sec); prms[1].setModify();
	if(prms.size() >= 3)	{ prms[2].setI(tm_tm.tm_min); prms[2].setModify(); }
	if(prms.size() >= 4) 	{ prms[3].setI(tm_tm.tm_hour); prms[3].setModify(); }
	if(prms.size() >= 5) 	{ prms[4].setI(tm_tm.tm_mday); prms[4].setModify(); }
	if(prms.size() >= 6) 	{ prms[5].setI(tm_tm.tm_mon); prms[5].setModify(); }
	if(prms.size() >= 7) 	{ prms[6].setI(1900+tm_tm.tm_year); prms[6].setModify(); }
	if(prms.size() >= 8) 	{ prms[7].setI(tm_tm.tm_wday); prms[7].setModify(); }
	if(prms.size() >= 9) 	{ prms[8].setI(tm_tm.tm_yday); prms[8].setModify(); }
	if(prms.size() >= 10) 	{ prms[9].setI(tm_tm.tm_isdst); prms[9].setModify(); }
	return 0;
    }
    // int mktime(int sec, int min, int hour, int mday, int month, int year, int wday, int yday, int isdst)
    //      - return time since the Epoch 1.1.1970 converted from broken-down time
    //  sec - seconds
    //  min - minutes
    //  hour - hours
    //  mday - days of the month
    //  month - months
    //  year - years
    //  wday - days in the week
    //  yday - days in the year
    //  isdst - sign of summer time
    if(iid == "mktime" || (alt1=(iid=="timegm"))) {
	struct tm tm_tm;
	tm_tm.tm_sec	= (prms.size()>=1) ? prms[0].getI() : 0;
	tm_tm.tm_min	= (prms.size()>=2) ? prms[1].getI() : 0;
	tm_tm.tm_hour	= (prms.size()>=3) ? prms[2].getI() : 0;
	tm_tm.tm_mday	= (prms.size()>=4) ? prms[3].getI() : 0;
	tm_tm.tm_mon	= (prms.size()>=5) ? prms[4].getI() : 0;
	tm_tm.tm_year	= (prms.size()>=6) ? prms[5].getI()-1900 : 0;
	tm_tm.tm_wday	= (prms.size()>=7) ? prms[6].getI() : -1;
	tm_tm.tm_yday	= (prms.size()>=8) ? prms[7].getI() : -1;
	tm_tm.tm_isdst	= (prms.size()>=9) ? prms[8].getI() : -1;
	time_t rez = alt1 ? timegm(&tm_tm) : mktime(&tm_tm);
	if(prms.size() >= 1)	{ prms[0].setI(tm_tm.tm_sec);	prms[0].setModify(); }
	if(prms.size() >= 2)	{ prms[1].setI(tm_tm.tm_min);	prms[1].setModify(); }
	if(prms.size() >= 3)	{ prms[2].setI(tm_tm.tm_hour);	prms[2].setModify(); }
	if(prms.size() >= 4)	{ prms[3].setI(tm_tm.tm_mday);	prms[3].setModify(); }
	if(prms.size() >= 5)	{ prms[4].setI(tm_tm.tm_mon);	prms[4].setModify(); }
	if(prms.size() >= 6)	{ prms[5].setI(1900+tm_tm.tm_year); prms[5].setModify(); }
	if(prms.size() >= 7)	{ prms[6].setI(tm_tm.tm_wday);	prms[6].setModify(); }
	if(prms.size() >= 8)	{ prms[7].setI(tm_tm.tm_yday);	prms[7].setModify(); }
	if(prms.size() >= 9)	{ prms[8].setI(tm_tm.tm_isdst);	prms[8].setModify(); }

	return (int64_t)rez;
    }
    // string {strftime|strftimegm}(int sec, string form = "%Y-%m-%d %H:%M:%S")
    //      - converts an absolute time <sec> to the string of the desired format <form>
    //  sec - time ins seconds from the epoch 1.1.1970
    //  form - result string format
    if((iid == "strftime" || (alt1=(iid=="strftimegm"))) && !prms.empty()) {
	time_t tm_t = prms[0].getI();
	struct tm tm_tm;
	if(alt1) gmtime_r(&tm_t, &tm_tm);
	else localtime_r(&tm_t, &tm_tm);
	char buf[1000];
	int rez = strftime(buf, sizeof(buf), (prms.size()>=2) ? prms[1].getS().c_str() : "%Y-%m-%d %H:%M:%S", &tm_tm);
	return (rez>0) ? string(buf,rez) : "";
    }
    // int {strptime|strptimegm}(string str, string form = "%Y-%m-%d %H:%M:%S") - returns the time in seconds from the epoch of 1/1/1970,
    //      based on the string record of time <str>, in accordance with the specified template <form>
    //  str - source time in string
    //  form - string's time template in format POSIX-function "strptime"
    if((iid == "strptime" || (alt1=(iid=="strptimegm"))) && !prms.empty()) {
	struct tm stm;
	stm.tm_isdst = -1;
	strptime(prms[0].getS().c_str(), (prms.size()>=2) ? prms[1].getS().c_str() : "%Y-%m-%d %H:%M:%S", &stm);
	return (int64_t)(alt1 ? timegm(&stm) : mktime(&stm));
    }
    // int cron(string cronreq, int base = 0) - returns the time, planned in the format of the standard Cron <cronreq>,
    //      beginning from basic time <base> or from the current, if the basic is not specified
    //  cronreq - shedule in standard Cron format
    //  base - base time
    if(iid == "cron" && !prms.empty())
	return (int64_t)cron(prms[0].getS(), (prms.size()>=2) ? prms[1].getI() : 0);
    // string strFromCharCode(int char1, int char2, int char3, ...) - string creation from symbol's codes
    //  char1, char2. char3 - symbol's codes
    if(iid == "strFromCharCode") {
	string rez;
	for(unsigned i_p = 0; i_p < prms.size(); i_p++)
	    rez += (unsigned char)prms[i_p].getI();
	return rez;
    }
    // string strCodeConv( string src, string fromCP, string toCP ) - String text encode from codepage <fromCP> to codepage <toCP>.
    //  src - source text;
    //  fromCP - from codepage, empty for use internal codepage;
    //  toCP - to codepage, empty for use internal codepage.
    if(iid == "strCodeConv" && prms.size() >= 3)
	return Mess->codeConv((prms[1].getS().size() ? prms[1].getS() : Mess->charset()),
			(prms[2].getS().size() ? prms[2].getS() : Mess->charset()), prms[0].getS());
    // string strEncode( string src, string tp = "Bin", string opt1 = "" ) - String encode from <src> by <tp> and options <opt1>.
    //  src - source;
    //  tp  - encode type: "PathEl", "HttpURL", "HTML", "JavaScript", "SQL", "Custom", "Base64", "FormatPrint",
    //			   "OscdID", "Bin", "Reverse", "ShieldSimb"
    //  opt1 - option 1, symbols for "Custom"
    if(iid == "strEncode" && prms.size() >= 1) {
	string stp = (prms.size()>1) ? prms[1].getS() : "Bin";
	Code tp = (Code)0;
	if(stp == "PathEl")		tp = PathEl;
	else if(stp == "HttpURL")	tp = HttpURL;
	else if(stp == "HTML")		tp = Html;
	else if(stp == "JavaScript")	tp = JavaSc;
	else if(stp == "SQL")		tp = SQL;
	else if(stp == "Custom")	tp = Custom;
	else if(stp == "Base64")	tp = base64;
	else if(stp == "FormatPrint")	tp = FormatPrint;
	else if(stp == "OscdID")	tp = oscdID;
	else if(stp == "Bin")		tp = Bin;
	else if(stp == "Reverse")	tp = Reverse;
	else if(stp == "ShieldSimb")	tp = ShieldSimb;
	else return "";
	return strEncode(prms[0].getS(), tp, (prms.size()>2) ? prms[2].getS() : "");
    }
    // string strDecode( string src, string tp = "Bin", string opt1 = "" ) - String decode from <src> by <tp> and options <opt1>.
    //  src - source;
    //  tp  - encode type: "PathEl", "HttpURL", "Custom", "Base64", "Bin"
    //  opt1 - option 1, separator for "Bin"
    if(iid == "strDecode" && prms.size() >= 1) {
	string stp = (prms.size()>1) ? prms[1].getS() : "Bin";
	Code tp = (Code)0;
	if(stp == "PathEl")		tp = PathEl;
	else if(stp == "HttpURL")	tp = HttpURL;
	else if(stp == "Custom")	tp = Custom;
	else if(stp == "Base64")	tp = base64;
	else if(stp == "Bin")		tp = Bin;
	else return "";
	return strDecode(prms[0].getS(), tp, (prms.size()>2) ? prms[2].getS() : "");
    }

    return TCntrNode::objFuncCall(iid,prms,user);
}

void TSYS::ctrListFS( XMLNode *nd, const string &fsBaseIn, const string &fileExt )
{
    int pathLev = 0;

    //Source path check and normalize
    bool fromRoot = (fsBaseIn.size() && fsBaseIn[0] == '/');
    nd->childAdd("el")->setText("/");
    if(TSYS::pathLev(fsBaseIn,0) != ".") nd->childAdd("el")->setText(".");
    string fsBase, tEl;
    for(int off = 0; (tEl=TSYS::pathLev(fsBaseIn,0,false,&off)).size(); pathLev++) {
	fsBase += ((pathLev || fromRoot)?"/":"")+tEl;
	nd->childAdd("el")->setText(fsBase);
    }
    if(fromRoot && pathLev == 0) fsBase = "/";

    //Previous items set
    if(!fromRoot) {
	if(pathLev == 0) nd->childAdd("el")->setText("..");
	else if(TSYS::pathLev(fsBase,pathLev-1) == "..") nd->childAdd("el")->setText(fsBase+"/..");
    }

    //From work directory check
    string fsBaseCor = fsBase;
    if(!fsBaseCor.size() || (fsBaseCor[0] != '/' && fsBaseCor[0] != '.')) fsBaseCor = "./"+fsBaseCor;

    //Child items process
    vector<string> its, fits;
    DIR *IdDir = opendir(fsBaseCor.c_str());
    if(IdDir != NULL) {
	dirent	*sDirRez = NULL,
		*sDir = (dirent*)malloc(offsetof(dirent,d_name) + NAME_MAX + 1);
	while(readdir_r(IdDir,sDir,&sDirRez) == 0 && sDirRez) {
	    if(strcmp(sDirRez->d_name,"..") == 0 || strcmp(sDirRez->d_name,".") == 0) continue;
	    if(sDirRez->d_type == DT_DIR || sDirRez->d_type == DT_LNK ||
		    ((sDirRez->d_type == DT_CHR || sDirRez->d_type == DT_BLK) && fileExt.find(tEl+"<dev>;") != string::npos) ||
		    (sDirRez->d_type == DT_CHR && fileExt.find(tEl+"<chrdev>;") != string::npos))
		fits.push_back(sDirRez->d_name);
	    else if(sDirRez->d_type == DT_REG && fileExt.size()) {
		tEl = sDirRez->d_name;
		size_t extPos = tEl.rfind(".");
		tEl = (extPos != string::npos) ? tEl.substr(extPos+1) : "";
		if(fileExt == "*" || (tEl.size() && fileExt.find(tEl+";") != string::npos)) fits.push_back(sDirRez->d_name);
	    }
	}
	closedir(IdDir);
	free(sDir);
    }
    sort(its.begin(),its.end());
    for(unsigned i_it = 0; i_it < its.size(); i_it++)
	nd->childAdd("el")->setText(fsBase+(pathLev?"/":"")+its[i_it]);
    sort(fits.begin(),fits.end());
    for(unsigned i_it = 0; i_it < fits.size(); i_it++)
	nd->childAdd("el")->setText(fsBase+(pathLev?"/":"")+fits[i_it]);
}

void TSYS::cntrCmdProc( XMLNode *opt )
{
    char buf[STR_BUF_LEN];
    string u = opt->attr("user");
    string a_path = opt->attr("path");

    //Service commands process
    if(a_path == "/serv/redundant") {	//Redundant service requests
	if(ctrChkNode(opt,"st",RWRWR_,"root",SDAQ_ID,SEC_RD)) {	//State
	    opt->setAttr("StLevel", i2s(rdStLevel()));
	    return;
	}
    }

    //Get page info
    if(opt->name() == "info") {
	TCntrNode::cntrCmdProc(opt);
	snprintf(buf,sizeof(buf),_("%s station: \"%s\""),PACKAGE_NAME,trU(name(),u).c_str());
	ctrMkNode("oscada_cntr",opt,-1,"/",buf,R_R_R_)->setAttr("doc","AboutOpenSCADA|ProgrammManual");
	if(ctrMkNode("branches",opt,-1,"/br","",R_R_R_))
	    ctrMkNode("grp",opt,-1,"/br/sub_",_("Subsystem"),R_R_R_,"root","root",1,"idm","1");
	if(TUIS::icoGet(id(),NULL,true).size()) ctrMkNode("img",opt,-1,"/ico","",R_R_R_);
	if(ctrMkNode("area",opt,-1,"/gen",_("Station"),R_R_R_)) {
	    ctrMkNode("fld",opt,-1,"/gen/id",_("ID"),R_R_R_,"root","root",1,"tp","str");
	    ctrMkNode("fld",opt,-1,"/gen/stat",_("Station name"),RWRWR_,"root","root",1,"tp","str");
	    ctrMkNode("fld",opt,-1,"/gen/prog",_("Program"),R_R_R_,"root","root",1,"tp","str");
	    ctrMkNode("fld",opt,-1,"/gen/ver",_("Version"),R_R_R_,"root","root",1,"tp","str");
	    ctrMkNode("fld",opt,-1,"/gen/host",_("Host name"),R_R_R_,"root","root",1,"tp","str");
	    ctrMkNode("fld",opt,-1,"/gen/user",_("System user"),R_R_R_,"root","root",1,"tp","str");
	    ctrMkNode("fld",opt,-1,"/gen/sys",_("Operation system"),R_R_R_,"root","root",1,"tp","str");
	    ctrMkNode("fld",opt,-1,"/gen/CPU",_("CPU"),R_R_R_,"root","root",1,"tp","str");
	    if(nCPU() > 1)
		ctrMkNode("fld",opt,-1,"/gen/mainCPUs",_("Main CPUs set"),RWRWR_,"root","root",2,"tp","str",
		    "help",_("For using CPU set write processors numbers string separated by symbol ':'.\nCPU numbers started from 0."));
	    ctrMkNode("fld",opt,-1,"/gen/clk",_("Tasks planning clock"),R_R_R_,"root","root",1,"tp","str");
	    ctrMkNode("fld",opt,-1,"/gen/in_charset",_("Internal charset"),R_R___,"root","root",1,"tp","str");
	    ctrMkNode("fld",opt,-1,"/gen/config",_("Config-file"),R_R___,"root","root",1,"tp","str");
	    ctrMkNode("fld",opt,-1,"/gen/workdir",_("Work directory"),R_R___,"root","root",3,"tp","str","dest","sel_ed","select","/gen/workDirList");
	    ctrMkNode("fld",opt,-1,"/gen/moddir",_("Modules directory"),R_R___,"root","root",3,"tp","str","dest","sel_ed","select","/gen/modDirList");
	    ctrMkNode("fld",opt,-1,"/gen/icodir",_("Icons directory"),R_R___,"root","root",4,"tp","str","dest","sel_ed","select","/gen/icoDirList",
		"help",_("Separate directory paths with icons by symbol ';'."));
	    ctrMkNode("fld",opt,-1,"/gen/docdir",_("Documents directory"),R_R___,"root","root",4,"tp","str","dest","sel_ed","select","/gen/docDirList",
		"help",_("Separate directory paths with documents by symbol ';'."));
	    ctrMkNode("fld",opt,-1,"/gen/wrk_db",_("Work DB"),RWRWR_,"root","root",4,"tp","str","dest","select","select","/db/list",
		"help",_("Work DB address in format [<DB module>.<DB name>].\nChange it field if you want save or reload all system from other DB."));
	    ctrMkNode("fld",opt,-1,"/gen/saveExit",_("Save the system at exit"),RWRWR_,"root","root",2,"tp","bool",
		"help",_("Select for automatic system saving to DB on exit."));
	    ctrMkNode("fld",opt,-1,"/gen/savePeriod",_("Save the system period"),RWRWR_,"root","root",2,"tp","dec",
		"help",_("Use no zero period (seconds) for periodic saving of changed systems parts to DB."));
	    ctrMkNode("fld",opt,-1,"/gen/lang",_("Language"),RWRWR_,"root","root",1,"tp","str");
	    if(ctrMkNode("area",opt,-1,"/gen/mess",_("Messages"),R_R_R_)) {
		ctrMkNode("fld",opt,-1,"/gen/mess/lev",_("Least level"),RWRWR_,"root","root",6,"tp","dec","len","1","dest","select",
		    "sel_id","0;1;2;3;4;5;6;7",
		    "sel_list",_("Debug (0);Information (1);Notice (2);Warning (3);Error (4);Critical (5);Alert (6);Emergency (7)"),
		    "help",_("Least messages level which process by the system."));
		ctrMkNode("fld",opt,-1,"/gen/mess/log_sysl",_("To syslog"),RWRWR_,"root","root",1,"tp","bool");
		ctrMkNode("fld",opt,-1,"/gen/mess/log_stdo",_("To stdout"),RWRWR_,"root","root",1,"tp","bool");
		ctrMkNode("fld",opt,-1,"/gen/mess/log_stde",_("To stderr"),RWRWR_,"root","root",1,"tp","bool");
		ctrMkNode("fld",opt,-1,"/gen/mess/log_arch",_("To archive"),RWRWR_,"root","root",1,"tp","bool");
	    }
	}
	if(ctrMkNode("area",opt,-1,"/subs",_("Subsystems")))
	    ctrMkNode("list",opt,-1,"/subs/br",_("Subsystems"),R_R_R_,"root","root",3,"idm","1","tp","br","br_pref","sub_");
	if(ctrMkNode("area",opt,-1,"/redund",_("Redundancy"))) {
	    ctrMkNode("fld",opt,-1,"/redund/status",_("Status"),R_R_R_,"root","root",1,"tp","str");
	    ctrMkNode("fld",opt,-1,"/redund/statLev",_("Station level"),RWRWR_,"root","root",1,"tp","dec");
	    ctrMkNode("fld",opt,-1,"/redund/tskPer",_("Redundant task period (s)"),RWRWR_,"root","root",1,"tp","real");
	    ctrMkNode("fld",opt,-1,"/redund/restConn",_("Restore connection timeout (s)"),RWRWR_,"root","root",1,"tp","dec");
	    ctrMkNode("fld",opt,-1,"/redund/primCmdTr",_("Local primary commands transfer"),RWRWR_,"root","root",1,"tp","bool");
	    if(ctrMkNode("table",opt,-1,"/redund/sts",_("Stations"),RWRWR_,"root","root",2,"key","st","s_com","add,del")) {
		ctrMkNode("list",opt,-1,"/redund/sts/st",_("ID"),RWRWR_,"root","root",3,"tp","str","dest","select","select","/redund/lsSt");
		ctrMkNode("list",opt,-1,"/redund/sts/name",_("Name"),R_R_R_,"root","root",1,"tp","str");
		ctrMkNode("list",opt,-1,"/redund/sts/live",_("Live"),R_R_R_,"root","root",1,"tp","bool");
		ctrMkNode("list",opt,-1,"/redund/sts/lev",_("Lev."),R_R_R_,"root","root",1,"tp","dec");
		ctrMkNode("list",opt,-1,"/redund/sts/cnt",_("Cntr."),R_R_R_,"root","root",1,"tp","real");
	    }
	    ctrMkNode("comm",opt,-1,"/redund/hostLnk",_("Go to remote stations list configuration"),0660,"root","Transport",1,"tp","lnk");
	}
	if(ctrMkNode("area",opt,-1,"/tasks",_("Tasks"),R_R___))
	    if(ctrMkNode("table",opt,-1,"/tasks/tasks",_("Tasks"),RWRW__,"root","root",2,"key","path",
		"help",(nCPU()<=1)?"":_("For an using CPU set write processors numbers string separated by symbol ':'.\n"
				       "CPU numbers started from 0.")))
	    {
		ctrMkNode("list",opt,-1,"/tasks/tasks/path",_("Path"),R_R___,"root","root",1,"tp","str");
		ctrMkNode("list",opt,-1,"/tasks/tasks/thrd",_("Thread"),R_R___,"root","root",1,"tp","str");
		ctrMkNode("list",opt,-1,"/tasks/tasks/tid",_("TID"),R_R___,"root","root",1,"tp","dec");
		ctrMkNode("list",opt,-1,"/tasks/tasks/stat",_("Status"),R_R___,"root","root",1,"tp","str");
		ctrMkNode("list",opt,-1,"/tasks/tasks/plc",_("Policy"),R_R___,"root","root",1,"tp","str");
		ctrMkNode("list",opt,-1,"/tasks/tasks/prior",_("Prior."),R_R___,"root","root",1,"tp","dec");
#if __GLIBC_PREREQ(2,4)
		if(nCPU() > 1) ctrMkNode("list",opt,-1,"/tasks/tasks/cpuSet",_("CPU set"),RWRW__,"root","root",1,"tp","str");
#endif
	    }
	if(ctrMkNode("area",opt,-1,"/tr",_("Translations"))) {
	    ctrMkNode("fld",opt,-1,"/tr/baseLang",_("Text variable's base language"),RWRWR_,"root","root",5,
		"tp","str","len","2","dest","sel_ed","select","/tr/baseLangLs",
		"help",_("Multilingual for variable texts support enabling by base language selection."));
	    if(Mess->lang2CodeBase().size()) {
		ctrMkNode("fld",opt,-1,"/tr/dyn",_("Dynamic translation"),R_R_R_,"root","root",2,"tp","bool","help",_("Current dynamic translation state."));
		ctrMkNode("fld",opt,-1,"/tr/dynPlan","",RWRW__,"root","root",2,"tp","bool","help",_("Plan for dynamic translation at next start."));
		if(!Mess->translDyn())
		    ctrMkNode("fld",opt,-1,"/tr/enMan",_("Enable manager"),RWRWR_,"root","root",2,"tp","bool",
			"help",_("Enable common translation manage which cause full reloading for all built messages obtain."));
	    }
	    if(Mess->translEnMan()) {
		ctrMkNode("fld",opt,-1,"/tr/langs",_("Languages"),RWRWR_,"root","root",2,"tp","str",
		    "help",_("Processed languages list by two symbols code and separated symbol ';'."));
		ctrMkNode("fld",opt,-1,"/tr/fltr",_("Source filter"),RWRWR_,"root","root",1,"tp","str");
		ctrMkNode("fld",opt,-1,"/tr/chkUnMatch",_("Check for mismatch"),RWRWR_,"root","root",1,"tp","bool");
		if(ctrMkNode("table",opt,-1,"/tr/mess",_("Messages"),RWRWR_,"root","root",1,"key","base")) {
		    ctrMkNode("list",opt,-1,"/tr/mess/base",Mess->lang2CodeBase().c_str(),RWRWR_,"root","root",1,"tp","str");
		    string lngEl;
		    for(int off = 0; (lngEl=strParse(Mess->translLangs(),0,";",&off)).size(); )
			if(lngEl.size() == 2 && lngEl != Mess->lang2CodeBase())
			    ctrMkNode("list",opt,-1,("/tr/mess/"+lngEl).c_str(),lngEl.c_str(),RWRWR_,"root","root",1,"tp","str");
		    ctrMkNode("list",opt,-1,"/tr/mess/src",_("Source"),R_R_R_,"root","root",1,"tp","str");
		}
	    }
	}
	if((mess_lev() == TMess::Debug || !cntrEmpty()) && ctrMkNode("area",opt,-1,"/debug",_("Debug"))) {
	    if(!cntrEmpty() && ctrMkNode("table",opt,-1,"/debug/cntr",_("Counters"),R_R_R_,"root","root")) {
		ctrMkNode("list",opt,-1,"/debug/cntr/id","ID",R_R_R_,"root","root",1,"tp","str");
		ctrMkNode("list",opt,-1,"/debug/cntr/vl",_("Value"),R_R_R_,"root","root",1,"tp","real");
	    }
	    if(ctrMkNode("table",opt,-1,"/debug/dbgCats",_("Debug categories"),RWRWR_,"root","root",1,"key","cat")) {
		ctrMkNode("list",opt,-1,"/debug/dbgCats/cat",_("Category"),R_R_R_,"root","root",1,"tp","str");
		ctrMkNode("list",opt,-1,"/debug/dbgCats/prc",_("Process"),RWRWR_,"root","root",1,"tp","bool");
	    }
	    ctrMkNode("comm",opt,-1,"/debug/mess",_("See to messages"),R_R_R_,"root","root",1,"tp","lnk");
	}
	return;
    }

    //Process command to page
    if(a_path == "/ico" && ctrChkNode(opt)) {
	string itp;
	opt->setText(TSYS::strEncode(TUIS::icoGet(id(),&itp),TSYS::base64));
	opt->setAttr("tp",itp);
    }
    else if(a_path == "/gen/host" && ctrChkNode(opt))	opt->setText(host());
    else if(a_path == "/gen/sys" && ctrChkNode(opt)) {
	utsname ubuf; uname(&ubuf);
	opt->setText(string(ubuf.sysname)+"-"+ubuf.release);
    }
    else if(a_path == "/gen/user" && ctrChkNode(opt))	opt->setText(mUser);
    else if(a_path == "/gen/prog" && ctrChkNode(opt))	opt->setText(PACKAGE_NAME);
    else if(a_path == "/gen/ver" && ctrChkNode(opt))	opt->setText(VERSION);
    else if(a_path == "/gen/id" && ctrChkNode(opt))	opt->setText(id());
    else if(a_path == "/gen/stat") {
	if(ctrChkNode(opt,"get",RWRWR_,"root","root",SEC_RD))	opt->setText(trU(name(),u));
	if(ctrChkNode(opt,"set",RWRWR_,"root","root",SEC_WR))	setName(trSetU(name(),u,opt->text()));
    }
    else if(a_path == "/gen/CPU" && ctrChkNode(opt))	opt->setText(strMess(_("%dx%0.3gGHz"),nCPU(),(float)sysClk()/1e9));
    else if(a_path == "/gen/mainCPUs") {
	if(ctrChkNode(opt,"get",RWRWR_,"root","root",SEC_RD)) {
	    string vl = mainCPUs();
#if __GLIBC_PREREQ(2,4)
	    cpu_set_t cpuset;
	    CPU_ZERO(&cpuset);
	    pthread_getaffinity_np(mainPthr, sizeof(cpu_set_t), &cpuset);
	    vl += "(";
	    for(int iCPU = 0; iCPU < nCPU(); iCPU++)
		if(CPU_ISSET(iCPU,&cpuset)) vl += string(vl[vl.size()-1]=='('?"":":")+i2s(iCPU);
	    vl += ")";
#endif
	    opt->setText(vl);
	}
	if(ctrChkNode(opt,"set",RWRWR_,"root","root",SEC_WR))	setMainCPUs(TSYS::strParse(opt->text(),0,"("));
    }
    else if(a_path == "/gen/clk" && ctrChkNode(opt)) {
	struct timespec tmval;
	clock_getres(SYS->clockRT()?CLOCK_REALTIME:CLOCK_MONOTONIC, &tmval);
	opt->setText(TSYS::strMess(SYS->clockRT()?_("Real-time %s"):_("Monotonic %s"),tm2s(1e-9*tmval.tv_nsec).c_str()));
    }
    else if(a_path == "/gen/in_charset" && ctrChkNode(opt))	opt->setText(Mess->charset());
    else if(a_path == "/gen/config") {
	if(ctrChkNode(opt))	opt->setText(mConfFile);
	if(ctrChkNode(opt,"scan",RWRWR_,"root","root",SEC_WR))	cfgFileScan(false, true);
    }
    else if(a_path == "/gen/wrk_db") {
	if(ctrChkNode(opt,"get",RWRWR_,"root","root",SEC_RD))	opt->setText(workDB());
	if(ctrChkNode(opt,"set",RWRWR_,"root","root",SEC_WR))	setWorkDB(opt->text());
    }
    else if(a_path == "/gen/saveExit") {
	if(ctrChkNode(opt,"get",RWRWR_,"root","root",SEC_RD))	opt->setText(i2s(saveAtExit()));
	if(ctrChkNode(opt,"set",RWRWR_,"root","root",SEC_WR))	setSaveAtExit(s2i(opt->text()));
    }
    else if(a_path == "/gen/savePeriod") {
	if(ctrChkNode(opt,"get",RWRWR_,"root","root",SEC_RD))	opt->setText(i2s(savePeriod()));
	if(ctrChkNode(opt,"set",RWRWR_,"root","root",SEC_WR))	setSavePeriod(s2i(opt->text()));
    }
    else if(a_path == "/gen/workdir") {
	if(ctrChkNode(opt,"get",RWRW__,"root","root",SEC_RD))	opt->setText(workDir());
	if(ctrChkNode(opt,"set",RWRW__,"root","root",SEC_WR))	setWorkDir(opt->text().c_str());
    }
    else if(a_path == "/gen/workDirList" && ctrChkNode(opt))	ctrListFS(opt, workDir());
    else if(a_path == "/gen/moddir") {
	if(ctrChkNode(opt,"get",RWRW__,"root","root",SEC_RD))	opt->setText(modDir());
	if(ctrChkNode(opt,"set",RWRW__,"root","root",SEC_WR))	setModDir(opt->text().c_str());
    }
    else if(a_path == "/gen/modDirList" && ctrChkNode(opt))	ctrListFS(opt, modDir());
    else if(a_path == "/gen/icodir") {
	if(ctrChkNode(opt,"get",RWRW__,"root","root",SEC_RD))	opt->setText(icoDir());
	if(ctrChkNode(opt,"set",RWRW__,"root","root",SEC_WR))	setIcoDir(opt->text().c_str());
    }
    else if(a_path == "/gen/icoDirList" && ctrChkNode(opt))	ctrListFS(opt, TSYS::strParse(icoDir(),0,";"));
    else if(a_path == "/gen/docdir") {
	if(ctrChkNode(opt,"get",RWRW__,"root","root",SEC_RD))	opt->setText(docDir());
	if(ctrChkNode(opt,"set",RWRW__,"root","root",SEC_WR))	setDocDir(opt->text().c_str());
    }
    else if(a_path == "/gen/docDirList" && ctrChkNode(opt))	ctrListFS(opt, TSYS::strParse(docDir(),0,";"));
    else if(a_path == "/gen/lang") {
	if(ctrChkNode(opt,"get",RWRWR_,"root","root",SEC_RD))	opt->setText(Mess->lang());
	if(ctrChkNode(opt,"set",RWRWR_,"root","root",SEC_WR))	Mess->setLang(opt->text());
    }
    else if(a_path == "/gen/mess/lev") {
	if(ctrChkNode(opt,"get",RWRWR_,"root","root",SEC_RD))	opt->setText(i2s(Mess->messLevel()));
	if(ctrChkNode(opt,"set",RWRWR_,"root","root",SEC_WR))	Mess->setMessLevel(s2i(opt->text()));
    }
    else if(a_path == "/gen/mess/log_sysl") {
	if(ctrChkNode(opt,"get",RWRWR_,"root","root",SEC_RD))	opt->setText((Mess->logDirect()&0x01)?"1":"0");
	if(ctrChkNode(opt,"set",RWRWR_,"root","root",SEC_WR))	Mess->setLogDirect(s2i(opt->text())?Mess->logDirect()|0x01:Mess->logDirect()&(~0x01));
    }
    else if(a_path == "/gen/mess/log_stdo") {
	if(ctrChkNode(opt,"get",RWRWR_,"root","root",SEC_RD))	opt->setText((Mess->logDirect()&0x02)?"1":"0");
	if(ctrChkNode(opt,"set",RWRWR_,"root","root",SEC_WR))	Mess->setLogDirect(s2i(opt->text())?Mess->logDirect()|0x02:Mess->logDirect()&(~0x02));
    }
    else if(a_path == "/gen/mess/log_stde") {
	if(ctrChkNode(opt,"get",RWRWR_,"root","root",SEC_RD))	opt->setText((Mess->logDirect()&0x04)?"1":"0");
	if(ctrChkNode(opt,"set",RWRWR_,"root","root",SEC_WR))	Mess->setLogDirect(s2i(opt->text())?Mess->logDirect()|0x04:Mess->logDirect()&(~0x04));
    }
    else if(a_path == "/gen/mess/log_arch") {
	if(ctrChkNode(opt,"get",RWRWR_,"root","root",SEC_RD))	opt->setText((Mess->logDirect()&0x08)?"1":"0");
	if(ctrChkNode(opt,"set",RWRWR_,"root","root",SEC_WR))	Mess->setLogDirect(s2i(opt->text())?Mess->logDirect()|0x08:Mess->logDirect()&(~0x08));
    }
    else if((a_path == "/br/sub_" || a_path == "/subs/br") && ctrChkNode(opt,"get",R_R_R_,"root","root",SEC_RD)) {
	vector<string> lst;
	list(lst);
	for(unsigned i_a = 0; i_a < lst.size(); i_a++)
	    opt->childAdd("el")->setAttr("id",lst[i_a])->setText(at(lst[i_a]).at().subName());
    }
    else if(a_path == "/redund/status" && ctrChkNode(opt,"get",R_R_R_,"root","root"))
	opt->setText(TSYS::strMess(_("Spent time: %s."),tm2s(1e-6*mRdPrcTm).c_str()));
    else if(a_path == "/redund/statLev") {
	if(ctrChkNode(opt,"get",RWRWR_,"root","root",SEC_RD))	opt->setText(i2s(rdStLevel()));
	if(ctrChkNode(opt,"set",RWRWR_,"root","root",SEC_WR))	setRdStLevel(s2i(opt->text()));
    }
    else if(a_path == "/redund/tskPer") {
	if(ctrChkNode(opt,"get",RWRWR_,"root","root",SEC_RD))	opt->setText(r2s(rdTaskPer()));
	if(ctrChkNode(opt,"set",RWRWR_,"root","root",SEC_WR))	setRdTaskPer(s2r(opt->text()));
    }
    else if(a_path == "/redund/restConn") {
	if(ctrChkNode(opt,"get",RWRWR_,"root","root",SEC_RD))	opt->setText(i2s(rdRestConnTm()));
	if(ctrChkNode(opt,"set",RWRWR_,"root","root",SEC_WR))	setRdRestConnTm(s2i(opt->text()));
    }
    else if(a_path == "/redund/primCmdTr") {
	if(ctrChkNode(opt,"get",RWRWR_,"root","root",SEC_RD))	opt->setText(i2s(rdPrimCmdTr()));
	if(ctrChkNode(opt,"set",RWRWR_,"root","root",SEC_WR))	setRdPrimCmdTr(s2i(opt->text()));
    }
    else if(a_path == "/redund/sts") {
	if(ctrChkNode(opt,"get",RWRWR_,"root","root",SEC_RD)) {
	    XMLNode *nSt	= ctrMkNode("list",opt,-1,"/redund/sts/st","",RWRWR_,"root","root");
	    XMLNode *nName	= ctrMkNode("list",opt,-1,"/redund/sts/name","",R_R_R_,"root","root");
	    XMLNode *nLive	= ctrMkNode("list",opt,-1,"/redund/sts/live","",R_R_R_,"root","root");
	    XMLNode *nLev	= ctrMkNode("list",opt,-1,"/redund/sts/lev","",R_R_R_,"root","root");
	    XMLNode *nCnt	= ctrMkNode("list",opt,-1,"/redund/sts/cnt","",R_R_R_,"root","root");

	    ResAlloc res(mRdRes, false);
	    for(map<string,TSYS::SStat>::iterator sit = mSt.begin(); sit != mSt.end(); sit++) {
		if(nSt)		nSt->childAdd("el")->setText(sit->first);
		if(nName)	nName->childAdd("el")->setText(SYS->transport().at().extHostGet("*",sit->first).name);
		if(nLive)	nLive->childAdd("el")->setText(sit->second.isLive?"1":"0");
		if(nLev)	nLev->childAdd("el")->setText(i2s(sit->second.lev));
		if(nCnt)	nCnt->childAdd("el")->setText(r2s(sit->second.cnt));
	    }
	}
	else {
	    ResAlloc res(mRdRes, true);
	    if(ctrChkNode(opt,"add",RWRWR_,"root","root",SEC_WR))	{ mSt[_("<newStat>")] = SStat(0); modif(); }
	    else if(ctrChkNode(opt,"del",RWRWR_,"root","root",SEC_WR))	{ mSt.erase(opt->attr("key_st")); modif(); }
	    else if(ctrChkNode(opt,"set",RWRWR_,"root","root",SEC_WR) && opt->attr("col") == "st") {
		mSt.erase(opt->attr("key_st"));
		mSt[opt->text()] = SStat();
		modif();
	    }
	}
    }
    else if(a_path == "/redund/lsSt" && ctrChkNode(opt)) {
	vector<TTransportS::ExtHost> hls;
	SYS->transport().at().extHostList("*", hls);
	for(unsigned iH = 0; iH < hls.size(); iH++)
	    opt->childAdd("el")->setText(hls[iH].id);
    }
    else if(a_path == "/redund/hostLnk" && ctrChkNode(opt,"get",0660,"root","Transport",SEC_RD)) opt->setText("/Transport");
    else if(a_path == "/tasks/tasks") {
	if(ctrChkNode(opt,"get",RWRW__,"root","root")) {
	    XMLNode *n_path	= ctrMkNode("list",opt,-1,"/tasks/tasks/path","",R_R___,"root","root");
	    XMLNode *n_thr	= ctrMkNode("list",opt,-1,"/tasks/tasks/thrd","",R_R___,"root","root");
	    XMLNode *n_tid	= ctrMkNode("list",opt,-1,"/tasks/tasks/tid","",R_R___,"root","root");
	    XMLNode *n_stat	= ctrMkNode("list",opt,-1,"/tasks/tasks/stat","",R_R___,"root","root");
	    XMLNode *n_plc	= ctrMkNode("list",opt,-1,"/tasks/tasks/plc","",R_R___,"root","root");
	    XMLNode *n_prior	= ctrMkNode("list",opt,-1,"/tasks/tasks/prior","",R_R___,"root","root");
	    XMLNode *n_cpuSet	= (nCPU()>1) ? ctrMkNode("list",opt,-1,"/tasks/tasks/cpuSet","",RWRW__,"root","root") : NULL;

	    ResAlloc res(taskRes, false);
	    for(map<string,STask>::iterator it = mTasks.begin(); it != mTasks.end(); it++) {
		if(n_path)	n_path->childAdd("el")->setText(it->first);
		if(n_thr)	n_thr->childAdd("el")->setText(u2s(it->second.thr));
		if(n_tid)	n_tid->childAdd("el")->setText(i2s(it->second.tid));
		if(n_stat) {
		    int64_t	tm_beg = 0, tm_end = 0, tm_per = 0, tm_pnt = 0, lagMax = 0, consMax = 0;
		    for(int i_tr = 0; tm_beg == tm_per && i_tr < 2; i_tr++) {
			tm_beg = it->second.tm_beg; tm_end = it->second.tm_end;
			tm_per = it->second.tm_per; tm_pnt = it->second.tm_pnt;
			lagMax = it->second.lagMax; consMax = it->second.consMax;
		    }
		    XMLNode *cn = n_stat->childAdd("el");
		    if(it->second.flgs&STask::FinishTask) cn->setText(_("Finished. "));
		    if(tm_beg && tm_beg < tm_per)
			cn->setText(cn->text()+TSYS::strMess(_("Last: %s. Consume: %3.1f[%3.1f]%% (%s from %s). Lag: %s[%s]. Cycles lost: %g."),
			    atm2s((time_t)(1e-9*tm_per),SYS->clockRT()?"%d-%m-%Y %H:%M:%S":"%d-%m %H:%M:%S").c_str(),
			    100*(double)(tm_end-tm_beg)/(double)(tm_per-tm_beg),
			    100*(double)consMax/(double)(tm_per-tm_beg), tm2s(1e-9*(tm_end-tm_beg)).c_str(), tm2s(1e-9*(tm_per-tm_beg)).c_str(),
			    tm2s(1e-9*(tm_per-tm_pnt)).c_str(), tm2s(1e-9*lagMax).c_str(), (double)it->second.cycleLost));
		}
		if(n_plc) {
		    string plcVl = _("Standard");
		    if(it->second.policy == SCHED_FIFO)		plcVl = _("RT FIFO");
		    else if(it->second.policy == SCHED_RR)	plcVl = _("RT Round-robin");
#if __GLIBC_PREREQ(2,4)
		    else if(it->second.policy == SCHED_BATCH)	plcVl = _("Style \"batch\"");
#endif
		    n_plc->childAdd("el")->setText(plcVl);
		}
		if(n_prior)	n_prior->childAdd("el")->setText(i2s(it->second.prior));
		if(n_cpuSet) {
		    string vl = it->second.cpuSet;
#if __GLIBC_PREREQ(2,4)
		    cpu_set_t cpuset;
		    CPU_ZERO(&cpuset);
		    pthread_getaffinity_np(it->second.thr, sizeof(cpu_set_t), &cpuset);
		    vl += "(";
		    for(int iCPU = 0; iCPU < nCPU(); iCPU++)
			if(CPU_ISSET(iCPU,&cpuset)) vl += string(vl[vl.size()-1]=='('?"":":")+i2s(iCPU);
		    vl += ")";
#endif
		    n_cpuSet->childAdd("el")->setText(vl);
		}
	    }
	}
#if __GLIBC_PREREQ(2,4)
	if(nCPU() > 1 && ctrChkNode(opt,"set",RWRW__,"root","root",SEC_WR) && opt->attr("col") == "cpuSet") {
	    ResAlloc res(taskRes, true);
	    map<string,STask>::iterator it = mTasks.find(opt->attr("key_path"));
	    if(it == mTasks.end()) throw err_sys(_("No present task '%s'."));
	    if(it->second.flgs & STask::Detached) return;

	    it->second.cpuSet = TSYS::strParse(opt->text(),0,"(");

	    cpu_set_t cpuset;
	    CPU_ZERO(&cpuset);
	    string sval;
	    if(!it->second.cpuSet.size()) for(int iCPU = 0; iCPU < nCPU(); iCPU++) CPU_SET(iCPU, &cpuset);
	    else {
		string prcVl;
		for(int off = 0; (sval=TSYS::strParse(it->second.cpuSet,0,":",&off)).size(); ) {
		    if(s2i(sval) < nCPU()) CPU_SET(s2i(sval), &cpuset);
		    prcVl += (prcVl.size()?":":"") + i2s(s2i(sval));
		}
		it->second.cpuSet = prcVl;
	    }

	    int rez = pthread_setaffinity_np(it->second.thr, sizeof(cpu_set_t), &cpuset);
	    res.release();
	    TBDS::genDBSet(nodePath()+"CpuSet:"+it->first, it->second.cpuSet);
	    if(rez == EINVAL) throw err_sys(_("Set not allowed processors try."));
	    if(rez) throw err_sys(_("CPUs set for the thread error."));
	}
#endif
    }
    else if(a_path == "/tr/baseLang") {
	if(ctrChkNode(opt,"get",RWRWR_,"root","root",SEC_RD))	opt->setText(Mess->lang2CodeBase());
	if(ctrChkNode(opt,"set",RWRWR_,"root","root",SEC_WR))	Mess->setLang2CodeBase(opt->text());
    }
    else if(a_path == "/tr/baseLangLs" && ctrChkNode(opt)) {
	opt->childAdd("el")->setText(Mess->lang2Code());
	if(!Mess->lang2CodeBase().empty() && Mess->lang2CodeBase() != Mess->lang2Code())
	    opt->childAdd("el")->setText(Mess->lang2CodeBase());
	opt->childAdd("el")->setText("");
    }
    else if(a_path == "/tr/dyn" && ctrChkNode(opt))	opt->setText(i2s(Mess->translDyn()));
    else if(a_path == "/tr/dynPlan") {
	if(ctrChkNode(opt,"get",RWRW__,"root","root",SEC_RD))	opt->setText(i2s(Mess->translDyn(true)));
	if(ctrChkNode(opt,"set",RWRW__,"root","root",SEC_WR))	Mess->setTranslDyn(s2i(opt->text()));
    }
    else if(a_path == "/tr/enMan") {
	if(ctrChkNode(opt,"get",RWRWR_,"root","root",SEC_RD))	opt->setText(i2s(Mess->translEnMan()));
	if(ctrChkNode(opt,"set",RWRWR_,"root","root",SEC_WR))	Mess->setTranslEnMan(s2i(opt->text()));
    }
    else if(a_path == "/tr/langs") {
	if(ctrChkNode(opt,"get",RWRWR_,"root","root",SEC_RD))	opt->setText(Mess->translLangs());
	if(ctrChkNode(opt,"set",RWRWR_,"root","root",SEC_WR))	Mess->setTranslLangs(opt->text());
    }
    else if(a_path == "/tr/fltr") {
	if(ctrChkNode(opt,"get",RWRWR_,"root","root",SEC_RD))	opt->setText(TBDS::genDBGet(nodePath()+"TrFltr","",opt->attr("user")));
	if(ctrChkNode(opt,"set",RWRWR_,"root","root",SEC_WR))	TBDS::genDBSet(nodePath()+"TrFltr",opt->text(),opt->attr("user"));
    }
    else if(a_path == "/tr/chkUnMatch") {
	if(ctrChkNode(opt,"get",RWRWR_,"root","root",SEC_RD))	opt->setText(TBDS::genDBGet(nodePath()+"TrChkUnMatch","0",opt->attr("user")));
	if(ctrChkNode(opt,"set",RWRWR_,"root","root",SEC_WR))	TBDS::genDBSet(nodePath()+"TrChkUnMatch",opt->text(),opt->attr("user"));
    }
    else if(a_path == "/tr/mess") {
	if(ctrChkNode(opt,"get",RWRWR_,"root","root",SEC_RD)) {
	    bool chkUnMatch = s2i(TBDS::genDBGet(nodePath()+"TrChkUnMatch","0",opt->attr("user")));
	    string tStr, trFltr = TBDS::genDBGet(nodePath()+"TrFltr","",opt->attr("user"));
	    TConfig req;
	    vector<XMLNode*> ns;

	    // Columns list prepare
	    ns.push_back(ctrMkNode("list",opt,-1,"/tr/mess/base","",RWRWR_,"root","root"));
	    for(int off = 0; (tStr=strParse(Mess->translLangs(),0,";",&off)).size(); )
		if(tStr.size() == 2 && tStr != Mess->lang2CodeBase())
		    ns.push_back(ctrMkNode("list",opt,-1,("/tr/mess/"+tStr).c_str(),tStr.c_str(),RWRWR_,"root","root"));
	    ns.push_back(ctrMkNode("list",opt,-1,"/tr/mess/src","",R_R_R_,"root","root"));

	    // Values request from first source
	    MtxAlloc res(Mess->mRes, true);
	    for(map<string, map<string,string> >::iterator im = Mess->trMessIdx.begin(); im != Mess->trMessIdx.end(); ++im) {
		//  Check for filter
		if(trFltr.size()) {
		    map<string,string>::iterator is;
		    for(is = im->second.begin(); is != im->second.end() && (is->first+"#"+is->second).find(trFltr) == string::npos; ++is) ;
		    if(is == im->second.end()) continue;
		}

		//  Rows append
		for(unsigned i_n = 0; i_n < ns.size(); i_n++) {
		    if(i_n == 0) ns[i_n]->childAdd("el")->setText(im->first);
		    else if(i_n < (ns.size()-1)) ns[i_n]->childAdd("el")->setText("");	//Empty cell at start
		    else {
			tStr.clear();
			for(map<string,string>::iterator is = im->second.begin(); is != im->second.end(); ++is)
			    tStr += (tStr.size()?"\n":"")+is->first;// + "#" + is->second;
			ns[i_n]->childAdd("el")->setText(tStr);
		    }
		}
		if(ns.size() <= 2) continue;	//No any translated languages set

		//  Real translated data obtain and check
		for(map<string,string>::iterator is = im->second.begin(); is != im->second.end(); ++is) {
		    string tMath, trSrc = TSYS::strParse(is->first,0,"#"), trFld = TSYS::strParse(is->first,1,"#");
		    bool firstInst = (is == im->second.begin()), isCfg = false, haveMatch = false;
		    //  Source is config file or included DB
		    if((isCfg=trSrc.compare(0,4,"cfg:")==0) || trSrc.compare(0,3,"db:") == 0) {
			//  Need DB structure prepare
			req.elem().fldClear();
			req.elem().fldAdd(new TFld(trFld.c_str(),trFld.c_str(),TFld::String,0));
			for(unsigned i_n = 1; i_n < ns.size(); i_n++)
			    req.elem().fldAdd(new TFld(Mess->translFld(ns[i_n]->attr("id"),trFld,isCfg).c_str(),
				ns[i_n]->attr("descr").c_str(),TFld::String,0));
			req.cfg(trFld).setReqKey(true);
			req.cfg(trFld).setS(im->first);

			//  Get from config file or DB source
			bool seekRez = false;
			for(int inst = 0; true; inst++) {
			    vector< vector<string> > full;
			    seekRez = isCfg ? SYS->db().at().dataSeek("", trSrc.substr(4), inst, req, false, &full)
					    : SYS->db().at().dataSeek(trSrc.substr(3), "", inst, req, false, &full);
			    if(!seekRez) break;
			    for(unsigned i_n = 0; i_n < ns.size(); i_n++) {
				if(!(i_n && i_n < (ns.size()-1))) continue;
				tMath = req.cfg(Mess->translFld(ns[i_n]->attr("id"),trFld,isCfg)).getS();
				XMLNode *recNd = ns[i_n]->childGet(-1);
				if(firstInst) { recNd->setText(tMath); haveMatch = true; }
				else {
				    if(!s2i(recNd->attr("unmatch"))) {
					if(tMath == recNd->text()) haveMatch = true;
					else {
					    if(recNd->text().empty()) recNd->setText(tMath);
					    recNd->setText(_("<<<Unmatched sources!!!>>>\n")+recNd->text());
					    recNd->setAttr("unmatch","1");
					}
				    }
				}
			    }
			    if(!chkUnMatch || !haveMatch) break;
			}
		    } else haveMatch = true;
		    if(!chkUnMatch || !haveMatch) break;
		}
	    }
	}
	if(ctrChkNode(opt,"set",RWRWR_,"root","root",SEC_WR)) {
	    bool needReload = false;
	    Mess->translSet(opt->attr("key_base"), ((opt->attr("col")=="base")?Mess->lang2CodeBase():opt->attr("col")), opt->text(), &needReload);
	    if(!needReload) opt->setAttr("noReload","1");
	}
    }
    else if(!cntrEmpty() && a_path == "/debug/cntr" && ctrChkNode(opt,"get",R_R_R_,"root","root")) {
	XMLNode *n_id	= ctrMkNode("list",opt,-1,"/debug/cntr/id","",R_R_R_,"root","root");
	XMLNode *n_vl	= ctrMkNode("list",opt,-1,"/debug/cntr/vl","",R_R_R_,"root","root");

	MtxAlloc res(dataRes(), true);
	for(map<string,double>::iterator icnt = mCntrs.begin(); icnt != mCntrs.end(); icnt++) {
	    if(n_id)	n_id->childAdd("el")->setText(icnt->first);
	    if(n_vl)	n_vl->childAdd("el")->setText(r2s(icnt->second));
	}
    }
    else if(a_path == "/debug/dbgCats") {
	if(ctrChkNode(opt,"get",RWRWR_,"root","root",SEC_RD)) {
	    XMLNode *n_cat = ctrMkNode("list",opt,-1,"/debug/dbgCats/cat","",RWRWR_,"root","root");
	    XMLNode *n_prc = ctrMkNode("list",opt,-1,"/debug/dbgCats/prc","",RWRWR_,"root","root");

	    MtxAlloc res(Mess->mRes, true);
	    for(map<string,bool>::iterator idc = Mess->debugCats.begin(); idc != Mess->debugCats.end(); idc++) {
		if(n_cat) n_cat->childAdd("el")->setText(idc->first);
		if(n_prc) n_prc->childAdd("el")->setText(r2s(idc->second));
	    }
	}
	if(ctrChkNode(opt,"set",RWRWR_,"root","root",SEC_WR)) {
	    MtxAlloc res(Mess->mRes, true);
	    //Check for set category
	    if((Mess->debugCats[opt->attr("key_cat")]=s2i(opt->text()))) {
		for(vector<string>::iterator iDC = Mess->selectDebugCats.begin(); iDC != Mess->selectDebugCats.end(); ) {
		    if(*iDC != opt->attr("key_cat")) {
			if(iDC->compare(0,opt->attr("key_cat").size(),opt->attr("key_cat")) == 0) {
			    Mess->debugCats[*iDC] = false;
			    iDC = Mess->selectDebugCats.erase(iDC);
			    continue;
			}
			else if(opt->attr("key_cat").compare(0,iDC->size(),*iDC) == 0) {
			    Mess->debugCats[opt->attr("key_cat")] = false;
			    break;
			}
		    }
		    ++iDC;
		}
		if(Mess->debugCats[opt->attr("key_cat")]) { Mess->selectDebugCats.push_back(opt->attr("key_cat")); modif(); }
	    }
	    //Check for clear category
	    else for(vector<string>::iterator iDC = Mess->selectDebugCats.begin(); iDC != Mess->selectDebugCats.end(); iDC++)
		if(*iDC == opt->attr("key_cat")) { Mess->selectDebugCats.erase(iDC); modif(); break; }
	}
    }
    else if(a_path == "/debug/mess" && ctrChkNode(opt,"get"))	opt->setText(archive().at().nodePath(0,true));
    else TCntrNode::cntrCmdProc(opt);
}
