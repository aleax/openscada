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

#include <tcntrnode.h>

using std::string;
using std::vector;

#define IO_STR	0x01
#define IO_INT	0x02
#define IO_REAL	0x04
#define IO_BOOL	0x08
#define	IO_OUT	0x10
#define	IO_RET	0x20
#define	IO_VECT	0x40
#define IO_HIDE 0x80


//Function abstract object
class TFunction : public TCntrNode
{
    friend class TValFunc;
    public:
    	class IO
	{
	    public:
		IO( const string &iid, const string &iname, char itype, const string &idef="", const string &ivect="" );
	    	string 	id;
	    	string 	name;
	    	char   	type;
		string  def;
	    	string	vector;
	};
		
	TFunction( const string &iid );
	virtual ~TFunction();
	
	string &id(){ return m_id; };
	virtual string name() = 0;
	virtual string descr() = 0;
	
	void ioList( vector<string> &list );
	int ioId( const string &id );	
	IO *io( int id );
	char ioType( int id );
	
	virtual void calc( TValFunc *val ) = 0;
	
    protected:
	string nodeName(){ return id(); }
	//================== Controll functions ========================
	void ctrStat_( XMLNode *inf );
	void ctrDinGet_( const string &a_path, XMLNode *opt );
	void ctrDinSet_( const string &a_path, XMLNode *opt );
	
	void ioAdd( IO *io );
	
    private:
	TValFunc	*m_tval;
	string		m_id;
	vector<IO*>	m_io;
};


class TValFunc
{
    public:
	TValFunc( TFunction *ifunc );
    	virtual ~TValFunc( );
	
	void 	ioList( vector<string> &list );
	int 	ioId( const string &id );	//IO id
	//IO type
	char 	type( unsigned id )
	{
	    if( id >= m_val.size() || m_val[id].tp == 0 )
		throw TError("Id or IO %d error!",id);
	    return m_func->ioType(id);
        }
	
	//get IO value
	string 	getS( unsigned id )
	{
	    if( id >= m_val.size() || !(m_val[id].tp&IO_STR) )
		throw TError("Id or IO %d error!",id);
	    return *(string *)m_val[id].vl;
	}	    
	int getI( unsigned id )
	{
	    if( id >= m_val.size() || !(m_val[id].tp&IO_INT) )
		throw TError("Id or IO %d error!",id);
    	    return *(int *)m_val[id].vl;
	}	
	double getR( unsigned id )
	{
	    if( id >= m_val.size() || !(m_val[id].tp&IO_REAL) ) 
		throw TError("Id or IO %d error!",id);
	    return *(double *)m_val[id].vl;               
		
        }
	bool getB( unsigned id )
	{
	    if( id >= m_val.size() || !(m_val[id].tp&IO_BOOL) )
		throw TError("Id or IO %d error!",id);
	    return *(bool *)m_val[id].vl;
	}
	
	//set IO value
	void setS( unsigned id, const string &val )
	{
	    if( id >= m_val.size() || !(m_val[id].tp&IO_STR) )
		throw TError("Id or IO %d error!",id);
	    *(string *)m_val[id].vl = val;
	}
	void setI( unsigned id, int val )
	{
            if( id >= m_val.size() || !(m_val[id].tp&IO_INT) )
		throw TError("Id or IO %d error!",id);
	    *(int *)m_val[id].vl = val;
	}
	void setR( unsigned id, double val )
	{
	    if( id >= m_val.size() || !(m_val[id].tp&IO_REAL) ) 
		throw TError("Id or IO %d error!",id);
    	    *(double *)m_val[id].vl = val;
	}
	void setB( unsigned id, bool val )
	{
	    if( id >= m_val.size() || !(m_val[id].tp&IO_BOOL) )
		throw TError("Id or IO %d error!",id);
	    *(int *)m_val[id].vl = val;
        }

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
            char tp;
	    void *vl;
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
	
	void list( vector<string> &ls )	{ chldList(m_fnc,ls); }
	AutoHD<TFunction> at( const string &id ) 
	{ return chldAt(m_fnc,id); }

    protected:
	void reg( TFunction *fnc )		{ chldAdd(m_fnc,fnc); }
	void unreg( const string &id )	{ chldDel(m_fnc,id); } 

	string nodeName(){ return id(); }
	//================== Controll functions ========================
	void ctrStat_( XMLNode *inf );
	void ctrDinGet_( const string &a_path, XMLNode *opt );
	void ctrDinSet_( const string &a_path, XMLNode *opt );
	AutoHD<TCntrNode> ctrAt1( const string &br );
	
    private:
	string	m_id;	
	int	m_fnc;
};

//List of function libraries
class TKernel;

class TFunctionS : public TCntrNode
{
    public:
	TFunctionS(TKernel *app);
	~TFunctionS();
	
	string name();

	void list( vector<string> &ls )	{ chldList(m_lb,ls); }
	bool avoid( const string &id )  { return chldAvoid(m_lb,id); }
        void reg( TLibFunc *lib )	{ chldAdd(m_lb,lib); }
	void unreg( const string &id )	{ chldDel(m_lb,id); }
	AutoHD<TLibFunc> at( const string &id )	
	{ return chldAt(m_lb,id); }    
	
	//================== Controll functions ========================
	void ctrStat_( XMLNode *inf );
	void ctrDinGet_( const string &a_path, XMLNode *opt );
	void ctrDinSet_( const string &a_path, XMLNode *opt );
	AutoHD<TCntrNode> ctrAt1( const string &br );
	
	TKernel &owner() const { return(*m_owner); }
	
    private:
	int	m_lb;
	
	TKernel	*m_owner;	
};

#endif //TFUNCTIONS_H

