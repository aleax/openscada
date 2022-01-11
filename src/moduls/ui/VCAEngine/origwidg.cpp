
//OpenSCADA module UI.VCAEngine file: origwidg.cpp
/***************************************************************************
 *   Copyright (C) 2006-2022 by Roman Savochenko, <roman@oscada.org>       *
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

void PrWidget::preDisable( int flag )
{
    if(flag&NodeRemove)
	throw TError(mod->nodePath().c_str(),_("Impossible deleting the base primitive-widget."));

    Widget::preDisable(flag);
}

string PrWidget::ico( ) const
{
    if(LWidget::ico().size()) return LWidget::ico();
    if(TUIS::icoGet("VCA.wdg_"+id(),NULL,true).size())
	return TSYS::strEncode(TUIS::icoGet("VCA.wdg_"+id()),TSYS::base64);

    return "";
}

void PrWidget::setEnable( bool val, bool force )
{
    if(enable() == val) return;

    LWidget::setEnable(val);

    //Init active attributes
    if(val) {
	vector<string> ls;
	attrList(ls);
	for(unsigned iL = 0; iL < ls.size(); iL++) {
	    AutoHD<Attr> attr = attrAt(ls[iL]);
	    if(!(attr.at().flgGlob()&Attr::Active)) continue;
	    attr.at().setS(attr.at().getS(),true);
	    attr.at().setAModif_(0);
	    attrList(ls);
	}
    }
}

bool PrWidget::cntrCmdGeneric( XMLNode *opt )
{
    //Get page info
    if(opt->name() == "info") {
	Widget::cntrCmdGeneric(opt);
	ctrMkNode("oscada_cntr", opt, -1, "/", TSYS::strMess(_("Base widget '%s'."),id().c_str()));
	ctrMkNode("fld", opt, -1, "/wdg/st/parent", _("Parent"), R_R_R_, owner().c_str(),grp().c_str(), 2, "tp","str", "dest","");
	return true;
    }

    //Process command to page
    string a_path = opt->attr("path");
    if(a_path == "/wdg/st/parent" && ctrChkNode(opt,"get",R_R_R_,owner().c_str(),grp().c_str(),SEC_RD))
	opt->setText(parentAddr());
    else return Widget::cntrCmdGeneric(opt);

    return true;
}

void PrWidget::cntrCmdProc( XMLNode *opt )
{
    if(cntrCmdServ(opt)) return;

    //Get page info
    if(opt->name() == "info") {
	cntrCmdGeneric(opt);
	cntrCmdAttributes(opt);
	return;
    }

    //Process command to page
    if(!(cntrCmdGeneric(opt) || cntrCmdAttributes(opt))) TCntrNode::cntrCmdProc(opt);
}


//*************************************************
//* Original widgets based at primitive           *
//* widget template                               *

//*************************************************
//* OrigElFigure:                                 *
//*  Elementary figure original widget            *
//*************************************************
OrigElFigure::OrigElFigure( ) : PrWidget("ElFigure")	{ }

string OrigElFigure::name( ) const	{ return _("Elementary figure"); }

string OrigElFigure::descr( ) const	{ return _("Widget of the elementary figure of the finite visualization."); }

void OrigElFigure::postEnable( int flag )
{
    LWidget::postEnable(flag);

    if(flag&TCntrNode::NodeConnect) {
	attrAdd(new TFld("lineWdth",trS("Line: width"),TFld::Integer,TFld::NoFlag,"","1","0;99","",i2s(A_ElFigLineW).c_str()));
	attrAdd(new TFld("lineClr",trS("Line: color"),TFld::String,Attr::Color,"","#000000","","",i2s(A_ElFigLineClr).c_str()));
	attrAdd(new TFld("lineStyle",trS("Line: style"),TFld::Integer,TFld::Selectable,"","0",
	    TSYS::strMess("%d;%d;%d",EF_SOLID,EF_DASH,EF_DOT).c_str(),_("Solid;Dashed;Dotted"),i2s(A_ElFigLineStl).c_str()));
	attrAdd(new TFld("bordWdth",trS("Border: width"),TFld::Integer,TFld::NoFlag,"","0","0;99","",i2s(A_ElFigBordW).c_str()));
	attrAdd(new TFld("bordClr",trS("Border: color"),TFld::String,Attr::Color,"","#000000","","",i2s(A_ElFigBordClr).c_str()));
	attrAdd(new TFld("fillColor",trS("Fill: color"),TFld::String,Attr::Color,"","","","",i2s(A_ElFigFillClr).c_str()));
	attrAdd(new TFld("fillImg",trS("Fill: image"),TFld::String,Attr::Image,"","","","",i2s(A_ElFigFillImg).c_str()));
	attrAdd(new TFld("orient",trS("Orientation angle"),TFld::Integer,TFld::NoFlag,"","0","-360;360","",i2s(A_ElFigOrient).c_str()));
	attrAdd(new TFld("mirror",trS("Mirroring"),TFld::Boolean,TFld::NoFlag,"","0","","",i2s(A_ElFigMirror).c_str()));
	attrAdd(new TFld("elLst",trS("Elements list"),TFld::String,TFld::FullText|Attr::Active,"","","","",i2s(A_ElFigElLst).c_str()));
    }
}

bool OrigElFigure::cntrCmdAttributes( XMLNode *opt, Widget *src )
{
    if(!src) src = this;

    //Get page info
    if(opt->name() == "info") {
	Widget::cntrCmdAttributes(opt,src);
	XMLNode *root, *el;
	if((root=ctrMkNode("area",opt,-1,"/attr",_("Attributes"))))
	    for(unsigned iCh = 0; iCh < root->childSize(); iCh++) {
		el = root->childGet(iCh);
		int p = s2i(el->attr("p"));
		switch(p) {
		    case A_ElFigLineClr: case A_ElFigBordClr: case A_ElFigFillClr: el->setAttr("help",Widget::helpColor());	break;
		    case A_ElFigFillImg: el->setAttr("help",Widget::helpImg());	break;
		    case A_ElFigElLst: el->setAttr("SnthHgl","1")->setAttr("help",
		    _("The elements list can contain:\n"
		      "  line:({x}|{y}):({x}|{y})[:{width}[:{color}[:{bord_w}[:{bord_clr}[:{line_stl}]]]]]\n"
		      "  line:{p1}:{p2}[:w{n}[:c{n}[:w{n}[:c{n}[:s{n}]]]]]\n"
		      "  arc:({x}|{y}):({x}|{y}):({x}|{y}):({x}|{y}):({x}|{y})[:{width}[:{color}[:{bord_w}[:{bord_clr}[:{line_stl}]]]]]\n"
		      "  arc:{p1}:{p2}:{p3}:{p4}:{p5}[:w{n}[:c{n}[:w{n}[:c{n}[:s{n}]]]]]\n"
		      "  bezier:({x}|{y}):({x}|{y}):({x}|{y}):({x}|{y})[:{width}[:{color}[:{bord_w}[:{bord_clr}[:{line_stl}]]]]]\n"
		      "  bezier:{p1}:{p2}:{p3}:{p4}[:w{n}[:c{n}[:w{n}[:c{n}[:s{n}]]]]]\n"
		      "  fill:({x}|{y}):({x}|{y}):...:({x}|{y})[:{fill_clr}[:{fill_img}]]\n"
		      "  fill:{p1}:{p2}:...:{pN}[:c{n}[:i{n}]]\n"
		      "Where:\n"
		      "  x, y            - direct point (x,y), coordinate in float point pixels;\n"
		      "  p1 ... pN       - dynamic point 1...n;\n"
		      "  width, bord_w   - direct line and border width in float point pixels;\n"
		      "  w{n}            - dynamic width 'n';\n"
		      "  color, bord_clr, fill_clr - direct line, border and fill color name or 32bit code whith alpha: {name}[-{AAA}], #RRGGBB-AAA;\n"
		      "  c{n}            - dynamic color 'n';\n"
		      "  line_stl        - direct line style: 0-Solid, 1-Dashed, 2-Dotted;\n"
		      "  s{n}            - dynamic style 'n';\n"
		      "  fill_img        - direct image of the filling in the form \"[{src}:]{name}\", where:\n"
		      "     \"src\"      - image source:\n"
		      "        file      - directly from a local file by the path;\n"
		      "        res       - from table of DB-mime resources.\n"
		      "     \"name\"     - file path or mime-resource identifier.\n"
		      "  i{n}            - dynamic image of the filling 'n'.\n"
		      "For example:\n"
		      "  line:(50|25):(90.5|25):2:yellow:3:green:2\n"
		      "  arc:(25|50):(25|50):1:4:(25|50)::#000000-0\n"
		      "  fill:(25|50):(25|50):c2:i2\n"
		      "  fill:(50|25):(90.5|25):(90|50):(50|50):#d3d3d3:h_31"));
		    default:
			switch((p-A_ElFigIts)%A_ElFigItsSz) {
			    case A_ElFigItClr:	el->setAttr("help",Widget::helpColor());	break;
			    case A_ElFigItImg:	el->setAttr("help",Widget::helpImg());		break;
			}
			break;
		}
	    }
	return true;
    }

    //Process command to page
    string a_path = opt->attr("path");
    if(a_path == "/attr/elLst" && ctrChkNode(opt,"SnthHgl",RWRWR_,"root",SUI_ID,SEC_RD)) {
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
    if((cfg.flgGlob()&Attr::Active) && cfg.id() == "elLst") {
	string sel, sel1;
	string ls_prev = prev.getS();
	map<int,char> pntls, pntls_prev, wls, wls_prev, clrls, clrls_prev, imgls, imgls_prev, lstls, lstls_prev;

	//Parse last attributes list and make point list
	for(int iP = 0; iP < 2; iP++) {
	    string ls_w = (iP == 0) ? ls_prev : cfg.getS();
	    map<int,char> &pntls_w = (iP == 0) ? pntls_prev : pntls;
	    map<int,char> &wls_w = (iP == 0) ? wls_prev : wls;
	    map<int,char> &clrls_w = (iP == 0) ? clrls_prev : clrls;
	    map<int,char> &imgls_w = (iP == 0) ? imgls_prev : imgls;
	    map<int,char> &lstls_w = (iP == 0) ? lstls_prev : lstls;
	    for(int off = 0; (sel=TSYS::strSepParse(ls_w,0,'\n',&off)).size(); ) {
		int offe = 0;
		string fTp = TSYS::strSepParse(sel,0,':',&offe);
		if(fTp == "line" || fTp == "arc" || fTp == "bezier") {
		    int np = (fTp=="arc") ? 5 : ((fTp=="bezier") ? 4 : 2);
		    for(int i = 0; i < np; i++) pntls_w[s2i(TSYS::strSepParse(sel,0,':',&offe))] = true;
		    if((sel1=TSYS::strSepParse(sel,0,':',&offe)).size() && sel1[0] == 'w') wls_w[s2i(sel1.substr(1))] = true;
		    if((sel1=TSYS::strSepParse(sel,0,':',&offe)).size() && sel1[0] == 'c') clrls_w[s2i(sel1.substr(1))] = true;
		    if((sel1=TSYS::strSepParse(sel,0,':',&offe)).size() && sel1[0] == 'w') wls_w[s2i(sel1.substr(1))] = true;
		    if((sel1=TSYS::strSepParse(sel,0,':',&offe)).size() && sel1[0] == 'c') clrls_w[s2i(sel1.substr(1))] = true;
		    if((sel1=TSYS::strSepParse(sel,0,':',&offe)).size() && sel1[0] == 's') lstls_w[s2i(sel1.substr(1))] = true;
		}
		else if(fTp == "fill")
		    for(int zPnt = 0; !(sel1=TSYS::strSepParse(sel,0,':',&offe)).empty() || zPnt < 2; ) {
			if(sel1.empty())	zPnt++;
			if(sel1[0] == 'c')	clrls_w[s2i(sel1.substr(1))] = true;
			else if(sel1[0] == 'i')	imgls_w[s2i(sel1.substr(1))] = true;
		    }
	    }
	}

	//Add new dynamic items
	// Add no present dynamic points
	for(map<int,char>::iterator it = pntls.begin(); it != pntls.end(); ++it)
	    if(it->first && pntls_prev.find(it->first) == pntls_prev.end() && !cfg.owner()->attrPresent("p"+i2s(it->first)+"x"))
	    {
		cfg.owner()->attrAdd(new TFld(("p"+i2s(it->first)+"x").c_str(),(_("Point ")+i2s(it->first)+":x").c_str(),
		    TFld::Real,Attr::Mutable,"","0","","",i2s(A_ElFigIts+it->first*A_ElFigItsSz+A_ElFigItPntX).c_str()));
		cfg.owner()->attrAdd(new TFld(("p"+i2s(it->first)+"y").c_str(),(_("Point ")+i2s(it->first)+":y").c_str(),
		    TFld::Real,Attr::Mutable,"","0","","",i2s(A_ElFigIts+it->first*A_ElFigItsSz+A_ElFigItPntY).c_str()));
	    }
	// Add no present dynamic widths
	for(map<int,char>::iterator it = wls.begin(); it != wls.end(); ++it)
	    if(it->first && wls_prev.find(it->first) == wls_prev.end() && !cfg.owner()->attrPresent("w"+i2s(it->first)))
		cfg.owner()->attrAdd(new TFld(("w"+i2s(it->first)).c_str(),(_("Width ")+i2s(it->first)).c_str(),
		    TFld::Real,Attr::Mutable,"","1","","",i2s(A_ElFigIts+it->first*A_ElFigItsSz+A_ElFigItW).c_str()));
	// Add no present dynamic colors
	for(map<int,char>::iterator it = clrls.begin(); it != clrls.end(); ++it)
	    if(it->first && clrls_prev.find(it->first) == clrls_prev.end() && !cfg.owner()->attrPresent("c"+i2s(it->first)))
		cfg.owner()->attrAdd(new TFld(("c"+i2s(it->first)).c_str(),(_("Color ")+i2s(it->first)).c_str(),
		    TFld::String,Attr::Mutable|Attr::Color,"","","","",i2s(A_ElFigIts+it->first*A_ElFigItsSz+A_ElFigItClr).c_str()));
	//>> Add no present dynamic images
	for(map<int,char>::iterator it = imgls.begin(); it != imgls.end(); ++it)
	    if(it->first && imgls_prev.find(it->first) == imgls_prev.end() && !cfg.owner()->attrPresent("i"+i2s(it->first)))
		cfg.owner()->attrAdd(new TFld(("i"+i2s(it->first)).c_str(),(_("Image ")+i2s(it->first)).c_str(),
		    TFld::String,Attr::Mutable|Attr::Image,"","","","",i2s(A_ElFigIts+it->first*A_ElFigItsSz+A_ElFigItImg).c_str()));
	// Add no present line styles
	for(map<int,char>::iterator it = lstls.begin(); it != lstls.end(); ++it)
	    if(it->first && lstls_prev.find(it->first) == lstls_prev.end() && !cfg.owner()->attrPresent("s"+i2s(it->first)))
		cfg.owner()->attrAdd(new TFld(("s"+i2s(it->first)).c_str(),(_("Style ")+i2s(it->first)).c_str(),
		    TFld::Integer,Attr::Mutable|TFld::Selectable,"","0",
		    TSYS::strMess("%d;%d;%d",EF_SOLID,EF_DASH,EF_DOT).c_str(),_("Solid;Dashed;Dotted"),
		    i2s(A_ElFigIts+it->first*A_ElFigItsSz+A_ElFigItStl).c_str()));

	//Delete no dynamic items
	// Delete dynamic points
	for(map<int,char>::iterator it = pntls_prev.begin(); it != pntls_prev.end(); ++it)
	    if(it->first && pntls.find(it->first) == pntls.end()) {
		cfg.owner()->attrDel("p"+i2s(it->first)+"x");
		cfg.owner()->attrDel("p"+i2s(it->first)+"y");
	    }
	// Delete dynamic widths
	for(map<int,char>::iterator it = wls_prev.begin(); it != wls_prev.end(); ++it)
	    if(it->first && wls.find(it->first) == wls.end())
		cfg.owner()->attrDel("w"+i2s(it->first));
	// Delete dynamic colors
	for(map<int,char>::iterator it = clrls_prev.begin(); it != clrls_prev.end(); ++it)
	    if(it->first && clrls.find(it->first) == clrls.end())
		cfg.owner()->attrDel("c"+i2s(it->first));
	//>> Delete dynamic images
	for(map<int,char>::iterator it = imgls_prev.begin(); it != imgls_prev.end(); ++it)
	    if(it->first && imgls.find(it->first) == imgls.end())
		cfg.owner()->attrDel("i"+i2s(it->first));
	// Delete dynamic line styles
	for(map<int,char>::iterator it = lstls_prev.begin(); it != lstls_prev.end(); ++it)
	    if(it->first && lstls.find(it->first) == lstls.end())
		cfg.owner()->attrDel("s"+i2s(it->first));
    }

    return Widget::attrChange(cfg, prev);
}

//*************************************************
//* OrigFormEl: Form element original widget      *
//*************************************************
OrigFormEl::OrigFormEl( ) : PrWidget("FormEl")	{ }

string OrigFormEl::name( ) const	{ return _("Form element"); }

string OrigFormEl::descr( ) const	{ return _("Form element widget of the finite visualization."); }

void OrigFormEl::postEnable( int flag )
{
    LWidget::postEnable(flag);

    if(flag&TCntrNode::NodeConnect) {
	attrAdd(new TFld("elType",trS("Element type"),TFld::Integer,TFld::Selectable|Attr::Active,"2","0",
	    TSYS::strMess("%d;%d;%d;%d;%d;%d;%d;%d;%d;%d",F_LINE_ED,F_TEXT_ED,F_CHECK_BOX,F_BUTTON,F_COMBO,F_LIST,F_TREE,F_TABLE,F_SLIDER,F_SCROLL_BAR).c_str(),
	    _("Line edit;Text edit;Check box;Button;Combo box;List;Tree;Table;Slider;Scroll Bar"),i2s(A_FormElType).c_str()));
	attrAt("name").at().fld().setReserve(i2s(A_FormElName));
    }
}

bool OrigFormEl::attrChange( Attr &cfg, TVariant prev )
{
    if((cfg.flgGlob()&Attr::Active) && cfg.id() == "elType") {
	//Delete specific attributes
	if(cfg.getI() != prev.getI())
	    switch(prev.getI()) {
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
		case F_TABLE:
		    cfg.owner()->attrDel("set");
		case F_COMBO: case F_LIST: case F_TREE:
		    cfg.owner()->attrDel("value");
		    cfg.owner()->attrDel("items");
		    cfg.owner()->attrDel("font");
		    break;
		case F_SLIDER: case F_SCROLL_BAR:
		    cfg.owner()->attrDel("value");
		    cfg.owner()->attrDel("cfg");
		    break;
	    }

	//Create specific attributes
	switch(cfg.getI()) {
	    case F_LINE_ED:
		cfg.owner()->attrAdd(new TFld("value",trS("Value"),TFld::String,TFld::TransltText|Attr::Mutable,"200","","","",i2s(A_FormElValue).c_str()));
		cfg.owner()->attrAdd(new TFld("view",trS("View"),TFld::Integer,TFld::Selectable|Attr::Mutable|Attr::Active,"1","0",
		    TSYS::strMess("%d;%d;%d;%d;%d;%d;%d;%d",FL_TEXT,FL_COMBO,FL_INTEGER,FL_REAL,FL_TIME,FL_DATE,FL_DATE_TM,FL_PASS).c_str(),
		    _("Text;Combo;Integer;Real;Time;Date;Date and time;Password"),i2s(A_FormElMixP1).c_str()));
		cfg.owner()->attrAdd(new TFld("cfg",trS("Configuration"),TFld::String,TFld::FullText|TFld::TransltText|Attr::Mutable,"","","","",i2s(A_FormElMixP2).c_str()));
		cfg.owner()->attrAdd(new TFld("confirm",trS("Confirm"),TFld::Boolean,Attr::Mutable,"","1","","",i2s(A_FormElMixP3).c_str()));
		cfg.owner()->attrAdd(new TFld("font",trS("Font"),TFld::String,Attr::Font,"50","Arial 11","","",i2s(A_FormElFont).c_str()));
		break;
	    case F_TEXT_ED:
		cfg.owner()->attrAdd(new TFld("value",trS("Value"),TFld::String,TFld::FullText|TFld::TransltText|Attr::Mutable,"","","","",i2s(A_FormElValue).c_str()));
		cfg.owner()->attrAdd(new TFld("wordWrap",trS("Word wrap"),TFld::Boolean,Attr::Mutable,"1","1","","",i2s(A_FormElMixP1).c_str()));
		cfg.owner()->attrAdd(new TFld("confirm",trS("Confirm"),TFld::Boolean,Attr::Mutable,"","1","","",i2s(A_FormElMixP3).c_str()));
		cfg.owner()->attrAdd(new TFld("font",trS("Font"),TFld::String,Attr::Font,"50","Arial 11","","",i2s(A_FormElFont).c_str()));
		break;
	    case F_CHECK_BOX:
		cfg.owner()->attrAdd(new TFld("value",trS("Value"),TFld::Boolean,Attr::Mutable,"","","","",i2s(A_FormElValue).c_str()));
		cfg.owner()->attrAdd(new TFld("font",trS("Font"),TFld::String,Attr::Font,"50","Arial 11","","",i2s(A_FormElFont).c_str()));
		break;
	    case F_BUTTON:
		cfg.owner()->attrAdd(new TFld("value",trS("Repeat"),TFld::String,TFld::TransltText|Attr::Mutable,"","","","",i2s(A_FormElValue).c_str()));
		cfg.owner()->attrAdd(new TFld("img",trS("Image"),TFld::String,Attr::Image|Attr::Mutable,"","","","",i2s(A_FormElMixP1).c_str()));
		cfg.owner()->attrAdd(new TFld("color",trS("Color: button"),TFld::String,Attr::Color|Attr::Mutable,"20","","","",i2s(A_FormElMixP2).c_str()));
		cfg.owner()->attrAdd(new TFld("colorText",trS("Color: text"),TFld::String,Attr::Color|Attr::Mutable,"20","","","",i2s(A_FormElMixP4).c_str()));
		cfg.owner()->attrAdd(new TFld("mode",trS("Mode"),TFld::Integer,TFld::Selectable|Attr::Mutable|Attr::Active,"1","0",
		    TSYS::strMess("%d;%d;%d;%d;%d",FBT_STD,FBT_CHECK,FBT_MENU,FBT_LOAD,FBT_SAVE).c_str(),
		    _("Standard;Checkable;Menu;Load;Save"),i2s(A_FormElMixP3).c_str()));
		cfg.owner()->attrAdd(new TFld("font",trS("Font"),TFld::String,Attr::Font,"50","Arial 11","","",i2s(A_FormElFont).c_str()));
		break;
	    case F_TABLE:
		cfg.owner()->attrAdd(new TFld("value",trS("Value"),TFld::String,TFld::TransltText|Attr::Mutable,"255","","","",i2s(A_FormElValue).c_str()));
		cfg.owner()->attrAdd(new TFld("items",trS("Items"),TFld::String,TFld::FullText|TFld::TransltText|Attr::Mutable,"","","","",i2s(A_FormElMixP1).c_str()));
		cfg.owner()->attrAdd(new TFld("font",trS("Font"),TFld::String,Attr::Font,"50","Arial 11","","",i2s(A_FormElFont).c_str()));
		cfg.owner()->attrAdd(new TFld("set",trS("Set value"),TFld::String,Attr::Mutable,"255","","","",i2s(A_FormElMixP2).c_str()));
		break;
	    case F_LIST:
		cfg.owner()->attrAdd(new TFld("value",trS("Value"),TFld::String,TFld::FullText|TFld::TransltText|Attr::Mutable,"255","","","",i2s(A_FormElValue).c_str()));
		cfg.owner()->attrAdd(new TFld("items",trS("Items"),TFld::String,TFld::FullText|TFld::TransltText|Attr::Mutable,"","","","",i2s(A_FormElMixP1).c_str()));
		cfg.owner()->attrAdd(new TFld("font",trS("Font"),TFld::String,Attr::Font,"50","Arial 11","","",i2s(A_FormElFont).c_str()));
		cfg.owner()->attrAdd(new TFld("mult",trS("Multiple selection"),TFld::Boolean,Attr::Mutable,"1","0","","",i2s(A_FormElMixP2).c_str()));
		break;
	    case F_COMBO: case F_TREE:
		cfg.owner()->attrAdd(new TFld("value",trS("Value"),TFld::String,TFld::TransltText|Attr::Mutable,"255","","","",i2s(A_FormElValue).c_str()));
		cfg.owner()->attrAdd(new TFld("items",trS("Items"),TFld::String,TFld::TransltText|TFld::FullText|Attr::Mutable,"","","","",i2s(A_FormElMixP1).c_str()));
		cfg.owner()->attrAdd(new TFld("font",trS("Font"),TFld::String,Attr::Font,"50","Arial 11","","",i2s(A_FormElFont).c_str()));
		break;
	    case F_SLIDER: case F_SCROLL_BAR:
		cfg.owner()->attrAdd(new TFld("value",trS("Value"),TFld::Integer,Attr::Mutable,"20","0","","",i2s(A_FormElValue).c_str()));
		cfg.owner()->attrAdd(new TFld("cfg",trS("Configuration"),TFld::String,Attr::Mutable,"100","0:0:100:1:10","","",i2s(A_FormElMixP1).c_str()));
		break;
	}
    }
    //Value type change
    else if((cfg.flgGlob()&Attr::Active) && ((cfg.id() == "view" && cfg.owner()->attrAt("elType").at().getI() == F_LINE_ED) ||
					     (cfg.id() == "mode" && cfg.owner()->attrAt("elType").at().getI() == F_BUTTON)))
    {
	unsigned mdfSt = cfg.owner()->attrAt("value").at().aModif_();
	TFld::Type	ntp = TFld::String;
	int		flg = Attr::Mutable;
	Attr::SelfAttrFlgs sflg = cfg.owner()->attrAt("value").at().flgSelf();
	string		val = cfg.owner()->attrAt("value").at().getS();
	string		cfgTmpl = cfg.owner()->attrAt("value").at().cfgTempl();
	string		cfgVal = cfg.owner()->attrAt("value").at().cfgVal();
	string		vName = _("Value");
	if(cfg.id() == "view")
	    switch(cfg.getI()) {
		case FL_INTEGER: case FL_TIME:	ntp = TFld::Integer;	break;
		case FL_REAL:			ntp = TFld::Real;	break;
		case FL_DATE: case FL_DATE_TM:	ntp = TFld::Integer; flg |= Attr::DateTime;	break;
		default: flg |= TFld::TransltText;			break;
	    }
	else	// mode
	    switch(cfg.getI()) {
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
	cfg.owner()->attrAt("value").at().setAModif_(mdfSt);	//!!!! Restore the original modification state
    }

    return Widget::attrChange(cfg, prev);
}

bool OrigFormEl::cntrCmdAttributes( XMLNode *opt, Widget *src )
{
    if(!src) src = this;

    //Get page info
    if(opt->name() == "info") {
	Widget::cntrCmdAttributes(opt, src);
	XMLNode *root, *el;
	int elTp = 0;
	if((root=ctrMkNode("area",opt,-1,"/attr",_("Attributes"))))
	    switch((elTp=src->attrAt("elType").at().getI())) {
		case F_LINE_ED:
		    if((el=ctrId(root,"/font",true)))	el->setAttr("help",Widget::helpFont());
		    if(!(el=ctrId(root,"/cfg",true)))	break;
		    switch(src->attrAt("view").at().getI()) {
			case FL_TEXT:
			    el->setAttr("help",_("Enter text line by the template with the char items:\n"
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
			    el->setAttr("help",_("Integer value configuration in the form: \"{Min}:{Max}:{ChangeStep}:{Prefix}:{Suffix}\"."));
			    break;
			case FL_REAL:
			    el->setAttr("help",_("Real value configuration in the form: \"{Min}:{Max}:{ChangeStep}:{Prefix}:{Suffix}:{SignsAfterDot}\"."));
			    break;
			case FL_TIME: case FL_DATE: case FL_DATE_TM:
			    el->setAttr("help",_("Enter date and/or time by the template with the items:\n"
				"  \"d\" - number of the day (1-31);\n"
				"  \"dd\" - number of the day (01-31);\n"
				"  \"ddd\" - acronym of the day ('Mon' ... 'Sun');\n"
				"  \"dddd\" - the full name of the day ('Monday' ... 'Sunday');\n"
				"  \"M\" - number of the month (1-12);\n"
				"  \"MM\" - number of the month (01-12);\n"
				"  \"MMM\" - acronym of the month ('Jan' ... 'Dec');\n"
				"  \"MMMM\" - the full name of the month ('January' ... 'December');\n"
				"  \"yy\" - the last two digits of the year;\n"
				"  \"yyyy\" - the full year;\n"
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
			_("Operation modes of the button:\n"
			  "  \"Standard\" - standard button with the ability to repeat the events when hold (the parameters in \"value\");\n"
			  "  \"Checkable\" - checkable button (the value in \"value\");\n"
			  "  \"Menu\" - opening menu by click (the list of items in \"value\");\n"
			  "  \"Load\" - provides user downloading of small files through the visualization interface;\n"
			  "             at the push of a button, in this mode, the dialog opens the file selection for download,\n"
			  "             and the content of the selected file is loaded to the attribute \"value\";\n"
			  "  \"Save\" - provides user saving small files through the interface of visualization;\n"
			  "             after the contents of the file have been written to the \"value\" attribute,\n"
			  "             a dialog will be opened for selecting-defining a file to save, after which the contents\n"
			  "             of the attribute \"value\" will be saved to the file and it will be cleared itself."));
		    if((el=ctrId(root,"/value",true)))
			switch(src->attrAt("mode").at().getI()) {
			    case FBT_STD:  el->setAttr("help", _("Parameters of the repeating of the events at holding the button \"{delay}-{interval}\", time in milliseconds."));	break;
			    case FBT_CHECK:el->setAttr("help", _("Value of the toggle."));	break;
			    case FBT_MENU: el->setAttr("help", _("List of menu items in the form \"/grp1/grp2/item1\"."));	break;
			    case FBT_LOAD: el->setAttr("help", _("First line of the descriptor \"{FilesTemplate}|{Header}|{FileByDefaultAndLoaded}|{FileMime}\" and the loaded file content from the next line.\n"
			                                         "The files template writing like to \"Images (*.png *.xpm *.jpg);;CSV-file (*.csv)\"."));	break;
			    case FBT_SAVE: el->setAttr("help", _("First line of the descriptor \"{FilesTemplate}|{Header}|{FileByDefault}|{FileMime}\" and the saving file content from the next line.\n"
			                                         "The files template writing like to \"Images (*.png *.xpm *.jpg);;CSV-file (*.csv)\"."));	break;
			}
		    break;
		case F_COMBO: case F_LIST: case F_TREE:
		    if((el=ctrId(root,"/items",true)))	el->setAttr("help",
			(elTp==F_TREE)?_("List of the hierarchical items by the path \"/{DIR}/{DIR}/{ITEM}\" per line."):_("List of items-values by lines."));
		    if((el=ctrId(root,"/font",true)))	el->setAttr("help",Widget::helpFont());
		    break;
		case F_SLIDER: case F_SCROLL_BAR:
		    if((el=ctrId(root,"/cfg",true))) el->setAttr("help",
			_("Configuration of the slider in the format: \"{VertOrient}:{Min}:{Max}:{SinglStep}:{PageStep}\".\n"
			  "Where:\n"
			  "  \"VertOrient\" - sign of the vertical orientation, the default one is the horizontal orientation;\n"
			  "  \"Min\" - minimum value;\n"
			  "  \"Max\" - maximum value;\n"
			  "  \"SinglStep\" - the size of a single step;\n"
			  "  \"PageStep\" - the size of the page step."));
		    break;
		case F_TABLE:
		    if((el=ctrId(root,"/items",true)))  el->setAttr("SnthHgl","1")->setAttr("help",
			_("XML tag \"tbl\" for the table fill:\n"
			  "  <tbl>\n"
			  "    <h><s>{Header1}</s><s>{Header2}</s></h>\n"
			  "    <r><s>{Row1Column1String}</s><i>{Row1Column1Integer}</i></r>\n"
			  "    <r><b>{Row2Column1Logical}</b><r>{Row2Column2Real}</r></r>\n"
			  "  </tbl>\n"
			  "Tags:\n"
			  "  tbl - Table, the properties at all:\n"
			  "    sel         - selection mode of the table items:\n"
			  "                 \"row\" - by rows, \"col\" - by columns, \"cell\" - by cells (by default);\n"
			  "    keyID       - row-column number of the key, for the selection value get;\n"
			  "    colsWdthFit - fits the columns (in the unfixed size) size to fill the full width of the table;\n"
			  "    hHdrVis, vHdrVis - visibility of the headers, for horizontal and vertical;\n"
			  "    sortEn - enables sorting directly by the columns.\n"
			  "  h   - Row of the headers. Possible attributes of the header cell tag for the column as a whole:\n"
			  "    width - column width, in pixels or percents (10%);\n"
			  "    edit  - allowing to the cells of the row edition (0 or 1), by default - no (0);\n"
			  "    color - column color as a whole into the color name or code;\n"
			  "    colorText - color of the column text as a whole into the color name or code;\n"
			  "    font - font of the column text in the typical OpenSCADA string;\n"
			  "    prec - value precision of the real type cells in the column;\n"
			  "    sort - sorting by the column [0 - Descending; 1 - Ascending];\n"
			  "    align - alignment the column for: \"left\", \"right\" and \"center\".\n"
			  "  r   - Row of the values. Possible attributes of the row cell tag for the row as a whole:\n"
			  "    color - row color as a whole into the color name or code;\n"
			  "    colorText - color of the row text as a whole into the color name or code;\n"
			  "    font - font of the row text in the typical OpenSCADA string;\n"
			  "    prec - value precision of the real type cells in the row.\n"
			  "  s, t, i, r, b - cells of the data types \"String\", \"Text\", \"Integer\", \"Real\" and \"Logical\". Possible attributes:\n"
			  "    color - cell color;\n"
			  "    colorText - color of the cell text into the color name or code;\n"
			  "    font - font of the cell text in the typical OpenSCADA string;\n"
			  "    prec - value precision of the real type cell;\n"
			  "    img - image of the cell into the form \"[{src}:]{name}\";\n"
			  "    edit - allowing the cell edition (0 or 1), by default - no (0);\n"
			  "    align - alignment the cell for: \"left\", \"right\" and \"center\"."));
		    if((el=ctrId(root,"/font",true)))	el->setAttr("help",Widget::helpFont());
		    break;
	    }
	return true;
    }

    //Process command to page
    string a_path = opt->attr("path");
    if(a_path == "/attr/items" && ctrChkNode(opt,"SnthHgl",RWRWR_,"root",SUI_ID,SEC_RD)) {
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

bool OrigFormEl::eventProc( const string &ev, Widget *src )
{
    int elTp = 0;
    switch((elTp=src->attrAt("elType").at().getI())) {
	case F_TABLE: {
	    if(ev.compare(0,13,"ws_TableEdit_") != 0 ||
		    src->attrAt("items").at().getS().size() > limUserFile_SZ)	//Do not apply to the representer after some size
		break;
	    bool setOK = false;
	    int col = s2i(TSYS::strParse(ev,2,"_"));
	    int row = s2i(TSYS::strParse(ev,3,"_"));
	    XMLNode items("tbl");
	    items.load(src->attrAt("items").at().getS(), XMLNode::LD_Full);
	    for(int iChR = 0, iR = 0; iChR < (int)items.childSize() && !setOK; iChR++) {
		XMLNode *chRN = items.childGet(iChR);
		if(chRN->name() != "r") continue;
		for(int iChC = 0, iC = 0; iChC < (int)chRN->childSize() && !setOK; iChC++) {
		    XMLNode *chCN = chRN->childGet(iChC);
		    if(!(chCN->name() == "s" || chCN->name() == "t" || chCN->name() == "r" || chCN->name() == "i" || chCN->name() == "b")) continue;
		    if(iC == col && iR == row) { chCN->setText(src->attrAt("set").at().getS(),true); setOK = true; }
		    iC++;
		}
		iR++;
	    }
	    if(setOK) src->attrAt("items").at().setS(items.save());
	    break;
	}
    }

    return false;
}

//************************************************
//* OrigText: Text element original widget       *
//************************************************
OrigText::OrigText( ) : PrWidget("Text")	{ }

string OrigText::name( ) const	{ return _("Text field"); }

string OrigText::descr( ) const	{ return _("Text field widget of the finite visualization."); }

void OrigText::postEnable( int flag )
{
    LWidget::postEnable(flag);

    if(flag&TCntrNode::NodeConnect) {
	attrAdd(new TFld("backColor",trS("Background: color"),TFld::String,Attr::Color,"","","","",i2s(A_BackColor).c_str()));
	attrAdd(new TFld("backImg",trS("Background: image"),TFld::String,Attr::Image,"","","","",i2s(A_BackImg).c_str()));
	attrAdd(new TFld("bordWidth",trS("Border: width"),TFld::Integer,TFld::NoFlag,"","0","","",i2s(A_BordWidth).c_str()));
	attrAdd(new TFld("bordColor",trS("Border: color"),TFld::String,Attr::Color,"","#000000","","",i2s(A_BordColor).c_str()));
	attrAdd(new TFld("bordStyle",trS("Border: style"),TFld::Integer,TFld::Selectable,"","3",
	    TSYS::strMess("%d;%d;%d;%d;%d;%d;%d;%d;%d",FBRD_NONE,FBRD_DOT,FBRD_DASH,FBRD_SOL,FBRD_DBL,FBRD_GROOVE,FBRD_RIDGE,FBRD_INSET,FBRD_OUTSET).c_str(),
	    _("None;Dotted;Dashed;Solid;Double;Groove;Ridge;Inset;Outset"),i2s(A_BordStyle).c_str()));
	attrAdd(new TFld("font",trS("Font"),TFld::String,Attr::Font,"50","Arial 11","","",i2s(A_TextFont).c_str()));
	attrAdd(new TFld("color",trS("Color"),TFld::String,Attr::Color,"20","#000000","","",i2s(A_TextColor).c_str()));
	attrAdd(new TFld("orient",trS("Orientation angle"),TFld::Integer,TFld::NoFlag,"3","0","-360;360","",i2s(A_TextOrient).c_str()));
	attrAdd(new TFld("wordWrap",trS("Word wrap"),TFld::Boolean,TFld::NoFlag,"1","1","","",i2s(A_TextWordWrap).c_str()));
	attrAdd(new TFld("alignment",trS("Alignment"),TFld::Integer,TFld::Selectable,"1","0",
	    TSYS::strMess("%d;%d;%d;%d;%d;%d;%d;%d;%d;%d;%d;%d",FT_TOP_LEFT,FT_TOP_RIGHT,FT_TOP_CENTER,FT_TOP_JUST,
		FT_BT_LEFT,FT_BT_RIGHT,FT_BT_CENTER,FT_BT_JUST,FT_CNTR_LEFT,FT_CNTR_RIGHT,FT_CNTR,FT_CNTR_JUST).c_str(),
			    _("Top left;Top right;Top center;Top justify;"
			    "Bottom left;Bottom right;Bottom center;Bottom justify;"
			    "V center left;V center right;Center;V center justify"),i2s(A_TextAlignment).c_str()));
	attrAdd(new TFld("inHtml",trS("In HTML"),TFld::Boolean,Attr::Active,"1","0","","",i2s(A_TextHTML).c_str()));
	attrAdd(new TFld("text",trS("Text"),TFld::String,TFld::TransltText|TFld::FullText,"","Text","","",i2s(A_TextText).c_str()));
	attrAdd(new TFld("numbArg",trS("Arguments number"),TFld::Integer,Attr::Active,"","0","0;20","",i2s(A_TextNumbArg).c_str()));
    }
}

bool OrigText::attrChange( Attr &cfg, TVariant prev )
{
    if(cfg.flgGlob()&Attr::Active) {
	int aid = s2i(cfg.fld().reserve());
	if(cfg.id() == "numbArg") {
	    string fid("arg"), fnm(_("Argument ")), fidp, fnmp;
	    //Delete specific unnecessary attributes of parameters
	    for(int iP = 0; true; iP++) {
		fidp = fid+i2s(iP);
		if(!cfg.owner()->attrPresent(fidp+"val")) break;
		else if(iP >= cfg.getI()) {
		    cfg.owner()->attrDel(fidp+"val");
		    cfg.owner()->attrDel(fidp+"tp");
		    cfg.owner()->attrDel(fidp+"cfg");
		}
	    }
	    //Create ullage attributes of parameters
	    for(int iP = 0; iP < cfg.getI(); iP++) {
		fidp = fid+i2s(iP);
		fnmp = fnm+i2s(iP);
		if(cfg.owner()->attrPresent(fidp+"val")) continue;
		cfg.owner()->attrAdd(new TFld((fidp+"tp").c_str(),(fnmp+_(": type")).c_str(),
		    TFld::Real,TFld::Selectable|Attr::Mutable|Attr::Active,"","0",
			TSYS::strMess("%d;%d;%d",FT_INT,FT_REAL,FT_STR).c_str(),_("Integer;Real;String"),
			i2s(A_TextArs+A_TextArsTp+A_TextArsSz*iP).c_str()));
		cfg.owner()->attrAdd(new TFld((fidp+"val").c_str(),(fnmp+_(": value")).c_str(),
		    TFld::Integer,Attr::Mutable,"","","","",i2s(A_TextArs+A_TextArsVal+A_TextArsSz*iP).c_str()));
		cfg.owner()->attrAdd(new TFld((fidp+"cfg").c_str(),(fnmp+_(": config")).c_str(),
		    TFld::String,Attr::Mutable,"","","","",i2s(A_TextArs+A_TextArsCfg+A_TextArsSz*iP).c_str()));
	    }
	}
	else if(aid >= A_TextArs && (aid%A_TextArsSz) == A_TextArsTp && prev.getI() != cfg.getI()) {
	    int narg = (aid-A_TextArs)/A_TextArsSz;
	    string fid = "arg"+i2s(narg)+"val";
	    string fnm = _("Argument ")+i2s(narg)+_(": value");
	    int apos = cfg.owner()->attrPos(fid);
	    VCA::Attr::SelfAttrFlgs sflg =  cfg.owner()->attrAt(fid).at().flgSelf();
	    cfg.owner()->attrDel(fid);
	    cfg.owner()->attrAdd(new TFld(fid.c_str(),fnm.c_str(),
			(cfg.getI()==1) ? TFld::Real : ((cfg.getI()==2) ? TFld::String : TFld::Integer),
			((cfg.getI()==2)?TFld::TransltText:0)|Attr::Mutable,"","","","",i2s(A_TextArs+A_TextArsSz*narg).c_str()), apos);
	    cfg.owner()->attrAt(fid).at().setFlgSelf(sflg);
	}
    }
    return Widget::attrChange(cfg,prev);
}

bool OrigText::cntrCmdAttributes( XMLNode *opt, Widget *src )
{
    if(!src) src = this;

    //Get page info
    if(opt->name() == "info") {
	Widget::cntrCmdAttributes(opt, src);
	XMLNode *root, *el;
	if((root=ctrMkNode("area",opt,-1,"/attr",_("Attributes")))) {
	    for(unsigned iCh = 0; iCh < root->childSize(); iCh++) {
		el = root->childGet(iCh);
		int p = s2i(el->attr("p"));
		switch(p) {
		    case A_BackColor: case A_BordColor: case A_TextColor: el->setAttr("help",Widget::helpColor());	break;
		    case A_TextFont: el->setAttr("help",Widget::helpFont());	break;
		    case A_BackImg:  el->setAttr("help",Widget::helpImg());	break;
		    case A_TextText: {
			bool inHTML = src->attrAt("inHtml").at().getB();
			if(inHTML) el->setAttr("SnthHgl","1")->setAttr("help",_("Text value in HTML. Use \"%{x}\" for argument \"x\" (from 1) value insert."));
			else el->setAttr("SnthHgl","1")->setAttr("help",_("Text value. Use \"%{x}\" for argument \"x\" (from 1) value insert."));
			break;
		    }
		}
	    }
	    for(int iArg = 0; iArg < src->attrAt("numbArg").at().getI(); iArg++) {
		if(!(el=ctrId(root,"/arg"+i2s(iArg)+"cfg",true))) continue;
		switch(src->attrAt("arg"+i2s(iArg)+"tp").at().getI()) {
		    case A_TextArsVal:	el->setAttr("help",_("Integer value configuration in the form \"{len}\"."));break;
		    case A_TextArsTp:	el->setAttr("help",_("Real value configuration in the form: \"{width};{form};{prec}\".\n"
							     "Where \"form\" is 'g', 'e' or 'f'."));		break;
		    case A_TextArsCfg:	el->setAttr("help",_("String value configuration in the form \"{len}\"."));	break;
		}
	    }
	}
	return true;
    }

    //Process command to page
    string a_path = opt->attr("path");
    if(a_path == "/attr/text" && ctrChkNode(opt,"SnthHgl",RWRWR_,"root",SUI_ID,SEC_RD)) {
	opt->childAdd("rule")->setAttr("expr","%[0-9]")->setAttr("color","darkcyan");
	if(src->attrAt("inHtml").at().getB()) {
	    opt->childAdd("blk")->setAttr("beg","<!--")->setAttr("end","-->")->setAttr("color","gray")->setAttr("font_italic","1");
	    XMLNode *tag = opt->childAdd("blk")->setAttr("beg","<\\?")->setAttr("end","\\?>")->setAttr("color","#666666");
	    tag = opt->childAdd("blk")->setAttr("beg","<\\w+")->setAttr("end","\\/?>")->setAttr("font_weight","1");
		tag->childAdd("rule")->setAttr("expr","\\b\\w+[ ]*(?==)")->setAttr("color","blue");
		tag->childAdd("rule")->setAttr("expr","[ ]?\"[^\"]+\"")->setAttr("color","darkgreen");
		tag->childAdd("rule")->setAttr("expr","[ ]?'[^']+'")->setAttr("color","darkgreen");
	    opt->childAdd("rule")->setAttr("expr","\\%\\d*);")->setAttr("color","cyan");
	    opt->childAdd("rule")->setAttr("expr","<\\/[\\w]+>")->setAttr("font_weight","1");
	    opt->childAdd("rule")->setAttr("expr","&([a-zA-Z]*|#\\d*);")->setAttr("color","#AF7E00");
	}
    }
    else return Widget::cntrCmdAttributes(opt, src);

    return true;
}

//************************************************
//* OrigMedia: Media view original widget        *
//************************************************
OrigMedia::OrigMedia( ) : PrWidget("Media")	{ }

string OrigMedia::name( ) const	{ return _("Media"); }

string OrigMedia::descr( ) const{ return _("Media widget of the finite visualization."); }

void OrigMedia::postEnable( int flag )
{
    LWidget::postEnable(flag);

    if(flag&TCntrNode::NodeConnect) {
	attrAdd(new TFld("backColor",trS("Background: color"),TFld::String,Attr::Color,"","#FFFFFF","","",i2s(A_BackColor).c_str()));
	attrAdd(new TFld("backImg",trS("Background: image"),TFld::String,Attr::Image,"","","","",i2s(A_BackImg).c_str()));
	attrAdd(new TFld("bordWidth",trS("Border: width"),TFld::Integer,TFld::NoFlag,"","0","","",i2s(A_BordWidth).c_str()));
	attrAdd(new TFld("bordColor",trS("Border: color"),TFld::String,Attr::Color,"","#000000","","",i2s(A_BordColor).c_str()));
	attrAdd(new TFld("bordStyle",trS("Border: style"),TFld::Integer,TFld::Selectable,"","3",
	    TSYS::strMess("%d;%d;%d;%d;%d;%d;%d;%d;%d",FBRD_NONE,FBRD_DOT,FBRD_DASH,FBRD_SOL,FBRD_DBL,FBRD_GROOVE,FBRD_RIDGE,FBRD_INSET,FBRD_OUTSET).c_str(),
	    _("None;Dotted;Dashed;Solid;Double;Groove;Ridge;Inset;Outset"),i2s(A_BordStyle).c_str()));
	attrAdd(new TFld("src",trS("Source"),TFld::String,0,"50","","","",i2s(A_MediaSrc).c_str()));
	attrAdd(new TFld("type",trS("Type"),TFld::Integer,TFld::Selectable|Attr::Active,"1","0",
	    TSYS::strMess("%d;%d;%d;%d",FM_IMG,FM_ANIM,FM_VIDEO,FM_AUDIO).c_str(),_("Image;Animation;Video;Audio"),i2s(A_MediaType).c_str()));
	attrAdd(new TFld("areas",trS("Map areas"),TFld::Integer,Attr::Active,"2","0","0;100","",i2s(A_MediaAreas).c_str()));
    }
}

bool OrigMedia::attrChange( Attr &cfg, TVariant prev )
{
    if(cfg.flgGlob()&Attr::Active) {
	if(cfg.id() == "type") {
	    //Delete specific attributes
	    switch(prev.getI()) {
		case FM_IMG:
		    cfg.owner()->attrDel("fit");
		    break;
		case FM_ANIM:
		    cfg.owner()->attrDel("fit");
		    cfg.owner()->attrDel("speed");
		    break;
		case FM_VIDEO: case FM_AUDIO:
		    cfg.owner()->attrDel("play");
		    cfg.owner()->attrDel("roll");
		    cfg.owner()->attrDel("pause");
		    cfg.owner()->attrDel("size");
		    cfg.owner()->attrDel("seek");
		    cfg.owner()->attrDel("volume");
		    break;
	    }

	    //Create specific attributes
	    switch(cfg.getI()) {
		case FM_IMG:
		    cfg.owner()->attrAdd(new TFld("fit",trS("Fit to the widget size"),TFld::Boolean,Attr::Mutable,"","","","",i2s(A_MediaFit).c_str()));
		    break;
		case FM_ANIM:
		    cfg.owner()->attrAdd(new TFld("fit",trS("Fit to the widget size"),TFld::Boolean,Attr::Mutable,"","","","",i2s(A_MediaFit).c_str()));
		    cfg.owner()->attrAdd(new TFld("speed",trS("Play speed"),TFld::Integer,Attr::Mutable,"","100","1;900","",i2s(A_MediaSpeedPlay).c_str()));
		    break;
		case FM_VIDEO: case FM_AUDIO:
		    cfg.owner()->attrAdd(new TFld("play",trS("Play"),TFld::Boolean,Attr::Mutable,"","0","","",i2s(A_MediaSpeedPlay).c_str()));
		    cfg.owner()->attrAdd(new TFld("roll",trS("Roll play"),TFld::Boolean,Attr::Mutable,"","0","","",i2s(A_MediaRoll).c_str()));
		    cfg.owner()->attrAdd(new TFld("pause",trS("Pause"),TFld::Boolean,Attr::Mutable,"","0","","",i2s(A_MediaPause).c_str()));
		    cfg.owner()->attrAdd(new TFld("size",trS("Size"),TFld::Real,Attr::Mutable,"","0","0;1e300","",i2s(A_MediaSize).c_str()));
		    cfg.owner()->attrAdd(new TFld("seek",trS("Seek"),TFld::Real,Attr::Mutable,"","0","0;1e300","",i2s(A_MediaSeek).c_str()));
		    cfg.owner()->attrAdd(new TFld("volume",trS("Volume"),TFld::Real,Attr::Mutable,"","50","0;100","",i2s(A_MediaVolume).c_str()));
		    break;
	    }
	}
	else if(cfg.id() == "areas") {
	    string fid("area"), fnm(_("Area ")), fidp, fnmp;
	    //Delete specific unnecessary attributes of map areas
	    for(int iP = 0; true; iP++) {
		fidp = fid+i2s(iP);
		if(!cfg.owner()->attrPresent(fidp+"shp"))	break;
		else if(iP >= cfg.getI()) {
		    cfg.owner()->attrDel(fidp+"shp");
		    cfg.owner()->attrDel(fidp+"coord");
		    cfg.owner()->attrDel(fidp+"title");
		}
	    }
	    //Create ullage attributes of map areas
	    for(int iP = 0; iP < cfg.getI(); iP++) {
		fidp = fid+i2s(iP);
		fnmp = fnm+i2s(iP);
		if(cfg.owner()->attrPresent(fidp+"shp")) continue;
		cfg.owner()->attrAdd(new TFld((fidp+"shp").c_str(),(fnmp+_(": shape")).c_str(),TFld::Integer,TFld::Selectable|Attr::Mutable,"1","0",
		    TSYS::strMess("%d;%d;%d",FM_RECT,FM_POLY,FM_CIRCLE).c_str(),_("Rect;Poly;Circle"),i2s(A_MediaArs+A_MediaArShape+A_MediaArsSz*iP).c_str()));
		cfg.owner()->attrAdd(new TFld((fidp+"coord").c_str(),(fnmp+_(": coordinates")).c_str(),TFld::String,Attr::Mutable,"","",
		    "","",i2s(A_MediaArs+A_MediaArCoord+A_MediaArsSz*iP).c_str()));
		cfg.owner()->attrAdd(new TFld((fidp+"title").c_str(),(fnmp+_(": title")).c_str(),TFld::String,TFld::TransltText|Attr::Mutable,"","",
		    "","",i2s(A_MediaArs+A_MediaArTitle+A_MediaArsSz*iP).c_str()));
	    }
	}
    }

    return Widget::attrChange(cfg,prev);
}

bool OrigMedia::cntrCmdAttributes( XMLNode *opt, Widget *src )
{
    if(!src) src = this;

    //Get page info
    if(opt->name() == "info") {
	Widget::cntrCmdAttributes(opt,src);
	XMLNode *root, *el;
	if((root=ctrMkNode("area",opt,-1,"/attr",_("Attributes")))) {
	    for(unsigned iCh = 0; iCh < root->childSize(); iCh++) {
		el = root->childGet(iCh);
		int p = s2i(el->attr("p"));
		switch(p) {
		    case A_BackColor: case A_BordColor: el->setAttr("help",Widget::helpColor());	break;
		    case A_BackImg: el->setAttr("help",Widget::helpImg());		break;
		    case A_MediaSrc: el->setAttr("dest","sel_ed")->setAttr("select","/attrImg/sel_"+el->attr("id"))->setAttr("help",
			_("Name of the media source in the form \"[{src}:]{name}\", where:\n"
			"  \"src\" - source:\n"
			"    file - directly from a local file (for visualizator or engine) by the path;\n"
			"    res - from table of DB-mime resources;\n"
			"    stream - stream URL to play video and audio.\n"
			"  \"name\" - file path or mime-resource identifier.\n"
			"Examples:\n"
			"  \"res:workMedia\" - from the table of DB-mime resources for the ID \"workMedia\";\n"
			"  \"workMedia\" - like to the previous;\n"
			"  \"file:/var/tmp/workMedia.mng\" - from a local file by the path \"/var/tmp/workMedia.mng\";\n"
			"  \"stream:http://localhost.localhost:5050\" - playing the stream video and audio from the URL."));
			break;
		    case A_MediaType: el->setAttr("help",
			_("Media type variant:\n"
			"  \"Image\" - raster or vector image, like to: PNG, JPEG, GIF, SVG;\n"
			"  \"Animation\" - simple animation image, like to: GIF, MNG;\n"
			"  \"Full video\" - full video, audio or stream, like to: OGG, OGM, AVI, MKV, MPG, MP3, MP4."));
			break;
		}
	    }
	    for(int iA = 0; iA < src->attrAt("areas").at().getI(); iA++) {
		el = ctrId(root,TSYS::strMess("/area%dcoord",iA),true);
		if(!el) continue;
		switch(src->attrAt(TSYS::strMess("area%dshp",iA)).at().getI()) {
		    case FM_RECT: el->setAttr("help",_("Rectangle area in the form \"x1,y1,x2,y2\"."));	break;
		    case FM_POLY: el->setAttr("help",_("Polygon area in the form \"x1,y1,x2,y2,xN,yN\".")); break;
		    case FM_CIRCLE: el->setAttr("help",_("Circle area in the form \"x,y,r\"."));	break;
		}
	    }
	}
	return true;
    }

    //Process command to page
    return Widget::cntrCmdAttributes(opt,src);
}

//************************************************
//* OrigDiagram: Diagram original widget         *
//************************************************
OrigDiagram::OrigDiagram( ) : PrWidget("Diagram")	{ }

string OrigDiagram::name( ) const	{ return _("Diagram"); }

string OrigDiagram::descr( ) const	{ return _("Diagram widget of the finite visualization."); }

void OrigDiagram::postEnable( int flag )
{
    LWidget::postEnable(flag);

    if(flag&TCntrNode::NodeConnect) {
	attrAdd(new TFld("backColor",trS("Background: color"),TFld::String,Attr::Color,"","black","","",i2s(A_BackColor).c_str()));
	attrAdd(new TFld("backImg",trS("Background: image"),TFld::String,Attr::Image,"","","","",i2s(A_BackImg).c_str()));
	attrAdd(new TFld("bordWidth",trS("Border: width"),TFld::Integer,TFld::NoFlag,"","0","","",i2s(A_BordWidth).c_str()));
	attrAdd(new TFld("bordColor",trS("Border: color"),TFld::String,Attr::Color,"","#000000","","",i2s(A_BordColor).c_str()));
	attrAdd(new TFld("bordStyle",trS("Border: style"),TFld::Integer,TFld::Selectable,"","3",
	    TSYS::strMess("%d;%d;%d;%d;%d;%d;%d;%d;%d",FBRD_NONE,FBRD_DOT,FBRD_DASH,FBRD_SOL,FBRD_DBL,FBRD_GROOVE,FBRD_RIDGE,FBRD_INSET,FBRD_OUTSET).c_str(),
	    _("None;Dotted;Dashed;Solid;Double;Groove;Ridge;Inset;Outset"),i2s(A_BordStyle).c_str()));
	attrAdd(new TFld("trcPer",trS("Tracing period, seconds"),TFld::Real,TFld::NoFlag,"","0","0;360","",i2s(A_DiagramTrcPer).c_str()));
	attrAdd(new TFld("type",trS("Type"),TFld::Integer,TFld::Selectable|Attr::Active,"1","0",
	    TSYS::strMess("%d;%d;%d",FD_TRND,FD_SPECTR,FD_XY).c_str(),_("Trend;Spectrum;XY"),i2s(A_DiagramType).c_str()));
    }
}

bool OrigDiagram::attrChange( Attr &cfg, TVariant prev )
{
    if(!(cfg.flgGlob()&Attr::Active))	return Widget::attrChange(cfg, prev);

    if(cfg.id() == "active" && cfg.getB() != prev.getB()) {
	if(!cfg.getB()) {
	    cfg.owner()->attrDel("curSek");
	    cfg.owner()->attrDel("curUSek");
	    cfg.owner()->attrDel("curColor");
	}
	else {
	    cfg.owner()->attrAdd(new TFld("curSek",trS("Cursor: seconds"),TFld::Integer,Attr::DateTime|Attr::Mutable,"","","","",i2s(A_DiagramCurSek).c_str()));
	    cfg.owner()->attrAdd(new TFld("curUSek",trS("Cursor: microseconds"),TFld::Integer,Attr::Mutable,"","","","",i2s(A_DiagramCurUSek).c_str()));
	    cfg.owner()->attrAdd(new TFld("curColor",trS("Cursor: color"),TFld::String,Attr::Color|Attr::Mutable,"","white","","",i2s(A_DiagramCurColor).c_str()));
	}
    }
    else if(cfg.id() == "type") {
	//Delete specific attributes
	switch(prev.getI()) {
	    case FD_TRND:
		if(!(cfg.getI() == FD_TRND))	cfg.owner()->attrDel("sclHorPer");
	}

	// Create specific attributes
	switch(cfg.getI()) {
	    case FD_TRND:
		cfg.owner()->attrAdd(new TFld("sclHorPer",trS("Scale: horizontal grid size, seconds"),TFld::Real,Attr::Mutable,
		    "","0","0;3e6","",i2s(A_DiagramSclHorPer).c_str()));
	    case FD_SPECTR: case FD_XY:
		cfg.owner()->attrAdd(new TFld("tSek",trS("Time: seconds"),TFld::Integer,Attr::DateTime|Attr::Mutable,"","","","",i2s(A_DiagramTSek).c_str()));
		cfg.owner()->attrAdd(new TFld("tUSek",trS("Time: microseconds"),TFld::Integer,Attr::Mutable,"","","","",i2s(A_DiagramTUSek).c_str()));
		cfg.owner()->attrAdd(new TFld("tSize",trS("Size, seconds"),TFld::Real,Attr::Mutable,"","60","0;32e6","",i2s(A_DiagramTSize).c_str()));
		if(cfg.owner()->attrAt("active").at().getB()) {
		    cfg.owner()->attrAdd(new TFld("curSek",trS("Cursor: seconds"),TFld::Integer,Attr::DateTime|Attr::Mutable,"","","","",i2s(A_DiagramCurSek).c_str()));
		    cfg.owner()->attrAdd(new TFld("curUSek",trS("Cursor: microseconds"),TFld::Integer,Attr::Mutable,"","","","",i2s(A_DiagramCurUSek).c_str()));
		    cfg.owner()->attrAdd(new TFld("curColor",trS("Cursor: color"),TFld::String,Attr::Color|Attr::Mutable,"","white","","",i2s(A_DiagramCurColor).c_str()));
		}
		cfg.owner()->attrAdd(new TFld("sclColor",trS("Scale: color"),TFld::String,Attr::Color|Attr::Mutable,"","grey","","",i2s(A_DiagramSclColor).c_str()));
		cfg.owner()->attrAdd(new TFld("sclHor",trS("Scale: horizontal"),TFld::Integer,Attr::Mutable|TFld::Selectable,"1",i2s(FD_NO).c_str(),
		    TSYS::strMess("%d;%d;%d;%d;%d;%d;%d",FD_NO,FD_GRD,FD_MARKS,FD_GRD_MARKS,FD_GRD_LOG,FD_MARKS_LOG,FD_GRD_MARKS_LOG).c_str(),
		    _("No draw;Grid;Markers;Grid and markers;Grid (log);Markers (log);Grid and markers (log)"),i2s(A_DiagramSclHor).c_str()));
		cfg.owner()->attrAdd(new TFld("sclHorScl",trS("Scale: horizontal scale (%)"),TFld::Real,Attr::Mutable,"","100","10;1000","",i2s(A_DiagramSclHorScl).c_str()));
		cfg.owner()->attrAdd(new TFld("sclHorSclOff",trS("Scale: horizontal scale offset (%)"),TFld::Real,Attr::Mutable,"","0","-100;100","",i2s(A_DiagramSclHorSclOff).c_str()));
		cfg.owner()->attrAdd(new TFld("sclVer",trS("Scale: vertical"),TFld::Integer,Attr::Mutable|TFld::Selectable,"1",i2s(FD_NO).c_str(),
		    TSYS::strMess("%d;%d;%d;%d;%d;%d;%d",FD_NO,FD_GRD,FD_MARKS,FD_GRD_MARKS,FD_GRD_LOG,FD_MARKS_LOG,FD_GRD_MARKS_LOG).c_str(),
		    _("No draw;Grid;Markers;Grid and markers;Grid (log);Markers (log);Grid and markers (log)"),i2s(A_DiagramSclVer).c_str()));
		cfg.owner()->attrAdd(new TFld("sclVerScl",trS("Scale: vertical scale (%)"),TFld::Real,Attr::Mutable,"","100","10;1000","",i2s(A_DiagramSclVerScl).c_str()));
		cfg.owner()->attrAdd(new TFld("sclVerSclOff",trS("Scale: vertical scale offset (%)"),TFld::Real,Attr::Mutable,"","0","-100;100","",i2s(A_DiagramSclVerSclOff).c_str()));
		cfg.owner()->attrAdd(new TFld("sclMarkColor",trS("Scale: Markers: color"),TFld::String,Attr::Color|Attr::Mutable,"","white","","",i2s(A_DiagramSclMarkColor).c_str()));
		cfg.owner()->attrAdd(new TFld("sclMarkFont",trS("Scale: Markers: font"),TFld::String,Attr::Font|Attr::Mutable,"","Arial 10","","",i2s(A_DiagramSclMarkFont).c_str()));
		cfg.owner()->attrAdd(new TFld("valArch",trS("Value archiver"),TFld::String,Attr::Mutable,"","","","",i2s(A_DiagramValArch).c_str()));
		cfg.owner()->attrAdd(new TFld("valsForPix",trS("Values per pixel"),TFld::Integer,Attr::Mutable,"","1","1;100","",i2s(A_DiagramValsForPix).c_str()));
		cfg.owner()->attrAdd(new TFld("parNum",trS("Parameters number"),TFld::Integer,Attr::Mutable|Attr::Active,"","1","0;100","",i2s(A_DiagramParNum).c_str()));
		break;
	}
    }
    else if(cfg.id() == "parNum") {
	string fid("prm"), fnm(_("Parameter ")), fidp, fnmp;
	//Delete specific unnecessary attributes of parameters
	for(int iP = 0; true; iP++) {
	    fidp = fid+i2s(iP);
	    if(!cfg.owner()->attrPresent(fidp+"addr"))	break;
	    else if(iP >= cfg.getI()) {
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
	//Create ullage attributes of parameters
	for(int iP = 0; iP < cfg.getI(); iP++) {
	    fidp = fid+i2s(iP);
	    fnmp = fnm+i2s(iP);
	    if(cfg.owner()->attrPresent(fidp+"addr")) continue;
	    cfg.owner()->attrAdd(new TFld((fidp+"addr").c_str(),(fnmp+_(": address")).c_str(),
		TFld::String,Attr::Address|Attr::Mutable,"","","","",i2s(A_DiagramTrs+A_DiagramTrAddr+A_DiagramTrsSz*iP).c_str()));
	    cfg.owner()->attrAdd(new TFld((fidp+"bordL").c_str(),(fnmp+_(": view border: lower")).c_str(),
		TFld::Real,Attr::Mutable,"","","","",i2s(A_DiagramTrs+A_DiagramTrBordL+A_DiagramTrsSz*iP).c_str()));
	    cfg.owner()->attrAdd(new TFld((fidp+"bordU").c_str(),(fnmp+_(": view border: upper")).c_str(),
		TFld::Real,Attr::Mutable,"","","","",i2s(A_DiagramTrs+A_DiagramTrBordU+A_DiagramTrsSz*iP).c_str()));
	    cfg.owner()->attrAdd(new TFld((fidp+"color").c_str(),(fnmp+_(": color")).c_str(),
		TFld::String,Attr::Color|Attr::Mutable,"","","","",i2s(A_DiagramTrs+A_DiagramTrClr+A_DiagramTrsSz*iP).c_str()));
	    cfg.owner()->attrAdd(new TFld((fidp+"width").c_str(),(fnmp+_(": width")).c_str(),
		TFld::Integer,Attr::Mutable,"","1","1;10","",i2s(A_DiagramTrs+A_DiagramTrWdth+A_DiagramTrsSz*iP).c_str()));
	    cfg.owner()->attrAdd(new TFld((fidp+"scl").c_str(),(fnmp+_(": scale")).c_str(),TFld::Integer,Attr::Mutable|TFld::Selectable,
		"","0","0;2;3;6;7",_("Global;Markers;Grid and markers;Markers (log);Grid and markers (log)"),
		i2s(A_DiagramTrs+A_DiagramTrScl+A_DiagramTrsSz*iP).c_str()));
	    cfg.owner()->attrAdd(new TFld((fidp+"val").c_str(),(fnmp+_(": value")).c_str(),
		TFld::Real,Attr::Mutable,"","","","",i2s(A_DiagramTrs+A_DiagramTrVal+A_DiagramTrsSz*iP).c_str()));
	    cfg.owner()->attrAdd(new TFld((fidp+"prop").c_str(),(fnmp+_(": properties")).c_str(),
		TFld::String,Attr::Mutable,"","","","",i2s(A_DiagramTrs+A_DiagramTrProp+A_DiagramTrsSz*iP).c_str()));
	}
    }

    return Widget::attrChange(cfg,prev);
}

bool OrigDiagram::cntrCmdAttributes( XMLNode *opt, Widget *src )
{
    if(!src) src = this;

    //Get page info
    if(opt->name() == "info") {
	Widget::cntrCmdAttributes(opt,src);
	XMLNode *root, *el;
	if((root=ctrMkNode("area",opt,-1,"/attr",_("Attributes")))) {
	    for(unsigned iCh = 0; iCh < root->childSize(); iCh++) {
		el = root->childGet(iCh);
		int p = s2i(el->attr("p"));
		switch(p) {
		    case A_BackColor: case A_BordColor: case A_DiagramSclColor: case A_DiagramSclMarkColor:
			el->setAttr("help",Widget::helpColor());	break;
		    case A_DiagramSclMarkFont: el->setAttr("help",Widget::helpFont());	break;
		    case A_BackImg: el->setAttr("help",Widget::helpImg());	break;
		    case A_DiagramValArch:
			el->setAttr("help",_("Value archiver in the form \"{ArchMod}.{ArchiverId}\"."));	break;
		    case A_DiagramValsForPix:
			el->setAttr("help",_("Number of the values per pixel. Increase to enhance the export accuracy at large time intervals."));
			break;
		}
	    }
	    for(int iP = 0; iP < src->attrAt("parNum").at().getI(); iP++) {
		if((el=ctrId(root,TSYS::strMess("/prm%dcolor",iP),true))) el->setAttr("help",Widget::helpColor());
		if((el=ctrId(root,TSYS::strMess("/prm%daddr",iP),true))) el->setAttr("help",
		    _("Full address to the DAQ attribute of the parameter or to the archive.\n"
		      "Data installation by the direct prefix is supported:\n"
		      "  \"data:{XMLNodeData}\" - drawing from the direct set data;\n"
		      "  \"line:{value}\" - drawing horizontal line by the value, no sense have for the type \"XY\".\n"
		      "Example:\n"
		      "  \"/DAQ/System/AutoDA/MemInfo/use\" - address to the attribute \"use\" of the parameter \"MemInfo\"\n"
		      "    of the controller \"AutoDA\" of the DAQ module \"System\";\n"
		      "  \"/Archive/va_CPULoad_load\" - address to the archive \"CPULoad_load\";\n"
		      "  \"data:<d s=\"1\" tm=\"1369465209\" tm_grnd=\"1369465200\" per=\"1\">\n"
		      "    0 3.14\n"
		      "    1 3.141\n"
		      "    5 3.1415</d> - data for 10 seconds and period in 1 second from \"25.05.2013 10:00:00\";\n"
		      "      in the absence of \"tm\" and \"tm_grnd\" they will be set from the range of the diagram,\n"
		      "      and setting the attribute \"s\" will allow you to specify the time in seconds;\n"
		      "  \"line:3.14159265\" - horizontal line for the value \"3.14159265\"."));
		if((el=ctrId(root,TSYS::strMess("/prm%dprop",iP),true))) el->setAttr("help",
		    _("Actual archive properties in the form \"{BegArh}:{EndArh}:{DataPeriod}\", where:\n"
		      "  \"BegArh\", \"EndArh\", \"DataPeriod\" - begin, end and period of data of the archive in seconds,\n"
		      "                  it is the real value up to the microseconds deep (1e-6)."));
	    }
	}
	return true;
    }

    //Process command to page
    return Widget::cntrCmdAttributes(opt,src);
}

//************************************************
//* OrigProtocol: Protocol original widget       *
//************************************************
OrigProtocol::OrigProtocol( ) : PrWidget("Protocol")	{ }

string OrigProtocol::name( ) const	{ return _("Protocol"); }

string OrigProtocol::descr( ) const	{ return _("Protocol widget of the finite visualization."); }

void OrigProtocol::postEnable( int flag )
{
    LWidget::postEnable(flag);

    if(flag&TCntrNode::NodeConnect) {
	attrAdd(new TFld("backColor",trS("Background: color"),TFld::String,Attr::Color,"","","","",i2s(A_BackColor).c_str()));
	attrAdd(new TFld("backImg",trS("Background: image"),TFld::String,Attr::Image,"","","","",i2s(A_BackImg).c_str()));
	attrAdd(new TFld("font",trS("Font"),TFld::String,Attr::Font,"","Arial 11","","",i2s(A_ProtFont).c_str()));
	attrAdd(new TFld("headVis",trS("Header visible"),TFld::Boolean,TFld::NoFlag,"","1","","",i2s(A_ProtHeadVis).c_str()));
	attrAdd(new TFld("time",trS("Time, seconds"),TFld::Integer,Attr::DateTime,"","","","",i2s(A_ProtTime).c_str()));
	attrAdd(new TFld("tSize",trS("Size, seconds"),TFld::Integer,TFld::NoFlag,"","60","0;50000000","",i2s(A_ProtTSize).c_str()));
	attrAdd(new TFld("trcPer",trS("Tracing period, seconds"),TFld::Integer,TFld::NoFlag,"","0","0;360","",i2s(A_ProtTrcPer).c_str()));
	attrAdd(new TFld("arch",trS("Archiver"),TFld::String,0,"","","","",i2s(A_ProtArch).c_str()));
	attrAdd(new TFld("tmpl",trS("Template"),TFld::String,0,"","","","",i2s(A_ProtTmpl).c_str()));
	attrAdd(new TFld("lev",trS("Level"),TFld::Integer,TFld::NoFlag,"","0","-7;7","",i2s(A_ProtLev).c_str()));
	attrAdd(new TFld("viewOrd",trS("View order"),TFld::Integer,TFld::Selectable,"","0",
	    TSYS::strMess("%d;%d;%d;%d;%d;%d;%d;%d",FP_ON_TM,FP_ON_LEV,FP_ON_CAT,FP_ON_MESS,FP_ON_TM_REV,FP_ON_LEV_REV,FP_ON_CAT_REV,FP_ON_MESS_REV).c_str(),
	    _("On time;On level;On category;On messages;On time (reverse);On level (reverse);On category (reverse);On messages (reverse)"),i2s(A_ProtViewOrd).c_str()));
	attrAdd(new TFld("col",trS("Show columns"),TFld::String,0,"","pos;tm;utm;lev;cat;mess","","",i2s(A_ProtCol).c_str()));
	attrAdd(new TFld("itProp",trS("Item properties"),TFld::Integer,Attr::Active,"","","","",i2s(A_ProtItProp).c_str()));
    }
}

bool OrigProtocol::attrChange( Attr &cfg, TVariant prev )
{
    if(cfg.flgGlob()&Attr::Active) {
	if(cfg.id() == "itProp") {
	    string fid("it"), fnm(_("Item ")), fidp, fnmp;

	    //Delete specific unnecessary items
	    for(int iP = 0; true; iP++) {
		fidp = fid + i2s(iP);
		if(!cfg.owner()->attrPresent(fidp+"lev")) break;
		else if(iP >= cfg.getI()) {
		    cfg.owner()->attrDel(fidp+"lev");
		    cfg.owner()->attrDel(fidp+"tmpl");
		    cfg.owner()->attrDel(fidp+"fnt");
		    cfg.owner()->attrDel(fidp+"color");
		}
	    }

	    //Create ullage items
	    for(int iP = 0; iP < cfg.getI(); iP++) {
		fidp = fid + i2s(iP);
		fnmp = fnm + i2s(iP);
		if(cfg.owner()->attrPresent(fidp+"lev")) continue;
		cfg.owner()->attrAdd(new TFld((fidp+"lev").c_str(),(fnmp+_(": level")).c_str(),
		    TFld::Integer,Attr::Mutable,"","0","0;7","",i2s(A_ProtProps+A_ProtPropLev+A_ProtPropsSz*iP).c_str()));
		cfg.owner()->attrAdd(new TFld((fidp+"tmpl").c_str(),(fnmp+_(": template")).c_str(),
		    TFld::String,Attr::Mutable,"","","","",i2s(A_ProtProps+A_ProtPropTmpl+A_ProtPropsSz*iP).c_str()));
		cfg.owner()->attrAdd(new TFld((fidp+"fnt").c_str(),(fnmp+_(": font")).c_str(),
		    TFld::String,Attr::Font|Attr::Mutable,"","","","",i2s(A_ProtProps+A_ProtPropFnt+A_ProtPropsSz*iP).c_str()));
		cfg.owner()->attrAdd( new TFld((fidp+"color").c_str(),(fnmp+_(": color")).c_str(),
		    TFld::String,Attr::Color|Attr::Mutable,"","","","",i2s(A_ProtProps+A_ProtPropClr+A_ProtPropsSz*iP).c_str()));
	    }
	}
    }

    return Widget::attrChange(cfg, prev);
}

bool OrigProtocol::cntrCmdAttributes( XMLNode *opt, Widget *src )
{
    if(!src) src = this;

    //Get page info
    if(opt->name() == "info") {
	Widget::cntrCmdAttributes(opt,src);
	XMLNode *root, *el;
	if((root=ctrMkNode("area",opt,-1,"/attr",_("Attributes")))) {
	    for(unsigned iCh = 0; iCh < root->childSize(); iCh++) {
		el = root->childGet(iCh);
		int p = s2i(el->attr("p"));
		switch(p) {
		    case A_BackColor: el->setAttr("help",Widget::helpColor());	break;
		    case A_BackImg: el->setAttr("help",Widget::helpImg());	break;
		    case A_ProtFont: el->setAttr("help",Widget::helpFont());	break;
		    case A_ProtArch: el->setAttr("help",_("Messages archiver in the form \"{ArchMod}.{ArchiverId}\"."));	break;
		    case A_ProtLev: el->setAttr("help",_("Set value to \"< 0\" for get the current alarms."));	break;
		    case A_ProtTSize: el->setAttr("help",_("Set value to \"0\" for get all alarms, for \"lev\" < 0."));	break;
		    case A_ProtTmpl: el->setAttr("help",
			_("Category template or the regular expression \"/{re}/\". For the template there reserved the special symbols:\n"
			"  '*' - any multiply symbols group;\n"
			"  '?' - any one symbol;\n"
			"  '\\' - use to shield the special simbols."));
			break;
		    case A_ProtCol: el->setAttr("help",
			_("List of visible and the columns order separated by the symbol ';'. Supported columns:\n"
			"  \"pos\" - row number;\n"
			"  \"tm\" - date and time of the message;\n"
			"  \"utm\" - microseconds part of the message time;\n"
			"  \"lev\" - level of the message;\n"
			"  \"cat\" - category of the message;\n"
			"  \"mess\" - text of the message."));
			break;
		}
	    }
	    for(int iP = 0; iP < src->attrAt("itProp").at().getI(); iP++) {
		if((el=ctrId(root,TSYS::strMess("/it%dcolor",iP),true))) el->setAttr("help",Widget::helpColor());
		if((el=ctrId(root,TSYS::strMess("/it%dfnt",iP),true))) el->setAttr("help",Widget::helpFont());
		if((el=ctrId(root,TSYS::strMess("/it%dtmpl",iP),true))) el->setAttr("help",
		    _("Category template or the regular expression \"/{re}/\". For the template there reserved the special symbols:\n"
		    "  '*' - any multiply symbols group;\n"
		    "  '?' - any one symbol;\n"
		    "  '\\' - use to shield the special simbols."));
	    }
	}
	return true;
    }

    //Process command to page
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

string OrigDocument::name( ) const	{ return _("Document"); }

string OrigDocument::descr( ) const	{ return _("Document widget of the finite visualization."); }

void OrigDocument::postEnable( int flag )
{
    LWidget::postEnable(flag);

    if(flag&TCntrNode::NodeConnect) {
	attrAdd(new TFld("style","CSS",TFld::String,TFld::FullText,"","","","",i2s(A_DocStyle).c_str()));
	attrAdd(new TFld("tmpl",trS("Template"),TFld::String,TFld::TransltText|TFld::FullText,"","","","",i2s(A_DocTmpl).c_str()));
	attrAdd(new TFld("doc",trS("Document"),TFld::String,TFld::TransltText|TFld::FullText,"","","","",i2s(A_DocDoc).c_str()));
	attrAdd(new TFld("font",trS("Font"),TFld::String,Attr::Font,"","Arial 11","","",i2s(A_DocFont).c_str()));
	attrAdd(new TFld("bTime",trS("Time: begin"),TFld::Integer,Attr::DateTime,"","0","","",i2s(A_DocBTime).c_str()));
	attrAdd(new TFld("time",trS("Time: current"),TFld::Integer,Attr::DateTime|Attr::Active,"","0","","",i2s(A_DocTime).c_str()));
	attrAdd(new TFld("process",trS("In the process"),TFld::Boolean,TFld::NoWrite,"","0","","",i2s(A_DocProcess).c_str()));
	attrAdd(new TFld("n",trS("Archive size"),TFld::Integer,Attr::Active,"","0",TSYS::strMess("0;%d",DocArhSize).c_str(),"",i2s(A_DocN).c_str()));
    }
}

void OrigDocument::disable( Widget *base )
{
    if(dynamic_cast<SessWdg*>(base))
	SYS->taskDestroy(base->nodePath('.',true)+".doc", NULL, 3*prmInterf_TM);
}

bool OrigDocument::attrChange( Attr &cfg, TVariant prev )
{
    //Document's number change process
    if(cfg.id() == "n" && cfg.getI() != prev.getI()) {
	if(!cfg.getI()) {
	    cfg.owner()->attrDel("aCur");
	    cfg.owner()->attrDel("vCur");
	    cfg.owner()->attrDel("aDoc");
	    cfg.owner()->attrDel("aSize");
	}
	else {
	    if(!cfg.owner()->attrPresent("vCur"))
		cfg.owner()->attrAdd(new TFld("vCur",trS("Archive: cursor: view"),TFld::Integer,Attr::NotStored|Attr::Mutable|Attr::Active,"","0",
		    TSYS::strMess("-2;%d",DocArhSize-1).c_str()));
	    if(!cfg.owner()->attrPresent("aCur"))
		cfg.owner()->attrAdd(new TFld("aCur",trS("Archive: cursor: current"),TFld::Integer,Attr::NotStored|Attr::Mutable|Attr::Active,"","0",
		    TSYS::strMess("-1;%d",DocArhSize-1).c_str()));
	    if(!cfg.owner()->attrPresent("aDoc"))
		cfg.owner()->attrAdd(new TFld("aDoc",trS("Archive: current document"),TFld::String,Attr::NotStored|TFld::FullText|Attr::Mutable|Attr::Active));
	    if(!cfg.owner()->attrPresent("aSize"))
		cfg.owner()->attrAdd(new TFld("aSize",trS("Archive: size"),TFld::Integer,Attr::NotStored|Attr::Mutable));
	}
    }

    SessWdg *sw = dynamic_cast<SessWdg*>(cfg.owner());
    if(!sw) return Widget::attrChange(cfg,prev);

    string u, l;
    if(sw) { u = sw->ownerSess()->reqUser(); l = sw->ownerSess()->reqLang(); }

    //Make document after time set
    if(cfg.id() == "time" && (cfg.getI() != prev.getI() || (!cfg.getI() && prev.getI()))) {
	if(!cfg.getI() && prev.getI()) cfg.setI(prev.getI(),false,true);
	try {
	    string taskNm = sw->nodePath('.',true)+".doc";
	    SYS->taskDestroy(taskNm);
	    SYS->taskCreate(taskNm, -1, OrigDocument::DocTask, sw, 2);
	    sw->attrAt("process").at().setB(true);
	} catch(TError &err) { }
    }
    //Load document's from project's DB
    else if(cfg.id() == "n" && cfg.getI() != prev.getI()) {
	string tVl;
	AutoHD<Attr> aCur, aDoc, vCur;
	try {
	    aCur = cfg.owner()->attrAt("aCur");
	    aDoc = cfg.owner()->attrAt("aDoc");
	    vCur = cfg.owner()->attrAt("vCur");
	} catch(TError &err) { }

	if(!(aCur.freeStat() || aDoc.freeStat() || vCur.freeStat())) {
	    // Archive position load
	    if((tVl=sw->sessAttr("aCur")).size()) aCur.at().setS(tVl, false, true);
	    // Current archive documents load
	    if((tVl=sw->sessAttr("doc"+aCur.at().getS())).size()) aDoc.at().setS(tVl, false, true);
	    // Set current document
	    vCur.at().setI(aCur.at().getI(), false, true);

	    cfg.owner()->attrAt("doc").at().setS(aDoc.at().getS(),false,true);
	    // Parse current document and restore last document's time
	    string cdoc = cfg.owner()->attrAt("doc").at().getS();
	    if(!cdoc.empty()) {
		XMLNode xdoc;
		try { xdoc.load(XHTML_entity+cdoc, false, Mess->charset()); } catch(TError &err) { }
		cfg.owner()->attrAt("time").at().setS(xdoc.attr("docTime"), false, true);
	    }
	    sizeUpdate(sw);
	}
    }
    //Move the archive cursor
    else if(cfg.id() == "aCur" && cfg.getI() != prev.getI()) {
	int n = cfg.owner()->attrAt("n").at().getI();
	if(cfg.getI() < 0) cfg.setI(((prev.getI()+1) >= n) ? 0 : (prev.getI()+1), false, true);
	else if(cfg.getI() >= n) cfg.setI(n-1, false, true);
	if(cfg.getI() != prev.getI()) {
	    cfg.owner()->attrAt("aDoc").at().setS(trD_LU(cfg.owner()->attrAt("tmpl").at().getS(),l,u));
	    if(prev.getI() == cfg.owner()->attrAt("vCur").at().getI())
		cfg.owner()->attrAt("vCur").at().setI(cfg.getI());

	    // Save cursor to document to project's DB
	    if(prev.getI() < n && prev.getI() >= 0) sw->sessAttrSet(cfg.id(), cfg.getS());
	}
	sizeUpdate(sw);
    }
    //Document save
    else if(cfg.id() == "aDoc" && cfg.getS() != prev.getS())
	sw->sessAttrSet("doc"+cfg.owner()->attrAt("aCur").at().getS(), cfg.getS());
    //Move archive view cursor
    else if(cfg.id() == "vCur" && cfg.getI() != prev.getI()) {
	try {
	    AutoHD<Attr> aCur = cfg.owner()->attrAt("aCur");
	    int n = cfg.owner()->attrAt("n").at().getI();

	    if(cfg.getI() < 0) {
		int docN = prev.getI();
		// Search next document
		if(cfg.getI() == -1)
		    while(docN != aCur.at().getI()) {
			if(docN != prev.getI() && sw->sessAttr("doc"+i2s(docN),true).size()) break;
			if(++docN >= n) docN = 0;
		    }
		// Search previous document
		else {
		    if(--docN < 0) docN = n-1;
		    if(docN == aCur.at().getI()) docN = prev.getI();
		    if(!sw->sessAttr("doc"+i2s(docN),true).size()) docN = prev.getI();
		}
		if(docN != cfg.getI())	cfg.setI(docN,false,true);
	    }
	    else if(cfg.getI() >= n)	cfg.setI(aCur.at().getI(), false, true);
	    if(cfg.getI() != prev.getI())	cfg.owner()->attrAt("doc").at().setS(sw->sessAttr("doc"+cfg.getS()));
	} catch(TError &err) { }
    }

    return Widget::attrChange(cfg,prev);
}

bool OrigDocument::cntrCmdAttributes( XMLNode *opt, Widget *src )
{
    if(!src) src = this;

    //Get page info
    if(opt->name() == "info") {
	Widget::cntrCmdAttributes(opt,src);
	XMLNode *root, *el;
	if((root=ctrMkNode("area",opt,-1,"/attr",_("Attributes"))))
	    for(unsigned iCh = 0; iCh < root->childSize(); iCh++) {
		el = root->childGet(iCh);
		int p = s2i(el->attr("p"));
		switch(p) {
		    case A_DocStyle: el->setAttr("SnthHgl","1")->setAttr("help",
			_("CSS rules in the rows like to \"body { background-color:#818181; }\""));
			break;
		    case A_DocTmpl: el->setAttr("SnthHgl","1")->setAttr("help",
			_("Document template in XHTML. Starts from the tag \"body\" and include the procedures parts:\n"
			"<body docProcLang=\"JavaLikeCalc.JavaScript\">\n<h1>Value<?dp return wCod+1.314;?></h1>\n</body>"));
			break;
		    case A_DocDoc:
			el->setAttr("SnthHgl","1")->setAttr("help",_("Final document in XHTML, starting from the tag \"body\"."));
			break;
		    case A_DocTime:
			el->setAttr("help",_("Write the time for the document generation from the specified one or zero for regeneration."));
			break;
		    case A_DocFont: el->setAttr("help",Widget::helpFont());	break;
		    default:
			if(el->attr("id") == "aDoc")
			    el->setAttr("SnthHgl","1")->setAttr("help",_("Current archive document in XHTML, starting from the tag \"body\"."));
		}
	    }
	return true;
    }

    //Process command to page
    string a_path = opt->attr("path");
    if((a_path == "/attr/tmpl" || a_path == "/attr/doc" || a_path == "/attr/aDoc") && ctrChkNode(opt,"SnthHgl",RWRWR_,"root",SUI_ID,SEC_RD))
    {
	opt->childAdd("blk")->setAttr("beg","<!--")->setAttr("end","-->")->setAttr("color","gray")->setAttr("font_italic","1");
	XMLNode *tag = opt->childAdd("blk")->setAttr("beg","<\\?")->setAttr("end","\\?>")->setAttr("color","#666666");
	// Get document's language syntax highlight.
	try {
	    string dcLng;
	    TArrayObj *reRez = TRegExp(".*<body\\s+.*\\s*docProcLang=\"([^\"]+)\".*>.*","").match(src->attrAt(a_path.substr(6)).at().getS());
	    if(reRez->propGet("length").getI() >= 2) dcLng = reRez->arGet(1).getS();
	    delete reRez;
	    if(dcLng.size())
		SYS->daq().at().at(TSYS::strParse(dcLng,0,".")).at().compileFuncSynthHighl(TSYS::strParse(dcLng,1,"."),*tag);
	} catch(...) { }
	tag = opt->childAdd("blk")->setAttr("beg","<\\w+")->setAttr("end","\\/?>")->setAttr("font_weight","1");
	    tag->childAdd("rule")->setAttr("expr","\\b\\w+[ ]*(?==)")->setAttr("color","blue");
	    tag->childAdd("rule")->setAttr("expr","[ ]?\"[^\"]+\"")->setAttr("color","darkgreen");
	    tag->childAdd("rule")->setAttr("expr","[ ]?'[^']+'")->setAttr("color","darkgreen");
	opt->childAdd("rule")->setAttr("expr","<\\/[\\w]+>")->setAttr("font_weight","1");
	opt->childAdd("rule")->setAttr("expr","&([a-zA-Z]*|#\\d*);")->setAttr("color","#AF7E00");
    }
    else if(a_path == "/attr/style" && ctrChkNode(opt,"SnthHgl",RWRWR_,"root",SUI_ID,SEC_RD)) {
	opt->childAdd("blk")->setAttr("beg", "/\\*")->setAttr("end", "\\*/")->setAttr("color", "gray")->setAttr("font_italic", "1");
	opt->childAdd("blk")->setAttr("beg", "\\{")->setAttr("end", "\\}")->setAttr("color", "#666666")->
	    childAdd("rule")->setAttr("expr", ":[^;]+")->setAttr("color", "blue");
	opt->childAdd("rule")->setAttr("expr", "(\\.|#)\\w+\\s")->setAttr("color", "darkorange");
    }
    else return Widget::cntrCmdAttributes(opt,src);

    return true;
}

