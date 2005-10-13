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

#include <tsys.h>

#include "os_contr.h"
#include "da_smart.h"

using namespace SystemCntr;

//======================================================================
//==== HddTemp
//======================================================================
Hddtemp::Hddtemp( ) : err_st(false)
{    
    //HDD value structure
    fldAdd( new TFld("disk",mod->I18N("Name"),TFld::String,FLD_NWR) );
    fldAdd( new TFld("ed",mod->I18N("Measure unit"),TFld::String,FLD_NWR) );
    fldAdd( new TFld("value",mod->I18N("Temperature"),TFld::Dec,FLD_NWR,"3","0") );    
}

Hddtemp::~Hddtemp()
{
    ((TTipTransport &)SYS->transport().at().modAt(t_tr).at()).outDel(n_tr);
}

void Hddtemp::init( TMdPrm *prm )
{
    t_tr = "socket";
    n_tr = "tr_"+prm->name();
	
    ((TTipTransport &)SYS->transport().at().modAt(t_tr).at()).outAdd(n_tr);
    (((TTipTransport &)SYS->transport().at().modAt(t_tr).at()).outAt(n_tr)).at().lName() = mod->I18N("Parametr Hddtemp");
    (((TTipTransport &)SYS->transport().at().modAt(t_tr).at()).outAt(n_tr)).at().address() = "TCP:127.0.0.1:7634";
    (((TTipTransport &)SYS->transport().at().modAt(t_tr).at()).outAt(n_tr)).at().start();	
    
    TCfg &c_subt = prm->cfg("SUBT");
    
    //Create Config
    c_subt.fld().descr() = mod->I18N("Disk");
    c_subt.fld().selValI().clear();
    c_subt.fld().selNm().clear();    

    vector<string> list;
    dList(list);
    for( int i_l = 0; i_l < list.size(); i_l++ )
    {
	c_subt.fld().selValI().push_back(i_l);
	c_subt.fld().selNm().push_back(list[i_l]);
    }
    if( list.size() ) c_subt.setSEL(list[0]);    
}

void Hddtemp::dList( vector<string> &list )
{    
    int  len;
    char buf[20];
    string val;
    try 
    { 
	len = (((TTipTransport &)SYS->transport().at().modAt(t_tr).at()).outAt(n_tr)).at().messIO("1",1,buf,sizeof(buf),1); 
	buf[len] = '\0';	
	err_st = false;	
	val.append(buf,len);
	while( len == sizeof(buf) )
	{
	    len = (((TTipTransport &)SYS->transport().at().modAt(t_tr).at()).outAt(n_tr)).at().messIO(NULL,0,buf,sizeof(buf),1); 
	    buf[len] = '\0';
	    val.append(buf,len);
	}
	
	len = -1;
	do
	{	    	    
	    int l_nm;
	    string val_t;
	    
	    //Get one disk
	    len += 1;	    
	    l_nm = val.find("||",len);
	    if( l_nm == string::npos )  val_t = val.substr(len);
	    else	    		val_t = val.substr(len,l_nm-len+1);
	    len = l_nm;
	    //Parse geted disk
	    l_nm = val_t.find("|",1);
	    if( l_nm != string::npos ) list.push_back( val_t.substr(1,l_nm-1) );
	}while( len != string::npos );
    }
    catch( TError err ) 
    { 
	if( !err_st ) Mess->put(err.cat.c_str(),TMess::Error,err.mess.c_str()); 
	err_st = true;
    }
}

void Hddtemp::getVal( TMdPrm *prm )
{    
    int  len;
    char buf[20];
    string val;
    try 
    { 
       	string dev = prm->cfg("SUBT").getSEL();
	
	len = (((TTipTransport &)SYS->transport().at().modAt(t_tr).at()).outAt(n_tr)).at().messIO("1",1,buf,sizeof(buf),1);
	err_st = false;
	val.append(buf,len);
	while( len == sizeof(buf) )
	{
	    len = (((TTipTransport &)SYS->transport().at().modAt(t_tr).at()).outAt(n_tr)).at().messIO(NULL,0,buf,sizeof(buf),1);
	    val.append(buf,len);
	}

	len = -1;
	do
	{	    
	    int l_nm;
	    string val_t;
	    
	    len += 1;
	    l_nm = val.find("||",len);
	    if( l_nm == string::npos )  val_t = val.substr(len);
	    else	    		val_t = val.substr(len,l_nm-len+1);
	    len = l_nm;
	    
	    l_nm = val_t.find("|",1);
	    if( l_nm != string::npos && val_t.substr(1,l_nm-1) == dev ) 
	    {
		int l_nm1 = l_nm + 1;
		
		l_nm = val_t.find("|",l_nm1)+1;
		prm->vlAt("disk").at().setS( val_t.substr(l_nm1,l_nm-l_nm1-1), NULL, true ); 
		l_nm1 = l_nm;
		
		l_nm = val_t.find("|",l_nm1)+1; 
		prm->vlAt("value").at().setI( atoi(val_t.substr(l_nm1,l_nm-l_nm1-1).c_str()), NULL, true );
		l_nm1 = l_nm;
		
		l_nm = val_t.find("|",l_nm1)+1; 
		prm->vlAt("ed").at().setS( val_t.substr(l_nm1,l_nm-l_nm1-1), NULL, true ); 
	    }
	}while( len != string::npos );
    }    
    catch( TError err ) 
    {
	if( !err_st ) Mess->put(err.cat.c_str(),TMess::Error,err.mess.c_str());
	err_st = true;
    }
}

void Hddtemp::chCfg( TMdPrm *prm, TCfg &i_cfg )
{

}

