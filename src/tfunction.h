
//OpenSCADA file: tfunction.h
/***************************************************************************
 *   Copyright (C) 2003-2018 by Roman Savochenko, <rom_as@oscada.org>      *
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

#ifndef TFUNCTIONS_H
#define TFUNCTIONS_H

#include <string>
#include <vector>

#include "tmess.h"
#include "tcntrnode.h"
#include "tvariant.h"

using std::string;
using std::vector;

namespace OSCADA
{

//*************************************************
//* IO                                            *
//*************************************************
class TFunction;

class IO
{
    friend class TFunction;
    public:
	//Data
	enum Type { String, Integer, Real, Boolean, Object };
	enum IOFlgs {
	    Default	= 0x00,	//Default mode (Input IO)
	    Output	= 0x01,
	    Return	= 0x02,
	    FullText	= 0x04,
	    Selectable	= 0x08
	};

	//Methods
	IO( const char *id, const char *name, IO::Type type, unsigned flgs, const char *def = "",
		bool hide = false, const char *rez = "" );
	IO( const IO &src );

	IO &operator=( const IO &iio );

	const string &id( ) const	{ return mId; }
	const string &name( ) const	{ return mName; }
	const Type &type( ) const	{ return mType; }
	unsigned flg( ) const		{ return mFlg; }
	const string &def( ) const	{ return mDef; }
	bool  hide( ) const		{ return mHide; }
	const string &rez( ) const	{ return mRez; }

	void setId( const string &val );
	void setName( const string &val );
	void setType( Type val );
	void setFlg( unsigned val );
	void setDef( const string &val );
	void setHide( bool val );
	void setRez( const string &val );

    private:
	//Attributes
	string	mId;
	string	mName;
	Type	mType;
	unsigned mFlg;
	string	mDef;
	bool	mHide;
	string	mRez;

	TFunction *owner;
};

//*************************************************
//* Function abstract object                      *
//*************************************************
class TValFunc;

class TFunction : public TCntrNode
{
    public:
	//Methods
	TFunction( const string &id, const char *grp = "root", const string &stor = "" );
	TFunction( const TFunction &src );
	virtual ~TFunction( );

	TFunction &operator=( const TFunction &func );

	string	id( ) const		{ return mId.c_str(); };
	virtual	string name( )		{ return ""; }
	virtual	string descr( )		{ return ""; }
	virtual	string prog( )		{ return ""; }
	virtual	string stor( ) const	{ return mStor; }
	bool	startStat( ) const	{ return runSt; }
	int	use( )			{ return used.size(); }
	ResRW	&fRes( )		{ return mFRes; }

	void setId( const string &vl );
	virtual void setProg( const string &prg )	{ }
	void setStor( const string &istor )		{ mStor = istor; }
	virtual void setStart( bool val );

	// IO
	void ioList( vector<string> &list );
	int ioId( const string &id ) const;
	int ioSize( ) const;
	IO *io( int id ) const;
	int ioAdd( IO *io );
	int ioIns( IO *io, int pos );
	void ioDel( int pos );
	void ioMove( int pos, int to );

	virtual void calc( TValFunc *val )	{ }

	virtual void valAtt( TValFunc *vfnc );
	virtual void valDet( TValFunc *vfnc );

	virtual void preIOCfgChange( );
	virtual void postIOCfgChange( );

	TVariant objFuncCall( const string &id, vector<TVariant> &prms, const string &user );

    protected:
	//Methods
	const char *nodeName( ) const	{ return mId.c_str(); }
	void cntrCmdProc( XMLNode *opt );	//Control interface command process

	void preDisable( int flag );

	//Attributes
	string		mId,
			mStor;			//Link to storage
	bool		runSt, beStart;
	TValFunc	*mTVal;
	vector<TValFunc*> used;
	const char	*grp;

    private:
	//Attributes
	ResRW		mFRes;
	vector<IO*>	mIO;
};

//*************************************************
//* TValFunc                                      *
//*************************************************
class TValFunc
{
    public:
	//Methods
	TValFunc( const string &iname = "", TFunction *ifunc = NULL, bool iblk = true, const string &iuser = "root" );
	virtual ~TValFunc( );

	bool blk( )				{ return mBlk; }
	bool mdfChk( )				{ return mMdfChk; }
	bool isChangedProg( bool clear = false ) { bool mPrgCh_ = mPrgCh; if(clear) mPrgCh = false; return mPrgCh_; }

	string user( )				{ return mUser; }
	string lang( )				{ return mLang; }
	const string &vfName( )			{ return mName; }

	void setUser( const string &vl )	{ mUser = vl; }
	void setLang( const string &vl )	{ mLang = vl; }
	void setVfName( const string &inm )	{ mName = inm; }

	void setMdfChk( bool set );

	void ioList( vector<string> &list );
	int  ioId( const string &id );
	int  ioSize( ) const;
	IO::Type ioType( unsigned id ) {
	    if(id >= mVal.size()) throw TError("ValFunc", _("%s: Error with ID or IO %d!"), "ioType()", id);
	    return mFunc->io(id)->type();
	}
	unsigned ioFlg( unsigned id ) const {
	    if(!mFunc) throw TError("ValFunc", _("%s: No function set!"),"ioFlg()", id);
	    if(id >= mVal.size()) throw TError("ValFunc", _("%s: Error with ID or IO %d for the function '%s'!"), "ioFlg()", id, mFunc->nodePath().c_str());
	    return mFunc->io(id)->flg();
	}
	bool ioHide( unsigned id ) {
	    if(id >= mVal.size()) throw TError("ValFunc", _("%s: Error with ID or IO %d!"), "ioHide()", id);
	    return mFunc->io(id)->hide();
	}
	bool ioMdf( unsigned id ) {
	    if(id >= mVal.size()) throw TError("ValFunc", _("%s: Error with ID or IO %d!"), "ioMdf()", id);
	    return mVal[id].mdf;
	}

	// Get IO value
	TVariant get( unsigned id );
	string	getS( unsigned id );
	int64_t	getI( unsigned id );
	double	getR( unsigned id );
	char	getB( unsigned id );
	AutoHD<TVarObj>	getO( unsigned id );

	// Set IO value
	void set( unsigned id, const TVariant &val );
	void setS( unsigned id, const string &val );
	void setI( unsigned id, int64_t val );
	void setR( unsigned id, double val );
	void setB( unsigned id, char val );
	void setO( unsigned id, AutoHD<TVarObj> val );

	// Calc function
	virtual void calc( const string &user = "" );

	// Attached function
	TFunction *func( )		{ return mFunc; }
	void setFunc( TFunction *func, bool att_det = true );

	virtual void preIOCfgChange( );
	virtual void postIOCfgChange( );
	virtual void progChange( )	{ mPrgCh = true; }

	// Context operations
	TValFunc *ctxGet( int key );
	void ctxSet( int key, TValFunc *val );
	void ctxClear( );

	//Attributes
	void	*exCtx;

    protected:
	//Data
	struct SVl {
	    unsigned short	tp	: 4;
	    unsigned short	mdf	: 1;
	    union {
		string	*s;
		double	r;
		int64_t	i;
		char	b;
		AutoHD<TVarObj>	*o;
	    } val;
	};

	//Attributes
	vector<SVl>	mVal;	//pointer to: string, int, double, bool

    private:
	//Methods
	void funcDisConnect( bool det = true );

	//Attributes
	string	mName,			//Value name
		mUser,			//Link to user
		mLang;			//Force language
	unsigned short	mBlk	:1;	//Blocked values screen
	unsigned short	mMdfChk	:1;	//Checking for the IO modification
	unsigned short	mPrgCh	:1;	//Changing the program text

	TFunction	*mFunc;
	map<int, TValFunc* >	vctx;
	ResMtx mRes;
};

//***********************************************************
//* TFuncArgObj                                             *
//*   The function arguments access object.                 *
//***********************************************************
class TFuncArgsObj : public TVarObj
{
    public:
	//Methods
	TFuncArgsObj( TValFunc &ivf ) : vf(ivf)    { }

	string objName( )       { return "FuncArgs"; }

	TVariant propGet( const string &id );
	void propSet( const string &id, TVariant val );

	string getStrXML( const string &oid = "" );

    private:
	//Attributes
	TValFunc &vf;
};

}

#endif //TFUNCTIONS_H
