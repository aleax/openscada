
//OpenSCADA system module UI.WebVision file: web_vision.cpp
/***************************************************************************
 *   Copyright (C) 2007 by Roman Savochenko                                *
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

#include <getopt.h>
#include <signal.h>

#include <config.h>
#include <tsys.h>
#include <tmess.h>

#include "web_vision.h"

//************************************************
//* Modul info!                                  *
#define MOD_ID		"WebVision"
#define MOD_NAME	"Operation user interface (WEB)"
#define MOD_TYPE	"UI"
#define VER_TYPE	VER_UI
#define SUB_TYPE	"WWW"
#define MOD_VERSION	"0.3.0"
#define AUTORS		"Roman Savochenko"
#define DEVELOPERS	"Roman Savochenko, Lysenko Maxim, Yashina Kseniya"
#define DESCRIPTION	"Web operation user interface for visual control area (VCA) projects playing."
#define LICENSE		"GPL"
//************************************************

WebVision::TWEB *WebVision::mod;

extern "C"
{
    TModule::SAt module( int n_mod )
    {
	if( n_mod==0 )	return TModule::SAt(MOD_ID,MOD_TYPE,VER_TYPE);
	return TModule::SAt("");
    }

    TModule *attach( const TModule::SAt &AtMod, const string &source )
    {
	if( AtMod == TModule::SAt(MOD_ID,MOD_TYPE,VER_TYPE) )
	    return new WebVision::TWEB( source );
	return NULL;
    }
}

using namespace WebVision;

//************************************************
//* TWEB                                         *
//************************************************
TWEB::TWEB( string name ) : m_t_auth(10), chck_st(false)
{
    mId		= MOD_ID;
    mName	= MOD_NAME;
    mType	= MOD_TYPE;
    mVers	= MOD_VERSION;
    mAutor	= AUTORS;
    mDescr	= DESCRIPTION;
    mLicense	= LICENSE;
    mSource	= name;

    mod		= this;
    id_vcases	= grpAdd("ses_");

    //- Reg export functions -
    modFuncReg( new ExpFunc("void HttpGet(const string&,string&,const string&,vector<string>&);",
        "Process Get comand from http protocol's!",(void(TModule::*)( )) &TWEB::HttpGet) );
    modFuncReg( new ExpFunc("void HttpPost(const string&,string&,const string&,vector<string>&,const string&);",
        "Process Set comand from http protocol's!",(void(TModule::*)( )) &TWEB::HttpPost) );

    //- Create check sessions timer -
    struct sigevent sigev;
    sigev.sigev_notify = SIGEV_THREAD;
    sigev.sigev_value.sival_ptr = this;
    sigev.sigev_notify_function = TaskSessCheck;
    sigev.sigev_notify_attributes = NULL;
    timer_create(CLOCK_REALTIME,&sigev,&chkSessTm);

    //- Create named colors' container -
    colors["aliceblue"] = rgb(240, 248, 255);
    colors["antiquewhite"] = rgb(250, 235, 215);
    colors["aqua"] = rgb( 0, 255, 255);
    colors["aquamarine"] = rgb(127, 255, 212);
    colors["azure"] = rgb(240, 255, 255);
    colors["beige"] = rgb(245, 245, 220);
    colors["bisque"] = rgb(255, 228, 196);
    colors["black"] = rgb( 0, 0, 0);
    colors["blanchedalmond"] = rgb(255, 235, 205);
    colors["blue"] = rgb( 0, 0, 255);
    colors["blueviolet"] = rgb(138, 43, 226);
    colors["brown"] = rgb(165, 42, 42);
    colors["burlywood"] = rgb(222, 184, 135);
    colors["cadetblue"] = rgb( 95, 158, 160);
    colors["chartreuse"] = rgb(127, 255, 0);
    colors["chocolate"] = rgb(210, 105, 30);
    colors["coral"] = rgb(255, 127, 80);
    colors["cornflowerblue"] = rgb(100, 149, 237);
    colors["cornsilk"] = rgb(255, 248, 220);
    colors["crimson"] = rgb(220, 20, 60);
    colors["cyan"] = rgb( 0, 255, 255);
    colors["darkblue"] = rgb( 0, 0, 139);
    colors["darkcyan"] = rgb( 0, 139, 139);
    colors["darkgoldenrod"] = rgb(184, 134, 11);
    colors["darkgray"] = rgb(169, 169, 169);
    colors["darkgreen"] = rgb( 0, 100, 0);
    colors["darkgrey"] = rgb(169, 169, 169);
    colors["darkkhaki"] = rgb(189, 183, 107);
    colors["darkmagenta"] = rgb(139, 0, 139);
    colors["darkolivegreen"] = rgb( 85, 107, 47);
    colors["darkorange"] = rgb(255, 140, 0);
    colors["darkorchid"] = rgb(153, 50, 204);
    colors["darkred"] = rgb(139, 0, 0);
    colors["darksalmon"] = rgb(233, 150, 122);
    colors["darkseagreen"] = rgb(143, 188, 143);
    colors["darkslateblue"] = rgb( 72, 61, 139);
    colors["darkslategray"] = rgb( 47, 79, 79);
    colors["darkslategrey"] = rgb( 47, 79, 79);
    colors["darkturquoise"] = rgb( 0, 206, 209);
    colors["darkviolet"] = rgb(148, 0, 211);
    colors["deeppink"] = rgb(255, 20, 147);
    colors["deepskyblue"] = rgb( 0, 191, 255);
    colors["dimgray"] = rgb(105, 105, 105);
    colors["dimgrey"] = rgb(105, 105, 105);
    colors["dodgerblue"] = rgb( 30, 144, 255);
    colors["firebrick"] = rgb(178, 34, 34);
    colors["floralwhite"] = rgb(255, 250, 240);
    colors["forestgreen"] = rgb( 34, 139, 34);
    colors["fuchsia"] = rgb(255, 0, 255);
    colors["gainsboro"] = rgb(220, 220, 220);
    colors["ghostwhite"] = rgb(248, 248, 255);
    colors["gold"] = rgb(255, 215, 0);
    colors["goldenrod"] = rgb(218, 165, 32);
    colors["gray"] = rgb(128, 128, 128);
    colors["grey"] = rgb(128, 128, 128);
    colors["green"] = rgb( 0, 128, 0);
    colors["greenyellow"] = rgb(173, 255, 47);
    colors["honeydew"] = rgb(240, 255, 240);
    colors["hotpink"] = rgb(255, 105, 180);
    colors["indianred"] = rgb(205, 92, 92);
    colors["indigo"] = rgb( 75, 0, 130);
    colors["ivory"] = rgb(255, 255, 240);
    colors["khaki"] = rgb(240, 230, 140);
    colors["lavender"] = rgb(230, 230, 250);
    colors["lavenderblush"] = rgb(255, 240, 245);
    colors["lawngreen"] = rgb(124, 252, 0);
    colors["lemonchiffon"] = rgb(255, 250, 205);
    colors["lightblue"] = rgb(173, 216, 230);
    colors["lightcoral"] = rgb(240, 128, 128);
    colors["lightcyan"] = rgb(224, 255, 255);
    colors["lightgoldenrodyellow"] = rgb(250, 250, 210);
    colors["lightgray"] = rgb(211, 211, 211);
    colors["lightgreen"] = rgb(144, 238, 144);
    colors["lightgrey"] = rgb(211, 211, 211);
    colors["lightpink"] = rgb(255, 182, 193);
    colors["lightsalmon"] = rgb(255, 160, 122);
    colors["lightseagreen"] = rgb( 32, 178, 170);
    colors["lightskyblue"] = rgb(135, 206, 250);
    colors["lightslategray"] = rgb(119, 136, 153);
    colors["lightslategrey"] = rgb(119, 136, 153);
    colors["lightsteelblue"] = rgb(176, 196, 222);
    colors["lightyellow"] = rgb(255, 255, 224);
    colors["lime"] = rgb( 0, 255, 0);
    colors["limegreen"] = rgb( 50, 205, 50);
    colors["linen"] = rgb(250, 240, 230);
    colors["magenta"] = rgb(255, 0, 255);
    colors["maroon"] = rgb(128, 0, 0);
    colors["mediumaquamarine"] = rgb(102, 205, 170);
    colors["mediumblue"] = rgb( 0, 0, 205);
    colors["mediumorchid"] = rgb(186, 85, 211);
    colors["mediumpurple"] = rgb(147, 112, 219);
    colors["mediumseagreen"] = rgb( 60, 179, 113);
    colors["mediumslateblue"] = rgb(123, 104, 238);
    colors["mediumspringgreen"] = rgb( 0, 250, 154);
    colors["mediumturquoise"] = rgb( 72, 209, 204);
    colors["mediumvioletred"] = rgb(199, 21, 133);
    colors["midnightblue"] = rgb( 25, 25, 112);
    colors["mintcream"] = rgb(245, 255, 250);
    colors["mistyrose"] = rgb(255, 228, 225);
    colors["moccasin"] = rgb(255, 228, 181);
    colors["navajowhite"] = rgb(255, 222, 173);
    colors["navy"] = rgb( 0, 0, 128);
    colors["oldlace"] = rgb(253, 245, 230);
    colors["olive"] = rgb(128, 128, 0);
    colors["olivedrab"] = rgb(107, 142, 35);
    colors["orange"] = rgb(255, 165, 0);
    colors["orangered"] = rgb(255, 69, 0);
    colors["orchid"] = rgb(218, 112, 214);
    colors["palegoldenrod"] = rgb(238, 232, 170);
    colors["palegreen"] = rgb(152, 251, 152);
    colors["paleturquoise"] = rgb(175, 238, 238);
    colors["palevioletred"] = rgb(219, 112, 147);
    colors["papayawhip"] = rgb(255, 239, 213);
    colors["peachpuff"] = rgb(255, 218, 185);
    colors["peru"] = rgb(205, 133, 63);
    colors["pink"] = rgb(255, 192, 203);
    colors["plum"] = rgb(221, 160, 221);
    colors["powderblue"] = rgb(176, 224, 230);
    colors["purple"] = rgb(128, 0, 128);
    colors["red"] = rgb(255, 0, 0);
    colors["rosybrown"] = rgb(188, 143, 143);
    colors["royalblue"] = rgb( 65, 105, 225);
    colors["saddlebrown"] = rgb(139, 69, 19);
    colors["salmon"] = rgb(250, 128, 114);
    colors["sandybrown"] = rgb(244, 164, 96);
    colors["seagreen"] = rgb( 46, 139, 87);
    colors["seashell"] = rgb(255, 245, 238);
    colors["sienna"] = rgb(160, 82, 45);
    colors["silver"] = rgb(192, 192, 192);
    colors["skyblue"] = rgb(135, 206, 235);
    colors["slateblue"] = rgb(106, 90, 205);
    colors["slategray"] = rgb(112, 128, 144);
    colors["slategrey"] = rgb(112, 128, 144);
    colors["snow"] = rgb(255, 250, 250);
    colors["springgreen"] = rgb( 0, 255, 127);
    colors["steelblue"] = rgb( 70, 130, 180);
    colors["tan"] = rgb(210, 180, 140);
    colors["teal"] = rgb( 0, 128, 128);
    colors["thistle"] = rgb(216, 191, 216);
    colors["tomato"] = rgb(255, 99, 71);
    colors["turquoise"] = rgb( 64, 224, 208);
    colors["violet"] = rgb(238, 130, 238);
    colors["wheat"] = rgb(245, 222, 179);
    colors["white"] = rgb(255, 255, 255);
    colors["whitesmoke"] = rgb(245, 245, 245);
    colors["yellow"] = rgb(255, 255, 0);
    colors["yellowgreen"] = rgb(154, 205, 50);

    //- Default CSS init -
    m_CSStables =
	"hr {width:100%}\n"
	"body {background-color:#818181}\n"
	"h1.head {text-align:center; color:#ffff00 }\n"
	"h2.title {text-align:center; font-style:italic; margin: 0px; padding: 0px; border-width:0 }\n"
	"table.page_head {background-color:#cccccc; border:3px ridge blue; width:100% }\n"
	"table.page_head td.tool {text-align:center; border:1px solid blue; width:120px;  white-space: nowrap }\n"
	"table.page_head td.user {text-align:left; border:1px solid blue; width:120px; white-space: nowrap }\n"
	"table.page_area {background-color:#9999ff; border:3px ridge #a9a9a9; width:100%; padding:2px }\n"
	"table.page_area tr.content {background-color:#cccccc; border:5px ridge #9999ff; padding:5px }\n"
	"table.page_auth {background-color:#9999ff; border:3px ridge #a9a9a9; padding:2px }\n"
	"table.page_auth tr.content {background-color:#cccccc; border:5px ridge #9999ff; padding:5px }\n"
	".vertalign { display: table-cell; text-align: center; vertical-align: middle; }\n"
	".vertalign * { vertical-align: middle;	}\n";

    VCAjs =
	"/***************************************************\n"
	" * pathLev - Path parsing function.                *\n"
	" ***************************************************/\n"
	"pathLev.off = 0;\n"
	"function pathLev( path, level )\n"
	"{\n"
	"  var an_dir = pathLev.off;\n"
	"  var t_lev = 0;\n"
	"  var t_dir;\n"
	"  while( an_dir<path.length && path.charAt(an_dir)=='/' ) an_dir++;\n"
	"  if( an_dir >= path.length ) return '';\n"
	"  while( true )\n"
	"  {\n"
	"    t_dir = path.indexOf('/',an_dir);\n"
	"    if( t_dir < 0 ) { pathLev.off=path.length; return (t_lev==level)?path.substr(an_dir):''; }\n"
	"    if( t_lev==level ) { pathLev.off=t_dir; return path.substr(an_dir,t_dir-an_dir); }\n"
	"    an_dir = t_dir;\n"
	"    t_lev++;\n"
	"    while( an_dir<path.length && path.charAt(an_dir)=='/' ) an_dir++;\n"
	"  }\n"
	"}\n"
	"/***************************************************\n"
	" * nodeText - Get DOM node text                    *\n"
	" ***************************************************/\n"
	"function nodeText( node )\n"
	"{\n"
	"  var rez = '';\n"
	"  for( var i = 0; i < node.childNodes.length; i++ )\n"
	"    if( node.childNodes[i].nodeType == 3 ) rez += node.childNodes[i].data;\n"
	"  return rez;\n"
	"}\n"
	"/***************************************************\n"
	" * getXmlHttp - Check and return XMLHttpRequest for*\n"
	" *  various brousers.                              *\n"
	" ***************************************************/\n"
	"function getXmlHttp()\n"
	"{\n"
	"  if( window.XMLHttpRequest )	return new XMLHttpRequest();\n"
	"  else if( window.ActiveXObject )\n"
	"  {\n"
	"    try { return new ActiveXObject('Msxml2.XMLHTTP'); }\n"
	"    catch(e) { return new ActiveXObject('Microsoft.XMLHTTP'); }\n"
	"  }\n"
	"  return null;\n"
	"}\n"
	"/***************************************************\n"
	" * servGet - XML get request to server             *\n"
	" ***************************************************/\n"
	"function servGet( adr, prm, full )\n"
	"{\n"
	"  var req = getXmlHttp();\n"
	"  req.open('GET',encodeURI((full?'':'/"MOD_ID"')+adr+'?'+prm),false);\n"
	"  req.send(null);\n"
	"  if( req.status == 200 && req.responseXML.childNodes.length )\n"
	"    return req.responseXML.childNodes[0];\n"
	"}\n"
	"/***************************************************\n"
	" * servSet - XML set request to server             *\n"
	" ***************************************************/\n"
	"function servSet( adr, prm, body )\n"
	"{\n"
	"  var req = getXmlHttp();\n"
	"  req.open('POST',encodeURI('/"MOD_ID"'+adr+'?'+prm),true);\n"
	"  req.send(body);\n"
	"}\n"
	"/***************************************************\n"
	" * getWAttrs - request page/widget attributes      *\n"
	" ***************************************************/\n"
	"function getWAttrs( wId, tm )\n"
	"{\n"
	"  var rNode = servGet(wId,'com=attrs'+(tm?('&tm='+tm):''))\n"
	"  if( !rNode ) return;\n"
	"  var atrLs = new Object();\n"
	"  for( var i = 0; i < rNode.childNodes.length; i++ )\n"
	"    if( rNode.childNodes[i].nodeType == 1 )\n"
	"      atrLs[rNode.childNodes[i].getAttribute('id')] = nodeText(rNode.childNodes[i]);\n"
	"  return atrLs;\n"
	"}\n"
	"/***************************************************\n"
	" * setWAttrs - set page/widget attributes          *\n"
	" ***************************************************/\n"
	"function setWAttrs( wId, attrs, val )\n"
	"{\n"
	"  var body = '<set>';\n"
	"  if( typeof(attrs) == 'string' ) body+='<el id=\\''+attrs+'\\'>'+val+'</el>';\n"
	"  else for( var i in attrs ) body+='<el id=\\''+i+'\\'>'+attrs[i]+'</el>';\n"
	"  body+='</set>';\n"
	"  servSet(wId,'com=attrs',body);\n"
	"}\n"
	"/***************************************************\n"
	" * pwDescr - Page/widget descriptor object         *\n"
	" ***************************************************/\n"
	"function callPage( pgId, pgAttrs, upW )\n"
	"{\n"
	"  if( !pgId ) return true;\n"
	"  if( this == masterPage )\n"
	"  {\n"
	"    var opPg = this.findOpenPage(pgId);\n"
	"    if( opPg && upW )\n"
	"      for( var i = 0; i < upW.length; i++ )\n"
	"      {\n"
	"        pathLev.off = 0;\n"
	"        pathLev(upW[i],0);\n"
	"        opPg.makeEl(false,(upW[i].length<=pathLev.off)?'/':upW[i].substr(pathLev.off));\n"
	"      }\n"
	"    if( opPg ) return true;\n"
	"  }\n"
	"  if( !pgAttrs ) pgAttrs = getWAttrs(pgId);\n"
	"  var pgGrp = pgAttrs['pgGrp'];\n"
	"  var pgOpenSrc = pgAttrs['pgOpenSrc'];\n"
	"  if( !this.addr.length || (this == masterPage && pgGrp == 'main') || pgGrp == this.attrs['pgGrp'] )\n"
	"  {\n"
	"    this.addr  = pgId;\n"
	"    this.attrs = pgAttrs;\n"
	"    this.place = document.createElement('div');\n"
	"    this.makeEl(true,'');\n"
	"    document.body.appendChild(this.place);\n"
	"    //- Set project's icon and RunTime page title -\n"
	"    document.getElementsByTagName(\"link\")[0].setAttribute(\"href\",location.pathname+'?com=ico');\n" 
	"    return true;\n"
	"  }\n"
	"  //- Find for include page creation -\n"
	"  for( var i in this.wdgs )\n"
	"    if( this.wdgs[i].tp == 'Box' && pgGrp == this.wdgs[i].attrs['pgGrp'] && pgId != this.wdgs[i].attrs['pgOpenSrc'] )\n"
	"    {\n"
	"      this.wdgs[i].attrs['pgOpenSrc'] = pgId;\n"
	"      this.wdgs[i].makeEl(true,'');\n"
	"      return true;\n"
	"    }\n"
	"  //- Put checking to child pages -\n"
	"  for( var i = 0; i < this.pages.length; i++ )\n"
	"    if( this.pages[i].callPage(pgId,pgAttrs,upW) ) return true;\n"
	"  return false;\n"
	"}\n"
	"function findOpenPage( pgId )\n"
	"{\n"
	"  if( pgId == this.addr ) return this;\n"
	"  //- Check from included widgets -\n"
	"  for( var i in this.wdgs )\n"
	"    if( this.wdgs[i].tp == 'Box' && pgId == this.wdgs[i].attrs['pgOpenSrc'] )\n"
	"      return this.wdgs[i].pages[pgId];\n"
	"  //- Put checking to child pages -\n"
	"  for( var i = 0; i < this.pages.length; i++ )\n"
	"  {\n"
	"    var opPg = this.pages[i].findOpenPage(pgId);\n"
	"    if( opPg ) return opPg;\n"
	"  }\n"
	"}\n"
	"function makeEl( hot, el_addr )\n"
	"{\n"
	"  if( el_addr && el_addr != '/' )\n"
	"  {\n"
	"    pathLev.off = 0;\n"
	"    var pthEl = pathLev(el_addr,0);\n"
	"    this.wdgs[pthEl].makeEl(false,(el_addr.length<=pathLev.off)?'/':el_addr.substr(pathLev.off));\n"
	"    return;\n"
	"  }\n"
	"  var rNode;\n"
	"  if( !el_addr )\n"
	"  {\n"
	"    if( !(rNode = servGet(this.addr,'com=root')) ) return;\n"
	"    this.tp = nodeText(rNode);\n"
	"  }\n"
	"  var margBrdUpd = false;\n"
	"  if( !hot )\n"
	"  {\n"
	"    var tAttrs = getWAttrs(this.addr,el_addr?tmCnt:0);\n"
	"    for( var i in tAttrs )\n"
	"    {\n"
	"      if( (i == 'bordWidth' || i == 'geomMargin') && this.attrs[i] != tAttrs[i] ) margBrdUpd = true;\n"
	"      this.attrs[i] = tAttrs[i];\n"
	"    }\n"
	"  }\n"
	"  var elMargin = parseInt(this.attrs['geomMargin']);\n"
	"  var elBorder = 0;\n"
	"  if( this.attrs['bordWidth'] ) elBorder=parseInt(this.attrs['bordWidth']);\n"
	"  var elStyle = '';\n"
	"  if( !parseInt(this.attrs['en']) ) elStyle+='visibility : hidden; ';\n"
	"  var geomX = parseFloat(this.attrs['geomX']);\n"
	"  var geomY = parseFloat(this.attrs['geomY']);\n"
	"  if( this.pg ) geomX = geomY = 0;\n"
	"  else{ geomX *= this.parent.xScale(true); geomY *= this.parent.yScale(true); }\n"
	"  if( this.parent && !(this.pg && this.parent.pg) )\n"
	"  {\n"
	"    geomX -= parseInt(this.parent.attrs['geomMargin'])+parseInt(this.parent.attrs['bordWidth']);\n"
	"    geomY -= parseInt(this.parent.attrs['geomMargin'])+parseInt(this.parent.attrs['bordWidth']);\n"
	"  }\n"
	"  elStyle+='position: absolute; left: '+geomX.toFixed(0)+'px; top: '+geomY.toFixed(0)+'px; ';\n"
	"  var geomW = parseFloat(this.attrs['geomW'])-2*(elMargin+elBorder);\n"
	"  var geomH = parseFloat(this.attrs['geomH'])-2*(elMargin+elBorder);\n"
	"  if( this.pg && this.parent && this.parent.inclOpen && this.parent.inclOpen == this.addr )\n"
	"  {\n"
	"    elStyle += 'overflow: auto; ';\n"
	"    geomW = parseFloat(this.parent.attrs['geomW'])-2*(elMargin+elBorder);\n"
	"    geomH = parseFloat(this.parent.attrs['geomH'])-2*(elMargin+elBorder);\n"
	"  }\n"
	"  else elStyle += 'overflow: hidden; ';\n"
	"  var xSc = this.xScale(true);\n"
	"  var ySc = this.yScale(true);\n"
	"  geomW = (geomW*xSc).toFixed(0); geomH = (geomH*ySc).toFixed(0);\n"
	"  if( this.tp == 'ElFigure' )\n"
	"  {\n"
	"    if( this.attrs['backColor'] ) elStyle+='background-color: '+this.attrs['backColor']+'; ';\n"
	"    if( this.attrs['backImg'] )\n"
	"      elStyle+='background-image: url(\\'/"MOD_ID"'+this.addr+'?com=res&val='+this.attrs['backImg']+'\\'); ';\n"
	"    var figObj = this.place.childNodes[0];\n"
	"    if( !figObj )\n"
	"    {\n"
	"      figObj = document.createElement('img');\n"
	"      figObj.width = geomW; figObj.height = geomH;\n"
	"      figObj.border = 0;\n"
	"      this.place.appendChild(figObj);\n"
	"    }\n"
	"    figObj.src = '/"MOD_ID"'+this.addr+'?com=obj&tm='+tmCnt+'&xSc='+xSc.toFixed(2)+'&ySc='+ySc.toFixed(2);\n"
	"  }\n"
	"  else if( this.tp == 'Box' )\n"
	"  {\n"
	"    if( this.attrs['backColor'] ) elStyle+='background-color: '+this.attrs['backColor']+'; ';\n"
	"    if( this.attrs['backImg'] )   elStyle+='background-image: url(\\'/"MOD_ID"'+this.addr+'?com=res&val='+this.attrs['backImg']+'\\'); ';\n"
	"    elStyle+='border-style: solid; border-width: '+this.attrs['bordWidth']+'px; ';\n"
	"    if( this.attrs['bordColor'] ) elStyle+='border-color: '+this.attrs['bordColor']+'; ';\n"
	"    switch( parseInt(this.attrs['bordStyle']) )\n"
	"    {\n"
	"      case 1: elStyle+='border-style: dotted; '; break;\n"
	"      case 2: elStyle+='border-style: dashed; '; break;\n"
	"      case 3: elStyle+='border-style: solid; ';  break;\n"
	"      case 4: elStyle+='border-style: double; '; break;\n"
	"      case 5: elStyle+='border-style: groove; '; break;\n"
	"      case 6: elStyle+='border-style: ridge; ';  break;\n"
	"      case 7: elStyle+='border-style: inset; ';  break;\n"
	"      case 8: elStyle+='border-style: outset; '; break;\n"
	"    }\n"
	"    if( this.attrs['pgOpenSrc'] && !this.pages[this.attrs['pgOpenSrc']] )\n"
	"    {\n"
	"      if( this.inclOpen )\n"
	"      {\n"
	"        servSet(this.inclOpen,'com=pgClose','')\n"
	"        pgCache[this.inclOpen] = this.pages[this.inclOpen]\n"
	"        this.place.removeChild(this.pages[this.inclOpen].place);\n"
	"        this.pages[this.inclOpen].perUpdtEn(false);\n"
	"        delete this.pages[this.inclOpen];\n"
	"      }\n"
	"      this.inclOpen = this.attrs['pgOpenSrc'];\n"
	"      if( pgCache[this.inclOpen] )\n"
	"      {\n"
	"        this.pages[this.inclOpen] = pgCache[this.inclOpen];\n"
	"        this.place.appendChild(this.pages[this.inclOpen].place);\n"
	"        this.pages[this.inclOpen].perUpdtEn(true);\n"
	"        this.pages[this.inclOpen].makeEl(true,'');\n"
	"      }\n"
	"      else\n"
	"      {\n"
	"        var iPg = new pwDescr(this.inclOpen,true,this);\n"
	"        iPg.place = document.createElement('div');\n"
	"        iPg.makeEl(false,'');\n"
	"        this.pages[this.inclOpen] = iPg;\n"
	"        this.place.appendChild(iPg.place);\n"
	"      }\n"
	"    }\n"
	"  }\n"
	"  else if( this.tp == 'Text' )\n"
	"  {\n"
	"    if( this.attrs['backColor'] ) elStyle+='background-color: '+this.attrs['backColor']+'; ';\n"
	"    if( this.attrs['backImg'] )   elStyle+='background-image: url(\\'/"MOD_ID"'+this.addr+'?com=res&val='+this.attrs['backImg']+'\\'); ';\n"
	"    elStyle+='border-style: solid; border-width: '+this.attrs['bordWidth']+'px; ';\n"
	"    if( this.attrs['bordColor'] ) elStyle+='border-color: '+this.attrs['bordColor']+'; ';\n"
	"    var txtAlign = parseInt(this.attrs['alignment']);\n"
	"    switch(txtAlign&0x3)\n"
	"    {\n"
	"      case 0: elStyle+='text-align: left; '; break;\n"
	"      case 1: elStyle+='text-align: right; '; break;\n"
	"      case 2: elStyle+='text-align: center; '; break;\n"
	"      case 3: elStyle+='text-align: justify; '; break;\n"
	"    }\n"
	"    switch(txtAlign>>2)\n"
	"    {\n"
	"      case 0: elStyle+='vertical-align: top; '; break;\n"
	"      case 1: elStyle+='vertical-align: bottom; '; break;\n"
	"      case 2: elStyle+='vertical-align: middle; '; break;\n"
	"    }\n"
	"    var allFnt = this.attrs['font'].split(' ');\n"
	"    if( allFnt.length >= 1 ) elStyle+='font-family: '+allFnt[0].replace('_',' ')+'; ';\n"
	"    if( allFnt.length >= 2 ) elStyle+='font-size: '+(parseInt(allFnt[1])*Math.min(xSc,ySc)).toFixed(0)+'px; ';\n"
	"    if( allFnt.length >= 3 ) elStyle+='font-weight: '+(parseInt(allFnt[2])?'bold':'normal')+'; ';\n"
	"    if( allFnt.length >= 4 ) elStyle+='font-style: '+(parseInt(allFnt[3])?'italic':'normal')+'; ';\n"
	"    elStyle+='color: '+this.attrs['color']+'; ';\n"
	"    var txtVal = this.attrs['text'];\n"
	"    for( var i = 0; i < parseInt(this.attrs['numbArg']); i++ )\n"
	"    {\n"
	"      var argVal;\n"
	"      var argCfg = new Array();\n"
	"      switch(parseInt(this.attrs['arg'+i+'tp']))\n"
	"      {\n"
	"        case 0: case 2:\n"
	"          argCfg[0]=this.attrs['arg'+i+'cfg'];\n"
	"          argVal=this.attrs['arg'+i+'val'];\n"
	"          break;\n"
	"        case 1:\n"
	"          argCfg=this.attrs['arg'+i+'cfg'].split(';');\n"
	"          if( argCfg[1] == 'g' )      argVal=parseFloat(this.attrs['arg'+i+'val']).toPrecision(argCfg[2]);\n"
	"          else if( argCfg[1] == 'f' ) argVal=parseFloat(this.attrs['arg'+i+'val']).toFixed(argCfg[2]);\n"
	"          else argVal=this.attrs['arg'+i+'val'];\n"
	"          break;\n"
	"      }\n"
	"      var argSize = parseInt(argCfg[0]);\n"
	"      var argPad = '';\n"
	"      for( var j = argVal.length; j < Math.abs(argSize); j++ ) argPad+='&nbsp;';\n"
	"      if( argSize > 0 ) argVal=argPad+argVal; else argVal+=argPad;\n"
	"      txtVal = txtVal.replace('%'+(i+1),argVal);\n"
	"    }\n"
	"    var txtVal1 = '';\n"
	"    for( var j = 0; j < txtVal.length; j++ )\n"
	"      if( txtVal[j] == '\\n' ) txtVal1+='<br />'; else txtVal1+=txtVal[j];\n"
	//"    txtVal.replace(/\\n/g,'<br />');\n"
	"    while(this.place.childNodes.length) this.place.removeChild(this.place.childNodes[0]);\n"
	"    this.place.innerHTML = txtVal1;\n"
	"  }\n"
	"  else if( this.tp == 'Media' )\n"
	"  {\n"
	"    if( this.attrs['backColor'] ) elStyle+='background-color: '+this.attrs['backColor']+'; ';\n"
	"    if( this.attrs['backImg'] )\n"
	"      elStyle+='background-image: url(\\'/"MOD_ID"'+this.addr+'?com=res&val='+this.attrs['backImg']+'\\'); ';\n"
	"    elStyle+='border-style: solid; border-width: '+this.attrs['bordWidth']+'px; ';\n"
	"    if( this.attrs['bordColor'] ) elStyle+='border-color: '+this.attrs['bordColor']+'; ';\n"
	"    while(this.place.childNodes.length) this.place.removeChild(this.place.childNodes[0]);\n"
	"    var medObj = document.createElement('img');\n"
	"    medObj.src = this.attrs['src'].length ? ('/"MOD_ID"'+this.addr+'?com=res&val='+this.attrs['src']) : '';\n"
	"    medObj.border = 0;\n"
	"    if( this.attrs['fit'] == 1 ) { medObj.width = geomW; medObj.height = geomH; }\n"
	"    if( parseInt(this.attrs['active']) )\n"
	"    {\n"
	"      if( parseInt(this.attrs['areas']) )\n"
	"      {\n"
	"        var mapObj = document.createElement('map');\n"
	"        mapObj.name = this.addr;\n"
	"        for( var i = 0; i <  parseInt(this.attrs['areas']); i++ )\n"
	"        {\n"
	"          var arObj = document.createElement('area');\n"
	"          switch(parseInt(this.attrs['area'+i+'shp']))\n"
	"          {\n"
	"            case 0: arObj.shape = 'rect'; break;\n"
	"            case 1: arObj.shape = 'poly'; break;\n"
	"            case 2: arObj.shape = 'circle'; break;\n"
	"          }\n"
	"          arObj.coords = this.attrs['area'+i+'coord'];\n"
	"          arObj.title = this.attrs['area'+i+'title'];\n"
	"          arObj.href = '';\n"
	"          arObj.onclick = function() { setWAttrs(this.wdgLnk.addr,'event','ws_MapAct'+i+'Left'); return false; }\n"
	"          arObj.wdgLnk = this;\n"
	"          mapObj.appendChild(arObj);\n"
	"        }\n"
	"        this.place.appendChild(mapObj);\n"
	"        medObj.setAttribute('usemap','#'+this.addr);\n"
	"        this.place.appendChild(medObj);\n"
	"      }\n"
	"    }\n"
	"    else this.place.appendChild(medObj);\n"
	"  }\n"
	"  else if( this.tp == 'FormEl' )\n"
	"  {\n"
	"    var elTp = parseInt(this.attrs['elType']);\n"
	"    while(this.place.childNodes.length) this.place.removeChild(this.place.childNodes[0]);\n"
	"    var fontCfg = '';\n"
	"    if( this.attrs['font'] )\n"
	"    {\n"
	"      var allFnt = this.attrs['font'].split(' ');\n"
	"      if( allFnt.length >= 3 && parseInt(allFnt[2]) ) fontCfg += 'bold ';\n"
	"      if( allFnt.length >= 4 && parseInt(allFnt[3]) ) fontCfg += 'italic ';\n"	
	"      if( allFnt.length >= 2 ) fontCfg += (parseInt(allFnt[1])*Math.min(xSc,ySc)).toFixed(0)+'px ';\n"	
	"      if( allFnt.length >= 1 ) fontCfg += allFnt[0].replace('_',' ')+' ';\n"
	"    }\n"
	"    switch(elTp)\n"
	"    {\n"
	"      case 0:	//Line edit\n"
	"        var tblCell = document.createElement('div');\n"
	"        var formObj = document.createElement('input');\n"
	"        tblCell.className = 'vertalign';\n"
	"        tblCell.style.width = formObj.style.width = (geomW-4)+'px'; tblCell.style.height = geomH+'px';\n"
	"        formObj.type = 'text'; formObj.value = this.attrs['value'];\n"
	"        formObj.style.borderStyle = 'ridge'; formObj.style.borderWidth = '2px';\n"
	"        formObj.style.font = fontCfg;\n"
	"        tblCell.appendChild(formObj); this.place.appendChild(tblCell);\n"
	"        break;\n"
	"      case 1:	//Text edit\n"
	"        var formObj = document.createElement('textarea');\n"
	"        formObj.style.width = (geomW-4)+'px'; formObj.style.height = (geomH-4)+'px';\n"
	"        formObj.style.borderStyle = 'ridge'; formObj.style.borderWidth = '2px';\n"
	"        formObj.style.font = fontCfg;\n"
	"        formObj.appendChild(document.createTextNode(this.attrs['value']));\n"
	"        this.place.appendChild(formObj);\n"
	"        break;\n"
	"      case 2:	//Chek box\n"
	"        var tblCell = document.createElement('div');\n"
	"        var formObj = document.createElement('input');\n"
	"        var textObj = document.createTextNode('');\n"
	"        tblCell.className = 'vertalign';\n"
	"        tblCell.style.width = geomW+'px'; tblCell.style.height = geomH+'px';\n"
	"        tblCell.style.textAlign = 'left';\n"
	"        tblCell.style.font = fontCfg;\n"
	"        formObj.type='checkbox'; formObj.checked=parseInt(this.attrs['value']);\n"
	"        textObj.nodeValue = this.attrs['name'];\n"
	"        tblCell.appendChild(formObj); tblCell.appendChild(textObj); this.place.appendChild(tblCell);\n"
	"        break;\n"
	"      case 3:	//Button\n"
	"        var formObj;\n"
	"        var iconImg = this.attrs['img'];\n"
	"        if( iconImg )\n"
	"        {\n"
	"          formObj = document.createElement('div');\n"
	"          formObj.className = 'vertalign';\n"
	"          formObj.style.font = fontCfg;\n"
	"          elStyle+='border-style: outset; cursor: pointer; border-width: 2px; ';\n"
	"          if( this.attrs['color'] ) elStyle+='background-color: '+this.attrs['color']+'; ';\n"
	"          else elStyle+='background-color: snow; ';\n"
	"          this.place.onmouseup  = function() { this.style.borderStyle='outset'; };\n"
	"          this.place.onmousedown= function() { this.style.borderStyle='inset';  };\n"
	"          this.place.onmouseout = function() { this.style.borderStyle='outset'; };\n"
	"          this.place.onclick = function() { setWAttrs(this.wdgLnk.addr,'event','ws_BtPress'); };\n"
	"          this.place.wdgLnk = this;\n"
	"          var btImg = document.createElement('IMG');\n"
	"          btImg.src = '/"MOD_ID"'+this.addr+'?com=res&val='+this.attrs['img'];\n"
	"          btImg.width = btImg.height = Math.min(geomW-8,geomH-8);\n"
	"          btImg.float = 'left';\n"
	"          formObj.appendChild(btImg);\n"
	"          if( this.attrs['name'].length ) formObj.appendChild(document.createTextNode(this.attrs['name']));\n"
	"          geomW-=6; geomH-=6;\n"
	"        }\n"
	"        else\n"
	"        {\n"
	"          formObj = document.createElement('input');\n"
	"          formObj.style.font = fontCfg;\n"
	"          formObj.type='button';\n"
	"          formObj.value=this.attrs['name'];\n"
	"          formObj.onclick = function() { setWAttrs(this.wdgLnk.addr,'event','ws_BtPress'); }\n"
	"          formObj.wdgLnk = this;\n"
	"          if( this.attrs['color'] ) formObj.style.backgroundColor=this.attrs['color'];\n"
	"        }\n"
	"        formObj.style.width = geomW+'px'; formObj.style.height = geomH+'px';\n"
	"        this.place.appendChild(formObj);\n"
	"        break;\n"
	"      case 4: case 5:	//Combo box, List\n"
	"        var tblCell = document.createElement('div');\n"
	"        var formObj = document.createElement('select');\n"
	"        tblCell.className = 'vertalign';\n"
	"        tblCell.style.width = geomW+'px'; tblCell.style.height = geomH+'px';\n"
	"        formObj.style.width = (geomW-4)+'px';\n"
	"        formObj.style.borderStyle = 'ridge'; formObj.style.borderWidth = '2px';\n"
	"        formObj.style.font = fontCfg;\n"
	"        if( elTp == 5 )\n"
	"        {\n"
	"          formObj.style.height = (geomH-4)+'px';\n"
	"          formObj.setAttribute('size',100);\n"
	"        }\n"
	"        var selVal = this.attrs['value'];\n"
	"        var elLst = this.attrs['items'].split('\\n');\n"
	"        var optEl;\n"
	"        var selOk = false;\n"
	"        for( var i = 0; i < elLst.length; i++ )\n"
	"        {\n"
	"          optEl = document.createElement('option');\n"
	"          optEl.appendChild(document.createTextNode(elLst[i]));\n"
	"          if( selVal == elLst[i] ) selOk=optEl.defaultSelected=optEl.selected=true;\n"
	"          formObj.appendChild(optEl);\n"
	"        }\n"
	"        if( !selOk && elTp == 4 )\n"
	"        {\n" 
	"          optEl = document.createElement('option');\n"
	"          optEl.textContent = selVal;\n"
	"          optEl.selected = optEl.defaultSelected = true;\n"
	"          formObj.appendChild(optEl);\n"
	"        }\n"
	"        tblCell.appendChild(formObj); this.place.appendChild(tblCell);\n"
	"        break;\n"
	"    }\n"
	"  }\n"
	"  else if( this.tp == 'Diagram' )\n"
	"  {\n"
	"    if( this.attrs['backColor'] ) elStyle+='background-color: '+this.attrs['backColor']+'; ';\n"
	"    if( this.attrs['backImg'] )   elStyle+='background-image: url(\\'/"MOD_ID"'+this.addr+'?com=res&val='+this.attrs['backImg']+'\\'); ';\n"
	"    elStyle+='border-style: solid; border-width: '+this.attrs['bordWidth']+'px; ';\n"
	"    if( this.attrs['bordColor'] ) elStyle+='border-color: '+this.attrs['bordColor']+'; ';\n"
	"    var dgrObj = this.place.childNodes[0];\n"
	"    if( !dgrObj )\n"
	"    {\n"
	"      dgrObj = document.createElement('img');\n"
	"      dgrObj.border = 0;\n"
	"      this.place.appendChild(dgrObj);\n"
	"    }\n"
	"    dgrObj.src = '/"MOD_ID"'+this.addr+'?com=obj&tm='+tmCnt+'&xSc='+xSc.toFixed(2)+'&ySc='+ySc.toFixed(2);\n"
	"    this.perUpdtEn( this.isEnabled() && parseInt(this.attrs['trcPer']) );\n"
	"  }\n"
	"  elStyle+='width: '+geomW+'px; height: '+geomH+'px; z-index: '+this.attrs['geomZ']+'; margin: '+elMargin+'px; ';\n"
	"  this.place.style.cssText = elStyle;\n"
	"  if( el_addr && !margBrdUpd ) return;\n"
	"  rNode = servGet(this.addr,'com=chlds'); if( !rNode ) return;\n"
	"  for( var i = 0; i < rNode.childNodes.length; i++ )\n"
	"  {\n"
	"    var chEl = rNode.childNodes[i].getAttribute('id');\n"
	"    if( this.wdgs[chEl] ) this.wdgs[chEl].makeEl(hot,el_addr);\n"
	"    else\n"
	"    {\n"
	"      var wdgO = new pwDescr(this.addr+'/wdg_'+chEl,false,this);\n"
	"      wdgO.place = document.createElement('div');\n"
	"      wdgO.makeEl(false,'');\n"
	"      this.place.appendChild(wdgO.place);\n"
	"      this.wdgs[chEl] = wdgO;\n"
	"    }\n"
	"  }\n"
	"}\n"
	"function perUpdtEn( en )\n"
	"{\n"
	"  if( this.tp == 'Diagram' )\n"
	"  {\n"
	"     if( en && this.isEnabled() && !perUpdtWdgs[this.addr] && parseInt(this.attrs['trcPer']) ) perUpdtWdgs[this.addr] = this;\n"
	"     if( !en && perUpdtWdgs[this.addr] ) delete perUpdtWdgs[this.addr];\n"
	"  }\n"
	"  for( var i in this.wdgs ) this.wdgs[i].perUpdtEn(en);\n"
	"}\n"
	"function perUpdt( )\n"
	"{\n"
	"  if( this.tp == 'Diagram' && (this.updCntr % parseInt(this.attrs['trcPer'])) == 0 )\n"
	"  {\n"
	"    var dgrObj = this.place.childNodes[0];\n"
	"    if( dgrObj ) dgrObj.src = '/"MOD_ID"'+this.addr+'?com=obj&tm='+tmCnt+'&xSc='+this.xScale(true).toFixed(2)+'&ySc='+this.yScale(true).toFixed(2);\n"
	"  }\n"
	"  this.updCntr++;\n"
	"}\n"
	"function xScale( full )\n"
	"{\n"
	"  var rez = parseFloat(this.attrs['geomXsc'])\n"
	"  if( !full ) return rez;\n"
	"  if( !this.pg ) return rez*this.parent.xScale(full);\n"
	"  if( this != masterPage ) return masterPage.xScale()*rez;\n"
	"  return rez;\n"
	"}\n"
	"function yScale( full )\n"
	"{\n"
	"  var rez = parseFloat(this.attrs['geomYsc'])\n"
	"  if( !full ) return rez;\n"
	"  if( !this.pg ) return rez*this.parent.yScale(full);\n"
	"  if( this != masterPage ) return masterPage.yScale()*rez;\n"
	"  return rez;\n"
	"}\n"
	"function isEnabled( )\n"
	"{\n"
	"  var rez = parseInt(this.attrs['en']);\n"
	"  if( !rez || this.pg ) return rez;\n"
	"  return this.parent.isEnabled();\n"
	"}\n"
	"function pwDescr( pgAddr, pg, parent )\n"
	"{\n"
	"  this.addr = pgAddr;\n"
	"  this.pages = new Object();\n"
	"  this.wdgs = new Object();\n"
	"  this.attrs = new Object();\n"
	"  this.pg = pg;\n"
	"  this.tp = null;\n"
	"  this.parent = parent;\n"
	"  this.place = null;\n"
	"  this.callPage = callPage;\n"
	"  this.findOpenPage = findOpenPage;\n"
	"  this.makeEl = makeEl;\n"
	"  this.perUpdtEn = perUpdtEn;\n"
	"  this.perUpdt = perUpdt;\n"
	"  this.xScale = xScale;\n"
	"  this.yScale = yScale;\n"
	"  this.isEnabled = isEnabled;\n"
	"  this.updCntr = 0;\n"
	"}\n"
	"/***************************************************\n"
	" * makeUI                                          *\n"
	" ***************************************************/\n"
	"function makeUI()\n"
	"{\n"
	"  //- Get open pages list -\n"
	"  var pgNode = servGet(location.pathname,'com=pgOpenList&tm='+tmCnt,true)\n"
	"  if( pgNode )\n"
	"  {\n"
	"    var pags = pgNode.getElementsByTagName('pg');\n"
	"    for( var i = 0; i < pags.length; i++ )\n"
	"    {\n"
	"      var pgnm = nodeText(pags[i]);\n"
	"      var upW = new Array();\n"
	"      for( var j = 0; j < pags[i].childNodes.length; j++ )\n"
	"        if( pags[i].childNodes[j].nodeName == 'el' ) upW.push(nodeText(pags[i].childNodes[j]));\n"
	"      masterPage.callPage(pgnm,null,upW);\n"
	"    }\n"
	"    tmCnt = parseInt(pgNode.getAttribute('tm'));\n"
	"  }\n"
	"  //- Update some widgets -\n"
	"  for( var i in perUpdtWdgs ) perUpdtWdgs[i].perUpdt();\n"
	"  setTimeout(makeUI,1000);\n"
	"}\n"
	"/***************************************************\n"
	" * Main start code                                 *\n"
	" ***************************************************/\n"
	"tmCnt = 0;\n"
	"pgCache = new Object();\n"
	"perUpdtWdgs = new Object();\n"
	"masterPage = new pwDescr('',true);\n"
	"setTimeout(makeUI,10);\n";
}

