
//OpenSCADA system module UI.WebVision file: VCA.js
/***************************************************************************
 *   Copyright (C) 2007-2021 by Roman Savochenko, <roman@oscada.org>       *
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

var MOD_ID = 'WebVision';
var isNN = navigator.appName.indexOf('Netscape') != -1;
var isIE = navigator.appName.indexOf('Microsoft') != -1;
var isOpera = navigator.appName.indexOf('Opera') != -1;
var isKonq = navigator.userAgent.indexOf('Konqueror') != -1;
var mainTmId = 0;
var gPrms = window.location.search || '';
var tmClearEdit = 10;			//Clear time of line edit fields, seconds
var tmFullUpd = 5;			//Full tree updating time,seconds
var tmAlrmUpd = 0.5;			//Alarms updating time, seconds
var limTblItmCnt = 300;			//Limit of the table item content

/***************************************************
 * pathLev - Path parsing function.                *
 ***************************************************/
pathLev.off = 0;
function pathLev( path, level, scan )
{
    var an_dir = scan ? pathLev.off : 0;
    var t_lev = 0;
    var t_dir;
    while(an_dir < path.length && path.charAt(an_dir) == '/') an_dir++;
    if(an_dir >= path.length) return '';
    while(true) {
	t_dir = path.indexOf('/', an_dir);
	if(t_dir < 0)		{ pathLev.off = path.length; return (t_lev==level)?path.substr(an_dir):''; }
	if(t_lev == level)	{ pathLev.off = t_dir; return path.substr(an_dir,t_dir-an_dir); }
	an_dir = t_dir;
	t_lev++;
	while(an_dir < path.length && path.charAt(an_dir) == '/') an_dir++;
    }
}

/***************************************************
 * noSpace - Get no space (trimmed) string         *
 ***************************************************/
function noSpace( str )
{
    var posSt = -1; var posEnd = 0;
    for(var i = 0; i < str.length; i++)
	if(str[i] != ' ') { if(posSt<0) posSt = i; posEnd = i; }
    return (posSt>=0) ? str.substr(posSt,posEnd-posSt+1) : '';
}

/***************************************************
 * i2s - integer to string                         *
 ***************************************************/
function i2s( vl, base, len )
{
    rez = vl.toString(base);
    if(len) while(rez.length < len) rez = "0"+rez;
    return rez;
}

/***************************************************
 * posGetX - Get absolute position                 *
 **************************************************/
function posGetX( obj, noWScrl )
{
    var posX = 0;
    for( ; obj && obj.nodeName != 'BODY'; obj = obj.parentNode) {
	posX += (obj.style.left?parseInt(obj.style.left):0)+
	    (obj.parentNode.style.borderLeftWidth?parseInt(obj.parentNode.style.borderLeftWidth):0)+
	    (obj.parentNode.style.marginLeft?parseInt(obj.parentNode.style.marginLeft):0);
	if(obj.style.position == 'relative') posX += obj.offsetLeft;
    }
    return posX + (!noWScrl?-window.pageXOffset:0);
}

/***************************************************
 * posGetY - Get absolute position                 *
 **************************************************/
function posGetY( obj, noWScrl )
{
    var posY = 0;
    for( ; obj && obj.nodeName != 'BODY'; obj = obj.parentNode) {
	posY += (obj.style.top?parseInt(obj.style.top):0)+
	    (obj.parentNode.style.borderTopWidth?parseInt(obj.parentNode.style.borderTopWidth):0)+
	    (obj.parentNode.style.marginTop?parseInt(obj.parentNode.style.marginTop):0);
	if(obj.style.position == 'relative') posY += obj.offsetTop;
	posY -= obj.scrollTop;
    }
    return posY + (!noWScrl?-window.pageYOffset:0);
}

/***************************************************
 * getXmlHttp - Check and return XMLHttpRequest for*
 *  various browsers.                              *
 ***************************************************/
function getXmlHttp( )
{
    reqO = null;
    if(window.XMLHttpRequest) reqO = new XMLHttpRequest();
    else if(window.ActiveXObject) {
	try { reqO = new ActiveXObject('Msxml2.XMLHTTP'); }
	catch(e) { reqO = new ActiveXObject('Microsoft.XMLHTTP'); }
    }

    for(iO = 0; iO < reqObjects.length; iO++) {
	if(reqObjects[iO] && reqObjects[iO].readyState == 4) { delete reqObjects[iO]; reqObjects[iO] = null; }
	if(!reqObjects[iO]) { reqObjects.splice(iO, 1); iO--; }
    }

    reqObjects.push(reqO);

    return reqO;
}

/***************************************************
 * realRound - Real numbers round                  *
 ***************************************************/
function realRound( val, dig, toInt )
{
    var rrRez = Math.floor(val*Math.pow(10,dig?dig:0)+0.5)/Math.pow(10,dig?dig:0);
    if(toInt) return Math.floor(rrRez+0.5);
    return rrRez;
}

/***************************************************
 * evMouseGet - Get mouse key code from event      *
 ***************************************************/
function evMouseGet( e )
{
    if(e.which == 1)		return 'Left';
    else if(e.which == 2)	return 'Midle';
    else if(e.which == 3)	return 'Right';
    return '';
}

/***************************************************
 * evKeyGet - Get key code from event              *
 ***************************************************/
function evKeyGet( e )
{
    if((e.keyCode >= 48 && e.keyCode <= 57) || (e.keyCode >= 65 && e.keyCode <= 90))
	return String.fromCharCode(e.keyCode);
    switch(e.keyCode) {
	case 27: return 'Esc';
	case 8:  return 'BackSpace';
	case 13: return 'Enter';
	case 45: return 'Insert';
	case 19: return 'Pause';
	case 36: return 'Home';
	case 35: return 'End';
	case 37: return 'Left';
	case 38: return 'Up';
	case 39: return 'Right';
	case 40: return 'Down';
	case 33: return 'PageUp';
	case 34: return 'PageDown';
	case 112: return 'F1';
	case 113: return 'F2';
	case 114: return 'F3';
	case 115: return 'F4';
	case 116: return 'F5';
	case 117: return 'F6';
	case 118: return 'F7';
	case 119: return 'F8';
	case 120: return 'F9';
	case 121: return 'F10';
	case 122: return 'F11';
	case 123: return 'F12';
	case 32: return 'Space';
	case 59: return 'Semicolon';
	case 61: return 'Equal';
    }
    if(isNN) switch(e.keyCode) {
	case 46: return 'Delete';
	case 44: return 'Print';
	case 192: return 'Apostrophe';
	case 106: return 'Asterisk';
	case 107: return 'Plus';
	case 188: return 'Comma';
	case 109: return 'Minus';
	case 190: return 'Period';
	case 220: return 'Slash';
	case 219: return 'BracketLeft';
	case 191: return 'BackSlash';
	case 221: return 'BracketRight';
	case 222: return 'QuoteLeft';
    }
    else switch(e.keyCode) {
	case 127: return 'Delete';
	case 96: return 'Apostrophe';
	case 42: return 'Asterisk';
	case 43: return 'Plus';
	case 44: return 'Comma';
	case 45: return 'Minus';
	case 46: return 'Period';
	case 92: return 'Slash';
	case 91: return 'BracketLeft';
	case 47: return 'BackSlash';
	case 93: return 'BracketRight';
	case 39: return 'QuoteLeft';
    }
    return '#'+e.keyCode.toString(16);
}

/***************************************************
 * servGet - XML get request to server             *
 ***************************************************/
function servGet( adr, prm, callBack, callBackPrm )
{
    var req = getXmlHttp();
    req.open('GET', encodeURI('/'+MOD_ID+adr+(gPrms.length?gPrms+'&':'?')+prm), callBack ? true : false);
    if(callBack) {
	req.callBack = callBack;
	req.callBackPrm = callBackPrm;
	req.responseXML = null;
	req.onreadystatechange = function( ) {
	    if(this.readyState != 4) return;
	    if(this.status != 200) window.location.reload();
	    else if(this.responseXML && this.responseXML.childNodes.length) {
		this.responseXML.childNodes[0].callBackPrm = this.callBackPrm;
		this.callBack(this.responseXML.childNodes[0]);
	    }
	    else this.callBack(-1);
	};
	req.send(null);
	return null;
    }
    req.send(null);
    if(req.status != 200) window.location.reload();
    else if(req.responseXML.childNodes.length)	return req.responseXML.childNodes[0];

    return null;
}

/***************************************************
 * servSet - XML set request to server             *
 ***************************************************/
function servSet( adr, prm, body, noWaitRez )
{
    if(noWaitRez == null) noWaitRez = false;	//!!!! Otherwise we can get of processing old events after new ones
    var req = getXmlHttp();
    req.open('POST', encodeURI('/'+MOD_ID+adr+(gPrms.length?gPrms+'&':'?')+prm), noWaitRez);
    try {
	req.responseXML = null;
	req.send(body);
	if(!noWaitRez && req.status == 200 && req.responseXML.childNodes.length)
	    return req.responseXML.childNodes[0];
	else if(req.status == 404) window.location.reload();
	//if(mainTmId) clearTimeout(mainTmId);
	//mainTmId = setTimeout(makeUI, 1000);
    } catch(e) { window.location.reload(); /*window.location = '/'+MOD_ID;*/ }

    return null;
}

/***************************************************
 * getWAttr - request page/widget attribute        *
 ***************************************************/
function getWAttr( wId, attr )
{
    var rNode = servGet(wId, 'com=attr&attr='+attr);
    if(!rNode) return null;

    return rNode.textContent;
}

/***************************************************
 * setWAttrs - set page/widget attributes          *
 ***************************************************/
function setWAttrs( wId, attrs, val )
{
    var body = '<set>';
    if(typeof(attrs) == 'string')
	body += '<el id=\''+attrs+'\'>'+((typeof(val)=='string')?val.replace(new RegExp('&','g'),'&amp;').replace(new RegExp('<','g'),'&lt;').replace(new RegExp('>','g'),'&gt;'):val)+'</el>';
    else for(var i in attrs)
	body += '<el id=\''+i+'\'>'+((typeof(attrs[i])=='string')?attrs[i].replace(new RegExp('&','g'),'&amp;').replace(new RegExp('<','g'),'&lt;').replace(new RegExp('>','g'),'&gt;'):attrs[i])+'</el>';
    body += '</set>';

    servSet(wId, 'com=attrs', body);
}

/***************************************************
 * getFont( fStr, fSc, opt ) - Parse font          *
 *  opt - options: 1-punkts; 2-return size         *
 ***************************************************/
function getFont( fStr, fSc, opt )
{
    var rez = '';
    if(fStr) {
	var allFnt = fStr.split(' ');
	if(opt == 2) return (parseInt(allFnt[1])*(fSc?fSc:1)).toFixed(0);
	if(allFnt.length >= 1) rez += 'font-family: ' + allFnt[0].replace(/_/g,' ') + '; ';
	if(allFnt.length >= 2) rez += 'font-size: ' + (parseInt(allFnt[1])*(fSc?fSc:1)).toFixed(0) + (opt==1?'pt; ':'px; ');
	if(allFnt.length >= 3) rez += 'font-weight: ' + (parseInt(allFnt[2])?'bold':'normal') + '; ';
	if(allFnt.length >= 4) rez += 'font-style: ' + (parseInt(allFnt[3])?'italic':'normal') + '; ';
	if(allFnt.length >= 5 && parseInt(allFnt[4])) rez += 'text-decoration: underline; ';
	else if(allFnt.length >= 6 && parseInt(allFnt[5])) rez += 'text-decoration: line-through; ';
    }

    return rez;
}

/***************************************************
 * getFontCond( fStr, fSc, opt ) - Parse font          *
 *  opt - options: 1-punkts; 2-return size         *
 ***************************************************/
function getFontCond( fStr, fSc, opt )
{
    fontCfg = "";
    if(fStr) {
	allFnt = fStr.split(" ");
	if(allFnt.length >= 3 && parseInt(allFnt[2])) fontCfg += "bold ";
	if(allFnt.length >= 4 && parseInt(allFnt[3])) fontCfg += "italic ";
	if(allFnt.length >= 2) fontCfg += (parseInt(allFnt[1])*(fSc?fSc:1)).toFixed(0) + (opt==1?"pt ":"px ");
	if(allFnt.length >= 1) fontCfg += allFnt[0].replace(/_/g," ") + " ";
    }

    return fontCfg;
}

/***************************************************
 * getColor - Parse color                          *
 ***************************************************/
function getColor( cStr, getOpacity )
{
    var fPos = cStr.indexOf('-');
    if(getOpacity) return (fPos >= 0) ? Math.max(0,Math.min(255,parseFloat(cStr.slice(fPos+1))/255)) : 1;
    return (fPos >= 0) ? cStr.slice(0,fPos) : cStr;
}

/*****************************************************
 * chkPattern - Check for allow expression to pattern *
 *****************************************************/
function chkPattern( val, patt )
{
    //Check by regular expression
    if(patt.length > 2 && patt.charAt(0) == '/' && patt.charAt(patt.length-1) == '/')
	return (new RegExp(patt.slice(1,patt.length-1),'')).test(val);

    //Check by simple pattern
    var mult_s = false;
    var v_cnt = 0, p_cnt = 0;
    var v_bck = -1, p_bck = -1;

    while(true) {
	if(p_cnt >= patt.length)	return true;
	if(patt.charAt(p_cnt) == '?')	{ v_cnt++; p_cnt++; mult_s = false; continue; }
	if(patt.charAt(p_cnt) == '*')	{ p_cnt++; mult_s = true; v_bck = -1; continue; }
	if(patt.charAt(p_cnt) == '\\')	p_cnt++;
	if(v_cnt >= val.length)	break;
	if(patt.charAt(p_cnt) == val.charAt(v_cnt)) {
	    if(mult_s && v_bck < 0)	{ v_bck = v_cnt+1; p_bck = p_cnt; }
	    v_cnt++; p_cnt++;
	}
	else {
	    if(mult_s) {
		if(v_bck >= 0) { v_cnt = v_bck; p_cnt = p_bck; v_bck = -1; }
		else v_cnt++;
	    }
	    else break;
	}
    }
    return false;
}

/***************************************************
 * getTabIndex - Calculate the absolute tab index  *
 ***************************************************/
function getTabIndex( wdgO, origPos )
{
    for(pow = 1, curPar = wdgO.parent; curPar; curPar = curPar.parent)
	if(curPar.wPos >= 0) { origPos += curPar.wPos*Math.pow(100,pow); pow++; }

    return origPos;
}

/***************************************************
 * setFocus - Command for set focus                *
 ***************************************************/
function setFocus( wdg, onlyClr )
{
    if(masterPage.focusWdf && masterPage.focusWdf == wdg) return;

    var attrs = new Object();
    if(masterPage.focusWdf) { attrs.focus = '0'; attrs.event = 'ws_FocusOut'; setWAttrs(masterPage.focusWdf,attrs); }
    masterPage.focusWdf = wdg;
    if(!onlyClr) { attrs.focus = '1'; attrs.event = 'ws_FocusIn'; setWAttrs(masterPage.focusWdf,attrs); }
}

/****************************************************************************
 * PageObj pgCacheGet( addr ) - Getting for the page <addr> from the cache. *
 *   addr - address of the page.                                            *
 ***************************************************************************/
function pgCacheGet( addr )
{
    for(iPg = 0; iPg < pgCache.length; iPg++)
	if(pgCache[iPg].addr == addr)
	    return pgCache.splice(iPg,1)[0];
    return null;
}

/*********************************************************************
 * pgCacheProc( page ) - Processing the cache for the <page> or      *
 *     the lifetime fo null <page>                                   *
 *   addr - address of the page.                                     *
 *********************************************************************/
function pgCacheProc( page )
{
    if(page) { pgCache.unshift(page); page.cacheTm = (new Date()).getTime()/1000; }
    //Removing the limited pages
    for(iPg = pgCache.length-1; iPg >= 0; iPg--)
	if((cachePgLife > 0.01 && ((((new Date()).getTime()/1000)-pgCache[iPg].cacheTm) > cachePgLife*60*60)) ||
		(cachePgSz && pgCache.length > cachePgSz))
	    delete pgCache.splice(iPg,1)[0];
	else break;
}

/**********************************************************
 * callPage - call page 'pgId' for open update and other. *
 **********************************************************/
function callPage( pgId, updWdg, pgGrp, pgOpenSrc )
{
    if(!pgId) return true;

    //Checking and updating the presented pages
    if(this == masterPage)
	if((opPg=this.findOpenPage(pgId))) {
	    opPg.fullUpdCnt = opPg.fullUpdCnt ? Math.max(0,opPg.fullUpdCnt-planePer) : tmFullUpd;
	    if(updWdg || !opPg.fullUpdCnt)
		servGet(pgId, 'com=attrsBr&FullTree='+(opPg.fullUpdCnt?0:1)+'&tm='+tmCnt, makeEl, opPg);
	    return true;
	}

    //Checking pages before opening
    if(!pgGrp || !pgOpenSrc) {
	reqPgGen = "<CntrReqs>"+
	    "<get path='/%2fattr%2fpgGrp'/>"+
	    "<get path='/%2fattr%2fpgOpenSrc'/>";
	// For per-page notification
	for(iNtf = 0; iNtf < 8; iNtf++)
	    reqPgGen += "<activate path='/%2fserv%2fattr%2fnotify"+iNtf+"'/>"+
			"<activate path='/%2fserv%2fattr%2fnotifyVis"+MOD_ID+iNtf+"'/>";
	reqPgGen += "</CntrReqs>";
	reqPgGen = servSet("/UI/VCAEngine"+pgId, 'com=com', reqPgGen, false);
	pgGrp = reqPgGen.childNodes[0].textContent;
	pgOpenSrc = reqPgGen.childNodes[1].textContent;
    }

    //Creating/replacing the main page
    if(this == masterPage && (!this.addr.length || pgGrp == 'main' || pgGrp == this.attrs['pgGrp'])) {
	if(this.addr.length) {
	    servSet(this.addr, 'com=pgClose&cacheCntr', '');
	    this.pwClean(true);
	    document.body.removeChild(this.window);
	}
	this.addr  = pgId;
	this.place = document.createElement('div');
	this.place.setAttribute('id', 'mainCntr');

	//Get and activate for specific attributes to the master-page
	servSet("/UI/VCAEngine"+this.addr, 'com=com', "<CntrReqs>"+
	    "<activate path='/%2fserv%2fattr%2fkeepAspectRatio' aNm='Keep aspect ratio on scale' aTp='0'/>"+
	    "<activate path='/%2fserv%2fattr%2fstBarNoShow' aNm='Do not show the status bar' aTp='0'/>"+
	    "<activate path='/%2fserv%2fattr%2fuserSetVis'/>"+
	    "</CntrReqs>");

	this.makeEl(servGet(pgId,'com=attrsBr'), false, true);
	var centerTag = document.createElement('center');
	centerTag.appendChild(this.place);
	document.body.appendChild(centerTag);
	this.window = centerTag;
	//Set project's icon and RunTime page title
	document.getElementsByTagName('link')[0].setAttribute('href',location.pathname+'?com=ico');
	return true;
    }

    //Find for included pages creation
    for(var i in this.wdgs)
	if(this.wdgs[i].attrs['root'] == 'Box' && this.wdgs[i].isVisible) {
	    if(pgGrp == this.wdgs[i].attrs['pgGrp']) {
		if(pgId != this.wdgs[i].attrs['pgOpenSrc']) {
		    //this.wdgs[i].attrs['pgOpenSrc'] = pgId;
		    //this.wdgs[i].makeEl(null, true);
		    // Changing the pages only through 'pgOpenSrc' but the source page direct update may cause collisions on fast switching
		    setWAttrs(this.wdgs[i].addr, 'pgOpenSrc', pgId);
		}
		return true;
	    }
	    if(this.wdgs[i].inclOpen && this.wdgs[i].pages[this.wdgs[i].inclOpen].callPage(pgId,updWdg,pgGrp,pgOpenSrc)) return true;
	}
    //Put checking to child pages
    for(var i in this.pages)
	if(this.pages[i].callPage(pgId,updWdg,pgGrp,pgOpenSrc)) return true;
    //Check for open child page or for unknown and empty source pages open as master page child windows
    if((!pgGrp.length && pgOpenSrc == this.addr) || this == masterPage) {
	var iPg = new pwDescr(pgId, true, this);
	var attrBrVal = servGet(pgId, 'com=attrsBr');

	var winName = null;
	var winWidth = 600;
	var winHeight = 400;
	for(var iCh = 0; iCh < attrBrVal.childNodes.length; iCh++)
	    if(attrBrVal.childNodes[iCh].nodeName != 'el') continue;
	    else if(attrBrVal.childNodes[iCh].getAttribute('id') == 'name') winName = attrBrVal.childNodes[iCh].textContent;
	    else if(attrBrVal.childNodes[iCh].getAttribute('id') == 'geomW') winWidth = parseInt(attrBrVal.childNodes[iCh].textContent);
	    else if(attrBrVal.childNodes[iCh].getAttribute('id') == 'geomH') winHeight = parseInt(attrBrVal.childNodes[iCh].textContent);

	//New external <div> window create
	if(winWidth < parseInt(masterPage.attrs['geomW']) && winHeight < parseInt(masterPage.attrs['geomH'])) {
	    iPg.window = document.createElement('table');
	    iPg.window.iPg = iPg;
	    iPg.window.id = "gen-pnl-dlg";
	    iPg.window.setAttribute('cellspacing', '0');
	    iPg.window.setAttribute('style','left: '+((parseInt(masterPage.attrs['geomW'])-winWidth-5)/2)+'px; '+
					    'top: '+((parseInt(masterPage.attrs['geomH'])-winHeight-18)/2)+'px; ');
	    iPg.window.innerHTML = "<tr class='hd' onmousedown='this.clX = event.clientX; this.clY = event.clientY; return false;' "+
		"onmouseup='this.clX = this.clY = null; return false;' onmouseout='this.onmouseup()' "+
		"onmousemove='if(this.clX != null) { "+
		" this.offsetParent.style.left = (parseInt(this.offsetParent.style.left)+event.clientX-this.clX)+\"px\"; "+
		" this.offsetParent.style.top  = (parseInt(this.offsetParent.style.top)+event.clientY-this.clY)+\"px\"; "+
		" this.clX = event.clientX; this.clY = event.clientY; }'>"+
		" <td title='"+winName+"' style='padding-left: 5px; overflow: hidden; white-space: nowrap;'>"+winName+"</td>"+
		" <td style='color: red; cursor: pointer; text-align: right; width: 1px;' "+
		"  onclick='servSet(this.offsetParent.iPg.addr,\"com=pgClose&cacheCntr\",\"\"); "+
		"   /*this.offsetParent.iPg.pwClean(); "+	//!!!! Commented to prevent the flicking
		"   delete this.offsetParent.iPg.parent.pages[this.offsetParent.iPg.addr]; "+
		"   document.getElementById(\"mainCntr\").removeChild(this.offsetParent.iPg.window);*/'>X</td></tr>\n"+
		"<tr><td colspan='2'><div/></td></tr>";
	    document.getElementById('mainCntr').appendChild(iPg.window);
	    iPg.place = iPg.window.rows[1].cells[0].children[0];
	}
	//New external window create
	else {
	    iPg.window = window.open('about:blank',pgId,'width='+(winWidth+20)+',height='+(winHeight+60)+
		',directories=no,menubar=no,toolbar=no,scrollbars=yes,dependent=yes,location=no,locationbar=no,status=no,statusbar=no,alwaysRaised=yes');
	    if(!iPg.window) return true;
	    iPg.window.document.open();
	    iPg.window.document.write("<html><body style='background-color: #E6E6E6;'><center><div id='main'/></center></body></html>\n");
	    iPg.window.document.close();
	    iPg.window.document.title = winName;
	    var mainDiv = iPg.window.document.getElementById('main');
	    iPg.place = mainDiv;
	    iPg.windowExt = true;
	}

	this.pages[iPg.addr] = iPg;
	iPg.makeEl(attrBrVal, false, true);

	return true;
    }

  return false;
}

