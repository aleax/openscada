
//OpenSCADA system file: xml.cpp
/***************************************************************************
 *   Copyright (C) 2003-2010 by Roman Savochenko                           *
 *   rom_as@oscada.org, rom_as@fromru.com                                  *
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

using namespace OSCADA;

//*************************************************
//* XMLNode                                       *
//*************************************************
XMLNode::XMLNode(const XMLNode &nd) : mName("nd"), mText(""), mParent(NULL)
{
    this->operator=(nd);
}

XMLNode &XMLNode::operator=(const XMLNode &prm)
{
    //> Delete self children and attributes
    mAttr.clear();
    mPrcInstr.clear();
    for(unsigned i_ch = 0; i_ch < mChildren.size(); i_ch++)
	delete mChildren[i_ch];
    mChildren.clear();

    //> Copy params (name,text, attributes and instructions)
    setName( prm.name() );
    setText( prm.text() );
    vector<string> ls;
    prm.attrList(ls);
    for(unsigned i_a = 0; i_a < ls.size(); i_a++)
	setAttr(ls[i_a],prm.attr(ls[i_a]));
    prm.prcInstrList(ls);
    for(unsigned i_p = 0; i_p < ls.size(); i_p++)
	setPrcInstr(ls[i_p],prm.prcInstr(ls[i_p]));

    //> Recursive copy children
    for(unsigned i_ch = 0; i_ch < prm.childSize(); i_ch++)
	*childAdd() = *prm.childGet(i_ch);

    return *this;
}

void XMLNode::childAdd( XMLNode *n )
{
    if(!n) return;
    mChildren.push_back(n);
    n->mParent = this;
}

XMLNode* XMLNode::childAdd( const string &name )
{
    XMLNode *n = new XMLNode(name);
    childAdd(n);

    return n;
}

void XMLNode::childDel( const unsigned id )
{
    if(id >= childSize()) throw TError("XMLNode",_("Child %d is not present."),id);
    delete mChildren[id];
    mChildren.erase(mChildren.begin()+id);
}

void XMLNode::childDel( XMLNode *nd )
{
    for(unsigned i_ch = 0; i_ch < mChildren.size(); i_ch++)
	if(mChildren[i_ch] == nd)
	{
	    delete mChildren[i_ch];
	    mChildren.erase( mChildren.begin()+i_ch );
	    break;
	}
}

void XMLNode::childClear( const string &name )
{
    for(unsigned i_ch = 0; i_ch < mChildren.size(); )
	if(name.empty() || mChildren[i_ch]->name() == name) childDel(i_ch);
	else i_ch++;
}

int XMLNode::childIns( unsigned id, XMLNode * n )
{
    if(!n) return -1;

    if(id > childSize()) id = childSize();
    mChildren.insert(mChildren.begin()+id, n);
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
    if(index >= 0 && index < (int)childSize()) return mChildren[index];
    if(noex) return NULL;
    throw TError("XMLNode",_("Child %d is not present."),index);
}

XMLNode* XMLNode::childGet( const string &name, const int numb, bool noex ) const
{
    for(int i_ch = 0, i_n = 0; i_ch < (int)childSize(); i_ch++)
	if(childGet(i_ch)->name() == name && i_n++ == numb)
	    return childGet(i_ch);

    if(noex) return NULL;
    throw TError("XMLNode",_("Child %s:%d is not found!"),name.c_str(),numb);
}

XMLNode* XMLNode::childGet( const string &attr, const string &val, bool noex ) const
{
    for(unsigned i_f = 0; i_f < childSize(); i_f++)
	if(childGet(i_f)->attr(attr) == val) return childGet(i_f);

    if(noex) return NULL;
    throw TError("XMLNode",_("Child with attribut %s=%s is not present."),attr.c_str(),val.c_str());
}

void XMLNode::attrList( vector<string> & list ) const
{
    list.clear();
    for(unsigned i_a = 0; i_a < mAttr.size(); i_a++)
	list.push_back(mAttr[i_a].first);
}

XMLNode* XMLNode::attrDel( const string &name )
{
    for(unsigned i_a = 0; i_a < mAttr.size(); i_a++)
	if(mAttr[i_a].first == name)
	{ mAttr.erase(mAttr.begin()+i_a); break; }

    return this;
}

void XMLNode::attrClear( )
{
    mAttr.clear();
}

string XMLNode::attr( const string &name ) const
{
    for(unsigned i_a = 0; i_a < mAttr.size(); i_a++)
	if(mAttr[i_a].first == name) return mAttr[i_a].second;

    return "";
}

XMLNode* XMLNode::setAttr( const string &name, const string &val )
{
    for(unsigned i_a = 0; i_a < mAttr.size(); i_a++)
	if(mAttr[i_a].first == name)
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
    for(unsigned i_p = 0; i_p < mPrcInstr.size(); i_p++)
	list.push_back(mPrcInstr[i_p].first);
}

void XMLNode::prcInstrDel( const string &target )
{
    for(unsigned i_p = 0; i_p < mPrcInstr.size(); i_p++)
	if(mPrcInstr[i_p].first == target)
	{ mPrcInstr.erase(mPrcInstr.begin()+i_p); return; }
}

void XMLNode::prcInstrClear( )
{
    mPrcInstr.clear();
}

string XMLNode::prcInstr( const string &target ) const
{
    for(unsigned i_p = 0; i_p < mPrcInstr.size(); i_p++)
	if(mPrcInstr[i_p].first == target) return mPrcInstr[i_p].second;

    return "";
}

XMLNode* XMLNode::setPrcInstr( const string &target, const string &val )
{
    for(unsigned i_p = 0; i_p < mPrcInstr.size(); i_p++)
	if(mPrcInstr[i_p].first == target)
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

string XMLNode::save( unsigned flg )
{
    string xml;
    xml.reserve(10000);

    if(flg&XMLHeader)
    {
	xml += "<?xml version='1.0' encoding='UTF-8' ?>";
	if(flg&XMLNode::BrClosePast) xml += "\n";
    }

    saveNode(flg,xml);
    return xml;
}

/*unsigned XMLNode::loadNode( const string &vl, LoadCtx &ctx, unsigned pos )
{
    bool initTag = true;
    unsigned cpos, bpos, entpos = 0, tpos;

nextTag:
    //> Find for a tag start symbol
    for( ; pos < vl.size() && vl[pos] != '<'; pos++)
	if(!initTag)
	{
	    if(!mText.empty() || !isspace(vl[pos]))
	    {
		if(mText.empty()) mText.reserve(1000);
		mText += vl[pos];
	    }
	    if(vl[pos] == '&') entpos = pos;
	    else if(vl[pos] == ';' && (pos-entpos) <= 7) parseEntity(mText,pos-entpos+1);
	}
    if((pos+1) >= vl.size()) throw TError("XMLNode",_("Unexpected end. Pos: %d"),pos);
    cpos = pos;

    switch(vl[pos+1])
    {
	case '!':	// Comment part
	    if((pos+4) >= vl.size() || vl[pos+2] != '-' || vl[pos+3] != '-')
		throw TError("XMLNode",_("Unfinished start comment block. Pos: %d"),pos);
	    while(cpos < vl.size() && vl.compare(cpos,3,"-->") != 0) cpos++;
	    if(cpos >= vl.size()) throw TError("XMLNode",_("No comment block end. Pos: %d"),pos);
	    pos = cpos+3;
	    goto nextTag;
	case '?':	// Program block
	    bpos = cpos+2;
	    tpos = bpos;
	    while(tpos < vl.size() && vl.compare(tpos,2,"?>") != 0) tpos++;
	    if(tpos >= vl.size()) throw TError("XMLNode",_("Unexpected end. Pos: %d"),pos);
	    //> Get program block's name
	    //>>> Get tag name
	    cpos = bpos;
	    while(cpos < tpos && !isspace(vl[cpos])) cpos++;
	    if(cpos < tpos)
	    {
		ctx.nm.assign(vl,bpos,cpos-bpos);
		//>> Pass spaces
		while(isspace(vl[cpos])) cpos++;
		//> Place program block
		if(!initTag) mPrcInstr.push_back(pair<string,string>(ctx.nm,vl.substr(cpos,tpos-cpos)));
		//> Process specific block <?xml ?>
		if(ctx.nm == "xml")
		    while(parseAttr(vl,cpos,ctx.nm,ctx.value))
			if(ctx.nm == "encoding") ctx.enc.assign(ctx.value);
	    }
	    pos = tpos+2;
	    goto nextTag;
	case '/':	//> End tag
	    if(vl.compare(cpos+2,mName.size(),mName) == 0)
	    {
		cpos += 2+mName.size();
		while(cpos < vl.size() && isspace(vl[cpos])) cpos++;
		if(cpos < vl.size() && vl[cpos] == '>')
		{
		    //> Remove spaces from end of text
		    int i_ch = mText.size()-1;
		    while(i_ch >= 0 && isspace(mText[i_ch])) i_ch--;
		    if(i_ch < ((int)mText.size()-1)) mText.erase(i_ch+1);
		    mText = Mess->codeConvIn(ctx.enc,mText);
		    return cpos+1;
		}
	    }
	    throw TError("XMLNode",_("Unexpected or error end tag. Pos: %d"),cpos);
    }
    //> Process for standard XML node
    //>> It is me node
    if(initTag)
    {
	bpos = cpos+1;
	//>>> Get tag name
	for(cpos = bpos; cpos < vl.size() && !isspace(vl[cpos]) && vl[cpos] != '>' && vl[cpos] != '/'; ) cpos++;
	if(cpos >= vl.size()) throw TError("XMLNode",_("Unexpected end. Pos: %d"),pos);
	mName.assign(vl,bpos,cpos-bpos);
	initTag = false;
	//>>> Process tag attributes
	while(parseAttr(vl,cpos,ctx.nm,ctx.value))
	    mAttr.push_back(pair<string,string>(ctx.nm,Mess->codeConvIn(ctx.enc,ctx.value)));
	//>>> Pass spaces
	while(cpos < vl.size() && isspace(vl[cpos])) cpos++;
	if(cpos >= vl.size()) throw TError("XMLNode",_("Unexpected end. Pos: %d"),pos);
	//>>> Process close tag or the tag content
	if(vl[cpos]=='>') { pos = cpos+1; goto nextTag; }
	else if(vl.compare(cpos,2,"/>") == 0) return cpos+2;
	throw TError("XMLNode",_("Start tag error. Pos: %d"),cpos);
    }
    //>> New XML node create
    else
    {
	pos = childAdd()->loadNode(vl, ctx, pos-1);
	goto nextTag;
    }

    return cpos;
}

bool XMLNode::parseAttr(const string &vl, unsigned &pos, string &nm, string &value)
{
    //> Get attribute name
    //>> Pass spaces
    while(pos < vl.size() && isspace(vl[pos])) pos++;
    if(pos >= vl.size()) throw TError("XMLNode",_("Unexpected end. Pos: %d"),pos);
    if(!isalpha(vl[pos])) return false;

    unsigned bpos = pos, entpos = 0;
    while(pos < vl.size() && !isspace(vl[pos]) && vl[pos] != '=') pos++;
    if(pos >= vl.size()) throw TError("XMLNode",_("Unexpected end. Pos: %d"),pos);
    nm.assign(vl,bpos,pos-bpos);
    //> Get symbol '='
    //>> Pass spaces
    while(pos < vl.size() && isspace(vl[pos])) pos++;
    if(pos >= vl.size()) throw TError("XMLNode",_("Unexpected end. Pos: %d"),pos);
    if(vl[pos] != '=') throw TError("XMLNode",_("Unfinished attribute. Pos: %d"),bpos);
    pos++;
    //> Get symbol "'" or '"'
    //>> Pass spaces
    while(pos < vl.size() && isspace(vl[pos])) pos++;
    if(pos >= vl.size()) throw TError("XMLNode",_("Unexpected end. Pos: %d"),pos);
    if(vl[pos] != '\'' && vl[pos] != '"') throw TError("XMLNode",_("Unfinished attribute. Pos: %d"),bpos);
    char brc = vl[pos];
    //> Get value
    //printf("TEST 00: '%d'\n",value.capacity());
    //value.reserve(100);
    value.clear();
    for(pos++; pos < vl.size() && vl[pos] != brc; pos++)
    {
	value += vl[pos];
        if(vl[pos] == '&') entpos = pos;
        else if(vl[pos] == ';' && (pos-entpos) <= 7) parseEntity(value,pos-entpos+1);
    }
    if(pos >= vl.size()) throw TError("XMLNode",_("Unexpected end. Pos: %d"),pos);
    pos++;

    //printf("TEST 01: '%d'\n",value.capacity());

    return true;
}

void XMLNode::parseEntity(string &vl, unsigned rpos)
{
    rpos = vl.size()-rpos;
    if(vl[rpos] != '&')	return;
    if(vl.compare(rpos,5,"&amp;") == 0) vl.replace(rpos,string::npos,"&");
    else if(vl.compare(rpos,4,"&lt;") == 0) vl.replace(rpos,string::npos,"<");
    else if(vl.compare(rpos,4,"&gt;") == 0) vl.replace(rpos,string::npos,">");
    else if(vl.compare(rpos,6,"&apos;") == 0) vl.replace(rpos,string::npos,"'");
    else if(vl.compare(rpos,6,"&quot;") == 0) vl.replace(rpos,string::npos,"\"");
    else if(vl[rpos+1] == '#')
    {
	if(vl[rpos+2] == 'X' || vl[rpos+2] == 'x') vl.replace(rpos,string::npos,1,(char)strtol(vl.data()+rpos+2,NULL,16));
	else vl.replace(rpos,string::npos,1,(char)atoi(vl.data()+rpos+2));
    }
}*/

