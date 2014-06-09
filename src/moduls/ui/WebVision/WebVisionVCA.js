
//OpenSCADA system module UI.WebVision file: VCA.js
/***************************************************************************
 *   Copyright (C) 2007-2014 by Roman Savochenko, <rom_as@oscada.org>      *
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

/***************************************************
 * strEncode - String encoding.                    *
 ***************************************************/
function strEncode( vl, tp )
{
    var encRez = '';
    if(!tp || tp == "html")
	for(var i_enc = 0; i_enc < vl.length; i_enc++)
	    switch(vl.charAt(i_enc))
	    {
		case '&': encRez += '&amp;'; break;
		case '>': encRez += '&gt;'; break;
		case '<': encRez += '&lt;'; break;
		case '"': encRez += '&quot;'; break;
		default:  encRez += vl.charAt(i_enc);
	    }
    return encRez;
}

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
    while(true)
    {
	t_dir = path.indexOf('/',an_dir);
	if(t_dir < 0)		{ pathLev.off = path.length; return (t_lev==level)?path.substr(an_dir):''; }
	if(t_lev == level)	{ pathLev.off = t_dir; return path.substr(an_dir,t_dir-an_dir); }
	an_dir = t_dir;
	t_lev++;
	while(an_dir<path.length && path.charAt(an_dir)=='/') an_dir++;
    }
}
/***************************************************
 * noSpace - Get no space string                   *
 ***************************************************/
function noSpace( str )
{
    var posSt = -1; var posEnd = 0;
    for(var i = 0; i < str.length; i++)
	if(str[i] != ' ') { if(posSt<0) posSt = i; posEnd = i; }
    return (posSt>=0) ? str.substr(posSt,posEnd-posSt+1) : '';
}
/***************************************************
 * nodeText - Get DOM node text                    *
 ***************************************************/
function nodeText( node )
{
    var rez = '';
    for(var i = 0; i < node.childNodes.length; i++)
	if(node.childNodes[i].nodeType == 3) rez += node.childNodes[i].data;
    return rez;
}
/***************************************************
 * setNodeText - Set DOM node text                 *
 ***************************************************/
function setNodeText( node, val )
{
    if(!node) return;
    for(var i = 0; i < node.childNodes.length; i++)
	if(node.childNodes[i].nodeType == 3)
	{ node.childNodes[i].data = val; return; }
    node.appendChild(node.ownerDocument.createTextNode(val));
}
/*****************************************************
 * nodeTextByTagId - Get DOM node by tag name and id *
 *****************************************************/
function nodeTextByTagId( node, tag, avl )
{
    for(var i = 0; i < node.childNodes.length; i++)
	if(node.childNodes[i].nodeName == tag && node.childNodes[i].getAttribute('id') == avl)
	    return nodeText(node.childNodes[i]);
    return '';
}
/***************************************************
 * posGetX - Get absolute position                 *
 **************************************************/
function posGetX(obj,noWScrl)
{
    var posX = 0;
    for( ; obj && obj.nodeName != 'BODY'; obj = obj.parentNode)
    {
	posX += (obj.style.left?parseInt(obj.style.left):0)+
	    (obj.parentNode.style.borderLeftWidth?parseInt(obj.parentNode.style.borderLeftWidth):0)+
	    (obj.parentNode.style.marginLeft?parseInt(obj.parentNode.style.marginLeft):0);
	if(obj.style.position == 'relative') posX += obj.offsetLeft;
    }
    return posX+(!noWScrl?-window.pageXOffset:0);
}
/***************************************************
 * posGetY - Get absolute position                 *
 **************************************************/
function posGetY(obj,noWScrl)
{
    var posY = 0;
    for( ; obj && obj.nodeName != 'BODY'; obj = obj.parentNode)
    {
	posY += (obj.style.top?parseInt(obj.style.top):0)+
	    (obj.parentNode.style.borderTopWidth?parseInt(obj.parentNode.style.borderTopWidth):0)+
	    (obj.parentNode.style.marginTop?parseInt(obj.parentNode.style.marginTop):0);
	if(obj.style.position == 'relative') posY += obj.offsetTop;
    }
    return posY+(!noWScrl?-window.pageYOffset:0);
}
/***************************************************
 * getXmlHttp - Check and return XMLHttpRequest for*
 *  various browsers.                              *
 ***************************************************/
