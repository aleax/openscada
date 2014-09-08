
//OpenSCADA system file: tarchval.cpp
/***************************************************************************
 *   Copyright (C) 2006-2014 by Roman Savochenko, <rom_as@oscada.org>      *
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
#include <algorithm>

#include "tsys.h"
#include "tvalue.h"
#include "tarchives.h"
#include "tarchval.h"

#if HAVE_GD_CORE
#include <gd.h>
#endif

#define HalfDivMinWin	5

using namespace OSCADA;

//*************************************************
//* Value archivator                              *
//*************************************************

//*************************************************
//* TValBuf                                       *
//*************************************************
TValBuf::TValBuf( ) : mValTp(TFld::Integer), mHgResTm(false), mHrdGrd(false), mEnd(0), mBeg(0), mPer(0), mSize(100)
{
    buf.bl = NULL;

    makeBuf(mValTp, mSize, mPer, mHrdGrd, mHgResTm);
}

TValBuf::TValBuf( TFld::Type vtp, int isz, int64_t ipr, bool ihgrd, bool ihres ) :
    mValTp(vtp), mHgResTm(ihres), mHrdGrd(ihgrd), mEnd(0), mBeg(0), mPer(vmax(0,ipr)), mSize(isz)
{
    buf.bl = NULL;

    makeBuf(mValTp, mSize, mPer, mHrdGrd, mHgResTm);
}

TValBuf::~TValBuf( )
{
    switch(mValTp)
    {
	case TFld::Boolean:	delete buf.bl;	break;
	case TFld::Int16:	delete buf.i16;	break;
	case TFld::Int32:	delete buf.i32;	break;
	case TFld::Int64:	delete buf.i64;	break;
	case TFld::Float:	delete buf.rFlt;break;
	case TFld::Double:	delete buf.rDbl;break;
	case TFld::String:	delete buf.str;	break;
	default: break;
    }
}

TValBuf &TValBuf::operator=( TValBuf &src )
{
    makeBuf(src.mValTp, src.mSize, src.mPer, src.mHrdGrd, src.mHgResTm);

    switch(mValTp)
    {
	case TFld::Boolean:
	    buf.bl->cur = src.buf.bl->cur;
	    if(mHrdGrd)		*(buf.bl->buf.grid) = *(src.buf.bl->buf.grid);
	    else if(mHgResTm)	*(buf.bl->buf.tm_high) = *(src.buf.bl->buf.tm_high);
	    else		*(buf.bl->buf.tm_low) = *(src.buf.bl->buf.tm_low);
	    break;
	case TFld::Int16:
	    buf.i16->cur = src.buf.i16->cur;
	    if(mHrdGrd)		*(buf.i16->buf.grid) = *(src.buf.i16->buf.grid);
	    else if(mHgResTm)	*(buf.i16->buf.tm_high) = *(src.buf.i16->buf.tm_high);
	    else		*(buf.i16->buf.tm_low) = *(src.buf.i16->buf.tm_low);
	    break;
	case TFld::Int32:
	    buf.i32->cur = src.buf.i32->cur;
	    if(mHrdGrd)		*(buf.i32->buf.grid) = *(src.buf.i32->buf.grid);
	    else if(mHgResTm)	*(buf.i32->buf.tm_high) = *(src.buf.i32->buf.tm_high);
	    else		*(buf.i32->buf.tm_low) = *(src.buf.i32->buf.tm_low);
	    break;
	case TFld::Int64:
	    buf.i64->cur = src.buf.i64->cur;
	    if(mHrdGrd)		*(buf.i64->buf.grid) = *(src.buf.i64->buf.grid);
	    else if(mHgResTm)	*(buf.i64->buf.tm_high) = *(src.buf.i64->buf.tm_high);
	    else		*(buf.i64->buf.tm_low) = *(src.buf.i64->buf.tm_low);
	    break;
	case TFld::Float:
	    buf.rFlt->cur = src.buf.rFlt->cur;
	    if(mHrdGrd)		*(buf.rFlt->buf.grid) = *(src.buf.rFlt->buf.grid);
	    else if(mHgResTm)	*(buf.rFlt->buf.tm_high) = *(src.buf.rFlt->buf.tm_high);
	    else		*(buf.rFlt->buf.tm_low) = *(src.buf.rFlt->buf.tm_low);
	    break;
	case TFld::Double:
	    buf.rDbl->cur = src.buf.rDbl->cur;
	    if(mHrdGrd)		*(buf.rDbl->buf.grid) = *(src.buf.rDbl->buf.grid);
	    else if(mHgResTm)	*(buf.rDbl->buf.tm_high) = *(src.buf.rDbl->buf.tm_high);
	    else		*(buf.rDbl->buf.tm_low) = *(src.buf.rDbl->buf.tm_low);
	    break;
	case TFld::String:
	    buf.str->cur = src.buf.str->cur;
	    if(mHrdGrd)		*(buf.str->buf.grid) = *(src.buf.str->buf.grid);
	    else if(mHgResTm)	*(buf.str->buf.tm_high) = *(src.buf.str->buf.tm_high);
	    else		*(buf.str->buf.tm_low) = *(src.buf.str->buf.tm_low);
	    break;
	default: break;
    }
    mEvalCnt = src.mEvalCnt;

    return *this;
}

void TValBuf::clear( )
{
    switch(mValTp)
    {
	case TFld::Boolean:	buf.bl->clear();	break;
	case TFld::Int16:	buf.i16->clear();	break;
	case TFld::Int32:	buf.i32->clear();	break;
	case TFld::Int64:	buf.i64->clear();	break;
	case TFld::Float:	buf.rFlt->clear();	break;
	case TFld::Double:	buf.rDbl->clear();	break;
	case TFld::String:	buf.str->clear(); 	break;
	default: break;
    }
}

int TValBuf::realSize( )
{
    switch(mValTp)
    {
	case TFld::Boolean:	return buf.bl->realSize();
	case TFld::Int16:	return buf.i16->realSize();
	case TFld::Int32:	return buf.i32->realSize();
	case TFld::Int64:	return buf.i64->realSize();
	case TFld::Float:	return buf.rFlt->realSize();
	case TFld::Double:	return buf.rDbl->realSize();
	case TFld::String:	return buf.str->realSize();
	default: break;
    }

    return 0;
}

bool TValBuf::vOK( int64_t ibeg, int64_t iend )
{
    if(!period()) return !(!begin() || !end() || iend < begin() || ibeg > end() || ibeg > iend);

    return !(!begin() || !end() || iend/period() < begin()/period() || ibeg/period() > end()/period() || ibeg/period() > iend/period());
}

void TValBuf::setValType( TFld::Type vl )	{ makeBuf(vl, mSize, mPer, mHrdGrd, mHgResTm); }

void TValBuf::setHardGrid( bool vl )		{ makeBuf(mValTp, mSize, mPer, vl, mHgResTm); }

void TValBuf::setHighResTm( bool vl )		{ makeBuf(mValTp, mSize, mPer, mHrdGrd, vl); }

void TValBuf::setSize( int vl )			{ makeBuf(mValTp, vl, mPer, mHrdGrd, mHgResTm); }

void TValBuf::setPeriod( int64_t vl )		{ makeBuf(mValTp, mSize, vl, mHrdGrd, mHgResTm); }

void TValBuf::makeBuf( TFld::Type v_tp, int isz, int64_t ipr, bool hd_grd, bool hg_res )
{
    ResAlloc res(bRes, true);

    //Destroy buffer
    if(v_tp != mValTp && buf.bl) {
	switch(mValTp)
	{
	    case TFld::Boolean:	delete buf.bl;	break;
	    case TFld::Int16:	delete buf.i16;	break;
	    case TFld::Int32:	delete buf.i32;	break;
	    case TFld::Int64:	delete buf.i64;	break;
	    case TFld::Float:	delete buf.rFlt;break;
	    case TFld::Double:	delete buf.rDbl;break;
	    case TFld::String:	delete buf.str;	break;
	    default: break;
	}
	buf.bl = NULL;
    }

    if(!buf.bl) {
	//Make new buffer
	switch(v_tp)
	{
	    case TFld::Boolean:	buf.bl = new TBuf<char>(EVAL_BOOL, mSize, mPer, mHrdGrd, mHgResTm, mEnd, mBeg, mEvalCnt);	break;
	    case TFld::Int16:	buf.i16 = new TBuf<int16_t>(EVAL_INT16, mSize, mPer, mHrdGrd, mHgResTm, mEnd, mBeg, mEvalCnt);	break;
	    case TFld::Int32:	buf.i32 = new TBuf<int32_t>(EVAL_INT32, mSize, mPer, mHrdGrd, mHgResTm, mEnd, mBeg, mEvalCnt);	break;
	    case TFld::Int64:	buf.i64 = new TBuf<int64_t>(EVAL_INT64, mSize, mPer, mHrdGrd, mHgResTm, mEnd, mBeg, mEvalCnt);	break;
	    case TFld::Float:	buf.rFlt = new TBuf<float>(EVAL_RFlt, mSize, mPer, mHrdGrd, mHgResTm, mEnd, mBeg, mEvalCnt);	break;
	    case TFld::Double:	buf.rDbl = new TBuf<double>(EVAL_RDbl, mSize, mPer, mHrdGrd, mHgResTm, mEnd, mBeg, mEvalCnt);	break;
	    case TFld::String:	buf.str = new TBuf<string>(EVAL_STR, mSize, mPer, mHrdGrd, mHgResTm, mEnd, mBeg, mEvalCnt);	break;
	    default: break;
	}
	mValTp = v_tp;
    }
    if(isz != mSize || ipr != mPer || hd_grd != mHrdGrd || hg_res != mHgResTm)
	switch(mValTp)
	{
	    case TFld::Boolean:	buf.bl->makeBuf(isz, ipr, hd_grd, hg_res);	break;
	    case TFld::Int16:	buf.i16->makeBuf(isz, ipr, hd_grd, hg_res);	break;
	    case TFld::Int32:	buf.i32->makeBuf(isz, ipr, hd_grd, hg_res);	break;
	    case TFld::Int64:	buf.i64->makeBuf(isz, ipr, hd_grd, hg_res);	break;
	    case TFld::Float:	buf.rFlt->makeBuf(isz, ipr, hd_grd, hg_res);	break;
	    case TFld::Double:	buf.rDbl->makeBuf(isz, ipr, hd_grd, hg_res);	break;
	    case TFld::String:	buf.str->makeBuf(isz, ipr, hd_grd, hg_res);	break;
	    default: break;
	}
}

TVariant TValBuf::get( int64_t *itm, bool up_ord )
{
    switch(valType())
    {
	case TFld::Boolean:	return getB(itm, up_ord);
	case TFld::Integer:	return getI(itm, up_ord);
	case TFld::Real:	return getR(itm, up_ord);
	case TFld::String:	return getS(itm, up_ord);
	default: break;
    }
    if(itm) *itm = 0;
    return TVariant();
}

string TValBuf::getS( int64_t *itm, bool up_ord )
{
    switch(valType())
    {
	case TFld::Boolean: { char vl = getB(itm, up_ord); return (vl==EVAL_BOOL) ? EVAL_STR : i2s((bool)vl); }
	case TFld::Integer: { int64_t vl = getI(itm, up_ord); return (vl==EVAL_INT) ? EVAL_STR : i2s(vl); }
	case TFld::Real:    { double vl = getR(itm, up_ord); return (vl==EVAL_REAL) ? EVAL_STR : r2s(vl); }
	case TFld::String:  { ResAlloc res(bRes, false); return buf.str->get(itm, up_ord); }
	default: break;
    }
    return EVAL_STR;
}

double TValBuf::getR( int64_t *itm, bool up_ord )
{
    switch(valType(true))
    {
	case TFld::Boolean: { char vl = getB(itm, up_ord); return (vl==EVAL_BOOL) ? EVAL_REAL : (bool)vl; }
	case TFld::Int16: case TFld::Int32: case TFld::Int64:
			    { int64_t vl = getI(itm, up_ord); return (vl==EVAL_INT) ? EVAL_REAL : (double)vl; }
	case TFld::String:  { string vl = getS(itm, up_ord); return (vl==EVAL_STR) ? EVAL_REAL : s2r(vl); }
	case TFld::Float:   { ResAlloc res(bRes, false); float vl = buf.rFlt->get(itm, up_ord); return (vl==EVAL_RFlt) ? EVAL_REAL : vl; }
	case TFld::Double:  { ResAlloc res(bRes, false); double vl = buf.rDbl->get(itm, up_ord); return (vl==EVAL_RDbl) ? EVAL_REAL : vl; }
	default: break;
    }
    return EVAL_REAL;
}

int64_t TValBuf::getI( int64_t *itm, bool up_ord )
{
    switch(valType(true))
    {
	case TFld::Boolean: { char vl = getB(itm, up_ord); return (vl==EVAL_BOOL) ? EVAL_INT : (bool)vl; }
	case TFld::String:  { string vl = getS(itm, up_ord); return (vl==EVAL_STR) ? EVAL_INT : s2ll(vl); }
	case TFld::Float: case TFld::Double:
			    { double vl = getR(itm, up_ord); return (vl==EVAL_REAL) ? EVAL_INT : (int64_t)vl; }
	case TFld::Int16:   { ResAlloc res(bRes, false); int16_t vl = buf.i16->get(itm, up_ord); return (vl==EVAL_INT16) ? EVAL_INT : vl; }
	case TFld::Int32:   { ResAlloc res(bRes, false); int32_t vl = buf.i32->get(itm, up_ord); return (vl==EVAL_INT32) ? EVAL_INT : vl; }
	case TFld::Int64:   { ResAlloc res(bRes, false); int64_t vl = buf.i64->get(itm, up_ord); return (vl==EVAL_INT64) ? EVAL_INT : vl; }
	default: break;
    }
    return EVAL_INT;
}

char TValBuf::getB( int64_t *itm, bool up_ord )
{
    switch(valType())
    {
	case TFld::Integer: { int64_t vl = getI(itm, up_ord); return (vl==EVAL_INT) ? EVAL_BOOL : (bool)vl; }
	case TFld::String:  { string vl = getS(itm, up_ord); return (vl==EVAL_STR) ? EVAL_BOOL : (bool)s2i(vl); }
	case TFld::Real:    { double vl = getR(itm, up_ord); return (vl==EVAL_REAL) ? EVAL_BOOL : (bool)vl; }
	case TFld::Boolean: { ResAlloc res(bRes, false); return buf.bl->get(itm, up_ord); }
	default: break;
    }
    return EVAL_BOOL;
}

void TValBuf::setS( const string &value, int64_t tm )
{
    switch(valType())
    {
	case TFld::Boolean: setB((value==EVAL_STR)?EVAL_BOOL:(bool)s2i(value), tm);	break;
	case TFld::Integer: setI((value==EVAL_STR)?EVAL_INT:s2ll(value), tm);		break;
	case TFld::Real:    setR((value==EVAL_STR)?EVAL_REAL:s2r(value), tm);		break;
	case TFld::String:  { ResAlloc res(bRes, true); buf.str->set(value, tm); }	break;
	default: break;
    }
}

void TValBuf::setR( double value, int64_t tm )
{
    switch(valType(true))
    {
	case TFld::Boolean: setB((value==EVAL_REAL)?EVAL_BOOL:(bool)value, tm);		break;
	case TFld::Int16: case TFld::Int32: case TFld::Int64:
			    setI((value==EVAL_REAL)?EVAL_INT:(int64_t)value, tm);	break;
	case TFld::String:  setS((value==EVAL_REAL)?EVAL_STR:r2s(value), tm);break;
	case TFld::Float:   { ResAlloc res(bRes, true); buf.rFlt->set((value==EVAL_REAL)?EVAL_RFlt:value, tm); }	break;
	case TFld::Double:  { ResAlloc res(bRes, true); buf.rDbl->set((value==EVAL_REAL)?EVAL_RDbl:value, tm); }	break;
	default: break;
    }
}

void TValBuf::setI( int64_t value, int64_t tm )
{
    switch(valType(true))
    {
	case TFld::Boolean: setB((value==EVAL_INT)?EVAL_BOOL:(bool)value, tm);	break;
	case TFld::String:  setS((value==EVAL_INT)?EVAL_STR:ll2s(value), tm);	break;
	case TFld::Float: case TFld::Double:
			    setR((value==EVAL_INT)?EVAL_REAL:value, tm);	break;
	case TFld::Int16: { ResAlloc res(bRes, true); buf.i16->set((value==EVAL_INT)?EVAL_INT16:value, tm); } break;
	case TFld::Int32: { ResAlloc res(bRes, true); buf.i32->set((value==EVAL_INT)?EVAL_INT32:value, tm); } break;
	case TFld::Int64: { ResAlloc res(bRes, true); buf.i64->set((value==EVAL_INT)?EVAL_INT64:value, tm); } break;
	default: break;
    }
}

void TValBuf::setB( char value, int64_t tm )
{
    switch(valType())
    {
	case TFld::Integer: setI((value==EVAL_BOOL)?EVAL_INT:(bool)value, tm);		break;
	case TFld::String:  setS((value==EVAL_BOOL)?EVAL_STR:i2s((bool)value), tm);	break;
	case TFld::Real:    setR((value==EVAL_BOOL)?EVAL_REAL:(bool)value, tm);		break;
	case TFld::Boolean: { ResAlloc res(bRes, true); buf.bl->set(value, tm); }	break;
	default: break;
    }
}

void TValBuf::set( const TVariant &value, int64_t tm )
{
    switch(value.type())
    {
	case TVariant::Boolean:	setB(value.getB(), tm);	break;
	case TVariant::Integer:	setI(value.getI(), tm);	break;
	case TVariant::Real:	setR(value.getR(), tm);	break;
	case TVariant::String:	setS(value.getS(), tm);	break;
	default: break;
    }
}

void TValBuf::getVals( TValBuf &buf, int64_t ibeg, int64_t iend )
{
    if(!vOK(ibeg,iend))	return;
    ibeg = vmax(ibeg, begin());
    iend = vmin(iend, end());

    int64_t t_step = 1;
    if(buf.period() && period() && buf.period() > period()) {
	t_step = buf.period();
	ibeg = (ibeg/t_step)*t_step;
    }
    switch(valType())
    {
	case TFld::Boolean: {
	    char vl;
	    for( ; ibeg <= iend; ibeg += t_step) { vl = getB(&ibeg, true); buf.setB(vl, ibeg); }
	    break;
	}
	case TFld::Integer: {
	    int64_t vl;
	    for( ; ibeg <= iend; ibeg += t_step) { vl = getI(&ibeg, true); buf.setI(vl, ibeg); }
	    break;
	}
	case TFld::Real: {
	    double vl;
	    for( ; ibeg <= iend; ibeg += t_step) { vl = getR(&ibeg,true); buf.setR(vl,ibeg); }
	    break;
	}
	case TFld::String: {
	    string vl;
	    for( ; ibeg <= iend; ibeg += t_step) { vl = getS(&ibeg,true); buf.setS(vl,ibeg); }
	    break;
	}
	default: break;
    }
}

void TValBuf::setVals( TValBuf &buf, int64_t ibeg, int64_t iend )	{ buf.getVals(*this, ibeg, iend); }

//*************************************************
//* TValBuf::TBuf                                 *
//*************************************************
template <class TpVal> TValBuf::TBuf<TpVal>::TBuf( TpVal ieval, int &isz, int64_t &ipr,
	bool &ihgrd, bool &ihres, int64_t &iend, int64_t &ibeg, unsigned int &iEvalCnt ) :
    hg_res_tm(ihres), hrd_grd(ihgrd), end(iend), beg(ibeg), per(ipr), size(isz), cur(0), eval(ieval), mEvalCnt(iEvalCnt)
{
    buf.grid = NULL;

    makeBuf(size, per, hrd_grd, hg_res_tm);
}

template <class TpVal> TValBuf::TBuf<TpVal>::~TBuf( )
{
    if(hrd_grd)		delete buf.grid;
    else if(hg_res_tm)	delete buf.tm_high;
    else		delete buf.tm_low;
}

template <class TpVal> void TValBuf::TBuf<TpVal>::clear( )
{
    if(!buf.grid)	return;
    if(hrd_grd)		buf.grid->clear();
    else if(hg_res_tm)	buf.tm_high->clear();
    else		buf.tm_low->clear();
    cur = end = beg = 0;
    mEvalCnt = 0;
}

template <class TpVal> int TValBuf::TBuf<TpVal>::realSize( )
{
    if(!buf.grid)	return 0;
    if(hrd_grd)		return buf.grid->size();
    else if(hg_res_tm)	return buf.tm_high->size();
    else		return buf.tm_low->size();
}

template <class TpVal> void TValBuf::TBuf<TpVal>::makeBuf( int isz, int64_t ipr, bool ihd_grd, bool ihg_res )
{
    bool recr_buf = false;
    if(!buf.grid)			recr_buf = true;
    if(isz < size || (!size && isz))	recr_buf = true;
    if(hrd_grd != ihd_grd) {
	if(ihd_grd && !ipr)	ipr = 1000000;
	recr_buf = true;
    }
    if(hg_res_tm != ihg_res)	recr_buf = true;
    if(per != ipr) {
	if(hrd_grd && !ipr)	ipr = 1000000;
	recr_buf = true;
    }

    size = isz;
    hrd_grd = ihd_grd;
    hg_res_tm = ihg_res;
    per = ipr;

    if(recr_buf) {
	//Destroy buffer
	cur = end = beg = 0;
	if(buf.grid) {
	    if(hrd_grd)		delete buf.grid;
	    else if(hg_res_tm)	delete buf.tm_high;
	    else		delete buf.tm_low;
	}
	buf.grid = NULL;

	//Make new buffer
	if(ihd_grd)	buf.grid = new vector<TpVal>;
	else if(ihg_res)buf.tm_high = new vector<SHg>;
	else		buf.tm_low = new vector<SLw>;
	mEvalCnt = 0;
    }
}

template <class TpVal> TpVal TValBuf::TBuf<TpVal>::get( int64_t *itm, bool up_ord )
{
    int64_t tm = (itm)?(*itm):TSYS::curTime();

    if((up_ord && tm > end) || (!up_ord && tm < beg))	throw TError("ValBuf",_("Value is not present."));

    tm = up_ord ? vmax(tm, beg) : vmin(tm, end);
    //Process hard grid buffer
    if(hrd_grd) {
	int npos = up_ord ? (end-tm)/per : (int64_t)buf.grid->size()-1-(tm-beg)/per;
	if(npos < 0 || npos >= (int)buf.grid->size()) { if(itm) *itm = 0; return eval; }
	if(itm)	*itm = end-npos*per;
	return (*buf.grid)[((cur-npos-1)>=0)?(cur-npos-1):(buf.grid->size()+(cur-npos-1))];
    }
    //Process soft grid buffer
    else if(per) {
	int npos = (up_ord?(end-tm):(tm-beg))/per;
	if(hg_res_tm) {
	    int c_cur = cur ? cur-1 : buf.tm_high->size()-1;
	    int c_end = c_cur;

	    // Half divider
	    for(int d_win = buf.tm_high->size()/2; d_win > HalfDivMinWin; d_win /= 2)
	    {
		int c_cnext = c_end-d_win;
		if(c_cnext < 0) c_cnext += buf.tm_high->size();
		if(tm/per < (*buf.tm_high)[c_cnext].tm/per) c_end = c_cnext;
	    }
	    // Proving
	    do {
		int w_pos = up_ord ? (end/per-(*buf.tm_high)[c_end].tm/per) :
				     ((*buf.tm_high)[c_end].tm/per-beg/per);
		if(up_ord && w_pos >= npos) {
		    if(itm) *itm = (w_pos==npos) ? (*buf.tm_high)[c_end].tm : end-npos*per;
		    return (*buf.tm_high)[c_end].val;
		}
		else if(!up_ord && w_pos <= npos) {
		    if(itm) *itm = (w_pos==npos) ? (*buf.tm_high)[c_end].tm : beg+npos*per;
		    return (*buf.tm_high)[c_end].val;
		}
		if(--c_end < 0) c_end = buf.tm_high->size()-1;
	    } while(c_end != c_cur);

	    if(itm) *itm = 0;
	    return eval;
	}
	else {
	    int c_cur = cur ? cur-1 : buf.tm_low->size()-1;
	    int c_end = c_cur;

	    // Half divider
	    for(int d_win = buf.tm_low->size()/2; d_win > HalfDivMinWin; d_win /= 2)
	    {
		int c_cnext = c_end-d_win;
		if(c_cnext < 0) c_cnext += buf.tm_low->size();
		if(tm/per < (int64_t)(*buf.tm_low)[c_cnext].tm*1000000/per) c_end=c_cnext;
	    }
	    // Proving
	    do {
		int w_pos = up_ord ? (end/per-(int64_t)(*buf.tm_low)[c_end].tm*1000000/per) :
				     ((int64_t)(*buf.tm_low)[c_end].tm*1000000/per-beg/per);
		if(up_ord && w_pos >= npos) {
		    if(itm) *itm = (w_pos==npos) ? (int64_t)(*buf.tm_low)[c_end].tm*1000000 : end-npos*per;
		    return (*buf.tm_low)[c_end].val;
		}
		if(!up_ord && w_pos <= npos) {
		    if(itm) *itm = (w_pos==npos) ? (int64_t)(*buf.tm_low)[c_end].tm*1000000 : beg+npos*per;
		    return (*buf.tm_low)[c_end].val;
		}
		if(--c_end < 0) c_end = buf.tm_low->size()-1;
	    }while(c_end != c_cur);

	    if(itm) *itm = 0;
	    return eval;
	}
    }
    //Proccess flow buffer
    else {
	if(hg_res_tm) {
	    int c_end = buf.tm_high->size()-1;
	    // Half divider
	    for(int d_win = c_end/2; d_win > HalfDivMinWin; d_win /= 2)
		if(!((!up_ord && tm >= (*buf.tm_high)[c_end-d_win].tm) ||
			(up_ord && tm <= (*buf.tm_high)[buf.tm_high->size()-(c_end-d_win)-1].tm)))
		    c_end -= d_win;
	    // Scan last window
	    while(c_end >= 0) {
		if(!up_ord && tm >= (*buf.tm_high)[c_end].tm) {
		    if(itm) *itm = (*buf.tm_high)[c_end].tm;
		    return (*buf.tm_high)[c_end].val;
		}
		else if(up_ord && tm <= (*buf.tm_high)[buf.tm_high->size()-c_end-1].tm) {
		    if(itm) *itm = (*buf.tm_high)[buf.tm_high->size()-c_end-1].tm;
		    return (*buf.tm_high)[buf.tm_high->size()-c_end-1].val;
		}
		c_end--;
	    }
	    if(itm) *itm = 0;
	    return eval;
	}
	else {
	    int c_end = buf.tm_low->size()-1;
	    // Half divider
	    for(int d_win = c_end/2; d_win > HalfDivMinWin; d_win /= 2)
		if(!((!up_ord && tm/1000000 >= (*buf.tm_low)[c_end-d_win].tm) ||
			(up_ord && tm <= (int64_t)(*buf.tm_low)[buf.tm_low->size()-(c_end-d_win)-1].tm*1000000)))
		    c_end -= d_win;
	    // Scan last window
	    while(c_end >= 0) {
		if(!up_ord && tm/1000000 >= (*buf.tm_low)[c_end].tm) {
		    if(itm) *itm = (int64_t)(*buf.tm_low)[c_end].tm*1000000;
		    return (*buf.tm_low)[c_end].val;
		}
		else if(up_ord && tm <= (int64_t)(*buf.tm_low)[buf.tm_low->size()-c_end-1].tm*1000000) {
		    if(itm) *itm = (int64_t)(*buf.tm_low)[buf.tm_low->size()-c_end-1].tm*1000000;
		    return (*buf.tm_low)[buf.tm_low->size()-c_end-1].val;
		}
		c_end--;
	    }
	    if(itm) *itm = 0;
	    return eval;
	}
    }
}

template <class TpVal> void TValBuf::TBuf<TpVal>::set( TpVal value, int64_t tm )
{
    if(!tm)  tm = TSYS::curTime();
    if(!end) end = per ? per*(tm/per-1) : tm;
    if(!beg) beg = per ? per*(tm/per) : tm;

    //Process hard grid buffer
    if(hrd_grd) {
	int npos = (tm-end)/per;
	// Set value
	if(npos <= 0 && (-npos) < (int)buf.grid->size()) {
	    int wcur = cur+npos-1;
	    if(wcur < 0) wcur = buf.grid->size()+wcur;
	    if(value == eval) mEvalCnt++;
	    if((*buf.grid)[wcur] == eval) mEvalCnt--;
	    (*buf.grid)[wcur] = value;
	    return;
	}
	else if(npos < 0) throw TError("ValBuf",_("Grid mode doesn't support inserting too old values %lld (%lld-%lld)."),tm,beg,end);
	else
	    while(npos--) {
		TpVal w_val = npos ? eval : value;
		// Set new value
		if(w_val == eval) mEvalCnt++;
		if(cur >= (int)buf.grid->size()) buf.grid->push_back(w_val);
		else {
		    beg += per;
		    if((*buf.grid)[cur] == eval) mEvalCnt--;
		    (*buf.grid)[cur] = w_val;
		}
		// Update cursor
		if((++cur >= size) && size) {
		    cur = 0;
		    //  Memory consumption is made optimal for limited size
		    if((int)buf.grid->capacity() > size) {
			vector<TpVal> *ns = new vector<TpVal>;
			*ns = *buf.grid;
			delete buf.grid;
			buf.grid = ns;
		    }
		}
		// Update end time
		end += per;
	    }
    }
    //Process soft grid buffer
    else if(per) {
	int npos = (tm-end)/per;
	// Set value
	if(npos < 0) throw TError("ValBuf",_("Grid mode doesn't support inserting old values."));
	else {
	    if(hg_res_tm) {
		SHg b_el;

		// Update current end value
		if(npos == 0) {
		    b_el.tm = end; b_el.val = value;
		    int h_el = cur ? cur-1 : buf.tm_high->size()-1;
		    if(((*buf.tm_high)[h_el].tm-end)/per) {
			//  Write new value
			if(b_el.val == eval) mEvalCnt++;
			if(cur >= (int)buf.tm_high->size()) buf.tm_high->push_back(b_el);
			else {
			    if(cur+1 >= (int)buf.tm_high->size()) beg = (*buf.tm_high)[0].tm;
			    else beg = (*buf.tm_high)[cur+1].tm;
			    beg = per*(beg/per);
			    if((*buf.tm_high)[cur].val == eval) mEvalCnt--;
			    (*buf.tm_high)[cur] = b_el;
			}
			//  Update cursor
			if((++cur >= size) && size) {
			    cur = 0;
			    //   Memory consumption is made optimal for limited size
			    if((int)buf.tm_high->capacity() > size) {
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

		// Insert new value
		int c_npos = npos;
		while(c_npos--) {
		    //  Prepare data
		    if(c_npos) 	{ b_el.tm = end+(npos-c_npos)*per; b_el.val = eval; }
		    else	{ b_el.tm = tm; b_el.val = value; }
		    //  Check previous value
		    if(!buf.tm_high->size() ||
			    (cur && (*buf.tm_high)[cur-1].val != b_el.val ) ||
			    (!cur && (*buf.tm_high)[buf.tm_high->size()-1].val != b_el.val))
		    {
			//   Write new value
			if(b_el.val == eval) mEvalCnt++;
			if(cur >= (int)buf.tm_high->size()) buf.tm_high->push_back(b_el);
			else {
			    if(cur+1 >= (int)buf.tm_high->size()) beg = (*buf.tm_high)[0].tm;
			    else beg = (*buf.tm_high)[cur+1].tm;
			    beg = per*(beg/per);
			    if((*buf.tm_high)[cur].val == eval) mEvalCnt--;
			    (*buf.tm_high)[cur] = b_el;
			}
			//   Update cursor
			if((++cur >= size) && size) {
			    cur = 0;
			    //   Memory consumption is made optimal for limited size
			    if((int)buf.tm_high->capacity() > size) {
				vector<SHg> *ns = new vector<SHg>;
				*ns = *buf.tm_high;
				delete buf.tm_high;
				buf.tm_high = ns;
			    }
			}
		    }
		    //  Update end time
		    end += per;
		}
	    }
	    else {
		SLw b_el;

		// Update current end value
		if(npos == 0) {
		    b_el.tm = end/1000000; b_el.val = value;
		    int h_el = cur ? cur-1 : buf.tm_low->size()-1;
		    if(((int64_t)(*buf.tm_low)[h_el].tm*1000000-end)/per) {
			//  Write new value
			if(b_el.val == eval) mEvalCnt++;
			if(cur >= (int)buf.tm_low->size()) buf.tm_low->push_back(b_el);
			else {
			    if(cur+1 >= (int)buf.tm_low->size()) beg = (int64_t)(*buf.tm_low)[0].tm * 1000000;
			    else beg = (int64_t)(*buf.tm_low)[cur+1].tm * 1000000;
			    beg = per*(beg/per);
			    if((*buf.tm_low)[cur].val == eval) mEvalCnt--;
			    (*buf.tm_low)[cur] = b_el;
			}
			//  Update cursor
			if((++cur >= size) && size) {
			    cur = 0;
			    //   Memory consumption is made optimal for limited size
			    if((int)buf.tm_low->capacity() > size) {
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

		// Insert new value
		int c_npos = npos;
		while(c_npos--) {
		    //  Prepare data
		    if(c_npos)	{ b_el.tm = (end+(npos-c_npos)*per)/1000000; b_el.val = eval; }
		    else	{ b_el.tm = tm/1000000; b_el.val = value; }
		    //  Check previous value
		    if(!buf.tm_low->size() ||
			    (cur && (*buf.tm_low)[cur-1].val != b_el.val) ||
			    (!cur && (*buf.tm_low)[buf.tm_low->size()-1].val != b_el.val))
		    {
			//   Write new value
			if(b_el.val == eval) mEvalCnt++;
			if(cur >= (int)buf.tm_low->size()) buf.tm_low->push_back(b_el);
			else {
			    if(cur+1 >= (int)buf.tm_low->size())
				beg = (int64_t)(*buf.tm_low)[0].tm * 1000000;
			    else beg = (int64_t)(*buf.tm_low)[cur+1].tm * 1000000;
			    beg = per*(beg/per);
			    if((*buf.tm_low)[cur].val == eval) mEvalCnt--;
			    (*buf.tm_low)[cur] = b_el;
			}
			//   Update cursor
			if((++cur >= size) && size) {
			    cur = 0;
			    //   Memory consumption is made optimal for limited size
			    if((int)buf.tm_low->capacity() > size) {
				vector<SLw> *ns = new vector<SLw>;
				*ns = *buf.tm_low;
				delete buf.tm_low;
				buf.tm_low = ns;
			    }
			}
		    }
		    //  Update end time
		    end += per;
		}
	    }
	}
    }
    //Proccess flow buffer
    else {
	if(hg_res_tm) {
	    SHg b_el = { tm, value };
	    if(tm < beg && size && (int)buf.tm_high->size() >= size) throw TError("ValBuf",_("Set too old value to buffer."));
	    int c_pos = 0;

	    // Half divider
	    int d_win = buf.tm_high->size()/2;
	    while(d_win > HalfDivMinWin) {
		if(tm > (*buf.tm_high)[c_pos+d_win].tm) c_pos += d_win;
		d_win /= 2;
	    }
	    // Scan last window
	    while(true) {
		if(c_pos >= (int)buf.tm_high->size() || tm < (*buf.tm_high)[c_pos].tm)
		{
		    if(b_el.val == eval) mEvalCnt++;
		    if(c_pos == (int)buf.tm_high->size()) buf.tm_high->push_back(b_el);
		    else buf.tm_high->insert(buf.tm_high->begin()+c_pos,b_el);
		    if(size && (int)buf.tm_high->size() > size) {
			if(buf.tm_high->begin()->val == eval) mEvalCnt--;
			buf.tm_high->erase(buf.tm_high->begin());
		    }
		    beg = (*buf.tm_high)[0].tm;
		    end = (*buf.tm_high)[buf.tm_high->size()-1].tm;
		    return;
		}
		else if(tm == (*buf.tm_high)[c_pos].tm) {
		    if((*buf.tm_high)[c_pos].val == eval) mEvalCnt--;
		    (*buf.tm_high)[c_pos] = b_el;
		    return;
		}
		c_pos++;
	    }
	}
	else {
	    SLw b_el = { tm/1000000, value };
	    if(tm < beg && size && (int)buf.tm_low->size() >= size) throw TError("ValBuf",_("Set too old value to buffer."));
	    int c_pos = 0;
	    // Half divider
	    int d_win = buf.tm_low->size()/2;
	    while(d_win > HalfDivMinWin) {
		if(tm/1000000 > (*buf.tm_low)[c_pos+d_win].tm) c_pos += d_win;
		d_win /= 2;
	    }
	    // Scan last window
	    while(true) {
		if(c_pos >= (int)buf.tm_low->size() || tm/1000000 < (*buf.tm_low)[c_pos].tm)
		{
		    if(b_el.val == eval) mEvalCnt++;
		    if(c_pos == (int)buf.tm_low->size()) buf.tm_low->push_back(b_el);
		    else buf.tm_low->insert(buf.tm_low->begin()+c_pos,b_el);
		    if(size && (int)buf.tm_low->size() > size) {
			if( buf.tm_low->begin()->val == eval ) mEvalCnt--;
			buf.tm_low->erase(buf.tm_low->begin());
		    }
		    beg = (int64_t)(*buf.tm_low)[0].tm*1000000;
		    end = (int64_t)(*buf.tm_low)[buf.tm_low->size()-1].tm*1000000;
		    return;
		}
		else if(tm/1000000 == (*buf.tm_low)[c_pos].tm) {
		    if((*buf.tm_low)[c_pos].val == eval) mEvalCnt--;
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
    TConfig(cf_el), runSt(false), mDB(idb),
    mId(cfg("ID")), mVType(cfg("VTYPE")), mSrcMode(cfg("SrcMode")), mSource(cfg("Source")),
    mCombMode(cfg("CombMode")), mBPer(cfg("BPER")), mBSize(cfg("BSIZE")), mStart(cfg("START").getBd()),
    mBHGrd(cfg("BHGRD").getBd()), mBHRes(cfg("BHRES").getBd())
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
    if(!src_n) return *this;

    //Configuration copy
    exclCopy(*src_n, "ID;SrcMode;Source;");
    mDB = src_n->mDB;

    if(src_n->startStat() && toStart() && !startStat())	start();

    return *this;
}

void TVArchive::preDisable( int flag )
{
    stop(flag);
}

void TVArchive::postDisable( int flag )
{
    if(flag) SYS->db().at().dataDel(fullDB(),owner().nodePath()+tbl(),*this,true);
}

bool TVArchive::cfgChange( TCfg &co, const TVariant &pc )
{
    if(co.getS() == pc.getS()) return true;
    if(co.name() == "VTYPE" || co.name() == "BSIZE" || co.name() == "BHGRD" || co.name() == "BHRES") setUpBuf();
    else if(co.name() == "SrcMode") {
	if(startStat()) stop();
	setSrcMode((TVArchive::SrcMode)co.getI());
    }
    else if(co.name() == "Source") {
	if(startStat()) stop();
	setSrcMode(SaveCur, co.getS(), true);
    }
    else if(co.name() == "BPER") {
	if(co.getR() < 1e-6) co.setR(1e-6);
	else mBSize = 100/co.getR();
    }

    modif();

    return true;
}

string TVArchive::name( )
{
    string rez = cfg("NAME").getS();
    return rez.size() ? rez : (srcData().size() ? srcData() : mId);
}

AutoHD<TVal> TVArchive::srcPAttr( bool force, const string &ipath )
{
    if(!force && ipath.empty() && !pattr_src.freeStat()) return pattr_src;
    string srcPath = ipath.empty() ? srcData() : ipath;
    AutoHD<TVal> attr;
    if(srcMode() == TVArchive::Passive)	return attr;
    try {
	if(TSYS::strParse(srcPath,0,".") == "sub_DAQ" || TSYS::strParse(srcPath,0,".") == "DAQ")
	    attr = SYS->nodeAt(srcPath,0,'.');
	else attr = SYS->daq().at().attrAt(srcPath,'.');
    }
    catch(TError err) { }

    return attr;
}

TArchiveS &TVArchive::owner( )	{ return *(TArchiveS *)nodePrev(); }

string TVArchive::tbl( )	{ return owner().subId()+"_val"; }

int64_t TVArchive::end( const string &arch )
{
    int64_t rez = 0;
    if(arch.empty() || arch == BUF_ARCH_NM) {
	rez = TValBuf::end();
	if(!arch.empty()) return rez;
    }

    ResAlloc res(aRes, false);
    for(unsigned i_a = 0; i_a < arch_el.size(); i_a++)
	if((arch.empty() || arch == arch_el[i_a]->archivator().workId()) && arch_el[i_a]->end() > rez)
	{
	    rez = arch_el[i_a]->end();
	    if(!arch.empty())	break;
	}

    return rez;
}

int64_t TVArchive::begin( const string &arch )
{
    int64_t cTm = TSYS::curTime();
    int64_t rez = cTm;
    if(arch.empty() || arch == BUF_ARCH_NM) {
	rez = TValBuf::begin();
	if(!arch.empty()) return rez;
    }
    if(!rez) rez = cTm;	//Empty buffer

    ResAlloc res(aRes, false);
    for(unsigned i_a = 0; i_a < arch_el.size(); i_a++)
	if((arch.empty() || arch == arch_el[i_a]->archivator().workId()) && arch_el[i_a]->begin() < rez)
	{
	    rez = arch_el[i_a]->begin();
	    if(!arch.empty())	break;
	}

    return (rez==cTm) ? 0 : rez;
}

int64_t TVArchive::period( const string &arch )
{
    if(arch.empty() || arch == BUF_ARCH_NM) return TValBuf::period();
    ResAlloc res(aRes, false);
    for(unsigned i_a = 0; i_a < arch_el.size(); i_a++)
	if(arch == arch_el[i_a]->archivator().workId())
	    return (int64_t)(1e6*arch_el[i_a]->archivator().valPeriod());
    return 0;
}

void TVArchive::setValType( TFld::Type vl )	{ mVType = vl; }

void TVArchive::setHardGrid( bool vl )		{ mBHGrd = vl; setUpBuf(); modif(); }

void TVArchive::setHighResTm( bool vl )		{ mBHRes = vl; setUpBuf(); modif(); }

void TVArchive::setSize( int vl )		{ mBSize = vl; }

void TVArchive::setPeriod( int64_t vl )
{
    if(!vl) vl = 1000000;
    mBPer = (double)vl/1000000.;
    mBSize = 100/mBPer.getR();
}

void TVArchive::setUpBuf( )	{ makeBuf((TFld::Type)mVType.getI(), mBSize, (int64_t)(1e6*mBPer.getR()), mBHGrd, mBHRes); }

void TVArchive::load_( )
{
    if(!SYS->chkSelDB(DB())) throw TError();
    SYS->db().at().dataGet(fullDB(), owner().nodePath()+tbl(), *this);
    setUpBuf();
}

void TVArchive::save_( )
{
    SYS->db().at().dataSet(fullDB(), owner().nodePath()+tbl(), *this);
}

void TVArchive::start( )
{
    if(!runSt)
	try {
	    runSt = true;
	    setSrcMode();
	} catch(...){ runSt = false; throw; }

    //Attach to archivators
    string arch, archs = cfg("ArchS").getS();
    for(int i_off = 0; (arch = TSYS::strSepParse(archs,0,';',&i_off)).size(); )
	if(!archivatorPresent(arch))
	    try{ archivatorAttach(arch); }
	    catch(TError err)	{ mess_err(err.cat.c_str(),"%s",err.mess.c_str()); }
}

void TVArchive::stop( bool full_del )
{
    runSt = false;

    //Detach all archivators
    vector<string> arch_ls;
    archivatorList(arch_ls);
    for(unsigned i_l = 0; i_l < arch_ls.size(); i_l++)
	archivatorDetach(arch_ls[i_l],full_del,false);

    setSrcMode();
}

void TVArchive::setSrcMode( SrcMode ivl, const string &isrc, bool noex )
{
    SrcMode vl = (ivl == TVArchive::SaveCur) ? srcMode() : ivl;
    string  src = (isrc == "<*>") ? mSource : isrc;

    bool stOnCh = runSt;

    //Disable all links
    if(!pattr_src.freeStat() && (!runSt || vl == Passive || vl != srcMode() || src != srcData()))
    {
	owner().setActVal(this, false);
	srcPAttr().at().setArch(AutoHD<TVArchive>());
	pattr_src.free();
    }

    //Set all links
    if(runSt && vl == ActiveAttr) {
	pattr_src = srcPAttr(true,src);
	if(pattr_src.freeStat()) { if(!noex) throw TError(nodePath().c_str(),_("Connect to source '%s' error."),src.c_str()); }
	else {
	    //Double link prevent
	    if(!pattr_src.at().arch().freeStat() && &pattr_src.at().arch().at() != this)
	    {
		if(!noex) {
		    pattr_src.free();
		    throw TError(nodePath().c_str(), _("The archive '%s' was already connected to target parameter '%s'."),
			srcPAttr(true,src).at().arch().at().id().c_str(), src.c_str());
		}
	    }
	    else {
		pattr_src.at().setArch(AutoHD<TVArchive>(this));
		owner().setActVal(this, true);
	    }
	}
    }

    if(runSt && vl == PassiveAttr) {
	pattr_src = srcPAttr(true,src);
	if(pattr_src.freeStat()) { if(!noex) throw TError(nodePath().c_str(),_("Connect to source '%s' error."),src.c_str()); }
	else {
	    //Double link prevent
	    if(!pattr_src.at().arch().freeStat() && &pattr_src.at().arch().at() != this)
	    {
		if(!noex) {
		    pattr_src.free();
		    throw TError(nodePath().c_str(), _("The archive '%s' was already connected to target parameter '%s'."),
			srcPAttr(true,src).at().arch().at().id().c_str(), src.c_str());
		}
	    }
	    else pattr_src.at().setArch(AutoHD<TVArchive>(this));
	}
    }

    if(mSrcMode.getI() != vl)	mSrcMode = (int)vl;
    if(mSource != src)	mSource = src;
    //Restore start status
    if(stOnCh && !runSt && (srcMode() == Passive || !srcData().empty())) start();
}

TVariant TVArchive::getVal( int64_t *tm, bool up_ord, const string &arch, bool onlyLocal )
{
    //Get from buffer
    if((arch.empty() || arch == BUF_ARCH_NM) && (!tm || (*tm >= begin() && *tm <= end())))
	switch(TValBuf::valType())
	{
	    case TFld::Integer:	return TValBuf::getI(tm, up_ord);
	    case TFld::String:	return TValBuf::getS(tm, up_ord);
	    case TFld::Real:	return TValBuf::getR(tm, up_ord);
	    case TFld::Boolean:	return TValBuf::getB(tm, up_ord);
	    default: break;
	}
    //Get from archivators
    else {
	ResAlloc res(aRes, false);
	vector<pair<float,TVArchEl*> >	propArchs;
	for(unsigned i_a = 0; i_a < arch_el.size(); i_a++) {
	    TVArchivator &archPr = arch_el[i_a]->archivator();
	    if(((arch.empty() && archPr.selPrior()) || arch == archPr.workId()) &&
		    (!tm ||
			(up_ord && *tm <= arch_el[i_a]->end() && *tm > arch_el[i_a]->begin()-(int64_t)(1e6*archPr.valPeriod())) ||
			(!up_ord && *tm < arch_el[i_a]->end()+(int64_t)(1e6*archPr.valPeriod()) && *tm >= arch_el[i_a]->begin())))
		propArchs.push_back(pair<float,TVArchEl*>(archPr.selPrior()/(archPr.valPeriod()?archPr.valPeriod():1), arch_el[i_a]));
	}
	std::sort(propArchs.begin(), propArchs.end());
	TVariant rez;
	for(vector<pair<float,TVArchEl*> >::reverse_iterator iA = propArchs.rbegin(); iA != propArchs.rend(); ++iA)
	    if(!(rez=iA->second->getVal(tm,up_ord,onlyLocal)).isEVal()) return rez;
    }

    return EVAL_REAL;
}

void TVArchive::getVals( TValBuf &buf, int64_t ibeg, int64_t iend, const string &arch, int limit, bool onlyLocal )
{
    //Get from buffer
    if((arch.empty() || arch == BUF_ARCH_NM) && vOK(ibeg,iend)) {
	ibeg = vmax(ibeg,iend-TValBuf::period()*limit);
	TValBuf::getVals(buf,ibeg,iend);
	iend = buf.begin()-1;
	if(arch == BUF_ARCH_NM) return;
    }

    //Get priority archivators list for requested range
    ResAlloc res(aRes, false);
    vector<pair<float,TVArchEl*> >	propArchs;
    for(unsigned i_a = 0; i_a < arch_el.size(); i_a++) {
	TVArchivator &archPr = arch_el[i_a]->archivator();
	if(((arch.empty() && archPr.selPrior()) || arch == archPr.workId()) &&
		((!ibeg || ibeg <= arch_el[i_a]->end()) && (!iend || iend > arch_el[i_a]->begin())) && ibeg <= iend)
	    propArchs.push_back(pair<float,TVArchEl*>(archPr.selPrior()/(archPr.valPeriod()?archPr.valPeriod():1), arch_el[i_a]));
    }
    std::sort(propArchs.begin(), propArchs.end());

    //Process the range by priority
    for(vector<pair<float,TVArchEl*> >::reverse_iterator iA = propArchs.rbegin(); iA != propArchs.rend(); ++iA)
    {
	if(iA->second->begin() >= iend)	continue;	//Try the block from next archivator

	// Decrease the range begin to the limit
	int prevSz = buf.realSize();
	if(!prevSz) ibeg = vmax(ibeg, iend-(int64_t)(1e6*iA->second->archivator().valPeriod())*(limit-buf.realSize()));
	iA->second->getVals(buf, ibeg, iend, onlyLocal);

	if(buf.realSize() != prevSz) {			// Request good
	    iend = buf.begin()-1;
	    if(iend <= ibeg)	break;	//All data requested
	    iA = propArchs.rbegin();	//Next block check 
	}
    }
}

void TVArchive::setVals( TValBuf &buf, int64_t ibeg, int64_t iend, const string &arch )
{
    //Check for put to buffer
    if((arch.empty() || arch == BUF_ARCH_NM) && iend > TValBuf::begin())
	TValBuf::setVals(buf, vmax(ibeg,iend-TValBuf::size()*TValBuf::period()), iend);

    //Put to archivators
    ResAlloc res(aRes, false);
    for(unsigned i_a = 0; i_a < arch_el.size(); i_a++)
	if((arch.empty() || arch == arch_el[i_a]->archivator().workId()))
		//&& (!arch_el[i_a]->lastGet() || ibeg < arch_el[i_a]->lastGet()))	//!!!! Impossible write direct else
	    arch_el[i_a]->setVals(buf, ibeg, iend/*vmin(iend,arch_el[i_a]->lastGet())*/);
}

