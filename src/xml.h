/***************************************************************************
 *   Copyright (C) 2004 by Roman Savochenko                                *
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

#ifndef XML_H
#define XML_H

#include <string>
#include <vector>

#include <expat.h>

using std::string;
using std::vector;

class XMLNode 
{
    public:
    	XMLNode( const string &name = "" ) : 
    	    m_root(NULL), m_current_node(NULL), m_cleanup ( true ), m_name ( name ), m_text("") {  }

	~XMLNode() 
	{ cleanup(); }

	void new_xml(){ cleanup(); };
	string get_xml( bool humen = false ) const;
	void load_xml ( const string & );

	int get_child_count() const { return m_children.size(); }
	void add_child ( XMLNode * );
	XMLNode* add_child( const string &name = "" );
	int ins_child ( unsigned id, XMLNode * );
	void del_child ( const unsigned id );
	void clean_childs( const string &name = "" );
	XMLNode* ins_child( unsigned id, const string &name = "" );
	XMLNode* get_child( const int ) const;
	XMLNode* get_child( const string &name, const int numb = 0 ) const;	
	XMLNode* get_child( const string &attr, const string &name ) const;	

	string get_text() const { return m_text; }
	void set_text ( const string &s ) { m_text = s; }
	void append_text ( const string &s ) { m_text += s; }
    
	void get_attr_list( vector<string> & list ) const;
	string get_attr( const string &name ) const;
	void set_attr( const string &name, const string &val, const bool add = true );

	string get_name() const { return m_name; }
	void set_name( const string &s ) { m_name = s; }

    private:
	void cleanup();
	string encode ( const string &s ) const;
	
	static void start_element ( void *data, const char *el, const char **attr );
	static void end_element ( void *data, const char *el );
	static void characters ( void *userData, const XML_Char *s, int len );
	
    private:	
	string m_name;
    	string m_text;
	vector<XMLNode *> m_children;
	vector<string>   n_attr;
	vector<string>   v_attr;	

	static const char *o_name;
    private:    
	vector<XMLNode*>& node_stack() { return m_node_stack; }
        XMLNode* current_node()        { return m_current_node; }
	void set_current_node ( XMLNode* p ) { m_current_node = p; }
	void set_root( XMLNode* p )    { if( m_root ) delete (m_root); m_root = p; }
        XMLNode* root()                { return m_root; }

    private:
        XMLNode *m_root;
    	vector<XMLNode*> m_node_stack;
        XMLNode* m_current_node;
	bool m_cleanup;
};

#endif  //XML_H

