/***************************************************************************
 *   Copyright (C) 2005 by Roman Savochenko                                *
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

using std::string;
using std::vector;

class TFunction;

class IO
{
    friend class TFunction;
    public:		
	enum Type { String, Integer, Real, Boolean, Vector };
	enum Mode { Input, Output, Return };
		
	IO( const char *iid, const char *iname, IO::Type itype, IO::Mode imode, 
	    const char *idef = "", bool ihide = false, const char *ivect = "" );

	const string &id() 	{ return m_id; }
	const string &name() 	{ return m_name; }
	const Type &type()	{ return m_type; }
	const Mode &mode()	{ return m_mode; }		
	const string &def() 	{ return m_def; }
	const string &vector() 	{ return m_vect; }
	bool  hide() 		{ return m_hide; }

	void id( const string &val );
	void name( const string &val );
	void type( Type val );
	void mode( Mode val );
	void def( const string &val );
	void vector( const string &val );
	void hide( bool val );

    private:
	string 	m_id;
	string 	m_name;
	Type	m_type;
	Mode	m_mode;
	string  m_def;
	string	m_vect;
	bool	m_hide;
	
	TFunction *owner;
};

//Function abstract object
class TValFunc;

class TFunction : public TCntrNode
{
    public:		
	TFunction( const string &iid );
	virtual ~TFunction();
	
	string &id()		{ return m_id; };
	virtual string name() = 0;
	virtual string descr() = 0;
	
	bool startStat()        { return run_st; }
	virtual void start( bool val )  { run_st = val; }
	
	void ioList( vector<string> &list );
	int ioId( const string &id );
	int ioSize();
	IO *io( int id );
	
	virtual void calc( TValFunc *val ) = 0;
	
	void valAtt( TValFunc *vfnc );
        void valDet( TValFunc *vfnc );		
	
	virtual void preIOCfgChange();
        virtual void postIOCfgChange();
	
    protected:
	//================== Controll functions ========================
	void cntrCmd_( const string &a_path, XMLNode *opt, TCntrNode::Command cmd );
	
	void ioAdd( IO *io );
	void ioIns( IO *io, int pos );
	void ioDel( int pos );
	void ioMove( int pos, int to );
	
	void preDisable(int flag);
	
    protected:
	string          m_id;
	bool            run_st;	
	TValFunc        *m_tval;

    private:
	string nodeName()       { return id(); }
	string nodeType()       { return "TFunction"; }
    	
    private:	
	vector<IO*>	m_io;
	vector<TValFunc*>	used;
};


class TValFunc
{
    public:
	TValFunc( const string &iname = "", TFunction *ifunc = NULL, bool iblk = true );
    	virtual ~TValFunc( );
	
	const string &name()	{ return m_name; }
    	void name( const char &inm )	{ m_name = inm; }
	
	void 	ioList( vector<string> &list );	
	int 	ioId( const string &id );	//IO id
	int 	ioSize( );
	IO::Type ioType( unsigned id )
	{
	    if( id >= m_val.size() )    throw TError("ValFunc",Mess->I18N("Id or IO %d error!"),id);
    	    return m_func->io(id)->type();
	}
	IO::Mode ioMode( unsigned id )
	{    	
	    if( id >= m_val.size() )    throw TError("ValFunc",Mess->I18N("Id or IO %d error!"),id);
	    return m_func->io(id)->mode();
	}	    
	bool ioHide( unsigned id )
	{
	    if( id >= m_val.size() )    throw TError("ValFunc",Mess->I18N("Id or IO %d error!"),id);
    	    return m_func->io(id)->hide();	
	}
	
	//get IO value
	string getS( unsigned id );
	int getI( unsigned id );
	double getR( unsigned id );
	bool getB( unsigned id );
	
	//set IO value
	void setS( unsigned id, const string &val );
	void setI( unsigned id, int val );
	void setR( unsigned id, double val );
	void setB( unsigned id, bool val );

	//Blocked values screen
	bool	blk()			{ return m_blk; }

	//Dimension controll	
	bool	dimens()		{ return m_dimens; }
	void	dimens( bool set )	{ m_dimens = set; }
 	
	//Calc function
	virtual void calc( );
	//Calc time function
	double  calcTm( )		{ return tm_calc; }
	
	//Attached function
	TFunction *func( )		{ return m_func; }
	void func( TFunction *ifunc, bool att_det = true );
	
	virtual void preIOCfgChange();
        virtual void postIOCfgChange();

    protected:
	struct SVl
        {
	    IO::Type 	tp;
	    void 	*vl;
	};
				
	vector<SVl>     m_val;          //pointer to: string, int, double, bool								

    private:
	void funcDisConnect( bool det = true );
		
    private:
	string	m_name;		//Value name
	bool  	m_blk,		//Blocked values screen
		m_dimens;	//Make dimension of the calc time
	double	tm_calc;	//Calc time in mikroseconds
	
	TFunction	*m_func;
};

#endif //TFUNCTIONS_H