void OrigDocument::sizeUpdate( SessWdg *sw )
{
    try {
	AutoHD<Attr> aCur = sw->attrAt("aCur");
	AutoHD<Attr> aSize = sw->attrAt("aSize");

	int n = sw->attrAt("n").at().getI();
	int rSz = n;
	if(aCur.at().getI() < n && (!sw->sessAttr("doc"+i2s(aCur.at().getI()+1),true).size() || !sw->sessAttr("doc"+i2s(n-1),true).size()))
	    rSz = aCur.at().getI()+1;
	aSize.at().setI(rSz);
    } catch(TError &err) { }
}

TVariant OrigDocument::objFuncCall_w( const string &id, vector<TVariant> &prms, const string &user_lang, Widget *src )
{
    // string getArhDoc(integer nDoc) - get archive document text to 'nDoc' depth.
    //  nDoc - archive document at depth (0-{aSize-1})
    if(id == "getArhDoc" && prms.size() >= 1) {
	int nDoc = prms[0].getI();
        int aCur = src->attrAt("aCur").at().getI();
	int aSize = src->attrAt("aSize").at().getI();
	SessWdg *sw = dynamic_cast<SessWdg*>(src);
	if(!sw || nDoc < 0 || nDoc >= aSize) return "";

	aCur -= nDoc;
	if(aCur < 0) aCur += aSize;

	return sw->sessAttr("doc"+i2s(aCur));
    }

    return TVariant();
}

