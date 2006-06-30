
//OpenSCADA system file: tarchval.cpp
/***************************************************************************
 *   Copyright (C) 2006-2006 by Roman Savochenko                           *
 *   rom_as@fromru.com                                                     *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
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
#include <time.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <math.h>
#include <gd.h>
#include <gdfonts.h>
#include <gdfontt.h>

#include "resalloc.h"
#include "tsys.h"
#include "tvalue.h"
#include "tarchives.h"
#include "tarchval.h"


//================================================================
//===================== Value archivator =========================
//================================================================

//========================= TValBuf ==============================
TValBuf::TValBuf( ) : 
    m_val_tp(TFld::Dec), m_size(100), m_per(0), m_hrd_grd(false), m_hg_res_tm(false), m_end(0), m_beg(0)
{   
    buf.bl = NULL;
    hd_res = ResAlloc::resCreate();
    
    makeBuf(m_val_tp,m_size,m_per,m_hrd_grd,m_hg_res_tm);
}

TValBuf::TValBuf( TFld::Type vtp, int isz, long long ipr, bool ihgrd, bool ihres ) :
    m_val_tp(vtp), m_size(isz), m_per(ipr), m_hrd_grd(ihgrd), m_hg_res_tm(ihres), m_end(0), m_beg(0)
{
    buf.bl = NULL;
    hd_res = ResAlloc::resCreate();
    
    makeBuf(m_val_tp,m_size,m_per,m_hrd_grd,m_hg_res_tm);
}

void TValBuf::clear()
{
    switch(m_val_tp)
    {
        case TFld::Bool:    buf.bl->clear();	break;
        case TFld::Dec: case TFld::Oct: case TFld::Hex:
    			    buf.dec->clear();	break;
	case TFld::Real:    buf.real->clear();	break;
        case TFld::String:  buf.str->clear(); 	break;
    }
}

TValBuf::~TValBuf( )
{
    switch(m_val_tp)
    {
        case TFld::Bool:    delete buf.bl;  break;
	case TFld::Dec: case TFld::Oct: case TFld::Hex:
	                    delete buf.dec; break;
        case TFld::Real:    delete buf.real;break;
        case TFld::String:  delete buf.str; break;
    }
    ResAlloc::resDelete(hd_res);
}

int TValBuf::realSize()
{
    switch(valType())
    {
        case TFld::Bool:    return buf.bl->realSize();
	case TFld::Dec: case TFld::Oct: case TFld::Hex:
	                    return buf.dec->realSize();
        case TFld::Real:    return buf.real->realSize();
        case TFld::String:  return buf.str->realSize();
    }    

    return 0;
}

bool TValBuf::vOK( long long ibeg, long long iend )
{
    if( !begin() || !end() || iend < begin() || ibeg > end() || ibeg > iend )
	return false;
    return true;
} 

void TValBuf::valType( TFld::Type vl )
{
    makeBuf(vl, m_size, m_per, m_hrd_grd, m_hg_res_tm );
}

void TValBuf::hardGrid( bool vl )
{   
    makeBuf( m_val_tp, m_size, m_per, vl, m_hg_res_tm );
}

void TValBuf::highResTm( bool vl )
{
    makeBuf( m_val_tp, m_size, m_per, m_hrd_grd, vl );
}

void TValBuf::size( int vl )     
{ 
    makeBuf( m_val_tp, vl, m_per, m_hrd_grd, m_hg_res_tm );
}

void TValBuf::period( long long vl )
{    
    makeBuf( m_val_tp, m_size, vl, m_hrd_grd, m_hg_res_tm );
}

void TValBuf::makeBuf( TFld::Type v_tp, int isz, long long ipr, bool hd_grd, bool hg_res )
{
    ResAlloc res(hd_res,true);
    
    //Destroy buffer
    if( v_tp != m_val_tp && buf.bl )
    {
	switch(m_val_tp)
	{
		case TFld::Bool:	delete buf.bl;	break;
		case TFld::Dec: case TFld::Oct: case TFld::Hex:
		    		    	delete buf.dec;	break;
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
	    case TFld::Bool:	buf.bl = new TBuf<char>(EVAL_BOOL,m_size,m_per,m_hrd_grd,m_hg_res_tm,m_end,m_beg); 	break;
	    case TFld::Dec: case TFld::Oct: case TFld::Hex:
			    	buf.dec = new TBuf<int>(EVAL_INT,m_size,m_per,m_hrd_grd,m_hg_res_tm,m_end,m_beg);	break;
	    case TFld::Real:	buf.real = new TBuf<double>(EVAL_REAL,m_size,m_per,m_hrd_grd,m_hg_res_tm,m_end,m_beg);break;
	    case TFld::String:	buf.str = new TBuf<string>(EVAL_STR,m_size,m_per,m_hrd_grd,m_hg_res_tm,m_end,m_beg);	break;
	}    
	m_val_tp = v_tp;
    }
    if( isz != m_size || ipr != m_per || hd_grd != m_hrd_grd || hg_res != m_hg_res_tm )
	switch(m_val_tp)
	{
	    case TFld::Bool:    buf.bl->makeBuf(isz, ipr, hd_grd, hg_res);	break;
	    case TFld::Dec: case TFld::Oct: case TFld::Hex:
				buf.dec->makeBuf(isz, ipr, hd_grd, hg_res);	break;
	    case TFld::Real:	buf.real->makeBuf(isz, ipr, hd_grd, hg_res);	break;
	    case TFld::String:	buf.str->makeBuf(isz, ipr, hd_grd, hg_res);	break;
	}
}
	    
string TValBuf::getS( long long *itm, bool up_ord )
{    
    switch(valType())
    {
	case TFld::Bool:	
	    { char vl = getB(itm,up_ord); return (vl==EVAL_BOOL)?EVAL_STR:TSYS::int2str((bool)vl); }
	case TFld::Dec: case TFld::Oct: case TFld::Hex:				
	    { int vl = getI(itm,up_ord); return (vl==EVAL_INT)?EVAL_STR:TSYS::int2str(vl); }
	case TFld::Real:	
	    { double vl = getR(itm,up_ord); return (vl==EVAL_REAL)?EVAL_STR:TSYS::real2str(vl); }
	case TFld::String:      
	    ResAlloc res(hd_res,false); return buf.str->get(itm,up_ord);
    }
}

double TValBuf::getR( long long *itm, bool up_ord )
{
    switch(valType())
    {
	case TFld::Bool:	
	    { char vl = getB(itm,up_ord); return (vl==EVAL_BOOL)?EVAL_REAL:(bool)vl; }
	case TFld::Dec: case TFld::Oct: case TFld::Hex:
	    { int vl = getI(itm,up_ord); return (vl==EVAL_INT)?EVAL_REAL:(double)vl; }
	case TFld::String:	
	    { string vl = getS(itm,up_ord); return (vl==EVAL_STR)?EVAL_REAL:atof(vl.c_str()); }
	case TFld::Real:	
	    ResAlloc res(hd_res,false); return buf.real->get(itm,up_ord);
    }
}

int TValBuf::getI( long long *itm, bool up_ord )
{
    switch(valType())
    {
	case TFld::Bool:	
	    { char vl = getB(itm,up_ord); return (vl==EVAL_BOOL)?EVAL_INT:(bool)vl; }
	case TFld::String:	
	    { string vl = getS(itm,up_ord); return (vl==EVAL_STR)?EVAL_INT:atoi(vl.c_str()); }
	case TFld::Real:	
	    { double vl = getR(itm,up_ord); return (vl==EVAL_REAL)?EVAL_INT:(int)vl; }
	case TFld::Dec: case TFld::Oct: case TFld::Hex:
	    ResAlloc res(hd_res,false); return buf.dec->get(itm,up_ord);	
    }
}

char TValBuf::getB( long long *itm, bool up_ord )
{
    switch(valType())
    {
	case TFld::Dec: case TFld::Oct: case TFld::Hex:
	    { int vl = getI(itm,up_ord); return (vl==EVAL_INT)?EVAL_BOOL:(bool)vl; }
	case TFld::String:	
	    { string vl = getS(itm,up_ord); return (vl==EVAL_STR)?EVAL_BOOL:(bool)atoi(vl.c_str()); }
	case TFld::Real:	
	    { double vl = getR(itm,up_ord); return (vl==EVAL_REAL)?EVAL_BOOL:(bool)vl; }
	case TFld::Bool:	
	    ResAlloc res(hd_res,false); return buf.bl->get(itm,up_ord);
    }
}

void TValBuf::setS( const string &value, long long tm )
{
    switch(valType())
    {
	case TFld::Bool:	
	    setB((value==EVAL_STR)?EVAL_BOOL:(bool)atoi(value.c_str()),tm);	break;
	case TFld::Dec: case TFld::Oct: case TFld::Hex:
	    setI((value==EVAL_STR)?EVAL_INT:atoi(value.c_str()),tm);	break;
	case TFld::Real:	
	    setR((value==EVAL_STR)?EVAL_REAL:atof(value.c_str()),tm); 	break;
	case TFld::String:     	
	    ResAlloc res(hd_res,true); buf.str->set(value,tm); 		break;
    }
}

void TValBuf::setR( double value, long long tm )
{
    switch(valType())
    {
	case TFld::Bool:	
	    setB((value==EVAL_REAL)?EVAL_BOOL:(bool)value,tm);	break;
	case TFld::Dec: case TFld::Oct: case TFld::Hex:
	    setI((value==EVAL_REAL)?EVAL_INT:(int)value,tm);	break;
	case TFld::String:	
	    setS((value==EVAL_REAL)?EVAL_STR:TSYS::real2str(value),tm);	break;
	case TFld::Real:       	
	    ResAlloc res(hd_res,true); buf.real->set(value,tm); break;
    }
}

void TValBuf::setI( int value, long long tm )
{
    switch(valType())
    {
	case TFld::Bool:
	    setB((value==EVAL_INT)?EVAL_BOOL:(bool)value,tm);	break;
	case TFld::String:	
	    setS((value==EVAL_INT)?EVAL_STR:TSYS::int2str(value),tm);	break;
	case TFld::Real:        
	    setR((value==EVAL_INT)?EVAL_REAL:value,tm);	break;
	case TFld::Dec: case TFld::Oct: case TFld::Hex:
	    ResAlloc res(hd_res,true); buf.dec->set(value,tm);	break;
    }
}

void TValBuf::setB( char value, long long tm )
{
    switch(valType())
    {	
	case TFld::Dec: case TFld::Oct: case TFld::Hex:
	    setI((value==EVAL_BOOL)?EVAL_INT:(bool)value,tm);	break;
	case TFld::String:	
	    setS((value==EVAL_BOOL)?EVAL_STR:TSYS::int2str((bool)value),tm);	break;
	case TFld::Real:        
	    setR((value==EVAL_BOOL)?EVAL_REAL:(bool)value,tm);	break;
	case TFld::Bool:	
	    ResAlloc res(hd_res,true); buf.bl->set(value,tm);	break;
    }
}

void TValBuf::getVal( TValBuf &buf, long long ibeg, long long iend )
{
    if(!vOK(ibeg,iend))	return;
    if( ibeg < begin() )ibeg = begin();
    if( iend > end() ) 	iend = end();
    
    while(ibeg<iend)
    {	
	switch(valType())
	{
	    case TFld::Bool:	
	    {
		char val = getB(&ibeg,true);
		buf.setB(val,ibeg);
		break;
	    }
	    case TFld::Dec: case TFld::Oct: case TFld::Hex:
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

//======================= TValBuf::TBuf =============================
template <class TpVal> TValBuf::TBuf<TpVal>::TBuf( TpVal ieval, int &isz, long long &ipr, bool &ihgrd, bool &ihres, long long& iend, long long& ibeg ) :
    eval(ieval), size(isz), per(ipr), hrd_grd(ihgrd), hg_res_tm(ihres), end(iend), beg(ibeg), cur(0)
{
    buf.grid = NULL;

    makeBuf(size,per,hrd_grd,hg_res_tm);
}
 
template <class TpVal> TValBuf::TBuf<TpVal>::~TBuf( )
{
    if( hrd_grd ) 		delete buf.grid;
    else if( hg_res_tm )  	delete buf.tm_high;
    else    			delete buf.tm_low;
} 

template <class TpVal> void TValBuf::TBuf<TpVal>::clear()
{
    if( !buf.grid )	return;
    if( hrd_grd )   	buf.grid->clear();
    else if(hg_res_tm)	buf.tm_high->clear();
    else            	buf.tm_low->clear();
    cur = end = beg = 0;
}

template <class TpVal> int TValBuf::TBuf<TpVal>::realSize()
{
    if( !buf.grid )	return 0;
    if( hrd_grd )       return buf.grid->size();
    else if(hg_res_tm)  return buf.tm_high->size();
    else 		return buf.tm_low->size();
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
    	//Destroy buffer
    	cur = end = beg = 0;
	if( buf.grid )
	{
	    if( hrd_grd )		delete buf.grid;
	    else if( hg_res_tm )	delete buf.tm_high;
	    else			delete buf.tm_low;
    	}
    	buf.grid = NULL;

	//Make new buffer
	if( ihd_grd )	buf.grid = new vector<TpVal>;
	else if(ihg_res)buf.tm_high = new vector<SHg>;
	else		buf.tm_low = new vector<SLw>;
    }
}

template <class TpVal> TpVal TValBuf::TBuf<TpVal>::get( long long *itm, bool up_ord )
{
    long long tm = (itm)?(*itm):TSYS::curTime();
	
    if((up_ord && tm > end) || (!up_ord && tm < beg))	
	throw TError("ValBuf",Mess->I18N("Value no present."));
    if( hrd_grd )
    {	
	//*** Process hard grid buffer ***
	int npos = up_ord?(end-tm)/per:(long long)buf.grid->size()-1-(tm-beg)/per;
	if( npos < 0 || npos >= buf.grid->size() ) return eval;
	if(itm)	*itm = end-npos*per;
	return (*buf.grid)[((cur-npos-1)>=0)?(cur-npos-1):(buf.grid->size()+(cur-npos-1))];
    }
    else if( per )
    {
	//*** Process soft grid buffer ***
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
	//*** Proccess flow buffer ***
	if( hg_res_tm )
	{
	    int c_end = buf.tm_high->size()-1;	    
	    //- Half divider -
	    int d_win = c_end/2;
	    while(d_win>10)
	    {
		if( !((!up_ord && tm >= (*buf.tm_high)[c_end-d_win].tm) ||
			(up_ord && tm <= (*buf.tm_high)[buf.tm_high->size()-(c_end-d_win)-1].tm)) )
		    c_end-=d_win;
		d_win/=2;	    
	    }
	    //- Scan last window -
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
	    //- Half divider -
	    int d_win = c_end/2;
	    while(d_win>10)
	    {
		if( !((!up_ord && tm/1000000 >= (*buf.tm_low)[c_end-d_win].tm) ||
			(up_ord && tm <= (long long)(*buf.tm_low)[buf.tm_low->size()-(c_end-d_win)-1].tm*1000000)) )
		    c_end-=d_win;
		d_win/=2;	    
	    }
	    //- Scan last window -
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
    if(!tm) tm = TSYS::curTime();
    if(!end) 	end = (per)?per*(tm/per-1):tm;
    if(!beg) 	beg = (per)?per*(tm/per):tm;

    if( hrd_grd )
    {
	//*** Process hard grid buffer ***
	int npos = (tm-end)/per;
	//Set value
	if( !npos )	
	{
	    (*buf.grid)[(cur)?cur-1:buf.grid->size()-1] = value;	//Update last value
	    return;
	}
	else if( npos < 0 )	throw TError("ValBuf",Mess->I18N("Grid mode no support inserting old values."));	
	else
	    while(npos--)
	    {
		TpVal w_val = (npos)?eval:value;
		//Set new value
		if(cur >= buf.grid->size())	buf.grid->push_back(w_val);
		else
		{ 
		    beg+=per;
		    (*buf.grid)[cur]=w_val;
		}
		//Update cursor
		if( ++cur >= size ) cur = 0;
		//Update end time
		end+=per;			
	    }
    }
    else if( per )
    {
	//*** Process soft grid buffer ***
	int npos = (tm-end)/per;
	//Set value	
	if( npos < 0 )	throw TError("ValBuf",Mess->I18N("Grid mode no support inserting old values."));
	else
	{
	    if( hg_res_tm )
            {
		SHg b_el;
		
    		//Update current end value
		if( npos == 0)
		{
		    b_el.tm = end;
		    b_el.val = value;
		    int h_el = (cur)?cur-1:buf.tm_high->size()-1;
		    if( ((*buf.tm_high)[h_el].tm-end)/per )
		    {
			//Write new value
			if(cur >= buf.tm_high->size()) buf.tm_high->push_back(b_el);
			else
			{
			    if( cur+1 >= buf.tm_high->size() )
                                beg = (*buf.tm_high)[0].tm;
		       	    else beg = (*buf.tm_high)[cur+1].tm;
			    beg = per*(beg/per);
                            (*buf.tm_high)[cur]=b_el;
			}
			//Update cursor
			if( ++cur >= size ) cur = 0;			
		    }
		    else (*buf.tm_high)[h_el]=b_el;
		    return;
		}
		
    		//Insert new value
    		int c_npos = npos;
		while(c_npos--)
		{
		    //Prepare data
		    if(c_npos) 	{ b_el.tm = end+(npos-c_npos)*per;  b_el.val = eval; }
		    else	{ b_el.tm = tm; b_el.val = value; }
		    //Check previous value
		    if( !buf.tm_high->size() || 
			    (cur && (*buf.tm_high)[cur-1].val != b_el.val ) ||
			    (!cur && (*buf.tm_high)[buf.tm_high->size()-1].val != b_el.val) )
		    {
			//Write new value
			if(cur >= buf.tm_high->size())	buf.tm_high->push_back(b_el);
			else
			{
			    if( cur+1 >= buf.tm_high->size() ) 
				beg = (*buf.tm_high)[0].tm;
			    else beg = (*buf.tm_high)[cur+1].tm;
			    beg = per*(beg/per);
			    (*buf.tm_high)[cur]=b_el;
			}
			//Update cursor
			if( ++cur >= size ) cur = 0;
		    }
		    //Update end time
		    end+=per;
		}
	    }
    	    else
	    {
		SLw b_el;

    		//Update current end value
		if( npos == 0)
		{
		    b_el.tm = end/1000000;
		    b_el.val = value;
		    int h_el = (cur)?cur-1:buf.tm_low->size()-1;
		    if( ((long long)(*buf.tm_low)[h_el].tm*1000000-end)/per )
		    {
			//Write new value
			if(cur >= buf.tm_low->size()) buf.tm_low->push_back(b_el);
			else
			{
			    if( cur+1 >= buf.tm_low->size() )
                                beg = (long long)(*buf.tm_low)[0].tm * 1000000;
		       	    else beg = (long long)(*buf.tm_low)[cur+1].tm * 1000000;
			    beg = per*(beg/per);
                            (*buf.tm_low)[cur]=b_el;
			}
			//Update cursor
			if( ++cur >= size ) cur = 0;			
		    }
		    else (*buf.tm_low)[h_el]=b_el;
		    return;
		}
		
    		//Insert new value		
    		int c_npos = npos;
		while(c_npos--)
		{		
		    //Prepare data
		    if(c_npos) 	{ b_el.tm = (end+(npos-c_npos)*per)/1000000; b_el.val = eval; }
		    else	{ b_el.tm = tm/1000000; b_el.val = value; }
		    //Check previous value
		    if( !buf.tm_low->size() || 
			    (cur && (*buf.tm_low)[cur-1].val != b_el.val ) ||
			    (!cur && (*buf.tm_low)[buf.tm_low->size()-1].val != b_el.val) )
		    {
			//Write new value
			if(cur >= buf.tm_low->size())	buf.tm_low->push_back(b_el);
			else
			{
			    if( cur+1 >= buf.tm_low->size() ) 
				beg = (long long)(*buf.tm_low)[0].tm * 1000000;
			    else beg = (long long)(*buf.tm_low)[cur+1].tm * 1000000;
			    beg = per*(beg/per);
			    (*buf.tm_low)[cur]=b_el;
			}
			//Update cursor
			if( ++cur >= size ) cur = 0;
		    }
		    //Update end time
		    end+=per;
		}
	    }
	}
    }
    else
    {
	//*** Proccess flow buffer ***
	if( hg_res_tm )
	{
	    SHg b_el = { tm, value };
	    if( tm < beg && size && buf.tm_high->size() >= size )
		throw TError("ValBuf",Mess->I18N("Set too old value to buffer."));
	    int c_pos = 0;
	    
	    //- Half divider -
	    int d_win = buf.tm_high->size()/2;
	    while(d_win>10)
	    {
		if( tm > (*buf.tm_high)[c_pos+d_win].tm )
		    c_pos+=d_win;
		d_win/=2;
	    }
	    //- Scan last window -
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
		throw TError("ValBuf",Mess->I18N("Set too old value to buffer."));
	    int c_pos = 0;
	    //- Half divider -
	    int d_win = buf.tm_low->size()/2;
	    while(d_win>10)
	    {
		if( tm/1000000 > (*buf.tm_low)[c_pos+d_win].tm )
		    c_pos+=d_win;
		d_win/=2;
	    }
	    //- Scan last window -
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


//========================= TVArchive ============================
time_t TVArchive::m_beg = time(NULL);
time_t TVArchive::m_end = time(NULL);
int TVArchive::m_ubeg = 0; 
int TVArchive::m_uend = 0;
string TVArchive::m_arch = "";
bool TVArchive::m_sw_trend = false;

TVArchive::TVArchive( const string &iid, const string &idb, TElem *cf_el ) : 
    TConfig(cf_el), run_st(false), m_bd(idb),
    m_id(cfg("ID").getSd()), m_name(cfg("NAME").getSd()), m_dscr(cfg("DESCR").getSd()), m_start(cfg("START").getBd()),
    m_vtype(cfg("VTYPE").getId()), m_bper(cfg("BPER").getRd()), m_bsize(cfg("BSIZE").getId()), 
    m_bhgrd(cfg("BHGRD").getBd()), m_bhres(cfg("BHRES").getBd()), m_srcmode(cfg("SrcMode").getId()), 
    m_dsourc(cfg("Source").getSd()), m_archs(cfg("ArchS").getSd())
{
    //First init
    m_id = iid;
    m_vtype = TFld::Real;
    
    a_res = ResAlloc::resCreate();
    
    setUpBuf();
}

TVArchive::~TVArchive( )
{
    ResAlloc::resDelete(a_res);
}

void TVArchive::preDisable(int flag)
{
    stop(flag);
}

void TVArchive::postDisable(int flag)
{
    try
    {
        if( flag )
	    SYS->db().at().dataDel(BD(),owner().nodePath()+"ValArchive/",*this);
    }catch(TError err)
    { Mess->put(err.cat.c_str(),TMess::Warning,"%s",err.mess.c_str()); }    
}

string TVArchive::name()
{
    return (m_name.size())?m_name:m_id;
}

TArchiveS &TVArchive::owner()
{ 
    return *(TArchiveS *)nodePrev();
}

string TVArchive::BD()
{
    return m_bd+"."+owner().subId()+"_val";
}

long long TVArchive::end( const string &arch )
{
    long long rez = 0;
    if( !arch.size() || arch == BUF_ARCH_NM )
    {
	rez = TValBuf::end();
	if( arch.size() ) return rez;
    }
	
    ResAlloc res(a_res,false);
    for( int i_a = 0; i_a < arch_el.size(); i_a++ )
        if( (!arch.size() || arch == arch_el[i_a]->archivator().workId()) && arch_el[i_a]->end() > rez )
	{
            rez = arch_el[i_a]->end();
	    if( arch.size() )	break;
	}
    return rez;	    
}

long long TVArchive::begin( const string &arch )
{
    long long rez = TSYS::curTime();
    if( !arch.size() || arch == BUF_ARCH_NM )
    {
        rez = TValBuf::begin();
	if( arch.size() ) return rez;
    }
	
    ResAlloc res(a_res,false);
    for( int i_a = 0; i_a < arch_el.size(); i_a++ )
        if( (!arch.size() || arch == arch_el[i_a]->archivator().workId()) && arch_el[i_a]->begin() < rez )
	{
            rez = arch_el[i_a]->begin();
	    if( arch.size() )   break;
	}
    return rez;	
}

void TVArchive::valType( TFld::Type vl ){ m_vtype = vl; setUpBuf(); }
void TVArchive::hardGrid( bool vl ) 	{ m_bhgrd = vl; setUpBuf(); }
void TVArchive::highResTm( bool vl )	{ m_bhres = vl; setUpBuf(); }

void TVArchive::size( int vl ) 		
{ 
    if(vl<10) vl = 10;
    if(vl>10000000) vl = 10000000;
    m_bsize = vl;
    setUpBuf();
}

void TVArchive::period( long long vl ) 	
{ 
    if(!vl) vl = 1000000;
    m_bper = (double)vl/1000000.;
    setUpBuf(); 
}

void TVArchive::setUpBuf()
{
    makeBuf((TFld::Type)m_vtype, m_bsize, (long long)(m_bper*1000000.), m_bhgrd, m_bhres );
}

void TVArchive::load( )
{
    SYS->db().at().dataGet(BD(),owner().nodePath()+"ValArchive/",*this);
    setUpBuf();
}

void TVArchive::save( )
{
    //Update Archivators list
    if( startStat() )
    {
	vector<string> arch_ls;
	archivatorList(arch_ls);
	m_archs = "";
	for( int i_l = 0; i_l < arch_ls.size(); i_l++ )
	    m_archs+=arch_ls[i_l]+";";
    }

    SYS->db().at().dataSet(BD(),owner().nodePath()+"ValArchive/",*this);
}
 
void TVArchive::start( )
{
    if( run_st ) return;
    
    run_st = true;
    
    srcMode((TVArchive::SrcMode)m_srcmode,m_dsourc);
    
    //Attach to archivators
    string arch;
    int i_arch = 0;
    while((arch = TSYS::strSepParse(m_archs,i_arch++,';')).size())
	if(!archivatorPresent(arch)) archivatorAttach(arch);
}

void TVArchive::stop( bool full_del )
{
    if( !run_st )	return;
    
    run_st = false;
    
    srcMode((TVArchive::SrcMode)m_srcmode,m_dsourc);
    
    //Detach all archivators
    vector<string> arch_ls;
    archivatorList(arch_ls);
    for( int i_l = 0; i_l < arch_ls.size(); i_l++ )
	archivatorDetach(arch_ls[i_l],full_del);
}

void TVArchive::srcMode( SrcMode vl, const string &isrc )
{
    //Disable all links
    if( (!run_st || vl != ActiveAttr || isrc != m_dsourc) && !pattr_src.freeStat() )
    {
	owner().setActValArch( id(), false );
	pattr_src.free();
	dynamic_cast<TVal&>(SYS->nodeAt(m_dsourc,0,'.').at()).arch(AutoHD<TVArchive>());
    }
    
    try
    {
	if( (!run_st || vl != PassiveAttr || isrc != m_dsourc) && 
		dynamic_cast<TVal *>(&SYS->nodeAt(m_dsourc,0,'.').at()) )
    	    dynamic_cast<TVal&>(SYS->nodeAt(m_dsourc,0,'.').at()).arch(AutoHD<TVArchive>());
    }catch(...){  }
	
    //Set all links
    if( run_st && vl == ActiveAttr && dynamic_cast<TVal *>(&SYS->nodeAt(isrc,0,'.').at()) )
    {
	dynamic_cast<TVal&>(SYS->nodeAt(isrc,0,'.').at()).arch(AutoHD<TVArchive>(this));
	pattr_src = SYS->nodeAt(isrc,0,'.');
	owner().setActValArch( id(), true );
    }
    
    if( run_st && vl == PassiveAttr && dynamic_cast<TVal *>(&SYS->nodeAt(isrc,0,'.').at()) )
	dynamic_cast<TVal&>(SYS->nodeAt(isrc,0,'.').at()).arch(AutoHD<TVArchive>(this));

    m_srcmode = vl;
    m_dsourc = isrc;
}

string TVArchive::getS( long long *tm, bool up_ord, const string &arch )
{    
    //Get from buffer
    if( (!arch.size() || arch == BUF_ARCH_NM) && (!tm || (*tm >= begin() && *tm <= end())) )
	return TValBuf::getS(tm,up_ord);
    //Get from archivators
    else 
    {
	ResAlloc res(a_res,false);
	for( int i_a = 0; i_a < arch_el.size(); i_a++ )	
	    if( (!arch.size() || arch == arch_el[i_a]->archivator().workId()) && 
		    (!tm || (*tm >= arch_el[i_a]->begin() && *tm <= arch_el[i_a]->end())) )
		return arch_el[i_a]->getS(tm,up_ord);
    }
    return EVAL_STR;
}

double TVArchive::getR( long long *tm, bool up_ord, const string &arch )
{
    //Get from buffer
    if( (!arch.size() || arch == BUF_ARCH_NM) && (!tm || (*tm >= begin() && *tm <= end())) )
        return TValBuf::getR(tm);
    //Get from archivators	
    else 
    {
	ResAlloc res(a_res,false);
	for( int i_a = 0; i_a < arch_el.size(); i_a++ )	
	    if( (!arch.size() || arch == arch_el[i_a]->archivator().workId()) && 
		    (!tm || (*tm >= arch_el[i_a]->begin() && *tm <= arch_el[i_a]->end())) )
		return arch_el[i_a]->getR(tm,up_ord);
    }
    return EVAL_REAL;
}

int TVArchive::getI( long long *tm, bool up_ord, const string &arch )
{
    //Get from buffer
    if( (!arch.size() || arch == BUF_ARCH_NM) && (!tm || (*tm >= begin() && *tm <= end())) )
	return TValBuf::getI(tm);
    //Get from archivators
    else 
    {
	ResAlloc res(a_res,false);
	for( int i_a = 0; i_a < arch_el.size(); i_a++ )	
	    if( (!arch.size() || arch == arch_el[i_a]->archivator().workId()) && 
		    (!tm || (*tm >= arch_el[i_a]->begin() && *tm <= arch_el[i_a]->end())) )
		return arch_el[i_a]->getI(tm,up_ord);
    }
    return EVAL_INT;
}

char TVArchive::getB( long long *tm, bool up_ord, const string &arch )
{
    //Get from buffer
    if( (!arch.size() || arch == BUF_ARCH_NM) && (!tm || (*tm >= begin() && *tm <= end())) )
        return TValBuf::getB(tm);	
    //Get from archivators
    else 
    {
	ResAlloc res(a_res,false);
	for( int i_a = 0; i_a < arch_el.size(); i_a++ )	
	    if( (!arch.size() || arch == arch_el[i_a]->archivator().workId()) && 
		    (!tm || (*tm >= arch_el[i_a]->begin() && *tm <= arch_el[i_a]->end())) )
		return arch_el[i_a]->getB(tm,up_ord);
    }
    return EVAL_BOOL;
}

void TVArchive::getVal( TValBuf &buf, long long ibeg, long long iend, const string &arch )
{
    //Get from buffer
    if( (!arch.size() || arch == BUF_ARCH_NM) && vOK(ibeg,iend) )
    {	
	TValBuf::getVal(buf,ibeg,iend);
	iend = buf.begin()-1;
    }
    //Get from archivators
    ResAlloc res(a_res,false);
    for( int i_a = 0; i_a < arch_el.size(); i_a++ )
    {
    	if( (!arch.size() || arch == arch_el[i_a]->archivator().workId()) &&
		((!ibeg || ibeg <= arch_el[i_a]->end()) && (!iend || iend > arch_el[i_a]->begin())) && ibeg <= iend )
	{
	    arch_el[i_a]->getVal(buf,ibeg,iend);
	    iend = buf.begin()-1;
	}
    }	
}

void TVArchive::setVal( TValBuf &buf, long long ibeg, long long iend, const string &arch )
{
    //Put to archivators
    ResAlloc res(a_res,false);
    for( int i_a = 0; i_a < arch_el.size(); i_a++ )
    	if( (!arch.size() || arch == arch_el[i_a]->archivator().workId()) )
	    arch_el[i_a]->setVal(buf,ibeg,iend);
}

void TVArchive::getActiveData()
{       
    long long tm = TSYS::curTime();
    if( !pattr_src.freeStat() )
    {
	switch(valType())
	{
	    case TFld::Bool:	setB(pattr_src.at().getB(&tm),tm); break;
	    case TFld::Dec: case TFld::Oct: case TFld::Hex: 
				setI(pattr_src.at().getI(&tm),tm); break;
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
    AutoHD<TVArchivator> archivat(dynamic_cast<TVArchivator *>(&owner().nodeAt(arch,0,'.').at()));
    if(archivat.freeStat()) return false;
    for( int i_l = 0; i_l < arch_el.size(); i_l++ )
        if( &arch_el[i_l]->archivator() == &archivat.at() )
    	    return true;
    return false;
}

void TVArchive::archivatorAttach( const string &arch )
{
    ResAlloc res(a_res,true);
    
    AutoHD<TVArchivator> archivat(dynamic_cast<TVArchivator *>(&owner().nodeAt(arch,0,'.').at()));
    if(archivat.freeStat() || !archivat.at().startStat())
	throw TError(nodePath().c_str(),"Archivator <%s> error or no started.",arch.c_str());	

    //Find already present archivator
    for( int i_l = 0; i_l < arch_el.size(); i_l++ )
        if( &arch_el[i_l]->archivator() == &archivat.at() )
	    return;
    //Find position
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

    AutoHD<TVArchivator> archivat(dynamic_cast<TVArchivator *>(&owner().nodeAt(arch,0,'.').at()));
    if(archivat.freeStat())
        throw TError(nodePath().c_str(),"Archivator path <%s> error.",arch.c_str());
    //Find archivator
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

string TVArchive::makeTrendImg( long long ibeg, long long iend, const string &iarch, int hsz, int vsz )
{
    char c_buf[30];
    time_t tm_t;
    struct tm *ttm;
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
    h_w_start = hv_border+h_border_serv;
    h_w_size  = hsz-h_w_start-h_border_serv-hv_border;
    v_w_start = hv_border+v_border_serv;
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
    int h_div = 1;
    long long h_len = iend - ibeg;
    while(h_len>=10){ h_div*=10; h_len/=10; }
    long long h_min = (ibeg/h_div)*h_div;
    long long h_max = (iend/h_div + ((iend%h_div)?1:0))*h_div;
    while(((h_max-h_min)/h_div)<5) h_div/=2;
    
    getVal(buf,h_min,h_max,iarch);
    if(!buf.end() || !buf.begin())      return rez;
	
    //--- Draw horisontal grid and symbols ---
    tm_t = h_min/1000000;
    ttm = localtime(&tm_t);
    snprintf(c_buf,sizeof(c_buf),"%d-%02d-%d",ttm->tm_mday,ttm->tm_mon+1,ttm->tm_year+1900);
    gdImageString(im,gdFontTiny,hv_border,v_w_start+v_w_size+3,(unsigned char *)c_buf,clr_symb);
    int i_cnt = 1;
    for(long long i_h = h_min; i_h <= h_max; i_h+=h_div, i_cnt++)
    {
	int h_pos = h_w_start+h_w_size*(i_h-h_min)/(h_max-h_min);
	gdImageLine(im,h_pos,v_w_start,h_pos,v_w_start+v_w_size,clr_grid);
	
	tm_t = i_h/1000000;
	ttm = localtime(&tm_t);
	snprintf(c_buf,sizeof(c_buf),"%d:%02d:%02d.%d",ttm->tm_hour,ttm->tm_min,ttm->tm_sec,i_h%1000000);
	gdImageString(im,gdFontTiny,h_pos-gdFontTiny->w*strlen(c_buf)/2,v_w_start+v_w_size+3+(i_cnt%2)*gdFontTiny->h,(unsigned char *)c_buf,clr_symb);
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
    if(v_max==v_min){ v_max+=1.; v_min-=1.; }
    double v_div = 1.;
    double v_len = v_max - v_min;
    while(v_len>=10.){ v_div*=10.; v_len/=10.; }
    while(v_len<1.) { v_div/=10.; v_len*=10.; }
    v_min = floor(v_min/v_div)*v_div;    
    v_max = ceil(v_max/v_div)*v_div;
    while(((v_max-v_min)/v_div)<5) v_div/=2;
    //--- Draw vertical grid and symbols ---
    for(double i_v = v_min; i_v <= v_max; i_v+=v_div)
    {
	int v_pos = v_w_start+v_w_size-(int)((double)v_w_size*(i_v-v_min)/(v_max-v_min));
	gdImageLine(im,h_w_start,v_pos,h_w_start+h_w_size,v_pos,clr_grid);
	
	snprintf(c_buf,sizeof(c_buf),"%.*f",fmod(i_v,1)?2:0,i_v);
	gdImageString(im,gdFontTiny,hv_border,v_pos-gdFontTiny->h,(unsigned char *)c_buf,clr_symb);
    }    

    //-- Draw trend --
    double aver_vl = EVAL_REAL;
    int    aver_pos= 0;
    long long aver_tm = 0;
    long long aver_lsttm = 0;
    double prev_vl = EVAL_REAL;
    int    prev_pos = 0;
    for(c_tm = buf.begin();true;c_tm++)
    {
	int c_pos;
	if(c_tm <= buf.end())
	{
	    c_val = buf.getR(&c_tm,true);
	    c_pos = h_w_start+h_w_size*(c_tm-h_min)/(h_max-h_min);
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

void TVArchive::cntrCmd_( const string &a_path, XMLNode *opt, TCntrNode::Command cmd )
{
    vector<string> list;

    if( cmd==TCntrNode::Info )
    {
	int my_gr = owner().subSecGrp();
	
	TCntrNode::cntrCmd_(a_path,opt,cmd);
	
	ctrMkNode("oscada_cntr",opt,-1,a_path.c_str(),"/",Mess->I18N("Value archive: ")+name());
	ctrMkNode("area",opt,-1,a_path.c_str(),"/prm",Mess->I18N("Archive"));
	ctrMkNode("area",opt,-1,a_path.c_str(),"/prm/st",Mess->I18N("State"));
	ctrMkNode("fld",opt,-1,a_path.c_str(),"/prm/st/st",Mess->I18N("Runing"),0664,0,my_gr,1,"tp","bool");
	ctrMkNode("fld",opt,-1,a_path.c_str(),"/prm/st/bd",Mess->I18N("Archive DB (module.db)"),0660,0,0,1,"tp","str");
	ctrMkNode("area",opt,-1,a_path.c_str(),"/prm/cfg",Mess->I18N("Config"));
	ctrMkNode("fld",opt,-1,a_path.c_str(),"/prm/cfg/id",cfg("ID").fld().descr(),0444,0,my_gr,1,"tp","str");
	ctrMkNode("fld",opt,-1,a_path.c_str(),"/prm/cfg/nm",cfg("NAME").fld().descr(),0664,0,my_gr,1,"tp","str");
	ctrMkNode("fld",opt,-1,a_path.c_str(),"/prm/cfg/dscr",cfg("DESCR").fld().descr(),0664,0,my_gr,3,"tp","str","cols","50","rows","3");
	ctrMkNode("fld",opt,-1,a_path.c_str(),"/prm/cfg/start",Mess->I18N("To start"),0664,0,my_gr,1,"tp","bool");	
	ctrMkNode("fld",opt,-1,a_path.c_str(),"/prm/cfg/vtp",cfg("VTYPE").fld().descr(),0664,0,my_gr,3,"tp","dec","dest","select","select","/cfg/vtp_ls");
	ctrMkNode("fld",opt,-1,a_path.c_str(),"/prm/cfg/srcm",cfg("Source").fld().descr(),0664,0,my_gr,3,"tp","dec","dest","select","select","/cfg/srcm_ls");
	if( srcMode() == PassiveAttr || srcMode() == ActiveAttr )
	    ctrMkNode("fld",opt,-1,a_path.c_str(),"/prm/cfg/src","",0664,0,my_gr,3,"tp","str","dest","sel_ed","select","/cfg/prm_atr_ls");
	ctrMkNode("fld",opt,-1,a_path.c_str(),"/prm/cfg/b_per",cfg("BPER").fld().descr(),0664,0,my_gr,1,"tp","real");
	ctrMkNode("fld",opt,-1,a_path.c_str(),"/prm/cfg/b_size",cfg("BSIZE").fld().descr(),0664,0,my_gr,1,"tp","dec");
	ctrMkNode("fld",opt,-1,a_path.c_str(),"/prm/cfg/b_hgrd",cfg("BHGRD").fld().descr(),0664,0,my_gr,1,"tp","bool");
	ctrMkNode("fld",opt,-1,a_path.c_str(),"/prm/cfg/b_hres",cfg("BHRES").fld().descr(),0664,0,my_gr,1,"tp","bool");
	ctrMkNode("comm",opt,-1,a_path.c_str(),"/prm/cfg/load",Mess->I18N("Load"),0440,0,my_gr);
	ctrMkNode("comm",opt,-1,a_path.c_str(),"/prm/cfg/save",Mess->I18N("Save"),0440,0,my_gr);
	ctrMkNode("area",opt,-1,a_path.c_str(),"/arch",Mess->I18N("Archivators"),0440,0,my_gr);
	ctrMkNode("table",opt,-1,a_path.c_str(),"/arch/arch",Mess->I18N("Archivators"),0664,0,0,1,"key","arch");
        ctrMkNode("list",opt,-1,a_path.c_str(),"/arch/arch/arch",Mess->I18N("Archivator"),0444,0,0,1,"tp","str");
	ctrMkNode("list",opt,-1,a_path.c_str(),"/arch/arch/start",Mess->I18N("Start"),0444,0,0,1,"tp","bool");
        ctrMkNode("list",opt,-1,a_path.c_str(),"/arch/arch/proc",Mess->I18N("Process"),0664,0,0,1,"tp","bool");
        ctrMkNode("list",opt,-1,a_path.c_str(),"/arch/arch/per",Mess->I18N("Period (s)"),0444,0,0,1,"tp","real");
	ctrMkNode("list",opt,-1,a_path.c_str(),"/arch/arch/beg",Mess->I18N("Begin"),0444,0,0,1,"tp","str");
        ctrMkNode("list",opt,-1,a_path.c_str(),"/arch/arch/end",Mess->I18N("End"),0444,0,0,1,"tp","str");
	if( run_st )
        {
            ctrMkNode("area",opt,-1,a_path.c_str(),"/val",Mess->I18N("Values"),0440,0,my_gr);
            ctrMkNode("fld",opt,-1,a_path.c_str(),"/val/beg",Mess->I18N("Begin"),0660,0,my_gr,1,"tp","time");
	    ctrMkNode("fld",opt,-1,a_path.c_str(),"/val/ubeg","",0660,0,my_gr,4,"tp","dec","len","6","min","0","max","999999");
            ctrMkNode("fld",opt,-1,a_path.c_str(),"/val/end",Mess->I18N("End"),0660,0,my_gr,1,"tp","time");
	    ctrMkNode("fld",opt,-1,a_path.c_str(),"/val/uend","",0660,0,my_gr,4,"tp","dec","len","6","min","0","max","999999");
	    ctrMkNode("fld",opt,-1,a_path.c_str(),"/val/arch",Mess->I18N("Archivator"),0660,0,my_gr,1,"tp","str");
	    ctrMkNode("fld",opt,-1,a_path.c_str(),"/val/sw_trend",Mess->I18N("Show trend"),0660,0,my_gr,1,"tp","bool");	    
	    if(!m_sw_trend)
	    {		
		ctrMkNode("table",opt,-1,a_path.c_str(),"/val/val",Mess->I18N("Values table"),0440);
        	ctrMkNode("list",opt,-1,a_path.c_str(),"/val/val/0",Mess->I18N("Time"),0440,0,0,1,"tp","str");
        	ctrMkNode("list",opt,-1,a_path.c_str(),"/val/val/1",Mess->I18N("Value"),0440,0,0,1,"tp","str");
	    }
	    else ctrMkNode("img",opt,-1,a_path.c_str(),"/val/trend",Mess->I18N("Values trend"),0444,0,my_gr);
	}
    }
    else if( cmd==TCntrNode::Get )
    {
	if( a_path == "/prm/st/st" )		ctrSetB( opt, run_st );
	else if( a_path == "/prm/st/bd" )	ctrSetS( opt, m_bd );
    	else if( a_path == "/prm/cfg/id" )	ctrSetS( opt, id() );
    	else if( a_path == "/prm/cfg/nm" )	ctrSetS( opt, name() );
    	else if( a_path == "/prm/cfg/dscr" )	ctrSetS( opt, dscr() );
    	else if( a_path == "/prm/cfg/start" ) 	ctrSetB( opt, m_start );
    	else if( a_path == "/prm/cfg/vtp" )	ctrSetI( opt, m_vtype );
	else if( a_path == "/prm/cfg/srcm" )	ctrSetI( opt, m_srcmode );
	else if( a_path == "/prm/cfg/src" )	ctrSetS( opt, m_dsourc );
    	else if( a_path == "/prm/cfg/b_per" )	ctrSetR( opt, m_bper );
    	else if( a_path == "/prm/cfg/b_size" )	ctrSetI( opt, m_bsize );
	else if( a_path == "/prm/cfg/b_hgrd" )	ctrSetB( opt, m_bhgrd );
	else if( a_path == "/prm/cfg/b_hres" )	ctrSetB( opt, m_bhres );
	else if( a_path == "/cfg/vtp_ls" )
	{
            opt->childClean();
            ctrSetS( opt, Mess->I18N("Boolean"), TSYS::int2str(TFld::Bool).c_str() );
            ctrSetS( opt, Mess->I18N("Integer"), TSYS::int2str(TFld::Dec).c_str() );
            ctrSetS( opt, Mess->I18N("Real"), 	 TSYS::int2str(TFld::Real).c_str() );
	    ctrSetS( opt, Mess->I18N("String"),  TSYS::int2str(TFld::String).c_str() );	
	}
	else if( a_path == "/cfg/srcm_ls" )
	{
	    opt->childClean();
	    ctrSetS( opt, Mess->I18N("Passive"), TSYS::int2str(TVArchive::Passive).c_str() );
	    ctrSetS( opt, Mess->I18N("Passive param. atribute"), TSYS::int2str(TVArchive::PassiveAttr).c_str() );
	    ctrSetS( opt, Mess->I18N("Active param. atribute"), TSYS::int2str(TVArchive::ActiveAttr).c_str() );
	}
	else if( a_path == "/cfg/prm_atr_ls" )
	{
	    opt->childClean();
	    string l_prm = m_dsourc;
			    
            int c_lv = 0;
            string c_path = "";
            ctrSetS( opt, c_path );
            while(TSYS::strSepParse(l_prm,c_lv,'.').size())
            {
                if( c_lv ) c_path+=".";
                c_path = c_path+TSYS::strSepParse(l_prm,c_lv,'.');
                ctrSetS( opt, c_path );
                c_lv++;
            }
            if(c_lv) c_path+=".";
            if( !dynamic_cast<TVal *>(&SYS->nodeAt(l_prm,0,'.').at()) )
            SYS->nodeAt(l_prm,0,'.').at().nodeList(list);
            for( unsigned i_a=0; i_a < list.size(); i_a++ )
        	ctrSetS( opt, c_path+list[i_a]);
	}
	else if( a_path == "/arch/arch" )
        {
	    //Fill Archivators table
    	    XMLNode *n_arch = ctrId(opt,"arch");
	    XMLNode *n_start= ctrId(opt,"start");
            XMLNode *n_prc  = ctrId(opt,"proc");
            XMLNode *n_per  = ctrId(opt,"per");
	    XMLNode *n_beg  = ctrId(opt,"beg");
            XMLNode *n_end  = ctrId(opt,"end");
	    
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
		    ctrSetS(n_arch,owner().at(t_arch_ls[i_ta]).at().valAt(arch_ls[i_a]).at().workId());
		    ctrSetB(n_start,owner().at(t_arch_ls[i_ta]).at().valAt(arch_ls[i_a]).at().startStat());
		    ctrSetR(n_per,owner().at(t_arch_ls[i_ta]).at().valAt(arch_ls[i_a]).at().valPeriod());
		    if( a_el )
		    {			
			ctrSetB(n_prc,true);
			char c_buf[30];
	        	struct tm *ttm;
			time_t tm_t = a_el->end()/1000000;
			ttm = localtime(&tm_t);
			snprintf(c_buf,sizeof(c_buf),"%d-%02d-%d %d:%02d:%02d.%d",
			    ttm->tm_mday,ttm->tm_mon+1,ttm->tm_year+1900,ttm->tm_hour,ttm->tm_min,ttm->tm_sec,a_el->end()%1000000);
			ctrSetS(n_end,c_buf);
			tm_t = a_el->begin()/1000000;
			ttm = localtime(&tm_t);
			snprintf(c_buf,sizeof(c_buf),"%d-%02d-%d %d:%02d:%02d.%d",
			    ttm->tm_mday,ttm->tm_mon+1,ttm->tm_year+1900,ttm->tm_hour,ttm->tm_min,ttm->tm_sec,a_el->begin()%1000000);
			ctrSetS(n_beg,c_buf);
		    }
		    else { ctrSetB(n_prc,false); ctrSetS(n_end,""); ctrSetS(n_beg,""); }
		}	    
	    }	    
        }
	else if( a_path == "/val/beg" )	ctrSetI( opt, m_beg );
        else if( a_path == "/val/end" ) ctrSetI( opt, m_end );
	else if( a_path == "/val/ubeg" )ctrSetI( opt, m_ubeg );
	else if( a_path == "/val/uend" )ctrSetI( opt, m_uend );
	else if( a_path == "/val/arch")	ctrSetS( opt, m_arch );
	else if( a_path == "/val/sw_trend" )	ctrSetB( opt, m_sw_trend );
	else if( a_path == "/val/val" )
	{
	    TValBuf buf( TFld::String, 0, 0, false, true );	    
	    
	    getVal( buf, (long long)m_beg*1000000+m_ubeg, (long long)m_end*1000000+m_uend, m_arch );
	    
	    XMLNode *n_tm   = ctrId(opt,"0");
            XMLNode *n_val  = ctrId(opt,"1");
	    
	    char c_buf[30];
	    struct tm *ttm;
	    time_t tm_t;
	    long long c_tm = buf.begin();
	    if(buf.end() && buf.begin())
		while(c_tm <= buf.end())
		{	    
		    string val = buf.getS(&c_tm,true);
		    tm_t = c_tm/1000000;
		    ttm = localtime(&tm_t);
		    snprintf(c_buf,sizeof(c_buf),"%d-%02d-%d %d:%02d:%02d.%d",
			ttm->tm_mday,ttm->tm_mon+1,ttm->tm_year+1900,ttm->tm_hour,ttm->tm_min,ttm->tm_sec,c_tm%1000000);
		    ctrSetS(n_tm,c_buf);
		    ctrSetS(n_val,val);
		    c_tm++;
		}
	}
	else if( a_path == "/val/trend" )
	{
	    opt->text(TSYS::strCode(makeTrendImg((long long)m_beg*1000000+m_ubeg,(long long)m_end*1000000+m_uend,m_arch),TSYS::base64));
	    opt->attr("tp","png");
	}
    	else TCntrNode::cntrCmd_(a_path,opt,cmd);
    }
    else if( cmd==TCntrNode::Set )
    {
	if( a_path == "/prm/st/st" )		{ if( ctrGetB( opt ) ) start(); else stop(); }
	else if( a_path == "/prm/st/bd" )       m_bd 	= ctrGetS( opt );
	else if( a_path == "/prm/cfg/id" )	m_id	= ctrGetS( opt );
	else if( a_path == "/prm/cfg/nm" )	m_name	= ctrGetS( opt );
	else if( a_path == "/prm/cfg/dscr" )  	m_dscr	= ctrGetS( opt );
	else if( a_path == "/prm/cfg/start" ) 	m_start = ctrGetB( opt );
	else if( a_path == "/prm/cfg/srcm" )    srcMode((TVArchive::SrcMode)ctrGetI(opt),m_dsourc);	
        else if( a_path == "/prm/cfg/src" )     srcMode((TVArchive::SrcMode)m_srcmode,ctrGetS( opt ));
    	else if( a_path == "/prm/cfg/vtp" )	valType((TFld::Type)ctrGetI( opt ));
    	else if( a_path == "/prm/cfg/b_per" )	period((long long)(1000000.*ctrGetR(opt)));
    	else if( a_path == "/prm/cfg/b_size" )	size(ctrGetI( opt ));
	else if( a_path == "/prm/cfg/b_hgrd" )	hardGrid(ctrGetB( opt ));
	else if( a_path == "/prm/cfg/b_hres" )	highResTm(ctrGetB( opt ));
 	else if( a_path == "/prm/cfg/load" )	load();
	else if( a_path == "/prm/cfg/save" )	save();
	else if( a_path == "/arch/arch" )
        {
	    if( opt->attr("col") == "proc" )
	    {
		if(ctrGetB( opt ))	archivatorAttach(opt->attr("key_arch"));
		else 			archivatorDetach(opt->attr("key_arch"));
	    }
	}
	else if( a_path == "/val/beg" ) 	m_beg = ctrGetI( opt );
        else if( a_path == "/val/end" ) 	m_end = ctrGetI( opt );		
	else if( a_path == "/val/ubeg" )	m_ubeg = ctrGetI( opt );
        else if( a_path == "/val/uend" )	m_uend = ctrGetI( opt );
	else if( a_path == "/val/sw_trend" )    m_sw_trend = ctrGetB( opt );
	else if( a_path == "/val/arch")		m_arch = ctrGetS( opt );
	else TCntrNode::cntrCmd_(a_path,opt,cmd);
    }
}


//================= TVArchivator =================================
TVArchivator::TVArchivator( const string &iid, const string &idb, TElem *cf_el ) : run_st(false), prc_st(false), 
    tm_calc(0.0), TConfig(cf_el), m_bd(idb), m_id(cfg("ID").getSd()), m_name(cfg("NAME").getSd()), 
    m_dscr(cfg("DESCR").getSd()), m_addr(cfg("ADDR").getSd()), m_start(cfg("START").getBd()), 
    m_v_per(cfg("V_PER").getRd()), m_a_per(cfg("A_PER").getId())
{
    m_id = iid;
    
    a_res = ResAlloc::resCreate();

    //Create calc timer
    struct sigevent sigev;
    sigev.sigev_notify = SIGEV_THREAD;
    sigev.sigev_value.sival_ptr = this;
    sigev.sigev_notify_function = Task;
    sigev.sigev_notify_attributes = NULL;
    timer_create(CLOCK_REALTIME,&sigev,&tmId);                        
}

TVArchivator::~TVArchivator()
{
    timer_delete(tmId);
    
    ResAlloc::resDelete(a_res);
}

string TVArchivator::name()   
{ 
    return (m_name.size())?m_name:m_id;
}

string TVArchivator::BD()
{
    return m_bd+"."+owner().owner().subId()+"_val_proc";
}

void TVArchivator::valPeriod( double iper )
{     
    m_v_per = iper?iper:1.;
    
    //Call sort for all archives
    ResAlloc res(a_res,false);
    for( int i_l = 0; i_l < arch_el.size(); i_l++ )
        arch_el[i_l]->archive().archivatorSort();
}

void TVArchivator::postEnable()
{
    cfg("MODUL").setS(owner().modId());
}

void TVArchivator::postDisable(int flag)
{
    try
    {
        if( flag )
	    SYS->db().at().dataDel(BD(),SYS->archive().at().nodePath()+"ValProc/",*this);
    }catch(TError err)
    { Mess->put(err.cat.c_str(),TMess::Warning,"%s",err.mess.c_str()); }    
}

void TVArchivator::preDisable(int flag)
{
    if(startStat())	stop(flag);
}

string TVArchivator::workId()
{
    return owner().modId()+".val_"+id();
}    

void TVArchivator::start()
{ 
    //Start interval timer for periodic thread creating
    struct itimerspec itval;
    itval.it_interval.tv_sec = itval.it_value.tv_sec = m_a_per;
    itval.it_interval.tv_nsec = itval.it_value.tv_nsec = 0;
    timer_settime(tmId, 0, &itval, NULL);
    
    run_st = true;
};

void TVArchivator::stop( bool full_del )
{
    //Stop interval timer for periodic thread creating
    struct itimerspec itval;
    itval.it_interval.tv_sec = itval.it_interval.tv_nsec = itval.it_value.tv_sec = itval.it_value.tv_nsec = 0;
    timer_settime(tmId, 0, &itval, NULL);
    if( TSYS::eventWait( prc_st, false, nodePath()+"stop",5) )
	throw TError(nodePath().c_str(),Mess->I18N("Archive thread no stoped!"));

    //Detach from all archives
    ResAlloc res(a_res,false);
    while(arch_el.size())
    {
	AutoHD<TVArchive> arch(&arch_el[0]->archive());
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
    for( int i_l = 0; i_l < arch_el.size(); i_l++ )
	ls.push_back(arch_el[i_l]->archive().id());
}

bool TVArchivator::archivePresent( const string &iid )
{
    ResAlloc res(a_res,false);
    for( int i_l = 0; i_l < arch_el.size(); i_l++ )
        if( arch_el[i_l]->archive().id() == iid )
	    return true;
    return false;	    
}

TVArchEl *TVArchivator::archivePlace( TVArchive &item )
{    
    ResAlloc res(a_res,true);
    for( int i_l = 0; i_l < arch_el.size(); i_l++ )
        if( arch_el[i_l]->archive().id() == item.id() )
	    return arch_el[i_l];
    TVArchEl *it_el = getArchEl( item );
    arch_el.push_back(it_el);
    
    return it_el;
}

void TVArchivator::archiveRemove( const string &iid, bool full )
{
    ResAlloc res(a_res,true);
    for( int i_l = 0; i_l < arch_el.size(); i_l++ )
        if( arch_el[i_l]->archive().id() == iid )
	{
	    if(full) arch_el[i_l]->fullErase();
	    delete arch_el[i_l];
	    arch_el.erase(arch_el.begin()+i_l);
	    break;
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

void TVArchivator::load( )
{
    SYS->db().at().dataGet(BD(),SYS->archive().at().nodePath()+"ValProc/",*this);
}

void TVArchivator::save( )
{
    SYS->db().at().dataSet(BD(),SYS->archive().at().nodePath()+"ValProc/",*this);
}

void TVArchivator::Task(union sigval obj)
{
    TVArchivator *arch = (TVArchivator *)obj.sival_ptr;
    if( arch->prc_st )  return;
    arch->prc_st = true;
    
    //Archiving
    try
    {
	unsigned long long t_cnt = SYS->shrtCnt();
	
	ResAlloc res(arch->a_res,false);
	long long beg, end;	
	for( int i_l = 0; i_l < arch->arch_el.size(); i_l++ )
	{
	    beg = arch->arch_el[i_l]->m_last_get;
	    end = arch->arch_el[i_l]->archive().end();
	    arch->arch_el[i_l]->setVal( arch->arch_el[i_l]->archive(), beg, end );
	    arch->arch_el[i_l]->m_last_get = end+1;
	}
	
	arch->tm_calc = 1.0e3*((double)(SYS->shrtCnt()-t_cnt))/((double)SYS->sysClk());    
    } catch(TError err)
    { Mess->put(err.cat.c_str(),TMess::Error,"%s",err.mess.c_str() ); }

    arch->prc_st = false;
}

void TVArchivator::archPeriod( int iper )
{    
    m_a_per = iper?iper:1;
    if( startStat() )
    {
	struct itimerspec itval;
	itval.it_interval.tv_sec = itval.it_value.tv_sec = m_a_per;
	itval.it_interval.tv_nsec = itval.it_value.tv_nsec = 0;
	timer_settime(tmId, 0, &itval, NULL);
    }
}

void TVArchivator::cntrCmd_( const string &a_path, XMLNode *opt, TCntrNode::Command cmd )
{
    if( cmd==TCntrNode::Info )
    {
	int my_gr = owner().owner().subSecGrp();
	
	TCntrNode::cntrCmd_(a_path,opt,cmd);

	ctrMkNode("oscada_cntr",opt,-1,a_path.c_str(),"/",Mess->I18N("Value archivator: ")+name());
	ctrMkNode("area",opt,-1,a_path.c_str(),"/prm",Mess->I18N("Archivator"));
	ctrMkNode("area",opt,-1,a_path.c_str(),"/prm/st",Mess->I18N("State"));
	ctrMkNode("fld",opt,-1,a_path.c_str(),"/prm/st/st",Mess->I18N("Runing"),0664,0,my_gr,1,"tp","bool");
	ctrMkNode("fld",opt,-1,a_path.c_str(),"/prm/st/tarch",Mess->I18N("Archiving time (msek)"),0444,0,0,1,"tp","real");
	ctrMkNode("fld",opt,-1,a_path.c_str(),"/prm/st/bd",Mess->I18N("Archivator DB (module.db)"),0660,0,0,1,"tp","str");
	ctrMkNode("area",opt,-1,a_path.c_str(),"/prm/cfg",Mess->I18N("Config"));	
	ctrMkNode("fld",opt,-1,a_path.c_str(),"/prm/cfg/id",cfg("ID").fld().descr(),0444,0,my_gr,1,"tp","str");
	ctrMkNode("fld",opt,-1,a_path.c_str(),"/prm/cfg/nm",cfg("NAME").fld().descr(),0664,0,my_gr,1,"tp","str");
	ctrMkNode("fld",opt,-1,a_path.c_str(),"/prm/cfg/dscr",cfg("DESCR").fld().descr(),0664,0,my_gr,3,"tp","str","cols","50","rows","3");
	ctrMkNode("fld",opt,-1,a_path.c_str(),"/prm/cfg/vper",cfg("V_PER").fld().descr(),0664,0,my_gr,1,"tp","real");
	ctrMkNode("fld",opt,-1,a_path.c_str(),"/prm/cfg/aper",cfg("A_PER").fld().descr(),0664,0,my_gr,1,"tp","dec");
	ctrMkNode("fld",opt,-1,a_path.c_str(),"/prm/cfg/addr",cfg("ADDR").fld().descr(),0664,0,my_gr,1,"tp","str");
	ctrMkNode("fld",opt,-1,a_path.c_str(),"/prm/cfg/start",Mess->I18N("To start"),0664,0,my_gr,1,"tp","bool");
	ctrMkNode("comm",opt,-1,a_path.c_str(),"/prm/cfg/load",Mess->I18N("Load"),0440,0,my_gr);
	ctrMkNode("comm",opt,-1,a_path.c_str(),"/prm/cfg/save",Mess->I18N("Save"),0440,0,my_gr);
	ctrMkNode("area",opt,-1,a_path.c_str(),"/arch",Mess->I18N("Archives"));
	ctrMkNode("table",opt,-1,a_path.c_str(),"/arch/arch",Mess->I18N("Archives"),0444);
        ctrMkNode("list",opt,-1,a_path.c_str(),"/arch/arch/0",Mess->I18N("Archive"),0444,0,0,1,"tp","str");
        ctrMkNode("list",opt,-1,a_path.c_str(),"/arch/arch/1",Mess->I18N("Period (s)"),0444,0,0,1,"tp","real");
	ctrMkNode("list",opt,-1,a_path.c_str(),"/arch/arch/2",Mess->I18N("Buffer size"),0444,0,0,1,"tp","dec");
    }
    else if( cmd==TCntrNode::Get )
    {
	if( a_path == "/prm/st/st" )		ctrSetB( opt, run_st );
	else if( a_path == "/prm/st/tarch" )	ctrSetR( opt, tm_calc );
	else if( a_path == "/prm/st/bd" )      	ctrSetS( opt, m_bd );
    	else if( a_path == "/prm/cfg/id" )	ctrSetS( opt, id() );
    	else if( a_path == "/prm/cfg/nm" )	ctrSetS( opt, name() );
    	else if( a_path == "/prm/cfg/dscr" )	ctrSetS( opt, dscr() );
	else if( a_path == "/prm/cfg/vper" )	ctrSetR( opt, valPeriod() );
	else if( a_path == "/prm/cfg/aper" )    ctrSetI( opt, archPeriod() );
	else if( a_path == "/prm/cfg/start" )	ctrSetB( opt, toStart() );
    	else if( a_path == "/prm/cfg/addr" )  	ctrSetS( opt, m_addr );
    	else if( a_path == "/prm/cfg/per" ) 	ctrSetB( opt, m_start );
	else if( a_path == "/arch/arch" )
        {
            //Fill Archives table
            XMLNode *n_arch = ctrId(opt,"0");
            XMLNode *n_per  = ctrId(opt,"1");
	    XMLNode *n_size = ctrId(opt,"2");
	    
	    ResAlloc res(a_res,false);
    	    for( int i_l = 0; i_l < arch_el.size(); i_l++ )
	    {
		ctrSetS(n_arch,arch_el[i_l]->archive().id());
                ctrSetR(n_per,(double)arch_el[i_l]->archive().period()/1000000.);
		ctrSetI(n_size,arch_el[i_l]->archive().size());
	    }
	}
	else TCntrNode::cntrCmd_(a_path,opt,cmd);
    }
    else if( cmd==TCntrNode::Set )
    {
	if( a_path == "/prm/st/st" )		{ if( ctrGetB( opt ) ) start(); else stop(); }
	else if( a_path == "/prm/st/bd" )      	m_bd   = ctrGetS(opt);
	else if( a_path == "/prm/cfg/nm" )	m_name = ctrGetS( opt );
	else if( a_path == "/prm/cfg/dscr" )	m_dscr = ctrGetS( opt );
	else if( a_path == "/prm/cfg/vper" )	valPeriod(ctrGetR( opt ));
	else if( a_path == "/prm/cfg/aper" )    archPeriod(ctrGetI( opt ));
	else if( a_path == "/prm/cfg/addr" )  	m_addr  = ctrGetS( opt );
	else if( a_path == "/prm/cfg/start" ) 	m_start = ctrGetB( opt );
	else if( a_path == "/prm/cfg/load" )	load();
	else if( a_path == "/prm/cfg/save" )	save();
	else TCntrNode::cntrCmd_(a_path,opt,cmd);
    }
}

//========================= TVArchEl =============================
TVArchEl::TVArchEl( TVArchive &iarchive, TVArchivator &iarchivator ) :
    m_achive(iarchive), m_archivator(iarchivator), m_last_get(0)
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
