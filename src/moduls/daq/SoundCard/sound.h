
//OpenSCADA system module DAQ.SoundCard file: sound.h
/***************************************************************************
 *   Copyright (C) 2008 by Roman Savochenko                                *
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
 
#ifndef SOUND_H
#define SOUND_H

#include <tmodule.h>
#include <tcontroller.h>
#include <ttipdaq.h>
//#include <tparamcontr.h>

#include <string>
#include <vector>

#undef _
#define _(mess) mod->I18N(mess)

using std::string;
using std::vector;

namespace SoundCard
{

//*************************************************
//* TMdPrm                                        *
//*************************************************
class TMdContr;

class TMdPrm : public TParamContr
{
    public:
	//Methods
	TMdPrm( string name, TTipParam *tp_prm );
	~TMdPrm( );

	void load_( );

	TMdContr &owner( )	{ return (TMdContr &)TParamContr::owner(); }

    protected:
	//Methods
	void vlArchMake( TVal &val );

	void postEnable( int flag );

    private:
	//Attributes
	string	&m_in;
	TElem	p_el;		//Work atribute elements
};

//*************************************************
//* TMdContr                                      *
//*************************************************
class TTpContr;

class TMdContr: public TController
{
    friend class TMdPrm;
    public:
	//Methods
	TMdContr( string name_c, const string &daq_db, ::TElem *cfgelem);
	~TMdContr( );

	TParamContr *ParamAttach( const string &name, int type );

	void load_( );
	void save_( );
	void start_( );
	void stop_( );

	TTpContr &owner( )	{ return (TTpContr&)TController::owner(); }

    private:
	//Methods
	static void *Task( void *param );

	//Attributes
	pthread_t procPthr;
	bool	prc_st, endrun_req;
	string	&m_card;
};

//*************************************************
//* TTpContr                                      *
//*************************************************
class TTpContr: public TTipDAQ
{
    public:
	//Methods
	TTpContr( string name );
	~TTpContr();

	void postEnable( int flag );

	TController *ContrAttach( const string &name, const string &daq_db );

    private:
	//Attributes
};

extern TTpContr *mod;

} //End namespace

#endif //SOUND_H
