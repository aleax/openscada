// file "tvision.cpp"

/***************************************************************************
 *   Copyright (C) 2005 by Evgen Zaichuk                               
 *   evgen@diyaorg.dp.ua                                                     
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

#include <qtextcodec.h>
#include <qapplication.h>

#include <qmessagebox.h> 
#include <qimage.h>

#include <tsys.h>
#include <tmess.h>

#include "tvisionrun.h"
#include "tvision.h"
#include "tvisiondev.h"
#include "tconfiguration.h"
#include "tdbgw.h"

#include <stdio.h>


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
    
    modFuncReg( new ExpFunc("QMainWindow *openWindow();","Start QT GUI.",
                               (void(TModule::*)( )) &TVision::openWindow) );    
    
    //runtime = NULL;
    //development = NULL;
}

TVision::~TVision()
{
    if( run_st ) modStop();
}

void TVision::modInfo( vector<string> &list )
{
    TModule::modInfo(list);
    list.push_back("SubType");
}

string TVision::modInfo( const string &name )
{
    if( name == "SubType" ) return(SUB_TYPE);
    else return( TModule::modInfo( name) );
}


void TVision::modLoad( )
{
   //printf("TEST VISION from load 00\n");
}

void TVision::postEnable( )
{
   TModule::postEnable( );
}

QMainWindow *TVision::openWindow()
{
    printf("TEST VISION: Creating Configuration & Dev\n");
    
    cfg = new TConfiguration();
    new TDBGW(); //”œ⁄ƒ¡Œ…≈ €Ã¿⁄¡ À ‚‰
    
    development = new TVisionDev(this, cfg, NULL, NULL, Qt::WDestructiveClose);
    return development;
}
        
void TVision::modStart()
{
    run_st = true;
}

void TVision::modStop()
{
    int i_w;
    for( i_w = 0; i_w < mn_winds.size(); i_w++ )
        if( mn_winds[i_w] )	mn_winds[i_w]->close();//deleteLater();// close();
    
    do for( i_w = 0; i_w < mn_winds.size(); i_w++ ) if( mn_winds[i_w] )	break;
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

void TVision::callDevelopment()
{
}

void TVision::callRuntime()
{
   if (runtime == NULL)
    {//QMessageBox::warning( NULL, "runtime", "new TVisionRun");
     runtime = new TVisionRun(this, cfg, NULL, NULL, Qt::WDestructiveClose);
     //connect(visionRun, SIGNAL(runtimeIsClosing()), this, SLOT(runtimeIsClosed())); ÓÂ Ú·‚ÔÙ·ÂÙ ÚÈ CTRL+C
     runtime->show();
    }
    else
      {//QMessageBox::warning( NULL, "runtime", "show TVisionRun");
       runtime->show();
      }
}

void TVision::closeRuntime()
{
   runtime = NULL;
}

