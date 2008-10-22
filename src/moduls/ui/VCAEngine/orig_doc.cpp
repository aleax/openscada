
//OpenSCADA system module UI.VCAEngine file: orig_doc.cpp
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

#include <tsys.h>

#include "orig_doc.h"
#include "vcaengine.h"

using namespace VCA;

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

void OrigDocument::postEnable( int flag )
{
    LWidget::postEnable(flag);

    if( flag&TCntrNode::NodeConnect )
    {
	attrAdd( new TFld("backColor",_("Background:color"),TFld::String,Attr::Color,"","","","","20") );
	attrAdd( new TFld("backImg",_("Background:image"),TFld::String,Attr::Image,"","","","","21") );
	attrAdd( new TFld("style",_("CSS"),TFld::String,TFld::FullText,"","","","","22") );
	attrAdd( new TFld("tmpl",_("Template"),TFld::String,TFld::FullText,"","","","","23") );
	attrAdd( new TFld("n",_("Documents' number"),TFld::Integer,Attr::Active,"","0","0;99","","24") );
	attrAdd( new TFld("doc",_("Current document"),TFld::String,TFld::FullText,"","","","","25") );
	attrAdd( new TFld("time",_("Time:current"),TFld::Integer,Attr::DataTime,"","0","","","26") );
	attrAdd( new TFld("bTime",_("Time:begin"),TFld::Integer,Attr::DataTime,"","0","","","27") );
	attrAdd( new TFld("trcPer",_("Tracing period (s)"),TFld::Integer,TFld::NoFlag,"","0","0;360","","28") );
    }
}

bool OrigDocument::attrChange( Attr &cfg, void *prev )
{
    //- Document's number change process -
    if( cfg.id() == "n" && cfg.getI() != *(int*)prev )
    {
	if( !cfg.getI() )
	{
	    cfg.owner()->attrDel("aCur");
	    cfg.owner()->attrDel("vCur");
	}
	else
	{
	    cfg.owner()->attrAdd( new TFld("aCur",_("Cursor:archive"),TFld::Integer,Attr::Mutable,"","0","0;99","","30") );
	    cfg.owner()->attrAdd( new TFld("vCur",_("Cursor:view"),TFld::Integer,Attr::Mutable,"","0","0;99","","31") );
	}
	string fidp;
	//- Delete archive document's attributes -
	for( int i_p = 0; true; i_p++ )
	{
	    fidp = "doc"+TSYS::int2str(i_p);
	    if( !cfg.owner()->attrPresent(fidp) )      break;
	    else if( i_p >= cfg.getI() )	cfg.owner()->attrDel(fidp);
	}
	//- Create archive document's attributes -
	for( int i_p = 0; i_p < cfg.getI(); i_p++ )
	{
	    fidp = "doc"+TSYS::int2str(i_p);
	    if( cfg.owner()->attrPresent(fidp) ) continue;
	    cfg.owner()->attrAdd( new TFld(fidp.c_str(),(_("Document ")+TSYS::int2str(i_p)).c_str(),TFld::String,
		TFld::FullText|Attr::Mutable,"","","","",TSYS::int2str(50+i_p).c_str()) );
	}
    }
    //- Make document after time set -
    else if( cfg.id() == "time" && cfg.getI() != *(int*)prev )
    {
	string mkDk = makeDoc(cfg.owner()->attrAt("tmpl").at().getS());
	if( !cfg.owner()->attrAt("n").at().getI() )	cfg.owner()->attrAt("doc").at().setS(mkDk);
	else
	{
	    int aCur = cfg.owner()->attrAt("aCur").at().getI();
	    int vCur = cfg.owner()->attrAt("vCur").at().getI();
	    cfg.owner()->attrAt("doc"+TSYS::int2str(aCur)).at().setS(mkDk);
	    if( aCur == vCur )	cfg.owner()->attrAt("doc").at().setS(mkDk);
	}
    }
    //- Move archive cursor -
    else if( cfg.id() == "aCur" && cfg.getI() != *(int*)prev )
    {
	int n = cfg.owner()->attrAt("n").at().getI();
	if( cfg.getI() < 0 )		cfg.owner()->attrAt("aCur").at().setI( ((*(int*)prev)+1 >= n) ? 0 : (*(int*)prev)+1 );
	else if( cfg.getI() >= n )	cfg.owner()->attrAt("aCur").at().setI( n-1 );
	if( *(int*)prev == cfg.owner()->attrAt("vCur").at().getI() )
	    cfg.owner()->attrAt("vCur").at().setI(cfg.owner()->attrAt("aCur").at().getI());
    }
    //- Move archive view cursor -
    else if( cfg.id() == "vCur" && cfg.getI() != *(int*)prev )
    {
	int aCur = cfg.owner()->attrAt("aCur").at().getI();
	int n = cfg.owner()->attrAt("n").at().getI();
	if( cfg.getI() < 0 )
	{
	    int docN = *(int*)prev;
	    //-- Search next document --
	    if( cfg.getI() == -1 )
		while( docN != aCur && (docN == *(int*)prev || cfg.owner()->attrAt("doc"+TSYS::int2str(docN)).at().getS().empty()) )
		    if( ++docN >= n )	docN = 0;
	    //- Search previous document -
	    else
	    {
		if( --docN < 0 ) docN = n-1;
		if( cfg.owner()->attrAt("doc"+TSYS::int2str(docN)).at().getS().empty() )	docN = *(int*)prev;
	    }
	    //- Copy selected document to attribut doc -
	    if( docN != *(int*)prev )	cfg.setI(docN);
	}
	else if( cfg.getI() >= n )	cfg.setI( cfg.owner()->attrAt("aCur").at().getI() );
	else cfg.owner()->attrAt("doc").at().setS( cfg.owner()->attrAt("doc"+TSYS::int2str(cfg.getI())).at().getS() );
    }

    return Widget::attrChange(cfg,prev);
}

string OrigDocument::makeDoc( const string &tmpl )
{
    return "";
}