void TVArchive::getActiveData( )
{
    if(pattr_src.freeStat() || !pattr_src.at().dataActive()) return;

    int64_t tm = TSYS::curTime();
    TVariant vl = pattr_src.at().get(&tm);
    switch(valType())
    {
	case TFld::Boolean: setB(vl, tm);	break;
	case TFld::Integer: setI(vl, tm);	break;
	case TFld::Real:    setR(vl, tm);	break;
	case TFld::String:  setS(vl, tm);	break;
	default: break;
    }
}

void TVArchive::archivatorList( vector<string> &ls )
{
    ResAlloc res(aRes, false);
    ls.clear();
    for(unsigned i_l = 0; i_l < arch_el.size(); i_l++)
	ls.push_back(arch_el[i_l]->archivator().workId());
}

bool TVArchive::archivatorPresent( const string &arch )
{
    ResAlloc res(aRes, false);
    try {
	AutoHD<TVArchivator> archivat = owner().at(TSYS::strSepParse(arch,0,'.')).at().
						valAt(TSYS::strSepParse(arch,1,'.'));
	for(unsigned i_l = 0; i_l < arch_el.size(); i_l++)
	    if(&arch_el[i_l]->archivator() == &archivat.at())
		return true;
    } catch(TError err){ }

    return false;
}

