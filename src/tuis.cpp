
//OpenSCADA file: tuis.cpp
/***************************************************************************
 *   Copyright (C) 2003-2020 by Roman Savochenko, <roman@oscada.org>       *
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

#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

#include "tsys.h"
#include "tmess.h"
#include "tuis.h"

#if HAVE_GD_FORCE
# include <gd.h>
#endif

using namespace OSCADA;

//*************************************************
//* TUIS                                          *
//*************************************************
TUIS::TUIS( ) : TSubSYS(SUI_ID,_("User Interfaces"), true)
{
#if HAVE_GD_FORCE
    gdFTUseFontConfig(1);
#endif
}

string TUIS::optDescr( )
{
    return TSYS::strMess(_(
	"===================== Subsystem \"User interfaces\" options ===============\n\n")) + TSubSYS::optDescr();
}

void TUIS::load_( )
{
    //Load parameters from command line

    //Load parameters from config-file
}

string TUIS::icoGet( const string &inm, string *tp, bool retPath )
{
    unsigned iT = 0;
    int hd = -1;
    string rez, pathi;
    char types[][5] = {"png", "gif", "jpg", "jpeg"};

    for(int off = 0; hd == -1 && (pathi=TSYS::strParse(SYS->icoDir(),0,";",&off)).size(); )
	for(iT = 0; iT < sizeof(types)/5; iT++)
	    if((hd=open((pathi+"/"+inm+"."+types[iT]).c_str(),O_RDONLY)) != -1) break;
    if(hd != -1) {
	if(tp) *tp = types[iT];
	if(retPath) rez = pathi+"/"+inm+"."+types[iT];
	else {
	    char buf[prmStrBuf_SZ];
	    for(int len = 0; (len=read(hd,buf,sizeof(buf))) > 0; ) rez.append(buf,len);
	}
	close(hd);
    }

    return rez;
}

string TUIS::docGet( const string &inm, string *tp, unsigned opt )
{
    unsigned iTr = 0, iT = 0;
    string rez, pathi, nm = TSYS::strParse(inm, 0, "|");
    vector<string> transl;

    //Find the offline document on the filesystem
    int hd = -1;
    char types[][5] = {"pdf", "html", "odt"};
    transl.push_back(""); transl.push_back(Mess->lang2Code()); transl.push_back("en");

    for(int off = 0; nm.size() && hd == -1 && (pathi=TSYS::strParse(SYS->docDir(),0,";",&off)).size(); )
	for(iTr = 0; iTr < transl.size(); ++iTr) {
	    for(iT = 0; iT < sizeof(types)/5; ++iT)
		if((hd=open((pathi+"/"+transl[iTr]+"/"+nm+"."+types[iT]).c_str(),O_RDONLY)) != -1) break;
	    if(hd != -1) break;
	}
    if(hd != -1) {
	if(tp) *tp = types[iT];
	if(opt&GetPathURL) rez = pathi+"/"+transl[iTr]+"/"+nm+"."+types[iT];
	else if(opt&GetContent) {
	    char buf[prmStrBuf_SZ];
	    for(int len = 0; (len=read(hd,buf,sizeof(buf))) > 0; ) rez.append(buf,len);
	} else rez = "xdg-open "+pathi+"/"+transl[iTr]+"/"+nm+"."+types[iT]+" &";
	close(hd);
    }

    // Detect the LTS and connect to the network copy of the offline documentation
    if(rez.empty() && (nm=TSYS::strParse(inm,0,"|")).size() && TSYS::strParse(inm,1,"|").size()) {
	TArrayObj *tArr;
	if((tArr=TRegExp("(0.\\d+|\\d+)\\.\\d+").match(VERSION))) {
	    if(tArr->size() >= 2) {
		const char  *docHost = "ftp.oscada.org/OpenSCADA";

		if(opt&GetPathURL) rez = string("http://") + docHost + "/" + tArr->arGet(1).getS() + "/doc/en/" + nm + ".html";
		else if(opt&GetContent) ;//rez = req.text();
		else rez = string("xdg-open ") + "http://" + docHost + "/" + tArr->arGet(1).getS() + "/doc/en/" + nm + ".html &";
	    }
	    delete tArr;
	}
    }

    //Use the online document into the network
    if(rez.empty() && (nm=TSYS::strParse(inm,1,"|")).size()) {
	const char  *docHost = "oscada.org/wiki/Special:MyLanguage";

	if(opt&GetPathURL) rez = string("http://") + docHost + "/" + nm;
	else if(opt&GetContent) ;//rez = req.text();
	else rez = string("xdg-open ") + "http://" + docHost + "/" + nm + " &";
    }

    return rez;
}

string TUIS::docKeyGet( const string &itxt )
{
    TArrayObj *rez;
    if((rez=TRegExp("DOC:\\s*(.+)$").match(itxt))) {
	if(rez->size() >= 2) return rez->arGet(1).getS();
	delete rez;
    }
    return "";
}

string TUIS::mimeGet( const string &inm, const string &fDt, const string &orig )
{
    string prc = TSYS::strParse(orig,0,";"), stvl;

    //First init to empty orig
    if(prc.empty() || TSYS::pathLev(prc,1).empty())
	prc = "file/"+ ((inm.rfind(".")==string::npos)?"unknown":inm.substr(inm.rfind(".")+1));

    //Adjust to group for used and known ones
    stvl = TSYS::pathLev(prc, 1);
    const char *tvl = stvl.c_str();
    // Text
    if(strcasecmp(tvl,"txt") == 0)					prc = "text/plain";
    else if(strcasecmp(tvl,"xml") == 0)					prc = "text/xml";
    else if(strcasecmp(tvl,"html") == 0)				prc = "text/html";
    else if(strcasecmp(tvl,"css") == 0)					prc = "text/css";
    else if(strcasecmp(tvl,"js") == 0)					prc = "text/javascript";
    else if(strcasecmp(tvl,"sgml") == 0)				prc = "text/sgml";
    else if(strcasecmp(tvl,"docbook") == 0)				prc = "text/docbook";
    else if(strcasecmp(tvl,"csv") == 0)					prc = "text/csv";
    else if(strcasecmp(tvl,"diff") == 0)				prc = "text/diff";
    else if(strcasecmp(tvl,"log") == 0)					prc = "text/log";
    else if(strcasecmp(tvl,"rtf") == 0)					prc = "text/rtf";
    else if(strcasecmp(tvl,"ics") == 0)					prc = "text/calendar";
    else if(strcasecmp(tvl,"vcs") == 0)					prc = "text/vcalendar";
    else if(strcasecmp(tvl,"vcf") == 0 || strcasecmp(tvl,"vct") == 0)	prc = "text/vcard";
    // Images
    else if(strcasecmp(tvl,"png") == 0)					prc = "image/png";
    else if(strcasecmp(tvl,"jpg") == 0 || strcasecmp(tvl,"jpeg") == 0)	prc = "image/jpg";
    else if(strcasecmp(tvl,"gif") == 0)					prc = "image/gif";
    else if(strcasecmp(tvl,"tif") == 0 || strcasecmp(tvl,"tiff") == 0)	prc = "image/tiff";
    else if(strcasecmp(tvl,"xpm") == 0)					prc = "image/xpm";
    else if(strcasecmp(tvl,"ico") == 0)					prc = "image/ico";
    else if(strcasecmp(tvl,"pcx") == 0)					prc = "image/pcx";
    else if(strcasecmp(tvl,"bmp") == 0)					prc = "image/bmp";
    else if(strcasecmp(tvl,"svg") == 0 || strcasecmp(tvl,"svg+xml") == 0) prc = "image/svg+xml";
		//string("image/") + ((fDt.find("<?xml ") == string::npos) ? "svg" : "svg+xml");
    // Audio
    else if(strcasecmp(tvl,"wav") == 0)	prc = "audio/wav";
    else if(strcasecmp(tvl,"ogg") == 0)	prc = "audio/ogg";
    else if(strcasecmp(tvl,"mp2") == 0)	prc = "audio/mp2";
    else if(strcasecmp(tvl,"mp3") == 0)	prc = "audio/mp3";
    // Video
    else if(strcasecmp(tvl,"mng") == 0)	prc = "video/mng";
    else if(strcasecmp(tvl,"ogm") == 0)	prc = "video/ogm";
    else if(strcasecmp(tvl,"avi") == 0)	prc = "video/avi";
    else if(strcasecmp(tvl,"mp4") == 0)	prc = "video/mp4";
    else if(strcasecmp(tvl,"mpeg") == 0)prc = "video/mpeg";
    else if(strcasecmp(tvl,"mkv") == 0)	prc = "video/matroska";

    return prc + ((stvl=TSYS::strParse(orig,1,";")).size()?";"+stvl:"");
}

void TUIS::cntrCmdProc( XMLNode *opt )
{
    //Get page info
    if(opt->name() == "info") {
	TSubSYS::cntrCmdProc(opt);
	return;
    }
    //Process command to page
    string a_path = opt->attr("path");
    TSubSYS::cntrCmdProc(opt);
}

//*************************************************
//* TUI                                           *
//*************************************************
TUI::TUI( const string &id ) : TModule(id), runSt(false)
{

}

void TUI::cntrCmdProc( XMLNode *opt )
{
    //Get page info
    if(opt->name() == "info") {
	TModule::cntrCmdProc(opt);
	if(ctrMkNode("area",opt,0,"/prm",_("User interface")))
	    if(ctrMkNode("area",opt,-1,"/prm/st",_("State")))
		ctrMkNode("fld",opt,-1,"/prm/st/r_st",_("Running"),RWRWR_,"root",SUI_ID,1,"tp","bool");
	return;
    }

    //Process command to page
    string a_path = opt->attr("path");
    if(a_path == "/prm/st/r_st") {
	if(ctrChkNode(opt,"get",RWRWR_,"root",SUI_ID,SEC_RD))	opt->setText(runSt?"1":"0");
	if(ctrChkNode(opt,"set",RWRWR_,"root",SUI_ID,SEC_WR))	s2i(opt->text()) ? modStart() : modStop();
    }
    else TModule::cntrCmdProc(opt);
}
