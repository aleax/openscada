
//OpenSCADA file: tsys.cpp
/***************************************************************************
 *   Copyright (C) 2003-2022 by Roman Savochenko, <roman@oscada.org>       *
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
#include <zlib.h>

#include <algorithm>

#include "ieee754.h"
#include "terror.h"
#include "tmess.h"
#include "tsys.h"

#ifdef HAVE_SYSCALL_H
# include <syscall.h>
#endif

using namespace OSCADA;

//Default values of the global limits
uint8_t	OSCADA::limObjID_SZ = 20;
uint8_t	OSCADA::limObjNm_SZ = 100;
uint8_t	OSCADA::limArchID_SZ = 50;
int	OSCADA::limUserFile_SZ = 10048576;
int	OSCADA::limUserIts_N = 1000000;
unsigned OSCADA::limCacheIts_N = 100;
unsigned OSCADA::limCacheIts_TM = 60;

int	OSCADA::prmStrBuf_SZ = 10000;
float	OSCADA::prmWait_DL = 0.1;
uint8_t	OSCADA::prmWait_TM = 5;
uint8_t	OSCADA::prmInterf_TM = 7;
uint8_t	OSCADA::prmServTask_PER = 10;

//Continuously access variable
TMess	*OSCADA::Mess = NULL;
TSYS	*OSCADA::SYS = NULL;
pthread_key_t TSYS::sTaskKey;

TSYS::TSYS( int argi, char ** argb, char **env ) : argc(argi), argv((const char **)argb), envp((const char **)env),
    isLoaded(false), mRunning(false), isServPrc(false), mFinalKill(false),
    mUser("root"), mConfFile(sysconfdir_full "/oscada.xml"), mId("InitSt"),
    mModDir(oscd_moddir_full), mIcoDir("icons;" oscd_datadir_full "/icons"), mDocDir("docs;" oscd_datadir_full "/docs"),
    mWorkDB(dataRes()), mSelDB(dataRes()), mMainCPUs(dataRes()), mTaskInvPhs(10), mSaveAtExit(false), mSavePeriod(0),
    mModifCalc(false), rootModifCnt(0), sysModifFlgs(0), mStopSignal(0), mN_CPU(1),
    mainPthr(0), mSysTm(0), mClockRT(false), mPrjCustMode(true), mPrjNm(dataRes()), mCfgCtx(NULL), mCfgCtxLast(NULL),
    mRdStLevel(0), mRdRestConnTm(10), mRdTaskPer(1), mRdPrimCmdTr(false)
{
    srand(TSYS::curTime()%1000000);
    mWorkDB = DB_CFG;

    Mess = new TMess();

    mName = _("Initial Station");

    mFinalKill = false;
    SYS = this;		//Init global access value
    mSubst = grpAdd("sub_", true);
    nodeEn();
    mainPthr = pthread_self();
    pthread_key_create(&sTaskKey, NULL);

    if(getenv("USER")) mUser = getenv("USER");

    //Init system clock
    clkCalc();

#if !defined(__ANDROID__) && __GLIBC_PREREQ(2,4)
    //Multi CPU allow check
    cpu_set_t cpuset;
    CPU_ZERO(&cpuset);
    pthread_getaffinity_np(mainPthr, sizeof(cpu_set_t), &cpuset);
# if __GLIBC_PREREQ(2,6)
    mN_CPU = CPU_COUNT(&cpuset);
# endif
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

    //Load commandline options
    string argCom, argVl;
    for(int argPos = 0; (argCom=getCmdOpt(argPos,&argVl)).size(); )
	mCmdOpts[strEncode(argCom,ToLower)] = argVl;
    mCmdOpts[""] = argv[0];

    //Early starting the service task
    taskCreate("SYS_Service", 0, TSYS::ServTask, NULL, 10);
}

TSYS::~TSYS( )
{
    //Stop for ordinal service task
    taskDestroy("SYS_Service");

    int mLev = Mess->messLevel();
    mFinalKill = true;

    mainThr.free();

    //Delete all nodes in the order
    if(present("BD")) {
	del("ModSched");
	del("UI");
	del("Special");
	del("Archive");
	del("DAQ");
	del("Protocol");
	del("Transport");
	del("Security");
	del("BD");
    }

    if(prjNm().size() && prjLockUpdPer()) prjLock("free");

    if(mLev == TMess::Debug) {
	string cntrsStr;
	dataRes().lock();
	for(map<string,double>::iterator icnt = mCntrs.begin(); icnt != mCntrs.end(); icnt++)
	    cntrsStr += TSYS::strMess("%s: %g\n",icnt->first.c_str(),icnt->second);
	dataRes().unlock();
	printf("Program counters at the exit time: %s", cntrsStr.c_str());
    }

    delete Mess;
    pthread_key_delete(sTaskKey);

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

    //Common locks cleaning
    for(map<string, ResMtx*>::iterator iCL = mCommonLocks.begin(); iCL != mCommonLocks.end(); ++iCL)
	delete iCL->second;
}

string TSYS::host( )
{
    utsname ubuf; uname(&ubuf);
    return ubuf.nodename;
}

string TSYS::workDir( )
{
    char buf[prmStrBuf_SZ];
    return getcwd(buf, sizeof(buf));
}

void TSYS::setWorkDir( const string &wdir, bool init )
{
    if(wdir.empty() || workDir() == wdir) return;
    if(chdir(wdir.c_str()) != 0)
	mess_sys(TMess::Warning, _("Error changing working directory in '%s': %s. Perhaps the current directory is already correctly set to %s'."),
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
    string sEl, ndNm;

    XMLNode *tNode = &rootN;
    if(tNode->name() != "OpenSCADA") {
	if(!create) return NULL;
	tNode->setName("OpenSCADA");
    }

    for(int lOff = 0, nLev = 0; true; nLev++) {
	sEl = TSYS::pathLev(path, 0, true, &lOff);
	if(sEl.empty()) return tNode;
	bool ok = false;
	for(unsigned iF = 0; !ok && iF < tNode->childSize(); iF++)
	    if(strcasecmp(tNode->childGet(iF)->attr("id").c_str(),sEl.c_str()) == 0) {
		tNode = tNode->childGet(iF);
		ok = true;
	    }
	if(!ok) {
	    if(!create)	return NULL;
	    ndNm = "prm";
	    switch(nLev) {
		case 0: ndNm = "station";	break;
		case 1: if(sEl.compare(0,4,"sub_") == 0) ndNm = "node";	break;
		case 2: if(sEl.compare(0,4,"mod_") == 0) ndNm = "node";	break;
	    }
	    if(ndNm == "prm") tNode = tNode->childIns(0,ndNm)->setAttr("id", sEl);
	    else tNode = tNode->childAdd(ndNm)->setAttr("id", sEl);
        }
    }

    return tNode;
}

void TSYS::modifCfg( bool chkPossibleWR )
{
    if(chkPossibleWR) {
	//Check config file for readonly
	if(access(mConfFile.c_str(),F_OK|W_OK) != 0)
	    throw err_sys(_("Read-only access to file '%s'."), mConfFile.c_str());
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

string TSYS::atime2str( time_t itm, const string &format, bool gmt )
{
    struct tm tm_tm;
    if(gmt) gmtime_r(&itm, &tm_tm);
    else localtime_r(&itm, &tm_tm);
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

/*double TSYS::str2real( const string &val )
{
    const char *chChr = val.c_str();

    //Pass spaces before
    for( ; true; ++chChr) {
	switch(*chChr) {
	    case ' ': case '\t': continue;
	}
	break;
    }

    //Check and process the base
    bool isNeg = false, isExpNeg = false;
    double tVl = 0;
    int16_t nAftRdx = 0, tAftRdx = 0;
    if(*chChr && ((*chChr >= '0' && *chChr <= '9') || *chChr == '-' || *chChr == '+')) {
	if(*chChr == '+')	++chChr;
	else if(*chChr == '-')	{ isNeg = true; ++chChr; }
	for(bool notFirst = false; *chChr >= '0' && *chChr <= '9'; ++chChr, notFirst = true) {
	    if(notFirst) tVl *= 10;
	    tVl += *chChr - '0';
	}
    }
    if(*chChr == '.' || *chChr == ',') {
	for(++chChr; *chChr >= '0' && *chChr <= '9'; ++chChr, ++nAftRdx)
	    tVl = tVl*10 + (*chChr - '0');
    }
    if(isNeg) tVl *= -1;

    //Read exponent
    if(*chChr && (*chChr == 'e' || *chChr == 'E')) {
	++chChr;
	if(*chChr == '+')	++chChr;
	else if(*chChr == '-')	{ isExpNeg = true; ++chChr; }
	for(bool notFirst = false; *chChr >= '0' && *chChr <= '9'; ++chChr, notFirst = true) {
	    if(notFirst) tAftRdx *= 10;
	    tAftRdx += *chChr - '0';
	}
	if(isExpNeg) tAftRdx *= -1;
    }

    //Combine
    return tVl * pow(10, tAftRdx-nAftRdx);
}*/

time_t TSYS::str2atime( const string &val, const string &format, bool gmt )
{
    struct tm stm;
    stm.tm_isdst = -1;
    if(!strptime(val.c_str(),(format.empty()?"%d-%m-%Y %H:%M:%S":format.c_str()),&stm))
	return 0;

    return gmt ? timegm(&stm) : mktime(&stm);
}

string TSYS::addr2str( void *addr )
{
    char buf[sizeof(void*)*2+3];
    snprintf(buf,sizeof(buf),"%p",addr);

    return buf;
}

void *TSYS::str2addr( const string &str )	{ return (void *)strtoull(str.c_str(),NULL,16); }

string TSYS::strTrim( const string &val, const string &cfg )
{
    int beg = -1, end = -1;

    for(unsigned iS = 0, iC = 0; iS < val.size(); iS++) {
	for(iC = 0; iC < cfg.size() && val[iS] != cfg[iC]; iC++) ;
	if(iC < cfg.size())	continue;
	if(beg < 0) beg = iS;
	end = iS;
    }

    return (beg >= 0) ? val.substr(beg, end-beg+1) : "";
}

string TSYS::strMess( const char *fmt, ... )
{
    char str[prmStrBuf_SZ];
    va_list argptr;

    va_start(argptr, fmt);
    vsnprintf(str, sizeof(str), fmt, argptr);
    va_end(argptr);

    return str;
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
    string rez = TSYS::strMess(_(
	"***************************************************************************\n"
	"********** %s v%s (%s-%s). *********\n"
	"***************************************************************************\n\n"
	"===========================================================================\n"
	"==================== Generic options ======================================\n"
	"===========================================================================\n"
	"-h, --help		This help text about the program command-line options and configuration file parameters.\n"
	"    --projName=<name>	OpenSCADA project name to switch it.\n"
	"			For this feature there also uses an environment variable \"OSCADA_ProjName\" and the program binary name \"openscada_{ProjName}\".\n"
	"    --projUserDir={dir} Directory of user projects (writeable) of OpenSCADA, \"~/.openscada\" by default.\n"
	"    --projLock={per}	Uses the projects locking by creation the \"lock\" file into the project folder and update it in the period <per>,\n"
	"			by default it is enabled and the updating period <per> is 60 seconds. To disable set the updating period <per> to zero.\n"
	"    --lang=<LANG>	Station language, in the view \"en_US.UTF-8\".\n"
	"    --config=<file>	Station configuration file.\n"
	"    --station=<id>	Station identifier.\n"
	"    --statName=<name>	Station name.\n"
	"    --modDir=<path>	Directories with the modules, separated by ';', they can include a files' template at the end.\n"
	"    --messLev=<level>	Level of the processing messages (0-7).\n"
	"    --log=<direct>	Direct messages to, by the bitfield:\n"
	"			  0x1 - syslogd;\n"
	"			  0x2 - stdout;\n"
	"			  0x4 - stderr;\n"
	"			  0x8 - the messages archive.\n"
	"    --consoleCharSet={CharSet} Forcing to the console <CharSet> for the console messages, by default it is system one.\n"
	"    --demon, --daemon	Run in the daemon mode.\n"
	"    --pidFile=<file>	File for the program process ID placing here.\n"
	"    --noCoreDump	Prevents from the core dump creation at crashes - don't set the limit to the unlimited value.\n"
	"    --permCrtFiles={perm} Permissions of the created OpenSCADA files, by default it is 0755 (RWXRW_RW_).\n"
	"----------- Station '%s(%s)' settings in the configuration file -----------\n"
	"StName     <nm>	Station name.\n"
	"WorkDB     <Type.Name> Working DB (<type> and <name>).\n"
	"WorkDir    <path>	Working directory.\n"
	"ModDir     <path>	Directories with the modules, separated by ';', they can include a files' template at the end.\n"
	"IcoDir     <path>	Icons directory.\n"
	"DocDir     <path>	Documents directory.\n"
	"MessLev    <level>	Level of the processing messages (0-7).\n"
	"SelDebCats <list>	Debug categories list, separated by ';'.\n"
	"LogTarget  <direction> Direct messages to, by the bitfield:\n"
	"			  0x1 - syslogd;\n"
	"			  0x2 - stdout;\n"
	"			  0x4 - stderr;\n"
	"			  0x8 - the messages archive.\n"
	"Lang       <lang>	Station language, in the view \"uk_UA.UTF-8\".\n"
	"Lang2CodeBase <lang>	Base language for variable texts translation, in the two symbols code.\n"
	"MainCPUs   <list>	Main list of the using CPUs, separated by ':'.\n"
	"TaskInvPhs <n>		Number of phases of the task invoking, 1 to disable the phasing.\n"
	"ClockRT    <0|1>	Sets the clock source to use to REALTIME (otherwise MONOTONIC), which is problematic one at the system clock modification.\n"
	"SaveAtExit <0|1>	Save the program at exit.\n"
	"SavePeriod <seconds>	Period of the program saving, 0 to disable.\n"
	"ModifCalc  <0|1>	Set modification for the calculated objects.\n"
	"RdStLevel  <lev>	Level of the redundancy of the current station.\n"
	"RdTaskPer  <seconds>	Call period of the redundancy task.\n"
	"RdRestConnTm <seconds>	Time to restore connection to \"dead\" reserve station.\n"
	"RdStList   <list>	Redundant stations list, separated symbol ';' (st1;st2).\n"
	"RdPrimCmdTr <0|1>	Enables the transmission of primary commands to the reserve stations.\n"
	"    Global configurable limits:\n"
	"limObjID_SZ	[*20..50] ID size of the OpenSCADA objects.\n"
	"		WARNING! Big size can cause the key limit error on MySQL like DB!\n"
	"			 Change that once before use on DBs with the fixed type \"char({N})\"!\n"
	"limObjNm_SZ	[*100...200] NAME size of the OpenSCADA objects.\n"
	"		WARNING! Change that once before use on DBs with the fixed type \"char({N})\"!\n"
	"limArchID_SZ	[*50...90] ID size of the value archive objects.\n"
	"		WARNING! Increase it only, else you can get problems on Archive.FSArch!\n"
	"			 Change that once before use on DBs with the fixed type \"char({N})\"!\n"
	"limUserFile_SZ	[1MB...*10MB...1000MB] The files size limit at loading and processing in the userspace\n"
	"		and the part size of the big files transferring.\n"
	"limUserIts_N	[1000...*1000000...1000000000] The limit on count of creating user items, like to array items.\n"
	"limCacheIts_N	[*100...100000] The limit on count of the caching items.\n"
	"limCacheIts_TM	[10...*60...1000] The limit on the caching items time, seconds.\n"
	"    Global configurable parameters:\n"
	"prmStrBuf_SZ	[1000...*10000...1000000] Length of string buffers, no string class.\n"
	"prmWait_DL	[0.001...*0.1...1] Quantum of the waiting time cycles, seconds.\n"
	"prmWait_TM	[*5...10] Standard waiting timeout length, seconds.\n"
	"prmInterf_TM	[*7...15] Time of waiting for the interface reaction, seconds.\n"
	"prmServTask_PER [1...*10...120] Service task period, seconds.\n\n"),
	PACKAGE_NAME, VERSION, buf.sysname, buf.release, name().c_str(), id().c_str());

    vector<string> ls;
    list(ls);
    for(unsigned iM = 0; iM < ls.size(); iM++)
	rez += at(ls[iM]).at().optDescr();

    return rez;
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
		return rez.substr(0, fPos);
	    }
	    if(argVal) *argVal = ((argI+1) < argc && argv[argI+1][0] != '-') ? argv[argI+1] : "";
	    return rez;
	}
	//Check for short: "-v", "-v val", "-abcv" or "-abcv val"
	else {
	    if((argIsh+1) >= argLen) continue;
	    curPos = argI+((argIsh+1)<<8);
	    if(argVal) *argVal = ((argIsh+2) == argLen && (argI+1) < argc && argv[argI+1][0] != '-') ? argv[argI+1] : "";
	    return string(argv[argI]+argIsh+1, 1);
	}
    }

    return "";
}

