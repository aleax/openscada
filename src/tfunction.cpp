
//OpenSCADA system file: tfunction.cpp
/***************************************************************************
 *   Copyright (C) 2003-2008 by Roman Savochenko                           *
 *   rom_as@fromru.com                                                     *
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

//*************************************************
//* Function abstract object                      *
//*************************************************
TFunction::TFunction( const string &iid ) : m_id(iid), m_tval(NULL), run_st(false)
{

}

TFunction::~TFunction()
{
    for( int i_io = 0; i_io < m_io.size(); i_io++ )
	delete m_io[i_io];
}

TFunction &TFunction::operator=(TFunction &func)
{
    if(m_id.empty())	m_id = func.id();
    //- Copy IO -
    //-- Clear no present IO --
    for( int i_io = 0; i_io < ioSize(); )
	if( func.ioId(io(i_io)->id()) < 0 )	ioDel(i_io);
	else i_io++;
    //-- Update present and create new IO --
    for( int i_io = 0; i_io < func.ioSize(); i_io++ )
    {
	int dst_io = ioId(func.io(i_io)->id());
	if( dst_io < 0 )
	    ioIns( new IO( func.io(i_io)->id().c_str(), func.io(i_io)->name().c_str(), func.io(i_io)->type(), func.io(i_io)->flg(),
		func.io(i_io)->def().c_str(), func.io(i_io)->hide(), func.io(i_io)->rez().c_str() ), i_io );
	else *io(dst_io) = *func.io(i_io);
    }

    return *this;
}

void TFunction::preDisable(int flag)
{
    if( m_tval ) { delete m_tval; m_tval = NULL; }
    if( used.size() )
    {
	string mess;
	for( int i=0; i < used.size(); i++ )
	    mess+=used[i]->vfName()+", ";
	throw TError(nodePath().c_str(),_("Function used by: %s"),mess.c_str());
    }
}

int TFunction::ioSize()
{
    return m_io.size();
}

IO *TFunction::io( int iid )
{
    if( iid >= m_io.size() ) throw TError(nodePath().c_str(),_("Index %d broken!"),iid);
    return m_io[iid];
}

int TFunction::ioId( const string &id )
{
    for( int i_io = 0; i_io < m_io.size(); i_io++ )
	if( m_io[i_io]->id() == id ) return i_io;
    return -1;
}

void TFunction::ioList( vector<string> &list )
{
    for( int i_io = 0; i_io < m_io.size(); i_io++ )
	list.push_back( m_io[i_io]->id() );
}

void TFunction::ioAdd( IO *io )
{
    preIOCfgChange();
    m_io.push_back(io);
    io->owner = this;
    postIOCfgChange();
}

void TFunction::ioIns( IO *io, int pos )
{
    if( pos < 0 || pos > m_io.size() )
	pos = m_io.size();

    preIOCfgChange();
    m_io.insert(m_io.begin()+pos,io);
    io->owner = this;
    postIOCfgChange();
}

void TFunction::ioDel( int pos )
{
    if( pos < 0 || pos >= m_io.size() )
        throw TError(nodePath().c_str(),_("Delete IO <%d> error."),pos);

    preIOCfgChange();
    m_io.erase(m_io.begin()+pos);
    postIOCfgChange();
}

void TFunction::ioMove( int pos, int to )
{
    if( pos < 0 || pos >= m_io.size() || to < 0 || to >= m_io.size() )
	throw TError(nodePath().c_str(),_("Move IO from %d to %d error."),pos,to);

    preIOCfgChange();
    IO *io = m_io[to];
    m_io[to] = m_io[pos];
    m_io[pos] = io;
    postIOCfgChange();
}

void TFunction::preIOCfgChange()
{
    string blk_lst;
    for(unsigned i=0; i < used.size(); i++)
	if( used[i]->blk() )	blk_lst+=used[i]->vfName()+",";
    if( blk_lst.size() )
	throw TError(nodePath().c_str(),_("Change no permit by function used: %s"),blk_lst.c_str());

    for(unsigned i=0; i < used.size(); i++)
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
    for(unsigned i=0; i < used.size() ;i++)
	if(used[i] == vfnc)
	    throw TError(nodePath().c_str(),_("Value <%s> already attached!"),vfnc->vfName().c_str());
    used.push_back(vfnc);
}

void TFunction::valDet( TValFunc *vfnc )
{
    f_res.resRequestW();
    for(unsigned i=0; i < used.size() ;i++)
	if(used[i] == vfnc)
	{
	    used.erase(used.begin()+i);
	    break;
	}
    f_res.resReleaseW();
}

void TFunction::cntrCmdProc( XMLNode *opt )
{
    //- Get page info -
    if( opt->name() == "info" )
    {
	ctrMkNode("oscada_cntr",opt,-1,"/",_("Function: ")+name(),0444,"root","root");
	if(ctrMkNode("area",opt,-1,"/func",_("Function")))
	{
	    if(ctrMkNode("area",opt,-1,"/func/st",_("State")))
		ctrMkNode("fld",opt,-1,"/func/st/st",_("Accessing"),0664,"root","root",1,"tp","bool");
	    if(ctrMkNode("area",opt,-1,"/func/cfg",_("Config")))
	    {
		ctrMkNode("fld",opt,-1,"/func/cfg/id",_("Id"),0444,"root","root",1,"tp","str");
		ctrMkNode("fld",opt,-1,"/func/cfg/name",_("Name"),0444,"root","root",1,"tp","str");
		ctrMkNode("fld",opt,-1,"/func/cfg/descr",_("Description"),0444,"root","root",3,"tp","str","cols","70","rows","4");
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
	    //-- Add test form --
	    if( m_tval )
	    {
		if(ctrMkNode("area",opt,-1,"/exec/io",_("IO")))
		    for( int i_io = 0; i_io < ioSize(); i_io++ )
		    {
			if( m_io[i_io]->hide() ) continue;

			const char *tp = "";
			switch(io(i_io)->type())
			{
			    case IO::String:	tp = "str";	break;
			    case IO::Integer:	tp = "dec";	break;
			    case IO::Real:	tp = "real";	break;
			    case IO::Boolean:	tp = "bool";	break;
			}
			ctrMkNode("fld",opt,-1,("/exec/io/"+io(i_io)->id()).c_str(),io(i_io)->name(),0664,"root","root",1,"tp",tp);
		    }
		//-- Add Calc button and Calc time --
		ctrMkNode("fld",opt,-1,"/exec/n_clc",_("Number calcs"),0664,"root","root",1,"tp","dec");
		ctrMkNode("fld",opt,-1,"/exec/tm",_("Calc time (mks)"),0444,"root","root",1,"tp","real");
		ctrMkNode("comm",opt,-1,"/exec/calc",_("Calc"),0666);
	    }
	}
        return;
    }

    //- Process command to page -
    string a_path = opt->attr("path");
    if( a_path == "/func/st/st" )
    {
	if( ctrChkNode(opt,"get",0664,"root","root",SEQ_RD) )	opt->setText(run_st?"1":"0");
	if( ctrChkNode(opt,"set",0664,"root","root",SEQ_WR) )	setStart(atoi(opt->text().c_str()));
    }
    else if( a_path == "/func/cfg/id" && ctrChkNode(opt) )	opt->setText(id());
    else if( a_path == "/func/cfg/name" && ctrChkNode(opt) )	opt->setText(name());
    else if( a_path == "/func/cfg/descr" && ctrChkNode(opt) )	opt->setText(descr());
    else if( a_path == "/io/io" && ctrChkNode(opt,"get",0440,"root","root",SEQ_RD) )
    {
	XMLNode *n_id	= ctrMkNode("list",opt,-1,"/io/io/0","");
	XMLNode *n_nm  	= ctrMkNode("list",opt,-1,"/io/io/1","");
	XMLNode *n_type	= ctrMkNode("list",opt,-1,"/io/io/2","");
	XMLNode *n_mode = ctrMkNode("list",opt,-1,"/io/io/3","");
	XMLNode *n_hide = ctrMkNode("list",opt,-1,"/io/io/4","");
	XMLNode *n_def 	= ctrMkNode("list",opt,-1,"/io/io/5","");
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
	if( ctrChkNode(opt,"get",0660,"root","root",SEQ_RD) )	opt->setText(m_tval?"1":"0");
	if( ctrChkNode(opt,"set",0660,"root","root",SEQ_WR) )
	{
	    bool to_en_exec = atoi(opt->text().c_str());
	    if( to_en_exec && !m_tval )	{ m_tval = new TValFunc(id()+"_exec",this); m_tval->setDimens(true); }
	    if( !to_en_exec && m_tval ) { delete m_tval; m_tval = NULL; }
	}
    }
    else if( a_path == "/exec/n_clc" && m_tval )
    {
	if( ctrChkNode(opt,"get",0664,"root","root",SEQ_RD) )	opt->setText(TBDS::genDBGet(nodePath()+"ntCalc","10",opt->attr("user")));
	if( ctrChkNode(opt,"set",0664,"root","root",SEQ_WR) )	TBDS::genDBSet(nodePath()+"ntCalc",opt->text(),opt->attr("user"));
    }
    else if( a_path == "/exec/tm" && m_tval && ctrChkNode(opt) )opt->setText(TSYS::real2str(m_tval->calcTm(),6));
    else if( a_path.substr(0,8) == "/exec/io" && m_tval )
    {
	string io_id = TSYS::pathLev(a_path,2);
	for( int i_io = 0; i_io < m_io.size(); i_io++ )
	    if( io_id == io(i_io)->id() )
	    {
		if( ctrChkNode(opt,"get",0664,"root","root",SEQ_RD) )	
		    opt->setText( (m_tval->ioType(i_io)==IO::Real) ? TSYS::real2str(m_tval->getR(i_io),6) : m_tval->getS(i_io) );
		if( ctrChkNode(opt,"set",0664,"root","root",SEQ_WR) )
		    m_tval->setS(i_io,opt->text());
		break;
	    }
    }
    else if( a_path == "/exec/calc" && m_tval && ctrChkNode(opt,"set",0666,"root","root",SEQ_WR) )	
    {
        double c_rez = 0;
	int n_tcalc = atoi(TBDS::genDBGet(nodePath()+"ntCalc","10",opt->attr("user")).c_str());
	for(int i_c = 0; i_c < n_tcalc; i_c++ )
	{
	    m_tval->calc();
	    c_rez += m_tval->calcTm();
	}
        m_tval->setCalcTm(c_rez);
    }
    else TCntrNode::cntrCmdProc(opt);
}


//*************************************************
//* IO                                            *
//*************************************************
IO::IO( const char *iid, const char *iname, IO::Type itype,  unsigned iflgs, const char *idef, bool ihide, const char *irez )
{
    m_id   = iid;
    m_name = iname;
    m_type = itype;
    m_flg  = iflgs;
    m_hide = ihide;
    m_def  = idef;
    m_rez  = irez;
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
    if(m_id==val) return;
    owner->preIOCfgChange();
    m_id = val;
    owner->postIOCfgChange();
}

void IO::setName( const string &val )
{
    if(m_name==val) return;
    //owner->preIOCfgChange();
    m_name = val;
    //owner->postIOCfgChange();
}

void IO::setType( Type val )
{
    if(m_type==val) return;
    owner->preIOCfgChange();
    m_type = val;
    owner->postIOCfgChange();
}

void IO::setFlg( unsigned val )
{
    if(m_flg==val) return;
    owner->preIOCfgChange();
    m_flg = val;
    owner->postIOCfgChange();
}

void IO::setDef( const string &val )
{
    if(m_def==val) return;
    //owner->preIOCfgChange();
    m_def = val; 
    //owner->postIOCfgChange();
}

void IO::setHide( bool val )
{
    if(m_hide==val) return;
    //owner->preIOCfgChange();
    m_hide = val;
    //owner->postIOCfgChange();
}

void IO::setRez( const string &val )
{
    if(m_rez==val) return;
    m_rez = val;
}

//*************************************************
//* TValFunc                                      *
//*************************************************
TValFunc::TValFunc( const string &iname, TFunction *ifunc, bool iblk ) : 
    m_name(iname), m_func(NULL), m_dimens(false), tm_calc(0.0), m_blk(iblk)
{
    setFunc(ifunc);
}

TValFunc::~TValFunc( )
{
    if( m_func ) funcDisConnect();
}

void TValFunc::setFunc( TFunction *ifunc, bool att_det )
{
    if( m_func ) funcDisConnect(att_det);
    if( ifunc )
    {
	m_func = ifunc;
	if( att_det )
	{
	    m_func->AHDConnect();
	    m_func->valAtt(this);
	}
	for( int i_vl = 0; i_vl < m_func->ioSize(); i_vl++ )
	{
	    SVl val;
	    val.tp = m_func->io(i_vl)->type();
	    if( val.tp == IO::String ) 		val.val.s = new string(m_func->io(i_vl)->def());
	    else if( val.tp == IO::Integer )	val.val.i = atoi(m_func->io(i_vl)->def().c_str());
	    else if( val.tp == IO::Real ) 	val.val.r = atof(m_func->io(i_vl)->def().c_str());
	    else if( val.tp == IO::Boolean )	val.val.b = atoi(m_func->io(i_vl)->def().c_str());
	    m_val.push_back(val);
	}
    }
}

void TValFunc::funcDisConnect( bool det )
{
    if( m_func )
    {
	for( int i_vl = 0; i_vl < m_val.size(); i_vl++ )
	    if( m_val[i_vl].tp == IO::String )		delete m_val[i_vl].val.s;
	m_val.clear();
	if(det)
	{
	    m_func->valDet(this);
	    m_func->AHDDisConnect();
	    m_func = NULL;
	}
    }
}

int TValFunc::ioId( const string &iid )
{
    if( !m_func )	throw TError("ValFnc",_("IO <%s> no present!"),iid.c_str());
    return m_func->ioId(iid);
}

void TValFunc::ioList( vector<string> &list )
{
    if( !m_func )       throw TError("ValFnc",_("Function no attached!"));
    return m_func->ioList(list);
}

int TValFunc::ioSize( )
{
    if( !m_func )       throw TError("ValFnc",_("Function no attached!"));
    return m_func->ioSize();
}

string TValFunc::getS( unsigned id )
{
    if( id >= m_val.size() )    throw TError("ValFnc",_("Id or IO %d error!"),id);
    switch(m_val[id].tp)
    {
	case IO::Integer:	return (m_val[id].val.i!=EVAL_INT) ? TSYS::int2str(m_val[id].val.i) : EVAL_STR;
	case IO::Real:		return (m_val[id].val.r!=EVAL_REAL) ? TSYS::real2str(m_val[id].val.r) : EVAL_STR;
	case IO::Boolean:	return (m_val[id].val.b!=EVAL_BOOL) ? TSYS::int2str((bool)m_val[id].val.b) : EVAL_STR;
	case IO::String:	return *(m_val[id].val.s);
    }
    return "";
}

int TValFunc::getI( unsigned id )
{
    if( id >= m_val.size() )    throw TError("ValFnc",_("Id or IO %d error!"),id);
    switch(m_val[id].tp)
    {
	case IO::String:	return ((*m_val[id].val.s)!=EVAL_STR) ? atoi(m_val[id].val.s->c_str()) : EVAL_INT;
	case IO::Real:		return (m_val[id].val.r!=EVAL_REAL) ? (int)m_val[id].val.r : EVAL_INT;
	case IO::Boolean:	return (m_val[id].val.b!=EVAL_BOOL) ? (bool)m_val[id].val.b : EVAL_INT;
	case IO::Integer:	return m_val[id].val.i;
    }
    return 0;
}

double TValFunc::getR( unsigned id )
{
    if( id >= m_val.size() )    throw TError("ValFnc",_("Id or IO %d error!"),id);
    switch(m_val[id].tp)
    {
	case IO::String:	return ((*m_val[id].val.s)!=EVAL_STR) ? atof(m_val[id].val.s->c_str()) : EVAL_REAL;
	case IO::Integer:	return (m_val[id].val.i!=EVAL_INT) ? m_val[id].val.i : EVAL_REAL;
	case IO::Boolean:	return (m_val[id].val.b!=EVAL_BOOL) ? (bool)m_val[id].val.b : EVAL_REAL;
	case IO::Real:		return m_val[id].val.r;
    }
    return 0.0;
}

char TValFunc::getB( unsigned id )
{
    if( id >= m_val.size() )    throw TError("ValFnc",_("Id or IO %d error!"),id);
    switch(m_val[id].tp)
    {
	case IO::String:	return ((*m_val[id].val.s)!=EVAL_STR) ? (bool)atoi(m_val[id].val.s->c_str()) : EVAL_BOOL;
	case IO::Integer:	return (m_val[id].val.i!=EVAL_INT) ? (bool)m_val[id].val.i : EVAL_BOOL;
	case IO::Real:		return (m_val[id].val.r!=EVAL_REAL) ? (bool)m_val[id].val.r : EVAL_BOOL;
	case IO::Boolean:	return m_val[id].val.b;
    }
    return false;
}

void TValFunc::setS( unsigned id, const string &val )
{
    if( id >= m_val.size() )    throw TError("ValFnc",_("Id or IO %d error!"),id);
    switch(m_val[id].tp)
    {
	case IO::Integer:	m_val[id].val.i = (val!=EVAL_STR) ? atoi(val.c_str()) : EVAL_INT;	break;
	case IO::Real:		m_val[id].val.r = (val!=EVAL_STR) ? atof(val.c_str()) : EVAL_REAL;	break;
	case IO::Boolean:	m_val[id].val.b = (val!=EVAL_STR) ? (bool)atoi(val.c_str()) : EVAL_BOOL;	break;
	case IO::String:	*(m_val[id].val.s) = val;	break;
    }
}

void TValFunc::setI( unsigned id, int val )
{
    if( id >= m_val.size() )    throw TError("ValFnc",_("Id or IO %d error!"),id);
    switch(m_val[id].tp)
    {
	case IO::String:	*(m_val[id].val.s) = (val!=EVAL_INT) ? TSYS::int2str(val) : EVAL_STR;	break;
	case IO::Real:		m_val[id].val.r = (val!=EVAL_INT) ? val : EVAL_REAL;		break;
	case IO::Boolean:	m_val[id].val.b = (val!=EVAL_INT) ? (bool)val : EVAL_BOOL;	break;
	case IO::Integer:	m_val[id].val.i = val;	break;
    }
}

void TValFunc::setR( unsigned id, double val )
{
    if( id >= m_val.size() )    throw TError("ValFnc",_("Id or IO %d error!"),id);
    if( isnan(val) ) val = 0.;	//Check for 'Not a Number'
    switch(m_val[id].tp)
    {
	case IO::String:	*(m_val[id].val.s) = (val!=EVAL_REAL) ? TSYS::real2str(val) : EVAL_STR;	break;
	case IO::Integer:	m_val[id].val.i = (val!=EVAL_REAL) ? (int)val : EVAL_INT;	break;
	case IO::Boolean:	m_val[id].val.b = (val!=EVAL_REAL) ? (bool)val : EVAL_BOOL;	break;
	case IO::Real:		m_val[id].val.r = val;	break;
    }
}

void TValFunc::setB( unsigned id, char val )
{
    if( id >= m_val.size() )    throw TError("ValFnc",_("Id or IO %d error!"),id);
    switch(m_val[id].tp)
    {
	case IO::String:	*(m_val[id].val.s) = (val!=EVAL_BOOL) ? TSYS::int2str((bool)val) : EVAL_STR;	break;
	case IO::Integer:	m_val[id].val.i = (val!=EVAL_BOOL) ? (bool)val : EVAL_INT;	break;
	case IO::Real:		m_val[id].val.r = (val!=EVAL_BOOL) ? (bool)val : EVAL_REAL;	break;
	case IO::Boolean:	m_val[id].val.b = val;	break;
    }
}

void TValFunc::calc( )
{
    if( !m_func || !m_func->startStat() ) return;
    if( !m_dimens ) m_func->calc(this);
    else
    {
	unsigned long long t_cnt = SYS->shrtCnt();
	m_func->calc(this);
	tm_calc = 1.0e6*((double)(SYS->shrtCnt()-t_cnt))/((double)SYS->sysClk());
    }
}

void TValFunc::preIOCfgChange()
{
    setFunc( NULL, false );
}

void TValFunc::postIOCfgChange()
{
    setFunc( m_func, false );
}
