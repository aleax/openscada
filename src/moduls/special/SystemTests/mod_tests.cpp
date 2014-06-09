
//OpenSCADA system module Special.SystemTests file: test_kernel.cpp
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

#include <string>

#include <sys/times.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <math.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>
#include <stdarg.h>

#include "mod_tests.h"
#include "test_prm.h"
#include "test_xml.h"
#include "test_mess.h"
#include "test_SOAttach.h"
#include "test_val.h"
#include "test_DB.h"
#include "test_TrOut.h"
#include "test_SysContrLang.h"
#include "test_ValBuf.h"
#include "test_archive.h"
#include "test_Base64Code.h"

//*************************************************
//* Modul info!                                   *
#define MOD_ID		"SystemTests"
#define MOD_NAME	_("OpenSCADA system's tests")
#define MOD_TYPE	SSPC_ID
#define VER_TYPE	SSPC_VER
#define SUB_TYPE	"TEST"
#define MOD_VER		"1.5.1"
#define AUTHORS		_("Roman Savochenko")
#define DESCRIPTION	_("Allow the group tests for OpenSCADA system.")
#define LICENSE		"GPL2"
//*************************************************

KernelTest::TTest *KernelTest::mod;

extern "C"
{
    TModule::SAt module( int n_mod )
    {
	if(n_mod == 0)	return TModule::SAt(MOD_ID,MOD_TYPE,VER_TYPE);
	return TModule::SAt("");
    }

    TModule *attach( const TModule::SAt &AtMod, const string &source )
    {
	if(AtMod == TModule::SAt(MOD_ID,MOD_TYPE,VER_TYPE)) return new KernelTest::TTest(source);
	return NULL;
    }
}

using namespace KernelTest;

//*************************************************
//* BDTest::TTest                                 *
//*************************************************
TTest::TTest( string name ) : TSpecial(MOD_ID)
{
    mod		= this;

    mName	= MOD_NAME;
    mType	= MOD_TYPE;
    mVers	= MOD_VER;
    mAuthor	= AUTHORS;
    mDescr	= DESCRIPTION;
    mLicense	= LICENSE;
    mSource	= name;

    mTest = grpAdd("test_",true);
}

TTest::~TTest()
{
    if(run_st) modStop();
}

void TTest::postEnable( int flag )
{
    TModule::postEnable(flag);

    if(flag&TCntrNode::NodeRestore) return;

    //> Reg static tests
    testReg(new TestPrm());
    testReg(new TestXML());
    testReg(new TestMess());
    testReg(new TestSOAttach());
    testReg(new TestVal());
    testReg(new TestDB());
    testReg(new TestTrOut());
    testReg(new TestSysContrLang());
    testReg(new TestValBuf());
    testReg(new TestArchive());
    testReg(new TestBase64Code());

    //> Enable static tests
    vector<string> lst;
    testList(lst);
    for(unsigned i_l = 0; i_l < lst.size(); i_l++)
	testAt(lst[i_l]).at().setStart(true);
}

string TTest::cat( )	{ return "TEST:"MOD_ID":"; }

void TTest::mess( const string &testNm, const char *fmt,  ... )
{
    char mess[STR_BUF_LEN];
    va_list argptr;

    va_start(argptr, fmt);
    vsnprintf(mess, sizeof(mess), fmt, argptr);
    va_end(argptr);

    mess_info((cat()+testNm).c_str(), "%s", mess);
}

string TTest::modInfo( const string &name )
{
    if(name == "SubType") return SUB_TYPE;
    else return TModule::modInfo(name);
}

void TTest::modInfo( vector<string> &list )
{
    TModule::modInfo(list);
    list.push_back("SubType");
}

string TTest::optDescr( )
{
    string rez;

    rez = TSYS::strMess(_(
	"======================= The module <%s:%s> options =======================\n"
	"---------- Parameters of the module section '%s' in config-file ----------\n"
	"All tests main options:\n"
	"  id           test's id;\n"
	"  on           on test's flag;\n"
	"  per          repeat period (sek).\n"
	"       *** Test's options ***\n"),MOD_TYPE,MOD_ID,nodePath().c_str());

    vector<string> ls;
    testList(ls);
    for(unsigned i_t = 0; i_t < ls.size(); i_t++)
    {
	AutoHD<TFunction> fnc = testAt(ls[i_t]);
	rez += TSYS::strMess("%d) %s\t%s\n",i_t+1,fnc.at().id().c_str(),fnc.at().descr().c_str());
	for(int i_prm = 0; i_prm < fnc.at().ioSize( ); i_prm++)
	{
	    if(fnc.at().io(i_prm)->flg() & (IO::Output|IO::Return))	continue;
	    rez += TSYS::strMess("  %d:%s\t%s\n",i_prm,fnc.at().io(i_prm)->id().c_str(),fnc.at().io(i_prm)->name().c_str());
	}
    }
    rez += "\n";

    return rez;
}

void TTest::load_( )
{
    //> Load parameters from command line
    string argCom, argVl;
    for(int argPos = 0; (argCom=SYS->getCmdOpt(argPos,&argVl)).size(); )
        if(argCom == "h" || argCom == "help")	fprintf(stdout,"%s",optDescr().c_str());

    //> Load parameters from config-file
}