bool TSYS::cmdOptPresent( const string &opt )
{
    MtxAlloc res(dataRes(), true);
    return mCmdOpts.find(strEncode(opt,ToLower)) != mCmdOpts.end();
}

string TSYS::cmdOpt( const string &opt, const string &setVl )
{
    MtxAlloc res(dataRes(), true);

    //Set command option
    if(setVl.size()) mCmdOpts[strEncode(opt,ToLower)] = setVl;

    //Get value
    map<string, string>::iterator iOpt = mCmdOpts.find(strEncode(opt,ToLower));
    if(iOpt == mCmdOpts.end()) return "";
    return iOpt->second.size() ? iOpt->second : "";	// "1";
}

int TSYS::permCrtFiles( bool exec )
{
    int rez = 0755;

    if(cmdOptPresent("permCrtFiles"))
	rez = strtol(cmdOpt("permCrtFiles").c_str(),NULL,0);

    return rez & (exec?0777:0666);
}

ResMtx *TSYS::commonLock( const string &nm )
{
    MtxAlloc res(dataRes(), true);

    if(mCommonLocks.find(nm) == mCommonLocks.end())
	mCommonLocks[nm] = new ResMtx(true);

    return mCommonLocks[nm];
}

void TSYS::cfgFileLoad( )
{
    //================ Load parameters from the commandline =========================
    string tVl;
    if((tVl=cmdOpt("config")).size())	mConfFile = tVl;
    if((tVl=cmdOpt("station")).size())	mId = tVl;
    if((tVl=cmdOpt("statName")).size())	mName = tVl;
    if((tVl=cmdOpt("modDir")).size())	setModDir(tVl, true);

    //Save changes before
    cfgFileSave();

    //Load config-file
    int hd = open(mConfFile.c_str(), O_RDONLY);
    if(hd < 0) mess_sys(TMess::Error, _("Error the configuration file '%s': %s"), mConfFile.c_str(), strerror(errno));
    else {
	bool fOK = true;
	string s_buf;
	int cf_sz = lseek(hd, 0, SEEK_END);
	if(cf_sz > 0) {
	    lseek(hd, 0, SEEK_SET);
	    char buf[prmStrBuf_SZ];
	    for(int len = 0; (len=read(hd,buf,sizeof(buf))) > 0; ) s_buf.append(buf, len);
	    fOK = s_buf.size();
	}
	close(hd);
	if(!fOK) mess_sys(TMess::Error, _("Error loading the configuration file '%s'."), mConfFile.c_str());

	try {
	    ResAlloc res(cfgRes(), true);
	    rootN.load(s_buf, XMLNode::LD_Full);
	    if(rootN.name() == "OpenSCADA") {
		XMLNode *stat_n = NULL;
		for(int iSt = rootN.childSize()-1; iSt >= 0; iSt--)
		    if(rootN.childGet(iSt)->name() == "station") {
			stat_n = rootN.childGet(iSt);
			if(stat_n->attr("id") == mId) break;
		    }
		if(stat_n && stat_n->attr("id") != mId) {
		    if(mId != "InitSt")
			mess_sys(TMess::Warning, _("Station '%s' is not in the configuration file. The configuration of the station '%s' has been used!"),
			    mId.c_str(), stat_n->attr("id").c_str());
		    mId	= stat_n->attr("id");
		}
		if(!stat_n)	rootN.clear();
	    }
	    else rootN.clear();
	    if(!rootN.childSize()) mess_sys(TMess::Error, _("Error configuration '%s'!"), mConfFile.c_str());
	    rootModifCnt = 0;
	} catch(TError &err) { mess_sys(TMess::Error, _("Error loading the configuration file '%s'."), err.mess.c_str()); }
    }
}

void TSYS::cfgFileSave( )
{
    ResAlloc res(cfgRes(), true);
    if(!rootModifCnt) return;
    int hd = open(mConfFile.c_str(), O_CREAT|O_TRUNC|O_WRONLY, permCrtFiles());
    if(hd < 0) mess_sys(TMess::Error, _("Error the configuration file '%s': %s"), mConfFile.c_str(), strerror(errno));
    else {
	string rezFile = rootN.save(XMLNode::XMLHeader);
	int rez = write(hd, rezFile.data(), rezFile.size());
	if(rez != (int)rezFile.size()) mess_sys(TMess::Error,_("Error writing the configuration file '%s': %s"), mConfFile.c_str(), ((rez<0)?strerror(errno):""));
	rootModifCnt = 0;
	rootFlTm = time(NULL);
	close(hd);
    }
}

void TSYS::cfgPrmLoad( )
{
    //Global limits
    limObjID_SZ = vmax(20, vmin(50,s2i(TBDS::genPrmGet(nodePath()+"limObjID_SZ",i2s(limObjID_SZ)))));
    limObjNm_SZ = vmax(100, vmin(200,s2i(TBDS::genPrmGet(nodePath()+"limObjNm_SZ",i2s(limObjNm_SZ)))));
    limArchID_SZ = vmax(50, vmin(90,s2i(TBDS::genPrmGet(nodePath()+"limArchID_SZ",i2s(limArchID_SZ)))));
    limUserFile_SZ = vmax(1048576, vmin(100048576,s2i(TBDS::genPrmGet(nodePath()+"limUserFile_SZ",i2s(limUserFile_SZ)))));
    limUserIts_N = vmax(1000, vmin(1000000000,s2i(TBDS::genPrmGet(nodePath()+"limUserIts_N",i2s(limUserIts_N)))));
    limCacheIts_N = vmax(100, vmin(100000,s2i(TBDS::genPrmGet(nodePath()+"limCacheIts_N",i2s(limCacheIts_N)))));
    limCacheIts_TM = vmax(10, vmin(1000,s2i(TBDS::genPrmGet(nodePath()+"limCacheIts_TM",i2s(limCacheIts_TM)))));

    //Global parameters
    prmStrBuf_SZ = vmax(1000, vmin(1000000,s2i(TBDS::genPrmGet(nodePath()+"prmStrBuf_SZ",i2s(prmStrBuf_SZ)))));
    prmWait_DL = vmax(0.001, vmin(1,s2r(TBDS::genPrmGet(nodePath()+"prmWait_DL",r2s(prmWait_DL)))));
    prmWait_TM = vmax(5, vmin(10,s2i(TBDS::genPrmGet(nodePath()+"prmWait_TM",i2s(prmWait_TM)))));
    prmInterf_TM = vmax(7, vmin(15,s2i(TBDS::genPrmGet(nodePath()+"prmInterf_TM",i2s(prmInterf_TM)))));
    prmServTask_PER = vmax(1, vmin(120,s2i(TBDS::genPrmGet(nodePath()+"prmServTask_PER",i2s(prmServTask_PER)))));

    //System parameters
    setClockRT(s2i(TBDS::genPrmGet(nodePath()+"ClockRT",i2s(clockRT()))));
    mName = TBDS::genPrmGet(nodePath()+"StName", name(), "root", TBDS::UseTranslation);
    mWorkDB = TBDS::genPrmGet(nodePath()+"WorkDB",workDB());
    setWorkDir(TBDS::genPrmGet(nodePath()+"Workdir","").c_str(), true);
    setModDir(TBDS::genPrmGet(nodePath()+"ModDir",modDir()), true);
    setIcoDir(TBDS::genPrmGet(nodePath()+"IcoDir",icoDir()), true);
    setDocDir(TBDS::genPrmGet(nodePath()+"DocDir",docDir()), true);
    setMainCPUs(TBDS::genPrmGet(nodePath()+"MainCPUs",mainCPUs()));
    setTaskInvPhs(s2i(TBDS::genPrmGet(nodePath()+"TaskInvPhs",i2s(taskInvPhs()))));
    setSaveAtExit(s2i(TBDS::genPrmGet(nodePath()+"SaveAtExit",i2s(saveAtExit()))));
    setSavePeriod(s2i(TBDS::genPrmGet(nodePath()+"SavePeriod",i2s(savePeriod()))));
    setModifCalc(s2i(TBDS::genPrmGet(nodePath()+"ModifCalc",i2s(modifCalc()))));

    //Redundancy parameters
    setRdStLevel(s2i(TBDS::genPrmGet(nodePath()+"RdStLevel",i2s(rdStLevel()))));
    setRdTaskPer(s2r(TBDS::genPrmGet(nodePath()+"RdTaskPer",r2s(rdTaskPer()))));
    setRdRestConnTm(s2i(TBDS::genPrmGet(nodePath()+"RdRestConnTm",i2s(rdRestConnTm()))));
    setRdPrimCmdTr(s2i(TBDS::genPrmGet(nodePath()+"RdPrimCmdTr",i2s(rdPrimCmdTr()))));
    string stLs = TBDS::genPrmGet(nodePath()+"RdStList"), stId;
    mRdRes.lock(true);
    for(int off = 0; (stId=TSYS::strSepParse(stLs,0,';',&off)).size(); )
	if(mSt.find(stId) == mSt.end()) mSt[stId] = SStat();
    mRdRes.unlock();
}

void TSYS::load_( )
{
    //Check for a OpenSCADA project selection and switch at need
    // Get current project name
    setPrjNm(cmdOpt("statName"));
    if(!(prjNm().size() && TRegExp("/"+prjNm()+"$").test(workDir()) &&
	    TRegExp("/("+prjNm()+"/oscada.xml|oscada_"+prjNm()+".xml)$").test(cmdOpt("config"))))
	setPrjNm("");
    // Get name for the command of project switch
    if(!prjNm().size()) {
	setPrjNm(cmdOpt("projName"));
	if(!prjNm().size()) {
	    TArrayObj *rez = TRegExp("openscada_(.+)$").match(cmdOpt(""));
	    if(rez) {
		if(rez->size() >= 2) setPrjNm(rez->arGet(1).getS());
		delete rez;
	    }
	}
	if(!prjNm().size() && getenv("OSCADA_ProjName")) setPrjNm(getenv("OSCADA_ProjName"));
	if(prjNm().size() && !prjSwitch(prjNm())) setPrjNm("");
    }

    mStopSignal = 0;

    //Check for the custom (not project) mode
    setPrjCustMode(!prjNm().size() && cmdOpt("config").size());
    //Check for the project lock
    if(prjNm().size() && prjLockUpdPer() && !prjLock("hold")) {
	mess_sys(TMess::Warning, _("Impossible to hold the project lock! Seems the project '%s' already running."), prjNm().c_str());
	setPrjNm("");
	stop();
	return;
    }

    bool cmd_help = (SYS->cmdOptPresent("h") || SYS->cmdOptPresent("help"));
    if(cmd_help) Mess->setMessLevel(TMess::Emerg);
    cfgFileLoad();
    mess_sys(TMess::Info, _("Loading."));
    cfgPrmLoad();
    if(!cmd_help) Mess->load();	//Messages load

    //Create subsystems
    if(!present("BD")) {
	add(new TBDS());
	add(new TSecurity());
	add(new TTransportS());
	add(new TProtocolS());
	add(new TDAQS());
	add(new TArchiveS());
	add(new TSpecialS());
	add(new TUIS());
	add(new TModSchedul());
    }

    if(!isLoaded) {
	//Load modules
	modSchedul().at().load();
	if(!modSchedul().at().loadLibS()) {
	    mess_sys(TMess::Error, _("No module has been loaded, your configuration is likely to be broken!"));
	    stop();
	}

	//First DB subsystem load
	db().at().load();
	if(!cmd_help) {
	    modSchedul().at().modifG();	// For try reload from the DB

	    //Second load for load from the generic DB
	    Mess->load();
	}
	cfgPrmLoad();

	isLoaded = true;
    }

    //Force the main threaded module START update
    if(!mainThr.freeStat()) mainThr.at().perSYSCall(1);

    //Direct loading of subsystems and modules
    vector<string> lst;
    list(lst);
    for(unsigned iA = 0; iA < lst.size(); iA++)
	try { at(lst[iA]).at().load(); }
	catch(TError &err) {
	    mess_err(err.cat.c_str(), "%s", err.mess.c_str());
	    mess_sys(TMess::Error, _("Error loading the subsystem '%s'."), lst[iA].c_str());
	}

    if(cmd_help) {
	string cmdHelp = optDescr();
	if(cmdOpt("consoleCharSet").size()) cmdHelp = Mess->codeConvOut(cmdOpt("consoleCharSet"), cmdHelp);
	fprintf(stdout, "%s", cmdHelp.c_str());

	stop();
    }
}

