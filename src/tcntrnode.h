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
 
#ifndef TCNTRNODE_H
#define TCNTRNODE_H

#include <string>
#include <vector>

#include "terror.h"
#include "xml.h"
#include "autohd.h"

using std::string;
using std::vector;

//***************************************************************
//* TCntrNode - Controll node					*
//***************************************************************
class TCntrNode
{        
    //***********************************************************
    //******* Controll scenaries language section ***************
    //***********************************************************
    public:
	enum Command { Info, Get, Set };
    
	TCntrNode( TCntrNode *prev = NULL );
	virtual ~TCntrNode( );

	void cntrCmd( const string &path, XMLNode *opt, TCntrNode::Command cmd, int lev = 0 );
	
	//============== Static functions =======================
	static XMLNode *ctrId( XMLNode *inf, const string &n_id );      //get node for he individual number
	static string ctrChk( XMLNode *fld, bool fix = false );		// Check fld valid
	
	// Controll Fields
	static XMLNode *ctrMkNode( const char *n_nd, XMLNode *nd, const char *req, const char *path, 
	    const string &dscr, int perm=0777, int uid=0, int gid=0, const char *tp="" );	
	static XMLNode *ctrInsNode( const char *n_nd, int pos, XMLNode *nd, const char *req, const char *path, 
	    const string &dscr, int perm=0777, int uid=0, int gid=0, const char *tp="" );
	
	// Get option's values
	static string ctrGetS( XMLNode *fld );	//string
	static int    ctrGetI( XMLNode *fld );	//integer
	static double ctrGetR( XMLNode *fld );	//real
        static bool   ctrGetB( XMLNode *fld );	//boolean
	
	// Set option's values	
	static void ctrSetS( XMLNode *fld, const string &val, const char *id=NULL );	//string
	static void ctrSetI( XMLNode *fld, int val, const char *id=NULL );   	//integer
	static void ctrSetR( XMLNode *fld, double val, const char *id=NULL );	//real
	static void ctrSetB( XMLNode *fld, bool val, const char *id=NULL );	//boolean

    protected:
	virtual void cntrCmd_( const string &path, XMLNode *opt, TCntrNode::Command cmd ){ }
	
    //***********************************************************
    //*********** Resource section ******************************
    //***********************************************************
    public:
	enum Mode { MkDisable, Disable, MkEnable, Enable };
	
       	virtual string nodeName()	{ return "NO Named!"; }
	virtual string nodeType()       { return "TCntrNode"; }	
	string nodePath();
	
	void nodeList(vector<string> &list);		//Full node list
	AutoHD<TCntrNode> nodeAt(const string &path, int lev = 0, char sep = 0 );	//Get node for full path
	
	TCntrNode *nodePrev();
        Mode nodeMode()			{ return m_mod; }
	unsigned nodeUse( );

	void connect();
	void disConnect();
    
    protected:
	//Commands
	void nodeEn();
	void nodeDis(long tm = 0,int flag = 0);
	
	void nodeDelAll( );	//For hard link objects
	
	void nodePrev( TCntrNode *node )	{ prev.node = node; }
	
	//Conteiners
        unsigned grpSize()	{ return chGrp.size(); }
        unsigned grpAdd( const string &iid );
	
	//Childs
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
	//Child atributes	
	int 	hd_res;				//Resource HD
	struct GrpEl
	{
	    string 	id;
	    vector<TCntrNode*>	el;
	};
	vector<GrpEl>	chGrp;	//Child groups
	
	//Curent node atributes
	static long	dtm;			//Default timeout
	static XMLNode	m_dummy;		//Dummy node for noview requests
	
	int     m_use;                          //Use counter
	struct
	{
	    TCntrNode	*node;
	    int		grp;
	} prev;
	
	Mode	m_mod;
};

#endif //TCNTRNODE_H

