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

#ifndef TEST_KERNEL_H
#define TEST_KERNEL_H

#include <xml.h>
#include <tspecials.h>

namespace KernelTest
{
    class TTest: public TSpecial
    {
	public:
	    TTest( string name );
	    ~TTest();

	    void start( );
	    void stop();

	    void mod_CheckCommandLine( );
	    void mod_UpdateOpt();
	private:
	    /*
	     * Print XMLNode attributes
	     */
	    void pr_XMLNode( XMLNode *node, int level );
	    void pr_opt_descr( FILE * stream );	
	    string mod_info( const string name );
	    void   mod_info( vector<string> &list );

	    static void *Task(void *);
	    void Test( int count );
	private:
	    static    SExpFunc ExpFuncLc[];
	    pthread_t pthr_tsk;

	    bool      endrun;	   // Stop pthread command     	
    };    
}

#endif //TEST_KERNEL_H
