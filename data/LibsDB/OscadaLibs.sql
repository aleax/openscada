PRAGMA foreign_keys=OFF;
BEGIN TRANSACTION;
CREATE TABLE 'ParamTemplLibs' ("ID" TEXT DEFAULT '' ,"NAME" TEXT DEFAULT '' ,"uk#NAME" TEXT DEFAULT '' ,"DESCR" TEXT DEFAULT '' ,"uk#DESCR" TEXT DEFAULT '' ,"DB" TEXT DEFAULT '' ,"ru#NAME" TEXT DEFAULT '' ,"ru#DESCR" TEXT DEFAULT '' , PRIMARY KEY ("ID"));
INSERT INTO "ParamTemplLibs" VALUES('S7','S7','','Templates library of parameters for PLC series S7 of firm Siemens.','Бібліотека шаблонів параметрів для контролерів фірми Siemens серії S7.','tmplib_S7','','Библиотека шаблонов параметров для контроллеров фирмы Siemens серии S7.');
INSERT INTO "ParamTemplLibs" VALUES('base','Main templates','Базові шаблони','Author: Roman Savochenko <rom_as@oscada.org>
Version: 1.0.1
Allow realisation of the main templates.','Автор: Роман Савоченко <rom_as@oscada.org>
Версія: 1.0.1
Надає реалізацію базових шаблонів.','tmplib_base','Базовые шаблоны','Автор: Роман Савоченко <rom_as@oscada.org>
Версия: 1.0.1
Предоставляет реализацию базовых шаблонов.');
INSERT INTO "ParamTemplLibs" VALUES('DevLib','Devices','Бібліотека пристроїв','The templates library provides common templates and related functions for custom access to wide range of devices'' data with simple protocol to implement into User Protocol module, present complex protocols (ModBus, OPC_UA, HTTP) or direct at internal language and also for some integration the devices data.
Version: 1.9.0','','tmplib_DevLib','Библиотека устройств','');
INSERT INTO "ParamTemplLibs" VALUES('PrescrTempl','Prescription templates','Шаблони рецепту','','','tmplib_PrescrTempl','Шаблоны рецепта','');
INSERT INTO "ParamTemplLibs" VALUES('LowDevLib','Low-level devices','','The templates library provides common templates and related functions for custom access to low-level devices'' data with simple protocol to implement into User Protocol module, present complex protocols (ModBus, OPC_UA, HTTP) or direct at internal language and also for some integration the devices data.
Version: 1.1.0','','tmplib_LowDevLib','','');
CREATE TABLE 'UserFuncLibs' ("ID" TEXT DEFAULT '' ,"NAME" TEXT DEFAULT '' ,"DESCR" TEXT DEFAULT '' ,"DB" TEXT DEFAULT '' ,"uk#NAME" TEXT DEFAULT '' ,"uk#DESCR" TEXT DEFAULT '' ,"ru#NAME" TEXT DEFAULT '' ,"ru#DESCR" TEXT DEFAULT '' ,"PROG_TR" INTEGER DEFAULT '' , PRIMARY KEY ("ID"));
INSERT INTO "UserFuncLibs" VALUES('techApp','Technological devices','The models of the technological process devices.
Founded: october 2005
Author: Roman Savochenko
Version: 1.0.0
License: GPL','techApp','Технологічні апарати','Моделі апаратів технологічних процесів.

Засновано: жовтень 2005р
Автор: Роман Савоченко
Ліцензія: GPL','Технологические аппараты','Модели аппаратов технологических процессов.

Основан: октябрь 2005г
Автор: Роман Савоченко
Лицензия: GPL',0);
INSERT INTO "UserFuncLibs" VALUES('servProc','Service procedures','Library of service procedures for different using.
Author: Roman Savochenko <rom_as@oscada.org>
Version: 1.1.1
License: GPL','lib_servProc','Сервісні процедури','Бібліотека різноманітних сервісних процедур.','Сервисные процедуры','Библиотека различных сервисных процедур.',1);
INSERT INTO "UserFuncLibs" VALUES('doc','Report''s documents','Library of functions to facilitate the implementation of typical computations the primitive form of reporting documentation VCA "Document".
Founded: January 2008
Author: Roman Savochenko <rom_as@oscada.org>
Version: 1.1.0
License: GPL','flb_doc','Звітна документація','Бібліотека функцій для полегшення виконання типових обчислень примітиву формування звітної документації СВК "Документ".
Засновано: січень 2008р
Автор: Роман Савоченко <rom_as@oscada.org>
Версія: 1.1.0
Ліцензія: GPL','Отчётная документация','Библиотека функций для облегчения выполнения типовых
вычислений примитива формирования отчётной документации
СВУ "Документ".
Основана: январь 2008г
Автор: Роман Савоченко <rom_as@oscada.org>
Версия: 1.1.0
Лицензия: GPL',1);
INSERT INTO "UserFuncLibs" VALUES('regEl','Regulation elements','Regulation elements library.
Author: Roman Savochenko <rom_as@oscada.org>
Version: 1.0.0
License: GPL','flb_regEl','Елементи регулювання','Бібліотека елементів регулювання','Элементы регулирования','Библиотека элементов регулирования',0);
INSERT INTO "UserFuncLibs" VALUES('Controller','Controllers','Library of programs of controllers based on JavaLikeCalc.
Author: Roman Savochenko <rom_as@oscada.org>
Version: 1.1.1
License: GPL','lib_Controllers','Контролери','Програми контролерів базованих на JavaLikeCalc.','Контроллеры','Программы контроллеров основанных на JavaLikeCalc.',0);
INSERT INTO "UserFuncLibs" VALUES('web','XHTML-template','Pages processing functions library for XHTML-template user''s Web-interface.
Author: Roman Savochenko
Version: 0.1.0
License: GPL','flb_web','XHTML-шаблон','Бібліотека функцій обробки сторінок XHTML-шаблонів користувацткого Web-інтерфейсу.','XHTML-шаблон','Библиотека функций обработки страниц XHTML-шаблонов пользовательского Web-интерфейса.',0);
CREATE TABLE 'flb_doc_io' ("F_ID" TEXT DEFAULT '' ,"ID" TEXT DEFAULT '' ,"NAME" TEXT DEFAULT '' ,"TYPE" INTEGER DEFAULT '' ,"MODE" INTEGER DEFAULT '' ,"DEF" TEXT DEFAULT '' ,"HIDE" INTEGER DEFAULT '' ,"POS" INTEGER DEFAULT '' ,"uk#NAME" TEXT DEFAULT '' ,"uk#DEF" TEXT DEFAULT '' ,"ru#NAME" TEXT DEFAULT '' ,"ru#DEF" TEXT DEFAULT '' , PRIMARY KEY ("F_ID","ID"));
INSERT INTO "flb_doc_io" VALUES('getVal','rez','Result',0,2,'',0,0,'Результат','','Результат','');
INSERT INTO "flb_doc_io" VALUES('getVal','addr','Address',0,0,'',0,1,'Адреса','','Адрес','');
INSERT INTO "flb_doc_io" VALUES('getVal','time','Time (s)',1,0,'',0,2,'Час (сек.)','','Время (сек.)','');
INSERT INTO "flb_doc_io" VALUES('getVal','uTime','Time (us)',1,0,'',0,3,'Час (мксек.)','','Время (мксек.)','');
INSERT INTO "flb_doc_io" VALUES('getVal','prec','Signs after dot',1,0,'3',0,4,'Знаків після коми','','Знаков после запятой','');
INSERT INTO "flb_doc_io" VALUES('getVal','archtor','Archiver',0,0,'',0,5,'Архіватор','','Архиватор','');
INSERT INTO "flb_doc_io" VALUES('sumVal','rez','Result',0,2,'',0,0,'Результат','','Результат','');
INSERT INTO "flb_doc_io" VALUES('sumVal','addr','Address',0,0,'',0,1,'Адреса','','Адрес','');
INSERT INTO "flb_doc_io" VALUES('averVal','addr','Address',0,0,'',0,1,'Адреса','','Адрес','');
INSERT INTO "flb_doc_io" VALUES('averVal','rez','Result',0,2,'',0,0,'Результат','','Результат','');
INSERT INTO "flb_doc_io" VALUES('sumVal','prec','Signs after dot',1,0,'3',0,4,'Знаків після коми','','Знаков после запятой','');
INSERT INTO "flb_doc_io" VALUES('sumVal','archtor','Archiver',0,0,'',0,5,'Архіватор','','Архиватор','');
INSERT INTO "flb_doc_io" VALUES('sumVal','bTime','Begin time (s)',1,0,'',0,2,'Час початку (сек.)','','Время начала (сек.)','');
INSERT INTO "flb_doc_io" VALUES('sumVal','eTime','End time (s)',1,0,'',0,3,'Час кінця (сек.)','','Время конца (сек.)','');
INSERT INTO "flb_doc_io" VALUES('averVal','bTime','Begin time (s)',1,0,'',0,2,'Час початку (сек.)','','Время начала (сек.)','');
INSERT INTO "flb_doc_io" VALUES('averVal','eTime','End time (s)',1,0,'',0,3,'Час кінця (сек.)','','Время конца (сек.)','');
INSERT INTO "flb_doc_io" VALUES('averVal','prec','Signs after dot',1,0,'3',0,4,'Знаків після коми','','Знаков после запятой','');
INSERT INTO "flb_doc_io" VALUES('averVal','archtor','Archiver',0,0,'',0,5,'Архіватор','','Архиватор','');
INSERT INTO "flb_doc_io" VALUES('getVal','strong','Precisely',3,0,'0',0,6,'Точно','','Точно','');
INSERT INTO "flb_doc_io" VALUES('getVal','tryTo','Пробовать значение до {микросек}',1,0,'0',0,7,'','','Пробовать значение до {микросек}','');
INSERT INTO "flb_doc_io" VALUES('averVal','actPerc','Процент активных значений',2,1,'',0,6,'','','Процент активных значений','');
CREATE TABLE 'flb_regEl_io' ("F_ID" TEXT DEFAULT '' ,"ID" TEXT DEFAULT '' ,"NAME" TEXT DEFAULT '' ,"uk#NAME" TEXT DEFAULT '' ,"TYPE" INTEGER DEFAULT '' ,"MODE" INTEGER DEFAULT '' ,"DEF" TEXT DEFAULT '' ,"uk#DEF" TEXT DEFAULT '' ,"HIDE" INTEGER DEFAULT '' ,"POS" INTEGER DEFAULT '' ,"ru#NAME" TEXT DEFAULT '' ,"ru#DEF" TEXT DEFAULT '' , PRIMARY KEY ("F_ID","ID"));
INSERT INTO "flb_regEl_io" VALUES('pidUnif','var','Variable','Змінна',2,0,'0','',0,0,'Переменная','');
INSERT INTO "flb_regEl_io" VALUES('pidUnif','sp','Setpoint','Завдання',2,0,'0','',0,1,'Задание','');
INSERT INTO "flb_regEl_io" VALUES('pidUnif','max','Scale:maximum','Шкала:максимум',2,0,'100','',0,2,'Шкала:максимум','');
INSERT INTO "flb_regEl_io" VALUES('pidUnif','min','Scale:minimum','Шкала:мінімум',2,0,'0','',0,3,'Шкала:минимум','');
INSERT INTO "flb_regEl_io" VALUES('pidUnif','manIn','Manual input','Ручний ввід',2,0,'0','',0,4,'Ручной ввод','');
INSERT INTO "flb_regEl_io" VALUES('pidUnif','out','Output (%)','Вихід (%)',2,2,'0','',0,5,'Выход (%)','');
INSERT INTO "flb_regEl_io" VALUES('pidUnif','impQup','Impulse output up','Імп. вихід у гору',3,1,'0','',0,6,'Имп. выход вверх','');
INSERT INTO "flb_regEl_io" VALUES('pidUnif','impQdwn','Impulse output down','Імп. вихід до низу',3,1,'0','',0,7,'Имп. выход вниз','');
INSERT INTO "flb_regEl_io" VALUES('pidUnif','analog','Analog','Аналоговий',3,0,'1','',0,8,'Аналоговый','');
INSERT INTO "flb_regEl_io" VALUES('pidUnif','auto','Automate','Автомат',3,0,'0','',0,9,'Автомат','');
INSERT INTO "flb_regEl_io" VALUES('pidUnif','casc','Cascade','Каскад',3,0,'0','',0,10,'Каскад','');
INSERT INTO "flb_regEl_io" VALUES('pidUnif','Kp','Gain','Kп',2,0,'1','',0,11,'Kп','');
INSERT INTO "flb_regEl_io" VALUES('pidUnif','Ti','Ti (ms)','Tі (мс)',1,0,'1000','',0,12,'Tи (мс)','');
INSERT INTO "flb_regEl_io" VALUES('pidUnif','Td','Td (ms)','Tд (мс)',1,0,'0','',0,14,'Tд (мс)','');
INSERT INTO "flb_regEl_io" VALUES('pidUnif','TImpPer','T impulses period (ms)','T період імпульсів (мс)',1,0,'5000','',0,16,'T период импульса (мс)','');
INSERT INTO "flb_regEl_io" VALUES('pidUnif','TImpMin','T minimal impulse (ms)','Мінімальна довжина імпульсу (мс)',1,0,'500','',0,17,'T минимальная длина импульса (мс)','');
INSERT INTO "flb_regEl_io" VALUES('pidUnif','KImpRfact','Impulse rate factor','Рейтинг фактор імпульсів',2,0,'1','',0,18,'Рейтинг фактор импульса','');
INSERT INTO "flb_regEl_io" VALUES('pidUnif','Hup','Upper output border (%)','Верхня границя виходу (%)',2,0,'100','',0,19,'Верхняя граница выхода (%)','');
INSERT INTO "flb_regEl_io" VALUES('pidUnif','Hdwn','Lower output border (%)','Нижня границя виходу (%)',2,0,'0','',0,20,'Нижняя граница выхода (%)','');
INSERT INTO "flb_regEl_io" VALUES('pidUnif','Zi','Insensitivity (%)','Нечутливість (%)',2,0,'0','',0,21,'Нечувствительность (%)','');
INSERT INTO "flb_regEl_io" VALUES('pidUnif','K1','Coefficient of input 1','Коеф. входу 1',2,0,'0','',0,23,'Коэф. входа 1','');
INSERT INTO "flb_regEl_io" VALUES('pidUnif','in1','Input 1','Вхід 1',2,0,'0','',0,24,'Вход 1','');
INSERT INTO "flb_regEl_io" VALUES('pidUnif','K2','Coefficient of input 2','Коеф. входу 2',2,0,'0','',0,25,'Коэф. входа 2','');
INSERT INTO "flb_regEl_io" VALUES('pidUnif','in2','Input 2','Вхід 2',2,0,'0','',0,26,'Вход 2','');
INSERT INTO "flb_regEl_io" VALUES('pidUnif','K3','Coefficient of input 3','Коеф. входу 3',2,0,'0','',0,27,'Коэф. входа 3','');
INSERT INTO "flb_regEl_io" VALUES('pidUnif','in3','Input 3','Вхід 3',2,0,'0','',0,28,'Вход 3','');
INSERT INTO "flb_regEl_io" VALUES('pidUnif','K4','Coefficient of input 4','Коеф. входу 4',2,0,'0','',0,29,'Коэф. входа 4','');
INSERT INTO "flb_regEl_io" VALUES('pidUnif','in4','Input 4','Вхід 4',2,0,'0','',0,30,'Вход 4','');
INSERT INTO "flb_regEl_io" VALUES('pidUnif','impAnOut','Analog position of impulse regulator','Аналогове положення імпульсного регулятора',2,0,'0','',0,31,'Аналоговое положение импульсного регулятора','');
INSERT INTO "flb_regEl_io" VALUES('pidUnif','int','Integral accumulation','Накопичення інтегралу',2,1,'0','',1,38,'Накопление интеграла','');
INSERT INTO "flb_regEl_io" VALUES('pidUnif','dif','Differential accumulation','Знач. диференціалу',2,1,'0','',1,39,'Накопление производной','');
INSERT INTO "flb_regEl_io" VALUES('pidUnif','lag','Lag accumulation','Накопичення затримки',2,1,'0','',1,40,'Накопление задержки','');
INSERT INTO "flb_regEl_io" VALUES('pidUnif','impLag','Impulse lag time','Час утримання імпульсу',2,1,'0','',1,41,'Время удержания импульса','');
INSERT INTO "flb_regEl_io" VALUES('pidUnif','perLag','New impulse generation time','Час генерації нового імпульсу',2,1,'0','',1,42,'Время генерации нового импульса','');
INSERT INTO "flb_regEl_io" VALUES('pidUnifD','var','Variable','Змінна',2,0,'0','',0,0,'Переменная','');
INSERT INTO "flb_regEl_io" VALUES('pidUnifD','sp','Setpoint','Завдання',2,0,'0','',0,1,'Задание','');
INSERT INTO "flb_regEl_io" VALUES('pidUnifD','max','Scale:maximum','Шкала:максимум',2,0,'100','',0,2,'Шкала:максимум','');
INSERT INTO "flb_regEl_io" VALUES('pidUnifD','min','Scale:minimum','Шкала:мінімум',2,0,'0','',0,3,'Шкала:минимум','');
INSERT INTO "flb_regEl_io" VALUES('pidUnifD','manIn','Manual input','Ручний ввід',2,0,'0','',0,4,'Ручной ввод','');
INSERT INTO "flb_regEl_io" VALUES('pidUnifD','out','Output (%)','Вихід (%)',2,2,'0','',0,5,'Выход (%)','');
INSERT INTO "flb_regEl_io" VALUES('pidUnifD','impQup','Impulse output up','Імп. вихід у гору',3,1,'0','',0,6,'Имп. выход вверх','');
INSERT INTO "flb_regEl_io" VALUES('pidUnifD','impQdwn','Impulse output down','Імп. вихід до низу',3,1,'0','',0,7,'Имп. выход вниз','');
INSERT INTO "flb_regEl_io" VALUES('pidUnifD','analog','Analog','Аналоговий',3,0,'1','',0,8,'Аналоговый','');
INSERT INTO "flb_regEl_io" VALUES('pidUnifD','auto','Automat','Автомат',3,0,'0','',0,9,'Автомат','');
INSERT INTO "flb_regEl_io" VALUES('pidUnifD','casc','Cascade','Каскад',3,0,'0','',0,10,'Каскад','');
INSERT INTO "flb_regEl_io" VALUES('pidUnifD','Kp','Gain','Kп',2,0,'1','',0,11,'Kп','');
INSERT INTO "flb_regEl_io" VALUES('pidUnifD','Ti','Ti (ms)','Tі (мс)',1,0,'1000','',0,12,'Tи (мс)','');
INSERT INTO "flb_regEl_io" VALUES('pidUnifD','Kd','Gain differential','Кп диференціалу',2,0,'1','',0,13,'Кп производной','');
INSERT INTO "flb_regEl_io" VALUES('pidUnifD','Td','Td (ms)','Tд (мс)',1,0,'0','',0,14,'Tд (мс)','');
INSERT INTO "flb_regEl_io" VALUES('pidUnifD','Tzd','T differential lag (ms)','T фільтру диференціалу (мс)',1,0,'0','',0,15,'T задержки дифференциров. (мс)','');
INSERT INTO "flb_regEl_io" VALUES('pidUnifD','TImpPer','T impulses period (ms)','T період імпульсів (мс)',1,0,'5000','',0,16,'T период импульса (мс)','');
INSERT INTO "flb_regEl_io" VALUES('pidUnifD','TImpMin','T minimal impulse (ms)','Мінімальна довжина імпульсу (мс)',1,0,'500','',0,17,'T минимальная длина импульса (мс)','');
INSERT INTO "flb_regEl_io" VALUES('pidUnifD','KImpRfact','Impulse rate factor','Рейтинг фактор імпульсів',2,0,'1','',0,18,'Рейтинг фактор импульса','');
INSERT INTO "flb_regEl_io" VALUES('pidUnifD','Hup','Upper output border (%)','Верхня границя виходу (%)',2,0,'100','',0,19,'Верхняя граница выхода (%)','');
INSERT INTO "flb_regEl_io" VALUES('pidUnifD','Hdwn','Lower output border (%)','Нижня границя виходу (%)',2,0,'0','',0,20,'Нижняя граница выхода (%)','');
INSERT INTO "flb_regEl_io" VALUES('pidUnifD','Zi','Insensitivity (%)','Нечутливість (%)',2,0,'0','',0,21,'Нечувствительность (%)','');
INSERT INTO "flb_regEl_io" VALUES('pidUnifD','followSp','Follow to variable by setpoint into manual','Відслідковувати завданням за змінною у ручному',3,0,'1','',0,22,'Следить заданием за переменной в ручном','');
INSERT INTO "flb_regEl_io" VALUES('pidUnifD','K1','Coefficient of input 1','Коеф. входу 1',2,0,'0','',0,23,'Коэф. входа 1','');
INSERT INTO "flb_regEl_io" VALUES('pidUnifD','in1','Input 1','Вхід 1',2,0,'0','',0,24,'Вход 1','');
INSERT INTO "flb_regEl_io" VALUES('pidUnifD','K2','Coefficient of input 2','Коеф. входу 2',2,0,'0','',0,25,'Коэф. входа 2','');
INSERT INTO "flb_regEl_io" VALUES('pidUnifD','in2','Input 2','Вхід 2',2,0,'0','',0,26,'Вход 2','');
INSERT INTO "flb_regEl_io" VALUES('pidUnifD','K3','Coefficient of input 3','Коеф. входу 3',2,0,'0','',0,27,'Коэф. входа 3','');
INSERT INTO "flb_regEl_io" VALUES('pidUnifD','in3','Input 3','Вхід 3',2,0,'0','',0,28,'Вход 3','');
INSERT INTO "flb_regEl_io" VALUES('pidUnifD','K4','Coefficient of input 4','Коеф. входу 4',2,0,'0','',0,29,'Коэф. входа 4','');
INSERT INTO "flb_regEl_io" VALUES('pidUnifD','in4','Input 4','Вхід 4',2,0,'0','',0,30,'Вход 4','');
INSERT INTO "flb_regEl_io" VALUES('pidUnifD','impAnOut','Real analog position of impulse regulator, [0...100], -1 - disable','Аналогове положення імпульсного регулятора',2,0,'-1','',0,33,'Аналоговое положение импульсного регулятора или ск','');
INSERT INTO "flb_regEl_io" VALUES('pidUnif','Kd','Gain differential','Кп диференціалу',2,0,'1','',0,13,'Кп производной','');
INSERT INTO "flb_regEl_io" VALUES('pidUnifD','impAnImit','Full stroke time for analog position imitation, s','Імітація аналогового положення',2,0,'-1','',0,34,'Имитация аналогового положения','');
INSERT INTO "flb_regEl_io" VALUES('pidUnifD','impQupTm','Imp. output up lag for GUI','Імп. вихід вгору утримано для GUI',3,1,'0','',0,35,'Имп. выход вверх удерж. для GUI','');
INSERT INTO "flb_regEl_io" VALUES('pidUnifD','impQdwnTm','Imp. output down lag for GUI','Імп. вихід додолу утримано для GUI',3,1,'0','',0,36,'Имп. выход вниз удерж. для GUI','');
INSERT INTO "flb_regEl_io" VALUES('pidUnifD','f_frq','Calc frequency (Hz)','Частота обчислення (Гц)',2,0,'1','',1,37,'Частота вычисления (Гц)','');
INSERT INTO "flb_regEl_io" VALUES('pidUnifD','f_start','First start','Перший запуск',3,0,'0','',1,38,'Первый запуск','');
INSERT INTO "flb_regEl_io" VALUES('pidUnifD','f_stop','Last start','Останній запуск',3,0,'0','',1,39,'Последний запуск','');
INSERT INTO "flb_regEl_io" VALUES('pidUnifD','impLag','Impulse lag time','Час утримання імпульсу',2,1,'0','',1,40,'Время удержания импульса','');
INSERT INTO "flb_regEl_io" VALUES('pidUnifD','perLag','New impulse generation time','Час генерації нового імпульсу',2,1,'0','',1,41,'Время генерации нового импульса','');
INSERT INTO "flb_regEl_io" VALUES('pidUnifD','integ','Integral accumulation','Накопичення інтегралу',2,1,'0','',1,42,'Накопление интеграла','');
INSERT INTO "flb_regEl_io" VALUES('pidUnifD','difer','Differential accumulation','Накопичення диференціалу',2,1,'0','',1,43,'Накопление производной','');
INSERT INTO "flb_regEl_io" VALUES('pidUnifD','dlag','Differential lag accumulation','Фільтр диференціалу',2,1,'0','',1,44,'Накопление задержки производной','');
INSERT INTO "flb_regEl_io" VALUES('pidUnif','Tzd','T differential lag (ms)','T фільтру диференціалу (мс)',1,0,'0','',0,15,'T задержки дифференциров. (мс)','');
INSERT INTO "flb_regEl_io" VALUES('pidUnif','followSp','Follow to variable by setpoint into manual','Відслідковувати завданням за змінною у ручному',3,0,'1','',0,22,'Следить заданием за переменной в ручном','');
INSERT INTO "flb_regEl_io" VALUES('pidUnif','impAnImit','Analog position imitation','Імітація аналогового положення',3,0,'0','',0,32,'Имитация аналогового положения','');
INSERT INTO "flb_regEl_io" VALUES('pidUnif','impQupTm','Imp. output up lag for GUI',' Імп. вихід вгору утримано для GUI',1,1,'0','',0,33,'Имп. выход вверх удерж. для GUI','');
INSERT INTO "flb_regEl_io" VALUES('pidUnif','impQdwnTm','Imp. output down lag for GUI','Імп. вихід додолу утримано для GUI',1,1,'0','',0,34,'Имп. выход вниз удерж. для GUI','');
INSERT INTO "flb_regEl_io" VALUES('pidUnif','f_frq','Calc frequency (Hz)','Частота обчислення (Гц)',1,0,'1','',1,35,'Частота вычисления (Гц)','');
INSERT INTO "flb_regEl_io" VALUES('pidUnif','f_start','First start','Перший запуск',3,0,'0','',1,36,'Первый запуск','');
INSERT INTO "flb_regEl_io" VALUES('pidUnif','f_stop','Last start','Останній запуск',3,0,'0','',1,37,'Последний запуск','');
INSERT INTO "flb_regEl_io" VALUES('pidImp','var','Variable','Змінна',2,0,'0','',0,0,'Переменная','');
INSERT INTO "flb_regEl_io" VALUES('pidImp','sp','Setpoint','Завдання',2,0,'0','',0,1,'Задание','');
INSERT INTO "flb_regEl_io" VALUES('pidImp','max','Scale:maximum','Шкала:максимум',2,0,'100','',0,2,'Шкала:максимум','');
INSERT INTO "flb_regEl_io" VALUES('pidImp','min','Scale:minimum','Шкала:мінімум',2,0,'0','',0,3,'Шкала:минимум','');
INSERT INTO "flb_regEl_io" VALUES('pidImp','manIn','Manual input','Ручний ввід',2,0,'0','',0,4,'Ручной ввод','');
INSERT INTO "flb_regEl_io" VALUES('pidImp','out','Output (%)','Вихід (%)',2,2,'0','',0,5,'Выход (%)','');
INSERT INTO "flb_regEl_io" VALUES('pidImp','outPID','PID output','Вихід ПІД',2,1,'0','',0,6,'Выход ПИД','');
INSERT INTO "flb_regEl_io" VALUES('pidImp','impQup','Impulse output up','Імп. вихід у гору',3,1,'0','',0,7,'Имп. выход вверх','');
INSERT INTO "flb_regEl_io" VALUES('pidImp','impQdwn','Impulse output down','Імп. вихід до низу',3,1,'0','',0,8,'Имп. выход вниз','');
INSERT INTO "flb_regEl_io" VALUES('pidImp','auto','Automate','Автомат',3,0,'0','',0,9,'Автомат','');
INSERT INTO "flb_regEl_io" VALUES('pidImp','casc','Cascade','Каскад',3,0,'0','',0,10,'Каскад','');
INSERT INTO "flb_regEl_io" VALUES('pidImp','Kp','Gain','Kп',2,0,'1','',0,11,'Kп','');
INSERT INTO "flb_regEl_io" VALUES('pidImp','Ki','Gain integration','Кп інтегралу',2,0,'1','',0,12,'Кп интеграла','');
INSERT INTO "flb_regEl_io" VALUES('pidImp','Ti','Ti (ms)','Tі (мс)',1,0,'1000','',0,13,'Tи (мс)','');
INSERT INTO "flb_regEl_io" VALUES('pidImp','Kd','Gain differential','Кп диференціалу',2,0,'1','',0,14,'Кп производной','');
INSERT INTO "flb_regEl_io" VALUES('pidImp','Td','Td (ms)','Tд (мс)',1,0,'0','',0,15,'Tд (мс)','');
INSERT INTO "flb_regEl_io" VALUES('pidImp','Tzd','T differential lag (ms)','T фільтру диференціалу (мс)',1,0,'0','',0,16,'T задержки дифференциров. (мс)','');
INSERT INTO "flb_regEl_io" VALUES('pidImp','TImpPer','T impulses period (ms)','T період імпульсів (мс)',1,0,'5000','',0,17,'T период импульса (мс)','');
INSERT INTO "flb_regEl_io" VALUES('pidImp','TImpMin','T minimal impulse (ms)','Мінімальна довжина імпульсу (мс)',1,0,'500','',0,18,'T минимальная длина импульса (мс)','');
INSERT INTO "flb_regEl_io" VALUES('pidImp','KImpRfact','Impulse rate factor','Рейтинг фактор імпульсів',2,0,'1','',0,19,'Рейтинг фактор импульса','');
INSERT INTO "flb_regEl_io" VALUES('pidImp','Hup','Upper output border (%)','Верхня границя виходу (%)',2,0,'100','',0,20,'Верхняя граница выхода (%)','');
INSERT INTO "flb_regEl_io" VALUES('pidImp','Hdwn','Lower output border (%)','Нижня границя виходу (%)',2,0,'0','',0,21,'Нижняя граница выхода (%)','');
INSERT INTO "flb_regEl_io" VALUES('pidImp','Zi','Insensitivity (%)','Нечутливість (%)',2,0,'0','',0,22,'Нечувствительность (%)','');
INSERT INTO "flb_regEl_io" VALUES('pidImp','followSp','Follow to variable by setpoint into manual','Відслідковувати завданням за змінною у ручному',3,0,'1','',0,23,'Следить заданием за переменной в ручном','');
INSERT INTO "flb_regEl_io" VALUES('pidImp','K1','Coefficient of input 1','Коеф. входу 1',2,0,'0','',0,24,'Коэф. входа 1','');
INSERT INTO "flb_regEl_io" VALUES('pidImp','in1','Input 1','Вхід 1',2,0,'0','',0,25,'Вход 1','');
INSERT INTO "flb_regEl_io" VALUES('pidImp','K2','Coefficient of input 2','Коеф. входу 2',2,0,'0','',0,26,'Коэф. входа 2','');
INSERT INTO "flb_regEl_io" VALUES('pidImp','in2','Input 2','Вхід 2',2,0,'0','',0,27,'Вход 2','');
INSERT INTO "flb_regEl_io" VALUES('pidImp','K3','Coefficient of input 3','Коеф. входу 3',2,0,'0','',0,28,'Коэф. входа 3','');
INSERT INTO "flb_regEl_io" VALUES('pidImp','in3','Input 3','Вхід 3',2,0,'0','',0,29,'Вход 3','');
INSERT INTO "flb_regEl_io" VALUES('pidImp','K4','Coefficient of input 4','Коеф. входу 4',2,0,'0','',0,30,'Коэф. входа 4','');
INSERT INTO "flb_regEl_io" VALUES('pidImp','in4','Input 4','Вхід 4',2,0,'0','',0,31,'Вход 4','');
INSERT INTO "flb_regEl_io" VALUES('pidImp','impAnOut','Analog position of impulse regulator','Аналогове положення імпульсного регулятора',2,0,'0','',0,32,'Аналоговое положение импульсного регулятора или ск','');
INSERT INTO "flb_regEl_io" VALUES('pidImp','impAnImit','Analog position imitation','Імітація аналогового положення',3,0,'0','',0,33,'Имитация аналогового положения','');
INSERT INTO "flb_regEl_io" VALUES('pidImp','impQupTm','Imp. output up lag for GUI','Імп. вихід вгору утримано для GUI',1,1,'0','',0,34,'Имп. выход вверх удерж. для GUI','');
INSERT INTO "flb_regEl_io" VALUES('pidImp','impQdwnTm','Imp. output down lag for GUI','Імп. вихід додолу утримано для GUI',1,1,'0','',0,35,'Имп. выход вниз удерж. для GUI','');
INSERT INTO "flb_regEl_io" VALUES('pidImp','f_frq','Calc frequency (Hz)','Частота обчислення (Гц)',2,0,'1','',1,36,'Частота вычисления (Гц)','');
INSERT INTO "flb_regEl_io" VALUES('pidImp','f_start','First start','Перший запуск',3,0,'0','',1,37,'Первый запуск','');
INSERT INTO "flb_regEl_io" VALUES('pidImp','f_stop','Last start','Останній запуск',3,0,'0','',1,38,'Последний запуск','');
INSERT INTO "flb_regEl_io" VALUES('pidImp','impLag','Impulse lag time','Час утримання імпульсу',2,1,'0','',1,39,'Время удержания импульса','');
INSERT INTO "flb_regEl_io" VALUES('pidImp','perLag','New impulse generation time','Час генерації нового імпульсу',2,1,'0','',1,40,'Время генерации нового импульса','');
INSERT INTO "flb_regEl_io" VALUES('pidImp','integ','Integral accumulation','Накопичення інтегралу',2,1,'0','',1,41,'Накопление интеграла','');
INSERT INTO "flb_regEl_io" VALUES('pidImp','difer','Differential accumulation','Накопичення диференціалу',2,1,'0','',1,42,'Накопление производной','');
INSERT INTO "flb_regEl_io" VALUES('pidImp','dlag','Differential lag accumulation','Фільтр диференціалу',2,1,'0','',1,43,'Накопление задержки производной','');
INSERT INTO "flb_regEl_io" VALUES('pidImp','err1','Error step back','Помилка крок назад',2,1,'0','',1,44,'Ошибка шаг назад','');
INSERT INTO "flb_regEl_io" VALUES('pidImp','err2','Error two step back','Помилка два кроки назад',2,1,'0','',1,45,'Ошибка два шага назад','');
INSERT INTO "flb_regEl_io" VALUES('pidUnifD','QO','Opened state of the control mechanism','Відкритий стан виконавчого механізму',3,0,'0','',0,31,'Открытое состояние исполнительного механизма','');
INSERT INTO "flb_regEl_io" VALUES('pidUnifD','QZ','Closed state of the control mechanism','Закритий стан виконавчого механізму',3,0,'0','',0,32,'Закрытое состояние исполнительного механизма','');
CREATE TABLE 'lib_Controllers_io' ("F_ID" TEXT DEFAULT '' ,"ID" TEXT DEFAULT '' ,"NAME" TEXT DEFAULT '' ,"TYPE" INTEGER DEFAULT '' ,"MODE" INTEGER DEFAULT '' ,"DEF" TEXT DEFAULT '' ,"HIDE" INTEGER DEFAULT '' ,"POS" INTEGER DEFAULT '' ,"ru#NAME" TEXT DEFAULT '' ,"ru#DEF" TEXT DEFAULT '' ,"uk#NAME" TEXT DEFAULT '' ,"uk#DEF" TEXT DEFAULT '' , PRIMARY KEY ("F_ID","ID"));
INSERT INTO "lib_Controllers_io" VALUES('prescr','dbDB','DB:DB with tables name',0,1,'',0,4,'БД:Имя БД с таблицами','','БД:Ім''я БД з таблицями','');
INSERT INTO "lib_Controllers_io" VALUES('prescr','dbComs','DB:Table with commands',0,1,'',0,5,'БД:Таблица с коммандами','','БД:Таблиця з командами','');
INSERT INTO "lib_Controllers_io" VALUES('prescr','dbProgs','DB:Table with programs',0,1,'',0,6,'БД:Таблица с программами','','БД:Таблиця з програмами','');
INSERT INTO "lib_Controllers_io" VALUES('prescr','mode','Mode: Finish(-2); Error(-1); Stop(0); Run (1); Pause(2)',1,1,'',0,8,'Режим: Оконч.(-2); Ошибка(-1); Стоп(0); Исп.(1); П','','Режим: Окінч.(-2); Помилка(-1); Стоп(0); Вик.(1); ','');
INSERT INTO "lib_Controllers_io" VALUES('prescr','curMode','Current mode',1,0,'',0,9,'Текущий режим','','Поточний режим','');
INSERT INTO "lib_Controllers_io" VALUES('prescr','prog','Name of selected program',0,1,'',0,10,'Имя выбранной программы','','Ім''я обраної програми','');
INSERT INTO "lib_Controllers_io" VALUES('prescr','startTm','Start time (seconds)',1,0,'',0,11,'Время запуска (секунды)','','Час запуску (секунди)','');
INSERT INTO "lib_Controllers_io" VALUES('prescr','curCom','Current command-step',0,0,'',0,12,'Текущая комманда-шаг','','Поточна команда-крок','');
INSERT INTO "lib_Controllers_io" VALUES('prescr','work','Work program',4,0,'',0,13,'Рабочая программа','','Робоча програма','');
INSERT INTO "lib_Controllers_io" VALUES('prescr','comsCntr','Commands controller',0,0,'',0,7,'Контроллер команд','','Контролер команд','');
INSERT INTO "lib_Controllers_io" VALUES('prescr','comLs','Commands list',4,0,'',0,14,'Список команд','','Перелік команд','');
INSERT INTO "lib_Controllers_io" VALUES('prescr','clcCnt','Cycles counter',1,0,'',0,15,'Счётчик циклов','','Лічильник циклів','');
INSERT INTO "lib_Controllers_io" VALUES('test','offset','Offset',1,0,'100',0,4,'Смещение','','Зміщення','');
INSERT INTO "lib_Controllers_io" VALUES('test','out','Out',2,1,'',0,5,'Вых.','','Вих.','');
INSERT INTO "lib_Controllers_io" VALUES('test','test','Test',0,1,'',0,6,'Тест','','Тест','');
INSERT INTO "lib_Controllers_io" VALUES('test','rez','Result',0,1,'',0,7,'Результат','','Результат','');
INSERT INTO "lib_Controllers_io" VALUES('test1','clc','Clock',2,0,'',0,0,'Часы','','Годинник','');
INSERT INTO "lib_Controllers_io" VALUES('test1','p_tm','Previous time (s)',1,1,'0',0,1,'Предыдущее время (с)','','Попередній час (с)','');
INSERT INTO "lib_Controllers_io" VALUES('test1','rez','Result',2,1,'0',0,2,'Результат','','Результат','');
INSERT INTO "lib_Controllers_io" VALUES('prescr','f_frq','Function calculate frequency (Hz)',2,0,'1000',0,0,'','','','');
INSERT INTO "lib_Controllers_io" VALUES('prescr','f_start','Function start flag',3,0,'0',0,1,'','','','');
INSERT INTO "lib_Controllers_io" VALUES('prescr','f_stop','Function stop flag',3,0,'0',0,2,'','','','');
INSERT INTO "lib_Controllers_io" VALUES('prescr','this','This controller object link',4,0,'0',0,3,'','','','');
INSERT INTO "lib_Controllers_io" VALUES('ntfDispatch','f_start','Function start flag',3,0,'',0,15,'','','','');
INSERT INTO "lib_Controllers_io" VALUES('ntfDispatch','emailTopic','EMail topic',0,0,'Notification',0,8,'','','','');
INSERT INTO "lib_Controllers_io" VALUES('ntfDispatch','emailMess','EMail message',0,0,'',0,9,'','','','');
INSERT INTO "lib_Controllers_io" VALUES('ntfDispatch','f_frq','Function calculate frequency (Hz)',2,0,'1000',0,17,'','','','');
INSERT INTO "lib_Controllers_io" VALUES('ntfDispatch','f_stop','Function stop flag',3,0,'0',0,16,'','','','');
INSERT INTO "lib_Controllers_io" VALUES('ntfDispatch','this','This controller object link',4,0,'0',0,18,'','','','');
INSERT INTO "lib_Controllers_io" VALUES('ntfDispatch','tmOut','Maximum notification timeout, seconds',1,0,'5',0,0,'','','','');
INSERT INTO "lib_Controllers_io" VALUES('ntfDispatch','SMSTrId','SMS serial transport, empty for disable',0,0,'SMS',0,11,'','','','');
INSERT INTO "lib_Controllers_io" VALUES('ntfDispatch','SMSPin','SMS pin, empty for disable',0,0,'1111',0,12,'','','','');
INSERT INTO "lib_Controllers_io" VALUES('test','f_frq','Function calculate frequency (Hz)',2,0,'1000',0,0,'','','','');
INSERT INTO "lib_Controllers_io" VALUES('test','f_start','Function start flag',3,0,'0',0,1,'','','','');
INSERT INTO "lib_Controllers_io" VALUES('test','f_stop','Function stop flag',3,0,'0',0,2,'','','','');
INSERT INTO "lib_Controllers_io" VALUES('test','this','This controller object link',4,0,'0',0,3,'','','','');
INSERT INTO "lib_Controllers_io" VALUES('ntfDispatch','emailServer','Output transport for SMTP connection, empty for disable',0,0,'Sockets.out_SMTP',0,4,'','','','');
INSERT INTO "lib_Controllers_io" VALUES('ntfDispatch','emailAuth','Auth, empty for disable',0,0,'user:pass',0,5,'','','','');
INSERT INTO "lib_Controllers_io" VALUES('ntfDispatch','emailSender','Sender',0,0,'noreply@oscada.org',0,6,'','','','');
INSERT INTO "lib_Controllers_io" VALUES('ntfDispatch','emailReceiver','Destination receiver address',0,0,'test@oscada.org',0,7,'','','','');
INSERT INTO "lib_Controllers_io" VALUES('ntfDispatch','SMSTel','SMS destination receiver, tel. number',0,0,'+380XXXXXXXXX',0,13,'','','','');
INSERT INTO "lib_Controllers_io" VALUES('ntfDispatch','emailState','Email notification current state',0,1,'',0,3,'','','','');
INSERT INTO "lib_Controllers_io" VALUES('ntfDispatch','SMSState','SMS notification current state',0,1,'',0,10,'','','','');
INSERT INTO "lib_Controllers_io" VALUES('ntfDispatch','SMSTextMd','SMS in text mode, else PDU',3,0,'0',0,14,'','','','');
INSERT INTO "lib_Controllers_io" VALUES('ntfDispatch','messLev','Messages level, negative for alarms',1,0,'1',0,1,'','','','');
INSERT INTO "lib_Controllers_io" VALUES('ntfDispatch','messCat','Messages category, template or regular expression',0,0,'al*:*',0,2,'','','','');
CREATE TABLE 'lib_servProc_io' ("F_ID" TEXT DEFAULT '' ,"ID" TEXT DEFAULT '' ,"NAME" TEXT DEFAULT '' ,"TYPE" INTEGER DEFAULT '' ,"MODE" INTEGER DEFAULT '' ,"DEF" TEXT DEFAULT '' ,"HIDE" INTEGER DEFAULT '' ,"POS" INTEGER DEFAULT '' ,"uk#NAME" TEXT DEFAULT '' ,"uk#DEF" TEXT DEFAULT '' ,"ru#NAME" TEXT DEFAULT '' ,"ru#DEF" TEXT DEFAULT '' , PRIMARY KEY ("F_ID","ID"));
INSERT INTO "lib_servProc_io" VALUES('procArh','fromarch','From archive',0,0,'Archive.va_ai3_code',0,0,'З архіву','','Из архива','');
INSERT INTO "lib_servProc_io" VALUES('procArh','toarch','To archive',0,0,'Archive.va_ai3_dP',0,1,'У архів','','В архив','');
INSERT INTO "lib_servProc_io" VALUES('procArh','begs','Begin time',0,0,'2006-08-08 9:21:56',0,2,'Час початку','','Время начала','');
INSERT INTO "lib_servProc_io" VALUES('procArh','ends','End time',0,0,'2006-08-08 11:21:55',0,3,'Час закінчення','','Время конца','');
INSERT INTO "lib_servProc_io" VALUES('crc16','in','Input',0,0,'',0,0,'Вхід','','Вход','');
INSERT INTO "lib_servProc_io" VALUES('crc16','out','Result',1,1,'',0,1,'Результат','','Результат','');
INSERT INTO "lib_servProc_io" VALUES('releaseTests','sub','Subsystem (-1:All;0:SYS;1:Security;2:Transport;3:A',1,0,'-1',0,0,'Підсистема (-1:Всі;0:SYS;1:Безпека;2:Транспорти;...)','','Подсистема (-1:Все;0:SYS;1:Безопасность;2:Транспорты;...)','');
INSERT INTO "lib_servProc_io" VALUES('releaseTests','rez','Result',0,1,'',0,1,'Результат','','Результат','');
INSERT INTO "lib_servProc_io" VALUES('crc16','poly','Polynomial (reversion)',1,0,'40961',0,2,'','','','');
INSERT INTO "lib_servProc_io" VALUES('archPackFStests','tm','Start time (14.03.2015 21:37)',1,0,'1426361839',0,0,'','','','');
INSERT INTO "lib_servProc_io" VALUES('archPackFStests','per','Period, seconds',1,0,'60',0,1,'','','','');
INSERT INTO "lib_servProc_io" VALUES('archPackFStests','addr','Archive address',0,0,'test123',0,2,'','','','');
INSERT INTO "lib_servProc_io" VALUES('archPackFStests','step','Step (0-8), -1 for all',1,0,'-1',0,4,'','','','');
INSERT INTO "lib_servProc_io" VALUES('archPackFStests','archiver','Archiver address',0,0,'FSArch.1m',0,3,'','','','');
INSERT INTO "lib_servProc_io" VALUES('wacko2media','ndb','DB',0,0,'wiki',0,0,'','','','');
INSERT INTO "lib_servProc_io" VALUES('wacko2media','wpg','Wiki page',0,0,'HomePageUk/Doc/SQLite',0,1,'','','','');
INSERT INTO "lib_servProc_io" VALUES('wacko2media','rez','Result',0,6,'',0,2,'','','','');
INSERT INTO "lib_servProc_io" VALUES('docOffLine','resDir','Results directory',0,0,'/home/roman/roman/work/Проекти/OpenSCADA/doc/',0,2,'','','','');
INSERT INTO "lib_servProc_io" VALUES('docOffLine','pages','Pages list of "{wiki}:{langs}:{dest}"',0,4,'Documents:en,uk,ru:index.html
About:en,uk,ru:About.html
Documents/Terms:en,uk,ru:Terms.html
Functions_and_demands:en,uk,ru:Functions_and_demands.html
Documents/FAQ:en,uk,ru:FAQ.html
Documents/DAQ:en,uk,ru:DAQ.html
Documents/Program_manual:en,uk,ru:Program_manual.html
Documents/User_API:en,uk,ru:User_API.html
Modules/SQLite:en,uk,ru:Modules/SQLite.html
Modules/MySQL:en,uk,ru:Modules/MySQL.html
Modules/FireBird:en,uk,ru:Modules/FireBird.html
Modules/DBF:en,uk,ru:Modules/DBF.html
Modules/PostgreSQL:en,uk,ru:Modules/PostgreSQL.html
Modules/Sockets:en,uk,ru:Modules/Sockets.html
Modules/Serial:en,uk,ru:Modules/Serial.html
Modules/SSL:en,uk,ru:Modules/SSL.html
Modules/SelfSystem:en,uk,ru:Modules/SelfSystem.html
Modules/UserProtocol:en,uk,ru:Modules/UserProtocol.html
Modules/HTTP:en,uk,ru:Modules/HTTP.html
Modules/JavaLikeCalc:en,uk,ru:Modules/JavaLikeCalc.html
Modules/LogicLev:en,uk,ru:Modules/LogicLev.html
Modules/BlockCalc:en,uk,ru:Modules/BlockCalc.html
Modules/DAQGate:en,uk,ru:Modules/DAQGate.html
Modules/System:en,uk,ru:Modules/System.html
Modules/ModBus:en,uk,ru:Modules/ModBus.html
Modules/DCON:en,uk,ru:Modules/DCON.html
Modules/OPC_UA:en,uk,ru:Modules/OPC_UA.html
Modules/SNMP:en,uk,ru:Modules/SNMP.html
Modules/ICP_DAS:en,uk,ru:Modules/ICP_DAS.html
Modules/Siemens:en,uk,ru:Modules/Siemens.html
Modules/DiamondBoards:en,uk,ru:Modules/DiamondBoards.html
Modules/Comedi:en,uk,ru:Modules/Comedi.html
Modules/FSArch:en,uk,ru:Modules/FSArch.html
Modules/DBArch:en,uk,ru:Modules/DBArch.html
Modules/VCAEngine:en,ru:Modules/VCAEngine.html
Modules/QTStarter:en,uk,ru:Modules/QTStarter.html
Modules/QTCfg:en,uk,ru:Modules/QTCfg.html
Modules/FLibSYS:en,uk,ru:Modules/FLibSYS.html
Modules/SystemTests:en,uk,ru:Modules/SystemTests.html
Modules/FLibMath:en,uk,ru:Modules/FLibMath.html
Modules/FLibComplex1:en,uk,ru:Modules/FLibComplex1.html',0,3,'','','','');
INSERT INTO "lib_servProc_io" VALUES('docOffLine','pagesCur','Current pages list of "{wiki}:{langs}:{dest}", for empty here used "pages"',0,4,'',0,4,'','','','');
INSERT INTO "lib_servProc_io" VALUES('docOffLine','res','Result',0,1,'',0,0,'','','','');
INSERT INTO "lib_servProc_io" VALUES('docOffLine','reqTr','Tries of requests',1,1,'3',0,1,'','','','');
CREATE TABLE 'techApp' ("ID" TEXT DEFAULT '' ,"NAME" TEXT DEFAULT '' ,"DESCR" TEXT DEFAULT '' ,"MAXCALCTM" INTEGER DEFAULT '' ,"FORMULA" TEXT DEFAULT '' ,"uk#NAME" TEXT DEFAULT '' ,"uk#DESCR" TEXT DEFAULT '' ,"ru#NAME" TEXT DEFAULT '' ,"ru#DESCR" TEXT DEFAULT '' , PRIMARY KEY ("ID"));
INSERT INTO "techApp" VALUES('lag','Lag','Lag model. You can use this for sensors'' variables lag imitation.',10,'out-=(out-in)/(t_lg*f_frq);','Затримка','Модель затримки. Може використовуватися для імітації запізнення значень давачів.','Запаздывание','Модель задержки. Может использоваться для имитации запаздывания
значений датчиков.
');
INSERT INTO "techApp" VALUES('noise','Noise (2 harmonic + rand)','Noise model. Contain three parts:
- first harmonic part;
- second harmonic part;
- noise based on randomize generator of numbers.',10,'tmp_g1=(tmp_g1>6.28)?0:tmp_g1+6.28/(per_g1*f_frq);
tmp_g2=(tmp_g2>6.28)?0:tmp_g2+6.28/(per_g2*f_frq);
out=off+a_g1*sin(tmp_g1)+a_g2*sin(tmp_g2)+a_rnd*(rand(2)-1);','Шум (2 гарм. + випадк.)','Модель шуму. Містить три складові:
- перша гармоніка;
- друга гармоніка;
- шум на основі генератору випадкових чисел.','Шум (2 гарм. + случ)','Модель шума. Содержит три составляющие:
- первая гармоника;
- вторая гармоника;
- шум на основе генератора случайных чисел.');
INSERT INTO "techApp" VALUES('ballCrane','Ball crane','Ball crane model. Include going and estrangement time.',10,'if( !(st_close && !com) && !(st_open && com) )
{
  tmp_up=(pos>0&&pos<100)?0:(tmp_up>0&&lst_com==com)?tmp_up-1./f_frq:t_up;
  pos+=(tmp_up>0)?0:(100.*(com?1.:-1.))/(t_full*f_frq);
  pos=(pos>100)?100:(pos<0)?0:pos;
  st_open=(pos>=100)?true:false;
  st_close=(pos<=0)?true:false;
  lst_com=com;
}','Кульовий кран','Модель кульового крану. Включає час ходу та час відриву.','Шаровый кран','Модель шарового крана. Включает время хода и время отрыва.');
INSERT INTO "techApp" VALUES('separator','Separator','Separator model included two phase: liquid and gas.',10,'Fж=max(0,Fi*ProcЖ);
DAQ.JavaLikeCalc.lib_techApp.pipeBase(Fi,Pi,293,Si,Fo+Fж,Po,293,So,lo,Q0,0.95,0.01,f_frq);
Lж = max(0,min(100,Lж+0.27*(Fж-Fo_ж)/(Vap*Qж*f_frq)));
Po_ж = Po + Lж*Vap/Qж;','Сепаратор','Модель сепаратору з двома фазами рідинною та газовою.','Сепаратор','Модель сепаратора с двумя фазами, жидкой и газовой.');
INSERT INTO "techApp" VALUES('klap','Valve','Valve model, include:
- two valve in one;
- super-critical speed;
- temperature change on baffling;
- work to one side, back valve;
- valve position speed control;
- nonlinear cut changing by open position.',10,'Qr=Q0+Q0*Kpr*(Pi-1);
tmp_l1 += (abs(l_kl1-tmp_l1) > 5) ? 100*sign(l_kl1-tmp_l1)/(t_kl1*f_frq) : (l_kl1-tmp_l1)/(t_kl1*f_frq);
tmp_l2 += (abs(l_kl2-tmp_l2) > 5) ? 100*sign(l_kl2-tmp_l2)/(t_kl2*f_frq) : (l_kl2-tmp_l2)/(t_kl2*f_frq);
Sr=(S_kl1*pow(tmp_l1,Kln)+S_kl2*pow(tmp_l2,Kln))/pow(100,Kln);

DAQ.JavaLikeCalc.lib_techApp.pipeBase(Fi,Pi,Ti,Sr,EVAL_REAL,Po,293,So,lo,Q0,Kpr,0.01,f_frq);
if( noBack ) Fi = max(0,Fi);
Po = max(0,min(100,Po+0.27*(Fi-Fo)/(Q0*Kpr*So*lo*f_frq)));

To = max(0,min(2e3,To+(abs(Fi)*(Ti*pow(Po/Pi,0.02)-To)+(Fwind+1)*(Twind-To)/Riz)/(Ct*So*lo*Qr*f_frq)));','Засувка','Модель клапану яка враховує:
- два клапана в одному; 
- зверхкритичне витікання; 
- зміна температури при дроселюванні; 
- робота тільки у одному напрямку, зворотний клапан;
- керування швидкісттю зміни положення;
- нелінійність прохідного перетину від положення.','Задвижка','Модель клапана, учитывающая:
- два клапана в одном; 
- сверхкритическое истечение; 
- изменение температуры при дросселировании; 
- работа только в одном направлении, обратный клапан; 
- управляемая скорость изменения положения; 
- нелинейность проходного сечения от положения.');
INSERT INTO "techApp" VALUES('lagClean','Lag (clear)','Model of clear lag (transportable). Realised by include some simple lag links. Appointed for lags into long pipes.',10,'cl1-=(cl1-in)/(t_lg*f_frq/4);
cl2-=(cl2-cl1)/(t_lg*f_frq/4);
cl3-=(cl3-cl2)/(t_lg*f_frq/4);
out-=(out-cl3)/(t_lg*f_frq/4);','Затримка (чиста)','Модель чистої(транспортної) затримки. Реалізується шляхом включення декількох ланок простої затримки. Призначено для імітації затримок у довгих трубопроводах.','Запаздывание (чистое)','Модель чистого(транспортного) запаздывания. Реализуется путём включения нескольких простых звеньев запаздывания. Предназначен для имитации запаздывания в длинных трубопроводах.');
INSERT INTO "techApp" VALUES('net','Network (loading)','Loading with constant preasure on network. Contain parameter for noise connection.',10,'DAQ.JavaLikeCalc.lib_techApp.pipeBase(Fi,Pi,293,So,EVAL_REAL,Po,293,So,10,Q0,Kpr,0.01,f_frq);','Мережа (навантаження)','Навантаження з фіксованим тиском мережі. Містить параметр для підключення шуму.','Сеть (нагрузка)','Нагрузка с фиксированным давлением сети. Содержит параметр для подключения шума.');
INSERT INTO "techApp" VALUES('src_press','Source (pressure)','Source pressure with constant pressure. Contained the parameter for noise connection.',10,'DAQ.JavaLikeCalc.lib_techApp.pipeBase(Fit,Pi*Noise,293,So,Fo,Po,293,So,lo,Q0,Kpr,0.01,f_frq);','Джерело (тиск)','Джерело з фіксованим тиском. Містить параметр для підключення шуму.','Источник (давление)','Источник с фиксированным давлением. Содержит параметр для подключения шума.');
INSERT INTO "techApp" VALUES('cooler','Air cooler','Model of the air cooler for gas flow.',10,'DAQ.JavaLikeCalc.lib_techApp.pipeBase(Fi,Pi,293,Si,Fo,Po,293,So,lo,Q0,0.95,0.01,f_frq);
Qr = Q0+Q0*0.95*(Pi-1);
To+=(Fi*(Ti-To)+Wc*(Tair-To)/Rt)/(Ct*(Si*li+So*lo)*Qr*f_frq);','Повітряний холодильник','Модель повітряного охолоджувача газового потоку.','Возд. холодильник','Модель воздушного охладителя газового потока.');
INSERT INTO "techApp" VALUES('compressor','Gas compressor','Model of the gas compressor. Realised surge effect.
Sarge count from the dynamic-gas curve, and next count coefficient of sarge margin.',10,'Pmax = max(Pi,Po);
Pmin = min(Pi,Po);
Qr = Q0+Q0*Kpr*(Pi-1);
Qrf = Q0+Q0*Kpr*(Pmax-1);
Ftmp=(N>0.1)?(1-10*(Po-Pi)/(Qr*(pow(N,3)+0.1)*Kpmp)):1;
Kzp=1-Ftmp;  //Коэффиц. запаса
Fi=V*N*Qr*sign(Ftmp)*pow(abs(Ftmp),Kslp)+
     0.3*(4*So*Qrf/(0.01*lo*1.7724+4*Qrf))*sign(Pi-Po)*pow(Qrf*(Pmax-max(Pmax*0.528,Pmin)),0.5);
Fit -= (Fit-Fi)/max(1,(lo*f_frq)/max(1e-4,abs(Fi/(Qrf*So))));
Po = max(0,min(100,Po+0.27*(Fi-Fo)/(Q0*Kpr*So*lo*f_frq)));

To+=(abs(Fi)*(Ti*pow(Po/Pi,0.3)-To)+(Fwind+1)*(Twind-To)/Riz)/(Ct*(V+So*lo)*Qr*f_frq);','Компресор газовий','Модель газового компресора. Враховує ефект помпажу. 
Помпаж розраховується за газово-динамічною кривою, виходячи з якої розраховується коефіцієнт запасу по помпажу.','Компрессор газовый','Модель газового компрессора. Учитывает эффект помпажа. Помпаж вычисляется по газо-динамической кривой, исходя из которой получается коэффициент запаса по помпажу.');
INSERT INTO "techApp" VALUES('src_flow','Source (flow)','Source of constant flow. Contained parameter for noise connection.',10,'Po = max(0,min(100,Po+0.27*(Noise*Fi-Fo)/(Q0*Kpr*So*lo*f_frq)));','Джерело (витрати)','Джерело з фіксованими витратами. Містить параметр для підключення шуму.','Источник (расход)','Источник с фиксированным расходом. Содержит параметр для подключения шума.');
INSERT INTO "techApp" VALUES('pipe1_1','Pipe 1->1','Model of the pipe by scheme: 1 -> 1.',10,'DAQ.JavaLikeCalc.lib_techApp.pipeBase(Fi,Pi,293,So,Ft1,Pti,293,So,0.33*lo,Q0,Kpr,0.01,f_frq);
DAQ.JavaLikeCalc.lib_techApp.pipeBase(Ft1,Pti,293,So,Fto,Pt1,293,So,0.33*lo,Q0,Kpr,0.01,f_frq);
DAQ.JavaLikeCalc.lib_techApp.pipeBase(Fto,Pt1,293,So,Fo,Po,293,So,0.33*lo,Q0,Kpr,0.01,f_frq);','Труба 1->1','Модель вузла труб за схемою: 1 -> 1.','Труба 1->1','Модель узла труб по схеме: 1 -> 1.');
INSERT INTO "techApp" VALUES('pipe1_2','Pipe 1->2','Model of the pipe by scheme: 1 -> 2.',10,'DAQ.JavaLikeCalc.lib_techApp.pipeBase(F1tmp,Pi,293,So1,Fo1,Po1,293,So1,lo1,Q0,Kpr,0.01,f_frq);
DAQ.JavaLikeCalc.lib_techApp.pipeBase(F2tmp,Pi,293,So2,Fo2,Po2,293,So2,lo2,Q0,Kpr,0.01,f_frq);
Fi=F1tmp+F2tmp;','Труба 1->2','Модель вузла труб за схемою: 1 -> 2.','Труба 1->2','Модель узла труб по схеме: 1 -> 2.');
INSERT INTO "techApp" VALUES('pipe1_3','Pipe 1->3','Model of the pipe by scheme: 1 -> 3.',10,'DAQ.JavaLikeCalc.lib_techApp.pipeBase(F1tmp,Pi,293,So1,Fo1,Po1,293,So1,lo1,Q0,Kpr,0.01,f_frq);
DAQ.JavaLikeCalc.lib_techApp.pipeBase(F2tmp,Pi,293,So2,Fo2,Po2,293,So2,lo2,Q0,Kpr,0.01,f_frq);
DAQ.JavaLikeCalc.lib_techApp.pipeBase(F3tmp,Pi,293,So3,Fo3,Po3,293,So3,lo3,Q0,Kpr,0.01,f_frq);
Fi=F1tmp+F2tmp+F3tmp;','Труба 1->3','Модель вузла труб за схемою: 1 -> 3.','Труба 1->3','Модель узла труб по схеме: 1 -> 3.');
INSERT INTO "techApp" VALUES('pipe1_4','Pipe 1->4','Model of the pipe by scheme: 1 -> 4.',10,'DAQ.JavaLikeCalc.lib_techApp.pipeBase(F1tmp,Pi,293,So1,Fo1,Po1,293,So1,lo1,Q0,Kpr,0.01,f_frq);
DAQ.JavaLikeCalc.lib_techApp.pipeBase(F2tmp,Pi,293,So2,Fo2,Po2,293,So2,lo2,Q0,Kpr,0.01,f_frq);
DAQ.JavaLikeCalc.lib_techApp.pipeBase(F3tmp,Pi,293,So3,Fo3,Po3,293,So3,lo3,Q0,Kpr,0.01,f_frq);
DAQ.JavaLikeCalc.lib_techApp.pipeBase(F4tmp,Pi,293,So4,Fo4,Po4,293,So4,lo4,Q0,Kpr,0.01,f_frq);
Fi=F1tmp+F2tmp+F3tmp+F4tmp;','Труба 1->4','Модель вузла труб за схемою: 1 -> 4.','Труба 1->4','Модель узла труб по схеме: 1 -> 4.');
INSERT INTO "techApp" VALUES('klapMech','Valve proc. mechanism','Model of the valve process mechanism.
Include going time (aperiodic links of two level) and estrangement time.',10,'if( (pos >= 99 && com >= 99) || (pos <= 1 && com <=1 ) )
{ 
  tmp_up = t_up;
  if(pos>=99) { pos=100; st_open=true; }
  else { pos = 0; st_close=true; }
}
else if( tmp_up > 0 ) tmp_up-=1./f_frq;
else
{
  st_open=st_close=false;
  lst_com+=(com-lst_com)/(0.5*t_full*f_frq);
  pos+=(lst_com-pos)/(0.5*t_full*f_frq);
}
pos_sensor+=(pos-pos_sensor)/(t_sensor*f_frq);','Виконавчий мех. клапану','Модель виконавчого механізму клапана. Включає час ходу та час відриву.','Исполн. мех. клапана','Модель исполнительного механизма клапана. Включает время хода и время отрыва.');
INSERT INTO "techApp" VALUES('diafragma','Diaphragm','Diaphragm model.',10,'DAQ.JavaLikeCalc.lib_techApp.pipeBase(Fi,Pi,293,Sdf,Fo,Po,293,So,lo,Q0,Kpr,0.01,f_frq);
dP -= (dP-100*(Pi-Po))/f_frq;','Діафрагма','Модель діафрагми.','Диафрагма','Модель диафрагмы.');
INSERT INTO "techApp" VALUES('pipe3_1','Pipe 3->1','Model of the pipe by scheme: 3 -> 1.',10,'DAQ.JavaLikeCalc.lib_techApp.pipeBase(Fi1,Pi1,293,Si1,EVAL_REAL,Po,293,So,lo,Q0,Kpr,0.01,f_frq);
DAQ.JavaLikeCalc.lib_techApp.pipeBase(Fi2,Pi2,293,Si2,EVAL_REAL,Po,293,So,lo,Q0,Kpr,0.01,f_frq);
DAQ.JavaLikeCalc.lib_techApp.pipeBase(Fi3,Pi3,293,Si3,EVAL_REAL,Po,293,So,lo,Q0,Kpr,0.01,f_frq);
Po = max(0,min(100,Po+0.27*(Fi1+Fi2+Fi3-Fo)/(Q0*Kpr*So*lo*f_frq)));
To = max(0,To+(Fi1*(Ti1-To)+Fi2*(Ti2-To)+Fi3*(Ti3-To)+(Fwind+1)*(Twind-To)/Riz)/(Ct*So*lo*Q0*f_frq));','Труба 3->1','Модель вузла труб за схемою: 3 -> 1.','Труба 3->1','Модель узла труб по схеме: 3 -> 1.');
INSERT INTO "techApp" VALUES('pipeBase','Pipe-base','Implementation of the basic foundations of the model pipe:
- Flow in the pipe, taking into account the speed, pressure drop, resistance due to friction and the critical flow.
- Calculation of pressure.
- Accounting for medium density and degree of compressibility for both gases and liquids.',10,'Pmax = max(Pi,Po);
Pmin = min(Pi,Po);
Qr = Q0+Q0*Kpr*(Pmax-1);
Fit = 630*(4*Si*So*Qr/(Ktr*lo*1.7724*Si+4*So*Qr))*sign(Pi-Po)*pow(Qr*(Pmax-max(Pmax*0.528,Pmin)),0.5);
Fi -= (Fi-Fit)/max(1,(lo*f_frq)/max(1,abs(Fit/(Qr*So))));
if( !Fo.isEVal() ) Po = max(0,min(100,Po+0.27*(Fi-Fo)/(Q0*Kpr*So*lo*f_frq)));','Труба-база','Реалізація базових основ моделі труби:
- Витрати у трубі з урахуванням швидкості руху, різниці тиску, опору за рахунок тертя та критичної течі.
- Розрахунок тиску. 
- Урахування щільності середовища та ступеня його стиснення як для газів, так і для рідин.','Труба-база','Реализация базовых основ модели трубы:
- Расход в трубе с учётом скорости движения, перепада давления, сопротивления за счёт трения и критического истечения.
- Расчёт давления.
- Учёт плотности среды и степени её сжимаемости как для газов, так и для жидкостей.');
INSERT INTO "techApp" VALUES('heatExch','Heat exchanger','The model of the heat exchanger, it calculates the heat exchange of the two streams.',10,'DAQ.JavaLikeCalc.lib_techApp.pipeBase(Fi1,Pi1,Ti1,Si1,Fo1,Po1,293,So1,lo1,Q0i1,Kpr1,0.01,f_frq);
DAQ.JavaLikeCalc.lib_techApp.pipeBase(Fi2,Pi2,Ti2,Si2,Fo2,Po2,293,So2,lo2,Q0i2,Kpr2,0.01,f_frq);

To1=max(0,min(1e4,(Fi1*Ti1*Ci1+ki*Fi2*Ti2*Ci2)/(Fi1*Ci1+ki*Fi2*Ci2)));
To2=max(0,min(1e4,(ki*Fi1*Ti1*Ci1+Fi2*Ti2*Ci2)/(ki*Fi1*Ci1+Fi2*Ci2)));','Теплообмінник','Модель теплообміннику, яка розраховує теплообмін двох потоків.','Теплообменник','Модель теплообменника, рассчитывающая теплообмен двух потоков.');
INSERT INTO "techApp" VALUES('boilerBarrel','Boiler: barrel','The model of the boiler''s barrel. ',10,'// Water
DAQ.JavaLikeCalc.lib_techApp.pipeBase(Fi1,Pi1,293,Si1,EVAL_REAL,Po1,293,So1,lo1,1e3,0.001,0.01,f_frq);
Fi1 = max(0,Fi1);

// Steam
Lo = max(0,min(100,Lo+(Fi1-Fpara)*100/(Vi1*1000*f_frq)));
To1 = (100*pow(Po1,0.241)+5)+273;

if( Tv<To1 )
{
  Tv+=(k*S*(Ti2-Tv)-Fi1*0.00418*(Tv-Ti1))/f_frq;
  Fpara=0;
}
if( Tv >= To1 )
{
  Tv=To1;
  Lambda=2750.0-0.00418*(Tv-273);
  Fpara=(5*S*Fi2*(Ti2-Tv)-Fi1*0.00418*(Tv-Ti1))/(Po1*Lambda);
}

To2=Ti2-Tv/k;
Po1 = max(0,min(100,Po1+0.27*(Fpara-Fo)/(1.2*0.98*((1-Lo/100)*Vi1+So1*lo1)*f_frq)));

// Smoke gas
DAQ.JavaLikeCalc.lib_techApp.pipeBase(Fi2,Pi2,293,Si2,Fo2,Po2,293,Si2,30,1.2,0.98,0.01,f_frq);','Котел: барабан','Модель барабану котлоагрегату.','Котёл: барабан','Модель барабана котлоагрегата.');
INSERT INTO "techApp" VALUES('boilerBurner','Boiler: burner','The fire chamber''s of the boiler model which works with three fuels: blast-furnace gas, coke and natural gas. ',10,'using DAQ.JavaLikeCalc.lib_techApp;
pipeBase(Fi1,Pi1,Ti1,Si1,EVAL_REAL,Po,293,So,lo,1.2,0.95,0.01,f_frq); Fi1 = max(0,Fi1);
pipeBase(Fi2,Pi2,Ti2,Si2,EVAL_REAL,Po,293,So,lo,0.7,0.95,0.01,f_frq); Fi2 = max(0,Fi2);
pipeBase(Fi3,Pi3,Ti3,Si3,EVAL_REAL,Po,293,So,lo,1.33,0.95,0.01,f_frq); Fi3 = max(0,Fi3);
pipeBase(Fi4,Pi4,Ti4,Si4,EVAL_REAL,Po,293,So,lo,1.293,0.95,0.01,f_frq); Fi4 = max(0,Fi4);

Neobhod_vzd = Fi1+10*Fi2+4*Fi3;
F_DG = Fi1+Fi2+Fi3+Fi4;
O2 = max(0,min(100,(Fi4-Neobhod_vzd)*100/F_DG));
CO = min(100, (O2<1) ? (1.2*abs(O2)) : 0);
koef = min(1,Fi4/Neobhod_vzd);
Q = koef*(8050*Fi2+3900*Fi3+930*Fi1);
delta_t = Q/(F_DG*1.047);
To = max(0,min(2000,(delta_t+(Ti4-273)+(Ti3-273)*(Fi3/Fi1)+(Ti2-273)*(Fi2/Fi1)+(Ti1-273)*(Fi1/Fi4))+273));

Po = max(0,min(10,Po+0.27*(F_DG-Fo)/(1.2*0.95*(So*lo+V)*f_frq)));','Котел: топка','Модель топки котлоагрегату, який працює на трьох видах палива: доменному, коксовому та природному газах.','Котёл: топка','Модель топки котлоагрегата, работающего на трех видах топлива: доменном, коксовом и природном газе.');
INSERT INTO "techApp" VALUES('pipe2_1','Pipe 2->1','Model of the pipe by scheme: 2 -> 1.',10,'DAQ.JavaLikeCalc.lib_techApp.pipeBase(Fi1,Pi1,293,Si1,EVAL_REAL,Po,293,So,lo,Q0,Kpr,0.01,f_frq);
DAQ.JavaLikeCalc.lib_techApp.pipeBase(Fi2,Pi2,293,Si2,EVAL_REAL,Po,293,So,lo,Q0,Kpr,0.01,f_frq);
Po = max(0,min(100,Po+0.27*(Fi1+Fi2-Fo)/(Q0*Kpr*So*lo*f_frq)));
To = max(0,To+(Fi1*(Ti1-To)+Fi2*(Ti2-To)+(Fwind+1)*(Twind-To)/Riz)/(Ct*So*lo*Q0*f_frq));','Труба 2->1','Модель вузла труб за схемою: 2 -> 1.','Труба 2->1','Модель узла труб по схеме: 2 -> 1.');
CREATE TABLE 'techApp_io' ("F_ID" TEXT DEFAULT '' ,"ID" TEXT DEFAULT '' ,"NAME" TEXT DEFAULT '' ,"TYPE" INTEGER DEFAULT '' ,"MODE" INTEGER DEFAULT '' ,"DEF" TEXT DEFAULT '' ,"HIDE" INTEGER DEFAULT '' ,"POS" INTEGER DEFAULT '' ,"uk#NAME" TEXT DEFAULT '' ,"ru#NAME" TEXT DEFAULT '' ,"ru#DEF" TEXT DEFAULT '' ,"uk#DEF" TEXT DEFAULT '' , PRIMARY KEY ("F_ID","ID"));
INSERT INTO "techApp_io" VALUES('lag','out','Output',2,2,'0',0,0,'Вихід','Выход','','');
INSERT INTO "techApp_io" VALUES('lag','in','Input',2,0,'0',0,1,'Вхід','Вход','','');
INSERT INTO "techApp_io" VALUES('lag','t_lg','Lag time (s)',2,0,'10',0,2,'Час затримки (с)','Время запазд. (с)','','');
INSERT INTO "techApp_io" VALUES('lag','f_frq','Calc frequency (Hz)',2,0,'100',1,3,'Частота обчислення (Гц)','Частота вычисления (Гц)','','');
INSERT INTO "techApp_io" VALUES('noise','out','Output',2,2,'0',0,0,'Вихід','Выход','','');
INSERT INTO "techApp_io" VALUES('noise','off','Main offset',2,0,'1',0,1,'Загальний зсув','Общее смещение','','');
INSERT INTO "techApp_io" VALUES('noise','a_g1','Harmonic part 1 amplitude',2,0,'10',0,2,'Амплітуда гармоніки 1','Амплитуда гармоники 1','','');
INSERT INTO "techApp_io" VALUES('noise','per_g1','Harmonic part 1 period (s)',2,0,'10',0,3,'Період гармоніки 1 (сек)','Период гармоники 1 (сек)','','');
INSERT INTO "techApp_io" VALUES('noise','a_g2','Harmonic part 2 amplitude',2,0,'5',0,4,'Амплітуда гармоніки 2','Амплитуда гармоники 2','','');
INSERT INTO "techApp_io" VALUES('noise','per_g2','Harmonic part 2 period (s)',2,0,'0.1',0,5,'Період гармоніки 2 (сек)','Период гармоники 2 (сек)','','');
INSERT INTO "techApp_io" VALUES('noise','a_rnd','Random numbers amplitude',2,0,'1',0,6,'Амплітуда випадкових значень','Амплитуда случ. значений','','');
INSERT INTO "techApp_io" VALUES('noise','f_frq','Calc function period (Hz)',2,0,'100',1,7,'Частота обчислення функції (Гц)','Частота обсчёта функции (Гц)','','');
INSERT INTO "techApp_io" VALUES('noise','tmp_g1','Harmonic part 1 counter',2,0,'0',1,8,'Лічильник гармоніки 1','Счётчик гармоники 1','','');
INSERT INTO "techApp_io" VALUES('noise','tmp_g2','Harmonic part 2 counter',2,0,'0',1,9,'Лічильник гармоніки 2','Счётчик гармоники 2','','');
INSERT INTO "techApp_io" VALUES('ballCrane','pos','Position (%)',2,1,'0',0,0,'Положення (%)','Положение (%)','','');
INSERT INTO "techApp_io" VALUES('ballCrane','com','Command',3,0,'0',0,1,'Команда','Команда','','');
INSERT INTO "techApp_io" VALUES('ballCrane','st_open','State "Opened"',3,1,'0',0,2,'Стан "Відкрито"','Сост. "Открыто"','','');
INSERT INTO "techApp_io" VALUES('ballCrane','st_close','State "Closed"',3,1,'1',0,3,'Стан "Закрито"','Сост. "Закрыто"','','');
INSERT INTO "techApp_io" VALUES('ballCrane','t_full','Going time (s)',2,0,'5',0,4,'Час ходу (с)','Время хода (с)','','');
INSERT INTO "techApp_io" VALUES('ballCrane','t_up','Estrangement time (s)',2,0,'0.5',0,5,'Час зриву (с)','Время срыва (с)','','');
INSERT INTO "techApp_io" VALUES('ballCrane','f_frq','Calc frequency (Hz)',2,0,'100',1,6,'Частота обчислення (Гц)','Частота вычисления (Гц)','','');
INSERT INTO "techApp_io" VALUES('ballCrane','tmp_up','Estrangement counter',2,0,'0',1,7,'Лічильник зриву','Счётчик срыва','','');
INSERT INTO "techApp_io" VALUES('ballCrane','lst_com','Last command',3,0,'0',1,8,'Остання команда','Последняя команда','','');
INSERT INTO "techApp_io" VALUES('separator','Fi','Input flow (tones/h)',2,1,'0',0,0,'Вхідн. витрати (т/год)','Входн. расход (т/ч)','','');
INSERT INTO "techApp_io" VALUES('separator','Pi','Input pressure (at)',2,0,'1',0,1,'Вхідн. тиск (ата)','Входн. давление (ата)','','');
INSERT INTO "techApp_io" VALUES('separator','Si','Input cutset (m2)',2,0,'0.2',0,2,'Вхідн. перетин (м2)','Входн. сечение (м2)','','');
INSERT INTO "techApp_io" VALUES('separator','Fo','Output flow (tones/h)',2,0,'0',0,3,'Вих. витрати (т/год)','Вых. расход (т/ч)','','');
INSERT INTO "techApp_io" VALUES('separator','Po','Output pressure (at)',2,1,'1',0,4,'Вих. тиск (ата)','Вых. давление (ата)','','');
INSERT INTO "techApp_io" VALUES('separator','So','Output cutset (m2)',2,0,'0.2',0,5,'Вих. перетин (м2)','Вых. сечение (м2)','','');
INSERT INTO "techApp_io" VALUES('separator','lo','Output length (m)',2,0,'10',0,6,'Вих. довжина (м)','Вых. длина (м)','','');
INSERT INTO "techApp_io" VALUES('separator','Fo_ж','Output liquid flow (tones/h)',2,0,'0',0,7,'Вих. витрати рідини (т/год)','Вых. расход жидкости (т/ч)','','');
INSERT INTO "techApp_io" VALUES('separator','Po_ж','Output liquid pressure (ata)',2,1,'1',0,8,'Вих. тиск рідини (ата)','Вых. давление жидкости (ата)','','');
INSERT INTO "techApp_io" VALUES('separator','Lж','Liquid level (%)',2,1,'0',0,9,'Рівень рідини (%)','Уровень жидкости (%)','','');
INSERT INTO "techApp_io" VALUES('separator','ProcЖ','% liquid.1',2,0,'0.01',0,10,'% рідини.','% жидкости.','','');
INSERT INTO "techApp_io" VALUES('separator','Vap','Device capacity (m3)',2,0,'10',0,11,'Об''єм апарату (м3)','Объём аппарата (м3)','','');
INSERT INTO "techApp_io" VALUES('separator','Q0','Norm density of environs (kg/m3)',2,0,'1',0,12,'Норм. щільність середовища (кг/м3)','Норм. плотн. среды (кг/м3)','','');
INSERT INTO "techApp_io" VALUES('separator','Qж','Liquid density (kg/m3)',2,0,'1000',0,13,'Щільність рідини (кг/м3)','Плотн. жидкости (кг/м3)','','');
INSERT INTO "techApp_io" VALUES('separator','f_frq','Calc frequency (Hz)',2,0,'200',1,14,'Частота обчислення (Гц)','Частота вычисления (Гц)','','');
INSERT INTO "techApp_io" VALUES('klap','t_kl1','Valve 1 open time (s)',2,0,'10',0,10,'Час відкриття клапану 1 (с)','Valve 1 open time (s)','','');
INSERT INTO "techApp_io" VALUES('klap','t_kl2','Valve 2 open time (s)',2,0,'5',0,13,'Час відкриття клапану 2 (с)','Valve 2 open time (s)','','');
INSERT INTO "techApp_io" VALUES('klap','tmp_l1','Position 1 lag',2,1,'0',1,23,'Затримка положення 1','Задержка положения 1','','');
INSERT INTO "techApp_io" VALUES('klap','tmp_l2','Position 2 lag',2,1,'0',1,24,'Затримка положення 2','Задержка положения 2','','');
INSERT INTO "techApp_io" VALUES('klap','Fi','Input flow (tones/h)',2,1,'0',0,0,'Вхідн. витрати (т/год)','Входн. расход (т/ч)','','');
INSERT INTO "techApp_io" VALUES('klap','Pi','Input pressure (at)',2,0,'1',0,1,'Вхідн. тиск (ата)','Входн. давление (ата)','','');
INSERT INTO "techApp_io" VALUES('klap','Ti','Input temperature (K)',2,0,'273',0,2,'Вхідн. температура (К)','Входн. температура (К)','','');
INSERT INTO "techApp_io" VALUES('klap','Fo','Output flow (tones/h)',2,0,'0',0,3,'Вих. витрати (т/год)','Вых. расход (т/ч)','','');
INSERT INTO "techApp_io" VALUES('klap','Po','Output pressure (at)',2,1,'1',0,4,'Вих. тиск (ата)','Вых. давление (ата)','','');
INSERT INTO "techApp_io" VALUES('klap','To','Output temperature (K)',2,1,'273',0,5,'Вих. температура (К)','Вых. температура (К)','','');
INSERT INTO "techApp_io" VALUES('klap','So','Output pipe cutset (m2)',2,0,'.2',0,6,'Вих. перетин труби (м2)','Вых. сечение трубы (м2)','','');
INSERT INTO "techApp_io" VALUES('klap','lo','Output pipe length (m)',2,0,'10',0,7,'Вих. довжина труби (м)','Вых. длина трубы (м)','','');
INSERT INTO "techApp_io" VALUES('klap','S_kl1','Valve 1 cutset (m2)',2,0,'.1',0,8,'Перетин клапану 1 (м2)','Сечение клапана 1 (м2)','','');
INSERT INTO "techApp_io" VALUES('klap','S_kl2','Valve 2 cutset (m2)',2,0,'.05',0,11,'Перетин клапану 2 (м2)','Сечение клапана 2 (м2)','','');
INSERT INTO "techApp_io" VALUES('klap','l_kl1','Valve 1 open (%)',2,0,'0',0,9,'Полож. клапану 1 (%)','Полож. клапана 1 (%)','','');
INSERT INTO "techApp_io" VALUES('klap','l_kl2','Valve 2 open (%)',2,0,'0',0,12,'Полож. клапану 2 (%)','Полож. клапана 2 (%)','','');
INSERT INTO "techApp_io" VALUES('klap','Q0','Norm density of environs (kg/m3)',2,0,'1',0,14,'Норм. щільність середовища (кг/м3)','Норм. плотн. ср. (кг/м3)','','');
INSERT INTO "techApp_io" VALUES('klap','Kln','Linearity coefficient',2,0,'1',0,15,'Коефіц. нелінійності','Коэфиц. нелинейности','','');
INSERT INTO "techApp_io" VALUES('klap','Kpr','Compressibility coefficient (0...1)',2,0,'0.95',0,16,'Коефіц. стиснення середовища (0...1)','Коэфиц. сжим. среды (0...1)','','');
INSERT INTO "techApp_io" VALUES('klap','Ct','Warm capacity of environs',2,0,'20',0,17,'Теплоемність середовища','Теплоёмкость среды','','');
INSERT INTO "techApp_io" VALUES('klap','Riz','Warm resistance of isolation',2,0,'20',0,18,'Тепл. опір ізоляції','Тепл. сопр. изоляции','','');
INSERT INTO "techApp_io" VALUES('klap','noBack','Back valve',3,0,'0',0,19,'Зворотній клапан','Обратный клапан','','');
INSERT INTO "techApp_io" VALUES('klap','Fwind','Air speed',2,0,'1',0,20,'Швидкість повітря','Скорость воздуха','','');
INSERT INTO "techApp_io" VALUES('klap','Twind','Air temperature',2,0,'273',0,21,'Темпер. повітря','Темпер. воздуха','','');
INSERT INTO "techApp_io" VALUES('klap','f_frq','Calc frequency (Hz)',2,0,'200',1,22,'Частота обчислення (Гц)','Частота вычисления (Гц)','','');
INSERT INTO "techApp_io" VALUES('src_press','Kpr','Compressibility coefficient (0...1)',2,0,'0.95',0,7,'Коефіц. стиснення середовища (0...1)','Коэфиц. сжим. среды (0...1)','','');
INSERT INTO "techApp_io" VALUES('lagClean','out','Output',2,2,'0',0,0,'Вихід','Выход','','');
INSERT INTO "techApp_io" VALUES('lagClean','in','Input',2,0,'0',0,1,'Вхід','Вход','','');
INSERT INTO "techApp_io" VALUES('lagClean','t_lg','Lag time (s)',2,0,'10',0,2,'Час затримки (с)','Время запазд. (с)','','');
INSERT INTO "techApp_io" VALUES('lagClean','f_frq','Calc frequency (Hz)',2,0,'100',1,3,'Частота обчислення (Гц)','Частота вычисления (Гц)','','');
INSERT INTO "techApp_io" VALUES('lagClean','cl1','Link 1',2,0,'0',1,4,'Ланка 1','Звено 1','','');
INSERT INTO "techApp_io" VALUES('lagClean','cl2','Link 2',2,0,'0',1,5,'Ланка 2','Звено 2','','');
INSERT INTO "techApp_io" VALUES('lagClean','cl3','Link 3',2,0,'0',1,6,'Ланка 3','Звено 3','','');
INSERT INTO "techApp_io" VALUES('net','Fi','Input flow (tones/h)',2,1,'10',0,0,'Вхідн. витрати (т/год)','Входн. расхода (т/ч)','','');
INSERT INTO "techApp_io" VALUES('net','Pi','Input pressure (at)',2,0,'1',0,1,'Вхідн. тиск (ата)','Входн. давление (ата)','','');
INSERT INTO "techApp_io" VALUES('net','Po','Output pressure setpoint (at)',2,0,'1',0,2,'Вих. завдання тиску (ата)','Вых. задание давления (ата)','','');
INSERT INTO "techApp_io" VALUES('net','So','Output pipe cutset (m2)',2,0,'0.1',0,3,'Вих. перетин труби (м2)','Вых. сечение трубы (м2)','','');
INSERT INTO "techApp_io" VALUES('net','Kpr','Compressibility coefficient (0...1)',2,0,'0.95',0,4,'Коефіц. стиснення середовища (0...1)','Коэфиц. сжим. среды (0...1)','','');
INSERT INTO "techApp_io" VALUES('net','Noise','Input flow''s noise',2,0,'1',0,5,'Шум вхідн. витрат','Шум входн. расхода','','');
INSERT INTO "techApp_io" VALUES('net','Q0','Norm density of environs (kg/m3)',2,0,'1',0,6,'Норм. щільність середовища (кг/м3)','Норм. плотн. среды (кг/м3)','','');
INSERT INTO "techApp_io" VALUES('net','f_frq','Calc frequency (Hz)',2,0,'200',1,7,'Частота обчислення (Гц)','Частота вычисления (Гц)','','');
INSERT INTO "techApp_io" VALUES('src_press','Pi','Input pressure setpoint (at)',2,0,'10',0,0,'Завдання вхідного тиску (ата)','Задание входного давления (ата)','','');
INSERT INTO "techApp_io" VALUES('src_press','Fo','Output flow (tones/h)',2,0,'0',0,1,'Вих. витрати (т/год)','Вых. расход (т/ч)','','');
INSERT INTO "techApp_io" VALUES('src_press','Po','Output pressure (at)',2,1,'1',0,2,'Вих. тиск (ата)','Вых. давление (ата)','','');
INSERT INTO "techApp_io" VALUES('src_press','So','Output pipe cutset (m2)',2,0,'0.1',0,3,'Вих. перетин труби (м2)','Вых. сечение трубы (м2)','','');
INSERT INTO "techApp_io" VALUES('src_press','lo','Output pipe length (m)',2,0,'100',0,4,'Вих. довжина труби (м)','Вых. длина трубы (м)','','');
INSERT INTO "techApp_io" VALUES('src_press','Noise','Input flow''s noise',2,0,'1',0,5,'Шум вхідн. витрат','Шум входн. расхода','','');
INSERT INTO "techApp_io" VALUES('src_press','Q0','Norm density of environs (kg/m3)',2,0,'1',0,6,'Норм. щільність середовища (кг/м3)','Плотность среды в норм. услов. (кг/м3)','','');
INSERT INTO "techApp_io" VALUES('src_press','f_frq','Calc frequency (Hz)',2,0,'200',1,8,'Частота обчислення (Гц)','Частота вычисления (Гц)','','');
INSERT INTO "techApp_io" VALUES('src_press','Fit','Input flow laged',2,1,'0',1,9,'Вхід. витрати утримані','Вход. расход удержаный','','');
INSERT INTO "techApp_io" VALUES('cooler','Fi','Input flow (tones/h)',2,1,'0',0,0,'Вхідн. витрати (т/год)','Входн. расход (т/ч)','','');
INSERT INTO "techApp_io" VALUES('cooler','Pi','Input pressure (at)',2,0,'1',0,1,'Вхідн. тиск (ата)','Входн. давление (ата)','','');
INSERT INTO "techApp_io" VALUES('cooler','Ti','Input temperature (K)',2,0,'273',0,2,'Вхідн. температура (К)','Входн. температура (К)','','');
INSERT INTO "techApp_io" VALUES('cooler','Si','Cooler''s pipes cutset (m2)',2,0,'0.05',0,3,'Перетин трубок (м2)','Сечение трубок (м2)','','');
INSERT INTO "techApp_io" VALUES('cooler','li','Full cooler''s pipes length (m)',2,0,'10',0,4,'Загальна довжина трубок (м)','Общая длина трубок (м)','','');
INSERT INTO "techApp_io" VALUES('cooler','Fo','Output flow (tones/h)',2,0,'0',0,5,'Вих. витрати (т/год)','Вых. расход (т/ч)','','');
INSERT INTO "techApp_io" VALUES('cooler','Po','Output pressure (at)',2,1,'1',0,6,'Вих. тиск (ата)','Вых. давление (ата)','','');
INSERT INTO "techApp_io" VALUES('cooler','To','Output temperature (K)',2,1,'273',0,7,'Вих. температура (К)','Вых. температура (К)','','');
INSERT INTO "techApp_io" VALUES('cooler','So','Output pipe cutset (m2)',2,0,'.2',0,8,'Вих. перетин труби (м2)','Вых. сечение трубы (м2)','','');
INSERT INTO "techApp_io" VALUES('cooler','lo','Output pipe length (m)',2,0,'10',0,9,'Вих. довжина труби (м)','Вых. длина трубы (м)','','');
INSERT INTO "techApp_io" VALUES('cooler','Tair','Cooling air temperature (К)',2,0,'283',0,10,'Темп. охолодж. повітря (К)','Темп. охложд. воздуха (К)','','');
INSERT INTO "techApp_io" VALUES('cooler','Wc','Cooler performance',2,0,'200',0,11,'Продуктивність холод.','Производит. холод.','','');
INSERT INTO "techApp_io" VALUES('cooler','Q0','Norm density of environs (kg/m3)',2,0,'1',0,12,'Норм. щільність середовища (кг/м3)','Норм. плотн. ср. (кг/м3)','','');
INSERT INTO "techApp_io" VALUES('cooler','Ct','Warm capacity of environs',2,0,'100',0,13,'Теплоємність середовища','Теплоёмкость среды','','');
INSERT INTO "techApp_io" VALUES('cooler','Rt','Warm resistance of isolation',2,0,'1',0,14,'Тепл. опір','Тепл. сопротивление','','');
INSERT INTO "techApp_io" VALUES('cooler','f_frq','Calc frequency (Hz)',2,0,'200',1,15,'Частота обчислення (Гц)','Частота вычисления (Гц)','','');
INSERT INTO "techApp_io" VALUES('src_flow','Kpr','Compressibility coefficient (0...1)',2,0,'0.95',0,7,'Коефіц. стиснення середовища (0...1)','Коэфиц. сжим. среды (0...1)','','');
INSERT INTO "techApp_io" VALUES('pipe1_1','Kpr','Compressibility coefficient (0...1)',2,0,'0.95',0,7,'Коефіц. стиснення середовища (0...1)','Коэфиц. сжим. среды (0...1)','','');
INSERT INTO "techApp_io" VALUES('compressor','Fi','Input flow (tones/h)',2,1,'0',0,0,'Вхідн. витрати (т/год)','Входн. расход (т/ч)','','');
INSERT INTO "techApp_io" VALUES('compressor','Pi','Input pressure (at)',2,0,'1',0,1,'Вхідн. тиск (ата)','Входн. давление (ата)','','');
INSERT INTO "techApp_io" VALUES('compressor','Ti','Input temperature (K)',2,0,'273',0,2,'Вхідн. температура (К)','Входн. температура (К)','','');
INSERT INTO "techApp_io" VALUES('compressor','Fo','Output flow (tones/h)',2,0,'0',0,3,'Вих. витрати (т/год)','Вых. расход (т/ч)','','');
INSERT INTO "techApp_io" VALUES('compressor','Po','Output pressure (at)',2,1,'1',0,4,'Вих. тиск (ата)','Вых. давление (ата)','','');
INSERT INTO "techApp_io" VALUES('compressor','To','Output temperature (K)',2,1,'273',0,5,'Вих. температура (К)','Вых. температура (К)','','');
INSERT INTO "techApp_io" VALUES('compressor','So','Output pipe cutset (m2)',2,0,'0.2',0,6,'Вих. перетин труби (м2)','Вых. сечение трубы (м2)','','');
INSERT INTO "techApp_io" VALUES('compressor','lo','Output pipe length (m)',2,0,'2',0,7,'Вих. довжина труби (м)','Вых. длина трубы (м)','','');
INSERT INTO "techApp_io" VALUES('compressor','Kzp','Surge protect margin coefficient',2,1,'0.1',0,8,'Коеф. запасу по помпажу','Коэф. запаса по помпажу','','');
INSERT INTO "techApp_io" VALUES('compressor','N','Turnovers (1000 x turn/min)',2,0,'0',0,9,'Об. компр. (тис. об./хвил)','Об. компр. (тыс. об./мин)','','');
INSERT INTO "techApp_io" VALUES('compressor','V','Compressor capacity (m3)',2,0,'7',0,10,'Об''єм компресору (м3)','Объём компрессора (м3)','','');
INSERT INTO "techApp_io" VALUES('compressor','Kpmp','Surge coefficient (surge point)',2,0,'0.066',0,11,'Коеф. помп. (точка помп.)','Коэф. помп. (точка помп.)','','');
INSERT INTO "techApp_io" VALUES('compressor','Kslp','Slope coefficient of surge curve',2,0,'0.08',0,12,'Коеф. нахилу помп. кривої','Коэф. накл. помп. кривой','','');
INSERT INTO "techApp_io" VALUES('compressor','Q0','Norm density of environs (kg/m3)',2,0,'1',0,13,'Норм. щільність середовища (кг/м3)','Норм. плотн. ср. (кг/м3)','','');
INSERT INTO "techApp_io" VALUES('compressor','Ct','Warm capacity of environs',2,0,'100',0,15,'Теплоємність середовища','Теплоёмкость среды','','');
INSERT INTO "techApp_io" VALUES('compressor','Riz','Warm resistance of isolation',2,0,'100',0,16,'Тепл. опір ізоляції','Тепл. сопр. изоляции','','');
INSERT INTO "techApp_io" VALUES('compressor','Fwind','Air speed',2,0,'1',0,17,'Швидкість повітря','Скорость воздуха','','');
INSERT INTO "techApp_io" VALUES('compressor','Twind','Air temperature',2,0,'273',0,18,'Темпер. повітря','Темпер. воздуха','','');
INSERT INTO "techApp_io" VALUES('compressor','f_frq','Calc frequency (Hz)',2,0,'200',1,19,'Частота обчислення (Гц)','Частота вычисления (Гц)','','');
INSERT INTO "techApp_io" VALUES('pipe1_2','Kpr','Compressibility coefficient (0...1)',2,0,'0.95',0,11,'Коефіц. стиснення середовища (0...1)','Коэфиц. сжим. среды (0...1)','','');
INSERT INTO "techApp_io" VALUES('compressor','Fit','Input flow laged',2,1,'0',1,20,'Вхід. витрати утримані','Вход. расход удержаный','','');
INSERT INTO "techApp_io" VALUES('src_flow','Fi','Input flow setpoint (tones/h)',2,0,'10',0,0,'Завдання вхідних витрат (т/год)','Задание входн. расхода (т/ч)','','');
INSERT INTO "techApp_io" VALUES('src_flow','Fo','Output flow (tones/h)',2,0,'10',0,1,'Вих. витрати (т/год)','Вых. расход (т/ч)','','');
INSERT INTO "techApp_io" VALUES('src_flow','Po','Output pressure (at)',2,1,'1',0,2,'Вих. тиск (ата)','Вых. давление (ата)','','');
INSERT INTO "techApp_io" VALUES('src_flow','So','Output pipe cutset (m2)',2,0,'0.1',0,3,'Вих. перетин труби (м2)','Вых. сечение трубы (м2)','','');
INSERT INTO "techApp_io" VALUES('src_flow','lo','Output pipe length (m)',2,0,'100',0,4,'Вих. довжина труби (м)','Вых. длина трубы (м)','','');
INSERT INTO "techApp_io" VALUES('src_flow','Noise','Input flow''s noise',2,0,'1',0,5,'Шум вхідних витрат','Шум входн. расхода','','');
INSERT INTO "techApp_io" VALUES('src_flow','Q0','Norm density of environs (kg/m3)',2,0,'1',0,6,'Норм. щільність середовища (кг/м3)','Плотность среды в норм. услов. (кг/м3)','','');
INSERT INTO "techApp_io" VALUES('src_flow','f_frq','Calc frequency (Hz)',2,0,'100',1,8,'Частота обчислення (Гц)','Частота вычисления (Гц)','','');
INSERT INTO "techApp_io" VALUES('pipe1_1','Fi','Input flow (tones/h)',2,1,'0',0,0,'Вхідні витрати (т/год)','Входн. расход (т/ч)','','');
INSERT INTO "techApp_io" VALUES('pipe1_1','Pi','Input pressure (at)',2,0,'1',0,1,'Вхідний тиск (ата)','Входн. давление (ата)','','');
INSERT INTO "techApp_io" VALUES('pipe1_1','Fo','Output flow (tones/h)',2,0,'0',0,2,'Вих. витрати (т/год)','Вых. расход (т/ч)','','');
INSERT INTO "techApp_io" VALUES('pipe1_1','Po','Output pressure (at)',2,1,'1',0,3,'Вих. тиск (ата)','Вых. давление (ата)','','');
INSERT INTO "techApp_io" VALUES('pipe1_1','So','Output cutset (m2)',2,0,'.2',0,4,'Вих. перетин (м2)','Вых. сечение (м2)','','');
INSERT INTO "techApp_io" VALUES('pipe1_1','lo','Output length (m)',2,0,'10',0,5,'Вих. довжина (м)','Вых. длина (м)','','');
INSERT INTO "techApp_io" VALUES('pipe1_1','Q0','Norm density of environs (kg/m3)',2,0,'1',0,6,'Норм. щільність середовища (кг/м3)','Норм. плотн. среды (кг/м3)','','');
INSERT INTO "techApp_io" VALUES('pipe1_1','f_frq','Calc frequency (Hz)',2,0,'200',1,8,'Частота обчислення (Гц)','Частота вычисления (Гц)','','');
INSERT INTO "techApp_io" VALUES('pipe1_1','Pti','Pti',2,1,'1',1,9,'','','','');
INSERT INTO "techApp_io" VALUES('pipe1_1','Fto','Fto',2,1,'0',1,10,'','','','');
INSERT INTO "techApp_io" VALUES('pipe1_1','Pt1','Pt1',2,1,'1',1,11,'','','','');
INSERT INTO "techApp_io" VALUES('pipe1_1','Ft1','Ft1',2,1,'0',1,12,'','','','');
INSERT INTO "techApp_io" VALUES('pipe1_2','Fi','Input flow (tones/h)',2,1,'0',0,0,'Вхідні витрати (т/год)','Входн. расход (т/ч)','','');
INSERT INTO "techApp_io" VALUES('pipe1_2','Pi','Input pressure (at)',2,0,'1',0,1,'Вхідний тиск (ата)','Входн. давление (ата)','','');
INSERT INTO "techApp_io" VALUES('pipe1_2','Fo1','Output 1 flow (tones/h)',2,0,'0',0,2,'Вих. витрати 1 (т/год)','Вых. расход 1 (т/ч)','','');
INSERT INTO "techApp_io" VALUES('pipe1_2','Po1','Output 1 pressure (at)',2,1,'1',0,3,'Вих. тиск 1 (ата)','Вых. давление 1 (ата)','','');
INSERT INTO "techApp_io" VALUES('pipe1_2','So1','Output 1 cutset (m2)',2,0,'.2',0,4,'Вих. перетин труби 1 (м2)','Вых. сечение 1 (м2)','','');
INSERT INTO "techApp_io" VALUES('pipe1_2','lo1','Output 1 length (m)',2,0,'10',0,5,'Вих. довжина труби 1 (м)','Вых. длина 1 (м)','','');
INSERT INTO "techApp_io" VALUES('pipe1_2','Fo2','Output 2 flow (tones/h)',2,0,'0',0,6,'Вих. витрати 2 (т/год)','Вых. расход 2 (т/ч)','','');
INSERT INTO "techApp_io" VALUES('pipe1_2','Po2','Output 2 pressure (at)',2,1,'1',0,7,'Вих. тиск 2 (ата)','Вых. давление 2 (ата)','','');
INSERT INTO "techApp_io" VALUES('pipe1_2','So2','Output 2 cutset (m2)',2,0,'.2',0,8,'Вих. перетин труби 2 (м2)','Вых. сечение 2 (м2)','','');
INSERT INTO "techApp_io" VALUES('pipe1_2','lo2','Output 2 length (m)',2,0,'10',0,9,'Вих. довжина труби 2 (м)','Вых. длина 2 (м)','','');
INSERT INTO "techApp_io" VALUES('pipe1_2','Q0','Norm density of environs (kg/m3)',2,0,'1',0,10,'Норм. щільність середовища (кг/м3)','Норм. плотн. среды (кг/м3)','','');
INSERT INTO "techApp_io" VALUES('pipe1_2','f_frq','Calc frequency (Hz)',2,0,'100',1,12,'Частота обчислення (Гц)','Частота вычисления (Гц)','','');
INSERT INTO "techApp_io" VALUES('pipe1_2','F1tmp','Temporary flow 1',2,1,'0',1,13,'Тимчас. витрати 1','Врем. расход 1','','');
INSERT INTO "techApp_io" VALUES('pipe1_2','F2tmp','Temporary flow 2',2,1,'0',1,14,'Тимчас. витрати 2','Врем. расход 2','','');
INSERT INTO "techApp_io" VALUES('pipe1_2','Pot1','Temporary pressure 1',2,1,'1',1,15,'Тимчас. тиск 1','Временное давление 1','','');
INSERT INTO "techApp_io" VALUES('pipe1_2','Pot2','Temporary pressure 2',2,1,'1',1,16,'Тимчас. тиск 2','Временное давление 2','','');
INSERT INTO "techApp_io" VALUES('pipe1_3','Fi','Input flow (tones/h)',2,1,'0',0,0,'Вхідні витрати (т/год)','Входн. расход (т/ч)','','');
INSERT INTO "techApp_io" VALUES('pipe1_3','Pi','Input pressure (at)',2,0,'1',0,1,'Вхідний тиск (ата)','Входн. давление (ата)','','');
INSERT INTO "techApp_io" VALUES('pipe1_3','Fo1','Output 1 flow (tones/h)',2,0,'0',0,2,'Вих. витрати 1 (т/год)','Вых. расход 1 (т/ч)','','');
INSERT INTO "techApp_io" VALUES('pipe1_3','Po1','Output 1 pressure (at)',2,1,'1',0,3,'Вих. тиск 1 (ата)','Вых. давление 1 (ата)','','');
INSERT INTO "techApp_io" VALUES('pipe1_3','So1','Output 1 cutset (m2)',2,0,'0.2',0,4,'Вих. перетин труби 1 (м2)','Вых. сечение 1 (м2)','','');
INSERT INTO "techApp_io" VALUES('pipe1_3','lo1','Output 1 length (m)',2,0,'10',0,5,'Вих. довжина труби 1 (м)','Вых. длина 1 (м)','','');
INSERT INTO "techApp_io" VALUES('pipe1_3','Fo2','Output 2 flow (tones/h)',2,0,'0',0,6,'Вих. витрати 2 (т/год)','Вых. расход 2 (т/ч)','','');
INSERT INTO "techApp_io" VALUES('pipe1_3','Po2','Output 2 pressure (at)',2,1,'1',0,7,'Вих. тиск 2 (ата)','Вых. давление 2 (ата)','','');
INSERT INTO "techApp_io" VALUES('pipe1_3','So2','Output 2 cutset (m2)',2,0,'0.2',0,8,'Вих. перетин труби 2 (м2)','Вых. сечение 2 (м2)','','');
INSERT INTO "techApp_io" VALUES('pipe1_3','lo2','Output 2 length (m)',2,0,'10',0,9,'Вих. довжина труби 2 (м)','Вых. длина 2 (м)','','');
INSERT INTO "techApp_io" VALUES('pipe1_3','Fo3','Output 3 flow (tones/h)',2,0,'0',0,10,'Вих. витрати 3 (т/год)','Вых. расход 3 (т/ч)','','');
INSERT INTO "techApp_io" VALUES('pipe1_3','Po3','Output 3 pressure (at)',2,1,'1',0,11,'Вих. тиск 3 (ата)','Вых. давление 3 (ата)','','');
INSERT INTO "techApp_io" VALUES('pipe1_3','So3','Output 3 cutset (m2)',2,0,'0.2',0,12,'Вих. перетин труби 3 (м2)','Вых. сечение 3 (м2)','','');
INSERT INTO "techApp_io" VALUES('pipe1_3','lo3','Output 3 length (m)',2,0,'10',0,13,'Вих. довжина труби 3 (м)','Вых. длина 3 (м)','','');
INSERT INTO "techApp_io" VALUES('pipe1_3','Q0','Norm density of environs (kg/m3)',2,0,'1',0,14,'Норм. щільність середовища (кг/м3)','Норм. плотн. среды (кг/м3)','','');
INSERT INTO "techApp_io" VALUES('pipe1_3','f_frq','Calc frequency (Hz)',2,0,'200',1,16,'Частота обчислення (Гц)','Частота вычисления (Гц)','','');
INSERT INTO "techApp_io" VALUES('pipe1_3','F1tmp','Temporary flow 1',2,1,'0',0,17,'Тимчас. витрати 1','Врем. расход 1','','');
INSERT INTO "techApp_io" VALUES('pipe1_3','F2tmp','Temporary flow 2',2,1,'0',0,18,'Тимчас. витрати 2','Врем. расход 2','','');
INSERT INTO "techApp_io" VALUES('pipe1_3','F3tmp','Temporary flow 3',2,1,'0',0,19,'Тимчас. витрати 3','Врем. расход 3','','');
INSERT INTO "techApp_io" VALUES('pipe1_3','Pot1','Temporary pressure 1',2,1,'1',1,20,'Тимчас. тиск 1','Временное давление 1','','');
INSERT INTO "techApp_io" VALUES('pipe1_3','Pot2','Temporary pressure 2',2,1,'1',1,21,'Тимчас. тиск 2','Временное давление 2','','');
INSERT INTO "techApp_io" VALUES('pipe1_3','Pot3','Temporary pressure 3',2,1,'1',1,22,'Тимчас. тиск 3','Временное давление 3','','');
INSERT INTO "techApp_io" VALUES('pipe1_4','Fi','Input flow (tones/h)',2,1,'0',0,0,'Вхідні витрати (т/год)','Входн. расход (т/ч)','','');
INSERT INTO "techApp_io" VALUES('pipe1_4','Pi','Input pressure (at)',2,0,'1',0,1,'Вхідний тиск (ата)','Входн. давление (ата)','','');
INSERT INTO "techApp_io" VALUES('pipe1_4','Fo1','Output 1 flow (tones/h)',2,0,'0',0,2,'Вих. витрати 1 (т/год)','Вых. расход 1 (т/ч)','','');
INSERT INTO "techApp_io" VALUES('pipe1_4','Po1','Output 1 pressure (at)',2,1,'1',0,3,'Вих. тиск 1 (ата)','Вых. давление 1 (ата)','','');
INSERT INTO "techApp_io" VALUES('pipe1_4','So1','Output 1 cutset (m2)',2,0,'0.2',0,4,'Вих. перетин труби 1 (м2)','Вых. сечение 1 (м2)','','');
INSERT INTO "techApp_io" VALUES('pipe1_4','lo1','Output 1 length (m)',2,0,'10',0,5,'Вих. довжина труби 1 (м)','Вых. длина 1 (м)','','');
INSERT INTO "techApp_io" VALUES('pipe1_4','Fo2','Output 2 flow (tones/h)',2,0,'0',0,6,'Вих. витрати 2 (т/год)','Вых. расход 2 (т/ч)','','');
INSERT INTO "techApp_io" VALUES('pipe1_4','Po2','Output 2 pressure (at)',2,1,'1',0,7,'Вих. тиск 2 (ата)','Вых. давление 2 (ата)','','');
INSERT INTO "techApp_io" VALUES('pipe1_4','So2','Output 2 cutset (m2)',2,0,'0.2',0,8,'Вих. перетин труби 2 (м2)','Вых. сечение 2 (м2)','','');
INSERT INTO "techApp_io" VALUES('pipe1_4','lo2','Output 2 length (m)',2,0,'10',0,9,'Вих. довжина труби 2 (м)','Вых. длина 2 (м)','','');
INSERT INTO "techApp_io" VALUES('pipe1_4','Fo3','Output 3 flow (tones/h)',2,0,'0',0,10,'Вих. витрати 3 (т/год)','Вых. расход 3 (т/ч)','','');
INSERT INTO "techApp_io" VALUES('pipe1_4','Po3','Output 3 pressure (at)',2,1,'1',0,11,'Вих. тиск 3 (ата)','Вых. давление 3 (ата)','','');
INSERT INTO "techApp_io" VALUES('pipe1_4','So3','Output 3 cutset (m2)',2,0,'0.2',0,12,'Вих. перетин труби 3 (м2)','Вых. сечение 3 (м2)','','');
INSERT INTO "techApp_io" VALUES('pipe1_4','lo3','Output 3 length (m)',2,0,'10',0,13,'Вих. довжина труби 3 (м)','Вых. длина 3 (м)','','');
INSERT INTO "techApp_io" VALUES('pipe1_4','Fo4','Output 4 flow (tones/h)',2,0,'0',0,14,'Вих. витрати 4 (т/год)','Вых. расход 4 (т/ч)','','');
INSERT INTO "techApp_io" VALUES('pipe1_4','Po4','Output 4 pressure (at)',2,1,'1',0,15,'Вих. тиск 4 (ата)','Вых. давление 4 (ата)','','');
INSERT INTO "techApp_io" VALUES('pipe1_4','So4','Output 4 cutset (m2)',2,0,'0.2',0,16,'Вих. перетин труби 4 (м2)','Вых. сечение 4 (м2)','','');
INSERT INTO "techApp_io" VALUES('pipe1_4','lo4','Output 4 length (m)',2,0,'10',0,17,'Вих. довжина труби 4 (м)','Вых. длина 4 (м)','','');
INSERT INTO "techApp_io" VALUES('pipe1_4','Q0','Norm density of environs (kg/m3)',2,0,'1',0,18,'Норм. щільність середовища (кг/м3)','Норм. плотн. среды (кг/м3)','','');
INSERT INTO "techApp_io" VALUES('pipe1_4','f_frq','Calc frequency (Hz)',2,0,'200',1,20,'Частота обчислення (Гц)','Частота вычисления (Гц)','','');
INSERT INTO "techApp_io" VALUES('pipe1_4','F1tmp','Temporary flow 1',2,1,'0',1,21,'Тимчас. витрати 1','Врем. расход 1','','');
INSERT INTO "techApp_io" VALUES('pipe1_4','F2tmp','Temporary flow 2',2,1,'0',1,22,'Тимчас. витрати 2','Врем. расход 2','','');
INSERT INTO "techApp_io" VALUES('pipe1_4','F3tmp','Temporary flow 3',2,1,'0',1,23,'Тимчас. витрати 3','Врем. расход 3','','');
INSERT INTO "techApp_io" VALUES('pipe1_4','F4tmp','Temporary flow 4',2,1,'0',1,24,'Тимчас. витрати 4','Врем. расход 4','','');
INSERT INTO "techApp_io" VALUES('pipe1_4','Pot1','Temporary pressure 1',2,1,'1',1,25,'Тимчас. тиск 1','Временное давление 1','','');
INSERT INTO "techApp_io" VALUES('pipe1_4','Pot2','Temporary pressure 2',2,1,'1',1,26,'Тимчас. тиск 2','Временное давление 2','','');
INSERT INTO "techApp_io" VALUES('pipe1_4','Pot3','Temporary pressure 3',2,1,'1',1,27,'Тимчас. тиск 3','Временное давление 3','','');
INSERT INTO "techApp_io" VALUES('pipe1_4','Pot4','Temporary pressure 4',2,1,'1',1,28,'Тимчас. тиск 4','Временное давление 4','','');
INSERT INTO "techApp_io" VALUES('klapMech','pos','Position (%)',2,1,'0',0,0,'Положення (%)','Положение (%)','','');
INSERT INTO "techApp_io" VALUES('klapMech','pos_sensor','Position by sensor (%)',2,1,'0',0,1,'Положення за давачем (%)','Положение по датчику (%)','','');
INSERT INTO "techApp_io" VALUES('klapMech','com','Command',2,0,'0',0,2,'Команда','Команда','','');
INSERT INTO "techApp_io" VALUES('klapMech','st_open','State "Opened"',3,1,'0',0,3,'Стан "Відкрито"','Сост. "Открыто"','','');
INSERT INTO "techApp_io" VALUES('klapMech','st_close','State "Closed"',3,1,'1',0,4,'Стан "Закрито"','Сост. "Закрыто"','','');
INSERT INTO "techApp_io" VALUES('klapMech','t_full','Going time (s)',2,0,'3',0,5,'Час ходу (с)','Время хода (с)','','');
INSERT INTO "techApp_io" VALUES('klapMech','t_up','Estrangement time (s)',2,0,'1',0,6,'Час зриву (с)','Время срыва (с)','','');
INSERT INTO "techApp_io" VALUES('klapMech','t_sensor','Sensors'' lag time (s)',2,0,'1',0,7,'Час затримки сенсора (с)','Время задержки сенсора (с)','','');
INSERT INTO "techApp_io" VALUES('klapMech','f_frq','Calc frequency (Hz)',2,0,'100',1,8,'Частота обчислення (Гц)','Частота вычисления (Гц)','','');
INSERT INTO "techApp_io" VALUES('klapMech','tmp_up','Estrangement counter',2,1,'0',0,9,'Лічильник зриву','Счётчик срыва','','');
INSERT INTO "techApp_io" VALUES('klapMech','lst_com','Last command',2,1,'0',0,10,'Остання команда','Последняя команда','','');
INSERT INTO "techApp_io" VALUES('diafragma','Fi','Input flow (tones/h)',2,1,'0',0,0,'Вхідні витрати (т/год)','Входн. расход (т/ч)','','');
INSERT INTO "techApp_io" VALUES('diafragma','Pi','Input pressure (at)',2,0,'1',0,1,'Вхідний тиск (ата)','Входн. давление (ata)','','');
INSERT INTO "techApp_io" VALUES('diafragma','Fo','Output flow (tones/h)',2,0,'0',0,2,'Вих. витрати (т/год)','Вых. расход (т/ч)','','');
INSERT INTO "techApp_io" VALUES('diafragma','Po','Output pressure (at)',2,1,'1',0,3,'Вих. тиск (ата)','Вых. давление (ата)','','');
INSERT INTO "techApp_io" VALUES('diafragma','dP','Pressure differential (kPa)',2,1,'0',0,4,'Перепад тиску (кПа)','Перепад давления (кПа)','','');
INSERT INTO "techApp_io" VALUES('diafragma','Sdf','Diaphragm cutset (m2)',2,0,'0.1',0,5,'Перетин діафрагми (м2)','Сеч. диафрагмы (м2)','','');
INSERT INTO "techApp_io" VALUES('diafragma','So','Output pipe cutset (m2)',2,0,'0.2',0,6,'Вих. перетин труби (м2)','Вых. сечение трубы (м2)','','');
INSERT INTO "techApp_io" VALUES('diafragma','lo','Output pipe length (m)',2,0,'10',0,7,'Вих. довжина труби (м)','Вых. длина трубы (м)','','');
INSERT INTO "techApp_io" VALUES('diafragma','Q0','Norm density of environs (kg/m3)',2,0,'1',0,8,'Норм. щільність середовища (кг/м3)','Пл. при реальн. усл. (кг/м3)','','');
INSERT INTO "techApp_io" VALUES('diafragma','f_frq','Calc frequency (Hz)',2,0,'100',1,10,'Частота обчислення (Гц)','Частота вычисления (Гц)','','');
INSERT INTO "techApp_io" VALUES('pipe3_1','Kpr','Compressibility coefficient (0...1)',2,0,'0.95',0,18,'Коефіц. стиснення середовища (0...1)','Коэфиц. сжим. среды (0...1)','','');
INSERT INTO "techApp_io" VALUES('pipe3_1','Fi1','Input 1 flow (tones/h)',2,1,'0',0,0,'Вхідні витрати 1 (т/год)','Входн. расход 1 (т/ч)','','');
INSERT INTO "techApp_io" VALUES('pipe3_1','Pi1','Input 1 pressure (at)',2,0,'1',0,1,'Вхідний тиск 1 (ата)','Входн. давление 1 (ата)','','');
INSERT INTO "techApp_io" VALUES('pipe3_1','Ti1','Input 1 temperature (K)',2,0,'273',0,2,'Вхідна температура 1 (К)','Входн. температура 1 (К)','','');
INSERT INTO "techApp_io" VALUES('pipe3_1','Si1','Input 1 cutset (m2)',2,0,'0.2',0,3,'Вхідний перетин 1 (м2)','Входн. сечение 1 (м2)','','');
INSERT INTO "techApp_io" VALUES('pipe3_1','Fi2','Input 2 flow (tones/h)',2,1,'0',0,4,'Вхідні витрати 2 (т/год)','Входн. расход 2 (т/ч)','','');
INSERT INTO "techApp_io" VALUES('pipe3_1','Pi2','Input 2 pressure (at)',2,0,'1',0,5,'Вхідний тиск 2 (ата)','Входн. давление 2 (ата)','','');
INSERT INTO "techApp_io" VALUES('pipe3_1','Ti2','Input 2 temperature (K)',2,0,'273',0,6,'Вхідна температура 2 (К)','Входн. температура 2 (К)','','');
INSERT INTO "techApp_io" VALUES('pipe3_1','Si2','Input 2 cutset (m2)',2,0,'0.2',0,7,'Вхідний перетин 2 (м2)','Входн. сечение 2 (м2)','','');
INSERT INTO "techApp_io" VALUES('pipe3_1','Fi3','Input 3 flow (tones/h)',2,1,'0',0,8,'Вхідні витрати 3 (т/год)','Входн. расход 3 (т/ч)','','');
INSERT INTO "techApp_io" VALUES('pipe3_1','Pi3','Input 3 pressure (at)',2,0,'1',0,9,'Вхідний тиск 3 (ата)','Входн. давление 3 (ата)','','');
INSERT INTO "techApp_io" VALUES('pipe3_1','Ti3','Input 3 temperature (K)',2,0,'273',0,10,'Вхідна температура 3 (К)','Входн. температура 3 (К)','','');
INSERT INTO "techApp_io" VALUES('pipe3_1','Si3','Input 3 cutset (m2)',2,0,'0.2',0,11,'Вхідний перетин 3 (м2)','Входн. сечение 3 (м2)','','');
INSERT INTO "techApp_io" VALUES('pipe3_1','Fo','Output flow (tones/h)',2,0,'0',0,12,'Вих. витрати (т/год)','Вых. расход (т/ч)','','');
INSERT INTO "techApp_io" VALUES('pipe3_1','Po','Output pressure (at)',2,1,'1',0,13,'Вих. тиск (ата)','Вых. давление (ата)','','');
INSERT INTO "techApp_io" VALUES('pipe3_1','To','Output temperature (K)',2,1,'273',0,14,'Вих. температура (К)','Вых. температура (К)','','');
INSERT INTO "techApp_io" VALUES('pipe3_1','So','Output cutset (m2)',2,0,'.2',0,15,'Вих. перетин (м2)','Вых. сечение (м2)','','');
INSERT INTO "techApp_io" VALUES('pipe3_1','lo','Output length (m)',2,0,'10',0,16,'Вих. довжина (м)','Вых. длина (м)','','');
INSERT INTO "techApp_io" VALUES('pipe3_1','Q0','Norm density of environs (kg/m3)',2,0,'1',0,17,'Норм. щільність середовища (кг/м3)','Норм. плотн. среды (кг/м3)','','');
INSERT INTO "techApp_io" VALUES('pipe3_1','Ct','Warm capacity of environs',2,0,'20',0,19,'Теплоємність середовища','Теплоёмкость среды','','');
INSERT INTO "techApp_io" VALUES('pipe3_1','Riz','Warm resistance of isolation',2,0,'20',0,20,'Тепл. опір ізоляції','Тепл. сопр. изоляции','','');
INSERT INTO "techApp_io" VALUES('pipe3_1','Fwind','Air speed',2,0,'1',0,21,'Швидкість повітря','Скорость воздуха','','');
INSERT INTO "techApp_io" VALUES('pipe3_1','Twind','Air temperature (К)',2,0,'273',0,22,'Темпер. повітря (К)','Температура воздуха (К)','','');
INSERT INTO "techApp_io" VALUES('pipe3_1','f_frq','Calc frequency (Hz)',2,0,'100',1,23,'Частота обчислення (Гц)','Частота вычисления (Гц)','','');
INSERT INTO "techApp_io" VALUES('compressor','Kpr','Compressibility coefficient (0...1)',2,0,'0.95',0,14,'Коефіц. стиснення середовища (0...1)','Коэфиц. сжим. среды (0...1)','','');
INSERT INTO "techApp_io" VALUES('pipeBase','Fi','Input flow (tones/h)',2,1,'0',0,0,'Вхідні витрати (т/год)','Входн. расход (т/ч)','','');
INSERT INTO "techApp_io" VALUES('pipeBase','Pi','Input pressure (at)',2,0,'1',0,1,'Вхідний тиск (ата)','Входн. давление (ата)','','');
INSERT INTO "techApp_io" VALUES('pipeBase','Ti','Input temperature (K)',2,0,'293',0,2,'Вхідна температура (К)','Входн. температура (К)','','');
INSERT INTO "techApp_io" VALUES('pipeBase','Si','Input cutset (m2)',2,0,'.2',0,3,'Вхідний перетин (м2)','Входн. сечение (м2)','','');
INSERT INTO "techApp_io" VALUES('pipeBase','Fo','Output flow (tones/h)',2,0,'0',0,4,'Вих. витрати (т/год)','Вых. расход (т/ч)','','');
INSERT INTO "techApp_io" VALUES('pipeBase','Po','Output pressure (at)',2,1,'1',0,5,'Вих. тиск (ата)','Вых. давление (ата)','','');
INSERT INTO "techApp_io" VALUES('pipeBase','To','Output temperature (K)',2,1,'293',0,6,'Вих. температура (К)','Вых. температура (К)','','');
INSERT INTO "techApp_io" VALUES('pipeBase','So','Output cutset (m2)',2,0,'.2',0,7,'Вих. перетин (м2)','Вых. сечение (м2)','','');
INSERT INTO "techApp_io" VALUES('pipeBase','lo','Output length (m)',2,0,'10',0,8,'Вих. довжина (м)','Вых. длина (м)','','');
INSERT INTO "techApp_io" VALUES('pipeBase','Q0','Norm density of environs (kg/m3)',2,0,'1',0,9,'Норм. щільність середовища (кг/м3)','Норм. плотн. среды (кг/м3)','','');
INSERT INTO "techApp_io" VALUES('pipeBase','Kpr','Compressibility coefficient (0...1)',2,0,'0.98',0,10,'Коефіц. стиснення середовища (0...1)','Коэфиц. сжим. среды (0...1)','','');
INSERT INTO "techApp_io" VALUES('pipeBase','Ktr','Coefficient of friction',2,0,'0.01',0,11,'Коефіцієнт тертя','Коэф. трения','','');
INSERT INTO "techApp_io" VALUES('pipeBase','f_frq','Calc frequency (Hz)',2,0,'100',0,12,'Частота обчислення (Гц)','Частота вычисления (Гц)','','');
INSERT INTO "techApp_io" VALUES('heatExch','Fi1','Input 1 flow (tones/h)',2,0,'20',0,0,'Вхідні витрати 1 (т/год)','Вх. расход первого потока (т/ч)','','');
INSERT INTO "techApp_io" VALUES('heatExch','Pi1','Input 1 pressure (at)',2,0,'1',0,1,'Вхідний тиск 1 (ата)','Входн. давление 1 (ата)','','');
INSERT INTO "techApp_io" VALUES('heatExch','Ti1','Input 1 temperature (K)',2,0,'20',0,2,'Вхідна температура 1 (К)','Вх. температура первого потока (K)','','');
INSERT INTO "techApp_io" VALUES('heatExch','Si1','Input 1 cutset (m2)',2,0,'1',0,3,'Вхідний перетин 1 (м2)','Вх. сечение труб первого потока (м2)','','');
INSERT INTO "techApp_io" VALUES('heatExch','li1','Input 1 length (m)',2,0,'10',0,4,'Вхідна довжина 1 (м)','Входн. длина 1 (м)','','');
INSERT INTO "techApp_io" VALUES('heatExch','Q0i1','Input 1 norm. density (kg/m3)',2,0,'1',0,5,'Вхідна норм. щільність 1 (кг/м3)','Входная норм. плотность 1 (кг/м3)','','');
INSERT INTO "techApp_io" VALUES('heatExch','Kpr1','Input 1 compressibility coefficient (0...1)',2,0,'0.9',0,6,'Вхідний коеф. стиснення середовища 1 (0...1)','Входн. коэфиц. сжим. среды 1 (0...1)','','');
INSERT INTO "techApp_io" VALUES('heatExch','Ci1','Input 1 warm capacity',2,0,'1',0,7,'Вхідна теплоємність 1','Входн. теплоёмкость 1','','');
INSERT INTO "techApp_io" VALUES('heatExch','Fi2','Input 2 flow (tones/h)',2,0,'20',0,8,'Вхідні витрати 2 (т/год)','Вх. расход второго потока (т/ч)','','');
INSERT INTO "techApp_io" VALUES('heatExch','Pi2','Input 2 pressure (at)',2,0,'1',0,9,'Вхідний тиск 2 (ата)','Входн. давление 2 (ата)','','');
INSERT INTO "techApp_io" VALUES('heatExch','Ti2','Input 2 temperature (K)',2,0,'40',0,10,'Вхідна температура 2 (К)','Вх. температура второго потока (K)','','');
INSERT INTO "techApp_io" VALUES('heatExch','Si2','Input 2 cutset (m2)',2,0,'1',0,11,'Вхідний перетин 2 (м2)','Вх. сечение труб второго потока (м2)','','');
INSERT INTO "techApp_io" VALUES('heatExch','li2','Input 2 length (m)',2,0,'10',0,12,'Вхідна довжина 2 (м)','Входн. длина 2 (м)','','');
INSERT INTO "techApp_io" VALUES('heatExch','Q0i2','Input 2 norm. density (kg/m3)',2,0,'1',0,13,'Вхідна норм. щільність 2 (кг/м3)','Входная норм. плотность 2 (кг/м3)','','');
INSERT INTO "techApp_io" VALUES('heatExch','Kpr2','Input 2 compressibility coefficient (0...1)',2,0,'0.9',0,14,'Вхідний коеф. стиснення середовища 2 (0...1)','Входн. коэфиц. сжим. среды 2 (0...1)','','');
INSERT INTO "techApp_io" VALUES('heatExch','Ci2','Input 2 warm capacity',2,0,'1',0,15,'Вхідна теплоємність 2','Входн. теплоёмкость 2','','');
INSERT INTO "techApp_io" VALUES('heatExch','ki','Heat transfer coefficient',2,0,'0.9',0,16,'Коефіцієнт тепловіддачі','Коэффициент теплоотдачи ','','');
INSERT INTO "techApp_io" VALUES('heatExch','Fo1','Output 1 flow (tones/h)',2,0,'0',0,17,'Вих. витрати 1 (т/год)','Вых. расход 1 (т/ч)','','');
INSERT INTO "techApp_io" VALUES('heatExch','Po1','Output 1 pressure (at)',2,1,'1',0,18,'Вих. тиск 1 (ата)','Вых. давление 1 (ата)','','');
INSERT INTO "techApp_io" VALUES('heatExch','To1','Output 1 temperature (K)',2,1,'273',0,19,'Вих. температура 1 (К)','Вых. температура 1 (K)','','');
INSERT INTO "techApp_io" VALUES('heatExch','So1','Output 1 cutset (m2)',2,1,'1',0,20,'Вих. перетин труби 1 (м2)','Вых. сечение 1 (м2)','','');
INSERT INTO "techApp_io" VALUES('heatExch','lo1','Output 1 length (m)',2,1,'10',0,21,'Вих. довжина труби 1 (м)','Вых. длина 1 (м)','','');
INSERT INTO "techApp_io" VALUES('heatExch','Fo2','Output 2 flow (tones/h)',2,0,'0',0,22,'Вих. витрати 2 (т/год)','Вых. расход 2 (т/ч)','','');
INSERT INTO "techApp_io" VALUES('heatExch','Po2','Output 2 pressure (at)',2,1,'1',0,23,'Вих. тиск 2 (ата)','Вых. давление 2 (ата)','','');
INSERT INTO "techApp_io" VALUES('heatExch','To2','Output 2 temperature (K)',2,1,'273',0,24,'Вих. температура 2 (К)','Вых. температура 2 (K)','','');
INSERT INTO "techApp_io" VALUES('heatExch','So2','Output 2 cutset (m2)',2,1,'1',0,25,'Вих. перетин труби 2 (м2)','Вых. сечение 2 (м2)','','');
INSERT INTO "techApp_io" VALUES('heatExch','lo2','Output 2 length (m)',2,1,'10',0,26,'Вих. довжина труби 2 (м)','Вых. длина 2 (м)','','');
INSERT INTO "techApp_io" VALUES('heatExch','f_frq','Calc frequency (Hz)',2,0,'200',0,27,'Частота обчислення (Гц)','Частота вычисления (Гц)','','');
INSERT INTO "techApp_io" VALUES('boilerBarrel','Fi1','Input water flow (tones/h)',2,1,'22',0,0,'Вхідн. витрати води (т/год)','Вх. расход воды (т/ч)','','');
INSERT INTO "techApp_io" VALUES('boilerBarrel','Pi1','Input water pressure (at)',2,0,'43',0,1,'Вхідн. тиск води (ата)','Вх. давление воды (ата)','','');
INSERT INTO "techApp_io" VALUES('boilerBarrel','Ti1','Input water temperature (K)',2,0,'523',0,2,'Вхідн. температура води (К)','Вх. температура воды (K)','','');
INSERT INTO "techApp_io" VALUES('boilerBarrel','Si1','Input water cutset (m2)',2,0,'0.6',0,3,'Вхідн. перетин труби води (м2)','Вх. сечение труб с водой (м2)','','');
INSERT INTO "techApp_io" VALUES('boilerBarrel','Fi2','Input smoke gas flow (tones/h)',2,1,'',0,4,'Вхідн. витрати димових газів (т/год)','Вх. расход дымовых газов (т/ч)','','');
INSERT INTO "techApp_io" VALUES('boilerBarrel','Pi2','Input smoke gas pressure (at)',2,0,'1.3',0,5,'Вхідн. тиск димових газів (ата)','Вх. давление дымовых газов (ата)','','');
INSERT INTO "techApp_io" VALUES('boilerBarrel','Ti2','Input smoke gas temperature (K)',2,0,'1700',0,6,'Вхідн. температура димових газів (К)','Вх. температура дымовых газов (K)','','');
INSERT INTO "techApp_io" VALUES('boilerBarrel','Si2','Input smoke gas cutset (m2)',2,0,'10',0,7,'Вхідн. перетин труби димових газів (м2)','Вх. сечение трубы дымовых газов (м2)','','');
INSERT INTO "techApp_io" VALUES('boilerBarrel','Vi1','Barrel volume (m3)',2,0,'3',0,8,'Об''єм барабану (м3)','Объем барабана (м3)','','');
INSERT INTO "techApp_io" VALUES('boilerBarrel','Lo','Barrel level (%)',2,1,'10',0,9,'Рівень у барабані (%)','Уровень в барабане(%)','','');
INSERT INTO "techApp_io" VALUES('boilerBarrel','S','Heated surface (м2)',2,0,'15',0,10,'Поверхня нагріву (м2)','Поверхность нагрева (м2)','','');
INSERT INTO "techApp_io" VALUES('boilerBarrel','k','Heat transfer coefficient',2,0,'0.8',0,11,'Коефіцієнт тепловіддачі','Коэффициент теплоотдачи','','');
INSERT INTO "techApp_io" VALUES('boilerBarrel','Fo','Output steam flow (tones/h)',2,0,'20',0,12,'Вих. витрати пару (т/год)','Вых. расход пара (т/ч)','','');
INSERT INTO "techApp_io" VALUES('boilerBarrel','Po1','Output steam pressure (at)',2,1,'41.68',0,13,'Вих. тиск пару (ата)','Вых. давление пара (ата)','','');
INSERT INTO "techApp_io" VALUES('boilerBarrel','To1','Output steam temperature (K)',2,1,'10',0,14,'Вих. температура пару (К)','Вых. температура пара (K)','','');
INSERT INTO "techApp_io" VALUES('boilerBarrel','So1','Output steam pipe cutset (m2)',2,0,'0.5',0,15,'Вих. перетин труби пару (м2)','Вых. сечение трубы по пару (м2)','','');
INSERT INTO "techApp_io" VALUES('boilerBarrel','lo1','Output steam pipe length (m)',2,0,'5',0,16,'Вих. довжина труби пару (м)','Вых. длина трубы пара (м)','','');
INSERT INTO "techApp_io" VALUES('boilerBarrel','Fo2','Output smoke gas flow (tones/h)',2,0,'180',0,17,'Вих. витрати димових газів (т/год)','Вых. расход дымовых газов (т/ч)','','');
INSERT INTO "techApp_io" VALUES('boilerBarrel','Po2','Output smoke gas pressure (at)',2,1,'1',0,18,'Вих. тиск димових газів (ата)','Вых. давление дымовых газов (ата)','','');
INSERT INTO "techApp_io" VALUES('boilerBarrel','To2','Output smoke gas temperature (K)',2,0,'0',0,19,'Вих. температура димових газів (К)','Вых. температура дымовых газов (K)','','');
INSERT INTO "techApp_io" VALUES('boilerBarrel','Fpara','Inner barrel steam flow (tones/h)',2,1,'0',0,20,'Витрати пару у барабані (т/год)','Расход пара в барабане(т/ч)','','');
INSERT INTO "techApp_io" VALUES('boilerBarrel','Tv','Inner water temperature (K)',2,1,'0',0,21,'Температура води у барабані (K)','Температура воды в барабане(K)','','');
INSERT INTO "techApp_io" VALUES('boilerBarrel','f_frq','Calc frequency (Hz)',2,0,'200',0,22,'Частота обчислення (Гц)','Частота вычисления (Гц)','','');
INSERT INTO "techApp_io" VALUES('boilerBurner','Fi1','Input blast furnace gas flow (tone/h)',2,1,'',0,0,'Вхідн. витрати димових газів (т/год)','Вх. расход доменного газа (т/ч)','','');
INSERT INTO "techApp_io" VALUES('boilerBurner','Pi1','Input blast furnace gas pressure (at)',2,0,'',0,1,'Вхідн. тиск димових газів (ата)','Вх. давление доменного газа (ата)','','');
INSERT INTO "techApp_io" VALUES('boilerBurner','Ti1','Input blast furnace gas temperature (K)',2,0,'40',0,2,'Вхідн. температура димових газів (К)','Вх. температура доменного газа (K)','','');
INSERT INTO "techApp_io" VALUES('boilerBurner','Si1','Input blast furnace gas pipe cutset (m2)',2,0,'',0,3,'Вхідн. перетин труби димових газів (м2)','Вх. сечение трубы доменного газа (м2)','','');
INSERT INTO "techApp_io" VALUES('boilerBurner','Fi2','Input natural gas flow (tone/h)',2,1,'',0,4,'Вхідн. витрати природного газу (т/год)','Вх. расход природного газа (т/ч)','','');
INSERT INTO "techApp_io" VALUES('boilerBurner','Pi2','Input natural gas pressure (at)',2,0,'',0,5,'Вхідн. тиск природного газу (ата)','Вх. давление природного газа (ата)','','');
INSERT INTO "techApp_io" VALUES('boilerBurner','Ti2','Input natural gas temperature (K)',2,0,'20',0,6,'Вхідн. температура природного газу (К)','Вх. температура природного газа (K)','','');
INSERT INTO "techApp_io" VALUES('boilerBurner','Si2','Input natural gas pipe cutset (m2)',2,0,'',0,7,'Вхідн. перетин труби природного газу (м2)','Вх. сечение трубы природного газа (м2)','','');
INSERT INTO "techApp_io" VALUES('boilerBurner','Fi3','Input coke oven gas flow (tone/h)',2,1,'',0,8,'Вхідн. витрати коксового газу (т/год)','Вх. расход коксового газа (т/ч)','','');
INSERT INTO "techApp_io" VALUES('boilerBurner','Pi3','Input coke oven gas pressure (at)',2,0,'',0,9,'Вхідн. тиск коксового газу (ата)','Вх. давление коксового газа (ата)','','');
INSERT INTO "techApp_io" VALUES('boilerBurner','Ti3','Input coke oven gas temperature (K)',2,0,'0',0,10,'Вхідн. температура коксового газу (К)','Вх. температура коксового газа (K)','','');
INSERT INTO "techApp_io" VALUES('boilerBurner','Si3','Input coke oven gas pipe cutset (m2)',2,0,'',0,11,'Вхідн. перетин труби коксового газу (м2)','Вх. сечение трубы коксового газа (м2)','','');
INSERT INTO "techApp_io" VALUES('boilerBurner','Fi4','Input air flow (tone/h)',2,1,'',0,12,'Вхідн. витрати повітря (т/год)','Вх. расход воздуха (т/ч)','','');
INSERT INTO "techApp_io" VALUES('boilerBurner','Pi4','Input air pressure (at)',2,0,'',0,13,'Вхідн. тиск повітря (ата)','Вх. давление воздуха (ата)','','');
INSERT INTO "techApp_io" VALUES('boilerBurner','Ti4','Input air temperature (K)',2,0,'20',0,14,'Вхідн. температура повітря (К)','Вх. температура воздуха (K)','','');
INSERT INTO "techApp_io" VALUES('boilerBurner','Si4','Input air cutset (m2)',2,0,'',0,15,'Вхідн. перетин труби повітря (м2)','Вх. сечение трубы воздуха (м2)','','');
INSERT INTO "techApp_io" VALUES('boilerBurner','Fo','Output smoke gas flow (tones/h)',2,0,'',0,16,'Вих. витрати димових газів (т/год)','Вых. расход дымовых газов (т/ч)','','');
INSERT INTO "techApp_io" VALUES('boilerBurner','Po','Output smoke gas pressure (at)',2,1,'',0,17,'Вих. тиск димових газів (ата)','Вых. давление и в топке (ата)','','');
INSERT INTO "techApp_io" VALUES('boilerBurner','To','Output smoke gas temperature (K)',2,1,'',0,18,'Вих. температура димових газів (К)','Вых. температура и в топке (K)','','');
INSERT INTO "techApp_io" VALUES('boilerBurner','So','Output smoke gas pipe cutset (m2)',2,0,'90',0,19,'Вих. перетин труби димових газів (м2)','Вых. сечение трубы дымовых газов (м2)','','');
INSERT INTO "techApp_io" VALUES('boilerBurner','lo','Output smoke gas pipe length (m)',2,0,'',0,20,'Вих. довжина труби димових газів (м)','Вых. длина трубы дымовых газов (м)','','');
INSERT INTO "techApp_io" VALUES('boilerBurner','V','Burner volume (m3)',2,0,'830',0,21,'Об''єм топки (м3)','Объём топки(м3)','','');
INSERT INTO "techApp_io" VALUES('boilerBurner','CO','The percentage of CO in the flue stack gases (%)',2,1,'',0,22,'Відсоток вмісту CO у димових газах (%)','Процент содержания CO в уходящих дымовых газах (%)','','');
INSERT INTO "techApp_io" VALUES('boilerBurner','O2','The percentage of O2 in the flue stack gases (%)',2,1,'',0,23,'Відсоток вмісту O2 у димових газах (%)','Процент содержания O2 в уходящих дымовых газах (%)','','');
INSERT INTO "techApp_io" VALUES('boilerBurner','f_frq','Calc frequency (Hz)',2,0,'200',0,24,'Частота обчислення (Гц)','Частота вычисления (Гц)','','');
INSERT INTO "techApp_io" VALUES('pipe2_1','Fi1','Input 1 flow (tones/h)',2,1,'0',0,0,'Вхідні витрати 1 (т/год)','Входн. расход 1 (т/ч)','','');
INSERT INTO "techApp_io" VALUES('pipe2_1','Pi1','Input 1 pressure (at)',2,0,'1',0,1,'Вхідний тиск 1 (ата)','Входн. давление 1 (ата)','','');
INSERT INTO "techApp_io" VALUES('pipe2_1','Ti1','Input 1 temperature (K)',2,0,'273',0,2,'Вхідна температура 1 (К)','Входн. температура 1 (К)','','');
INSERT INTO "techApp_io" VALUES('pipe2_1','Si1','Input 1 cutset (m2)',2,0,'0.2',0,3,'Вхідний перетин 1 (м2)','Входн. сечение 1 (м2)','','');
INSERT INTO "techApp_io" VALUES('pipe2_1','Fi2','Input 2 flow (tones/h)',2,1,'0',0,4,'Вхідні витрати 2 (т/год)','Входн. расход 2 (т/ч)','','');
INSERT INTO "techApp_io" VALUES('pipe2_1','Pi2','Input 2 pressure (at)',2,0,'1',0,5,'Вхідний тиск 2 (ата)','Входн. давление 2 (ата)','','');
INSERT INTO "techApp_io" VALUES('pipe2_1','Ti2','Input 2 temperature (K)',2,0,'273',0,6,'Вхідна температура 2 (К)','Входн. температура 2 (К)','','');
INSERT INTO "techApp_io" VALUES('pipe2_1','Si2','Input 2 cutset (m2)',2,0,'0.2',0,7,'Вхідний перетин 2 (м2)','Входн. сечение 2 (м2)','','');
INSERT INTO "techApp_io" VALUES('pipe2_1','Fo','Output flow (tones/h)',2,0,'0',0,8,'Вих. витрати (т/год)','Вых. расход (т/ч)','','');
INSERT INTO "techApp_io" VALUES('pipe2_1','Po','Output pressure (at)',2,1,'1',0,9,'Вих. тиск (ата)','Вых. давление (ата)','','');
INSERT INTO "techApp_io" VALUES('pipe2_1','To','Output temperature (K)',2,1,'273',0,10,'Вих. температура (К)','Вых. температура (К)','','');
INSERT INTO "techApp_io" VALUES('pipe2_1','So','Output cutset (m2)',2,0,'.2',0,11,'Вих. перетин (м2)','Вых. сечение (м2)','','');
INSERT INTO "techApp_io" VALUES('pipe2_1','lo','Output length (m)',2,0,'10',0,12,'Вих. довжина (м)','Вых. длина (м)','','');
INSERT INTO "techApp_io" VALUES('pipe2_1','Q0','Norm density of environs (kg/m3)',2,0,'1',0,13,'Норм. щільність середовища (кг/м3)','Норм. плотн. среды (кг/м3)','','');
INSERT INTO "techApp_io" VALUES('pipe2_1','Kpr','Compressibility coefficient (0...1)',2,0,'0.95',0,14,'Коефіц. стиснення середовища (0...1)','Коэфиц. сжим. среды (0...1)','','');
INSERT INTO "techApp_io" VALUES('pipe2_1','Ct','Warm capacity of environs',2,0,'20',0,15,'Теплоємність середовища','Теплоёмкость среды','','');
INSERT INTO "techApp_io" VALUES('pipe2_1','Riz','Warm resistance of isolation',2,0,'20',0,16,'Тепл. опір ізоляції','Тепл. сопр. изоляции','','');
INSERT INTO "techApp_io" VALUES('pipe2_1','Fwind','Air speed',2,0,'1',0,17,'Швидкість повітря','Скорость воздуха','','');
INSERT INTO "techApp_io" VALUES('pipe2_1','Twind','Air temperature (К)',2,0,'273',0,18,'Темпер. повітря (К)','Температура воздуха (К)','','');
INSERT INTO "techApp_io" VALUES('pipe2_1','f_frq','Calc frequency (Hz)',2,0,'100',1,19,'Частота обчислення (Гц)','Частота вычисления (Гц)','','');
INSERT INTO "techApp_io" VALUES('pipe1_3','Kpr','Compressibility coefficient (0...1)',2,0,'0.95',0,15,'Коефіц. стиснення середовища (0...1)','Коэфиц. сжим. среды (0...1)','','');
INSERT INTO "techApp_io" VALUES('pipe1_4','Kpr','Compressibility coefficient (0...1)',2,0,'0.95',0,19,'Коефіц. стиснення середовища (0...1)','Коэфиц. сжим. среды (0...1)','','');
INSERT INTO "techApp_io" VALUES('diafragma','Kpr','Compressibility coefficient (0...1)',2,0,'0.95',0,9,'Коефіц. стиснення середовища (0...1)','Коэфиц. сжим. среды (0...1)','','');
CREATE TABLE 'tmplib_DevLib_io' ("TMPL_ID" TEXT DEFAULT '' ,"ID" TEXT DEFAULT '' ,"NAME" TEXT DEFAULT '' ,"TYPE" INTEGER DEFAULT '' ,"FLAGS" INTEGER DEFAULT '' ,"VALUE" TEXT DEFAULT '' ,"POS" INTEGER DEFAULT '' ,"ru#NAME" TEXT DEFAULT '' ,"ru#VALUE" TEXT DEFAULT '' ,"uk#NAME" TEXT DEFAULT '' ,"uk#VALUE" TEXT DEFAULT '' , PRIMARY KEY ("TMPL_ID","ID"));
INSERT INTO "tmplib_DevLib_io" VALUES('SCU750','transport','Transport',0,64,'SCU750',0,'Транспорт','','Транспорт','');
INSERT INTO "tmplib_DevLib_io" VALUES('SCU750','addr','Device address (-1...255)',1,64,'1',1,'Адрес устройства (-1...255)','','Адреса пристрою (-1...255)','');
INSERT INTO "tmplib_DevLib_io" VALUES('SCU750','perGet','Period data getting (s)',2,64,'10',2,'Период получения данных (с)','','Період отримання даних (с)','');
INSERT INTO "tmplib_DevLib_io" VALUES('SCU750','version','Version',0,16,'',3,'Версия','','Версія','');
INSERT INTO "tmplib_DevLib_io" VALUES('SCU750','snCntrUnit','Serial number: Control Unit',0,16,'',4,'Серийный номер: Блок Управления','','Серійний номер: Блок Керування','');
INSERT INTO "tmplib_DevLib_io" VALUES('SCU750','snPump','Serial number: Pump',0,16,'',5,'Серийный номер: Насос','','Серійний номер: Насос','');
INSERT INTO "tmplib_DevLib_io" VALUES('SCU750','cntPumpHour','Pump hour counter (minutes)',2,16,'',6,'Счётчик часов насоса (минуты)','','Лічильник годин насосу (хвилини)','');
INSERT INTO "tmplib_DevLib_io" VALUES('SCU750','cntCntrUnitHour','Control unit hour counter (minutes)',2,16,'',7,'Счётчик часов объекта контроля (минуты)','','Лічильник годин об''єкта контролю (хвилини)','');
INSERT INTO "tmplib_DevLib_io" VALUES('SCU750','cntStart','Start counter',2,16,'',8,'Запуск счётчика','','Пуск лічильника','');
INSERT INTO "tmplib_DevLib_io" VALUES('SCU750','operMode','Pump Operation Mode',0,16,'',9,'Режим работы насоса','','Режим роботи насосу','');
INSERT INTO "tmplib_DevLib_io" VALUES('SCU750','errors','Errors',0,16,'',10,'Ошибки','','Помилки','');
INSERT INTO "tmplib_DevLib_io" VALUES('SCU750','events','Events',0,16,'',11,'События','','Події','');
INSERT INTO "tmplib_DevLib_io" VALUES('SCU750','tTMS','TMS temperature (°С)',1,16,'',12,'TMS температура (°С)','','TMS температура (°С)','');
INSERT INTO "tmplib_DevLib_io" VALUES('SCU750','tMotor','Motor temperature (°С)',1,16,'',13,'Температура мотора (°С)','','Температура мотору (°С)','');
INSERT INTO "tmplib_DevLib_io" VALUES('SCU750','rotSpeed','Rotational speed (HZ)',1,16,'',14,'Скорость вращения (Гц)','','Швидкість обертання (Гц)','');
INSERT INTO "tmplib_DevLib_io" VALUES('SCU750','comStart','Command: START',3,32,'',15,'Команда: ЗАПУСК','','Команда: ЗАПУСК','');
INSERT INTO "tmplib_DevLib_io" VALUES('SCU750','comStop','Command: STOP',3,32,'',16,'Команда: СТОП','','Команда: СТОП','');
INSERT INTO "tmplib_DevLib_io" VALUES('SCU750','comReset','Command: RESET',3,32,'',17,'Команда: СБРОС','','Команда: СКИД','');
INSERT INTO "tmplib_DevLib_io" VALUES('SCU750','spSpeed','Set point: Speed',1,32,'',18,'Задание: Скорость','','Завдання: Швидкість','');
INSERT INTO "tmplib_DevLib_io" VALUES('SCU750','spTMSt','Set point: TMS temperature',1,16,'',19,'Задание: температура TMS','','Завдання: температура TMS','');
INSERT INTO "tmplib_DevLib_io" VALUES('SCU750','spSpeedPrev','Set point: Speed (previous)',1,1,'',20,'Задание: Скорость (предыдущая)','','Завдання: Швидкість (попередня)','');
INSERT INTO "tmplib_DevLib_io" VALUES('SCU750','cnt','Counter',1,1,'',21,'Счётчик','','Лічильник','');
INSERT INTO "tmplib_DevLib_io" VALUES('SCU750','operModes','Operation modes',4,1,'',22,'Режимы работы','','Режими роботи','');
INSERT INTO "tmplib_DevLib_io" VALUES('TMH','transport','Transport',0,64,'TMH',0,'Транспорт','','Транспорт','');
INSERT INTO "tmplib_DevLib_io" VALUES('TMH','addr','Device address (-1...255)',1,64,'1',1,'Адрес устройства (-1...255)','','Адреса пристрою (-1...255)','');
INSERT INTO "tmplib_DevLib_io" VALUES('TMH','perGet','Period data getting (s)',2,64,'10',2,'Период получения данных (с)','','Період отримання даних (с)','');
INSERT INTO "tmplib_DevLib_io" VALUES('TMH','modelID','Model ID',0,16,'',3,'ID модели','','ID моделі','');
INSERT INTO "tmplib_DevLib_io" VALUES('TMH','errors','Errors',0,16,'',4,'Ошибки','','Помилки','');
INSERT INTO "tmplib_DevLib_io" VALUES('TMH','rotSpeed','Rotational speed (HZ)',1,16,'',5,'Скорость вращения (Гц)','','Швидкість обертання (Гц)','');
INSERT INTO "tmplib_DevLib_io" VALUES('TMH','I','Current (A)',2,16,'',6,'Ток (А)','','Струм (А)','');
INSERT INTO "tmplib_DevLib_io" VALUES('TMH','axle1disb','Axle 1 disbalance',1,16,'',7,'Нарушение границы 1','','Порушення границі 1','');
INSERT INTO "tmplib_DevLib_io" VALUES('TMH','axle2disb','Axle 2 disbalance',1,16,'',8,'Нарушение границы 2','','Порушення границі 2','');
INSERT INTO "tmplib_DevLib_io" VALUES('TMH','MP_X1','MP X1',1,16,'',9,'','','','');
INSERT INTO "tmplib_DevLib_io" VALUES('TMH','MP_Y1','MP Y1',1,16,'',10,'','','','');
INSERT INTO "tmplib_DevLib_io" VALUES('TMH','MP_X2','MP X2',1,16,'',11,'','','','');
INSERT INTO "tmplib_DevLib_io" VALUES('TMH','MP_Y2','MP Y2',1,16,'',12,'','','','');
INSERT INTO "tmplib_DevLib_io" VALUES('TMH','MP_Z','MP Z',1,16,'',13,'','','','');
INSERT INTO "tmplib_DevLib_io" VALUES('TMH','operMode','Operation Mode',0,16,'',14,'Режим работы','','Режим роботи','');
INSERT INTO "tmplib_DevLib_io" VALUES('TMH','comStart','Command: START',3,32,'',15,'Команда: ЗАПУСК','','Команда: ЗАПУСК','');
INSERT INTO "tmplib_DevLib_io" VALUES('TMH','comStop','Command: STOP',3,32,'',16,'Команда: СТОП','','Команда: СТОП','');
INSERT INTO "tmplib_DevLib_io" VALUES('TMH','comReset','Command: RESET',3,32,'',17,'Команда: СБРОС','','Команда: СКИД','');
INSERT INTO "tmplib_DevLib_io" VALUES('TMH','operCntr','Operation Control Mode',0,16,'',18,'Режим работы и контроля','','Режим роботи та контролю','');
INSERT INTO "tmplib_DevLib_io" VALUES('TMH','comInteract','Interactive mode',3,32,'',19,'Интерактивный режим','','Інтерактивний режим','');
INSERT INTO "tmplib_DevLib_io" VALUES('TMH','comAutonom','Autonomous mode',3,32,'',20,'Автономный режим','','Автономний режим','');
INSERT INTO "tmplib_DevLib_io" VALUES('TMH','cnt','Counter',1,1,'',21,'Счётчик','','Лічильник','');
INSERT INTO "tmplib_DevLib_io" VALUES('TMH','operModes','Operation modes',4,1,'',22,'Режимы работы','','Режими роботи','');
INSERT INTO "tmplib_DevLib_io" VALUES('TM510x','transport','Transport',0,64,'tm510x',0,'Транспорт','','Транспорт','');
INSERT INTO "tmplib_DevLib_io" VALUES('TM510x','addr','Device address (1...254)',1,64,'1',1,'Адрес устройства (1...254)','','Адреса пристрою (1...254)','');
INSERT INTO "tmplib_DevLib_io" VALUES('TM510x','devTp','Device type',1,16,'',2,'Тип устройства','','Тип пристрою','');
INSERT INTO "tmplib_DevLib_io" VALUES('TM510x','errors','Errors',0,16,'',3,'Ошибки','','Помилки','');
INSERT INTO "tmplib_DevLib_io" VALUES('TM510x','in1','Input 1',2,16,'',4,'Вход 1','','Вхід 1','');
INSERT INTO "tmplib_DevLib_io" VALUES('TM510x','in2','Input 2',2,16,'',5,'Вход 2','','Вхід 2','');
INSERT INTO "tmplib_DevLib_io" VALUES('TM510x','in3','Input 3',2,16,'',6,'Вход 3','','Вхід 3','');
INSERT INTO "tmplib_DevLib_io" VALUES('TM510x','in4','Input 4',2,16,'',7,'Вход 4','','Вхід 4','');
INSERT INTO "tmplib_DevLib_io" VALUES('TM510x','in5','Input 5',2,16,'',8,'Вход 5','','Вхід 5','');
INSERT INTO "tmplib_DevLib_io" VALUES('TM510x','in6','Input 6',2,16,'',9,'Вход 6','','Вхід 6','');
INSERT INTO "tmplib_DevLib_io" VALUES('TM510x','in7','Input 7',2,16,'',10,'Вход 7','','Вхід 7','');
INSERT INTO "tmplib_DevLib_io" VALUES('TM510x','in8','Input 8',2,16,'',11,'Вход 8','','Вхід 8','');
INSERT INTO "tmplib_DevLib_io" VALUES('UPS','srcAddr','Source object''s address',0,64,'',0,'Адрес исходного объекта','','Адреса вихідного об''єкту','');
INSERT INTO "tmplib_DevLib_io" VALUES('UPS','items','All items',4,33,'',1,'Все элементы','','Всі елементи','');
INSERT INTO "tmplib_DevLib_io" VALUES('UPS','this','The object',4,0,'',15,'Объект','','Об''єкт','');
INSERT INTO "tmplib_DevLib_io" VALUES('UPS','SHIFR','Code',0,0,'',16,'Шифр','','Шифр','');
INSERT INTO "tmplib_DevLib_io" VALUES('UPS','NAME','Name',0,0,'',17,'Имя','','Ім''я','');
INSERT INTO "tmplib_DevLib_io" VALUES('UPS','DESCR','Description',0,0,'',18,'Описание','','Опис','');
INSERT INTO "tmplib_DevLib_io" VALUES('VKT7','imit','Imitation drift % (0-disable)',2,64,'0',0,'Дрейф имитации % (0-отключен)','','Дрейф імітації % (0-відключено)','');
INSERT INTO "tmplib_DevLib_io" VALUES('VKT7','trAddr','Transport',0,64,'Transport.Serial.out_VKT7',1,'Транспорт','','Транспорт','');
INSERT INTO "tmplib_DevLib_io" VALUES('VKT7','arhH','Archiver: hours',0,64,'',2,'Архиватор: часы','','Архіватор: години','');
INSERT INTO "tmplib_DevLib_io" VALUES('VKT7','arhD','Archiver: days',0,64,'',3,'Архиватор: дни','','Архіватор: дні','');
INSERT INTO "tmplib_DevLib_io" VALUES('VKT7','arhRes','Archiver: results-month',0,64,'',4,'Архиватор: результаты-месяцы','','Архіватор: результати-місяці','');
INSERT INTO "tmplib_DevLib_io" VALUES('VKT7','maxDayDepth','Archiver: maximum depth of reading for not hours archivers, days',1,64,'366',5,'Архиватор: максимум читаемой глубины для нечасового архиватора, дней','','Архіватор: максимум глибини читання для негодинного архіватора, днів','');
INSERT INTO "tmplib_DevLib_io" VALUES('VKT7','lastResTm','Last result months read time (s)',1,33,'0',6,'Время последнего чтения результирующих месяцев (с)','','Час останього читання результуючих місяців (с)','');
INSERT INTO "tmplib_DevLib_io" VALUES('VKT7','leftResTm','Left result months for read from archive',1,17,'',7,'Осталось результ. месяцев для чтения из архива','','Залишилося результ. місяців для читання із архіву','');
INSERT INTO "tmplib_DevLib_io" VALUES('VKT7','lastDTm','Last days read time (s)',1,33,'0',8,'Время последнего чтения дней (с)','','Час останього читання днів (с)','');
INSERT INTO "tmplib_DevLib_io" VALUES('VKT7','leftDTm','Left days for read from archive',1,17,'',9,'Осталось дней для чтения из архива','','Залишилося днів для читання із архіву','');
INSERT INTO "tmplib_DevLib_io" VALUES('VKT7','lastHTm','Last hours read time (s)',1,33,'',10,'Время последнего чтения часов (с)','','Час останього читання годин (с)','');
INSERT INTO "tmplib_DevLib_io" VALUES('VKT7','leftHTm','Left hours for read from archive',1,17,'',11,'Осталось часов для чтения из архива','','Залишилося годин для читання із архіву','');
INSERT INTO "tmplib_DevLib_io" VALUES('VKT7','diffTm','Difference time (server-counter), hours',1,16,'',12,'Разница времени (сервер-счётчик), часы','','Різниця часу (сервер-лічильник), години','');
INSERT INTO "tmplib_DevLib_io" VALUES('VKT7','firmVer','Firmware version',2,16,'',13,'Версия прошивки','','Версія прошивки','');
INSERT INTO "tmplib_DevLib_io" VALUES('VKT7','abonId','Subscriber ID',0,16,'',14,'ID абонента','','ID абоненту','');
INSERT INTO "tmplib_DevLib_io" VALUES('VKT7','repDay','Report day',1,16,'',15,'День отчёта','','День звіту','');
INSERT INTO "tmplib_DevLib_io" VALUES('VKT7','netNum','Network number',1,16,'',16,'Сетевой номер','','Мережевий номер','');
INSERT INTO "tmplib_DevLib_io" VALUES('VKT7','modelImpl','Model implementation',1,16,'',17,'Реализация модели','','Реалізація моделі','');
INSERT INTO "tmplib_DevLib_io" VALUES('VKT7','tTypeM','t: dimension',0,16,'',18,'t: ед. измерения','','t: од. виміру','');
INSERT INTO "tmplib_DevLib_io" VALUES('VKT7','GTypeM','G: dimension',0,16,'',19,'G: ед. измерения','','G: од. виміру','');
INSERT INTO "tmplib_DevLib_io" VALUES('VKT7','VTypeM','V: dimension',0,16,'',20,'V: ед. измерения','','V: од. виміру','');
INSERT INTO "tmplib_DevLib_io" VALUES('VKT7','MTypeM','M: dimension',0,16,'',21,'M: ед.измерения','','M: од.вимірювання','');
INSERT INTO "tmplib_DevLib_io" VALUES('VKT7','PTypeM','P: dimension',0,16,'',22,'P: ед. измерения','','P: од.виміру','');
INSERT INTO "tmplib_DevLib_io" VALUES('VKT7','QoTypeM','Qo: dimension',0,16,'',23,'Qo: ед. измерения','','Qo: од. виміру','');
INSERT INTO "tmplib_DevLib_io" VALUES('VKT7','QntTypeHIM','ВНР: dimension',0,16,'',24,'ВНР: ед.измерения','','ВНР: од.виміру','');
INSERT INTO "tmplib_DevLib_io" VALUES('VKT7','QntTypeM','ВОК: dimension',0,16,'',25,'ВОК: ед.измерения','','ВОК: од.виміру','');
INSERT INTO "tmplib_DevLib_io" VALUES('VKT7','t1_1','t1 (Tв1)',2,32,'',26,'','','','');
INSERT INTO "tmplib_DevLib_io" VALUES('VKT7','t2_1','t2 (Tв1)',2,32,'',27,'','','','');
INSERT INTO "tmplib_DevLib_io" VALUES('VKT7','t3_1','t3 (Tв1)',2,32,'',28,'','','','');
INSERT INTO "tmplib_DevLib_io" VALUES('VKT7','V1_1','V1 (Tв1)',2,32,'',29,'','','','');
INSERT INTO "tmplib_DevLib_io" VALUES('VKT7','V2_1','V2 (Tв1)',2,32,'',30,'','','','');
INSERT INTO "tmplib_DevLib_io" VALUES('VKT7','V3_1','V3 (Tв1)',2,32,'',31,'','','','');
INSERT INTO "tmplib_DevLib_io" VALUES('VKT7','M1_1','M1 (Tв1)',2,32,'',32,'','','','');
INSERT INTO "tmplib_DevLib_io" VALUES('VKT7','M2_1','M2 (Tв1)',2,32,'',33,'','','','');
INSERT INTO "tmplib_DevLib_io" VALUES('VKT7','M3_1','M3 (Tв1)',2,32,'',34,'','','','');
INSERT INTO "tmplib_DevLib_io" VALUES('VKT7','P1_1','P1 (Tв1)',2,32,'',35,'','','','');
INSERT INTO "tmplib_DevLib_io" VALUES('VKT7','P2_1','P2 (Tв1)',2,32,'',36,'','','','');
INSERT INTO "tmplib_DevLib_io" VALUES('VKT7','Mg_1','Mg (Tв1)',2,32,'',37,'','','','');
INSERT INTO "tmplib_DevLib_io" VALUES('VKT7','Qo_1','Qo (Tв1)',2,32,'',38,'','','','');
INSERT INTO "tmplib_DevLib_io" VALUES('VKT7','Qg_1','Qg (Tв1)',2,32,'',39,'','','','');
INSERT INTO "tmplib_DevLib_io" VALUES('VKT7','dt_1','dt (Tв1)',2,32,'',40,'','','','');
INSERT INTO "tmplib_DevLib_io" VALUES('VKT7','BNR_1','ВНР (Tв1)',2,32,'',41,'','','','');
INSERT INTO "tmplib_DevLib_io" VALUES('VKT7','BOC_1','ВОС (Tв1)',2,32,'',42,'','','','');
INSERT INTO "tmplib_DevLib_io" VALUES('VKT7','G1_1','G1 (Tв1)',2,32,'',43,'','','','');
INSERT INTO "tmplib_DevLib_io" VALUES('VKT7','G2_1','G2 (Tв1)',2,32,'',44,'','','','');
INSERT INTO "tmplib_DevLib_io" VALUES('VKT7','t1_2','t1 (Tв2)',2,32,'',45,'','','','');
INSERT INTO "tmplib_DevLib_io" VALUES('VKT7','t2_2','t2 (Tв2)',2,32,'',46,'','','','');
INSERT INTO "tmplib_DevLib_io" VALUES('VKT7','t3_2','t3 (Tв2)',2,32,'',47,'','','','');
INSERT INTO "tmplib_DevLib_io" VALUES('VKT7','V1_2','V1 (Tв2)',2,32,'',48,'','','','');
INSERT INTO "tmplib_DevLib_io" VALUES('VKT7','V2_2','V2 (Tв2)',2,32,'',49,'','','','');
INSERT INTO "tmplib_DevLib_io" VALUES('VKT7','V3_2','V3 (Tв2)',2,32,'',50,'','','','');
INSERT INTO "tmplib_DevLib_io" VALUES('VKT7','M1_2','M1 (Tв2)',2,32,'',51,'','','','');
INSERT INTO "tmplib_DevLib_io" VALUES('VKT7','M2_2','M2 (Tв2)',2,32,'',52,'','','','');
INSERT INTO "tmplib_DevLib_io" VALUES('VKT7','M3_2','M3 (Tв2)',2,32,'',53,'','','','');
INSERT INTO "tmplib_DevLib_io" VALUES('VKT7','P1_2','P1 (Tв2)',2,32,'',54,'','','','');
INSERT INTO "tmplib_DevLib_io" VALUES('VKT7','P2_2','P2 (Tв2)',2,32,'',55,'','','','');
INSERT INTO "tmplib_DevLib_io" VALUES('VKT7','Mg_2','Mg (Tв2)',2,32,'',56,'','','','');
INSERT INTO "tmplib_DevLib_io" VALUES('VKT7','Qo_2','Qo (Tв2)',2,32,'',57,'','','','');
INSERT INTO "tmplib_DevLib_io" VALUES('VKT7','Qg_2','Qg (Tв2)',2,32,'',58,'','','','');
INSERT INTO "tmplib_DevLib_io" VALUES('VKT7','dt_2','dt (Tв2)',2,32,'',59,'','','','');
INSERT INTO "tmplib_DevLib_io" VALUES('VKT7','BNR_2','ВНР (Tв2)',2,32,'',60,'','','','');
INSERT INTO "tmplib_DevLib_io" VALUES('VKT7','BOC_2','ВОС (Tв2)',2,32,'',61,'','','','');
INSERT INTO "tmplib_DevLib_io" VALUES('VKT7','G1_2','G1 (Tв2)',2,32,'',62,'','','','');
INSERT INTO "tmplib_DevLib_io" VALUES('VKT7','G2_2','G2 (Tв2)',2,32,'',63,'','','','');
INSERT INTO "tmplib_DevLib_io" VALUES('VKT7','this','This parameter object',4,0,'',64,'Объект параметра','','Об''єкт параметру','');
INSERT INTO "tmplib_DevLib_io" VALUES('SMDP','transport','Transport',0,64,'SMDP',0,'Транспорт','','Транспорт','');
INSERT INTO "tmplib_DevLib_io" VALUES('SMDP','addr','Device address (16...254)',1,64,'16',1,'Адрес устройства (16...254)','','Адреса пристрою (16...254)','');
INSERT INTO "tmplib_DevLib_io" VALUES('SMDP','perGet','Period of the data getting, s',2,64,'10',2,'Период сбора данных, с','','Період збору даних, с','');
INSERT INTO "tmplib_DevLib_io" VALUES('SMDP','version','Firmware version',0,16,'',3,'Версия прошивки','','Версія прошивки','');
INSERT INTO "tmplib_DevLib_io" VALUES('SMDP','CfgPrmSSID','Configuration parameter session ID',1,16,'',4,'Параметр конфигурации ID сеанса','','Параметр конфігурації ID сеансу','');
INSERT INTO "tmplib_DevLib_io" VALUES('SMDP','Srlno','Measurement serial number',1,16,'',5,'Измерянный номер серии','','Виміряний номер серії','');
INSERT INTO "tmplib_DevLib_io" VALUES('SMDP','RawFreq','Channel freq.',2,16,'',6,'Частота канала.','','Частота каналу.','');
INSERT INTO "tmplib_DevLib_io" VALUES('SMDP','GoodFreq','Last used, good channel freq., Hz',2,16,'',7,'Последне использ.: частота хорошего канала, Гц','','Останнє використ.: частота хорошого каналу, Гц','');
INSERT INTO "tmplib_DevLib_io" VALUES('SMDP','RawThick','Computed raw thickness, from frequency. (A)',2,16,'',8,'Вычисленная сырая толщина, с частоты. (А)','','Обчислена сира товщина, з частоти. (A)','');
INSERT INTO "tmplib_DevLib_io" VALUES('SMDP','XtalThick','Computed material related thickness, can be zeroed. (A)',2,16,'',9,'Вычисленная относительная толщина материала, может быть нулевой. (A)','','Обчислена відносна товщина матеріалу, може бути нульовою. (A)','');
INSERT INTO "tmplib_DevLib_io" VALUES('SMDP','XtalThick_F','Filtered computed material related thickness, can be zeroed. (A)',2,16,'',10,'Фильтрованная вычисленная относительная толщина материала, может быть нулевой. (A)','','Фільтрована обчислена відносна товщина матеріалу, може бути нульовою. (A)','');
INSERT INTO "tmplib_DevLib_io" VALUES('SMDP','XtalRate','Rate, angstroms per second.',2,16,'',11,'','','','');
INSERT INTO "tmplib_DevLib_io" VALUES('SMDP','XtalRate_F','Rate, angstroms per second, filtered.',2,16,'',12,'','','','');
INSERT INTO "tmplib_DevLib_io" VALUES('SMDP','XtalLife','XtalLife (%)',2,16,'',13,'','','','');
INSERT INTO "tmplib_DevLib_io" VALUES('SMDP','XtalQual','Quality level (0-9).',1,16,'',14,'','','','');
INSERT INTO "tmplib_DevLib_io" VALUES('SMDP','XtalQualPeak','Highest quality level seen (0-9).',1,16,'',15,'','','','');
INSERT INTO "tmplib_DevLib_io" VALUES('SMDP','XtalStab','Stability level (0-9).',1,16,'',16,'','','','');
INSERT INTO "tmplib_DevLib_io" VALUES('SMDP','XtalStabPeak','Highest stability level seen (0-9).',1,16,'',17,'','','','');
INSERT INTO "tmplib_DevLib_io" VALUES('SMDP','XtalStat','Channel status.',0,16,'',18,'Статус канала.','','Статус каналу.','');
INSERT INTO "tmplib_DevLib_io" VALUES('SMDP','XtalLife_C','XtalLife (%).',1,16,'',19,'','','','');
INSERT INTO "tmplib_DevLib_io" VALUES('SMDP','SessId','Session ID',1,32,'',20,'ID сеанса','','ID сеансу','');
INSERT INTO "tmplib_DevLib_io" VALUES('SMDP','Fq','Xtal start freq. (Hz)',2,32,'',21,'','','','');
INSERT INTO "tmplib_DevLib_io" VALUES('SMDP','Fm','Xtal min freq. (Hz)',2,32,'',22,'','','','');
INSERT INTO "tmplib_DevLib_io" VALUES('SMDP','Density','Material density (gm/cc).',2,32,'',23,'','','','');
INSERT INTO "tmplib_DevLib_io" VALUES('SMDP','Zratio','Material Z ratio. Not scaled or unitized.',2,32,'',24,'','','','');
INSERT INTO "tmplib_DevLib_io" VALUES('SMDP','Tooling','1.000 is 100 % tooling (unity).',2,32,'',25,'1.000 есть 100 % настройка (объединение).','','1.000 є 100 % налаштування (об''єднання).','');
INSERT INTO "tmplib_DevLib_io" VALUES('SMDP','RateReq','Requested rate (A/S).',2,32,'',26,'','','','');
INSERT INTO "tmplib_DevLib_io" VALUES('SMDP','QlvlTrip','Quality threshold, if non 0 and exceeded, xtal fail occurs.',1,32,'',27,'','','','');
INSERT INTO "tmplib_DevLib_io" VALUES('SMDP','SlvlTrip','Stability threshold, if non 0 and exceeded, xtal fail occurs.',1,32,'',28,'','','','');
INSERT INTO "tmplib_DevLib_io" VALUES('IT3','transport','Transport',0,64,'IT3',0,'Транспорт','','Транспорт','');
INSERT INTO "tmplib_DevLib_io" VALUES('IT3','addr','Device address',1,64,'1',1,'Адрес устройства','','Адреса пристрою','');
INSERT INTO "tmplib_DevLib_io" VALUES('IT3','T','Temperature',2,16,'',3,'Температура','','Температура','');
INSERT INTO "tmplib_DevLib_io" VALUES('IT3','H','Upper border',3,16,'',4,'Верхняя граница','','Верхня границя','');
INSERT INTO "tmplib_DevLib_io" VALUES('IT3','L','Lower border',3,16,'',5,'Нижняя граница','','Нижня границя','');
INSERT INTO "tmplib_DevLib_io" VALUES('IT3','relSt','Relay state',3,16,'',6,'Состояние реле','','Стан реле','');
INSERT INTO "tmplib_DevLib_io" VALUES('IVE_452HS_02','transport','Transport',0,64,'Sockets.out_IVE',0,'Транспорт','','Транспорт','');
INSERT INTO "tmplib_DevLib_io" VALUES('IVE_452HS_02','addr','Device address',1,64,'255',1,'Адрес устройства','','Адреса пристрою','');
INSERT INTO "tmplib_DevLib_io" VALUES('IVE_452HS_02','COIA','Output current level set-point',1,33,'',2,'Задание уровня выходного тока','','Завдання рівня вихідного струму','');
INSERT INTO "tmplib_DevLib_io" VALUES('IVE_452HS_02','COUA','Output voltage level set-point',1,33,'',3,'Задание уровня выходного напряжения','','Завдання рівня вихідної напруги','');
INSERT INTO "tmplib_DevLib_io" VALUES('IVE_452HS_02','COPA','Output power level set-point',1,33,'',4,'Задание уровня выходного мощности','','Завдання рівня вихідної потужності','');
INSERT INTO "tmplib_DevLib_io" VALUES('IVE_452HS_02','DIA','Output current',1,16,'',5,'Выходной ток','','Вихідний струм','');
INSERT INTO "tmplib_DevLib_io" VALUES('IVE_452HS_02','DUA','Output voltage',1,16,'',6,'Выходное напряжение','','Вихідна напруга','');
INSERT INTO "tmplib_DevLib_io" VALUES('IVE_452HS_02','DPA','Output power',1,16,'',7,'Выходная мощность','','Вихідна потужність','');
INSERT INTO "tmplib_DevLib_io" VALUES('IVE_452HS_02','DFA','Electric arcs frequency',1,16,'',8,'Частота появления "микродуг"','','Частота виникнення "мікродуг"','');
INSERT INTO "tmplib_DevLib_io" VALUES('IVE_452HS_02','DAC','Electric arcs counter',1,16,'',9,'Счетчик возникновения дуги','','Лічильник виникнення дуги','');
INSERT INTO "tmplib_DevLib_io" VALUES('IVE_452HS_02','COM_DEW','Command: Blocking by SC and XX',3,32,'',10,'Команда: Блокировка отключения выхода по "КЗ" и "ХХ"','','Команда: Блокування відключеня виходу по "КЗ" та "ХХ"','');
INSERT INTO "tmplib_DevLib_io" VALUES('IVE_452HS_02','COM_OUT','Command: Display on UI out block''s params',3,32,'',11,'Команда: Отображение на «УИ» выходных/опорных параметров каналов блока','','Команда: Відображення на «УІ» вихідних/опорних параметрів каналів блоку','');
INSERT INTO "tmplib_DevLib_io" VALUES('IVE_452HS_02','COM_DEP','Command: Enable MK',3,32,'',12,'Команда: Включение "МК"','','Команда: Включення "МК"','');
INSERT INTO "tmplib_DevLib_io" VALUES('IVE_452HS_02','COM_DEL','Command: Blocks #1,2 to line',3,32,'',13,'Команда: Включение блока "по сети" - канал№1,2','','Команда: Включення блоку "по мережі" - канал№1,2','');
INSERT INTO "tmplib_DevLib_io" VALUES('IVE_452HS_02','COM_UF','Command: Display on UI frequency and current',3,32,'',14,'Команда: Отображение на «УИ» напряжения и тока/частоты и мощности блока','','Команда: Відображення на «УІ» напруги та струму/частоти та потужності блоку','');
INSERT INTO "tmplib_DevLib_io" VALUES('IVE_452HS_02','DKW','MUBR of the block overheated',3,16,'',15,'Перегрев MUBR блока','','Перегрів MUBR блоку','');
INSERT INTO "tmplib_DevLib_io" VALUES('IVE_452HS_02','DKZ','Short circuits on the block out',3,16,'',16,'Короткое замыкание выходных цепей блока','','Коротке замикання вихідних ланцюгів блоку','');
INSERT INTO "tmplib_DevLib_io" VALUES('IVE_452HS_02','DK','MK of the block overheated',3,16,'',17,'Перегрев MK блока','','Перегрів MK блоку','');
INSERT INTO "tmplib_DevLib_io" VALUES('IVE_452HS_02','DE','Power and voltage on out present',3,16,'',18,'Мощность и напряжение есть на выходе','','Потужність та напруга є на виході','');
INSERT INTO "tmplib_DevLib_io" VALUES('OPTRIS','transport','Transport',0,64,'Serial.out_OPTRIS',0,'Транспорт','','Транспорт','');
INSERT INTO "tmplib_DevLib_io" VALUES('OPTRIS','addr','Device address',3,64,'1',1,'Адрес устройства','','Адреса пристрою','');
INSERT INTO "tmplib_DevLib_io" VALUES('OPTRIS','Tproc','T process',2,16,'',2,'Т обработки','','Т обробки','');
INSERT INTO "tmplib_DevLib_io" VALUES('OPTRIS','Thead','T head',2,16,'',3,'Т головы','','Т голови','');
INSERT INTO "tmplib_DevLib_io" VALUES('OPTRIS','Tbox','T box',2,16,'',4,'Т корпуса','','Т корпусу','');
INSERT INTO "tmplib_DevLib_io" VALUES('OPTRIS','Tact','T act.',2,16,'',5,'Т акт.','','Т акт.','');
INSERT INTO "tmplib_DevLib_io" VALUES('OPTRIS','eps','IR epsilon',2,32,'',6,'IR эпсилон','','IR епсилон','');
INSERT INTO "tmplib_DevLib_io" VALUES('OPTRIS','trans','IR transmission',2,32,'',7,'IR передачи','','IR передавання','');
INSERT INTO "tmplib_DevLib_io" VALUES('OPTRIS','spIll','Spot illumination',3,32,'',8,'Точечное освещение','','Точкове освітлення','');
INSERT INTO "tmplib_DevLib_io" VALUES('CTR','transport','Transport',0,64,'Serial.out_CTR',0,'Транспорт','','Транспорт','');
INSERT INTO "tmplib_DevLib_io" VALUES('CTR','press','Pressure, Tor',2,17,'',1,'Давление, Tor','','Тиск, Tor','');
INSERT INTO "tmplib_DevLib_io" VALUES('CTR','zeroSet','Zero set',3,32,'',2,'Установка нуля','','Встановлення нуля','');
INSERT INTO "tmplib_DevLib_io" VALUES('IEC60870','transport','Transport',0,64,'Sockets.out_IEC60870',0,'Транспорт','','Транспорт','');
INSERT INTO "tmplib_DevLib_io" VALUES('IEC60870','addr','Address, {addr}.{OA}',0,64,'0.5',1,'Адрес, {addr}.{OA}','','Адреса, {addr}.{OA}','');
INSERT INTO "tmplib_DevLib_io" VALUES('IEC60870','tmRetr','Retry connection time, s',2,64,'10',2,'Время восстановления подключения, с','','Час відновлення підключення, с','');
INSERT INTO "tmplib_DevLib_io" VALUES('IEC60870','t1','Acknowledge lack timeout, s',2,64,'1.5',3,'Таймаут подтверждения отсутствия активности, с','','Таймаут підтвердження відсутності активності, с','');
INSERT INTO "tmplib_DevLib_io" VALUES('IEC60870','t2','Acknowledge timeout, s',2,64,'1',4,'Таймаут подтверждения, с','','Таймаут підтвердження, с','');
INSERT INTO "tmplib_DevLib_io" VALUES('IEC60870','t3','Test timeout, s',2,64,'2',5,'Таймаут проверки, с','','Таймаут перевірки, с','');
INSERT INTO "tmplib_DevLib_io" VALUES('IEC60870','k','Maximum unconfirmed',1,64,'12',6,'Максимально неподтверждённых','','Максимально непідтверджених','');
INSERT INTO "tmplib_DevLib_io" VALUES('IEC60870','w','Maximum no ack',1,64,'8',7,'Максимальное отсутсвие "ack"','','Максимальна відсутність "ack"','');
INSERT INTO "tmplib_DevLib_io" VALUES('IEC60870','itemsSet','Items set by: "ai|di|do:{IOA}[-{EndIOA}]:a[:{NameBase}]"',0,36,'',8,'Небор элементов по: "ai|di|do:{IOA}[-{EndIOA}]:a[:{NameBase}]"','','Набір елементів за: "ai|di|do:{IOA}[-{EndIOA}]:a[:{NameBase}]"','');
INSERT INTO "tmplib_DevLib_io" VALUES('IEC60870','items','All items',4,33,'',9,'Все елементы','','Всі елементи','');
INSERT INTO "tmplib_DevLib_io" VALUES('IEC60870','this','Object',4,0,'',10,'Объект','','Об''єкт','');
INSERT INTO "tmplib_DevLib_io" VALUES('SSCP','transport','Transport of the Ethernet network, Sockets',0,64,'SSCP',0,'Транспорт сети Ethernet, Сокеты','','Транспорт мережі Ethernet, Сокети','');
INSERT INTO "tmplib_DevLib_io" VALUES('SSCP','addr','Address [0...255]',1,64,'1',1,'Адрес [0...255]','','Адреса [0...255]','');
INSERT INTO "tmplib_DevLib_io" VALUES('SSCP','user','User',0,64,'admin',2,'Пользователь','','Користувач','');
INSERT INTO "tmplib_DevLib_io" VALUES('SSCP','pass','Password',0,64,'rw',3,'Пароль','','Пароль','');
INSERT INTO "tmplib_DevLib_io" VALUES('SSCP','list','Selected to process variables',0,36,'',8,'Переменные, выбранные для обработки','','Змінні, обрані для обробки','');
INSERT INTO "tmplib_DevLib_io" VALUES('SSCP','listsDir','List files'' folder',0,64,'SSCP',5,'Директория файлов-списков','','Тека файлів-переліків','');
INSERT INTO "tmplib_DevLib_io" VALUES('SSCP','this','Object',4,0,'',9,'Объект','','Об''єкт','');
INSERT INTO "tmplib_DevLib_io" VALUES('SSCP','maxDtFrm','Maximum data frame size',1,64,'2048',4,'Максимальный размер фрейма данных','','Максимальний розмір фрейму даних','');
INSERT INTO "tmplib_DevLib_io" VALUES('SSCP','verPrt','Protocol version',1,16,'',6,'Версия протокола','','Версія протоколу','');
INSERT INTO "tmplib_DevLib_io" VALUES('SSCP','maxDtFrmServ','Server''s maximum data frame size',1,16,'',7,'Максимальный размер фрейма данных сервера','','Максимальний розмір фрейму даних серверу','');
INSERT INTO "tmplib_DevLib_io" VALUES('IT3','mdPass','Passive mode, writing by an input protocol''s part',3,64,'0',2,'Пассивный режим, запись входной частью протокола','','Пасивний режим, запис вхідною частиною протоколу','');
INSERT INTO "tmplib_DevLib_io" VALUES('IT3','this','Object',4,0,'',7,'Объект','','Об''єкт','');
INSERT INTO "tmplib_DevLib_io" VALUES('UPS','bChL','Battery charge low',2,32,'20',3,'Заряд батареи низкий','','Заряд батареї низький','');
INSERT INTO "tmplib_DevLib_io" VALUES('UPS','bChLL','Battery charge critical',2,32,'5',4,'Заряд батареи критический','','Заряд батареї критичний','');
INSERT INTO "tmplib_DevLib_io" VALUES('UPS','inVL','Input voltage low',2,32,'210',5,'Входное напряжение низкое','','Вхідна напруга низька','');
INSERT INTO "tmplib_DevLib_io" VALUES('UPS','inVH','Input voltage high',2,32,'250',6,'Входное напряжение высокое','','Вхідна напруга висока','');
INSERT INTO "tmplib_DevLib_io" VALUES('UPS','inFLL','Input frequency too low',2,32,'40',7,'Входная частота очень низкая','','Вхідна частота дуже низька','');
INSERT INTO "tmplib_DevLib_io" VALUES('UPS','inFL','Input frequency low',2,32,'45',8,'Входная частота низкая','','Вхідна частота низька','');
INSERT INTO "tmplib_DevLib_io" VALUES('UPS','inFH','Input frequency high',2,32,'55',9,'Входная частота высокая','','Вхідна частота висока','');
INSERT INTO "tmplib_DevLib_io" VALUES('UPS','inFHH','Input frequency too high',2,32,'60',10,'Входная частота очень высокая','','Вхідна частота дуже висока','');
INSERT INTO "tmplib_DevLib_io" VALUES('UPS','loadH','Load high',2,32,'80',11,'Нагрузка высокая','','Навантаження високе','');
INSERT INTO "tmplib_DevLib_io" VALUES('UPS','loadHH','Load too high',2,32,'100',12,'Нагрузка очень высокая','','Навантаження дуже високе','');
INSERT INTO "tmplib_DevLib_io" VALUES('UPS','tH','Temperature high',2,32,'50',13,'Температура высокая','','Температура висока','');
INSERT INTO "tmplib_DevLib_io" VALUES('UPS','tHH','Temperature too high',2,32,'70',14,'Температура очень высокая','','Температура дуже висока','');
INSERT INTO "tmplib_DevLib_io" VALUES('UPS','alDelay','Alarms delay, s',2,32,'0',2,'Задержка сигнализации, с','','Затримка сигналізації, с','');
INSERT INTO "tmplib_DevLib_io" VALUES('m200','s','Energy from reset, tariffs sum',0,16,'',0,'Энергия от сброса сумма Тарифов','','Енергія від скидання сума Тарифів','');
INSERT INTO "tmplib_DevLib_io" VALUES('m200','t1','Energy from reset, tariff 1',2,16,'',1,'Энергия от сброса Тариф1','','Енергія від скидання Тариф1','');
INSERT INTO "tmplib_DevLib_io" VALUES('m200','t2','Energy from reset, tariff 2',2,16,'',2,'Энергия от сброса Тариф2','','Енергія від скидання Тариф2','');
INSERT INTO "tmplib_DevLib_io" VALUES('m200','t3','Energy from reset, tariff 3',2,16,'',3,'Энергия от сброса Тариф3','','Енергія від скидання Тариф3','');
INSERT INTO "tmplib_DevLib_io" VALUES('m200','t4','Energy from reset, tariff 4',2,16,'',4,'Энергия от сброса Тариф4','','Енергія від скидання Тариф4','');
INSERT INTO "tmplib_DevLib_io" VALUES('m200','U','Voltage U, V',2,16,'',5,'Напряжение U, В','','Напруга U, В','');
INSERT INTO "tmplib_DevLib_io" VALUES('m200','I','Current I, A',2,16,'',6,'Ток I, А','','Струм I, А','');
INSERT INTO "tmplib_DevLib_io" VALUES('m200','P','Power P, W',2,16,'',7,'Мощность P, Вт','','Потужність P, Вт','');
INSERT INTO "tmplib_DevLib_io" VALUES('m200','transport','Transport',0,64,'merc200',8,'Транспорт','','Транспорт','');
INSERT INTO "tmplib_DevLib_io" VALUES('m200','netaddr','Network address',2,64,'1',9,'Сетевой адрес','','Мережева адреса','');
INSERT INTO "tmplib_DevLib_io" VALUES('m200','naladchik','Coded address by the program Fixer+',3,64,'0',10,'Адрес закодирован программой Наладчик+','','Адреса закодована програмою Налаштувач+','');
INSERT INTO "tmplib_DevLib_io" VALUES('m200','tarif','Read energy from reset',3,64,'1',11,'Считывать энергию от сброса','','Вичитувати енергію від скидання','');
INSERT INTO "tmplib_DevLib_io" VALUES('m200','UIP','Read current values',3,64,'1',12,'Считывать мгновенные значения','','Вичитувати миттеві значення','');
INSERT INTO "tmplib_DevLib_io" VALUES('m200','first','First byte of the address',1,0,'',13,'Первый байт адреса','','Перший байт адреси','');
INSERT INTO "tmplib_DevLib_io" VALUES('m200','second','Last 3 bytes of the address',1,0,'',14,'Последние 3 байта адреса','','Останні 3 байти адреси','');
INSERT INTO "tmplib_DevLib_io" VALUES('m230','answer','Answer to the password request',0,16,'',0,'Ответ на запрос пароля','','Відповідь на запит пароля','');
INSERT INTO "tmplib_DevLib_io" VALUES('m230','TS','Read energy of tariffs sum',3,64,'0',1,'Считывать энергию суммы тарифов','','Вичитувати енергію суми тарифів','');
INSERT INTO "tmplib_DevLib_io" VALUES('m230','T1','Read energy of tariff 1',3,64,'0',2,'Считывать энергию тарифа 1','','Вичитувати енергію тарифу 1','');
INSERT INTO "tmplib_DevLib_io" VALUES('m230','T2','Read energy of tariff 2',3,64,'0',3,'Считывать энергию тарифа 2','','Вичитувати енергію тарифу 2','');
INSERT INTO "tmplib_DevLib_io" VALUES('m230','T3','Read energy of tariff 3',3,64,'0',4,'Считывать энергию тарифа 3','','Вичитувати енергію тарифу 3','');
INSERT INTO "tmplib_DevLib_io" VALUES('m230','T4','Read energy of tariff 4',3,64,'0',5,'Считывать энергию тарифа 4','','Вичитувати енергію тарифу 4','');
INSERT INTO "tmplib_DevLib_io" VALUES('m230','P','Read power P',3,64,'0',6,'Считывать мощность P','','Вичитувати потужність P','');
INSERT INTO "tmplib_DevLib_io" VALUES('m230','Q','Read power Q',3,64,'0',7,'Считывать мощность Q','','Вичитувати потужність Q','');
INSERT INTO "tmplib_DevLib_io" VALUES('m230','S','Read power S',3,64,'0',8,'Считывать мощность S','','Вичитувати потужність S','');
INSERT INTO "tmplib_DevLib_io" VALUES('m230','U','Read voltage U',3,64,'0',9,'Считывать напряжение U','','Вичитувати напругу U','');
INSERT INTO "tmplib_DevLib_io" VALUES('m230','I','Read current I',3,64,'0',10,'Считывать ток I','','Вичитувати струм I','');
INSERT INTO "tmplib_DevLib_io" VALUES('m230','K','Read power coefficient',3,64,'0',11,'Считывать коэф. мощности','','Вичитувати коеф. потужності','');
INSERT INTO "tmplib_DevLib_io" VALUES('m230','F','Read frequency F',3,64,'0',12,'Считывать частоту F','','Вичитувати частоту F','');
INSERT INTO "tmplib_DevLib_io" VALUES('m230','N','Read serial number',3,64,'0',13,'Считывать серийный номер','','Вичитувати серійний номер','');
INSERT INTO "tmplib_DevLib_io" VALUES('m230','TSAp','Energy from resetting, tariffs sum A+',2,16,'',14,'Энергия от сброса сумма тарифов A+','','Енергія від скидання сума тарифів A+','');
INSERT INTO "tmplib_DevLib_io" VALUES('m230','TSAm','Energy from resetting, tariffs sum A-',2,16,'',15,'Энергия от сброса сумма тарифов A-','','Енергія від скидання сума тарифів A-','');
INSERT INTO "tmplib_DevLib_io" VALUES('m230','TSRp','Energy from resetting, tariffs sum R+',2,16,'',16,'Энергия от сброса сумма тарифов R+','','Енергія від скидання сума тарифів R+','');
INSERT INTO "tmplib_DevLib_io" VALUES('m230','TSRm','Energy from resetting, tariffs sum R-',2,16,'',17,'Энергия от сброса сумма тарифов R-','','Енергія від скидання сума тарифів R-','');
INSERT INTO "tmplib_DevLib_io" VALUES('m230','T1Ap','Energy from resetting, tariff 1 A+',2,16,'',18,'Энергия от сброса тариф 1 A+','','Енергія від скидання тариф 1 A+','');
INSERT INTO "tmplib_DevLib_io" VALUES('m230','T1Am','Energy from resetting, tariff 1 A-',2,16,'',19,'Энергия от сброса тариф 1 A-','','Енергія від скидання тариф 1 A-','');
INSERT INTO "tmplib_DevLib_io" VALUES('m230','T1Rp','Energy from resetting, tariff 1 R+',2,16,'',20,'Энергия от сброса тариф 1 R+','','Енергія від скидання тариф 1 R+','');
INSERT INTO "tmplib_DevLib_io" VALUES('m230','T1Rm','Energy from resetting, tariff 1 R-',2,16,'',21,'Энергия от сброса тариф 1 R-','','Енергія від скидання тариф 1 R-','');
INSERT INTO "tmplib_DevLib_io" VALUES('m230','T2Ap','Energy from resetting, tariff 2 A+',2,16,'',22,'Энергия от сброса тариф 2 A+','','Енергія від скидання тариф 2 A+','');
INSERT INTO "tmplib_DevLib_io" VALUES('m230','T2Am','Energy from resetting, tariff 2 A-',2,16,'',23,'Энергия от сброса тариф 2 A-','','Енергія від скидання тариф 2 A-','');
INSERT INTO "tmplib_DevLib_io" VALUES('m230','T2Rp','Energy from resetting, tariff 2 R+',2,16,'',24,'Энергия от сброса тариф 2 R+','','Енергія від скидання тариф 2 R+','');
INSERT INTO "tmplib_DevLib_io" VALUES('m230','T2Rm','Energy from resetting, tariff 2 R-',2,16,'',25,'Энергия от сброса тариф 2 R-','','Енергія від скидання тариф 2 R-','');
INSERT INTO "tmplib_DevLib_io" VALUES('m230','T3Ap','Energy from resetting, tariff 3 A+',2,16,'',26,'Энергия от сброса тариф 3 A+','','Енергія від скидання тариф 3 A+','');
INSERT INTO "tmplib_DevLib_io" VALUES('m230','T3Am','Energy from resetting, tariff 3 A-',2,16,'',27,'Энергия от сброса тариф 3 A-','','Енергія від скидання тариф 3 A-','');
INSERT INTO "tmplib_DevLib_io" VALUES('m230','T3Rp','Energy from resetting, tariff 3 R+',2,16,'',28,'Энергия от сброса тариф 3 R+','','Енергія від скидання тариф 3 R+','');
INSERT INTO "tmplib_DevLib_io" VALUES('m230','T3Rm','Energy from resetting, tariff 3 R-',2,16,'',29,'Энергия от сброса тариф 3 R-','','Енергія від скидання тариф 3 R-','');
INSERT INTO "tmplib_DevLib_io" VALUES('m230','T4Ap','Energy from resetting, tariff 4 A+',2,16,'',30,'Энергия от сброса тариф 4 A+','','Енергія від скидання тариф 4 A+','');
INSERT INTO "tmplib_DevLib_io" VALUES('m230','T4Am','Energy from resetting, tariff 4 A-',2,16,'',31,'Энергия от сброса тариф 4 A-','','Енергія від скидання тариф 4 A-','');
INSERT INTO "tmplib_DevLib_io" VALUES('m230','T4Rp','Energy from resetting, tariff 4 R+',2,16,'',32,'Энергия от сброса тариф 4 R+','','Енергія від скидання тариф 4 R+','');
INSERT INTO "tmplib_DevLib_io" VALUES('m230','T4Rm','Energy from resetting, tariff 4 R-',2,16,'',33,'Энергия от сброса тариф 4 R-','','Енергія від скидання тариф 4 R-','');
INSERT INTO "tmplib_DevLib_io" VALUES('m230','PS','Power P(W) by the phases sum',2,16,'',34,'Мощность P(Вт) по сумме фаз','','Потужніст P(Вт) по сумі фаз','');
INSERT INTO "tmplib_DevLib_io" VALUES('m230','P1','Power P(W) by the phase 1',2,16,'',35,'Мощность P(Вт) по фазе 1','','Потужніст P(Вт) по фазі 1','');
INSERT INTO "tmplib_DevLib_io" VALUES('m230','P2','Power P(W) by the phase 2',2,16,'',36,'Мощность P(Вт) по фазе 2','','Потужніст P(Вт) по фазі 2','');
INSERT INTO "tmplib_DevLib_io" VALUES('m230','P3','Power P(W) by the phase 3',2,16,'',37,'Мощность P(Вт) по фазе 3','','Потужніст P(Вт) по фазі 3','');
INSERT INTO "tmplib_DevLib_io" VALUES('m230','QS','Power Q(var) by the phases sum',2,16,'',38,'Мощность Q(вар) по сумме фаз','','Потужніст Q(вар) по сумі фаз','');
INSERT INTO "tmplib_DevLib_io" VALUES('m230','Q1','Power Q(var) by the phase 1',2,16,'',39,'Мощность Q(вар) по фазе 1','','Потужніст Q(вар) по фазі 1','');
INSERT INTO "tmplib_DevLib_io" VALUES('m230','Q2','Power Q(var) by the phase 2',2,16,'',40,'Мощность Q(вар) по фазе 2','','Потужніст Q(вар) по фазі 2','');
INSERT INTO "tmplib_DevLib_io" VALUES('m230','Q3','Power Q(var) by the phase 3',2,16,'',41,'Мощность Q(вар) по фазе 3','','Потужніст Q(вар) по фазі 3','');
INSERT INTO "tmplib_DevLib_io" VALUES('m230','SS','Power S(VA) by the phases sum',2,16,'',42,'Мощность S(ВА) по сумме фаз','','Потужніст S(ВА) по сумі фаз','');
INSERT INTO "tmplib_DevLib_io" VALUES('m230','S1','Power S(VA) by the phase 1',2,16,'',43,'Мощность S(ВА) по фазе 1','','Потужніст S(ВА) по фазі 1','');
INSERT INTO "tmplib_DevLib_io" VALUES('m230','S2','Power S(VA) by the phase 2',2,16,'',44,'Мощность S(ВА) по фазе 2','','Потужніст S(ВА) по фазі 2','');
INSERT INTO "tmplib_DevLib_io" VALUES('m230','S3','Power S(VA) by the phase 3',2,16,'',45,'Мощность S(ВА) по фазе 3','','Потужніст S(ВА) по фазі 3','');
INSERT INTO "tmplib_DevLib_io" VALUES('m230','U1','Voltage U(V) by the phase 1',2,16,'',46,'Напряжение U(В) по фазе 1','','Напруга U(В) по фазі 1','');
INSERT INTO "tmplib_DevLib_io" VALUES('m230','U2','Voltage U(V) by the phase 2',2,16,'',47,'Напряжение U(В) по фазе 2','','Напруга U(В) по фазі 2','');
INSERT INTO "tmplib_DevLib_io" VALUES('m230','U3','Voltage U(V) by the phase 3',2,16,'',48,'Напряжение U(В) по фазе 3','','Напруга U(В) по фазі 3','');
INSERT INTO "tmplib_DevLib_io" VALUES('m230','I1','Current I(A) by the phase 1',2,16,'',49,'Ток I(А) по фазе 1','','Струм I(А) по фазі 1','');
INSERT INTO "tmplib_DevLib_io" VALUES('m230','I2','Current I(A) by the phase 2',2,16,'',50,'Ток I(А) по фазе 2','','Струм I(А) по фазі 2','');
INSERT INTO "tmplib_DevLib_io" VALUES('m230','I3','Current I(A) by the phase 3',2,16,'',51,'Ток I(А) по фазе 3','','Струм I(А) по фазі 3','');
INSERT INTO "tmplib_DevLib_io" VALUES('m230','KS','Power coefficient by the phases sum',2,16,'',52,'Коэф.мощности по сумме фаз','','Коеф.потужності по сумі фаз','');
INSERT INTO "tmplib_DevLib_io" VALUES('m230','K1','Power coefficient by the phase 1',2,16,'',53,'Коэф.мощности по фазе 1','','Коеф.потужності по фазі 1','');
INSERT INTO "tmplib_DevLib_io" VALUES('m230','K2','Power coefficient by the phase 2',2,16,'',54,'Коэф.мощности по фазе 2','','Коеф.потужності по фазі 2','');
INSERT INTO "tmplib_DevLib_io" VALUES('m230','K3','Power coefficient by the phase 3',2,16,'',55,'Коэф.мощности по фазе 3','','Коеф.потужності по фазі 3','');
INSERT INTO "tmplib_DevLib_io" VALUES('m230','F1','Frequency F(Hz)',2,16,'',56,'Частота F(Гц)','','Частота F(Гц)','');
INSERT INTO "tmplib_DevLib_io" VALUES('m230','N1','Serial number',0,16,'',57,'Серийный номер','','Серійний номер','');
INSERT INTO "tmplib_DevLib_io" VALUES('m230','netaddr','Network address (0...240)',1,64,'1',58,'Сетевой адрес (0...240)','','Мережева адреса (0...240)','');
INSERT INTO "tmplib_DevLib_io" VALUES('m230','password','Password',0,64,'111111',59,'Пароль','','Пароль','');
INSERT INTO "tmplib_DevLib_io" VALUES('m230','transport','Transport',0,64,'merc230',60,'Транспорт','','Транспорт','');
CREATE TABLE 'tmplib_PrescrTempl_io' ("TMPL_ID" TEXT DEFAULT '' ,"ID" TEXT DEFAULT '' ,"NAME" TEXT DEFAULT '' ,"TYPE" INTEGER DEFAULT '' ,"FLAGS" INTEGER DEFAULT '' ,"VALUE" TEXT DEFAULT '' ,"POS" INTEGER DEFAULT '' ,"ru#NAME" TEXT DEFAULT '' ,"ru#VALUE" TEXT DEFAULT '' ,"uk#NAME" TEXT DEFAULT '' ,"uk#VALUE" TEXT DEFAULT '' , PRIMARY KEY ("TMPL_ID","ID"));
INSERT INTO "tmplib_PrescrTempl_io" VALUES('timer','run','Command: run',3,32,'0',4,'Команда: исполнение','','Команда: виконання','');
INSERT INTO "tmplib_PrescrTempl_io" VALUES('timer','pause','Command: pause',3,32,'0',5,'Команда: пауза','','Команда: пауза','');
INSERT INTO "tmplib_PrescrTempl_io" VALUES('timer','error','Prescription:error',3,32,'0',2,'Рецепт:ошибка','','Рецепт:помилка','');
INSERT INTO "tmplib_PrescrTempl_io" VALUES('timer','abort','Prescription:abort',3,32,'0',3,'Рецепт:прерывание','','Рецепт:перервано','');
INSERT INTO "tmplib_PrescrTempl_io" VALUES('timer','rez','Command: result',0,32,'1',6,'Команда: результат','','Команда: результат','');
INSERT INTO "tmplib_PrescrTempl_io" VALUES('timer','arg1','Time (s)',2,32,'',7,'Время (сек.)','','Час (сек.)','');
INSERT INTO "tmplib_PrescrTempl_io" VALUES('timer','tmp1','Temporary 1',2,1,'0',8,'Временный 1','','Тимчасово 1','');
INSERT INTO "tmplib_PrescrTempl_io" VALUES('backTimer','run','Command: run',3,32,'0',4,'Команда: исполнение','','Команда: виконання','');
INSERT INTO "tmplib_PrescrTempl_io" VALUES('backTimer','pause','Command: pause',3,32,'0',5,'Команда: пауза','','Команда: пауза','');
INSERT INTO "tmplib_PrescrTempl_io" VALUES('backTimer','error','Prescription:error',3,32,'0',2,'Рецепт:ошибка','','Рецепт:помилка','');
INSERT INTO "tmplib_PrescrTempl_io" VALUES('backTimer','abort','Prescription:abort',3,32,'0',3,'Рецепт:прерывание','','Рецепт:перервано','');
INSERT INTO "tmplib_PrescrTempl_io" VALUES('backTimer','rez','Command: result',0,32,'1',6,'Команда: результат','','Команда: результат','');
INSERT INTO "tmplib_PrescrTempl_io" VALUES('backTimer','arg1','Time (s)',2,32,'',7,'Время (сек.)','','Час (сек.)','');
INSERT INTO "tmplib_PrescrTempl_io" VALUES('backTimer','tmp1','Temporary 1',2,1,'0',8,'Временный 1','','Тимчасово 1','');
INSERT INTO "tmplib_PrescrTempl_io" VALUES('backTimer','start','Prescription:start',3,32,'0',0,'Рецепт:старт','','Рецепт:старт','');
INSERT INTO "tmplib_PrescrTempl_io" VALUES('backTimer','stop','Prescription:stop',3,32,'0',1,'Рецепт:стоп','','Рецепт:стоп','');
INSERT INTO "tmplib_PrescrTempl_io" VALUES('timer','start','Prescription:start',3,32,'0',0,'Рецепт:старт','','Рецепт:старт','');
INSERT INTO "tmplib_PrescrTempl_io" VALUES('timer','stop','Prescription:stop',3,32,'0',1,'Рецепт:стоп','','Рецепт:стоп','');
CREATE TABLE 'tmplib_S7' ("ID" TEXT DEFAULT '' ,"NAME" TEXT DEFAULT '' ,"DESCR" TEXT DEFAULT '' ,"PROGRAM" TEXT DEFAULT '' ,"uk#NAME" TEXT DEFAULT '' ,"uk#DESCR" TEXT DEFAULT '' ,"uk#PROGRAM" TEXT DEFAULT '' ,"ru#NAME" TEXT DEFAULT '' ,"ru#DESCR" TEXT DEFAULT '' ,"ru#PROGRAM" TEXT DEFAULT '' , PRIMARY KEY ("ID"));
INSERT INTO "tmplib_S7" VALUES('ai_simple','Simple AI','Simple analog parameter.','JavaLikeCalc.JavaScript
val=val_cod;
if(val!=EVAL_REAL)
{
  val=bDwn+(bUp-bDwn)*(val/27648.0);
  if(bUp!=bDwn && val>bUp)		f_err="101:Upper work border violation";
  else if(bUp!=bDwn && val<bDwn)	f_err="102:Lower work border violation";
  else if(bErrUp!=bErrDwn && val>bErrUp)	f_err="103:Upper alarm border violation";
  else if(bErrUp!=bErrDwn && val<bErrDwn)	f_err="104:Lower alarm border violation";
  else if(bWUp!=bWDwn && val>bWUp)	f_err="105:Upper warning border violation";
  else if(bWUp!=bWDwn && val<bWDwn)	f_err="106:Lower warning border violation";
  else f_err="0";
}','Простий AI','Простий аналоговий параметр.','JavaLikeCalc.JavaScript
val=val_cod;
if(val!=EVAL_REAL)
{
  val=bDwn+(bUp-bDwn)*(val/27648.0);
  if(bUp!=bDwn && val>bUp)		f_err="101:Порушення верхньої робочої границі";
  else if(bUp!=bDwn && val<bDwn)	f_err="102:Порушення нижньої робочої границі";
  else if(bErrUp!=bErrDwn && val>bErrUp)	f_err="103:Порушення верхньої аварійної границі";
  else if(bErrUp!=bErrDwn && val<bErrDwn)	f_err="104:Порушення нижньої аварійної границі";
  else if(bWUp!=bWDwn && val>bWUp)	f_err="105:Порушення верхньої попереджувальної границі";
  else if(bWUp!=bWDwn && val<bWDwn)	f_err="106:Порушення нижньої попереджувальної границі";
  else f_err="0";
}','Простой AI','Простой аналоговый параметр.','JavaLikeCalc.JavaScript
val=val_cod;
if(val!=EVAL_REAL)
{
  val=bDwn+(bUp-bDwn)*(val/27648.0);
  if(bUp!=bDwn && val>bUp)		f_err="101:Нарушение верхней рабочей границы";
  else if(bUp!=bDwn && val<bDwn)	f_err="102:Нарушение нижней рабочей границы";
  else if(bErrUp!=bErrDwn && val>bErrUp)	f_err="103:Нарушение верхней аварийной границы";
  else if(bErrUp!=bErrDwn && val<bErrDwn)	f_err="104:Нарушение нижней аварийной границы";
  else if(bWUp!=bWDwn && val>bWUp)	f_err="105:Нарушение верхней предупредительной границы";
  else if(bWUp!=bWDwn && val<bWDwn)	f_err="106:Нарушение нижней предупредительной границы";
  else f_err="0";
}');
INSERT INTO "tmplib_S7" VALUES('ai_man','Manual input of AI','Manual input of a standard analog parameter.','JavaLikeCalc.JavaScript
//Check and write new value
if(val!=val_o)
{
  val_cod=(27648.0*(val-bDwn))/(bUp-bDwn);
  val_o=val;
}
else
{
  val=val_cod;
}

if(val!=EVAL_REAL)
{
  val=val_o=bDwn+(bUp-bDwn)*(val/27648.0);
  if(bUp!=bDwn && val>bUp)		f_err="101:Upper work border violation";
  else if(bUp!=bDwn && val<bDwn)	f_err="102:Lower work border violation";
  else if(bErrUp!=bErrDwn && val>bErrUp)	f_err="103:Upper alarm border violation";
  else if(bErrUp!=bErrDwn && val<bErrDwn)	f_err="104:Lower alarm border violation";
  else if(bWUp!=bWDwn && val>bWUp)	f_err="105:Upper warning border violation";
  else if(bWUp!=bWDwn && val<bWDwn)	f_err="106:Lower warning border violation";
  else f_err="0";
}','Ручний ввід AI','Стандартний аналоговий параметр ручного вводу.','JavaLikeCalc.JavaScript
//Check and write new value
if(val!=val_o)
{
  val_cod=(27648.0*(val-bDwn))/(bUp-bDwn);
  val_o=val;
}
else
{
  val=val_cod;
}

if(val!=EVAL_REAL)
{
  val=val_o=bDwn+(bUp-bDwn)*(val/27648.0);
  if(bUp!=bDwn && val>bUp)		f_err="101:Порушення верхньої робочої границі";
  else if(bUp!=bDwn && val<bDwn)	f_err="102:Порушення нижньої робочої границі";
  else if(bErrUp!=bErrDwn && val>bErrUp)	f_err="103:Порушення верхньої аварійної границі";
  else if(bErrUp!=bErrDwn && val<bErrDwn)	f_err="104:Порушення нижньої аварійної границі";
  else if(bWUp!=bWDwn && val>bWUp)	f_err="105:Порушення верхньої попереджувальної границі";
  else if(bWUp!=bWDwn && val<bWDwn)	f_err="106:Порушення нижньої попереджувальної границі";
  else f_err="0";
}','Ручной ввод AI','Стандартный аналоговый параметр ручного ввода.','JavaLikeCalc.JavaScript
//Check and write new value
if(val!=val_o)
{
  val_cod=(27648.0*(val-bDwn))/(bUp-bDwn);
  val_o=val;
}
else
{
  val=val_cod;
}

if(val!=EVAL_REAL)
{
  val=val_o=bDwn+(bUp-bDwn)*(val/27648.0);
  if(bUp!=bDwn && val>bUp)		f_err="101:Нарушение верхней рабочей границы";
  else if(bUp!=bDwn && val<bDwn)	f_err="102:Нарушение нижней рабочей границы";
  else if(bErrUp!=bErrDwn && val>bErrUp)	f_err="103:Нарушение верхней аварийной границы";
  else if(bErrUp!=bErrDwn && val<bErrDwn)f_err="104:Нарушение нижней аварийной границы";
  else if(bWUp!=bWDwn && val>bWUp)	f_err="105:Нарушение верхней предупредительной границы";
  else if(bWUp!=bWDwn && val<bWDwn)	f_err="106:Нарушение нижней предупредительной границы";
  else f_err="0";
}');
INSERT INTO "tmplib_S7" VALUES('test','Test','S7 controller testing','
','Тест','Тестування контролера S7','
','Тест','Тестирование контроллера S7','
');
CREATE TABLE 'tmplib_S7_io' ("TMPL_ID" TEXT DEFAULT '' ,"ID" TEXT DEFAULT '' ,"NAME" TEXT DEFAULT '' ,"TYPE" INTEGER DEFAULT '' ,"FLAGS" INTEGER DEFAULT '' ,"VALUE" TEXT DEFAULT '' ,"POS" INTEGER DEFAULT '' ,"uk#NAME" TEXT DEFAULT '' ,"uk#VALUE" TEXT DEFAULT '' ,"ru#NAME" TEXT DEFAULT '' ,"ru#VALUE" TEXT DEFAULT '' , PRIMARY KEY ("TMPL_ID","ID"));
INSERT INTO "tmplib_S7_io" VALUES('ai_simple','val_cod','Value''s source code',1,128,'',0,'Вихідний код значення',' ','Исходный код значения','');
INSERT INTO "tmplib_S7_io" VALUES('ai_simple','val','Value',2,16,'0',1,'Значення','','Значение','');
INSERT INTO "tmplib_S7_io" VALUES('ai_simple','val_tp','Dimension',0,32,'',2,'Од. виміру',' ','Ед. измерения','');
INSERT INTO "tmplib_S7_io" VALUES('ai_simple','bDwn','Lower value border',2,32,'0',3,'Нижня границя значення','','Нижняя граница значения','');
INSERT INTO "tmplib_S7_io" VALUES('ai_simple','bUp','Upper value border',2,32,'100',4,'Верхня границя значення','','Верхняя граница значения','');
INSERT INTO "tmplib_S7_io" VALUES('ai_simple','bWDwn','Lower technical border',2,32,'20',5,'Нижня технолог. границя','','Нижняя технологич. граница','');
INSERT INTO "tmplib_S7_io" VALUES('ai_simple','bWUp','Upper technical border',2,32,'80',6,'Верхня технолог. границя','','Верхняя технологич. граница','');
INSERT INTO "tmplib_S7_io" VALUES('ai_simple','bErrDwn','Lower alarm border',2,32,'10',7,'Нижня аварійна границя','','Нижняя аварийная граница','');
INSERT INTO "tmplib_S7_io" VALUES('ai_simple','bErrUp','Upper alarm border',2,32,'90',8,'Верхня аварійна границя','','Верхняя аварийная граница','');
INSERT INTO "tmplib_S7_io" VALUES('ai_man','val_cod','Value''s source code',1,129,'',0,'Вихідний код значення',' ','Исходный код значения','');
INSERT INTO "tmplib_S7_io" VALUES('ai_man','val','Value',2,32,'0',1,'Значення','','Значение','');
INSERT INTO "tmplib_S7_io" VALUES('ai_man','val_o','Previous value',2,0,'0',2,'Попередне значення','','Предыдущее значение','');
INSERT INTO "tmplib_S7_io" VALUES('ai_man','val_tp','Dimension',0,32,'',3,'Од. виміру',' ','Ед. измерения','');
INSERT INTO "tmplib_S7_io" VALUES('ai_man','bDwn','Lower value border',2,32,'0',4,'Нижня границя значення','','Нижняя граница значения','');
INSERT INTO "tmplib_S7_io" VALUES('ai_man','bUp','Upper value border',2,32,'100',5,'Верхня границя значення','','Верхняя граница значения','');
INSERT INTO "tmplib_S7_io" VALUES('ai_man','bWDwn','Lower technical border',2,32,'20',6,'Нижня технологічна границя','','Нижняя технологическая граница','');
INSERT INTO "tmplib_S7_io" VALUES('ai_man','bWUp','Upper technical border',2,32,'80',7,'Верхня технологічна границя','','Верхняя технологич. граница','');
INSERT INTO "tmplib_S7_io" VALUES('ai_man','bErrDwn','Lower alarm border',2,32,'10',8,'Нижня аварійна границя','','Нижняя аварийная граница','');
INSERT INTO "tmplib_S7_io" VALUES('ai_man','bErrUp','Upper alarm border',2,32,'90',9,'Верхня аварійна границя','','Верхняя аварийная граница','');
INSERT INTO "tmplib_S7_io" VALUES('test','in50','Input50',1,160,'DB Test|108',54,'Вхід50','DB Тест|108','Вход50','DB Тест|108');
INSERT INTO "tmplib_S7_io" VALUES('test','in49','Input49',1,160,'DB Test|106',53,'Вхід49','DB Тест|106','Вход49','DB Тест|106');
INSERT INTO "tmplib_S7_io" VALUES('test','in48','Input48',1,160,'DB Test|104',52,'Вхід48','DB Тест|104','Вход48','DB Тест|104');
INSERT INTO "tmplib_S7_io" VALUES('test','in47','Input47',1,160,'DB Test|102',51,'Вхід47','DB Тест|102','Вход47','DB Тест|102');
INSERT INTO "tmplib_S7_io" VALUES('test','in46','Input46',1,160,'DB Test|100',50,'Вхід46','DB Тест|100','Вход46','DB Тест|100');
INSERT INTO "tmplib_S7_io" VALUES('test','in45','Input45',1,160,'DB Test|98',49,'Вхід45','DB Тест|98','Вход45','DB Тест|98');
INSERT INTO "tmplib_S7_io" VALUES('test','in44','Input44',1,160,'DB Test|96',48,'Вхід44','DB Тест|96','Вход44','DB Тест|96');
INSERT INTO "tmplib_S7_io" VALUES('test','in43','Input43',1,160,'DB Test|94',47,'Вхід43','DB Тест|94','Вход43','DB Тест|94');
INSERT INTO "tmplib_S7_io" VALUES('test','in42','Input42',1,160,'DB Test|92',46,'Вхід42','DB Тест|92','Вход42','DB Тест|92');
INSERT INTO "tmplib_S7_io" VALUES('test','in41','Input41',1,160,'DB Test|90',45,'Вхід41','DB Тест|90','Вход41','DB Тест|90');
INSERT INTO "tmplib_S7_io" VALUES('test','in40','Input40',1,160,'DB Test|86',44,'Вхід40','DB Тест|86','Вход40','DB Тест|86');
INSERT INTO "tmplib_S7_io" VALUES('test','in39','Input39',1,160,'DB Test|84',43,'Вхід39','DB Тест|84','Вход39','DB Тест|84');
INSERT INTO "tmplib_S7_io" VALUES('test','in38','Input38',1,160,'DB Test|82',42,'Вхід38','DB Тест|82','Вход38','DB Тест|82');
INSERT INTO "tmplib_S7_io" VALUES('test','in37','Input37',1,160,'DB Test|80',41,'Вхід37','DB Тест|80','Вход37','DB Тест|80');
INSERT INTO "tmplib_S7_io" VALUES('test','in36','Input36',1,160,'DB Test|78',40,'Вхід36','DB Тест|78','Вход36','DB Тест|78');
INSERT INTO "tmplib_S7_io" VALUES('test','in35','Input35',1,160,'DB Test|76',39,'Вхід35','DB Тест|76','Вход35','DB Тест|76');
INSERT INTO "tmplib_S7_io" VALUES('test','in34','Input34',1,160,'DB Test|74',38,'Вхід34','DB Тест|74','Вход34','DB Тест|74');
INSERT INTO "tmplib_S7_io" VALUES('test','in33','Input33',1,160,'DB Test|72',37,'Вхід33','DB Тест|72','Вход33','DB Тест|72');
INSERT INTO "tmplib_S7_io" VALUES('test','in32','Input32',1,160,'DB Test|70',36,'Вхід32','DB Тест|70','Вход32','DB Тест|70');
INSERT INTO "tmplib_S7_io" VALUES('test','in31','Input31',1,160,'DB Test|68',35,'Вхід31','DB Тест|68','Вход31','DB Тест|68');
INSERT INTO "tmplib_S7_io" VALUES('test','in30','Input30',1,160,'DB Test|64',34,'Вхід30','DB Тест|64','Вход30','DB Тест|64');
INSERT INTO "tmplib_S7_io" VALUES('test','in29','Input29',1,160,'DB Test|62',33,'Вхід29','DB Тест|62','Вход29','DB Тест|62');
INSERT INTO "tmplib_S7_io" VALUES('test','in28','Input28',1,160,'DB Test|60',32,'Вхід28','DB Тест|60','Вход28','DB Тест|60');
INSERT INTO "tmplib_S7_io" VALUES('test','in27','Input27',1,160,'DB Test|58',31,'Вхід27','DB Тест|58','Вход27','DB Тест|58');
INSERT INTO "tmplib_S7_io" VALUES('test','in26','Input26',1,160,'DB Test|56',30,'Вхід26','DB Тест|56','Вход26','DB Тест|56');
INSERT INTO "tmplib_S7_io" VALUES('test','in25','Input25',1,160,'DB Test|54',29,'Вхід25','DB Тест|54','Вход25','DB Тест|54');
INSERT INTO "tmplib_S7_io" VALUES('test','in24','Input24',1,160,'DB Test|52',28,'Вхід24','DB Тест|52','Вход24','DB Тест|52');
INSERT INTO "tmplib_S7_io" VALUES('test','in23','Input23',1,160,'DB Test|50',27,'Вхід23','DB Тест|50','Вход23','DB Тест|50');
INSERT INTO "tmplib_S7_io" VALUES('test','in22','Input22',1,160,'DB Test|48',26,'Вхід22','DB Тест|48','Вход22','DB Тест|48');
INSERT INTO "tmplib_S7_io" VALUES('test','in21','Input21',1,160,'DB Test|46',25,'Вхід21','DB Тест|46','Вход21','DB Тест|46');
INSERT INTO "tmplib_S7_io" VALUES('test','in20','Input20',1,160,'DB Test|44',24,'Вхід20','DB Тест|44','Вход20','DB Тест|44');
INSERT INTO "tmplib_S7_io" VALUES('test','in19','Input19',1,160,'DB Test|42',23,'Вхід19','DB Тест|42','Вход19','DB Тест|42');
INSERT INTO "tmplib_S7_io" VALUES('test','in18','Input18',1,160,'DB Test|40',22,'Вхід18','DB Тест|40','Вход18','DB Тест|40');
INSERT INTO "tmplib_S7_io" VALUES('test','in17','Input17',1,160,'DB Test|38',21,'Вхід17','DB Тест|38','Вход17','DB Тест|38');
INSERT INTO "tmplib_S7_io" VALUES('test','in16','Input16',1,160,'DB Test|36',20,'Вхід16','DB Тест|36','Вход16','DB Тест|36');
INSERT INTO "tmplib_S7_io" VALUES('test','in15','Input15',1,160,'DB Test|34',19,'Вхід15','DB Тест|34','Вход15','DB Тест|34');
INSERT INTO "tmplib_S7_io" VALUES('test','in14','Input14',1,160,'DB Test|32',18,'Вхід14','DB Тест|32','Вход14','DB Тест|32');
INSERT INTO "tmplib_S7_io" VALUES('test','in13','Input13',1,160,'DB Test|30',17,'Вхід13','DB Тест|30','Вход13','DB Тест|30');
INSERT INTO "tmplib_S7_io" VALUES('test','in12','Input12',1,160,'DB Test|28',16,'Вхід12','DB Тест|28','Вход12','DB Тест|28');
INSERT INTO "tmplib_S7_io" VALUES('test','in11','Input11',1,160,'DB Test|26',15,'Вхід11','DB Тест|26','Вход11','DB Тест|26');
INSERT INTO "tmplib_S7_io" VALUES('test','in10','Input10',1,160,'DB Test|24',14,'Вхід10','DB Тест|24','Вход10','DB Тест|24');
INSERT INTO "tmplib_S7_io" VALUES('test','in9','Input9',1,160,'DB Test|22',13,'Вхід9','DB Тест|22','Вход9','DB Тест|22');
INSERT INTO "tmplib_S7_io" VALUES('test','in8','Input8',1,160,'DB Test|20',12,'Вхід8','DB Тест|20','Вход8','DB Тест|20');
INSERT INTO "tmplib_S7_io" VALUES('test','in7','Input7',1,160,'DB Test|18',11,'Вхід7','DB Тест|18','Вход7','DB Тест|18');
INSERT INTO "tmplib_S7_io" VALUES('test','in6','Input6',1,160,'DB Test|16',10,'Вхід6','DB Тест|16','Вход6','DB Тест|16');
INSERT INTO "tmplib_S7_io" VALUES('test','in5','Input5',1,160,'DB Test|14',9,'Вхід5','DB Тест|14','Вход5','DB Тест|14');
INSERT INTO "tmplib_S7_io" VALUES('test','in4','Input4',1,160,'DB Test|12',8,'Вхід4','DB Тест|12','Вход4','DB Тест|12');
INSERT INTO "tmplib_S7_io" VALUES('test','in3','Input3',1,160,'DB Test|10',7,'Вхід3','DB Тест|10','Вход3','DB Тест|10');
INSERT INTO "tmplib_S7_io" VALUES('test','in2','Input2',1,160,'DB Test|8',6,'Вхід2','DB Тест|8','Вход2','DB Тест|8');
INSERT INTO "tmplib_S7_io" VALUES('test','in1','Input1',1,160,'DB Test|6',5,'Вхід1','DB Тест|6','Вход1','DB Тест|6');
INSERT INTO "tmplib_S7_io" VALUES('test','in','Input',1,160,'DB Test|4',4,'Вхід','DB Тест|4','Вход','DB Тест|4');
CREATE TABLE 'tmplib_base_io' ("TMPL_ID" TEXT DEFAULT '' ,"ID" TEXT DEFAULT '' ,"NAME" TEXT DEFAULT '' ,"TYPE" INTEGER DEFAULT '' ,"FLAGS" INTEGER DEFAULT '' ,"VALUE" TEXT DEFAULT '' ,"POS" INTEGER DEFAULT '' ,"uk#NAME" TEXT DEFAULT '' ,"uk#VALUE" TEXT DEFAULT '' ,"ru#NAME" TEXT DEFAULT '' ,"ru#VALUE" TEXT DEFAULT '' , PRIMARY KEY ("TMPL_ID","ID"));
INSERT INTO "tmplib_base_io" VALUES('digAlarm','in','Input',3,144,'Input|in',2,'Вхід',' ','Вход','');
INSERT INTO "tmplib_base_io" VALUES('simleBoard','in','Input',2,128,'Parameter|var',0,'Вхід','','Вход','');
INSERT INTO "tmplib_base_io" VALUES('simleBoard','var','Variable',2,32,'0',1,'Змінна','','Переменная','');
INSERT INTO "tmplib_base_io" VALUES('simleBoard','ed','Dimension',0,32,' ',2,'Од. виміру',' ','Ед. измерения','');
INSERT INTO "tmplib_base_io" VALUES('simleBoard','min','Lower work border',2,32,'0',3,'Нижня робоча границя','','Нижн. раб. граница','');
INSERT INTO "tmplib_base_io" VALUES('simleBoard','max','Upper work border',2,32,'100',4,'Верхня робоча границя','','Верхн. раб. граница','');
INSERT INTO "tmplib_base_io" VALUES('simleBoard','aMin','Lower alarm border',2,32,'0',5,'Нижня аварійна границя','','Нижняя аварийная граница','');
INSERT INTO "tmplib_base_io" VALUES('simleBoard','aMax','Upper alarm border',2,32,'0',6,'Верхня аварійна границя','','Верхняя аварийная граница','');
INSERT INTO "tmplib_base_io" VALUES('simleBoard','wMin','Lower warning border',2,32,'0',7,'Нижня попереджувальна границя','','Нижн. предупредительная граница','');
INSERT INTO "tmplib_base_io" VALUES('simleBoard','wMax','Upper warning border',2,32,'0',8,'Верхня попереджувальна границя','','Верхн. предупредительная граница','');
INSERT INTO "tmplib_base_io" VALUES('simleBoard','iAdd','Addon to input',2,64,'0',9,'Додаток до входу','','Добавка ко входу','');
INSERT INTO "tmplib_base_io" VALUES('simleBoard','iMult','Input multiplication',2,64,'1',10,'Множник до входу','','Множитель входа','');
INSERT INTO "tmplib_base_io" VALUES('digitBlock','cmdOpen','Command "Open"',3,161,'Crane|com',0,'Команда "Відкрити"','Кран|com','Команда "Открыть"','Кран|com');
INSERT INTO "tmplib_base_io" VALUES('digitBlock','cmdClose','Command "Close"',3,161,'Crane|close',1,'Команда "Закрити"','Кран|close','Команда "Закрыть"','Кран|close');
INSERT INTO "tmplib_base_io" VALUES('digitBlock','cmdStop','Command "Stop"',3,161,'Crane|stop',2,'Команда "Стоп"','Кран|stop','Команда "Стоп"','Кран|stop');
INSERT INTO "tmplib_base_io" VALUES('digitBlock','stOpen','State "Opened"',3,144,'Crane|st_open',3,'Стан "Відкрито"','Кран|st_open','Сост. "Открыто"','Кран|st_open');
INSERT INTO "tmplib_base_io" VALUES('digitBlock','stClose','State "Closed"',3,144,'Crane|st_close',4,'Стан "Закрито"','Кран|st_close','Сост. "Закрыто"','Кран|st_close');
INSERT INTO "tmplib_base_io" VALUES('digitBlock','tCmd','Hold command time (s)',1,32,'5',5,'Час утримання команди (с)','','Время удерж. команды (с)','');
INSERT INTO "tmplib_base_io" VALUES('digitBlock','last_cmd','Last command',1,1,'0',6,'Остання команда','','Последняя команда','');
INSERT INTO "tmplib_base_io" VALUES('digitBlock','w_tm','Process command counter',2,1,'0',7,'Лічильник опрацювання команди','','Счётчик отраб. команды','');
INSERT INTO "tmplib_base_io" VALUES('gasPoint','DS','Density (kg/m3)',2,16,'0',5,'Щильність (кг/м3)','','Плотность (кг/м3)','');
INSERT INTO "tmplib_base_io" VALUES('gasPoint','dP','Differential pressure (kgH/m2)',2,16,'0',4,'Перепад (кГс/м2)','','Перепад (кГс/м2)','');
INSERT INTO "tmplib_base_io" VALUES('gasPoint','T','Temperature (°C)',2,144,'T|var',3,'Температура (°С)','','Температура (°С)','');
INSERT INTO "tmplib_base_io" VALUES('gasPoint','P','Pressure (kgH/sm2)',2,144,'P|var',2,'Тиск (кГс/см2)','','Давление (кГс/см2)','');
INSERT INTO "tmplib_base_io" VALUES('gasPoint','Q','Volume (m3)',2,16,'0',1,'Об''єм (м3)','','Объём (м3)','');
INSERT INTO "tmplib_base_io" VALUES('gasPoint','F','Flow (m3/h)',2,144,'F|var',0,'Витрати (м3/ч)','','Расход (м3/ч)','');
INSERT INTO "tmplib_base_io" VALUES('manInUnif','inout','Input/Output',2,129,'Parameter|var',0,'Вхід/Вихід','','Вход/Выход','');
INSERT INTO "tmplib_base_io" VALUES('manInUnif','var','Variable',2,16,'0',1,'Змінна','','Переменная','');
INSERT INTO "tmplib_base_io" VALUES('manInUnif','ed','Dimension',0,32,' ',3,'Од. виміру',' ','Ед. измерения',' ');
INSERT INTO "tmplib_base_io" VALUES('manInUnif','min','Scale:minimum',2,32,'0',4,'Шкала:мінімум','','Шкала:минимум','');
INSERT INTO "tmplib_base_io" VALUES('manInUnif','max','Scale:maximum',2,32,'100',5,'Шкала:максимум','','Шкала:максимум','');
INSERT INTO "tmplib_base_io" VALUES('manInUnif','scSqr','Scale:square',3,0,'0',6,'Шкала:квадратична','','Шкала:квадратичная','');
INSERT INTO "tmplib_base_io" VALUES('manInUnif','aMin','Border down alarm',2,32,'0',8,'Границя нижня ав.','','Граница нижняя ав.','');
INSERT INTO "tmplib_base_io" VALUES('manInUnif','aMax','Border up alarm',2,32,'0',9,'Границя верхня аварійна','','Граница верхняя ав.','');
INSERT INTO "tmplib_base_io" VALUES('manInUnif','wMin','Border down warning',2,32,'0',10,'Границя нижня попереджув.','','Граница нижняя пред.','');
INSERT INTO "tmplib_base_io" VALUES('manInUnif','wMax','Border up warning',2,32,'0',11,'Границя верхня попереджув.','','Граница верхняя пред.','');
INSERT INTO "tmplib_base_io" VALUES('manInUnif','speed','Motion speed (%/cycle)',2,32,'0',13,'Швидкість зміни (%/цикл)','','Скорость изменения (%/цикл)','');
INSERT INTO "tmplib_base_io" VALUES('manInUnif','Tf','Filter time (s)',2,32,'0',16,'Час фільтру (с)','','Время фильтрации (с)','');
INSERT INTO "tmplib_base_io" VALUES('manInUnif','iAdd','Input addon',2,64,'0',17,'Додаток до входу','','Дополнение ко входу','');
INSERT INTO "tmplib_base_io" VALUES('manInUnif','iMult','Input multiplication',2,64,'1',18,'Множник входу','','Множитель ко входу','');
INSERT INTO "tmplib_base_io" VALUES('manInUnif','plcMin','Minimum module scale',2,64,'0',19,'Мінімум шкали модуля','','Минимум шкалы модуля','');
INSERT INTO "tmplib_base_io" VALUES('manInUnif','plcMax','Maximum module scale',2,64,'0',20,'Максимум шкали модуля','','Максимум шкалы модуля','');
INSERT INTO "tmplib_base_io" VALUES('manInUnif','plcExcess','Allowed scale exceeding (%)',2,64,'0',21,'Дозволене перевищення шкали (%)','','Допустимое превышение шкалы модуля (%)','');
INSERT INTO "tmplib_base_io" VALUES('anUnif','in','Input',2,128,'Input|var',0,'Вхід','Вхід|var','Вход','Вход|var');
INSERT INTO "tmplib_base_io" VALUES('anUnif','var','Variable',2,16,'0',2,'Змінна','','Переменная','');
INSERT INTO "tmplib_base_io" VALUES('anUnif','ed','Dimension',0,32,' ',3,'Од. виміру',' ','Ед. измерения',' ');
INSERT INTO "tmplib_base_io" VALUES('anUnif','min','Scale:minimum',2,32,'0',4,'Шкала:мінімум','','Шкала:минимум','');
INSERT INTO "tmplib_base_io" VALUES('anUnif','max','Scale:maximum',2,32,'100',5,'Шкала:максимум','','Шкала:максимум','');
INSERT INTO "tmplib_base_io" VALUES('anUnif','scSqr','Scale:square',3,32,'0',6,'Шкала:квадратична','','Шкала:квадратичная','');
INSERT INTO "tmplib_base_io" VALUES('anUnif','aMin','Border down alarm',2,32,'0',11,'Границя нижня ав.','','Граница нижняя ав.','');
INSERT INTO "tmplib_base_io" VALUES('anUnif','aMax','Border up alarm',2,32,'0',12,'Границя верхня аварійна','','Граница верхняя ав.','');
INSERT INTO "tmplib_base_io" VALUES('anUnif','wMin','Border down warning',2,32,'0',13,'Границя нижня попереджув.','','Граница нижняя пред.','');
INSERT INTO "tmplib_base_io" VALUES('anUnif','wMax','Border up warning',2,32,'0',14,'Границя верхня попереджув.','','Граница верхняя пред.','');
INSERT INTO "tmplib_base_io" VALUES('anUnif','speed','Motion speed (%/cycle)',2,32,'0',16,'Швидкість зміни (%/цикл)','','Скорость изменения (%/цикл)','');
INSERT INTO "tmplib_base_io" VALUES('anUnif','prec','Precision (signs)',1,32,'2',17,'Точність (знаків)','','Точность (знаков)','');
INSERT INTO "tmplib_base_io" VALUES('anUnif','Tf','Filter time, s',2,32,'0',19,'Час фільтру (с)','','Время фильтрации (с)','');
INSERT INTO "tmplib_base_io" VALUES('anUnif','iAdd','Input addon',2,64,'0',20,'Додаток до входу','','Дополнение ко входу','');
INSERT INTO "tmplib_base_io" VALUES('anUnif','iMult','Input multiplication',2,64,'1',21,'Множник входу','','Множитель ко входу','');
INSERT INTO "tmplib_base_io" VALUES('anUnif','plcMin','Minimum module scale',2,64,'0',22,'Мінімум шкали модуля','','Минимум шкалы модуля','');
INSERT INTO "tmplib_base_io" VALUES('anUnif','plcMax','Maximum module scale',2,64,'0',23,'Максимум шкали модуля','','Максимум шкалы модуля','');
INSERT INTO "tmplib_base_io" VALUES('anUnif','plcExcess','Allowed scale exceeding, %',2,64,'0',24,'Дозволене перевищення шкали (%)','','Допустимое превышение шкалы модуля (%)','');
INSERT INTO "tmplib_base_io" VALUES('anUnif','plcImit','PLC imitation',3,64,'0',25,'Імітація ПЛК','','Имитация ПЛК','');
INSERT INTO "tmplib_base_io" VALUES('anUnif','plcImitIn','PLC imitation input',2,128,'Imitation|var',26,'Вхід імітації ПЛК','Імітація|var','Вход имитации ПЛК','Имитация|var');
INSERT INTO "tmplib_base_io" VALUES('digitBlockUnif','com','Command "Open"',3,161,'Crane|com',0,'Команда "Відкрити"','Кран|com','Команда "Открыть"','Кран|com');
INSERT INTO "tmplib_base_io" VALUES('digitBlockUnif','close','Command "Close"',3,161,'Crane|close',1,'Команда "Закрити"','Кран|close','Команда "Закрыть"','Кран|close');
INSERT INTO "tmplib_base_io" VALUES('digitBlockUnif','stop','Command "Stop"',3,161,'Crane|stop',2,'Команда "Зупинити"','Кран|stop','Команда "Стоп"','Кран|stop');
INSERT INTO "tmplib_base_io" VALUES('digitBlockUnif','st_open','State "Opened"',3,144,'Crane|st_open',3,'Стан "Відкрито"','Кран|st_open','Сост. "Открыто"','Кран|st_open');
INSERT INTO "tmplib_base_io" VALUES('digitBlockUnif','st_close','State "Closed"',3,144,'Crane|st_close',4,'Стан "Закрито"','Кран|st_close','Сост. "Закрыто"','Кран|st_close');
INSERT INTO "tmplib_base_io" VALUES('digitBlockUnif','tCmd','Hold command time (s)',1,64,'5',5,'Час утримання команди (с)','','Время удерж. команды (с)','');
INSERT INTO "tmplib_base_io" VALUES('digitBlockUnif','last_cmd','Last command',1,1,'0',6,'Остання команда','','Последняя команда','');
INSERT INTO "tmplib_base_io" VALUES('digitBlockUnif','w_tm','Process command counter',2,1,'0',7,'Лічильнк обробки команди','','Счётчик отраб. команды','');
INSERT INTO "tmplib_base_io" VALUES('pidUnifImp','in','Input',2,128,'Signal|in',0,'Вхід','','Вход','Signal|in');
INSERT INTO "tmplib_base_io" VALUES('pidUnifImp','sp','Setpoint',2,160,'PID|sp',1,'Завдання','ПІД|sp','Задание','ПИД|sp');
INSERT INTO "tmplib_base_io" VALUES('pidUnifImp','var','Variable',2,16,'0',2,'Змінна','','Переменная','');
INSERT INTO "tmplib_base_io" VALUES('pidUnifImp','manIn','Manual input (%)',2,160,'PID|manIn',3,'Ручний ввід (%)','ПІД|manIn','Ручной ввод (%)','ПИД|manIn');
INSERT INTO "tmplib_base_io" VALUES('pidUnifImp','out','Output (%)',2,144,'PID|out',4,'Вихід (%)','ПІД|out','Выход (%)','ПИД|out');
INSERT INTO "tmplib_base_io" VALUES('pidUnifImp','impQup','Impulse output up',3,144,'PID|impQupTm',5,'Імп. вихід у гору','ПІД|impQupTm','Имп. выход вверх','ПИД|impQupTm');
INSERT INTO "tmplib_base_io" VALUES('pidUnifImp','impQdwn','Impulse output down',3,144,'PID|impQdwnTm',6,'Імп. вихід до низу','ПІД|impQdwnTm','Имп. выход вниз','ПИД|impQdwnTm');
INSERT INTO "tmplib_base_io" VALUES('pidUnifImp','auto','Mode (automate)',3,160,'PID|auto',7,'Режим (автомат)','ПІД|auto','Режим (автомат)','ПИД|auto');
INSERT INTO "tmplib_base_io" VALUES('pidUnifImp','analog','Analog',3,144,'PID|analog',8,'Аналоговий','ПІД|analog','Аналоговый','ПИД|analog');
INSERT INTO "tmplib_base_io" VALUES('pidUnifImp','ed','Dimension',0,32,' ',9,'Од. виміру',' ','Ед. измерения',' ');
INSERT INTO "tmplib_base_io" VALUES('pidUnifImp','min','Scale:minimum',2,160,'PID|min',10,'Шкала:мінімум','ПІД|min','Шкала:минимум','ПИД|min');
INSERT INTO "tmplib_base_io" VALUES('pidUnifImp','max','Scale:maximum',2,160,'PID|max',11,'Шкала:максимум','ПІД|max','Шкала:максимум','ПИД|max');
INSERT INTO "tmplib_base_io" VALUES('pidUnifImp','scSqr','Scale:square',3,32,'0',12,'Шкала:квадратична','','Шкала:квадратичная','');
INSERT INTO "tmplib_base_io" VALUES('pidUnifImp','aMin','Border down alarm',2,32,'0',13,'Границя нижня ав.','','Граница нижняя ав.','');
INSERT INTO "tmplib_base_io" VALUES('pidUnifImp','aMax','Border up alarm',2,32,'0',14,'Границя верхня аварійна','','Граница верхняя ав.','');
INSERT INTO "tmplib_base_io" VALUES('pidUnifImp','wMin','Border down warning',2,32,'0',15,'Границя нижня попереджув.','','Граница нижняя пред.','');
INSERT INTO "tmplib_base_io" VALUES('pidUnifImp','wMax','Border up warning',2,32,'0',16,'Границя верхня попереджув.','','Граница верхняя пред.','');
INSERT INTO "tmplib_base_io" VALUES('pidUnifImp','HystBnd','Hysteresis of boders violation',2,32,'0',17,'Гістерезис порушень границь','','Гистерезиз наруш. границ','');
INSERT INTO "tmplib_base_io" VALUES('pidUnifImp','speed','Motion speed (%/cycle)',2,32,'0',18,'Швидкість зміни (%/цикл)','','Скорость изменения (%/цикл)','');
INSERT INTO "tmplib_base_io" VALUES('pidUnifImp','prec','Precision (signs)',1,32,'2',19,'Точність (знаків)','','Точность (знаков)','');
INSERT INTO "tmplib_base_io" VALUES('pidUnifImp','H','Border up warning violation',3,17,'0',20,'Порушення верхньої попереджув. границі','','Нарушение верхней предупредительной границы','');
INSERT INTO "tmplib_base_io" VALUES('pidUnifImp','L','Border down warning violation',3,17,'0',21,'Порушення нижньої попереджув. границі','','Нарушение нижней предупредительной границы','');
INSERT INTO "tmplib_base_io" VALUES('pidUnifImp','HH','Border up alarm violation',3,17,'0',22,'Порушення верхньої аварійної границі','','Нарушение верхней аварийной границы','');
INSERT INTO "tmplib_base_io" VALUES('pidUnifImp','LL','Border down alarm violation',3,17,'0',23,'Порушення нижньої аварійної границі','','Нарушение нижней аварийной границы','');
INSERT INTO "tmplib_base_io" VALUES('pidUnifImp','EVAL','Value is eval',3,17,'0',24,'Значення невірогідне','','Значение недостоверно','');
INSERT INTO "tmplib_base_io" VALUES('pidUnifImp','Tf','Filter time (ms)',2,32,'0',25,'Час фільтру (мс)','','Время фильтрации (мс)','');
INSERT INTO "tmplib_base_io" VALUES('pidUnifImp','Kp','Gain',2,160,'PID|Kp',26,'Kп','ПІД|Kp','Kп','ПИД|Kp');
INSERT INTO "tmplib_base_io" VALUES('pidUnifImp','Ki','Gain integration',2,160,'PID|Ki',27,'Кп інтегралу','ПІД|Ki','Кп интеграла','ПИД|Ki');
INSERT INTO "tmplib_base_io" VALUES('pidUnifImp','Ti','Ti (ms)',1,160,'PID|Ti',28,'Tі (мс)','ПІД|Ti','Tи (мс)','ПИД|Ti');
INSERT INTO "tmplib_base_io" VALUES('pidUnifImp','Kd','Gain differential',2,160,'PID|Kd',29,'Кп диференціалу','ПІД|Kd','Кп производной','ПИД|Kd');
INSERT INTO "tmplib_base_io" VALUES('pidUnifImp','Td','Td (ms)',1,160,'PID|Td',30,'Tд (мс)','ПІД|Td','Tд (мс)','ПИД|Td');
INSERT INTO "tmplib_base_io" VALUES('pidUnifImp','Tzd','T differential lag (ms)',1,160,'PID|Tzd',31,'T фільтру диференціалу (мс)','ПІД|Tzd','T задержки дифференциров. (мс)','ПИД|Tzd');
INSERT INTO "tmplib_base_io" VALUES('pidUnifImp','TImpPer','T impulses period (ms)',1,160,'PID|TImpPer',32,'T період імпульсів (мс)','ПІД|TImpPer','T период импульса (мс)','ПИД|TImpPer');
INSERT INTO "tmplib_base_io" VALUES('pidUnifImp','TImpMin','T minimal impulse (ms)',1,160,'PID|TImpMin',33,'Мінімальна довжина імпульсу (мс)','ПІД|TImpMin','T минимальная длина импульса (мс)','ПИД|TImpMin');
INSERT INTO "tmplib_base_io" VALUES('pidUnifImp','KImpRfact','Impulse rate factor',2,160,'PID|KImpRfact',34,'Рейтинг фактор імпульсів','ПІД|KImpRfact','Рейтинг фактор импульса','ПИД|KImpRfact');
INSERT INTO "tmplib_base_io" VALUES('pidUnifImp','Zi','Insensitivity (%)',2,160,'PID|Zi',35,'Нечутливість (%)','ПІД|Zi','Нечувствительность (%)','ПИД|Zi');
INSERT INTO "tmplib_base_io" VALUES('pidUnifImp','Hup','Upper output border (%)',2,160,'PID|Hup',36,'Верхня границя виходу (%)','ПІД|Hup','Верхняя граница выхода (%)','ПИД|Hup');
INSERT INTO "tmplib_base_io" VALUES('pidUnifImp','Hdwn','Lower output border (%)',2,160,'PID|Hdwn',37,'Нижня границя виходу (%)','ПІД|Hdwn','Нижняя граница выхода (%)','ПИД|Hdwn');
INSERT INTO "tmplib_base_io" VALUES('pidUnifImp','K1','Coefficient of input 1',2,160,'PID|K1',38,'Коеф. входу 1','ПІД|K1','Коэф. входа 1','ПИД|K1');
INSERT INTO "tmplib_base_io" VALUES('pidUnifImp','K2','Coefficient of input 2',2,160,'PID|K2',39,'Коеф. входу 2','ПІД|K2','Коэф. входа 2','ПИД|K2');
INSERT INTO "tmplib_base_io" VALUES('pidUnifImp','K3','Coefficient of input 3',2,160,'PID|K3',40,'Коеф. входу 3','ПІД|K3','Коэф. входа 3','ПИД|K3');
INSERT INTO "tmplib_base_io" VALUES('pidUnifImp','K4','Coefficient of input 4',2,160,'PID|K4',41,'Коеф. входу 4','ПІД|K4','Коэф. входа 4','ПИД|K4');
INSERT INTO "tmplib_base_io" VALUES('pidUnifImp','dmsk','Maximum value on eval (Max/Min)',2,64,'0',42,'Макс. парам. при недостовірності (Макс/Мин)','','Макс. парам. при недост. (Мин/Макс)','');
INSERT INTO "tmplib_base_io" VALUES('pidUnifImp','iAdd','Input addon',2,64,'0',43,'Додаток до входу','','Дополнение ко входу','');
INSERT INTO "tmplib_base_io" VALUES('pidUnifImp','iMult','Input multiplication',2,64,'1',44,'Множник входу','','Множитель ко входу','');
INSERT INTO "tmplib_base_io" VALUES('pidUnifImp','plcMin','Minimum module scale',2,64,'1',45,'Мінімум шкали модуля','','Минимум шкалы модуля','');
INSERT INTO "tmplib_base_io" VALUES('pidUnifImp','plcMax','Maximum module scale',2,64,'5',46,'Максимум шкали модуля','','Максимум шкалы модуля','');
INSERT INTO "tmplib_base_io" VALUES('pidUnifImp','plcExcess','Allowed scale exceeding (%)',2,64,'0',47,'Дозволене перевищення шкали (%)','','Допустимое превышение шкалы модуля (%)','');
INSERT INTO "tmplib_base_io" VALUES('pidUnifImp','plcImit','PLC imitation',3,64,'0',48,'Імітація ПЛК','','Имитация ПЛК','');
INSERT INTO "tmplib_base_io" VALUES('pidUnifImp','plcImitIn','PLC imitation input',2,128,'Imitation|var',49,'Вхід імітації ПЛК','Імітація|var','Вход имитации ПЛК','Имитация|var');
INSERT INTO "tmplib_base_io" VALUES('anUnifSt','in','Input',2,128,'Input|var',0,'Вхід','Вхід|var','Вход','Вход|var');
INSERT INTO "tmplib_base_io" VALUES('anUnifSt','var','Variable',2,16,'0',2,'Змінна','','Переменная','');
INSERT INTO "tmplib_base_io" VALUES('anUnifSt','H','Border up warning violation',3,17,'',3,'Порушення верхньої попереджув. границі','','Нарушение верхней предупредительной границы','');
INSERT INTO "tmplib_base_io" VALUES('anUnifSt','L','Border down warning violation',3,17,'',4,'Порушення нижньої попереджув. границі','','Нарушение нижней предупредительной границы','');
INSERT INTO "tmplib_base_io" VALUES('anUnifSt','HH','Border up alarm violation',3,17,'',5,'Порушення верхньої аварійної границі','','Нарушение верхней аварийной границы','');
INSERT INTO "tmplib_base_io" VALUES('anUnifSt','LL','Border down alarm violation',3,17,'',6,'Порушення нижньої аварійної границі','','Нарушение нижней аварийной границы','');
INSERT INTO "tmplib_base_io" VALUES('anUnifSt','EVAL','Value is eval',3,17,'',7,'Значення невірогідне','','Значение недостоверно','');
INSERT INTO "tmplib_base_io" VALUES('anUnifSt','ed','Dimension',0,32,' ',8,'Од. виміру',' ','Ед. измерения',' ');
INSERT INTO "tmplib_base_io" VALUES('anUnifSt','min','Scale:minimum',2,32,'0',9,'Шкала:мінімум','','Шкала:минимум','');
INSERT INTO "tmplib_base_io" VALUES('anUnifSt','max','Scale:maximum',2,32,'100',10,'Шкала:максимум','','Шкала:максимум','');
INSERT INTO "tmplib_base_io" VALUES('anUnifSt','scSqr','Scale:square',3,32,'0',11,'Шкала:квадратична','','Шкала:квадратичная','');
INSERT INTO "tmplib_base_io" VALUES('anUnifSt','aMin','Border down alarm',2,32,'0',16,'Границя нижня ав.','','Граница нижняя ав.','');
INSERT INTO "tmplib_base_io" VALUES('anUnifSt','aMax','Border up alarm',2,32,'0',17,'Границя верхня аварійна','','Граница верхняя ав.','');
INSERT INTO "tmplib_base_io" VALUES('anUnifSt','wMin','Border down warning',2,32,'0',18,'Границя нижня попереджув.','','Граница нижняя пред.','');
INSERT INTO "tmplib_base_io" VALUES('anUnifSt','wMax','Border up warning',2,32,'0',19,'Границя верхня попереджув.','','Граница верхняя пред.','');
INSERT INTO "tmplib_base_io" VALUES('anUnifSt','HystBnd','Hysteresis of boders violation',2,32,'0',20,'Гистерезис нарушений границ','','Гистерезиз наруш. границ','');
INSERT INTO "tmplib_base_io" VALUES('anUnifSt','speed','Motion speed (%/cycle)',2,32,'0',21,'Швидкість зміни (%/цикл)','','Скорость изменения (%/цикл)','');
INSERT INTO "tmplib_base_io" VALUES('anUnifSt','prec','Precision (signs)',1,32,'2',22,'Точність (знаків)','','Точность (знаков)','');
INSERT INTO "tmplib_base_io" VALUES('anUnifSt','Tf','Filter time, s',2,32,'0',24,'Час фільтру (с)','','Время фильтрации (с)','');
INSERT INTO "tmplib_base_io" VALUES('anUnifSt','iAdd','Input addon',2,64,'0',25,'Додаток до входу','','Дополнение ко входу','');
INSERT INTO "tmplib_base_io" VALUES('anUnifSt','iMult','Input multiplication',2,64,'1',26,'Множник входу','','Множитель ко входу','');
INSERT INTO "tmplib_base_io" VALUES('anUnifSt','plcMin','Minimum module scale',2,64,'1',27,'Мінімум шкали модуля','','Минимум шкалы модуля','');
INSERT INTO "tmplib_base_io" VALUES('anUnifSt','plcMax','Maximum module scale',2,64,'5',28,'Максимум шкали модуля','','Максимум шкалы модуля','');
INSERT INTO "tmplib_base_io" VALUES('anUnifSt','plcExcess','Allowed scale exceeding, %',2,64,'0',29,'Дозволене перевищення шкали (%)','','Допустимое превышение шкалы модуля (%)','');
INSERT INTO "tmplib_base_io" VALUES('anUnifSt','plcImit','PLC imitation',3,64,'0',30,'Імітація ПЛК','','Имитация ПЛК','');
INSERT INTO "tmplib_base_io" VALUES('anUnifSt','plcImitIn','PLC imitation input',2,128,'Imitation|var',31,'Вхід імітації ПЛК','Імітація|var','Вход имитации ПЛК','Имитация|var');
INSERT INTO "tmplib_base_io" VALUES('pidUnif','in','Input',2,128,'Signal|in',0,'Вхід','','Вход','Signal|in');
INSERT INTO "tmplib_base_io" VALUES('pidUnif','sp','Setpoint',2,160,'PID|sp',1,'Завдання','ПІД|sp','Задание','ПИД|sp');
INSERT INTO "tmplib_base_io" VALUES('pidUnif','var','Variable',2,16,'0',2,'Змінна','','Переменная','');
INSERT INTO "tmplib_base_io" VALUES('pidUnif','manIn','Manual input (%)',2,160,'PID|manIn',3,'Ручний ввід (%)','ПІД|manIn','Ручной ввод (%)','ПИД|manIn');
INSERT INTO "tmplib_base_io" VALUES('pidUnif','out','Output (%)',2,160,'PID|out',4,'Вихід (%)','ПІД|out','Выход (%)','ПИД|out');
INSERT INTO "tmplib_base_io" VALUES('pidUnif','auto','Mode (automate)',3,160,'PID|auto',5,'Режим (автомат)','ПІД|auto','Режим (автомат)','ПИД|auto');
INSERT INTO "tmplib_base_io" VALUES('pidUnif','casc','Mode (cascad)',3,160,'PID|casc',6,'Режим (каскад)','ПІД|casc','Режим (каскад)','ПИД|casc');
INSERT INTO "tmplib_base_io" VALUES('pidUnif','ed','Dimension',0,32,' ',7,'Од. виміру',' ','Ед. измерения',' ');
INSERT INTO "tmplib_base_io" VALUES('pidUnif','min','Scale:minimum',2,160,'PID|min',8,'Шкала:мінімум','ПІД|min','Шкала:минимум','ПИД|min');
INSERT INTO "tmplib_base_io" VALUES('pidUnif','max','Scale:maximum',2,160,'PID|max',9,'Шкала:максимум','ПІД|max','Шкала:максимум','ПИД|max');
INSERT INTO "tmplib_base_io" VALUES('pidUnif','scSqr','Scale:square',3,32,'0',10,'Шкала:квадратична','','Шкала:квадратичная','');
INSERT INTO "tmplib_base_io" VALUES('pidUnif','aMin','Border down alarm',2,32,'0',11,'Границя нижня ав.','','Граница нижняя ав.','');
INSERT INTO "tmplib_base_io" VALUES('pidUnif','aMax','Border up alarm',2,32,'0',12,'Границя верхня аварійна','','Граница верхняя ав.','');
INSERT INTO "tmplib_base_io" VALUES('pidUnif','wMin','Border down warning',2,32,'0',13,'Границя нижня попереджув.','','Граница нижняя пред.','');
INSERT INTO "tmplib_base_io" VALUES('pidUnif','wMax','Border up warning',2,32,'0',14,'Границя верхня попереджув.','','Граница верхняя пред.','');
INSERT INTO "tmplib_base_io" VALUES('pidUnif','HystBnd','Hysteresis of boders violation',2,32,'0',15,'Гістерезис порушень границь','','Гистерезиз наруш. границ','');
INSERT INTO "tmplib_base_io" VALUES('pidUnif','speed','Motion speed (%/cycle)',2,32,'0',16,'Швидкість зміни (%/цикл)','','Скорость изменения (%/цикл)','');
INSERT INTO "tmplib_base_io" VALUES('pidUnif','prec','Precision (signs)',1,32,'2',17,'Точність (знаків)','','Точность (знаков)','');
INSERT INTO "tmplib_base_io" VALUES('pidUnif','H','Border up warning violation',3,17,'0',18,'Порушення верхньої попереджув. границі','','Нарушение верхней предупредительной границы','');
INSERT INTO "tmplib_base_io" VALUES('pidUnif','L','Border down warning violation',3,17,'0',19,'Порушення нижньої попереджув. границі','','Нарушение нижней предупредительной границы','');
INSERT INTO "tmplib_base_io" VALUES('pidUnif','HH','Border up alarm violation',3,17,'0',20,'Порушення верхньої аварійної границі','','Нарушение верхней аварийной границы','');
INSERT INTO "tmplib_base_io" VALUES('pidUnif','LL','Border down alarm violation',3,17,'0',21,'Порушення нижньої аварійної границі','','Нарушение нижней аварийной границы','');
INSERT INTO "tmplib_base_io" VALUES('pidUnif','EVAL','Value is eval',3,17,'0',22,'Значення невірогідне','','Значение недостоверно','');
INSERT INTO "tmplib_base_io" VALUES('pidUnif','Tf','Filter time (ms)',2,32,'0',23,'Час фільтру (мс)','','Время фильтрации (мс)','');
INSERT INTO "tmplib_base_io" VALUES('pidUnif','Kp','Gain',2,160,'PID|Kp',24,'Kп','ПІД|Kp','Kп','ПИД|Kp');
INSERT INTO "tmplib_base_io" VALUES('pidUnif','Ti','Ti (ms)',1,160,'PID|Ti',25,'Tі (мс)','ПІД|Ti','Tи (мс)','ПИД|Ti');
INSERT INTO "tmplib_base_io" VALUES('pidUnif','Kd','Gain differential',2,160,'PID|Kd',26,'Кп диференціалу','ПІД|Kd','Кп производной','ПИД|Kd');
INSERT INTO "tmplib_base_io" VALUES('pidUnif','Td','Td (ms)',1,160,'PID|Td',27,'Tд (мс)','ПІД|Td','Tд (мс)','ПИД|Td');
INSERT INTO "tmplib_base_io" VALUES('pidUnif','Tzd','T differential lag (ms)',1,160,'PID|Tzd',28,'T фільтру диференціалу (мс)','ПІД|Tzd','T задержки дифференциров. (мс)','ПИД|Tzd');
INSERT INTO "tmplib_base_io" VALUES('pidUnif','Zi','Insensitivity (%)',2,160,'PID|Zi',29,'Нечутливість (%)','ПІД|Zi','Нечувствительность (%)','ПИД|Zi');
INSERT INTO "tmplib_base_io" VALUES('pidUnif','Hup','Upper output border (%)',2,160,'PID|Hup',30,'Верхня границя виходу (%)','ПІД|Hup','Верхняя граница выхода (%)','ПИД|Hup');
INSERT INTO "tmplib_base_io" VALUES('pidUnif','Hdwn','Lower output border (%)',2,160,'PID|Hdwn',31,'Нижня границя виходу (%)','ПІД|Hdwn','Нижняя граница выхода (%)','ПИД|Hdwn');
INSERT INTO "tmplib_base_io" VALUES('pidUnif','K1','Coefficient of input 1',2,160,'PID|K1',32,'Коеф. входу 1','ПІД|K1','Коэф. входа 1','ПИД|K1');
INSERT INTO "tmplib_base_io" VALUES('pidUnif','K2','Coefficient of input 2',2,160,'PID|K2',33,'Коеф. входу 2','ПІД|K2','Коэф. входа 2','ПИД|K2');
INSERT INTO "tmplib_base_io" VALUES('pidUnif','K3','Coefficient of input 3',2,160,'PID|K3',34,'Коеф. входу 3','ПІД|K3','Коэф. входа 3','ПИД|K3');
INSERT INTO "tmplib_base_io" VALUES('pidUnif','K4','Coefficient of input 4',2,160,'PID|K4',35,'Коеф. входу 4','ПІД|K4','Коэф. входа 4','ПИД|K4');
INSERT INTO "tmplib_base_io" VALUES('pidUnif','dmsk','Maximum value on eval (Max/Min)',2,64,'0',36,'Макс. парам. при недостовірності (Макс/Мин)','','Макс. парам. при недост. (Мин/Макс)','');
INSERT INTO "tmplib_base_io" VALUES('pidUnif','iAdd','Input addon',2,64,'0',37,'Додаток до входу','','Дополнение ко входу','');
INSERT INTO "tmplib_base_io" VALUES('pidUnif','iMult','Input multiplication',2,64,'1',38,'Множник входу','','Множитель ко входу','');
INSERT INTO "tmplib_base_io" VALUES('pidUnif','plcMin','Minimum module scale',2,64,'1',39,'Мінімум шкали модуля','','Минимум шкалы модуля','');
INSERT INTO "tmplib_base_io" VALUES('pidUnif','plcMax','Maximum module scale',2,64,'5',40,'Максимум шкали модуля','','Максимум шкалы модуля','');
INSERT INTO "tmplib_base_io" VALUES('pidUnif','plcExcess','Allowed scale exceeding (%)',2,64,'0',41,'Дозволене перевищення шкали (%)','','Допустимое превышение шкалы модуля (%)','');
INSERT INTO "tmplib_base_io" VALUES('pidUnif','plcImit','PLC imitation',3,64,'0',42,'Імітація ПЛК','','Имитация ПЛК','');
INSERT INTO "tmplib_base_io" VALUES('pidUnif','plcImitIn','PLC imitation input',2,128,'Imitation|var',43,'Вхід імітації ПЛК','Імітація|var','Вход имитации ПЛК','Имитация|var');
INSERT INTO "tmplib_base_io" VALUES('manInUnif','varIn','Variable input',2,32,'0',2,'Введення змінної','','Ввод переменной','');
INSERT INTO "tmplib_base_io" VALUES('pidUnif','SHIFR','Code',0,0,'',45,'Шифр','','Шифр','');
INSERT INTO "tmplib_base_io" VALUES('anUnifSt','this','Object',4,1,'',32,'Об''єкт','','Объект','');
INSERT INTO "tmplib_base_io" VALUES('anUnifSt','SHIFR','Code',0,0,'',33,'Шифр','','Шифр','');
INSERT INTO "tmplib_base_io" VALUES('anUnifSt','NAME','Name',0,0,'',34,'Ім''я','','Имя','');
INSERT INTO "tmplib_base_io" VALUES('anUnifSt','DESCR','Description',0,0,'',35,'Опис','','Описание','');
INSERT INTO "tmplib_base_io" VALUES('anUnif','this','Object',4,1,'',27,'Об''єкт','','Объект','');
INSERT INTO "tmplib_base_io" VALUES('pidUnif','NAME','Name',0,0,'',46,'Ім''я','','Имя','');
INSERT INTO "tmplib_base_io" VALUES('pidUnif','this','Object',4,1,'',44,'Об''єкт','','Объект','');
INSERT INTO "tmplib_base_io" VALUES('anUnif','SHIFR','Code',0,0,'',28,'Шифр','','Шифр','');
INSERT INTO "tmplib_base_io" VALUES('anUnif','NAME','Name',0,0,'',29,'Ім''я','','Имя','');
INSERT INTO "tmplib_base_io" VALUES('anUnif','DESCR','Description',0,0,'',30,'Опис','','Описание','');
INSERT INTO "tmplib_base_io" VALUES('pidUnif','DESCR','Description',0,0,'',47,'Опис','','Описание','');
INSERT INTO "tmplib_base_io" VALUES('pidUnifImp','this','Object',4,1,'',50,'Об''єкт','','Объект','');
INSERT INTO "tmplib_base_io" VALUES('pidUnifImp','SHIFR','Code',0,0,'',51,'Шифр','','Шифр','');
INSERT INTO "tmplib_base_io" VALUES('pidUnifImp','NAME','Name',0,0,'',52,'Ім''я','','Имя','');
INSERT INTO "tmplib_base_io" VALUES('pidUnifImp','DESCR','Description',0,0,'',53,'Опис','','Описание','');
INSERT INTO "tmplib_base_io" VALUES('anUnif','HystBnd','Hysteresis of boders violation',2,32,'1',15,'Гістерезиз поруш. границь','','Гистерезиз наруш. границ','');
INSERT INTO "tmplib_base_io" VALUES('anUnif','inProc','Input processing procedure',0,68,'',1,'Процедура обробки входу','','Процедура обработки входа','');
INSERT INTO "tmplib_base_io" VALUES('anUnif','subMode','Substitute:mode (0-no; 1-last; 2-substitute)',1,32,'0',7,'Заміна:режим (0-немає; 1-останне; 2-заміна)','','Замена:режим (0-нет; 1-последний; 2-замена)','');
INSERT INTO "tmplib_base_io" VALUES('anUnif','subVar','Substitute:variable',2,32,'0',8,'Заміна:змінна','','Замена:переменная','');
INSERT INTO "tmplib_base_io" VALUES('anUnif','alSup','Alarms (0-init NORMA, 1-suppress, 2-save on change)',1,32,'0',9,'Порушення (0-ініціаліз. НОРМА, 1-придушення, 2-збереж. при зміні)','','Нарушения (0-принужд. НОРМА, 1-подавление, 2-сохр. при смене)','');
INSERT INTO "tmplib_base_io" VALUES('anUnifSt','inProc','Input processing procedure',0,68,'',1,'Процедура обробки входу','','Процедура обработки входа','');
INSERT INTO "tmplib_base_io" VALUES('anUnifSt','subMode','Substitute:mode (0-no; 1-last; 2-substitute)',1,32,'0',12,'Заміна:режим (0-немає; 1-останне; 2-заміна)','','Замена:режим (0-нет; 1-последний; 2-замена)','');
INSERT INTO "tmplib_base_io" VALUES('anUnifSt','subVar','Substitute:variable',2,32,'0',13,'Заміна:змінна','','Замена:переменная','');
INSERT INTO "tmplib_base_io" VALUES('anUnifSt','alSup','Alarms (0-init NORMA, 1-suppress, 2-save on change)',1,32,'0',14,'Порушення (0-инициализ. НОРМА, 1-придушення, 2-збереж. при зміні)','','Нарушения (0-принужд. НОРМА, 1-подавление, 2-сохр. при смене)','');
INSERT INTO "tmplib_base_io" VALUES('manInUnif','alSup','Alarms suppress',3,32,'0',7,'Придушення порушень','','Подавление нарушений','');
INSERT INTO "tmplib_base_io" VALUES('manInUnif','HystBnd','Hysteresis of boders violation',2,32,'1',12,'Гістерезиз поруш. границь','','Гистерезиз наруш. границ','');
INSERT INTO "tmplib_base_io" VALUES('manInUnif','prec','Precision (signs)',1,32,'2',14,'Точність (знаків)','','Точность (знаков)','');
INSERT INTO "tmplib_base_io" VALUES('manInUnif','this','Object',4,1,'',22,'Об''єкт','','Объект','');
INSERT INTO "tmplib_base_io" VALUES('manInUnif','SHIFR','Code',0,0,'',23,'Шифр','','Шифр','');
INSERT INTO "tmplib_base_io" VALUES('manInUnif','NAME','Name',0,0,'',24,'Ім''я','','Имя','');
INSERT INTO "tmplib_base_io" VALUES('manInUnif','DESCR','Description',0,0,'',25,'Опис','','Описание','');
INSERT INTO "tmplib_base_io" VALUES('SNMP','srcAddr','Source object''s address',0,64,'',0,'Адреса вихідного об''єкту','','Адрес исходного объекта','');
INSERT INTO "tmplib_base_io" VALUES('SNMP','manTables','Manual tables',0,64,'',1,'Ручні таблиці','','Ручные таблицы','');
INSERT INTO "tmplib_base_io" VALUES('SNMP','items','All items',4,33,'',2,'Всі елементи','','Все элементы','');
INSERT INTO "tmplib_base_io" VALUES('SNMP','this','The object',4,0,'',3,'Об''єкт','','Объект','');
INSERT INTO "tmplib_base_io" VALUES('SNMP','SHIFR','Code',0,0,'',4,'Шифр','','Шифр','');
INSERT INTO "tmplib_base_io" VALUES('SNMP','NAME','Name',0,0,'',5,'Ім''я','','Имя','');
INSERT INTO "tmplib_base_io" VALUES('SNMP','DESCR','Description',0,0,'',6,'Опис','','Описание','');
INSERT INTO "tmplib_base_io" VALUES('digAlarm','alrm','Alarm "{st}:{lev}:{mess}"',0,64,' ',0,'Сигнал "{st}:{lev}:{mess}"','','Сигнал "{st}:{lev}:{mess}"','');
INSERT INTO "tmplib_base_io" VALUES('digAlarm','SHIFR','Code',0,0,'',6,'Шифр','','Шифр','');
INSERT INTO "tmplib_base_io" VALUES('digAlarm','NAME','Name',0,0,'',7,'Ім''я','','Имя','');
INSERT INTO "tmplib_base_io" VALUES('digAlarm','DESCR','Description',0,0,'',8,'Опис','','Описание','');
INSERT INTO "tmplib_base_io" VALUES('digAlarm','this','The object',4,0,'',9,'Опис','','Описание','');
INSERT INTO "tmplib_base_io" VALUES('anUnif','log','Logarithmic scale',3,32,'0',18,'Логарифмічна шкала','','Логарифмическая шкала','');
INSERT INTO "tmplib_base_io" VALUES('manInUnif','log','Logarithmic scale',3,32,'0',15,'Логарифмічна шкала','','Логарифмическая шкала','');
INSERT INTO "tmplib_base_io" VALUES('digAlarm','stInv','State inverse',3,64,'',1,'Інверсія стану','','Инверсия состояния','');
INSERT INTO "tmplib_base_io" VALUES('digAlarm','st_open','State "Opened"',3,16,'',4,'Стан "Відкрито"','','Состояние "Открыто"','');
INSERT INTO "tmplib_base_io" VALUES('digAlarm','st_close','State "Closed"',3,16,'',5,'Стан "Закрито"','','Состояние "Закрыто"','');
INSERT INTO "tmplib_base_io" VALUES('anUnifSt','log','Logarithmic scale',3,32,'0',23,'Логарифмічна шкала','','Логарифмическая шкала','');
INSERT INTO "tmplib_base_io" VALUES('digAlarm','inProc','Input processing procedure',0,68,'',3,'Процедура обробки входу','','Процедура обработки входа','');
INSERT INTO "tmplib_base_io" VALUES('anUnif','alDelay','Alarms delay, s',2,32,'0',10,'Затримка порушення, с','','Задержка нарушения, с','');
INSERT INTO "tmplib_base_io" VALUES('anUnifSt','alDelay','Alarms delay, s',2,32,'',15,'Затримка порушення, с','','Задержка нарушения, с','');
CREATE TABLE 'DAQ_JavaLikeCalc' ("ID" TEXT DEFAULT '' ,"NAME" TEXT DEFAULT '' ,"ru#NAME" TEXT DEFAULT '' ,"uk#NAME" TEXT DEFAULT '' ,"DESCR" TEXT DEFAULT '' ,"ru#DESCR" TEXT DEFAULT '' ,"uk#DESCR" TEXT DEFAULT '' ,"ENABLE" INTEGER DEFAULT '0' ,"START" INTEGER DEFAULT '0' ,"MESS_LEV" INTEGER DEFAULT '3' ,"REDNT" INTEGER DEFAULT '0' ,"REDNT_RUN" TEXT DEFAULT '<high>' ,"PRM_BD" TEXT DEFAULT 'system' ,"FUNC" TEXT DEFAULT '' ,"SCHEDULE" TEXT DEFAULT '1' ,"PRIOR" INTEGER DEFAULT '0' ,"ITER" INTEGER DEFAULT '1' , PRIMARY KEY ("ID"));
CREATE TABLE 'DAQ_LogicLev' ("ID" TEXT DEFAULT '' ,"NAME" TEXT DEFAULT '' ,"ru#NAME" TEXT DEFAULT '' ,"uk#NAME" TEXT DEFAULT '' ,"DESCR" TEXT DEFAULT '' ,"ru#DESCR" TEXT DEFAULT '' ,"uk#DESCR" TEXT DEFAULT '' ,"ENABLE" INTEGER DEFAULT '0' ,"START" INTEGER DEFAULT '0' ,"MESS_LEV" INTEGER DEFAULT '3' ,"REDNT" INTEGER DEFAULT '0' ,"REDNT_RUN" TEXT DEFAULT '<high>' ,"PRM_BD" TEXT DEFAULT '' ,"PRM_BD_REFL" TEXT DEFAULT '' ,"PERIOD" INTEGER DEFAULT '0' ,"SCHEDULE" TEXT DEFAULT '1' ,"PRIOR" INTEGER DEFAULT '0' , PRIMARY KEY ("ID"));
CREATE TABLE 'flb_web_io' ("F_ID" TEXT DEFAULT '' ,"ID" TEXT DEFAULT '' ,"NAME" TEXT DEFAULT '' ,"TYPE" INTEGER DEFAULT '' ,"MODE" INTEGER DEFAULT '' ,"DEF" TEXT DEFAULT '' ,"HIDE" INTEGER DEFAULT '' ,"POS" INTEGER DEFAULT '' ,"ru#NAME" TEXT DEFAULT '' ,"uk#NAME" TEXT DEFAULT '' , PRIMARY KEY ("F_ID","ID"));
INSERT INTO "flb_web_io" VALUES('alarms','rules','Rules',0,1,'\n',0,0,'Правила','Правила');
INSERT INTO "flb_web_io" VALUES('devMon','rez','Result',0,2,'',0,0,'Результат','Результат');
INSERT INTO "flb_web_io" VALUES('devMon','urlPrms','Address parameters',4,0,'',0,1,'Адрес параметров','Адреса параметрів');
INSERT INTO "flb_web_io" VALUES('devMon','tmpl','Template',0,0,'',0,2,'Шаблон','Шаблон');
INSERT INTO "flb_web_io" VALUES('devMon','cnts','POST request''s context',4,0,'',0,3,'Контекст POST-запроса','Контекст POST-запиту');
INSERT INTO "flb_web_io" VALUES('devMon','user','User',0,0,'',0,4,'Пользователь','Користувач');
INSERT INTO "flb_web_io" VALUES('devices','rez','Result',0,2,'',0,0,'Результат','Результат');
INSERT INTO "flb_web_io" VALUES('devices','urlPrms','Address parameters',4,0,'',0,1,'Адрес параметров','Адреса параметрів');
INSERT INTO "flb_web_io" VALUES('devices','tmpl','Template',0,0,'',0,2,'Шаблон','Шаблон');
INSERT INTO "flb_web_io" VALUES('devices','cnts','POST request''s context',4,0,'',0,3,'Контекст POST-запроса','Контекст POST-запиту');
INSERT INTO "flb_web_io" VALUES('devices','user','User',0,0,'',0,4,'Пользователь','Користувач');
INSERT INTO "flb_web_io" VALUES('mess','rez','Result',0,2,'',0,0,'Результат','Результат');
INSERT INTO "flb_web_io" VALUES('mess','urlPrms','Address parameters',4,0,'',0,1,'Адрес параметров','Адреса параметрів');
INSERT INTO "flb_web_io" VALUES('mess','tmpl','Template',0,0,'',0,2,'Шаблон','Шаблон');
INSERT INTO "flb_web_io" VALUES('mess','cnts','POST request''s context',4,0,'',0,3,'Контекст POST-запроса','Контекст POST-запиту');
INSERT INTO "flb_web_io" VALUES('mess','user','User',0,0,'',0,4,'Пользователь','Користувач');
INSERT INTO "flb_web_io" VALUES('report','rez','Result',0,2,'',0,0,'Результат','Результат');
INSERT INTO "flb_web_io" VALUES('report','urlPrms','Address parameters',4,0,'',0,1,'Адрес параметров','Адреса параметрів');
INSERT INTO "flb_web_io" VALUES('report','tmpl','Template',0,0,'',0,2,'Шаблон','Шаблон');
INSERT INTO "flb_web_io" VALUES('report','cnts','POST request''s context',4,0,'',0,3,'Контекст POST-запроса','Контекст POST-запиту');
INSERT INTO "flb_web_io" VALUES('report','user','User',0,0,'',0,4,'Пользователь','Користувач');
INSERT INTO "flb_web_io" VALUES('test','f_start','Start the function flag',3,0,'0',0,0,'Флаг запуска функции','Прапорець запуску функції');
INSERT INTO "flb_web_io" VALUES('test','test','Test',0,1,'',0,1,'Тест','Тест');
INSERT INTO "flb_web_io" VALUES('timerWrite','rules','Rules',0,1,'\n',0,0,'Правила','Правила');
INSERT INTO "flb_web_io" VALUES('user','rez','Result',0,2,'',0,0,'Результат','Результат');
INSERT INTO "flb_web_io" VALUES('user','urlPrms','Address parameters',4,0,'',0,1,'Адрес параметров','Адреса параметрів');
INSERT INTO "flb_web_io" VALUES('user','tmpl','Template',0,0,'',0,2,'Шаблон','Шаблон');
INSERT INTO "flb_web_io" VALUES('user','cnts','POST request''s context',4,0,'',0,3,'Контекст POST-запроса','Контекст POST-запиту');
INSERT INTO "flb_web_io" VALUES('user','user','User',0,0,'',0,4,'Пользователь','Користувач');
CREATE TABLE 'flb_web' ("ID" TEXT DEFAULT '' ,"NAME" TEXT DEFAULT '' ,"DESCR" TEXT DEFAULT '' ,"MAXCALCTM" INTEGER DEFAULT '10' ,"FORMULA" TEXT DEFAULT '' ,"TIMESTAMP" INTEGER DEFAULT '' ,"ru#NAME" TEXT DEFAULT '' ,"uk#NAME" TEXT DEFAULT '' , PRIMARY KEY ("ID"));
INSERT INTO "flb_web" VALUES('alarms','Alarms','',10,'//> Load rules
var alarmsTree = SYS.XMLNode("ALARMS");
alarmsTree.load(rules);
for(i_rl = 0; i_rl < alarmsTree.childSize(); i_rl++)
{
	if(!alarmsTree.childGet(i_rl).text().length) continue;
	var subId = alarmsTree.childGet(i_rl).attr("id").parse(0,".");
	var devId =  alarmsTree.childGet(i_rl).attr("id").parse(1,".");
	var curAttrVal = SYS.DAQ.ModBus.ZegBee[subId][devId].get();
	var curErr = "";
	if(curAttrVal.isEVal()) curErr = "Value error. Perhaps the device is missing.";
	else
	{
		var progArgs = new Object();
		progArgs.err = "";
		progArgs.x = curAttrVal;
		SYS.DAQ.funcCall("JavaLikeCalc.JavaScript",progArgs,alarmsTree.childGet(i_rl).text());
		if(progArgs.err.length) curErr = progArgs.err;
	}
	//> Create alarm
	if(curErr.length && curErr != alarmsTree.childGet(i_rl).attr("err"))
	{
		SYS.message("ALARM:House:"+alarmsTree.childGet(i_rl).attr("id"),-4,curErr);
		//>> Send SMS for new alarm messages to users
		var smsReq = SYS.XMLNode("send").setAttr("ProtIt","SMS").setText(alarmsTree.childGet(i_rl).attr("id")+": "+curErr);
		var userLst = SYS.XMLNode("get").setAttr("path","/sub_Security/grp_Web/%2fprm%2fusers");
		SYS.cntrReq(userLst);
		for(var i = 0; i < userLst.childSize(); i++)
		{
			var isSelAdmin = SYS.Security.access(userLst.childGet(i).text(),4,"root","WebRoot",0440);
			var treeEl = SYS.XMLNode("get").setAttr("path","/sub_Security/usr_"+userLst.childGet(i).text()+"/%2fprm%2fldscr");
			SYS.cntrReq(treeEl);
			var re = treeEl.text().match("SMS:\\s+([^\\n]*)");
			if(!re.length || re[1] != "true") continue;
			if(!isSelAdmin)
			{
				re=treeEl.text().match("sub_"+subId+":\\s+([^\\n]*)");
				if(!re.length || re[1] == "--") continue;
			}
			re = treeEl.text().match("TEL:\\s+([^\\n]*)");
			if(!re.length) continue;
			var telLs = re[1].split(new RegExp("\\s+"));
			for(var i_t = 0; i_t < telLs.length; i_t++)
			{
				smsReq.setAttr("tel",telLs[i_t]);
				SYS.Transport.Serial.out_GSM.messIO(smsReq,"UserProtocol");
				if(smsReq.attr("err") != "0") SYS.messWarning("SMS send",smsReq.attr("err"));
			}
		}
	}
	//> Clear alarm
	else if(!curErr.length && !alarmsTree.childGet(i_rl).attr("err").isEVal())
		SYS.message("ALARM:House:"+alarmsTree.childGet(i_rl).attr("id"),1,"Norma");
	alarmsTree.childGet(i_rl).setAttr("err",curErr);	
}
rules = alarmsTree.save();',1377261539,'Сигнализации','Сигналізації');
INSERT INTO "flb_web" VALUES('devMon','Devices monitor','',10,'var treeEl = false;

//> Parse template
var pgTree = SYS.XMLNode();
var err = pgTree.load(tmpl,false,true);

//> Access
var isAdmin = SYS.Security.access(user,4,"root","WebRoot",0440);
treeEl = SYS.XMLNode("get").setAttr("path","/sub_Security/usr_"+user+"/%2fprm%2fldscr");
SYS.cntrReq(treeEl);
re=treeEl.text().match("sub_"+pgTree.attr("class")+":\\s+([^\\n]*)");
var subAccess = re.length ? re[1] : "--";

//> Devices list place to template
var cntrPrm = SYS.DAQ.ModBus.ZegBee[pgTree.attr("class")];
catAttrReq = SYS.XMLNode("get").setAttr("path","/DAQ/ModBus/ZegBee/"+pgTree.attr("class")+"/%2fprm%2fcfg%2fATTR_LS");
SYS.cntrReq(catAttrReq);
catAttrLs = catAttrReq.text();

//> Direct requests by XML process
if(!urlPrms["com"].isEVal())
{
	var xmlRespTree = SYS.XMLNode(urlPrms["com"]);
	if(urlPrms["com"] == "info")
	{
		//> Get timers
		var timersList = SYS.XMLNode("get").setAttr("path","/sub_DAQ/mod_JavaLikeCalc/cntr_timers/prm_rules/%2fval%2fvar");
		SYS.cntrReq(timersList);
		var timersTree = SYS.XMLNode("TIMERS");
		timersTree.load(timersList.text());

		//> Devices process
		for(var off = 0; (catAttrId=catAttrLs.parse(0,"\n",off)).length; )
		{
			if((catAttrId.parse(2,":") == "r" && (isAdmin || subAccess != "--")) || (catAttrId.parse(2,":") == "rw" && (isAdmin || subAccess == "rw")))
			{
				var xmlRespEl = xmlRespTree.childAdd(catAttrId.parse(5,":")).setAttr("name",catAttrId.parse(4,":")).setAttr("id",catAttrId.parse(3,":")).setAttr("acs",catAttrId.parse(2,":"));
				if(catAttrId.parse(5,":") == "bin")
					xmlRespEl.setAttr("vl1",catAttrId.parse(6,":")).setAttr("nm1",catAttrId.parse(7,":"))
								  .setAttr("vl2",catAttrId.parse(8,":")).setAttr("nm2",catAttrId.parse(9,":")).setAttr("sensorPos",catAttrId.parse(10,":"));
				else if(catAttrId.parse(5,":") == "dec") xmlRespEl.setAttr("ed",catAttrId.parse(6,":")).setAttr("sensorPos",catAttrId.parse(8,":"));;
				//> Timer get
				var timerNd = timersTree.getElementBy(pgTree.attr("class")+"."+catAttrId.parse(3,":"));
				if(typeof(timerNd)=="XMLNode" && timerNd.attr("tm").toInt(10) > 0) xmlRespEl.setAttr("tm",timerNd.attr("tm"));
			}
		}
	}
	else if(urlPrms["com"] == "vals")
		for(var off = 0; (catAttrId=catAttrLs.parse(0,"\n",off)).length; )
		{
			if((catAttrId.parse(2,":") == "r" && (isAdmin || subAccess != "--")) || (catAttrId.parse(2,":") == "rw" && (isAdmin || subAccess == "rw")))
			{
				var curAttrVal = cntrPrm[catAttrId.parse(3,":")].get();
				var xmlRespEl = xmlRespTree.childAdd(catAttrId.parse(5,":")).setAttr("id",catAttrId.parse(3,":"));
				//>>> Value error
				if(curAttrVal.isEVal()) xmlRespEl.setAttr("val","<err>").setText("Ошибка");
				else
				{
					xmlRespEl.setAttr("val",curAttrVal);
					//>>> Prepare value for different type
					if(catAttrId.parse(5,":") == "dec")
					{
						//>>>> Calc by formule add
						var calcProg = catAttrId.parse(7,":").replace("%3A",":");
						if(calcProg.length && catAttrId.parse(2,":") != "rw")
						{
							var progArgs = new Object();
							progArgs.y = curAttrVal;
							progArgs.x = curAttrVal;
							SYS.DAQ.funcCall("JavaLikeCalc.JavaScript",progArgs,calcProg);
							curAttrVal = progArgs.y;
						}
						xmlRespEl.setText(curAttrVal);
					}
					else if(catAttrId.parse(5,":") == "bin")
					{
						if(curAttrVal == catAttrId.parse(6,":").replace("%3A",":")) curAttrVal = catAttrId.parse(7,":").replace("%3A",":");
 						else if(curAttrVal == catAttrId.parse(8,":").replace("%3A",":")) curAttrVal = catAttrId.parse(9,":").replace("%3A",":");
						else curAttrVal = "Неизвестно";
						xmlRespEl.setText(curAttrVal);
					}
				}
			}
		}
	else if(urlPrms["com"] == "set")
		for(var off = 0; (catAttrId=catAttrLs.parse(0,"\n",off)).length; )
		{
			if(catAttrId.parse(2,":") != "rw" || catAttrId.parse(3,":") != urlPrms["dev"]) continue;
			var setVal = EVAL_BOOL;
			if(catAttrId.parse(5,":") == "bin")
			{
				if(urlPrms["val"] != cntrPrm[urlPrms["dev"]].get())	setVal = urlPrms["val"];
			}
			else if(catAttrId.parse(5,":") == "dec")
			{
				var calcProg = catAttrId.parse(7,":").replace("%3A",":");
				if(!calcProg.length)
				{
					if(urlPrms["val"] != cntrPrm[urlPrms["dev"]].get()) setVal = urlPrms["val"];
				}
				else
				{
					var progArgs = new Object();
					progArgs.y = urlPrms["val"];
					progArgs.x = urlPrms["val"];
					SYS.DAQ.funcCall("JavaLikeCalc.JavaScript",progArgs,calcProg);
					setVal = progArgs.y;
				}
			}
			if(!setVal.isEVal())
			{				
				if(urlPrms["tm"].toInt(10) <= 0)
				{
					cntrPrm[urlPrms["dev"]].set(setVal);
					SYS.message("SET:House:"+pgTree.attr("class")+"."+urlPrms["dev"],2,user+": "+pgTree.attr("class")+"."+urlPrms["dev"]+": установлен в: "+setVal);
				}
				else
				{
					var timersList = SYS.XMLNode("get").setAttr("path","/sub_DAQ/mod_JavaLikeCalc/cntr_timers/prm_rules/%2fval%2fvar");
					SYS.cntrReq(timersList);
					var timersTree = SYS.XMLNode("TIMERS");
					timersTree.load(timersList.text());

					var timerNd = timersTree.getElementBy(pgTree.attr("class")+"."+urlPrms["dev"]);
					if(typeof(timerNd) != "XMLNode") timerNd = timersTree.childAdd("timer").setAttr("id",pgTree.attr("class")+"."+urlPrms["dev"]);
					timerNd.setAttr("tm",urlPrms["tm"]).setAttr("user",user).setText(setVal);
					timersList = SYS.XMLNode("set").setAttr("path","/sub_DAQ/mod_JavaLikeCalc/cntr_timers/prm_rules/%2fval%2fvar").setText(timersTree.save());
					SYS.cntrReq(timersList);
					SYS.message("SET:House:"+pgTree.attr("class")+"."+urlPrms["dev"],2,user+": "+pgTree.attr("class")+"."+urlPrms["dev"]+": установка в: "+setVal+", по таймеру: "+urlPrms["tm"]+"секунд.");
				}
			}
			break;
		}
	else if(urlPrms["com"] == "timerClear")
		for(var off = 0; (catAttrId=catAttrLs.parse(0,"\n",off)).length; )
		{
			if(catAttrId.parse(2,":") != "rw" || catAttrId.parse(3,":") != urlPrms["dev"]) continue;
			var timersList = SYS.XMLNode("get").setAttr("path","/sub_DAQ/mod_JavaLikeCalc/cntr_timers/prm_rules/%2fval%2fvar");
			SYS.cntrReq(timersList);
			var timersTree = SYS.XMLNode("TIMERS");
			timersTree.load(timersList.text());

			var timerNd = timersTree.getElementBy(pgTree.attr("class")+"."+urlPrms["dev"]);
			if(typeof(timerNd) == "XMLNode")
			{
				timerNd.setAttr("tm",0);
				timersList = SYS.XMLNode("set").setAttr("path","/sub_DAQ/mod_JavaLikeCalc/cntr_timers/prm_rules/%2fval%2fvar").setText(timersTree.save());
				SYS.cntrReq(timersList);
			}
			break;
		}

	return "200 OK\nContent-Type:text/xml\n\n"+xmlRespTree.save();
}

//Save template
rez = pgTree.save();
//SYS.messDebug("TEST Dev","TEST 00: "+typeof(devLs[formEl["cat"]+"."+itId]));',1377260842,'Монитор устройств','Монітор пристроїв');
INSERT INTO "flb_web" VALUES('devices','Devices dispatcher','',10,'var treeEl = false;

//> Parse template
var pgTree = SYS.XMLNode();
var err = pgTree.load(tmpl,false,true);

//> Get categories list
var catLs = SYS.XMLNode("get").setAttr("path","/DAQ/ModBus/ZegBee/%2fprm%2fprm");
SYS.cntrReq(catLs);

//> Get devices list
var devLs = new Object();
for(var i = 0; i < catLs.childSize(); i++)
{
	var curCat = catLs.childGet(i).attr("id");
	catAttrReq = SYS.XMLNode("get").setAttr("path","/DAQ/ModBus/ZegBee/"+curCat+"/%2fprm%2fcfg%2fATTR_LS");
	SYS.cntrReq(catAttrReq);
	catAttrLs = catAttrReq.text();
	for(var off = 0; (catAttrId=catAttrLs.parse(0,"\n",off)).length; )
		devLs[curCat+"."+catAttrId.parse(3,":")] = true;
}

//> Process page
//>> Place categories list
if(typeof(treeEl=pgTree.getElementBy("cat","name")) == "XMLNode")
	for(var i = 0; i < catLs.childSize(); i++)
		treeEl.childAdd("OPTION").setAttr("value",catLs.childGet(i).attr("id")).setText(catLs.childGet(i).text());

//> Check POST content for commands
if(cnts.length)
{
	var formEl = new Object();
	re = new RegExp("name=\"(.+)\"");
	//>> POST form elements process
	for(var i = 0; i < cnts.length; i++)
		formEl[re.exec(cnts[i].attr("Content-Disposition"))[1]] = cnts[i].text();
	//>> Check for apply
	if(!formEl["apply"].isEVal())
	{
		var applyOK = true;
		//>> ID: Check for correct form''s values
		var itId = formEl["id"].match("[^\\s].+[^\\s]");
		if(!itId.length)
		{
			treeEl = pgTree.getElementBy("idNm","name");
			treeEl.childAdd("br");
			treeEl.childAdd("span").setAttr("style","color : red;").setText("(Пустое значение недопустимо)");
			itId = "";
			applyOK = false;
		}
		else itId = itId[0];
		pgTree.getElementBy("id","name").setAttr("value",itId);

		//>> Name: Check for correct form''s values
		var itName = formEl["name"].match("[^\\s].+[^\\s]");
		if(!itName.length)
		{
			treeEl = pgTree.getElementBy("nameNm","name");
			treeEl.childAdd("br");
			treeEl.childAdd("span").setAttr("style","color : red;").setText("(Пустое значение недопустимо)");
			itName = "";
			applyOK = false;
		}
		else itName = itName[0];
		pgTree.getElementBy("name","name").setAttr("value",itName);

		//>> Address: Check for correct form''s values
		var itAddr = formEl["addr"].toInt();
		if(itAddr < 0 || itAddr > 65535)
		{
			treeEl = pgTree.getElementBy("addrNm","name");
			treeEl.childAdd("br");
			treeEl.childAdd("span").setAttr("style","color : red;").setText("(Адрес ModBus должен быть в диапазоне (1-65535))");
			applyOK = false;
		}
		pgTree.getElementBy("addr","name").setAttr("value",itAddr);

		//>> Type: Check for correct form''s values
		pgTree.getElementBy(formEl["type"],"value").setAttr("selected","selected");

		//>> sensorPos: Check for correct form''s values
		pgTree.getElementBy("sensorPos","name").setAttr("value",formEl["sensorPos"]);

		//>> Category: Check for correct form''s values
		pgTree.getElementBy(formEl["cat"],"value").setAttr("selected","selected");

		//>> Data type: 
		pgTree.getElementBy(formEl["typeData"],"value").setAttr("selected","selected");
		pgTree.getElementBy("in1").setAttr("style","display: "+((formEl["typeData"] == "bin")?"":"none"));
		pgTree.getElementBy("in2").setAttr("style","display: "+((formEl["typeData"] == "bin")?"":"none"));
		pgTree.getElementBy("ed").setAttr("style","display: "+((formEl["typeData"] == "dec")?"":"none"));
		pgTree.getElementBy("formul").setAttr("style","display: "+((formEl["typeData"] == "dec")?"":"none"));

		//>> Binary type properties
		pgTree.getElementBy("in1","name").setAttr("value",formEl["in1"]);
		pgTree.getElementBy("view1","name").setAttr("value",formEl["view1"]);
		pgTree.getElementBy("in2","name").setAttr("value",formEl["in2"]);
		pgTree.getElementBy("view2","name").setAttr("value",formEl["view2"]);

		//>> Digital type properties
		pgTree.getElementBy("ed","name").setAttr("value",formEl["ed"]);
		pgTree.getElementBy("formul","name").setAttr("value",formEl["formul"]);	

		//>> Alarm
		pgTree.getElementBy("alarm","name").childGet(0).setText(formEl["alarm"]);

		//> Add or update device
		if(applyOK)
		{
			//>> Get curent attributes list for category
			catAttrReq = SYS.XMLNode("get").setAttr("path","/DAQ/ModBus/ZegBee/"+formEl["cat"]+"/%2fprm%2fcfg%2fATTR_LS");
			SYS.cntrReq(catAttrReq);
			catAttrLs = catAttrReq.text();
			var newAttrReq = "R:"+itAddr+":"+(formEl["type"]=="out"?"rw":"r")+":"+itId+":"+itName.replace(":","%3A")+":"+formEl["typeData"];
			if(formEl["typeData"] == "bin") newAttrReq += ":"+formEl["in1"].replace(":","%3A")+":"+formEl["view1"].replace(":","%3A")+":"+formEl["in2"].replace(":","%3A")+":"+formEl["view2"].replace(":","%3A");
			else if(formEl["typeData"] == "dec") newAttrReq += ":"+formEl["ed"].replace(":","%3A")+":"+formEl["formul"].replace(":","%3A");
			newAttrReq += ":"+formEl["sensorPos"]+"\n";
			//>> Append record
			var isAppend = typeof(devLs[formEl["cat"]+"."+itId]) == "null";
			if(isAppend)
			{
				catAttrLs += newAttrReq;
				devLs[formEl["cat"]+"."+itId] = true;
			}
			//>> Replace
			else catAttrLs = catAttrLs.replace(new RegExp("R:[^:]*:[^:]*:"+itId+":[^\n]*\n"),newAttrReq);
			//>> Write new attributes list
			catAttrReq = SYS.XMLNode("CntrReqs").setAttr("path","/DAQ/ModBus/ZegBee/"+formEl["cat"]);
			catAttrReq.childAdd("set").setAttr("path","/%2fprm%2fcfg%2fATTR_LS").setText(catAttrLs);
			catAttrReq.childAdd("set").setAttr("path","/%2fprm%2fst%2fen").setText(0);
			catAttrReq.childAdd("set").setAttr("path","/%2fprm%2fst%2fen").setText(1);
			catAttrReq.childAdd("save").setAttr("path","/%2fobj");
			//XMLNode("set").setAttr("path","/DAQ/ModBus/ZegBee/"+formEl["cat"]+"/%2fprm%2fcfg%2fATTR_LS").setText(catAttrLs);
			SYS.cntrReq(catAttrReq);
			//>> Set message about correct apply
			treeEl=pgTree.getElementBy("mess","name");
			treeEl.childAdd("span").setAttr("style","color : green;").setText(
				isAppend?"Новое устройство успешно добавлено. Можно продолжать.":"Устройство успешно обновлено. Можно продолжать.");
			//>> Set alarm procedure
			var alarmsList = SYS.XMLNode("get").setAttr("path","/sub_DAQ/mod_JavaLikeCalc/cntr_alarms/prm_rules/%2fval%2fvar");
			SYS.cntrReq(alarmsList);
			var alarmsTree = SYS.XMLNode("ALARMS");
			alarmsTree.load(alarmsList.text());
			var alarmNd = alarmsTree.getElementBy(formEl["cat"]+"."+formEl["id"]);
			if(typeof(alarmNd)=="XMLNode") alarmNd.setText(formEl["alarm"]);
			else alarmsTree.childAdd("it").setAttr("id",formEl["cat"]+"."+formEl["id"]).setText(formEl["alarm"]);
			alarmsList = SYS.XMLNode("CntrReqs").setAttr("path","/sub_DAQ/mod_JavaLikeCalc/cntr_alarms/prm_rules");
			alarmsList.childAdd("set").setAttr("path","/%2fval%2fvar").setText(alarmsTree.save());
			alarmsList.childAdd("save").setAttr("path","/%2fobj");
			SYS.cntrReq(alarmsList);
		}
		//> Form error
		else
		{
			treeEl=pgTree.getElementBy("mess","name");
			treeEl.childAdd("span").setAttr("style","color : red;").setText("Ошибка в формировании формы! Исправьте отмеченные поля и повторите.");
		}
	}
	else if(!formEl["edDel"].isEVal())
	{
		if(formEl["edId"].length && typeof(devLs[formEl["edId"]]) != "null")
		{
			//>> Get curent attributes list for category
			catAttrReq = SYS.XMLNode("get").setAttr("path","/DAQ/ModBus/ZegBee/"+(formEl["edId"].parse(0))+"/%2fprm%2fcfg%2fATTR_LS");
			SYS.cntrReq(catAttrReq);
			catAttrLs = catAttrReq.text();
			catAttrLs = catAttrLs.replace(new RegExp("R:[^:]*:[^:]*:"+formEl["edId"].parse(1)+":[^\n]*\n"),"");
			//>> Write new attributes list
			catAttrReq = SYS.XMLNode("set").setAttr("path","/DAQ/ModBus/ZegBee/"+(formEl["edId"].parse(0))+"/%2fprm%2fcfg%2fATTR_LS").setText(catAttrLs);
			SYS.cntrReq(catAttrReq);
			//>> Set message about correct apply
			treeEl=pgTree.getElementBy("mess","name");
			treeEl.childAdd("span").setAttr("style","color : green;").setText("Устройство успешно удалено. Можно продолжать.");
			devLs[formEl["edId"]] = false;
		}
		else
		{
			treeEl=pgTree.getElementBy("mess","name");
			treeEl.childAdd("span").setAttr("style","color : red;").setText("Устройство для удаления отсутствует.");
		}
	}
}

//>> Devices list place to template
treeEl=pgTree.getElementBy("edId","name");
treeEl.childAdd("OPTION");
for(var i_o in devLs)
	if(devLs[i_o]) treeEl.childAdd("OPTION").setAttr("value",i_o).setText(i_o);

//>> Load variables for select device
if(!cnts.length && !urlPrms["selDev"].isEVal() && urlPrms["selDev"].length) 
{
	//> Select device
	pgTree.getElementBy(urlPrms["selDev"],"value").setAttr("selected","selected");
	//> Load values for select device
	catAttrReq = SYS.XMLNode("get").setAttr("path","/DAQ/ModBus/ZegBee/"+(urlPrms["selDev"].parse(0))+"/%2fprm%2fcfg%2fATTR_LS");
	SYS.cntrReq(catAttrReq);
	catAttrLs = catAttrReq.text().match("R:([^:]*):([^:]*):"+urlPrms["selDev"].parse(1)+":([^\n]*)");
	//>> ID: Check for correct form''s values
	pgTree.getElementBy("id","name").setAttr("value",urlPrms["selDev"].parse(1));
	//>> Name: Check for correct form''s values
	pgTree.getElementBy("name","name").setAttr("value",catAttrLs[3].parse(0,":").replace("%3A",":"));
	//>> Address: Check for correct form''s values
	pgTree.getElementBy("addr","name").setAttr("value",catAttrLs[1]);
	//>> Type: Check for correct form''s values
	pgTree.getElementBy(((catAttrLs[2]=="rw")?"out":"sensor"),"value").setAttr("selected","selected");
	//>> Category: Check for correct form''s values
	pgTree.getElementBy(urlPrms["selDev"].parse(0),"value").setAttr("selected","selected");
	//>> Data type: 
	var dtType = catAttrLs[3].parse(1,":");
	pgTree.getElementBy(dtType,"value").setAttr("selected","selected");
	if(dtType == "bin")
	{
		pgTree.getElementBy("in1","name").setAttr("value",catAttrLs[3].parse(2,":").replace("%3A",":"));
		pgTree.getElementBy("view1","name").setAttr("value",catAttrLs[3].parse(3,":").replace("%3A",":"));
		pgTree.getElementBy("in2","name").setAttr("value",catAttrLs[3].parse(4,":").replace("%3A",":"));
		pgTree.getElementBy("view2","name").setAttr("value",catAttrLs[3].parse(5,":").replace("%3A",":"));
		pgTree.getElementBy("sensorPos","name").setAttr("value",catAttrLs[3].parse(6,":"));
	}
	else if(dtType == "dec")
	{
		pgTree.getElementBy("ed","name").setAttr("value",catAttrLs[3].parse(2,":").replace("%3A",":"));
		pgTree.getElementBy("formul","name").setAttr("value",catAttrLs[3].parse(3,":").replace("%3A",":"));
		pgTree.getElementBy("sensorPos","name").setAttr("value",catAttrLs[3].parse(4,":"));
	}
	pgTree.getElementBy("in1").setAttr("style","display: "+((dtType == "bin")?"":"none"));
	pgTree.getElementBy("in2").setAttr("style","display: "+((dtType == "bin")?"":"none"));
	pgTree.getElementBy("ed").setAttr("style","display: "+((dtType == "dec")?"":"none"));
	pgTree.getElementBy("formul").setAttr("style","display: "+((dtType == "dec")?"":"none"));

	//>> Get alarm procedure
	alarmsList = SYS.XMLNode("get").setAttr("path","/sub_DAQ/mod_JavaLikeCalc/cntr_alarms/prm_rules/%2fval%2fvar");
	SYS.cntrReq(alarmsList);
	var alarmsTree = SYS.XMLNode();
	alarmsTree.load(alarmsList.text());
	var alarmNd = alarmsTree.getElementBy(urlPrms["selDev"]);
	pgTree.getElementBy("alarm","name").childGet(0).setText((typeof(alarmNd)=="XMLNode")?alarmNd.text():"");
}
else pgTree.getElementBy("edDel","name").setAttr("style","visibility : hidden;");

//Save template
rez = pgTree.save();
//SYS.messDebug("TEST Dev","TEST 00: "+typeof(devLs[formEl["cat"]+"."+itId]));',1377260944,'Диспетчер устройств','Диспетчер пристроїв');
INSERT INTO "flb_web" VALUES('mess','Alarm messages','',10,'var treeEl = false;

//> Parse template
var pgTree = SYS.XMLNode();
var err = pgTree.load(tmpl,false,true);
treeEl = pgTree.getElementBy("mess");

var curTime = SYS.time();
var messAr = SYS.Archive.messGet(curTime,curTime,"ALARM:House:*",-1);
var trEl = treeEl.childAdd("tr");
trEl.childAdd("th").setText("Время");
trEl.childAdd("th").setText("Категория");
trEl.childAdd("th").setText("Уровень");
trEl.childAdd("th").setText("Сообщение");
for(i = 0; i < messAr.length; i++)
{
	var trEl = treeEl.childAdd("tr");
	trEl.childAdd("td").setAttr("class","mTm").setText(SYS.strftime(messAr[i].tm));
	trEl.childAdd("td").setAttr("class","mCat").setText(messAr[i].categ);
	trEl.childAdd("td").setAttr("class","mLev").setText(messAr[i].level);
	trEl.childAdd("td").setAttr("class","mMess").setText(messAr[i].mess);
}

//Save template
rez = pgTree.save();
//SYS.messDebug("TEST Dev","TEST 00: "+typeof(devLs[formEl["cat"]+"."+itId]));',1377261138,'Сообщения сигнализаций','Повідомлення сигналізацій');
INSERT INTO "flb_web" VALUES('report','Reports','',10,'var treeEl = false;
var pgDir = "./HouseSpirit/Web/";

//> Parse template
var pgTree = SYS.XMLNode();
var err = pgTree.load(tmpl,false,true);

//> Load default values
pgTree.getElementBy("btm","name").setAttr("value",SYS.strftime(SYS.time()));
pgTree.getElementBy("etm","name").setAttr("value",SYS.strftime(SYS.time()));
var catLs = SYS.XMLNode("get").setAttr("path","/DAQ/ModBus/ZegBee/%2fprm%2fprm");
SYS.cntrReq(catLs);
treeEl = pgTree.getElementBy("subsys");
for(var i = 0; i < catLs.childSize(); i++)
{
	treeEl.childAdd("span").setText(catLs.childGet(i).text());
	treeEl.childAdd("input").setAttr("type","checkbox").setAttr("name",catLs.childGet(i).attr("id"));
	treeEl.childAdd("br");
}

//> Check POST content for commands - forming report
if(cnts.length)
{
	var formEl = new Object();
	var re = new RegExp("name=\"(.+)\"");
	//>> POST form elements process
	for(i_c = 0; i_c < cnts.length; i_c++)
		formEl[re.exec(cnts[i_c].attr("Content-Disposition"))[1]] = cnts[i_c].text();

	//>> Check for apply
	if(!formEl["show"].isEVal())
	{
		var reqRule = "";
		var applyOK = true;
		//>> Time: begin and end time check
		var btm = SYS.strptime(formEl["btm"]);
		pgTree.getElementBy("btm","name").setAttr("value",SYS.strftime(btm));
		var etm = SYS.strptime(formEl["etm"]);
		pgTree.getElementBy("etm","name").setAttr("value",SYS.strftime(etm));

		//>> System
		if(formEl["sys"].length)
		{
			pgTree.getElementBy("sys","name").setAttr("checked","checked");
			reqRule = "\\/sub_Protocol\\/mod_HTTP\\/";
		}

		//>> Subsystems
		var catLs = SYS.XMLNode("get").setAttr("path","/DAQ/ModBus/ZegBee/%2fprm%2fprm");
		SYS.cntrReq(catLs);
		for(var i = 0; i < catLs.childSize(); i++)
		{
			var subId = catLs.childGet(i).attr("id");
			if(formEl[subId].length)
			{
				pgTree.getElementBy(subId,"name").setAttr("checked","checked");
				if(reqRule.length) reqRule += "|";
				reqRule += "(ALARM|SET)\\:House\\:"+subId;
			}
		}

		//>> Report generate
		if(btm < etm && (etm-btm) < (365*24*60*60) && reqRule.length)
		{
			var messAr = SYS.Archive.messGet(btm,etm,"/("+reqRule+")/",1);
			treeEl = pgTree.getElementBy("report");
			var trEl = treeEl.childAdd("tr");
			trEl.childAdd("th").setText("Время");
			trEl.childAdd("th").setText("Категория");
			trEl.childAdd("th").setText("Уровень");
			trEl.childAdd("th").setText("Сообщение");
			for(i = 0; i < messAr.length; i++)
			{
				var trEl = treeEl.childAdd("tr");
				trEl.childAdd("td").setAttr("class","mTm").setText(SYS.strftime(messAr[i].tm));
				trEl.childAdd("td").setAttr("class","mCat").setText(messAr[i].categ);
				trEl.childAdd("td").setAttr("class","mLev").setText(messAr[i].level);
				trEl.childAdd("td").setAttr("class","mMess").setText(messAr[i].mess);
			}
			//>> Save report to file
			var saveFile = "<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n"+
					"<!DOCTYPE html PUBLIC \"-//W3C//DTD XHTML 1.0 Transitional//EN\" \"http://www.w3.org/TR/xhtml1/DTD/xhtml1-transitional.dtd\">\n"+
					"<html xmlns=''http://www.w3.org/1999/xhtml''>\n"+
					"<head>\n"+
					"  <meta http-equiv=''Content-Type'' content=''text/html; charset=UTF-8''/>\n"+
					"  <title>Отчёт</title>\n"+
					"  <style type=''text/css''>\n"+
					"    #report { width: 100%; border: 1px solid black; border-collapse: collapse; empty-cells: show; }\n"+
					"    #report TH { border: 1px solid black; background-color: #E6E6E6; }\n"+
					"    #report TD { border: 1px solid black; }\n"+
					"    #report .mTm { white-space: nowrap; }\n"+
					"    #report .mLev { text-align: center; }\n"+
					"  </style>\n"+
					"</head>\n"+
					"<body>\n"+treeEl.save()+"\n</body>\n"+
					"</html>";
			SYS.fileWrite(pgDir+"reports/rep_"+user+".html",saveFile);
			pgTree.getElementBy("saveFile").setAttr("href","reports/rep_"+user+".html").setText("rep_"+user+".html");
		}
	}
}

//Save template
rez = pgTree.save();
//SYS.messDebug("TEST Dev","TEST 00: "+typeof(devLs[formEl["cat"]+"."+itId]));',1377261196,'Отчёты','Звіти');
INSERT INTO "flb_web" VALUES('test','Test','',10,'if(f_start)
{
	var req = SYS.XMLNode("send").setAttr("ProtIt","SMS").setAttr("tel","+380679859815").setText("Тестовое сообщение");
	SYS.Transport.Serial.out_GSM.messIO(req,"UserProtocol");
}',1377261398,'Тест','Тест');
INSERT INTO "flb_web" VALUES('timerWrite','Write by the timer','',10,'//> Load rules
var timersTree = SYS.XMLNode("TIMERS");
timersTree.load(rules);
for(i_rl = 0; i_rl < timersTree.childSize(); i_rl++)
{
	var curTm = timersTree.childGet(i_rl);
	if(!curTm.text().length) continue;
	var subId = curTm.attr("id").parse(0);
	var devId = curTm.attr("id").parse(1);
	var timer = curTm.attr("tm").toInt(10);
	if(timer && max(0,timer-1) == 0)
	{
		SYS.DAQ.ModBus.ZegBee[subId][devId].set(curTm.text());
		SYS.message("SET:House:"+curTm.attr("id"),2,curTm.attr("user")+": "+curTm.attr("id")+": установлен в: "+curTm.text());
	}
	curTm.setAttr("tm",max(0,timer-1));
}
rules = timersTree.save();',1377261462,'Запись по таймеру','Запис за таймером');
INSERT INTO "flb_web" VALUES('user','Users dispatcher','',10,'var treeEl = false;

//> Parse template
var pgTree = SYS.XMLNode();
var err = pgTree.load(tmpl,false,true);
var isAdmin = SYS.Security.access(user,4,"root","WebRoot",0440);
var curUser = user;

//> Check POST content for commands
if(cnts.length)
{
	var formEl = new Object();
	re = new RegExp("name=\"(.+)\"");
	//>> POST form elements process
	for(var i = 0; i < cnts.length; i++)
		formEl[re.exec(cnts[i].attr("Content-Disposition"))[1]] = cnts[i].text();
	//>> Check for apply
	if(!formEl["apply"].isEVal())
	{
		var applyOK = true;
		//>> Login: Check for correct form''s values
		if(!(userLogin=formEl["login"].match("[^\\s].+[^\\s]")).length)
		{
			treeEl = pgTree.getElementBy("loginNm","name");
			treeEl.childAdd("br");
			treeEl.childAdd("span").setAttr("style","color : red;").setText("(Пустое значение недопустимо)");
			userLogin = "";
			applyOK = false;
		}
		else userLogin = userLogin[0];
		if(userLogin.isEVal()) userLogin = user;
		pgTree.getElementBy("login","name").setAttr("value",userLogin);
		var isUserPres = (typeof(SYS.Security.nodeAt("usr_"+userLogin)) == "TCntrNode");
		var isWebUser = isUserPres && SYS.Security.access(userLogin,4,"root","Web",0440);
		var isWebRootUser = isUserPres && SYS.Security.access(userLogin,4,"root","WebRoot",0440);
		if(isUserPres && !(isWebUser||isWebRootUser)) applyOK = false;
		if(!isAdmin && userLogin != user) applyOK = false;

		//>> Name: Check for correct form''s values
		var userName = formEl["fio"].match("[^\\s].+[^\\s]");
		if(!userName.length)
		{
			treeEl = pgTree.getElementBy("fioNm","name");
			treeEl.childAdd("br");
			treeEl.childAdd("span").setAttr("style","color : red;").setText("(Пустое значение недопустимо)");
			userName = "";
			applyOK = false;
		}
		else userName = userName[0];
		pgTree.getElementBy("fio","name").setAttr("value",userName);

		//>> Password: Check for correct form''s values
		if((!isUserPres && formEl["pass"].length < 8) || (isUserPres && formEl["pass"].length && formEl["pass"].length < 8))
		{
			treeEl = pgTree.getElementBy("passNm","name");
			treeEl.childAdd("br");
			treeEl.childAdd("span").setAttr("style","color : red;").setText("(Пароль должен быть более 8 символов)");
			userName = "";
			applyOK = false;
		}

		if(isAdmin)
		{
			//>> Telepfons list, SMS send and report view
			pgTree.getElementBy("tel","name").setAttr("value",formEl["tel"]);
			if(formEl["sms"].length) pgTree.getElementBy("sms","name").setAttr("checked","checked");
			if(formEl["report"].length) pgTree.getElementBy("report","name").setAttr("checked","checked");

			if(!isWebRootUser)
			{
				//>> Subsystem''s permition
				var catLs = SYS.XMLNode("get").setAttr("path","/DAQ/ModBus/ZegBee/%2fprm%2fprm");
				SYS.cntrReq(catLs);
				treeEl = pgTree.getElementBy("sub_access");
				for(var i = 0; i < catLs.childSize(); i++)
				{
					var subId = "sub_"+catLs.childGet(i).attr("id");
					treeEl.childAdd("span").setText(catLs.childGet(i).text());
					treeEl.childAdd("br");
					treeEl1 = treeEl.childAdd("select").setAttr("name",subId);
					treeEl.childAdd("br");
					treeEl2 = treeEl1.childAdd("option").setAttr("value","--").setText("Нет доступа");
					if(formEl[subId] == "--") treeEl2.setAttr("selected","selected");
					treeEl2 = treeEl1.childAdd("option").setAttr("value","r-").setText("Наблюдение");
					if(formEl[subId] == "r-") treeEl2.setAttr("selected","selected");
					treeEl2 = treeEl1.childAdd("option").setAttr("value","rw").setText("Управление");
					if(formEl[subId] == "rw") treeEl2.setAttr("selected","selected");
				}
			}
			else
			{
				//> Session life time
				treeEl = pgTree.getElementBy("sub_access");
				treeEl.childAdd("span").setText("Время жизни сеанса аутентификации (минут)");
				treeEl.childAdd("br");
				treeEl.childAdd("input").setAttr("type","text").setAttr("name","sesAuth").setAttr("value",formEl["sesAuth"]);
			}
		}

		//> Add or update user
		if(applyOK)
		{
			//> Create new user if no present
			if(!isUserPres)
			{
				userReq = SYS.XMLNode("CntrReqs").setAttr("path","/sub_Security");
				userReq.childAdd("add").setAttr("path","%2fusgr%2fusers").setText(userLogin);
				userReq.childAdd("add").setAttr("path","/grp_Web/%2fprm%2fusers").setText(userLogin);
				SYS.cntrReq(userReq); 
			}
			//> Set user''s properties
			userReq = SYS.XMLNode("CntrReqs").setAttr("path","/sub_Security/usr_"+userLogin);
			userReq.childAdd("set").setAttr("path","%2fprm%2fdscr").setText(userName);
			if(formEl["pass"].length)
				userReq.childAdd("set").setAttr("path","%2fprm%2fpass").setText(formEl["pass"]);
			if(isAdmin)
			{
				var descrVal = "TEL: "+formEl["tel"]+"\n"+
							"SMS: "+(formEl["sms"].length?"true":"false")+"\n"+
							"Report: "+(formEl["report"].length?"true":"false")+"\n";
				if(!isWebRootUser)
				{
					//>> Subsystem''s permition
					for(var i = 0; i < catLs.childSize(); i++)
					{
						var subId = "sub_"+catLs.childGet(i).attr("id");
						descrVal += subId+": "+formEl[subId]+"\n";
					}
				}
				else
				{
					var sessLife = SYS.XMLNode("CntrReqs").setAttr("path","/sub_Protocol/mod_HTTP");
					sessLife.childAdd("set").setAttr("path","/%2fprm%2flf_tm").setText(max(1,formEl["sesAuth"].toInt()));
					sessLife.childAdd("save").setAttr("path","/%2fobj");
					SYS.cntrReq(sessLife);

				}
				userReq.childAdd("set").setAttr("path","%2fprm%2fldscr").setText(descrVal);
			}
			userReq.childAdd("save").setAttr("path","/%2fobj");
			SYS.cntrReq(userReq); 
			//>> Set message about correct apply
			treeEl=pgTree.getElementBy("mess","name");
			treeEl.childAdd("span").setAttr("style","color : green;").setText(
				(!isUserPres)?"Новый пользователь успешно добавлен. Можно продолжать.":"Пользователь успешно обновлен. Можно продолжать.");
		}
		//> Form error
		else
		{
			treeEl=pgTree.getElementBy("mess","name").childAdd("span").setAttr("style","color : red;");
			if(!isAdmin && userLogin != user) treeEl.setText("Обычный пользователь может редактировать только собственную учётную запись.");
			else if(isUserPres && !isWebUser) treeEl.setText("Пользователь присутствует, но не является Web-пользователем. Измените логин пользователя и повторите.");
			else treeEl.setText("Ошибка в формировании формы! Исправьте отмеченные поля и повторите.");
		}
	}
	else if(!formEl["edDel"].isEVal())
	{
		var isWebUser = SYS.Security.access(formEl["edLogin"],4,"root","Web",0440);
		var isWebRootUser = SYS.Security.access(formEl["edLogin"],4,"root","WebRoot",0440);
		if(isWebUser && !isWebRootUser)
		{
			userReq = SYS.XMLNode("CntrReqs").setAttr("path","/sub_Security");
			userReq.childAdd("del").setAttr("path","%2fusgr%2fusers").setText(formEl["edLogin"]);
			userReq.childAdd("del").setAttr("path","/grp_Web/%2fprm%2fusers").setText(formEl["edLogin"]);
			userReq.childAdd("save").setAttr("path","/%2fobj");
			SYS.cntrReq(userReq); 

			//>> Set message about correct remove
			treeEl=pgTree.getElementBy("mess","name");
			treeEl.childAdd("span").setAttr("style","color : green;").setText("Пользователь успешно удален. Можно продолжать.");
		}
		else
		{
			treeEl=pgTree.getElementBy("mess","name").childAdd("span").setAttr("style","color : red;");
			if(isWebRootUser) treeEl.setText("Удаление суперпользователя недопустимо!");
			else treeEl.setText("Пользователь для удаления отсутствует.");
		}
	}
}

//>> Load variables for select device
if(isAdmin)
{
	//> Load users list
	var userLst = SYS.XMLNode("get").setAttr("path","/sub_Security/grp_Web/%2fprm%2fusers");
	SYS.cntrReq(userLst);

	treeEl = pgTree.getElementBy("edLogin","name");
	treeEl.childAdd("OPTION").setAttr("value","").setText("");
	treeEl.childAdd("OPTION").setAttr("value","new").setText("Новый пользователь");
	for(var i = 0; i < userLst.childSize(); i++)
	{
		treeEl1 = SYS.XMLNode("get").setAttr("path","/sub_Security/usr_"+userLst.childGet(i).text()+"/%2fprm%2fdscr");
		SYS.cntrReq(treeEl1);
		treeEl.childAdd("OPTION").setAttr("value",userLst.childGet(i).text()).setText(treeEl1.text());
	}
}
else
{
	//> Hide select input and block login edit
	pgTree.getElementBy("edLogin","name").setAttr("style","display : none;");
	pgTree.getElementBy("login","name").setAttr("disabled","disabled");
	pgTree.getElementBy("tel").setAttr("style","visibility : hidden;");
	pgTree.getElementBy("sms").setAttr("style","visibility : hidden;");
	pgTree.getElementBy("report").setAttr("style","visibility : hidden;");
}

if(!cnts.length || !formEl["edDel"].isEVal())
{
	if(!urlPrms["selUser"].isEVal() && urlPrms["selUser"].length)
	{
		curUser = urlPrms["selUser"];
		pgTree.getElementBy(curUser,"value").setAttr("selected","selected");
	}
	var isSelAdmin = SYS.Security.access(curUser,4,"root","WebRoot",0440);

	//> Connect to user object
	//>> User login
	pgTree.getElementBy("login","name").setAttr("value",curUser);
	if(isAdmin && SYS.Security.access(curUser,4,"root","WebRoot",0440))
	{
		pgTree.getElementBy("login","name").setAttr("disabled","disabled");
		pgTree.getElementBy("edDel","name").setAttr("style","display : none;");
	}
	//>> User FN
	treeEl = SYS.XMLNode("get").setAttr("path","/sub_Security/usr_"+curUser+"/%2fprm%2fdscr");
	SYS.cntrReq(treeEl);
	pgTree.getElementBy("fio","name").setAttr("value",treeEl.text());
	if(isAdmin)
	{
		//>> Description
		treeEl = SYS.XMLNode("get").setAttr("path","/sub_Security/usr_"+curUser+"/%2fprm%2fldscr");
		SYS.cntrReq(treeEl);
		//>> Phones, SMS and repor
		re=treeEl.text().match("TEL:\\s+([^\\n]*)");
		pgTree.getElementBy("tel","name").setAttr("value",re.length ? re[1] : "");
		re=treeEl.text().match("SMS:\\s+([^\\n]*)");
		if(re.length && re[1] == "true") pgTree.getElementBy("sms","name").setAttr("checked","checked");
		re=treeEl.text().match("Report:\\s+([^\\n]*)");
		if(re.length && re[1] == "true") pgTree.getElementBy("report","name").setAttr("checked","checked");
		//>> Subsystem''s permition
		if(!isSelAdmin)
		{
			var catLs = SYS.XMLNode("get").setAttr("path","/DAQ/ModBus/ZegBee/%2fprm%2fprm");
			SYS.cntrReq(catLs);
			treeEl1 = pgTree.getElementBy("sub_access");
			for(var i = 0; i < catLs.childSize(); i++)
			{
				var subId = "sub_"+catLs.childGet(i).attr("id");
				re=treeEl.text().match(subId+":\\s+([^\\n]*)");
				treeEl1.childAdd("span").setText(catLs.childGet(i).text());
				treeEl1.childAdd("br");
				treeEl2 = treeEl1.childAdd("select").setAttr("name",subId);
				treeEl1.childAdd("br");
				treeEl3 = treeEl2.childAdd("option").setAttr("value","--").setText("Нет доступа");
				if(re.length && re[1] == "--") treeEl3.setAttr("selected","selected");
				treeEl3 = treeEl2.childAdd("option").setAttr("value","r-").setText("Наблюдение");
				if(re.length && re[1] == "r-") treeEl3.setAttr("selected","selected");
				treeEl3 = treeEl3.childAdd("option").setAttr("value","rw").setText("Управление");
				if(re.length && re[1] == "rw") treeEl3.setAttr("selected","selected");
			}
		}
		else
		{
			//> Session life time
			treeEl1 = pgTree.getElementBy("sub_access");
			treeEl1.childAdd("span").setText("Время жизни сеанса аутентификации (минут)");
			treeEl1.childAdd("br");
			var sessLife = SYS.XMLNode("get").setAttr("path","/sub_Protocol/mod_HTTP/%2fprm%2flf_tm");
			SYS.cntrReq(sessLife);
			treeEl1.childAdd("input").setAttr("type","text").setAttr("name","sesAuth").setAttr("value",sessLife.text());
		}
	}
}

//> Delete button hide
if(!isAdmin || cnts.length || urlPrms["selUser"].isEVal() || !urlPrms["selUser"].length)
	pgTree.getElementBy("edDel","name").setAttr("style","display : none;");

//Save template
rez = pgTree.save();
//SYS.messDebug("TEST Dev","TEST 00: "+typeof(devLs[formEl["cat"]+"."+itId]));',1377261537,'Диспетчер пользователей','Диспетчер користувачів');
CREATE TABLE 'WebUser_uPg' ("ID" TEXT DEFAULT '' ,"NAME" TEXT DEFAULT '' ,"uk#NAME" TEXT DEFAULT '' ,"DESCR" TEXT DEFAULT '' ,"uk#DESCR" TEXT DEFAULT '' ,"EN" INTEGER DEFAULT '0' ,"PROG" TEXT DEFAULT '' ,"uk#PROG" TEXT DEFAULT '' ,"TIMESTAMP" INTEGER DEFAULT '' ,"ru#DESCR" TEXT DEFAULT '' ,"ru#NAME" TEXT DEFAULT '' , PRIMARY KEY ("ID"));
INSERT INTO "WebUser_uPg" VALUES('tmpl','XHTML-template','XHTML-шаблон','Site from XHTML template.','Сайт із XHTML шаблону',1,'JavaLikeCalc.JavaScript
var pgDir = "Web/";
var firstEl = url.parsePath(0);
var procPg = false;
var tmplPg = "";
var tmplPrcPg = false;
var pgTree = false;
var treeEl = false;
var err = false;
var isUser = SYS.Security.access(user,4,"root","Web",0440);
var isRootUser = SYS.Security.access(user,4,"root","WebRoot",0440);

//> Pages requests process
if(!firstEl.length || (tmplPg=SYS.fileRead(pgDir+firstEl+".html")).length)
{
	//> Early call page''s script
	if(tmplPg.length)
	{
		procPg = SYS.DAQ.JavaLikeCalc.lib_web[(URLprms["script"].isEVal() ? firstEl : URLprms["script"])];
		if(typeof(procPg) == "TCntrNode")
		{
			tmplPrcPg = procPg.call(URLprms,tmplPg,cnts,user);
			//>> Check for full XML or resource request
			var poff = 0;
			if(tmplPrcPg.parse(0,"\n",poff) == "200 OK")
			{
				while((pEl=tmplPrcPg.parse(0,"\n",poff)).length) HTTPvars[pEl.parse(0,":")] = pEl.parse(1,":");
				page = tmplPrcPg.slice(poff);
				return "200 OK";
			}
		}
		else tmplPrcPg = tmplPg;
	}

	//> Parse template
	pgTree = SYS.XMLNode();
	err = pgTree.load(pgDir+"main.html",true,true);

	//> Page time place
	pgTree.getElementBy("time_vl").setAttr("value",SYS.time());
	var sessLife = SYS.XMLNode("get").setAttr("path","/sub_Protocol/mod_HTTP/%2fprm%2flf_tm");
	SYS.cntrReq(sessLife);
	pgTree.getElementBy("ses_vl").setAttr("value",sessLife.text().toInt()*60);

	//> Check access to WEB resource
	if(!user.length || !isUser)
	{
		pgTree.getElementBy("ses_vl").setAttr("value",10);
		pgTree.getElementBy("menu_l").setAttr("style","visibility : hidden;");
		pgTree.getElementBy("menu_r").setAttr("style","visibility : hidden;");
		pgTree.getElementBy("status").setAttr("style","visibility : hidden;");
		tmplPrcPg = SYS.fileRead(pgDir+"loginError.html");
	}
	//> Process include page template
	else if(tmplPg.length)
	{
		//>> Select page display
		treeEl = pgTree.getElementBy(firstEl);
		if(typeof(treeEl) == "XMLNode")
			treeEl.setAttr("class",treeEl.attr("class").replace(new RegExp("menu([^\"]*)"),"menuSel$1"));
	}
	else tmplPrcPg = SYS.fileRead(pgDir+"welcome.html");

	//> Status make
	pgTree.getElementBy("path").setText("/"+firstEl);
	pgTree.getElementBy("user_st").childIns(0,"span").setText(user+"("+(isRootUser?"суперпользователь":"пользователь")+")      ");

	//> Super user access
	if(isUser && !isRootUser)
	{
		pgTree.getElementBy("cfg").parent().setAttr("style","visibility : hidden;");
		pgTree.getElementBy("devices").parent().setAttr("style","visibility : hidden;");

		treeEl = SYS.XMLNode("get").setAttr("path","/sub_Security/usr_"+user+"/%2fprm%2fldscr");
		SYS.cntrReq(treeEl);
		//>> Report disable
		re=treeEl.text().match("Report:\\s+([^\\n]*)");
		if(!re.length || re[1] != "true") pgTree.getElementBy("report").parent().setAttr("style","visibility : hidden;");
		//>> Subsystem''s permition
		var catLs = SYS.XMLNode("get").setAttr("path","/DAQ/ModBus/ZegBee/%2fprm%2fprm");
		SYS.cntrReq(catLs);
		for(var i = 0; i < catLs.childSize(); i++)
		{
			re=treeEl.text().match("sub_"+catLs.childGet(i).attr("id")+":\\s+([^\\n]*)");
			if(!re.length || re[1] == "--") pgTree.getElementBy(catLs.childGet(i).attr("id")).parent().setAttr("style","visibility : hidden;");
		}
	}

	//> Save template
	page = "<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n"+
			"<!DOCTYPE html PUBLIC \"-//W3C//DTD XHTML 1.0 Transitional//EN\" \"http://www.w3.org/TR/xhtml1/DTD/xhtml1-transitional.dtd\">\n"+
			pgTree.save();
	page = page.replace("#####CONTEXT#####",tmplPrcPg);
	return "200 OK";
}

//> Resources get process
if(HTTPreq == "GET")
{
	page = SYS.fileRead(pgDir+url);
	if(!page.length) { page = "Resource ''"+url+"'' call error!"; return "404 Not Found"; }
	//>> Extesion types processing
	var fext = url.lastIndexOf(".");
	fext = (fext > 0) ? url.slice(fext+1) : "";
	if(fext == "png" || fext == "jpg" || fext == "ico") HTTPvars["Content-Type"] = "image/"+fext;
	else if(fext == "css" || fext == "html" || fext == "xml") HTTPvars["Content-Type"] = "text/"+fext;
	else if(fext == "js") HTTPvars["Content-Type"] = "text/javascript";
	//SYS.messDebug("TEST Web","TEST 10: ''"+fext+"'' .");
	return "200 OK";
}

return "404 Not Found";','',1377264714,'Сайт из XHTML шаблона.','XHTML-шаблон');
CREATE TABLE 'Trs' ("base" TEXT DEFAULT '' ,"uk#base" TEXT DEFAULT '' ,"ru#base" TEXT DEFAULT '' , PRIMARY KEY ("base"));
INSERT INTO "Trs" VALUES('No data or connection with source','Немає даних або зв''язку із джерелом','Нет данных или связи с источником');
INSERT INTO "Trs" VALUES('The signal exceed to upper hardware border','Сигнал перевищив верхню апаратну границю','Выход сигнала за верхнюю аппаратную границу');
INSERT INTO "Trs" VALUES('The signal exceed to bottom hardware border','Сигнал перевищив нижню апаратну границю','Выход сигнала за нижнюю аппаратную границу');
INSERT INTO "Trs" VALUES('Upper alarm border error','Помилка верхньої аварійної границі','Нарушение верхней аварийной границы');
INSERT INTO "Trs" VALUES('Lower alarm border error','Помилка нижньої аварійної границі','Нарушение нижней аварийной границы');
INSERT INTO "Trs" VALUES('Upper warning border error','Помилка верхньої поперджув. границі','Нарушение верхней предупредительной границы');
INSERT INTO "Trs" VALUES('Lower warning border error','Помилка нижньої поперджув. границі','Нарушение нижней предупредительной границы');
INSERT INTO "Trs" VALUES('Too big parameter''s motion speed','Дуже велика швидкість зміни параметру','Очень большая скорость изменения параметра');
INSERT INTO "Trs" VALUES('NORMA','НОРМА','НОРМА');
INSERT INTO "Trs" VALUES('Step missed','Крок пропущено','Шаг пропущен');
INSERT INTO "Trs" VALUES('Program terminated','Програму перервано','Программа прервана');
INSERT INTO "Trs" VALUES('Terminated by user session of the program','Перерваний користувачем сеанс програми','Прерванный пользователем сеанс программы');
INSERT INTO "Trs" VALUES('Terminated by error session of the program','Перерваний помилкою сеанс програми','Прерванный ошибкой сеанс программы');
INSERT INTO "Trs" VALUES('Successful session of the program','Вдалий сеанс програми','Успешный сеанс программы');
INSERT INTO "Trs" VALUES('Command miss: ','Відсутня команда: ','Отсутствует команда: ');
INSERT INTO "Trs" VALUES('Waiting %1s expired','Очікування %1с вичерпано','Ожидание %1c исчерпано');
INSERT INTO "Trs" VALUES('Waiting %1s','Очікування %1с','Ожидание %1c');
INSERT INTO "Trs" VALUES('Background waiting %1s','Фонове очікування %1с','Фоновое ожидание %1c');
INSERT INTO "Trs" VALUES('No current node present','Поточний вузол відсутній','Текущий узел отсутствует');
INSERT INTO "Trs" VALUES('No connection to source object','Немає з''єднання із об''єктом джерела','Нет подключения с объектом источника');
INSERT INTO "Trs" VALUES('Source error','Помилка джерела','Ошибка источника');
INSERT INTO "Trs" VALUES('Status','Статус','Статус');
INSERT INTO "Trs" VALUES('On battery','Від батареї','От батареи');
INSERT INTO "Trs" VALUES('Low battery','Батарею розряджено','Батарея разряжена');
INSERT INTO "Trs" VALUES('Shutdown load','Скид навантаження','Сброс нагрузки');
INSERT INTO "Trs" VALUES('ALARM','АВАРІЯ','АВАРИЯ');
INSERT INTO "Trs" VALUES('None good battery present','Відсутні хорощі батареї','Отсутствуют хорошие батареи');
INSERT INTO "Trs" VALUES('Battery charge low','Низький заряд батареї','Низкий заряд батареи');
INSERT INTO "Trs" VALUES('Battery charge critical','Критичний заряд батареї','Критический заряд батареи');
INSERT INTO "Trs" VALUES('Bad %1 batteries present','Зіпсованих батарей %1','Испорченных батарей %1');
INSERT INTO "Trs" VALUES('Input voltage low','Низька вхідна напруга','Низкое входное напряжение');
INSERT INTO "Trs" VALUES('Input voltage high','Висока вхідна напруга','Высокое входное напряжение');
INSERT INTO "Trs" VALUES('Input frequency too low','Дуже низька вхідна частота','Очень низкая входная частота');
INSERT INTO "Trs" VALUES('Input frequency too high','Дуже висока вхідна частота','Очень высокая входная частота');
INSERT INTO "Trs" VALUES('Input frequency low','Низька вхідна частота','Низкая входная частота');
INSERT INTO "Trs" VALUES('Input frequency high','Висока вхідна частота','Высокая входная частота');
INSERT INTO "Trs" VALUES('UPS overloaded','ДБЖ перевантажено','ИБП перегружено');
INSERT INTO "Trs" VALUES('UPS load high','Високе навантаження ДБЖ','Высокая нагрузка ИБП');
INSERT INTO "Trs" VALUES('UPS overheated','ДБЖ перегрітий','ИБП перегретый');
INSERT INTO "Trs" VALUES('Temperature high','Висока температура','Высокая температура');
INSERT INTO "Trs" VALUES('Levitation','Левітація','Левитация');
INSERT INTO "Trs" VALUES('No Levitation','Не левітація','Не левитация');
INSERT INTO "Trs" VALUES('Acceleration','Розгін','Разгон');
INSERT INTO "Trs" VALUES('Normal','Нормальний','Нормальный');
INSERT INTO "Trs" VALUES('Deceleration (Brake)','Гальмування','Торможение');
INSERT INTO "Trs" VALUES('Autotest','Автотест','Автотест');
INSERT INTO "Trs" VALUES('Tuning','Підлаштування','Подстройка');
INSERT INTO "Trs" VALUES('Tuning Complete','Підлаштування завершене','Подстройка завершена');
INSERT INTO "Trs" VALUES('Local','Локально','Локально');
INSERT INTO "Trs" VALUES('Remote','Віддалено','Удалённо');
INSERT INTO "Trs" VALUES('Start session.','Запуск сеансу.','Запуск сеанса.');
INSERT INTO "Trs" VALUES('Request for values present list and size.','Запит наявних значень та розмірів','Запрос доступных значений и размеров');
INSERT INTO "Trs" VALUES('Readed items list write.','Запис переліку элементів для читання.','Запись списка элементов для чтения.');
INSERT INTO "Trs" VALUES('Write to hour archive for time','Запис до архіву годин на час','Запись в архив часов за время');
INSERT INTO "Trs" VALUES('Write to day archive for time','Запис до архіву днів на час','Запись в архив дней за время');
INSERT INTO "Trs" VALUES('Write to result month archive for time','Запис до результуючого архіву місяців на час','Запись в результирующий архив месяцев за время');
INSERT INTO "Trs" VALUES('Error response','Помилка відповіді','Ошибка ответа');
INSERT INTO "Trs" VALUES('Quality errors','Помилки якості','Ошибки качетсва');
INSERT INTO "Trs" VALUES('No a connection','Нема підключення','Нет подключения');
INSERT INTO "Trs" VALUES('Data list for request empty.','Перелік даних запиту порожній.','Список данных запроса пустой.');
INSERT INTO "Trs" VALUES('Unknown','Невідомий','Неизвестный');
INSERT INTO "Trs" VALUES('Deceleration','Гальмування','Торможение');
INSERT INTO "Trs" VALUES('Stop operation failure','Невдача операції зупинки','Неудача операции останова');
INSERT INTO "Trs" VALUES('Idling failure','Невдача простою','Неудача простоя');
INSERT INTO "Trs" VALUES('Regeneration breaking failure','Збій регенерації гальмування','Сбой регенерации торможения');
INSERT INTO "Trs" VALUES('Deceleration failure','Помилка гальмування','Ошибка торможения');
INSERT INTO "Trs" VALUES('No connect','Не підключено','Не подключено');
INSERT INTO "Trs" VALUES('Error or no response.','Помилка або немає відповіді','Ошибка или нет ответа');
INSERT INTO "Trs" VALUES('CRC error.','Помилка CRC.','Ошибка CRC.');
INSERT INTO "Trs" VALUES('DCON error.','Помилка DCON.','Ошибка DCON.');
INSERT INTO "Trs" VALUES('No respond','Немає відповіді','Нет ответа');
INSERT INTO "Trs" VALUES('No acknowledgment','Немає підтвердження','Нет подтверждения');
INSERT INTO "Trs" VALUES('Data block short or error','Блок даних короткий або помилка','Блок данных короткий или ошибка');
INSERT INTO "Trs" VALUES('Block sequence.','Блокувальна послідовність.','Блокирующая последовательность.');
INSERT INTO "Trs" VALUES('Unknown block end.','Кінець невідомого блоку.','Конец неизвестного блока.');
INSERT INTO "Trs" VALUES('Not full respond','Неповна відповідь','Неполный ответ');
INSERT INTO "Trs" VALUES('Data range error.','Помилка діапазону даних','Ошибка диапазона данных');
INSERT INTO "Trs" VALUES('Unknown error.','Невідома помилка.','Неизвестная ошибка.');
INSERT INTO "Trs" VALUES('Error set PIN-code.','Помилка встановлення PIN-коду.','Ошибка установки PIN-кода.');
INSERT INTO "Trs" VALUES('Error set PDU mode.','Помилка встановлення режиму PDU.','Ошибка установки режима PDU.');
INSERT INTO "Trs" VALUES('Error sent SMS.','Помилка надсилання SMS.','Ошибка отправки SMS.');
INSERT INTO "Trs" VALUES('Error sent SMS PDU','Помилка надсилання SMS PDU','Ошибка отправки SMS PDU');
INSERT INTO "Trs" VALUES('Device address out of range 1...254','Адреса пристрою за межою 1...254','Адрес устройства за границей 1...254');
INSERT INTO "Trs" VALUES('No or error respond','Немає або помилка відповіді','Нет или ошибка ответа');
INSERT INTO "Trs" VALUES('Device address out of range 1...32','Адреса пристрою за межою 1...32','Адрес устройства за границей 1...32');
INSERT INTO "Trs" VALUES('Device address out of range 0...254','Адреса пристрою за межою 0...254','Адрес устройства за границей 0...254');
INSERT INTO "Trs" VALUES('Device address out of range 0...2047','Адреса пристрою за межою 0...2047','Адрес устройства за границей 0...2047');
INSERT INTO "Trs" VALUES('No or short respond','Немає або коротка відповідь','Нет или короткий ответ');
INSERT INTO "Trs" VALUES('Device address out of range 0...255','Адреса пристрою за межою 0...255','Адрес устройства за границей 0...255');
INSERT INTO "Trs" VALUES('No a respond','Немає відповіді','Нет ответа');
INSERT INTO "Trs" VALUES('CRC error','Помилка CRC','Ошибка CRC');
INSERT INTO "Trs" VALUES('Signal exceed to upper hardware border','Вихід сигналу за нижню апаратну границю','Выход сигнала за нижнюю аппаратную границу');
INSERT INTO "Trs" VALUES('Signal exceed to bottom hardware border','Вихід сигналу за верхню апаратну границю','Выход сигнала за верхнюю аппаратную границу');
INSERT INTO "Trs" VALUES('Request','Запит','Запрос');
INSERT INTO "Trs" VALUES('Header error','Помилка заголовку','Ошибка заголовка');
INSERT INTO "Trs" VALUES('Header CRC error','Помилка контрольної суми заголовку','Ошибка контрольной суммы заголовка');
INSERT INTO "Trs" VALUES('Info block size error','Помилка розміру інформаційного блоку','Ошибка размера информационного блока');
INSERT INTO "Trs" VALUES('Info block CRC error.','Помилка контрольної суми інформаційного блоку','Ошибка контрольной суммы информационного блока');
INSERT INTO "Trs" VALUES('Request error','Помилка запиту','Ошибка запроса');
INSERT INTO "Trs" VALUES('Respond too short or wrong','Відповідь дуже коротка або помилкова','Ответ очень короткий или ошибочный');
INSERT INTO "Trs" VALUES('Wrong or no a respond.','Помилка або немає відповіді.','Ошибка или нет ответа.');
INSERT INTO "Trs" VALUES('Message''s length more 255*255','Розмір повідомлення більш за 255*255','Размер сообщения больше чем 255*255');
INSERT INTO "Trs" VALUES('No data block get','Не отримано блоку даних','Не получено блока данных');
INSERT INTO "Trs" VALUES('LRC error.','Помилка LRC.','Ошибка LRC.');
INSERT INTO "Trs" VALUES('Message empty','Повідомлення порожне','Ответ пуст');
INSERT INTO "Trs" VALUES('Request:','Запит:','Запрос:');
INSERT INTO "Trs" VALUES('Wrong respond','Помилкова відповідь','Ошибочный ответ');
INSERT INTO "Trs" VALUES('Respond:','Відповідь:','Ответ:');
INSERT INTO "Trs" VALUES('Illegal command (CMD code not valid).','','');
INSERT INTO "Trs" VALUES('Syntax error. (Too many bytes in data field, not enough bytes, etc).','','');
INSERT INTO "Trs" VALUES('Inhibited.','','');
INSERT INTO "Trs" VALUES('Obsolete command. No action taken, but not really an error.','','');
INSERT INTO "Trs" VALUES('Telephone number error.','','');
INSERT INTO "Trs" VALUES('Long length (%1) of the message.','','');
INSERT INTO "Trs" VALUES('KS error.','','');
INSERT INTO "Trs" VALUES('KS error','','');
INSERT INTO "Trs" VALUES('Request error.','','');
INSERT INTO "Trs" VALUES('Serial output transport ''%1'' error.','','');
INSERT INTO "Trs" VALUES('Request: TRANSL_TEMP.','','');
INSERT INTO "Trs" VALUES('No connection','','');
INSERT INTO "Trs" VALUES('Output transport ''%1'' error.','','');
INSERT INTO "Trs" VALUES('Read all data.','','');
INSERT INTO "Trs" VALUES('Reply is not full','Відповідь не повна','Ответ не полный');
INSERT INTO "Trs" VALUES('No Problems. The rest below are prioritized.','','');
INSERT INTO "Trs" VALUES('Frequency of xtal is > Fq or < Fm. Halts all other calculations.','','');
INSERT INTO "Trs" VALUES('Frequency was bad (previous XtalStat was XPROB_FREQ) and frequency is now in range, but life is less than 3%.','','');
INSERT INTO "Trs" VALUES('Stability level XtalStab of xtal >= SlvlTrip.','','');
INSERT INTO "Trs" VALUES('Unable to determine rate because of a computation error, most likely caused by a parameter (density, z-ratio, etc) having a zero or otherwise invalid (non numeric or infinity) value.','','');
INSERT INTO "Trs" VALUES('XtalQual of xtal >= QlvlTrip.','','');
INSERT INTO "Trs" VALUES('Transport ''%1'' error.','','');
INSERT INTO "Trs" VALUES('Request for service info.','','');
INSERT INTO "Trs" VALUES('Request for counter time and hour archive begin.','','');
INSERT INTO "Trs" VALUES('Request for dimensions and precisions.','','');
INSERT INTO "Trs" VALUES('Request for curent values.','','');
INSERT INTO "Trs" VALUES('Request for hour archive values.','','');
INSERT INTO "Trs" VALUES('Request for set archive data.','','');
INSERT INTO "Trs" VALUES('Request for day archive values.','','');
INSERT INTO "Trs" VALUES('Request for results month archive values.','','');
INSERT INTO "Trs" VALUES('Request for data read.','','');
INSERT INTO "Trs" VALUES('Set current','','');
INSERT INTO "Trs" VALUES('Device address out of range 0...15','','');
INSERT INTO "Trs" VALUES('Respond too short','','');
INSERT INTO "Trs" VALUES('Respond too long, possible continuous mode','','');
INSERT INTO "Trs" VALUES('RS232 synchronization error. ','','');
INSERT INTO "Trs" VALUES('Incorrect command, e.g. inadmissible address (syntax error). ','','');
INSERT INTO "Trs" VALUES('Inadmissible read command. ','','');
INSERT INTO "Trs" VALUES('SP1 status. ','','');
INSERT INTO "Trs" VALUES('SP2 status. ','','');
INSERT INTO "Trs" VALUES('Atm. pressure out of range. ','','');
INSERT INTO "Trs" VALUES('Temperature out of range. ','','');
INSERT INTO "Trs" VALUES('Cal. mode wrong. ','','');
INSERT INTO "Trs" VALUES('Pressure underflow. ','','');
INSERT INTO "Trs" VALUES('Pressure overflow. ','','');
INSERT INTO "Trs" VALUES('Zero adjust warning. ','','');
INSERT INTO "Trs" VALUES('PT1000 fault (CTR 101 only). ','','');
INSERT INTO "Trs" VALUES('Heaterblock overtemp. ','','');
INSERT INTO "Trs" VALUES('Electronic overtemp. ','','');
INSERT INTO "Trs" VALUES('Zero adjust error. ','','');
INSERT INTO "Trs" VALUES('Output transport ''%1'' stoped.','','');
INSERT INTO "Trs" VALUES('Wrong sequence, rejected','','');
INSERT INTO "Trs" VALUES('Connection terminated by the input sequence broken, lost input packages, S(R)=%1, S(T)=%2','','');
INSERT INTO "Trs" VALUES('Connection terminated by not acknowledge transmited package ''%1'' by timeout t1 (%2).','','');
INSERT INTO "Trs" VALUES('Connection terminated by not confirmed TEST package by timeout t1 (%1).','','');
INSERT INTO "Trs" VALUES('Error','','');
INSERT INTO "Trs" VALUES('Good output sequence','','');
INSERT INTO "Trs" VALUES('Good input sequence','','');
INSERT INTO "Trs" VALUES('M_SP_NA_1: No the SQ mode implemented yet','','');
INSERT INTO "Trs" VALUES('M_SP_NA_1: Items number discrepancy to the package size','','');
INSERT INTO "Trs" VALUES('M_ME_NB_1: No the SQ mode implemented yet','','');
INSERT INTO "Trs" VALUES('M_ME_NB_1: Items number discrepancy to the package size','','');
INSERT INTO "Trs" VALUES('C_SC_NA_1: No the SQ mode implemented yet','','');
INSERT INTO "Trs" VALUES('C_SC_NA_1: Items number discrepancy to the package size','','');
INSERT INTO "Trs" VALUES('Device address ''%1'' out of range [0...119].','','');
INSERT INTO "Trs" VALUES('No read result.','','');
INSERT INTO "Trs" VALUES('No a device present on the bus ''%1''.','','');
INSERT INTO "Trs" VALUES('1W device','','');
INSERT INTO "Trs" VALUES('Temperature','','');
INSERT INTO "Trs" VALUES('User cell [0...65535]','','');
INSERT INTO "Trs" VALUES('Resolution [9...12]','','');
INSERT INTO "Trs" VALUES('1:Device missed','','');
INSERT INTO "Trs" VALUES('3:No a response or the response short.','','');
INSERT INTO "Trs" VALUES('3:Incomplete response.','','');
INSERT INTO "Trs" VALUES('4:Request error.','','');
INSERT INTO "Trs" VALUES('Address ''%1'' out of range [0...255].','','');
INSERT INTO "Trs" VALUES('Response','','');
INSERT INTO "Trs" VALUES('Appending variable','','');
INSERT INTO "Trs" VALUES('4:Unknown function.','','');
INSERT INTO "Trs" VALUES('4:Insufficient rights.','','');
INSERT INTO "Trs" VALUES('4:Invalid function received.','','');
INSERT INTO "Trs" VALUES('5:Data error...','','');
INSERT INTO "Trs" VALUES('4:Invalid host''s address received.','','');
INSERT INTO "Trs" VALUES('5:Operation error.','','');
INSERT INTO "Trs" VALUES('Wrong login.','','');
INSERT INTO "Trs" VALUES('No such variable.','','');
INSERT INTO "Trs" VALUES('Unknown error %1.','','');
INSERT INTO "Trs" VALUES('Destination:','','');
INSERT INTO "Trs" VALUES('DI0','','');
INSERT INTO "Trs" VALUES('DI1','','');
INSERT INTO "Trs" VALUES('DO0','','');
INSERT INTO "Trs" VALUES('DO1','','');
INSERT INTO "Trs" VALUES('DS2480 is not detected.','','');
INSERT INTO "Trs" VALUES('Size mismatch.','','');
INSERT INTO "Trs" VALUES('day','','');
INSERT INTO "Trs" VALUES('hour','','');
INSERT INTO "Trs" VALUES('min','','');
INSERT INTO "Trs" VALUES('s','','');
INSERT INTO "Trs" VALUES('ms','','');
INSERT INTO "Trs" VALUES('us','','');
INSERT INTO "Trs" VALUES('ns','','');
INSERT INTO "Trs" VALUES('Allowed variables','','');
INSERT INTO "Trs" VALUES('No data','','');
INSERT INTO "Trs" VALUES('No a data','','');
INSERT INTO "Trs" VALUES('Disable ECHO error: %1','','');
INSERT INTO "Trs" VALUES('Check for PIN-code error: %1','','');
INSERT INTO "Trs" VALUES('Set PIN error: %1','','');
INSERT INTO "Trs" VALUES('Set SMS Text mode error: %1','','');
INSERT INTO "Trs" VALUES('Sent SMS error: %1','','');
INSERT INTO "Trs" VALUES('Sent SMS PDU error: %1','','');
INSERT INTO "Trs" VALUES('Set SMS PDU mode error: %1','','');
INSERT INTO "Trs" VALUES('Impossible connect to the SMTP-server.','','');
INSERT INTO "Trs" VALUES('No a response.','','');
INSERT INTO "Trs" VALUES('HELLO error: %1.','','');
INSERT INTO "Trs" VALUES('Unsupported auth methods: %1.','','');
INSERT INTO "Trs" VALUES('Auth required: %1.','','');
INSERT INTO "Trs" VALUES('AUTH error: %1.','','');
INSERT INTO "Trs" VALUES('AUTH USER error: %1.','','');
INSERT INTO "Trs" VALUES('AUTH PASS error: %1.','','');
INSERT INTO "Trs" VALUES('MAIL FROM error: %1.','','');
INSERT INTO "Trs" VALUES('RCPT TO error: %1.','','');
INSERT INTO "Trs" VALUES('DATA error: %1.','','');
INSERT INTO "Trs" VALUES('DATA send error: %1.','','');
INSERT INTO "Trs" VALUES('Alarm','','');
INSERT INTO "Trs" VALUES('Norm','','');
INSERT INTO "Trs" VALUES('Sent %1. In queue %2.','','');
INSERT INTO "Trs" VALUES('Error: %1.','','');
INSERT INTO "Trs" VALUES('Wrong or empty respond to the calibration request.','','');
INSERT INTO "Trs" VALUES('Wrong or empty respond to the temperature data.','','');
INSERT INTO "Trs" VALUES('Wrong or empty read respond.','','');
INSERT INTO "Trs" VALUES('Device address out of range 0...119.','','');
INSERT INTO "Trs" VALUES('Memory offset out of range 0...8191.','','');
INSERT INTO "Trs" VALUES('Readed memory size out of range 0...8191.','','');
INSERT INTO "Trs" VALUES('Not or short respond %1 from %2.','','');
INSERT INTO "Trs" VALUES('Too long write sequence.','','');
INSERT INTO "Trs" VALUES('DI','','');
INSERT INTO "Trs" VALUES('DO','','');
INSERT INTO "Trs" VALUES('Resolution','','');
INSERT INTO "Trs" VALUES('AI','','');
INSERT INTO "Trs" VALUES('Mode','','');
INSERT INTO "Trs" VALUES('Voltage','','');
INSERT INTO "Trs" VALUES('Current','','');
INSERT INTO "Trs" VALUES('Temperature, °С','','');
INSERT INTO "Trs" VALUES('AI, V','','');
INSERT INTO "Trs" VALUES('Voltage, V','','');
INSERT INTO "Trs" VALUES('Current, A','','');
INSERT INTO "Trs" VALUES('Current A/D','','');
INSERT INTO "Trs" VALUES('Current Accumulator','','');
INSERT INTO "Trs" VALUES('Current Accumulator Shadow','','');
INSERT INTO "Trs" VALUES('Voltage A/D (0-VAD,1-VDD)','','');
INSERT INTO "Trs" VALUES('Accumulation threshold','','');
INSERT INTO "Trs" VALUES('Elapsed time meter','','');
INSERT INTO "Trs" VALUES('Current Offset','','');
INSERT INTO "Trs" VALUES('Enable Current A/D','','');
INSERT INTO "Trs" VALUES('Enable Current Accumulator','','');
INSERT INTO "Trs" VALUES('Enable Current Accumulator Shadow','','');
INSERT INTO "Trs" VALUES('Disconnect time','','');
INSERT INTO "Trs" VALUES('End of charge time','','');
INSERT INTO "Trs" VALUES('No link to external functions on ''%1''.','','');
INSERT INTO "Trs" VALUES('Unknown device %1 [0-DHT11, 1-DHT22].','','');
INSERT INTO "Trs" VALUES('Tries number %1 out of range [1...5].','','');
INSERT INTO "Trs" VALUES('Get data after %1 tries error.','','');
INSERT INTO "Trs" VALUES('None of good battery present','','');
INSERT INTO "Trs" VALUES('Error for ''%1'' as an output transport of I2C or a link to external functions of GPIO.','','');
INSERT INTO "Trs" VALUES('Empty','Порожньо','Пусто');
INSERT INTO "Trs" VALUES('Wrong or empty respond to the calibration T1-3 or P1-9 request.','','');
INSERT INTO "Trs" VALUES('Wrong or empty respond to the calibration H1 request.','','');
INSERT INTO "Trs" VALUES('Wrong or empty respond to the calibration H2-H6 request.','','');
INSERT INTO "Trs" VALUES('Wrong or empty respond to the pressure data.','','');
INSERT INTO "Trs" VALUES('Wrong or empty respond to the humidity data.','','');
CREATE TABLE 'tmplib_DevLib' ("ID" TEXT DEFAULT '' ,"NAME" TEXT DEFAULT '' ,"uk#NAME" TEXT DEFAULT '' ,"ru#NAME" TEXT DEFAULT '' ,"DESCR" TEXT DEFAULT '' ,"uk#DESCR" TEXT DEFAULT '' ,"ru#DESCR" TEXT DEFAULT '' ,"MAXCALCTM" INTEGER DEFAULT '10' ,"PR_TR" INTEGER DEFAULT '1' ,"PROGRAM" TEXT DEFAULT '' ,"uk#PROGRAM" TEXT DEFAULT '' ,"ru#PROGRAM" TEXT DEFAULT '' ,"TIMESTAMP" INTEGER DEFAULT '' , PRIMARY KEY ("ID"));
INSERT INTO "tmplib_DevLib" VALUES('SCU750','EDWARDS TURBOMOLECULAR PUMPS','','','Typical EDWARDS TURBOMOLECULAR PUMPS (http://edwardsvacuum.com) data request by SCU750 Cotrol Unit protocol.
Author: Roman Savochenko <rom_as@oscada.org>
Sponsored: Vasiliy Grigoriev from "Vacuum technologies laboratory (http://e-beam.ru)".
Version: 1.0.0','','',10,0,'JavaLikeCalc.JavaScript
if(f_start) {
	version = "";
	operModes = new Object();
	operModes[1] = tr("Levitation");
	operModes[2] = tr("No Levitation");
	operModes[3] = tr("Acceleration");
	operModes[4] = tr("Normal");
	operModes[5] = tr("Deceleration (Brake)");
	operModes[6] = tr("Autotest");
	operModes[7] = tr("Tuning");
	operModes[8] = tr("Tuning Complete");
}

tr = SYS.Transport.Serial.nodeAt("out_"+transport);
req = SYS.XMLNode("mess").setAttr("ProtIt","SCU750").setAttr("addr",addr);

//======= Commands =========
//START, STOP and RESET process
if(comReset == true) req.setText(" E04");
else if(comStop == true) req.setText(" E02");
else if(comStart == true) req.setText(" E01");
else req.setText("");
if(req.text().length) { tr.messIO(req,"UserProtocol"); comReset=comStop=comStart=false; }

//Speed setpoint set
if(spSpeed != spSpeedPrev) {
	req.setText(" h"+spSpeed.toString(16,4));
	tr.messIO(req,"UserProtocol");
	spSpeedPrev = spSpeed;
}

//======= Data getting (by self bigger period) ========
if((cnt++)%max(1,perGet*f_frq))	return;
t_err = "";

//Read version, single
if(!version.length || version.isEVal()) {
	req.setText("?V");
	tr.messIO(req,"UserProtocol");
	if(req.attr("err").length) { t_err = req.attr("err"); version = EVAL_STR; }
	else {
		if(req.text().length != 42) version = EVAL_STR;
		else {
			CUSv = "";
			lstNoSp = 0;
			for(i_s = 2; i_s < 32; i_s+=2) {
				cod = req.text().slice(i_s,i_s+2).toInt(16);
				CUSv += SYS.strFromCharCode(cod);
				if(cod != 0x20) lstNoSp = i_s/2;
			}
			version = "Contr. Unit Soft V.="+CUSv.slice(0,lstNoSp)+"; "
					    "Motor Drive Soft V.="+req.text().charAt(35)+"."+req.text().charAt(36)+"; "
					    "DSP Soft V.="+req.text().charAt(39)+"."+req.text().charAt(40);
		}
	}
}

//Read rotational speed.
if(t_err.length) tTMS = tMotor = rotSpeed = EVAL_INT;
else {
	req.setText("?[");
	tr.messIO(req,"UserProtocol");
	if(req.attr("err").length) { t_err = req.attr("err"); tTMS = tMotor = rotSpeed = EVAL_INT; }
	else {
		tTMS = req.text().slice(32,36).toInt(16);
		tMotor = req.text().slice(36,40).toInt(16);
		rotSpeed = req.text().slice(50,54).toInt(16);
	}
}

//Pump operation mode and errors read
if(t_err.length) { operMode = EVAL_INT; errors = EVAL_STR; }
else {
	req.setText("?M");
	tr.messIO(req,"UserProtocol");
	if(req.attr("err").length) { t_err = req.attr("err"); operMode = EVAL_INT; errors = EVAL_STR; }
	else {
		oMode = req.text().slice(2,4).toInt(16);
		operMode = oMode.toString()+":"+(operModes[oMode].isEVal()?tr("Unknown"):operModes[oMode]);
		//SYS.messDebug("CNTR","pumpOperMode: "+pumpOperMode);
		
		errCnt = req.text().slice(4,6).toInt(16);
		errors = "";
		for(i_e = 0; i_e < errCnt; i_e++)
			errors += req.text().slice(6+i_e*2,8+i_e*2)+"; ";
	}
}

//Events read
if(t_err.length)	events = EVAL_STR;
else {
	req.setText("?g");
	tr.messIO(req,"UserProtocol");
	if(req.attr("err").length) { t_err = req.attr("err"); events = EVAL_STR; }
	else {
		errCnt = req.text().slice(2,4).toInt(16);
		events = "";
		for(i_e = 0; i_e < errCnt; i_e++)
			events += req.text().slice(4+i_e*2,6+i_e*2)+"; ";
	}
}

//Counters
if(t_err.length) { snCntrUnit = snPump = EVAL_STR; cntPumpHour = cntCntrUnitHour = cntStart = EVAL_REAL; }
else {
	req.setText("?c");
	tr.messIO(req,"UserProtocol");
	if(req.attr("err").length) { t_err = req.attr("err"); snCntrUnit = snPump = EVAL_STR; cntPumpHour = cntCntrUnitHour = cntStart = EVAL_REAL; }
	else {
		snCntrUnit = req.text().slice(2,12);
		snPump = req.text().slice(12,22);
		cntPumpHour = req.text().slice(22,30).toInt(16);
		cntCntrUnitHour = req.text().slice(30,38).toInt(16);
		cntStart = req.text().slice(38,46).toInt(16);
	}
}

//Read setpoints
if(t_err.length)	spSpeed = spSpeedPrev = spTMSt = EVAL_INT;
else {
	req.setText("?d");
	tr.messIO(req,"UserProtocol");
	if(req.attr("err").length) { t_err = req.attr("err"); spSpeed = spSpeedPrev = spTMSt = EVAL_INT; }
	else {
		spSpeed = spSpeedPrev = req.text().slice(2,6).toInt(16);
		spTMSt = req.text().slice(6,10).toInt(16);
	}
}

f_err = t_err;','','',1509274008);
INSERT INTO "tmplib_DevLib" VALUES('TMH','Power supply for turbomolecular pumps','','','Power supply for turbomolecular pumps of firm SHIMADZU (http://www.shimadzu.com), model EI-R04M.
Author: Roman Savochenko <rom_as@oscada.org>
Sponsored: Vasiliy Grigoriev from "Vacuum technologies laboratory (http://e-beam.ru)".
Version: 1.0.0','','',10,0,'JavaLikeCalc.JavaScript
tr = SYS.Transport.Serial.nodeAt("out_"+transport);
req = SYS.XMLNode("mess").setAttr("ProtIt","TMH").setAttr("addr",addr);

if(f_start) {
	modelID = "";
	operModes = new Object();
	//Modes
	operModes["NS"] = "1:"+tr("Levitation");
	operModes["NA"] = "3:"+tr("Acceleration");
	operModes["NN"] = "4:"+tr("Normal");
	operModes["NB"] = "5:"+tr("Deceleration");
	operModes["FS"] = "6:"+tr("Stop operation failure");
	operModes["FF"] = "7:"+tr("Idling failure");
	operModes["FR"] = "8:"+tr("Regeneration breaking failure");
	operModes["FB"] = "9:"+tr("Deceleration failure");
	//Conrols
	operModes["LL"] = tr("Local");
	operModes["LR"] = tr("Remote");
	operModes["LC"] = "RS-232C";
	operModes["LD"] = "RS-485";

	//Set to Interactive
	req.setText("LN");
	tr.messIO(req,"UserProtocol");
}

//======= Commands =========
//START, STOP and RESET process
if(comReset == true)			req.setText("RR");
else if(comStop == true)		req.setText("RP");
else if(comStart == true)	req.setText("RT");
else req.setText("");
if(req.text().length)	{ tr.messIO(req,"UserProtocol"); comReset = comStop = comStart = false; }

//Interactive, Autonomous
if(comInteract == true)		req.setText("LN");
else if(comAutonom == true)	req.setText("LF");
else req.setText("");
if(req.text().length)	{ tr.messIO(req,"UserProtocol"); comInteract = comAutonom = false; }

//======= Data getting (by self bigger period) ========
if((cnt++)%max(1,perGet*f_frq))	return;
t_err = "0";

//Oper mode
req.setText("CS");
tr.messIO(req,"UserProtocol");
if(req.attr("err").length) { t_err = req.attr("err"); operMode = EVAL_STR; }
else operMode = /*req.text().slice(2)+":"+*/operModes[req.text().slice(0,2)];

//Oper control
if(t_err.toInt())	operCntr = EVAL_STR;
else {
	req.setText("LS");
	tr.messIO(req,"UserProtocol");
	if(req.attr("err").length) { t_err = req.attr("err"); operCntr = EVAL_STR; }
	else operCntr = operModes[req.text().slice(0,2)];
}

//Alarms list obtain
if(t_err.toInt())	errors = EVAL_STR;
else {
	errors = "";
	for(i = 0; true; i++) {
		req.setText("CF"+i.toString(10,2));
		tr.messIO(req,"UserProtocol");
		if(req.attr("err").length) { t_err = req.attr("err"); errors = EVAL_STR; break; }
		//SYS.messDebug("TMH","Result: "+req.text());
		if(req.text().slice(0,2) != "CA")	break;
		errors += req.text().slice(2)+";";
		continue;
	}
}

//Get parameters
// Model ID
if(t_err.toInt())	modelID = EVAL_STR;
else if(!modelID.length || modelID == EVAL_STR) {
	req.setText("PR01");
	tr.messIO(req,"UserProtocol");
	if(req.attr("err").length) { t_err = req.attr("err"); modelID = EVAL_STR;}
	else if(req.text().slice(0,2) != "PA") modelID = EVAL_STR;
	else modelID = "TMP-"+req.text().slice(4);
}

// Rotation speed
if(t_err.toInt())	rotSpeed = EVAL_INT;
else {
	req.setText("PR03");
	tr.messIO(req,"UserProtocol");
	if(req.attr("err").length) { t_err = req.attr("err"); rotSpeed = EVAL_INT;}
	else if(req.text().slice(0,2) != "PA") rotSpeed = EVAL_INT;
	else rotSpeed = req.text().slice(4).toInt()*10;
}

// Current
if(t_err.toInt())	I = EVAL_REAL;
else {
	req.setText("PR04");
	tr.messIO(req,"UserProtocol");
	if(req.attr("err").length) { t_err = req.attr("err"); I = EVAL_REAL;}
	else if(req.text().slice(0,2) != "PA") I = EVAL_REAL;
	else I = req.text().slice(4).toInt()/10;
}

// Axle 1 disbalance
if(t_err.toInt())	axle1disb = EVAL_INT;
else {
	req.setText("PR21");
	tr.messIO(req,"UserProtocol");
	if(req.attr("err").length) { t_err = req.attr("err"); axle1disb = EVAL_INT;}
	else if(req.text().slice(0,2) != "PA") axle1disb = EVAL_INT;
	else axle1disb = req.text().slice(4).toInt();
}

// Axle 2 disbalance
if(t_err.toInt())	axle2disb = EVAL_INT;
else {
	req.setText("PR22");
	tr.messIO(req,"UserProtocol");
	if(req.attr("err").length) { t_err = req.attr("err"); axle2disb = EVAL_INT;}
	else if(req.text().slice(0,2) != "PA") axle2disb = EVAL_INT;
	else axle2disb = req.text().slice(4).toInt();
}

// MP X1
if(t_err.toInt())	MP_X1 = EVAL_INT;
else {
	req.setText("PR26");
	tr.messIO(req,"UserProtocol");
	if(req.attr("err").length) { t_err = req.attr("err"); MP_X1 = EVAL_INT;}
	else if(req.text().slice(0,2) != "PA") MP_X1 = EVAL_INT;
	else MP_X1 = req.text().slice(4).toInt();
}

// MP Y1
if(t_err.toInt())	MP_Y1 = EVAL_INT;
else {
	req.setText("PR27");
	tr.messIO(req,"UserProtocol");
	if(req.attr("err").length) { t_err = req.attr("err"); MP_Y1 = EVAL_INT;}
	else if(req.text().slice(0,2) != "PA") MP_Y1 = EVAL_INT;
	else MP_Y1 = req.text().slice(4).toInt();
}

// MP X2
if(t_err.toInt())	MP_X2 = EVAL_INT;
else {
	req.setText("PR28");
	tr.messIO(req,"UserProtocol");
	if(req.attr("err").length) { t_err = req.attr("err"); MP_X2 = EVAL_INT;}
	else if(req.text().slice(0,2) != "PA") MP_X2 = EVAL_INT;
	else MP_X2 = req.text().slice(4).toInt();
}

// MP Y2
if(t_err.toInt())	MP_Y2 = EVAL_INT;
else {
	req.setText("PR29");
	tr.messIO(req,"UserProtocol");
	if(req.attr("err").length) { t_err = req.attr("err"); MP_Y2 = EVAL_INT;}
	else if(req.text().slice(0,2) != "PA") MP_Y2 = EVAL_INT;
	else MP_Y2 = req.text().slice(4).toInt();
}

// MP Z
if(t_err.toInt())	MP_Z = EVAL_INT;
else {
	req.setText("PR30");
	tr.messIO(req,"UserProtocol");
	if(req.attr("err").length) { t_err = req.attr("err"); MP_Z = EVAL_INT;}
	else if(req.text().slice(0,2) != "PA") MP_Z = EVAL_INT;
	else MP_Z = req.text().slice(4).toInt();
}

f_err = t_err;','','',1509283338);
INSERT INTO "tmplib_DevLib" VALUES('TM510x','Elemer TM510x','','','Multichannels thermometer Elemer TM5102 and TM5103 of firm Elemer (http://www.elemer.ru).
Author: Roman Savochenko <rom_as@oscada.org>
Version: 1.0.0','','',10,0,'JavaLikeCalc.JavaScript
if(f_start) devTp = EVAL_INT;

tr = SYS.Transport.Serial.nodeAt("out_"+transport);
req = SYS.XMLNode("mess").setAttr("ProtIt", "TM510x").setAttr("addr", addr);

t_err = "";

//Device type get
if(devTp.isEVal() || !devTp) {
	req.setText("0");
	tr.messIO(req, "UserProtocol");
	if(req.attr("err").length)	t_err = req.attr("err");
	else devTp = req.text().toInt();
}

//Inputs read
errors = "";
if(t_err.length) in1 = in2 = in3 = in4 = in5 = in6 = in7 = in8 = EVAL_REAL;
else {
	req.setText("6;0;8");
	tr.messIO(req,"UserProtocol");
	if(req.attr("err").length)
	{ t_err = req.attr("err"); in1 = in2 = in3 = in4 = in5 = in6 = in7 = in8 = EVAL_REAL; }
	else {
		off = 0;
		tvl = "";
		in1 = ((tvl=req.text().parse(0, ";", off))[0] == "$") ? EVAL_INT : tvl.toReal();
		errors += ((tvl[0] == "$") ? tvl.slice(1) : "0")+";";
		in2 = ((tvl=req.text().parse(0, ";", off))[0] == "$") ? EVAL_INT : tvl.toReal();
		errors += ((tvl[0] == "$") ? tvl.slice(1) : "0")+";";
		in3 = ((tvl=req.text().parse(0, ";", off))[0] == "$") ? EVAL_INT : tvl.toReal();
		errors += ((tvl[0] == "$") ? tvl.slice(1) : "0")+";";
		in4 = ((tvl=req.text().parse(0, ";", off))[0] == "$") ? EVAL_INT : tvl.toReal();
		errors += ((tvl[0] == "$") ? tvl.slice(1) : "0")+";";
		in5 = ((tvl=req.text().parse(0, ";", off))[0] == "$") ? EVAL_INT : tvl.toReal();
		errors += ((tvl[0] == "$") ? tvl.slice(1) : "0")+";";
		in6 = ((tvl=req.text().parse(0, ";", off))[0] == "$") ? EVAL_INT : tvl.toReal();
		errors += ((tvl[0] == "$") ? tvl.slice(1) : "0")+";";
		in7 = ((tvl=req.text().parse(0, ";", off))[0] == "$") ? EVAL_INT : tvl.toReal();
		errors += ((tvl[0] == "$") ? tvl.slice(1) : "0")+";";
		in8 = ((tvl=req.text().parse(0, ";", off))[0] == "$") ? EVAL_INT : tvl.toReal();
		errors += ((tvl[0] == "$") ? tvl.slice(1) : "0")+";";
	}
}

f_err = t_err;','','',1509274015);
INSERT INTO "tmplib_DevLib" VALUES('UPS','','','','Uninterruptible power supply unifying data for provide all the data into single attribute of object type "All items" for next the control as the object with the data provide as table, alarming and allowing set writable attributes. The template aimed for using with module "System" data source as "UPS" and widget "Main.objProps" as the data presenter. The template also you can use as example for create like other data unification as complex object with properties, alarming and writing.
Author: Roman Savochenko <rom_as@oscada.org>
Version: 1.2.0','','',10,0,'JavaLikeCalc.JavaScript
if(f_start)	{ srcPrm = false; items = new Object(); conDelay_ = 0; }

alLev = 0;
tErr = "";

//Connect to source
if(typeof(srcPrm) != "TCntrNode:TValue:TParamContr") srcPrm = SYS.DAQ.nodeAt(srcAddr,".");
if(!srcPrm) { tErr = tr("No connection to source object"); alLev = 3; }
else if(srcPrm.err.get() != 0)	 { tErr = tr("Source error")+": "+srcPrm.err.get().parse(1,":"); alLev = 3; }
else {
	//Attributes list get and "items" update
	nLst = srcPrm.nodeList("a_");
	for(i_n = 0; i_n < nLst.length; i_n++) {
		aId = nLst[i_n].slice(2);
		aNd = srcPrm[nLst[i_n]];
		if(items[aId].isEVal()) {
			items[aId] = itW = new Object();
			itW.descr = aNd.descr();

			// Writeable check
			//SYS.messInfo("UPS test", aId+": aNd.flg()="+aNd.flg());
			if((itW.wr=!(aNd.flg()&0x04)) && aNd.flg()&0x01) {
				itW.wr = "";
				for(off = 0, pos = 0; (selId=aNd.values().parse(0,";",off)).length; pos++)
					itW.wr += ((selId==(selNm=aNd.selNames().parse(pos,";")))?selId:(selNm+" ("+selId+")"))+";";
			}
			itW.alarm = 0;
		}
		items[aId].val = aNd.get();
	}

	//Alarms process and mark
	varS = "ups_status";
	if(!(tP=srcPrm[varS]).isEVal() && !(tVl=tP.get()).isEVal()) {
		if(tVl == "OB")	{ items[varS].alarm = 1; tErr += tr("Status")+" \""+tr("On battery")+"\"; "; }
		else if(tVl == "LB")	{ items[varS].alarm = 2; tErr += tr("Status")+" \""+tr("Low battery")+"\"; "; }
		else if(tVl == "SD")	{ items[varS].alarm = 2; tErr += tr("Status")+" \""+tr("Shutdown load")+"\"; "; }
		else if(tVl.indexOf("ALARM") != -1)	{ items[varS].alarm = 2; tErr += tr("Status")+" \""+tr("ALARM")+"\"; "; }
		else items[varS].alarm = 0;
	}
	varS = "battery_packs";
	if(!(tP=srcPrm[varS]).isEVal() && !(tVl=tP.get()).isEVal()) {
		if(tVl.toInt() == 0)	{ items[varS].alarm = 2; tErr += tr("None of good battery present")+"; "; }
		else items[varS].alarm = 0;
	}
	varS = "battery_charge";
	if(!(tP=srcPrm[varS]).isEVal() && !(tVl=tP.get()).isEVal()) {
		if(tVl.toReal() < bChL) { items[varS].alarm = 1; tErr += tr("Battery charge low")+"; "; }
		else if(tVl.toReal() < bChLL) { items[varS].alarm = 2; tErr += tr("Battery charge critical")+"; "; }
		else items[varS].alarm = 0;
	}
	varS = "battery_packs_bad";
	if(!(tP=srcPrm[varS]).isEVal() && !(tVl=tP.get()).isEVal()) {
		if(tVl.toInt())	{ items[varS].alarm = 1; tErr += tr("Bad %1 batteries present").replace("%1",tVl); }
		else items[varS].alarm = 0;
	}
	varS = "input_voltage";
	if(!(tP=srcPrm[varS]).isEVal() && !(tVl=tP.get()).isEVal()) {
		if(tVl.toReal() > 10 && tVl.toReal() < inVL) { items[varS].alarm = 1; tErr += tr("Input voltage low")+"; "; }
		else if(tVl.toReal() > inVH) { items[varS].alarm = 1; tErr += tr("Input voltage high")+"; "; }
		else items[varS].alarm = 0;
	}
	varS = "input_frequency";
	if(!(tP=srcPrm[varS]).isEVal() && !(tVl=tP.get()).isEVal()) {
		if(tVl.toReal() < inFLL) { items[varS].alarm = 2; tErr += tr("Input frequency too low")+"; "; }
		else if(tVl.toReal() > inFHH)	{ items[varS].alarm = 2; tErr += tr("Input frequency too high")+"; "; }
		else if(tVl.toReal() < inFL) { items[varS].alarm = 1; tErr += tr("Input frequency low")+"; "; }
		else if(tVl.toReal() > inFH) { items[varS].alarm = 1; tErr += tr("Input frequency high")+"; "; }
		else items[varS].alarm = 0;
	}
	varS = "ups_load";
	if(!(tP=srcPrm[varS]).isEVal() && !(tVl=tP.get()).isEVal()) {
		if(tVl.toReal() > loadHH)		{ items[varS].alarm = 2; tErr += tr("UPS overloaded")+"; "; }
		else if(tVl.toReal() > loadH)	{ items[varS].alarm = 1; tErr += tr("UPS load high")+"; "; }
		else items[varS].alarm = 0;
	}
	varS = "ups_temperature";
	if(!(tP=srcPrm[varS]).isEVal() && !(tVl=tP.get()).isEVal()) {
		if(tVl.toReal() > tHH) 		{ items[varS].alarm = 2; tErr += tr("UPS overheated")+"; "; }
		else if(tVl.toReal() > tH)	{ items[varS].alarm = 1; tErr += tr("Temperature high")+"; "; }
		else items[varS].alarm = 0;
	}

	//Set variables process
	for(var aIt in items) {
		it = items[aIt];
		if(!it.set.isEVal()) {
			aNd = srcPrm["a_"+it.id];
			if(aNd.flg()&0x01 && (selV=it.set.match(".+\\((.+)\\)$")).length) it.set = selV[1];
			aNd.set(it.set);
			it.set = EVAL_REAL;
		}
		alLev = max(alLev, it.alarm);
	}
}

//SYS.messInfo("UPS test", "tErr="+tErr+"; alLev="+alLev);
tErr = tErr.length ? ""+alLev+":"+tErr : "0";

//Alarms forming
if(alDelay > 0 && conDelay_ <= 0 && tErr.toInt() != f_err.toInt())	tErr1 = tErr.toInt();
if(alDelay > 0 && conDelay_ > 0 && tErr.toInt() != tErr1)	{ conDelay_ = 0; tErr1 = tErr.toInt(); }
if(alDelay > 0 && conDelay_ < alDelay){ conDelay_ += 1/f_frq; return; }
if(tErr.toInt() && tErr.toInt() != f_err.toInt())
	this.cntr().alarmSet((NAME.length?NAME:SHIFR)+": "+DESCR+": "+tErr.parse(1,":"), -(2+alLev), SHIFR);
else if(f_err.toInt() && !tErr.toInt())
	this.cntr().alarmSet((NAME.length?NAME:SHIFR)+": "+DESCR+": "+tr("NORMA"), 1, SHIFR);
f_err = tErr;
conDelay_ = 0;','','',1481351833);
INSERT INTO "tmplib_DevLib" VALUES('VKT7','VKT-7','','','Firm "Teplocom" (http://www.teplocom.spb.ru) computer "VKT-7" for complex heat measurement and counting. The device complex enough to provide more parameters, more history and accessed by a nonlinear Serial-based protocol at low speed. The template implements acquisition for all significant parameters, gets for their history by hours, days and result months. Also you can to append easily enough for processing of the remained parameters.
Author: Roman Savochenko <rom_as@oscada.org>
Sponsored: Vladislav Chubuk
Version: 1.0.0','','',60,0,'JavaLikeCalc.JavaScript
using Special.FLibSYS;

if(f_start) {
	//Variables list prepare
	varsLs = new Object();
	//Тв1
	varsLs["t1_1"] = 0; varsLs["t2_1"] = 1; varsLs["t3_1"] = 2;		//t
	varsLs["V1_1"] = 3; varsLs["V2_1"] = 4;	varsLs["V3_1"] = 5;		//V
	varsLs["M1_1"] = 6;	varsLs["M2_1"] = 7;	varsLs["M3_1"] = 8;		//M
	varsLs["P1_1"] = 9;	varsLs["P2_1"] = 10;	//P
	varsLs["Mg_1"] = 11;	//Mg
	varsLs["Qo_1"] = 12;	//Qo
	varsLs["Qg_1"] = 13;	//Qg
	varsLs["dt_1"] = 14;	//dt
	varsLs["BNR_1"] = 17; varsLs["BOC_1"] = 18;//BNR and BOC
	varsLs["G1_1"] = 19; varsLs["G2_1"] = 20;	//G
	//Тв2
	varsLs["t1_2"] = 22; varsLs["t2_2"] = 23; varsLs["t3_2"] = 24;	//t
	varsLs["V1_2"] = 25; varsLs["V2_2"] = 26; varsLs["V3_2"] = 27;	//V
	varsLs["M1_2"] = 28; varsLs["M2_2"] = 29; varsLs["M3_2"] = 30;	//M
	varsLs["P1_2"] = 31; varsLs["P2_2"] = 32;	//P
	varsLs["Mg_2"] = 33;	//Mg
	varsLs["Qo_2"] = 34;	//Qo
	varsLs["Qg_2"] = 35;	//Qg
	varsLs["dt_2"] = 36;	//dt
	varsLs["BNR_2"] = 39; varsLs["BOC_2"] = 40;//BNR and BOC
	varsLs["G1_2"] = 41; varsLs["G2_2"] = 42;	//G

	//Buffers create
	bfH = vArhBuf(4/*Real*/, 10, 3600*1000000, true, false);
	bfD = vArhBuf(4/*Real*/, 10, 24*3600*1000000, true, false);
	bfM = vArhBuf(4/*Real*/, 10, 24*3600*1000000, true, false);
}

t_err = "";
//Imitation for values
if(imit > 0.1) {
	//SYS.messDebug("/VKT7/TMPL","imit="+t1_1+"; dt="+(imit*(rand(t1_1)-t1_1/2)/100));
	for(var cA in varsLs)
		if(cA.slice(0,2) == "dt") arguments[cA] = arguments["t1_"+cA.slice(3)]-arguments["t2_"+cA.slice(3)];
		else arguments[cA] += imit*(rand(arguments[cA])-arguments[cA]/2)/100;
	f_err = "0";
	return;
}

if(f_stop) {
	f_err = "";
	for(var cA in varsLs) arguments[cA] = EVAL_REAL;
	return;
}

//Set transport and RTS
if(f_start){ tr = SYS.nodeAt(trAddr,"."); tr.TS(true); valsPrec = false; valsSz = false; }
if(!tr)	t_err = "1:"+tr("Transport ''%1'' error.").replace("%1",trAddr);

req = SYS.XMLNode("mess").setAttr("ProtIt","VKT7").setAttr("addr",0);

//Session start
if(!t_err.length) {
	SYS.messDebug("/VKT7/TMPL",tr("Start session."));
	req.setAttr("err","2:"+tr("No connection")).setText(SYS.strFromCharCode(0x10,0x3F,0xFF,0x00,0x00,0xCC,0x80,0x00,0x00,0x00));
	tr.messIO(req,"UserProtocol");
	t_err = req.attr("err");
}

//Request for service information
if(!t_err.length && (f_start || firmVer.isEVal())) {
	SYS.messDebug("/VKT7/TMPL",tr("Request for service info."));
	req.setAttr("err","2:"+tr("No connection")).setText(SYS.strFromCharCode(0x03,0x3f,0xf9,0x00,0x00));
	tr.messIO(req,"UserProtocol");
	t_err = req.attr("err");
	if(!t_err.length) {
		rez = req.text();
		firmVer = (rez.charCodeAt(2)>>4)+(rez.charCodeAt(2)&0x0F)/10;
		abonId = rez.slice(7,7+8);
		netNum = rez.charCodeAt(15);
		repDay = rez.charCodeAt(16);
		modelImpl = rez.charCodeAt(17);
	}
}

//Request for counter time and hour archive begin
if(!t_err.length && (f_start || !lastHTm || !lastDTm || !lastResTm)) {
	SYS.messDebug("/VKT7/TMPL",tr("Request for counter time and hour archive begin."));
	req.setAttr("err","2:"+tr("No connection")).setText(SYS.strFromCharCode(0x03,0x3f,0xf6,0x00,0x00));
	tr.messIO(req,"UserProtocol");
	t_err = req.attr("err");
	if(!t_err.length) {
		rez = req.text();
		lastHTm = max(lastHTm, SYS.mktime(0,0,rez.charCodeAt(5),rez.charCodeAt(2),rez.charCodeAt(3)-1,2000+rez.charCodeAt(4)));
		diffTm = floor((SYS.time()-SYS.mktime(0,0,rez.charCodeAt(9),rez.charCodeAt(6),rez.charCodeAt(7)-1,2000+rez.charCodeAt(8)))/3600);
		leftHTm = (SYS.time()-lastHTm)/3600 - diffTm;
	}
	lastDTm = max(lastDTm, SYS.time()-maxDayDepth*24*3600);
	leftDTm = (SYS.time()-lastDTm)/(24*3600);
	lastResTm = max(lastResTm, SYS.time()-maxDayDepth*24*3600);
	leftResTm = (SYS.time()-lastResTm)/(31*24*3600);
}

//Request for dimensions and precisions
if(!t_err.length && (f_start || !valsPrec)) {
	SYS.messDebug("/VKT7/TMPL",tr("Request for dimensions and precisions."));
	req.setAttr("err","2:"+tr("No connection")).setText(SYS.strFromCharCode(0x10, 0x3F, 0xFF, 0x00, 0x00, 0x72,
		0x2C, 0x00, 0x00, 0x40, 0x07, 0x00,	//tTypeM
		0x2D, 0x00, 0x00, 0x40, 0x07, 0x00,	//GTypeM
		0x2E, 0x00, 0x00, 0x40, 0x07, 0x00,	//VTypeM
		0x2F, 0x00, 0x00, 0x40, 0x07, 0x00,	//MTypeM
		0x30, 0x00, 0x00, 0x40, 0x07, 0x00,	//PTypeM
		0x35, 0x00, 0x00, 0x40, 0x07, 0x00,	//QoTypeM
		0x37, 0x00, 0x00, 0x40, 0x07, 0x00,	//QntTypeHIM
		0x38, 0x00, 0x00, 0x40, 0x07, 0x00,	//QntTypeM
		0x39, 0x00, 0x00, 0x40, 0x01, 0x00,	//tTypeFractDiNum
		0x3B, 0x00, 0x00, 0x40, 0x01, 0x00,	//VTypeFractDigNum1
		0x3C, 0x00, 0x00, 0x40, 0x01, 0x00,	//MTypeFractDigNum1
		0x3D, 0x00, 0x00, 0x40, 0x01, 0x00,	//PTypeFractDigNum1
		0x3E, 0x00, 0x00, 0x40, 0x01, 0x00,	//dtTypeFractDigNum1
		0x42, 0x00, 0x00, 0x40, 0x01, 0x00,	//QoTypeFractDigNum1
		0x45, 0x00, 0x00, 0x40, 0x01, 0x00,	//VTypeFractDigNum2
		0x46, 0x00, 0x00, 0x40, 0x01, 0x00,	//MTypeFractDigNum2
		0x47, 0x00, 0x00, 0x40, 0x01, 0x00,	//PTypeFractDigNum2
		0x48, 0x00, 0x00, 0x40, 0x01, 0x00,	//dtTypeFractDigNum2
		0x4C, 0x00, 0x00, 0x40, 0x01, 0x00));	//QoTypeFractDigNum2
	tr.messIO(req,"UserProtocol");
	t_err = req.attr("err");
	if(!t_err.length) {
		req.setAttr("err","2:"+tr("No connection")).setText(SYS.strFromCharCode(0x03,0x3F,0xFE,0x00,0x00));
		tr.messIO(req,"UserProtocol");
		t_err = req.attr("err");
		if(!t_err.length) {
			rez = req.text();	off = 2;
			vLen = (rez.charCodeAt(off+1)<<8)|rez.charCodeAt(off);
			tTypeM = SYS.strCodeConv(rez.slice(off+2,off+2+vLen),"CP866","");	off += vLen+4;
			vLen = (rez.charCodeAt(off+1)<<8)|rez.charCodeAt(off);
			GTypeM = SYS.strCodeConv(rez.slice(off+2,off+2+vLen),"CP866","");	off += vLen+4;
			vLen = (rez.charCodeAt(off+1)<<8)|rez.charCodeAt(off);
			VTypeM = SYS.strCodeConv(rez.slice(off+2,off+2+vLen),"CP866","");	off += vLen+4;
			vLen = (rez.charCodeAt(off+1)<<8)|rez.charCodeAt(off);
			MTypeM = SYS.strCodeConv(rez.slice(off+2,off+2+vLen),"CP866","");	off += vLen+4;
			vLen = (rez.charCodeAt(off+1)<<8)|rez.charCodeAt(off);
			PTypeM = SYS.strCodeConv(rez.slice(off+2,off+2+vLen),"CP866","");	off += vLen+4;
			vLen = (rez.charCodeAt(off+1)<<8)|rez.charCodeAt(off);
			QoTypeM = SYS.strCodeConv(rez.slice(off+2,off+2+vLen),"CP866","");	off += vLen+4;
			vLen = (rez.charCodeAt(off+1)<<8)|rez.charCodeAt(off);
			QntTypeHIM = SYS.strCodeConv(rez.slice(off+2,off+2+vLen),"CP866","");	off += vLen+4;
			vLen = (rez.charCodeAt(off+1)<<8)|rez.charCodeAt(off);
			QntTypeM = SYS.strCodeConv(rez.slice(off+2,off+2+vLen),"CP866","");	off += vLen+4;
			valsPrec = new Object();
			valsPrec["tTypeFractDiNum"] = rez.charCodeAt(off);	off += 3;
			valsPrec["VTypeFractDigNum1"] = rez.charCodeAt(off);	off += 3;
			valsPrec["MTypeFractDigNum1"] = rez.charCodeAt(off);	off += 3;
			valsPrec["PTypeFractDigNum1"] = rez.charCodeAt(off);	off += 3;
			valsPrec["dtTypeFractDigNum1"] = rez.charCodeAt(off);	off += 3;
			valsPrec["QoTypeFractDigNum1"] = rez.charCodeAt(off);	off += 3;
			valsPrec["VTypeFractDigNum2"] = rez.charCodeAt(off);	off += 3;
			valsPrec["MTypeFractDigNum2"] = rez.charCodeAt(off);	off += 3;
			valsPrec["PTypeFractDigNum2"] = rez.charCodeAt(off);	off += 3;
			valsPrec["dtTypeFractDigNum2"] = rez.charCodeAt(off);	off += 3;
			valsPrec["QoTypeFractDigNum2"] = rez.charCodeAt(off);	off += 3;
		}
	}
}

//Request for values present list and size.
if(!t_err.length && (f_start || !valsSz || !valsSz.length)) {
	SYS.messDebug("/VKT7/TMPL",tr("Request for values present list and size."));
	req.setAttr("err","2:"+tr("No connection")).setText(SYS.strFromCharCode(0x03,0x3f,0xfc,0x00,0x00));
	tr.messIO(req,"UserProtocol");
	t_err = req.attr("err");
	if(!t_err.length) {
		rez = req.text();
		valsSz = new Array();
		for(off = 2; off < rez.length; off += 6)
			valsSz[rez.charCodeAt(off)] = rez.charCodeAt(off+4);
	}
}

//Readed items list write
if(!t_err.length) {
	SYS.messDebug("/VKT7/TMPL",tr("Readed items list write."));
	data = "";
	for(var cA in varsLs) {
		itId = varsLs[cA];
		if(!valsSz[itId].isEVal())	data += SYS.strFromCharCode(itId, 0x00, 0x00, 0x40, valsSz[itId], 0x00);
	}

	if(!data.length) {
		for(var cA in varsLs) arguments[cA] = EVAL_REAL;
		f_err = "10:"+tr("Data list for request empty.");
		return;
	}
	req.setAttr("err","2:"+tr("No connection")).setText(SYS.strFromCharCode(0x10,0x3f,0xff,0x00,0x00,data.length)+data);
	//SYS.messDebug("/VKT7/TMPL","SetDataList: "+Special.FLibSYS.strDec4Bin(req.text()));
	tr.messIO(req,"UserProtocol");
	t_err = req.attr("err");
}

//Read 0-current value(4), 1-hour archive(0) and 2-result month archive(3)
errAttrs = "";
for(i_dt = 0; !t_err.length && i_dt < 4; i_dt++) {
	tpReq = 4;																						//current
	if(i_dt == 1)			{ tpReq = 0; if(!arhH.length) continue; }		//hour archive
	else if(i_dt == 2)	{ tpReq = 1; if(!arhD.length) continue; }		//day archive
	else if(i_dt == 3)	{ tpReq = 3; if(!arhRes.length) continue; }	//result archive

	//Request for curent values (4).
	if(tpReq == 4 && !t_err.length) {
		SYS.messDebug("/VKT7/TMPL",tr("Request for curent values."));
		req.setAttr("err","2:"+tr("No connection")).setText(SYS.strFromCharCode(0x10,0x3f,0xfd,0x00,0x00,0x02,0x04,0x00));
		tr.messIO(req,"UserProtocol");
		t_err = req.attr("err");
	}

	//Request for archive values hour
	if(tpReq == 0) {
		if(!t_err.length) {
			if((SYS.time()-lastHTm-diffTm*3600)/3600 < 2) continue;	//No more ready hours present
			SYS.messDebug("/VKT7/TMPL",tr("Request for hour archive values."));
			req.setAttr("err","2:"+tr("No connection")).setText(SYS.strFromCharCode(0x10,0x3f,0xfd,0x00,0x00,0x02,tpReq,0x00));
			tr.messIO(req,"UserProtocol");
			t_err = req.attr("err");
		}

		// Set for hour archive value data
		if(!t_err.length) {
			SYS.messDebug("/VKT7/TMPL",tr("Request for set archive data."));
			stHour = stDay = stMonth = stYear = 0;
			SYS.localtime(lastHTm, 0, 0, stHour, stDay, stMonth, stYear);
			stHour++;
			rdTime = SYS.mktime(0, 0, stHour, stDay, stMonth, stYear);

			req.setAttr("err","2:"+tr("No connection")).setText(SYS.strFromCharCode(0x10,0x3f,0xfb,0x00,0x00,0x04,stDay,stMonth+1,stYear-2000,stHour));
			tr.messIO(req,"UserProtocol");
			t_err = req.attr("err");
			if(t_err.parse(1,":").toInt() == 3) { lastHTm = rdTime; leftHTm = (SYS.time()-lastHTm)/3600 - diffTm; t_err = ""; continue; }
		}
	}

	//Request for archive values day
	if(tpReq == 1) {
		if(!t_err.length) {
			if((SYS.time()-lastDTm-diffTm*3600)/(24*3600) < 1) continue;	//No more ready days present
			SYS.messDebug("/VKT7/TMPL",tr("Request for day archive values."));
			req.setAttr("err","2:"+tr("No connection")).setText(SYS.strFromCharCode(0x10,0x3f,0xfd,0x00,0x00,0x02,tpReq,0x00));
			tr.messIO(req,"UserProtocol");
			t_err = req.attr("err");
		}

		// Set for day archive value data
		if(!t_err.length) {
			SYS.messDebug("/VKT7/TMPL",tr("Request for set archive data."));
			stDay = stMonth = stYear = 0;
			SYS.gmtime(lastDTm, 0, 0, 0, stDay, stMonth, stYear);
			stDay++;
			rdDTime = SYS.timegm(0, 0, 0, stDay, stMonth, stYear);

			req.setAttr("err","2:"+tr("No connection")).setText(SYS.strFromCharCode(0x10,0x3f,0xfb,0x00,0x00,0x04,stDay,stMonth+1,stYear-2000,stHour));
			tr.messIO(req,"UserProtocol");
			t_err = req.attr("err");
			if(t_err.parse(1,":").toInt() == 3) { lastDTm = rdDTime; leftDTm = (SYS.time()-lastDTm)/(24*3600) - diffTm; t_err = ""; continue; }
		}
	}

	//Request for results month archive
	if(tpReq == 3) {
		if(!t_err.length) {
			if((SYS.time()-lastResTm-diffTm*3600)/(24*3600) < 31) continue;	//No more ready month present
			SYS.messDebug("/VKT7/TMPL",tr("Request for results month archive values."));
			req.setAttr("err","2:"+tr("No connection")).setText(SYS.strFromCharCode(0x10,0x3f,0xfd,0x00,0x00,0x02,tpReq,0x00));
			tr.messIO(req,"UserProtocol");
			t_err = req.attr("err");
		}
		// Set for month results archive value data
		if(!t_err.length) {
			SYS.messDebug("/VKT7/TMPL",tr("Request for set archive data."));
			stMonth = stYear = 0;
			SYS.gmtime(lastResTm, 0, 0, 0, 0, stMonth, stYear);
			stMonth++;
			rdResTime = SYS.timegm(0, 0, 0, repDay, stMonth, stYear);

			req.setAttr("err","2:"+tr("No connection")).setText(SYS.strFromCharCode(0x10,0x3f,0xfb,0x00,0x00,0x04,repDay,stMonth+1,stYear-2000,0));
			tr.messIO(req,"UserProtocol");
			t_err = req.attr("err");
			if(t_err.parse(1,":").toInt() == 3) { lastResTm = rdResTime; leftResTm = (SYS.time()-lastResTm)/(31*24*3600); t_err = ""; continue; }
		}
	}

	//Request for data read
	if(!t_err.length) {
		SYS.messDebug("/VKT7/TMPL",tr("Request for data read."));
		req.setAttr("err","2:"+tr("No connection")).setText(SYS.strFromCharCode(0x03,0x3F,0xFE,0x00,0x00));
		tr.messIO(req,"UserProtocol");
		t_err = req.attr("err");
		if(!t_err.length) {
			rez = req.text(); off = 2; vSz = 0;
			for(var cA in varsLs) {
				itId = varsLs[cA]; vSz = valsSz[itId];
				if(vSz.isEVal()) {
					if(tpReq == 4) arguments[cA] = EVAL_REAL;
					continue;
				}
				//SYS.messDebug("/VKT7/TMPL","off: "+off+"; itId: "+itId+"; vSz: "+vSz);
				if(rez.charCodeAt(off+vSz) == 0xC0) {
					for(pVal = 0, i_b = 0; i_b < vSz; i_b++) pVal = pVal | rez.charCodeAt(off+i_b)<<(i_b*8);
					//Float type values
					if(cA[0] == "G") pVal = floatMergeWord(pVal&0xFFFF, (pVal>>16)&0xFFFF);
					//Integer type values, negative process
					else if(pVal&(0x80<<(8*(vSz-1))))	pVal -= (1<<(8*vSz));
					// Type specific
					if(cA[0] == "t")						tmpVl = pVal/pow(10,valsPrec["tTypeFractDiNum"]);
					else if(cA[0] == "V")				tmpVl = pVal/pow(10,valsPrec["VTypeFractDigNum"+cA[3]]);
					else if(cA[0] == "M")				tmpVl = pVal/pow(10,valsPrec["MTypeFractDigNum"+cA[3]]);
					else if(cA[0] == "P")				tmpVl = pVal/pow(10,valsPrec["PTypeFractDigNum"+cA[3]]);
					else if(cA[0] == "Q")				tmpVl = pVal/pow(10,valsPrec["QoTypeFractDigNum"+cA[3]]);
					else if(cA.slice(0,2) == "dt")tmpVl = pVal/pow(10,valsPrec["dtTypeFractDigNum"+cA[3]]);
					else tmpVl = pVal;

					// Write current or archive
					if(tpReq == 4) {
						arguments[cA] = tmpVl;
						SYS.messDebug("/VKT7/TMPL",tr("Set current")+" "+cA+": "+tmpVl);
					}
					else if(!(aObj=vArh(this.nodePath(".")+"."+cA)).isNull()) {
						if(tpReq == 0) {
							bfH.set(tmpVl, rdTime, 0); aObj.copy(bfH, rdTime, 0, rdTime, 0, arhH);
							SYS.messDebug("/VKT7/TMPL",tr("Write to hour archive for time")+": "+rdTime+"; "+cA+": "+tmpVl);
						}
						if(tpReq == 1) {
							bfD.set(tmpVl, rdDTime, 0); aObj.copy(bfD, rdDTime, 0, rdDTime, 0, arhD);
							SYS.messDebug("/VKT7/TMPL",tr("Write to day archive for time")+": "+rdDTime+"; "+cA+": "+tmpVl);
						}
						else if(tpReq == 3) {
							bfM.set(tmpVl, rdResTime, 0); aObj.copy(bfM, rdResTime, 0, rdResTime, 0, arhRes);
							SYS.messDebug("/VKT7/TMPL",tr("Write to result month archive for time")+": "+rdResTime+"; "+cA+": "+tmpVl);
						}
					}
				}
				else if(tpReq == 4) { arguments[cA] = EVAL_REAL; errAttrs += ""+itId+"="+rez.charCodeAt(off+vSz).toString(16)+"; "; }
				off += vSz+2;
			}
			if(tpReq == 0) { lastHTm = rdTime; leftHTm = (SYS.time()-lastHTm)/3600 - diffTm; }
			else if(tpReq == 1) { lastDTm = rdDTime; leftDTm = (SYS.time()-lastDTm)/(24*3600) - diffTm; }
			else if(tpReq == 3) { lastResTm = rdResTime; leftResTm = (SYS.time()-lastResTm)/(31*24*3600); }
		}
	}
}

if(t_err.length) {
	SYS.messDebug("/VKT7/TMPL",tr("Error response")+": "+t_err);
	f_err = t_err;
	for(var cA in varsLs) arguments[cA] = EVAL_REAL;
}
else f_err = errAttrs.length ? "11:"+tr("Quality errors")+": "+errAttrs : "0";','','',1509274088);
INSERT INTO "tmplib_DevLib" VALUES('SMDP','Sycon Multi Drop Protocol','','','STM devices for resonant frequency measurement for mass of deposited films attached to its surface by Sycon Instruments, Inc. (http://www.sycon.com).
Author: Roman Savochenko <rom_as@oscada.org>
Sponsored: Vasiliy Grigoriev from "Vacuum technologies laboratory (http://e-beam.ru)".
Version: 1.0.0','','',10,0,'JavaLikeCalc.JavaScript
if(f_stop) return;
if(f_start) {
	cnt = 0;
	version = "";
	SessId_ = SessId = QlvlTrip_ = QlvlTrip = SlvlTrip_ = SlvlTrip = EVAL_INT;
	Fq_ = Fq = Fm_ = Fm = Density_ = Density = Zratio_ = Zratio = Tooling_ = Tooling = RateReq_ = RateReq = EVAL_REAL;
}

tr = SYS.Transport.Serial.nodeAt("out_"+transport);
req = SYS.XMLNode("mess").setAttr("ProtIt","SMDP").setAttr("addr",addr).setAttr("try",3);

t_err = "";

//Configurations first write
// Session ID.
if(SessId != SessId_) {
	SYS.messDebug("/SMDP/tmpl",""+SessId+"!="+SessId_);	
	req.setAttr("err","1:"+tr("No connect")).setText("\xD0\x41"+SessId); tr.messIO(req,"UserProtocol");
	if(!(t_err=req.attr("err")).length) SessId_ = SessId; else SessId = SessId_;
}
// Xtal start freq. (Hz)
if(Fq != Fq_) {
	req.setAttr("err","1:"+tr("No connect")).setText("\xD0\x42"+Fq); tr.messIO(req,"UserProtocol");
	if(!(t_err=req.attr("err")).length) Fq_ = Fq; else Fq = Fq_;
}
// Xtal min freq. (Hz)
if(Fm != Fm_) {
	req.setAttr("err","1:"+tr("No connect")).setText("\xD0\x43"+Fm); tr.messIO(req,"UserProtocol");
	if(!(t_err=req.attr("err")).length) Fm_ = Fm; else Fm = Fm_;
}
// Material density (gm/cc).
if(Density != Density_) {
	req.setAttr("err","1:"+tr("No connect")).setText("\xD0\x44"+Density); tr.messIO(req,"UserProtocol");
	if(!(t_err=req.attr("err")).length) Density_ = Density; else Density = Density_;
}
// Material Z ratio. Not scaled or unitized.
if(Zratio != Zratio_) {
	req.setAttr("err","1:"+tr("No connect")).setText("\xD0\x45"+Zratio); tr.messIO(req,"UserProtocol");
	if(!(t_err=req.attr("err")).length) Zratio_ = Zratio; else Zratio = Zratio_;
}
// 1.000 is 100 % tooling (unity).
if(Tooling != Tooling_) {
	req.setAttr("err","1:"+tr("No connect")).setText("\xD0\x46"+Tooling); tr.messIO(req,"UserProtocol");
	if(!(t_err=req.attr("err")).length) Tooling_ = Tooling; else Tooling = Tooling_;
}
// Requested rate (A/S).
if(RateReq != RateReq_) {
	req.setAttr("err","1:"+tr("No connect")).setText("\xD0\x47"+RateReq); tr.messIO(req,"UserProtocol");
	if(!(t_err=req.attr("err")).length) RateReq_ = RateReq; else RateReq = RateReq_;
}
// Quality threshold, if non 0 and exceeded, xtal fail occurs.
if(QlvlTrip != QlvlTrip_) {
	req.setAttr("err","1:"+tr("No connect")).setText("\xD0\x48"+QlvlTrip); tr.messIO(req,"UserProtocol");
	if(!(t_err=req.attr("err")).length) QlvlTrip_ = QlvlTrip; else QlvlTrip = QlvlTrip_;
}
// Stability threshold, if non 0 and exceeded, xtal fail occurs.
if(SlvlTrip != SlvlTrip_) {
	req.setAttr("err","1:"+tr("No connect")).setText("\xD0\x49"+SlvlTrip); tr.messIO(req,"UserProtocol");
	if(!(t_err=req.attr("err")).length) SlvlTrip_ = SlvlTrip; else SlvlTrip = SlvlTrip_;
}

//======= Data getting (by self bigger period) ========
if((cnt++)%max(1,perGet*f_frq))	return;

//Configurations first read
// Session ID.
if(!t_err.length && SessId.isEVal()) {
	req.setAttr("err","1:"+tr("No connect")).setText("\xC0\x41"); tr.messIO(req,"UserProtocol");
	SessId = SessId_ = (t_err=req.attr("err")).length ? EVAL_INT : req.text().slice(2).toInt(10);
}
// Xtal start freq. (Hz)
if(!t_err.length && Fq.isEVal()) {
	req.setAttr("err","1:"+tr("No connect")).setText("\xC0\x42"); tr.messIO(req,"UserProtocol");
	Fq = Fq_ = (t_err=req.attr("err")).length ? EVAL_REAL : req.text().slice(2).toReal();
}
// Xtal min freq. (Hz)
if(!t_err.length && Fm.isEVal()) {
	req.setAttr("err","1:"+tr("No connect")).setText("\xC0\x43"); tr.messIO(req,"UserProtocol");
	Fm = Fm_ = (t_err=req.attr("err")).length ? EVAL_REAL : req.text().slice(2).toReal();
}
// Material density (gm/cc).
if(!t_err.length && Density.isEVal()) {
	req.setAttr("err","1:"+tr("No connect")).setText("\xC0\x44"); tr.messIO(req,"UserProtocol");
	Density = Density_ = (t_err=req.attr("err")).length ? EVAL_REAL : req.text().slice(2).toReal();
}
// Material Z ratio. Not scaled or unitized.
if(!t_err.length && Zratio.isEVal()) {
	req.setAttr("err","1:"+tr("No connect")).setText("\xC0\x45"); tr.messIO(req,"UserProtocol");
	Zratio = Zratio_ = (t_err=req.attr("err")).length ? EVAL_REAL : req.text().slice(2).toReal();
}
// 1.000 is 100 % tooling (unity).
if(!t_err.length && Tooling.isEVal()) {
	req.setAttr("err","1:"+tr("No connect")).setText("\xC0\x46"); tr.messIO(req,"UserProtocol");
	Tooling = Tooling_ = (t_err=req.attr("err")).length ? EVAL_REAL : req.text().slice(2).toReal();
}
// Requested rate (A/S).
if(!t_err.length && RateReq.isEVal()) {
	req.setAttr("err","1:"+tr("No connect")).setText("\xC0\x47"); tr.messIO(req,"UserProtocol");
	RateReq = RateReq_ = (t_err=req.attr("err")).length ? EVAL_REAL : req.text().slice(2).toReal();
}
// Quality threshold, if non 0 and exceeded, xtal fail occurs.
if(!t_err.length && QlvlTrip.isEVal()) {
	req.setAttr("err","1:"+tr("No connect")).setText("\xC0\x48"); tr.messIO(req,"UserProtocol");
	QlvlTrip = QlvlTrip_ = (t_err=req.attr("err")).length ? EVAL_REAL : req.text().slice(2).toInt(10);
}
// Stability threshold, if non 0 and exceeded, xtal fail occurs.
if(!t_err.length && SlvlTrip.isEVal()) {
	req.setAttr("err","1:"+tr("No connect")).setText("\xC0\x49"); tr.messIO(req,"UserProtocol");
	SlvlTrip = SlvlTrip_ = (t_err=req.attr("err")).length ? EVAL_REAL : req.text().slice(2).toInt(10);
}

//Runtime variables
// Read version, single
if(!t_err.length && (!version.length || version.isEVal())) {
	req.setAttr("err","1:"+tr("No connect")).setText("\x40"); tr.messIO(req,"UserProtocol");
	version = (t_err=req.attr("err")).length ? EVAL_STR : req.text().slice(1);
}

// Read Configuration parameter session ID
if(t_err.length) CfgPrmSSID = EVAL_INT;
else {
	req.setAttr("err","1:"+tr("No connect")).setText("\xC0\x61"); tr.messIO(req,"UserProtocol");
	CfgPrmSSID = (t_err=req.attr("err")).length ? EVAL_INT : req.text().slice(2).toInt(10);
	//SYS.messDebug("/SMDP/tmpl","CfgPrmSSID: "+req.text());
}

// Read Measurement serial number
if(t_err.length) Srlno = EVAL_INT;
else {
	req.setAttr("err","1:"+tr("No connect")).setText("\xC0\x62"); tr.messIO(req,"UserProtocol");
	Srlno = (t_err=req.attr("err")).length ? EVAL_INT : req.text().slice(2).toInt(10);
}

// Read Channel freq.
if(t_err.length) RawFreq = EVAL_REAL;
else {
	req.setAttr("err","1:"+tr("No connect")).setText("\xC0\x63"); tr.messIO(req,"UserProtocol");
	RawFreq = (t_err=req.attr("err")).length ? EVAL_REAL : req.text().slice(2).toReal();
}

// Read Last used, good channel freq. (Hz)
if(t_err.length) GoodFreq = EVAL_REAL;
else {
	req.setAttr("err","1:"+tr("No connect")).setText("\xC0\x64"); tr.messIO(req,"UserProtocol");
	GoodFreq = (t_err=req.attr("err")).length ? EVAL_REAL : req.text().slice(2).toReal();
}

// Read Computed raw thickness, from frequency. (A)
if(t_err.length) RawThick = EVAL_REAL;
else {
	req.setAttr("err","1:"+tr("No connect")).setText("\xC0\x65"); tr.messIO(req,"UserProtocol");
	RawThick = (t_err=req.attr("err")).length ? EVAL_REAL : req.text().slice(2).toReal();
}

// Read Computed material related thickness, can be zeroed. (A)
if(t_err.length) XtalThick = EVAL_REAL;
else {
	req.setAttr("err","1:"+tr("No connect")).setText("\xC0\x66"); tr.messIO(req,"UserProtocol");
	XtalThick = (t_err=req.attr("err")).length ? EVAL_REAL : req.text().slice(2).toReal();
}

// Read Filtered computed material related thickness, can be zeroed. (A)
if(t_err.length) XtalThick_F = EVAL_REAL;
else {
	req.setAttr("err","1:"+tr("No connect")).setText("\xC0\x67"); tr.messIO(req,"UserProtocol");
	XtalThick_F = (t_err=req.attr("err")).length ? EVAL_REAL : req.text().slice(2).toReal();
}

// Read Rate, angstroms per second.
if(t_err.length) XtalRate = EVAL_REAL;
else {
	req.setAttr("err","1:"+tr("No connect")).setText("\xC0\x68"); tr.messIO(req,"UserProtocol");
	XtalRate = (t_err=req.attr("err")).length ? EVAL_REAL : req.text().slice(2).toReal();
}

// Read Rate, angstroms per second, filtered.
if(t_err.length) XtalRate_F = EVAL_REAL;
else {
	req.setAttr("err","1:"+tr("No connect")).setText("\xC0\x69"); tr.messIO(req,"UserProtocol");
	XtalRate_F = (t_err=req.attr("err")).length ? EVAL_REAL : req.text().slice(2).toReal();
}

// Read XtalLife (%)
if(t_err.length) XtalLife = EVAL_REAL;
else {
	req.setAttr("err","1:"+tr("No connect")).setText("\xC0\x6A"); tr.messIO(req,"UserProtocol");
	XtalLife = (t_err=req.attr("err")).length ? EVAL_REAL : req.text().slice(2).toReal();
}

// Read Quality level (0-9).
if(t_err.length) XtalQual = EVAL_INT;
else {
	req.setAttr("err","1:"+tr("No connect")).setText("\xC0\x6B"); tr.messIO(req,"UserProtocol");
	XtalQual = (t_err=req.attr("err")).length ? EVAL_INT : req.text().slice(2).toInt(10);
}

// Read Highest quality level seen (0-9).
if(t_err.length) XtalQualPeak = EVAL_INT;
else {
	req.setAttr("err","1:"+tr("No connect")).setText("\xC0\x6C"); tr.messIO(req,"UserProtocol");
	XtalQualPeak = (t_err=req.attr("err")).length ? EVAL_INT : req.text().slice(2).toInt(10);
}

// Read Stability level (0-9).
if(t_err.length) XtalStab = EVAL_INT;
else {
	req.setAttr("err","1:"+tr("No connect")).setText("\xC0\x6D"); tr.messIO(req,"UserProtocol");
	XtalStab = (t_err=req.attr("err")).length ? EVAL_INT : req.text().slice(2).toInt(10);
}

// Read Highest stability level seen (0-9).
if(t_err.length) XtalStabPeak = EVAL_INT;
else {
	req.setAttr("err","1:"+tr("No connect")).setText("\xC0\x6E"); tr.messIO(req,"UserProtocol");
	XtalStabPeak = (t_err=req.attr("err")).length ? EVAL_INT : req.text().slice(2).toInt(10);
}

// Read Channel status.
if(t_err.length) XtalStat = EVAL_STR;
else {
	req.setAttr("err","1:"+tr("No connect")).setText("\xC0\x6F"); tr.messIO(req,"UserProtocol");
	if((t_err=req.attr("err")).length) XtalStat = EVAL_STR;
	else {
		stCd = req.text().slice(2).toInt(10);
		stLab = tr("Unknown");
		if(stCd == 0)			stLab = "XPROB_NONE: "+tr("No Problems. The rest below are prioritized.");
		else if(stCd == 1)	stLab = "XPROB_FREQ: "+tr("Frequency of xtal is > Fq or < Fm. Halts all other calculations.");
		else if(stCd == 2)	stLab = "XPROB_LOWLIFE: "+tr("Frequency was bad (previous XtalStat was XPROB_FREQ) and frequency is now in range, but life is less than 3%.");
		else if(stCd == 3)	stLab = "XPROB_S_FAIL: "+tr("Stability level XtalStab of xtal >= SlvlTrip.");
		else if(stCd == 4)	stLab = "XPROB_MATH: "+tr("Unable to determine rate because of a computation error, most likely caused by a parameter (density, z-ratio, etc) having a zero or otherwise invalid (non numeric or infinity) value.");
		else if(stCd == 5)	stLab = "XPROB_Q_FAIL: "+tr("XtalQual of xtal >= QlvlTrip.");
		XtalStat = ""+stCd+":"+stLab;
	}
}

// Read XtalLife (%).
if(t_err.length) XtalLife_C = EVAL_INT;
else {
	req.setAttr("err","1:"+tr("No connect")).setText("\xC0\x70"); tr.messIO(req,"UserProtocol");
	XtalLife_C = (t_err=req.attr("err")).length ? EVAL_INT : req.text().slice(2).toInt(10);
}

f_err = t_err.length ? t_err : "0";','','',1509282151);
INSERT INTO "tmplib_DevLib" VALUES('IT3','Temperature measurement IT-3','','','Temperature measurement IT-3 from OmskEtalon (http://www.omsketalon.ru).
Author: Roman Savochenko <rom_as@oscada.org>
Sponsored: Vasiliy Grigoriev from "Vacuum technologies laboratory (http://e-beam.ru)".
Version: 1.2.0','','',10,0,'JavaLikeCalc.JavaScript
if(mdPass)	{
	if(f_start) { f_err = EVAL; this.cnt.set(0, 0, 0, true); cnt_ = 0; tDl = 5; }
	if((cnt=this.cnt.get()) != cnt_)	{ tDl = 5; cnt_ = cnt; }
	else if(tDl > 0 && (tDl=tDl-1/f_frq) <= 0) {
		this.err.set("1:"+tr("No a data"), 0, 0, true);
		this.T.set(EVAL, 0, 0, true);
		this.H.set(EVAL, 0, 0, true);
		this.L.set(EVAL, 0, 0, true);
		this.relSt.set(EVAL, 0, 0, true);
	}
	return;
}

//Set transport
if(f_start)	tr = SYS.Transport.Serial.nodeAt("out_"+transport);
if(!tr)	t_err = "1:"+tr("Serial output transport ''%1'' error.").replace("%1",transport);

req = SYS.XMLNode("mess").setAttr("ProtIt","IT3").setAttr("addr",addr);

SYS.messDebug("/IT3/TMPL",tr("Request: TRANSL_TEMP."));
req.setAttr("err","2:"+tr("No connection")).setText(SYS.strFromCharCode(0xC3));
tr.messIO(req, "UserProtocol");
if(!(t_err=req.attr("err")).length && req.text().charCodeAt(0) == 0x85) {
	cntrB = req.text().charCodeAt(3);
	T = (req.text().charCodeAt(2)<<8)+req.text().charCodeAt(1);
	T = T/((cntrB&0x8)?100:10);
	relSt = (cntrB&0x1);
	L = (cntrB&0x2);
	H = (cntrB&0x4);
	//SYS.messDebug("/IT3/TMPL",tr("Request: TRANSL_TEMP: "+req.text().length));
}
else {
	T = H = L = EVAL_STR;
	if(t_err.parse(1,":").toInt() == 8) {
		req.setAttr("err","2:"+tr("No connection")).setText(SYS.strFromCharCode(0x00));
		tr.messIO(req, "UserProtocol");
	}
}

if(t_err.length) { SYS.messDebug("/IT3/TMPL",tr("Error response")+": "+t_err); f_err = t_err; }
else f_err = "0";','','',1472721622);
INSERT INTO "tmplib_DevLib" VALUES('IVE_452HS_02','IVE-452HS-02','','','Power supply of beam-electrons evaporator of "Plasma Tech" Ltd, from Moskov.
Author: Roman Savochenko <rom_as@oscada.org>
Sponsored: Vasiliy Grigoriev from "Vacuum technologies laboratory (http://e-beam.ru)".
Version: 1.0.0','','',10,0,'JavaLikeCalc.JavaScript
//Set transport
if(f_start) {
	addr_ = addr;
	transport_ = transport;
	COIA = COIA_ = COUA = COUA_ = COPA = COPA_ = DIA = DUA = DPA = DFA = EVAL_INT;
	COM_DEW = COM_DEW_ = COM_OUT = COM_OUT_ = COM_DEP	= COM_DEP_ = COM_DEL = COM_DEL_ = COM_UF = COM_UF_ =
					DKW = DKZ = DK = DE = EVAL_BOOL;
	tr = SYS.Transport.nodeAt(transport,".");
}

//Check for the transport change and connect
if(!tr || transport != transport_)	{
	tr = SYS.Transport.nodeAt(transport,".");
	transport_ = transport;
}
if(!tr)	t_err = "1:"+tr("Output transport ''%1'' error.").replace("%1",transport);
else {
	req = SYS.XMLNode("mess").setAttr("ProtIt","IVE_452HS_02").setAttr("addr",addr);

	//Check for the address need to change
	if(addr != addr_) {
		hwReg = 0x00;
		req.setAttr("err","2:"+tr("No connection")).setAttr("addr",addr_)
			.setText(SYS.strFromCharCode(0x57,0x04,0x00,hwReg,hwReg,0x06,addr));
		tr.messIO(req, "UserProtocol");
		req.setText(SYS.strFromCharCode(0x57,0x04,0x00,hwReg,hwReg,0x08,0x00));
		tr.messIO(req, "UserProtocol");
		req.setAttr("addr", addr);
		addr_ = addr;
	}

	//Check and write commands
	if(COIA != COIA_ && !(t_err=req.attr("err")).length)	{
		hwReg = 0x01;
		req.setAttr("err","2:"+tr("No connection")).setText(SYS.strFromCharCode(0x57,0x04,0x00,hwReg,hwReg,COIA&0xFF,(COIA>>8)&0xFF));
		tr.messIO(req, "UserProtocol");
	}
	if(COUA != COUA_ && !(t_err=req.attr("err")).length) {
		hwReg = 0x02;
		req.setAttr("err","2:"+tr("No connection")).setText(SYS.strFromCharCode(0x57,0x04,0x00,hwReg,hwReg,COUA&0xFF,(COUA>>8)&0xFF));
		tr.messIO(req, "UserProtocol");
	}
	if(COPA != COPA_ && !(t_err=req.attr("err")).length) {
		hwReg = 0x03;
		req.setAttr("err","2:"+tr("No connection")).setText(SYS.strFromCharCode(0x57,0x04,0x00,hwReg,hwReg,COPA&0xFF,(COPA>>8)&0xFF));
		tr.messIO(req, "UserProtocol");
	}
	if((COM_DEW != COM_DEW_ || COM_OUT != COM_OUT_ || COM_DEP != COM_DEP_ || COM_DEL != COM_DEL_ || COM_UF != COM_UF_) && !(t_err=req.attr("err")).length) {
		tmp = 0;
		if(COM_DEW)	tmp = tmp | 0x8000;
		if(COM_OUT)	tmp = tmp | 0x4000;
		if(COM_DEP)	tmp = tmp | 0x1000;
		if(COM_DEL)	tmp = tmp | 0x0800;
		if(COM_UF)	tmp = tmp | 0x0004;
		hwReg = 0x15;
		req.setAttr("err","2:"+tr("No connection")).setText(SYS.strFromCharCode(0x57,0x04,0x00,hwReg,hwReg,tmp&0xFF,(tmp>>8)&0xFF));
		tr.messIO(req, "UserProtocol");
	}

	//Read data
	if(!(t_err=req.attr("err")).length) {
		SYS.messDebug("/IVE452HS02/TMPL",tr("Read all data."));
		req.setAttr("err","2:"+tr("No connection")).setText(SYS.strFromCharCode(0x52,0x02,0x00,0x01,0x16));
		tr.messIO(req, "UserProtocol");
		if(!(t_err=req.attr("err")).length) {
			if(req.text().length < (0x16*2+5))	req.setAttr("err", "10:"+tr("Reply is not full"));
			else {
				hwReg = 0x01; COIA = COIA_ = (req.text().charCodeAt((hwReg-1)*2+6)<<8)+req.text().charCodeAt((hwReg-1)*2+5);
				hwReg = 0x02; COUA = COUA_ = (req.text().charCodeAt((hwReg-1)*2+6)<<8)+req.text().charCodeAt((hwReg-1)*2+5);
				hwReg = 0x03; COPA = COPA_ = (req.text().charCodeAt((hwReg-1)*2+6)<<8)+req.text().charCodeAt((hwReg-1)*2+5);
				hwReg = 0x07; DIA = (req.text().charCodeAt((hwReg-1)*2+6)<<8)+req.text().charCodeAt((hwReg-1)*2+5);
				hwReg = 0x08; DUA = (req.text().charCodeAt((hwReg-1)*2+6)<<8)+req.text().charCodeAt((hwReg-1)*2+5);
				hwReg = 0x0E; DAC = (req.text().charCodeAt((hwReg-1)*2+6)<<8)+req.text().charCodeAt((hwReg-1)*2+5);
				hwReg = 0x10; DPA = (req.text().charCodeAt((hwReg-1)*2+6)<<8)+req.text().charCodeAt((hwReg-1)*2+5);
				hwReg = 0x11; DFA = (req.text().charCodeAt((hwReg-1)*2+6)<<8)+req.text().charCodeAt((hwReg-1)*2+5);
				hwReg = 0x15; tmp = (req.text().charCodeAt((hwReg-1)*2+6)<<8)+req.text().charCodeAt((hwReg-1)*2+5);
				COM_DEW = COM_DEW_ = tmp&0x8000;
				COM_OUT = COM_OUT_ = tmp&0x4000;
				COM_DEP = COM_DEP_ = tmp&0x1000;
				COM_DEL = COM_DEL_ = tmp&0x0800;
				COM_UF = COM_UF_ = tmp&0x0004;
				hwReg = 0x16; tmp = (req.text().charCodeAt((hwReg-1)*2+6)<<8)+req.text().charCodeAt((hwReg-1)*2+5);
				DKW = !(tmp&0x8); DKZ = !(tmp&0x4); DK = !(tmp&0x2); DE = tmp&0x1;
			}
		}
	}
}

if(t_err.length) {
	COIA = COIA_ = COUA = COUA_ = COPA = COPA_ = DIA = DUA = DAC = DPA = DFA = EVAL_INT;
	COM_DEW = COM_DEW_ = COM_OUT = COM_OUT_ = COM_DEP	= COM_DEP_ = COM_DEL = COM_DEL_ = COM_UF = COM_UF_ = 
					DKW = DKZ = DK = DE = EVAL_BOOL;
	SYS.messDebug("/IVE452HS02/TMPL",tr("Error response")+": "+t_err);
	f_err = t_err;
}
else f_err = "0";','','',1509285275);
INSERT INTO "tmplib_DevLib" VALUES('OPTRIS','OPTRIS CT/CTL','','','OPTRIS CT/CTL communication interface of "Optris" GmbH, from Berlin.
Author: Roman Savochenko <rom_as@oscada.org>
Sponsored: Vasiliy Grigoriev from "Vacuum technologies laboratory (http://e-beam.ru)".
Version: 1.0.0','','',10,0,'JavaLikeCalc.JavaScript
//Set transport
if(f_start) {
	transport_ = transport;
	Tproc = Thead = Tbox = Tact = eps = eps_ = trans = trans_ = EVAL_REAL;
	spIll = spIll_ = EVAL_BOOL;
	tr = SYS.Transport.nodeAt(transport, ".");
}

//Check for the transport change and connect
if(!tr || transport != transport_)	{
	tr = SYS.Transport.nodeAt(transport, ".");
	transport_ = transport;
}
if(!tr)	t_err = "1:"+tr("Output transport ''%1'' error.").replace("%1",transport);
else {
	req = SYS.XMLNode("mess").setAttr("ProtIt", "OPTRIS").setAttr("addr", addr).setAttr("cs", "1");

	//Check and write
	// Epsilon
	if(eps != eps_ && !(t_err=req.attr("err")).length) {
		epsH = eps*1000;
		req.setAttr("err","2:"+tr("No connection")).setText(SYS.strFromCharCode(0x84,epsH/256,epsH%256));
		tr.messIO(req, "UserProtocol");
	}
	// Transmission
	if(trans != trans_ && !(t_err=req.attr("err")).length) {
		transH = trans*1000;
		req.setAttr("err","2:"+tr("No connection")).setText(SYS.strFromCharCode(0x85,transH/256,transH%256));
		tr.messIO(req, "UserProtocol");
	}
	// Spot illumination
	if(spIll != spIll_ && !(t_err=req.attr("err")).length) {
		req.setAttr("err","2:"+tr("No connection")).setText(SYS.strFromCharCode(0xA5,spIll));
		tr.messIO(req, "UserProtocol");
	}

	//Read data
	req.setAttr("cs", "0");
	// Tproc
	if(!(t_err=req.attr("err")).length) {
		req.setAttr("err","2:"+tr("No connection")).setText(SYS.strFromCharCode(0x01));
		tr.messIO(req, "UserProtocol");
		Tproc = (!(t_err=req.attr("err")).length && req.text().length >= 2) ? (req.text().charCodeAt(0)*256+req.text().charCodeAt(1)-1000)/10 : EVAL_REAL;
	}
	// Thead
	if(!(t_err=req.attr("err")).length) {
		req.setAttr("err","2:"+tr("No connection")).setText(SYS.strFromCharCode(0x02));
		tr.messIO(req, "UserProtocol");
		Thead = (!(t_err=req.attr("err")).length && req.text().length >= 2) ? (req.text().charCodeAt(0)*256+req.text().charCodeAt(1)-1000)/10 : EVAL_REAL;
	}
	// Tbox
	if(!(t_err=req.attr("err")).length) {
		req.setAttr("err","2:"+tr("No connection")).setText(SYS.strFromCharCode(0x03));
		tr.messIO(req, "UserProtocol");
		Tbox = (!(t_err=req.attr("err")).length && req.text().length >= 2) ? (req.text().charCodeAt(0)*256+req.text().charCodeAt(1)-1000)/10 : EVAL_REAL;
	}
	// Tact
	if(!(t_err=req.attr("err")).length) {
		req.setAttr("err","2:"+tr("No connection")).setText(SYS.strFromCharCode(0x81));
		tr.messIO(req, "UserProtocol");
		Tact = (!(t_err=req.attr("err")).length && req.text().length >= 2) ? (req.text().charCodeAt(0)*256+req.text().charCodeAt(1)-1000)/10 : EVAL_REAL;
	}
	// IR epsilon
	if(!(t_err=req.attr("err")).length) {
		req.setAttr("err","2:"+tr("No connection")).setText(SYS.strFromCharCode(0x04));
		tr.messIO(req, "UserProtocol");
		eps = eps_ = (!(t_err=req.attr("err")).length && req.text().length >= 2) ? (req.text().charCodeAt(0)*256+req.text().charCodeAt(1))/1000 : EVAL_REAL;
	}
	// IR transmission
	if(!(t_err=req.attr("err")).length) {
		req.setAttr("err","2:"+tr("No connection")).setText(SYS.strFromCharCode(0x05));
		tr.messIO(req, "UserProtocol");
		trans = trans_ = (!(t_err=req.attr("err")).length && req.text().length >= 2) ? (req.text().charCodeAt(0)*256+req.text().charCodeAt(1))/1000 : EVAL_REAL;
	}
	// Spot illumination
	if(!(t_err=req.attr("err")).length) {
		req.setAttr("err","2:"+tr("No connection")).setText(SYS.strFromCharCode(0x25));
		tr.messIO(req, "UserProtocol");
		spIll = spIll_ = (!(t_err=req.attr("err")).length && req.text().length >= 1) ? req.text().charCodeAt(0) : EVAL_BOOL;
	}
}

//Error set
if(t_err.length) {
	Tproc = Thead = Tbox = Tact = eps = eps_ = trans = trans_ = spIll = spIll_ = EVAL_REAL;
	spIll = spIll_ = EVAL_BOOL;
	SYS.messDebug("/OPTRIS/TMPL",tr("Error response")+": "+t_err);
	f_err = t_err;
}
else f_err = "0";','','',1509908064);
INSERT INTO "tmplib_DevLib" VALUES('CTR','CTR 100, 101','','','The RS232C Serial Interface permits the communication between the digital Oerlikon Leybold Vacuum CERAVAC, from Köln.
Author: Roman Savochenko <rom_as@oscada.org>
Sponsored: Vasiliy Grigoriev from "Vacuum technologies laboratory (http://e-beam.ru)".
Version: 1.0.0','','',10,0,'JavaLikeCalc.JavaScript
//Set transport
if(f_start) {
	transport_ = transport;
	press = FSR_Mantissa = FSR_Exp = EVAL_REAL;
	tr = SYS.Transport.nodeAt(transport, ".");
}

u_err = "";

//Check for the transport change and connect
if(!tr || transport != transport_)	{
	tr = SYS.Transport.nodeAt(transport, ".");
	transport_ = transport;
}
if(!tr)	t_err = "1:"+tr("Output transport ''%1'' error.").replace("%1",transport);
else {
	req = SYS.XMLNode("mess").setAttr("ProtIt","CTR");

	//Read data
	// F.S.R Mantisa
	if(!(t_err=req.attr("err")).length && FSR_Mantissa.isEVal()) {
		req.setAttr("err","2:"+tr("No connection")).setText(SYS.strFromCharCode(0x00,0x39,0x00));
		tr.messIO(req, "UserProtocol");
		if(!(t_err=req.attr("err")).length) {
			if((cV=req.text().charCodeAt(5)) == 0) FSR_Mantissa = 1.0;
			else if(cV == 1) FSR_Mantissa = 1.1;
			else if(cV == 2) FSR_Mantissa = 2.0;
			else if(cV == 3) FSR_Mantissa = 2.5;
			else if(cV == 4) FSR_Mantissa = 5.0;
		}
	}
	// F.S.R Exp
	if(!(t_err=req.attr("err")).length && FSR_Exp.isEVal()) {
		req.setAttr("err","2:"+tr("No connection")).setText(SYS.strFromCharCode(0x00,0x38,0x00));
		tr.messIO(req, "UserProtocol");
		if(!(t_err=req.attr("err")).length) {
			if((cV=req.text().charCodeAt(5)) == 2) FSR_Exp = -1;
			else if(cV == 3) FSR_Exp = 0;
			else if(cV == 4) FSR_Exp = 1;
			else if(cV == 5) FSR_Exp = 2;
			else if(cV == 6) FSR_Exp = 3;
		}
	}
	// Main value
	if(!(t_err=req.attr("err")).length && !FSR_Mantissa.isEVal() && !FSR_Exp.isEVal()) {
		req.setAttr("err","2:"+tr("No connection")).setText(SYS.strFromCharCode(0x00,0x00,0x00));
		tr.messIO(req, "UserProtocol");
		if(!(t_err=req.attr("err")).length) {
			press = ((req.text().charCodeAt(3)*256+req.text().charCodeAt(4))/32000)*FSR_Mantissa*pow(10,FSR_Exp);
			//  Error process
			cV = req.text().charCodeAt(2);
			if(cV&0x7F) {
				if(cV&0x01)	u_err += tr("RS232 synchronization error. ");
				if(cV&0x02)	u_err += tr("Incorrect command, e.g. inadmissible address (syntax error). ");
				if(cV&0x04)	u_err += tr("Inadmissible read command. ");
				if(cV&0x08)	u_err += tr("SP1 status. ");
				if(cV&0x10)	u_err += tr("SP2 status. ");
			}
			else {
				//   Extended Error L-Byte
				req.setAttr("err","2:"+tr("No connection")).setText(SYS.strFromCharCode(0x00,0x36,0x00));
				tr.messIO(req, "UserProtocol");
				if(!(t_err=req.attr("err")).length) {
					cV = req.text().charCodeAt(5);
					if(cV&0x01)	u_err += tr("Atm. pressure out of range. ");
					if(cV&0x02)	u_err += tr("Temperature out of range. ");
					if(cV&0x10)	u_err += tr("Cal. mode wrong. ");
					if(cV&0x20)	u_err += tr("Pressure underflow. ");
					if(cV&0x40)	u_err += tr("Pressure overflow. ");
					if(cV&0x80)	u_err += tr("Zero adjust warning. ");
				}
				//   Extended Error H-Byte
				req.setAttr("err","2:"+tr("No connection")).setText(SYS.strFromCharCode(0x00,0x37,0x00));
				tr.messIO(req, "UserProtocol");
				cV = req.text().charCodeAt(5);
				if(cV&0x01)	u_err += tr("PT1000 fault (CTR 101 only). ");
				if(cV&0x02)	u_err += tr("Heaterblock overtemp. ");
				if(cV&0x04)	u_err += tr("Electronic overtemp. ");
				if(cV&0x08)	u_err += tr("Zero adjust error. ");
			}
		}
	}

	//Check and write commands
	// Zero-adjust
	if(!(t_err=req.attr("err")).length && zeroSet) {
		req.setAttr("err", "2:"+tr("No connection")).setText(SYS.strFromCharCode(0x40,0x02,0x00));
		tr.messIO(req, "UserProtocol");
		if(!(t_err=req.attr("err")).length) zeroSet = false;
	}

	//Check and set poll mode
	if((t_err=req.attr("err")).length && t_err.toInt() == 4) {
		req.setAttr("err", "2:"+tr("No connection")).setText(SYS.strFromCharCode(0x10,0x00,0x01));
		tr.messIO(req, "UserProtocol");
	}
}

if(t_err.length) {
	press = EVAL_REAL;
	f_err = t_err + " " + u_err;
}
else f_err = "0: " + u_err;','','',1509287409);
INSERT INTO "tmplib_DevLib" VALUES('IEC60870','IEC-60870','','','IEC 60870 part 5 is one of the IEC 60870 set of standards which define systems used for telecontrol (supervisory control and data acquisition) in electrical engineering and power system automation applications. Part 5 provides a communication profile for sending basic telecontrol messages between two systems, which uses permanent directly connected data circuits between the systems. The template implements part 104 (Ethernet transport) for client and followed services: STARTDT, STOPDT, TESTFR, Ack, C_IC_NA_1, C_CI_NA_1, C_SC_NA_1, M_SP_NA_1, M_ME_NB_1, C_CS_NA_1. For acquired and control data primarily used an object into attribute "items" for next the control as the object with the data provide as table, alarming and allowing set writable attributes. To the data control by attributes at once you can its describe into "itemsSet". Into the template for the first time used the non request mode of an output transport and free attributes creation wile performing.
Author: Roman Savochenko <rom_as@oscada.org>
Sponsored: Ustijancev Michael.
Version: 1.0.2','','',10,0,'JavaLikeCalc.JavaScript
if(f_start)	{
	transport_ = transport;
	tr = SYS.Transport.nodeAt(transport,".");
	if(tr) tr.start(false);
	itemsSet_ = "";
	tmRetr_ = tmRetr;
	t1_ = t1__ = t2_ = t3_ = w_ = 0;
	TESTFR_Act = false;
	STARTDT_act = STARTDT_con = false;
	bufIn = bufOut = "";
	cntTx = cntRx = 0;
	destAddr = addr.parse(0,".").toInt();
	destOA = addr.parse(1,".").toInt();
	//C_IC_NA_1_req = 0;
	C_IC_NA_1 = C_SC_NA_1 = 0;
	C_IC_NA_1con = C_CS_NA_1 = C_CS_NA_1con = C_CI_NA_1 = C_CI_NA_1con = false;
	items = new Object();
	oAVals = new Object();
}

//Items set changing process
if(itemsSet != itemsSet_) {
	for(off = 0; (iIt=itemsSet.parse(0,"\n",off)).length; ) {
		iIt_tp = iIt.parse(0, ":");
		iIt_IOA = iIt.parse(1, ":");
		iIt_flgs = iIt.parse(2, ":");
		iIt_nmBase = iIt.parse(3, ":");
		iIt_sDscr = false;
		if((iIt_EndIOA=iIt_IOA.indexOf("-")) >= 0) {
			iIt_EndIOA = iIt_IOA.slice(iIt_EndIOA+1).toInt();
			iIt_IOA = iIt_IOA.slice(0,iIt_EndIOA).toInt();
		}
		else { iIt_IOA = iIt_EndIOA = iIt_IOA.toInt(); iIt_sDscr = iIt_nmBase.length; }
		if(iIt_tp == "ai")			{ iIt_nmBase = iIt_nmBase.length ? iIt_nmBase : "AI"; iIt_vtp = "integer,ro"; }
		else if(iIt_tp == "di")	{ iIt_nmBase = iIt_nmBase.length ? iIt_nmBase : "DI"; iIt_vtp = "boolean,ro"; }
		else if(iIt_tp == "do")	{ iIt_nmBase = iIt_nmBase.length ? iIt_nmBase : "DO"; iIt_vtp = "boolean"; }
		else continue;
		while(iIt_IOA <= iIt_EndIOA) {
			aId = iIt_tp+iIt_IOA; aDscr = iIt_nmBase+(iIt_sDscr?"":"["+iIt_IOA+"]"); aWr = (iIt_tp == "do");
			if(items[aId].isEVal()) { items[aId] = itW = new Object(); itW.descr = aDscr; itW.wr = aWr; itW.alarm = 0; }
			if(iIt_flgs.indexOf("a") >= 0) {
				this.attrAdd(aId, aDscr, iIt_vtp);
				if(aWr)	oAVals[aId] = EVAL_INT;
				this[aId].set(EVAL_INT, 0, 0, true);
			}
			iIt_IOA++;
		}
	}
	itemsSet_ = itemsSet;
}

//Check for the transport change and connect
t_err = "";
if(!tr || transport != transport_)	{
	tr = SYS.Transport.nodeAt(transport, ".");
	transport_ = transport;
	if(tr) tr.start(false);
	itemsSet_ = "";
	tmRetr_ = tmRetr;
	STARTDT_act = false;
	items = new Object();
	oAVals = new Object();
}
if(!tr)	t_err = "1:"+tr("Output transport ''%1'' error.").replace("%1",transport);
else if(!tr.start()) {
	if(tmRetr_ >= tmRetr) {
		tr.start(true);
		bufIn = bufOut = itemsSet_ = "";
		tmRetr_ = t1_ = t1__ = t2_ = t3_ = w_ = 0;
	}
	else tmRetr_ += 1/f_frq;
	if(tr.start()) STARTDT_act = false;
	else t_err = "1:"+tr("Output transport ''%1'' stoped.").replace("%1",transport);
}
else {
	//Check and finish, U (STOPDT, Act)
	if(f_stop && STARTDT_con) bufOut += SYS.strFromCharCode(0x68, 0x04, 0x10|0x03, 0, 0, 0);

	//Send output buffer and read the transport for an input data
	if(bufOut.length) SYS.messDebug("/IEC60870",tr("Good output sequence")+": "+SYS.strDecode(bufOut,"Bin"," "));
	bufIn += tr.messIO(bufOut, -0.001);
	bufOut = "";
	//Input requests processing
	while(bufIn.length) {
		if(bufIn.length >= 2 && (bufIn.charCodeAt(0) != 0x68 || (seqSz=bufIn.charCodeAt(1)) > 253)) {
			SYS.messDebug("/IEC60870",tr("Wrong sequence, rejected")+": "+SYS.strDecode(bufIn,"Bin"," "));
			bufIn = "";
			break;
		}
		if(bufIn.length < 6 || (bufIn.length-2) < seqSz)	break;	//Not full, wait

		SYS.messDebug("/IEC60870",tr("Good input sequence")+": "+SYS.strDecode(bufIn.slice(0,seqSz+2),"Bin"," "));

		cntRx_ = -1;
		sB1 = bufIn.charCodeAt(2);
		if((sB1&0x3) == 0x3 && (sB1&0x40))			TESTFR_Act = true;	//U (TESTFR, Act)
		else if((sB1&0x3) == 0x3 && (sB1&0x80))	t1__ = 0;					//U (TESTFR, Con)
		else if((sB1&0x3) == 0x3 && (sB1&0x08)) {	//U (STARTDT, Con)
			STARTDT_con = true;
			t1_ = t1__ = t2_ = t3_ = 0;
			cntTx = cntRx = 0;
			destAddr = addr.parse(0,".").toInt();
			destOA = addr.parse(1,".").toInt();
			C_IC_NA_1 = C_SC_NA_1 = 0;
			C_CS_NA_1 = C_CI_NA_1 = false;
		}
		else if(STARTDT_con && (sB1&0x3) == 1)	cntRx_ = (bufIn.charCodeAt(5)>>1)*128 + (bufIn.charCodeAt(4)>>1);		//S (Ack)
		else if(STARTDT_con && (sB1&0x1) == 0) {	//I
			cntTx_ = bufIn.charCodeAt(3)*128 + (sB1>>1);
			if(cntTx_ != cntRx)
				t_err = "2:"+tr("Connection terminated by the input sequence broken, lost input packages, S(R)=%1, S(T)=%2").replace("%1",cntRx.toString()).replace("%2",cntTx_.toString());
			cntRx_ = bufIn.charCodeAt(5)*128 + (bufIn.charCodeAt(4)>>1);
			ASDU_id = bufIn.charCodeAt(6);
			ASDU_els = bufIn.charCodeAt(7);			//7=SQ, 0...6=number
			ASDU_reas = bufIn.charCodeAt(8);		//7=TEST, 6=P/N, 0...5=number
			ASDU_addr = bufIn.charCodeAt(9);
			ASDU_OA = bufIn.charCodeAt(11)*256 + bufIn.charCodeAt(10);
			//destAddr = ASDU_addr, destOA = ASDU_OA;
			cntRx = (cntRx+1)&0x7FFF;
			t2_ = 1/f_frq;	
			w_++;

			if(ASDU_id == 100) {
				if(ASDU_reas == 7)			C_IC_NA_1con = true;	//ActCon
				else if(ASDU_reas == 10)	C_IC_NA_1 = -1;// 1;				//ActTerm, repeate after 1 second
			}
			else if(ASDU_id == 101) {
				if(ASDU_reas == 7)			C_CI_NA_1con = true;	//ActCon
				//else if(ASDU_reas == 10)	C_CI_NA_1 = false;		//ActTerm, repeate after some timeout
			}
			else if(ASDU_id == 103 && ASDU_reas == 7)	C_CS_NA_1con = true;
			else if(ASDU_id == 1) {	//M_SP_NA_1, Single-point information
				if(ASDU_els&0x80) SYS.messDebug("/IEC60870",tr("M_SP_NA_1: No the SQ mode implemented yet"));
				else if((10+ASDU_els*4) != seqSz)	SYS.messDebug("/IEC60870",tr("M_SP_NA_1: Items number discrepancy to the package size"));
				else for(iEl = 0, iOff = 12; iEl < ASDU_els; iEl++, iOff+=4) {
					IOA = (bufIn.charCodeAt(iOff+2)<<16) + (bufIn.charCodeAt(iOff+1)<<8) + bufIn.charCodeAt(iOff);
					SIQ = bufIn.charCodeAt(iOff+3);
					aid = "di"+IOA;
					if(items[aid].isEVal()) {
						items[aid] = itW = new Object();
						itW.descr = "DI["+IOA+"]";
						itW.wr = false; itW.alarm = 0;
					}
					items[aid].val = SIQ&0x01;
					items[aid].SIQ = SIQ;	//For a specific quality processing
					if((aO=this[aid])) aO.set(items[aid].val, 0, 0, true);
				}
			}
			else if(ASDU_id == 11) {	//M_ME_NB_1, Measured value, scaled value
				if(ASDU_els&0x80) SYS.messDebug("/IEC60870",tr("M_ME_NB_1: No the SQ mode implemented yet"));
				else if((10+ASDU_els*6) != seqSz)	SYS.messDebug("/IEC60870",tr("M_ME_NB_1: Items number discrepancy to the package size"));
				else for(iEl = 0, iOff = 12; iEl < ASDU_els; iEl++, iOff+=6) {
					IOA = (bufIn.charCodeAt(iOff+2)<<16) + (bufIn.charCodeAt(iOff+1)<<8) + bufIn.charCodeAt(iOff);
					val = (bufIn.charCodeAt(iOff+4)<<8) + bufIn.charCodeAt(iOff+3);
					if(val > 32767) val -= 65536;
					SIQ = bufIn.charCodeAt(iOff+5);
					aid = "ai"+IOA;

					if(items[aid].isEVal()) {
						items[aid] = itW = new Object();
						itW.descr = "AI["+IOA+"]";
						itW.wr = false; itW.alarm = 0;
					}
					items[aid].val = val;
					items[aid].SIQ = SIQ;	//For a specific quality processing
					if((aO=this[aid])) aO.set(items[aid].val, 0, 0, true);
				}
			}
			else if(ASDU_id == 45) {	//C_SC_NA_1, Single-point confirm
				if(ASDU_els&0x80) SYS.messDebug("/IEC60870", tr("C_SC_NA_1: No the SQ mode implemented yet"));
				else if((10+ASDU_els*4) != seqSz)	SYS.messDebug("/IEC60870", tr("C_SC_NA_1: Items number discrepancy to the package size"));
				else for(iEl = 0, iOff = 12; iEl < ASDU_els; iEl++, iOff+=4) {
					IOA = (bufIn.charCodeAt(iOff+2)<<16) + (bufIn.charCodeAt(iOff+1)<<8) + bufIn.charCodeAt(iOff);
					SIQ = bufIn.charCodeAt(iOff+3);
					aid = "do" + IOA;
					items[aid].val = SIQ&0x01;
					items[aid].SIQ = SIQ;	//For a specific quality processing
					if((aO=this[aid])) { aO.set(items[aid].val, 0, 0, true); oAVals[aid] = items[aid].val; }
				}
				C_SC_NA_1 = 0;
			}
			//else ...
		}
		if(cntRx_ >= 0 && cntRx_ == cntTx) t1_ = 0;

		t3_ = 0;
		bufIn = bufIn.slice(seqSz+2);
	}
	//Check and the service negotiation perform.
	while(!t_err.length) {
		if(TESTFR_Act) {
			bufOut += SYS.strFromCharCode(0x68, 0x04, 0x80|0x03, 0, 0, 0);
			TESTFR_Act = false;
		}
		else if(!STARTDT_act)	{	//U (STARTDT, Act);
			bufOut += SYS.strFromCharCode(0x68, 0x04, 0x04|0x03, 0, 0, 0);
			STARTDT_act = true; STARTDT_con = false;
		}
		else if(t1_ > t1)
			t_err = "3:" + tr("Connection terminated by not acknowledge transmited package ''%1'' by timeout t1 (%2).").replace("%1",cntTx.toString()).replace("%2",t1.toFixed(2));
		else if(t1__ > t1)
			t_err = "4:" + tr("Connection terminated by not confirmed TEST package by timeout t1 (%1).").replace("%1",t1.toFixed(2));
		else if(t3_ > t3) {						//U (TESTFR, Act);
			bufOut += SYS.strFromCharCode(0x68, 0x04, 0x40|0x03, 0, 0, 0);
			t3_ = 0; t1__ = 1/f_frq;
		}
		else if(t2_ > t2 || w_ >= w) {	//S (Ack)
			bufOut += SYS.strFromCharCode(0x68, 0x04, 0x01, 0, (cntRx<<1)&0xFF, cntRx>>7);
			t2_ = w_ = 0;
		}
		else if(STARTDT_con /*&& destAddr >= 0*/) {	//I
			ASDU_ = "";
			// Services process
			if(C_IC_NA_1 > 0)	C_IC_NA_1 = max(0, C_IC_NA_1-1/f_frq);
			else if(C_IC_NA_1 == 0) {
				//Generic acquisition request (20)
				ASDU_ = SYS.strFromCharCode(100, 0x01, 0x06, destAddr, destOA&0xFF, destOA>>8, 0x00, 0x00, 0x00, 0x14);
				C_IC_NA_1 = -1; C_IC_NA_1con = false;
			}
			else if(!C_CI_NA_1) {
				//Generic counters request (5)
				ASDU_ = SYS.strFromCharCode(101, 0x01, 0x06, destAddr, destOA&0xFF, destOA>>8, 0x00, 0x00, 0x00, 0x05);
				C_CI_NA_1 = true; C_CI_NA_1con = false;
			}
			else if(!C_CS_NA_1) {
				//Time sync
				tmMs = 0; tm = SYS.time(tmMs); tmMs = (tm%60)*1000+tmMs/1000;
				tmMin = tmHour = tmDay = tmMonth = tmYear = tmWDay = tmIsDst = 0;
				SYS.localtime(tm, 0, tmMin, tmHour, tmDay, tmMonth, tmYear, tmWDay, 0, tmIsDst);
				ASDU_ = SYS.strFromCharCode(103, 0x01, 0x06, destAddr, destOA&0xFF, destOA>>8, 0, 0, 0, tmMs&0xFF, tmMs>>8, tmMin, tmHour|(tmIsDst?0x80:0), tmDay|((tmWDay?tmWDay:7)<<5), tmMonth+1, tmYear-2000);
				C_CS_NA_1 = true; C_CS_NA_1con = false;
			}
			else if((C_SC_NA_1=max(0,C_SC_NA_1-1/f_frq)) == 0) {
				//Check for commands set, C_SC_NA_1 (45)
				cItN = 0; cItSeq = "";
				// By attributes
				for(iIt in oAVals)
					if((aO=this[iIt]) && !(aOval=aO.get()).isEVal() && aOval != oAVals[iIt] && !cItN) {
						IOA = iIt.slice(2).toInt();
						cItSeq += SYS.strFromCharCode(IOA&0xFF, (IOA>>8)&0xFF, IOA>>16, aOval?0x01:0);
						//oAVals[iIt] = aOval;
						cItN++;
					}
				// By items object
				for(iIt in items)
					if((aO=items[iIt]) && !(aOval=aO.set).isEVal() && !cItN) {
						IOA = iIt.slice(2).toInt();
						cItSeq += SYS.strFromCharCode(IOA&0xFF, (IOA>>8)&0xFF, IOA>>16, aOval.toInt()?0x01:0);
						aO.set = EVAL_BOOL;
						cItN++;
					}
				// Finish the request
				if(cItN) {
					ASDU_ = SYS.strFromCharCode(45, cItN, 0x06, destAddr, destOA&0xFF, destOA>>8) + cItSeq;
					C_SC_NA_1 = 1;	//After one second repeat
				}
			}

			// Append header and place to output
			if(ASDU_.length) {
				bufOut += SYS.strFromCharCode(0x68, 4+ASDU_.length, (cntTx<<1)&0xFF, cntTx>>7, (cntRx<<1)&0xFF, cntRx>>7) + ASDU_;
				cntTx = (cntTx+1)&0x7FFF;
				t1_ = 1/f_frq;
				//t2_ = w_ = 0;
			} else break;
		}
		else break;
	}

	if(t1_)		t1_ += 1/f_frq;
	if(t1__)	t1__ += 1/f_frq;
	if(t2_)		t2_ += 1/f_frq;
	t3_ += 1/f_frq;
}

//Error set
if(t_err.length) {
	if(tr && tr.start()) tr.start(false);
	if(f_err != t_err) {
		items = new Object();
		oAVals = new Object();
		aLs = this.nodeList("a_");
		for(iA = 0; iA < aLs.length; iA++)
			if((aPref=aLs[iA].slice(2,4)) == "di" || aPref == "do" || aPref == "ai")
				this[aLs[iA].slice(2)].set(EVAL_INT, 0, 0, true);
		SYS.messDebug("/IEC60870", tr("Error")+": "+t_err);
	}
	f_err = t_err;
}
else f_err = "0";','','',1509288456);
INSERT INTO "tmplib_DevLib" VALUES('SSCP','Shark Slave Communication Protocol','','','Shark Slave Communication Protocol from EnergoCentrum PLUS, s.r.o.
Author: Roman Savochenko <rom_as@oscada.org>
Sponsored: Costumer Faster CZ (http://faster.cz)
Version: 0.6.2','','',30,0,'JavaLikeCalc.JavaScript
//Same request to the device
function req(tr, addr, func, data) {
	req = SYS.strFromCharCode(addr, (func>>8)&0xFF, func&0xFF, (data.length>>8)&0xFF, data.length&0xFF) + data;
	//SYS.messInfo("/SSCP",tr("Request")+": "+SYS.strDecode(req,"Bin"," "));
	resp = tr.messIO(req);
	while(resp.length && (resp.length < 5 || (resp.length-5) < ((resp.charCodeAt(3)<<8)|resp.charCodeAt(4))) &&
		(tresp=tr.messIO("")).length) resp += tresp;
	//SYS.messInfo("/SSCP",tr("Response")+": "+SYS.strDecode(resp,"Bin"," "));
	data = resp;
	if(resp.length < 5)
		return tr("3:No a response or the response short.");
	if((resp.length-5) < ((resp.charCodeAt(3)<<8)|resp.charCodeAt(4)))
		return tr("3:Incomplete response.");
	if(resp.charCodeAt(0) != addr)	return tr("4:Invalid host''s address received.");
	func_ = ((resp.charCodeAt(1)&0xFF)<<8) | resp.charCodeAt(2);
	if(func_ == 0xfffe)						return tr("4:Unknown function.");
	if(func_ == 0xffff)						return tr("4:Insufficient rights.");
	if((func_&0x3FFF) != func)			return tr("4:Invalid function received.");
	if((func_&0xC000) != 0x8000) {
		opErr = tr("5:Operation error.");
		if(resp.length >= 9) {
			errCod = (resp.charCodeAt(5)<<24) | (resp.charCodeAt(6)<<16) | (resp.charCodeAt(7)<<8) | resp.charCodeAt(8);
			if(errCod == 0x0101)		opErr += " "+tr("Wrong login.");
			else if(errCod == 0x103)	opErr += " "+tr("No such variable.");
			else if(errCod == 0x112)	opErr += " "+tr("Size mismatch.");
			//... Append here for needs
			else opErr += " "+tr("Unknown error %1.").replace("%1",errCod.toString());
		}
		if(resp.length >= 17)	opErr += " "+tr("Destination:")+" "+SYS.strDecode(resp.slice(9),"Bin","")+"h";
		return opErr;
	}
	data = resp.slice(5);
	return "0";
}

//Set transport and init
if(f_start) {
	transport_ = transport;
	tr = SYS.Transport.Sockets.nodeAt("out_"+transport);
	vlist = new Object();
	prcVList = new Array();
	isLogin = false;
	toLoadVarLsts = true;
	aSelList = false;
	list_ = "";
}
if(f_stop) return;

t_err = "0";

//Check for the transport change and connect
if(!tr || transport != transport_)	{
	tr = SYS.Transport.Sockets.nodeAt("out_"+transport);
	transport_ = transport;
}
if(!tr)	t_err = "1:"+tr("Output transport ''%1'' error.").replace("%1",transport);
else if(addr < 0 || addr > 255)
	t_err = "2:"+tr("Address ''%1'' out of range [0...255].").replace("%1",addr.toString());
else {
	//Generic information update.
	if(toLoadVarLsts) {
		toLoadVarLsts = false;
		fLs = SYS.system("ls -1 "+listsDir+"/*.vlist");
		for(off = 0; (fln=fLs.parse(0,"\n",off)).length; ) {
			//stTm = SYS.time();
			fl = SYS.fileRead(fln);
			//fl = fl.split("\x0D\x0A");
			//for(iL = 0; iL < fl.length; iL++) {
				//ln = fl[iL];
			for(off1 = 0; (ln=fl.parse(0,"\x0D\x0A",off1)).length; ) {
			//for(off1 = 0; (ln=Special.FLibSYS.strParse(fl,0,"\x0D\x0A",off1)).length; ) {
				off2 = 0;
				vid = ln.parse(0, ";", off2);
				if(vid == "Project" || !(tVl=ln.parse(0,";",off2)).length) continue;
				vid += ":"+tVl;
				//SYS.messInfo("SSCP","vid="+vid);
				vlist[vid] = vO = new Object();
				vO.Type = ln.parse(0, ";", off2);
				vO.CommUid = ln.parse(0, ";", off2).toInt();
				vO.Offset = ln.parse(0, ";", off2).toInt();
				vO.Length = ln.parse(0, ";", off2).toInt();
				vO.ParentTypeFamily = ln.parse(0, ";", off2);
				vO.HistoryId = ln.parse(0, ";", off2);
			}
			//SYS.messInfo("SSCP", "Time="+(SYS.time()-stTm)+"s; length="+fl.length);
		}
		fl = selLst = "";
		//for(tVl in vlist) selLst += tVl+";";
		//this.attrAdd("selList", tr("Appending variable"), "string|sel", selLst+"\n"+selLst);
		//aSelList = this.selList;
		//aSelList.set("");
		for(tVl in vlist) selLst += tVl+"\n";
		this.attrAdd("selList", tr("Allowed variables"), "text|ro");
		aSelList = this.selList;
		aSelList.set(selLst,0,0,true);
	}
	//Check for select a varable and append it to the processing list
	/*if(aSelList && (tVl=aSelList.get()).length) {
		if(list.indexOf(tVl) == -1)
			list += ((list.length && list[list.length-1] != "\n")?"\n":"")+ tVl+"\n";
		aSelList.set("");
	}*/
	//The processing variables list update/form
	if(list != list_) {
		list_ = list;
		prcVList = new Array();
		for(off = 0; (vid=list.parse(0,"\n",off)).length; ) {
			if((vO=vlist[vid]).isEVal()) continue;
			prcVList.push(vO);
			vO.aId = vid.replace(new RegExp("[\\.\\:]+","g"),"_").replace(new RegExp("[\\$\\-]+","g"),"");
			vTp = "string";
			if(vO.Type.indexOf("bool") != -1)			vTp = "boolean";
			else if(vO.Type.indexOf("real") != -1)	vTp = "real";
			else if(vO.Type.indexOf("int") != -1)		vTp = "integer";
			this.attrAdd(vO.aId, vid, vTp+"|ro");
			vO.aO = this[vO.aId];
			//SYS.messInfo("/SSCP","aId="+vO.aId);
		}
	}

	//Same requests
	if(!isLogin) {
		io = SYS.strFromCharCode(0x01, (maxDtFrm>>8)&0xFF, maxDtFrm&0xFF, user.length) +
				user + SYS.strFromCharCode(0x10) + Special.FLibSYS.MD5(pass);
		if(!(t_err=req(tr,addr,0x0100,io)).toInt()) {
			verPrt = io.charCodeAt(0);
			maxDtFrmServ = (io.charCodeAt(1)<<8) | io.charCodeAt(2);
			isLogin = true;
		}
	}
	if(isLogin && !t_err.toInt()) {
		for(iV0 = iV1 = 0; iV0 < prcVList.length; iV0 = iV1) {
			frmSz = frmSzServ = 0;
			io = Special.FLibSYS.IO("", "", "b");
			io.wr(0x80);
			for(iV1 = iV0; iV1 < prcVList.length; iV1++) {
				vO = prcVList[iV1];
				frmSz += vO.Length; frmSzServ += 12;
				if(frmSz >= maxDtFrm || frmSzServ >= maxDtFrmServ) break;
				io.wr(vO.CommUid, "uint32").wr(vO.Offset, "uint32").wr(vO.Length, "uint32");
			}
			//SYS.messInfo("/SSCP","req="+SYS.strDecode(io.string,"Bin"," "));
			t_err = req(tr, addr, 0x0500, io.string); io.pos = 0;
			//SYS.messInfo("/SSCP","resp="+SYS.strDecode(io.string,"Bin"," "));
			for(iV2 = iV0; iV2 < iV1; iV2++) {
				vO = prcVList[iV2];
				//SYS.messInfo("/SSCP","aId="+vO.aId+"; pos="+io.pos);
				if(t_err.toInt()) tVl = EVAL;
				else {
					if(vO.Type.indexOf("bool") != -1 && vO.Length == 1)
						tVl = io.read("int8", 1);
					else if(vO.Type.indexOf("real") != -1 && (vO.Length == 4 || vO.Length == 8))
						tVl = io.read((vO.Length==8)?"double":"float", 1);
					else if(vO.Type.indexOf("int") != -1 && (vO.Length == 2 || vO.Length == 4))
						tVl = io.read((vO.Length==4)?"int32":"int16", 1);
					else if(vO.Type.indexOf("dt") != -1 && vO.Length == 8) {
						tVl = io.read("int64", 1);
						tVl = SYS.strftime(tVl*1e-7-62135596800) + "." + (tVl%10000000).toString(10,7);
					}
					else if(vO.Type.indexOf("time") != -1 && vO.Length == 8) {
						tm = io.read("int64",1)*1e-7;
						lev = 0;
						days = floor(tm/(24*60*60));
						hours = floor(tm/(60*60))%24;
						mins = floor(tm/(60))%60;
						usec = 1e6 * (tm - days*24*60*60 - hours*60*60 - mins*60);
						tVl = "";
						if(days)	{ tVl += days+tr("day"); lev = max(lev,6); }
						if(hours)	{ tVl += (tVl.length?" ":"")+hours+tr("hour"); lev = max(lev,5); }
						if(mins && lev < 6)	{ tVl += (tVl.length?" ":"")+mins+tr("min"); lev = max(lev,4); }
						if((1e-6*usec) > 0.5 && lev < 5)		{ tVl += (tVl.length?" ":"")+(1e-6*usec).toPrecision(3)+tr("s"); lev = max(lev,3); }
						else if((1e-3*usec) > 0.5 && !lev)	{ tVl += (tVl.length?" ":"")+(1e-3*usec).toPrecision(4)+tr("ms"); lev = max(lev,2); }
						else if(usec > 0.5 && !lev)				{ tVl += (tVl.length?" ":"")+usec.toPrecision(4)+tr("us"); lev = max(lev,1); }
						else if(!lev)	tVl += (tVl.length?" ":"")+(1e3*usec).toPrecision(4)+tr("ns");
					}
					else tVl = SYS.strDecode(io.read("char", vO.Length), "Bin", " ");;
				}
				vO.aO.set(tVl, 0, 0, true);
			}
		}
	}
}

f_err = t_err;','','',1509375504);
INSERT INTO "tmplib_DevLib" VALUES('m200','Mercury 200','Меркурій 200','Меркурий 200','One phase counter of electricity Mercury 200, 203.2Т, 206 from firm Incotex (http://www.incotexcom.ru).
Author: Arsen Zakojan <godzilla919@gmail.com>
Version: 1.0.0','Однофазні лічильники електроенергії Меркурій 200,  203.2Т,  206 компанії Інкотекс (http://www.incotexcom.ru).
Автор: Арсен Закоян <godzilla919@gmail.com>
Версія 1.0.0','Однофазные счетчики электроэнергии Меркурий 200,  203.2Т,  206 компании Инкотекс (http://www.incotexcom.ru).
Автор: Арсен Закоян <godzilla919@gmail.com>
Версия 1.0.0',600,0,'JavaLikeCalc.JavaScript
if(naladchik)//если счетчик закондирован программой Наладчик+
{first=250;
second=3+(8*netaddr);}
else//если обычный сетевой адрес(с завода 6 последних цифр)
{first= floor(netaddr/16777216);//результат деления с округлением в меньшую сторону
second=netaddr-(first*16777216);}//результат вычисления остатка от деления
if(f_start){;t1= EVAL_STR;t2= EVAL_STR;t3= EVAL_STR;t4= EVAL_STR;s= EVAL_STR;U= EVAL_STR;I= EVAL_STR;P= EVAL_STR;}//обнуляем значения
if(tarif)//если выбрано считывание показаний тарифов
{req = SYS.XMLNode("mess").setAttr("ProtIt","m200").setAttr("first",first).setAttr("second",second).setAttr("data",SYS.strFromCharCode(0x27));}
SYS.Transport["Serial"]["out_"+transport].messIO(req,"UserProtocol");
if(req.text().length <23){ answer = EVAL_STR;t1= EVAL_STR;t2= EVAL_STR;t3= EVAL_STR;t4= EVAL_STR;s= EVAL_STR;}//если нет ответа
else //если ответ получен
{t1 =Special.FLibSYS.str2real( req.text().slice(15,17)+req.text().slice(18,20)+req.text().slice(21,23)+req.text().slice(24,26))/100;
t2 =Special.FLibSYS.str2real( req.text().slice(27,29)+req.text().slice(30,32)+req.text().slice(33,35)+req.text().slice(36,38))/100;
t3 =Special.FLibSYS.str2real( req.text().slice(39,41)+req.text().slice(42,44)+req.text().slice(45,47)+req.text().slice(48,50))/100;
t4 =Special.FLibSYS.str2real( req.text().slice(51,53)+req.text().slice(54,56)+req.text().slice(57,59)+req.text().slice(60,62))/100;
s=t1+t2+t3+t4;}
if(UIP)//{//если выбрано считывание мгновенных показаний
{req = SYS.XMLNode().setAttr("ProtIt","m200").setAttr("first",first).setAttr("second",second).setAttr("data",SYS.strFromCharCode(0x63));
SYS.Transport["Serial"]["out_"+transport].messIO(req,"UserProtocol");}
if(req.text().length <14){U= EVAL_STR;I= EVAL_STR;P= EVAL_STR;}//если нет ответа
else//если ответ получен
{U =Special.FLibSYS.str2real( req.text().slice(15,17)+req.text().slice(18,20))/10;
I =Special.FLibSYS.str2real( req.text().slice(21,23)+req.text().slice(24,26))/100;
P =Special.FLibSYS.str2real( req.text().slice(27,29)+req.text().slice(30,32)+req.text().slice(33,35));}','','',1509290186);
INSERT INTO "tmplib_DevLib" VALUES('m230','Mercury 230','Меркурій 230','Меркурий 230','Three phase counter of electricity Mercury 230, 231, 232, 233, 234, 236 from firm Incotex (http://www.incotexcom.ru).
Author: Arsen Zakojan <godzilla919@gmail.com>
Version: 1.0.0','Трифазні лічильники електроенергії Меркурій 230, 231, 232, 233,  234,  236  компанії Інкотекс (http://www.incotexcom.ru).
Автор: Арсен Закоян <godzilla919@gmail.com>
Версія 1.0','Трехфазные счетчики электроэнергии Меркурий 230, 231, 232, 233,  234,  236  компании Инкотекс (http://www.incotexcom.ru).
Автор: Арсен Закоян <godzilla919@gmail.com>
Версия 1.0',300,0,'JavaLikeCalc.JavaScript
if(f_start)
//очищаем все данные
{answer = EVAL_STR;TSAp  = EVAL_STR;TSAm = EVAL_STR;TSRp = EVAL_STR;TSRm = EVAL_STR;
T1Ap  = EVAL_STR;T1Am = EVAL_STR;T1Rp = EVAL_STR;T1Rm = EVAL_STR;T2Ap  = EVAL_STR;T2Am = EVAL_STR;
T2Rp = EVAL_STR;T2Rm = EVAL_STR; T3Ap  = EVAL_STR;T3Am = EVAL_STR;T3Rp = EVAL_STR;T3Rm = EVAL_STR;
T4Ap  = EVAL_STR;T4Am = EVAL_STR;T4Rp = EVAL_STR;T4Rm = EVAL_STR;
PS = EVAL_STR;P1= EVAL_STR;P2= EVAL_STR;P3= EVAL_STR;U1= EVAL_STR;U2= EVAL_STR;U3= EVAL_STR;
I1= EVAL_STR;I2= EVAL_STR;I3= EVAL_STR;QS= EVAL_STR;Q1= EVAL_STR;Q2= EVAL_STR;Q3= EVAL_STR;
SS= EVAL_STR;S1= EVAL_STR;S2= EVAL_STR;S3= EVAL_STR;KS= EVAL_STR;K1= EVAL_STR;K2= EVAL_STR;
K3= EVAL_STR;F1= EVAL_STR;N1= EVAL_STR;}
//отправка пароля
req = SYS.XMLNode().setAttr("ProtIt","m230").setAttr("netaddr",netaddr).setAttr("data",SYS.strFromCharCode(0x01,0x01,Special.FLibSYS.str2int(password.slice(0,1)),Special.FLibSYS.str2int(password.slice(1,2)),Special.FLibSYS.str2int(password.slice(2,3)),Special.FLibSYS.str2int(password.slice(3,4)),Special.FLibSYS.str2int(password.slice(4,5)),Special.FLibSYS.str2int(password.slice(5,6))));
SYS.Transport["Serial"]["out_"+transport].messIO(req,"UserProtocol");
if(!req.text()) {answer = EVAL_STR;TSAp  = EVAL_STR;TSAm = EVAL_STR;TSRp = EVAL_STR;TSRm = EVAL_STR;
T1Ap  = EVAL_STR;T1Am = EVAL_STR;T1Rp = EVAL_STR;T1Rm = EVAL_STR;T2Ap  = EVAL_STR;T2Am = EVAL_STR;
T2Rp = EVAL_STR;T2Rm = EVAL_STR; T3Ap  = EVAL_STR;T3Am = EVAL_STR;T3Rp = EVAL_STR;T3Rm = EVAL_STR;
T4Ap  = EVAL_STR;T4Am = EVAL_STR;T4Rp = EVAL_STR;T4Rm = EVAL_STR;
PS = EVAL_STR;P1= EVAL_STR;P2= EVAL_STR;P3= EVAL_STR;U1= EVAL_STR;U2= EVAL_STR;U3= EVAL_STR;
I1= EVAL_STR;I2= EVAL_STR;I3= EVAL_STR;QS= EVAL_STR;Q1= EVAL_STR;Q2= EVAL_STR;Q3= EVAL_STR;
SS= EVAL_STR;S1= EVAL_STR;S2= EVAL_STR;S3= EVAL_STR;KS= EVAL_STR;K1= EVAL_STR;K2= EVAL_STR;
K3= EVAL_STR;F1= EVAL_STR;N1= EVAL_STR;return;}
else answer =req.text();
//Энергия от сброса сумма Т
if(TS){
req = SYS.XMLNode().setAttr("ProtIt","m230").setAttr("netaddr",netaddr).setAttr("data",SYS.strFromCharCode(0x05,0x00,0x00));
SYS.Transport["Serial"]["out_"+transport].messIO(req,"UserProtocol");
if(!req.text()){TSAp  = EVAL_STR;TSAm = EVAL_STR;TSRp = EVAL_STR;TSRm = EVAL_STR; }
else {
if(req.text().slice(0,10)!=2147483647)TSAp=req.text().slice(0,10)/1000;
else TSAp= EVAL_STR;
if(req.text().slice(10,20)!=2147483647)TSAm=req.text().slice(10,20)/1000;
else TSAm= EVAL_STR;
if(req.text().slice(20,30)!=2147483647)TSRp=req.text().slice(20,30)/1000;
else TSRp= EVAL_STR;
if(req.text().slice(30,40)!=2147483647)TSAm=req.text().slice(30,40)/1000;
else TSRm= EVAL_STR;}
}
if(T1){
//Энергия от сброса Т1
req = SYS.XMLNode().setAttr("ProtIt","m230").setAttr("netaddr",netaddr).setAttr("data",SYS.strFromCharCode(0x05,0x00,0x01));
SYS.Transport["Serial"]["out_"+transport].messIO(req,"UserProtocol");
if(!req.text()){T1Ap  = EVAL_STR;T1Am = EVAL_STR;T1Rp = EVAL_STR;T1Rm = EVAL_STR; }
else {
if(req.text().slice(0,10)!=2147483647)T1Ap=req.text().slice(0,10)/1000;
else T1Ap= EVAL_STR;
if(req.text().slice(10,20)!=2147483647)T1Am=req.text().slice(10,20)/1000;
else T1Am= EVAL_STR;
if(req.text().slice(20,30)!=2147483647)T1Rp=req.text().slice(20,30)/1000;
else T1Rp= EVAL_STR;
if(req.text().slice(30,40)!=2147483647)T1Am=req.text().slice(30,40)/1000;
else T1Rm= EVAL_STR;}
}
//Энергия от сброса Т2
if(T2){
req = SYS.XMLNode().setAttr("ProtIt","m230").setAttr("netaddr",netaddr).setAttr("data",SYS.strFromCharCode(0x05,0x00,0x02));
SYS.Transport["Serial"]["out_"+transport].messIO(req,"UserProtocol");
if(!req.text()){T2Ap  = EVAL_STR;T2Am = EVAL_STR;T2Rp = EVAL_STR;T2Rm = EVAL_STR; }
else {
if(req.text().slice(0,10)!=2147483647)T2Ap=req.text().slice(0,10)/1000;
else T2Ap= EVAL_STR;
if(req.text().slice(10,20)!=2147483647)T2Am=req.text().slice(10,20)/1000;
else T2Am= EVAL_STR;
if(req.text().slice(20,30)!=2147483647)T2Rp=req.text().slice(20,30)/1000;
else T2Rp= EVAL_STR;
if(req.text().slice(30,40)!=2147483647)T2Am=req.text().slice(30,40)/1000;
else T2Rm= EVAL_STR;}
}
//Энергия от сброса Т3
if(T3){
req = SYS.XMLNode().setAttr("ProtIt","m230").setAttr("netaddr",netaddr).setAttr("data",SYS.strFromCharCode(0x05,0x00,0x03));
SYS.Transport["Serial"]["out_"+transport].messIO(req,"UserProtocol");
if(!req.text()){T3Ap  = EVAL_STR;T3Am = EVAL_STR;T3Rp = EVAL_STR;T3Rm = EVAL_STR; }
else {
if(req.text().slice(0,10)!=2147483647)T3Ap=req.text().slice(0,10)/1000;
else T3Ap= EVAL_STR;
if(req.text().slice(10,20)!=2147483647)T3Am=req.text().slice(10,20)/1000;
else T3Am= EVAL_STR;
if(req.text().slice(20,30)!=2147483647)T3Rp=req.text().slice(20,30)/1000;
else T3Rp= EVAL_STR;
if(req.text().slice(30,40)!=2147483647)T3Am=req.text().slice(30,40)/1000;
else T3Rm= EVAL_STR;}
}
//Энергия от сброса Т4
if(T4){
req = SYS.XMLNode().setAttr("ProtIt","m230").setAttr("netaddr",netaddr).setAttr("data",SYS.strFromCharCode(0x05,0x00,0x04));
SYS.Transport["Serial"]["out_"+transport].messIO(req,"UserProtocol");
if(!req.text()){T4Ap  = EVAL_STR;T4Am = EVAL_STR;T4Rp = EVAL_STR;T4Rm = EVAL_STR; }
else {
if(req.text().slice(0,10)!=2147483647)T4Ap=req.text().slice(0,10)/1000;
else T4Ap= EVAL_STR;
if(req.text().slice(10,20)!=2147483647)T4Am=req.text().slice(10,20)/1000;
else T4Am= EVAL_STR;
if(req.text().slice(20,30)!=2147483647)T4Rp=req.text().slice(20,30)/1000;
else T4Rp= EVAL_STR;
if(req.text().slice(30,40)!=2147483647)T4Am=req.text().slice(30,40)/1000;
else T4Rm= EVAL_STR;}
}
//Мощность P
if(P){
req = SYS.XMLNode().setAttr("ProtIt","m230").setAttr("netaddr",netaddr).setAttr("data",SYS.strFromCharCode(0x08,0x16,0x00));
SYS.Transport["Serial"]["out_"+transport].messIO(req,"UserProtocol");
if(!req.text()){PS=EVAL_STR;P1=EVAL_STR;P2=EVAL_STR;P3=EVAL_STR;}
else PS=req.text().slice(0,7)/100;P1=req.text().slice(7,14)/100;P2=req.text().slice(14,21)/100;P3=req.text().slice(21,28)/100;}
//Мощность Q
if(Q){
req = SYS.XMLNode().setAttr("ProtIt","m230").setAttr("netaddr",netaddr).setAttr("data",SYS.strFromCharCode(0x08,0x16,0x04));
SYS.Transport["Serial"]["out_"+transport].messIO(req,"UserProtocol");
if(!req.text()){QS=EVAL_REAL;Q1=EVAL_REAL;Q2=EVAL_REAL;Q3=EVAL_REAL;}
else 
{if (req.text().slice(28,29)==4||req.text().slice(28,29)=="C")QS=(req.text().slice(0,7)/100)*-1;
else QS=(req.text().slice(0,7)/100);
if (req.text().slice(29,30)==4||req.text().slice(29,30)=="C")Q1=(req.text().slice(7,14)/100)*-1;
else Q1=(req.text().slice(7,14)/100);
if (req.text().slice(30,31)==4||req.text().slice(30,31)=="C")Q2=(req.text().slice(14,21)/100)*-1;
else Q2=(req.text().slice(14,21)/100);
if (req.text().slice(31,32)==4||req.text().slice(31,32)=="C")Q3=(req.text().slice(21,28)/100)*-1;
else Q3=(req.text().slice(21,28)/100);}}
//Мощность S
if(S){
req = SYS.XMLNode().setAttr("ProtIt","m230").setAttr("netaddr",netaddr).setAttr("data",SYS.strFromCharCode(0x08,0x16,0x08));
SYS.Transport["Serial"]["out_"+transport].messIO(req,"UserProtocol");
if(!req.text()){SS=EVAL_STR;S1=EVAL_STR;S2=EVAL_STR;S3=EVAL_STR;}
else SS=req.text().slice(0,7)/100;S1=req.text().slice(7,14)/100;S2=req.text().slice(14,21)/100;S3=req.text().slice(21,28)/100;}
// Напряжение U
if(U){
req = SYS.XMLNode().setAttr("ProtIt","m230").setAttr("netaddr",netaddr).setAttr("data",SYS.strFromCharCode(0x08,0x16,0x11));
SYS.Transport["Serial"]["out_"+transport].messIO(req,"UserProtocol");
if(!req.text()){U1=EVAL_STR;U2=EVAL_STR;U3=EVAL_STR;}
else U1=req.text().slice(0,8)/100;U2=req.text().slice(8,16)/100;U3=req.text().slice(16,24)/100;}
//Ток I
if(I){
req = SYS.XMLNode().setAttr("ProtIt","m230").setAttr("netaddr",netaddr).setAttr("data",SYS.strFromCharCode(0x08,0x16,0x21));
SYS.Transport["Serial"]["out_"+transport].messIO(req,"UserProtocol");
if(!req.text()){I1=EVAL_STR;I2=EVAL_STR;I3=EVAL_STR;}
else I1=req.text().slice(0,8)/1000;I2=req.text().slice(8,16)/1000;I3=req.text().slice(16,24)/1000;}
//Коэффициент мощности
if(K){
req = SYS.XMLNode().setAttr("ProtIt","m230").setAttr("netaddr",netaddr).setAttr("data",SYS.strFromCharCode(0x08,0x16,0x30));
SYS.Transport["Serial"]["out_"+transport].messIO(req,"UserProtocol");
if(!req.text()){KS=EVAL_STR;K1=EVAL_STR;K2=EVAL_STR;K3=EVAL_STR;}
else KS=req.text().slice(0,7)/1000;K1=req.text().slice(7,14)/1000;K2=req.text().slice(14,21)/1000;K3=req.text().slice(21,28)/1000;}
//частота F
if(F){
req = SYS.XMLNode().setAttr("ProtIt","m230").setAttr("netaddr",netaddr).setAttr("data",SYS.strFromCharCode(0x08,0x16,0x40));
SYS.Transport["Serial"]["out_"+transport].messIO(req,"UserProtocol");
if(!req.text())F1= EVAL_STR;
else F1 =req.text();}
//серийный номер
if(N){
req = SYS.XMLNode().setAttr("ProtIt","m230").setAttr("netaddr",netaddr).setAttr("data",SYS.strFromCharCode(0x08,0x00));
SYS.Transport["Serial"]["out_"+transport].messIO(req,"UserProtocol");
if(!req.text())N1= EVAL_STR;
else N1 =req.text();}
','','',1509290192);
CREATE TABLE 'tmplib_PrescrTempl' ("ID" TEXT DEFAULT '' ,"NAME" TEXT DEFAULT '' ,"uk#NAME" TEXT DEFAULT '' ,"ru#NAME" TEXT DEFAULT '' ,"DESCR" TEXT DEFAULT '' ,"uk#DESCR" TEXT DEFAULT '' ,"ru#DESCR" TEXT DEFAULT '' ,"MAXCALCTM" INTEGER DEFAULT '10' ,"PR_TR" INTEGER DEFAULT '1' ,"PROGRAM" TEXT DEFAULT '' ,"uk#PROGRAM" TEXT DEFAULT '' ,"ru#PROGRAM" TEXT DEFAULT '' ,"TIMESTAMP" INTEGER DEFAULT '' , PRIMARY KEY ("ID"));
INSERT INTO "tmplib_PrescrTempl" VALUES('timer','Timer','Таймер','Таймер','Typical timer. Hold run up to time elapse.','Типовий таймер. Утримує виконання до завершення часу.','Типовой таймер. Удерживает выполнение до завершения времени.',10,0,'JavaLikeCalc.JavaScript
//Reset to default
if(f_start || (!run && tmp1)) { run = pause = error = abort = false; tmp1 = 0; rez = 1; }
//Run
if(run && !pause) {
	if(tmp1 == 0) tmp1 = arg1+1/f_frq;
	tmp1 = max(0, tmp1 - 1/f_frq);
	if(tmp1 <= 0) { rez = "1:"+tr("Waiting %1s expired").replace("%1",arg1.toPrecision(3)); run = false; }
	else rez = "0:"+tr("Waiting %1s").replace("%1",tmp1.toPrecision(3));
}','','',1416656093);
INSERT INTO "tmplib_PrescrTempl" VALUES('backTimer','Background timer','Фоновий таймер','Фоновый таймер','Background timer. Updating parallel with current command.','Фоновий таймер. Оновлення паралельно із поточною командой.','Фоновый таймер. Обновление параллельно с текущей командой.',10,0,'JavaLikeCalc.JavaScript
//Reset to default
if(f_start || (!run && tmp1)) { run = pause = error = abort = false; tmp1 = 0; rez = 1; }
//Run
if(run && !pause) {
	if(tmp1 == 0) tmp1 = arg1+1/f_frq;
	tmp1 = max(0, tmp1 - 1/f_frq);
	if(tmp1 <= 0) { rez = "1:"+tr("Waiting %1s expired").replace("%1",arg1.toPrecision(3)); run = false; }
	//if(tmp1 <= 10)	rez = "-1:Waiting "+arg1.toPrecision(3)+"s expired";
	else rez = "10:"+tr("Background waiting %1s").replace("%1",tmp1.toPrecision(3));
}','','',1416656088);
CREATE TABLE 'tmplib_base' ("ID" TEXT DEFAULT '' ,"NAME" TEXT DEFAULT '' ,"uk#NAME" TEXT DEFAULT '' ,"ru#NAME" TEXT DEFAULT '' ,"DESCR" TEXT DEFAULT '' ,"uk#DESCR" TEXT DEFAULT '' ,"ru#DESCR" TEXT DEFAULT '' ,"MAXCALCTM" INTEGER DEFAULT '10' ,"PR_TR" INTEGER DEFAULT '1' ,"PROGRAM" TEXT DEFAULT '' ,"uk#PROGRAM" TEXT DEFAULT '' ,"ru#PROGRAM" TEXT DEFAULT '' ,"TIMESTAMP" INTEGER DEFAULT '' , PRIMARY KEY ("ID"));
INSERT INTO "tmplib_base" VALUES('digAlarm','Alarm digital','Сигн. дискретна','Сигн. дискретная','Alarm from a digital parameter.','Сигналізація за дискретним параметром.','Сигнализация по дискретному параметру.',10,0,'JavaLikeCalc.JavaScript
if(f_start) {
	f_err = "0", prevVar = EVAL_REAL;
	//Prepare data for preprocessing
	inPrcId = this.nodePath("_");
	inPrcLng = "JavaLikeCalc.JavaScript";
	inPrcArgs = new Object();
	inPrcArgs.this = this;
	inPrcArgs.ctx = new Object();
	return;
}

//Call a specific preprocessing procedure
if(inProc.length)	{
	inPrcArgs.f_frq = f_frq;
	inPrcArgs.in = in;
	SYS.DAQ.funcCall(inPrcLng, inPrcArgs, inProc, inPrcId);
	in = inPrcArgs.in;
}

//State set
tErr = "0", levErr = 0;
if(in.isEVal()) {
	tErr = "1:"+tr("No data or connection with source");
	levErr = -5;
	st_open = st_close = in;
}
else {
	if(in == alrm.toInt())	tErr = "3:"+alrm.parse(2,":"), levErr = -alrm.parse(1,":").toInt();
	st_open = stInv ? !in : in;
	st_close = !st_open;
}

//Alarms forming
if(tErr.toInt() && tErr.toInt() != f_err.toInt())
	this.cntr().alarmSet((NAME.length?NAME:SHIFR)+": "+DESCR+": "+tErr.parse(1,":"), levErr, SHIFR);
else if(f_err.toInt() && !tErr.toInt())
	this.cntr().alarmSet((NAME.length?NAME:SHIFR)+": "+DESCR+": "+tr("NORMA"), 1, SHIFR);
f_err = tErr;','','',1463998184);
INSERT INTO "tmplib_base" VALUES('simleBoard','Analog alarm by borders','Сигн. аналог. за границями','Сигн. аналог. по границам','The template of simple parameter included boders and dimension variable.','Шаблон простого параметру з перевіркою границь та одиницею виміру.','Шаблон простого параметра с проверкой границ и единицей измерения.',10,1,'JavaLikeCalc.JavaScript
var=iMult*(in+iAdd);
if(var>max)			f_err="1:Upper work border violation";
else if(var<min)			f_err="2:Bottom work border violation";
else if(aMax>aMin && var>aMax)		f_err="3:Upper alarm border violation";
else if(aMax>aMin && var<aMin)		f_err="4:Bottom alarm border violation";
else if(wMax>wMin && var>wMax)	f_err="5:Upper warning border violation";
else if(wMax>wMin && var<wMin)	f_err="6:Bottom warning border violation";
else f_err="0";','JavaLikeCalc.JavaScript
var=iMult*(in+iAdd);
if(var>max)			f_err="1:Порушення верхньої робочої границі";
else if(var<min)			f_err="2:Порушення нижньої робочої границі";
else if(aMax>aMin && var>aMax)		f_err="3:Порушення верхньої аварійної границі";
else if(aMax>aMin && var<aMin)		f_err="4:Порушення нижньої аварійної границі";
else if(wMax>wMin && var>wMax)	f_err="5:Порушення верхньої попереджувальної границі";
else if(wMax>wMin && var<wMin)	f_err="6:Порушення нижньої попереджувальної границі";
else f_err="0";','JavaLikeCalc.JavaScript
var=iMult*(in+iAdd);
if(var>max)		f_err="1:Нарушение верхней рабочей границы";
else if(var<min)	f_err="2:Нарушение нижней рабочей границы";
else if(aMax>aMin && var>aMax)	f_err="3:Нарушение верхней аварийной границы";
else if(aMax>aMin && var<aMin)	f_err="4:Нарушение нижней аварийной границы";
else if(wMax>wMin && var>wMax)	f_err="5:Нарушение верхней предупредительной границы";
else if(wMax>wMin && var<wMin)	f_err="6:Нарушение нижней предупредительной границы";
else f_err="0";','');
INSERT INTO "tmplib_base" VALUES('digitBlock','Diskret parameters block','Блок дискр. параметрів','Блок дискр. параметров','The block for union of Diskret parameters for one device control.','Блок для збору дискретних параметрів, керуючих одним апаратом.','Блок для сборки дискретных параметров управляющих одним аппаратом.',10,0,'JavaLikeCalc.JavaScript
set = false;
if(cmdOpen && !(last_cmd&0x1))	{ last_cmd = last_cmd|0x1; set = true; }
if(cmdClose && !(last_cmd&0x2))	{ last_cmd = last_cmd|0x2; set = true; }
if(cmdStop && !(last_cmd&0x4))	{ last_cmd = last_cmd|0x4; set = true; }
if(set && tCmd > 0) w_tm = tCmd;
if(w_tm > 0) w_tm -= 1/f_frq;
else {
	w_tm = 0;
	if(tCmd > 0) {
		if(last_cmd&0x1) { cmdOpen = false; last_cmd = last_cmd&(~0x1); }
		if(last_cmd&0x2) { cmdClose = false; last_cmd = last_cmd&(~0x2); }
		if(last_cmd&0x4) { cmdStop = false; last_cmd = last_cmd&(~0x4); }
	}
}','','',1416656400);
INSERT INTO "tmplib_base" VALUES('gasPoint','Flow control point','Витрато-вимірювальний вузол','Расходомерный узел',' ',' ','',10,0,'JavaLikeCalc.JavaScript
if(f_start)	Q = 0;
//F=200+(rand(5)-2.5);
Q += F.isEVal() ? 0 : F/3600;
//P=9+(rand(2)-1);
//T=15+(rand(2)-1);
dP = F.isEVal() ? 0 : F/33;
DS = 1+(rand(0.2)-0.1);
','','',1503582557);
INSERT INTO "tmplib_base" VALUES('manInUnif','Manual input (Unif)','Ручний ввід (Уніф)','Ручной ввод (Униф)','Unified template for manual input signals.','Уніфікований шаблон для ручного вводу значень сигналів.','Унифицированный шаблон ручного ввода сигнала.',10,0,'JavaLikeCalc.JavaScript
if(f_start)	f_err = "0";

pMax = plcMax; pMin = plcMin;	//Copy for local modifies using
if(passIn=(pMax==pMin)) { pMax = max/iMult - iAdd; pMin = min/iMult - iAdd; }

//Manual input set process
if(!varIn.isEVal() && (varIn != var || inout.isEVal())) {
	if(passIn) { pMin = iMult*(pMin+iAdd); pMax = iMult*(pMax+iAdd); }
	vCalibr = (varIn-min)/(max-min);
	if(scSqr)	vCalibr = pow(vCalibr,2);
	if(pMax < pMin) vCalibr = 1-vCalibr;
	vCalibr = min(pMax,pMin)+vCalibr*abs(pMax-pMin);
	inout = vCalibr/iMult - iAdd;
}

levErr = 0;
tErr = "0";
//Input data check and postprocess
if(inout > (max(pMax,pMin)+plcExcess*abs(pMax-pMin)/100)) {
	tErr = "1:"+tr("The signal exceed to upper hardware border"); levErr = -5;
	var = max + plcExcess*(max-min)/100;
}
else if(inout < (min(pMax,pMin)-plcExcess*abs(pMax-pMin)/100)) {
	tErr = "2:"+tr("The signal exceed to bottom hardware border"); levErr = -5;
	var = min - plcExcess*(max-min)/100;
}
if(!tErr) {
	vCalibr = iMult*(inout+iAdd);
	if(passIn) { pMin = iMult*(pMin+iAdd); pMax = iMult*(pMax+iAdd); }
	if(!passIn || scSqr) {
		vCalibr = (vCalibr-min(pMax,pMin))/abs(pMax-pMin);
		if(pMax < pMin) vCalibr = 1-vCalibr;
		vCalibr = min + (max-min)*(scSqr?pow(vCalibr,0.5):vCalibr);
	}
	if(var.isEVal())	var = vCalibr;
	varDt = vCalibr - var;
	var += varDt/max(1,Tf*f_frq);
	varIn = var;

	bndVarHyst = (max-min)*HystBnd/100;
	if(aMax < max && aMax > aMin && (var >= aMax || (f_err.toInt() == 3 && var >= (aMax-bndVarHyst))))
	{ tErr="3:"+tr("Upper alarm border error"); levErr = -4; }
	else if(aMin > min && aMax > aMin && (var <= aMin || (f_err.toInt() == 4 && var <= (aMin+bndVarHyst))))
	{ tErr="4:"+tr("Lower alarm border error"); levErr = -4; }
	else if(wMax < max && wMax > wMin && (var >= wMax || (f_err.toInt() == 5 && var >= (wMax-bndVarHyst))))
	{ tErr="5:"+tr("Upper warning border error"); levErr = -2; }
	else if(wMin > min && wMax > wMin && (var <= wMin || (f_err.toInt() == 6 && var <= (wMin+bndVarHyst))))
	{ tErr="6:"+tr("Lower warning border error"); levErr = -2; }
	else if(speed && varDt > speed)	{ tErr="7:"+tr("Too big parameter''s motion speed"); levErr = -2; }
}

//Alarms forming
if(alSup)	f_err = "0";
else {
	if(tErr.toInt() && tErr.toInt() != f_err.toInt())
		this.cntr().alarmSet((NAME.length?NAME:SHIFR)+": "+DESCR+": "+tErr.parse(1,":"), levErr, SHIFR);
	else if(f_err.toInt() && !tErr.toInt())
		this.cntr().alarmSet((NAME.length?NAME:SHIFR)+": "+DESCR+": "+tr("NORMA"), 1, SHIFR);
	f_err = tErr;
}','','',1441880537);
INSERT INTO "tmplib_base" VALUES('anUnif','Analog sign. (Unif)','Аналог. сигнал (Уніф)','Аналог. сигн. (Униф)','Unified template for analog input signals processing.','Уніфікований шаблон для обробки аналогових вхідних сигналів.','Унифицированный шаблон обработки аналогового входного сигнала.',10,0,'JavaLikeCalc.JavaScript
if(f_start) {
	if(alSup == 0) f_err = "0";
	prevVar = EVAL_REAL;
	conDelay_ = 0;
	//Prepare data for preprocessing
	inPrcId = this.nodePath("_");
	inPrcLng = "JavaLikeCalc.JavaScript";
	inPrcArgs = new Object();
	inPrcArgs.this = this;
	inPrcArgs.ctx = new Object();
	return;
}
if(f_stop) return;

pMax = plcMax; pMin = plcMin;	//Copy for local modifies using
if(passIn=(pMax==pMin)) { pMax = max/iMult - iAdd; pMin = min/iMult - iAdd; }

if(plcImit) {	//Data imitation
  if(!plcImitIn.isEVal()) in = plcImitIn;
  else {
    plcDif = abs(pMax-pMin);
    in = pMin + plcDif/2 + rand(plcDif/10)-plcDif/20;
  }
}

//Call specific preprocessing procedure
if(inProc.length)	{
	inPrcArgs.f_frq = f_frq;
	inPrcArgs.in = in; inPrcArgs.var = var; inPrcArgs.min = min; inPrcArgs.max = max;
	inPrcArgs.plcMin = pMin; inPrcArgs.plcMax = pMax;
	inPrcArgs.plcImit = plcImit; inPrcArgs.plcImitIn = plcImitIn;
	SYS.DAQ.funcCall(inPrcLng, inPrcArgs, inProc, inPrcId);
	in = inPrcArgs.in;
}

levErr = 0;
tErr = "0";
//Input data check and postprocess
if(in.isEVal()) {
	//if(alDelay > 0 && conDelay_ < alDelay){ conDelay_ += 1/f_frq; return; }
	tErr = "1:"+tr("No data or connection with source"); levErr = -5;
	var = EVAL_REAL;
	if(subMode == 1) var = prevVar;
	else if(subMode == 2) var = subVar;
}
else if(in > (max(pMax,pMin)+plcExcess*abs(pMax-pMin)/100)) {
	tErr = "1:"+tr("The signal exceed to upper hardware border"); levErr = -5;
	var = EVAL_REAL;
	if(subMode == 1) var = prevVar.isEVal() ? max+plcExcess*(max-min)/100 : prevVar;
	else if(subMode == 2) var = subVar;
}
else if(in < (min(pMax,pMin)-plcExcess*abs(pMax-pMin)/100)) {
	tErr = "2:"+tr("The signal exceed to bottom hardware border"); levErr = -5;
	var = EVAL_REAL;
	if(subMode == 1) var = prevVar.isEVal() ? min-plcExcess*(max-min)/100 : prevVar;
	else if(subMode == 2) var = subVar;
}
if(!tErr) {
	vCalibr = iMult*(in+iAdd);
	if(passIn) { pMin = iMult*(pMin+iAdd); pMax = iMult*(pMax+iAdd); }
	if(!passIn || scSqr) {
		vCalibr = (vCalibr-min(pMax,pMin))/abs(pMax-pMin);
		if(pMax < pMin) vCalibr = 1-vCalibr;
		vCalibr = min + (max-min)*(scSqr?pow(vCalibr,0.5):vCalibr);
	}
	if(var.isEVal())	var = vCalibr;
	varDt = vCalibr - var;
	var += varDt/max(1,Tf*f_frq);
	prevVar = var;

	bndVarHyst = (max-min)*HystBnd/100;
	if(aMax < max && aMax > aMin && (var >= aMax || (f_err.toInt() == 3 && var >= (aMax-bndVarHyst))))
	{ tErr = "3:"+tr("Upper alarm border error"); levErr = -4; }
	else if(aMin > min && aMax > aMin && (var <= aMin || (f_err.toInt() == 4 && var <= (aMin+bndVarHyst))))
	{ tErr = "4:"+tr("Lower alarm border error"); levErr = -4; }
	else if(wMax < max && wMax > wMin && (var >= wMax || (f_err.toInt() == 5 && var >= (wMax-bndVarHyst))))
	{ tErr = "5:"+tr("Upper warning border error"); levErr = -2; }
	else if(wMin > min && wMax > wMin && (var <= wMin || (f_err.toInt() == 6 && var <= (wMin+bndVarHyst))))
	{ tErr = "6:"+tr("Lower warning border error"); levErr = -2; }
	else if(speed && varDt > speed)	{ tErr = "7:"+tr("Too big parameter''s motion speed"); levErr = -2; }
}

//Alarms forming
if(alSup == 1)	f_err = "0";
else {
	if(alDelay > 0 && conDelay_ <= 0 && tErr.toInt() != f_err.toInt())	tErr1 = tErr.toInt();
	if(alDelay > 0 && conDelay_ > 0 && tErr.toInt() != tErr1)	{ conDelay_ = 0; tErr1 = tErr.toInt(); }
	if(alDelay > 0 && conDelay_ < alDelay){ conDelay_ += 1/f_frq; return; }
	if(tErr.toInt() && tErr.toInt() != f_err.toInt())
		this.cntr().alarmSet((NAME.length?NAME:SHIFR)+": "+DESCR+": "+tErr.parse(1,":"), levErr, SHIFR);
	else if(f_err.toInt() && !tErr.toInt())
		this.cntr().alarmSet((NAME.length?NAME:SHIFR)+": "+DESCR+": "+tr("NORMA"), 1, SHIFR);
	toSave = (alSup == 2 && f_err != tErr);
	f_err = tErr;
	if(toSave) SYS.cntrReq(SYS.XMLNode("save").setAttr("path",this.nodePath()+"/%2fobj").setAttr("force",1));
	conDelay_ = 0;
}','','',1481216651);
INSERT INTO "tmplib_base" VALUES('digitBlockUnif','Diskret block (Unif)','Блок дискретних (Уніф)','Блок дискр. (Униф)','The block for union of Diskret parameters for one device control.','Блок поєднання дискретних сигналів контролю одним пристроєм.','Блок для дискретных параметров управляющих одним аппаратом.',10,0,'JavaLikeCalc.JavaScript
set = false;
if(!com.isEVal() && com && last_cmd != 1)		last_cmd = 1, set = true;
if(!close.isEVal() && close && last_cmd != 2)last_cmd = 2, set = true;
if(!stop.isEVal() && stop && last_cmd != 3)	last_cmd = 3, set = true;
if(set && tCmd > 0) w_tm = tCmd;
if(w_tm > 0) w_tm -= 1/f_frq;
else {
	w_tm = 0;
	if(tCmd > 0) {
		if(last_cmd == 1)	com = false;
		if(last_cmd == 2)	close = false;
		if(last_cmd == 3)	stop = false;
		last_cmd = 0;
	}
}','','',1441903511);
INSERT INTO "tmplib_base" VALUES('pidUnifImp','Impulse PID sign. (Unif, stats)','Імпульсний ПІД сигнал (Уніф, стани)','ПИД импульсный сигнал (Униф, состояния)','The unified template for process analog signals with properties impulse PID.','Уніфікований шаблон для обробки аналогового сигналу з властивостями імпульсного ПІД.','Унифицированный шаблон обработки аналогового сигнала со свойствами импульсного ПИД.',10,0,'JavaLikeCalc.JavaScript
if(f_start) f_err = "0";

if(plcImit) {
	if(!plcImitIn.isEVal()) in = plcImitIn;
	else {
		plcDif = plcMax-plcMin;
		in = plcMin + plcDif/2 + rand(plcDif/10)-plcDif/20;
	}
}

tErr = "0";
if(in > (plcMax+plcExcess*(plcMax-plcMin)/100))
{ tErr = "1:"+tr("Signal exceed to upper hardware border"); var = max+plcExcess*(max-min)/100; }
else if(in < (plcMin-plcExcess*(plcMax-plcMin)/100))
{ tErr = "2:"+tr("Signal exceed to bottom hardware border"); var = min-plcExcess*(max-min)/100; }
if(tErr) {
	//var = dmsk ? max+plcExcess*(max-min)/100 : min-plcExcess*(max-min)/100;
	EVAL = true;
}
else {
	vCalibr = iMult*(in+iAdd);

	vCalibr = (vCalibr-plcMin)/(plcMax-plcMin);
	varDt = min + (max-min)*(scSqr?pow(vCalibr,0.5):vCalibr) - var;
	var += varDt/max(1,Tf/1000*f_frq);

	bndVarHyst = (aMax-aMin)*HystBnd/100;
	if(aMax > aMin && (var >= aMax || (HH && var >= (aMax-bndVarHyst))))
	{ tErr = "3:"+tr("Upper alarm border error"); HH = true; EVAL = H = LL = L = false; }
	else if(aMax > aMin && (var <= aMin || (LL && var <= (aMin+bndVarHyst))))
	{ tErr="4:"+tr("Lower alarm border error"); LL = true; EVAL = HH = H = L = false; }
	else if(wMax > wMin && (var >= wMax || (H && var >= (wMax-bndVarHyst))))
	{ tErr="5:"+tr("Upper warning border error"); H = true; EVAL = HH = LL = L = false; }
	else if(wMax > wMin && (var <= wMin || (L && var <= (wMin+bndVarHyst))))
	{ tErr="6:"+tr("Lower warning border error"); L = true; EVAL = HH = H = LL = false; }
	else if(speed && varDt > speed)
	{ tErr = "7:"+tr("Too big parameter''s motion speed"); EVAL = true; HH = H = LL = L = false; }
	else EVAL = HH = H = LL = L = false;
}

if(!f_err.toInt() && tErr.toInt()) this.cntr().alarmSet((NAME.length?NAME:SHIFR)+": "+DESCR+": "+tErr.parse(1,":"), -4, SHIFR);
else if(f_err.toInt() && !tErr.toInt()) this.cntr().alarmSet((NAME.length?NAME:SHIFR)+": "+DESCR+": "+tr("NORMA"), 1, SHIFR);
f_err = tErr;','','',1450381598);
INSERT INTO "tmplib_base" VALUES('anUnifSt','Analog sign. (Unif, stats)','Аналог. сигнал (Уніф, стани)','Аналог. сигн. (Униф, состояния)','Unified template for analog input signals processing.','Уніфікований шаблон для обробки аналогових вхідних сигналів.','Унифицированный шаблон обработки аналогового входного сигнала.',10,0,'JavaLikeCalc.JavaScript
if(f_start) {
	if(alSup == 0) f_err = "0";
	prevVar = EVAL_REAL;
	conDelay_ = 0;
	//Prepare data for preprocessing
	inPrcId = this.nodePath("_");
	inPrcLng = "JavaLikeCalc.JavaScript";
	inPrcArgs = new Object();
	inPrcArgs.this = this;
	inPrcArgs.ctx = new Object();
	return;
}
if(f_stop) return;

pMax = plcMax; pMin = plcMin;	//Copy for local modifies using
if(passIn=(pMax==pMin)) { pMax = max/iMult - iAdd; pMin = min/iMult - iAdd; }

if(plcImit) {	//Data imitation
	if(!plcImitIn.isEVal()) in = plcImitIn;
	else {
		plcDif = abs(pMax-pMin);
		in = pMin + plcDif/2 + rand(plcDif/10)-plcDif/20;
	}
}

//Call specific preprocessing procedure
if(inProc.length)	{
	inPrcArgs.f_frq = f_frq;
	inPrcArgs.in = in; inPrcArgs.var = var; inPrcArgs.min = min; inPrcArgs.max = max;
	inPrcArgs.plcMin = pMin; inPrcArgs.plcMax = pMax;
	inPrcArgs.plcImit = plcImit; inPrcArgs.plcImitIn = plcImitIn;
	SYS.DAQ.funcCall(inPrcLng, inPrcArgs, inProc, inPrcId);
	in = inPrcArgs.in;
}

levErr = 0;
tErr = "0";
//Input data check and postprocess
if(in.isEVal()) {
	//if(alDelay > 0 && conDelay_ < alDelay){ conDelay_ += 1/f_frq; return; }
	tErr = "1:"+tr("No data or connection with source"); levErr = -5;
	var = EVAL_REAL;
	if(subMode == 1) var = prevVar;
	else if(subMode == 2) var = subVar;
}
else if(in > (max(pMax,pMin)+plcExcess*abs(pMax-pMin)/100)) {
	tErr = "1:"+tr("The signal exceed to upper hardware border"); levErr = -5;
	var = EVAL_REAL;
	if(subMode == 1) var = prevVar.isEVal() ? max+plcExcess*(max-min)/100 : prevVar;
	else if(subMode == 2) var = subVar;
}
else if(in < (min(pMax,pMin)-plcExcess*abs(pMax-pMin)/100)) {
	tErr = "2:"+tr("The signal exceed to bottom hardware border"); levErr = -5;
	var = EVAL_REAL;
	if(subMode == 1) var = prevVar.isEVal() ? min-plcExcess*(max-min)/100 : prevVar;
	else if(subMode == 2) var = subVar;
}
if(tErr)	EVAL = true, HH = H = LL = L = false;
else {
	vCalibr = iMult*(in+iAdd);
	if(passIn) { pMin = iMult*(pMin+iAdd); pMax = iMult*(pMax+iAdd); }
	if(!passIn || scSqr) {
		vCalibr = (vCalibr-min(pMax,pMin))/abs(pMax-pMin);
		if(pMax < pMin) vCalibr = 1-vCalibr;
		vCalibr = min + (max-min)*(scSqr?pow(vCalibr,0.5):vCalibr);
	}
	if(var.isEVal())	var = vCalibr;
	varDt = vCalibr - var;
	var += varDt/max(1,Tf*f_frq);
	prevVar = var;

	bndVarHyst = (max-min)*HystBnd/100;
	EVAL = HH_ = H_ = LL_ = L_ = false;
	if(speed && varDt > speed)	{ tErr = "7:"+tr("Too big parameter''s motion speed"); levErr = -2; }
	if(wMin > min && wMax > wMin && (var <= wMin || (L && var <= (wMin+bndVarHyst))))
	{ tErr = "6:"+tr("Lower warning border error"); levErr = -2; L_ = true; }
	if(wMax < max && wMax > wMin && (var >= wMax || (H && var >= (wMax-bndVarHyst))))
	{ tErr = "5:"+tr("Upper warning border error"); levErr = -2; H_ = true; }
	if(aMin > min && aMax > aMin && (var <= aMin || (LL && var <= (aMin+bndVarHyst))))
	{ tErr = "4:"+tr("Lower alarm border error"); levErr = -4; LL_ = true; }
	if(aMax < max && aMax > aMin && (var >= aMax || (HH && var >= (aMax-bndVarHyst))))
	{ tErr = "3:"+tr("Upper alarm border error"); levErr = -4; HH_ = true; }
	HH = HH_; H = H_; LL = LL_; L = L_;
}

//Alarms forming
if(alSup == 1)	f_err = "0";
else {
	if(alDelay > 0 && conDelay_ <= 0 && tErr.toInt() != f_err.toInt())	tErr1 = tErr.toInt();
	if(alDelay > 0 && conDelay_ > 0 && tErr.toInt() != tErr1)	{ conDelay_ = 0; tErr1 = tErr.toInt(); }
	if(alDelay > 0 && conDelay_ < alDelay){ conDelay_ += 1/f_frq; return; }
	if(tErr.toInt() && tErr.toInt() != f_err.toInt())
		this.cntr().alarmSet((NAME.length?NAME:SHIFR)+": "+DESCR+": "+tErr.parse(1,":"), levErr, SHIFR);
	else if(f_err.toInt() && !tErr.toInt())
		this.cntr().alarmSet((NAME.length?NAME:SHIFR)+": "+DESCR+": "+tr("NORMA"), 1, SHIFR);
	toSave = (alSup == 2 && f_err != tErr);
	f_err = tErr;
	if(toSave) SYS.cntrReq(SYS.XMLNode("save").setAttr("path",this.nodePath()+"/%2fobj").setAttr("force",1));
	conDelay_ = 0;
}','','',1481216725);
INSERT INTO "tmplib_base" VALUES('pidUnif','PID sign. (Unif, stats)','ПІД сигнал (Уніф, стани)','ПИД сигнал (Униф, состояния)','The unified template for process analog signals with properties PID.','Уніфікований шаблон для обробки аналогового сигналу з властивостями ПІД.','Унифицированный шаблон обработки аналогового сигнала со свойствами ПИД.',10,0,'JavaLikeCalc.JavaScript
if(f_start) f_err = "0";

if(plcImit) {
	if(!plcImitIn.isEVal()) in = plcImitIn;
	else {
		plcDif = plcMax-plcMin;
		in = plcMin + plcDif/2 + rand(plcDif/10)-plcDif/20;
	}
}

tErr = "0";
if(in > (plcMax+plcExcess*(plcMax-plcMin)/100))
{ tErr = "1:"+tr("Signal exceed to upper hardware border"); var = max+plcExcess*(max-min)/100; }
else if(in < (plcMin-plcExcess*(plcMax-plcMin)/100))
{ tErr = "2:"+tr("Signal exceed to bottom hardware border"); var = min-plcExcess*(max-min)/100; }
if(tErr) {
	//var = dmsk ? max+plcExcess*(max-min)/100 : min-plcExcess*(max-min)/100;
	EVAL = true;
}
else {
	vCalibr = iMult*(in+iAdd);

	vCalibr = (vCalibr-plcMin)/(plcMax-plcMin);
	varDt = min + (max-min)*(scSqr?pow(vCalibr,0.5):vCalibr) - var;
	var += varDt/max(1,Tf/1000*f_frq);

	bndVarHyst = (aMax-aMin)*HystBnd/100;
	if(aMax > aMin && (var >= aMax || (HH && var >= (aMax-bndVarHyst))))
	{ tErr = "3:"+tr("Upper alarm border error"); HH = true; EVAL = H = LL = L = false; }
	else if(aMax > aMin && (var <= aMin || (LL && var <= (aMin+bndVarHyst))))
	{ tErr="4:"+tr("Lower alarm border error"); LL = true; EVAL = HH = H = L = false; }
	else if(wMax > wMin && (var >= wMax || (H && var >= (wMax-bndVarHyst))))
	{ tErr="5:"+tr("Upper warning border error"); H = true; EVAL = HH = LL = L = false; }
	else if(wMax > wMin && (var <= wMin || (L && var <= (wMin+bndVarHyst))))
	{ tErr="6:"+tr("Lower warning border error"); L = true; EVAL = HH = H = LL = false; }
	else if(speed && varDt > speed)
	{ tErr = "7:"+tr("Too big parameter''s motion speed"); EVAL = true; HH = H = LL = L = false; }
	else EVAL = HH = H = LL = L = false;
}

if(!f_err.toInt() && tErr.toInt()) this.cntr().alarmSet((NAME.length?NAME:SHIFR)+": "+DESCR+": "+tErr.parse(1,":"), -4, SHIFR);
else if(f_err.toInt() && !tErr.toInt()) this.cntr().alarmSet((NAME.length?NAME:SHIFR)+": "+DESCR+": "+tr("NORMA"), 1, SHIFR);
f_err = tErr;','','',1450381549);
INSERT INTO "tmplib_base" VALUES('SNMP','','','','','','',10,0,'JavaLikeCalc.JavaScript
if(f_start)	{ srcPrm = false; items = new Object(); }

alLev = 0;
tErr = "";

//Connect to source
if(typeof(srcPrm) != "TCntrNode:TValue:TParamContr") srcPrm = SYS.DAQ.nodeAt(srcAddr,".");
if(!srcPrm) { tErr = tr("No connection to source object"); alLev = 3; }
else if(srcPrm.err.get() != 0)	 { tErr = tr("Source error")+": "+srcPrm.err.get().parse(1,":"); alLev = 3; }
else {
	//Attributes list get and "items" update
	nLst = srcPrm.nodeList("a_");
	lstTable = "";
	for(i_n = 0; i_n < nLst.length; i_n++) {
		aNd = srcPrm[nLst[i_n]];
		aId = nLst[i_n].slice(2);
		aIdPrc = aNd.descr();
		if(!aIdPrc.parse(1,"::").length) continue;
		if(aIdPrc.slice(-13) == "TotalNumber.0")	{ lstTable = aIdPrc.slice(0,-13); continue; }

		//Manual tables check
		if(!lstTable.length || lstTable != aIdPrc.slice(0,lstTable.length))
			for(off = 0; (iMnT=manTables.parse(0,";",off)).length; )
				if(iMnT == aIdPrc.slice(0,iMnT.length)) { lstTable = iMnT; break; }
		
		if(lstTable.length && lstTable == aIdPrc.slice(0,lstTable.length))
			aIdPrc = lstTable.parse(1,"::")+(prt1=aIdPrc.slice(lstTable.length)).parse(1,".")+prt1.parse(0,".");
		else aIdPrc = aIdPrc.parse(1,"::").parse(0,".");

		if(items[aIdPrc].isEVal()) {
			items[aIdPrc] = itW = new Object();
			itW.descr = aIdPrc;//aNd.descr();
			itW.id = aId;

			// Writeable check
			//SYS.messInfo("UPS test", aId+": aNd.flg()="+aNd.flg());
			if((itW.wr=!(aNd.flg()&0x04)) && aNd.flg()&0x01) {
				itW.wr = "";
				for(off = 0, pos = 0; (selId=aNd.values().parse(0,";",off)).length; pos++)
					itW.wr += ((selId==(selNm=aNd.selNames().parse(pos,";")))?selId:(selNm+" ("+selId+")"))+";";
			}
			itW.alarm = 0;
		}
		cVl = aNd.get();
		// Selectable value specifying
		if(aNd.flg()&0x01)
			for(off = 0, pos = 0; (selId=aNd.values().parse(0,";",off)).length; pos++)
				if(cVl == selId && cVl != aNd.selNames().parse(pos,";")) {		
					cVl = aNd.selNames().parse(pos,";")+"("+cVl+")";
					break;
				}
		items[aIdPrc].val = cVl;
	}

	//Alarms process and mark
	/*varS = "battery_packs";
	if(tP=srcPrm[varS])
	{
		if(tP.get().toInt() == 0)	{ items[varS].alarm = 2; tErr += "None good battery present; "; }
		else items[varS].alarm = 0;
	}*/

	//Set variables process
	for(var aIt in items) {
		it = items[aIt];
		if(!it.set.isEVal()) {
			aNd = srcPrm["a_"+it.id];
			if(aNd.flg()&0x01 && (selV=it.set.match(".+\\((.+)\\)$")).length) it.set = selV[1];
			aNd.set(it.set);
			it.set = EVAL_REAL;
		}
		alLev = max(alLev, it.alarm);
	}
}

//SYS.messInfo("UPS test", "tErr="+tErr+"; alLev="+alLev);
tErr = tErr.length ? ""+alLev+":"+tErr : "0";

//Alarms forming
if(tErr.toInt() && tErr.toInt() != f_err.toInt())
	this.cntr().alarmSet((NAME.length?NAME:SHIFR)+": "+DESCR+": "+tErr.parse(1,":"), -(2+alLev), SHIFR);
else if(f_err.toInt() && !tErr.toInt())
	this.cntr().alarmSet((NAME.length?NAME:SHIFR)+": "+DESCR+": "+tr("NORMA"), 1, SHIFR);
f_err = tErr;','','',1416765601);
CREATE TABLE 'lib_Controllers' ("ID" TEXT DEFAULT '' ,"NAME" TEXT DEFAULT '' ,"ru#NAME" TEXT DEFAULT '' ,"uk#NAME" TEXT DEFAULT '' ,"DESCR" TEXT DEFAULT '' ,"ru#DESCR" TEXT DEFAULT '' ,"uk#DESCR" TEXT DEFAULT '' ,"START" INTEGER DEFAULT '1' ,"MAXCALCTM" INTEGER DEFAULT '10' ,"PR_TR" INTEGER DEFAULT '1' ,"FORMULA" TEXT DEFAULT '' ,"ru#FORMULA" TEXT DEFAULT '' ,"uk#FORMULA" TEXT DEFAULT '' ,"TIMESTAMP" INTEGER DEFAULT '' , PRIMARY KEY ("ID"));
INSERT INTO "lib_Controllers" VALUES('prescr','Prescriptions manager','','','Prescriptions manager and controller. Used in addition with user interface''s cadre "Prescription: editing" and "Prescription: runtime" for which into a parameter of the controller you must pass that parameters: "mode", "prog", "startTm", "curCom", "comLs", "work".
Author: Roman Savochenko <rom_as@oscada.org>
Sponsor: Vasiliy Grigoriev from "Vacuum technologies laboratory (http://e-beam.ru)".
Version: 1.1.0','','',1,10,0,'clcCnt++;

if(f_start)	work = SYS.XMLNode("prg");

//Check to commands controller present
comCntrO = false;
if(comsCntr.length)	comCntrO = SYS.DAQ.nodeAt(comsCntr,".");

//Commands list update
if(f_start || !(clcCnt%(60*f_frq))) {
	if(!comCntrO) {
		rez = SYS.BD.nodeAt(dbDB,".").SQLReq("SELECT * FROM "+dbComs+";");
		comLs = new Object();
		for(i_t = 1; i_t < rez.length; i_t++) {
			comNm = rez[i_t]["name"];
			comLs[comNm] = new Object();
			comLs[comNm]["descr"] = rez[i_t]["descr"];
			comLs[comNm]["proc"] = rez[i_t]["proc"];
			for(i_a = 1; i_a <= 5; i_a++)
				comLs[comNm]["arg"+i_a] = rez[i_t]["arg"+i_a];
		}
	}
	else {
		var nL = comCntrO.nodeList("prm_");
		comLs = new Object();
		for(i_n = 0; i_n < nL.length; i_n++) {
			comO = comCntrO[nL[i_n]];
			comNm = comO.cfg("NAME");
			comLs[comNm] = new Object();
			comLs[comNm]["prmID"] = nL[i_n].slice(4);
			comLs[comNm]["descr"] = comO.cfg("DESCR");
			for(i_a = 1; i_a <= 5; i_a++)
				if(!(comA=comO["arg"+i_a]).isEVal())
					comLs[comNm]["arg"+i_a] = comA.descr();
		}
	}
}

//Programm selection change
if(curMode <= 0 && prog.length && (prog != work.attr("name") || mode == 1)) {
	work = SYS.XMLNode("prg");
	rez = SYS.BD.nodeAt(dbDB,".").SQLReq("SELECT prgTxt FROM "+dbProgs+" WHERE name=''"+prog+"'';");
	if(rez.length > 1) {
		//Parse program and insert procedure text and argument''s labels
		work.load(rez[1][0]);
		work.setAttr("name",prog);
		for(comElN = work, comPos = 0; true; comPos++) {
			if(comPos >= comElN.childSize()) {
				if((comElN=comElN.parent())) { comPos = comElN.attr("seekPos").toInt(); continue; }
				break;
			}
			comElI = comElN.childGet(comPos);
			comId = comElI.attr("id");
			if(comLs[comId].isEVal())	continue;
			comElI.setAttr("proc",comLs[comId].proc);
			for(i_a = 1; i_a <= 5; i_a++) comElI.setAttr("labArg"+i_a,comLs[comId]["arg"+i_a]);
			if(comElI.childSize()) { comElN.setAttr("seekPos",comPos); comElN = comElI; comPos = -1; }
		}
	}
	else prog = "";
}

//Start process
if(curMode <= 0 && mode == 1 && prog.length) {
	startTm = SYS.time();
	curCom = 0;
	curMode = mode;	

	if(comCntrO) {
		var cL = comCntrO.nodeList("prm_");
		for(i_c = 0; i_c < cL.length; i_c++) {
			var cLi = comCntrO[cL[i_c]];
			cLi.run.set(false);
			cLi.stop.set(false);
			cLi.error.set(false);
			cLi.abort.set(false);
			cLi.start.set(true);
		}
	}
}
//Pause and other process
else if((mode == 0 && curMode < 0) || (curMode == 1 && mode == 2) || (curMode == 2 && (mode == 0 || mode == 1))) {
	//curMode = mode;
	if((mode == 0 || mode == 1 || mode == 2) && comCntrO) {
		var cL = comCntrO.nodeList("prm_");
		for(i_c = 0; i_c < cL.length; i_c++)
			comCntrO[cL[i_c]].pause.set(mode==2);
	}
}

//Call program
if(curMode == 1 || curMode == 2) {
	prog = work.attr("name");

	//Curent node at levels obtain
	curComNd = work;
	for(curComLev = 0, curComPos = 0, off = 0; curComNd && (tVl=curCom.parse(0,":",off)).length; curComLev++, curComPos = tVl.toInt())
		curComNd = curComNd.childGet(tVl.toInt());
	if(!curComNd) {
		SYS.messInfo("uprg"+prog,tr("No current node present")+" \""+prog+"\" : "+SYS.strftime(startTm)+" : "+SYS.strftime(SYS.time()));
		curMode = mode = -3;
	}
	curComNd = curComNd.parent();

	//Internal commands call
	if(!comCntrO) {
		if(mode == 3 && curCom >= 0 && curCom < work.childSize()) {
			work.childGet(curCom).setAttr("rez", "-10:"+tr("Step missed"));
			curCom++;
		}
		if(curCom >= 0 && curCom < work.childSize()) {
			comEl = work.childGet(curCom);
			//Stop process
			if(mode == 0) {
				comEl.setAttr("rez","-10:"+tr("Program terminated"));
				// Call stop command
				if(!comLs["Stop"].isEVal()) {
					off = 0;
					prLang = comLs["Stop"].proc.parse(0,"\n",off);
					SYS.DAQ.funcCall(prLang,new Object(),comLs["Stop"].proc.slice(off));
				}
				SYS.messInfo("uprg"+prog,tr("Terminated by user session of the program")+" \""+prog+"\" : "+SYS.strftime(startTm)+" : "+SYS.strftime(SYS.time()));
				curMode = mode = -3;
			}
			//Pass empty command		
			else if(!comEl.attr("proc").length) curCom++;
			//Execute command
			else if(curMode == 1) {
				args = new Object();
				args.rez = "";
				args.f_start = !comEl.attr("tm").length;
				if(args.f_start) comEl.setAttr("tm",SYS.time());
				args.f_frq = f_frq;
				for(i_a = 1; i_a <= 5; i_a++) args["arg"+i_a] = comEl.attr("arg"+i_a);
				for(i_a = 1; i_a <= 10; i_a++) args["tmp"+i_a] = comEl.attr("tmp"+i_a);
				off = 0;
				prLang = comEl.attr("proc").parse(0,"\n",off);
				rez = SYS.DAQ.funcCall(prLang,args,comEl.attr("proc").slice(off));
				if(rez && args.rez.toInt() > 0) curCom++;
				else if(!rez || args.rez.toInt() < 0) {
					// Call error command
					if(!comLs["Error"].isEVal()) {
						off = 0;
						prLang = comLs["Error"].proc.parse(0,"\n",off);
						SYS.DAQ.funcCall(prLang,new Object(),comLs["Error"].proc.slice(off));
					}
					SYS.messInfo("uprg"+prog,tr("Terminated by error session of the program")+" \""+prog+"\" : "+SYS.strftime(startTm)+" : "+SYS.strftime(SYS.time()));
					curMode = mode = -1;
				}
				comEl.setAttr("rez",args.rez);
				for(i_a = 1; i_a <= 5; i_a++) comEl.setAttr("arg"+i_a, args["arg"+i_a]);
				for(i_a = 1; i_a <= 10; i_a++) comEl.setAttr("tmp"+i_a, args["tmp"+i_a]);
				//SYS.messDebug("TEST Calc","TEST Calc rezult: "+args.y);
			}
		}

		//End call
		if(curCom < 0 || curCom >= work.childSize()) {
			// Call stop command
			if(!comLs["Stop"].isEVal()) {
				off = 0;
				prLang = comLs["Stop"].proc.parse(0,"\n",off);
				SYS.DAQ.funcCall(prLang,new Object(),comLs["Stop"].proc.slice(off));
			}

			curMode = mode = -2;
			SYS.messInfo("uprg"+prog,tr("Successful session of the program")+" \""+prog+"\" : "+SYS.strftime(startTm)+" : "+SYS.strftime(SYS.time()));

			//Place to program last execution time
			rez = SYS.BD.nodeAt(dbDB,".").SQLReq("SELECT prgTxt FROM "+dbProgs+" WHERE name=''"+prog+"'';");
			if(rez.length > 1) {
				//Parse programm and insert procedure text and argument''s labels to here
				comTree = SYS.XMLNode("prg");
				comTree.load(rez[1][0]);
				comTree.setAttr("wtm",SYS.time()-startTm);
				SYS.BD.nodeAt(dbDB,".").SQLReq("UPDATE "+dbProgs+" SET prgTxt=''"+comTree.save().replace("''","''''")+"'' WHERE name=''"+prog+"'';");
			}
		}
	}
	//External commands call
	else {
		// Get current command parameter-object		
		toNext = toStop = toPass = false;
		isRun = actBackGrnd = false;

		curComPrm = EVAL_BOOL;
		if(curComPos >= 0 && curComPos < curComNd.childSize()) {
			if(!(comId=comLs[curComNd.childGet(curComPos).attr("id")]).isEVal()) curComPrm = comCntrO["prm_"+comId.prmID];
			if(curComPrm.isEVal()) {
				curMode = mode = -1;
				curComNd.childGet(curComPos).setAttr("rez","-11:"+tr("Command miss: ")+work.childGet(curCom).attr("id"));
				return;
			}
		}
		if(mode == 3 && curComPos >= 0 && curComPos < curComNd.childSize()) {
			//curComNd.childGet(curComPos).setAttr("rez","-10:"+tr("Step missed"));
			//curComPrm.run.set(false);
			mode = curMode;
			toNext = toPass = true;
		}
		else if(curComPos >= 0 && curComPos < curComNd.childSize()) {
			comEl = curComNd.childGet(curComPos);

			//Stop process
			if(mode == 0) {
				toStop = true;
				//comEl.setAttr("rez","-12:"+tr("Program terminated"));
				// Stop all typical and set "abort" flag
				var cL = comCntrO.nodeList("prm_");
				for(i_c = 0; i_c < cL.length; i_c++) {
					var cLi = comCntrO[cL[i_c]];
					cLi.run.set(false);
					cLi.start.set(false);
					cLi.abort.set(true);
				}
				SYS.messInfo("uprg"+prog, tr("Terminated by user session of the program")+" \""+prog+"\" : "+SYS.strftime(startTm)+" : "+SYS.strftime(SYS.time()));
				curMode = mode = -3;
			}
			//Commands process
			else {
				// Start command
				if(!comEl.attr("tm").length) {
					curComPrm.run.set(false);	//Stop for possible background call
					comEl.setAttr("tm",SYS.time());
					for(i_a = 1; i_a <= 5; i_a++)
						if(!(comA=curComPrm["arg"+i_a]).isEVal())
							comA.set(comEl.attr("arg"+i_a));
					curComPrm.rez.set(0);
					curComPrm.run.set(true);
				}
				isRun = true;
			}
		}
		//Update steps status, up to the current comand
		isCurCmd = false;
		for(comElN = work, comPos = 0, comLev = 0, comCur = true; isRun || toPass || toStop; comPos++) {
			if(comPos >= comElN.childSize()) {
				if((comElN=comElN.parent())) {
					comLev--;
					comPos = comElN.attr("seekPos").toInt();
					comCur = comElN.attr("comCur").toInt();
					continue;
				}
				if(actBackGrnd && toNext) toNext = false;
				break;
			}
			comElI = comElN.childGet(comPos);
			isBackgrnd = comElI.attr("backgrnd").toInt();
			if(!isBackgrnd && actBackGrnd)	toNext = false;
			if(isCurCmd) break;
			isCurCmd = comCur && comLev == (curComLev-1) && curCom.parse(comLev,":").toInt() == comPos;
			curComPI = comCntrO["prm_"+comLs[comElI.attr("id")].prmID];
			rez = curComPI.rez.get();
			if(isCurCmd || ((toStop || curComPI.run.get()) && comElI.attr("rez").toInt() != 1 && comElI.attr("rez").toInt() > -10)) {
				if(toStop)	rez = "-12:" + tr("Program terminated");
				else if(toPass) {
					rez = "-10:" + tr("Step missed");
					curComPI.run.set(false);
				}
				else if(rez.toInt() < 0) {
					// Stop all typical and call "error" command
					var cL = comCntrO.nodeList("prm_");
					for(i_c = 0; i_c < cL.length; i_c++) {
						var cLi = comCntrO[cL[i_c]];
						cLi.run.set(false);
						cLi.start.set(false);
						cLi.error.set(true);
					}
					SYS.messInfo("uprg"+prog,tr("Terminated by error session of the program")+" \""+prog+"\" : "+SYS.strftime(startTm)+" : "+SYS.strftime(SYS.time()));
					curMode = mode = -1;
				}
				if(!actBackGrnd && isBackgrnd && rez.toInt() == 0) actBackGrnd = true;
				if(isCurCmd && (rez.toInt() > 0 || isBackgrnd) && !toStop)	toNext = true;
				comElI.setAttr("rez", rez);
				for(i_a = 1; i_a <= 5; i_a++)
					if(!(comA=curComPI["arg"+i_a]).isEVal())
						comElI.setAttr("arg"+i_a, comA.get());
			}
			if(isCurCmd) continue;
			//Force backgrounded rezult update for running
			if(rez.toInt() != 0 && comElI.attr("rez").toInt() == 0)	comElI.setAttr("rez", rez);

			if(comElI.childSize()) {
				comElN.setAttr("seekPos", comPos);
				comElN.setAttr("comCur", comCur);
				comElN = comElI;
				comCur = comCur && comLev < curComLev && curCom.parse(comLev,":").toInt() == comPos;
				comPos = -1; comLev++;
			}
		}

		// Go to next step
		if(toNext) {
			if(curComNd.childGet(curComPos).childSize())	{ curComLev++; curComPos = 0; }
			else if((++curComPos) >= curComNd.childSize())
			{ curComLev--; curComPos = curComLev ? curCom.parse(curComLev-1,":").toInt()+1 : curComNd.childSize(); }
			for(i_c = 0, curComN = ""; i_c < (curComLev-1); i_c++) curComN += curCom.parse(i_c,":");
			curCom = curComN.length ? curComN+":"+curComPos : curComPos;
		}
		
		//End call
		if(curComLev <= 0) {
			// Stop all typical and call "stop" command
			var cL = comCntrO.nodeList("prm_");
			for(i_c = 0; i_c < cL.length; i_c++) {
				var cLi = comCntrO[cL[i_c]];
				cLi.run.set(false);
				cLi.start.set(false);
				cLi.stop.set(true);
			}

			curMode = mode = -2;
			SYS.messInfo("uprg"+prog,tr("Successful session of the program")+" \""+prog+"\" : "+SYS.strftime(startTm)+" : "+SYS.strftime(SYS.time()));

			//Place to program last execution time
			rez = SYS.BD.nodeAt(dbDB,".").SQLReq("SELECT prgTxt FROM "+dbProgs+" WHERE name=''"+prog+"'';");
			if(rez.length > 1) {
				//Parse program and insert procedure text and argument''s labels to here
				comTree = SYS.XMLNode("prg");
				comTree.load(rez[1][0]);
				comTree.setAttr("wtm",SYS.time()-startTm);
				SYS.BD.nodeAt(dbDB,".").SQLReq("UPDATE "+dbProgs+" SET prgTxt=''"+comTree.save().replace("''","''''")+"'' WHERE name=''"+prog+"'';");
			}
		}
	}
}
curMode = mode;
//mode = curMode;','','',1508607633);
INSERT INTO "lib_Controllers" VALUES('test','test','test','','Different tests of the JavaLikeCalc language for execution into the controller mode.
Author: Roman Savochenko
Version: 1.0.0
','','',1,10,0,'using Special.FLibSYS;

out+=10;
if(out>100) out=0;

test = (10).toString(16,2,true);
test = (-10.34).toFixed(4,8);

//Request prepare
//req = SYS.XMLNode("#").setAttr("ProtIt","DCON").setAttr("addr",10);
//Send request
//SYS.Transport["Serial"]["out_TestDCON"].messIO(req,"UserProtocol");
//if(!req.attr("err").length) SYS.messDebug("TEST REQ","RES: "+req.text());

//test = "Text \n value[\041abc\x21].";

//test = arguments[4];
//arguments[4] = 10;

//test="Java123Script".search(new RegExp("script","i"));
//text = "Javascript3\"sdf\"3javaScript __ javascript";
//test = text.replace(4,3,"67");
//test = text.replace("3","55");
//test = text.replace(new RegExp("javascript","ig"),"JavaScript");//
//test = text.replace(new RegExp("\"([^\"]*)\"","g"),"``$1''''");
//text = "1 плюс 2 плюс 3";
//test = text.match("\\d+","g");
//text = "Посетите мою домашнюю страницу http://www.isp.com/~david";
//test = text.match("(\\w+):\\/\\/([\\w.]+)\\/(\\S*)");
//test = "1,2, 3 , 4 ,5".split(new RegExp("\\s*,\\s*"));
//test = "hello <b>world</b>".split(new RegExp("(<[^>]*>)"));
//var p = new RegExp("(\\d\\d)[-/](\\d\\d)[-/](\\d\\d(?:\\d\\d)?)","");
//test = p.test("abc12/30/1969def");

/*CRC = new Array(0,1,2,3,4,5);vSz = 3
hi=CRC[out%6];
test = hi;*/

//test = SYS.Transport.Sockets.out_testModBus.messIO("123456");

/*obj = new Object();
obj["test1"]=1;l
obj["test2"]=1;
for( var i in obj ) SYS.messDebug("test","Prop: "+i);
SYS.messDebug("test","Props test");*/

//vl = SYS.strFromCharCode(100,200,50,60);
//test = vl.charCodeAt(1);

//test = SYS.system("ps -Ao pid,comm");

//ModBus.Unidrive.pos.pos=out;
//ModBus.testRTU.test.pos=out;
//test = rand(offset*0.4);
//val="Test";
//if( val=="Test1" || val=="Test2" ) test="TestN"; else test="Test";

//test1=new Array(1,"a",2,"b",3,"c",4);
//test1[7]=new Array(5,6,7);
//test=test1[7][1];

//test1="ab;cde;fgh;jk;lm";
//test = test1.split(";").join();

//req = xmlNode("get").setAttr("path","/%2fgen%2fid");
//test = xmlCntrReq(req);
//test = req.text();

/*test1 = vArh("DAQ.System.AutoDA.CPULoad.load");
test2 = test1.FFT(0,60);
test = "";
for( i = 0; i < test2.length; i++ ) test += ""+test2[i]+";";*/

//messPut("cron",0,"Time: "+tmFStr(tmTime()));

/*req = SYS.XMLNode();
test = req.load("test1.xml",true);*/
//for( i = 0; i < req.childSize(); i++ )
//  test += req.childGet(i).attr("id")+"; ";
/*sutm = 0; stm = SYS.time(sutm);
req.save(0,"test2.xml");
eutm = 0; etm = SYS.time(eutm);
test = 1e6*(etm-stm)+(eutm-sutm);*/

//test = strDec4Bin(Transport.Sockets.out_testModBus.messIO(strEnc2Bin("15 01 00 00 00 06 01 03 00 00 00 05")));

/*req = SYS.XMLNode("GET");
req.setAttr("URI","/");
SYS.Transport.Sockets.out_testHTTP.messIO(req,"HTTP");
test = req.text();*/

/*req = SYS.XMLNode("POST");
req.setAttr("URI","/WebUser/FlowTec.txt");
cntNode = req.childAdd("cnt").setAttr("name","pole0").setAttr("filename","Object2-k001-100309-17.txt");
cntNode.childAdd("prm").setAttr("id","Content-Type").setText("text/plain");
cntText = "Object2-k001\r\n";
cntText += "\r\n";
cntText += "v002\r\n";
cntText += " n1\r\n";
cntText += "  09.03.10 16   Polnyj    7155.25    216.0  32.000  17.5\r\n";
cntText += "v005\r\n";
cntText += " n1\r\n";
cntText += "  09.03.10 16   Polnyj     188.81    350.0   4.000  40.0\r\n";
cntText += "\r\n";
cntNode.setText(cntText);
SYS.Transport.Sockets.out_testHTTP.messIO(req,"HTTP");*/

//test = SYS.UI.VCAEngine["ses_AGLKS"].alrmSndPlay();
//test = "1;2;5;7;9";
//test = "1;2;5;7;9".type();

//SYS.messDebug("test",strEnc2Bin("68 74 74 70 3a 2f 2f 77 77 77 2e 77 33 2e 6f 72 67 2f 32 30 30 30 2f 30 39 2f 78 6d 6c 64 73 69 67 23 72 73 61 2d 73 68 61 31"));

//test = SYS.DAQ.ModBus.testTCP.test.a_8_1_1_ПС_ОПД_стенд_температура__ОПД_.get();

/*req = SYS.XMLNode("get").setAttr("path","/%2fgen%2fid");
rez = SYS.cntrReq(req,"loop");
test = req.text();*/

/*DBTbl=SYS.BD.MySQL.GenDB.SQLReq("SELECT * from DB;");
for( var i_rw = 0; i_rw < DBTbl.length; i_rw++ )
{
  var rec = "";
  for( var i_fld = 0; i_fld < DBTbl[i_rw].length; i_fld++ )
    rec += DBTbl[i_rw][i_fld]+"\t";
  SYS.messDebug("TEST DB","Row "+i_rw+": "+rec);
}*/','','',1479056226);
INSERT INTO "lib_Controllers" VALUES('test1','test1','','test1','Different tests of the JavaLikeCalc language for execution into the controller mode.
Author: Roman Savochenko
Version: 1.0.0','','',1,10,0,'//clc=0;
//First getting previous time
//if(!p_tm){ p_tm = Special.FLibSYS.tmTime(); break; }

//Get curent time
//c_tm = Special.FLibSYS.tmTime()-2;

//Open value archive
//a_id = Special.FLibSYS.avalOpen("Archive.va_test");
//if(a_id==-1) break;

//Calc average archive
//for(i_tm=p_tm;i_tm<c_tm;i_tm++)
//{ 
//  val = Special.FLibSYS.avalGetR(a_id,i_tm,0,"");
//  rez = (rez*(i_tm-p_tm)+val)/(i_tm-p_tm+1);
//  clc++;
//}

//Close value archive
//Special.FLibSYS.avalClose(a_id);
//p_tm=c_tm;','','',1488128862);
INSERT INTO "lib_Controllers" VALUES('ntfDispatch','Notifications dispatcher','','','Notifications dispatcher by EMail and SMS for pointed messages of OpenSCADA messages buffer.
Author: Roman Savochenko <rom_as@oscada.org>
Sponsor: Oleksandr Knestyapin <olexanderrr@gmail.com>
Version: 1.3.1','','',1,30,0,'//Initial
if(f_start) {
	queue = new Object();
	queueSMS = new Object();
	prTm = SYS.time();
	prTmU = 0;
	emailSentN = SMSSentN = 0;
	emailServer_ = emailServer;
	SMTPTr = emailServer.length ? SYS.Transport.nodeAt(emailServer,".") : false;
	SMSTrId_ = SMSTrId;
	SMSTr = SMSTrId.length ? SYS.Transport.Serial.nodeAt("out_"+SMSTrId) : false;
	return;	//Don''t real process at start
}
if(f_stop)	{ emailState = SMSState = ""; return; }
if(emailServer_ != emailServer) {
	SMTPTr = emailServer.length ? SYS.Transport.nodeAt(emailServer,".") : false;
	emailServer_ = emailServer;
}
if(SMSTrId_ != SMSTrId)	{
	SMSTr = SMSTrId.length ? SYS.Transport.Serial.nodeAt("out_"+SMSTrId) : false;
	SMSTrId_ = SMSTrId;
}

//Current alarms reading, and the queue of dispatching update
mess = SYS.Archive.messGet(prTm, SYS.time(), messCat, abs(messLev), "<buffer>");
for(iM = 0; iM < mess.length; iM++) {
	oM = mess[iM];
	if((oM.tm == prTm && oM.utm <= prTmU) || (messLev < 0 && oM.level >= 0))	continue;

	if(emailServer.length)	queue[oM.level.toString()+oM.tm.toString()+oM.categ] = oM;
	if(SMSTr)	queueSMS[oM.level.toString()+oM.tm.toString()+oM.categ] = oM;

	prTm = oM.tm; prTmU = oM.utm;
	//SYS.messInfo("NTF","Queue mess: "+oM.mess);
}

//The notifications mail queue process in a cycle limited by overall time
stTm = SYS.time();
emailQueueN = 0;
emailErr = "";
for(var nM in queue) {
	if(!SMTPTr)	break;
	if((SYS.time()-stTm) > tmOut)	{ emailQueueN++; continue; }
	oM = queue[nM];
	//SYS.messInfo("NTF","Send mess \""+nM+"\": "+oM.mess);

	topic = emailTopic + " (" + ((oM.level < 0)?tr("Alarm"):tr("Norm")) + ")";
	mess = emailMess + SYS.strftime(oM.tm) + ": " + oM.mess;

	req = SYS.XMLNode("send");
	req.setAttr("ProtIt", "SMTP").setAttr("auth", emailAuth).setAttr("from", emailSender).setAttr("to", emailReceiver).setAttr("topic", topic).setText(mess);
	SMTPTr.messIO(req, "UserProtocol");
	//SYS.messInfo("NTF", "Send error: "+req.attr("err"));
	if(!req.attr("err").toInt()) { delete queue[nM]; emailSentN++; }
	else {
		emailQueueN++;
		if(!emailErr.length)	emailErr = req.attr("err");
	}

	/*req = "sendemail -q -f " + emailSender + " -t " + emailReceiver +
			" -u \"=?utf8?B?" + SYS.strEncode(topic,"Base64") + "?=\" -m \"" + mess + "\" -s " + emailServer;
	if(emailAuth.length)	req += " -xu " + emailAuth.parse(0,":") + " -xp " + emailAuth.parse(1,":");
	req += " -o tls=no -o message-charset=utf8";
	//SYS.messInfo("NTF","Send mess: "+req);
	rez = SYS.system(req, true);
	if(!rez) { delete queue[nM]; emailSentN++; }
	else {
		emailQueueN++;
		if(!emailErr.length)	emailErr = req.attr("err");
	}*/
}
if(!SMTPTr)	emailState = "Disabled!";
else {
	emailState = tr("Sent %1. In queue %2.").replace("%1",emailSentN.toString()).replace("%2",emailQueueN.toString());
	if(emailErr.length) emailState += " "+tr("Error: %1.").replace("%1",emailErr);
}

//The notifications SMS queue process in a cycle limited by overall time
stTm = SYS.time();
SMSQueueN = 0;
SMSErr = "";
for(var nM in queueSMS) {
	if(!SMSTr)	break;
	if((SYS.time()-stTm) > tmOut)	{ SMSQueueN++; continue; }
	oM = queueSMS[nM];
	//SYS.messInfo("NTF","Send SMS \""+nM+"\": "+oM.mess);

	mess = oM.mess;
	req = SYS.XMLNode("send"); req.setAttr("ProtIt", "SMS").setAttr("pin", SMSPin).setAttr("text",SMSTextMd).setAttr("tel", SMSTel).setText(mess);
	SMSTr.messIO(req, "UserProtocol");
	if(!req.attr("err").toInt()) { delete queueSMS[nM]; SMSSentN++; }
	else {
		SMSQueueN++;
		if(!SMSErr.length)	SMSErr = req.attr("err");
	}
}
if(!SMSTr)	SMSState = "Disabled!";
else {
	SMSState = tr("Sent %1. In queue %2.").replace("%1",SMSSentN.toString()).replace("%2",SMSQueueN.toString());
	if(SMSErr.length) SMSState += " "+tr("Error: %1.").replace("%1",SMSErr);
}','','',1480872892);
CREATE TABLE 'lib_servProc' ("ID" TEXT DEFAULT '' ,"NAME" TEXT DEFAULT '' ,"uk#NAME" TEXT DEFAULT '' ,"ru#NAME" TEXT DEFAULT '' ,"DESCR" TEXT DEFAULT '' ,"uk#DESCR" TEXT DEFAULT '' ,"ru#DESCR" TEXT DEFAULT '' ,"START" INTEGER DEFAULT '1' ,"MAXCALCTM" INTEGER DEFAULT '10' ,"PR_TR" INTEGER DEFAULT '0' ,"FORMULA" TEXT DEFAULT '' ,"uk#FORMULA" TEXT DEFAULT '' ,"ru#FORMULA" TEXT DEFAULT '' ,"TIMESTAMP" INTEGER DEFAULT '' , PRIMARY KEY ("ID"));
INSERT INTO "lib_servProc" VALUES('procArh','Archives recalc','Перерахунок архівів','Пересчёт архивов','Value archives recalculation.
Author: Roman Savochenko <rom_as@oscada.org>
Version: 1.0.0','','',1,600,0,'using Special.FLibSYS;

//messPut("UserProc/ProcessArch",1,"Archive open");
a_src = vArh(fromarch);
a_dst = vArh(toarch);

//messPut("UserProc/ProcessArch",1,"Archive process");

bf_per = 100;	//Buffers period microseconds
bf_size = 100000;	//Buffers size

//Open value buffers
bf_src = vArhBuf(1,bf_size,bf_per,true,true);
bf_dst = vArhBuf(4,bf_size,bf_per,true,true);

//Parse time string
begtm = tmStrPTime(begs);
endtm = tmStrPTime(ends);

begwtm = begtm;
while( begwtm<endtm )
{
    //Get value
    endwtm = begwtm+bf_size*bf_per/1000000;
    if(endwtm>endtm) endwtm = endtm;
    messPut("UserProc/ProcessArch",1,"Values from: "+begwtm+", to: "+endwtm);
    bf_src.copy(a_src,begwtm,0,endwtm,0);

    buf_beg = begwtm; buf_beg_u = 0;
    buf_end = endwtm; buf_end_u = 0;
    while( !(buf_beg>=buf_end && buf_beg_u>buf_end_u) )
    {
        r_val = bf_src.get(buf_beg,buf_beg_u,true);
        if(r_val == EVAL_INT) bf_dst.set(EVAL_REAL,buf_beg,buf_beg_u);
        else bf_dst.set(200.*((10.*r_val/32768.)-1.)/4.,buf_beg,buf_beg_u);

        buf_beg_u++;
    }

    a_dst.copy(bf_dst,begwtm,0,endwtm,0);
    begwtm = endwtm;
}','','',1479061485);
INSERT INTO "lib_servProc" VALUES('crc16','CRC 16','','','Standard Cyclic Redundancy Check (CRC) with free polynomial set, by default it is 0x8005. It isn''t actual more by the common CRC implementing.
Author: Roman Savochenko <rom_as@oscada.org>
Version: 1.0.0','','',1,10,0,'out = 0xFFFF;
for(i = 0; i < in.length; i++) {
  out = out^in.charCodeAt(i);
  for(j = 0; j < 8; j++) out = (out&1) ? ((out>>1)^poly) : out >> 1;
}','','',1479061164);
INSERT INTO "lib_servProc" VALUES('releaseTests','TEST: Release','Тести випуску','Тесты выпуска','OpenSCADA releases formal sets pack in list:
  - 0:System
  - 1:Security User/Group tests
  - 2:Transport subsystem tests
  - 3:Archive subsystem tests
  - 4:DB subsystem tests
  - 5:Protocol subsystem tests
  - 6:DAQ subsystem tests
  - 7:UI subsystem tests
  - 8:Special subsystem tests
Author: Roman Savochenko <rom_as@oscada.org>
Version: 1.0.0','','',1,10,0,'nNm = "testNode";
node  = false;		//Process node
testREZ = "";		//Test rezult
rez = "";

//**********************************
//Get config file name
SYS.cntrReq(req=SYS.XMLNode("get").setAttr("path","/%2fgen%2fconfig"));
configPath = req.text();
//Save original config file
SYS.fileWrite(configPath+".orig",SYS.fileRead(configPath));
//Load config file tree
cfgTree=SYS.XMLNode();
cfgTree.load(configPath,true,true);

//**********************************
//System
if(sub <= 0) {
	testREZ = SYS.Special.SystemTests.XML(configPath);
	rez += "<tr><td>Control interface: Test of the XML file parsing</td><td>"+testREZ+"</td></tr>\n";
	testREZ = SYS.Special.SystemTests.SysContrLang("/Archive/FSArch/mess_StatErrors/%2fprm%2fst");
	rez += "<tr><td>Control interface: Test of the control system language</td><td>"+testREZ+"</td></tr>\n";
	testREZ = SYS.Special.SystemTests.Base64Code();
	rez += "<tr><td>SYS: Tests of the Mime Base64 encoding algorithm</td><td>"+testREZ+"</td></tr>\n";
}

//**********************************
//Security User/Group tests
if(sub < 0 || sub == 1) {
	testREZ = "PASSED";
	// User
	nAddr = "/Security/usr_"+nNm;
	if(SYS.cntrReq(SYS.XMLNode("add").setAttr("path","/Security/%2fbr%2fusr_").setText(nNm)) != 0 || !(node=SYS.nodeAt(nAddr))) testREZ = "FAILED: user add";
	else if(SYS.cntrReq(SYS.XMLNode("set").setAttr("path",nAddr+"/%2fprm%2fdscr").setText("Test")) != 0 || node.cfg("DESCR") != "Test") testREZ = "FAILED: change user";
	else if(SYS.cntrReq(SYS.XMLNode("save").setAttr("path",nAddr+"/%2fobj")) != 0) testREZ = "FAILED: save user";
	else if(SYS.cntrReq(SYS.XMLNode("set").setAttr("path",nAddr+"/%2fprm%2fdscr").setText("Test 1")) != 0 || 
		SYS.cntrReq(SYS.XMLNode("load").setAttr("path",nAddr+"/%2fobj")) != 0 || node.cfg("DESCR") != "Test") testREZ = "FAILED: load user";
	else {
		node = false;
		if(SYS.cntrReq(SYS.XMLNode("del").setAttr("path","/Security/%2fbr%2fusr_").setText(nNm)) != 0 || SYS.nodeAt(nAddr)) testREZ = "FAILED: user delete";
		else {
			cfgTree.getElementBy("sub_Security").childAdd("tbl").setAttr("id","Security_user").childAdd("fld").setAttr("NAME",nNm+"_cfg");
			cfgTree.save(0x20, configPath);
			SYS.cntrReq(SYS.XMLNode("scan").setAttr("path","/%2fgen%2fconfig"));		//Reload config command
			if(!SYS.nodeAt(nAddr+"_cfg")) testREZ = "FAILED: load user from config";
		}
	}
	//Group
	nAddr = "/Security/grp_"+nNm;
	if(testREZ == "PASSED" && (SYS.cntrReq(SYS.XMLNode("add").setAttr("path","/Security/%2fbr%2fgrp_").setText(nNm)) != 0 || !(node=SYS.nodeAt(nAddr)))) testREZ = "FAILED: group add";
	else if(SYS.cntrReq(SYS.XMLNode("set").setAttr("path",nAddr+"/%2fprm%2fdscr").setText("Test")) != 0 || node.cfg("DESCR") != "Test") testREZ = "FAILED: change group";
	else if(SYS.cntrReq(SYS.XMLNode("save").setAttr("path",nAddr+"/%2fobj")) != 0) testREZ = "FAILED: save group";
	else if(SYS.cntrReq(SYS.XMLNode("set").setAttr("path",nAddr+"/%2fprm%2fdscr").setText("Test 1")) != 0 || 
		SYS.cntrReq(SYS.XMLNode("load").setAttr("path",nAddr+"/%2fobj")) != 0 || node.cfg("DESCR") != "Test") testREZ = "FAILED: load group";
	else {
		node = false;
		if(SYS.cntrReq(SYS.XMLNode("del").setAttr("path","/Security/%2fbr%2fgrp_").setText(nNm)) != 0 || SYS.nodeAt(nAddr)) testREZ = "FAILED: group delete";
		else {
			cfgTree.getElementBy("sub_Security").childAdd("tbl").setAttr("id","Security_grp").childAdd("fld").setAttr("NAME",nNm+"_cfg").setAttr("USERS",nNm+"_cfg");
			cfgTree.save(0x20, configPath);
			SYS.cntrReq(SYS.XMLNode("scan").setAttr("path","/%2fgen%2fconfig"));		//Reload config command
			if(!SYS.nodeAt(nAddr+"_cfg")) testREZ = "FAILED: load group from config";
		}
	}
	rez += "<tr><td>Security: User/Group create/remove, change, load, save and load from config</td><td>"+testREZ+"</td></tr>\n";
}

//**********************************
//Transport subsystem tests
if(sub < 0 || sub == 2) {
	nList = SYS.Transport.nodeList("mod_");
	cfgTree.getElementBy("sub_Transport").childAdd("tbl").setAttr("id","Transport_in");
	cfgTree.getElementBy("sub_Transport").childAdd("tbl").setAttr("id","Transport_out");
	for(i_n = 0; i_n < nList.length; i_n++) {
		testREZ = "PASSED";
		//Input transport
		nAddr = "/Transport/"+nList[i_n]+"/in_"+nNm;
		if(SYS.cntrReq(SYS.XMLNode("add").setAttr("path","/Transport/"+nList[i_n]+"/%2fbr%2fin_").setAttr("id",nNm)) != 0 || !(node=SYS.nodeAt(nAddr))) testREZ = "FAILED: input add";
		else if(SYS.cntrReq(SYS.XMLNode("set").setAttr("path",nAddr+"/%2fprm%2fcfg%2fdscr").setText("Test")) != 0 || node.cfg("DESCRIPT") != "Test") testREZ = "FAILED: change input";
		else if(SYS.cntrReq(SYS.XMLNode("save").setAttr("path",nAddr+"/%2fobj")) != 0) testREZ = "FAILED: save input";
		else if(SYS.cntrReq(SYS.XMLNode("set").setAttr("path",nAddr+"/%2fprm%2fcfg%2fdscr").setText("Test 1")) != 0 || 
			SYS.cntrReq(SYS.XMLNode("load").setAttr("path",nAddr+"/%2fobj")) != 0 || node.cfg("DESCRIPT") != "Test") testREZ = "FAILED: load input";
		else {
			node = false;
			if(SYS.cntrReq(SYS.XMLNode("del").setAttr("path","/Transport/"+nList[i_n]+"/%2fbr%2fin_").setAttr("id",nNm)) != 0 || SYS.nodeAt(nAddr)) testREZ = "FAILED: input delete";
			else {
				cfgTree.getElementBy("Transport_in").childAdd("fld").setAttr("ID",nNm+"_cfg").setAttr("MODULE",nList[i_n].slice(4));
				cfgTree.save(0x20, configPath);
				SYS.cntrReq(SYS.XMLNode("scan").setAttr("path","/%2fgen%2fconfig"));		//Reload config command
				if(!SYS.nodeAt(nAddr+"_cfg")) testREZ = "FAILED: load input from config";
			}
		}
		//Output transport
		nAddr = "/Transport/"+nList[i_n]+"/out_"+nNm;
		if(testREZ == "PASSED" && (SYS.cntrReq(SYS.XMLNode("add").setAttr("path","/Transport/"+nList[i_n]+"/%2fbr%2fout_").setAttr("id",nNm)) != 0 || !(node=SYS.nodeAt(nAddr)))) testREZ = "FAILED: output add";
		else if(SYS.cntrReq(SYS.XMLNode("set").setAttr("path",nAddr+"/%2fprm%2fcfg%2fdscr").setText("Test")) != 0 || node.cfg("DESCRIPT") != "Test") testREZ = "FAILED: change output";
		else if(SYS.cntrReq(SYS.XMLNode("save").setAttr("path",nAddr+"/%2fobj")) != 0) testREZ = "FAILED: save output";
		else if(SYS.cntrReq(SYS.XMLNode("set").setAttr("path",nAddr+"/%2fprm%2fcfg%2fdscr").setText("Test 1")) != 0 || 
			SYS.cntrReq(SYS.XMLNode("load").setAttr("path",nAddr+"/%2fobj")) != 0 || node.cfg("DESCRIPT") != "Test") testREZ = "FAILED: load output";
		else {
			node = false;
			if(SYS.cntrReq(SYS.XMLNode("del").setAttr("path","/Transport/"+nList[i_n]+"/%2fbr%2fout_").setAttr("id",nNm)) != 0 || SYS.nodeAt(nAddr)) testREZ = "FAILED: output delete";
			else {
				cfgTree.getElementBy("Transport_out").childAdd("fld").setAttr("ID",nNm+"_cfg").setAttr("MODULE",nList[i_n].slice(4));
				cfgTree.save(0x20, configPath);
				SYS.cntrReq(SYS.XMLNode("scan").setAttr("path","/%2fgen%2fconfig"));		//Reload config command
				if(!SYS.nodeAt(nAddr+"_cfg")) testREZ = "FAILED: load output from config";
			}
		}
		rez += "<tr><td>Transport."+nList[i_n].slice(4)+": Input/Output create/remove, change, load, save and load from config</td><td>"+testREZ+"</td></tr>\n";

		//Disconnect/Connect module
		req = SYS.XMLNode("get").setAttr("path","/Transport/"+nList[i_n]+"/%2fhelp%2fm_inf%2fSource");
		SYS.cntrReq(req);
		testREZ = SYS.Special.SystemTests.SOAttach(req.text(),0);
		testREZ += "/"+SYS.Special.SystemTests.SOAttach(req.text(),0);
		rez += "<tr><td>Transport."+nList[i_n].slice(4)+": Disconnect/Connect</td><td>"+testREZ+"</td></tr>\n";
	}
}

//**********************************
//Archive subsystem tests
if(sub < 0 || sub == 3) {
	testREZ = SYS.Special.SystemTests.ValBuf();
	rez += "<tr><td>Archive: Tests of the values'' buffer.</td><td>"+testREZ+"</td></tr>\n";

	// Value archive
	testREZ = "PASSED";
	nAddr = "/Archive/va_"+nNm;
	if(SYS.cntrReq(SYS.XMLNode("add").setAttr("path","/Archive/%2fbr%2fva_").setAttr("id",nNm)) != 0 || !(node=SYS.nodeAt(nAddr))) testREZ = "FAILED: add";
	else if(SYS.cntrReq(SYS.XMLNode("set").setAttr("path",nAddr+"/%2fprm%2fcfg%2fdscr").setText("Test")) != 0 || node.cfg("DESCR") != "Test") testREZ = "FAILED: change";
	else if(SYS.cntrReq(SYS.XMLNode("save").setAttr("path",nAddr+"/%2fobj")) != 0) testREZ = "FAILED: save";
	else if(SYS.cntrReq(SYS.XMLNode("set").setAttr("path",nAddr+"/%2fprm%2fcfg%2fdscr").setText("Test 1")) != 0 || 
		SYS.cntrReq(SYS.XMLNode("load").setAttr("path",nAddr+"/%2fobj")) != 0 || node.cfg("DESCR") != "Test") testREZ = "FAILED: load";
	else {
		node = false;
		if(SYS.cntrReq(SYS.XMLNode("del").setAttr("path","/Archive/%2fbr%2fva_").setAttr("id",nNm)) != 0 || SYS.nodeAt(nAddr)) testREZ = "FAILED: delete";
		else {
			cfgTree.getElementBy("sub_Archive").childAdd("tbl").setAttr("id","Archive_val").childAdd("fld").setAttr("ID",nNm+"_cfg");
			cfgTree.save(0x20, configPath);
			SYS.cntrReq(SYS.XMLNode("scan").setAttr("path","/%2fgen%2fconfig"));		//Reload config command
			if(!SYS.nodeAt(nAddr+"_cfg")) testREZ = "FAILED: load from config";
		}
	}
	rez += "<tr><td>Archive: Value archive create/remove, change, load, save and load from config</td><td>"+testREZ+"</td></tr>\n";

	// Archivators
	nList = SYS.Archive.nodeList("mod_");
	cfgTree.getElementBy("sub_Archive").childAdd("tbl").setAttr("id","Archive_mess_proc");
	cfgTree.getElementBy("sub_Archive").childAdd("tbl").setAttr("id","Archive_val_proc");
	for(i_n = 0; i_n < nList.length; i_n++) {
		testREZ = "PASSED";
		//Messages archivator
		nAddr = "/Archive/"+nList[i_n]+"/mess_"+nNm;
		if(SYS.cntrReq(SYS.XMLNode("add").setAttr("path","/Archive/"+nList[i_n]+"/%2fbr%2fmess_").setAttr("id",nNm)) != 0 || !(node=SYS.nodeAt(nAddr))) testREZ = "FAILED: messages add";
		else if(SYS.cntrReq(SYS.XMLNode("set").setAttr("path",nAddr+"/%2fprm%2fcfg%2fdscr").setText("Test")) != 0 || node.cfg("DESCR") != "Test") testREZ = "FAILED: change messages";
		else if(SYS.cntrReq(SYS.XMLNode("save").setAttr("path",nAddr+"/%2fobj")) != 0) testREZ = "FAILED: save messages";
		else if(SYS.cntrReq(SYS.XMLNode("set").setAttr("path",nAddr+"/%2fprm%2fcfg%2fdscr").setText("Test 1")) != 0 || 
			SYS.cntrReq(SYS.XMLNode("load").setAttr("path",nAddr+"/%2fobj")) != 0 || node.cfg("DESCR") != "Test") testREZ = "FAILED: load messages";
		else {
			node = false;
			if(SYS.cntrReq(SYS.XMLNode("del").setAttr("path","/Archive/"+nList[i_n]+"/%2fbr%2fmess_").setAttr("id",nNm)) != 0 || SYS.nodeAt(nAddr)) testREZ = "FAILED: messages delete";
			else {
				cfgTree.getElementBy("Archive_mess_proc").childAdd("fld").setAttr("ID",nNm+"_cfg").setAttr("MODUL",nList[i_n].slice(4));
				cfgTree.save(0x20, configPath);
				SYS.cntrReq(SYS.XMLNode("scan").setAttr("path","/%2fgen%2fconfig"));		//Reload config command
				if(!SYS.nodeAt(nAddr+"_cfg")) testREZ = "FAILED: load messages from config";
			}
		}
		//Value archivator
		nAddr = "/Archive/"+nList[i_n]+"/val_"+nNm;
		if(testREZ == "PASSED" && (SYS.cntrReq(SYS.XMLNode("add").setAttr("path","/Archive/"+nList[i_n]+"/%2fbr%2fval_").setAttr("id",nNm)) != 0 || !(node=SYS.nodeAt(nAddr)))) testREZ = "FAILED: values add";
		else if(SYS.cntrReq(SYS.XMLNode("set").setAttr("path",nAddr+"/%2fprm%2fcfg%2fdscr").setText("Test")) != 0 || node.cfg("DESCR") != "Test") testREZ = "FAILED: change values";
		else if(SYS.cntrReq(SYS.XMLNode("save").setAttr("path",nAddr+"/%2fobj")) != 0) testREZ = "FAILED: save values";
		else if(SYS.cntrReq(SYS.XMLNode("set").setAttr("path",nAddr+"/%2fprm%2fcfg%2fdscr").setText("Test 1")) != 0 || 
			SYS.cntrReq(SYS.XMLNode("load").setAttr("path",nAddr+"/%2fobj")) != 0 || node.cfg("DESCR") != "Test") testREZ = "FAILED: load values";
		else {
			node = false;
			if(SYS.cntrReq(SYS.XMLNode("del").setAttr("path","/Archive/"+nList[i_n]+"/%2fbr%2fval_").setAttr("id",nNm)) != 0 || SYS.nodeAt(nAddr)) testREZ = "FAILED: values delete";
			else {
				cfgTree.getElementBy("Archive_val_proc").childAdd("fld").setAttr("ID",nNm+"_cfg").setAttr("MODUL",nList[i_n].slice(4));
				cfgTree.save(0x20, configPath);
				SYS.cntrReq(SYS.XMLNode("scan").setAttr("path","/%2fgen%2fconfig"));		//Reload config command
				if(!SYS.nodeAt(nAddr+"_cfg")) testREZ = "FAILED: load values from config";
			}
		}
		rez += "<tr><td>Archive."+nList[i_n].slice(4)+": Messages/Values create/remove, change, load, save and load from config</td><td>"+testREZ+"</td></tr>\n";

		//Disconnect/Connect module
		req = SYS.XMLNode("get").setAttr("path","/Archive/"+nList[i_n]+"/%2fhelp%2fm_inf%2fSource");
		SYS.cntrReq(req);
		testREZ = SYS.Special.SystemTests.SOAttach(req.text(),0);
		testREZ += "/"+SYS.Special.SystemTests.SOAttach(req.text(),0);
		rez += "<tr><td>Archive."+nList[i_n].slice(4)+": Disconnect/Connect</td><td>"+testREZ+"</td></tr>\n";
	}
}

//**********************************
//DB subsystem tests
if(sub < 0 || sub == 4) {
	nList = SYS.BD.nodeList("mod_");
	for(i_n = 0; i_n < nList.length; i_n++) {
		testREZ = "PASSED";
		//Messages archivator
		nAddr = "/BD/"+nList[i_n]+"/db_"+nNm;
		if(SYS.cntrReq(SYS.XMLNode("add").setAttr("path","/BD/"+nList[i_n]+"/%2fbr%2fdb_").setAttr("id",nNm)) != 0 || !(node=SYS.nodeAt(nAddr))) testREZ = "FAILED: add";
		else if(SYS.cntrReq(SYS.XMLNode("set").setAttr("path",nAddr+"/%2fprm%2fcfg%2fdscr").setText("Test")) != 0 || node.cfg("DESCR") != "Test") testREZ = "FAILED: change";
		else if(SYS.cntrReq(SYS.XMLNode("save").setAttr("path",nAddr+"/%2fobj")) != 0) testREZ = "FAILED: save";
		else if(SYS.cntrReq(SYS.XMLNode("set").setAttr("path",nAddr+"/%2fprm%2fcfg%2fdscr").setText("Test 1")) != 0 || 
			SYS.cntrReq(SYS.XMLNode("load").setAttr("path",nAddr+"/%2fobj")) != 0 || node.cfg("DESCR") != "Test") testREZ = "FAILED: load";
		else {
			node = false;
			if(SYS.cntrReq(SYS.XMLNode("del").setAttr("path","/BD/"+nList[i_n]+"/%2fbr%2fdb_").setAttr("id",nNm)) != 0 || SYS.nodeAt(nAddr)) testREZ = "FAILED: delete";
			else {
				cfgTree.getElementBy("DB").childAdd("fld").setAttr("ID",nNm+"_cfg").setAttr("TYPE",nList[i_n].slice(4)).setAttr("EN",0);
				cfgTree.save(0x20, configPath);
				SYS.cntrReq(SYS.XMLNode("scan").setAttr("path","/%2fgen%2fconfig"));		//Reload config command
				if(!SYS.nodeAt(nAddr+"_cfg")) testREZ = "FAILED: load from config";
			}
		}
		rez += "<tr><td>DB."+nList[i_n].slice(4)+": create/remove, change, load, save and load from config</td><td>"+testREZ+"</td></tr>\n";

		//Disconnect/Connect module
		req = SYS.XMLNode("get").setAttr("path","/BD/"+nList[i_n]+"/%2fhelp%2fm_inf%2fSource");
		SYS.cntrReq(req);
		testREZ = SYS.Special.SystemTests.SOAttach(req.text(),0);
		testREZ += "/"+SYS.Special.SystemTests.SOAttach(req.text(),0);
		rez += "<tr><td>DB."+nList[i_n].slice(4)+": Disconnect/Connect</td><td>"+testREZ+"</td></tr>\n";
	}
}

//**********************************
//Protocol subsystem tests
if(sub < 0 || sub == 5) {
	nList = SYS.Protocol.nodeList("mod_");
	for(i_n = 0; i_n < nList.length; i_n++) {
		//Disconnect/Connect module
		req = SYS.XMLNode("get").setAttr("path","/Protocol/"+nList[i_n]+"/%2fhelp%2fm_inf%2fSource");
		SYS.cntrReq(req);
		testREZ = SYS.Special.SystemTests.SOAttach(req.text(),0);
		testREZ += "/"+SYS.Special.SystemTests.SOAttach(req.text(),0);
		rez += "<tr><td>Protocol."+nList[i_n].slice(4)+": Disconnect/Connect</td><td>"+testREZ+"</td></tr>\n";
	}
}

//**********************************
//DAQ subsystem tests
if(sub < 0 || sub == 6) {
	// Templates
	testREZ = "PASSED";
	nAddr = "/DAQ/tmplb_"+nNm;
	if(SYS.cntrReq(SYS.XMLNode("add").setAttr("path","/DAQ/%2fbr%2ftmplb_").setAttr("id",nNm)) != 0 || !(node=SYS.nodeAt(nAddr))) testREZ = "FAILED: template library add";
	else if(SYS.cntrReq(SYS.XMLNode("set").setAttr("path",nAddr+"/%2flib%2fcfg%2fdescr").setText("Test")) != 0 || node.cfg("DESCR") != "Test") testREZ = "FAILED: change template library";
	else if(SYS.cntrReq(SYS.XMLNode("save").setAttr("path",nAddr+"/%2fobj")) != 0) testREZ = "FAILED: save template library";
	else if(SYS.cntrReq(SYS.XMLNode("set").setAttr("path",nAddr+"/%2flib%2fcfg%2fdescr").setText("Test 1")) != 0 || 
		SYS.cntrReq(SYS.XMLNode("load").setAttr("path",nAddr+"/%2fobj")) != 0 || node.cfg("DESCR") != "Test") testREZ = "FAILED: load template library";
	else {
		node = false;

		// Template
		nAddrLb = nAddr+"/tmpl_"+nNm;
		if(SYS.cntrReq(SYS.XMLNode("add").setAttr("path",nAddr+"/%2fbr%2ftmpl_").setAttr("id",nNm)) != 0 || !(node=SYS.nodeAt(nAddrLb))) testREZ = "FAILED: template add";
		else if(SYS.cntrReq(SYS.XMLNode("set").setAttr("path",nAddrLb+"/%2ftmpl%2fcfg%2fdescr").setText("Test")) != 0 || node.cfg("DESCR") != "Test") testREZ = "FAILED: change template";
		else if(SYS.cntrReq(SYS.XMLNode("save").setAttr("path",nAddrLb+"/%2fobj")) != 0) testREZ = "FAILED: save template";
		else if(SYS.cntrReq(SYS.XMLNode("set").setAttr("path",nAddrLb+"/%2ftmpl%2fcfg%2fdescr").setText("Test 1")) != 0 || 
			SYS.cntrReq(SYS.XMLNode("load").setAttr("path",nAddrLb+"/%2fobj")) != 0 || node.cfg("DESCR") != "Test") testREZ = "FAILED: load template";
		node = false;

		if(SYS.cntrReq(SYS.XMLNode("del").setAttr("path","/DAQ/%2fbr%2ftmplb_").setAttr("id",nNm)) != 0 || SYS.nodeAt(nAddr)) testREZ = "FAILED: template library delete";
		else {
			cfgTree.getElementBy("sub_DAQ").childAdd("tbl").setAttr("id","tmplib").childAdd("fld").setAttr("ID",nNm+"_cfg").setAttr("DB","tmplib_"+nNm+"_cfg");
			cfgTree.getElementBy("sub_DAQ").childAdd("tbl").setAttr("id","tmplib_"+nNm+"_cfg").childAdd("fld").setAttr("ID",nNm+"_cfg");
			cfgTree.save(0x20, configPath);
			SYS.cntrReq(SYS.XMLNode("scan").setAttr("path","/%2fgen%2fconfig"));		//Reload config command
			if(!SYS.nodeAt(nAddr+"_cfg") || !SYS.nodeAt(nAddr+"_cfg/tmpl_"+nNm+"_cfg")) testREZ = "FAILED: load template library from config";
		}
	}
	rez += "<tr><td>DAQ: Templates library create/remove, change, load, save and load from config</td><td>"+testREZ+"</td></tr>\n";

	// Modules
	nList = SYS.DAQ.nodeList("mod_");
	for(i_n = 0; i_n < nList.length; i_n++) {
		testREZ = "PASSED";
		testREZ_ = "PASSED";
		//> Controller
		nAddr = "/DAQ/"+nList[i_n]+"/cntr_"+nNm;
		if(SYS.cntrReq(SYS.XMLNode("add").setAttr("path","/DAQ/"+nList[i_n]+"/%2fbr%2fcntr_").setAttr("id",nNm)) != 0 || !(node=SYS.nodeAt(nAddr))) testREZ = "FAILED: controller add";
		else if(SYS.cntrReq(SYS.XMLNode("set").setAttr("path",nAddr+"/%2fcntr%2fcfg%2fDESCR").setText("Test")) != 0 || node.cfg("DESCR") != "Test") testREZ = "FAILED: controller change";
		else if(SYS.cntrReq(SYS.XMLNode("save").setAttr("path",nAddr+"/%2fobj")) != 0) testREZ = "FAILED: controller save";
		else if(SYS.cntrReq(SYS.XMLNode("set").setAttr("path",nAddr+"/%2fcntr%2fcfg%2fDESCR").setText("Test 1")) != 0 || 
			SYS.cntrReq(SYS.XMLNode("load").setAttr("path",nAddr+"/%2fobj")) != 0 || node.cfg("DESCR") != "Test") testREZ = "FAILED: controller load";
		else {
			node = false;

			// Parameter
			if(nList[i_n] != "mod_DAQGate") {
				nAddr1 = nAddr+"/prm_"+nNm;
				if(SYS.cntrReq(SYS.XMLNode("add").setAttr("path",nAddr+"/%2fbr%2fprm_").setAttr("id",nNm)) != 0 || !(node=SYS.nodeAt(nAddr1))) testREZ = "FAILED: parameter add";
				else if(SYS.cntrReq(SYS.XMLNode("set").setAttr("path",nAddr1+"/%2fprm%2fcfg%2fDESCR").setText("Test")) != 0 || node.cfg("DESCR") != "Test") testREZ = "FAILED: parameter change";
				else if(SYS.cntrReq(SYS.XMLNode("save").setAttr("path",nAddr1+"/%2fobj")) != 0) testREZ = "FAILED: parameter save";
				else if(SYS.cntrReq(SYS.XMLNode("set").setAttr("path",nAddr1+"/%2fprm%2fcfg%2fDESCR").setText("Test 1")) != 0 || 
					SYS.cntrReq(SYS.XMLNode("load").setAttr("path",nAddr1+"/%2fobj")) != 0 || node.cfg("DESCR") != "Test") testREZ = "FAILED: parameter load";
				node = false;
			}

			if(nList[i_n] == "mod_BlockCalc") {
				nAddr1 = nAddr+"/blk_"+nNm;
				if(SYS.cntrReq(SYS.XMLNode("add").setAttr("path",nAddr+"/%2fbr%2fblk_").setAttr("id",nNm)) != 0 || !(node=SYS.nodeAt(nAddr1))) testREZ_ = "FAILED: block add";
				else if(SYS.cntrReq(SYS.XMLNode("set").setAttr("path",nAddr1+"/%2fblck%2fcfg%2fdescr").setText("Test")) != 0 || node.cfg("DESCR") != "Test") testREZ_ = "FAILED: block change";
				else if(SYS.cntrReq(SYS.XMLNode("save").setAttr("path",nAddr1+"/%2fobj")) != 0) testREZ_ = "FAILED: block save";
				else if(SYS.cntrReq(SYS.XMLNode("set").setAttr("path",nAddr1+"/%2fblck%2fcfg%2fdescr").setText("Test 1")) != 0 || 
					SYS.cntrReq(SYS.XMLNode("load").setAttr("path",nAddr1+"/%2fobj")) != 0 || node.cfg("DESCR") != "Test") testREZ_ = "FAILED: block load";
				node = false;
			}
			if(nList[i_n] == "mod_JavaLikeCalc") {
				nAddr_ = "/DAQ/"+nList[i_n]+"/lib_"+nNm;
				if(SYS.cntrReq(SYS.XMLNode("add").setAttr("path","/DAQ/"+nList[i_n]+"/%2fbr%2flib_").setAttr("id",nNm)) != 0 || !(node=SYS.nodeAt(nAddr_))) testREZ_ = "FAILED: library add";
				else if(SYS.cntrReq(SYS.XMLNode("set").setAttr("path",nAddr_+"/%2flib%2fcfg%2fdescr").setText("Test")) != 0 || node.cfg("DESCR") != "Test") testREZ_ = "FAILED: library change";
				else if(SYS.cntrReq(SYS.XMLNode("save").setAttr("path",nAddr_+"/%2fobj")) != 0) testREZ_ = "FAILED: library save";
				else if(SYS.cntrReq(SYS.XMLNode("set").setAttr("path",nAddr_+"/%2flib%2fcfg%2fdescr").setText("Test 1")) != 0 || 
					SYS.cntrReq(SYS.XMLNode("load").setAttr("path",nAddr_+"/%2fobj")) != 0 || node.cfg("DESCR") != "Test") testREZ_ = "FAILED: library load";
				else {
					node = false;

					// Function
					nAddr_1 = nAddr_+"/fnc_"+nNm;
					if(SYS.cntrReq(SYS.XMLNode("add").setAttr("path",nAddr_+"/%2fbr%2ffnc_").setAttr("id",nNm)) != 0 || !(node=SYS.nodeAt(nAddr_1))) testREZ_ = "FAILED: function add";
					else if(SYS.cntrReq(SYS.XMLNode("set").setAttr("path",nAddr_1+"/%2ffunc%2fcfg%2fdescr").setText("Test")) != 0 || node.cfg("DESCR") != "Test") testREZ_ = "FAILED: function change";
					else if(SYS.cntrReq(SYS.XMLNode("save").setAttr("path",nAddr_1+"/%2fobj")) != 0) testREZ_ = "FAILED: function save";
					else if(SYS.cntrReq(SYS.XMLNode("set").setAttr("path",nAddr_1+"/%2ffunc%2fcfg%2fdescr").setText("Test 1")) != 0 || 
						SYS.cntrReq(SYS.XMLNode("load").setAttr("path",nAddr_1+"/%2fobj")) != 0 || node.cfg("DESCR") != "Test") testREZ_ = "FAILED: function load";
					node = false;
					if(SYS.cntrReq(SYS.XMLNode("del").setAttr("path","/DAQ/"+nList[i_n]+"/%2fbr%2flib_").setAttr("id",nNm)) != 0 || SYS.nodeAt(nAddr_)) testREZ_ = "FAILED: function delete";
				}			
			}

			if(SYS.cntrReq(SYS.XMLNode("del").setAttr("path","/DAQ/"+nList[i_n]+"/%2fbr%2fcntr_").setAttr("id",nNm)) != 0 || SYS.nodeAt(nAddr)) testREZ = "FAILED: controller delete";
			else {
				treeNd = cfgTree.getElementBy("sub_DAQ");
				if(!treeNd.getElementBy(nList[i_n])) treeNd.childAdd("node").setAttr("id",nList[i_n]);
				treeNd = treeNd.getElementBy(nList[i_n]);

				treeNd_t = treeNd.childAdd("tbl").setAttr("id","DAQ").childAdd("fld").setAttr("ID",nNm+"_cfg").setAttr("PRM_BD",nNm+"_cfg_prm").setAttr("ENABLE",1);
				if(nList[i_n] == "mod_JavaLikeCalc") treeNd_t.setAttr("FUNC","servProc.fnc_crc16");
				treeNd.childAdd("tbl").setAttr("id",nNm+"_cfg_prm").childAdd("fld").setAttr("SHIFR",nNm+"_cfg");
				if(nList[i_n] == "mod_BlockCalc") {
					treeNd_t.setAttr("BLOCK_SH",nNm+"_cfg_blcks");
					treeNd.childAdd("tbl").setAttr("id",nNm+"_cfg_blcks").childAdd("fld").setAttr("ID",nNm+"_cfg");
				}
				if(nList[i_n] == "mod_JavaLikeCalc") {
					treeNd.childAdd("tbl").setAttr("id","lib").childAdd("fld").setAttr("ID",nNm+"_cfg").setAttr("DB","lib_"+nNm+"_cfg");
					treeNd.childAdd("tbl").setAttr("id","lib_"+nNm+"_cfg").childAdd("fld").setAttr("ID",nNm+"_cfg");
				}

				cfgTree.save(0x20, configPath);
				SYS.cntrReq(SYS.XMLNode("scan").setAttr("path","/%2fgen%2fconfig"));		//Reload config command
				SYS.nodeAt(nAddr+"_cfg").enable(true);
				if(!SYS.nodeAt(nAddr+"_cfg")) testREZ = "FAILED: load controller from config";
				else {
					if(!SYS.nodeAt(nAddr+"_cfg/prm_"+nNm+"_cfg")) testREZ = "FAILED: load parameter from config";
					if(nList[i_n] == "mod_BlockCalc" && !SYS.nodeAt(nAddr+"_cfg/blk_"+nNm+"_cfg")) testREZ_ = "FAILED: load block from config";
					if(nList[i_n] == "mod_JavaLikeCalc") {
						if(!SYS.nodeAt(nAddr_+"_cfg")) testREZ_ = "FAILED: load library from config";
						else if(!SYS.nodeAt(nAddr_+"_cfg/fnc_"+nNm+"_cfg")) testREZ_ = "FAILED: load function from config";
					}
				}
			}
		}
		rez += "<tr><td>DAQ."+nList[i_n].slice(4)+": create/remove, change, load, save and load from config</td><td>"+testREZ+"</td></tr>\n";
		if(nList[i_n] == "mod_BlockCalc")
			rez += "<tr><td>DAQ."+nList[i_n].slice(4)+": Blocks create/remove, change, load, save and load from config</td><td>"+testREZ_+"</td></tr>\n";
		else if(nList[i_n] == "mod_JavaLikeCalc")
			rez += "<tr><td>DAQ."+nList[i_n].slice(4)+": Functions and its libraries create/remove, change, load, save and load from config</td><td>"+testREZ_+"</td></tr>\n";

		//Disconnect/Connect module
		if(nList[i_n] != "mod_JavaLikeCalc") {
			req = SYS.XMLNode("get").setAttr("path","/DAQ/"+nList[i_n]+"/%2fhelp%2fm_inf%2fSource");
			SYS.cntrReq(req);
			testREZ = SYS.Special.SystemTests.SOAttach(req.text(),0);
			testREZ += "/"+SYS.Special.SystemTests.SOAttach(req.text(),0);
			rez += "<tr><td>DAQ."+nList[i_n].slice(4)+": Disconnect/Connect</td><td>"+testREZ+"</td></tr>\n";
		}
	}
}

//**********************************
//UI subsystem tests
if(sub < 0 || sub == 7) {
	nList = SYS.UI.nodeList("mod_");
	for(i_n = 0; i_n < nList.length; i_n++) {
		if(nList[i_n] == "mod_QTStarter" || nList[i_n] == "mod_QTCfg") continue;

		//UI.VCAEngine tests
		if(nList[i_n] == "mod_VCAEngine") {
			// Widgets library
			testREZ = "PASSED";
			nAddr = "/UI/"+nList[i_n]+"/wlb_"+nNm;
			if(SYS.cntrReq(SYS.XMLNode("add").setAttr("path","/UI/"+nList[i_n]+"/%2fbr%2fwlb_").setAttr("id",nNm)) || !(node=SYS.nodeAt(nAddr))) testREZ = "FAILED: widgets library add";
			else if(SYS.cntrReq(SYS.XMLNode("set").setAttr("path",nAddr+"/%2fobj%2fcfg%2fdescr").setText("Test")) || 
				SYS.cntrReq(req=SYS.XMLNode("get").setAttr("path",nAddr+"/%2fobj%2fcfg%2fdescr")) || req.text() != "Test") testREZ = "FAILED: change widgets library";
			else if(SYS.cntrReq(SYS.XMLNode("save").setAttr("path",nAddr+"/%2fobj"))) testREZ = "FAILED: save widgets library";
			else if(SYS.cntrReq(SYS.XMLNode("set").setAttr("path",nAddr+"/%2fobj%2fcfg%2fdescr").setText("Test 1")) || 
				SYS.cntrReq(SYS.XMLNode("load").setAttr("path",nAddr+"/%2fobj")) || SYS.cntrReq(req=SYS.XMLNode("get").setAttr("path",nAddr+"/%2fobj%2fcfg%2fdescr")) ||
				req.text() != "Test") testREZ = "FAILED: load widgets library";
			else {
				node = false;

				// Widget
				nAddr1 = nAddr+"/wdg_"+nNm;
				if(SYS.cntrReq(SYS.XMLNode("add").setAttr("path",nAddr+"/%2fbr%2fwdg_").setAttr("id",nNm)) || !(node=SYS.nodeAt(nAddr1))) testREZ = "FAILED: widget add";
				else if(SYS.cntrReq(SYS.XMLNode("set").setAttr("path",nAddr1+"/%2fwdg%2fcfg%2fdescr").setText("Test")) || 
					SYS.cntrReq(req=SYS.XMLNode("get").setAttr("path",nAddr1+"/%2fwdg%2fcfg%2fdescr")) || req.text() != "Test") testREZ = "FAILED: change widget";
				else if(SYS.cntrReq(SYS.XMLNode("save").setAttr("path",nAddr1+"/%2fobj"))) testREZ = "FAILED: save widget";
				else if(SYS.cntrReq(SYS.XMLNode("set").setAttr("path",nAddr1+"/%2fwdg%2fcfg%2fdescr").setText("Test 1")) || 
					SYS.cntrReq(SYS.XMLNode("load").setAttr("path",nAddr1+"/%2fobj")) || SYS.cntrReq(req=SYS.XMLNode("get").setAttr("path",nAddr1+"/%2fwdg%2fcfg%2fdescr")) ||
						req.text() != "Test") testREZ = "FAILED: load widget";
				node = false;

				if(SYS.cntrReq(SYS.XMLNode("del").setAttr("path","/UI/"+nList[i_n]+"/%2fbr%2fwlb_").setAttr("id",nNm)) != 0 || SYS.nodeAt(nAddr)) testREZ = "FAILED: widgets library delete";
				else {
					cfgTree.getElementBy("mod_VCAEngine").childAdd("tbl").setAttr("id","LIB").childAdd("fld").setAttr("ID",nNm+"_cfg").setAttr("DB_TBL","wlib_"+nNm+"_cfg");
					cfgTree.getElementBy("mod_VCAEngine").childAdd("tbl").setAttr("id","wlib_"+nNm+"_cfg").childAdd("fld").setAttr("ID",nNm+"_cfg").setAttr("PARENT","/wlb_originals/wdg_Box");
					cfgTree.save(0x20, configPath);
					SYS.cntrReq(SYS.XMLNode("scan").setAttr("path","/%2fgen%2fconfig"));		//Reload config command
					if(!SYS.nodeAt(nAddr+"_cfg") || !SYS.nodeAt(nAddr+"_cfg/wdg_"+nNm+"_cfg")) testREZ = "FAILED: load widgets library from config";
				}
			}
			rez += "<tr><td>UI.VCAEngine: Widgets and widgets'' library create/remove, change, load, save and load from config</td><td>"+testREZ+"</td></tr>\n";

			// Project
			testREZ = "PASSED";
			nAddr = "/UI/"+nList[i_n]+"/prj_"+nNm;
			if(SYS.cntrReq(SYS.XMLNode("add").setAttr("path","/UI/"+nList[i_n]+"/%2fbr%2fprj_").setAttr("id",nNm)) || !(node=SYS.nodeAt(nAddr))) testREZ = "FAILED: project add";
			else if(SYS.cntrReq(SYS.XMLNode("set").setAttr("path",nAddr+"/%2fobj%2fcfg%2fdescr").setText("Test"))  || 
				SYS.cntrReq(req=SYS.XMLNode("get").setAttr("path",nAddr+"/%2fobj%2fcfg%2fdescr")) || req.text() != "Test") testREZ = "FAILED: change project";
			else if(SYS.cntrReq(SYS.XMLNode("save").setAttr("path",nAddr+"/%2fobj")) != 0) testREZ = "FAILED: save project";
			else if(SYS.cntrReq(SYS.XMLNode("set").setAttr("path",nAddr+"/%2fobj%2fcfg%2fdescr").setText("Test 1")) != 0 || 
				SYS.cntrReq(SYS.XMLNode("load").setAttr("path",nAddr+"/%2fobj")) != 0 || SYS.cntrReq(req=SYS.XMLNode("get").setAttr("path",nAddr+"/%2fobj%2fcfg%2fdescr")) ||
				req.text() != "Test") testREZ = "FAILED: load project";
			else {
				node = false;

				// Page
				nAddr1 = nAddr+"/pg_"+nNm;
				if(SYS.cntrReq(SYS.XMLNode("add").setAttr("path",nAddr+"/%2fbr%2fpg_").setAttr("id",nNm)) != 0 || !(node=SYS.nodeAt(nAddr1))) testREZ = "FAILED: page add";
				else if(SYS.cntrReq(SYS.XMLNode("set").setAttr("path",nAddr1+"/%2fwdg%2fst%2fparent").setText("/wlb_originals/wdg_Box")) != 0) testREZ = "FAILED: change page";
				else if(SYS.cntrReq(SYS.XMLNode("set").setAttr("path",nAddr1+"/%2fwdg%2fst%2fen").setText(1)) != 0) testREZ = "FAILED: change page";
				else if(SYS.cntrReq(SYS.XMLNode("set").setAttr("path",nAddr1+"/%2fwdg%2fcfg%2fdescr").setText("Test")) != 0 || 
						SYS.cntrReq(req=SYS.XMLNode("get").setAttr("path",nAddr1+"/%2fwdg%2fcfg%2fdescr")) || req.text() != "Test") testREZ = "FAILED: change page";
				else if(SYS.cntrReq(SYS.XMLNode("save").setAttr("path",nAddr1+"/%2fobj")) != 0) testREZ = "FAILED: save page";
				else if(SYS.cntrReq(SYS.XMLNode("set").setAttr("path",nAddr1+"/%2fwdg%2fcfg%2fdescr").setText("Test 1")) != 0 || 
						SYS.cntrReq(SYS.XMLNode("load").setAttr("path",nAddr1+"/%2fobj")) != 0 || SYS.cntrReq(req=SYS.XMLNode("get").setAttr("path",nAddr1+"/%2fwdg%2fcfg%2fdescr")) ||
					req.text() != "Test") testREZ = "FAILED: load page";

				nAddr2 = nAddr1+"/wdg_"+nNm;
				if(SYS.cntrReq(SYS.XMLNode("add").setAttr("path",nAddr1+"/%2fbr%2fwdg_").setAttr("id",nNm)) != 0 || !(node=SYS.nodeAt(nAddr2))) testREZ = "FAILED: widget add";
				else if(SYS.cntrReq(SYS.XMLNode("set").setAttr("path",nAddr2+"/%2fwdg%2fcfg%2fdescr").setText("Test")) != 0 || 
						SYS.cntrReq(req=SYS.XMLNode("get").setAttr("path",nAddr2+"/%2fwdg%2fcfg%2fdescr")) || req.text() != "Test") testREZ = "FAILED: change widget";
				else if(SYS.cntrReq(SYS.XMLNode("save").setAttr("path",nAddr2+"/%2fobj")) != 0) testREZ = "FAILED: save widget";
				else if(SYS.cntrReq(SYS.XMLNode("set").setAttr("path",nAddr2+"/%2fwdg%2fcfg%2fdescr").setText("Test 1")) != 0 || 
						SYS.cntrReq(SYS.XMLNode("load").setAttr("path",nAddr2+"/%2fobj")) != 0 || SYS.cntrReq(req=SYS.XMLNode("get").setAttr("path",nAddr2+"/%2fwdg%2fcfg%2fdescr")) ||
					req.text() != "Test") testREZ = "FAILED: load widget";
				node = false;

				if(SYS.cntrReq(SYS.XMLNode("del").setAttr("path","/UI/"+nList[i_n]+"/%2fbr%2fprj_").setAttr("id",nNm)) != 0 || SYS.nodeAt(nAddr)) testREZ = "FAILED: project delete";
				else {
					cfgTree.getElementBy("mod_VCAEngine").childAdd("tbl").setAttr("id","PRJ").childAdd("fld").setAttr("ID",nNm+"_cfg").setAttr("DB_TBL","prj_"+nNm+"_cfg");
					cfgTree.getElementBy("mod_VCAEngine").childAdd("tbl").setAttr("id","prj_"+nNm+"_cfg").childAdd("fld").setAttr("OWNER","/"+nNm+"_cfg").setAttr("ID",nNm+"_cfg").setAttr("PARENT","/wlb_originals/wdg_Box");//.setAttr("FLGS",1);
					cfgTree.getElementBy("mod_VCAEngine").childAdd("tbl").setAttr("id","prj_"+nNm+"_cfg_incl").childAdd("fld").setAttr("IDW","/prj_"+nNm+"_cfg/pg_"+nNm+"_cfg").setAttr("ID",nNm+"_cfg").setAttr("PARENT","/wlb_originals/wdg_Box");
					cfgTree.save(0x20, configPath);
					SYS.cntrReq(SYS.XMLNode("scan").setAttr("path","/%2fgen%2fconfig"));		//Reload config command
					if(!SYS.nodeAt(nAddr+"_cfg"))	testREZ = "FAILED: load project from config";
					else if(!SYS.nodeAt(nAddr+"_cfg/pg_"+nNm+"_cfg"))	testREZ = "FAILED: load page from config";
					else if(!SYS.nodeAt(nAddr+"_cfg/pg_"+nNm+"_cfg/wdg_"+nNm+"_cfg"))	testREZ = "FAILED: load widget from config";
				}
			}
			rez += "<tr><td>UI.VCAEngine: Project, pages and widgets create/remove, change, load, save and load from config</td><td>"+testREZ+"</td></tr>\n";

			//Session
			testREZ = "PASSED";
			nAddr = "/UI/"+nList[i_n]+"/ses_"+nNm;
			req = SYS.XMLNode("CntrReqs").setAttr("path","/UI/"+nList[i_n]);
			req.childAdd("add").setAttr("path","/%2fbr%2fses_").setText(nNm);
			req.childAdd("set").setAttr("path","/ses_"+nNm+"/%2fobj%2fst%2fprj").setText("tmplSO");
			req.childAdd("set").setAttr("path","/ses_"+nNm+"/%2fobj%2fst%2fstart").setText(1);
			if(SYS.cntrReq(req) != 0 || !SYS.nodeAt(nAddr)) testREZ = "FAILED: session add/start";
			else if(SYS.cntrReq(req=SYS.XMLNode("del").setAttr("path","/UI/"+nList[i_n]+"/%2fbr%2fses_").setText(nNm)) || SYS.nodeAt(nAddr)) testREZ = "FAILED: session delete";
			rez += "<tr><td>UI.VCAEngine: Session create, start and remove</td><td>"+testREZ+"</td></tr>\n";
		}

		//Disconnect/Connect module
		req = SYS.XMLNode("get").setAttr("path","/UI/"+nList[i_n]+"/%2fhelp%2fm_inf%2fSource");
		SYS.cntrReq(req);
		testREZ = SYS.Special.SystemTests.SOAttach(req.text(),0);
		testREZ += "/"+SYS.Special.SystemTests.SOAttach(req.text(),0);
		rez += "<tr><td>UI."+nList[i_n].slice(4)+": Disconnect/Connect</td><td>"+testREZ+"</td></tr>\n";
	}
}

//**********************************
//Special subsystem tests
if(sub < 0 || sub == 8) {
	nList = SYS.Special.nodeList("mod_");
	for(i_n = 0; i_n < nList.length; i_n++) {
		if(nList[i_n] == "mod_SystemTests") continue;
		//Disconnect/Connect module
		req = SYS.XMLNode("get").setAttr("path","/Special/"+nList[i_n]+"/%2fhelp%2fm_inf%2fSource");
		SYS.cntrReq(req);
		testREZ = SYS.Special.SystemTests.SOAttach(req.text(),0);
		testREZ += "/"+SYS.Special.SystemTests.SOAttach(req.text(),0);
		rez += "<tr><td>Special."+nList[i_n].slice(4)+": Disconnect/Connect</td><td>"+testREZ+"</td></tr>\n";
	}
}

//Restore original config file
SYS.fileWrite(configPath,SYS.fileRead(configPath+".orig"));

//Save result to file ReleaseTests.html
SYS.fileWrite("ReleaseTests.html", "<?xml version=''1.0'' ?>\n"
	"<!DOCTYPE html PUBLIC ''-//W3C//DTD XHTML 1.0 Transitional//EN''\n"
	"''DTD/xhtml1-transitional.dtd''>\n"
	"<html xmlns=''http://www.w3.org/1999/xhtml''>\n"
	"<head>\n"
	"  <meta http-equiv=''Content-Type'' content=''text/html; charset=UTF-8''/>\n"
	"</head>\n"
	"<body>\n"
	"<h1>Release tests report table</h1>\n"
	"<TABLE border=\"1\" cellpadding=\"2\" cellspacing=\"0\" width=\"100%\">\n"
	"  <TR align=\"center\"><TH>Test</TH><TH>Result</TH></TR>\n"+
	rez+
	"</TABLE>\n"
	"</body>"
	"</html>");','','',1479062874);
INSERT INTO "lib_servProc" VALUES('archPackFStests','TEST: FS archiver pack','','','Archiving to File System tests pack, for check the optimization algorithm.  The stages supported:
  - Stage 0: Main values fill: "1,1,10,10,100,100,1000,1000,10000,10000,EVAL"
  - Stage 1: Change to different value 20: "1,1,20,10,100,100,1000,1000,10000,10000,EVAL"
  - Stage 2: Set value for merge, equal, to up 10: "1,1,20,10,10,100,1000,1000,10000,10000,EVAL"
  - Stage 3: Set value for merge, equal, to down 1000: "1,1,20,10,10,1000,1000,1000,10000,10000,EVAL"
  - Stage 4: Set different value to end 20000: "1,1,20,10,10,1000,1000,1000,10000,10000,20000,EVAL"
  - Stage 5: Change end value for merge to up 10000: "1,1,20,10,10,1000,1000,1000,10000,10000,10000,EVAL"
  - Stage 6: Set value to end for merge to up 10000: "1,1,20,10,10,1000,1000,1000,10000,10000,10000,10000,EVAL"
  - Stage 7: Set to end value EVAL: "1,1,20,10,10,1000,1000,1000,10000,10000,10000,EVAL"
  - Stage 8: Set equal value to insert 10000: "1,1,20,10,10,1000,1000,1000,10000,10000,10000,EVAL"
Author: Roman Savochenko <rom_as@oscada.org>
Version: 1.0.0','','',1,10,0,'using Special.FLibSYS;
arh = vArh("Archive.va_"+addr);

//Stage 0: Main values fill: "1,1,10,10,100,100,1000,1000,10000,10000,EVAL"
if(step < 0 || step == 0) {
	buf = vArhBuf(1, 10, per*1000000, true, true);
	for(iV = 0; iV < 10; iV++) buf.set(pow(10,floor(iV/2)), tm+iV*per, 0);
	arh.copy(buf, buf.begin(), 0, buf.end(), 0, archiver);
}

//Stage 1: Change to different value 20: "1,1,20,10,100,100,1000,1000,10000,10000,EVAL"
if(step < 0 || step == 1) {
	buf = vArhBuf(1, 10, per*1000000, true, true);
	buf.set(20, tm+2*per, 0); arh.copy(buf, buf.begin(), 0, buf.end(), 0, archiver);
}

//Stage 2: Set value for merge, equal, to up 10: "1,1,20,10,10,100,1000,1000,10000,10000,EVAL"
if(step < 0 || step == 2) {
	buf = vArhBuf(1, 10, per*1000000, true, true);
	buf.set(10, tm+4*per, 0); arh.copy(buf, buf.begin(), 0, buf.end(), 0, archiver);
}

//Stage 3: Set value for merge, equal, to down 1000: "1,1,20,10,10,1000,1000,1000,10000,10000,EVAL"
if(step < 0 || step == 3) {
	buf = vArhBuf(1, 10, per*1000000, true, true);
	buf.set(1000, tm+5*per, 0); arh.copy(buf, buf.begin(), 0, buf.end(), 0, archiver);
}

//Stage 4: Set different value to end 20000: "1,1,20,10,10,1000,1000,1000,10000,10000,20000,EVAL"
if(step < 0 || step == 4) {
	buf = vArhBuf(1, 10, per*1000000, true, true);
	buf.set(20000, tm+10*per, 0); arh.copy(buf, buf.begin(), 0, buf.end(), 0, archiver);
}

//Stage 5: Change end value for merge to up 10000: "1,1,20,10,10,1000,1000,1000,10000,10000,10000,EVAL"
if(step < 0 || step == 5) {
	buf = vArhBuf(1, 10, per*1000000, true, true);
	buf.set(10000, tm+10*per, 0); arh.copy(buf, buf.begin(), 0, buf.end(), 0, archiver);
}

//Stage 6: Set value to end for merge to up 10000: "1,1,20,10,10,1000,1000,1000,10000,10000,10000,10000,EVAL"
if(step < 0 || step == 6) {
	buf = vArhBuf(1, 10, per*1000000, true, true);
	buf.set(10000, tm+11*per, 0); arh.copy(buf, buf.begin(), 0, buf.end(), 0, archiver);
}

//Stage 7: Set to end value EVAL: "1,1,20,10,10,1000,1000,1000,10000,10000,10000,EVAL"
if(step < 0 || step == 7) {
	buf = vArhBuf(1, 10, per*1000000, true, true);
	buf.set(EVAL_INT, tm+11*per, 0); arh.copy(buf, buf.begin(), 0, buf.end(), 0, archiver);
}

//Stage 8: Set equal value to insert 10000: "1,1,20,10,10,1000,1000,1000,10000,10000,10000,EVAL"
if(step < 0 || step == 8) {
	buf = vArhBuf(1, 10, per*1000000, true, true);
	buf.set(10000, tm+9*per, 0); arh.copy(buf, buf.begin(), 0, buf.end(), 0, archiver);
}','','',1479062821);
INSERT INTO "lib_servProc" VALUES('wacko2media','WackoWiki to MediaWiki','','','','','',1,10,0,'dbO = SYS.BD.MySQL[ndb];
if(!dbO)	return "Error: DB ''"+ndb+"'' missed!";
DBTbl = dbO.SQLReq("SELECT body FROM `wk_pages` WHERE `tag`=\""+wpg+"\";");
if(DBTbl.err.length) return "Error: "+DBTbl.err;
else if(DBTbl.length < 2 || !DBTbl[1].length)	return "Error: Page ''"+wpg+"'' is not found!"; 
ibuf = DBTbl[1][0];
//ibuf = "==== 1.1. Вызов консольных команд и утилит операционной системы (sysCall) ====\n"
//	"//Описание:// Осуществляет вызовы консольных команд ОС. Функция открывает широкие возможности пользователю ~OpenSCADA путём вызова любых системных программ, утилит и скриптов, а также получения посредством них доступа к огромному объёму системных данных. Например команда \"ls -l\" вернёт детализированное содержимое рабочей директории.\n"
//	"//Параметры://";
obuf = "";
tblIn = tblInRow = formIt = formBold = formAlert = formSup = formUndrLn = false;
for(pos = 0; pos < ibuf.length; ) {
	//if(ln.search(new RegExp("script","i"))
	stLine = (pos == 0 || ibuf[pos-1] == "\n");
	pos1 = pos;
	ln = ibuf.parseLine(0, pos1);
	twos = ibuf.slice(pos, pos+2);
	// Blocks: %% {Cnt} %%
	if(twos == "%%" && (pos1=ibuf.indexOf("%%",pos+2)) >= 0) {
		cntr = ibuf.slice(pos+2, pos1);
		cntrProps = cntr.match("^\\s*\\([^)]*\\)\\s*\\n*","gm");
		if(cntrProps.length && cntrProps[0].indexOf("wrapper_align=center",0) >= 0)
			obuf += (cntrProps.length?cntr.slice(cntrProps[0].length):cntr);
		else obuf += "<pre style=\"white-space: pre-wrap;\">\n" + (cntrProps.length?cntr.slice(cntrProps[0].length):cntr) + "</pre>";
		pos = pos1+2;
		continue;
	}
	//Templates
	if((mr=ln.match("^{{(TableOfContents|toc)[^}]*}}\\s*$")).length)	{ obuf += "__TOC__"; pos += mr[0].length; continue; }
	// Generic shild simbol ~
	if(ibuf[pos] == "~")	{ obuf += ibuf[pos+1]; pos += 2; }
	// Tables
	if((mr=ln.match("^#\\|\\s*$","gm")).length)	{ tblIn = true; obuf += "{| class=\"wikitable\""; pos += mr[0].length; continue; }
	if(tblIn && twos == "||") {
		if(!tblInRow)	{ obuf += "|-\n|"; tblInRow = true; }
		else tblInRow = false;
		pos += 2; continue;
	}
	if(tblIn && tblInRow && ibuf[pos] == "|")	{ obuf += "||"; pos++; continue; }
	if(tblIn && (mr=ln.match("^\\|#\\s*$","gm")).length)	{ tblIn = tblInRow = false; obuf += "|}"; pos += mr[0].length; continue; }
	// Links processing
	if((twos == "((" && (pos1=ln.indexOf("))",0)) > 0) || (twos == "[[" && (pos1=ln.indexOf("]]",0)) > 0)) {
		pos2 = ln.slice(2,pos1).indexOf(" ", 0);
		//obuf += "["+ln.slice(2,pos1).parse(0," ",pos2)+"|"+ln.slice(2,pos1).slice(pos2)+"]"; 
		if(pos2 >= 0) obuf += "["+((ln[2]=="/")?"http://wiki.oscada.org":"")+ln.slice(2,pos1)+"]";
		else obuf += ((ln[2]=="/")?"http://wiki.oscada.org":"[[") + ln.slice(2,pos1) + ((ln[2]=="/")?"":"|"+ln.slice(2,pos1)+"]]");
		pos += pos1+2; continue;
	}
	if((mr=ln.match("^file:[^\\s\\n]+")).length) {
		obuf += "[[File:"+mr[0].slice(5)+"]]";
		pos += mr[0].length; continue;
	}
	// Generic format
	if(twos == "//" && ibuf.slice(pos-5,pos+2) != "http://") {
		if(!formIt && ln.match("//","g").length > 1) { obuf += "''''"; formIt = true; pos += 2; continue; }
		if(formIt)	{ obuf += "''''"; formIt = false; pos += 2; continue; }
	}
	if(twos == "**") {
		if(!formBold && ln.match("\\*\\*","g").length > 1) { obuf += "''''''"; formBold = true; pos += 2; continue; }
		if(formBold)	{ obuf += "''''''"; formBold = false; pos += 2; continue; }
	}
	if(twos == "__") {
		if(!formUndrLn && ln.match("__","g").length > 1) { obuf += "<u>"; formUndrLn = true; pos += 2; continue; }
		if(formUndrLn)	{ obuf += "</u>"; formUndrLn = false; pos += 2; continue; }
	}
	if(twos == "!!") {
		if(!formAlert && ln.match("!!","g").length > 1) {
			mr = ln.match("^!!\\s*\\(([^\\)]+)\\)");
			obuf += "<span style=\"color: "+(mr.length?mr[1]:"red")+"\">";
			formAlert = true;
			pos += mr.length ? mr[0].length : 2;
			continue;
		}
		if(formAlert)	{ obuf += "</span>"; formAlert = false; pos += 2; continue; }
	}
	if(twos == "^^") {
		if(!formSup && ln.match("\\^\\^","g").length > 1) { obuf += "<sup>"; formSup = true; pos += 2; continue; }
		if(formSup)	{ obuf += "</sup>"; formSup = false; pos += 2; continue; }
	}
	// Lists
	if(stLine && (mr=ln.match("^ {2,}+[\\*-]")).length && ((mr[0].length-1)%2) == 0) {
		for(iS = 0; iS < (mr[0].length-1); iS += 2)	obuf += "*";
		pos += mr[0].length; continue;
	}
	// Indents
	if(stLine && (mr=ln.match("^ {2,}+")).length) {
		for(iS = 0; iS < (mr[0].length); iS += 2)	obuf += ":";
		pos += mr[0].length; continue;
	}
	if(ibuf.slice(pos-1,pos+3).match("^[^\n=%#]\n[^\n=|#{ %]").length)	obuf += "<br/>";
	obuf += ibuf[pos]; pos++;
}

return obuf;','','',1510947218);
INSERT INTO "lib_servProc" VALUES('docOffLine','Off-line documentation','','','','','',1,240,0,'trNm = "offLine";
docHost = "oscada.org:80";
docHost_ = "http://" + docHost.parse(0, ":");
defLang = "en";

//Check and connect to self transport
tr = SYS.Transport.Sockets.nodeAt("out_"+trNm);
if(!tr) {
	//Create self transport if it is missed
	SYS.cntrReq((req=SYS.XMLNode("add").setAttr("path","/sub_Transport/mod_Sockets/%2fbr%2fout_").setAttr("id",trNm)));
	tr = SYS.Transport.Sockets.nodeAt("out_"+trNm);
}
if(!tr)	return;

//Preparation
pCnt = imgCnt = lnkCnt = lngCnt = 0;
dirsO = new Object();
imgsO = new Object();

// Pages list moving to an object for rapid links checking
pgsO = new Object();
for(off = 0; (tVl=pages.parseLine(0,off)).length; ) {
	off1 = 0; pWiki = tVl.parse(0, ":", off1); pLngs = tVl.parse(0, ":", off1); pOffLn = tVl.parse(0, ":", off1);
	for(off1 = 0; (tVl2=pLngs.parse(0,",",off1)).length; ) {
		if(dirsO[tVl2] != true) {
			if(tVl2 != defLang)
				SYS.system("if test ! -d "+resDir+tVl2+"; then install -d "+resDir+tVl2+"/files; cp "+resDir+"en/files/doc.css "+resDir+tVl2+"/files; fi", true);
			dirsO[tVl2] = true; lngCnt++;
		}
		pgsO[pWiki+"/"+tVl2] = pOffLn;
	}
}

if(pagesCur.length) {
	pgsOprc = new Object();
	for(off = 0; (tVl=pagesCur.parseLine(0,off)).length; ) {
		off1 = 0; pWiki = tVl.parse(0, ":", off1); pLngs = tVl.parse(0, ":", off1); pOffLn = tVl.parse(0, ":", off1);
		for(off1 = 0; (tVl2=pLngs.parse(0,",",off1)).length; ) {
			if(dirsO[tVl2] != true) {
				if(tVl2 != defLang)
					SYS.system("if test ! -d "+resDir+tVl2+"; then install -d "+resDir+tVl2+"/files; cp "+resDir+"en/files/doc.css "+resDir+tVl2+"/files; fi", true);
				dirsO[tVl2] = true; lngCnt++;
			}
			pgsOprc[pWiki+"/"+tVl2] = pOffLn;
		}
	}
} else pgsOprc = pgsO;

//Pages fetching and processing
for(var ip in pgsOprc) {
	pLang = ip.lastIndexOf("/"); pWiki = ip.slice(0, pLang); pLang = ip.slice(pLang+1);
	pOffLn = pgsO[ip];

	// Take folders configuration
	dstDir = topDir = "";
	for(off = 0; (tVl=pOffLn.parsePath(0,off)).length && off < pOffLn.length; ) {
		dstDir += tVl+"/";
		topDir += "../";
	}
	// Create intermediate folders
	if(dstDir.length && dirsO[pLang+"/"+dstDir] != true) {
		SYS.system("if test ! -d "+resDir+pLang+"/"+dstDir+"; then install -d "+resDir+pLang+"/"+dstDir+"; fi", true);
		dirsO[pLang+"/"+dstDir] = true;
	}

	// Request the documentation page
	req = SYS.XMLNode("GET").setAttr("Host",docHost).setAttr("URI","/wiki/index.php?title="+ip+"&printable=yes");
	// Load the documentation page to a XML tree
	for(iTr = 0; iTr < reqTr; iTr++)
		if(!(rez=tr.messIO(req,"HTTP")).length && req.text().length) break;
	//if(req.text().length < 10)
	//	SYS.messInfo("OffLine", "TEST 00: rez="+rez+"; length="+req.text().length);
	if(iTr == reqTr)	{ res = "1: Page "+ip+" load error in "+reqTr+" tries"; return; }

	docTree = SYS.XMLNode();
	rez1 = docTree.load(req.text(), false, 0x01);
	if(rez1.toInt()) {
		//SYS.messInfo("OffLine", "TEST 00a: page="+ip+"; rez="+rez+"; length="+req.text().length+"; rez1="+rez1);
		res = "1: Page ''"+ip+"'' parsing for XML is an error: "+rez1+". The original one saved!";
		SYS.fileWrite(resDir+pLang+"/"+pOffLn, req.text());
		return;
	}

	// HTML header processing
	head = docTree.childGet("head");
	for(iEl = 0, passPrev = true; iEl < head.childSize(); iEl++) {
		chO = head.childGet(iEl);
		if(chO.name() == "<*>") {
			if(!passPrev)	{ head.childDel(iEl); iEl--; }
			continue;
		}
		passPrev = false;
		if(chO.name() == "script" || chO.name() == "style" ||
				(chO.name() == "meta" && chO.attr("name") != "generator") ||
				(chO.name() == "link" && chO.attr("rel") != "copyright"))	{
			head.childDel(iEl);
			iEl--;
		}
		else passPrev = true;
	}
	head.childAdd("link").setAttr("rel","stylesheet").setAttr("href",topDir+"files/doc.css");

	// BODY processing
	body = docTree.childGet("body");
	header = body.getElementBy("mw_header");
	cnt = body.getElementBy("mw-content-text");
	body.clear(true);
	body.childAdd(header); body.childAdd(cnt);

	// Images processing
	imgs = body.getElementsBy("img");
	for(iEl = 0; iEl < imgs.length; iEl++)
		if((tVl=imgs[iEl].attr("src").match("^/.*/(.*)$")).length) {
			if(imgsO[pLang+":"+tVl[1]] != true) {
				req = SYS.XMLNode("GET").setAttr("Host",docHost).setAttr("URI",imgs[iEl].attr("src"));
				for(iTr = 0; iTr < reqTr; iTr++)
					if(!(rez=tr.messIO(req,"HTTP")).length && req.text().length) break;
				//if(req.text().length < 10)
				//	SYS.messInfo("OffLine", "TEST 01: rez="+rez+"; length="+req.text().length);
				if(iTr == reqTr)	{ res = "1: Image ''"+req.attr("URI")+"'' load error in "+reqTr+" tries"; return; }
				SYS.fileWrite(resDir+pLang+"/files/"+tVl[1], req.text());
				imgsO[pLang+":"+tVl[1]] = true;
				imgCnt++;
			}
			imgs[iEl].setAttr("src", topDir+"files/"+tVl[1]);
		}

	// Links processing
	anchs = body.getElementsBy("a");
	for(iEl = 0; iEl < anchs.length; iEl++) {
		href = anchs[iEl].attr("href");
		if(href[0] == "/") {
			dynLngPg = (href.indexOf("/wiki/Special:MyLanguage/") == 0) ? href.slice(25) : "";
			stLngPg = (!dynLngPg.length && href.indexOf("/wiki/") == 0) ? href.slice(6) : "";
			if(dynLngPg.length &&
				(!(tDst=pgsO[dynLngPg+"/"+pLang]).isEVal() || !(tDst=pgsO[stLngPg]).isEVal()))
					anchs[iEl].setAttr("href", topDir+tDst);
			else {
				anchs[iEl].setAttr("href", docHost_+href);
				if(!anchs[iEl].attr("class").length)	anchs[iEl].setAttr("class", "external");
			}
			lnkCnt++;
		}
	}

	docTree.save(0x3C0, resDir+pLang+"/"+pOffLn);
	pCnt++;
	//SYS.messInfo("OffLine", "TEST 00: pLang="+pLang);
}

res = "0: Fetched and processed pages="+pCnt+"; images="+imgCnt+"; links="+lnkCnt+"; languages="+lngCnt;','','',1511637642);
CREATE TABLE 'flb_regEl' ("ID" TEXT DEFAULT '' ,"NAME" TEXT DEFAULT '' ,"uk#NAME" TEXT DEFAULT '' ,"ru#NAME" TEXT DEFAULT '' ,"DESCR" TEXT DEFAULT '' ,"uk#DESCR" TEXT DEFAULT '' ,"ru#DESCR" TEXT DEFAULT '' ,"START" INTEGER DEFAULT '1' ,"MAXCALCTM" INTEGER DEFAULT '10' ,"PR_TR" INTEGER DEFAULT '0' ,"FORMULA" TEXT DEFAULT '' ,"uk#FORMULA" TEXT DEFAULT '' ,"ru#FORMULA" TEXT DEFAULT '' ,"TIMESTAMP" INTEGER DEFAULT '' , PRIMARY KEY ("ID"));
INSERT INTO "flb_regEl" VALUES('pidUnif','PID (unified)','ПІД (уніфікований)','ПИД (унифицированный)','Composite-unified analog and pulse PID. At the heart of the regulator is core a standard analog PID controller from the library "FLibComplex1" (http://wiki.oscada.org/HomePageEn/Doc/FLibComplex1#h902-15) and the implementation of the PWM for the pulse part.','Суміщений-уніфікований аналоговий та імпульсний ПІД-регулятор. У основі регулятора лежить мова стандартного аналогового ПІД-регулятора з бібліотеки "FLibComplex1" та реалізація ШІМ для імпульсної частини.','Совмещённый-унифицированный аналоговый и импульсный ПИД-регулятор. В основе регулятора лежит ядро стандартного аналогового ПИД-регулятора из библиотеки "FLibComplex1" (http://wiki.oscada.org/Doc/FLibComplex1#h91-15) и реализация ШИМ для импульсной части.',1,10,0,'//Call standard analog PID
outA = Special.FLibComplex1.pid(var,sp,max,min,manIn,auto,casc,Kp,Ti,Kd,Td,Tzd,Hup,Hdwn,Zi,followSp,K1,in1,K2,in2,K3,in3,K4,in4,f_frq,int,dif,lag);
if(analog) return outA;

if(f_start || f_stop) { impQup = impQdwn = false; return 0; }

//Call impuls generator
outA = 2*(outA-50);
if(perLag <= 0 && ((outA*KImpRfact) >= (100*TImpMin/TImpPer) || (-outA/KImpRfact) >= (100*TImpMin/TImpPer) || (!auto && abs(outA) > 0.1)))
{
	impLag = max(abs(outA)*TImpPer/100, TImpMin);
	impLag = (outA>0) ? impLag*KImpRfact : impLag/KImpRfact;
	perLag = TImpPer;
	if(outA > 0) impQup = true; else impQdwn = true;  
}
else if(perLag > 0) {
	perLag -= 1e3/f_frq;
	if((TImpPer-perLag) >= impLag) {
		impQup = impQdwn = false;
		if(!auto) perLag = 0;
	}
}

if(!auto) manIn = 50;

if(impQupTm > 1 || (impQupTm && !impQup)) impQupTm--;
if(impQup && !impQupTm) impQupTm = 2*f_frq;
if(impQdwnTm > 1 || (impQdwnTm && !impQdwn)) impQdwnTm--;
if(impQdwn && !impQdwnTm) impQdwnTm = 2*f_frq;

if(impAnImit) {
	if(impQup)	out += 100/(impAnOut*f_frq);
	if(impQdwn)	out -= 100/(impAnOut*f_frq);
	return max(0,min(100,out));
}
return impAnOut;','','',1441908785);
INSERT INTO "flb_regEl" VALUES('pidUnifD','PID dynamic','ПІД динамічний','ПИД динамический','Completely identical to unified PID regulator is implemented dynamically on JavaLikeCalc. The dynamic implementation allows you to easily adapt the regulator to the desired requirements, simply by editing it.','Повністю ідентичний уніфікованому ПІД регулятор, реалізований динамічно на JavaLikeCalc. Динамічна реалізація дозволяє легко адаптувати регулятор під потрібні вимоги, просто відредагувати його.','Полностью идентичный унифицированному ПИД регулятор, реализованный динамически на JavaLikeCalc. Динамическая реализация позволяет легко адаптировать регулятор под нужные требования, просто отредактировав его.',1,10,0,'if(f_start) {
	auto_ = auto;
	outA = out;
	impQupTm_ = impQdwnTm_ = 0;
	impQup_ = impQdwn_ = 0;
}

//Call standard analog PID
Kzd = min(1e3/(f_frq*Tzd),1);
Kint = min(1e3/(f_frq*Ti),1);
Kdif = min(1e3/(f_frq*Td),1);

//Scale error
if(var.isEVal() || max <= min) return out;
sp = max(min,min(max,sp));

//Prepare values
spv = 100*(sp-min)/(max-min);
val = 100*(var-min)/(max-min);
val += K1*in1 + K2*in2;
val = min(100,max(-100,val));

//Error
err = spv - val;
err = (abs(err)<Zi) ? 0 : ((err>0) ? err-Zi : err+Zi );
err *= Kp;
err = min(100,max(-100,err));

integ += Kint*err;						//Integral
difer -= Kdif*(difer-val);			//Differential wait
dlag  += Kzd*((difer-val)-dlag);	//Differential lag

//Automatic mode enabled
if(auto) {
	outA_ = err + integ + Kd*dlag + K3*in3 + K4*in4;
	if(QO) outA_ = min(outA, outA_);
	if(QZ) outA_ = max(outA, outA_);
	outA = outA_;
}
else {
	casc = false;
	if(followSp) sp = var;
	outA = manIn;
}

//Check output limits
outA = manIn = min(Hup,max(Hdwn,outA));

//Fix integral for manual and limits
integ = outA - err - Kd*dlag - K3*in3 - K4*in4;

if(analog) return outA;

if(f_start || f_stop) { impQup = impQdwn = false; return 0; }

//Call impuls generator
outA_ = (impAnOut < 0) ? 2*(outA-50) : (outA-impAnOut);
if(perLag <= 0 && ((outA_*KImpRfact) >= (100*TImpMin/TImpPer) || (-outA_/KImpRfact) >= (100*TImpMin/TImpPer)))
{
	impLag = max(abs(outA_)*TImpPer/100,TImpMin);
	impLag = (outA_>0) ? impLag*KImpRfact : impLag/KImpRfact;
	perLag = TImpPer;
	if(outA_ > 0) impQup = !QO; else impQdwn = !QZ;
}
else if(perLag > 0) {
	perLag -= 1e3/f_frq;
	if((TImpPer-perLag) >= impLag) {
		impQup = impQdwn = false;
		if(!auto) perLag = 0;
	}
}

if(!auto && auto != auto_)	manIn = (impAnOut < 0) ? 50 : impAnOut;
auto_ = auto;

if(impQup && !impQup_) impQupTm_ = 2*f_frq;
impQupTm_ = max(0,impQupTm_-1);
impQupTm = (impQup || impQupTm_ > 0.1);
impQup_ = impQup;
if(impQdwn && !impQdwn_) impQdwnTm_ = 2*f_frq;
impQdwnTm_ = max(0,impQdwnTm_-1);
impQdwnTm = (impQdwn || impQdwnTm_ > 0.1);
impQdwn_ = impQdwn;

if(impAnImit > 0) {
	if(impQup)	out += 100/(impAnImit*f_frq);
	if(impQdwn)	out -= 100/(impAnImit*f_frq);
	return max(0, min(100,out));
}
return max(0, min(100,impAnOut));','','',1479063083);
INSERT INTO "flb_regEl" VALUES('pidImp','PID pulse','ПІД імпульсний','ПИД импульсный','Specialized pulse PID regulator is implemented on a special algorithm with compensation of double integration.','Спеціалізований імпульсний ПІД регулятор реалізований за особливим алгоритмом з компенсацією подвійного інтегрування.','Специализированный импульсный ПИД регулятор реализованный по особому алгоритму с компенсацией двойного интегрирования.',1,10,0,'//Call standard analog PID
Kzd = min(1e3/(f_frq*Tzd), 1);
Kint = min(1e3/(f_frq*Ti), 1);
Kdif = min(1e3/(f_frq*Td), 1);

//Scale error
if(max <= min) return 0;

//Prepare values
spv = 100*(sp-min)/(max-min);
val = 100*(var-min)/(max-min);
val += K1*in1 + K2*in2;
val = min(100,max(-100,val));

//Error
err = spv - val;
err = (abs(err)<Zi) ? 0 : ((err>0) ? err-Zi : err+Zi );
err *= Kp;
err = min(100,max(-100,err));
errP = err-err1;
errD = err1-err2;
err2 = err1; err1 = err;

integ += Kint*err;							//Integral
difer -= Kdif*(difer-errD);				//Differecial wait
dlag  += Kzd*((errP-difer)-dlag);	//Differecial lag

//Automatic mode enabled
if(auto) { outPID = errP + Ki*err + integ + Kd*dlag; outA = 50 + outPID + K3*in3 + K4*in4; }
else {
	outPID = 0;
	casc = false;
	if(followSp) sp = var;
	outA = manIn;
}

//Check output limits
outA = manIn = min(Hup, max(Hdwn,outA));

//Fix integral for manual and limits
//integ = outA - errP - Kd*dlag - K3*in3 - K4*in4;

if(f_start || f_stop) { impQup = impQdwn = false; return 0; }

//Call impuls generator
outA = 2*(outA-50);
if(perLag <= 0 && ((outA*KImpRfact) >= (100*TImpMin/TImpPer) || (-outA/KImpRfact) >= (100*TImpMin/TImpPer) || (!auto && abs(outA) > 0.1)))
{
	impLag = max(abs(outA)*TImpPer/100,TImpMin);
	impLag = (outA > 0) ? impLag*KImpRfact : impLag/KImpRfact;
	perLag = TImpPer;
	if(outA > 0) impQup = true; else impQdwn = true;  
}
else if(perLag > 0) {
	perLag -= 1e3/f_frq;
	if((TImpPer-perLag) >= impLag) {
		impQup = impQdwn = false;
		if(!auto) perLag = 0;
  }
}

if(!auto) manIn = 50;

if(impQupTm > 1 || (impQupTm && !impQup)) impQupTm--;
if(impQup && !impQupTm) impQupTm = 2*f_frq;
if(impQdwnTm > 1 || (impQdwnTm && !impQdwn)) impQdwnTm--;
if(impQdwn && !impQdwnTm) impQdwnTm = 2*f_frq;

if(impQup || impQdwn) integ = 0;

if(impAnImit) {
	if(impQup) out += 100/(impAnOut*f_frq);
	if(impQdwn) out -= 100/(impAnOut*f_frq);
	return max(0, min(100,out));
}
return impAnOut;','','',1479063320);
CREATE TABLE 'UserProtocol_uPrt' ("ID" TEXT DEFAULT '' ,"NAME" TEXT DEFAULT '' ,"uk#NAME" TEXT DEFAULT '' ,"ru#NAME" TEXT DEFAULT '' ,"DESCR" TEXT DEFAULT '' ,"uk#DESCR" TEXT DEFAULT '' ,"ru#DESCR" TEXT DEFAULT '' ,"EN" INTEGER DEFAULT '0' ,"PR_TR" INTEGER DEFAULT '1' ,"WaitReqTm" INTEGER DEFAULT '0' ,"InPROG" TEXT DEFAULT '' ,"uk#InPROG" TEXT DEFAULT '' ,"ru#InPROG" TEXT DEFAULT '' ,"OutPROG" TEXT DEFAULT '' ,"uk#OutPROG" TEXT DEFAULT '' ,"ru#OutPROG" TEXT DEFAULT '' ,"TIMESTAMP" INTEGER DEFAULT '0' , PRIMARY KEY ("ID"));
INSERT INTO "UserProtocol_uPrt" VALUES('SMS','','','','Provides operations with SMS by GSM-modem connected as serial device. For now supported only sending SMS messages to a number of remote cell phone or GSM modem.
Author: Roman Savochenko <rom_as@oscada.org>
Version: 2.1.1','','',1,0,0,'','','','JavaLikeCalc.JavaScript
//Request form:
//<{cmd} pin="1111" tel="+380XXXXXXXXX" text="{0|1}" err="1:Error">{SMSText}</cmd>
//  cmd - command, for now only "send" allowed;
//  pin - PIN code to the SIM card access;
//  tel - telephone number for receiver (remote cell phone or GSM modem);
//  text{0,1} - SMS in the text mode, only ASCII
//  SMSText - the message text;
//  err - sets for the request result.

//Transport''s timings check
if(tr.timings() != "5000:100")	tr.timings("5000:100");

if(io.name() == "send") {
	tel = io.attr("tel");
	if(!tel.length || tel[0] != "+") { io.setAttr("err", "100:"+tr("Telephone number error.")); return; }

	//Disable ECHO
	rez = tr.messIO("ATE0\r");
	while(rez.length && (trez=tr.messIO("")).length) rez += trez;
	if(rez.indexOf("OK\r") < 0)	{ io.setAttr("err", "101:"+tr("Disable ECHO error: %1").replace("%1",rez)); return; }

	//Set PIN
	if(io.attr("pin").length) {
		//Check for PIN
		rez = tr.messIO("AT+CPIN?\r");
		while(rez.length && (trez=tr.messIO("")).length) rez += trez;
		if(rez.indexOf("OK\r") < 0)	{ io.setAttr("err", "102:"+tr("Check for PIN-code error: %1").replace("%1",rez)); return; }
		if(rez.indexOf("+CPIN: READY\r") < 0) {	//PIN is needed
			rez = tr.messIO("AT+CPIN="+io.attr("pin")+"\r");
			while(rez.length && (trez=tr.messIO("")).length) rez += trez;
			if(rez.indexOf("OK\r") < 0)	{ io.setAttr("err", "102:"+tr("Set PIN error: %1").replace("%1",rez)); return; }
		}
	}

	if(io.attr("text").toInt()) {	//SMS in the text mode
		//Switch to Text SMS mode
		rez = tr.messIO("AT+CMGF=1\r");
		while(rez.length && (trez=tr.messIO("")).length) rez += trez;
		if(rez.indexOf("OK\r") < 0)	{ io.setAttr("err", "103:"+tr("Set SMS Text mode error: %1").replace("%1",rez)); return; }

		//Send the Telephone number
		rez = tr.messIO("AT+CMGS=\""+tel+"\"\r");
		while(rez.length && (trez=tr.messIO("")).length) rez += trez;
		if(rez.indexOf(">") < 0)	{ io.setAttr("err", "104:"+tr("Sent SMS error: %1").replace("%1",rez)); return; }
		rez = tr.messIO(io.text()+"\x1A");
		while(rez.length && (trez=tr.messIO("")).length) rez += trez;
		//for(var iTr = 0; iTr < 100 && rez.indexOf("OK\r") < 0; iTr++) rez += tr.messIO("");	//Up to 10 seconds wait for reply
		if(rez.indexOf("OK\r") < 0)	{ io.setAttr("err", "104:"+tr("Sent SMS PDU error: %1").replace("%1",rez)); return; }
	}
	else {	//SMS in the PDU mode
		// Switch to PDU SMS mode
		rez = tr.messIO("AT+CMGF=0\r");
		while(rez.length && (trez=tr.messIO("")).length) rez += trez;
		if(rez.indexOf("OK\r") < 0)	{ io.setAttr("err", "103:"+tr("Set SMS PDU mode error: %1").replace("%1",rez)); return; }

		//Prepare PDU
		pdu = "001100";	//SMS center number (default) + SMS-Submit
		//Telephone number encode
		tel = tel.slice(1);
		pdu += tel.length.toString(16,2) + "91";	//Telephone length and type
		while(tel.length < 12) tel += "F";
		for(i = 0; i < 6; i++) pdu += tel[i*2+1]+tel[i*2];
		//Message encode
		text = SYS.strCodeConv(io.text(),"","UCS2");
		if((text.length/2) > 70)	text = text.slice(0,140);
		//{ io.setAttr("err", "104:"+tr("Too long length (%1) of the message.").replace("%1",(text.length/2).toString())); return; }
		pdu += "0018C1"+(text.length).toString(16,2);
		for(i = 0; i < text.length/2; i++) pdu += text.charCodeAt(i*2+1).toString(16,2)+text.charCodeAt(i*2).toString(16,2);
		//SYS.messDebug("TEST SMS","PDU :"+pdu);

		// Send the PDU message
		rez = tr.messIO("AT+CMGS="+(pdu.length/2-1)+"\r");
		while(rez.length && (trez=tr.messIO("")).length) rez += trez;
		if(rez.indexOf(">") < 0)	{ io.setAttr("err", "104:"+tr("Sent SMS error: %1").replace("%1",rez)); return; }
		rez = tr.messIO(pdu+"\x1A");
		while(rez.length && (trez=tr.messIO("")).length) rez += trez;
		//for(var iTr = 0; iTr < 100 && rez.indexOf("OK\r") < 0; iTr++) rez += tr.messIO("");	//Up to 10 seconds wait for reply
		if(rez.indexOf("OK\r") < 0)	{ io.setAttr("err", "104:"+tr("Sent SMS PDU error: %1").replace("%1",rez)); return; }
		//SYS.messDebug("TEST SMS","PDU REZ :"+rez);
	}
	io.setAttr("err", "0");
}','','',1478107736);
INSERT INTO "UserProtocol_uPrt" VALUES('SCU750','EDWARDS TURBOMOLECULAR PUMPS','','','Protocol level of typical EDWARDS TURBOMOLECULAR PUMPS (http://edwardsvacuum.com) data request by SCU750 Cotrol Unit protocol.
Author: Roman Savochenko <rom_as@oscada.org>
Sponsored: Vasiliy Grigoriev from "Vacuum technologies laboratory (http://e-beam.ru)".
Version: 1.0.0','','',1,0,0,'','','','JavaLikeCalc.JavaScript
//Request form:
//<mess addr="1" err="1:Error">{req}</mess> - message tag
//  req - request/respond data;
//  addr - remote station address (<0 - single; >=0 - multi port);
//  err - sets for the request result.
if(io.text().length > 255*255) { io.setAttr("err","1:"+tr("Message''s length more 255*255")); return; }
addr = io.attr("addr").toInt();
k = ceil(io.text().length/255);	//transmission blocks
for(i_k = 1; i_k <= k; i_k++) {
	request = "\x02"+k.toString(16,3) + io.text().slice((i_k-1)*255,(i_k-1)*255+min(255,io.text().length-(i_k-1)*255)) + ((k>1&&i_k<k)?"\x17":"\x03");
	// Calc LRC
	LRC = 0xFF;
	for(i = 0; i < request.length; i++) LRC = LRC^request.charCodeAt(i);
	request += SYS.strFromCharCode(LRC);

	// Multy port
	if(addr>=0) request = "@"+addr.toString(16,2)+request;
	//SYS.messDebug("PRT","Request: "+Special.FLibSYS.strDec4Bin(request));

	//Send the request
	resp = tr.messIO(request);
	while(resp.length) {
		tresp = tr.messIO("");
		if(!tresp.length) break;
  		resp += tresp;
	}
	if(!resp.length) { io.setAttr("err","2:"+tr("No respond")); return; }
	//SYS.messDebug("PRT","Ack: "+Special.FLibSYS.strDec4Bin(resp));
	if(resp.charCodeAt(0) != 6) { io.setAttr("err","3:"+tr("No acknowledgment")); return; }
	// Pass included acknowledgement
	resp = resp.slice((addr>=0)?3:1);

	// Read data blocks
	io.setText("");
	for(i_k = 1; true; i_k++) {
		//Send application acknowledgement and wait data
		if(!resp.length) {
			request = "\x06";
			if(addr >= 0) request += addr.toString(16,2);
			resp = tr.messIO(request);
			while(resp.length) {
				tresp = tr.messIO("");
				if(!tresp.length) break;
  				resp += tresp;
			}
			if(!resp.length) { io.setAttr("err","4:"+tr("No data block get")); return; }
		}
		if(resp.length < ((addr>=0)?10:7) || resp.charCodeAt(0) != 0x40) { io.setAttr("err","5:"+tr("Data block short or error")); return; }

		//SYS.messDebug("PRT","BLK "+i_k+": "+Special.FLibSYS.strDec4Bin(resp));

		if(addr >= 0) resp = resp.slice(3);
		LRC = 0xFF;
		for(i = 0; i < (resp.length-1); i++) LRC = LRC^resp.charCodeAt(i);
		if(LRC != resp.charCodeAt(resp.length-1)) { io.setAttr("err","6:"+tr("LRC error.")); return; }
		if(i_k != resp.slice(1,4).toInt(16)) { io.setAttr("err","7:"+tr("Block sequence.")); return; }
		io.setText(io.text()+resp.slice(4,resp.length-2));
		if(resp.charCodeAt(resp.length-2) == 0x03) break;
		if(resp.charCodeAt(resp.length-2) == 0x17) { resp = ""; continue; }
		io.setAttr("err","8:"+tr("Unknown block end."));
		return;
	}
}','','',1509283349);
INSERT INTO "UserProtocol_uPrt" VALUES('TMH','Power supply for turbomolecular pumps','','','Protocol level of power supply for turbomolecular pumps of firm SHIMADZU (http://www.shimadzu.com), model EI-R04M.
Author: Roman Savochenko <rom_as@oscada.org>
Sponsored: Vasiliy Grigoriev from "Vacuum technologies laboratory (http://e-beam.ru)".
Version: 1.0.0','','',1,0,0,'','','','JavaLikeCalc.JavaScript
//Request form:
//<mess addr="1" err="1:Error">{req}</mess> - message tag
//  req - request/respond data;
//  addr - remote station address (1...32);
//  err - sets for the request result.

io.setAttr("err","");
addr = io.attr("addr").toInt();
if(addr < 1 || addr > 32) { io.setAttr("err","1:"+tr("Device address out of range 1...32")); return; }
request = "MJ"+addr.toString(10,2)+io.text();
//Calc CRC
CRC = 0;
for(i = 0; i < request.length; i++) CRC += request.charCodeAt(i);
request += (CRC&0xFF).toString(16,2)+"\r";
//SYS.messDebug("PRT","Request: "+Special.FLibSYS.strDec4Bin(request));

//Send request
resp = tr.messIO(request);
while(resp.length && resp[resp.length-1] != "\r") {
	tresp = tr.messIO("");
	if(!tresp.length) break;
  	resp += tresp;
}
if(resp.length < 6 || resp[resp.length-1] != "\r" || resp.slice(0,2) != "MJ" || resp.slice(2,4).toInt() != addr)
{ io.setAttr("err","2:"+tr("No or error respond")); return; }
//SYS.messDebug("PRT","Respond: "+Special.FLibSYS.strDec4Bin(resp));
CRC = 0;
for(i = 0; i < (resp.length-3); i++) CRC += resp.charCodeAt(i);
if((CRC&0xFF) != resp.slice(resp.length-3,resp.length-1).toInt(16)) { io.setAttr("err","6:"+tr("CRC error.")); return; }
io.setText(resp.slice(4,resp.length-3));','','',1509283419);
INSERT INTO "UserProtocol_uPrt" VALUES('VKT7','VKT-7','','','Protocol level of firm "Teplocom" (http://www.teplocom.spb.ru) computer "VKT-7" for complex heat measurement and counting. The device complex enough to provide more parameters, more history and accessed by a nonlinear Serial-based protocol at low speed. The template implements acquisition for all significant parameters, gets for their history by hours, days and result months. Also you can to append easily enough for processing of the remained parameters.
Author: Roman Savochenko <rom_as@oscada.org>
Sponsored: Vladislav Chubuk
Version: 1.0.0','','',1,0,0,'','','','JavaLikeCalc.JavaScript
//Request form:
//<mess addr="1" err="1:Error">{req}</mess> - message tag
//  req - request/respond data;
//  addr - remote station address (0...254);
//  err - sets for the request result.
io.setAttr("err", "");
addr = io.attr("addr").toInt();
if(addr < 0 || addr > 254) { io.setAttr("err", "1:"+tr("Device address out of range 0...254")); return; }
request = SYS.strFromCharCode(addr) + io.text();
//Calc KS
KS = 0xFFFF;
for(i = 0; i < request.length; i++) {
	KS = KS ^ request.charCodeAt(i);
	for(j = 0; j < 8; j++)
		KS = (KS&0x01) ? (KS >> 1)^0xA001 : (KS >> 1);
}
request = SYS.strFromCharCode(0xFF,0xFF) + request + SYS.strFromCharCode(KS,KS>>8);
SYS.messDebug("/VKT7/PRT","Request: "+Special.FLibSYS.strDec4Bin(request));

//Send request
resp = tr.messIO(request);
while(resp.length) {
	tresp = tr.messIO("");
	if(!tresp.length) break;
  	resp += tresp;
}
if(resp.length < 4 || resp.charCodeAt(0) != addr)	{ io.setAttr("err", "2:"+tr("No or error respond")); return; }
SYS.messDebug("/VKT7/PRT","Respond: "+Special.FLibSYS.strDec4Bin(resp));

//Calc KS
KS = 0xFFFF;
for(i = 0; i < (resp.length-2); i++) {
	KS = KS ^ resp.charCodeAt(i);
	for(j = 0; j < 8; j++)
		KS = (KS&0x01) ? (KS >> 1)^0xA001 : (KS >> 1);
}
if(KS != ((resp.charCodeAt(resp.length-1)<<8)|resp.charCodeAt(resp.length-2)))
{ io.setAttr("err","6:"+tr("KS error")+" "+KS.toString(16,4)+"=="+((resp.charCodeAt(resp.length-1)<<8)|resp.charCodeAt(resp.length-2)).toString(16,4)); return; }
if(resp.charCodeAt(1)&0x80)
{ io.setAttr("err","7:"+resp.charCodeAt(2)+":"+tr("Request error.")); return; }
io.setText(resp.slice(1,-2));','','',1509283495);
INSERT INTO "UserProtocol_uPrt" VALUES('DCON','DCON','','','Mostly aimed for example implement user protocols into OpenSCADA and contains only main functions into the protocol part.
Author: Roman Savochenko <rom_as@oscada.org>
Version: 1.0.1','','',1,0,0,'JavaLikeCalc.JavaScript
//Processing: test implements only processing for requests ''#'' and ''@'' with fixed reply data.

var enCRC = true;
//SYS.messDebug("/DCON/in","REQ: "+request);
//Test request for full
if(request.length < 4 || request[request.length-1] != "\r") {
  if(request.length > 10) request = "";
  return true;
}
//Check for integrity of the request (CRC) and address
if(enCRC) {
	CRC = 0;
	for(i = 0; i < (request.length-3); i++) CRC += request.charCodeAt(i);
	if(CRC != request.slice(request.length-3,request.length-1).toInt(16) || request.slice(1,3).toInt(16) != 10) return false;
}
//Analysis of the request and response prepare
if(request[0] == "#") answer = ">+05.123+04.153+07.234-02.356+10.000-05.133+02.345+08.234";
else if(request[0] == "@") answer = ">AB3C";
else answer = "?";
//Finish response
if(enCRC) {
	CRC = 0;
	for(i=0; i < answer.length; i++) CRC += answer.charCodeAt(i);
	answer += (CRC&0xFF).toString(16)+"\r";
}
//SYS.messDebug("/DCON/in","ANSV: "+answer[0]);
return 0;','','','JavaLikeCalc.JavaScript
//Request form:
//<ReqSymb addr="1" err="1:Error">{req}</ReqSymb> - message tag
//  ReqSymb - request type symbol into the tag name, like: ''#'', ''@'', etc
//  req - request/respond data;
//  addr - remote host address [1...240];
//  err - sets for the request result.

//Result request prepare
request = io.name().slice(0,1) + io.attr("addr").toInt().toString(16,2) + io.text();
if(io.attr("CRC").toInt()) {
	CRC = 0;
	for(i = 0; i < request.length; i++) CRC += request.charCodeAt(i);
	request += (CRC&0xFF).toString(16) + "\r";
}
else request += "\r";
//Send request
resp = tr.messIO(request);
while(resp[resp.length-1] != "\r") {
  tresp = tr.messIO("");
  if(!tresp.length) break;
  resp += tresp;
}
if(io.attr("CRC").toInt()) {
	//Analysis response
	if(resp.length < 4 || resp[resp.length-1] != "\r") { io.setAttr("err","10:"+tr("Error or no response.")); return; }
	//Check response to the integrity (CRC)
	CRC = 0;
	for(i = 0; i < (resp.length-3); i++) CRC += resp.charCodeAt(i);
	if(CRC != resp.slice(resp.length-3,resp.length-1).toInt(16)) { io.setAttr("err","11:"+tr("CRC error.")); return; }
}
//Analysis response
else if(resp.length < 2 || resp[resp.length-1] != "\r") { io.setAttr("err","10:"+tr("Error or no response.")); return; }
if(resp[0] != ">") { io.setAttr("err","12:"+resp[0]+":"+tr("DCON error.")); return; }
//The result return
io.setAttr("err","");
io.setText(resp.slice(1,resp.length-3));','','',1424879498);
INSERT INTO "UserProtocol_uPrt" VALUES('TM510x','Elemer TM510x','','','Protocol level of multichannels thermometer Elemer TM5102 and TM5103 of firm Elemer (http://www.elemer.ru).
Author: Roman Savochenko <rom_as@oscada.org>
Version: 1.0.0','','',1,0,0,'','','','JavaLikeCalc.JavaScript
//Request form:
//<mess addr="1" err="1:Error">{req}</mess> - message tag
//  req - request/respond data;
//  addr - remote station address (1...254);
//  err - sets for the request result.
io.setAttr("err", "");
addr = io.attr("addr").toInt();
if(addr < 1 || addr > 254) { io.setAttr("err","1:"+tr("Device address out of range 1...254")); return; }
request = ":"+addr.toString(10)+";"+io.text()+";";
//Calc KS
KS = 0xFFFF;
for(i = 1; i < request.length; i++) {
	KS = KS ^ request.charCodeAt(i);
	for(j = 0; j < 8; j++)
		KS = (KS&0x01) ? (KS >> 1)^0xA001 : (KS >> 1);
}
request += KS.toString(10)+"\r";
//SYS.messDebug("PRT","Request: "+request);

//Send request
resp = tr.messIO(request);
while(resp.length && resp[resp.length-1] != "\r") {
	tresp = tr.messIO("");
	if(!tresp.length) break;
  	resp += tresp;
}
if(resp.length && resp.charCodeAt(0) == 0xFF) resp = resp.slice(1);
if(resp.length < 7 || resp[resp.length-1] != "\r" ||
	resp[0] != "!" || resp.slice(1).toInt() != addr || (KSpos=resp.lastIndexOf(";")) < 0)
{ io.setAttr("err","2:"+tr("No or error respond")); return; }
//SYS.messDebug("PRT","Respond: "+resp);

//Calc KS
KS = 0xFFFF;
for(i = 1; i < min(KSpos+1,resp.length); i++) {
	KS = KS ^ resp.charCodeAt(i);
	for(j = 0; j < 8; j++)
		KS = (KS&0x01) ? (KS >> 1)^0xA001 : (KS >> 1);
}
if(KS != resp.slice(KSpos+1).toInt(10)) { io.setAttr("err","6:"+tr("KS error.")); return; }
io.setText(resp.slice(resp.indexOf(";")+1,KSpos));','','',1509283413);
INSERT INTO "UserProtocol_uPrt" VALUES('SMDP','Sycon Multi Drop Protocol','','','Protocol level of STM devices for resonant frequency measurement for mass of deposited films attached to its surface by Sycon Instruments, Inc. (http://www.sycon.com)
Author: Roman Savochenko <rom_as@oscada.org>
Sponsored: Vasiliy Grigoriev from "Vacuum technologies laboratory (http://e-beam.ru)".
Version: 1.0.0','','',1,0,0,'','','','JavaLikeCalc.JavaScript
//Request form:
//<mess addr="16" try="1" err="1:Error">{req}</mess> - message tag
//req - request/respond data;
//addr - remote device address [16...254];
//try - tries for the request;
//err - sets for the request result.

if(!io.text().length) { io.setAttr("err","1:"+tr("Message empty")); return; }
addr = io.attr("addr").toInt();
try = min(5,max(1,io.attr("try").toInt()));
request = "\x02"+SYS.strFromCharCode(addr) + io.text();
//Calc CHKSUM
CRC = 0;
for(i = 1; i < request.length; i++) CRC += request.charCodeAt(i);
request += SYS.strFromCharCode((CRC>>4&0xF)+0x30,(CRC&0xF)+0x30,0x0D);

SYS.messDebug("/UserProt/SMDP",tr("Request:")+" "+Special.FLibSYS.strDec4Bin(request));

for( ; try > 0; try--) {
	//Send the request
	resp = tr.messIO(request);
	while(resp.length && resp.charCodeAt(resp.length-1) != 0x0D)
		if((tresp=tr.messIO("")).length) resp += tresp;
		else break;

	if(!resp.length)	{ io.setAttr("err","2:"+tr("No respond")); continue; }
	if(resp.length < 6 || resp.charCodeAt(resp.length-1) != 0x0D)	{ io.setAttr("err","3:"+tr("Not full respond")); continue; }
	if(resp.charCodeAt(0) != 0x02 || resp.charCodeAt(1) != addr)	{ io.setAttr("err","4:"+tr("Wrong respond")); continue; }

	SYS.messDebug("/UserProt/SMDP",tr("Respond:")+" "+Special.FLibSYS.strDec4Bin(resp));

	CRC = 0;
	for(i = 1; i < (resp.length-3); i++) CRC += resp.charCodeAt(i);
	if((CRC&0xFF) != (((resp.charCodeAt(resp.length-3)-0x30)<<4)|(resp.charCodeAt(resp.length-2)-0x30)))
	{ io.setAttr("err","6:"+tr("CRC error.")); continue; }
	break;
}
if(try <= 0)	return;

errCd = resp.charCodeAt(2)&0x7;
if(errCd == 1) io.setAttr("err","").setText(resp.slice(2,-2));
else if(errCd == 2) io.setAttr("err",""+(10+errCd)+":Err_inv_cmd: "+tr("Illegal command (CMD code not valid)."));
else if(errCd == 3) io.setAttr("err",""+(10+errCd)+":Err_syntax: "+tr("Syntax error. (Too many bytes in data field, not enough bytes, etc)."));
else if(errCd == 4) io.setAttr("err",""+(10+errCd)+":Err_range: "+tr("Data range error."));
else if(errCd == 5) io.setAttr("err",""+(10+errCd)+":Err_inhibited: "+tr("Inhibited."));
else if(errCd == 6) io.setAttr("err",""+(10+errCd)+":Err_obso: "+tr("Obsolete command. No action taken, but not really an error."));
else io.setAttr("err",""+(10+errCd)+":"+tr("Unknown error."));','','',1509283368);
INSERT INTO "UserProtocol_uPrt" VALUES('OWEN','OWEN','','','Protocol level of mostly models of OWEN devices by specific protocol.
Author: Constantine (IrmIngeneer)
Refactoring: Roman Savochenko <rom_as@oscada.org>','','',1,0,0,'','','','JavaLikeCalc.JavaScript
//Request form:
//<mess addr="1" err="1:Error">{req}</mess> - message tag
//  req - request/respond data;
//  addr - remote station address (0...2047);
//  err - sets for the request result.
io.setAttr("err", "");
addr = io.attr("addr").toInt();
if(addr < 0 || addr > 2047) { io.setAttr("err", "1:"+tr("Device address out of range 0...2047")); return; }
request = SYS.strFromCharCode((addr<256)?addr:addr>>3, ((addr<256)?0:addr<<5)|0x10) + io.text();

// CRC append
for(CRC = 0, i = 0; i < request.length; i++) {
	b = request.charCodeAt(i);
	for(j = 0; j < 8; j++, b = b << 1)
		CRC = ((b^(CRC>>8)&0x80)?(CRC<<1)^0x8F57:CRC<<1) & 0xFFFF;
}
request += SYS.strFromCharCode(CRC>>8, CRC&255);

// Convert to ASCII
for(reqASCII = "", i = 0; i < request.length; i++)
	reqASCII += SYS.strFromCharCode((request.charCodeAt(i)>>4)+71, (request.charCodeAt(i)&0x0F)+71);

// Result request
request = "#" + reqASCII + "\r";

//Send the request and get respond
for(resp = tr.messIO(request); resp[resp.length-1] != "\r" && (respTail=tr.messIO("")).length; ) resp += respTail;

//Respond process
if(resp.length < 14 || resp[resp.length-1] != "\r" || resp[0] != "#" || (resp.length%2) != 0) {
	io.setAttr("err", "10:"+tr("Wrong or no a respond."));
	return; 
}

// Remove markers
resp = resp.slice(1, resp.length-1);

// Convert to binary
for(respBin = "", b = 0, i = 0, fullB = false; i < resp.length; i++, fullB = !fullB) {
	b += resp.charCodeAt(i) - 71;
	if(fullB) { respBin += SYS.strFromCharCode(b); b = 0; } else b *= 16;
}
resp = respBin;

// Check for CRC
for(CRC = 0, i = 0; i < resp.length-2; i++) {
	b = resp.charCodeAt(i);
	for(j = 0; j < 8; j++, b = b << 1)
		CRC = ((b^(CRC>>8)&0x80)?(CRC<<1)^0x8F57:CRC<<1) & 0xFFFF;
}
if(CRC != ((resp.charCodeAt(resp.length-2)<<8)+resp.charCodeAt(resp.length-1))) {
	io.setAttr("err", "11:"+tr("CRC error."));
	return;
}

//Return result
io.setText(resp.slice(0,resp.length-2));','','',1425737955);
INSERT INTO "UserProtocol_uPrt" VALUES('IT3','Temperature measurement IT-3','','','Protocol level of temperature measurement IT-3 from OmskEtalon (http://www.omsketalon.ru).
Author: Roman Savochenko <rom_as@oscada.org>
Sponsored: Vasiliy Grigoriev from "Vacuum technologies laboratory (http://e-beam.ru)".
Version: 1.2.0','','',1,0,0,'JavaLikeCalc.JavaScript
// Input part of the protocol IT-3
// for work into the background/listen mode to an other interaction session.
var prm;
if(prm.isEVal())	prm = SYS.nodeAt("/DAQ/LogicLev/cryo/prm_IT3");
if(prm.isEVal())	return false;	//Drop all request but the target parameter''s object wrong
if(!request.length) {					//Empty request means it is not a data for the pointed time, 1 second
	prm.err.set("1:"+tr("No data"), 0, 0, true);
	prm.T.set(EVAL, 0, 0, true);
	prm.relSt.set(EVAL, 0, 0, true);
	prm.L.set(EVAL, 0, 0, true);
	prm.H.set(EVAL, 0, 0, true);
	return true;
}
//SYS.messDebug("/IT3/IN","TEST 00: req="+SYS.strDecode(request,"Bin"," "));

while(request.length >= 6) {	//the full header
	if(request.charCodeAt(0) != 0x16)	{ request = ""; break; }		//The package is broken, drop full

	if((6+(blkLen=request.charCodeAt(1))) > request.length)	break;	//The package is not full, wait

	//The package''s header control sum check and pass broken packages
	for(KS = 0, i = 1; i < 4; i++) KS += request.charCodeAt(i);
	KS = (~(KS+1))&0xFFFF;
	if(((request.charCodeAt(5)<<8)+request.charCodeAt(4)) != KS) { request = request.slice(6+blkLen); continue; }

	//The package''s body control sum check and pass broken packages
	for(KS = 0, i = 6; i < (6+blkLen-2); i++) KS += request.charCodeAt(i);
	KS = (~(KS+1))&0xFFFF;
	if(((request.charCodeAt(6+blkLen-1)<<8)+request.charCodeAt(6+blkLen-2)) != KS)	{ request = request.slice(6+blkLen); continue; }

	if(request.charCodeAt(2))	{ request = request.slice(6+blkLen); continue; }	//The package is a request, pass

	//Info respond package
	if(request.charCodeAt(6) == 0x80)
		prm.err.set((errCode=request.charCodeAt(6+1))?("5:"+errCode+":"+tr("Request error")):"0", 0, 0, true);
	//Data respond package
	if(request.charCodeAt(6) == 0x85) {
		errCode = 0;//prm.err.get().toInt();
		cntrB = request.charCodeAt(6+3);
		T = errCode ? EVAL : ((request.charCodeAt(6+2)<<8)+request.charCodeAt(6+1));
		if(!T.isEVal())	T = T/((cntrB&0x8)?100:10);
		prm.T.set(T, 0, 0, true);
		prm.relSt.set(errCode?EVAL:(cntrB&0x1), 0, 0, true);
		prm.L.set(errCode?EVAL:(cntrB&0x2), 0, 0, true);
		prm.H.set(errCode?EVAL:(cntrB&0x4), 0, 0, true);
		prm.cnt.set(prm.cnt.get()+1, 0, 0, true);
	}

	request = request.slice(6+blkLen);
}

return true;','','','JavaLikeCalc.JavaScript
//Request form:
//<mess addr="1" err="1:Error">{req}</mess> - message tag
//  req - request/respond data;
//  addr - remote station address (1...32);
//  err - sets for the request result.
io.setAttr("err", "");
addr = io.attr("addr").toInt();
if(addr < 1 || addr > 32) { io.setAttr("err","1:"+tr("Device address out of range 1...32")); return; }
//Header
reqHdr = SYS.strFromCharCode(0x16,0x03,addr,0x00);
for(KS = 0, i = 1; i < reqHdr.length; i++)	KS += reqHdr.charCodeAt(i);
KS = ~(KS+1);
reqHdr += SYS.strFromCharCode(KS, KS>>8);
//Body
reqBody = io.text();
for(KS = 0, i = 0; i < reqBody.length; i++)	KS += reqBody.charCodeAt(i);
KS = ~(KS+1);
reqBody += SYS.strFromCharCode(KS, KS>>8);
request = reqHdr+reqBody;
SYS.messDebug("/IT3/PRT",tr("Request")+": "+SYS.strDecode(request,"Bin"," "));

//Send request
resp = tr.messIO(request);
while(resp.length) {
	if(!(tresp=tr.messIO("")).length) break;
  	resp += tresp;
}
if(resp.length == 0) { io.setAttr("err","2:"+tr("No or short respond")); return; }
SYS.messDebug("/IT3/PRT","Respond: "+SYS.strDecode(resp,"Bin"," "));
for(blkOff = 0, blkLen = 0; blkOff < resp.length; blkOff += 6+blkLen) {
	if(resp.charCodeAt(blkOff) != 0x16){ io.setAttr("err","3:"+tr("Header error")); return; }
	for(KS = 0, i = (blkOff+1); i < (blkOff+4); i++) KS += resp.charCodeAt(i);
	KS = (~(KS+1))&0xFFFF;
	if(((resp.charCodeAt(blkOff+5)<<8)+resp.charCodeAt(blkOff+4)) != KS)
	{ io.setAttr("err","3:"+tr("Header CRC error")); return; }
	blkLen = resp.charCodeAt(blkOff+1);
	if(blkOff+6+blkLen > resp.length) { io.setAttr("err","4:"+tr("Info block size error")); return; }
	for(KS = 0, i = (blkOff+6); i < (blkOff+6+blkLen-2); i++) KS += resp.charCodeAt(i);
	KS = (~(KS+1))&0xFFFF;
	if(((resp.charCodeAt(blkOff+6+blkLen-1)<<8)+resp.charCodeAt(blkOff+6+blkLen-2)) != KS)
	{ io.setAttr("err","4:"+tr("Info block CRC error.")); return; }
	if(resp.charCodeAt(blkOff+6) == 0x80 && resp.charCodeAt(blkOff+7))
	{ io.setAttr("err","5:"+resp.charCodeAt(blkOff+7)+":"+tr("Request error")); return; }
	io.setText(resp.slice(blkOff+6,blkOff+6+blkLen));
}','','',1472721683);
INSERT INTO "UserProtocol_uPrt" VALUES('IVE_452HS_02','IVE-452HS-02','IVE-452HS-02','','Protocol level of power supply of beam-electrons evaporator of "Plasma Tech" Ltd, from Moskov.
Author: Roman Savochenko <rom_as@oscada.org>
Sponsored: Vasiliy Grigoriev from "Vacuum technologies laboratory (http://e-beam.ru)".
Version: 1.0.0','','',1,0,0,'','','','JavaLikeCalc.JavaScript
//Request form:
//<mess addr="255" err="1:Error">{req}</mess> - message tag
//  req - request/respond data;
//  addr - remote station address (0...255);
//  err - sets for the request result.
io.setAttr("err", "");
addr = io.attr("addr").toInt();
if(addr < 0 || addr > 255) { io.setAttr("err","1:"+tr("Device address out of range 0...255")); return; }
//Request prepare
req = SYS.strFromCharCode(addr)+io.text();
for(KS = 0, i = 0; i < req.length; i++)	KS += req.charCodeAt(i);
KS = 0x100-KS&0xFF;
req += SYS.strFromCharCode(KS);
SYS.messDebug("/IVE452HS02/PRT",tr("Request")+": "+SYS.strDecode(req,"Bin"," "));

//Send request
resp = tr.messIO(req);
while(resp.length) {
	if(!(tresp=tr.messIO("")).length) break;
  	resp += tresp;
}
if(resp.length == 0) { io.setAttr("err","2:"+tr("No a respond")); return; }
SYS.messDebug("/IVE452HS02/PRT","Respond: "+SYS.strDecode(resp,"Bin"," "));
if(resp.length <= 3 || resp.charCodeAt(0) != addr || resp.charCodeAt(1) != req.charCodeAt(1))
{ io.setAttr("err","3:"+tr("Respond too short or wrong")); return; }
for(KS = 0, i = 0; i < resp.length-1; i++)	KS += resp.charCodeAt(i);
KS = 0x100-KS&0xFF;
if(resp.charCodeAt(resp.length-1) != KS )	{ io.setAttr("err","3:"+tr("CRC error")); return; }
io.setText(resp.slice(1,resp.length-1));','','',1509285266);
INSERT INTO "UserProtocol_uPrt" VALUES('OPTRIS','OPTRIS CT/CTL','','','Protocol level of OPTRIS CT/CTL communication interface of "Optris" GmbH, from Berlin.
Author: Roman Savochenko <rom_as@oscada.org>
Sponsored: Vasiliy Grigoriev from "Vacuum technologies laboratory (http://e-beam.ru)".
Version: 1.0.0','','',1,0,0,'','','','JavaLikeCalc.JavaScript
//Request form:
//<mess addr="1" cs="1" err="1:Error">{req}</mess> - message tag
//  req - request/respond data;
//  addr - remote station address (0...15);
//  cs - use control sum for SET commands [0,1]
//  err - sets for the request result.
io.setAttr("err", "");
addr = io.attr("addr").toInt();
if(addr < 0 || addr > 15) { io.setAttr("err", "1:"+tr("Device address out of range 0...15")); return; }
request = SYS.strFromCharCode(0xB0+addr) + io.text();
if(io.attr("cs").toInt()) {
	for(CS = 0, i = 0; i < request.length; i++) CS += request.charCodeAt(i);
	request += SYS.strFromCharCode(CS);
}
SYS.messDebug("/OPTRIS/PRT",tr("Request")+": "+SYS.strDecode(request,"Bin"," "));

//Send request
resp = tr.messIO(request);
while(resp.length && (tresp=tr.messIO("")).length) resp += tresp;
SYS.messDebug("/OPTRIS/PRT","Respond: "+SYS.strDecode(resp,"Bin"," "));
if(resp.length <= 0) { io.setAttr("err","3:"+tr("No a respond")); return; }
io.setText(resp);','','',1509286719);
INSERT INTO "UserProtocol_uPrt" VALUES('CTR','CTR 100, 101','CTR 100, 101','','Protocol part of the RS232C Serial Interface permits the communication between the digital Oerlikon Leybold Vacuum CERAVAC, from Köln.
Author: Roman Savochenko <rom_as@oscada.org>
Sponsored: Vasiliy Grigoriev from "Vacuum technologies laboratory (http://e-beam.ru)".
Version: 1.0.0','Protocol part of the RS232C Serial Interface permits the communication between the digital Oerlikon Leybold Vacuum CERAVAC, from Köln.
Author: Roman Savochenko <rom_as@oscada.org>
Sponsored: Vasiliy Grigoriev from "Vacuum technologies laboratory (http://e-beam.ru)".','',1,0,0,'','','','JavaLikeCalc.JavaScript
//Request form:
//<mess err="1:Error">{req}</mess> - message tag
//  req - request/respond data;
//  err - sets for the request result.
io.setAttr("err", "");
//Request prepare
req = SYS.strFromCharCode(3) + io.text();
for(CS = 0, i = 1; i < req.length; i++)	CS += req.charCodeAt(i);
req += SYS.strFromCharCode(CS&0xFF);
SYS.messDebug("/CTR/PRT",tr("Request")+": "+SYS.strDecode(req,"Bin"," "));

//Send request
for(resp = tr.messIO(req); resp.length < 9 && (tresp=tr.messIO("")).length; resp += tresp) ;
if(resp.length == 0){ io.setAttr("err","2:"+tr("No a respond")); return; }
SYS.messDebug("/CTR/PRT","Respond: "+SYS.strDecode(resp,"Bin"," "));
if(resp.length < 9)	{ io.setAttr("err","3:"+tr("Respond too short")); return; }
if(resp.length > 9)	{ io.setAttr("err","4:"+tr("Respond too long, possible continuous mode")); return; }
for(CS = 0, i = 1; i < resp.length-1; i++)	CS += resp.charCodeAt(i);
if(resp.charCodeAt(resp.length-1) != (CS&0xFF))	{ io.setAttr("err","5:"+tr("CRC error")); return; }
io.setText(resp.slice(1,resp.length-1));','','',1509287419);
INSERT INTO "UserProtocol_uPrt" VALUES('SMTP','SMTP','SMTP','','Provides EMAIL sending by the protocol SMTP.
Author: Roman Savochenko <rom_as@oscada.org>
Version: 1.0.0','Provides EMAIL sending by the protocol SMTP.
Author: Roman Savochenko <rom_as@oscada.org>
Version: 1.1.0','',1,0,0,'','','','JavaLikeCalc.JavaScript
//Request form:
//<send auth="{user}:{pass}" from="{FromAddr}" to="{ToAddr}" topic="{My Topic}" err="1:Error">{MessageText}</send>
//  topic - the message topic;
//  from - sender address;
//  to - destination address of the receiver;
//  auth - authenticate user and password;
//  MessageText - the message text;
//  err - sets for the request result.

//Transport''s timings check
if(tr.timings() != "5:1")	tr.timings("5:1");
if(!tr.start(true))	{ io.setAttr("err", "10:"+tr("Impossible connect to the SMTP-server.")); return; }

//Wait for a greeting after the connection
rez = "";
for(iTr = 0; iTr < 5 && !rez.length; iTr++) rez += tr.messIO("");
while(rez.length && rez.slice(-2) != "\x0D\x0A" && (trez=tr.messIO("")).length) rez += trez;

auth = io.attr("auth");

//Send HELLO
rez = tr.messIO("EHLO "+SYS.system("hostname -f").parse(0,"\n")+"\x0D\x0A");
while(rez.length && rez.slice(-2) != "\x0D\x0A" && (trez=tr.messIO("")).length) rez += trez;
if(!rez.length)	{ io.setAttr("err", "100:"+tr("No a response.")); return; }
for(off = 0; (sit=rez.parse(0,"\x0D\x0A",off)).length; )
	if(sit.toInt() != 250)	{ io.setAttr("err", "101:"+tr("HELLO error: %1.").replace("%1",sit)); return; }
	else if(sit.indexOf("AUTH LOGIN") >= 0) {
		if(sit.indexOf("PLAIN") < 0)	{ io.setAttr("err", "101:"+tr("Unsupported auth methods: %1.").replace("%1",sit)); return; }
		else if(!auth.length)				{ io.setAttr("err", "101:"+tr("Auth required: %1.").replace("%1",sit)); return; }
	}

//Send AUTH
if(auth.length) {
	rez = tr.messIO("AUTH LOGIN\x0D\x0A");
	while(rez.length && rez.slice(-2) != "\x0D\x0A" && (trez=tr.messIO("")).length) rez += trez;
	if(!rez.length)	{ io.setAttr("err", "100:"+tr("No a response.")); return; }
	if(rez.toInt() != 334)	{ io.setAttr("err", "102:"+tr("AUTH error: %1.").replace("%1",rez)); return; }

	rez = tr.messIO(SYS.strEncode(io.attr("auth").parse(0,":"),"Base64")+"\x0D\x0A");
	while(rez.length && rez.slice(-2) != "\x0D\x0A" && (trez=tr.messIO("")).length) rez += trez;
	if(!rez.length)	{ io.setAttr("err", "100:"+tr("No a response.")); return; }
	if(rez.toInt() != 334)	{ io.setAttr("err", "102:"+tr("AUTH USER error: %1.").replace("%1",rez)); return; }

	rez = tr.messIO(SYS.strEncode(io.attr("auth").parse(1,":"),"Base64")+"\x0D\x0A");
	while(rez.length && rez.slice(-2) != "\x0D\x0A" && (trez=tr.messIO("")).length) rez += trez;
	if(!rez.length)	{ io.setAttr("err", "100:"+tr("No a response.")); return; }
	if(rez.toInt() != 235)	{ io.setAttr("err", "102:"+tr("AUTH PASS error: %1.").replace("%1",rez)); return; }
}

//Send FROM
rez = tr.messIO("MAIL FROM:<"+io.attr("from")+">\x0D\x0A");
while(rez.length && rez.slice(-2) != "\x0D\x0A" && (trez=tr.messIO("")).length) rez += trez;
if(!rez.length)	{ io.setAttr("err", "100:"+tr("No a response.")); return; }
if(rez.toInt() != 250)	{ io.setAttr("err", "103:"+tr("MAIL FROM error: %1.").replace("%1",rez)); return; }

//Send TO
rez = tr.messIO("RCPT TO:<"+io.attr("to")+">\x0D\x0A");
while(rez.length && rez.slice(-2) != "\x0D\x0A" && (trez=tr.messIO("")).length) rez += trez;
if(!rez.length)	{ io.setAttr("err", "100:"+tr("No a response.")); return; }
if(rez.toInt() != 250)	{ io.setAttr("err", "104:"+tr("RCPT TO error: %1.").replace("%1",rez)); return; }

//Prepare and send DATA
rez = tr.messIO("DATA\x0D\x0A");
while(rez.length && rez.slice(-2) != "\x0D\x0A" && (trez=tr.messIO("")).length) rez += trez;
if(!rez.length)	{ io.setAttr("err", "100:"+tr("No a response.")); return; }
if(rez.toInt() != 354)	{ io.setAttr("err", "105:"+tr("DATA error: %1.").replace("%1",rez)); return; }

// Req version
reqVer = SYS.XMLNode("get").setAttr("path","/%2fgen%2fver"); SYS.cntrReq(reqVer);

data = "Message-ID: <"+rand(999999).toString()+"."+rand(999999999).toString()+"-OpenSCADA@"+SYS.system("hostname").parse(0,"\n")+">\x0D\x0A"+
			"From: \""+io.attr("from")+"\" <"+io.attr("from")+">\x0D\x0A"+
			"To: \""+io.attr("to")+"\" <"+io.attr("to")+">\x0D\x0A"+
			"Subject: =?utf8?B?"+SYS.strEncode(io.attr("topic"),"Base64")+"?=\x0D\x0A"+
			"Date: "+SYS.system("LANG=en date \"+%a, %d %b %Y %T %z\"").parse(0,"\n")+"\x0D\x0A"+
			//"Date: "+SYS.strftime(SYS.time(),"%a, %d %b %Y %T %z")+"\x0D\x0A"+
			"X-Mailer: OpenSCADA "+reqVer.text()+"\x0D\x0A"+
			"Content-type: text/plain;charset=utf-8\x0D\x0A\x0D\x0A"+
			io.text()+"\x0D\x0A"+
			"\x0D\x0A.\x0D\x0A";	//End for code 354
rez = tr.messIO(data);
while(rez.length && rez.slice(-2) != "\x0D\x0A" && (trez=tr.messIO("")).length) rez += trez;
if(!rez.length)	{ io.setAttr("err", "100:"+tr("No a response.")); return; }
if(rez.toInt() != 250)	{ io.setAttr("err", "105:"+tr("DATA send error: %1.").replace("%1",rez)); return; }

//Send QUIT
rez = tr.messIO("QUIT\x0D\x0A");
while(rez.length && rez.slice(-2) != "\x0D\x0A" && (trez=tr.messIO("")).length) rez += trez;

//SYS.messInfo("SMTP", "Response: "+rez);

//Disconnect from the SMTP-server
tr.start(false);','','',1477849632);
INSERT INTO "UserProtocol_uPrt" VALUES('AT24CXX','I2C: AT24C{32|64}','','','Provides operations with EEPROM memory based on I2C chips AT24C32 (4KB) and AT24C64 (8KB). Supported random reading and writing.
Author: Roman Savochenko <rom_as@oscada.org>
Version: 1.0.0','','',1,0,0,'','','','JavaLikeCalc.JavaScript
//Request form:
//<{cmd} addr="{ChipAddr}" off="{MemOffset}" size="{ReadSize}" err="1:Error">{ReadWriteSeq}</{cmd}>
//  cmd - command, for now there allowed: "read", "write";
//  addr - I2C device address [0...119];
//  off - memory part offset;
//  size - readed memory block size;
//  ReadWriteSeq - Read/Write bytes sequence;
//  err - sets for the request result.

//Transport''s timings check
if(tr.timings() != "100:0.01")	tr.timings("100:0.01");
addr = io.attr("addr").toInt();
if(addr < 0 || addr > 119) { io.setAttr("err", "1:"+tr("Device address out of range 0...119.")); return; }
off = io.attr("off").toInt();
if(off < 0 || off >= 8192) { io.setAttr("err", "1:"+tr("Memory offset out of range 0...8191.")); return; }

io.setAttr("err", "");

if(io.name() == "read") {
	size = io.attr("size").toInt();
	if(size < 0 || size >= 8192) { io.setAttr("err", "2:"+tr("Readed memory size out of range 0...8191.")); return; }
	resp = tr.messIO(SYS.strFromCharCode(addr,off>>8,off&0xFF), 0, size);
	if(resp.length < size)	{ io.setAttr("err", "3:"+tr("Not or short respond %1 from %2.").replace("%1",resp.length.toString()).replace("%2",size.toString())); return; }
	io.setText(resp);
	return;
}
if(io.name() == "write") {
	if(io.text().length >= 8192) { io.setAttr("err", "2:"+tr("Too long write sequence.")); return; }	
	tr.messIO(SYS.strFromCharCode(addr,off>>8,off&0xFF)+io.text(), 0, 0);
	return;
}','','',1479654824);
INSERT INTO "UserProtocol_uPrt" VALUES('m200','Mercury 200','','Меркурий 200','Protocol level of one phase counter of electricity Mercury 200, 203.2Т, 206 from firm Incotex (http://www.incotexcom.ru).
Author: Arsen Zakojan <godzilla919@gmail.com>
Version: 1.0.0','','Протокольный уровень однофазных счетчиков электроэнергии Меркурий 200,  203.2Т,  206 компании Инкотекс (http://www.incotexcom.ru).
Автор: Арсен Закоян <godzilla919@gmail.com>
Версия 1.0',1,1,0,'','','','JavaLikeCalc.JavaScript
//resp="";
//преобразуем сетевой адрес для запроса
net0 ="0x"+io.attr("first").toInt(10).toString(16,2);
net1 ="0x"+io.attr("second").toInt(10).toString(16,6).slice(0,2);
net2 ="0x"+io.attr("second").toInt(10).toString(16,6).slice(2,4);
net3 ="0x"+io.attr("second").toInt(10).toString(16,6).slice(4,6);
request = SYS.strFromCharCode(Special.FLibSYS.str2int(net0),Special.FLibSYS.str2int(net1),Special.FLibSYS.str2int(net2),Special.FLibSYS.str2int(net3))+io.attr("data");
//Вычисление CRC запроса
out = 0xFFFF;
for( i = 0; i < request.length; i++ )
{
out = out^request.charCodeAt(i);
for( j=0; j<8; j++ ) out = (out&1) ? ((out>>1)^0xA001) : out >> 1;
}
CRC=SYS.strFromCharCode(out&0xFF,(out&0xFF00)>>8 );
//запрос
resp1 = tr.messIO(request+CRC);
resp2 = tr.messIO("");
resp3 = tr.messIO("");
resp=(resp1+resp2+resp3);
//Что есть ответ и что есть контрольная сумма
answer=resp.slice(0,resp.length-2);
CRCans=resp.slice(resp.length-2,resp.length);
// Вычисление CRC ответа
out1 = 0xFFFF;
for( i1 = 0; i1 < answer.length; i1++ )
{
out1 = out1^answer.charCodeAt(i1);
for( j1=0; j1<8; j1++ ) out1 = (out1&1) ? ((out1>>1)^0xA001) : out1 >> 1;
}
CRC1=SYS.strFromCharCode(out1&0xFF,(out1&0xFF00)>>8 );
//Проверяем контрольную сумму  и принимаем ответ
if((CRCans==CRC1)&&(request.charCodeAt(0)==answer.charCodeAt(0))&&(request.charCodeAt(1)==answer.charCodeAt(1))&&(request.charCodeAt(2)==answer.charCodeAt(2))&&(request.charCodeAt(3)==answer.charCodeAt(3)))//break;
//k++;}
io.setText(Special.FLibSYS.strDec4Bin(resp));
','','JavaLikeCalc.JavaScript
//resp="";
//преобразуем сетевой адрес для запроса
net0 ="0x"+io.attr("first").toInt(10).toString(16,2);
net1 ="0x"+io.attr("second").toInt(10).toString(16,6).slice(0,2);
net2 ="0x"+io.attr("second").toInt(10).toString(16,6).slice(2,4);
net3 ="0x"+io.attr("second").toInt(10).toString(16,6).slice(4,6);
request = SYS.strFromCharCode(Special.FLibSYS.str2int(net0),Special.FLibSYS.str2int(net1),Special.FLibSYS.str2int(net2),Special.FLibSYS.str2int(net3))+io.attr("data");
//Вычисление CRC запроса
out = 0xFFFF;
for( i = 0; i < request.length; i++ )
{
out = out^request.charCodeAt(i);
for( j=0; j<8; j++ ) out = (out&1) ? ((out>>1)^0xA001) : out >> 1;
}
CRC=SYS.strFromCharCode(out&0xFF,(out&0xFF00)>>8 );
//запрос
resp1 = tr.messIO(request+CRC);
resp2 = tr.messIO("");
resp3 = tr.messIO("");
resp=(resp1+resp2+resp3);
//Что есть ответ и что есть контрольная сумма
answer=resp.slice(0,resp.length-2);
CRCans=resp.slice(resp.length-2,resp.length);
// Вычисление CRC ответа
out1 = 0xFFFF;
for( i1 = 0; i1 < answer.length; i1++ )
{
out1 = out1^answer.charCodeAt(i1);
for( j1=0; j1<8; j1++ ) out1 = (out1&1) ? ((out1>>1)^0xA001) : out1 >> 1;
}
CRC1=SYS.strFromCharCode(out1&0xFF,(out1&0xFF00)>>8 );
//Проверяем контрольную сумму  и принимаем ответ
if((CRCans==CRC1)&&(request.charCodeAt(0)==answer.charCodeAt(0))&&(request.charCodeAt(1)==answer.charCodeAt(1))&&(request.charCodeAt(2)==answer.charCodeAt(2))&&(request.charCodeAt(3)==answer.charCodeAt(3)))//break;
//k++;}
io.setText(Special.FLibSYS.strDec4Bin(resp));
',1509290174);
INSERT INTO "UserProtocol_uPrt" VALUES('m230','Mercury 230','','Меркурий 230','Protocol level of three phase counter of electricity Mercury 230, 231, 232, 233, 234, 236 from firm Incotex (http://www.incotexcom.ru).
Author: Arsen Zakojan <godzilla919@gmail.com>
Version: 1.0.0','','Протокольный уровень трехфазных счетчиков электроэнергии Меркурий 230, 231, 232, 233,  234,  236  компании Инкотекс (http://www.incotexcom.ru).
Автор: Арсен Закоян <godzilla919@gmail.com>
Версия 1.0',1,1,0,'','','','JavaLikeCalc.JavaScript
//преобрауем сетевой адрес,преобразуем из строки в целое
netaddr1 ="0x"+io.attr("netaddr").toInt(10).toString(16);
request = SYS.strFromCharCode(Special.FLibSYS.str2int(netaddr1))+io.attr("data");
//Вычисление CRC запроса
out = 0xFFFF;
for( i = 0; i < request.length; i++ )
{
out = out^request.charCodeAt(i);
for( j=0; j<8; j++ ) out = (out&1) ? ((out>>1)^0xA001) : out >> 1;
}
CRC=SYS.strFromCharCode(out&0xFF,(out&0xFF00)>>8 );
//Отправляем запрос
//resp = tr.messIO(request+CRC);
resp1 = tr.messIO(request+CRC);
resp2 = tr.messIO("");
resp3 = tr.messIO("");
resp=(resp1+resp2+resp3);
//Что есть ответ и что есть контрольная сумма
answer=resp.slice(0,resp.length-2);
CRCans=resp.slice(resp.length-2,resp.length);
// Вычисление CRC ответа
out1 = 0xFFFF;
for( i1 = 0; i1 < answer.length; i1++ )
{
out1 = out1^answer.charCodeAt(i1);
for( j1=0; j1<8; j1++ ) out1 = (out1&1) ? ((out1>>1)^0xA001) : out1 >> 1;
}
CRC1=SYS.strFromCharCode(out1&0xFF,(out1&0xFF00)>>8 );
//Последовательность байт для интерпретации ответа
b1 =Special.FLibSYS.strDec4Bin(resp.slice(1,2));
b2 =Special.FLibSYS.strDec4Bin(resp.slice(2,3));
b3 =Special.FLibSYS.strDec4Bin(resp.slice(3,4));
b4 =Special.FLibSYS.strDec4Bin(resp.slice(4,5));
b5 =Special.FLibSYS.strDec4Bin(resp.slice(5,6));
b6 =Special.FLibSYS.strDec4Bin(resp.slice(6,7));
b7 =Special.FLibSYS.strDec4Bin(resp.slice(7,8));
b8 =Special.FLibSYS.strDec4Bin(resp.slice(8,9));
b9 =Special.FLibSYS.strDec4Bin(resp.slice(9,10));
b10 =Special.FLibSYS.strDec4Bin(resp.slice(10,11));
b11 =Special.FLibSYS.strDec4Bin(resp.slice(11,12));
b12 =Special.FLibSYS.strDec4Bin(resp.slice(12,13));
b13 =Special.FLibSYS.strDec4Bin(resp.slice(13,14));
b14 =Special.FLibSYS.strDec4Bin(resp.slice(14,15));
b15 =Special.FLibSYS.strDec4Bin(resp.slice(15,16));
b16 =Special.FLibSYS.strDec4Bin(resp.slice(16,17));
//если контрольная сумма ответа совпадает и адрес ответа равен адресу запроса 
if(CRCans==CRC1&&(request.charCodeAt(0)==answer.charCodeAt(0))/*&&answer.length>1*/ )
//отклик от счетчика
{
if(request.charCodeAt(1)==0x01)
 io.setText(Special.FLibSYS.strDec4Bin(resp));
//чтение энергии
else if((request.charCodeAt(1)==0x05)&&(answer.length==17))
{R1=b2+b1+b4+b3;R2=b6+b5+b8+b7;R3=b10+b9+b12+b11;R4=b14+b13+b16+b15;
 io.setText(R1.toInt(16).toString(10,10,)+R2.toInt(16).toString(10,10,)+R3.toInt(16).toString(10,10,)+R4.toInt(16).toString(10,10,));}

//чтение напряжения U разом
else if((request.charCodeAt(1)==0x08)&&(request.charCodeAt(2)==0x16)&&(request.charCodeAt(3)==0x11)&&(answer.length==10)) 
{U1=b1+b3+b2;U2=b4+b6+b5;U3=b7+b9+b8;
io.setText(U1.toInt(16).toString(10,8,)+U2.toInt(16).toString(10,8,)+U3.toInt(16).toString(10,8,));}
//чтение Мощности P Q S разом
else if((request.charCodeAt(1)==0x08)&&(request.charCodeAt(2)==0x16)&&(request.charCodeAt(3)==0x00||request.charCodeAt(3)==0x04||request.charCodeAt(3)==0x08)&&(answer.length==13)) 
{PS=b1.slice(1,2)+b3+b2;P1=b4.slice(1,2)+b6+b5;P2=b7.slice(1,2)+b9+b8;P3=b10.slice(1,2)+b12+b11;
io.setText(PS.toInt(16).toString(10,7,)+P1.toInt(16).toString(10,7,)+P2.toInt(16).toString(10,7,)+P3.toInt(16).toString(10,7,)+b1.slice(0,1)+b4.slice(0,1)+b7.slice(0,1)+b10.slice(0,1));}

//чтение тока I разом
else if((request.charCodeAt(1)==0x08)&&(request.charCodeAt(2)==0x16)&&(request.charCodeAt(3)==0x21)&&(answer.length==10)) 
{I1=b1+b3+b2;I2=b4+b6+b5;I3=b7+b9+b8;
io.setText(I1.toInt(16).toString(10,8,)+I2.toInt(16).toString(10,8,)+I3.toInt(16).toString(10,8,));}
//чтение коэф.мощности
else if((request.charCodeAt(1)==0x08)&&(request.charCodeAt(2)==0x16)&&(request.charCodeAt(3)==0x30)&&(answer.length==13)) 
{KS=b1.slice(1,2)+b3+b2;K1=b4.slice(1,2)+b6+b5;K2=b7.slice(1,2)+b9+b8;K3=b10.slice(1,2)+b12+b11;
io.setText(KS.toInt(16).toString(10,7,)+K1.toInt(16).toString(10,7,)+K2.toInt(16).toString(10,7,)+K3.toInt(16).toString(10,7,));}
//чтение частоты F
else if((request.charCodeAt(1)==0x08)&&(request.charCodeAt(2)==0x16)&&(request.charCodeAt(3)==0x40)&&(answer.length==4))
{resp=b1+b3+b2;
 io.setText(resp.toInt(16).toString(10,,)/100);}
//чтение серийного номера
else if((request.charCodeAt(1)==0x08)&&(request.charCodeAt(2)==0x00)&&(answer.length==8))
 io.setText(b1.toInt(16).toString(10,2,)+b2.toInt(16).toString(10,2,)+b3.toInt(16).toString(10,2,)+b4.toInt(16).toString(10,2,));
;}','','JavaLikeCalc.JavaScript
//преобрауем сетевой адрес,преобразуем из строки в целое
netaddr1 ="0x"+io.attr("netaddr").toInt(10).toString(16);
request = SYS.strFromCharCode(Special.FLibSYS.str2int(netaddr1))+io.attr("data");
//Вычисление CRC запроса
out = 0xFFFF;
for( i = 0; i < request.length; i++ )
{
out = out^request.charCodeAt(i);
for( j=0; j<8; j++ ) out = (out&1) ? ((out>>1)^0xA001) : out >> 1;
}
CRC=SYS.strFromCharCode(out&0xFF,(out&0xFF00)>>8 );
//Отправляем запрос
//resp = tr.messIO(request+CRC);
resp1 = tr.messIO(request+CRC);
resp2 = tr.messIO("");
resp3 = tr.messIO("");
resp=(resp1+resp2+resp3);
//Что есть ответ и что есть контрольная сумма
answer=resp.slice(0,resp.length-2);
CRCans=resp.slice(resp.length-2,resp.length);
// Вычисление CRC ответа
out1 = 0xFFFF;
for( i1 = 0; i1 < answer.length; i1++ )
{
out1 = out1^answer.charCodeAt(i1);
for( j1=0; j1<8; j1++ ) out1 = (out1&1) ? ((out1>>1)^0xA001) : out1 >> 1;
}
CRC1=SYS.strFromCharCode(out1&0xFF,(out1&0xFF00)>>8 );
//Последовательность байт для интерпретации ответа
b1 =Special.FLibSYS.strDec4Bin(resp.slice(1,2));
b2 =Special.FLibSYS.strDec4Bin(resp.slice(2,3));
b3 =Special.FLibSYS.strDec4Bin(resp.slice(3,4));
b4 =Special.FLibSYS.strDec4Bin(resp.slice(4,5));
b5 =Special.FLibSYS.strDec4Bin(resp.slice(5,6));
b6 =Special.FLibSYS.strDec4Bin(resp.slice(6,7));
b7 =Special.FLibSYS.strDec4Bin(resp.slice(7,8));
b8 =Special.FLibSYS.strDec4Bin(resp.slice(8,9));
b9 =Special.FLibSYS.strDec4Bin(resp.slice(9,10));
b10 =Special.FLibSYS.strDec4Bin(resp.slice(10,11));
b11 =Special.FLibSYS.strDec4Bin(resp.slice(11,12));
b12 =Special.FLibSYS.strDec4Bin(resp.slice(12,13));
b13 =Special.FLibSYS.strDec4Bin(resp.slice(13,14));
b14 =Special.FLibSYS.strDec4Bin(resp.slice(14,15));
b15 =Special.FLibSYS.strDec4Bin(resp.slice(15,16));
b16 =Special.FLibSYS.strDec4Bin(resp.slice(16,17));
//если контрольная сумма ответа совпадает и адрес ответа равен адресу запроса 
if(CRCans==CRC1&&(request.charCodeAt(0)==answer.charCodeAt(0))/*&&answer.length>1*/ )
//отклик от счетчика
{
if(request.charCodeAt(1)==0x01)
 io.setText(Special.FLibSYS.strDec4Bin(resp));
//чтение энергии
else if((request.charCodeAt(1)==0x05)&&(answer.length==17))
{R1=b2+b1+b4+b3;R2=b6+b5+b8+b7;R3=b10+b9+b12+b11;R4=b14+b13+b16+b15;
 io.setText(R1.toInt(16).toString(10,10,)+R2.toInt(16).toString(10,10,)+R3.toInt(16).toString(10,10,)+R4.toInt(16).toString(10,10,));}

//чтение напряжения U разом
else if((request.charCodeAt(1)==0x08)&&(request.charCodeAt(2)==0x16)&&(request.charCodeAt(3)==0x11)&&(answer.length==10)) 
{U1=b1+b3+b2;U2=b4+b6+b5;U3=b7+b9+b8;
io.setText(U1.toInt(16).toString(10,8,)+U2.toInt(16).toString(10,8,)+U3.toInt(16).toString(10,8,));}
//чтение Мощности P Q S разом
else if((request.charCodeAt(1)==0x08)&&(request.charCodeAt(2)==0x16)&&(request.charCodeAt(3)==0x00||request.charCodeAt(3)==0x04||request.charCodeAt(3)==0x08)&&(answer.length==13)) 
{PS=b1.slice(1,2)+b3+b2;P1=b4.slice(1,2)+b6+b5;P2=b7.slice(1,2)+b9+b8;P3=b10.slice(1,2)+b12+b11;
io.setText(PS.toInt(16).toString(10,7,)+P1.toInt(16).toString(10,7,)+P2.toInt(16).toString(10,7,)+P3.toInt(16).toString(10,7,)+b1.slice(0,1)+b4.slice(0,1)+b7.slice(0,1)+b10.slice(0,1));}

//чтение тока I разом
else if((request.charCodeAt(1)==0x08)&&(request.charCodeAt(2)==0x16)&&(request.charCodeAt(3)==0x21)&&(answer.length==10)) 
{I1=b1+b3+b2;I2=b4+b6+b5;I3=b7+b9+b8;
io.setText(I1.toInt(16).toString(10,8,)+I2.toInt(16).toString(10,8,)+I3.toInt(16).toString(10,8,));}
//чтение коэф.мощности
else if((request.charCodeAt(1)==0x08)&&(request.charCodeAt(2)==0x16)&&(request.charCodeAt(3)==0x30)&&(answer.length==13)) 
{KS=b1.slice(1,2)+b3+b2;K1=b4.slice(1,2)+b6+b5;K2=b7.slice(1,2)+b9+b8;K3=b10.slice(1,2)+b12+b11;
io.setText(KS.toInt(16).toString(10,7,)+K1.toInt(16).toString(10,7,)+K2.toInt(16).toString(10,7,)+K3.toInt(16).toString(10,7,));}
//чтение частоты F
else if((request.charCodeAt(1)==0x08)&&(request.charCodeAt(2)==0x16)&&(request.charCodeAt(3)==0x40)&&(answer.length==4))
{resp=b1+b3+b2;
 io.setText(resp.toInt(16).toString(10,,)/100);}
//чтение серийного номера
else if((request.charCodeAt(1)==0x08)&&(request.charCodeAt(2)==0x00)&&(answer.length==8))
 io.setText(b1.toInt(16).toString(10,2,)+b2.toInt(16).toString(10,2,)+b3.toInt(16).toString(10,2,)+b4.toInt(16).toString(10,2,));
;}',1509290179);
CREATE TABLE 'flb_doc' ("ID" TEXT DEFAULT '' ,"NAME" TEXT DEFAULT '' ,"uk#NAME" TEXT DEFAULT '' ,"ru#NAME" TEXT DEFAULT '' ,"DESCR" TEXT DEFAULT '' ,"uk#DESCR" TEXT DEFAULT '' ,"ru#DESCR" TEXT DEFAULT '' ,"START" INTEGER DEFAULT '1' ,"MAXCALCTM" INTEGER DEFAULT '10' ,"PR_TR" INTEGER DEFAULT '0' ,"FORMULA" TEXT DEFAULT '' ,"uk#FORMULA" TEXT DEFAULT '' ,"ru#FORMULA" TEXT DEFAULT '' ,"TIMESTAMP" INTEGER DEFAULT '0' , PRIMARY KEY ("ID"));
INSERT INTO "flb_doc" VALUES('getVal','Getting value from archive','Запит значення архіву','Запрос знач. архива','Query the value for a specified time from the assigned archive and issuing the result with the specified number of decimal points.
Author: Roman Savochenko <rom_as@oscada.org>
Version: 1.1.0','Запит значення на визначений час з вказаного архіву та видача результату з вказаною кількістю знаків після точки.
Автор: Роман Савоченко <rom_as@oscada.org>
Версія: 1.1.0','Запрос значения на указанное время из установленного архива и выдача результата с указанным количеством знаков после точки.
Автор: Роман Савоченко <rom_as@oscada.org>
Версия: 1.1.0',1,10,0,'srcTime = time*1e6 + uTime;
aO = SYS.nodeAt(addr).arch();
if(aO) {
	if(!tryTo || tryTo == srcTime)	val = aO.getVal(srcTime, false, archtor);
	else {
		aPer = aO.period(archtor);
		if(srcTime < tryTo)
			for(srcTime1 = srcTime; srcTime < tryTo && (val=aO.getVal(srcTime1,false,archtor)).isEVal(); )
			{ srcTime += aPer; srcTime1 = srcTime; }
		else
			for(srcTime1 = srcTime; srcTime > tryTo && (val=aO.getVal(srcTime1,false,archtor)).isEVal(); )
			{ srcTime -= aPer; srcTime1 = srcTime; }
	}
	if(strong && (srcTime*1e-6) != time)	val = EVAL;
} else val = EVAL;
if(prec < 0)	return val;
if(val.isEVal())	return tr("Empty");
if(typeof(val) == "real")	return val.toFixed(prec);
return val;','','',1501145635);
INSERT INTO "flb_doc" VALUES('sumVal','Sum values of archive','Сума значень архіву','Сумма знач. архива','Calculating the sum of values ​​for a specified period of time an archive and issuing results with the specified number of decimal points.
Author: Roman Savochenko <rom_as@oscada.org>
Version: 1.1.0','Обчислення суми значень за проміжок часу визначеного архіву та видача результату з вказаною кількістю знаків після точки.
Автор: Роман Савоченко <rom_as@oscada.org>
Версія: 1.1.0','Вычисление суммы значений за промежуток времени указанного архива и выдача результата с указанным количеством знаков после точки.
Автор: Роман Савоченко <rom_as@oscada.org>
Версия: 1.1.0',1,10,0,'val = 0;
aO = SYS.nodeAt(addr).arch();
if(aO) {
	bTime = bTime*1e6; eTime = eTime*1e6;
	buf_beg = max(aO.begin(archtor), bTime); buf_end = min(aO.end(archtor), eTime);
	while(true) {
		r_val = aO.getVal(buf_beg, true, archtor);
		if(buf_beg > buf_end)	break;
		if(!r_val.isEVal())	val += r_val;
		if(buf_beg == 0 || buf_beg >= buf_end) break;
		buf_beg++;
	}
}
if(prec < 0)	return val;
if(val.isEVal())	return tr("Empty");
if(typeof(val) == "real")	return val.toFixed(prec);
return val;','','',1501231568);
INSERT INTO "flb_doc" VALUES('averVal','Average value of archive','Середнє значення архіву','Среднее знач. архива','Calculate the average value over a specified period of time an archive and issuing results with the specified number of decimal points.
Author: Roman Savochenko <rom_as@oscada.org>
Version: 1.1.0','Обчислення середнього значення за проміжок часу визначеного архіву та видача результату з вказаною кількістю знаків після точки.
Автор: Роман Савоченко <rom_as@oscada.org>
Версія: 1.1.0','Вычисление среднего значения за промежуток времени указанного архива и выдача результата с указанным количеством знаков после точки.
Автор: Роман Савоченко <rom_as@oscada.org>
Версия: 1.1.0',1,10,0,'val = EVAL;
actPerc = 0;
aO = SYS.nodeAt(addr).arch();
if(aO) {
	bTime = bTime*1e6; eTime = eTime*1e6;
	vCnt = (eTime-bTime)/aO.period(archtor)+1;
	buf_beg = max(aO.begin(archtor), bTime); buf_end = min(aO.end(archtor), eTime);
	for(vCntA = 0; true; ) {
  		buf_beg1 = buf_beg;
		r_val = aO.getVal(buf_beg, true, archtor);
		if(buf_beg > buf_end)	break;
		if(!r_val.isEVal())	vCntA++;
		if(val.isEVal())	val = r_val;
		else if(!r_val.isEVal())
			val = (val*(buf_beg-bTime) + r_val*(buf_beg-buf_beg1))/(buf_beg - bTime + buf_beg - buf_beg1);
		if(buf_beg == 0 || buf_beg >= buf_end) break;
		buf_beg++;
	}
	//SYS.messInfo("averVal", "vCntA="+vCntA+"("+vCnt+"); archtor="+archtor); 
	actPerc = vCntA ? min(100,100*vCntA/vCnt) : 0;
}
if(prec < 0)	return val;
if(val.isEVal())	return tr("Empty");
if(typeof(val) == "real")	return val.toFixed(prec);
return val;','','',1501228052);
CREATE TABLE 'tmplib_LowDevLib' ("ID" TEXT DEFAULT '' ,"NAME" TEXT DEFAULT '' ,"DESCR" TEXT DEFAULT '' ,"MAXCALCTM" INTEGER DEFAULT '10' ,"PR_TR" INTEGER DEFAULT '0' ,"PROGRAM" TEXT DEFAULT '' ,"TIMESTAMP" INTEGER DEFAULT '0' , PRIMARY KEY ("ID"));
INSERT INTO "tmplib_LowDevLib" VALUES('1602A','GPIO|I2C: 1602A(HD44780)','LCD Module 1602A, STN, BLUB, 16 Character x 2 Line,  5 x 8 Dots, by the direct (Raspberry PI BCM2835 GPIO) or I2C (PCF8574) wiring.
Conditions: Default planing policy but realtime one preferred.
Author: Roman Savochenko <rom_as@oscada.org>
Version: 1.1.1',10,0,'JavaLikeCalc.JavaScript
using Special.FLibSYS;

//Set link to fast external functions
if(f_start)	f_err = "0", transport_ = ln1_ = ln2_ = "";

t_err = "0";

//Check for the transport change and the link update
if(transport != transport_)	{
	transport_ = transport;
	if(!(tr=SYS.Transport.Serial.nodeAt("out_"+transport)))
		function put = transport+".fnc_put";
}

//Write a byte function declaration
function byte(tr, vl, md, dl) {
	if(md == EVAL_BOOL) md = false;
	if(dl == EVAL_BOOL) dl = 50e-6;
	if(tr) {
		//RS(md) | RW(false) | E(false) | BackLight(true) | D4 | D5 | D6 | D7
		com = (md?0x01:0) | 0x8 | (vl&0xF0);
		tr.messIO(SYS.strFromCharCode(addr,com), 0, 0);			tmSleep(dl);
		tr.messIO(SYS.strFromCharCode(addr,com|0x04), 0, 0);	tmSleep(dl);	//E toggle
		tr.messIO(SYS.strFromCharCode(addr,com), 0, 0);			tmSleep(dl);
		//RS(md) | RW(false) | E(false) | BackLight(true) | D0 | D1 | D2 | D3
		com = (md?0x01:0) | 0x8 | ((vl<<4)&0xF0);
		tr.messIO(SYS.strFromCharCode(addr,com), 0, 0);			tmSleep(dl);
		tr.messIO(SYS.strFromCharCode(addr,com|0x04), 0, 0);	tmSleep(dl);	//E toggle
		tr.messIO(SYS.strFromCharCode(addr,com), 0, 0);			tmSleep(dl);
	}
	else if(put) {
		put(RS, md);
		put(D4, vl&0x10); put(D5, vl&0x20); put(D6, vl&0x40); put(D7, vl&0x80);
		tmSleep(dl); put(E, true); tmSleep(dl); put(E, false); tmSleep(dl);
		put(D4, vl&0x01); put(D5, vl&0x02); put(D6, vl&0x04); put(D7, vl&0x08);
		tmSleep(dl); put(E, true); tmSleep(dl); put(E, false); tmSleep(dl);
	}
}

//Main processing
if(!tr && !put)
	t_err = "1:"+tr("Error for ''%1'' as an output transport of I2C or a link to external functions of GPIO.").replace("%1",transport);
else if(ln1 != ln1_ || ln2 != ln2_) {
	ln1_ = ln1; ln2_ = ln2;	

	//Init
	byte(tr, 0x33); byte(tr, 0x32); byte(tr, 0x28); byte(tr, 0x0C); byte(tr, 0x06); byte(tr, 0x01);
	//Line 1
	if(ln1.length) {
		tmSleep(2e-3);
		byte(tr, 0x80);
		for(iC = 0; iC < min(16,ln1.length); iC++)
			byte(tr, ln1.charCodeAt(iC), true);
	}
	//Line 2
	if(ln2.length) {
		tmSleep(2e-3);
		byte(tr, 0xC0);
		for(iC = 0; iC < min(16,ln2.length); iC++)
			byte(tr, ln2.charCodeAt(iC), true);
	}
}

f_err = t_err;',1509990639);
INSERT INTO "tmplib_LowDevLib" VALUES('1W_DS9097','One Wire by DS9097','One Wire sensors bus implementing by 1Wire-adapter DS9097. Supported direct and parasite powering for the temperature sensors.
Supported 1Wire-devices: DS1820, DS1820/DS18S20/DS1920 (not tested), DS1822 (not tested), DS2413, DS2408, DS2450, DS2438.
Author: Roman Savochenko <rom_as@oscada.org>
Version: 1.2.1',30,0,'JavaLikeCalc.JavaScript
//Functions
function reset(tr) {
	tr.addr(tr.addr().parse(0,":")+":9600:8N1"); tr.start(true);
	rez = tr.messIO(SYS.strFromCharCode(0xF0));
	tr.addr(tr.addr().parse(0,":")+":115200:8N1"); tr.timings("100:2-0"); tr.start(true);
	tr.messIO("");
	return rez.length && rez.charCodeAt(0) != 0xF0;
}

function io(tr, mess, bits) {
	if(bits == EVAL) bits = mess.length * 8;
	req = "";
	for(iB = 0; iB < ceil(bits/8); iB++) {
		vB = mess.charCodeAt(iB);
		bB = ((iB+1)*8 > bits) ? bits-floor(bits/8)*8 : 8;
		for(iBi = 0; iBi < bB; iBi++) req += SYS.strFromCharCode((vB&(1<<iBi)) ? 0xFF : 0x00);
	}
	//SYS.messInfo("OneWire","bits="+bits+"; req="+SYS.strDecode(req,"Bin"," "));
	for(resp = tr.messIO(req); resp.length < req.length && (tresp=tr.messIO("")).length; resp += tresp) ;
	//SYS.messInfo("OneWire","bits="+bits+"; resp="+SYS.strDecode(resp,"Bin"," "));
	rez = "";
	for(iR = 0; iR < resp.length; ) {
		vB = 0;
		for(ib = 0; ib < 8 && iR < resp.length; iR++, ib++)
			if(resp.charCodeAt(iR)&1) vB = vB | (1<<ib);
		rez += SYS.strFromCharCode(vB);
	}		
	return rez;
}

function scan(tr, sn, lstDiscr) {
	if(!reset(tr)) return "";
	io(tr, SYS.strFromCharCode(0xF0));
	for(nByte = 0, nBit = 1, tBit = 0, sDir = false, mskByte = 1, lstZero = 0; nByte < 8; ) {
		tBit = io(tr, SYS.strFromCharCode(0xFF), 1).charCodeAt(0) << 1;
		tBit = tBit | io(tr, SYS.strFromCharCode(0xFF), 1).charCodeAt(0);
		//SYS.messInfo("OneWire","nByte="+nByte+"; nBit="+nBit+"; tBit="+tBit);
		if(tBit == 3) break;
		if(tBit > 0) sDir = !(tBit&0x01);
		else {
			//SYS.messInfo("OneWire","nByte="+nByte+"; nBit="+nBit+"; lstDiscr="+lstDiscr);
			if(nBit < lstDiscr) sDir = sn[nByte]&mskByte;
			else sDir = (nBit == lstDiscr);
			if(!sDir) lstZero = nBit;
			//if(lstZero < 9) lstDiscr = lstZero;
			//SYS.messInfo("OneWire","nByte="+nByte+"; nBit="+nBit+"; sDir="+sDir+"; mskByte="+mskByte);
		}
		if(sDir) sn[nByte] = sn[nByte] | mskByte;
		else sn[nByte] = sn[nByte] & (~mskByte);
		tr.messIO(SYS.strFromCharCode(sDir?0xFF:0x00));
		nBit++;
     mskByte = (mskByte<<1)&0xFF;
		if(!mskByte) { nByte++; mskByte = 1; }
	}

	rez = "";
	if(nByte == 8) {
		for(iB = 0; iB < nByte; iB++) rez += SYS.strFromCharCode(sn[iB]);
		if(!Special.FLibSYS.CRC(rez,0x8C,8,0))	lstDiscr = lstZero;
		else { rez = ""; lstDiscr = 0; }
	}

	return rez;
}

//Set transport and init
if(f_start) {
	tmResc_ = tmResc;
	devLs = new Object();
	transport_ = transport;
	tr = SYS.Transport.Serial.nodeAt("out_"+transport);
}
if(f_stop) {
	for(var devID in devLs) {
		if((dO=devLs[devID]).isEVal()) continue;
		aLs = dO.dP.nodeList("a_");
		for(iA = 0; iA < aLs.length; iA++)
			if(!dO.dP[aLs[iA]].isCfg()) dO.dP[aLs[iA]].set(EVAL, 0, 0, true);
	}
	devLs = tr = false;
	return;
}

t_err = "0";

//Check for the transport change and connect
if(!tr || transport != transport_)	{
	tr = SYS.Transport.Serial.nodeAt("out_"+transport);
	transport_ = transport;
}
if(!tr)	t_err = "1:"+tr("Output transport ''%1'' error.").replace("%1",transport);
else {
	//Generic information update.
	if(f_start || (tmResc && (tmResc_=tmResc_-1/f_frq) <= 0)) {
		tmResc_ = tmResc;
		tmSc = SYS.time();
		// Check for power
		if(!reset(tr) ||
		 	io(tr,(req=SYS.strFromCharCode(0xCC))) != req ||
			io(tr,(req=SYS.strFromCharCode(0xB4))) != req ||
			!(resp=io(tr,SYS.strFromCharCode(1),1)).length) power = EVAL;
		else power = resp.charCodeAt(0);
		// Scan for allowed devices on the bus.
		sn = new Array(0, 0, 0, 0, 0, 0, 0, 0);
		for(devID = "start", lstDiscr = -1; devID.length && lstDiscr; ) {
			if(lstDiscr < 0) lstDiscr = 0;
			if((devID=scan(tr,sn,lstDiscr)).length)	{
				//SYS.messInfo("OneWire","devID="+SYS.strDecode(devID,"Bin"," "));
				if((dO=devLs[devID]).isEVal()) {
					devLs[devID] = dO = new Object();
					devFamily = devID.charCodeAt(0); devNm = tr("1W device"); dO.isT = false;
					if(devFamily == 0x10)			{ devNm = "DS1820/DS18S20/DS1920"; dO.isT = true; }
					else if(devFamily == 0x20)	devNm = "DS2450";		//4AI
					else if(devFamily == 0x22)	{ devNm = "DS1822"; dO.isT = true; }
					else if(devFamily == 0x26)	devNm = "DS2438";		//SmartBat
					else if(devFamily == 0x28)	{ devNm = "DS18B20"; dO.isT = true; }
					else if(devFamily == 0x29)	devNm = "DS2408";		//8DIO
					else if(devFamily == 0x3A)	devNm = "DS2413";		//2DIO

					// Create the device''s parameter object
					decId = SYS.strDecode(devID,"Bin","");
					SYS.cntrReq(SYS.XMLNode("add").setAttr("path",this.nodePath()+"/%2fbr%2fprm_").setAttr("id",decId).setText(devNm+": "+decId));
					SYS.cntrReq(SYS.XMLNode("set").setAttr("path",this.nodePath()+"/prm_"+decId+"/%2fprm%2fst%2fen").setText("1"));
					// Create the device''s proper attributes
					dO.dP = dP = this["prm_"+decId];
					if(dO.isT) {	//Temperature
						dP.attrAdd("val", tr("Temperature, °С"), "real|ro");
						dP.attrAdd("m", tr("User cell [0...65535]"), "integer");
						dP.attrAdd("res", tr("Resolution")+" [9...12]", "integer");
					}
					else if(devFamily == 0x20)		//DS2450
						for(iN = 0; iN < 4; iN++) {
							dP.attrAdd("ai"+iN, tr("AI, V")+iN, "real|ro");
							dP.attrAdd("res"+iN, tr("Resolution")+iN+" [1...16]", "integer");
							dP.attrAdd("md"+iN, tr("Mode")+iN, "integer|sel", "0;1;2\n2.55V;5.10V;DO");
							dP.attrAdd("do"+iN, tr("DO")+iN, "boolean");
						}
					else if(devFamily == 0x26) {	//DS2438
						dP.attrAdd("t", tr("Temperature, °С"), "real|ro");
						dP.attrAdd("v", tr("Voltage, V"), "real|ro");
						dP.attrAdd("i", tr("Current, A"), "real|ro");
						dP.attrAdd("ICA", tr("Current Accumulator"), "integer");
						dP.attrAdd("thr", tr("Accumulation threshold"), "integer|sel", "0;1;2;3\nNone;±2LSB;±4LSB;±8LSB");
						dP.attrAdd("ETM", tr("Elapsed time meter"), "integer");
						dP.attrAdd("TM_DISC", tr("Disconnect time"), "integer");
						dP.attrAdd("TM_END", tr("End of charge time"), "integer");
						dP.attrAdd("OFF", tr("Current Offset"), "integer");
						dP.attrAdd("IAD", tr("Enable Current A/D"), "boolean");
						dP.attrAdd("CA", tr("Enable Current Accumulator"), "boolean");
						dP.attrAdd("EE", tr("Enable Current Accumulator Shadow"), "boolean");
						dP.attrAdd("AD", tr("Voltage A/D (0-VAD,1-VDD)"), "boolean");
					}
					else if(devFamily == 0x29)		//DS2408
						for(iD = 0; iD < 8; iD++) {
							dP.attrAdd("di"+iD, tr("DI")+iD, "boolean|ro");
							dP.attrAdd("do"+iD, tr("DO")+iD, "boolean");
						}
					else if(devFamily == 0x3A)		//DS2413
						for(iD = 0; iD < 2; iD++) {
							dP.attrAdd("di"+iD, tr("DI")+iD, "boolean|ro");
							dP.attrAdd("do"+iD, tr("DO")+iD, "boolean");
						}
				}
				if(dO.tmSc == tmSc)	break;	//Somthing wrong into the scan but repeat, interruption
				dO.tmSc = tmSc;
				dO.try = tryEVAL;
			}
		}
		//Check for removed devices
		for(devID in devLs) {
			if((dO=devLs[devID]).isEVal()) continue;
			if(dO.tmSc != tmSc)	{
				devLs[devID] = EVAL;
				aLs = dO.dP.nodeList("a_");
				for(iA = 0; iA < aLs.length; iA++)
					if(!dO.dP[aLs[iA]].isCfg())	dO.dP[aLs[iA]].set(EVAL, 0, 0, true);
			}
		}
	}
	else {
		//Process devices for reading current value and set modifable
		for(devID in devLs) {
			if((dO=devLs[devID]).isEVal()) continue;
			devFamily = devID.charCodeAt(0);
			if(dO.isT) {	//Temperature
				dP = dO.dP;
				//SYS.messInfo("OneWire","devID="+SYS.strDecode(devID,"Bin"," "));
				//Check for a modifications
				isMdf = false;
				m_ = dP.m.get(), res_ = max(9,min(12,dP.res.get()));
				if(!dO.m.isEVal() && !dO.res.isEVal() && (m_ != dO.m || res_ != dO.res)) {
					//Set scratchpad
					if(!reset(tr)) continue;
					if(io(tr,(req=SYS.strFromCharCode(0x55)+devID)) != req) continue;
					if(io(tr,(req=SYS.strFromCharCode(0x4E,m_&0xFF,(m_>>8)&0xFF,((res_-9)<<5)|0x1F))) != req) continue;
					//Copy scratchpad part to the EEPROM
					if(!reset(tr)) continue;
					if(io(tr,(req=SYS.strFromCharCode(0x55)+devID)) != req) continue;
					if(io(tr,(req=SYS.strFromCharCode(0x48))) != req) continue;
					isMdf = true;
				}

				//Read current
				// Call the transmission
				if(!reset(tr)) continue;
				if(io(tr,(req=SYS.strFromCharCode(0x55)+devID)) != req) continue;
				if(io(tr,(req=SYS.strFromCharCode(0x44))) != req) continue;
				if(!power) SYS.sleep(0.094*pow(2,(dO.res.isEVal()?12:dO.res)-9));
				// Get scratchpad
				if(!reset(tr)) continue;
				if(io(tr,(req=SYS.strFromCharCode(0x55)+devID)) != req) continue;
				io(tr, SYS.strFromCharCode(0xBE));
				resp = io(tr, SYS.strFromCharCode(0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF));
				//SYS.messInfo("OneWire","resp="+SYS.strDecode(resp,"Bin"," "));
				if(!Special.FLibSYS.CRC(resp,0x8C,8,0)) {
					val = resp.charCodeAt(1)*256 + resp.charCodeAt(0);
					if(val > 32767) val -= 65536;
					dP.val.set(val/16, 0, 0, true);
					if(dO.m.isEVal() || dO.res.isEVal() || isMdf) {
						dO.m = resp.charCodeAt(3)*256 + resp.charCodeAt(2); dP.m.set(dO.m, 0, 0, true);
						dO.res = ((resp.charCodeAt(4)>>5)&0x3)+9; dP.res.set(dO.res, 0, 0, true);
					}
					dO.try = 0;
				}
				else if((dO.try=dO.try+1) > tryEVAL) {
					dP.val.set(EVAL, 0, 0, true); dP.m.set(EVAL, 0, 0, true); dP.res.set(EVAL, 0, 0, true);
					dO.m = dO.res = EVAL;
				}
			}
			else if(devFamily == 0x20) {	//DS2450
				dP = dO.dP;
				//Check for a modifications
				for(iN = 0; iN < 4; iN++) {
					if(dO["st1"+iN].isEVal())	continue;
					if((tRes=max(0,min(16,dP["res"+iN].get()))) == 16)	tRes = 0;
					tSt = (dO["st1"+iN]&(~0xCF)) + tRes + ((dP["md"+iN].get()==2)?0x80:0) + (dP["do"+iN].get()?0x40:0);
					if(tSt != dO["st1"+iN]) {	//Same write and check the result
						if(!reset(tr)) continue;
						if(io(tr,(req=SYS.strFromCharCode(0x55)+devID)) != req) continue;
						if(io(tr,(req=SYS.strFromCharCode(0x55,8+iN*2,0,tSt))) != req) continue;
						resp = io(tr, SYS.strFromCharCode(0xFF,0xFF,0xFF));
						if(((~((resp.charCodeAt(1)<<8) + resp.charCodeAt(0)))&0xFFFF) == Special.FLibSYS.CRC(req,0xA001,16,0) && resp.charCodeAt(2) == tSt)
							dO["st1"+iN] = tSt;
					}
					tSt = (dO["st2"+iN]&(~0x01)) + (dP["md"+iN].get()?1:0);
					if(tSt != dO["st2"+iN]) {	//Same write and check the result
						if(!reset(tr)) continue;
						if(io(tr,(req=SYS.strFromCharCode(0x55)+devID)) != req) continue;
						if(io(tr,(req=SYS.strFromCharCode(0x55,8+iN*2+1,0,tSt))) != req) continue;
						resp = io(tr, SYS.strFromCharCode(0xFF,0xFF,0xFF));
						if(((~((resp.charCodeAt(1)<<8) + resp.charCodeAt(0)))&0xFFFF) == Special.FLibSYS.CRC(req,0xA001,16,0) && resp.charCodeAt(2) == tSt)
							dO["st2"+iN] = tSt;
					}
				}

				//Read control status data
				if(dP["res0"].get().isEVal()) {
					if(!reset(tr)) continue;
					if(io(tr,(req=SYS.strFromCharCode(0x55)+devID)) != req) continue;
					if(io(tr,(req=SYS.strFromCharCode(0xAA,0x08,0x00))) != req) continue;
					resp = io(tr, SYS.strFromCharCode(0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF));
					CRC_OK = (~((resp.charCodeAt(resp.length-1)<<8) + resp.charCodeAt(resp.length-2)))&0xFFFF;
					CRC_OK = (Special.FLibSYS.CRC(SYS.strFromCharCode(0xAA,0x08,0x00)+resp.slice(0,-2),0xA001,16,0) == CRC_OK);
					for(iN = 0; iN < 4; iN++) {
						dP["res"+iN].set(CRC_OK?((tVl=resp.charCodeAt(iN*2)&0xF)?tVl:16):EVAL, 0, 0, true);
						dP["md"+iN].set(CRC_OK?((resp.charCodeAt(iN*2)&0x80)?2:resp.charCodeAt(iN*2+1)&1):EVAL, 0, 0, true);
						dP["do"+iN].set(CRC_OK?((resp.charCodeAt(iN*2)&0x40)?true:false):EVAL, 0, 0, true);
						dO["st1"+iN] = resp.charCodeAt(iN*2); dO["st2"+iN] = resp.charCodeAt(iN*2+1);
					}
				} else CRC_OK = true;

				//Read AI value
				if(CRC_OK) {
					if(!reset(tr)) continue;
					if(io(tr,(req=SYS.strFromCharCode(0x55)+devID)) != req) continue;
					if(io(tr,(req=SYS.strFromCharCode(0xAA,0x00,0x00))) != req) continue;
					resp = io(tr, SYS.strFromCharCode(0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF));
					//SYS.messInfo("OneWire","resp="+SYS.strDecode(resp,"Bin"," ")/*+"; CRC="+Special.FLibSYS.CRC(SYS.strFromCharCode(0x3C,0x08,0x00)/*+resp.slice(0,-2),0xA001,16,0).toString(16)*/);
					CRC_OK = (~((resp.charCodeAt(resp.length-1)<<8) + resp.charCodeAt(resp.length-2)))&0xFFFF;
					CRC_OK = (Special.FLibSYS.CRC(SYS.strFromCharCode(0xAA,0x00,0x00)+resp.slice(0,-2),0xA001,16,0) == CRC_OK);
				}
				for(iN = 0; iN < 4; iN++)
					if(CRC_OK) {
						tVl = (resp.charCodeAt(iN*2+1)<<8) + resp.charCodeAt(iN*2);
						vlMax = dP["md"+iN].get() ? 5.1 : 2.55;
						dP["ai"+iN].set(tVl*vlMax/65536, 0, 0, true);
					} else dP["ai"+iN].set(EVAL, 0, 0, true);

				//Start A/D conversion for prepare data for next iteration
				if(CRC_OK) {
					if(!reset(tr)) continue;
					if(io(tr,(req=SYS.strFromCharCode(0x55)+devID)) != req) continue;
					if(io(tr,(req=SYS.strFromCharCode(0x3C,0x0F,0x00))) != req) continue;
					resp = io(tr, SYS.strFromCharCode(0xFF,0xFF));
				}
			}
			else if(devFamily == 0x26) {	//DS2438
				dP = dO.dP;
				//Check for a modifications
				//????

				//Recall memory page 0
				if(!reset(tr)) continue;
				if(io(tr,(req=SYS.strFromCharCode(0x55)+devID)) != req) continue;
				if(io(tr,(req=SYS.strFromCharCode(0xB8,0))) != req) continue;
				//Read current state
				if(!reset(tr)) continue;
				if(io(tr,(req=SYS.strFromCharCode(0x55)+devID)) != req) continue;
				if(io(tr,(req=SYS.strFromCharCode(0xBE,0))) != req) continue;
				resp = io(tr, SYS.strFromCharCode(0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF));
				CRC_OK = !Special.FLibSYS.CRC(resp, 0x8C, 8, 0);
				if(CRC_OK) {
					if((tVl=(resp.charCodeAt(2)<<8)+resp.charCodeAt(1)) >= 32767) tVl -= 65536;
					dP["t"].set(tVl/256, 0, 0, true);
					if((tVl=(resp.charCodeAt(6)<<8)+resp.charCodeAt(5)) >= 32767) tVl -= 65536;
					dP["i"].set(tVl, 0, 0, true);
				}
				else {
					dP["t"].set(EVAL, 0, 0, true);
					dP["i"].set(EVAL, 0, 0, true);
				}
				dP["v"].set(CRC_OK?((resp.charCodeAt(4)<<8)+resp.charCodeAt(3))/100:EVAL, 0, 0, true);
				dP["IAD"].set(CRC_OK?(resp.charCodeAt(0)&0x01):EVAL, 0, 0, true);
				dP["CA"].set(CRC_OK?(resp.charCodeAt(0)&0x02):EVAL, 0, 0, true);
				dP["EE"].set(CRC_OK?(resp.charCodeAt(0)&0x04):EVAL, 0, 0, true);
				dP["AD"].set(CRC_OK?(resp.charCodeAt(0)&0x08):EVAL, 0, 0, true);
				dP["thr"].set(CRC_OK?(resp.charCodeAt(7)>>6):EVAL, 0, 0, true);

				if(CRC_OK) {
					if(!reset(tr)) continue;
					if(io(tr,(req=SYS.strFromCharCode(0x55)+devID)) != req) continue;
					if(io(tr,(req=SYS.strFromCharCode(0xBE,1))) != req) continue;
					resp = io(tr, SYS.strFromCharCode(0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF));
					CRC_OK = !Special.FLibSYS.CRC(resp, 0x8C, 8, 0);
					SYS.messInfo("OneWire","resp="+SYS.strDecode(resp,"Bin"," "));
				}
				dP["ETM"].set(CRC_OK?((resp.charCodeAt(3)<<24)+(resp.charCodeAt(2)<<16)+(resp.charCodeAt(1)<<8)+resp.charCodeAt(0)):EVAL, 0, 0, true);
				dP["ICA"].set(CRC_OK?resp.charCodeAt(4):EVAL, 0, 0, true);
				if(CRC_OK) {
					if((tVl=(resp.charCodeAt(6)<<8)+resp.charCodeAt(5)) >= 32767) tVl -= 65536;
					dP["OFF"].set(tVl, 0, 0, true);
				} else dP["OFF"].set(EVAL, 0, 0, true);

				if(CRC_OK) {
					if(!reset(tr)) continue;
					if(io(tr,(req=SYS.strFromCharCode(0x55)+devID)) != req) continue;
					if(io(tr,(req=SYS.strFromCharCode(0xBE,2))) != req) continue;
					resp = io(tr, SYS.strFromCharCode(0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF));
					CRC_OK = !Special.FLibSYS.CRC(resp, 0x8C, 8, 0);
					SYS.messInfo("OneWire","resp="+SYS.strDecode(resp,"Bin"," "));
				}
				dP["TM_DISC"].set(CRC_OK?((resp.charCodeAt(3)<<24)+(resp.charCodeAt(2)<<16)+(resp.charCodeAt(1)<<8)+resp.charCodeAt(0)):EVAL, 0, 0, true);
				dP["TM_END"].set(CRC_OK?((resp.charCodeAt(7)<<24)+(resp.charCodeAt(6)<<16)+(resp.charCodeAt(5)<<8)+resp.charCodeAt(4)):EVAL, 0, 0, true);

				//Start T ant V conversion for prepare the data for next iteration
				if(CRC_OK) {
					if(!reset(tr)) continue;
					if(io(tr,(req=SYS.strFromCharCode(0x55)+devID)) != req) continue;
					if(io(tr,(req=SYS.strFromCharCode(0x44))) != req) continue;
					if(!reset(tr)) continue;
					if(io(tr,(req=SYS.strFromCharCode(0x55)+devID)) != req) continue;
					if(io(tr,(req=SYS.strFromCharCode(0xB4))) != req) continue;
				}
			}
			else if(devFamily == 0x29) {	//DS2408
				dP = dO.dP;
				//Check for a modifications
				isMdf = false; tVl = 0; do_ = new Array();
				for(iD = 0; iD < 8; iD++) {
					if(dO["do"+iD].isEVal())	{ isMdf = false; break; }
					do_[iD] = dP["do"+iD].get();
					if(do_[iD] != dO["do"+iD])	isMdf = true;
					if(do_[iD])	tVl = tVl | (1<<iD);
				}
				if(isMdf) {
					if(!reset(tr)) continue;
					if(io(tr,(req=SYS.strFromCharCode(0x55)+devID)) != req) continue;
					if(io(tr,(req=SYS.strFromCharCode(0x5A,tVl,~tVl))) != req) continue;
					// Verify the correct writing, pass by that can be wrong
					//resp = io(tr, SYS.strFromCharCode(0xFF,0xFF));
					//SYS.messInfo("OneWire","Write resp="+SYS.strDecode(resp,"Bin"," "));
				}

				//Read current state
				if(!reset(tr)) continue;
				if(io(tr,(req=SYS.strFromCharCode(0x55)+devID)) != req) continue;
				if(io(tr,(req=SYS.strFromCharCode(0xF0,0x88,0x00))) != req) continue;
				resp = io(tr, SYS.strFromCharCode(0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF));
				//SYS.messInfo("OneWire","resp="+SYS.strDecode(resp,"Bin"," ")+"; CRC="+Special.FLibSYS.CRC(SYS.strFromCharCode(0xF0,0x88,0x00)+resp.slice(0,-2),0xA001,16,0).toString(16));
				CRC_OK = (~((resp.charCodeAt(resp.length-1)<<8) + resp.charCodeAt(resp.length-2)))&0xFFFF;
				CRC_OK = (Special.FLibSYS.CRC(SYS.strFromCharCode(0xF0,0x88,0x00)+resp.slice(0,-2),0xA001,16,0) == CRC_OK);
				for(iD = 0; iD < 8; iD++) {
					dP["di"+iD].set(CRC_OK?resp.charCodeAt(0)&(1<<iD):EVAL, 0, 0, true);
					if(dO["do"+iD].isEVal() || isMdf || !CRC_OK) {
						dO["do"+iD] = CRC_OK ? ((resp.charCodeAt(1)&(1<<iD))?true:false) : EVAL;
						dP["do"+iD].set(dO["do"+iD], 0, 0, true);
					}
				}
			}
			else if(devFamily == 0x3A) {	//DS2413
				dP = dO.dP;
				//SYS.messInfo("OneWire","devID="+SYS.strDecode(devID,"Bin"," "));
				//Check for a modifications
				isMdf = false;
				do0_ = dP.do0.get(), do1_ = dP.do1.get();
				if(!dO.do0.isEVal() && !dO.do1.isEVal() && (do0_ != dO.do0 || do1_ != dO.do1)) {
					tVl = (do1_<<1) | do0_;
					if(!reset(tr)) continue;
					if(io(tr,(req=SYS.strFromCharCode(0x55)+devID)) != req) continue;
					if(io(tr,(req=SYS.strFromCharCode(0x5A,tVl,(~tVl)&0xFF))) != req) continue;
					isMdf = true;
				}

				//Read current state
				if(!reset(tr)) continue;
				if(io(tr,(req=SYS.strFromCharCode(0x55)+devID)) != req) continue;
				if(io(tr,(req=SYS.strFromCharCode(0xF5))) != req) continue;
				resp = io(tr, SYS.strFromCharCode(0xFF));
				//SYS.messInfo("OneWire","resp="+SYS.strDecode(resp,"Bin"," "));
				if(((tVl=resp.charCodeAt(0))&0xF) == ((~(tVl>>4))&0xF)) {
					dP.di0.set(tVl&0x01, 0, 0, true); dP.di1.set(tVl&0x04, 0, 0, true);
					if(dO.do0.isEVal() || dO.do1.isEVal() || isMdf) {
						dO.do0 = (tVl&0x02)?true:false; dP.do0.set(dO.do0, 0, 0, true);
						dO.do1 = (tVl&0x08)?true:false; dP.do1.set(dO.do1, 0, 0, true);
					}
				}
				else {
					dP.di0.set(EVAL, 0, 0, true); dP.di1.set(EVAL, 0, 0, true);
					dP.do0.set(EVAL, 0, 0, true); dP.do1.set(EVAL, 0, 0, true);
					dO.do0 = dO.do1 = EVAL;
				}
			}
		}
	}
}

f_err = t_err;',1509990639);
INSERT INTO "tmplib_LowDevLib" VALUES('1W_DS9097U','One Wire by DS9097U','One Wire sensors bus implementing by 1Wire-adapter DS9097U. Supported direct and parasite powering for the temperature sensors.
Supported 1Wire-devices: DS1820, DS1820/DS18S20/DS1920 (not tested), DS1822 (not tested), DS2413, DS2408, DS2450, DS2438.
Author: Roman Savochenko <rom_as@oscada.org>
Version: 1.2.1',30,0,'JavaLikeCalc.JavaScript
//Functions
function reset(tr) {
	req = (isData?SYS.strFromCharCode(0xE3):"") +
				SYS.strFromCharCode(0xC5);	//CMD_COMM(0x81) | FUNCTSEL_RESET(0x40) | SPEEDSEL_FLEX(0x04)
	isData = false;
	resp = tr.messIO(req);
	return (resp.charCodeAt(0)&0x03) == 0x01;	//RB_PRESENCE
}

function io(tr, mess, bits) {
	if(bits == EVAL) {
		req = isData ? "" : SYS.strFromCharCode(0xE1);	//MODE_DATA
		isData = true;
		for(iB = 0; iB < mess.length; iB++) {
			req += SYS.strFromCharCode(tVl=mess.charCodeAt(iB));
			if(tVl == 0xE3) req += SYS.strFromCharCode(0xE3);	//duplication to the COMMAND mode
		}
		for(resp = tr.messIO(req); resp.length && resp.length < mess.length && (tresp=tr.messIO("")).length; resp += tresp) ;
		return resp;
	}
	else {
		req = (isData?SYS.strFromCharCode(0xE3):"") + SYS.strFromCharCode(0x3F);	//MODE_COMMAND, CMD_CONFIG(0x01) | PARMSEL_5VPULSE(0x30) | PARMSET_infinite(0x0E)
		isData = false;
		for(iB = 0; iB < ceil(bits/8); iB++) {
			vB = mess.charCodeAt(iB);
			bB = ((iB+1)*8 > bits) ? bits-floor(bits/8)*8 : 8;
			for(iBi = 0; iBi < bB; iBi++)
				//{BITPOL_ONE(0x10)|BITPOL_ZERO(0x00)}|CMD_COMM(0x81)|FUNCTSEL_BIT(0x00)|SPEEDSEL_FLEX(0x04)|{PRIME5V_TRUE(0x02)|PRIME5V_FALSE(0x00)}
				req += SYS.strFromCharCode(((vB&(1<<iBi))?0x10:0x00)|0x85/*|((iB==(ceil(bits/8)-1)&&iBi==(bB-1))?0x02:0x00)*/);
		}
		for(resp = tr.messIO(req); resp.length && resp.length < (bits+1) && (tresp=tr.messIO("")).length; resp += tresp) ;
		rez = "";
		if(resp.length && !(resp.charCodeAt(0)&0x81))
			for(iR = 1;  iR < resp.length; ) {
				vB = 0;
				for(ib = 0; ib < 8 && iR < resp.length; iR++, ib++)
					if(resp.charCodeAt(iR)&1) vB = vB | (1<<ib);
				rez += SYS.strFromCharCode(vB);
			}
	}
	return rez;
}

function scan(tr, sn, lstDiscr) {
	if(!reset(tr)) return "";
	
	//Set the bits in the added buffer
	asn = new Array(0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0);
	for(i = 0; lstDiscr >= 0 && i < 64; i++)
		if((i < (lstDiscr-1) && sn[floor(i/8)]&(1<<(i%8))) || i == (lstDiscr-1))
			asn[(i*2+1)/8] = asn[(i*2+1)/8] | (1<<((i*2+1)%8));

	req = isData ? "" : SYS.strFromCharCode(0xE1);	//MODE_DATA
	req += SYS.strFromCharCode(0xF0, 0xE3, 0xB5, 0xE1,
				asn[0], asn[1], asn[2], asn[3], asn[4], asn[5], asn[6], asn[7],
				asn[8], asn[9], asn[10], asn[11], asn[12], asn[13], asn[14], asn[15],
				0xE3, 0xA5);
	isData = false;
	for(resp = tr.messIO(req); resp.length && resp.length < 17 && (tresp=tr.messIO("")).length; resp += tresp) ;

	tmpLastDesc = -1;
	for(i = 0; i < 64; i++) {
		if(resp.charCodeAt(floor((i*2+1)/8)+1)&(1<<((i*2+1)%8)))	sn[floor(i/8)] = sn[floor(i/8)] | (1<<(i%8));
		else sn[floor(i/8)] = sn[floor(i/8)] & (~(1<<(i%8)));
		// Check LastDiscrepancy
		if((resp.charCodeAt(floor((i*2)/8)+1)&(1<<((i*2)%8))) && !(resp.charCodeAt(floor((i*2+1)/8)+1)&(1<<((i*2+1)%8))))
			tmpLastDesc = i + 1;
	}

	rez = "";
	for(iB = 0; iB < 8; iB++) rez += SYS.strFromCharCode(sn[iB]);
	if(sn[0] && lstDiscr != 63 && !Special.FLibSYS.CRC(rez,0x8C,8,0))	lstDiscr = tmpLastDesc;
	else { rez = ""; lstDiscr = 0; }

	return rez;
}

//Set transport and init
if(f_start) {
	tmResc_ = 0;
	devLs = new Object();
	transport_ = transport;
	tr = SYS.Transport.Serial.nodeAt("out_"+transport);
	DS2480 = isData = false;
}
if(f_stop) {
	for(var devID in devLs) {
		if((dO=devLs[devID]).isEVal()) continue;
		aLs = dO.dP.nodeList("a_");
		for(iA = 0; iA < aLs.length; iA++)
			if(!dO.dP[aLs[iA]].isCfg()) dO.dP[aLs[iA]].set(EVAL, 0, 0, true);
	}
	devLs = tr = false;
	return;
}

t_err = "0";

//Check for the transport change and connect
if(!tr || transport != transport_)	{
	tr = SYS.Transport.Serial.nodeAt("out_"+transport);
	transport_ = transport;
	DS2480 = false;
}
//Try DS2480
if(tr && !DS2480)	{
	tr.addr(tr.addr().parse(0,":")+":9600:8N1");	tr.timings("500:20");
	if(tr.start(true)) {
		tr.sendbreak(); SYS.sleep(2e-3);
		tr.messIO(SYS.strFromCharCode(0xC1));	SYS.sleep(4e-3);	//Send timing byte
		//PDSRC=1.37Vus; W1LT=10us; DSO/WORT=8us;
		//read the baud rate (to test command block); do 1 bit operation (to test 1-Wire block)
		req = SYS.strFromCharCode(0x17, 0x45, 0x5B, 0x0F, 0x91);
		for(resp = tr.messIO(req); resp.length && resp.length < 5 && (tresp=tr.messIO("")).length; resp += tresp) ;
		if(resp.length == 5 && (resp.charCodeAt(3)&0xF1) == 0x00 && (resp.charCodeAt(3)&0x0E) == 0x00/*9600*/ &&
				(resp.charCodeAt(4)&0xF0) == 0x90 && (resp.charCodeAt(4)&0x0C) == 0x00/*9600*/)
			DS2480 = true;
	}
	isData = false;
}
if(!tr)	t_err = "1:"+tr("Output transport ''%1'' error.").replace("%1", transport);
else if(!DS2480)	t_err = "2:"+tr("DS2480 is not detected.");
else {
	//Generic information update.
	if(f_start || (tmResc && (tmResc_=tmResc_-1/f_frq) <= 0)) {
		tmResc_ = tmResc;
		tmSc = SYS.time();
		// Check for power
		if(!reset(tr) ||
		 	io(tr,(req=SYS.strFromCharCode(0xCC))) != req ||
			io(tr,(req=SYS.strFromCharCode(0xB4))) != req ||
			!(resp=io(tr,SYS.strFromCharCode(1),1)).length) power = EVAL;
		else power = resp.charCodeAt(0);
		// Scan for allowed devices on the bus.
		sn = new Array(0, 0, 0, 0, 0, 0, 0, 0);
		for(devID = "start", lstDiscr = 0; devID.length && lstDiscr >= 0; ) {
			if((devID=scan(tr,sn,lstDiscr)).length)	{
				//SYS.messInfo("OneWire","lstDiscr="+lstDiscr+"; devID="+SYS.strDecode(devID,"Bin"," "));
				if((dO=devLs[devID]).isEVal()) {
					devLs[devID] = dO = new Object();
					devFamily = devID.charCodeAt(0); devNm = tr("1W device"); dO.isT = false;
					if(devFamily == 0x10)			{ devNm = "DS1820/DS18S20/DS1920"; dO.isT = true; }
					else if(devFamily == 0x20)	devNm = "DS2450";		//4AI
					else if(devFamily == 0x22)	{ devNm = "DS1822"; dO.isT = true; }
					else if(devFamily == 0x26)	devNm = "DS2438";		//SmartBat
					else if(devFamily == 0x28)	{ devNm = "DS18B20"; dO.isT = true; }
					else if(devFamily == 0x29)	devNm = "DS2408";		//8DIO
					else if(devFamily == 0x3A)	devNm = "DS2413";		//2DIO

					// Create the device''s parameter object
					decId = SYS.strDecode(devID,"Bin","");
					SYS.cntrReq(SYS.XMLNode("add").setAttr("path",this.nodePath()+"/%2fbr%2fprm_").setAttr("id",decId).setText(devNm+": "+decId));
					SYS.cntrReq(SYS.XMLNode("set").setAttr("path",this.nodePath()+"/prm_"+decId+"/%2fprm%2fst%2fen").setText("1"));
					// Create the device''s proper attributes
					dO.dP = dP = this["prm_"+decId];
					if(dO.isT) {	//Temperature
						dP.attrAdd("val", tr("Temperature, °С"), "real|ro");
						dP.attrAdd("m", tr("User cell [0...65535]"), "integer");
						dP.attrAdd("res", tr("Resolution")+" [9...12]", "integer");
					}
					else if(devFamily == 0x20)		//DS2450
						for(iN = 0; iN < 4; iN++) {
							dP.attrAdd("ai"+iN, tr("AI, V")+iN, "real|ro");
							dP.attrAdd("res"+iN, tr("Resolution")+iN+" [1...16]", "integer");
							dP.attrAdd("md"+iN, tr("Mode")+iN, "integer|sel", "0;1;2\n2.55V;5.10V;DO");
							dP.attrAdd("do"+iN, tr("DO")+iN, "boolean");
						}
					else if(devFamily == 0x26) {	//DS2438
						dP.attrAdd("t", tr("Temperature, °С"), "real|ro");
						dP.attrAdd("v", tr("Voltage, V"), "real|ro");
						dP.attrAdd("i", tr("Current, A"), "real|ro");
						dP.attrAdd("ICA", tr("Current Accumulator"), "integer");
						dP.attrAdd("thr", tr("Accumulation threshold"), "integer|sel", "0;1;2;3\nNone;±2LSB;±4LSB;±8LSB");
						dP.attrAdd("ETM", tr("Elapsed time meter"), "integer");
						dP.attrAdd("TM_DISC", tr("Disconnect time"), "integer");
						dP.attrAdd("TM_END", tr("End of charge time"), "integer");
						dP.attrAdd("OFF", tr("Current Offset"), "integer");
						dP.attrAdd("IAD", tr("Enable Current A/D"), "boolean");
						dP.attrAdd("CA", tr("Enable Current Accumulator"), "boolean");
						dP.attrAdd("EE", tr("Enable Current Accumulator Shadow"), "boolean");
						dP.attrAdd("AD", tr("Voltage A/D (0-VAD,1-VDD)"), "boolean");
					}
					else if(devFamily == 0x29)		//DS2408
						for(iD = 0; iD < 8; iD++) {
							dP.attrAdd("di"+iD, tr("DI")+iD, "boolean|ro");
							dP.attrAdd("do"+iD, tr("DO")+iD, "boolean");
						}
					else if(devFamily == 0x3A)		//DS2413
						for(iD = 0; iD < 2; iD++) {
							dP.attrAdd("di"+iD, tr("DI")+iD, "boolean|ro");
							dP.attrAdd("do"+iD, tr("DO")+iD, "boolean");
						}
				}
				if(dO.tmSc == tmSc)	break;	//Somthing wrong into the scan but repeat, interruption
				dO.tmSc = tmSc;
				dO.try = tryEVAL;
			}
		}
		//Check for removed devices
		for(devID in devLs) {
			if((dO=devLs[devID]).isEVal()) continue;
			if(dO.tmSc != tmSc)	{
				devLs[devID] = EVAL;
				aLs = dO.dP.nodeList("a_");
				for(iA = 0; iA < aLs.length; iA++)
					if(!dO.dP[aLs[iA]].isCfg())	dO.dP[aLs[iA]].set(EVAL, 0, 0, true);
			}
		}
	}
	else {
		//Process devices for reading current value and set modifable
		for(devID in devLs) {
			if((dO=devLs[devID]).isEVal()) continue;
			devFamily = devID.charCodeAt(0);
			if(dO.isT) {	//Temperature
				dP = dO.dP;
				//SYS.messInfo("OneWire","devID="+SYS.strDecode(devID,"Bin"," "));
				//Check for a modifications
				isMdf = false;
				m_ = dP.m.get(), res_ = max(9,min(12,dP.res.get()));
				if(!dO.m.isEVal() && !dO.res.isEVal() && (m_ != dO.m || res_ != dO.res)) {
					//Set scratchpad
					if(!reset(tr)) continue;
					if(io(tr,(req=SYS.strFromCharCode(0x55)+devID)) != req) continue;
					if(io(tr,(req=SYS.strFromCharCode(0x4E,m_&0xFF,(m_>>8)&0xFF,((res_-9)<<5)|0x1F))) != req) continue;
					//Copy scratchpad part to the EEPROM
					if(!reset(tr)) continue;
					if(io(tr,(req=SYS.strFromCharCode(0x55)+devID)) != req) continue;
					if(io(tr,(req=SYS.strFromCharCode(0x48))) != req) continue;
					isMdf = true;
				}

				//Read current
				// Call the transmission
				if(!reset(tr)) continue;
				if(io(tr,(req=SYS.strFromCharCode(0x55)+devID)) != req) continue;
				if(io(tr,(req=SYS.strFromCharCode(0x44))) != req) continue;
				if(!power) SYS.sleep(0.094*pow(2,(dO.res.isEVal()?12:dO.res)-9));
				// Get scratchpad
				if(!reset(tr)) continue;
				if(io(tr,(req=SYS.strFromCharCode(0x55)+devID)) != req) continue;
				/*out*/io(tr, SYS.strFromCharCode(0xBE));
				resp = io(tr, SYS.strFromCharCode(0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF));
				//SYS.messInfo("OneWire","resp="+SYS.strDecode(resp,"Bin"," "));
				if(!Special.FLibSYS.CRC(resp,0x8C,8,0)) {
					val = resp.charCodeAt(1)*256 + resp.charCodeAt(0);
					if(val > 32767) val -= 65536;
					dP.val.set(val/16, 0, 0, true);
					if(dO.m.isEVal() || dO.res.isEVal() || isMdf) {
						dO.m = resp.charCodeAt(3)*256 + resp.charCodeAt(2); dP.m.set(dO.m, 0, 0, true);
						dO.res = ((resp.charCodeAt(4)>>5)&0x3)+9; dP.res.set(dO.res, 0, 0, true);
					}
					dO.try = 0;
				}
				else if((dO.try=dO.try+1) > tryEVAL) {
					dP.val.set(EVAL, 0, 0, true); dP.m.set(EVAL, 0, 0, true); dP.res.set(EVAL, 0, 0, true);
					dO.m = dO.res = EVAL;
				}
			}
			else if(devFamily == 0x20) {	//DS2450
				dP = dO.dP;
				//Check for a modifications
				for(iN = 0; iN < 4; iN++) {
					if(dO["st1"+iN].isEVal())	continue;
					if((tRes=max(0,min(16,dP["res"+iN].get()))) == 16)	tRes = 0;
					tSt = (dO["st1"+iN]&(~0xCF)) + tRes + ((dP["md"+iN].get()==2)?0x80:0) + (dP["do"+iN].get()?0x40:0);
					if(tSt != dO["st1"+iN]) {	//Same write and check the result
						if(!reset(tr)) continue;
						if(io(tr,(req=SYS.strFromCharCode(0x55)+devID)) != req) continue;
						if(io(tr,(req=SYS.strFromCharCode(0x55,8+iN*2,0,tSt))) != req) continue;
						resp = io(tr, SYS.strFromCharCode(0xFF,0xFF,0xFF));
						if(((~((resp.charCodeAt(1)<<8) + resp.charCodeAt(0)))&0xFFFF) == Special.FLibSYS.CRC(req,0xA001,16,0) && resp.charCodeAt(2) == tSt)
							dO["st1"+iN] = tSt;
					}
					tSt = (dO["st2"+iN]&(~0x01)) + (dP["md"+iN].get()?1:0);
					if(tSt != dO["st2"+iN]) {	//Same write and check the result
						if(!reset(tr)) continue;
						if(io(tr,(req=SYS.strFromCharCode(0x55)+devID)) != req) continue;
						if(io(tr,(req=SYS.strFromCharCode(0x55,8+iN*2+1,0,tSt))) != req) continue;
						resp = io(tr, SYS.strFromCharCode(0xFF,0xFF,0xFF));
						if(((~((resp.charCodeAt(1)<<8) + resp.charCodeAt(0)))&0xFFFF) == Special.FLibSYS.CRC(req,0xA001,16,0) && resp.charCodeAt(2) == tSt)
							dO["st2"+iN] = tSt;
					}
				}

				//Read control status data
				if(dP["res0"].get().isEVal()) {
					if(!reset(tr)) continue;
					if(io(tr,(req=SYS.strFromCharCode(0x55)+devID)) != req) continue;
					if(io(tr,(req=SYS.strFromCharCode(0xAA,0x08,0x00))) != req) continue;
					resp = io(tr, SYS.strFromCharCode(0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF));
					CRC_OK = (~((resp.charCodeAt(resp.length-1)<<8) + resp.charCodeAt(resp.length-2)))&0xFFFF;
					CRC_OK = (Special.FLibSYS.CRC(SYS.strFromCharCode(0xAA,0x08,0x00)+resp.slice(0,-2),0xA001,16,0) == CRC_OK);
					for(iN = 0; iN < 4; iN++) {
						dP["res"+iN].set(CRC_OK?((tVl=resp.charCodeAt(iN*2)&0xF)?tVl:16):EVAL, 0, 0, true);
						dP["md"+iN].set(CRC_OK?((resp.charCodeAt(iN*2)&0x80)?2:resp.charCodeAt(iN*2+1)&1):EVAL, 0, 0, true);
						dP["do"+iN].set(CRC_OK?((resp.charCodeAt(iN*2)&0x40)?true:false):EVAL, 0, 0, true);
						dO["st1"+iN] = resp.charCodeAt(iN*2); dO["st2"+iN] = resp.charCodeAt(iN*2+1);
					}
				} else CRC_OK = true;

				//Read AI value
				if(CRC_OK) {
					if(!reset(tr)) continue;
					if(io(tr,(req=SYS.strFromCharCode(0x55)+devID)) != req) continue;
					if(io(tr,(req=SYS.strFromCharCode(0xAA,0x00,0x00))) != req) continue;
					resp = io(tr, SYS.strFromCharCode(0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF));
					//SYS.messInfo("OneWire","resp="+SYS.strDecode(resp,"Bin"," ")/*+"; CRC="+Special.FLibSYS.CRC(SYS.strFromCharCode(0x3C,0x08,0x00)/*+resp.slice(0,-2),0xA001,16,0).toString(16)*/);
					CRC_OK = (~((resp.charCodeAt(resp.length-1)<<8) + resp.charCodeAt(resp.length-2)))&0xFFFF;
					CRC_OK = (Special.FLibSYS.CRC(SYS.strFromCharCode(0xAA,0x00,0x00)+resp.slice(0,-2),0xA001,16,0) == CRC_OK);
				}
				for(iN = 0; iN < 4; iN++)
					if(CRC_OK) {
						tVl = (resp.charCodeAt(iN*2+1)<<8) + resp.charCodeAt(iN*2);
						vlMax = dP["md"+iN].get() ? 5.1 : 2.55;
						dP["ai"+iN].set(tVl*vlMax/65536, 0, 0, true);
					} else dP["ai"+iN].set(EVAL, 0, 0, true);

				//Start A/D conversion for prepare data for next iteration
				if(CRC_OK) {
					if(!reset(tr)) continue;
					if(io(tr,(req=SYS.strFromCharCode(0x55)+devID)) != req) continue;
					if(io(tr,(req=SYS.strFromCharCode(0x3C,0x0F,0x00))) != req) continue;
					resp = io(tr, SYS.strFromCharCode(0xFF,0xFF));
				}
			}
			else if(devFamily == 0x26) {	//DS2438
				dP = dO.dP;
				//Check for a modifications
				//????

				//Recall memory page 0
				if(!reset(tr)) continue;
				if(io(tr,(req=SYS.strFromCharCode(0x55)+devID)) != req) continue;
				if(io(tr,(req=SYS.strFromCharCode(0xB8,0))) != req) continue;
				//Read current state
				if(!reset(tr)) continue;
				if(io(tr,(req=SYS.strFromCharCode(0x55)+devID)) != req) continue;
				if(io(tr,(req=SYS.strFromCharCode(0xBE,0))) != req) continue;
				resp = io(tr, SYS.strFromCharCode(0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF));
				CRC_OK = !Special.FLibSYS.CRC(resp, 0x8C, 8, 0);
				if(CRC_OK) {
					if((tVl=(resp.charCodeAt(2)<<8)+resp.charCodeAt(1)) >= 32767) tVl -= 65536;
					dP["t"].set(tVl/256, 0, 0, true);
					if((tVl=(resp.charCodeAt(6)<<8)+resp.charCodeAt(5)) >= 32767) tVl -= 65536;
					dP["i"].set(tVl, 0, 0, true);
				}
				else {
					dP["t"].set(EVAL, 0, 0, true);
					dP["i"].set(EVAL, 0, 0, true);
				}
				dP["v"].set(CRC_OK?((resp.charCodeAt(4)<<8)+resp.charCodeAt(3))/100:EVAL, 0, 0, true);
				dP["IAD"].set(CRC_OK?(resp.charCodeAt(0)&0x01):EVAL, 0, 0, true);
				dP["CA"].set(CRC_OK?(resp.charCodeAt(0)&0x02):EVAL, 0, 0, true);
				dP["EE"].set(CRC_OK?(resp.charCodeAt(0)&0x04):EVAL, 0, 0, true);
				dP["AD"].set(CRC_OK?(resp.charCodeAt(0)&0x08):EVAL, 0, 0, true);
				dP["thr"].set(CRC_OK?(resp.charCodeAt(7)>>6):EVAL, 0, 0, true);

				if(CRC_OK) {
					if(!reset(tr)) continue;
					if(io(tr,(req=SYS.strFromCharCode(0x55)+devID)) != req) continue;
					if(io(tr,(req=SYS.strFromCharCode(0xBE,1))) != req) continue;
					resp = io(tr, SYS.strFromCharCode(0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF));
					CRC_OK = !Special.FLibSYS.CRC(resp, 0x8C, 8, 0);
					SYS.messInfo("OneWire","resp="+SYS.strDecode(resp,"Bin"," "));
				}
				dP["ETM"].set(CRC_OK?((resp.charCodeAt(3)<<24)+(resp.charCodeAt(2)<<16)+(resp.charCodeAt(1)<<8)+resp.charCodeAt(0)):EVAL, 0, 0, true);
				dP["ICA"].set(CRC_OK?resp.charCodeAt(4):EVAL, 0, 0, true);
				if(CRC_OK) {
					if((tVl=(resp.charCodeAt(6)<<8)+resp.charCodeAt(5)) >= 32767) tVl -= 65536;
					dP["OFF"].set(tVl, 0, 0, true);
				} else dP["OFF"].set(EVAL, 0, 0, true);

				if(CRC_OK) {
					if(!reset(tr)) continue;
					if(io(tr,(req=SYS.strFromCharCode(0x55)+devID)) != req) continue;
					if(io(tr,(req=SYS.strFromCharCode(0xBE,2))) != req) continue;
					resp = io(tr, SYS.strFromCharCode(0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF));
					CRC_OK = !Special.FLibSYS.CRC(resp, 0x8C, 8, 0);
					SYS.messInfo("OneWire","resp="+SYS.strDecode(resp,"Bin"," "));
				}
				dP["TM_DISC"].set(CRC_OK?((resp.charCodeAt(3)<<24)+(resp.charCodeAt(2)<<16)+(resp.charCodeAt(1)<<8)+resp.charCodeAt(0)):EVAL, 0, 0, true);
				dP["TM_END"].set(CRC_OK?((resp.charCodeAt(7)<<24)+(resp.charCodeAt(6)<<16)+(resp.charCodeAt(5)<<8)+resp.charCodeAt(4)):EVAL, 0, 0, true);

				//Start T ant V conversion for prepare the data for next iteration
				if(CRC_OK) {
					if(!reset(tr)) continue;
					if(io(tr,(req=SYS.strFromCharCode(0x55)+devID)) != req) continue;
					if(io(tr,(req=SYS.strFromCharCode(0x44))) != req) continue;
					if(!reset(tr)) continue;
					if(io(tr,(req=SYS.strFromCharCode(0x55)+devID)) != req) continue;
					if(io(tr,(req=SYS.strFromCharCode(0xB4))) != req) continue;
				}
			}
			else if(devFamily == 0x29) {	//DS2408
				dP = dO.dP;
				//Check for a modifications
				isMdf = false; tVl = 0; do_ = new Array();
				for(iD = 0; iD < 8; iD++) {
					if(dO["do"+iD].isEVal())	{ isMdf = false; break; }
					do_[iD] = dP["do"+iD].get();
					if(do_[iD] != dO["do"+iD])	isMdf = true;
					if(do_[iD])	tVl = tVl | (1<<iD);
				}
				if(isMdf) {
					if(!reset(tr)) continue;
					if(io(tr,(req=SYS.strFromCharCode(0x55)+devID)) != req) continue;
					if(io(tr,(req=SYS.strFromCharCode(0x5A,tVl,~tVl))) != req) continue;
					// Verify the correct writing, pass by that can be wrong
					//resp = io(tr, SYS.strFromCharCode(0xFF,0xFF));
					//SYS.messInfo("OneWire","Write resp="+SYS.strDecode(resp,"Bin"," "));
				}

				//Read current state
				if(!reset(tr)) continue;
				if(io(tr,(req=SYS.strFromCharCode(0x55)+devID)) != req) continue;
				if(io(tr,(req=SYS.strFromCharCode(0xF0,0x88,0x00))) != req) continue;
				resp = io(tr, SYS.strFromCharCode(0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF));
				//SYS.messInfo("OneWire","resp="+SYS.strDecode(resp,"Bin"," "));//+"; CRC="+Special.FLibSYS.CRC(SYS.strFromCharCode(0xF0,0x88,0x00)+resp.slice(0,-2),0xA001,16,0).toString(16));
				CRC_OK = (~((resp.charCodeAt(resp.length-1)<<8) + resp.charCodeAt(resp.length-2)))&0xFFFF;
				CRC_OK = (Special.FLibSYS.CRC(SYS.strFromCharCode(0xF0,0x88,0x00)+resp.slice(0,-2),0xA001,16,0) == CRC_OK);
				for(iD = 0; iD < 8; iD++) {
					dP["di"+iD].set(CRC_OK?resp.charCodeAt(0)&(1<<iD):EVAL, 0, 0, true);
					if(dO["do"+iD].isEVal() || isMdf || !CRC_OK) {
						dO["do"+iD] = CRC_OK ? ((resp.charCodeAt(1)&(1<<iD))?true:false) : EVAL;
						dP["do"+iD].set(dO["do"+iD], 0, 0, true);
					}
				}
			}
			else if(devFamily == 0x3A) {	//DS2413
				dP = dO.dP;
				//SYS.messInfo("OneWire","devID="+SYS.strDecode(devID,"Bin"," "));
				//Check for a modifications
				isMdf = false;
				do0_ = dP.do0.get(), do1_ = dP.do1.get();
				if(!dO.do0.isEVal() && !dO.do1.isEVal() && (do0_ != dO.do0 || do1_ != dO.do1)) {
					tVl = (do1_<<1) | do0_;
					if(!reset(tr)) continue;
					if(io(tr,(req=SYS.strFromCharCode(0x55)+devID)) != req) continue;
					if(io(tr,(req=SYS.strFromCharCode(0x5A,tVl,(~tVl)&0xFF))) != req) continue;
					isMdf = true;
				}

				//Read current state
				if(!reset(tr)) continue;
				if(io(tr,(req=SYS.strFromCharCode(0x55)+devID)) != req) continue;
				if(io(tr,(req=SYS.strFromCharCode(0xF5))) != req) continue;
				resp = io(tr, SYS.strFromCharCode(0xFF));
				//SYS.messInfo("OneWire","resp="+SYS.strDecode(resp,"Bin"," "));
				if(((tVl=resp.charCodeAt(0))&0xF) == ((~(tVl>>4))&0xF)) {
					dP.di0.set(tVl&0x01, 0, 0, true); dP.di1.set(tVl&0x04, 0, 0, true);
					if(dO.do0.isEVal() || dO.do1.isEVal() || isMdf) {
						dO.do0 = (tVl&0x02)?true:false; dP.do0.set(dO.do0, 0, 0, true);
						dO.do1 = (tVl&0x08)?true:false; dP.do1.set(dO.do1, 0, 0, true);
					}
				}
				else {
					dP.di0.set(EVAL, 0, 0, true); dP.di1.set(EVAL, 0, 0, true);
					dP.do0.set(EVAL, 0, 0, true); dP.do1.set(EVAL, 0, 0, true);
					dO.do0 = dO.do1 = EVAL;
				}
			}
		}
	}
}

f_err = t_err;',1509990639);
INSERT INTO "tmplib_LowDevLib" VALUES('BMP180','I2C: BMP180','I2C Pressure and Temperature sensor. Connect through a Serial output transport into the I2C mode.
Author: Roman Savochenko <rom_as@oscada.org>
Version: 1.0.1',10,0,'JavaLikeCalc.JavaScript
//Initial set
if(f_start) {
	f_err = "0";
	transport_ = transport;
	tr = SYS.Transport.Serial.nodeAt("out_"+transport);
	AC1 = 0;
}

t_err = "0";

//Check for the transport change and connect
if(!tr || transport != transport_)	{
	tr = SYS.Transport.Serial.nodeAt("out_"+transport);
	transport_ = transport;
}
if(!tr)	t_err = "1:"+tr("Output transport ''%1'' error.").replace("%1", transport);
else if(addr < 0 || addr > 119)	t_err = "2:"+tr("Device address ''%1'' out of range [0...119].").replace("%1",addr);
else {
	resp = Special.FLibSYS.IO("", "", "b");
	//Calibration data, from register 0xAA and to 0xBF
	if(!AC1) {
		resp.string = tr.messIO(SYS.strFromCharCode(addr,0xAA), 0, 22);
		if(resp.length != 22) t_err = "3:"+tr("Wrong or empty respond to the calibration request.");
		else {
			AC1 = resp.read("int16",1);	AC2 = resp.read("int16",1);	AC3 = resp.read("int16",1);
			AC4 = resp.read("uint16",1); AC5 = resp.read("uint16",1);	AC6 = resp.read("uint16",1);
			B1 = resp.read("int16",1);	B2 = resp.read("int16",1);
			MB = resp.read("int16",1);	MC = resp.read("int16",1);	MD = resp.read("int16",1);
			//SYS.messInfo("BMP180","AC1="+AC1+"; AC2="+AC2+"; AC3="+AC3+"; AC4="+AC4+"; AC5="+AC5+"; AC6="+AC6);
		}
	}

	//Read and calculate temperature
	if(AC1 && !t_err.toInt()) {
		// Start conversion
		tr.messIO(SYS.strFromCharCode(addr,0xF4,0x2E), 0, 0);
		SYS.sleep(4.5e-3);
		// Read value
		resp.pos = 0; resp.string = tr.messIO(SYS.strFromCharCode(addr,0xF6), 0, 2);
		if(resp.length != 2) t_err = "3:"+tr("Wrong or empty respond to the temperature data.");
		else {
			UT = resp.read("int16",1);
			X1 = (UT-AC6)*AC5/pow(2,15);	X2 = MC*pow(2,11)/(X1+MD);
			B5 = X1 + X2;
			t = 0.1*((B5+8)/pow(2,4));
			//SYS.messInfo("BMP180","UT="+UT+"; X1="+X1+"; X2="+X2+"; B5="+B5+"; t="+t);
		}
	}
	
	//Read and calculate pressure
	if(AC1 && !t_err.toInt()) {
		oss = max(0, min(3,oss));
		// Start conversion
		tr.messIO(SYS.strFromCharCode(addr,0xF4,0x34+(oss<<6)), 0, 0);
		SYS.sleep((1.5+3*pow(2,oss))*1e-3);
		// Read value
		resp.pos = 0; resp.string = tr.messIO(SYS.strFromCharCode(addr,0xF6), 0, 4);
		UP = resp.read("int32",1)>>(16-oss);
		B6 = B5 - 4000;
		X1 = (B2*(B6*floor(B6/pow(2,12))))/pow(2,11);
		X2 = AC2*B6/pow(2,11);
		X3 = X1 + X2;
		B3 = (((AC1*4+X3)<<oss)+2)/4;
		X1 = AC3*B6/pow(2,13);
		X2 = (B1*(B6*B6/pow(2,12)))/pow(2,16);
		X3 = ((X1+X2)+2)/pow(2,2);
		B4 = AC4*(X3+32768)/pow(2,15);
		B7 = (UP-B3)*(50000>>oss);
		p = (B7 < 0x80000000) ? (B7*2)/B4 : (B7/B4)*2;
		X1 = (p/pow(2,8))*(p/pow(2,8));
		X1 = (X1*3038)/pow(2,16);
		X2 = (-7357*p)/pow(2,16);
		p = p + (X1+X2+3791)/pow(2,4);
	}
}

if(t_err.toInt() && !f_err.toInt()) t = p = EVAL;
f_err = t_err;',1509990639);
INSERT INTO "tmplib_LowDevLib" VALUES('DHT','GPIO: DHT11,22 (AM23XX)','Digital Temperature and Humidity Sensor for models: DHT11, DHT12, AM2302, AM2320, ...
The module designed for the sensors connect through GPIO, mostly it''s Raspberry PI BCM2835 GPIO.
Conditions: Exclusively realtime planing in the priority 199 (FIFO-99).
Author: Roman Savochenko <rom_as@oscada.org>
Version: 1.0.0',10,0,'JavaLikeCalc.JavaScript
using Special.FLibSYS;

//Set link to fast external functions
if(f_start)	f_err = "0", addr_ = "";

t_err = "0";

//Check for the address change and link
if(addr != addr_)	{
	addr_ = addr;
	function mode = addr+".fnc_mode";
	function get = addr+".fnc_get";
	function put = addr+".fnc_put";
}

//Read sensor''s data function declaration
function read() {
	vl = 0;						//Meassured value
	cntHoldMax = 100;		//Maximum wait counter

	//Call the device to a respond
	mode(pin, 4); put(pin, true); tmSleep(500e-3);	//Set pin to output mode and next to true for 500ms
	put(pin, false); tmSleep(20e-3);							//Set output to false for 20ms
	mode(pin, 2);														//Set pin to input mode

	//Read
	// Wait for pull pin low.
	for(cntHold = 0; get(pin); cntHold++)
		if(cntHold > cntHoldMax) return 0;

	//Meassure the typical pulse length
	for(cntHold = 0; !get(pin); cntHold++)
		if(cntHold > cntHoldMax) return 0;
	for(cntHold = 0; get(pin); cntHold++)
		if(cntHold > cntHoldMax) return 0;
	cntPulse = cntHold;

	//Read meassured value
	for(iB = 0; iB < 40; iB++) {
		//stg = iB;
		for(cntHold = 0; !get(pin); cntHold++)
			if(cntHold > cntHoldMax) return 0;
		for(cntHold = 0; get(pin); cntHold++)
			if(cntHold > cntHoldMax) return 0;
		vl = vl << 1;
		if(cntHold > cntPulse/2)	vl = vl | 1;
	}

	return vl;
}

//Main processing
if(!mode)	t_err = "1:"+tr("No link to external functions on ''%1''.").replace("%1",addr);
else if(!(dev == 0 || dev == 1))	t_err = "2:"+tr("Unknown device %1 [0-DHT11, 1-DHT22].").replace("%1",dev.toString());
else if(tries < 1 || tries > 5)	t_err = "3:"+tr("Tries number %1 out of range [1...5].").replace("%1",tries.toString());
else {
	for(i = 0; i < tries; i++) {
		if(i) tmSleep(2);	//Retry after two seconds
		if((vl=read()) && (vl&0xFF) == ((((vl>>8)&0xFF)+((vl>>16)&0xFF)+((vl>>24)&0xFF)+((vl>>32)&0xFF))&0xFF)) {
			//SYS.messInfo("/DHT","vl="+vl.toString(16));
			if(dev == 1) {
				h = 0.1*(((vl>>32)&0xFF)*256+((vl>>24)&0xFF));
				t = 0.1*(((vl>>16)&0xFF)*256+((vl>>8)&0xFF));
				break;
			}
			h = ((vl>>32)&0xFF) + 20; t = (vl>>16)&0xFF;
		}
	}
	if(i >= tries)	t_err = "10:"+tr("Get data after %1 tries error.").replace("%1",tries.toString());
}

if(t_err.toInt() && !f_err.toInt()) t = h = EVAL;
f_err = t_err;',1509373346);
INSERT INTO "tmplib_LowDevLib" VALUES('DS3231','I2C: DS3231','I2C RTC chip with Temperature sensor and calibration on it. Connect through a Serial output transport into the I2C mode.
Author: Roman Savochenko <rom_as@oscada.org>
Version: 1.0.1',10,0,'JavaLikeCalc.JavaScript
//Set transport
if(f_start) {
	f_err = "0";
	transport_ = transport;
	tr = SYS.Transport.Serial.nodeAt("out_"+transport);
	tm_ = tm = "";
	agOff_ = agOff = 0;
	p32k_ = p32k = false;
	pSQW_ = pSQW = false;
	pSQWf_ = pSQWf = 0;
}

t_err = "0";

//Check for the transport change and connect
if(!tr || transport != transport_)	{
	tr = SYS.Transport.Serial.nodeAt("out_"+transport);
	transport_ = transport;
}
if(!tr)	t_err = "1:"+tr("Output transport ''%1'' error.").replace("%1", transport);
else if(addr < 0 || addr > 119)	t_err = "2:"+tr("Device address ''%1'' out of range [0...119].").replace("%1",addr);
else {
	//Check for modification
	if(tm != tm_) {
		off = 0; sdt = tm.parse(0,"T",off); stm = tm.parse(0,"T",off);
		off = 0; year = sdt.parse(0,"-",off).toInt(16); month = sdt.parse(0,"-",off).toInt(16); day = sdt.parse(0,"-",off).toInt(16);
		off = 0; hour = stm.parse(0,":",off).toInt(16); min = stm.parse(0,":",off).toInt(16); sec = stm.parse(0,":",off).toInt(16);
		tr.messIO(SYS.strFromCharCode(addr,0,sec,min,hour,0,day,month+((year&0xF00)?0x80:0),year&0xFF), 0, 0);
		tm_ = tm;
	}
	if(agOff != agOff_) {
		tr.messIO(SYS.strFromCharCode(addr,16,agOff), 0, 0);
		agOff_ = agOff;
	}
	if(p32k != p32k_) {
		resp = tr.messIO(SYS.strFromCharCode(addr,15), 0, 1);
		if(resp.length != 1) t_err = "3:"+tr("Wrong or empty read respond.");
		else {
			cntrB = resp.charCodeAt(0);
			cntrB = p32k ?	cntrB|0x08 : cntrB&(~0x08);
			tr.messIO(SYS.strFromCharCode(addr,15,cntrB), 0, 0);
		}
		p32k_ = p32k;
	}
	if(pSQW != pSQW_) {
		resp = tr.messIO(SYS.strFromCharCode(addr,14), 0, 1);
		if(resp.length != 1) t_err = "3:"+tr("Wrong or empty read respond.");
		else {
			cntrB = resp.charCodeAt(0);
			cntrB = pSQW ? cntrB&(~0x04) : cntrB|0x04;
			tr.messIO(SYS.strFromCharCode(addr,14,cntrB), 0, 0);
		}
		pSQW_ = pSQW;
	}
	if(pSQWf != pSQWf_) {
		pSQWf = max(0,min(3,pSQWf));
		resp = tr.messIO(SYS.strFromCharCode(addr,14), 0, 1);
		if(resp.length != 1) t_err = "3:"+tr("Wrong or empty read respond.");
		else {
			cntrB = (resp.charCodeAt(0)&(~0x18)) + (pSQWf<<3);
			tr.messIO(SYS.strFromCharCode(addr,14,cntrB), 0, 0);
		}
		pSQWf_ = pSQWf;
	}

	//Get current
	resp = tr.messIO(SYS.strFromCharCode(addr,0), 0, 19);
	if(resp.length != 19) t_err = "3:"+tr("Wrong or empty read respond.");
	else {
		// Get current time
		tm_ = tm = (20+(resp.charCodeAt(5)>>7)).toString(10,2)+resp.charCodeAt(6).toString(16,2)+"-" +
				(resp.charCodeAt(5)&0x7F).toString(16,2)+"-" + 
				resp.charCodeAt(4).toString(16,2)+"T" +
				(resp.charCodeAt(2)&0x3F).toString(16,2)+":" +
				resp.charCodeAt(1).toString(16,2)+":" + 
				resp.charCodeAt(0).toString(16,2);
		// Aging offset
		if((agOff=resp.charCodeAt(16)) >= 128)	agOff = agOff-256;
		agOff_ = agOff;

		// Miscellaneous
		p32k = p32k_ = resp.charCodeAt(15)&0x08;
		pSQW = pSQW_ = !(resp.charCodeAt(14)&0x04);
		pSQWf = pSQWf_ = (resp.charCodeAt(14)>>3)&0x3;

		// Get temperature
		t = (resp.charCodeAt(17)<<2)+(resp.charCodeAt(18)>>6);
		if(t >= 128) t = t-256;
		t = t/4;
	}
}

if(t_err.toInt() && !f_err.toInt()) t = p = EVAL;
f_err = t_err;',1509990639);
INSERT INTO "tmplib_LowDevLib" VALUES('PCF8574','I2C: PCF8574','I2C 8-bit 8DIO. Connect through a Serial output transport into the I2C mode.
Author: Roman Savochenko <rom_as@oscada.org>
Version: 1.0.1',10,0,'JavaLikeCalc.JavaScript
//Set transport
if(f_start) {
	f_err = "0";
	transport_ = transport;
	tr = SYS.Transport.Serial.nodeAt("out_"+transport);
	di0 = di1 = di2 = di3 = di4 = di5 = di6 = di7 = EVAL;
	do0 = do1 = do2 = do3 = do4 = do5 = do6 = do7 = true;
	do0_ = do1_ = do2_ = do3_ = do4_ = do5_ = do6_ = do7_ = true;
}

t_err = "0";

//Check for the transport change and connect
if(!tr || transport != transport_)	{
	tr = SYS.Transport.Serial.nodeAt("out_"+transport);
	transport_ = transport;
}
if(!tr)	t_err = "1:"+tr("Output transport ''%1'' error.").replace("%1", transport);
else if(addr < 0 || addr > 119)	t_err = "2:"+tr("Device address ''%1'' out of range [0...119].").replace("%1",addr);
else {
	if(do0 != do0_ || do1 != do1_ || do2 != do2_ || do3 != do3_ || do4 != do4_ || do5 != do5_ || do6 != do6_ || do7 != do7_) {
		tr.messIO(SYS.strFromCharCode(addr,(do0?0x01:0)|(do1?0x02:0)|(do2?0x04:0)|(do3?0x08:0)|
																(do4?0x10:0)|(do5?0x20:0)|(do6?0x40:0)|(do7?0x80:0)), 0, 0);
		do0_ = do0; do1_ = do1; do2_ = do2; do3_ = do3; do4_ = do4; do5_ = do5; do6_ = do6; do7_ = do7;
	}
	rez = tr.messIO(SYS.strFromCharCode(addr), 0, 1);
	if(rez.length) {
		vl = rez.charCodeAt(0);
		di0 = (vl&0x01); di1 = (vl&0x02); di2 = (vl&0x04); di3 = (vl&0x08);
		di4 = (vl&0x10); di5 = (vl&0x20); di6 = (vl&0x40); di7 = (vl&0x80);
	}
	else t_err = "3:"+tr("No read result.");
}

if(t_err.toInt() && !f_err.toInt())
	di0 = di1 = di2 = di3 = di4 = di5 = di6 = di7 = EVAL;

f_err = t_err;',1509990639);
INSERT INTO "tmplib_LowDevLib" VALUES('PCF8591','I2C: PCF8591','I2C 8-bit 4xA/D and D/A converter. Connect through a Serial output transport into the I2C mode.
Author: Roman Savochenko <rom_as@oscada.org>
Version: 1.0.1',10,0,'JavaLikeCalc.JavaScript
//Set transport
if(f_start) {
	f_err = "0";
	transport_ = transport;
	tr = SYS.Transport.Serial.nodeAt("out_"+transport);
}

t_err = "0";

//Check for the transport change and connect
if(!tr || transport != transport_)	{
	tr = SYS.Transport.Serial.nodeAt("out_"+transport);
	transport_ = transport;
}
if(!tr)	t_err = "1:"+tr("Output transport ''%1'' error.").replace("%1", transport);
else if(addr < 0 || addr > 119)	t_err = "2:"+tr("Device address ''%1'' out of range [0...119].").replace("%1",addr);
else {
	for(i = 0; i < 4 && !t_err.toInt(); i++) {
		//Select next channel and the output
		tr.messIO(SYS.strFromCharCode(addr,0x40|((i+1)&0x3),255*max(0,min(1,ao/vRef))), 0, 0);
		SYS.sleep(1e-3);
		rez = tr.messIO("", 0, 1);
		if(rez.length)	arguments["ai"+i] = vRef*(rez.charCodeAt(0)/255);
		else t_err = "3:"+tr("No read result.");
	}
}

if(t_err.toInt() && !f_err.toInt())
	for(i = 0; i < 4; i++)
		arguments["ai"+i] = EVAL;

f_err = t_err;',1509908745);
INSERT INTO "tmplib_LowDevLib" VALUES('BME280','I2C: BME280','I2C Pressure, Temperature and Humidity sensor. Connect through a Serial output transport into the I2C mode.
Author: Arcadiy Kisel, Roman Savochenko <rom_as@oscada.org>
Version: 1.0.1',10,0,'JavaLikeCalc.JavaScript
//Initial set
if(f_start) {
	f_err = "0";
	transport_ = transport;
	tr = SYS.Transport.Serial.nodeAt("out_"+transport);
	dig_T1 = 0;
}

t_err = "0";

//Check for the transport change and connect
if(!tr || transport != transport_)	{
	tr = SYS.Transport.Serial.nodeAt("out_"+transport);
	transport_ = transport;
}
if(!tr)	t_err = "1:"+tr("Output transport ''%1'' error.").replace("%1",transport);
else if(addr < 0 || addr > 119)	t_err = "2:"+tr("Device address ''%1'' out of range [0...119].").replace("%1",addr);
else {
	resp = Special.FLibSYS.IO("", "", "l");
	//Calibration data, from register 0x88 and count 24 
	if(!dig_T1) {
		resp.string = tr.messIO(SYS.strFromCharCode(addr,0x88), 0, 24); //start address 88, count = 24
		if(resp.length != 24)	t_err = "3:"+tr("Wrong or empty respond to the calibration T1-3 or P1-9 request.");
		else {
			dig_T1 = resp.read("uint16", 1);	dig_T2 = resp.read("int16", 1);	dig_T3 = resp.read("int16", 1);
			dig_P1 = resp.read("uint16", 1);	dig_P2 = resp.read("int16", 1);	dig_P3 = resp.read("int16", 1);
			dig_P4 = resp.read("int16", 1);	dig_P5 = resp.read("int16", 1);	dig_P6 = resp.read("int16", 1);
			dig_P7 = resp.read("int16", 1);	dig_P8 = resp.read("int16", 1);	dig_P9 = resp.read("int16", 1);
		}
		resp.pos = 0;
		resp.string = tr.messIO(SYS.strFromCharCode(addr,0xA1), 0, 1); //start address A1, count = 1
		if(resp.length != 1) t_err = "3:"+tr("Wrong or empty respond to the calibration H1 request.");
		else dig_H1 = resp.read("uint8",1);
		resp.pos = 0;
		resp.string = tr.messIO(SYS.strFromCharCode(addr,0xE1), 0, 7); //start address E1, count = 8
		if(resp.length != 7) t_err = "3:"+tr("Wrong or empty respond to the calibration H2-H6 request.");
		else {
			dig_H2 = resp.read("int16", 1);
			dig_H3 = resp.read("uint8", 1);

			E4 = resp.read("int8", 1);
			E5 = resp.read("int8", 1);
			E6 = resp.read("int8", 1);
			// dig_H4, dig_H5 is signed. may be bug here, when <0
			dig_H4 = (E4 << 4) | (E5 & 0x0F);
			dig_H5 = (E6 << 4) | ((E5 >> 4) & 0x0F);
			dig_H6 = resp.read("int8", 1);
		}
	}

	oss = max(0, min(7,oss));

	//Pressure and Temperature
	tr.messIO(SYS.strFromCharCode(addr,0xF4,(oss<<5)|(oss<<2)|0x3), 0, 0);
	
	//Humidity
	tr.messIO(SYS.strFromCharCode(addr,0xF2,oss), 0, 0);	

	SYS.sleep((1.25 + (2.3*oss) + 2*(2.3*oss + 0.575))*1e-3); 

	//Read and calculate temperature
	if(dig_T1 && !t_err.toInt()) {
		// Read value
		resp = Special.FLibSYS.IO("", "", "b");
		resp.pos = 0;
		resp.string = tr.messIO(SYS.strFromCharCode(addr,0xFA), 0, 4);
		if(resp.length != 4) t_err = "3:"+tr("Wrong or empty respond to the temperature data.");
		else {
			adc_T = resp.read("uint32", 1) >> 12; //8 бит т.к. прочли четвертый "лишний" байт. 4 бит по даташиту
			var1 = (adc_T/16384 - dig_T1/1024)*dig_T2;
			var2 = ((adc_T/131072 - dig_T1/8192)*(adc_T/131072 - dig_T1/8192))*dig_T3;
			t_fine = var1 + var2; //for pressure
			t = (var1 + var2)/5120;
		}
	}

	//Read and calculate pressure
	if(dig_T1 && !t_err.toInt()) {
		// Read value
		resp.pos = 0; 
		resp.string = tr.messIO(SYS.strFromCharCode(addr,0xF7), 0, 4);
		if(resp.length != 4) t_err = "3:"+tr("Wrong or empty respond to the pressure data.");
		else {
			adc_P = resp.read("uint32", 1) >> 12; //8 бит т.к. прочли четвертый "лишний" байт. 4 бит по даташиту
			
			var1 = t_fine/2 - 64000;
			var2 = var1*var1*dig_P6/32768;
			var2 = var2 + var1*dig_P5*2;
			var2 = var2/4 + dig_P4*65536;
			var1 = (dig_P3*var1*var1/524288 + dig_P2*var1)/524288;
			var1 = (1 + var1/32768)*dig_P1;
			if(var1 == 0)	p = EVAL_REAL; // avoid exception caused by division by zero
			else {
				p = 1048576 - adc_P;
				p = (p - (var2/4096))*6250/var1;
				var1 = dig_P9*p*p/2147483648;
				var2 = p*dig_P8/32768;
				p = p + (var1 + var2 + dig_P7)/16;
			}
		}
	}
	
	//Read and calculate humidity
	if(dig_T1 && !t_err.toInt()) {
		resp.pos = 0;
		resp.string = tr.messIO(SYS.strFromCharCode(addr,0xFD), 0, 2);
		if(resp.length != 2) t_err = "3:"+tr("Wrong or empty respond to the humidity data.");
		else {
			adc_H = resp.read("uint16", 1);
			
			h = t_fine - 76800;
			h = (adc_H - (dig_H4*64 + dig_H5/16384*h))*(dig_H2/65536*(1 + dig_H6/67108864*h*(1 + dig_H3/67108864*h)));
			h = max(0, min(100, h*(1 - dig_H1*h/524288)));
		}
	}	
}

if(t_err.toInt() && !f_err.toInt()) t = p = h = EVAL;

f_err = t_err;',1509989860);
CREATE TABLE 'tmplib_LowDevLib_io' ("TMPL_ID" TEXT DEFAULT '' ,"ID" TEXT DEFAULT '' ,"NAME" TEXT DEFAULT '' ,"TYPE" INTEGER DEFAULT '0' ,"FLAGS" INTEGER DEFAULT '0' ,"VALUE" TEXT DEFAULT '' ,"POS" INTEGER DEFAULT '0' , PRIMARY KEY ("TMPL_ID","ID"));
INSERT INTO "tmplib_LowDevLib_io" VALUES('1602A','transport','Transport of the I2C, Serial (i2c) or
GPIO address with function put(), mostly it''s BCM2835 (DAQ.BCM2835.pi.pi)',0,64,'i2c',0);
INSERT INTO "tmplib_LowDevLib_io" VALUES('1602A','addr','I2C device address [0...119]',1,64,'39',1);
INSERT INTO "tmplib_LowDevLib_io" VALUES('1602A','RS','GPIO Pin: Reset',1,64,'7',2);
INSERT INTO "tmplib_LowDevLib_io" VALUES('1602A','E','GPIO Pin: Enable',1,64,'8',3);
INSERT INTO "tmplib_LowDevLib_io" VALUES('1602A','D4','GPIO Pin: Data4',1,64,'25',4);
INSERT INTO "tmplib_LowDevLib_io" VALUES('1602A','D5','GPIO Pin: Data5',1,64,'24',5);
INSERT INTO "tmplib_LowDevLib_io" VALUES('1602A','D6','GPIO Pin: Data6',1,64,'23',6);
INSERT INTO "tmplib_LowDevLib_io" VALUES('1602A','D7','GPIO Pin: Data7',1,64,'18',7);
INSERT INTO "tmplib_LowDevLib_io" VALUES('1602A','ln1','Line 1',0,32,'',8);
INSERT INTO "tmplib_LowDevLib_io" VALUES('1602A','ln2','Line 2',0,32,'',9);
INSERT INTO "tmplib_LowDevLib_io" VALUES('1W_DS9097','transport','Transport of the One Wire bus, Serial',0,64,'oneWire',0);
INSERT INTO "tmplib_LowDevLib_io" VALUES('1W_DS9097','tmResc','Rescan period, s',2,64,'60',1);
INSERT INTO "tmplib_LowDevLib_io" VALUES('1W_DS9097','tryEVAL','Tries after which set value to EVAL',1,64,'3',2);
INSERT INTO "tmplib_LowDevLib_io" VALUES('1W_DS9097','power','Power, for temperature',3,16,'',3);
INSERT INTO "tmplib_LowDevLib_io" VALUES('1W_DS9097','this','Object',4,0,'',4);
INSERT INTO "tmplib_LowDevLib_io" VALUES('1W_DS9097U','transport','Transport of the One Wire bus, Serial',0,64,'oneWire',0);
INSERT INTO "tmplib_LowDevLib_io" VALUES('1W_DS9097U','tmResc','Rescan period, s',2,64,'60',1);
INSERT INTO "tmplib_LowDevLib_io" VALUES('1W_DS9097U','tryEVAL','Tries after which set value to EVAL',1,64,'3',2);
INSERT INTO "tmplib_LowDevLib_io" VALUES('1W_DS9097U','power','Power, for temperature',3,16,'',3);
INSERT INTO "tmplib_LowDevLib_io" VALUES('1W_DS9097U','isData','In data mode',3,0,'0',4);
INSERT INTO "tmplib_LowDevLib_io" VALUES('1W_DS9097U','this','Object',4,0,'',5);
INSERT INTO "tmplib_LowDevLib_io" VALUES('BMP180','transport','Transport of the I2C, Serial',0,64,'i2c',0);
INSERT INTO "tmplib_LowDevLib_io" VALUES('BMP180','addr','Device address [0...119]',1,64,'119',1);
INSERT INTO "tmplib_LowDevLib_io" VALUES('BMP180','oss','Oversampling setting (0...3)',1,64,'0',2);
INSERT INTO "tmplib_LowDevLib_io" VALUES('BMP180','t','T, °С',2,16,'',3);
INSERT INTO "tmplib_LowDevLib_io" VALUES('BMP180','p','P, Pa',2,16,'',4);
INSERT INTO "tmplib_LowDevLib_io" VALUES('DHT','addr','GPIO address with functions mode(), get() and put(), mostly it''s BCM2835',0,64,'DAQ.BCM2835.pi.pi',0);
INSERT INTO "tmplib_LowDevLib_io" VALUES('DHT','pin','IO pin number of the GPIO',1,64,'17',1);
INSERT INTO "tmplib_LowDevLib_io" VALUES('DHT','tries','Tries [1...5]',1,64,'2',2);
INSERT INTO "tmplib_LowDevLib_io" VALUES('DHT','dev','Device (0-DHT11, 1-DHT22)',1,64,'1',3);
INSERT INTO "tmplib_LowDevLib_io" VALUES('DHT','t','T, °С',2,16,'',4);
INSERT INTO "tmplib_LowDevLib_io" VALUES('DHT','h','H, %',2,16,'',5);
INSERT INTO "tmplib_LowDevLib_io" VALUES('DS3231','transport','Transport of the I2C, Serial',0,64,'i2c',0);
INSERT INTO "tmplib_LowDevLib_io" VALUES('DS3231','addr','Device address [0...119]',1,64,'104',1);
INSERT INTO "tmplib_LowDevLib_io" VALUES('DS3231','tm','Date and time, YYYY-MM-DDTHH:mm:SS',0,32,'',2);
INSERT INTO "tmplib_LowDevLib_io" VALUES('DS3231','agOff','Aging offset, [-128...127]',1,32,'',3);
INSERT INTO "tmplib_LowDevLib_io" VALUES('DS3231','t','T, °С',2,16,'',4);
INSERT INTO "tmplib_LowDevLib_io" VALUES('DS3231','p32k','Enable 32768Hz',3,32,'',5);
INSERT INTO "tmplib_LowDevLib_io" VALUES('DS3231','pSQW','Enable SQUARE-WAVE OUTPUT',3,32,'',6);
INSERT INTO "tmplib_LowDevLib_io" VALUES('DS3231','pSQWf','SQUARE-WAVE OUTPUT frequency: 0-1Hz, 1-1.024kHz, 2-4.096kHz, 3-8.192kHz',1,32,'',7);
INSERT INTO "tmplib_LowDevLib_io" VALUES('PCF8574','transport','Transport of the I2C, Serial',0,64,'i2c',0);
INSERT INTO "tmplib_LowDevLib_io" VALUES('PCF8574','addr','Device address [0...119]',1,64,'39',1);
INSERT INTO "tmplib_LowDevLib_io" VALUES('PCF8574','di0','DI0',3,16,'',2);
INSERT INTO "tmplib_LowDevLib_io" VALUES('PCF8574','di1','DI1',3,16,'',3);
INSERT INTO "tmplib_LowDevLib_io" VALUES('PCF8574','di2','DI2',3,16,'',4);
INSERT INTO "tmplib_LowDevLib_io" VALUES('PCF8574','di3','DI3',3,16,'',5);
INSERT INTO "tmplib_LowDevLib_io" VALUES('PCF8574','di4','DI4',3,16,'',6);
INSERT INTO "tmplib_LowDevLib_io" VALUES('PCF8574','di5','DI5',3,16,'',7);
INSERT INTO "tmplib_LowDevLib_io" VALUES('PCF8574','di6','DI6',3,16,'',8);
INSERT INTO "tmplib_LowDevLib_io" VALUES('PCF8574','di7','DI7',3,16,'',9);
INSERT INTO "tmplib_LowDevLib_io" VALUES('PCF8574','do0','DO0',3,33,'',10);
INSERT INTO "tmplib_LowDevLib_io" VALUES('PCF8574','do1','DO1',3,33,'',11);
INSERT INTO "tmplib_LowDevLib_io" VALUES('PCF8574','do2','DO2',3,33,'',12);
INSERT INTO "tmplib_LowDevLib_io" VALUES('PCF8574','do3','DO3',3,33,'',13);
INSERT INTO "tmplib_LowDevLib_io" VALUES('PCF8574','do4','DO4',3,33,'',14);
INSERT INTO "tmplib_LowDevLib_io" VALUES('PCF8574','do5','DO5',3,33,'',15);
INSERT INTO "tmplib_LowDevLib_io" VALUES('PCF8574','do6','DO6',3,33,'',16);
INSERT INTO "tmplib_LowDevLib_io" VALUES('PCF8574','do7','DO7',3,33,'',17);
INSERT INTO "tmplib_LowDevLib_io" VALUES('PCF8591','transport','Transport of the I2C, Serial',0,64,'i2c',0);
INSERT INTO "tmplib_LowDevLib_io" VALUES('PCF8591','addr','Device address [0...119]',1,64,'72',1);
INSERT INTO "tmplib_LowDevLib_io" VALUES('PCF8591','vRef','Reference voltage, V',2,64,'3.2',2);
INSERT INTO "tmplib_LowDevLib_io" VALUES('PCF8591','ai0','AI0',2,16,'',3);
INSERT INTO "tmplib_LowDevLib_io" VALUES('PCF8591','ai1','AI1',2,16,'',4);
INSERT INTO "tmplib_LowDevLib_io" VALUES('PCF8591','ai2','AI2',2,16,'',5);
INSERT INTO "tmplib_LowDevLib_io" VALUES('PCF8591','ai3','AI3',2,16,'',6);
INSERT INTO "tmplib_LowDevLib_io" VALUES('PCF8591','ao','AO',2,32,'',7);
INSERT INTO "tmplib_LowDevLib_io" VALUES('BME280','transport','Transport of the I2C, Serial',0,64,'i2c',0);
INSERT INTO "tmplib_LowDevLib_io" VALUES('BME280','addr','Device address [0...119]',1,64,'118',1);
INSERT INTO "tmplib_LowDevLib_io" VALUES('BME280','oss','Oversampling setting (0...7)',1,64,'3',2);
INSERT INTO "tmplib_LowDevLib_io" VALUES('BME280','t','T, °С',2,16,'',3);
INSERT INTO "tmplib_LowDevLib_io" VALUES('BME280','p','P, Pa',2,16,'',4);
INSERT INTO "tmplib_LowDevLib_io" VALUES('BME280','h','H, %',2,16,'',5);
COMMIT;
