
//OpenSCADA system module UI.WebCfgD file: VCA.js
/***************************************************************************
 *   Copyright (C) 2008 by Roman Savochenko                                *
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

//> Global parameters init
MOD_ID = 'WebCfgD';	//Module identifier
stTmID = null;		//Status line timer identifier
pgRefrTmID = null;	//Periodic curent page update timer identifier
dlgWin = null;		//Opened window dialog
selPath = '';		//Selected node path
queSZ = 20;		//Previous and next arrays queue size
ndPrev = new Array();	//Previous nodes array
ndNext = new Array();	//Next nodes array
pgInfo = document.createElement('info');	//Curent page XML tree.
root = document.createElement('oscada_cntr');	//Root page's node.
selPath = '';		//Select item path
SEQ_XT = 0x01;		//Extended
SEQ_WR = 0x02;		//Write access
SEQ_RD = 0x04;		//Read access
copyBuf = '0';		//Copy node address buffer
//> Browser type detect
var isNN = navigator.appName.indexOf('Netscape') != -1;
var isIE = navigator.appName.indexOf('Microsoft') != -1;
var isOpera = navigator.appName.indexOf('Opera') != -1;
var isKonq = navigator.appName.indexOf('Konqueror') != -1;
/***************************************************
 * pathLev - Path parsing function.                *
 ***************************************************/
pathLev.off = 0;
function pathLev( path, level, scan )
{
  var an_dir = scan ? pathLev.off : 0;
  var t_lev = 0;
  var t_dir;
  while( an_dir < path.length && path.charAt(an_dir) == '/' ) an_dir++;
  if( an_dir >= path.length ) return '';
  while( true )
  {
    t_dir = path.indexOf('/',an_dir);
    if( t_dir < 0 ) { pathLev.off=path.length; return (t_lev==level)?path.substr(an_dir):''; }
    if( t_lev == level ) { pathLev.off=t_dir; return path.substr(an_dir,t_dir-an_dir); }
    an_dir = t_dir;
    t_lev++;
    while( an_dir<path.length && path.charAt(an_dir)=='/' ) an_dir++;
  }
}
/***************************************************
 * nodeText - Get DOM node text                    *
 ***************************************************/
function nodeText( node )
{
  var rez = '';
  if( node )
    for( var i = 0; i < node.childNodes.length; i++ )
      if( node.childNodes[i].nodeType == 3 ) rez += node.childNodes[i].data;
  return rez;
}
/***************************************************
 * setNodeText - Set DOM node text                 *
 ***************************************************/
function setNodeText( node, val )
{
  if( !node ) return;
  for( var i = 0; i < node.childNodes.length; i++ )
    if( node.childNodes[i].nodeType == 3 )
    { node.childNodes[i].data = val; return; }
  node.appendChild(node.ownerDocument.createTextNode(val));
}
/*****************************************************
 * nodeTextByTagId - Get DOM node by tag name and id *
 *****************************************************/
function nodeTextByTagId( node, tag, avl )
{
  for( var i = 0; i < node.childNodes.length; i++ )
    if( node.childNodes[i].nodeName == tag && node.childNodes[i].getAttribute('id') == avl )
      return nodeText(node.childNodes[i]);
  return null;
}
/***************************************************
 * posGetX - Get absolute position                 *
 ***************************************************/
function posGetX(obj)
{
  var posX = 0;
  for( ; obj && obj.nodeName != 'BODY' ; obj=obj.parentNode )
    posX += (obj.style.left?parseInt(obj.style.left):0)+
	(obj.parentNode.style.borderLeftWidth?parseInt(obj.parentNode.style.borderLeftWidth):0)+
	(obj.parentNode.style.marginLeft?parseInt(obj.parentNode.style.marginLeft):0);
  return posX;
}
/***************************************************
 * posGetY - Get absolute position                 *
 ***************************************************/
function posGetY(obj)
{
  var posY = 0;
  for( ; obj && obj.nodeName != 'BODY' ; obj=obj.parentNode )
    posY += (obj.style.top?parseInt(obj.style.top):0)+
	(obj.parentNode.style.borderTopWidth?parseInt(obj.parentNode.style.borderTopWidth):0)+
	(obj.parentNode.style.marginTop?parseInt(obj.parentNode.style.marginTop):0);
  return posY;
}
/***************************************************
 * getXmlHttp - Check and return XMLHttpRequest for*
 *  various browsers.                              *
 ***************************************************/
function getXmlHttp( )
{
  if( window.XMLHttpRequest ) return new XMLHttpRequest();
  else if( window.ActiveXObject )
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
  rez = Math.floor(val*Math.pow(10,dig?dig:0)+0.5)/Math.pow(10,dig?dig:0);
  if( toInt ) return Math.floor(rez+0.5);
  return rez;
}
/***************************************************
 * servGet - XML get request to server             *
 ***************************************************/
function servGet( adr, prm )
{
  var req = getXmlHttp();
  req.open('GET',encodeURI('/'+MOD_ID+adr+'?'+prm),false);
  try
  { req.send(null);
    if( req.status == 200 && req.responseXML.childNodes.length )
      return req.responseXML.childNodes[0];
  } catch( e ) { window.location='/'; }
}
/***************************************************
 * servSet - XML set request to server             *
 ***************************************************/
function servSet( adr, prm, body, waitRez )
{
  var req = getXmlHttp();
  req.open('POST',encodeURI('/'+MOD_ID+adr+'?'+prm),!waitRez);
  try
  { req.send(body);
    if( waitRez && req.status == 200 && req.responseXML.childNodes.length )
      return req.responseXML.childNodes[0];
  } catch( e ) { window.location='/'; }
  return null;
}
/***************************************************
 * setStatus - Setup status message.               *
 ***************************************************/
function setStatus( mess, tm )
{
  document.getElementById('status').childNodes[0].nodeValue = (mess?mess:'Ready');
  if( !mess ) return;
  if( stTmID ) clearTimeout(stTmID);
  if( !tm || tm > 0 ) stTmID = setTimeout('setStatus(null)',tm?tm:1000);
}
/***************************************************
 * expand - Expand/roll select subtree.            *
 ***************************************************/
