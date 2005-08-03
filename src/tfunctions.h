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

#ifndef TFUNCTIONS_H
#define TFUNCTIONS_H

#include <string>
#include <vector>

#include "tsys.h"
#include "tcntrnode.h"

using std::string;
using std::vector;

class IO
{
    public:		
	enum Type { String, Integer, Real, Boolean, Vector };
	enum Mode { Input, Output, Return };
		
	IO( const char *iid, const char *iname, IO::Type itype, IO::Mode imode, const char *idef = "", bool ihide = false, const char *ivect = "" );

	const string &id() 	{ return m_id; }
	const string &name() 	{ return m_name; }
	const Type &type()	{ return m_type; }
	const Mode &mode()	{ return m_mode; }		
	const string &def() 	{ return m_def; }
	const string &vector() 	{ return m_vect; }
	bool  hide() 		{ return m_hide; }

	void id( const string &val ) 	{ m_id = val; }
	void name( const string &val ) 	{ m_name = val; }
	void type( Type val ) 	{ m_type = val; }
	void mode( Mode val ) 	{ m_mode = val; }
	void def( const string &val ) 	{ m_def = val; }
	void vector( const string &val ){ m_vect = val; }
	void hide( bool val )	{ m_hide = val; }

    private:
	string 	m_id;
	string 	m_name;
	Type	m_type;
	Mode	m_mode;
	string  m_def;
	string	m_vect;
	bool	m_hide;
};

//Function abstract object
class TValFunc;

class TFunction : public TCntrNode
{
    public:		
	TFunction( const string &iid );
	virtual ~TFunction();
	
	bool startStat() { return run_st; }		 
	virtual void start( bool val ) 	{ run_st = val; }
	
	string &id(){ return m_id; };
	virtual string name() = 0;
	virtual string descr() = 0;
	
	void ioList( vector<string> &list );
	int ioId( const string &id );
	int ioSize();
	IO *io( int id );
	
	virtual void calc( TValFunc *val ) = 0;
	
    protected:
	string nodeName(){ return id(); }
	//================== Controll functions ========================
	void cntrCmd_( const string &a_path, XMLNode *opt, int cmd );
	
	void ioAdd( IO *io );
	void ioIns( IO *io, int pos );
	void ioDel( int pos );
	void ioMove( int pos, int to );		
	
    protected:
	bool            run_st;	
	string          m_id;
	
    private:	
	TValFunc	*m_tval;
	vector<IO*>	m_io;
};


class TValFunc
{
    public:
	TValFunc( TFunction *ifunc = NULL );
    	virtual ~TValFunc( );
	
	void 	ioList( vector<string> &list );
	int 	ioId( const string &id );	//IO id
	IO::Type ioType( unsigned id )
	{
	    if( id >= m_val.size() )    throw TError("Id or IO %d error!",id);
    	    return m_func->io(id)->type();
	}
	IO::Mode ioMode( unsigned id )
	{    	
	    if( id >= m_val.size() )    throw TError("Id or IO %d error!",id);
	    return m_func->io(id)->mode();
	}	    
	bool ioHide( unsigned id )
	{
	    if( id >= m_val.size() )    throw TError("Id or IO %d error!",id);
    	    return m_func->io(id)->hide();	
	}
	
	//get IO value
	string 	getS( unsigned id );
	int getI( unsigned id );
	double getR( unsigned id );
	bool getB( unsigned id );
	
	//set IO value
	void setS( unsigned id, const string &val );
	void setI( unsigned id, int val );
	void setR( unsigned id, double val );
	void setB( unsigned id, bool val );

	//Dimension controll	
	bool	dimens(){ return m_dimens; }
	void	dimens( bool set ){ m_dimens = set; }
 	
	//Calc function
	virtual void calc( );
	//Calc time function
	double  calcTm( ){ return tm_calc; }
	
	//Attached function
	TFunction *func( ){ return m_func; }
	void func( TFunction *ifunc );

    protected:
	struct SVl
        {
	    IO::Type 	tp;
	    void 	*vl;
	};
				
	vector<SVl>     m_val;          //pointer to: string, int, double, bool								

    private:
	void funcDisConnect( );
		
    private:
	bool            m_dimens;	//make dimension of the calc time
	double		tm_calc;	//calc time in mikroseconds
	
	TFunction 		*m_func;	
};

//Function library abstract object
class TLibFunc : public TCntrNode
{
    public:
	TLibFunc( const string &iid );
	virtual ~TLibFunc( );

	string &id(){ return m_id; };
	virtual string name() = 0;
	virtual string descr() = 0;
	
	bool startStat( ) 	{ return run_st; }
	virtual void start( bool val );
	
	void list( vector<string> &ls )	{ chldList(m_fnc,ls); }
	bool avoid( const string &id )  { return chldAvoid(m_fnc,id); }
	AutoHD<TFunction> at( const string &id ) 
	{ return chldAt(m_fnc,id); }

    protected:
	void reg( TFunction *fnc )	{ chldAdd(m_fnc,fnc); }
	void unreg( const string &id )	{ chldDel(m_fnc,id); } 

	string nodeName(){ return id(); }
	//================== Controll functions ========================
	void cntrCmd_( const string &a_path, XMLNode *opt, int cmd );
	AutoHD<TCntrNode> ctrAt1( const string &br );
	
    protected:
	int     m_fnc;
    	
    private:
	bool	run_st;
	string	m_id;	
};

//List of function libraries
class TKernel;

class TFunctionS : public TCntrNode
{
    public:
	TFunctionS(TKernel *app);
	~TFunctionS();

	string id(){ return "func"; }	
	string name();
	
	void start( bool val );

	void list( vector<string> &ls )	{ chldList(m_lb,ls); }
	bool avoid( const string &id )  { return chldAvoid(m_lb,id); }
        void reg( TLibFunc *lib )	{ chldAdd(m_lb,lib); }
	void unreg( const string &id )	{ chldDel(m_lb,id); }
	AutoHD<TLibFunc> at( const string &id )	
	{ return chldAt(m_lb,id); }    
	
    protected:
	//================== Controll functions ========================
	void cntrCmd_( const string &a_path, XMLNode *opt, int cmd );
	AutoHD<TCntrNode> ctrAt1( const string &br );
	
	TKernel &owner() const { return(*m_owner); }
	
    private:
	bool    run_st;
	int	m_lb;
	
	TKernel	*m_owner;	
};

#endif //TFUNCTIONS_H

