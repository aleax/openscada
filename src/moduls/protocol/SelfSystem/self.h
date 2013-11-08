
//OpenSCADA system module Protocol.SelfSystem file: self.h
/***************************************************************************
 *   Copyright (C) 2007-2010 by Roman Savochenko                           *
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

#ifndef SELF_H
#define SELF_H

#include <tprotocols.h>

#undef _
#define _(mess) mod->I18N(mess)

using namespace OSCADA;

namespace SelfPr
{

//*************************************************
//* TProtIn                                       *
//*************************************************
class TProtIn: public TProtocolIn
{
    public:
	//Methods
	TProtIn( string name );
	~TProtIn( );

	bool mess( const string &request, string &answer );

    private:
	//Attributes
	bool m_nofull;
	string req_buf;
};

//*************************************************
//* TProt                                         *
//*************************************************
class TProt: public TProtocol
{
    public:
	//Data
	//*****************************************
	//* SAuth                                 *
	class SAuth
	{
	    public:
		//Methods
		SAuth(time_t itm, string inm, int ises) :
		    t_auth(itm), name(inm), id_ses(ises) { }
		//Attributes
		time_t t_auth;
		string name;
		int    id_ses;
	};

	//Methods
	TProt( string name );
	~TProt();

	int authTime( )			{ return m_t_auth; }
	int comprLev( )			{ return mComprLev; }
	int comprBrd( )			{ return mComprBrd; }

	void setAuthTime( int vl )	{ m_t_auth = vmax(1,vl); modif(); }
	void setComprLev( int vl )	{ mComprLev = vmax(-1,vmin(9,vl)); modif(); }
	void setComprBrd( int vl )	{ mComprBrd = vmax(10,vl); modif(); }

	int sesOpen( const char *user, const char *pass );
	void sesClose( int id_ses );
	SAuth sesGet( int id_ses );

	void outMess( XMLNode &io, TTransportOut &tro );

    protected:
	void load_( );
	void save_( );

    private:
	//Methods
	void cntrCmdProc( XMLNode *opt );
	TProtocolIn *in_open( const string &name );

	//Attributes
	Res		ses_res;
	vector<SAuth>	auth_lst;
	int		m_t_auth, mComprLev, mComprBrd;
};

extern TProt *mod;
} //End namespace SelfPr

#endif //SELF_H
