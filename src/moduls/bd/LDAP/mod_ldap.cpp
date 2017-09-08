
//OpenSCADA system module DB.ODBC file: mod_ldap.cpp
/***************************************************************************
 *   Copyright (C) 2017 by Roman Savochenko, <rom_as@oscada.org>           *
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

#include <errno.h>

#include <ldap.h>

#include <tsys.h>
#include <tmess.h>

#include "mod_ldap.h"

//************************************************
//* Module info!                                 *
#define MOD_ID		"LDAP"
#define MOD_NAME	_("Directory by LDAP")
#define MOD_TYPE	SDB_ID
#define VER_TYPE	SDB_VER
#define MOD_VER		"0.1.0"
#define AUTHORS		_("Roman Savochenko")
#define DESCRIPTION	_("BD module. Provides support of directories by LDAP.")
#define MOD_LICENSE	"GPL2"
//************************************************

DB_LDAP::BDMod *DB_LDAP::mod;	//Pointer for direct access to the module

extern "C"
{
#ifdef MOD_INCL
    TModule::SAt DB_LDAP_module( int n_mod )
#else
    TModule::SAt module( int n_mod )
#endif
    {
	if(n_mod == 0)	return TModule::SAt(MOD_ID, MOD_TYPE, VER_TYPE);
	return TModule::SAt("");
    }
#ifdef MOD_INCL
    TModule *DB_LDAP_attach( const TModule::SAt &AtMod, const string &source )
#else
    TModule *attach( const TModule::SAt &AtMod, const string &source )
#endif
    {
	if(AtMod == TModule::SAt(MOD_ID,MOD_TYPE,VER_TYPE)) return new DB_LDAP::BDMod(source);
	return NULL;
    }
}

using namespace DB_LDAP;

//************************************************
//* DB_LDAP::BDMod				 *
//************************************************
BDMod::BDMod(string name) : TTypeBD(MOD_ID)
{
    mod = this;

    modInfoMainSet(MOD_NAME, MOD_TYPE, MOD_VER, AUTHORS, DESCRIPTION, MOD_LICENSE, name);
}

BDMod::~BDMod( )
{

}

TBD *BDMod::openBD( const string &name )	{ return new MBD(name, &owner().openDB_E()); }


//************************************************
//* DB_LDAP::MBD				 *
//************************************************
MBD::MBD( string iid, TElem *cf_el ) :
    TBD(iid, cf_el), ldp(NULL), reqCnt(0), reqCntTm(0), trOpenTm(0), connRes(true)
{

}

MBD::~MBD( )
{

}

void MBD::postDisable( int flag )
{
    TBD::postDisable(flag);

    if(flag && owner().fullDeleteDB()) {
	MtxAlloc resource(connRes, true);
    }
}

void MBD::enable( )
{
    MtxAlloc resource(connRes, true);
    if(enableStat())	return;

    int off = 0;
    string uri = TSYS::strParse(addr(), 0, ";", &off);
    bdn = TSYS::strParse(addr(), 0, ";", &off);
    string adn = TSYS::strParse(addr(), 0, ";", &off);
    string pass = TSYS::strParse(addr(), 0, ";", &off);
    string tm = TSYS::strParse(addr(), 0, ";", &off);
    float rtm = tm.size() ? s2r(tm) : 10;

    try {
	int rez;
	ldp = NULL;
	if((rez=ldap_initialize(&ldp,uri.c_str())) != LDAP_SUCCESS)	throw err_sys(_("INIT: %s"), ldap_err2string(rez));
	int version = 3;
	if(ldap_set_option(ldp,LDAP_OPT_PROTOCOL_VERSION,&version) != LDAP_OPT_SUCCESS)	throw err_sys(_("OPT_PROTOCOL_VERSION: set %d error."), version);
	//if(ldap_start_tls_s(ldp,NULL,NULL) != LDAP_SUCCESS)		throw err_sys("START TLS: %s", strerror(errno));
	//if(ldap_set_option(ldp,LDAP_OPT_REFERRALS,LDAP_OPT_OFF) != LDAP_OPT_SUCCESS)	throw err_sys("OPT_REFERRALS: off error.");
	struct timeval timeout = {(int)rtm, (int)(rtm*1e6)%1000000};
	if(ldap_set_option(ldp,LDAP_OPT_NETWORK_TIMEOUT,&timeout) != LDAP_OPT_SUCCESS ||
	    ldap_set_option(ldp,LDAP_OPT_TIMEOUT,&timeout) != LDAP_OPT_SUCCESS)		throw err_sys(_("OPT_TIMEOUT: %g error."), 1.0);
	if((rez=ldap_simple_bind_s(ldp,adn.c_str(),pass.c_str())) != LDAP_SUCCESS)	throw err_sys(_("BIND: %s"), ldap_err2string(rez));

	TBD::enable();
    } catch(TError&) { disable(); throw; }
}

void MBD::disable( )
{
    MtxAlloc resource(connRes, true);

    TBD::disable();

    if(ldp) ldap_unbind_s(ldp);
    ldp = NULL;
}

void MBD::allowList( vector<string> &list ) const
{
    MtxAlloc resource(*const_cast<ResMtx*>(&connRes), true);
    if(!enableStat())	return;
    list.clear();
    LDAPMessage	*result, *entry;
    char *attrs[2]; attrs[0] = (char*)"ou"; attrs[1] = NULL;
    if(ldap_search_s(ldp,bdn.c_str(),LDAP_SCOPE_ONE,NULL,attrs,0,&result) != LDAP_SUCCESS)	return;
    for(entry = ldap_first_entry(ldp,result); entry; entry = ldap_next_entry(ldp,entry)) {
	char **vals = ldap_get_values(ldp, entry, "ou");
	if(vals) { list.push_back(vals[0]); ldap_value_free(vals); }
    }
    ldap_msgfree(result);
}

TTable *MBD::openTable( const string &inm, bool create )
{
    MtxAlloc resource(*const_cast<ResMtx*>(&connRes), true);
    if(!enableStat()) throw err_sys(_("Error open table '%s'. DB is disabled."), inm.c_str());

    int rez;
    LDAPMessage	*result;
    char *attrs[2]; attrs[0] = (char*)"ou"; attrs[1] = NULL;
    if((rez=ldap_search_s(ldp,bdn.c_str(),LDAP_SCOPE_ONE,("(ou="+inm+")").c_str(),attrs,0,&result)) != LDAP_SUCCESS)
	throw err_sys(_("SEARCH: %s"), ldap_err2string(rez));
    rez = ldap_count_entries(ldp, result);
    ldap_msgfree(result);
    if(!rez) throw err_sys(_("OpenTable '%s': missed."), inm.c_str());

    return new MTable(inm, this);
}

void MBD::cntrCmdProc( XMLNode *opt )
{
    //Get page info
    if(opt->name() == "info") {
	TBD::cntrCmdProc(opt);
	ctrRemoveNode(opt, "/sql");
	ctrMkNode("fld",opt,-1,"/prm/cfg/ADDR",EVAL_STR,enableStat()?R_R___:RWRW__,"root",SDB_ID,1,
	    "help",_("Directory server address as \"ldap[s]://{host}[:{port}];bdn[;{adn};{pass}[;{tm}]]\".\n"
	    "Where:\n"
	    "  host - hostname of the directory service;\n"
	    "  port - port, default 389;\n"
	    "  bdn  - base DN, an entry which sub-entries means as tables of the database with RDN attribute \"ou\" presents;\n"
	    "  adn  - Distinguished Name of the auth user, omiting the user and next password will cause to anonymous connection;\n"
	    "  pass - password of the Distinguished Name of the auth user;\n"
	    "  tm   - network and generic timeout in seconds by real number."));
	return;
    }

    //Process command to page
    string a_path = opt->attr("path");
    TBD::cntrCmdProc(opt);
}

//************************************************
//* DB_LDAP::Table                                *
//************************************************
MTable::MTable( string name, MBD *iown ) : TTable(name)
{
    setNodePrev(iown);
    codepage = owner().codePage().size() ? owner().codePage() : Mess->charset();
}

MTable::~MTable( )			{ }

void MTable::postDisable( int flag )	{ }

MBD &MTable::owner( ) const	{ return (MBD&)TTable::owner(); }

void MTable::fieldStruct( TConfig &cfg )
{
    MtxAlloc resource(owner().connRes, true);
    if(!owner().enableStat())	return;
    LDAPMessage	*result, *entry;
    if(ldap_search_s(owner().ldp,("ou="+name()+","+owner().bdn).c_str(),LDAP_SCOPE_ONE,NULL,NULL,0,&result) != LDAP_SUCCESS)	return;
    for(entry = ldap_first_entry(owner().ldp,result); entry; entry = ldap_next_entry(owner().ldp,entry)) {
	char *chDN, *a, **vals;
	//Use the DN for key attribute detection
	if(!(chDN=ldap_get_dn(owner().ldp,entry))) continue;
	string keyA = TSYS::strParse(TSYS::strParse(chDN,0,","),0,"=");
	ldap_memfree(chDN);
	BerElement *ber;
	for(a = ldap_first_attribute(owner().ldp,entry,&ber); a != NULL; a = ldap_next_attribute(owner().ldp,entry,ber)) {
	    //!! May be read the attribute properties from the schema
	    cfg.elem().fldAdd(new TFld(a,a,TFld::String,(keyA==a)?(int)TCfg::Key:(int)TFld::NoFlag));
	    ldap_memfree(a);
	}
    }
    ldap_msgfree(result);
}

bool MTable::fieldSeek( int row, TConfig &cfg, vector< vector<string> > *full )
{
    MtxAlloc resource(owner().connRes, true);
    if(!owner().enableStat())	return false;
    mLstUse = SYS->sysTm();

    vector< vector<string> >	inTbl,
				&tbl = full ? *full : inTbl;

    //Request
    if(!full || !full->size() || row == 0) {
	tbl.clear();
	vector<string> row, cf_el;

	// Request preparing
	map<string,int> headers;
	cfg.cfgList(cf_el);
	char *attrs[cf_el.size()+1];
	row.reserve(cf_el.size());
	string fltr; int fltrN = 0;
	for(unsigned iC = 0, iA = 0; iC < cf_el.size(); iC++) {
	    TCfg &cf = cfg.cfg(cf_el[iC]);
	    if(cf.isKey() && cf.keyUse()) { fltr += "("+cf_el[iC]+"="+getVal(cf)+")"; fltrN++; }
	    else if(cf.isKey() || cf.view()) {
		headers[cf_el[iC]] = row.size();
		row.push_back(cf_el[iC]);
		attrs[iA++] = (char*)cf_el[iC].c_str();	attrs[iA] = NULL;
	    }
	}
	tbl.push_back(row);
	if(fltrN > 1)	fltr = "&"+fltr;
	if(fltr.empty())fltr = "(objectclass=*)";

	// Request
	LDAPMessage	*result, *entry;
	if(ldap_search_s(owner().ldp,("ou="+name()+","+owner().bdn).c_str(),LDAP_SCOPE_ONE,fltr.c_str(),attrs,0,&result) != LDAP_SUCCESS) return false;
	for(entry = ldap_first_entry(owner().ldp,result); entry; entry = ldap_next_entry(owner().ldp,entry)) {
	    row.clear(); row.resize(headers.size());
	    char *a, **vals;
	    BerElement *ber;
	    bool entrMatch = true;
	    for(a = ldap_first_attribute(owner().ldp,entry,&ber); a != NULL && entrMatch; a = ldap_next_attribute(owner().ldp,entry,ber)) {
		string attr = a;
		ldap_memfree(a);
		string val;
		if((vals=ldap_get_values(owner().ldp,entry,attr.c_str())) != NULL) {
		    for(int iV = 0; vals[iV] != NULL; iV++) val += vals[iV];
		    ldap_value_free(vals);
		}

		TCfg *cf = NULL;	//cfg.at(attr, true);
		for(unsigned iC = 0; iC < cf_el.size() && !cf; iC++)
		    if(strcasecmp(attr.c_str(),cf_el[iC].c_str()) == 0) { cf = cfg.at(cf_el[iC], true); attr = cf_el[iC]; }

		if(cf) {
		    if(cf->isKey() && (val.empty() || (cf->keyUse() && getVal(*cf) != val)))	entrMatch = false;
		    if(headers.find(attr) != headers.end()) row[headers[attr]] = val;
		}
	    }
	    if(entrMatch) {
		// Late checking for keys
		for(unsigned iC = 0; iC < cf_el.size() && entrMatch; iC++) {
		    TCfg &cf = cfg.cfg(cf_el[iC]);
		    if(cf.isKey() && !cf.keyUse() && row[headers[cf_el[iC]]].empty())	entrMatch = false;
		}
		if(entrMatch) tbl.push_back(row);
	    }
	}
	ldap_msgfree(result);
    }

    if(tbl.size() < 2 || (full && (row+1) >= tbl.size())) return false;

    //Processing of the query
    row = full ? row+1 : 1;
    for(unsigned iFld = 0; iFld < tbl[0].size(); iFld++) {
	string sid = tbl[0][iFld];
	TCfg *cf = cfg.at(sid, true);
	if(cf) setVal(*cf, tbl[row][iFld]);
    }

    return true;
}

void MTable::fieldGet( TConfig &cfg )
{
    MtxAlloc resource(owner().connRes, true);
    if(!owner().enableStat())	return;
    mLstUse = SYS->sysTm();

    vector<string> cf_el;

    //Request preparing
    cfg.cfgList(cf_el);
    char *attrs[cf_el.size()+1];
    string fltr; int fltrN = 0;
    for(unsigned iC = 0, iA = 0; iC < cf_el.size(); iC++) {
	TCfg &cf = cfg.cfg(cf_el[iC]);
	if(cf.isKey()) { fltr += "("+cf_el[iC]+"="+getVal(cf)+")"; fltrN++; }
	else if(cf.view()) {
	    attrs[iA++] = (char*)cf_el[iC].c_str(); attrs[iA] = NULL;
	    setVal(cf, "");	//Clear from previous value
	}
    }
    if(fltrN > 1)	fltr = "&"+fltr;
    if(fltr.empty())	fltr = "(objectclass=*)";

    //Request
    int rez;
    LDAPMessage	*result = NULL, *entry;
    try {
	if((rez=ldap_search_s(owner().ldp,("ou="+name()+","+owner().bdn).c_str(),LDAP_SCOPE_ONE,fltr.c_str(),attrs,0,&result)) != LDAP_SUCCESS)
	    throw err_sys(_("SEARCH: %s"), ldap_err2string(rez));
	if(!ldap_count_entries(owner().ldp,result) || !(entry=ldap_first_entry(owner().ldp,result)))
	    throw err_sys(_("Entry \"%s\" is not present."), fltr.c_str());

	char *a, **vals;
	BerElement *ber;
	for(a = ldap_first_attribute(owner().ldp,entry,&ber); a != NULL; a = ldap_next_attribute(owner().ldp,entry,ber)) {
	    string attr = a;
	    ldap_memfree(a);
	    string val;
	    if((vals=ldap_get_values(owner().ldp,entry,attr.c_str())) != NULL) {
		for(int iV = 0; vals[iV] != NULL; iV++) val += vals[iV];
		ldap_value_free(vals);
	    }

	    TCfg *cf = NULL;	//cfg.at(attr, true);
	    for(unsigned iC = 0; iC < cf_el.size() && !cf; iC++)
		if(strcasecmp(attr.c_str(),cf_el[iC].c_str()) == 0) { cf = cfg.at(cf_el[iC], true); attr = cf_el[iC]; }
	    if(cf) setVal(*cf, val);
	}

	ldap_msgfree(result);
    }
    catch(TError&) {
	if(result) ldap_msgfree(result);
	throw;
    }
}

void MTable::fieldSet( TConfig &cfg )
{
    MtxAlloc resource(owner().connRes, true);
    if(!owner().enableStat())	return;
    mLstUse = SYS->sysTm();

    vector<string> cf_el;

    //Find for need entry
    // Request preparing
    cfg.cfgList(cf_el);
    char *attrs[cf_el.size()+1];
    LDAPMod mods[cf_el.size()];
    LDAPMod *mods_[cf_el.size()+1]; mods_[0] = NULL;
    string  vals[cf_el.size()];
    char *vals_[cf_el.size()][2];
    string fltr; int fltrN = 0;
    for(unsigned iC = 0, iA = 0; iC < cf_el.size(); iC++) {
	TCfg &cf = cfg.cfg(cf_el[iC]);
	if(cf.isKey()) { fltr += "("+cf_el[iC]+"="+getVal(cf)+")"; fltrN++; }
	else if(cf.view()) { attrs[iA++] = (char*)cf_el[iC].c_str(); attrs[iA] = NULL; }
    }
    if(fltrN > 1)	fltr = "&"+fltr;
    if(fltr.empty())	fltr = "(objectclass=*)";

    // Request
    int rez;
    LDAPMessage	*result = NULL, *entry;
    char *chDN = NULL;
    try {
	if((rez=ldap_search_s(owner().ldp,("ou="+name()+","+owner().bdn).c_str(),LDAP_SCOPE_ONE,fltr.c_str(),attrs,0,&result)) != LDAP_SUCCESS)
	    throw err_sys(_("SEARCH: %s"), ldap_err2string(rez));
	if(!ldap_count_entries(owner().ldp,result) || !(entry=ldap_first_entry(owner().ldp,result))) {
	    ldap_msgfree(result);
	    throw err_sys(_("Entry \"%s\" is not present."), fltr.c_str());
	}

	//Get DN of the entry
	chDN = ldap_get_dn(owner().ldp, entry);
	if(!chDN) throw err_sys(_("Get DN of the entry \"%s\" error."), fltr.c_str());

	//Check for changed attributes of the entry to replace their.
	int iA = 0;
	char *a, **tvals;
	BerElement *ber;
	for(a = ldap_first_attribute(owner().ldp,entry,&ber); a != NULL; a = ldap_next_attribute(owner().ldp,entry,ber)) {
	    string attr = a;
	    ldap_memfree(a);
	    string val;
	    if((tvals=ldap_get_values(owner().ldp,entry,attr.c_str())) != NULL) {
		for(int iV = 0; tvals[iV] != NULL; iV++) val += tvals[iV];
		ldap_value_free(tvals);
	    }

	    TCfg *cf = NULL;	//cfg.at(attr, true);
	    unsigned iC = 0;
	    for( ; iC < cf_el.size(); iC++)
		if(strcasecmp(attr.c_str(),cf_el[iC].c_str()) == 0) { cf = cfg.at(cf_el[iC], true); attr = cf_el[iC]; break; }
	    if(cf && (vals[iA]=getVal(*cf)) != val) {
		vals_[iA][0] = (char*)vals[iA].c_str(); vals_[iA][1] = NULL;
		mods[iA].mod_op = LDAP_MOD_REPLACE;
		mods[iA].mod_type = (char*)cf_el[iC].c_str();
		mods[iA].mod_values = vals_[iA];
		mods_[iA] = &mods[iA]; mods_[++iA] = NULL;
	    }
	}

	//Call the generic modify request
	if((rez=ldap_modify_s(owner().ldp,chDN,mods_)) != LDAP_SUCCESS)
	    throw err_sys(_("MODIFY: %s"), ldap_err2string(rez));

	ldap_memfree(chDN);
	ldap_msgfree(result);
    }
    catch(TError&) {
	if(chDN)	ldap_memfree(chDN);
	if(result)	ldap_msgfree(result);
	throw;
    }
}

/*void MTable::fieldDel( TConfig &cfg )
{

}*/

string MTable::getVal( TCfg &cf )
{
    return Mess->codeConvOut(codepage, cf.getS());
}

void MTable::setVal( TCfg &cf, const string &val )
{
    cf.setS(Mess->codeConvIn(codepage,val));
}
