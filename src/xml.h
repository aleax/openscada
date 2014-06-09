
//OpenSCADA system file: xml.h
/***************************************************************************
 *   Copyright (C) 2003-2014 by Roman Savochenko, <rom_as@oscada.org>      *
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

#include <stdlib.h>
#include <string>
#include <vector>
#include <map>

#include "config.h"

using std::string;
using std::vector;
using std::map;
using std::pair;

namespace OSCADA
{

//*************************************************
//* XMLNode                                       *
//*************************************************
class XMLNode
{
    public:
	//Data
	enum LoadFlgs
	{
	    LD_Full		= 0x01,		//Load XML full
	    LD_NoTxtSpcRemEnBeg	= 0x02,		//No remove spaces for begin and end tag's text, for not full mode
	};
	enum SaveView
	{
	    BrOpenPrev		= 0x01,		//Break before open tag
	    BrOpenPast		= 0x02,		//Break after open tag
	    BrClosePast		= 0x04,		//Break after close tag
	    BrTextPast		= 0x08,		//Break after text
	    BrSpecBlkPast	= 0x10,		//Break after special block
	    BrAllPast		= 0x1E,		//Break after all
	    XMLHeader		= 0x20,		//Include standard XML header
	    XHTMLHeader		= 0x40,		//Incluse standard XHTML header
	    Clean		= 0x80,		//Clean service tags: <??>, <!-- -->
	    MissTagEnc		= 0x100,	//Miss tag name encode
	    MissAttrEnc		= 0x200		//Miss attribute name encode
	};

	//Methods
	XMLNode(const string &name = "") : mName(name), mText(""), mParent(NULL)	{  }
	XMLNode(const XMLNode &nd);
	~XMLNode( )				{ clear(); }

	XMLNode &operator=( const XMLNode &prm );

	string	name( ) const			{ return mName; }
	XMLNode* setName( const string &s )	{ mName = s; return this; }

	string	text( bool childs = false, bool recursive = false ) const;
	XMLNode* setText( const string &s, bool childs = false );

	void	attrList( vector<string> &list ) const;
	XMLNode* attrDel( const string &name );
	void	attrClear( );
	string	attr( const string &name, bool caseSens = true ) const;
	XMLNode* setAttr( const string &name, const string &val );

	void	load( const string &vl, unsigned flg = 0, const string &cp = "UTF-8" );
	string	save( unsigned flg = 0, const string &cp = "UTF-8" );
	XMLNode* clear( );

	bool	childEmpty( ) const		{ return mChildren.empty(); }
	unsigned childSize( ) const		{ return mChildren.size(); }
	void	childAdd( XMLNode *nd );
	XMLNode* childAdd( const string &name = "" );
	int	childIns( int id, XMLNode *nd );
	XMLNode* childIns( int id, const string &name = "" );
	void	childDel( int id );
	void	childDel( XMLNode *nd );
	void	childClear( const string &name = "" );
	XMLNode* childGet( const int, bool noex = false ) const;
	XMLNode* childGet( const string &name, const int numb = 0, bool noex = false ) const;
	XMLNode* childGet( const string &attr, const string &name, bool noex = false ) const;
	XMLNode* getElementBy( const string &attr, const string &val );

	XMLNode* parent( )			{ return mParent; }
	XMLNode* root( );

    private:
	//Methods
	void saveNode( unsigned flg, string &xml, const string &cp );
	void encode( const string &s, string &rez, bool text = false ) const;

	//Attributes
	string mName;
	string mText;
	vector< XMLNode* >		mChildren;
	vector< pair<string,string> >	mAttr;
	XMLNode *mParent;

	//Data
	class LoadCtx
	{
	    public:
		//Methods
		LoadCtx( const string &ivl, unsigned flg );

		//Attributes
		unsigned flg;
		string vl, enc, aNm, aVl;
		map<string,string> ent;
	};

	//Methods
	unsigned loadNode( LoadCtx &ctx, unsigned pos = 0 );
	inline bool parseAttr( LoadCtx &ctx, unsigned &pos, char sep = '=' );
	void parseEntity( LoadCtx &ctx, unsigned &rpos, string &rez );
};

}

#endif  //XML_H