function findOpenPage( pgId )
{
    var opPg;
    if(pgId == this.addr) return this;
    //Check from included widgets
    for(var i in this.wdgs)
	if(this.wdgs[i].attrs['root'] == 'Box' && this.wdgs[i].isVisible) {
	    if(pgId == this.wdgs[i].attrs['pgOpenSrc']) return this.wdgs[i].pages[pgId];
	    if(this.wdgs[i].inclOpen) {
		opPg = this.wdgs[i].pages[this.wdgs[i].inclOpen].findOpenPage(pgId);
		if(opPg) return opPg;
	    }
	}
    //Put checking to child pages
    for(var i in this.pages) {
	opPg = this.pages[i].findOpenPage(pgId);
	if(opPg) return opPg;
    }

    return null;
}

function makeEl( pgBr, inclPg, full, FullTree )
{
    //Callback processing
    if(pgBr) {
	if(pgBr == -1) { window.location.reload(true); return; }	//Try to restore the connection
	if(pgBr.getAttribute('FullTree')) FullTree = parseInt(pgBr.getAttribute('FullTree'));
	if(pgBr.callBackPrm) {
	    elO = pgBr.callBackPrm; pgBr.callBackPrm = null;
	    elO.makeEl(pgBr, inclPg, full, FullTree);
	    return;
	}
    }

    //Main processing
    var margBrdUpd = false; var newAttr = false;
    this.place.wdgLnk = this;
    if(!inclPg && pgBr) {
	//Clear modify flag for all
	for(var aId in this.attrsMdf) this.attrsMdf[aId] = false;
	//Updated attributes
	for(var iCh = 0; iCh < pgBr.childNodes.length; iCh++) {
	    if(pgBr.childNodes[iCh].nodeName != 'el') continue;
	    var aId = pgBr.childNodes[iCh].getAttribute('id');
	    if((aId == 'bordWidth' || aId == 'geomMargin') && (!this.attrs[aId] || this.attrs[aId] != pgBr.childNodes[iCh].textContent)) margBrdUpd = true;
	    this.attrsMdf[aId] = !this.attrs[aId] || this.attrs[aId] != pgBr.childNodes[iCh].textContent;
	    if(this.attrsMdf[aId]) { this.attrs[aId] = pgBr.childNodes[iCh].textContent; newAttr = true; }
	}
    }

    if(newAttr || inclPg || !pgBr) {
	var elWr = (parseInt(this.attrs['active']) && parseInt(this.attrs['perm'])&SEC_WR);
	var elMargin = parseInt(this.attrs['geomMargin']);
	var elBorder = 0;
	if(this.attrs['bordWidth']) elBorder = parseInt(this.attrs['bordWidth']);
	var elStyle = '';
	this.isVisible = true;
	if(!(parseInt(this.attrs['en']) && (this.pg || parseInt(this.attrs['perm'])&SEC_RD)))
	{ elStyle += 'visibility : hidden; '; this.isVisible = false; }
	var geomX = parseFloat(this.attrs['geomX']);
	var geomY = parseFloat(this.attrs['geomY']);
	if(this.pg) { geomX = geomY = 0; elStyle += 'overflow: hidden; '; }
	else { geomX *= this.parent.xScale(true); geomY *= this.parent.yScale(true); }
	if(this.parent && !(this.pg && this.parent.pg)) {
	    geomX -= parseInt(this.parent.attrs['geomMargin'])+parseInt(this.parent.attrs['bordWidth']);
	    geomY -= parseInt(this.parent.attrs['geomMargin'])+parseInt(this.parent.attrs['bordWidth']);
	}
	elStyle += 'position: '+((this==masterPage || this.window)?'relative':'absolute')+'; left: '+realRound(geomX)+'px; top: '+realRound(geomY)+'px; ';

	// Calculation of the main window/page scale
	if(this == masterPage) {
	    sesUser = pgBr.getAttribute('user');

	    //  Own status bar reserve
	    var toCrtStBar = false;
	    if(!parseInt(this.attrs["stBarNoShow"]) && !masterPage.status) {
		masterPage.status = document.createElement('div');
		masterPage.status.srcHeight = 25;
		masterPage.status.setAttribute('id', 'gen-pnl-status');
		this.place.appendChild(masterPage.status);
		toCrtStBar = true;
	    }

	    var geomW = parseFloat(this.attrs['geomW']);
	    var geomH = parseFloat(this.attrs['geomH']);
	    if(window.devicePixelRatio && window.devicePixelRatio != 1)	wx_scale = wy_scale = 1;
	    else {
		wx_scale = Math.max(1, window.innerWidth/geomW);
		wy_scale = Math.max(1, window.innerHeight/(geomH+(masterPage.status?masterPage.status.srcHeight:0)));
	    }
	    if(parseInt(this.attrs['keepAspectRatio']))
		wx_scale = wy_scale = Math.min(wx_scale, wy_scale);

	    if(masterPage.status) {
		masterPage.status.height = Math.floor(masterPage.status.srcHeight*wy_scale);
		masterPage.status.style.top = Math.floor(geomH*wy_scale)+"px";
		masterPage.status.style.width = (Math.floor(geomW*wx_scale)-5)+"px";
		masterPage.status.style.height = (masterPage.status.height-1)+"px";
		masterPage.status.style.fontSize = Math.floor(masterPage.status.height*0.7)+"px";

		if(toCrtStBar) {
		    stBar = "<table width='100%'><TR><td id='StatusBar' width='100%'/>";
		    stBar += "<td id='st_alarm' title1='###Alarm level: %1###' style='display: flex;'>"+
			    "<img id='alarmLev' onclick='alarmQuiet(this.id)' height='"+(masterPage.status.height-2)+"px' src='/"+MOD_ID+"/img_alarmLev'/></td>";
		    if(modelStyles && parseInt(modelStyles.getAttribute('curStlId')) >= 0 && modelStyles.childNodes.length > 1) {
			stBar += "<td id='st_style' title='###Field for displaying and changing the used interface style.###'>";
			stBar += "<select onchange='styleSet(this.selectedOptions[0].getAttribute(\"itId\"))'>";
			for(iSt = 0; iSt < modelStyles.childNodes.length; iSt++) {
			    if(parseInt(modelStyles.childNodes[iSt].getAttribute("id")) < 0) continue;
			    stBar += "<option itId='"+modelStyles.childNodes[iSt].getAttribute("id")+"' "+
				((modelStyles.childNodes[iSt].getAttribute("id")==modelStyles.getAttribute('curStlId'))?"selected='1'":"")+">"+
				    modelStyles.childNodes[iSt].textContent+"</option>";
			}
			stBar += "</select></td>";
		    }
		    stBar += "<td id='st_user' title='###Field for displaying and changing the current user.###'><a href='/login"+window.location.pathname+"'>"+sesUser+"</a></td>";
		    stBar += "</TR></table>";
		    masterPage.status.innerHTML = stBar;
		}

		elStyle += "overflow: visible; ";
	    }
	}

	var xSc = this.xScale(true);
	var ySc = this.yScale(true);
	var geomW = parseFloat(this.attrs['geomW'])*xSc;
	var geomH = parseFloat(this.attrs['geomH'])*ySc;

	this.mousedown = new Array;
	this.mouseup = new Array;

	if(this.pg && this.parent && this.parent.inclOpen && this.parent.inclOpen == this.addr) {
	    var geomWpar = parseFloat(this.parent.attrs['geomW'])*this.parent.xScale(true);
	    var geomHpar = parseFloat(this.parent.attrs['geomH'])*this.parent.yScale(true);
	    this.parent.place.style.overflow = (geomW > geomWpar || geomH > geomHpar) ? 'auto' : 'hidden';
	    geomWScrl = (geomH > geomHpar) ? 20 : 0; geomHScrl = (geomW > geomWpar) ? 20 : 0;	//!!!! 20 is most scrollbar width and clientWidth does not work here
	    geomW = Math.max(geomW, geomWpar-geomWScrl);
	    geomH = Math.max(geomH, geomHpar-geomHScrl);
	}
	//else elStyle += 'overflow: hidden; ';

	geomW = realRound(geomX+geomW-xSc) - realRound(geomX) + 1;
	geomH = realRound(geomY+geomH-ySc) - realRound(geomY) + 1;

	geomW -= 2*(elMargin+elBorder);
	geomH -= 2*(elMargin+elBorder);

	//Set included window geometry to widget size
	//if( this == masterPage ) resizeTo(geomW,geomH);
	if(this.pg && this.window && this.windowExt) {
	    if(this.window.innerHeight)
		this.window.resizeTo(geomW+(this.window.outerWidth-this.window.innerWidth)+20,
				     geomH+(this.window.outerHeight-this.window.innerHeight)+20);
	    else this.window.resizeTo(geomW+20,geomH+40);
	}

	if(this.attrs['focus'] && parseInt(this.attrs['focus'])) setFocus(this.addr, true);

	this.place.className = "Primitive " + this.attrs['root'];
	if(!this.place.getAttribute("id"))
	    this.place.setAttribute("id", this.addr.slice(this.addr.lastIndexOf("/")+1));	// this.addr.replace("/"+sessId,"").replace(/\//g, "_"));

	var isPrim = true;
	if(!(parseInt(this.attrs['perm'])&SEC_RD)) {
	    if(this.pg) {
		elStyle += 'background-color: #B0B0B0; border: 1px solid black; color: red; overflow: auto; ';
		this.place.innerHTML = "<div class='vertalign' style='width: "+(geomW-2)+"px; height: "+(geomH-2)+"px;'>###Page###: '"+this.addr+"'.<br/>###View access is not permitted.###</div>";
	    }
	    isPrim = false;
	}
	else if(this.attrs['root'] == 'ElFigure') {
	    var toInit = !this.place.childNodes.length;
	    var figObj = toInit ? this.place.ownerDocument.createElement('img') : this.place.childNodes[0];
	    figObj.width = geomW; figObj.height = geomH;
	    // Src update always therefore for any changes is shape update
	    figObj.src = '/'+MOD_ID+this.addr+'?com=obj&tm='+tmCnt+'&geomX='+geomX.toFixed(3)+'&geomY='+geomY.toFixed(3)+
						    '&geomW='+geomW.toFixed(3)+'&geomH='+geomH.toFixed(3)+
						    '&xSc='+xSc.toFixed(3)+'&ySc='+ySc.toFixed(3);
	    if(elWr != this.place.elWr) {
		figObj.onclick = !elWr ? null : function(e) {
		    if(!e) e = window.event;
		    servSet(this.wdgLnk.addr,'com=obj&sub=point&geomX='+geomX.toFixed(3)+'&geomY='+geomY.toFixed(3)+
						    '&xSc='+xSc.toFixed(3)+'&ySc='+ySc.toFixed(3)+
						    '&x='+(e.offsetX?e.offsetX:(e.clientX-posGetX(this)))+
						    '&y='+(e.offsetY?e.offsetY:(e.clientY-posGetY(this)))+
						    '&key='+evMouseGet(e),'');
		    return false;
		}
		figObj.ondblclick = !elWr ? null : function(e) {
		    if(!e) e = window.event;
		    servSet(this.wdgLnk.addr,'com=obj&sub=point&geomX='+geomX.toFixed(3)+'&geomY='+geomY.toFixed(3)+
						    '&xSc='+xSc.toFixed(3)+'&ySc='+ySc.toFixed(3)+
						    '&x='+(e.offsetX?e.offsetX:(e.clientX-posGetX(this)))+
						    '&y='+(e.offsetY?e.offsetY:(e.clientY-posGetY(this)))+
						    '&key=DblClick','');
		}
	    }

	    if(toInit) {
		figObj.border = 0;
		figObj.wdgLnk = this;
		this.place.appendChild(figObj);
		//Disable drag mostly for FireFox
		figObj.onmousedown = function(e) { e = e?e:window.event; if(e.preventDefault) e.preventDefault(); }
	    }
	}
	else if(this.attrs['root'] == 'Box') {
	    if(this == masterPage && this.attrs['tipStatus'].length) { setStatus(this.attrs['tipStatus'],10000); this.attrs['tipStatus'] = ""; }
	    elStyle += 'border-style: solid; border-width: '+this.attrs['bordWidth']+'px; ';
	    if(elWr && this.attrs['backColor'].length && getColor(this.attrs['backColor'],true)) elStyle += 'cursor: pointer; ';

	    if(this.attrs['vs_winTitle']) {
		if(this == masterPage || (this.pg && this.window && this.windowExt))
		    window.document.title = this.attrs['vs_winTitle'];
		else if(this.pg && this.window && !this.windowExt)
		    this.place.parentElement.offsetParent.rows[0].cells[0].innerText = this.attrs['vs_winTitle'];
	    }
	    if((tVl=this.attrs['userSetVis']) && !full && tVl.length && this.attrsMdf['userSetVis']) {
		if(tVl == '*') window.location = '/login'+window.location.pathname;
		else { servSet(this.addr, 'com=setUser&user='+tVl, ''); window.location.reload(); }
	    }
	    for(iNtf = 0; iNtf < 8; iNtf++)
		if((tVl=this.attrs["notifyVis"+MOD_ID+iNtf]) && this.attrsMdf["notifyVis"+MOD_ID+iNtf])
		    ntfReg(iNtf, tVl, this.addr, true);
		else if((tVl=this.attrs["notify"+iNtf]) && this.attrsMdf["notify"+iNtf])
		    ntfReg(iNtf, tVl, this.addr, false);

	    if(!this.pg && ((this.inclOpen && this.attrs['pgOpenSrc'] != this.inclOpen) ||
		    (!this.inclOpen && this.attrs['pgOpenSrc'].length)))
	    {
		if(this.inclOpen) {
		    servSet(this.inclOpen, 'com=pgClose&cacheCntr&cachePg', '');
		    this.pages[this.inclOpen].pwClean();

		    this.pages[this.inclOpen].reqTm = tmCnt;
		    pgCacheProc(this.pages[this.inclOpen]);
		    //this.place.removeChild(this.pages[this.inclOpen].place);
		    while(this.place.children.length) this.place.removeChild(this.place.children[0]);
		    this.pages[this.inclOpen].perUpdtEn(false);
		    delete this.pages[this.inclOpen];
		    this.inclOpen = null;
		}
		if(this.attrs['pgOpenSrc'].length) {
		    this.inclOpen = this.attrs['pgOpenSrc'];
		    if((pgO=pgCacheGet(this.inclOpen))) {
			this.pages[this.inclOpen] = pgO;
			this.place.appendChild(this.pages[this.inclOpen].place);
			this.pages[this.inclOpen].perUpdtEn(true);
			this.pages[this.inclOpen].makeEl(servGet(this.inclOpen, 'com=attrsBr&tm='+pgO.reqTm));
		    }
		    else {
			var iPg = new pwDescr(this.inclOpen, true, this);
			iPg.place = this.place.ownerDocument.createElement('div');
			iPg.makeEl(servGet(this.inclOpen,'com=attrsBr'), false, true);

			this.pages[this.inclOpen] = iPg;
			this.place.appendChild(iPg.place);
		    }
		}
	    }
	    this.place.wdgLnk = this;
	}
	else if(this.attrs['root'] == 'Text') {
	    elStyle += 'border-style: solid; border-width: '+this.attrs['bordWidth']+'px; overflow: hidden; ';
	    if(elWr && this.attrs['text'].length && this.attrs['backColor'].length && getColor(this.attrs['backColor'],true)) elStyle += 'cursor: pointer; ';
	    if(elMargin) { elStyle += 'padding: '+elMargin+'px; '; elMargin = 0; }
	    //if(parseInt(this.attrs['orient']) == 0) {
		var txtAlign = parseInt(this.attrs['alignment']);
		var spanStyle = "display: table-cell; width: "+geomW+"px; height: "+geomH+"px; line-height: 1; white-space: "+(parseInt(this.attrs["wordWrap"])?"pre-wrap":"pre")+"; ";
		switch(txtAlign&0x3) {
		    case 0: spanStyle += 'text-align: left; ';		break;
		    case 1: spanStyle += 'text-align: right; ';		break;
		    case 2: spanStyle += 'text-align: center; ';	break;
		    case 3: spanStyle += 'text-align: justify; ';	break;
		}
		switch(txtAlign>>2) {
		    case 0: spanStyle += 'vertical-align: top; ';	break;
		    case 1: spanStyle += 'vertical-align: bottom; ';	break;
		    case 2: spanStyle += 'vertical-align: middle; ';	break;
		}
		if(parseInt(this.attrs['orient']) != 0) spanStyle += "transform: rotate("+parseInt(this.attrs['orient'])+"deg); ";
		spanStyle += getFont(this.attrs['font'], Math.min(xSc, ySc));
		spanStyle += 'color: ' + (this.attrs['color']?getColor(this.attrs['color']):'black') + '; ';
		var txtVal = this.attrs['text'];
		for(var i = 0; i < parseInt(this.attrs['numbArg']); i++) {
		    var argVal;
		    var argCfg = new Array();
		    switch(parseInt(this.attrs['arg'+i+'tp'])) {
			case 0:
			    argCfg[0] = this.attrs['arg'+i+'cfg'];
			    argVal = parseInt(this.attrs['arg'+i+'val']);
			    if(isNaN(argVal)) argVal = 0;
			    break;
			case 1:
			    argCfg = this.attrs['arg'+i+'cfg'].split(';');
			    if(argCfg[1] == 'g')	argVal = parseFloat(this.attrs['arg'+i+'val']).toPrecision(Math.max(0,argCfg[2]));
			    else if(argCfg[1] == 'e')	argVal = parseFloat(this.attrs['arg'+i+'val']).toExponential(Math.max(0,argCfg[2]));
			    else if(argCfg[1] == 'f')	argVal = parseFloat(this.attrs['arg'+i+'val']).toFixed(Math.max(0,argCfg[2]));
			    else argVal = this.attrs['arg'+i+'val'];
			    if(isNaN(argVal)) argVal = 0;
			    break;
			case 2:
			    argCfg[0] = this.attrs['arg'+i+'cfg'];
			    argVal = this.attrs['arg'+i+'val'];
			    break;
		    }
		    var argSize = Math.max(-1000,Math.min(1000,parseInt(argCfg[0])));
		    var argPad = '';
		    for(var j = argVal.length; j < Math.abs(argSize); j++) argPad += ' ';//&nbsp;';
		    if(argSize > 0) argVal = argPad+argVal; else argVal += argPad;
		    txtVal = txtVal.replace('%'+(i+1), argVal);
		}
		var spEl = this.place.children.length ? this.place.children[0] : this.place.ownerDocument.createElement('span');
		this.place.appendChild(spEl);
		if(parseInt(this.attrs['inHtml'])) spEl.innerHTML = txtVal;
		else spEl.innerText = txtVal;
		spEl.setAttribute('style', spanStyle);
		//this.place.innerHTML = "<span style='"+spanStyle+"'>"+txtVal+"</span>";
	    //}
	    //else this.place.innerHTML = "<img width='"+geomW+"px' height='"+geomH+"px' border='0' src='/"+MOD_ID+this.addr+
	    //				"?com=obj&tm="+tmCnt+"&xSc="+xSc.toFixed(3)+"&ySc="+ySc.toFixed(3)+"'/>";

	    //this.place.wdgLnk = this;
	}
	else if(this.attrs['root'] == 'Media') {
	    elStyle += 'border-width: '+this.attrs['bordWidth']+'px; ';
	    if(elWr && !parseInt(this.attrs['areas']) && this.attrs['src'].length && this.attrs['backColor'].length && getColor(this.attrs['backColor'],true))
		elStyle += 'cursor: pointer; ';
	    if(this.place.elWr != elWr || (parseInt(this.attrs['areas']) && this.place.children.length <= 1) ||
					  (!parseInt(this.attrs['areas']) && this.place.children.length > 1) ||
					  ((this.attrsMdf["src"] || this.attrsMdf["fit"]) && this.attrs['fit'] != 1))
		while(this.place.children.length) this.place.removeChild(this.place.children[0]);

	    var toInit = !this.place.children.length;
	    if(this.attrs['type'] == 2)		medObjTp = 'video';
	    else if(this.attrs['type'] == 3)	medObjTp = 'audio';
	    else medObjTp = 'img';
	    var medObj = toInit ? this.place.ownerDocument.createElement(medObjTp) : this.place.children[0];
	    if(this.attrs['roll'] && (toInit || this.attrsMdf["roll"])) medObj.loop = parseInt(this.attrs['roll']);
	    if(this.attrs['pause'] && (toInit || this.attrsMdf["pause"]) && parseInt(this.attrs['play'])) {
		if(parseInt(this.attrs['pause'])) medObj.pause();
		else medObj.play();
	    }
	    if(this.attrs['seek'] && (toInit || this.attrsMdf["seek"]) && Math.abs(parseFloat(this.attrs['seek'])-medObj.currentTime) > 10)
		medObj.currentTime = parseFloat(this.attrs['seek']);
	    if(this.attrs['volume'] && (toInit || this.attrsMdf["volume"])) medObj.volume = parseFloat(this.attrs['volume'])/100;
	    if(toInit || this.attrsMdf["src"] || this.attrsMdf["fit"] || this.attrsMdf["play"] || !pgBr) {
		if(!this.attrs['src'].length || (this.attrs['play'] && !parseInt(this.attrs['play'])))	medObj.src = "";
		else if(this.attrs['src'].indexOf("http://") == 0 || this.attrs['src'].indexOf("https://") == 0)
		    medObj.src = this.attrs['src'];
		else medObj.src = "/"+MOD_ID+this.addr+"?com=res&val="+this.attrs['src'];
		medObj.hidden = !this.attrs['src'].length;
	    }
	    if(this.attrs['play'] && (toInit || this.attrsMdf["play"])) {
		if(parseInt(this.attrs['play'])) {
		    if(toInit) medObj.autoplay = true;
		    else medObj.play();
		}
		this.perUpdtEn(parseInt(this.attrs['play']));
	    }
	    if(toInit || this.attrsMdf["fit"] || !pgBr) {
		if(this.attrs['fit'] == 1) {
		    medObj.width = geomW; medObj.height = geomH;
		    // Only for the type "Image(0)"
		    if(this.attrs['src'].length && this.attrs['type'] == 0) medObj.src += "&size="+geomH;
		    medObj.onload = null;
		}
		else medObj.onload = function() {
		    cWdth = this.width;
		    cHeight = this.height;
		    this.width = cWdth * this.wdgLnk.xScale(true);
		    this.height = cHeight * this.wdgLnk.yScale(true);
		    //this.style.cssText = "top: "+((this.parentNode.height-this.height)/2)+"px; "+
		    //				"left: "+((this.parentNode.width-this.width)/2)+"px; ";
		}
	    }
	    if(elWr && (toInit || this.attrsMdf["areas"])) {
		var mapObj = toInit ? this.place.ownerDocument.createElement('map') : this.place.children[1];
		while(mapObj.children.length) mapObj.removeChild(mapObj.children[0]);
		for(var i = 0; i < parseInt(this.attrs['areas']); i++) {
		    var arObj = this.place.ownerDocument.createElement('area');
		    switch(parseInt(this.attrs['area'+i+'shp'])) {
			case 0: arObj.shape = 'rect';	break;
			case 1: arObj.shape = 'poly';	break;
			case 2: arObj.shape = 'circle';	break;
		    }
		    arObj.coords = this.attrs['area'+i+'coord'];
		    arObj.title = this.attrs['area'+i+'title'];
		    arObj.href = '';
		    arObj.areaId = i;
		    arObj.onclick = function( )	{ setWAttrs(this.wdgLnk.addr,'event','ws_MapAct'+this.areaId+'Left'); return false; }
		    arObj.wdgLnk = this;
		    mapObj.appendChild(arObj);
		}
	    }
	    if(toInit) {
		medObj.wdgLnk = this;
		if(this.attrs['play']) {
		    medObj.onloadeddata = function() { setWAttrs(this.wdgLnk.addr,'size',this.duration.toFixed(1)); return false; }
		    if(this.attrs['type'] == 2)	medObj.setAttribute('style','height: inherit; width: inherit; ');
		}
		//Disable drag mostly for FireFox
		medObj.onmousedown = function(e) { e = e?e:window.event; if(e.preventDefault) e.preventDefault(); }
		medObj.border = 0;
		this.place.appendChild(medObj);
		if(elWr) {
		    this.place.appendChild(mapObj);
		    mapObj.name = this.addr;
		    medObj.setAttribute('usemap','#'+this.addr);
		}
	    }
	}
	else if(this.attrs['root'] == 'FormEl' && !this.place.isModify) {
	    var elTp = parseInt(this.attrs['elType']);
	    if(this.attrsMdf['elType'] || this.place.elWr != elWr)
		while(this.place.childNodes.length) this.place.removeChild(this.place.childNodes[0]);

	    if(this.attrsMdf['font'])	this.place.fontCfg = getFontCond(this.attrs['font'], Math.min(xSc,ySc));
	    var fntSz = Math.min(geomH, (getFont(this.attrs['font'],Math.min(xSc,ySc),2)*1.4).toFixed(0));
	    var applySz = Math.max(16, fntSz);

	    // Processing the custom behaviour attributes
	    bordStyle = backStyle = null;
	    if(this.attrs['vs_border'])	bordStyle = this.attrs['vs_border'];
	    if(this.attrs['vs_background']) {	//!!!! Must be moved to a separate function for the transparent background of the included items
		if((backClr=this.attrs['vs_background'].split(' ')[0]) && (backOp=getColor(backClr,true))) {
		    if(backOp == 1) {
			backStyle = 'background-color: '+getColor(backClr)+'; ';
			if(this.placeTr) this.placeTr.style.cssText = elStyle;
		    }
		    else {
			backStyle = '';
			if(!this.placeTr) {
			    this.placeTr = this.place.ownerDocument.createElement('div');
			    this.place.parentNode.appendChild(this.placeTr);
			}
			this.placeTr.style.cssText = elStyle + 'width: ' + geomW + 'px; height: ' + geomH + 'px; background-color: '+getColor(backClr)+'; opacity: '+backOp+"; z-index: "+(parseInt(this.attrs['geomZ'])-1)+"; ";
		    }
		}
		if((backImg=this.attrs['vs_background'].split(' ')[1]))	backStyle = (backStyle?backStyle:'') + 'background-image: url(\'/'+MOD_ID+this.addr+'?com=res&val='+backImg+'\'); ';
	    }

	    var comElMdf = (this.attrs['vs_border'] && this.attrsMdf['vs_border']) || (this.attrs['vs_background'] && this.attrsMdf['vs_background']);

	    switch(elTp) {
		case 0:	//Line edit
		    var toInit = !this.place.childNodes.length;
		    var formObj = toInit ? this.place.ownerDocument.createElement('input') : this.place.childNodes[0];
		    if(toInit || comElMdf || this.attrsMdf['geomH'] || this.attrsMdf['geomW'] || this.attrsMdf['font']) {
			brdW = (bordStyle?parseInt(bordStyle):1) + 1;
			var geomWint = geomW - 2*brdW;
			formObj.className = "LineEd";
			formObj.style.cssText = 'padding: 1px; top: '+((geomH-fntSz)/2)+'px; width: '+geomWint+'px; '+
						'height: '+(fntSz-brdW)+'px; font: '+this.place.fontCfg+';';
			formObj.style.cssText += "border: "+(bordStyle?bordStyle:"1px solid gray")+"; ";
			formObj.style.cssText += (backStyle == null) ? 'background-color: white; ' : (backStyle.length?backStyle:'');
		    }
		    if(formObj.valSet && this.attrsMdf['value']) formObj.valSet(this.attrs['value']);
		    this.place.view = parseInt(this.attrs['view']);
		    this.place.cfg = this.attrs['cfg'];
		    if(!toInit) break;
		    formObj.wdgLnk = this;
		    formObj.disabled = !elWr;
		    this.place.appendChild(formObj);
		    if(elWr) {
			switch(this.place.view) {
			    case 1:		//Combo
				var combImg = this.place.ownerDocument.createElement('img');
				combImg.className = "cntr"; combImg.src = '/'+MOD_ID+'/img_combar';
				combImg.style.cssText = 'left: '+(geomW-16)+'px; top: '+((geomH-fntSz)/2)+'px; height: '+fntSz+'px; ';
				this.place.appendChild(combImg);
				formObj.style.width = (geomWint-16)+'px';
				combImg.onclick = function( ) {
				    var formObj = this.parentNode.children[0];
				    var combList = this.ownerDocument.getElementById('combomenu');
				    if(!combList) {
					combList = this.ownerDocument.createElement('div');
					combList.id = 'combomenu';
					combList.appendChild(this.ownerDocument.createElement('select'));
					combList.childNodes[0].size = '100';
					combList.childNodes[0].onclick = function( ) {
					    //this.formObj.value = this.options[this.selectedIndex].value;
					    //this.formObj.setModify(true);
					    this.formObj.valSet(this.options[this.selectedIndex].value);
					    this.formObj.chApply();
					    this.parentNode.style.visibility = 'hidden';
					    this.parentNode.style.top = "-100px";
					}
					combList.childNodes[0].onblur = function( ) {
					    this.parentNode.style.visibility = 'hidden';
					    this.parentNode.style.top = "-100px";
					}
					combList.onmouseleave = function( ) { this.style.visibility = 'hidden'; this.style.top = "-100px"; }
					this.ownerDocument.body.appendChild(combList);
				    }
				    while(combList.childNodes[0].childNodes.length)
					combList.childNodes[0].removeChild(combList.childNodes[0].childNodes[0]);
				    var elLst = formObj.parentNode.cfg.split('\n');
				    for(var i = 0; i < elLst.length; i++) {
					var optEl = this.ownerDocument.createElement('option');
					optEl.appendChild(this.ownerDocument.createTextNode(elLst[i]));
					if(formObj.valGet() == elLst[i]) optEl.defaultSelected = optEl.selected = true;
					combList.childNodes[0].appendChild(optEl);
				    }
				    if(combList.childNodes[0].childNodes.length) {
					combList.style.cssText = 'left: '+posGetX(formObj,true)+'px; top: '+(posGetY(formObj,true)+formObj.offsetHeight)+'px; '+
								 'width: '+formObj.offsetWidth+'px; height: '+(Math.min(elLst.length,10)*parseInt(this.style.height))+'px; ';
					combList.childNodes[0].style.cssText = 'width: '+formObj.offsetWidth+'px; height: '+(Math.min(elLst.length,10)*parseInt(this.style.height))+'px; '+
									       'font: '+formObj.parentNode.fontCfg+'; ';
					combList.childNodes[0].formObj = formObj;
					combList.childNodes[0].focus();
				    }
				    return false;
				}
				break;
			    case 2: case 3:	//Integer, Real
				if(this.place.childNodes.length >= 2) break;
				var spinImg = this.place.ownerDocument.createElement('img');
				spinImg.className = "cntr"; spinImg.src = '/'+MOD_ID+'/img_spinar';
				spinImg.style.cssText = 'left: '+(geomW-16)+'px; top: '+((geomH-fntSz)/2)+'px; height: '+fntSz+'px; ';
				spinImg.border = '0';
				formObj.style.width = (geomWint-16)+'px';
				spinImg.onclick = function(e) {
				    if(!e) e = window.event;
				    var formObj = this.parentNode.childNodes[0];
				    var argCfg = this.parentNode.cfg.split(':');
				    if((e.clientY-posGetY(this)) < fntSz/2)
					formObj.valSet(formObj.valGet()+((argCfg.length>2)?((this.parentNode.view==2)?parseInt(argCfg[2]):parseFloat(argCfg[2])):1), true);
				    else formObj.valSet(formObj.valGet()-((argCfg.length>2)?((this.parentNode.view==2)?parseInt(argCfg[2]):parseFloat(argCfg[2])):1), true);
				    formObj.setModify(true);
				    return false;
				}
				this.place.appendChild(spinImg);
				break;
			    case 4:	break;	//Time
			    case 5:	//Date
			    case 6:	//Date and time
				formObj.onclick = function( ) {
				    if((cldrDlg=this.ownerDocument.getElementById('clndrdlg'))) cldrDlg.style.visibility = 'hidden';
				}
				var cldrImg = this.place.ownerDocument.createElement('img');
				cldrImg.className = "cntr"; cldrImg.src = '/'+MOD_ID+'/img_combar';
				cldrImg.style.cssText = 'left: '+(geomW-16)+'px; top: '+((geomH-fntSz)/2)+'px; height: '+fntSz+'px; ';
				this.place.appendChild(cldrImg);
				formObj.style.width = (geomWint-16)+'px';
				cldrImg.onclick = function( ) {
				    var formObj = this.parentNode.children[0];
				    var cldrDlg = this.ownerDocument.getElementById('clndrdlg');
				    if(!cldrDlg) {
					cldrDlg = this.ownerDocument.createElement('div');
					cldrDlg.id = 'clndrdlg';
					cldrDlg.innerHTML = "<input type='button' value='###Today###'/><select></select><select></select>"+
					    "<table><tr><th>###Mon###</th><th>###Tue###</th><th>###Wed###</th><th>###Thr###</th><th>###Fri###</th><th class='end'>###Sat###</th><th class='end'>###Sun###</th></tr>"+
					    "<tr><td/><td/><td/><td/><td/><td/><td/></tr>"+
					    "<tr><td/><td/><td/><td/><td/><td/><td/></tr>"+
					    "<tr><td/><td/><td/><td/><td/><td/><td/></tr>"+
					    "<tr><td/><td/><td/><td/><td/><td/><td/></tr>"+
					    "<tr><td/><td/><td/><td/><td/><td/><td/></tr>"+
					    "<tr><td/><td/><td/><td/><td/><td/><td/></tr>"+
					    "</table>";
					cldrDlg.children[0].onclick = function( ) {
					    this.parentElement.formObj.valSet((new Date()).getTime()/1000);
					    this.parentElement.formObj.chApply();
					    this.parentElement.style.visibility = 'hidden';
					    this.parentElement.style.top = "-100px";
					}
					cldrDlg.children[1].onchange = cldrDlg.children[2].onchange = function( ) {
					    this.parentElement.tmSet((new Date(parseInt(this.parentElement.children[1].options[this.parentElement.children[1].selectedIndex].value),
									this.parentElement.children[2].selectedIndex,1)).getTime()/1000);
					}
					cldrDlg.daySet = function() {
					    dlgN = this.offsetParent.parentElement;
					    dtSrc = new Date(dlgN.formObj.valGet()*1000);
					    dtSrc.setFullYear(parseInt(dlgN.children[1].options[dlgN.children[1].selectedIndex].value));
					    dtSrc.setMonth(dlgN.children[2].selectedIndex);
					    dtSrc.setDate(parseInt(this.textContent));
					    dlgN.formObj.valSet(dtSrc.getTime()/1000);
					    dlgN.formObj.chApply();
					    dlgN.style.visibility = 'hidden';
					    dlgN.style.top = "-100px";
					    dlgN.cldrDlg = null;
					}
					//Fill and activate the table
					cldrDlg.tmSet = function(tm) {
					    var dtCur = new Date();
					    var dtSet = new Date(parseInt(tm)*1000);
					    // Years fill
					    var optLs = ""; hasCur = false;
					    for(iY = 0; iY < 10; iY++) {
						iY_ = (dtCur.getFullYear()-iY);
						if(iY_ != dtSet.getFullYear()) optLs += "<option>"+iY_+"</option>";
						else { optLs += "<option selected>"+iY_+"</option>"; hasCur = true; }
					    }
					    if(!hasCur) optLs += "<option selected>"+dtSet.getFullYear()+"</option>";
					    this.children[1].innerHTML = optLs;
					    // Months fill
					    var optLs = "";
					    for(iM = 0; iM < 12; iM++) {
						var mNm = "";
						if(iM == 0)		mNm = "###January###";
						else if(iM == 1)	mNm = "###February###";
						else if(iM == 2)	mNm = "###March###";
						else if(iM == 3)	mNm = "###April###";
						else if(iM == 4)	mNm = "###May###";
						else if(iM == 5)	mNm = "###June###";
						else if(iM == 6)	mNm = "###July###";
						else if(iM == 7)	mNm = "###August###";
						else if(iM == 8)	mNm = "###September###";
						else if(iM == 9)	mNm = "###October###";
						else if(iM == 10)	mNm = "###November###";
						else if(iM == 11)	mNm = "###December###";
						optLs += "<option"+((iM==dtSet.getMonth())?" selected":"")+">"+mNm+"</option>";
					    }
					    this.children[2].innerHTML = optLs;
					    // Days fill
					    dtSetMBeg = new Date(dtSet.getFullYear(), dtSet.getMonth(), 1);
					    maxDayInMonth = 28;
					    while((new Date(dtSet.getFullYear(),dtSet.getMonth(),maxDayInMonth+1)).getMonth() == dtSet.getMonth() && maxDayInMonth < 31)
						maxDayInMonth++;
					    for(iW = 0, iD = 1; iW < 6; iW++)
						for(iWd = 0; iWd < 7; iWd++) {
						    tdEl = this.children[3].rows[iW+1].cells[iWd];
						    if((iW == 0 && iWd < ((tVl=dtSetMBeg.getDay()-1)<0?7+tVl:tVl)) || iD > maxDayInMonth)
						    { tdEl.innerHTML = ""; tdEl.className = ""; tdEl.onclick = null; continue; }
						    tdEl.innerHTML = iD;
						    tdEl.className = "active";
						    tdEl.onclick = this.daySet;
						    if(iD == dtSet.getDate()) tdEl.className += " sel";
						    if(iWd == 5 || iWd == 6) tdEl.className += " end";
						    iD++;
						}
					    this.formObj.setModify(true);
					}
					this.ownerDocument.body.appendChild(cldrDlg);
				    }
				    //cldrDlg.onmouseleave = formObj.onclick;
				    formObj.cldrDlg = cldrDlg;
				    cldrDlg.formObj = formObj;
				    cldrDlg.tmSet(formObj.valGet());
				    cldrDlg.style.cssText = 'left: '+posGetX(formObj,true)+'px; top: '+(posGetY(formObj,true)+formObj.offsetHeight)+'px; '+
					'font: '+formObj.parentNode.fontCfg+'; ';
				    return false;
				}
				break;
			    case 7:	//Password
				formObj.type = "password";
				break;
			}
			formObj.onmousedown = function(e) { this.setModify(true); }
			formObj.onkeyup = function(e) {
			    e.stopImmediatePropagation();
			    if(this.modify() && e.keyCode == 13) this.chApply();
			    if(this.modify() && e.keyCode == 27) this.chEscape();
			    if(this.saveVal != this.value) this.setModify(true);
			    return true;
			}
			formObj.onkeydown = function(e) { e.stopImmediatePropagation(); return true; }
			formObj.oncontextmenu = function(e) { e.stopImmediatePropagation(); return true; }
			formObj.modify = function( )
			{ return (this.parentNode.children[this.parentNode.children.length-1].style.visibility == 'visible'); }
			formObj.setModify = function(on) {
			    if(on && this.tmClearEdit) this.tmClearEdit = tmClearEdit;
			    if(this.modify() == on) return;
			    var posOkImg = this.parentNode.children.length-1;
			    var okImg = this.parentNode.children[posOkImg];
			    if(on) {
				this.style.width = (parseInt(this.style.width)-applySz)+'px';
				if(posOkImg == 2)
				    this.parentNode.children[1].style.left = (parseInt(this.parentNode.children[1].style.left)-applySz)+'px';
				okImg.style.visibility = 'visible';
				this.wdgLnk.perUpdtEn(true); this.tmClearEdit = tmClearEdit;
			    }
			    else {
				this.style.width = (parseInt(this.style.width)+applySz)+'px';
				if(posOkImg == 2)
				    this.parentNode.children[1].style.left = (parseInt(this.parentNode.children[1].style.left)+applySz)+'px';
				okImg.style.visibility = 'hidden';
				this.wdgLnk.perUpdtEn(false); this.tmClearEdit = 0;
				if(this.cldrDlg) this.onclick();
			    }
			    this.parentNode.isModify = on;
			}
		    }
		    formObj.valSet = function(val, noUpSave) {
			switch(this.parentNode.view) {
			    case 0: case 1: case 7: this.value = val; break;
			    case 2:	//Integer
				if(isNaN(tVal=parseInt(val))) tVal = 0;
				var argCfg = this.parentNode.cfg.split(':');
				this.value = ((argCfg.length>3)?argCfg[3]:'')+
				    Math.max((argCfg.length>1)?parseInt(argCfg[0]):0,Math.min((argCfg.length>1)?parseInt(argCfg[1]):100,parseInt(tVal)))+((argCfg.length>4)?argCfg[4]:'');
				break;
			    case 3:	//Real
				if(isNaN(tVal=parseFloat(val))) tVal = 0;
				var argCfg = this.parentNode.cfg.split(':');
				this.value = ((argCfg.length>3)?argCfg[3]:'')+Math.max((argCfg.length>1)?parseFloat(argCfg[0]):0,Math.min((argCfg.length>1)?parseFloat(argCfg[1]):100,parseFloat(tVal))).toFixed((argCfg.length>5)?parseInt(argCfg[5]):2)+((argCfg.length>4)?argCfg[4]:'');
				break;
			    case 4:	//Time
				var rez = (this.parentNode.cfg.length) ? this.parentNode.cfg : 'hh:mm';
				var v = (Math.floor(val/3600)%(ap?12:24)).toString(10); rez = rez.replace('hh',i2s(v,10,2)); rez = rez.replace('h',v);
				v = (Math.floor(val/60)%60).toString(10); rez = rez.replace('mm',i2s(v,10,2)); rez = rez.replace('m',v);
				v = (val%60).toString(10); rez = rez.replace('ss',i2s(v,10,2)); rez = rez.replace('s',v);
				if(rez.indexOf('ap') >= 0) { rez = rez.replace('ap',(val>=43200)?'pm':'am'); var ap = true; }
				if(rez.indexOf('AP') >= 0) { rez = rez.replace('AP',(val>=43200)?'PM':'AM'); var ap = true; }
				this.value = rez;
				break;
			    case 5:	//Date
				var rez = (this.parentNode.cfg.length) ? this.parentNode.cfg : 'dd.MM.yy';
				var dt = new Date(parseInt(val)*1000);
				var v = dt.getDate().toString(10); rez = rez.replace('dddd',i2s(v,10,2)); rez = rez.replace('ddd',i2s(v,10,2)); rez = rez.replace('dd',i2s(v,10,2)); rez = rez.replace('d',v);
				v = (dt.getMonth()+1).toString(10); rez = rez.replace('MMMM',i2s(v,10,2)); rez = rez.replace('MMM',i2s(v,10,2)); rez = rez.replace('MM',i2s(v,10,2)); rez = rez.replace('M',v);
				v = dt.getFullYear().toString(10); rez = rez.replace('yyyy',i2s(v,10,4)); rez = rez.replace('yy',i2s(v.substr(2,2),10,2));
				this.value = rez;
				break;
			    case 6:	//Date and time
				var rez = (this.parentNode.cfg.length) ? this.parentNode.cfg : 'dd.MM.yy hh:mm';
				var dt = new Date(parseInt(val)*1000);
				var v = dt.getDate().toString(10); rez = rez.replace('dddd',i2s(v,10,2)); rez = rez.replace('ddd',i2s(v,10,2)); rez = rez.replace('dd',i2s(v,10,2)); rez = rez.replace('d',v);
				v = (dt.getMonth()+1).toString(10); rez = rez.replace('MMMM',i2s(v,10,2)); rez = rez.replace('MMM',i2s(v,10,2)); rez = rez.replace('MM',i2s(v,10,2)); rez = rez.replace('M',v);
				v = dt.getFullYear().toString(10); rez = rez.replace('yyyy',i2s(v,10,4)); rez = rez.replace('yy',i2s(v.substr(2,2),10,2));
				v = dt.getHours().toString(10); rez = rez.replace('hh',i2s(v,10,2)); rez = rez.replace('h',v);
				v = dt.getMinutes().toString(10); rez = rez.replace('mm',i2s(v,10,2)); rez = rez.replace('m',v);
				v = dt.getSeconds().toString(10); rez = rez.replace('ss',(v.length==1)?'0'+v:v); rez = rez.replace('s',v);
				if(rez.indexOf('ap') >= 0)	{ rez = rez.replace('ap',(val>=43200)?'pm':'am'); var ap = true; }
				if(rez.indexOf('AP') >= 0)	{ rez = rez.replace('AP',(val>=43200)?'PM':'AM'); var ap = true; }
				this.value = rez;
				break;
			}
			if(noUpSave == null || !noUpSave)	this.saveVal = this.value;
			this.srcVal = val;
		    }
		    formObj.valGet = function( ) {
			switch(this.parentNode.view) {
			    case 0: case 1: case 7: return this.value;
			    case 2:
				var rez = this.value;
				var argCfg = this.parentNode.cfg.split(':');
				var tmp = (argCfg&&argCfg.length>3) ? noSpace(argCfg[3]) : '';
				if(tmp.length && rez.indexOf(tmp) >= 0) rez = rez.substring(rez.indexOf(tmp)+tmp.length);
				tmp = (argCfg&&argCfg.length>4) ? noSpace(argCfg[4]) : '';
				if(tmp.length && rez.indexOf(tmp) >= 0) rez = rez.substring(0,rez.indexOf(tmp));
				return parseInt(rez);
			    case 3:
				var rez = this.value;
				var argCfg = this.parentNode.cfg.split(':');
				var tmp = (argCfg&&argCfg.length>3) ? noSpace(argCfg[3]) : '';
				if(tmp.length && rez.indexOf(tmp) >= 0) rez = rez.substring(rez.indexOf(tmp)+tmp.length);
				tmp = (argCfg&&argCfg.length>4) ? noSpace(argCfg[4]) : '';
				if(tmp.length && rez.indexOf(tmp) >= 0) rez = rez.substring(0,rez.indexOf(tmp));
				return parseFloat(rez);
			    case 4:
				var cfg = (this.parentNode.cfg.length) ? this.parentNode.cfg : 'hh:mm';
				var rez = this.value;
				if(cfg.indexOf('ap') >= 0 || cfg.indexOf('AP') >= 0) {
				    cfg = cfg.replace('ap','');
				    cfg = cfg.replace('AP','');
				    rez = rez.replace('am','');
				    rez = rez.replace('AM','');
				    rez = rez.replace('pm','');
				    rez = rez.replace('PM','');
				}
				var hour = Math.floor(this.srcVal/3600);
				var min  = Math.floor(this.srcVal/60)%60;
				var sec  = this.srcVal%60;
				var i = 0; var lstS = ''; var lenS = 0;
				while(true) {
				    if(i >= cfg.length || cfg.charAt(i) == 'h' || cfg.charAt(i) == 'm' || cfg.charAt(i) == 's') {
					var space = lenS ? noSpace(cfg.substr(i-lenS,lenS)) : '';
					if(lenS && !space.length) space = ' ';
					if((space.length && lstS != '') || i >= cfg.length) {
					    if(space.length && rez.indexOf(space) == -1) return 0;
					    var val = parseInt(space.length?rez.substr(0,rez.indexOf(space)):rez,10);
					    rez = rez.substr(rez.indexOf(space));
					    if(lstS == 'h')	hour = val;
					    else if(lstS == 'm')min = val;
					    else if(lstS == 's')sec = val;
					    if(i >= cfg.length) break;
					}
					if(space.length) {
					    if(rez.indexOf(space) == -1) return 0;
					    rez = rez.substr(rez.indexOf(space)+space.length);
					}
					lstS = cfg.charAt(i); lenS = 0;
					i++;
					continue;
				    }
				    lenS++; i++;
				}
				if(this.value.indexOf('pm') >= 0 || this.value.indexOf('PM') >= 0) hour += 12;
				return (hour%24)*3600+(min%60)*60+(sec%60);
			    case 5:
				var cfg = (this.parentNode.cfg.length) ? this.parentNode.cfg : 'd.MM.yy';
				var rez  = this.value;
				var dt = new Date(this.srcVal*1000);
				var i = 0; var lstS = ''; var lenS = 0;
				while(true) {
				    if(i >= cfg.length || cfg.charAt(i) == 'd' || cfg.charAt(i) == 'M' || cfg.charAt(i) == 'y') {
					var space = lenS ? noSpace(cfg.substr(i-lenS,lenS)) : '';
					if(lenS && !space.length) space = ' ';
					if((space.length && lstS != '') || i >= cfg.length) {
					    if(space.length && rez.indexOf(space) == -1) return 0;
					    var val = parseInt(space.length?rez.substr(0,rez.indexOf(space)):rez,10);
					    rez = rez.substr(rez.indexOf(space));
					    if(lstS == 'd')	dt.setDate(val);
					    else if(lstS == 'M')dt.setMonth(val-1);
					    else if(lstS == 'y')dt.setFullYear((val<100)?2000+val:val);
					    if(i >= cfg.length) break;
					}
					if(space.length) {
					    if(rez.indexOf(space) == -1) return 0;
					    rez = rez.substr(rez.indexOf(space)+space.length);
					}
					lstS = cfg.charAt(i); lenS = 0;
					i++;
					continue;
				    }
				    lenS++; i++;
				}
				rez = dt.getTime()/1000;
				return rez;
			    case 6:
				var cfg = (this.parentNode.cfg.length) ? this.parentNode.cfg : 'dd.MM.yy hh:mm';
				var rez  = this.value;
				if(cfg.indexOf('ap') >= 0 || cfg.indexOf('AP') >= 0) {
				    cfg = cfg.replace('ap','');
				    cfg = cfg.replace('AP','');
				    rez = rez.replace('am','');
				    rez = rez.replace('AM','');
				    rez = rez.replace('pm','');
				    rez = rez.replace('PM','');
				}
				var dt = new Date(this.srcVal*1000);
				var i = 0; var lstS = ''; var lenS = 0;
				while(true) {
				    if(i >= cfg.length || cfg.charAt(i) == 'd' || cfg.charAt(i) == 'M' || cfg.charAt(i) == 'y' ||
					cfg.charAt(i) == 'h' || cfg.charAt(i) == 'm' || cfg.charAt(i) == 's')
				    {
					var space = lenS ? noSpace(cfg.substr(i-lenS,lenS)) : '';
					if(lenS && !space.length) space = ' ';
					if((space.length && lstS != '') || i >= cfg.length) {
					    if(space.length && rez.indexOf(space) == -1) return 0;
					    var val = parseInt(space.length?rez.substr(0,rez.indexOf(space)):rez,10);
					    rez = rez.substr(rez.indexOf(space));
					    if(lstS == 'd')	dt.setDate(val);
					    else if(lstS == 'M')dt.setMonth(val-1);
					    else if(lstS == 'y')dt.setFullYear((val<100)?2000+val:val);
					    else if(lstS == 'h')dt.setHours(val);
					    else if(lstS == 'm')dt.setMinutes(val);
					    else if(lstS == 's')dt.setSeconds(val);
					    if(i >= cfg.length) break;
					}
					if(space.length) {
					    if(rez.indexOf(space) == -1) return 0;
					    rez = rez.substr(rez.indexOf(space)+space.length);
					}
					lstS = cfg.charAt(i); lenS = 0; i++; continue;
				    }
				    lenS++; i++;
				}
				if(this.value.indexOf('pm') >= 0 || this.value.indexOf('PM') >= 0) dt.setHours(dt.getHours()+12);
				rez = dt.getTime()/1000;
				return rez;
			}
			return '';
		    }
		    formObj.chApply = function( ) {
			var val = this.valGet();
			this.valSet(val);
			this.setModify(false);
			var attrs = new Object(); attrs.value = val; attrs.event = 'ws_LnAccept';
			setWAttrs(this.wdgLnk.addr, attrs);
		    }
		    formObj.chEscape = function( ) {
			this.value = this.saveVal;
			this.setModify(false);
		    }
		    var okImg = this.place.ownerDocument.createElement('img');
		    okImg.className = "ok";
		    okImg.src = '/'+MOD_ID+'/img_button_ok';
		    okImg.style.cssText = 'left: '+(geomW-applySz)+'px; top: '+((geomH-applySz)/2)+'px; height: '+applySz+'px; width: '+applySz+'px; ';
		    okImg.onclick = function() { this.parentNode.childNodes[0].chApply(); return false; };
		    this.place.appendChild(okImg);
		    formObj.valSet(this.attrs['value']);
		    break;
		case 1:	//Text edit
		    var fntSz = (getFont(this.attrs['font'],Math.min(xSc,ySc),2)*1.4).toFixed(0);
		    var applySz = Math.max(16, fntSz);

		    var toInit = !this.place.childNodes.length;
		    var formObj = toInit ? this.place.ownerDocument.createElement('textarea') : this.place.childNodes[0];
		    if(toInit || comElMdf || this.attrsMdf['geomW'] || this.attrsMdf['geomH'] || this.attrsMdf['font']) {
			formObj.style.cssText = 'padding: 1px; width: '+(geomW-5)+'px; height: '+(geomH-5)+'px; font: '+this.place.fontCfg+'; ';
			formObj.style.cssText += "border: "+(bordStyle?bordStyle:"1px solid gray")+"; ";
			formObj.style.cssText += (backStyle == null) ? 'background-color: white; ' : (backStyle.length?backStyle:'');
		    }
		    if(this.attrsMdf['value']) formObj.saveVal = formObj.value = this.attrs['value'];
		    if(!toInit) break;
		    formObj.disabled = !elWr;
		    formObj.wdgLnk = this;
		    formObj.appendChild(this.place.ownerDocument.createTextNode(this.attrs['value']));
		    formObj.onkeyup = function(e) {
			e.stopImmediatePropagation();
			if(this.saveVal != this.value) this.setModify(true);
			if(this.modify()) {
			    if(e.keyCode == 13 && e.ctrlKey) this.nextSibling.onclick();
			    if(e.keyCode == 27) this.nextSibling.nextSibling.onclick();
			}
			return true;
		    };
		    formObj.onkeydown = function(e) { e.stopImmediatePropagation(); return true; }
		    formObj.oncontextmenu = function(e) { e.stopImmediatePropagation(); return true; }
		    formObj.modify = function( ) { return (this.parentNode.childNodes[1].style.visibility == 'visible'); }
		    formObj.setModify = function(on) {
			if(this.modify() == on) return;
			if(on) {
			    this.style.height = (parseInt(this.style.height)-applySz)+'px';
			    this.parentNode.childNodes[1].style.visibility = this.parentNode.childNodes[2].style.visibility = 'visible';
			}
			else {
			    this.style.height = (parseInt(this.style.height)+applySz)+'px';
			    this.parentNode.childNodes[1].style.visibility = this.parentNode.childNodes[2].style.visibility = 'hidden';
			}
		    }
		    var okImg = this.place.ownerDocument.createElement('img');
		    okImg.src = '/'+MOD_ID+'/img_button_ok';
		    okImg.style.cssText = 'visibility: hidden; position: absolute; left: '+(geomW-2*applySz)+'px; '+
					  'top: '+(geomH-applySz)+'px; width: '+applySz+'px; height: '+applySz+'px; cursor: pointer;';
		    okImg.onclick = function( ) {
			var attrs = new Object();
			attrs.value = this.parentNode.childNodes[0].value; attrs.event = 'ws_TxtAccept';
			setWAttrs(this.parentNode.childNodes[0].wdgLnk.addr,attrs);
			this.parentNode.childNodes[0].setModify(false);
			return false;
		    };
		    var cancelImg = this.place.ownerDocument.createElement('img');
		    cancelImg.src = '/'+MOD_ID+'/img_button_cancel';
		    cancelImg.style.cssText = 'visibility: hidden; position: absolute; left: '+(geomW-applySz)+'px; top: '+(geomH-applySz)+'px; width: '+applySz+'px; height: '+applySz+'px; cursor: pointer;';
		    cancelImg.onclick = function( ) {
			this.parentNode.childNodes[0].value = this.parentNode.childNodes[0].saveVal;
			this.parentNode.childNodes[0].setModify(false);
			return false;
		    };
		    this.place.appendChild(formObj);
		    this.place.appendChild(okImg);
		    this.place.appendChild(cancelImg);
		    break;
		case 2:	//Chek box
		    var toInit = !this.place.childNodes.length;
		    var tblCell = toInit ? this.place.ownerDocument.createElement('div') : this.place.childNodes[0];
		    if(toInit || this.attrsMdf['geomH'] || this.attrsMdf['geomW'] || this.attrsMdf['font']) {
			tblCell.className = 'vertalign';
			tblCell.style.cssText = 'position: absolute; top: '+((geomH-15)/2)+'px; width: '+geomW+'px; '+
					    'height: '+Math.min(geomH,15)+'px; text-align: left; font: '+this.place.fontCfg+'; ';
		    }
		    var formObj = tblCell.childNodes.length ? tblCell.childNodes[0] : this.place.ownerDocument.createElement('input');
		    var spanObj = tblCell.childNodes.length > 1 ? tblCell.childNodes[1] : this.place.ownerDocument.createElement('span');
		    spanObj.style.cssText = 'display: table-cell; white-space: pre-line; word-break: break-word; height: '+geomH+'px; ';

		    if(toInit || this.attrsMdf['value']) formObj.checked = parseInt(this.attrs['value']);
		    if(toInit) {
			formObj.type = 'checkbox';
			formObj.disabled = !elWr;
			formObj.style.cssText = 'width: '+Math.min(geomW,geomH)+'px; height: '+Math.min(geomW,geomH)+'px;';
			formObj.wdgLnk = this;
			formObj.onclick = function( ) {
			    var attrs = new Object();
			    attrs.value = this.checked ? '1' : '0';	attrs.event = 'ws_ChkChange';
			    setWAttrs(this.wdgLnk.addr,attrs);
			    return true;
			}
			tblCell.appendChild(formObj);
			tblCell.appendChild(spanObj);
			spanObj.textContent = this.attrs['name'];
			this.place.appendChild(tblCell);
		    }
		    if(this.attrsMdf['name'])	spanObj.textContent = this.attrs['name'];
		    break;
		case 3:	//Button
		    var formObj;
		    var iconImg = this.attrs['img'];
		    this.place.checkable = (parseInt(this.attrs['mode']) == 1);
		    this.place.isMenu = (parseInt(this.attrs['mode']) == 2);
		    this.place.isLoad = (parseInt(this.attrs['mode']) == 3);
		    this.place.isSave = (parseInt(this.attrs['mode']) == 4);
		    var custBut = iconImg;
		    if(custBut != this.place.custBut || this.attrsMdf['mode'])
			while(this.place.childNodes.length) this.place.removeChild(this.place.childNodes[0]);
		    this.place.custBut = custBut;
		    var toInit = !this.place.childNodes.length;
		    formObj = toInit ? this.place.ownerDocument.createElement('button') : this.place.childNodes[0];
		    if(toInit || this.attrsMdf['font'])		formObj.style.font = this.place.fontCfg;
		    if(toInit || this.attrsMdf['color'])	formObj.style.backgroundColor = getColor(this.attrs['color']);
		    if(toInit || this.attrsMdf['colorText'])	formObj.style.color = getColor(this.attrs['colorText']);

		    this.mouseup[this.mouseup.length] = function(e,el) { el.btUp(); }
		    this.mousedown[this.mousedown.length] = function(e,el) { el.btDown(); };
		    if(toInit) {
			formObj.style.cursor = elWr ? 'pointer' : '';
			formObj.disabled = !elWr;
			formObj.wdgLnk = this;
			formObj.style.width = geomW+'px'; formObj.style.height = geomH+'px';
			formObj.style.padding = "0";
			this.place.appendChild(formObj);
			this.place.btDown = function( ) {
			    if(this.checkable) {
				var attrs = new Object();
				if(!this.childNodes[0].classList.contains("pressed"))
				{ attrs.value = '1'; this.childNodes[0].classList.add("pressed"); setWAttrs(this.wdgLnk.addr,'event','ws_BtPress'); }
				else { attrs.value = '0'; this.childNodes[0].classList.remove("pressed"); setWAttrs(this.wdgLnk.addr,'event','ws_BtRelease'); }
				setWAttrs(this.wdgLnk.addr,'value',attrs.value);
				setWAttrs(this.wdgLnk.addr,'event','ws_BtToggleChange');
				return;
			    }
			    this.childNodes[0].classList.add("pressed");
			    setWAttrs(this.wdgLnk.addr,'event','ws_BtPress');
			}
			this.place.btUp = function( ) {
			    if(this.checkable) return;
			    this.childNodes[0].classList.remove("pressed");
			    if(this.wdgLnk.attrs["vs_goHttpUrl"]) window.location = this.wdgLnk.attrs["vs_goHttpUrl"];
			    setWAttrs(this.wdgLnk.addr,'event','ws_BtRelease');
			}
			formObj.onkeydown = function(e)	{ if(e.keyCode == 13 || e.keyCode == 32) this.parentNode.btDown(); }
			formObj.onkeyup = function(e)	{ if(e.keyCode == 13 || e.keyCode == 32) this.parentNode.btUp(); }
		    }

		    if(custBut && !this.place.isLoad) {
			var stdBordWidth = 2;
			var imgObj = formObj.childNodes.length ? formObj.childNodes[0] : this.place.ownerDocument.createElement('img');
			var spanObj = formObj.childNodes.length ? formObj.childNodes[1] : this.place.ownerDocument.createElement('span');

			spanObj.style.cssText = "display: table-cell; height: "+(geomH-2*stdBordWidth)+"px; line-height: 1; text-align: center; white-space: pre-line; word-break: break-word; width: "+(geomW-2*stdBordWidth)+"px; ";
			if(toInit || this.attrsMdf['name']) {
			    spanObj.disabled = !this.attrs['name'].length;
			    spanObj.innerText = this.attrs['name'].replace('\\n','\n') + (this.place.isMenu?" ▾":"");
			}
			if(toInit || this.attrsMdf['img'] || this.attrsMdf['name']) {
			    imgObj.hidden = !this.attrs['img'].length;
			    if(!imgObj.hidden) {
				imgObj.className = elWr ? 'active' : 'inactive';
				imgObj.src = "/"+MOD_ID+this.addr+"?com=res&val="+this.attrs['img']+"&size="+(Math.min(geomW,geomH)-2*stdBordWidth);//+(!elWr?"&filtr=unact":"");
				//imgObj.width = Math.min(geomW, geomH);
				imgObj.height = Math.min(geomW, geomH)-2*stdBordWidth;
				imgObj.style.float = spanObj.disabled ? null : 'left';
				imgObj.style.marginRight = this.attrs['name'].length ? "2px" : "0px";
			    }
			}

			if(toInit) {
			    formObj.className = 'vertalign';
			    //Disable drag mostly for FireFox
			    imgObj.onmousedown = function(e) { e = e?e:window.event; if(e.preventDefault) e.preventDefault(); }
			    formObj.appendChild(imgObj);
			    formObj.appendChild(spanObj);
			}
		    }
		    else if(toInit || this.attrsMdf['name'])	formObj.innerText = this.attrs['name'].replace('\\n','\n') + (this.place.isMenu?" ▾":"");

		    if(this.place.checkable) {
			if(parseInt(this.attrs['value'])) formObj.classList.add("pressed"); else formObj.classList.remove("pressed");
		    }

		    if(this.place.isMenu)
			formObj.onclick = function( ) {
			    var menuWin = this.ownerDocument.getElementById('menuwin');
			    if(!menuWin) {
				menuWin = this.ownerDocument.createElement('div');
				menuWin.id = 'menuwin';
				menuWin.className = "ItemsTree active";
				menuWin.treeFont = 'font:'+this.wdgLnk.place.fontCfg+'; ';
				menuWin.close = function( ) { this.style.visibility = 'hidden'; this.style.top = "-100px"; }
				menuWin.setList = function(list) {
				    if(!this.children.length) this.appendChild(getTree());
				    this.children[0].style.cssText = this.treeFont;
				    this.children[0].select = function(ipath) {
					if(!ipath.length) return;
					attrs = new Object();
					attrs.event = 'ws_BtMenu='+ipath;
					setWAttrs(this.parentNode.formObj.wdgLnk.addr, attrs);
					this.parentNode.close();
				    }
				    this.children[0].setList(list);
				}
				this.ownerDocument.body.appendChild(menuWin);
			    }
			    menuWin.style.cssText = 'left: '+posGetX(this,true)+'px; top: '+(posGetY(this,true)+this.offsetHeight)+'px; ';
			    menuWin.onmouseleave = menuWin.close;
			    menuWin.formObj = this;
			    menuWin.setList(this.wdgLnk.attrs['value']);
			}
		    else if(this.place.isLoad && toInit) {
			iformObj = this.place.ownerDocument.createElement('form');
			iformObj.method = 'post';
			iformObj.action = "/"+MOD_ID+this.addr+"?com=obj"
			iformObj.enctype = "multipart/form-data";
			fHeader = this.attrs['value'].split("\n",1)[0].split("|");
			iformObj.innerHTML = "<input type='file' style='display: none' name='File' accept='"+fHeader[3]+"' defaultValue='"+fHeader[2]+"'/>";
			formObj.iformObj = iformObj;
			formObj.onclick = function( ) { this.iformObj.children[0].click(); }
			iformObj.children[0].wdgLnk = this;
			iformObj.children[0].onchange = function( ) { if(this.value.length) this.form.submit(); }
			this.place.appendChild(iformObj);
		    }
		    else if(this.place.isSave) {
			if(this.attrs['value'].length && this.attrs['value'] != this.valuePrev)
			    window.open('/'+MOD_ID+this.addr+"/"+this.attrs['value'].split("\n",1)[0].split("|")[2]+'?com=obj');
			this.valuePrev = this.attrs['value'];
		    }
		    break;
		case 4: case 5:	//Combo box, List
		    var toInit = !this.place.childNodes.length;
		    var formObj = toInit ? this.place.ownerDocument.createElement('select') : this.place.childNodes[0];
		    if(toInit || comElMdf || this.attrsMdf['geomW'] || this.attrsMdf['geomH'] || this.attrsMdf['font']) {
			formObj.style.cssText = 'padding: 0; top: '+((elTp==4)?(geomH-fntSz)/2:0)+'px; '+
					    'height: '+((elTp==4)?fntSz:geomH)+'px; width: '+geomW+'px; '+
					    'font: '+this.place.fontCfg+'; ';
			formObj.style.cssText += "border: "+(bordStyle?bordStyle:"1px solid gray")+"; ";
			formObj.style.cssText += (backStyle == null) ? 'background-color: white; ' : (backStyle.length?backStyle:'');
		    }
		    formObj.multiple = parseInt(this.attrs['mult']) ? true : null;
		    if(this.attrsMdf['items'] || this.attrsMdf['value']) {
			while(formObj.childNodes.length) formObj.removeChild(formObj.childNodes[0]);
			var selVal = this.attrs['value'].split('\n');
			var elLst = this.attrs['items'].split('\n');
			for(var i = 0; i < elLst.length; i++) {
			    var optEl = this.place.ownerDocument.createElement('option');
			    optEl.appendChild(this.place.ownerDocument.createTextNode(elLst[i]));
			    if((selId=selVal.indexOf(elLst[i])) >= 0) {
				optEl.defaultSelected = optEl.selected = true;
				selVal.splice(selId, 1);
			    }
			    formObj.appendChild(optEl);
			}
			for(i = 0; i < selVal.length && elTp == 4; i++) {
			    var optEl = this.place.ownerDocument.createElement('option');
			    optEl.textContent = selVal[i];
			    optEl.selected = optEl.defaultSelected = true;
			    formObj.appendChild(optEl);
			}
		    }
		    if(!toInit) break;
		    formObj.disabled = !elWr;
		    formObj.wdgLnk = this;
		    if(elTp == 4)
			formObj.onchange = function( ) {
			    var attrs = new Object();
			    attrs.value = this.options[this.selectedIndex].value; attrs.event = 'ws_CombChange';
			    this.wdgLnk.attrs['value'] = attrs.value;
			    setWAttrs(this.wdgLnk.addr,attrs);
			}
		    else {
			formObj.size = 100;
			formObj.onclick = function( ) {
			    var attrs = new Object();
			    attrs.value = "";
			    for(iO = 0; iO < this.options.length; iO++)
				if(this.options[iO].selected) attrs.value += (attrs.value.length?"\n":"") + this.options[iO].value;
			    attrs.event = 'ws_ListChange';
			    this.wdgLnk.attrs['value'] = attrs.value;
			    setWAttrs(this.wdgLnk.addr, attrs);
			}
		    }
		    this.place.appendChild(formObj);
		    break;
		case 6: case 7:	//Slider and Scroll bar
		    isProgr = false;	//(elTp == 7);
		    var applySz = Math.floor(Math.max(16, Math.min(xSc,ySc)*16));
		    var toInit = !this.place.children.length;
		    var formObj = toInit ? this.place.ownerDocument.createElement('input') : this.place.children[0];
		    if(toInit || this.attrsMdf['value']) formObj.value = parseInt(this.attrs['value']);
		    formObj.vOr = false;
		    if(toInit || this.attrsMdf['cfg']) {
			cfgLst = this.attrs['cfg'].split(':');
			formObj.vOr = (cfgLst.length && parseInt(cfgLst[0]));
			formObj.setAttribute("orient", formObj.vOr?"vertical":"");
			formObj.setAttribute("min", (cfgLst.length > 1)?parseInt(cfgLst[1]):0);
			formObj.setAttribute("max", (cfgLst.length > 2)?parseInt(cfgLst[2]):100);
			formObj.setAttribute("step", (cfgLst.length > 3)?parseInt(cfgLst[3]):1);
		    }
		    if(!toInit) break;
		    formObj.type = 'range';
		    formObj.disabled = !elWr;
		    formObj.wdgLnk = this;
		    formObj.style.cssText = "width: "+(formObj.vOr?applySz:(isProgr?geomW-2*applySz:geomW))+"px; "+
			"height: "+(formObj.vOr?(isProgr?geomH-2*applySz:geomH):applySz)+"px; ";
		    formObj.onclick = formObj.onkeyup = function( ) {
			if(this.value == parseInt(this.wdgLnk.attrs['value']))	return;
			var attrs = new Object();
			attrs.value = this.value; attrs.event = 'ws_SliderChange';
			setWAttrs(this.wdgLnk.addr, attrs);
			this.defaultValue = this.value;
			//return true;
		    }
		    this.place.appendChild(formObj);
		    break;
		case 8:	//Tree
		    elStyle += "border: "+(bordStyle?bordStyle:"1px solid gray")+"; ";
		    elStyle += (backStyle == null) ? 'background-color: white; ' : (backStyle.length?backStyle:'');
		    geomW -= 2; geomH -= 2;
		    //elStyle += "border: 1px solid gray; padding: 1px; ";
		    this.place.className += " ItemsTree";
		    if(elWr) this.place.className += " active";
		    var toInit = !this.place.children.length;
		    var formObj = toInit ? getTree() : this.place.children[0];
		    formObj.wdgLnk = this;
		    formObj.elWr = elWr;
		    if(toInit || this.attrsMdf['font'])	formObj.style.cssText = 'font: '+this.place.fontCfg+'; ';
		    //Events and the processings init
		    if(toInit)
			formObj.select = function(ipath) {
			    if(!ipath || !ipath.length) return;
			    attrs = new Object();
			    attrs.value = ipath;
			    attrs.event = 'ws_TreeChange';
			    setWAttrs(this.wdgLnk.addr, attrs);
			}
		    //Processing for fill and changes
		    if(toInit || this.attrsMdf['items']) formObj.setList(this.attrs['items']);
		    if(toInit || this.attrsMdf['value'] || this.attrsMdf['items']) formObj.selIt(this.attrs['value']);
		    if(toInit) this.place.appendChild(formObj);
		    break;
		case 9:	//Table
		    elStyle += "padding: 1px; ";
		    if(bordStyle) elStyle += "border: "+bordStyle+"; ";
		    if(backStyle != null) elStyle += backStyle;
		    geomW -= 4; geomH -= 4;
		    this.place.className += " Table";
		    if(elWr) this.place.className += " active";
		    var toInit = !this.place.children.length;
		    var formObj = toInit ? this.place.ownerDocument.createElement('table') : this.place.children[0];
		    formObj.wdgLnk = this;
		    formObj.elWr = elWr;
		    // Events and the processings init
		    if(toInit) {
			formObj.onclick = function( ) {
			    if(this.nodeName == "TABLE" || !(elTbl=this.offsetParent) || !elTbl.elWr) return true;
			    attrs = null;
			    if(elTbl.oSel == "row" && this.parentNode.rowIndex > 0) {
				elTbl.selIt(this.parentNode.rowIndex);
				attrs = new Object();
				attrs.value = elTbl.getVal(this.parentNode.rowIndex-1, elTbl.oKeyID);
				if(this.parentNode.cells[elTbl.oKeyID+1].outTp == "b")
				    attrs.value = (attrs.value == "true") ? 1 : 0;
			    }
			    else if(elTbl.oSel == "col" && this.cellIndex > 0) {
				elTbl.selIt(null, this.cellIndex);
				attrs = new Object();
				attrs.value = elTbl.getVal(elTbl.oKeyID, this.cellIndex-1);
				if(elTbl.tBodies[0].rows[elTbl.oKeyID].cells[this.cellIndex].outTp == "b")
				    attrs.value = (attrs.value == "true") ? 1 : 0;
			    }
			    else if(this.parentNode.rowIndex > 0 && this.cellIndex > 0) {
				elTbl.selIt(this.parentNode.rowIndex, this.cellIndex);
				attrs = new Object();
				if(elTbl.oSel == "cell") {
				    attrs.value = elTbl.getVal(this.parentNode.rowIndex-1, this.cellIndex-1);
				    if(this.outTp == "b") attrs.value = (attrs.value == "true") ? 1 : 0;
				} else attrs.value = (this.parentNode.rowIndex-1) + ":" + (this.cellIndex-1);
			    }
			    if(attrs) { attrs.event = 'ws_TableChangeSel'; setWAttrs(elTbl.wdgLnk.addr, attrs); }
			}
			formObj.ondblclick = function( ) {
			    if(this.nodeName == "TABLE" || !(elTbl=this.offsetParent) || !elTbl.elWr || !this.isEdit) return true;
			    if(this.isEnter) {
				this.innerHTML = this.svInnerHTML;
				this.isEnter = false; elTbl.edIt = null;
			    }
			    else {
				if(elTbl.edIt) { elTbl.edIt.innerHTML = elTbl.edIt.svInnerHTML; elTbl.edIt.isEnter = false; }
				this.isEnter = true; elTbl.edIt = this;
				this.svInnerHTML = this.innerHTML;

				if(this.outTp == "b") {
				    tVl = elTbl.getVal(this.parentNode.rowIndex-1,this.cellIndex-1);
				    this.innerHTML = "<input type='checkbox'/>";
				    this.firstChild.checked = tVl;
				    this.firstChild.onclick = function( ) {
					this.parentNode.offsetParent.setVal((this.checked?1:0),
					    this.parentNode.parentNode.rowIndex-1, this.parentNode.cellIndex-1);
				    }
				    this.firstChild.onkeyup = function(e) {
					if(e.keyCode == 13 || e.keyCode == 20) {
					    this.checked = !this.checked;
					    this.parentNode.offsetParent.setVal((this.checked?1:0), this.parentNode.parentNode.rowIndex-1, this.parentNode.cellIndex-1);
					}
					if(e.keyCode == 27) {
					    this.parentNode.isEnter = false; this.parentNode.offsetParent.edIt = null;
					    this.parentNode.innerHTML = this.parentNode.svInnerHTML;
					}
					return true;
				    }
				    this.firstChild.focus();
				}
				else if(this.outTp == "i" || this.outTp == "r" || this.outTp == "s") {
				    tVl = elTbl.getVal(this.parentNode.rowIndex-1, this.cellIndex-1);
				    this.innerHTML = (this.outTp == "s") ? "<textarea style='height: "+this.clientHeight+"px;'/>" : "<input/>";
				    this.firstChild.value = tVl;
				    this.firstChild.onkeyup = function(e) {
					e.stopImmediatePropagation();
					if(e.keyCode == 13 && (this.nodeName != "TEXTAREA" || e.ctrlKey))
					    this.parentNode.offsetParent.setVal(this.value, this.parentNode.parentNode.rowIndex-1, this.parentNode.cellIndex-1);
					if(e.keyCode == 27) {
					    this.parentNode.isEnter = false; this.parentNode.offsetParent.edIt = null;
					    this.parentNode.innerHTML = this.parentNode.svInnerHTML;
					}
					return true;
				    }
				    this.firstChild.onkeydown = function(e) { e.stopImmediatePropagation(); return true; }
				    this.firstChild.oncontextmenu = function(e) { e.stopImmediatePropagation(); return true; }
				    this.firstChild.focus();
				} else { this.isEnter = false; elTbl.edIt = null; }
			    }
			    //   Prevent for wrong selection
			    if(window.getSelection) window.getSelection().removeAllRanges();
			    else if(document.selection) document.selection.empty();
			    return false;
			}
			formObj.selIt = function(row, col) {
			    //Restore saved
			    if(this.svRow || this.svCol) {
				if(!this.svRow)
				    this.tHead.rows[0].cells[this.svCol].style.backgroundColor =
						this.tHead.rows[0].cells[this.svCol].svBackgroundColor;
				for(iR = (this.svRow?this.svRow-1:0); iR <= (this.svRow?Math.min(this.svRow-1,this.tBodies[0].rows.length-1):this.tBodies[0].rows.length-1); iR++)
				    for(iC = (this.svCol?this.svCol:0); iC <= (this.svCol?this.svCol:(this.tBodies[0].rows[iR].cells.length-1)); iC++)
					this.tBodies[0].rows[iR].cells[iC].style.backgroundColor =
						this.tBodies[0].rows[iR].cells[iC].svBackgroundColor;
			    }
			    //Set new
			    if(row || col) {
				if(!row) {
				    this.tHead.rows[0].cells[col].svBackgroundColor = this.tHead.rows[0].cells[col].style.backgroundColor;
				    this.tHead.rows[0].cells[col].style.backgroundColor = "LightBlue";
				}
				for(iR = (row?row-1:0); iR <= (row?row-1:this.tBodies[0].rows.length-1); iR++)
				    for(iC = (col?col:0); iC <= (col?col:this.tBodies[0].rows[iR].cells.length-1); iC++) {
					this.tBodies[0].rows[iR].cells[iC].svBackgroundColor = this.tBodies[0].rows[iR].cells[iC].style.backgroundColor;
					this.tBodies[0].rows[iR].cells[iC].style.backgroundColor = "LightBlue";
				    }
			    }
			    this.svRow = row; this.svCol = col;
			}
			formObj.getVal = function( row, col ) {
			    tit = this.tBodies[0].rows[row].cells[col+1];
			    return tit.origVl ? tit.origVl : tit.innerText;
			}
			formObj.setVal = function( val, row, col ) {
			    tit = this.tBodies[0].rows[row].cells[col+1];
			    if(tit.isEnter) {
				//tit.innerHTML = tit.svInnerHTML;
				attrs = new Object(); attrs.set = val; attrs.event = "ws_TableEdit_"+col+"_"+this.tBodies[0].rows[row].rowReal;
				setWAttrs(this.wdgLnk.addr, attrs);
				tit.isEnter = false; this.edIt = null;
			    }
			    tit.origVl = null;
			    switch(tit.outTp) {
				case 'b':
				    tit.childNodes[0].style.display = parseInt(val) ? "" : "none";
				    if(tit.isEdit) tit.origVl = parseInt(val);
				    break;
				case 'i': tit.innerText = parseInt(val);	break;
				case 'r': tit.innerText = parseFloat(parseFloat(val).toPrecision(6));	break;
				default:
				    if(val.length <= limTblItmCnt) tit.innerText = val;
				    else { tit.innerText = val.slice(0,limTblItmCnt) + "..."; if(tit.isEdit) tit.origVl = val; }
				    break;
			    }
			}
		    }
		    if(toInit || this.attrsMdf['font'])  formObj.style.cssText = 'font: '+this.place.fontCfg+'; ';
		    // Processing for fill and changes
		    if(this.attrs['items'] && (toInit || ((this.attrsMdf['items'] /*|| this.attrsMdf['value']*/ || this.attrsMdf['font']) && !formObj.edIt))) {
			hdrPresent = false, maxCols = 0, maxRows = 0;
			toReFit = this.attrsMdf['font'];
			var itemsO = (new DOMParser()).parseFromString(this.attrs['items'], "text/xml");
			if(!itemsO.children.length || (tX=itemsO.children[0]).nodeName != "tbl")	formObj.innerHTML = "";
			else {
			    if(toInit || !formObj.children.length) formObj.innerHTML = "<THEAD><TR/></THEAD><TBODY/>";
			    else if(!((wVl=tX.getAttribute("sortEn")) && parseInt(wVl))) formObj.sortCol = null;

			    rClr = null, rClrTxt = null, rFnt = null;
			    startRows = formObj.tBodies[0].rows.length;
			    startCols = startRows ? formObj.tBodies[0].rows[0].cells.length : 0;
			    sortCol = 0;
			    for(iR = 0, iRR = 0, iCh = 0; iCh < tX.children.length || iR < formObj.tBodies[0].rows.length; iCh++) {
				tR = (iCh < itemsO.children[0].children.length) ? itemsO.children[0].children[iCh] : null;
				isH = false, hit = null, tit = null;
				if(tR && !((isH=(tR.nodeName=="h")) || tR.nodeName == "r")) continue;
				if(!isH && iR >= formObj.tBodies[0].rows.length)
				    formObj.tBodies[0].appendChild(this.place.ownerDocument.createElement('tr'));
				if(!isH && tR) {
				    formObj.tBodies[0].rows[iR].rowReal = iR;
				    rClr = tR.getAttribute("color");
				    rClrTxt = tR.getAttribute("colorText");
				    rFnt = tR.getAttribute("font");
				}
				for(iC = 0, iCR = 0, iCh1 = 0; (tR && iCh1 < tR.children.length) || iC < formObj.tHead.children.length; ) {
				    tC = (tR && iCh1 < tR.children.length) ? tR.children[iCh1] : null;
				    if(iC >= formObj.tHead.rows[0].cells.length)
					formObj.tHead.rows[0].appendChild(this.place.ownerDocument.createElement('th'));
				    hit = formObj.tHead.rows[0].cells[iC];
				    hit.onclick = formObj.onclick;
				    if(isH) {	//Header process
					if(iC == 0) { hit.innerText = '*'; iC++; continue; }
					if(tC) {
					    if(hit.innerText != tC.textContent) toReFit = true;
					    hit.innerText = tC.textContent;
					    hit.widthSrc = ""
					    if(!(wVl=tC.getAttribute("width"))) ;
					    else if(wVl.indexOf("%") >= 0)	hit.widthSrc = parseInt(wVl) + "%";
					    else hit.widthSrc = (parseInt(wVl)*xSc) + "px";
					    hit.outEdit = parseInt(tC.getAttribute("edit"));
					    hit.outColor = tC.getAttribute("color");
					    hit.outColorText = tC.getAttribute("colorText");
					    hit.outFont = tC.getAttribute("font");
					    hit.outAlign = tC.getAttribute("align");
					    hit.style.display = (hit.widthSrc.length && !parseInt(hit.widthSrc)) ? "none" : "";
					    if((wVl=tC.getAttribute("sort")))	{ sortCol = iC+1; if(!parseInt(wVl)) sortCol *= -1; }
					} else hit.innerText = "";
				    }
				    else {	//Rows content process
					if(iC >= formObj.tBodies[0].rows[iR].cells.length)
					    formObj.tBodies[0].rows[iR].appendChild(this.place.ownerDocument.createElement(iC==0?'th':'td'));
					tit = formObj.tBodies[0].rows[iR].cells[iC];
					tit.onclick = formObj.onclick;
					if(iC == 0) { tit.innerText = iR+1; iC++; continue; }
					else tit.ondblclick = formObj.ondblclick;
					// Modify set
					if(hit.outEdit || (tC && (wVl=tC.getAttribute("edit")) && parseInt(wVl))) tit.isEdit = true;
					else tit.isEdit = false;
					// Value
					if(tC) {
					    tit.outTp = tC.nodeName;
					    tit.style.textAlign = (tit.outTp == "b" || tit.outTp == "i" || tit.outTp == "r") ? "center" : "";
					    if(tit.outTp == "b") tit.innerHTML = "<img src='/"+MOD_ID+"/img_button_ok' height='"+fntSz+"px'/> ";
					    formObj.setVal(tC.textContent, iR, iC-1);
					}
					// Visibility
					tit.style.display = (hit.widthSrc.length && !parseInt(hit.widthSrc)) ? "none" : "";
					// Back color
					if((tC && (wVl=tC.getAttribute("color"))) || (wVl=hit.outColor) || (wVl=rClr))
					    tit.style.backgroundColor = getColor(wVl);
					else tit.style.backgroundColor = null;
					// Text font and color
					if((tC && (wVl=tC.getAttribute("colorText"))) || (wVl=hit.outColorText) || (wVl=rClrTxt))
					    tit.style.color = getColor(wVl);
					else tit.style.color = null;
					if((tC && (wVl=tC.getAttribute("font"))) || (wVl=hit.outFont) || (wVl=rFnt))
					    tit.style.font = getFontCond(wVl, Math.min(xSc,ySc));
					else tit.style.font = null;
					// Cell image
					if(tC && (wVl=tC.getAttribute("img")))
					    tit.innerHTML = "<img src='/"+MOD_ID+this.addr+"?com=res&val="+wVl+"'/> " + tit.innerText;
					// Alignment set
					if((tC && (wVl=tC.getAttribute("align"))) || (wVl=hit.outAlign)) tit.style.textAlign = wVl;
				    }
				    if(tC)	{ ++iCR; maxCols = Math.max(maxCols, iCR); }
				    iC++; iCh1++;
				}
				if(!isH) {
				    if(tR)	{ ++iRR; maxRows = Math.max(maxRows, iRR); }
				    iR++;
				} else hdrPresent = true;
			    }

			    if((!startRows && formObj.tBodies[0].rows.length) || (formObj.tBodies[0].rows.length && formObj.tBodies[0].rows[0].cells.length > startCols))
				toReFit = true;

			    // Generic properties set
			    formObj.oKeyID = (wVl=tX.getAttribute("keyID")) ? parseInt(wVl) : 0;
			    formObj.oSel = tX.getAttribute("sel");
			    if(formObj.oSel == "row" && formObj.oKeyID)		formObj.oKeyID = Math.min(formObj.oKeyID, maxCols-1);
			    else if(formObj.oSel == "col" && formObj.oKeyID)	formObj.oKeyID = Math.min(formObj.oKeyID, maxRows-1);

			    // Remove spare rows and columns; Headers visibility process
			    if(!(wVl=tX.getAttribute("hHdrVis")) || !wVl.length || parseInt(wVl))
				formObj.tHead.rows[0].style.display = ""
			    else {
				formObj.tHead.rows[0].style.display = "none";
				if(formObj.tBodies[0].rows.length)
				    for(iC = 0; iC < formObj.tBodies[0].rows[0].cells.length; iC++)
					formObj.tBodies[0].rows[0].cells[iC].widthSrc = formObj.tHead.rows[0].cells[iC].widthSrc;
			    }
			    //formObj.tHead.rows[0].style.display =
			    //	(!(wVl=tX.getAttribute("hHdrVis")) || !wVl.length || parseInt(wVl)) ? "" : "none";
			    while(formObj.tHead.rows[0].cells.length > (maxCols+1))
				formObj.tHead.rows[0].removeChild(formObj.tHead.rows[0].lastChild);
			    wVl = (wVl=tX.getAttribute("vHdrVis")) ? parseInt(wVl) : false;
			    if(formObj.tHead.rows[0].cells.length)
				formObj.tHead.rows[0].cells[0].style.display = wVl ? "" : "none";
			    for(iR = 0; iR < formObj.tBodies[0].rows.length; iR++) {
				tR = formObj.tBodies[0].rows[iR];
				if(tR.cells.length) tR.cells[0].style.display = wVl ? "" : "none";
				while(tR.cells.length > (maxCols+1)) tR.removeChild(tR.lastChild);
			    }
			    while(formObj.tBodies[0].rows.length > maxRows)
				formObj.tBodies[0].removeChild(formObj.tBodies[0].lastChild);

			    //Specifying the columns mode fitting - the table layout mode
			    if(toReFit) {
				hdrRow = formObj.tHead.rows[0];
				if(hdrRow.style.display == "none") hdrRow = formObj.tBodies[0].rows[0];
				for(iC = 0; hdrRow && iC < hdrRow.cells.length; iC++)
				    hdrRow.cells[iC].style.width = hdrRow.cells[iC].widthSrc ? hdrRow.cells[iC].widthSrc : null;

				formObj.style.width = ""; formObj.style.tableLayout = "auto";
				if((wVl=tX.getAttribute("colsWdthFit")) && parseInt(wVl)) formObj.style.width = "100%";
				else { this.toReFit = toReFit; this.perUpdtEn(true); }
			    }

			    //Sorting
			    if(((wVl=tX.getAttribute("sortEn")) && parseInt(wVl)) || sortCol) {
				formObj.sort = function(col, force) {
				    if(force) this.sortCol = -col;
				    col = Math.abs(col);
				    isDesc = (Math.abs(this.sortCol) == col && this.sortCol && this.sortCol > 0);
				    prcArr = Array.prototype.slice.call(this.tBodies[0].rows,0).sort(
					    function(a,b) { return (isDesc?1:-1)*a.cells[col-1].textContent.localeCompare(b.cells[col-1].textContent); }
					);
				    for(iR = 0; iR < prcArr.length; ++iR) {
					if(this.svRow && this.svRow == parseInt(prcArr[iR].cells[0].textContent)) {
					    this.svRow = iR+1;
					    prcArr[iR].cells[0].style.backgroundColor = prcArr[iR].cells[0].svBackgroundColor ? prcArr[iR].cells[0].svBackgroundColor : null;
					}
					prcArr[iR].cells[0].textContent = iR+1;
					this.tBodies[0].appendChild(prcArr[iR]);
				    }
				    this.tHead.rows[0].cells[col-1].innerHTML = this.tHead.rows[0].cells[col-1].childNodes[0].textContent + "<span>&nbsp;"+(isDesc?"▲":"▼")+"</span>";
				    if(this.sortCol && Math.abs(this.sortCol) != col)
					this.tHead.rows[0].cells[Math.abs(this.sortCol)-1].innerText = this.tHead.rows[0].cells[Math.abs(this.sortCol)-1].childNodes[0].textContent;
				    this.sortCol = (Math.abs(this.sortCol) == col) ? -this.sortCol : col;
				}
				for(iC = 1; iC < formObj.tHead.rows[0].cells.length; iC++) {
				    formObj.tHead.rows[0].cells[iC].onclick = function() { this.offsetParent.sort(this.cellIndex+1); }
				    if(!sortCol && formObj.tHead.rows[0].cells[iC].style.display != "none") sortCol = iC+1;
				}

				if(formObj.sortCol) formObj.sort(formObj.sortCol, true);
				else if(sortCol) formObj.sort(sortCol, true);
			    }
			}
			delete itemsO;
			//!!!! Preventing of storing big table sources
			if(this.attrs['items'].length > 10000) { delete this.attrs['items']; this.attrs['items'] = null; }
		    }
		    if(toInit) this.place.appendChild(formObj);

		    // Set the value
		    if((toInit || this.attrsMdf['value'] || this.attrsMdf['items']) && formObj.innerHTML.length) {
			val = this.attrs['value'];
			if(formObj.oSel == "row" || formObj.oSel == "col" || formObj.oSel == "cell") {
			    findOK = false;
			    for(iR = ((formObj.oSel=="col")?formObj.oKeyID:0);
				    !findOK && iR <= ((formObj.oSel=="col")?formObj.oKeyID:formObj.tBodies[0].rows.length-1); iR++)
				for(iC = ((formObj.oSel=="row")?formObj.oKeyID+1:1);
					!findOK && iC <= ((formObj.oSel=="row")?formObj.oKeyID+1:(formObj.tBodies[0].rows[iR].cells.length-1)); iC++) {
				    cO = formObj.tBodies[0].rows[iR].cells[iC];
				    valc = formObj.getVal(iR, iC-1); if(cO.outTp == "b") valc = (valc=="true") ? 1 : 0;
				    if((findOK=(valc==val))) formObj.selIt(((formObj.oSel=="col")?null:iR+1), ((formObj.oSel=="row")?null:iC));
				}
			    if(!findOK) formObj.selIt();
			} else if((sepPos=val.indexOf(":")) > 0) formObj.selIt(parseInt(val.slice(0,sepPos))+1, parseInt(val.slice(sepPos+1))+1);
		    }
		    break;
	    }
	    this.place.isSpecFocus = true;
	    if(elWr != this.place.elWr || this.attrsMdf['geomZ'])
		if(elWr) formObj.setAttribute('tabIndex', getTabIndex(this,parseInt(this.attrs['geomZ']))); else formObj.removeAttribute('tabIndex');
	    if(elWr != this.place.elWr) formObj.onfocus = !elWr ? null : function(e) { setFocus(this.parentNode.wdgLnk.addr); }
	}
	else if(this.attrs['root'] == 'Diagram') {
	    elStyle += 'border-style: solid; border-width: '+this.attrs['bordWidth']+'px; ';
	    var dgrObj = this.place.childNodes[0];
	    if(!dgrObj)	{
		this.place.appendChild((dgrObj=this.place.ownerDocument.createElement('img')));
		dgrObj.wdgLnk = this;
	    }
	    dgrObj.isActive = elWr;
	    dgrObj.onclick = function(e) {
		if(!this.isActive) return false;
		if(!e) e = window.event;
		servSet(this.wdgLnk.addr,'com=obj&sub=point&x='+(e.offsetX?e.offsetX:(e.clientX-posGetX(this)))+
							  '&y='+(e.offsetY?e.offsetY:(e.clientY-posGetY(this)))+
							  '&key='+evMouseGet(e),'');
		return false;
	    }
	    dgrObj.isLoad = false;
	    dgrObj.onload = function( )	{ this.isLoad = true; }
	    dgrObj.src = '/'+MOD_ID+this.addr+'?com=obj&tm='+tmCnt+'&xSc='+xSc.toFixed(3)+'&ySc='+ySc.toFixed(3);
	    //Disable drag mostly for FireFox
	    dgrObj.onmousedown = function(e) { e = e?e:window.event; if(e.preventDefault) e.preventDefault(); }
	    this.perUpdtEn(this.isEnabled() && parseInt(this.attrs['trcPer']));
	}
	else if(this.attrs['root'] == 'Protocol') {
	    if(!this.attrs['backColor'] || !getColor(this.attrs['backColor'],true)) elStyle += 'background-color: white; ';
	    elStyle += 'border: 1px solid black; overflow: auto; padding: 2px; text-align: left; ';
	    geomW -= 6; geomH -= 6;

	    this.wFont = getFont(this.attrs['font'], Math.min(xSc,ySc));

	    if(!this.place.firstChild) {
		this.place.appendChild(document.createElement('table'));
		this.place.firstChild.setAttribute('width','100%');
		this.place.firstChild.wdgLnk = this;
		this.place.firstChild.isActive = elWr;
		//this.place.firstChild.onclick = function(e) { return false; }
		this.place.firstChild.className = 'prot';
		this.loadData = function( ) {
		    if(!this.tmPrev) this.tmPrev = 0;
		    if(!this.tmGrndPrev) this.tmGrndPrev = 0;

		    var tblB = this.place.firstChild;

		    //Get archive parameters
		    var tTime = parseInt(this.attrs['time']);
		    var tTimeCurent = false;
		    if(!tTime) {
			tTimeCurent = true;
			tTime = (new Date()).getTime()/1000;
		    }
		    var srcTime = tTime;
		    var tTimeGrnd = tTime - parseInt(this.attrs['tSize']);

		    if(this.curCols != this.attrs['col'] || this.curArch != this.attrs['arch'] || this.curTmpl != this.attrs['tmpl'] ||
			this.curLev != this.attrs['lev'] || this.headVis != this.attrs['headVis'])
		    {
			this.arhBeg = this.arhEnd = 0;
			this.messList = new Array();

			this['col_pos'] = this['col_tm'] = this['col_utm'] = this['col_lev'] = this['col_cat'] = this['col_mess'] = -1;
			this.curCols = this.attrs['col'];
			this.curArch = this.attrs['arch'];
			this.curTmpl = this.attrs['tmpl'];
			this.curLev = this.attrs['lev'];
			this.headVis = this.attrs['headVis'];
			while(tblB.childNodes.length) tblB.removeChild(tblB.lastChild);
			var rowEl = document.createElement('tr');
			tblB.appendChild(rowEl);
			var colCfg = '';
			var clm = this.curCols.split(';');
			for(var c_off = 0; c_off < clm.length; c_off++) {
			    if(!parseInt(this.attrs['headVis'])) colCfg += "<th style='display: none' />";
			    else colCfg += "<th ind='"+clm[c_off]+"' "+
						"style='"+this.wFont+"'>"+((clm[c_off]=='pos') ? '#' :
						    (clm[c_off]=='tm') ? '###Date and time###' :
						    (clm[c_off]=='utm') ? '###mcsec###' :
						    (clm[c_off]=='lev') ? '###Level###' :
						    (clm[c_off]=='cat') ? '###Category###' :
						    (clm[c_off]=='mess') ? '###Message###' : '')+"</th>";
			    this['col_'+clm[c_off]] = c_off;
			}
			rowEl.innerHTML = colCfg;
		    }

		    if(parseInt(this.curLev) < 0) this.messList = new Array();
		    else {
			if(!this.arhBeg || !this.arhEnd || !tTime || tTime > this.arhEnd || tTimeCurent) {
			    var rez = servSet('/Archive/%2fserv%2fmess','com=com',"<info arch='"+this.curArch+"'/>");
			    if(!rez || parseInt(rez.getAttribute('rez')) != 0)	this.arhBeg = this.arhEnd = 0;
			    else {
				this.arhBeg = parseInt(rez.getAttribute('beg'));
				this.arhEnd = parseInt(rez.getAttribute('end'));
				if(!tTime) { tTime = this.arhEnd; tTimeGrnd += tTime; }
			    }
			}
			if(!this.arhBeg || !this.arhEnd) return;

			//Correct request to archive border
			tTime = tTimeCurent ? this.arhEnd : Math.min(tTime,this.arhEnd);
			tTimeGrnd = Math.max(tTimeGrnd, this.arhBeg);
		    }

		    //Clear data at time error
		    var valEnd = 0; var valBeg = 0;
		    while(this.messList.length && (valEnd=this.messList[0][0]) > tTime) this.messList.shift();
		    while(this.messList.length && (valBeg=this.messList[this.messList.length-1][0]) < tTimeGrnd) this.messList.pop();

		    if(!this.messList.length) valEnd = valBeg = 0;

		    if(tTime < tTimeGrnd || (tTime < valEnd && tTimeGrnd > valBeg)) {
			this.messList = new Array();
			while(tblB.childNodes.length > 1) tblB.removeChild(tblB.lastChild);
			valEnd = valBeg = 0;
			return;
		    }

		    //Correct request to present data
		    var toUp = false, isDtChang = false;
		    if(srcTime > this.tmPrev) { if(valEnd) tTimeGrnd = valEnd; toUp = true; }
		    else if((srcTime-parseInt(this.attrs['tSize'])) < this.tmGrndPrev) { if(valBeg) tTime = valBeg-1; }
		    else return;
		    this.tmPrev = srcTime;
		    this.tmGrndPrev = srcTime-parseInt(this.attrs['tSize']);

		    var rez = servSet('/Archive/%2fserv%2fmess','com=com',
			"<get arch='"+this.curArch+"' tm='"+tTime+"' tm_grnd='"+tTimeGrnd+"' cat='"+this.curTmpl+"' lev='"+this.curLev+"' />");
		    if(!rez || parseInt(rez.getAttribute('rez')) != 0) return;

		    if(toUp)
			for(var i_req = 0; i_req < rez.childNodes.length; i_req++) {
			    var rcd = rez.childNodes[i_req];
			    var mess = new Array(parseInt(rcd.getAttribute('time')), parseInt(rcd.getAttribute('utime')),
						    Math.abs(parseInt(rcd.getAttribute('lev'))), rcd.getAttribute('cat'), rcd.textContent);

			    // Check for dublicates
			    var isDbl = false;
			    for(var iP = 0; !isDbl && iP < this.messList.length; iP++) {
				if(mess[0] > this.messList[0][0] && iP) break;
				if(this.messList[iP][1] == mess[1] && this.messList[iP][2] == mess[2] &&
				    this.messList[iP][3] == mess[3] && this.messList[iP][4] == mess[4]) isDbl = true;
			    }
			    if(isDbl) continue;

			    // Insert new row
			    this.messList.unshift(mess);
			    isDtChang = true;
			}
		    else
			for(var i_req = rez.childNodes.length-1; i_req >= 0; i_req--) {
			    var rcd = rez.childNodes[i_req];
			    var mess = new Array(parseInt(rcd.getAttribute('time')), parseInt(rcd.getAttribute('utime')),
						    Math.abs(parseInt(rcd.getAttribute('lev'))), rcd.getAttribute('cat'), rcd.textContent);

			    // Check for dublicates
			    var isDbl = false;
			    for(var iP = this.messList.length-1; !isDbl && iP >= 0; iP--) {
				if(mess[0] < this.messList[this.messList.length-1][0] && iP < (this.messList.length-1)) break;
				if(this.messList[iP][1] == mess[1] && this.messList[iP][2] == mess[2] &&
				    this.messList[iP][3] == mess[3] && this.messList[iP][4] == mess[4]) isDbl = true;
			    }
			    if(isDbl) continue;

			    // Insert new row
			    this.messList.push(mess);
			    isDtChang = true;
			}

		    if((tblB.childNodes.length-1) == this.messList.length && !isDtChang) return;

		    //Sort data
		    var sortIts = new Array();
		    switch(parseInt(this.attrs['viewOrd'])&0x3) {
			case 0:
			    for(var i_m = 0; i_m < this.messList.length; i_m++)
				sortIts.push(this.messList[i_m][0]+' '+this.messList[i_m][1]+':'+i_m);
			    break;
			case 1:
			    for(var i_m = 0; i_m < this.messList.length; i_m++)
				sortIts.push(this.messList[i_m][2]+':'+i_m);
			    break;
			case 2:
			    for(var i_m = 0; i_m < this.messList.length; i_m++)
				sortIts.push(this.messList[i_m][3]+':'+i_m);
			    break;
			case 3:
			    for(var i_m = 0; i_m < this.messList.length; i_m++)
				sortIts.push_back(this.messList[i_m][4]+':'+i_m);
			    break;
		    }

		    sortIts.sort();
		    if(parseInt(this.attrs['viewOrd'])&0x4) sortIts.reverse();

		    //Write to the table
		    for(var i_m = 0; i_m < sortIts.length; i_m++) {
			var rowEl = (i_m>=(tblB.childNodes.length-1)) ? document.createElement('tr') : tblB.childNodes[i_m+1];
			var elPos = parseInt(sortIts[i_m].slice(sortIts[i_m].lastIndexOf(':')+1));

			var rowFnt = this.wFont;
			var rowColor = '';

			// Check properties
			for(var i_it = 0, lst_lev = -1; i_it < parseInt(this.attrs['itProp']); i_it++) {
			    var prpLev = parseInt(this.attrs['it'+i_it+'lev']);
			    if(this.messList[elPos][2] >= prpLev && prpLev > lst_lev &&
				    chkPattern(this.messList[elPos][3],this.attrs['it'+i_it+'tmpl']))
			    {
				var tRowFnt = getFont(this.attrs['it'+i_it+'fnt'], Math.min(xSc,ySc));
				if(tRowFnt.length) rowFnt = tRowFnt;
				rowColor = 'background-color: '+getColor(this.attrs['it'+i_it+'color'])+'; ';
				if(this.messList[elPos][2] == parseInt(this.attrs['it'+i_it+'lev'])) break;
				lst_lev = prpLev;
			    }
			}
			//if( rowColor.length ) fclr = ((0.3*clr.red()+0.59*clr.green()+0.11*clr.blue()) > 128) ? Qt::black : Qt::white;
			rowEl.style.cssText = rowFnt+rowColor;

			for(var i_cel = 0; i_cel < tblB.childNodes[0].childNodes.length; i_cel++) {
			    var celEl = (i_cel>=rowEl.childNodes.length) ? document.createElement('td') : rowEl.childNodes[i_cel];
			    if(this.col_pos == i_cel) { celEl.textContent = i_m; celEl.style.cssText += ' text-align: center; '; }
			    else if(this.col_tm == i_cel) {
				var dt = new Date(this.messList[elPos][0]*1000);
				celEl.textContent = dt.getDate()+'.'+(dt.getMonth()+1)+'.'+dt.getFullYear()+' '+dt.getHours()+':'+
				    ((dt.getMinutes()<10)?('0'+dt.getMinutes()):dt.getMinutes())+':'+((dt.getSeconds()<10)?('0'+dt.getSeconds()):dt.getSeconds());
			    }
			    else if(this.col_utm == i_cel)	celEl.textContent = this.messList[elPos][1];
			    else if(this.col_lev == i_cel)	celEl.textContent = this.messList[elPos][2];
			    else if(this.col_cat == i_cel)	celEl.textContent = this.messList[elPos][3];
			    else if(this.col_mess == i_cel)	celEl.textContent = this.messList[elPos][4];
			    if(i_cel >= rowEl.childNodes.length) rowEl.appendChild(celEl);
			}
			if(i_m >= (tblB.childNodes.length-1)) tblB.appendChild(rowEl);
		    }
		    while((tblB.childNodes.length-1) > sortIts.length) tblB.removeChild(tblB.lastChild);
		}
	    }

	    if(this.isEnabled() && parseInt(this.attrs['trcPer'])) this.perUpdtEn(true);
	    else { this.perUpdtEn(false); this.loadData(); }
	}
	else if(this.attrs['root'] == 'Document') {
	    elStyle += 'background-color: white; ';

	    if(this.attrsMdf["style"] || this.attrsMdf["font"] ||
		(this.attrsMdf["doc"] && this.attrs["doc"].length) || (this.attrsMdf["tmpl"] && !this.attrs["doc"].length))
	    {
		this.wFont = getFont(this.attrs['font'], Math.min(xSc,ySc),1);

		var ifrmObj = this.place.childNodes[0];
		try {
		    if(ifrmObj && (!ifrmObj.contentDocument || document.URL != ifrmObj.contentDocument.URL))
		    { this.place.removeChild(ifrmObj); ifrmObj = null; }
		}catch(e) { this.place.removeChild(ifrmObj); ifrmObj = null; }
		if(!ifrmObj) {
		    ifrmObj = this.place.ownerDocument.createElement('iframe');
		    this.place.appendChild(ifrmObj);
		}

		ifrmObj.style.cssText = 'width: '+geomW+'px; height: '+geomH+'px; border-style: ridge; border-width: 0px; padding: 0px;';
		//ifrmObj.style.cssText = 'width: '+(geomW-14)+'px; height: '+(geomH-14)+'px; border-style: ridge; border-width: 2px; padding: 5px;';
		this.perUpdtEn(true);
	    }
	}
	elStyle += 'width: ' + geomW + 'px; height: ' + geomH + 'px; z-index: ' + this.attrs['geomZ'] + '; margin: ' + elMargin + 'px; ';
	//Scroll for the included containers correct
	if(this.inclOpen) {
	    var geomWpar = parseFloat(this.attrs['geomW'])*this.xScale(true);
	    var geomHpar = parseFloat(this.attrs['geomH'])*this.yScale(true);
	    var geomWch = parseFloat(this.pages[this.inclOpen].attrs['geomW'])*this.pages[this.inclOpen].xScale(true);
	    var geomHch = parseFloat(this.pages[this.inclOpen].attrs['geomH'])*this.pages[this.inclOpen].yScale(true);
	    elStyle += "overflow: "+((geomWch > geomWpar || geomHch > geomHpar) ? 'auto' : 'hidden')+"; ";
	}
	else if(isPrim) {
	    // Border processing in generic
	    if(this.attrs['bordColor']) elStyle += 'border-color: '+getColor(this.attrs['bordColor'])+'; ';
	    switch(parseInt(this.attrs['bordStyle'])) {
		case 1: elStyle += 'border-style: dotted; '; break;
		case 2: elStyle += 'border-style: dashed; '; break;
		case 3: elStyle += 'border-style: solid; ';  break;
		case 4: elStyle += 'border-style: double; '; break;
		case 5: elStyle += 'border-style: groove; '; break;
		case 6: elStyle += 'border-style: ridge; ';  break;
		case 7: elStyle += 'border-style: inset; ';  break;
		case 8: elStyle += 'border-style: outset; '; break;
	    }
	    // Background processing in generic
	    backStyle = "";
	    if(this.attrs['backColor'] && (backOp=getColor(this.attrs['backColor'],true))) {
		if(backOp == 1) {
		    backStyle += 'background-color: '+getColor(this.attrs['backColor'])+'; ';
		    if(this.placeTr) this.placeTr.style.cssText = elStyle;
		}
		else {
		    if(!this.placeTr) {
			this.placeTr = this.place.ownerDocument.createElement('div');
			this.place.parentNode.appendChild(this.placeTr);
		    }
		    this.placeTr.style.cssText = elStyle + 'background-color: '+getColor(this.attrs['backColor'])+'; opacity: '+backOp+"; z-index: "+(parseInt(this.attrs['geomZ'])-1)+"; ";
		}
	    }
	    if(this.attrs['backImg'])	backStyle += 'background-image: url(\'/'+MOD_ID+this.addr+'?com=res&val='+this.attrs['backImg']+'\'); ';
	    elStyle += backStyle;
	    if(this == masterPage) document.body.style.cssText = parseInt(this.attrs['bordWidth']) ? '' : backStyle;
	}

	if(elStyle != this.place.style.cssText)	this.place.style.cssText = elStyle;

	//Generic events process
	if(elWr) {
	    this.mousedown[this.mousedown.length] = function(e,el) {
		setWAttrs(el.wdgLnk.addr,'event','key_mousePres'+evMouseGet(e));
		setFocus(el.wdgLnk.addr);
	    }
	    this.mouseup[this.mouseup.length] = function(e,el) {
		setWAttrs(el.wdgLnk.addr,'event','key_mouseRels'+evMouseGet(e));
		if(!el.isSpecFocus) el.focus();
	    }
	}
	if(!this.place.isSpecFocus) {
	    if(elWr != this.place.elWr || this.attrsMdf['geomZ'])
		if(elWr) this.place.setAttribute('tabIndex', getTabIndex(this,parseInt(this.attrs['geomZ']))); else this.place.removeAttribute('tabIndex');
	    if(elWr != this.place.elWr) this.place.onfocus = !elWr ? null : function(e) { setFocus(this.wdgLnk.addr); }
	}
	if(elWr != this.place.elWr) {
	    this.place.ondblclick = !elWr ? null : function(e) { setWAttrs(this.wdgLnk.addr,'event','key_mouseDblClick'); return false; }
	    this.place.onkeydown = !elWr ? null : function(e) {
		e = e ? e : window.event;
		setWAttrs(this.wdgLnk.addr,'event','key_pres'+(e.ctrlKey?"Ctrl":"")+(e.altKey?"Alt":"")+(e.shiftKey?"Shift":"")+evKeyGet(e));
	    }
	    this.place.onkeyup = !elWr ? null : function(e) {
		e = e ? e : window.event;
		setWAttrs(this.wdgLnk.addr,'event','key_rels'+(e.ctrlKey?"Ctrl":"")+(e.altKey?"Alt":"")+(e.shiftKey?"Shift":"")+evKeyGet(e));
	    }
	}

	//Context menu setup
	if(elWr && this.attrs['contextMenu'].length) {
	    var ctxEv = function(e,el) {
		var cel = el ? el : this;
		if(!e) e = window.event;
		if((isKonq || isOpera) && evMouseGet(e) != 'Right') return true;
		var popUpMenu = getPopup();
		var optEl = '';
		var cntxEls = cel.wdgLnk.attrs['contextMenu'].split('\n');
		for(var i_ce = 0; i_ce < cntxEls.length; i_ce++)
		    optEl += "<option sign='"+cntxEls[i_ce].split(':')[1]+"'>"+cntxEls[i_ce].split(':')[0]+"</option>";
		popUpMenu.childNodes[0].innerHTML = optEl;
		if(popUpMenu.childNodes[0].childNodes.length) {
		    popUpMenu.childNodes[0].wdgLnk = cel.wdgLnk;
		    popUpMenu.childNodes[0].size = Math.max(3,popUpMenu.childNodes[0].childNodes.length);
		    popUpMenu.style.cssText = 'visibility: visible; left: '+(e.clientX+window.pageXOffset)+'px; top: '+(e.clientY+window.pageYOffset)+'px;';
		    popUpMenu.childNodes[0].selectedIndex = -1;
		    popUpMenu.childNodes[0].onclick = function( ) {
			this.parentNode.style.cssText = 'visibility: hidden; left: -200px; top: -200px;';
			if(this.selectedIndex < 0) return false;
			setWAttrs(this.wdgLnk.addr,'event','usr_'+this.options[this.selectedIndex].getAttribute('sign'));
			return false;
		    }
		}
		return false;
	    }
	    if(isKonq || isOpera) this.mousedown[this.mousedown.length] = ctxEv;
	    else this.place.oncontextmenu = ctxEv;
	}
	else if(elWr) this.place.oncontextmenu = function(e) { return false; };
	else this.place.oncontextmenu = null;

	//Common mouse events processing
	if(this.mousedown.length)
	    this.place.onmousedown = function(e) {
		if(!e) e = window.event;
		if(e.elPrc) return true;
		e.elPrc = true;
		for(var iOn = 0; iOn < this.wdgLnk.mousedown.length; iOn++)
		    this.wdgLnk.mousedown[iOn](e, this);
		return true;
	    }
	else delete this.mousedown;
	if(this.mouseup.length)
	    this.place.onmouseup = function(e) {
		if(!e) e = window.event;
		if(e.elPrc) return true;
		e.elPrc = true;
		for(var iOn = 0; iOn < this.wdgLnk.mouseup.length; iOn++)
		    this.wdgLnk.mouseup[iOn](e,this);
		return true;
	    }
	else delete this.mouseup;

	this.place.elWr = elWr;
    }
    if(margBrdUpd || this.attrsMdf["geomXsc"] || this.attrsMdf["geomYsc"]) for(var i in this.wdgs) this.wdgs[i].makeEl();
    if(this.attrs['tipTool'] && this.attrs['tipTool'].length) this.place.setAttribute('title', this.attrs['tipTool']);
    else this.place.removeAttribute('title');
    this.place.onmouseover = function(e) {
	if(this.wdgLnk.attrs['tipStatus']) {
	    setStatus(this.wdgLnk.attrs['tipStatus'], 10000);
	    e.stopPropagation();
	}
    };

    //Checking for deletion the child widgets
    if(FullTree && pgBr)
	for(var i in this.wdgs) {
	    var j;
	    for(j = 0; j < pgBr.childNodes.length; j++)
		if(pgBr.childNodes[j].nodeName == 'w' && pgBr.childNodes[j].getAttribute('id') == i)
		    break;
	    if(j >= pgBr.childNodes.length) {
		this.wdgs[i].place.parentNode.removeChild(this.wdgs[i].place);
		delete this.wdgs[i];
	    }
	}

    //Child widgets process
    if(pgBr && !inclPg && parseInt(this.attrs['perm'])&SEC_RD) {
	for(var j = 0; j < pgBr.childNodes.length; j++) {
	    if(pgBr.childNodes[j].nodeName != 'w') continue;
	    var chEl = pgBr.childNodes[j].getAttribute('id');
	    if(this.wdgs[chEl]) this.wdgs[chEl].makeEl(pgBr.childNodes[j], false, full, FullTree);
	    else {
		var wdgO = new pwDescr(this.addr+'/wdg_'+chEl, false, this);
		wdgO.wPos = j;
		wdgO.place = this.place.ownerDocument.createElement('div');
		this.place.appendChild(wdgO.place);
		this.wdgs[chEl] = wdgO;
		wdgO.makeEl(full?pgBr.childNodes[j]:servGet(wdgO.addr,'com=attrsBr'), false, full, FullTree);
		//wdgO.makeEl(pgBr.childNodes[j]);		//!!!! Need full
	    }
	}
    }
}

