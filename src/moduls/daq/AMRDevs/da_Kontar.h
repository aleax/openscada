
//OpenSCADA system module DAQ.AMRDevs file: da_Kontar.h
/***************************************************************************
 *   Copyright (C) 2014 by Roman Savochenko                                *
 *   rom_as@oscada.org, rom_as@fromru.com                                  *
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

#ifndef DA_KONTAR_H
#define DA_KONTAR_H

#include "ttiparam.h"
#include "mod_amr.h"

#define MaxLenReq 1024

#define ROTL32(X,C) (((X)<<(C))|((X)>>(32-(C))))
#define ROTR32(X,C) (((X)>>(C))|((X)<<(32-(C))))

namespace AMRDevs
{

//*************************************************
//* Kontar                                        *
//*************************************************
class Kontar: public TTipParam
{
    public:
	//Data
	//* SMemBlk - memory block of variables
	class SMemBlk
	{
	    public:
		SMemBlk( int ioff, int v_rez ) : off(ioff)
		{ val.assign(v_rez,0); err = _("11:Value not gathered."); }

		int	off;		//Data block start offset
		string	val,		//Data block values kadr
			err;		//Acquisition error text
	};
	//* tval - The parameter specific values object
	class tval
	{
	    public:
		//Methods
		tval( ) : lstClcSet(0)	{ }

		// RC5 encoding
		string RC5Encr( const string &src, const string &key );
		string RC5Decr( const string &src, const string &key );
		string RC5Key( const string &key );

		//Atributes
		XMLNode	cfg;
		string	prevTr;
		string	pass, key;
		vector<SMemBlk> mBlks;	//Acquisition memory blocks for values
		time_t	lstClcSet;
	};

	//Methods
	Kontar( );
	~Kontar( );

	string DB( TController *cntr )	{ return mod->modId()+"KontarPrm_"+cntr->id(); }

	void create( TParamContr *prm );
	void destroy( TParamContr *prm );

	void enable( TParamContr *prm );
	//void disable( TParamContr *prm );

	bool cfgChange( TParamContr *prm, TCfg &cfg );

	void getVals( TParamContr *prm );
	void vlSet( TParamContr *prm, TVal &vo, const TVariant &vl, const TVariant &pvl );

	bool cntrCmdProc( TParamContr *prm, XMLNode *opt );

    private:
	//Methods
	void regVal( TMdPrm *prm, int off, int sz );	//Register value for acquisition

	string req( TMdPrm *prm, string &pdu, bool passUpdate = false );
	const char *getVal( TMdPrm *prm, int off, int dtSz );
};

} //End namespace

#endif //DA_KONTAR_H
