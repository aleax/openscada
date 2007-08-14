
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

void ModVArch::setValPeriod( double iper )
{
    TVArchivator::setValPeriod(iper);
    
    time_size=vmax(0.2,1000.*valPeriod());    
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
    if( !startStat() )	return;
    if( now || time(NULL) > m_lst_check + checkTm()*60 )
    {
	//- Find archive files for no present archives and create it. -
	struct stat file_stat;
	dirent *scan_dirent;
    
	//-- Open/create new directory --
	DIR *IdDir = opendir(addr().c_str());
	if(IdDir == NULL) 
	{
    	    if( mkdir(addr().c_str(),0777) ) throw TError(nodePath().c_str(),_("Can not create dir <%s>."),addr().c_str());
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
		owner().owner().valAt(ArhNm).at().setToStart(true);
		owner().owner().valAt(ArhNm).at().setValType(ArhTp);
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

void ModVArch::expArch(const string &arch_nm, time_t beg, time_t end, const string &file_tp, const string &file_nm)
{
    long long buf_sz = 100000;
    long long buf_per = (long long)(valPeriod()*1000000.);
    long long c_tm, c_tm1;

    TValBuf buf( TFld::Real, buf_sz, buf_per, true, true );
    beg=vmax(beg,SYS->archive().at().valAt(arch_nm).at().begin(workId())/1000000);
    end=vmin(end,SYS->archive().at().valAt(arch_nm).at().end(workId())/1000000);
    
    if(file_tp == "wav")
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
    
	int hd=open((file_nm+"."+file_tp).c_str(),O_RDWR|O_CREAT|O_TRUNC, 0666);
	if( hd == -1 ) return;
	write(hd,&rif,sizeof(rif));
	write(hd,&chnk,sizeof(chnk));
	write(hd,&wv_form,sizeof(wv_form));
	strncpy(chnk.chunk_id,"data",4); 
	chnk.chunksize = 0;		//Set temporary size buf.realSize()*sizeof(float);
	off_t sz_pos = lseek(hd,0,SEEK_CUR);
	write(hd,&chnk,sizeof(chnk));
    
	//Calc overage and scale of value
	float c_val, v_over=0, v_max=-1e30, v_min=1e30;	
    
	c_tm = (long long)beg*1000000;
	while( c_tm < (long long)end*1000000 )
	{
	    long long end_tm = c_tm+buf_sz*buf_per;
	    end_tm = vmin(end_tm,(long long)end*1000000);
	    SYS->archive().at().valAt(arch_nm).at().getVal(buf,c_tm,end_tm,workId());
	
	    //Check scale
	    for(;c_tm <= buf.end();c_tm++)
	    {
		c_val = buf.getR(&c_tm,true);
		if(c_val == EVAL_REAL)	continue;
		v_max=vmax(c_val,v_max);
		v_min=vmin(c_val,v_min);
	    }
	    v_over = (v_max+v_min)/2;	    
	}
	buf.clear();	
	
	//Transver value
	int val_cnt = 0;
	c_tm = (long long)beg*1000000;
	while( c_tm < (long long)end*1000000 )
	{
	    long long end_tm = c_tm+buf_sz*buf_per;
	    end_tm = vmin(end_tm,(long long)end*1000000);
	    SYS->archive().at().valAt(arch_nm).at().getVal(buf,c_tm,end_tm,workId());	
	
	    for(;c_tm <= buf.end();c_tm++, val_cnt++)
	    {
		c_val = buf.getR(&c_tm,true);
		if(c_val == EVAL_REAL)	c_val = v_over;
		c_val = 2.*(c_val-v_over)/(v_max-v_min);
		//printf("TEST 00: %f\n",c_val);
		write(hd,&c_val,sizeof(float));
	    }
	}
	
	//Write value count
	lseek(hd,sz_pos,SEEK_SET);
	chnk.chunksize = val_cnt*sizeof(float);
	write(hd,&chnk,sizeof(chnk));
	
	close(hd);
    }
    else 
    {
	char c_val[40];
    	int hd=open((file_nm+"."+file_tp).c_str(),O_RDWR|O_CREAT|O_TRUNC, 0666);
	if( hd == -1 ) return;
	
	c_tm = (long long)beg*1000000;
	while( c_tm < (long long)end*1000000 )
	{
	    long long end_tm = c_tm+buf_sz*buf_per;
	    end_tm = vmin(end_tm,(long long)end*1000000);
	    SYS->archive().at().valAt(arch_nm).at().getVal(buf,c_tm,end_tm,workId());	
	
    	    for(; c_tm <= buf.end(); c_tm++ )
	    {
		sprintf(c_val,"%g\n",buf.getR(&c_tm,true));
		write(hd,c_val,strlen(c_val));
	    }
	}
	close(hd);
    }
}

TVArchEl *ModVArch::getArchEl( TVArchive &arch )
{   
    ModVArchEl *v_el = new ModVArchEl(arch,*this); 
    v_el->checkArchivator();
    return v_el;
}

void ModVArch::cntrCmdProc( XMLNode *opt )
{
    string grp = owner().owner().subId();
    //Get page info
    if( opt->name() == "info" )
    {
        TVArchivator::cntrCmdProc(opt);
	if(ctrMkNode("area",opt,1,"/bs",_("Additional options"),0444,"root",grp.c_str()))
	{
	    ctrMkNode("fld",opt,-1,"/bs/tm",cfg("BaseArhTmSize").fld().descr(),0664,"root",grp.c_str(),1,"tp","real");
	    ctrMkNode("fld",opt,-1,"/bs/fn",cfg("BaseArhNFiles").fld().descr(),0664,"root",grp.c_str(),1,"tp","dec");
	    ctrMkNode("fld",opt,-1,"/bs/round",cfg("BaseArhRound").fld().descr(),0664,"root",grp.c_str(),1,"tp","real");
	    ctrMkNode("fld",opt,-1,"/bs/pcktm",cfg("BaseArhPackTm").fld().descr(),0664,"root",grp.c_str(),1,"tp","dec");
	    ctrMkNode("fld",opt,-1,"/bs/tmout",cfg("BaseArhTm").fld().descr(),0664,"root",grp.c_str(),1,"tp","dec");
	    ctrMkNode("comm",opt,-1,"/bs/chk_nw",_("Check archivator directory now"),0660,"root",grp.c_str());	
	}
	ctrMkNode("list",opt,-1,"/arch/arch/3",_("Files size (Mb)"),0444,"root","root",1,"tp","real");
	if(ctrMkNode("comm",opt,-1,"/arch/exp",_("Export"),0660))
	{
	    ctrMkNode("fld",opt,-1,"/arch/exp/arch",_("Archive"),0660,"root","root",3,"tp","str","dest","select","select","/arch/lst");
	    ctrMkNode("fld",opt,-1,"/arch/exp/beg",_("Begin"),0660,"root","root",1,"tp","time");
	    ctrMkNode("fld",opt,-1,"/arch/exp/end",_("End"),0660,"root","root",1,"tp","time");
	    ctrMkNode("fld",opt,-1,"/arch/exp/tfl",_("Type"),0660,"root","root",3,"tp","str","dest","select","select","/arch/tpflst");
	    ctrMkNode("fld",opt,-1,"/arch/exp/file",_("To file"),0660,"root","root",1,"tp","str");
	}
        return;
    }
    //Process command to page
    string a_path = opt->attr("path");
    if( a_path == "/bs/tm" )
    {
	if( ctrChkNode(opt,"get",0664,"root",grp.c_str(),SEQ_RD) )	opt->setText(TSYS::real2str(time_size,6));
	if( ctrChkNode(opt,"set",0664,"root",grp.c_str(),SEQ_WR) )	time_size = atof(opt->text().c_str());
    }	
    else if( a_path == "/bs/fn" )
    {
	if( ctrChkNode(opt,"get",0664,"root",grp.c_str(),SEQ_RD) )	opt->setText(TSYS::int2str(numb_files));
	if( ctrChkNode(opt,"set",0664,"root",grp.c_str(),SEQ_WR) )	numb_files = atoi(opt->text().c_str());
    }
    else if( a_path == "/bs/round" )
    {
	if( ctrChkNode(opt,"get",0664,"root",grp.c_str(),SEQ_RD) )	opt->setText(TSYS::real2str(round_proc,6));
	if( ctrChkNode(opt,"set",0664,"root",grp.c_str(),SEQ_WR) )	round_proc = atof(opt->text().c_str());
    }
    else if( a_path == "/bs/pcktm" )
    {
	if( ctrChkNode(opt,"get",0664,"root",grp.c_str(),SEQ_RD) )	opt->setText(TSYS::int2str(m_pack_tm));
	if( ctrChkNode(opt,"set",0664,"root",grp.c_str(),SEQ_WR) )	m_pack_tm = atoi(opt->text().c_str());
    }
    else if( a_path == "/bs/tmout" )	
    {
	if( ctrChkNode(opt,"get",0664,"root",grp.c_str(),SEQ_RD) )	opt->setText(TSYS::int2str(m_chk_tm));
	if( ctrChkNode(opt,"set",0664,"root",grp.c_str(),SEQ_WR) )	m_chk_tm = atoi(opt->text().c_str());
    }
    else if( a_path == "/arch/arch" && ctrChkNode(opt) )
    {
        //Fill Archives table
        XMLNode *n_arch = ctrMkNode("list",opt,-1,"/arch/arch/0","");
        XMLNode *n_per  = ctrMkNode("list",opt,-1,"/arch/arch/1","");
        XMLNode *n_size = ctrMkNode("list",opt,-1,"/arch/arch/2","");
        XMLNode *f_size = ctrMkNode("list",opt,-1,"/arch/arch/3","");
					
        ResAlloc res(a_res,false);
        for( int i_l = 0; i_l < arch_el.size(); i_l++ )
        {
            if(n_arch)	n_arch->childAdd("el")->setText(arch_el[i_l]->archive().id());
	    if(n_per)	n_per->childAdd("el")->setText(TSYS::real2str((double)arch_el[i_l]->archive().period()/1000000.,6));
	    if(n_size)	n_size->childAdd("el")->setText(TSYS::int2str(arch_el[i_l]->archive().size()));
	    if(f_size)	f_size->childAdd("el")->setText(TSYS::real2str((double)((ModVArchEl *)arch_el[i_l])->size()/1024.,6));
	}
    }
    else if( a_path == "/arch/lst" && ctrChkNode(opt) )
    {
        vector<string> a_ls;
        archiveList(a_ls);
        for( int i_el = 0; i_el < a_ls.size(); i_el++ )
    	    opt->childAdd("el")->setText(a_ls[i_el]);
    }
    else if( a_path == "/arch/tpflst" && ctrChkNode(opt) )
    {
	opt->childAdd("el")->setText("ascii");
	opt->childAdd("el")->setText("wav");
    }
    else if( a_path == "/bs/chk_nw" && ctrChkNode(opt,"set",0660,"root",grp.c_str(),SEQ_WR) )	checkArchivator(true);
    else if( a_path == "/arch/exp" && ctrChkNode(opt,"set",0660,"root","root",SEQ_WR) )
	expArch(ctrId(opt,"arch")->text(),
		atoi(ctrId(opt,"beg")->text().c_str()),
		atoi(ctrId(opt,"end")->text().c_str()),
		ctrId(opt,"tfl")->text(),
		ctrId(opt,"file")->text());
    else TVArchivator::cntrCmdProc(opt);
}

//*** BaseArch::ModVArchEl - Value archive element ***
ModVArchEl::ModVArchEl( TVArchive &iachive, TVArchivator &iarchivator ) :
    TVArchEl(iachive,iarchivator)
{

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

void ModVArchEl::getVal( TValBuf &buf, long long ibeg, long long iend )
{
    iend = vmin( iend, end() );
    ibeg = vmax( ibeg, begin() );

    ResAlloc res(m_res,false);
    for( int i_a = 0; i_a < arh_f.size(); i_a++ )
	if( ibeg > iend ) break;
	else if( !arh_f[i_a]->err() && ibeg <= arh_f[i_a]->end() && iend >= arh_f[i_a]->begin() )
        {
	    for( ; ibeg < arh_f[i_a]->begin(); ibeg+=(long long)(archivator().valPeriod()*1000000.) )
		buf.setI(EVAL_INT,ibeg);
            arh_f[i_a]->getVal(buf,ibeg,vmin(iend,arh_f[i_a]->end()));
	    ibeg = arh_f[i_a]->end()+1;
	}
    for( ; ibeg <= iend; ibeg+=(long long)(archivator().valPeriod()*1000000.) )
	buf.setI(EVAL_INT,ibeg);
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
    beg = vmax(beg,buf.begin());
    end = vmin(end,buf.end());
    
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
    cach_pr_rd.pos = cach_pr_rd.off = cach_pr_wr.pos = cach_pr_wr.off = 0;
    m_acces = time(NULL);
}


VFileArch::VFileArch( const string &iname, long long ibeg, long long iend, long long iper, TFld::Type itp, ModVArchEl *iowner) :
    m_owner(iowner), m_err(false), m_size(0), m_name(iname), m_beg(ibeg), m_end(iend), m_pack(false),
    m_per(iper), m_tp(itp)
{
    char buf[1000];
    cach_pr_rd.pos = cach_pr_rd.off = cach_pr_wr.pos = cach_pr_wr.off = 0;
    
    //- Round-up begin and end position to period -
    m_beg = (m_beg/m_per)*m_per;
    m_end = (m_end/m_per)*m_per+m_per;
    
    //- Open/create new archive file -
    int hd = open( name().c_str(),O_RDWR|O_CREAT|O_TRUNC, 0666 );
    if(hd <= 0) throw TError(owner().archivator().nodePath().c_str(),_("Can not create file: <%s>!"),name().c_str());

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
	case TFld::Integer:
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
	case TFld::Boolean:
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

}

void VFileArch::delFile()
{
    ResAlloc res(m_res,true);
    remove(name().c_str());
    m_err = true;
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
	    case TFld::Integer:
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
	    case TFld::Boolean:
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
	if( hd == -1 )	throw TError(owner().archivator().nodePath().c_str(),_("Archive file <%s> no openned!"),name.c_str());
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
		case TFld::Integer:
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
	mess_err(err.cat.c_str(),"%s",err.mess.c_str()); 
	mess_err(mod->nodePath().c_str(),_("Attach file <%s> error."),name.c_str());
	m_err = true;
    }
}                                               

void VFileArch::check( )
{
    //- Check for pack archive file -
    ResAlloc res(m_res,false);
    if( !m_err && !m_pack && owner().archivator().packTm() && (time(NULL) > m_acces + owner().archivator().packTm()*60) )
    {
	res.request(true);
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
    int vpos_beg, vpos_end, voff_beg, vlen_beg, voff_end, vlen_end;
    char *pid_b, *val_b;    
    
    ResAlloc res(m_res,false);
    if( m_err ) throw TError(owner().archivator().nodePath().c_str(),_("Archive file error!"));
    
    //- Get values block characteristic -
    beg = (beg/period()+(bool)(beg%period()))*period();    
    vpos_beg = vmax(0,(beg-begin())/period());
    if( vpos_beg > mpos )	return;
    vpos_end = vmin(mpos,(end-begin())/period());
    if( vpos_end < 0 )   	return;
    //if( (vpos_end-vpos_beg) > (TArchiveS::max_req_vals-buf.realSize()) )
    //	vpos_end = vpos_beg+TArchiveS::max_req_vals-buf.realSize();
    if( vpos_beg > vpos_end )	return;

    if( m_pack ) 
    { 
	res.request(true);
	m_name = mod->unPackArch(m_name); 
	m_pack = false;
	res.request(false);
    }
    
    //- Open archive file -
    int hd = open(name().c_str(),O_RDONLY);
    if( hd <= 0 ) { m_err = true; return; }
    
    voff_beg = calcVlOff(hd,vpos_beg,&vlen_beg);
    
    //- Get the pack index block and the value block -
    if( fixVl )
    {
	//-- Get index block --
	int i_beg = sizeof(FHead)+vpos_beg/8;
	int i_end = sizeof(FHead)+vpos_end/8+1;
	lseek(hd,i_beg,SEEK_SET);
	pid_b = (char*)malloc(i_end-i_beg);
	read(hd,pid_b,i_end-i_beg);
	//-- Calc end offset --
	voff_end=voff_beg;
	for(int i_pos = vpos_beg+1; i_pos <= vpos_end; i_pos++ )
	    voff_end += vSize*(bool)((0x01<<(i_pos%8))&pid_b[(i_pos/8)-(vpos_beg/8)]);	    
	//-- Get value block --
	i_beg=voff_beg;
	i_end=voff_end+vSize;
	lseek(hd,i_beg,SEEK_SET);
	val_b = (char*)malloc(i_end-i_beg);
	read(hd,val_b,i_end-i_beg);
    }
    else
    {
	//-- Get index block --
	int i_beg = sizeof(FHead)+vpos_beg*vSize;
	int i_end = sizeof(FHead)+vpos_end*vSize+vSize;
	lseek(hd,i_beg,SEEK_SET);
	pid_b = (char*)malloc(i_end-i_beg);
	read(hd,pid_b,i_end-i_beg);
	//-- Calc end offset --
	voff_end=voff_beg;
	vlen_end=vlen_beg;
	for(int i_pos = vpos_beg+1; i_pos <= vpos_end; i_pos++ )
	{
	    int pk_vl = 0;
	    for(int i_e = 0; i_e < vSize; i_e++)
        	pk_vl+=pid_b[vSize*(i_pos-vpos_beg)+i_e]<<(8*i_e);
	    if(pk_vl)
	    {
		voff_end += vlen_end;
		vlen_end = pk_vl;		
	    }
	}
    	//-- Get value block --
	i_beg=voff_beg;
	i_end=voff_end+vlen_end;
	lseek(hd,i_beg,SEEK_SET);
	val_b = (char*)malloc(i_end-i_beg);
	read(hd,val_b,i_end-i_beg);
    }    
    
    //- Free file resource and close file -
    close(hd);
    m_acces = time(NULL);
    res.release();    
        
    //- Process value block -
    int pid_off = vpos_beg;
    voff_end -= voff_beg;
    voff_beg = 0;
    while( true )
    {
	switch(type())
        {
    	    case TFld::Boolean: 
		buf.setB((bool)*(val_b+voff_beg),begin()+vpos_beg*period());
		break;
	    case TFld::Integer:
		buf.setI(*(int*)(val_b+voff_beg),begin()+vpos_beg*period());
		break;
	    case TFld::Real:
                buf.setR(*(double*)(val_b+voff_beg),begin()+vpos_beg*period());
                break;
	    case TFld::String:
		buf.setS(string(val_b+voff_beg,vlen_beg),begin()+vpos_beg*period());
		break;
	}	
	vpos_beg++;
	if(vpos_beg > vpos_end)	break;
	if(fixVl) 
	    voff_beg += vSize*(bool)(pid_b[(vpos_beg/8)-(pid_off/8)]&(0x01<<(vpos_beg%8)));
	else
	{
	    int pk_vl = 0;
            for(int i_e = 0; i_e < vSize; i_e++)		
		pk_vl+=pid_b[vSize*(vpos_beg-pid_off)+i_e]<<(8*i_e);
	    if(pk_vl)
	    {	
		voff_beg += vlen_beg;
		vlen_beg = pk_vl;
	    }
	}	
    }    
    free(pid_b);
    free(val_b);
}

string VFileArch::getS( int vpos )
{
    ResAlloc res(m_res,false);
    if( m_err ) throw TError(owner().archivator().nodePath().c_str(),_("Archive file error!"));    
    if( m_pack ) 
    {
	res.request(true); 
	m_name = mod->unPackArch(m_name); 
	m_pack = false; 
	res.release();
    }

    switch(type())
    {
	case TFld::Boolean: 
	{ char vl = getB(vpos); return (vl==EVAL_BOOL)?EVAL_STR:TSYS::int2str((bool)vl); }
	case TFld::Integer:
	{ int vl = getI(vpos); return (vl==EVAL_INT)?EVAL_STR:TSYS::int2str(vl); }
	case TFld::Real:
	{ double vl = getR(vpos); return (vl==EVAL_REAL)?EVAL_STR:TSYS::real2str(vl); }
	case TFld::String:
	    m_acces = time(NULL);
	    res.request(false);
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
    ResAlloc res(m_res,false);
    if( m_err ) throw TError(owner().archivator().nodePath().c_str(),_("Archive file error!"));    
    if( m_pack ) 
    { 
	res.request(true);
	m_name = mod->unPackArch(m_name); 
	m_pack = false;
	res.release();
    }

    switch(type())
    {
	case TFld::Boolean:
	{ char vl = getB(vpos); return (vl==EVAL_BOOL)?EVAL_REAL:(bool)vl; }
	case TFld::Integer:
	{ int vl = getI(vpos); return (vl==EVAL_INT)?EVAL_REAL:(double)vl; }
	case TFld::String:
	{ string vl = getS(vpos); return (vl==EVAL_STR)?EVAL_REAL:atof(vl.c_str()); }
	case TFld::Real:
	    m_acces = time(NULL);
	    res.request(false);
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
    ResAlloc res(m_res,false);
    if( m_err ) throw TError(owner().archivator().nodePath().c_str(),_("Archive file error!"));
    if( m_pack ) 
    { 
	res.request(true);
	m_name = mod->unPackArch(m_name); 
	m_pack = false; 
	res.release();
    }
    
    switch(type())
    {
        case TFld::Boolean:
	{ char vl = getB(vpos); return (vl==EVAL_BOOL)?EVAL_INT:(bool)vl; }
        case TFld::String:
	{ string vl = getS(vpos); return (vl==EVAL_STR)?EVAL_INT:atoi(vl.c_str()); }
	case TFld::Real:
	{ double vl = getR(vpos); return (vl==EVAL_REAL)?EVAL_INT:(int)vl; }
	case TFld::Integer:
	    m_acces = time(NULL);
	    res.request(false);
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
    ResAlloc res(m_res,false);
    if( m_err ) throw TError(owner().archivator().nodePath().c_str(),_("Archive file error!"));
    if( m_pack ) 
    { 
	res.request(true);
	m_name = mod->unPackArch(m_name); 
	m_pack = false;
	res.release(); 
    }

    switch(type())
    {
        case TFld::Integer:
	{ int vl = getI(vpos); return (vl==EVAL_INT)?EVAL_BOOL:(bool)vl; }
        case TFld::String:
	{ string vl = getS(vpos); return (vl==EVAL_STR)?EVAL_BOOL:(bool)atoi(vl.c_str()); }
	case TFld::Real:
	{ double vl = getR(vpos); return (vl==EVAL_REAL)?EVAL_BOOL:(bool)vl; }
	case TFld::Boolean:
	    m_acces = time(NULL);
	    res.request(false);
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
    int vpos_beg, vpos_end, vdif;
    string val_b, value, value_first, value_end;       //Set value

    ResAlloc res(m_res,false);
    if( m_err ) throw TError(owner().archivator().nodePath().c_str(),_("Archive file error!"));

    ibeg = vmax(ibeg,begin());
    iend = vmin(iend,end());
    if( ibeg > iend )	return;

    if( m_pack )
    { 
	res.request(true);
	m_name = mod->unPackArch(m_name); 
	m_pack = false; 
	res.request(false);
    }
    
    //Init pack index buffer
    vpos_beg = (ibeg-begin())/period();
    vpos_end = (iend-begin())/period();    
    string pid_b( fixVl ? (vpos_end/8)-(vpos_beg/8)+1 : vSize*(vpos_end-vpos_beg+1), '\0' );
    
    //Reserve memory for values buffer
    val_b.reserve(fixVl ? vSize*(vpos_end-vpos_beg+1)/2 : 10*(vpos_end-vpos_beg+1));
    //printf("TEST 00: beg=%lld; end=%lld\n",ibeg,iend);
    
    //- Get values, make value buffer and init the pack index table -
    vpos_beg = 0;
    vpos_end = -1;
    while( ibeg <= iend )
    {
	//-- Get value and put it to file --
    	switch(type())
    	{
    	    case TFld::Boolean:
    	    { 
		char tval = buf.getB(&ibeg,true); 
		value.assign(&tval,vSize); 
		break; 
	    }
	    case TFld::Integer:
	    { 
		int tval = buf.getI(&ibeg,true);
 		if( ((ModVArch&)owner().archivator()).roundProc() && vpos_end >= 0 )
		{
		    int vprev = *(int*)value_end.c_str();
		    if( ((vprev > 0 && tval > 0) || (vprev < 0 && tval < 0)) &&
			    100.*(double)abs(vprev-tval)/(double)vmax(abs(vprev),abs(tval)) <= ((ModVArch&)owner().archivator()).roundProc() )
			tval = vprev;
		}
		value.assign((char*)&tval,vSize);
		break; 
	    }
	    case TFld::Real:	
	    { 
		double tval = buf.getR(&ibeg,true); 		
 		if( ((ModVArch&)owner().archivator()).roundProc() && vpos_end >= 0 )
		{		    
		    double vprev = *(double*)value_end.c_str();
		    if( ((vprev > 0 && tval > 0) || (vprev < 0 && tval < 0)) &&
		    	    100.*fabs(vprev-tval)/vmax(fabs(vprev),fabs(tval)) <= ((ModVArch&)owner().archivator()).roundProc() )
			tval = vprev;
		}
		value.assign((char*)&tval,vSize); 
		break; 
	    }
	    case TFld::String:
	    {
		value = buf.getS(&ibeg,true); 
		if(!value.size()) value = " ";
		if(value.size() >= 1<<(vSize*8))
		    value.erase((1<<(vSize*8))-1);
		break;
	    }
	}
		
	int pos_cur = (ibeg-begin())/period();
	if( vpos_end < 0 ) { vpos_beg = pos_cur; vpos_end = vpos_beg-1; }
	int pos_i = vpos_end;
	while(pos_i < pos_cur)
	{                      
	    pos_i++;
	    string wr_val=(pos_i==pos_cur)?value:eVal;
	    if( vpos_end < vpos_beg || wr_val != value_end )
	    {
		if(fixVl)
		{
		    int b_n = (pos_i/8)-(vpos_beg/8);
		    pid_b[b_n] = pid_b[b_n]|(0x01<<(pos_i%8));
		    val_b.append(wr_val);
		}
		else
		{
		    int v_sz = wr_val.size();
		    for(int v_psz = 0; v_psz < vSize; v_psz++ )
		    	pid_b[vSize*(pos_i-vpos_beg)+v_psz] = *(((char *)&v_sz)+v_psz);
		    val_b.append(wr_val);
		}
		if(vpos_end < vpos_beg ) value_first = wr_val;
		value_end = wr_val;
	    }
	    vpos_end = pos_i;
	}
	ibeg++;
    }
	
    res.request(true);
    //- Open archive file -
    int hd = open(name().c_str(),O_RDWR);
    if( hd <= 0 ) { m_err = true; return; }

    //- Get block geometry from file -
    int foff_beg_len, foff_beg, foff_begprev_len, foff_begprev, foff_end_len, foff_end, foff_endnext_len, foff_endnext;
    if( vpos_beg ) 
	foff_begprev = calcVlOff(hd,vpos_beg-1,&foff_begprev_len,true);
    foff_beg = calcVlOff(hd,vpos_beg,&foff_beg_len,true);    
    foff_end = calcVlOff(hd,vpos_end,&foff_end_len,true);
    if( vpos_beg < mpos )
	foff_endnext = calcVlOff(hd,vpos_end+1,&foff_endnext_len,true);
    //printf("TEST 01: off_beg=%d; off_end=%d\n",foff_beg,foff_end);    
    
    //- Checking and adaptation border -
    if(fixVl)
    {
    	//-- Check begin border --	
	if( vpos_beg )
	{
	    if( getValue(hd,foff_begprev,vSize) == value_first )
	    {
		pid_b[0] &= ~(0x01<<(vpos_beg%8));
		if( getPkVal(hd,vpos_beg) )	foff_beg-=vSize;
	    }
	    else if( !getPkVal(hd,vpos_beg) )	foff_beg+=vSize;
	}
	//--Check end border --
    	if( vpos_end < mpos ) 
	{
	    if( getValue(hd,foff_endnext,vSize) == value_end )
	    {
		if( getPkVal(hd,vpos_end+1) )
		{
		    foff_end+=2*vSize;
		    setPkVal(hd,vpos_end+1,0);
		}
		else foff_end+=vSize;
	    }
	    else 
	    {
		if( getPkVal(hd,vpos_end+1) ) foff_end+=vSize;
		else setPkVal(hd,vpos_end+1,1);
	    }
	}
	//Merge begin and end pack values	
     	char tmp_pb;
	lseek(hd,sizeof(FHead)+vpos_beg/8,SEEK_SET);
	read(hd,&tmp_pb,1);
	pid_b[0] |= (~(0xFF<<(vpos_beg%8)))&tmp_pb;
	lseek(hd,sizeof(FHead)+vpos_end/8,SEEK_SET);
	read(hd,&tmp_pb,1);
	pid_b[vpos_end/8-vpos_beg/8] |= (0xFE<<(vpos_end%8))&tmp_pb;
    }
    else
    {
     	//-- Check begin border --	
	if( vpos_beg )
	{
	    if( getValue(hd,foff_begprev,foff_begprev_len) == value_first )
	    {
		for( int i_sz = 0; i_sz < vSize; i_sz++ ) pid_b[i_sz] = 0;
		if( getPkVal(hd,vpos_beg) )	foff_beg-=foff_begprev_len;
	    }
	    else if( !getPkVal(hd,vpos_beg) ) foff_beg+=foff_begprev_len;
	}
	//--Check end border --
    	if( vpos_end < mpos )
	{
	    if( getValue(hd,foff_endnext,foff_endnext_len) == value_end )
	    {
		if( getPkVal(hd,vpos_end+1) )
		{
		    foff_end+=foff_end_len+foff_endnext_len;
		    setPkVal(hd,vpos_end+1,0);
		}
		else foff_end+=foff_end_len;		
	    }
	    else 
	    {
		if( getPkVal(hd,vpos_end+1) ) foff_end+=foff_end_len;
		else setPkVal(hd,vpos_end+1,foff_endnext_len);
	    }
	}
    }    
    //- Write pack id buffer and value buffer -
    int pid_b_sz;
    if( fixVl )	
    {
	lseek(hd,sizeof(FHead)+vpos_beg/8,SEEK_SET);
	pid_b_sz = vpos_end/8 - vpos_beg/8 + 1;
    }
    else
    {
     	lseek(hd,sizeof(FHead)+vSize*vpos_beg,SEEK_SET);
	pid_b_sz = vSize*(vpos_end-vpos_beg+1);
    }
    write(hd,pid_b.c_str(),pid_b_sz);
    moveTail(hd,foff_end,foff_end+(val_b.size()-(foff_end-foff_beg)));
    lseek(hd,foff_beg,SEEK_SET);
    write(hd,val_b.c_str(),val_b.size());
    
    //printf("TEST 04: beg=%d; end=%d\n",vpos_beg,vpos_end);
    //printf("TEST 05: Move off_beg=%d; off_end=%d; len=%d\n",foff_end,foff_end+(val_b.size()-(foff_end-foff_beg)),val_b.size());
    
    //- Drop cache -    
    cacheDrop(vpos_beg);
    cacheSet(vpos_end,foff_beg+val_b.size()-value_end.size(),value_end.size(),true,true);
    
    //repairFile(hd,false);
    m_acces = time(NULL);

    m_size = lseek(hd,0,SEEK_END);

    close(hd);
}

string VFileArch::getValue( int hd, int voff, int vsz )
{ 
    string get_vl;
    get_vl.reserve(10);
    
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

void VFileArch::repairFile(int hd, bool fix)
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
	    mess_err(owner().archivator().nodePath().c_str(),
		_("Error archive file structure: <%s>. Margin = %d byte. Will try fix it!"),name().c_str(),dt);
	    //Fix file
	    if(fix)
	    {
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
	}
	else
	{
	    //In progress !!!!
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

void VFileArch::cacheDrop( int pos )
{
    for( int i_p = 0; i_p < cache.size(); i_p++ )
	if( cache[i_p].pos >= pos ) 
	{ cache.erase(cache.begin()+i_p); i_p--; }
    if( cach_pr_rd.pos >= pos ) 
	cach_pr_rd.off = cach_pr_rd.pos = cach_pr_rd.vsz = 0;
    if( cach_pr_wr.pos >= pos ) 
	cach_pr_wr.off = cach_pr_wr.pos = cach_pr_wr.vsz = 0;
}

