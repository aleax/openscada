
//OpenSCADA system module UI.VCAEngine file: origwidg.cpp
/***************************************************************************
 *   Copyright (C) 2006-2007 by Roman Savochenko
 *   rom_as@diyaorg.dp.ua
 *
 *   This program is free software; you can redistribute it and/or modify
 *   it under the terms of the GNU General Public License as published by
 *   the Free Software Foundation; either version 2 of the License, or
 *   (at your option) any later version.
 *
 *   This program is distributed in the hope that it will be useful,
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *   GNU General Public License for more details.
 *
 *   You should have received a copy of the GNU General Public License
 *   along with this program; if not, write to the
 *   Free Software Foundation, Inc.,
 *   59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
 ***************************************************************************/

#include <tsys.h>

#include "origwidg.h"
#include "vcaengine.h"
		  
using namespace VCA;

//*************************************************
//* Primitive widget template                     *
//*************************************************
PrWidget::PrWidget( const string &iid ) : LWidget(iid)
{ 

}
    
string PrWidget::ico( )
{
    if( LWidget::ico().size() )
        return LWidget::ico();
    if( TUIS::icoPresent("VCA.wdg_"+id()) )
        return TSYS::strEncode(TUIS::icoGet("VCA.wdg_"+id()),TSYS::base64);
    return "";
}

void PrWidget::cntrCmdProc( XMLNode *opt )
{
    //Get page info
    if( opt->name() == "info" )
    {
        cntrCmdGeneric(opt);
        cntrCmdAttributes(opt );
        ctrMkNode("oscada_cntr",opt,-1,"/",_("Base widget: ")+id());
	ctrMkNode("fld",opt,-1,"/wdg/st/parent",_("Parent"),R_R_R_,user().c_str(),grp().c_str(),
		2,"tp","str","dest","");		
        return;
    }
    //Process command to page
    string a_path = opt->attr("path");
    if( a_path == "/wdg/st/parent" && ctrChkNode(opt,"get",R_R_R_,user().c_str(),grp().c_str(),SEQ_RD) )
	opt->setText(parentNm());
    else cntrCmdGeneric(opt) || cntrCmdAttributes(opt);
}

void PrWidget::preDisable(int flag)
{
    if( flag )
	throw TError(mod->nodePath().c_str(),_("Deleting of main primitive widgets is error."));
    
    Widget::preDisable(flag);
}

//============ Original widgets based at primitive widget template ============

//*************************************************
//* Elementary figures original widget            *
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
	attrAdd( new TFld("lineWdth",_("Line:width"),TFld::Integer,TFld::NoFlag,"2","1","0;99") );
	attrAdd( new TFld("lineClr",_("Line:color"),TFld::String,Attr::Color,"20","#000000") );
	attrAdd( new TFld("lineDecor",_("Line:decorate"),TFld::Integer,TFld::Selected,"1","0","0;1",_("No decor;Pipe")) );
	attrAdd( new TFld("bordWdth",_("Border:width"),TFld::Integer,TFld::NoFlag,"2","0","0;99") );
	attrAdd( new TFld("bordClr",_("Border:color"),TFld::String,Attr::Color,"20","#000000") );
	attrAdd( new TFld("backgClr",_("Background:color"),TFld::String,Attr::Color,"20","") );
	attrAdd( new TFld("backgImg",_("Background:image"),TFld::String,Attr::Image,"20","") );
	attrAdd( new TFld("elLst",_("Element's list"),TFld::String,TFld::FullText,"300","") );
    }
}

//*************************************************
//* Form element original widget                  *
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
    {
	attrAdd( new TFld("elType",_("Element type"),TFld::Integer,TFld::Selected|Attr::Active,"2","0","0;1;2;3;4;5",
                    	       _("Line edit;Text edit;Chek box;Button;Combo box;List")) );
    }
}
								  
