
//OpenSCADA system file: tfunction.h
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

#ifndef TFUNCTIONS_H
#define TFUNCTIONS_H

#include <string>
#include <vector>

#include "tmess.h"
#include "tcntrnode.h"

using std::string;
using std::vector;

class TFunction;

class IO
{
    friend class TFunction;
    public:		
	enum Type { String, Integer, Real, Boolean };
	enum IOFlgs 
	{ 
	    Default = 0x00,	//Default mode (Input IO)
	    Output  = 0x01,
	    Return  = 0x02
	};
		
	IO( const char *iid, const char *iname, IO::Type itype, unsigned iflgs, const char *idef = "", 
		bool ihide = false, const char *irez = "" );

	IO &operator=(IO &iio);

	const string &id() 	{ return m_id; }
	const string &name() 	{ return m_name; }
	const Type &type()	{ return m_type; }
	unsigned flg()		{ return m_flg; }		
	const string &def() 	{ return m_def; }
	bool  hide() 		{ return m_hide; }
	const string &rez()	{ return m_rez; }

	void setId( const string &val );
	void setName( const string &val );
	void setType( Type val );
	void setFlg( unsigned val );
	void setDef( const string &val );
	void setHide( bool val );
	void setRez( const string &val );

    private:
	string 	m_id;
	string 	m_name;
	Type	m_type;
	unsigned m_flg;
	string  m_def;
	bool	m_hide;
	string	m_rez;
	
	TFunction *owner;
};

//Function abstract object
class TValFunc;

class TFunction : public TCntrNode
{
    public:
	//Methods		
	TFunction( const string &iid );
	virtual ~TFunction();
	
	TFunction &operator=(TFunction &func);
	
	const string &id()	{ return m_id; };
	virtual string name()	{ return ""; }
	virtual string descr()	{ return ""; }
	
	bool startStat()        { return run_st; }
	virtual void start( bool val )  { run_st = val; }
	
	//- IO -
	void ioList( vector<string> &list );
	int ioId( const string &id );
	int ioSize();
	IO *io( int id );
	void ioAdd( IO *io );
	void ioIns( IO *io, int pos );
	void ioDel( int pos );
	void ioMove( int pos, int to );
	
	virtual void calc( TValFunc *val )	{ }
	
	void valAtt( TValFunc *vfnc );
        void valDet( TValFunc *vfnc );		
	
	virtual void preIOCfgChange();
        virtual void postIOCfgChange();
	
    protected:
	//Methods
	void cntrCmdProc( XMLNode *opt );       //Control interface command process
	
	void preDisable(int flag);
	
	//Attributes
	string          m_id;
	bool            run_st;	
	TValFunc        *m_tval;

    private:
	//Methods
	string nodeName()       { return id(); }
    
	//Attributes	
	vector<IO*>	m_io;
	vector<TValFunc*>	used;
};


class TValFunc
{
    public:
	//Methods
	TValFunc( const string &iname = "", TFunction *ifunc = NULL, bool iblk = true );
    	virtual ~TValFunc( );
	
	const string &vfName()	{ return m_name; }
    	void setVfName( const string &inm )	{ m_name = inm; }
	
	void 	ioList( vector<string> &list );	
	int 	ioId( const string &id );	//IO id
	int 	ioSize( );
	IO::Type ioType( unsigned id )
	{
	    if( id >= m_val.size() )    throw TError("ValFunc",_("Id or IO %d error!"),id);
    	    return m_func->io(id)->type();
	}
	unsigned ioFlg( unsigned id )
	{    	
	    if( id >= m_val.size() )    throw TError("ValFunc",_("Id or IO %d error!"),id);
	    return m_func->io(id)->flg();
	}	    
	bool ioHide( unsigned id )
	{
	    if( id >= m_val.size() )    throw TError("ValFunc",_("Id or IO %d error!"),id);
    	    return m_func->io(id)->hide();	
	}
	
	//- get IO value -
	string getS( unsigned id );
	int getI( unsigned id );
	double getR( unsigned id );
	bool getB( unsigned id );
	
	//- set IO value -
	void setS( unsigned id, const string &val );
	void setI( unsigned id, int val );
	void setR( unsigned id, double val );
	void setB( unsigned id, bool val );

	//- Blocked values screen -
	bool	blk()			{ return m_blk; }

	//- Dimension controll -
	bool	dimens()		{ return m_dimens; }
	void	dimens( bool set )	{ m_dimens = set; }
 	
	//- Calc function -
	virtual void calc( );
	//- Calc time function -
	double  calcTm( )		{ return tm_calc; }
	void calcTm( double ivl )	{ tm_calc = ivl; }
	
	//- Attached function -
	TFunction *func( )		{ return m_func; }
	void func( TFunction *ifunc, bool att_det = true );
	
	virtual void preIOCfgChange();
        virtual void postIOCfgChange();

    protected:
	//Data
	struct SVl
        {
	    IO::Type 	tp;
	    union
	    {
	        string *s;
	        double r;
	        int    i;
	        bool   b;
	    }val;										    
	};
	
	//Attributes			
	vector<SVl>     m_val;          //pointer to: string, int, double, bool								

    private:
	//Methods
	void funcDisConnect( bool det = true );
	
	//Attributes	
	string	m_name;		//Value name
	bool  	m_blk,		//Blocked values screen
		m_dimens;	//Make dimension of the calc time
	
	double tm_calc;		//Calc time in mikroseconds
	
	TFunction	*m_func;
};

#endif //TFUNCTIONS_H

