
//OpenSCADA system module UI.WebCfgD file: VCA.js
/***************************************************************************
 *   Copyright (C) 2008-2014 by Roman Savochenko, <rom_as@oscada.org>      *
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

//Global parameters init
MOD_ID = 'WebCfgD';	//Module identifier
stTmID = null;		//Status line timer identifier
pgRefrTmID = null;	//Periodic current page update timer identifier
dlgWin = null;		//Opened window dialog
selPath = '';		//Selected node path
queSZ = 20;		//Previous and next arrays queue size
ndPrev = new Array();	//Previous nodes array
ndNext = new Array();	//Next nodes array
pgInfo = document.createElement('info');	//Curent page XML tree.
root = document.createElement('oscada_cntr');	//Root page's node.
selPath = '';		//Select item path
SEC_XT = 0x01;		//Extended
SEC_WR = 0x02;		//Write access
SEC_RD = 0x04;		//Read access
copyBuf = '0';		//Copy node address buffer
genReqs = null;		//Generic request object

//Browser type detect
var isNN = navigator.appName.indexOf('Netscape') != -1;
var isIE = navigator.appName.indexOf('Microsoft') != -1;
var isOpera = navigator.appName.indexOf('Opera') != -1;
var isKonq = navigator.userAgent.indexOf('Konqueror') != -1;
var isChrome = navigator.userAgent.indexOf('Chrome') != -1;

/***************************************************
 * strEncode - String encoding.                    *
 ***************************************************/
