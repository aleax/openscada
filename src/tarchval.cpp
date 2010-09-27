
//OpenSCADA system file: tarchval.cpp
/***************************************************************************
 *   Copyright (C) 2006-2009 by Roman Savochenko                           *
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

#include <signal.h>
#include <string.h>
#include <time.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <math.h>

#include <gd.h>

#include "tsys.h"
#include "tvalue.h"
#include "tarchives.h"
#include "tarchval.h"

#define HalfDivMinWin	5

using namespace OSCADA;

//*************************************************
//* Value archivator                              *
//*************************************************

//*************************************************
//* TValBuf                                       *
//*************************************************
TValBuf::TValBuf( ) :
    mValTp(TFld::Integer), mSize(100), mPer(0), mHrdGrd(false), mHgResTm(false), mEnd(0), mBeg(0)
{
    buf.bl = NULL;

    makeBuf( mValTp, mSize, mPer, mHrdGrd, mHgResTm );
}

TValBuf::TValBuf( TFld::Type vtp, int isz, long long ipr, bool ihgrd, bool ihres ) :
    mValTp(vtp), mSize(isz), mPer(ipr), mHrdGrd(ihgrd), mHgResTm(ihres), mEnd(0), mBeg(0)
{
    buf.bl = NULL;

    makeBuf( mValTp, mSize, mPer, mHrdGrd, mHgResTm );
}

TValBuf::~TValBuf( )
{
    switch(mValTp)
    {
	case TFld::Boolean:	delete buf.bl;	break;
	case TFld::Integer:	delete buf.dec;	break;
	case TFld::Real:	delete buf.real;break;
	case TFld::String:	delete buf.str;	break;
    }
}

TValBuf &TValBuf::operator=( TValBuf &src )
{
    makeBuf( src.mValTp, src.mSize, src.mPer, src.mHrdGrd, src.mHgResTm );

    switch(mValTp)
    {
	case TFld::Boolean:
	    buf.bl->cur = src.buf.bl->cur;
	    if( mHrdGrd ) *(buf.bl->buf.grid) = *(src.buf.bl->buf.grid);
	    else if( mHgResTm )	*(buf.bl->buf.tm_high) = *(src.buf.bl->buf.tm_high);
	    else *(buf.bl->buf.tm_low) = *(src.buf.bl->buf.tm_low);
	    break;
	case TFld::Integer:
	    buf.dec->cur = src.buf.dec->cur;
	    if( mHrdGrd ) *(buf.dec->buf.grid) = *(src.buf.dec->buf.grid);
	    else if( mHgResTm )	*(buf.dec->buf.tm_high) = *(src.buf.dec->buf.tm_high);
	    else *(buf.dec->buf.tm_low) = *(src.buf.dec->buf.tm_low);
	    break;
	case TFld::Real:
	    buf.real->cur = src.buf.real->cur;
	    if( mHrdGrd ) *(buf.real->buf.grid) = *(src.buf.real->buf.grid);
	    else if( mHgResTm )	*(buf.real->buf.tm_high) = *(src.buf.real->buf.tm_high);
	    else *(buf.real->buf.tm_low) = *(src.buf.real->buf.tm_low);
	    break;
	case TFld::String:
	    buf.str->cur = src.buf.str->cur;
	    if( mHrdGrd ) *(buf.str->buf.grid) = *(src.buf.str->buf.grid);
	    else if( mHgResTm )	*(buf.str->buf.tm_high) = *(src.buf.str->buf.tm_high);
	    else *(buf.str->buf.tm_low) = *(src.buf.str->buf.tm_low);
	    break;
    }
    mEvalCnt = src.mEvalCnt;
}

void TValBuf::clear()
{
    switch(mValTp)
    {
	case TFld::Boolean:	buf.bl->clear();	break;
	case TFld::Integer:	buf.dec->clear();	break;
	case TFld::Real:	buf.real->clear();	break;
	case TFld::String:	buf.str->clear(); 	break;
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

void TValBuf::setValType( TFld::Type vl )	{ makeBuf(vl, mSize, mPer, mHrdGrd, mHgResTm ); }

void TValBuf::setHardGrid( bool vl )		{ makeBuf( mValTp, mSize, mPer, vl, mHgResTm ); }

void TValBuf::setHighResTm( bool vl )		{ makeBuf( mValTp, mSize, mPer, mHrdGrd, vl ); }

void TValBuf::setSize( int vl )			{ makeBuf( mValTp, vl, mPer, mHrdGrd, mHgResTm ); }

void TValBuf::setPeriod( long long vl )		{ makeBuf( mValTp, mSize, vl, mHrdGrd, mHgResTm ); }

void TValBuf::makeBuf( TFld::Type v_tp, int isz, long long ipr, bool hd_grd, bool hg_res )
{
    ResAlloc res(bRes,true);

    //Destroy buffer
    if( v_tp != mValTp && buf.bl )
    {
	switch(mValTp)
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
	    case TFld::Boolean:	buf.bl = new TBuf<char>( EVAL_BOOL, mSize, mPer, mHrdGrd, mHgResTm, mEnd, mBeg, mEvalCnt );	break;
	    case TFld::Integer:	buf.dec = new TBuf<int>( EVAL_INT, mSize, mPer, mHrdGrd, mHgResTm, mEnd, mBeg, mEvalCnt );	break;
	    case TFld::Real:	buf.real = new TBuf<double>( EVAL_REAL, mSize, mPer, mHrdGrd, mHgResTm, mEnd, mBeg, mEvalCnt );	break;
	    case TFld::String:	buf.str = new TBuf<string>( EVAL_STR, mSize, mPer, mHrdGrd, mHgResTm, mEnd, mBeg, mEvalCnt );	break;
	}
	mValTp = v_tp;
    }
    if( isz != mSize || ipr != mPer || hd_grd != mHrdGrd || hg_res != mHgResTm )
	switch(mValTp)
	{
	    case TFld::Boolean:	buf.bl->makeBuf( isz, ipr, hd_grd, hg_res );	break;
	    case TFld::Integer:	buf.dec->makeBuf( isz, ipr, hd_grd, hg_res );	break;
	    case TFld::Real:	buf.real->makeBuf( isz, ipr, hd_grd, hg_res );	break;
	    case TFld::String:	buf.str->makeBuf( isz, ipr, hd_grd, hg_res );	break;
	}
}

string TValBuf::getS( long long *itm, bool up_ord )
{
    switch(valType())
    {
	case TFld::Boolean:	{ char vl = getB(itm,up_ord); return (vl==EVAL_BOOL)?EVAL_STR:TSYS::int2str((bool)vl); }
	case TFld::Integer:	{ int vl = getI(itm,up_ord); return (vl==EVAL_INT)?EVAL_STR:TSYS::int2str(vl); }
	case TFld::Real:	{ double vl = getR(itm,up_ord); return (vl==EVAL_REAL)?EVAL_STR:TSYS::real2str(vl); }
	case TFld::String:	{ ResAlloc res(bRes,false); return buf.str->get(itm,up_ord); }
    }
    return EVAL_STR;
}

double TValBuf::getR( long long *itm, bool up_ord )
{
    switch(valType())
    {
	case TFld::Boolean:	{ char vl = getB(itm,up_ord); return (vl==EVAL_BOOL)?EVAL_REAL:(bool)vl; }
	case TFld::Integer:	{ int vl = getI(itm,up_ord); return (vl==EVAL_INT)?EVAL_REAL:(double)vl; }
	case TFld::String:	{ string vl = getS(itm,up_ord); return (vl==EVAL_STR)?EVAL_REAL:atof(vl.c_str()); }
	case TFld::Real:	{ ResAlloc res(bRes,false); return buf.real->get(itm,up_ord); }
    }
    return EVAL_REAL;
}

int TValBuf::getI( long long *itm, bool up_ord )
{
    switch(valType())
    {
	case TFld::Boolean:	{ char vl = getB(itm,up_ord); return (vl==EVAL_BOOL)?EVAL_INT:(bool)vl; }
	case TFld::String:	{ string vl = getS(itm,up_ord); return (vl==EVAL_STR)?EVAL_INT:atoi(vl.c_str()); }
	case TFld::Real:	{ double vl = getR(itm,up_ord); return (vl==EVAL_REAL)?EVAL_INT:(int)vl; }
	case TFld::Integer:	{ ResAlloc res(bRes,false); return buf.dec->get(itm,up_ord); }
    }
    return EVAL_INT;
}

char TValBuf::getB( long long *itm, bool up_ord )
{
    switch(valType())
    {
	case TFld::Integer:	{ int vl = getI(itm,up_ord); return (vl==EVAL_INT)?EVAL_BOOL:(bool)vl; }
	case TFld::String:	{ string vl = getS(itm,up_ord); return (vl==EVAL_STR)?EVAL_BOOL:(bool)atoi(vl.c_str()); }
	case TFld::Real:	{ double vl = getR(itm,up_ord); return (vl==EVAL_REAL)?EVAL_BOOL:(bool)vl; }
	case TFld::Boolean:	{ ResAlloc res(bRes,false); return buf.bl->get(itm,up_ord); }
    }
    return EVAL_BOOL;
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
	    ResAlloc res(bRes,true); buf.str->set(value,tm);		break;
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
	    ResAlloc res(bRes,true); buf.real->set(value,tm);	break;
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
	    ResAlloc res(bRes,true); buf.dec->set(value,tm);	break;
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
	    ResAlloc res(bRes,true); buf.bl->set(value,tm);	break;
    }
}

void TValBuf::getVals( TValBuf &buf, long long ibeg, long long iend )
{
    if(!vOK(ibeg,iend))	return;
    ibeg = vmax(ibeg,begin());
    iend = vmin(iend,end());

    while( ibeg <= iend )
    {
	switch(valType())
	{
	    case TFld::Boolean:	{ char vl = getB(&ibeg,true); buf.setB(vl,ibeg); break; }
	    case TFld::Integer:	{ int vl = getI(&ibeg,true); buf.setI(vl,ibeg); break; }
	    case TFld::Real:	{ double vl = getR(&ibeg,true); buf.setR(vl,ibeg); break; }
	    case TFld::String:	{ string vl = getS(&ibeg,true); buf.setS(vl,ibeg); break; }
	}
	ibeg++;
    }
}

void TValBuf::setVals( TValBuf &buf, long long ibeg, long long iend )
{
    buf.getVals( *this, ibeg, iend );
}

//*************************************************
//* TValBuf::TBuf                                 *
//*************************************************
template <class TpVal> TValBuf::TBuf<TpVal>::TBuf( TpVal ieval, int &isz, long long &ipr,
	bool &ihgrd, bool &ihres, long long& iend, long long& ibeg, unsigned int &iEvalCnt ) :
    eval(ieval), size(isz), per(ipr), hrd_grd(ihgrd), hg_res_tm(ihres), end(iend), beg(ibeg), cur(0), mEvalCnt(iEvalCnt)
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
    mEvalCnt = 0;
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
    if( isz < size || (!size && isz) )	recr_buf = true;
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
	//> Destroy buffer
	cur = end = beg = 0;
	if( buf.grid )
	{
	    if( hrd_grd )		delete buf.grid;
	    else if( hg_res_tm )	delete buf.tm_high;
	    else			delete buf.tm_low;
	}
	buf.grid = NULL;

	//> Make new buffer
	if( ihd_grd )	buf.grid = new vector<TpVal>;
	else if(ihg_res)buf.tm_high = new vector<SHg>;
	else		buf.tm_low = new vector<SLw>;
	mEvalCnt = 0;
    }
}

template <class TpVal> TpVal TValBuf::TBuf<TpVal>::get( long long *itm, bool up_ord )
{
    long long tm = (itm)?(*itm):TSYS::curTime();

    if( (up_ord && tm > end) || (!up_ord && tm < beg) )	throw TError("ValBuf",_("Value is not present."));
    tm = up_ord ? vmax(tm,beg) : vmin(tm,end);
    //> Process hard grid buffer
    if( hrd_grd )
    {
	int npos = up_ord?(end-tm)/per:(long long)buf.grid->size()-1-(tm-beg)/per;
	if( npos < 0 || npos >= buf.grid->size() ) { if(itm) *itm = 0; return eval; }
	if( itm )	*itm = end-npos*per;
	return (*buf.grid)[((cur-npos-1)>=0)?(cur-npos-1):(buf.grid->size()+(cur-npos-1))];
    }
    //> Process soft grid buffer
    else if( per )
    {
	int npos = (up_ord?(end-tm):(tm-beg))/per;
	if( hg_res_tm )
	{
	    int c_cur = (cur)?cur-1:buf.tm_high->size()-1;
	    int c_end = c_cur;

	    //>> Half divider
	    for(int d_win = buf.tm_high->size()/2; d_win > HalfDivMinWin; d_win/=2)
	    {
		int c_cnext = c_end-d_win;
		if(c_cnext < 0) c_cnext += buf.tm_high->size();
		if(tm < (*buf.tm_high)[c_cnext].tm) c_end=c_cnext;
	    }
	    //>> Proving
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

	    if(itm) *itm = 0;
	    return eval;
	}
	else
	{
	    int c_cur = (cur)?cur-1:buf.tm_low->size()-1;
	    int c_end = c_cur;

	    //>> Half divider
	    for(int d_win = buf.tm_low->size()/2; d_win > HalfDivMinWin; d_win/=2)
	    {
		int c_cnext = c_end-d_win;
		if(c_cnext < 0) c_cnext += buf.tm_low->size();
		if(tm < (long long)(*buf.tm_low)[c_cnext].tm*1000000) c_end=c_cnext;
	    }
	    //>> Proving
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

	    if(itm) *itm = 0;
	    return eval;
	}
    }
    //> Proccess flow buffer
    else
    {
	if( hg_res_tm )
	{
	    int c_end = buf.tm_high->size()-1;
	    //>> Half divider
	    for( int d_win = c_end/2; d_win > HalfDivMinWin; d_win/=2 )
		if( !((!up_ord && tm >= (*buf.tm_high)[c_end-d_win].tm) ||
			(up_ord && tm <= (*buf.tm_high)[buf.tm_high->size()-(c_end-d_win)-1].tm)) )
		    c_end-=d_win;
	    //>> Scan last window
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
	    if(itm) *itm = 0;
	    return eval;
	}
	else
	{
	    int c_end = buf.tm_low->size()-1;
	    //>> Half divider
	    for( int d_win = c_end/2; d_win > HalfDivMinWin; d_win/=2 )
		if( !((!up_ord && tm/1000000 >= (*buf.tm_low)[c_end-d_win].tm) ||
			(up_ord && tm <= (long long)(*buf.tm_low)[buf.tm_low->size()-(c_end-d_win)-1].tm*1000000)) )
		    c_end-=d_win;
	    //>> Scan last window
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
	    if(itm) *itm = 0;
	    return eval;
	}
    }
}

template <class TpVal> void TValBuf::TBuf<TpVal>::set( TpVal value, long long tm )
{
    if( !tm )	tm = TSYS::curTime();
    if( !end )	end = (per) ? per*(tm/per-1) : tm;
    if( !beg )	beg = (per) ? per*(tm/per) : tm;

    //> Process hard grid buffer
    if( hrd_grd )
    {
	int npos = (tm-end)/per;
	//>> Set value
	if( npos <= 0 && (-npos) < buf.grid->size() )
	{
	    int wcur = cur+npos-1;
	    if( wcur < 0 ) wcur = buf.grid->size()+wcur;
	    if( value == eval ) mEvalCnt++;
	    if( (*buf.grid)[wcur] == eval ) mEvalCnt--;
	    (*buf.grid)[wcur] = value;
	    return;
	}
	else if( npos < 0 )	throw TError("ValBuf",_("Grid mode doesn't support inserting too old values %lld (%lld-%lld)."),tm,beg,end);
	else
	    while( npos-- )
	    {
		TpVal w_val = npos ? eval : value;
		//>> Set new value
		if( w_val == eval ) mEvalCnt++;
		if( cur >= buf.grid->size() )	buf.grid->push_back(w_val);
		else
		{
		    beg += per;
		    if( (*buf.grid)[cur] == eval ) mEvalCnt--;
		    (*buf.grid)[cur] = w_val;
		}
		//>> Update cursor
		if( (++cur >= size) && size )
		{
		    cur = 0;
		    //>>> Memory consumption is made optimal for limited size
		    if( buf.grid->capacity() > size )
		    {
			vector<TpVal> *ns = new vector<TpVal>;
			*ns = *buf.grid;
			delete buf.grid;
			buf.grid = ns;
		    }
		}
		//>> Update end time
		end += per;
	    }
    }
    //> Process soft grid buffer
    else if( per )
    {
	int npos = (tm-end)/per;
	//>> Set value
        if( npos < 0 )	throw TError("ValBuf",_("Grid mode doesn't support inserting old values."));
	else
	{
	    if( hg_res_tm )
	    {
		SHg b_el;

		//>> Update current end value
		if( npos == 0 )
		{
		    b_el.tm = end; b_el.val = value;
		    int h_el = cur ? cur-1 : buf.tm_high->size()-1;
		    if( ((*buf.tm_high)[h_el].tm-end)/per )
		    {
			//>>> Write new value
			if( b_el.val == eval ) mEvalCnt++;
			if( cur >= buf.tm_high->size() ) buf.tm_high->push_back(b_el);
			else
			{
			    if( cur+1 >= buf.tm_high->size() ) beg = (*buf.tm_high)[0].tm;
			    else beg = (*buf.tm_high)[cur+1].tm;
			    beg = per*(beg/per);
			    if( (*buf.tm_high)[cur].val == eval ) mEvalCnt--;
			    (*buf.tm_high)[cur] = b_el;
			}
			//>>> Update cursor
			if( (++cur >= size) && size )
			{
			    cur = 0;
			    //>>>> Memory consumption is made optimal for limited size
			    if( buf.tm_high->capacity() > size )
			    {
				vector<SHg> *ns = new vector<SHg>;
				*ns = *buf.tm_high;
				delete buf.tm_high;
				buf.tm_high = ns;
			    }
			}
		    }
		    else (*buf.tm_high)[h_el] = b_el;
		    return;
		}

		//>> Insert new value
		int c_npos = npos;
		while( c_npos-- )
		{
		    //>>> Prepare data
		    if(c_npos) 	{ b_el.tm = end+(npos-c_npos)*per;  b_el.val = eval; }
		    else	{ b_el.tm = tm; b_el.val = value; }
		    //>>> Check previous value
		    if( !buf.tm_high->size() ||
			    (cur && (*buf.tm_high)[cur-1].val != b_el.val ) ||
			    (!cur && (*buf.tm_high)[buf.tm_high->size()-1].val != b_el.val) )
		    {
			//>>>> Write new value
			if( b_el.val == eval ) mEvalCnt++;
			if( cur >= buf.tm_high->size() ) buf.tm_high->push_back(b_el);
			else
			{
			    if( cur+1 >= buf.tm_high->size() )
				beg = (*buf.tm_high)[0].tm;
			    else beg = (*buf.tm_high)[cur+1].tm;
			    beg = per*(beg/per);
			    if( (*buf.tm_high)[cur].val == eval ) mEvalCnt--;
			    (*buf.tm_high)[cur] = b_el;
			}
			//>>>> Update cursor
			if( (++cur >= size) && size )
			{
			    cur = 0;
			    //>>>> Memory consumption is made optimal for limited size
			    if( buf.tm_high->capacity() > size )
			    {
				vector<SHg> *ns = new vector<SHg>;
				*ns = *buf.tm_high;
				delete buf.tm_high;
				buf.tm_high = ns;
			    }
			}
		    }
		    //>>> Update end time
		    end += per;
		}
	    }
	    else
	    {
		SLw b_el;

		//>> Update current end value
		if( npos == 0 )
		{
		    b_el.tm = end/1000000; b_el.val = value;
		    int h_el = (cur) ? cur-1 : buf.tm_low->size()-1;
		    if( ((long long)(*buf.tm_low)[h_el].tm*1000000-end)/per )
		    {
			//>>> Write new value
			if( b_el.val == eval ) mEvalCnt++;
			if( cur >= buf.tm_low->size() )	buf.tm_low->push_back(b_el);
			else
			{
			    if( cur+1 >= buf.tm_low->size() )
				beg = (long long)(*buf.tm_low)[0].tm * 1000000;
			    else beg = (long long)(*buf.tm_low)[cur+1].tm * 1000000;
			    beg = per*(beg/per);
			    if( (*buf.tm_low)[cur].val == eval ) mEvalCnt--;
			    (*buf.tm_low)[cur] = b_el;
			}
			//>>> Update cursor
			if( (++cur >= size) && size )
			{
			    cur = 0;
			    //>>>> Memory consumption is made optimal for limited size
			    if( buf.tm_low->capacity() > size )
			    {
				vector<SLw> *ns = new vector<SLw>;
				*ns = *buf.tm_low;
				delete buf.tm_low;
				buf.tm_low = ns;
			    }
			}
		    }
		    else (*buf.tm_low)[h_el] = b_el;
		    return;
		}

		//>> Insert new value
		int c_npos = npos;
		while( c_npos-- )
		{
		    //>>> Prepare data
		    if(c_npos)	{ b_el.tm = (end+(npos-c_npos)*per)/1000000; b_el.val = eval; }
		    else	{ b_el.tm = tm/1000000; b_el.val = value; }
		    //>>> Check previous value
		    if( !buf.tm_low->size() ||
			    (cur && (*buf.tm_low)[cur-1].val != b_el.val ) ||
			    (!cur && (*buf.tm_low)[buf.tm_low->size()-1].val != b_el.val) )
		    {
			//>>>> Write new value
			if( b_el.val == eval ) mEvalCnt++;
			if( cur >= buf.tm_low->size() )	buf.tm_low->push_back(b_el);
			else
			{
			    if( cur+1 >= buf.tm_low->size() )
				beg = (long long)(*buf.tm_low)[0].tm * 1000000;
			    else beg = (long long)(*buf.tm_low)[cur+1].tm * 1000000;
			    beg = per*(beg/per);
			    if( (*buf.tm_low)[cur].val == eval ) mEvalCnt--;
			    (*buf.tm_low)[cur] = b_el;
			}
			//>>>> Update cursor
			if( (++cur >= size) && size )
			{
			    cur = 0;
			    //>>>> Memory consumption is made optimal for limited size
			    if( buf.tm_low->capacity() > size )
			    {
				vector<SLw> *ns = new vector<SLw>;
				*ns = *buf.tm_low;
				delete buf.tm_low;
				buf.tm_low = ns;
			    }
			}
		    }
		    //>>> Update end time
		    end+=per;
		}
	    }
	}
    }
    //> Proccess flow buffer
    else
    {
	if( hg_res_tm )
	{
	    SHg b_el = { tm, value };
	    if( tm < beg && size && buf.tm_high->size() >= size )
		throw TError("ValBuf",_("Set too old value to buffer."));
	    int c_pos = 0;

	    //>> Half divider
	    int d_win = buf.tm_high->size()/2;
	    while(d_win > HalfDivMinWin)
	    {
		if( tm > (*buf.tm_high)[c_pos+d_win].tm ) c_pos+=d_win;
		d_win/=2;
	    }
	    //>> Scan last window
	    while( true )
	    {
		if( c_pos >= buf.tm_high->size() || tm < (*buf.tm_high)[c_pos].tm )
		{
		    if( b_el.val == eval ) mEvalCnt++;
		    if( c_pos == buf.tm_high->size() )	buf.tm_high->push_back(b_el);
		    else buf.tm_high->insert(buf.tm_high->begin()+c_pos,b_el);
		    if( size && buf.tm_high->size() > size )
		    {
			if( buf.tm_high->begin()->val == eval ) mEvalCnt--;
			buf.tm_high->erase(buf.tm_high->begin());
		    }
		    beg = (*buf.tm_high)[0].tm;
		    end = (*buf.tm_high)[buf.tm_high->size()-1].tm;
		    return;
		}
		else if( tm == (*buf.tm_high)[c_pos].tm )
		{
		    if( (*buf.tm_high)[c_pos].val == eval ) mEvalCnt--;
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
	    //>> Half divider
	    int d_win = buf.tm_low->size()/2;
	    while(d_win > HalfDivMinWin)
	    {
		if( tm/1000000 > (*buf.tm_low)[c_pos+d_win].tm ) c_pos += d_win;
		d_win /= 2;
	    }
	    //>> Scan last window
	    while( true )
	    {
		if( c_pos >= buf.tm_low->size() || tm/1000000 < (*buf.tm_low)[c_pos].tm )
		{
		    if( b_el.val == eval ) mEvalCnt++;
		    if( c_pos == buf.tm_low->size() ) buf.tm_low->push_back(b_el);
		    else buf.tm_low->insert(buf.tm_low->begin()+c_pos,b_el);
		    if( size && buf.tm_low->size() > size )
		    {
			if( buf.tm_low->begin()->val == eval ) mEvalCnt--;
			buf.tm_low->erase(buf.tm_low->begin());
		    }
		    beg = (long long)(*buf.tm_low)[0].tm*1000000;
		    end = (long long)(*buf.tm_low)[buf.tm_low->size()-1].tm*1000000;
		    return;
		}
		else if( tm/1000000 == (*buf.tm_low)[c_pos].tm )
		{
		    if( (*buf.tm_low)[c_pos].val == eval ) mEvalCnt--;
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
    TConfig(cf_el), run_st(false), mDB(idb),
    mId(cfg("ID").getSd()), mName(cfg("NAME").getSd()), mDscr(cfg("DESCR").getSd()), mStart(cfg("START").getBd()),
    mVType(cfg("VTYPE").getId()), mBPer(cfg("BPER").getRd()), mBSize(cfg("BSIZE").getId()),
    mBHGrd(cfg("BHGRD").getBd()), mBHRes(cfg("BHRES").getBd()), mSrcMode(cfg("SrcMode").getId()),
    mDSourc(cfg("Source").getSd()), mArchs(cfg("ArchS").getSd())
{
    mId = iid;
    mVType = TFld::Real;

    setUpBuf();
}

TVArchive::~TVArchive( )
{

}

TCntrNode &TVArchive::operator=( TCntrNode &node )
{
    TVArchive *src_n = dynamic_cast<TVArchive*>(&node);
    if( !src_n ) return *this;

    //> Configuration copy
    string tid = id();
    *(TConfig*)this = *(TConfig*)src_n;
    mId = tid;
    mDB = src_n->mDB;

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
	    SYS->db().at().dataDel(fullDB(),owner().nodePath()+tbl(),*this,true);
    }catch(TError err)
    { mess_warning(err.cat.c_str(),"%s",err.mess.c_str()); }
}

string TVArchive::name( )	{ return mName.size() ? mName : mId; }

TArchiveS &TVArchive::owner( )	{ return *(TArchiveS *)nodePrev(); }

string TVArchive::tbl( )	{ return owner().subId()+"_val"; }

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

long long TVArchive::period( const string &arch )
{
    if( arch.empty() || arch == BUF_ARCH_NM ) return TValBuf::period();
    ResAlloc res(a_res,false);
    for( int i_a = 0; i_a < arch_el.size(); i_a++ )
	if( arch == arch_el[i_a]->archivator().workId() )
	    return (long long)(1e6*arch_el[i_a]->archivator().valPeriod());
    return 0;
}

void TVArchive::setValType( TFld::Type vl )
{
    mVType = vl;
    setUpBuf( );
    modif( );
}

void TVArchive::setHardGrid( bool vl )
{
    mBHGrd = vl;
    setUpBuf( );
    modif( );
}

void TVArchive::setHighResTm( bool vl )
{
    mBHRes = vl;
    setUpBuf( );
    modif( );
}

void TVArchive::setSize( int vl )
{
    mBSize = vmax(10,vmin(10000000,vl));
    setUpBuf( );
    modif( );
}

void TVArchive::setPeriod( long long vl )
{
    if( !vl )	vl = 1000000;
    mBPer = (double)vl/1000000.;
    mBSize = vmax(10,(int)vmin(1e7,100.0/mBPer));
    setUpBuf( );
    modif( );
}

void TVArchive::setUpBuf( )
{
    makeBuf( (TFld::Type)mVType, mBSize, (long long)(mBPer*1000000.), mBHGrd, mBHRes );
}

void TVArchive::load_( )
{
    if( !SYS->chkSelDB(DB()) ) return;
    SYS->db().at().dataGet(fullDB(),owner().nodePath()+tbl(),*this);
    setUpBuf();
}

void TVArchive::save_( )
{
    //> Update Archivators list
    if( startStat() )
    {
	vector<string> arch_ls;
	archivatorList(arch_ls);
	mArchs = "";
	for( int i_l = 0; i_l < arch_ls.size(); i_l++ )
	    mArchs += arch_ls[i_l]+";";
    }

    SYS->db().at().dataSet(fullDB(),owner().nodePath()+tbl(),*this);
}

void TVArchive::start( )
{
    if( run_st ) return;

    run_st = true;

    try
    {
	setSrcMode((TVArchive::SrcMode)mSrcMode,mDSourc);

	//> Attach to archivators
	string arch;
	for( int i_off = 0; (arch = TSYS::strSepParse(mArchs,0,';',&i_off)).size();  )
	    if( !archivatorPresent(arch) )
		try{ archivatorAttach(arch); }
		catch(TError err)	{ mess_err(err.cat.c_str(),"%s",err.mess.c_str()); }
    }catch(...){ run_st = false; throw; }
}

void TVArchive::stop( bool full_del )
{
    if( !run_st )	return;

    run_st = false;

    setSrcMode((TVArchive::SrcMode)mSrcMode,mDSourc);

    //> Detach all archivators
    vector<string> arch_ls;
    archivatorList(arch_ls);
    for( int i_l = 0; i_l < arch_ls.size(); i_l++ )
	archivatorDetach(arch_ls[i_l],full_del);
}

void TVArchive::setSrcMode( SrcMode vl, const string &isrc )
{
    //> Disable all links
    if( (!run_st || vl != ActiveAttr || isrc != mDSourc) && !pattr_src.freeStat() )
    {
	owner().setActValArch( id(), false );
	pattr_src.free();
	dynamic_cast<TVal&>(SYS->nodeAt(mDSourc,0,'.').at()).setArch(AutoHD<TVArchive>());
    }

    try
    {
	if( (!run_st || vl != PassiveAttr || isrc != mDSourc) &&
		dynamic_cast<TVal*>(&SYS->nodeAt(mDSourc,0,'.').at()) )
	    dynamic_cast<TVal&>(SYS->nodeAt(mDSourc,0,'.').at()).setArch(AutoHD<TVArchive>());
    }catch(...){  }

    //> Set all links
    if( run_st && vl == ActiveAttr && dynamic_cast<TVal*>(&SYS->nodeAt(isrc,0,'.').at()) )
    {
	dynamic_cast<TVal&>(SYS->nodeAt(isrc,0,'.').at()).setArch( AutoHD<TVArchive>(this) );
	pattr_src = SYS->nodeAt( isrc, 0, '.' );
	owner().setActValArch( id(), true );
    }

    if( run_st && vl == PassiveAttr && dynamic_cast<TVal*>(&SYS->nodeAt(isrc,0,'.').at()) )
	dynamic_cast<TVal&>(SYS->nodeAt(isrc,0,'.').at()).setArch( AutoHD<TVArchive>(this) );

    if( mSrcMode != vl || mDSourc != isrc )	modif();

    mSrcMode = vl;
    mDSourc = isrc;
}

TVariant TVArchive::getVal( long long *tm, bool up_ord, const string &arch, bool onlyLocal )
{
    //> Get from buffer
    if( (arch.empty() || arch == BUF_ARCH_NM) && (!tm || (*tm >= begin() && *tm <= end())) )
	switch( TValBuf::valType() )
	{
	    case TFld::Integer:	return TValBuf::getI(tm,up_ord);
	    case TFld::String:	return TValBuf::getS(tm,up_ord);
	    case TFld::Real:	return TValBuf::getR(tm,up_ord);
	    case TFld::Boolean:	return TValBuf::getB(tm,up_ord);
	}
    //> Get from archivators
    else
    {
	ResAlloc res(a_res,false);
	for( int i_a = 0; i_a < arch_el.size(); i_a++ )
	    if( (arch.empty() || arch == arch_el[i_a]->archivator().workId()) &&
		    (!tm ||
			(up_ord && *tm <= arch_el[i_a]->end() && *tm > arch_el[i_a]->begin()-(long long)(1000000.*arch_el[i_a]->archivator( ).valPeriod())) ||
			(!up_ord && *tm < arch_el[i_a]->end()+(long long)(1000000.*arch_el[i_a]->archivator( ).valPeriod()) && *tm >= arch_el[i_a]->begin()) ) )
		return arch_el[i_a]->getVal(tm,up_ord,onlyLocal);

    }
    return EVAL_REAL;
}

void TVArchive::getVals( TValBuf &buf, long long ibeg, long long iend, const string &arch, int limit, bool onlyLocal )
{
    //> Get from buffer
    if( (arch.empty() || arch == BUF_ARCH_NM) && vOK(ibeg,iend) )
    {
	ibeg = vmax(ibeg,iend-TValBuf::period()*limit);
	TValBuf::getVals(buf,ibeg,iend);
	iend = buf.begin()-1;
	if( arch == BUF_ARCH_NM ) return;
    }

    //> Get from archivators
    ResAlloc res(a_res,false);
    for( int i_a = 0; i_a < arch_el.size(); i_a++ )
	if( (arch.empty() || arch == arch_el[i_a]->archivator().workId()) &&
		((!ibeg || ibeg <= arch_el[i_a]->end()) && (!iend || iend > arch_el[i_a]->begin())) && ibeg <= iend )
	{
	    //> Local request to data
	    if( !buf.size() )
		ibeg = vmax(ibeg,iend-(long long)(1000000.*arch_el[i_a]->archivator().valPeriod())*(limit-buf.realSize()));
	    arch_el[i_a]->getVals(buf,ibeg,iend,onlyLocal);
	    iend = buf.begin()-1;
	}
}

void TVArchive::setVals( TValBuf &buf, long long ibeg, long long iend, const string &arch )
{
    //> Check for put to buffer
    if( (arch.empty() || arch == BUF_ARCH_NM) && iend > TValBuf::begin() )
	TValBuf::setVals(buf,vmax(ibeg,iend-TValBuf::size()*TValBuf::period()),iend);

    //> Put to archivators
    ResAlloc res(a_res,false);
    for( int i_a = 0; i_a < arch_el.size(); i_a++ )
	if( (arch.empty() || arch == arch_el[i_a]->archivator().workId()) &&
		(!arch_el[i_a]->lastGet() || ibeg < arch_el[i_a]->lastGet()) )
	    arch_el[i_a]->setVals(buf,ibeg,vmin(iend,arch_el[i_a]->lastGet()));
}

void TVArchive::getActiveData()
{
    if( pattr_src.freeStat() )	return;

    long long tm = TSYS::curTime();
    switch( valType() )
    {
	case TFld::Boolean:	{ char vl = pattr_src.at().getB(&tm); setB(vl,tm); break; }
	case TFld::Integer:	{ int vl = pattr_src.at().getI(&tm); setI(vl,tm); break; }
	case TFld::Real:	{ double vl = pattr_src.at().getR(&tm); setR(vl,tm); break; }
	case TFld::String:	{ string vl = pattr_src.at().getS(&tm); setS(vl,tm); break; }
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

    AutoHD<TVArchivator> archivat = owner().at(TSYS::strSepParse(arch,0,'.')).at().valAt(TSYS::strSepParse(arch,1,'.'));

    if(!archivat.at().startStat())
	throw TError(nodePath().c_str(),"Archivator <%s> error or it is not started.",arch.c_str());

    //> Find already present archivator
    for( int i_l = 0; i_l < arch_el.size(); i_l++ )
        if( &arch_el[i_l]->archivator() == &archivat.at() )
	    return;
    //> Find position
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
    //> Find archivator
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
    string lab_tm, lab_dt;
    time_t tm_t;
    struct tm ttm, ttm1;
    long long c_tm;
    string rez;
    int hv_border,		//Image border size
	h_w_start, h_w_size,	//Trend window horizontal start and size
	v_w_start, v_w_size;	//Trend window vertical start and size
    string sclMarkFont = "Times";
    int mrkFontSize = 8, begMarkBrd = -1, endMarkBrd;

    //> Check and get data
    if( ibeg >= iend || valType( ) == TFld::String )	return rez;
    TValBuf buf( TFld::Real, 0, 0, false, true );

    //> Calc base image parameters
    hv_border = 5;
    h_w_start = hv_border;
    h_w_size  = hsz-h_w_start-hv_border;
    v_w_start = hv_border;
    v_w_size  = vsz-v_w_start-hv_border;

    int mrkHeight = 0;
    int brect[8];
    char *gdR = gdImageStringFT(NULL,&brect[0],0,(char*)sclMarkFont.c_str(),mrkFontSize,0.,0,0,"000000");
    if( gdR ) mess_err(nodePath().c_str(),_("gdImageStringFT for font '%s' error: %s."),sclMarkFont.c_str(),gdR);
    else mrkHeight = brect[3]-brect[7];
    //if( mrkHeight <= 0 ) return rez;

    v_w_size -= 2*mrkHeight;

    //> Create image
    gdImagePtr im = gdImageCreate(hsz,vsz);
    int clr_backgr = gdImageColorAllocate(im,0x35,0x35,0x35);
    int clr_grid   = gdImageColorAllocate(im,0x8e,0x8e,0x8e);
    int clr_symb   = gdImageColorAllocate(im,0x11,0xff,0x5f);
    int clr_trnd   = gdImageColorAllocate(im,0x1f,0xf2,0xff);

    gdImageFilledRectangle(im,0,0,hsz-1,vsz-1,clr_backgr);
    gdImageRectangle(im,h_w_start,v_w_start,h_w_start+h_w_size,v_w_start+v_w_size,clr_grid);

    //> Make horisontal grid and symbols
    long long h_div = 1;
    long long h_min = ibeg;
    long long h_max = iend;
    int hmax_ln = vsz/(mrkHeight?mrkHeight:10);
    if( hmax_ln >= 2 )
    {
	int hvLev = 0;
	long long hLen = iend - ibeg;
	if( hLen/86400000000ll >= 2 )		{ hvLev = 5; h_div = 86400000000ll; }	//Days
	else if( hLen/3600000000ll >= 2 )	{ hvLev = 4; h_div =  3600000000ll; }	//Hours
	else if( hLen/60000000 >= 2 )		{ hvLev = 3; h_div =    60000000; }	//Minutes
	else if( hLen/1000000 >= 2 )		{ hvLev = 2; h_div =     1000000; }	//Seconds
	else if( hLen/1000 >= 2 )		{ hvLev = 1; h_div =        1000; }	//Milliseconds
	while( hLen/h_div > hmax_ln )		h_div *= 10;
	while( hLen/h_div < hmax_ln/2 )		h_div /= 2;

	//>> Select most like archivator
	string rarch = iarch;
	if( rarch.empty() && !vOK(ibeg,iend) )
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

	getVals(buf,h_min,h_max,rarch,600000);
	if(!buf.end() || !buf.begin())	{ gdImageDestroy(im); return rez; }

	//>> Draw full trend's data and time to the trend end position
	if( mrkHeight )
	{
	    tm_t = iend/1000000;
	    localtime_r(&tm_t,&ttm);
	    lab_dt = TSYS::strMess("%d-%02d-%d",ttm.tm_mday,ttm.tm_mon+1,ttm.tm_year+1900);
	    if( ttm.tm_sec == 0 && iend%1000000 == 0 ) lab_tm = TSYS::strMess("%d:%02d",ttm.tm_hour,ttm.tm_min);
	    else if( iend%1000000 == 0 ) lab_tm = TSYS::strMess("%d:%02d:%02d",ttm.tm_hour,ttm.tm_min,ttm.tm_sec);
	    else lab_tm = TSYS::strMess("%d:%02d:%g",ttm.tm_hour,ttm.tm_min,(float)ttm.tm_sec+(float)(iend%1000000)/1e6);

	    gdImageStringFT(NULL,&brect[0],0,(char*)sclMarkFont.c_str(),mrkFontSize,0.0,0,0,(char*)lab_dt.c_str());
	    int markBrd = h_w_start+h_w_size-(brect[2]-brect[6]);
	    endMarkBrd = markBrd;
	    gdImageStringFT(im,NULL,clr_symb,(char*)sclMarkFont.c_str(),mrkFontSize,0.0,markBrd,v_w_start+v_w_size+3+2*mrkHeight,(char*)lab_dt.c_str());
	    gdImageStringFT(NULL,&brect[0],0,(char*)sclMarkFont.c_str(),mrkFontSize,0.0,0,0,(char*)lab_tm.c_str());
	    markBrd = h_w_start+h_w_size-(brect[2]-brect[6]);
	    vmin(endMarkBrd,markBrd);
	    gdImageStringFT(im,NULL,clr_symb,(char*)sclMarkFont.c_str(),mrkFontSize,0.0,markBrd,v_w_start+v_w_size+3+mrkHeight,(char*)lab_tm.c_str());
	}

	//>> Draw grid and/or markers
	bool first_m = true;
	for( long long i_h = h_min; true; )
	{
	    //>>> Draw grid
	    int h_pos = h_w_start+h_w_size*(i_h-h_min)/(h_max-h_min);
	    gdImageLine(im,h_pos,v_w_start,h_pos,v_w_start+v_w_size,clr_grid);

	    if( mrkHeight && !(i_h%h_div) && i_h != iend )
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
		lab_dt = lab_tm = "";
		//Date
		if( hvLev == 5 || chLev >= 4 )
		    lab_dt = (chLev>=5 || chLev==-1) ? TSYS::strMess("%d-%02d-%d",ttm.tm_mday,ttm.tm_mon+1,ttm.tm_year+1900) : TSYS::strMess("%d",ttm.tm_mday);
		//Hours and minuts
		if( (hvLev == 4 || hvLev == 3 || ttm.tm_min) && !ttm.tm_sec ) lab_tm = TSYS::strMess("%d:%02d",ttm.tm_hour,ttm.tm_min);
		//Seconds
		else if( (hvLev == 2 || ttm.tm_sec) && !(i_h%1000000) )
		    lab_tm = (chLev>=2 || chLev==-1) ? TSYS::strMess("%d:%02d:%02d",ttm.tm_hour,ttm.tm_min,ttm.tm_sec) : TSYS::strMess(_("%ds"),ttm.tm_sec);
		//Milliseconds
		else if( hvLev <= 1 || i_h%1000000 )
		    lab_tm = (chLev>=2 || chLev==-1) ? TSYS::strMess("%d:%02d:%g",ttm.tm_hour,ttm.tm_min,(float)ttm.tm_sec+(float)(i_h%1000000)/1e6) :
			     (chLev>=1) ? TSYS::strMess(_("%gs"),(float)ttm.tm_sec+(float)(i_h%1000000)/1e6) :
					  TSYS::strMess(_("%gms"),(double)(i_h%1000000)/1000.);
		int wdth, tpos, endPosTm = 0, endPosDt = 0;
		if( lab_dt.size() )
		{
		    gdImageStringFT(NULL,&brect[0],0,(char*)sclMarkFont.c_str(),mrkFontSize,0.0,0,0,(char*)lab_dt.c_str());
		    wdth = brect[2]-brect[6];
		    tpos = vmax(h_pos-wdth/2,hv_border);
		    if( (tpos+wdth) < endMarkBrd && tpos > begMarkBrd )
		    {
			gdImageStringFT(im,NULL,clr_symb,(char*)sclMarkFont.c_str(),mrkFontSize,0.0,tpos,v_w_start+v_w_size+3+2*mrkHeight,(char*)lab_dt.c_str());
			endPosTm = tpos+wdth;
		    }
		}
		if( lab_tm.size() )
		{
		    gdImageStringFT(NULL,&brect[0],0,(char*)sclMarkFont.c_str(),mrkFontSize,0.0,0,0,(char*)lab_tm.c_str());
		    wdth = brect[2]-brect[6];
		    tpos = vmax(h_pos-wdth/2,hv_border);
		    if( (tpos+wdth) < endMarkBrd && tpos > begMarkBrd )
		    {
			gdImageStringFT(im,NULL,clr_symb,(char*)sclMarkFont.c_str(),mrkFontSize,0.0,tpos,v_w_start+v_w_size+3+mrkHeight,(char*)lab_tm.c_str());
			endPosDt = tpos+wdth;
		    }
		}
		begMarkBrd = vmax(begMarkBrd,vmax(endPosTm,endPosDt));
		memcpy((char*)&ttm1,(char*)&ttm,sizeof(tm));
		first_m = false;
	    }
	    //>>> Next
	    if( i_h >= iend )	break;
	    i_h = (i_h/h_div)*h_div + h_div;
	    if( i_h > iend )	i_h = iend;
	}
    }

    //> Make vertical grid and symbols
    double	c_val,
		v_max = -3e300,
		v_min = 3e300;
    for( c_tm = buf.begin(); c_tm <= buf.end(); c_tm++ )
    {
	c_val = buf.getR(&c_tm,true);
	if(c_val == EVAL_REAL)	continue;
	v_min = vmin(v_min,c_val);
	v_max = vmax(v_max,c_val);
    }
    if( valmax!=valmin )
    {
	v_max = vmax(v_max,valmax);
	v_min = vmin(v_min,valmin);
    }
    if( v_max==-3e300 )	{ gdImageDestroy(im); return rez; }
    if( TSYS::realRound(v_max,3) == TSYS::realRound(v_min,3) )	{ v_max+=1.0; v_min-=1.0; }
    double v_div = 1;
    double v_len = v_max - v_min;
    while( v_len > 10 )	{ v_div *= 10; v_len /= 10; }
    while( v_len < 1 ) 	{ v_div /= 10; v_len *= 10; }
    v_min = floor(v_min/v_div)*v_div;
    v_max = ceil(v_max/v_div)*v_div;
    while( ((v_max-v_min)/v_div) < 5 ) v_div /= 2;
    //>> Draw vertical grid and symbols
    for( double i_v = v_min; (v_max-i_v)/v_div > -0.1; i_v += v_div )
    {
	int v_pos = v_w_start+v_w_size-(int)((double)v_w_size*(i_v-v_min)/(v_max-v_min));
	gdImageLine(im,h_w_start,v_pos,h_w_start+h_w_size,v_pos,clr_grid);
	bool isMax = (fabs((v_max-i_v)/v_div) < 0.1);
	if( mrkHeight )
	    gdImageStringFT(im,NULL,clr_symb,(char*)sclMarkFont.c_str(),mrkFontSize,0.0,hv_border+2,v_pos+(isMax?mrkHeight:0),
		(char*)TSYS::strMess("%g",i_v).c_str());
    }

    //> Draw trend
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

    //> Get image and transfer it
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
    //> Service commands process
    if( a_path == "/serv/val" )		//Values access
    {
	if( ctrChkNode(opt,"info",RWRWRW,"root","root",SEC_RD) )	//Value's data information
	{
	    string arch = opt->attr("arch");
	    opt->setAttr("end",TSYS::ll2str(end(arch)));
	    opt->setAttr("beg",TSYS::ll2str(begin(arch)));
	    opt->setAttr("vtp",TSYS::int2str(TValBuf::valType()));
	    if( arch.empty() || arch == BUF_ARCH_NM ) opt->setAttr("per",TSYS::ll2str(TValBuf::period()));
	    else
	    {
		ResAlloc res(a_res,false);
		for( int i_a = 0; i_a < arch_el.size(); i_a++ )
		    if( arch == arch_el[i_a]->archivator().workId() )
		    { opt->setAttr("per",TSYS::ll2str((long long)(1000000.*arch_el[i_a]->archivator().valPeriod()))); break;  }
	    }
	}
	else if( ctrChkNode(opt,"get",RWRWRW,"root","root",SEC_RD) )	//Value's data request
	{
	    long long	tm	= atoll(opt->attr("tm").c_str());
	    long long	tm_grnd	= atoll(opt->attr("tm_grnd").c_str());
	    string	arch	= opt->attr("arch");
	    bool	local	= atoi(opt->attr("local").c_str());

	    //>> Process one value request
	    if( !tm )	tm = TSYS::curTime();
	    if( !tm_grnd )
	    {
		opt->setText(getVal(&tm,false,arch,local).getS());
		opt->setAttr("tm",TSYS::ll2str(tm));
		return;
	    }
	    if( tm < tm_grnd )	throw TError(nodePath().c_str(),"Range error");

	    long long period = atoll(opt->attr("per").c_str());

	    //>> Process of archive block request
	    TValBuf buf( TValBuf::valType(), 100000, TValBuf::period(), true, true );

	    //>>> Get values buffer
	    if( (arch.empty() || arch == BUF_ARCH_NM) && vOK(tm_grnd,tm) )
	    {
		TValBuf::getVals(buf,tm_grnd,tm);
		opt->setAttr("arch",BUF_ARCH_NM);
	    }
	    else
	    {
		ResAlloc res( a_res, false );
		for( int i_a = (arch_el.size()-1); i_a >= 0; i_a-- )
		    if( ((arch.empty() && (!i_a || arch_el[i_a]->archivator().valPeriod() <= (period/1e6))) || arch == arch_el[i_a]->archivator().workId()) &&
			(tm_grnd <= arch_el[i_a]->end() && tm > arch_el[i_a]->begin()) )
		    {
			buf.setPeriod( (long long)(1000000.*arch_el[i_a]->archivator().valPeriod()) );
			arch_el[i_a]->getVals( buf, tm_grnd, tm, local );
			opt->setAttr("arch",arch_el[i_a]->archivator().workId());
			break;
		    }
		res.release();
	    }
	    //>>> Prepare buffer's data for transfer
	    bool    isEnd = false;	//Last archive value process
	    string  text;
	    text.reserve(100);
	    int vpos_beg = 0, vpos_end = 0, vpos_cur;
	    long long ibeg = buf.begin(), iend = buf.end();
	    period = vmax(period,buf.period());
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
	    //>>> Set result and information attributes
	    opt->setAttr("tm",TSYS::ll2str( (buf.end()/period)*period ));
	    opt->setAttr("tm_grnd",TSYS::ll2str( (buf.begin()/period)*period + ((buf.begin()%period)?period:0) ));
	    opt->setAttr("per",TSYS::ll2str(period));
	    opt->setText( (mode==2) ? TSYS::strEncode(text,TSYS::base64) : text);
	    opt->setAttr("vtp",TSYS::int2str(TValBuf::valType()));
	}
	return;
    }

    //- Interface comands process -
    //-- Info command process --
    if( opt->name() == "info" )
    {
	TCntrNode::cntrCmdProc(opt);
	ctrMkNode("oscada_cntr",opt,-1,"/",_("Value archive: ")+name(),0664,"root","Archive");
	if(ctrMkNode("area",opt,-1,"/prm",_("Archive")))
	{
	    if(ctrMkNode("area",opt,-1,"/prm/st",_("State")))
	    {
		ctrMkNode("fld",opt,-1,"/prm/st/st",_("Runing"),0664,"root","Archive",1,"tp","bool");
		ctrMkNode("fld",opt,-1,"/prm/st/bEnd",_("Buffer end"),0444,"root","Archive",1,"tp","str");
		ctrMkNode("fld",opt,-1,"/prm/st/bBeg",_("Buffer begin"),0444,"root","Archive",1,"tp","str");
		ctrMkNode("fld",opt,-1,"/prm/st/db",_("Archive DB"),0664,"root","Archive",4,"tp","str","dest","select","select","/db/list",
		    "help",_("DB address in format [<DB module>.<DB name>].\nFor use main work DB set '*.*'."));
	    }
	    if(ctrMkNode("area",opt,-1,"/prm/cfg",_("Config")))
	    {
		ctrMkNode("fld",opt,-1,"/prm/cfg/id",cfg("ID").fld().descr(),0444,"root","Archive",1,"tp","str");
		ctrMkNode("fld",opt,-1,"/prm/cfg/nm",cfg("NAME").fld().descr(),0664,"root","Archive",2,"tp","str","len","50");
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
	    ctrMkNode("fld",opt,-1,"/val/size",_("Size (s)"),0660,"root","Archive",1,"tp","real");
	    ctrMkNode("fld",opt,-1,"/val/arch",_("Archivator"),0660,"root","Archive",4,"tp","str","dest","select","select","/val/lstAVal",
		"help",_("Values archivator.\nNo set archivator for process by buffer and all archivators.\nSet '<buffer>' for process by buffer."));
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
    //>> Process command to page
    if( a_path == "/prm/st/st" )
    {
	if( ctrChkNode(opt,"get",0664,"root","Archive",SEC_RD) )	opt->setText( startStat() ? "1" : "0" );
	if( ctrChkNode(opt,"set",0664,"root","Archive",SEC_WR) )	atoi(opt->text().c_str()) ? start() : stop();
    }
    if( a_path == "/prm/st/bEnd" && ctrChkNode(opt) )
    {
	struct tm ttm;
	time_t tm_t = end(BUF_ARCH_NM)/1000000; localtime_r(&tm_t,&ttm);
	opt->setText(TSYS::strMess("%d-%02d-%d %d:%02d:%02d.%d",ttm.tm_mday,ttm.tm_mon+1,ttm.tm_year+1900,ttm.tm_hour,ttm.tm_min,ttm.tm_sec,end(BUF_ARCH_NM)%1000000));
    }
    if( a_path == "/prm/st/bBeg" && ctrChkNode(opt) )
    {
	struct tm ttm;
	time_t tm_t = begin(BUF_ARCH_NM)/1000000; localtime_r(&tm_t,&ttm);
	opt->setText(TSYS::strMess("%d-%02d-%d %d:%02d:%02d.%d",ttm.tm_mday,ttm.tm_mon+1,ttm.tm_year+1900,ttm.tm_hour,ttm.tm_min,ttm.tm_sec,begin(BUF_ARCH_NM)%1000000));
    }
    else if( a_path == "/prm/st/db" )
    {
	if( ctrChkNode(opt,"get",0664,"root","Archive",SEC_RD) )	opt->setText( DB() );
	if( ctrChkNode(opt,"set",0664,"root","Archive",SEC_WR) )	setDB( opt->text() );
    }
    else if( a_path == "/prm/cfg/id" && ctrChkNode(opt) )		opt->setText(id());
    else if( a_path == "/prm/cfg/nm" )
    {
	if( ctrChkNode(opt,"get",0664,"root","Archive",SEC_RD) )	opt->setText( name() );
	if( ctrChkNode(opt,"set",0664,"root","Archive",SEC_WR) )	setName( opt->text() );
    }
    else if( a_path == "/prm/cfg/dscr" )
    {
	if( ctrChkNode(opt,"get",0664,"root","Archive",SEC_RD) )	opt->setText( dscr() );
	if( ctrChkNode(opt,"set",0664,"root","Archive",SEC_WR) )	setDscr( opt->text() );
    }
    else if( a_path == "/prm/cfg/start" )
    {
	if( ctrChkNode(opt,"get",0664,"root","Archive",SEC_RD) )	opt->setText( toStart() ? "1" : "0" );
	if( ctrChkNode(opt,"set",0664,"root","Archive",SEC_WR) )	setToStart( atoi(opt->text().c_str()) );
    }
    else if( a_path == "/prm/cfg/vtp" )
    {
	if( ctrChkNode(opt,"get",0664,"root","Archive",SEC_RD) )	opt->setText(TSYS::int2str(mVType));
	if( ctrChkNode(opt,"set",0664,"root","Archive",SEC_WR) )	setValType((TFld::Type)atoi(opt->text().c_str()));
    }
    else if( a_path == "/prm/cfg/srcm" )
    {
	if( ctrChkNode(opt,"get",0664,"root","Archive",SEC_RD) )	opt->setText(TSYS::int2str(mSrcMode));
	if( ctrChkNode(opt,"set",0664,"root","Archive",SEC_WR) )	setSrcMode((TVArchive::SrcMode)atoi(opt->text().c_str()),mDSourc);
    }
    else if( a_path == "/prm/cfg/src" )
    {
	if( ctrChkNode(opt,"get",0664,"root","Archive",SEC_RD) )	opt->setText(mDSourc);
	if( ctrChkNode(opt,"set",0664,"root","Archive",SEC_WR) )	setSrcMode((TVArchive::SrcMode)mSrcMode,opt->text());
    }
    else if( a_path == "/prm/cfg/b_per" )
    {
	if( ctrChkNode(opt,"get",0664,"root","Archive",SEC_RD) )	opt->setText(TSYS::real2str(mBPer,6));
	if( ctrChkNode(opt,"set",0664,"root","Archive",SEC_WR) )	setPeriod((long long)(1000000.*atof(opt->text().c_str())));
    }
    else if( a_path == "/prm/cfg/b_size" )
    {
	if( ctrChkNode(opt,"get",0664,"root","Archive",SEC_RD) )	opt->setText(TSYS::int2str(mBSize));
	if( ctrChkNode(opt,"set",0664,"root","Archive",SEC_WR) )	setSize(atoi(opt->text().c_str()));
    }
    else if( a_path == "/prm/cfg/b_hgrd" )
    {
	if( ctrChkNode(opt,"get",0664,"root","Archive",SEC_RD) )	opt->setText(mBHGrd?"1":"0");
	if( ctrChkNode(opt,"set",0664,"root","Archive",SEC_WR) )	setHardGrid(atoi(opt->text().c_str()));
    }
    else if( a_path == "/prm/cfg/b_hres" )
    {
	if( ctrChkNode(opt,"get",0664,"root","Archive",SEC_RD) )	opt->setText(mBHRes?"1":"0");
	if( ctrChkNode(opt,"set",0664,"root","Archive",SEC_WR) )	setHighResTm(atoi(opt->text().c_str()));
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
	opt->childAdd("el")->setAttr("id",TSYS::int2str(TVArchive::PassiveAttr))->setText(_("Passive param. attribute"));
	opt->childAdd("el")->setAttr("id",TSYS::int2str(TVArchive::ActiveAttr))->setText(_("Active param. attribute"));
    }
    else if( a_path == "/cfg/prm_atr_ls" && ctrChkNode(opt) )
    {
	vector<string> list;
	int c_lv = 0;
	string c_path = "", c_el;
	for( int c_off = 0; (c_el=TSYS::strSepParse(mDSourc,0,'.',&c_off)).size(); c_lv++ )
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
	if( ctrChkNode(opt,"get",0664,"root","Archive",SEC_RD) )
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
			struct tm ttm;
			if(n_end)
			{
			    time_t tm_t = a_el->end()/1000000; localtime_r(&tm_t,&ttm);
			    n_end->childAdd("el")->setText(TSYS::strMess("%d-%02d-%d %d:%02d:%02d.%d",
				    ttm.tm_mday,ttm.tm_mon+1,ttm.tm_year+1900,ttm.tm_hour,ttm.tm_min,ttm.tm_sec,a_el->end()%1000000));
			}
			if(n_beg)
			{
			    time_t tm_t = a_el->begin()/1000000; localtime_r(&tm_t,&ttm);
			    n_beg->childAdd("el")->setText(TSYS::strMess("%d-%02d-%d %d:%02d:%02d.%d",
				    ttm.tm_mday,ttm.tm_mon+1,ttm.tm_year+1900,ttm.tm_hour,ttm.tm_min,ttm.tm_sec,a_el->begin()%1000000));
			}
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
	if( ctrChkNode(opt,"set",0664,"root","Archive",SEC_WR) )
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
	if( ctrChkNode(opt,"get",0660,"root","Archive",SEC_RD) )
	{
	    opt->setText(TBDS::genDBGet(owner().nodePath()+"vaTm","0",opt->attr("user")));
	    if( !atoi(opt->text().c_str()) )	opt->setText(TSYS::int2str(time(NULL)));
	}
	if( ctrChkNode(opt,"set",0660,"root","Archive",SEC_WR) )
	    TBDS::genDBSet(owner().nodePath()+"vaTm",(atoi(opt->text().c_str())>=time(NULL))?"0":opt->text(),opt->attr("user"));
    }
    else if( a_path == "/val/utm" )
    {
	if( ctrChkNode(opt,"get",0660,"root","Archive",SEC_RD) )	opt->setText(TBDS::genDBGet(owner().nodePath()+"vaTm_u","0",opt->attr("user")));
	if( ctrChkNode(opt,"set",0660,"root","Archive",SEC_WR) )	TBDS::genDBSet(owner().nodePath()+"vaTm_u",opt->text(),opt->attr("user"));
    }
    else if( a_path == "/val/size" )
    {
	if( ctrChkNode(opt,"get",0660,"root","Archive",SEC_RD) )	opt->setText(TBDS::genDBGet(owner().nodePath()+"vaSize","1",opt->attr("user")));
	if( ctrChkNode(opt,"set",0660,"root","Archive",SEC_WR) )	TBDS::genDBSet(owner().nodePath()+"vaSize",opt->text(),opt->attr("user"));
    }
    else if( a_path == "/val/arch")
    {
	if( ctrChkNode(opt,"get",0660,"root","Archive",SEC_RD) )	opt->setText(TBDS::genDBGet(owner().nodePath()+"vArch","",opt->attr("user")));
	if( ctrChkNode(opt,"set",0660,"root","Archive",SEC_WR) )	TBDS::genDBSet(owner().nodePath()+"vArch",opt->text(),opt->attr("user"));
    }
    else if( a_path == "/val/sw_trend" )
    {
	if( ctrChkNode(opt,"get",0660,"root","Archive",SEC_RD) )	opt->setText(TBDS::genDBGet(owner().nodePath()+"vShowTrnd","0",opt->attr("user")));
	if( ctrChkNode(opt,"set",0660,"root","Archive",SEC_WR) )	TBDS::genDBSet(owner().nodePath()+"vShowTrnd",opt->text(),opt->attr("user"));
    }
    else if( a_path == "/val/pct_w" )
    {
	if( ctrChkNode(opt,"get",0660,"root","Archive",SEC_RD) )	opt->setText(TBDS::genDBGet(owner().nodePath()+"vPctW","650",opt->attr("user")));
	if( ctrChkNode(opt,"set",0660,"root","Archive",SEC_WR) )	TBDS::genDBSet(owner().nodePath()+"vPctW",opt->text(),opt->attr("user"));
    }
    else if( a_path == "/val/pct_h" )
    {
	if( ctrChkNode(opt,"get",0660,"root","Archive",SEC_RD) )	opt->setText(TBDS::genDBGet(owner().nodePath()+"vPctH","230",opt->attr("user")));
	if( ctrChkNode(opt,"set",0660,"root","Archive",SEC_WR) )	TBDS::genDBSet(owner().nodePath()+"vPctH",opt->text(),opt->attr("user"));
    }
    else if( a_path == "/val/max" )
    {
	if( ctrChkNode(opt,"get",0660,"root","Archive",SEC_RD) )	opt->setText(TBDS::genDBGet(owner().nodePath()+"vMax","0",opt->attr("user")));
	if( ctrChkNode(opt,"set",0660,"root","Archive",SEC_WR) )	TBDS::genDBSet(owner().nodePath()+"vMax",opt->text(),opt->attr("user"));
    }
    else if( a_path == "/val/min" )
    {
        if( ctrChkNode(opt,"get",0660,"root","Archive",SEC_RD) )	opt->setText(TBDS::genDBGet(owner().nodePath()+"vMin","0",opt->attr("user")));
        if( ctrChkNode(opt,"set",0660,"root","Archive",SEC_WR) )	TBDS::genDBSet(owner().nodePath()+"vMin",opt->text(),opt->attr("user"));
    }
    else if( a_path == "/val/lstAVal" && ctrChkNode(opt,"get",0440) )
    {
	opt->childAdd("el")->setText("");
	opt->childAdd("el")->setText(BUF_ARCH_NM);
	vector<string> lsm, lsa;
	owner().modList(lsm);
	for( int i_m = 0; i_m < lsm.size(); i_m++ )
	{
	    owner().at(lsm[i_m]).at().valList(lsa);
	    for( int i_a = 0; i_a < lsa.size(); i_a++ )
	    opt->childAdd("el")->setText(lsm[i_m]+"."+lsa[i_a]);
	}
    }
    else if( a_path == "/val/val" && ctrChkNode(opt,"get",0440,"root","Archive",SEC_RD) )
    {
	long long end = (long long)atoi(TBDS::genDBGet(owner().nodePath()+"vaTm","0",opt->attr("user")).c_str())*1000000;
	if( !(end/1000000) ) end = (long long)time(NULL) * 1000000;
	end += atoi(TBDS::genDBGet(owner().nodePath()+"vaTm_u","0",opt->attr("user")).c_str());
	long long beg = end - (long long)(atof(TBDS::genDBGet(owner().nodePath()+"vaSize","1",opt->attr("user")).c_str())*1e6);

	TValBuf buf( TFld::String, 0, 0, false, true );
	getVals( buf, beg, end, TBDS::genDBGet(owner().nodePath()+"vArch","",opt->attr("user")), 2000 );

	XMLNode *n_tm   = ctrMkNode("list",opt,-1,"/val/val/0","",0440);
	XMLNode *n_val  = ctrMkNode("list",opt,-1,"/val/val/1","",0440);

	struct tm ttm;
	time_t tm_t;
	long long c_tm = buf.begin();
	if(buf.end() && buf.begin())
	    while(c_tm <= buf.end())
	    {
	        string val = buf.getS(&c_tm,true);
		if( n_tm )
		{
		    tm_t = c_tm/1000000;
		    localtime_r(&tm_t,&ttm);
		    n_tm->childAdd("el")->setText(TSYS::strMess("%d-%02d-%d %d:%02d:%02d.%d",
			ttm.tm_mday,ttm.tm_mon+1,ttm.tm_year+1900,ttm.tm_hour,ttm.tm_min,ttm.tm_sec,c_tm%1000000));
		}
		if( n_val )	n_val->childAdd("el")->setText(val);
		c_tm++;
	    }
    }
    else if( a_path == "/val/trend" && ctrChkNode(opt,"get",0444,"root","Archive",SEC_RD) )
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
TVArchivator::TVArchivator( const string &iid, const string &idb, TElem *cf_el ) : run_st(false), endrunReq(false),
    tm_calc(0.0), TConfig(cf_el), mDB(idb), mId(cfg("ID").getSd()), mName(cfg("NAME").getSd()),
    mDscr(cfg("DESCR").getSd()), mAddr(cfg("ADDR").getSd()), mStart(cfg("START").getBd()),
    mVPer(cfg("V_PER").getRd()), mAPer(cfg("A_PER").getId())
{
    mId = iid;
}

TVArchivator::~TVArchivator()
{

}

TCntrNode &TVArchivator::operator=( TCntrNode &node )
{
    TVArchivator *src_n = dynamic_cast<TVArchivator*>(&node);
    if( !src_n ) return *this;

    //> Configuration copy
    string tid = id();
    *(TConfig*)this = *(TConfig*)src_n;
    mId = tid;
    mDB = src_n->mDB;

    if( src_n->startStat() && toStart() && !startStat() )
        start( );

    return *this;
}

string TVArchivator::name( )	{ return mName.size() ? mName : mId; }

string TVArchivator::tbl( )	{ return owner().owner().subId()+"_val_proc"; }

void TVArchivator::setValPeriod( double iper )
{
    mVPer = iper ? iper : 1.;

    //> Call sort for all archives
    ResAlloc res(a_res,false);
    for( map<string,TVArchEl*>::iterator iel = archEl.begin(); iel != archEl.end(); ++iel )
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
	    SYS->db().at().dataDel(fullDB(),SYS->archive().at().nodePath()+tbl(),*this,true);
    }catch(TError err)
    { mess_warning(err.cat.c_str(),"%s",err.mess.c_str()); }
}

string TVArchivator::workId()
{
    return owner().modId()+"."+id();
}

void TVArchivator::start()
{
    if( run_st ) return;

    //> Start archivator thread
    SYS->taskCreate( nodePath('.',true), 0, TVArchivator::Task, this, &run_st );
}

void TVArchivator::stop( bool full_del )
{
    if( !run_st ) return;

    //> Values acquisition task stop
    SYS->taskDestroy( nodePath('.',true), &run_st, &endrunReq );

    //> Detach from all archives
    ResAlloc res(a_res,false);
    while(archEl.size())
    {
	AutoHD<TVArchive> arch(&archEl.begin()->second->archive());
	res.release();
	arch.at().archivatorDetach(workId(),full_del);
	res.request(false);
    }
}

void TVArchivator::archiveList( vector<string> &ls )
{
    ResAlloc res(a_res,false);
    ls.clear();
    for( map<string,TVArchEl*>::iterator iel = archEl.begin(); iel != archEl.end(); ++iel )
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
    if( !SYS->chkSelDB(DB()) ) return;
    SYS->db().at().dataGet(fullDB(),SYS->archive().at().nodePath()+tbl(),*this);
}

void TVArchivator::save_( )
{
    SYS->db().at().dataSet(fullDB(),SYS->archive().at().nodePath()+tbl(),*this);
}

void *TVArchivator::Task( void *param )
{
    TVArchivator &arch = *(TVArchivator*)param;
    arch.endrunReq = false;
    arch.run_st = true;
    bool isLast = false;

    sleep( arch.archPeriod() );

    //> Archiving
    while( true )
	try
	{
	    if( arch.endrunReq ) isLast = true;

	    long long t_cnt = TSYS::curTime();

	    ResAlloc res(arch.a_res,false);
	    long long beg, end;
	    for( map<string,TVArchEl*>::iterator iel = arch.archEl.begin(); iel != arch.archEl.end(); ++iel )
		if( iel->second->archive().startStat() )
		{
		    TVArchEl *arch_el = iel->second;
		    beg = vmax(arch_el->mLastGet,arch_el->archive().begin());
		    end = arch_el->archive().end();
		    if( !beg || !end || beg > end )	continue;
		    arch_el->setVals( arch_el->archive(), beg, end );
		}
	    res.release();

	    arch.tm_calc = 1e-3*(TSYS::curTime()-t_cnt);

	    if( isLast ) break;

	    TSYS::taskSleep( (long long)(1e9*arch.archPeriod()) );
	} catch(TError err) { mess_err(err.cat.c_str(),"%s",err.mess.c_str() ); }

    arch.run_st = false;
}

void TVArchivator::cntrCmdProc( XMLNode *opt )
{
    //> Get page info
    if( opt->name() == "info" )
    {
	TCntrNode::cntrCmdProc(opt);
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
		ctrMkNode("fld",opt,-1,"/prm/cfg/nm",cfg("NAME").fld().descr(),0664,"root","Archive",2,"tp","str","len","50");
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

    //> Process command to page
    string a_path = opt->attr("path");
    if( a_path == "/prm/st/st" )
    {
	if( ctrChkNode(opt,"get",0664,"root","Archive",SEC_RD) )	opt->setText( startStat() ? "1" : "0" );
	if( ctrChkNode(opt,"set",0664,"root","Archive",SEC_WR) )	atoi(opt->text().c_str()) ? start() : stop();
    }
    else if( a_path == "/prm/st/tarch" && ctrChkNode(opt) )		opt->setText(TSYS::real2str(tm_calc,6));
    else if( a_path == "/prm/st/db" )
    {
	if( ctrChkNode(opt,"get",0664,"root","Archive",SEC_RD) )	opt->setText( DB() );
	if( ctrChkNode(opt,"set",0664,"root","Archive",SEC_WR) )	setDB( opt->text() );
    }
    else if( a_path == "/prm/cfg/id" && ctrChkNode(opt) )		opt->setText(id());
    else if( a_path == "/prm/cfg/nm" )
    {
	if( ctrChkNode(opt,"get",0664,"root","Archive",SEC_RD) )	opt->setText( name() );
	if( ctrChkNode(opt,"set",0664,"root","Archive",SEC_WR) )	setName( opt->text() );
    }
    else if( a_path == "/prm/cfg/dscr" )
    {
	if( ctrChkNode(opt,"get",0664,"root","Archive",SEC_RD) )	opt->setText( dscr() );
	if( ctrChkNode(opt,"set",0664,"root","Archive",SEC_WR) )	setDscr( opt->text() );
    }
    else if( a_path == "/prm/cfg/addr" )
    {
	if( ctrChkNode(opt,"get",0664,"root","Archive",SEC_RD) )	opt->setText( addr() );
	if( ctrChkNode(opt,"set",0664,"root","Archive",SEC_WR) )	setAddr( opt->text() );
    }
    else if( a_path == "/prm/cfg/start" )
    {
	if( ctrChkNode(opt,"get",0664,"root","Archive",SEC_RD) )	opt->setText( toStart() ? "1" : "0" );
	if( ctrChkNode(opt,"set",0664,"root","Archive",SEC_WR) )	setToStart( atoi(opt->text().c_str()) );
    }
    else if( a_path == "/prm/cfg/vper" )
    {
	if( ctrChkNode(opt,"get",0664,"root","Archive",SEC_RD) )	opt->setText(TSYS::real2str(valPeriod(),6));
	if( ctrChkNode(opt,"set",0664,"root","Archive",SEC_WR) )	setValPeriod(atof(opt->text().c_str()));
    }
    else if( a_path == "/prm/cfg/aper" )
    {
	if( ctrChkNode(opt,"get",0664,"root","Archive",SEC_RD) )	opt->setText(TSYS::int2str(archPeriod()));
	if( ctrChkNode(opt,"set",0664,"root","Archive",SEC_WR) )	setArchPeriod(atoi(opt->text().c_str()));
    }
    else if( a_path == "/arch/arch" && ctrChkNode(opt) )
    {
	//-- Fill Archives table --
	XMLNode *n_arch = ctrMkNode("list",opt,-1,"/arch/arch/0","");
	XMLNode *n_per  = ctrMkNode("list",opt,-1,"/arch/arch/1","");
	XMLNode *n_size = ctrMkNode("list",opt,-1,"/arch/arch/2","");

	ResAlloc res(a_res,false);
	for( map<string,TVArchEl*>::iterator iel = archEl.begin(); iel != archEl.end(); ++iel )
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
    mArchive(iarchive), mArchivator(iarchivator), mLastGet(0), prev_tm(0)
{

}

TVArchEl::~TVArchEl()
{

}

TVArchive &TVArchEl::archive( )		{ return mArchive; }

TVArchivator &TVArchEl::archivator( )	{ return mArchivator; }

TVariant TVArchEl::getVal( long long *tm, bool up_ord, bool onlyLocal )
{
    TVariant vl = getValProc(tm,up_ord);

    if( !onlyLocal && tm && archive().startStat() && vl.getS() == EVAL_STR && SYS->daq().at().rdActive() &&
	(archive().srcMode( ) == TVArchive::ActiveAttr || archive().srcMode( ) == TVArchive::PassiveAttr) )
    {
	long long remTm = 0;
	string lstStat;
	AutoHD<TVal> paVl = SYS->nodeAt(archive().srcData(),0,'.');
	AutoHD<TParamContr> sPrm(dynamic_cast<TParamContr*>(&paVl.at().owner()));
	if( sPrm.at().owner().owner().redntAllow() && sPrm.at().owner().redntMode( ) != TController::Off )
	{
	    XMLNode req("get");
	    req.clear()->setAttr("local","1")->
		setAttr("path",TSYS::sepstr2path(archive().srcData())+"/%2fserv%2fval")->
		setAttr("tm",TSYS::ll2str(*tm))->
		setAttr("arch",archivator().workId());
	    reqCall:
	    lstStat = SYS->daq().at().rdStRequest(sPrm.at().owner().workId(),req,lstStat,false);
	    if( !lstStat.empty() && !atoi(req.attr("rez").c_str()) )
	    {
		remTm = atoll(req.attr("tm").c_str());
		vl.setS(req.text());
		if( vl.getS() == EVAL_STR ) goto reqCall;
		*tm = remTm;
		//> Put to local archive
		TValBuf buf(archive().valType( ),0,0,false,true);
		buf.setS(vl.getS(),remTm);
		setVals(buf);
	    }else if( !lstStat.empty() ) goto reqCall;
	}
    }

    return vl;
}

TVariant TVArchEl::getValProc( long long *tm, bool up_ord )
{
    if(tm) *tm = 0;
    return TVariant();
}

void TVArchEl::getVals( TValBuf &buf, long long ibeg, long long iend, bool onlyLocal )
{
    //> Get local archive data
    unsigned int ecnt = buf.evalCnt( );
    getValsProc(buf,ibeg,iend);

    //> Check for holes fill
    //>> Check for redundant allow
    if( !onlyLocal && archive().startStat() && buf.evalCnt( ) > ecnt && SYS->daq().at().rdActive() &&
	(archive().srcMode( ) == TVArchive::ActiveAttr || archive().srcMode( ) == TVArchive::PassiveAttr) )
    {
	AutoHD<TVal> paVl = SYS->nodeAt(archive().srcData(),0,'.');
	AutoHD<TParamContr> sPrm(dynamic_cast<TParamContr*>(&paVl.at().owner()));
	if( sPrm.at().owner().owner().redntAllow() && sPrm.at().owner().redntMode( ) != TController::Off )
	{
	    //> Holes process
	    long long firstEval = 0, curEval;
	    long long cbeg = buf.begin(), lastHole = 0;
	    iend = buf.end();
	    string lstStat;
	    XMLNode req("get");
	    do
	    {
		string curVal = buf.getS(&cbeg,true);
		if( curVal == EVAL_STR )
		{
		    curEval = cbeg;
		    if( !firstEval ) firstEval = curEval;
		}
		cbeg++;
		//> Request to remote station for data
		if( (curVal != EVAL_STR || cbeg > iend) && firstEval )
		{
		    reqCall:
		    req.clear()->setAttr("local","1")->
			setAttr("path",TSYS::sepstr2path(archive().srcData())+"/%2fserv%2fval")->
			setAttr("tm_grnd",TSYS::ll2str(firstEval))->
			setAttr("tm",TSYS::ll2str(curEval))->
			setAttr("arch",archivator().workId())->
			setAttr("mode","1");

		    lstStat = SYS->daq().at().rdStRequest(sPrm.at().owner().workId(),req,lstStat,false);
		    bool evalOk = false, noEvalOk = false;
		    if( !lstStat.empty() && !atoi(req.attr("rez").c_str()) && atoll(req.attr("tm_grnd").c_str()) && atoll(req.attr("tm").c_str()) )
		    {
			//> Get values and put to buffer
			long long bbeg = atoll(req.attr("tm_grnd").c_str());
			long long bend = atoll(req.attr("tm").c_str());
			long long bper = atoll(req.attr("per").c_str());
			int val_tp = atoi(req.attr("vtp").c_str());
			int prevPos = 0, curPos;
			string prevVal = EVAL_STR, curVal;

			for( int v_off = 0; true; )
			{
			    string svl = TSYS::strSepParse(req.text(),0,'\n',&v_off);
			    if( svl.size() )
			    {
				int sepPos = svl.find(" ");
				if( sepPos == string::npos ) break;
				curPos = atoi(svl.substr(0,sepPos).c_str());
				curVal = svl.substr(sepPos+1);
			    }
			    else curPos = ((bend-bbeg)/bper)+1;
			    switch(val_tp)
			    {
				case TFld::Boolean:
				{
				    char vl = atoi(prevVal.c_str());
				    if( vl == EVAL_BOOL ) { evalOk = true; prevPos = curPos; }
				    else { noEvalOk = true; for( ; prevPos < curPos; prevPos++ ) buf.setB(vl,bbeg+prevPos*bper); }
				    break;
				}
				case TFld::Integer:
				{
				    int vl = atoi(prevVal.c_str());
				    if( vl == EVAL_INT ) { evalOk = true; prevPos = curPos; }
				    else { noEvalOk = true; for( ; prevPos < curPos; prevPos++ ) buf.setI(vl,bbeg+prevPos*bper); }
				    break;
				}
				case TFld::Real:
				{
				    double vl = atof(prevVal.c_str());
				    if( vl == EVAL_REAL ) { evalOk = true; prevPos = curPos; }
				    else { noEvalOk = true; for( ; prevPos < curPos; prevPos++ ) buf.setR(vl,bbeg+prevPos*bper); }
				    break;
				}
				case TFld::String:
				{
				    if( prevVal == EVAL_STR ) { evalOk = true; prevPos = curPos; }
				    else { noEvalOk = true; for( ; prevPos < curPos; prevPos++ ) buf.setS(prevVal,bbeg+prevPos*bper); }
				    break;
				}
			    }
			    prevVal = curVal;
			    if( prevPos > (bend-bbeg)/bper ) break;
			}

			//> Put buffer part to archive
			if( noEvalOk )	setVals(buf,firstEval,curEval);
		    }
		    if( !lstStat.empty() && evalOk ) goto reqCall;
		    lstStat = "";
		    firstEval = 0;
		}
	    }while( cbeg <= iend );
	}
    }
}

void TVArchEl::setVals( TValBuf &ibuf, long long beg, long long end )
{
    long long a_per = (long long)(1000000.*archivator().valPeriod());

    if( !beg || !end ) { beg = ibuf.begin(); end = ibuf.end(); }
    beg = vmax(beg,ibuf.begin()); end = vmin(end,ibuf.end());

    if( !beg || !end || beg > end ) return;

    //>> Check for put to buffer
    if( &archive() != &ibuf && mLastGet && end > mLastGet && ((end-mLastGet)/archive().period()) < archive().size() )
    { archive().TValBuf::setVals(ibuf,vmax(archive().end(),beg),end); return; }
    //>> Put direct to archive
    long long wPrevTm = 0;
    string wPrevVal;

    if( &archive() == &ibuf || end > archive().end() ) { wPrevTm = prev_tm; wPrevVal = prev_val; }

    //>> Averaging
    if( a_per > ibuf.period() )
    {
	TValBuf obuf(ibuf.valType(),0,a_per,true,true);
	for( long long c_tm = beg; c_tm <= end;)
	{
	    switch(ibuf.valType())
	    {
		case TFld::Boolean:
		{
		    char c_val = ibuf.getB(&c_tm,true);
		    obuf.setB(c_val,c_tm);
		    c_tm += a_per;
		    break;
		}
		case TFld::String:
		{
		    string c_val = ibuf.getS(&c_tm,true);
		    obuf.setS(c_val,c_tm);
		    c_tm += a_per;
		    break;
		}
		case TFld::Integer:
		{
		    int c_val = ibuf.getI(&c_tm,true);
		    int vdif = c_tm/a_per - wPrevTm/a_per;
		    if( !vdif )
		    {
			int v_o = *(int*)wPrevVal.c_str();
			if( c_val == EVAL_INT ) c_val = v_o;
			if( c_val != EVAL_INT && v_o != EVAL_INT )
			{
			    long long s_k = c_tm-a_per*(c_tm/a_per);
			    long long n_k = ibuf.period();
			    c_val = ((long long)v_o*s_k+(long long)c_val*n_k)/(s_k+n_k);
			}
			wPrevVal.assign((char*)&c_val,sizeof(int));
			wPrevTm = c_tm;
		    }
		    if( vdif == 1 || c_tm+1 > end ) obuf.setI(*(int*)wPrevVal.c_str(),wPrevTm);
		    if( vdif )
		    {
			wPrevVal.assign((char*)&c_val,sizeof(int));
			wPrevTm = c_tm;
		    }
		    c_tm++;
		    break;
		}
		case TFld::Real:
		{
		    double c_val = ibuf.getR(&c_tm,true);
		    int vdif = c_tm/a_per - wPrevTm/a_per;
		    if( !vdif )
		    {
			double v_o = *(double*)wPrevVal.c_str();
			if( c_val == EVAL_REAL ) c_val = v_o;
			if( c_val != EVAL_REAL && v_o != EVAL_REAL )
			{
			    long long s_k = c_tm-a_per*(c_tm/a_per);
			    long long n_k = ibuf.period();
			    c_val = (v_o*s_k+c_val*n_k)/(s_k+n_k);
			}
			wPrevVal.assign((char*)&c_val,sizeof(double));
			wPrevTm = c_tm;
		    }
		    if( vdif == 1 || c_tm+1 > end ) obuf.setR(*(double*)wPrevVal.c_str(),wPrevTm);
		    if( vdif )
		    {
			wPrevVal.assign((char*)&c_val,sizeof(double));
			wPrevTm = c_tm;
		    }
		    c_tm++;
		    break;
		}
	    }
	}
	setValsProc( obuf, obuf.begin(), end );
    }
    else setValsProc( ibuf, beg, end );

    if( end > mLastGet ) mLastGet = end+1;
    if( &archive() == &ibuf || end > archive().end() ) { prev_tm = wPrevTm; prev_val = wPrevVal; }
}
