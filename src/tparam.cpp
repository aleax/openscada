/***************************************************************************
 *   Copyright (C) 2004 by Roman Savochenko                                *
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

#include "tsys.h"
#include "tkernel.h"
#include "tparamcontr.h"
#include "tparams.h"
#include "tparam.h"

const char *TParam::o_name = "TParam";


TParam::TParam( TControllerS::SName cntr, const string &nm, TParamS *prms ) : 
	work(0), m_owner(prms)
{    
    m_name = nm;
    hd_res = ResAlloc::resCreate();
    reg( cntr, m_name );
}

TParam::~TParam(  )
{
    ResAlloc res(hd_res,true);
    while(PrmC.size()) PrmC.erase(PrmC.begin());	
    res.release();
    
    ResAlloc::resDelete(hd_res);
}

int TParam::reg( TControllerS::SName cntr, const string &nm )
{
    ResAlloc res(hd_res,true);
    //Check already registry parameters
    for(unsigned i_pr = 0; i_pr < PrmC.size(); i_pr++)
 	if( PrmC[i_pr].ctr.at().name() == cntr.obj &&
	    PrmC[i_pr].ctr.at().owner().modId() == cntr.tp &&
	    PrmC[i_pr].prm.at().name() == nm) return( PrmC.size() );
    //Registry parameter
    SEl prm;
    prm.ctr = ((TTipController &)owner().owner().controller().gmdAt(cntr.tp).at()).at(cntr.obj,o_name);
    prm.prm = prm.ctr.at().at(nm,o_name); 
    PrmC.push_back(prm);

    return( PrmC.size() );
} 

int TParam::unreg( TControllerS::SName cntr, const string &nm )
{
    ResAlloc res(hd_res,true);
    //Check registry parameters
    for(unsigned i_pr = 0; i_pr < PrmC.size(); i_pr++)
 	if( PrmC[i_pr].ctr.at().name() == cntr.obj &&
	    PrmC[i_pr].ctr.at().owner().modId() == cntr.tp &&
	    PrmC[i_pr].prm.at().name() == nm)
	{
	    PrmC.erase(PrmC.begin()+i_pr);
	    break;	    	    
	}
    return( PrmC.size() );	    
}

TParamContr &TParam::at()
{    
    ResAlloc res(hd_res,false);
    return( PrmC[work].prm.at() );      
}