TWEB::~TWEB()
{

}

string TWEB::modInfo( const string &name )
{
    if( name == "SubType" )		return SUB_TYPE;
    else if( name == _("Developers") )	return DEVELOPERS;
    else return TModule::modInfo(name);
}

void TWEB::modInfo( vector<string> &list )
{
    TModule::modInfo(list);
    list.push_back("SubType");
    list.push_back(_("Developers"));
}

void TWEB::vcaSesAdd( const string &name, bool isCreate )
{
    if( vcaSesPresent(name) )	return;
    chldAdd( id_vcases, new VCASess(name,isCreate) );
}

string TWEB::optDescr( )
{
    char buf[STR_BUF_LEN];

    snprintf(buf,sizeof(buf),_(
	"======================= The module <%s:%s> options =======================\n"
	"---------- Parameters of the module section <%s> in config file ----------\n\n"
	"SessTimeLife <time>      Time of the sesion life, minutes (default 10).\n"
	"CSSTables    <CSS>       CSS for creating pages.\n\n"),
	MOD_TYPE,MOD_ID,nodePath().c_str());

    return buf;
}

void TWEB::load_( )
{
    //- Load parameters from command line -
    int next_opt;
    char *short_opt="h";
    struct option long_opt[] =
    {
	{"help"      ,0,NULL,'h'},
	{NULL        ,0,NULL,0  }
    };

    optind=opterr=0;
    do
    {
	next_opt=getopt_long(SYS->argc,(char * const *)SYS->argv,short_opt,long_opt,NULL);
	switch(next_opt)
	{
	    case 'h': fprintf(stdout,optDescr().c_str()); break;
	    case -1 : break;
	}
    } while(next_opt != -1);

    //- Load parameters from config file -
    m_t_auth = atoi( TBDS::genDBGet(nodePath()+"SessTimeLife",TSYS::int2str(m_t_auth)).c_str() );
    m_CSStables = TBDS::genDBGet(nodePath()+"CSSTables",m_CSStables);
}

