
#include "tvisionerror.h"

using namespace VISION;

TVisionError::TVisionError(const QString &errClass, const QString &errMess, const int errType)
{
   this->errClass = errClass;
   this->errMess = errMess;
   this->errType = errType;
}

QString TVisionError::getClass() const
{
   return errClass;
}
     
QString TVisionError::getMess() const
{
   return errMess;
}

int TVisionError::getType() const
{
   return errType;
}