bool OrigFormEl::attrChange( Attr &cfg )
{
    if( cfg.flgGlob()&Attr::Active && cfg.id() == "elType" )
    {
	//- Delete specific attributes -
	if( cfg.owner()->attrPresent("value") )		cfg.owner()->attrDel("value");
	if( cfg.owner()->attrPresent("view") )		cfg.owner()->attrDel("view");
	if( cfg.owner()->attrPresent("cfg") )		cfg.owner()->attrDel("cfg");
	if( cfg.owner()->attrPresent("wordWrap") )	cfg.owner()->attrDel("wordWrap");
	if( cfg.owner()->attrPresent("text") )		cfg.owner()->attrDel("text");
	if( cfg.owner()->attrPresent("img") )		cfg.owner()->attrDel("img");
	if( cfg.owner()->attrPresent("color") )		cfg.owner()->attrDel("color");
	if( cfg.owner()->attrPresent("checkable") )	cfg.owner()->attrDel("checkable");
	if( cfg.owner()->attrPresent("items") )		cfg.owner()->attrDel("items");
	
	//- Create specific attributes -
	int tp = cfg.getI();	
	switch(tp)
	{
	    case 0: 
		cfg.owner()->attrAdd( new TFld("view",_("View"),TFld::Integer,TFld::Selected|Attr::Mutable,
		    "1","0","0;1;2;3;4;5;6",_("Text;Combo;Integer;Real;Time;Data;Data and time")) );
		cfg.owner()->attrAdd( new TFld("value",_("Value"),TFld::String,Attr::Mutable,"200") );
		cfg.owner()->attrAdd( new TFld("cfg",_("Config"),TFld::String,TFld::FullText|Attr::Mutable) );
		break;
	    case 1: 
		cfg.owner()->attrAdd( new TFld("value",_("Value"),TFld::String,TFld::FullText|Attr::Mutable) );
		cfg.owner()->attrAdd( new TFld("wordWrap",_("Word wrap"),TFld::Boolean,Attr::Mutable,"1","1") );
		break;
	    case 2:
		cfg.owner()->attrAdd( new TFld("value",_("Value"),TFld::Boolean,Attr::Mutable) );
		break;
	    case 3:
		cfg.owner()->attrAdd( new TFld("value",_("Value"),TFld::Boolean,Attr::Mutable) );
		cfg.owner()->attrAdd( new TFld("img",_("Image"),TFld::String,Attr::Image|Attr::Mutable) );
		cfg.owner()->attrAdd( new TFld("color",_("Color"),TFld::String,Attr::Color|Attr::Mutable,"20") );
		cfg.owner()->attrAdd( new TFld("checkable",_("Checkable"),TFld::Boolean,Attr::Mutable) );
		break;
	    case 4:
		cfg.owner()->attrAdd( new TFld("value",_("Value"),TFld::String,Attr::Mutable,"200") );
		cfg.owner()->attrAdd( new TFld("items",_("Items"),TFld::String,TFld::FullText|Attr::Mutable) );
		break;
	    case 5:
		cfg.owner()->attrAdd( new TFld("value",_("Value"),TFld::String,Attr::Mutable,"200") );
		cfg.owner()->attrAdd( new TFld("items",_("Items"),TFld::String,TFld::FullText|Attr::Mutable) );
		break;
	}
    }
    return true;
}