function pwClean( complete )
{
    if(this.pg)	{
	ntfReg(-1, '', this.addr);
	for(iNtf = 0; iNtf < 8; iNtf++)
	    if(this.attrs["notifyVis"+MOD_ID+iNtf]) delete this.attrs["notifyVis"+MOD_ID+iNtf];
	    else if(this.attrs["notify"+iNtf]) delete this.attrs["notify"+iNtf];
    }

    for(var i in this.pages) this.pages[i].pwClean(complete);
    for(var i in this.wdgs) this.wdgs[i].pwClean(complete);

    if(complete) {
	this.perUpdtEn(false);		//Periodic update disable
	this.pages = new Object()	//Pages recursively clean;
	this.wdgs = new Object();	//Widgets recursively clean
    }
}

function perUpdtEn( en )
{
    if(this.attrs['root'] == 'Diagram' || this.attrs['root'] == 'Protocol') {
	if(en && this.isEnabled() && !perUpdtWdgs[this.addr] && parseInt(this.attrs['trcPer']))	perUpdtWdgs[this.addr] = this;
	if(!en && perUpdtWdgs[this.addr]) delete perUpdtWdgs[this.addr];
    }
    else if(this.attrs['root'] == 'Document' || this.attrs['root'] == 'FormEl' || this.attrs['root'] == 'Media') {
	if(en) perUpdtWdgs[this.addr] = this;
	else delete perUpdtWdgs[this.addr];
    }
    for(var i in this.wdgs) this.wdgs[i].perUpdtEn(en);	//!!!! To restore updating of elements like to Document after returning from the cache
}

