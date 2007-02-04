
//OpenSCADA system module UI.QTCfg file: tuimod.h
/***************************************************************************
 *   Copyright (C) 2004-2006 by Roman Savochenko                           *
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

#ifndef TUIMOD_H
#define TUIMOD_H

#include <QMainWindow>

#include <tuis.h>
#include <telem.h>

#undef _
#define _(mess) mod->I18N(mess)

namespace QTCFG
{

class ConfApp;

class ExtHost
{
    public:
	//Methods
	ExtHost(const string &iuser_open, const string &iid, const string &iname, const string &itransp, 
		const string &iaddr, const string &iuser, const string &ipass) :
	    id(iid), user_open(iuser_open), name(iname), transp(itransp), addr(iaddr), 
	    user(iuser), pass(ipass), ses_id(-1), link_ok(false) { }
    
	//Attributes
	string	user_open;       //User has open remote host
	string	id;		//External host id
	string	name;		//Name
	string	transp;		//Connect transport
	string	addr;		//External host address
	string	user;		//External host user
	string	pass;		//External host password
	int	ses_id;		//Session ID
	bool	link_ok;	//Link OK 
};
    
class TUIMod: public TUI
{
    public:
	//Methods
	TUIMod( string name );
	~TUIMod();
	
	string startPath()	{ return start_path; }
	string startUser()	{ return start_user; }
	string extTranspBD();

	void modStart();
	void modStop();

	void postEnable( );
	void modLoad( );
	void modSave( );
	
	//- Module info attributes -    
        string modInfo( const string &name );
    	void   modInfo( vector<string> &list );
	
	//- External hosts methods -
	void extHostList(const string &user, vector<string> &list);
	bool extHostPresent(const string &user, const string &iid);
	void extHostSet(const ExtHost &host);
	void extHostDel(const string &user, const string &id);
	ExtHost extHostGet(const string &user, const string &id);
	
	//- Register window -
	void regWin( QMainWindow *win );
	void unregWin( QMainWindow *win );
    
    private:
	//Methods
	void cntrCmdProc( XMLNode *opt );       //Control interface command process
	QMainWindow *openWindow();    
	QIcon icon();
	
        string optDescr( );
	
	//Attributes
        vector<QMainWindow*> cfapp;	//Opened configurator opened window
	vector<ExtHost>	extHostLs;	//External hosts list	
	int 	extHostRes;		//External hosts resource id
	TElem	el_ext;
	
	string	start_user;		//No password requested start user
	string	start_path;		//Start path
};
    
extern TUIMod *mod;
}

#endif //TUIMOD_H

