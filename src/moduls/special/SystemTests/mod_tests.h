
//OpenSCADA system module Special.SystemTests file: test_kernel.h
/***************************************************************************
 *   Copyright (C) 2003-2014 by Roman Savochenko, <rom_as@oscada.org>      *
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

using namespace OSCADA;

namespace KernelTest
{

//************************************************
//* TTest                                        *
//************************************************
class TTest: public TSpecial
{
    public:
	//Methods
	TTest( string name );
	~TTest( );

	void modStart( );
	void modStop( );

	static string cat( );
	void mess( const string &test, const char *fmt,  ... );
	void prXMLNode( const string &cat, XMLNode *node, int level = 0 );

	//> Test's functions
	void testList( vector<string> &ls )		{ chldList(mTest,ls); }
	bool testPresent( const string &id )		{ return chldPresent(mTest,id); }
	void testReg( TFunction *test )			{ chldAdd(mTest, test); }
	AutoHD<TFunction> testAt( const string &id )	{ return chldAt(mTest,id); }

    protected:
	//Methods
	void load_( );
	void cntrCmdProc( XMLNode *opt );		//Control interface command process
	TVariant objFuncCall( const string &id, vector<TVariant> &prms, const string &user );

    private:
	//Methods
	void postEnable( int flag );

	string optDescr( );
	string modInfo( const string &name );
	void   modInfo( vector<string> &list );

	static void *Task(void *);

	//Attributes
	bool	endrun;		// Stop pthread command

	int	mTest;
};

extern TTest *mod;
}

#endif //TEST_KERNEL_H