void XMLNode::saveNode( unsigned flg, string &xml )
{
    xml.append((flg&XMLNode::BrOpenPrev) ? "\n<" : "<");
    if(flg&XMLNode::MissTagEnc) xml.append(name());
    else encode(name(), xml);

    for(unsigned i_a = 0; i_a < mAttr.size(); i_a++)
    {
	xml.append(" ");
	if(flg&XMLNode::MissAttrEnc) xml.append(mAttr[i_a].first);
	else encode(mAttr[i_a].first, xml);
	xml.append("=\"");
	encode(Mess->codeConvOut("UTF-8",mAttr[i_a].second), xml);
	xml.append("\"");
    }

    if(childEmpty() && text().empty() && mPrcInstr.empty()) xml.append((flg&(XMLNode::BrOpenPast|XMLNode::BrClosePast)) ? "/>\n" : "/>");
    else
    {
	xml.append((flg&XMLNode::BrOpenPast) ? ">\n" : ">");
	//> Save text
	if(!text().empty())
	{
	    encode(Mess->codeConvOut("UTF-8",text()), xml, true);
	    xml.append(flg&XMLNode::BrTextPast ? "\n" : "");
	}
	//> Save process instructions
	for(unsigned i_p = 0; i_p < mPrcInstr.size(); i_p++)
	    xml.append("<?"+mPrcInstr[i_p].first+" "+Mess->codeConvOut("UTF-8",mPrcInstr[i_p].second)+(flg&XMLNode::BrPrcInstrPast?"?>\n":"?>"));
	//> Save included childs
	for(unsigned i_c = 0; i_c < childSize(); i_c++) childGet(i_c)->saveNode(flg,xml);
	//> Close tag
	xml.append("</");
	if(flg&XMLNode::MissTagEnc) xml.append(name() );
	else encode(name(), xml);
	xml.append(flg&XMLNode::BrClosePast ? ">\n" : ">");
    }
}

