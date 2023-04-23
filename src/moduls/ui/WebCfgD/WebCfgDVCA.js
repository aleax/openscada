
//OpenSCADA system module UI.WebCfgD file: VCA.js
/***************************************************************************
 *   Copyright (C) 2008-2023 by Roman Savochenko, <roman@oscada.org>       *
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
var MOD_ID = 'WebCfgD';	//Module identifier
var stTmID = null;	//Status line timer identifier
var pgRefrTmID = null;	//Periodic current page update timer identifier
var dlgWin = null;	//Opened window dialog
var selPath = '';	//Selected node path
var queSZ = 20;		//Previous and next arrays queue size
var ndPrev = new Array();//Previous nodes array
var ndNext = new Array();//Next nodes array
var pgInfo = document.createElement('info');	//Curent page XML tree
var root = document.createElement('oscada_cntr');//Root page's node
var SEC_XT = 0x01;	//Extended
var SEC_WR = 0x02;	//Write access
var SEC_RD = 0x04;	//Read access
var copyBuf = '0';	//Copy node address buffer
var genReqs = null;	//Generic request object
var limTblItmCnt = 300;	//Limit of the table item content
var Er_NoError = 0;	//Error code of no error
var Er_Core_CntrWarning = 1; //Error code of warning the Control Interface

//Browser type detect
var isNN = navigator.appName.indexOf('Netscape') != -1;
var isIE = navigator.appName.indexOf('Microsoft') != -1;
var isOpera = navigator.appName.indexOf('Opera') != -1;
var isKonq = navigator.userAgent.indexOf('Konqueror') != -1;
var isChrome = navigator.userAgent.indexOf('Chrome') != -1;
var gPrms = window.location.search || '';

/***************************************************
 * strEncode - String encoding.                    *
 ***************************************************/
