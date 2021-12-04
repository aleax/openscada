
//OpenSCADA file: tcntrnode.h
/***************************************************************************
 *   Copyright (C) 2003-2021 by Roman Savochenko, <roman@oscada.org>       *
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

#ifndef TCNTRNODE_H
#define TCNTRNODE_H

#include <string.h>
#include <pthread.h>

#include <string>
#include <vector>

#include "xml.h"
#include "autohd.h"
#include "resalloc.h"

//#ifndef OSC_HASHMAP
#include <map>
using std::map;
/*#else
#include <ext/hash_map>
using __gnu_cxx::hash_map;
//#include <unordered_map>
//using std::unordered_map;
#endif*/

//> Security standard permissions
#define R_R_R_  0444
#define R_R___  0440
#define R_____  0400
#define RWRWRW  0666
#define RWRWR_  0664
#define RWR_R_  0644
#define RWRW__  0660
#define RWR___  0640
#define RW____  0600

using std::string;
using std::vector;

namespace OSCADA
{

class TCntrNode;

//#ifndef OSC_HASHMAP
struct StrPntLess
{
    bool operator()(const char *s1, const char *s2) const	{ return strcmp(s1,s2) < 0; }
};

typedef map<const char*, TCntrNode*, StrPntLess> TMap;
/*#else
namespace __gnu_cxx
{
    template <> class hash<const char*>
    {
	public:
	    size_t operator()(const char *h) const	{ return hash<const char*>()(h); }
    };

}
typedef hash_map<const char*, TCntrNode*, __gnu_cxx::hash<string> > TMap;

//typedef unordered_map<string, TCntrNode* > TMap;

#endif*/

//***************************************************************
//* TCntrNode - Controll node					*
//***************************************************************
class TVariant;
class TConfig;

//#pragma pack(push,1)
class TCntrNode
{
    //* Controll scenaries language section *
    public:
	//Methods
	TCntrNode( TCntrNode *prev = NULL );
	TCntrNode( const TCntrNode &src );
	virtual ~TCntrNode( );

	virtual string objName( )	{ return "TCntrNode"; }

	virtual TCntrNode &operator=( const TCntrNode &node );

	void cntrCmd( XMLNode *opt, int lev = 0, const string &path = "", int off = 0 );

	// Static functions
	//  Controll Field
	static XMLNode *ctrId( XMLNode *inf, const string &n_id, bool noex = false );		//get node for it full identifier
	static XMLNode *ctrMkNode( const char *n_nd, XMLNode *nd, int pos, const char *req, const string &dscr,
	    int perm = 0777, const char *user = "root", const char *grp = "root", int n_attr = 0, ... );
	static XMLNode *ctrMkNode2( const char *n_nd, XMLNode *nd, int pos, const char *req, const string &dscr,
	    int perm = 0777, const char *user = "root", const char *grp = "root", ... );	//End by zero pointer
	static XMLNode *_ctrMkNode( const char *n_nd, XMLNode *nd, int pos, const char *req, const string &dscr,
	    int perm = 0777, const char *user = "root", const char *grp = "root" );
	static bool ctrRemoveNode( XMLNode *nd, const char *path );
	static bool ctrChkNode( XMLNode *nd, const char *cmd = "get", int perm = 0444, const char *user = "root",
	    const char *grp = "root", char mode = 04, const char *warn = NULL );

    protected:
	//Methods
	virtual void cntrCmdProc( XMLNode *req );

    //* Resource section *
    public:
	//Data
	enum Flag {
	    // Modes
	    DoDisable	= 0x00,		//Node do disable
	    Disabled	= 0x01,		//Node disabled
	    DoEnable	= 0x02,		//Node do enable
	    Enabled	= 0x03,		//Node enabled
	    // Flags
	    SelfModify	= 0x04,		//Self modify
	    SelfModifyS	= 0x08,		//Self modify store
	    SelfSaveForceOnChild = 0x10	//Save force on childs modify flag set
	};
	// Node's enable and disable flags
	enum EnDisFlag {
	    NodeNoFlg	= 0,
	    //  Enable flags
	    NodeConnect	= 0x01,		//Connect node to control tree
	    NodeRestore	= 0x02,		//Restore node enabling after broken disabling.
	    //  Disable flags
	    NodeRemove	= 0x01,		//Completely remove node
	    NodeRemoveOnlyStor = 0x02	//Remove node from the storage
	    //  > 0x100 - reserved for the node specific 
	};
	enum ModifFlag	{ Self = 0x01, Child = 0x02, All = 0x03 };