void TSYS::save_( )
{
    mess_sys(TMess::Info, _("Saving."));

    //System parameters
    TBDS::genPrmSet(nodePath()+"StName", mName, "root", TBDS::UseTranslation);
    TBDS::genPrmSet(nodePath()+"WorkDB", workDB(), "root", TBDS::OnlyCfg);
    if(sysModifFlgs&MDF_WorkDir)TBDS::genPrmSet(nodePath()+"Workdir", workDir(), "root", TBDS::OnlyCfg);
    if(sysModifFlgs&MDF_ModDir)	TBDS::genPrmSet(nodePath()+"ModDir", modDir(), "root", TBDS::OnlyCfg);
    if(sysModifFlgs&MDF_IcoDir)	TBDS::genPrmSet(nodePath()+"IcoDir", icoDir(), "root", TBDS::OnlyCfg);
    if(sysModifFlgs&MDF_DocDir)	TBDS::genPrmSet(nodePath()+"DocDir", docDir(), "root", TBDS::OnlyCfg);
    TBDS::genPrmSet(nodePath()+"MainCPUs", mainCPUs());
    TBDS::genPrmSet(nodePath()+"ClockRT", i2s(clockRT()));
    TBDS::genPrmSet(nodePath()+"TaskInvPhs", i2s(taskInvPhs()));
    TBDS::genPrmSet(nodePath()+"SaveAtExit", i2s(saveAtExit()));
    TBDS::genPrmSet(nodePath()+"SavePeriod", i2s(savePeriod()));
    TBDS::genPrmSet(nodePath()+"ModifCalc", i2s(modifCalc()));

    //Redundancy parameters
    TBDS::genPrmSet(nodePath()+"RdStLevel", i2s(rdStLevel()), "root", TBDS::OnlyCfg);
    TBDS::genPrmSet(nodePath()+"RdTaskPer", r2s(rdTaskPer()), "root", TBDS::OnlyCfg);
    TBDS::genPrmSet(nodePath()+"RdRestConnTm", i2s(rdRestConnTm()), "root", TBDS::OnlyCfg);
    TBDS::genPrmSet(nodePath()+"RdPrimCmdTr", i2s(rdPrimCmdTr()), "root", TBDS::OnlyCfg);
    mRdRes.lock(false);
    string stLs;
    for(map<string,TSYS::SStat>::iterator sit = mSt.begin(); sit != mSt.end(); sit++)
	stLs += sit->first+";";
    mRdRes.unlock();
    TBDS::genPrmSet(nodePath()+"RdStList", stLs, "root", TBDS::OnlyCfg);

    Mess->save();	//Messages load
}

int TSYS::start( )
{
    //Start for high priority service and redundancy tasks
    taskCreate("SYS_HighPriority", 120, TSYS::HPrTask, NULL);
    taskCreate("SYS_Redundancy", 5, TSYS::RdTask, NULL);

    //Start for subsystems
    vector<string> lst;
    list(lst);

    mess_sys(TMess::Info, _("Starting."));

    for(unsigned iA = 0; iA < lst.size(); iA++)
	try { at(lst[iA]).at().subStart(); }
	catch(TError &err) {
	    mess_err(err.cat.c_str(), "%s", err.mess.c_str());
	    mess_sys(TMess::Error, _("Error starting the subsystem '%s'."), lst[iA].c_str());
	}

    cfgFileScan(true);

    //Register user API translations into config
    Mess->translReg("", "uapi:" DB_CFG);

    mStopSignal = 0;
    mess_sys(TMess::Info, _("Starting is completed!"));

    //Call in monopoly for main thread module or wait for a signal.
    mRunning = true;
    if(!mainThr.freeStat()) mainThr.at().modStart();
    while(!mStopSignal) sysSleep(prmWait_DL);
    mRunning = false;
    TSYS::eventWait(isServPrc, false, string("SYS_Service: ")+_("waiting the processing finish ..."));

    mess_sys(TMess::Info, _("Stopping."));

    if(saveAtExit() || savePeriod()) save();
    cfgFileSave();

    //Stop for the subsystems
    for(int iA = lst.size()-1; iA >= 0; iA--)
	try { at(lst[iA]).at().subStop(); }
	catch(TError &err) {
	    mess_err(err.cat.c_str(), "%s", err.mess.c_str());
	    mess_sys(TMess::Error, _("Error stopping the subsystem '%s'."), lst[iA].c_str());
	}

    //Stop for high priority service and redundancy tasks
    taskDestroy("SYS_Redundancy");
    taskDestroy("SYS_HighPriority");

    return mStopSignal;
}

void TSYS::stop( int sig )
{
    if(!mStopSignal) mStopSignal = sig;

    if(SYS->cmdOptPresent("h") || SYS->cmdOptPresent("help")) {
	vector<string> lst;
	list(lst);

	//Stop for the subsystems
	for(int iA = lst.size()-1; iA >= 0; iA--)
	    try { at(lst[iA]).at().subStop(); }
	    catch(TError &err) {
		mess_err(err.cat.c_str(), "%s", err.mess.c_str());
		mess_sys(TMess::Error, _("Error stopping the subsystem '%s'."), lst[iA].c_str());
	    }
    }
}

void TSYS::unload( )
{
    at("ModSched").at().unload();
    at("UI").at().unload();
    at("Special").at().unload();
    at("Archive").at().unload();
    at("DAQ").at().unload();
    at("Protocol").at().unload();
    at("Transport").at().unload();
    at("Security").at().unload();
    at("BD").at().unload();

    //Clear counters
    if(Mess->messLevel() == TMess::Debug) {
	dataRes().lock();
	mCntrs.clear();
	dataRes().unlock();
    }

    if(prjNm().size() && prjLockUpdPer()) prjLock("free");

    Mess->unload();

    mRdRes.lock(true);
    mSt.clear();
    mRdStLevel = 0, mRdRestConnTm = 10, mRdTaskPer = 1, mRdPrimCmdTr = false;
    mRdRes.unlock();

    mId = "InitSt", mName = _("Initial Station"), mUser = "root", mMainCPUs = "";
    mConfFile = sysconfdir_full "/oscada.xml";
    mModDir = oscd_moddir_full;
    mIcoDir = "icons;" oscd_datadir_full "/icons";
    mDocDir = "docs;" oscd_datadir_full "/docs";
    mWorkDB = DB_CFG;
    mSaveAtExit = false, mSavePeriod = 0, isLoaded = false, rootModifCnt = 0, sysModifFlgs = 0, mPrjCustMode = true;

    modifG();
}

bool TSYS::chkSelDB( const string& wDB,  bool isStrong )
{
    return (selDB().empty() && !isStrong) ||
	(selDB().size() && SYS->selDB() == TBDS::realDBName(wDB) &&
	    (wDB == DB_CFG || wDB == "*.*" || ((AutoHD<TBD>)db().at().nodeAt(wDB,0,'.')).at().enableStat()));
}

