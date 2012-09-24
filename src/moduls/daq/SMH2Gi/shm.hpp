#include <stddef.h>
#include <string.h>
#include <map>

#include "rlinifile.h"
#include "rlsharedmemory.h"

using namespace std;

enum { BOOL, SHORT, LONG, FLOAT };

class ShmValue {

	int	typ;
	int	off;
    
    public:
	ShmValue(int _type, int _off) : typ(_type), off(_off) { };
	int offset(void) { return off; };
	int type(void) { return typ; };
};

class string {
    char * s;
    char alloc;
    public:
	string(const char * str) { s = new char[(strlen(str) + 1)]; strcpy(s, str); alloc = 1; };
	string() { alloc = 0; };
	~string() { if(alloc) { delete s; alloc = 0; }; };
	bool operator()(const string * a, const string * b) { return strcmp(a->s, b->s) < 0; };
	bool operator()(const char * a, const char * b) { return strcmp(a, b) < 0; };
	string & operator=(const char * a) { s = (char*)a; return *this; };
};

typedef map<string*, ShmValue* , string> map_t;


class Shm {

//	char buf[4];
	map_t map_coil;
	map_t map_status;
	map_t map_inreg;
	map_t map_holdreg;
//	map<string*, ShmValue *>::iterator last;
	map_t *maps[4];
	rlSharedMemory	* shm;

	int 	parseSect(rlIniFile & ini, const char * sect, map_t * list);
	int 	parseItem(const char * name, map_t * list);
	int 	parseHeader(rlIniFile & ini, const char * item, int * off, int * qty);
	int 	isFound(const char *name, map_t::iterator &, map_t * m);
	int 	set(const char *name, void * , int size, map_t * first);
	int 	set(int, void * , int size);
	void *	get(const char * name, void *, int, map_t *, map_t *);
	int	get(const char * name, int);
	void *	get(int, void*, int);
    public:
    
	Shm(const char * name);
	~Shm();
	int 	getType(const char * name);
	int 	getOffset(const char * name);

	char 	getBool(const char * name);
	char 	getBool(int);
	int  	setBool(const char * name, char val);
	int  	setBool(int, char val);
	short	getShort(const char * name);
	short	getShort(int);
	int  	setShort(const char * name, short val);
	int  	setShort(int, short val);
	long 	getLong(const char * name);
	long 	getLong(int);
	int  	setLong(const char * name, long val);
	int  	setLong(int, long val);
	float 	getFloat(const char * name);
	float	getFloat(int);
	int  	setFloat(const char * name, float val);
	int  	setFloat(int, float val);
};



