
#ifndef TDYNPROPDIALOG
#define TDYNPROPDIALOG

#include <qdialog.h>

//#include <qlabel.h>
#include "titem.h"

namespace VISION
{
class TDynamicPropDialog : public QDialog
{
	Q_OBJECT
   public:
      //вызов диалогового окна настройки динамики propText свойства propName для элемента item:
      static QString getDynamic(QWidget *parent, TItem *item, const QString &propName, QString &propText);
   
   private:
      TDynamicPropDialog(QWidget* parent, TItem *item, const QString &propName, QString &propText);//const char* name, bool modal);
      ~TDynamicPropDialog();
      
      QString propName;
      QString propText;
      TItem *item;
      
      
   
};

}
#endif