function getXmlHttp( )
{
    if(window.XMLHttpRequest) return new XMLHttpRequest();
    else if(window.ActiveXObject)
    {
	try { return new ActiveXObject('Msxml2.XMLHTTP'); }
	catch(e) { return new ActiveXObject('Microsoft.XMLHTTP'); }
    }
    return null;
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
    switch(e.keyCode)
    {
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
    if(isNN) switch(e.keyCode)
    {
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
    else switch(e.keyCode)
    {
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
function servGet( adr, prm, callBack )
{
    var req = getXmlHttp();
    req.open('GET', encodeURI('/'+MOD_ID+adr+'?'+prm), callBack ? true : false);
    if(callBack)
    {
	req.callBack = callBack;
	req.onreadystatechange = function( )
	{
	    if(this.readyState != 4) return;
	    if(this.status == 200 && this.responseXML.childNodes.length)
		this.callBack(this.responseXML.childNodes[0]);
	    else this.callBack(-1);
	};
	req.send(null);
	return null;
    }
    try
    {
	req.send(null);
	if(req.status == 200 && req.responseXML.childNodes.length)
	    return req.responseXML.childNodes[0];
    } catch(e) { window.location = '/'+MOD_ID; }

    return null;
}
/***************************************************
 * servSet - XML set request to server             *
 ***************************************************/
function servSet( adr, prm, body, waitRez )
{
    var req = getXmlHttp();
    req.open('POST', encodeURI('/'+MOD_ID+adr+'?'+prm), !waitRez);
    try
    {
	req.send(body);
	if(waitRez && req.status == 200 && req.responseXML.childNodes.length)
	    return req.responseXML.childNodes[0];
    }
    catch(e) { window.location = '/'+MOD_ID; }

    return null;
}
/***************************************************
 * getWAttr - request page/widget attribute        *
 ***************************************************/
function getWAttr( wId, attr )
{
    var rNode = servGet(wId,'com=attr&attr='+attr);
    if(!rNode) return null;

    return nodeText(rNode);
}
/***************************************************
 * setWAttrs - set page/widget attributes          *
 ***************************************************/
function setWAttrs( wId, attrs, val )
{
    var body = '<set>';
    if(typeof(attrs) == 'string') body += '<el id=\''+attrs+'\'>'+val+'</el>';
    else for(var i in attrs) body += '<el id=\''+i+'\'>'+attrs[i]+'</el>';
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
    if(fStr)
    {
	var allFnt = fStr.split(' ');
	if(opt == 2) return (parseInt(allFnt[1])*(fSc?fSc:1)).toFixed(0);
	if(allFnt.length >= 1) rez += 'font-family: ' + allFnt[0].replace(/_/g,' ') + '; ';
	if(allFnt.length >= 2) rez += 'font-size: ' + (parseInt(allFnt[1])*(fSc?fSc:1)).toFixed(0) + (opt==1?'pt; ':'px; ');
	if(allFnt.length >= 3) rez += 'font-weight: ' + (parseInt(allFnt[2])?'bold':'normal') + '; ';
	if(allFnt.length >= 4) rez += 'font-style: ' + (parseInt(allFnt[3])?'italic':'normal') + '; ';
    }

    return rez;
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
    //> Check by regular expression
    if(patt.length > 2 && patt.charAt(0) == '/' && patt.charAt(patt.length-1) == '/')
	return (new RegExp(patt.slice(1,patt.length-1),'')).test(val);

    //> Check by simple pattern
    var mult_s = false;
    var v_cnt = 0, p_cnt = 0;
    var v_bck = -1, p_bck = -1;

    while(true)
    {
	if(p_cnt >= patt.length)	return true;
	if(patt.charAt(p_cnt) == '?')	{ v_cnt++; p_cnt++; mult_s = false; continue; }
	if(patt.charAt(p_cnt) == '*')	{ p_cnt++; mult_s = true; v_bck = -1; continue; }
	if(patt.charAt(p_cnt) == '\\')	p_cnt++;
	if(v_cnt >= val.length)	break;
	if(patt.charAt(p_cnt) == val.charAt(v_cnt))
	{
	    if(mult_s && v_bck < 0)	{ v_bck = v_cnt+1; p_bck = p_cnt; }
	    v_cnt++; p_cnt++;
	}
	else
	{
	    if(mult_s)
	    {
		if(v_bck >= 0) { v_cnt = v_bck; p_cnt = p_bck; v_bck = -1; }
		else v_cnt++;
	    }
	    else break;
	}
    }
    return false;
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

/**********************************************************
 * callPage - call page 'pgId' for open update and other. *
 **********************************************************/
function callPage( pgId, updWdg, pgGrp, pgOpenSrc )
{
    if(!pgId) return true;

    //> Check and update present page
    if(this == masterPage)
    {
	var opPg = this.findOpenPage(pgId);
	if(opPg)
	{
	    if(!(prcCnt%5)) opPg.makeEl(servGet(pgId,'com=attrsBr&FullTree=1&tm='+tmCnt),false,false,true);
	    else if(updWdg) opPg.makeEl(servGet(pgId,'com=attrsBr&tm='+tmCnt));
	    return true;
	}
    }

    //> Create or replace main page
    if(!pgGrp) pgGrp = getWAttr(pgId,'pgGrp');
    if(!pgOpenSrc) pgOpenSrc = getWAttr(pgId,'pgOpenSrc');
    if(this == masterPage && (!this.addr.length || pgGrp == 'main' || pgGrp == this.attrs['pgGrp']))
    {
	if(this.addr.length)
	{
	    servSet(this.addr,'com=pgClose','');
	    this.pwClean();
	    document.body.removeChild(this.window);
	}
	this.addr  = pgId;
	this.place = document.createElement('div');
	this.place.setAttribute('id','mainCntr');
	this.makeEl(servGet(pgId,'com=attrsBr'),false,true);
	var centerTag = document.createElement('center');
	centerTag.appendChild(this.place);
	document.body.appendChild(centerTag);
	this.window = centerTag;
	//> Set project's icon and RunTime page title
	document.getElementsByTagName('link')[0].setAttribute('href',location.pathname+'?com=ico');
	return true;
    }

    //> Find for include page creation
    for(var i in this.wdgs)
	if(this.wdgs[i].attrs['root'] == 'Box' && this.wdgs[i].isVisible)
	{
	    if(pgGrp == this.wdgs[i].attrs['pgGrp'] && pgId != this.wdgs[i].attrs['pgOpenSrc'])
	    {
		this.wdgs[i].attrs['pgOpenSrc'] = pgId;
		this.wdgs[i].makeEl(null,true);
		setWAttrs(this.wdgs[i].addr,'pgOpenSrc',pgId);
		return true;
	    }
	    if(this.wdgs[i].inclOpen && this.wdgs[i].pages[this.wdgs[i].inclOpen].callPage(pgId,updWdg,pgGrp,pgOpenSrc)) return true;
	}
    //> Put checking to child pages
    for(var i in this.pages)
	if(this.pages[i].callPage(pgId,updWdg,pgGrp,pgOpenSrc)) return true;
    //> Check for open child page or for unknown and empty source pages open as master page child windows
    if((!pgGrp.length && pgOpenSrc == this.addr) || this == masterPage)
    {
	var iPg = new pwDescr(pgId,true,this);
	var attrBrVal = servGet(pgId,'com=attrsBr');

	var winName = null;
	var winWidth = 600;
	var winHeight = 400;
	for(var i_ch = 0; i_ch < attrBrVal.childNodes.length; i_ch++)
	    if(attrBrVal.childNodes[i_ch].nodeName != 'el') continue;
	    else if(attrBrVal.childNodes[i_ch].getAttribute('id') == 'name') winName = nodeText(attrBrVal.childNodes[i_ch]);
	    else if(attrBrVal.childNodes[i_ch].getAttribute('id') == 'geomW') winWidth = parseInt(nodeText(attrBrVal.childNodes[i_ch]));
	    else if(attrBrVal.childNodes[i_ch].getAttribute('id') == 'geomH') winHeight = parseInt(nodeText(attrBrVal.childNodes[i_ch]));

	//> New external <div> window create
	if(winWidth < parseInt(masterPage.attrs['geomW']) && winHeight < parseInt(masterPage.attrs['geomH']))
	{
	    iPg.window = document.createElement('table');
	    iPg.window.setAttribute('cellpadding','2');
	    iPg.window.setAttribute('cellspacing','0');
	    iPg.window.setAttribute('border','0');
	    iPg.window.setAttribute('style','position: absolute; font-size: 12px; z-index: 9999; background-color: #8FA9FF; border: 1px solid darkblue; '+
		'left: '+((parseInt(masterPage.attrs['geomW'])-winWidth-5)/2)+'px; '+
		'top: '+((parseInt(masterPage.attrs['geomH'])-winHeight-18)/2)+'px; ');
	    var wRow = document.createElement('tr');
	    iPg.window.appendChild(wRow);
	    wRow.setAttribute('style','cursor: move;');
	    wRow.win = iPg.window;
    	    wRow.onmousedown = function(e)
    	    {
		if(!e) e = window.event;
		this.clX = e.clientX; this.clY = e.clientY;
		return false;
	    }
	    wRow.onmouseup = function(e)
	    {
		if(!e) e = window.event;
		this.clX = this.clY = null;
		return false;
	    }
	    wRow.onmouseout = wRow.onmouseup;
	    wRow.onmousemove = function(e)
	    {
		if(!e) e = window.event;
		if(this.clX != null)
		{
		    this.win.style.left = (parseInt(this.win.style.left)+e.clientX-this.clX)+"px";
		    this.win.style.top  = (parseInt(this.win.style.top)+e.clientY-this.clY)+"px";
		    this.clX = e.clientX; this.clY = e.clientY;
		}
	    }
	    var titleBlk = document.createElement('td');
	    wRow.appendChild(titleBlk);
	    titleBlk.setAttribute('style','max-width: '+(winWidth-7)+'px; width: '+(winWidth-7)+'px; overflow: hidden; white-space: nowrap;');
	    titleBlk.setAttribute('title',winName);
	    titleBlk.appendChild(document.createTextNode(winName));
	    var closeWin = document.createElement('td');
	    wRow.appendChild(closeWin);
	    closeWin.appendChild(document.createTextNode('X'));
	    closeWin.setAttribute('style','color: red; cursor: pointer;');
	    closeWin.onclick = function()
	    {
		servSet(this.iPg.addr,'com=pgClose','');
		document.getElementById('mainCntr').removeChild(this.iPg.window);
		this.iPg.pwClean();
		delete this.iPg.parent.pages[this.iPg.addr];
	    }
	    closeWin.iPg = iPg;
	    wRow = document.createElement('tr');
	    iPg.window.appendChild(wRow);
	    var wCntCell = document.createElement('td');
	    wRow.appendChild(wCntCell);
	    wCntCell.setAttribute('colspan','2');
	    wCntCell.setAttribute('align','center');
	    iPg.place = document.createElement('div');
	    wCntCell.appendChild(iPg.place);
	    document.getElementById('mainCntr').appendChild(iPg.window);
	}
	//> New external window create
	else
	{
	    iPg.window = window.open('about:blank',pgId,'width='+(winWidth+20)+',height='+(winHeight+60)+
		',directories=no,menubar=no,toolbar=no,scrollbars=yes,dependent=yes,location=no,locationbar=no,status=no,statusbar=no,alwaysRaised=yes');
	    if(!iPg.window) return true;
	    iPg.window.document.open( );
	    iPg.window.document.write("<html><body style='background-color: #E6E6E6;'><center><div id='main'/></center></body></html>\n");
	    iPg.window.document.close( );
	    iPg.window.document.title = winName;
	    var mainDiv = iPg.window.document.getElementById('main');
	    iPg.place = mainDiv;
	    iPg.windowExt = true;
	}

	this.pages[iPg.addr] = iPg;
	iPg.makeEl(attrBrVal,false,true);

	return true;
    }

  return false;
}

function findOpenPage( pgId )
{
    var opPg;
    if(pgId == this.addr) return this;
    //> Check from included widgets
    for(var i in this.wdgs)
	if(this.wdgs[i].attrs['root'] == 'Box' && this.wdgs[i].isVisible)
	{
	    if(pgId == this.wdgs[i].attrs['pgOpenSrc']) return this.wdgs[i].pages[pgId];
	    if(this.wdgs[i].inclOpen)
	    {
		opPg = this.wdgs[i].pages[this.wdgs[i].inclOpen].findOpenPage(pgId);
		if(opPg) return opPg;
	    }
	}
    //> Put checking to child pages
    for(var i in this.pages)
    {
	opPg = this.pages[i].findOpenPage(pgId);
	if(opPg) return opPg;
    }

    return null;
}

function makeEl( pgBr, inclPg, full, FullTree )
{
    var margBrdUpd = false; var newAttr = false;
    this.place.wdgLnk = this;
    if(!inclPg && pgBr)
    {
	//> Clear modify flag for all
	for(var j in this.attrsMdf) this.attrsMdf[j] = false;
	//> Updated attributes
	for(var j = 0; j < pgBr.childNodes.length; j++)
	{
	    if(pgBr.childNodes[j].nodeName != 'el') continue;
	    var i = pgBr.childNodes[j].getAttribute('id');
	    if((i == 'bordWidth' || i == 'geomMargin') && (!this.attrs[i] || this.attrs[i] != nodeText(pgBr.childNodes[j]))) margBrdUpd = true;
	    var curAttr = nodeText(pgBr.childNodes[j]);
	    this.attrsMdf[i] = !this.attrs[i] || this.attrs[i] != curAttr;
	    this.attrs[i] = curAttr;
	    newAttr = true;
	}
    }

    if(newAttr || inclPg || !pgBr)
    {
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
	if(this.parent && !(this.pg && this.parent.pg))
	{
	    geomX -= parseInt(this.parent.attrs['geomMargin'])+parseInt(this.parent.attrs['bordWidth']);
	    geomY -= parseInt(this.parent.attrs['geomMargin'])+parseInt(this.parent.attrs['bordWidth']);
	}
	elStyle+='position: '+((this==masterPage || this.window)?'relative':'absolute')+'; left: '+realRound(geomX)+'px; top: '+realRound(geomY)+'px; ';

	var xSc = this.xScale(true);
	var ySc = this.yScale(true);
	var geomW = realRound(parseFloat(this.attrs['geomW'])*xSc);
	var geomH = realRound(parseFloat(this.attrs['geomH'])*ySc);

	this.mousedown = new Array;
	this.mouseup = new Array;

	if(this.pg && this.parent && this.parent.inclOpen && this.parent.inclOpen == this.addr)
	{
	    var geomWpar = parseFloat(this.parent.attrs['geomW'])*this.parent.xScale(true);
	    var geomHpar = parseFloat(this.parent.attrs['geomH'])*this.parent.yScale(true);
	    this.parent.place.style.overflow = (geomW > geomWpar || geomH > geomHpar) ? 'scroll' : 'visible';
	    //elStyle += 'overflow: auto; ';
	    geomW = Math.max(geomW, geomWpar);
	    geomH = Math.max(geomH, geomHpar);
	}
	//else elStyle += 'overflow: hidden; ';

	geomW -= 2*(elMargin+elBorder);
	geomH -= 2*(elMargin+elBorder);

	//> Set included window geometry to widget size
	//if( this == masterPage ) resizeTo(geomW,geomH);
	if(this.pg && this.window && this.windowExt)
	{
	    if(this.window.innerHeight)
		this.window.resizeTo(geomW+(this.window.outerWidth-this.window.innerWidth)+20,
				     geomH+(this.window.outerHeight-this.window.innerHeight)+20);
	    else this.window.resizeTo(geomW+20,geomH+40);
	}

	if(this.attrs['focus'] && parseInt(this.attrs['focus'])) setFocus(this.addr,true);

	if(!(parseInt(this.attrs['perm'])&SEC_RD))
	{
	    if(this.pg)
	    {
		elStyle += 'background-color: #B0B0B0; border: 1px solid black; color: red; overflow: auto; ';
		this.place.innerHTML = "<div class='vertalign' style='width: "+(geomW-2)+"px; height: "+(geomH-2)+"px;'>Page: '"+this.addr+"'.<br/>View access is no permitted.</div>";
	    }
	}
	else if(this.attrs['root'] == 'ElFigure')
	{
	    if(this.attrs['backColor'] && getColor(this.attrs['backColor'],true))
		elStyle += 'background-color: '+getColor(this.attrs['backColor'])+'; ';
	    if(this.attrs['backImg']) elStyle += 'background-image: url(\'/'+MOD_ID+this.addr+'?com=res&val='+this.attrs['backImg']+'\'); ';
	    var toInit = !this.place.childNodes.length;
	    var figObj = toInit ? this.place.ownerDocument.createElement('img') : this.place.childNodes[0];
	    figObj.width = geomW; figObj.height = geomH;
	    //>> Src update always therefore for any changes is shape update
	    figObj.src = '/'+MOD_ID+this.addr+'?com=obj&tm='+tmCnt+'&geomX='+geomX.toFixed(3)+'&geomY='+geomY.toFixed(3)+
						    '&xSc='+xSc.toFixed(3)+'&ySc='+ySc.toFixed(3);
	    if(elWr != this.place.elWr)
	    {
		figObj.onclick = !elWr ? '' : function(e)
		{
		    if(!e) e = window.event;
		    servSet(this.wdgLnk.addr,'com=obj&sub=point&geomX='+geomX.toFixed(3)+'&geomY='+geomY.toFixed(3)+
						    '&xSc='+xSc.toFixed(3)+'&ySc='+ySc.toFixed(3)+
						    '&x='+(e.offsetX?e.offsetX:(e.clientX-posGetX(this)))+
						    '&y='+(e.offsetY?e.offsetY:(e.clientY-posGetY(this)))+
						    '&key='+evMouseGet(e),'');
		    return false;
    		}
    		figObj.ondblclick = !elWr ? '' : function(e)
    		{
		    if(!e) e = window.event;
		    servSet(this.wdgLnk.addr,'com=obj&sub=point&geomX='+geomX.toFixed(3)+'&geomY='+geomY.toFixed(3)+
						    '&xSc='+xSc.toFixed(3)+'&ySc='+ySc.toFixed(3)+
						    '&x='+(e.offsetX?e.offsetX:(e.clientX-posGetX(this)))+
						    '&y='+(e.offsetY?e.offsetY:(e.clientY-posGetY(this)))+
						    '&key=DblClick','');
		}
	    }

	    if(toInit)
	    {
		figObj.border = 0;
		figObj.wdgLnk = this;
		this.place.appendChild(figObj);
		//> Disable drag mostly for FireFox
		figObj.onmousedown = function(e) { e = e?e:window.event; if(e.preventDefault) e.preventDefault(); }
	    }
	}
	else if(this.attrs['root'] == 'Box')
	{
	    if(this == masterPage && this.attrs['tipStatus'].length) setStatus(this.attrs['tipStatus'],10000);
	    backStyle = '';
	    if(this.attrs['backColor'] && getColor(this.attrs['backColor'],true))
		backStyle += 'background-color: '+getColor(this.attrs['backColor'])+'; ';
	    if(this.attrs['backImg'])   backStyle += 'background-image: url(\'/'+MOD_ID+this.addr+'?com=res&val='+this.attrs['backImg']+'\'); ';
	    if(this == masterPage) document.body.style.cssText = parseInt(this.attrs['bordWidth']) ? '' : backStyle;
	    elStyle += backStyle;
	    elStyle += 'border-style: solid; border-width: '+this.attrs['bordWidth']+'px; ';
	    if(this.attrs['bordColor']) elStyle += 'border-color: '+getColor(this.attrs['bordColor'])+'; ';
	    switch(parseInt(this.attrs['bordStyle']))
	    {
		case 1: elStyle += 'border-style: dotted; '; break;
		case 2: elStyle += 'border-style: dashed; '; break;
		case 3: elStyle += 'border-style: solid; ';  break;
		case 4: elStyle += 'border-style: double; '; break;
		case 5: elStyle += 'border-style: groove; '; break;
		case 6: elStyle += 'border-style: ridge; ';  break;
		case 7: elStyle += 'border-style: inset; ';  break;
		case 8: elStyle += 'border-style: outset; '; break;
	    }
	    if(!this.pg && ((this.inclOpen && this.attrs['pgOpenSrc'] != this.inclOpen) ||
		    (!this.inclOpen && this.attrs['pgOpenSrc'].length)))
	    {
		if(this.inclOpen)
		{
		    servSet(this.inclOpen,'com=pgClose','');
		    pgCache[this.inclOpen] = this.pages[this.inclOpen];
		    pgCache[this.inclOpen].reqTm = tmCnt;
		    this.place.removeChild(this.pages[this.inclOpen].place);
		    this.pages[this.inclOpen].perUpdtEn(false);
		    delete this.pages[this.inclOpen];
		    this.inclOpen = null;
		}
		if(this.attrs['pgOpenSrc'].length)
		{
		    this.inclOpen = this.attrs['pgOpenSrc'];
		    if(pgCache[this.inclOpen])
		    {
			this.pages[this.inclOpen] = pgCache[this.inclOpen];
			this.place.appendChild(this.pages[this.inclOpen].place);
			pgBr = servGet(this.inclOpen,'com=attrsBr&tm='+pgCache[this.inclOpen].reqTm);
			this.pages[this.inclOpen].perUpdtEn(true);
			this.pages[this.inclOpen].makeEl(pgBr);
			delete pgCache[this.inclOpen];
		    }
		    else
		    {
			var iPg = new pwDescr(this.inclOpen,true,this);
			iPg.place = this.place.ownerDocument.createElement('div');
			iPg.makeEl(servGet(this.inclOpen,'com=attrsBr'),false,true);
			this.pages[this.inclOpen] = iPg;
			this.place.appendChild(iPg.place);
		    }
		    //Oppress scroll for include page size equal to the continer
		    if(this.place.width = this.pages[this.inclOpen].place.width) this.pages[this.inclOpen].place.width--;
		    if(this.place.height = this.pages[this.inclOpen].place.height) this.pages[this.inclOpen].place.height--;
		}
		elStyle += "overflow: scroll; ";
	    }
	    this.place.wdgLnk = this;
	    this.place.onclick = (!elWr) ? null : function() { setFocus(this.wdgLnk.addr); return false; };
	}
	else if(this.attrs['root'] == 'Text')
	{
	    if(this.attrs['backColor'] && getColor(this.attrs['backColor'],true))
		elStyle += 'background-color: '+getColor(this.attrs['backColor'])+'; ';
	    if(this.attrs['backImg'])	elStyle += 'background-image: url(\'/'+MOD_ID+this.addr+'?com=res&val='+this.attrs['backImg']+'\'); ';
	    elStyle += 'border-style: solid; border-width: '+this.attrs['bordWidth']+'px; overflow: hidden; ';
	    if(this.attrs['bordColor'])	elStyle += 'border-color: '+getColor(this.attrs['bordColor'])+'; ';
	    if(elMargin) { elStyle += 'padding: '+elMargin+'px; '; elMargin = 0; }
	    if(parseInt(this.attrs['orient']) == 0)
	    {
		var txtAlign = parseInt(this.attrs['alignment']);
		var spanStyle = 'display: table-cell; width: '+geomW+'px; height: '+geomH+'px; ';
		switch(txtAlign&0x3)
		{
		    case 0: spanStyle += 'text-align: left; ';		break;
		    case 1: spanStyle += 'text-align: right; ';		break;
		    case 2: spanStyle += 'text-align: center; ';	break;
		    case 3: spanStyle += 'text-align: justify; ';	break;
		}
		switch(txtAlign>>2)
		{
		    case 0: spanStyle += 'vertical-align: top; ';	break;
		    case 1: spanStyle += 'vertical-align: bottom; ';	break;
		    case 2: spanStyle += 'vertical-align: middle; ';	break;
		}
		spanStyle += getFont(this.attrs['font'],Math.min(xSc,ySc));
		spanStyle += 'color: ' + (this.attrs['color']?getColor(this.attrs['color']):'black') + '; ';
		var txtVal = this.attrs['text'];
		for(var i = 0; i < parseInt(this.attrs['numbArg']); i++)
		{
		    var argVal;
		    var argCfg = new Array();
		    switch(parseInt(this.attrs['arg'+i+'tp']))
		    {
			case 0: case 2:
			    argCfg[0] = this.attrs['arg'+i+'cfg'];
			    argVal = this.attrs['arg'+i+'val'];
			    break;
			case 1:
			    argCfg = this.attrs['arg'+i+'cfg'].split(';');
			    if(argCfg[1] == 'g')	argVal = parseFloat(this.attrs['arg'+i+'val']).toPrecision(Math.max(0,argCfg[2]));
			    else if(argCfg[1] == 'e')	argVal = parseFloat(this.attrs['arg'+i+'val']).toExponential(Math.max(0,argCfg[2]));
			    else if(argCfg[1] == 'f')	argVal = parseFloat(this.attrs['arg'+i+'val']).toFixed(Math.max(0,argCfg[2]));
			    else argVal = this.attrs['arg'+i+'val'];
			    if(isNaN(argVal)) argVal = 0;
			    break;
		    }
		    var argSize = Math.max(-1000,Math.min(1000,parseInt(argCfg[0])));
		    var argPad = '';
		    for(var j = argVal.length; j < Math.abs(argSize); j++) argPad += '&nbsp;';
		    if(argSize > 0) argVal = argPad+argVal; else argVal += argPad;
		    txtVal = txtVal.replace('%'+(i+1),argVal);
		}
		var txtVal1 = '';
		for(var j = 0; j < txtVal.length; j++)
		    if(txtVal[j] == '\n') txtVal1 += '<br />'; else txtVal1 += txtVal[j];
		//txtVal.replace(/\n/g,'<br />');
		//while(this.place.childNodes.length) this.place.removeChild(this.place.childNodes[0]);
		this.place.innerHTML = "<span style='"+spanStyle+"'>"+txtVal1+"</span>";
	    }
	    else this.place.innerHTML = "<img width='"+geomW+"px' height='"+geomH+"px' border='0' src='/"+MOD_ID+this.addr+
					"?com=obj&tm="+tmCnt+"&xSc="+xSc.toFixed(2)+"&ySc="+ySc.toFixed(2)+"'/>";

	    this.place.wdgLnk = this;
	    if(elWr) this.place.onclick = function() { setFocus(this.wdgLnk.addr); return false; };
	    else this.place.onclick = '';
	}
	else if(this.attrs['root'] == 'Media')
	{
	    if(this.attrs['backColor'] && getColor(this.attrs['backColor'],true))
		elStyle += 'background-color: '+getColor(this.attrs['backColor'])+'; ';
	    if(this.attrs['backImg']) elStyle += 'background-image: url(\'/'+MOD_ID+this.addr+'?com=res&val='+this.attrs['backImg']+'\'); ';
	    elStyle += 'border-style: solid; border-width: '+this.attrs['bordWidth']+'px; ';
	    if(this.attrs['bordColor']) elStyle += 'border-color: '+getColor(this.attrs['bordColor'])+'; ';
	    if(this.place.elWr != elWr || (parseInt(this.attrs['areas']) && this.place.childNodes.length <= 1) ||
					  (!parseInt(this.attrs['areas']) && this.place.childNodes.length > 1))
		while(this.place.childNodes.length) this.place.removeChild(this.place.childNodes[0]);

	    var toInit = !this.place.childNodes.length;
            var medObj = toInit ? this.place.ownerDocument.createElement('img') : this.place.childNodes[0];
            if(toInit || this.attrsMdf["src"])
            {
        	medObj.src = this.attrs['src'].length ? '/'+MOD_ID+this.addr+'?com=res&val='+this.attrs['src'] : '';
		medObj.hidden = !this.attrs['src'].length;
	    }
	    if(toInit || this.attrsMdf["fit"])
	    {
		if(this.attrs['fit'] == 1)
		{
		    medObj.width = geomW; medObj.height = geomH;
		    if(this.attrs['src'].length) medObj.src += "&size="+geomH;
		    medObj.onload = null;
		}
		else medObj.onload = function()
		{
		    var cWdth = this.width; var cHeight = this.height;
		    this.width = cWdth * this.wdgLnk.xScale(true);
		    this.height = cHeight * this.wdgLnk.yScale(true);
		}
	    }
	    if(elWr && (toInit || this.attrsMdf["areas"]))
	    {
		var mapObj = toInit ? this.place.ownerDocument.createElement('map') : this.place.childNodes[1];
		while(mapObj.childNodes.length) mapObj.removeChild(mapObj.childNodes[0]);
		for(var i = 0; i < parseInt(this.attrs['areas']); i++)
		{
		    var arObj = this.place.ownerDocument.createElement('area');
		    switch(parseInt(this.attrs['area'+i+'shp']))
		    {
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
	    if(toInit)
	    {
		medObj.wdgLnk = this;
		//> Disable drag mostly for FireFox
		medObj.onmousedown = function(e) { e = e?e:window.event; if(e.preventDefault) e.preventDefault(); }
		medObj.border = 0;
		this.place.appendChild(medObj);
		if(elWr)
		{
		    this.place.appendChild(mapObj);
		    mapObj.name = this.addr;
		    medObj.setAttribute('usemap','#'+this.addr);
		}
	    }
	}
	else if(this.attrs['root'] == 'FormEl' && !this.place.isModify)
	{
	    var elTp = parseInt(this.attrs['elType']);
	    if(this.attrsMdf['elType'] || this.place.elWr != elWr)
		while(this.place.childNodes.length) this.place.removeChild(this.place.childNodes[0]);

	    if(this.attrsMdf['font'])
	    {
		this.place.fontCfg = '';
		if(this.attrs['font'])
		{
		    var allFnt = this.attrs['font'].split(' ');
		    if(allFnt.length >= 3 && parseInt(allFnt[2])) this.place.fontCfg += 'bold ';
		    if(allFnt.length >= 4 && parseInt(allFnt[3])) this.place.fontCfg += 'italic ';
		    if(allFnt.length >= 2) this.place.fontCfg += (parseInt(allFnt[1])*Math.min(xSc,ySc)).toFixed(0)+'px ';
		    if(allFnt.length >= 1) this.place.fontCfg += allFnt[0].replace(/_/g,' ')+' ';
		}
	    }
	    var fntSz = Math.min(geomH,(getFont(this.attrs['font'],Math.min(xSc,ySc),2)*1.4).toFixed(0));
	    switch(elTp)
	    {
		case 0:	//Line edit
		    var toInit = !this.place.childNodes.length;
		    var formObj = toInit ? this.place.ownerDocument.createElement('input') : this.place.childNodes[0];
		    if(toInit || this.attrsMdf['geomZ']) formObj.tabIndex = parseInt(this.attrs['geomZ'])+1;
		    if(toInit || this.attrsMdf['geomH'] || this.attrsMdf['geomW'] || this.attrsMdf['font'])
		    {
			var geomWint = geomW-4;
			formObj.style.cssText = 'position: absolute; left: 0px; top: '+((geomH-fntSz)/2)+'px; width: '+geomWint+'px; '+
						'height: '+(fntSz-2)+'px; border: 1px solid black; font: '+this.place.fontCfg+'; padding: 1px;';
		    }
		    if(formObj.valSet && this.attrsMdf['value']) formObj.valSet(this.attrs['value']);
		    this.place.view = parseInt(this.attrs['view']);
		    this.place.cfg = this.attrs['cfg'];
		    if(!toInit) break;
		    formObj.wdgLnk = this;
		    formObj.disabled = !elWr;
		    this.place.appendChild(formObj);
		    if(elWr)
		    {
			switch(this.place.view)
			{
			    case 1:		//Combo
				var combImg = this.place.ownerDocument.createElement('img');
				combImg.src = '/'+MOD_ID+'/img_combar';
				combImg.style.cssText = 'position: absolute; left: '+(geomW-16)+'px; top: '+((geomH-fntSz)/2)+'px; width: 16px; height: '+fntSz+'px; cursor: pointer; ';
				this.place.appendChild(combImg);
				formObj.style.width = (geomWint-16)+'px';
				combImg.onclick = function()
				{
				    var formObj = this.parentNode.childNodes[0];
				    var combList = this.ownerDocument.getElementById('#combo');
				    if(!combList)
				    {
					combList = this.ownerDocument.createElement('div');
					combList.id = '#combo';
					combList.appendChild(this.ownerDocument.createElement('select'));
					combList.childNodes[0].size = '100';
					combList.childNodes[0].onchange = function()
					{
					    this.formObj.value = this.options[this.selectedIndex].value;
					    this.formObj.setModify(true);
					    this.parentNode.style.visibility = 'hidden';
					}
					combList.childNodes[0].onblur = function()
					{
					    this.parentNode.style.visibility = 'hidden';
					    this.parentNode.style.top = "-100px";
					}
					this.ownerDocument.body.appendChild(combList);
				    }
				    while(combList.childNodes[0].childNodes.length)
					combList.childNodes[0].removeChild(combList.childNodes[0].childNodes[0]);
				    var elLst = formObj.parentNode.cfg.split('\n');
				    for(var i = 0; i < elLst.length; i++)
				    {
					var optEl = this.ownerDocument.createElement('option');
					optEl.appendChild(this.ownerDocument.createTextNode(elLst[i]));
					if(formObj.valGet() == elLst[i]) optEl.defaultSelected = optEl.selected = true;
					combList.childNodes[0].appendChild(optEl);
				    }
				    if(combList.childNodes[0].childNodes.length)
				    {
					combList.style.cssText = 'position: absolute; visibility : visible; left: '+posGetX(formObj,true)+'px; '+
							         'top: '+(posGetY(formObj,true)+formObj.offsetHeight)+'px; width: '+formObj.offsetWidth+'px; '+
							         'height: '+Math.min(elLst.length*15,70)+'px; border: 0; ';
					combList.childNodes[0].style.cssText = 'width: '+formObj.offsetWidth+'px; height: '+Math.min(elLst.length*15,70)+'px; '+
									       'border-width: 2px; font: '+formObj.parentNode.fontCfg+'; padding: 1px;';
					combList.childNodes[0].formObj = formObj;
					combList.childNodes[0].focus();
				    }
				    return false;
				}
				break;
			    case 2: case 3:	//Integer, Real
				if(this.place.childNodes.length >= 2) break;
				var spinImg = this.place.ownerDocument.createElement('img');
				spinImg.src = '/'+MOD_ID+'/img_spinar';
				spinImg.style.cssText = 'position: absolute; left: '+(geomW-16)+'px; top: '+((geomH-fntSz)/2)+'px; width: 16px; height: '+fntSz+'px; cursor: pointer; ';
				spinImg.border = '0';
				formObj.style.width = (geomWint-16)+'px';
				spinImg.onclick = function(e)
				{
				    if(!e) e = window.event;
				    var formObj = this.parentNode.childNodes[0];
				    var argCfg = this.parentNode.cfg.split(':');
				    if((e.clientY-posGetY(this)) < fntSz/2)
					formObj.valSet(formObj.valGet()+((argCfg.length>2)?((this.parentNode.view==2)?parseInt(argCfg[2]):parseFloat(argCfg[2])):1));
				    else formObj.valSet(formObj.valGet()-((argCfg.length>2)?((this.parentNode.view==2)?parseInt(argCfg[2]):parseFloat(argCfg[2])):1));
				    formObj.setModify(true);
				    return false;
				}
				this.place.appendChild(spinImg);
				break;
			}
			formObj.onkeyup = function(e)
			{
			    if(!e) e = window.event;
			    if(this.modify() && e.keyCode == 13) this.chApply();
			    if(this.modify() && e.keyCode == 27) this.chEscape();
			    if(this.saveVal != this.value) this.setModify(true);
			}
			formObj.modify = function( )
			{ return (this.parentNode.childNodes[(this.parentNode.view>=1&&this.parentNode.view<=3)?2:1].style.visibility == 'visible'); }
			formObj.setModify = function(on)
			{
			    if(on && this.clearTm) this.clearTm = 5;
			    if(this.modify() == on) return;
			    var posOkImg = (this.parentNode.view>=1&&this.parentNode.view<=3)?2:1;
			    var okImg = this.parentNode.childNodes[posOkImg];
			    if(on)
			    {
				this.style.width = (parseInt(this.style.width)-16)+'px';
				if(posOkImg == 2)
				    this.parentNode.childNodes[1].style.left = (parseInt(this.parentNode.childNodes[1].style.left)-16)+'px';
				okImg.style.visibility = 'visible';
				this.wdgLnk.perUpdtEn(true); this.clearTm = 5;
			    }
			    else
			    {
				this.style.width = (parseInt(this.style.width)+16)+'px';
				if(posOkImg == 2)
				    this.parentNode.childNodes[1].style.left = (parseInt(this.parentNode.childNodes[1].style.left)+16)+'px';
				okImg.style.visibility = 'hidden';
				this.wdgLnk.perUpdtEn(false); this.clearTm = 0;
			    }
			    this.parentNode.isModify = on;
			}
		    }
		    formObj.valSet = function(val)
		    {
			switch(this.parentNode.view)
			{
			    case 0: case 1: this.value = val; break;
			    case 2:
				var argCfg = this.parentNode.cfg.split(':');
				this.value = ((argCfg.length>3)?argCfg[3]:'')+
				    Math.max((argCfg.length>1)?parseInt(argCfg[0]):0,Math.min((argCfg.length>1)?parseInt(argCfg[1]):100,parseInt(val)))+((argCfg.length>4)?argCfg[4]:'');
				break;
			    case 3:
				var argCfg = this.parentNode.cfg.split(':');
				this.value = ((argCfg.length>3)?argCfg[3]:'')+Math.max((argCfg.length>1)?parseFloat(argCfg[0]):0,Math.min((argCfg.length>1)?parseFloat(argCfg[1]):100,parseFloat(val))).toFixed((argCfg.length>5)?parseInt(argCfg[5]):2)+((argCfg.length>4)?argCfg[4]:'');
				break;
			    case 4:
				var rez = (this.parentNode.cfg.length) ? this.parentNode.cfg : 'hh:mm';
				var v = (Math.floor(val/3600)%(ap?12:24)).toString(10); rez = rez.replace('hh',(v.length==1)?'0'+v:v); rez = rez.replace('h',v);
				v = (Math.floor(val/60)%60).toString(10); rez = rez.replace('mm',(v.length==1)?'0'+v:v); rez = rez.replace('m',v);
				v = (val%60).toString(10); rez = rez.replace('ss',(v.length==1)?'0'+v:v); rez = rez.replace('s',v);
				if(rez.indexOf('ap') >= 0) { rez = rez.replace('ap',(val>=43200)?'pm':'am'); var ap = true; }
				if(rez.indexOf('AP') >= 0) { rez = rez.replace('AP',(val>=43200)?'PM':'AM'); var ap = true; }
				this.value = rez;
				break;
			    case 5:
				var rez = (this.parentNode.cfg.length) ? this.parentNode.cfg : 'dd.MM.yy';
				var dt = new Date(parseInt(val)*1000);
				var v = dt.getDate().toString(10); rez = rez.replace('dddd',(v.length==1)?'0'+v:v); rez = rez.replace('ddd',(v.length==1)?'0'+v:v); rez = rez.replace('dd',(v.length==1)?'0'+v:v); rez = rez.replace('d',v);
				v = (dt.getMonth()+1).toString(10); rez = rez.replace('MMMM',(v.length==1)?'0'+v:v); rez = rez.replace('MMM',(v.length==1)?'0'+v:v); rez = rez.replace('MM',(v.length==1)?'0'+v:v); rez = rez.replace('M',v);
				v = dt.getFullYear().toString(10); rez = rez.replace('yyyy',v); rez = rez.replace('yy',v.substr(2,2));
				this.value = rez;
				break;
			    case 6:
				var rez = (this.parentNode.cfg.length) ? this.parentNode.cfg : 'dd.MM.yy hh:mm';
				var dt = new Date(parseInt(val)*1000);
				var v = dt.getDate().toString(10); rez = rez.replace('dddd',(v.length==1)?'0'+v:v); rez = rez.replace('ddd',(v.length==1)?'0'+v:v); rez = rez.replace('dd',(v.length==1)?'0'+v:v); rez = rez.replace('d',v);
				v = (dt.getMonth()+1).toString(10); rez = rez.replace('MMMM',(v.length==1)?'0'+v:v); rez = rez.replace('MMM',(v.length==1)?'0'+v:v); rez = rez.replace('MM',(v.length==1)?'0'+v:v); rez = rez.replace('M',v);
				v = dt.getFullYear().toString(10); rez = rez.replace('yyyy',v); rez = rez.replace('yy',v.substr(2,2));
				v = dt.getHours().toString(10); rez = rez.replace('hh',(v.length==1)?'0'+v:v); rez = rez.replace('h',v);
				v = dt.getMinutes().toString(10); rez = rez.replace('mm',(v.length==1)?'0'+v:v); rez = rez.replace('m',v);
				v = dt.getSeconds().toString(10); rez = rez.replace('ss',(v.length==1)?'0'+v:v); rez = rez.replace('s',v);
				if(rez.indexOf('ap') >= 0)	{ rez = rez.replace('ap',(val>=43200)?'pm':'am'); var ap = true; }
				if(rez.indexOf('AP') >= 0)	{ rez = rez.replace('AP',(val>=43200)?'PM':'AM'); var ap = true; }
				this.value = rez;
				break;
			}
			this.saveVal = this.value;
			this.srcVal = val;
		    }
		    formObj.valGet = function( )
		    {
			switch(this.parentNode.view)
			{
			    case 0: case 1: return this.value;
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
				if(cfg.indexOf('ap') >= 0 || cfg.indexOf('AP') >= 0)
				{
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
				while(true)
				{
				    if(i >= cfg.length || cfg.charAt(i) == 'h' || cfg.charAt(i) == 'm' || cfg.charAt(i) == 's')
				    {
					var space = lenS ? noSpace(cfg.substr(i-lenS,lenS)) : '';
					if(lenS && !space.length) space = ' ';
					if((space.length && lstS != '') || i >= cfg.length)
					{
					    if(space.length && rez.indexOf(space) == -1) return 0;
					    var val = parseInt(space.length?rez.substr(0,rez.indexOf(space)):rez,10);
					    rez = rez.substr(rez.indexOf(space));
					    if(lstS == 'h')	hour = val;
					    else if(lstS == 'm')min = val;
					    else if(lstS == 's')sec = val;
					    if(i >= cfg.length) break;
					}
					if(space.length)
					{
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
				while(true)
				{
				    if(i >= cfg.length || cfg.charAt(i) == 'd' || cfg.charAt(i) == 'M' || cfg.charAt(i) == 'y')
				    {
					var space = lenS ? noSpace(cfg.substr(i-lenS,lenS)) : '';
					if(lenS && !space.length) space = ' ';
					if((space.length && lstS != '') || i >= cfg.length)
					{
					    if(space.length && rez.indexOf(space) == -1) return 0;
					    var val = parseInt(space.length?rez.substr(0,rez.indexOf(space)):rez,10);
					    rez = rez.substr(rez.indexOf(space));
					    if(lstS == 'd')	dt.setDate(val);
					    else if(lstS == 'M')dt.setMonth(val-1);
					    else if(lstS == 'y')dt.setFullYear((val<100)?2000+val:val);
					    if(i >= cfg.length) break;
					}
					if(space.length)
					{
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
				if(cfg.indexOf('ap') >= 0 || cfg.indexOf('AP') >= 0)
				{
				    cfg = cfg.replace('ap','');
				    cfg = cfg.replace('AP','');
				    rez = rez.replace('am','');
				    rez = rez.replace('AM','');
				    rez = rez.replace('pm','');
				    rez = rez.replace('PM','');
				}
				var dt = new Date(this.srcVal*1000);
				var i = 0; var lstS = ''; var lenS = 0;
				while(true)
				{
				    if(i >= cfg.length || cfg.charAt(i) == 'd' || cfg.charAt(i) == 'M' || cfg.charAt(i) == 'y' ||
					cfg.charAt(i) == 'h' || cfg.charAt(i) == 'm' || cfg.charAt(i) == 's')
				    {
					var space = lenS ? noSpace(cfg.substr(i-lenS,lenS)) : '';
					if(lenS && !space.length) space = ' ';
					if((space.length && lstS != '') || i >= cfg.length)
					{
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
					if(space.length)
					{
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
		    formObj.chApply = function( )
		    {
			var val = this.valGet();
			this.valSet(val);
			var okImg = this.parentNode.childNodes[(this.parentNode.view>=1&&this.parentNode.view<=3)?2:1];
			this.setModify(false);
			var attrs = new Object(); attrs.value = val; attrs.event = 'ws_LnAccept';
			setWAttrs(this.wdgLnk.addr,attrs);
		    }
		    formObj.chEscape = function( )
		    {
			this.value = this.saveVal;
			var okImg = this.parentNode.childNodes[(this.parentNode.view>=1&&this.parentNode.view<=3)?2:1];
			this.setModify(false);
		    }
		    var okImg = this.place.ownerDocument.createElement('img');
		    okImg.src = '/'+MOD_ID+'/img_button_ok';
		    okImg.style.cssText = 'visibility: hidden; position: absolute; left: '+(geomW-16)+'px; top: '+((geomH-16)/2)+'px; width: 16px; height: 16px; cursor: pointer;';
		    okImg.onclick = function() { this.parentNode.childNodes[0].chApply(); return false; };
		    this.place.appendChild(okImg);
		    formObj.valSet(this.attrs['value']);
		    break;
		case 1:	//Text edit
		    var toInit = !this.place.childNodes.length;
		    var formObj = toInit ? this.place.ownerDocument.createElement('textarea') : this.place.childNodes[0];
		    if(toInit || this.attrsMdf['geomZ']) formObj.tabIndex = parseInt(this.attrs['geomZ'])+1;
		    if(toInit || this.attrsMdf['geomW'] || this.attrsMdf['geomH'] || this.attrsMdf['font'])
			formObj.style.cssText = 'width: '+(geomW-5)+'px; height: '+(geomH-5)+'px; border: 1px solid black; font: '+this.place.fontCfg+'; padding: 1px;';
		    if(this.attrsMdf['value']) formObj.saveVal = formObj.value = this.attrs['value'];
		    if(!toInit) break;
		    formObj.disabled = !elWr;
		    formObj.wdgLnk = this;
		    formObj.appendChild(this.place.ownerDocument.createTextNode(this.attrs['value']));
		    formObj.onkeyup = function( ) { if(this.saveVal != this.value) this.setModify(true); };
		    formObj.modify = function( ) { return (this.parentNode.childNodes[1].style.visibility == 'visible'); }
		    formObj.setModify = function(on)
		    {
			if(this.modify() == on) return;
			if(on)
			{
			    this.style.height = (parseInt(this.style.height)-16)+'px';
			    this.parentNode.childNodes[1].style.visibility = this.parentNode.childNodes[2].style.visibility = 'visible';
			}
			else
			{
			    this.style.height = (parseInt(this.style.height)+16)+'px';
			    this.parentNode.childNodes[1].style.visibility = this.parentNode.childNodes[2].style.visibility = 'hidden';
			}
		    }
		    var okImg = this.place.ownerDocument.createElement('img');
		    okImg.src = '/'+MOD_ID+'/img_button_ok';
		    okImg.style.cssText = 'visibility: hidden; position: absolute; left: '+(geomW-35)+'px; '+
					  'top: '+(geomH-16)+'px; width: 16px; height: 16px; cursor: pointer;';
		    okImg.onclick = function()
		    {
			var attrs = new Object();
			attrs.value = this.parentNode.childNodes[0].value; attrs.event = 'ws_TxtAccept';
			setWAttrs(this.parentNode.childNodes[0].wdgLnk.addr,attrs);
			this.parentNode.childNodes[0].setModify(false);
			return false;
		    };
		    var cancelImg = this.place.ownerDocument.createElement('img');
		    cancelImg.src = '/'+MOD_ID+'/img_button_cancel';
		    cancelImg.style.cssText = 'visibility: hidden; position: absolute; left: '+(geomW-16)+'px; top: '+(geomH-16)+'px; width: 16px; height: 16px; cursor: pointer;';
		    cancelImg.onclick = function()
		    {
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
		    if(toInit || this.attrsMdf['geomH'] || this.attrsMdf['geomW'] || this.attrsMdf['font'])
			tblCell.style.cssText = 'position: absolute; top: '+((geomH-15)/2)+'px; width: '+geomW+'px; '+
					    'height: '+Math.min(geomH,15)+'px; text-align: left; font: '+this.place.fontCfg+'; ';
		    var formObj = tblCell.childNodes.length ? tblCell.childNodes[0] : this.place.ownerDocument.createElement('input');
		    if(toInit || this.attrsMdf['geomZ']) formObj.tabIndex = parseInt(this.attrs['geomZ'])+1;
		    if(toInit || this.attrsMdf['value']) formObj.checked = parseInt(this.attrs['value']);
		    if(!toInit) break;
		    formObj.type = 'checkbox';
		    formObj.disabled = !elWr;
		    formObj.wdgLnk = this;
		    formObj.onclick = function( )
		    {
			//console.log(this.addr+": TEST 00: ChkChange="+this.checked);
			var attrs = new Object();
			attrs.value = this.checked ? '1' : '0';	attrs.event = 'ws_ChkChange';
			setWAttrs(this.wdgLnk.addr,attrs);
			return true;
		    }
		    tblCell.appendChild(formObj);
		    tblCell.appendChild(this.place.ownerDocument.createTextNode(this.attrs['name']));
		    this.place.appendChild(tblCell);
		    break;
		case 3:	//Button
		    var formObj;
		    var iconImg = this.attrs['img'];
		    this.place.checkable = (parseInt(this.attrs['mode']) == 1);
		    var custBut = (iconImg || this.place.checkable);
		    if(custBut != this.place.custBut)
			while(this.place.childNodes.length) this.place.removeChild(this.place.childNodes[0]);
		    this.place.custBut = custBut;
		    var toInit = !this.place.childNodes.length;
		    if(custBut)
		    {
			formObj =  toInit ? this.place.ownerDocument.createElement('div') : this.place.childNodes[0];
			//>> Container widget style
			elStyle += 'border-style: '+((this.place.checkable && parseInt(this.attrs['value']))?'inset; ':'outset; ')+
						    (elWr?'cursor: pointer; ':'')+'border-width: 2px; ';
			if(this.attrs['colorText'])
			    elStyle += 'color: '+(elWr ? getColor(this.attrs['colorText']) : 'silver')+'; ';
			if(elWr && this.attrs['color']) elStyle += 'background-color: '+getColor(this.attrs['color'])+'; ';
			else elStyle += 'background-color: snow; ';

			if(toInit || this.attrsMdf['font']) formObj.style.font = this.place.fontCfg;
			var imgObj = formObj.childNodes.length ? formObj.childNodes[0] : this.place.ownerDocument.createElement('img');
			var spanObj = formObj.childNodes.length ? formObj.childNodes[1] : this.place.ownerDocument.createElement('span');
			if(toInit || this.attrsMdf['img'] || this.attrsMdf['name'])
			{
			    imgObj.hidden = !this.attrs['img'].length;
			    if(!imgObj.hidden)
			    {
				imgObj.src = "/"+MOD_ID+this.addr+"?com=res&val="+this.attrs['img']+"&size="+Math.min(geomW-6,geomH-6)+(!elWr?"&filtr=unact":"");
				imgObj.width = Math.min(geomW-6,geomH-6);
				imgObj.height = Math.min(geomW-6,geomH-6);
				imgObj.float = 'left';
				imgObj.style.marginRight = this.attrs['name'].length ? "2px" : "0px";
			    }
			}
			if(toInit || this.attrsMdf['name'])
			{
			    spanObj.disabled = !this.attrs['name'].length;
			    //while(spanObj.childNodes.length) spanObj.removeChild(spanObj.childNodes[0]);
			    txtVal1 = '';
			    for(var j = 0; j < this.attrs['name'].length; j++)
				txtVal1 += this.attrs['name'].substr(j,2) != '\\n' ? strEncode(this.attrs['name'][j]) : '<br />';
			    spanObj.innerHTML = txtVal1;
			}

			geomW -= 4; geomH -= 4;

			if(elWr)
			{
			    this.mouseup[this.mouseup.length] = function(e,el)
			    {
				if(el.checkable) return;
				el.style.borderStyle = 'outset'; setWAttrs(el.wdgLnk.addr,'event','ws_BtRelease');
			    };
			    this.mousedown[this.mousedown.length] = function(e,el)
			    {
				if(el.checkable) return;
				el.style.borderStyle = "inset"; setWAttrs(el.wdgLnk.addr,'event','ws_BtPress');
			    };
			}

			if(!toInit) break;
			formObj.className = 'vertalign';
			//> Disable drag mostly for FireFox
			imgObj.onmousedown = function(e) { e = e?e:window.event; if(e.preventDefault) e.preventDefault(); }
			if(elWr)
			{
			    this.place.onmouseout = function()
			    {
				if(this.checkable || this.style.borderStyle == 'outset') return false;
				this.style.borderStyle = 'outset'; setWAttrs(this.wdgLnk.addr,'event','ws_BtRelease');
				return false;
			    };
			    this.place.onclick = function()
			    {
				if(!this.checkable) return false;
				var attrs = new Object();
				if(this.style.borderLeftStyle == 'outset')
				{ attrs.value = '1'; this.style.borderStyle = 'inset'; setWAttrs(this.wdgLnk.addr,'event','ws_BtPress'); }
				else { attrs.value = '0'; this.style.borderStyle = 'outset'; setWAttrs(this.wdgLnk.addr,'event','ws_BtRelease'); }
				setWAttrs(this.wdgLnk.addr,'event','ws_BtToggleChange');
				setWAttrs(this.wdgLnk.addr,'value',attrs.value);
				return false;
			    };
			    this.place.wdgLnk = this;
			}
			formObj.appendChild(imgObj);
			formObj.appendChild(spanObj);
			formObj.style.width = geomW+'px'; formObj.style.height = geomH+'px';
			this.place.appendChild(formObj);
		    }
		    else
		    {
			formObj = toInit ? this.place.ownerDocument.createElement('input') : this.place.childNodes[0];
			if(toInit || this.attrsMdf['geomZ'])	formObj.tabIndex = parseInt(this.attrs['geomZ'])+1;
			if(toInit || this.attrsMdf['font'])	formObj.style.font = this.place.fontCfg;
			if(toInit || this.attrsMdf['name'])	formObj.value = this.attrs['name'].replace('\\n','\n');
			if(toInit || this.attrsMdf['color'])	formObj.style.backgroundColor = getColor(this.attrs['color']);
			if(toInit || this.attrsMdf['colorText'])formObj.style.color = getColor(this.attrs['colorText']);
			this.mouseup[this.mouseup.length] = function(e,el)	{ setWAttrs(el.wdgLnk.addr,'event','ws_BtRelease'); };
			this.mousedown[this.mousedown.length] = function(e,el)	{ setWAttrs(el.wdgLnk.addr,'event','ws_BtPress'); };
			if(!toInit) break;
			formObj.style.cursor = elWr ? 'pointer' : '';
			formObj.disabled = !elWr;
			formObj.type = 'button';
			formObj.wdgLnk = this;
			formObj.style.width = geomW+'px'; formObj.style.height = geomH+'px';
			this.place.appendChild(formObj);
			formObj.value = this.attrs['name'].replace('\\n','\n');	//Need for Opera after place to DOM
		    }
		    break;
		case 4: case 5:	//Combo box, List
		    var toInit = !this.place.childNodes.length;
		    var formObj = toInit ? this.place.ownerDocument.createElement('select') : this.place.childNodes[0];
		    if(toInit || this.attrsMdf['geomZ']) formObj.tabIndex = parseInt(this.attrs['geomZ'])+1;
		    if(toInit || this.attrsMdf['geomW'] || this.attrsMdf['geomH'] || this.attrsMdf['font'])
			formObj.style.cssText = 'position: absolute; left: 0px; top: '+((elTp==4)?(geomH-fntSz)/2:0)+'px; '+
					    'height: '+((elTp==4)?fntSz:(geomH-4))+'px; width: '+geomW+'px; '+
					    'border: 1px solid black; font: '+this.place.fontCfg+'; padding: 1px; ';
		    if(this.attrsMdf['items'] || this.attrsMdf['value'])
		    {
			while(formObj.childNodes.length) formObj.removeChild(formObj.childNodes[0]);
			var selVal = this.attrs['value'];
			var elLst = this.attrs['items'].split('\n');
			var selOk = false;
			for(var i = 0; i < elLst.length; i++)
			{
			    var optEl = this.place.ownerDocument.createElement('option');
			    optEl.appendChild(this.place.ownerDocument.createTextNode(elLst[i]));
			    if(selVal == elLst[i]) selOk = optEl.defaultSelected=optEl.selected = true;
			    formObj.appendChild(optEl);
			}
			if(!selOk && elTp == 4)
			{
			    var optEl = this.place.ownerDocument.createElement('option');
			    optEl.textContent = selVal;
			    optEl.selected = optEl.defaultSelected = true;
			    formObj.appendChild(optEl);
			}
		    }
		    if(!toInit) break;
		    formObj.disabled = !elWr;
		    formObj.wdgLnk = this;
		    //f(elTp == 5) formObj.setAttribute('size',100);
		    if(elTp == 4)
			formObj.onchange = function( )
			{
			    var attrs = new Object();
			    attrs.value = this.options[this.selectedIndex].value; attrs.event = 'ws_CombChange';
			    setWAttrs(this.wdgLnk.addr,attrs);
			}
		    else
		    {
			formObj.size = 100;
			formObj.onchange = function( )
			{
			    var attrs = new Object();
			    attrs.value = this.options[this.selectedIndex].value; attrs.event = 'ws_ListChange';
			    setWAttrs(this.wdgLnk.addr,attrs);
			}
		    }
		    this.place.appendChild(formObj);
		    break;
	    }
	}
	else if(this.attrs['root'] == 'Diagram')
	{
	    if(this.attrs['backColor'] && getColor(this.attrs['backColor'],true))
		elStyle += 'background-color: '+getColor(this.attrs['backColor'])+'; ';
	    if(this.attrs['backImg']) elStyle += 'background-image: url(\'/'+MOD_ID+this.addr+'?com=res&val='+this.attrs['backImg']+'\'); ';
	    elStyle += 'border-style: solid; border-width: '+this.attrs['bordWidth']+'px; ';
	    if(this.attrs['bordColor']) elStyle += 'border-color: '+getColor(this.attrs['bordColor'])+'; ';
	    var anchObj = this.place.childNodes[0];
	    if(!anchObj)
	    {
		anchObj = this.place.ownerDocument.createElement('a');
		anchObj.wdgLnk = this;
		var dgrObj = this.place.ownerDocument.createElement('img');
		dgrObj.border = 0;
		anchObj.appendChild(dgrObj); this.place.appendChild(anchObj);
	    }
	    anchObj.isActive = elWr;
	    anchObj.href = '#';
	    anchObj.tabIndex = parseInt(this.attrs['geomZ'])+1;
	    anchObj.onfocus = function( ) { if(this.isActive) setFocus(this.wdgLnk.addr); }
	    anchObj.onkeydown = function(e) { if(this.isActive) setWAttrs(this.wdgLnk.addr,'event','key_pres'+evKeyGet(e?e:window.event)); }
	    anchObj.onkeyup = function(e) { if(this.isActive) setWAttrs(this.wdgLnk.addr,'event','key_rels'+evKeyGet(e?e:window.even)); }
	    anchObj.onclick = function(e)
	    {
		if(!this.isActive) return false;
		if(!e) e = window.event;
		servSet(this.wdgLnk.addr,'com=obj&sub=point&x='+(e.offsetX?e.offsetX:(e.clientX-posGetX(this)))+
							  '&y='+(e.offsetY?e.offsetY:(e.clientY-posGetY(this)))+
							  '&key='+evMouseGet(e),'');
		setFocus(this.wdgLnk.addr);
		return false;
	    }
	    var dgrObj = anchObj.childNodes[0];
	    dgrObj.isLoad = false;
	    dgrObj.onload = function( )	{ this.isLoad = true; }
	    dgrObj.src = '/'+MOD_ID+this.addr+'?com=obj&tm='+tmCnt+'&xSc='+xSc.toFixed(2)+'&ySc='+ySc.toFixed(2);
	    //> Disable drag mostly for FireFox
	    dgrObj.onmousedown = function(e) { e = e?e:window.event; if(e.preventDefault) e.preventDefault(); }
	    this.perUpdtEn(this.isEnabled() && parseInt(this.attrs['trcPer']));
	}
	else if(this.attrs['root'] == 'Protocol')
	{
	    if(this.attrs['backColor'] && getColor(this.attrs['backColor'],true))
		elStyle += 'background-color: '+getColor(this.attrs['backColor'])+'; ';
	    else elStyle += 'background-color: white; ';
	    if(this.attrs['backImg'])	elStyle += 'background-image: url(\'/'+MOD_ID+this.addr+'?com=res&val='+this.attrs['backImg']+'\'); ';
	    elStyle += 'border: 1px solid black; overflow: auto; padding: 2px; text-align: left; ';
	    geomW -= 6; geomH -= 6;

	    this.wFont = getFont(this.attrs['font'],Math.min(xSc,ySc));

	    if(!this.place.firstChild)
	    {
		this.place.appendChild(document.createElement('table'));
		this.place.firstChild.setAttribute('width','100%');
		this.place.firstChild.wdgLnk = this;
		this.place.firstChild.isActive = elWr;
		this.place.firstChild.onclick = function(e)
		{
		    if(this.isActive) setFocus(this.wdgLnk.addr);
		    return false;
		}
		this.place.firstChild.className='prot';
		this.loadData = function( )
		{
		    if(!this.tmPrev) this.tmPrev = 0;
		    if(!this.tmGrndPrev) this.tmGrndPrev = 0;

		    var tblB = this.place.firstChild;

		    //> Get archive parameters
		    var tTime = parseInt(this.attrs['time']);
		    var tTimeCurent = false;
		    if(!tTime)
		    {
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
			var rowEl = document.createElement('tr');
			tblB.appendChild(rowEl);
			var colCfg = '';
			var clm = this.curCols.split(';');
			for(var c_off = 0; c_off < clm.length; c_off++)
			{
			    if(!parseInt(this.attrs['headVis'])) colCfg += "<th/>";
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
		    else
		    {
			if(!this.arhBeg || !this.arhEnd || !tTime || tTime > this.arhEnd || tTimeCurent)
			{
			    var rez = servSet('/Archive/%2fserv%2fmess','com=com',"<info arch='"+this.curArch+"'/>",true);
			    if(!rez || parseInt(rez.getAttribute('rez')) != 0)	this.arhBeg = this.arhEnd = 0;
			    else
			    {
				this.arhBeg = parseInt(rez.getAttribute('beg'));
				this.arhEnd = parseInt(rez.getAttribute('end'));
				if(!tTime) { tTime = this.arhEnd; tTimeGrnd += tTime; }
			    }
			}
			if(!this.arhBeg || !this.arhEnd) return;

			//> Correct request to archive border
			tTime = tTimeCurent ? this.arhEnd : Math.min(tTime,this.arhEnd);
			tTimeGrnd = Math.max(tTimeGrnd, this.arhBeg);
		    }

		    //> Clear data at time error
		    var valEnd = 0; var valBeg = 0;
		    while(this.messList.length && (valEnd=this.messList[0][0]) > tTime) this.messList.shift();
		    while(this.messList.length && (valBeg=this.messList[this.messList.length-1][0]) < tTimeGrnd) this.messList.pop();

		    if(!this.messList.length) valEnd = valBeg = 0;

		    if(tTime < tTimeGrnd || (tTime < valEnd && tTimeGrnd > valBeg))
		    {
			this.messList = new Array();
			while(tblB.childNodes.length > 1) tblB.removeChild(tblB.lastChild);
			valEnd = valBeg = 0;
			return;
		    }

		    //> Correct request to present data
		    var toUp = false, isDtChang = false;
		    if(srcTime > this.tmPrev) { if(valEnd) tTimeGrnd = valEnd; toUp = true; }
		    else if((srcTime-parseInt(this.attrs['tSize'])) < this.tmGrndPrev) { if(valBeg) tTime = valBeg-1; }
		    else return;
		    this.tmPrev = srcTime;
		    this.tmGrndPrev = srcTime-parseInt(this.attrs['tSize']);

		    var rez = servSet('/Archive/%2fserv%2fmess','com=com',
			"<get arch='"+this.curArch+"' tm='"+tTime+"' tm_grnd='"+tTimeGrnd+"' cat='"+this.curTmpl+"' lev='"+this.curLev+"' />",true);
		    if(!rez || parseInt(rez.getAttribute('rez')) != 0) return;

		    if(toUp)
			for(var i_req = 0; i_req < rez.childNodes.length; i_req++)
			{
			    var rcd = rez.childNodes[i_req];
			    var mess = new Array(parseInt(rcd.getAttribute('time')), parseInt(rcd.getAttribute('utime')),
						    Math.abs(parseInt(rcd.getAttribute('lev'))), rcd.getAttribute('cat'), nodeText(rcd));

			    //>> Check for dublicates
			    var isDbl = false;
			    for(var i_p = 0; !isDbl && i_p < this.messList.length; i_p++)
			    {
				if(mess[0] > this.messList[0][0] && i_p) break;
				if(this.messList[i_p][1] == mess[1] && this.messList[i_p][2] == mess[2] &&
				    this.messList[i_p][3] == mess[3] && this.messList[i_p][4] == mess[4]) isDbl = true;
			    }
			    if(isDbl) continue;

			    //>> Insert new row
			    this.messList.unshift(mess);
			    isDtChang = true;
			}
		    else
			for(var i_req = rez.childNodes.length-1; i_req >= 0; i_req--)
			{
			    var rcd = rez.childNodes[i_req];
			    var mess = new Array(parseInt(rcd.getAttribute('time')), parseInt(rcd.getAttribute('utime')),
						    Math.abs(parseInt(rcd.getAttribute('lev'))), rcd.getAttribute('cat'), nodeText(rcd));

			    //>> Check for dublicates
			    var isDbl = false;
			    for(var i_p = this.messList.length-1; !isDbl && i_p >= 0; i_p--)
			    {
				if(mess[0] < this.messList[this.messList.length-1][0] && i_p < (this.messList.length-1)) break;
				if(this.messList[i_p][1] == mess[1] && this.messList[i_p][2] == mess[2] &&
				    this.messList[i_p][3] == mess[3] && this.messList[i_p][4] == mess[4]) isDbl = true;
			    }
			    if(isDbl) continue;

			    //>> Insert new row
			    this.messList.push(mess);
			    isDtChang = true;
			}

		    if((tblB.childNodes.length-1) == this.messList.length && !isDtChang) return;

		    //> Sort data
		    var sortIts = new Array();
		    switch(parseInt(this.attrs['viewOrd'])&0x3)
		    {
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

		    //> Write to table
		    for(var i_m = 0; i_m < sortIts.length; i_m++)
		    {
			var rowEl = (i_m>=(tblB.childNodes.length-1)) ? document.createElement('tr') : tblB.childNodes[i_m+1];
			var elPos = parseInt(sortIts[i_m].slice(sortIts[i_m].lastIndexOf(':')+1));

			var rowFnt = this.wFont;
			var rowColor = '';

			//>> Check properties
			for(var i_it = 0, lst_lev = -1; i_it < parseInt(this.attrs['itProp']); i_it++)
			{
			    var prpLev = parseInt(this.attrs['it'+i_it+'lev']);
			    if(this.messList[elPos][2] >= prpLev && prpLev > lst_lev &&
				    chkPattern(this.messList[elPos][3],this.attrs['it'+i_it+'tmpl']))
			    {
				var tRowFnt = getFont(this.attrs['it'+i_it+'fnt'],Math.min(xSc,ySc));
				if(tRowFnt.length) rowFnt = tRowFnt;
				rowColor = 'background-color: '+getColor(this.attrs['it'+i_it+'color'])+'; ';
				if(this.messList[elPos][2] == parseInt(this.attrs['it'+i_it+'lev'])) break;
				lst_lev = prpLev;
			    }
			}
			//if( rowColor.length ) fclr = ((0.3*clr.red()+0.59*clr.green()+0.11*clr.blue()) > 128) ? Qt::black : Qt::white;
			rowEl.style.cssText = rowFnt+rowColor;

			for(var i_cel = 0; i_cel < tblB.childNodes[0].childNodes.length; i_cel++)
			{
			    var celEl = (i_cel>=rowEl.childNodes.length) ? document.createElement('td') : rowEl.childNodes[i_cel];
			    if(this.col_pos == i_cel) { setNodeText(celEl,i_m); celEl.style.cssText += ' text-align: center; '; }
			    else if(this.col_tm == i_cel)
			    {
				var dt = new Date(this.messList[elPos][0]*1000);
				setNodeText(celEl,dt.getDate()+'.'+(dt.getMonth()+1)+'.'+dt.getFullYear()+' '+dt.getHours()+':'+
				    ((dt.getMinutes()<10)?('0'+dt.getMinutes()):dt.getMinutes())+':'+((dt.getSeconds()<10)?('0'+dt.getSeconds()):dt.getSeconds()));
			    }
			    else if(this.col_utm == i_cel)	setNodeText(celEl,this.messList[elPos][1]);
			    else if(this.col_lev == i_cel)	setNodeText(celEl,this.messList[elPos][2]);
			    else if(this.col_cat == i_cel)	setNodeText(celEl,this.messList[elPos][3]);
			    else if(this.col_mess == i_cel)	setNodeText(celEl,this.messList[elPos][4]);
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
	else if(this.attrs['root'] == 'Document')
	{
	    elStyle += 'background-color: white; ';

	    if(this.attrsMdf["style"] || this.attrsMdf["font"] ||
		(this.attrsMdf["doc"] && this.attrs["doc"].length) || (this.attrsMdf["tmpl"] && !this.attrs["doc"].length))
	    {
		this.wFont = getFont(this.attrs['font'],Math.min(xSc,ySc),1);

		var ifrmObj = this.place.childNodes[0];
		try
		{
		    if(ifrmObj && (!ifrmObj.contentDocument || document.URL != ifrmObj.contentDocument.URL))
		    { this.place.removeChild(ifrmObj); ifrmObj = null; }
		}catch(e) { this.place.removeChild(ifrmObj); ifrmObj = null; }
		if(!ifrmObj)
		{
		    ifrmObj = this.place.ownerDocument.createElement('iframe');
		    this.place.appendChild(ifrmObj);
		}

		ifrmObj.style.cssText = 'width: '+(geomW-14)+'px; height: '+(geomH-14)+'px; border-style: ridge; border-width: 2px; padding: 5px;';
		this.perUpdtEn(true);
	    }
	}
	elStyle += 'width: '+geomW+'px; height: '+geomH+'px; z-index: '+this.attrs['geomZ']+'; margin: '+elMargin+'px; ';
	if(elStyle != this.place.style.cssText)	this.place.style.cssText = elStyle;

	//> Generic mouse events process
	if(elWr)
	{
	    //console.log(this.addr+": TEST 03: mDownCnt="+this.mDownCnt);
	    this.mousedown[this.mousedown.length] = function(e,el)
	    {
		if(!e) e = window.event;
		setWAttrs(el.wdgLnk.addr,'event','key_mousePres'+evMouseGet(e));
	    }
	    this.mouseup[this.mouseup.length] = function(e,el)
	    {
		if(!e) e = window.event;
		setWAttrs(el.wdgLnk.addr,'event','key_mouseRels'+evMouseGet(e));
	    }
	    this.place.ondblclick = function(e) { setWAttrs(this.wdgLnk.addr,'event','key_mouseDblClick'); return false; }
	}
	else this.place.ondblclick = '';

	//> Context menu setup
	if(elWr && this.attrs['contextMenu'].length)
	{
	    var ctxEv = function(e,el)
	    {
		var cel = el ? el : this;
		if(!e) e = window.event;
		if((isKonq || isOpera) && evMouseGet(e) != 'Right') return true;
		var popUpMenu = getPopup();
		var optEl = '';
		var cntxEls = cel.wdgLnk.attrs['contextMenu'].split('\n');
		for(var i_ce = 0; i_ce < cntxEls.length; i_ce++)
		    optEl += "<option sign='"+cntxEls[i_ce].split(':')[1]+"'>"+cntxEls[i_ce].split(':')[0]+"</option>";
		popUpMenu.childNodes[0].innerHTML = optEl;
		if(popUpMenu.childNodes[0].childNodes.length)
		{
		    popUpMenu.childNodes[0].wdgLnk = cel.wdgLnk;
		    popUpMenu.childNodes[0].size = Math.max(3,popUpMenu.childNodes[0].childNodes.length);
		    popUpMenu.style.cssText = 'visibility: visible; left: '+(e.clientX+window.pageXOffset)+'px; top: '+(e.clientY+window.pageYOffset)+'px;';
		    popUpMenu.childNodes[0].selectedIndex = -1;
		    popUpMenu.childNodes[0].onclick = function()
		    {
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
	else this.place.oncontextmenu = null;

	//> Common mouse events process
	if(this.mousedown.length)
	    this.place.onmousedown = function(e)
	    {
		for(var i_on = 0; i_on < this.wdgLnk.mousedown.length; i_on++)
		this.wdgLnk.mousedown[i_on](e,this);
		return true;
	    }
	else delete this.mousedown;
	if(this.mouseup.length)
	    this.place.onmouseup = function(e)
	    {
		for(var i_on = 0; i_on < this.wdgLnk.mouseup.length; i_on++)
		    this.wdgLnk.mouseup[i_on](e,this);
		return true;
	    }
	else delete this.mouseup;

	this.place.elWr = elWr;
    }
    if(margBrdUpd || this.attrsMdf["geomXsc"] || this.attrsMdf["geomYsc"]) for(var i in this.wdgs) this.wdgs[i].makeEl();
    this.place.setAttribute('title',this.attrs['tipTool']);
    this.place.onmouseover = function() { if( this.wdgLnk.attrs['tipStatus'] ) setStatus(this.wdgLnk.attrs['tipStatus'],10000); };

    //> Delete child widgets check
    if(FullTree && pgBr)
	for(var i in this.wdgs)
	{
	    var j;
	    for(j = 0; j < pgBr.childNodes.length; j++)
		if(pgBr.childNodes[j].nodeName == 'w' && pgBr.childNodes[j].getAttribute('id') == i)
		    break;
	    if(j >= pgBr.childNodes.length)
	    {
		this.wdgs[i].place.parentNode.removeChild(this.wdgs[i].place);
		delete this.wdgs[i];
	    }
	}

    //> Child widgets process
    if(pgBr && !inclPg && parseInt(this.attrs['perm'])&SEC_RD)
	for(var j = 0; j < pgBr.childNodes.length; j++)
	{
	    if(pgBr.childNodes[j].nodeName != 'w') continue;
	    var chEl = pgBr.childNodes[j].getAttribute('id');
	    if(this.wdgs[chEl]) this.wdgs[chEl].makeEl(pgBr.childNodes[j], false, full, FullTree);
	    else
	    {
		var wdgO = new pwDescr(this.addr+'/wdg_'+chEl,false,this);
		wdgO.place = this.place.ownerDocument.createElement('div');
		this.place.appendChild(wdgO.place);
		this.wdgs[chEl] = wdgO;
		wdgO.makeEl(full?pgBr.childNodes[j]:servGet(wdgO.addr,'com=attrsBr'));
		//wdgO.makeEl(pgBr.childNodes[j]);		//!!!! Need full
	    }
	}
}

function pwClean( )
{
    //> Periodic update disable
    this.perUpdtEn(false);
    //> Pages recursively clean
    for(var i in this.pages) this.pages[i].pwClean();
    this.pages = new Object();
    //> Widgets recursively clean
    for(var i in this.wdgs) this.wdgs[i].pwClean();
    this.wdgs = new Object();
}

function perUpdtEn( en )
{
    if(this.attrs['root'] == 'Diagram' || this.attrs['root'] == 'Protocol')
    {
	if(en && this.isEnabled() && !perUpdtWdgs[this.addr] && parseInt(this.attrs['trcPer']))	perUpdtWdgs[this.addr] = this;
	if(!en && perUpdtWdgs[this.addr]) delete perUpdtWdgs[this.addr];
    }
    else if(this.attrs['root'] == 'Document' || this.attrs['root'] == 'FormEl')
    {
	if(en) perUpdtWdgs[this.addr] = this;
	else delete perUpdtWdgs[this.addr];
    }
    for(var i in this.wdgs) this.wdgs[i].perUpdtEn(en);
}

function perUpdt( )
{
    if(this.attrs['root'] == 'FormEl' && this.place.childNodes.length && this.place.childNodes[0].clearTm &&
	    (this.place.childNodes[0].clearTm-=prcTm) <= 0)
	this.place.childNodes[0].chEscape();
    else if(this.attrs['root'] == 'Diagram' && (this.updCntr-=prcTm) <= 0)
    {
	this.updCntr = parseInt(this.attrs['trcPer']);
	var dgrObj = this.place.childNodes[0].childNodes[0];
	if(!dgrObj.stLoadTm) dgrObj.stLoadTm = (new Date()).getTime();
	if(dgrObj && (dgrObj.isLoad || ((new Date()).getTime()-dgrObj.stLoadTm) > this.updCntr*3000))
	{
	    dgrObj.isLoad = false;
	    dgrObj.src = '/'+MOD_ID+this.addr+'?com=obj&tm='+tmCnt+'&xSc='+this.xScale(true).toFixed(2)+'&ySc='+this.yScale(true).toFixed(2);
	    dgrObj.stLoadTm = (new Date()).getTime();
	}
    }
    else if(this.attrs['root'] == 'Protocol' && (this.updCntr-=prcTm) <= 0)
    {
	this.updCntr = parseInt(this.attrs['trcPer']);
	this.loadData();
    }
    else if(this.attrs['root'] == 'Document')
    {
	var frDoc = this.place.childNodes[0].contentDocument || this.place.childNodes[0].contentWindow || this.place.childNodes[0].document;
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
	frDoc.body.onclick = function(e)
	{
	    if(this.isActive) setFocus(this.wdgLnk.addr);
	    return true;
	}
	this.perUpdtEn( false );
    }
}

function xScale( full )
{
    var rez = parseFloat(this.attrs['geomXsc'])
    if(!full) return rez;
    if(!this.pg) return rez*this.parent.xScale(full);
    // if(this != masterPage) return masterPage.xScale()*rez;
    return rez;
}

function yScale( full )
{
    var rez = parseFloat(this.attrs['geomYsc'])
    if(!full) return rez;
    if(!this.pg) return rez*this.parent.yScale(full);
    // if(this != masterPage) return masterPage.yScale()*rez;
    return rez;
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
}
/***************************************************
 * makeUI                                          *
 ***************************************************/
function makeUI( callBackRez )
{
    if(!callBackRez)
    {
	prcCnt++;
	stTmMain = new Date();
    }

    //> Get open pages list
    //>> Synchronous
    var pgNode = servGet('/'+sessId,'com=pgOpenList&tm='+tmCnt);
    //>> Asynchronous
    //var pgNode = null;
    //if(callBackRez) pgNode = (callBackRez == -1) ? null : callBackRez;
    //else { servGet('/'+sessId,'com=pgOpenList&tm='+tmCnt,makeUI); return; }
    if(pgNode)
    {
	modelPer = parseInt(pgNode.getAttribute("per"));
	//>> Check for delete pages
	for(var i_p = 0; i_p < pgList.length; i_p++)
	{
	    var opPg; var i_ch;
	    for(i_ch = 0; i_ch < pgNode.childNodes.length; i_ch++)
		if(pgNode.childNodes[i_ch].nodeName == 'pg' && nodeText(pgNode.childNodes[i_ch]) == pgList[i_p])
		    break;
	    if(i_ch < pgNode.childNodes.length || !(opPg=masterPage.findOpenPage(pgList[i_p]))) continue;
	    if(opPg.window)
	    {
		if(opPg.windowExt) opPg.window.close();
		else if(opPg != masterPage) document.getElementById('mainCntr').removeChild(opPg.window);
		else
		{
		    document.body.removeChild(opPg.window);
		    masterPage = new pwDescr('',true)
		}
		if(opPg.parent)	delete opPg.parent.pages[pgList[i_p]];
	    }
	    else if(opPg.parent && opPg.parent.inclOpen && opPg.parent.inclOpen == pgList[i_p])
	    { opPg.parent.attrs['pgOpenSrc'] = ''; opPg.parent.makeEl(null,true); }
	}
	//>> Process opened pages
	pgList = new Array();
	for(var i = 0; i < pgNode.childNodes.length; i++)
	    if(pgNode.childNodes[i].nodeName == 'pg')
	    {
	        var prPath = nodeText(pgNode.childNodes[i]);
		//>>> Check for closed window
		var opPg = masterPage.findOpenPage(prPath);
		if(opPg && opPg.window && opPg.windowExt && opPg.window.closed)
		{
		    servSet(prPath,'com=pgClose','');
		    opPg.pwClean();
		    delete opPg.parent.pages[prPath];
		    continue;
		}
		//>>> Call page
		pgList.push(prPath);
		masterPage.callPage(prPath,parseInt(pgNode.childNodes[i].getAttribute('updWdg')));
	    }
	tmCnt = parseInt(pgNode.getAttribute('tm'));
    }
    //> Update some widgets
    for(var i in perUpdtWdgs) perUpdtWdgs[i].perUpdt();

    //> Elapsed time get and adjust for plane update period depends from network speed
    var elTm = 1e-3*((new Date()).getTime()-stTmMain.getTime());
    if(!planePer) planePer = 1e-3*modelPer;
    planePer += (Math.max(1e-3*modelPer,elTm*3)-planePer)/100;
    var sleepTm = Math.max(0, planePer-elTm);
    prcTm = elTm + sleepTm;
    //console.log("sleepTm: "+sleepTm+"s; prcTm: "+prcTm+"s; elTm: "+elTm+"s; planePer: "+planePer+"s.");
    setTimeout(makeUI,sleepTm*1e3);

    //prcTm = Math.max(modelPer*1e-3,Math.min(60,3e-3*((new Date()).getTime() - stTmMain.getTime())));
    //setTimeout(makeUI,prcTm*1e3);

    //> Execution performance test
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
    window.status = mess ? mess : '###Ready###';
    if(!mess) return;
    if(stTmID) clearTimeout(stTmID);
    if(!tm || tm > 0) stTmID = setTimeout('setStatus(null)',tm?tm:1000);
}

/**************************************************
 * getPopup - Get popup menu.                     *
 **************************************************/
function getPopup( )
{
    var popUpMenu = document.getElementById('popupmenu');
    if(!popUpMenu)
    {
	popUpMenu = document.createElement('div'); popUpMenu.id = 'popupmenu';
	popUpMenu.appendChild(document.createElement('select'));
	document.body.appendChild(popUpMenu);
	popUpMenu.style.visibility = 'hidden';
    }
    return popUpMenu;
}

/***************************************************
 * Main start code                                 *
 ***************************************************/
var SEC_XT = 0x01;	//Extended
var SEC_WR = 0x02;	//Write access
var SEC_RD = 0x04;	//Read access

//Call session identifier
var sessId = location.pathname.split('/');
for(var i_el = sessId.length-1; i_el >= 0; i_el--)
    if( sessId[i_el].length )
    { sessId = sessId[i_el]; break; }

document.body.onmouseup = function(e)
{
    if(!e) e = window.event;
    if(evMouseGet(e) != 'Left') return true;
    var popUpMenu = document.getElementById('popupmenu');
    if(popUpMenu) popUpMenu.style.visibility = 'hidden';
    return false;
}

var modelPer = 0;			//Model proc period
var prcCnt = 0;				//Process counter
var prcTm = 0;				//Process time
var planePer = 0;			//Planed update period
var tmCnt = 0;				//Call counter
var pgList = new Array();		//Opened pages list
var pgCache = new Object();		//Cached pages' data
var perUpdtWdgs = new Object();		//Periodic updated widgets register
var masterPage = new pwDescr('',true);	//Master page create
var stTmID = null;			//Status line timer identifier
var stTmMain = null;			//Main cycle start time

setTimeout(makeUI,1000);		//First call init