void TTest::modStart(  )
{
    if(!run_st) SYS->taskCreate(nodePath('.',true), 0, Task, this);
}

void TTest::modStop(  )
{
    if(run_st) SYS->taskDestroy(nodePath('.',true), &endrun);
}

void *TTest::Task( void *CfgM )
{
    int count = 0, i_cnt = 0;

    TTest *tst = (TTest*)CfgM;
    tst->run_st = true;
    tst->endrun = false;

    //Task counter
    while(!tst->endrun)
    {
	//1 sec
	if(++i_cnt > 1000/STD_WAIT_DELAY)  // 1 sec
	{
	    i_cnt = 0;
	    if(++count == 1000000) count = 0;

	    //Get All fields
	    ResAlloc res(SYS->nodeRes(), false);
	    XMLNode *mn = SYS->cfgNode(tst->nodePath(0,false)), *t_n = NULL;
	    for(int nd_cnt = 0; mn && (t_n=mn->childGet("prm",nd_cnt++,true)); )
		if(tst->testPresent(t_n->attr("id")) && t_n->attr("on") == "1" &&
		    atoi(t_n->attr("per").c_str()) && !(count%atoi(t_n->attr("per").c_str())))
		{
		    AutoHD<TFunction> fnc = tst->testAt(t_n->attr("id"));
		    TValFunc fc("stdcalc", &fnc.at());
		    for(int i_prm = 0; i_prm < fc.ioSize( ); i_prm++)
		    {
			if(fc.ioFlg(i_prm) & (IO::Output|IO::Return))	continue;
			fc.setS(i_prm, t_n->attr(fc.func()->io(i_prm)->id()));
		    }
		    try{ fc.calc("root"); }
		    catch(TError err) { mess_err(err.cat.c_str(), "%s", err.mess.c_str()); }
		}
	}
	TSYS::sysSleep(STD_WAIT_DELAY*1e-3);
    }
    tst->run_st = false;

    return NULL;
}

void TTest::prXMLNode( const string &testNm, XMLNode *node, int level )
{
    vector<string> list;
    mess(testNm, _("%s{%d \"%s\", text \"%s\", childs - %d."),
	string(level,' ').c_str(), level, node->name().c_str(), node->text().c_str(), node->childSize());
    node->attrList(list);
    for(unsigned i_att = 0; i_att < list.size(); i_att++)
	mess(testNm, _("        Attr \"%s\" = \"%s\"."), list[i_att].c_str(), node->attr(list[i_att]).c_str());
    for(unsigned i_ch = 0; i_ch < node->childSize(); i_ch++)
	prXMLNode(testNm, node->childGet(i_ch), level+1);
    mess(testNm, "%s}%d \"%s\"", string(level,' ').c_str(), level, node->name().c_str());
}

TVariant TTest::objFuncCall( const string &iid, vector<TVariant> &prms, const string &user )
{
    // ElTp {funcID}(ElTp prm1, ...) - the test {funcID} call
    //  prm{N} - {N} parameter to the test.
    if(testPresent(iid)) return testAt(iid).at().objFuncCall("call", prms, user);

    return TCntrNode::objFuncCall(iid, prms, user);
}

void TTest::cntrCmdProc( XMLNode *opt )
{
    //Get page info
    if(opt->name() == "info")
    {
	TSpecial::cntrCmdProc(opt);
	ctrMkNode("grp",opt,-1,"/br/test_",_("Test"),R_R_R_,"root",SSPC_ID,2,"idm",OBJ_NM_SZ,"idSz",OBJ_ID_SZ);
	if(ctrMkNode("area",opt,1,"/tests",_("Tests")))
	{
	    ctrMkNode("fld",opt,-1,"/tests/nmb",_("Number"),R_R_R_,"root",SSPC_ID,1,"tp","str");
	    ctrMkNode("list",opt,-1,"/tests/tests",_("Tests"),R_R_R_,"root",SSPC_ID,4,
		"tp","br","idm",OBJ_NM_SZ,"br_pref","test_","idSz",OBJ_ID_SZ);
	}
	return;
    }
    //Process command to page
    string a_path = opt->attr("path");
    if((a_path == "/br/test_" || a_path == "/tests/tests") && ctrChkNode(opt,"get",R_R_R_,"root",SSPC_ID,SEC_RD))
    {
	vector<string> lst;
	testList(lst);
	for(unsigned i_t=0; i_t < lst.size(); i_t++)
	    opt->childAdd("el")->setAttr("id",lst[i_t])->setText(testAt(lst[i_t]).at().name());
    }
    else if(a_path == "/tests/nmb" && ctrChkNode(opt))
    {
	vector<string> lst;
	testList(lst);
	int enCnt = 0;
	for(unsigned i_t = 0; i_t < lst.size(); i_t++)
	    if(testAt(lst[i_t]).at().startStat()) enCnt++;
	opt->setText(TSYS::strMess(_("All: %d; Accessing: %d"),lst.size(),enCnt));
    }
    else TSpecial::cntrCmdProc(opt);
}