void TVArchive::archivatorAttach( const string &arch )
{
    ResAlloc res(aRes, true);

    AutoHD<TVArchivator> archivat = owner().at(TSYS::strSepParse(arch,0,'.')).at().valAt(TSYS::strSepParse(arch,1,'.'));

    if(!archivat.at().startStat())
	throw TError(nodePath().c_str(),_("Archivator '%s' error or it is not started."),arch.c_str());

    if(startStat()) {	//Attach allow only to started archive
	int i_l, i_ins = -1;
	for(i_l = 0; i_l < (int)arch_el.size(); i_l++) {
	    if(&arch_el[i_l]->archivator() == &archivat.at()) break;
	    if(i_ins < 0 && archivat.at().valPeriod() <= arch_el[i_l]->archivator().valPeriod()) i_ins = i_l;
	}
	if(i_l >= (int)arch_el.size()) {
	    if(i_ins < 0) arch_el.push_back(archivat.at().archivePlace(*this));
	    else arch_el.insert(arch_el.begin()+i_ins,archivat.at().archivePlace(*this));
	}
    }

    if(!TRegExp("(^|;)"+arch+"(;|$)").test(cfg("ArchS").getS())) {
	string als;
	for(unsigned i_l = 0; i_l < arch_el.size(); i_l++)
	    als += arch_el[i_l]->archivator().workId()+";";
	cfg("ArchS").setS(als);
    }
}