void XMLNode::encode( const string &s, string &rez, bool text ) const
{
    const char *replStr = NULL;
    for(unsigned i_sz = 0, f_pos = 0; true; )
    {
	switch(s[i_sz])
	{
	    case '>': replStr = "&gt;"; break;
	    case '<': replStr = "&lt;"; break;
	    case '"': replStr = "&quot;"; break;
	    case '&': replStr = "&amp;"; break;
	    case '\'': replStr = "&#039;"; break;
	    case '\n': if( !text ) replStr = "&#010;"; break;
	}
	i_sz++;
	if(replStr)
	{
	    if((i_sz-1) > f_pos) rez.append(s,f_pos,i_sz-f_pos-1);
	    rez.append(replStr);
	    replStr = NULL;
	    f_pos = i_sz;
	}
	if(i_sz >= s.size())
	{
	    if(i_sz > f_pos) rez.append(s,f_pos,i_sz-f_pos);
	    break;
	}
    }
}

void XMLNode::load( const string &s )
{
    clear();

    //LoadCtx ctx;
    //loadNode(s,ctx);

    XML_Parser p = XML_ParserCreate("UTF-8");
    if(!p) throw TError("XMLNode",_("Couldn't allocate memory for parser."));

    XML_SetElementHandler(p, start_element, end_element);
    XML_SetCharacterDataHandler(p, characters);
    XML_SetProcessingInstructionHandler(p, instrHandler);
    XML_SetUserData(p, this);

    XMLNode *lstParent = mParent;
    mParent = NULL;

    if(!XML_Parse(p, s.data(), s.size(), true))
    {
	int cL = XML_GetCurrentLineNumber(p);
	string xmlErr = XML_ErrorString(XML_GetErrorCode(p));
	XML_ParserFree(p);
	mParent = lstParent;
	throw TError("XMLNode",_("Parse error at line %d --- %s. Source string: '%s'"), cL, xmlErr.c_str(), ((s.size()>1024)?s.substr(0,1024)+"...":s).c_str());
    }
    XML_ParserFree(p);
    mParent = lstParent;
}