string OrigDocument::makeDoc( const string &tmpl, Widget *wdg )
{
    SessWdg *sw = (SessWdg*)wdg;
    string  u = sw->ownerSess()->reqUser(),
	    l = sw->ownerSess()->reqLang();

    XMLNode xdoc;
    string iLang;				//Process instruction language
    string wProgO;				//Object of work program
    time_t lstTime = 0;				//Last time
    TFunction funcIO(TSYS::path2sepstr(wdg->addr(),'_'));
    funcIO.setStor(wdg->calcProgStors("doc"));
    TValFunc funcV(wdg->id()+"_doc", NULL, false);
    funcV.setUser(u); funcV.setLang(l);
    vector<string> als;

    //Parse template
    try {
	if(!tmpl.empty()) xdoc.load(XHTML_entity+tmpl, true, Mess->charset());
	else return "";
    } catch(TError &err) {
	mess_err(wdg->nodePath().c_str(),_("Error parsing the document: %s."),err.mess.c_str());
	return "";
    }

    //Prepare call instructions environment
    if(strcasecmp(xdoc.name().c_str(),"body") == 0) {
	iLang = xdoc.attr("docProcLang");
	lstTime = s2i(xdoc.attr("docTime"));
    }
    if(sTrm(iLang).empty())	iLang = "JavaLikeCalc.JavaScript";
    if(!lstTime) lstTime = wdg->attrAt("bTime").at().getI();

    // Add generic io
    funcIO.ioIns(new IO("rez",trS("Result"),IO::String,IO::Return), A_DocCalcPrmRez);
    funcIO.ioIns(new IO("time",trS("Document time"),IO::Integer,IO::Default), A_DocCalcPrmTime);
    funcIO.ioIns(new IO("bTime",trS("Document begin time"),IO::Integer,IO::Default), A_DocCalcPrmBTime);
    funcIO.ioIns(new IO("lTime",trS("Last time"),IO::Integer,IO::Default), A_DocCalcPrmLTime);
    funcIO.ioIns(new IO("rTime",trS("Repeat time, seconds"),IO::Integer,IO::Default), A_DocCalcPrmRTime);
    funcIO.ioIns(new IO("rTimeU",trS("Repeat time, microseconds"),IO::Integer,IO::Default), A_DocCalcPrmRTimeU);
    funcIO.ioIns(new IO("rPer",trS("Repeat period"),IO::Real,IO::Default), A_DocCalcPrmRPer);
    funcIO.ioIns(new IO("mTime",trS("Message time, seconds"),IO::Integer,IO::Default), A_DocCalcPrmMTime);
    funcIO.ioIns(new IO("mTimeU",trS("Message time, microseconds"),IO::Integer,IO::Default), A_DocCalcPrmMTimeU);
    funcIO.ioIns(new IO("mLev",trS("Message level"),IO::Integer,IO::Default), A_DocCalcPrmMLev);
    funcIO.ioIns(new IO("mCat",trS("Message category"),IO::String,IO::Default), A_DocCalcPrmMCat);
    funcIO.ioIns(new IO("mVal",trS("Message value"),IO::String,IO::Default), A_DocCalcPrmMVal);

    // Add user io
    wdg->attrList(als);
    for(unsigned iA = 0; iA < als.size(); iA++) {
	AutoHD<Attr> cattr = wdg->attrAt(als[iA]);
	if(!(cattr.at().flgGlob()&Attr::IsUser)) continue;
	funcIO.ioAdd(new IO(als[iA].c_str(),cattr.at().name().c_str(),cattr.at().fld().typeIO(),IO::Output));
    }
    try {
	// Compile empty function for binding to object
	wProgO = SYS->daq().at().at(TSYS::strSepParse(iLang,0,'.')).at().compileFunc(TSYS::strSepParse(iLang,1,'.'),funcIO,"");
	// Connect to compiled function
	funcV.setFunc(&((AutoHD<TFunction>)SYS->nodeAt(wProgO)).at());
	// Load values of generic IO
	funcV.setI(A_DocCalcPrmTime, wdg->attrAt("time").at().getI());
	funcV.setI(A_DocCalcPrmBTime, wdg->attrAt("bTime").at().getI());
	funcV.setI(A_DocCalcPrmLTime, lstTime);
	// Load values of user IO
	for(int iA = 12; iA < funcV.ioSize(); iA++)
	    funcV.set(iA,wdg->attrAt(funcV.func()->io(iA)->id()).at().get());
    } catch(TError &err) {
	mess_err(wdg->nodePath().c_str(),_("Error compiling function for the document: %s"),err.mess.c_str());
	return "";
    }

    //Warning timeout message subtree add to generic tree
    xdoc.childAdd()->load(TSYS::strMess(
	_("<BODY>\n"
	"  <H1>The document is forming now ...</H1>\n"
	"  <P>The document is too big for generating into the standard user interface timeout (%ds). "
	"And for now the document is into the building. "
	"You can wait the forming finish or terminate the process by select lesser time interval for the document.</P>\n"
	"  <P>The building progress:\n"
	"    <ul id='progress' />\n"
	"  </P>\n"
	"</BODY>"),prmInterf_TM), false, Mess->charset());

    //Node proocess
    OrigDocument::nodeProcess(wdg, &xdoc, funcV, funcIO, iLang);

    //Remove warning timeout message subtree
    xdoc.childDel(xdoc.childSize()-1);

    xdoc.setAttr("docTime", i2s(funcV.getI(A_DocCalcPrmTime)));

    return xdoc.save(0, Mess->charset());
}

