
//OpenSCADA system file: xml.cpp
/***************************************************************************
 *   Copyright (C) 2003-2017 by Roman Savochenko, <rom_as@oscada.org>      *
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

#include <strings.h>

#include "terror.h"
#include "tmess.h"
#include "xml.h"

using namespace OSCADA;

//*************************************************
//* XMLNode                                       *
//*************************************************
XMLNode::XMLNode( const XMLNode &nd ) : mName("nd"), mText(""), mParent(NULL)
{
    this->operator=(nd);
}

XMLNode &XMLNode::operator=( const XMLNode &prm )
{
    //Delete self children and attributes
    mAttr.clear();
    for(unsigned iCh = 0; iCh < mChildren.size(); iCh++)
	delete mChildren[iCh];
    mChildren.clear();

    //Copy params (name,text, attributes and instructions)
    mName = prm.mName;
    mText = prm.mText;
    vector<string> ls;
    prm.attrList(ls);
    for(unsigned i_a = 0; i_a < ls.size(); i_a++)
	setAttr(ls[i_a],prm.attr(ls[i_a]));

    //Recursive copy children
    for(unsigned iCh = 0; iCh < prm.childSize(); iCh++)
	*childAdd() = *prm.childGet(iCh);

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

void XMLNode::childDel( int id )
{
    if(id < 0) id = (int)childSize()+id;
    if(id < 0 || id >= (int)childSize()) throw TError("XMLNode", _("Child %d is not present."), id);
    delete mChildren[id];
    mChildren.erase(mChildren.begin()+id);
}

void XMLNode::childDel( XMLNode *nd )
{
    for(unsigned iCh = 0; iCh < mChildren.size(); iCh++)
	if(mChildren[iCh] == nd) {
	    delete mChildren[iCh];
	    mChildren.erase(mChildren.begin()+iCh);
	    break;
	}
}

void XMLNode::childClear( const string &name )
{
    //From the end it will be faster here
    for(int iCh = (int)mChildren.size()-1; iCh >= 0; iCh--)
	if(name.empty() || mChildren[iCh]->name() == name) childDel(iCh);
}

int XMLNode::childIns( int id, XMLNode * n )
{
    if(!n) return -1;
    if(id < 0) id = (int)childSize()+id;

    if(id < 0 || id > (int)childSize()) id = childSize();
    mChildren.insert(mChildren.begin()+id, n);
    n->mParent = this;

    return id;
}

XMLNode* XMLNode::childIns( int id, const string &name )
{
    XMLNode *n = new XMLNode(name);
    childIns(id, n);

    return n;
}

XMLNode* XMLNode::childGet( const int index, bool noex ) const
{
    int wIdx = (index >= 0) ? index : childSize() + index;
    if(wIdx >= 0 && wIdx < (int)childSize()) return mChildren[wIdx];
    if(noex) return NULL;
    throw TError("XMLNode", _("Child %d is not present."), index);
}

XMLNode* XMLNode::childGet( const string &name, const int numb, bool noex ) const
{
    for(int iCh = 0, i_n = 0; iCh < (int)childSize(); iCh++)
	if(strcasecmp(childGet(iCh)->name().c_str(),name.c_str()) == 0 && i_n++ == numb)
	    return childGet(iCh);

    if(noex) return NULL;
    throw TError("XMLNode", _("Child %s:%d is not found!"), name.c_str(), numb);
}

XMLNode* XMLNode::childGet( const string &attr, const string &val, bool noex ) const
{
    for(unsigned i_f = 0; i_f < childSize(); i_f++)
	if(childGet(i_f)->attr(attr) == val) return childGet(i_f);

    if(noex) return NULL;
    throw TError("XMLNode", _("Child with attribut %s=%s is not present."), attr.c_str(), val.c_str());
}

XMLNode* XMLNode::getElementBy( const string &iattr, const string &val )
{
    if(attr(iattr) == val)	return this;

    XMLNode* rez = NULL;
    for(unsigned iCh = 0; !rez && iCh < childSize(); iCh++)
	rez = childGet(iCh)->getElementBy(iattr,val);

    return rez;
}

XMLNode* XMLNode::root( )
{
    XMLNode *cur = this;
    while(cur->parent()) cur = cur->parent();
    return cur;
}

string	XMLNode::text( bool childs, bool recursive ) const
{
    if(!childs || mName == "<*>") return mText;

    string rez;
    for(unsigned i_f = 0; i_f < childSize(); i_f++)
	if(childGet(i_f)->name() == "<*>") rez += childGet(i_f)->text();
	else if(recursive) rez += childGet(i_f)->text(childs, recursive);

    return rez;
}

XMLNode* XMLNode::setText( const string &s, bool childs )
{
    if(!childs || mName == "<*>") { mText = s; return this; }

    int iCh = -1;
    for(int i_f = 0; i_f < (int)childSize(); i_f++)
	if(childGet(i_f)->name() == "<*>") {
	    if(iCh < 0) childGet(i_f)->mText = s;
	    else childDel(i_f--);
	    iCh = i_f;
	}
    if(iCh < 0) childAdd("<*>")->mText = s;

    return this;
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

void XMLNode::attrClear( )	{ mAttr.clear(); }

string XMLNode::attr( const string &name, bool caseSens ) const
{
    if(caseSens) {
	for(unsigned i_a = 0; i_a < mAttr.size(); i_a++)
	    if(mAttr[i_a].first == name) return mAttr[i_a].second;
    }
    else for(unsigned i_a = 0; i_a < mAttr.size(); i_a++)
	if(strcasecmp(mAttr[i_a].first.c_str(),name.c_str()) == 0) return mAttr[i_a].second;

    return "";
}

XMLNode* XMLNode::setAttr( const string &name, const string &val )
{
    for(unsigned i_a = 0; i_a < mAttr.size(); i_a++)
	if(mAttr[i_a].first == name) {
	    mAttr[i_a].second = val;
	    return this;
	}

    mAttr.push_back(pair<string,string>(name,val));

    return this;
}

XMLNode* XMLNode::clear( )
{
    attrClear();
    mText.clear();
    childClear();

    return this;
}

string XMLNode::save( unsigned flg, const string &cp ) const
{
    string xml;
    xml.reserve(10000);

    if(flg&(XMLHeader|XHTMLHeader)) xml += "<?xml version='1.0' encoding='"+cp+"' ?>\n";
    if(flg&XHTMLHeader)
	xml += "<!DOCTYPE html PUBLIC '-//W3C//DTD XHTML 1.0 Transitional//EN' 'http://www.w3.org/TR/xhtml1/DTD/xhtml1-transitional.dtd'>\n";

    saveNode(flg, xml, cp);
    return xml;
}

void XMLNode::saveNode( unsigned flg, string &xml, const string &cp ) const
{
    //Text block
    if(name() == "<*>")	{ encode(Mess->codeConvOut(cp,mText), xml, true, flg); return; }
    //Commentary block
    if(name() == "<!>")	{ if(!(flg&Clean)) xml += "<!--"+Mess->codeConvOut(cp,mText)+"-->"; return; }
    //Process special block <? ... ?>
    if(name().compare(0,2,"<?") == 0)
    { if(!(flg&Clean)) xml += name()+" "+Mess->codeConvOut(cp,mText)+(flg&BrSpecBlkPast?"?>\n":"?>"); return; }

    xml.append((flg&BrOpenPrev && xml.size() && xml[xml.size()-1] != '\n') ? "\n<" : "<");
    if(flg&MissTagEnc) xml.append(name());
    else encode(name(), xml, false, flg);

    for(unsigned i_a = 0; i_a < mAttr.size(); i_a++) {
	if(mAttr[i_a].second.empty()) continue;
	xml.append(" ");
	if(flg&MissAttrEnc) xml.append(mAttr[i_a].first);
	else encode(mAttr[i_a].first, xml, false, flg);
	xml.append("=\"");
	encode(Mess->codeConvOut(cp,mAttr[i_a].second), xml, false, flg);
	xml.append("\"");
    }

    if(childEmpty() && mText.empty())
	xml.append((((flg&XHTMLHeader)&&(name()=="script"||name()=="div"))?"></"+name()+">":" />") + ((flg&(BrOpenPast|BrClosePast))?"\n":""));
    else {
	xml.append((flg&BrOpenPast) ? ">\n" : ">");
	//Save text
	if(!mText.empty()) {
	    encode(Mess->codeConvOut(cp,mText), xml, true, flg);
	    xml.append(flg&BrTextPast ? "\n" : "");
	}
	//Save included childs
	for(unsigned iC = 0; iC < childSize(); iC++) childGet(iC)->saveNode(flg, xml, cp);
	//Close tag
	xml.append("</");
	if(flg&MissTagEnc) xml.append(name() );
	else encode(name(), xml, false, flg);
	xml.append(flg&BrClosePast ? ">\n" : ">");
    }
}

void XMLNode::encode( const string &s, string &rez, bool text, unsigned flg ) const
{
    int	len;
    int32_t symb;

    //Append UTF8 codes checking Mess->isUTF8(), at first it is for &nbsp; = \xC2\xA0"
    for(unsigned iSz = 0; iSz < s.size(); iSz++) {
	const char *replStr = NULL;
	switch(s[iSz]) {
	    case 0:	replStr = "\\000";	break;	//Bynary symbol but for next problems are possible also and here needs check to Unicode
	    //case 0:	replStr = "&#000;";	break;
	    case '>':	replStr = "&gt;";	break;
	    case '<':	replStr = "&lt;";	break;
	    case '&':	replStr = "&amp;";	break;
	    case '\'':	if(!text) replStr = "&#039;";	break;
	    case '"':	if(!text) replStr = "&quot;";	break;
	    case '\n':	if(!text) replStr = "&#010;";	break;
	}
	if(replStr) { rez += replStr; continue; }
	else if((len=Mess->getUTF8(s,iSz,&symb)) >= 2) {
	    switch(symb) {
		case 0xA0: rez += (flg&XHTMLHeader) ? "&nbsp;" : "&#x00A0;";	break;
		default: rez.append(s,iSz,len);	break;
	    }
	    iSz += len-1;
	    continue;
	}
	rez += s[iSz];
    }
}

void XMLNode::load( const string &s, unsigned flg, const string &cp )
{
    clear();

    LoadCtx ctx(s, flg);
    ctx.enc = cp;
    loadNode(ctx);
}

unsigned XMLNode::loadNode( LoadCtx &ctx, unsigned pos )
{
    bool initTag = true;
    unsigned cpos, bpos, tpos;

nextTag:
    //Find for a tag start symbol
    for( ; pos < ctx.vl.size() && ctx.vl[pos] != '<'; pos++) {
	if(initTag) continue;
	if((ctx.flg&(LD_Full|LD_NoTxtSpcRemEnBeg)) || mText.size() || !isspace(ctx.vl[pos])) {
	    if(!mText.size())	mText.reserve(100);
	    if(ctx.vl[pos] != '&') mText += ctx.vl[pos]; else parseEntity(ctx, pos, mText);
	}
    }
    if((pos+2) >= ctx.vl.size()) throw TError("XMLNode", _("Unexpected end. Pos: %d"), pos);
    cpos = pos;

    switch(ctx.vl[pos+1]) {
	case '!':	// Comment - special part
	    //Comment block
	    if(ctx.vl.compare(pos,4,"<!--") == 0) {
		size_t comBlkEnd = ctx.vl.find("-->",cpos+4);
		if(comBlkEnd == string::npos) throw TError("XMLNode", _("No comment block end. Pos: %d"), pos);
		if(ctx.flg&LD_Full) {
		    if(mText.size()) { childAdd("<*>")->mText = Mess->codeConvIn(ctx.enc,mText); mText.clear(); }
		    childAdd("<!>")->mText = Mess->codeConvIn(ctx.enc,ctx.vl.substr(cpos+4,comBlkEnd-(cpos+4)));
		}
		pos = comBlkEnd+3;
	    }
	    //Special "DOCTYPE" block
	    else if(ctx.vl.compare(pos,10,"<!DOCTYPE ") == 0) {
		// Find subblock
		for(cpos += 10; ctx.vl[cpos] != '['; cpos++)
		    if(cpos >= ctx.vl.size()) throw TError("XMLNode", _("Unfinished '!DOCTYPE' block. Pos: %d"), pos);
		    else if(ctx.vl[cpos] == '>') { pos = cpos+1; goto nextTag; }
		// Process entities container
		for(cpos += 1; ctx.vl[cpos] != ']'; cpos++)
		    if(cpos >= ctx.vl.size()) throw TError("XMLNode", _("Unfinished '!DOCTYPE [ ]' container. Pos: %d"), pos);
		    else if(ctx.vl.compare(cpos,9,"<!ENTITY ") == 0) {
			for(cpos += 9; parseAttr(ctx,cpos,0); )
			    if(ctx.aVl.size()) ctx.ent.insert(pair<string,string>(ctx.aNm,ctx.aVl));
			while(isspace(ctx.vl[cpos])) cpos++;
			if(ctx.vl[cpos] != '>')	throw TError("XMLNode", _("Unexpected or error end tag. Pos: %d"), cpos);
			cpos++;
		    }
		cpos++;
		while(isspace(ctx.vl[cpos])) cpos++;
		if(ctx.vl[cpos] != '>')	throw TError("XMLNode", _("Unexpected or error end tag. Pos: %d"), cpos);
		pos = cpos+1;
	    }
	    else throw TError("XMLNode", _("Unfinished start comment or unknown block. Pos: %d"), pos);
	    goto nextTag;
	case '?':	//Program block
	    for(bpos = cpos+2, tpos = bpos; ctx.vl.compare(tpos,2,"?>") != 0; tpos++)
		if(tpos >= ctx.vl.size()) throw TError("XMLNode", _("Unexpected end. Pos: %d"), pos);
	    //Get program block's name
	    // Get tag name
	    cpos = bpos;
	    while(cpos < tpos && !isspace(ctx.vl[cpos])) cpos++;
	    if(cpos < tpos) {
		string nm(ctx.vl,bpos,cpos-bpos);
		// Pass spaces
		while(isspace(ctx.vl[cpos])) cpos++;
		// Place program block
		if(!initTag) childAdd("<?"+nm)->mText = ctx.vl.substr(cpos,tpos-cpos);
		// Process specific block <?xml ?>
		if(nm == "xml")
		    while(parseAttr(ctx,cpos))
			if(ctx.aNm.compare("encoding") == 0) ctx.enc.assign(ctx.aVl);
	    }
	    pos = tpos+2;
	    goto nextTag;
	case '/':	//End tag
	    if(ctx.vl.compare(cpos+2,mName.size(),mName) == 0) {
		cpos += 2+mName.size();
		while(isspace(ctx.vl[cpos])) cpos++;
		if(ctx.vl[cpos] == '>') {
		    if(mText.size() && (ctx.flg&LD_Full)) { childAdd("<*>")->mText = Mess->codeConvIn(ctx.enc, mText); mText.clear(); }
		    if(mText.size()) {
			if(ctx.flg&LD_NoTxtSpcRemEnBeg) mText = Mess->codeConvIn(ctx.enc, mText);
			else {
			    //Remove spaces from end of the text, trim
			    int iCh = mText.size()-1;
			    while(iCh >= 0 && isspace(mText[iCh])) iCh--;
			    mText = Mess->codeConvIn(ctx.enc, mText.substr(0,iCh+1));
			}
		    }
		    return cpos+1;
		}
	    }
	    throw TError("XMLNode", _("Unexpected or error end tag. Pos: %d"), cpos);
    }
    //Process for standard XML node
    // It's my node
    if(initTag) {
	bpos = cpos+1;
	//  Get the tag name
	for(cpos = bpos; !isspace(ctx.vl[cpos]) && ctx.vl[cpos] != '>' && ctx.vl[cpos] != '/'; cpos++)
	    if(cpos >= ctx.vl.size()) throw TError("XMLNode", _("Unexpected end. Pos: %d"), pos);
	mName.assign(ctx.vl, bpos, cpos-bpos);
	initTag = false;
	//  Process the tag attributes
	while(parseAttr(ctx,cpos))
	    mAttr.push_back(pair<string,string>(ctx.aNm,ctx.aVl.size()?Mess->codeConvIn(ctx.enc,ctx.aVl):string("")));
	//  Pass spaces
	while(isspace(ctx.vl[cpos])) cpos++;
	//  Process close tag or the tag content
	if(ctx.vl[cpos] == '>') { pos = cpos+1; goto nextTag; }
	else if(ctx.vl.compare(cpos,2,"/>") == 0) return cpos+2;
	throw TError("XMLNode", _("Start tag error. Pos: %d"), cpos);
    }
    // New XML node create
    else {
	if(mText.size() && (ctx.flg&LD_Full)) { childAdd("<*>")->mText = Mess->codeConvIn(ctx.enc, mText); mText.clear(); }
	pos = childAdd()->loadNode(ctx, pos-1);
	goto nextTag;
    }

    return cpos;
}

bool XMLNode::parseAttr( LoadCtx &ctx, unsigned &pos, char sep )
{
    //Get attribute name
    // Pass spaces
    while(isspace(ctx.vl[pos])) pos++;
    if(!isalpha(ctx.vl[pos]) && !isxdigit(ctx.vl[pos])) return false;

    unsigned bpos = pos;
    for( ; !isspace(ctx.vl[pos]) && ctx.vl[pos] != '='; pos++)
	if(pos >= ctx.vl.size()) throw TError("XMLNode", _("Unexpected end. Pos: %d"), pos);
    ctx.aNm.assign(ctx.vl,bpos,pos-bpos);
    //Get symbol '='
    // Pass spaces
    while(isspace(ctx.vl[pos])) pos++;
    if(sep) {
	if(ctx.vl[pos] != sep) throw TError("XMLNode", _("Unfinished attribute. Pos: %d"), bpos);
	pos++;
    }
    //Get symbol "'" or '"'
    // Pass spaces
    while(isspace(ctx.vl[pos])) pos++;
    if(ctx.vl[pos] != '\'' && ctx.vl[pos] != '"') throw TError("XMLNode", _("Unfinished attribute. Pos: %d"), bpos);
    char brc = ctx.vl[pos];
    //Get value
    ctx.aVl.clear();
    for(pos++; ctx.vl[pos] != brc; pos++) {
	if(pos >= ctx.vl.size()) throw TError("XMLNode", _("Unexpected end. Pos: %d"), pos);
	if(!ctx.aVl.size())	ctx.aVl.reserve(20);
	if(ctx.vl[pos] != '&')	ctx.aVl += ctx.vl[pos]; else parseEntity(ctx, pos, ctx.aVl);
    }
    pos++;

    return true;
}

void XMLNode::parseEntity( LoadCtx &ctx, unsigned &rpos, string &rez )
{
    //Check for standard entities
    if(ctx.vl.compare(rpos,5,"&amp;") == 0)		{ rpos += 4; rez += '&'; }
    else if(ctx.vl.compare(rpos,4,"&lt;") == 0)		{ rpos += 3; rez += '<'; }
    else if(ctx.vl.compare(rpos,4,"&gt;") == 0)		{ rpos += 3; rez += '>'; }
    else if(ctx.vl.compare(rpos,6,"&apos;") == 0)	{ rpos += 5; rez += '\''; }
    else if(ctx.vl.compare(rpos,6,"&quot;") == 0)	{ rpos += 5; rez += '"'; }
    else if(Mess->isUTF8() && ctx.vl.compare(rpos,6,"&nbsp;") == 0)	{ rpos += 5; rez += "\xC2\xA0"; }
    //Check for code entities
    else if((rpos+3) < ctx.vl.size() && ctx.vl[rpos+1] == '#') {
	uint32_t eVal = 0;
	if(ctx.vl[rpos+2] == 'X' || ctx.vl[rpos+2] == 'x') {
	    rpos += 3;
	    unsigned nBeg = rpos;
	    while(isxdigit(ctx.vl[rpos])) rpos++;
	    if(ctx.vl[rpos] != ';') throw TError("XMLNode", _("Entity error. Pos: %d"), nBeg-3);
	    eVal = strtoul(ctx.vl.data()+nBeg, NULL, 16);
	}
	else {
	    rpos += 2;
	    unsigned nBeg = rpos;
	    while(isdigit(ctx.vl[rpos])) rpos++;
	    if(ctx.vl[rpos] != ';') throw TError("XMLNode", _("Entity error. Pos: %d"), nBeg-2);
	    eVal = strtoul(ctx.vl.data()+nBeg, NULL, 10);
	}
	//Value process		!!!! Rewrote
	if(eVal < 0x80)	rez += (char)eVal;
	else if(Mess->isUTF8())
	    for(int iCh = 5, i_st = -1; iCh >= 0; iCh--) {
		if(i_st < iCh && (eVal>>(iCh*6))) i_st = iCh;
		if(iCh == i_st) rez += (char)(0xC0|(eVal>>(iCh*6)));
		else if(iCh < i_st) rez += (char)(0x80|(0x3F&(eVal>>(iCh*6))));
	    }
    }
    //Check for loaded entities
    else {
	rpos += 1;
	unsigned nBeg = rpos;
	for( ; ctx.vl[rpos] != ';'; rpos++)
	    if(rpos >= ctx.vl.size()) throw TError("XMLNode", _("Entity error. Pos: %d"), nBeg-1);
	map<string,string>::iterator ient = ctx.ent.size() ? ctx.ent.find(ctx.vl.substr(nBeg,rpos-nBeg)) : ctx.ent.end();
	if(ient != ctx.ent.end()) rez += ient->second;
	else {
	    rez += '?';
	    mess_debug("/XMLNode", _("Unknown entity '%s'. Pos: %d"), ctx.vl.substr(nBeg,rpos-nBeg).c_str(), rpos);
	}
    }
}


//*************************************************
//* XMLNode::LoadCtx                              *
//*************************************************
XMLNode::LoadCtx::LoadCtx( const string &ivl, unsigned iflg ) : flg(iflg), enc("UTF-8")
{
    vl = ivl+char(0);
}
