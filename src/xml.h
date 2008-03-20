
//OpenSCADA system file: xml.h
/***************************************************************************
 *   Copyright (C) 2003-2007 by Roman Savochenko                           *
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

#ifndef XML_H
#define XML_H

#include <string>
#include <vector>

#include <expat.h>

using std::string;
using std::vector;

//*************************************************
//* XMLNode                                       *
//*************************************************
class XMLNode 
{
    public:
	//Data
	enum SaveView	
	{ 
	    BrOpenPrev  = 0x01,		//Break preview open tag
	    BrOpenPast  = 0x02,		//Break past open tag
	    BrClosePast = 0x04,		//Break past close tag
	    BrTextPast  = 0x08		//Break past text
	};
	
	//Methods
    	XMLNode( const string &name = "" ) : m_name(name), m_text(""), current_node(NULL) 
						{  }
	~XMLNode( ) 				{ clear(); }

	XMLNode &operator=( XMLNode &prm );

	string 	name( ) const 			{ return m_name; }
        XMLNode* setName( const string &s ) 	{ m_name = s; return this; }

	string	text( ) const 			{ return m_text; }
        XMLNode* setText( const string &s ) 	{ m_text = s; return this; }
	
	void	attrList( vector<string> & list ) const;
	void	attrClear( );
        string 	attr( const string &name ) const;
        XMLNode* setAttr( const string &name, const string &val );
	XMLNode* setAttr_( const char *name, const char *val );

	void 	load( const string &vl );
	string 	save( unsigned char flgs = 0 );
	XMLNode* clear( );

	int 	childSize( ) const 		{ return m_children.size(); }
	void 	childAdd( XMLNode *nd );
	XMLNode* childAdd( const string &name = "" );
	int 	childIns( unsigned id, XMLNode *nd );
	XMLNode* childIns( unsigned id, const string &name = "" );
	void 	childDel( const unsigned id );
	void 	childClean( const string &name = "" );
	XMLNode* childGet( const int, bool noex = false ) const;
	XMLNode* childGet( const string &name, const int numb = 0 ) const;	
	XMLNode* childGet( const string &attr, const string &name, bool noex = false ) const;	

    private:
	//Methods
	string encode ( const string &s ) const;
	
	static void start_element ( void *data, const char *el, const char **attr );
	static void end_element ( void *data, const char *el );
	static void characters ( void *userData, const XML_Char *s, int len );

	//Attributes	
	string m_name;
    	string m_text;
	vector<XMLNode *> m_children;
	vector<string>   n_attr;
	vector<string>   v_attr;	

	static const char *o_name;

	//- Parse/load XML attributes -
    	vector<XMLNode*> node_stack;
        XMLNode* current_node;
};

#endif  //XML_H