function strEncode( vl, tp )
{
    //if(!tp || tp == "html") return vl.replace(/&/g,'&amp;').replace(/>/g,'&gt;').replace(/</g,'&lt;').replace(/"/g,'&quot;');
    //return vl;
    var encRez = '';
    if(!tp || tp == "html")
	for(var i_enc = 0; i_enc < vl.length; i_enc++)
	    switch(vl.charAt(i_enc)) {
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
    while(true) {
	t_dir = path.indexOf('/',an_dir);
	if(t_dir < 0) { pathLev.off = path.length; return (t_lev==level) ? path.substr(an_dir) : ''; }
	if(t_lev == level) { pathLev.off = t_dir; return path.substr(an_dir,t_dir-an_dir); }
	an_dir = t_dir;
	t_lev++;
	while(an_dir < path.length && path.charAt(an_dir) == '/') an_dir++;
    }
}

/***************************************************
 * nodeText - Get DOM node text                    *
 ***************************************************/
function nodeText( node )
{
    var rez = '';
    for(var i = 0; node && i < node.childNodes.length; i++)
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
    return null;
}

/***************************************************
 * posGetX - Get absolute position                 *
 ***************************************************/
function posGetX( obj, noWScrl )
{
    var posX = 0;
    //Calc offset on blocks
    for(var cObj = obj; cObj != null; cObj = cObj.offsetParent) posX += cObj.offsetLeft;
    //Calc block's scroll
    if(!isOpera)
	for(var cObj = obj ; (cObj && cObj.nodeName!='BODY'); cObj = cObj.parentNode)
	    posX -= (cObj.scrollLeft?cObj.scrollLeft:0);

    return posX + (!noWScrl?-window.pageXOffset:0) + (isNN?5:0);
}

/***************************************************
 * posGetY - Get absolute position                 *
 ***************************************************/
function posGetY( obj, noWScrl )
{
    var posY = 0;
    //Calc offset on blocks
    for(var cObj = obj; cObj != null; cObj = cObj.offsetParent) posY += cObj.offsetTop;
    //Calc block's scroll
    if(!isOpera)
	for(var cObj = obj ; (cObj && cObj.nodeName!='BODY'); cObj = cObj.parentNode)
	    posY -= (cObj.scrollTop?cObj.scrollTop:0);

    return posY + (!noWScrl?-window.pageYOffset:0) + (isNN?5:0);
}

/***************************************************
 * getXmlHttp - Check and return XMLHttpRequest for*
 *  various browsers.                              *
 ***************************************************/
function getXmlHttp( )
{
    if(window.XMLHttpRequest) return new XMLHttpRequest();
    else if(window.ActiveXObject) {
	try { return new ActiveXObject('Msxml2.XMLHTTP'); }
	catch(e) { return new ActiveXObject('Microsoft.XMLHTTP'); }
    }
    return null;
}

/***************************************************
 * crDoc - Create new XML document object for      *
 *  various browsers.                              *
 ***************************************************/
function crDoc()
{
    var doc;
    if(document.implementation && document.implementation.createDocument)
	doc = document.implementation.createDocument("", "", null);
    else if(window.ActiveXObject) {
	for(var i = 0; i < MSXML.length; i++)
	    try { doc = new ActiveXObject(MSXML[i]); } catch (e) { }
	if(!doc) { alert("Cannot create XMLDocument object"); return false; }
    }

    return doc;
}

/***************************************************
 * realRound - Real numbers round                  *
 ***************************************************/
function realRound( val, dig, toInt )
{
    rez = Math.floor(val*Math.pow(10,dig?dig:0)+0.5)/Math.pow(10,dig?dig:0);
    if(toInt) return Math.floor(rez+0.5);
    return rez;
}

/***************************************************
 * servGet - XML get request to server             *
 ***************************************************/
function servGet( adr, prm )
{
    if(genReqs) {
	var rCom = prm.slice(4);
	if(genReqs.childNodes[0].getAttribute('fillMode') == '1') {
	    var tEl = genReqs.createElement(rCom);
	    tEl.setAttribute('path',adr);
	    genReqs.childNodes[0].appendChild(tEl);
	    return null;
	}
	else {
	    var sNd = null;
	    var c_pos = genReqs.childNodes[0].getAttribute("curPos");
	    if(!c_pos) c_pos = 0;
	    for( ; c_pos < genReqs.childNodes[0].childNodes.length && !sNd; c_pos++) {
		var wNd = genReqs.childNodes[0].childNodes[c_pos];
		if(wNd.getAttribute('path') == adr && wNd.tagName == rCom) sNd = wNd;
	    }
	    if(!sNd && c_pos)
		for(c_pos = 0; c_pos < genReqs.childNodes[0].childNodes.length && !sNd; c_pos++)
	    {
		var wNd = genReqs.childNodes[0].childNodes[c_pos];
		if(wNd.getAttribute('path') == adr && wNd.tagName == rCom) sNd = wNd;
	    }

	    if(sNd) {
		genReqs.childNodes[0].setAttribute('curPos',c_pos+1);
		return sNd;
	    }
	    adr = genReqs.childNodes[0].getAttribute('path')+'/'+adr;
	}
    }

    var req = getXmlHttp();
    req.open('GET',encodeURI('/'+MOD_ID+adr+'?'+prm),false);
    //console.log("TEST 10: "+encodeURI('/'+MOD_ID+adr+'?'+prm));
//    req.setRequestHeader('Content-Type','text/xml; charset=utf-8');
    try {
	req.send(null);
	if(req.status == 200 && req.responseXML.childNodes.length)
	return req.responseXML.childNodes[0];
    } catch(e) { window.location = '/'; }

    return null;
}

/***************************************************
 * servSet - XML set request to server             *
 ***************************************************/
function servSet( adr, prm, body, waitRez )
{
    var req = getXmlHttp();
    req.open('POST',encodeURI('/'+MOD_ID+adr+'?'+prm),!waitRez);
    try {
	req.send(body);
	if(waitRez && req.status == 200 && req.responseXML.childNodes.length)
	return req.responseXML.childNodes[0];
    } catch(e) { window.location='/'; }

    return null;
}

/***************************************************
 * servReq - XML generic request to server         *
 *  body - XML document body                       *
 ***************************************************/
function servReq( body )
{
    var req = getXmlHttp();
    req.open('POST',encodeURI('/'+MOD_ID+'/?com=req'),false);
    req.setRequestHeader("Content-Type", "application/xml;charset=UTF-8");
    try {
	req.send(body);
	if(req.status == 200) return req.responseXML;
    } catch(e) { window.location='/'; }

    return null;
}

/***************************************************
 * setStatus - Setup status message.               *
 ***************************************************/
function setStatus( mess, tm )
{
    setNodeText(document.getElementById('status'), mess?mess:'###Ready###');
    if(!mess) return;
    if(stTmID) clearTimeout(stTmID);
    if(!tm || tm > 0) stTmID = setTimeout('setStatus(null)', tm?tm:1000);
}

/***************************************************
 * expand - Expand/roll select subtree.            *
 ***************************************************/
function expand( el, val, upTree )
{
    if(!val) {
	for(var i = 0; i < el.childNodes.length; i++)
	    if(el.childNodes[i].nodeName == 'UL')
	    { el.removeChild(el.childNodes[i]); break; }
    }
    else {
	var cUL = null;
	for(var i = 0; i < el.childNodes.length; i++)
	    if(el.childNodes[i].nodeName == 'UL')
	    { cUL = el.childNodes[i]; break; }
	if(!cUL) {
	    cUL = document.createElement('ul');
	    if(el.parentNode.lastChild != el) cUL.style.backgroundImage = 'url(img_treeLine)';
	    el.appendChild(cUL);
	}
	if(el.grps.length > 1) {
	    //Add and update present
	    for(var i_g = 0; i_g < el.grps.length; i_g++) {
		var liN = null;
		// Search present item
		if(upTree)
		    for(var i_it = 0; i_it < cUL.childNodes.length; i_it++)
			if(cUL.childNodes[i_it].grps[0].getAttribute('id') == el.grps[i_g].getAttribute('id'))
			{ liN = cUL.childNodes[i_it]; break; }
		if(!liN) {
		    liN = document.createElement('li');
		    if(i_g >= cUL.childNodes.length) cUL.appendChild(liN);
		    else cUL.insertBefore(liN, cUL.childNodes[i_g]);
		    liN.isExpand = false;
		}
		liN.setAttribute('id', el.getAttribute('id'));
		// Set group
		liN.grps = new Array(); liN.grps.push(el.grps[i_g]);
		// Init links
		var isUsable = parseInt(liN.grps[0].getAttribute('chPresent'));
		var treeIco = '/'+MOD_ID+'/img_tree'+(isUsable?(liN.isExpand?'Minus':'Plus'):'')+'Up'+((i_g!=(el.grps.length-1))?'Down':'');
		var liCont = isUsable?"<a class='pm' onclick='expand(this.parentNode,!this.parentNode.isExpand); return false;'>":"";
		liCont += "<img src='"+treeIco+"'/></a>";
		liCont += "<span style='font-style: italic;'>"+strEncode(el.grps[i_g].getAttribute('dscr'))+":</span>";
		// Next node for update
		if(upTree && liN.isExpand) {
		    var liNUL = null;
		    for(var i = 0; i < liN.childNodes.length; i++)
			if(liN.childNodes[i].nodeName == 'UL') { liNUL = liN.childNodes[i]; break; }
		    liN.innerHTML = liCont;
		    if(liNUL) liN.appendChild(liNUL);
		    expand(liN,val,upTree);
		}
		else liN.innerHTML = liCont;
	    }
	    //Delete no present
	    if(upTree)
		for(var i_it = 0; i_it < cUL.childNodes.length; i_it++) {
		    var i_g;
		    for(i_g = 0; i_g < el.grps.length; i_g++)
			if(cUL.childNodes[i_it].grps[0].getAttribute('id') == el.grps[i_g].getAttribute('id'))
			    break;
		    if(i_g >= el.grps.length) { cUL.removeChild(cUL.childNodes[i_it]); i_it--; }
		}
	}
	else {
	    var grpId = el.grps[0].getAttribute('id');
	    var hostN = servGet(el.getAttribute('id'),'com=chlds&grp='+grpId);
	    if(hostN && parseInt(hostN.getAttribute('rez')) == 0) {
		//Add and update present
		for(var i_e = 0; i_e < hostN.childNodes.length; i_e++) {
		    var tmpNdId = el.getAttribute('id')+'/'+
			(grpId+(hostN.childNodes[i_e].getAttribute('id')?hostN.childNodes[i_e].getAttribute('id'):
									 nodeText(hostN.childNodes[i_e])));
		    var liN = null;
		    // Find item
		    if(upTree)
		    for(var i_it = 0; i_it < cUL.childNodes.length; i_it++)
			if(cUL.childNodes[i_it].getAttribute('id') == tmpNdId)
			{ liN = cUL.childNodes[i_it]; break; }
		    if(!liN) {
			liN = document.createElement('li');
			if(i_e >= cUL.childNodes.length) cUL.appendChild(liN);
			else cUL.insertBefore(liN, cUL.childNodes[i_e]);
			liN.isExpand = false;
		    }
		    liN.setAttribute('id',tmpNdId);
		    // Load groups
		    liN.grps = new Array();
		    for(var i_grp = 0; i_grp < hostN.childNodes[i_e].childNodes.length; i_grp++)
		    if(hostN.childNodes[i_e].childNodes[i_grp].nodeName == 'grp')
			liN.grps.push(hostN.childNodes[i_e].childNodes[i_grp]);
		    // Init links
		    var isUsable = (liN.grps.length>1)||(liN.grps.length&&parseInt(liN.grps[0].getAttribute('chPresent')));
		    var treeIco = '/'+MOD_ID+'/img_tree'+(isUsable?(liN.isExpand?'Minus':'Plus'):'')+'Up'+((i_e!=(hostN.childNodes.length-1))?'Down':'');
		    var liCont = isUsable?"<a class='pm' onclick='expand(this.parentNode,!this.parentNode.isExpand); return false;'>":"";
		    liCont += "<img src='"+treeIco+"'/></a>";
		    if(parseInt(hostN.childNodes[i_e].getAttribute('icoSize')))
			liCont += "<span><img src='/"+MOD_ID+liN.getAttribute('id')+"?com=ico&size=16'/></span>"; 
		    //liCont += "<span><img height='16px' src='/"+MOD_ID+liN.getAttribute('id')+"?com=ico'/></span>";
		    liCont += "<span><a onclick='selectPage(this.parentNode.parentNode.getAttribute(\"id\")); return false;' "+
			"onmouseover='setStatus(this.parentNode.parentNode.getAttribute(\"id\"),10000);' href='#'>"+strEncode(nodeText(hostN.childNodes[i_e]))+"</a></span>";
		    // Next node for update
		    if(upTree && liN.isExpand) {
			var liNUL = null;
			for(var i_eu = 0; i_eu < liN.childNodes.length; i_eu++)
			    if(liN.childNodes[i_eu].nodeName == 'UL') { liNUL = liN.childNodes[i_eu]; break; }
			liN.innerHTML = liCont;
			if(liNUL) liN.appendChild(liNUL);
			expand(liN, val, upTree);
		    } else liN.innerHTML = liCont;
		}
		//Delete no present
		if(upTree)
		    for(var i_it = 0; i_it < cUL.childNodes.length; i_it++) {
			var i_e;
			for(i_e = 0; i_e < hostN.childNodes.length; i_e++) {
			    var grpId = el.grps[0].getAttribute('id');
			    var tmpNdId = el.getAttribute('id')+'/'+(grpId+(hostN.childNodes[i_e].getAttribute('id')?hostN.childNodes[i_e].getAttribute('id'):nodeText(hostN.childNodes[i_e])));
			    if(cUL.childNodes[i_it].getAttribute('id') == tmpNdId) break;
			}
			if(i_e >= hostN.childNodes.length) { cUL.removeChild(cUL.childNodes[i_it]); i_it--; }
		    }
	    }
	}
    }

    if(el.isExpand != val) {
	//Change tree icon
	el.childNodes[0].childNodes[0].src = val ? el.childNodes[0].childNodes[0].src.replace('Plus','Minus') :
						   el.childNodes[0].childNodes[0].src.replace('Minus','Plus');
	el.isExpand = val;
    }
}

/***************************************************
 * selectPage - Select node by user.               *
 ***************************************************/
function selectPage( path )
{
    if(!path) return;

    //Prev and next
    if(selPath.length) ndPrev.push(selPath);
    while(ndPrev.length >= queSZ) ndPrev.shift();
    ndNext = new Array();

    //Page display
    pageDisplay(path);
}

/***************************************************
 * pageDisplay - Display selected page.            *
 ***************************************************/
function pageDisplay( path )
{
    if(!path) return;

    //Chek Up
    actEnable('actUp',path.lastIndexOf('/') != -1 && path.lastIndexOf('/') != 0);

    //Check Prev and Next
    actEnable('actPrevious',ndPrev.length);
    actEnable('actNext',ndNext.length);

    if(path != pgInfo.getAttribute('path')) {
	// Stop refresh
	pageCyclRefrStop();

	if(selPath.length && document.getElementById(selPath)) document.getElementById(selPath).className = '';
	selPath = path;
	if(selPath.length && document.getElementById(selPath)) document.getElementById(selPath).className = 'select';
	setNodeText(document.getElementById('selPath'),selPath);

	pgInfo = servGet(selPath,'com=info');
	if(parseInt(pgInfo.getAttribute('rez')) != 0) { alert(nodeText(pgInfo)); return; }
	pgInfo.setAttribute('path',selPath);
	root = pgInfo.childNodes[0];
    }
    else {
	// Check the new node structure and the old node
	var iTree = servGet(selPath,'com=info');
	if(parseInt(iTree.getAttribute('rez')) != 0) { alert(nodeText(iTree)); return; }
	if(chkStruct(root,iTree.childNodes[0]))
	{ pgInfo = iTree; pgInfo.setAttribute('path',selPath); root = pgInfo.childNodes[0]; }
    }

    //Complex request form and it result use
    genReqs = crDoc();
    genReqs.appendChild(genReqs.createElement('CntrReqs'));
    genReqs.childNodes[0].setAttribute('path',selPath);
    genReqs.childNodes[0].setAttribute('fillMode','1');
    for(var genReqPrc = 0; genReqPrc < 2; genReqPrc++) {
	selectChildRecArea(root,'/',null);

	//Load and Save allow check
	var reqModif = servGet('/%2fobj','com=modify');
	actEnable('actLoad',parseInt(nodeText(reqModif)));
	actEnable('actSave',parseInt(nodeText(reqModif)));

	//Send prepared generic request
	if(genReqPrc == 0) {
	    genReqs.childNodes[0].setAttribute('fillMode','0');
	    genReqs = servReq(genReqs);
	}
    }
    genReqs = null;

    //The add and the delete access allow check
    actEnable('actAddIt',false);
    for(var i_ch = 0; i_ch < root.childNodes.length; i_ch++)
	if(root.childNodes[i_ch].getAttribute('id') == 'br') {
	    for(var i_g = 0; i_g < root.childNodes[i_ch].childNodes.length; i_g++)
		if(parseInt(root.childNodes[i_ch].childNodes[i_g].getAttribute('acs'))&SEC_WR)
		{ actEnable('actAddIt',true); break; }
	break;
    }
    actEnable('actDelIt',parseInt(root.getAttribute('acs'))&SEC_WR)

    //Edit tools update
    editToolUpdate( );
}

/***************************************************
 * editToolUpdate - Edit action state update.      *
 ***************************************************/
function editToolUpdate( )
{
    actEnable('actCut',(selPath.length&&parseInt(root.getAttribute('acs'))&SEC_WR));
    actEnable('actCopy',selPath.length);
    actEnable('actPaste',false);

    //Src and destination elements calc
    if(copyBuf.length <= 1 || copyBuf.substr(1) == selPath || pathLev(copyBuf.substr(1),0) != pathLev(selPath,0)) return;
    var s_elp; var s_el; var t_el;
    pathLev.off = 0;
    while((t_el=pathLev(copyBuf.substr(1),0,true)).length) { s_elp += ('/'+s_el); s_el = t_el; }

    for(var i_ch = 0; i_ch < root.childNodes.length; i_ch++)
	if(root.childNodes[i_ch].getAttribute('id') == 'br') {
	    for(var i_g = 0; i_g < root.childNodes[i_ch].childNodes.length; i_g++)
		if(parseInt(root.childNodes[i_ch].childNodes[i_g].getAttribute('acs'))&SEC_WR)
		{ actEnable('actPaste',true); break; }
	    break;
	}
    if(parseInt(root.getAttribute('acs'))&SEC_WR) actEnable('actPaste',true);
}

/***************************************************
 * selectChildRecArea - Make page content          *
 ***************************************************/
function selectChildRecArea( node, aPath, cBlk )
{
    var i_area = 0;
    //View title name
    if(aPath == "/") {
	// Set node icon
	if(nodeTextByTagId(node,'img','ico') != null) {
	    document.getElementById('pgIco').src = '/'+MOD_ID+selPath+'?com=ico&size=32';
	    document.getElementById('pgIco').style.visibility = 'visible';
	}
	else document.getElementById('pgIco').style.visibility = 'hidden';
	// Set title
	setNodeText(document.getElementById('pgTitle'),node.getAttribute('dscr'));
	// Delete tabs of deleted areas
	var activeTab = null;
	var tabs = document.getElementById('pgTabs');
	for(var i_tbs = 0; i_tbs < tabs.childNodes.length; i_tbs++) {
	    var i_cf;
	    for(i_cf = 0; i_cf < node.childNodes.length; i_cf++)
		if(node.childNodes[i_cf].nodeName.toLowerCase() == 'area' &&
			nodeText(tabs.childNodes[i_tbs]) == node.childNodes[i_cf].getAttribute('dscr'))
		    break;
	    if(i_cf >= node.childNodes.length) {
		tabs.removeChild(tabs.childNodes[i_tbs]);
		i_tbs--;
	    }
	    else if(tabs.childNodes[i_tbs].className == 'active') activeTab = tabs.childNodes[i_tbs];
	}
	// Add new tabs
	for(var i_cf = 0; i_cf < node.childNodes.length; i_cf++) {
	    if(node.childNodes[i_cf].nodeName.toLowerCase() != 'area') continue;
	    var i_tbs;
	    for(i_tbs = 0; i_tbs < tabs.childNodes.length; i_tbs++)
		if(nodeText(tabs.childNodes[i_tbs]) == node.childNodes[i_cf].getAttribute('dscr'))
		    break;
	    if(i_tbs >= tabs.childNodes.length) {
		var itab = document.createElement('span');
		setNodeText(itab,node.childNodes[i_cf].getAttribute('dscr'));
		itab.onclick = function( ) { tabSelect(this); return false; }
		if(i_area >= tabs.childNodes.length) tabs.appendChild(itab);
		else tabs.insertBefore(itab,tabs.childNodes[i_area]);
		node.childNodes[i_cf].setAttribute('qview','0');
	    }
	    i_area++;
	}
	if(!activeTab && tabs.childNodes.length) { activeTab = tabs.childNodes[0]; activeTab.className = 'active'; }
	// Prepare active tab
	for(var i_cf = 0; i_cf < node.childNodes.length; i_cf++)
	    if(node.childNodes[i_cf].nodeName.toLowerCase() == 'area' &&
		    nodeText(activeTab) == node.childNodes[i_cf].getAttribute('dscr'))
	    {
		var refresh = parseInt(node.childNodes[i_cf].getAttribute('qview'));
		var cPg = document.getElementById('pgCont');
		if(!refresh) {
		    while(cPg.childNodes.length) cPg.removeChild(cPg.childNodes[0]);
		    selectChildRecArea(node.childNodes[i_cf],aPath+node.childNodes[i_cf].getAttribute('id')+'/',cPg);
		    //  Mark last drawed tabs
		    node.childNodes[i_cf].setAttribute('qview','1');
		}
		else selectChildRecArea(node.childNodes[i_cf],aPath+node.childNodes[i_cf].getAttribute('id')+'/',null);
	    }
	return;
    }
    else for(var i_cf = 0; i_cf < node.childNodes.length; i_cf++) {
	var t_s = node.childNodes[i_cf];

	// Check access to node
	var wr = parseInt(t_s.getAttribute('acs'))&SEC_WR;

	// View areas
	if(t_s.nodeName.toLowerCase() == 'area') {
	    var cntBlk = cBlk;
	    if(cntBlk) {
		var cntBlk = document.createElement('fieldset');
		cntBlk.className = 'elem';
		cntBlk.appendChild(document.createElement('legend'));
		cntBlk.childNodes[0].appendChild(document.createTextNode(t_s.getAttribute('dscr')));
		cBlk.appendChild(cntBlk);
	    }
	    selectChildRecArea(t_s,aPath+t_s.getAttribute('id')+'/',cntBlk);
	}
	// View list elements
	else if(t_s.nodeName.toLowerCase() == 'list') {
	    var brPath = (aPath+t_s.getAttribute('id')).replace(/%/g,'%25').replace(/\//g,'%2f');
	    var lab = null; var val = null;

	    if(cBlk) {
		var dBlk = document.createElement('div'); dBlk.className = 'elem';
		lab = document.createElement('span'); lab.className = 'label';
		dBlk.appendChild(lab);
		dBlk.appendChild(document.createElement('br'));
		val = document.createElement('select'); val.className = 'list';
		val.size = 10;
		val.srcNode = t_s;
		val.itPath = selPath+'/'+brPath;
		val.onmouseover = function() { setStatus(this.itPath,10000); }
		val.onclick = function(e)
		{
		    if(!e) e = window.event;
		    var popUpMenu = getPopup();
		    var optEl = '';
		    if(this.srcNode.getAttribute('tp') == 'br' && this.selectedIndex >= 0)
			optEl += "<option posId='go'>###Go###</option><option disabled='true'>------------</option>";
		    if((parseInt(this.srcNode.getAttribute('acs'))&SEC_WR) && this.srcNode.getAttribute('s_com'))
		    {
			if(this.srcNode.getAttribute('s_com').search('add') != -1)
			    optEl += "<option posId='add'>###Add###</option>";
			if(this.srcNode.getAttribute('s_com').search('ins') != -1 && this.selectedIndex >= 0)
			    optEl += "<option posId='ins'>###Insert###</option>";
			if(this.srcNode.getAttribute('s_com').search('edit') != -1 && this.selectedIndex >= 0)
			    optEl += "<option posId='edit'>###Edit###</option>";
			if(this.srcNode.getAttribute('s_com').search('del') != -1 && this.selectedIndex >= 0)
			    optEl += "<option posId='del'>###Delete###</option>";
			if(this.srcNode.getAttribute('s_com').search('move') != -1 && this.selectedIndex >= 0)
			    optEl += "<option disabled='true'>------------</option><option posId='up'>###Item up###</option><option posId='down'>###Item down###</option>";
		    }
		    popUpMenu.childNodes[0].innerHTML = optEl;
		    if(popUpMenu.childNodes[0].childNodes.length) {
			popUpMenu.srcNode = this.srcNode;
			popUpMenu.itPath = this.itPath;
			if(this.selectedIndex >= 0) {
			    popUpMenu.lsId = this.options[this.selectedIndex].lsId;
			    popUpMenu.lsText = this.options[this.selectedIndex].value;
			    if(!popUpMenu.lsId) popUpMenu.lsId = popUpMenu.lsText;
			}
			popUpMenu.childNodes[0].size = Math.max(3,popUpMenu.childNodes[0].childNodes.length);
			popUpMenu.style.cssText = 'visibility: visible; left: '+(e.clientX+window.pageXOffset)+'px; top: '+(e.clientY+window.pageYOffset)+'px;';
			popUpMenu.childNodes[0].focus();
			popUpMenu.childNodes[0].selectedIndex = -1;
			popUpMenu.childNodes[0].onclick = function()
			{
			    this.parentNode.style.cssText = 'visibility: hidden; left: -200px; top: -200px;';
			    if(this.selectedIndex < 0 || !this.options[this.selectedIndex].getAttribute('posId')) return;
			    var idm = parseInt(this.parentNode.srcNode.getAttribute('idm'));
			    var posId = this.options[this.selectedIndex].getAttribute('posId');
			    if(posId == 'go')
				selectPage(selPath+'/'+(this.parentNode.srcNode.getAttribute('br_pref')+this.parentNode.lsId).replace(/%/g,'%25').replace(/\//g,'%2f'));
			    else if(posId == 'add' || posId == 'ins' || posId == 'edit') {
				dlgWin = ReqIdNameDlg('/'+MOD_ID+'/ico');
				setNodeText(dlgWin.document.getElementById('wDlgHeader'),'###Item name set###');
				var idSz = parseInt(this.parentNode.srcNode.getAttribute('idSz'));
				dlgWin.document.getElementById('wDlgId').childNodes[1].childNodes[0].maxLength = ((idSz > 0) ? idSz : 1000);
				dlgWin.document.getElementById('wDlgType').style.display = 'none';
				dlgWin.document.getElementById('wDlgName').style.display = idm ? '' : 'none';
				dlgWin.document.getElementById('wDlgName').childNodes[1].childNodes[0].maxLength = ((idm > 1) ? idm : 1000);
				var actOkFld = dlgWin.document.getElementById('wDlgActOk');
				actOkFld.itPath = this.parentNode.itPath;
				actOkFld.srcNode = this.parentNode.srcNode;
				actOkFld.selectedIndex = this.selectedIndex;
				actOkFld.lsId = this.parentNode.lsId;
				actOkFld.lsText = this.parentNode.lsText;
				if(posId == 'add') {
				    setNodeText(dlgWin.document.getElementById('wDlgTitle').childNodes[1],'###Add new element.###');
				    actOkFld.onclick = function()
				    {
					var idm = dlgWin.document.getElementById('wDlgName').style.display!='none';
					var inpId = dlgWin.document.getElementById('wDlgId').childNodes[1].childNodes[0].value;
					var inpName = idm ? dlgWin.document.getElementById('wDlgName').childNodes[1].childNodes[0].value : inpId;
					var rez = servSet(this.itPath,'com=com',"<add "+(idm?("id='"+inpId+"'"):"")+">"+inpName+"</add>",true);
					if(rez && parseInt(rez.getAttribute('rez')) != 0) alert(nodeText(rez));
					if(this.srcNode.getAttribute('tp') == 'br') treeUpdate();
					pageRefresh();
					document.body.dlgWin.close();
					return false;
				    }
				}
				else if(posId == 'ins') {
				    setNodeText(dlgWin.document.getElementById('wDlgTitle').childNodes[1],'###Insert new element.###');
				    actOkFld.onclick = function()
				    {
					var idm = dlgWin.document.getElementById('wDlgName').style.display!='none';
					var inpId = dlgWin.document.getElementById('wDlgId').childNodes[1].childNodes[0].value;
					var inpName = idm ? dlgWin.document.getElementById('wDlgName').childNodes[1].childNodes[0].value : inpId;
					var com = "<ins "+(idm?("id='"+inpId+"' "):"")+" pos='"+this.selectedIndex+"' p_id='"+this.lsId+"'>"+inpName+"</ins>";
					var rez = servSet(this.itPath,'com=com',com,true);
					if(rez && parseInt(rez.getAttribute('rez')) != 0) alert(nodeText(rez));
					if(this.srcNode.getAttribute('tp') == 'br') treeUpdate();
					pageRefresh();
					document.body.dlgWin.close();
					return false;
				    }
				}
				else if(posId == 'edit') {
				    setNodeText(dlgWin.document.getElementById('wDlgTitle').childNodes[1],'###Rename element.###');
				    dlgWin.document.getElementById('wDlgId').childNodes[1].childNodes[0].value = idm ? this.parentNode.lsId : this.parentNode.lsText;
				    if(idm) dlgWin.document.getElementById('wDlgName').childNodes[1].childNodes[0].value = this.parentNode.lsText;
				    actOkFld.onclick = function()
				    {
					var idm = dlgWin.document.getElementById('wDlgName').style.display!='none';
					var inpId = dlgWin.document.getElementById('wDlgId').childNodes[1].childNodes[0].value;
					var inpName = idm ? dlgWin.document.getElementById('wDlgName').childNodes[1].childNodes[0].value : inpId;
					var com = "<edit "+(idm?("id='"+inpId+"' "):"")+" pos='"+this.selectedIndex+"' p_id='"+this.lsId+"'>"+inpName+"</edit>";
					var rez = servSet(this.itPath,'com=com',com,true);
					if(rez && parseInt(rez.getAttribute('rez')) != 0) alert(nodeText(rez));
					if(this.srcNode.getAttribute('tp') == 'br') treeUpdate();
					pageRefresh();
					document.body.dlgWin.close();
					return false;
				    }
				}
			    }
			    else if(posId == 'up' || posId == 'down') {
				var c_new = (posId == 'down') ? c_new = this.selectedIndex+1 : this.selectedIndex-1;
				var rez = servSet(this.parentNode.itPath,'com=com',"<move pos='"+this.selectedIndex+"' to='"+c_new+"'/>",true);
				if(rez && parseInt(rez.getAttribute('rez')) != 0) alert(nodeText(rez));
				pageRefresh();
			    }
			    else if(posId == 'del') {
				var com = idm ? ("<del pos='"+this.selectedIndex+"' id='"+this.parentNode.lsId+"'/>") :
						("<del pos='"+this.selectedIndex+"'>"+this.parentNode.lsText+"</del>");
				var rez = servSet(this.parentNode.itPath,'com=com',com,true);
				if(rez && parseInt(rez.getAttribute('rez')) != 0) alert(nodeText(rez));
				if(this.parentNode.srcNode.getAttribute('tp') == 'br') treeUpdate();
				pageRefresh();
			    }
			    return false;
			}
		    }
		    return false;
		}
		dBlk.appendChild(val);
		cBlk.appendChild(dBlk);

		t_s.addr_lab = lab; t_s.addr_val = val;
	    }
	    else { lab = t_s.addr_lab; val = t_s.addr_val; }
	    //  Fill list
	    setNodeText(lab,t_s.getAttribute('dscr')+': ');
	    val.title = t_s.getAttribute('help');

	    while(val.childNodes.length) val.removeChild(val.childNodes[0]);
	    var dataReq = servGet(brPath,'com=get');
	    if(!dataReq) continue;
	    if(parseInt(dataReq.getAttribute('rez')) != 0) { alert(nodeText(dataReq)); continue; }
	    for(var i_el = 0; i_el < dataReq.childNodes.length; i_el++)
		if(dataReq.childNodes[i_el].nodeName.toLowerCase() == 'el') {
		    var opt = document.createElement('option');
		    opt.lsId = dataReq.childNodes[i_el].getAttribute('id');
		    setNodeText(opt,nodeText(dataReq.childNodes[i_el]));
		    val.appendChild(opt);
		}
	    while(val.childNodes.length < 4) {
		var opt = document.createElement('option');
		opt.disabled = true;
		val.appendChild(opt);
	    }
	    val.size = Math.min(10,Math.max(4,val.childNodes.length));
	}
	// View images
	else if(t_s.nodeName.toLowerCase() == "img") {
	    var brPath = (aPath+t_s.getAttribute('id')).replace(/%/g,'%25').replace(/\//g,'%2f').replace(/%/g,'%25');
	    var lab = null; var val = null;

	    if(cBlk) {
		var dBlk = document.createElement('div'); dBlk.className = 'elem';
		lab = document.createElement('span'); lab.className = 'label';
		dBlk.appendChild(lab);
		if(!t_s.getAttribute("v_sz") || parseInt(t_s.getAttribute("v_sz")) > 70)
		    dBlk.appendChild(document.createElement('br'));
		val = document.createElement('img'); val.className = 'picture';
		if(t_s.getAttribute('h_sz')) val.style.width = t_s.getAttribute('h_sz')+'px';
		if(t_s.getAttribute("v_sz")) val.style.height = t_s.getAttribute("v_sz")+'px';
		val.itPath = selPath+'/'+brPath;
		val.onmouseover = function() { setStatus(this.itPath,10000); }
		if(wr) {
		    val.style.cursor = 'pointer';
		    val.onclick = function()
		    {
			dlgWin = ReqIdNameDlg('/'+MOD_ID+'/img_save','###Select image file for download to picture field.###','/'+MOD_ID+this.itPath+'?com=img',true);
			setNodeText(dlgWin.document.getElementById('wDlgHeader'),'###Image set###');
			dlgWin.document.getElementById('wDlgType').style.display = 'none';
			dlgWin.document.getElementById('wDlgId').style.display = 'none';
			dlgWin.document.getElementById('wDlgName').style.display = '';
			dlgWin.document.getElementById('wDlgActOk').type = 'submit';
			if(!isKonq) dlgWin.document.getElementById('wDlgFormBlk').onsubmit = function( )
			{ setTimeout('document.body.dlgWin.close(); pageRefresh();',200); }
			else dlgWin.document.getElementById('wDlgActCancel').onclick = function( )
			{ setTimeout('document.body.dlgWin.close(); pageRefresh();',200); }
			return false;
		    }
		}
		dBlk.appendChild(val);
		cBlk.appendChild(dBlk);

		t_s.addr_lab = lab; t_s.addr_val = val;
	    }
	    else { lab = t_s.addr_lab; val = t_s.addr_val; }
	    //  Set image
	    if(lab) setNodeText(lab,t_s.getAttribute('dscr')+':');
	    if(val) {
		val.title = t_s.getAttribute('help');
		val.src = '/'+MOD_ID+selPath+'/'+brPath+'?com=img&rnd='+Math.floor(Math.random()*1000);
	    }
	}
	else if(t_s.nodeName.toLowerCase() == 'table') {
	    var brPath = (aPath+t_s.getAttribute('id')).replace(/%/g,'%25').replace(/\//g,'%2f');
	    var lab = null; var table = null;
	    if(cBlk) {
		var dBlk = document.createElement('div'); dBlk.className = 'elem';
		lab = document.createElement('span'); lab.className = 'label';
		dBlk.appendChild(lab);
		dBlk.appendChild(document.createElement('div')); dBlk.childNodes[1].className = 'table';
		table = document.createElement('table'); table.className = 'elem'; table.cellPadding = 2; table.cellSpacing = 0;
		table.itPath = selPath+'/'+brPath;
		table.onmouseover = function() { setStatus(this.itPath,10000); }
		if(wr) {
		    table.srcNode = t_s;
		    table.comSet = function(row,col,val) {
			var com = "<set col='"+this.srcNode.childNodes[col].getAttribute('id')+"' ";
			setNodeText(this.srcNode.childNodes[col].childNodes[row], val);
			if(!this.srcNode.getAttribute('key')) com += "row='"+row+"' ";
			else {
			    var keys = this.srcNode.getAttribute('key').split(',');
			    for(var i_off = 0; i_off < keys.length; i_off++)
				for(var i_el = 0; i_el < this.srcNode.childNodes.length; i_el++)
				    if(this.srcNode.childNodes[i_el].getAttribute('id') == keys[i_off])
				    { com += "key_"+keys[i_off]+"='"+nodeText(this.srcNode.childNodes[i_el].childNodes[row])+"' "; break; }
			}
			com += ">"+strEncode(val)+"</set>";

			var rez = servSet(this.itPath,'com=com',com,true);
			if(rez && parseInt(rez.getAttribute('rez')) != 0) alert(nodeText(rez));
			if(!parseInt(rez.getAttribute('noReload'))) setTimeout('pageRefresh()',500);
			else this.setElements();
		    }
		}
		dBlk.childNodes[1].appendChild(table);
		cBlk.appendChild(dBlk);

		table.setElements = function()	// (1)
		{
		    for(var i_col = 0; i_col < this.srcNode.childNodes.length; i_col++) {
			var prcCol = this.srcNode.childNodes[i_col];
			setNodeText(this.childNodes[0].childNodes[i_col+1],prcCol.getAttribute('dscr'));

			//   Load selected list
			if((!prcCol.val_ls || !prcCol.length) && (prcCol.getAttribute('dest') == 'select' || prcCol.getAttribute('dest') == 'sel_ed')) {
			    prcCol.ind_ls = new Array();
			    prcCol.val_ls = new Array();
			    if(!prcCol.getAttribute('select')) {
				if(prcCol.getAttribute('sel_id')) prcCol.ind_ls = prcCol.getAttribute('sel_id').split(';');
				prcCol.val_ls = prcCol.getAttribute('sel_list').split(';');
			    }
			    else {
				var x_lst = servGet((genReqs?'':selPath+'/')+prcCol.getAttribute('select').replace(/%/g,'%25').replace(/\//g,'%2f'),'com=get');
				for(var i_el = 0; x_lst && i_el < x_lst.childNodes.length; i_el++) {
				    if(x_lst.childNodes[i_el].nodeName.toLowerCase() != 'el') continue;
				    if(x_lst.childNodes[i_el].getAttribute('id'))
					prcCol.ind_ls.push(x_lst.childNodes[i_el].getAttribute('id'));
				    prcCol.val_ls.push(nodeText(x_lst.childNodes[i_el]));
				}
			    }
			}
			for(var i_row = 0; i_row < prcCol.childNodes.length; i_row++) {
			    var tblCell = this.childNodes[i_row+1].childNodes[i_col+1];
			    var cval = nodeText(prcCol.childNodes[i_row]);
			    tblCell.isEdited = false; tblCell.cRow = i_row; tblCell.cCol = i_col;
			    if(tblCell.isEnter) while(tblCell.childNodes.length) tblCell.removeChild(tblCell.lastChild);
			    tblCell.isEnter = this.isEnter = false;
			    if(prcCol.getAttribute('tp') == 'bool') {
				setNodeText(tblCell,parseInt(cval)?'On':'Off');
				tblCell.style.fontWeight = parseInt(cval)?'bold':'normal';
				tblCell.style.textAlign = 'center';
			    }
			    else if(prcCol.getAttribute('dest') == 'select') {
				setNodeText(tblCell,cval);
				for(var i_el = 0; i_el < prcCol.val_ls.length; i_el++)
				    if((prcCol.ind_ls.length && prcCol.ind_ls[i_el] == cval) ||
					    (!prcCol.ind_ls.length && prcCol.val_ls[i_el] == cval))
					setNodeText(tblCell,prcCol.val_ls[i_el]);
			    }
			    else if(prcCol.getAttribute('tp') == 'time') {
				var dt = new Date(parseInt(cval)*1000);
				setNodeText(tblCell,dt.getDate()+'.'+(dt.getMonth()+1)+'.'+dt.getFullYear()+' '+dt.getHours()+':'+
				    ((dt.getMinutes()<10)?('0'+dt.getMinutes()):dt.getMinutes())+':'+((dt.getSeconds()<10)?('0'+dt.getSeconds()):dt.getSeconds()));
			    }
			    else {
				if(prcCol.getAttribute('tp') == 'hex') cval = '0x'+parseInt(cval).toString(16);
				else if(prcCol.getAttribute('tp') == 'oct') cval = '0'+parseInt(cval).toString(8);
				setNodeText(tblCell,cval);
			    }
			}
		    }
		    return false;
		}

		t_s.addr_lab = lab; t_s.addr_tbl = table;
	    }
	    else { table = t_s.addr_tbl; lab = t_s.addr_lab; }

	    //  Fill table
	    if(lab) setNodeText(lab,t_s.getAttribute('dscr')+':');
	    if(table) {
		table.title = t_s.getAttribute('help');

		var dataReq = servGet(brPath,'com=get');
		if(dataReq && parseInt(dataReq.getAttribute('rez'))!=0) { alert(nodeText(dataReq)); continue; }

		//   Copy values to info tree
		for(var i_cl = 0; dataReq && i_cl < dataReq.childNodes.length; i_cl++) {
		    var i_cli;
		    for(i_cli = 0; i_cli < t_s.childNodes.length; i_cli++)
			if(dataReq.childNodes[i_cl].getAttribute('id') == t_s.childNodes[i_cli].getAttribute('id')) break;
		    if(i_cli == t_s.childNodes.length) {
			var el = t_s.ownerDocument.createElement(dataReq.childNodes[i_cl].nodeName);
			for(var i_a = 0; i_a < dataReq.childNodes[i_cl].attributes.length; i_a++)
			    el.setAttribute(dataReq.childNodes[i_cl].attributes[i_a].name,dataReq.childNodes[i_cl].attributes[i_a].value);
			t_s.appendChild(el);
		    }
		    while(t_s.childNodes[i_cli].lastChild) t_s.childNodes[i_cli].removeChild(t_s.childNodes[i_cli].lastChild);
		    for(var i_rw = 0; i_rw < dataReq.childNodes[i_cl].childNodes.length; i_rw++) {
			var el = t_s.ownerDocument.createElement(dataReq.childNodes[i_cl].childNodes[i_rw].nodeName);
			setNodeText(el,nodeText(dataReq.childNodes[i_cl].childNodes[i_rw]))
			t_s.childNodes[i_cli].appendChild(el);
		    }
		}

		//   Calc rows and columns
		var n_col = t_s.childNodes.length;
		var n_row = n_col ? t_s.childNodes[0].childNodes.length : 0;

		//   Update table structure
		while(table.childNodes.length > (n_row+1)) table.removeChild(table.lastChild);
		while(table.childNodes.length < (n_row+1)) table.appendChild(document.createElement('tr'));

		for(var i_rw = 0; i_rw < table.childNodes.length; i_rw++) {
		    while(table.childNodes[i_rw].childNodes.length > (n_col+1))
			table.childNodes[i_rw].removeChild(table.childNodes[i_rw].lastChild);
		    while(table.childNodes[i_rw].childNodes.length < (n_col+1)) {
			var cCell = document.createElement(i_rw?'td':'th');
			if(!table.childNodes[i_rw].childNodes.length) {
			    if(i_rw) cCell.className = 'hd';
			    if(wr && t_s.getAttribute('s_com')) {
				cCell.style.cursor = 'pointer';
				cCell.srcNode = t_s;
				cCell.onclick = function(e)
				{
				    var rowP = parseInt(nodeText(this));
				    if(!e) e = window.event;
				    var popUpMenu = getPopup();
				    var optEl = '';
				    if(this.srcNode.getAttribute('s_com').search('add') != -1)
					optEl += "<option posId='add'>###Add row###</option>";
				    if(this.srcNode.getAttribute('s_com').search('ins') != -1 && rowP)
					optEl += "<option posId='ins'>###Insert row###</option>";
				    if(this.srcNode.getAttribute('s_com').search('del') != -1 && rowP)
					optEl += "<option posId='del'>###Delete row###</option>";
				    if(this.srcNode.getAttribute('s_com').search('move') != -1 && rowP)
					optEl += "<option disabled='true'>--------------</option><option posId='up'>###Up row###</option><option posId='down'>###Down row###</option>";
				    popUpMenu.childNodes[0].innerHTML = optEl;

				    if(popUpMenu.childNodes[0].childNodes.length) {
					popUpMenu.childNodes[0].srcNode = this.srcNode;
					popUpMenu.childNodes[0].itPath = this.parentNode.parentNode.itPath;
					popUpMenu.childNodes[0].rowP = rowP-1;
					popUpMenu.childNodes[0].size = Math.max(3,popUpMenu.childNodes[0].childNodes.length);
					popUpMenu.style.cssText = 'visibility: visible; left: '+(e.clientX+window.pageXOffset)+'px; top: '+(e.clientY+window.pageYOffset)+'px;';
					popUpMenu.childNodes[0].focus();
					popUpMenu.childNodes[0].selectedIndex = -1;
					popUpMenu.childNodes[0].onclick = function( )
					{
					    this.parentNode.style.cssText = 'visibility: hidden; left: -200px; top: -200px;';
					    if(this.selectedIndex < 0 || !this.options[this.selectedIndex].getAttribute('posId')) return;
					    var posId = this.options[this.selectedIndex].getAttribute('posId');
					    var com = '';
					    if(posId == 'add')		com = "<add/>";
					    else if(posId == 'ins')	com = "<ins row='"+this.rowP+"'/>";
					    else if(posId == 'up')	com = "<move row='"+this.rowP+"' to='"+(this.rowP-1)+"'/>";
					    else if(posId == 'down')	com = "<move row='"+this.rowP+"' to='"+(this.rowP+1)+"'/>";
					    else if(posId == 'del') {
						var com = "<del ";
						if(!this.srcNode.getAttribute('key')) com += "row='"+this.rowP+"' ";
						else {
						    var keys = this.srcNode.getAttribute('key').split(',');
						    for(var i_off = 0; i_off < keys.length; i_off++)
							for(var i_el = 0; i_el < this.srcNode.childNodes.length; i_el++)
							    if(this.srcNode.childNodes[i_el].getAttribute('id') == keys[i_off])
							    { com += "key_"+keys[i_off]+"='"+nodeText(this.srcNode.childNodes[i_el].childNodes[this.rowP])+"' "; break; }
						}
						com += "/>";
					    }
					    if(com.length) {
						var rez = servSet(this.itPath,'com=com',com,true);
						if(rez && parseInt(rez.getAttribute('rez')) != 0) alert(nodeText(rez));
						setTimeout('pageRefresh()',200);
					    }
					    return false;
					}
				    }
				    return false;
				}
			    }
			}
			else {
			    cCell.onclick = function( ) {
				var cTbl = this.parentNode.parentNode;
				if(cTbl.isEnter && !this.isEnter) cTbl.setElements();
				return true;
			    }
			    if(i_rw && wr && parseInt(t_s.childNodes[table.childNodes[i_rw].childNodes.length-1].getAttribute('acs'))&SEC_WR)
			    cCell.ondblclick = function( )
			    {
				var cTbl = this.parentNode.parentNode;
				if(cTbl.isEnter) cTbl.setElements();
				this.isEnter = cTbl.isEnter = true;
				var prcCol = cTbl.srcNode.childNodes[this.cCol];
				var cval = nodeText(prcCol.childNodes[this.cRow]);

				if(prcCol.getAttribute('tp') == 'bool') {
				    this.innerHTML = "<input type='checkbox'/>";
				    this.firstChild.checked = parseInt(cval);
				    this.firstChild.onclick = function( )
				    {
					this.parentNode.parentNode.parentNode.comSet(this.parentNode.cRow,this.parentNode.cCol,this.checked?'1':'0');
				    }
				}
				else if(prcCol.getAttribute('dest') == 'select') {
				    this.innerHTML = "<select/>";
				    this.firstChild.onchange = function( )
				    {
					if(this.selectedIndex < 0) return;
					var selId = this.options[this.selectedIndex].getAttribute('vid');
					var selVal = nodeText(this.options[this.selectedIndex]);
					this.parentNode.parentNode.parentNode.comSet(this.parentNode.cRow,this.parentNode.cCol,selId?selId:selVal);
					return false;
				    }
				    var valWCfg = ''; var sel_ok = false;
				    for(var i_el = 0; i_el < prcCol.val_ls.length; i_el++) {
					valWCfg += "<option "+
					    (prcCol.ind_ls.length ? ("vid='"+strEncode(prcCol.ind_ls[i_el])+"' "+((prcCol.ind_ls[i_el]==cval)?"selected='true'":""))
								  : ((prcCol.val_ls[i_el]==cval)?"selected='true'":""))+">"+strEncode(prcCol.val_ls[i_el])+"</option>";
					if((prcCol.ind_ls.length && prcCol.ind_ls[i_el] == cval) || (!prcCol.ind_ls.length && prcCol.val_ls[i_el] == cval))
					    sel_ok = true;
				    }
				    if(!sel_ok) valWCfg += "<option selected='true'>"+strEncode(cval)+"</option>";
				    this.firstChild.innerHTML = valWCfg;
				}
				else if(prcCol.getAttribute('tp') == 'time') {
				    var dt = new Date(parseInt(cval)*1000);
				    this.innerHTML = "<input size='2'/><input size='2'/><input size='4'/>&nbsp;<input size='2'/><input size='2'/><input size='2'/>";
				    this.childNodes[0].onkeyup = this.childNodes[1].onkeyup = this.childNodes[2].onkeyup =
				    this.childNodes[4].onkeyup = this.childNodes[5].onkeyup = this.childNodes[6].onkeyup = function(e)
				    {
					if(!e) e = window.event;
					if(this.parentNode.isEdited && e.keyCode == 13)
					{ this.parentNode.childNodes[7].onclick(); return true; }
					if(this.parentNode.isEdited && e.keyCode == 27) {
					    var val_w = this.parentNode;
					    for(var i_ch = 0; i_ch < val_w.childNodes.length; i_ch++)
						if(val_w.childNodes[i_ch].defaultValue)
						    val_w.childNodes[i_ch].value = val_w.childNodes[i_ch].defaultValue;
					    val_w.removeChild(this.parentNode.childNodes[7]);
					    val_w.isEdited = false;
					    return true;
					}
					if(this.parentNode.isEdited || this.value == this.defaultValue) return true;
					var btOk = document.createElement('img'); btOk.src = '/'+MOD_ID+'/img_button_ok';
					btOk.onclick = function( )
					{
					    var val_w = this.parentNode;
					    var dt = new Date(0);
					    dt.setDate(parseInt(val_w.childNodes[0].value));
					    dt.setMonth(parseInt(val_w.childNodes[1].value)-1);
					    dt.setFullYear(parseInt(val_w.childNodes[2].value));
					    dt.setHours(parseInt(val_w.childNodes[4].value));
					    dt.setMinutes(parseInt(val_w.childNodes[5].value));
					    dt.setSeconds(parseInt(val_w.childNodes[6].value));
					    val_w.parentNode.parentNode.comSet(val_w.cRow,val_w.cCol,Math.floor(dt.getTime()/1000));
					    val_w.removeChild(this);
					    val_w.isEdited = false;
					    return false;
					}
					this.parentNode.appendChild(btOk);
					this.parentNode.isEdited = true;
					return true;
				    }
				    this.childNodes[0].value = this.childNodes[0].defaultValue = dt.getDate();
				    this.childNodes[1].value = this.childNodes[1].defaultValue = dt.getMonth()+1;
				    this.childNodes[2].value = this.childNodes[2].defaultValue = dt.getFullYear();
				    this.childNodes[4].value = this.childNodes[4].defaultValue = dt.getHours();
				    this.childNodes[5].value = this.childNodes[5].defaultValue = dt.getMinutes();
				    this.childNodes[6].value = this.childNodes[6].defaultValue = dt.getSeconds();
				    this.isEdited = false;
				}
				else {
				    if(prcCol.getAttribute('tp') == 'hex') cval = '0x'+parseInt(cval).toString(16);
				    else if(prcCol.getAttribute('tp') == 'oct') cval = '0'+parseInt(cval).toString(8);
				    this.innerHTML = "<input/>";
				    var tp = prcCol.getAttribute('tp');
				    if(prcCol.getAttribute('dest') == 'sel_ed') {
					this.childNodes[0].size = 20;
					var cmbImg = document.createElement('img'); cmbImg.src = '/'+MOD_ID+'/img_combar';
					cmbImg.onclick = function( )
					{
					    if(!this.parentNode.val_ls || !this.parentNode.val_ls.length) return false;
					    var combMenu = getCombo();
					    var optHTML = '';
					    for(var i_l = 0; i_l < this.parentNode.val_ls.length; i_l++)
						optHTML += '<option>'+this.parentNode.val_ls[i_l]+'</option>';
					    var edFld = this.parentNode.childNodes[0];
					    combMenu.childNodes[0].edFld = edFld;
					    combMenu.childNodes[0].innerHTML = optHTML;
					    combMenu.childNodes[0].size = Math.max(3,this.parentNode.val_ls.length);
					    combMenu.style.cssText = 'visibility: visible; left: '+posGetX(edFld,true)+'px; '+
								     'top: '+(posGetY(edFld,true)+edFld.offsetHeight)+'px; '+
								     'width: '+edFld.offsetWidth+'px';
					    combMenu.childNodes[0].focus();
					    combMenu.childNodes[0].onclick = function()
					    {
						this.parentNode.style.cssText = 'visibility: hidden; left: -200px; top: -200px;';
						if(this.selectedIndex < 0) return;
						this.edFld.value = nodeText(this.options[this.selectedIndex]);
						this.edFld.parentNode.parentNode.parentNode.comSet(this.edFld.parentNode.cRow,this.edFld.parentNode.cCol,this.edFld.value);
						return false;
					    }
					    return false;
					}
					tblCell.appendChild(cmbImg);
				    }
				    else if(tp == 'dec') {
					this.className += ' number'; this.childNodes[0].size = 7;
					var spinImg = document.createElement('img'); spinImg.src = '/'+MOD_ID+'/img_spinar';
					spinImg.onclick = function(e)
					{
					    if(!e) e = window.event;
					    var val_w = this.parentNode.childNodes[0];
					    val_w.value = parseInt(val_w.value)+(((e.clientY-posGetY(this))<10)?1:-1);
					    val_w.onkeyup();
					    return false;
					}
					val_w.appendChild(spinImg);
				    }
				    else if(tp == 'hex' || tp == 'oct' || tp == 'real')
				    { this.className += ' number'; this.childNodes[0].size = 7; }
				    else {
					this.childNodes[0].size = 30;
					this.childNodes[0].maxLength = prcCol.getAttribute('len');
					if(!this.childNodes[0].maxLength) this.childNodes[0].maxLength = 1000;
				    }
				    this.prcCol = prcCol;
				    this.childNodes[0].onkeyup = function(e)
				    {
					if(!e) e = window.event;
					if(this.parentNode.isEdited && e.keyCode == 13) { this.parentNode.lastChild.onclick(); return true; }
					if(this.parentNode.isEdited && e.keyCode == 27)	{ this.parentNode.parentNode.parentNode.setElements(); return true; }
					if(this.parentNode.isEdited) return true;
					var btOk = document.createElement('img'); btOk.src = '/'+MOD_ID+'/img_button_ok';
					btOk.onclick = function( )
					{
					    var curVal = this.parentNode.childNodes[0].value;
					    if(this.parentNode.prcCol.getAttribute('tp') == 'hex') curVal = parseInt(curVal,16);
					    else if(this.parentNode.prcCol.getAttribute('tp') == 'oct') curVal = parseInt(curVal,8);
					    this.parentNode.parentNode.parentNode.comSet(this.parentNode.cRow,this.parentNode.cCol,curVal);
					    return false;
					}
					this.parentNode.appendChild(btOk);
					this.parentNode.isEdited = true;
					return true;
				    }
				    this.firstChild.value = cval;
				    this.isEdited = false;
				}
				//   Prevent broken selection
				if(window.getSelection) window.getSelection().removeAllRanges();
				else if(document.selection) document.selection.empty();
				return false;
			    }
			}
			table.childNodes[i_rw].appendChild(cCell);
		    }
		    if(table.childNodes[i_rw].childNodes.length) setNodeText(table.childNodes[i_rw].childNodes[0],i_rw?i_rw:'#');
		}
		table.setElements();
		table.parentNode.style.height = Math.min(300,Math.max(100,table.clientHeight))+'px';
	    }
	}
	//  View standard fields
	else if(t_s.nodeName.toLowerCase() == 'fld') basicFields(t_s,aPath,cBlk,wr);
	else if(t_s.nodeName.toLowerCase() == 'comm') {
	    var brPath = (aPath+t_s.getAttribute('id')).replace(/%/g,'%25').replace(/\//g,'%2f');
	    var dBlk = null; var button = null;
	    if(cBlk) {
		if(t_s.childNodes.length) {
		    dBlk = document.createElement('fieldset');
		    dBlk.appendChild(document.createElement('legend'));
		    dBlk.childNodes[0].appendChild(document.createTextNode(t_s.getAttribute('dscr')));
		}else dBlk = document.createElement('div');
		dBlk.className = 'elem';
		button = document.createElement('input'); button.type = 'button';
		button.srcNode = t_s;
		button.brPath = brPath;
		button.onclick = function( )
		{
		    //Check link
		    if(this.srcNode.getAttribute('tp') == 'lnk') {
			var dataReq = servGet(selPath+'/'+this.brPath,'com=get');
			if(!dataReq) return false;
			else if(parseInt(dataReq.getAttribute('rez')) != 0) { alert(nodeText(dataReq)); return false; }
			selectPage('/'+pathLev(selPath,0)+nodeText(dataReq));
		    }
		    else {
			var com = '<set>';
			for(var f_com = 0; f_com < this.srcNode.childNodes.length; f_com++)
			    com += "<"+this.srcNode.childNodes[f_com].nodeName+" id='"+this.srcNode.childNodes[f_com].getAttribute('id')+"'>"+
			nodeText(this.srcNode.childNodes[f_com])+"</"+this.srcNode.childNodes[f_com].nodeName+">";
			com += '</set>';
			var rez = servSet(selPath+'/'+this.brPath,'com=com',com,true);
			if(rez && parseInt(rez.getAttribute('rez')) != 0) alert(nodeText(rez));
			pageRefresh();
		    }
		    return false;
		}
		dBlk.appendChild(button);
		cBlk.appendChild(dBlk);
		t_s.addr_butt = button;
	    }
	    else button = t_s.addr_butt;

	    //  Update or create parameters
	    for(var f_com = 0; f_com < t_s.childNodes.length; f_com++)
		if(t_s.childNodes[f_com].nodeName.toLowerCase() == 'fld')
		    basicFields(t_s.childNodes[f_com],aPath+t_s.getAttribute('id')+'/',dBlk,true,true);

	    //  Fill command
	    button.value = t_s.getAttribute('dscr');
	    button.title = t_s.getAttribute('help');
	}
    }
}

/***************************************************
 * basicFields - Prepare basic fields view         *
 ***************************************************/
function basicFields( t_s, aPath, cBlk, wr, comm )
{
  var brPath = (aPath+t_s.getAttribute('id')).replace(/%/g,'%25').replace(/\//g,'%2f');

  var dataReq = document.createElement('get');
  if(!comm)
  {
    dataReq = servGet(brPath,'com=get');
    if(!dataReq) dataReq = document.createElement('get');
    else if(parseInt(dataReq.getAttribute('rez'))!=0) { alert(nodeText(dataReq)); setNodeText(dataReq,''); }
  }

  //> View select fields
  if( t_s.getAttribute('dest') == 'select' )
  {
    var lab = null; var val_r = null; var val_w = null;
    if( cBlk )
    {
      //>> View info
      if( !wr )
      {
	val_r = document.createElement('span'); val_r.className = 'const';
	val_r.StatusTip = selPath+'/'+brPath; val_r.onmouseover = function() { setStatus(this.StatusTip,10000); }
      }
      //>> View edit
      else
      {
	val_w = document.createElement('span');	val_w.className = 'line';
	val_w.appendChild(document.createElement('select'));
	val_w.childNodes[0].itPath = selPath+'/'+brPath;
	val_w.childNodes[0].srcNode = t_s;
	val_w.childNodes[0].itComm = comm;
	val_w.childNodes[0].onchange = function( )
	{
	  if( this.selectedIndex < 0 ) return;
	  var selId = this.options[this.selectedIndex].getAttribute('vid');
	  var selVal = nodeText(this.options[this.selectedIndex]);
	  if( this.itComm ) setNodeText(this.srcNode,(selId?selId:selVal));
	  else
	  {
	    var rez = servSet(this.itPath,'com=com','<set>'+strEncode(selId?selId:selVal)+'</set>',true);
	    if( rez && parseInt(rez.getAttribute('rez')) != 0 ) alert(nodeText(rez));
	    setTimeout('pageRefresh()',500);
	  }
	  return false;
	}
	val_w.StatusTip = selPath+'/'+brPath; val_w.onmouseover = function() { setStatus(this.StatusTip,10000); }
      }
      //>> Check use label
      if( t_s.getAttribute('dscr') )
      {
	lab = document.createElement('span'); lab.className = 'label';
	cBlk.dBlk = document.createElement('div'); cBlk.dBlk.className = 'elem'; cBlk.dBlk.appendChild(lab);
	if( val_w ) cBlk.dBlk.appendChild(val_w);
	if( val_r ) cBlk.dBlk.appendChild(val_r);
	cBlk.appendChild(cBlk.dBlk);
      }
      else
      {
	if( val_w ) { if( cBlk.dBlk ) cBlk.dBlk.appendChild(val_w); else { delete val_w; val_w = null; } }
	if( val_r ) { if( cBlk.dBlk ) cBlk.dBlk.appendChild(val_r); else { delete val_r; val_r = null; } }
      }
      t_s.addr_lab = lab; t_s.addr_val_r = val_r; t_s.addr_val_w = val_w;
    }
    else { lab = t_s.addr_lab; val_r = t_s.addr_val_r; val_w = t_s.addr_val_w; }
    //>> Fill combo
    if( lab ) setNodeText(lab,t_s.getAttribute('dscr')+':');
    if( val_w || val_r )
    {
      (val_r||val_w).title = t_s.getAttribute('help');
      var sel_ok = false, c_el = 0;
      if( !t_s.getAttribute('select') )
      {
	var ind_ls = t_s.getAttribute('sel_id') ? t_s.getAttribute('sel_id').split(';') : new Array();
	var val_ls = t_s.getAttribute('sel_list').split(';');
	var valWCfg = '';
	for( var ls_i = 0; ls_i < val_ls.length; ls_i++ )
	{
	  if(val_w) valWCfg+="<option "+
		(ind_ls.length ? ("vid='"+strEncode(ind_ls[ls_i])+"' "+((ind_ls[ls_i]==nodeText(dataReq))?"selected='true'":""))
			       : ((val_ls[ls_i]==nodeText(dataReq))?"selected='true'":""))+">"+strEncode(val_ls[ls_i])+"</option>";
	  if( (ind_ls.length && ind_ls[ls_i] == nodeText(dataReq) ) || (!ind_ls.length && val_ls[ls_i] == nodeText(dataReq)) )
	  { sel_ok = true; if(val_r) setNodeText(val_r,val_ls[ls_i]); }
	}
      }
      else
      {
	var x_lst = servGet(t_s.getAttribute('select').replace(/%/g,'%25').replace(/\//g,'%2f'),'com=get');
	if(x_lst)
	{
	  for(var i_el = 0; i_el < x_lst.childNodes.length; i_el++)
	  {
	    if( x_lst.childNodes[i_el].nodeName.toLowerCase() != 'el' ) continue;
	    var curElId = x_lst.childNodes[i_el].getAttribute('id');
	    var curElVl = nodeText(x_lst.childNodes[i_el]);
	    if( val_w ) valWCfg+="<option "+
		(curElId ? ("vid='"+strEncode(curElId)+"' "+((curElId==nodeText(dataReq))?"selected='true'":""))
			 : ((curElVl==nodeText(dataReq))?"selected='true'":""))+">"+strEncode(curElVl)+"</option>";
	    if( (curElId && curElId == nodeText(dataReq)) || (!curElId && curElVl == nodeText(dataReq)) )
	    { sel_ok = true; if(val_r) setNodeText(val_r,curElVl); }
	  }
	}
      }
      ///>>> Insert empty field if none selected
      if( !sel_ok )
      {
	if(val_w) valWCfg+="<option selected='true'>"+nodeText(dataReq)+"</option>";
	if(val_r) setNodeText(val_r,nodeText(dataReq));
      }
      if( val_w ) val_w.childNodes[0].innerHTML = valWCfg;
    }
  }
  else
  {
    //> View Boolean fields
    if( t_s.getAttribute('tp') == 'bool' )
    {
      var lab = null; var val_r = null; var val_w = null;
      if( cBlk )
      {
	//>> View info
	if( !wr )
	{
	  val_r = document.createElement('span'); val_r.className = 'const';
	  val_r.StatusTip = selPath+'/'+brPath; val_r.onmouseover = function() { setStatus(this.StatusTip,10000); }
	}
	//>> View edit
	else
	{
	  val_w = document.createElement('span'); val_w.className = 'line';
	  val_w.appendChild(document.createElement('input'));
	  val_w.childNodes[0].type = 'checkbox';
	  val_w.childNodes[0].itPath = selPath+'/'+brPath;
	  val_w.childNodes[0].srcNode = t_s;
	  val_w.childNodes[0].itComm = comm;
	  val_w.childNodes[0].onclick = function( )
	  {
	    if( this.itComm ) setNodeText(this.srcNode,(this.checked?'1':'0'));
	    else
	    {
	      var rez = servSet(this.itPath,'com=com','<set>'+(this.checked?'1':'0')+'</set>',true);
	      if( rez && parseInt(rez.getAttribute('rez')) != 0 ) alert(nodeText(rez));
	      setTimeout('pageRefresh()',500);
	    }
	    return false;
	  }
	  val_w.StatusTip = selPath+'/'+brPath; val_w.onmouseover = function() { setStatus(this.StatusTip,10000); }
	}
	//>> Check use label
	if( t_s.getAttribute('dscr') )
	{
	  lab = document.createElement('span'); lab.className = 'label';
	  cBlk.dBlk = document.createElement('div'); cBlk.dBlk.className = 'elem'; cBlk.dBlk.appendChild(lab);
	  if( val_w ) cBlk.dBlk.appendChild(val_w);
	  if( val_r ) cBlk.dBlk.appendChild(val_r);
	  cBlk.appendChild(cBlk.dBlk);
	}
	else
	{
	  if( val_w ) { if( cBlk.dBlk ) cBlk.dBlk.appendChild(val_w); else { delete val_w; val_w = null; } }
	  if( val_r ) { if( cBlk.dBlk ) cBlk.dBlk.appendChild(val_r); else { delete val_r; val_r = null; } }
	}
	t_s.addr_lab = lab; t_s.addr_val_r = val_r; t_s.addr_val_w = val_w;
      }
      else { lab = t_s.addr_lab; val_r = t_s.addr_val_r; val_w = t_s.addr_val_w; }
      //>> Fill CheckBox
      if( lab ) setNodeText(lab,t_s.getAttribute('dscr')+':');
      if( val_w )
      {
	val_w.title = t_s.getAttribute('help');
	val_w.childNodes[0].checked = parseInt(nodeText(dataReq));
      }
      if( val_r )
      {
	val_r.title = t_s.getAttribute('help');
	setNodeText(val_r,nodeText(dataReq)=='<EVAL>'?'<EVAL>':(parseInt(nodeText(dataReq))?'On ':'Off '));
      }
    }
    //> View edit fields
    else if( t_s.getAttribute('tp') == 'str' && (t_s.getAttribute('rows') || t_s.getAttribute('cols')) )
    {
      var lab = null; var edit = null;
      if( cBlk )
      {
	dBlk = document.createElement('div'); dBlk.className = 'elem';
	lab = document.createElement('span'); lab.className = 'label';
	edit = document.createElement('textarea');
	edit.itPath = selPath+'/'+brPath;
	edit.srcNode = t_s;
	edit.itComm = comm;
	edit.readOnly = !wr;
	if( t_s.getAttribute('cols') ) edit.setAttribute('cols',parseInt(t_s.getAttribute('cols')));
	else edit.setAttribute('wrap','off');
	edit.setAttribute('rows',parseInt(t_s.getAttribute('rows')) ? parseInt(t_s.getAttribute('rows')):5);
	edit.onkeyup = function()
	{
	  if( this.itComm ) setNodeText(this.srcNode,this.value);
	  else if( !this.isChanged && this.value != this.defaultValue )
	  {
	    var btBlk = document.createElement('div'); btBlk.style.textAlign = 'right';
	    var btApply = document.createElement('input'); btApply.type = 'button'; btApply.value = 'Apply';
	    btApply.onclick = function()
	    {
	      var wEl = this.parentNode.parentNode;
	      var rez = servSet(wEl.childNodes[2].itPath,'com=com','<set>'+strEncode(wEl.childNodes[2].value,'html')+'</set>',true);
	      if( rez && parseInt(rez.getAttribute('rez')) != 0 ) alert(nodeText(rez));
	      setTimeout('pageRefresh()',500);
	      wEl.removeChild(this.parentNode);
	      wEl.childNodes[2].isChanged = false;
	      return false;
	    }
	    var btCancel = document.createElement('input'); btCancel.type = 'button'; btCancel.value = 'Cancel';
	    btCancel.onclick = function()
	    {
	      var wEl = this.parentNode.parentNode;
	      wEl.childNodes[2].value = wEl.childNodes[2].defaultValue;
	      wEl.removeChild(this.parentNode);
	      wEl.childNodes[2].isChanged = false;
	      return false;
	    }
	    btBlk.appendChild(btApply); btBlk.appendChild(btCancel); this.parentNode.appendChild(btBlk);
	    this.isChanged = true;
	  }
	  else if( this.isChanged && this.value == this.defaultValue && this.parentNode.childNodes[3] )
	  { this.parentNode.removeChild(this.parentNode.childNodes[3]); this.isChanged = false; }
	  return true;
	}
	edit.StatusTip = selPath+'/'+brPath;
	edit.onmouseover = function() { setStatus(this.StatusTip,10000); }
	dBlk.appendChild(lab); dBlk.appendChild(document.createElement('br')); dBlk.appendChild(edit); cBlk.appendChild(dBlk);
	t_s.addr_lab = lab; t_s.addr_edit = edit;
      }
      else { lab = t_s.addr_lab; edit = t_s.addr_edit; }
      //>> Fill Edit
      if( lab ) setNodeText(lab,t_s.getAttribute('dscr')+':');
      if( edit && !edit.isChanged )
      {
	edit.title = t_s.getAttribute('help');
	edit.value = edit.defaultValue = nodeText(dataReq);
      }
    }
    //> View Data-Time fields
    else if( t_s.getAttribute('tp') == 'time' )
    {
      var lab = null; var val_r = null; var val_w = null;
      if( cBlk )
      {
	//>> View info
	if( !wr )
	{
	  val_r = document.createElement('span'); val_r.className = 'const';
	  val_r.StatusTip = selPath+'/'+brPath; val_r.onmouseover = function() { setStatus(this.StatusTip,10000); }
	}
	//>> View edit
	else
	{
	  val_w = document.createElement('span'); val_w.className = 'line number';
	  val_w.itPath = selPath+'/'+brPath;
	  val_w.srcNode = t_s;
	  val_w.itComm = comm;
	  val_w.innerHTML = "<input size='2' style='width: 30px;'/><input size='2' style='width: 30px;'/><input size='4' style='width: 60px;'/>"+
			    "&nbsp;<input size='2' style='width: 30px;'/><input size='2' style='width: 30px;'/><input size='2' style='width: 30px;'/>";
	  val_w.childNodes[0].onkeyup = val_w.childNodes[1].onkeyup = val_w.childNodes[2].onkeyup =
		val_w.childNodes[4].onkeyup = val_w.childNodes[5].onkeyup = val_w.childNodes[6].onkeyup = function(e)
	  {
	    if( this.parentNode.itComm )
	    {
	      var val_w = this.parentNode;
	      var dt = new Date(0);
	      dt.setDate(parseInt(val_w.childNodes[0].value)); dt.setMonth(parseInt(val_w.childNodes[1].value)-1); dt.setFullYear(parseInt(val_w.childNodes[2].value));
	      dt.setHours(parseInt(val_w.childNodes[4].value)); dt.setMinutes(parseInt(val_w.childNodes[5].value)); dt.setSeconds(parseInt(val_w.childNodes[6].value));
	      setNodeText(val_w.srcNode,Math.floor(dt.getTime()/1000));
	      return true;
	    }
	    if( !e ) e = window.event;
	    if( this.parentNode.isEdited && e.keyCode == 13 ) { this.parentNode.childNodes[7].onclick(); return true; }
	    if( this.parentNode.isEdited && e.keyCode == 27 )
	    {
	      var val_w = this.parentNode;
	      for( var i_ch = 0; i_ch < val_w.childNodes.length; i_ch++ )
	        if( val_w.childNodes[i_ch].defaultValue )
	          val_w.childNodes[i_ch].value = val_w.childNodes[i_ch].defaultValue;
	      val_w.removeChild(this.parentNode.childNodes[7]);
	      val_w.isEdited = false;
	      return true;
	    }
	    if( this.parentNode.isEdited || this.value == this.defaultValue ) return true;
	    var btOk = document.createElement('img'); btOk.src = '/'+MOD_ID+'/img_button_ok';
	    btOk.onclick = function( )
	    {
	      var val_w = this.parentNode;
	      var dt = new Date(0);
	      dt.setDate(parseInt(val_w.childNodes[0].value)); dt.setMonth(parseInt(val_w.childNodes[1].value)-1); dt.setFullYear(parseInt(val_w.childNodes[2].value));
	      dt.setHours(parseInt(val_w.childNodes[4].value)); dt.setMinutes(parseInt(val_w.childNodes[5].value)); dt.setSeconds(parseInt(val_w.childNodes[6].value));
	      var rez = servSet(val_w.itPath,'com=com','<set>'+Math.floor(dt.getTime()/1000)+'</set>',true);
	      if( rez && parseInt(rez.getAttribute('rez')) != 0 ) alert(nodeText(rez));
	      setTimeout('pageRefresh()',500);
	      val_w.removeChild(this);
	      val_w.isEdited = false;
	      return false;
	    }
	    this.parentNode.appendChild(btOk);
	    this.parentNode.isEdited = true;
	    return true;
	  }
	  val_w.StatusTip = selPath+'/'+brPath; val_w.onmouseover = function() { setStatus(this.StatusTip,10000); }
	}
	//>> Check use label
	if( t_s.getAttribute('dscr') )
	{
	  lab = document.createElement('span'); lab.className = 'label';
	  cBlk.dBlk = document.createElement('div'); cBlk.dBlk.className = 'elem'; cBlk.dBlk.appendChild(lab);
	  if( val_w ) cBlk.dBlk.appendChild(val_w);
	  if( val_r ) cBlk.dBlk.appendChild(val_r);
	  cBlk.appendChild(cBlk.dBlk);
	}
	else
	{
	  if( val_w ) { if( cBlk.dBlk ) cBlk.dBlk.appendChild(val_w); else { delete val_w; val_w = null; } }
	  if( val_r ) { if( cBlk.dBlk ) cBlk.dBlk.appendChild(val_r); else { delete val_r; val_r = null; } }
	}
	t_s.addr_lab = lab; t_s.addr_val_r = val_r; t_s.addr_val_w = val_w;
      }
      else { lab = t_s.addr_lab; val_r = t_s.addr_val_r; val_w = t_s.addr_val_w; }
      //>> Fill data
      if( lab ) setNodeText(lab,t_s.getAttribute('dscr')+':');
      if( val_w && !val_w.isEdited )
      {
	val_w.title = t_s.getAttribute('help');
	var dt_time_t = parseInt(nodeText(dataReq));
	var dt = new Date(dt_time_t?(dt_time_t*1000):0);
	val_w.childNodes[0].value = val_w.childNodes[0].defaultValue = dt.getDate();
	val_w.childNodes[1].value = val_w.childNodes[1].defaultValue = dt.getMonth()+1;
	val_w.childNodes[2].value = val_w.childNodes[2].defaultValue = dt.getFullYear();
	val_w.childNodes[4].value = val_w.childNodes[4].defaultValue = dt.getHours();
	val_w.childNodes[5].value = val_w.childNodes[5].defaultValue = dt.getMinutes();
	val_w.childNodes[6].value = val_w.childNodes[6].defaultValue = dt.getSeconds();
      }
      if( val_r )
      {
	val_r.title = t_s.getAttribute('help');
	var dt = new Date(parseInt(nodeText(dataReq))*1000);
	setNodeText(val_r,dt.getDate()+'.'+(dt.getMonth()+1)+'.'+dt.getFullYear()+' '+dt.getHours()+':'+
			  ((dt.getMinutes()<10)?('0'+dt.getMinutes()):dt.getMinutes())+':'+((dt.getSeconds()<10)?('0'+dt.getSeconds()):dt.getSeconds()));
      }
    }
    //> View other string and numberic fields
    else
    {
      var lab = null; var val_r = null; var val_w = null;
      if( cBlk )
      {
	//>> View info
	if( !wr )
	{
	  val_r = document.createElement('span'); val_r.className = 'const';
	  val_r.StatusTip = selPath+'/'+brPath; val_r.onmouseover = function() { setStatus(this.StatusTip,10000); }
	}
	//>> View edit
	else
	{
	  val_w = document.createElement('span'); val_w.className = 'line';
	  val_w.itPath = selPath+'/'+brPath;
	  val_w.srcNode = t_s;
	  val_w.itComm = comm;
	  val_w.appendChild(document.createElement('input')); val_w.childNodes[0].type = 'text';
	  val_w.StatusTip = selPath+'/'+brPath; val_w.onmouseover = function() { setStatus(this.StatusTip,10000); }

	  var tp = t_s.getAttribute('tp');
	  if( t_s.getAttribute('dest') == 'sel_ed' )
	  {
	    val_w.childNodes[0].size = 50;
	    var cmbImg = document.createElement('img'); cmbImg.src = '/'+MOD_ID+'/img_combar';
	    cmbImg.onclick = function( )
	    {
	      if( !this.parentNode.sel_list || !this.parentNode.sel_list.length ) return false;
	      var combMenu = getCombo();
	      var optHTML = '';
	      for( var i_l = 0; i_l < this.parentNode.sel_list.length; i_l++ )
		optHTML += '<option>'+this.parentNode.sel_list[i_l]+'</option>';
	      var edFld = this.parentNode.childNodes[0];
	      combMenu.childNodes[0].edFld = edFld;
	      combMenu.childNodes[0].innerHTML = optHTML;
	      combMenu.childNodes[0].size = Math.max(3,this.parentNode.sel_list.length);
	      combMenu.style.cssText = 'visibility: visible; left: '+posGetX(edFld,true)+'px; '+
				       'top: '+(posGetY(edFld,true)+edFld.offsetHeight)+'px; '+
				       'width: '+edFld.offsetWidth+'px';
	      combMenu.childNodes[0].focus();
	      combMenu.childNodes[0].onclick = function()
	      {
		this.parentNode.style.cssText = 'visibility: hidden; left: -200px; top: -200px;';
		if( this.selectedIndex < 0 ) return;
		this.edFld.value = nodeText(this.options[this.selectedIndex]);
		if( this.edFld.parentNode.itComm ) setNodeText(this.edFld.parentNode.srcNode,this.edFld.value);
		else
		{
		  var rez = servSet(this.edFld.parentNode.itPath,'com=com','<set>'+this.edFld.value+'</set>',true);
		  if( rez && parseInt(rez.getAttribute('rez')) != 0 ) alert(nodeText(rez));
		  setTimeout('pageRefresh()',500);
		}
		return false;
	      }
	      return false;
	    }
	    val_w.appendChild(cmbImg);
	  }
	  else if( tp == 'dec' )
	  {
	    val_w.className += ' number';
	    val_w.childNodes[0].size = 7;
	    var spinImg = document.createElement('img'); spinImg.src = '/'+MOD_ID+'/img_spinar';
	    spinImg.onclick = function(e)
	    {
	      if(!e) e = window.event;
	      var val_w = this.parentNode.childNodes[0];
	      val_w.value = parseInt(val_w.value)+(((e.clientY-posGetY(this))<10)?1:-1);
	      val_w.onkeyup();
	      return false;
	    }
	    val_w.appendChild(spinImg);
	  }
	  else if( tp == 'hex' || tp == 'oct' || tp == 'real' ) { val_w.className += ' number'; val_w.childNodes[0].size = 10; }
	  else
	  {
	    val_w.childNodes[0].size = 30;
	    val_w.childNodes[0].maxLength = t_s.getAttribute('len');
	    if( !val_w.childNodes[0].maxLength ) val_w.childNodes[0].maxLength = 1000;
	  }

	  val_w.childNodes[0].onkeyup = function(e)
	  {
	    if( this.parentNode.itComm )
	    {
	      var curVal = this.value;
	      if( this.parentNode.srcNode.getAttribute('tp') == 'hex' ) curVal = parseInt(curVal,16);
	      else if( this.parentNode.srcNode.getAttribute('tp') == 'oct' ) curVal = parseInt(curVal,8);
	      setNodeText(this.parentNode.srcNode,curVal);
	      return true;
	    }
	    if( !e ) e = window.event;
	    if( this.parentNode.isEdited && e.keyCode == 13 ) { this.parentNode.lastChild.onclick(); return true; }
	    if( this.parentNode.isEdited && e.keyCode == 27 )
	    {
	      this.value = this.defaultValue;
	      this.parentNode.isEdited = false;
	      this.parentNode.removeChild(this.parentNode.lastChild);
	      return true;
	    }
	    if( this.parentNode.isEdited || this.value == this.defaultValue ) return true;
	    var btOk = document.createElement('img'); btOk.src = '/'+MOD_ID+'/img_button_ok';
	    btOk.onclick = function( )
	    {
	      var curVal = this.parentNode.childNodes[0].value;
	      if( this.parentNode.srcNode.getAttribute('tp') == 'hex' ) curVal = parseInt(curVal,16);
	      else if( this.parentNode.srcNode.getAttribute('tp') == 'oct' ) curVal = parseInt(curVal,8);
	      var rez = servSet(this.parentNode.itPath,'com=com','<set>'+curVal+'</set>',true);
	      if( rez && parseInt(rez.getAttribute('rez')) != 0 ) alert(nodeText(rez));
	      setTimeout('pageRefresh()',500);
	      this.parentNode.isEdited = false;
	      this.parentNode.removeChild(this);
	      return false;
	    }
	    this.parentNode.appendChild(btOk);
	    this.parentNode.isEdited = true;
	    return true;
	  }
	}
	//>> Check use label
	if( t_s.getAttribute('dscr') )
	{
	  lab = document.createElement('span'); lab.className = 'label';
	  cBlk.dBlk = document.createElement('div'); cBlk.dBlk.className = 'elem'; cBlk.dBlk.appendChild(lab);
	  if( val_w ) cBlk.dBlk.appendChild(val_w);
	  if( val_r ) cBlk.dBlk.appendChild(val_r);
	  cBlk.appendChild(cBlk.dBlk);
	}
	else
	{
	  if( val_w ) { if(cBlk.dBlk) cBlk.dBlk.appendChild(val_w); else { delete val_w; val_w = null; } }
	  if( val_r ) { if(cBlk.dBlk) cBlk.dBlk.appendChild(val_r); else { delete val_r; val_r = null; } }
	}
	t_s.addr_lab = lab; t_s.addr_val_r = val_r; t_s.addr_val_w = val_w;
      }
      else { lab = t_s.addr_lab; val_r = t_s.addr_val_r; val_w = t_s.addr_val_w; }
      //>> Fill line
      var sval = nodeText(dataReq);
      if( t_s.getAttribute('tp') == 'hex' ) sval = '0x'+parseInt(sval).toString(16);
      else if( t_s.getAttribute('tp') == 'oct' ) sval = '0'+parseInt(sval).toString(8);

      if( lab ) setNodeText(lab,t_s.getAttribute('dscr')+':');
      if( val_r )
      {
	val_r.title = t_s.getAttribute('help');
	setNodeText(val_r,sval);
      }
      if( val_w && !val_w.isEdited )
      {
	val_w.title = t_s.getAttribute('help');
	val_w.childNodes[0].value = val_w.childNodes[0].defaultValue = sval;
	//>> Load combo list
	if( t_s.getAttribute('dest') == 'sel_ed' )
	{
	  if( !t_s.getAttribute('select') ) val_w.sel_list = t_s.getAttribute('sel_list').split(';');
	  else
	  {
	    val_w.sel_list = new Array();
	    var x_lst = servGet(t_s.getAttribute('select').replace(/%/g,'%25').replace(/\//g,'%2f'),'com=get');
	    if(x_lst)
	      for(var i_el = 0; i_el < x_lst.childNodes.length; i_el++)
		if(x_lst.childNodes[i_el].nodeName.toLowerCase() == 'el')
		  val_w.sel_list.push(nodeText(x_lst.childNodes[i_el]));
	  }
	}
      }
    }
  }
}
/***************************************************
 * actEnable - Actions enable.                     *
 ***************************************************/
function actEnable( act, vl )
{
  var actEl = document.getElementById(act);
  if( !actEl ) return;
  actEl.className = vl?'active':'inactive';
  actEl.childNodes[0].src = vl ? actEl.childNodes[0].src.replace('filtr=unact','filtr=none') :
				 actEl.childNodes[0].src.replace('filtr=none','filtr=unact');
}
/***************************************************
 * chkStruct - Info page tree check structure.     *
 ***************************************************/
function chkStruct( w_nd, n_nd )
{
  //> Check access
  if( w_nd.getAttribute('acs') != n_nd.getAttribute('acs') ) return true;

  //> Scan deleted nodes
  for( var i_w = 0; i_w < w_nd.childNodes.length; i_w++ )
  {
    var i_n;
    for( i_n = 0; i_n < n_nd.childNodes.length; i_n++ )
      if( w_nd.childNodes[i_w].nodeName.toLowerCase() == n_nd.childNodes[i_n].nodeName.toLowerCase() &&
	    w_nd.childNodes[i_w].getAttribute('id') == n_nd.childNodes[i_n].getAttribute('id') )
	break;
    if( i_n >= n_nd.childNodes.length && w_nd.nodeName.toLowerCase() != 'table' && w_nd.nodeName.toLowerCase() != 'list' )
	return true;
  }

  //> Scan for new nodes and check present nodes
  for( var i_n = 0; i_n < n_nd.childNodes.length; i_n++ )
  {
    var i_w;
    for( i_w = 0; i_w < w_nd.childNodes.length; i_w++ )
      if( w_nd.childNodes[i_w].nodeName.toLowerCase() == n_nd.childNodes[i_n].nodeName.toLowerCase() &&
	    w_nd.childNodes[i_w].getAttribute('id') == n_nd.childNodes[i_n].getAttribute('id') )
	break;
    if( i_w >= w_nd.childNodes.length ) return true;
    else if( chkStruct(w_nd.childNodes[i_w],n_nd.childNodes[i_n]) ) return true;

    //> Check of the description present
    if( (w_nd.childNodes[i_w].getAttribute('dscr') && !n_nd.childNodes[i_n].getAttribute('dscr')) ||
	(!w_nd.childNodes[i_w].getAttribute('dscr') && n_nd.childNodes[i_n].getAttribute('dscr')) )
      return true;

    //> Check base fields destination change
    if( w_nd.childNodes[i_w].nodeName.toLowerCase() == 'fld' &&
	(w_nd.childNodes[i_w].getAttribute('dest') != n_nd.childNodes[i_n].getAttribute('dest') ||
	w_nd.childNodes[i_w].getAttribute('tp') != n_nd.childNodes[i_n].getAttribute('tp')) )
      return true;
  }

  return false;
}
/***************************************************
 * tabSelect - Select page tab                     *
 ***************************************************/
function tabSelect( tab )
{
  if( !tab || tab.className == 'active' ) return;
  for( var i_t = 0; i_t < tab.parentNode.childNodes.length; i_t++ )
    if( tab.parentNode.childNodes[i_t].className == 'active' )
      tab.parentNode.childNodes[i_t].className = '';
  tab.className = 'active';

  for( var i_cf = 0; i_cf < root.childNodes.length; i_cf++ )
    if( root.childNodes[i_cf].nodeName.toLowerCase() == 'area' && nodeText(tab) == root.childNodes[i_cf].getAttribute('dscr') )
    { root.childNodes[i_cf].setAttribute('qview','0'); break; }

  pageCyclRefrStop();
  pageDisplay(selPath);
}
/***************************************************
 * hostsUpdate - Init/update tree hosts.           *
 ***************************************************/
function hostsUpdate( )
{
  var treeRoot = document.getElementById('treeRoot');
  //> Make hosts
  var hostN = servGet('/','com=chlds');
  if( hostN )
  {
    //> Remove no present hosts
    for( var i_top = 0; i_top < treeRoot.childNodes.length; i_top++ )
    {
      var i_h;
      for( i_h = 0; i_h < hostN.childNodes.length; i_h++ )
	if( treeRoot.childNodes[i_top].getAttribute('id') == ('/'+hostN.childNodes[i_h].getAttribute('id')) )
	  break;
      if( i_h >= hostN.childNodes.length ) { treeRoot.removeChild(treeRoot.childNodes[i_top]); i_top--; }
    }
    //> Add/update hosts
    var emptyTree = treeRoot.childNodes.length;
    for( var i = 0; i < hostN.childNodes.length; i++ )
    {
      var liN = null;
      if( !emptyTree )
        for( var i_top = 0; i_top < treeRoot.childNodes.length; i_top++ )
        {
	  if( treeRoot.childNodes[i_top].getAttribute('id') == ('/'+hostN.childNodes[i].getAttribute('id')) )
	  { liN = treeRoot.childNodes[i_top]; break; }
        }
      if( !liN ) { liN = document.createElement('li'); treeRoot.appendChild(liN); liN.isExpand = false; }
      liN.setAttribute('id','/'+hostN.childNodes[i].getAttribute('id'));
      //> Load groups
      liN.grps = new Array();
      for( var i_grp = 0; i_grp < hostN.childNodes[i].childNodes.length; i_grp++ )
	if( hostN.childNodes[i].childNodes[i_grp].nodeName.toLowerCase() == 'grp' )
	  liN.grps.push(hostN.childNodes[i].childNodes[i_grp]);
      //> Init links
      var treeIco = '/'+MOD_ID+'/img_tree'+(liN.isExpand?'Minus':'Plus')+((i!=0)?'Up':'')+((i!=(hostN.childNodes.length-1))?'Down':'');
      var liCont = "<a class='pm' onclick='expand(this.parentNode,!this.parentNode.isExpand); return false;'><img src='"+treeIco+"'/></a>";
      if( parseInt(hostN.childNodes[i].getAttribute('icoSize')) )
	liCont += "<span><img src='/"+MOD_ID+liN.getAttribute('id')+"?com=ico&size=16'/></span>";
 	//liCont += "<span><img height='16px' src='/"+MOD_ID+liN.getAttribute('id')+"?com=ico'/></span>"; 
      liCont += "<span><a onclick='selectPage(this.parentNode.parentNode.getAttribute(\"id\")); return false;' "+
	"onmouseover='setStatus(this.parentNode.parentNode.getAttribute(\"id\"),10000);' href='#'>"+nodeText(hostN.childNodes[i])+"</a></span>";
      liN.innerHTML = liCont;
    }
    if( hostN.childNodes.length ) return '/'+hostN.childNodes[0].getAttribute('id');
  }
}
/**************************************************
 * treeUpdate - Update tree expanded elements.    *
 **************************************************/
function treeUpdate( )
{
  var treeRoot = document.getElementById('treeRoot');
  for( var i_t = 0; i_t < treeRoot.childNodes.length; i_t++ )
    if( treeRoot.childNodes[i_t].isExpand )
      expand(treeRoot.childNodes[i_t],true,true);
}
/**************************************************
 * getPopup - Get popup menu.                     *
 **************************************************/
function getPopup( )
{
  var popUpMenu = document.getElementById('popupmenu');
  if( !popUpMenu )
  {
    popUpMenu = document.createElement('div'); popUpMenu.id = 'popupmenu';
    popUpMenu.appendChild(document.createElement('select'));
    popUpMenu.childNodes[0].onblur = function() { this.parentNode.style.visibility = 'hidden'; }
    document.body.appendChild(popUpMenu);
    popUpMenu.style.visibility = 'hidden';
  }
  return popUpMenu;
}
/**************************************************
 * getCombo - Get combo menu.                     *
 **************************************************/
function getCombo( )
{
  var comboMenu = document.getElementById('combomenu');
  if(!comboMenu)
  {
    comboMenu = document.createElement('div'); comboMenu.id = 'combomenu';
    comboMenu.appendChild(document.createElement('select'));
    comboMenu.childNodes[0].onblur = function() { this.parentNode.style.visibility = 'hidden'; }
    document.body.appendChild(comboMenu);
    comboMenu.style.visibility = 'hidden';
  }
  return comboMenu;
}
/**************************************************
 * pageRefresh - Curent page refrash call.        *
 **************************************************/
function pageRefresh( )
{
  pageDisplay(selPath);
  if( pgRefrTmID ) pgRefrTmID = setTimeout('pageRefresh();',5000);
}
/********************************************************
 * pageCyclRefrStart - Start current page cyclic refrash.*
 ********************************************************/
function pageCyclRefrStart( )
{
  if( !pgRefrTmID ) pgRefrTmID = setTimeout('pageRefresh();',5000);
  actEnable('actUpdate',false); actEnable('actStart',false); actEnable('actStop',true);
}
/********************************************************
 * pageCyclRefrStop - Stop current page cyclic refrash.  *
 ********************************************************/
function pageCyclRefrStop( )
{
  if( pgRefrTmID ) { clearTimeout(pgRefrTmID); pgRefrTmID = null; }
  actEnable('actUpdate',true); actEnable('actStart',true); actEnable('actStop',false);
}
/**************************************************
 * itDBLoad - Load current page from DB.           *
 **************************************************/
function itDBLoad( )
{
  var rez = servSet(selPath+'/%2fobj','com=com','<load/>',true);
  if( rez && parseInt(rez.getAttribute('rez')) != 0 ) alert(nodeText(rez));
  else setTimeout('pageRefresh()',500);
}
/**************************************************
 * itDBSave - Save current page to DB.             *
 **************************************************/
function itDBSave( )
{
  var rez = servSet(selPath+'/%2fobj','com=com','<save/>',true);
  if( rez && parseInt(rez.getAttribute('rez')) != 0 ) alert(nodeText(rez));
  else setTimeout('pageRefresh()',500);
}
/**************************************************
 * pageUp - Select up page.                       *
 **************************************************/
function pageUp( )
{
  var i_l = selPath.length-1;
  while(true)
  {
    var i_l = selPath.lastIndexOf('/',i_l);
    if( i_l == -1 || i_l == 0 ) return;
    if( (selPath.length-i_l) > 1 ) break;
    i_l--;
  }
  selectPage(selPath.substr(0,i_l));
}
/**************************************************
 * pagePrev - Get previous page.                  *
 **************************************************/
function pagePrev( )
{
  if( !ndPrev.length ) return;
  ndNext.push(selPath);
  pageDisplay(ndPrev.pop());
}
/**************************************************
 * pageNext - Get next page.                      *
 **************************************************/
function pageNext( )
{
  if( !ndNext.length ) return;
  ndPrev.push(selPath);
  pageDisplay(ndNext.pop());
}
/**************************************************
 * itAdd - Add new item action process.           *
 **************************************************/
function itAdd( )
{
  if(!selPath.length) return;
  var branchS = null;
  for(var i_ch = 0; i_ch < root.childNodes.length; i_ch++)
    if(root.childNodes[i_ch].nodeName.toLowerCase() == 'branches' && root.childNodes[i_ch].getAttribute('id') == 'br')
      branchS =  root.childNodes[i_ch];
  if(!branchS) return;

  //> Load branches list
  var typeCfg = '';
  for(var i_b = 0; i_b < branchS.childNodes.length; i_b++)
    if(parseInt(branchS.childNodes[i_b].getAttribute('acs'))&SEC_WR)
      typeCfg += "<option idSz='"+branchS.childNodes[i_b].getAttribute('idSz')+
			"' gid='"+branchS.childNodes[i_b].getAttribute('id')+
			"' idm='"+branchS.childNodes[i_b].getAttribute('idm')+"'>"+
		branchS.childNodes[i_b].getAttribute('dscr')+"</option>";
  if(!typeCfg.length) { alert('###No one editable container present.###'); return; }

  dlgWin = ReqIdNameDlg(document.getElementById('actAddIt').childNodes[0].src,("###Add item to node '%1'.###").replace('%1',selPath));
  setNodeText(dlgWin.document.getElementById('wDlgHeader'),'###Add node###');
  dlgWin.document.getElementById('wDlgType').style.display = '';
  dlgWin.document.getElementById('wDlgType').childNodes[1].childNodes[0].innerHTML = typeCfg;
  dlgWin.document.getElementById('wDlgType').childNodes[1].childNodes[0].onchange = function( )
  {
    if(this.selectedIndex < 0) return;
    var idSz = parseInt(this.options[this.selectedIndex].getAttribute('idSz'));
    dlgWin.document.getElementById('wDlgId').childNodes[1].childNodes[0].maxLength = (idSz > 0) ? idSz : 1000;
    var idm = parseInt(this.options[this.selectedIndex].getAttribute('idm'));
    dlgWin.document.getElementById('wDlgName').style.display = idm ? '' : 'none';
    dlgWin.document.getElementById('wDlgName').childNodes[1].childNodes[0].maxLength = (idm > 1) ? idm : 1000;
  }
  dlgWin.document.getElementById('wDlgType').childNodes[1].childNodes[0].onchange();
  dlgWin.document.getElementById('wDlgActOk').onclick = function()
  {
    var tpSel = dlgWin.document.getElementById('wDlgType').childNodes[1].childNodes[0];
    if( !tpSel || tpSel.selectedIndex < 0 ) { document.body.dlgWin.close(); return false; }
    var idm = parseInt(tpSel.options[tpSel.selectedIndex].getAttribute('idm'));
    var gbrId = tpSel.options[tpSel.selectedIndex].getAttribute('gid');
    var inpId = dlgWin.document.getElementById('wDlgId').childNodes[1].childNodes[0].value;
    var inpName = idm ? dlgWin.document.getElementById('wDlgName').childNodes[1].childNodes[0].value : inpId;

    //> Check for already present node
    var req = servSet(selPath+'/%2fbr%2f'+gbrId,'com=com','<get/>',true);
    if( !req || parseInt(req.getAttribute('rez')) != 0 ) { if(req) alert(nodeText(req)); document.body.dlgWin.close(); return false; }
    for( var i_lel = 0; i_lel < req.childNodes.length; i_lel++ )
      if( (req.childNodes[i_lel].getAttribute('id') && req.childNodes[i_lel].getAttribute('id') == inpId) ||
	(!req.childNodes[i_lel].getAttribute('id') && nodeText(req.childNodes[i_lel]) == inpId) )
      { alert(("###Item '%1' already present.###").replace('%1',inpId)); document.body.dlgWin.close(); return; }

    //> Send command
    var rez = servSet(selPath+'/%2fbr%2f'+gbrId,'com=com',"<add "+(idm?("id='"+inpId+"'"):"")+">"+inpName+"</add>",true);
    if( !rez || parseInt(rez.getAttribute('rez')) != 0 ) { if(rez) alert(nodeText(rez)); document.body.dlgWin.close(); return false; }
    document.body.dlgWin.close();
    treeUpdate(); pageRefresh();
    return false;
  }
}
/**************************************************
 * itDel - Delete new item action process.        *
 **************************************************/
function itDel( iit )
{
  var rmit = iit ? iit : selPath;
  if( !rmit || !rmit.length ) return;

  if( !iit && !confirm(("###You sure for delete node '%1'?###").replace('%1',rmit)) ) return;

  var t_el, sel_own = '', sel_el;
  var n_obj = 0;
  for( pathLev.off = 0; (t_el=pathLev(rmit,0,true)).length; n_obj++ )
  { if( n_obj ) sel_own += ('/'+sel_el); sel_el = t_el; }
  if( n_obj > 2 )
  {
    var req = servGet(sel_own+'/%2fbr','com=info');
    if( parseInt(req.getAttribute('rez'))!=0 ) { alert(nodeText(req)); return; }
    if( !req.childNodes[0] ) return;

    var branch = req.childNodes[0];
    for( var i_b = 0; i_b < branch.childNodes.length; i_b++ )
    {
      var b_id = branch.childNodes[i_b].getAttribute('id');
      if( b_id == sel_el.substr(0,b_id.length) && parseInt(branch.childNodes[i_b].getAttribute('acs'))&SEC_WR )
      {
	var idm = parseInt(branch.childNodes[i_b].getAttribute('idm'));
	var com = idm ? ("<del id='"+sel_el.substr(b_id.length)+"'/>") :
	                ("<del>"+sel_el.substr(b_id.length)+"</del>");
	var rez = servSet(sel_own+'/%2fbr%2f'+b_id,'com=com',com,true);
	if( rez && parseInt(rez.getAttribute('rez')) != 0 ) alert(nodeText(rez));
	else { treeUpdate(); if(!iit) { selPath = sel_own; pageDisplay(selPath); } }
	break;
      }
    }
  }
}
/**************************************************
 * itCut - Cut selected item.                     *
 **************************************************/
function itCut( )
{
  copyBuf = '1'+selPath;
  editToolUpdate();
}
/**************************************************
 * itCopy - Copy selected item.                   *
 **************************************************/
function itCopy( )
{
  copyBuf = '0'+selPath;
  editToolUpdate();
}
/**************************************************
 * itPaste - Paste copy or cut item.              *
 **************************************************/
function itPaste( )
{
  var typeCfg = '';
  var itCnt = 0, defIt = 0;
  var s_el, s_elp = '', t_el, b_grp = '';
  var branchS = null;

  //> Src elements calc
  var n_sel = 0;
  for( pathLev.off = 0; (t_el=pathLev(copyBuf.substr(1),0,true)).length; n_sel++ )
  { if(n_sel) s_elp += ('/'+s_el); s_el = t_el; }

  if( pathLev(copyBuf.substr(1),0) != pathLev(selPath,0) ) { alert('###Copy is impossible.###'); return; }

  if(parseInt(root.getAttribute('acs'))&SEC_WR) { typeCfg += "<option idSz='-1' gid=''>###Selected###</option>"; itCnt++; }
  for(var i_ch = 0; i_ch < root.childNodes.length; i_ch++)
    if(root.childNodes[i_ch].nodeName.toLowerCase() == 'branches' && root.childNodes[i_ch].getAttribute('id') == 'br')
      branchS = root.childNodes[i_ch];
  if( branchS )
    for( var i_b = 0; i_b < branchS.childNodes.length; i_b++, itCnt++ )
      if( parseInt(branchS.childNodes[i_b].getAttribute('acs'))&SEC_WR )
      {
	var gbrId = branchS.childNodes[i_b].getAttribute('id');
	typeCfg+="<option idSz='"+branchS.childNodes[i_b].getAttribute('idSz')+"' gid='"+gbrId+"'>"+branchS.childNodes[i_b].getAttribute('dscr')+"</option>";
	if( s_el.substr(0,gbrId.length) == gbrId ) { defIt = itCnt; b_grp = gbrId; }
      }

  //> Make request dialog
  dlgWin = ReqIdNameDlg('/'+MOD_ID+'/img_it_add');
  setNodeText(dlgWin.document.getElementById('wDlgHeader'),'###Move or copy node###');
  if( copyBuf.charAt(0) == '1' )
    setNodeText(dlgWin.document.getElementById('wDlgTitle').childNodes[1],("###Move node '%1' to '%2'.###").replace('%1',copyBuf.substr(1)).replace('%2',selPath));
  else setNodeText(dlgWin.document.getElementById('wDlgTitle').childNodes[1],("###Copy node '%1' to '%2'.###").replace('%1',copyBuf.substr(1)).replace('%2',selPath));
  if( b_grp.length ) dlgWin.document.getElementById('wDlgId').childNodes[1].childNodes[0].value = s_el.substr(b_grp.length);
  dlgWin.document.getElementById('wDlgName').style.display = 'none';
  dlgWin.document.getElementById('wDlgType').style.display = '';
  dlgWin.document.getElementById('wDlgType').childNodes[1].childNodes[0].innerHTML = typeCfg;
  dlgWin.document.getElementById('wDlgType').childNodes[1].childNodes[0].onchange = function( )
  {
    if(this.selectedIndex < 0) return;
    var idSz = parseInt(this.options[this.selectedIndex].getAttribute('idSz'));
    dlgWin.document.getElementById('wDlgId').style.display = (idSz >= 0) ? '' : 'none';
    dlgWin.document.getElementById('wDlgId').childNodes[1].childNodes[0].maxLength = (idSz > 0) ? idSz : 1000;
  }
  dlgWin.document.getElementById('wDlgType').childNodes[1].childNodes[0].selectedIndex = defIt;
  dlgWin.document.getElementById('wDlgType').childNodes[1].childNodes[0].onchange();
  dlgWin.document.getElementById('wDlgActOk').onclick = function()
  {
    var tpSel = dlgWin.document.getElementById('wDlgType').childNodes[1].childNodes[0];
    if( !tpSel || tpSel.selectedIndex < 0 ) { document.body.dlgWin.close(); return false; }
    var idSz = parseInt(tpSel.options[tpSel.selectedIndex].getAttribute('idSz'));
    var gbrId = tpSel.options[tpSel.selectedIndex].getAttribute('gid');
    var inpId = dlgWin.document.getElementById('wDlgId').childNodes[1].childNodes[0].value;

    var statNm, srcNm;
    pathLev.off = 1; statNm = pathLev(copyBuf,0,true); srcNm = copyBuf.substr(pathLev.off);
    pathLev.off = 0; statNm = pathLev(selPath,0,true); dstNm = selPath.substr(pathLev.off);

    if( idSz >= 0 )
    {
      dstNm+='/'+gbrId+inpId;
      //> Check for already present node
      var req = servSet(selPath+'/%2fbr%2f'+gbrId,'com=com','<get/>',true);
      if( !req || parseInt(req.getAttribute('rez')) != 0 ) { if(req) alert(nodeText(req)); document.body.dlgWin.close(); return false; }
      for( var i_lel = 0; i_lel < req.childNodes.length; i_lel++ )
	if( (req.childNodes[i_lel].getAttribute('id') && req.childNodes[i_lel].getAttribute('id') == inpId) ||
	    (!req.childNodes[i_lel].getAttribute('id') && nodeText(req.childNodes[i_lel]) == inpId) )
	{
	  if( confirm(("###Node '%1' already present. Continue?###").replace('%1',dstNm)) ) break;
	  document.body.dlgWin.close(); return;
	}
    }
    //> Copy visual item
    var rez = servSet('/'+statNm+'/%2fobj','com=com',"<copy src='"+srcNm+"' dst='"+dstNm+"'/>",true);
    if( !rez || parseInt(rez.getAttribute('rez')) != 0 ) { if(rez) alert(nodeText(rez)); document.dlgWin.close(); return false; }

    //> Remove source widget
    if( copyBuf.charAt(0) == '1' ) { itDel(copyBuf.substr(1)); copyBuf = '0'; }
    treeUpdate( ); pageRefresh( );
    document.body.dlgWin.close(); return false;
  }
}
/**********************************************************
 * ReqIdNameDlg - Get identifier and name request dialog. *
 **********************************************************/
function ReqIdNameDlg( ico, mess, actPath, nmFile )
{
    var dlgWin = document.body.dlgWin;
    if(!dlgWin) {
	dlgWin = document.createElement('table');
	document.body.dlgWin = dlgWin;
	dlgWin.close = function( ) { document.body.removeChild(this); this.isLoad = null; }
	dlgWin.setAttribute('cellpadding','2');
	dlgWin.setAttribute('cellspacing','0');
	dlgWin.setAttribute('border','0');
	dlgWin.setAttribute('style','position: absolute; font-size: 12px; z-index: 9999; background-color: #8FA9FF; border: 1px solid darkblue; '+
	    'left: '+((window.innerWidth-400-5)/2)+'px; top: '+((this.window.innerHeight-200-18)/2)+'px; ');
    }
    if(dlgWin.isLoad) document.body.removeChild(this);

    dlgWin.innerHTML =
	"<tr style='cursor: move;' onmousedown='this.clX = event.clientX; this.clY = event.clientY; return false;' \n"+
	"   onmouseup='this.clX = this.clY = null; return false;' \n"+
	"   onmouseout='this.clX = this.clY = null; return false;' \n"+
	"   onmousemove='if(this.clX != null) { "+
	"     document.body.dlgWin.style.left = (parseInt(document.body.dlgWin.style.left)+event.clientX-this.clX)+\"px\"; "+
	"     document.body.dlgWin.style.top  = (parseInt(document.body.dlgWin.style.top)+event.clientY-this.clY)+\"px\"; "+
	"     this.clX = event.clientX; this.clY = event.clientY; }'>"+
	" <td id='wDlgHeader' style='padding-left: 5px; max-width: "+(400-10)+"px; width: "+(400-10)+"px; overflow: hidden; white-space: nowrap;'>###Node id and/or name select###</td>\n"+
	" <td style='color: red; cursor: pointer; text-align: right;' onclick='document.body.dlgWin.close();'>X</td>\n"+
	"</tr>\n"+
	"<tr><td style='max-width: 400px;' colspan='2' align='center'>\n"+
	" <center>\n"+
	"  <form id='wDlgFormBlk' action='"+actPath+"' method='post' enctype='multipart/form-data'>\n"+
	"   <table border='0' cellspacing='3px' class='dlg'>\n"+
	"    <tr><td id='wDlgTitle' colspan='2'><img src='' style='height: 32px; float: left;'/><span/></td></tr>\n"+
	"    <tr id='wDlgType'><td>###Element type:###</td><td><select name='type'/></td></tr>\n"+
	"    <tr id='wDlgId'><td>###ID:###</td><td><input name='id'/></td></tr>\n"+
	"    <tr id='wDlgName'><td>###Name:###</td><td><input name='name' type='"+(nmFile?'file':'text')+"'/></td></tr>\n"+
	"    <tr><td colspan='2' style='text-align: right; border-top: 1px solid black; padding-top: 10px;'>\n"+
	"      <input id='wDlgActOk' name='actOk' type='button' value='###Ok###'/> <input id='wDlgActCancel' name='actCancel' type='button' value='###Close###' onclick='document.body.dlgWin.close(); return false;'/>\n"+
	"    </td></tr>\n"+
	"   </table>\n"+
	"  </form>\n"+
	" </center>\n"+
	"</td></tr>";
    document.body.appendChild(dlgWin);
    dlgWin.isLoad = true;

    if(ico) document.getElementById('wDlgTitle').childNodes[0].src = ico;
    if(mess) setNodeText(document.getElementById('wDlgTitle').childNodes[1],mess);

    return window;
}

//First start data init
// Tool bar init
//  Tree update action
var actTreeUpdt = document.getElementById('treeBlk');
if(actTreeUpdt) {
    actTreeUpdt.ondblclick = function()	{ treeUpdate(); }
    actTreeUpdt.onmousedown = function(){ return false; }
}
//  Update actions
var actUpdate = document.getElementById('actUpdate');
if(actUpdate) actUpdate.onclick = function()	{ if(this.className=='active') pageRefresh(); return false; }
var actStart = document.getElementById('actStart');
if(actStart) actStart.onclick = function()	{ if(this.className=='active') pageCyclRefrStart(); return false; }
var actStop = document.getElementById('actStop');
if(actStop) actStop.onclick = function()	{ if(this.className=='active') pageCyclRefrStop(); return false; }
//  DB actions
var actLoad = document.getElementById('actLoad');
if(actLoad) actLoad.onclick = function()	{ if(this.className=='active') itDBLoad(); return false; }
var actSave = document.getElementById('actSave');
if(actSave) actSave.onclick = function()	{ if(this.className=='active') itDBSave(); return false; }
//  Navigate actions
var actUp = document.getElementById('actUp');
if(actUp) actUp.onclick = function()		{ if(this.className=='active') pageUp(); return false; }
var actPrevious = document.getElementById('actPrevious');
if(actPrevious) actPrevious.onclick = function(){ if(this.className=='active') pagePrev(); return false; }
var actNext = document.getElementById('actNext');
if(actNext) actNext.onclick = function()	{ if(this.className=='active') pageNext(); return false; }
//  Create/delete actions
var actAddIt = document.getElementById('actAddIt');
if(actAddIt) actAddIt.onclick = function()	{ if(this.className=='active') itAdd(); return false; }
var actDelIt = document.getElementById('actDelIt');
if(actDelIt) actDelIt.onclick = function()	{ if(this.className=='active') itDel(); return false; }
//  Copy actions
var actCopy = document.getElementById('actCopy');
if(actCopy) actCopy.onclick = function()	{ if(this.className=='active') itCopy(); return false; }
var actCut = document.getElementById('actCut');
if(actCut) actCut.onclick = function()		{ if(this.className=='active') itCut(); return false; }
var actPaste = document.getElementById('actPaste');
if(actPaste) actPaste.onclick = function()	{ if(this.className=='active') itPaste(); return false; }

pageDisplay(hostsUpdate());

setStatus('###Page loaded.###',5000);
