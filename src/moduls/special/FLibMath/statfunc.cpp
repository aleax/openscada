
//OpenSCADA module Special.FLibMath file: statfunc.cpp
/***************************************************************************
 *   Copyright (C) 2005-2022 by Roman Savochenko, <roman@oscada.org>       *
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

#include "stdmath.h"
#include "statfunc.h"

//*************************************************
//* Modul info!                                   *
#define MOD_ID		"FLibMath"
#define MOD_NAME	trS("Math functions")
#define MOD_TYPE	SSPC_ID
#define VER_TYPE	SSPC_VER
#define SUB_TYPE	"LIB"
#define MOD_VER		"0.7.4"
#define AUTHORS		trS("Roman Savochenko")
#define DESCRIPTION	trS("Provides a library of the standard mathematical functions.")
#define LICENSE		"GPL2"
//*************************************************

FLibMath::Lib *FLibMath::mod;

extern "C"
{
#ifdef MOD_INCL
    TModule::SAt spec_FLibMath_module( int n_mod )
#else
    TModule::SAt module( int n_mod )
#endif
    {
	if( n_mod==0 )	return TModule::SAt(MOD_ID,MOD_TYPE,VER_TYPE);
	return TModule::SAt("");
    }

#ifdef MOD_INCL
    TModule *spec_FLibMath_attach( const TModule::SAt &AtMod, const string &source )
#else
    TModule *attach( const TModule::SAt &AtMod, const string &source )
#endif
    {
	if( AtMod == TModule::SAt(MOD_ID,MOD_TYPE,VER_TYPE) )
	    return new FLibMath::Lib( source );
	return NULL;
    }
}

using namespace FLibMath;

//*************************************************
//* Lib: Complex1 functions library               *
//*************************************************
Lib::Lib( string src ) : TSpecial(MOD_ID)
{
    mod = this;

    modInfoMainSet(MOD_NAME, MOD_TYPE, MOD_VER, AUTHORS, DESCRIPTION, LICENSE, src);

    m_fnc = grpAdd("fnc_");
}

Lib::~Lib( )
{

}

void Lib::postEnable( int flag )
{
    TModule::postEnable(flag);

    if(flag&TCntrNode::NodeRestore)	return;

    //Register the functions
    reg(new MathAcos());
    reg(new MathAsin());
    reg(new MathAtan());
    reg(new MathCos());
    reg(new MathCosh());
    reg(new MathExp());
    reg(new MathLn());
    reg(new MathLg());
    reg(new MathPow());
    reg(new MathSin());
    reg(new MathSinh());
    reg(new MathSqrt());
    reg(new MathTan());
    reg(new MathTanh());
    reg(new MathCeil());
    reg(new MathAbs());
    reg(new MathFloor());
    reg(new MathRand());
    reg(new MathIf());

    //Enable the functions
    vector<string> lst;
    list(lst);
    for(unsigned iL = 0; iL < lst.size(); iL++)
	at(lst[iL]).at().setStart(true);
}

void Lib::modStart( )
{
    vector<string> lst;
    list(lst);
    for(unsigned iL = 0; iL < lst.size(); iL++)
	at(lst[iL]).at().setStart(true);
    runSt = true;
}

void Lib::modStop( )
{
    vector<string> lst;
    list(lst);
    for(unsigned iL = 0; iL < lst.size(); iL++)
	at(lst[iL]).at().setStart(false);
    runSt = false;
}

TVariant Lib::objFuncCall( const string &id, vector<TVariant> &prms, const string &user_lang )
{
    // ElTp {funcID}(ElTp prm1, ...) - the function {funcID} call
    //  prm{N} - {N} parameter to the function.
    if(present(id)) return at(id).at().objFuncCall("call", prms, user_lang);

    return TCntrNode::objFuncCall(id, prms, user_lang);
}

void Lib::cntrCmdProc( XMLNode *opt )
{
    //Get page info
    if(opt->name() == "info") {
	TSpecial::cntrCmdProc(opt);
	ctrMkNode("grp",opt,-1,"/br/fnc_",_("Function"),R_R_R_,"root",SSPC_ID,1,"idm","1");
	ctrMkNode("list",opt,-1,"/prm/func",_("Functions"),R_R_R_,"root",SSPC_ID,3,"tp","br","idm","1","br_pref","fnc_");
	return;
    }

    //Process command to page
    string a_path = opt->attr("path");
    if((a_path == "/br/fnc_" || a_path == "/prm/func") && ctrChkNode(opt)) {
	vector<string> lst;
	list(lst);
	for(unsigned iF = 0; iF < lst.size(); iF++)
	    opt->childAdd("el")->setAttr("id",lst[iF])->setText(at(lst[iF]).at().name());
    } else TSpecial::cntrCmdProc(opt);
}