void TWEB::save_( )
{
    TBDS::genDBSet(nodePath()+"SessTimeLife",TSYS::int2str(m_t_auth));
    TBDS::genDBSet(nodePath()+"CSSTables",m_CSStables);
}

void TWEB::modStart( )
{
    //- Start interval timer for periodic thread creating of DB syncing -
    struct itimerspec itval;
    itval.it_interval.tv_sec = itval.it_value.tv_sec = 10;
    itval.it_interval.tv_nsec = itval.it_value.tv_nsec = 0;
    timer_settime(chkSessTm, 0, &itval, NULL);

    run_st = true;
}

void TWEB::modStop( )
{
    //- Stop interval timer for periodic thread creating -
    struct itimerspec itval;
    itval.it_interval.tv_sec = itval.it_interval.tv_nsec =
    itval.it_value.tv_sec = itval.it_value.tv_nsec = 0;
    timer_settime(chkSessTm, 0, &itval, NULL);
    if( TSYS::eventWait( chck_st, false, nodePath()+"chck_stop",5) )
	throw TError(nodePath().c_str(),_("Checking of session no stoped!"));

    run_st = false;
}

void TWEB::TaskSessCheck( union sigval obj )
{
    TWEB *web = (TWEB *)obj.sival_ptr;
    if( web->chck_st )  return;
    web->chck_st = true;

    time_t cur_tm = time(NULL);

    //- Check for opened sessions timeout close -
    vector<string> list;
    web->vcaSesList( list );
    for( int i_s = 0; i_s < list.size(); i_s++ )
	if( cur_tm > web->vcaSesAt(list[i_s]).at().lstReq()+web->authTime()*60 )
	    web->vcaSesDel(list[i_s]);

    //- Check for user auth session timeout -
    ResAlloc res(web->m_res,false);    
    for( map< int, SAuth >::iterator authEl = web->m_auth.begin(); authEl != web->m_auth.end(); )
	if( cur_tm > authEl->second.t_auth+web->authTime()*60 )
	    web->m_auth.erase(authEl++);
	else authEl++;

    web->chck_st = false;
}