function perUpdt( )
{
    if(this.attrs['root'] == 'FormEl') {
	if(parseInt(this.attrs['elType']) == 9 && this.toReFit) {	//Table type post adjust
	    if(!(tblWdth=this.place.clientWidth)) return;		//Waiting the table drawing
	    var tblWdthSum = 0;
	    hdrRow = this.place.childNodes[0].tHead.rows[0];
	    if(hdrRow.style.display == "none") hdrRow = this.place.childNodes[0].tBodies[0].rows[0];
	    for(iC = 0; iC < hdrRow.cells.length; iC++) {
		if(hdrRow.cells[iC].style.display == "none")	continue;
		if(!hdrRow.cells[iC].style.width.length) {
		    tblWdthSum += hdrRow.cells[iC].clientWidth;
		    hdrRow.cells[iC].style.width = hdrRow.cells[iC].clientWidth+"px";
		}
		else if(hdrRow.cells[iC].style.width.indexOf("%") >= 0) {
		    hdrRow.cells[iC].style.width = (parseInt(hdrRow.cells[iC].style.width)*tblWdth/100) + "px";
		    tblWdthSum += parseInt(hdrRow.cells[iC].style.width);
		}
		else tblWdthSum += parseInt(hdrRow.cells[iC].style.width);
	    }
	    this.place.childNodes[0].style.width = tblWdthSum+"px";
	    this.place.childNodes[0].style.tableLayout = "fixed";
	    this.toReFit = false;
	    this.perUpdtEn(false);
	}
	else if(this.place.childNodes.length && this.place.childNodes[0].tmClearEdit && (this.place.childNodes[0].tmClearEdit-=prcTm) <= 0)
	    this.place.childNodes[0].chEscape();
    }
    else if(this.attrs['root'] == 'Diagram' && (this.updCntr-=prcTm) <= 0) {
	this.updCntr = parseInt(this.attrs['trcPer']);
	var dgrObj = this.place.childNodes[0];
	if(!dgrObj.stLoadTm) dgrObj.stLoadTm = (new Date()).getTime();
	if(dgrObj && (dgrObj.isLoad || ((new Date()).getTime()-dgrObj.stLoadTm) > this.updCntr*3000)) {
	    dgrObj.isLoad = false;
	    dgrObj.src = '/'+MOD_ID+this.addr+'?com=obj&tm='+tmCnt+'&xSc='+this.xScale(true).toFixed(3)+'&ySc='+this.yScale(true).toFixed(3);
	    dgrObj.stLoadTm = (new Date()).getTime();
	}
    }
    else if(this.attrs['root'] == 'Media' && (this.updCntr-=prcTm) <= 0) {
	this.updCntr = 10;
	var videoObj = this.place.childNodes[0];

	if(videoObj.currentTime) setWAttrs(this.addr,'seek',videoObj.currentTime.toFixed(1));
    }
    else if(this.attrs['root'] == 'Protocol' && (this.updCntr-=prcTm) <= 0) {
	this.updCntr = parseInt(this.attrs['trcPer']);
	this.loadData();
    }
    else if(this.attrs['root'] == 'Document') {
	var frDoc = this.place.childNodes[0].contentDocument || this.place.childNodes[0].contentWindow || this.place.childNodes[0].document;
	if(frDoc) {	//Sometimes can be null
	    frDoc.open();
	    frDoc.write("<html><head>\n"+
		"<style type='text/css'>\n"+
		" * { "+this.wFont+" }\n"+
		" big { font-size: 120%; }\n"+
		" small { font-size: 90%; }\n"+
		" h1 { font-size: 200%; }\n"+
		" h2 { font-size: 150%; }\n"+
		" h3 { font-size: 120%; }\n"+
		" h4 { font-size: 105%; }\n"+
		" h5 { font-size: 95%; }\n"+
		" h6 { font-size: 70%; }\n"+
		" u,b,i { font-size : inherit; }\n"+
		" sup,sub { font-size: 80%; }\n"+
		" th { font-weight: bold; }\n"+
		this.attrs['style']+"</style>"+
		"</head>"+(this.attrs['doc']?this.attrs['doc']:this.attrs['tmpl'])+"</html>");
	    frDoc.close();
	    frDoc.body.wdgLnk = this;
	    frDoc.body.isActive = (parseInt(this.attrs['active']) && parseInt(this.attrs['perm'])&SEC_WR);
	    frDoc.body.onclick = function(e) {
		if(this.isActive) setFocus(this.wdgLnk.addr);
		return true;
	    }
	}
	this.perUpdtEn(false);
    }
}

