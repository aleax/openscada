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

#ifndef MAIL_H
#define MAIL_H

#include <ttransports.h>

class TMailIn;

class TMailIn: public TTransportIn
{
    public:
	TMailIn(string name, TTipTransport *owner);
	~TMailIn();
};

class TMailOut: public TTransportOut
{
    public:
    	TMailOut(string name, TTipTransport *owner);
	~TMailOut();
};

class TTransMail: public TTipTransport
{
    public:
	TTransMail( string name );
	~TTransMail();
	
	void modLoad( );
        
	TTransportIn  *In( const string &name );
	TTransportOut *Out( const string &name );	    
	
    private:	
	string optDescr( );
};

#endif //MAIL_H

