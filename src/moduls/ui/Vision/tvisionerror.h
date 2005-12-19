
#ifndef TVISIONERR
#define TVISIONERR

#include <qstring.h>

namespace VISION
{
class TVisionError
{
   public:
      TVisionError(const QString &errClass, const QString &errMess, const int errType = 0);
      
      QString getClass() const;
      QString getMess() const;
      int getType() const;
      
   private:
      QString errClass;
      QString errMess;
      int errType;
};

}
#endif