function expand( el, val, upTree )
{
  if( !val )
  {
    alert
    for( var i = 0; i < el.childNodes.length; i++ )
      if( el.childNodes[i].nodeName == 'UL' )
      { el.removeChild(el.childNodes[i]); break; }
//    var treeRoot = document.getElementById('treeRoot');
//    if( treeRoot.scrollHeight <= treeRoot.offsetHeight ) treeRoot.childNodes[0].scrollIntoView();
  }
  else
  {
    var newCUL = false;
    var cUL = null;
    for( var i = 0; i < el.childNodes.length; i++ )
      if( el.childNodes[i].nodeName == 'UL' )
      { cUL = el.childNodes[i]; break; }
    if( !cUL )
    {
      cUL = document.createElement('ul');
      if( el.parentNode.lastChild!=el ) cUL.style.backgroundImage = 'url(img_treeLine)';
      el.appendChild(cUL);
      newCUL = true;
    }
    if( el.grps.length > 1 )
    {
      //> Add and update present
      for( var i_g = 0; i_g < el.grps.length; i_g++ )
      {
	var liN = null;
	//> Search present item
	if( upTree )
	  for( var i_it = 0; i_it < cUL.childNodes.length; i_it++ )
	    if( cUL.childNodes[i_it].grps[0].getAttribute('id') == el.grps[i_g].getAttribute('id') )
	    { liN = cUL.childNodes[i_it]; break; }
	if( !liN )
	{
	  liN = document.createElement('li');
	  if( i_g >= cUL.childNodes.length ) cUL.appendChild(liN);
	  else cUL.insertBefore(liN,cUL.childNodes[i_g]);
	  liN.isExpand = false;
	}
	liN.setAttribute('id',el.getAttribute('id'));
	//> Set group
	liN.grps = new Array(); liN.grps.push(el.grps[i_g]);
	//> Init links
	var isUsable = parseInt(liN.grps[0].getAttribute('chPresent'));
	var treeIco = '/'+MOD_ID+'/img_tree'+(isUsable?(liN.isExpand?'Minus':'Plus'):'')+'Up'+((i_g!=(el.grps.length-1))?'Down':'');
	var liCont = isUsable?"<a class='pm' onclick='expand(this.parentNode,!this.parentNode.isExpand); return false;'>":"";
	liCont += "<img src='"+treeIco+"'/></a>";
	liCont += "<span style='font-style: italic;'>"+el.grps[i_g].getAttribute('dscr')+":</span>";
	//>> Next node for update
	if( upTree && liN.isExpand )
	{
	  var liNUL = null;
	  for( var i = 0; i < liN.childNodes.length; i++ ) if( liN.childNodes[i].nodeName == 'UL' ) { liNUL = liN.childNodes[i]; break; }
	  liN.innerHTML = liCont;
	  if( liNUL ) liN.appendChild(liNUL);
	  expand(liN,val,upTree);
	}
	else liN.innerHTML = liCont;
      }
      //> Delete no present
      if( upTree )
	for( var i_it = 0; i_it < cUL.childNodes.length; i_it++ )
	{
	  var i_g;
	  for( i_g = 0; i_g < el.grps.length; i_g++ )
	    if( cUL.childNodes[i_it].grps[0].getAttribute('id') == el.grps[i_g].getAttribute('id') )
	      break;
	  if( i_g >= el.grps.length ) { cUL.removeChild(cUL.childNodes[i_it]); i_it--; }
	}
    }
    else
    {
      var grpId = el.grps[0].getAttribute('id');
      var hostN = servGet(el.getAttribute('id'),'com=chlds&grp='+grpId);
      if( hostN && parseInt(hostN.getAttribute('rez'))==0 && hostN.childNodes.length )
      {
	//> Add and update present
	for( var i = 0; i < hostN.childNodes.length; i++ )
	{
	  var tmpNdId = el.getAttribute('id')+'/'+(grpId+(hostN.childNodes[i].getAttribute('id')?hostN.childNodes[i].getAttribute('id'):nodeText(hostN.childNodes[i])));
	  var liN = null;
	  //>> Find item
	  if( upTree )
	    for( var i_it = 0; i_it < cUL.childNodes.length; i_it++ )
	      if( cUL.childNodes[i_it].getAttribute('id') == tmpNdId )
	      { liN = cUL.childNodes[i_it]; break; }
	  if( !liN )
	  {
	    liN = document.createElement('li');
	    if( i >= cUL.childNodes.length ) cUL.appendChild(liN);
	    else cUL.insertBefore(liN,cUL.childNodes[i]);
	    liN.isExpand = false;
	  }
	  liN.setAttribute('id',tmpNdId);
	  //> Load groups
	  liN.grps = new Array();
	  for( var i_grp = 0; i_grp < hostN.childNodes[i].childNodes.length; i_grp++ )
	    if( hostN.childNodes[i].childNodes[i_grp].nodeName == 'grp' )
	      liN.grps.push(hostN.childNodes[i].childNodes[i_grp]);
	  //> Init links
	  var isUsable = liN.grps.length>1||(liN.grps.length&&parseInt(liN.grps[0].getAttribute('chPresent')));
	  var treeIco = '/'+MOD_ID+'/img_tree'+(isUsable?(liN.isExpand?'Minus':'Plus'):'')+'Up'+((i!=(hostN.childNodes.length-1))?'Down':'');
	  var liCont = isUsable?"<a class='pm' onclick='expand(this.parentNode,!this.parentNode.isExpand); return false;'>":"";
	  liCont += "<img src='"+treeIco+"'/></a>";
	  if( parseInt(hostN.childNodes[i].getAttribute('icoSize')) )
	    liCont += "<span><img height='16px' src='/"+MOD_ID+liN.getAttribute('id')+"?com=ico'/></span>";
	  liCont += "<span><a onclick='selectPage(this.parentNode.parentNode.getAttribute(\"id\")); return false;' "+
	    "onmouseover='setStatus(this.parentNode.parentNode.getAttribute(\"id\"),10000);' href='#'>"+nodeText(hostN.childNodes[i])+"</a></span>";
	  //>> Next node for update
	  if( upTree && liN.isExpand )
	  {
	    var liNUL = null;
	    for( var i = 0; i < liN.childNodes.length; i++ ) if( liN.childNodes[i].nodeName == 'UL' ) { liNUL = liN.childNodes[i]; break; }
	    liN.innerHTML = liCont;
	    if( liNUL ) liN.appendChild(liNUL);
	    expand(liN,val,upTree);
	  }
	  else liN.innerHTML = liCont;
	}
	//> Delete no present
	if( upTree )
	  for( var i_it = 0; i_it < cUL.childNodes.length; i_it++ )
	  {
	    var i_e;
	    for( var i_e = 0; i_e < hostN.childNodes.length; i_e++ )
	    {
	      var grpId = el.grps[0].getAttribute('id');
	      var tmpNdId = el.getAttribute('id')+'/'+(grpId+(hostN.childNodes[i_e].getAttribute('id')?hostN.childNodes[i_e].getAttribute('id'):nodeText(hostN.childNodes[i_e])));
	      if( cUL.childNodes[i_it].getAttribute('id') == tmpNdId ) break;
	    }
	    if( i_e >= hostN.childNodes.length ) { cUL.removeChild(cUL.childNodes[i_it]); i_it--; }
	  }
      }
    }
  }

  if( el.isExpand != val )
  {
    //> Change tree icon
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
  if( !path ) return;

  //> Prev and next
  if( selPath.length ) ndPrev.push(selPath);
  while( ndPrev.length >= queSZ ) ndPrev.shift();
  ndNext = new Array();

  //> Page display
  pageDisplay(path);
}
/***************************************************
 * pageDisplay - Display selected page.            *
 ***************************************************/
function pageDisplay( path )
{
  if( !path ) return;

  //> Chek Up
  actEnable('actUp',path.lastIndexOf('/') != -1 && path.lastIndexOf('/') != 0);

  //> Check Prev and Next
  actEnable('actPrevious',ndPrev.length);
  actEnable('actNext',ndNext.length);

  if( path != pgInfo.getAttribute('path') )
  {
    //> Stop refresh
    pageCyclRefrStop();

    //> Delete all tabs for new node
//    var tabs = document.getElementById('pgTabs');
//    while( tabs.childNodes.length ) tabs.removeChild(tabs.childNodes[0]);

    if( selPath.length && document.getElementById(selPath) ) document.getElementById(selPath).className = '';
    selPath = path;
    if( selPath.length && document.getElementById(selPath) ) document.getElementById(selPath).className = 'select';
    setNodeText(document.getElementById('selPath'),selPath);

    pgInfo = servGet(selPath,'com=info');
    if( parseInt(pgInfo.getAttribute('rez'))!=0 ) { alert(nodeText(pgInfo)); return; }
    pgInfo.setAttribute('path',selPath);
    root = pgInfo.childNodes[0];
  }
  else
  {
    //>> Check the new node structure and the old node
    var iTree = servGet(selPath,'com=info');
    if( parseInt(iTree.getAttribute('rez'))!=0 ) { alert(nodeText(iTree)); return; }
    upStruct(root,iTree.childNodes[0]);
  }

  selectChildRecArea(root,'/');

  //> The add and the delete access allow check
  actEnable('actAddIt',false);
  for( var i_ch = 0; i_ch < root.childNodes.length; i_ch++ )
    if( root.childNodes[i_ch].getAttribute('id') == 'br' )
    {
      for( var i_g = 0; i_g < root.childNodes[i_ch].childNodes.length; i_g++ )
        if( parseInt(root.childNodes[i_ch].childNodes[i_g].getAttribute('acs'))&SEQ_WR )
        { actEnable('actAddIt',true); break; }
      break;
    }
  actEnable('actDelIt',parseInt(root.getAttribute('acs'))&SEQ_WR)

  //> Load and Save allow check
  var reqModif = servGet(selPath,'com=modify');
  actEnable('actLoad',parseInt(nodeText(reqModif)));
  actEnable('actSave',parseInt(nodeText(reqModif)));

  //> Edit tools update
  editToolUpdate( );
}
/***************************************************
 * editToolUpdate - Edit action state update.      *
 ***************************************************/
function editToolUpdate( )
{
  actEnable('actCut',(selPath.length&&parseInt(root.getAttribute('acs'))&SEQ_WR));
  actEnable('actCopy',selPath.length);
  actEnable('actPaste',false);

  //> Src and destination elements calc
  if( copyBuf.length <= 1 || copyBuf.substr(1) == selPath || pathLev(copyBuf.substr(1),0) != pathLev(selPath,0) ) return;
  var s_elp; var s_el; var t_el;
  pathLev.off = 0;
  while( (t_el=pathLev(copyBuf.substr(1),0,true)).length )
  { s_elp += ('/'+s_el); s_el = t_el; }

  for( var i_ch = 0; i_ch < root.childNodes.length; i_ch++ )
    if( root.childNodes[i_ch].getAttribute('id') == 'br' )
    {
      for( var i_g = 0; i_g < root.childNodes[i_ch].childNodes.length; i_g++ )
      if( parseInt(root.childNodes[i_ch].childNodes[i_g].getAttribute('acs'))&SEQ_WR )
      { actEnable('actPaste',true); break; }
      break;
    }
}
/***************************************************
 * selectChildRecArea - Make page content          *
 ***************************************************/
function selectChildRecArea( node, aPath, cBlk )
{
  var i_area = 0;
  //> View title name
  if( aPath == "/" )
  {
    //>> Set node icon
    document.getElementById('pgIco').src = (nodeTextByTagId(node,'img','ico') != null) ? ('/'+MOD_ID+selPath+'?com=ico') : '';
    //>> Set title
    setNodeText(document.getElementById('pgTitle'),node.getAttribute('dscr'));
    //>> Delete tabs of deleted areas
    var activeTab = null;
    var tabs = document.getElementById('pgTabs');
    for( var i_tbs = 0; i_tbs < tabs.childNodes.length; i_tbs++ )
    {
      var i_cf;
      for( i_cf = 0; i_cf < node.childNodes.length; i_cf++ )
	if( node.childNodes[i_cf].nodeName == 'area' && nodeText(tabs.childNodes[i_tbs]) == node.childNodes[i_cf].getAttribute('dscr') )
	  break;
      if( i_cf >= node.childNodes.length )
      {
	if( tabs.childNodes[i_tbs].className == 'active' ) node.childNodes[i_tbs].setAttribute('qview','0');
	tabs.removeChild(tabs.childNodes[i_tbs]);
	i_tbs--;
      }
      else if( tabs.childNodes[i_tbs].className == 'active' ) activeTab = tabs.childNodes[i_tbs];
    }
    //>> Add new tabs
    for( var i_cf = 0; i_cf < node.childNodes.length; i_cf++ )
    {
      if( node.childNodes[i_cf].nodeName != 'area' ) continue;
      var i_tbs;
      for( i_tbs = 0; i_tbs < tabs.childNodes.length; i_tbs++ )
	if( nodeText(tabs.childNodes[i_tbs]) == node.childNodes[i_cf].getAttribute('dscr') )
	  break;
      if( i_tbs >= tabs.childNodes.length )
      {
	 var itab = document.createElement('span');
	 setNodeText(itab,node.childNodes[i_cf].getAttribute('dscr'));
	 itab.onclick = function( ) { tabSelect(this); return false; }
	 if( i_area >= tabs.childNodes.length ) tabs.appendChild(itab);
	 else tabs.insertBefore(itab,tabs.childNodes[i_area]);
	 node.childNodes[i_cf].setAttribute('qview','0');
      }
      i_area++;
    }
    if( !activeTab && tabs.childNodes.length ) { activeTab = tabs.childNodes[0]; activeTab.className = 'active'; }
    //>> Prepare active tab
    for( var i_cf = 0; i_cf < node.childNodes.length; i_cf++ )
      if( node.childNodes[i_cf].nodeName == 'area' && nodeText(activeTab) == node.childNodes[i_cf].getAttribute('dscr') )
      {
        var refresh = parseInt(node.childNodes[i_cf].getAttribute('qview'));
        var cPg = document.getElementById('pgCont');
	if( !refresh )
	{
//	  var v_scrl = (scrl->verticalScrollBar()) ? scrl->verticalScrollBar()->value() : 0;
	  while( cPg.childNodes.length ) cPg.removeChild(cPg.childNodes[0]);
	  selectChildRecArea(node.childNodes[i_cf],aPath+node.childNodes[i_cf].getAttribute('id')+'/',cPg);
//	  if( scrl->verticalScrollBar() ) scrl->verticalScrollBar()->setValue(v_scrl);
	  //>>> Mark last drawed tabs
	  node.childNodes[i_cf].setAttribute('qview','1');
	}
	else selectChildRecArea(node.childNodes[i_cf],aPath+node.childNodes[i_cf].getAttribute('id')+'/',null);
      }
    return;
  }
  else for( var i_cf = 0; i_cf < node.childNodes.length; i_cf++ )
  {
    var t_s = node.childNodes[i_cf];

    //>> Check access to node
    var wr = parseInt(t_s.getAttribute('acs'))&SEQ_WR;

    //>> View areas
    if( t_s.nodeName == 'area' )
    {
      var cntBlk = cBlk;
      if( cntBlk )
      {
        var cntBlk = document.createElement('fieldset');
        cntBlk.className = 'elem';
        cntBlk.appendChild(document.createElement('legend'));
        cntBlk.childNodes[0].appendChild(document.createTextNode(t_s.getAttribute('dscr')));
        cBlk.appendChild(cntBlk);
      }
      selectChildRecArea(t_s,aPath+t_s.getAttribute('id')+'/',cntBlk);
    }
    //>> View list elements
    else if( t_s.nodeName == 'list' )
    {
      var brPath = (aPath+t_s.getAttribute('id')).replace(/%/g,'%25').replace(/\//g,'%2f');
      var lab = null; var val = null;

      if( cBlk )
      {
	var dBlk = document.createElement('div'); dBlk.className = 'elem';
	lab = document.createElement('span'); lab.className = 'label';
	dBlk.appendChild(lab);
	dBlk.appendChild(document.createElement('br'));
	val = document.createElement('select');
	val.size = 10;
	val.srcNode = t_s;
	val.itPath = selPath+'/'+brPath;
	val.onmouseover = function() { setStatus(this.itPath,10000); }
	val.onclick = function(e)
	{
	  if( !e ) e = window.event;
	  var popUpMenu = getPopup();
	  while( popUpMenu.childNodes[0].childNodes.length ) popUpMenu.childNodes[0].removeChild(popUpMenu.childNodes[0].childNodes[0]);
	  var optEl = null;
	  if( this.srcNode.getAttribute('tp') == 'br' && this.selectedIndex >= 0 )
	  {
	    optEl = document.createElement('option'); setNodeText(optEl,'Go'); optEl.posId = 'go'; popUpMenu.childNodes[0].appendChild(optEl);
	    optEl = document.createElement('option'); setNodeText(optEl,'----------'); optEl.disabled = true; popUpMenu.childNodes[0].appendChild(optEl);
	  }
	  if( (parseInt(this.srcNode.getAttribute('acs'))&SEQ_WR) && this.srcNode.getAttribute('s_com') )
	  {
	    if( this.srcNode.getAttribute('s_com').search('add') != -1 )
	    { optEl = document.createElement('option'); setNodeText(optEl,'Add'); optEl.posId = 'add'; popUpMenu.childNodes[0].appendChild(optEl); }
	    if( this.srcNode.getAttribute('s_com').search('ins') != -1 && this.selectedIndex >= 0 )
	    { optEl = document.createElement('option'); setNodeText(optEl,'Insert'); optEl.posId = 'ins'; popUpMenu.childNodes[0].appendChild(optEl); }
	    if( this.srcNode.getAttribute('s_com').search('edit') != -1 && this.selectedIndex >= 0 )
	    { optEl = document.createElement('option'); setNodeText(optEl,'Edit'); optEl.posId = 'edit'; popUpMenu.childNodes[0].appendChild(optEl); }
	    if( this.srcNode.getAttribute('s_com').search('del') != -1 && this.selectedIndex >= 0 )
	    { optEl = document.createElement('option'); setNodeText(optEl,'Delete'); optEl.posId = 'del'; popUpMenu.childNodes[0].appendChild(optEl); }
	    if( this.srcNode.getAttribute('s_com').search('move') != -1 && this.selectedIndex >= 0 )
	    {
	      optEl = document.createElement('option'); setNodeText(optEl,'----------'); optEl.disabled = true; popUpMenu.childNodes[0].appendChild(optEl);
	      optEl = document.createElement('option'); setNodeText(optEl,'Up'); optEl.posId = 'up'; popUpMenu.childNodes[0].appendChild(optEl);
	      optEl = document.createElement('option'); setNodeText(optEl,'Down'); optEl.posId = 'down'; popUpMenu.childNodes[0].appendChild(optEl);
	    }
	  }
	  if( popUpMenu.childNodes[0].childNodes.length )
	  {
	    popUpMenu.srcNode = this.srcNode;
	    popUpMenu.itPath = this.itPath;
	    if( this.selectedIndex >= 0 )
	    {
	      popUpMenu.lsId = this.childNodes[this.selectedIndex].lsId;
	      popUpMenu.lsText = this.childNodes[this.selectedIndex].value;
	      if( !popUpMenu.lsId ) popUpMenu.lsId = popUpMenu.lsText;
	    }
	    popUpMenu.childNodes[0].size = Math.max(3,popUpMenu.childNodes[0].childNodes.length);
	    popUpMenu.style.cssText = 'visibility: visible; left: '+e.clientX+'px; top: '+e.clientY+'px;';
	    popUpMenu.childNodes[0].focus();
	    popUpMenu.childNodes[0].selectedIndex = -1;
	    popUpMenu.childNodes[0].onclick = function()
	    {
	      this.parentNode.style.cssText = 'visibility: hidden; left: -200px; top: -200px;';
	      if( this.selectedIndex < 0 || !this.childNodes[this.selectedIndex].posId ) return;
	      var idm = parseInt(this.parentNode.srcNode.getAttribute('idm'));
	      var posId = this.childNodes[this.selectedIndex].posId;
	      if( posId == 'go' )
	        selectPage(selPath+'/'+(this.parentNode.srcNode.getAttribute('br_pref')+this.parentNode.lsId).replace(/%/g,'%25').replace(/\//g,'%2f'));
	      else if( posId == 'add' || posId == 'ins' || posId == 'edit' )
	      {
		dlgWin = ReqIdNameDlg('/'+MOD_ID+'/ico');
		dlgWin.document.getElementById('id').childNodes[1].childNodes[0].maxLength = this.parentNode.srcNode.getAttribute('idSz');
		dlgWin.document.getElementById('type').style.display = 'none';
		dlgWin.document.getElementById('name').style.display = idm?'':'none';
		dlgWin.document.getElementById('actOk').itPath = this.parentNode.itPath;
		dlgWin.document.getElementById('actOk').srcNode = this.parentNode.srcNode;
		dlgWin.document.getElementById('actOk').selectedIndex = this.selectedIndex;
		dlgWin.document.getElementById('actOk').lsId = this.parentNode.lsId;
		dlgWin.document.getElementById('actOk').lsText = this.parentNode.lsText;
		if( posId == 'add' )
		{
		  setNodeText(dlgWin.document.getElementById('title').childNodes[1],'Add new element.');
		  dlgWin.document.getElementById('actOk').onclick = function()
		  {
		    var idm = dlgWin.document.getElementById('name').style.display!='none';
		    var inpId = dlgWin.document.getElementById('id').childNodes[1].childNodes[0].value;
		    var inpName = idm ? dlgWin.document.getElementById('name').childNodes[1].childNodes[0].value : inpId;
		    var rez = servSet(this.itPath,'com=com',"<add "+(idm?("id='"+inpId+"'"):"")+">"+inpName+"</add>",true);
		    if( rez && parseInt(rez.getAttribute('rez')) != 0 ) alert(nodeText(rez));
		    if( this.srcNode.getAttribute('tp') == 'br' ) treeUpdate();
		    pageRefresh();
		    dlgWin.close(); return false;
		  }
		}
		else if( posId == 'ins' )
		{
		  setNodeText(dlgWin.document.getElementById('title').childNodes[1],'Insert new element.');
		  dlgWin.document.getElementById('actOk').onclick = function()
		  {
		    var idm = dlgWin.document.getElementById('name').style.display!='none';
		    var inpId = dlgWin.document.getElementById('id').childNodes[1].childNodes[0].value;
		    var inpName = idm ? dlgWin.document.getElementById('name').childNodes[1].childNodes[0].value : inpId;
		    var com = "<ins "+(idm?("id='"+inpId+"' "):"")+" pos='"+this.selectedIndex+"' p_id='"+this.lsId+"'>"+inpName+"</ins>";
		    var rez = servSet(this.itPath,'com=com',com,true);
		    if( rez && parseInt(rez.getAttribute('rez')) != 0 ) alert(nodeText(rez));
		    if( this.srcNode.getAttribute('tp') == 'br' ) treeUpdate();
		    pageRefresh();
		    dlgWin.close(); return false;
		  }
		}
		else if( posId == 'edit' )
		{
		  setNodeText(dlgWin.document.getElementById('title').childNodes[1],'Rename element.');
		  dlgWin.document.getElementById('id').childNodes[1].childNodes[0].value = idm ? this.parentNode.lsId : this.parentNode.lsText;
		  if( idm ) dlgWin.document.getElementById('name').childNodes[1].childNodes[0].value = this.parentNode.lsText;
		  dlgWin.document.getElementById('actOk').onclick = function()
		  {
		    var idm = dlgWin.document.getElementById('name').style.display!='none';
		    var inpId = dlgWin.document.getElementById('id').childNodes[1].childNodes[0].value;
		    var inpName = idm ? dlgWin.document.getElementById('name').childNodes[1].childNodes[0].value : inpId;
		    var com = "<edit "+(idm?("id='"+inpId+"' "):"")+" pos='"+this.selectedIndex+"' p_id='"+this.lsId+"'>"+inpName+"</edit>";
		    var rez = servSet(this.itPath,'com=com',com,true);
		    if( rez && parseInt(rez.getAttribute('rez')) != 0 ) alert(nodeText(rez));
		    if( this.srcNode.getAttribute('tp') == 'br' ) treeUpdate();
		    pageRefresh();
		    dlgWin.close(); return false;
		  }
		}
	      }
	      else if( posId == 'up' || posId == 'down' )
	      {
	        var c_new = (posId == 'down') ? c_new = this.selectedIndex+1 : this.selectedIndex-1;
	        var rez = servSet(this.parentNode.itPath,'com=com',"<move pos='"+this.selectedIndex+"' to='"+c_new+"'/>",true);
	        if( rez && parseInt(rez.getAttribute('rez')) != 0 ) alert(nodeText(rez));
	        pageRefresh();
	      }
	      else if( posId == 'del' )
	      {
	        var com = idm ? ("<del pos='"+this.selectedIndex+"' id='"+this.parentNode.lsId+"'/>") :
	                        ("<del pos='"+this.selectedIndex+"'>"+this.parentNode.lsText+"</del>");
	        var rez = servSet(this.parentNode.itPath,'com=com',com,true);
	        if( rez && parseInt(rez.getAttribute('rez')) != 0 ) alert(nodeText(rez));
		if( this.parentNode.srcNode.getAttribute('tp') == 'br' ) treeUpdate();
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
      //>>> Fill list
      setNodeText(lab,t_s.getAttribute('dscr')+': ');
      val.title = t_s.getAttribute('help');

      var dataReq = servGet(selPath+'/'+brPath,'com=get');
      if( !dataReq || parseInt(dataReq.getAttribute('rez'))!=0 ) { alert(nodeText(dataReq)); continue; }

      while( val.childNodes.length ) val.removeChild(val.childNodes[0]);
      for( var i_el = 0; i_el < dataReq.childNodes.length; i_el++ )
	if( dataReq.childNodes[i_el].nodeName == 'el' )
	{
	  var opt = document.createElement('option');
	  opt.lsId = dataReq.childNodes[i_el].getAttribute('id');
	  setNodeText(opt,nodeText(dataReq.childNodes[i_el]));
	  val.appendChild(opt);
	}
      while( val.childNodes.length < 4 ) { var opt = document.createElement('option'); opt.disabled = true; val.appendChild(opt); }
      val.size = Math.min(10,Math.max(4,val.childNodes.length));
    }
    //>> View standart fields
    else if( t_s.nodeName == 'fld' ) basicFields(t_s,aPath,cBlk,wr);
  }
}
/***************************************************
 * basicFields - Prepare basic fields view         *
 ***************************************************/
function basicFields( t_s, aPath, cBlk, wr, comm )
{
  var brPath = (aPath+t_s.getAttribute('id')).replace(/%/g,'%25').replace(/\//g,'%2f');

  var dataReq = document.createElement('get');
  if( !comm )
  {
    dataReq = servGet(selPath+'/'+brPath,'com=get');
    if( !dataReq || parseInt(dataReq.getAttribute('rez'))!=0 ) { alert(nodeText(dataReq)); setNodeText(dataReq,''); }
  }

  //> View select fields
  if( t_s.getAttribute('dest') == 'select' )
  {
    //?
  }
  else
  {
    //> View boolean fields
    if( t_s.getAttribute('tp') == 'bool' )
    {
      var lab = null; var val_r = null; var val_w = null;
      if( cBlk )
      {
	//>> View info
	if( !wr )
	{
	  val_r = document.createElement('span'); val_r.className = 'const';
	  val_r.StatusTip = selPath+'/'+brPath;
	  val_r.onmouseover = function() { setStatus(this.StatusTip,10000); }
	}
	//>> View edit
	else
	{
	  val_w = document.createElement('span');
	  val_w.appendChild(document.createElement('input'));
	  val_w.childNodes[0].type = 'checkbox'; val_w.childNodes[0].className = 'line'; val_w.childNodes[0].itPath = selPath+'/'+brPath;
	  val_w.childNodes[0].onclick = function( )
	  {
	    var rez = servSet(this.itPath,'com=com','<set>'+(this.checked?'1':'0')+'</set>',true);
	    if( rez && parseInt(rez.getAttribute('rez')) != 0 ) alert(nodeText(rez));
	    setTimeout('pageRefresh()',500);
	    return false;
	  }
	  val_w.StatusTip = selPath+'/'+brPath;
	  val_w.onmouseover = function() { setStatus(this.StatusTip,10000); }
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
      if( lab ) setNodeText(lab,t_s.getAttribute('dscr')+': ');
      if( val_w )
      {
	val_w.title = t_s.getAttribute('help');
	val_w.childNodes[0].checked = parseInt(nodeText(dataReq));
      }
      if( val_r )
      {
	val_r.title = t_s.getAttribute('help');
	setNodeText(val_r,parseInt(nodeText(dataReq))?'On':'Off');
      }
    }
    //> View edit fields
    else if( t_s.getAttribute('tp') == 'str' && (t_s.getAttribute('rows') || t_s.getAttribute('cols')) )
    {
      //?
    }
    //> View Data-Time fields
    else if( t_s.getAttribute('tp') == 'time' )
    {
      //?
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
	  val_r.StatusTip = selPath+'/'+brPath;
	  val_r.onmouseover = function() { setStatus(this.StatusTip,10000); }
        }
	//>> View edit
	else
	{
	  val_w = document.createElement('span');
	  val_w.appendChild(document.createElement('input')); val_w.childNodes[0].type = 'text'; val_w.childNodes[0].className = 'line';
	  val_w.StatusTip = selPath+'/'+brPath;
	  val_w.onmouseover = function() { setStatus(this.StatusTip,10000); }
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
      //>> Fill line
      var sval = nodeText(dataReq);
      if( t_s.getAttribute('tp') == 'hex' ) sval = '0x'+parseInt(nodeText(dataReq)).toString(16);
      else if( t_s.getAttribute('tp') == 'oct' ) sval = '0'+parseInt(nodeText(dataReq)).toString(8);

      if( lab ) setNodeText(lab,t_s.getAttribute('dscr')+': ');
      if( val_w /*&& !val_w->isEdited()*/ )
      {
	val_w.title = t_s.getAttribute('help');
	val_w.childNodes[0].value = sval;
      }
      if( val_r )
      {
	val_r.title = t_s.getAttribute('help');
	setNodeText(val_r,sval);
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
  actEl.childNodes[0].src = vl ? actEl.childNodes[0].src.replace('filtr=gray','filtr=none') :
				 actEl.childNodes[0].src.replace('filtr=none','filtr=gray');
}
/***************************************************
 * upStruct - Info page tree update structure.     *
 ***************************************************/
function upStruct( w_nd, n_nd )
{
  var strCh = false;

  //> Check access
  if( w_nd.getAttribute('acs') != n_nd.getAttribute('acs') ) strCh = true;

  //> Scan deleted nodes
  for( var i_w = 0; i_w < w_nd.childNodes.length; i_w++ )
  {
    var i_n;
    for( i_n = 0; i_n < n_nd.childNodes.length; i_n++ )
      if( w_nd.childNodes[i_w].nodeName == n_nd.childNodes[i_n].nodeName && w_nd.childNodes[i_w].getAttribute('id') == n_nd.childNodes[i_n].getAttribute('id') )
	break;
    if( i_n >= n_nd.childNodes.length )
    {
      w_nd.removeChild(w_nd.childNodes[i_w]);
      i_w--;
      if( w_nd.nodeName != 'table' && w_nd.nodeName != 'list' ) strCh = true;
    }
  }

  //> Scan for new nodes and check present nodes
  for( var i_n = 0; i_n < n_nd.childNodes.length; i_n++ )
  {
    var i_w;
    for( i_w = 0; i_w < w_nd.childNodes.length; i_w++ )
      if( w_nd.childNodes[i_w].nodeName == n_nd.childNodes[i_n].nodeName &&
	    w_nd.childNodes[i_w].getAttribute('id') == n_nd.childNodes[i_n].getAttribute('id') )
	break;
    if( i_w >= w_nd.childNodes.length )
    {
      //>> Add node
      if( i_n >= w_nd.childNodes.length ) w_nd.appendChild(n_nd.childNodes[i_n].cloneNode(true));
      else w_nd.insertBefore(n_nd.childNodes[i_n].cloneNode(true),w_nd.childNodes[i_n]);
      strCh = true;
      i_w = i_n;
    }
    else
    {
      //>> Check present node
      if( upStruct(w_nd.childNodes[i_w],n_nd.childNodes[i_n]) ) strCh = true;
      if( strCh && w_nd.nodeName == 'oscada_cntr' )
      {
	w_nd.childNodes[i_w].setAttribute('qview','0');
	strCh = false;
	continue;
      }
    }

    //> Check of the description present
    if( (w_nd.childNodes[i_w].getAttribute('dscr').length && !n_nd.childNodes[i_n].getAttribute('dscr').length) ||
	(!w_nd.childNodes[i_w].getAttribute('dscr').length && n_nd.childNodes[i_n].getAttribute('dscr').length) )
      strCh = true;

    //> Check base fields destination change
    if( w_nd.childNodes[i_w].nodeName == 'fld' &&
	(w_nd.childNodes[i_w].getAttribute('dest') != n_nd.childNodes[i_n].getAttribute('dest') ||
	w_nd.childNodes[i_w].getAttribute('tp') != n_nd.childNodes[i_n].getAttribute('tp')) )
    {
      w_nd.childNodes[i_w].setAttribute('dest','');
      w_nd.childNodes[i_w].setAttribute('tp','');
      strCh = true;
    }

    //> Sync node parameters (text and atributes)
    if( nodeText(w_nd.childNodes[i_w]) != nodeText(n_nd.childNodes[i_n]) )
      setNodeText(w_nd.childNodes[i_w],nodeText(n_nd.childNodes[i_n]));
    for( var i_a = 0; i_a < n_nd.childNodes[i_n].attributes.length; i_a++ )
      w_nd.childNodes[i_w].setAttribute(n_nd.childNodes[i_n].attributes[i_a].name,n_nd.childNodes[i_n].attributes[i_a].value);
  }

  return strCh;
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
    if( root.childNodes[i_cf].nodeName == 'area' && nodeText(tab) == root.childNodes[i_cf].getAttribute('dscr') )
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
//          alert(treeRoot.childNodes[i_top]);
          if( treeRoot.childNodes[i_top].getAttribute('id') == ('/'+hostN.childNodes[i].getAttribute('id')) )
          { liN = treeRoot.childNodes[i_top]; break; }
        }
      if( !liN ) { liN = document.createElement('li'); treeRoot.appendChild(liN); liN.isExpand = false; }
      liN.setAttribute('id','/'+hostN.childNodes[i].getAttribute('id'));
      //> Load groups
      liN.grps = new Array();
      for( var i_grp = 0; i_grp < hostN.childNodes[i].childNodes.length; i_grp++ )
	if( hostN.childNodes[i].childNodes[i_grp].nodeName == 'grp' )
	  liN.grps.push(hostN.childNodes[i].childNodes[i_grp]);
      //> Init links
      var treeIco = '/'+MOD_ID+'/img_tree'+(liN.isExpand?'Minus':'Plus')+((i!=0)?'Up':'')+((i!=(hostN.childNodes.length-1))?'Down':'');
      var liCont = "<a class='pm' onclick='expand(this.parentNode,!this.parentNode.isExpand); return false;'><img src='"+treeIco+"'/></a>";
      if( parseInt(hostN.childNodes[i].getAttribute('icoSize')) )
	liCont += "<span><img height='16px' src='/"+MOD_ID+liN.getAttribute('id')+"?com=ico'/></span>";
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
    popUpMenu = document.createElement('div');
    popUpMenu.id = 'popupmenu';
    popUpMenu.appendChild(document.createElement('select'));
    popUpMenu.childNodes[0].onblur = function() { this.parentNode.style.visibility = 'hidden'; }
    document.body.appendChild(popUpMenu);
    popUpMenu.style.visibility = 'hidden';
  }
  return popUpMenu;
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
 * pageCyclRefrStart - Start curent page cyclic refrash.*
 ********************************************************/
function pageCyclRefrStart( )
{
  if( !pgRefrTmID ) pgRefrTmID = setTimeout('pageRefresh();',5000);
  actEnable('actUpdate',false); actEnable('actStart',false); actEnable('actStop',true);
}
/********************************************************
 * pageCyclRefrStop - Stop curent page cyclic refrash.  *
 ********************************************************/
function pageCyclRefrStop( )
{
  if( pgRefrTmID ) { clearTimeout(pgRefrTmID); pgRefrTmID = null; }
  actEnable('actUpdate',true); actEnable('actStart',true); actEnable('actStop',false);
}
/**************************************************
 * itDBLoad - Load curent page from DB.           *
 **************************************************/
function itDBLoad( )
{
  var rez = servSet(selPath+'/%2fobj','com=com','<load/>',true);
  if( rez && parseInt(rez.getAttribute('rez')) != 0 ) alert(nodeText(rez));
  else setTimeout('pageRefresh()',500);
}
/**************************************************
 * itDBSave - Save curent page to DB.             *
 **************************************************/
function itDBSave( )
{
  var rez = servSet(selPath+'/%2fobj','com=com','<save/>',true);
  if( rez && parseInt(rez.getAttribute('rez')) != 0 ) alert(nodeText(rez));
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
  if( !selPath.length ) return;
  var branchS = null;
  for( var i_ch = 0; i_ch < root.childNodes.length; i_ch++ )
    if( root.childNodes[i_ch].nodeName == 'branches' && root.childNodes[i_ch].getAttribute('id') == 'br' )
      branchS =  root.childNodes[i_ch];
  if( !branchS ) return;

  //> Load branches list
  var typeCfg;
  for( var i_b = 0; i_b < branchS.childNodes.length; i_b++ )
    if( parseInt(branchS.childNodes[i_b].getAttribute('acs'))&SEQ_WR )
      typeCfg+="<option idSz='"+branchS.childNodes[i_b].getAttribute('idSz')+
		     "' gid='"+branchS.childNodes[i_b].getAttribute('id')+
		     "' idm='"+(parseInt(branchS.childNodes[i_b].getAttribute('idm'))?"1":"0")+"'>"+
		branchS.childNodes[i_b].getAttribute('dscr')+"</option>";
  if( !typeCfg.length ) { alert('No one editable container present.'); return; }

  dlgWin = ReqIdNameDlg(document.getElementById('actAddIt').childNodes[0].src,'Add item to node:'+selPath);
  dlgWin.document.getElementById('type').style.display = '';
  dlgWin.document.getElementById('type').childNodes[1].childNodes[0].innerHTML = typeCfg;
  dlgWin.document.getElementById('type').childNodes[1].childNodes[0].onchange = function( )
  {
    if( this.selectedIndex < 0 ) return;
    dlgWin.document.getElementById('id').childNodes[1].childNodes[0].maxLength = this.childNodes[this.selectedIndex].getAttribute('idSz');
    dlgWin.document.getElementById('name').style.display = parseInt(this.childNodes[this.selectedIndex].getAttribute('idm'))?'':'none';
  }
  dlgWin.document.getElementById('type').childNodes[1].childNodes[0].onchange();
  dlgWin.document.getElementById('actOk').onclick = function()
  {
    var tpSel = dlgWin.document.getElementById('type').childNodes[1].childNodes[0];
    if( !tpSel || tpSel.selectedIndex < 0 ) return;
    var idm = parseInt(tpSel.childNodes[tpSel.selectedIndex].getAttribute('idm'));
    var inpId = dlgWin.document.getElementById('id').childNodes[1].childNodes[0].value;
    var inpName = idm ? dlgWin.document.getElementById('name').childNodes[1].childNodes[0].value : inpId;
    var rez = servSet(selPath+'/%2fbr%2f'+tpSel.childNodes[tpSel.selectedIndex].getAttribute('gid'),'com=com',
	"<add "+(idm?("id='"+inpId+"'"):"")+">"+inpName+"</add>",true);
    if( rez && parseInt(rez.getAttribute('rez')) != 0 ) alert(nodeText(rez));
    else { treeUpdate(); pageRefresh(); }
    dlgWin.close(); return false;
  }
}
/**************************************************
 * itDel - Delete new item action process.        *
 **************************************************/
function itDel( iit )
{
  var rmit = iit ? selPath : iit;
  if( !rmit || !rmit.length ) return;
}
/**********************************************************
 * ReqIdNameDlg - Get identifier and name request dialog. *
 **********************************************************/
function ReqIdNameDlg( ico, mess )
{
  var dlgWin = window.open('','ReqIdNameDlg','width=300,height=180,directories=no,menubar=no,toolbar=no,scrollbars=yes,dependent=yes,location=no,status=no,alwaysRaised=yes');

  dlgWin.document.open();
  dlgWin.document.write(
    "<html><head>\n"+
    " <style type='text/css'>\n"+
    "  table.dlg { width: 98%; border: 3px ridge #FF9253; padding: 5px; text-align: left; vertical-align: top; font-family: Verdana,Arial,Helvetica,sans-serif; font-size:12px; }\n"+
    " </style>\n"+
    "</head>\n"+
    "<body style='background-color: #E6E6E6;'>\n"+
    "<center>\n"+
    "<table border='0' cellspacing='3px' class='dlg'>\n"+
    "<tr><td id='title' colspan='2'><img src='' style='height: 32px; float: left;'/><span/></td></tr>\n"+
    "<tr id='type'><td>Element type:</td><td><select/></td></tr>\n"+
    "<tr id='id'><td>ID:</td><td><input type='text'/></td></tr>\n"+
    "<tr id='name'><td>Name:</td><td><input type='text'/></td></tr>\n"+
    "<tr><td colspan='2' style='text-align: right; border-top: 1px solid black; padding-top: 10px;'>\n"+
    "<input id='actOk' type='button' value='Ok'/> <input id='actCancel' type='button' value='Cancel' onclick='window.close(); return false;'/></td></tr>\n"+
    "</table>\n"+
    "</center></body></html>");
  dlgWin.document.close();

  if( ico ) dlgWin.document.getElementById('title').childNodes[0].src = ico;
  if( mess ) setNodeText(dlgWin.document.getElementById('title').childNodes[1],mess);

  var wWidth, wHeight;
  if( window.innerWidth ) { wWidth = dlgWin.outerWidth; wHeight = dlgWin.outerHeight; }
  else { wWidth = dlgWin.document.body.clientWidth; wHeight = dlgWin.document.body.clientHeight; }
  dlgWin.moveTo(Math.round((screen.availWidth-wWidth)/2),Math.round((screen.availHeight-wHeight)/2));

  return dlgWin;
}

//> First start data init
//>> Tool bar init
//>>> Update actions
var actUpdate = document.getElementById('actUpdate');
if( actUpdate ) actUpdate.onclick = function() { if( this.className=='active' ) pageRefresh(); return false; }
var actStart = document.getElementById('actStart');
if( actStart ) actStart.onclick = function() { if( this.className=='active' ) pageCyclRefrStart(); return false; }
var actStop = document.getElementById('actStop');
if( actStop ) actStop.onclick = function() { if( this.className=='active' ) pageCyclRefrStop(); return false; }
//>>> DB actions
var actLoad = document.getElementById('actLoad');
if( actLoad ) actLoad.onclick = function() { if( this.className=='active' ) itDBLoad(); return false; }
var actSave = document.getElementById('actSave');
if( actSave ) actSave.onclick = function() { if( this.className=='active' ) itDBSave(); return false; }
//>>> Navigate actions
var actUp = document.getElementById('actUp');
if( actUp ) actUp.onclick = function() { if( this.className=='active' ) pageUp(); return false; }
var actPrevious = document.getElementById('actPrevious');
if( actPrevious ) actPrevious.onclick = function() { if( this.className=='active' ) pagePrev(); return false; }
var actNext = document.getElementById('actNext');
if( actNext ) actNext.onclick = function() { if( this.className=='active' ) pageNext(); return false; }
//>>> Create/delete actions
var actAddIt = document.getElementById('actAddIt');
if( actAddIt ) actAddIt.onclick = function() { if( this.className=='active' ) itAdd(); return false; }
var actDelIt = document.getElementById('actDelIt');
if( actDelIt ) actDelIt.onclick = function() { if( this.className=='active' ) itDel(); return false; }

pageDisplay(hostsUpdate());

setStatus('Page loaded.',5000);