void TVArchive::archivatorDetach( const string &arch, bool full, bool toModify )
{
    ResAlloc res(aRes, true);

    AutoHD<TVArchivator> archivat = owner().at(TSYS::strSepParse(arch,0,'.')).at().
					    valAt(TSYS::strSepParse(arch,1,'.'));
    //Find archivator
    for(unsigned i_l = 0; i_l < arch_el.size(); )
	if(&arch_el[i_l]->archivator() == &archivat.at()) {
	    archivat.at().archiveRemove(id(),full);
	    arch_el.erase(arch_el.begin()+i_l);
	} else i_l++;

    if(toModify && TRegExp("(^|;)"+arch+"(;|$)").test(cfg("ArchS").getS()))
    {
	string als;
	for(unsigned i_l = 0; i_l < arch_el.size(); i_l++)
	    als += arch_el[i_l]->archivator().workId()+";";
	cfg("ArchS").setS(als);
    }
}

void TVArchive::archivatorSort()
{
    int rep_try;

    ResAlloc res(aRes, true);

    do {
	rep_try = false;
	for(unsigned i_l = 1; i_l < arch_el.size(); i_l++)
	    if(arch_el[i_l-1]->archivator().valPeriod() > arch_el[i_l]->archivator().valPeriod())
	    {
		TVArchEl *t_el = arch_el[i_l-1];
		arch_el[i_l-1] = arch_el[i_l];
		arch_el[i_l] = t_el;
		rep_try = true;
	    }
    }
    while(rep_try);
}

