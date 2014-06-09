
//OpenSCADA system module UI.VCAEngine file: origwidg.cpp
/***************************************************************************
 *   Copyright (C) 2006-2014 by Roman Savochenko, <rom_as@oscada.org>      *
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

#include "types.h"
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
    if(flag) throw TError(mod->nodePath().c_str(),_("Deleting the base primitive-widget error."));

    Widget::preDisable(flag);
}

string PrWidget::ico( )
{
    if(LWidget::ico().size()) return LWidget::ico();
    if(TUIS::icoGet("VCA.wdg_"+id(),NULL,true).size())
	return TSYS::strEncode(TUIS::icoGet("VCA.wdg_"+id()),TSYS::base64);
    return "";
}

void PrWidget::setEnable( bool val )
{
    if(enable() == val) return;

    LWidget::setEnable(val);

    //> Init active attributes
    if(val)
    {
	vector<string> ls;
	attrList(ls);
	for(unsigned i_l = 0; i_l < ls.size(); i_l++)
	{
	    AutoHD<Attr> attr = attrAt(ls[i_l]);
	    if(!(attr.at().flgGlob()&Attr::Active)) continue;
	    attr.at().setS(attr.at().getS(),true);
	    attr.at().setModif(0);
    	    attrList(ls);
	}
    }
}

bool PrWidget::cntrCmdGeneric( XMLNode *opt )
{
    //> Get page info
    if(opt->name() == "info")
    {
	Widget::cntrCmdGeneric(opt);
	ctrMkNode("oscada_cntr",opt,-1,"/",_("Base widget: ")+id());
	ctrMkNode("fld",opt,-1,"/wdg/st/parent",_("Parent"),R_R_R_,owner().c_str(),grp().c_str(),2,"tp","str","dest","");
	return true;
    }

    //> Process command to page
    string a_path = opt->attr("path");
    if(a_path == "/wdg/st/parent" && ctrChkNode(opt,"get",R_R_R_,owner().c_str(),grp().c_str(),SEC_RD))
	opt->setText(parentNm());
    else return Widget::cntrCmdGeneric(opt);

    return true;
}

void PrWidget::cntrCmdProc( XMLNode *opt )
{
    if(cntrCmdServ(opt)) return;

    //> Get page info
    if(opt->name() == "info")
    {
	cntrCmdGeneric(opt);
	cntrCmdAttributes(opt );
	return;
    }

    //> Process command to page
    if(!(cntrCmdGeneric(opt) || cntrCmdAttributes(opt)))
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
    return _("Elementary figures widget of the finite visualization.");
}

void OrigElFigure::postEnable( int flag )
{
    LWidget::postEnable(flag);

    if(flag&TCntrNode::NodeConnect)
    {
	attrAdd(new TFld("lineWdth",_("Line:width"),TFld::Integer,TFld::NoFlag,"","1","0;99","",i2s(A_ElFigLineW).c_str()));
	attrAdd(new TFld("lineClr",_("Line:color"),TFld::String,Attr::Color,"","#000000","","",i2s(A_ElFigLineClr).c_str()));
	attrAdd(new TFld("lineStyle",_("Line:style"),TFld::Integer,TFld::Selected,"","0",
	    TSYS::strMess("%d;%d;%d",EF_SOLID,EF_DASH,EF_DOT).c_str(),_("Solid;Dashed;Dotted"),i2s(A_ElFigLineStl).c_str()));
	attrAdd(new TFld("bordWdth",_("Border:width"),TFld::Integer,TFld::NoFlag,"","0","0;99","",i2s(A_ElFigBordW).c_str()));
	attrAdd(new TFld("bordClr",_("Border:color"),TFld::String,Attr::Color,"","#000000","","",i2s(A_ElFigBordClr).c_str()));
	attrAdd(new TFld("fillColor",_("Fill:color"),TFld::String,Attr::Color,"","","","",i2s(A_ElFigFillClr).c_str()));
	attrAdd(new TFld("fillImg",_("Fill:image"),TFld::String,Attr::Image,"","","","",i2s(A_ElFigFillImg).c_str()));
	attrAdd(new TFld("orient",_("Orientation angle"),TFld::Integer,TFld::NoFlag,"","0","-360;360","",i2s(A_ElFigOrient).c_str()));
	attrAdd(new TFld("elLst",_("Elements list"),TFld::String,TFld::NoStrTransl|TFld::FullText|Attr::Active,"","","","",i2s(A_ElFigElLst).c_str()));
    }
}

bool OrigElFigure::cntrCmdAttributes( XMLNode *opt, Widget *src )
{
    if(!src) src = this;
    //> Get page info
    if(opt->name() == "info")
    {
	Widget::cntrCmdAttributes(opt,src);
	XMLNode *root, *el;
	if((root=ctrMkNode("area",opt,-1,"/attr",_("Attributes"))))
	    for(unsigned i_ch = 0; i_ch < root->childSize(); i_ch++)
	    {
		el = root->childGet(i_ch);
		int p = atoi(el->attr("p").c_str());
		switch(p)
		{
		    case A_ElFigLineClr: case A_ElFigBordClr: case A_ElFigFillClr: el->setAttr("help",Widget::helpColor());	break;
		    case A_ElFigFillImg: el->setAttr("help",Widget::helpImg());	break;
		    case A_ElFigElLst: el->setAttr("SnthHgl","1")->setAttr("help",
		    _("The list of elements can contain:\n"
	              "  line:(x|y)|{1}:(x|y)|{2}:[width|w{n}]:[color|c{n}]:[bord_w|w{n}]:[bord_clr|c{n}]:[line_stl|s{n}]\n"
	              "  arc:(x|y)|{1}:(x|y)|{2}:(x|y)|{3}:(x|y)|{4}:(x|y)|{5}:[width|w{n}]:[color|c{n}]:[bord_w|w{n}]:[bord_clr|c{n}]:[line_stl|s{n}]\n"
	              "  bezier:(x|y)|{1}:(x|y)|{2}:(x|y)|{3}:(x|y)|{4}:[width|w{n}]:[color|c{n}]:[bord_w|w{n}]:[bord_clr|c{n}]:[line_stl|s{n}]\n"
	              "  fill:(x|y)|{1},(x|y){2},...,(x|y)|{n}:[fill_clr|c{n}]:[fill_img|i{n}]\n"
	              "Where:\n"
	              "  (x|y)           - direct point (x,y) coordinate in float point pixels;\n"
	              "  {1}...{n}       - dynamic point 1...n;\n"
	              "  width, bord_w   - direct line and border width in float point pixels;\n"
	              "  w{n}            - dynamic width 'n';\n"
	              "  color, bord_clr, fill_clr - direct line, border and fill color name or 32bit code whith alpha: {name}-AAA, #RRGGBB-AAA;\n"
	              "  c{n}            - dynamic color 'n';\n"
	              "  line_stl        - direct line style: 0-Solid, 1-Dashed, 2-Dotted;\n"
	    	      "  s{n}            - dynamic style 'n';\n"
	    	      "  fill_img        - direct fill image in form \"[src%3Aname]\", where:\n"
	              "      \"src\" - image source:\n"
	              "         file - direct from local file by path;\n"
	              "         res  - from DB mime resources table.\n"
                      "      \"name\" - file path or resource mime Id.\n"
	              "  i{n}            - dynamic fill image 'n'.\n"
	              "For example:\n"
	              "  line:(50|25):(90.5|25):2:yellow:3:green:2\n"
	              "  arc:(25|50):(25|50):1:4:(25|50)::#000000-0\n"
	    	      "  fill:(25|50):(25|50):c2:i2\n"
	              "  fill:(50|25):(90.5|25):(90|50):(50|50):#d3d3d3:h_31"));
		    default:
			switch((p-A_ElFigIts)%A_ElFigItsSz)
			{
			    case A_ElFigItClr:	el->setAttr("help",Widget::helpColor());	break;
			    case A_ElFigItImg:	el->setAttr("help",Widget::helpImg());		break;
			}
			break;
		}
	    }
	return true;
    }

    //> Process command to page
    string a_path = opt->attr("path");
    if(a_path == "/attr/elLst" && ctrChkNode(opt,"SnthHgl",RWRWR_,"root",SUI_ID,SEC_RD))
    {
	opt->childAdd("rule")->setAttr("expr","\\:")->setAttr("color","darkblue")->setAttr("font_weight","1");
	opt->childAdd("rule")->setAttr("expr","^(arc|line|fill|bezier):")->setAttr("color","darkorange")->setAttr("font_weight","1");
	opt->childAdd("rule")->setAttr("expr","\\(\\d*\\.?\\d+\\|\\d*\\.?\\d+\\)")->setAttr("color","#3D87FF")->
	     childAdd("rule")->setAttr("expr","\\d*\\.?\\d+")->setAttr("color","blue");
	opt->childAdd("rule")->setAttr("expr","\\#([0-9a-fA-F]{6}\\-\\d+|[0-9a-fA-F]{6})")->setAttr("color","blue");
    }
    else return Widget::cntrCmdAttributes(opt,src);

    return true;
}

bool OrigElFigure::attrChange( Attr &cfg, TVariant prev )
{
    if(cfg.flgGlob()&Attr::Active && cfg.id() == "elLst")
    {
	string sel, sel1;
	string ls_prev = prev.getS();
	map<int,char> pntls, pntls_prev, wls, wls_prev, clrls, clrls_prev, imgls, imgls_prev, lstls, lstls_prev;
	//> Parse last attributes list and make point list
	for(int i_p = 0; i_p < 2; i_p++)
	{
	    string ls_w = (i_p == 0) ? ls_prev : cfg.getS();
	    map<int,char> &pntls_w = (i_p == 0) ? pntls_prev : pntls;
	    map<int,char> &wls_w = (i_p == 0) ? wls_prev : wls;
	    map<int,char> &clrls_w = (i_p == 0) ? clrls_prev : clrls;
	    map<int,char> &imgls_w = (i_p == 0) ? imgls_prev : imgls;
	    map<int,char> &lstls_w = (i_p == 0) ? lstls_prev : lstls;
	    for(int off = 0; (sel=TSYS::strSepParse(ls_w,0,'\n',&off)).size(); )
	    {
		int offe = 0;
		string fTp = TSYS::strSepParse(sel,0,':',&offe);
		if(fTp == "line" || fTp == "arc" || fTp == "bezier")
		{
		    int np = (fTp=="arc") ? 5 : ((fTp=="bezier") ? 4 : 2);
		    for(int i = 0; i < np; i++) pntls_w[atoi(TSYS::strSepParse(sel,0,':',&offe).c_str())] = true;
		    if((sel1=TSYS::strSepParse(sel,0,':',&offe)).size() && sel1[0] == 'w') wls_w[atoi(sel1.substr(1).c_str())] = true;
		    if((sel1=TSYS::strSepParse(sel,0,':',&offe)).size() && sel1[0] == 'c') clrls_w[atoi(sel1.substr(1).c_str())] = true;
		    if((sel1=TSYS::strSepParse(sel,0,':',&offe)).size() && sel1[0] == 'w') wls_w[atoi(sel1.substr(1).c_str())] = true;
		    if((sel1=TSYS::strSepParse(sel,0,':',&offe)).size() && sel1[0] == 'c') clrls_w[atoi(sel1.substr(1).c_str())] = true;
		    if((sel1=TSYS::strSepParse(sel,0,':',&offe)).size() && sel1[0] == 's') lstls_w[atoi(sel1.substr(1).c_str())] = true;
		}
		else if(fTp == "fill")
		    for(int zPnt = 0; !(sel1=TSYS::strSepParse(sel,0,':',&offe)).empty() || zPnt < 2; )
		    {
			if(sel1.empty())	zPnt++;
			if(sel1[0] == 'c')	clrls_w[atoi(sel1.substr(1).c_str())] = true;
			else if(sel1[0] == 'i')	imgls_w[atoi(sel1.substr(1).c_str())] = true;
		    }
	    }
	}

	//> Add new dynamic items
	//>> Add no present dynamic points
	for(map<int,char>::iterator it = pntls.begin(); it != pntls.end(); ++it)
	    if(it->first && pntls_prev.find(it->first) == pntls_prev.end() && !cfg.owner()->attrPresent("p"+i2s(it->first)+"x"))
	    {
		cfg.owner()->attrAdd(new TFld(("p"+i2s(it->first)+"x").c_str(),(_("Point ")+i2s(it->first)+":x").c_str(),
		    TFld::Real,Attr::Mutable,"","0","","",i2s(A_ElFigIts+it->first*A_ElFigItsSz+A_ElFigItPntX).c_str()));
		cfg.owner()->attrAdd(new TFld(("p"+i2s(it->first)+"y").c_str(),(_("Point ")+i2s(it->first)+":y").c_str(),
		    TFld::Real,Attr::Mutable,"","0","","",i2s(A_ElFigIts+it->first*A_ElFigItsSz+A_ElFigItPntY).c_str()));
	    }
	//>> Add no present dynamic widths
	for(map<int,char>::iterator it = wls.begin(); it != wls.end(); ++it)
	    if(it->first && wls_prev.find(it->first) == wls_prev.end() && !cfg.owner()->attrPresent("w"+i2s(it->first)))
		cfg.owner()->attrAdd(new TFld(("w"+i2s(it->first)).c_str(),(_("Width ")+i2s(it->first)).c_str(),
		    TFld::Real,Attr::Mutable,"","1","","",i2s(A_ElFigIts+it->first*A_ElFigItsSz+A_ElFigItW).c_str()));
	//>> Add no present dynamic colors
	for(map<int,char>::iterator it = clrls.begin(); it != clrls.end(); ++it)
	    if(it->first && clrls_prev.find(it->first) == clrls_prev.end() && !cfg.owner()->attrPresent("c"+i2s(it->first)))
		cfg.owner()->attrAdd(new TFld(("c"+i2s(it->first)).c_str(),(_("Color ")+i2s(it->first)).c_str(),
		    TFld::String,Attr::Mutable|Attr::Color,"","","","",i2s(A_ElFigIts+it->first*A_ElFigItsSz+A_ElFigItClr).c_str()));
	//>> Add no present dynamic images
	for(map<int,char>::iterator it = imgls.begin(); it != imgls.end(); ++it)
	    if(it->first && imgls_prev.find(it->first) == imgls_prev.end() && !cfg.owner()->attrPresent("i"+i2s(it->first)))
		cfg.owner()->attrAdd(new TFld(("i"+i2s(it->first)).c_str(),(_("Image ")+i2s(it->first)).c_str(),
		    TFld::String,Attr::Mutable|Attr::Image,"","","","",i2s(A_ElFigIts+it->first*A_ElFigItsSz+A_ElFigItImg).c_str()));
	//>> Add no present line styles
	for(map<int,char>::iterator it = lstls.begin(); it != lstls.end(); ++it)
	    if(it->first && lstls_prev.find(it->first) == lstls_prev.end() && !cfg.owner()->attrPresent("s"+i2s(it->first)))
		cfg.owner()->attrAdd(new TFld(("s"+i2s(it->first)).c_str(),(_("Style ")+i2s(it->first)).c_str(),
		    TFld::Integer,Attr::Mutable|TFld::Selected,"","0",
		    TSYS::strMess("%d;%d;%d",EF_SOLID,EF_DASH,EF_DOT).c_str(),_("Solid;Dashed;Dotted"),
		    i2s(A_ElFigIts+it->first*A_ElFigItsSz+A_ElFigItStl).c_str()));

	//> Delete no dynamic items
	//>> Delete dynamic points
	for(map<int,char>::iterator it = pntls_prev.begin(); it != pntls_prev.end(); ++it)
	    if(it->first && pntls.find(it->first) == pntls.end())
	    {
		cfg.owner()->attrDel("p"+i2s(it->first)+"x");
		cfg.owner()->attrDel("p"+i2s(it->first)+"y");
	    }
	//>> Delete dynamic widths
	for(map<int,char>::iterator it = wls_prev.begin(); it != wls_prev.end(); ++it)
	    if(it->first && wls.find(it->first) == wls.end())
		cfg.owner()->attrDel("w"+i2s(it->first));
	//>> Delete dynamic colors
	for(map<int,char>::iterator it = clrls_prev.begin(); it != clrls_prev.end(); ++it)
	    if(it->first && clrls.find(it->first) == clrls.end())
		cfg.owner()->attrDel("c"+i2s(it->first));
	//>> Delete dynamic images
	for(map<int,char>::iterator it = imgls_prev.begin(); it != imgls_prev.end(); ++it)
	    if(it->first && imgls.find(it->first) == imgls.end())
		cfg.owner()->attrDel("i"+i2s(it->first));
	//>> Delete dynamic line styles
	for(map<int,char>::iterator it = lstls_prev.begin(); it != lstls_prev.end(); ++it)
	    if(it->first && lstls.find(it->first) == lstls.end())
		cfg.owner()->attrDel("s"+i2s(it->first));
    }
    return Widget::attrChange(cfg, prev);
}

//*************************************************
//* OrigFormEl: Form element original widget      *
//*************************************************
OrigFormEl::OrigFormEl( ) : PrWidget("FormEl")
{

}

string OrigFormEl::name( )
{
    return _("Form elements");
}

string OrigFormEl::descr( )
{
    return _("Form elements widget of the finite visualization.");
}

void OrigFormEl::postEnable( int flag )
{
    LWidget::postEnable(flag);

    if(flag&TCntrNode::NodeConnect)
    {
	attrAdd(new TFld("elType",_("Element type"),TFld::Integer,TFld::Selected|Attr::Active,"2","0",
	    TSYS::strMess("%d;%d;%d;%d;%d;%d;%d;%d;%d;%d",F_LINE_ED,F_TEXT_ED,F_CHECK_BOX,F_BUTTON,F_COMBO,F_LIST,F_TREE,F_TABLE,F_SLIDER,F_SCROLL_BAR).c_str(),
	    _("Line edit;Text edit;Check box;Button;Combo box;List;Tree;Table;Slider;Scroll Bar"),i2s(A_FormElType).c_str()));
	attrAt("name").at().fld().setReserve(i2s(A_FormElName));
    }
}

bool OrigFormEl::attrChange( Attr &cfg, TVariant prev )
{
    if((cfg.flgGlob()&Attr::Active) && cfg.id() == "elType")
    {
	//> Delete specific attributes
	if(cfg.getI() != prev.getI())
	    switch(prev.getI())
	    {
		case F_LINE_ED:
		    cfg.owner()->attrDel("value");
		    cfg.owner()->attrDel("view");
		    cfg.owner()->attrDel("cfg");
		    cfg.owner()->attrDel("confirm");
		    cfg.owner()->attrDel("font");
		    break;
		case F_TEXT_ED:
		    cfg.owner()->attrDel("value");
		    cfg.owner()->attrDel("wordWrap");
		    cfg.owner()->attrDel("confirm");
		    cfg.owner()->attrDel("font");
		    break;
		case F_CHECK_BOX:
		    cfg.owner()->attrDel("value");
		    cfg.owner()->attrDel("font");
		    break;
		case F_BUTTON:
		    cfg.owner()->attrDel("value");
		    cfg.owner()->attrDel("img");
		    cfg.owner()->attrDel("color");
		    cfg.owner()->attrDel("colorText");
		    cfg.owner()->attrDel("mode");
		    cfg.owner()->attrDel("font");
		    break;
		case F_COMBO: case F_LIST: case F_TREE: case F_TABLE:
		    cfg.owner()->attrDel("value");
		    cfg.owner()->attrDel("items");
		    cfg.owner()->attrDel("font");
		    break;
		case F_SLIDER: case F_SCROLL_BAR:
		    cfg.owner()->attrDel("value");
		    cfg.owner()->attrDel("cfg");
		    break;
	    }

	//> Create specific attributes
	switch(cfg.getI())
	{
	    case F_LINE_ED:
		cfg.owner()->attrAdd(new TFld("value",_("Value"),TFld::String,Attr::Mutable,"200","","","",i2s(A_FormElValue).c_str()));
		cfg.owner()->attrAdd(new TFld("view",_("View"),TFld::Integer,TFld::Selected|Attr::Mutable|Attr::Active,"1","0",
		    TSYS::strMess("%d;%d;%d;%d;%d;%d;%d",FL_TEXT,FL_COMBO,FL_INTEGER,FL_REAL,FL_TIME,FL_DATE,FL_DATE_TM).c_str(),
		    _("Text;Combo;Integer;Real;Time;Date;Date and time"),i2s(A_FormElMixP1).c_str()));
		cfg.owner()->attrAdd(new TFld("cfg",_("Configuration"),TFld::String,TFld::FullText|Attr::Mutable,"","","","",i2s(A_FormElMixP2).c_str()));
		cfg.owner()->attrAdd(new TFld("confirm",_("Confirm"),TFld::Boolean,Attr::Mutable,"","1","","",i2s(A_FormElMixP3).c_str()));
		cfg.owner()->attrAdd(new TFld("font",_("Font"),TFld::String,Attr::Font,"50","Arial 11","","",i2s(A_FormElFont).c_str()));
		break;
	    case F_TEXT_ED:
		cfg.owner()->attrAdd(new TFld("value",_("Value"),TFld::String,TFld::FullText|Attr::Mutable,"","","","",i2s(A_FormElValue).c_str()));
		cfg.owner()->attrAdd(new TFld("wordWrap",_("Word wrap"),TFld::Boolean,Attr::Mutable,"1","1","","",i2s(A_FormElMixP1).c_str()));
		cfg.owner()->attrAdd(new TFld("confirm",_("Confirm"),TFld::Boolean,Attr::Mutable,"","1","","",i2s(A_FormElMixP3).c_str()));
		cfg.owner()->attrAdd(new TFld("font",_("Font"),TFld::String,Attr::Font,"50","Arial 11","","",i2s(A_FormElFont).c_str()));
		break;
	    case F_CHECK_BOX:
		cfg.owner()->attrAdd(new TFld("value",_("Value"),TFld::Boolean,Attr::Mutable,"","","","",i2s(A_FormElValue).c_str()));
		cfg.owner()->attrAdd(new TFld("font",_("Font"),TFld::String,Attr::Font,"50","Arial 11","","",i2s(A_FormElFont).c_str()));
		break;
	    case F_BUTTON:
		cfg.owner()->attrAdd(new TFld("value",_("Repeat"),TFld::String,Attr::Mutable,"","","","",i2s(A_FormElValue).c_str()));
		cfg.owner()->attrAdd(new TFld("img",_("Image"),TFld::String,Attr::Image|Attr::Mutable,"","","","",i2s(A_FormElMixP1).c_str()));
		cfg.owner()->attrAdd(new TFld("color",_("Color:button"),TFld::String,Attr::Color|Attr::Mutable,"20","","","",i2s(A_FormElMixP2).c_str()));
		cfg.owner()->attrAdd(new TFld("colorText",_("Color:text"),TFld::String,Attr::Color|Attr::Mutable,"20","","","",i2s(A_FormElMixP4).c_str()));
		cfg.owner()->attrAdd(new TFld("mode",_("Mode"),TFld::Integer,TFld::Selected|Attr::Mutable|Attr::Active,"1","0",
		    TSYS::strMess("%d;%d;%d;%d;%d",FBT_STD,FBT_CHECK,FBT_MENU,FBT_LOAD,FBT_SAVE).c_str(),
		    _("Standard;Checkable;Menu;Load;Save"),i2s(A_FormElMixP3).c_str()));
		cfg.owner()->attrAdd(new TFld("font",_("Font"),TFld::String,Attr::Font,"50","Arial 11","","",i2s(A_FormElFont).c_str()));
		break;
	    case F_COMBO: case F_LIST: case F_TREE: case F_TABLE:
		cfg.owner()->attrAdd(new TFld("value",_("Value"),TFld::String,Attr::Mutable,"200","","","",i2s(A_FormElValue).c_str()));
		cfg.owner()->attrAdd(new TFld("items",_("Items"),TFld::String,TFld::FullText|Attr::Mutable,"","","","",i2s(A_FormElMixP1).c_str()));
		cfg.owner()->attrAdd(new TFld("font",_("Font"),TFld::String,Attr::Font,"50","Arial 11","","",i2s(A_FormElFont).c_str()));
		break;
	    case F_SLIDER: case F_SCROLL_BAR:
		cfg.owner()->attrAdd(new TFld("value",_("Value"),TFld::Integer,Attr::Mutable,"20","0","","",i2s(A_FormElValue).c_str()));
		cfg.owner()->attrAdd(new TFld("cfg",_("Configuration"),TFld::String,Attr::Mutable,"100","0:0:100:1:10","","",i2s(A_FormElMixP1).c_str()));
		break;
	}
    }
    //> Value type change
    else if((cfg.flgGlob()&Attr::Active) && ((cfg.id() == "view" && cfg.owner()->attrAt("elType").at().getI() == F_LINE_ED) ||
					     (cfg.id() == "mode" && cfg.owner()->attrAt("elType").at().getI() == F_BUTTON)))
    {
	TFld::Type	ntp = TFld::String;
	int		flg = Attr::Mutable;
	Attr::SelfAttrFlgs	sflg = cfg.owner()->attrAt("value").at().flgSelf();
	string		val = cfg.owner()->attrAt("value").at().getS();
	string		cfgTmpl = cfg.owner()->attrAt("value").at().cfgTempl();
	string		cfgVal = cfg.owner()->attrAt("value").at().cfgVal();
	string		vName = _("Value");
	if(cfg.id() == "view")
	    switch(cfg.getI())
	    {
		case FL_INTEGER: case FL_TIME:	ntp = TFld::Integer;	break;
		case FL_REAL:			ntp = TFld::Real;	break;
		case FL_DATE: case FL_DATE_TM:	ntp = TFld::Integer; flg |= Attr::DateTime;	break;
	    }
	else	// mode
	    switch(cfg.getI())
	    {
		case FBT_STD:	ntp = TFld::String; vName = _("Repeat");	break;
		case FBT_CHECK:	ntp = TFld::Boolean; vName = _("Check");	break;
		case FBT_MENU:	ntp = TFld::String; flg |= TFld::FullText; vName = _("Items");	break;
		case FBT_LOAD: case FBT_SAVE:
		    ntp = TFld::String; flg |= TFld::FullText; vName = _("File content");
		    break;
	    }
	int apos = cfg.owner()->attrPos("value");
	cfg.owner()->attrDel("value");
	cfg.owner()->attrAdd(new TFld("value",vName.c_str(),ntp,flg,"200","","","",i2s(A_FormElValue).c_str()), apos);
	cfg.owner()->attrAt("value").at().setFlgSelf(sflg);
	cfg.owner()->attrAt("value").at().setS(val);
	cfg.owner()->attrAt("value").at().setCfgTempl(cfgTmpl);
	cfg.owner()->attrAt("value").at().setCfgVal(cfgVal);
    }

    return Widget::attrChange(cfg, prev);
}

bool OrigFormEl::cntrCmdAttributes( XMLNode *opt, Widget *src )
{
    if(!src) src = this;
    //> Get page info
    if(opt->name() == "info")
    {
	Widget::cntrCmdAttributes(opt, src);
	XMLNode *root, *el;
	int elTp = 0;
	if((root=ctrMkNode("area",opt,-1,"/attr",_("Attributes"))))
	    switch((elTp=src->attrAt("elType").at().getI()))
	    {
		case F_LINE_ED:
		    if((el=ctrId(root,"/font",true)))	el->setAttr("help",Widget::helpFont());
		    if(!(el=ctrId(root,"/cfg",true)))	break;
		    switch(src->attrAt("view").at().getI())
		    {
			case FL_TEXT:
			    el->setAttr("help",_("Enter text line by template with the char items:\n"
				"  'A' - ASCII alphabetic character required. A-Z, a-z.\n"
				"  'a' - ASCII alphabetic character permitted but not required.\n"
				"  'N' - ASCII alphanumeric character required. A-Z, a-z, 0-9.\n"
				"  'n' - ASCII alphanumeric character permitted but not required.\n"
				"  'X' - Any character required.\n"
				"  'x' - Any character permitted but not required.\n"
				"  '9' - ASCII digit required. 0-9.\n"
				"  '0' - ASCII digit permitted but not required.\n"
				"  'D' - ASCII digit required. 1-9.\n"
				"  'd' - ASCII digit permitted but not required (1-9).\n"
				"  '#' - ASCII digit or plus/minus sign permitted but not required.\n"
				"  'H' - Hexadecimal character required. A-F, a-f, 0-9.\n"
				"  'h' - Hexadecimal character permitted but not required.\n"
				"  'B' - Binary character required. 0-1.\n"
				"  'b' - Binary character permitted but not required.\n"
				"  '>' - All following alphabetic characters are uppercased.\n"
				"  '<' - All following alphabetic characters are lowercased.\n"
				"  '!' - Switch off case conversion.\n"
				"  '\\' - Use to escape the special characters listed above to use them as separators."));
			    break;
			case FL_COMBO:
			    el->setAttr("help",_("List of values the editable combobox by lines."));
			    break;
			case FL_INTEGER:
			    el->setAttr("help",_("Integer value configuration in form: \"[Min]:[Max]:[ChangeStep]:[Prefix]:[Suffix]\"."));
			    break;
			case FL_REAL:
			    el->setAttr("help",_("Real value configuration in form: \"[Min]:[Max]:[ChangeStep]:[Prefix]:[Suffix]:[SignsAfterDot]\"."));
			    break;
			case FL_TIME: case FL_DATE: case FL_DATE_TM:
			    el->setAttr("help",_("Enter date and/or time by template with the items:\n"
				"  \"d\" - number of the day (1-31);\n"
				"  \"dd\" - number of the day (01-31);\n"
				"  \"ddd\" - acronym of the day ('Mon' ... 'Sun');\n"
				"  \"dddd\" - the full name of the day ('Monday' ... 'Sunday');\n"
				"  \"M\" - number of the month (1-12);\n"
				"  \"MM\" - number of the month (01-12);\n"
				"  \"MMM\" - acronym of the month ('Jan' ... 'Dec');\n"
				"  \"MMMM\" - the full name of the month ('January' ... 'December');\n"
				"  \"yy\" - last two digits of the year;\n"
				"  \"yyyy\" - full year;\n"
				"  \"h\" - hour (0-23);\n"
				"  \"hh\" - hour (00-23);\n"
				"  \"m\" - minutes (0-59);\n"
				"  \"mm\" - minutes (00-59);\n"
				"  \"s\" - seconds (0-59);\n"
				"  \"ss\" - seconds (00-59);\n"
				"  \"AP,ap\" - to display AM/PM or am/pm."));
			    break;
		    }
		    break;
		case F_TEXT_ED: case F_CHECK_BOX:
		    if((el=ctrId(root,"/font",true)))	el->setAttr("help",Widget::helpFont());
		    break;
		case F_BUTTON:
		    if((el=ctrId(root,"/img",true)))	el->setAttr("help",Widget::helpImg());
		    if((el=ctrId(root,"/color",true)))	el->setAttr("help",Widget::helpColor());
		    if((el=ctrId(root,"/colorText",true))) el->setAttr("help",Widget::helpColor());
		    if((el=ctrId(root,"/font",true)))	el->setAttr("help",Widget::helpFont());
		    if((el=ctrId(root,"/mode",true)))	el->setAttr("help",
			_("The button operation mode:\n"
			  "  \"Standard\" - normal button which allow events repeat on it hold (the parameters into \"value\");\n"
			  "  \"Checkable\" - fixed button (values into \"value\");\n"
			  "  \"Menu\" - open menu on press (items list into \"value\");\n"
			  "  \"Load\" - provides user-space small files loading through the visual interface;\n"
			  "             on the mode the button press will open selection file dialog for loading and\n"
			  "             the file content next saving to attribute \"value\";\n"
			  "  \"Save\" - provides user-space small files saving through the visual interface;\n"
			  "             on the file content writing to \"value\" attribute for user will open selection/set file dialog\n"
			  "             and next saving the attribute \"value\" content to the file and next the attribute \"value\" will set clean."));
		    if((el=ctrId(root,"/value",true)))
			switch(src->attrAt("mode").at().getI())
			{
			    case FBT_STD:  el->setAttr("help", _("Repeating parameters of events on holding \"{delay}-{interval}\", time in milliseconds."));	break;
			    case FBT_CHECK:el->setAttr("help", _("Toggle value."));	break;
			    case FBT_MENU: el->setAttr("help", _("Addresses of menu elements list like \"/grp1/grp2/item1\"."));		break;
			    case FBT_LOAD: el->setAttr("help", _("Description line \"{FilesTemplate}|{Header}|{FileByDefault}\" and loaded file content.\n"
			                                         "Files template like \"Images (*.png *.xpm *.jpg);;CSV-file (*.csv)\"."));	break;
			    case FBT_SAVE: el->setAttr("help", _("Description line \"{FilesTemplate}|{Header}|{FileByDefault}\" and saved file content.\n"
			                                         "Files template like \"Images (*.png *.xpm *.jpg);;CSV-file (*.csv)\"."));	break;
			}
		    break;
		case F_COMBO: case F_LIST: case F_TREE:
		    if((el=ctrId(root,"/items",true)))	el->setAttr("help",
			(elTp==F_TREE)?_("List of hierarchical items in path \"/{DIR}/{DIR}/{ITEM}\" by lines."):_("List of items-values by lines."));
		    if((el=ctrId(root,"/font",true)))	el->setAttr("help",Widget::helpFont());
		    break;
		case F_SLIDER: case F_SCROLL_BAR:
		    if((el=ctrId(root,"/cfg",true))) el->setAttr("help",
			_("Configuration of the slider in the format: \"[VertOrient]:[Min]:[Max]:[SinglStep]:[PageStep]\".\n"
			  "Where:\n"
			  "  \"VertOrient\" - sign of a vertical orientation, the default is the horizontal orientation;\n"
			  "  \"Min\" - minimum value;\n"
			  "  \"Max\" - maximum value;\n"
			  "  \"SinglStep\" - the size of a single step;\n"
			  "  \"PageStep\" - the size of the page step."));
		    break;
		case F_TABLE:
		    if((el=ctrId(root,"/items",true)))  el->setAttr("SnthHgl","1")->setAttr("help",
			_("XML tag \"tbl\" for the table fill:\n"
			  "<tbl>\n<h><s>Col1</s><s>Col2</s></h>\n<r><s>Col1ValS</s><i>Col2ValI</i></r>\n</table>"));
		    if((el=ctrId(root,"/font",true)))	el->setAttr("help",Widget::helpFont());
		    break;
	    }
	return true;
    }

    //> Process command to page
    if(ctrChkNode(opt,"SnthHgl",RWRWR_,"root",SUI_ID,SEC_RD))
    {
	opt->childAdd("blk")->setAttr("beg","<!--")->setAttr("end","-->")->setAttr("color","gray")->setAttr("font_italic","1");
	opt->childAdd("blk")->setAttr("beg","<\\?")->setAttr("end","\\?>")->setAttr("color","#666666");
	XMLNode *tag = opt->childAdd("blk")->setAttr("beg","<\\w+")->setAttr("end","\\/?>")->setAttr("font_weight","1");
	    tag->childAdd("rule")->setAttr("expr","\\b\\w+[ ]*(?==)")->setAttr("color","blue");
	    tag->childAdd("rule")->setAttr("expr","[ ]?\"[^\"]+\"")->setAttr("color","darkgreen");
	    tag->childAdd("rule")->setAttr("expr","[ ]?'[^']+'")->setAttr("color","darkgreen");
	opt->childAdd("rule")->setAttr("expr","<\\/[\\w]+>")->setAttr("font_weight","1");
	opt->childAdd("rule")->setAttr("expr","&([a-zA-Z]*|#\\d*);")->setAttr("color","#AF7E00");
    }
    else return Widget::cntrCmdAttributes(opt, src);

    return true;
}

//************************************************
//* OrigText: Text element original widget       *
//************************************************
OrigText::OrigText( ) : PrWidget("Text")	{ }

string OrigText::name( )	{ return _("Text fields"); }

string OrigText::descr( )	{ return _("Text fields widget of the finite visualization."); }

void OrigText::postEnable( int flag )
{
    LWidget::postEnable(flag);

    if(flag&TCntrNode::NodeConnect)
    {
	attrAdd(new TFld("backColor",_("Background:color"),TFld::String,Attr::Color,"","","","",i2s(A_BackColor).c_str()));
	attrAdd(new TFld("backImg",_("Background:image"),TFld::String,Attr::Image,"","","","",i2s(A_BackImg).c_str()));
	attrAdd(new TFld("bordWidth",_("Border:width"),TFld::Integer,TFld::NoFlag,"","0","","",i2s(A_BordWidth).c_str()));
	attrAdd(new TFld("bordColor",_("Border:color"),TFld::String,Attr::Color,"","#000000","","",i2s(A_BordColor).c_str()));
	attrAdd(new TFld("bordStyle",_("Border:style"),TFld::Integer,TFld::Selected,"","3",
	    TSYS::strMess("%d;%d;%d;%d;%d;%d;%d;%d;%d",FBRD_NONE,FBRD_DOT,FBRD_DASH,FBRD_SOL,FBRD_DBL,FBRD_GROOVE,FBRD_RIDGE,FBRD_INSET,FBRD_OUTSET).c_str(),
	    _("None;Dotted;Dashed;Solid;Double;Groove;Ridge;Inset;Outset"),i2s(A_BordStyle).c_str()));
	attrAdd(new TFld("font",_("Font"),TFld::String,Attr::Font,"50","Arial 11","","",i2s(A_TextFont).c_str()));
	attrAdd(new TFld("color",_("Color"),TFld::String,Attr::Color,"20","#000000","","",i2s(A_TextColor).c_str()));
	attrAdd(new TFld("orient",_("Orientation angle"),TFld::Integer,TFld::NoFlag,"3","0","-360;360","",i2s(A_TextOrient).c_str()));
	attrAdd(new TFld("wordWrap",_("Word wrap"),TFld::Boolean,TFld::NoFlag,"1","1","","",i2s(A_TextWordWrap).c_str()));
	attrAdd(new TFld("alignment",_("Alignment"),TFld::Integer,TFld::Selected,"1","0",
	    TSYS::strMess("%d;%d;%d;%d;%d;%d;%d;%d;%d;%d;%d;%d",FT_TOP_LEFT,FT_TOP_RIGHT,FT_TOP_CENTER,FT_TOP_JUST,
		FT_BT_LEFT,FT_BT_RIGHT,FT_BT_CENTER,FT_BT_JUST,FT_CNTR_LEFT,FT_CNTR_RIGHT,FT_CNTR,FT_CNTR_JUST).c_str(),
			    _("Top left;Top right;Top center;Top justify;"
			    "Bottom left;Bottom right;Bottom center;Bottom justify;"
			    "V center left;V center right;Center;V center justify"),i2s(A_TextAlignment).c_str()));
	attrAdd(new TFld("text",_("Text"),TFld::String,TFld::FullText,"0","Text","","",i2s(A_TextText).c_str()));
	attrAdd(new TFld("numbArg",_("Arguments number"),TFld::Integer,Attr::Active,"","0","0;20","",i2s(A_TextNumbArg).c_str()));
    }
}

bool OrigText::attrChange( Attr &cfg, TVariant prev )
{
    if(cfg.flgGlob()&Attr::Active)
    {
	int aid = atoi(cfg.fld().reserve().c_str());
	if(cfg.id() == "numbArg")
	{
	    string fid("arg"), fnm(_("Argument ")), fidp, fnmp;
	    //Delete specific unnecessary attributes of parameters
	    for(int i_p = 0; true; i_p++)
	    {
		fidp = fid+i2s(i_p);
		if(!cfg.owner()->attrPresent(fidp+"val")) break;
		else if(i_p >= cfg.getI())
		{
		    cfg.owner()->attrDel(fidp+"val");
		    cfg.owner()->attrDel(fidp+"tp");
		    cfg.owner()->attrDel(fidp+"cfg");
		}
	    }
	    //Create ullage attributes of parameters
	    for(int i_p = 0; i_p < cfg.getI(); i_p++)
	    {
		fidp = fid+i2s(i_p);
		fnmp = fnm+i2s(i_p);
		if(cfg.owner()->attrPresent(fidp+"val")) continue;
		cfg.owner()->attrAdd(new TFld((fidp+"tp").c_str(),(fnmp+_(":type")).c_str(),
		    TFld::Real,TFld::Selected|Attr::Mutable|Attr::Active,"","0",
			TSYS::strMess("%d;%d;%d",FT_INT,FT_REAL,FT_STR).c_str(),_("Integer;Real;String"),
			i2s(A_TextArs+A_TextArsTp+A_TextArsSz*i_p).c_str()));
		cfg.owner()->attrAdd(new TFld((fidp+"val").c_str(),(fnmp+_(":value")).c_str(),
		    TFld::Integer,Attr::Mutable,"","","","",i2s(A_TextArs+A_TextArsVal+A_TextArsSz*i_p).c_str()));
		cfg.owner()->attrAdd(new TFld((fidp+"cfg").c_str(),(fnmp+_(":config")).c_str(),
		    TFld::String,Attr::Mutable,"","","","",i2s(A_TextArs+A_TextArsCfg+A_TextArsSz*i_p).c_str()));
	    }
	}
	else if(aid >= A_TextArs && (aid%A_TextArsSz) == A_TextArsTp && prev.getI() != cfg.getI())
	{
	    int narg = (aid-A_TextArs)/A_TextArsSz;
	    string fid = "arg"+i2s(narg)+"val";
	    string fnm = _("Argument ")+i2s(narg)+_(":value");
	    int apos = cfg.owner()->attrPos(fid);
	    VCA::Attr::SelfAttrFlgs sflg =  cfg.owner()->attrAt(fid).at().flgSelf();
	    cfg.owner()->attrDel(fid);
	    cfg.owner()->attrAdd(new TFld(fid.c_str(),fnm.c_str(),
			(cfg.getI()==1) ? TFld::Real : ((cfg.getI()==2) ? TFld::String : TFld::Integer),
			Attr::Mutable,"","","","",i2s(A_TextArs+A_TextArsSz*narg).c_str()), apos);
	    cfg.owner()->attrAt(fid).at().setFlgSelf(sflg);
	}
    }
    return Widget::attrChange(cfg,prev);
}

bool OrigText::cntrCmdAttributes( XMLNode *opt, Widget *src )
{
    if(!src) src = this;

    //Get page info
    if(opt->name() == "info")
    {
	Widget::cntrCmdAttributes(opt, src);
	XMLNode *root, *el;
	if((root=ctrMkNode("area",opt,-1,"/attr",_("Attributes"))))
	{
	    for(unsigned i_ch = 0; i_ch < root->childSize(); i_ch++)
	    {
		el = root->childGet(i_ch);
		int p = atoi(el->attr("p").c_str());
		switch(p)
		{
		    case A_BackColor: case A_BordColor: case A_TextColor: el->setAttr("help",Widget::helpColor());	break;
		    case A_TextFont: el->setAttr("help",Widget::helpFont());	break;
		    case A_BackImg:  el->setAttr("help",Widget::helpImg());	break;
		    case A_TextText: el->setAttr("help",_("Text value. Use \"%{n}\" for argument {n} (from 1) value insert.")); break;
		}
	    }
	    for(int i_arg = 0; i_arg < src->attrAt("numbArg").at().getI(); i_arg++)
	    {
		if(!(el=ctrId(root,"/arg"+i2s(i_arg)+"cfg",true))) continue;
		switch(src->attrAt("arg"+i2s(i_arg)+"tp").at().getI())
		{
		    case A_TextArsVal:	el->setAttr("help",_("Integer value configuration in form \"[valLen]\"."));	break;
		    case A_TextArsTp:	el->setAttr("help",_("Real value configuration in form: \"[width];[form];[prec]\".\n"
							     "Where \"form\" that 'g', 'e' or 'f'."));			break;
		    case A_TextArsCfg:	el->setAttr("help",_("String value configuration in form \"[strLen]\"."));	break;
		}
	    }
	}
	return true;
    }

    //Process command to page
    return Widget::cntrCmdAttributes(opt, src);
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
    return _("Media widget of the finite visualization.");
}

void OrigMedia::postEnable( int flag )
{
    LWidget::postEnable(flag);

    if(flag&TCntrNode::NodeConnect)
    {
	attrAdd(new TFld("backColor",_("Background:color"),TFld::String,Attr::Color,"","#FFFFFF","","",i2s(A_BackColor).c_str()));
	attrAdd(new TFld("backImg",_("Background:image"),TFld::String,Attr::Image,"","","","",i2s(A_BackImg).c_str()));
	attrAdd(new TFld("bordWidth",_("Border:width"),TFld::Integer,TFld::NoFlag,"","0","","",i2s(A_BordWidth).c_str()));
	attrAdd(new TFld("bordColor",_("Border:color"),TFld::String,Attr::Color,"","#000000","","",i2s(A_BordColor).c_str()));
	attrAdd(new TFld("bordStyle",_("Border:style"),TFld::Integer,TFld::Selected,"","3",
	    TSYS::strMess("%d;%d;%d;%d;%d;%d;%d;%d;%d",FBRD_NONE,FBRD_DOT,FBRD_DASH,FBRD_SOL,FBRD_DBL,FBRD_GROOVE,FBRD_RIDGE,FBRD_INSET,FBRD_OUTSET).c_str(),
	    _("None;Dotted;Dashed;Solid;Double;Groove;Ridge;Inset;Outset"),i2s(A_BordStyle).c_str()));
	attrAdd(new TFld("src",_("Source"),TFld::String,TFld::NoFlag,"50","","","",i2s(A_MediaSrc).c_str()));
	attrAdd(new TFld("type",_("Type"),TFld::Integer,TFld::Selected|Attr::Active,"1","0",
	    TSYS::strMess("%d;%d;%d",FM_IMG,FM_ANIM,FM_FULL_VIDEO).c_str(),_("Image;Animation;Full video"),i2s(A_MediaType).c_str()));
	attrAdd(new TFld("areas",_("Map areas"),TFld::Integer,Attr::Active,"2","0","0;100","",i2s(A_MediaAreas).c_str()));
    }
}

bool OrigMedia::attrChange( Attr &cfg, TVariant prev )
{
    if(cfg.flgGlob()&Attr::Active)
    {
	if(cfg.id() == "type")
	{
	    //> Delete specific attributes
	    switch(prev.getI())
	    {
		case FM_IMG:
		    cfg.owner()->attrDel("fit");
		    break;
		case FM_ANIM:
		    cfg.owner()->attrDel("fit");
		    cfg.owner()->attrDel("speed");
		    break;
		case FM_FULL_VIDEO:
		    cfg.owner()->attrDel("play");
		    cfg.owner()->attrDel("roll");
		    cfg.owner()->attrDel("pause");
		    cfg.owner()->attrDel("size");
		    cfg.owner()->attrDel("seek");
		    cfg.owner()->attrDel("volume");
		    break;
	    }

	    //> Create specific attributes
	    switch(cfg.getI())
	    {
		case FM_IMG:
		    cfg.owner()->attrAdd(new TFld("fit",_("Fit to widget size"),TFld::Boolean,Attr::Mutable,"","","","",i2s(A_MediaFit).c_str()));
		    break;
		case FM_ANIM:
		    cfg.owner()->attrAdd(new TFld("fit",_("Fit to widget size"),TFld::Boolean,Attr::Mutable,"","","","",i2s(A_MediaFit).c_str()));
		    cfg.owner()->attrAdd(new TFld("speed",_("Play speed"),TFld::Integer,Attr::Mutable,"","100","1;900","",i2s(A_MediaSpeedPlay).c_str()));
		    break;
		case FM_FULL_VIDEO:
		    cfg.owner()->attrAdd(new TFld("play",_("Play"),TFld::Boolean,Attr::Mutable,"","0","","",i2s(A_MediaSpeedPlay).c_str()));
		    cfg.owner()->attrAdd(new TFld("roll",_("Roll play"),TFld::Boolean,Attr::Mutable,"","0","","",i2s(A_MediaRoll).c_str()));
		    cfg.owner()->attrAdd(new TFld("pause",_("Pause"),TFld::Boolean,Attr::Mutable,"","0","","",i2s(A_MediaPause).c_str()));
		    cfg.owner()->attrAdd(new TFld("size",_("Size"),TFld::Real,Attr::Mutable,"","0","0;1e300","",i2s(A_MediaSize).c_str()));
		    cfg.owner()->attrAdd(new TFld("seek",_("Seek"),TFld::Real,Attr::Mutable,"","0","0;1e300","",i2s(A_MediaSeek).c_str()));
		    cfg.owner()->attrAdd(new TFld("volume",_("Volume"),TFld::Real,Attr::Mutable,"","50","0;100","",i2s(A_MediaVolume).c_str()));
		    break;
	    }
	}
	else if(cfg.id() == "areas")
	{
	    string fid("area"), fnm(_("Area ")), fidp, fnmp;
	    //> Delete specific unnecessary attributes of map areas
	    for(int i_p = 0; true; i_p++)
	    {
		fidp = fid+i2s(i_p);
		if(!cfg.owner()->attrPresent(fidp+"shp"))	break;
		else if(i_p >= cfg.getI())
		{
		    cfg.owner()->attrDel(fidp+"shp");
		    cfg.owner()->attrDel(fidp+"coord");
		    cfg.owner()->attrDel(fidp+"title");
		}
	    }
	    //> Create ullage attributes of map areas
	    for(int i_p = 0; i_p < cfg.getI(); i_p++)
	    {
		fidp = fid+i2s(i_p);
		fnmp = fnm+i2s(i_p);
		if(cfg.owner()->attrPresent(fidp+"shp")) continue;
		cfg.owner()->attrAdd(new TFld((fidp+"shp").c_str(),(fnmp+_(":shape")).c_str(),TFld::Integer,TFld::Selected|Attr::Mutable,"1","0",
		    TSYS::strMess("%d;%d;%d",FM_RECT,FM_POLY,FM_CIRCLE).c_str(),_("Rect;Poly;Circle"),i2s(A_MediaArs+A_MediaArShape+A_MediaArsSz*i_p).c_str()));
		cfg.owner()->attrAdd(new TFld((fidp+"coord").c_str(),(fnmp+_(":coordinates")).c_str(),TFld::String,Attr::Mutable,"","",
		    "","",i2s(A_MediaArs+A_MediaArCoord+A_MediaArsSz*i_p).c_str()));
		cfg.owner()->attrAdd(new TFld((fidp+"title").c_str(),(fnmp+_(":title")).c_str(),TFld::String,Attr::Mutable,"","",
		    "","",i2s(A_MediaArs+A_MediaArTitle+A_MediaArsSz*i_p).c_str()));
	    }
	}
    }
    return Widget::attrChange(cfg,prev);
}

bool OrigMedia::cntrCmdAttributes( XMLNode *opt, Widget *src )
{
    if(!src) src = this;
    //> Get page info
    if(opt->name() == "info")
    {
	Widget::cntrCmdAttributes(opt,src);
	XMLNode *root, *el;
	if((root=ctrMkNode("area",opt,-1,"/attr",_("Attributes"))))
	{
	    for(unsigned i_ch = 0; i_ch < root->childSize(); i_ch++)
            {
                el = root->childGet(i_ch);
                int p = atoi(el->attr("p").c_str());
                switch(p)
                {
		    case A_BackColor: case A_BordColor: el->setAttr("help",Widget::helpColor());	break;
		    case A_BackImg: el->setAttr("help",Widget::helpImg());		break;
		    case A_MediaSrc: el->setAttr("dest","sel_ed")->setAttr("select","/attrImg/sel_"+el->attr("id"))->setAttr("help",
			_("Media source name in form \"[src:]name\", where:\n"
        		"  \"src\" - source:\n"
        		"    file - direct from local (visualizator or engine) file by path;\n"
        		"    res - from DB mime resources table;\n"
        		"    stream - Stream URL for video and audio play.\n"
        		"  \"name\" - file path or resource mime Id.\n"
        		"Examples:\n"
        		"  \"res:workMedia\" - from DB mime resources table for Id \"workMedia\";\n"
        		"  \"workMedia\" - like previous;\n"
        		"  \"file:/var/tmp/workMedia.mng\" - from local file by path \"/var/tmp/workMedia.mng\";\n"
        		"  \"stream:http://localhost.localhost:5050\" - video and audio stream play from URL."));
        		break;
        	    case A_MediaType: el->setAttr("help",
        		_("Media type variant:\n"
        		"  \"Image\" - raster or vector(can not support) image, like: PNG, JPEG, GIF;\n"
        		"  \"Animation\" - simple animation image, like: GIF, MNG;\n"
        		"  \"Full video\" - full video, audio or stream, like: OGG, OGM, AVI, MKV, MPG, MP3, MP4."));
        		break;
        	}
    	    }
	    for(int i_a = 0; i_a < src->attrAt("areas").at().getI(); i_a++)
	    {
		el = ctrId(root,TSYS::strMess("/area%dcoord",i_a),true);
		if(!el) continue;
		switch(src->attrAt(TSYS::strMess("area%dshp",i_a)).at().getI())
		{
		    case FM_RECT: el->setAttr("help",_("Rectangle area in form \"x1,y1,x2,y2\"."));	break;
		    case FM_POLY: el->setAttr("help",_("Polygon area in form \"x1,y1,x2,y2,xN,yN\"."));	break;
		    case FM_CIRCLE: el->setAttr("help",_("Circle area in form \"x,y,r\"."));		break;
		}
	    }
	}
	return true;
    }

    //> Process command to page
    return Widget::cntrCmdAttributes(opt,src);
}

//************************************************
//* OrigDiagram: Diagram original widget         *
//************************************************
OrigDiagram::OrigDiagram( ) : PrWidget("Diagram")	{ }

string OrigDiagram::name( )	{ return _("Diagram"); }

string OrigDiagram::descr( )	{ return _("Diagram widget of the finite visualization."); }

void OrigDiagram::postEnable( int flag )
{
    LWidget::postEnable(flag);

    if(flag&TCntrNode::NodeConnect)
    {
	attrAdd(new TFld("backColor",_("Background:color"),TFld::String,Attr::Color,"","black","","",i2s(A_BackColor).c_str()));
	attrAdd(new TFld("backImg",_("Background:image"),TFld::String,Attr::Image,"","","","",i2s(A_BackImg).c_str()));
	attrAdd(new TFld("bordWidth",_("Border:width"),TFld::Integer,TFld::NoFlag,"","0","","",i2s(A_BordWidth).c_str()));
	attrAdd(new TFld("bordColor",_("Border:color"),TFld::String,Attr::Color,"","#000000","","",i2s(A_BordColor).c_str()));
	attrAdd(new TFld("bordStyle",_("Border:style"),TFld::Integer,TFld::Selected,"","3",
	    TSYS::strMess("%d;%d;%d;%d;%d;%d;%d;%d;%d",FBRD_NONE,FBRD_DOT,FBRD_DASH,FBRD_SOL,FBRD_DBL,FBRD_GROOVE,FBRD_RIDGE,FBRD_INSET,FBRD_OUTSET).c_str(),
	    _("None;Dotted;Dashed;Solid;Double;Groove;Ridge;Inset;Outset"),i2s(A_BordStyle).c_str()));
	attrAdd(new TFld("trcPer",_("Tracing period (s)"),TFld::Integer,TFld::NoFlag,"","0","0;360","",i2s(A_DiagramTrcPer).c_str()));
	attrAdd(new TFld("type",_("Type"),TFld::Integer,TFld::Selected|Attr::Active,"1","0",
	    TSYS::strMess("%d;%d",FD_TRND,FD_SPECTR).c_str(),_("Trend;Spectrum"),i2s(A_DiagramType).c_str()));
    }
}

bool OrigDiagram::attrChange( Attr &cfg, TVariant prev )
{
    if(!(cfg.flgGlob()&Attr::Active))	return Widget::attrChange(cfg, prev);

    if(cfg.id() == "active" && cfg.getB() != prev.getB())
    {
	if(!cfg.getB())
	{
	    cfg.owner()->attrDel("curSek");
	    cfg.owner()->attrDel("curUSek");
	    cfg.owner()->attrDel("curColor");
	}
	else
	{
	    cfg.owner()->attrAdd(new TFld("curSek",_("Cursor:sek"),TFld::Integer,Attr::DateTime|Attr::Mutable,"","","","","30"));
	    cfg.owner()->attrAdd(new TFld("curUSek",_("Cursor:usek"),TFld::Integer,Attr::Mutable,"","","","","31"));
	    cfg.owner()->attrAdd(new TFld("curColor",_("Cursor:color"),TFld::String,Attr::Color|Attr::Mutable,"","white","","","32"));
	}
    }
    else if(cfg.id() == "type")
    {
	//> Delete specific attributes
	switch(prev.getI())
	{
	    case FD_TRND:
		if(cfg.getI() != prev.getI())	cfg.owner()->attrDel("sclHorPer");
	    case FD_SPECTR:
		if(cfg.getI() == 0 || cfg.getI() == 1) break;
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
		cfg.owner()->attrDel("valsForPix");
		cfg.owner()->attrDel("parNum");
		break;
	}

	//> Create specific attributes
	switch(cfg.getI())
	{
	    case FD_TRND:
		cfg.owner()->attrAdd(new TFld("sclHorPer",_("Scale:horizontal grid size, sek"),TFld::Real,Attr::Mutable,
		    "","0","0;3e6","",i2s(A_DiagramSclHorPer).c_str()));
	    case FD_SPECTR:
		cfg.owner()->attrAdd(new TFld("tSek",_("Time:sek"),TFld::Integer,Attr::DateTime|Attr::Mutable,"","","","",i2s(A_DiagramTSek).c_str()));
		cfg.owner()->attrAdd(new TFld("tUSek",_("Time:usek"),TFld::Integer,Attr::Mutable,"","","","",i2s(A_DiagramTUSek).c_str()));
		cfg.owner()->attrAdd(new TFld("tSize",_("Size, sek"),TFld::Real,Attr::Mutable,"","60","0;3e6","",i2s(A_DiagramTSize).c_str()));
		if(cfg.owner()->attrAt("active").at().getB())
		{
		    cfg.owner()->attrAdd(new TFld("curSek",_("Cursor:sek"),TFld::Integer,Attr::DateTime|Attr::Mutable,"","","","",i2s(A_DiagramCurSek).c_str()));
		    cfg.owner()->attrAdd(new TFld("curUSek",_("Cursor:usek"),TFld::Integer,Attr::Mutable,"","","","",i2s(A_DiagramCurUSek).c_str()));
		    cfg.owner()->attrAdd(new TFld("curColor",_("Cursor:color"),TFld::String,Attr::Color|Attr::Mutable,"","white","","",i2s(A_DiagramCurColor).c_str()));
		}
		cfg.owner()->attrAdd(new TFld("sclColor",_("Scale:color"),TFld::String,Attr::Color|Attr::Mutable,"","grey","","",i2s(A_DiagramSclColor).c_str()));
		cfg.owner()->attrAdd(new TFld("sclHor",_("Scale:horizontal"),TFld::Integer,Attr::Mutable|TFld::Selected,"1",i2s(FD_NO).c_str(),
		    TSYS::strMess("%d;%d;%d;%d",FD_NO,FD_GRD,FD_MARKS,FD_GRD_MARKS).c_str(),
		    _("No draw;Grid;Markers;Grid and markers"),i2s(A_DiagramSclHor).c_str()));
		cfg.owner()->attrAdd(new TFld("sclVer",_("Scale:vertical"),TFld::Integer,Attr::Mutable|TFld::Selected,"1",i2s(FD_NO).c_str(),
		    TSYS::strMess("%d;%d;%d;%d;%d;%d;%d",FD_NO,FD_GRD,FD_MARKS,FD_GRD_MARKS,FD_GRD_LOG,FD_MARKS_LOG,FD_GRD_MARKS_LOG).c_str(),
		    _("No draw;Grid;Markers;Grid and markers;Grid (log);Markers (log);Grid and markers (log)"),i2s(A_DiagramSclVer).c_str()));
		cfg.owner()->attrAdd(new TFld("sclVerScl",_("Scale:vertical scale (%)"),TFld::Real,Attr::Mutable,"","100","10;1000","",i2s(A_DiagramSclVerScl).c_str()));
		cfg.owner()->attrAdd(new TFld("sclVerSclOff",_("Scale:vertical scale offset (%)"),TFld::Real,Attr::Mutable,"","0","-100;100","",i2s(A_DiagramSclVerSclOff).c_str()));
		cfg.owner()->attrAdd(new TFld("sclMarkColor",_("Scale:Markers:color"),TFld::String,Attr::Color|Attr::Mutable,"","white","","",i2s(A_DiagramSclMarkColor).c_str()));
		cfg.owner()->attrAdd(new TFld("sclMarkFont",_("Scale:Markers:font"),TFld::String,Attr::Font|Attr::Mutable,"","Arial 10","","",i2s(A_DiagramSclMarkFont).c_str()));
		cfg.owner()->attrAdd(new TFld("valArch",_("Value archivator"),TFld::String,Attr::Mutable,"","","","",i2s(A_DiagramValArch).c_str()));
		cfg.owner()->attrAdd(new TFld("valsForPix",_("Values for pixel"),TFld::Integer,Attr::Mutable,"","1","1;100","",i2s(A_DiagramValsForPix).c_str()));
		cfg.owner()->attrAdd(new TFld("parNum",_("Parameters number"),TFld::Integer,Attr::Mutable|Attr::Active,"","1","0;100","",i2s(A_DiagramParNum).c_str()));
		break;
	}
    }
    else if(cfg.id() == "parNum")
    {
	string fid("prm"), fnm(_("Parameter ")), fidp, fnmp;
	//> Delete specific unnecessary attributes of parameters
	for(int i_p = 0; true; i_p++)
	{
	    fidp = fid+i2s(i_p);
	    if(!cfg.owner()->attrPresent(fidp+"addr"))	break;
	    else if(i_p >= cfg.getI())
	    {
		cfg.owner()->attrDel(fidp+"addr");
		cfg.owner()->attrDel(fidp+"bordL");
		cfg.owner()->attrDel(fidp+"bordU");
		cfg.owner()->attrDel(fidp+"aScale");
		cfg.owner()->attrDel(fidp+"color");
		cfg.owner()->attrDel(fidp+"width");
		cfg.owner()->attrDel(fidp+"scl");
		cfg.owner()->attrDel(fidp+"val");
		cfg.owner()->attrDel(fidp+"prop");
	    }
	}
	//> Create ullage attributes of parameters
	for(int i_p = 0; i_p < cfg.getI(); i_p++)
	{
	    fidp = fid+i2s(i_p);
	    fnmp = fnm+i2s(i_p);
	    if(cfg.owner()->attrPresent(fidp+"addr")) continue;
	    cfg.owner()->attrAdd(new TFld((fidp+"addr").c_str(),(fnmp+_(":address")).c_str(),
		TFld::String,Attr::Address|Attr::Mutable,"","","","",i2s(A_DiagramTrs+A_DiagramTrAddr+A_DiagramTrsSz*i_p).c_str()));
	    cfg.owner()->attrAdd(new TFld((fidp+"bordL").c_str(),(fnmp+_(":view border:lower")).c_str(),
		TFld::Real,Attr::Mutable,"","","","",i2s(A_DiagramTrs+A_DiagramTrBordL+A_DiagramTrsSz*i_p).c_str()));
	    cfg.owner()->attrAdd(new TFld((fidp+"bordU").c_str(),(fnmp+_(":view border:upper")).c_str(),
		TFld::Real,Attr::Mutable,"","","","",i2s(A_DiagramTrs+A_DiagramTrBordU+A_DiagramTrsSz*i_p).c_str()));
	    cfg.owner()->attrAdd(new TFld((fidp+"color").c_str(),(fnmp+_(":color")).c_str(),
		TFld::String,Attr::Color|Attr::Mutable,"","","","",i2s(A_DiagramTrs+A_DiagramTrClr+A_DiagramTrsSz*i_p).c_str()));
	    cfg.owner()->attrAdd(new TFld((fidp+"width").c_str(),(fnmp+_(":width")).c_str(),
		TFld::Integer,Attr::Mutable,"","1","1;10","",i2s(A_DiagramTrs+A_DiagramTrWdth+A_DiagramTrsSz*i_p).c_str()));
	    cfg.owner()->attrAdd(new TFld((fidp+"scl").c_str(),(fnmp+_(":scale")).c_str(),TFld::Integer,Attr::Mutable|TFld::Selected,
		"","0","0;2;3;6;7",_("Global;Markers;Grid and markers;Markers (log);Grid and markers (log)"),
		i2s(A_DiagramTrs+A_DiagramTrScl+A_DiagramTrsSz*i_p).c_str()));
	    cfg.owner()->attrAdd(new TFld((fidp+"val").c_str(),(fnmp+_(":value")).c_str(),
		TFld::Real,Attr::Mutable,"","","","",i2s(A_DiagramTrs+A_DiagramTrVal+A_DiagramTrsSz*i_p).c_str()));
	    cfg.owner()->attrAdd(new TFld((fidp+"prop").c_str(),(fnmp+_(":properties")).c_str(),
		TFld::String,Attr::Mutable,"","","","",i2s(A_DiagramTrs+A_DiagramTrProp+A_DiagramTrsSz*i_p).c_str()));
	}
    }

    return Widget::attrChange(cfg,prev);
}

bool OrigDiagram::cntrCmdAttributes( XMLNode *opt, Widget *src )
{
    if(!src) src = this;
    //> Get page info
    if(opt->name() == "info")
    {
	Widget::cntrCmdAttributes(opt,src);
	XMLNode *root, *el;
	if((root=ctrMkNode("area",opt,-1,"/attr",_("Attributes"))))
	{
	    for(unsigned i_ch = 0; i_ch < root->childSize(); i_ch++)
            {
                el = root->childGet(i_ch);
                int p = atoi(el->attr("p").c_str());
                switch(p)
                {
		    case A_BackColor: case A_BordColor: case A_DiagramSclColor: case A_DiagramSclMarkColor:
			el->setAttr("help",Widget::helpColor());	break;
		    case A_DiagramSclMarkFont: el->setAttr("help",Widget::helpFont());	break;
		    case A_BackImg: el->setAttr("help",Widget::helpImg());	break;
		    case A_DiagramValArch:
			el->setAttr("help",_("Value archivator in form \"ArchMod.ArchivatorId\"."));	break;
		    case A_DiagramValsForPix:
			el->setAttr("help",_("The number of values per pixel. Increase to enhance the accuracy of export at large time intervals."));
			break;
		}
	    }
	    for(int i_p = 0; i_p < src->attrAt("parNum").at().getI(); i_p++)
	    {
		if((el=ctrId(root,TSYS::strMess("/prm%dcolor",i_p),true))) el->setAttr("help",Widget::helpColor());
		if((el=ctrId(root,TSYS::strMess("/prm%daddr",i_p),true))) el->setAttr("help",
		    _("Full address to DAQ attribute of a parameter or to an archive.\n"
		      "Also support direct data set by prefixes:\n"
		      "  \"data:{XMLNodeData}\" - draw from direct set data;\n"
		      "  \"line:{value}\" - draw horizontal line by value.\n"
		      "Example:\n"
		      "  \"/DAQ/System/AutoDA/MemInfo/use\" - address to attribute \"use\" of parameter \"MemInfo\"\n"
		      "	    of controller \"AutoDA\" of DAQ module \"System\";\n"
		      "  \"/Archive/va_CPULoad_load\" - address to archive \"CPULoad_load\";\n"
		      "  \"data:<d tm=\"1369465209000000\" tm_grnd=\"1369465200000000\" per=\"1000000\">\n"
		      "    0 3.14\n"
		      "    1 3.141\n"
		      "    5 3.1415</d> - data for 10 seconds and period 1 second from \"25.05.2013 10:00:00\";\n"
		      "  \"line:3.14159265\" - horizontal line into value \"3.14159265\"."));
		if((el=ctrId(root,TSYS::strMess("/prm%dprop",i_p),true))) el->setAttr("help",
		    _("Real archive properties in form \"BegArh:EndArh:DataPeriod\", where:\n"
		      "  \"BegArh\", \"EndArh\", \"DataPeriod\" - begin, end and period archive's data in seconds,\n"
		      "                  real up to microseconds (1e-6)."));
	    }
	}
	return true;
    }

    //> Process command to page
    return Widget::cntrCmdAttributes(opt,src);
}

//************************************************
//* OrigProtocol: Protocol original widget       *
//************************************************
OrigProtocol::OrigProtocol( ) : PrWidget("Protocol")	{ }

string OrigProtocol::name( )	{ return _("Protocol"); }

string OrigProtocol::descr( )	{ return _("Protocol widget of the finite visualization."); }

void OrigProtocol::postEnable( int flag )
{
    LWidget::postEnable(flag);

    if(flag&TCntrNode::NodeConnect)
    {
	attrAdd(new TFld("backColor",_("Background:color"),TFld::String,Attr::Color,"","","","",i2s(A_BackColor).c_str()));
	attrAdd(new TFld("backImg",_("Background:image"),TFld::String,Attr::Image,"","","","",i2s(A_BackImg).c_str()));
	attrAdd(new TFld("font",_("Font"),TFld::String,Attr::Font,"","Arial 11","","",i2s(A_ProtFont).c_str()));
	attrAdd(new TFld("headVis",_("Header visible"),TFld::Boolean,TFld::NoFlag,"","1","","",i2s(A_ProtHeadVis).c_str()));
	attrAdd(new TFld("time",_("Time, sek"),TFld::Integer,Attr::DateTime,"","","","",i2s(A_ProtTime).c_str()));
	attrAdd(new TFld("tSize",_("Size, sek"),TFld::Integer,TFld::NoFlag,"","60","0;50000000","",i2s(A_ProtTSize).c_str()));
	attrAdd(new TFld("trcPer",_("Tracing period (s)"),TFld::Integer,TFld::NoFlag,"","0","0;360","",i2s(A_ProtTrcPer).c_str()));
	attrAdd(new TFld("arch",_("Archivator"),TFld::String,TFld::NoFlag,"","","","",i2s(A_ProtArch).c_str()));
	attrAdd(new TFld("tmpl",_("Template"),TFld::String,TFld::NoFlag,"","","","",i2s(A_ProtTmpl).c_str()));
	attrAdd(new TFld("lev",_("Level"),TFld::Integer,TFld::NoFlag,"","0","-7;7","",i2s(A_ProtLev).c_str()));
	attrAdd(new TFld("viewOrd",_("View order"),TFld::Integer,TFld::Selected,"","0",
	    TSYS::strMess("%d;%d;%d;%d;%d;%d;%d;%d",FP_ON_TM,FP_ON_LEV,FP_ON_CAT,FP_ON_MESS,FP_ON_TM_REV,FP_ON_LEV_REV,FP_ON_CAT_REV,FP_ON_MESS_REV).c_str(),
	    _("On time;On level;On category;On messages;On time (reverse);On level (reverse);On category (reverse);On messages (reverse)"),i2s(A_ProtViewOrd).c_str()));
	attrAdd(new TFld("col",_("View columns"),TFld::String,TFld::NoFlag,"","pos;tm;utm;lev;cat;mess","","",i2s(A_ProtCol).c_str()));
	attrAdd(new TFld("itProp",_("Item properties"),TFld::Integer,Attr::Active,"","","","",i2s(A_ProtItProp).c_str()));
    }
}

bool OrigProtocol::attrChange( Attr &cfg, TVariant prev )
{
    if(cfg.flgGlob()&Attr::Active)
    {
	if(cfg.id() == "itProp")
	{
	    string fid("it"), fnm(_("Item ")), fidp, fnmp;
	    //> Delete specific unnecessary items
	    for(int i_p = 0; true; i_p++)
	    {
		fidp = fid + i2s(i_p);
		if(!cfg.owner()->attrPresent(fidp+"lev")) break;
		else if(i_p >= cfg.getI())
		{
		    cfg.owner()->attrDel(fidp+"lev");
		    cfg.owner()->attrDel(fidp+"tmpl");
		    cfg.owner()->attrDel(fidp+"fnt");
		    cfg.owner()->attrDel(fidp+"color");
		}
	    }
	    //> Create ullage items
	    for(int i_p = 0; i_p < cfg.getI(); i_p++)
	    {
		fidp = fid + i2s(i_p);
		fnmp = fnm + i2s(i_p);
		if(cfg.owner()->attrPresent(fidp+"lev")) continue;
		cfg.owner()->attrAdd(new TFld((fidp+"lev").c_str(),(fnmp+_(":level")).c_str(),
		    TFld::Integer,Attr::Mutable,"","0","0;7","",i2s(A_ProtProps+A_ProtPropLev+A_ProtPropsSz*i_p).c_str()));
		cfg.owner()->attrAdd(new TFld((fidp+"tmpl").c_str(),(fnmp+_(":template")).c_str(),
		    TFld::String,Attr::Mutable,"","","","",i2s(A_ProtProps+A_ProtPropTmpl+A_ProtPropsSz*i_p).c_str()));
		cfg.owner()->attrAdd(new TFld((fidp+"fnt").c_str(),(fnmp+_(":font")).c_str(),
		    TFld::String,Attr::Font|Attr::Mutable,"","","","",i2s(A_ProtProps+A_ProtPropFnt+A_ProtPropsSz*i_p).c_str()));
		cfg.owner()->attrAdd( new TFld((fidp+"color").c_str(),(fnmp+_(":color")).c_str(),
		    TFld::String,Attr::Color|Attr::Mutable,"","","","",i2s(A_ProtProps+A_ProtPropClr+A_ProtPropsSz*i_p).c_str()));
	    }
	}
    }
    return Widget::attrChange(cfg, prev);
}

bool OrigProtocol::cntrCmdAttributes( XMLNode *opt, Widget *src )
{
    if(!src) src = this;
    //> Get page info
    if(opt->name() == "info")
    {
	Widget::cntrCmdAttributes(opt,src);
	XMLNode *root, *el;
	if((root=ctrMkNode("area",opt,-1,"/attr",_("Attributes"))))
	{
	    for(unsigned i_ch = 0; i_ch < root->childSize(); i_ch++)
	    {
		el = root->childGet(i_ch);
		int p = atoi(el->attr("p").c_str());
		switch(p)
		{
		    case A_BackColor: el->setAttr("help",Widget::helpColor());	break;
		    case A_BackImg: el->setAttr("help",Widget::helpImg());	break;
		    case A_ProtFont: el->setAttr("help",Widget::helpFont());	break;
		    case A_ProtArch: el->setAttr("help",_("Messages archivator in form \"ArchMod.ArchivatorId\"."));	break;
		    case A_ProtLev: el->setAttr("help",_("Set value to < 0 for get current alarms."));	break;
		    case A_ProtTSize: el->setAttr("help",_("Set value to '0' for get all alarms, for \"lev\" < 0."));	break;
		    case A_ProtTmpl: el->setAttr("help",
			_("Category template or regular expression \"/{re}/\". For template reserved special symbols:\n"
			"  '*' - any multiply symbols group;\n"
			"  '?' - any one symbol;\n"
			"  '\\' - use for shield special simbols."));
			break;
		    case A_ProtCol: el->setAttr("help",
			_("Visible and order columns list separated by symbol ';'. Supported columns:\n"
			"  \"pos\" - row number;\n"
			"  \"tm\" - date and time of the message;\n"
			"  \"utm\" - microseconds part of time of the message;\n"
			"  \"lev\" - level of the message;\n"
			"  \"cat\" - category of the message;\n"
			"  \"mess\" - the message text."));
			break;
		}
	    }
	    for(int i_p = 0; i_p < src->attrAt("itProp").at().getI(); i_p++)
	    {
		if((el=ctrId(root,TSYS::strMess("/it%dcolor",i_p),true))) el->setAttr("help",Widget::helpColor());
		if((el=ctrId(root,TSYS::strMess("/it%dfnt",i_p),true))) el->setAttr("help",Widget::helpFont());
		if((el=ctrId(root,TSYS::strMess("/it%dtmpl",i_p),true))) el->setAttr("help",
		    _("Category template or regular expression \"/{re}/\". For template reserved special symbols:\n"
		    "  '*' - any multiply symbols group;\n"
		    "  '?' - any one symbol;\n"
		    "  '\\' - use for shield special simbols."));
	    }
	}
	return true;
    }

    //> Process command to page
    return Widget::cntrCmdAttributes(opt,src);
}

//************************************************
//* OrigDocument: Document original widget       *
//************************************************
const char *OrigDocument::XHTML_entity =
    "<!DOCTYPE xhtml [\n"
    "  <!ENTITY nbsp \"&#160;\" >\n"
    "]>\n";

OrigDocument::OrigDocument( ) : PrWidget("Document")	{ }

string OrigDocument::name( )	{ return _("Document"); }

string OrigDocument::descr( )	{ return _("Document widget of the finite visualization."); }

void OrigDocument::postEnable( int flag )
{
    LWidget::postEnable(flag);

    if(flag&TCntrNode::NodeConnect)
    {
	attrAdd(new TFld("style",_("CSS"),TFld::String,TFld::FullText,"","","","",i2s(A_DocStyle).c_str()));
	attrAdd(new TFld("tmpl",_("Template"),TFld::String,TFld::FullText,"","","","",i2s(A_DocTmpl).c_str()));
	attrAdd(new TFld("doc",_("Document"),TFld::String,TFld::FullText,"","","","",i2s(A_DocDoc).c_str()));
	attrAdd(new TFld("font",_("Font"),TFld::String,Attr::Font,"","Arial 11","","",i2s(A_DocFont).c_str()));
	attrAdd(new TFld("bTime",_("Time:begin"),TFld::Integer,Attr::DateTime,"","0","","",i2s(A_DocBTime).c_str()));
	attrAdd(new TFld("time",_("Time:current"),TFld::Integer,Attr::DateTime|Attr::Active,"","0","","",i2s(A_DocTime).c_str()));
	attrAdd(new TFld("process",_("Process"),TFld::Boolean,TFld::NoWrite,"","0","","",i2s(A_DocProcess).c_str()));
	attrAdd(new TFld("n",_("Archive size"),TFld::Integer,Attr::Active,"","0",TSYS::strMess("0;%d",DocArhSize).c_str(),"",i2s(A_DocN).c_str()));
    }
}

void OrigDocument::disable( Widget *base )	{ SYS->taskDestroy(base->nodePath('.',true)+".doc"); }

bool OrigDocument::attrChange( Attr &cfg, TVariant prev )
{
    int off = 0;

    //Document's number change process
    if(cfg.id() == "n" && cfg.getI() != prev.getI())
    {
	if(!cfg.getI())
	{
	    cfg.owner()->attrDel("aCur");
	    cfg.owner()->attrDel("vCur");
	    cfg.owner()->attrDel("aDoc");
	    cfg.owner()->attrDel("aSize");
	}
	else
	{
	    if(!cfg.owner()->attrPresent("vCur"))
	    {
		cfg.owner()->attrAdd(new TFld("vCur",_("Archive:cursor:view"),TFld::Integer,Attr::Mutable|Attr::Active,"","0",
		    TSYS::strMess("-2;%d",DocArhSize-1).c_str()));
		cfg.owner()->inheritAttr("vCur");
	    }
	    if(!cfg.owner()->attrPresent("aCur"))
	    {
		cfg.owner()->attrAdd(new TFld("aCur",_("Archive:cursor:current"),TFld::Integer,Attr::Mutable|Attr::Active,"","0",
		    TSYS::strMess("-1;%d",DocArhSize-1).c_str()));
		cfg.owner()->inheritAttr("aCur");
	    }
	    if(!cfg.owner()->attrPresent("aDoc"))
	    {
		cfg.owner()->attrAdd(new TFld("aDoc",_("Archive:current document"),TFld::String,TFld::FullText|Attr::Mutable|Attr::Active));
		cfg.owner()->inheritAttr("aDoc");
	    }
	    if(!cfg.owner()->attrPresent("aSize"))
	    {
		cfg.owner()->attrAdd(new TFld("aSize",_("Archive:size"),TFld::Integer,Attr::Mutable));
		cfg.owner()->inheritAttr("aSize");
	    }
	}
    }

    SessWdg *sw = dynamic_cast<SessWdg*>(cfg.owner());
    if(!sw) return Widget::attrChange(cfg,prev);

    string db  = sw->ownerSess()->parent().at().DB();
    string tbl = sw->ownerSess()->parent().at().tbl()+"_ses";

    //Make document after time set
    if(cfg.id() == "time" && (cfg.getI() != prev.getI() || (!cfg.getI() && prev.getI())))
    {
	if(!cfg.getI() && prev.getI()) cfg.setI(prev.getI(),false,true);
	try
	{
	    string taskNm = sw->nodePath('.',true)+".doc";
	    SYS->taskDestroy(taskNm);
	    SYS->taskCreate(taskNm, -1, OrigDocument::DocTask, sw, 2);
	    sw->attrAt("process").at().setB(true);
	}catch(TError err) { }
    }
    //Load document's from project's DB
    else if(cfg.id() == "n" && cfg.getI() != prev.getI())
    {
	TConfig c_el(&mod->elPrjSes());
	TSYS::pathLev(sw->path(),0,true,&off);
	c_el.cfg("IDW").setS(sw->path().substr(off));
	// Archive position load
	c_el.cfg("ID").setS("aCur");
	if(SYS->db().at().dataGet(db+"."+tbl,mod->nodePath()+tbl,c_el,false,true))
	    cfg.owner()->attrAt("aCur").at().setI(c_el.cfg("IO_VAL").getI(),false,true);
	// Current archive socuments load
	c_el.cfg("ID").setS("doc"+i2s(cfg.owner()->attrAt("aCur").at().getI()));
	if(SYS->db().at().dataGet(db+"."+tbl,mod->nodePath()+tbl,c_el,false,true))
	    cfg.owner()->attrAt("aDoc").at().setS(c_el.cfg("IO_VAL").getS(),false,true);
	// Set current document
	cfg.owner()->attrAt("vCur").at().setI(cfg.owner()->attrAt("aCur").at().getI(),false,true);
	cfg.owner()->attrAt("doc").at().setS(cfg.owner()->attrAt("aDoc").at().getS(),false,true);
	// Parse current document and restore last document's time
	string cdoc = cfg.owner()->attrAt("doc").at().getS();
	if(!cdoc.empty())
	{
	    XMLNode xdoc;
	    try{ xdoc.load(XHTML_entity+cdoc, false, Mess->charset()); } catch(TError err) { }
	    cfg.owner()->attrAt("time").at().setS(xdoc.attr("docTime"),false,true);
	}
	sizeUpdate(sw);
    }
    //Move archive cursor
    else if(cfg.id() == "aCur" && cfg.getI() != prev.getI())
    {
	int n = cfg.owner()->attrAt("n").at().getI();
	if(cfg.getI() < 0) cfg.setI(((prev.getI()+1) >= n) ? 0 : (prev.getI()+1), false, true);
	else if(cfg.getI() >= n) cfg.setI(n-1, false, true);
	if(cfg.getI() != prev.getI())
	{
	    cfg.owner()->attrAt("aDoc").at().setS(cfg.owner()->attrAt("tmpl").at().getS());
	    if(prev.getI() == cfg.owner()->attrAt("vCur").at().getI())
		cfg.owner()->attrAt("vCur").at().setI(cfg.getI());

	    // Save cursor to document to project's DB
	    if(prev.getI() < n && prev.getI() >= 0) {
		TConfig c_el(&mod->elPrjSes());
		TSYS::pathLev(sw->path(),0,true,&off);
		c_el.cfg("IDW").setS(sw->path().substr(off));
		c_el.cfg("ID").setS(cfg.id());
		c_el.cfg("IO_VAL").setI(cfg.getI());
		SYS->db().at().dataSet(db+"."+tbl,mod->nodePath()+tbl,c_el,false,true);
	    }
	}
	sizeUpdate(sw);
    }
    //Document save
    else if(cfg.id() == "aDoc" && cfg.getS() != prev.getS())
    {
	TConfig c_el(&mod->elPrjSes());
	TSYS::pathLev(sw->path(),0,true,&off);
	c_el.cfg("IDW").setS(sw->path().substr(off));
	c_el.cfg("ID").setS("doc"+i2s(cfg.owner()->attrAt("aCur").at().getI()));
	c_el.cfg("IO_VAL").setS(cfg.getS());
	SYS->db().at().dataSet(db+"."+tbl, mod->nodePath()+tbl, c_el, false, true);
    }
    //Move archive view cursor
    else if(cfg.id() == "vCur" && cfg.getI() != prev.getI())
    {
	TConfig c_el(&mod->elPrjSes());
	TSYS::pathLev(sw->path(),0,true,&off);
	c_el.cfg("IDW").setS(sw->path().substr(off));
	c_el.cfg("IO_VAL").setView(false);

	int aCur = cfg.owner()->attrAt("aCur").at().getI();
	int n = cfg.owner()->attrAt("n").at().getI();

	if(cfg.getI() < 0)
	{
	    int docN = prev.getI();
	    // Search next document
	    if(cfg.getI() == -1)
		while(docN != aCur)
		{
		    c_el.cfg("ID").setS("doc"+i2s(docN));
		    if(docN != prev.getI() && SYS->db().at().dataGet(db+"."+tbl,mod->nodePath()+tbl,c_el,false,true)) break;
		    if(++docN >= n) docN = 0;
		}
	    // Search previous document
	    else
	    {
		if(--docN < 0) docN = n-1;
		if(docN == aCur) docN = prev.getI();
		c_el.cfg("ID").setS("doc"+i2s(docN));
		if(!SYS->db().at().dataGet(db+"."+tbl,mod->nodePath()+tbl,c_el,false,true)) docN = prev.getI();
	    }
	    if(docN != cfg.getI())	cfg.setI(docN,false,true);
	}
	else if(cfg.getI() >= n)	cfg.setI(cfg.owner()->attrAt("aCur").at().getI(), false, true);
	if(cfg.getI() != prev.getI())
	{
	    c_el.cfg("ID").setS("doc"+i2s(cfg.getI()));
	    c_el.cfg("IO_VAL").setView(true);
	    SYS->db().at().dataGet(db+"."+tbl,mod->nodePath()+tbl,c_el,false,true);
	    cfg.owner()->attrAt("doc").at().setS(c_el.cfg("IO_VAL").getS());
	}
    }

    return Widget::attrChange(cfg,prev);
}

bool OrigDocument::cntrCmdAttributes( XMLNode *opt, Widget *src )
{
    if(!src) src = this;
    //> Get page info
    if(opt->name() == "info")
    {
	Widget::cntrCmdAttributes(opt,src);
	XMLNode *root, *el;
	if((root=ctrMkNode("area",opt,-1,"/attr",_("Attributes"))))
	    for(unsigned i_ch = 0; i_ch < root->childSize(); i_ch++)
            {
                el = root->childGet(i_ch);
                int p = atoi(el->attr("p").c_str());
                switch(p)
                {
		    case A_DocStyle: el->setAttr("SnthHgl","1")->setAttr("help",
			_("CSS rules in rows like \"body { background-color:#818181; }\""));
			break;
		    case A_DocTmpl: el->setAttr("SnthHgl","1")->setAttr("help",
			_("Document's template in XHTML. Start from tag \"body\" and include procedures parts:\n"
			"<body docProcLang=\"JavaLikeCalc.JavaScript\">\n<h1>Value<?dp return wCod+1.314;?></h1>\n</body>"));
			break;
		    case A_DocDoc:
			el->setAttr("SnthHgl","1")->setAttr("help",_("Final document in XHTML. Start from tag \"body\"."));
			break;
		    case A_DocTime:
			el->setAttr("help",_("Write the time for document generation from that point or zero for regeneration."));
			break;
		    case A_DocFont: el->setAttr("help",Widget::helpFont());	break;
		    default:
			if(el->attr("id") == "aDoc")
			    el->setAttr("SnthHgl","1")->setAttr("help",_("Current archive document in XHTML. Start from tag \"body\"."));
		}
	    }
	return true;
    }

    //> Process command to page
    string a_path = opt->attr("path");
    if((a_path == "/attr/tmpl" || a_path == "/attr/doc" || a_path == "/attr/aDoc") && ctrChkNode(opt,"SnthHgl",RWRWR_,"root",SUI_ID,SEC_RD))
    {
	opt->childAdd("blk")->setAttr("beg","<!--")->setAttr("end","-->")->setAttr("color","gray")->setAttr("font_italic","1");
	XMLNode *tag = opt->childAdd("blk")->setAttr("beg","<\\?")->setAttr("end","\\?>")->setAttr("color","#666666");
	//>> Get document's language syntax highlight.
	try
	{
	    string dcLng;
	    TArrayObj *reRez = TRegExp(".*<body\\s+.*\\s*docProcLang=\"([^\"]+)\".*>.*","").match(src->attrAt(a_path.substr(6)).at().getS());
	    if(reRez->propGet("length").getI() >= 2) dcLng = reRez->propGet("1").getS();
	    delete reRez;
	    if(dcLng.size())
		SYS->daq().at().at(TSYS::strParse(dcLng,0,".")).at().compileFuncSynthHighl(TSYS::strParse(dcLng,1,"."),*tag);
	} catch(...){ }
	tag = opt->childAdd("blk")->setAttr("beg","<\\w+")->setAttr("end","\\/?>")->setAttr("font_weight","1");
	    tag->childAdd("rule")->setAttr("expr","\\b\\w+[ ]*(?==)")->setAttr("color","blue");
	    tag->childAdd("rule")->setAttr("expr","[ ]?\"[^\"]+\"")->setAttr("color","darkgreen");
	    tag->childAdd("rule")->setAttr("expr","[ ]?'[^']+'")->setAttr("color","darkgreen");
	opt->childAdd("rule")->setAttr("expr","<\\/[\\w]+>")->setAttr("font_weight","1");
	opt->childAdd("rule")->setAttr("expr","&([a-zA-Z]*|#\\d*);")->setAttr("color","#AF7E00");
    }
    else if(a_path == "/attr/style" && ctrChkNode(opt,"SnthHgl",RWRWR_,"root",SUI_ID,SEC_RD))
    {
	opt->childAdd("blk")->setAttr("beg","\\{")->setAttr("end","\\}")->setAttr("color","#666666")->
	    childAdd("rule")->setAttr("expr",":[^;]+")->setAttr("color","blue");
	opt->childAdd("rule")->setAttr("expr","(\\.|#)\\w+\\s")->setAttr("color","darkorange");
    }
    else return Widget::cntrCmdAttributes(opt,src);

    return true;
}

void OrigDocument::sizeUpdate( SessWdg *sw )
{
    string db  = sw->ownerSess()->parent().at().DB();
    string tbl = sw->ownerSess()->parent().at().tbl()+"_ses";

    int aCur = sw->attrAt("aCur").at().getI();
    int n = sw->attrAt("n").at().getI();
    int rSz = n;
    if(aCur < n)
    {
	int off = 0;
	TConfig c_el(&mod->elPrjSes());
	TSYS::pathLev(sw->path(),0,true,&off);
	c_el.cfg("IDW").setS(sw->path().substr(off));
	c_el.cfg("ID").setS("doc"+i2s(aCur+1));
	c_el.cfg("IO_VAL").setView(false);
	if(!SYS->db().at().dataGet(db+"."+tbl,mod->nodePath()+tbl,c_el,false,true)) rSz = aCur+1;
	else {
	    c_el.cfg("ID").setS("doc"+i2s(n-1));
	    if(!SYS->db().at().dataGet(db+"."+tbl,mod->nodePath()+tbl,c_el,false,true)) rSz = aCur+1;
	}
    }
    sw->attrAt("aSize").at().setI(rSz);
}

TVariant OrigDocument::objFuncCall_w( const string &iid, vector<TVariant> &prms, const string &user, Widget *src )
{
    // string getArhDoc(integer nDoc) - get archive document text to 'nDoc' depth.
    //  nDoc - archive document at depth (0-{aSize-1})
    if(iid == "getArhDoc" && prms.size() >= 1)
    {
	int nDoc = prms[0].getI();
        int aCur = src->attrAt("aCur").at().getI();
	int aSize = src->attrAt("aSize").at().getI();
	SessWdg *sw = dynamic_cast<SessWdg*>(src);
	if(!sw || nDoc < 0 || nDoc >= aSize) return "";

	aCur -= nDoc;
	if(aCur < 0) aCur += aSize;

	string db  = sw->ownerSess()->parent().at().DB();
	string tbl = sw->ownerSess()->parent().at().tbl()+"_ses";

	int off = 0;
	TConfig c_el(&mod->elPrjSes());
	TSYS::pathLev(sw->path(),0,true,&off);
	c_el.cfg("IDW").setS(sw->path().substr(off));
	c_el.cfg("ID").setS("doc"+i2s(aCur));
	if(SYS->db().at().dataGet(db+"."+tbl,mod->nodePath()+tbl,c_el,false,true)) return c_el.cfg("IO_VAL").getS();

	return "";
    }

    return TVariant();
}

string OrigDocument::makeDoc( const string &tmpl, Widget *wdg )
{
    XMLNode xdoc;
    string iLang;				//Process instruction language
    string wProgO;				//Object of work program
    time_t lstTime = 0;				//Last time
    TFunction funcIO(TSYS::path2sepstr(wdg->path(),'_'));
    TValFunc funcV(wdg->id()+"_doc",NULL,false);
    vector<string> als;

    //> Parse template
    try
    {
	if(!tmpl.empty()) xdoc.load(XHTML_entity+tmpl, true, Mess->charset());
	else return "";
    }
    catch(TError err)
    {
	mess_err(wdg->nodePath().c_str(),_("Document parsing error: %s."),err.mess.c_str());
	return "";
    }

    //> Prepare call instructions environment
    if(strcasecmp(xdoc.name().c_str(),"body") == 0)
    {
	iLang = xdoc.attr("docProcLang");
	lstTime = atoi(xdoc.attr("docTime").c_str());
    }
    if(TSYS::strNoSpace(iLang).empty())	iLang = "JavaLikeCalc.JavaScript";
    if(!lstTime) lstTime = wdg->attrAt("bTime").at().getI();

    //>> Add generic io
    funcIO.ioIns(new IO("rez",_("Result"),IO::String,IO::Return), A_DocCalcPrmRez);
    funcIO.ioIns(new IO("time",_("Document time"),IO::Integer,IO::Default), A_DocCalcPrmTime);
    funcIO.ioIns(new IO("bTime",_("Document begin time"),IO::Integer,IO::Default), A_DocCalcPrmBTime);
    funcIO.ioIns(new IO("lTime",_("Last time"),IO::Integer,IO::Default), A_DocCalcPrmLTime);
    funcIO.ioIns(new IO("rTime",_("Repeat time (s)"),IO::Integer,IO::Default), A_DocCalcPrmRTime);
    funcIO.ioIns(new IO("rTimeU",_("Repeat time (us)"),IO::Integer,IO::Default), A_DocCalcPrmRTimeU);
    funcIO.ioIns(new IO("rPer",_("Repeat period"),IO::Real,IO::Default), A_DocCalcPrmRPer);
    funcIO.ioIns(new IO("mTime",_("Message time"),IO::Integer,IO::Default), A_DocCalcPrmMTime);
    funcIO.ioIns(new IO("mTimeU",_("Message time (mcs)"),IO::Integer,IO::Default), A_DocCalcPrmMTimeU);
    funcIO.ioIns(new IO("mLev",_("Message level"),IO::Integer,IO::Default), A_DocCalcPrmMLev);
    funcIO.ioIns(new IO("mCat",_("Message category"),IO::String,IO::Default), A_DocCalcPrmMCat);
    funcIO.ioIns(new IO("mVal",_("Message value"),IO::String,IO::Default), A_DocCalcPrmMVal);
    //>> Add user io
    wdg->attrList(als);
    for(unsigned i_a = 0; i_a < als.size(); i_a++)
    {
	AutoHD<Attr> cattr = wdg->attrAt(als[i_a]);
	if(!(cattr.at().flgGlob()&Attr::IsUser)) continue;
	funcIO.ioAdd(new IO(als[i_a].c_str(),cattr.at().name().c_str(),cattr.at().fld().typeIO(),IO::Output));
    }
    try
    {
	//>> Compile empty function for binding to object
	wProgO = SYS->daq().at().at(TSYS::strSepParse(iLang,0,'.')).at().compileFunc(TSYS::strSepParse(iLang,1,'.'),funcIO,"");
	//>> Connect to compiled function
	funcV.setFunc(&((AutoHD<TFunction>)SYS->nodeAt(wProgO)).at());
	//>> Load values of generic IO
	funcV.setI(A_DocCalcPrmTime, wdg->attrAt("time").at().getI());
	funcV.setI(A_DocCalcPrmBTime, wdg->attrAt("bTime").at().getI());
	funcV.setI(A_DocCalcPrmLTime, lstTime);
	//>> Load values of user IO
	for(int i_a = 12; i_a < funcV.ioSize( ); i_a++)
	    funcV.set(i_a,wdg->attrAt(funcV.func()->io(i_a)->id()).at().get());
    }
    catch(TError err)
    {
	mess_err(wdg->nodePath().c_str(),_("Compile function for document is error: %s"),err.mess.c_str());
	return "";
    }

    //> Warning timeout message subtree add to generic tree
    xdoc.childAdd()->load(TSYS::strMess(
	_("<BODY>\n"
	"  <H1>The document is forming now ...</H1>\n"
	"  <P>The document is too big to generate into standard user interface timeout (%ds). "
	"And for now the document is into building. "
	"You can wait for forming finish or terminate the process by select lesser time interval for the document.</P>\n"
	"  <P>Building progress:\n"
	"    <ul id='progress' />\n"
	"  </P>\n"
	"</BODY>"),STD_INTERF_TM), false, Mess->charset());

    //> Node proocess
    OrigDocument::nodeProcess(wdg, &xdoc, funcV, funcIO, iLang);

    //> Remove warning timeout message subtree
    xdoc.childDel(xdoc.childSize()-1);

    xdoc.setAttr("docTime", i2s(funcV.getI(A_DocCalcPrmTime)));

    return xdoc.save(0, Mess->charset());
}

void OrigDocument::nodeProcess( Widget *wdg, XMLNode *xcur, TValFunc &funcV, TFunction &funcIO, const string &iLang, bool instrDel, time_t upTo )
{
    //> Progress warning node
    XMLNode *progrNode = NULL;

    if(!upTo) upTo = time(NULL)+STD_INTERF_TM;
    //> Process instructions
    if(xcur->childGet("<?dp",0,true))
    {
	if(!atoi(xcur->attr("docAppend").c_str()))
	    for(unsigned i_t = 0; i_t < xcur->childSize(); )
		if(xcur->childGet(i_t)->name().compare(0,4,"<?dp") != 0) xcur->childDel(i_t);
		else i_t++;
	//> Call procedures
	for(int i_t = 0; i_t < (int)xcur->childSize(); i_t++)
	{
	    XMLNode *curPrc = xcur->childGet(i_t);
	    if(curPrc->name().compare(0,4,"<?dp") != 0) continue;
	    try
	    {
		//>>> Compile for new instruction
		SYS->daq().at().at(TSYS::strSepParse(iLang,0,'.')).
		       at().compileFunc(TSYS::strSepParse(iLang,1,'.'), funcIO, curPrc->text());
		//>>> Call
		funcV.setS(A_DocCalcPrmRez, "");
		funcV.calc();
		//>>> Load result to XML tree
		XMLNode xproc;
		xproc.load(string(XHTML_entity)+"<i>"+funcV.getS(A_DocCalcPrmRez)+"</i>", true, Mess->charset());
		//>>> Set result
		for(unsigned i_tr = 0; i_tr < xproc.childSize(); i_tr++)
		    *(xcur->childAdd()) = *xproc.childGet(i_tr);
		if(instrDel) { xcur->childDel(i_t--); }
	    }
	    catch(TError err)
	    {
		mess_err(wdg->nodePath().c_str(),_("Instruction procedure '%s' error: %s"), TSYS::strSepParse(iLang,1,'.').c_str(), err.mess.c_str());
		mess_err(wdg->nodePath().c_str(),_("Error code: %s"), curPrc->text().c_str());
	    }
	}
    }

    float dRpt = 1;
    string dAMess;
    //> Go to include nodes
    for(unsigned i_c = 0; i_c < xcur->childSize(); i_c++)
    {
	XMLNode *reptN = xcur->childGet(i_c);
	if(reptN->name().size() && reptN->name()[0] == '<') continue;
	//>> Repeat tags
	if(funcV.getI(A_DocCalcPrmBTime) && (dRpt=atof(reptN->attr("docRept").c_str())) > 1e-6)
	{
	    int rCnt = 0;

	    bool docRevers = atoi(reptN->attr("docRevers").c_str());
	    funcV.setR(A_DocCalcPrmRPer, dRpt);
	    int64_t wTime = (int64_t)funcV.getI(A_DocCalcPrmTime)*1000000;
	    int64_t bTime = (int64_t)funcV.getI(A_DocCalcPrmBTime)*1000000;
	    int64_t lstTime = (int64_t)funcV.getI(A_DocCalcPrmLTime)*1000000;
	    int64_t perRpt = (int64_t)(1000000*dRpt);
	    int64_t rTime = bTime + perRpt*((lstTime-bTime)/perRpt);
	    if(lstTime && lstTime < bTime) rTime -= perRpt;
	    //if(((time-rTime)/perRpt) > 1000) continue;

	    //>> Progress prepare
	    if(!progrNode)
	    {
		progrNode = xcur->root();
		progrNode = progrNode->childGet(progrNode->childSize()-1)->getElementBy("id","progress");
	    }
	    progrNode->childAdd("li")->setText(TSYS::strMess(_("Data block %d: %0.2f%% loaded."),progrNode->childSize(),0));

	    while(rTime < wTime && !TSYS::taskEndRun())
	    {
		//> Drop current changes and continue
		if(time(NULL) >= upTo)
		{
		    upTo = time(NULL)+STD_INTERF_TM;
		    if(!wdg->attrAt("n").at().getI() || wdg->attrAt("aCur").at().getI() == wdg->attrAt("vCur").at().getI())
		    {
			progrNode->childGet(progrNode->childSize()-1)->
			    setText(TSYS::strMess(_("Data block %d: %0.2f%% loaded."),progrNode->childSize(),100*(float)(rTime-bTime)/(float)(wTime-bTime)));
			XMLNode *rootN = xcur->root();
			wdg->attrAt("doc").at().setS(rootN->childGet(rootN->childSize()-1)->save(0, Mess->charset()));
		    }
		}
		//> Process
		if(atoi(reptN->attr("docRptEnd").c_str()))
		{
		    int i_n = docRevers?(i_c+1):i_c;
		    *(xcur->childIns(i_n)) = *reptN;
		    OrigDocument::nodeClear(xcur->childGet(i_n));
		    if(!docRevers) i_c++;
		    rCnt++;
		}
		int64_t rTimeT = vmin(rTime+perRpt,wTime);
		funcV.setI(A_DocCalcPrmRTime, rTimeT/1000000);
		funcV.setI(A_DocCalcPrmRTimeU, rTimeT%1000000);
		funcV.setR(A_DocCalcPrmRPer, (rTimeT-rTime)/1000000.0);
		OrigDocument::nodeProcess(wdg, reptN, funcV, funcIO, iLang, false, upTo);
		reptN->setAttr("docRptEnd", ((rTimeT-rTime)==perRpt)?"1":"0");
		rTime = rTimeT;
	    }
	    funcV.setI(A_DocCalcPrmRTime, 0);
	    funcV.setI(A_DocCalcPrmRTimeU, 0);
	    funcV.setR(A_DocCalcPrmRPer, 0);
	    if(docRevers) i_c += rCnt;

	    progrNode->childGet(progrNode->childSize()-1)->setText(TSYS::strMess(_("Data block %d: %0.2f%% loaded."),progrNode->childSize(),100));
	}
	//>> Repeat messages
	else if(!(dAMess=xcur->childGet(i_c)->attr("docAMess")).empty())
	{
	    int rCnt = 0;
	    XMLNode *reptN = xcur->childGet(i_c);
	    bool docRevers = atoi(xcur->childGet(i_c)->attr("docRevers").c_str());

	    int off = 0;
	    int dACat = atoi(TSYS::strSepParse(dAMess,0,':',&off).c_str());
	    string dATmpl = dAMess.substr(off);

	    vector<TMess::SRec> mess;
	    SYS->archive().at().messGet(funcV.getI(A_DocCalcPrmLTime), funcV.getI(A_DocCalcPrmTime), mess, dATmpl, (TMess::Type)dACat);

	    //>> Progress bar prepare
	    if(!progrNode)
	    {
		progrNode = xcur->root();
		progrNode = progrNode->childGet(progrNode->childSize()-1)->getElementBy("id","progress");
	    }
	    progrNode->childAdd("li")->setText(TSYS::strMess(_("Messages block %d: %0.2f%% loaded."),progrNode->childSize(),0));

	    for(unsigned i_r = 0; i_r < mess.size() && !TSYS::taskEndRun(); i_r++)
	    {
		//> Drop current changes and continue
		if(time(NULL) >= upTo)
		{
		    upTo = time(NULL)+STD_INTERF_TM;
		    if(!wdg->attrAt("n").at().getI() || wdg->attrAt("aCur").at().getI() == wdg->attrAt("vCur").at().getI())
		    {
			progrNode->childGet(progrNode->childSize()-1)->
			    setText(TSYS::strMess(_("Messages block %d: %0.2f%% loaded."),progrNode->childSize(),100*(float)i_r/(float)mess.size()));
			XMLNode *rootN = xcur->root();
			wdg->attrAt("doc").at().setS(rootN->childGet(rootN->childSize()-1)->save(0, Mess->charset()));
		    }
		}

		if(atoi(reptN->attr("docRptEnd").c_str()))
		{
		    unsigned i_n = (docRevers ? (i_c+1) : i_c);
		    *(xcur->childIns(i_n)) = *reptN;
		    nodeClear(xcur->childGet(i_n));
		    if(!docRevers) i_c++;
		    rCnt++;
		}
		funcV.setI(A_DocCalcPrmMTime, mess[i_r].time);
		funcV.setI(A_DocCalcPrmMTimeU, mess[i_r].utime);
		funcV.setI(A_DocCalcPrmMLev, mess[i_r].level);
		funcV.setS(A_DocCalcPrmMCat, mess[i_r].categ);
		funcV.setS(A_DocCalcPrmMVal, mess[i_r].mess);
		nodeProcess(wdg, reptN, funcV, funcIO, iLang, false, upTo);
		reptN->setAttr("docRptEnd", "1");
	    }

	    progrNode->childGet(progrNode->childSize()-1)->setText(TSYS::strMess(_("Messages block %d: %0.2f%% loaded."),progrNode->childSize(),100));

	    funcV.setI(A_DocCalcPrmMTime, 0);
	    funcV.setI(A_DocCalcPrmMTimeU, 0);
	    funcV.setI(A_DocCalcPrmMLev, 0);
	    funcV.setS(A_DocCalcPrmMCat, "");
	    funcV.setS(A_DocCalcPrmMVal, "");
	    if(docRevers) i_c += rCnt;
	}
	else nodeProcess(wdg,xcur->childGet(i_c),funcV,funcIO,iLang,instrDel,upTo);
    }
}

void OrigDocument::nodeClear( XMLNode *xcur )
{
    xcur->attrDel("docRept");
    xcur->attrDel("docRptEnd");
    xcur->attrDel("docRevers");
    xcur->attrDel("docAMess");

    for(unsigned i_c = 0; i_c < xcur->childSize(); )
	if(xcur->childGet(i_c)->name().compare(0,4,"<?dp") == 0) xcur->childDel(i_c);
	else nodeClear(xcur->childGet(i_c++));
}

void *OrigDocument::DocTask( void *param )
{
    Widget *sw = (Widget *)param;

    // The document generation
    string mkDk;
    if(!sw->attrAt("n").at().getI())
    {
	mkDk = sw->attrAt("doc").at().getS();
	if(mkDk.empty()) mkDk = sw->attrAt("tmpl").at().getS();
	mkDk = OrigDocument::makeDoc(mkDk,sw);
	sw->attrAt("doc").at().setS(mkDk);
    }
    else
    {
	int aCur = sw->attrAt("aCur").at().getI();
	mkDk = sw->attrAt("aDoc").at().getS();
	if(mkDk.empty()) mkDk = sw->attrAt("tmpl").at().getS();

	mkDk = makeDoc(mkDk,sw);
	sw->attrAt("aDoc").at().setS(mkDk);
	if(aCur == sw->attrAt("vCur").at().getI()) sw->attrAt("doc").at().setS(mkDk);
    }
    sw->attrAt("process").at().setB(false);

    return NULL;
}

//************************************************
//* OrigFunction: User function original widget  *
//************************************************
OrigFunction::OrigFunction( ) : PrWidget("Function")
{

}

string OrigFunction::name( )
{
    return _("Built-in function");
}

string OrigFunction::descr( )
{
    return _("Built-in function widget.");
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
    return _("Elements box widget of the finite visualization.");
}

void OrigBox::postEnable( int flag )
{
    LWidget::postEnable(flag);

    if(flag&TCntrNode::NodeConnect)
    {
	attrAdd(new TFld("pgOpenSrc",_("Page:open source"),TFld::String,TFld::NoFlag,"","","","",i2s(A_PG_OPEN_SRC).c_str()));
	attrAdd(new TFld("pgGrp",_("Page:group"),TFld::String,TFld::NoFlag,"","","","",i2s(A_PG_GRP).c_str()));
	attrAdd(new TFld("backColor",_("Background:color"),TFld::String,Attr::Color,"","#FFFFFF","","",i2s(A_BackColor).c_str()));
	attrAdd(new TFld("backImg",_("Background:image"),TFld::String,Attr::Image,"","","","",i2s(A_BackImg).c_str()));
	attrAdd(new TFld("bordWidth",_("Border:width"),TFld::Integer,TFld::NoFlag,"","0","","",i2s(A_BordWidth).c_str()));
	attrAdd(new TFld("bordColor",_("Border:color"),TFld::String,Attr::Color,"","#000000","","",i2s(A_BordColor).c_str()));
	attrAdd(new TFld("bordStyle",_("Border:style"),TFld::Integer,TFld::Selected,"",i2s(FBRD_SOL).c_str(),
	    TSYS::strMess("%d;%d;%d;%d;%d;%d;%d;%d;%d",FBRD_NONE,FBRD_DOT,FBRD_DASH,FBRD_SOL,FBRD_DBL,FBRD_GROOVE,FBRD_RIDGE,FBRD_INSET,FBRD_OUTSET).c_str(),
	    _("None;Dotted;Dashed;Solid;Double;Groove;Ridge;Inset;Outset"),i2s(A_BordStyle).c_str()));
    }
}

bool OrigBox::cntrCmdAttributes( XMLNode *opt, Widget *src )
{
    if(!src) src = this;
    //> Get page info
    if(opt->name() == "info")
    {
	Widget::cntrCmdAttributes(opt,src);
	XMLNode *root, *el;
	if((root=ctrMkNode("area",opt,-1,"/attr",_("Attributes"))))
	    for(unsigned i_ch = 0; i_ch < root->childSize(); i_ch++)
            {
                el = root->childGet(i_ch);
                int p = atoi(el->attr("p").c_str());
                switch(p)
                {
		    case A_BackColor: case A_BordColor: el->setAttr("help",Widget::helpColor());	break;
		    case A_BackImg: el->setAttr("help",Widget::helpImg());		break;
		}
	    }
	return true;
    }

    //> Process command to page
    return Widget::cntrCmdAttributes(opt,src);
}