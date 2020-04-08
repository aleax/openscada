
//OpenSCADA module Special.SystemTests file: test_DB.h
/***************************************************************************
 *   Copyright (C) 2005-2018 by Roman Savochenko, <rom_as@oscada.org>      *
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

#ifndef TEST_DB_H
#define TEST_DB_H

#include <tfunction.h>

namespace KernelTest
{

//*************************************************
//* TestDB: Full database test.                   *
//*************************************************
class TestDB : public TFunction
{
    public:
	TestDB( ) : TFunction("DB",SSPC_ID) {
	    ioAdd(new IO("rez",_("Result"),IO::String,IO::Return));
	    ioAdd(new IO("type",_("DB type"),IO::String,IO::Default,"SQLite"));
	    ioAdd(new IO("addr",_("DB address"),IO::String,IO::Default,"./DATA/test.db"));
	    ioAdd(new IO("table",_("DB table"),IO::String,IO::Default,"test"));
	    ioAdd(new IO("size",_("Records number"),IO::Integer,IO::Default,"1000"));
	}

	string name( )	{ return _("DB test"); }
	string descr( )	{ return _("Complete database test. Performs:\n"
				   "  - create/open DB;\n"
				   "  - create/open table;\n"
				   "  - create multiple records for the determined structure;\n"
				   "  - update multiple records;\n"
				   "  - update multiple records by a request key;\n"
				   "  - get and check values for multiple records;\n"
				   "  - seek records;\n"
				   "  - seek records in preload;\n"
				   "  - modify a record and the table structure;\n"
				   "  - remove multiple records;\n"
				   "  - close/remove table;\n"
				   "  - close/remove DB."); }

	void calc( TValFunc *val ) {
	    try {
		mod->mess(id(),_("Test: Start"));

		//Get test param
		string t_bd = val->getS(1);
		string n_bd = "test_bd";
		string bd_addr = val->getS(2);
		string n_tbl = val->getS(3);
		int experem = val->getI(4);

		AutoHD<TTypeBD> bd = SYS->db().at().modAt(t_bd);

		mod->mess(id(),_("Open the DB '%s'."),n_bd.c_str());
		bd.at().open(n_bd);
		bd.at().at(n_bd).at().setAddr(bd_addr);
		bd.at().at(n_bd).at().enable();

		mod->mess(id(),_("Open the table '%s'."),n_tbl.c_str());
		bd.at().at(n_bd).at().open(n_tbl,true);
		mod->mess(id(),_("Connect to the table '%s'."),n_tbl.c_str());
		AutoHD<TTable> tbl = bd.at().at(n_bd).at().at(n_tbl);

		mod->mess(id(),_("Create a DB configuration."));
		TConfig bd_cfg;
		bd_cfg.elem().fldAdd(new TFld("name",_("Name"),TFld::String,TCfg::Key,OBJ_ID_SZ));
		bd_cfg.elem().fldAdd(new TFld("descr",_("Description"),TFld::String,0,OBJ_NM_SZ));
		bd_cfg.elem().fldAdd(new TFld("val",_("Value"),TFld::Real,0,"10.2","5"));
		bd_cfg.elem().fldAdd(new TFld("id",_("Identifier"),TFld::Integer,0,"7","34"));
		bd_cfg.elem().fldAdd(new TFld("stat",_("State"),TFld::Boolean,0,"1","1"));
		bd_cfg.elem().fldAdd(new TFld("reqKey",_("Request's key"),TFld::String,0,"100"));
		bd_cfg.elem().fldAdd(new TFld("reqVal",_("Request's value"),TFld::String,0,"100"));

		//Test for create records
		mod->mess(id(),_("Create records."));
		int64_t ctime = TSYS::curTime();
		for(int i_fld = 0; i_fld < experem; i_fld++) {
		    bd_cfg.cfg("name").setS("Sh"+i2s(i_fld));
		    bd_cfg.cfg("descr").setS("Shifr '"+i2s(i_fld)+"'");
		    bd_cfg.cfg("val").setR(sqrt(i_fld));
		    bd_cfg.cfg("id").setI(i_fld);
		    bd_cfg.cfg("stat").setB((i_fld%2)==0?true:false);
		    bd_cfg.cfg("reqKey").setS(i_fld<(experem/2)?"req0":"req1");
		    bd_cfg.cfg("reqVal").setS(i_fld<(experem/2)?"Request 0 value":"Request 1 value");
		    tbl.at().fieldSet(bd_cfg);
		}
		mod->mess(id(),_("Created %d records for time %f sec."),experem,1e-6*(TSYS::curTime()-ctime));

		//Check for update fields
		mod->mess(id(),_("Update records."));
		ctime = TSYS::curTime();
		for(int i_fld = 0; i_fld < experem; i_fld++) {
		    bd_cfg.cfg("name").setS("Sh"+i2s(i_fld));
		    bd_cfg.cfg("descr").setS("New shifr \""+i2s(i_fld)+"\"");
		    bd_cfg.cfg("val").setR(2.*sqrt(i_fld));
		    bd_cfg.cfg("id").setI(2*i_fld);
		    bd_cfg.cfg("stat").setB((i_fld%2)==0?false:true);
		    bd_cfg.cfg("reqKey").setView(false);
		    bd_cfg.cfg("reqVal").setView(false);
		    tbl.at().fieldSet(bd_cfg);
		}
		mod->mess(id(),_("Updated %d records for time %f sec."),experem,1e-6*(TSYS::curTime()-ctime));

		//Check for update fields, by request key
		mod->mess(id(),_("Update records by a request key for %d records."), experem/2);
		ctime = TSYS::curTime();
		bd_cfg.cfgViewAll(false);
		bd_cfg.cfg("reqKey").setReqKey(true);
		bd_cfg.cfg("reqKey").setS("req0",TCfg::ForceUse|TCfg::ExtValTwo);
		bd_cfg.cfg("reqKey").setS("reqNew0",TCfg::ForceUse);
		bd_cfg.cfg("reqVal").setS("Request new 0 value",TCfg::ForceUse);
		tbl.at().fieldSet(bd_cfg);
		mod->mess(id(),_("Updated %d records by a request key for time %f sec."),experem/2,1e-6*(TSYS::curTime()-ctime));
		bd_cfg.cfg("reqKey").setReqKey(false);
		bd_cfg.cfg("reqKey").setExtVal(false);
		bd_cfg.cfgViewAll(true);

		//Check for get fields
		mod->mess(id(),_("Check records."));
		ctime = TSYS::curTime();
		for(int i_fld = 0; i_fld < experem; i_fld++) {
		    bd_cfg.cfg("name").setS("Sh"+i2s(i_fld));
		    tbl.at().fieldGet(bd_cfg);

		    if(experem > 155 && i_fld == 155)
			mod->mess(id(),_("Record #155='%s'; Descr='%s'; Value=%f; Id=%d; Stat=%d."),//; reqKey='%s'; reqVal='%s'."),
			    bd_cfg.cfg("name").getS().c_str(), bd_cfg.cfg("descr").getS().c_str(),
			    bd_cfg.cfg("val").getR(), bd_cfg.cfg("id").getI(), bd_cfg.cfg("stat").getB());/*,
			    bd_cfg.cfg("reqKey").getS().c_str(), bd_cfg.cfg("reqVal").getS().c_str());*/	//By some GLibC crashes

		    if(bd_cfg.cfg("name").getS() != (string("Sh")+i2s(i_fld)))
			throw TError(nodePath().c_str(),_("Field <Sh> '%s' != '%s' error."),
			    bd_cfg.cfg("name").getS().c_str(),(string("Sh")+i2s(i_fld)).c_str());
		    if(bd_cfg.cfg("descr").getS() != (string("New shifr \"")+i2s(i_fld)+"\""))
			throw TError(nodePath().c_str(),_("Field <descr> '%s' != '%s' error."),
			    bd_cfg.cfg("descr").getS().c_str(),(string("New shifr ")+i2s(i_fld)).c_str());
		    //ceil(100.*bd_cfg.cfg("val").getR()) != ceil(2.*sqrt(i_fld)) ||
		    if(bd_cfg.cfg("id").getI() != (2*i_fld))
			throw TError(nodePath().c_str(),_("Field <id> %d != %d error."),bd_cfg.cfg("id").getI(), (2*i_fld) );
		    if(bd_cfg.cfg("stat").getB() != ((i_fld%2)==0?false:true))
			throw TError(nodePath().c_str(),_("Field <stat> %d != %d error."),bd_cfg.cfg("stat").getB(),((i_fld%2)==0?false:true));
		    if(bd_cfg.cfg("reqKey").getS() != ((i_fld<(experem/2))?"reqNew0":"req1"))
			throw TError(nodePath().c_str(), _("Field <reqKey> '%s' != '%s' error."),
			    bd_cfg.cfg("reqKey").getS().c_str(), ((i_fld<(experem/2))?"reqNew0":"req1"));
		    if(bd_cfg.cfg("reqVal").getS() != ((i_fld<(experem/2))?"Request new 0 value":"Request 1 value"))
			throw TError(nodePath().c_str(), _("Field <reqVal> '%s' != '%s' error."),
			    bd_cfg.cfg("reqVal").getS().c_str(), ((i_fld<(experem/2))?"Request new 0 value":"Request 1 value"));
		}
		mod->mess(id(),_("Got %d records for time %f sec."),experem,1e-6*(TSYS::curTime()-ctime));

		//Seek all records
		mod->mess(id(), _("Seek records."));
		ctime = TSYS::curTime();
		bd_cfg.cfg("name").setKeyUse(false);
		int pos = 0;
		while(tbl.at().fieldSeek(pos,bd_cfg)) pos++;
		mod->mess(id(), _("Sought %d records for time %f sec."), pos, 1e-6*(TSYS::curTime()-ctime));

		//Seek in preload all records
		mod->mess(id(), _("Seek records in preload."));
		ctime = TSYS::curTime();
		vector< vector<string> > full;
		pos = 0;
		while(tbl.at().fieldSeek(pos,bd_cfg,&full)) pos++;
		bd_cfg.cfg("name").setKeyUse(true);
		mod->mess(id(), _("Sought %d records in preload for time %f sec."), pos, 1e-6*(TSYS::curTime()-ctime));

		//Check for fix structure
		mod->mess(id(),_("Change DB structure."));
		//Add column
		bd_cfg.elem().fldAdd(new TFld("fix",_("DB fix test"),TFld::String,0,"20"));
		bd_cfg.cfg("name").setS("Sh1");
		tbl.at().fieldSet(bd_cfg);
		bd_cfg.cfg("name").setS("Sh2");
		tbl.at().fieldGet(bd_cfg);
		if(bd_cfg.cfg("name").getS() != "Sh2" || bd_cfg.cfg("descr").getS() != "New shifr \"2\"" ||
		    bd_cfg.cfg("id").getI() != 4 || bd_cfg.cfg("stat").getB() != false)
		{
		    mod->mess(id(),_("Record #2='%s'; Descr='%s'; Value=%f; Id=%d; Stat=%d."),
			bd_cfg.cfg("name").getS().c_str(), bd_cfg.cfg("descr").getS().c_str(),
			bd_cfg.cfg("val").getR(), bd_cfg.cfg("id").getI(), bd_cfg.cfg("stat").getB());
		    throw TError(nodePath().c_str(),_("Error adding a column."));
		}
		else mod->mess(id(),_("OK adding a column."));

		//Del column
		bd_cfg.elem().fldDel(bd_cfg.elem().fldId("fix"));
		bd_cfg.cfg("name").setS("Sh1");
		tbl.at().fieldSet(bd_cfg);
		bd_cfg.cfg("name").setS("Sh2");
		tbl.at().fieldGet(bd_cfg);
		if(bd_cfg.cfg("name").getS() != "Sh2" || bd_cfg.cfg("descr").getS() != "New shifr \"2\"" ||
		    bd_cfg.cfg("id").getI() != 4 || bd_cfg.cfg("stat").getB() != false)
		{
		    mod->mess(id(),"Record #2='%s'; Descr='%s'; Value=%f; Id=%d; Stat=%d.",
			bd_cfg.cfg("name").getS().c_str(), bd_cfg.cfg("descr").getS().c_str(),
			bd_cfg.cfg("val").getR(), bd_cfg.cfg("id").getI(), bd_cfg.cfg("stat").getB() );
		    throw TError(nodePath().c_str(),_("Error deleting a column."));
		}
		else mod->mess(id(),_("OK deleting a column."));

		//Check List
		//vector<string> ls_elem;
		//tbl.at().fieldList("name",ls_elem);
		//if(ls_elem.size() != experem) mod->mess(id(),_("Error list size!"));

		//Delete fields
		ctime = TSYS::curTime();
		for(int i_fld = 0; i_fld < experem; i_fld++) {
		    bd_cfg.cfg("name").setS("Sh"+i2s(i_fld),true);
		    tbl.at().fieldDel(bd_cfg);
		}
		mod->mess(id(),_("Deleted %d records for time %f sec."),experem,1e-6*(TSYS::curTime()-ctime));

		tbl.free();

		mod->mess(id(),_("Close and delete the table '%s'."),n_tbl.c_str());
		bd.at().at(n_bd).at().close(n_tbl,true);

		mod->mess(id(),_("Close and delete the DB '%s'."),n_bd.c_str());
		bd.at().close(n_bd,true);

		bd.free();

		mod->mess(id(),_("Test: Passed"));
		val->setS(0,_("Passed"));
	    }
	    catch(TError err) {
		mod->mess(id(),_("Test: Failed: %s"),err.mess.c_str());
		val->setS(0,TSYS::strMess(_("Failed: %s"),err.mess.c_str()));
	    }
	}
};

}

#endif //TEST_DB_H