string TVArchive::makeTrendImg( int64_t ibeg, int64_t iend, const string &iarch, int hsz, int vsz, double valmax, double valmin, string *tp )
{
    string rez;

    string lab_tm, lab_dt;
    time_t tm_t;
    struct tm ttm, ttm1 = ttm;
    int64_t c_tm;
    int hv_border,		//Image border size
	h_w_start, h_w_size,	//Trend window horizontal start and size
	v_w_start, v_w_size;	//Trend window vertical start and size
    string sclMarkFont = "Times";
    int mrkFontSize = 8, begMarkBrd = -1, endMarkBrd = 0;

    //Check and get data
    if(ibeg >= iend || valType() == TFld::String) return rez;
    TValBuf buf(TFld::Real, 0, 0, false, true);

    //Calc base image parameters
    hv_border = 5;
    h_w_start = hv_border;
    h_w_size  = hsz-h_w_start-hv_border;
    v_w_start = hv_border;
    v_w_size  = vsz-v_w_start-hv_border;

    int mrkHeight = 0;

#if HAVE_GD_CORE
    int brect[8];
    char *gdR = gdImageStringFT(NULL, &brect[0], 0, (char*)sclMarkFont.c_str(), mrkFontSize, 0, 0, 0, (char*)"000000");
    if(gdR) mess_err(nodePath().c_str(), _("gdImageStringFT for font '%s' error: %s."), sclMarkFont.c_str(), gdR);
    else mrkHeight = brect[3]-brect[7];
    //if( mrkHeight <= 0 ) return rez;
    int hmax_ln = vsz / (mrkHeight?(brect[2]-brect[6]):15);

    v_w_size -= 2*mrkHeight;

    //Create image
    gdImagePtr im = gdImageCreate(hsz, vsz);
    int clr_backgr = gdImageColorAllocate(im, 0x35, 0x35, 0x35);
    int clr_grid   = gdImageColorAllocate(im, 0x8e, 0x8e, 0x8e);
    int clr_symb   = gdImageColorAllocate(im, 0x11, 0xff, 0x5f);
    int clr_trnd   = gdImageColorAllocate(im, 0x1f, 0xf2, 0xff);

    gdImageFilledRectangle(im, 0, 0, hsz-1, vsz-1, clr_backgr);
    gdImageRectangle(im, h_w_start, v_w_start, h_w_start+h_w_size, v_w_start+v_w_size, clr_grid);

#else
    mrkHeight = mrkFontSize;
    int mrkWidth = 5*mrkFontSize;
    int hmax_ln = vsz / 15;
    v_w_size -= 2*mrkHeight;
    const char	*clr_backgr = "#353535",
		*clr_grid = "#8e8e8e",
		*clr_symb = "#11ff5f",
		*clr_trnd = "#1ff2ff";

    XMLNode im("svg");
    im.setAttr("xmlns","http://www.w3.org/2000/svg")->setAttr("version","1.1")->setAttr("width",i2s(hsz))->setAttr("height",i2s(vsz));
    im.childAdd("rect")->setAttr("width",i2s(hsz))->setAttr("height",i2s(vsz))->
	setAttr("stroke","none")->setAttr("stroke-width","0")->setAttr("fill",clr_backgr);
    /*im.childAdd("rect")->setAttr("x",i2s(h_w_start))->setAttr("y",i2s(v_w_start))->
	setAttr("width",i2s(h_w_size))->setAttr("height",i2s(v_w_size))->
	setAttr("stroke",clr_grid)->setAttr("stroke-width","1")->setAttr("fill","none");*/
    XMLNode *trPath = NULL;
#endif

    //Make horisontal grid and symbols
    int64_t h_div = 1;
    int64_t h_min = ibeg;
    int64_t h_max = iend;
    if(hmax_ln >= 2) {
	int hvLev = 0;
	int64_t hLen = iend - ibeg;
	if(hLen/86400000000ll >= 2)	{ hvLev = 5; h_div = 86400000000ll; }	//Days
	else if(hLen/3600000000ll >= 2)	{ hvLev = 4; h_div =  3600000000ll; }	//Hours
	else if(hLen/60000000 >= 2)	{ hvLev = 3; h_div =    60000000; }	//Minutes
	else if(hLen/1000000 >= 2)	{ hvLev = 2; h_div =     1000000; }	//Seconds
	else if(hLen/1000 >= 2)		{ hvLev = 1; h_div =        1000; }	//Milliseconds
	while(hLen/h_div > hmax_ln)	h_div *= 10;
	while(hLen/h_div < hmax_ln/2)	h_div /= 2;

	// Select most like archivator
	string rarch = iarch;
	if(rarch.empty() && !vOK(ibeg,iend)) {
	    double best_a_per = 0;
	    ResAlloc res(aRes, false);
	    for(unsigned i_a = 0; i_a < arch_el.size(); i_a++)
		if(arch_el[i_a]->archivator().valPeriod() > best_a_per &&
		    arch_el[i_a]->archivator().valPeriod() <= (double)(h_max-h_min)/(1e5*hsz))
		{
		    best_a_per = arch_el[i_a]->archivator().valPeriod();
		    rarch = arch_el[i_a]->archivator().workId();
		}
	}

	getVals(buf, h_min, h_max, rarch, 600000);
	if(!buf.end() || !buf.begin()) {
#if HAVE_GD_CORE
	    gdImageDestroy(im);
#endif
	    return rez;
	}

	// Draw full trend's data and time to the trend end position
	if(mrkHeight) {
	    tm_t = iend/1000000;
	    localtime_r(&tm_t, &ttm);
	    lab_dt = TSYS::strMess("%d-%02d-%d", ttm.tm_mday, ttm.tm_mon+1, ttm.tm_year+1900);
	    if(ttm.tm_sec == 0 && iend%1000000 == 0) lab_tm = TSYS::strMess("%d:%02d", ttm.tm_hour, ttm.tm_min);
	    else if(iend%1000000 == 0) lab_tm = TSYS::strMess("%d:%02d:%02d", ttm.tm_hour, ttm.tm_min, ttm.tm_sec);
	    else lab_tm = TSYS::strMess("%d:%02d:%g", ttm.tm_hour, ttm.tm_min, (float)ttm.tm_sec+(float)(iend%1000000)/1e6);

#if HAVE_GD_CORE
	    gdImageStringFT(NULL, &brect[0], 0, (char*)sclMarkFont.c_str(), mrkFontSize, 0, 0, 0, (char*)lab_dt.c_str());
	    int markBrd = h_w_start + h_w_size - (brect[2]-brect[6]);
	    endMarkBrd = markBrd;
	    gdImageStringFT(im, NULL, clr_symb, (char*)sclMarkFont.c_str(), mrkFontSize, 0, markBrd, v_w_start+v_w_size+3+2*mrkHeight, (char*)lab_dt.c_str());
	    gdImageStringFT(NULL, &brect[0], 0, (char*)sclMarkFont.c_str(), mrkFontSize, 0, 0, 0, (char*)lab_tm.c_str());
	    markBrd = h_w_start + h_w_size - (brect[2]-brect[6]);
	    endMarkBrd = vmin(endMarkBrd, markBrd);
	    gdImageStringFT(im, NULL, clr_symb, (char*)sclMarkFont.c_str(), mrkFontSize, 0, markBrd, v_w_start+v_w_size+3+mrkHeight, (char*)lab_tm.c_str());
#else
	    endMarkBrd = h_w_start + h_w_size - mrkWidth;
	    im.childAdd("text")->setAttr("text-anchor","end")->setAttr("stroke","none")->
		setAttr("font-family",sclMarkFont)->setAttr("font-size",TSYS::strMess("%dpx",mrkFontSize))->setAttr("fill",clr_symb)->
		setAttr("x",i2s(h_w_start+h_w_size))->setAttr("y",i2s(v_w_start+v_w_size+3+2*mrkHeight))->setText(lab_dt);
	    im.childAdd("text")->setAttr("text-anchor","end")->setAttr("stroke","none")->
		setAttr("font-family",sclMarkFont)->setAttr("font-size",TSYS::strMess("%dpx",mrkFontSize))->setAttr("fill",clr_symb)->
		setAttr("x",i2s(h_w_start+h_w_size))->setAttr("y",i2s(v_w_start+v_w_size+3+mrkHeight))->setText(lab_tm);
#endif
	}

	// Draw grid and/or markers
	bool first_m = true;
	for(int64_t i_h = h_min; true; ) {
	    //  Draw grid
	    int h_pos = h_w_start + h_w_size*(i_h-h_min)/(h_max-h_min);
#if HAVE_GD_CORE
	    gdImageLine(im, h_pos, v_w_start, h_pos, v_w_start+v_w_size, clr_grid);
#else
	    im.childAdd("rect")->setAttr("x",i2s(h_pos))->setAttr("y",i2s(v_w_start))->
		setAttr("width","1")->setAttr("height",i2s(v_w_size))->setAttr("stroke","none")->setAttr("fill",clr_grid);
#endif

	    if(mrkHeight && !(i_h%h_div) && i_h != iend) {
		tm_t = i_h/1000000;
		localtime_r(&tm_t, &ttm);
		int chLev = -1;
		if(!first_m) {
		    if(ttm.tm_mon > ttm1.tm_mon || ttm.tm_year > ttm1.tm_year)  chLev = 5;
		    else if(ttm.tm_mday > ttm1.tm_mday)	chLev = 4;
		    else if(ttm.tm_hour > ttm1.tm_hour)	chLev = 3;
		    else if(ttm.tm_min > ttm1.tm_min)	chLev = 2;
		    else if(ttm.tm_sec > ttm1.tm_sec)	chLev = 1;
		    else chLev = 0;
		}
		lab_dt = lab_tm = "";
		//Date
		if(hvLev == 5 || chLev >= 4)
		    lab_dt = (chLev>=5 || chLev==-1) ? TSYS::strMess("%d-%02d-%d",ttm.tm_mday,ttm.tm_mon+1,ttm.tm_year+1900) :
						       TSYS::strMess("%d",ttm.tm_mday);
		//Hours and minuts
		if((hvLev == 4 || hvLev == 3 || ttm.tm_hour || ttm.tm_min) && !ttm.tm_sec) lab_tm = TSYS::strMess("%d:%02d", ttm.tm_hour, ttm.tm_min);
		//Seconds
		else if((hvLev == 2 || ttm.tm_sec) && !(i_h%1000000))
		    lab_tm = (chLev>=2 || chLev==-1) ? TSYS::strMess("%d:%02d:%02d",ttm.tm_hour,ttm.tm_min,ttm.tm_sec) :
						       TSYS::strMess(_("%ds"),ttm.tm_sec);
		//Milliseconds
		else if(hvLev <= 1 || i_h%1000000)
		    lab_tm = (chLev>=2 || chLev==-1) ? TSYS::strMess("%d:%02d:%g",ttm.tm_hour,ttm.tm_min,(float)ttm.tm_sec+(float)(i_h%1000000)/1e6) :
			     (chLev>=1) ? TSYS::strMess(_("%gs"),(float)ttm.tm_sec+(float)(i_h%1000000)/1e6) :
					  TSYS::strMess(_("%gms"),(double)(i_h%1000000)/1000.);
		int tpos;
#if HAVE_GD_CORE
		int wdth, endPosTm = 0, endPosDt = 0;
		if(lab_dt.size()) {
		    gdImageStringFT(NULL, &brect[0], 0, (char*)sclMarkFont.c_str(), mrkFontSize, 0, 0, 0, (char*)lab_dt.c_str());
		    wdth = brect[2] - brect[6];
		    tpos = vmax(h_pos-wdth/2, hv_border);
		    if((tpos+wdth) < endMarkBrd && tpos > begMarkBrd) {
			gdImageStringFT(im, NULL, clr_symb, (char*)sclMarkFont.c_str(), mrkFontSize, 0, tpos, v_w_start+v_w_size+3+2*mrkHeight, (char*)lab_dt.c_str());
			endPosTm = tpos + wdth;
		    }
		}
		if(lab_tm.size()) {
		    gdImageStringFT(NULL, &brect[0], 0, (char*)sclMarkFont.c_str(), mrkFontSize, 0, 0, 0, (char*)lab_tm.c_str());
		    wdth = brect[2] - brect[6];
		    tpos = vmax(h_pos-wdth/2,hv_border);
		    if((tpos+wdth) < endMarkBrd && tpos > begMarkBrd) {
			gdImageStringFT(im, NULL, clr_symb, (char*)sclMarkFont.c_str(), mrkFontSize, 0, tpos, v_w_start+v_w_size+3+mrkHeight, (char*)lab_tm.c_str());
			endPosDt = tpos + wdth;
		    }
		}
		begMarkBrd = vmax(begMarkBrd, vmax(endPosTm,endPosDt));
#else
		tpos = vmax(h_pos-mrkWidth/2, hv_border);
		if((tpos+mrkWidth) < endMarkBrd && tpos > begMarkBrd) {
		    if(lab_dt.size()) {
			im.childAdd("text")->setAttr("text-anchor","middle")->setAttr("stroke","none")->
			    setAttr("font-family",sclMarkFont)->setAttr("font-size",TSYS::strMess("%dpx",mrkFontSize))->setAttr("fill",clr_symb)->
			    setAttr("x",i2s(tpos+mrkWidth/2))->setAttr("y",i2s(v_w_start+v_w_size+3+2*mrkHeight))->setText(lab_dt);
		    }
		    if(lab_tm.size()) {
			im.childAdd("text")->setAttr("text-anchor","middle")->setAttr("stroke","none")->
			    setAttr("font-family",sclMarkFont)->setAttr("font-size",TSYS::strMess("%dpx",mrkFontSize))->setAttr("fill",clr_symb)->
			    setAttr("x",i2s(tpos+mrkWidth/2))->setAttr("y",i2s(v_w_start+v_w_size+3+mrkHeight))->setText(lab_tm);
		    }
		    begMarkBrd = vmax(begMarkBrd, tpos+mrkWidth);
		}
#endif
		memcpy((char*)&ttm1, (char*)&ttm, sizeof(tm));
		first_m = false;
	    }
	    //  Next
	    if(i_h >= iend)	break;
	    i_h = (i_h/h_div)*h_div + h_div;
	    if(i_h > iend)	i_h = iend;
	}
    }

    //Make vertical grid and symbols
    double c_val = EVAL_REAL, v_max = -3e300, v_min = 3e300;
    for(c_tm = buf.begin(); c_tm <= buf.end(); c_tm++) {
	c_val = buf.getR(&c_tm, true);
	if(c_val == EVAL_REAL)	continue;
	v_min = vmin(v_min, c_val);
	v_max = vmax(v_max, c_val);
    }
    if(v_max == -3e300) {
#if HAVE_GD_CORE
	gdImageDestroy(im);
#endif
	return rez;
    }
    else if((v_max-v_min) < 1e-30 && fabs(v_max) < 1e-30)	{ v_max += 0.5; v_min -= 0.5; }
    else if((v_max-v_min) / fabs(v_min+(v_max-v_min)/2) < 0.001)
    {
        double wnt_dp = 0.001*fabs(v_min+(v_max-v_min)/2)-(v_max-v_min);
        v_min -= wnt_dp/2;
        v_max += wnt_dp/2;
    }
    if(valmax > valmin) {
	v_max = vmax(v_max, valmax);
	v_min = vmin(v_min, valmin);
    }

    if(TSYS::realRound(v_max,3) == TSYS::realRound(v_min,3))	{ v_max+=1.0; v_min-=1.0; }
    double v_div = 1;
    double v_len = v_max - v_min;
    while(v_len > 10)	{ v_div *= 10; v_len /= 10; }
    while(v_len < 1) 	{ v_div /= 10; v_len *= 10; }
    v_min = floor(v_min/v_div)*v_div;
    v_max = ceil(v_max/v_div)*v_div;
    while(((v_max-v_min)/v_div) < 5) v_div /= 2;
    //  Draw vertical grid and symbols
    for(double i_v = v_min; (v_max-i_v)/v_div > -0.1; i_v += v_div)
    {
	int v_pos = v_w_start + v_w_size - (int)((double)v_w_size*(i_v-v_min)/(v_max-v_min));
#if HAVE_GD_CORE
	gdImageLine(im, h_w_start, v_pos, h_w_start+h_w_size, v_pos, clr_grid);
	if(mrkHeight)
	    gdImageStringFT(im, NULL, clr_symb, (char*)sclMarkFont.c_str(), mrkFontSize, 0,
		hv_border+2, v_pos+((fabs((v_max-i_v)/v_div) < 0.1)?mrkHeight:0), (char*)TSYS::strMess("%g",i_v).c_str());
#else
	im.childAdd("rect")->setAttr("x",i2s(h_w_start))->setAttr("y",i2s(v_pos))->
	    setAttr("width",i2s(h_w_size))->setAttr("height","1")->setAttr("stroke","none")->setAttr("fill",clr_grid);
	if(mrkHeight)
	    im.childAdd("text")->setAttr("x",i2s(hv_border+2))->setAttr("y",i2s(v_pos+((fabs((v_max-i_v)/v_div) < 0.1)?mrkHeight:0)))->
		setAttr("font-family",sclMarkFont)->setAttr("font-size",TSYS::strMess("%dpx",mrkFontSize))->setAttr("stroke","none")->
		setAttr("fill",clr_symb)->setText(TSYS::strMess("%g",i_v));
#endif
    }

    //Draw trend
    double aver_vl = EVAL_REAL;
    int    aver_pos= 0;
    int64_t aver_tm = 0;
    int64_t aver_lsttm = 0;
    double prev_vl = EVAL_REAL;
    int    prev_pos = 0;
    for(c_tm = buf.begin(); true; c_tm++) {
	int c_pos;
	if(c_tm <= buf.end()) {
	    c_val = buf.getR(&c_tm, true);
	    c_pos = h_w_start+h_w_size*(vmax(vmin(c_tm,h_max),h_min)-h_min)/(h_max-h_min);
	}else c_pos = 0;
	//Square Average
	if(aver_pos == c_pos) {
	    if(aver_vl == EVAL_REAL)	aver_vl = c_val;
	    else if(c_val != EVAL_REAL)
		aver_vl = (aver_vl*(double)(c_tm-aver_tm)+c_val*(double)(c_tm-aver_lsttm))/
			  ((double)(2*c_tm-aver_tm-aver_lsttm));
	    aver_lsttm = c_tm;
	    continue;
	}
	//Write point and line
#if HAVE_GD_CORE
	if(aver_vl != EVAL_REAL) {
	    int c_vpos = v_w_start + v_w_size - (int)((double)v_w_size*(aver_vl-v_min)/(v_max-v_min));

	    gdImageSetPixel(im, aver_pos, c_vpos, clr_trnd);
	    if(prev_vl != EVAL_REAL) {
		int c_vpos_prv = v_w_start + v_w_size - (int)((double)v_w_size*(prev_vl-v_min)/(v_max-v_min));
		gdImageLine(im, prev_pos, c_vpos_prv, aver_pos, c_vpos, clr_trnd);
	    }
	}
#else
	if(aver_vl != EVAL_REAL) {
	    int c_vpos = v_w_start + v_w_size - (int)((double)v_w_size*(aver_vl-v_min)/(v_max-v_min));
	    if(!trPath) trPath = im.childAdd("path")->setAttr("stroke",clr_trnd)->setAttr("stroke-width","1")->setAttr("fill","none");
	    if(prev_vl == EVAL_REAL) trPath->setAttr("d",trPath->attr("d")+TSYS::strMess("M%d,%d",aver_pos,c_vpos));	//Set single point
	    else trPath->setAttr("d",trPath->attr("d")+TSYS::strMess("L%d,%d",aver_pos,c_vpos));
	}
	else if(prev_vl != EVAL_REAL) {
	    int c_vpos = v_w_start + v_w_size - (int)((double)v_w_size*(prev_vl-v_min)/(v_max-v_min));
	    trPath->setAttr("d",trPath->attr("d")+TSYS::strMess("L%d.1,%d.1",prev_pos,c_vpos));
	}
#endif
	prev_vl  = aver_vl;
	prev_pos = aver_pos;
	aver_vl  = c_val;
	aver_pos = c_pos;
	aver_tm = aver_lsttm = c_tm;
	if(!c_pos) break;
    }

    //Get image and transfer it
#if HAVE_GD_CORE
    int img_sz;
    char *img_ptr = (char *)gdImagePngPtrEx(im, &img_sz, 1);
    rez.assign(img_ptr,img_sz);
    gdFree(img_ptr);
    gdImageDestroy(im);

    if(tp) *tp = "png";
#else
    rez = im.save(XMLNode::XMLHeader);
    if(tp) *tp = "svg+xml";
#endif

    return rez;
}

TVariant TVArchive::objFuncCall( const string &iid, vector<TVariant> &prms, const string &user )
{
    //Configuration functions call
    TVariant cfRez = objFunc(iid, prms, user);
    if(!cfRez.isNull()) return cfRez;

    return TCntrNode::objFuncCall(iid, prms, user);
}

