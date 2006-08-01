
//OpenSCADA system file: xml.cpp
/***************************************************************************
 *   Copyright (C) 2003-2006 by Roman Savochenko                           *
 *   rom_as@fromru.com                                                     *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
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
#include "xml.h"

const char *XMLNode::o_name = "XMLNode";

XMLNode &XMLNode::operator=(XMLNode &prm)    
{
    //Delete self children and atributes
    n_attr.clear();
    v_attr.clear();
    for( int i_ch = 0; i_ch < m_children.size(); i_ch++ )
	delete m_children[i_ch];
    m_children.clear();    	
    
    //Copy params (name,text and atributes)
    name( prm.name() );
    text( prm.text() );
    vector<string> ls;
    prm.attrList(ls);
    for( int i_atr = 0; i_atr < ls.size(); i_atr++)
	attr(ls[i_atr],prm.attr(ls[i_atr]));

    //Recursive copy children
    for( int i_ch = 0; i_ch < prm.childSize(); i_ch++ )
	*childAdd() = *prm.childGet(i_ch);
    
    return *this;
}

void XMLNode::childAdd( XMLNode * n )
{
    if( n )  m_children.push_back( n );
}

XMLNode* XMLNode::childAdd( const string &name )
{
    XMLNode *n = new XMLNode( name );
    childAdd( n );
    
    return n;
}

void XMLNode::childDel( const unsigned id )
{
    if( id >= childSize() ) throw TError(o_name,"Child %d no present.",id);
    delete m_children[id];
    m_children.erase( m_children.begin()+id );
}
	
void XMLNode::childClean( const string &name )
{
    for( int i_ch = 0; i_ch < m_children.size(); i_ch++ )
    	if( !name.size() || m_children[i_ch]->name() == name )
	    childDel(i_ch--);
}

int XMLNode::childIns( unsigned id, XMLNode * n )
{
    if( n ) 
    {
	if( id > childSize() ) id = childSize();    
    	m_children.insert( m_children.begin()+id, n );
	return(id);
    }
}

XMLNode* XMLNode::childIns( unsigned id, const string &name )
{
    XMLNode *n = new XMLNode( name );
    childIns( id, n );
    
    return n;
}

XMLNode* XMLNode::childGet( const int index, bool noex ) const
{
    if( index < childSize() )	return m_children[index]; 
    if( noex )	return NULL;
    throw TError(o_name,"Child %d no present.",index);
}

XMLNode* XMLNode::childGet( const string &name, const int numb ) const
{
    for( int i_ch = 0, i_n = 0; i_ch < childSize(); i_ch++)
	if( childGet(i_ch)->name() == name )
	    if( i_n++ == numb ) return( childGet(i_ch) );
	    
    throw TError(o_name,"Child %s:%d no found!",name.c_str(),numb);
}

XMLNode* XMLNode::childGet( const string &attr, const string &val, bool noex ) const
{
    for( unsigned i_f = 0; i_f < childSize(); i_f++)
	if( childGet(i_f)->attr(attr) == val ) return( childGet(i_f) );
	
    if( noex ) return NULL;
    throw TError(o_name,"Child with attribut %s=%s no present.",attr.c_str(),val.c_str());
}

void XMLNode::attrList( vector<string> & list ) const
{
    list.clear();
    for(unsigned i_opt = 0; i_opt < n_attr.size(); i_opt++)
	list.push_back(n_attr[i_opt]);    
}

string XMLNode::attr( const string &name ) const
{
    for(unsigned i_opt = 0; i_opt < n_attr.size(); i_opt++)
	if(n_attr[i_opt] == name) return(v_attr[i_opt]);

    return("");
}

XMLNode* XMLNode::attr_( const char *name, const char *val )
{
    return attr( name, val );
}

XMLNode* XMLNode::attr( const string &name, const string &val )
{
    for(unsigned i_opt = 0; i_opt < n_attr.size(); i_opt++)
	if(n_attr[i_opt] == name)
	{
	    v_attr[i_opt] = val;  
	    return this;
	}

    n_attr.push_back(name);
    v_attr.push_back(val);
    
    return this;
}

void XMLNode::clear()
{
    n_attr.clear();	
    v_attr.clear();	
    for( unsigned i_ch = 0; i_ch < m_children.size(); i_ch++ )
    	if( m_children[i_ch] ) delete( m_children[i_ch] );    

    m_children.clear();
    set_root( NULL );
    m_current_node = 0;
}

string XMLNode::save( unsigned char flg )
{
    string xml = ((flg&XML_BR_OPEN_PREV)?"\n<":"<") + encode( name() );

    for(unsigned i_atr = 0; i_atr < n_attr.size(); i_atr++)
	xml = xml + " " + n_attr[i_atr] + "=\"" + v_attr[i_atr] + "\"";
    
    xml = xml + ((flg&XML_BR_OPEN_PAST)?">\n":">") + encode( text() ) + ((flg&XML_BR_TEXT_PAST)?"\n":"");

    for( int child_index = 0; child_index < childSize(); child_index++ )
    {
	XMLNode *child = childGet( child_index );
	if( child )  xml += child->save(flg);
    }

    xml+= string("</") + encode( name() ) + ((flg&XML_BR_CLOSE_PAST)?">\n":">");

    return xml;
}

string XMLNode::encode( const string &s ) const
{
    string tmp;

    for( string::const_iterator it = s.begin(); it != s.end(); it++ )
    {
    	char c = *(it);
	if( c == '&' )        tmp += "&amp;";
	else if( c == '<' )   tmp += "&lt;";
        else if( c == '>' )   tmp += "&gt;";
	else if( c == '"' )   tmp += "&quot;";
	else if ( c == '\'' ) tmp += "&apos;";
	else                  tmp += c;
    }
    
    return tmp;
}

void XMLNode::load( const string &s )
{
    clear();

    XML_Parser p = XML_ParserCreate ( "UTF-8" );
    if( ! p ) throw TError(o_name,"Couldn't allocate memory for parser.");

    XML_SetElementHandler( p, start_element, end_element );
    XML_SetCharacterDataHandler( p, characters );
    XML_SetUserData ( p, this );    

    if( !XML_Parse( p, s.c_str(), s.size(), true ) )
        throw TError(o_name,"Parse error at line %d --- %s", XML_GetCurrentLineNumber(p), XML_ErrorString(XML_GetErrorCode(p)) );
    XML_ParserFree( p );    
    if( m_root )
    {
	m_name = m_root->m_name;
	m_text = m_root->m_text;
	for( unsigned i_atr = 0; i_atr < m_root->n_attr.size(); i_atr++)
	{
	    n_attr.push_back(m_root->n_attr[i_atr]);
	    v_attr.push_back(m_root->v_attr[i_atr]);
	}	    
	m_children = m_root->m_children;
	m_root = NULL;
    }
}
							  
void XMLNode::start_element ( void *data, const char *el, const char **attr )
{
    if( !data ) return;
    XMLNode * p = ( XMLNode* )data;
    XMLNode * n = new XMLNode();

    if( !p->root() )        p->set_root( n );
    if( p->current_node() ) p->current_node()->childAdd( n );

    while(*attr)
    {
	n->n_attr.push_back(*attr++);
	n->v_attr.push_back(*attr++);
    }

    p->node_stack().push_back ( n );
    p->set_current_node( n );
    n->name( el );
}

void XMLNode::end_element ( void *data, const char *el )
{
    if ( !data ) return;
    XMLNode *p = ( XMLNode* )data;

    if( !p->node_stack().size() ) return;
    p->node_stack().pop_back();

    if( !p->node_stack().size() ) 
	p->set_current_node( NULL );
    else 
	p->set_current_node( *(p->node_stack().begin()+p->node_stack().size()-1) );
}

void XMLNode::characters ( void *userData, const XML_Char *s, int len )
{
    if( !userData ) return;
    XMLNode *p = ( XMLNode* ) userData;

    string tmp; 
    for(int i_ch = 0; i_ch < len; i_ch++)
        if(s[i_ch] != ' ' && s[i_ch] != '\n' && s[i_ch] != '\t' )
	{
	    tmp.assign(s,i_ch,len-i_ch); 
	    break;
	}
    
    if( p->current_node() && tmp.size() )
        p->current_node()->text( p->current_node()->text()+tmp );
}

      