//> Parse/load XML attributes
void XMLNode::start_element( void *data, const char *el, const char **attr )
{
    const char *a_n, *a_v;
    XMLNode *p = (XMLNode*)data;
    XMLNode *n = p;

    if(p->mParent) n = p->mParent->childAdd();

    n->setName(el);
    while(*attr)
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
    for(i_ch = p->mParent->mText.size()-1; i_ch >= 0; i_ch--)
	if(!isspace(p->mParent->mText[i_ch]))
	    break;
    //> Encode text
    p->mParent->setText(Mess->codeConvIn("UTF-8",p->mParent->mText.substr(0,i_ch+1)));

    if(!p->node_stack.empty()) p->node_stack.pop_back();
    if(p->node_stack.empty()) p->mParent = NULL;
    else p->mParent = p->node_stack[p->node_stack.size()-1];
}

void XMLNode::characters( void *userData, const XML_Char *s, int len )
{
    XMLNode *p = ((XMLNode*)userData)->mParent;
    //if( !len )	return;
    if(p->mText.size()) p->mText.append(s,len);
    else
	for(int i_ch = 0; i_ch < len; i_ch++)
	{
	    if(isspace(s[i_ch])) continue;
	    p->mText.assign(s+i_ch,len-i_ch);
	    break;
	}
}

void XMLNode::instrHandler( void *userData, const XML_Char *target, const XML_Char *data )
{
    ((XMLNode*)userData)->mParent->mPrcInstr.push_back(pair<string,string>(target,Mess->codeConvIn("UTF-8",data)));
}