string TWEB::httpHead( const string &rcode, int cln, const string &cnt_tp, const string &addattr )
{
    return  "HTTP/1.0 "+rcode+"\n"
	"Server: "+PACKAGE_STRING+"\n"
	"Accept-Ranges: bytes\n"
	"Content-Length: "+TSYS::int2str(cln)+"\n"
	"Connection: close\n"
	"Content-type: "+cnt_tp+"\n"
	"Charset="+Mess->charset()+"\n"+addattr+"\n";
}

string TWEB::pgHead( string head_els )
{
    string shead =
	"<?xml version='1.0' ?>\n"
	"<!DOCTYPE html PUBLIC '-//W3C//DTD XHTML 1.0 Transitional//EN'\n"
	"'DTD/xhtml1-transitional.dtd'>\n"
	"<html xmlns='http://www.w3.org/1999/xhtml'>\n"
	"<head>\n"
	"  <meta http-equiv='Content-Type' content='text/html; charset="+Mess->charset()+"'/>\n"
	"  <meta http-equiv='Cache-Control' content='no-store, no-cache, must-revalidate'/>\n"
	"  <meta http-equiv='Cache-Control' content='post-check=0, pre-check=0'/>\n"
	"  <meta http-equiv='Content-Script-Type' content='text/javascript'/>\n"
	"  <link rel='shortcut icon' href='/"MOD_ID"/ico' type='image' />\n"
	"  <title>"+PACKAGE_NAME+". "+_(MOD_NAME)+"</title>\n"
	"  <style type='text/css'>\n"+m_CSStables+"</style>\n"+
	head_els+
	"</head>\n"
	"<body alink='#33ccff' link='#3366ff' text='#000000' vlink='#339999'>\n";

    return shead;
}

