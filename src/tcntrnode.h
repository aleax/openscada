
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

//Security standard permissions
#define R_R_R_  0444
#define R_R___  0440
#define R_____  0400
#define RWRWRW  0666
#define RWRWR_  0664
#define RWR_R_  0644
#define RWR___  0640
#define RW____  0600


using std::string;
using std::vector;

class TCntrNode;

#if OSC_HASHMAP
#include <ext/hash_map>
using __gnu_cxx::hash_map;

namespace __gnu_cxx
{
template <> class hash<string>
{
    public:
        size_t operator()(const string& h) const
        {
            return hash<const char*>()(h.c_str());
        }
};
}
typedef hash_map<string, TCntrNode*, __gnu_cxx::hash<string> > TMap;
#else
#include <map>
using std::map;
typedef map<string, TCntrNode* > TMap;
#endif

#include "xml.h"
#include "autohd.h"
#include "resalloc.h"

#define DEF_TIMEOUT 2

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

	void cntrCmd( XMLNode *opt, int lev = 0, const string &path = "", int off = 0 );
	
	//- Static functions -
	static XMLNode *ctrId( XMLNode *inf, const string &n_id, bool noex = false );      //get node for it full identifier
	
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
	enum Flag 
	{ 
	    NodeConnect = 0x01,	//Connect node to control tree
	    NodeRestore = 0x02	//Restore node enabling after broken disabling. 
	};
	
	//Methods
       	virtual string nodeName( )	{ return "NO Named!"; }
	string nodePath( char sep = 0, bool from_root = false );
	
	void nodeList( vector<string> &list, const string& gid = "" );				//Full node list
	AutoHD<TCntrNode> nodeAt( const string &path, int lev = 0, char sep = 0, int off = 0 );	//Get node for full path
	//void nodeDel( const string &path, char sep = 0, int flag = 0 );			//Delete node at full path
	
	TCntrNode *nodePrev( bool noex = false );
        Mode nodeMode()			{ return m_mod; }
	unsigned nodeUse( );

	void AHDConnect();
	void AHDDisConnect();
    
    protected:
	//Methods
	//- Commands -
	void nodeEn( int flag = 0 );
	void nodeDis( long tm = 0, int flag = 0 );
	
	void nodeDelAll( );	//For hard link objects
	
	void nodePrev( TCntrNode *node )	{ prev.node = node; }
	
	//- Conteiners -
        unsigned grpSize()	{ return chGrp.size(); }
        unsigned grpAdd( const string &id, bool ordered = false );
	
	//- Childs -
	void chldList( unsigned igr, vector<string> &list );
	bool chldPresent( unsigned igr, const string &name );
	void chldAdd( unsigned igr, TCntrNode *node, int pos = -1 );
	void chldDel( unsigned igr, const string &name, long tm = -1, int flag = 0 );

        AutoHD<TCntrNode> chldAt( unsigned igr, const string &name, const string &user = "" );
	
	virtual void preEnable(int flag)	{ }
	virtual void postEnable(int flag)	{ }
	
	virtual void preDisable(int flag)	{ }
	virtual void postDisable(int flag)	{ }

    private:
	//Attributes
	//- Childs -	
	Res 	hd_res,			//Resource HD
		conn_res;		//Connect resource
	struct GrpEl
	{
	    string 	id;
	    bool	ordered;
	    TMap 	elem;
	    //vector<TCntrNode*>	el;
	};
	vector<GrpEl>	chGrp;		//Child groups
	
	//- Curent node -
	unsigned char		m_use;	//Use counter
	unsigned short int	m_oi;	//Order index
	struct
	{
	    TCntrNode	*node;
	    int		grp;
	} prev;
	
	Mode	m_mod;
};

#endif //TCNTRNODE_H