function xScale( full )
{
    var rez = parseFloat(this.attrs['geomXsc'])
    if(!full) return rez;
    if(!this.pg) return rez*this.parent.xScale(full);
    return rez * wx_scale;
}

function yScale( full )
{
    var rez = parseFloat(this.attrs['geomYsc'])
    if(!full) return rez;
    if(!this.pg) return rez*this.parent.yScale(full);
    return rez * wy_scale;
}

function isEnabled( )
{
    var rez = parseInt(this.attrs['en']);
    if(!rez || this.pg) return rez;
    return this.parent.isEnabled();
}

/***************************************************
 * pwDescr - Page/widget descriptor object         *
 ***************************************************/
function pwDescr( pgAddr, pg, parent )
{
    this.addr = pgAddr;
    this.pages = new Object();
    this.wdgs = new Object();
    this.attrs = new Object();
    this.attrsMdf = new Object();
    this.pg = pg;
    this.parent = parent;
    this.window = null;
    this.windowExt = false;
    this.place = null;
    this.callPage = callPage;
    this.findOpenPage = findOpenPage;
    this.makeEl = makeEl;
    this.pwClean = pwClean;
    this.perUpdtEn = perUpdtEn;
    this.perUpdt = perUpdt;
    this.xScale = xScale;
    this.yScale = yScale;
    this.isEnabled = isEnabled;
    this.updCntr = 0;
    this.wPos = -1;
}
/***************************************************
 * makeUI                                          *
 ***************************************************/