string TWEB::pgTail( )
{
    return
	"</body>\n"
	"</html>";
}

void TWEB::HttpGet( const string &url, string &page, const string &sender, vector<string> &vars )
{
    SSess ses(TSYS::strDecode(url,TSYS::HttpURL),page,sender,vars,"");
    ses.page = pgHead();

    try
    {
	string zero_lev = TSYS::pathLev(ses.url,0);
	//- Get about module page -
	if( zero_lev == "about" )       getAbout(ses);
	//- Get module icon -
	else if( zero_lev == "ico" )
	{
	    string itp;
	    ses.page=TUIS::icoGet("UI."MOD_ID,&itp);
	    page = httpHead("200 OK",ses.page.size(),string("image/")+itp)+ses.page;
	    return;
	}
	else
	{
	    sesCheck(ses);
	    //- Auth dialog preparing -
	    if( !ses.user.size() )
	    {
		ses.page = ses.page+"<h1 class='head'>"PACKAGE_NAME". "+_(MOD_NAME)+"</h1>\n<hr/><br/>\n";
		getAuth( ses );
	    }
	    //- Session select or new session for project creation -
	    else if( zero_lev.empty() )
	    {
		bool sesPrjOk = false;
		ses.page = ses.page+
		    "<h1 class='head'>"+PACKAGE_NAME+". "+_(MOD_NAME)+"</h1>\n<hr/><br/>\n"
		    "<center><table class='page_auth'>\n";
		//-- Get present sessions list --
		XMLNode req("get");
		req.setAttr("path","/%2fses%2fses")->setAttr("chkUserPerm","1");
		cntrIfCmd(req,ses.user);
		if( req.childSize() )
		{
		    ses.page = ses.page+
			"<tr><td><b>"+_("Connect to opened session")+"</b></td></tr>\n"
			"<tr class='content'><td align='center'>\n"
			"<table border='0'>\n";
		    for( int i_ch = 0; i_ch < req.childSize(); i_ch++ )
			ses.page += "<tr><td><a href='/"MOD_ID"/ses_"+req.childGet(i_ch)->text()+"'>"+
			    req.childGet(i_ch)->text()+"</a></td></tr>";
		    ses.page += "</table></td></tr>\n";
		    sesPrjOk = true;
		}
		//-- Get present projects list --
		req.clear()->setAttr("path","/%2fprm%2fcfg%2fprj")->setAttr("chkUserPerm","1");
		cntrIfCmd(req,ses.user);
		if( req.childSize() )
		{
		    ses.page = ses.page +
			"<tr><td><b>"+_("Create new session for present project")+"</b></td></tr>\n"
			"<tr class='content'><td align='center'>\n"
			"<table border='0'>\n";
		    for( int i_ch = 0; i_ch < req.childSize(); i_ch++ )
			ses.page += "<tr><td><a href='/"MOD_ID"/prj_"+req.childGet(i_ch)->attr("id")+"'>"+
			    req.childGet(i_ch)->text()+"</a></td></tr>";
		    ses.page += "</table></td></tr>\n";
		    sesPrjOk = true;
		}
		ses.page += "</table></center>";
		if( !sesPrjOk )	messPost(ses.page,nodePath(),_("No one sessions and projects VCA engine present for user!"),TWEB::Warning);
	    }
	    //- New session creation -
	    else if( zero_lev.size() > 4 && zero_lev.substr(0,4) == "prj_" )
	    {
		XMLNode req("connect");
		req.setAttr("path","/%2fserv%2fsess")->setAttr("prj",zero_lev.substr(4));
		if( cntrIfCmd(req,ses.user) )
		    messPost(ses.page,req.attr("mcat").c_str(),req.text().c_str(),TWEB::Error);
		else
		{
		    ses.page = pgHead("<META HTTP-EQUIV='Refresh' CONTENT='0; URL=/"MOD_ID"/ses_"+req.attr("sess")+"'/>")+
			"<center>Open new session '"+req.attr("sess")+"' for project: '"+zero_lev.substr(4)+"'</center>\n<br/>";
		    vcaSesAdd(req.attr("sess"),true);
		}
	    }
	    //- Main session page data prepare -
	    else if( zero_lev.size() > 4 && zero_lev.substr(0,4) == "ses_" )
	    {
		string sesnm = zero_lev.substr(4);
		//-- Call to session --
		try{ vcaSesAt(sesnm).at().getReq(ses); }
		catch(...)
		{
		    if( !vcaSesPresent(sesnm) )
		    {
			vcaSesAdd(sesnm,false);
			vcaSesAt(sesnm).at().getReq(ses);
		    }
		    else throw;
		}
		page = ses.page;
		return;
	    }
	    else mess_err(nodePath().c_str(),_("No permit request is received: '%s'"),zero_lev.c_str());
	}
    }catch(TError err)
    {
	ses.page = "Page <"+ses.url+"> error: "+err.mess;
	page = httpHead("404 Not Found",ses.page.size())+ses.page;
	return;
    }

    ses.page += pgTail();
    page = httpHead("200 OK",ses.page.size())+ses.page;
}