void TVArchive::cntrCmdProc( XMLNode *opt )
{
    string a_path = opt->attr("path");
    //Service commands process
    if(a_path == "/serv/val") {		//Values access
	if(ctrChkNode(opt,"info",RWRWRW,"root","root",SEC_RD)) {	//Value's data information
	    string arch = opt->attr("arch");
	    opt->setAttr("end", ll2s(end(arch)));
	    opt->setAttr("beg", ll2s(begin(arch)));
	    opt->setAttr("vtp", i2s(TValBuf::valType()));
	    if(arch.empty() || arch == BUF_ARCH_NM) opt->setAttr("per",ll2s(TValBuf::period()));
	    else {
		ResAlloc res(aRes, false);
		for(unsigned i_a = 0; i_a < arch_el.size(); i_a++)
		    if(arch == arch_el[i_a]->archivator().workId())
		    { opt->setAttr("per",ll2s((int64_t)(1e6*arch_el[i_a]->archivator().valPeriod()))); break; }
	    }
	}
	else if(ctrChkNode(opt,"get",RWRWRW,"root","root",SEC_RD)) {	//Value's data request
	    int64_t	tm	= s2ll(opt->attr("tm"));
	    int64_t	tm_grnd	= s2ll(opt->attr("tm_grnd"));
	    string	arch	= opt->attr("arch");
	    bool	local	= s2i(opt->attr("local"));

	    // Process one value request
	    if(!tm) 	tm = TSYS::curTime();
	    if(!tm_grnd) {
		opt->setText(getVal(&tm,false,arch,local).getS());
		opt->setAttr("tm", ll2s(tm));
		return;
	    }
	    if(tm < tm_grnd)	throw TError(nodePath().c_str(),_("Range error"));

	    int64_t period = s2ll(opt->attr("per"));

	    // Process of archive block request
	    TValBuf buf(TValBuf::valType(), 0/*100000*/, vmax(TValBuf::period(),period), true, true);

	    //  Get the values buffer
	    if((arch.empty() || arch == BUF_ARCH_NM) && vOK(tm_grnd,tm)) {
		TValBuf::getVals(buf, tm_grnd, tm);
		opt->setAttr("arch", BUF_ARCH_NM);
	    }
	    else {
		ResAlloc res(aRes, false);
		//Get priority archivators list for requested range
		vector<pair<float,TVArchEl*> >	propArchs;
		for(unsigned i_a = 0; i_a < arch_el.size(); i_a++) {
		    TVArchivator &archPr = arch_el[i_a]->archivator();
		    if(((arch.empty() && archPr.selPrior()) || arch == archPr.workId()) &&
			    tm_grnd <= arch_el[i_a]->end() && tm >= arch_el[i_a]->begin())
			propArchs.push_back(pair<float,TVArchEl*>(archPr.selPrior()/vmax(1,abs(archPr.valPeriod()-period/1e6)), arch_el[i_a]));
		}
		std::sort(propArchs.begin(), propArchs.end());

		//Process the range by priority
		for(vector<pair<float,TVArchEl*> >::reverse_iterator iA = propArchs.rbegin(); iA != propArchs.rend(); ++iA)
		{
		    buf.setPeriod(vmax((int64_t)(1e6*iA->second->archivator().valPeriod()),period));
		    iA->second->getVals(buf, vmax(tm_grnd,iA->second->begin()), tm, local);		//vmax for allow access to next level archive
													//into single request, like from 1m to 10m
		    if(!buf.realSize()) continue;	//Try next
		    opt->setAttr("arch", iA->second->archivator().workId());
		    break;
		}
		res.release();
	    }

	    //  Prepare buffer's data for transfer
	    bool    isEnd = false;	//Last archive value process
	    string  text;
	    text.reserve(100);
	    int vpos_end = 0, vpos_cur;
	    int64_t ibeg = buf.begin(), iend = buf.end();
	    period = vmax(period,buf.period());
	    int mode = s2i(opt->attr("mode"));
	    if(mode < 0 || mode > 2) throw TError(nodePath().c_str(),_("No support data mode '%d'"),mode);
	    switch(buf.valType())
	    {
		case TFld::Boolean: {
		    char tval_pr = EVAL_BOOL, tval_pr1 = EVAL_BOOL;
		    while(ibeg <= iend) {
			char tval = buf.getB(&ibeg, true);
			vpos_cur = (ibeg-buf.begin())/period;
			if(vpos_cur > vpos_end)
			    bool1: switch(mode)
			    {
				case 0: text += ((tval_pr==EVAL_BOOL) ? EVAL_STR : i2s(tval_pr)) + "\n"; break;
				case 1:
				    if(!vpos_end || tval_pr != tval_pr1)
					text += i2s(vpos_end) + " " + ((tval_pr==EVAL_BOOL) ? EVAL_STR : i2s(tval_pr)) + "\n";
				    tval_pr1 = tval_pr;
				    break;
				case 2: text += tval_pr; break;
			    }
			tval_pr = tval;
			vpos_end = vpos_cur;
			ibeg++;
			if(isEnd)	break;
			if(ibeg > iend)	{ isEnd = true;	goto bool1; }	//Flush last value
		    }
		    break;
		}
		case TFld::Integer: {
		    float round_perc = s2r(opt->attr("round_perc"));
		    int64_t tval_pr = EVAL_INT, tval_pr1 = EVAL_INT;
		    while(ibeg <= iend) {
			int64_t tval = buf.getI(&ibeg, true);
			vpos_cur = (ibeg-buf.begin())/period;
			if(vpos_cur <= vpos_end) {
			    if(tval == EVAL_INT) tval = tval_pr;
			    if(tval != EVAL_INT && tval_pr != EVAL_INT) {
				int s_k = ibeg-period*(ibeg/period);
				int n_k = buf.period();
				tval = ((int64_t)tval_pr*s_k+(int64_t)tval*n_k)/(s_k+n_k);
			    }
			}
			else int1: switch(mode)
			{
			    case 0: text += ((tval_pr==EVAL_INT) ? EVAL_STR : ll2s(tval_pr)) + "\n"; break;
			    case 1:
				if(!(vpos_end && (tval_pr==tval_pr1 || (((tval_pr1 > 0 && tval_pr > 0) || (tval_pr1 < 0 && tval_pr < 0)) &&
					100.*(double)abs(tval_pr1-tval_pr)/(double)vmax(abs(tval_pr1),abs(tval_pr)) <= round_perc))))
				{
				    text += i2s(vpos_end) + " " + ((tval_pr==EVAL_INT) ? EVAL_STR : ll2s(tval_pr)) + "\n";
				    tval_pr1 = tval_pr;
				}
				break;
			    case 2: text += string((char*)&tval_pr, sizeof(int64_t)); break;
			}
			tval_pr = tval;
			vpos_end = vpos_cur;
			ibeg++;
			if(isEnd)	break;
			if(ibeg > iend)	{ isEnd = true;	goto int1; }	//Flush last value
		    }
		    break;
		}
		case TFld::Real: {
		    float round_perc = s2r(opt->attr("round_perc"));
		    int   real_prec  = s2i(opt->attr("real_prec"));
		    if(!real_prec) real_prec = 10;
		    double tval_pr = EVAL_REAL, tval_pr1 = EVAL_REAL;
		    while(ibeg <= iend) {
			double tval = buf.getR(&ibeg, true);
			vpos_cur = (ibeg-buf.begin())/period;
			if(vpos_cur <= vpos_end) {
			    if(tval == EVAL_REAL) tval = tval_pr;
			    if(tval != EVAL_REAL && tval_pr != EVAL_REAL) {
				int s_k = ibeg-period*(ibeg/period);
				int n_k = buf.period();
				tval = (tval_pr*s_k+tval*n_k)/(s_k+n_k);
			    }
			}
			else real1: switch(mode)
			{
			    case 0: text += ((tval_pr==EVAL_REAL) ? EVAL_STR : r2s(tval_pr,real_prec)) + "\n";	break;
			    case 1:
				if(!(vpos_end && (tval_pr==tval_pr1 || (((tval_pr1 > 0 && tval_pr > 0) || (tval_pr1 < 0 && tval_pr < 0)) &&
					100.*fabs(tval_pr1-tval_pr)/vmax(fabs(tval_pr1),fabs(tval_pr)) <= round_perc))))
				{
				    text += i2s(vpos_end) + " " + ((tval_pr==EVAL_REAL) ? EVAL_STR : r2s(tval_pr,real_prec)) + "\n";
				    tval_pr1 = tval_pr;
				}
				break;
			    case 2: text += string((char*)&tval_pr, sizeof(double));	break;
			}
			tval_pr = tval;
			vpos_end = vpos_cur;
			ibeg++;
			if(isEnd)	break;
			if(ibeg > iend)	{ isEnd = true;	goto real1; }	//Flush last value
		    }
		    break;
		}
		case TFld::String: {
		    string tval_pr = EVAL_STR, tval_pr1;
		    while(ibeg <= iend) {
			string tval = buf.getS(&ibeg, true);
			vpos_cur = (ibeg-buf.begin())/period;
			if(vpos_cur > vpos_end)
			    str1: switch(mode)
			    {
				case 0: text += TSYS::strEncode(tval_pr,TSYS::Custom,"\n")+"\n"; break;
				case 1:
				    if(!vpos_end || tval_pr != tval_pr1)
					text += i2s(vpos_end)+" "+TSYS::strEncode(tval_pr,TSYS::Custom,"\n")+"\n";
				    tval_pr1 = tval_pr;
				    break;
				case 2: throw TError(nodePath().c_str(),_("Binary mode no support for string data"));
			    }
			tval_pr = tval;
			vpos_end = vpos_cur;
			ibeg++;
			if(isEnd)	break;
			if(ibeg > iend)	{ isEnd = true;	goto str1; }	//Flush last value
		    }
		    break;
		}
		default: break;
	    }
	    //  Set result and information attributes
	    opt->setAttr("tm", ll2s((buf.end()/period)*period));
	    opt->setAttr("tm_grnd", ll2s((buf.begin()/period)*period + ((buf.begin()%period)?period:0)));
	    opt->setAttr("per", ll2s(period));
	    opt->setText((mode==2) ? TSYS::strEncode(text,TSYS::base64) : text);
	    opt->setAttr("vtp", i2s(TValBuf::valType()));
	}
	else if(ctrChkNode(opt,"name",RWRWRW,"root","root",SEC_RD)) {	//Archive name request
	    AutoHD<TVal> vObj = srcPAttr(true);
	    opt->setText((!vObj.freeStat() && vObj.at().owner().vlPresent("NAME")) ?
		vObj.at().owner().vlAt("NAME").at().getS()+"."+vObj.at().name():name());
	}
	return;
    }

    //Interface comands process
    // Info command process
    if(opt->name() == "info") {
	TCntrNode::cntrCmdProc(opt);
	ctrMkNode("oscada_cntr",opt,-1,"/",_("Value archive: ")+name(),RWRWR_,"root",SARH_ID);
	if(ctrMkNode("area",opt,-1,"/prm",_("Archive"))) {
	    if(ctrMkNode("area",opt,-1,"/prm/st",_("State"))) {
		ctrMkNode("fld",opt,-1,"/prm/st/st",_("Running"),RWRWR_,"root",SARH_ID,1,"tp","bool");
		ctrMkNode("fld",opt,-1,"/prm/st/bEnd",_("Buffer end"),R_R_R_,"root",SARH_ID,1,"tp","str");
		ctrMkNode("fld",opt,-1,"/prm/st/bBeg",_("Buffer begin"),R_R_R_,"root",SARH_ID,1,"tp","str");
		ctrMkNode("fld",opt,-1,"/prm/st/db",_("Archive DB"),RWRWR_,"root",SARH_ID,4,
		    "tp","str","dest","select","select","/db/list","help",TMess::labDB());
	    }
	    if(ctrMkNode("area",opt,-1,"/prm/cfg",_("Configuration"))) {
		TConfig::cntrCmdMake(opt,"/prm/cfg",0,"root",SARH_ID,RWRWR_);
		if(srcMode() == PassiveAttr || srcMode() == ActiveAttr)
		    ctrMkNode("fld",opt,-1,"/prm/cfg/Source","",RWRWR_,"root",SARH_ID,2,"dest","sel_ed","select","/cfg/prm_atr_ls");
		else ctrRemoveNode(opt,"/prm/cfg/Source");
		ctrRemoveNode(opt,"/prm/cfg/ArchS");
	    }
	}
	if(ctrMkNode("area",opt,-1,"/arch",_("Archivators"),R_R_R_,"root",SARH_ID)) {
	    if(ctrMkNode("table",opt,-1,"/arch/arch",_("Archivators"),RWRWR_,"root",SARH_ID,1,"key","arch"))
	    {
		ctrMkNode("list",opt,-1,"/arch/arch/arch",_("Archivator"),R_R_R_,"root",SARH_ID,1,"tp","str");
		ctrMkNode("list",opt,-1,"/arch/arch/start",_("Start"),R_R_R_,"root",SARH_ID,1,"tp","bool");
		ctrMkNode("list",opt,-1,"/arch/arch/proc",_("Process"),RWRWR_,"root",SARH_ID,1,"tp","bool");
		ctrMkNode("list",opt,-1,"/arch/arch/per",_("Period (s)"),R_R_R_,"root",SARH_ID,1,"tp","real");
		ctrMkNode("list",opt,-1,"/arch/arch/beg",_("Begin"),R_R_R_,"root",SARH_ID,1,"tp","str");
		ctrMkNode("list",opt,-1,"/arch/arch/end",_("End"),R_R_R_,"root",SARH_ID,1,"tp","str");
	    }
	}
	if(runSt && ctrMkNode("area",opt,-1,"/val",_("Values"),R_R___,"root",SARH_ID)) {
	    ctrMkNode("fld",opt,-1,"/val/tm",_("Time"),RWRW__,"root",SARH_ID,1,"tp","time");
	    ctrMkNode("fld",opt,-1,"/val/utm","",RWRW__,"root",SARH_ID,5,"tp","dec","len","6","min","0","max","999999","help",_("Microseconds"));
	    ctrMkNode("fld",opt,-1,"/val/size",_("Size (s)"),RWRW__,"root",SARH_ID,1,"tp","real");
	    ctrMkNode("fld",opt,-1,"/val/arch",_("Archivator"),RWRW__,"root",SARH_ID,4,"tp","str","dest","select","select","/val/lstAVal",
		"help",_("Values archivator.\nNo set archivator for process by buffer and all archivators.\nSet '<buffer>' for process by buffer."));
	    ctrMkNode("fld",opt,-1,"/val/sw_trend",_("Show trend"),RWRW__,"root",SARH_ID,1,"tp","bool");
	    if(!s2i(TBDS::genDBGet(owner().nodePath()+"vShowTrnd","0",opt->attr("user")))) {
		if(ctrMkNode("table",opt,-1,"/val/val",_("Values table"),R_R___,"root",SARH_ID)) {
		    ctrMkNode("list",opt,-1,"/val/val/0",_("Time"),R_R___,"root",SARH_ID,1,"tp","time");
		    ctrMkNode("list",opt,-1,"/val/val/0a",_("mcsec"),R_R___,"root",SARH_ID,1,"tp","dec");
		    ctrMkNode("list",opt,-1,"/val/val/1",_("Value"),R_R___,"root",SARH_ID,1,"tp","str");
		}
	    }
	    else {
		ctrMkNode("fld",opt,-1,"/val/pct_w",_("Picture size"),RWRW__,"root",SARH_ID,4,"tp","dec","min","100","max","1024","help",_("Picture width in pixels."));
		ctrMkNode("fld",opt,-1,"/val/pct_h","",RWRW__,"root",SARH_ID,4,"tp","dec","min","50","max","800","help",_("Picture height in pixels."));
		ctrMkNode("fld",opt,-1,"/val/min",_("Value scale"),RWRW__,"root",SARH_ID,2,"tp","dec","help",_("Picture minimum value."));
		ctrMkNode("fld",opt,-1,"/val/max","",RWRW__,"root",SARH_ID,2,"tp","dec","help",_("Picture maximum value."));
		ctrMkNode("img",opt,-1,"/val/trend",_("Values trend"),R_R___,"root",SARH_ID);
	    }
	}
	return;
    }
    // Process command to page
    if(a_path == "/prm/st/st") {
	if(ctrChkNode(opt,"get",RWRWR_,"root",SARH_ID,SEC_RD))	opt->setText(startStat() ? "1" : "0");
	if(ctrChkNode(opt,"set",RWRWR_,"root",SARH_ID,SEC_WR))	s2i(opt->text()) ? start() : stop();
    }
    if(a_path == "/prm/st/bEnd" && ctrChkNode(opt))
	opt->setText(TSYS::time2str(end(BUF_ARCH_NM)/1000000,"%d-%m-%Y %H:%M:%S.")+i2s(end(BUF_ARCH_NM)%1000000));
    if(a_path == "/prm/st/bBeg" && ctrChkNode(opt))
	opt->setText(TSYS::time2str(begin(BUF_ARCH_NM)/1000000,"%d-%m-%Y %H:%M:%S.")+i2s(begin(BUF_ARCH_NM)%1000000));
    else if(a_path == "/prm/st/db") {
	if(ctrChkNode(opt,"get",RWRWR_,"root",SARH_ID,SEC_RD))	opt->setText(DB());
	if(ctrChkNode(opt,"set",RWRWR_,"root",SARH_ID,SEC_WR))	setDB(opt->text());
    }
    else if(a_path == "/prm/cfg/Source") {
	if(ctrChkNode(opt,"get",RWRWR_,"root",SARH_ID,SEC_RD))	opt->setText(srcData()+(srcPAttr(true).freeStat()?"":" (+)"));
	if(ctrChkNode(opt,"set",RWRWR_,"root",SARH_ID,SEC_WR))	mSrcMode = TSYS::strParse(opt->text(),0," ");
    }
    else if(a_path == "/cfg/prm_atr_ls" && ctrChkNode(opt)) SYS->daq().at().ctrListPrmAttr(opt, srcData(), false, '.');
    else if(a_path.compare(0,8,"/prm/cfg") == 0) TConfig::cntrCmdProc(opt,TSYS::pathLev(a_path,2),"root",SARH_ID,RWRWR_);
    else if(a_path == "/arch/arch") {
	if(ctrChkNode(opt,"get",RWRWR_,"root",SARH_ID,SEC_RD)) {
	    //Fill Archivators table
	    XMLNode *n_arch = ctrMkNode("list",opt,-1,"/arch/arch/arch","",R_R_R_,"root",SARH_ID);
	    XMLNode *n_start= ctrMkNode("list",opt,-1,"/arch/arch/start","",R_R_R_,"root",SARH_ID);
	    XMLNode *n_prc  = ctrMkNode("list",opt,-1,"/arch/arch/proc","",RWRWR_,"root",SARH_ID);
	    XMLNode *n_per  = ctrMkNode("list",opt,-1,"/arch/arch/per","",R_R_R_,"root",SARH_ID);
	    XMLNode *n_beg  = ctrMkNode("list",opt,-1,"/arch/arch/beg","",R_R_R_,"root",SARH_ID);
	    XMLNode *n_end  = ctrMkNode("list",opt,-1,"/arch/arch/end","",R_R_R_,"root",SARH_ID);

	    vector<string> t_arch_ls, arch_ls;
	    owner().modList(t_arch_ls);
	    for(unsigned i_ta = 0; i_ta < t_arch_ls.size(); i_ta++) {
		owner().at(t_arch_ls[i_ta]).at().valList(arch_ls);
		for(unsigned i_a = 0; i_a < arch_ls.size(); i_a++) {
		    TVArchEl *a_el = NULL;
		    //Find attached
		    ResAlloc res(aRes, false);
		    for(unsigned i_l = 0; i_l < arch_el.size(); i_l++)
			if(arch_el[i_l]->archivator().owner().modId() == t_arch_ls[i_ta] && arch_el[i_l]->archivator().id() == arch_ls[i_a])
			    a_el = arch_el[i_l];
		    //Fill table element
		    if(n_arch)	n_arch->childAdd("el")->setText(owner().at(t_arch_ls[i_ta]).at().valAt(arch_ls[i_a]).at().workId());
		    if(n_start)	n_start->childAdd("el")->setText(owner().at(t_arch_ls[i_ta]).at().valAt(arch_ls[i_a]).at().startStat()?"1":"0");
		    if(n_per)	n_per->childAdd("el")->setText(r2s(owner().at(t_arch_ls[i_ta]).at().valAt(arch_ls[i_a]).at().valPeriod(),6));
		    if(a_el) {
			if(n_prc) n_prc->childAdd("el")->setText("1");
			if(n_end)
			    n_end->childAdd("el")->setText(TSYS::time2str(a_el->end()/1000000,"%d-%m-%Y %H:%M:%S.")+i2s(a_el->end()%1000000));
			if(n_beg)
			    n_beg->childAdd("el")->setText(TSYS::time2str(a_el->begin()/1000000,"%d-%m-%Y %H:%M:%S.")+i2s(a_el->begin()%1000000));
		    }
		    else {
			if(n_prc) n_prc->childAdd("el")->setText("0");
			if(n_end) n_end->childAdd("el")->setText("");
			if(n_beg) n_beg->childAdd("el")->setText("");
		    }
		}
	    }
	}
	if(ctrChkNode(opt,"set",RWRWR_,"root",SARH_ID,SEC_WR)) {
	    if(opt->attr("col") == "proc") {
		s2i(opt->text()) ? archivatorAttach(opt->attr("key_arch")) : archivatorDetach(opt->attr("key_arch"), true);
		modif();
	    }
	}
    }
    else if(a_path == "/val/tm") {
	if(ctrChkNode(opt,"get",RWRW__,"root",SARH_ID,SEC_RD)) {
	    opt->setText(TBDS::genDBGet(owner().nodePath()+"vaTm","0",opt->attr("user")));
	    if(!s2i(opt->text())) opt->setText(i2s(time(NULL)));
	}
	if(ctrChkNode(opt,"set",RWRW__,"root",SARH_ID,SEC_WR))
	    TBDS::genDBSet(owner().nodePath()+"vaTm",(s2i(opt->text())>=time(NULL))?"0":opt->text(),opt->attr("user"));
    }
    else if(a_path == "/val/utm") {
	if(ctrChkNode(opt,"get",RWRW__,"root",SARH_ID,SEC_RD))	opt->setText(TBDS::genDBGet(owner().nodePath()+"vaTm_u","0",opt->attr("user")));
	if(ctrChkNode(opt,"set",RWRW__,"root",SARH_ID,SEC_WR))	TBDS::genDBSet(owner().nodePath()+"vaTm_u",opt->text(),opt->attr("user"));
    }
    else if(a_path == "/val/size") {
	if(ctrChkNode(opt,"get",RWRW__,"root",SARH_ID,SEC_RD))	opt->setText(TBDS::genDBGet(owner().nodePath()+"vaSize","1",opt->attr("user")));
	if(ctrChkNode(opt,"set",RWRW__,"root",SARH_ID,SEC_WR))	TBDS::genDBSet(owner().nodePath()+"vaSize",opt->text(),opt->attr("user"));
    }
    else if(a_path == "/val/arch") {
	if(ctrChkNode(opt,"get",RWRW__,"root",SARH_ID,SEC_RD))	opt->setText(TBDS::genDBGet(owner().nodePath()+"vArch","",opt->attr("user")));
	if(ctrChkNode(opt,"set",RWRW__,"root",SARH_ID,SEC_WR))	TBDS::genDBSet(owner().nodePath()+"vArch",opt->text(),opt->attr("user"));
    }
    else if(a_path == "/val/sw_trend") {
	if(ctrChkNode(opt,"get",RWRW__,"root",SARH_ID,SEC_RD))	opt->setText(TBDS::genDBGet(owner().nodePath()+"vShowTrnd","0",opt->attr("user")));
	if(ctrChkNode(opt,"set",RWRW__,"root",SARH_ID,SEC_WR))	TBDS::genDBSet(owner().nodePath()+"vShowTrnd",opt->text(),opt->attr("user"));
    }
    else if(a_path == "/val/pct_w") {
	if(ctrChkNode(opt,"get",RWRW__,"root",SARH_ID,SEC_RD))	opt->setText(TBDS::genDBGet(owner().nodePath()+"vPctW","650",opt->attr("user")));
	if(ctrChkNode(opt,"set",RWRW__,"root",SARH_ID,SEC_WR))	TBDS::genDBSet(owner().nodePath()+"vPctW",opt->text(),opt->attr("user"));
    }
    else if(a_path == "/val/pct_h") {
	if(ctrChkNode(opt,"get",RWRW__,"root",SARH_ID,SEC_RD))	opt->setText(TBDS::genDBGet(owner().nodePath()+"vPctH","230",opt->attr("user")));
	if(ctrChkNode(opt,"set",RWRW__,"root",SARH_ID,SEC_WR))	TBDS::genDBSet(owner().nodePath()+"vPctH",opt->text(),opt->attr("user"));
    }
    else if(a_path == "/val/max") {
	if(ctrChkNode(opt,"get",RWRW__,"root",SARH_ID,SEC_RD))	opt->setText(TBDS::genDBGet(owner().nodePath()+"vMax","0",opt->attr("user")));
	if(ctrChkNode(opt,"set",RWRW__,"root",SARH_ID,SEC_WR))	TBDS::genDBSet(owner().nodePath()+"vMax",opt->text(),opt->attr("user"));
    }
    else if(a_path == "/val/min") {
	if(ctrChkNode(opt,"get",RWRW__,"root",SARH_ID,SEC_RD))	opt->setText(TBDS::genDBGet(owner().nodePath()+"vMin","0",opt->attr("user")));
	if(ctrChkNode(opt,"set",RWRW__,"root",SARH_ID,SEC_WR))	TBDS::genDBSet(owner().nodePath()+"vMin",opt->text(),opt->attr("user"));
    }
    else if(a_path == "/val/lstAVal" && ctrChkNode(opt,"get",R_R___)) {
	opt->childAdd("el")->setText("");
	opt->childAdd("el")->setText(BUF_ARCH_NM);
	vector<string> lsm, lsa;
	owner().modList(lsm);
	for(unsigned i_m = 0; i_m < lsm.size(); i_m++) {
	    owner().at(lsm[i_m]).at().valList(lsa);
	    for(unsigned i_a = 0; i_a < lsa.size(); i_a++)
		opt->childAdd("el")->setText(lsm[i_m]+"."+lsa[i_a]);
	}
    }
    else if(a_path == "/val/val" && ctrChkNode(opt,"get",R_R___,"root",SARH_ID,SEC_RD)) {
	int64_t end = (int64_t)s2i(TBDS::genDBGet(owner().nodePath()+"vaTm","0",opt->attr("user")))*1000000;
	if(!(end/1000000)) end = (int64_t)time(NULL) * 1000000;
	end += s2i(TBDS::genDBGet(owner().nodePath()+"vaTm_u","0",opt->attr("user")));
	int64_t beg = end - (int64_t)(s2r(TBDS::genDBGet(owner().nodePath()+"vaSize","1",opt->attr("user")))*1e6);

	TValBuf buf(TFld::String, 0, 0, false, true);
	getVals(buf, beg, end, TBDS::genDBGet(owner().nodePath()+"vArch","",opt->attr("user")), 2000);

	XMLNode *n_tm   = ctrMkNode("list",opt,-1,"/val/val/0","",0440);
	XMLNode *n_utm	= ctrMkNode("list",opt,-1,"/val/val/0a","",0440);
	XMLNode *n_val  = ctrMkNode("list",opt,-1,"/val/val/1","",0440);

	int64_t c_tm = buf.begin();
	if(buf.end() && buf.begin())
	    while(c_tm <= buf.end()) {
	        string val = buf.getS(&c_tm,true);
		if(n_tm) n_tm->childIns(0,"el")->setText(i2s(c_tm/1000000));
		if(n_tm) n_utm->childIns(0,"el")->setText(i2s(c_tm%1000000));
		if(n_val)n_val->childIns(0,"el")->setText(val);
		c_tm++;
	    }
    }
    else if(a_path == "/val/trend" && ctrChkNode(opt,"get",R_R_R_,"root",SARH_ID,SEC_RD)) {
	int vPctW = vmin(1024,vmax(100,s2i(TBDS::genDBGet(owner().nodePath()+"vPctW","650",opt->attr("user")))));
	int vPctH = vmin(800,vmax(50,s2i(TBDS::genDBGet(owner().nodePath()+"vPctH","230",opt->attr("user")))));
	double vMax = s2r(TBDS::genDBGet(owner().nodePath()+"vMax","0",opt->attr("user")));
	double vMin = s2r(TBDS::genDBGet(owner().nodePath()+"vMin","0",opt->attr("user")));
	int64_t end = (int64_t) s2i(TBDS::genDBGet(owner().nodePath()+"vaTm",i2s(time(NULL)),opt->attr("user")))*1000000+
				s2i(TBDS::genDBGet(owner().nodePath()+"vaTm_u","0",opt->attr("user")));
	if( !(end/1000000) )	end = (int64_t)time(NULL) * 1000000;
	int64_t beg = end - (int64_t)(s2r(TBDS::genDBGet(owner().nodePath()+"vaSize","1",opt->attr("user")))*1e6);

	string tp = "png";
	opt->setText(TSYS::strEncode(makeTrendImg(beg,end,TBDS::genDBGet(owner().nodePath()+"vArch","",opt->attr("user")),vPctW,vPctH,vMax,vMin,&tp),TSYS::base64));
	opt->setAttr("tp", tp);
    }
    else TCntrNode::cntrCmdProc(opt);
}