function makeUI( callBackRez )
{
    if(!callBackRez) stTmMain = new Date();
    else prcCnt = (callBackRez == -1) ? Math.min(0,prcCnt) - 1 :  Math.max(0,prcCnt) + 1;

    //Get open pages list
    // Synchronous
    //var pgNode = servGet('/'+sessId,'com=pgOpenList&tm='+tmCnt);
    // Asynchronous
    var pgNode = null;
    if(callBackRez) pgNode = (callBackRez == -1) ? null : callBackRez;
    else { servGet('/'+sessId,'com=pgOpenList&tm='+tmCnt,makeUI); return; }
    if(pgNode) {
	// Getting for the styles
	if(tmCnt == 0)	modelStyles = servGet('/'+sessId,'com=style');

	modelPer = parseInt(pgNode.getAttribute("per"));
	if((alrmUpdCnt+=planePer) > tmAlrmUpd) { alarmSet(parseInt(pgNode.getAttribute("alarmSt"))); alrmUpdCnt = 0; }
	cachePgSz   = parseInt(pgNode.getAttribute("cachePgSz"));
	cachePgLife = parseFloat(pgNode.getAttribute("cachePgLife"));
	// Checking for deleting the pages
	for(var iP = 0; iP < pgList.length; iP++) {
	    var opPg; var iCh;
	    for(iCh = 0; iCh < pgNode.childNodes.length; iCh++)
		if(pgNode.childNodes[iCh].nodeName == 'pg' && pgNode.childNodes[iCh].textContent == pgList[iP])
		    break;
	    if(iCh < pgNode.childNodes.length || !(opPg=masterPage.findOpenPage(pgList[iP]))) continue;
	    if(opPg.window) {
		if(opPg.windowExt) opPg.window.close();
		else if(opPg != masterPage) document.getElementById('mainCntr').removeChild(opPg.window);
		else {
		    document.body.removeChild(opPg.window);
		    masterPage = new pwDescr('', true)
		}
		if(opPg.parent)	delete opPg.parent.pages[pgList[iP]];
	    }
	    else if(opPg.parent && opPg.parent.inclOpen && opPg.parent.inclOpen == pgList[iP])
	    { opPg.parent.attrs['pgOpenSrc'] = ''; opPg.parent.makeEl(null, true); }
	}
	// Processing the opened pages
	pgList = new Array();
	for(var i = 0; i < pgNode.childNodes.length; i++)
	    if(pgNode.childNodes[i].nodeName == 'pg') {
	        var prPath = pgNode.childNodes[i].textContent;
		//  Checking for closed windows
		var opPg = masterPage.findOpenPage(prPath);
		if(opPg && opPg.window && opPg.windowExt && opPg.window.closed) {
		    servSet(prPath, 'com=pgClose&cacheCntr', '');
		    opPg.pwClean(true);
		    delete opPg.parent.pages[prPath];
		    continue;
		}
		//  Call page
		pgList.push(prPath);
		masterPage.callPage(prPath, parseInt(pgNode.childNodes[i].getAttribute('updWdg')));
	    }
	tmCnt_ = parseInt(pgNode.getAttribute('tm'));
	if((tmCnt_-tmCnt) < -10) window.location.reload();	//The server or the counter restarted - reload the session
	tmCnt = tmCnt_;
    }
    else if(prcCnt < 10) window.location.reload();

    //Update some widgets
    for(var i in perUpdtWdgs) perUpdtWdgs[i].perUpdt();

    //Processing for the cache lifetime
    pgCacheProc();

    //Elapsed time get and adjust for plane update period depends from the network speed
    var elTm = 1e-3*((new Date()).getTime()-stTmMain.getTime());
    if(!planePer) planePer = 1e-3*modelPer;
    planePer = Math.min(Math.max(1e-3*modelPer,elTm*10), planePer + (Math.max(1e-3*modelPer,elTm*3)-planePer)/100);
    var sleepTm = Math.max(0, planePer-elTm);
    prcTm = elTm + sleepTm;
    //console.log("sleepTm: "+sleepTm+"s; prcTm: "+prcTm+"s; elTm: "+elTm+"s; planePer: "+planePer+"s.");
    setTimeout(makeUI, sleepTm*1e3);
    //if(mainTmId) clearTimeout(mainTmId);
    //mainTmId = setTimeout(makeUI, 1000);

    //prcTm = Math.max(modelPer*1e-3,Math.min(60,3e-3*((new Date()).getTime() - stTmMain.getTime())));
    //setTimeout(makeUI,prcTm*1e3);

    //Execution performance test
    /*var startChkTm = (new Date()).getTime();
    for(var i_cnt = 0; i_cnt < 1000000; i_cnt++)
	var trez = Math.sin(Math.PI);
    var stopChkTm = (new Date()).getTime();
    console.log("sin(PI): "+(stopChkTm-startChkTm)+"ms; trez: "+trez);*/
}