void TWEB::getAbout( SSess &ses )
{
    ses.page = ses.page+"<center><table class='page_auth'>\n"
	"<TR><TD>"+PACKAGE+" "+VERSION+"</TD></TR>\n"
	"<TR class='content'><TD>\n"
	"<table border='0'>\n"
	"<TR><TD><font color='Blue'>"+_("Name: ")+"</font></TD><TD>OpenSCADA</TD></TR>\n"
	"<TR><TD><font color='Blue'>"+_("License: ")+"</font></TD><TD>GPL</TD></TR>\n"
	"<TR><TD><font color='Blue'>"+_("Author: ")+"</font></TD><TD>Roman Savochenko</TD></TR>\n"
	"</table>\n"
	"</TD></TR></table><br/>\n"
	"<table class='page_auth'>\n"
	"<TR><TD>"MOD_ID" "MOD_VERSION"</TD></TR>\n"
	"<TR class='content'><TD>\n"
	"<table border='0'>\n"
	"<TR><TD><font color='Blue'>"+_("Name: ")+"</font></TD><TD>"+_(MOD_NAME)+"</TD></TR>"
	"<TR><TD><font color='Blue'>"+_("Desription: ")+"</font></TD><TD>"+_(DESCRIPTION)+"</TD></TR>"
	"<TR><TD><font color='Blue'>"+_("License: ")+"</font></TD><TD>"+_(LICENSE)+"</TD></TR>"
	"<TR><TD><font color='Blue'>"+_("Author: ")+"</font></TD><TD>"+_(AUTORS)+"</TD></TR>"
	"</table>\n"
	"</TD></TR>\n</table><br/></center>\n";
}

