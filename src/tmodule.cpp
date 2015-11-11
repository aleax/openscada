
//OpenSCADA system file: tmodule.cpp
/***************************************************************************
 *   Copyright (C) 2003-2015 by Roman Savochenko, <rom_as@oscada.org>      *
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
#include <stdarg.h>
#include <unistd.h>
#include <dlfcn.h>
#include <string.h>

#include "tsys.h"
#include "terror.h"
#include "tmess.h"
#include "tsubsys.h"
#include "tmodule.h"

#ifdef HAVE_LIBINTL_H
#include <libintl.h>
#endif

using namespace OSCADA;

//*************************************************
//* TModule                                       *
//*************************************************
const char *TModule::lInfo[] = {"Module", "Name", "Type", "Source", "Version", "Author", "Description", "License"};

TModule::TModule( const string &id ) : mModId(id)
{
    lcId = string("oscd_")+mModId;

#ifdef HAVE_LIBINTL_H
    bindtextdomain(lcId.c_str(), localedir_full);
#endif

    //Dynamic string translation hook
#if 0
    char mess[][100] = { _("Author"), _("License") };
#endif

    if(mess_lev() == TMess::Debug) SYS->cntrIter(objName(), 1);
}

TModule::~TModule( )
{
    //Clean export function list
    for(unsigned i = 0; i < mEfunc.size(); i++) delete mEfunc[i];

    if(mess_lev() == TMess::Debug) SYS->cntrIter(objName(), -1);
}

string TModule::objName( )	{ return TCntrNode::objName()+":TModule"; }

string TModule::modName( )	{ return mModName; }

void TModule::modInfoMainSet( const string &name, const string &type, const string &vers, const string &author,
			      const string &descr, const string &license, const string &source )
{
    mModName	= name;
    mModType	= type;
    mModVers	= vers;
    mModAuthor	= author;
    mModDescr	= descr;
    mModLicense	= license;
    mModSource	= source;
}

void TModule::postEnable( int flag )
{
    if(flag&TCntrNode::NodeRestore)	return;

    mess_info(nodePath().c_str(),_("Connect module."));
}

TSubSYS &TModule::owner( )	{ return *(TSubSYS*)nodePrev(); }

void TModule::modFuncList( vector<string> &list )
{
    list.clear();
    for(unsigned i = 0; i < mEfunc.size(); i++)
	list.push_back(mEfunc[i]->prot);
}

bool TModule::modFuncPresent( const string &prot )
{
    for(unsigned i = 0; i < mEfunc.size(); i++)
	if(mEfunc[i]->prot == prot)
	    return true;

    return false;
}

TModule::ExpFunc &TModule::modFunc( const string &prot )
{
    for(unsigned i = 0; i < mEfunc.size(); i++)
	if(mEfunc[i]->prot == prot) return *mEfunc[i];
    throw TError(nodePath().c_str(),_("Function '%s' is not present in the module!"),prot.c_str());
}

void TModule::modFunc( const string &prot, void (TModule::**offptr)() )
{
    *offptr = modFunc(prot).ptr;
}

void TModule::modInfo( vector<string> &list )
{
    for(unsigned i_opt = 0; i_opt < sizeof(lInfo)/sizeof(char *); i_opt++)
	list.push_back(lInfo[i_opt]);
}

string TModule::modInfo( const string &name )
{
    string info;

    if(name == lInfo[0])	info = mModId;
    else if(name == lInfo[1])	info = mModName;
    else if(name == lInfo[2])	info = mModType;
    else if(name == lInfo[3])	info = mModSource;
    else if(name == lInfo[4])	info = mModVers;
    else if(name == lInfo[5])	info = mModAuthor;
    else if(name == lInfo[6])	info = mModDescr;
    else if(name == lInfo[7])	info = mModLicense;

    return info;
}

void TModule::cntrCmdProc( XMLNode *opt )
{
    //Get page info
    if(opt->name() == "info") {
	TCntrNode::cntrCmdProc(opt);
	ctrMkNode("oscada_cntr",opt,-1,"/",_("Module: ")+modId(),R_R_R_,"root","root",1,
	    "doc",("Modules/"+owner().subId()+"."+modId()+"|"+modId()).c_str());
	ctrMkNode("branches",opt,-1,"/br","",R_R_R_);
	if(TUIS::icoGet(owner().subId()+"."+modId(),NULL,true).size()) ctrMkNode("img",opt,-1,"/ico","",R_R_R_);
	if(ctrMkNode("area",opt,-1,"/module",_("Module")))
	    if(ctrMkNode("area",opt,-1,"/module/m_inf",_("Module information"))) {
		vector<string> list;
		modInfo(list);
		for(unsigned i_l = 0; i_l < list.size(); i_l++)
		    ctrMkNode("fld",opt,-1,(string("/module/m_inf/")+list[i_l]).c_str(),_(list[i_l].c_str()),R_R_R_,"root","root",1,"tp","str");
	    }
	return;
    }

    //Process command to page
    string a_path = opt->attr("path");
    if(a_path == "/ico" && ctrChkNode(opt)) {
	string itp;
	opt->setText(TSYS::strEncode(TUIS::icoGet(owner().subId()+"."+modId(),&itp),TSYS::base64));
	opt->setAttr("tp",itp);
    }
    else if(a_path.compare(0,13,"/module/m_inf") == 0 && ctrChkNode(opt)) opt->setText(modInfo(TSYS::pathLev(a_path,2)));
    else TCntrNode::cntrCmdProc(opt);
}

const char *TModule::I18N( const char *mess )
{
#ifdef HAVE_LIBINTL_H
    const char *rez = Mess->I18N(mess, lcId.c_str());
    if(!strcmp(mess,rez)) rez = _(mess);
    return rez;
#else
    return mess;
#endif
}
