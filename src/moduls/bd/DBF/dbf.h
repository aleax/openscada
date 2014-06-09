
//OpenSCADA system module BD.DBF file: dbf.h
/***************************************************************************
 *   Copyright (C) 2001-2014 by Roman Savochenko, <rom_as@oscada.org>      *
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

#ifndef DBF_H
#define DBF_H

#include <string>
using std::string;

//************************************************
//* db_head                                      *
//************************************************
struct db_head
{
    char ver;			//Version of DBF (def = 3)
    char data[3];		//Modify data
    int numb_rec;		//Records count
    short len_head;		//Header length
    short len_rec;		//Record length
    char res[20];
};

//************************************************
//* db_str_rec                                   *
//************************************************
struct db_str_rec
{
    char name[11];		//Field name
    char tip_fild;		//Field type (C - ASCII; N - number; L - logical; M - Memo; D - Data)
    //long adr_in_mem;
    int adr_in_mem;
    unsigned char len_fild;	//Field length
    unsigned char dec_field;	//Digits after "."
    char res[14];
};

//************************************************
//* TBaseDBF                                     *
//************************************************
class TBasaDBF
{
    public:
	//Methods
	TBasaDBF( );
	~TBasaDBF( );

	int LoadFields( db_str_rec * fields, int number );
	int addField( int pos, db_str_rec * field_ptr );
	int DelField( int pos );
	int DelField( char *NameField );
	db_str_rec *getField( int posField );
	db_str_rec *getField( char *NameField );
	int setField( int posField, db_str_rec *attr );
	int setField( char *NameField, db_str_rec *attr );
	int CreateItems( int pos );
	int DeleteItems( int pos, int fr );
	void *getItem( int posItem );
	void AddItem( int posItem, void *it );
	int ModifiFieldIt( int posItems, int posField, const char *str );
	int ModifiFieldIt( int posItems, char *NameField, const char *str );
	int GetFieldIt( int posItems, int posField, string & str );
	int GetFieldIt( int posItems, char *NameField, string & str );
	int GetCountItems(  );
	int SaveFile( char *Name );
	int LoadFile( char *Name );

    protected:
	//Attributes
	db_head * db_head_ptr;		//pointer to header
	db_str_rec *db_field_ptr;	//pointer to db fields
	void **items;			//records
};

#endif // DBF_H
