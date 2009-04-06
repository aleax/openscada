
//OpenSCADA system module Transport.Serial file: mod_serial.h
/***************************************************************************
 *   Copyright (C) 2009 by Roman Savochenko                                *
 *   rom_as@fromru.com                                                     *
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

#ifndef MOD_SERIAL_H
#define MOD_SERIAL_H

#include <pthread.h>

#include <ttransports.h>

#undef _
#define _(mess) mod->I18N(mess)

namespace Serial
{

//************************************************
//* Serial::TTrIn				 *
//************************************************
class TTrIn: public TTransportIn
{
    public:
	TTrIn( string name, const string &idb, TElem *el );
	~TTrIn( );

	string getStatus( );

	string timings( )			{ return mTimings; }

	void setTimings( const string &vl )	{ mTimings = vl; modif(); }

	void start( );
	void stop( );

    private:
	//Methods
	static void *Task( void * );

	void cntrCmdProc( XMLNode *opt );	//Control interface command process

	//Attributes
	string		&mTimings;
	int		fd;

	pthread_t	pthr_tsk;
	bool		endrun;			// Command for stop task

	float		trIn, trOut;		// Traffic in and out counter
};

//************************************************
//* Serial::TTrOut				 *
//************************************************
class TTrOut: public TTransportOut
{
    public:
	TTrOut( string name, const string &idb, TElem *el );
	~TTrOut( );

	string getStatus( );

	string timings( )			{ return mTimings; }

	void setAddr( const string &addr );
	void setTimings( const string &vl )	{ mTimings = vl; modif(); }

	void start( );
	void stop( );

	int messIO( const char *obuf, int len_ob, char *ibuf = NULL, int len_ib = 0, int time = 0 );

    private:
	//Methods
	void cntrCmdProc( XMLNode *opt );	//Control interface command process

	//Attributes
	string	&mTimings;
	int	fd;
	long long mLstReqTm;

	float	trIn, trOut;	// Traffic in and out counter
};

//************************************************
//* Serial::TTr					*
//************************************************
class TTr: public TTipTransport
{
    public:
	TTr( string name );
	~TTr( );

	TTransportIn  *In( const string &name, const string &idb );
	TTransportOut *Out( const string &name, const string &idb );

    protected:
	void load_( );

    private:
	//Methods
	string optDescr( );
	void cntrCmdProc( XMLNode *opt );		//Control interface command process

	void postEnable( int flag );
};

extern TTr *mod;
}

#endif //MOD_SERIAL_H
