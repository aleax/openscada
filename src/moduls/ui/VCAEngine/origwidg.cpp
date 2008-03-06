
//OpenSCADA system module UI.VCAEngine file: origwidg.cpp
/***************************************************************************
 *   Copyright (C) 2006-2007 by Roman Savochenko                           *
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
	throw TError(mod->nodePath().c_str(),_("Deleting of main primitive widgets is error."));
    
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
    
    //- Init active attributes -
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
    //- Get page info -
    if( opt->name() == "info" )
    {
        Widget::cntrCmdGeneric(opt);
        ctrMkNode("oscada_cntr",opt,-1,"/",_("Base widget: ")+id());
	ctrMkNode("fld",opt,-1,"/wdg/st/parent",_("Parent"),R_R_R_,user().c_str(),grp().c_str(),
		2,"tp","str","dest","");		
        return true;
    }
    
    //- Process command to page -
    string a_path = opt->attr("path");
    if( a_path == "/wdg/st/parent" && ctrChkNode(opt,"get",R_R_R_,user().c_str(),grp().c_str(),SEQ_RD) )
	opt->setText(parentNm());
    else if( Widget::cntrCmdGeneric(opt) ) return true;
    else return false;
}

void PrWidget::cntrCmdProc( XMLNode *opt )
{
    if( cntrCmdServ(opt) ) return;
    
    //- Get page info -
    if( opt->name() == "info" )
    {
        cntrCmdGeneric(opt);
        cntrCmdAttributes(opt );
        return;
    }
    
    //- Process command to page -
    cntrCmdGeneric(opt) || cntrCmdAttributes(opt);
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
    return _("Elementary figures widget of the end visualisation."); 
}

void OrigElFigure::postEnable( int flag )
{
    LWidget::postEnable(flag);
    
    if( flag&TCntrNode::NodeConnect )
    {
	attrAdd( new TFld("lineWdth",_("Line:width"),TFld::Integer,TFld::NoFlag,"2","1","0;99","",20) );
	attrAdd( new TFld("lineClr",_("Line:color"),TFld::String,Attr::Color,"20","#000000","","",21) );
	attrAdd( new TFld("lineDecor",_("Line:decorate"),TFld::Integer,TFld::Selected,"1","0","0;1",_("No decor;Pipe"),22) );
	attrAdd( new TFld("bordWdth",_("Border:width"),TFld::Integer,TFld::NoFlag,"2","0","0;99","",23) );
	attrAdd( new TFld("bordClr",_("Border:color"),TFld::String,Attr::Color,"20","#000000","","",24) );
	attrAdd( new TFld("fillColor",_("Fill:color"),TFld::String,Attr::Color,"20","","","",25) );
	attrAdd( new TFld("fillImg",_("Fill:image"),TFld::String,Attr::Image,"20","","","",26) );
        attrAdd( new TFld("orient",_("Orientation angle"),TFld::Integer,TFld::NoFlag,"3","0","-180;180","",28) );
	attrAdd( new TFld("elLst",_("Element's list"),TFld::String,TFld::FullText|Attr::Active,"300","","","",27) );
    }
}

bool OrigElFigure::attrChange( Attr &cfg, void *prev )
{
    if( cfg.flgGlob()&Attr::Active && cfg.id() == "elLst" )
    {
	string sel;
	string ls_prev = *(string*)prev;
	vector<int> pntls, pntls_prev;	
	int p[5];
	//- Parse last attributes list and make point list -	
	for( int off = 0; (sel=TSYS::strSepParse(ls_prev,0,'\n',&off)).size(); )
	{
	    for( int i_p = 0; i_p < sizeof(p)/sizeof(int); i_p++ ) p[i_p]=-1;
	    sscanf(sel.c_str(),"line:%d:%d",&p[0],&p[1]) || 
	    sscanf(sel.c_str(),"arc:%d:%d:%d:%d:%d",&p[0],&p[1],&p[2],&p[3],&p[4]) ||
	    sscanf(sel.c_str(),"bezier:%d:%d:%d:%d",&p[0],&p[1],&p[2],&p[3]);
	    for( int i_p = 0; i_p < sizeof(p)/sizeof(int); i_p++ ) 
		if( p[i_p]>=0 ) pntls_prev.push_back(p[i_p]);
	}
	//- Parse new attributes list and update point's attributes -
	for( int off = 0; (sel=TSYS::strSepParse(cfg.getS(),0,'\n',&off)).size(); )
	{
	    for( int i_p = 0; i_p < sizeof(p)/sizeof(int); i_p++ ) p[i_p]=-1;	
	    sscanf(sel.c_str(),"line:%d:%d",&p[0],&p[1]) || 
	    sscanf(sel.c_str(),"arc:%d:%d:%d:%d:%d",&p[0],&p[1],&p[2],&p[3],&p[4]) ||
	    sscanf(sel.c_str(),"bezier:%d:%d:%d:%d",&p[0],&p[1],&p[2],&p[3]);
	    for( int i_p = 0; i_p < sizeof(p)/sizeof(int); i_p++ )
		if( p[i_p] >= 0 )
		{		    
		    pntls.push_back(p[i_p]);
		    if( cfg.owner()->attrPresent("p"+TSYS::int2str(p[i_p])+"x") )	continue;
		    cfg.owner()->attrAdd( new TFld(("p"+TSYS::int2str(p[i_p])+"x").c_str(),(_("Point ")+TSYS::int2str(p[i_p])+":x").c_str(),
			TFld::Integer,Attr::Mutable,"","0","","",30+p[i_p]*2) );
		    cfg.owner()->attrAdd( new TFld(("p"+TSYS::int2str(p[i_p])+"y").c_str(),(_("Point ")+TSYS::int2str(p[i_p])+":y").c_str(),
			TFld::Integer,Attr::Mutable,"","0","","",30+p[i_p]*2+1) );
		}
	}
	//- Delete points -
	for( int i_p = 0; i_p < pntls_prev.size(); i_p++ )
	{
	    int i_p1;
	    for( i_p1 = 0; i_p1 < pntls.size(); i_p1++ )
		if( pntls[i_p1] == pntls_prev[i_p] ) break;
	    if( i_p1 >= pntls.size() )
	    {
		cfg.owner()->attrDel("p"+TSYS::int2str(pntls_prev[i_p])+"x");
		cfg.owner()->attrDel("p"+TSYS::int2str(pntls_prev[i_p])+"y");
	    }
	}
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
    return _("Form's elements widget of the end visualisation."); 
}

void OrigFormEl::postEnable( int flag )
{
    LWidget::postEnable(flag);
    
    if( flag&TCntrNode::NodeConnect )
	attrAdd( new TFld("elType",_("Element type"),TFld::Integer,TFld::Selected|Attr::Active,"2","0","0;1;2;3;4;5;6;7",
                    	       _("Line edit;Text edit;Chek box;Button;Combo box;List;Slider;Scroll Bar"),20) );
}
								  
bool OrigFormEl::attrChange( Attr &cfg, void *prev )
{
    if( cfg.flgGlob()&Attr::Active && cfg.id() == "elType" )
    {
	//- Delete specific attributes -
	switch(*(int*)prev)
	{
	    case 0: 
		cfg.owner()->attrDel("value");
		cfg.owner()->attrDel("view");
		cfg.owner()->attrDel("cfg");
		break;
	    case 1: 
		cfg.owner()->attrDel("value");
		cfg.owner()->attrDel("wordWrap");
		break;
	    case 2:	cfg.owner()->attrDel("value");	break;
	    case 3:
		cfg.owner()->attrDel("value");
	        cfg.owner()->attrDel("img");
		cfg.owner()->attrDel("color");
		cfg.owner()->attrDel("checkable");
		break;
	    case 4: case 5:
		cfg.owner()->attrDel("value");
		cfg.owner()->attrDel("items");
		break;
	    case 6: case 7:
		cfg.owner()->attrDel("value");
		cfg.owner()->attrDel("cfg");
		break;
	}
	
	//- Create specific attributes -
	switch(cfg.getI())
	{
	    case 0:
		cfg.owner()->attrAdd( new TFld("value",_("Value"),TFld::String,Attr::Mutable,"200","","","",21) );
		cfg.owner()->attrAdd( new TFld("view",_("View"),TFld::Integer,TFld::Selected|Attr::Mutable|Attr::Active,
		    "1","0","0;1;2;3;4;5;6",_("Text;Combo;Integer;Real;Time;Data;Data and time"),22) );
		cfg.owner()->attrAdd( new TFld("cfg",_("Config"),TFld::String,TFld::FullText|Attr::Mutable,"","","","",23) );
		break;
	    case 1: 
		cfg.owner()->attrAdd( new TFld("value",_("Value"),TFld::String,TFld::FullText|Attr::Mutable,"","","","",21) );
		cfg.owner()->attrAdd( new TFld("wordWrap",_("Word wrap"),TFld::Boolean,Attr::Mutable,"1","1","","",22) );
		break;
	    case 2:
		cfg.owner()->attrAdd( new TFld("value",_("Value"),TFld::Boolean,Attr::Mutable,"","","","",21) );
		break;
	    case 3:
		cfg.owner()->attrAdd( new TFld("value",_("Value"),TFld::Boolean,Attr::Mutable,"","","","",21) );
		cfg.owner()->attrAdd( new TFld("img",_("Image"),TFld::String,Attr::Image|Attr::Mutable,"","","","",22) );
		cfg.owner()->attrAdd( new TFld("color",_("Color"),TFld::String,Attr::Color|Attr::Mutable,"20","","","",23) );
		cfg.owner()->attrAdd( new TFld("checkable",_("Checkable"),TFld::Boolean,Attr::Mutable,"","","","",24) );
		break;
	    case 4: case 5:
		cfg.owner()->attrAdd( new TFld("value",_("Value"),TFld::String,Attr::Mutable,"200","","","",21) );
		cfg.owner()->attrAdd( new TFld("items",_("Items"),TFld::String,TFld::FullText|Attr::Mutable,"","","","",22) );
		break;
	    case 6: case 7:
		cfg.owner()->attrAdd( new TFld("value",_("Value"),TFld::Integer,Attr::Mutable,"20","0","","",21) );
		cfg.owner()->attrAdd( new TFld("cfg",_("Config"),TFld::String,Attr::Mutable,"100","0:0:100:1:10","","",22) );
		break;
	}
    }
    else if( cfg.flgGlob()&Attr::Active && cfg.id() == "view" )
    {
	TFld::Type	ntp = TFld::String;
	int 		flg = Attr::Mutable;
	switch(cfg.getI())
	{
	    case 2: case 4:	ntp = TFld::Integer;	break;
	    case 3:		ntp = TFld::Real;	break;
	    case 5: case 6:	ntp = TFld::Integer; flg|=Attr::DataTime;	break;
	}
	int apos = cfg.owner()->attrPos("value");
	cfg.owner()->attrDel("value");
	cfg.owner()->attrAdd( new TFld("value",_("Value"),ntp,flg,"200","","","",21), apos );
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
    return _("Text fields widget of the end visualisation."); 
}

void OrigText::postEnable( int flag )
{
    LWidget::postEnable(flag);
    
    if( flag&TCntrNode::NodeConnect )
    { 
        attrAdd( new TFld("backColor",_("Background:color"),TFld::String,Attr::Color,"","","","",20) );
        attrAdd( new TFld("backImg",_("Background:image"),TFld::String,Attr::Image,"","","","",21) );
        attrAdd( new TFld("bordWidth",_("Border:width"),TFld::Integer,TFld::NoFlag,"","0","","",22) );
        attrAdd( new TFld("bordColor",_("Border:color"),TFld::String,Attr::Color,"","#000000","","",23) );
        attrAdd( new TFld("bordStyle",_("Border:style"),TFld::Integer,TFld::Selected,"","3","0;1;2;3;4;5;6;7;8",
						_("None;Dotted;Dashed;Solid;Double;Groove;Ridge;Inset;Outset"),19) );	
	attrAdd( new TFld("font",_("Font:full"),TFld::String,Attr::Font,"50","Arial 11","","",24) );
	attrAdd( new TFld("fontFamily",_("Font:family"),TFld::String,TFld::NoFlag,"10","Arial","","",25) );
	attrAdd( new TFld("fontSize",_("Font:size"),TFld::Integer,TFld::NoFlag,"2","11","","",26) );
        attrAdd( new TFld("fontBold",_("Font:bold"),TFld::Boolean,TFld::NoFlag,"1","0","","",27) );
        attrAdd( new TFld("fontItalic",_("Font:italic"),TFld::Boolean,TFld::NoFlag,"1","0","","",28) );
        attrAdd( new TFld("fontUnderline",_("Font:underline"),TFld::Boolean,TFld::NoFlag,"1","0","","",29) );
        attrAdd( new TFld("fontStrikeout",_("Font:strikeout"),TFld::Boolean,TFld::NoFlag,"1","0","","",30) );
        attrAdd( new TFld("color",_("Color"),TFld::String,Attr::Color,"20","#000000","","",31) );
        attrAdd( new TFld("orient",_("Orientation angle"),TFld::Integer,TFld::NoFlag,"3","0","-180;180","",32) );
        attrAdd( new TFld("wordWrap",_("Word wrap"),TFld::Boolean,TFld::NoFlag,"1","1","","",33) );
        attrAdd( new TFld("alignment",_("Alignment"),TFld::Integer,TFld::Selected,"1","0","0;1;2;3;4;5;6;7;8;9;10;11",
                          _("Top left;Top right;Top center;Top justify;"
			    "Bottom left;Bottom right;Bottom center;Bottom justify;"
			    "V center left; V center right; Center; V center justify"),34) );
    	attrAdd( new TFld("text",_("Text"),TFld::String,TFld::FullText,"0","Text","","",35) );
    	attrAdd( new TFld("numbArg",_("Arguments number"),TFld::Integer,Attr::Active,"","0","0;10","",36) );
    }
} 

bool OrigText::attrChange( Attr &cfg, void *prev )
{
    if( cfg.flgGlob()&Attr::Active )
    {
     	if( cfg.id() == "numbArg" )
	{
	    string fid("arg"), fnm(_("Argument ")), fidp, fnmp;
	    //- Delete specific unnecessary attributes of parameters -
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
	    //- Create ullage attributes of parameters -
	    for( int i_p = 0; i_p < cfg.getI(); i_p++ )
	    {
		fidp = fid+TSYS::int2str(i_p);
		fnmp = fnm+TSYS::int2str(i_p);
		if( cfg.owner()->attrPresent( fidp+"val" ) ) continue;
		cfg.owner()->attrAdd( new TFld((fidp+"tp").c_str(),(fnmp+_(":type")).c_str(),
		    TFld::Real,TFld::Selected|Attr::Mutable|Attr::Active,"","0","0;1;2",_("Integer;Real;String"),51+10*i_p) );
		cfg.owner()->attrAdd( new TFld((fidp+"val").c_str(),(fnmp+_(":value")).c_str(),
		    TFld::Integer,Attr::Mutable,"","","","",50+10*i_p) );		    		    
		cfg.owner()->attrAdd( new TFld((fidp+"cfg").c_str(),(fnmp+_(":config")).c_str(),
		    TFld::String,Attr::Mutable,"","","","",52+10*i_p) );
	    }
	}
	else if( cfg.fld().reserve() >= 50 && (cfg.fld().reserve()%10) == 1 && *(int*)prev != cfg.getI() )
	{
	    int narg = (cfg.fld().reserve()/10)-5;
	    string fid = "arg"+TSYS::int2str(narg)+"val";
	    string fnm = _("Argument ")+TSYS::int2str(narg)+":value";
	    int apos = cfg.owner()->attrPos(fid);
	    cfg.owner()->attrDel(fid);
	    cfg.owner()->attrAdd( new TFld(fid.c_str(),fnm.c_str(),
			(cfg.getI()==1) ? TFld::Real : ((cfg.getI()==2) ? TFld::String : TFld::Integer),
			Attr::Mutable,"","","","",50+10*narg), apos );
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
    return _("Media widget of the end visualisation."); 
}
 
void OrigMedia::postEnable( int flag )
{
    LWidget::postEnable(flag);
    
    if( flag&TCntrNode::NodeConnect )
    { 	
        attrAdd( new TFld("backColor",_("Background:color"),TFld::String,Attr::Color,"","#FFFFFF","","",20) );
        attrAdd( new TFld("backImg",_("Background:image"),TFld::String,Attr::Image,"","","","",21) );
        attrAdd( new TFld("bordWidth",_("Border:width"),TFld::Integer,TFld::NoFlag,"","0","","",22) );
        attrAdd( new TFld("bordColor",_("Border:color"),TFld::String,Attr::Color,"","#000000","","",23) );
        attrAdd( new TFld("bordStyle",_("Border:style"),TFld::Integer,TFld::Selected,"","3","0;1;2;3;4;5;6;7;8",
						_("None;Dotted;Dashed;Solid;Double;Groove;Ridge;Inset;Outset"),19) );
        attrAdd( new TFld("src",_("Source"),TFld::String,TFld::NoFlag,"50","","","",24) );
    	attrAdd( new TFld("fit",_("Fit to widget size"),TFld::Boolean,TFld::NoFlag,"","","","",26) );
        attrAdd( new TFld("type",_("Type"),TFld::Integer,TFld::Selected|Attr::Active,"1","0","0;1",_("Image;Movie"),25) );
        attrAdd( new TFld("areas",_("Map areas"),TFld::Integer,Attr::Active,"2","0","0;10","",28) );
    }
}

bool OrigMedia::attrChange( Attr &cfg, void *prev )
{
    if( cfg.flgGlob()&Attr::Active )
    {
        if( cfg.id() == "type" )
	{
	    //- Delete specific attributes -
	    if( *(int*)prev == 1 ) cfg.owner()->attrDel("speed");
	
	    //- Create specific attributes -
	    if(cfg.getI() == 1 )
		cfg.owner()->attrAdd( new TFld("speed",_("Play speed"),TFld::Integer,Attr::Mutable,"3","100","1;900","",27) );
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
					       TFld::Integer,TFld::Selected|Attr::Mutable,"1","0","0;1;2","Rect;Poly;Circle",40+3*i_p) );
    		cfg.owner()->attrAdd( new TFld((fidp+"coord").c_str(),(fnmp+_(":coordinates")).c_str(),
					       TFld::String,Attr::Mutable,"","","","",41+3*i_p) );
		cfg.owner()->attrAdd( new TFld((fidp+"title").c_str(),(fnmp+_(":title")).c_str(),
					       TFld::String,Attr::Mutable,"","","","",42+3*i_p) );
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
    return _("Diagram widget of the end visualisation.");
}
 
void OrigDiagram::postEnable( int flag )
{
    LWidget::postEnable(flag);
    
    if( flag&TCntrNode::NodeConnect ) 
    { 
        attrAdd( new TFld("backColor",_("Background:color"),TFld::String,Attr::Color,"","black","","",20) );
        attrAdd( new TFld("backImg",_("Background:image"),TFld::String,Attr::Image,"","","","",21) );
        attrAdd( new TFld("bordWidth",_("Border:width"),TFld::Integer,TFld::NoFlag,"","0","","",22) );
        attrAdd( new TFld("bordColor",_("Border:color"),TFld::String,Attr::Color,"","#000000","","",23) );
        attrAdd( new TFld("bordStyle",_("Border:style"),TFld::Integer,TFld::Selected,"","3","0;1;2;3;4;5;6;7;8",
						_("None;Dotted;Dashed;Solid;Double;Groove;Ridge;Inset;Outset"),19) );
        attrAdd( new TFld("trcPer",_("Tracing period (s)"),TFld::Integer,TFld::NoFlag,"","0","0;360","",24) );
        attrAdd( new TFld("type",_("Type"),TFld::Integer,TFld::Selected|Attr::Active,"1","0","0",_("Trend"),25) );
    }
} 

bool OrigDiagram::attrChange( Attr &cfg, void *prev )
{
    if( cfg.flgGlob()&Attr::Active )
    {
	if( cfg.id() == "active" && cfg.getB() != *(bool*)prev && cfg.owner()->attrPresent("type") && 
		cfg.owner()->attrAt("type").at().getI() == 0 )
	{
	    if( !cfg.getB() )
	    {
	        cfg.owner()->attrDel("curSek");
	        cfg.owner()->attrDel("curUSek");
	        cfg.owner()->attrDel("curColor");
	    }
	    else
	    {
    	        cfg.owner()->attrAdd( new TFld("curSek",_("Cursor:sek"),TFld::Integer,Attr::DataTime|Attr::Mutable,"","","","",29) );
    	        cfg.owner()->attrAdd( new TFld("curUSek",_("Cursor:usek"),TFld::Integer,Attr::Mutable,"","","","",30) );
    		cfg.owner()->attrAdd( new TFld("curColor",_("Cursor:color"),TFld::String,Attr::Color|Attr::Mutable,"","white","","",36) );
	    }
	}
	else if( cfg.id() == "type" )
	{
	    //- Delete specific attributes -
	    switch(*(int*)prev)
	    {
		case 0:
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
	
	    //- Create specific attributes -
	    switch(cfg.getI())
	    {
		case 0:
    		    cfg.owner()->attrAdd( new TFld("tSek",_("Time:sek"),TFld::Integer,Attr::DataTime|Attr::Mutable,"","","","",26) );
    		    cfg.owner()->attrAdd( new TFld("tUSek",_("Time:usek"),TFld::Integer,Attr::Mutable,"","","","",27) );
    		    cfg.owner()->attrAdd( new TFld("tSize",_("Size, sek"),TFld::Real,Attr::Mutable,"","60","","",28) );
		    if( cfg.owner()->attrAt("active").at().getB() )
		    {
    			cfg.owner()->attrAdd( new TFld("curSek",_("Cursor:sek"),TFld::Integer,Attr::DataTime|Attr::Mutable,"","","","",29) );
    			cfg.owner()->attrAdd( new TFld("curUSek",_("Cursor:usek"),TFld::Integer,Attr::Mutable,"","","","",30) );
    			cfg.owner()->attrAdd( new TFld("curColor",_("Cursor:color"),TFld::String,Attr::Color|Attr::Mutable,"","white","","",36) );
		    }
    		    cfg.owner()->attrAdd( new TFld("sclColor",_("Scale:color"),TFld::String,Attr::Color|Attr::Mutable,"","grey","","",31) );
    		    cfg.owner()->attrAdd( new TFld("sclHor",_("Scale:horizontal"),TFld::Integer,Attr::Mutable|TFld::Selected,
			"1","0","0;1;2;3",_("No draw;Grid;Markers;Grid and markers"),32) );
    		    cfg.owner()->attrAdd( new TFld("sclVer",_("Scale:vertical"),TFld::Integer,Attr::Mutable|TFld::Selected,
			"1","0","0;1;2;3;5;6;7",_("No draw;Grid;Markers;Grid and markers;Grid (log);Marker (log);Grid and markers (log)"),33) );
    		    cfg.owner()->attrAdd( new TFld("sclMarkColor",_("Scale:Markers:color"),TFld::String,Attr::Color|Attr::Mutable,"","white","","",37) );
    		    cfg.owner()->attrAdd( new TFld("sclMarkFont",_("Scale:Markers:font"),TFld::String,Attr::Font|Attr::Mutable,"","Arial 10","","",38) );
    		    cfg.owner()->attrAdd( new TFld("valArch",_("Value archivator"),TFld::String,Attr::Mutable,"","","","",34) );
    		    cfg.owner()->attrAdd( new TFld("parNum",_("Parameters number"),TFld::Integer,Attr::Mutable|Attr::Active,"1","1","0;10","",35) );
		    break;
	    }
	}
	else if( cfg.id() == "parNum" )
	{
	    string fid("prm"), fnm(_("Parametr ")), fidp, fnmp;
	    //- Delete specific unnecessary attributes of parameters -
	    for( int i_p = 0; true; i_p++ )
	    {
		fidp = fid+TSYS::int2str(i_p); 
		if( !cfg.owner()->attrPresent( fidp+"addr" ) )	break;
		else if( i_p >= cfg.getI() )
		{
		    cfg.owner()->attrDel(fidp+"addr");
		    cfg.owner()->attrDel(fidp+"bordL");
		    cfg.owner()->attrDel(fidp+"bordU");
		    cfg.owner()->attrDel(fidp+"color");
		    cfg.owner()->attrDel(fidp+"val");
		}
	    }
	    //- Create ullage attributes of parameters -
	    for( int i_p = 0; i_p < cfg.getI(); i_p++ )
	    {
		fidp = fid+TSYS::int2str(i_p);
		fnmp = fnm+TSYS::int2str(i_p);
		if( cfg.owner()->attrPresent( fidp+"addr" ) ) continue;
    		cfg.owner()->attrAdd( new TFld((fidp+"addr").c_str(),(fnmp+_(":address")).c_str(),
					       TFld::String,Attr::Address|Attr::Mutable,"","","","",50+10*i_p) );
    		cfg.owner()->attrAdd( new TFld((fidp+"bordL").c_str(),(fnmp+_(":view border:lower")).c_str(),
					       TFld::Real,Attr::Mutable,"","","","",51+10*i_p) );
    		cfg.owner()->attrAdd( new TFld((fidp+"bordU").c_str(),(fnmp+_(":view border:upper")).c_str(),
					       TFld::Real,Attr::Mutable,"","","","",52+10*i_p) );
    		cfg.owner()->attrAdd( new TFld((fidp+"color").c_str(),(fnmp+_(":color")).c_str(),
					       TFld::String,Attr::Color|Attr::Mutable,"","","","",53+10*i_p) );
    		cfg.owner()->attrAdd( new TFld((fidp+"val").c_str(),(fnmp+_(":value")).c_str(),
					       TFld::Real,Attr::Mutable,"","","","",54+10*i_p) );
	    }
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
    return _("Protocol widget of the end visualisation.");
}

void OrigProtocol::postEnable( int flag )
{
    LWidget::postEnable(flag);
    
    if( flag&TCntrNode::NodeConnect ) 
    { 
        attrAdd( new TFld("backColor",_("Background:color"),TFld::String,Attr::Color,"","","","",20) );
        attrAdd( new TFld("backImg",_("Background:image"),TFld::String,Attr::Image,"","","","",21) );
    	attrAdd( new TFld("time",_("Time, sek"),TFld::Integer,Attr::DataTime,"","","","",24) );
        attrAdd( new TFld("tSize",_("Size, sek"),TFld::Integer,TFld::NoFlag,"","60","","",25) );
        attrAdd( new TFld("trcPer",_("Tracing period (s)"),TFld::Integer,TFld::NoFlag,"","0","0;360","",26) );
        attrAdd( new TFld("arch",_("Archivator"),TFld::String,TFld::NoFlag,"","","","",27) );
        attrAdd( new TFld("tmpl",_("Template"),TFld::String,TFld::NoFlag,"","","","",28) );
        attrAdd( new TFld("lev",_("Level"),TFld::Integer,TFld::NoFlag,"","0","","",29) );
        attrAdd( new TFld("viewOrd",_("View order"),TFld::Integer,TFld::Selected,"","0",
	    "0;1;2",_("On time;On level;On level and trigered"),30) );
        attrAdd( new TFld("col",_("View columns"),TFld::String,TFld::NoFlag,"","pos;tm;lev;cat;mess","","",31) );
        attrAdd( new TFld("itProp",_("Items properties"),TFld::Integer,Attr::Active,"","0","0;10","",32) );
    }
} 

bool OrigProtocol::attrChange( Attr &cfg, void *prev )
{
    if( cfg.flgGlob()&Attr::Active )
    {
	if( cfg.id() == "itProp" )
	{
	    string fid("it"), fnm(_("Item ")), fidp, fnmp;
	    //- Delete specific unnecessary items -
	    for( int i_p = 0; true; i_p++ )
	    {
		fidp = fid+TSYS::int2str(i_p); 
		if( !cfg.owner()->attrPresent( fidp+"lev" ) )	break;
		else if( i_p >= cfg.getI() )
		{
		    cfg.owner()->attrDel(fidp+"lev");
		    cfg.owner()->attrDel(fidp+"tmpl");
		    cfg.owner()->attrDel(fidp+"fnt");
		    cfg.owner()->attrDel(fidp+"img");
		    cfg.owner()->attrDel(fidp+"color");
		    cfg.owner()->attrDel(fidp+"blink");
		}
	    }
	    //- Create ullage items -
	    for( int i_p = 0; i_p < cfg.getI(); i_p++ )
	    {
		fidp = fid+TSYS::int2str(i_p);
		fnmp = fnm+TSYS::int2str(i_p);
		if( cfg.owner()->attrPresent( fidp+"lev" ) ) continue;
    		cfg.owner()->attrAdd( new TFld((fidp+"lev").c_str(),(fnmp+_(":levels")).c_str(),
					       TFld::String,Attr::Mutable,"","","","",50+10*i_p) );
    		cfg.owner()->attrAdd( new TFld((fidp+"tmpl").c_str(),(fnmp+_(":template")).c_str(),
					       TFld::String,Attr::Mutable,"","","","",51+10*i_p) );
    		cfg.owner()->attrAdd( new TFld((fidp+"fnt").c_str(),(fnmp+_(":font")).c_str(),
					       TFld::String,Attr::Font|Attr::Mutable,"","","","",52+10*i_p) );
    		cfg.owner()->attrAdd( new TFld((fidp+"color").c_str(),(fnmp+_(":color")).c_str(),
					       TFld::String,Attr::Color|Attr::Mutable,"","","","",53+10*i_p) );
    		cfg.owner()->attrAdd( new TFld((fidp+"blink").c_str(),(fnmp+_(":blink")).c_str(),
					       TFld::Boolean,Attr::Mutable,"","0","","",54+10*i_p) );
	    }
	}
    }
    return Widget::attrChange(cfg,prev);    
}

//************************************************
//* OrigDocument: Document original widget       *
//************************************************
OrigDocument::OrigDocument( ) : PrWidget("Document")
{ 
    
}
    
string OrigDocument::name( )      
{ 
    return _("Document"); 
}

string OrigDocument::descr( )     
{ 
    return _("Document widget of the end visualisation."); 
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
    return _("Elements box widget of the end visualisation.");
}

void OrigBox::postEnable( int flag )
{
    LWidget::postEnable(flag);
    
    if( flag&TCntrNode::NodeConnect )  
    { 
        attrAdd( new TFld("backColor",_("Background:color"),TFld::String,Attr::Color,"","#FFFFFF","","",20) );
        attrAdd( new TFld("backImg",_("Background:image"),TFld::String,Attr::Image,"","","","",21) );
        attrAdd( new TFld("bordWidth",_("Border:width"),TFld::Integer,TFld::NoFlag,"","0","","",22) );
        attrAdd( new TFld("bordColor",_("Border:color"),TFld::String,Attr::Color,"","#000000","","",23) );
        attrAdd( new TFld("bordStyle",_("Border:style"),TFld::Integer,TFld::Selected,"","3","0;1;2;3;4;5;6;7;8",
						_("None;Dotted;Dashed;Solid;Double;Groove;Ridge;Inset;Outset"),19) );
	attrAdd( new TFld("pgOpen",_("Page:open state"),TFld::Boolean,TFld::NoFlag,"","","","",24) );
	attrAdd( new TFld("pgNoOpenProc",_("Page:no open process"),TFld::Boolean,TFld::NoFlag,"","","","",25) );
        attrAdd( new TFld("pgOpenSrc",_("Page:open source"),TFld::String,TFld::NoFlag,"","","","",26) );
        attrAdd( new TFld("pgGrp",_("Page:group"),TFld::String,TFld::NoFlag,"","","","",27) );
	attrAdd( new TFld("pgFullScr",_("Page:full screen"),TFld::Boolean,TFld::NoFlag,"","","","",28) );
    }
} 

//************************************************
//* OrigLink: Link original widget               *
//************************************************
OrigLink::OrigLink( ) : PrWidget("Link")
{

}
 
string OrigLink::name( )      
{ 
    return _("Interwidget link"); 
}

string OrigLink::descr( )     
{ 
    return _("Interwidget link of the end visualisation."); 
}
 
void OrigLink::postEnable( int flag )
{
    LWidget::postEnable(flag);
    
    if( flag&TCntrNode::NodeConnect )  
    { 
    	attrAdd( new TFld("out",_("Output"),TFld::String,TFld::NoFlag,"50","","","",20) );
    	attrAdd( new TFld("in",_("Input"),TFld::String,TFld::NoFlag,"50","","","",21) );
    	attrAdd( new TFld("lineWdth",_("Line:width"),TFld::Integer,TFld::NoFlag,"2","1","0;99","",22) );
    	attrAdd( new TFld("lineClr",_("Line:color"),TFld::String,Attr::Color,"20","#000000","","",23) );
    	attrAdd( new TFld("lineSquare",_("Line:square angle"),TFld::Boolean,TFld::NoFlag,"1","0","","",24) );
	//Next is dynamic created internal points    
    }
} 
