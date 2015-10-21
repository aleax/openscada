
//OpenSCADA system module Special.FLibSYS file: varchfnc.cpp
/***************************************************************************
 *   Copyright (C) 2009-2015 by Roman Savochenko, <rom_as@oscada.org>      *
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

#include "varchfnc.h"

#if HAVE_FFTW3_H
#include <fftw3.h>
#endif

using namespace FLibSYS;

//*************************************************
//* VArchObj - Value archive object               *
//*************************************************
VArchObj::VArchObj( ) : mIsArch(false), mBuf(NULL)
{
    if(mess_lev() == TMess::Debug) SYS->cntrIter(objName(), 1);
}

VArchObj::~VArchObj( )
{
    close();
    if(mess_lev() == TMess::Debug) SYS->cntrIter(objName(), -1);
}

bool VArchObj::open( const string &inm )
{
    close();

    try {
	AutoHD<TVArchive> ta;
	AutoHD<TCntrNode> nd = SYS->nodeAt(inm,0,'.');
	if(dynamic_cast<TVal*>(&nd.at()))		ta = dynamic_cast<TVal&>(nd.at()).arch();
	else if(dynamic_cast<TVArchive*>(&nd.at()))	ta = nd;
	if(ta.freeStat()) return false;
	mArch = new AutoHD<TVArchive>(ta);
	mIsArch = true;
    }catch(TError err)	{ return false; }

    return true;
}

bool VArchObj::open( TFld::Type vtp, int isz, int64_t ipr, bool ihgrd, bool ihres )
{
    close();

    if(!(mBuf=new TValBuf(vtp,isz,ipr,ihgrd,ihres))) return false;
    mIsArch = false;

    return true;
}

void VArchObj::close( )
{
    if(isArch() && mArch) delete mArch;
    if(!isArch() && mBuf) delete mBuf;
    mIsArch = false;
    mBuf = NULL;
}

AutoHD<TVArchive> VArchObj::arch( )
{
    if(!isArch() || !mArch) return AutoHD<TVArchive>();
    return *mArch;
}

TValBuf *VArchObj::buf( )
{
    if(isArch()) return NULL;
    return mBuf;
}

TVariant VArchObj::propGet( const string &id )	{ throw TError("VArchObj", _("Properties no supported by the object.")); }

void VArchObj::propSet( const string &id, TVariant val ) { throw TError("VArchObj", _("Properties no supported by the object.")); }

TVariant VArchObj::funcCall( const string &id, vector<TVariant> &prms )
{
    //bool isNull() - no initiated object
    if(id == "isNull") return (!isArch() && !buf());
    //int begin( int usec = 0, string archivator = "" ) - getting the start time of the archive through the return of seconds
    //    and microseconds <usec> for the archivator <archivator>.
    if(id == "begin") {
	int64_t vtm;
	if(isArch())	vtm = arch().at().begin((prms.size()>=2) ? prms[1].getS() : "");
	else {
	    if(!buf()) return (int64_t)EVAL_INT;
	    vtm = buf()->begin();
	}
	if(prms.size() >= 1) { prms[0].setI(vtm%1000000); prms[0].setModify(); }
	return vtm/1000000;
    }
    //int end( int usec = 0, string archivator = "" ) - getting the end time of the archive through the return of seconds
    //    and microseconds <usec> for the archivator <archivator>.
    if(id == "end") {
	int64_t vtm;
	if(isArch())	vtm = arch().at().end( (prms.size()>=2) ? prms[1].getS() : "" );
	else {
	    if(!buf()) return (int64_t)EVAL_INT;
	    vtm = buf()->end();
	}
	if(prms.size() >= 1) { prms[0].setI(vtm%1000000); prms[0].setModify(); }
	return vtm/1000000;
    }
    //int period( int usec = 0, string archivator = "" ) - getting the periodicity of the archive through the return of seconds
    //    and microseconds <usec> for the archivator <archivator>.
    if(id == "period") {
	int64_t vper;
	if(isArch())	vper = arch().at().period((prms.size()>=2) ? prms[1].getS() : "");
	else {
	    if(!buf()) return (int64_t)EVAL_INT;
	    vper = buf()->period();
	}
	if(prms.size() >= 1) { prms[0].setI(vper%1000000); prms[0].setModify(); }
	return vper/1000000;
    }
    //ValObj get( int sec, int usec, bool upOrd = false, string archivator = "" ) - getting the value from the archive
    //       at the time <sec>:<usec> linked to the top <upOrd> for the archivator <archivator>.
    //       Real time of the value obtained is set in <sec>:<usec>.
    if(id == "get" && prms.size() >= 2) {
	TVariant vl;
	int64_t vtm = (int64_t)prms[0].getI()*1000000 + prms[1].getI();
	if(isArch()) vl = arch().at().getVal(&vtm, (prms.size()>=3)?prms[2].getB():false, (prms.size()>=4)?prms[3].getS():"");
	else {
	    if(!buf()) return EVAL_REAL;
	    switch(buf()->valType()) {
		case TFld::Boolean:	vl = buf()->getB(&vtm, (prms.size()>=3)?prms[2].getB():false);	break;
		case TFld::Integer:	vl = buf()->getI(&vtm, (prms.size()>=3)?prms[2].getB():false);	break;
		case TFld::Real:	vl = buf()->getR(&vtm, (prms.size()>=3)?prms[2].getB():false);	break;
		case TFld::String:	vl = buf()->getS(&vtm, (prms.size()>=3)?prms[2].getB():false);	break;
		default: break;
	    }
	}
	prms[0].setI(vtm/1000000); prms[0].setModify();
	prms[1].setI(vtm%1000000); prms[1].setModify();
	return vl;
    }
    //bool set( ValObj val, int sec, int usec ) - writing of the value <val> to the archive buffer for the time <sec>:<usec>.
    if(id == "set" && prms.size() >= 3) {
	if(isArch())
	    switch(arch().at().valType()) {
		case TFld::Boolean:	arch().at().setB(prms[0].getB(),(int64_t)prms[1].getI()*1000000+prms[2].getI());	break;
		case TFld::Integer:	arch().at().setI(prms[0].getI(),(int64_t)prms[1].getI()*1000000+prms[2].getI());	break;
		case TFld::Real:	arch().at().setR(prms[0].getR(),(int64_t)prms[1].getI()*1000000+prms[2].getI());	break;
		case TFld::String:	arch().at().setS(prms[0].getS(),(int64_t)prms[1].getI()*1000000+prms[2].getI());	break;
		default: break;
	    }
	else {
	    if(!buf()) return false;
	    switch(buf()->valType()) {
		case TFld::Boolean:	buf()->setB(prms[0].getB(),(int64_t)prms[1].getI()*1000000+prms[2].getI());	break;
		case TFld::Integer:	buf()->setI(prms[0].getI(),(int64_t)prms[1].getI()*1000000+prms[2].getI());	break;
		case TFld::Real:	buf()->setR(prms[0].getR(),(int64_t)prms[1].getI()*1000000+prms[2].getI());	break;
		case TFld::String:	buf()->setS(prms[0].getS(),(int64_t)prms[1].getI()*1000000+prms[2].getI());	break;
		default: break;
	    }
	}
	return true;
    }
    //bool copy( VArchObj src, int begSec, int begUSec, int endSec, int endUSec, string archivator = "" ) - copying of the part of
    //     the source archive <src> or its buffer in the current beginning from <begSec>:<begUSec> and ending with <endSec>:<endUSec>
    //     for the archivator <archivator>.
    if(id == "copy" && prms.size() >= 5) {
	AutoHD<VArchObj> src = prms[0].getO();
	if(src.freeStat()) return false;

	AutoHD<TVArchive> tarch;
	if(src.at().isArch()) {
	    TValBuf* vb = NULL;
	    if(isArch()) {
		tarch = arch();
		vb = &tarch.at();
	    }
	    else vb = buf();
	    if(!vb)     return false;
	    src.at().arch().at().getVals(*vb, prms[1].getI()*1000000+prms[2].getI(),
					      prms[3].getI()*1000000+prms[4].getI(), (prms.size()>=6)?prms[5].getS():"");
	}
	else if(isArch()) {
	    TValBuf* vb = NULL;
	    if(src.at().isArch()) {
		tarch = src.at().arch();
		vb = &tarch.at();
	    }
	    else vb = src.at().buf();
	    if(!vb)	return false;
	    arch().at().setVals(*vb, prms[1].getI()*1000000+prms[2].getI(),
				     prms[3].getI()*1000000+prms[4].getI(), (prms.size()>=6)?prms[5].getS():"");
	}
	else {
	    TValBuf* svb = src.at().buf();
	    TValBuf* dvb = buf();
	    if(!svb || !dvb) return false;
	    svb->getVals(*dvb, prms[1].getI()*1000000+prms[2].getI(), prms[3].getI()*1000000+prms[4].getI());
	}
	return true;
    }
#if HAVE_FFTW3_H
    //ArrayObj FFT( int tm, real size, string archivator = "", int tm_usec = 0, double vlOnEVAL = <EVAL> )
    //  - performs the Fast Fourier Transformation using the FFT algorithm.
    //    Returns an array of amplitudes of the frequencies for archive's values window for time <tm>:<tm_usec> (seconds:microseconds),
    //    depth to history <size> (seconds) and for archivator <archivator> with fill EVALs by <vlOnEVAL>.
    // tm, tm_usec - FFT window time (end); the curent time uses for zero <tm>;
    // size        - size in seconds to history (begin);
    // archivator  - concrete archiver select for FFT processing;
    // vlOnEVAL    - replacing EVAL value; for same <vlOnEVAL> is EVAL used previous actual value for the EVAL replace.
    if(id == "FFT" && prms.size() >= 2) {
	int64_t etm = 1000000ll * (!prms[0].getI() ? time(NULL) : prms[0].getI()) + ((prms.size()>=4) ? prms[3].getI() : 0);
	int64_t btm = etm - (int64_t)(1e6*prms[1].getR());
	int fftN = 0, iN = 0, bufEVAL = 0;
	double *fftIn = NULL, tVl,
	    vlOnEVAL = (prms.size()>=5) ? prms[4].getR() : EVAL_REAL;
	bool tracePrevNotEVAL = (vlOnEVAL == EVAL_REAL);
	if(tracePrevNotEVAL) vlOnEVAL = 0;

	TArrayObj *ao = new TArrayObj();
	if(isArch()) {
	    string archivator = (prms.size()>=3) ? prms[2].getS() : "";
	    TValBuf tb(TFld::Real, 0, arch().at().period(archivator), true, true);
	    arch().at().getVals(tb, btm, etm, archivator, 600000);
	    fftN = tb.realSize();
	    if(fftN > 10) {
		fftIn = (double*)malloc(sizeof(double)*fftN);
		for(btm = tb.begin(); btm <= tb.end() && iN < fftN; btm++, iN++)
		    if((tVl=tb.getR(&btm,true)) == EVAL_REAL) { fftIn[iN] = vlOnEVAL; bufEVAL++; }
		    else {
			fftIn[iN] = tVl;
			if(tracePrevNotEVAL) vlOnEVAL = tVl;
		    }
	    }
	}
	else if(buf() && buf()->realSize() > 10) {
	    fftN = buf()->realSize();
	    fftIn = (double*)malloc(sizeof(double)*fftN);
	    for(btm = buf()->begin(); btm <= buf()->end() && iN < fftN; btm++, iN++)
		if((tVl=buf()->getR(&btm,true)) == EVAL_REAL) { fftIn[iN] = vlOnEVAL; bufEVAL++; }
		else {
		    fftIn[iN] = tVl;
		    if(tracePrevNotEVAL) vlOnEVAL = tVl;
		}
	}

	if(fftIn) {
	    fftw_complex *fftOut = (fftw_complex*)malloc(sizeof(fftw_complex)*(fftN/2+1));
	    fftw_plan p = fftw_plan_dft_r2c_1d(fftN, fftIn, fftOut, FFTW_ESTIMATE);
	    fftw_execute(p);
	    fftw_destroy_plan(p);
	    for(int iV = 0; iV < (fftN/2+1); iV++)
		if(!iV) ao->arSet(iV, fftOut[iV][0]/fftN);
		else ao->arSet(iV, pow(pow(fftOut[iV][0],2)+pow(fftOut[iV][1],2),0.5)/(fftN/2));
	    delete fftIn;
	    delete fftOut;
	    ao->propSet("bufSize", fftN); ao->propSet("bufEVAL", bufEVAL);
	}

	return ao;
    }
#endif
    throw TError("VArchObj",_("Function '%s' error or not enough parameters."),id.c_str());
}
