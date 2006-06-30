
//OpenSCADA system module Archive.BaseArh file: val.cpp
/***************************************************************************
 *   Copyright (C) 2003-2006 by Roman Savochenko                           *
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

#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <dirent.h>
#include <math.h>

#include <tsys.h>
#include <resalloc.h>
#include <tmess.h>
#include "base.h"
#include "val.h"

using namespace BaseArch;

//*** BaseArch::ModVArch - Value archivator ***
ModVArch::ModVArch( const string &iid, const string &idb, TElem *cf_el ) : 
    TVArchivator(iid,idb,cf_el), m_lst_check(0),
    time_size(cfg("BaseArhTmSize").getRd()), numb_files(cfg("BaseArhNFiles").getId()), 
    round_proc(cfg("BaseArhRound").getRd()), m_chk_tm(cfg("BaseArhTm").getId()), 
    m_pack_tm(cfg("BaseArhPackTm").getId())
{

}

ModVArch::~ModVArch( )
{
    try{ stop(); }catch(...){}
}

void ModVArch::start()
{
    //- Start getting data cycle -
    TVArchivator::start();
    
    //- First scan dir. Load and connect archive files -
    checkArchivator();
}

void ModVArch::stop()
{
    //- Stop getting data cicle an detach archives -
    TVArchivator::stop();
}

bool ModVArch::filePrmGet( const string &anm, string *archive, TFld::Type *vtp, long long *abeg, long long *aend, long long *aper )
{
    bool unpck = false;
    string a_fnm = anm;
    if( mod->filePack(anm) )
    {
	//Get file info from DB
	TConfig c_el(&mod->packFE());
	c_el.cfg("FILE").setS(anm);
	if(SYS->db().at().dataGet(mod->filesDB(),mod->nodePath()+"Pack/",c_el))
	{
	    if( abeg ) 		*abeg = strtoll(c_el.cfg("BEGIN").getS().c_str(),NULL,16);
	    if( aend ) 		*aend = strtoll(c_el.cfg("END").getS().c_str(),NULL,16);
	    if( archive ) 	*archive = c_el.cfg("PRM1").getS();
	    if( aper )		*aper = strtoll(c_el.cfg("PRM2").getS().c_str(),NULL,16);
	    if( vtp )		*vtp  = (TFld::Type)atoi(c_el.cfg("PRM3").getS().c_str());
	    return true;
	}
	a_fnm = mod->unPackArch(anm,false);
        unpck = true;
    }
    //Get params from file
    int hd = open(a_fnm.c_str(),O_RDONLY);
    if( hd <= 0 )   return false;
    //---- Read Header ----
    VFileArch::FHead head;
    read(hd,&head,sizeof(VFileArch::FHead));
    close(hd);
    if( VFileArch::afl_id != head.f_tp || head.term != 0x55 )	
	return false;
    //---- Check to archive present ----
    if( archive )       *archive = head.archive;
    if( abeg )          *abeg = head.beg;
    if( aend )          *aend = head.end;
    if( aper )		*aper = head.period;
    if( vtp )		*vtp  = (TFld::Type)head.vtp;

    if( unpck ) 
    {
	remove(a_fnm.c_str());
	//Write info to DB
	TConfig c_el(&mod->packFE());
	c_el.cfg("FILE").setS(anm);
	c_el.cfg("BEGIN").setS(TSYS::ll2str(head.beg,TSYS::Hex));
	c_el.cfg("END").setS(TSYS::ll2str(head.end,TSYS::Hex));
	c_el.cfg("PRM1").setS(head.archive);
	c_el.cfg("PRM2").setS(TSYS::ll2str(head.period,TSYS::Hex));
	c_el.cfg("PRM3").setS(TSYS::int2str(head.vtp));
	SYS->db().at().dataSet(mod->filesDB(),mod->nodePath()+"Pack/",c_el);
    }
    
    return true;
}

void ModVArch::checkArchivator( bool now )
{
    if( now || time(NULL) > m_lst_check + checkTm()*60 )
    {
	//- Find archive files for no present archives and create it. -
	struct stat file_stat;
	dirent *scan_dirent;
    
	//-- Open/create new directory --
	DIR *IdDir = opendir(addr().c_str());
	if(IdDir == NULL) 
	{
    	    if( mkdir(addr().c_str(),0777) ) throw TError(nodePath().c_str(),mod->I18N("Can not create dir <%s>."),addr().c_str());
    	    IdDir = opendir(addr().c_str());
	}
    
	//-- Scan opened directory --
	while((scan_dirent = readdir(IdDir)) != NULL)
	{
	    if( string("..") == scan_dirent->d_name || string(".") == scan_dirent->d_name ) continue;
	
	    string 	   ArhNm;
	    TFld::Type ArhTp;
	    string NameArhFile = addr()+"/"+scan_dirent->d_name;	
	
	    stat(NameArhFile.c_str(),&file_stat);        
	    if( (file_stat.st_mode&S_IFMT) != S_IFREG || access(NameArhFile.c_str(),F_OK|R_OK) != 0 ) continue;
	
	    if(!filePrmGet(NameArhFile,&ArhNm,&ArhTp,NULL,NULL,NULL))	continue;
	
	    //---- Check to archive present ----
	    if( !owner().owner().valPresent(ArhNm) )
	    {
		//----- Add no present archive -----
		owner().owner().valAdd(ArhNm);
		owner().owner().valAt(ArhNm).at().toStart(true);
		owner().owner().valAt(ArhNm).at().valType(ArhTp);
		owner().owner().valAt(ArhNm).at().start();
	    }	
	    //---- Check for attached ----	
    	    if( !owner().owner().valAt(ArhNm).at().archivatorPresent(workId()) )
		owner().owner().valAt(ArhNm).at().archivatorAttach(workId());
	}
    
	closedir(IdDir);
	now = true;
    }

    //-- Scan files of attached archives --
    ResAlloc res(a_res,false);
    for( int i_l = 0; i_l < arch_el.size(); i_l++ )
        ((ModVArchEl *)arch_el[i_l])->checkArchivator(now);
}

void ModVArch::expArch(const string &arch_nm, time_t beg, time_t end, const string &file_nm)
{
    //Export to wav
    struct
    {
	char riff[4];
        int  filesize;
	char rifftype[4];
    } rif;
    struct 
    {
        char chunk_id[4];
        int  chunksize;
    } chnk;
    struct 
    {
	short wFormatTag;
        short nChannels;
	int   nSamplesPerSec;
	int   nAvgBytesPerSec;
	short nBlockAlign;
	short wBitsPerSample;
    } wv_form;
    
    TValBuf buf( TFld::Real, 100000, (long long)(valPeriod()*1000000.), true, true );
    SYS->archive().at().valAt(arch_nm).at().getVal(buf,(long long)beg*1000000,(long long)end*1000000,workId());
    
    strncpy(rif.riff,"RIFF",4);
    rif.filesize=buf.realSize()*sizeof(float)+sizeof(rif)+2*sizeof(chnk)+sizeof(wv_form);
    strncpy(rif.rifftype,"WAVE",4);
    strncpy(chnk.chunk_id,"fmt ",4); 
    chnk.chunksize = sizeof(wv_form);
    wv_form.wFormatTag = 3; 
    wv_form.nChannels = 1;
    wv_form.nSamplesPerSec = 1000000/buf.period(); 
    wv_form.nAvgBytesPerSec = wv_form.nSamplesPerSec;
    wv_form.nBlockAlign = 4; 
    wv_form.wBitsPerSample=32;
    
    int hd=open(file_nm.c_str(),O_RDWR|O_CREAT|O_TRUNC, 0666);
    if( hd == -1 ) return;
    write(hd,&rif,sizeof(rif));
    write(hd,&chnk,sizeof(chnk));
    write(hd,&wv_form,sizeof(wv_form));
    strncpy(chnk.chunk_id,"data",4); 
    chnk.chunksize = buf.realSize()*sizeof(float);
    write(hd,&chnk,sizeof(chnk));
    
    //Check scale
    long long c_tm;
    float c_val, v_max=-1e30, v_min=1e30;
    for(c_tm = buf.begin();c_tm <= buf.end();c_tm++)
    {
	c_val = buf.getR(&c_tm,true);
	v_max=vmax(c_val,v_max);
	v_min=vmin(c_val,v_min);
    }
    float v_over = (v_max+v_min)/2;
    //Transver value
    for(c_tm = buf.begin();c_tm <= buf.end();c_tm++)
    {
	c_val = 2.*(buf.getR(&c_tm,true)-v_over)/(v_max-v_min);
	//printf("TEST 00: %f\n",c_val);
	write(hd,&c_val,sizeof(float));    
    }
    close(hd);
}

TVArchEl *ModVArch::getArchEl( TVArchive &arch )
{   
    ModVArchEl *v_el = new ModVArchEl(arch,*this); 
    v_el->checkArchivator();
    return v_el;
}

void ModVArch::cntrCmd_( const string &a_path, XMLNode *opt, TCntrNode::Command cmd )
{
    if( cmd==TCntrNode::Info )
    {
	int my_gr = owner().owner().subSecGrp();
	TVArchivator::cntrCmd_( a_path, opt, cmd );       //Call parent
	 
	ctrMkNode("area",opt,1,a_path.c_str(),"/bs",mod->I18N("Additional options"),0444,0,my_gr);
	ctrMkNode("fld",opt,-1,a_path.c_str(),"/bs/tm",cfg("BaseArhTmSize").fld().descr(),0664,0,my_gr,1,"tp","real");
	ctrMkNode("fld",opt,-1,a_path.c_str(),"/bs/fn",cfg("BaseArhNFiles").fld().descr(),0664,0,my_gr,1,"tp","dec");
	ctrMkNode("fld",opt,-1,a_path.c_str(),"/bs/round",cfg("BaseArhRound").fld().descr(),0664,0,my_gr,1,"tp","real");
	ctrMkNode("fld",opt,-1,a_path.c_str(),"/bs/pcktm",cfg("BaseArhPackTm").fld().descr(),0664,0,my_gr,1,"tp","dec");
	ctrMkNode("fld",opt,-1,a_path.c_str(),"/bs/tmout",cfg("BaseArhTm").fld().descr(),0664,0,my_gr,1,"tp","dec");
	ctrMkNode("comm",opt,-1,a_path.c_str(),"/bs/chk_nw",mod->I18N("Check archivator directory now"),0440,0,my_gr);	
	ctrMkNode("list",opt,-1,a_path.c_str(),"/arch/arch/3",mod->I18N("Files size (Mb)"),0444,0,0,1,"tp","real");
	ctrMkNode("comm",opt,-1,a_path.c_str(),"/arch/exp",mod->I18N("Export"),0440);
	ctrMkNode("fld",opt,-1,a_path.c_str(),"/arch/exp/arch",mod->I18N("Archive"),0660,0,0,3,"tp","str","dest","select","select","/arch/lst");
	ctrMkNode("fld",opt,-1,a_path.c_str(),"/arch/exp/beg",mod->I18N("Begin"),0660,0,0,1,"tp","time");
	ctrMkNode("fld",opt,-1,a_path.c_str(),"/arch/exp/end",mod->I18N("End"),0660,0,0,1,"tp","time");
	ctrMkNode("fld",opt,-1,a_path.c_str(),"/arch/exp/file",mod->I18N("To file"),0660,0,0,1,"tp","str");
    }
    else if( cmd==TCntrNode::Get )
    {
	if( a_path == "/bs/tm" )		ctrSetR( opt, time_size );
	else if( a_path == "/bs/fn" )		ctrSetI( opt, numb_files );
	else if( a_path == "/bs/round" )	ctrSetR( opt, round_proc );
	else if( a_path == "/bs/pcktm" )	ctrSetI( opt, m_pack_tm );
	else if( a_path == "/bs/tmout" )	ctrSetI( opt, m_chk_tm );
        else if( a_path == "/arch/arch" )
        {
            //Fill Archives table
            XMLNode *n_arch = ctrId(opt,"0");
            XMLNode *n_per  = ctrId(opt,"1");
            XMLNode *n_size = ctrId(opt,"2");
	    XMLNode *f_size = ctrId(opt,"3");
						
    	    ResAlloc res(a_res,false);
            for( int i_l = 0; i_l < arch_el.size(); i_l++ )
    	    {
                ctrSetS(n_arch,arch_el[i_l]->archive().id());
                ctrSetR(n_per,(double)arch_el[i_l]->archive().period()/1000000.);
        	ctrSetI(n_size,arch_el[i_l]->archive().size());
		ctrSetR(f_size,(double)((ModVArchEl *)arch_el[i_l])->size()/1024.);
	    }
        }
	else if( a_path == "/arch/lst" )
	{
	    vector<string> a_ls;
	    archiveList(a_ls);
	    opt->childClean();
	    for( int i_el = 0; i_el < a_ls.size(); i_el++ )
		ctrSetS( opt, a_ls[i_el].c_str() );	
	}
	else TVArchivator::cntrCmd_( a_path, opt, cmd );
    }
    else if( cmd==TCntrNode::Set )
    {
	if( a_path == "/bs/tm" )        	time_size  = ctrGetR( opt );
        else if( a_path == "/bs/fn" )   	numb_files = ctrGetI( opt );
	else if( a_path == "/bs/round" )	round_proc = ctrGetR( opt );
	else if( a_path == "/bs/pcktm" )        m_pack_tm  = ctrGetI( opt );
	else if( a_path == "/bs/tmout" )	m_chk_tm   = ctrGetI( opt );
	else if( a_path == "/bs/chk_nw" )       checkArchivator(true);
	else if( a_path == "/arch/exp" )
	    expArch(ctrGetS(ctrId(opt,"arch")),ctrGetI(ctrId(opt,"beg")),ctrGetI(ctrId(opt,"end")),ctrGetS(ctrId(opt,"file")));	    
	else TVArchivator::cntrCmd_( a_path, opt, cmd );
    }
}

//*** BaseArch::ModVArchEl - Value archive element ***
ModVArchEl::ModVArchEl( TVArchive &iachive, TVArchivator &iarchivator ) :
    TVArchEl(iachive,iarchivator), prev_tm(0)
{
    m_res = ResAlloc::resCreate( );
}

ModVArchEl::~ModVArchEl( )
{
    //- Clear a files list -
    ResAlloc res(m_res,true);
    while(arh_f.size())
    {
	delete arh_f[0];
	arh_f.pop_front();
    }
    res.release();
	
    //- Delete resource -
    ResAlloc::resDelete( m_res );
}

void ModVArchEl::fullErase()
{
    //- Remove archive's files -
    ResAlloc res(m_res,true);
    while(arh_f.size())
    {
	arh_f[0]->delFile();
        delete arh_f[0];
        arh_f.pop_front();
    }
    res.release();
}

int ModVArchEl::size()
{
    int rez = 0;
    ResAlloc res(m_res,false);
    for( int i_arh = 0; i_arh < arh_f.size(); i_arh++)    
        rez += arh_f[i_arh]->size()/1024;
    
    return rez;	
}

void ModVArchEl::checkArchivator( bool now )
{
    if( now )
    {
	//- Scan directory for find new files and deleted files -
	struct stat file_stat;
	dirent *scan_dirent;
    
	//-- Open archive derictory --
	DIR *IdDir = opendir(archivator().addr().c_str());
	if(IdDir == NULL) return;
    
	//-- Clean scan flag --
	ResAlloc res(m_res,false);
	for( unsigned i_arh = 0; i_arh < arh_f.size(); i_arh++) 
	    arh_f[i_arh]->scan = false;
	res.release();
    
	//-- Check to allow files --
	while((scan_dirent = readdir(IdDir)) != NULL)
	{
	    if( string("..") == scan_dirent->d_name || string(".") == scan_dirent->d_name ) continue;
	
	    string     ArhNm;
	    string NameArhFile = archivator().addr()+"/"+scan_dirent->d_name;	
	
	    stat(NameArhFile.c_str(),&file_stat);        
	    if( (file_stat.st_mode&S_IFMT) != S_IFREG || access(NameArhFile.c_str(),F_OK|R_OK) != 0) continue;
	
	    if(!archivator().filePrmGet(NameArhFile,&ArhNm,NULL,NULL,NULL,NULL))    continue;
	
	    if( archive().id() != ArhNm )	continue;
	
	    //---- Check to present archive files ----
	    int i_arh;
    	    res.request(false);
    	    for( i_arh = 0; i_arh < arh_f.size(); i_arh++)
		if( arh_f[i_arh]->name() == NameArhFile )
		{
		    arh_f[i_arh]->scan = true;
		    res.release();
		    break;
		}
		
	    //----- Attach a new archive file -----
	    if( i_arh >= arh_f.size() )
	    {		
		res.release();
	    
		VFileArch *f_arh = new VFileArch(this);
		f_arh->attach( NameArhFile );
		f_arh->scan = true;
		
		//------ Broken archives delete. Oldest arhives to up. ------
		if( f_arh->err() )	{ delete f_arh; continue; }	//arh_f.push_front( f_arh );
		else
		{
		    res.request(true);
		    int i_arh;
		    for( i_arh = arh_f.size()-1; i_arh >= 0 ; i_arh--)
			if( arh_f[i_arh]->err() || f_arh->begin() >= arh_f[i_arh]->begin() ) 
			{ 
			    arh_f.insert(arh_f.begin()+i_arh+1,f_arh); 
			    break; 
			}
		    if( i_arh >= arh_f.size() ) arh_f.push_front( f_arh );
		    res.release();
		}
	    }
	}
    
	closedir(IdDir);
    
	//-- Check to deleting archive files --
	res.request(true);
	for( unsigned i_arh = 0; i_arh < arh_f.size(); i_arh++) 
	    if( !arh_f[i_arh]->scan )
	    {
		delete arh_f[i_arh];
		arh_f.erase( arh_f.begin() + i_arh );
		i_arh--;
	    }	
	res.release();

	//-- Check file count for delete old files --
	if( ((ModVArch &)archivator()).fileNumber() )
    	    if( arh_f.size() > ((ModVArch &)archivator()).fileNumber() )
        	for( int i_arh = 0; i_arh < arh_f.size(); i_arh++ )
            	    if( arh_f.size() <= ((ModVArch &)archivator()).fileNumber() )     break;
            	    else if( !arh_f[i_arh]->err() )
            	    {
                	string f_nm = arh_f[i_arh]->name();
                	delete arh_f[i_arh];
                	arh_f.erase( arh_f.begin() + i_arh );
	        	remove(f_nm.c_str());
	        	i_arh--;
            	    }
    }
    
    //Check the archive's files
    ResAlloc res(m_res,false);
    for( int i_arh = 0; i_arh < arh_f.size(); i_arh++)
        arh_f[i_arh]->check();
}

long long ModVArchEl::end()
{
    ResAlloc res(m_res,false);
    for( int i_a = arh_f.size()-1; i_a >= 0; i_a-- )
	if( !arh_f[i_a]->err() )
	    return arh_f[i_a]->end();
	    
    return 0;
}

long long ModVArchEl::begin()
{
    ResAlloc res(m_res,false);
    for( int i_a = 0; i_a < arh_f.size(); i_a++ )
        if( !arh_f[i_a]->err() )
            return arh_f[i_a]->begin();
	    
    return 0;
}

void ModVArchEl::getVal( TValBuf &buf, long long beg, long long end )
{
    long long n_end, n_beg;
    
    ResAlloc res(m_res,false);
    for( int i_a = 0; i_a < arh_f.size(); i_a++ )
	if( beg >= end ) break;
	else if( !arh_f[i_a]->err() && 
	    beg <= arh_f[i_a]->end() && end > arh_f[i_a]->begin() )
        {
    	    n_end = vmin(end,arh_f[i_a]->end());
	    n_beg = vmax(beg,arh_f[i_a]->begin());
            arh_f[i_a]->getVal(buf,n_beg,n_end);
    	    beg = n_end;
	}
}

string ModVArchEl::getS( long long *tm, bool up_ord )
{   
    long long itm = tm?*tm:SYS->curTime();
    ResAlloc res(m_res,false);
    for( int i_a = 0; i_a < arh_f.size(); i_a++ )
        if( !arh_f[i_a]->err() && (
		(up_ord && itm <= arh_f[i_a]->end() && itm > arh_f[i_a]->begin()-arh_f[i_a]->period()) ||
	        (!up_ord && itm < arh_f[i_a]->end()+arh_f[i_a]->period() && itm >= arh_f[i_a]->begin()) ) )
	    return arh_f[i_a]->getS(up_ord?arh_f[i_a]->maxPos()-(arh_f[i_a]->end()-itm)/arh_f[i_a]->period():(itm-arh_f[i_a]->begin())/arh_f[i_a]->period());
    return EVAL_STR;
}

double ModVArchEl::getR( long long *tm, bool up_ord )
{
    long long itm = tm?*tm:SYS->curTime();
    ResAlloc res(m_res,false);
    for( int i_a = 0; i_a < arh_f.size(); i_a++ )
        if( !arh_f[i_a]->err() && (
		(up_ord && itm <= arh_f[i_a]->end() && itm > arh_f[i_a]->begin()-arh_f[i_a]->period()) ||
	        (!up_ord && itm < arh_f[i_a]->end()+arh_f[i_a]->period() && itm >= arh_f[i_a]->begin()) ) )
	    return arh_f[i_a]->getR(up_ord?arh_f[i_a]->maxPos()-(arh_f[i_a]->end()-itm)/arh_f[i_a]->period():(itm-arh_f[i_a]->begin())/arh_f[i_a]->period());
    return EVAL_REAL;
}

int ModVArchEl::getI( long long *tm, bool up_ord )
{
    long long itm = tm?*tm:SYS->curTime();
    ResAlloc res(m_res,false);
    for( int i_a = 0; i_a < arh_f.size(); i_a++ )
        if( !arh_f[i_a]->err() && (
		(up_ord && itm <= arh_f[i_a]->end() && itm > arh_f[i_a]->begin()-arh_f[i_a]->period()) ||
	        (!up_ord && itm < arh_f[i_a]->end()+arh_f[i_a]->period() && itm >= arh_f[i_a]->begin()) ) )
	    return arh_f[i_a]->getI(up_ord?arh_f[i_a]->maxPos()-(arh_f[i_a]->end()-itm)/arh_f[i_a]->period():(itm-arh_f[i_a]->begin())/arh_f[i_a]->period());
    return EVAL_INT;
}

char ModVArchEl::getB( long long *tm, bool up_ord )
{
    long long itm = tm?*tm:SYS->curTime();
    ResAlloc res(m_res,false);
    for( int i_a = 0; i_a < arh_f.size(); i_a++ )
        if( !arh_f[i_a]->err() && (
		(up_ord && itm <= arh_f[i_a]->end() && itm > arh_f[i_a]->begin()-arh_f[i_a]->period()) ||
	        (!up_ord && itm < arh_f[i_a]->end()+arh_f[i_a]->period() && itm >= arh_f[i_a]->begin()) ) )
	    return arh_f[i_a]->getB(up_ord?arh_f[i_a]->maxPos()-(arh_f[i_a]->end()-itm)/arh_f[i_a]->period():(itm-arh_f[i_a]->begin())/arh_f[i_a]->period());
    return EVAL_BOOL;
}

void ModVArchEl::setVal( TValBuf &buf, long long beg, long long end )
{    
    //- Check border -
    if( !buf.vOK(beg,end) )	return;
    if( beg < buf.begin() ) 	beg = buf.begin();
    if( end > buf.end() )	end = buf.end();
    
    //- Put values to files -
    ResAlloc res(m_res,false);
    for( int i_a = 0; i_a < arh_f.size(); i_a++ )
 	if( !arh_f[i_a]->err() && beg <= end )
	{
	    //-- Check begin to creating want --
	    if( beg < arh_f[i_a]->begin() )
	    {
		//--- Create new file(s) for old data ---
		char c_buf[30];
		time_t tm = beg/1000000;
		strftime(c_buf,sizeof(c_buf)," %F %T.val",localtime(&tm));
		string AName = archivator().addr()+"/"+archive().id()+c_buf;
		
		long long n_end;	//New file end position
		if( (arh_f[i_a]->begin()-beg) > (long long)((ModVArch &)archivator()).fileTimeSize()*60.*60.*1000000.)
		    n_end = beg+(long long)(((ModVArch &)archivator()).fileTimeSize()*60.*60.*1000000.);
		else n_end = arh_f[i_a]->begin();
		
		res.request(true);		
		arh_f.insert(arh_f.begin()+i_a,new VFileArch(AName,beg,n_end,(long long)(archivator().valPeriod()*1000000.),archive().valType(),this) );
	        res.request(false);
	    }
	    //-- Insert values to archive --
	    if( beg <= arh_f[i_a]->end() && end >= arh_f[i_a]->begin() )
	    {
		long long n_end = (end > arh_f[i_a]->end())?arh_f[i_a]->end():end;
		arh_f[i_a]->setVal(buf,beg,n_end);
		beg = n_end+(long long)(archivator().valPeriod()*1000000.);
	    }
	}
    //-- Check end to creating want --
    while( end >= beg )
    {
	res.request(true);
	//-- Create new file(s) for new data --
	char c_buf[30];
	time_t tm = beg/1000000;
	strftime(c_buf,sizeof(c_buf)," %F %T.val",localtime(&tm));
	string AName = archivator().addr()+"/"+archive().id()+c_buf;
	
	long long n_end = beg+(long long)(((ModVArch &)archivator()).fileTimeSize()*60.*60.*1000000.);
	arh_f.push_back(new VFileArch(AName,beg,n_end,(long long)(archivator().valPeriod()*1000000.),archive().valType(),this) );
	n_end = (end > n_end)?n_end:end;
	arh_f[arh_f.size()-1]->setVal(buf,beg,n_end);
	beg = n_end+(long long)(archivator().valPeriod()*1000000.);
    }	    
}

//*** BaseArch::VFileArch - Value archivator file ***
string VFileArch::afl_id = "OpenSCADA Val Arch.";

VFileArch::VFileArch( ModVArchEl *owner ) : 
    m_owner(owner), m_err(true), m_size(0), m_beg(0), m_end(0), m_per(1000000), m_pack(false),
    m_tp(TFld::Real), fixVl(true), vSize(sizeof(double)), mpos(0)
{
    m_res = ResAlloc::resCreate( );
    cach_pr_rd.pos = cach_pr_rd.off = cach_pr_wr.pos = cach_pr_wr.off = 0;
    m_acces = time(NULL);
}


VFileArch::VFileArch( const string &iname, long long ibeg, long long iend, long long iper, TFld::Type itp, ModVArchEl *iowner) :
    m_owner(iowner), m_err(false), m_size(0), m_name(iname), m_beg(ibeg), m_end(iend), m_pack(false),
    m_per(iper), m_tp(itp)
{
    char buf[1000];
    m_res = ResAlloc::resCreate( );    
    cach_pr_rd.pos = cach_pr_rd.off = cach_pr_wr.pos = cach_pr_wr.off = 0;
    
    //- Round-up begin and end position to period -
    m_beg = (m_beg/m_per)*m_per;
    m_end = (m_end/m_per)*m_per+m_per;
    
    //- Open/create new archive file -
    int hd = open( name().c_str(),O_RDWR|O_CREAT|O_TRUNC, 0666 );
    if(hd <= 0) throw TError(owner().archivator().nodePath().c_str(),mod->I18N("Can not create file: <%s>!"),name().c_str());

    //- Prepare and write the file archive header -
    FHead head;
    memset(&head,0,sizeof(FHead));
    strncpy(head.f_tp,afl_id.c_str(),sizeof(head.f_tp));
    strncpy(head.archive,owner().archive().id().c_str(),sizeof(head.archive));
    head.beg = begin();
    head.end = end();
    head.period = period();
    head.vtp = type();
    head.hgrid = owner().archive().hardGrid();
    head.hres = owner().archive().highResTm();
    head.term = 0x55;
    write(hd,&head,sizeof(FHead));

    //- Create bit table and init first value -
    mpos = (end()-begin())/period();        
    switch(type())
    {
	case TFld::String:
    	{
	    fixVl = false;
	    vSize = sizeof(char);
	    string s_val = EVAL_STR;
	    eVal  = s_val;
	    
	    //Init pack table
	    int i_buf = 0;
	    for( int i_bt=0; i_bt < mpos*vSize; i_bt++, i_buf++ )
	    {		
		if( i_buf == sizeof(buf) ) { write(hd,buf,i_buf); i_buf = 0; }
		buf[i_buf] = i_bt?0:strlen(EVAL_STR);
	    }
	    if(i_buf)	write(hd,buf,i_buf);
	    
	    write(hd,s_val.c_str(),s_val.size());
	    break;
	}
	case TFld::Dec: case TFld::Oct: case TFld::Hex:
	{
	    fixVl = true;
	    vSize = sizeof(int);
	    int s_val = EVAL_INT;
	    eVal.assign((char*)&s_val,vSize);
	    
	    int i_buf = 0;
	    for( int i_bt = 0; i_bt < (mpos/8+(bool)(mpos%8)); i_bt++, i_buf++ )
	    {
		if( i_buf == sizeof(buf) ) { write(hd,buf,i_buf); i_buf = 0; }
		buf[i_buf] = i_bt?0:1;
	    }
	    if(i_buf)	write(hd,buf,i_buf);
	    
	    write(hd,&s_val,sizeof(s_val));
	    break;
	}
	case TFld::Real:
	{	    
	    fixVl = true;
	    vSize = sizeof(double);
	    double s_val = EVAL_REAL;
	    eVal.assign((char*)&s_val,vSize);

	    int i_buf = 0;
	    for( int i_bt = 0; i_bt < (mpos/8+(bool)(mpos%8)); i_bt++, i_buf++ )
	    {
		if( i_buf == sizeof(buf) ) { write(hd,buf,i_buf); i_buf = 0; }
		buf[i_buf] = i_bt?0:1;
	    }
	    if(i_buf)	write(hd,buf,i_buf);
	    
	    write(hd,&s_val,sizeof(s_val));
	    break;
	}
	case TFld::Bool:
	{
	    fixVl = true;
	    vSize = sizeof(char);
	    char s_val = EVAL_BOOL;
	    eVal.assign((char*)&s_val,vSize);
	    
	    int i_buf = 0;
	    for( int i_bt = 0; i_bt < (mpos/8+(bool)(mpos%8)); i_bt++, i_buf++ )
	    {
		if( i_buf == sizeof(buf) ) { write(hd,buf,i_buf); i_buf = 0; }
		buf[i_buf] = i_bt?0:1;
	    }
	    if(i_buf)	write(hd,buf,i_buf);	    

	    write(hd,&s_val,sizeof(s_val));
	    break;
	}
    }
    m_size = lseek(hd,0,SEEK_END);
    close(hd);
    m_acces = time(NULL);
}

VFileArch::~VFileArch()
{
    ResAlloc::resDelete( m_res );       
}

void VFileArch::delFile()
{
    remove(name().c_str());
}

void VFileArch::attach( const string &name )
{
    try
    {
	ResAlloc res(m_res,true);
    
	m_name = name;
	m_acces = time(NULL);
    
	m_pack = mod->filePack(m_name);
	m_err  = !owner().archivator().filePrmGet(m_name,NULL,&m_tp,&m_beg,&m_end,&m_per);
	
	//Init values type parameters
	switch(type())
	{
	    case TFld::String:
	    {
		fixVl = false; 
		vSize = sizeof(char);
		eVal = EVAL_STR; 
		break;
	    }	
	    case TFld::Dec: case TFld::Oct: case TFld::Hex:
	    {
		fixVl = true; 
		vSize = sizeof(int);
		int s_val = EVAL_INT;
		eVal.assign((char*)&s_val,vSize); 
		break;
	    }
	    case TFld::Real:
	    {	
		fixVl = true; 
		vSize = sizeof(double);
		double s_val = EVAL_REAL;
		eVal.assign((char*)&s_val,vSize); 
		break;
	    }
	    case TFld::Bool:
	    {	
		fixVl = true; 
		vSize = sizeof(char);
		char s_val = EVAL_BOOL;
		eVal.assign((char*)&s_val,vSize);			    
		break;
	    }
	}
	
	//Load previous val check	
	bool load_prev = false;
	long long cur_tm = TSYS::curTime();
	if( cur_tm >= begin() && cur_tm <= end() && period() > 10000000 )
	{ owner().prev_tm = cur_tm; load_prev = true; }	
	
	//Check and repare last archive files
	//Get file size
	int hd = open(m_name.c_str(),O_RDWR);
	if( hd == -1 )	throw TError(owner().archivator().nodePath().c_str(),mod->I18N("Archive file <%s> no openned!"),name.c_str());
	m_size = lseek(hd,0,SEEK_END);
	mpos = (end()-begin())/period();
	if( !m_pack && cur_tm >= begin() && cur_tm <= end() )
	    repairFile(hd);
	close(hd);
	res.release();
	
	//Load previous value
	if( load_prev )
	    switch(type())
	    {
		case TFld::Dec: case TFld::Oct: case TFld::Hex:
		{
		    int tval = getI((cur_tm-begin())/period()); 
		    owner().prev_val.assign((char*)&tval,vSize);		
		    break;
		}
		case TFld::Real:
		{    
		    double tval = getR((cur_tm-begin())/period());
		    owner().prev_val.assign((char*)&tval,vSize);		
		}
		break;
	    }	
    }
    catch( TError err )
    { 
	Mess->put(err.cat.c_str(),TMess::Error,"%s",err.mess.c_str()); 
	Mess->put(mod->nodePath().c_str(),TMess::Error,mod->I18N("Attach file <%s> error."),name.c_str());
	m_err = true;
    }
}                                               

void VFileArch::check( )
{
    //- Check for pack archive file -
    if( !m_err && !m_pack && owner().archivator().packTm() && (time(NULL) > m_acces + owner().archivator().packTm()*60) )
    {
	m_name = mod->packArch(name());
	m_pack = true;
	
        //Write info to DB
        TConfig c_el(&mod->packFE());
        c_el.cfg("FILE").setS(m_name);
	c_el.cfg("BEGIN").setS(TSYS::ll2str(begin(),TSYS::Hex));
        c_el.cfg("END").setS(TSYS::ll2str(end(),TSYS::Hex));
        c_el.cfg("PRM1").setS(owner().archive().id());
        c_el.cfg("PRM2").setS(TSYS::ll2str(period(),TSYS::Hex));
        c_el.cfg("PRM3").setS(TSYS::int2str(type()));
        SYS->db().at().dataSet(mod->filesDB(),mod->nodePath()+"Pack/",c_el);
	
	//Get file size
	int hd = open(m_name.c_str(),O_RDONLY);
	if( hd > 0 )
	{
	    m_size = lseek(hd,0,SEEK_END);
	    close(hd);
	}
    }
}

void VFileArch::getVal( TValBuf &buf, long long beg, long long end )
{    
    string val;
    ResAlloc res(m_res,false);
    
    beg = (beg/period()+(bool)(beg%period()))*period();
    
    if( m_err ) throw TError(owner().archivator().nodePath().c_str(),mod->I18N("Archive file error!"));
    if( m_pack ) { m_name = mod->unPackArch(m_name); m_pack = false; }
    
    //- Open archive file -
    int hd = open(name().c_str(),O_RDONLY);
    if( hd <= 0 ) { m_err = true; return; }
    m_acces = time(NULL);
    
    while(beg <= end)
    {
	int vpos = (beg-begin())/period();	
	if( vpos < 0 || vpos > mpos ) break;
	if( buf.realSize() >= TArchiveS::max_req_vals )	break;
    	switch(type())
    	{
    	    case TFld::Bool: 
		val = getValue(hd,calcVlOff(hd,vpos),sizeof(char));
		buf.setB((bool)*val.c_str(),begin()+vpos*period());
		break;
	    case TFld::Dec: case TFld::Oct: case TFld::Hex:
            	val = getValue(hd,calcVlOff(hd,vpos),sizeof(int));
		buf.setI(*(int*)val.c_str(),begin()+vpos*period());
		break;
	    case TFld::Real:
		val = getValue(hd,calcVlOff(hd,vpos),sizeof(double));
                buf.setR(*(double*)val.c_str(),begin()+vpos*period());		
                break;
	    case TFld::String:	    
	        int v_sz;
		int v_off = calcVlOff(hd,vpos,&v_sz);
		buf.setS(getValue(hd,v_off,v_sz),begin()+vpos*period());
		break;	    
	}    	    	
	beg+=period();
    }
    
    close(hd);
}

string VFileArch::getS( int vpos )
{
    if( m_err ) throw TError(owner().archivator().nodePath().c_str(),mod->I18N("Archive file error!"));    
    if( m_pack ) { m_name = mod->unPackArch(m_name); m_pack = false; }

    switch(type())
    {
	case TFld::Bool: 
	{ char vl = getB(vpos); return (vl==EVAL_BOOL)?EVAL_STR:TSYS::int2str((bool)vl); }
	case TFld::Dec: case TFld::Oct: case TFld::Hex:
	{ int vl = getI(vpos); return (vl==EVAL_INT)?EVAL_STR:TSYS::int2str(vl); }
	case TFld::Real:
	{ double vl = getR(vpos); return (vl==EVAL_REAL)?EVAL_STR:TSYS::real2str(vl); }
	case TFld::String:
	    m_acces = time(NULL);
	    ResAlloc res(m_res,false);
	    //- Open archive file -
            int hd = open(name().c_str(),O_RDONLY);
            if( hd <= 0 ) { m_err = true; return EVAL_STR; }
	    int v_sz;
            int v_off = calcVlOff(hd,vpos,&v_sz);
	    string rez = getValue(hd,v_off,v_sz);
	    close(hd);
	    return rez;
    }
}

double VFileArch::getR( int vpos )
{
    if( m_err ) throw TError(owner().archivator().nodePath().c_str(),mod->I18N("Archive file error!"));
    if( m_pack ) { m_name = mod->unPackArch(m_name); m_pack = false; }

    switch(type())
    {
	case TFld::Bool:
	{ char vl = getB(vpos); return (vl==EVAL_BOOL)?EVAL_REAL:(bool)vl; }
	case TFld::Dec: case TFld::Oct: case TFld::Hex:
	{ int vl = getI(vpos); return (vl==EVAL_INT)?EVAL_REAL:(double)vl; }
	case TFld::String:
	{ string vl = getS(vpos); return (vl==EVAL_STR)?EVAL_REAL:atof(vl.c_str()); }
	case TFld::Real:
	    m_acces = time(NULL);
	    ResAlloc res(m_res,false);
	    //- Open archive file -
    	    int hd = open(name().c_str(),O_RDONLY);
	    if( hd <= 0 ) { m_err = true; return EVAL_REAL; }		    
	    double rez = *(double*)getValue(hd,calcVlOff(hd,vpos),sizeof(double)).c_str();
	    close(hd);
	    return rez;
    }
}

int VFileArch::getI( int vpos )
{
    if( m_err ) throw TError(owner().archivator().nodePath().c_str(),mod->I18N("Archive file error!"));
    if( m_pack ) { m_name = mod->unPackArch(m_name); m_pack = false; }
    
    switch(type())
    {
        case TFld::Bool:
	{ char vl = getB(vpos); return (vl==EVAL_BOOL)?EVAL_INT:(bool)vl; }
        case TFld::String:
	{ string vl = getS(vpos); return (vl==EVAL_STR)?EVAL_INT:atoi(vl.c_str()); }
	case TFld::Real:
	{ double vl = getR(vpos); return (vl==EVAL_REAL)?EVAL_INT:(int)vl; }
	case TFld::Dec: case TFld::Oct: case TFld::Hex:
	    m_acces = time(NULL);
	    ResAlloc res(m_res,false);
	    //- Open archive file -
	    int hd = open(name().c_str(),O_RDONLY);
            if( hd <= 0 ) { m_err = true; return EVAL_INT; }
	    int rez = *(int*)getValue(hd,calcVlOff(hd,vpos),sizeof(int)).c_str();
	    close(hd);
	    return rez;
    }
}

char VFileArch::getB( int vpos )
{
    if( m_err ) throw TError(owner().archivator().nodePath().c_str(),mod->I18N("Archive file error!"));
    if( m_pack ) { m_name = mod->unPackArch(m_name); m_pack = false; }

    switch(type())
    {
        case TFld::Dec: case TFld::Oct: case TFld::Hex:
	{ int vl = getI(vpos); return (vl==EVAL_INT)?EVAL_BOOL:(bool)vl; }
        case TFld::String:
	{ string vl = getS(vpos); return (vl==EVAL_STR)?EVAL_BOOL:(bool)atoi(vl.c_str()); }
	case TFld::Real:
	{ double vl = getR(vpos); return (vl==EVAL_REAL)?EVAL_BOOL:(bool)vl; }
	case TFld::Bool:
	    m_acces = time(NULL);
	    ResAlloc res(m_res,false);
            //- Open archive file -
    	    int hd = open(name().c_str(),O_RDONLY);
	    if( hd <= 0 ) { m_err = true; return EVAL_BOOL; }
	    char rez = *(char*)getValue(hd,calcVlOff(hd,vpos),sizeof(char)).c_str();
	    close(hd);
	    return rez;
    }
}				    

void VFileArch::setVal( TValBuf &buf, long long ibeg, long long iend )
{   
    string value;       //Set value

    if( m_err ) throw TError(owner().archivator().nodePath().c_str(),mod->I18N("Archive file error!"));
    if( m_pack ) { m_name = mod->unPackArch(m_name); m_pack = false; }
    
    //- Open archive file -
    int hd = open(name().c_str(),O_RDWR);
    if( hd <= 0 ) { m_err = true; return; }
    m_acces = time(NULL);

    //- Read values, put it to file buffer and flush buffer to file -    
    bool first_val = true;
    while( ibeg <= iend )
    {
	//-- Get value and put it to file --
    	switch(type())
    	{
    	    case TFld::Bool:
    	    { char tval = buf.getB(&ibeg,true); value.assign(&tval,sizeof(char)); break; }
	    case TFld::Dec: case TFld::Oct: case TFld::Hex:
	    { int tval = buf.getI(&ibeg,true); value.assign((char*)&tval,sizeof(int)); break; }
	    case TFld::Real:	
	    { double tval = buf.getR(&ibeg,true); value.assign((char*)&tval,sizeof(double)); break; }
	    case TFld::String: 
		value = buf.getS(&ibeg,true); 
		if(!value.size()) value = " ";
		if(value.size() >= 1<<(vSize*8))
		    value.erase((1<<(vSize*8))-1);
		break;
	}
	
	//--- Process for ordering and averaging ---
	int pos_cur = (ibeg-begin())/period();
	int pos_prev = (owner().prev_tm-begin())/period();
	int vdif = pos_cur-pos_prev;
	if( !vdif )
	{
	    //Average for int and real
	    switch(type())
	    {
		case TFld::Dec: case TFld::Oct: case TFld::Hex:
		{
		    int v_o = *(int*)owner().prev_val.c_str();
		    int rez = *(int*)value.c_str();
		    if( rez == EVAL_INT ) rez = v_o;
		    if( rez != EVAL_INT && v_o != EVAL_INT )
		    {
			int s_k = ibeg-(period()*pos_cur+begin());
			int n_k = ibeg-owner().prev_tm;
			rez = ((long long)v_o*s_k+(long long)rez*n_k)/(s_k+n_k);
		    }
		    value.assign((char*)&rez,sizeof(int));
		    break;
		}
		case TFld::Real:
		{
		    double v_o = *(double*)owner().prev_val.c_str();
		    double rez = *(double*)value.c_str();
		    if( rez == EVAL_REAL ) rez = v_o;
		    if( rez != EVAL_REAL && v_o != EVAL_REAL )
		    {
			int s_k = ibeg-(period()*pos_cur+begin());
                        int n_k = ibeg-owner().prev_tm;
			rez = (v_o*s_k+rez*n_k)/(s_k+n_k);
		    }
		    value.assign((char*)&rez,sizeof(double));
		    break;
		}
	    }
	    //Write to prev_val
	    owner().prev_val = value;
	    owner().prev_tm  = ibeg;
	}
	else if( vdif >= 1 )
	{
	    //Write prev_val to file
	    if( vdif == 1 && !first_val )
		putValue(hd,(owner().prev_tm-begin())/period(),owner().prev_val);
	    //Write new to prev_val
	    owner().prev_val = value;
	    owner().prev_tm  = ibeg;
	}
	//Direct write very old value
	if( vdif < 0 )	putValue(hd,(ibeg-begin())/period(),value);
	//Write prev_val to file
	else if( ibeg >= iend ) putValue(hd,(owner().prev_tm-begin())/period(),owner().prev_val);
	
	first_val = false;
	ibeg++; 
    }
			    
    m_size = lseek(hd,0,SEEK_END);
    close(hd);
}

void VFileArch::putValue( int hd, int vpos, const string &ival )
{
    if( vpos > mpos )	return;

    int voff = 0,
    	pk_val_cur = 0,        	//Pack value curent position
    	pk_val_nxt = 0,		//Pack value next position
    	pk_val_prev = 0;	//Pack value previous position
    string val_next,		//Next value
	   val_prev,		//Previous value
	   val_old;		//Old value

    ResAlloc res(m_res,true);

    if(fixVl)
    {
	//- Fix mode --
	//-- Calc value position offset --
	voff = calcVlOff(hd,vpos,NULL,true);
	//-- Get pack values (current, next and previous) --
	pk_val_cur = getPkVal(hd,vpos);	
	if(vpos < mpos)	pk_val_nxt = getPkVal(hd,vpos+1);	
	//-- Read old and previous values --
	val_old = getValue(hd,voff,vSize);
    	if(ival == val_old) return;
	if(vpos) val_prev = pk_val_cur?getValue(hd,voff-vSize,vSize):val_old;
	//-- Update header and write value --
	//--- Check equale with previous and rounding ---
	bool equal = false;
	if( vpos )
	{
	    equal = (ival == val_prev);
	    if( !equal && (type() == TFld::Dec || type() == TFld::Oct || type() == TFld::Hex) )
	    {
		int v_prv = *(int*)val_prev.c_str();
		int v_set = *(int*)ival.c_str();
		if( (v_prv > 0 && v_set > 0) || (v_prv < 0 && v_set < 0) )
		    equal = 100.*(double)abs(v_prv-v_set)/(double)vmax(abs(v_prv),abs(v_set)) <= ((ModVArch&)owner().archivator()).roundProc();
	    }
	    else if( !equal && type() == TFld::Real )
	    {
		double v_prv = *(double*)val_prev.c_str();
		double v_set = *(double*)ival.c_str();
	        if( (v_prv > 0. && v_set > 0.) || (v_prv < 0. && v_set < 0.) )
		    equal = 100.*fabs(v_prv-v_set)/vmax(fabs(v_prv),fabs(v_set)) <= ((ModVArch&)owner().archivator()).roundProc();
	    }
	}	
	if(equal && val_old == val_prev) return;
	
	//--- Write value and update pack header ---
    	if( equal )
	{
	    if( pk_val_nxt )
	    {
		setPkVal(hd,vpos,0);		//pk_val[vpos] = 0
		moveTail(hd,voff+vSize,voff);	//move (down): voff+1 -> voff
    	    }
    	    else
    	    {
    		if(vpos<mpos) setPkVal(hd,vpos+1,1);	//pk_val[vpos+1] = pk_val[vpos]
    		setPkVal(hd,vpos,0);			//pk_val[vpos] = 0
    	    }
	}
	else
	{
	    if( pk_val_cur )
	    {
		if( pk_val_nxt )
		{
		    if( ival == getValue(hd,voff+vSize,vSize) )	//Compare with next value
		    {
			moveTail(hd,voff+vSize,voff);	//move (down): voff+1 -> voff
			setPkVal(hd,vpos+1,0);		//pk_val[vpos+1] = 0
		    }
		    else setValue(hd,voff,ival);	//val[voff] = value;
		}
		else
		{
		    if(vpos<mpos)
		    {
		    	moveTail(hd,voff,voff+vSize);	//move (up): voff -> voff+1
			setPkVal(hd,vpos+1,1);  	//pk_val[vpos+1] = pk_val[vpos]
		    }
		    setValue(hd,voff,ival);		//val[voff] = value;		    
		}
	    }
	    else
	    {
		if( pk_val_nxt )
		{
		    if( ival == getValue(hd,voff+vSize,vSize) )
		    {
			setPkVal(hd,vpos,1);	//pk_val[vpos] = pk_val[vpos+1]
			setPkVal(hd,vpos+1,0);	//pk_val[vpos+1] = 0
		    }
		    else
		    {
			moveTail(hd,voff+vSize,voff+2*vSize);	//move (up): voff+1 -> voff+2
			setValue(hd,voff+vSize,ival);		//val[voff+1] = value;
			setPkVal(hd,vpos,1);			//pk_val[vpos] = 1
		    }
		}
		else
		{
		    if(vpos<mpos)
                    {
		    	moveTail(hd,voff+vSize,voff+3*vSize);	//move (up): voff+1 -> voff+3
		    	setValue(hd,voff+2*vSize,val_old);	//val[voff+2] = val[voff]
		    	setPkVal(hd,vpos+1,1);	    		//pk_val[vpos+1] = 1
		    }
		    setValue(hd,voff+vSize,ival);		//val[voff+1] = value
		    setPkVal(hd,vpos,1);	    		//pk_val[vpos] = 1
		}
	    }
	}
    }
    else
    {
	//- Flow mode -	
	//-- Get pack values (current, next and previous) --
	pk_val_cur = getPkVal(hd,vpos);	
	if(vpos < mpos)	pk_val_nxt = getPkVal(hd,vpos+1);	
	//-- Read old and previous values --
	int v_sz;
	voff = calcVlOff(hd,vpos,&v_sz,true);
	val_old = getValue(hd,voff,v_sz);
    	if(ival == val_old) return;
	if(vpos) 
	{
	    if(pk_val_cur)
	    {
		int v_sz_prev;
		int voff_prev = calcVlOff(hd,vpos-1,&v_sz_prev,true);
		val_prev = getValue(hd,voff_prev,v_sz_prev);
	    }else val_prev = val_old;
	}
	//--- Write value and update pack header ---	
    	if( vpos && ival == val_prev )
	{
	    if( pk_val_nxt )
	    {
		setPkVal(hd,vpos,0);		//pk_val[vpos] = 0
		moveTail(hd,voff+v_sz,voff);	//move (down): voff+1 -> voff
    	    }
    	    else
    	    {
    		setPkVal(hd,vpos+1,v_sz);	//pk_val[vpos+1] = pk_val[vpos]
    		setPkVal(hd,vpos,0);		//pk_val[vpos] = 0
    	    }
	}
	else
	{	
	    if( pk_val_cur )
	    {
		if( pk_val_nxt )
		{
		    int v_sz_next;
		    int voff_next = calcVlOff(hd,vpos+1,&v_sz_next,true);
                    val_next = getValue(hd,voff_next,v_sz_next);
		    if( ival == val_next )
		    {
			moveTail(hd,voff+v_sz,voff);	//move (down): voff+1 -> voff
			setPkVal(hd,vpos,v_sz_next);	//pk_val[vpos] = pk_val[vpos+1]
			setPkVal(hd,vpos+1,0);		//pk_val[vpos+1] = 0
		    }
		    else
		    {
		    	moveTail(hd,voff+v_sz,voff+ival.size());
	    		setPkVal(hd,vpos,ival.size());
			setValue(hd,voff,ival);		//val[voff] = value
		    }
		}
		else
		{
 		    if(vpos<mpos)
		    {        	    
		    	moveTail(hd,voff,voff+ival.size());	//move (up): voff -> voff+1
			setPkVal(hd,vpos+1,v_sz);     	//pk_val[vpos+1] = pk_val[vpos]
		    }
		    else moveTail(hd,voff+v_sz,voff+ival.size());
		    setValue(hd,voff,ival);		//val[voff] = value
		    setPkVal(hd,vpos,ival.size());    	//pk_val[vpos] = 1
		}
	    }
	    else
	    {
		if( pk_val_nxt )
		{
 		    int v_sz_next;
		    int voff_next = calcVlOff(hd,vpos+1,&v_sz_next,true);
                    val_next = getValue(hd,voff_next,v_sz_next);        	    
		    if( ival == val_next )
		    {
			setPkVal(hd,vpos,v_sz_next);	//pk_val[vpos] = pk_val[vpos+1]
			setPkVal(hd,vpos+1,0);		//pk_val[vpos+1] = 0
		    }
		    else
		    {
			moveTail(hd,voff+v_sz,voff+v_sz+ival.size());	//move (up): voff+1 -> voff+2
			setValue(hd,voff+v_sz,ival);	//val[voff+1] = value
			setPkVal(hd,vpos,ival.size());	//pk_val[vpos] = 1
		    }
		}
		else
		{
 		    if(vpos<mpos)
                    {        	    
		    	moveTail(hd,voff+v_sz,voff+v_sz+ival.size()+v_sz);	//move (up): voff+1 -> voff+3
		    	setValue(hd,voff+v_sz+ival.size(),val_old);	//val[voff+2] = val[voff]
		    	setPkVal(hd,vpos+1,v_sz);			//pk_val[vpos+1] = 1
		    }		    
		    setValue(hd,voff+v_sz,ival);			//val[voff+1] = value
		    setPkVal(hd,vpos,ival.size());			//pk_val[vpos] = 1
		}
	    }
	}
    }
}

string VFileArch::getValue( int hd, int voff, int vsz )
{ 
    string get_vl;
    
    lseek(hd,voff,SEEK_SET);
    read(hd,&tbt,1);
    get_vl.assign((char *)&tbt,sizeof(char));
    for( int i_vs = 0; i_vs < vsz-1; i_vs++ )
    {
	read(hd,&tbt,1);
      	get_vl.append((char *)&tbt,sizeof(char));
    }
    return get_vl;
}

int VFileArch::calcVlOff( int hd, int vpos, int *vsz, bool wr )
{    
    int b_sz = 0, i_bf = 0;
    char buf[STR_BUF_LEN];
    int voff;
    
    if( fixVl )
    {
	int cach_pos = vpos;
	int cach_off = cacheGet( cach_pos );
	cach_pos++;
	if( cach_off ) voff = cach_off;
	else voff = sizeof(FHead)+mpos/8+(bool)(mpos%8);
	lseek(hd,sizeof(FHead)+cach_pos/8,SEEK_SET);
	
	if( cach_pos%8 ) 
	{
	    b_sz = vmin((vpos/8)-(cach_pos/8)+1,sizeof(buf));
	    read(hd,buf,b_sz);
	}
	for(int i_ps = cach_pos; i_ps <= vpos; i_ps++)
        {
	    int rest = i_ps%8;
	    if( !rest )
	    {
		if( ++i_bf >= b_sz )
		{
		    b_sz = vmin((vpos/8)-(i_ps/8)+1,sizeof(buf));
		    read(hd,&buf,b_sz);
		    i_bf = 0;
		}
	    }
	    voff += vSize*(bool)((0x01<<rest)&buf[i_bf]);
	    //Update cache
            if( (i_ps-cach_pos && !((i_ps-cach_pos)%VAL_CACHE_POS)) || i_ps == vpos )
                cacheSet(i_ps,voff,0,i_ps==vpos,wr);
	}
    }
    else
    {
	int cach_pos = vpos;	
    	int lst_pk_vl;
	int cach_off = cacheGet( cach_pos, &lst_pk_vl );
	if( cach_off )	{ voff = cach_off; cach_pos++; }
	else voff = sizeof(FHead)+mpos*vSize;
	lseek(hd,sizeof(FHead)+cach_pos*vSize,SEEK_SET);
	
	for(int i_ps = cach_pos; i_ps <= vpos; i_ps++)
	{
	    int pk_vl = 0;
	    for(int i_e = 0; i_e < vSize; i_e++)
	    {
		if( ++i_bf >= b_sz )
		{
		    b_sz = vmin(vSize*(vpos-i_ps+1),sizeof(buf));
		    read(hd,&buf,b_sz);
		    i_bf = 0;
		}
		pk_vl+=buf[i_bf]<<(8*i_e);
	    }	
	    if( pk_vl )
	    { 
		if(i_ps) voff += lst_pk_vl;
		lst_pk_vl = pk_vl;
	    }
	    //Update cache
	    if( (i_ps-cach_pos && !((i_ps-cach_pos)%VAL_CACHE_POS)) || i_ps == vpos )
                cacheSet(i_ps,voff,lst_pk_vl,i_ps==vpos,wr);
	}
	if(vsz) *vsz = lst_pk_vl;
    }
    
    return voff;
}

void VFileArch::setValue( int hd, int voff, const string &val )
{
    lseek(hd,voff,SEEK_SET);
    write(hd,val.c_str(),val.size());
}

void VFileArch::moveTail( int hd, int old_st, int new_st )
{
    char buf[STR_BUF_LEN];
    
    if( old_st == new_st ) return;
    if( new_st > old_st )
    {
	//Move down (insert)
	int beg_cur;
	int mv_beg = old_st;
	int mv_end = lseek(hd,0,SEEK_END);
	if( mv_end <= mv_beg )	return;
	do
    	{
    	    beg_cur = ((mv_end-mv_beg)>=sizeof(buf))?mv_end-sizeof(buf):mv_beg;
	    lseek(hd,beg_cur,SEEK_SET);
	    read(hd,buf,mv_end-beg_cur);
	    lseek(hd,beg_cur+(new_st-old_st),SEEK_SET);
	    write(hd,buf,mv_end-beg_cur);
	    mv_end-=sizeof(buf);
	}
	while(beg_cur!=mv_beg);
    }
    else
    {
	//Move up (erase)
	int end_cur;
	int mv_beg = old_st;
	int mv_end = lseek(hd,0,SEEK_END);
	if( mv_end <= mv_beg )	return;	
	do
    	{
    	    end_cur = ((mv_end-mv_beg)>=sizeof(buf))?mv_beg+sizeof(buf):mv_end;
	    lseek(hd,mv_beg,SEEK_SET);
	    read(hd,buf,end_cur-mv_beg);
	    lseek(hd,mv_beg-(old_st-new_st),SEEK_SET);
	    write(hd,buf,end_cur-mv_beg);
	    mv_beg+=sizeof(buf);
	}
	while(end_cur!=mv_end);
	//Truncate tail
	ftruncate(hd,mv_end-(old_st-new_st));
    }    
}

int VFileArch::getPkVal( int hd, int vpos )
{
    int pk_val = 0;
    
    if(fixVl)
    {
	lseek(hd,sizeof(FHead)+vpos/8,SEEK_SET);
	read(hd,&tbt,1);
	pk_val = ((0x01<<(vpos%8))&tbt)?1:0;
    }
    else
    {	
	lseek(hd,sizeof(FHead)+vpos*vSize,SEEK_SET);
	for(int v_psz = 0; v_psz < vSize; v_psz++ )
	{
    	    read(hd,&tbt,1);
	    pk_val += tbt<<(v_psz*8);
	}
    }
    return pk_val;
}

void VFileArch::setPkVal( int hd, int vpos, int vl )
{
    //Update cache
    cacheUpdate(vpos,vl-getPkVal(hd,vpos));	        

    if(fixVl)
    {
	lseek(hd,sizeof(FHead)+vpos/8,SEEK_SET);
	read(hd,&tbt,1);
	tbt=vl?((0x01<<(vpos%8))|tbt):(~(0x01<<(vpos%8))&tbt);
	lseek(hd,-1,SEEK_CUR);
	write(hd,&tbt,1);	
    }
    else
    {
	lseek(hd,sizeof(FHead)+vpos*vSize,SEEK_SET);	
	for(int v_psz = 0; v_psz < vSize; v_psz++ )
    	    write(hd,((char *)&vl)+v_psz,1);
    }
}

void VFileArch::repairFile(int hd)
{
    int v_sz;
    if( !m_pack )
    {	
	int f_sz = lseek(hd,0,SEEK_END);
	int f_off = calcVlOff(hd,mpos,&v_sz);
	if( fixVl )
	{
	    int dt = f_sz-f_off-vSize;
	    if( !dt )	return;
	    Mess->put(owner().archivator().nodePath().c_str(),TMess::Error,
		mod->I18N("Error archive file structure: <%s>. Margin = %d byte. Will try fix it!"),name().c_str(),dt);
	    //Fix file
	    if(dt>0)
	    {
		ftruncate(hd,f_off);
		setValue(hd,f_off,eVal);
	    }
	    else 
	    {
		f_sz = f_off-vSize*((f_off-f_sz)/vSize);
		while(f_sz<=f_off) { setValue(hd,f_sz,eVal); f_sz+=vSize; }
	    }
	}
	else
	{
	    //In progress
	}
    }
}

int VFileArch::cacheGet( int &pos, int *vsz )
{
    CacheEl rez = {0,0,0};
    for( int i_p = cache.size()-1; i_p >= 0; i_p-- )
	if( pos >= cache[i_p].pos ) { rez = cache[i_p]; break; }
    if( pos >= cach_pr_rd.pos && cach_pr_rd.pos > rez.pos ) 
	rez = cach_pr_rd;
    if( pos >= cach_pr_wr.pos && cach_pr_wr.pos > rez.pos )
        rez = cach_pr_wr;
    
    pos = rez.pos;
    if(vsz) *vsz = rez.vsz;
    return rez.off;
}

void VFileArch::cacheSet( int pos, int off, int vsz, bool last, bool wr  )
{
    CacheEl el = { pos, off, vsz };

    if( !last )
    {
	for( int i_p = 0; i_p < cache.size(); i_p++ )
	    if( el.pos == cache[i_p].pos )	{ cache[i_p] = el; return; }
	    else if( el.pos < cache[i_p].pos )	{ cache.insert(cache.begin()+i_p,el); return; }
	cache.push_back(el);
    }
    else if( wr ) cach_pr_wr = el;
    else cach_pr_rd = el;
}

void VFileArch::cacheUpdate( int pos, int var )
{
    if( !var )	return;
    for( int i_p = 0; i_p < cache.size(); i_p++ )
	if( cache[i_p].pos >= pos ) 
	{ 
	    if( fixVl )	cache[i_p].off += (fixVl?vSize*var:var);
	    else { cache.erase(cache.begin()+i_p); i_p--; }
	}
    if( cach_pr_rd.pos >= pos ) 
    {
	if( fixVl ) cach_pr_rd.off += (fixVl?vSize*var:var);
	else cach_pr_rd.off = cach_pr_rd.pos = cach_pr_rd.vsz = 0;
    }
    if( cach_pr_wr.pos >= pos ) 
    {
	if( fixVl ) cach_pr_wr.off += (fixVl?vSize*var:var);
	else cach_pr_wr.off = cach_pr_wr.pos = cach_pr_wr.vsz = 0;
    }
}

