
//OpenSCADA module UI.WebVision file: web_vision.h
/***************************************************************************
 *   Copyright (C) 2007-2021 by Roman Savochenko, <roman@oscada.org>       *
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

#ifndef WEB_VISION_H
#define WEB_VISION_H

#include <string>
#include <vector>
#include <map>

#include <tuis.h>

#include "vca_sess.h"

#undef _
#define _(mess) mod->I18N(mess)

#define rgb(r,g,b) (((r)<<16)+((g)<<8)+(b))

#define POS_PREC_DIG	3

using std::string;
using std::vector;
using std::map;
using namespace OSCADA;

namespace WebVision
{

//*************************************************
//* SSess                                         *
//*************************************************
struct SSess
{
    SSess( const string &iuser ) : user(iuser), mRoot(-1)	{ }
    SSess( const string &iurl, const string &isender, const string &iuser, vector<string> &ivars,
	    const string &icontent, TProtocolIn *iprt );

    bool isRoot( );

    //Attributes
    TProtocolIn	*prt;		//input protocol
    string	url,		//request URL
		page,
		sender,		//request sender
		user,		//sesion user
		content,	//Contains
		gPrms,		//Global parameters
		lang;		//Language
    int		mRoot;		//Root access user

    vector<string>	vars;	//request vars
    vector<XMLNode>	cnt;	//Parsed contain
    map<string,string>	prm;	//URL parameters
};

//************************************************
//* TWEB                                         *
//************************************************
class TWEB: public TUI
{
    public:
	//Data
	enum MessLev	{ Info, Warning, Error };

	//Methods
	TWEB( string name );
	~TWEB( );

	int	sessTime( )				{ return mTSess; }
	int	sessLimit( )				{ return mSessLimit; }
	int	PNGCompLev( )				{ return mPNGCompLev; }
	double	cachePgLife( )				{ return mCachePgLife; }
	int	cachePgSz( )				{ return mCachePgSz; }
	bool	imgResize( )				{ return mImgResize; }
	string	custCSS( )				{ return mCustCSS; }

	void setSessTime( int vl )			{ mTSess = vmax(1,vmin(24*60,vl)); modif(); }
	void setSessLimit( int vl )			{ mSessLimit = vmax(1,vmin(100,vl)); modif(); }
	void setCachePgLife( double vl )		{ mCachePgLife = vmax(0, vmin(1000,vl)); modif(); }
	void setCachePgSz( int vl )			{ mCachePgSz = vmax(0, vmin(100,vl)); modif(); }
	void setPNGCompLev( int vl )			{ mPNGCompLev = vmax(-1,vmin(9,vl)); modif(); }
	void setImgResize( bool vl )			{ mImgResize = vl; modif(); }
	void setCustCSS( const string &vl )		{ mCustCSS = vl; modif(); }

	// VCA sessions
	void vcaSesList( vector<string> &list ) const	{ chldList(id_vcases,list); }
	bool vcaSesPresent( const string &name ) const	{ return chldPresent(id_vcases,name); }
	void vcaSesAdd( const string &name );
	void vcaSesDel( const string &name )		{ chldDel(id_vcases, name); }
	AutoHD<VCASess> vcaSesAt( const string &name ) const	{ return chldAt(id_vcases, name); }

	// Web process methods
	void HTTP_GET( const string &url, string &page, vector<string> &vars, const string &user, TProtocolIn *iprt );
	void HTTP_POST( const string &url, string &page, vector<string> &vars, const string &user, TProtocolIn *iprt );

	string optDescr( );
	void   modInfo( vector<string> &list );
	string modInfo( const string &name );

	string pgCreator( TProtocolIn *iprt, const string &cnt, const string &rcode = "", const string &httpattrs = "",
	    const string &htmlHeadEls = "", const string &forceTmplFile = "", const string &lang = "" );
	bool pgAccess( TProtocolIn *iprt, const string &URL );

	int cntrIfCmd( XMLNode &node, const SSess &ses, bool VCA = true );

	void imgConvert( SSess &ses, const string &mime = "" );
	int colorParse( const string &clr );
	static int colorResolve( gdImagePtr im, int clr );
	string trMessReplace( const string &tsrc );

	void perSYSCall( unsigned int cnt );

	ResMtx &cacheRes( )	{ return mCacheRes; }

	// Post message dialog
	string messPost( const string &cat, const string &mess, MessLev type = Info );

    protected:
	//Methods
	void load_( );
	void save_( );

	void modStart( );
        void modStop( );

	void cntrCmdProc( XMLNode *opt );		//Control interface command process

    private:
	//Attributes
	int		mTSess;				//Time of sesion life (minutes)
	int		mSessLimit;			//Sessions limit
	double		mCachePgLife;			//Cached pages lifetime
	int		mCachePgSz;			//Cached pages number
	int		mPNGCompLev;			//PNG images compression level
	bool		mImgResize;			//Resizing raster images on the server side
	int		id_vcases;			//VCA session's container identifier
	map<string,int> colors;				//Named colors
	MtxString	mCustCSS;			//Custom CSS

	ResRW		mSesRes;			//Sessions resource
	ResMtx		mCacheRes;
};

extern TWEB *mod;
}
#endif //WEB_VISION_H