/***************************************************
 * setStatus - Setup status message.               *
 ***************************************************/
function setStatus( mess, tm )
{
    if(!mess) mess = '';
    if(masterPage && masterPage.status && (stb=document.getElementById('StatusBar'))) stb.innerText = mess;
    else window.status = mess;
    if(!mess) return;
    if(stTmID) clearTimeout(stTmID);
    if(!tm || tm > 0) stTmID = setTimeout('setStatus(null)', tm?tm:1000);
}

/**************************************************
 * getPopup - Get popup menu.                     *
 **************************************************/
function getPopup( )
{
    var popUpMenu = document.getElementById('popupmenu');
    if(!popUpMenu) {
	popUpMenu = document.createElement('div'); popUpMenu.id = 'popupmenu';
	popUpMenu.appendChild(document.createElement('select'));
	document.body.appendChild(popUpMenu);
	popUpMenu.style.visibility = 'hidden';
    }
    return popUpMenu;
}

/**************************************************
 * getTree - Get the items tree                   *
 **************************************************/
function getTree( )
{
    var formObj = document.createElement('ul');
    formObj.finish = function(lit, selIt) {
	this.style.cssText += 'font-style: italic; ';
	if(selIt && !selIt.length) return false;
	var cnt = lit ? lit.children[2] : this;
	imgSz = Math.floor(Math.max(16,parseInt(this.style.fontSize)*1.14));
	for(var iIt = cnt.children.length-1; iIt >= 0; iIt--) {
	    var oIt = cnt.children[iIt];
	    if(selIt) {
		if(selIt == oIt.itPath) { oIt.classList.add("select"); this.mSelIt = oIt; return true; }
		if(this.finish(oIt,selIt)) oIt.itExp = true;
	    }
	    else {
		if(oIt.itMdf != this.itMdf) { cnt.removeChild(oIt); continue; }
		this.finish(oIt);
		oIt.children[2].style.backgroundImage = (oIt!=cnt.lastChild?"url(/"+MOD_ID+"/img_treeLine)":"");
	    }
	    isExpandable = oIt.children[2].children.length;
	    oIt.children[0].src = "/"+MOD_ID+"/img_tree"+
		(isExpandable?(oIt.itExp?"Minus":"Plus"):"")+
		"Up"+(oIt!=cnt.lastChild?"Down":"");
	    oIt.children[0].style = "height: "+imgSz+"px; ";
	    if(isExpandable) oIt.children[2].style.display = oIt.itExp ? "" : "none";
	    if(isExpandable) oIt.classList.add("expandable"); else oIt.classList.remove("expandable");
	    if(selIt && this.mSelIt) return true;
	}
	return false;
    }
    formObj.expand = function(lit) {
	if(!lit || !lit.children[2].children.length) return;
	lit.itExp = !lit.itExp;
	lit.children[2].style.display = lit.itExp ? "" : "none";
	lit.children[0].src = "/"+MOD_ID+"/img_tree"+(lit.itExp?"Minus":"Plus")+"Up"+(lit!=lit.parentNode.lastChild?"Down":"");
    }
    formObj.selIt = function(ipath, sendEv) {
	if(sendEv) this.select(ipath);
	if(this.mSelIt) { this.mSelIt.classList.remove("select"); this.mSelIt = null; }
	this.finish(null, ipath);
    }
    formObj.setList = function(items) {
	while(this.children.length) this.removeChild(this.children[0]);
	formObj.itMdf = formObj.itMdf ? (formObj.itMdf+1) : 1;
	elLst = items.split('\n');
	cur_it = null;
	for(iEl = 0; iEl < elLst.length; iEl++) {
	    ipath = elLst[iEl];
	    for(lev = 0, pathLev.off = 0; (item=pathLev(ipath,0,true)).length; lev++) {
		if(lev == 0) {
		    for(cur_it = null, iR = 0; !cur_it && iR < formObj.children.length; iR++)
			if(formObj.children[iR].children[1].innerText == item) cur_it = formObj.children[iR];
		    if(!cur_it) formObj.appendChild((cur_it=this.ownerDocument.createElement('li')));
		    cur_it.itMdf = formObj.itMdf;
		}
		else {
		    for(t_it = null, iR = 0; !t_it && iR < cur_it.children[2].children.length; iR++)
			if(cur_it.children[2].children[iR].children[1].innerText == item) t_it = cur_it.children[2].children[iR];
		    if(!t_it) cur_it.children[2].appendChild((t_it=this.ownerDocument.createElement('li')));
		    cur_it = t_it;
		    cur_it.itMdf = formObj.itMdf;
		}
		if(!cur_it.children.length) {
		    cur_it.innerHTML = "<img onclick='this.parentNode.formObj.expand(this.parentNode)'/>"+
				       "<span onclick='this.parentNode.formObj.selIt(this.parentNode.itPath,true)'>"+item+"</span><ul\>";
		    cur_it.formObj = formObj;
		}
	    }
	    if(cur_it && ipath.length) {
		cur_it.classList.add("selectable");
		cur_it.itPath = ipath;
	    }
	}
	formObj.finish();
    }

    return formObj;
}