void TSYS::setMainCPUs( const string &vl )
{
    mMainCPUs = vl;

#if !defined(__ANDROID__) && __GLIBC_PREREQ(2,4)
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

void TSYS::setTaskInvPhs( int vl )
{
    mTaskInvPhs = (vl <= 0) ?
		    vmax(1, mTasks.size()/(10*nCPU())) * 10 :
		    vmax(1, vmin(100,vl));

    modif();
}

void TSYS::sighandler( int signal, siginfo_t *siginfo, void *context )
{
    switch(signal) {
	case SIGINT:
	    SYS->mStopSignal = signal;
	    break;
	case SIGTERM:
	    SYS->mess_sys(TMess::Warning, _("Termination signal is received. Stopping the program!"));
	    SYS->mStopSignal = signal;
	    break;
	case SIGFPE:
	    SYS->mess_sys(TMess::Warning, _("Floating point exception is caught!"));
	    exit(1);
	    break;
	case SIGCHLD: {
	    int status;
	    pid_t pid = wait(&status);
	    if(!WIFEXITED(status) && pid > 0) SYS->mess_sys(TMess::Info, _("Close the process %d of the child!"), pid);
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
	if(!mSysclc && ((fp=fopen("/sys/devices/system/cpu/cpu0/cpufreq/cpuinfo_cur_freq","r")) ||
			(fp=fopen("/sys/devices/system/cpu/cpu0/cpufreq/scaling_cur_freq","r")))) {
	    size_t rez = fread(buf, 1, sizeof(buf)-1, fp); buf[rez] = 0;
	    mSysclc = uint64_t(s2r(buf)*1e3);
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
	MtxAlloc res(SYS->cfgLoadSaveM(), true);
	modifG();
	setSelDB(DB_CFG);
	load();
	setSelDB("");
    }
}

int64_t TSYS::curTime( clockid_t clc )
{
    struct timespec tm;
    clock_gettime(clc, &tm);
    return 1000000ll*tm.tv_sec + tm.tv_nsec/1000;

    /*timeval cur_tm;
    gettimeofday(&cur_tm, NULL);
    return (int64_t)cur_tm.tv_sec*1000000 + cur_tm.tv_usec;*/
}

uint64_t TSYS::curTimeN( clockid_t clc )
{
    struct timespec tm;
    clock_gettime(clc, &tm);
    return 1000000000ll*tm.tv_sec + tm.tv_nsec;
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
	    SYS->mess_sys(TMess::Crit, _("Waiting for event ..."));
	}
	sysSleep(prmWait_DL);
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
    int an_dir = vmax(0, off ? *off : 0);
    if(an_dir >= (int)path.size() || sep.empty()) return "";

    int t_lev = 0;
    string rezTk;
    bool isFound = false;
    while(true) {
	size_t t_dir = path.find(sep, an_dir);
	if(t_dir == string::npos) {
	    if(off) *off = path.size();
	    return (t_lev == level) ? path.substr(an_dir) : "";
	}
	else if(t_lev == level) {
	    rezTk = path.substr(an_dir, t_dir-an_dir);
	    isFound = true;
	}
	an_dir = t_dir + sep.size();

	if(mergeSepSymb && sep.size() == 1)
	    while(an_dir < (int)path.size() && path[an_dir] == sep[0]) an_dir++;
	if(isFound) {
	    if(off) *off = an_dir;
	    break;
	}
	t_lev++;
    }

    return rezTk;
}

string TSYS::strParseEnd( const string &path, int level, const string &sep, int *off, bool mergeSepSymb )
{
    int an_dir = vmin((int)path.size(), off ? *off : (int)path.size()-1);
    if(an_dir < 0 || sep.empty() || path.empty()) return "";

    int t_lev = 0;
    string rezTk;
    bool isFound = false;
    while(true) {
	size_t t_dir = path.rfind(sep, an_dir);
	if(t_dir == string::npos) {
	    if(off) *off = -1;
	    return (t_lev == level) ? path.substr(0,an_dir+1) : "";
	}
	else if(t_lev == level) {
	    rezTk = path.substr(t_dir+sep.size(), an_dir-(t_dir+sep.size()-1));
	    isFound = true;
	}
	an_dir = t_dir - sep.size();

	if(mergeSepSymb && sep.size() == 1)
	    while(an_dir >= 0 && path[an_dir] == sep[0]) an_dir--;
	if(isFound) {
	    if(off) *off = an_dir;
	    break;
	}
	t_lev++;
    }

    return rezTk;
}

string TSYS::strLine( const string &str, int level, int *off )
{
    int an_dir = vmax(0, off ? *off : 0);
    if(an_dir >= (int)str.size()) return "";

    int t_lev = 0, edLnSmbSz = 1;
    string rezTk;
    bool isFound = false;
    size_t t_dir;
    while(true) {
	for(t_dir = an_dir; t_dir < str.size(); t_dir++)
	    if(str[t_dir] == '\x0D' || str[t_dir] == '\x0A')
	    {
		edLnSmbSz = (str[t_dir] == '\x0D' && ((t_dir+1) < str.size()) && str[t_dir+1] == '\x0A') ? 2 : 1;
		break;
	    }
	if(t_dir >= str.size()) {
	    if(off) *off = str.size();
	    return (t_lev==level) ? str.substr(an_dir) : "";
	}
	else if(t_lev == level) { rezTk = str.substr(an_dir, t_dir-an_dir); isFound = true; }
	an_dir = t_dir + edLnSmbSz;

	if(isFound) {
	    if(off) *off = an_dir;
	    break;
	}
	t_lev++;
    }

    return rezTk;
}

string TSYS::pathLev( const string &path, int level, bool decode, int *offCmtbl, int *off )
{
    int an_dir = vmax(0, off ? *off : (offCmtbl ? *offCmtbl : 0));
    //Begin separators pass
    while(an_dir < (int)path.size() && path[an_dir] == '/') an_dir++;
    if(an_dir >= (int)path.size()) return "";

    //Path level process
    int t_lev = 0;
    string rezTk;
    bool isFound = false;
    while(true) {
	size_t t_dir = path.find("/", an_dir);
	if(t_dir == string::npos) {
	    if(off) *off = path.size();
	    else if(offCmtbl) *offCmtbl = path.size();
	    rezTk = (t_lev == level) ? path.substr(an_dir) : "";
	    break;
	}
	else if(t_lev == level) { rezTk = path.substr(an_dir, t_dir-an_dir); isFound = true; }
	an_dir = t_dir;

	// Pass equal symbols '/'
	while((!isFound || off) && an_dir < (int)path.size() && path[an_dir]=='/') an_dir++;

	if(isFound) {
	    if(off) *off = an_dir;
	    else if(offCmtbl) *offCmtbl = an_dir;
	    break;
	}
	t_lev++;
    }

    return (decode && rezTk.size()) ? TSYS::strDecode(rezTk, TSYS::PathEl) : rezTk;
}

string TSYS::pathLevEnd( const string &path, int level, bool decode, int *off )
{
    int an_dir = vmin((int)path.size(), off ? *off : (int)path.size()-1);
    if(an_dir < 0 || path.empty()) return "";

    int t_lev = 0;
    string rezTk;
    bool isFound = false;
    while(true) {
	size_t t_dir = path.rfind("/", an_dir);
	if(t_dir == string::npos) {
	    if(off) *off = -1;
	    return (t_lev == level) ? (decode ? TSYS::strDecode(path.substr(0,an_dir),TSYS::PathEl) : path.substr(0,an_dir)) : "";
	}
	else if(t_lev == level) {
	    rezTk = decode ? TSYS::strDecode(path.substr(t_dir+1,an_dir-t_dir),TSYS::PathEl) : path.substr(t_dir+1,an_dir-t_dir);
	    isFound = true;
	}
	an_dir = t_dir - 1;

	while(an_dir >= 0 && path[an_dir] == '/') an_dir--;
	if(isFound) {
	    if(off) *off = an_dir;
	    break;
	}
	t_lev++;
    }

    return rezTk;
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
		unsigned iSmb;
		for(iSmb = 0; iSmb < opt1.size(); iSmb++)
		    if(in[iSz] == opt1[iSmb]) {
			snprintf(buf,sizeof(buf),"%%%02X",(unsigned char)in[iSz]);
			sout += buf;
			break;
		    }
		if(iSmb >= opt1.size()) sout += in[iSz];
	    }
	    break;
	}
	case TSYS::base64: {
	    sout.reserve(in.size()+in.size()/4+in.size()/57+10);
	    const char *base64alph = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";
	    for(iSz = 0; iSz < (int)in.size(); iSz += 3) {
		//if(iSz && !(iSz%57))	sout.push_back('\n');
		if(iSz && !(iSz%57) && opt1.size())	sout += opt1;
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
		if(isxdigit(in[iCh])) { sout += (char)strtol(in.substr(iCh,2).c_str(),NULL,16); iCh++; }
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
		} else sout += in[iSz];
	    break;
	case TSYS::ShieldBin: {
	    sout.reserve(in.size());
	    char buf[10];
	    for(iSz = 0; iSz < (int)in.size(); iSz++)
		switch(in[iSz]) {
		    case 0 ... 8:
		    case 0xB ... 0xC:
		    case 0x0E ... 0x1F:
			sprintf(buf, "\\%03o", in[iSz]);
			sout += buf;
			break;
		    default: sout += in[iSz];
		}
	    break;
	}
	case TSYS::ToLower:
	    sout.reserve(in.size());
	    for(iSz = 0; iSz < (int)in.size(); iSz++)
		sout += (char)tolower(in[iSz]);
	    break;
	case TSYS::Limit: {
	    int lVal = s2i(opt1), off = 0;
	    if(!lVal) sout = in;
	    else {
		for(int oL = 0, cL = 0; off < (int)in.size() && oL < lVal; oL++)
		    off += (cL=Mess->getUTF8(in,off)) ? cL : 1;
		sout = in.substr(0, off);
	    }
	    break;
	}
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
		//if(in[iSz] == '\n') iSz += sizeof('\n');
		if(isspace(in[iSz])) { iSz++; continue; }
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
	//Binary decoding to hex bytes string. Option <opt1> uses for:
	//  "<text>" - includes the text part in right
	//  "{sep}" - short separator
	case TSYS::Bin: {
	    sout.reserve(in.size()*2);
	    if(opt1 == "<text>") {
		char buf[3];
		string txt, offForm = TSYS::strMess("%%0%dx: ", vmax(2,(int)ceil(log(in.size())/log(16))));
		for(iSz = 0; iSz < in.size() || (iSz%16); ) {
		    if(offForm.size() && (iSz%16) == 0) sout += TSYS::strMess(offForm.c_str(), iSz);
		    if(iSz < in.size()) {
			snprintf(buf, sizeof(buf), "%02X", (unsigned char)in[iSz]);
			txt += isprint(in[iSz]) ? in[iSz] : '.';
		    } else strcpy(buf, "  ");
		    if((++iSz)%16) sout = sout + buf + " ";
		    else { sout = sout + buf + "   " + txt + ((iSz<in.size())?"\n":""); txt = ""; }
		}
		break;
	    }
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

    string rez;
    z_stream strm;
    strm.zalloc = Z_NULL;
    strm.zfree = Z_NULL;
    strm.opaque = Z_NULL;

    if(deflateInit(&strm,lev) != Z_OK) return "";

    strm.next_in = (Bytef*)in.data();
    strm.avail_in = (uInt)in.size();

    unsigned char out[vmax(100,vmin((in.size()/10)*10,prmStrBuf_SZ))];

    do {
	strm.next_out = (Bytef*)out;
	strm.avail_out = sizeof(out);
	int ret = deflate(&strm, Z_FINISH);
	if(ret == Z_STREAM_ERROR) { rez = ""; break; }
	rez.append((char*)out, sizeof(out)-strm.avail_out);
    } while(strm.avail_out == 0);

    deflateEnd(&strm);

    return rez;
}

string TSYS::strUncompr( const string &in )
{
    int ret;
    z_stream strm;
    string rez;

    strm.zalloc = Z_NULL;
    strm.zfree = Z_NULL;
    strm.opaque = Z_NULL;
    strm.avail_in = 0;
    strm.next_in = Z_NULL;

    if(in.empty() || inflateInit(&strm) != Z_OK) return "";

    unsigned char out[fmax(100,fmin((((int)in.size()*2)/10)*10,prmStrBuf_SZ))];

    strm.avail_in = in.size();
    strm.next_in = (Bytef*)in.data();
    do {
	strm.avail_out = sizeof(out);
	strm.next_out = out;
	ret = inflate(&strm, Z_NO_FLUSH);
	if(ret == Z_STREAM_ERROR || ret == Z_NEED_DICT || ret == Z_DATA_ERROR || ret == Z_MEM_ERROR || ret == Z_VERSION_ERROR)
	    break;
	rez.append((char*)out, sizeof(out)-strm.avail_out);
    } while(strm.avail_out == 0 && ret != Z_STREAM_END);

    inflateEnd(&strm);

    return (ret == Z_STREAM_END) ? rez : "";
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
	MtxAlloc reqSt(sit->second.reqM, true);
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

string TSYS::prjUserDir( )
{
    string userDir = cmdOpt("projUserDir");
    if(userDir.empty() && !cmdOptPresent("projUserDir"))	userDir = "~/.openscada";
    size_t posHome = userDir.find("~/");
    if(posHome != string::npos && getenv("HOME")) userDir.replace(posHome, 2, string(getenv("HOME"))+"/");

    return userDir;
}

bool TSYS::prjSwitch( const string &prj, bool toCreate )
{
    //Check for the project availability into folder of preistalled ones for writibility and next into the user folder
    struct stat file_stat;

    //Call the projects manager procedure
#if defined(__ANDROID__)
    if(access(bindir_full "/openscada-proj",F_OK) == 0 && access(bindir_full "/openscada-proj",X_OK) != 0)
	chmod(bindir_full "/openscada-proj", 0700);	//Set openscada-proj to the execution mode
#endif
    if(access(bindir_full "/openscada-proj",F_OK|X_OK) == 0)
	system((string("dPrj=") + oscd_datadir_full +
		" dPrjUser=" + prjUserDir() +
		" dSysCfg=" + sysconfdir_full +
		" dData=" + datadir_full +
		" " bindir_full "/openscada-proj" +
		" " + (toCreate?"create":"proc") +
		" " + prj).c_str());

    //Check for the project folder presence and main items creation at missing or wrong the projects manager procedure
    //?!?!

    //Check for the project folder availability and switch to the project
    string  prjDir = prjUserDir() + "/" + prj,
	    prjCfg = prjDir + "/oscada.xml";
    if(!(prjUserDir().size() && stat(prjDir.c_str(),&file_stat) == 0 && (file_stat.st_mode&S_IFMT) == S_IFDIR && access(prjDir.c_str(), X_OK|W_OK) == 0 &&
	stat(prjCfg.c_str(),&file_stat) == 0 && (file_stat.st_mode&S_IFMT) == S_IFREG))
    {
	prjDir = string(oscd_datadir_full) + "/" + prj;
	if(!(stat(prjDir.c_str(),&file_stat) == 0 && (file_stat.st_mode&S_IFMT) == S_IFDIR && access(prjDir.c_str(), X_OK|W_OK) == 0))
	    return false;
	prjCfg = prjDir + "/oscada.xml";
	if(!(stat(prjCfg.c_str(),&file_stat) == 0 && (file_stat.st_mode&S_IFMT) == S_IFREG)) {
	    prjCfg = string(sysconfdir_full) + "/oscada_" + prj + ".xml";
	    if(!(stat(prjCfg.c_str(),&file_stat) == 0 && (file_stat.st_mode&S_IFMT) == S_IFREG))
		return false;
	}
    }

    // Switch to the found project
    //  Set the project configuration file into "--config", name into "--statName", change the work directory
    cmdOpt("config", prjCfg);
    cmdOpt("statName", prj);
    setWorkDir(prjDir, true);

    //  Set an exit signal for restarting the system, clean prjNm and return true
    stop(SIGUSR2);

    return true;
}

int TSYS::prjLockUpdPer( )
{
    int rez = 60;
    if(cmdOpt("projLock").size()) rez = s2i(cmdOpt("projLock"));

    return vmax(0, rez);
}

bool TSYS::prjLock( const char *cmd )
{
    if(strcmp(cmd,"free") == 0 && prjLockFile.size() && remove(prjLockFile.c_str()) == 0) {
	prjLockFile = "";
	return true;
    }

    int hd = -1;
    if(strcmp(cmd,"hold") == 0) {
	prjLockFile = workDir() + "/lock";
	//Check for presented file
	if((hd=open(prjLockFile.c_str(),O_RDONLY)) >= 0) {
	    int bufLn = 35;
	    char buf[bufLn+1]; buf[bufLn] = 0;
	    bool toRemove = (read(hd,buf,bufLn) <= 0);
	    close(hd);
	    if(!toRemove) {
		int pid = 0, tm = 0;
		toRemove = ((sscanf(buf,"%d %d",&pid,&tm) < 2) || pid == getpid() || abs(sysTm()-tm) > 2*prjLockUpdPer());
	    }
	    if(toRemove) remove(prjLockFile.c_str());
	}

	//Hold the lock file
	if((hd=open(prjLockFile.c_str(),O_CREAT|O_EXCL|O_WRONLY,permCrtFiles())) < 0)
	    return false;
    }
    else if(strcmp(cmd,"update") == 0 && (prjLockFile.empty() || (hd=open(prjLockFile.c_str(),O_WRONLY)) < 0))
	return false;

    if(hd >= 0) {
	string lockInfo = TSYS::strMess("%010d %020d", (int)getpid(), (int)sysTm());
	write(hd, lockInfo.data(), lockInfo.size());
	close(hd);
	return true;
    }

    return false;
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
	//Error by this active task presence
	if(time(NULL) >= (c_tm+wtm)) throw err_sys(_("Task '%s' is already present!"), path.c_str());
	sysSleep(0.01);
	res.request(true);
    }
    STask &htsk = mTasks[path];
    htsk.path = path;
    htsk.task = start_routine;
    htsk.taskArg = arg;
    htsk.flgs = 0;
    htsk.thr = 0;
    htsk.phase = s2i(TBDS::genPrmGet(SYS->nodePath()+"TaskPhase:"+path,i2s(mTasks.size()-1)));
    htsk.prior = priority%100;
    res.release();

    if(pAttr) pthr_attr = pAttr;
    else {
	pthr_attr = &locPAttr;
	pthread_attr_init(pthr_attr);
    }
#if !defined(__ANDROID__)
    pthread_attr_setinheritsched(pthr_attr, PTHREAD_EXPLICIT_SCHED);
#endif
    struct sched_param prior;
    prior.sched_priority = 0;

    int policy = SCHED_OTHER;
#if __GLIBC_PREREQ(2,4)
    if(priority < 0)	policy = SCHED_BATCH;
#endif
    if(priority > 0)	policy = SCHED_RR;
    if(priority >= 100)	policy = SCHED_FIFO;
    pthread_attr_setschedpolicy(pthr_attr, policy);
    prior.sched_priority = vmax(sched_get_priority_min(policy), vmin(sched_get_priority_max(policy),priority%100));
    pthread_attr_setschedparam(pthr_attr, &prior);

    try {
	pthread_attr_getdetachstate(pthr_attr, &detachStat);
	if(detachStat == PTHREAD_CREATE_DETACHED) htsk.flgs |= STask::Detached;
	int rez = pthread_create(&procPthr, pthr_attr, taskWrap, &htsk);
	if(rez == EPERM) {
	    mess_sys(TMess::Warning, _("There is no access to create a real-time task for '%s'. An ordinal task is created!"), path.c_str());
	    policy = SCHED_OTHER;
	    pthread_attr_setschedpolicy(pthr_attr, policy);
	    prior.sched_priority = 0;
	    pthread_attr_setschedparam(pthr_attr, &prior);
	    rez = pthread_create(&procPthr, pthr_attr, taskWrap, &htsk);
	}
	if(!pAttr) pthread_attr_destroy(pthr_attr);

	if(rez) throw err_sys(TError::Core_TaskCrt, _("Error %d creation a task."), rez);

	//Wait for thread structure initialization finish for not detachable tasks
	while(!(htsk.flgs&STask::Detached) && !htsk.thr) TSYS::sysSleep(1e-3); //sched_yield(); !!! don't use for hard realtime systems with high priority
	//Wait for start status
	for(time_t c_tm = time(NULL); !(htsk.flgs&STask::Detached) && startSt && !(*startSt); ) {
	    if(time(NULL) >= (c_tm+wtm)) throw err_sys(_("Timeout of starting the task '%s'!"), path.c_str());
	    sysSleep(prmWait_DL);
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
	if(first) pthread_kill(it->second.thr, SIGUSR1);	//User's termination signal, is checked by function taskEndRun()
	if(!noSignal) pthread_kill(it->second.thr, SIGALRM);	//Sleep, select and other system calls termination
	if(cv) pthread_cond_signal(cv);
	res.release();

	time_t c_tm = time(NULL);
	//Check timeout
	if(wtm && (c_tm > (s_tm+wtm))) {
	    mess_sys(TMess::Crit, _("Timeout of the task '%s' !!!"), path.c_str());
	    throw err_sys(_("Task '%s' is not stopped!"), path.c_str());
	}
	//Make messages
	if(c_tm > t_tm+1) {	//1sec
	    t_tm = c_tm;
	    mess_sys(TMess::Info, _("Waiting for an event of the task '%s' ..."), path.c_str());
	}
	sysSleep(prmWait_DL);
	first = false;
	res.request(true);
    }
    if(it != mTasks.end()) {
	if(!(it->second.flgs&STask::Detached)) pthread_join(it->second.thr, NULL);
	mTasks.erase(it);
    }
}

void TSYS::taskSendSIGALRM( const string &path )
{
    ResAlloc res(taskRes, false);
    map<string,STask>::iterator it = mTasks.find(path);
    if(it != mTasks.end())
	pthread_kill(it->second.thr, SIGALRM);	//Sleep, select and other system calls termination
}

double TSYS::taskUtilizTm( const string &path, bool max )
{
    ResAlloc res(taskRes, false);
    map<string,STask>::iterator it = mTasks.find(path);
    if(it == mTasks.end()) return 0;
    if(max) return 1e-9*it->second.consMax;
    int64_t tm_beg = 0, tm_end = 0, tm_per = 0;
    for(int iTr = 0; tm_beg == tm_per && iTr < 2; iTr++) {
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
    //tsk->prior = param.sched_priority;	//!!!! Commented for NICE allowing

#if !defined(__ANDROID__) && __GLIBC_PREREQ(2,4)
    //Load and init CPU set
    if(SYS->nCPU() > 1 && !(tsk->flgs&STask::Detached)) {
	tsk->cpuSet = TBDS::genPrmGet(SYS->nodePath()+"CpuSet:"+tsk->path);
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
#ifdef HAVE_SYSCALL_H
    tsk->tid = syscall(SYS_gettid);
#endif
    // Set nice level without realtime if it no permitted
    if(!(tsk->policy == SCHED_FIFO || tsk->policy == SCHED_RR) && tsk->prior > 0 &&
	    setpriority(PRIO_PROCESS,tsk->tid,-tsk->prior/5) != 0)
	tsk->prior = 0;
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
	SYS->mess_sys(TMess::Error, _("Task %u unexpected terminated by an exception."), tsk->thr);
    }
    //!!!! The code causes - FATAL, do not rethrow
    //catch(...)	{ mess_sys(TMess::Error, _("Task %u unexpected terminated by an unknown exception."), tsk->thr); }

    //Mark for task finish
    tsk->flgs |= STask::FinishTask;

    //> Remove task object for detached
    if(tsk->flgs & STask::Detached)	SYS->taskDestroy(tsk->path, NULL);

    return rez;
}

void *TSYS::ServTask( void * )
{
    for(unsigned int iCnt = 1; !TSYS::taskEndRun(); iCnt++) {
	SYS->isServPrc = true;

	//Lock file update
	if(SYS->prjNm().size() && SYS->prjLockUpdPer() && !(iCnt%SYS->prjLockUpdPer()))
	    SYS->prjLock("update");

	if(SYS->isRunning())
	    try {
		//CPU frequency calculation (per ten seconds)
		if(!(iCnt%10))	SYS->clkCalc();

		//Config-file checking for changes (per ten seconds)
		if(!(iCnt%10))	SYS->cfgFileScan();

		//Translation cache limit per time, limCacheIts_TM(60) seconds
		if(!(iCnt%limCacheIts_TM)) Mess->translCacheLimits(limCacheIts_TM);

		//Checking for shared libraries
		if(SYS->modSchedul().at().chkPer() && !(iCnt%SYS->modSchedul().at().chkPer()))
		    SYS->modSchedul().at().libLoad(SYS->modDir(), true);

		//Changes saving
		if(SYS->savePeriod() && !(iCnt%SYS->savePeriod())) SYS->save();

		//Config-file checking for need to save
		if(!(iCnt%10))	SYS->cfgFileSave();

		//Subsystems calling, per prmServTask_PER(10) seconds
		if(!(iCnt%prmServTask_PER)) {
		    vector<string> lst;
		    SYS->list(lst);
		    for(unsigned iA = 0; iA < lst.size(); iA++)
			try { SYS->at(lst[iA]).at().perSYSCall(iCnt); }
			catch(TError &err) { mess_err(err.cat.c_str(), "%s", err.mess.c_str()); }
		}
	    } catch(TError&) { }

	else if(!SYS->mainThr.freeStat()) SYS->mainThr.at().perSYSCall(0);

	SYS->isServPrc = false;

	TSYS::taskSleep(1000000000);
    }

    return NULL;
}

void *TSYS::HPrTask( void * )
{
    while(!TSYS::taskEndRun()) {
	SYS->mSysTm = time(NULL);
	TSYS::taskSleep(1000000000);
    }

    return NULL;
}

void *TSYS::RdTask( void * )
{
    vector<string> subLs;
    SYS->list(subLs);

    while(!TSYS::taskEndRun())
    try {
	//Update wait time for dead stations and process connections to stations
	ResAlloc res(SYS->mRdRes, false);
	for(map<string,TSYS::SStat>::iterator sit = SYS->mSt.begin(); sit != SYS->mSt.end(); sit++) {
	    // Live stations and connect to new station process
	    if(sit->second.isLive || (!sit->second.isLive && sit->second.cnt <= 0)) {
		// Prepare request to a remote station
		XMLNode req("CntrReqs");
		req.setAttr("path", "/"+sit->first);
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
		if(sit->second.isLive) SYS->mess_sys(TMess::Notice, _("Redundant station '%s' is up."),
						SYS->transport().at().extHostGet("*",sit->first).name.c_str());
		else SYS->mess_sys(TMess::Warning, _("Redundant station '%s' is down."),
						SYS->transport().at().extHostGet("*",sit->first).name.c_str());
		sit->second.isLivePrev = sit->second.isLive;
	    }
	}
	res.release();

	//Call to main service request
	for(int iL = 0; iL < (int)subLs.size(); iL++)
	    if(!SYS->at(subLs[iL]).at().rdProcess())
		subLs.erase(subLs.begin()+(iL--));

	//Waiting the next iteration
	TSYS::taskSleep((int64_t)(SYS->rdTaskPer()*1e9));
    } catch(TError &err) { mess_err(err.cat.c_str(), "%s", err.mess.c_str()); }

    return NULL;
}

int TSYS::sysSleep( float tm )
{
    struct timespec spTm;
    clockid_t clkId = SYS->clockRT() ? CLOCK_REALTIME : CLOCK_MONOTONIC;

    if(tm < 300e-6) {	//Wait into the direct cycle
	for(int64_t stTm = 0, cTm, toTm = 1000000000ll*tm; true; ) {
	    clock_gettime(clkId, &spTm);
	    cTm = 1000000000ll*spTm.tv_sec + spTm.tv_nsec;
	    if(!stTm) stTm = cTm;
	    else if((cTm-stTm) >= toTm) break;
	}
	return 0;
    }

    spTm.tv_sec = (time_t)tm;
    spTm.tv_nsec = (long)(1e9*(tm-floorf(tm)));
    return clock_nanosleep(clkId, 0, &spTm, NULL);

    /*struct timespec spTm;
    spTm.tv_sec = (time_t)tm;
    spTm.tv_nsec = (long int)(1e9*(tm-floorf(tm)));
    return nanosleep(&spTm, NULL);*/
}

void TSYS::taskSleep( int64_t per, const string &icron, int64_t *lag )
{
    struct timespec spTm;
    STask *stsk = (STask*)pthread_getspecific(sTaskKey);

    if(icron.empty()) {
	if(!per) per = 1000000000ll;
	clockid_t clkId = SYS->clockRT() ? CLOCK_REALTIME : CLOCK_MONOTONIC;
	clock_gettime(clkId, &spTm);
	int64_t off = (stsk && SYS->taskInvPhs() > 1) ? per*(stsk->phase%SYS->taskInvPhs())/SYS->taskInvPhs() : 0;	//phasing offset
	int64_t cur_tm = (int64_t)spTm.tv_sec*1000000000ll + spTm.tv_nsec,
		pnt_tm = ((cur_tm-off)/per + 1)*per,
		wake_tm = 0;
	do {
	    spTm.tv_sec = (pnt_tm+off)/1000000000ll; spTm.tv_nsec = (pnt_tm+off)%1000000000ll;
	    if(clock_nanosleep(clkId,TIMER_ABSTIME,&spTm,NULL)) return;
	    clock_gettime(clkId, &spTm);
	    wake_tm = (int64_t)spTm.tv_sec*1000000000ll + spTm.tv_nsec - off;
	} while(wake_tm < pnt_tm);

	if(stsk) {
	    if(stsk->tm_pnt) stsk->cycleLost += vmax(0, pnt_tm/per-stsk->tm_pnt/per-1);
	    if(lag) *lag = stsk->tm_pnt ? wake_tm-stsk->tm_pnt-per : 0;
	    stsk->tm_beg = stsk->tm_per;
	    stsk->tm_end = cur_tm - off;
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
	sscanf(tEl.c_str(), "%d-%d/%d", &vbeg, &vend, &vstep);
	if(vbeg < 0) { sscanf(tEl.c_str(), "*/%d", &vstep); vbeg = 0; vend = 59; }
	if(vend < 0) vm = vmin(vm, vbeg+((ttm.tm_min>=vbeg)?60:0));
	else if((vbeg=vmax(0,vbeg)) < (vend=vmin(59,vend))) {
	    if(ttm.tm_min < vbeg) vm = vmin(vm, vbeg);
	    else if((vstep>1 && ttm.tm_min >= (vbeg+((vend-vbeg)/vstep)*vstep)) || (vstep <= 0 && ttm.tm_min >= vend))
		vm = vmin(vm, vbeg+60);
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
	if(vend < 0) vm = vmin(vm, vbeg+((ttm.tm_hour>vbeg)?24:0));
	else if((vbeg=vmax(0,vbeg)) < (vend=vmin(23,vend))) {
	    if(ttm.tm_hour < vbeg) vm = vmin(vm, vbeg);
	    else if((vstep>1 && ttm.tm_hour > (vbeg+((vend-vbeg)/vstep)*vstep)) || (vstep <= 0 && ttm.tm_hour > vend))
		vm = vmin(vm, vbeg+24);
	    else if(vstep > 1) vm = vmin(vm, vbeg + vstep*((ttm.tm_hour-vbeg)/vstep + (((ttm.tm_hour-vbeg)%vstep)?1:0)));
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
	    if(vend < 0) vm = vmin(vm, vbeg+((ttm.tm_mday>vbeg)?31:0));
	    else if((vbeg=vmax(1,vbeg)) < (vend=vmin(31,vend))) {
		if(ttm.tm_mday < vbeg) vm = vmin(vm, vbeg);
		else if((vstep>1 && ttm.tm_mday > (vbeg+((vend-vbeg)/vstep)*vstep)) || (vstep <= 0 && ttm.tm_mday > vend))
		    vm = vmin(vm, vbeg+31);
		else if(vstep > 1) vm = vmin(vm, vbeg + vstep*((ttm.tm_mday-vbeg)/vstep + (((ttm.tm_mday-vbeg)%vstep)?1:0)));
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
	if(vend < 0) vm = vmin(vm, vbeg+(((ttm.tm_mon+1)>vbeg)?12:0));
	else if((vbeg=vmax(1,vbeg)) < (vend=vmin(12,vend))) {
	    if((ttm.tm_mon+1) < vbeg) vm = vmin(vm, vbeg);
	    else if((vstep>1 && (ttm.tm_mon+1) > (vbeg+((vend-vbeg)/vstep)*vstep)) || (vstep <= 0 && (ttm.tm_mon+1) > vend))
		vm = vmin(vm, vbeg+12);
	    else if(vstep > 1) vm = vmin(vm, vbeg + vstep*(((ttm.tm_mon+1)-vbeg)/vstep + ((((ttm.tm_mon+1)-vbeg)%vstep)?1:0)));
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

TVariant TSYS::objFuncCall( const string &iid, vector<TVariant> &prms, const string &user_lang )
{
    bool alt1 = false;
    // int message(string cat, int level, string mess) - formation of the program message <mess> with the category <cat>, level <level>
    //  cat - message category
    //  level - message level
    //  mess - message text
    if(iid == "message" && prms.size() >= 3)	{ message(prms[0].getS().c_str(), (TMess::Type)prms[1].getI(), "%s", prms[2].getS().c_str()); return 0; }
    // int mess{Debug,Info,Note,Warning,Err,Crit,Alert,Emerg}(string cat, string mess) -
    //		formation of the program message <mess> with the category <cat> and the appropriate level
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
    // {string|int} system(string cmd, bool noPipe = false) - calls the console commands <cmd> of OS returning the result by the channel
    //  cmd - command text
    //  noPipe - pipe result disable for background call
    if(iid == "system" && prms.size() >= 1) {
	if(prms.size() >= 2 && prms[1].getB()) return system(prms[0].getS().c_str());
	FILE *fp = popen(prms[0].getS().c_str(), "r");
	if(!fp) return string("");

	char buf[prmStrBuf_SZ];
	string rez;
	for(int r_cnt = 0; (r_cnt=fread(buf,1,sizeof(buf),fp)) || !feof(fp); )
	    rez.append(buf,r_cnt);

	pclose(fp);
	return rez;
    }
    // int fileSize( string file ) - Return the <file> size.
    if(iid == "fileSize" && prms.size() >= 1) {
	int hd = open(prms[0].getS().c_str(), O_RDONLY);
	int rez = -1;
	if(hd >= 0) {
	    rez = lseek(hd, 0, SEEK_END);
	    close(hd);
	}
	return rez;
    }
    // string fileRead( string file, int off = 0, int sz = -1 ) - Return the <file> content from offset <off> and for the block size <sz>.
    if(iid == "fileRead" && prms.size() >= 1) {
	char buf[prmStrBuf_SZ];
	string rez;
	int hd = open(prms[0].getS().c_str(), O_RDONLY);
	if(hd >= 0) {
	    if(prms.size() >= 2) lseek(hd, prms[1].getI(), SEEK_SET);
	    int sz = (prms.size() >= 3) ? prms[2].getI() : -1;
	    if(sz < 0)
		for(int len = 0; (len=read(hd,buf,sizeof(buf))) > 0; )
		    rez.append(buf, len);
	    else
		for(int len = 0, rLen = 0; rLen < sz && (len=read(hd,buf,fmin(sz-rLen,(int)sizeof(buf)))) > 0; rLen += len)
		    rez.append(buf, len);
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
	int hd = open(prms[0].getS().c_str(), wflags, permCrtFiles());
	if(hd >= 0) {
	    wcnt = write(hd, val.data(), val.size());
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
    // string cntrReq(XMLNodeObj req, string stat = "") - request of the control interface to the program via XML
    //  req - request's XML node
    //  stat - remote OpenSCADA-station for request
    if(iid == "cntrReq" && prms.size() >= 1) {
	XMLNode req;
	AutoHD<XMLNodeObj> xnd = prms[0].getO();
	if(xnd.freeStat()) return _("1:Request is not an object!");
	xnd.at().toXMLNode(req);
	string path = req.attr("path");
	if(prms.size() < 2 || prms[1].getS().empty()) {
	    req.setAttr("user", TSYS::strLine(user_lang,0));
	    cntrCmd(&req);
	}
	else try {
	    req.setAttr("path", "/"+prms[1].getS()+path);
	    transport().at().cntrIfCmd(req, "cntrReq");
	    req.setAttr("path", path);
	} catch(TError &err) { return TSYS::strMess(_("10:Error remote request: %s"), err.mess.c_str()); }
	xnd.at().fromXMLNode(req);
	return "0";
    }
    // string lang(string full) - returns the system language in two symbols and the full language in <full>
    //  full - microseconds of time
    if(iid == "lang") {
	if(prms.size() >= 1) { prms[0].setS(Mess->lang()); prms[0].setModify(); }
	return Mess->lang2Code();
    }
    // int sleep(real tm, int ntm = 0) - call for task sleep to <tm> seconds and <ntm> nanoseconds.
    //  tm - wait time in seconds (precised up to nanoseconds), up to prmInterf_TM(7 seconds)
    //  ntm - wait time part in nanoseconds
    if(iid == "sleep" && prms.size() >= 1) {
	return sysSleep(fmin(prms[0].getR()+1e-9*((prms.size()>=2)?prms[1].getI():0),(double)prmInterf_TM));
	/*struct timespec spTm;
	spTm.tv_sec = prms[0].getI();
	spTm.tv_nsec = 1000000000l*(prms[0].getR()-spTm.tv_sec) + ((prms.size()>=2)?prms[1].getI():0);
	spTm.tv_sec = vmin(prmInterf_TM, spTm.tv_sec);
	clockid_t clkId = SYS->clockRT() ? CLOCK_REALTIME : CLOCK_MONOTONIC;
	return clock_nanosleep(clkId, 0, &spTm, NULL);*/
	//return nanosleep(&spTm, NULL);
    }
    // int time(int usec) - returns absolute time in seconds from the epoch of 1/1/1970 and in microseconds, if <usec> is specified
    //  usec - microseconds of time
    if(iid == "time") {
	if(prms.empty()) return (int64_t)time(NULL);
	int64_t tm = curTime();
	prms[0].setI(tm%1000000); prms[0].setModify();
	return (int64_t)(tm/1000000);
    }
    // int utime(); int mtime(); - return absolute time in microseconds and milliseconds from the epoch of 1/1/1970
    if(iid == "utime") return (int64_t)curTime();
    if(iid == "mtime") return (int64_t)curTime()/1000;
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
	int rez = 0;
	string lang = TSYS::strLine(user_lang, 1);
	if(lang.size()) {
	    lang = Mess->langToLocale(lang);
	    //!?!? Prepare the locales cache to speed up. Use in new locale string functions of conversion real numbers
	    locale_t dLoc = newlocale(LC_TIME_MASK, lang.c_str(), 0);
	    if(dLoc != 0) {
		rez = strftime_l(buf, sizeof(buf), (prms.size()>=2) ? prms[1].getS().c_str() : "%Y-%m-%d %H:%M:%S", &tm_tm, dLoc);
		freelocale(dLoc);
		return (rez > 0) ? string(buf,rez) : "";
	    }
	}
	rez = strftime(buf, sizeof(buf), (prms.size()>=2) ? prms[1].getS().c_str() : "%Y-%m-%d %H:%M:%S", &tm_tm);
	return (rez > 0) ? string(buf,rez) : "";
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
	for(unsigned iP = 0; iP < prms.size(); iP++)
	    rez += (unsigned char)prms[iP].getI();
	return rez;
    }
    // string strFromCharUTF([string type = "UTF-8",] int char1, int char2, int char3, ...) - string creation from UTF codes
    //  type - symbol type, (UTF-8, UTF-16, UTF-16LE, UTF-16BE, UTF-32, UTF-32LE, UTF-32BE)
    //  char1, char2. char3 - symbol's codes
    if(iid == "strFromCharUTF") {
	string rez;
	int st = 0;
	string tp = "";
	if(prms.size() && prms[0].type() == TVariant::String) st = 1, tp = TSYS::strEncode(prms[0].getS(),TSYS::ToLower);
	if(tp.find("utf-16") == 0) {
	    uint16_t tvl = 0;
	    for(unsigned iP = st; iP < prms.size(); iP++) {
		if(tp.find("be") != string::npos) tvl = TSYS::i16_BE(prms[iP].getI());
		else tvl = TSYS::i16_LE(prms[iP].getI());
		rez += string((const char*)&tvl, 2);
	    }
	}
	else if(tp.find("utf-32") == 0) {
	    uint32_t tvl = 0;
	    for(unsigned iP = st; iP < prms.size(); iP++) {
		if(tp.find("be") != string::npos) tvl = TSYS::i32_BE(prms[iP].getI());
		else tvl = TSYS::i32_LE(prms[iP].getI());
		rez += string((const char*)&tvl, 4);
	    }
	}
	else for(unsigned iP = st; iP < prms.size(); iP++)
	    rez += TMess::setUTF8(prms[iP].getI());

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
    //			   "OscdID", "Bin", "Reverse", "ShieldSimb", "ToLower", "Limit", "ShieldBin"
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
	else if(stp == "ToLower")	tp = ToLower;
	else if(stp == "Limit")		tp = Limit;
	else if(stp == "ShieldBin")	tp = ShieldBin;
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

    return TCntrNode::objFuncCall(iid, prms, user_lang);
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
    sort(its.begin(), its.end());
    for(unsigned iIt = 0; iIt < its.size(); iIt++)
	nd->childAdd("el")->setText(fsBase+(pathLev?"/":"")+its[iIt]);
    sort(fits.begin(), fits.end());
    for(unsigned iIt = 0; iIt < fits.size(); iIt++)
	nd->childAdd("el")->setText(fsBase+(pathLev?"/":"")+fits[iIt]);
}

void TSYS::cntrCmdProc( XMLNode *opt )
{
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
	ctrMkNode("oscada_cntr",opt,-1,"/",TSYS::strMess(_("%s station: \"%s\""),PACKAGE_NAME,trD(name()).c_str()),R_R_R_)->
	    setAttr("doc","Program_manual|Documents/Program_manual");
	if(ctrMkNode("branches",opt,-1,"/br","",R_R_R_))
	    ctrMkNode("grp",opt,-1,"/br/sub_",_("Subsystem"),R_R_R_,"root","root",1,"idm","1");
	if(TUIS::icoGet(name(),NULL,true).size() || TUIS::icoGet(id(),NULL,true).size()) ctrMkNode("img",opt,-1,"/ico","",R_R_R_);
	if(ctrMkNode("area",opt,-1,"/gen",_("Station"),R_R_R_)) {
	    ctrMkNode("fld",opt,-1,"/gen/id",_("Identifier"),R_R_R_,"root","root",1,"tp","str");
	    ctrMkNode("fld",opt,-1,"/gen/stat",_("Station name"),RWRWR_,"root","root",1,"tp","str");
	    ctrMkNode("fld",opt,-1,"/gen/prog",_("Program"),R_R_R_,"root","root",1,"tp","str");
	    ctrMkNode("fld",opt,-1,"/gen/ver",_("Version"),R_R_R_,"root","root",1,"tp","str");
	    ctrMkNode("fld",opt,-1,"/gen/host",_("Host name"),R_R_R_,"root","root",1,"tp","str");
	    ctrMkNode("fld",opt,-1,"/gen/user",_("System user"),R_R_R_,"root","root",1,"tp","str");
	    ctrMkNode("fld",opt,-1,"/gen/sys",_("Operation system"),R_R_R_,"root","root",1,"tp","str");
	    ctrMkNode("fld",opt,-1,"/gen/CPU",_("CPU"),R_R_R_,"root","root",1,"tp","str");
	    if(nCPU() > 1)
		ctrMkNode("fld",opt,-1,"/gen/mainCPUs",_("Main CPUs set"),RWRWR_,"root","root",2,"tp","str",
		    "help",_("To set up the processors you use, write a row of numbers separated by a ':' character.\nProcessor numbers start at 0."));
	    ctrMkNode("fld",opt,-1,"/gen/clk",_("Tasks planning clock"),R_R_R_,"root","root",1,"tp","str");
	    ctrMkNode("fld",opt,-1,"/gen/taskInvPhs",_("Number of phases of the task invoking"),RWRWR_,"root","root",2,"tp","dec",
		"help",_("To set up phasing of the task invoking in the determined phases number, <= 0 to set optimal, 1 to disable the tasks phasing."));
	    ctrMkNode("fld",opt,-1,"/gen/in_charset",_("Internal charset"),R_R___,"root","root",1,"tp","str");
	    ctrMkNode("fld",opt,-1,"/gen/config",_("Configuration file"),R_R___,"root","root",1,"tp","str");
	    ctrMkNode("fld",opt,-1,"/gen/workdir",_("Work directory"),R_R___,"root","root",3,"tp","str","dest","sel_ed","select","/gen/workDirList");
	    ctrMkNode("fld",opt,-1,"/gen/moddir",_("Modules directory"),R_R___,"root","root",3,"tp","str","dest","sel_ed","select","/gen/modDirList");
	    ctrMkNode("fld",opt,-1,"/gen/icodir",_("Icons directory"),R_R___,"root","root",4,"tp","str","dest","sel_ed","select","/gen/icoDirList",
		"help",_("Separate directory paths with icons by symbol ';'."));
	    ctrMkNode("fld",opt,-1,"/gen/docdir",_("Documents directory"),R_R___,"root","root",4,"tp","str","dest","sel_ed","select","/gen/docDirList",
		"help",_("Separate directory paths with documents by symbol ';'."));
	    ctrMkNode("fld",opt,-1,"/gen/wrk_db",_("Work DB"),RWRWR_,"root","root",4,"tp","str","dest","select","select","/db/list",
		"help",_("Work DB address in format \"{DB module}.{DB name}\".\nChange this field if you want to save or to reload all the program from other DB."));
	    ctrMkNode("fld",opt,-1,"/gen/saveExit",_("Save the program at exit"),RWRWR_,"root","root",2,"tp","bool",
		"help",_("Select for the program automatic saving to DB on exit."));
	    ctrMkNode("fld",opt,-1,"/gen/savePeriod",_("Period the program saving"),RWRWR_,"root","root",2,"tp","dec",
		"help",_("Use not a zero period (seconds) to periodically save program changes to the DB."));
	    ctrMkNode("fld",opt,-1,"/gen/modifCalc",_("Set modification for the calculated objects"),RWRWR_,"root","root",2,"tp","bool",
		"help",_("Most suitable for the production systems together with the previous configuration properties, for the calculation context saving.\n"
			 "But it is inconvinient in the development mode, all time reminding for the saving need."));
	    ctrMkNode("fld",opt,-1,"/gen/lang",_("Language"),RWRWR_,"root","root",3,
		"tp","str", "dest","sel_ed", "sel_list",Mess->langBase().c_str());
	    if(ctrMkNode("area",opt,-1,"/gen/mess",_("Messages"),R_R_R_)) {
		ctrMkNode("fld",opt,-1,"/gen/mess/lev",_("Least level"),RWRWR_,"root","root",6,"tp","dec","len","1","dest","select",
		    "sel_id","0;1;2;3;4;5;6;7",
		    "sel_list",_("Debug (0);Information (1[X]);Notice (2[X]);Warning (3[X]);Error (4[X]);Critical (5[X]);Alert (6[X]);Emergency (7[X])"),
		    "help",_("Least messages level which is procesed by the program."));
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
	    ctrMkNode("fld",opt,-1,"/redund/tskPer",_("Redundant task period, seconds"),RWRWR_,"root","root",1,"tp","real");
	    ctrMkNode("fld",opt,-1,"/redund/restConn",_("Restore connection timeout, seconds"),RWRWR_,"root","root",1,"tp","dec");
	    ctrMkNode("fld",opt,-1,"/redund/primCmdTr",_("Local primary commands transfer"),RWRWR_,"root","root",1,"tp","bool");
	    if(ctrMkNode("table",opt,-1,"/redund/sts",_("Stations"),RWRWR_,"root","root",2,"key","st","s_com","add,del")) {
		ctrMkNode("list",opt,-1,"/redund/sts/st",_("Identifier"),RWRWR_,"root","root",3,"tp","str","dest","select","select","/redund/lsSt");
		ctrMkNode("list",opt,-1,"/redund/sts/name",_("Name"),R_R_R_,"root","root",1,"tp","str");
		ctrMkNode("list",opt,-1,"/redund/sts/live",_("Live"),R_R_R_,"root","root",1,"tp","bool");
		ctrMkNode("list",opt,-1,"/redund/sts/lev",_("Level"),R_R_R_,"root","root",1,"tp","dec");
		ctrMkNode("list",opt,-1,"/redund/sts/cnt",_("Counter"),R_R_R_,"root","root",1,"tp","real");
	    }
	    ctrMkNode("comm",opt,-1,"/redund/hostLnk",_("Go to the configuration of the list of remote stations"),0660,"root","Transport",1,"tp","lnk");
	}
	if(ctrMkNode("area",opt,-1,"/tr",_("Translations"))) {
	    ctrMkNode("fld",opt,-1,"/tr/status",_("Status"),R_R_R_,"root","root",1,"tp","str");
	    XMLNode *blNd = ctrMkNode("fld",opt,-1,"/tr/baseLang",_("Base language - locales list"),RWRWR_,"root","root",2,
		"tp","str", "help",_("Enables the multilingual support for text variables in the configuration DBs "
		    "by entry the base language and the project whole locales (like to \"en_US.UTF-8\") list (optional) separated by ';'.\n"
		    "You can entry here other language besides English(en) as the base, but take in your mind that "
		    "all standard OpenSCADA libraries formed for the base language English(en), so other base languages "
		    "will break these DBs at change!"));
	    if(Mess->lang2CodeBase().size()) {
		blNd->setAttr("dscr", blNd->attr("dscr") + ", " + _("dynamic translation"));
		ctrMkNode("fld",opt,-1,"/tr/dynPlan","",RWRW__,"root","root",2,"tp","bool","help",_("Dynamic translation scheduling for next startup."));
		if(!Mess->translDyn())
		    ctrMkNode("fld",opt,-1,"/tr/enMan",_("Enable the manager"),RWRWR_,"root","root",2,"tp","bool",
			"help",_("Enables the generic translation manager which cause full reloading for all built messages obtain."));
	    }
	    if(Mess->translEnMan()) {
		ctrMkNode("fld",opt,-1,"/tr/langs",_("Languages of managing"),RWRW__,"root","root",2,"tp","str",
		    "help",_("List of the processing languages, in the two-character representation and separated by the character ';'."));
		ctrMkNode("fld",opt,-1,"/tr/fltr",_("Source filter, check/fix, pass"),RWRW__,"root","root",2,"tp","str",
		    "help",_("Source filter of limiting the manager work in some DBs, the source just must contain such substring."));
		ctrMkNode("fld",opt,-1,"/tr/chkAndFix","",RWRW__,"root","root",2,"tp","bool",
		    "help",_("Enable for checking the base messages translation equality in different sources\n"
			    "and fixing by: propagation the translations to empty sources, "
			    "clearing the double to base translations "
			    "and merging the base messages as the translations."));
		ctrMkNode("fld",opt,-1,"/tr/pass","",RWRW__,"root","root",3,"tp","dec","min","0",
		    "help",_("Pass the specified number of the table items from the top, "
			    "useful for very big projects which are limited in time of the table complete processing."));
		if(ctrMkNode("table",opt,-1,"/tr/mess",_("Messages"),RWRW__,"root","root",1,"key","base")) {
		    ctrMkNode("list",opt,-1,"/tr/mess/base",Mess->lang2CodeBase().c_str(),RWRW__,"root","root",1,"tp","str");
		    string lngEl;
		    for(int off = 0; (lngEl=strParse(Mess->translLangs(),0,";",&off)).size(); )
			if(lngEl.size() == 2 && lngEl != Mess->lang2CodeBase())
			    ctrMkNode("list",opt,-1,("/tr/mess/"+lngEl).c_str(),lngEl.c_str(),RWRW__,"root","root",1,"tp","str");
		    ctrMkNode("list",opt,-1,"/tr/mess/src",_("Source"),R_R_R_,"root","root",1,"tp","str");
		}
	    }
	}
	if(ctrMkNode("area",opt,-1,"/tasks",_("Tasks"),R_R___))
	    if(ctrMkNode("table",opt,-1,"/tasks/tasks",_("Tasks"),RWRW__,"root","root",1,"key","path")) {
		ctrMkNode("list",opt,-1,"/tasks/tasks/path",_("Path"),R_R___,"root","root",1,"tp","str");
		ctrMkNode("list",opt,-1,"/tasks/tasks/thrd",_("Thread"),R_R___,"root","root",1,"tp","str");
		ctrMkNode("list",opt,-1,"/tasks/tasks/tid",_("TID"),R_R___,"root","root",1,"tp","dec");
		ctrMkNode("list",opt,-1,"/tasks/tasks/stat",_("Status"),R_R___,"root","root",1,"tp","str");
		ctrMkNode("list",opt,-1,"/tasks/tasks/plc",_("Policy"),R_R___,"root","root",1,"tp","str");
		ctrMkNode("list",opt,-1,"/tasks/tasks/prior",_("Prior."),R_R___,"root","root",1,"tp","dec");
#if __GLIBC_PREREQ(2,4)
		if(nCPU() > 1) ctrMkNode("list",opt,-1,"/tasks/tasks/cpuSet",_("CPU set"),RWRW__,"root","root",2,"tp","str",
		    "help",_("To set up the processors you use, write a row of numbers separated by a ':' character."));
#endif
		if(taskInvPhs() > 1) ctrMkNode("list",opt,-1,"/tasks/tasks/taskPh",_("Phase"),RWRW__,"root","root",2,"tp","dec",
		    "help",_("To set up the task invoking phase."));
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
	    ctrMkNode("comm",opt,-1,"/debug/mess",_("See to the messages"),R_R_R_,"root","root",1,"tp","lnk");
	}
	return;
    }

    //Process command to page
    if(a_path == "/ico" && ctrChkNode(opt)) {
	string itp;
	opt->setText(TSYS::strEncode(TUIS::icoGet(name(),&itp),TSYS::base64));
	if(!itp.size())
	    opt->setText(TSYS::strEncode(TUIS::icoGet(id(),&itp),TSYS::base64));
	opt->setAttr("tp", itp);
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
	if(ctrChkNode(opt,"get",RWRWR_,"root","root",SEC_RD))	opt->setText(trD(name()));
	if(ctrChkNode(opt,"set",RWRWR_,"root","root",SEC_WR))	setName(trDSet(name(),opt->text()));
    }
    else if(a_path == "/gen/CPU" && ctrChkNode(opt))	opt->setText(strMess(_("%dx%0.3gGHz"),nCPU(),(float)sysClk()/1e9));
    else if(a_path == "/gen/mainCPUs") {
	if(ctrChkNode(opt,"get",RWRWR_,"root","root",SEC_RD)) {
	    string vl = mainCPUs();
#if !defined(__ANDROID__) && __GLIBC_PREREQ(2,4)
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
    else if(a_path == "/gen/taskInvPhs") {
	if(ctrChkNode(opt,"get",RWRWR_,"root","root",SEC_RD))	opt->setText(i2s(taskInvPhs()));
	if(ctrChkNode(opt,"set",RWRWR_,"root","root",SEC_WR))	setTaskInvPhs(s2i(opt->text()));
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
    else if(a_path == "/gen/modifCalc") {
	if(ctrChkNode(opt,"get",RWRWR_,"root","root",SEC_RD))	opt->setText(i2s(modifCalc()));
	if(ctrChkNode(opt,"set",RWRWR_,"root","root",SEC_WR))	setModifCalc(s2i(opt->text()));
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
	for(unsigned iA = 0; iA < lst.size(); iA++)
	    opt->childAdd("el")->setAttr("id",lst[iA])->setText(at(lst[iA]).at().subName());
    }
    else if(a_path == "/redund/status" && ctrChkNode(opt,"get",R_R_R_,"root","root"))
	opt->setText(TSYS::strMess(_("Spent time: %s[%s]."),
	    tm2s(SYS->taskUtilizTm("SYS_Redundancy")).c_str(), tm2s(SYS->taskUtilizTm("SYS_Redundancy",true)).c_str()));
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
	    XMLNode *n_cpuSet	= (nCPU() > 1) ? ctrMkNode("list",opt,-1,"/tasks/tasks/cpuSet","",RWRW__,"root","root") : NULL;
	    XMLNode *n_taskPh	= (taskInvPhs() > 1) ? ctrMkNode("list",opt,-1,"/tasks/tasks/taskPh","",RWRW__,"root","root") : NULL;

	    ResAlloc res(taskRes, false);
	    for(map<string,STask>::iterator it = mTasks.begin(); it != mTasks.end(); it++) {
		if(n_path)	n_path->childAdd("el")->setText(it->first);
		if(n_thr)	n_thr->childAdd("el")->setText(u2s(it->second.thr));
		if(n_tid)	n_tid->childAdd("el")->setText(i2s(it->second.tid));
		if(n_stat) {
		    int64_t	tm_beg = 0, tm_end = 0, tm_per = 0, tm_pnt = 0, lagMax = 0, consMax = 0;
		    for(int iTr = 0; tm_beg == tm_per && iTr < 2; iTr++) {
			tm_beg = it->second.tm_beg; tm_end = it->second.tm_end;
			tm_per = it->second.tm_per; tm_pnt = it->second.tm_pnt;
			lagMax = it->second.lagMax; consMax = it->second.consMax;
		    }
		    XMLNode *cn = n_stat->childAdd("el");
		    if(it->second.flgs&STask::FinishTask) cn->setText(_("Finished. "));
		    if(tm_beg && tm_beg < tm_per)
			cn->setText(cn->text()+TSYS::strMess(_("Last: %s. Consumed: %3.1f[%3.1f]%% (%s from %s). Lag: %s[%s]. Lost cycles: %g."),
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
#if !defined(__ANDROID__) && __GLIBC_PREREQ(2,4)
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
		if(n_taskPh)	n_taskPh->childAdd("el")->setText(i2s(it->second.phase%taskInvPhs()));
	    }
	}
#if !defined(__ANDROID__) && __GLIBC_PREREQ(2,4)
	if(nCPU() > 1 && opt->attr("col") == "cpuSet" && ctrChkNode(opt,"set",RWRW__,"root","root",SEC_WR)) {
	    ResAlloc res(taskRes, true);
	    map<string,STask>::iterator it = mTasks.find(opt->attr("key_path"));
	    if(it == mTasks.end()) throw err_sys(_("The task '%s' is missing."));
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
	    TBDS::genPrmSet(nodePath()+"CpuSet:"+it->first, it->second.cpuSet);
	    if(rez == EINVAL) throw err_sys(_("Attempt to set missing processor."));
	    if(rez) throw err_sys(_("Error installing processors for the thread."));
	}
#endif
	if(taskInvPhs() > 1 && opt->attr("col") == "taskPh" && ctrChkNode(opt,"set",RWRW__,"root","root",SEC_WR)) {
	    ResAlloc res(taskRes, true);
	    map<string,STask>::iterator it = mTasks.find(opt->attr("key_path"));
	    if(it == mTasks.end()) throw err_sys(_("The task '%s' is missing."));
	    if(it->second.flgs & STask::Detached) return;

	    int tVl = vmax(0, vmin(100,s2i(opt->text())));
	    it->second.phase = tVl;
	    res.release();
	    TBDS::genPrmSet(nodePath()+"TaskPhase:"+it->first, i2s(tVl));
	}
    }
    else if(a_path == "/tr/status" && ctrChkNode(opt)) {
	string stM, stV;
	if(Mess->lang2CodeBase().empty())
	    stM = _("SINGLELANGUAGE"), stV += _("only use the already multilanguage DBs with their modification");
	else {
	    if(Mess->translDyn()) stM = _("MULTILANGUAGE-DYNAMIC"), stV += _("dynamic translation");
	    else stM = _("MULTILANGUAGE");
	    stV += (Mess->translDyn()?", ":"") +
		    TSYS::strMess(_("creating or modification the configuration DBs as multilanguage ones with the pointed base language '%s'"),
			Mess->lang2CodeBase().c_str());
	}
	if(stV.size() && (Mess->trMessIdx.size() || Mess->trMessCache.size()))
	    stV += ". " + TSYS::strMess(_("Messages indexed=%d, cached=%d."), Mess->trMessIdx.size(), Mess->trMessCache.size());
	if(stV.size() && Mess->trCtxs.size())
	    stV += (stV[stV.size()-1]!='.'?". ":" ") + TSYS::strMess(_("Translation contexts=%d."), Mess->trCtxs.size());
	opt->setText(stM+", "+stV);
    }
    else if(a_path == "/tr/baseLang") {
	if(ctrChkNode(opt,"get",RWRWR_,"root","root",SEC_RD))	opt->setText(Mess->langBase());
	if(ctrChkNode(opt,"set",RWRWR_,"root","root",SEC_WR))	Mess->setLangBase(opt->text());
    }
    else if(a_path == "/tr/dynPlan") {
	if(ctrChkNode(opt,"get",RWRW__,"root","root",SEC_RD))	opt->setText(i2s(Mess->translDyn(true)));
	if(ctrChkNode(opt,"set",RWRW__,"root","root",SEC_WR))	Mess->setTranslDyn(s2i(opt->text()));
    }
    else if(a_path == "/tr/enMan") {
	if(ctrChkNode(opt,"get",RWRWR_,"root","root",SEC_RD))	opt->setText(i2s(Mess->translEnMan()));
	if(ctrChkNode(opt,"set",RWRWR_,"root","root",SEC_WR))	Mess->setTranslEnMan(s2i(opt->text()));
    }
    else if(a_path == "/tr/langs") {
	if(ctrChkNode(opt,"get",RWRW__,"root","root",SEC_RD))	opt->setText(Mess->translLangs());
	if(ctrChkNode(opt,"set",RWRW__,"root","root",SEC_WR))	Mess->setTranslLangs(opt->text());
    }
    else if(a_path == "/tr/fltr") {
	if(ctrChkNode(opt,"get",RWRW__,"root","root",SEC_RD))	opt->setText(TBDS::genPrmGet(nodePath()+"TrFltr","",opt->attr("user")));
	if(ctrChkNode(opt,"set",RWRW__,"root","root",SEC_WR))	{
	    TBDS::genPrmSet(nodePath()+"TrFltr",opt->text(),opt->attr("user"));
	    TBDS::genPrmSet(nodePath()+"TrPassN","0",opt->attr("user"));
	}
    }
    else if(a_path == "/tr/chkAndFix") {
	if(ctrChkNode(opt,"get",RWRW__,"root","root",SEC_RD))	opt->setText(TBDS::genPrmGet(nodePath()+"TrChkAndFix","0",opt->attr("user")));
	if(ctrChkNode(opt,"set",RWRW__,"root","root",SEC_WR))	TBDS::genPrmSet(nodePath()+"TrChkAndFix",opt->text(),opt->attr("user"));
    }
    else if(a_path == "/tr/pass") {
	if(ctrChkNode(opt,"get",RWRW__,"root","root",SEC_RD))	opt->setText(TBDS::genPrmGet(nodePath()+"TrPassN","0",opt->attr("user")));
	if(ctrChkNode(opt,"set",RWRW__,"root","root",SEC_WR))	TBDS::genPrmSet(nodePath()+"TrPassN",opt->text(),opt->attr("user"));
    }
    else if(a_path == "/tr/mess") {
	if(ctrChkNode(opt,"get",RWRW__,"root","root",SEC_RD)) {
	    bool chkAndFix = s2i(TBDS::genPrmGet(nodePath()+"TrChkAndFix","0",opt->attr("user")));
	    string tStr, trFltr = TBDS::genPrmGet(nodePath()+"TrFltr","",opt->attr("user"));
	    unsigned passN = vmax(0, s2i(TBDS::genPrmGet(nodePath()+"TrPassN","0",opt->attr("user")))),
		    mess_TrModifMarkLen = strlen(mess_TrModifMark);
	    TConfig req;
	    req.setNoTransl(true);
	    vector<XMLNode*> ns;

	    // Columns list preparing
	    ns.push_back(ctrMkNode("list",opt,-1,"/tr/mess/base","",RWRW__,"root","root"));
	    for(int off = 0; (tStr=strParse(Mess->translLangs(),0,";",&off)).size(); )
		if(tStr.size() == 2 && tStr != Mess->lang2CodeBase())
		    ns.push_back(ctrMkNode("list",opt,-1,("/tr/mess/"+tStr).c_str(),tStr.c_str(),RWRW__,"root","root"));
	    ns.push_back(ctrMkNode("list",opt,-1,"/tr/mess/src","",R_R_R_,"root","root"));

	    // Values requesting from the first source
	    MtxAlloc res(Mess->trMessIdxRes, true);
	    time_t stTm = time(NULL);
	    map<string, map<string,string> >::iterator im = Mess->trMessIdx.begin();
	    for(unsigned pos = 0; im != Mess->trMessIdx.end() && (time(NULL)-stTm) < prmInterf_TM; ++im) {
		//  Checking for the filter
		if(trFltr.size()) {
		    map<string,string>::iterator is;
		    for(is = im->second.begin(); is != im->second.end() && (is->first+"#"+is->second).find(trFltr) == string::npos; ++is) ;
		    if(is == im->second.end()) continue;
		}

		//  Pass the specified items number
		if((pos++) < passN) continue;

		//  Rows appending
		for(unsigned iN = 0; iN < ns.size(); iN++) {
		    if(iN == 0)
			ns[iN]->childAdd("el")->setText(im->first);
		    else if(iN < (ns.size()-1))
			ns[iN]->childAdd("el")->setText("");	//Empty cells at the start
		    else {
			tStr.clear();
			for(map<string,string>::iterator is = im->second.begin(); is != im->second.end(); ++is)
			    tStr += (tStr.size()?"\n":"")+is->first;// + "#" + is->second;
			ns[iN]->childAdd("el")->setText(tStr);
		    }
		}
		if(ns.size() <= 2) continue;	//No any translated languages set

		//  Real translated data obtaining and checking
		bool firstInst = true;
		for(map<string,string>::iterator is = im->second.begin(); is != im->second.end(); ++is)
		try {
		    //  Pass the out filter sources
		    if(trFltr.size() && (is->first+"#"+is->second).find(trFltr) == string::npos) continue;

		    string tMath, trSrc = TSYS::strParse(is->first, 0, "#"), trFld = TSYS::strParse(is->first, 1, "#");
		    bool isCfg = false;
		    //  Source is the config file or the included DB
		    if((isCfg=trSrc.find("cfg:")==0) || trSrc.find("db:") == 0) {
			//  Need for the DB structure preparing
			req.elem().fldClear();
			req.elem().fldAdd(new TFld(trFld.c_str(),"",TFld::String,0));
			for(unsigned iN = 1; iN < ns.size(); iN++)
			    req.elem().fldAdd(new TFld(Mess->translFld(ns[iN]->attr("id"),trFld,isCfg).c_str(),"",TFld::String,0));
			req.cfg(trFld).setReqKey(true);
			req.cfg(trFld).setS(im->first);

			//  Getting from the config file or the DB source
			bool seekRez = false;
			for(int inst = 0; true; inst++) {
			    seekRez = isCfg ? TBDS::dataSeek("", trSrc.substr(4), inst, req, TBDS::UseCache)
					    : TBDS::dataSeek(trSrc.substr(3), "", inst, req, TBDS::UseCache);
			    if(!seekRez) break;
			    for(unsigned iN = 0; iN < ns.size(); iN++) {
				if(!(iN && iN < (ns.size()-1))) continue;
				tMath = req.cfg(Mess->translFld(ns[iN]->attr("id"),trFld,isCfg)).getS();
				XMLNode *recNd = ns[iN]->childGet(-1);
				if(chkAndFix && tMath.empty())	recNd->setAttr("toPropagOnSp", "1");
				if(firstInst)	recNd->setText(tMath);
				else if(!s2i(recNd->attr("unmatch"))) {
				    if(tMath.empty()) ;
				    else if(recNd->text().empty())	recNd->setText(tMath);
				    else if(tMath != recNd->text()) {
					if(recNd->text().empty()) recNd->setText(tMath);
					recNd->setText(string(_("<<<Unmatched sources>>>\n"))+"1. "+recNd->text()+"\n2. "+tMath);
					recNd->setAttr("unmatch", "1")->attrDel("toPropagOnSp");
				    }
				}
			    }
			    firstInst = false;
			    if(!chkAndFix) break;
			}
		    }
		    if(!chkAndFix) break;
		} catch(TError &err) { continue; }

		//  Postprocessing the translation checking
		/*for(unsigned iN = 0; chkAndFix && iN < ns.size(); iN++) {
		    if(!(iN && iN < (ns.size()-1))) continue;*/

		for(int iN = ns.size()-2; iN >= 0; --iN) {
		    XMLNode *recNd = ns[iN]->childGet(-1);
		    if(iN == 0) {
			//   Processing the translation changing mark
			if(recNd->text().size() > mess_TrModifMarkLen &&
				recNd->text().rfind(mess_TrModifMark) == (recNd->text().size()-mess_TrModifMarkLen))
			    recNd->setText(string(_("<<<Translation changed>>>\n"))+
					    recNd->text().substr(0,recNd->text().size()-mess_TrModifMarkLen));
			continue;
		    }

		    XMLNode *recNdBs = ns[0]->childGet(-1);
		    if(chkAndFix) {
			string lng = ns[iN]->attr("dscr");
			bool needReload = false;
			if(!s2i(recNd->attr("unmatch")) && recNd->text().size()) {
			    //   Clearing the equel to base translation
			    if(recNd->text() == recNdBs->text()) {
				mess_warning((nodePath()+"Tr").c_str(), _("Clearing the equel to base translation '%s' for '%s'."),
				    lng.c_str(), recNdBs->text().c_str());
				Mess->translSet(recNdBs->text(), lng, "", &needReload, TBDS::genPrmGet(nodePath()+"TrFltr","",opt->attr("user")));
				recNd->setText("");
			    }
			    else if(s2i(recNd->attr("toPropagOnSp"))) {
				mess_warning((nodePath()+"Tr").c_str(), _("Propagation the translation '%s' to all empty sources for '%s'.'%s'."),
				    lng.c_str(), recNdBs->text().c_str(), recNd->text().c_str());
				Mess->translSet(recNdBs->text(), lng, recNd->text(), &needReload, TBDS::genPrmGet(nodePath()+"TrFltr","",opt->attr("user")));
			    }
			}
			if(recNd->text().size() && recNd->text() != recNdBs->text() && Mess->trMessIdx.find(recNd->text()) != Mess->trMessIdx.end()) {
			    mess_warning((nodePath()+"Tr").c_str(), _("Merging the base message to the translation '%s' for '%s' > '%s'."),
				lng.c_str(), recNd->text().c_str(), recNdBs->text().c_str());
			    //   Copying the real translations
			    for(unsigned iN2 = 1; iN2 < ns.size()-1; ++iN2) {
				XMLNode *recNd2 = ns[iN2]->childGet(-1);
				string lng2 = ns[iN2]->attr("dscr");
				Mess->translSet(recNd->text(), lng2, recNd2->text(), &needReload);
			    }

			    //   Replacing the message base
			    Mess->translSet(recNd->text(), Mess->lang2CodeBase(), recNdBs->text(), &needReload);
			}
			recNd->attrDel("unmatch")->attrDel("toPropagOnSp");
		    }

		    //   Processing the base changing mark
		    if(recNd->text().size() > mess_TrModifMarkLen &&
			    recNd->text().rfind(mess_TrModifMark) == (recNd->text().size()-mess_TrModifMarkLen))
			recNd->setText(string(_("<<<Base changed>>>\n"))+
					recNd->text().substr(0,recNd->text().size()-mess_TrModifMarkLen));
		}
	    }
	}
	if(ctrChkNode(opt,"set",RWRW__,"root","root",SEC_WR)) {
	    bool needReload = false;
	    Mess->translSet(opt->attr("key_base"), ((opt->attr("col")=="base")?Mess->lang2CodeBase():opt->attr("col")), opt->text(),
				&needReload, TBDS::genPrmGet(nodePath()+"TrFltr","",opt->attr("user")));
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

	    MtxAlloc res(Mess->dbgRes, true);
	    for(map<string,bool>::iterator idc = Mess->debugCats.begin(); idc != Mess->debugCats.end(); idc++) {
		if(n_cat) n_cat->childAdd("el")->setText(idc->first);
		if(n_prc) n_prc->childAdd("el")->setText(r2s(idc->second));
	    }
	}
	if(ctrChkNode(opt,"set",RWRWR_,"root","root",SEC_WR)) {
	    MtxAlloc res(Mess->dbgRes, true);
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

#if defined(__ANDROID__)
int main( int argc, char *argv[] )
{
    int rez = 0;

    setenv("QT_SCALE_FACTOR", "1.5", 1);

    //Same load and start the core object TSYS
    SYS = new TSYS(argc, argv, NULL);
    try {
	while(true) {
	    SYS->load();
	    if((rez=SYS->stopSignal()) && rez != SIGUSR2)
		throw TError(SYS->nodePath().c_str(), "Stop by signal %d on load.", rez);
	    if(!rez) rez = SYS->start();
	    if(rez != SIGUSR2)	break;
	    SYS->unload();
	}
    } catch(TError err) { mess_err(err.cat.c_str(), "%s", err.mess.c_str()); }

    //Free OpenSCADA root object
    if(SYS) delete SYS;

    return rez;
}
#endif
