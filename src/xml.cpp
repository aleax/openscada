#include "terror.h"
#include "xml.h"

const char *XMLNode::o_name = "XMLNode";

void XMLNode::add_child( XMLNode * n )
{
    if( n )  m_children.push_back( n );
}

XMLNode* XMLNode::add_child( const string name )
{
    XMLNode *n = new XMLNode( name );
    add_child( n );
    
    return n;
}

XMLNode* XMLNode::get_child( const int index ) const
{
    if( (int)m_children.size() > index )
    	return( *(m_children.begin()+index) );
    else return 0;
}

XMLNode* XMLNode::get_child( const string name, const int numb ) const
{
    for( int i_ch = 0, i_n = 0; i_ch < get_child_count(); i_ch++)
	if( get_child(i_ch)->get_name() == name )
	    if( i_n++ == numb ) return( get_child(i_ch) );
	    
    throw TError("%s: Child %s:%d no found!", o_name, name.c_str(), numb);
}

void XMLNode::get_attr_list( vector<string> & list ) const
{
    list.clear();
    for(unsigned i_opt = 0; i_opt < n_attr.size(); i_opt++)
	list.push_back(n_attr[i_opt]);    
}

string XMLNode::get_attr( const string name ) const
{
    for(unsigned i_opt = 0; i_opt < n_attr.size(); i_opt++)
	if(n_attr[i_opt] == name) return(v_attr[i_opt]);

    return("");
}

void XMLNode::set_attr( const string name, const string val, const bool add )
{
    for(unsigned i_opt = 0; i_opt < n_attr.size(); i_opt++)
	if(n_attr[i_opt] == name)
	{
	    v_attr[i_opt] = val;  
	    return;
	}

    if(add)
    {
	n_attr.push_back(name);
	v_attr.push_back(val);
    }
}

void XMLNode::cleanup()
{
    //if( !m_cleanup ) return;
    n_attr.clear();	
    v_attr.clear();	
    for( unsigned i_ch = 0; i_ch < m_children.size(); i_ch++ )
    	if( m_children[i_ch] ) delete( m_children[i_ch] );    

    m_children.clear();
    set_root( NULL );
    m_current_node = 0;
}

string XMLNode::get_xml( bool humen ) const
{
    string xml = string("<") + encode( get_name() );

    for(unsigned i_atr = 0; i_atr < n_attr.size(); i_atr++)
	xml = xml + " " + n_attr[i_atr] + "=\"" + v_attr[i_atr] + "\"";
    
    xml = xml + ((humen)?">\n":">") + encode( get_text() ) + ((humen)?"\n":"");

    for( int child_index = 0; child_index < get_child_count(); child_index++ )
    {
	XMLNode *child = get_child( child_index );
	if( child )  xml += child->get_xml(humen);
    }

    xml+= string("</") + encode( get_name() ) + ((humen)?">\n":">");

    return xml;
}

string XMLNode::encode( const string s ) const
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

void XMLNode::load_xml( const string &s )
{
    cleanup();

    XML_Parser p = XML_ParserCreate ( NULL );
    if( ! p ) throw TError( "%s: Couldn't allocate memory for parser.",o_name );

    XML_SetElementHandler( p, start_element, end_element );
    XML_SetCharacterDataHandler( p, characters );
    XML_SetUserData ( p, this );    

    if( !XML_Parse( p, s.c_str(), s.size(), true ) )
        throw TError( "%s: Parse error at line %d --- %s", o_name, XML_GetCurrentLineNumber(p), XML_ErrorString(XML_GetErrorCode(p)) );
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
	//m_root->m_cleanup = false;
	//set_root ( NULL );
	m_root = NULL;
    }
}
							  
void XMLNode::start_element ( void *data, const char *el, const char **attr )
{
    if( !data ) return;
    XMLNode * p = ( XMLNode* )data;
    XMLNode * n = new XMLNode();

    if( !p->root() )        p->set_root( n );
    if( p->current_node() ) p->current_node()->add_child ( n );

    while(*attr)
    {
	n->n_attr.push_back(*attr++);
	n->v_attr.push_back(*attr++);
    }

    p->node_stack().push_back ( n );
    p->set_current_node( n );
    n->set_name( el );
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
        p->current_node()->append_text( tmp );
}

      