//************************************************
//* Text element original widget                 *
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
    	attrAdd( new TFld("text",_("Text"),TFld::String,TFld::FullText,"1000","Text") );
	attrAdd( new TFld("font",_("Font:full"),TFld::String,TFld::NoFlag,"50","Arial 11") );
	attrAdd( new TFld("fontFamily",_("Font:family"),TFld::String,TFld::NoFlag,"10","Arial") );
	attrAdd( new TFld("fontSize",_("Font:size"),TFld::Integer,TFld::NoFlag,"2","11") );
        attrAdd( new TFld("fontBold",_("Font:bold"),TFld::Boolean,TFld::NoFlag,"1","0") );
        attrAdd( new TFld("fontItalic",_("Font:italic"),TFld::Boolean,TFld::NoFlag,"1","0") );
        attrAdd( new TFld("fontUnderline",_("Font:underline"),TFld::Boolean,TFld::NoFlag,"1","0") );
        attrAdd( new TFld("fontStrikeout",_("Font:strikeout"),TFld::Boolean,TFld::NoFlag,"1","0") );
        attrAdd( new TFld("color",_("Color"),TFld::String,Attr::Color,"20","#000000") );
        attrAdd( new TFld("orient",_("Orientation angle"),TFld::Integer,TFld::NoFlag,"3","0","-180;180") );
        attrAdd( new TFld("wordWrap",_("Word wrap"),TFld::Boolean,TFld::NoFlag,"1","1") );
        attrAdd( new TFld("alignment",_("Alignment"),TFld::Integer,TFld::Selected,"1","0","0;1;2;3;4;5;6;7;8;9;10;11",
                          _("Top left;Top right;Top center;Top justify;"
			    "Bottom left;Bottom right;Bottom center;Bottom justify;"
			    "V center left; V center right; Center; V center justify")) );
        attrAdd( new TFld("backColor",_("Background:color"),TFld::String,Attr::Color,"","") );
        attrAdd( new TFld("backImg",_("Background:image"),TFld::String,Attr::Image,"","") );
        attrAdd( new TFld("bordWidth",_("Border:width"),TFld::Integer,TFld::NoFlag,"","0") );
        attrAdd( new TFld("bordColor",_("Border:color"),TFld::String,Attr::Color,"","#000000") );	
    }
} 

//************************************************
//* Media view original widget                   *
//************************************************
OrigMedia::OrigMedia( ) : PrWidget("Media")
{

}
    
string OrigMedia::name( )
{ 
    return _("Media view"); 
}

string OrigMedia::descr( )	
{ 
    return _("Media view widget of the end visualisation."); 
}
 
void OrigMedia::postEnable( int flag )
{
    LWidget::postEnable(flag);
    
    if( flag&TCntrNode::NodeConnect )
    { 	
        attrAdd( new TFld("backColor",_("Background:color"),TFld::String,Attr::Color,"","#FFFFFF") );
        attrAdd( new TFld("backImg",_("Background:image"),TFld::String,Attr::Image,"","") );
        attrAdd( new TFld("bordWidth",_("Border:width"),TFld::Integer,TFld::NoFlag,"","0") );
        attrAdd( new TFld("bordColor",_("Border:color"),TFld::String,Attr::Color,"","#000000") );
        attrAdd( new TFld("src",_("Source"),TFld::String,TFld::NoFlag,"50","") );	
        attrAdd( new TFld("type",_("Type"),TFld::Integer,TFld::Selected|Attr::Active,"1","0","0;1",_("Image;Movie")) );	
    }
}

bool OrigMedia::attrChange( Attr &cfg )
{
    if( cfg.flgGlob()&Attr::Active && cfg.id() == "type" )
    {
	//- Delete specific attributes -
	if( cfg.owner()->attrPresent("scale") )		cfg.owner()->attrDel("scale");
	if( cfg.owner()->attrPresent("fit") )		cfg.owner()->attrDel("fit");
	if( cfg.owner()->attrPresent("play") )		cfg.owner()->attrDel("play");
	if( cfg.owner()->attrPresent("speed") )		cfg.owner()->attrDel("speed");
	
	//- Create specific attributes -
	int tp = cfg.getI();	
	switch(tp)
	{
	    case 0:
    		cfg.owner()->attrAdd( new TFld("scale",_("Scale ratio (0-10)"),TFld::Real,Attr::Mutable,"2.2","1","0:10") );
		break;
	    case 1:	    
    		cfg.owner()->attrAdd( new TFld("play",_("Play"),TFld::Boolean,Attr::Mutable) );
		cfg.owner()->attrAdd( new TFld("speed",_("Play speed"),TFld::Integer,Attr::Mutable,"3","100","0;900") );
    		cfg.owner()->attrAdd( new TFld("fit",_("Fit to widget size"),TFld::Boolean,Attr::Mutable) );
		break;
	}
    }
    return true;
}