/***************************************************
 * Main start code                                 *
 ***************************************************/
var SEC_XT = 0x01;	//Extended
var SEC_WR = 0x02;	//Write access
var SEC_RD = 0x04;	//Read access

//Alarms processing
function alarmSet( alarm ) {
    ch_tp = alarm^mAlrmSt;

    alarmSt = document.getElementById("st_alarm");
    if(!alarmSt) return;
    alarmSt_img = alarmSt.childNodes[0];

    //Set notify to the alarm
    for(iNtf = 0; iNtf < 8; iNtf++) {
	if(notifiers[iNtf] == null) continue;

	if(((newSt=(alarm>>8)&(1<<iNtf)) && notifiers[iNtf].stTag.style.display.length) || (!newSt && !notifiers[iNtf].stTag.style.display.length))
	    notifiers[iNtf].stTag.style.display = newSt ? "" : "none";
	if(((newSt=(alarm>>16)&(1<<iNtf)) && notifiers[iNtf].stTag.className == 'inactive') || (!newSt && notifiers[iNtf].stTag.className != 'inactive')) {
	    notifiers[iNtf].stTag.className = newSt ? '' : 'inactive';
	    notifiers[iNtf].stTag.style.cursor = (newSt || notifiers[iNtf].f_quietanceRet) ? 'pointer' : '';
	}

	// Checking and updating the audio tags
	if(notifiers[iNtf].aTag) {
	    toPlay = (alarm>>16) & (1<<iNtf);
	    if(!toPlay && notifiers[iNtf].aTag.getAttribute("src").length) notifiers[iNtf].aTag.src = '';	//disabling the playback by clearing the source
	    else if(toPlay && (!notifiers[iNtf].aTag.getAttribute("src").length ||
		    !notifiers[iNtf].aTag.currentTime || (notifiers[iNtf].repDelay >= 0 && notifiers[iNtf].aTag.ended && (notifiers[iNtf].aTag.delay-=Math.max(planePer,tmAlrmUpd)) <= 0))) {
		//  Requesting the resource data
		reqRes = "<get mode='resource' tp='"+iNtf+"' tm='"+notifiers[iNtf].queueCurTm+"' wdg='"+notifiers[iNtf].queueCurPath+"'/>";
		reqRes = servSet("/UI/VCAEngine/"+sessId+"/%2fserv%2falarm", 'com=com', reqRes, false);
		notifiers[iNtf].queueCurTm = parseInt(reqRes.getAttribute("tm"));
		notifiers[iNtf].queueCurPath = reqRes.getAttribute("wdg");
		//mess = reqRes.getAttribute("mess");
		//lang = reqRes.getAttribute("lang");
		if(reqRes.textContent.length) {
		    resTp = reqRes.getAttribute("resTp");
		    notifiers[iNtf].aTag.src = "data:"+(resTp.length?resTp:"audio/ogg")+";base64,"+reqRes.textContent;
		    notifiers[iNtf].aTag.play();
		    notifiers[iNtf].aTag.delay = notifiers[iNtf].repDelay;
		}
	    }
	}
    }

    //Alarm state indicators update
    // Alarm level icon update
    if(ch_tp&0xFF || (alarm>>16)&notifiers.ntfSet || !alarmSt_img.style.backgroundColor.length) {
	alarmLev = alarm&0xFF;
	alarmSt_img.style.cursor = alarmLev ? 'pointer' : '';
	alarmSt_img.style.backgroundColor = alarmSt_img.style.backgroundColor.length ? '' :
	    '#'+(alarmLev?'ff':'00')+(alarmLev?((alarmLev>245)?'0':'')+(255-alarmLev).toString(16):'FF')+'00';
	alarmSt_img.title = alarmSt.getAttribute("title1").replace('%1',alarmLev);
    }

    mAlrmSt = alarm;
}

function alarmQuiet( ev, iNtf, ret ) {
    tmpl = (iNtf && iNtf >= 0) ? (1<<iNtf) : 255;

    servSet("/UI/VCAEngine/"+sessId+"/%2fserv%2falarm", "com=com", "<quietance tmpl='"+tmpl+"' ret='"+(ret?1:0)+"' wdg='"+(notifiers[iNtf]?notifiers[iNtf].curQueueWdg:'')+"'/>");

    var attrs = new Object();
    attrs.event = "ws_"+ev; setWAttrs(masterPage.addr, attrs);
}

function ntfReg( tp, props, pgCrtor, prior ) {
    if(tp < 0) {
	for(iNtf = 0; iNtf < 8; iNtf++)
	    ntfReg(iNtf, props, pgCrtor, prior);
	return;
    }

    if(prior == null)	prior = true;
    pgPropsQ = new Array();

    //Search for presented notification type
    if(notifiers[tp] != null) {
	if(notifiers[tp].pgProps.indexOf(pgCrtor+"\n") == 0 && (props == notifiers[tp].props || !prior)) return;
	pgPropsQ = notifiers[tp].pgPropsQ;
	if(notifiers[tp].pgProps.indexOf(pgCrtor+"\n") != 0) {
	    // Check the queue for the page already here
	    for(iQ = 0; iQ != pgPropsQ.length; ++iQ)
		if(pgPropsQ[iQ].slice(0,pgCrtor.length+1) == (pgCrtor+"\n")) {
		    if(!props.length) notifiers[tp].pgPropsQ.splice(iQ, 1);
		    else notifiers[tp].pgPropsQ[iQ] = pgCrtor + "\n" + props;
		    return;
		}
	    if(!props.length) return;
	    pgPropsQ.push(notifiers[tp].pgProps);
	}
	// Removing the notifier for external things
	if(notifiers[tp].aTag) notifiers[tp].aTag.parentNode.removeChild(notifiers[tp].aTag);
	if(notifiers[tp].stTag) notifiers[tp].stTag.parentNode.removeChild(notifiers[tp].stTag);

	delete notifiers[tp];
	notifiers.ntfSet &= ~(1<<tp);
    }

    //Creation new or replacing present one
    pgProps = pgCrtor + "\n" + props;
    if(props.length) ;
    //Take and place a notificator from the queue
    else if(pgPropsQ.length) pgProps = pgPropsQ.pop();
    else return;

    //Initiating the notifier for external things
    notifiers.ntfSet |= (1<<tp);
    notifiers[tp] = new Object();
    notifiers[tp].pgProps = pgProps;
    notifiers[tp].pgPropsQ = pgPropsQ;
    notifiers[tp].f_notify = false;
    notifiers[tp].f_queue = false;
    notifiers[tp].f_resource = false;
    notifiers[tp].f_quietanceRet = false;
    notifiers[tp].repDelay = -1;
    notifiers[tp].aTag = null;
    notifiers[tp].stTag = null;
    notifiers[tp].queueCurTm = 0;
    notifiers[tp].curQueueWdg = '';

    //Parsing the properties
    prpLns = pgProps.split('\n');
    for(iLn = 1, hasFlags = false, ico = name = ""; (!hasFlags || !ico.length || !name.length) && iLn < prpLns.length; iLn++)
	if(!hasFlags && (fPos=prpLns[iLn].indexOf("flags=")) >= 0) {
	    if((oPos=prpLns[iLn].indexOf("notify",fPos+6)) >= 0) {
		notifiers[tp].f_notify = true;
		notifiers[tp].repDelay = ((ntfChr=prpLns[iLn].charCodeAt(oPos+6)) >= 0x30 && ntfChr <= 0x39) ? Math.max(0,Math.min(100,parseInt(prpLns[iLn].slice(oPos+6)))) : -1;
	    }
	    if(prpLns[iLn].indexOf("resource",fPos+6) >= 0)	notifiers[tp].f_resource = true;
	    if(prpLns[iLn].indexOf("queue",fPos+6) >= 0)	{ notifiers[tp].f_queue = true; if(notifiers[tp].repDelay < 0) notifiers[tp].repDelay = 0; }
	    if(prpLns[iLn].indexOf("quietanceRet",fPos+6) >= 0)	notifiers[tp].f_quietanceRet = true;
	}
	else if(!ico.length && (fPos=prpLns[iLn].indexOf("ico=")) >= 0)   ico = prpLns[iLn].slice(fPos+4);
	else if(!name.length && (fPos=prpLns[iLn].indexOf("name=")) >= 0) name = prpLns[iLn].slice(fPos+5);

    //Creation the audio tag
    if(notifiers[tp].f_notify && (notifiers[tp].f_queue || notifiers[tp].f_resource)) {
	notifiers[tp].aTag = document.createElement("audio");
	notifiers[tp].aTag.autoplay = true;
	notifiers[tp].aTag.src = '';
	//notifiers[tp].aTag.id = "notifierAudioTag"+tp;
	document.body.appendChild(notifiers[tp].aTag);
    }

    //Creation the statusbar icons
    if(masterPage.status) {
	notifiers[tp].stTag = document.createElement("img");
	notifiers[tp].stTag.id = "alarmNtf"+tp;
	notifiers[tp].stTag.title = name.length ? name : "###Notificator %1###".replace("%1",tp);
	notifiers[tp].stTag.height = (masterPage.status.height-2);
	notifiers[tp].stTag.src = "/"+MOD_ID+(ico.length?pgCrtor+"?com=res&val="+ico+"&size="+(masterPage.status.height-2):"/img_alarmAlarm");
	notifiers[tp].stTag.style.display = 'none';
	notifiers[tp].stTag.quietanceRet = notifiers[tp].f_quietanceRet;
	notifiers[tp].stTag.onclick = function( ) { alarmQuiet(this.id, parseInt(this.id.slice(8)), (this.quietanceRet && this.className == 'inactive')); }
	masterPage.status.children[0].rows[0].children.st_alarm.appendChild(notifiers[tp].stTag);
    }
}

//Call session identifier
var sessId = location.pathname.split('/');
for(var i_el = sessId.length-1; i_el >= 0; i_el--)
    if(sessId[i_el].length)
    { sessId = sessId[i_el]; break; }

function styleSet( sid ) {
    servSet("/UI/VCAEngine/"+sessId+"/%2fobj%2fcfg%2fstyle", "com=com", "<set>"+sid+"</set>");
    stTmReload = setTimeout('window.location.reload()', 1000);
}

document.body.onmouseup = function(e)
{
    if(!e) e = window.event;
    if(evMouseGet(e) != 'Left') return true;
    var popUpMenu = document.getElementById('popupmenu');
    if(popUpMenu) popUpMenu.style.visibility = 'hidden';
    //return false;	//!!!! It's buggy on <input type=range> for Chrome
}

window.onresize = function( ) {
    if(stTmReload) clearTimeout(stTmReload);

    brwsSc = (window.devicePixelRatio && window.devicePixelRatio != 1);

    if((!brwsSc && ((window.innerHeight-document.body.clientHeight)/window.innerHeight > 0.1 ||
		((document.body.clientHeight-window.innerHeight)/document.body.clientHeight > 0.5 && wy_scale > 1))) ||
	    (brwsSc && (wy_scale != 1 || wx_scale != 1)))
	stTmReload = setTimeout('window.location.reload()', 1000);
}

var reqObjects = new Array();		//Request objects
var fullUpdCnt = 0;			//Full tree updating counter
var alrmUpdCnt = 0;			//Alarms updating counter
var mAlrmSt = -1;			//Current allarm state
var modelPer = 0;			//Model proc period
var modelStyles = null;			//Model styles
var prcCnt = 0;				//Process counter
var prcTm = 0;				//Process time
var planePer = 0;			//Planed update period
var tmCnt = 0;				//Calls counter
var pgList = new Array();		//Opened pages list
var cachePgLife = 0;			//Life time of the pages into the cache
var cachePgSz = 0;			//Maximum number of the pages cache
var pgCache = new Array();		//Cached pages' data
var perUpdtWdgs = new Object();		//Periodic updated widgets register
var notifiers = new Object();		//Notifiers object
notifiers.ntfSet = 0;
var masterPage = new pwDescr('', true);	//Master page create
var stTmID = null;			//Status line timer identifier
var stTmMain = null;			//Main cycle start time
var stTmReload = null;			//Main reload timeout at the main window resize
var wx_scale = 1;			//Main window scale for fit, X axis
var wy_scale = 1;			//Main window scale for fit, Y axis
var sesUser = '';			//Session user

mainTmId = setTimeout(makeUI, 100);	//First call init