//*************************************************
//* TVArchivator                                  *
//*************************************************
TVArchivator::TVArchivator( const string &iid, const string &idb, TElem *cf_el ) : TConfig(cf_el), runSt(false), endrunReq(false),
    mId(cfg("ID")), mVPer(cfg("V_PER")), mAPer(cfg("A_PER")), mStart(cfg("START").getBd()), mSelPrior(cfg("SEL_PR").getId()), mDB(idb), tm_calc(0)
{
    mId = iid;
}

TVArchivator::~TVArchivator( )
{

}

TCntrNode &TVArchivator::operator=( TCntrNode &node )
{
    TVArchivator *src_n = dynamic_cast<TVArchivator*>(&node);
    if(!src_n) return *this;

    //Configuration copy
    exclCopy(*src_n, "ID;");
    mDB = src_n->mDB;

    if(src_n->startStat() && toStart() && !startStat()) start();

    return *this;
}

string TVArchivator::name( )
{
    string rez = cfg("NAME").getS();
    return rez.size() ? rez : mId;
}

string TVArchivator::tbl( )	{ return owner().owner().subId()+"_val_proc"; }

void TVArchivator::postEnable( int flag )	{ cfg("MODUL").setS(owner().modId()); }

void TVArchivator::preDisable( int flag )	{ if(startStat()) stop(flag); }

void TVArchivator::postDisable( int flag )
{
    if(flag) SYS->db().at().dataDel(fullDB(), SYS->archive().at().nodePath()+tbl(), *this, true);
}

bool TVArchivator::cfgChange( TCfg &co, const TVariant &pc )
{
    if(co.name() == "V_PER") {
	if(!co.getR()) mVPer = 1;
	else {
	    //Call sort for all archives
	    ResAlloc res(archRes, false);
	    for(map<string,TVArchEl*>::iterator iel = archEl.begin(); iel != archEl.end(); ++iel)
		iel->second->archive().archivatorSort();
	}
    }
    else if(co.name() == "A_PER" && co.getB() != pc.getB() && startStat()) stop();

    modif();
    return true;
}

string TVArchivator::workId( )	{ return owner().modId()+"."+id(); }

void TVArchivator::start( )
{
    //Start archivator thread
    if(archPeriod() && !runSt) SYS->taskCreate(nodePath('.',true), 0, TVArchivator::Task, this, 2, NULL, &runSt);

    owner().owner().setToUpdate();
    runSt = true;
}

void TVArchivator::stop( bool full_del )
{
    //Values acquisition task stop
    if(runSt && archPeriod()) SYS->taskDestroy(nodePath('.',true), &endrunReq, archPeriod());
    runSt = false;

    //Detach from all archives
    ResAlloc res(archRes, false);
    while(archEl.size()) {
	AutoHD<TVArchive> arch(&archEl.begin()->second->archive());
	res.release();
	arch.at().archivatorDetach(workId(), full_del, false);
	res.request(false);
    }
}

void TVArchivator::archiveList( vector<string> &ls )
{
    ResAlloc res(archRes, false);
    ls.clear();
    for(map<string,TVArchEl*>::iterator iel = archEl.begin(); iel != archEl.end(); ++iel)
	ls.push_back(iel->first);
}

bool TVArchivator::archivePresent( const string &iid )
{
    ResAlloc res(archRes, false);
    return (archEl.find(iid) != archEl.end());
}

TVArchEl *TVArchivator::archivePlace( TVArchive &item )
{
    ResAlloc res(archRes, true);
    map<string,TVArchEl*>::iterator iel = archEl.find(item.id());
    if(iel != archEl.end()) return iel->second;
    TVArchEl *it_el = getArchEl(item);
    archEl[item.id()] = it_el;

    return it_el;
}

void TVArchivator::archiveRemove( const string &iid, bool full )
{
    ResAlloc res(archRes, true);
    map<string,TVArchEl*>::iterator iel = archEl.find(iid);
    if(iel != archEl.end()) {
	if(full) iel->second->fullErase();
	delete iel->second;
	archEl.erase(iel);
    }
}

TVArchEl *TVArchivator::getArchEl( TVArchive &arch )	{ return new TVArchEl(arch, *this); }

TTipArchivator &TVArchivator::owner( )			{ return *(TTipArchivator *)nodePrev(); }

void TVArchivator::load_( )
{
    if(!SYS->chkSelDB(DB())) throw TError();
    SYS->db().at().dataGet(fullDB(), SYS->archive().at().nodePath()+tbl(), *this);
}

void TVArchivator::save_( )
{
    SYS->db().at().dataSet(fullDB(), SYS->archive().at().nodePath()+tbl(), *this);
}

void *TVArchivator::Task( void *param )
{
    TVArchivator &arch = *(TVArchivator*)param;
    arch.endrunReq = false;
    arch.runSt = true;
    bool isLast = false;

    //time_t stTm = time(NULL);
    //while(!arch.endrunReq && (time(NULL)-stTm) < arch.archPeriod()) TSYS::sysSleep(STD_WAIT_DELAY*1e-3);
    TSYS::sysSleep(arch.archPeriod());

    //Archiving
    while(true)
	try {
	    //stTm = time(NULL);
	    if(arch.endrunReq) isLast = true;

	    int64_t t_cnt = TSYS::curTime();

	    ResAlloc res(arch.archRes, false);
	    int64_t beg, end;
	    for(map<string,TVArchEl*>::iterator iel = arch.archEl.begin(); iel != arch.archEl.end(); ++iel)
		if(iel->second->archive().startStat()) {
		    TVArchEl *arch_el = iel->second;
		    beg = vmax(arch_el->mLastGet,arch_el->archive().begin());
		    end = arch_el->archive().end();
		    if(!beg || !end || beg > end) continue;
		    arch_el->setVals(arch_el->archive(), beg, end);
		}
	    res.release();

	    arch.tm_calc = TSYS::curTime()-t_cnt;

	    if(isLast) break;

	    //while(!arch.endrunReq && (time(NULL)-stTm) < arch.archPeriod()) TSYS::sysSleep(STD_WAIT_DELAY*1e-3);
	    TSYS::taskSleep((int64_t)(1e9*arch.archPeriod()));
	}
	catch(TError err) { mess_err(err.cat.c_str(),"%s",err.mess.c_str() ); }

    arch.runSt = false;

    return NULL;
}

TVariant TVArchivator::objFuncCall( const string &iid, vector<TVariant> &prms, const string &user )
{
    //Configuration functions call
    TVariant cfRez = objFunc(iid, prms, user);
    if(!cfRez.isNull()) return cfRez;

    return TCntrNode::objFuncCall(iid, prms, user);
}