//************************************************
//* Trend view original widget                   *
//************************************************
OrigTrend::OrigTrend( ) : PrWidget("Trend")
{

}
    
string OrigTrend::name( )      
{ 
    return _("Trend view"); 
}

string OrigTrend::descr( )     
{ 
    return _("Trend view widget of the end visualisation."); 
}
 
void OrigTrend::postEnable( int flag )
{
    LWidget::postEnable(flag);
    
    if( flag&TCntrNode::NodeConnect ) 
    { 
        attrAdd( new TFld("type",_("Type"),TFld::Integer,TFld::Selected,"1","0","0;1",
                          _("Tradition;Cyrcle")) );
        attrAdd( new TFld("widthTime",_("Width time (ms)"),TFld::Integer,TFld::Selected,"6","60000","10;360000") );
        attrAdd( new TFld("number",_("Number"),TFld::Integer,TFld::Selected,"1","0","1;2;3;4;5;6;7;8","1;2;3;4;5;6;7;8") );
	//Next is dynamic created individual trend's item attributes    
    }
} 

//************************************************
//* Protocol view original widget                *
//************************************************
OrigProtocol::OrigProtocol( ) : PrWidget("Protocol")
{ 
    
}
    
string OrigProtocol::name( )      
{ 
    return _("Protocol view"); 
}

string OrigProtocol::descr( )	
{ 
    return _("Protocol view widget of the end visualisation."); 
}

//************************************************
//* Document view original widget                *
//************************************************
OrigDocument::OrigDocument( ) : PrWidget("Document")
{ 
    
}
    
string OrigDocument::name( )      
{ 
    return _("Document view"); 
}

string OrigDocument::descr( )     
{ 
    return _("Document view widget of the end visualisation."); 
}

//************************************************
//* User function original widget                *
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
    return _("Buildin function widget of the end visualisation."); 
}

//************************************************
//* User element original widget                 *
//************************************************
OrigUserEl::OrigUserEl( ) : PrWidget("UserEl")
{

}

string OrigUserEl::name( )
{ 
    return _("User element"); 
}

string OrigUserEl::descr( )
{ 
    return _("User element widget of the end visualisation."); 
}

void OrigUserEl::postEnable( int flag )
{
    LWidget::postEnable(flag);
    
    if( flag&TCntrNode::NodeConnect )  
    { 
        attrAdd( new TFld("backColor",_("Background:color"),TFld::String,Attr::Color,"","#FFFFFF") );
        attrAdd( new TFld("backImg",_("Background:image"),TFld::String,Attr::Image,"","") );
        attrAdd( new TFld("bordWidth",_("Border:width"),TFld::Integer,TFld::NoFlag,"","0") );
        attrAdd( new TFld("bordColor",_("Border:color"),TFld::String,Attr::Color,"","#000000") );
	attrAdd( new TFld("pgOpen",_("Page:open state"),TFld::Boolean,TFld::NoFlag) );
	attrAdd( new TFld("pgNoOpenProc",_("Page:no open process"),TFld::Boolean,TFld::NoFlag) );
        attrAdd( new TFld("pgOpenSrc",_("Page:open source"),TFld::String,TFld::NoFlag) );
        attrAdd( new TFld("pgGrp",_("Page:group"),TFld::String,TFld::NoFlag) );
    }
} 

//************************************************
//* Link original widget                         *
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
    	attrAdd( new TFld("out",_("Output"),TFld::String,TFld::NoFlag,"50","") );
    	attrAdd( new TFld("in",_("Input"),TFld::String,TFld::NoFlag,"50","") );
    	attrAdd( new TFld("lineWdth",_("Line:width"),TFld::Integer,TFld::NoFlag,"2","1","0;99") );
    	attrAdd( new TFld("lineClr",_("Line:color"),TFld::String,Attr::Color,"20","#000000") );
    	attrAdd( new TFld("lineSquare",_("Line:square angle"),TFld::Boolean,TFld::NoFlag,"1","0") );
	//Next is dynamic created internal points    
    }
} 