void TWEB::getAuth( SSess &ses )
{
    ses.page = ses.page+"<center><table class='page_auth'>"
	"<tr><td><b>"+_("Enter to module")+"</b></td></tr>\n"
	"<tr class='content'> <td align='center'>\n"
	"<form method='post' action='/"MOD_ID+ses.url+"' enctype='multipart/form-data'>\n"
	"<table cellpadding='3'>\n"
	"<tr><td><b>"+_("User name")+"</b></td><td><input type='text' name='user' size='20'/></td></tr>\n"
	"<tr><td><b>"+_("Password")+"</b></td><td><input type='password' name='pass' size='20'/></td></tr>\n"
	"<tr><td colspan='2' align='center'><input type='submit' name='auth_enter' value='"+_("Enter")+"'/>\n"
	"<input type='reset' name='clean' value='"+_("Clean")+"'/>\n"
	"</td></tr></table>\n</form>\n"
	"</td></tr></table></center>\n";
}

string TWEB::getCookie( string name, vector<string> &vars )
{
    for( unsigned i_var = 0; i_var < vars.size(); i_var++)
	if( vars[i_var].substr(0, vars[i_var].find(":",0)) == "Cookie" )
	{
	    int i_beg = vars[i_var].find(name+"=",0);
	    if( i_beg == string::npos ) return "";
	    i_beg += name.size()+1;
	    return vars[i_var].substr(i_beg,vars[i_var].find(";",i_beg)-i_beg);
	}
    return "";
}

int TWEB::sesOpen( string name )
{
    int sess_id;
    ResAlloc res(m_res,true);

    //- Get free identifier -
    do{ sess_id = rand(); }
    while( sess_id == 0 || m_auth.find(sess_id) != m_auth.end() );

    //- Add new session authentification -
    m_auth[sess_id] = SAuth(name,time(NULL));

    return sess_id;
}

