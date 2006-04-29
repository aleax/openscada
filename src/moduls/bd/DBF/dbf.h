
//OpenSCADA system module BD.DBF file: dbf.h
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

#ifndef DBF_H
#define DBF_H

#include <string>
using std::string;

struct db_head
{
    char ver;			//Версия DBF (def = 3)
    char data[3];		//Дата модификации
    //long numb_rec;		//Количество записей
    int numb_rec;		//Количество записей
    short len_head;		//Длина заголовка
    short len_rec;		//Длина записи
    char res[20];
};

struct db_str_rec
{
    char name[11];		//Имя поля
    char tip_fild;		//Тип поля (C - ASCII; N - номер; L - логический; M - Memo; D - Data)
    //long adr_in_mem;
    int adr_in_mem;
    unsigned char len_fild;		//Длина поля
    unsigned char dec_field;		//Знаков после "."
    char res[14];
};

//Объект база DBF

class TBasaDBF
{
  public:
    TBasaDBF(  );
    ~TBasaDBF(  );

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
    db_head * db_head_ptr;
				//Указатель на заголовок
    db_str_rec *db_field_ptr;	//Указатель на поля базы данных
    void **items;		//Записи
};

//----------------------------------------------------------------------

#endif // DBF_H

