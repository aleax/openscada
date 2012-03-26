
//!!! Module name, file name and module's license. Change for your need.
//OpenSCADA system module Transport.Tmpl file: module.h
/***************************************************************************
 *   Copyright (C) 2012 by MyName MyFamily                                 *
 *   my@email.org                                                          *
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

//!!! Multi-including this header file prevent. Change for your include file name
#ifndef MODULE_H
#define MODULE_H

//!!! System's includings. Add need for your module includings.
#include <pthread.h>

//!!! OpenSCADA module's API includings. Add need for your module includings.
#include <ttransports.h>

//!!! Individual module's translation function define. Not change it!
#undef _
#define _(mess) mod->I18N(mess)

using namespace OSCADA;

//!!! All module's object's include into self (individual) namespace. Change namespace for your module.
namespace ModTmpl
{

//!!! Input transport realisation object define. Add methods and attributes for your need.
//************************************************
//* ModTmpl::TTrIn				 *
//************************************************
class TTrIn: public TTransportIn
{
    public:
	//!!! Constructor for input transport object.
	TTrIn( string name, const string &idb, TElem *el );
	//!!! Destructor for input transport object.
	~TTrIn( );

	//!!! The inherited (virtual) status interface function
	string getStatus( );

	//!!! The inherited (virtual) start and stop interface functions
	void start( );
	void stop( );

    private:
	//Methods
	//!!! Thread's function for process input connections
	static void *Task( void * );

	//!!! OpenSCADA control interface comands process virtual function.
	void cntrCmdProc( XMLNode *opt );	//Control interface command process

	//Attributes
	//!!! Thread's attributes
	bool	endrun;				// Command for stop task

	//!!! Status interface atributes
	float	trIn, trOut;			// Traffic in and out counter
	int	connNumb;			// Connections number
};

//!!! Output transport realisation object define. Add methods and attributes for your need.
//************************************************
//* ModTmpl::TTrOut				 *
//************************************************
class TTrOut: public TTransportOut
{
    public:
	//!!! Constructor for output transport object.
	TTrOut( string name, const string &idb, TElem *el );
	//!!! Destructor for output transport object.
	~TTrOut( );

	//!!! The inherited (virtual) status interface function
	string getStatus( );

	//!!! The inherited (virtual) start and stop interface functions
	void start( );
	void stop( );

	//!!! The inherited (virtual) sending and receiving data interface function
	int messIO( const char *obuf, int len_ob, char *ibuf = NULL, int len_ib = 0, int time = 0, bool noRes = false );

    private:
	//Methods
	//!!! OpenSCADA control interface comands process virtual function.
	void cntrCmdProc( XMLNode *opt );	//Control interface command process

	//Attributes
	//!!! Status interface atributes
	float	trIn, trOut;	// Traffic in and out counter
};

//!!! Root module's object define. Add methods and attributes for your need.
//************************************************
//* ModTmpl::TTr				 *
//************************************************
class TTr: public TTipTransport
{
    public:
	//!!! Constructor for root module's object.
	TTr( string name );
	//!!! Destructor for root module's object.
	~TTr( );

	//!!! Main subsystem API functions for self modules input and output transport objects creation.
	TTransportIn  *In( const string &name, const string &idb );
	TTransportOut *Out( const string &name, const string &idb );

    protected:
	//!!! Inherited (virtual) load and save object's node methods. Call from OpenSCADA kernel.
	void load_( );
	void save_( );

    private:
	//Methods
	//!!! Module's comandline options for print help function.
	string optDescr( );
	//!!! OpenSCADA control interface comands process virtual function.
	void cntrCmdProc( XMLNode *opt );		//Control interface command process
};

extern TTr *mod;
}

#endif //MODULE_H
