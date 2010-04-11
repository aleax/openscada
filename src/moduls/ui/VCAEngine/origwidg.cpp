
//OpenSCADA system module UI.VCAEngine file: origwidg.cpp
/***************************************************************************
 *   Copyright (C) 2006-2009 by Roman Savochenko                           *
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

#include <tsys.h>

#include "origwidg.h"
#include "vcaengine.h"

using namespace VCA;

//*************************************************
//* PrWidget: Primitive widget template           *
//*************************************************
PrWidget::PrWidget( const string &iid ) : LWidget(iid)
{

}

void PrWidget::preDisable(int flag)
{
    if( flag )
	throw TError(mod->nodePath().c_str(),_("Deleting the base primitive-widget error."));

    Widget::preDisable(flag);
}

string PrWidget::ico( )
{
    if( LWidget::ico().size() )
	return LWidget::ico();
    if( TUIS::icoPresent("VCA.wdg_"+id()) )
	return TSYS::strEncode(TUIS::icoGet("VCA.wdg_"+id()),TSYS::base64);
    return "";
}

void PrWidget::setEnable( bool val )
{
    if( enable() == val ) return;

    LWidget::setEnable( val );

    //> Init active attributes
    if( val )
    {
	vector<string> ls;
	attrList(ls);
	for(int i_l = 0; i_l < ls.size(); i_l++)
	    if( attrAt(ls[i_l]).at().flgGlob()&Attr::Active )
	    {
		attrAt(ls[i_l]).at().setS(attrAt(ls[i_l]).at().getS(),true);
		attrList(ls);
	    }
    }
}

bool PrWidget::cntrCmdGeneric( XMLNode *opt )
{
    //> Get page info
    if( opt->name() == "info" )
    {
	Widget::cntrCmdGeneric(opt);
	ctrMkNode("oscada_cntr",opt,-1,"/",_("Base widget: ")+id());
	ctrMkNode("fld",opt,-1,"/wdg/st/parent",_("Parent"),R_R_R_,owner().c_str(),grp().c_str(),
		2,"tp","str","dest","");
	return true;
    }

    //> Process command to page
    string a_path = opt->attr("path");
    if( a_path == "/wdg/st/parent" && ctrChkNode(opt,"get",R_R_R_,owner().c_str(),grp().c_str(),SEC_RD) )
	opt->setText(parentNm());
    else return Widget::cntrCmdGeneric(opt);

    return true;
}

void PrWidget::cntrCmdProc( XMLNode *opt )
{
    if( cntrCmdServ(opt) ) return;

    //> Get page info
    if( opt->name() == "info" )
    {
	cntrCmdGeneric(opt);
	cntrCmdAttributes(opt );
	return;
    }

    //> Process command to page
    if( !(cntrCmdGeneric(opt) || cntrCmdAttributes(opt)) )
	TCntrNode::cntrCmdProc(opt);
}


//*************************************************
//* Original widgets based at primitive           *
//* widget template                               *

//*************************************************
//* OrigElFigure:                                 *
//*  Elementary figures original widget           *
//*************************************************
OrigElFigure::OrigElFigure( ) : PrWidget("ElFigure")
{

}

string OrigElFigure::name( )
{
    return _("Elementary figures");
}

string OrigElFigure::descr( )
{
    return _("Elementary figures widget of the finite visualisation.");
}

void OrigElFigure::postEnable( int flag )
{
    LWidget::postEnable(flag);

    if( flag&TCntrNode::NodeConnect )
    {
	attrAdd( new TFld("lineWdth",_("Line:width"),TFld::Integer,TFld::NoFlag,"","1","0;99","","20") );
	attrAdd( new TFld("lineClr",_("Line:color"),TFld::String,Attr::Color,"","#000000","","","21") );
	attrAdd( new TFld("lineStyle",_("Line:style"),TFld::Integer,TFld::Selected,"","0","0;1;2",_("Solid;Dashed;Dotted"),"22") );
	attrAdd( new TFld("bordWdth",_("Border:width"),TFld::Integer,TFld::NoFlag,"","0","0;99","","23") );
	attrAdd( new TFld("bordClr",_("Border:color"),TFld::String,Attr::Color,"","#000000","","","24") );
	attrAdd( new TFld("fillColor",_("Fill:color"),TFld::String,Attr::Color,"","","","","25") );
	attrAdd( new TFld("fillImg",_("Fill:image"),TFld::String,Attr::Image,"","","","","26") );
	attrAdd( new TFld("orient",_("Orientation angle"),TFld::Integer,TFld::NoFlag,"","0","-360;360","","28") );
	attrAdd( new TFld("elLst",_("Element's list"),TFld::String,TFld::FullText|Attr::Active,"","","","","27") );
    }
}

bool OrigElFigure::attrChange( Attr &cfg, TVariant prev )
{
    if( cfg.flgGlob()&Attr::Active && cfg.id() == "elLst" )
    {
	string sel, sel1;
	string ls_prev = prev.getS();
	map<int,char> pntls, pntls_prev, wls, wls_prev, clrls, clrls_prev, imgls, imgls_prev, lstls, lstls_prev;
	//> Parse last attributes list and make point list
	for( int i_p = 0; i_p < 2; i_p++ )
	{
	    string ls_w = (i_p==0)?ls_prev:cfg.getS();
	    map<int,char> &pntls_w = (i_p==0)?pntls_prev:pntls;
	    map<int,char> &wls_w = (i_p==0)?wls_prev:wls;
	    map<int,char> &clrls_w = (i_p==0)?clrls_prev:clrls;
	    map<int,char> &imgls_w = (i_p==0)?imgls_prev:imgls;
	    map<int,char> &lstls_w = (i_p==0)?lstls_prev:lstls;
	    for( int off = 0; (sel=TSYS::strSepParse(ls_w,0,'\n',&off)).size(); )
	    {
		int offe = 0;
		string fTp = TSYS::strSepParse(sel,0,':',&offe);
		if( fTp == "line" || fTp == "arc" || fTp == "bezier" )
		{
		    int np = (fTp=="arc") ? 5 : ((fTp=="bezier") ? 4 : 2);
		    for( int i = 0; i < np; i++ ) pntls_w[atoi(TSYS::strSepParse(sel,0,':',&offe).c_str())] = true;
		    if( (sel1=TSYS::strSepParse(sel,0,':',&offe)).size() && sel1[0] == 'w' ) wls_w[atoi(sel1.substr(1).c_str())] = true;
		    if( (sel1=TSYS::strSepParse(sel,0,':',&offe)).size() && sel1[0] == 'c' ) clrls_w[atoi(sel1.substr(1).c_str())] = true;
		    if( (sel1=TSYS::strSepParse(sel,0,':',&offe)).size() && sel1[0] == 'w' ) wls_w[atoi(sel1.substr(1).c_str())] = true;
		    if( (sel1=TSYS::strSepParse(sel,0,':',&offe)).size() && sel1[0] == 'c' ) clrls_w[atoi(sel1.substr(1).c_str())] = true;
		    if( (sel1=TSYS::strSepParse(sel,0,':',&offe)).size() && sel1[0] == 's' ) lstls_w[atoi(sel1.substr(1).c_str())] = true;
		}
		else if( fTp == "fill" )
		    for( int zPnt = 0; !(sel1=TSYS::strSepParse(sel,0,':',&offe)).empty() || zPnt < 2; )
		    {
			if( sel1.empty() )	zPnt++;
			if( sel1[0] == 'c' )	clrls_w[atoi(sel1.substr(1).c_str())] = true;
			else if( sel1[0] == 'i' )	imgls_w[atoi(sel1.substr(1).c_str())] = true;
		    }
	    }
	}

	//> Add new dynamic items
	//>> Add no present dynamic points
	for( map<int,char>::iterator it = pntls.begin(); it != pntls.end(); ++it )
	    if( it->first && pntls_prev.find(it->first) == pntls_prev.end() && !cfg.owner()->attrPresent("p"+TSYS::int2str(it->first)+"x") )
	    {
		cfg.owner()->attrAdd( new TFld(("p"+TSYS::int2str(it->first)+"x").c_str(),(_("Point ")+TSYS::int2str(it->first)+":x").c_str(),
		    TFld::Real,Attr::Mutable,"","0","","",TSYS::int2str(30+it->first*6).c_str()) );
		cfg.owner()->attrAdd( new TFld(("p"+TSYS::int2str(it->first)+"y").c_str(),(_("Point ")+TSYS::int2str(it->first)+":y").c_str(),
		    TFld::Real,Attr::Mutable,"","0","","",TSYS::int2str(30+it->first*6+1).c_str()) );
	    }
	//>> Add no present dynamic widths
	for( map<int,char>::iterator it = wls.begin(); it != wls.end(); ++it )
	    if( it->first && wls_prev.find(it->first) == wls_prev.end() && !cfg.owner()->attrPresent("w"+TSYS::int2str(it->first)) )
		cfg.owner()->attrAdd( new TFld(("w"+TSYS::int2str(it->first)).c_str(),(_("Width ")+TSYS::int2str(it->first)).c_str(),
		    TFld::Real,Attr::Mutable,"","1","","",TSYS::int2str(30+it->first*6+2).c_str()) );
	//>> Add no present dynamic colors
	for( map<int,char>::iterator it = clrls.begin(); it != clrls.end(); ++it )
	    if( it->first && clrls_prev.find(it->first) == clrls_prev.end() && !cfg.owner()->attrPresent("c"+TSYS::int2str(it->first)) )
		cfg.owner()->attrAdd( new TFld(("c"+TSYS::int2str(it->first)).c_str(),(_("Color ")+TSYS::int2str(it->first)).c_str(),
		    TFld::String,Attr::Mutable|Attr::Color,"","","","",TSYS::int2str(30+it->first*6+3).c_str()) );
	//>> Add no present dynamic images
	for( map<int,char>::iterator it = imgls.begin(); it != imgls.end(); ++it )
	    if( it->first && imgls_prev.find(it->first) == imgls_prev.end() && !cfg.owner()->attrPresent("i"+TSYS::int2str(it->first)) )
		cfg.owner()->attrAdd( new TFld(("i"+TSYS::int2str(it->first)).c_str(),(_("Image ")+TSYS::int2str(it->first)).c_str(),
		    TFld::String,Attr::Mutable|Attr::Image,"","","","",TSYS::int2str(30+it->first*6+4).c_str()) );
	//>> Add no present line styles
	for( map<int,char>::iterator it = lstls.begin(); it != lstls.end(); ++it )
	    if( it->first && lstls_prev.find(it->first) == lstls_prev.end() && !cfg.owner()->attrPresent("s"+TSYS::int2str(it->first)) )
		cfg.owner()->attrAdd( new TFld(("s"+TSYS::int2str(it->first)).c_str(),(_("Style ")+TSYS::int2str(it->first)).c_str(),
		    TFld::Integer,Attr::Mutable|TFld::Selected,"","0","0;1;2",_("Solid;Dashed;Dotted"),TSYS::int2str(30+it->first*6+5).c_str()) );

	//> Delete no dynamic items
	//>> Delete dynamic points
	for( map<int,char>::iterator it = pntls_prev.begin(); it != pntls_prev.end(); ++it )
	    if( it->first && pntls.find(it->first) == pntls.end() )
	    {
		cfg.owner()->attrDel("p"+TSYS::int2str(it->first)+"x");
		cfg.owner()->attrDel("p"+TSYS::int2str(it->first)+"y");
	    }
	//>> Delete dynamic widths
	for( map<int,char>::iterator it = wls_prev.begin(); it != wls_prev.end(); ++it )
	    if( it->first && wls.find(it->first) == wls.end() )
		cfg.owner()->attrDel("w"+TSYS::int2str(it->first));
	//>> Delete dynamic colors
	for( map<int,char>::iterator it = clrls_prev.begin(); it != clrls_prev.end(); ++it )
	    if( it->first && clrls.find(it->first) == clrls.end() )
		cfg.owner()->attrDel("c"+TSYS::int2str(it->first));
	//>> Delete dynamic images
	for( map<int,char>::iterator it = imgls_prev.begin(); it != imgls_prev.end(); ++it )
	    if( it->first && imgls.find(it->first) == imgls.end() )
		cfg.owner()->attrDel("i"+TSYS::int2str(it->first));
	//>> Delete dynamic line styles
	for( map<int,char>::iterator it = lstls_prev.begin(); it != lstls_prev.end(); ++it )
	    if( it->first && lstls.find(it->first) == lstls.end() )
		cfg.owner()->attrDel("s"+TSYS::int2str(it->first));
    }
    return Widget::attrChange(cfg,prev);
}

//*************************************************
//* OrigFormEl: Form element original widget      *
//*************************************************
OrigFormEl::OrigFormEl( ) : PrWidget("FormEl")
{

}

string OrigFormEl::name( )
{
    return _("Form's elements");
}

string OrigFormEl::descr( )
{
    return _("Form's elements widget of the finite visualisation.");
}

void OrigFormEl::postEnable( int flag )
{
    LWidget::postEnable(flag);

    if( flag&TCntrNode::NodeConnect )
	attrAdd( new TFld("elType",_("Element type"),TFld::Integer,TFld::Selected|Attr::Active,"2","0","0;1;2;3;4;5;6;7",
				    _("Line edit;Text edit;Chek box;Button;Combo box;List;Slider;Scroll Bar"),"20") );
}

bool OrigFormEl::attrChange( Attr &cfg, TVariant prev )
{
    if( cfg.flgGlob()&Attr::Active && cfg.id() == "elType" )
    {
	//> Delete specific attributes
	if( cfg.getI() != prev.getI() )
	    switch( prev.getI() )
	    {
		case 0:	//Line edit
		    cfg.owner()->attrDel("value");
		    cfg.owner()->attrDel("view");
		    cfg.owner()->attrDel("cfg");
		    cfg.owner()->attrDel("font");
		    break;
		case 1:	//Text edit
		    cfg.owner()->attrDel("value");
		    cfg.owner()->attrDel("wordWrap");
		    cfg.owner()->attrDel("font");
		    break;
		case 2:	//Check box
		    cfg.owner()->attrDel("value");
		    cfg.owner()->attrDel("font");
		    cfg.owner()->attrAt("name").at().fld().setReserve("");
		    break;
		case 3:	//Button
		    cfg.owner()->attrDel("value");
		    cfg.owner()->attrDel("img");
		    cfg.owner()->attrDel("color");
		    cfg.owner()->attrDel("colorText");
		    cfg.owner()->attrDel("checkable");
		    cfg.owner()->attrDel("font");
		    cfg.owner()->attrAt("name").at().fld().setReserve("");
		    break;
		case 4: case 5:	//Combo box and list
		    cfg.owner()->attrDel("value");
		    cfg.owner()->attrDel("items");
		    cfg.owner()->attrDel("font");
		    break;
		case 6: case 7:	//Slider and scroll bar
		    cfg.owner()->attrDel("value");
		    cfg.owner()->attrDel("cfg");
		    break;
	    }

	//> Create specific attributes
	switch(cfg.getI())
	{
	    case 0:	//Line edit
		cfg.owner()->attrAdd( new TFld("value",_("Value"),TFld::String,Attr::Mutable,"200","","","","21") );
		cfg.owner()->attrAdd( new TFld("view",_("View"),TFld::Integer,TFld::Selected|Attr::Mutable|Attr::Active,
		    "1","0","0;1;2;3;4;5;6",_("Text;Combo;Integer;Real;Time;Date;Date and time"),"22") );
		cfg.owner()->attrAdd( new TFld("cfg",_("Config"),TFld::String,TFld::FullText|Attr::Mutable,"","","","","23") );
		cfg.owner()->attrAdd( new TFld("font",_("Font"),TFld::String,Attr::Font,"50","Arial 11","","","25") );
		break;
	    case 1:	//Text edit
		cfg.owner()->attrAdd( new TFld("value",_("Value"),TFld::String,TFld::FullText|Attr::Mutable,"","","","","21") );
		cfg.owner()->attrAdd( new TFld("wordWrap",_("Word wrap"),TFld::Boolean,Attr::Mutable,"1","1","","","22") );
		cfg.owner()->attrAdd( new TFld("font",_("Font"),TFld::String,Attr::Font,"50","Arial 11","","","25") );
		break;
	    case 2:	//Check box
		cfg.owner()->attrAdd( new TFld("value",_("Value"),TFld::Boolean,Attr::Mutable,"","","","","21") );
		cfg.owner()->attrAdd( new TFld("font",_("Font"),TFld::String,Attr::Font,"50","Arial 11","","","25") );
		cfg.owner()->attrAt("name").at().fld().setReserve("26");
		break;
	    case 3:	//Button
		cfg.owner()->attrAdd( new TFld("value",_("Value"),TFld::Boolean,Attr::Mutable,"","","","","21") );
		cfg.owner()->attrAdd( new TFld("img",_("Image"),TFld::String,Attr::Image|Attr::Mutable,"","","","","22") );
		cfg.owner()->attrAdd( new TFld("color",_("Color:button"),TFld::String,Attr::Color|Attr::Mutable,"20","","","","23") );
		cfg.owner()->attrAdd( new TFld("colorText",_("Color:text"),TFld::String,Attr::Color|Attr::Mutable,"20","","","","27") );
		cfg.owner()->attrAdd( new TFld("checkable",_("Checkable"),TFld::Boolean,Attr::Mutable,"","","","","24") );
		cfg.owner()->attrAdd( new TFld("font",_("Font"),TFld::String,Attr::Font,"50","Arial 11","","","25") );
		cfg.owner()->attrAt("name").at().fld().setReserve("26");
		break;
	    case 4: case 5:	//Combo box and list
		cfg.owner()->attrAdd( new TFld("value",_("Value"),TFld::String,Attr::Mutable,"200","","","","21") );
		cfg.owner()->attrAdd( new TFld("items",_("Items"),TFld::String,TFld::FullText|Attr::Mutable,"","","","","22") );
		cfg.owner()->attrAdd( new TFld("font",_("Font"),TFld::String,Attr::Font,"50","Arial 11","","","25") );
		break;
	    case 6: case 7:	//Slider and scroll bar
		cfg.owner()->attrAdd( new TFld("value",_("Value"),TFld::Integer,Attr::Mutable,"20","0","","","21") );
		cfg.owner()->attrAdd( new TFld("cfg",_("Config"),TFld::String,Attr::Mutable,"100","0:0:100:1:10","","","22") );
		break;
	}
    }
    else if( cfg.flgGlob()&Attr::Active && cfg.id() == "view" )
    {
	TFld::Type	ntp = TFld::String;
	int		flg = Attr::Mutable;
	VCA::Attr::SelfAttrFlgs	sflg = cfg.owner()->attrAt("value").at().flgSelf();
	string		val = cfg.owner()->attrAt("value").at().getS();
	string		cfgTmpl = cfg.owner()->attrAt("value").at().cfgTempl( );
	string		cfgVal = cfg.owner()->attrAt("value").at().cfgVal( );
	switch(cfg.getI())
	{
	    case 2: case 4:	ntp = TFld::Integer;	break;
	    case 3:		ntp = TFld::Real;	break;
	    case 5: case 6:	ntp = TFld::Integer; flg|=Attr::DateTime;	break;
	}
	int apos = cfg.owner()->attrPos("value");
	cfg.owner()->attrDel("value");
	cfg.owner()->attrAdd( new TFld("value",_("Value"),ntp,flg,"200","","","","21"), apos );
	cfg.owner()->attrAt("value").at().setFlgSelf(sflg);
	cfg.owner()->attrAt("value").at().setS(val);
	cfg.owner()->attrAt("value").at().setCfgTempl(cfgTmpl);
	cfg.owner()->attrAt("value").at().setCfgVal(cfgVal);
    }

    return Widget::attrChange(cfg,prev);
}

//************************************************
//* OrigText: Text element original widget       *
//************************************************
OrigText::OrigText( ) : PrWidget("Text")
{

}

string OrigText::name( )
{
    return _("Text fields");
}

string OrigText::descr( )
{
    return _("Text fields widget of the finite visualisation.");
}

void OrigText::postEnable( int flag )
{
    LWidget::postEnable(flag);

    if( flag&TCntrNode::NodeConnect )
    {
	attrAdd( new TFld("backColor",_("Background:color"),TFld::String,Attr::Color,"","","","","20") );
	attrAdd( new TFld("backImg",_("Background:image"),TFld::String,Attr::Image,"","","","","21") );
	attrAdd( new TFld("bordWidth",_("Border:width"),TFld::Integer,TFld::NoFlag,"","0","","","22") );
	attrAdd( new TFld("bordColor",_("Border:color"),TFld::String,Attr::Color,"","#000000","","","23") );
	attrAdd( new TFld("bordStyle",_("Border:style"),TFld::Integer,TFld::Selected,"","3","0;1;2;3;4;5;6;7;8",
					_("None;Dotted;Dashed;Solid;Double;Groove;Ridge;Inset;Outset"),"24") );
	attrAdd( new TFld("font",_("Font"),TFld::String,Attr::Font,"50","Arial 11","","","25") );
	attrAdd( new TFld("color",_("Color"),TFld::String,Attr::Color,"20","#000000","","","26") );
	attrAdd( new TFld("orient",_("Orientation angle"),TFld::Integer,TFld::NoFlag,"3","0","-360;360","","27") );
	attrAdd( new TFld("wordWrap",_("Word wrap"),TFld::Boolean,TFld::NoFlag,"1","1","","","28") );
	attrAdd( new TFld("alignment",_("Alignment"),TFld::Integer,TFld::Selected,"1","0","0;1;2;3;4;5;6;7;8;9;10;11",
			    _("Top left;Top right;Top center;Top justify;"
			    "Bottom left;Bottom right;Bottom center;Bottom justify;"
			    "V center left; V center right; Center; V center justify"),"29") );
	attrAdd( new TFld("text",_("Text"),TFld::String,TFld::FullText,"0","Text","","","30") );
	attrAdd( new TFld("numbArg",_("Arguments number"),TFld::Integer,Attr::Active,"","0","0;20","","40") );
    }
}

bool OrigText::attrChange( Attr &cfg, TVariant prev )
{
    if( cfg.flgGlob()&Attr::Active )
    {
	int aid = atoi(cfg.fld().reserve().c_str());
	if( cfg.id() == "numbArg" )
	{
	    string fid("arg"), fnm(_("Argument ")), fidp, fnmp;
	    //> Delete specific unnecessary attributes of parameters
	    for( int i_p = 0; true; i_p++ )
	    {
		fidp = fid+TSYS::int2str(i_p);
		if( !cfg.owner()->attrPresent( fidp+"val" ) )	break;
		else if( i_p >= cfg.getI() )
		{
		    cfg.owner()->attrDel(fidp+"val");
		    cfg.owner()->attrDel(fidp+"tp");
		    cfg.owner()->attrDel(fidp+"cfg");
		}
	    }
	    //> Create ullage attributes of parameters
	    for( int i_p = 0; i_p < cfg.getI(); i_p++ )
	    {
		fidp = fid+TSYS::int2str(i_p);
		fnmp = fnm+TSYS::int2str(i_p);
		if( cfg.owner()->attrPresent( fidp+"val" ) ) continue;
		cfg.owner()->attrAdd( new TFld((fidp+"tp").c_str(),(fnmp+_(":type")).c_str(),
		    TFld::Real,TFld::Selected|Attr::Mutable|Attr::Active,"","0","0;1;2",_("Integer;Real;String"),TSYS::int2str(51+10*i_p).c_str()) );
		cfg.owner()->attrAdd( new TFld((fidp+"val").c_str(),(fnmp+_(":value")).c_str(),
		    TFld::Integer,Attr::Mutable,"","","","",TSYS::int2str(50+10*i_p).c_str()) );
		cfg.owner()->attrAdd( new TFld((fidp+"cfg").c_str(),(fnmp+_(":config")).c_str(),
		    TFld::String,Attr::Mutable,"","","","",TSYS::int2str(52+10*i_p).c_str()) );
	    }
	}
	else if( aid >= 50 && (aid%10) == 1 && prev.getI() != cfg.getI() )
	{
	    int narg = (aid/10)-5;
	    string fid = "arg"+TSYS::int2str(narg)+"val";
	    string fnm = _("Argument ")+TSYS::int2str(narg)+_(":value");
	    int apos = cfg.owner()->attrPos(fid);
	    VCA::Attr::SelfAttrFlgs sflg =  cfg.owner()->attrAt(fid).at().flgSelf();
	    cfg.owner()->attrDel(fid);
	    cfg.owner()->attrAdd( new TFld(fid.c_str(),fnm.c_str(),
			(cfg.getI()==1) ? TFld::Real : ((cfg.getI()==2) ? TFld::String : TFld::Integer),
			Attr::Mutable,"","","","",TSYS::int2str(50+10*narg).c_str()), apos );
	    cfg.owner()->attrAt(fid).at().setFlgSelf(sflg);
	}
    }
    return Widget::attrChange(cfg,prev);
}

//************************************************
//* OrigMedia: Media view original widget        *
//************************************************
OrigMedia::OrigMedia( ) : PrWidget("Media")
{

}

string OrigMedia::name( )
{
    return _("Media");
}

string OrigMedia::descr( )
{
    return _("Media widget of the finite visualisation.");
}

void OrigMedia::postEnable( int flag )
{
    LWidget::postEnable(flag);

    if( flag&TCntrNode::NodeConnect )
    {
	attrAdd( new TFld("backColor",_("Background:color"),TFld::String,Attr::Color,"","#FFFFFF","","","20") );
	attrAdd( new TFld("backImg",_("Background:image"),TFld::String,Attr::Image,"","","","","21") );
	attrAdd( new TFld("bordWidth",_("Border:width"),TFld::Integer,TFld::NoFlag,"","0","","","22") );
	attrAdd( new TFld("bordColor",_("Border:color"),TFld::String,Attr::Color,"","#000000","","","23") );
	attrAdd( new TFld("bordStyle",_("Border:style"),TFld::Integer,TFld::Selected,"","3","0;1;2;3;4;5;6;7;8",
						_("None;Dotted;Dashed;Solid;Double;Groove;Ridge;Inset;Outset"),"24") );
	attrAdd( new TFld("src",_("Source"),TFld::String,TFld::NoFlag,"50","","","","25") );
	attrAdd( new TFld("fit",_("Fit to widget size"),TFld::Boolean,TFld::NoFlag,"","","","","26") );
	attrAdd( new TFld("type",_("Type"),TFld::Integer,TFld::Selected|Attr::Active,"1","0","0;1",_("Image;Movie"),"27") );
	attrAdd( new TFld("areas",_("Map areas"),TFld::Integer,Attr::Active,"2","0","0;10","","28") );
    }
}

bool OrigMedia::attrChange( Attr &cfg, TVariant prev )
{
    if( cfg.flgGlob()&Attr::Active )
    {
	if( cfg.id() == "type" )
	{
	    //- Delete specific attributes -
	    if( prev.getI() == 1 ) cfg.owner()->attrDel("speed");

	    //- Create specific attributes -
	    if(cfg.getI() == 1 )
		cfg.owner()->attrAdd( new TFld("speed",_("Play speed"),TFld::Integer,Attr::Mutable,"3","100","1;900","","29") );
	}
	else if( cfg.id() == "areas" )
	{
	    string fid("area"), fnm(_("Area ")), fidp, fnmp;
	    //- Delete specific unnecessary attributes of map areas -
	    for( int i_p = 0; true; i_p++ )
	    {
		fidp = fid+TSYS::int2str(i_p); 
		if( !cfg.owner()->attrPresent( fidp+"shp" ) )	break;
		else if( i_p >= cfg.getI() )
		{
		    cfg.owner()->attrDel(fidp+"shp");
		    cfg.owner()->attrDel(fidp+"coord");
		    cfg.owner()->attrDel(fidp+"title");
		}
	    }
	    //- Create ullage attributes of map areas -
	    for( int i_p = 0; i_p < cfg.getI(); i_p++ )
	    {
		fidp = fid+TSYS::int2str(i_p);
		fnmp = fnm+TSYS::int2str(i_p);
		if( cfg.owner()->attrPresent( fidp+"shp" ) ) continue;
		cfg.owner()->attrAdd( new TFld((fidp+"shp").c_str(),(fnmp+_(":shape")).c_str(),
					       TFld::Integer,TFld::Selected|Attr::Mutable,"1","0","0;1;2","Rect;Poly;Circle",TSYS::int2str(40+3*i_p).c_str()) );
		cfg.owner()->attrAdd( new TFld((fidp+"coord").c_str(),(fnmp+_(":coordinates")).c_str(),
					       TFld::String,Attr::Mutable,"","","","",TSYS::int2str(41+3*i_p).c_str()) );
		cfg.owner()->attrAdd( new TFld((fidp+"title").c_str(),(fnmp+_(":title")).c_str(),
					       TFld::String,Attr::Mutable,"","","","",TSYS::int2str(42+3*i_p).c_str()) );
	    }
	}
    }
    return Widget::attrChange(cfg,prev);
}

//************************************************
//* OrigDiagram: Diagram original widget         *
//************************************************
OrigDiagram::OrigDiagram( ) : PrWidget("Diagram")
{

}

string OrigDiagram::name( )
{
    return _("Diagram");
}

string OrigDiagram::descr( )
{
    return _("Diagram widget of the finite visualisation.");
}

void OrigDiagram::postEnable( int flag )
{
    LWidget::postEnable(flag);

    if( flag&TCntrNode::NodeConnect )
    {
	attrAdd( new TFld("backColor",_("Background:color"),TFld::String,Attr::Color,"","black","","","20") );
	attrAdd( new TFld("backImg",_("Background:image"),TFld::String,Attr::Image,"","","","","21") );
	attrAdd( new TFld("bordWidth",_("Border:width"),TFld::Integer,TFld::NoFlag,"","0","","","22") );
	attrAdd( new TFld("bordColor",_("Border:color"),TFld::String,Attr::Color,"","#000000","","","23") );
	attrAdd( new TFld("bordStyle",_("Border:style"),TFld::Integer,TFld::Selected,"","3","0;1;2;3;4;5;6;7;8",
						_("None;Dotted;Dashed;Solid;Double;Groove;Ridge;Inset;Outset"),"24") );
	attrAdd( new TFld("trcPer",_("Tracing period (s)"),TFld::Integer,TFld::NoFlag,"","0","0;360","","25") );
	attrAdd( new TFld("type",_("Type"),TFld::Integer,TFld::Selected|Attr::Active,"1","0","0;1",_("Trend;Spectrum"),"26") );
    }
}

bool OrigDiagram::attrChange( Attr &cfg, TVariant prev )
{
    if( !(cfg.flgGlob()&Attr::Active) )	return Widget::attrChange(cfg,prev);

    if( cfg.id() == "active" && cfg.getB() != prev.getB() )
    {
	if( !cfg.getB() )
	{
	    cfg.owner()->attrDel("curSek");
	    cfg.owner()->attrDel("curUSek");
	    cfg.owner()->attrDel("curColor");
	}
	else
	{
	    cfg.owner()->attrAdd( new TFld("curSek",_("Cursor:sek"),TFld::Integer,Attr::DateTime|Attr::Mutable,"","","","","30") );
	    cfg.owner()->attrAdd( new TFld("curUSek",_("Cursor:usek"),TFld::Integer,Attr::Mutable,"","","","","31") );
	    cfg.owner()->attrAdd( new TFld("curColor",_("Cursor:color"),TFld::String,Attr::Color|Attr::Mutable,"","white","","","32") );
	}
    }
    else if( cfg.id() == "type" )
    {
	//> Delete specific attributes
	switch( prev.getI() )
	{
	    case 0: case 1:
		if( cfg.getI() == 0 || cfg.getI() == 1 ) break;
		cfg.owner()->attrDel("tSek");
		cfg.owner()->attrDel("tUSek");
		cfg.owner()->attrDel("tSize");
		cfg.owner()->attrDel("curSek");
		cfg.owner()->attrDel("curUSek");
		cfg.owner()->attrDel("curColor");
		cfg.owner()->attrDel("sclColor");
		cfg.owner()->attrDel("sclHor");
		cfg.owner()->attrDel("sclVer");
		cfg.owner()->attrDel("sclMarkColor");
		cfg.owner()->attrDel("sclMarkFont");
		cfg.owner()->attrDel("valArch");
		cfg.owner()->attrDel("parNum");
		break;
	}

	//> Create specific attributes
	switch( cfg.getI() )
	{
	    case 0: case 1:
		cfg.owner()->attrAdd( new TFld("tSek",_("Time:sek"),TFld::Integer,Attr::DateTime|Attr::Mutable,"","","","","27") );
		cfg.owner()->attrAdd( new TFld("tUSek",_("Time:usek"),TFld::Integer,Attr::Mutable,"","","","","28") );
		cfg.owner()->attrAdd( new TFld("tSize",_("Size, sek"),TFld::Real,Attr::Mutable,"","60","0;3e6","","29") );
		if( cfg.owner()->attrAt("active").at().getB() )
		{
		    cfg.owner()->attrAdd( new TFld("curSek",_("Cursor:sek"),TFld::Integer,Attr::DateTime|Attr::Mutable,"","","","","30") );
		    cfg.owner()->attrAdd( new TFld("curUSek",_("Cursor:usek"),TFld::Integer,Attr::Mutable,"","","","","31") );
		    cfg.owner()->attrAdd( new TFld("curColor",_("Cursor:color"),TFld::String,Attr::Color|Attr::Mutable,"","white","","","32") );
		}
		cfg.owner()->attrAdd( new TFld("sclColor",_("Scale:color"),TFld::String,Attr::Color|Attr::Mutable,"","grey","","","33") );
		cfg.owner()->attrAdd( new TFld("sclHor",_("Scale:horizontal"),TFld::Integer,Attr::Mutable|TFld::Selected,
		    "1","0","0;1;2;3",_("No draw;Grid;Markers;Grid and markers"),"34") );
		cfg.owner()->attrAdd( new TFld("sclVer",_("Scale:vertical"),TFld::Integer,Attr::Mutable|TFld::Selected,
		    "1","0","0;1;2;3;5;6;7",_("No draw;Grid;Markers;Grid and markers;Grid (log);Marker (log);Grid and markers (log)"),"35") );
		cfg.owner()->attrAdd( new TFld("sclVerScl",_("Scale:vertical scale (%)"),TFld::Real,Attr::Mutable,"","100","10;1000","","40") );
		cfg.owner()->attrAdd( new TFld("sclVerSclOff",_("Scale:vertical scale offset (%)"),TFld::Real,Attr::Mutable,"","0","-100;100","","41") );
		cfg.owner()->attrAdd( new TFld("sclMarkColor",_("Scale:Markers:color"),TFld::String,Attr::Color|Attr::Mutable,"","white","","","36") );
		cfg.owner()->attrAdd( new TFld("sclMarkFont",_("Scale:Markers:font"),TFld::String,Attr::Font|Attr::Mutable,"","Arial 10","","","37") );
		cfg.owner()->attrAdd( new TFld("valArch",_("Value archivator"),TFld::String,Attr::Mutable,"","","","","38") );
		cfg.owner()->attrAdd( new TFld("parNum",_("Parameters number"),TFld::Integer,Attr::Mutable|Attr::Active,"","1","0;10","","39") );
		break;
	}
    }
    else if( cfg.id() == "parNum" )
    {
	string fid("prm"), fnm(_("Parametr ")), fidp, fnmp;
	//> Delete specific unnecessary attributes of parameters
	for( int i_p = 0; true; i_p++ )
	{
	    fidp = fid+TSYS::int2str(i_p);
	    if( !cfg.owner()->attrPresent( fidp+"addr" ) )	break;
	    else if( i_p >= cfg.getI() )
	    {
		cfg.owner()->attrDel(fidp+"addr");
		cfg.owner()->attrDel(fidp+"bordL");
		cfg.owner()->attrDel(fidp+"bordU");
		cfg.owner()->attrDel(fidp+"aScale");
		cfg.owner()->attrDel(fidp+"color");
		cfg.owner()->attrDel(fidp+"val");
	    }
	}
	//> Create ullage attributes of parameters
	for( int i_p = 0; i_p < cfg.getI(); i_p++ )
	{
	    fidp = fid+TSYS::int2str(i_p);
	    fnmp = fnm+TSYS::int2str(i_p);
	    if( cfg.owner()->attrPresent( fidp+"addr" ) ) continue;
	    cfg.owner()->attrAdd( new TFld((fidp+"addr").c_str(),(fnmp+_(":address")).c_str(),
					    TFld::String,Attr::Address|Attr::Mutable,"","","","",TSYS::int2str(50+10*i_p).c_str()) );
	    cfg.owner()->attrAdd( new TFld((fidp+"bordL").c_str(),(fnmp+_(":view border:lower")).c_str(),
		TFld::Real,Attr::Mutable,"","","","",TSYS::int2str(51+10*i_p).c_str()) );
		cfg.owner()->attrAdd( new TFld((fidp+"bordU").c_str(),(fnmp+_(":view border:upper")).c_str(),
					    TFld::Real,Attr::Mutable,"","","","",TSYS::int2str(52+10*i_p).c_str()) );
		cfg.owner()->attrAdd( new TFld((fidp+"aScale").c_str(),(fnmp+_(":view border:autoscale")).c_str(),
					    TFld::Integer,Attr::Mutable,"","0","","",TSYS::int2str(55+10*i_p).c_str()) );
		cfg.owner()->attrAdd( new TFld((fidp+"color").c_str(),(fnmp+_(":color")).c_str(),
					    TFld::String,Attr::Color|Attr::Mutable,"","","","",TSYS::int2str(53+10*i_p).c_str()) );
		cfg.owner()->attrAdd( new TFld((fidp+"val").c_str(),(fnmp+_(":value")).c_str(),
					    TFld::Real,Attr::Mutable,"","","","",TSYS::int2str(54+10*i_p).c_str()) );
	}
    }

    return Widget::attrChange(cfg,prev);
}

//************************************************
//* OrigProtocol: Protocol original widget       *
//************************************************
OrigProtocol::OrigProtocol( ) : PrWidget("Protocol")
{

}

string OrigProtocol::name( )
{
    return _("Protocol");
}

string OrigProtocol::descr( )
{
    return _("Protocol widget of the finite visualisation.");
}

void OrigProtocol::postEnable( int flag )
{
    LWidget::postEnable(flag);

    if( flag&TCntrNode::NodeConnect )
    {
	attrAdd( new TFld("backColor",_("Background:color"),TFld::String,Attr::Color,"","","","","20") );
	attrAdd( new TFld("backImg",_("Background:image"),TFld::String,Attr::Image,"","","","","21") );
	attrAdd( new TFld("font",_("Font"),TFld::String,Attr::Font,"","Arial 11","","","22") );
	attrAdd( new TFld("headVis",_("Header visible"),TFld::Boolean,TFld::NoFlag,"","1","","","23") );
	attrAdd( new TFld("time",_("Time, sek"),TFld::Integer,Attr::DateTime,"","","","","24") );
	attrAdd( new TFld("tSize",_("Size, sek"),TFld::Integer,TFld::NoFlag,"","60","","","25") );
	attrAdd( new TFld("trcPer",_("Tracing period (s)"),TFld::Integer,TFld::NoFlag,"","0","0;360","","26") );
	attrAdd( new TFld("arch",_("Archivator"),TFld::String,TFld::NoFlag,"","","","","27") );
	attrAdd( new TFld("tmpl",_("Template"),TFld::String,TFld::NoFlag,"","","","","28") );
	attrAdd( new TFld("lev",_("Level"),TFld::Integer,TFld::NoFlag,"","0","-7;7","","29") );
	attrAdd( new TFld("viewOrd",_("View order"),TFld::Integer,TFld::Selected,"","0",
	    "0;1;2;3;4;5;6;7",_("On time;On level;On category;On messages;On time (reverse);On level (reverse);On category (reverse);On messages (reverse)"),"30") );
	attrAdd( new TFld("col",_("View columns"),TFld::String,TFld::NoFlag,"","pos;tm;utm;lev;cat;mess","","","31") );
	attrAdd( new TFld("itProp",_("Item's properties"),TFld::Integer,Attr::Active,"","","","","32") );
    }
}

bool OrigProtocol::attrChange( Attr &cfg, TVariant prev )
{
    if( cfg.flgGlob()&Attr::Active )
    {
	if( cfg.id() == "itProp" )
	{
	    string fid("it"), fnm(_("Item ")), fidp, fnmp;
	    //> Delete specific unnecessary items
	    for( int i_p = 0; true; i_p++ )
	    {
		fidp = fid+TSYS::int2str(i_p);
		if( !cfg.owner()->attrPresent( fidp+"lev" ) )	break;
		else if( i_p >= cfg.getI() )
		{
		    cfg.owner()->attrDel(fidp+"lev");
		    cfg.owner()->attrDel(fidp+"tmpl");
		    cfg.owner()->attrDel(fidp+"fnt");
		    cfg.owner()->attrDel(fidp+"color");
		}
	    }
	    //> Create ullage items
	    for( int i_p = 0; i_p < cfg.getI(); i_p++ )
	    {
		fidp = fid+TSYS::int2str(i_p);
		fnmp = fnm+TSYS::int2str(i_p);
		if( cfg.owner()->attrPresent( fidp+"lev" ) ) continue;
		cfg.owner()->attrAdd( new TFld((fidp+"lev").c_str(),(fnmp+_(":level")).c_str(),
					       TFld::Integer,Attr::Mutable,"","0","0;7","",TSYS::int2str(40+5*i_p).c_str()) );
		cfg.owner()->attrAdd( new TFld((fidp+"tmpl").c_str(),(fnmp+_(":template")).c_str(),
					       TFld::String,Attr::Mutable,"","","","",TSYS::int2str(41+5*i_p).c_str()) );
		cfg.owner()->attrAdd( new TFld((fidp+"fnt").c_str(),(fnmp+_(":font")).c_str(),
					       TFld::String,Attr::Font|Attr::Mutable,"","","","",TSYS::int2str(42+5*i_p).c_str()) );
		cfg.owner()->attrAdd( new TFld((fidp+"color").c_str(),(fnmp+_(":color")).c_str(),
					       TFld::String,Attr::Color|Attr::Mutable,"","","","",TSYS::int2str(43+5*i_p).c_str()) );
	    }
	}
    }
    return Widget::attrChange(cfg,prev);
}

//************************************************
//* OrigDocument: Document original widget       *
//************************************************
const char *OrigDocument::XHTML_entity =
    "<!DOCTYPE xhtml [\n"
    "  <!ENTITY nbsp \"&#160;\" >\n"
    "]>\n";

OrigDocument::OrigDocument( ) : PrWidget("Document")
{

}

string OrigDocument::name( )
{
    return _("Document");
}

string OrigDocument::descr( )
{
    return _("Document widget of the finite visualisation.");
}

void OrigDocument::postEnable( int flag )
{
    LWidget::postEnable(flag);

    if( flag&TCntrNode::NodeConnect )
    {
	attrAdd( new TFld("style",_("CSS"),TFld::String,TFld::FullText,"","","","","20") );
	attrAdd( new TFld("tmpl",_("Template"),TFld::String,TFld::FullText,"","","","","21") );
	attrAdd( new TFld("doc",_("Document"),TFld::String,TFld::FullText,"","","","","22") );
	attrAdd( new TFld("font",_("Font"),TFld::String,Attr::Font,"","Arial 11","","","26") );
	attrAdd( new TFld("bTime",_("Time:begin"),TFld::Integer,Attr::DateTime,"","0","","","24") );
	attrAdd( new TFld("time",_("Time:current"),TFld::Integer,Attr::DateTime|Attr::Active,"","0","","","23") );
	attrAdd( new TFld("n",_("Archive size"),TFld::Integer,Attr::Active,"","0","0;99","","25") );
    }
}

void OrigDocument::calc( Widget *base )
{
    //> Make document after time set
    if( base->attrAt("time").at().flgSelf()&0x100 )
    {
	base->attrAt("time").at().setFlgSelf((Attr::SelfAttrFlgs)(base->attrAt("time").at().flgSelf()&(~0x100)));
	string mkDk;
	int n = base->attrAt("n").at().getI();
	if( !n )
	{
	    mkDk = base->attrAt("doc").at().getS();
	    if( mkDk.empty() )	mkDk = base->attrAt("tmpl").at().getS();
	    mkDk = makeDoc(mkDk,base);
	    base->attrAt("doc").at().setS(mkDk);
	}
	else
	{
	    int aCur = base->attrAt("aCur").at().getI();
	    mkDk = base->attrAt("doc"+TSYS::int2str(aCur)).at().getS();
	    if( mkDk.empty() )	mkDk = base->attrAt("tmpl").at().getS();

	    mkDk = makeDoc(mkDk,base);
	    base->attrAt("doc"+TSYS::int2str(aCur)).at().setS(mkDk);
	    if( aCur == base->attrAt("vCur").at().getI() )
		base->attrAt("doc").at().setS(mkDk);
	}
    }
}

bool OrigDocument::attrChange( Attr &cfg, TVariant prev )
{
    int off = 0;

    //> Document's number change process
    if( cfg.id() == "n" && cfg.getI() != prev.getI() )
    {
	if( !cfg.getI() )
	{
	    cfg.owner()->attrDel("aCur");
	    cfg.owner()->attrDel("vCur");
	}
	else
	{
	    if( !cfg.owner()->attrPresent("vCur") )
	    {
		cfg.owner()->attrAdd( new TFld("vCur",_("Cursor:view"),TFld::Integer,Attr::Mutable|Attr::Active,"","0","-2;99") );
		cfg.owner()->inheritAttr("vCur");
	    }
	    if( !cfg.owner()->attrPresent("aCur") )
	    {
		cfg.owner()->attrAdd( new TFld("aCur",_("Cursor:archive"),TFld::Integer,Attr::Mutable|Attr::Active,"","0","-1;99") );
		cfg.owner()->inheritAttr("aCur");
	    }
	}

	string fidp;
	//>> Delete archive document's attributes
	for( int i_p = 0; true; i_p++ )
	{
	    fidp = "doc"+TSYS::int2str(i_p);
	    if( !cfg.owner()->attrPresent(fidp) )	break;
	    else if( i_p >= cfg.getI() )	cfg.owner()->attrDel(fidp);
	}

	//>> Create archive document's attributes
	for( int i_p = 0; i_p < cfg.getI(); i_p++ )
	{
	    fidp = "doc"+TSYS::int2str(i_p);
	    if( cfg.owner()->attrPresent(fidp) )	continue;
	    cfg.owner()->attrAdd( new TFld(fidp.c_str(),(_("Document ")+TSYS::int2str(i_p)).c_str(),TFld::String,TFld::FullText|Attr::Mutable|Attr::Active) );
	}
    }

    SessWdg *sw = dynamic_cast<SessWdg*>(cfg.owner());
    if( !sw )	return Widget::attrChange(cfg,prev);

    //> Make document after time set
    if( cfg.id() == "time" && cfg.getI() != prev.getI() ) cfg.setFlgSelf((Attr::SelfAttrFlgs)(cfg.flgSelf()|0x100));
    //> Load document's from project's DB
    else if( cfg.id() == "n" && cfg.getI() != prev.getI() )
    {
	string db  = sw->ownerSess()->parent().at().DB();
	string tbl = sw->ownerSess()->parent().at().tbl()+"_ses";

	TConfig c_el(&mod->elPrjSes());
	TSYS::pathLev(sw->path(),0,true,&off);
	c_el.cfg("IDW").setS(sw->path().substr(off));
	//>> Archive position load
	c_el.cfg("ID").setS("aCur");
	if( SYS->db().at().dataGet(db+"."+tbl,mod->nodePath()+tbl,c_el) )
	    cfg.owner()->attrAt("aCur").at().setI(c_el.cfg("IO_VAL").getI(),false,true);
	//>> Documents load
	for( int i_d = prev.getI(); i_d < cfg.getI(); i_d++ )
	{
	    c_el.cfg("ID").setS("doc"+TSYS::int2str(i_d));
	    if( SYS->db().at().dataGet(db+"."+tbl,mod->nodePath()+tbl,c_el) )
		cfg.owner()->attrAt("doc"+TSYS::int2str(i_d)).at().setS(c_el.cfg("IO_VAL").getS(),false,true);
	}
	//>> Set curent document
	cfg.owner()->attrAt("vCur").at().setI(cfg.owner()->attrAt("aCur").at().getI(),false,true);
	cfg.owner()->attrAt("doc").at().setS(cfg.owner()->attrAt("doc"+TSYS::int2str(cfg.owner()->attrAt("aCur").at().getI())).at().getS(),false,true);
	//>> Parse curent document and restore last document's time
	string cdoc = cfg.owner()->attrAt("doc").at().getS();
	if( !cdoc.empty() )
	{
	    XMLNode xdoc;
	    try{ xdoc.load(XHTML_entity+cdoc); } catch(TError err) { }
	    cfg.owner()->attrAt("time").at().setS(xdoc.attr("docTime"),false,true);
	}
    }
    //> Move archive cursor
    else if( cfg.id() == "aCur" && cfg.getI() != prev.getI() )
    {
	int n = cfg.owner()->attrAt("n").at().getI();
	if( cfg.getI() < 0 )		cfg.setI( (prev.getI()+1 >= n) ? 0 : prev.getI()+1, false,true );
	else if( cfg.getI() >= n )	cfg.setI( n-1, false,true );
	if( cfg.getI() != prev.getI() )
	{
	    cfg.owner()->attrAt("doc"+TSYS::int2str(cfg.getI())).at().setS(cfg.owner()->attrAt("tmpl").at().getS());
	    if( prev.getI() == cfg.owner()->attrAt("vCur").at().getI() )
		cfg.owner()->attrAt("vCur").at().setI(cfg.getI());

	    //>> Save cursor to document to project's DB
	    if( prev.getI() < n && prev.getI() >= 0 )
	    {
		string db  = sw->ownerSess()->parent().at().DB();
		string tbl = sw->ownerSess()->parent().at().tbl()+"_ses";

		TConfig c_el(&mod->elPrjSes());
		TSYS::pathLev(sw->path(),0,true,&off);
		c_el.cfg("IDW").setS(sw->path().substr(off));
		c_el.cfg("ID").setS(cfg.id());
		c_el.cfg("IO_VAL").setI(cfg.getI());
		SYS->db().at().dataSet(db+"."+tbl,mod->nodePath()+tbl,c_el);
	    }
	}
    }
    //> Document save
    else if( cfg.id().substr(0,3) == "doc" && cfg.getS() != prev.getS() )
    {
	string db  = sw->ownerSess()->parent().at().DB();
	string tbl = sw->ownerSess()->parent().at().tbl()+"_ses";

	TConfig c_el(&mod->elPrjSes());
	TSYS::pathLev(sw->path(),0,true,&off);
	c_el.cfg("IDW").setS(sw->path().substr(off));
	c_el.cfg("ID").setS(cfg.id());
	c_el.cfg("IO_VAL").setS(cfg.getS());
	SYS->db().at().dataSet(db+"."+tbl,mod->nodePath()+tbl,c_el);
    }
    //> Move archive view cursor
    else if( cfg.id() == "vCur" && cfg.getI() != prev.getI() )
    {
	int aCur = cfg.owner()->attrAt("aCur").at().getI();
	int n = cfg.owner()->attrAt("n").at().getI();
	if( cfg.getI() < 0 )
	{
	    int docN = prev.getI();
	    //>> Search next document
	    if( cfg.getI() == -1 )
	    {
		while( docN != aCur && (docN == prev.getI() || cfg.owner()->attrAt("doc"+TSYS::int2str(docN)).at().getS().empty()) )
		    if( ++docN >= n )	docN = 0;
	    }
	    //>> Search previous document
	    else
	    {
		if( --docN < 0 ) docN = n-1;
		if( docN == aCur || cfg.owner()->attrAt("doc"+TSYS::int2str(docN)).at().getS().empty() )
		    docN = prev.getI();
	    }
	    if( docN != cfg.getI() )	cfg.setI(docN,false,true);
	}
	else if( cfg.getI() >= n )	cfg.setI( cfg.owner()->attrAt("aCur").at().getI(), false, true );
	if( cfg.getI() != prev.getI() )
	    cfg.owner()->attrAt("doc").at().setS( cfg.owner()->attrAt("doc"+TSYS::int2str(cfg.getI())).at().getS() );
    }

    return Widget::attrChange(cfg,prev);
}

string OrigDocument::makeDoc( const string &tmpl, Widget *wdg )
{
    XMLNode xdoc;
    string iLang;				//Process instruction language
    string wProgO;				//Object of work programm
    time_t lstTime;				//Last time
    TFunction funcIO(TSYS::path2sepstr(wdg->path(),'_'));
    TValFunc funcV(wdg->id()+"_doc",NULL,false);
    vector<string> als;

    //> Parse template
    try{ xdoc.load(XHTML_entity+tmpl); }
    catch(TError err)
    {
	mess_err(wdg->nodePath().c_str(),_("Document's template parsing error: %s."),err.mess.c_str());
	return "";
    }

    //> Prepare call instructions environment
    if( strcasecmp(xdoc.name().c_str(),"body") == 0 )
    {
	iLang = xdoc.attr("docProcLang");
	lstTime = atoi(xdoc.attr("docTime").c_str());
    }
    if( TSYS::strNoSpace(iLang).empty() )	iLang = "JavaLikeCalc.JavaScript";
    if( !lstTime )		lstTime = wdg->attrAt("bTime").at().getI();

    //>> Add generic io
    funcIO.ioIns( new IO("rez",_("Result"),IO::String,IO::Return),0);
    funcIO.ioIns( new IO("time",_("Document time"),IO::Integer,IO::Default),1);
    funcIO.ioIns( new IO("bTime",_("Document begin time"),IO::Integer,IO::Default),2);
    funcIO.ioIns( new IO("lTime",_("Last time"),IO::Integer,IO::Default),3);
    funcIO.ioIns( new IO("rTime",_("Repeat time (s)"),IO::Integer,IO::Default),4);
    funcIO.ioIns( new IO("rTimeU",_("Repeat time (us)"),IO::Integer,IO::Default),5);
    funcIO.ioIns( new IO("rPer",_("Repeat period"),IO::Real,IO::Default),6);
    funcIO.ioIns( new IO("mTime",_("Message time"),IO::Integer,IO::Default),7);
    funcIO.ioIns( new IO("mTimeU",_("Message time (mcs)"),IO::Integer,IO::Default),8);
    funcIO.ioIns( new IO("mLev",_("Message level"),IO::Integer,IO::Default),9);
    funcIO.ioIns( new IO("mCat",_("Message category"),IO::String,IO::Default),10);
    funcIO.ioIns( new IO("mVal",_("Message value"),IO::String,IO::Default),11);
    //>> Add user io
    wdg->attrList(als);
    for( int i_a = 0; i_a < als.size(); i_a++ )
    {
	AutoHD<Attr> cattr = wdg->attrAt(als[i_a]);
	if( !(cattr.at().flgGlob()&Attr::IsUser) )	continue;
	IO::Type tp = IO::String;
	switch( cattr.at().type() )
	{
	    case TFld::Boolean:	tp = IO::Boolean;	break;
	    case TFld::Integer:	tp = IO::Integer;	break;
	    case TFld::Real:	tp = IO::Real;		break;
	    case TFld::String:	tp = IO::String;	break;
	}
	funcIO.ioAdd( new IO(als[i_a].c_str(),cattr.at().name().c_str(),tp,IO::Output) );
    }
    try
    {
	//>> Compile empty function for binding to object
	wProgO = SYS->daq().at().at(TSYS::strSepParse(iLang,0,'.')).at().compileFunc(TSYS::strSepParse(iLang,1,'.'),funcIO,"");
	//>> Connect to compiled function
	funcV.setFunc(&((AutoHD<TFunction>)SYS->nodeAt(wProgO,1)).at());
	//>> Load values of generic IO
	funcV.setI(1,wdg->attrAt("time").at().getI());
	funcV.setI(2,wdg->attrAt("bTime").at().getI());
	funcV.setI(3,lstTime);
	//>> Load values of user IO
	for( int i_a = 12; i_a < funcV.ioSize( ); i_a++ )
	    funcV.setS(i_a,wdg->attrAt(funcV.func()->io(i_a)->id()).at().getS());
    }catch( TError err )
    {
	mess_err(wdg->nodePath().c_str(),_("Compile function for document is error: %s"),err.mess.c_str());
	return "";
    }

    //> Node proocess
    nodeProcess( wdg, &xdoc, funcV, funcIO, iLang );

    xdoc.setAttr("docTime",TSYS::int2str(funcV.getI(1)));

    return xdoc.save(XMLNode::BrAllPast);
}

void OrigDocument::nodeProcess( Widget *wdg, XMLNode *xcur, TValFunc &funcV, TFunction &funcIO, const string &iLang, bool instrDel )
{
    //> Process instructions
    if( !xcur->prcInstr("dp").empty() )
    {
	try
	{
	    //>>> Compile for new instruction
	    SYS->daq().at().at(TSYS::strSepParse(iLang,0,'.')).
		       at().compileFunc(TSYS::strSepParse(iLang,1,'.'),funcIO,xcur->prcInstr("dp"));
	    //>>> Call
	    funcV.setS(0,"");
	    funcV.calc( );
	    //>>> Load result to XML tree
	    XMLNode xproc;
	    xproc.load(string(XHTML_entity)+"<i>"+funcV.getS(0)+"</i>");
	    //>>> Set result
	    bool docAppend = atoi(xcur->attr("docAppend").c_str());
	    //>>> Copy text
	    if( docAppend )	xcur->setText(xcur->text()+xproc.text());
	    else xcur->setText(xproc.text());
	    //>>> Copy included tags
	    if( !docAppend )	xcur->childClear();
	    for( int i_t = 0; i_t < xproc.childSize(); i_t++ )
		*(xcur->childIns(0)) = *xproc.childGet(i_t);
	    if( instrDel )	xcur->prcInstrDel("dp");
	}
	catch( TError err )
	{
	    mess_err(wdg->nodePath().c_str(),_("Instruction proc <%s> error: %s"),TSYS::strSepParse(iLang,1,'.').c_str(),err.mess.c_str());
	    mess_err(wdg->nodePath().c_str(),_("Error code: %s"),xcur->prcInstr("dp").c_str());
	}
    }

    float dRpt;
    string dAMess;
    //> Go to include nodes
    for( int i_c = 0; i_c < xcur->childSize(); i_c++ )
    {
	//>> Repeat tags
	if( funcV.getI(2) && (dRpt=atof(xcur->childGet(i_c)->attr("docRept").c_str())) > 1e-6 )
	{
	    int rCnt = 0;
	    XMLNode *reptN = xcur->childGet(i_c);
	    bool docRevers = atoi(xcur->childGet(i_c)->attr("docRevers").c_str());
	    funcV.setR(6,dRpt);

	    long long time = (long long)funcV.getI(1)*1000000;
	    long long bTime = (long long)funcV.getI(2)*1000000;
	    long long lstTime = (long long)funcV.getI(3)*1000000;
	    long long perRpt = (long long)(1000000*dRpt);
	    long long rTime = bTime + perRpt*((lstTime-bTime)/perRpt);
	    if( lstTime && lstTime<bTime ) rTime-=perRpt;
	    if( ((time-rTime)/perRpt) > 1000 ) continue;
	    while( rTime < time )
	    {
		if( atoi(reptN->attr("docRptEnd").c_str()) )
		{
		    int i_n = docRevers?(i_c+1):i_c;
		    *(xcur->childIns(i_n)) = *reptN;
		    nodeClear(xcur->childGet(i_n));
		    if( !docRevers ) i_c++;
		    rCnt++;
		}
		long long rTimeT = vmin(rTime+perRpt,time);
		funcV.setI(4,rTimeT/1000000); funcV.setI(5,rTimeT%1000000); funcV.setR(6,(rTimeT-rTime)/1000000.0);
		nodeProcess(wdg,reptN,funcV,funcIO,iLang);
		reptN->setAttr("docRptEnd",((rTimeT-rTime)==perRpt)?"1":"0");
		rTime = rTimeT;
	    }
	    funcV.setI(4,0); funcV.setI(5,0); funcV.setR(6,0);
	    if( docRevers ) i_c += rCnt;
	}
	//>> Repeat messages
	else if( !(dAMess=xcur->childGet(i_c)->attr("docAMess")).empty() )
	{
	    int rCnt = 0;
	    XMLNode *reptN = xcur->childGet(i_c);
	    bool docRevers = atoi(xcur->childGet(i_c)->attr("docRevers").c_str());

	    int off = 0;
	    int dACat = atoi(TSYS::strSepParse(dAMess,0,':',&off).c_str());
	    string dATmpl = dAMess.substr(off);

	    vector<TMess::SRec> mess;
	    SYS->archive().at().messGet( funcV.getI(3), funcV.getI(1), mess, dATmpl, (TMess::Type)dACat );

	    for( int i_r = 0; i_r < mess.size(); i_r++ )
	    {
		if( atoi(reptN->attr("docRptEnd").c_str()) )
		{
		    int i_n = docRevers?(i_c+1):i_c;
		    *(xcur->childIns(i_n)) = *reptN;
		    nodeClear(xcur->childGet(i_n));
		    if( !docRevers ) i_c++;
		    rCnt++;
		}
		funcV.setI(7,mess[i_r].time);
		funcV.setI(8,mess[i_r].utime);
		funcV.setI(9,mess[i_r].level);
		funcV.setS(10,mess[i_r].categ);
		funcV.setS(11,mess[i_r].mess);
		nodeProcess(wdg,reptN,funcV,funcIO,iLang);
		reptN->setAttr("docRptEnd","1");
	    }
	    funcV.setI(7,0); funcV.setI(8,0); funcV.setI(9,0); funcV.setS(10,""); funcV.setS(11,"");
	    if( docRevers ) i_c += rCnt;
	}
	else nodeProcess(wdg,xcur->childGet(i_c),funcV,funcIO,iLang,instrDel);
    }
}

void OrigDocument::nodeClear( XMLNode *xcur )
{
    xcur->prcInstrClear();
    xcur->attrDel("docRept");
    xcur->attrDel("docRptEnd");
    xcur->attrDel("docRevers");
    xcur->attrDel("docAMess");

    for( int i_c = 0; i_c < xcur->childSize(); i_c++ )
	nodeClear( xcur->childGet(i_c) );
}

//************************************************
//* OrigFunction: User function original widget  *
//************************************************
OrigFunction::OrigFunction( ) : PrWidget("Function")
{

}

string OrigFunction::name( )
{
    return _("Buildin function");
}

string OrigFunction::descr( )
{
    return _("Buildin function widget.");
}

//************************************************
//* OrigBox: User element original widget        *
//************************************************
OrigBox::OrigBox( ) : PrWidget("Box")
{

}

string OrigBox::name( )
{
    return _("Elements box");
}

string OrigBox::descr( )
{
    return _("Elements box widget of the finite visualisation.");
}

void OrigBox::postEnable( int flag )
{
    LWidget::postEnable(flag);

    if( flag&TCntrNode::NodeConnect )
    {
	attrAdd( new TFld("pgOpenSrc",_("Page:open source"),TFld::String,TFld::NoFlag,"","","","","3") );
	attrAdd( new TFld("pgGrp",_("Page:group"),TFld::String,TFld::NoFlag,"","","","","4") );
	attrAdd( new TFld("backColor",_("Background:color"),TFld::String,Attr::Color,"","#FFFFFF","","","20") );
	attrAdd( new TFld("backImg",_("Background:image"),TFld::String,Attr::Image,"","","","","21") );
	attrAdd( new TFld("bordWidth",_("Border:width"),TFld::Integer,TFld::NoFlag,"","0","","","22") );
	attrAdd( new TFld("bordColor",_("Border:color"),TFld::String,Attr::Color,"","#000000","","","23") );
	attrAdd( new TFld("bordStyle",_("Border:style"),TFld::Integer,TFld::Selected,"","3","0;1;2;3;4;5;6;7;8",
						_("None;Dotted;Dashed;Solid;Double;Groove;Ridge;Inset;Outset"),"24") );
    }
}
