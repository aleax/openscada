#include <stddef.h>
#include <string.h>
#include <map>

#include "rlinifile.h"
#include "shm.hpp"

int Shm::parseHeader(rlIniFile & ini, const char * item, int * off, int * qty)
{
    const char * ptr, *sect = "Slave";
    int ret = 0, tmp;
    
    ptr = ini.text(sect, item);
    if(ptr) {
	ret = sscanf(ptr, "%i,%i,%i", &tmp, off, qty);
    }
    return ret;
}

int Shm::parseItem(const char * name, map_t * list)
{
    char s[256];
    int tmp, size, type, off;
    int ret = sscanf(name, "%i,%i,%i,%i,%s", &tmp, &size, &type, &off, s);
    if(ret == 5) {
	ShmValue * val = new ShmValue(type, off);
	string * st = new string(s);
	list->insert(pair<string*, ShmValue*>(st, val));
//	list->insert(pair<const char *, ShmValue*>(s, val));
    }
    return 0;
}

int Shm::parseSect(rlIniFile & ini, const char * sect, map_t * list)
{
    const char * ptr = ini.firstName(sect);
    while(ptr) {
	const char * name = ini.text(sect, ptr);
	parseItem(name, list);
	ptr = ini.nextName(sect);
    }
    return 0;
}

Shm::~Shm()
{
    for(unsigned char i = 0; i < sizeof(maps) / sizeof(map_t *); i++) {
	for(map_t::iterator it = maps[i]->begin(); it != maps[i]->end(); it++) {
	    delete it->first;
	    delete it->second;
	}
    }    
    delete shm;
}

Shm::Shm(const char * name)
{
    maps[0] = &map_coil; maps[1] = &map_status; maps[2] = &map_inreg; maps[3] = &map_holdreg;

    const char *items[] = { "Coil", "Instat", "Inreg", "Holdreg" };
    
    rlIniFile ini;
    ini.read(name);
    int size = 0;
    for(unsigned char i = 0; i < sizeof(items) / sizeof(const char *); i++) {
	int ret, off, qty;
	ret = parseHeader(ini, items[i], &off, &qty);
	if(ret == 3)
	{
	    parseSect(ini, items[i], maps[i]);
	    size += qty;
	}
    }
    shm = new rlSharedMemory("/dev/shm/wsi", atoi(ini.text("Slave", "ShmSize")) ? atoi(ini.text("Slave", "ShmSize")) : size);
//    last = map_coil.end();
}

int Shm::isFound(const char *name, map_t::iterator & it, map_t * m)
{
    string s;
    s = name;
    it = m->find(&s);
    if(it != m->end())
	return 1;
    return 0;
}

int Shm::get(const char *name, int what)
{
    map_t *maps[] = { &map_coil, &map_status, &map_inreg, &map_holdreg };
    
    for(unsigned char i = 0; i < sizeof(maps) / sizeof(map_t*); i++) {
	map_t::iterator it;
	if(isFound(name, it, maps[i]))
	    return what ? it->second->offset() : it->second->type();
    }
    return -1;

}

int Shm::getType(const char * name)
{
    return get(name, 0);
}

int Shm::getOffset(const char * name)
{
    return get(name, 1);
}

void * Shm::get(int off, void * buf, int size)
{
    if(shm->read(off, buf, size) == size)
	return buf;
    return NULL;
}

int Shm::set(int off, void * val, int size)
{
    return shm->write(off, val, size);
}

void* Shm::get(const char *name, void * buf, int size, map_t * first, map_t * second)
{
    map_t::iterator it;
    if(isFound(name, it, first) || isFound(name, it, second)) {
	return get(it->second->offset(), buf, size);
    }
    return NULL;
}

int Shm::set(const char *name, void * val, int size, map_t * first)
{
    map_t::iterator it;
    if(isFound(name, it, first)) {
	return set(it->second->offset(), val, size);
    }
    return -1;
}

char Shm::getBool(const char *name)
{
    char buf;
    return get(name, &buf, 1, &map_status, &map_coil) ? buf : -1;
}

char Shm::getBool(int off)
{
    char buf;
    return get(off, &buf, 1) ? buf : -1;
}

int Shm::setBool(const char *name, char val)
{
//    if(last->second->type() == BOOL || strcmp(last->first, name) != 0)
    val = !!val;
    return set(name, &val, 1, &map_coil);
}

int Shm::setBool(int off, char val)
{
//    if(last->second->type() == BOOL || strcmp(last->first, name) != 0)
    val = !!val;
    return set(off, &val, 1);
}

short Shm::getShort(const char *name)
{	
    short buf;
    return get(name, &buf, 2, &map_inreg, &map_holdreg) ? buf : -1;
}

short Shm::getShort(int off)
{
    short buf;
    return get(off, &buf, 2) ? buf : -1;
}

int Shm::setShort(const char *name, short val)
{
    return set(name, &val, 2, &map_holdreg);
}

int Shm::setShort(int off, short val)
{
    return set(off, &val, 2);
}

long Shm::getLong(const char *name)
{
    long buf;
    return get(name, &buf, 4, &map_inreg, &map_holdreg) ? buf : -1;
}

long Shm::getLong(int off)
{
    long buf;
    return get(off, &buf, 4) ? buf : -1;
}

int Shm::setLong(const char *name, long val)
{
    return set(name, &val, 4, &map_holdreg);
}

int Shm::setLong(int off, long val)
{
    return set(off, &val, 4);
}

float Shm::getFloat(const char *name)
{
    float buf;
    return get(name, &buf, 4, &map_inreg, &map_holdreg) ? buf : -1;
}

float Shm::getFloat(int off)
{
    float buf;
    return get(off, &buf, 4) ? buf : -1;
}

int Shm::setFloat(const char *name, float val)
{
    return set(name, &val, 4, &map_holdreg);
}

int Shm::setFloat(int off, float val)
{
    return set(off, &val, 4);
}

