
//OpenSCADA file: tarchval.cpp
/***************************************************************************
 *   Copyright (C) 2006-2022 by Roman Savochenko, <roman@oscada.org>       *
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

#if HAVE_GD_FORCE
# include <gd.h>
#endif

#define HalfDivMinWin	5

using namespace OSCADA;

//*************************************************
//* Value archiver                                *
//*************************************************

//*************************************************
//* TValBuf                                       *
//*************************************************
TValBuf::TValBuf( ) : mValTp(TFld::Integer), mHgResTm(false), mHrdGrd(false), mFillLast(false), mEnd(0), mBeg(0), mPer(0), mSize(100)
{
    buf.bl = NULL;

    makeBuf(mValTp, mSize, mPer, mHrdGrd, mHgResTm);
}

TValBuf::TValBuf( TFld::Type vtp, int isz, int64_t ipr, bool ihgrd, bool ihres ) :
    mValTp(vtp), mHgResTm(ihres), mHrdGrd(ihgrd), mFillLast(false), mEnd(0), mBeg(0), mPer(vmax(0,ipr)), mSize(isz)
{
    buf.bl = NULL;

    makeBuf(mValTp, mSize, mPer, mHrdGrd, mHgResTm);
}

TValBuf::TValBuf( const TValBuf &src ) : mValTp(TFld::Integer),
    mHgResTm(false), mHrdGrd(false), mFillLast(false), mEnd(0), mBeg(0), mPer(0), mSize(100)
{
    buf.bl = NULL;

    operator=(src);
}

TValBuf::~TValBuf( )
{
    switch(mValTp) {
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

TValBuf &TValBuf::operator=( const TValBuf &src )
{
    makeBuf(src.mValTp, src.mSize, src.mPer, src.mHrdGrd, src.mHgResTm);

    switch(mValTp) {
	case TFld::Boolean:
	    buf.bl->cur = src.buf.bl->cur;
	    if(mHrdGrd)		*(buf.bl->buf.grid) = *(src.buf.bl->buf.grid);
	    else if(mHgResTm)	*(buf.bl->buf.tmHigh) = *(src.buf.bl->buf.tmHigh);
	    else		*(buf.bl->buf.tmLow) = *(src.buf.bl->buf.tmLow);
	    break;
	case TFld::Int16:
	    buf.i16->cur = src.buf.i16->cur;
	    if(mHrdGrd)		*(buf.i16->buf.grid) = *(src.buf.i16->buf.grid);
	    else if(mHgResTm)	*(buf.i16->buf.tmHigh) = *(src.buf.i16->buf.tmHigh);
	    else		*(buf.i16->buf.tmLow) = *(src.buf.i16->buf.tmLow);
	    break;
	case TFld::Int32:
	    buf.i32->cur = src.buf.i32->cur;
	    if(mHrdGrd)		*(buf.i32->buf.grid) = *(src.buf.i32->buf.grid);
	    else if(mHgResTm)	*(buf.i32->buf.tmHigh) = *(src.buf.i32->buf.tmHigh);
	    else		*(buf.i32->buf.tmLow) = *(src.buf.i32->buf.tmLow);
	    break;
	case TFld::Int64:
	    buf.i64->cur = src.buf.i64->cur;
	    if(mHrdGrd)		*(buf.i64->buf.grid) = *(src.buf.i64->buf.grid);
	    else if(mHgResTm)	*(buf.i64->buf.tmHigh) = *(src.buf.i64->buf.tmHigh);
	    else		*(buf.i64->buf.tmLow) = *(src.buf.i64->buf.tmLow);
	    break;
	case TFld::Float:
	    buf.rFlt->cur = src.buf.rFlt->cur;
	    if(mHrdGrd)		*(buf.rFlt->buf.grid) = *(src.buf.rFlt->buf.grid);
	    else if(mHgResTm)	*(buf.rFlt->buf.tmHigh) = *(src.buf.rFlt->buf.tmHigh);
	    else		*(buf.rFlt->buf.tmLow) = *(src.buf.rFlt->buf.tmLow);
	    break;
	case TFld::Double:
	    buf.rDbl->cur = src.buf.rDbl->cur;
	    if(mHrdGrd)		*(buf.rDbl->buf.grid) = *(src.buf.rDbl->buf.grid);
	    else if(mHgResTm)	*(buf.rDbl->buf.tmHigh) = *(src.buf.rDbl->buf.tmHigh);
	    else		*(buf.rDbl->buf.tmLow) = *(src.buf.rDbl->buf.tmLow);
	    break;
	case TFld::String:
	    buf.str->cur = src.buf.str->cur;
	    if(mHrdGrd)		*(buf.str->buf.grid) = *(src.buf.str->buf.grid);
	    else if(mHgResTm)	*(buf.str->buf.tmHigh) = *(src.buf.str->buf.tmHigh);
	    else		*(buf.str->buf.tmLow) = *(src.buf.str->buf.tmLow);
	    break;
	default: break;
    }
    mFillLast = src.mFillLast;
    mEnd = src.mEnd, mBeg = src.mBeg;
    mEvalCnt = src.mEvalCnt;

    return *this;
}

void TValBuf::clear( )
{
    switch(mValTp) {
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
    switch(mValTp) {
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
	switch(mValTp) {
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
	switch(v_tp) {
	    case TFld::Boolean:	buf.bl = new TBuf<char>(EVAL_BOOL, mSize, mPer, mHrdGrd, mHgResTm, mFillLast, mEnd, mBeg, mEvalCnt);	break;
	    case TFld::Int16:	buf.i16 = new TBuf<int16_t>(EVAL_INT16, mSize, mPer, mHrdGrd, mHgResTm, mFillLast, mEnd, mBeg, mEvalCnt);break;
	    case TFld::Int32:	buf.i32 = new TBuf<int32_t>(EVAL_INT32, mSize, mPer, mHrdGrd, mHgResTm, mFillLast, mEnd, mBeg, mEvalCnt);break;
	    case TFld::Int64:	buf.i64 = new TBuf<int64_t>(EVAL_INT64, mSize, mPer, mHrdGrd, mHgResTm, mFillLast, mEnd, mBeg, mEvalCnt);break;
	    case TFld::Float:	buf.rFlt = new TBuf<float>(EVAL_RFlt, mSize, mPer, mHrdGrd, mHgResTm, mFillLast, mEnd, mBeg, mEvalCnt);	break;
	    case TFld::Double:	buf.rDbl = new TBuf<double>(EVAL_RDbl, mSize, mPer, mHrdGrd, mHgResTm, mFillLast, mEnd, mBeg, mEvalCnt);break;
	    case TFld::String:	buf.str = new TBuf<string>(EVAL_STR, mSize, mPer, mHrdGrd, mHgResTm, mFillLast, mEnd, mBeg, mEvalCnt);	break;
	    default: break;
	}
	mValTp = v_tp;
    }
    if(isz != mSize || ipr != mPer || hd_grd != mHrdGrd || hg_res != mHgResTm)
	switch(mValTp) {
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
    switch(valType()) {
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
    switch(valType()) {
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
    switch(valType(true)) {
	case TFld::Boolean: { char vl = getB(itm, up_ord); return (vl==EVAL_BOOL) ? EVAL_REAL : (bool)vl; }
	case TFld::Int16: case TFld::Int32: case TFld::Int64:
			    { int64_t vl = getI(itm, up_ord); return (vl==EVAL_INT) ? EVAL_REAL : (double)vl; }
	case TFld::String:  { string vl = getS(itm, up_ord); return (vl==EVAL_STR) ? EVAL_REAL : s2r(vl); }
	case TFld::Float:   { ResAlloc res(bRes, false); float vl = buf.rFlt->get(itm, up_ord); return (vl/EVAL_RFlt>=0.99999) ? EVAL_REAL : vl; }
	case TFld::Double:  { ResAlloc res(bRes, false); double vl = buf.rDbl->get(itm, up_ord); return (vl/EVAL_RDbl>=0.99999) ? EVAL_REAL : vl; }
	default: break;
    }
    return EVAL_REAL;
}

int64_t TValBuf::getI( int64_t *itm, bool up_ord )
{
    switch(valType(true)) {
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
    switch(valType()) {
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
    switch(valType()) {
	case TFld::Boolean: setB((value==EVAL_STR)?EVAL_BOOL:(bool)s2i(value), tm);	break;
	case TFld::Integer: setI((value==EVAL_STR)?EVAL_INT:s2ll(value), tm);		break;
	case TFld::Real:    setR((value==EVAL_STR)?EVAL_REAL:s2r(value), tm);		break;
	case TFld::String:  { ResAlloc res(bRes, true); buf.str->set(value, tm); }	break;
	default: break;
    }
}

void TValBuf::setR( double value, int64_t tm )
{
    switch(valType(true)) {
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
    switch(valType(true)) {
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
    switch(valType()) {
	case TFld::Integer: setI((value==EVAL_BOOL)?EVAL_INT:(bool)value, tm);		break;
	case TFld::String:  setS((value==EVAL_BOOL)?EVAL_STR:i2s((bool)value), tm);	break;
	case TFld::Real:    setR((value==EVAL_BOOL)?EVAL_REAL:(bool)value, tm);		break;
	case TFld::Boolean: { ResAlloc res(bRes, true); buf.bl->set(value, tm); }	break;
	default: break;
    }
}

void TValBuf::set( const TVariant &value, int64_t tm )
{
    switch(value.type()) {
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
    switch(valType()) {
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
	    for( ; ibeg <= iend; ibeg += t_step) { vl = getR(&ibeg, true); buf.setR(vl, ibeg); }
	    break;
	}
	case TFld::String: {
	    string vl;
	    for( ; ibeg <= iend; ibeg += t_step) { vl = getS(&ibeg, true); buf.setS(vl, ibeg); }
	    break;
	}
	default: break;
    }
}

void TValBuf::setVals( TValBuf &ibuf, int64_t ibeg, int64_t iend )	{ ibuf.getVals(*this, ibeg, iend); }

//*************************************************
//* TValBuf::TBuf                                 *
//*************************************************
template <class TpVal> TValBuf::TBuf<TpVal>::TBuf( TpVal ieval, int &isz, int64_t &ipr,
	bool &ihgrd, bool &ihres, bool &iFillLast, int64_t &iend, int64_t &ibeg, unsigned int &iEvalCnt ) :
    hgResTm(ihres), hrdGrd(ihgrd), fillLast(iFillLast), end(iend), beg(ibeg), per(ipr), size(isz), cur(0), eval(ieval), mEvalCnt(iEvalCnt)
{
    buf.grid = NULL;

    makeBuf(size, per, hrdGrd, hgResTm);
}

template <class TpVal> TValBuf::TBuf<TpVal>::~TBuf( )
{
    if(hrdGrd)		delete buf.grid;
    else if(hgResTm)	delete buf.tmHigh;
    else		delete buf.tmLow;
}

template <class TpVal> void TValBuf::TBuf<TpVal>::clear( )
{
    if(!buf.grid)	return;
    if(hrdGrd)		buf.grid->clear();
    else if(hgResTm)	buf.tmHigh->clear();
    else		buf.tmLow->clear();
    cur = end = beg = 0;
    mEvalCnt = 0;
}

template <class TpVal> int TValBuf::TBuf<TpVal>::realSize( )
{
    if(!buf.grid)	return 0;
    if(hrdGrd)		return buf.grid->size();
    else if(hgResTm)	return buf.tmHigh->size();
    else		return buf.tmLow->size();
}

template <class TpVal> void TValBuf::TBuf<TpVal>::makeBuf( int isz, int64_t ipr, bool ihd_grd, bool ihg_res )
{
    bool recr_buf = false;
    if(!buf.grid)			recr_buf = true;
    if(isz < size || (!size && isz))	recr_buf = true;
    if(hrdGrd != ihd_grd) {
	if(ihd_grd && !ipr)	ipr = 1000000;
	recr_buf = true;
    }
    if(hgResTm != ihg_res)	recr_buf = true;
    if(per != ipr) {
	if(hrdGrd && !ipr)	ipr = 1000000;
	recr_buf = true;
    }

    size = isz;
    hrdGrd = ihd_grd;
    hgResTm = ihg_res;
    per = ipr;

    if(recr_buf) {
	//Destroy buffer
	cur = end = beg = 0;
	if(buf.grid) {
	    if(hrdGrd)		delete buf.grid;
	    else if(hgResTm)	delete buf.tmHigh;
	    else		delete buf.tmLow;
	}
	buf.grid = NULL;

	//Make new buffer
	if(ihd_grd)	buf.grid = new vector<TpVal>;
	else if(ihg_res)buf.tmHigh = new vector<SHg>;
	else		buf.tmLow = new vector<SLw>;
	mEvalCnt = 0;
    }
}

template <class TpVal> TpVal TValBuf::TBuf<TpVal>::get( int64_t *itm, bool up_ord )
{
    int64_t tm = (itm)?(*itm):TSYS::curTime();

    if((up_ord && tm > end) || (!up_ord && tm < beg))	return eval;	//throw TError("ValBuf", _("No value."));

    tm = up_ord ? vmax(tm, beg) : vmin(tm, end);
    //Process hard grid buffer
    if(hrdGrd) {
	int npos = up_ord ? (end-tm)/per : (int64_t)buf.grid->size()-1-(tm-beg)/per;
	if(npos < 0 || npos >= (int)buf.grid->size()) { if(itm) *itm = 0; return eval; }
	if(itm)	*itm = end-npos*per;
	return (*buf.grid)[((cur-npos-1)>=0)?(cur-npos-1):(buf.grid->size()+(cur-npos-1))];
    }
    //Process soft grid buffer
    else if(per) {
	int npos = (up_ord?(end-tm):(tm-beg))/per;
	if(hgResTm) {
	    int c_cur = cur ? cur-1 : buf.tmHigh->size()-1;
	    int c_end = c_cur;

	    // Half divider
	    for(int d_win = buf.tmHigh->size()/2; d_win > HalfDivMinWin; d_win /= 2) {
		int c_cnext = c_end-d_win;
		if(c_cnext < 0) c_cnext += buf.tmHigh->size();
		if(tm/per < (*buf.tmHigh)[c_cnext].tm/per) c_end = c_cnext;
	    }
	    // Proving
	    do {
		int w_pos = up_ord ? (end/per-(*buf.tmHigh)[c_end].tm/per) :
				     ((*buf.tmHigh)[c_end].tm/per-beg/per);
		if(up_ord && w_pos >= npos) {
		    if(itm) *itm = (w_pos==npos) ? (*buf.tmHigh)[c_end].tm : end-npos*per;
		    return (*buf.tmHigh)[c_end].val;
		}
		else if(!up_ord && w_pos <= npos) {
		    if(itm) *itm = (w_pos==npos) ? (*buf.tmHigh)[c_end].tm : beg+npos*per;
		    return (*buf.tmHigh)[c_end].val;
		}
		if(--c_end < 0) c_end = buf.tmHigh->size()-1;
	    } while(c_end != c_cur);

	    if(itm) *itm = 0;
	    return eval;
	}
	else {
	    int c_cur = cur ? cur-1 : buf.tmLow->size()-1;
	    int c_end = c_cur;

	    // Half divider
	    for(int d_win = buf.tmLow->size()/2; d_win > HalfDivMinWin; d_win /= 2) {
		int c_cnext = c_end-d_win;
		if(c_cnext < 0) c_cnext += buf.tmLow->size();
		if(tm/per < (int64_t)(*buf.tmLow)[c_cnext].tm*1000000/per) c_end=c_cnext;
	    }
	    // Proving
	    do {
		int w_pos = up_ord ? (end/per-(int64_t)(*buf.tmLow)[c_end].tm*1000000/per) :
				     ((int64_t)(*buf.tmLow)[c_end].tm*1000000/per-beg/per);
		if(up_ord && w_pos >= npos) {
		    if(itm) *itm = (w_pos==npos) ? (int64_t)(*buf.tmLow)[c_end].tm*1000000 : end-npos*per;
		    return (*buf.tmLow)[c_end].val;
		}
		if(!up_ord && w_pos <= npos) {
		    if(itm) *itm = (w_pos==npos) ? (int64_t)(*buf.tmLow)[c_end].tm*1000000 : beg+npos*per;
		    return (*buf.tmLow)[c_end].val;
		}
		if(--c_end < 0) c_end = buf.tmLow->size()-1;
	    }while(c_end != c_cur);

	    if(itm) *itm = 0;
	    return eval;
	}
    }
    //Process the flow buffer
    else {
	if(hgResTm) {
	    int c_end = buf.tmHigh->size()-1;
	    // Half divider
	    for(int d_win = c_end/2; d_win > HalfDivMinWin; d_win /= 2)
		if(!((!up_ord && tm >= (*buf.tmHigh)[c_end-d_win].tm) ||
			(up_ord && tm <= (*buf.tmHigh)[buf.tmHigh->size()-(c_end-d_win)-1].tm)))
		    c_end -= d_win;
	    // Scan last window
	    while(c_end >= 0) {
		if(!up_ord && tm >= (*buf.tmHigh)[c_end].tm) {
		    if(itm) *itm = (*buf.tmHigh)[c_end].tm;
		    return (*buf.tmHigh)[c_end].val;
		}
		else if(up_ord && tm <= (*buf.tmHigh)[buf.tmHigh->size()-c_end-1].tm) {
		    if(itm) *itm = (*buf.tmHigh)[buf.tmHigh->size()-c_end-1].tm;
		    return (*buf.tmHigh)[buf.tmHigh->size()-c_end-1].val;
		}
		c_end--;
	    }
	    if(itm) *itm = 0;
	    return eval;
	}
	else {
	    int c_end = buf.tmLow->size()-1;
	    // Half divider
	    for(int d_win = c_end/2; d_win > HalfDivMinWin; d_win /= 2)
		if(!((!up_ord && tm/1000000 >= (*buf.tmLow)[c_end-d_win].tm) ||
			(up_ord && tm <= (int64_t)(*buf.tmLow)[buf.tmLow->size()-(c_end-d_win)-1].tm*1000000)))
		    c_end -= d_win;
	    // Scan last window
	    while(c_end >= 0) {
		if(!up_ord && tm/1000000 >= (*buf.tmLow)[c_end].tm) {
		    if(itm) *itm = (int64_t)(*buf.tmLow)[c_end].tm*1000000;
		    return (*buf.tmLow)[c_end].val;
		}
		else if(up_ord && tm <= (int64_t)(*buf.tmLow)[buf.tmLow->size()-c_end-1].tm*1000000) {
		    if(itm) *itm = (int64_t)(*buf.tmLow)[buf.tmLow->size()-c_end-1].tm*1000000;
		    return (*buf.tmLow)[buf.tmLow->size()-c_end-1].val;
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
    if(hrdGrd) {
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
	else if(npos < 0)
	    throw TError(TError::Arch_Val_OldBufVl, "ValBuf", _("The grid mode doesn't support inserting too old values %lld (%lld-%lld)."), tm, beg, end);
	else {
	    TpVal fillVl = eval;
	    if(fillLast && buf.grid->size()) fillVl = cur ? (*buf.grid)[cur-1] : (*buf.grid)[buf.grid->size()-1];
	    while(npos--) {
		TpVal w_val = npos ? fillVl : value;
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
    }
    //Process soft grid buffer
    else if(per) {
	int npos = (tm-end)/per;
	// Set value
	if(npos < 0)
	    throw TError(TError::Arch_Val_OldBufVl, "ValBuf", _("The grid mode doesn't support inserting too old values."));
	else {
	    if(hgResTm) {
		SHg b_el;

		// Update current end value
		if(npos == 0) {
		    b_el.tm = end; b_el.val = value;
		    int h_el = cur ? cur-1 : buf.tmHigh->size()-1;
		    if(((*buf.tmHigh)[h_el].tm-end)/per) {
			//  Write new value
			if(b_el.val == eval) mEvalCnt++;
			if(cur >= (int)buf.tmHigh->size()) buf.tmHigh->push_back(b_el);
			else {
			    if(cur+1 >= (int)buf.tmHigh->size()) beg = (*buf.tmHigh)[0].tm;
			    else beg = (*buf.tmHigh)[cur+1].tm;
			    beg = per*(beg/per);
			    if((*buf.tmHigh)[cur].val == eval) mEvalCnt--;
			    (*buf.tmHigh)[cur] = b_el;
			}
			//  Update cursor
			if((++cur >= size) && size) {
			    cur = 0;
			    //   Memory consumption is made optimal for limited size
			    if((int)buf.tmHigh->capacity() > size) {
				vector<SHg> *ns = new vector<SHg>;
				*ns = *buf.tmHigh;
				delete buf.tmHigh;
				buf.tmHigh = ns;
			    }
			}
		    }
		    else (*buf.tmHigh)[h_el] = b_el;
		    return;
		}

		// Insert new value
		int c_npos = npos;
		TpVal fillVl = eval;
		if(fillLast && buf.tmHigh->size()) fillVl = cur ? (*buf.tmHigh)[cur-1].val : (*buf.tmHigh)[buf.tmHigh->size()-1].val;
		while(c_npos--) {
		    //  Prepare data
		    if(c_npos) 	{ b_el.tm = end+(npos-c_npos)*per; b_el.val = fillVl; }
		    else	{ b_el.tm = tm; b_el.val = value; }
		    //  Check previous value
		    if(!buf.tmHigh->size() ||
			    (cur && (*buf.tmHigh)[cur-1].val != b_el.val ) ||
			    (!cur && (*buf.tmHigh)[buf.tmHigh->size()-1].val != b_el.val))
		    {
			//   Write new value
			if(b_el.val == eval) mEvalCnt++;
			if(cur >= (int)buf.tmHigh->size()) buf.tmHigh->push_back(b_el);
			else {
			    if(cur+1 >= (int)buf.tmHigh->size()) beg = (*buf.tmHigh)[0].tm;
			    else beg = (*buf.tmHigh)[cur+1].tm;
			    beg = per*(beg/per);
			    if((*buf.tmHigh)[cur].val == eval) mEvalCnt--;
			    (*buf.tmHigh)[cur] = b_el;
			}
			//   Update cursor
			if((++cur >= size) && size) {
			    cur = 0;
			    //   Memory consumption is made optimal for limited size
			    if((int)buf.tmHigh->capacity() > size) {
				vector<SHg> *ns = new vector<SHg>;
				*ns = *buf.tmHigh;
				delete buf.tmHigh;
				buf.tmHigh = ns;
			    }
			}
		    }
		    //  Updating the end time
		    end += per;
		}
	    }
	    else {
		SLw b_el;

		// Update the current end value
		if(npos == 0) {
		    b_el.tm = end/1000000; b_el.val = value;
		    int h_el = cur ? cur-1 : buf.tmLow->size()-1;
		    if(((int64_t)(*buf.tmLow)[h_el].tm*1000000-end)/per) {
			//  Write new value
			if(b_el.val == eval) mEvalCnt++;
			if(cur >= (int)buf.tmLow->size()) buf.tmLow->push_back(b_el);
			else {
			    if(cur+1 >= (int)buf.tmLow->size()) beg = (int64_t)(*buf.tmLow)[0].tm * 1000000;
			    else beg = (int64_t)(*buf.tmLow)[cur+1].tm * 1000000;
			    beg = per*(beg/per);
			    if((*buf.tmLow)[cur].val == eval) mEvalCnt--;
			    (*buf.tmLow)[cur] = b_el;
			}
			//  Update cursor
			if((++cur >= size) && size) {
			    cur = 0;
			    //   Memory consumption is made optimal for limited size
			    if((int)buf.tmLow->capacity() > size) {
				vector<SLw> *ns = new vector<SLw>;
				*ns = *buf.tmLow;
				delete buf.tmLow;
				buf.tmLow = ns;
			    }
			}
		    }
		    else (*buf.tmLow)[h_el] = b_el;
		    return;
		}

		// Insert new value
		int c_npos = npos;
		TpVal fillVl = eval;
		if(fillLast && buf.tmLow->size()) fillVl = cur ? (*buf.tmLow)[cur-1].val : (*buf.tmLow)[buf.tmLow->size()-1].val;
		while(c_npos--) {
		    //  Prepare data
		    if(c_npos)	{ b_el.tm = (end+(npos-c_npos)*per)/1000000; b_el.val = fillVl; }
		    else	{ b_el.tm = tm/1000000; b_el.val = value; }
		    //  Check previous value
		    if(!buf.tmLow->size() ||
			    (cur && (*buf.tmLow)[cur-1].val != b_el.val) ||
			    (!cur && (*buf.tmLow)[buf.tmLow->size()-1].val != b_el.val))
		    {
			//   Write new value
			if(b_el.val == eval) mEvalCnt++;
			if(cur >= (int)buf.tmLow->size()) buf.tmLow->push_back(b_el);
			else {
			    if(cur+1 >= (int)buf.tmLow->size())
				beg = (int64_t)(*buf.tmLow)[0].tm * 1000000;
			    else beg = (int64_t)(*buf.tmLow)[cur+1].tm * 1000000;
			    beg = per*(beg/per);
			    if((*buf.tmLow)[cur].val == eval) mEvalCnt--;
			    (*buf.tmLow)[cur] = b_el;
			}
			//   Update cursor
			if((++cur >= size) && size) {
			    cur = 0;
			    //   Memory consumption is made optimal for limited size
			    if((int)buf.tmLow->capacity() > size) {
				vector<SLw> *ns = new vector<SLw>;
				*ns = *buf.tmLow;
				delete buf.tmLow;
				buf.tmLow = ns;
			    }
			}
		    }
		    //  Update end time
		    end += per;
		}
	    }
	}
    }
    //Process the flow buffer
    else {
	if(hgResTm) {
	    SHg b_el = { tm, value };
	    if(tm < beg && size && (int)buf.tmHigh->size() >= size)
		throw TError(TError::Arch_Val_OldBufVl, "ValBuf", _("Inserting too old values to the buffer."));
	    int c_pos = 0;

	    // Half divider
	    int d_win = buf.tmHigh->size()/2;
	    while(d_win > HalfDivMinWin) {
		if(tm > (*buf.tmHigh)[c_pos+d_win].tm) c_pos += d_win;
		d_win /= 2;
	    }
	    // Scan last window
	    while(true) {
		if(c_pos >= (int)buf.tmHigh->size() || tm < (*buf.tmHigh)[c_pos].tm) {
		    if(b_el.val == eval) mEvalCnt++;
		    if(c_pos == (int)buf.tmHigh->size()) buf.tmHigh->push_back(b_el);
		    else buf.tmHigh->insert(buf.tmHigh->begin()+c_pos,b_el);
		    if(size && (int)buf.tmHigh->size() > size) {
			if(buf.tmHigh->begin()->val == eval) mEvalCnt--;
			buf.tmHigh->erase(buf.tmHigh->begin());
		    }
		    beg = (*buf.tmHigh)[0].tm;
		    end = (*buf.tmHigh)[buf.tmHigh->size()-1].tm;
		    return;
		}
		else if(tm == (*buf.tmHigh)[c_pos].tm) {
		    if((*buf.tmHigh)[c_pos].val == eval) mEvalCnt--;
		    (*buf.tmHigh)[c_pos] = b_el;
		    return;
		}
		c_pos++;
	    }
	}
	else {
	    SLw b_el = { (time_t)(tm/1000000ll), value };
	    if(tm < beg && size && (int)buf.tmLow->size() >= size)
		throw TError(TError::Arch_Val_OldBufVl, "ValBuf", _("Inserting too old values to the buffer."));
	    int c_pos = 0;
	    // Half divider
	    int d_win = buf.tmLow->size()/2;
	    while(d_win > HalfDivMinWin) {
		if(tm/1000000 > (*buf.tmLow)[c_pos+d_win].tm) c_pos += d_win;
		d_win /= 2;
	    }
	    // Scan last window
	    while(true) {
		if(c_pos >= (int)buf.tmLow->size() || tm/1000000 < (*buf.tmLow)[c_pos].tm) {
		    if(b_el.val == eval) mEvalCnt++;
		    if(c_pos == (int)buf.tmLow->size()) buf.tmLow->push_back(b_el);
		    else buf.tmLow->insert(buf.tmLow->begin()+c_pos,b_el);
		    if(size && (int)buf.tmLow->size() > size) {
			if( buf.tmLow->begin()->val == eval ) mEvalCnt--;
			buf.tmLow->erase(buf.tmLow->begin());
		    }
		    beg = (int64_t)(*buf.tmLow)[0].tm*1000000;
		    end = (int64_t)(*buf.tmLow)[buf.tmLow->size()-1].tm*1000000;
		    return;
		}
		else if(tm/1000000 == (*buf.tmLow)[c_pos].tm) {
		    if((*buf.tmLow)[c_pos].val == eval) mEvalCnt--;
		    (*buf.tmLow)[c_pos] = b_el;
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
    mBHGrd(cfg("BHGRD").getBd()), mBHRes(cfg("BHRES").getBd()), mFillLast(cfg("FillLast").getBd())
{
    mId = iid;
    mVType = TFld::Real;

    setUpBuf();
}

TVArchive::~TVArchive( )
{

}

string TVArchive::objName( )	{ return TCntrNode::objName()+":TVArchive"; }

TCntrNode &TVArchive::operator=( const TCntrNode &node )
{
    const TVArchive *src_n = dynamic_cast<const TVArchive*>(&node);
    if(!src_n) return *this;

    //Configuration copy
    exclCopy(*src_n, "ID;SrcMode;Source;");
    setDB(src_n->DB());

    if(src_n->startStat() && toStart() && !startStat())	start();

    return *this;
}

void TVArchive::preDisable( int flag )
{
    stop(flag);
}

void TVArchive::postDisable( int flag )
{
    if(flag&(NodeRemove|NodeRemoveOnlyStor)) {
	TBDS::dataDel(fullDB(flag&NodeRemoveOnlyStor), owner().nodePath()+tbl(), *this, TBDS::UseAllKeys);

	if(flag&NodeRemoveOnlyStor) { setStorage(mDB, "", true); return; }
    }
}

bool TVArchive::cfgChange( TCfg &co, const TVariant &pc )
{
    if(co.getS() == pc.getS()) return true;
    if(co.name() == "VTYPE" || co.name() == "BSIZE" || co.name() == "BHGRD" || co.name() == "BHRES") setUpBuf();
    else if(co.name() == "FillLast")	TValBuf::setFillLast(mFillLast);
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
	setUpBuf();
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
    if(!force && ipath.empty() && !pattrSrc.freeStat()) return pattrSrc;
    string srcPath = ipath.empty() ? srcData() : ipath;
    AutoHD<TVal> attr;
    if(srcMode() == TVArchive::Passive)	return attr;
    try {
	if(TSYS::strParse(srcPath,0,".") == "sub_DAQ" || TSYS::strParse(srcPath,0,".") == "DAQ")
	    attr = SYS->nodeAt(srcPath,0,'.');
	else attr = SYS->daq().at().attrAt(srcPath,'.');
    } catch(TError &err) { }

    return attr;
}

TArchiveS &TVArchive::owner( ) const	{ return *(TArchiveS *)nodePrev(); }

string TVArchive::tbl( )	{ return owner().subId()+"_val"; }

int64_t TVArchive::end( const string &arch )
{
    int64_t rez = 0;
    if(arch.empty() || arch == ARCH_BUF) {
	rez = TValBuf::end();
	if(!arch.empty()) return rez;
    }

    ResAlloc res(aRes, false);
    for(unsigned iA = 0; iA < archEl.size(); iA++)
	if((arch.empty() || arch == archEl[iA]->archivator().workId()) && archEl[iA]->end() > rez) {
	    rez = archEl[iA]->end();
	    if(!arch.empty())	break;
	}

    return rez;
}

int64_t TVArchive::begin( const string &arch )
{
    int64_t cTm = TSYS::curTime();
    int64_t rez = cTm;
    if(arch.empty() || arch == ARCH_BUF) {
	rez = TValBuf::begin();
	if(!arch.empty()) return rez;
    }
    if(!rez) rez = cTm;	//Empty buffer

    ResAlloc res(aRes, false);
    for(unsigned iA = 0; iA < archEl.size(); iA++)
	if((arch.empty() || arch == archEl[iA]->archivator().workId()) && archEl[iA]->begin() < rez) {
	    rez = archEl[iA]->begin();
	    if(!arch.empty())	break;
	}

    return (rez==cTm) ? 0 : rez;
}

int64_t TVArchive::period( const string &arch )
{
    if(arch.empty() || arch == ARCH_BUF) return TValBuf::period();
    ResAlloc res(aRes, false);
    for(unsigned iA = 0; iA < archEl.size(); iA++)
	if(arch == archEl[iA]->archivator().workId())
	    return (int64_t)(1e6*archEl[iA]->archivator().valPeriod());
    return 0;
}

void TVArchive::setPeriod( int64_t vl )
{
    if(!vl) vl = 1000000;
    mBPer = (double)vl/1000000.;
    mBSize = 100/mBPer.getR();
}

void TVArchive::setUpBuf( )	{ makeBuf((TFld::Type)mVType.getI(), mBSize, (int64_t)(1e6*mBPer.getR()), mBHGrd, mBHRes); }

void TVArchive::load_( TConfig *icfg )
{
    if(!SYS->chkSelDB(DB())) throw TError();

    if(icfg) *(TConfig*)this = *icfg;
    else TBDS::dataGet(fullDB(), owner().nodePath()+tbl(), *this);

    setUpBuf();
}

void TVArchive::save_( )
{
    TBDS::dataSet(fullDB(), owner().nodePath()+tbl(), *this);
    setDB(DB(), true);
}

void TVArchive::start( )
{
    if(!runSt)
	try {
	    runSt = true;
	    setSrcMode();
	} catch(...) { runSt = false; throw; }

    //Attach to the archivers
    string arch, archs = cfg("ArchS").getS();
    for(int iOff = 0; (arch = TSYS::strSepParse(archs,0,';',&iOff)).size(); )
	if(!archivatorPresent(arch))
	    try { archivatorAttach(arch); }
	    catch(TError &err)	{ mess_err(err.cat.c_str(), "%s", err.mess.c_str()); }
}

void TVArchive::stop( bool full_del )
{
    runSt = false;

    //Detach all the archivers
    vector<string> arch_ls;
    archivatorList(arch_ls);
    for(unsigned iL = 0; iL < arch_ls.size(); iL++)
	archivatorDetach(arch_ls[iL],full_del,false);

    setSrcMode();
}

void TVArchive::setSrcMode( SrcMode ivl, const string &isrc, bool noex )
{
    SrcMode vl = (ivl == TVArchive::SaveCur) ? srcMode() : ivl;
    string  src = (isrc == "<*>") ? mSource : isrc;

    bool stOnCh = runSt;

    //Disable all links
    if(!pattrSrc.freeStat() && (!runSt || vl == Passive || vl != srcMode() || src != srcData())) {
	owner().setActVal(this, false);
	srcPAttr().at().setArch(AutoHD<TVArchive>());
	pattrSrc.free();
    }

    //Set all links
    if(runSt && vl == ActiveAttr) {
	pattrSrc = srcPAttr(true,src);
	if(pattrSrc.freeStat()) { if(!noex) throw err_sys(_("Error connecting to source '%s'."),src.c_str()); }
	else {
	    //Double link prevent
	    if(!pattrSrc.at().arch().freeStat() && &pattrSrc.at().arch().at() != this) {
		if(!noex) {
		    pattrSrc.free();
		    // Appears at group copy on Vinnica Aviaries 
		    throw err_sys(TError::Arch_Val_DblVSrc, _("Archive '%s' is already connected to the target parameter '%s'."),
			srcPAttr(true,src).at().arch().at().id().c_str(), src.c_str());
		}
	    }
	    else {
		pattrSrc.at().setArch(AutoHD<TVArchive>(this));
		owner().setActVal(this, true);
	    }
	}
    }

    if(runSt && vl == PassiveAttr) {
	pattrSrc = srcPAttr(true, src);
	if(pattrSrc.freeStat()) { if(!noex) throw err_sys(_("Error connecting to source '%s'."),src.c_str()); }
	else {
	    //Double link prevent
	    if(!pattrSrc.at().arch().freeStat() && &pattrSrc.at().arch().at() != this) {
		if(!noex) {
		    pattrSrc.free();
		    // Appears at group copy on Vinnica Aviaries 
		    throw err_sys(TError::Arch_Val_DblVSrc, _("Archive '%s' is already connected to the target parameter '%s'."),
			srcPAttr(true,src).at().arch().at().id().c_str(), src.c_str());
		}
	    }
	    else pattrSrc.at().setArch(AutoHD<TVArchive>(this));
	}
    }

    if(mSrcMode.getI() != vl)	mSrcMode = (int)vl;
    if(mSource != src)	mSource = src;
    //Restore start status
    if(stOnCh && !runSt && (srcMode() == Passive || !srcData().empty())) start();
}

TVariant TVArchive::getVal( int64_t *tm, bool up_ord, const string &arch, bool onlyLocal )
{
    //Get from the buffer
    if((arch.empty() || arch == ARCH_BUF) && (!tm || (begin() && *tm >= begin() && end() /*&& *tm <= end()*/)))
	switch(TValBuf::valType()) {
	    case TFld::Integer:	return TValBuf::getI(tm, up_ord);
	    case TFld::String:	return TValBuf::getS(tm, up_ord);
	    case TFld::Real:	return TValBuf::getR(tm, up_ord);
	    case TFld::Boolean:	return TValBuf::getB(tm, up_ord);
	    default: break;
	}
    //Get from the archivers
    else {
	ResAlloc res(aRes, false);
	vector<pair<float,TVArchEl*> >	propArchs;
	for(unsigned iA = 0; iA < archEl.size(); iA++) {
	    TVArchivator &archPr = archEl[iA]->archivator();
	    if(((arch.empty() && archPr.selPrior()) || arch == archPr.workId()) &&
		    (!tm ||
			(up_ord && *tm <= archEl[iA]->end() && *tm > archEl[iA]->begin()-(int64_t)(1e6*archPr.valPeriod())) ||
			(!up_ord && *tm < archEl[iA]->end()+(int64_t)(1e6*archPr.valPeriod()) && *tm >= archEl[iA]->begin())))
		propArchs.push_back(pair<float,TVArchEl*>((float)archPr.selPrior()/(archPr.valPeriod()?archPr.valPeriod():1), archEl[iA]));
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
    //Free buffers fill directly but not free ones must be filled from the begin to the end,
    //  then for their there a buffer array is used which is back replayed at the end.
    //The function of the data requesting through the buffer and different archivers for a fixed period is significant one then implemented here.
    bool isFreeBuf = !buf.period();
    vector<TValBuf*> tBuf;
    TValBuf	*wBuf = &buf;
    int64_t ibeg_ = 0;

    try {

    //Getting from the buffer
    if((arch.empty() || arch == ARCH_BUF) && vOK(ibeg,iend)) {
	if(!isFreeBuf && arch != ARCH_BUF) {
	    wBuf = new TValBuf(buf.valType(), 0, buf.period(), true, true);
	    tBuf.push_back(wBuf);
	}

	ibeg_ = vmax(ibeg, iend-TValBuf::period()*limit);
	TValBuf::getVals(*wBuf, ibeg_, iend);
	iend = wBuf->begin()-1;
	if(arch == ARCH_BUF) return;
    }

    //Getting the priority archivers list for requested range
    ResAlloc res(aRes, false);
    vector<pair<float,TVArchEl*> >	propArchs;
    for(unsigned iA = 0; iA < archEl.size(); iA++) {
	TVArchivator &archPr = archEl[iA]->archivator();
	if(((arch.empty() && archPr.selPrior()) || arch == archPr.workId()) &&		//!!!! iend >= archEl[iA]->begin() need for requests outward the border
		((!ibeg || ibeg <= archEl[iA]->end()) && (!iend || iend >= archEl[iA]->begin())) && ibeg <= iend)
	    propArchs.push_back(pair<float,TVArchEl*>((float)archPr.selPrior()/(archPr.valPeriod()?archPr.valPeriod():1), archEl[iA]));
    }
    std::sort(propArchs.begin(), propArchs.end());

    //Processing the range at the priority
    for(vector<pair<float,TVArchEl*> >::reverse_iterator iA = propArchs.rbegin(); iA != propArchs.rend(); ++iA) {
	if(iA->second->begin() > iend)	continue;	//!!!! Try a block from the next archiver

	if(!isFreeBuf && arch.empty()) {
	    wBuf = new TValBuf(buf.valType(), 0, buf.period(), true, true);
	    tBuf.push_back(wBuf);
	} else wBuf = &buf;

	// Decrease the range begin to the limit
	int prevSz = wBuf->realSize();
	ibeg_ = prevSz ? ibeg : vmax(ibeg, iend-(int64_t)(1e6*iA->second->archivator().valPeriod())*(limit/*-wBuf->realSize()*/));
	iA->second->getVals(*wBuf, ibeg_, iend, onlyLocal);

	if(wBuf->realSize() != prevSz) {// The request is good
	    iend = wBuf->begin()-1;
	    if(iend <= ibeg)	break;	// All data requested
	    iA = propArchs.rbegin();	// Checking the next block
	}
    }

    for(int iBf = tBuf.size()-1; iBf >= 0; iBf--) {
	tBuf[iBf]->getVals(buf, tBuf[iBf]->begin(), tBuf[iBf]->end());
	delete tBuf[iBf]; tBuf[iBf] = NULL;
    }

    } catch(TError &err) {
	for(int iBf = tBuf.size()-1; iBf >= 0; iBf--)
	    if(tBuf[iBf]) delete tBuf[iBf];
	mess_err(err.cat.c_str(), "%s", err.mess.c_str());
	throw;
    }
}

void TVArchive::setVals( TValBuf &buf, int64_t ibeg, int64_t iend, const string &arch )
{
    //Check for put to buffer
    if(((arch.empty() && TValBuf::end()) || arch == ARCH_BUF) && iend > TValBuf::begin()) {
	bool onlyBuf = (ibeg >= TValBuf::end()) &&
	    (iend-ibeg)/TValBuf::period() <= TValBuf::size();	//!!!! Allow of writing new data blocks for the redundancy and DAQGate
	TValBuf::setVals(buf, vmax(ibeg,iend-TValBuf::size()*TValBuf::period()), iend);
	if(arch == ARCH_BUF || onlyBuf) return;	//To prevent spare writings direct to the archivers
    }

    //Put to the archivers
    ResAlloc res(aRes, false);
    for(unsigned iA = 0; iA < archEl.size(); iA++)
	if((arch.empty() || arch == archEl[iA]->archivator().workId()))
		//&& (!archEl[iA]->lastGet() || ibeg < archEl[iA]->lastGet()))	//!!!! Impossible to write directly else
	    try { archEl[iA]->setVals(buf, ibeg, iend/*vmin(iend,archEl[iA]->lastGet())*/); }
	    catch(TError &err) { mess_err(err.cat.c_str(), "%s", err.mess.c_str()); }
}

void TVArchive::getActiveData( const int64_t &tm )
{
    if(pattrSrc.freeStat() || !pattrSrc.at().dataActive()) return;

    int64_t stm = 0;
    switch(valType()) {
	case TFld::Boolean: { char vl = pattrSrc.at().getB(&stm); setB(vl, tm /*(tm?tm:stm)*/); break; }
	case TFld::Integer: { int64_t vl = pattrSrc.at().getI(&stm); setI(vl, tm /*(tm?tm:stm)*/); break; }
	case TFld::Real:    { double vl = pattrSrc.at().getR(&stm); setR(vl, tm /*(tm?tm:stm)*/); break; }
	case TFld::String:  { string vl = pattrSrc.at().getS(&stm); setS(vl, tm /*(tm?tm:stm)*/); break; }
	default: break;
    }
}

void TVArchive::archivatorList( vector<string> &ls )
{
    ResAlloc res(aRes, false);
    ls.clear();
    for(unsigned iL = 0; iL < archEl.size(); iL++)
	ls.push_back(archEl[iL]->archivator().workId());
}

bool TVArchive::archivatorPresent( const string &arch )
{
    ResAlloc res(aRes, false);
    try {
	string	aMod = TSYS::strSepParse(arch,0,'.'),
		aArch = TSYS::strSepParse(arch,1,'.');
	if(!owner().modPresent(aMod) || !owner().at(aMod).at().valPresent(aArch))	//!!!! To prevent the attachment
	    return true;
	AutoHD<TVArchivator> archivat = owner().at(aMod).at().valAt(aArch);
	for(unsigned iL = 0; iL < archEl.size(); iL++)
	    if(&archEl[iL]->archivator() == &archivat.at())
		return true;
    } catch(TError &err) { }

    return false;
}

void TVArchive::archivatorAttach( const string &arch )
{
    ResAlloc res(aRes, true);

    AutoHD<TVArchivator> archivat = owner().at(TSYS::strSepParse(arch,0,'.')).at().valAt(TSYS::strSepParse(arch,1,'.'));

    if(!archivat.at().startStat()) return;

    if(startStat()) {	//Attaching allowed only for started archive
	int iL, i_ins = -1;
	for(iL = 0; iL < (int)archEl.size(); iL++) {
	    if(&archEl[iL]->archivator() == &archivat.at()) break;
	    if(i_ins < 0 && archivat.at().valPeriod() <= archEl[iL]->archivator().valPeriod()) i_ins = iL;
	}
	if(iL >= (int)archEl.size()) {
	    if(i_ins < 0) archEl.push_back(archivat.at().archivePlace(*this));
	    else archEl.insert(archEl.begin()+i_ins,archivat.at().archivePlace(*this));
	}
    }

    if(!TRegExp("(^|;)"+arch+"(;|$)").test(cfg("ArchS").getS())) {
	string als;
	for(unsigned iL = 0; iL < archEl.size(); iL++)
	    als += archEl[iL]->archivator().workId()+";";
	cfg("ArchS").setS(als);
    }
}

void TVArchive::archivatorDetach( const string &arch, bool full, bool toModify )
{
    ResAlloc res(aRes, true);

    AutoHD<TVArchivator> archivat = owner().at(TSYS::strSepParse(arch,0,'.')).at().
					    valAt(TSYS::strSepParse(arch,1,'.'));
    //Find the archiver
    for(unsigned iL = 0; iL < archEl.size(); )
	if(&archEl[iL]->archivator() == &archivat.at()) {
	    archivat.at().archiveRemove(id(), full);
	    archEl.erase(archEl.begin()+iL);
	} else iL++;

    if(toModify && TRegExp("(^|;)"+arch+"(;|$)").test(cfg("ArchS").getS())) {
	string als;
	for(unsigned iL = 0; iL < archEl.size(); iL++)
	    als += archEl[iL]->archivator().workId()+";";
	cfg("ArchS").setS(als);
    }
}

void TVArchive::archivatorSort( )
{
    bool repTry;

    ResAlloc res(aRes, true);

    do {
	repTry = false;
	for(unsigned iL = 1; iL < archEl.size(); iL++)
	    if(archEl[iL-1]->archivator().valPeriod() > archEl[iL]->archivator().valPeriod()) {
		TVArchEl *t_el = archEl[iL-1];
		archEl[iL-1] = archEl[iL];
		archEl[iL] = t_el;
		repTry = true;
	    }
    } while(repTry);
}

string TVArchive::makeTrendImg( int64_t ibeg, int64_t iend, const string &iarch, int hsz, int vsz, double valmax, double valmin, string *tp )
{
    string rez;

    string lab_tm, lab_dt;
    time_t tm_t;
    struct tm ttm, ttm1;
    int64_t c_tm;
    int hv_border,		//Image border size
	h_w_start, h_w_size,	//Trend window horizontal start and size
	v_w_start, v_w_size;	//Trend window vertical start and size
    string sclMarkFont = "Times";
    int mrkFontSize = 10, begMarkBrd = -1, endMarkBrd = 0;

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

#if HAVE_GD_FORCE
    int brect[8];
    char *gdR = gdImageStringFT(NULL, &brect[0], 0, (char*)sclMarkFont.c_str(), mrkFontSize, 0, 0, 0, (char*)"000000");
    if(gdR) mess_sys(TMess::Error, _("gdImageStringFT error for font '%s': %s."), sclMarkFont.c_str(), gdR);
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

	// Select most like archiver
	string rarch = iarch;
	if(rarch.empty() && !vOK(ibeg,iend)) {
	    double best_a_per = 0;
	    ResAlloc res(aRes, false);
	    for(unsigned iA = 0; iA < archEl.size(); iA++)
		if(archEl[iA]->archivator().valPeriod() > best_a_per &&
		    archEl[iA]->archivator().valPeriod() <= (double)(h_max-h_min)/(1e5*hsz))
		{
		    best_a_per = archEl[iA]->archivator().valPeriod();
		    rarch = archEl[iA]->archivator().workId();
		}
	}

	getVals(buf, h_min, h_max, rarch, 1000000);	// ! More than 1000000 is notable slow
	if(!buf.end() || !buf.begin()) {
#if HAVE_GD_FORCE
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

#if HAVE_GD_FORCE
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
#if HAVE_GD_FORCE
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
#if HAVE_GD_FORCE
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
#if HAVE_GD_FORCE
	gdImageDestroy(im);
#endif
	return rez;
    }
    else if((v_max-v_min) < 1e-30 && fabs(v_max) < 1e-30)	{ v_max += 0.5; v_min -= 0.5; }
    else if((v_max-v_min) / fabs(v_min+(v_max-v_min)/2) < 0.001) {
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
    for(double i_v = v_min; (v_max-i_v)/v_div > -0.1; i_v += v_div) {
	int v_pos = v_w_start + v_w_size - (int)((double)v_w_size*(i_v-v_min)/(v_max-v_min));
#if HAVE_GD_FORCE
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
	} else c_pos = 0;
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
#if HAVE_GD_FORCE
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
#if HAVE_GD_FORCE
    int img_sz;
    char *img_ptr = (char *)gdImagePngPtrEx(im, &img_sz, 1);
    rez.assign(img_ptr, img_sz);
    gdFree(img_ptr);
    gdImageDestroy(im);

    if(tp) *tp = "png";
#else
    rez = im.save(XMLNode::XMLHeader);
    if(tp) *tp = "svg+xml";
#endif

    return rez;
}

TVariant TVArchive::objFuncCall( const string &iid, vector<TVariant> &prms, const string &user_lang )
{
    // bool status( ) - get the archive start status.
    if(iid == "status")	return startStat();
    // int end( string arch = "" ) - get the archive data end time for the archiver <arch>, in microseconds.
    if(iid == "end")	return (int64_t)end(prms.size()?prms[0].getS():string(""));
    // int begin( string arch = "" ) - get the archive data begin time for the archiver <arch>, in microseconds.
    if(iid == "begin")	return (int64_t)begin(prms.size()?prms[0].getS():string(""));
    // int period( string arch = "" ) - get the archive data period for the archiver <arch>, in microseconds.
    if(iid == "period")	return (int64_t)period(prms.size()?prms[0].getS():string(""));
    // TArrayObj archivatorList( ) - get the archivers list which using the archive as source.
    if(iid == "archivatorList") {
	TArrayObj *rez = new TArrayObj();
	vector<string> ls; archivatorList(ls);
	for(unsigned iA = 0; iA < ls.size(); iA++) rez->arSet(iA, ls[iA]);
	return rez;
    }
    // VarType getVal( int tm, bool up_ord = false, string arch = "" ) -
    //         get one value from the archive for the time <tm>, up order <up_ord> and the archiver <arch>.
    //  tm     - the time for requested value, in microseconds. Set to 0 for end();
    //  up_ord - fit the requested value time to up for the grid;
    //  arch   - requesting archiver, set to empty to try for the buffer and all archivers, set to "<buffer>" to process only for the buffer.
    if(iid == "getVal" && prms.size()) {
	int64_t tm = prms[0].getI();
	string arch = (prms.size() >= 3) ? prms[2].getS() : string("");
	if(!tm) tm = end(arch);
	TVariant rez = getVal(&tm, (prms.size()>=2)?prms[1].getB():false, arch);
	prms[0].setI(tm); prms[0].setModify();
	return rez;
    }
    // bool setVal( int tm, VarType vl, string arch = "" ) - set one value to the archive for the time <tm> and the archiver <arch>.
    //  tm - the time for requested value, in microseconds. Set to 0 for end();
    //  vl - the value;
    //  arch - the archiver for the request, set to empty for buffer and all archivers try, set to "<buffer>" for the buffer only process.
    if(iid == "setVal" && prms.size() >= 2) {
	if(!SYS->security().at().access(TSYS::strLine(user_lang,0),SEC_WR,"root",SARH_ID,RWRWR_))
	    return false;

	int64_t tm = prms[0].getI();
	string arch = (prms.size() >= 3) ? prms[2].getS() : string("");
	if(!tm) tm = end(arch);
	TFld::Type tp = TFld::String;
	switch(prms[1].type()) {
	    case TVariant::Boolean:	tp = TFld::Boolean;	break;
	    case TVariant::Integer:	tp = TFld::Integer;	break;
	    case TVariant::Real:	tp = TFld::Real;	break;
	    default:	break;
	}
	TValBuf buf(tp, 10, period(), true, true); buf.set(prms[1], tm);
	setVals(buf, buf.begin(), buf.end(), arch);
	return true;
    }
    // Array getVals( int begTm, int endTm, int period, string arch = "" ) -
    //         gets for the archive/history values from <begTm> and up to <endTm> for the archiver <arch>
    //  begTm  - begin time of the requesting data range, in microseconds, will be changed to the real data begin;
    //  endTm  - end time of the requesting data range, in microseconds;
    //  period - period of the data, in microseconds, must be necessarily specified and
    //           will be used the maximum one from the archive, will be changed to the real data period;
    //  arch   - requesting archiver, set to empty to try for the buffer and all archivers, set to "<buffer>" to process only the buffer.
    if(iid == "getVals" && prms.size() >= 3) {
	TValBuf buf(TValBuf::valType(), 0, vmax(TValBuf::period(),prms[2].getI()), true, true);
	buf.setFillLast(true);	//For low quality archives, but which mach longer
	getVals(buf, prms[0].getI(), prms[1].getI(), (prms.size()>=4?prms[3].getS():""));
	TArrayObj *rez = new TArrayObj();
	int64_t btm = (buf.begin()/buf.period())*buf.period();
	for(int64_t tm = btm; tm <= buf.end(); tm += buf.period()) {
	    int64_t ttm = tm;
	    rez->arSet(-1, buf.get(&ttm));
	}
	prms[0].setI(btm); prms[0].setModify();
	prms[2].setI(buf.period()); prms[2].setModify();
	return rez;
    }
    // bool setVals( Array buf, int tm, int period, string arch = "" ) -
    //         sets for the archive/history values <buf> to the archive from the begin time <tm>, the values period <period> and the archiver <arch>.
    //  buf    - array of the values to set;
    //  tm     - begin time of the setting data range, in microseconds;
    //  period - period of the setting data, in microseconds, must be necessarily specified and
    //           will be used the maximum one from the archive, will be changed to the real data period;
    //  arch   - setting archiver, set to empty to set for the buffer and all archivers, set to "<buffer>" to process only the buffer.
    if(iid == "setVals" && prms.size() >= 3) {
	AutoHD<TArrayObj> aBuf;
	if(prms[0].type() != TVariant::Object || (aBuf=prms[0].getO()).freeStat() ||
		!SYS->security().at().access(TSYS::strLine(user_lang,0),SEC_WR,"root",SARH_ID,RWRWR_))
	    return false;
	int64_t btm = prms[1].getI(),
		per = vmax(TValBuf::period(), prms[2].getI());
	TValBuf buf(TValBuf::valType(), 0, per, true, true);
	for(int iIt = 0; iIt < (int)aBuf.at().arSize(); iIt++, btm += per)
	    buf.set(aBuf.at().arGet(iIt), btm);
	setVals(buf, buf.begin(), buf.end(), (prms.size()>=4?prms[3].getS():""));
	prms[2].setI(per); prms[2].setModify();

	return true;
    }

    //Configuration functions call
    TVariant cfRez = objFunc(iid, prms, user_lang, RWRWR_, "root:" SARH_ID);
    if(!cfRez.isNull()) return cfRez;

    return TCntrNode::objFuncCall(iid, prms, user_lang);
}

void TVArchive::cntrCmdProc( XMLNode *opt )
{
    string a_path = opt->attr("path");
    //Service commands processing
    if(a_path == "/serv/val") {		//Values access
	if(ctrChkNode(opt,"info",RWRWRW,"root","root",SEC_RD)) {	//Value's data information
	    string arch = opt->attr("arch");
	    opt->setAttr("end", ll2s(end(arch)));
	    opt->setAttr("beg", ll2s(begin(arch)));
	    opt->setAttr("vtp", i2s(TValBuf::valType()));
	    if(arch.empty() || arch == ARCH_BUF) opt->setAttr("per",ll2s(TValBuf::period()));
	    else {
		ResAlloc res(aRes, false);
		for(unsigned iA = 0; iA < archEl.size(); iA++)
		    if(arch == archEl[iA]->archivator().workId())
		    { opt->setAttr("per",ll2s((int64_t)(1e6*archEl[iA]->archivator().valPeriod()))); break; }
	    }
	}
	else if(ctrChkNode(opt,"get",RWRWRW,"root","root",SEC_RD)) {	//Value's data request
	    int64_t	tm	= s2ll(opt->attr("tm"));
	    int64_t	tm_grnd	= s2ll(opt->attr("tm_grnd"));
	    string	arch	= opt->attr("arch");
	    bool	local	= s2i(opt->attr("local"));

	    // Processing for requesting one value
	    if(!tm) 	tm = TSYS::curTime();
	    if(!tm_grnd) {
		opt->setText(getVal(&tm,false,arch,local).getS());
		opt->setAttr("tm", ll2s(tm));
		return;
	    }
	    if(tm < tm_grnd)	throw err_sys(_("Range error"));

	    int64_t period = s2ll(opt->attr("per"));

	    // Process of archive block request
	    TValBuf buf(TValBuf::valType(), 0/*100000*/, vmax(TValBuf::period(),period), true, true);

	    //  Get the values buffer
	    if((arch.empty() || arch == ARCH_BUF) && vOK(tm_grnd,tm)) {
		TValBuf::getVals(buf, tm_grnd, tm);
		opt->setAttr("arch", ARCH_BUF);
	    }
	    else {
		ResAlloc res(aRes, false);
		//Get priority archivers list for requested range
		vector<pair<float,TVArchEl*> >	propArchs;
		for(unsigned iA = 0; iA < archEl.size(); iA++) {
		    TVArchivator &archPr = archEl[iA]->archivator();
		    if(((arch.empty() && archPr.selPrior()) || arch == archPr.workId()) &&
			    tm_grnd <= archEl[iA]->end() && tm >= archEl[iA]->begin())
			propArchs.push_back(pair<float,TVArchEl*>((float)archPr.selPrior()/vmax(1,abs(archPr.valPeriod()-period/1e6)), archEl[iA]));
		}
		std::sort(propArchs.begin(), propArchs.end());

		//Processing the range at the priority
		for(vector<pair<float,TVArchEl*> >::reverse_iterator iA = propArchs.rbegin(); iA != propArchs.rend(); ++iA) {
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
	    period = vmax(period, buf.period());
	    int mode = s2i(opt->attr("mode"));
	    if(mode < 0 || mode > 2) throw err_sys(_("Data mode '%d' is not supported"), mode);
	    switch(buf.valType()) {
		case TFld::Boolean: {
		    char tval_pr = EVAL_BOOL, tval_pr1 = EVAL_BOOL;
		    while(ibeg <= iend) {
			char tval = buf.getB(&ibeg, true);
			vpos_cur = (ibeg-buf.begin())/period;
			if(vpos_cur > vpos_end)
			    bool1: switch(mode) {
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
			else int1: switch(mode) {
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
		    if(!real_prec) real_prec = 15;
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
			else real1: switch(mode) {
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
			    str1: switch(mode) {
				case 0: text += TSYS::strEncode(tval_pr,TSYS::Custom,"\n")+"\n"; break;
				case 1:
				    if(!vpos_end || tval_pr != tval_pr1)
					text += i2s(vpos_end)+" "+TSYS::strEncode(tval_pr,TSYS::Custom,"\n")+"\n";
				    tval_pr1 = tval_pr;
				    break;
				case 2: throw err_sys(_("Binary mode is not supported for strings data"));
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
		    "tp","str","dest","select","select","/db/list","help",TMess::labStor().c_str());
		if(DB(true).size())
		    ctrMkNode("comm",opt,-1,"/prm/st/removeFromDB",TSYS::strMess(_("Remove from '%s'"),DB(true).c_str()).c_str(),RWRW__,"root",SARH_ID,
			1,"help",(DB(true)=="*.*")?TMess::labStorRemGenStor().c_str():"");
	    }
	    if(ctrMkNode("area",opt,-1,"/prm/cfg",_("Configuration"))) {
		TConfig::cntrCmdMake(opt,"/prm/cfg",0,"root",SARH_ID,RWRWR_);
		if(srcMode() == PassiveAttr || srcMode() == ActiveAttr)
		    ctrMkNode("fld",opt,-1,"/prm/cfg/Source","",RWRWR_,"root",SARH_ID,2,"dest","sel_ed","select","/cfg/prm_atr_ls");
		else ctrRemoveNode(opt,"/prm/cfg/Source");
		ctrMkNode("fld",opt,-1,"/prm/cfg/BSIZE",EVAL_STR,RWRWR_,"root",SARH_ID,1,
		    "help",_("The size of the buffer of values should be large enough for the periodicity of the archivers.\n"
			     "By default, this value is automatically configured for periodicity of the values of this archive and periodicity of archivers in 60 seconds."));
		ctrMkNode("fld",opt,-1,"/prm/cfg/BHGRD",EVAL_STR,RWRWR_,"root",SARH_ID,1,
		    "help",_("Default for fast processing of archiving values."));
		ctrMkNode("fld",opt,-1,"/prm/cfg/BHRES",EVAL_STR,RWRWR_,"root",SARH_ID,1,
		    "help",_("Set in case of periodicity of values in less than a second."));
		ctrMkNode("fld",opt,-1,"/prm/cfg/FillLast",EVAL_STR,RWRWR_,"root",SARH_ID,1,
		    "help",_("The passage values mostly fill by EVAL but sometime data periodicity of the source greater to the archive's one and it is normal."));
		ctrRemoveNode(opt,"/prm/cfg/ArchS");
	    }
	}
	if(ctrMkNode("area",opt,-1,"/arch",_("Archivers"),R_R_R_,"root",SARH_ID)) {
	    if(ctrMkNode("table",opt,-1,"/arch/arch",_("Archivers"),RWRWR_,"root",SARH_ID,1,"key","arch")) {
		ctrMkNode("list",opt,-1,"/arch/arch/arch",_("Archiver"),R_R_R_,"root",SARH_ID,1,"tp","str");
		ctrMkNode("list",opt,-1,"/arch/arch/start",_("Running"),R_R_R_,"root",SARH_ID,1,"tp","bool");
		ctrMkNode("list",opt,-1,"/arch/arch/proc",_("Processing"),RWRWR_,"root",SARH_ID,1,"tp","bool");
		ctrMkNode("list",opt,-1,"/arch/arch/per",_("Period, seconds"),R_R_R_,"root",SARH_ID,1,"tp","real");
		ctrMkNode("list",opt,-1,"/arch/arch/beg",_("Begin"),R_R_R_,"root",SARH_ID,1,"tp","str");
		ctrMkNode("list",opt,-1,"/arch/arch/end",_("End"),R_R_R_,"root",SARH_ID,1,"tp","str");
	    }
	}
	if(runSt && ctrMkNode("area",opt,-1,"/val",_("Values"),R_R___,"root",SARH_ID)) {
	    ctrMkNode("fld",opt,-1,"/val/tm",_("Time"),RWRW__,"root",SARH_ID,1,"tp","time");
	    ctrMkNode("fld",opt,-1,"/val/utm","",RWRW__,"root",SARH_ID,5,"tp","dec","len","6","min","0","max","999999","help",_("Microseconds"));
	    ctrMkNode("fld",opt,-1,"/val/size",_("Size, seconds"),RWRW__,"root",SARH_ID,1,"tp","real");
	    ctrMkNode("fld",opt,-1,"/val/arch",_("Archiver"),RWRW__,"root",SARH_ID,4,"tp","str","dest","select","select","/val/lstAVal",
		"help",_("Values archiver.\n"
			 "If the value is empty, the request will be processed for the buffer and for all archivers.\n"
			 "If \"<buffer>\" is specified then the request will be processed only for the archive buffer."));
	    ctrMkNode("fld",opt,-1,"/val/sw_trend",_("Show trend"),RWRW__,"root",SARH_ID,1,"tp","bool");
	    if(!s2i(TBDS::genPrmGet(owner().nodePath()+"vShowTrnd","0",opt->attr("user")))) {
		if(ctrMkNode("table",opt,-1,"/val/val",_("Table of the values"),R_R___,"root",SARH_ID)) {
		    ctrMkNode("list",opt,-1,"/val/val/0",_("Time"),R_R___,"root",SARH_ID,1,"tp","time");
		    ctrMkNode("list",opt,-1,"/val/val/0a",_("mcsec"),R_R___,"root",SARH_ID,1,"tp","dec");
		    ctrMkNode("list",opt,-1,"/val/val/1",_("Value"),R_R___,"root",SARH_ID,1,"tp","str");
		}
	    }
	    else {
		ctrMkNode("fld",opt,-1,"/val/pct_w",_("Image size"),RWRW__,"root",SARH_ID,4,"tp","dec","min","100","max","1024","help",_("Image width in pixels."));
		ctrMkNode("fld",opt,-1,"/val/pct_h","",RWRW__,"root",SARH_ID,4,"tp","dec","min","50","max","800","help",_("Image height in pixels."));
		ctrMkNode("fld",opt,-1,"/val/min",_("Scale of the values"),RWRW__,"root",SARH_ID,2,"tp","dec","help",_("Image minimum value."));
		ctrMkNode("fld",opt,-1,"/val/max","",RWRW__,"root",SARH_ID,2,"tp","dec","help",_("Image maximum value."));
		ctrMkNode("img",opt,-1,"/val/trend",_("Trend of the values"),R_R___,"root",SARH_ID);
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
	opt->setText(atm2s(end(ARCH_BUF)/1000000,"%d-%m-%Y %H:%M:%S.")+i2s(end(ARCH_BUF)%1000000));
    if(a_path == "/prm/st/bBeg" && ctrChkNode(opt))
	opt->setText(atm2s(begin(ARCH_BUF)/1000000,"%d-%m-%Y %H:%M:%S.")+i2s(begin(ARCH_BUF)%1000000));
    else if(a_path == "/prm/st/db") {
	if(ctrChkNode(opt,"get",RWRWR_,"root",SARH_ID,SEC_RD))	opt->setText(DB());
	if(ctrChkNode(opt,"set",RWRWR_,"root",SARH_ID,SEC_WR))	setDB(opt->text());
    }
    else if(a_path == "/prm/st/removeFromDB" && ctrChkNode(opt,"set",RWRW__,"root",SARH_ID,SEC_WR))
	postDisable(NodeRemoveOnlyStor);
    else if(a_path == "/prm/cfg/Source") {
	if(ctrChkNode(opt,"get",RWRWR_,"root",SARH_ID,SEC_RD))	opt->setText(srcData()+(srcPAttr(true).freeStat()?"":" (+)"));
	if(ctrChkNode(opt,"set",RWRWR_,"root",SARH_ID,SEC_WR))	mSource = TSYS::strParse(opt->text(), 0, " ");
    }
    else if(a_path == "/cfg/prm_atr_ls" && ctrChkNode(opt)) SYS->daq().at().ctrListPrmAttr(opt, srcData(), false, '.');
    else if(a_path.compare(0,8,"/prm/cfg") == 0) TConfig::cntrCmdProc(opt, TSYS::pathLev(a_path,2), "root", SARH_ID, RWRWR_);
    else if(a_path == "/arch/arch") {
	if(ctrChkNode(opt,"get",RWRWR_,"root",SARH_ID,SEC_RD)) {
	    //Fill archivers table
	    XMLNode *n_arch = ctrMkNode("list",opt,-1,"/arch/arch/arch","",R_R_R_,"root",SARH_ID);
	    XMLNode *n_start= ctrMkNode("list",opt,-1,"/arch/arch/start","",R_R_R_,"root",SARH_ID);
	    XMLNode *n_prc  = ctrMkNode("list",opt,-1,"/arch/arch/proc","",RWRWR_,"root",SARH_ID);
	    XMLNode *n_per  = ctrMkNode("list",opt,-1,"/arch/arch/per","",R_R_R_,"root",SARH_ID);
	    XMLNode *n_beg  = ctrMkNode("list",opt,-1,"/arch/arch/beg","",R_R_R_,"root",SARH_ID);
	    XMLNode *n_end  = ctrMkNode("list",opt,-1,"/arch/arch/end","",R_R_R_,"root",SARH_ID);

	    vector<string> t_arch_ls, arch_ls;
	    owner().modList(t_arch_ls);
	    for(unsigned iTa = 0; iTa < t_arch_ls.size(); iTa++) {
		owner().at(t_arch_ls[iTa]).at().valList(arch_ls);
		for(unsigned iA = 0; iA < arch_ls.size(); iA++) {
		    TVArchEl *a_el = NULL;
		    //Find attached
		    ResAlloc res(aRes, false);
		    for(unsigned iL = 0; iL < archEl.size(); iL++)
			if(archEl[iL]->archivator().owner().modId() == t_arch_ls[iTa] && archEl[iL]->archivator().id() == arch_ls[iA])
			    a_el = archEl[iL];
		    //Fill table element
		    if(n_arch)	n_arch->childAdd("el")->setText(owner().at(t_arch_ls[iTa]).at().valAt(arch_ls[iA]).at().workId());
		    if(n_start)	n_start->childAdd("el")->setText(owner().at(t_arch_ls[iTa]).at().valAt(arch_ls[iA]).at().startStat()?"1":"0");
		    if(n_per)	n_per->childAdd("el")->setText(r2s(owner().at(t_arch_ls[iTa]).at().valAt(arch_ls[iA]).at().valPeriod(),6));
		    if(a_el) {
			if(n_prc) n_prc->childAdd("el")->setText("1");
			if(n_end)
			    n_end->childAdd("el")->setText(atm2s(a_el->end()/1000000,"%d-%m-%Y %H:%M:%S.")+i2s(a_el->end()%1000000));
			if(n_beg)
			    n_beg->childAdd("el")->setText(atm2s(a_el->begin()/1000000,"%d-%m-%Y %H:%M:%S.")+i2s(a_el->begin()%1000000));
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
	    opt->setText(TBDS::genPrmGet(owner().nodePath()+"vaTm","0",opt->attr("user")));
	    if(!s2i(opt->text())) opt->setText(i2s(TSYS::curTime()/1000000));
	}
	if(ctrChkNode(opt,"set",RWRW__,"root",SARH_ID,SEC_WR))
	    TBDS::genPrmSet(owner().nodePath()+"vaTm",(s2i(opt->text())>=TSYS::curTime()/1000000)?"0":opt->text(),opt->attr("user"));
    }
    else if(a_path == "/val/utm") {
	if(ctrChkNode(opt,"get",RWRW__,"root",SARH_ID,SEC_RD))	opt->setText(TBDS::genPrmGet(owner().nodePath()+"vaTm_u","0",opt->attr("user")));
	if(ctrChkNode(opt,"set",RWRW__,"root",SARH_ID,SEC_WR))	TBDS::genPrmSet(owner().nodePath()+"vaTm_u",opt->text(),opt->attr("user"));
    }
    else if(a_path == "/val/size") {
	if(ctrChkNode(opt,"get",RWRW__,"root",SARH_ID,SEC_RD))	opt->setText(TBDS::genPrmGet(owner().nodePath()+"vaSize","1",opt->attr("user")));
	if(ctrChkNode(opt,"set",RWRW__,"root",SARH_ID,SEC_WR))	TBDS::genPrmSet(owner().nodePath()+"vaSize",opt->text(),opt->attr("user"));
    }
    else if(a_path == "/val/arch") {
	if(ctrChkNode(opt,"get",RWRW__,"root",SARH_ID,SEC_RD))	opt->setText(TBDS::genPrmGet(owner().nodePath()+"vArch","",opt->attr("user")));
	if(ctrChkNode(opt,"set",RWRW__,"root",SARH_ID,SEC_WR))	TBDS::genPrmSet(owner().nodePath()+"vArch",opt->text(),opt->attr("user"));
    }
    else if(a_path == "/val/sw_trend") {
	if(ctrChkNode(opt,"get",RWRW__,"root",SARH_ID,SEC_RD))	opt->setText(TBDS::genPrmGet(owner().nodePath()+"vShowTrnd","0",opt->attr("user")));
	if(ctrChkNode(opt,"set",RWRW__,"root",SARH_ID,SEC_WR))	TBDS::genPrmSet(owner().nodePath()+"vShowTrnd",opt->text(),opt->attr("user"));
    }
    else if(a_path == "/val/pct_w") {
	if(ctrChkNode(opt,"get",RWRW__,"root",SARH_ID,SEC_RD))	opt->setText(TBDS::genPrmGet(owner().nodePath()+"vPctW","650",opt->attr("user")));
	if(ctrChkNode(opt,"set",RWRW__,"root",SARH_ID,SEC_WR))	TBDS::genPrmSet(owner().nodePath()+"vPctW",opt->text(),opt->attr("user"));
    }
    else if(a_path == "/val/pct_h") {
	if(ctrChkNode(opt,"get",RWRW__,"root",SARH_ID,SEC_RD))	opt->setText(TBDS::genPrmGet(owner().nodePath()+"vPctH","230",opt->attr("user")));
	if(ctrChkNode(opt,"set",RWRW__,"root",SARH_ID,SEC_WR))	TBDS::genPrmSet(owner().nodePath()+"vPctH",opt->text(),opt->attr("user"));
    }
    else if(a_path == "/val/max") {
	if(ctrChkNode(opt,"get",RWRW__,"root",SARH_ID,SEC_RD))	opt->setText(TBDS::genPrmGet(owner().nodePath()+"vMax","0",opt->attr("user")));
	if(ctrChkNode(opt,"set",RWRW__,"root",SARH_ID,SEC_WR))	TBDS::genPrmSet(owner().nodePath()+"vMax",opt->text(),opt->attr("user"));
    }
    else if(a_path == "/val/min") {
	if(ctrChkNode(opt,"get",RWRW__,"root",SARH_ID,SEC_RD))	opt->setText(TBDS::genPrmGet(owner().nodePath()+"vMin","0",opt->attr("user")));
	if(ctrChkNode(opt,"set",RWRW__,"root",SARH_ID,SEC_WR))	TBDS::genPrmSet(owner().nodePath()+"vMin",opt->text(),opt->attr("user"));
    }
    else if(a_path == "/val/lstAVal" && ctrChkNode(opt,"get",R_R___)) {
	opt->childAdd("el")->setText("");
	opt->childAdd("el")->setText(ARCH_BUF);
	vector<string> lsm, lsa;
	owner().modList(lsm);
	for(unsigned i_m = 0; i_m < lsm.size(); i_m++) {
	    owner().at(lsm[i_m]).at().valList(lsa);
	    for(unsigned iA = 0; iA < lsa.size(); iA++)
		opt->childAdd("el")->setText(lsm[i_m]+"."+lsa[iA]);
	}
    }
    else if(a_path == "/val/val" && ctrChkNode(opt,"get",R_R___,"root",SARH_ID,SEC_RD)) {
	int64_t end = (int64_t)s2i(TBDS::genPrmGet(owner().nodePath()+"vaTm","0",opt->attr("user")))*1000000;
	if(!(end/1000000)) end = (TSYS::curTime()/1000000) * 1000000;
	end += s2i(TBDS::genPrmGet(owner().nodePath()+"vaTm_u","0",opt->attr("user")));
	int64_t beg = end - (int64_t)(s2r(TBDS::genPrmGet(owner().nodePath()+"vaSize","1",opt->attr("user")))*1e6);

	TValBuf buf(TFld::String, 0, 0, false, true);
	getVals(buf, beg, end, TBDS::genPrmGet(owner().nodePath()+"vArch","",opt->attr("user")), 2000);

	XMLNode *n_tm   = ctrMkNode("list",opt,-1,"/val/val/0","",0440);
	XMLNode *n_utm	= ctrMkNode("list",opt,-1,"/val/val/0a","",0440);
	XMLNode *n_val  = ctrMkNode("list",opt,-1,"/val/val/1","",0440);

	int64_t c_tm = buf.begin();
	if(buf.end() && buf.begin())
	    while(c_tm <= buf.end()) {
	        string val = buf.getS(&c_tm, true);
		if(n_tm) n_tm->childIns(0,"el")->setText(i2s(c_tm/1000000));
		if(n_tm) n_utm->childIns(0,"el")->setText(i2s(c_tm%1000000));
		if(n_val)n_val->childIns(0,"el")->setText(val);
		c_tm++;
	    }
    }
    else if(a_path == "/val/trend" && ctrChkNode(opt,"get",R_R_R_,"root",SARH_ID,SEC_RD)) {
	int vPctW = vmin(1024, vmax(100,s2i(TBDS::genPrmGet(owner().nodePath()+"vPctW","650",opt->attr("user")))));
	int vPctH = vmin(800, vmax(50,s2i(TBDS::genPrmGet(owner().nodePath()+"vPctH","230",opt->attr("user")))));
	double vMax = s2r(TBDS::genPrmGet(owner().nodePath()+"vMax","0",opt->attr("user")));
	double vMin = s2r(TBDS::genPrmGet(owner().nodePath()+"vMin","0",opt->attr("user")));
	int64_t end = (int64_t) s2i(TBDS::genPrmGet(owner().nodePath()+"vaTm",i2s(TSYS::curTime()/1000000),opt->attr("user")))*1000000+
				s2i(TBDS::genPrmGet(owner().nodePath()+"vaTm_u","0",opt->attr("user")));
	if(!(end/1000000))	end = (TSYS::curTime()/1000000) * 1000000;
	int64_t beg = end - (int64_t)(s2r(TBDS::genPrmGet(owner().nodePath()+"vaSize","1",opt->attr("user")))*1e6);

	string tp = "png";
	opt->setText(TSYS::strEncode(makeTrendImg(beg,end,TBDS::genPrmGet(owner().nodePath()+"vArch","",opt->attr("user")),vPctW,vPctH,vMax,vMin,&tp),TSYS::base64));
	opt->setAttr("tp", tp);
    }
    else TCntrNode::cntrCmdProc(opt);
}

//*************************************************
//* TVArchivator                                  *
//*************************************************
TVArchivator::TVArchivator( const string &iid, const string &idb, TElem *cf_el ) : TConfig(cf_el), runSt(false), endrunReq(false),
    mId(cfg("ID")), mVPer(cfg("V_PER")), mAPer(cfg("A_PER")), mStart(cfg("START").getBd()), mSelPrior(cfg("SEL_PR").getId()), mDB(idb)
{
    mId = iid;
}

TVArchivator::~TVArchivator( )
{

}

TCntrNode &TVArchivator::operator=( const TCntrNode &node )
{
    const TVArchivator *src_n = dynamic_cast<const TVArchivator*>(&node);
    if(!src_n) return *this;

    //Configuration copy
    exclCopy(*src_n, "ID;ADDR;START;");
    setDB(src_n->DB());

    //if(src_n->startStat() && toStart() && !startStat()) start();

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
    if(flag&(NodeRemove|NodeRemoveOnlyStor)) {
	TBDS::dataDel(fullDB(flag&NodeRemoveOnlyStor), SYS->archive().at().nodePath()+tbl(), *this, TBDS::UseAllKeys);

	if(flag&NodeRemoveOnlyStor) { setStorage(mDB, "", true); return; }
    }
}

bool TVArchivator::cfgChange( TCfg &co, const TVariant &pc )
{
    if(co.name() == "V_PER") {
	if(!co.getR()) co.setR(1);
	co.setR(vmax(1e-6,co.getR()));	//Up to microseconds now

	//Call sort for all archives
	ResAlloc res(archRes, false);
	for(map<string,TVArchEl*>::iterator iel = archEl.begin(); iel != archEl.end(); ++iel)
	    iel->second->archive().archivatorSort();
    }
    else if(co.name() == "A_PER" && co.getB() != pc.getB() && startStat()) stop();

    modif();
    return true;
}

string TVArchivator::workId( )	{ return owner().modId()+"."+id(); }

void TVArchivator::start( )
{
    //Start archiver thread
    if(!runSt) {
	if(archPeriod()) SYS->taskCreate(nodePath('.',true), 0, TVArchivator::Task, this, 2, NULL, &runSt);
	owner().owner().setToUpdate();
    }

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

TTypeArchivator &TVArchivator::owner( ) const		{ return *(TTypeArchivator *)nodePrev(); }

void TVArchivator::load_( TConfig *icfg )
{
    if(!SYS->chkSelDB(DB())) throw TError();

    if(icfg) *(TConfig*)this = *icfg;
    else TBDS::dataGet(fullDB(), SYS->archive().at().nodePath()+tbl(), *this);
}

void TVArchivator::save_( )
{
    TBDS::dataSet(fullDB(), SYS->archive().at().nodePath()+tbl(), *this);
    setDB(DB(), true);
}

void *TVArchivator::Task( void *param )
{
    TVArchivator &arch = *(TVArchivator*)param;
    arch.endrunReq = false;
    arch.runSt = true;
    bool isLast = false;

    TSYS::sysSleep(arch.archPeriod());

    //Archiving
    while(true)
	try {
	    if(arch.endrunReq) isLast = true;

	    ResAlloc res(arch.archRes, false);
	    int64_t beg, end;
	    for(map<string,TVArchEl*>::iterator iel = arch.archEl.begin(); iel != arch.archEl.end(); ++iel)
		if(iel->second->archive().startStat()) {
		    TVArchEl *archEl = iel->second;
		    beg = vmax(archEl->mLastGet, archEl->archive().begin());
		    end = archEl->archive().end();
		    if(!beg || !end || beg > end) continue;
		    archEl->setVals(archEl->archive(), beg, end, true);
		}
	    res.release();

	    //Call for accumulated values archiving
	    arch.pushAccumVals();

	    if(isLast) break;

	    TSYS::taskSleep((int64_t)(1e9*arch.archPeriod()));
	} catch(TError &err) { mess_err(err.cat.c_str(), "%s", err.mess.c_str() ); }

    arch.runSt = false;

    return NULL;
}

TVariant TVArchivator::objFuncCall( const string &iid, vector<TVariant> &prms, const string &user_lang )
{
    // bool status( ) - get the archiver start status.
    if(iid == "status")	return startStat();

    //Configuration functions call
    TVariant cfRez = objFunc(iid, prms, user_lang, RWRWR_, "root:" SARH_ID);
    if(!cfRez.isNull()) return cfRez;

    return TCntrNode::objFuncCall(iid, prms, user_lang);
}

void TVArchivator::cntrCmdProc( XMLNode *opt )
{
    //Get page info
    if(opt->name() == "info") {
	TCntrNode::cntrCmdProc(opt);
	ctrMkNode("oscada_cntr",opt,-1,"/",_("Value archiver: ")+name(),RWRWR_,"root",SARH_ID);
	if(ctrMkNode("area",opt,-1,"/prm",_("Archiver"))) {
	    if(ctrMkNode("area",opt,-1,"/prm/st",_("State"))) {
		ctrMkNode("fld",opt,-1,"/prm/st/st",_("Running"),RWRWR_,"root",SARH_ID,1,"tp","bool");
		if(archPeriod()) ctrMkNode("fld",opt,-1,"/prm/st/tarch",_("Archiving time"),R_R_R_,"root",SARH_ID,1,"tp","str");
		ctrMkNode("fld",opt,-1,"/prm/st/db",_("Archiver DB"),RWRWR_,"root","root",4,
		    "tp","str","dest","select","select","/db/list","help",TMess::labStor().c_str());
		if(DB(true).size())
		    ctrMkNode("comm",opt,-1,"/prm/st/removeFromDB",TSYS::strMess(_("Remove from '%s'"),DB(true).c_str()).c_str(),RWRW__,"root",SARH_ID,
			1,"help",(DB(true)=="*.*")?TMess::labStorRemGenStor().c_str():"");
	    }
	    if(ctrMkNode("area",opt,-1,"/prm/cfg",_("Configuration"))) {
		TConfig::cntrCmdMake(opt,"/prm/cfg",0,"root",SARH_ID,RWRWR_);
		ctrMkNode("fld",opt,-1,"/prm/cfg/A_PER",EVAL_STR,RWRWR_,"root",SARH_ID,1,
		    "help",_("Set a zero period to disable the use of the archiver in processing the buffer - just a direct recording."));
		ctrMkNode("fld",opt,-1,"/prm/cfg/SEL_PR",EVAL_STR,RWRWR_,"root",SARH_ID,1,
		    "help",_("Selection of a priority of the archiver for using in the source mode \"All\". Set to zero for full the selection disabling."));
		ctrRemoveNode(opt,"/prm/cfg/MODUL");
	    }
	}
	if(ctrMkNode("area",opt,-1,"/arch",_("Archives")))
	    if(ctrMkNode("table",opt,-1,"/arch/arch",_("Archives"),R_R_R_,"root",SARH_ID)) {
		ctrMkNode("list",opt,-1,"/arch/arch/0",_("Archive"),R_R_R_,"root",SARH_ID,1,"tp","str");
		ctrMkNode("list",opt,-1,"/arch/arch/1",_("Period, seconds"),R_R_R_,"root",SARH_ID,1,"tp","real");
		ctrMkNode("list",opt,-1,"/arch/arch/2",_("Buffer size"),R_R_R_,"root",SARH_ID,1,"tp","dec");
		ctrMkNode("list",opt,-1,"/arch/arch/3",_("Last read buffer"),R_R_R_,"root",SARH_ID,1,"tp","str");
	    }
	return;
    }

    //Process command to page
    string a_path = opt->attr("path");
    if(a_path == "/prm/st/st") {
	if(ctrChkNode(opt,"get",RWRWR_,"root",SARH_ID,SEC_RD))	opt->setText(startStat() ? "1" : "0");
	if(ctrChkNode(opt,"set",RWRWR_,"root",SARH_ID,SEC_WR))	s2i(opt->text()) ? start() : stop();
    }
    else if(a_path == "/prm/st/tarch" && ctrChkNode(opt))
	opt->setText(tm2s(SYS->taskUtilizTm(nodePath('.',true))) + "[" + tm2s(SYS->taskUtilizTm(nodePath('.',true),true)) + "]");
    else if(a_path == "/prm/st/db") {
	if(ctrChkNode(opt,"get",RWRWR_,"root",SARH_ID,SEC_RD))	opt->setText(DB());
	if(ctrChkNode(opt,"set",RWRWR_,"root",SARH_ID,SEC_WR))	setDB(opt->text());
    }
    else if(a_path == "/prm/st/removeFromDB" && ctrChkNode(opt,"set",RWRW__,"root",SARH_ID,SEC_WR))
	postDisable(NodeRemoveOnlyStor);
    else if(a_path.compare(0,8,"/prm/cfg") == 0) TConfig::cntrCmdProc(opt, TSYS::pathLev(a_path,2), "root", SARH_ID, RWRWR_);
    else if(a_path == "/arch/arch" && ctrChkNode(opt)) {
	// Fill Archives table
	XMLNode *n_arch = ctrMkNode("list",opt,-1,"/arch/arch/0","");
	XMLNode *n_per  = ctrMkNode("list",opt,-1,"/arch/arch/1","");
	XMLNode *n_size = ctrMkNode("list",opt,-1,"/arch/arch/2","");
	XMLNode *n_lstRd = ctrMkNode("list",opt,-1,"/arch/arch/3","");

	ResAlloc res(archRes,false);
	for(map<string,TVArchEl*>::iterator iel = archEl.begin(); iel != archEl.end(); ++iel) {
	    if(n_arch)	n_arch->childAdd("el")->setText(iel->second->archive().id());
	    if(n_per)	n_per->childAdd("el")->setText(r2s((double)iel->second->archive().period()/1000000.,6));
	    if(n_size)	n_size->childAdd("el")->setText(i2s(iel->second->archive().size()));
	    if(n_lstRd)	n_lstRd->childAdd("el")->setText(atm2s(iel->second->mLastGet*1e-6)+"."+i2s(iel->second->mLastGet%1000000));
	}
    }
    else TCntrNode::cntrCmdProc(opt);
}

//*************************************************
//* TVArchEl                                      *
//*************************************************
TVArchEl::TVArchEl( TVArchive &iarchive, TVArchivator &iarchivator ) :
    mLastGet(0), prevTm(0), prevVal(EVAL_REAL), mArchive(iarchive), mArchivator(iarchivator)	{ }

TVArchEl::~TVArchEl( )	{ }

TVArchive &TVArchEl::archive( )		{ return mArchive; }

TVArchivator &TVArchEl::archivator( )	{ return mArchivator; }

TVariant TVArchEl::getVal( int64_t *tm, bool up_ord, bool onlyLocal )
{
    TVariant vl = getValProc(tm, up_ord);

    if(!onlyLocal && tm && archive().startStat() && vl.isEVal() && SYS->rdActive() &&
	(archive().srcMode() == TVArchive::ActiveAttr || archive().srcMode() == TVArchive::PassiveAttr))
    {
	int64_t remTm = 0;
	AutoHD<TVal> paVl = archive().srcPAttr();
	AutoHD<TParamContr> sPrm(dynamic_cast<TParamContr*>(&paVl.at().owner()));
	if(sPrm.at().owner().owner().redntAllow() && sPrm.at().owner().redntMode() != TController::Off) {
	    string lstStat;
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
		//Put to the local archive
		TValBuf buf(archive().valType(), 0, 0, false, true);
		buf.setS(vl.getS(), remTm);
		setVals(buf);
	    }
	    else if(!lstStat.empty()) goto reqCall;
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
    // Check for the redundancy allowing
    if(!onlyLocal && archive().startStat() && buf.evalCnt() > ecnt && SYS->rdActive() &&
	(archive().srcMode() == TVArchive::ActiveAttr || archive().srcMode() == TVArchive::PassiveAttr))
    {
	AutoHD<TVal> paVl = archive().srcPAttr();
	AutoHD<TParamContr> sPrm(dynamic_cast<TParamContr*>(&paVl.at().owner()));
	if(sPrm.at().owner().owner().redntAllow() && sPrm.at().owner().redntMode() != TController::Off) {
	    // Holes process
	    int64_t firstEval = 0, curEval = 0;
	    int64_t cbeg = buf.begin();
	    iend = buf.end();
	    XMLNode req("get");
	    do {
		string lstStat;
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
		    if(!lstStat.empty() && !s2i(req.attr("rez")) && s2ll(req.attr("tm_grnd")) && s2ll(req.attr("tm"))) {
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
			    switch(val_tp) {
				case TFld::Boolean: {
				    char vl = s2i(prevVal);
				    if(prevVal == EVAL_STR || vl == EVAL_BOOL) { evalOk = true; prevPos = curPos; }
				    else { noEvalOk = true; for( ; prevPos < curPos; prevPos++ ) buf.setB(vl, bbeg+prevPos*bper); }
				    break;
				}
				case TFld::Integer: {
				    int64_t vl = s2ll(prevVal);
				    if(prevVal == EVAL_STR || vl == EVAL_INT) { evalOk = true; prevPos = curPos; }
				    else { noEvalOk = true; for( ; prevPos < curPos; prevPos++ ) buf.setI(vl, bbeg+prevPos*bper); }
				    break;
				}
				case TFld::Real: {
				    double vl = s2r(prevVal);
				    if(prevVal == EVAL_STR || vl == EVAL_REAL) { evalOk = true; prevPos = curPos; }
				    else { noEvalOk = true; for( ; prevPos < curPos; prevPos++ ) buf.setR(vl, bbeg+prevPos*bper); }
				    break;
				}
				case TFld::String: {
				    if(prevVal == EVAL_STR) { evalOk = true; prevPos = curPos; }
				    else { noEvalOk = true; for( ; prevPos < curPos; prevPos++ ) buf.setS(prevVal, bbeg+prevPos*bper); }
				    break;
				}
			    }
			    prevVal = curVal;
			    if(prevPos > (bend-bbeg)/bper) break;
			}

			//Put buffer part to the archive
			if(noEvalOk)	setVals(buf, firstEval, curEval);
		    }
		    if(!lstStat.empty() && evalOk) goto reqCall;
		    lstStat = "";
		    firstEval = 0;
		}
	    } while(cbeg <= iend);
	}
    }
}

void TVArchEl::setVals( TValBuf &ibuf, int64_t beg, int64_t end, bool toAccum )
{
    int64_t a_per = (int64_t)(1e6*archivator().valPeriod());

    if(!beg || !end) { beg = ibuf.begin(); end = ibuf.end(); }
    beg = vmax((beg/a_per)*a_per, ibuf.begin()); end = vmin(end, ibuf.end());

    if(!beg || !end || beg/a_per > end/a_per) return;

    //Check for put to the buffer
    if(&archive() != &ibuf && archivator().archPeriod() && mLastGet && end > mLastGet && ((end-mLastGet)/archive().period()) < archive().size())
    { archive().TValBuf::setVals(ibuf,vmax(archive().end(),beg),end); return; }

    //Put direct to archive
    int64_t wPrevTm = 0, s_k, n_k;
    double wPrevVal = EVAL_REAL;

    if(&archive() == &ibuf || end > archive().end()) { wPrevTm = prevTm; wPrevVal = prevVal; }

    //Combining
    TVArchive::CombMode combM = archive().combMode();
    int64_t setOK = 0;
    if(&archive() == &ibuf && a_per > ibuf.period()) {
	TValBuf obuf(ibuf.valType(true), 0, a_per, true, true);
	for(int64_t c_tm = beg; c_tm <= end; ) {
	    switch(ibuf.valType()) {
		case TFld::Boolean: {
		    double c_val = ibuf.getR(&c_tm, true);
		    if(combM == TVArchive::LastVal) { obuf.setR(c_val, c_tm); c_tm += a_per; break; }

		    int vdif = c_tm/a_per - wPrevTm/a_per;
		    if(!vdif) {
			double v_o = wPrevVal;
			if(c_val == EVAL_REAL) c_val = v_o;
			if(c_val != EVAL_REAL && v_o != EVAL_REAL)
			    switch(combM) {
				case TVArchive::MovAver:
				    s_k = c_tm-a_per*(c_tm/a_per);
				    n_k = ibuf.period();
				    c_val = (v_o*s_k+c_val*n_k)/(s_k+n_k);
				    break;
				case TVArchive::MinVal:	c_val = vmin(c_val, v_o); break;
				case TVArchive::MaxVal:	c_val = vmax(c_val, v_o); break;
				default: break;
			    }
			wPrevVal = c_val; wPrevTm = c_tm;
		    }
		    if(vdif == 1 || (c_tm+1) > end) obuf.setR((wPrevVal!=EVAL_REAL) ? wPrevVal > 0.5 : wPrevVal, wPrevTm);
		    if(vdif) { wPrevVal = c_val; wPrevTm = c_tm; }
		    c_tm++;
		    break;
		}
		case TFld::String: {
		    string c_val = ibuf.getS(&c_tm,true);
		    obuf.setS(c_val,c_tm);
		    c_tm += a_per;
		    break;
		}
		case TFld::Integer: case TFld::Real: {
		    double c_val = ibuf.getR(&c_tm, true);
		    if(combM == TVArchive::LastVal) { obuf.setR(c_val, c_tm); c_tm += a_per; break; }

		    int vdif = c_tm/a_per - wPrevTm/a_per;
		    if(!vdif) {
			double v_o = wPrevVal;
			if(c_val == EVAL_REAL) c_val = v_o;
			if(c_val != EVAL_REAL && v_o != EVAL_REAL)
			    switch(combM) {
				case TVArchive::MovAver:
				    s_k = c_tm-a_per*(c_tm/a_per);
				    n_k = ibuf.period();
				    c_val = (v_o*s_k+c_val*n_k)/(s_k+n_k);
				    break;
				case TVArchive::MinVal:	c_val = vmin(c_val,v_o); break;
				case TVArchive::MaxVal:	c_val = vmax(c_val,v_o); break;
				default: break;
			    }
			wPrevVal = c_val; wPrevTm = c_tm;
		    }
		    if(vdif == 1 || (c_tm+1) > end) obuf.setR(wPrevVal, wPrevTm);
		    if(vdif) { wPrevVal = c_val; wPrevTm = c_tm; }
		    c_tm++;
		    break;
		}
		default: break;
	    }
	}
	setOK = setValsProc(obuf, obuf.begin(), obuf.end()/*end*/, toAccum);	//Or possible vmax(obuf.end(),end)
    }
    else setOK = setValsProc(ibuf, beg, end, toAccum);

    if(setOK) {
	if(&archive() == &ibuf && (!mLastGet || setOK > mLastGet)) mLastGet = setOK+1;
	if(&archive() == &ibuf || setOK > archive().end()) { prevTm = wPrevTm; prevVal = wPrevVal; }
    }
}
