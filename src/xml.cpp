
//OpenSCADA system file: xml.cpp
/***************************************************************************
 *   Copyright (C) 2003-2008 by Roman Savochenko                           *
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

#include "terror.h"
#include "tmess.h"
#include "xml.h"

//*************************************************
//* XMLNode                                       *
//*************************************************
XMLNode &XMLNode::operator=(XMLNode &prm)
{
    //- Delete self children and atributes -
    mAttr.clear();
    mPrcInstr.clear();
    for( int i_ch = 0; i_ch < mChildren.size(); i_ch++ )
	delete mChildren[i_ch];
    mChildren.clear();

    //- Copy params (name,text, atributes and instructions) -
    setName( prm.name() );
    setText( prm.text() );
    vector<string> ls;
    prm.attrList(ls);
    for( int i_a = 0; i_a < ls.size(); i_a++)
	setAttr(ls[i_a],prm.attr(ls[i_a]));
    prm.prcInstrList(ls);
    for( int i_p = 0; i_p < ls.size(); i_p++)
	setPrcInstr(ls[i_p],prm.prcInstr(ls[i_p]));

    //- Recursive copy children -
    for( int i_ch = 0; i_ch < prm.childSize(); i_ch++ )
	*childAdd() = *prm.childGet(i_ch);

    return *this;
}

void XMLNode::childAdd( XMLNode * n )
{
    if( !n )	return;
    mChildren.push_back(n);
    n->mParent = this;
}

XMLNode* XMLNode::childAdd( const string &name )
{
    XMLNode *n = new XMLNode( name );
    childAdd(n);

    return n;
}

void XMLNode::childDel( const unsigned id )
{
    if( id >= childSize() ) throw TError("XMLNode","Child %d is not present.",id);
    delete mChildren[id];
    mChildren.erase( mChildren.begin()+id );
}

void XMLNode::childClear( const string &name )
{
    for( int i_ch = 0; i_ch < mChildren.size(); i_ch++ )
	if( !name.size() || mChildren[i_ch]->name() == name )
	    childDel(i_ch--);
}

int XMLNode::childIns( unsigned id, XMLNode * n )
{
    if( !n )	return -1;

    if( id > childSize() ) id = childSize();
    mChildren.insert( mChildren.begin()+id, n );
    n->mParent = this;

    return id;
}

XMLNode* XMLNode::childIns( unsigned id, const string &name )
{
    XMLNode *n = new XMLNode( name );
    childIns(id,n);

    return n;
}

XMLNode* XMLNode::childGet( const int index, bool noex ) const
{
    if( index < childSize() )	return mChildren[index];
    if( noex )	return NULL;
    throw TError("XMLNode","Child %d is not present.",index);
}

XMLNode* XMLNode::childGet( const string &name, const int numb, bool noex ) const
{
    for( int i_ch = 0, i_n = 0; i_ch < childSize(); i_ch++ )
	if( childGet(i_ch)->name() == name && i_n++ == numb )
	    return childGet(i_ch);

    if( noex ) return NULL;
    throw TError("XMLNode","Child %s:%d is not found!",name.c_str(),numb);
}

XMLNode* XMLNode::childGet( const string &attr, const string &val, bool noex ) const
{
    for( unsigned i_f = 0; i_f < childSize(); i_f++ )
	if( childGet(i_f)->attr(attr) == val ) return childGet(i_f);

    if( noex ) return NULL;
    throw TError("XMLNode","Child with attribut %s=%s is not present.",attr.c_str(),val.c_str());
}

void XMLNode::attrList( vector<string> & list ) const
{
    list.clear();
    for( unsigned i_a = 0; i_a < mAttr.size(); i_a++ )
	list.push_back(mAttr[i_a].first);
}

XMLNode* XMLNode::attrDel( const string &name )
{
    for( unsigned i_a = 0; i_a < mAttr.size(); i_a++ )
	if( mAttr[i_a].first == name )
	{ mAttr.erase(mAttr.begin()+i_a); break; }

    return this;
}

void XMLNode::attrClear( )
{
    mAttr.clear();
}

string XMLNode::attr( const string &name ) const
{
    for( unsigned i_a = 0; i_a < mAttr.size(); i_a++ )
	if( mAttr[i_a].first == name ) return mAttr[i_a].second;

    return "";
}

XMLNode* XMLNode::setAttr( const string &name, const string &val )
{
    for( unsigned i_a = 0; i_a < mAttr.size(); i_a++ )
	if( mAttr[i_a].first == name )
	{
	    mAttr[i_a].second = val;
	    return this;
	}

    mAttr.push_back(pair<string,string>(name,val));

    return this;
}

void XMLNode::prcInstrList( vector<string> & list ) const
{
    list.clear();
    for( unsigned i_p = 0; i_p < mPrcInstr.size(); i_p++ )
	list.push_back(mPrcInstr[i_p].first);
}

void XMLNode::prcInstrDel( const string &target )
{
    for( unsigned i_p = 0; i_p < mPrcInstr.size(); i_p++ )
	if( mPrcInstr[i_p].first == target )
	{ mPrcInstr.erase(mPrcInstr.begin()+i_p); return; }
}

void XMLNode::prcInstrClear( )
{
    mPrcInstr.clear();
}

string XMLNode::prcInstr( const string &target ) const
{
    for( unsigned i_p = 0; i_p < mPrcInstr.size(); i_p++ )
	if( mPrcInstr[i_p].first == target ) return mPrcInstr[i_p].second;

    return "";
}

XMLNode* XMLNode::setPrcInstr( const string &target, const string &val )
{
    for( unsigned i_p = 0; i_p < mPrcInstr.size(); i_p++ )
	if( mPrcInstr[i_p].first == target )
	{
	    mPrcInstr[i_p].second = val;
	    return this;
	}

    mPrcInstr.push_back(pair<string,string>(target,val));

    return this;
}

XMLNode* XMLNode::clear()
{
    attrClear();
    setText("");
    prcInstrClear();
    childClear();

    return this;
}

string XMLNode::save( unsigned char flg )
{
    string xml;
    if( (flg&XMLHeader) && !(flg&InclNode) )
    {
	xml += "<?xml version='1.0' encoding='UTF-8' ?>";
	if( flg&XMLNode::BrClosePast ) xml+="\n";
	flg|=InclNode;
    }

    xml += ((flg&XMLNode::BrOpenPrev)?"\n<":"<") + encode(name());

    for( unsigned i_a = 0; i_a < mAttr.size(); i_a++ )
	xml = xml + " " + encode(mAttr[i_a].first) + "=\"" + Mess->codeConvOut("UTF-8",encode(mAttr[i_a].second)) + "\"";

    if( !childSize() && text().empty() && mPrcInstr.empty() )
	xml+= (flg&(XMLNode::BrOpenPast|XMLNode::BrClosePast))?"/>\n":"/>";
    else
    {
	xml = xml + ((flg&XMLNode::BrOpenPast)?">\n":">");
	//- Save text -
	if( !text().empty() )
	    xml = xml + Mess->codeConvOut("UTF-8",encode(text(),true)) + (flg&XMLNode::BrTextPast?"\n":"");
	//- Save process instructions -
	for( int i_p = 0; i_p < mPrcInstr.size(); i_p++ )
	    xml = xml + "<?"+mPrcInstr[i_p].first+" "+Mess->codeConvOut("UTF-8",mPrcInstr[i_p].second)+(flg&XMLNode::BrPrcInstrPast?"?>\n":"?>");
	//- Save included childs -
	for( int i_c = 0; i_c < childSize(); i_c++ )
	{
	    XMLNode *child = childGet(i_c);
	    if( child )  xml += child->save(flg);
	}
	//- Close tag -
	xml = xml + "</" + encode(name()) + (flg&XMLNode::BrClosePast?">\n":">");
    }

    return xml;
}

string XMLNode::encode( const string &s, bool text ) const
{
    string sout = s;

    for( int i_sz = 0; i_sz < sout.size(); i_sz++ )
	switch(sout[i_sz])
	{
	    case '>': sout.replace(i_sz,1,"&gt;"); i_sz+=3; break;
	    case '<': sout.replace(i_sz,1,"&lt;"); i_sz+=3; break;
	    case '"': sout.replace(i_sz,1,"&quot;"); i_sz+=5; break;
	    case '&': sout.replace(i_sz,1,"&amp;"); i_sz+=4; break;
	    case '\'': sout.replace(i_sz,1,"&#039;"); i_sz+=5; break;
	    case '\n': if( !text ) { sout.replace(i_sz,1,"&#010;"); i_sz+=5; } break;
	}

    return sout;
}

void XMLNode::load( const string &s )
{
    clear();

    XML_Parser p = XML_ParserCreate("UTF-8");
    if( !p ) throw TError("XMLNode","Couldn't allocate memory for parser.");

    XML_SetElementHandler( p, start_element, end_element );
    XML_SetCharacterDataHandler( p, characters );
    XML_SetProcessingInstructionHandler( p, instrHandler );
    XML_SetUserData( p, this );

    XMLNode *lstParent = mParent;
    mParent = NULL;

    if( !XML_Parse( p, s.c_str(), s.size(), true ) )
    {
	XML_ParserFree( p );
	mParent = lstParent;
	throw TError("XMLNode","Parse error at line %d --- %s. Source string: '%s'",
	    XML_GetCurrentLineNumber(p), XML_ErrorString(XML_GetErrorCode(p)), ((s.size()>1024)?s.substr(0,1024)+"...":s).c_str());
    }
    XML_ParserFree( p );
    mParent = lstParent;
}

//- Parse/load XML attributes -
void XMLNode::start_element( void *data, const char *el, const char **attr )
{
    const char *a_n, *a_v;
    XMLNode *p = (XMLNode*)data;
    XMLNode *n = p;

    if( p->mParent )
    {
	n = new XMLNode();
	p->mParent->childAdd( n );
    }

    n->setName(el);
    while( *attr )
    {
	a_n = *attr++; a_v = *attr++;
	n->mAttr.push_back(pair<string,string>(a_n,Mess->codeConvIn("UTF-8",a_v)));
    }

    p->node_stack.push_back(n);
    p->mParent = n;
}

void XMLNode::end_element( void *data, const char *el )
{
    XMLNode *p = (XMLNode*)data;

    //> Remove spaces from end of text
    int i_ch;
    for( i_ch = p->mParent->mText.size()-1; i_ch >= 0; i_ch--)
	if( !(p->mParent->mText[i_ch] == ' ' || p->mParent->mText[i_ch] == '\n' || p->mParent->mText[i_ch] == '\t') )
	    break;
    //> Encode text
    p->mParent->setText(Mess->codeConvIn("UTF-8",p->mParent->mText.substr(0,i_ch+1)));

    if( !p->node_stack.empty() ) p->node_stack.pop_back();
    if( p->node_stack.empty() ) p->mParent = NULL;
    else p->mParent = p->node_stack[p->node_stack.size()-1];
}

void XMLNode::characters( void *userData, const XML_Char *s, int len )
{
    XMLNode *p = ((XMLNode*)userData)->mParent;
    //if( !len )	return;
    if( p->mText.size() ) p->mText.append(s,len);
    else
	for(int i_ch = 0; i_ch < len; i_ch++)
	{
	    if(s[i_ch] == ' ' || s[i_ch] == '\n' || s[i_ch] == '\t' )	continue;
	    p->mText.assign(s+i_ch,len-i_ch);
	    break;
	}
}

void XMLNode::instrHandler( void *userData, const XML_Char *target, const XML_Char *data )
{
    ((XMLNode*)userData)->mParent->mPrcInstr.push_back(pair<string,string>(target,Mess->codeConvIn("UTF-8",data)));
}
