
//OpenSCADA system file: tcntrnode.h
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
 
#ifndef TCNTRNODE_H
#define TCNTRNODE_H

#include <string>
#include <vector>

#include "xml.h"
#include "autohd.h"

#define DEF_TIMEOUT 2

using std::string;
using std::vector;

//***************************************************************
//* TCntrNode - Controll node					*
//***************************************************************
class TCntrNode
{        
    //******* Controll scenaries language section ***************
    public:
	//Methods
	TCntrNode( TCntrNode *prev = NULL );
	virtual ~TCntrNode( );

	void cntrCmd( XMLNode *opt, int lev = 0 );
	
	//- Static functions -
	static XMLNode *ctrId( XMLNode *inf, const string &n_id, bool noex = false );      //get node for he individual number
	
	//-- Controll Field --
	static XMLNode *ctrMkNode( const char *n_nd, XMLNode *nd, int pos, const char *req, const string &dscr, 
	    int perm=0777, const char *user="root", const char *grp="root", int n_attr=0, ... );
	static bool ctrChkNode( XMLNode *nd, const char *cmd="get", int perm=0444, const char *user="root", 
	    const char *grp="root", char mode=04, const char *warn = NULL ); 
	
    protected:
	//Methods
	virtual void cntrCmdProc( XMLNode *req );
	
    //*********** Resource section ******************************
    public:
	//Data
	enum Mode { MkDisable, Disable, MkEnable, Enable };
	
	//Methods
       	virtual string nodeName()	{ return "NO Named!"; }
	string nodePath( char sep = 0 );
	
	void nodeList(vector<string> &list);		//Full node list
	AutoHD<TCntrNode> nodeAt(const string &path, int lev = 0, char sep = 0 );	//Get node for full path
	
	TCntrNode *nodePrev();
        Mode nodeMode()			{ return m_mod; }
	unsigned nodeUse( );

	void connect();
	void disConnect();
    
    protected:
	//Methods
	//- Commands -
	void nodeEn();
	void nodeDis(long tm = 0,int flag = 0);
	
	void nodeDelAll( );	//For hard link objects
	
	void nodePrev( TCntrNode *node )	{ prev.node = node; }
	
	//- Conteiners -
        unsigned grpSize()	{ return chGrp.size(); }
        unsigned grpAdd( const string &iid );
	
	//- Childs -
	void chldList( unsigned igr, vector<string> &list );
	bool chldPresent( unsigned igr, const string &name );
	void chldAdd( unsigned igr, TCntrNode *node, int pos = -1 );
	void chldDel( unsigned igr, const string &name, long tm = -1, int flag = 0 );

        AutoHD<TCntrNode> chldAt( unsigned igr, const string &name, const string &user = "" );
	
	virtual void preEnable()	{ }
	virtual void postEnable()	{ }
	
	virtual void preDisable(int flag)	{ }
	virtual void postDisable(int flag)	{ }

    private:
	//Attributes
	//- Childs -	
	int 	hd_res;				//Resource HD
	struct GrpEl
	{
	    string 	id;
	    vector<TCntrNode*>	el;
	};
	vector<GrpEl>	chGrp;	//Child groups
	
	//- Curent node -
	int     m_use;                          //Use counter
	struct
	{
	    TCntrNode	*node;
	    int		grp;
	} prev;
	
	Mode	m_mod;
};

#endif //TCNTRNODE_H