function strEncode( vl, tp )
{
    //if(!tp || tp == "html") return vl.replace(/&/g,'&amp;').replace(/>/g,'&gt;').replace(/</g,'&lt;').replace(/"/g,'&quot;');
    //return vl;
    var encRez = '';
    if(!tp || tp == "html")
	for(var iEnc = 0; iEnc < vl.length; iEnc++)
	    switch(vl.charAt(iEnc)) {
		case '&': encRez += '&amp;'; break;
		case '>': encRez += '&gt;'; break;
		case '<': encRez += '&lt;'; break;
		case '"': encRez += '&quot;'; break;
		default:  encRez += vl.charAt(iEnc);
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

/*****************************************************
 * nodeTextByTagId - Get DOM node by tag name and id *
 *****************************************************/
function nodeTextByTagId( node, tag, avl )
{
    for(var i = 0; i < node.childNodes.length; i++)
	if(node.childNodes[i].nodeName == tag && node.childNodes[i].getAttribute('id') == avl)
	    return node.childNodes[i].textContent;
    return null;
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
 ***************************************************/
function posGetX( obj, noWScrl )
{
    var posX = 0;
    //Calc offset on blocks
    for(var cObj = obj; cObj != null; cObj = cObj.offsetParent) posX += cObj.offsetLeft;
    //Calc block's scroll
    if(!isOpera)
	for(var cObj = obj ; (cObj && cObj.nodeName != 'BODY'); cObj = cObj.parentNode)
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
	for(var cObj = obj ; (cObj && cObj.nodeName != 'BODY'); cObj = cObj.parentNode)
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
function crDoc( )
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
    req.open('GET', encodeURI('/'+MOD_ID+adr+(gPrms.length?gPrms+'&':'?')+prm),false);
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
    req.open('POST',encodeURI('/'+MOD_ID+adr+(gPrms.length?gPrms+'&':'?')+prm),!waitRez);
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
    req.open('POST',encodeURI('/'+MOD_ID+'/'+(gPrms.length?gPrms+'&':'?')+'com=req'),false);
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
    document.getElementById('status').innerText = mess ? mess : '###Ready###';
    if(!mess) return;
    if(stTmID) clearTimeout(stTmID);
    if(!tm || tm > 0) stTmID = setTimeout('setStatus(null)', tm?tm:1000);
}

/***************************************************
 * alertCntr - Control Interface specific alert.   *
 ***************************************************/
function alertCntr( reqO )
{
    if(reqO) alert(reqO.getAttribute("mtxt") ? reqO.getAttribute("mtxt") : reqO.textContent);
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
	    for(var iG = 0; iG < el.grps.length; iG++) {
		var liN = null;
		// Search present item
		if(upTree)
		    for(var iIt = 0; iIt < cUL.childNodes.length; iIt++)
			if(cUL.childNodes[iIt].grps[0].getAttribute('id') == el.grps[iG].getAttribute('id'))
			{ liN = cUL.childNodes[iIt]; break; }
		if(!liN) {
		    liN = document.createElement('li');
		    if(iG >= cUL.childNodes.length) cUL.appendChild(liN);
		    else cUL.insertBefore(liN, cUL.childNodes[iG]);
		    liN.isExpand = false;
		}
		liN.setAttribute('id', el.getAttribute('id'));
		// Set group
		liN.grps = new Array(); liN.grps.push(el.grps[iG]);
		// Init links
		var isUsable = parseInt(liN.grps[0].getAttribute('chPresent'));
		var treeIco = '/'+MOD_ID+'/img_tree'+(isUsable?(liN.isExpand?'Minus':'Plus'):'')+'Up'+((iG!=(el.grps.length-1))?'Down':'');
		var liCont = isUsable?"<a class='pm' onclick='expand(this.parentNode,!this.parentNode.isExpand); return false;'>":"";
		liCont += "<img src='"+treeIco+"'/></a>";
		liCont += "<span style='font-style: italic;'>"+strEncode(el.grps[iG].getAttribute('dscr'))+":</span>";
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
		for(var iIt = 0; iIt < cUL.childNodes.length; iIt++) {
		    var iG;
		    for(iG = 0; iG < el.grps.length; iG++)
			if(cUL.childNodes[iIt].grps[0].getAttribute('id') == el.grps[iG].getAttribute('id'))
			    break;
		    if(iG >= el.grps.length) { cUL.removeChild(cUL.childNodes[iIt]); iIt--; }
		}
	}
	else {
	    var grpId = el.grps[0].getAttribute('id');
	    var hostN = servGet(el.getAttribute('id'),'com=chlds&grp='+grpId);
	    if(hostN && parseInt(hostN.getAttribute('rez')) == 0) {
		//Add and update present
		for(var iE = 0; iE < hostN.childNodes.length; iE++) {
		    var tmpNdId = el.getAttribute('id')+'/'+
			(grpId+(hostN.childNodes[iE].getAttribute('id')?hostN.childNodes[iE].getAttribute('id'):hostN.childNodes[iE].textContent));
		    var liN = null;
		    // Find item
		    if(upTree)
		    for(var iIt = 0; iIt < cUL.childNodes.length; iIt++)
			if(cUL.childNodes[iIt].getAttribute('id') == tmpNdId)
			{ liN = cUL.childNodes[iIt]; break; }
		    if(!liN) {
			liN = document.createElement('li');
			if(iE >= cUL.childNodes.length) cUL.appendChild(liN);
			else cUL.insertBefore(liN, cUL.childNodes[iE]);
			liN.isExpand = false;
		    }
		    liN.setAttribute('id',tmpNdId);
		    // Load groups
		    liN.grps = new Array();
		    for(var iGrp = 0; iGrp < hostN.childNodes[iE].childNodes.length; iGrp++)
		    if(hostN.childNodes[iE].childNodes[iGrp].nodeName == 'grp')
			liN.grps.push(hostN.childNodes[iE].childNodes[iGrp]);
		    // Init links
		    var isUsable = (liN.grps.length>1)||(liN.grps.length&&parseInt(liN.grps[0].getAttribute('chPresent')));
		    var treeIco = '/'+MOD_ID+'/img_tree'+(isUsable?(liN.isExpand?'Minus':'Plus'):'')+'Up'+((iE!=(hostN.childNodes.length-1))?'Down':'');
		    var liCont = isUsable?"<a class='pm' onclick='expand(this.parentNode,!this.parentNode.isExpand); return false;'>":"";
		    liCont += "<img src='"+treeIco+"'/></a>";
		    if(parseInt(hostN.childNodes[iE].getAttribute('icoSize')))
			liCont += "<span><img src='/"+MOD_ID+liN.getAttribute('id')+"?com=ico&size=16'/></span>"; 
		    //liCont += "<span><img height='16px' src='/"+MOD_ID+liN.getAttribute('id')+"?com=ico'/></span>";
		    liCont += "<span><a onclick='selectPage(this.parentNode.parentNode.getAttribute(\"id\")); return false;' "+
			"onmouseover='setStatus(this.parentNode.parentNode.getAttribute(\"id\"),10000);' href='#'>"+strEncode(hostN.childNodes[iE].textContent)+"</a></span>";
		    // Next node for update
		    if(upTree && liN.isExpand) {
			var liNUL = null;
			for(var iEu = 0; iEu < liN.childNodes.length; iEu++)
			    if(liN.childNodes[iEu].nodeName == 'UL') { liNUL = liN.childNodes[iEu]; break; }
			liN.innerHTML = liCont;
			if(liNUL) liN.appendChild(liNUL);
			expand(liN, val, upTree);
		    } else liN.innerHTML = liCont;
		}
		//Delete no present
		if(upTree)
		    for(var iIt = 0; iIt < cUL.childNodes.length; iIt++) {
			var iE;
			for(iE = 0; iE < hostN.childNodes.length; iE++) {
			    var grpId = el.grps[0].getAttribute('id');
			    var tmpNdId = el.getAttribute('id')+'/'+(grpId+(hostN.childNodes[iE].getAttribute('id')?hostN.childNodes[iE].getAttribute('id'):hostN.childNodes[iE].textContent));
			    if(cUL.childNodes[iIt].getAttribute('id') == tmpNdId) break;
			}
			if(iE >= hostN.childNodes.length) { cUL.removeChild(cUL.childNodes[iIt]); iIt--; }
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
function pageDisplay( path, isComplete )
{
    if(!path) return;

    //Chek Up
    actEnable('actUp', path.lastIndexOf('/') != -1 && path.lastIndexOf('/') != 0);

    //Check Prev and Next
    actEnable('actPrevious', ndPrev.length);
    actEnable('actNext', ndNext.length);

    if(path != pgInfo.getAttribute('path') || isComplete) {
	// Stop refresh
	pageCyclRefrStop();

	if(selPath.length && document.getElementById(selPath)) document.getElementById(selPath).className = '';
	selPath = path;
	document.getElementById('selPath').innerText = selPath;

	pgInfo = servGet(selPath, 'com=info');
	if(parseInt(pgInfo.getAttribute('rez')) != Er_NoError) { alertCntr(pgInfo); return; }
	pgInfo.setAttribute('path', selPath);
	root = pgInfo.childNodes[0];
    }
    else {
	// Check the new node structure and the old node
	var iTree = servGet(selPath,'com=info');
	if(parseInt(iTree.getAttribute('rez')) != Er_NoError) { alertCntr(iTree); return; }
	if(chkStruct(root,iTree.childNodes[0]))
	{ pgInfo = iTree; pgInfo.setAttribute('path',selPath); root = pgInfo.childNodes[0]; }
    }
    if(selPath.length && document.getElementById(selPath)) document.getElementById(selPath).className = 'select';

    actEnable('actManual', root.getAttribute("doc"));
    if(root.getAttribute("doc"))
	actManual.setAttribute("href", servGet("/doc/"+root.getAttribute("doc").split("|")[1].replace("/","%2f")).textContent);

    //Complex request form and it result use
    genReqs = crDoc();
    genReqs.appendChild(genReqs.createElement('CntrReqs'));
    genReqs.childNodes[0].setAttribute('path', selPath);
    genReqs.childNodes[0].setAttribute('fillMode', '1');
    for(var genReqPrc = 0; genReqPrc < 2; genReqPrc++) {
	selectChildRecArea(root, '/', null);

	//Load and Save checking to allow
	var reqModif = servGet('/%2fobj', 'com=modify');
	actEnable('actLoad', reqModif?parseInt(reqModif.textContent):0);
	actEnable('actSave', reqModif?parseInt(reqModif.textContent):0);

	//Send for the prepared generic request
	if(genReqPrc == 0) {
	    genReqs.childNodes[0].setAttribute('fillMode', '0');
	    genReqs = servReq(genReqs);
	}
    }
    genReqs = null;

    //The add and the delete access allow check
    actEnable('actAddIt', false);
    for(var iCh = 0; iCh < root.childNodes.length; iCh++)
	if(root.childNodes[iCh].getAttribute('id') == 'br') {
	    for(var iG = 0; iG < root.childNodes[iCh].childNodes.length; iG++)
		if(parseInt(root.childNodes[iCh].childNodes[iG].getAttribute('acs'))&SEC_WR)
		{ actEnable('actAddIt',true); break; }
	break;
    }
    actEnable('actDelIt',parseInt(root.getAttribute('acs'))&SEC_WR)

    //Edit tools update
    editToolUpdate();
}

/***************************************************
 * editToolUpdate - Edit action state update.      *
 ***************************************************/
function editToolUpdate( )
{
    rootAccess = root.getAttribute('acs') ? parseInt(root.getAttribute('acs')) : 0;
    actEnable('actCut', (selPath.length && (rootAccess&SEC_WR)));
    actEnable('actCopy', (rootAccess>>3)?((rootAccess>>3)&SEC_WR):selPath.length);
    actEnable('actPaste', false);

    //Src and destination elements calc
    if(copyBuf.length <= 1 /*|| copyBuf.substr(1) == selPath || pathLev(copyBuf.substr(1),0) != pathLev(selPath,0)*/) return;
    var sElp; var sEl; var tEl;
    pathLev.off = 0;
    while((tEl=pathLev(copyBuf.substr(1),0,true)).length) { sElp += ('/'+sEl); sEl = tEl; }

    for(var iCh = 0; iCh < root.childNodes.length; iCh++)
	if(root.childNodes[iCh].getAttribute('id') == 'br') {
	    for(var iG = 0; iG < root.childNodes[iCh].childNodes.length; iG++)
		if(parseInt(root.childNodes[iCh].childNodes[iG].getAttribute('acs'))&SEC_WR)
		{ actEnable('actPaste',true); break; }
	    break;
	}
    if(rootAccess&SEC_WR) actEnable('actPaste',true);
}

/***************************************************
 * selectChildRecArea - Make page content          *
 ***************************************************/
function selectChildRecArea( node, aPath, cBlk )
{
    var iArea = 0;
    //View title name
    if(aPath == "/") {
	// Set node icon
	if(nodeTextByTagId(node,'img','ico') != null) {
	    document.getElementById('pgIco').src = '/'+MOD_ID+selPath+'?com=ico&size=32';
	    document.getElementById('pgIco').style.visibility = 'visible';
	}
	else document.getElementById('pgIco').style.visibility = 'hidden';
	// Set title
	document.getElementById('pgTitle').innerText = node.getAttribute('dscr');
	// Delete tabs of deleted areas
	var activeTab = null;
	var tabs = document.getElementById('pgTabs');
	for(var iTbs = 0; iTbs < tabs.childNodes.length; iTbs++) {
	    var iCf;
	    for(iCf = 0; iCf < node.childNodes.length; iCf++)
		if(node.childNodes[iCf].nodeName.toLowerCase() == 'area' &&
			tabs.childNodes[iTbs].innerText == node.childNodes[iCf].getAttribute('dscr'))
		    break;
	    if(iCf >= node.childNodes.length) {
		tabs.removeChild(tabs.childNodes[iTbs]);
		iTbs--;
	    }
	    else if(tabs.childNodes[iTbs].className == 'active') activeTab = tabs.childNodes[iTbs];
	}
	// Add new tabs
	for(var iCf = 0; iCf < node.childNodes.length; iCf++) {
	    if(node.childNodes[iCf].nodeName.toLowerCase() != 'area') continue;
	    var iTbs;
	    for(iTbs = 0; iTbs < tabs.childNodes.length; iTbs++)
		if(tabs.childNodes[iTbs].innerText == node.childNodes[iCf].getAttribute('dscr'))
		    break;
	    if(iTbs >= tabs.childNodes.length) {
		var itab = document.createElement('span');
		itab.innerText = node.childNodes[iCf].getAttribute('dscr');
		itab.onclick = function( ) { tabSelect(this); return false; }
		if(iArea >= tabs.childNodes.length) tabs.appendChild(itab);
		else tabs.insertBefore(itab,tabs.childNodes[iArea]);
		node.childNodes[iCf].setAttribute('qview','0');
	    }
	    iArea++;
	}
	if(!activeTab && tabs.childNodes.length) { activeTab = tabs.childNodes[0]; activeTab.className = 'active'; }
	// Preparing of the active tab
	for(var iCf = 0; iCf < node.childNodes.length; iCf++)
	    if(node.childNodes[iCf].nodeName.toLowerCase() == 'area' &&
		    activeTab.innerText == node.childNodes[iCf].getAttribute('dscr'))
	    {
		var refresh = parseInt(node.childNodes[iCf].getAttribute('qview'));
		var cPg = document.getElementById('pgCont');
		if(!refresh) {
		    while(cPg.childNodes.length) cPg.removeChild(cPg.childNodes[0]);
		    selectChildRecArea(node.childNodes[iCf],aPath+node.childNodes[iCf].getAttribute('id')+'/',cPg);
		    //  Mark last drawed tabs
		    node.childNodes[iCf].setAttribute('qview','1');
		}
		else selectChildRecArea(node.childNodes[iCf],aPath+node.childNodes[iCf].getAttribute('id')+'/',null);

		// Elements of scalable by vertical get and their grow up to the scroll appear into the container
		scrlArea = document.getElementById('gen-pnl-right');
		sclIts = document.querySelectorAll(".elem textarea, .elem div.table, .elem select.list");
		sclFitSz = scrlArea.clientHeight-scrlArea.children[0].offsetHeight;
		for(fitStp = Math.max(5,sclFitSz/(10*Math.max(1,sclIts.length))), iScN = 0; sclFitSz > fitStp; ) {
		    sclIt = null;
		    sclFromBeg = (iScN == 0);
		    for( ; iScN < sclIts.length && !sclIt; iScN++) {
			if(sclIts[iScN].scrollHeight <= sclIts[iScN].clientHeight) continue;
			sclIt = sclIts[iScN];
			sclIt.style.height = (sclIt.offsetHeight-((sclIt.nodeName=="SELECT")?0:getSpareHeight(sclIt))+fitStp)+'px';
			sclFitSz -= fitStp;
		    }
		    if(!sclIt && sclFromBeg) break;
		    if(iScN && iScN >= sclIts.length) iScN = 0;
		}
	    }
	return;
    }
    else for(var iCf = 0; iCf < node.childNodes.length; iCf++) {
	var t_s = node.childNodes[iCf];

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
		val.size = 3;//10;
		val.srcNode = t_s;
		val.itPath = selPath+'/'+brPath;
		val.onmouseover = function() { setStatus(this.itPath,10000); }
		val.onclick = function(e) {
		    e = e ? e : window.event;
		    var popUpMenu = getPopup();
		    var optEl = '';
		    selOK = (this.selectedIndex >= 0 && this.options[this.selectedIndex].label);
		    if(this.srcNode.getAttribute('tp') == 'br' && selOK)
			optEl += "<option posId='go'>###Go###</option><option disabled='true'>------------</option>";
		    if((parseInt(this.srcNode.getAttribute('acs'))&SEC_WR) && this.srcNode.getAttribute('s_com')) {
			if(this.srcNode.getAttribute('s_com').search('add') != -1)
			    optEl += "<option posId='add'>###Add###</option>";
			if(this.srcNode.getAttribute('s_com').search('ins') != -1 && selOK)
			    optEl += "<option posId='ins'>###Insert###</option>";
			if(this.srcNode.getAttribute('s_com').search('edit') != -1 && selOK)
			    optEl += "<option posId='edit'>###Edit###</option>";
			if(this.srcNode.getAttribute('s_com').search('del') != -1 && selOK)
			    optEl += "<option posId='del'>###Delete###</option>";
			if(this.srcNode.getAttribute('s_com').search('move') != -1 && selOK)
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
			popUpMenu.childNodes[0].onclick = function() {
			    this.parentNode.style.cssText = 'visibility: hidden; left: -200px; top: -200px;';
			    if(this.selectedIndex < 0 || !this.options[this.selectedIndex].getAttribute('posId')) return;
			    var idm = parseInt(this.parentNode.srcNode.getAttribute('idm'));
			    var posId = this.options[this.selectedIndex].getAttribute('posId');
			    if(posId == 'go')
				selectPage(selPath+'/'+(this.parentNode.srcNode.getAttribute('br_pref')+this.parentNode.lsId).replace(/%/g,'%25').replace(/\//g,'%2f'));
			    else if(posId == 'add' || posId == 'ins' || posId == 'edit') {
				dlgWin = ReqIdNameDlg('/'+MOD_ID+'/ico');
				dlgWin.document.getElementById('wDlgHeader').innerText = '###Setting item name###';
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
				    dlgWin.document.getElementById('wDlgTitle').childNodes[1].innerText = '###Adding an element.###';
				    actOkFld.onclick = function() {
					var idm = dlgWin.document.getElementById('wDlgName').style.display!='none';
					var inpId = dlgWin.document.getElementById('wDlgId').childNodes[1].childNodes[0].value;
					var inpName = idm ? dlgWin.document.getElementById('wDlgName').childNodes[1].childNodes[0].value : inpId;
					var rez = servSet(this.itPath,'com=com',"<add "+(idm?("id='"+inpId+"'"):"")+">"+inpName+"</add>",true);
					if(rez && parseInt(rez.getAttribute('rez')) != Er_NoError) alertCntr(rez);
					if(this.srcNode.getAttribute('tp') == 'br') treeUpdate();
					pageRefresh();
					document.body.dlgWin.close();
					return false;
				    }
				}
				else if(posId == 'ins') {
				    dlgWin.document.getElementById('wDlgTitle').childNodes[1].innerText = '###Inserting an element.###';
				    actOkFld.onclick = function() {
					var idm = dlgWin.document.getElementById('wDlgName').style.display!='none';
					var inpId = dlgWin.document.getElementById('wDlgId').childNodes[1].childNodes[0].value;
					var inpName = idm ? dlgWin.document.getElementById('wDlgName').childNodes[1].childNodes[0].value : inpId;
					var com = "<ins "+(idm?("id='"+inpId+"' "):"")+" pos='"+this.selectedIndex+"' p_id='"+this.lsId+"'>"+inpName+"</ins>";
					var rez = servSet(this.itPath,'com=com',com,true);
					if(rez && parseInt(rez.getAttribute('rez')) != Er_NoError) alertCntr(rez);
					if(this.srcNode.getAttribute('tp') == 'br') treeUpdate();
					pageRefresh();
					document.body.dlgWin.close();
					return false;
				    }
				}
				else if(posId == 'edit') {
				    dlgWin.document.getElementById('wDlgTitle').childNodes[1].innerText = '###Renaming the element.###';
				    dlgWin.document.getElementById('wDlgId').childNodes[1].childNodes[0].value = idm ? this.parentNode.lsId : this.parentNode.lsText;
				    if(idm) dlgWin.document.getElementById('wDlgName').childNodes[1].childNodes[0].value = this.parentNode.lsText;
				    actOkFld.onclick = function() {
					var idm = dlgWin.document.getElementById('wDlgName').style.display!='none';
					var inpId = dlgWin.document.getElementById('wDlgId').childNodes[1].childNodes[0].value;
					var inpName = idm ? dlgWin.document.getElementById('wDlgName').childNodes[1].childNodes[0].value : inpId;
					var com = "<edit "+(idm?("id='"+inpId+"' "):"")+" pos='"+this.selectedIndex+"' p_id='"+this.lsId+"'>"+inpName+"</edit>";
					var rez = servSet(this.itPath,'com=com',com,true);
					if(rez && parseInt(rez.getAttribute('rez')) != Er_NoError) alertCntr(rez);
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
				if(rez && parseInt(rez.getAttribute('rez')) != Er_NoError) alertCntr(rez);
				pageRefresh();
			    }
			    else if(posId == 'del') {
				var com = idm ? ("<del pos='"+this.selectedIndex+"' id='"+this.parentNode.lsId+"'/>") :
						("<del pos='"+this.selectedIndex+"'>"+this.parentNode.lsText+"</del>");
				var rez = servSet(this.parentNode.itPath,'com=com',com,true);
				if(rez && parseInt(rez.getAttribute('rez')) != Er_NoError) alertCntr(rez);
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
	    lab.innerText = t_s.getAttribute('dscr') + ': ';
	    val.title = (tVl=t_s.getAttribute('help')) ? tVl : "";

	    while(val.childNodes.length) val.removeChild(val.childNodes[0]);
	    var dataReq = servGet(brPath,'com=get');
	    if(!dataReq) continue;
	    if((rezCod=parseInt(dataReq.getAttribute('rez'))) != Er_NoError) {
		alertCntr(dataReq);
		if(rezCod != Er_Core_CntrWarning) continue;
	    }
	    for(var iEl = 0; iEl < dataReq.childNodes.length; iEl++)
		if(dataReq.childNodes[iEl].nodeName.toLowerCase() == 'el') {
		    var opt = document.createElement('option');
		    opt.lsId = dataReq.childNodes[iEl].getAttribute('id');
		    opt.innerText = dataReq.childNodes[iEl].textContent;
		    val.appendChild(opt);
		}
	    while(val.childNodes.length < 3) {
		var opt = document.createElement('option');
		//opt.disabled = true;
		val.appendChild(opt);
	    }
	    val.size = 3; //Math.min(10,Math.max(4,val.childNodes.length));
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
		val.onmouseover = function( ) { setStatus(this.itPath,10000); }
		if(wr) {
		    val.style.cursor = 'pointer';
		    val.onclick = function( ) {
			dlgWin = ReqIdNameDlg('/'+MOD_ID+'/img_save','###Selecting an image file for download to the picture field.###','/'+MOD_ID+this.itPath+'?com=img',true);
			dlgWin.document.getElementById('wDlgHeader').innerText = '###Setting an image###';
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
	    if(lab) lab.innerText = t_s.getAttribute('dscr') + ':';
	    if(val) {
		val.title = (tVl=t_s.getAttribute('help')) ? tVl : "";
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
		dBlk.childNodes[1].style.width = (document.getElementById('pgCont').clientWidth-40)+'px';
		table = document.createElement('table'); table.className = 'elem'; table.cellPadding = 2; table.cellSpacing = 0;
		table.itPath = selPath+'/'+brPath;
		table.onmouseover = function() { setStatus(this.itPath,10000); }
		if(wr) {
		    table.srcNode = t_s;
		    table.comSet = function(row, col, val) {
			var com = "<set col='"+this.srcNode.childNodes[col].getAttribute('id')+"' ";
			if(!this.srcNode.getAttribute('key')) com += "row='" + row + "' ";
			else {
			    var keys = this.srcNode.getAttribute('key').split(',');
			    for(var iOff = 0; iOff < keys.length; iOff++)
				for(var iEl = 0; iEl < this.srcNode.childNodes.length; iEl++)
				    if(this.srcNode.childNodes[iEl].getAttribute('id') == keys[iOff])
				    { com += "key_"+keys[iOff]+"='"+this.srcNode.childNodes[iEl].childNodes[row].textContent+"' "; break; }
			}
			com += ">"+strEncode(val)+"</set>";

			var rez = servSet(this.itPath, 'com=com', com, true);
			if(rez && parseInt(rez.getAttribute('rez')) != Er_NoError) alertCntr(rez);
			else this.srcNode.childNodes[col].childNodes[row].innerText = val;
			if(!parseInt(rez.getAttribute('noReload'))) setTimeout('pageRefresh()', 500);
			else this.setElements(true);
		    }
		}
		dBlk.childNodes[1].appendChild(table);
		cBlk.appendChild(dBlk);

		table.setElements = function( saveCh ) {
		    for(var iCol = 0; iCol < this.srcNode.childNodes.length; iCol++) {
			var prcCol = this.srcNode.childNodes[iCol];
			this.childNodes[0].childNodes[iCol+1].innerText = prcCol.getAttribute('dscr');

			for(var iRow = 0; iRow < prcCol.childNodes.length; iRow++) {
			    var tblCell = this.childNodes[iRow+1].childNodes[iCol+1];

			    //   The list processingLoad selected list
			    var ind_ls = new Array(); var val_ls = new Array();
			    var isSel = false; var isLocSel = false;
			    if((isLocSel=(prcCol.childNodes[iRow].getAttribute('dest') == 'select' || prcCol.childNodes[iRow].getAttribute('dest') == 'sel_ed')) ||
				prcCol.getAttribute('dest') == 'select' || prcCol.getAttribute('dest') == 'sel_ed')
			    {
				isSel = true;
				var selO = isLocSel ? prcCol.childNodes[iRow] : prcCol;
				if(isLocSel || !prcCol.val_ls) {
				    if(!selO.getAttribute('select')) {
					if(selO.getAttribute('sel_id')) ind_ls = selO.getAttribute('sel_id').split(';');
					val_ls = selO.getAttribute('sel_list').split(';');
				    }
				    else {
					var xLst = servGet((genReqs?'':selPath+'/')+selO.getAttribute('select').replace(/%/g,'%25').replace(/\//g,'%2f'),'com=get');
					for(var iEl = 0; xLst && iEl < xLst.childNodes.length; iEl++) {
					    if(xLst.childNodes[iEl].nodeName.toLowerCase() != 'el') continue;
					    if(xLst.childNodes[iEl].getAttribute('id'))
						ind_ls.push(xLst.childNodes[iEl].getAttribute('id'));
					    val_ls.push(xLst.childNodes[iEl].textContent);
					}
				    }
				    if(!prcCol.val_ls) prcCol.val_ls = val_ls, prcCol.ind_ls = ind_ls;
				}
				if(!isLocSel) val_ls = prcCol.val_ls, ind_ls = prcCol.ind_ls;
				else selO.val_ls = val_ls, selO.ind_ls = ind_ls;
			    }

			    //   The item data
			    if(tblCell.isEnter && tblCell.childNodes.length && saveCh)
				prcCol.childNodes[iRow].textContent = tblCell.childNodes[0].value;
			    var cval = prcCol.childNodes[iRow].textContent;
			    tblCell.isEdited = false; tblCell.cRow = iRow; tblCell.cCol = iCol;
			    if(tblCell.isEnter) while(tblCell.childNodes.length) tblCell.removeChild(tblCell.lastChild);
			    tblCell.isEnter = this.isEnter = false;
			    tblCell.className = "";
			    if(prcCol.getAttribute('tp') == 'bool') {
				//tblCell.innerText = parseInt(cval) ? 'On' : 'Off';
				//tblCell.style.fontWeight = parseInt(cval)?'bold':'normal';
				tblCell.innerHTML = parseInt(cval) ? "<img src='/"+MOD_ID+"/img_button_ok'/> " : '';
			    }
			    else if(isSel) {
				tblCell.innerText = cval;
				for(var iEl = 0; iEl < val_ls.length; iEl++)
				    if((ind_ls.length && ind_ls[iEl] == cval) || (!ind_ls.length && val_ls[iEl] == cval))
					tblCell.innerText = val_ls[iEl];
			    }
			    else if(prcCol.getAttribute('tp') == 'time') {
				var dt = new Date(parseInt(cval)*1000);
				tblCell.innerText = i2s(dt.getDate(),10,2)+'.'+i2s(dt.getMonth()+1,10,2)+'.'+i2s(dt.getFullYear(),10,4)+
				    ' '+i2s(dt.getHours(),10,2)+':'+i2s(dt.getMinutes(),10,2)+':'+i2s(dt.getSeconds(),10,2);
			    }
			    else {
				if(prcCol.getAttribute('tp') == 'hex') cval = '0x'+parseInt(cval).toString(16);
				else if(prcCol.getAttribute('tp') == 'oct') cval = '0'+parseInt(cval).toString(8);
				if(cval.length <= limTblItmCnt) tblCell.innerText = cval;
				else tblCell.innerText = cval.slice(0,limTblItmCnt) + "...";
			    }
			}
		    }
		    return false;
		}

		t_s.addr_lab = lab; t_s.addr_tbl = table;
	    }
	    else { table = t_s.addr_tbl; lab = t_s.addr_lab; }

	    //  Fill the table
	    if(lab) lab.innerText = t_s.getAttribute('dscr') + ':';
	    if(table) {
		table.title = (tVl=t_s.getAttribute('help')) ? tVl : "";

		var dataReq = servGet(brPath,'com=get');
		if(dataReq && (rezCod=parseInt(dataReq.getAttribute('rez'))) != Er_NoError) {
		    alertCntr(dataReq);
		    if(rezCod != Er_Core_CntrWarning) continue;
		}

		//   Copy values to the info tree
		for(var iCl = 0; dataReq && iCl < dataReq.childNodes.length; iCl++) {
		    var iCli;
		    for(iCli = 0; iCli < t_s.childNodes.length; iCli++)
			if(dataReq.childNodes[iCl].getAttribute('id') == t_s.childNodes[iCli].getAttribute('id')) break;
		    if(iCli == t_s.childNodes.length) {
			var el = t_s.ownerDocument.createElement(dataReq.childNodes[iCl].nodeName);
			for(var i_a = 0; i_a < dataReq.childNodes[iCl].attributes.length; i_a++)
			    el.setAttribute(dataReq.childNodes[iCl].attributes[i_a].name,dataReq.childNodes[iCl].attributes[i_a].value);
			t_s.appendChild(el);
		    }
		    while(t_s.childNodes[iCli].lastChild) t_s.childNodes[iCli].removeChild(t_s.childNodes[iCli].lastChild);
		    for(var iRw = 0; iRw < dataReq.childNodes[iCl].childNodes.length; iRw++) {
			var el = t_s.ownerDocument.createElement(dataReq.childNodes[iCl].childNodes[iRw].nodeName);
			var dO = dataReq.childNodes[iCl].childNodes[iRw];
			el.textContent = dO.textContent;
			for(iA = 0; iA < dO.attributes.length; iA++)
			    el.setAttribute(dO.attributes[iA].name, dO.attributes[iA].value);
			t_s.childNodes[iCli].appendChild(el);
		    }
		}

		//   Calc rows and columns
		var nCol = t_s.childNodes.length;
		var nRow = nCol ? t_s.childNodes[0].childNodes.length : 0;

		//   Update table structure
		while(table.childNodes.length > (nRow+1)) table.removeChild(table.lastChild);
		while(table.childNodes.length < (nRow+1)) table.appendChild(document.createElement('tr'));

		for(var iRw = 0; iRw < table.childNodes.length; iRw++) {
		    while(table.childNodes[iRw].childNodes.length > (nCol+1))
			table.childNodes[iRw].removeChild(table.childNodes[iRw].lastChild);
		    while(table.childNodes[iRw].childNodes.length < (nCol+1)) {
			if(!table.childNodes[iRw].childNodes.length) {
			    var cCell = document.createElement('th');
			    if(wr && t_s.getAttribute('s_com')) {
				cCell.style.cursor = 'pointer';
				cCell.srcNode = t_s;
				cCell.onclick = function(e) {
				    var rowP = parseInt(this.innerText);
				    if(!e) e = window.event;
				    var popUpMenu = getPopup();
				    var optEl = '';
				    var coms = this.srcNode.getAttribute('s_com').split(',');
				    for(var iCom = 0; iCom < coms.length; iCom++)
					if((tVl=coms[iCom].split(":"))[0] == 'add')
					    optEl += "<option posId='add'>"+((tVl.length>1)?tVl[1]:"###Add row###")+"</option>";
					else if(tVl[0] == 'ins') {
					    if(!rowP) continue;
					    optEl += "<option posId='ins'>"+((tVl.length>1)?tVl[1]:"###Insert row###")+"</option>";
					}
					else if(tVl[0] == 'del') {
					    if(!rowP) continue;
					    optEl += "<option posId='del'>"+((tVl.length>1)?tVl[1]:"###Delete row###")+"</option>";
					}
					else if(tVl[0] == 'move') {
					    if(!rowP) continue;
					    optEl += "<option posId='up'>###Up row###</option><option posId='down'>###Down row###</option>";
					}
					//    User commands
					else optEl += "<option posId='"+tVl[0]+"'>"+((tVl.length>1)?tVl[1]:tVl[0])+"</option>";
				    popUpMenu.childNodes[0].innerHTML = optEl;

				    if(popUpMenu.childNodes[0].childNodes.length) {
					popUpMenu.childNodes[0].srcNode = this.srcNode;
					popUpMenu.childNodes[0].itPath = this.parentNode.parentNode.itPath;
					popUpMenu.childNodes[0].rowP = rowP-1;
					popUpMenu.childNodes[0].size = Math.max(3,popUpMenu.childNodes[0].childNodes.length);
					popUpMenu.style.cssText = 'visibility: visible; left: '+(e.clientX+window.pageXOffset)+'px; top: '+(e.clientY+window.pageYOffset)+'px;';
					popUpMenu.childNodes[0].focus();
					popUpMenu.childNodes[0].selectedIndex = -1;
					popUpMenu.childNodes[0].onclick = function( ) {
					    this.parentNode.style.cssText = 'visibility: hidden; left: -200px; top: -200px;';
					    if(this.selectedIndex < 0 || !this.options[this.selectedIndex].getAttribute('posId')) return;
					    var posId = this.options[this.selectedIndex].getAttribute('posId');
					    var com = '';
					    if(posId == 'add')		com = "<add/>";
					    else if(posId == 'ins')	com = "<ins row='"+this.rowP+"'/>";
					    else if(posId == 'up')	com = "<move row='"+this.rowP+"' to='"+(this.rowP-1)+"'/>";
					    else if(posId == 'down')	com = "<move row='"+this.rowP+"' to='"+(this.rowP+1)+"'/>";
					    else {
						var com;
						if(posId == 'del') com = "<del ";
						else com = "<"+posId+" ";	//User commands

						if(!this.srcNode.getAttribute('key')) com += "row='"+this.rowP+"' ";
						else if(this.rowP) {
						    var keys = this.srcNode.getAttribute('key').split(',');
						    for(var iOff = 0; iOff < keys.length; iOff++)
							for(var iEl = 0; iEl < this.srcNode.childNodes.length; iEl++)
							    if(this.srcNode.childNodes[iEl].getAttribute('id') == keys[iOff])
							    { com += "key_"+keys[iOff]+"='"+this.srcNode.childNodes[iEl].childNodes[this.rowP].textContent+"' "; break; }
						}
						com += "/>";
					    }
					    if(com.length) {
						var rez = servSet(this.itPath,'com=com',com,true);
						if(rez && parseInt(rez.getAttribute('rez')) != Er_NoError) alertCntr(rez);
						if(!parseInt(rez.getAttribute('noReload'))) setTimeout('pageRefresh()', 200);
					    }
					    return false;
					}
				    }
				    return false;
				}
			    }
			}
			else {
			    var cCell = document.createElement(iRw?'td':'th');
			    var iCol = table.childNodes[iRw].childNodes.length-1;
			    if(!iRw) {
				if((tVl=t_s.childNodes[iCol].getAttribute('help'))) cCell.title = tVl;
			    }
			    else {
				if((tVl=t_s.childNodes[iCol].childNodes[iRw-1].getAttribute('help'))) cCell.title = tVl;
				else if(table.childNodes[0].childNodes[iCol+1].title) cCell.title = table.childNodes[0].childNodes[iCol+1].title;
				var colTp = t_s.childNodes[iCol].getAttribute('tp');
				var colDst = t_s.childNodes[iCol].childNodes[iRw-1].getAttribute('dest');
				if(!colDst) colDst = t_s.childNodes[iCol].getAttribute('dest');
				cCell.style.textAlign = (colTp == "bool" || colTp == "dec" || colTp == "real" || colTp == "time" || colDst == "sel_ed" || colDst == "select") ? "center" : "";
			    }
			    cCell.onclick = function( ) {
				var cTbl = this.parentNode.parentNode;
				if(cTbl.isEnter && !this.isEnter) cTbl.setElements();
				return true;
			    }
			    if(iRw && wr && parseInt(t_s.childNodes[iCol].getAttribute('acs'))&SEC_WR)
			    cCell.ondblclick = function( ) {
				var cTbl = this.parentNode.parentNode;
				if(cTbl.isEnter) cTbl.setElements();
				this.isEnter = cTbl.isEnter = true;
				this.className = "edit";
				var prcCol = cTbl.srcNode.childNodes[this.cCol];
				var cval = prcCol.childNodes[this.cRow].textContent;
				var isLocSel = false;

				if(prcCol.getAttribute('tp') == 'bool') {
				    this.innerHTML = "<input type='checkbox'/>";
				    this.firstChild.checked = parseInt(cval);
				    this.firstChild.onclick = function( ) {
					this.parentNode.parentNode.parentNode.comSet(this.parentNode.cRow,this.parentNode.cCol,this.checked?'1':'0');
				    }
				}
				else if((isLocSel=prcCol.childNodes[this.cRow].getAttribute('dest')) || prcCol.getAttribute('dest') == 'select') {
				    var selO = isLocSel ? prcCol.childNodes[this.cRow] : prcCol;
				    this.innerHTML = "<select/>";
				    this.firstChild.onchange = function( ) {
					if(this.selectedIndex < 0) return;
					var selId = this.options[this.selectedIndex].getAttribute('vid');
					var selVal = this.options[this.selectedIndex].innerText;
					this.parentNode.parentNode.parentNode.comSet(this.parentNode.cRow,this.parentNode.cCol,selId?selId:selVal);
					return false;
				    }
				    var valWCfg = ''; var sel_ok = false;
				    for(var iEl = 0; iEl < selO.val_ls.length; iEl++) {
					valWCfg += "<option "+
					    (selO.ind_ls.length ? ("vid='"+strEncode(selO.ind_ls[iEl])+"' "+((selO.ind_ls[iEl]==cval)?"selected='true'":""))
								  : ((selO.val_ls[iEl]==cval)?"selected='true'":""))+">"+strEncode(selO.val_ls[iEl])+"</option>";
					if((selO.ind_ls.length && selO.ind_ls[iEl] == cval) || (!selO.ind_ls.length && selO.val_ls[iEl] == cval))
					    sel_ok = true;
				    }
				    if(!sel_ok) valWCfg += "<option selected='true'>"+strEncode(cval)+"</option>";
				    this.firstChild.innerHTML = valWCfg;
				}
				else if(prcCol.getAttribute('tp') == 'time') {
				    this.apply = function( ) {
					var dt = new Date(0);
					dt.setDate(parseInt(this.childNodes[0].value));
					dt.setMonth(parseInt(this.childNodes[1].value)-1);
					dt.setFullYear(parseInt(this.childNodes[2].value));
					dt.setHours(parseInt(this.childNodes[4].value));
					dt.setMinutes(parseInt(this.childNodes[5].value));
					dt.setSeconds(parseInt(this.childNodes[6].value));
					this.parentNode.parentNode.comSet(this.cRow,this.cCol,Math.floor(dt.getTime()/1000));
					return false;
				    }
				    this.innerHTML = "<input size='2'/><input size='2'/><input size='4'/>&nbsp;<input size='2'/><input size='2'/><input size='2'/>";
				    this.childNodes[0].onkeyup = this.childNodes[1].onkeyup = this.childNodes[2].onkeyup =
				    this.childNodes[4].onkeyup = this.childNodes[5].onkeyup = this.childNodes[6].onkeyup = function(e) {
					if(!e) e = window.event;
					if(e.keyCode == 13) { this.parentNode.apply(); return true; }
					if(e.keyCode == 27) {
					    var val_w = this.parentNode;
					    for(var iCh = 0; iCh < val_w.childNodes.length; iCh++)
						if(val_w.childNodes[iCh].defaultValue)
						    val_w.childNodes[iCh].value = val_w.childNodes[iCh].defaultValue;
					    val_w.removeChild(this.parentNode.childNodes[7]);
					    return true;
					}
					return true;
				    }
				    var dt = new Date(parseInt(cval)*1000);
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
					cmbImg.onclick = function( ) {
					    if(!this.parentNode.val_ls || !this.parentNode.val_ls.length) return false;
					    var combMenu = getCombo();
					    var optHTML = '';
					    for(var iL = 0; iL < this.parentNode.val_ls.length; iL++)
						optHTML += '<option>'+this.parentNode.val_ls[iL]+'</option>';
					    var edFld = this.parentNode.childNodes[0];
					    combMenu.childNodes[0].edFld = edFld;
					    combMenu.childNodes[0].innerHTML = optHTML;
					    combMenu.childNodes[0].size = Math.max(3,this.parentNode.val_ls.length);
					    combMenu.style.cssText = 'visibility: visible; left: '+posGetX(edFld,true)+'px; '+
								     'top: '+(posGetY(edFld,true)+edFld.offsetHeight)+'px; '+
								     'width: '+edFld.offsetWidth+'px';
					    combMenu.childNodes[0].focus();
					    combMenu.childNodes[0].onclick = function() {
						this.parentNode.style.cssText = 'visibility: hidden; left: -200px; top: -200px;';
						if(this.selectedIndex < 0) return;
						this.edFld.value = this.options[this.selectedIndex].innerText;
						this.edFld.parentNode.parentNode.parentNode.comSet(this.edFld.parentNode.cRow,this.edFld.parentNode.cCol,this.edFld.value);
						return false;
					    }
					    return false;
					}
					tblCell.appendChild(cmbImg);
				    }
				    else if(tp == 'dec') { this.className += ' number'; this.childNodes[0].size = 10; }
				    else if(tp == 'hex' || tp == 'oct' || tp == 'real') { this.className += ' number'; this.childNodes[0].size = 10; }
				    else this.innerHTML = "<textarea/>";
				    this.prcCol = prcCol;
				    this.apply = function( ) {
					var curVal = this.childNodes[0].value;
					if(this.prcCol.getAttribute('tp') == 'hex') curVal = parseInt(curVal, 16);
					else if(this.prcCol.getAttribute('tp') == 'oct') curVal = parseInt(curVal, 8);
					this.parentNode.parentNode.comSet(this.cRow, this.cCol, curVal);
					return false;
				    }
				    this.childNodes[0].onkeydown = function(e) {
					if(e.keyCode == 13) {
					    e.preventDefault();
					    if(this.nodeName == "TEXTAREA" && e.ctrlKey) {	//NewLine insertion
						var selStart = this.selectionStart;
						this.value = this.value.slice(0, selStart) + "\n" + this.value.slice(this.selectionEnd);
						this.setSelectionRange(selStart + 1, selStart + 1);
						this.blur(); this.focus();
						return true;
					    }
					    this.parentNode.apply();
					}
					if(e.keyCode == 27) this.parentNode.parentNode.parentNode.setElements();
				    }
				    this.firstChild.value = cval;
				    this.childNodes[0].focus();
				}
				//   Prevent for wrong selection
				if(window.getSelection) window.getSelection().removeAllRanges();
				else if(document.selection) document.selection.empty();
				return false;
			    }
			}
			table.childNodes[iRw].appendChild(cCell);
		    }
		    if(table.childNodes[iRw].childNodes.length)
			table.childNodes[iRw].childNodes[0].innerText = iRw ? iRw : '#';
		}
		table.srcNode = t_s;
		table.setElements();
		table.parentNode.style.height = "100px"; //Math.min(300,Math.max(100,table.clientHeight))+'px';
	    }
	}
	//  View standard fields
	else if(t_s.nodeName.toLowerCase() == 'fld') basicFields(t_s, aPath, cBlk, wr);
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
		button.onclick = function( ) {
		    //Check link
		    if(this.srcNode.getAttribute('tp') == 'lnk') {
			var dataReq = servGet(selPath+'/'+this.brPath,'com=get');
			if(!dataReq) return false;
			else if(parseInt(dataReq.getAttribute('rez')) != Er_NoError) { alertCntr(dataReq); return false; }
			selectPage('/'+pathLev(selPath,0)+dataReq.textContent);
		    }
		    else {
			var com = '<set>';
			for(var f_com = 0; f_com < this.srcNode.childNodes.length; f_com++)
			    com += "<"+this.srcNode.childNodes[f_com].nodeName+" id='"+this.srcNode.childNodes[f_com].getAttribute('id')+"'>"+
			this.srcNode.childNodes[f_com].textContent+"</"+this.srcNode.childNodes[f_com].nodeName+">";
			com += '</set>';
			var rez = servSet(selPath+'/'+this.brPath,'com=com',com,true);
			if(rez && parseInt(rez.getAttribute('rez')) != Er_NoError) alertCntr(rez);
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
		    basicFields(t_s.childNodes[f_com], aPath+t_s.getAttribute('id')+'/', dBlk, true, true);

	    //  Fill command
	    button.value = t_s.getAttribute('dscr');
	    button.title = (tVl=t_s.getAttribute('help')) ? tVl : "";
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
    if(!comm) {
	dataReq = servGet(brPath,'com=get');
	if(!dataReq) dataReq = document.createElement('get');
	else if((rezCod=parseInt(dataReq.getAttribute('rez'))) != Er_NoError) {
	    alertCntr(dataReq);
	    if(!dataReq.getAttribute("mtxt")) dataReq.textContent = '';
	}
    }

    //View select fields
    if(t_s.getAttribute('dest') == 'select') {
	var lab = null; var val_r = null; var val_w = null;
	if(cBlk) {
	    // View info
	    if(!wr) {
		val_r = document.createElement('span'); val_r.className = 'const';
		val_r.StatusTip = selPath+'/'+brPath; val_r.onmouseover = function() { setStatus(this.StatusTip,10000); }
	    }
	    // View edit
	    else {
		val_w = document.createElement('span');	val_w.className = 'line';
		val_w.appendChild(document.createElement('select'));
		val_w.childNodes[0].itPath = selPath+'/'+brPath;
		val_w.childNodes[0].srcNode = t_s;
		val_w.childNodes[0].itComm = comm;
		val_w.childNodes[0].onchange = function( ) {
		    if(this.selectedIndex < 0) return;
		    var selId = this.options[this.selectedIndex].getAttribute('vid');
		    var selVal = this.options[this.selectedIndex].innerText;
		    if(this.itComm) this.srcNode.textContent = selId ? selId : selVal;
		    else {
			var rez = servSet(this.itPath, 'com=com', '<set>'+strEncode(selId?selId:selVal)+'</set>', true);
			if(rez && parseInt(rez.getAttribute('rez')) != Er_NoError) alertCntr(rez);
			setTimeout('pageRefresh()', 500);
		    }
		    return false;
		}
		val_w.StatusTip = selPath+'/'+brPath; val_w.onmouseover = function() { setStatus(this.StatusTip,10000); }
	    }
	    // Check use label
	    if(t_s.getAttribute('dscr')) {
		lab = document.createElement('span'); lab.className = 'label';
		cBlk.dBlk = document.createElement('div'); cBlk.dBlk.className = 'elem'; cBlk.dBlk.appendChild(lab);
		if(val_w) cBlk.dBlk.appendChild(val_w);
		if(val_r) cBlk.dBlk.appendChild(val_r);
		cBlk.appendChild(cBlk.dBlk);
	    }
	    else {
		if(val_w) { if(cBlk.dBlk) cBlk.dBlk.appendChild(val_w); else { delete val_w; val_w = null; } }
		if(val_r) { if(cBlk.dBlk) cBlk.dBlk.appendChild(val_r); else { delete val_r; val_r = null; } }
	    }
	    t_s.addr_lab = lab; t_s.addr_val_r = val_r; t_s.addr_val_w = val_w;
	}
	else { lab = t_s.addr_lab; val_r = t_s.addr_val_r; val_w = t_s.addr_val_w; }
	// Fill combo
	if(lab) lab.innerText = t_s.getAttribute('dscr') + ':';
	if(val_w || val_r) {
	    (val_r||val_w).title = (tVl=t_s.getAttribute('help')) ? tVl : "";
	    var sel_ok = false, c_el = 0;
	    if(!t_s.getAttribute('select')) {
		var ind_ls = t_s.getAttribute('sel_id') ? t_s.getAttribute('sel_id').split(';') : new Array();
		var val_ls = t_s.getAttribute('sel_list').split(';');
		var valWCfg = '';
		for(var ls_i = 0; ls_i < val_ls.length && val_ls[ls_i].length && (!ind_ls.length || ls_i < ind_ls.length); ls_i++) {
		    if(val_w) valWCfg += "<option "+
			(ind_ls.length ? ("vid='"+strEncode(ind_ls[ls_i])+"' "+((ind_ls[ls_i]==dataReq.textContent)?"selected='true'":""))
			    : ((val_ls[ls_i]==dataReq.textContent)?"selected='true'":"")) + ">" + strEncode(val_ls[ls_i]) + "</option>";
		    if((ind_ls.length && ind_ls[ls_i] == dataReq.textContent) || (!ind_ls.length && val_ls[ls_i] == dataReq.textContent))
		    { sel_ok = true; if(val_r) val_r.innerText = val_ls[ls_i]; }
		}
	    }
	    else {
		var xLst = servGet(t_s.getAttribute('select').replace(/%/g,'%25').replace(/\//g,'%2f'),'com=get');
		if(xLst) {
		    for(var iEl = 0; iEl < xLst.childNodes.length; iEl++) {
			if(xLst.childNodes[iEl].nodeName.toLowerCase() != 'el') continue;
			var curElId = xLst.childNodes[iEl].getAttribute('id');
			var curElVl = xLst.childNodes[iEl].textContent;
			if(val_w) valWCfg += "<option " +
			    (curElId ? ("vid='"+strEncode(curElId)+"' "+((curElId==dataReq.textContent)?"selected='true'":""))
				     : ((curElVl==dataReq.textContent)?"selected='true'":"")) + ">" + strEncode(curElVl) + "</option>";
			if((curElId && curElId == dataReq.textContent) || (!curElId && curElVl == dataReq.textContent))
			{ sel_ok = true; if(val_r) val_r.innerText = curElVl; }
		    }
		}
	    }
	    //  Insert empty field if none selected
	    if(!sel_ok) {
		if(val_w) valWCfg += "<option selected='true'>" + dataReq.textContent + "</option>";
		if(val_r) val_r.innerText = dataReq.textContent;
	    }
	    if(val_w) val_w.childNodes[0].innerHTML = valWCfg;
	}
    }
    else {
	//View Boolean fields
	if(t_s.getAttribute('tp') == 'bool') {
	    var lab = null; var val_r = null; var val_w = null;
	    if(cBlk) {
		// View info
		if(!wr) {
		    val_r = document.createElement('span'); val_r.className = 'const';
		    val_r.StatusTip = selPath+'/'+brPath; val_r.onmouseover = function() { setStatus(this.StatusTip,10000); }
		}
		// View edit
		else {
		    val_w = document.createElement('span'); val_w.className = 'line';
		    val_w.appendChild(document.createElement('input'));
		    val_w.childNodes[0].type = 'checkbox';
		    val_w.childNodes[0].itPath = selPath+'/'+brPath;
		    val_w.childNodes[0].srcNode = t_s;
		    val_w.childNodes[0].itComm = comm;
		    val_w.childNodes[0].onclick = function( ) {
			if(this.itComm) this.srcNode.textContent = this.checked ? '1' : '0';
			else {
			    var rez = servSet(this.itPath, 'com=com', '<set>'+(this.checked?'1':'0')+'</set>', true);
			    if(rez && parseInt(rez.getAttribute('rez')) != Er_NoError) alertCntr(rez);
			    setTimeout('pageRefresh()', 500);
			}
			return false;
		    }
		    val_w.StatusTip = selPath + '/' + brPath; val_w.onmouseover = function() { setStatus(this.StatusTip,10000); }
		}
		// Check use label
		if(t_s.getAttribute('dscr')) {
		    lab = document.createElement('span'); lab.className = 'label';
		    cBlk.dBlk = document.createElement('div'); cBlk.dBlk.className = 'elem'; cBlk.dBlk.appendChild(lab);
		    if(val_w) cBlk.dBlk.appendChild(val_w);
		    if(val_r) cBlk.dBlk.appendChild(val_r);
		    cBlk.appendChild(cBlk.dBlk);
		}
		else {
		    if(val_w) { if(cBlk.dBlk) cBlk.dBlk.appendChild(val_w); else { delete val_w; val_w = null; } }
		    if(val_r) { if(cBlk.dBlk) cBlk.dBlk.appendChild(val_r); else { delete val_r; val_r = null; } }
		}
		t_s.addr_lab = lab; t_s.addr_val_r = val_r; t_s.addr_val_w = val_w;
	    }
	    else { lab = t_s.addr_lab; val_r = t_s.addr_val_r; val_w = t_s.addr_val_w; }
	    // Fill CheckBox
	    if(lab) lab.innerText = t_s.getAttribute('dscr') + ':';
	    if(val_w) {
		val_w.title = (tVl=t_s.getAttribute('help')) ? tVl : "";
		val_w.childNodes[0].checked = parseInt(dataReq.textContent);
	    }
	    if(val_r) {
		val_r.title = (tVl=t_s.getAttribute('help')) ? tVl : "";
		val_r.innerText = (dataReq.textContent=='<EVAL>') ? '<EVAL>' : (parseInt(dataReq.textContent)?'On ':'Off ');
	    }
	}
	//View edit fields
	else if(t_s.getAttribute('tp') == 'str' && (t_s.getAttribute('rows') || t_s.getAttribute('cols'))) {
	    var lab = null; var edit = null;
	    if(cBlk) {
		dBlk = document.createElement('div'); dBlk.className = 'elem';
		lab = document.createElement('span'); lab.className = 'label';
		edit = document.createElement('textarea');
		edit.itPath = selPath+'/'+brPath;
		edit.srcNode = t_s;
		edit.itComm = comm;
		edit.readOnly = !wr;
		if(t_s.getAttribute('cols')) edit.setAttribute('cols', parseInt(t_s.getAttribute('cols')));
		else edit.setAttribute('wrap', 'off');
		edit.setAttribute('rows',parseInt(t_s.getAttribute('rows')) ? parseInt(t_s.getAttribute('rows')):5);
		edit.onkeyup = function(e) {
		    if(this.isChanged && e.keyCode == 13 && e.ctrlKey) { this.parentNode.childNodes[3].childNodes[0].onclick(); return true; }
		    if(this.isChanged && e.keyCode == 27) { this.parentNode.children[3].children[1].onclick(); return true; }

		    if(this.itComm) this.srcNode.textContent = this.value;
		    else if(!this.isChanged && this.value != this.defaultValue) {
			var btBlk = document.createElement('div'); //btBlk.style.textAlign = 'right';
			var btApply = document.createElement('input'); btApply.type = 'button'; btApply.value = 'Apply';
			btApply.onclick = function( ) {
			    var wEl = this.parentNode.parentNode;
			    var rez = servSet(wEl.childNodes[2].itPath, 'com=com', '<set>'+strEncode(wEl.childNodes[2].value,'html')+'</set>', true);
			    if(rez && parseInt(rez.getAttribute('rez')) != Er_NoError) alertCntr(rez);
			    else wEl.childNodes[2].defaultValue = wEl.childNodes[2].value;
			    setTimeout('pageRefresh()',500);
			    wEl.removeChild(this.parentNode);
			    wEl.childNodes[2].isChanged = false;
			    return false;
			}
			var btCancel = document.createElement('input'); btCancel.type = 'button'; btCancel.value = 'Cancel';
			btCancel.onclick = function( ) {
			    var wEl = this.parentNode.parentNode;
			    wEl.childNodes[2].value = wEl.childNodes[2].defaultValue;
			    wEl.removeChild(this.parentNode);
			    wEl.childNodes[2].isChanged = false;
			    return false;
			}
			btBlk.appendChild(btApply); btBlk.appendChild(btCancel); this.parentNode.appendChild(btBlk);
			this.isChanged = true;
		    }
		    else if(this.isChanged && this.value == this.defaultValue && this.parentNode.childNodes[3])
		    { this.parentNode.removeChild(this.parentNode.childNodes[3]); this.isChanged = false; }
		    return true;
		}
		edit.StatusTip = selPath + '/' + brPath;
		edit.onmouseover = function() { setStatus(this.StatusTip,10000); }
		dBlk.appendChild(lab); dBlk.appendChild(document.createElement('br')); dBlk.appendChild(edit); cBlk.appendChild(dBlk);
		t_s.addr_lab = lab; t_s.addr_edit = edit;
	    }
	    else { lab = t_s.addr_lab; edit = t_s.addr_edit; }
	    // Fill Edit
	    if(lab) lab.innerText = t_s.getAttribute('dscr') + ':';
	    if(edit && !edit.isChanged) {
		edit.title = (tVl=t_s.getAttribute('help')) ? tVl : "";
		edit.value = edit.defaultValue = dataReq.textContent;
	    }
	}
	//View Data-Time fields
	else if(t_s.getAttribute('tp') == 'time') {
	    var lab = null; var val_r = null; var val_w = null;
	    if(cBlk) {
		// View info
		if(!wr) {
		    val_r = document.createElement('span'); val_r.className = 'const';
		    val_r.StatusTip = selPath+'/'+brPath; val_r.onmouseover = function() { setStatus(this.StatusTip,10000); }
		}
		// View edit
		else {
		    val_w = document.createElement('span'); val_w.className = 'line number';
		    val_w.itPath = selPath+'/'+brPath;
		    val_w.srcNode = t_s;
		    val_w.itComm = comm;
		    val_w.innerHTML = "<input type='datetime-local' step='1'/>";
		    val_w.childNodes[0].onkeyup = val_w.childNodes[0].onchange = function(e) {
			if(this.parentNode.itComm) {
			    var val_w = this.parentNode;
			    if(val_w.childNodes[0].value) {
				vD = val_w.childNodes[0].value.split("T")[0].split("-");
				vT = val_w.childNodes[0].value.split("T")[1].split(":");
				var dt = new Date(parseInt(vD[0]), parseInt(vD[1])-1, parseInt(vD[2]), parseInt(vT[0]), parseInt(vT[1]), parseInt(vT[2]));
				val_w.srcNode.textContent = Math.floor(dt.getTime()/1000);
			    }
			    return true;
			}
			e = e ? e : window.event;
			if(this.parentNode.isEdited && e.keyCode == 13) { this.parentNode.childNodes[1].onclick(); return true; }
			if(this.parentNode.isEdited && e.keyCode == 27) {
			    var val_w = this.parentNode;
			    if(val_w.childNodes[0].defaultValue) val_w.childNodes[0].value = val_w.childNodes[0].defaultValue;
			    val_w.removeChild(this.parentNode.childNodes[1]);
			    val_w.isEdited = false;
			    return true;
			}
			if(this.parentNode.isEdited || this.value == this.defaultValue) return true;
			var btOk = document.createElement('img'); btOk.src = '/' + MOD_ID + '/img_button_ok';
			btOk.onclick = function( ) {
			    var val_w = this.parentNode;
			    vD = val_w.childNodes[0].value.split("T")[0].split("-");
			    vT = val_w.childNodes[0].value.split("T")[1].split(":");
			    var dt = new Date(parseInt(vD[0]), parseInt(vD[1])-1, parseInt(vD[2]), parseInt(vT[0]), parseInt(vT[1]), parseInt(vT[2]));
			    var rez = servSet(val_w.itPath, 'com=com', '<set>'+Math.floor(dt.getTime()/1000)+'</set>', true);
			    if(rez && parseInt(rez.getAttribute('rez')) != Er_NoError) alertCntr(rez);
			    setTimeout('pageRefresh()', 500);
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
		// Check for the label using
		if(t_s.getAttribute('dscr')) {
		    lab = document.createElement('span'); lab.className = 'label';
		    cBlk.dBlk = document.createElement('div'); cBlk.dBlk.className = 'elem'; cBlk.dBlk.appendChild(lab);
		    if(val_w) cBlk.dBlk.appendChild(val_w);
		    if(val_r) cBlk.dBlk.appendChild(val_r);
		    cBlk.appendChild(cBlk.dBlk);
		}
		else {
		    if(val_w) { if(cBlk.dBlk) cBlk.dBlk.appendChild(val_w); else { delete val_w; val_w = null; } }
		    if(val_r) { if(cBlk.dBlk) cBlk.dBlk.appendChild(val_r); else { delete val_r; val_r = null; } }
		}
		t_s.addr_lab = lab; t_s.addr_val_r = val_r; t_s.addr_val_w = val_w;
	    }
	    else { lab = t_s.addr_lab; val_r = t_s.addr_val_r; val_w = t_s.addr_val_w; }
	    // Fill data
	    if(lab) lab.innerText = t_s.getAttribute('dscr') + ':';
	    if(val_w && !val_w.isEdited) {
		val_w.title = (tVl=t_s.getAttribute('help')) ? tVl : "";
		var dt_time_t = parseInt(dataReq.textContent);
		var dt = new Date(dt_time_t?(dt_time_t*1000):0);
		val_w.childNodes[0].value = val_w.childNodes[0].defaultValue =
		    i2s(dt.getFullYear(),10,4)+"-"+i2s(dt.getMonth()+1,10,2)+"-"+i2s(dt.getDate(),10,2)+
		    "T"+i2s(dt.getHours(),10,2)+":"+i2s(dt.getMinutes(),10,2)+":"+i2s(dt.getSeconds(),10,2);
	    }
	    if(val_r) {
		val_r.title = (tVl=t_s.getAttribute('help')) ? tVl : "";
		var dt = new Date(parseInt(dataReq.textContent)*1000);
		val_r.innerText = i2s(dt.getDate(),10,2)+'.'+i2s(dt.getMonth()+1,10,2)+'.'+i2s(dt.getFullYear(),10,4)+
		    ' '+i2s(dt.getHours(),10,2)+':'+i2s(dt.getMinutes(),10,2)+':'+i2s(dt.getSeconds(),10,2);
	    }
	}
	//View other string and numberic fields
	else {
	    var lab = null; var val_r = null; var val_w = null;
	    if(cBlk) {
		// View info
		if(!wr) {
		    val_r = document.createElement('span'); val_r.className = 'const';
		    val_r.StatusTip = selPath+'/'+brPath; val_r.onmouseover = function() { setStatus(this.StatusTip,10000); }
		}
		// View edit
		else {
		    val_w = document.createElement('span'); val_w.className = 'line';
		    val_w.itPath = selPath+'/'+brPath;
		    val_w.srcNode = t_s;
		    val_w.itComm = comm;
		    val_w.appendChild(document.createElement('input')); val_w.childNodes[0].type = 'text';
		    val_w.StatusTip = selPath+'/'+brPath; val_w.onmouseover = function() { setStatus(this.StatusTip,10000); }

		    var tp = t_s.getAttribute('tp');
		    if(t_s.getAttribute('dest') == 'sel_ed') {
			val_w.childNodes[0].size = 50;
			var cmbImg = document.createElement('img'); cmbImg.src = '/'+MOD_ID+'/img_combar';
			cmbImg.onclick = function( ) {
			    if(!this.parentNode.sel_list || !this.parentNode.sel_list.length) return false;
			    var combMenu = getCombo();
			    var optHTML = '';
			    for(var iL = 0; iL < this.parentNode.sel_list.length; iL++)
				optHTML += '<option>' + this.parentNode.sel_list[iL] + '</option>';
			    var edFld = this.parentNode.childNodes[0];
			    combMenu.childNodes[0].edFld = edFld;
			    combMenu.childNodes[0].innerHTML = optHTML;
			    combMenu.childNodes[0].size = Math.max(3,this.parentNode.sel_list.length);
			    combMenu.style.cssText = 'visibility: visible; left: '+posGetX(edFld,true)+'px; '+
						     'top: '+(posGetY(edFld,true)+edFld.offsetHeight)+'px; '+
						     'width: '+edFld.offsetWidth+'px';
			    combMenu.childNodes[0].focus();
			    combMenu.childNodes[0].onclick = function( ) {
				this.parentNode.style.cssText = 'visibility: hidden; left: -200px; top: -200px;';
				if(this.selectedIndex < 0) return;
				this.edFld.value = this.options[this.selectedIndex].innerText;
				if(this.edFld.parentNode.itComm) this.edFld.parentNode.srcNode.textContent = this.edFld.value;
				else {
				    var rez = servSet(this.edFld.parentNode.itPath, 'com=com', '<set>'+this.edFld.value+'</set>', true);
				    if(rez && parseInt(rez.getAttribute('rez')) != Er_NoError) alertCntr(rez);
				    setTimeout('pageRefresh()', 500);
				}
				return false;
			    }
			    return false;
			}
			val_w.appendChild(cmbImg);
		    }
		    else if(tp == 'dec') {
			val_w.className += ' number';
			val_w.childNodes[0].size = 10;
			var spinImg = document.createElement('img'); spinImg.src = '/'+MOD_ID+'/img_spinar';
			spinImg.onclick = function(e) {
			    if(!e) e = window.event;
			    var val_w = this.parentNode.childNodes[0];
			    val_w.value = parseInt(val_w.value)+(((e.clientY-posGetY(this))<10)?1:-1);
			    val_w.onkeyup();
			    return false;
			}
			val_w.appendChild(spinImg);
		    }
		    else if(tp == 'hex' || tp == 'oct' || tp == 'real') { val_w.className += ' number'; val_w.childNodes[0].size = 10; }
		    else {
			val_w.childNodes[0].size = 30;
			val_w.childNodes[0].maxLength = t_s.getAttribute('len');
			if(!val_w.childNodes[0].maxLength) val_w.childNodes[0].maxLength = 1000;
		    }

		    val_w.childNodes[0].onkeyup = function(e) {
			if(this.parentNode.itComm) {
			    var curVal = this.value;
			    if(this.parentNode.srcNode.getAttribute('tp') == 'hex') curVal = parseInt(curVal, 16);
			    else if(this.parentNode.srcNode.getAttribute('tp') == 'oct') curVal = parseInt(curVal, 8);
			    this.parentNode.srcNode.textContent = curVal;
			    return true;
			}
			if(!e) e = window.event;
			if(this.parentNode.isEdited && e.keyCode == 13) { this.parentNode.lastChild.onclick(); return true; }
			if(this.parentNode.isEdited && e.keyCode == 27) {
			    this.value = this.defaultValue;
			    this.parentNode.isEdited = false;
			    this.parentNode.removeChild(this.parentNode.lastChild);
			    return true;
			}
			if(this.parentNode.isEdited || this.value == this.defaultValue) return true;
			var btOk = document.createElement('img'); btOk.src = '/' + MOD_ID + '/img_button_ok';
			btOk.onclick = function( ) {
			    var curVal = this.parentNode.childNodes[0].value;
			    if(this.parentNode.srcNode.getAttribute('tp') == 'hex') curVal = parseInt(curVal, 16);
			    else if(this.parentNode.srcNode.getAttribute('tp') == 'oct') curVal = parseInt(curVal, 8);
			    var rez = servSet(this.parentNode.itPath, 'com=com', '<set>'+curVal+'</set>', true);
			    if(rez && parseInt(rez.getAttribute('rez')) != Er_NoError) alertCntr(rez);
			    setTimeout('pageRefresh()', 500);
			    this.parentNode.isEdited = false;
			    this.parentNode.removeChild(this);
			    return false;
			}
			this.parentNode.appendChild(btOk);
			this.parentNode.isEdited = true;
			return true;
		    }
		}
		// Check use label
		if(t_s.getAttribute('dscr')) {
		    lab = document.createElement('span'); lab.className = 'label';
		    cBlk.dBlk = document.createElement('div'); cBlk.dBlk.className = 'elem'; cBlk.dBlk.appendChild(lab);
		    if(val_w) cBlk.dBlk.appendChild(val_w);
		    if(val_r) cBlk.dBlk.appendChild(val_r);
		    cBlk.appendChild(cBlk.dBlk);
		}
		else {
		    if(val_w) { if(cBlk.dBlk) cBlk.dBlk.appendChild(val_w); else { delete val_w; val_w = null; } }
		    if(val_r) { if(cBlk.dBlk) cBlk.dBlk.appendChild(val_r); else { delete val_r; val_r = null; } }
		}
		t_s.addr_lab = lab; t_s.addr_val_r = val_r; t_s.addr_val_w = val_w;
	    }
	    else { lab = t_s.addr_lab; val_r = t_s.addr_val_r; val_w = t_s.addr_val_w; }
	    // Fill line
	    var sval = dataReq.textContent;
	    if(t_s.getAttribute('tp') == 'hex') sval = '0x' + parseInt(sval).toString(16);
	    else if(t_s.getAttribute('tp') == 'oct') sval = '0' + parseInt(sval).toString(8);

	    if(lab) lab.innerText = t_s.getAttribute('dscr') + ':';
	    if(val_r) {
		val_r.title = (tVl=t_s.getAttribute('help')) ? tVl : "";
		val_r.innerText = sval;
	    }
	    if(val_w && !val_w.isEdited) {
		val_w.title = (tVl=t_s.getAttribute('help')) ? tVl : "";
		val_w.childNodes[0].value = val_w.childNodes[0].defaultValue = sval;
		// Load combo list
		if(t_s.getAttribute('dest') == 'sel_ed') {
		    if(t_s.getAttribute('select')) {
			val_w.sel_list = new Array();
			var xLst = servGet(t_s.getAttribute('select').replace(/%/g,'%25').replace(/\//g,'%2f'),'com=get');
			if(xLst)
			    for(var iEl = 0; iEl < xLst.childNodes.length; iEl++)
				if(xLst.childNodes[iEl].nodeName.toLowerCase() == 'el')
				    val_w.sel_list.push(xLst.childNodes[iEl].textContent);
		    } else if(t_s.getAttribute('sel_list')) val_w.sel_list = t_s.getAttribute('sel_list').split(';');
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
    if(!actEl) return;
    actEl.disabled = !vl;
    actEl.className = vl ? 'active' : 'inactive';
    actEl.childNodes[0].src = vl ? actEl.childNodes[0].src.replace('filtr=unact','filtr=none') :
				   actEl.childNodes[0].src.replace('filtr=none','filtr=unact');
}
/***************************************************
 * chkStruct - Info page tree check structure.     *
 ***************************************************/
function chkStruct( wNd, nNd )
{
    //Check access
    if(wNd.getAttribute('acs') != nNd.getAttribute('acs')) return true;

    //Scan deleted nodes
    for(var iW = 0; iW < wNd.childNodes.length; iW++) {
	var iN;
	for(iN = 0; iN < nNd.childNodes.length; iN++)
	    if(wNd.childNodes[iW].nodeName.toLowerCase() == nNd.childNodes[iN].nodeName.toLowerCase() &&
		    wNd.childNodes[iW].getAttribute('id') == nNd.childNodes[iN].getAttribute('id'))
		break;
	if(iN >= nNd.childNodes.length && wNd.nodeName.toLowerCase() != 'table' && wNd.nodeName.toLowerCase() != 'list')
	return true;
    }

    //Scan for new nodes and check present nodes
    for(var iN = 0; iN < nNd.childNodes.length; iN++) {
	var iW;
	for(iW = 0; iW < wNd.childNodes.length; iW++)
	    if(wNd.childNodes[iW].nodeName.toLowerCase() == nNd.childNodes[iN].nodeName.toLowerCase() &&
		    wNd.childNodes[iW].getAttribute('id') == nNd.childNodes[iN].getAttribute('id'))
		break;
	if(iW >= wNd.childNodes.length) return true;
	else if(chkStruct(wNd.childNodes[iW],nNd.childNodes[iN])) return true;

	//Check of the description present
	if((wNd.childNodes[iW].getAttribute('dscr') && !nNd.childNodes[iN].getAttribute('dscr')) ||
		(!wNd.childNodes[iW].getAttribute('dscr') && nNd.childNodes[iN].getAttribute('dscr')))
	    return true;

	//Check base fields destination change
	if(wNd.childNodes[iW].nodeName.toLowerCase() == 'fld' &&
		(wNd.childNodes[iW].getAttribute('dest') != nNd.childNodes[iN].getAttribute('dest') ||
		wNd.childNodes[iW].getAttribute('tp') != nNd.childNodes[iN].getAttribute('tp')))
	    return true;
    }

    return false;
}
/***************************************************
 * tabSelect - Select page tab                     *
 ***************************************************/
function tabSelect( tab )
{
    if(!tab || tab.className == 'active') return;
    for(var iT = 0; iT < tab.parentNode.childNodes.length; iT++)
	if(tab.parentNode.childNodes[iT].className == 'active')
	    tab.parentNode.childNodes[iT].className = '';
    tab.className = 'active';

    for(var iCf = 0; iCf < root.childNodes.length; iCf++)
	if(root.childNodes[iCf].nodeName.toLowerCase() == 'area' && tab.innerText == root.childNodes[iCf].getAttribute('dscr'))
	{ root.childNodes[iCf].setAttribute('qview','0'); break; }

    pageCyclRefrStop();
    pageDisplay(selPath);
}
/***************************************************
 * hostsUpdate - Init/update tree hosts.           *
 ***************************************************/
function hostsUpdate( )
{
    var treeRoot = document.getElementById('treeRoot');
    //Make hosts
    var hostN = servGet('/', 'com=chlds');
    if(hostN) {
	// Remove no present hosts
	for(var iTop = 0; iTop < treeRoot.childNodes.length; iTop++) {
	    var iH;
	    for(iH = 0; iH < hostN.childNodes.length; iH++)
		if(treeRoot.childNodes[iTop].getAttribute('id') == ('/'+hostN.childNodes[iH].getAttribute('id')))
		    break;
	    if(iH >= hostN.childNodes.length) { treeRoot.removeChild(treeRoot.childNodes[iTop]); iTop--; }
	}
	// Add/update hosts
	for(var i = 0; i < hostN.childNodes.length; i++) {
	    var liN = null;
	    for(var iTop = 0; iTop < treeRoot.childNodes.length; iTop++) {
		if(treeRoot.childNodes[iTop].getAttribute('id') == ('/'+hostN.childNodes[i].getAttribute('id')))
		{ liN = treeRoot.childNodes[iTop]; break; }
	    }
	    if(!liN) { liN = document.createElement('li'); treeRoot.appendChild(liN); }
	    liN.setAttribute('id', '/'+hostN.childNodes[i].getAttribute('id'));
	    liN.isExpand = false;
	    // Load groups
	    liN.grps = new Array();
	    for(var iGrp = 0; iGrp < hostN.childNodes[i].childNodes.length; iGrp++)
		if(hostN.childNodes[i].childNodes[iGrp].nodeName.toLowerCase() == 'grp')
		    liN.grps.push(hostN.childNodes[i].childNodes[iGrp]);
	    // Init links
	    var treeIco = '/'+MOD_ID+'/img_tree'+(liN.isExpand?'Minus':'Plus')+((i!=0)?'Up':'')+((i!=(hostN.childNodes.length-1))?'Down':'');
	    var liCont = "<a class='pm' onclick='expand(this.parentNode,!this.parentNode.isExpand); return false;'><img src='"+treeIco+"'/></a>";
	    if(parseInt(hostN.childNodes[i].getAttribute('icoSize')))
		liCont += "<span><img src='/"+MOD_ID+liN.getAttribute('id')+"?com=ico&size=16'/></span>";
		//liCont += "<span><img height='16px' src='/"+MOD_ID+liN.getAttribute('id')+"?com=ico'/></span>"; 
	    liCont += "<span><a onclick='selectPage(this.parentNode.parentNode.getAttribute(\"id\")); return false;' "+
		"onmouseover='setStatus(this.parentNode.parentNode.getAttribute(\"id\"),10000);' href='#'>"+hostN.childNodes[i].textContent+"</a></span>";
	    liN.innerHTML = liCont;
	}
	if(hostN.childNodes.length) return '/'+hostN.childNodes[0].getAttribute('id');
    }
}
/**************************************************
 * treeUpdate - Update tree expanded elements.    *
 **************************************************/
function treeUpdate( )
{
    var treeRoot = document.getElementById('treeRoot');
    for(var iT = 0; iT < treeRoot.childNodes.length; iT++)
	if(treeRoot.childNodes[iT].isExpand)
	    expand(treeRoot.childNodes[iT], true, true);
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
    if(!comboMenu) {
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
function pageRefresh( isComplete )
{
    pageDisplay(selPath, isComplete);
    if(pgRefrTmID) pgRefrTmID = setTimeout('pageRefresh();', 5000);
}
/********************************************************
 * pageCyclRefrStart - Start current page cyclic refrash.*
 ********************************************************/
function pageCyclRefrStart( )
{
    if(!pgRefrTmID) pgRefrTmID = setTimeout('pageRefresh();',5000);
    actEnable('actUpdate',false); actEnable('actStart',false); actEnable('actStop',true);
}
/********************************************************
 * pageCyclRefrStop - Stop current page cyclic refrash.  *
 ********************************************************/
function pageCyclRefrStop( )
{
    if(pgRefrTmID) { clearTimeout(pgRefrTmID); pgRefrTmID = null; }
    actEnable('actUpdate',true); actEnable('actStart',true); actEnable('actStop',false);
}
/**************************************************
 * itDBLoad - Load current page from DB.           *
 **************************************************/
function itDBLoad( )
{
    var rez = servSet(selPath+'/%2fobj', 'com=com', '<load/>', true);
    if(rez && parseInt(rez.getAttribute('rez')) != Er_NoError) alertCntr(rez);
    else setTimeout('pageRefresh()', 500);
}
/**************************************************
 * itDBSave - Save current page to DB.             *
 **************************************************/
function itDBSave( )
{
    var rez = servSet(selPath+'/%2fobj', 'com=com', '<save/>', true);
    if(rez && parseInt(rez.getAttribute('rez')) != Er_NoError) alertCntr(rez);
    else setTimeout('pageRefresh()', 500);
}
/**************************************************
 * pageUp - Select up page.                       *
 **************************************************/
function pageUp( )
{
    var iL = selPath.length-1;
    while(true) {
	var iL = selPath.lastIndexOf('/', iL);
	if(iL == -1 || iL == 0) return;
	if((selPath.length-iL) > 1) break;
	iL--;
    }
    selectPage(selPath.substr(0,iL));
}
/**************************************************
 * pagePrev - Get previous page.                  *
 **************************************************/
function pagePrev( )
{
    if(!ndPrev.length) return;
    ndNext.push(selPath);
    pageDisplay(ndPrev.pop());
}
/**************************************************
 * pageNext - Get next page.                      *
 **************************************************/
function pageNext( )
{
    if(!ndNext.length) return;
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
    for(var iCh = 0; iCh < root.childNodes.length; iCh++)
	if(root.childNodes[iCh].nodeName.toLowerCase() == 'branches' && root.childNodes[iCh].getAttribute('id') == 'br')
	    branchS =  root.childNodes[iCh];
    if(!branchS) return;

    //Load branches list
    var typeCfg = '';
    for(var iB = 0; iB < branchS.childNodes.length; iB++)
	if(parseInt(branchS.childNodes[iB].getAttribute('acs'))&SEC_WR)
	    typeCfg += "<option idSz='"+branchS.childNodes[iB].getAttribute('idSz')+
			"' gid='"+branchS.childNodes[iB].getAttribute('id')+
			"' idm='"+branchS.childNodes[iB].getAttribute('idm')+"'>"+
		branchS.childNodes[iB].getAttribute('dscr')+"</option>";
    if(!typeCfg.length) { alert('###There is no container for editing.###'); return; }

    dlgWin = ReqIdNameDlg(document.getElementById('actAddIt').childNodes[0].src,("###Adding an item to the node '%1'.###").replace('%1',selPath));
    dlgWin.document.getElementById('wDlgHeader').innerText = '###Adding a node###';
    dlgWin.document.getElementById('wDlgType').style.display = '';
    dlgWin.document.getElementById('wDlgType').childNodes[1].childNodes[0].innerHTML = typeCfg;
    dlgWin.document.getElementById('wDlgType').childNodes[1].childNodes[0].onchange = function( ) {
	if(this.selectedIndex < 0) return;
	var idSz = parseInt(this.options[this.selectedIndex].getAttribute('idSz'));
	dlgWin.document.getElementById('wDlgId').childNodes[1].childNodes[0].maxLength = (idSz > 0) ? idSz : 1000;
	var idm = parseInt(this.options[this.selectedIndex].getAttribute('idm'));
	dlgWin.document.getElementById('wDlgName').style.display = idm ? '' : 'none';
	dlgWin.document.getElementById('wDlgName').childNodes[1].childNodes[0].maxLength = (idm > 1) ? idm : 1000;
    }
    dlgWin.document.getElementById('wDlgType').childNodes[1].childNodes[0].onchange();
    dlgWin.document.getElementById('wDlgActOk').onclick = function() {
	var tpSel = dlgWin.document.getElementById('wDlgType').childNodes[1].childNodes[0];
	if(!tpSel || tpSel.selectedIndex < 0) { document.body.dlgWin.close(); return false; }
	var idm = parseInt(tpSel.options[tpSel.selectedIndex].getAttribute('idm'));
	var gbrId = tpSel.options[tpSel.selectedIndex].getAttribute('gid');
	var inpId = dlgWin.document.getElementById('wDlgId').childNodes[1].childNodes[0].value;
	var inpName = idm ? dlgWin.document.getElementById('wDlgName').childNodes[1].childNodes[0].value : inpId;

	// Check for already present node
	var req = servSet(selPath+'/%2fbr%2f'+gbrId,'com=com','<get/>',true);
	if(!req || parseInt(req.getAttribute('rez')) != Er_NoError) { alertCntr(req); document.body.dlgWin.close(); return false; }
	for(var iLel = 0; iLel < req.childNodes.length; iLel++)
	    if((req.childNodes[iLel].getAttribute('id') && req.childNodes[iLel].getAttribute('id') == inpId) ||
		(!req.childNodes[iLel].getAttribute('id') && req.childNodes[iLel].textContent == inpId))
	    {
		alert(("###Item '%1' already exists.###").replace('%1',inpId));
		document.body.dlgWin.close();
		return;
	    }

	// Send command
	var rez = servSet(selPath+'/%2fbr%2f'+gbrId,'com=com',"<add "+(idm?("id='"+inpId+"'"):"")+">"+inpName+"</add>",true);
	if(!rez || parseInt(rez.getAttribute('rez')) != Er_NoError) { alertCntr(rez); document.body.dlgWin.close(); return false; }
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
    if(!rmit || !rmit.length) return;

    if(!iit && !confirm(("###Are you sure you want to remove the node '%1'?###").replace('%1',rmit))) return;

    var tEl, sel_own = '', sel_el;
    var n_obj = 0;
    for(pathLev.off = 0; (tEl=pathLev(rmit,0,true)).length; n_obj++)
    { if(n_obj) sel_own += ('/'+sel_el); sel_el = tEl; }
    if(n_obj > 2) {
	var req = servGet(sel_own+'/%2fbr','com=info');
	if(parseInt(req.getAttribute('rez')) != Er_NoError) { alertCntr(req); return; }
	if(!req.childNodes[0]) return;

	var branch = req.childNodes[0];
	for(var iB = 0; iB < branch.childNodes.length; iB++) {
	    var bId = branch.childNodes[iB].getAttribute('id');
	    if(bId == sel_el.substr(0,bId.length) && parseInt(branch.childNodes[iB].getAttribute('acs'))&SEC_WR) {
		var idm = parseInt(branch.childNodes[iB].getAttribute('idm'));
		var com = idm ? ("<del id='"+sel_el.substr(bId.length)+"'/>") :
				("<del>"+sel_el.substr(bId.length)+"</del>");
		var rez = servSet(sel_own+'/%2fbr%2f'+bId,'com=com',com,true);
		if(rez && parseInt(rez.getAttribute('rez')) != Er_NoError) alertCntr(rez);
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
    var sEl, sElp = '', tEl, bGrp = '';
    var branchS = null;

    //Src elements calc
    var nSel = 0;
    for(pathLev.off = 0; (tEl=pathLev(copyBuf.substr(1),0,true)).length; nSel++)
    { if(nSel) sElp += ('/'+sEl); sEl = tEl; }

    //if(pathLev(copyBuf.substr(1),0) != pathLev(selPath,0)) { alert('###Copy is impossible.###'); return; }

    rootW = root;
    toPath = selPath;

    if(copyBuf.substr(1) == toPath) {	//For copy into the branch and no select direct the parent node
	toPath = sElp;
	pgInfo = servGet(toPath, 'com=info');
	if(parseInt(pgInfo.getAttribute('rez')) != 0) return;
	rootW = pgInfo.childNodes[0];
    }

    if(parseInt(rootW.getAttribute('acs'))&SEC_WR) { typeCfg += "<option idSz='-1' gid=''>###Selected###</option>"; itCnt++; }
    for(var iCh = 0; iCh < rootW.childNodes.length; iCh++)
	if(rootW.childNodes[iCh].nodeName.toLowerCase() == 'branches' && rootW.childNodes[iCh].getAttribute('id') == 'br')
	    branchS = rootW.childNodes[iCh];
    if(branchS)
	for(var iB = 0; iB < branchS.childNodes.length; iB++)
	    if(parseInt(branchS.childNodes[iB].getAttribute('acs'))&SEC_WR) {
		var gbrId = branchS.childNodes[iB].getAttribute('id');
		typeCfg += "<option idSz='" + branchS.childNodes[iB].getAttribute('idSz') + "' gid='" + gbrId + "'>" +
				branchS.childNodes[iB].getAttribute('dscr') + "</option>";
		if(sEl.substr(0,gbrId.length) == gbrId) { defIt = itCnt; bGrp = gbrId; }
		itCnt++;
	    }

    pathLev.off = 1; statNmSrc = pathLev(copyBuf, 0, true);
    pathLev.off = 0; statNm = pathLev(toPath, 0, true);

    //Make a request dialog
    dlgWin = ReqIdNameDlg('/'+MOD_ID+'/img_it_add');
    dlgWin.document.getElementById('wDlgHeader').innerText = '###Moving or copying the node###';
    wDlgTitle = dlgWin.document.getElementById('wDlgTitle').childNodes[1];
    wDlgTitle.innerText = ((copyBuf.charAt(0)=='1')?"###Move node '%1' to '%2'.###":"###Copy node '%1' to '%2'.###").replace('%1',copyBuf.substr(1)).replace('%2',toPath);
    if(bGrp.length) dlgWin.document.getElementById('wDlgId').childNodes[1].childNodes[0].value = sEl.substr(bGrp.length);
    dlgWin.document.getElementById('wDlgId').childNodes[1].childNodes[0].disabled = (statNm != statNmSrc);
    dlgWin.document.getElementById('wDlgName').style.display = 'none';
    wDlgType = dlgWin.document.getElementById('wDlgType');
    wDlgType.style.display = '';
    wDlgType.childNodes[1].childNodes[0].innerHTML = typeCfg;
    wDlgType.childNodes[1].childNodes[0].onchange = function( ) {
	if(this.selectedIndex < 0) return;
	var idSz = parseInt(this.options[this.selectedIndex].getAttribute('idSz'));
	dlgWin.document.getElementById('wDlgId').style.display = (idSz >= 0) ? '' : 'none';
	dlgWin.document.getElementById('wDlgId').childNodes[1].childNodes[0].maxLength = (idSz > 0) ? idSz : 1000;
    }
    wDlgType.childNodes[1].childNodes[0].selectedIndex = defIt;
    wDlgType.childNodes[1].childNodes[0].onchange();
    wDlgType.childNodes[1].childNodes[0].disabled = (statNm != statNmSrc);
    dlgWin.document.getElementById('wDlgActOk').onclick = function( ) {
	var tpSel = dlgWin.document.getElementById('wDlgType').childNodes[1].childNodes[0];
	if(!tpSel || tpSel.selectedIndex < 0) { document.body.dlgWin.close(); return false; }
	var idSz = parseInt(tpSel.options[tpSel.selectedIndex].getAttribute('idSz'));
	var gbrId = tpSel.options[tpSel.selectedIndex].getAttribute('gid');
	var inpId = dlgWin.document.getElementById('wDlgId').childNodes[1].childNodes[0].value;

	var statNm, statNmSr, srcNm, dstNm;
	pathLev.off = 1; statNmSrc = pathLev(copyBuf, 0, true); srcNm = copyBuf.substr(pathLev.off);
	pathLev.off = 0; statNm = pathLev(toPath, 0, true); dstNm = toPath.substr(pathLev.off);

	if(idSz >= 0) {
	    dstNm += '/'+gbrId+inpId;
	    // Check for already present node
	    var req = servSet(toPath+'/%2fbr%2f'+gbrId,'com=com','<get/>', true);
	    if(!req || parseInt(req.getAttribute('rez')) != Er_NoError) { alertCntr(req); document.body.dlgWin.close(); return false; }
	    for(var iLel = 0; iLel < req.childNodes.length; iLel++)
		if((req.childNodes[iLel].getAttribute('id') && req.childNodes[iLel].getAttribute('id') == inpId) ||
			(!req.childNodes[iLel].getAttribute('id') && req.childNodes[iLel].textContent == inpId)) {
		    if(confirm(("###Node '%1' already present. Continue?###").replace('%1',dstNm))) break;
		    document.body.dlgWin.close(); return;
		}
	}
	//Copy visual item
	var rez = servSet('/'+statNm+'/%2fobj','com=copy',"<copy src='"+srcNm+"' dst='"+dstNm+"' statNmSrc='"+statNmSrc+"' statNm='"+statNm+"'/>", true);
	if(!rez || parseInt(rez.getAttribute('rez')) != Er_NoError) { alertCntr(rez); document.body.dlgWin.close(); return false; }

	//Remove source widget
	if(copyBuf.charAt(0) == '1') {
	    itDel(copyBuf.substr(1));
	    if(selPath == copyBuf.substr(1)) selectPage("/"+statNm+dstNm);
	    else pageRefresh();
	    copyBuf = '0';
	} else pageRefresh();
	treeUpdate();
	document.body.dlgWin.close();
	return false;
    }
}
/**********************************************************
 * ReqIdNameDlg - Get identifier and name request dialog. *
 **********************************************************/
function ReqIdNameDlg( ico, mess, actPath, nmFile )
{
    var dlgWin = document.body.dlgWin;
    if(!dlgWin) {
	dlgWin = document.createElement('div');
	dlgWin.setAttribute("id", 'gen-pnl-dlg');
	document.body.dlgWin = dlgWin;
	dlgWin.close = function( ) { document.body.removeChild(this); this.isLoad = null; }
    }
    if(dlgWin.isLoad) document.body.removeChild(this);

    dlgWin.innerHTML =
	"<table cellspacing='0' "+
	    "style='left: "+((window.innerWidth-400-5)/2)+"px; top: "+((this.window.innerHeight-200-18)/2)+"px;'>\n"+
	"<tr style='cursor: move;' onmousedown='this.clX = event.clientX; this.clY = event.clientY; return false;' \n"+
	"   onmouseup='this.clX = this.clY = null; return false;' \n"+
	"   onmouseout='this.clX = this.clY = null; return false;' \n"+
	"   onmousemove='if(this.clX != null) { "+
	"     this.offsetParent.style.left = (parseInt(this.offsetParent.style.left)+event.clientX-this.clX)+\"px\"; "+
	"     this.offsetParent.style.top  = (parseInt(this.offsetParent.style.top)+event.clientY-this.clY)+\"px\"; "+
	"     this.clX = event.clientX; this.clY = event.clientY; }'>"+
	" <td id='wDlgHeader'>###Selecting node id and/or name###</td>\n"+
	" <td style='color: red; cursor: pointer; text-align: right; width: 1px; ' onclick='document.body.dlgWin.close();'>X</td>\n"+
	"</tr>\n"+
	"<tr><td colspan='2' style='max-width: 400px;'>\n"+
	" <center>\n"+
	"  <form id='wDlgFormBlk' action='"+actPath+"' method='post' enctype='multipart/form-data'>\n"+
	"   <table border='0' cellspacing='3px' class='dlg'>\n"+
	"    <tr><td id='wDlgTitle' colspan='2'><img src='' style='height: 32px; float: left; padding-right: 5px;'/><span style='word-wrap: break-word; word-break: break-all;'/></td></tr>\n"+
	"    <tr id='wDlgType'><td>###Element type:###</td><td><select name='type'/></td></tr>\n"+
	"    <tr id='wDlgId'><td>###ID:###</td><td><input name='id'/></td></tr>\n"+
	"    <tr id='wDlgName'><td>###Name:###</td><td><input name='name' type='"+(nmFile?'file':'text')+"'/></td></tr>\n"+
	"    <tr><td colspan='2' style='text-align: right; border-top: 1px solid black; padding-top: 10px;'>\n"+
	"      <input id='wDlgActOk' name='actOk' type='button' value='###Ok###'/> <input id='wDlgActCancel' name='actCancel' type='button' value='###Close###' onclick='document.body.dlgWin.close(); return false;'/>\n"+
	"    </td></tr>\n"+
	"   </table>\n"+
	"  </form>\n"+
	" </center>\n"+
	"</td></tr></table>"+
	"<div/>\n";
    document.body.appendChild(dlgWin);
    dlgWin.isLoad = true;

    if(ico) document.getElementById('wDlgTitle').childNodes[0].src = ico;
    if(mess) document.getElementById('wDlgTitle').childNodes[1].innerText = mess;

    return window;
}

//First start data init
if(genPnlLeft=document.getElementById('gen-pnl-left')) {
    genPnlLeft.ondblclick = function(e) { if(e.elPrc) return true; e.elPrc = true; hostsUpdate(); }
    genPnlLeft.onmousedown = function(){ return false; }
}

// Tool bar init
var actManual = document.getElementById('actManual');
//  Tree update action
var actTreeUpdt = document.getElementById('treeBlk');
if(actTreeUpdt) {
    actTreeUpdt.ondblclick = function(e) { if(e.elPrc) return true; e.elPrc = true; treeUpdate(); }
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
//  About
var actAbout = document.getElementById('actAbout');
if(actAbout && gPrms.length) actAbout.href += gPrms;

pageDisplay(hostsUpdate());

setStatus('###Page loaded.###', 5000);

/**********************************************************
 * Design specific part					  */
function getSpareWidth( node ) {
    compStl = getComputedStyle(node, null);
    return parseInt(compStl.getPropertyValue('border-left-width')) + parseInt(compStl.getPropertyValue('padding-left')) +
    parseInt(compStl.getPropertyValue('border-right-width')) + parseInt(compStl.getPropertyValue('padding-right'));
}

function getSpareHeight( node ) {
    compStl = getComputedStyle(node, null);
    return parseInt(compStl.getPropertyValue('border-top-width')) + parseInt(compStl.getPropertyValue('padding-top')) +
    parseInt(compStl.getPropertyValue('border-bottom-width')) + parseInt(compStl.getPropertyValue('padding-bottom'));
}

//Resize processing
function pageResize( horOnly ) {
    var w = Math.max(600, window.innerWidth);
    var h = Math.max(400, window.innerHeight);
    genPnlUp = document.getElementById('gen-pnl-up');
    genPnlLeft = document.getElementById('gen-pnl-left');
    genPnlRight = document.getElementById('gen-pnl-right');
    genPnlDown = document.getElementById('gen-pnl-down');
    genPnlSplt = document.getElementById('gen-pnl-splt');

    genPnlUp.style.width = (w-getSpareWidth(genPnlUp))+'px';
    genPnlDown.style.width = (w-getSpareWidth(genPnlDown))+'px';
    genPnlRight.style.width = (w-genPnlRight.offsetLeft-getSpareWidth(genPnlRight))+'px';
    if(!horOnly) {
	genPnlDown.style.top = (h-genPnlDown.clientHeight-1)+'px';
	genPnlLeft.style.height = (genPnlDown.offsetTop-genPnlLeft.offsetTop-getSpareHeight(genPnlLeft))+'px';
	genPnlRight.style.height = (genPnlDown.offsetTop-genPnlRight.offsetTop-getSpareHeight(genPnlRight))+'px';
	genPnlSplt.style.height = (genPnlDown.offsetTop-genPnlSplt.offsetTop-getSpareHeight(genPnlSplt))+'px';
    }

    setTimeout('pageRefresh(true)', 2000);
}
window.onresize = function( ) { pageResize(); }
pageResize();

//Split processing
document.getElementById('gen-pnl-splt').onmousedown = function(e) {
    e = e ? e : window.event;
    if(e.preventDefault) e.preventDefault();
    this.toMove = true;
}
window.onmouseup = function( ) { document.getElementById('gen-pnl-splt').toMove = false; }
window.onmousemove = function( e ) {
    e = e ? e : window.event;
    genPnlSplt = document.getElementById('gen-pnl-splt');
    if(genPnlSplt.toMove) {
	genPnlLeft = document.getElementById('gen-pnl-left');
	genPnlRight = document.getElementById('gen-pnl-right');

	genPnlSplt.style.left = e.clientX+'px';// (e.clientX-genPnlSplt.clientWidth/2)+'px';
	genPnlRight.style.left = e.clientX+'px';
	genPnlLeft.style.width = (e.clientX-getSpareWidth(genPnlLeft))+'px';
	pageResize(true);
	return false;
    }
}