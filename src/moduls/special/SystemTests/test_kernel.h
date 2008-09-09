
//OpenSCADA system module Special.SystemTests file: test_kernel.h
/***************************************************************************
 *   Copyright (C) 2003-2008 by Roman Savochenko                           *
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

#ifndef TEST_KERNEL_H
#define TEST_KERNEL_H

#include "tsys.h"

#undef _
#define _(mess) mod->I18N(mess)

namespace KernelTest
{

//************************************************
//* TTest                                        *
//************************************************
class TTest: public TSpecial
{
    public:
	TTest( string name );
	~TTest( );

	void modStart( );
	void modStop( );

    protected:
	void load_( );

    private:
	//Methods
	void pr_XMLNode( const char *cat, XMLNode *node, int level );
	string optDescr( );
	string modInfo( const string &name );
	void   modInfo( vector<string> &list );

	static void *Task(void *);
	void Test( const string &id, XMLNode *t_n );

	//Attributes
	pthread_t pthr_tsk;

	bool	endrun;		// Stop pthread command
};

extern TTest *mod;
}

#endif //TEST_KERNEL_H
