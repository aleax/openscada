
//OpenSCADA system module Archive.FSArch file: mess.cpp
/***************************************************************************
 *   Copyright (C) 2003-2007 by Roman Savochenko                           *
 *   rom_as@fromru.com                                                     *
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

#include <sys/time.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <dirent.h>
#include <errno.h>

#include <tsys.h>
#include "base.h"
#include "mess.h"


using namespace FSArch;

//************************************************
//* FSArch::ModMArch - Messages archivator       *
//************************************************
ModMArch::ModMArch( const string &iid, const string &idb, TElem *cf_el ) :
    TMArchivator(iid,idb,cf_el), tm_calc(0.0), m_lst_check(0),
    m_use_xml(cfg("FSArchXML").getBd()), m_max_size(cfg("FSArchMSize").getId()),
    m_numb_files(cfg("FSArchNFiles").getId()), m_time_size(cfg("FSArchTmSize").getId()),
    m_chk_tm(cfg("FSArchTm").getId()), m_pack_tm(cfg("FSArchPackTm").getId())
{

}

ModMArch::~ModMArch( )
{
    try{ stop(); }catch(...){}
}

void ModMArch::start()
{
    //- First scan dir -
    checkArchivator();
    run_st = true;
}

void ModMArch::stop()
{
    //- Clear archive files list -
    ResAlloc res(m_res,true);
    while(arh_s.size())
    {
	delete arh_s[0];
	arh_s.pop_front();
    }

    run_st = false;
}

time_t ModMArch::begin()
{
    ResAlloc res( m_res, false );
    for( int i_arh = arh_s.size()-1; i_arh >= 0; i_arh-- ) 
	if( !arh_s[i_arh]->err() ) return arh_s[i_arh]->begin();

    return 0;
}

time_t ModMArch::end()
{
    ResAlloc res( m_res, false );
    for( int i_arh = 0; i_arh < arh_s.size(); i_arh++ )
	if( !arh_s[i_arh]->err() ) return arh_s[i_arh]->end();

    return 0;
}

void ModMArch::put( vector<TMess::SRec> &mess )
{
    unsigned long long t_cnt = SYS->shrtCnt();

    ResAlloc res(m_res,false);

    if(!run_st) throw TError(nodePath().c_str(),_("Archive no started!"));
    for( unsigned i_m = 0; i_m < mess.size(); i_m++)
    {
	if( !chkMessOK(mess[i_m].categ,mess[i_m].level) ) continue;

	int i_arh;
	for( i_arh = 0; i_arh < arh_s.size(); i_arh++) 
	    if( !arh_s[i_arh]->err() && mess[i_m].time >= arh_s[i_arh]->begin() )
	    {
		if( i_arh == 0 && 
		    ((m_max_size && arh_s[i_arh]->size() > m_max_size*1024) ||
		     (mess[i_m].time >= arh_s[i_arh]->begin()+m_time_size*24*60*60)) ) break;
		try{ arh_s[i_arh]->put(mess[i_m]); }
		catch(TError err)
		{ mess_err(err.cat.c_str(),err.mess.c_str()); continue; }
		i_arh = -1;
		break;
	    }
	//- If going a new data then create new file -
	if( i_arh >= 0 )
	{
	    res.release();

	    res.request(true);
	    //-- Create new Archive --
	    char c_buf[30];
	    time_t tm = time(NULL);
	    strftime(c_buf,sizeof(c_buf),"/%F %T.msg",localtime(&tm));
	    try{ arh_s.push_front( new MFileArch( addr()+c_buf, mess[i_m].time, this, Mess->charset(), useXML() ) ); }
	    catch(TError err)
	    {
		mess_crit(nodePath().c_str(),_("Error create new archive file <%s>!"),(addr()+c_buf).c_str() );
		return;
	    }
	    res.release();
	    //-- Allow parallel read access --
	    res.request(false);
	    arh_s[0]->put(mess[i_m]);
	}
    }
    tm_calc = 1.0e3*((double)(SYS->shrtCnt()-t_cnt))/((double)SYS->sysClk());
}

void ModMArch::get( time_t b_tm, time_t e_tm, vector<TMess::SRec> &mess, const string &category, char level )
{
    if( e_tm <= b_tm ) return;
    if(!run_st) throw TError(nodePath().c_str(),_("Archive no started!"));

    ResAlloc res(m_res,false);
    for( int i_arh = arh_s.size()-1; i_arh >= 0 && mess.size() < TArchiveS::max_req_mess; i_arh-- )
    {
	if( !arh_s[i_arh]->err() &&
		!( (b_tm < arh_s[i_arh]->begin() && e_tm < arh_s[i_arh]->begin() ) ||
		   (b_tm > arh_s[i_arh]->end() && e_tm > arh_s[i_arh]->end() ) ) )
	    arh_s[i_arh]->get(b_tm, e_tm, mess, category, level);
    }
}

void ModMArch::checkArchivator( bool now )
{
    if( now || time(NULL) > m_lst_check + checkTm()*60 )
    {
	struct stat file_stat;
	dirent *scan_dirent;

	DIR *IdDir = opendir(addr().c_str());
	if(IdDir == NULL)
	{
	    if( mkdir(addr().c_str(),0777) )
		throw TError(nodePath().c_str(),_("Can not create dir <%s>."),addr().c_str());
	    IdDir = opendir(addr().c_str());
	}
	//- Clean scan flag -
	ResAlloc res(m_res,false);
	for( unsigned i_arh = 0; i_arh < arh_s.size(); i_arh++)
	    arh_s[i_arh]->scan = false;
	res.release();

	while((scan_dirent = readdir(IdDir)) != NULL)
	{
	    if( string("..") == scan_dirent->d_name || string(".") == scan_dirent->d_name ) continue;
	    string NameArhFile = addr()+"/"+scan_dirent->d_name;
	    stat(NameArhFile.c_str(),&file_stat);
	    if( (file_stat.st_mode&S_IFMT) != S_IFREG || access(NameArhFile.c_str(),F_OK|R_OK) != 0) continue;
	    //-- Check all files --
	    int i_arh;
	    res.request(false);
	    for( i_arh = 0; i_arh < arh_s.size(); i_arh++)
		if( arh_s[i_arh]->name() == NameArhFile )
		{
		    arh_s[i_arh]->scan = true;
		    res.release();
		    break;
		}
	    //-- Registre new archive file --
	    if( i_arh >= arh_s.size() )
	    {
		res.release();

		MFileArch *f_arh = new MFileArch(this);
		f_arh->attach( NameArhFile, false );
		f_arh->scan = true;

		res.request(true);
		//--- Oldest and broken arhives to down ---
		if( f_arh->err() )	arh_s.push_back( f_arh );
		else
		{
		    int i_arh;
		    for( i_arh = 0; i_arh < arh_s.size(); i_arh++)
			if( arh_s[i_arh]->err() || f_arh->begin() >= arh_s[i_arh]->begin() )
			{
			    arh_s.insert(arh_s.begin()+i_arh,f_arh);
			    break;
			}
		    if( i_arh >= arh_s.size() ) arh_s.push_back( f_arh );
		}
		res.release();
	    }
	}

	closedir(IdDir);

	//- Check deleting Archives -
	res.request(true);
	for( unsigned i_arh = 0; i_arh < arh_s.size(); i_arh++) 
	    if( !arh_s[i_arh]->scan )
	    {
		delete arh_s[i_arh];
		arh_s.erase( arh_s.begin() + i_arh );
		i_arh--;
	    }
	res.release();

	//- Check file count and delete odd files -
	if( m_numb_files )
	{
	    int f_cnt = 0;	//Work files number
	    for( int i_arh = 0; i_arh < arh_s.size(); i_arh++)
		if( !arh_s[i_arh]->err() ) f_cnt++;
	    if( f_cnt > m_numb_files )
	    {
		//-- Delete oldest files --
		for( int i_arh = arh_s.size()-1; i_arh >= 0; i_arh-- )
		    if( f_cnt <= m_numb_files )	break;
		    else if( !arh_s[i_arh]->err() )
		    {
			string f_nm = arh_s[i_arh]->name();	//Deleted file name
			delete arh_s[i_arh];
			arh_s.erase( arh_s.begin() + i_arh );
			remove(f_nm.c_str());
			f_cnt--;
		    }
	    }
	}
	m_lst_check = time(NULL);
    }

    //- Call files checking -
    ResAlloc res(m_res,false);
    for( int i_arh = 0; i_arh < arh_s.size(); i_arh++)
        arh_s[i_arh]->check();
}

int ModMArch::size()
{
    int rez = 0;
    ResAlloc res(m_res,false);
    for( unsigned i_arh = 0; i_arh < arh_s.size(); i_arh++)
        rez+=arh_s[i_arh]->size();

    return rez;
}

void ModMArch::cntrCmdProc( XMLNode *opt )
{
    //- Get page info -
    if( opt->name() == "info" )
    {
	TMArchivator::cntrCmdProc(opt);
	ctrMkNode("fld",opt,-1,"/prm/st/fsz",_("Archive files size (kB)"),0444,"root","Archive",1,"tp","real");
	ctrMkNode("fld",opt,-1,"/prm/st/tarch",_("Archiving time (msek)"),0444,"root","Archive",1,"tp","real");
	if(ctrMkNode("area",opt,1,"/bs",_("Additional options"),0444,"root","Archive"))
	{
	    ctrMkNode("fld",opt,-1,"/bs/xml",cfg("FSArchXML").fld().descr(),0664,"root","Archive",1,"tp","bool");
	    ctrMkNode("fld",opt,-1,"/bs/sz",cfg("FSArchMSize").fld().descr(),0664,"root","Archive",1,"tp","dec");
	    ctrMkNode("fld",opt,-1,"/bs/fl",cfg("FSArchNFiles").fld().descr(),0664,"root","Archive",1,"tp","dec");
	    ctrMkNode("fld",opt,-1,"/bs/len",cfg("FSArchTmSize").fld().descr(),0664,"root","Archive",1,"tp","dec");
	    ctrMkNode("fld",opt,-1,"/bs/pcktm",cfg("FSArchPackTm").fld().descr(),0664,"root","Archive",1,"tp","dec");
	    ctrMkNode("fld",opt,-1,"/bs/tm",cfg("FSArchTm").fld().descr(),0664,"root","Archive",1,"tp","dec");
	    ctrMkNode("comm",opt,-1,"/bs/chk_nw",_("Check archivator directory now"),0660,"root","Archive");
	}
	return;
    }

    //- Process command to page -
    string a_path = opt->attr("path");
    if( a_path == "/prm/st/fsz" && ctrChkNode(opt) )		opt->setText(TSYS::real2str((double)size()/1024.,6));
    else if( a_path == "/prm/st/tarch" && ctrChkNode(opt) )	opt->setText(TSYS::real2str(tm_calc,6));
    else if( a_path == "/bs/xml" )
    {
	if( ctrChkNode(opt,"get",0664,"root","Archive",SEQ_RD) )	opt->setText( useXML() ? "1" : "0" );
	if( ctrChkNode(opt,"set",0664,"root","Archive",SEQ_WR) )	setUseXML( atoi(opt->text().c_str()) );
    }
    else if( a_path == "/bs/sz" )
    {
	if( ctrChkNode(opt,"get",0664,"root","Archive",SEQ_RD) )	opt->setText(TSYS::int2str( maxSize() ));
	if( ctrChkNode(opt,"set",0664,"root","Archive",SEQ_WR) )	setMaxSize( atoi(opt->text().c_str()) );
    }
    else if( a_path == "/bs/fl" )
    {
	if( ctrChkNode(opt,"get",0664,"root","Archive",SEQ_RD) )	opt->setText(TSYS::int2str( numbFiles() ));
	if( ctrChkNode(opt,"set",0664,"root","Archive",SEQ_WR) )	setNumbFiles( atoi(opt->text().c_str()) );
    }
    else if( a_path == "/bs/len" )
    {
	if( ctrChkNode(opt,"get",0664,"root","Archive",SEQ_RD) )	opt->setText(TSYS::int2str( timeSize() ));
	if( ctrChkNode(opt,"set",0664,"root","Archive",SEQ_WR) )	setTimeSize( atoi(opt->text().c_str()) );
    }
    else if( a_path == "/bs/pcktm" )
    {
	if( ctrChkNode(opt,"get",0664,"root","Archive",SEQ_RD) )	opt->setText(TSYS::int2str( packTm() ));
	if( ctrChkNode(opt,"set",0664,"root","Archive",SEQ_WR) )	setPackTm( atoi(opt->text().c_str()) );
    }
    else if( a_path == "/bs/tm" )
    {
	if( ctrChkNode(opt,"get",0664,"root","Archive",SEQ_RD) )	opt->setText(TSYS::int2str( checkTm() ));
	if( ctrChkNode(opt,"set",0664,"root","Archive",SEQ_WR) )	setCheckTm( atoi(opt->text().c_str()) );
    }
    else if( a_path == "/bs/chk_nw" && ctrChkNode(opt,"set",0660,"root","Archive",SEQ_WR) )	checkArchivator(true);
    else TMArchivator::cntrCmdProc(opt);
}

//*************************************************
//* FSArch::MFileArch - Messages archivator file  *
//*************************************************
MFileArch::MFileArch( ModMArch *owner ) : 
    m_xml(true), m_owner(owner), scan(false), m_err(false), m_write(false), m_load(false), m_pack(false),
    m_size(0), m_chars("UTF-8"), m_beg(0), m_end(0), m_node(NULL)
{
    cach_pr.tm = cach_pr.off = 0;
}

MFileArch::MFileArch( const string &iname, time_t ibeg, ModMArch *iowner, const string &icharset, bool ixml ) :
    m_xml(ixml), m_owner(iowner), scan(false), m_err(false), m_write(false), m_load(false), m_pack(false),
    m_size(0), m_name(iname), m_chars(icharset), m_beg(ibeg), m_end(ibeg), m_node(NULL)
{
    cach_pr.tm = cach_pr.off = 0;

    int hd = open( name().c_str(),O_RDWR|O_CREAT|O_TRUNC, 0666 );
    if(hd <= 0) throw TError(owner().nodePath().c_str(),_("Can not create file: <%s>!"),name().c_str());

    if( xmlM() )
    {
	//- Prepare XML file -
	m_chars = "UTF-8";
	m_node = new XMLNode();

	m_node->clear();
	m_node->setName(mod->modId());
	m_node->setAttr("Version",mod->modInfo("Version"));
	m_node->setAttr("Begin",TSYS::int2str(m_beg,TSYS::Hex));
	m_node->setAttr("End",TSYS::int2str(m_end,TSYS::Hex));
	string x_cf = "<?xml version=\"1.0\" encoding=\"UTF-8\" ?>" + m_node->save(XMLNode::BrOpenPrev);
	if( write(hd,x_cf.c_str(),x_cf.size()) < 0 )
	    throw TError(owner().nodePath().c_str(),_("Write to file error: %s"),strerror(errno));
    }
    else
    {
	//- Prepare plain text file -
	char s_buf[STR_BUF_LEN];
	snprintf(s_buf,sizeof(s_buf),"%s %s %8s %8x %8x\n",
	    mod->modId().c_str(),mod->modInfo("Version").c_str(),m_chars.c_str(),m_beg,m_end);
	if( write(hd,s_buf,strlen(s_buf)) < 0 )
	    throw TError(owner().nodePath().c_str(),_("Write to file error: %s"),strerror(errno));
    }
    close(hd);
    m_load = true;
}

MFileArch::~MFileArch()
{
    check();	//Check XML-archive

    if( m_node ) delete m_node;
}

void MFileArch::attach( const string &iname, bool full )
{
    char buf[STR_BUF_LEN];
    ResAlloc res(m_res,true);

    m_err = false;
    m_load = false;
    m_name = iname;
    m_pack = mod->filePack(name());
    m_acces = time(NULL);

    //- Check archive and unpack if want -
    if( m_pack )
    {
	//-- Get file info from DB --
	TConfig c_el(&mod->packFE());
	c_el.cfg("FILE").setS(name());
	if(SYS->db().at().dataGet(mod->filesDB(),mod->nodePath()+"Pack/",c_el))
	{
	    m_beg = strtol(c_el.cfg("BEGIN").getS().c_str(),NULL,16);
	    m_end = strtol(c_el.cfg("END").getS().c_str(),NULL,16);
	    m_chars = c_el.cfg("PRM1").getS();
	    m_xml = atoi(c_el.cfg("PRM2").getS().c_str());

	    if( !m_xml || (m_xml && !full) )
	    {
	        //--- Get file size ---
	        int hd = open(name().c_str(),O_RDONLY);
		if( hd > 0 )
		{
		    m_size = lseek(hd,0,SEEK_END);
		    close(hd);
		}
		return;
	    }
	}
	m_name = mod->unPackArch(name());
	m_pack = false;
    }

    FILE *f = fopen(name().c_str(),"r");
    if( f == NULL ) { m_err = true; return; }

    try
    {
	char s_char[9];
	//- Check to plain text archive -
	if( fgets(buf,sizeof(buf),f) == NULL )
	    throw TError(owner().nodePath().c_str(),_("File %s header error!"),name().c_str());
	string s_tmpl = mod->modId()+"%*s %8s %x %x";
	if( sscanf(buf,s_tmpl.c_str(),s_char,&m_beg,&m_end) == 3 )
	{
	    //-- Attach plain text archive file --
	    m_chars = s_char;
	    m_xml = false;
	    m_load = true;
	    fseek(f,0,SEEK_END);
	    m_size = ftell(f);

	    //-- Delete Node tree --
	    if( m_node ) delete m_node;
	    m_node = NULL;

	    fclose(f);
	}
	else
	{
	    if( !m_node ) m_node = new XMLNode();
	    fseek(f,0,SEEK_SET);
	    if( full )
	    {
		//-- Load and parse all file --
		int r_cnt;
		string s_buf;
		
		//-- Read full file to buffer --
		while( r_cnt = fread(buf,1,sizeof(buf),f) )
		    s_buf.append(buf,r_cnt);
		fclose(f); f = NULL;

		//-- Parse full file --
		m_node->load(s_buf);
		if( m_node->name() != mod->modId() )
		{
		    mess_err(owner().nodePath().c_str(),_("No my Archive file: <%s>"),name().c_str());
		    m_node->clear();
		    m_err = true;
		    return;
		}
		m_size = s_buf.size();
		m_write = false;
		m_chars = "UTF-8";
		m_beg = strtol( m_node->attr("Begin").c_str(),(char **)NULL,16);
		m_end = strtol( m_node->attr("End").c_str(),(char **)NULL,16);
		m_load = true;
		m_xml = true;
		return;
	    }
	    else
	    {
		//-- Process only archive header --
		char c;
		string prm, val;

		do
		{
		    while( (c = fgetc(f)) != '<' && c != EOF );
		    if( c == EOF )
		    {
			mess_err(owner().nodePath().c_str(),_("Archive <%s> file error."),name().c_str());
			m_err = true;
			fclose(f);
			return;
		    }
		    prm.clear();
		    while( (c = fgetc(f)) != ' ' && c != '\t' && c != '>' && c != EOF ) prm+=c;
		    if( c == EOF )
		    {
			mess_err(owner().nodePath().c_str(),_("Archive <%s> file error."),name().c_str());
			m_err = true;
			fclose(f);
			return;
		    }
		} while( prm != mod->modId() );
		//-- Go to --
		while( true )
		{
		    prm.clear();
		    val.clear();
		    while( (c = fgetc(f)) == ' ' || c == '\t' );
		    if( c == '>' || c == EOF ) break;
		    while( c != '=' && c != '>' && c != EOF )	{ prm+=c; c = fgetc(f); }
		    while( (c = fgetc(f)) != '"' && c != '>' && c != EOF );
		    while( (c = fgetc(f)) != '"' && c != '>' && c != EOF )  val+=c;

		    if( prm == "Begin" )	m_beg = strtol(val.c_str(),NULL,16);
		    else if( prm == "End" )	m_end = strtol(val.c_str(),NULL,16);
		}
		fseek(f,0,SEEK_END);
		m_size = ftell(f);
		fclose(f);
		m_write = false;
		m_load = false;
		m_xml = true;
		return;
	    }
	}
    }
    catch( TError err )
    {
	mess_err(err.cat.c_str(),"%s",err.mess.c_str());
	if( m_node ) delete m_node;
	m_node = NULL;
	m_err = true;
	if( f )	fclose(f);
    }
}

void MFileArch::put( TMess::SRec mess )
{
    if( m_err ) throw TError(owner().nodePath().c_str(),_("Put message to error Archive file!"));
    if( m_pack ) { m_name = mod->unPackArch(name()); m_pack = false; }
    if( !m_load )
    {
	attach( m_name );
	if( m_err || !m_load )
	{
	    m_err = true;
	    throw TError(owner().nodePath().c_str(),_("Archive file '%s' isn't attaching!"),m_name.c_str());
	}
    }

    ResAlloc res(m_res,true);
    m_acces = time(NULL);

    if( xmlM() )
    {
	int i_ch;
	for( i_ch = 0; i_ch < m_node->childSize(); i_ch++)
	    if( strtol( m_node->childGet(i_ch)->attr("tm").c_str(),(char **)NULL,16) > mess.time )
		break;

	XMLNode *cl_node = m_node->childIns(i_ch,"m");
	cl_node->setAttr("tm",TSYS::int2str(mess.time,TSYS::Hex));
	cl_node->setAttr("lv",TSYS::int2str(mess.level));
	cl_node->setAttr("cat",mess.categ);
	cl_node->setText(mess.mess);
	if( mess.time > m_end ) 
	{
	    m_end = mess.time;
	    m_node->setAttr("End",TSYS::int2str(m_end,TSYS::Hex));
	}
	m_write = true;
    }
    else
    {
	char buf[STR_BUF_LEN];
	//- Check to empty category and message
	if( !mess.categ.size() )	mess.categ = " ";
	if( !mess.mess.size() )		mess.mess = " ";
	//- Open file -
	FILE *f = fopen(m_name.c_str(),"r+");
	if( f == NULL ) { m_err = true; return; }
	if( mess.time >= m_end )
	{
	    //- Update header -
	    if( mess.time != m_end )
	    {
		m_end = mess.time;
		snprintf(buf,sizeof(buf),"%s %s %8s %8x %8x\n",
		    mod->modId().c_str(),mod->modInfo("Version").c_str(),m_chars.c_str(),m_beg,m_end);
		fwrite(buf,strlen(buf),1,f);
	    }
	    //- Prepare and put mess to end file -
	    snprintf(buf,sizeof(buf),"%8x %d %s %s\n",mess.time,mess.level,
		Mess->codeConvOut(m_chars,TSYS::strEncode(mess.categ,TSYS::Custom," \n\t%")).c_str(),
		Mess->codeConvOut(m_chars,TSYS::strEncode(mess.mess,TSYS::Custom," \n\t%")).c_str());
	    fseek(f,0,SEEK_END);
	    fwrite(buf,strlen(buf),1,f);
	}
	else
	{
	    time_t m_tm = 0;
	    //- Put into file -
	    //-- Get want position --
	    long c_off = cacheGet(mess.time);
	    if(c_off) fseek(f,c_off,SEEK_SET);
	    else fgets(buf,sizeof(buf),f);
	    //-- Check mess records --
	    int pass_cnt = 0;
	    time_t last_tm = 0;
	    while(fgets(buf,sizeof(buf),f) != NULL)
	    {
		sscanf(buf,"%x %*d",&m_tm);
		if( m_tm > mess.time )
		{
		    int prev_m_len = strlen(buf);
		    //--- Prepare message ---
		    snprintf(buf,sizeof(buf),"%8x %d %s %s\n",mess.time,mess.level,
			Mess->codeConvOut(m_chars,TSYS::strEncode(mess.categ,TSYS::Custom," \n\t%")).c_str(),
			Mess->codeConvOut(m_chars,TSYS::strEncode(mess.mess,TSYS::Custom," \n\t%")).c_str());
		    string s_buf = buf;
		    //--- Move tail at a new message size ---
		    int mv_beg = ftell(f)-prev_m_len;
		    fseek(f,0,SEEK_END);
		    int mv_end = ftell(f);
		    int beg_cur;
		    do
		    {
			beg_cur = ((mv_end-mv_beg)>=sizeof(buf))?mv_end-sizeof(buf):mv_beg;
			fseek(f,beg_cur,SEEK_SET);
			fread(buf,mv_end-beg_cur,1,f);
			fseek(f,beg_cur+s_buf.size(),SEEK_SET);
			fwrite(buf,mv_end-beg_cur,1,f);
			mv_end-=sizeof(buf);
		    }
		    while(beg_cur!=mv_beg);
		    //--- Write a new message ---
		    fseek(f,mv_beg,SEEK_SET);
		    fwrite(s_buf.c_str(),s_buf.size(),1,f);
		    cacheUpdate(mess.time,s_buf.size());
		    //--- Put last value to cache ---
		    cacheSet(mess.time,mv_beg,true);
		    break;
		}
		else if((pass_cnt++) > CACHE_POS && m_tm != last_tm)
		{
		    //--- Add too big position to cache ---
		    cacheSet(m_tm,ftell(f)-strlen(buf));
		    pass_cnt = 0;
		}
		last_tm = m_tm;
	    }
	}
	fseek(f,0,SEEK_END);
	m_size = ftell(f);
	fclose(f);
    }
}

void MFileArch::get( time_t b_tm, time_t e_tm, vector<TMess::SRec> &mess, const string &category, char level )
{
    TMess::SRec b_rec;

    if( m_err ) throw TError(owner().nodePath().c_str(),_("Get messages from error Archive file!"));
    if( m_pack ){ m_name = mod->unPackArch(name()); m_pack = false; }
    if( !m_load )
    {
	attach( m_name );
	if( m_err || !m_load )	throw TError(owner().nodePath().c_str(),_("Archive file isn't attaching!"));
    }

    ResAlloc res(m_res,false);
    m_acces = time(NULL);

    if( xmlM() )
    {
	for( int i_ch = 0; i_ch < m_node->childSize() && mess.size() < TArchiveS::max_req_mess; i_ch++)
	{
	    //- Find messages -
	    b_rec.time  = strtol( m_node->childGet(i_ch)->attr("tm").c_str(),(char **)NULL,16);
	    b_rec.categ = m_node->childGet(i_ch)->attr("cat");
	    b_rec.level = (TMess::Type)atoi( m_node->childGet(i_ch)->attr("lv").c_str() );
	    b_rec.mess  = m_node->childGet(i_ch)->text();
	    if( b_rec.time >= b_tm && b_rec.time < e_tm && b_rec.level >= level && TMess::chkPattern(b_rec.categ,category) )
	    {
		bool equal = false;
		int i_p = mess.size();
		for( int i_m = mess.size()-1; i_m >= 0; i_m-- )
		{
		    if( mess[i_m].time > b_rec.time )	i_p = i_m;
		    else if( b_rec.time == mess[i_m].time && 
			    b_rec.level == mess[i_m].level &&
			    b_rec.mess == mess[i_m].mess )
		    { equal = true; break; }
		    else if( mess[i_m].time < b_rec.time ) break;
		}
		if( !equal ) mess.insert(mess.begin()+i_p,b_rec);
	    }
	}
    }
    else
    {
	char buf[STR_BUF_LEN];
	//- Open file -
	FILE *f = fopen(m_name.c_str(),"r");
	if( f == NULL ) { m_err = true; return; }

	//- Get want position -
	long c_off = cacheGet(b_tm);
	if(c_off) fseek(f,c_off,SEEK_SET);
	else fgets(buf,sizeof(buf),f);
	//- Check mess records -
	int pass_cnt = 0;
	time_t last_tm = 0;
	while( fgets(buf,sizeof(buf),f) != NULL && mess.size() < TArchiveS::max_req_mess )
	{
	    sscanf(buf,"%x %d",&b_rec.time,&b_rec.level);
	    if( b_rec.time >= e_tm ) break;
	    if( b_rec.time >= b_tm )
	    {
		if( b_rec.level < level ) continue;
		char m_cat[1001], m_mess[1001];
		sscanf(buf,"%*x %*d %1000s %1000s",m_cat,m_mess);
		b_rec.categ = TSYS::strDecode(Mess->codeConvIn(m_chars,m_cat),TSYS::HttpURL);
		b_rec.mess  = TSYS::strDecode(Mess->codeConvIn(m_chars,m_mess),TSYS::HttpURL);
		if( !TMess::chkPattern(b_rec.categ,category) ) continue;
		//-- Check to equal messages and inserting --
		bool equal = false;
		int i_p = mess.size();
		for( int i_m = mess.size()-1; i_m >= 0; i_m-- )
		    if( mess[i_m].time > b_rec.time )   i_p = i_m;
		    else if( b_rec.time == mess[i_m].time &&
			    b_rec.level == mess[i_m].level &&
			    b_rec.mess == mess[i_m].mess )
			{ equal = true; break; }
		    else if( mess[i_m].time < b_rec.time ) break;
		if( !equal ) mess.insert(mess.begin()+i_p,b_rec);
	    }
	    else if((pass_cnt++) > CACHE_POS && b_rec.time != last_tm)
	    {
		cacheSet(b_rec.time,ftell(f)-strlen(buf));
		pass_cnt = 0;
	    }
	    last_tm = b_rec.time;
	}
	fclose(f);
    }
}

void MFileArch::check( bool free )
{
    ResAlloc res(m_res,true);
    if( !m_err && m_load && xmlM() )
    {
	if( m_write )
	{
	    int hd = open( m_name.c_str(),O_RDWR|O_TRUNC );
	    if(hd > 0 )
	    {
		string x_cf = "<?xml version=\"1.0\" encoding=\"UTF-8\" ?>" + m_node->save(XMLNode::BrOpenPrev);
		m_size = x_cf.size();
		write(hd,x_cf.c_str(),m_size);
		close(hd);
		m_write = false;
	    }
	}
	//- Free memory of XML-archive after 10 minets -
	if( time(NULL) > m_acces + owner().packTm()*30 || free )
	{
	    m_node->clear();
	    m_load = false;
	}
    }
    //- Check for pack archive file -
    if( !m_err && !m_pack && owner().packTm() && (time(NULL) > m_acces + owner().packTm()*60) && ((xmlM() && !m_load) || !xmlM()) )
    {
	m_name = mod->packArch(name());
	m_pack = true;
	//-- Get file size --
	int hd = open(name().c_str(),O_RDONLY);
	if( hd > 0 )
	{
	    m_size = lseek(hd,0,SEEK_END);
	    close(hd);
	}
	//-- Write info to DB --
	TConfig c_el(&mod->packFE());
	c_el.cfg("FILE").setS(name());
	c_el.cfg("BEGIN").setS(TSYS::int2str(begin(),TSYS::Hex));
	c_el.cfg("END").setS(TSYS::ll2str(end(),TSYS::Hex));
	c_el.cfg("PRM1").setS(charset());
	c_el.cfg("PRM2").setS(TSYS::int2str(xmlM()));
	SYS->db().at().dataSet(mod->filesDB(),mod->nodePath()+"Pack/",c_el);
    }
}

long MFileArch::cacheGet( time_t tm )
{
    CacheEl rez = {0,0};
    for( int i_c = cache.size()-1; i_c >= 0; i_c-- )
	if( tm >= cache[i_c].tm ) { rez = cache[i_c]; break; }
    if( tm >= cach_pr.tm && cach_pr.tm >= rez.tm )
	rez = cach_pr;

    return rez.off;
}

void MFileArch::cacheSet( time_t tm, long off, bool last )
{
    CacheEl el = { tm, off };

    if( !last )
    {
	for( int i_c = 0; i_c < cache.size(); i_c++ )
	    if( el.tm == cache[i_c].tm )	{ cache[i_c] = el; return; }
	    else if( el.tm < cache[i_c].tm )	{ cache.insert(cache.begin()+i_c,el); return; }
	cache.push_back(el);
    }
    else cach_pr = el;
}

void MFileArch::cacheUpdate( time_t tm, long v_add )
{
    for( int i_c = 0; i_c < cache.size(); i_c++ )
	if( cache[i_c].tm > tm ) cache[i_c].off+=v_add;
    if( cach_pr.tm > tm ) cach_pr.off += v_add;
}
