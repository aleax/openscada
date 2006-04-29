
//OpenSCADA system module Transport.Mail file: mail.h
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

#ifndef MAIL_H
#define MAIL_H

#include <ttransports.h>

class TMailIn;

class TMailIn: public TTransportIn
{
    public:
	TMailIn(string name,const string &idb,TElem *el);
	~TMailIn();
};

class TMailOut: public TTransportOut
{
    public:
    	TMailOut(string name,const string &idb,TElem *el);
	~TMailOut();
};

class TTransMail: public TTipTransport
{
    public:
	TTransMail( string name );
	~TTransMail();
	
	void modLoad( );
        
	TTransportIn  *In( const string &name, const string &idb );
	TTransportOut *Out( const string &name, const string &idb );	    
	
    private:	
	string optDescr( );
};

#endif //MAIL_H

