// file "tvision.cpp"

/***************************************************************************
 *   Copyright (C) 2005-2006 by Roman Savochenko based on Vision of Evgen Zaichuk 2005
 *   rom_as@diyaorg.dp.ua                                                     
 *                                                                         
 *   This program is free software; you can redistribute it and/or modify  
 *   it under the terms of the GNU General Public License as published by  
 *   the Free Software Foundation; either version 2 of the License, or     
 *   (at your option) any later version.                                   
 *                                                                         
 *   This program is distributed in the hope that it will be useful,       
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         
 *   GNU General Public License for more details.                          
 *                                                                         
 *   You should have received a copy of the GNU General Public License     
 *   along with this program; if not, write to the                         
 *   Free Software Foundation, Inc.,                                       
 *   59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.             
 ***************************************************************************/

#include <getopt.h>
#include <sys/types.h>
#include <unistd.h>       

#include <tsys.h>
#include <tmess.h>

#include "vis_devel.h"
#include "tvision.h"

VISION::TVision *VISION::mod;

extern "C"
{
    TModule::SAt module( int n_mod )
    {
    	TModule::SAt AtMod;

	if(n_mod==0)
	{
	    AtMod.id	= MOD_ID;
	    AtMod.type  = MOD_TYPE;
    	    AtMod.t_ver = VER_TYPE;
	}
	else
	    AtMod.id	= "";

	return AtMod;
    }

    TModule *attach( const TModule::SAt &AtMod, const string &source )
    {
	VISION::TVision *self_addr = NULL;

	if( AtMod.id == MOD_ID && AtMod.type == MOD_TYPE && AtMod.t_ver == VER_TYPE )
	    self_addr = VISION::mod = new VISION::TVision( source );       

	return ( self_addr );
    }    
}

using namespace VISION;

//==============================================================================
//================= QTCFG::TVision =============================================
//==============================================================================

TVision::TVision( string name )
{
    mId		= MOD_ID;
    mName       = MOD_NAME;
    mType  	= MOD_TYPE;
    mVers      	= VERSION;
    mAutor    	= AUTORS;
    mDescr  	= DESCRIPTION;
    mLicense   	= LICENSE;
    mSource    	= name;
    
    modFuncReg( new ExpFunc("QMainWindow *openWindow();","Start QT GUI.",(void(TModule::*)( )) &TVision::openWindow) );
}

TVision::~TVision()
{
    
}

void TVision::modInfo( vector<string> &list )
{
    TModule::modInfo(list);
    list.push_back("SubType");
}

string TVision::modInfo( const string &name )
{
    if( name == "SubType" ) return SUB_TYPE;
    else return TModule::modInfo( name);
}


void TVision::modLoad( )
{
    
}

void TVision::postEnable( )
{
   TModule::postEnable( );
}

QMainWindow *TVision::openWindow()
{
    return new VisDevelop();
}
        
void TVision::modStart()
{
    run_st = true;
}

void TVision::modStop()
{
    if(run_st)	return;
    int i_w;
    for( i_w = 0; i_w < mn_winds.size(); i_w++ )
        if( mn_winds[i_w] ) emit mn_winds[i_w]->close();//deleteLater();// close();
    
    do 
	for( i_w = 0; i_w < mn_winds.size(); i_w++ ) 
	    if( mn_winds[i_w] )	break;
    while(i_w<mn_winds.size());
    struct timespec tm = {0,500000000};
    nanosleep(&tm,NULL);    
    
    run_st = false;
}

void TVision::regWin( QMainWindow *mwd )
{
    int i_w;
    for( i_w = 0; i_w < mn_winds.size(); i_w++ )
	if( mn_winds[i_w] == NULL ) break;
    if( i_w == mn_winds.size() ) mn_winds.push_back(NULL);
    mn_winds[i_w] = mwd;
}
			
void TVision::unregWin( QMainWindow *mwd )
{
    for( int i_w = 0; i_w < mn_winds.size(); i_w++ )
        if( mn_winds[i_w] == mwd ) mn_winds[i_w] = NULL;
}