void OrigDocument::nodeProcess( Widget *wdg, XMLNode *xcur, TValFunc &funcV, TFunction &funcIO, const string &iLang, bool instrDel, time_t upTo )
{
    //Progress warning node
    XMLNode *progrNode = NULL;

    if(!upTo) upTo = time(NULL)+prmInterf_TM;

    //Process instructions
    if(xcur->childGet("<?dp",0,true)) {
	if(!s2i(xcur->attr("docAppend")))
	    for(unsigned i_t = 0; i_t < xcur->childSize(); ) {
		if(xcur->childGet(i_t)->name().compare(0,4,"<?dp") != 0) xcur->childDel(i_t);
		else i_t++;
	    }
	//Call procedures
	for(int i_t = 0; i_t < (int)xcur->childSize(); i_t++) {
	    XMLNode *curPrc = xcur->childGet(i_t);
	    if(curPrc->name().compare(0,4,"<?dp") != 0) continue;
	    try {
		// Compile for new instruction
		SYS->daq().at().at(TSYS::strSepParse(iLang,0,'.')).
		       at().compileFunc(TSYS::strSepParse(iLang,1,'.'), funcIO, curPrc->text());
		// Call
		funcV.setS(A_DocCalcPrmRez, "");
		funcV.calc();
		// Load result to XML tree
		XMLNode xproc;
		xproc.load(string(XHTML_entity)+"<i>"+funcV.getS(A_DocCalcPrmRez)+"</i>", true, Mess->charset());
		// Set result
		for(unsigned i_tr = 0; i_tr < xproc.childSize(); i_tr++)
		    *(xcur->childAdd()) = *xproc.childGet(i_tr);
		if(instrDel)	xcur->childDel(i_t--);
	    } catch(TError &err) {
		mess_err(wdg->nodePath().c_str(),_("Error executing instruction '%s': %s"), TSYS::strSepParse(iLang,1,'.').c_str(), err.mess.c_str());
		mess_err(wdg->nodePath().c_str(),_("Error code: %s"), curPrc->text().c_str());
	    }
	}
    }

    float dRpt = 1;
    string dAMess;
    //Go to include nodes
    for(unsigned iC = 0; iC < xcur->childSize(); iC++) {
	XMLNode *reptN = xcur->childGet(iC);
	if(reptN->name().size() && reptN->name()[0] == '<') continue;
	// Repeat tags
	if(funcV.getI(A_DocCalcPrmBTime) && (dRpt=s2r(reptN->attr("docRept"))) > 1e-6) {
	    int rCnt = 0;

	    bool docRevers = s2i(reptN->attr("docRevers"));
	    funcV.setR(A_DocCalcPrmRPer, dRpt);
	    int64_t wTime = (int64_t)funcV.getI(A_DocCalcPrmTime)*1000000;
	    int64_t bTime = (int64_t)funcV.getI(A_DocCalcPrmBTime)*1000000;
	    int64_t lstTime = (int64_t)funcV.getI(A_DocCalcPrmLTime)*1000000;
	    int64_t perRpt = (int64_t)(1000000*dRpt);
	    int64_t rTime = bTime + perRpt*((lstTime-bTime)/perRpt);
	    if(lstTime && lstTime < bTime) rTime -= perRpt;
	    //if(((time-rTime)/perRpt) > 1000) continue;

	    // Progress prepare
	    if(!progrNode) {
		progrNode = xcur->root();
		progrNode = progrNode->childGet(progrNode->childSize()-1)->getElementBy("id","progress");
	    }
	    progrNode->childAdd("li")->setText(TSYS::strMess(_("Data block %d: %0.2f%% loaded."),progrNode->childSize(),0));

	    while(rTime < wTime && !TSYS::taskEndRun()) {
		//Drop current changes and continue
		if(time(NULL) >= upTo) {
		    upTo = time(NULL) + prmInterf_TM;
		    if(!wdg->attrAt("n").at().getI() || wdg->attrAt("aCur").at().getI() == wdg->attrAt("vCur").at().getI()) {
			progrNode->childGet(progrNode->childSize()-1)->
			    setText(TSYS::strMess(_("Data block %d: %0.2f%% loaded."),progrNode->childSize(),100*(float)(rTime-bTime)/(float)(wTime-bTime)));
			XMLNode *rootN = xcur->root();
			wdg->attrAt("doc").at().setS(rootN->childGet(rootN->childSize()-1)->save(0,Mess->charset()));
		    }
		}
		//Process
		if(s2i(reptN->attr("docRptEnd"))) {
		    int iN = docRevers?(iC+1):iC;
		    *(xcur->childIns(iN)) = *reptN;
		    OrigDocument::nodeClear(xcur->childGet(iN));
		    if(!docRevers) iC++;
		    rCnt++;
		}
		int64_t rTimeT = vmin(rTime+perRpt, wTime);
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
	    if(docRevers) iC += rCnt;

	    progrNode->childGet(progrNode->childSize()-1)->setText(TSYS::strMess(_("Data block %d: %0.2f%% loaded."),progrNode->childSize(),100));
	}
	// Repeat messages
	else if(!(dAMess=xcur->childGet(iC)->attr("docAMess")).empty()) {
	    int rCnt = 0;
	    XMLNode *reptN = xcur->childGet(iC);
	    bool docRevers = s2i(xcur->childGet(iC)->attr("docRevers"));
	    string docAMessArchs = xcur->childGet(iC)->attr("docAMessArchs");

	    int off = 0;
	    int dACat = s2i(TSYS::strSepParse(dAMess,0,':',&off));
	    string dATmpl = dAMess.substr(off);

	    vector<TMess::SRec> mess;
	    SYS->archive().at().messGet(funcV.getI(A_DocCalcPrmLTime), funcV.getI(A_DocCalcPrmTime), mess,
		dATmpl, (TMess::Type)dACat, docAMessArchs);

	    // Progress bar prepare
	    if(!progrNode) {
		progrNode = xcur->root();
		progrNode = progrNode->childGet(progrNode->childSize()-1)->getElementBy("id","progress");
	    }
	    progrNode->childAdd("li")->setText(TSYS::strMess(_("Messages block %d: %0.2f%% loaded."),progrNode->childSize(),0));

	    for(unsigned iR = 0; iR < mess.size() && !TSYS::taskEndRun(); iR++) {
		// Drop current changes and continue
		if(time(NULL) >= upTo) {
		    upTo = time(NULL)+prmInterf_TM;
		    if(!wdg->attrAt("n").at().getI() || wdg->attrAt("aCur").at().getI() == wdg->attrAt("vCur").at().getI()) {
			progrNode->childGet(progrNode->childSize()-1)->
			    setText(TSYS::strMess(_("Messages block %d: %0.2f%% loaded."),progrNode->childSize(),100*(float)iR/(float)mess.size()));
			XMLNode *rootN = xcur->root();
			wdg->attrAt("doc").at().setS(rootN->childGet(rootN->childSize()-1)->save(0,Mess->charset()));
		    }
		}

		if(s2i(reptN->attr("docRptEnd"))) {
		    unsigned iN = (docRevers ? (iC+1) : iC);
		    *(xcur->childIns(iN)) = *reptN;
		    nodeClear(xcur->childGet(iN));
		    if(!docRevers) iC++;
		    rCnt++;
		}
		funcV.setI(A_DocCalcPrmMTime, mess[iR].time);
		funcV.setI(A_DocCalcPrmMTimeU, mess[iR].utime);
		funcV.setI(A_DocCalcPrmMLev, mess[iR].level);
		funcV.setS(A_DocCalcPrmMCat, mess[iR].categ);
		funcV.setS(A_DocCalcPrmMVal, mess[iR].mess);
		nodeProcess(wdg, reptN, funcV, funcIO, iLang, false, upTo);
		reptN->setAttr("docRptEnd", "1");
	    }

	    progrNode->childGet(progrNode->childSize()-1)->setText(TSYS::strMess(_("Messages block %d: %0.2f%% loaded."),progrNode->childSize(),100));

	    funcV.setI(A_DocCalcPrmMTime, 0);
	    funcV.setI(A_DocCalcPrmMTimeU, 0);
	    funcV.setI(A_DocCalcPrmMLev, 0);
	    funcV.setS(A_DocCalcPrmMCat, "");
	    funcV.setS(A_DocCalcPrmMVal, "");
	    if(docRevers) iC += rCnt;
	}
	else nodeProcess(wdg,xcur->childGet(iC),funcV,funcIO,iLang,instrDel,upTo);
    }
}

void OrigDocument::nodeClear( XMLNode *xcur )
{
    xcur->attrDel("docRept");
    xcur->attrDel("docRptEnd");
    xcur->attrDel("docRevers");
    xcur->attrDel("docAMess");

    for(unsigned iC = 0; iC < xcur->childSize(); )
	if(xcur->childGet(iC)->name().compare(0,4,"<?dp") == 0) xcur->childDel(iC);
	else nodeClear(xcur->childGet(iC++));
}

void *OrigDocument::DocTask( void *param )
{
    SessWdg *sw = (SessWdg*)param;
    string  u = sw->ownerSess()->reqUser(),
	    l = sw->ownerSess()->reqLang();

    // The document generation
    try {
	string mkDk;
	if(!sw->attrAt("n").at().getI()) {
	    mkDk = sw->attrAt("doc").at().getS();
	    if(mkDk.empty()) mkDk = trD_LU(sw->attrAt("tmpl").at().getS(), l, u);
	    mkDk = OrigDocument::makeDoc(mkDk,sw);
	    sw->attrAt("doc").at().setS(mkDk);
	}
	else {
	    AutoHD<Attr> aCur, aDoc, vCur;
	    try {
		aCur = sw->attrAt("aCur");
		aDoc = sw->attrAt("aDoc");
		vCur = sw->attrAt("vCur");
	    } catch(TError &err) { }

	    if(!(aCur.freeStat() || aDoc.freeStat() || vCur.freeStat())) {
		//int aCur = aCur.at().getI();
		mkDk = aDoc.at().getS();
		if(mkDk.empty()) mkDk = trD_LU(sw->attrAt("tmpl").at().getS(), l, u);

		mkDk = makeDoc(mkDk, sw);
		aDoc.at().setS(mkDk);
		if(aCur.at().getI() == vCur.at().getI()) sw->attrAt("doc").at().setS(mkDk);
	    }
	}
    } catch(TError &err) {
	mess_err(err.cat.c_str(), "%s", err.mess.c_str());
	mess_err(sw->nodePath().c_str(), _("Document generation terminated."));
    }

    sw->attrAt("process").at().setB(false);

    return NULL;
}

//************************************************
//* OrigFunction: User function original widget  *
//************************************************
OrigFunction::OrigFunction( ) : PrWidget("Function")	{ }

string OrigFunction::name( ) const	{ return _("Built-in function"); }

string OrigFunction::descr( ) const	{ return _("Built-in function widget."); }

//************************************************
//* OrigBox: User element original widget        *
//************************************************
OrigBox::OrigBox( ) : PrWidget("Box")	{ }

string OrigBox::name( ) const	{ return _("Group of elements"); }

string OrigBox::descr( ) const	{ return _("Widget of the group of elements of the finite visualization."); }

void OrigBox::postEnable( int flag )
{
    LWidget::postEnable(flag);

    if(flag&TCntrNode::NodeConnect) {
	attrAdd(new TFld("pgOpenSrc",trS("Page: source of the opening"),TFld::String,0,"","","","",i2s(A_PG_OPEN_SRC).c_str()));
	attrAdd(new TFld("pgGrp",trS("Page: group"),TFld::String,0,"","","","",i2s(A_PG_GRP).c_str()));
	attrAdd(new TFld("backColor",trS("Background: color"),TFld::String,Attr::Color,"","#FFFFFF","","",i2s(A_BackColor).c_str()));
	attrAdd(new TFld("backImg",trS("Background: image"),TFld::String,Attr::Image,"","","","",i2s(A_BackImg).c_str()));
	attrAdd(new TFld("bordWidth",trS("Border: width"),TFld::Integer,TFld::NoFlag,"","0","","",i2s(A_BordWidth).c_str()));
	attrAdd(new TFld("bordColor",trS("Border: color"),TFld::String,Attr::Color,"","#000000","","",i2s(A_BordColor).c_str()));
	attrAdd(new TFld("bordStyle",trS("Border: style"),TFld::Integer,TFld::Selectable,"",i2s(FBRD_SOL).c_str(),
	    TSYS::strMess("%d;%d;%d;%d;%d;%d;%d;%d;%d",FBRD_NONE,FBRD_DOT,FBRD_DASH,FBRD_SOL,FBRD_DBL,FBRD_GROOVE,FBRD_RIDGE,FBRD_INSET,FBRD_OUTSET).c_str(),
	    _("None;Dotted;Dashed;Solid;Double;Groove;Ridge;Inset;Outset"),i2s(A_BordStyle).c_str()));
    }
}

bool OrigBox::cntrCmdAttributes( XMLNode *opt, Widget *src )
{
    if(!src) src = this;

    //Get page info
    if(opt->name() == "info") {
	Widget::cntrCmdAttributes(opt,src);
	XMLNode *root, *el;
	if((root=ctrMkNode("area",opt,-1,"/attr",_("Attributes"))))
	    for(unsigned iCh = 0; iCh < root->childSize(); iCh++) {
		el = root->childGet(iCh);
		int p = s2i(el->attr("p"));
		switch(p) {
		    case A_PG_GRP: el->setAttr("help",
			_("EMPTY and the \"main\" group is meant of using this page as the Root-main page, so such ones will replace other Root-main pages.\n"
			  "The \"fl\" group is meant of using in the \"fly\" windows which are suitable for multiple open and must not be traced for doubles.\n"
			  "All other are meant for including to the containers-boxes or single opening, so they forced in checking for doubles when the last one will be opened and the previous ones be closed."));
			break;
		    case A_BackColor:
		    case A_BordColor: el->setAttr("help", Widget::helpColor());	break;
		    case A_BackImg: el->setAttr("help", Widget::helpImg());	break;
		}
	    }
	return true;
    }

    //Process command to page
    return Widget::cntrCmdAttributes(opt,src);
}
