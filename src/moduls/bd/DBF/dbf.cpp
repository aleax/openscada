
//OpenSCADA system module BD.DBF file: dbf.cpp
/***************************************************************************
 *   Copyright (C) 2001-2014 by Roman Savochenko                           *
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

#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>

#include <stdio.h>
#include "dbf.h"

//************************************************
//* TBaseDBF                                     *
//************************************************
TBasaDBF::TBasaDBF( )
{
    db_head_ptr = (db_head*)calloc(sizeof(db_head), 1);
    db_head_ptr->ver = 3;

    db_head_ptr->numb_rec = 0;
    db_head_ptr->len_head = sizeof(db_head) + 2;
    db_head_ptr->len_rec = 1;

    db_field_ptr = NULL;
    items = NULL;
}

TBasaDBF::~TBasaDBF( )
{
    if(db_field_ptr)	{ free(db_field_ptr); db_field_ptr = NULL; }
    if(items)
    {
	for(int i = 0; i < db_head_ptr->numb_rec; i++) free(items[i]);
	free(items); items = NULL;
    }

    free(db_head_ptr);
}

int TBasaDBF::LoadFile( char *Name )
{
    int hd;
    db_head db_head_temp;

    if((hd=open(Name,O_RDONLY)) < 0) return -1;
    off_t f_len = lseek(hd, 0, SEEK_END);
    lseek(hd, 0, SEEK_SET);
    bool rdOk = read(hd, &db_head_temp, sizeof(db_head)) > 0;	// read of dbf-file header
    if(!rdOk || f_len <= 0 || f_len != (db_head_temp.len_head+(db_head_temp.len_rec*db_head_temp.numb_rec)+1))
    {
	close(hd);
	return -1;
    }

    if(db_field_ptr)	{ free(db_field_ptr); db_field_ptr = NULL; }
    if(items)
    {
	for(int i = 0; i < db_head_ptr->numb_rec; i++) free(items[i]);
	free(items);
	items = NULL;
    }

    lseek(hd, 0, SEEK_SET);
    if((rdOk=read(hd, db_head_ptr, sizeof(db_head))) > 0)
    {
	db_field_ptr = (db_str_rec*)calloc(db_head_ptr->len_head-sizeof(db_head)-2, 1);
	if((rdOk=read(hd, db_field_ptr, db_head_ptr->len_head-sizeof(db_head)-2)) > 0)
	{
	    lseek(hd, 2, SEEK_CUR);
	    items = (void**)calloc(db_head_ptr->numb_rec, sizeof(void**));
	    for(int i = 0; rdOk && i < db_head_ptr->numb_rec; i++)
	    {
		items[i] = (void*)calloc(db_head_ptr->len_rec, 1);
		rdOk = read(hd, items[i], db_head_ptr->len_rec) > 0;
	    }
	}
    }

    close(hd);

    return db_head_ptr->numb_rec;
}

int TBasaDBF::SaveFile( char *Name )
{
    int hd;

    if((hd=open(Name,O_RDWR|O_CREAT|O_TRUNC,0666)) <= 0) return -1;
    bool wrOK = write(hd,db_head_ptr,sizeof(db_head)) > 0 &&
		write(hd,db_field_ptr,db_head_ptr->len_head-sizeof(db_head)-2) > 0 &&
		write(hd,"\x0D\x00",2) > 0;
    for(int i = 0; wrOK && i < db_head_ptr->numb_rec; i++)
	wrOK = write(hd,items[i],db_head_ptr->len_rec) > 0;
    wrOK = wrOK && write(hd, "\x1A", 1);
    close(hd);

    return wrOK ? 0 : -2;
}

int TBasaDBF::GetCountItems( )	{ return db_head_ptr->numb_rec; }

int TBasaDBF::LoadFields( db_str_rec *fields, int number )
{
    if(db_field_ptr)	{ free(db_field_ptr); db_field_ptr = NULL; }
    db_field_ptr = (db_str_rec*)calloc(number, sizeof(db_str_rec));
    memcpy(db_field_ptr, fields, number*sizeof(db_str_rec));

    if(items)
    {
	for(int i = 0; i < db_head_ptr->numb_rec; i++) free(items[i]);
	free(items); items = NULL;
    }

    db_head_ptr->numb_rec = 0;
    db_head_ptr->len_head = sizeof(db_head) + 2 + number*sizeof(db_str_rec);
    db_head_ptr->len_rec = 1;
    for(int i = 0; i < number; i++) db_head_ptr->len_rec += (db_field_ptr+i)->len_fild;

    return 0;
}

int TBasaDBF::DelField( int pos )
{
    int number, rec_len = 1, len_fild = 0;
    db_str_rec *field_ptr;

    number = (db_head_ptr->len_head-sizeof(db_head)-2) / sizeof(db_str_rec);
    if(pos >= number) return -1;
    if(db_head_ptr->numb_rec)
    {
	field_ptr = db_field_ptr + pos;
	len_fild = field_ptr->len_fild;
	if(pos == number - 1)
	    for(int i = 0; i < db_head_ptr->numb_rec; i++)
		items[i] = realloc(items[i], db_head_ptr->len_rec-len_fild);
	else
	{
	    for(int i = 0; i < pos; i++) rec_len += (db_field_ptr+i)->len_fild;
	    for(int i = 0; i < db_head_ptr->numb_rec; i++)
	    {
		memmove((char*)items[i]+rec_len, (char*)items[i]+rec_len+len_fild, db_head_ptr->len_rec-rec_len);
		items[i] = realloc(items[i], db_head_ptr->len_rec-len_fild);
	    }
	}
    }
    if(pos != (number-1)) memmove(db_field_ptr+pos, db_field_ptr+pos+1, (number-pos)*sizeof(db_str_rec));
    db_field_ptr = (db_str_rec*)realloc(db_field_ptr, (number-1)*sizeof(db_str_rec));

    db_head_ptr->len_head -= sizeof(db_str_rec);
    db_head_ptr->len_rec -= len_fild;

    return 0;
}

int TBasaDBF::DelField( char *NameField )
{
    int number, rec_len = 1, pos = -1, len_fild = 0;
    db_str_rec *field_ptr;

    number = (db_head_ptr->len_head-sizeof(db_head)-2) / sizeof(db_str_rec);
    for(int i = 0; i < number; i++)
	if(!strcmp(NameField,(db_field_ptr+i)->name))	{ pos = i; break; }
    if(pos == -1) return -1;
    if(db_head_ptr->numb_rec)
    {
	field_ptr = db_field_ptr + pos;
	len_fild = field_ptr->len_fild;
	if(pos == (number-1))
	    for(int i = 0; i < db_head_ptr->numb_rec; i++)
		items[i] = realloc(items[i], db_head_ptr->len_rec - len_fild);
	else
	{
	    for(int i = 0; i < pos; i++) rec_len += (db_field_ptr+i)->len_fild;
	    for(int i = 0; i < db_head_ptr->numb_rec; i++)
	    {
		memmove((char*)items[i]+rec_len, (char*)items[i]+rec_len+len_fild, db_head_ptr->len_rec-rec_len);
		items[i] = realloc(items[i], db_head_ptr->len_rec-len_fild);
	    }
	}
    }
    if(pos != (number-1)) memmove(db_field_ptr+pos, db_field_ptr+pos+1, (number-pos)*sizeof(db_str_rec));
    db_field_ptr = (db_str_rec*)realloc(db_field_ptr, (number-1)*sizeof(db_str_rec));

    db_head_ptr->len_head -= sizeof(db_str_rec);
    db_head_ptr->len_rec -= len_fild;

    return 0;
}

int TBasaDBF::addField( int pos, db_str_rec * field_ptr )
{
    int number, rec_len = 1, row;
    char *str_tmp;

    number = (db_head_ptr->len_head-sizeof(db_head)-2) / sizeof(db_str_rec);
    if(pos < (number-1))
    {
	db_field_ptr = (db_str_rec*)realloc(db_field_ptr, (number+1)*sizeof(db_str_rec));
	memmove(db_field_ptr+pos+1, db_field_ptr+pos, (number-pos)*sizeof(db_str_rec));
	memcpy(db_field_ptr+pos, field_ptr, sizeof(db_str_rec));

	if(db_head_ptr->numb_rec)
	{
	    for(int i = 0; i < pos; i++ ) rec_len += (db_field_ptr+i)->len_fild;
	    for(int i = 0; i < db_head_ptr->numb_rec; i++)
	    {
		//items[i]=realloc(items[i],db_head_ptr->len_rec+field_ptr->len_fild);
		str_tmp = (char*)malloc(db_head_ptr->len_rec+field_ptr->len_fild);
		memmove(str_tmp, items[i], db_head_ptr->len_rec);
		free(items[i]);
		items[i] = str_tmp;

		memmove((char*)items[i]+rec_len+field_ptr->len_fild, (char*)items[i]+rec_len, db_head_ptr->len_rec-rec_len);
		memset((char*)items[i]+rec_len, ' ', field_ptr->len_fild);
	    }
	}
	row = pos;
    }
    else
    {
	if(db_field_ptr) db_field_ptr = (db_str_rec*)realloc(db_field_ptr, (number+1)*sizeof(db_str_rec));
	else             db_field_ptr = (db_str_rec*)malloc(sizeof(db_str_rec));
	memcpy(db_field_ptr+number, field_ptr, sizeof(db_str_rec));
	for(int i = 0; i < db_head_ptr->numb_rec; i++)
	{
	    str_tmp = (char*)malloc(db_head_ptr->len_rec+field_ptr->len_fild);
	    memmove(str_tmp, items[i], db_head_ptr->len_rec);
	    free(items[i]);
	    items[i] = str_tmp;
	    //items[i]=realloc(items[i],db_head_ptr->len_rec+field_ptr->len_fild);
	    memset((char*)items[i]+db_head_ptr->len_rec, ' ', field_ptr->len_fild);
	}
	row = number;
    }
    db_head_ptr->len_head += sizeof(db_str_rec);
    db_head_ptr->len_rec += field_ptr->len_fild;

    return row;
}

db_str_rec *TBasaDBF::getField( int posField )
{
    int number = (db_head_ptr->len_head-sizeof(db_head)-2) / sizeof(db_str_rec);
    if(posField >= number) return NULL;
    return db_field_ptr + posField;
}

db_str_rec *TBasaDBF::getField( char *NameField )
{
    int number, posField = -1;

    number = (db_head_ptr->len_head-sizeof(db_head)-2) / sizeof(db_str_rec);
    for(int i = 0; i < number; i++)
	if(!strcmp(NameField,(db_field_ptr+i)->name)) { posField = i; break; }
    if(posField == -1) return NULL;
    return db_field_ptr + posField;
}

int TBasaDBF::setField( int posField, db_str_rec *attr )
{
    int number, rec_len = 1;
    char *str_tmp;

    number = (db_head_ptr->len_head-sizeof(db_head)-2) / sizeof(db_str_rec);
    if(posField >= number) return -1;

    if(!strncmp(db_field_ptr[posField].name,attr->name,11))	strncpy(db_field_ptr[posField].name, attr->name, 11);
    if(db_field_ptr[posField].tip_fild != attr->tip_fild)	db_field_ptr[posField].tip_fild = attr->tip_fild;
    if(db_field_ptr[posField].len_fild != attr->len_fild)
    {
	for(int i = 0; i < posField; i++) rec_len += db_field_ptr[i].len_fild;
	for(int i = 0; i < db_head_ptr->numb_rec; i++)
	{
	    str_tmp = (char*)calloc(db_head_ptr->len_rec+attr->len_fild-db_field_ptr[posField].len_fild, 1);
	    memmove(str_tmp, items[i], rec_len+(attr->len_fild<db_field_ptr[posField].len_fild)?attr->len_fild:db_field_ptr[posField].len_fild);
	    memmove(str_tmp+rec_len+attr->len_fild, (char*)items[i]+rec_len+db_field_ptr[posField].len_fild, db_head_ptr->len_rec-rec_len-db_field_ptr[posField].len_fild);
	    free(items[i]);
	    items[i] = str_tmp;
	}
	db_head_ptr->len_rec = db_head_ptr->len_rec+attr->len_fild-db_field_ptr[posField].len_fild;
	db_field_ptr[posField].len_fild = attr->len_fild;
    }
    if(db_field_ptr[posField].dec_field != attr->dec_field) db_field_ptr[posField].dec_field = attr->dec_field;

    return 0;
}

int TBasaDBF::setField( char *NameField, db_str_rec *attr )
{
    int number, posField = -1;

    number = (db_head_ptr->len_head-sizeof(db_head)-2) / sizeof(db_str_rec);
    for(int i = 0; i < number; i++)
	if(!strcmp(NameField,(db_field_ptr+i)->name))	{ posField = i; break; }
    if(posField == -1) return -1;
    return setField(posField, attr);
}

int TBasaDBF::CreateItems( int pos )
{
    void *temp;
    int number = db_head_ptr->numb_rec, line;

    if(pos < 0) pos = number;
    if(pos < number)
    {
	temp = (void*)calloc(number-pos, sizeof(void*));
	items = (void**)realloc(items, (number+1)*sizeof(void**));
	memcpy(temp, items+pos, (number-pos)*sizeof(void*));
	items[pos] = (void*)calloc(db_head_ptr->len_rec, 1);
	memset(items[pos], ' ', db_head_ptr->len_rec);
	memcpy(items+pos+1, temp, (number-pos)*sizeof(void*));
	free(temp);
	line = pos;
    }
    else
    {
	if(items) items = (void**)realloc(items, (number+1)*sizeof(void**));
	else      items = (void**)calloc(1, sizeof(void**));
	items[number] = (void*)calloc(db_head_ptr->len_rec, 1);
	memset(items[number], ' ', db_head_ptr->len_rec);
	line = number;
    }
    db_head_ptr->numb_rec++;

    return line;
}

int TBasaDBF::ModifiFieldIt( int posItems, int posField, const char *str )
{
    int rec_len = 1, number;

    number = (db_head_ptr->len_head-sizeof(db_head)-2) / sizeof(db_str_rec);
    if(posField >= number) return -1;
    for(int i = 0; i < posField; i++) rec_len += (db_field_ptr+i)->len_fild;
    if(posItems >= db_head_ptr->numb_rec) return -1;
    strncpy((char*)items[posItems]+rec_len, str, (db_field_ptr+posField)->len_fild);

    return 0;
}

int TBasaDBF::ModifiFieldIt( int posItems, char *NameField, const char *str )
{
    int rec_len = 1, number, posField = -1;
// float temp;

    number = (db_head_ptr->len_head-sizeof(db_head)-2) / sizeof(db_str_rec);
    for(int i = 0; i < number; i++)
	if(!strcmp(NameField,(db_field_ptr+i)->name))	{ posField = i; break; }
    if(posField == -1)	return -1;
    for(int i = 0; i < posField; i++) rec_len += (db_field_ptr+i)->len_fild;
    if(posItems >= db_head_ptr->numb_rec) return -1;

// if((db_field_ptr+posField)->tip_fild=='N')
// {
//  temp=atof(str); fcvt(temp,(db_field_ptr+posField)->dec_field,str);
// }
    strncpy((char*)items[posItems]+rec_len, str, (db_field_ptr+posField)->len_fild);

    return 0;
}

int TBasaDBF::GetFieldIt( int posItems, int posField, string & str )
{
    int rec_len = 1, number;

    number = (db_head_ptr->len_head-sizeof(db_head)-2) / sizeof(db_str_rec);
    if(posField >= number) return -1;
    for(int i = 0; i < posField; i++) rec_len += (db_field_ptr+i)->len_fild;
    if(posItems >= db_head_ptr->numb_rec) return -1;
    str.assign((char*)items[posItems]+rec_len, db_field_ptr[posField].len_fild);
    str.resize(strlen(str.c_str()));

    return 0;
}

int TBasaDBF::GetFieldIt( int posItems, char *NameField, string & str )
{
    int rec_len = 1, number, posField = -1;

    number = (db_head_ptr->len_head-sizeof(db_head)-2) / sizeof(db_str_rec);
    for(int i = 0; i < number; i++)
	if(!strcmp(NameField,(db_field_ptr+i)->name))	{ posField = i; break; }
    if(posField == -1) return -1;
    for(int i = 0; i < posField; i++) rec_len += (db_field_ptr+i)->len_fild;
    if(posItems >= db_head_ptr->numb_rec) return -1;
    str.assign((char*)items[posItems]+rec_len, db_field_ptr[posField].len_fild);
    str.resize(strlen(str.c_str()));

    return 0;
}

int TBasaDBF::DeleteItems( int pos, int fr )
{
    void *temp;
    int number = db_head_ptr->numb_rec;

    if(pos >= number) return -1;
    if(pos != (number-1))
    {
	temp = (void*)calloc(number-pos-1, sizeof(void*));
	memcpy(temp, items+pos+1, (number-pos-1)*sizeof(void*));
	if(fr) free(items[pos]);
	items = (void**)realloc(items, (number-1)*sizeof(void**));
	memcpy(items+pos, temp, (number-pos-1)*sizeof(void*));
	free(temp);
    }
    else
    {
	if(fr) free(items[pos]);
	items = (void**)realloc(items, (number-1)*sizeof(void**));
    }
    db_head_ptr->numb_rec--;

    return 0;
}

void *TBasaDBF::getItem( int posItem )
{
    if(posItem >= db_head_ptr->numb_rec) return items[db_head_ptr->numb_rec-1];
    else return items[posItem];
}

void TBasaDBF::AddItem( int pos, void *it )
{
    void *temp;
    int number = db_head_ptr->numb_rec;

    if(pos < number)
    {
	temp = (void*)calloc(number-pos, sizeof(void*));
	items = (void**)realloc(items, (number+1)*sizeof(void**));
	memcpy(temp, items+pos, (number-pos)*sizeof(void*));
	items[pos] = it;
	memcpy(items+pos+1, temp, (number-pos)*sizeof(void*));
	free(temp);
    }
    else
    {
	if(items) items = (void**)realloc(items, (number+1)*sizeof(void**));
	else items = (void**)calloc(1, sizeof(void**));
	items[db_head_ptr->numb_rec] = it;
    }
    db_head_ptr->numb_rec++;
}
