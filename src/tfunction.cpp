
//OpenSCADA system file: tfunction.cpp
/***************************************************************************
 *   Copyright (C) 2003-2009 by Roman Savochenko                           *
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

#include <math.h>

#include <tsys.h>
#include <tmess.h>
#include "tfunction.h"

using namespace OSCADA;

//*************************************************
//* Function abstract object                      *
//*************************************************
TFunction::TFunction( const string &iid ) : mId(iid), mTVal(NULL), run_st(false)
{

}

TFunction::~TFunction( )
{
    for( int i_io = 0; i_io < mIO.size(); i_io++ )
	delete mIO[i_io];
}

TFunction &TFunction::operator=( TFunction &func )
{
    //> Copy IO
    //>> Clear no present IO
    for( int i_io = 0; i_io < ioSize(); )
	if( func.ioId(io(i_io)->id()) < 0 )	ioDel(i_io);
	else i_io++;
    //>> Update present and create new IO
    for( int i_io = 0; i_io < func.ioSize(); i_io++ )
    {
	int dst_io = ioId(func.io(i_io)->id());
	if( dst_io < 0 )
	    dst_io = ioIns( new IO( func.io(i_io)->id().c_str(), func.io(i_io)->name().c_str(), func.io(i_io)->type(), func.io(i_io)->flg(),
		func.io(i_io)->def().c_str(), func.io(i_io)->hide(), func.io(i_io)->rez().c_str() ), i_io );
	else *io(dst_io) = *func.io(i_io);
	if( dst_io != i_io && !use() ) ioMove(dst_io,i_io);
    }

    if( mId.empty() )	mId = func.id();

    return *this;
}

void TFunction::preDisable( int flag )
{
    if( mTVal ) { delete mTVal; mTVal = NULL; }
    if( used.size() )
    {
	string mess;
	for( int i=0; i < used.size(); i++ )
	    mess+=used[i]->vfName()+", ";
	throw TError(nodePath().c_str(),_("Function is used by: %s"),mess.c_str());
    }
}

void TFunction::setId( const string &vl )
{
    if( !nodePrev(true) ) mId = vl;
}

int TFunction::ioSize( )
{
    return mIO.size( );
}

IO *TFunction::io( int iid )
{
    if( iid >= mIO.size() ) throw TError(nodePath().c_str(),_("Index %d is broken!"),iid);
    return mIO[iid];
}

int TFunction::ioId( const string &id )
{
    for( int i_io = 0; i_io < mIO.size(); i_io++ )
	if( mIO[i_io]->id() == id ) return i_io;
    return -1;
}

void TFunction::ioList( vector<string> &list )
{
    for( int i_io = 0; i_io < mIO.size(); i_io++ )
	list.push_back( mIO[i_io]->id() );
}

void TFunction::ioAdd( IO *io )
{
    preIOCfgChange();
    mIO.push_back(io);
    io->owner = this;
    postIOCfgChange();
}

int TFunction::ioIns( IO *io, int pos )
{
    if( pos < 0 || pos > mIO.size() ) pos = mIO.size();

    preIOCfgChange();
    mIO.insert(mIO.begin()+pos,io);
    io->owner = this;
    postIOCfgChange();

    return pos;
}

void TFunction::ioDel( int pos )
{
    if( pos < 0 || pos >= mIO.size() )
        throw TError(nodePath().c_str(),_("Delete IO <%d> error."),pos);

    preIOCfgChange();
    mIO.erase(mIO.begin()+pos);
    postIOCfgChange();
}

void TFunction::ioMove( int pos, int to )
{
    if( pos < 0 || pos >= mIO.size() || to < 0 || to >= mIO.size() )
	throw TError(nodePath().c_str(),_("Move IO from %d to %d error."),pos,to);

    preIOCfgChange();
    IO *io = mIO[to];
    mIO[to] = mIO[pos];
    mIO[pos] = io;
    postIOCfgChange();
}

void TFunction::preIOCfgChange()
{
    string blk_lst;
    for(unsigned i=0; i < used.size(); i++)
	if( used[i]->blk() )	blk_lst+=used[i]->vfName()+",";
    if( blk_lst.size() )
	throw TError(nodePath().c_str(),_("Change is not permited while function is used: %s"),blk_lst.c_str());

    for( unsigned i=0; i < used.size(); i++ )
	used[i]->preIOCfgChange();
}

void TFunction::postIOCfgChange()
{
    for(unsigned i=0; i < used.size(); i++)
        used[i]->postIOCfgChange();
}

void TFunction::valAtt( TValFunc *vfnc )
{
    ResAlloc res(f_res,true);
    for( unsigned i=0; i < used.size(); i++ )
	if( used[i] == vfnc )
	    throw TError(nodePath().c_str(),_("Value <%s> is already attached!"),vfnc->vfName().c_str());
    used.push_back(vfnc);
}

void TFunction::valDet( TValFunc *vfnc )
{
    f_res.resRequestW();
    for(unsigned i=0; i < used.size() ;i++)
	if(used[i] == vfnc)
	{ used.erase(used.begin()+i); break; }
    f_res.resRelease();
}

void TFunction::cntrCmdProc( XMLNode *opt )
{
    //> Get page info
    if( opt->name() == "info" )
    {
	TCntrNode::cntrCmdProc(opt);
	ctrMkNode("oscada_cntr",opt,-1,"/",_("Function: ")+name(),0444,"root","root");
	if(ctrMkNode("area",opt,-1,"/func",_("Function")))
	{
	    if(ctrMkNode("area",opt,-1,"/func/st",_("State")))
	    {
		ctrMkNode("fld",opt,-1,"/func/st/st",_("Accessing"),0664,"root","root",1,"tp","bool");
		ctrMkNode("fld",opt,-1,"/func/st/use",_("Used"),R_R_R_,"root","root",1,"tp","dec");
	    }
	    if(ctrMkNode("area",opt,-1,"/func/cfg",_("Config")))
	    {
		ctrMkNode("fld",opt,-1,"/func/cfg/id",_("Id"),0444,"root","root",1,"tp","str");
		ctrMkNode("fld",opt,-1,"/func/cfg/name",_("Name"),0444,"root","root",1,"tp","str");
		ctrMkNode("fld",opt,-1,"/func/cfg/descr",_("Description"),0444,"root","root",3,"tp","str","cols","90","rows","5");
	    }
	}
	if(ctrMkNode("area",opt,-1,"/io",_("IO")))
	    if(ctrMkNode("table",opt,-1,"/io/io",_("IO"),0440,"root","root"))
	    {
		ctrMkNode("list",opt,-1,"/io/io/0",_("Id"),0444,"root","root",1,"tp","str");
		ctrMkNode("list",opt,-1,"/io/io/1",_("Name"),0444,"root","root",1,"tp","str");
		ctrMkNode("list",opt,-1,"/io/io/2",_("Type"),0444,"root","root",1,"tp","str");
		ctrMkNode("list",opt,-1,"/io/io/3",_("Mode"),0444,"root","root",1,"tp","str");
		ctrMkNode("list",opt,-1,"/io/io/4",_("Hide"),0444,"root","root",1,"tp","bool");
		ctrMkNode("list",opt,-1,"/io/io/5",_("Default"),0444,"root","root",1,"tp","str");
	    }
	if(ctrMkNode("area",opt,-1,"/exec",_("Execute")))
	{
	    ctrMkNode("fld",opt,-1,"/exec/en",_("Enable"),0660,"root","root",1,"tp","bool");
	    //>> Add test form
	    if( mTVal )
	    {
		if(ctrMkNode("area",opt,-1,"/exec/io",_("IO")))
		    for( int i_io = 0; i_io < ioSize(); i_io++ )
		    {
			if( mIO[i_io]->hide() ) continue;
			XMLNode *nd = ctrMkNode("fld",opt,-1,("/exec/io/"+io(i_io)->id()).c_str(),io(i_io)->name(),0664,"root","root");
			if( nd )
			{
			    switch(io(i_io)->type())
			    {
				case IO::String:
				    nd->setAttr("tp","str");
				    if( io(i_io)->flg( )&IO::FullText ) nd->setAttr("cols","100")->setAttr("rows","4");
				    break;
				case IO::Integer:	nd->setAttr("tp","dec");	break;
				case IO::Real:		nd->setAttr("tp","real");	break;
				case IO::Boolean:	nd->setAttr("tp","bool");	break;
				case IO::Object:	nd->setAttr("tp","str")->setAttr("cols","100")->setAttr("rows","4");	break;
			    }
			}
		    }
		//>> Add Calc button and Calc time
		ctrMkNode("fld",opt,-1,"/exec/n_clc",_("Number runs"),0664,"root","root",1,"tp","dec");
		ctrMkNode("fld",opt,-1,"/exec/tm",_("Time for execute (mks)"),0444,"root","root",1,"tp","real");
		ctrMkNode("comm",opt,-1,"/exec/calc",_("Execute"),0666);
	    }
	}
        return;
    }

    //> Process command to page
    string a_path = opt->attr("path");
    if( a_path == "/func/st/st" )
    {
	if( ctrChkNode(opt,"get",0664,"root","root",SEC_RD) )	opt->setText(run_st?"1":"0");
	if( ctrChkNode(opt,"set",0664,"root","root",SEC_WR) )	setStart(atoi(opt->text().c_str()));
    }
    else if( a_path == "/func/st/use" && ctrChkNode(opt) )	{ ResAlloc res(f_res,false); opt->setText(TSYS::int2str(used.size())); }
    else if( a_path == "/func/cfg/id" && ctrChkNode(opt) )	opt->setText(id());
    else if( a_path == "/func/cfg/name" && ctrChkNode(opt) )	opt->setText(name());
    else if( a_path == "/func/cfg/descr" && ctrChkNode(opt) )	opt->setText(descr());
    else if( a_path == "/io/io" && ctrChkNode(opt,"get",0440,"root","root",SEC_RD) )
    {
	XMLNode *n_id	= ctrMkNode("list",opt,-1,"/io/io/0","");
	XMLNode *n_nm	= ctrMkNode("list",opt,-1,"/io/io/1","");
	XMLNode *n_type	= ctrMkNode("list",opt,-1,"/io/io/2","");
	XMLNode *n_mode	= ctrMkNode("list",opt,-1,"/io/io/3","");
	XMLNode *n_hide	= ctrMkNode("list",opt,-1,"/io/io/4","");
	XMLNode *n_def	= ctrMkNode("list",opt,-1,"/io/io/5","");
	//XMLNode *n_vect	= ctrId(opt,"6");
	for( int i_io = 0; i_io < ioSize(); i_io++ )
	{
	    string tmp_str;
	    if(n_id)	n_id->childAdd("el")->setText(io(i_io)->id());
	    if(n_nm)	n_nm->childAdd("el")->setText(io(i_io)->name());
	    if(n_type)
	    {
		switch(io(i_io)->type())
		{
		    case IO::String:	tmp_str = _("String");	break;
		    case IO::Integer:	tmp_str = _("Integer");	break;
		    case IO::Real:	tmp_str = _("Real");	break;
		    case IO::Boolean:	tmp_str = _("Bool");	break;
		    case IO::Object:	tmp_str = _("Object");	break;
		}
		n_type->childAdd("el")->setText(tmp_str);
	    }
	    if(n_mode)
	    {
		if(io(i_io)->flg()&IO::Return)		tmp_str = _("Return");
		else if(io(i_io)->flg()&IO::Output)	tmp_str = _("Output");
		else					tmp_str = _("Input");
		n_mode->childAdd("el")->setText(tmp_str);
	    }
	    if(n_hide)	n_hide->childAdd("el")->setText(io(i_io)->hide()?"1":"0");
	    if(n_def)	n_def->childAdd("el")->setText(io(i_io)->def());
	}
    }
    else if( a_path == "/exec/en" )
    {
	if( ctrChkNode(opt,"get",0660,"root","root",SEC_RD) )	opt->setText(mTVal?"1":"0");
	if( ctrChkNode(opt,"set",0660,"root","root",SEC_WR) )
	{
	    bool to_en_exec = atoi(opt->text().c_str());
	    if( to_en_exec && !mTVal )	{ mTVal = new TValFunc(id()+"_exec",this); mTVal->setDimens(true); }
	    if( !to_en_exec && mTVal )	{ delete mTVal; mTVal = NULL; }
	}
    }
    else if( a_path == "/exec/n_clc" && mTVal )
    {
	if( ctrChkNode(opt,"get",0664,"root","root",SEC_RD) )	opt->setText(TBDS::genDBGet(nodePath()+"ntCalc","10",opt->attr("user")));
	if( ctrChkNode(opt,"set",0664,"root","root",SEC_WR) )	TBDS::genDBSet(nodePath()+"ntCalc",opt->text(),opt->attr("user"));
    }
    else if( a_path == "/exec/tm" && mTVal && ctrChkNode(opt) )opt->setText(TSYS::real2str(mTVal->calcTm(),6));
    else if( a_path.substr(0,8) == "/exec/io" && mTVal )
    {
	string io_id = TSYS::pathLev(a_path,2);
	for( int i_io = 0; i_io < mIO.size(); i_io++ )
	    if( io_id == io(i_io)->id() )
	    {
		if( ctrChkNode(opt,"get",0664,"root","root",SEC_RD) )
		    opt->setText( (mTVal->ioType(i_io)==IO::Real) ? TSYS::real2str(mTVal->getR(i_io),6) : mTVal->getS(i_io) );
		if( ctrChkNode(opt,"set",0664,"root","root",SEC_WR) )
		    mTVal->setS(i_io,opt->text());
		break;
	    }
    }
    else if( a_path == "/exec/calc" && mTVal && ctrChkNode(opt,"set",0666,"root","root",SEC_WR) )
    {
	double c_rez = 0;
	int n_tcalc = atoi(TBDS::genDBGet(nodePath()+"ntCalc","10",opt->attr("user")).c_str());
	for(int i_c = 0; i_c < n_tcalc; i_c++ )
	{
	    mTVal->calc(opt->attr("user"));
	    c_rez += mTVal->calcTm();
	}
	mTVal->setCalcTm(c_rez);
    }
    else TCntrNode::cntrCmdProc(opt);
}


//*************************************************
//* IO                                            *
//*************************************************
IO::IO( const char *iid, const char *iname, IO::Type itype,  unsigned iflgs, const char *idef, bool ihide, const char *irez )
{
    mId		= iid;
    mName	= iname;
    mType	= itype;
    mFlg	= iflgs;
    mHide	= ihide;
    mDef	= idef;
    mRez	= irez;
}

IO &IO::operator=(IO &iio)
{
    setId(iio.id());
    setName(iio.name());
    setType(iio.type());
    setFlg(iio.flg());
    setDef(iio.def());
    setHide(iio.hide());
    setRez(iio.rez());

    return *this;
}

void IO::setId( const string &val )
{
    if(mId==val) return;
    owner->preIOCfgChange();
    mId = val;
    owner->postIOCfgChange();
}

void IO::setName( const string &val )
{
    mName = val;
}

void IO::setType( Type val )
{
    if(mType==val) return;
    owner->preIOCfgChange();
    mType = val;
    owner->postIOCfgChange();
}

void IO::setFlg( unsigned val )
{
    if(mFlg==val) return;
    owner->preIOCfgChange();
    mFlg = val;
    owner->postIOCfgChange();
}

void IO::setDef( const string &val )
{
    if(mDef==val) return;
    //owner->preIOCfgChange();
    mDef = val;
    //owner->postIOCfgChange();
}

void IO::setHide( bool val )
{
    if(mHide==val) return;
    //owner->preIOCfgChange();
    mHide = val;
    //owner->postIOCfgChange();
}

void IO::setRez( const string &val )
{
    if(mRez==val) return;
    mRez = val;
}

//*************************************************
//* TValFunc                                      *
//*************************************************
TValFunc::TValFunc( const string &iname, TFunction *ifunc, bool iblk, const string &iuser ) :
    mName(iname), mFunc(NULL), mDimens(false), tm_calc(0.0), mBlk(iblk), mUser(iuser)
{
    setFunc(ifunc);
}

TValFunc::~TValFunc( )
{
    if( mFunc ) funcDisConnect();
}

void TValFunc::setFunc( TFunction *ifunc, bool att_det )
{
    if( mFunc )	funcDisConnect(att_det);
    if( ifunc )
    {
	mFunc = ifunc;
	if( att_det )
	{
	    mFunc->AHDConnect();
	    mFunc->valAtt(this);
	}
	for( int i_vl = 0; i_vl < mFunc->ioSize(); i_vl++ )
	{
	    SVl val;
	    val.tp = mFunc->io(i_vl)->type();
	    val.mdf = false;
	    switch( val.tp )
	    {
		case IO::String:	val.val.s = new ResString(mFunc->io(i_vl)->def());	break;
		case IO::Integer:	val.val.i = atoi(mFunc->io(i_vl)->def().c_str());	break;
		case IO::Real:		val.val.r = atof(mFunc->io(i_vl)->def().c_str());	break;
		case IO::Boolean:	val.val.b = atoi(mFunc->io(i_vl)->def().c_str());	break;
		case IO::Object:	val.val.o = new TVarObj(); val.val.o->connect();	break;
	    }
	    mVal.push_back(val);
	}
    }
}

void TValFunc::funcDisConnect( bool det )
{
    ctxClear();

    if(mFunc)
    {
	for(int i_vl = 0; i_vl < mVal.size(); i_vl++)
	    switch( mVal[i_vl].tp )
	    {
		case IO::String:	delete mVal[i_vl].val.s;	break;
		case IO::Object:
		    if( mVal[i_vl].val.o && !mVal[i_vl].val.o->disconnect() ) delete mVal[i_vl].val.o;
		    break;
	    }
	mVal.clear();
	if(det)
	{
	    mFunc->valDet(this);
	    mFunc->AHDDisConnect();
	    mFunc = NULL;
	}
    }
}

int TValFunc::ioId( const string &iid )
{
    if( !mFunc )	throw TError("ValFnc",_("IO <%s> is not present!"),iid.c_str());
    return mFunc->ioId(iid);
}

void TValFunc::ioList( vector<string> &list )
{
    if( !mFunc )       throw TError("ValFnc",_("Function is not attached!"));
    return mFunc->ioList(list);
}

int TValFunc::ioSize( )
{
    if( !mFunc )       throw TError("ValFnc",_("Function is not attached!"));
    return mFunc->ioSize();
}

string TValFunc::getS( unsigned id )
{
    if( id >= mVal.size() )    throw TError("ValFnc",_("%s: Id or IO %d error!"),"getS()",id);
    switch(mVal[id].tp)
    {
	case IO::Integer:	return (mVal[id].val.i!=EVAL_INT) ? TSYS::int2str(mVal[id].val.i) : EVAL_STR;
	case IO::Real:		return (mVal[id].val.r!=EVAL_REAL) ? TSYS::real2str(mVal[id].val.r) : EVAL_STR;
	case IO::Boolean:	return (mVal[id].val.b!=EVAL_BOOL) ? TSYS::int2str((bool)mVal[id].val.b) : EVAL_STR;
	case IO::String:	return mVal[id].val.s->getVal();
	case IO::Object:	return mVal[id].val.o->getStrXML();
    }
    return EVAL_STR;
}

int TValFunc::getI( unsigned id )
{
    if( id >= mVal.size() )    throw TError("ValFnc",_("%s: Id or IO %d error!"),"getI()",id);
    switch( mVal[id].tp )
    {
	case IO::String:	return (mVal[id].val.s->getVal()!=EVAL_STR) ? atoi(mVal[id].val.s->getVal().c_str()) : EVAL_INT;
	case IO::Real:		return (mVal[id].val.r!=EVAL_REAL) ? (int)mVal[id].val.r : EVAL_INT;
	case IO::Boolean:	return (mVal[id].val.b!=EVAL_BOOL) ? (bool)mVal[id].val.b : EVAL_INT;
	case IO::Integer:	return mVal[id].val.i;
    }
    return EVAL_INT;
}

double TValFunc::getR( unsigned id )
{
    if( id >= mVal.size() )    throw TError("ValFnc",_("%s: Id or IO %d error!"),"getR()",id);
    switch( mVal[id].tp )
    {
	case IO::String:	return (mVal[id].val.s->getVal()!=EVAL_STR) ? atof(mVal[id].val.s->getVal().c_str()) : EVAL_REAL;
	case IO::Integer:	return (mVal[id].val.i!=EVAL_INT) ? mVal[id].val.i : EVAL_REAL;
	case IO::Boolean:	return (mVal[id].val.b!=EVAL_BOOL) ? (bool)mVal[id].val.b : EVAL_REAL;
	case IO::Real:		return mVal[id].val.r;
    }
    return EVAL_REAL;
}

char TValFunc::getB( unsigned id )
{
    if( id >= mVal.size() )	throw TError("ValFnc",_("%s: Id or IO %d error!"),"getB()",id);
    switch( mVal[id].tp )
    {
	case IO::String:	return (mVal[id].val.s->getVal()!=EVAL_STR) ? (bool)atoi(mVal[id].val.s->getVal().c_str()) : EVAL_BOOL;
	case IO::Integer:	return (mVal[id].val.i!=EVAL_INT) ? (bool)mVal[id].val.i : EVAL_BOOL;
	case IO::Real:		return (mVal[id].val.r!=EVAL_REAL) ? (bool)mVal[id].val.r : EVAL_BOOL;
	case IO::Boolean:	return mVal[id].val.b;
    }
    return EVAL_BOOL;
}

TVarObj *TValFunc::getO( unsigned id )
{
    if( id >= mVal.size() )	throw TError("ValFnc",_("%s: Id or IO %d error!"),"getO()",id);
    if( mVal[id].tp != IO::Object )	throw TError("ValFnc",_("Get object from not object's IO %d error!"),id);
    return mVal[id].val.o;
}

void TValFunc::setS( unsigned id, const string &val )
{
    if( id >= mVal.size() )	throw TError("ValFnc",_("%s: Id or IO %d error!"),"setS()",id);
    if( mdfChk() && val != getS(id) ) mVal[id].mdf = true;
    switch( mVal[id].tp )
    {
	case IO::Integer:	mVal[id].val.i = (val!=EVAL_STR) ? atoi(val.c_str()) : EVAL_INT;	break;
	case IO::Real:		mVal[id].val.r = (val!=EVAL_STR) ? atof(val.c_str()) : EVAL_REAL;	break;
	case IO::Boolean:	mVal[id].val.b = (val!=EVAL_STR) ? (bool)atoi(val.c_str()) : EVAL_BOOL;	break;
	case IO::String:	mVal[id].val.s->setVal(val);	break;
    }
}

void TValFunc::setI( unsigned id, int val )
{
    if( id >= mVal.size() )    throw TError("ValFnc",_("%s: Id or IO %d error!"),"setI()",id);
    if( mdfChk() && val != getI(id) ) mVal[id].mdf = true;
    switch( mVal[id].tp )
    {
	case IO::String:	mVal[id].val.s->setVal( (val!=EVAL_INT) ? TSYS::int2str(val) : EVAL_STR );	break;
	case IO::Real:		mVal[id].val.r = (val!=EVAL_INT) ? val : EVAL_REAL;		break;
	case IO::Boolean:	mVal[id].val.b = (val!=EVAL_INT) ? (bool)val : EVAL_BOOL;	break;
	case IO::Integer:	mVal[id].val.i = val;	break;
    }
}

void TValFunc::setR( unsigned id, double val )
{
    if( id >= mVal.size() )    throw TError("ValFnc",_("%s: Id or IO %d error!"),"setR()",id);
    if( isnan(val) ) val = 0;	//Check for 'Not a Number'
    if( mdfChk() && val != getR(id) ) mVal[id].mdf = true;
    switch( mVal[id].tp )
    {
	case IO::String:	mVal[id].val.s->setVal( (val!=EVAL_REAL) ? TSYS::real2str(val) : EVAL_STR );	break;
	case IO::Integer:	mVal[id].val.i = (val!=EVAL_REAL) ? (int)val : EVAL_INT;	break;
	case IO::Boolean:	mVal[id].val.b = (val!=EVAL_REAL) ? (bool)val : EVAL_BOOL;	break;
	case IO::Real:		mVal[id].val.r = val;	break;
    }
}

void TValFunc::setB( unsigned id, char val )
{
    if( id >= mVal.size() )	throw TError("ValFnc",_("%s: Id or IO %d error!"),"setB()",id);
    if( mdfChk() && val != getB(id) ) mVal[id].mdf = true;
    switch( mVal[id].tp )
    {
	case IO::String:	mVal[id].val.s->setVal( (val!=EVAL_BOOL) ? TSYS::int2str((bool)val) : EVAL_STR );	break;
	case IO::Integer:	mVal[id].val.i = (val!=EVAL_BOOL) ? (bool)val : EVAL_INT;	break;
	case IO::Real:		mVal[id].val.r = (val!=EVAL_BOOL) ? (bool)val : EVAL_REAL;	break;
	case IO::Boolean:	mVal[id].val.b = val;	break;
    }
}

void TValFunc::setO( unsigned id, TVarObj *val )
{
    if( id >= mVal.size() )	throw TError("ValFnc",_("%s: Id or IO %d error!"),"setO()",id);
    switch( mVal[id].tp )
    {
	case IO::String:	setS(id, val->getStrXML());	break;
	case IO::Integer: case IO::Real: case IO::Boolean:
				setB(id, true);			break;
	case IO::Object:
	    if( mVal[id].val.o && !mVal[id].val.o->disconnect() ) delete mVal[id].val.o;
	    mVal[id].val.o = val;
	    mVal[id].val.o->connect();
	    break;
    }
}

void TValFunc::setMdfChk( bool set )
{
    mMdfChk = set;
    if( set )
	for( int i_v = 0; i_v < mVal.size(); i_v++ )
	    mVal[i_v].mdf = false;
}

void TValFunc::calc( const string &user )
{
    if( !mFunc || !mFunc->startStat() ) return;
    if( !user.empty() ) mUser = user;
    if( !mDimens ) mFunc->calc(this);
    else
    {
	long long t_cnt = TSYS::curTime();
	mFunc->calc(this);
	tm_calc = TSYS::curTime()-t_cnt;
    }
}

void TValFunc::preIOCfgChange( )
{
    setFunc( NULL, false );
}

void TValFunc::postIOCfgChange( )
{
    setFunc( mFunc, false );
}

TValFunc *TValFunc::ctxGet( int key )
{
    map<int,TValFunc* >::iterator vc = vctx.find(key);
    if( vc == vctx.end() ) return NULL;
    return vc->second;
}

void TValFunc::ctxSet( int key, TValFunc *val )
{
    map<int,TValFunc* >::iterator vc = vctx.find(key);
    if( vc == vctx.end() ) vctx[key] = val;
    else
    {
	if( vc->second ) delete vc->second;
	vc->second = val;
    }
}

void TValFunc::ctxClear( )
{
    for( map<int,TValFunc* >::iterator ivcx = vctx.begin(); ivcx != vctx.end(); ivcx++ )
	delete ivcx->second;
    vctx.clear();
}