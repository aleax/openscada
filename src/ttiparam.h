#ifndef TTIPARAM_H
#define TTIPARAM_H

#include <string>
using std::string;
#include <vector>
using std::vector;

#include "tconfig.h"

class TTipParam : public TElem
{
    public:
	TTipParam( string name, string descr, string bd );
	~TTipParam(  );
	
	string Name()  { return(_name); }
	string Descr() { return(_descr); }
	string BD()    { return(_bd); }
    private:
	string _name;
	string _descr;
	string _bd;
};

#endif // TTIPARAM_H

