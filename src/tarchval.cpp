
//OpenSCADA system file: tarchval.cpp
/***************************************************************************
 *   Copyright (C) 2006-2008 by Roman Savochenko                           *
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

#include <signal.h>
#include <string.h>
#include <time.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <math.h>

#include <gd.h>
#include <gdfonts.h>
#include <gdfontt.h>

#include "tsys.h"
#include "tvalue.h"
#include "tarchives.h"
#include "tarchval.h"


//*************************************************
//* Value archivator                              *
//*************************************************

//*************************************************
//* TValBuf                                       *
//*************************************************
TValBuf::TValBuf( ) :
    m_val_tp(TFld::Integer), m_size(100), m_per(0), m_hrd_grd(false), m_hg_res_tm(false), m_end(0), m_beg(0)
{
    buf.bl = NULL;

    makeBuf(m_val_tp,m_size,m_per,m_hrd_grd,m_hg_res_tm);
}

TValBuf::TValBuf( TFld::Type vtp, int isz, long long ipr, bool ihgrd, bool ihres ) :
    m_val_tp(vtp), m_size(isz), m_per(ipr), m_hrd_grd(ihgrd), m_hg_res_tm(ihres), m_end(0), m_beg(0)
{
    buf.bl = NULL;

    makeBuf(m_val_tp,m_size,m_per,m_hrd_grd,m_hg_res_tm);
}

void TValBuf::clear()
{
    switch(m_val_tp)
    {
	case TFld::Boolean:	buf.bl->clear();	break;
	case TFld::Integer:	buf.dec->clear();	break;
	case TFld::Real:	buf.real->clear();	break;
	case TFld::String:	buf.str->clear(); 	break;
    }
}

TValBuf::~TValBuf( )
{
    switch(m_val_tp)
    {
	case TFld::Boolean:	delete buf.bl;  break;
	case TFld::Integer:	delete buf.dec; break;
	case TFld::Real:	delete buf.real;break;
	case TFld::String:	delete buf.str; break;
    }
}

int TValBuf::realSize()
{
    switch(valType())
    {
	case TFld::Boolean:	return buf.bl->realSize();
	case TFld::Integer:	return buf.dec->realSize();
	case TFld::Real:	return buf.real->realSize();
	case TFld::String:	return buf.str->realSize();
    }

    return 0;
}

bool TValBuf::vOK( long long ibeg, long long iend )
{
    if( !begin() || !end() || iend < begin() || ibeg > end() || ibeg > iend )
	return false;
    return true;
}

void TValBuf::setValType( TFld::Type vl )
{
    makeBuf(vl, m_size, m_per, m_hrd_grd, m_hg_res_tm );
}

void TValBuf::setHardGrid( bool vl )
{
    makeBuf( m_val_tp, m_size, m_per, vl, m_hg_res_tm );
}

void TValBuf::setHighResTm( bool vl )
{
    makeBuf( m_val_tp, m_size, m_per, m_hrd_grd, vl );
}

void TValBuf::setSize( int vl )
{
    makeBuf( m_val_tp, vl, m_per, m_hrd_grd, m_hg_res_tm );
}

void TValBuf::setPeriod( long long vl )
{
    makeBuf( m_val_tp, m_size, vl, m_hrd_grd, m_hg_res_tm );
}

void TValBuf::makeBuf( TFld::Type v_tp, int isz, long long ipr, bool hd_grd, bool hg_res )
{
    ResAlloc res(b_res,true);

    //Destroy buffer
    if( v_tp != m_val_tp && buf.bl )
    {
	switch(m_val_tp)
	{
	    case TFld::Boolean:	delete buf.bl;	break;
	    case TFld::Integer:	delete buf.dec;	break;
	    case TFld::Real:	delete buf.real;break;
	    case TFld::String:	delete buf.str;	break;
	}
	buf.bl = NULL;
    }

    if( !buf.bl )
    {
	//Make new buffer
	switch(v_tp)
	{
	    case TFld::Boolean:	buf.bl = new TBuf<char>(EVAL_BOOL,m_size,m_per,m_hrd_grd,m_hg_res_tm,m_end,m_beg);	break;
	    case TFld::Integer:	buf.dec = new TBuf<int>(EVAL_INT,m_size,m_per,m_hrd_grd,m_hg_res_tm,m_end,m_beg);	break;
	    case TFld::Real:	buf.real = new TBuf<double>(EVAL_REAL,m_size,m_per,m_hrd_grd,m_hg_res_tm,m_end,m_beg);	break;
	    case TFld::String:	buf.str = new TBuf<string>(EVAL_STR,m_size,m_per,m_hrd_grd,m_hg_res_tm,m_end,m_beg);	break;
	}
	m_val_tp = v_tp;
    }
    if( isz != m_size || ipr != m_per || hd_grd != m_hrd_grd || hg_res != m_hg_res_tm )
	switch(m_val_tp)
	{
	    case TFld::Boolean:	buf.bl->makeBuf(isz, ipr, hd_grd, hg_res);	break;
	    case TFld::Integer:	buf.dec->makeBuf(isz, ipr, hd_grd, hg_res);	break;
	    case TFld::Real:	buf.real->makeBuf(isz, ipr, hd_grd, hg_res);	break;
	    case TFld::String:	buf.str->makeBuf(isz, ipr, hd_grd, hg_res);	break;
	}
}

string TValBuf::getS( long long *itm, bool up_ord )
{
    switch(valType())
    {
	case TFld::Boolean:
	    { char vl = getB(itm,up_ord); return (vl==EVAL_BOOL)?EVAL_STR:TSYS::int2str((bool)vl); }
	case TFld::Integer:
	    { int vl = getI(itm,up_ord); return (vl==EVAL_INT)?EVAL_STR:TSYS::int2str(vl); }
	case TFld::Real:
	    { double vl = getR(itm,up_ord); return (vl==EVAL_REAL)?EVAL_STR:TSYS::real2str(vl); }
	case TFld::String:
	    ResAlloc res(b_res,false); return buf.str->get(itm,up_ord);
    }
}

double TValBuf::getR( long long *itm, bool up_ord )
{
    switch(valType())
    {
	case TFld::Boolean:
	    { char vl = getB(itm,up_ord); return (vl==EVAL_BOOL)?EVAL_REAL:(bool)vl; }
	case TFld::Integer:
	    { int vl = getI(itm,up_ord); return (vl==EVAL_INT)?EVAL_REAL:(double)vl; }
	case TFld::String:
	    { string vl = getS(itm,up_ord); return (vl==EVAL_STR)?EVAL_REAL:atof(vl.c_str()); }
	case TFld::Real:
	    ResAlloc res(b_res,false); return buf.real->get(itm,up_ord);
    }
}

int TValBuf::getI( long long *itm, bool up_ord )
{
    switch(valType())
    {
	case TFld::Boolean:
	    { char vl = getB(itm,up_ord); return (vl==EVAL_BOOL)?EVAL_INT:(bool)vl; }
	case TFld::String:
	    { string vl = getS(itm,up_ord); return (vl==EVAL_STR)?EVAL_INT:atoi(vl.c_str()); }
	case TFld::Real:
	    { double vl = getR(itm,up_ord); return (vl==EVAL_REAL)?EVAL_INT:(int)vl; }
	case TFld::Integer:
	    ResAlloc res(b_res,false); return buf.dec->get(itm,up_ord);
    }
}

char TValBuf::getB( long long *itm, bool up_ord )
{
    switch(valType())
    {
	case TFld::Integer:
	    { int vl = getI(itm,up_ord); return (vl==EVAL_INT)?EVAL_BOOL:(bool)vl; }
	case TFld::String:
	    { string vl = getS(itm,up_ord); return (vl==EVAL_STR)?EVAL_BOOL:(bool)atoi(vl.c_str()); }
	case TFld::Real:
	    { double vl = getR(itm,up_ord); return (vl==EVAL_REAL)?EVAL_BOOL:(bool)vl; }
	case TFld::Boolean:
	    ResAlloc res(b_res,false); return buf.bl->get(itm,up_ord);
    }
}

void TValBuf::setS( const string &value, long long tm )
{
    switch(valType())
    {
	case TFld::Boolean:
	    setB((value==EVAL_STR)?EVAL_BOOL:(bool)atoi(value.c_str()),tm);	break;
	case TFld::Integer:
	    setI((value==EVAL_STR)?EVAL_INT:atoi(value.c_str()),tm);	break;
	case TFld::Real:
	    setR((value==EVAL_STR)?EVAL_REAL:atof(value.c_str()),tm);	break;
	case TFld::String:
	    ResAlloc res(b_res,true); buf.str->set(value,tm);		break;
    }
}

void TValBuf::setR( double value, long long tm )
{
    switch(valType())
    {
	case TFld::Boolean:
	    setB((value==EVAL_REAL)?EVAL_BOOL:(bool)value,tm);	break;
	case TFld::Integer:
	    setI((value==EVAL_REAL)?EVAL_INT:(int)value,tm);	break;
	case TFld::String:
	    setS((value==EVAL_REAL)?EVAL_STR:TSYS::real2str(value),tm);	break;
	case TFld::Real:
	    ResAlloc res(b_res,true); buf.real->set(value,tm);	break;
    }
}

void TValBuf::setI( int value, long long tm )
{
    switch(valType())
    {
	case TFld::Boolean:
	    setB((value==EVAL_INT)?EVAL_BOOL:(bool)value,tm);	break;
	case TFld::String:
	    setS((value==EVAL_INT)?EVAL_STR:TSYS::int2str(value),tm);	break;
	case TFld::Real:
	    setR((value==EVAL_INT)?EVAL_REAL:value,tm);		break;
	case TFld::Integer:
	    ResAlloc res(b_res,true); buf.dec->set(value,tm);	break;
    }
}

void TValBuf::setB( char value, long long tm )
{
    switch(valType())
    {
	case TFld::Integer:
	    setI((value==EVAL_BOOL)?EVAL_INT:(bool)value,tm);	break;
	case TFld::String:
	    setS((value==EVAL_BOOL)?EVAL_STR:TSYS::int2str((bool)value),tm);	break;
	case TFld::Real:
	    setR((value==EVAL_BOOL)?EVAL_REAL:(bool)value,tm);	break;
	case TFld::Boolean:
	    ResAlloc res(b_res,true); buf.bl->set(value,tm);	break;
    }
}

void TValBuf::getVal( TValBuf &buf, long long ibeg, long long iend )
{
    if(!vOK(ibeg,iend))	return;
    ibeg = vmax(ibeg,begin());
    iend = vmin(iend,end());

    while(ibeg<iend)
    {
	switch(valType())
	{
	    case TFld::Boolean:
	    {
		char val = getB(&ibeg,true);
		buf.setB(val,ibeg);
		break;
	    }
	    case TFld::Integer:
	    {
		int val = getI(&ibeg,true);
		buf.setI(val,ibeg);
		break;
	    }
	    case TFld::Real:
	    {
		double val = getR(&ibeg,true);
		buf.setR(val,ibeg);
		break;
	    }
	    case TFld::String:
	    {
		string val = getS(&ibeg,true);
		buf.setS(val,ibeg);
		break;
	    }
	}
        ibeg++;
    }
}

void TValBuf::setVal( TValBuf &buf, long long ibeg, long long iend )
{
    buf.getVal( *this, ibeg, iend );
}

//*************************************************
//* TValBuf::TBuf                                 *
//*************************************************
template <class TpVal> TValBuf::TBuf<TpVal>::TBuf( TpVal ieval, int &isz, long long &ipr, bool &ihgrd, bool &ihres, long long& iend, long long& ibeg ) :
    eval(ieval), size(isz), per(ipr), hrd_grd(ihgrd), hg_res_tm(ihres), end(iend), beg(ibeg), cur(0)
{
    buf.grid = NULL;

    makeBuf(size,per,hrd_grd,hg_res_tm);
}

template <class TpVal> TValBuf::TBuf<TpVal>::~TBuf( )
{
    if( hrd_grd )		delete buf.grid;
    else if( hg_res_tm )	delete buf.tm_high;
    else			delete buf.tm_low;
}

template <class TpVal> void TValBuf::TBuf<TpVal>::clear()
{
    if( !buf.grid )	return;
    if( hrd_grd )	buf.grid->clear();
    else if(hg_res_tm)	buf.tm_high->clear();
    else		buf.tm_low->clear();
    cur = end = beg = 0;
}

template <class TpVal> int TValBuf::TBuf<TpVal>::realSize()
{
    if( !buf.grid )	return 0;
    if( hrd_grd )	return buf.grid->size();
    else if(hg_res_tm)	return buf.tm_high->size();
    else		return buf.tm_low->size();
}

template <class TpVal> void TValBuf::TBuf<TpVal>::makeBuf( int isz, long long ipr, bool ihd_grd, bool ihg_res )
{
    bool recr_buf = false;
    if( !buf.grid )		recr_buf = true;
    if( isz < size )		recr_buf = true;
    if( hrd_grd != ihd_grd )
    {
	if( ihd_grd && !ipr )	ipr = 1000000;
	recr_buf =true;
    }
    if( hg_res_tm != ihg_res )	recr_buf =true;
    if( per != ipr )
    {
	if( hrd_grd && !ipr )	ipr = 1000000;
	recr_buf =true;
    }

    size = isz;
    hrd_grd = ihd_grd;
    hg_res_tm = ihg_res;
    per = ipr;

    if(recr_buf)
    {
	//- Destroy buffer -
	cur = end = beg = 0;
	if( buf.grid )
	{
	    if( hrd_grd )		delete buf.grid;
	    else if( hg_res_tm )	delete buf.tm_high;
	    else			delete buf.tm_low;
	}
	buf.grid = NULL;

	//- Make new buffer -
	if( ihd_grd )	buf.grid = new vector<TpVal>;
	else if(ihg_res)buf.tm_high = new vector<SHg>;
	else		buf.tm_low = new vector<SLw>;
    }
}

template <class TpVal> TpVal TValBuf::TBuf<TpVal>::get( long long *itm, bool up_ord )
{
    long long tm = (itm)?(*itm):TSYS::curTime();

    if((up_ord && tm > end) || (!up_ord && tm < beg))
	throw TError("ValBuf",_("Value no present."));
    if( hrd_grd )
    {
	//- Process hard grid buffer -
	int npos = up_ord?(end-tm)/per:(long long)buf.grid->size()-1-(tm-beg)/per;
	if( npos < 0 || npos >= buf.grid->size() ) return eval;
	if(itm)	*itm = end-npos*per;
	return (*buf.grid)[((cur-npos-1)>=0)?(cur-npos-1):(buf.grid->size()+(cur-npos-1))];
    }
    else if( per )
    {
	//- Process soft grid buffer -
	int npos = (up_ord?(end-tm):(tm-beg))/per;
	if( hg_res_tm )
	{
	    int c_cur  = (cur)?cur-1:buf.tm_high->size()-1;
	    int c_end = c_cur;
	    do
	    {
		int w_pos = (up_ord?end-(*buf.tm_high)[c_end].tm:(*buf.tm_high)[c_end].tm-beg)/per;
		if( up_ord && w_pos >= npos )
		{
		    if(itm) *itm = (w_pos==npos)?(*buf.tm_high)[c_end].tm:end-npos*per;
		    return (*buf.tm_high)[c_end].val;
		}
		else if( !up_ord && w_pos <= npos )
		{
		    if(itm) *itm = (w_pos==npos)?(*buf.tm_high)[c_end].tm:beg+npos*per;
		    return (*buf.tm_high)[c_end].val;
		}
		if( --c_end < 0 ) c_end = buf.tm_high->size()-1;
	    }while(c_end != c_cur);

	    return eval;
	}
	else
	{
	    int c_cur = (cur)?cur-1:buf.tm_low->size()-1;
	    int c_end = c_cur;
	    do
	    {
		int w_pos = (up_ord?end-(long long)(*buf.tm_low)[c_end].tm*1000000:(long long)(*buf.tm_low)[c_end].tm*1000000-beg)/per;
		if( up_ord && w_pos >= npos )
		{
		    if(itm) *itm = (w_pos==npos)?(long long)(*buf.tm_low)[c_end].tm*1000000:end-npos*per;
		    return (*buf.tm_low)[c_end].val;
		}
		if( !up_ord && w_pos <= npos )
		{
		    if(itm) *itm = (w_pos==npos)?(long long)(*buf.tm_low)[c_end].tm*1000000:beg+npos*per;
		    return (*buf.tm_low)[c_end].val;
		}
		if( --c_end < 0 ) c_end = buf.tm_low->size()-1;
	    }while(c_end != c_cur);

	    return eval;
	}
    }
    else
    {
	//- Proccess flow buffer -
	if( hg_res_tm )
	{
	    int c_end = buf.tm_high->size()-1;
	    //-- Half divider --
	    for( int d_win = c_end/2; d_win > 10; d_win/=2 )
		if( !((!up_ord && tm >= (*buf.tm_high)[c_end-d_win].tm) ||
			(up_ord && tm <= (*buf.tm_high)[buf.tm_high->size()-(c_end-d_win)-1].tm)) )
		    c_end-=d_win;
	    //-- Scan last window --
	    while(c_end >= 0)
	    {
		if( !up_ord && tm >= (*buf.tm_high)[c_end].tm )
		{
		    if(itm) *itm = (*buf.tm_high)[c_end].tm;
		    return (*buf.tm_high)[c_end].val;
		}
		else if( up_ord && tm <= (*buf.tm_high)[buf.tm_high->size()-c_end-1].tm )
		{
		    if(itm) *itm = (*buf.tm_high)[buf.tm_high->size()-c_end-1].tm;
		    return (*buf.tm_high)[buf.tm_high->size()-c_end-1].val;
		}
		c_end--;
	    }
	    return eval;
	}
	else
	{
	    int c_end = buf.tm_low->size()-1;
	    //-- Half divider --
	    for( int d_win = c_end/2; d_win>10; d_win/=2 )
		if( !((!up_ord && tm/1000000 >= (*buf.tm_low)[c_end-d_win].tm) ||
			(up_ord && tm <= (long long)(*buf.tm_low)[buf.tm_low->size()-(c_end-d_win)-1].tm*1000000)) )
		    c_end-=d_win;
	    //-- Scan last window --
	    while(c_end >= 0)
	    {
		if( !up_ord && tm/1000000 >= (*buf.tm_low)[c_end].tm )
		{
		    if(itm) *itm = (long long)(*buf.tm_low)[c_end].tm*1000000;
		    return (*buf.tm_low)[c_end].val;
		}
		else if( up_ord && tm <= (long long)(*buf.tm_low)[buf.tm_low->size()-c_end-1].tm*1000000 )
		{
		    if(itm) *itm = (long long)(*buf.tm_low)[buf.tm_low->size()-c_end-1].tm*1000000;
		    return (*buf.tm_low)[buf.tm_low->size()-c_end-1].val;
		}
		c_end--;
	    }
	    return eval;
	}
    }
}

template <class TpVal> void TValBuf::TBuf<TpVal>::set( TpVal value, long long tm )
{
    if( !tm )	tm = TSYS::curTime();
    if( !end )	end = (per) ? per*(tm/per-1) : tm;
    if( !beg )	beg = (per) ? per*(tm/per) : tm;

    if( hrd_grd )
    {
	//- Process hard grid buffer -
	int npos = (tm-end)/per;
	//-- Set value --
	if( !npos )
	{
	    (*buf.grid)[ (cur) ? cur-1 : buf.grid->size()-1 ] = value;	//Update last value
	    return;
	}
	else if( npos < 0 )	throw TError("ValBuf",_("Grid mode no support inserting old values."));	
	else
	    while( npos-- )
	    {
		TpVal w_val = (npos)?eval:value;
		//--- Set new value ---
		if( cur >= buf.grid->size() )	buf.grid->push_back(w_val);
		else
		{
		    beg += per;
		    (*buf.grid)[cur] = w_val;
		}
		//--- Update cursor ---
		if( ++cur >= size ) cur = 0;
		//--- Update end time ---
		end += per;
	    }
    }
    else if( per )
    {
	//- Process soft grid buffer -
	int npos = (tm-end)/per;
	//-- Set value --
	if( npos < 0 )	throw TError("ValBuf",_("Grid mode no support inserting old values."));
	else
	{
	    if( hg_res_tm )
	    {
		SHg b_el;

		//--- Update current end value ---
		if( npos == 0)
		{
		    b_el.tm = end;
		    b_el.val = value;
		    int h_el = (cur)?cur-1:buf.tm_high->size()-1;
		    if( ((*buf.tm_high)[h_el].tm-end)/per )
		    {
			//---- Write new value ----
			if(cur >= buf.tm_high->size()) buf.tm_high->push_back(b_el);
			else
			{
			    if( cur+1 >= buf.tm_high->size() )
				beg = (*buf.tm_high)[0].tm;
			    else beg = (*buf.tm_high)[cur+1].tm;
			    beg = per*(beg/per);
			    (*buf.tm_high)[cur] = b_el;
			}
			//---- Update cursor ----
			if( ++cur >= size ) cur = 0;
		    }
		    else (*buf.tm_high)[h_el] = b_el;
		    return;
		}

		//--- Insert new value ----
		int c_npos = npos;
		while( c_npos-- )
		{
		    //---- Prepare data ----
		    if(c_npos) 	{ b_el.tm = end+(npos-c_npos)*per;  b_el.val = eval; }
		    else	{ b_el.tm = tm; b_el.val = value; }
		    //---- Check previous value ----
		    if( !buf.tm_high->size() ||
			    (cur && (*buf.tm_high)[cur-1].val != b_el.val ) ||
			    (!cur && (*buf.tm_high)[buf.tm_high->size()-1].val != b_el.val) )
		    {
			//---- Write new value ----
			if(cur >= buf.tm_high->size())	buf.tm_high->push_back(b_el);
			else
			{
			    if( cur+1 >= buf.tm_high->size() )
				beg = (*buf.tm_high)[0].tm;
			    else beg = (*buf.tm_high)[cur+1].tm;
			    beg = per*(beg/per);
			    (*buf.tm_high)[cur]=b_el;
			}
			//---- Update cursor ----
			if( ++cur >= size ) cur = 0;
		    }
		    //---- Update end time ----
		    end += per;
		}
	    }
	    else
	    {
		SLw b_el;

		//--- Update current end value ---
		if( npos == 0)
		{
		    b_el.tm = end/1000000;
		    b_el.val = value;
		    int h_el = (cur) ? cur-1 : buf.tm_low->size()-1;
		    if( ((long long)(*buf.tm_low)[h_el].tm*1000000-end)/per )
		    {
			//---- Write new value ----
			if( cur >= buf.tm_low->size() )	buf.tm_low->push_back(b_el);
			else
			{
			    if( cur+1 >= buf.tm_low->size() )
				beg = (long long)(*buf.tm_low)[0].tm * 1000000;
			    else beg = (long long)(*buf.tm_low)[cur+1].tm * 1000000;
			    beg = per*(beg/per);
			    (*buf.tm_low)[cur] = b_el;
			}
			//---- Update cursor ----
			if( ++cur >= size ) cur = 0;
		    }
		    else (*buf.tm_low)[h_el] = b_el;
		    return;
		}

		//--- Insert new value ---
		int c_npos = npos;
		while( c_npos-- )
		{
		    //---- Prepare data ----
		    if(c_npos)	{ b_el.tm = (end+(npos-c_npos)*per)/1000000; b_el.val = eval; }
		    else	{ b_el.tm = tm/1000000; b_el.val = value; }
		    //---- Check previous value ----
		    if( !buf.tm_low->size() ||
			    (cur && (*buf.tm_low)[cur-1].val != b_el.val ) ||
			    (!cur && (*buf.tm_low)[buf.tm_low->size()-1].val != b_el.val) )
		    {
			//---- Write new value ----
			if(cur >= buf.tm_low->size())	buf.tm_low->push_back(b_el);
			else
			{
			    if( cur+1 >= buf.tm_low->size() )
				beg = (long long)(*buf.tm_low)[0].tm * 1000000;
			    else beg = (long long)(*buf.tm_low)[cur+1].tm * 1000000;
			    beg = per*(beg/per);
			    (*buf.tm_low)[cur]=b_el;
			}
			//---- Update cursor ----
			if( ++cur >= size ) cur = 0;
		    }
		    //---- Update end time ----
		    end+=per;
		}
	    }
	}
    }
    else
    {
	//- Proccess flow buffer -
	if( hg_res_tm )
	{
	    SHg b_el = { tm, value };
	    if( tm < beg && size && buf.tm_high->size() >= size )
		throw TError("ValBuf",_("Set too old value to buffer."));
	    int c_pos = 0;

	    //-- Half divider --
	    int d_win = buf.tm_high->size()/2;
	    while(d_win>10)
	    {
		if( tm > (*buf.tm_high)[c_pos+d_win].tm )
		    c_pos+=d_win;
		d_win/=2;
	    }
	    //-- Scan last window --
	    while( true )
	    {
		if( c_pos >= buf.tm_high->size() || tm < (*buf.tm_high)[c_pos].tm )
		{
		    if( c_pos == buf.tm_high->size() )	buf.tm_high->push_back(b_el);
		    else buf.tm_high->insert(buf.tm_high->begin()+c_pos,b_el);
		    if( size && buf.tm_high->size() > size ) buf.tm_high->erase(buf.tm_high->begin());
		    beg = (*buf.tm_high)[0].tm;
		    end = (*buf.tm_high)[buf.tm_high->size()-1].tm;
		    return;
		}
		else if( tm == (*buf.tm_high)[c_pos].tm )
		{
		    (*buf.tm_high)[c_pos] = b_el;
		    return;
		}
		c_pos++;
	    }
	}
	else
	{
	    SLw b_el = { tm/1000000, value };
	    if( tm < beg && size && buf.tm_low->size() >= size )
		throw TError("ValBuf",_("Set too old value to buffer."));
	    int c_pos = 0;
	    //-- Half divider --
	    int d_win = buf.tm_low->size()/2;
	    while( d_win > 10 )
	    {
		if( tm/1000000 > (*buf.tm_low)[c_pos+d_win].tm )
		    c_pos += d_win;
		d_win /= 2;
	    }
	    //-- Scan last window --
	    while( true )
	    {
		if( c_pos >= buf.tm_low->size() || tm/1000000 < (*buf.tm_low)[c_pos].tm )
		{
		    if( c_pos == buf.tm_low->size() ) buf.tm_low->push_back(b_el);
		    else buf.tm_low->insert(buf.tm_low->begin()+c_pos,b_el);
		    if( size && buf.tm_low->size() > size ) buf.tm_low->erase(buf.tm_low->begin());
		    beg = (long long)(*buf.tm_low)[0].tm*1000000;
		    end = (long long)(*buf.tm_low)[buf.tm_low->size()-1].tm*1000000;
		    return;
		}
		else if( tm/1000000 == (*buf.tm_low)[c_pos].tm )
		{
		    (*buf.tm_low)[c_pos] = b_el;
		    return;
		}
		c_pos++;
	    }
	}
    }
}


//*************************************************
//*  TVArchive                                    *
//*************************************************
TVArchive::TVArchive( const string &iid, const string &idb, TElem *cf_el ) :
    TConfig(cf_el), run_st(false), m_db(idb),
    m_id(cfg("ID").getSd()), m_name(cfg("NAME").getSd()), m_dscr(cfg("DESCR").getSd()), m_start(cfg("START").getBd()),
    m_vtype(cfg("VTYPE").getId()), m_bper(cfg("BPER").getRd()), m_bsize(cfg("BSIZE").getId()),
    m_bhgrd(cfg("BHGRD").getBd()), m_bhres(cfg("BHRES").getBd()), m_srcmode(cfg("SrcMode").getId()),
    m_dsourc(cfg("Source").getSd()), m_archs(cfg("ArchS").getSd())
{
    //- First init -
    m_id = iid;
    m_vtype = TFld::Real;

    setUpBuf();
}

TVArchive::~TVArchive( )
{

}

TCntrNode &TVArchive::operator=( TCntrNode &node )
{
    TVArchive *src_n = dynamic_cast<TVArchive*>(&node);
    if( !src_n ) return *this;

    //- Configuration copy -
    string tid = id();
    *(TConfig*)this = *(TConfig*)src_n;
    m_id = tid;
    m_db = src_n->m_db;

    if( src_n->startStat() && toStart() && !startStat() )
	start( );

    return *this;
}

void TVArchive::preDisable( int flag )
{
    stop( flag );
}

void TVArchive::postDisable(int flag)
{
    try
    {
	if( flag )
	    SYS->db().at().dataDel(fullDB(),owner().nodePath()+tbl(),*this);
    }catch(TError err)
    { mess_warning(err.cat.c_str(),"%s",err.mess.c_str()); }
}

string TVArchive::name()
{
    return (m_name.size())?m_name:m_id;
}

TArchiveS &TVArchive::owner()
{
    return *(TArchiveS *)nodePrev();
}

string TVArchive::tbl( )
{
    return owner().subId()+"_val";
}

long long TVArchive::end( const string &arch )
{
    long long rez = 0;
    if( arch.empty() || arch == BUF_ARCH_NM )
    {
	rez = TValBuf::end();
	if( !arch.empty() ) return rez;
    }

    ResAlloc res(a_res,false);
    for( int i_a = 0; i_a < arch_el.size(); i_a++ )
        if( (arch.empty() || arch == arch_el[i_a]->archivator().workId()) && arch_el[i_a]->end() > rez )
	{
	    rez = arch_el[i_a]->end();
	    if( !arch.empty() )	break;
	}
    return rez;
}

long long TVArchive::begin( const string &arch )
{
    long long rez = TSYS::curTime();
    if( arch.empty() || arch == BUF_ARCH_NM )
    {
	rez = TValBuf::begin();
	if( !arch.empty() ) return rez;
    }

    ResAlloc res(a_res,false);
    for( int i_a = 0; i_a < arch_el.size(); i_a++ )
	if( (arch.empty() || arch == arch_el[i_a]->archivator().workId()) && arch_el[i_a]->begin() < rez )
	{
	    rez = arch_el[i_a]->begin();
	    if( !arch.empty() )	break;
	}
    return rez;
}

void TVArchive::setValType( TFld::Type vl )
{
    m_vtype = vl;
    setUpBuf( );
    modif( );
}

void TVArchive::setHardGrid( bool vl )
{
    m_bhgrd = vl;
    setUpBuf( );
    modif( );
}

void TVArchive::setHighResTm( bool vl )
{
    m_bhres = vl;
    setUpBuf( );
    modif( );
}

void TVArchive::setSize( int vl )
{
    m_bsize = vmax(10,vmin(10000000,vl));
    setUpBuf( );
    modif( );
}

void TVArchive::setPeriod( long long vl )
{
    if( !vl )	vl = 1000000;
    m_bper = (double)vl/1000000.;
    m_bsize = vmax(10,(int)vmin(1e7,100.0/m_bper));
    setUpBuf( );
    modif( );
}

void TVArchive::setUpBuf( )
{
    makeBuf( (TFld::Type)m_vtype, m_bsize, (long long)(m_bper*1000000.), m_bhgrd, m_bhres );
}

void TVArchive::load_( )
{
    SYS->db().at().dataGet(fullDB(),owner().nodePath()+tbl(),*this);
    setUpBuf();
}

void TVArchive::save_( )
{
    //- Update Archivators list -
    if( startStat() )
    {
	vector<string> arch_ls;
	archivatorList(arch_ls);
	m_archs = "";
	for( int i_l = 0; i_l < arch_ls.size(); i_l++ )
	    m_archs += arch_ls[i_l]+";";
    }

    SYS->db().at().dataSet(fullDB(),owner().nodePath()+tbl(),*this);
}

void TVArchive::start( )
{
    if( run_st ) return;

    run_st = true;

    setSrcMode((TVArchive::SrcMode)m_srcmode,m_dsourc);

    //- Attach to archivators -
    string arch;
    for( int i_off = 0; (arch = TSYS::strSepParse(m_archs,0,';',&i_off)).size();  )
	if(!archivatorPresent(arch))
	    try{ archivatorAttach(arch); }
	    catch(TError err)	{ mess_err(err.cat.c_str(),"%s",err.mess.c_str()); }
}

void TVArchive::stop( bool full_del )
{
    if( !run_st )	return;

    run_st = false;

    setSrcMode((TVArchive::SrcMode)m_srcmode,m_dsourc);

    //- Detach all archivators -
    vector<string> arch_ls;
    archivatorList(arch_ls);
    for( int i_l = 0; i_l < arch_ls.size(); i_l++ )
	archivatorDetach(arch_ls[i_l],full_del);
}

void TVArchive::setSrcMode( SrcMode vl, const string &isrc )
{
    //- Disable all links -
    if( (!run_st || vl != ActiveAttr || isrc != m_dsourc) && !pattr_src.freeStat() )
    {
	owner().setActValArch( id(), false );
	pattr_src.free();
	dynamic_cast<TVal&>(SYS->nodeAt(m_dsourc,0,'.').at()).setArch(AutoHD<TVArchive>());
    }

    try
    {
	if( (!run_st || vl != PassiveAttr || isrc != m_dsourc) &&
		dynamic_cast<TVal *>(&SYS->nodeAt(m_dsourc,0,'.').at()) )
	    dynamic_cast<TVal&>(SYS->nodeAt(m_dsourc,0,'.').at()).setArch(AutoHD<TVArchive>());
    }catch(...){  }

    //- Set all links -
    if( run_st && vl == ActiveAttr && dynamic_cast<TVal *>(&SYS->nodeAt(isrc,0,'.').at()) )
    {
	dynamic_cast<TVal&>(SYS->nodeAt(isrc,0,'.').at()).setArch(AutoHD<TVArchive>(this));
	pattr_src = SYS->nodeAt(isrc,0,'.');
	owner().setActValArch( id(), true );
    }

    if( run_st && vl == PassiveAttr && dynamic_cast<TVal *>(&SYS->nodeAt(isrc,0,'.').at()) )
	dynamic_cast<TVal&>(SYS->nodeAt(isrc,0,'.').at()).setArch(AutoHD<TVArchive>(this));

    if( m_srcmode != vl || m_dsourc != isrc )	modif();

    m_srcmode = vl;
    m_dsourc = isrc;
}

string TVArchive::getS( long long *tm, bool up_ord, const string &arch )
{
    //- Get from buffer -
    if( (arch.empty() || arch == BUF_ARCH_NM) && (!tm || (*tm >= begin() && *tm <= end())) )
	return TValBuf::getS(tm,up_ord);
    //- Get from archivators -
    else
    {
	ResAlloc res(a_res,false);
	for( int i_a = 0; i_a < arch_el.size(); i_a++ )
	    if( (arch.empty() || arch == arch_el[i_a]->archivator().workId()) &&
		    (!tm || (*tm >= arch_el[i_a]->begin() && *tm <= arch_el[i_a]->end())) )
		return arch_el[i_a]->getS(tm,up_ord);
    }
    return EVAL_STR;
}

double TVArchive::getR( long long *tm, bool up_ord, const string &arch )
{
    //- Get from buffer -
    if( (arch.empty() || arch == BUF_ARCH_NM) && (!tm || (*tm >= begin() && *tm <= end())) )
	return TValBuf::getR(tm,up_ord);
    //- Get from archivators -
    else
    {
	ResAlloc res(a_res,false);
	for( int i_a = 0; i_a < arch_el.size(); i_a++ )
	    if( (arch.empty() || arch == arch_el[i_a]->archivator().workId()) &&
		    (!tm || (*tm >= arch_el[i_a]->begin() && *tm <= arch_el[i_a]->end())) )
		return arch_el[i_a]->getR(tm,up_ord);
    }
    return EVAL_REAL;
}

int TVArchive::getI( long long *tm, bool up_ord, const string &arch )
{
    //- Get from buffer -
    if( (arch.empty() || arch == BUF_ARCH_NM) && (!tm || (*tm >= begin() && *tm <= end())) )
	return TValBuf::getI(tm,up_ord);
    //- Get from archivators -
    else
    {
	ResAlloc res(a_res,false);
	for( int i_a = 0; i_a < arch_el.size(); i_a++ )
	    if( (arch.empty() || arch == arch_el[i_a]->archivator().workId()) &&
		    (!tm || (*tm >= arch_el[i_a]->begin() && *tm <= arch_el[i_a]->end())) )
		return arch_el[i_a]->getI(tm,up_ord);
    }
    return EVAL_INT;
}

char TVArchive::getB( long long *tm, bool up_ord, const string &arch )
{
    //- Get from buffer -
    if( (arch.empty() || arch == BUF_ARCH_NM) && (!tm || (*tm >= begin() && *tm <= end())) )
        return TValBuf::getB(tm,up_ord);
    //- Get from archivators -
    else
    {
	ResAlloc res(a_res,false);
	for( int i_a = 0; i_a < arch_el.size(); i_a++ )
	    if( (arch.empty() || arch == arch_el[i_a]->archivator().workId()) &&
		    (!tm || (*tm >= arch_el[i_a]->begin() && *tm <= arch_el[i_a]->end())) )
		return arch_el[i_a]->getB(tm,up_ord);
    }
    return EVAL_BOOL;
}

void TVArchive::getVal( TValBuf &buf, long long ibeg, long long iend, const string &arch, int limit )
{
    //- Get from buffer -
    if( (arch.empty() || arch == BUF_ARCH_NM) && vOK(ibeg,iend) )
    {
	ibeg = vmax(ibeg,iend-TValBuf::period()*limit);
	TValBuf::getVal(buf,ibeg,iend);
	iend = buf.begin()-1;
    }
    //- Get from archivators -
    ResAlloc res(a_res,false);
    for( int i_a = 0; i_a < arch_el.size(); i_a++ )
	if( (arch.empty() || arch == arch_el[i_a]->archivator().workId()) &&
		((!ibeg || ibeg <= arch_el[i_a]->end()) && (!iend || iend > arch_el[i_a]->begin())) && ibeg <= iend )
	{
	    ibeg = vmax(ibeg,iend-(long long)(1000000.*arch_el[i_a]->archivator().valPeriod())*(limit-buf.realSize()));
	    arch_el[i_a]->getVal(buf,ibeg,iend);
	    iend = buf.begin()-1;
	}
}

void TVArchive::setVal( TValBuf &buf, long long ibeg, long long iend, const string &arch )
{
    //- Put to archivators -
    ResAlloc res(a_res,false);
    for( int i_a = 0; i_a < arch_el.size(); i_a++ )
	if( (arch.empty() || arch == arch_el[i_a]->archivator().workId()) )
	    arch_el[i_a]->setVal(buf,ibeg,iend);
}

void TVArchive::getActiveData()
{
    long long tm = TSYS::curTime();
    if( !pattr_src.freeStat() )
    {
	switch(valType())
	{
	    case TFld::Boolean:	setB(pattr_src.at().getB(&tm),tm); break;
	    case TFld::Integer:	setI(pattr_src.at().getI(&tm),tm); break;
	    case TFld::Real:	setR(pattr_src.at().getR(&tm),tm); break;
	    case TFld::String:	setS(pattr_src.at().getS(&tm),tm); break;
	}
    }
}

void TVArchive::archivatorList( vector<string> &ls )
{
    ResAlloc res(a_res,false);
    ls.clear();
    for( int i_l = 0; i_l < arch_el.size(); i_l++ )
	ls.push_back(arch_el[i_l]->archivator().workId());
}

bool TVArchive::archivatorPresent( const string &arch )
{
    ResAlloc res(a_res,false);
    try
    {
	AutoHD<TVArchivator> archivat = owner().at(TSYS::strSepParse(arch,0,'.')).at().
						valAt(TSYS::strSepParse(arch,1,'.'));
	for( int i_l = 0; i_l < arch_el.size(); i_l++ )
	    if( &arch_el[i_l]->archivator() == &archivat.at() )
		return true;
    } catch(TError err){ }
    return false;
}

void TVArchive::archivatorAttach( const string &arch )
{
    ResAlloc res(a_res,true);

    AutoHD<TVArchivator> archivat = owner().at(TSYS::strSepParse(arch,0,'.')).at().
					    valAt(TSYS::strSepParse(arch,1,'.'));

    if(!archivat.at().startStat())
	throw TError(nodePath().c_str(),"Archivator <%s> error or no started.",arch.c_str());

    //- Find already present archivator -
    for( int i_l = 0; i_l < arch_el.size(); i_l++ )
        if( &arch_el[i_l]->archivator() == &archivat.at() )
	    return;
    //- Find position -
    for( int i_l = 0; i_l < arch_el.size(); i_l++ )
        if( archivat.at().valPeriod() <= arch_el[i_l]->archivator().valPeriod() )
	{
	    arch_el.insert(arch_el.begin()+i_l,archivat.at().archivePlace(*this));
	    return;
	}
    arch_el.push_back(archivat.at().archivePlace(*this));
}

void TVArchive::archivatorDetach( const string &arch, bool full )
{
    ResAlloc res(a_res,true);

    AutoHD<TVArchivator> archivat = owner().at(TSYS::strSepParse(arch,0,'.')).at().
					    valAt(TSYS::strSepParse(arch,1,'.'));
    //- Find archivator -
    for( int i_l = 0; i_l < arch_el.size(); i_l++ )
        if( &arch_el[i_l]->archivator() == &archivat.at() )
	{
	    archivat.at().archiveRemove(id(),full);
	    arch_el.erase(arch_el.begin()+i_l);
	}
}

void TVArchive::archivatorSort()
{
    int rep_try;

    ResAlloc res(a_res,true);

    do
    {
	rep_try = false;
	for( int i_l = 1; i_l < arch_el.size(); i_l++ )
	    if( arch_el[i_l-1]->archivator().valPeriod() > arch_el[i_l]->archivator().valPeriod() )
	    {
		TVArchEl *t_el = arch_el[i_l-1];
		arch_el[i_l-1] = arch_el[i_l];
		arch_el[i_l] = t_el;
		rep_try=true;
	    }
    }
    while(rep_try);
}

string TVArchive::makeTrendImg( long long ibeg, long long iend, const string &iarch, int hsz, int vsz, double valmax, double valmin )
{
    char c_buf[30], c_buf1[30];
    time_t tm_t;
    struct tm ttm, ttm1;
    long long c_tm;
    string rez;
    int hv_border,		//Image border size
	h_w_start, h_w_size,	//Trend window horizontal start and size
	h_border_serv,		//Horizontal service border size
	v_w_start, v_w_size,	//Trend window vertical start and size
	v_border_serv;		//Vertical service border size

    //- Check and get data -
    if( ibeg >= iend || valType( ) == TFld::String )	return rez;
    TValBuf buf( TFld::Real, 0, 0, false, true );

    //- Calc base image parameters -
    hv_border = 5;
    h_border_serv = 30;
    v_border_serv = 20;
    h_w_start = hv_border;
    h_w_size  = hsz-h_w_start-hv_border;
    v_w_start = hv_border;
    v_w_size  = vsz-v_w_start-v_border_serv-hv_border;

    //- Create image -
    gdImagePtr im = gdImageCreate(hsz,vsz);
    int clr_backgr = gdImageColorAllocate(im,0x35,0x35,0x35); 
    int clr_grid   = gdImageColorAllocate(im,0x8e,0x8e,0x8e);
    int clr_symb   = gdImageColorAllocate(im,0x11,0xff,0x5f);
    int clr_trnd   = gdImageColorAllocate(im,0x1f,0xf2,0xff);

    gdImageFilledRectangle(im,0,0,hsz-1,vsz-1,clr_backgr);
    gdImageRectangle(im,h_w_start,v_w_start,h_w_start+h_w_size,v_w_start+v_w_size,clr_grid);

    //-- Make horisontal grid and symbols --
    //--- Calc horizontal scale ---
    long long h_div = 1, hDivBase = 1;
    long long h_min = ibeg;
    long long h_max = iend;
    int hmax_ln = vsz/40;
    if( hmax_ln >= 2 )
    {
	int hvLev = 0;
	long long hLen = iend - ibeg;
	if( hLen/86400000000ll >= 2 )		{ hvLev = 5; hDivBase = h_div = 86400000000ll; }	//Days
	else if( hLen/3600000000ll >= 2 )	{ hvLev = 4; hDivBase = h_div =  3600000000ll; }	//Hours
	else if( hLen/60000000 >= 2 )		{ hvLev = 3; hDivBase = h_div =    60000000; }		//Minutes
	else if( hLen/1000000 >= 2 )		{ hvLev = 2; hDivBase = h_div =     1000000; }		//Seconds
	else if( hLen/1000 >= 2 )		{ hvLev = 1; hDivBase = h_div =        1000; }		//Milliseconds
	while( hLen/h_div > hmax_ln )		h_div *= 10;
	while( hLen/h_div < hmax_ln/2 )		h_div /= 2;

	//--- Select most like archivator ---
	string rarch = iarch;
	if(rarch.empty() && !vOK(ibeg,iend))
	{
	    double best_a_per = 0;
	    ResAlloc res(a_res,false);
	    for( int i_a = 0; i_a < arch_el.size(); i_a++ )
		if( arch_el[i_a]->archivator().valPeriod() > best_a_per &&
		    arch_el[i_a]->archivator().valPeriod() <= (double)(h_max-h_min)/(1e5*hsz) )
		{
		    best_a_per = arch_el[i_a]->archivator().valPeriod();
		    rarch = arch_el[i_a]->archivator().workId();
		}
	}

	getVal(buf,h_min,h_max,rarch,600000);
	if(!buf.end() || !buf.begin())	{ gdImageDestroy(im); return rez; }

	//---- Draw full trend's data and time to the trend end position ----
	tm_t = iend/1000000;
	localtime_r(&tm_t,&ttm);
	snprintf(c_buf,sizeof(c_buf),"%d-%02d-%d",ttm.tm_mday,ttm.tm_mon+1,ttm.tm_year+1900);
	if( ttm.tm_sec == 0 && iend%1000000 == 0 )
	    snprintf(c_buf1,sizeof(c_buf1),"%d:%02d",ttm.tm_hour,ttm.tm_min);
	else if( iend%1000000 == 0 )
	    snprintf(c_buf1,sizeof(c_buf1),"%d:%02d:%02d",ttm.tm_hour,ttm.tm_min,ttm.tm_sec);
	else snprintf(c_buf1,sizeof(c_buf1),"%d:%02d:%g",ttm.tm_hour,ttm.tm_min,(float)ttm.tm_sec+(float)(iend%1000000)/1e6);
	gdImageString(im,gdFontTiny,h_w_start+h_w_size-gdFontTiny->w*strlen(c_buf),v_w_start+v_w_size+3+gdFontTiny->h,(unsigned char *)c_buf,clr_symb);
	gdImageString(im,gdFontTiny,h_w_start+h_w_size-gdFontTiny->w*strlen(c_buf1),v_w_start+v_w_size+3,(unsigned char *)c_buf1,clr_symb);
	int begMarkBrd = -1;
	int endMarkBrd = vmin(h_w_start+h_w_size-gdFontTiny->w*strlen(c_buf),h_w_start+h_w_size-gdFontTiny->w*strlen(c_buf1));

	//---- Draw grid and/or markers ----
	bool first_m = true;
	for( long long i_h = h_min; true; )
	{
	    //---- Draw grid ----
	    int h_pos = h_w_start+h_w_size*(i_h-h_min)/(h_max-h_min);
	    gdImageLine(im,h_pos,v_w_start,h_pos,v_w_start+v_w_size,clr_grid);

	    if( !(i_h%h_div) && i_h != iend )
	    {
		tm_t = i_h/1000000;
		localtime_r(&tm_t,&ttm);
		int chLev = -1;
		if( !first_m )
		{
		    if( ttm.tm_mon > ttm1.tm_mon || ttm.tm_year > ttm1.tm_year )  chLev = 5;
		    else if( ttm.tm_mday > ttm1.tm_mday )  chLev = 4;
		    else if( ttm.tm_hour > ttm1.tm_hour )  chLev = 3;
		    else if( ttm.tm_min > ttm1.tm_min )    chLev = 2;
		    else if( ttm.tm_sec > ttm1.tm_sec )    chLev = 1;
		    else chLev = 0;
		}
		c_buf[0] = c_buf1[0] = 0;
		if( hvLev == 5 || chLev >= 4 )                                      //Date
		    (chLev>=5 || chLev==-1) ? snprintf(c_buf,sizeof(c_buf),"%d-%02d-%d",ttm.tm_mday,ttm.tm_mon+1,ttm.tm_year+1900) :
				snprintf(c_buf,sizeof(c_buf),"%d",ttm.tm_mday);
		if( (hvLev == 4 || hvLev == 3 || ttm.tm_min) && !ttm.tm_sec )     //Hours and minuts
		    snprintf(c_buf1,sizeof(c_buf1),"%d:%02d",ttm.tm_hour,ttm.tm_min);
		else if( (hvLev == 2 || ttm.tm_sec) && !(i_h%1000000) )            //Seconds
		    (chLev>=2 || chLev==-1) ? snprintf(c_buf1,sizeof(c_buf1),"%d:%02d:%02d",ttm.tm_hour,ttm.tm_min,ttm.tm_sec) :
				snprintf(c_buf1,sizeof(c_buf1),"%ds",ttm.tm_sec);
		else if( hvLev <= 1 || i_h%1000000 )                                //Milliseconds
		    (chLev>=2 || chLev==-1) ? snprintf(c_buf1,sizeof(c_buf1),"%d:%02d:%g",ttm.tm_hour,ttm.tm_min,(float)ttm.tm_sec+(float)(i_h%1000000)/1e6) :
		    (chLev>=1) ? snprintf(c_buf1,sizeof(c_buf1),"%gs",(float)ttm.tm_sec+(float)(i_h%1000000)/1e6) :
				snprintf(c_buf1,sizeof(c_buf1),"%gms",(double)(i_h%1000000)/1000.);
		int wdth, tpos, endPosTm = 0, endPosDt = 0;
		if( c_buf[0] )
		{
		    wdth = gdFontTiny->w*strlen(c_buf);
		    tpos = vmax(h_pos-wdth/2,hv_border);
		    if( (tpos+wdth) < endMarkBrd && tpos > begMarkBrd )
		    {
			gdImageString(im,gdFontTiny,tpos,v_w_start+v_w_size+3+gdFontTiny->h,(unsigned char *)c_buf,clr_symb);
			endPosTm = tpos+wdth;
		    }
		}
		if( c_buf1[0] )
		{
		    wdth = gdFontTiny->w*strlen(c_buf1);
		    tpos = vmax(h_pos-wdth/2,hv_border);
		    if( (tpos+wdth) < endMarkBrd && tpos > begMarkBrd )
		    {
			gdImageString(im,gdFontTiny,tpos,v_w_start+v_w_size+3,(unsigned char *)c_buf1,clr_symb);
			endPosDt = tpos+wdth;
		    }
		}
		begMarkBrd = vmax(begMarkBrd,vmax(endPosTm,endPosDt));
		memcpy((char*)&ttm1,(char*)&ttm,sizeof(tm));
		first_m = false;
	    }
	    //---- Next ----
	    if( i_h >= iend )	break;
	    i_h = (i_h/h_div)*h_div + h_div;
	    if( i_h > iend )	i_h = iend;
	}
    }

    //-- Make vertical grid and symbols --
    //--- Calc vertical scale ---
    double	c_val,
		v_max = -3e300,
		v_min = 3e300;
    for(c_tm = buf.begin(); c_tm <= buf.end(); c_tm++)
    {
	c_val = buf.getR(&c_tm,true);
	if(c_val == EVAL_REAL)	continue;
	v_min = vmin(v_min,c_val);
	v_max = vmax(v_max,c_val);
    }
    if(valmax!=valmin)
    {
	v_max = vmax(v_max,valmax);
	v_min = vmin(v_min,valmin);
    }
    if( v_max==-3e300 )	{ gdImageDestroy(im); return rez; }
    if( TSYS::realRound(v_max,3) == TSYS::realRound(v_min,3) )	{ v_max+=1.0; v_min-=1.0; }
    double v_div = 1.;
    double v_len = v_max - v_min;
    while(v_len>=10.)	{ v_div*=10.; v_len/=10.; }
    while(v_len<1.) 	{ v_div/=10.; v_len*=10.; }
    v_min = floor(v_min/v_div)*v_div;
    v_max = ceil(v_max/v_div)*v_div;
    while(((v_max-v_min)/v_div)<5) v_div/=2;
    //--- Draw vertical grid and symbols ---
    for(double i_v = v_min; i_v <= v_max; i_v+=v_div)
    {
	int v_pos = v_w_start+v_w_size-(int)((double)v_w_size*(i_v-v_min)/(v_max-v_min));
	gdImageLine(im,h_w_start,v_pos,h_w_start+h_w_size,v_pos,clr_grid);

	snprintf(c_buf,sizeof(c_buf),"%g",i_v);
	gdImageString(im,gdFontTiny,hv_border+2,v_pos-((i_v>=v_max)?0:gdFontTiny->h),(unsigned char *)c_buf,clr_symb);
    }

    //-- Draw trend --
    double aver_vl = EVAL_REAL;
    int    aver_pos= 0;
    long long aver_tm = 0;
    long long aver_lsttm = 0;
    double prev_vl = EVAL_REAL;
    int    prev_pos = 0;
    for( c_tm = buf.begin(); true; c_tm++ )
    {
	int c_pos;
	if(c_tm <= buf.end())
	{
	    c_val = buf.getR(&c_tm,true);
	    c_pos = h_w_start+h_w_size*(vmax(vmin(c_tm,h_max),h_min)-h_min)/(h_max-h_min);
	}else c_pos = 0;
	//Square Average
	if( aver_pos == c_pos )
	{
	    if( aver_vl == EVAL_REAL )	aver_vl = c_val;
	    else if( c_val != EVAL_REAL )
		aver_vl = (aver_vl*(double)(c_tm-aver_tm)+c_val*(double)(c_tm-aver_lsttm))/
			  ((double)(2*c_tm-aver_tm-aver_lsttm));
	    aver_lsttm = c_tm;
	    continue;
	}
	//Write point and line
	if( aver_vl != EVAL_REAL )
	{
	    int c_vpos = v_w_start+v_w_size-(int)((double)v_w_size*(aver_vl-v_min)/(v_max-v_min));
	    gdImageSetPixel(im,aver_pos,c_vpos,clr_trnd);
	    if( prev_vl != EVAL_REAL )
	    {
		int c_vpos_prv = v_w_start+v_w_size-(int)((double)v_w_size*(prev_vl-v_min)/(v_max-v_min));
		gdImageLine(im,prev_pos,c_vpos_prv,aver_pos,c_vpos,clr_trnd);
	    }
	}
	prev_vl  = aver_vl;
	prev_pos = aver_pos;
	aver_vl  = c_val;
	aver_pos = c_pos;
	aver_tm = aver_lsttm = c_tm;
	if( !c_pos ) break;
    }

    //- Get image and transfer it -
    int img_sz;
    char *img_ptr = (char *)gdImagePngPtr(im, &img_sz);
    rez.assign(img_ptr,img_sz);
    gdFree(img_ptr);
    gdImageDestroy(im);

    return rez;
}

void TVArchive::cntrCmdProc( XMLNode *opt )
{
    string a_path = opt->attr("path");
    //- Service commands process -
    if( a_path == "/serv/val" )		//Values access
    {
	if( ctrChkNode(opt,"info",RWRWRW,"root","root",SEQ_RD) )	//Value's data information
	{
	    string arch = opt->attr("arch");
	    opt->setAttr("end",TSYS::ll2str(end(arch)));
	    opt->setAttr("beg",TSYS::ll2str(begin(arch)));
	    opt->setAttr("vtp",TSYS::int2str(TValBuf::valType()));
	    if( arch.empty() || arch == BUF_ARCH_NM )
		opt->setAttr("per",TSYS::ll2str(TValBuf::period()));
	    else
	    {
		ResAlloc res(a_res,false);
		for( int i_a = 0; i_a < arch_el.size(); i_a++ )
		    if( arch == arch_el[i_a]->archivator().workId() )
		    { opt->setAttr("per",TSYS::ll2str((long long)(1000000.*arch_el[i_a]->archivator().valPeriod()))); break;  }
	    }
	}
	else if( ctrChkNode(opt,"get",RWRWRW,"root","root",SEQ_RD) )	//Value's data request
	{
	    long long tm      = atoll(opt->attr("tm").c_str());
	    long long tm_grnd = atoll(opt->attr("tm_grnd").c_str());
	    string    arch    = opt->attr("arch");

	    //-- Process one value request --
	    if( !tm )	tm = TSYS::curTime();
	    if( !tm_grnd )
	    {
		opt->setText(getS(&tm,false,arch));
		opt->setAttr("tm",TSYS::ll2str(tm));
		return;
	    }
	    if( tm < tm_grnd )	throw TError(nodePath().c_str(),"Range error");

	    //-- Process of archive block request --
	    TValBuf buf( TValBuf::valType(), 100000, TValBuf::period(), true, true );

	    //--- Get values buffer ---
	    if( (arch.empty() || arch == BUF_ARCH_NM) && vOK(tm_grnd,tm) )
	    {
		TValBuf::getVal(buf,tm_grnd,tm);
		opt->setAttr("arch",BUF_ARCH_NM);
	    }
	    else
	    {
		ResAlloc res( a_res, false );
		for( int i_a = 0; i_a < arch_el.size(); i_a++ )
		    if( (arch.empty() || arch == arch_el[i_a]->archivator().workId()) &&
			(tm_grnd <= arch_el[i_a]->end() && tm > arch_el[i_a]->begin()) )
		    {
			buf.setPeriod( (long long)(1000000.*arch_el[i_a]->archivator().valPeriod()) );
			arch_el[i_a]->getVal( buf, tm_grnd, tm );
			opt->setAttr("arch",arch_el[i_a]->archivator().workId());
			break;
		    }
		res.release();
	    }
	    //--- Prepare buffer's data for transfer ---
	    bool    isEnd = false;	//Last archive value process
	    string  text;
	    text.reserve(100);
	    int vpos_beg = 0, vpos_end = 0, vpos_cur;
	    long long ibeg = buf.begin(), iend = buf.end(), period = vmax(atoll(opt->attr("per").c_str()),buf.period());
	    int mode = atoi(opt->attr("mode").c_str());
	    if( mode < 0 || mode > 2 ) throw TError(nodePath().c_str(),"No support data mode '%d'",mode);
	    switch(buf.valType())
	    {
		case TFld::Boolean:
		{
		    char tval_pr, tval_pr1;
		    while( ibeg <= iend )
		    {
			char tval = buf.getB(&ibeg,true);
			vpos_cur = (ibeg-buf.begin())/period;
			if( vpos_cur > vpos_end )
			    bool1: switch(mode)
			    {
				case 0: text+=TSYS::int2str(tval_pr)+"\n";	break;
				case 1:
				    if( !vpos_end || tval_pr != tval_pr1 )
					text+=TSYS::int2str(vpos_end)+" "+TSYS::int2str(tval_pr)+"\n";
				    tval_pr1 = tval_pr;	
				    break;
				case 2: text+=tval_pr;	break;
			    }
			tval_pr = tval;
			vpos_end = vpos_cur;
			ibeg++;
			if( isEnd )	break;
			if( ibeg > iend )	{ isEnd = true;	goto bool1; }	//Flush last value
		    }
		    break;
		}
		case TFld::Integer:
		{
		    float round_perc = atof(opt->attr("round_perc").c_str());
		    int tval_pr = EVAL_INT, tval_pr1;
		    while( ibeg <= iend )
		    {
			int tval = buf.getI(&ibeg,true);
			vpos_cur = (ibeg-buf.begin())/period;
			if( vpos_cur <= vpos_end )
			{
			    if( tval == EVAL_INT ) tval = tval_pr;
			    if( tval != EVAL_INT && tval_pr != EVAL_INT )
			    {
				int s_k = ibeg-period*(ibeg/period);
				int n_k = buf.period();
				tval = ((long long)tval_pr*s_k+(long long)tval*n_k)/(s_k+n_k);
			    }
			}
			else int1: switch(mode)
			{
			    case 0: text+=TSYS::int2str(tval_pr)+"\n";	break;
			    case 1: 
				if( !(vpos_end && ( tval_pr==tval_pr1 || (((tval_pr1 > 0 && tval_pr > 0) || (tval_pr1 < 0 && tval_pr < 0)) &&
					100.*(double)abs(tval_pr1-tval_pr)/(double)vmax(abs(tval_pr1),abs(tval_pr)) <= round_perc))) )
				{
				    text+=TSYS::int2str(vpos_end)+" "+TSYS::int2str(tval_pr)+"\n";
				    tval_pr1 = tval_pr;
				}
				break;
			    case 2: text+=string((char*)&tval_pr,sizeof(int)); break;
			}
			tval_pr = tval;
			vpos_end = vpos_cur;
			ibeg++;
			if( isEnd )	break;
			if( ibeg > iend )	{ isEnd = true;	goto int1; }	//Flush last value
		    }
		    break;
		}
		case TFld::Real:
		{
		    float round_perc = atof(opt->attr("round_perc").c_str());
		    int   real_prec  = atoi(opt->attr("real_prec").c_str());
		    if( !real_prec ) real_prec = 10;
		    double tval_pr = EVAL_REAL, tval_pr1;
		    while( ibeg <= iend )
		    {
			double tval = buf.getR(&ibeg,true);
			vpos_cur = (ibeg-buf.begin())/period;
			if( vpos_cur <= vpos_end )
			{	
			    if( tval == EVAL_REAL ) tval = tval_pr;
			    if( tval != EVAL_REAL && tval_pr != EVAL_REAL )
			    {
				int s_k = ibeg-period*(ibeg/period);
				int n_k = buf.period();
				tval = (tval_pr*s_k+tval*n_k)/(s_k+n_k);
			    }
			}
			else real1: switch(mode)
			{
			    case 0: text+=TSYS::real2str(tval_pr,real_prec)+"\n";	break;
			    case 1:
				if( !(vpos_end && ( tval_pr==tval_pr1 || (((tval_pr1 > 0 && tval_pr > 0) || (tval_pr1 < 0 && tval_pr < 0)) &&
					100.*fabs(tval_pr1-tval_pr)/vmax(fabs(tval_pr1),fabs(tval_pr)) <= round_perc))) )
				{
				    text+=TSYS::int2str(vpos_end)+" "+TSYS::real2str(tval_pr,real_prec)+"\n";
				    tval_pr1 = tval_pr;
				}
				break;
			    case 2: text+=string((char*)&tval_pr,sizeof(double));	break;
			}
			tval_pr = tval;
			vpos_end = vpos_cur;
			ibeg++;
			if( isEnd )	break;
			if( ibeg > iend )	{ isEnd = true;	goto real1; }	//Flush last value
		    }
		    break;
		}
		case TFld::String:
		{
		    string tval_pr = EVAL_STR, tval_pr1;
		    while( ibeg <= iend )
		    {
			string tval = buf.getS(&ibeg,true);
			vpos_cur = (ibeg-buf.begin())/period;
			if( vpos_cur > vpos_end )
			    str1: switch(mode)
			    {
				case 0: text+=TSYS::strEncode(tval_pr,TSYS::Custom,"\n")+"\n";	break;
				case 1:
				    if( !vpos_end || tval_pr != tval_pr1 ) 
					text+=TSYS::int2str(vpos_end)+" "+TSYS::strEncode(tval_pr,TSYS::Custom,"\n")+"\n";
				    tval_pr1 = tval_pr;
				    break;
				case 2: throw TError(nodePath().c_str(),"Binary mode no support for string data");
			    }
			tval_pr = tval;
			vpos_end = vpos_cur;
			ibeg++;
			if( isEnd )	break;
			if( ibeg > iend )	{ isEnd = true;	goto str1; }	//Flush last value
		    }
		    break;
		}
	    }
	    //--- Set result and information attributes ---
	    opt->setAttr("tm",TSYS::ll2str( (buf.end()/period)*period ));
	    opt->setAttr("tm_grnd",TSYS::ll2str( (buf.begin()/period)*period + ((buf.begin()%period)?period:0) ));
	    opt->setAttr("per",TSYS::ll2str(period));
	    opt->setText( (mode==2) ? TSYS::strEncode(text,TSYS::base64) : text);
	}
	return;
    }

    //- Interface comands process -
    //-- Info command process --
    if( opt->name() == "info" )
    {
	ctrMkNode("oscada_cntr",opt,-1,"/",_("Value archive: ")+name(),0664,"root","Archive");
	if(ctrMkNode("area",opt,-1,"/prm",_("Archive")))
	{
	    if(ctrMkNode("area",opt,-1,"/prm/st",_("State")))
	    {
		ctrMkNode("fld",opt,-1,"/prm/st/st",_("Runing"),0664,"root","Archive",1,"tp","bool");
		ctrMkNode("fld",opt,-1,"/prm/st/db",_("Archive DB"),0664,"root","Archive",4,"tp","str","dest","select","select","/db/list",
		    "help",_("DB address in format [<DB module>.<DB name>].\nFor use main work DB set '*.*'."));
	    }
	    if(ctrMkNode("area",opt,-1,"/prm/cfg",_("Config")))
	    {
		ctrMkNode("fld",opt,-1,"/prm/cfg/id",cfg("ID").fld().descr(),0444,"root","Archive",1,"tp","str");
		ctrMkNode("fld",opt,-1,"/prm/cfg/nm",cfg("NAME").fld().descr(),0664,"root","Archive",1,"tp","str");
		ctrMkNode("fld",opt,-1,"/prm/cfg/dscr",cfg("DESCR").fld().descr(),0664,"root","Archive",3,"tp","str","cols","50","rows","3");
		ctrMkNode("fld",opt,-1,"/prm/cfg/start",_("To start"),0664,"root","Archive",1,"tp","bool");
		ctrMkNode("fld",opt,-1,"/prm/cfg/vtp",cfg("VTYPE").fld().descr(),0664,"root","Archive",3,"tp","dec","dest","select","select","/cfg/vtp_ls");
		ctrMkNode("fld",opt,-1,"/prm/cfg/srcm",cfg("Source").fld().descr(),0664,"root","Archive",3,"tp","dec","dest","select","select","/cfg/srcm_ls");
		if( srcMode() == PassiveAttr || srcMode() == ActiveAttr )
		    ctrMkNode("fld",opt,-1,"/prm/cfg/src","",0664,"root","Archive",3,"tp","str","dest","sel_ed","select","/cfg/prm_atr_ls");
		ctrMkNode("fld",opt,-1,"/prm/cfg/b_per",cfg("BPER").fld().descr(),0664,"root","Archive",1,"tp","real");
		ctrMkNode("fld",opt,-1,"/prm/cfg/b_size",cfg("BSIZE").fld().descr(),0664,"root","Archive",1,"tp","dec");
		ctrMkNode("fld",opt,-1,"/prm/cfg/b_hgrd",cfg("BHGRD").fld().descr(),0664,"root","Archive",1,"tp","bool");
		ctrMkNode("fld",opt,-1,"/prm/cfg/b_hres",cfg("BHRES").fld().descr(),0664,"root","Archive",1,"tp","bool");
	    }
	}
	if(ctrMkNode("area",opt,-1,"/arch",_("Archivators"),0444,"root","Archive"))
	{
	    if(ctrMkNode("table",opt,-1,"/arch/arch",_("Archivators"),0664,"root","root",1,"key","arch"))
	    {
		ctrMkNode("list",opt,-1,"/arch/arch/arch",_("Archivator"),0444,"root","root",1,"tp","str");
		ctrMkNode("list",opt,-1,"/arch/arch/start",_("Start"),0444,"root","root",1,"tp","bool");
		ctrMkNode("list",opt,-1,"/arch/arch/proc",_("Process"),0664,"root","root",1,"tp","bool");
		ctrMkNode("list",opt,-1,"/arch/arch/per",_("Period (s)"),0444,"root","root",1,"tp","real");
		ctrMkNode("list",opt,-1,"/arch/arch/beg",_("Begin"),0444,"root","root",1,"tp","str");
		ctrMkNode("list",opt,-1,"/arch/arch/end",_("End"),0444,"root","root",1,"tp","str");
	    }
	}
	if( run_st && ctrMkNode("area",opt,-1,"/val",_("Values"),0440,"root","Archive") )
	{
	    ctrMkNode("fld",opt,-1,"/val/tm",_("Time"),0660,"root","Archive",1,"tp","time");
	    ctrMkNode("fld",opt,-1,"/val/utm","",0660,"root","Archive",4,"tp","dec","len","6","min","0","max","999999");
	    ctrMkNode("fld",opt,-1,"/val/size",_("Size"),0660,"root","Archive",1,"tp","real");
	    ctrMkNode("fld",opt,-1,"/val/arch",_("Archivator"),0660,"root","Archive",1,"tp","str");
	    ctrMkNode("fld",opt,-1,"/val/sw_trend",_("Show trend"),0660,"root","Archive",1,"tp","bool");
	    if(!atoi(TBDS::genDBGet(owner().nodePath()+"vShowTrnd","0",opt->attr("user")).c_str()))
	    {
		if(ctrMkNode("table",opt,-1,"/val/val",_("Values table"),0440,"root","Archive"))
		{
		    ctrMkNode("list",opt,-1,"/val/val/0",_("Time"),0440,"root","Archive",1,"tp","str");
		    ctrMkNode("list",opt,-1,"/val/val/1",_("Value"),0440,"root","Archive",1,"tp","str");
		}
	    }
	    else
	    {
		ctrMkNode("fld",opt,-1,"/val/pct_w",_("Picture size"),0660,"root","Archive",3,"tp","dec","min","100","max","1024");
		ctrMkNode("fld",opt,-1,"/val/pct_h","",0660,"root","Archive",3,"tp","dec","min","50","max","800");
		ctrMkNode("fld",opt,-1,"/val/max",_("Value scale"),0660,"root","Archive",1,"tp","dec");
		ctrMkNode("fld",opt,-1,"/val/min","",0660,"root","Archive",1,"tp","dec");
		ctrMkNode("img",opt,-1,"/val/trend",_("Values trend"),0440,"root","Archive");
	    }
	}
	return;
    }
    //-- Process command to page --
    if( a_path == "/prm/st/st" )
    {
	if( ctrChkNode(opt,"get",0664,"root","Archive",SEQ_RD) )	opt->setText( startStat() ? "1" : "0" );
	if( ctrChkNode(opt,"set",0664,"root","Archive",SEQ_WR) )	atoi(opt->text().c_str()) ? start() : stop();
    }
    else if( a_path == "/prm/st/db" )
    {
	if( ctrChkNode(opt,"get",0664,"root","Archive",SEQ_RD) )	opt->setText( DB() );
	if( ctrChkNode(opt,"set",0664,"root","Archive",SEQ_WR) )	setDB( opt->text() );
    }
    else if( a_path == "/prm/cfg/id" && ctrChkNode(opt) )		opt->setText(id());
    else if( a_path == "/prm/cfg/nm" )
    {
	if( ctrChkNode(opt,"get",0664,"root","Archive",SEQ_RD) )	opt->setText( name() );
	if( ctrChkNode(opt,"set",0664,"root","Archive",SEQ_WR) )	setName( opt->text() );
    }
    else if( a_path == "/prm/cfg/dscr" )
    {
	if( ctrChkNode(opt,"get",0664,"root","Archive",SEQ_RD) )	opt->setText( dscr() );
	if( ctrChkNode(opt,"set",0664,"root","Archive",SEQ_WR) )	setDscr( opt->text() );
    }
    else if( a_path == "/prm/cfg/start" )
    {
	if( ctrChkNode(opt,"get",0664,"root","Archive",SEQ_RD) )	opt->setText( toStart() ? "1" : "0" );
	if( ctrChkNode(opt,"set",0664,"root","Archive",SEQ_WR) )	setToStart( atoi(opt->text().c_str()) );
    }
    else if( a_path == "/prm/cfg/vtp" )
    {
	if( ctrChkNode(opt,"get",0664,"root","Archive",SEQ_RD) )	opt->setText(TSYS::int2str(m_vtype));
	if( ctrChkNode(opt,"set",0664,"root","Archive",SEQ_WR) )	setValType((TFld::Type)atoi(opt->text().c_str()));
    }
    else if( a_path == "/prm/cfg/srcm" )
    {
	if( ctrChkNode(opt,"get",0664,"root","Archive",SEQ_RD) )	opt->setText(TSYS::int2str(m_srcmode));
	if( ctrChkNode(opt,"set",0664,"root","Archive",SEQ_WR) )	setSrcMode((TVArchive::SrcMode)atoi(opt->text().c_str()),m_dsourc);
    }
    else if( a_path == "/prm/cfg/src" )
    {
	if( ctrChkNode(opt,"get",0664,"root","Archive",SEQ_RD) )	opt->setText(m_dsourc);
	if( ctrChkNode(opt,"set",0664,"root","Archive",SEQ_WR) )	setSrcMode((TVArchive::SrcMode)m_srcmode,opt->text());
    }
    else if( a_path == "/prm/cfg/b_per" )
    {
	if( ctrChkNode(opt,"get",0664,"root","Archive",SEQ_RD) )	opt->setText(TSYS::real2str(m_bper,6));
	if( ctrChkNode(opt,"set",0664,"root","Archive",SEQ_WR) )	setPeriod((long long)(1000000.*atof(opt->text().c_str())));
    }
    else if( a_path == "/prm/cfg/b_size" )
    {
	if( ctrChkNode(opt,"get",0664,"root","Archive",SEQ_RD) )	opt->setText(TSYS::int2str(m_bsize));
	if( ctrChkNode(opt,"set",0664,"root","Archive",SEQ_WR) )	setSize(atoi(opt->text().c_str()));
    }
    else if( a_path == "/prm/cfg/b_hgrd" )
    {
	if( ctrChkNode(opt,"get",0664,"root","Archive",SEQ_RD) )	opt->setText(m_bhgrd?"1":"0");
	if( ctrChkNode(opt,"set",0664,"root","Archive",SEQ_WR) )	setHardGrid(atoi(opt->text().c_str()));
    }
    else if( a_path == "/prm/cfg/b_hres" )
    {
	if( ctrChkNode(opt,"get",0664,"root","Archive",SEQ_RD) )	opt->setText(m_bhres?"1":"0");
	if( ctrChkNode(opt,"set",0664,"root","Archive",SEQ_WR) )	setHighResTm(atoi(opt->text().c_str()));
    }
    else if( a_path == "/cfg/vtp_ls" && ctrChkNode(opt) )
    {
	opt->childAdd("el")->setAttr("id",TSYS::int2str(TFld::Boolean))->setText(_("Boolean"));
	opt->childAdd("el")->setAttr("id",TSYS::int2str(TFld::Integer))->setText(_("Integer"));
	opt->childAdd("el")->setAttr("id",TSYS::int2str(TFld::Real))->setText(_("Real"));
	opt->childAdd("el")->setAttr("id",TSYS::int2str(TFld::String))->setText(_("String"));
    }
    else if( a_path == "/cfg/srcm_ls" && ctrChkNode(opt) )
    {
	opt->childAdd("el")->setAttr("id",TSYS::int2str(TVArchive::Passive))->setText(_("Passive"));
	opt->childAdd("el")->setAttr("id",TSYS::int2str(TVArchive::PassiveAttr))->setText(_("Passive param. atribute"));
	opt->childAdd("el")->setAttr("id",TSYS::int2str(TVArchive::ActiveAttr))->setText(_("Active param. atribute"));
    }
    else if( a_path == "/cfg/prm_atr_ls" && ctrChkNode(opt) )
    {
	vector<string> list;
	int c_lv = 0;
	string c_path = "", c_el;
	for( int c_off = 0; (c_el=TSYS::strSepParse(m_dsourc,0,'.',&c_off)).size(); c_lv++ )
	{
	    opt->childAdd("el")->setText(c_path);
	    if( c_lv ) c_path+=".";
	    c_path+=c_el;
	}
	opt->childAdd("el")->setText(c_path);
	if( c_lv != 0 ) c_path += ".";
	SYS->nodeAt(c_path,0,'.').at().nodeList(list);
	for( unsigned i_a=0; i_a < list.size(); i_a++ )
	    opt->childAdd("el")->setText(c_path+list[i_a]);
    }
    else if( a_path == "/arch/arch" )
    {
	if( ctrChkNode(opt,"get",0664,"root","Archive",SEQ_RD) )
	{
	    //Fill Archivators table
	    XMLNode *n_arch = ctrMkNode("list",opt,-1,"/arch/arch/arch","",0444);
	    XMLNode *n_start= ctrMkNode("list",opt,-1,"/arch/arch/start","",0444);
	    XMLNode *n_prc  = ctrMkNode("list",opt,-1,"/arch/arch/proc","",0664);
	    XMLNode *n_per  = ctrMkNode("list",opt,-1,"/arch/arch/per","",0444);
	    XMLNode *n_beg  = ctrMkNode("list",opt,-1,"/arch/arch/beg","",0444);
	    XMLNode *n_end  = ctrMkNode("list",opt,-1,"/arch/arch/end","",0444);

	    vector<string> t_arch_ls, arch_ls;
	    owner().modList(t_arch_ls);
	    for( int i_ta = 0; i_ta < t_arch_ls.size(); i_ta++ )
	    {
		owner().at(t_arch_ls[i_ta]).at().valList(arch_ls);
		for( int i_a = 0; i_a < arch_ls.size(); i_a++ )
		{
		    TVArchEl *a_el = NULL;
		    //Find attached
		    ResAlloc res(a_res,false);
		    for( int i_l = 0; i_l < arch_el.size(); i_l++ )
			if( arch_el[i_l]->archivator().owner().modId() == t_arch_ls[i_ta] && 
				arch_el[i_l]->archivator().id() == arch_ls[i_a] )
			    a_el = arch_el[i_l];
		    //Fill table element
		    if(n_arch)	n_arch->childAdd("el")->setText(owner().at(t_arch_ls[i_ta]).at().valAt(arch_ls[i_a]).at().workId());
		    if(n_start)	n_start->childAdd("el")->setText(owner().at(t_arch_ls[i_ta]).at().valAt(arch_ls[i_a]).at().startStat()?"1":"0");
		    if(n_per)	n_per->childAdd("el")->setText(TSYS::real2str(owner().at(t_arch_ls[i_ta]).at().valAt(arch_ls[i_a]).at().valPeriod(),6));
		    if(a_el)
		    {
			if(n_prc)	n_prc->childAdd("el")->setText("1");
			char c_buf[30];
			struct tm ttm;
			time_t tm_t = a_el->end()/1000000;
			localtime_r(&tm_t,&ttm);
			snprintf(c_buf,sizeof(c_buf),"%d-%02d-%d %d:%02d:%02d.%d",
			    ttm.tm_mday,ttm.tm_mon+1,ttm.tm_year+1900,ttm.tm_hour,ttm.tm_min,ttm.tm_sec,a_el->end()%1000000);
			if(n_end)	n_end->childAdd("el")->setText(c_buf);
			tm_t = a_el->begin()/1000000;
			localtime_r(&tm_t,&ttm);
			snprintf(c_buf,sizeof(c_buf),"%d-%02d-%d %d:%02d:%02d.%d",
			    ttm.tm_mday,ttm.tm_mon+1,ttm.tm_year+1900,ttm.tm_hour,ttm.tm_min,ttm.tm_sec,a_el->begin()%1000000);
			if(n_beg)	n_beg->childAdd("el")->setText(c_buf);
		    }
		    else
		    {
			if(n_prc)	n_prc->childAdd("el")->setText("0"); 
			if(n_end)	n_end->childAdd("el")->setText(""); 
			if(n_beg)	n_beg->childAdd("el")->setText("");
		    }
		}
	    }
	}
	if( ctrChkNode(opt,"set",0664,"root","Archive",SEQ_WR) )
	{
	    if( opt->attr("col") == "proc" )
	    {
		atoi(opt->text().c_str()) ? archivatorAttach( opt->attr("key_arch") ) :
					    archivatorDetach( opt->attr("key_arch"), true );
		modif();
	    }
	}
    }
    else if( a_path == "/val/tm" )
    {
	if( ctrChkNode(opt,"get",0660,"root","Archive",SEQ_RD) )
	{
	    opt->setText(TBDS::genDBGet(owner().nodePath()+"vaTm","0",opt->attr("user")));
	    if( !atoi(opt->text().c_str()) )	opt->setText(TSYS::int2str(time(NULL)));
	}
	if( ctrChkNode(opt,"set",0660,"root","Archive",SEQ_WR) )
	    TBDS::genDBSet(owner().nodePath()+"vaTm",(atoi(opt->text().c_str())>=time(NULL))?"0":opt->text(),opt->attr("user"));
    }
    else if( a_path == "/val/utm" )
    {
	if( ctrChkNode(opt,"get",0660,"root","Archive",SEQ_RD) )	opt->setText(TBDS::genDBGet(owner().nodePath()+"vaTm_u","0",opt->attr("user")));
	if( ctrChkNode(opt,"set",0660,"root","Archive",SEQ_WR) )	TBDS::genDBSet(owner().nodePath()+"vaTm_u",opt->text(),opt->attr("user"));
    }
    else if( a_path == "/val/size" )
    {
	if( ctrChkNode(opt,"get",0660,"root","Archive",SEQ_RD) )	opt->setText(TBDS::genDBGet(owner().nodePath()+"vaSize","1",opt->attr("user")));
	if( ctrChkNode(opt,"set",0660,"root","Archive",SEQ_WR) )	TBDS::genDBSet(owner().nodePath()+"vaSize",opt->text(),opt->attr("user"));
    }
    else if( a_path == "/val/arch")
    {
	if( ctrChkNode(opt,"get",0660,"root","Archive",SEQ_RD) )	opt->setText(TBDS::genDBGet(owner().nodePath()+"vArch","",opt->attr("user")));
	if( ctrChkNode(opt,"set",0660,"root","Archive",SEQ_WR) )	TBDS::genDBSet(owner().nodePath()+"vArch",opt->text(),opt->attr("user"));
    }
    else if( a_path == "/val/sw_trend" )
    {
	if( ctrChkNode(opt,"get",0660,"root","Archive",SEQ_RD) )	opt->setText(TBDS::genDBGet(owner().nodePath()+"vShowTrnd","0",opt->attr("user")));
	if( ctrChkNode(opt,"set",0660,"root","Archive",SEQ_WR) )	TBDS::genDBSet(owner().nodePath()+"vShowTrnd",opt->text(),opt->attr("user"));
    }
    else if( a_path == "/val/pct_w" )
    {
	if( ctrChkNode(opt,"get",0660,"root","Archive",SEQ_RD) )	opt->setText(TBDS::genDBGet(owner().nodePath()+"vPctW","650",opt->attr("user")));
	if( ctrChkNode(opt,"set",0660,"root","Archive",SEQ_WR) )	TBDS::genDBSet(owner().nodePath()+"vPctW",opt->text(),opt->attr("user"));
    }
    else if( a_path == "/val/pct_h" )
    {
	if( ctrChkNode(opt,"get",0660,"root","Archive",SEQ_RD) )	opt->setText(TBDS::genDBGet(owner().nodePath()+"vPctH","230",opt->attr("user")));
	if( ctrChkNode(opt,"set",0660,"root","Archive",SEQ_WR) )	TBDS::genDBSet(owner().nodePath()+"vPctH",opt->text(),opt->attr("user"));
    }
    else if( a_path == "/val/max" )
    {
	if( ctrChkNode(opt,"get",0660,"root","Archive",SEQ_RD) )	opt->setText(TBDS::genDBGet(owner().nodePath()+"vMax","0",opt->attr("user")));
	if( ctrChkNode(opt,"set",0660,"root","Archive",SEQ_WR) )	TBDS::genDBSet(owner().nodePath()+"vMax",opt->text(),opt->attr("user"));
    }
    else if( a_path == "/val/min" )
    {
        if( ctrChkNode(opt,"get",0660,"root","Archive",SEQ_RD) )	opt->setText(TBDS::genDBGet(owner().nodePath()+"vMin","0",opt->attr("user")));
        if( ctrChkNode(opt,"set",0660,"root","Archive",SEQ_WR) )	TBDS::genDBSet(owner().nodePath()+"vMin",opt->text(),opt->attr("user"));
    }
    else if( a_path == "/val/val" && ctrChkNode(opt,"get",0440,"root","Archive",SEQ_RD) )
    {
	long long end = (long long)atoi(TBDS::genDBGet(owner().nodePath()+"vaTm","0",opt->attr("user")).c_str())*1000000;
	if( !(end/1000000) ) end = (long long)time(NULL) * 1000000;
	end += atoi(TBDS::genDBGet(owner().nodePath()+"vaTm_u","0",opt->attr("user")).c_str());
	long long beg = end - (long long)(atof(TBDS::genDBGet(owner().nodePath()+"vaSize","1",opt->attr("user")).c_str())*1e6);
	
	TValBuf buf( TFld::String, 0, 0, false, true );
	getVal( buf, beg, end, TBDS::genDBGet(owner().nodePath()+"vArch","",opt->attr("user")), 2000 );

	XMLNode *n_tm   = ctrMkNode("list",opt,-1,"/val/val/0","",0440);
	XMLNode *n_val  = ctrMkNode("list",opt,-1,"/val/val/1","",0440);

	char c_buf[30];
	struct tm ttm;
	time_t tm_t;
	long long c_tm = buf.begin();
	if(buf.end() && buf.begin())
	    while(c_tm <= buf.end())
	    {
	        string val = buf.getS(&c_tm,true);
	        tm_t = c_tm/1000000;
	        localtime_r(&tm_t,&ttm);
	        snprintf(c_buf,sizeof(c_buf),"%d-%02d-%d %d:%02d:%02d.%d",
		    ttm.tm_mday,ttm.tm_mon+1,ttm.tm_year+1900,ttm.tm_hour,ttm.tm_min,ttm.tm_sec,c_tm%1000000);
		if(n_tm)	n_tm->childAdd("el")->setText(c_buf);
		if(n_val)	n_val->childAdd("el")->setText(val);
		c_tm++;
	    }
    }
    else if( a_path == "/val/trend" && ctrChkNode(opt,"get",0444,"root","Archive",SEQ_RD) )
    {
	int vPctW = vmin(1024,vmax(100,atoi(TBDS::genDBGet(owner().nodePath()+"vPctW","650",opt->attr("user")).c_str())));
	int vPctH = vmin(800,vmax(50,atoi(TBDS::genDBGet(owner().nodePath()+"vPctH","230",opt->attr("user")).c_str())));
	double vMax = atof(TBDS::genDBGet(owner().nodePath()+"vMax","0",opt->attr("user")).c_str());
	double vMin = atof(TBDS::genDBGet(owner().nodePath()+"vMin","0",opt->attr("user")).c_str());
	long long end = (long long) atoi(TBDS::genDBGet(owner().nodePath()+"vaTm",TSYS::int2str(time(NULL)),opt->attr("user")).c_str())*1000000+
				    atoi(TBDS::genDBGet(owner().nodePath()+"vaTm_u","0",opt->attr("user")).c_str());
	if( !(end/1000000) )	end = (long long)time(NULL) * 1000000;
	long long beg = end - (long long)(atof(TBDS::genDBGet(owner().nodePath()+"vaSize","1",opt->attr("user")).c_str())*1e6);

	opt->setText(TSYS::strEncode(makeTrendImg(beg,end,TBDS::genDBGet(owner().nodePath()+"vArch","",opt->attr("user")),vPctW,vPctH,vMax,vMin),TSYS::base64));
	opt->setAttr("tp","png");
    }
    else TCntrNode::cntrCmdProc(opt);
}

//*************************************************
//* TVArchivator                                  *
//*************************************************
TVArchivator::TVArchivator( const string &iid, const string &idb, TElem *cf_el ) : run_st(false), prc_st(false), 
    tm_calc(0.0), TConfig(cf_el), m_db(idb), m_id(cfg("ID").getSd()), m_name(cfg("NAME").getSd()), 
    m_dscr(cfg("DESCR").getSd()), m_addr(cfg("ADDR").getSd()), m_start(cfg("START").getBd()), 
    m_v_per(cfg("V_PER").getRd()), m_a_per(cfg("A_PER").getId())
{
    m_id = iid;

    //- Create calc timer -
    struct sigevent sigev;
    sigev.sigev_notify = SIGEV_THREAD;
    sigev.sigev_value.sival_ptr = this;
    sigev.sigev_notify_function = Task;
    sigev.sigev_notify_attributes = NULL;
    timer_create(CLOCK_REALTIME,&sigev,&tmId);
}

TCntrNode &TVArchivator::operator=( TCntrNode &node )
{
    TVArchivator *src_n = dynamic_cast<TVArchivator*>(&node);
    if( !src_n ) return *this;

    //- Configuration copy -
    string tid = id();
    *(TConfig*)this = *(TConfig*)src_n;
    m_id = tid;
    m_db = src_n->m_db;

    if( src_n->startStat() && toStart() && !startStat() )
        start( );

    return *this;
}

TVArchivator::~TVArchivator()
{
    timer_delete(tmId);
}

string TVArchivator::name()
{
    return (m_name.size())?m_name:m_id;
}

string TVArchivator::tbl( )
{
    return owner().owner().subId()+"_val_proc";
}

void TVArchivator::setValPeriod( double iper )
{
    m_v_per = iper ? iper : 1.;

    //- Call sort for all archives -
    ResAlloc res(a_res,false);
    for( map<string,TVArchEl*>::iterator iel = archEl.begin(); iel != archEl.end(); iel++ )
	iel->second->archive().archivatorSort();

    modif();
}

void TVArchivator::postEnable(int flag)
{
    cfg("MODUL").setS(owner().modId());
}

void TVArchivator::preDisable( int flag )
{
    if( startStat() )	stop( flag );
}

void TVArchivator::postDisable( int flag )
{
    try
    {
	if( flag )
	    SYS->db().at().dataDel(fullDB(),SYS->archive().at().nodePath()+tbl(),*this);
    }catch(TError err)
    { mess_warning(err.cat.c_str(),"%s",err.mess.c_str()); }
}

string TVArchivator::workId()
{
    return owner().modId()+"."+id();
}

void TVArchivator::start()
{
    //- Start interval timer for periodic thread creating -
    struct itimerspec itval;
    itval.it_interval.tv_sec = itval.it_value.tv_sec = m_a_per;
    itval.it_interval.tv_nsec = itval.it_value.tv_nsec = 0;
    timer_settime(tmId, 0, &itval, NULL);

    run_st = true;
};

void TVArchivator::stop( bool full_del )
{
    //- Stop interval timer for periodic thread creating -
    struct itimerspec itval;
    itval.it_interval.tv_sec = itval.it_interval.tv_nsec = itval.it_value.tv_sec = itval.it_value.tv_nsec = 0;
    timer_settime(tmId, 0, &itval, NULL);
    if( TSYS::eventWait( prc_st, false, nodePath()+"stop",5) )
	throw TError(nodePath().c_str(),_("Archive thread no stoped!"));

    //- Detach from all archives -
    ResAlloc res(a_res,false);
    while(archEl.size())
    {
	AutoHD<TVArchive> arch(&archEl.begin()->second->archive());
	res.release();
	arch.at().archivatorDetach(workId(),full_del);
	res.request(false);
    }

    run_st = false;
};

void TVArchivator::archiveList( vector<string> &ls )
{
    ResAlloc res(a_res,false);
    ls.clear();
    for( map<string,TVArchEl*>::iterator iel = archEl.begin(); iel != archEl.end(); iel++ )
	ls.push_back(iel->first);
}

bool TVArchivator::archivePresent( const string &iid )
{
    ResAlloc res(a_res,false);
    if( archEl.find(iid) != archEl.end() )	return true;
    return false;
}

TVArchEl *TVArchivator::archivePlace( TVArchive &item )
{
    ResAlloc res(a_res,true);
    map<string,TVArchEl*>::iterator iel = archEl.find(item.id());
    if( iel != archEl.end() )	return iel->second;
    TVArchEl *it_el = getArchEl( item );
    archEl[item.id()] = it_el;

    return it_el;
}

void TVArchivator::archiveRemove( const string &iid, bool full )
{
    ResAlloc res(a_res,true);
    map<string,TVArchEl*>::iterator iel = archEl.find(iid);
    if( iel != archEl.end() )
    {
	if( full ) iel->second->fullErase();
	delete iel->second;
	archEl.erase(iel);
    }
}

TVArchEl *TVArchivator::getArchEl( TVArchive &arch )
{
    return new TVArchEl( arch, *this );
}

TTipArchivator &TVArchivator::owner()
{
    return *(TTipArchivator *)nodePrev();
}

void TVArchivator::load_( )
{
    SYS->db().at().dataGet(fullDB(),SYS->archive().at().nodePath()+tbl(),*this);
}

void TVArchivator::save_( )
{
    SYS->db().at().dataSet(fullDB(),SYS->archive().at().nodePath()+tbl(),*this);
}

void TVArchivator::Task(union sigval obj)
{
    TVArchivator *arch = (TVArchivator*)obj.sival_ptr;
    if( arch->prc_st )  return;
    arch->prc_st = true;

    //- Archiving -
    try
    {
	unsigned long long t_cnt = SYS->shrtCnt();

	ResAlloc res(arch->a_res,false);
	long long beg, end;
	for( map<string,TVArchEl*>::iterator iel = arch->archEl.begin(); iel != arch->archEl.end(); iel++ )
	    if( iel->second->archive().startStat() )
	    {
		TVArchEl *arch_el = iel->second;
		beg = vmax(arch_el->m_last_get,arch_el->archive().begin());
		end = arch_el->archive().end();
		if(!beg || !end || beg > end )	continue;
		//-- Averaging --
		long long a_per = (long long)(1000000.*arch->valPeriod());
		if( a_per > arch_el->archive().period() )
		{
		    TValBuf buf(arch_el->archive().valType(),((end-beg)/a_per)+1,a_per,true,true);
		    for( long long c_tm = beg; c_tm <= end;)
		    {
			switch(arch_el->archive().valType())
			{
			    case TFld::Boolean:
			    {
				char c_val = arch_el->archive().getB(&c_tm,true);
				buf.setB(c_val,c_tm);
				c_tm+=a_per;
				break;
			    }
			    case TFld::String:
			    {
				string c_val = arch_el->archive().getS(&c_tm,true);
				buf.setS(c_val,c_tm);
				c_tm+=a_per;
				break;
			    }
			    case TFld::Integer:
			    {
				int c_val = arch_el->archive().getI(&c_tm,true);
				int vdif = c_tm/a_per - arch_el->prev_tm/a_per;
				if( !vdif )
				{
				    int v_o = *(int*)arch_el->prev_val.c_str();
				    if( c_val == EVAL_INT ) c_val = v_o;
				    if( c_val != EVAL_INT && v_o != EVAL_INT )
				    {
					long long s_k = c_tm-a_per*(c_tm/a_per);
					long long n_k = arch_el->archive().period();
					c_val = ((long long)v_o*s_k+(long long)c_val*n_k)/(s_k+n_k);
				    }
				    arch_el->prev_val.assign((char*)&c_val,sizeof(int));
				    arch_el->prev_tm = c_tm;
				}
				if( vdif == 1 || c_tm+1 > end )
				    buf.setI(*(int*)arch_el->prev_val.c_str(),arch_el->prev_tm);
				if( vdif )
				{
				    arch_el->prev_val.assign((char*)&c_val,sizeof(int));
				    arch_el->prev_tm = c_tm;
				}
				c_tm++;
				break;
			    }
			    case TFld::Real:
			    {
				double c_val = arch_el->archive().getR(&c_tm,true);
				int vdif = c_tm/a_per - arch_el->prev_tm/a_per;
				if( !vdif )
				{
				    double v_o = *(double*)arch_el->prev_val.c_str();
				    if( c_val == EVAL_REAL ) c_val = v_o;
				    if( c_val != EVAL_REAL && v_o != EVAL_REAL )
				    {
					long long s_k = c_tm-a_per*(c_tm/a_per);
					long long n_k = arch_el->archive().period();
					c_val = (v_o*s_k+c_val*n_k)/(s_k+n_k);
				    }
				    arch_el->prev_val.assign((char*)&c_val,sizeof(double));
				    arch_el->prev_tm = c_tm;
				}
				if( vdif == 1 || c_tm+1 > end )
				    buf.setR(*(double*)arch_el->prev_val.c_str(),arch_el->prev_tm);
				if( vdif )
				{
				    arch_el->prev_val.assign((char*)&c_val,sizeof(double));
				    arch_el->prev_tm = c_tm;
				}
				c_tm++;
				break;
			    }
			}
		    }
		    arch_el->setVal( buf, buf.begin(), end );
		}
		else arch_el->setVal( arch_el->archive(), beg, end );
		
		arch_el->m_last_get = end+1;
	    }

	arch->tm_calc = 1.0e3*((double)(SYS->shrtCnt()-t_cnt))/((double)SYS->sysClk());
    } catch(TError err)
    { mess_err(err.cat.c_str(),"%s",err.mess.c_str() ); }

    arch->prc_st = false;
}

void TVArchivator::setArchPeriod( int iper )
{
    m_a_per = iper ? iper : 1;
    if( startStat() )
    {
	struct itimerspec itval;
	itval.it_interval.tv_sec = itval.it_value.tv_sec = m_a_per;
	itval.it_interval.tv_nsec = itval.it_value.tv_nsec = 0;
	timer_settime(tmId, 0, &itval, NULL);
    }

    modif();
}

void TVArchivator::cntrCmdProc( XMLNode *opt )
{
    //- Get page info -
    if( opt->name() == "info" )
    {
	ctrMkNode("oscada_cntr",opt,-1,"/",_("Value archivator: ")+name(),0664,"root","Archive");
	if(ctrMkNode("area",opt,-1,"/prm",_("Archivator")))
	{
	    if(ctrMkNode("area",opt,-1,"/prm/st",_("State")))
	    {
		ctrMkNode("fld",opt,-1,"/prm/st/st",_("Runing"),0664,"root","Archive",1,"tp","bool");
		ctrMkNode("fld",opt,-1,"/prm/st/tarch",_("Archiving time (msek)"),0444,"root","Archive",1,"tp","real");
		ctrMkNode("fld",opt,-1,"/prm/st/db",_("Archivator DB"),0664,"root","root",4,"tp","str","dest","select","select","/db/list",
		    "help",_("DB address in format [<DB module>.<DB name>].\nFor use main work DB set '*.*'."));
	    }
	    if(ctrMkNode("area",opt,-1,"/prm/cfg",_("Config")))
	    {
		ctrMkNode("fld",opt,-1,"/prm/cfg/id",cfg("ID").fld().descr(),0444,"root","Archive",1,"tp","str");
		ctrMkNode("fld",opt,-1,"/prm/cfg/nm",cfg("NAME").fld().descr(),0664,"root","Archive",1,"tp","str");
		ctrMkNode("fld",opt,-1,"/prm/cfg/dscr",cfg("DESCR").fld().descr(),0664,"root","Archive",3,"tp","str","cols","50","rows","3");
		ctrMkNode("fld",opt,-1,"/prm/cfg/vper",cfg("V_PER").fld().descr(),0664,"root","Archive",1,"tp","real");
		ctrMkNode("fld",opt,-1,"/prm/cfg/aper",cfg("A_PER").fld().descr(),0664,"root","Archive",1,"tp","dec");
		ctrMkNode("fld",opt,-1,"/prm/cfg/addr",cfg("ADDR").fld().descr(),0664,"root","Archive",1,"tp","str");
		ctrMkNode("fld",opt,-1,"/prm/cfg/start",_("To start"),0664,"root","Archive",1,"tp","bool");
	    }
	}
	if(ctrMkNode("area",opt,-1,"/arch",_("Archives")))
	    if(ctrMkNode("table",opt,-1,"/arch/arch",_("Archives"),0444,"root","Archive"))
	    {
		ctrMkNode("list",opt,-1,"/arch/arch/0",_("Archive"),0444,"root","Archive",1,"tp","str");
		ctrMkNode("list",opt,-1,"/arch/arch/1",_("Period (s)"),0444,"root","Archive",1,"tp","real");
		ctrMkNode("list",opt,-1,"/arch/arch/2",_("Buffer size"),0444,"root","Archive",1,"tp","dec");
	    }
	return;
    }

    //- Process command to page -
    string a_path = opt->attr("path");
    if( a_path == "/prm/st/st" )
    {
	if( ctrChkNode(opt,"get",0664,"root","Archive",SEQ_RD) )	opt->setText( startStat() ? "1" : "0" );
	if( ctrChkNode(opt,"set",0664,"root","Archive",SEQ_WR) )	atoi(opt->text().c_str()) ? start() : stop();
    }
    else if( a_path == "/prm/st/tarch" && ctrChkNode(opt) )		opt->setText(TSYS::real2str(tm_calc,6));
    else if( a_path == "/prm/st/db" )
    {
	if( ctrChkNode(opt,"get",0664,"root","Archive",SEQ_RD) )	opt->setText( DB() );
	if( ctrChkNode(opt,"set",0664,"root","Archive",SEQ_WR) )	setDB( opt->text() );
    }
    else if( a_path == "/prm/cfg/id" && ctrChkNode(opt) )		opt->setText(id());
    else if( a_path == "/prm/cfg/nm" )
    {
	if( ctrChkNode(opt,"get",0664,"root","Archive",SEQ_RD) )	opt->setText( name() );
	if( ctrChkNode(opt,"set",0664,"root","Archive",SEQ_WR) )	setName( opt->text() );
    }
    else if( a_path == "/prm/cfg/dscr" )
    {
	if( ctrChkNode(opt,"get",0664,"root","Archive",SEQ_RD) )	opt->setText( dscr() );
	if( ctrChkNode(opt,"set",0664,"root","Archive",SEQ_WR) )	setDscr( opt->text() );
    }
    else if( a_path == "/prm/cfg/addr" )
    {
	if( ctrChkNode(opt,"get",0664,"root","Archive",SEQ_RD) )	opt->setText( addr() );
	if( ctrChkNode(opt,"set",0664,"root","Archive",SEQ_WR) )	setAddr( opt->text() );
    }
    else if( a_path == "/prm/cfg/start" )
    {
	if( ctrChkNode(opt,"get",0664,"root","Archive",SEQ_RD) )	opt->setText( toStart() ? "1" : "0" );
	if( ctrChkNode(opt,"set",0664,"root","Archive",SEQ_WR) )	setToStart( atoi(opt->text().c_str()) );
    }
    else if( a_path == "/prm/cfg/vper" )
    {
	if( ctrChkNode(opt,"get",0664,"root","Archive",SEQ_RD) )	opt->setText(TSYS::real2str(valPeriod(),6));
	if( ctrChkNode(opt,"set",0664,"root","Archive",SEQ_WR) )	setValPeriod(atof(opt->text().c_str()));
    }
    else if( a_path == "/prm/cfg/aper" )
    {
	if( ctrChkNode(opt,"get",0664,"root","Archive",SEQ_RD) )	opt->setText(TSYS::int2str(archPeriod()));
	if( ctrChkNode(opt,"set",0664,"root","Archive",SEQ_WR) )	setArchPeriod(atoi(opt->text().c_str()));
    }
    else if( a_path == "/arch/arch" && ctrChkNode(opt) )
    {
	//-- Fill Archives table --
	XMLNode *n_arch = ctrMkNode("list",opt,-1,"/arch/arch/0","");
	XMLNode *n_per  = ctrMkNode("list",opt,-1,"/arch/arch/1","");
	XMLNode *n_size = ctrMkNode("list",opt,-1,"/arch/arch/2","");

	ResAlloc res(a_res,false);
	for( map<string,TVArchEl*>::iterator iel = archEl.begin(); iel != archEl.end(); iel++ )
	{
	    if(n_arch)	n_arch->childAdd("el")->setText(iel->second->archive().id());
	    if(n_per)	n_per->childAdd("el")->setText(TSYS::real2str((double)iel->second->archive().period()/1000000.,6));
	    if(n_size)	n_size->childAdd("el")->setText(TSYS::int2str(iel->second->archive().size()));
	}
    }
    else TCntrNode::cntrCmdProc(opt);
}

//*************************************************
//* TVArchEl                                      *
//*************************************************
TVArchEl::TVArchEl( TVArchive &iarchive, TVArchivator &iarchivator ) :
    m_achive(iarchive), m_archivator(iarchivator), m_last_get(0), prev_tm(0)
{

}

TVArchEl::~TVArchEl()
{

}

TVArchive &TVArchEl::archive()
{
    return m_achive;
}

TVArchivator &TVArchEl::archivator()
{
    return m_archivator;
}