	//Methods
	ResMtx &dataRes( ) { return mDataM; }	//Generic node's data mutex
						//Allowed for using by heirs into the data resources allocation
						//  not for long-term functions-tasks resources allocation!
	virtual const char *nodeName( ) const		{ return ""; }
	virtual const char *nodeNameSYSM( ) const	{ return ""; }
	string nodePath( char sep = 0, bool from_root = true ) const;

	void nodeList( vector<string> &list, const string& gid = "" );	//Full node list
	AutoHD<TCntrNode> nodeAt( const string &path, int lev = 0, char sep = 0, int off = 0, bool noex = false );	//Get node for full path
	void nodeDel( const string &path, char sep = 0, int flag = NodeNoFlg );	//Delete node at full path
	static void nodeCopy( const string &src, const string &dst, const string &user = "root" );

	TCntrNode *nodePrev( bool noex = false ) const;
	char	 nodeFlg( )		{ return mFlg; }
	void	 setNodeFlg( char flg );
	char	 nodeMode( ) const	{ return mFlg&0x3; }
	unsigned nodeUse( bool selfOnly = false );
	unsigned nodePos( )		{ return mOi; }

	// Modify process methods
	int  isModify( int mflg = TCntrNode::All );	//Check for modify want
	void modif( bool save = false );		//Set local modify
	void modifG( );					//Set group modify
	void modifClr( bool save = false );		//Clear modify
	void modifGClr( );				//Modify group clear
	void load( TConfig *cfg = NULL, string *errs = NULL );	//Load node, if modified
	void save( unsigned lev = 0, string *errs = NULL );	//Save node, if modified

	// Connections counter
	virtual void AHDConnect( );
	virtual bool AHDDisConnect( );

	void mess_sys( int8_t level, const char *fmt,  ... );
	TError err_sys( const char *fmt,  ... ) const;
	TError err_sys( int cod, const char *fmt,  ... ) const;

	// User object access
	virtual TVariant objPropGet( const string &id );
	virtual void objPropSet( const string &id, TVariant val );
	virtual TVariant objFuncCall( const string &id, vector<TVariant> &prms, const string &user );

	// Childs
	int8_t	grpSize( );
	int8_t	grpId( const string &sid );
	virtual AutoHD<TCntrNode> chldAt( int8_t igr, const string &name, const string &user = "" ) const;
	virtual void chldList( int8_t igr, vector<string> &list, bool noex = false, bool onlyEn = true ) const;
	virtual bool chldPresent( int8_t igr, const string &name ) const;

    protected:
	//Data
	struct GrpEl {
	    string	id;
	    bool	ordered;
	    TMap	elem;
	};

	//Methods
	// Commands
	void nodeEn( int flag = NodeNoFlg );
	void nodeDis( long tm = 0, int flag = NodeNoFlg );

	void nodeDelAll( );	//For hard link objects call from destructor

	void setNodePrev( TCntrNode *node )	{ prev.node = node; }
	void setNodeMode( char mode );

	ResMtx &chM( )				{ return mChM; }	//Child nodes mutex

	// Childs and containers
	GrpEl	&grpAt( int8_t id );
	unsigned grpAdd( const string &id, bool ordered = false );
	void	grpDel( int8_t id );
	virtual string chldAdd( int8_t gr, TCntrNode *node, int pos = -1, bool noExp = false );
	void chldDel( int8_t gr, const string &name, long tm = -1, int flags = NodeNoFlg );

	virtual void preEnable( int flag )	{ }
	virtual void postEnable( int flag )	{ }

	virtual void preDisable( int flag )	{ }
	virtual void postDisable( int flag )	{ }

	virtual void load_( TConfig *cfg )	{ }
	virtual void load_( )			{ }	//In the begin
	virtual void load__( )			{ }	//In the end
	virtual void save_( )			{ }

	// Storage
	string storage( const string &cnt, bool forQueueOfData = false ) const;
	void setStorage( string &cnt, const string &vl, bool forQueueOfData = false );	//Empty <vl> and <forQueueOfData> removes the first storage from the queue

    private:
	//Data
	struct {
	    TCntrNode	*node;
	    int8_t	grp;
	} prev;

	//Attributes
	ResMtx	mChM, mDataM;		//Childs and generic data mutexes

	// Childs
	vector<GrpEl>		*chGrp;	//Childs groups

	// Curent node
	unsigned short int	mUse;	//Use counter
	unsigned short int	mOi;	//Order index

	char	mFlg;
};
//#pragma pack(pop)

}

#endif //TCNTRNODE_H