void TVArchivator::cntrCmdProc( XMLNode *opt )
{
    //Get page info
    if(opt->name() == "info") {
	TCntrNode::cntrCmdProc(opt);
	ctrMkNode("oscada_cntr",opt,-1,"/",_("Value archivator: ")+name(),RWRWR_,"root",SARH_ID);
	if(ctrMkNode("area",opt,-1,"/prm",_("Archivator"))) {
	    if(ctrMkNode("area",opt,-1,"/prm/st",_("State"))) {
		ctrMkNode("fld",opt,-1,"/prm/st/st",_("Running"),RWRWR_,"root",SARH_ID,1,"tp","bool");
		if(archPeriod()) ctrMkNode("fld",opt,-1,"/prm/st/tarch",_("Archiving time"),R_R_R_,"root",SARH_ID,1,"tp","str");
		ctrMkNode("fld",opt,-1,"/prm/st/db",_("Archivator DB"),RWRWR_,"root","root",4,
		    "tp","str","dest","select","select","/db/list","help",TMess::labDB());
	    }
	    if(ctrMkNode("area",opt,-1,"/prm/cfg",_("Configuration"))) {
		TConfig::cntrCmdMake(opt,"/prm/cfg",0,"root",SARH_ID,RWRWR_);
		ctrMkNode("fld",opt,-1,"/prm/cfg/A_PER",EVAL_STR,RWRWR_,"root",SARH_ID,1,
		    "help",_("Set zero period for disable using the archiver for buffer processing, only for direct write."));
		ctrMkNode("fld",opt,-1,"/prm/cfg/SEL_PR",EVAL_STR,RWRWR_,"root",SARH_ID,1,
		    "help",_("Selection the archiver priority for \"All\" sources using mode. Set to zero for full the selection disable."));
		ctrRemoveNode(opt,"/prm/cfg/MODUL");
	    }
	}
	if(ctrMkNode("area",opt,-1,"/arch",_("Archives")))
	    if(ctrMkNode("table",opt,-1,"/arch/arch",_("Archives"),R_R_R_,"root",SARH_ID)) {
		ctrMkNode("list",opt,-1,"/arch/arch/0",_("Archive"),R_R_R_,"root",SARH_ID,1,"tp","str");
		ctrMkNode("list",opt,-1,"/arch/arch/1",_("Period (s)"),R_R_R_,"root",SARH_ID,1,"tp","real");
		ctrMkNode("list",opt,-1,"/arch/arch/2",_("Buffer size"),R_R_R_,"root",SARH_ID,1,"tp","dec");
	    }
	return;
    }

    //Process command to page
    string a_path = opt->attr("path");
    if(a_path == "/prm/st/st") {
	if(ctrChkNode(opt,"get",RWRWR_,"root",SARH_ID,SEC_RD))	opt->setText(startStat() ? "1" : "0");
	if(ctrChkNode(opt,"set",RWRWR_,"root",SARH_ID,SEC_WR))	s2i(opt->text()) ? start() : stop();
    }
    else if(a_path == "/prm/st/tarch" && ctrChkNode(opt))	opt->setText(TSYS::time2str(tm_calc));
    else if(a_path == "/prm/st/db") {
	if(ctrChkNode(opt,"get",RWRWR_,"root",SARH_ID,SEC_RD))	opt->setText(DB());
	if(ctrChkNode(opt,"set",RWRWR_,"root",SARH_ID,SEC_WR))	setDB(opt->text());
    }
    else if(a_path.compare(0,8,"/prm/cfg") == 0) TConfig::cntrCmdProc(opt,TSYS::pathLev(a_path,2),"root",SARH_ID,RWRWR_);
    else if(a_path == "/arch/arch" && ctrChkNode(opt)) {
	// Fill Archives table
	XMLNode *n_arch = ctrMkNode("list",opt,-1,"/arch/arch/0","");
	XMLNode *n_per  = ctrMkNode("list",opt,-1,"/arch/arch/1","");
	XMLNode *n_size = ctrMkNode("list",opt,-1,"/arch/arch/2","");

	ResAlloc res(archRes,false);
	for(map<string,TVArchEl*>::iterator iel = archEl.begin(); iel != archEl.end(); ++iel)
	{
	    if(n_arch)	n_arch->childAdd("el")->setText(iel->second->archive().id());
	    if(n_per)	n_per->childAdd("el")->setText(r2s((double)iel->second->archive().period()/1000000.,6));
	    if(n_size)	n_size->childAdd("el")->setText(i2s(iel->second->archive().size()));
	}
    }
    else TCntrNode::cntrCmdProc(opt);
}

//*************************************************
//* TVArchEl                                      *
//*************************************************
TVArchEl::TVArchEl( TVArchive &iarchive, TVArchivator &iarchivator ) :
    prev_tm(0), mArchive(iarchive), mArchivator(iarchivator), mLastGet(0)	{ }

TVArchEl::~TVArchEl( )	{ }

TVArchive &TVArchEl::archive( )		{ return mArchive; }

TVArchivator &TVArchEl::archivator( )	{ return mArchivator; }

TVariant TVArchEl::getVal( int64_t *tm, bool up_ord, bool onlyLocal )
{
    TVariant vl = getValProc(tm,up_ord);

    if(!onlyLocal && tm && archive().startStat() && vl.isEVal() && SYS->daq().at().rdActive() &&
	(archive().srcMode() == TVArchive::ActiveAttr || archive().srcMode() == TVArchive::PassiveAttr))
    {
	int64_t remTm = 0;
	string lstStat;
	AutoHD<TVal> paVl = archive().srcPAttr();
	AutoHD<TParamContr> sPrm(dynamic_cast<TParamContr*>(&paVl.at().owner()));
	if(sPrm.at().owner().owner().redntAllow() && sPrm.at().owner().redntMode() != TController::Off)
	{
	    XMLNode req("get");
	    req.clear()->setAttr("local","1")->
		setAttr("path", paVl.at().nodePath(0,true)+"/%2fserv%2fval")->
		setAttr("tm", ll2s(*tm))->
		setAttr("arch", archivator().workId());
	    reqCall:
	    lstStat = SYS->daq().at().rdStRequest(sPrm.at().owner().workId(),req,lstStat,false);
	    if(!lstStat.empty() && !s2i(req.attr("rez"))) {
		remTm = s2ll(req.attr("tm"));
		vl.setS(req.text());
		if(vl.getS() == EVAL_STR) goto reqCall;
		*tm = remTm;
		//Put to local archive
		TValBuf buf(archive().valType(), 0, 0, false, true);
		buf.setS(vl.getS(), remTm);
		setVals(buf);
	    }else if(!lstStat.empty()) goto reqCall;
	}
    }

    return vl;
}

TVariant TVArchEl::getValProc( int64_t *tm, bool up_ord )
{
    if(tm) *tm = 0;
    return TVariant();
}

void TVArchEl::getVals( TValBuf &buf, int64_t ibeg, int64_t iend, bool onlyLocal )
{
    //Get local archive data
    unsigned int ecnt = buf.evalCnt();
    getValsProc(buf, ibeg, iend);

    //Check for holes fill
    // Check for redundant allow
    if(!onlyLocal && archive().startStat() && buf.evalCnt() > ecnt && SYS->daq().at().rdActive() &&
	(archive().srcMode( ) == TVArchive::ActiveAttr || archive().srcMode() == TVArchive::PassiveAttr))
    {
	AutoHD<TVal> paVl = archive().srcPAttr();
	AutoHD<TParamContr> sPrm(dynamic_cast<TParamContr*>(&paVl.at().owner()));
	if(sPrm.at().owner().owner().redntAllow() && sPrm.at().owner().redntMode() != TController::Off)
	{
	    // Holes process
	    int64_t firstEval = 0, curEval = 0;
	    int64_t cbeg = buf.begin();
	    iend = buf.end();
	    string lstStat;
	    XMLNode req("get");
	    do {
		string curVal = buf.getS(&cbeg, true);
		if(curVal == EVAL_STR) {
		    curEval = cbeg;
		    if(!firstEval) firstEval = curEval;
		}
		cbeg++;
		// Request to remote station for data
		if((curVal != EVAL_STR || cbeg > iend) && firstEval) {
		    reqCall:
		    req.clear()->setAttr("local", "1")->
			setAttr("path", paVl.at().nodePath(0,true)+"/%2fserv%2fval")->
			setAttr("tm_grnd", ll2s(firstEval))->
			setAttr("tm", ll2s(curEval))->
			setAttr("arch",archivator().workId())->
			setAttr("mode","1");

		    lstStat = SYS->daq().at().rdStRequest(sPrm.at().owner().workId(), req, lstStat, false);
		    bool evalOk = false, noEvalOk = false;
		    if(!lstStat.empty() && !s2i(req.attr("rez")) && s2ll(req.attr("tm_grnd")) && s2ll(req.attr("tm")))
		    {
			// Get values and put to buffer
			int64_t bbeg = s2ll(req.attr("tm_grnd"));
			int64_t bend = s2ll(req.attr("tm"));
			int64_t bper = s2ll(req.attr("per"));
			int val_tp = s2i(req.attr("vtp"));
			int prevPos = 0, curPos;
			string prevVal = EVAL_STR, curVal;

			for(int v_off = 0, var_off = 0; true; ) {
			    string svl = TSYS::strSepParse(req.text(),0,'\n',&v_off);
			    if(svl.size()) {
				var_off = 0;
				curPos = s2i(TSYS::strParse(svl,0," ",&var_off,true));
				curVal = TSYS::strParse(svl, 0, " ", &var_off, true);
			    }
			    else curPos = ((bend-bbeg)/bper)+1;
			    switch(val_tp)
			    {
				case TFld::Boolean:
				{
				    char vl = s2i(prevVal);
				    if(prevVal == EVAL_STR || vl == EVAL_BOOL) { evalOk = true; prevPos = curPos; }
				    else { noEvalOk = true; for( ; prevPos < curPos; prevPos++ ) buf.setB(vl, bbeg+prevPos*bper); }
				    break;
				}
				case TFld::Integer:
				{
				    int64_t vl = s2ll(prevVal);
				    if(prevVal == EVAL_STR || vl == EVAL_INT) { evalOk = true; prevPos = curPos; }
				    else { noEvalOk = true; for( ; prevPos < curPos; prevPos++ ) buf.setI(vl, bbeg+prevPos*bper); }
				    break;
				}
				case TFld::Real:
				{
				    double vl = s2r(prevVal);
				    if(prevVal == EVAL_STR || vl == EVAL_REAL) { evalOk = true; prevPos = curPos; }
				    else { noEvalOk = true; for( ; prevPos < curPos; prevPos++ ) buf.setR(vl, bbeg+prevPos*bper); }
				    break;
				}
				case TFld::String:
				{
				    if(prevVal == EVAL_STR) { evalOk = true; prevPos = curPos; }
				    else { noEvalOk = true; for( ; prevPos < curPos; prevPos++ ) buf.setS(prevVal, bbeg+prevPos*bper); }
				    break;
				}
			    }
			    prevVal = curVal;
			    if(prevPos > (bend-bbeg)/bper) break;
			}

			//Put buffer part to archive
			if(noEvalOk)	setVals(buf, firstEval, curEval);
		    }
		    if(!lstStat.empty() && evalOk) goto reqCall;
		    lstStat = "";
		    firstEval = 0;
		}
	    }while(cbeg <= iend);
	}
    }
}

void TVArchEl::setVals( TValBuf &ibuf, int64_t beg, int64_t end )
{
    int64_t a_per = (int64_t)(1e6*archivator().valPeriod());

    if(!beg || !end) { beg = ibuf.begin(); end = ibuf.end(); }
    beg = vmax((beg/a_per)*a_per, ibuf.begin()); end = vmin(end, ibuf.end());

    if(!beg || !end || beg/a_per > end/a_per) return;

    //Check for put to buffer
    if(&archive() != &ibuf && mLastGet && end > mLastGet && ((end-mLastGet)/archive().period()) < archive().size())
    { archive().TValBuf::setVals(ibuf,vmax(archive().end(),beg),end); return; }

    //Put direct to archive
    int64_t wPrevTm = 0, s_k, n_k;
    string wPrevVal;

    if(&archive() == &ibuf || end > archive().end()) { wPrevTm = prev_tm; wPrevVal = prev_val; }

    //Combining
    TVArchive::CombMode combM = archive().combMode();
    bool setOK = false;
    if(a_per > ibuf.period()) {
	TValBuf obuf(ibuf.valType(), 0, a_per, true, true);
	for(int64_t c_tm = beg; c_tm <= end; ) {
	    switch(ibuf.valType())
	    {
		case TFld::Boolean: {
		    float c_val = ibuf.getB(&c_tm, true);
		    if(combM == TVArchive::LastVal) { obuf.setB((char)c_val,c_tm); c_tm += a_per; break; }

		    int vdif = c_tm/a_per - wPrevTm/a_per;
		    if(!vdif) {
			float v_o = *(float*)wPrevVal.c_str();
			if(c_val == EVAL_BOOL) c_val = v_o;
			if(c_val != EVAL_BOOL && v_o != EVAL_BOOL)
			    switch(combM)
			    {
				case TVArchive::MovAver:
				    s_k = c_tm-a_per*(c_tm/a_per);
				    n_k = ibuf.period();
				    c_val = (v_o*s_k+c_val*n_k)/(s_k+n_k);
				    break;
				case TVArchive::MinVal:	c_val = vmin(c_val,v_o); break;
				case TVArchive::MaxVal: c_val = vmax(c_val,v_o); break;
				default: break;
			    }
			wPrevVal.assign((char*)&c_val,sizeof(float));
			wPrevTm = c_tm;
		    }
		    if(vdif == 1 || (c_tm+1) > end) {
			float t_vl = *(float*)wPrevVal.c_str();
			obuf.setB((t_vl!=EVAL_BOOL) ? t_vl > 0.5 : t_vl, wPrevTm);
		    }
		    if(vdif) {
			wPrevVal.assign((char*)&c_val,sizeof(float));
			wPrevTm = c_tm;
		    }
		    c_tm++;
		    break;
		}
		case TFld::String: {
		    string c_val = ibuf.getS(&c_tm,true);
		    obuf.setS(c_val,c_tm);
		    c_tm += a_per;
		    break;
		}
		case TFld::Integer: {
		    int64_t c_val = ibuf.getI(&c_tm, true);
		    if(combM == TVArchive::LastVal) { obuf.setI(c_val,c_tm); c_tm += a_per; break; }

		    int vdif = c_tm/a_per - wPrevTm/a_per;
		    if(!vdif) {
			int64_t v_o = *(int64_t*)wPrevVal.c_str();
			if(c_val == EVAL_INT) c_val = v_o;
			if(c_val != EVAL_INT && v_o != EVAL_INT)
			    switch(combM)
			    {
				case TVArchive::MovAver:
				    s_k = c_tm-a_per*(c_tm/a_per);
				    n_k = ibuf.period();
				    c_val = ((int64_t)v_o*s_k+(int64_t)c_val*n_k)/(s_k+n_k);
				    break;
				case TVArchive::MinVal:	c_val = vmin(c_val,v_o); break;
				case TVArchive::MaxVal: c_val = vmax(c_val,v_o); break;
				default: break;
			    }
			wPrevVal.assign((char*)&c_val,sizeof(int64_t));
			wPrevTm = c_tm;
		    }
		    if(vdif == 1 || (c_tm+1) > end) obuf.setI(*(int*)wPrevVal.c_str(),wPrevTm);
		    if(vdif) {
			wPrevVal.assign((char*)&c_val,sizeof(int64_t));
			wPrevTm = c_tm;
		    }
		    c_tm++;
		    break;
		}
		case TFld::Real: {
		    double c_val = ibuf.getR(&c_tm, true);
		    if(combM == TVArchive::LastVal) { obuf.setR(c_val, c_tm); c_tm += a_per; break; }

		    int vdif = c_tm/a_per - wPrevTm/a_per;
		    if(!vdif) {
			double v_o = *(double*)wPrevVal.c_str();
			if(c_val == EVAL_REAL) c_val = v_o;
			if(c_val != EVAL_REAL && v_o != EVAL_REAL)
			    switch(combM)
			    {
				case TVArchive::MovAver:
				    s_k = c_tm-a_per*(c_tm/a_per);
				    n_k = ibuf.period();
				    c_val = (v_o*s_k+c_val*n_k)/(s_k+n_k);
				    break;
				case TVArchive::MinVal:	c_val = vmin(c_val,v_o); break;
				case TVArchive::MaxVal:	c_val = vmax(c_val,v_o); break;
				default: break;
			    }
			wPrevVal.assign((char*)&c_val,sizeof(double));
			wPrevTm = c_tm;
		    }
		    if(vdif == 1 || (c_tm+1) > end) obuf.setR(*(double*)wPrevVal.c_str(),wPrevTm);
		    if(vdif) {
			wPrevVal.assign((char*)&c_val,sizeof(double));
			wPrevTm = c_tm;
		    }
		    c_tm++;
		    break;
		}
		default: break;
	    }
	}
	setOK = setValsProc(obuf, obuf.begin(), obuf.end()/*end*/);	//Or possible vmax(obuf.end(),end)
    }
    else setOK = setValsProc(ibuf, beg, end);

    if(setOK) {
	if(end > mLastGet) mLastGet = end+1;
	if(&archive() == &ibuf || end > archive().end()) { prev_tm = wPrevTm; prev_val = wPrevVal; }
    }
}