void TWEB::sesCheck( SSess &ses )
{
    ResAlloc res(m_res,false);
    map< int, SAuth >::iterator authEl = m_auth.find(atoi(getCookie( "oscdAuthVisionId", ses.vars ).c_str()));
    if( authEl != m_auth.end() )
    {
	ses.user = authEl->second.name;
	authEl->second.t_auth = time(NULL);
    }
}

void TWEB::HttpPost( const string &url, string &page, const string &sender, vector<string> &vars, const string &contain )
{
    map< string, string >::iterator cntEl;
    SSess ses(TSYS::strDecode(url,TSYS::HttpURL),page,sender,vars,contain);

    //- Check for autentification POST requests -
    if( ses.cnt.find("auth_enter") != ses.cnt.end() )
    {
	string pass;
	if( (cntEl=ses.cnt.find("user")) != ses.cnt.end() )	ses.user = cntEl->second;
	if( (cntEl=ses.cnt.find("pass")) != ses.cnt.end() )	pass = cntEl->second;
	if( SYS->security().at().usrPresent(ses.user) && SYS->security().at().usrAt(ses.user).at().auth(pass) )
	{
	    ses.page = pgHead("<META HTTP-EQUIV='Refresh' CONTENT='0; URL=/"MOD_ID"/"+url+"'/>")+pgTail();
	    page=httpHead("200 OK",ses.page.size(),"text/html",
		"Set-Cookie: oscdAuthVisionId="+TSYS::int2str(sesOpen(ses.user))+"; path=/;\n")+ses.page;
	    return;
	}
	ses.page = pgHead()+"<h1 class='head'>"+PACKAGE_NAME+". "+_(MOD_NAME)+"</h1>\n<hr/><br/>\n";
	messPost(ses.page,nodePath(),_("Auth wrong! Retry please."),TWEB::Error);
	ses.page += "\n";
	getAuth( ses );
	ses.page += pgTail();
	page = httpHead("200 OK",ses.page.size(),"text/html")+ses.page;
	return;
    }
    //- Session check -
    sesCheck( ses );
    if( !ses.user.size() || ses.cnt.find("auth_ch") != ses.cnt.end() )
    {
	ses.page = pgHead("<META HTTP-EQUIV='Refresh' CONTENT='0; URL="MOD_ID"/"+url+"'/>")+pgTail();
	page=httpHead("200 OK",ses.page.size(),"text/html",
	    "Set-Cookie: oscdAuthVisionId=""; path=/;\n")+ses.page;
	return;
    }

    //- Post command to session -
    try
    {
	string sesnm = TSYS::pathLev(ses.url,0);
	if( sesnm.size() <= 4 || sesnm.substr(0,4) != "ses_" ) page = httpHead("404 Not Found");
	else
	{
	    vcaSesAt(sesnm.substr(4)).at().postReq(ses); 
	    page = ses.page;
	}
    }
    catch(...) { page = httpHead("404 Not Found"); }
}

void TWEB::messPost( string &page, const string &cat, const string &mess, MessLev type )
{
    //- Put system message. -
    message(cat.c_str(), (type==Error) ? TMess::Error :
			 (type==Warning) ? TMess::Warning :
			 TMess::Info,"%s",mess.c_str());

    //- Prepare HTML messages -
    page = page+"<table border='2' width='40%' align='center'><tbody>\n";
    if(type == Warning )
	page = page+"<tr bgcolor='yellow'><td align='center'><b>Warning!</b></td></tr>\n";
    else if(type == Error )
	page = page+"<tr bgcolor='red'><td align='center'><b>Error!</b></td></tr>\n";
    else page = page+"<tr bgcolor='#9999ff'><td align='center'><b>Message!</b></td></tr>\n";
    page = page+"<tr bgcolor='#cccccc'> <td align='center'>"+TSYS::strEncode(mess,TSYS::Html)+"</td></tr>\n";
    page = page+"</tbody></table>\n";
}

int TWEB::cntrIfCmd( XMLNode &node, const string &user, bool VCA )
{
    node.setAttr("user",user);
    if( VCA )	node.setAttr("path","/UI/VCAEngine"+node.attr("path"));
    SYS->cntrCmd(&node);
    return atoi(node.attr("rez").c_str());
}

void TWEB::cntrCmdProc( XMLNode *opt )
{
    //- Get page info -
    if( opt->name() == "info" )
    {
	TUI::cntrCmdProc(opt);
	if(ctrMkNode("area",opt,1,"/prm/cfg",_("Module options")))
	{
	    ctrMkNode("fld",opt,-1,"/prm/cfg/lf_tm",_("Life time of auth sesion(min)"),0660,"root","root",1,"tp","dec");
	    ctrMkNode("fld",opt,-1,"/prm/cfg/CSS",_("CSS"),0660,"root","root",3,"tp","str","cols","90","rows","7");
	}
	ctrMkNode("fld",opt,-1,"/help/g_help",_("Options help"),0440,"root","root",3,"tp","str","cols","90","rows","5");
	return;
    }

    //- Process command to page -
    string a_path = opt->attr("path");
    if( a_path == "/prm/cfg/lf_tm" )
    {
	if( ctrChkNode(opt,"get",0660,"root","root",SEQ_RD) )   opt->setText( TSYS::int2str(authTime()) );
	if( ctrChkNode(opt,"set",0660,"root","root",SEQ_WR) )   setAuthTime( atoi(opt->text().c_str()) );
    }
    else if( a_path == "/prm/cfg/CSS" )
    {
	if( ctrChkNode(opt,"get",0660,"root","root",SEQ_RD) )   opt->setText( CSStables() );
	if( ctrChkNode(opt,"set",0660,"root","root",SEQ_WR) )   setCSStables( opt->text() );
    }
    else if( a_path == "/help/g_help" && ctrChkNode(opt,"get",0440) )   opt->setText(optDescr());
    else TUI::cntrCmdProc(opt);
}

int TWEB::colorParse( const string &clr )
{
    if( clr.size() >= 4 && clr[0] == '#' )
    {
	int el_sz = clr.size()/3;
	return (strtol(clr.substr(1,el_sz).c_str(),NULL,16)<<16)+
		(strtol(clr.substr(1+el_sz,el_sz).c_str(),NULL,16)<<8)+
		strtol(clr.substr(1+2*el_sz,el_sz).c_str(),NULL,16);
    }
    else
    {
	map<string,int>::iterator iclr = colors.find(clr);
	if( iclr != colors.end() )	return iclr->second;
    }
    return -1;
}

//*************************************************
//* SSess                                         *
//*************************************************
SSess::SSess( const string &iurl, const string &ipage, const string &isender,
	vector<string> &ivars, const string &icontent ) :
    url(iurl), page(ipage), sender(isender), vars(ivars), content(icontent)
{
    //- URL parameters parse -
    int prmSep = iurl.find("?");
    if( prmSep != string::npos )
    {
	url = iurl.substr(0,prmSep);
	string prms = iurl.substr(prmSep+1);
	string sprm;
	for( int iprm = 0; (sprm=TSYS::strSepParse(prms,0,'&',&iprm)).size(); )
	{
	    prmSep = sprm.find("=");
	    if( prmSep == string::npos ) prm[sprm] = "true";
	    else prm[sprm.substr(0,prmSep)] = sprm.substr(prmSep+1);
	}
    }

    //- Content parse -
    int pos = 0, i_bnd;
    string boundary;
    char *c_bound = "boundary=";
    char *c_term = "\r\n";
    char *c_end = "--";
    char *c_fd = "Content-Disposition: form-data;";
    char *c_name = "name=\"";
    char *c_file = "filename=\"";

    for( int i_vr = 0; i_vr < vars.size(); i_vr++ )
	if( vars[i_vr].substr(0,vars[i_vr].find(":",0)) == "Content-Type" )
	{
	    int pos = vars[i_vr].find(c_bound,0)+strlen(c_bound);
	    boundary = vars[i_vr].substr(pos,vars[i_vr].size()-pos);
	}
    if( !boundary.size() ) return;

    while(true)
    {
	pos = content.find(boundary,pos);
	if( pos == string::npos || content.substr(pos+boundary.size(),2) == "--" ) break;
	pos += boundary.size()+strlen(c_term);
	string c_head = content.substr(pos, content.find(c_term,pos)-pos);
	if( c_head.find(c_fd,0) == string::npos ) continue;

	//-- Get name --
	i_bnd = c_head.find(c_name,0)+strlen(c_name);
	string c_name = c_head.substr(i_bnd,c_head.find("\"",i_bnd)-i_bnd);
	i_bnd = c_head.find(c_file,0);
	if( i_bnd == string::npos )
	{
	    //--- Get value ---
	    pos += c_head.size()+(2*strlen(c_term));
	    if(pos >= content.size()) break;
	    string c_val  = content.substr(pos, content.find(string(c_term)+c_end+boundary,pos)-pos);
	    cnt[c_name] = c_val;
	}
	else
	{
	    i_bnd += strlen(c_file);
	    cnt[c_name] = c_head.substr(i_bnd,c_head.find("\"",i_bnd)-i_bnd);
	}
    }
}
