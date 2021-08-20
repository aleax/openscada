PRAGMA foreign_keys=OFF;
BEGIN TRANSACTION;
CREATE TABLE IF NOT EXISTS 'ParamTemplLibs' ("ID" TEXT DEFAULT '' ,"NAME" TEXT DEFAULT '' ,"uk#NAME" TEXT DEFAULT '' ,"DESCR" TEXT DEFAULT '' ,"uk#DESCR" TEXT DEFAULT '' ,"DB" TEXT DEFAULT '' ,"ru#NAME" TEXT DEFAULT '' ,"ru#DESCR" TEXT DEFAULT '' , PRIMARY KEY ("ID"));
INSERT INTO ParamTemplLibs VALUES('tests','Tests','Тести','Templates library of the test parameters for PLC series S7 of the firm Siemens.

Author: Roman Savochenko <roman@oscada.org>','Бібліотека шаблонів параметрів для контролерів фірми Siemens серії S7.

Автор: Роман Савоченко <roman@oscada.org>','tmplib_tests','Тесты','Библиотека шаблонов параметров для контроллеров фирмы Siemens серии S7.

Автор: Роман Савоченко <roman@oscada.org>');
INSERT INTO ParamTemplLibs VALUES('base','Main library','Основна бібліотека','Library of the main templates of the DAQ-sources processing and related services.

The library was created to provide main templates of the DAQ-sources processing and related services.

Author: Roman Savochenko <roman@oscada.org>
Used by: Most projects on OpenSCADA
Founded: January 2006
Version: 2.0.0
License: GPLv2
DOC: Libs_Main|Libs/Main','Бібліотека основних шаблонів опрацювання DAQ-джерел та похідних сервісів.

Бібліотеку створено для надання основних шаблонів опрацювання DAQ-джерел та похідних сервісів.

Автор: Роман Савоченко <roman@oscada.org>
Використано: Більшість проектів OpenSCADA
Засновано: Січень 2006
Версія: 2.0.0
Ліцензія: GPLv2
DOC: Libs_Main|Libs/Main','tmplib_base','Основная библиотека','Библиотека основных шаблонов обработки DAQ-источников и производных сервисов.

Библиотека создана для предоставления основных шаблонов обработки DAQ-источников и производных сервисов.

Автор: Роман Савоченко <roman@oscada.org>
Использовано: Большинство проектов OpenSCADA
Основано: Январь 2006
Версия: 2.0.0
Лицензия: GPLv2
DOC: Libs_Main|Libs/Main');
INSERT INTO ParamTemplLibs VALUES('DevLib','Industrial devices','Промислові пристрої','The user protocol devices library created to provide access to industrial device''s data through network, like to common industrial automation devices and wide resources counters, with protocols simple enough to implement into the User Protocol module, using the presented complex protocols (ModBus, OPC_UA, HTTP) or directly on the internal like to Java language.

The template''s names and their parameters are available in languages: English, Ukrainian and Russian. Their source code wrote in the human-language independent mode with calls for the translations by the function tr() and the message''s translation also allowed for English, Ukrainian and Russian.

Author: Roman Savochenko <roman@oscada.org>, Constantine (IrmIngeneer) (2018), Arsen Zakojan (2017), Ruslan Yarmoliuk (2017)
Founded: January 2010
Version: 2.6.1
License: GPLv2
DOC: Libs_Devices|Libs/Devices','Бібліотеку пристроїв користувацьких протоколів створено для надання доступу до даних промислових пристроїв через мережу із доволі простим протоколом, на кшталт пристроїв загальної промислової автоматики та лічильників різних ресурсів, із протоколом достатньо простим до реалізації у модулі користувацького протоколу, з використанням наявних комплексних протоколів (ModBus, OPC_UA, HTTP) або безпосередньо на внутрішній мові подібній до Java.

Автор: Роман Савоченко <roman@oscada.org>, Константин (IrmIngeneer) (2018), Арсен Закоян (2017), Руслан Ярмолюк (2017)
Засновано: Січень 2010
Версія: 2.6.1
Ліцензія: GPLv2
DOC: Libs_Devices|Libs/Devices','tmplib_DevLib','Промышленные устройства','');
INSERT INTO ParamTemplLibs VALUES('PrescrTempl','Prescriptions','Рецепти','The library is created to provide an environment of execution of scenarios of the technological operations — prescriptions, and frames of the user interface about them, including the frame of creation/edition the prescriptions and two frames of the execution control and reporting — "Prescription — run" and "Prescription — run, simple". The library is built on the basis primitives of the widgets and the internal programming language JavaLikeCalc, including templates and commands.

The section contains DAQ-templates of the prescription manager and commands of the "command-template" mode, which are intended to be connected to the logic level controller, by creating related parameters of the prescription manager and commands, which available for the user to choose from, and which will carry out all the work with the prescriptions-programs for their processing and execution.

The element''s names and their parameters are available in languages: English, Ukrainian and Russian. Their source code wrote in human-language independent mode with calls for the translations by the function tr() and the message''s translation also allowed for English, Ukrainian and Russian.

Author: Roman Savochenko <roman@oscada.org>
Sponsored by: Vassily Grigoriev, the Laboratory of Vacuum Technologies
Sponsored by, about the controller templateization: Magomed, SAVTECH
Founded: April 2012
Version: 1.1.0
License: GPLv2
DOC: Libs_Prescriptions|Libs/Prescriptions','Бібліотека створюється для надання середовища виконання сценаріїв технологічних процесів — рецептів, та кадрів користувацького інтерфейсу довкола нього, включно з кадром створення/редагування рецептів та двома кадрами контролю виконання та звітності — "Рецепт — виконання" та "Рецепт — виконання, простий". Будується бібліотека на основі примітивів віджетів та мови внутрішнього програмування JavaLikeCalc, включно для шаблонів та команд.

Розділ містить DAQ-шаблони менеджеру рецептів та команд режиму "команда-шаблон", які призначено до підключення у контролері логічного рівня, через створення відповідно параметрів менеджеру рецептів та команд, доступних до обрання користувачем, які і здійснюватимуть там всю роботу із рецептами-програмами щодо їх опрацювання та виконання.

Назви елементів та їх параметрів доступні на мовах: Англійська, Українська та Російська. Їх вихідний код написано у мово(людська)-незалежному режимі з викликом функції перекладу tr() та переклад цих повідомлень також доступний Англійською, Українською та Російською.

Автор: Роман Савоченко <roman@oscada.org>
Спонсоровано: Василь Григор''єв, Лабораторія Вакуумних Технологій
Спонсоровано, щодо шаблонізації контролеру: Магомед, SAVTECH
Засновано: Квітень 2012
Версія: 1.1.0
Ліцензія: GPLv2
DOC: Libs_Prescriptions|Libs/Prescriptions','tmplib_PrescrTempl','Рецепты','Библиотека создаётся для предоставления среды исполнения сценариев технологических процессов — рецептов, и кадров пользовательского интерфейса около него, включая кадр создания/редактирования рецептов и два кадра контроля исполнения и отчётности — "Рецепт — исполнение" и "Рецепт — исполнение, простой". Строится библиотека на основе примитивов виджетов и языка внутреннего программирования JavaLikeCalc, включая для шаблонов и команд.

Раздел содержит DAQ-шаблоны менеджера рецептов и команд режима "команда-шаблон", которые предназначены к подключению в контроллере логического уровня, путём создания соответственно параметров менеджера рецептов и команд, доступных для выбора пользователем, которые и будут осуществлять там всю роботу с рецептами-програмами на предмет их обработки и исполнения.

Названия элементов и их параметров доступны на языках: Английский, Украинский и Российский. Их исходный код написан в языко(человеческий)-независимом режиме с вызовом функции перевода tr() и перевод этих сообщений также доступен Английским, Украинским и Российским.

Автор: Роман Савоченко <roman@oscada.org>
Спонсировано: Василий Григорьев, Лаборатория Вакуумных Технологий
Спонсировано, на предмет шаблонизации контроллера: Магомед, SAVTECH
Основано: Апрель 2012
Версия: 1.1.0
Лицензия: GPLv2
DOC: Libs_Prescriptions|Libs/Prescriptions');
INSERT INTO ParamTemplLibs VALUES('LowDevLib','Low-level devices','Низькорівневі пристрої','Library of templates to provide access to device''s data of the low-level buses.

The user protocol devices library created to provide access to device''s data of low-level buses, with protocols simple enough to implement into the User Protocol module or directly on the internal like to Java language.

The template''s names and their parameters are available in languages: English, Ukrainian and Russian. Their source code wrote in the human-language independent mode with calls for the translations by the function tr() and the message''s translation also allowed for English, Ukrainian and Russian.

Author: Roman Savochenko <roman@oscada.org>, Arcadiy Kisel (2017)
Founded: Jul 2016
Version: 1.4.0
License: GPLv2
DOC: Libs_LowLevelDevices|Libs/LowLevelDevices','Бібліотека шаблонів надання доступу до даних пристроїв низькорівневих шин.

Бібліотеку пристроїв користувацьких протоколів створено для надання доступу до даних пристроїв низькорівневих шин, із протоколом достатньо простим до реалізації у модулі користувацького протоколу або безпосередньо на внутрішній мові подібній до Java.

Назви елементів та їх параметрів доступні на мовах: Англійська, Українська та Російська. Їх вихідний код написано у мово(людська)-незалежному режимі з викликом функції перекладу tr() та переклад цих повідомлень також доступний Англійською, Українською та Російською.

Автор: Роман Савоченко <roman@oscada.org>, Аркадій Кисіль (2017)
Засновано: Липень 2016
Версія: 1.4.0
Ліцензія: GPLv2
DOC: Libs_LowLevelDevices|Libs/LowLevelDevices','tmplib_LowDevLib','Низкоуровневые устройства','Библиотека шаблонов предоставления доступа к данным устройств низкоуровневых шин.

Библиотека устройств пользовательских протоколов создана для предоставления доступа к данным устройств низкоуровневых шин, с протоколом достаточно простым для реализации в модуле пользовательского протокола или непосредственно на внутреннем языке подобном на Java.

Названия элементов и их параметров доступны на языках: Английский, Украинский и Российский. Их исходный код написан в языко(человеческий)-независимом режиме с вызовом функции перевода tr() и перевод этих сообщений также доступен Английским, Украинским и Российским.

Автор: Роман Савоченко <roman@oscada.org>, Аркадий Кысиль (2017)
Основано: Июль 2016
Версия: 1.4.0
Лицензия: GPLv2
DOC: Libs_LowLevelDevices|Libs/LowLevelDevices');
CREATE TABLE IF NOT EXISTS 'UserFuncLibs' ("ID" TEXT DEFAULT '' ,"NAME" TEXT DEFAULT '' ,"DESCR" TEXT DEFAULT '' ,"DB" TEXT DEFAULT '' ,"uk#NAME" TEXT DEFAULT '' ,"uk#DESCR" TEXT DEFAULT '' ,"ru#NAME" TEXT DEFAULT '' ,"ru#DESCR" TEXT DEFAULT '' ,"PROG_TR" INTEGER DEFAULT '' , PRIMARY KEY ("ID"));
INSERT INTO UserFuncLibs VALUES('techApp','Technological apparatuses','Library of models of the technological apparatuses for creating complex simulators of the technological processes of block-schemes of the module DAQ.BlockCalc.

The library is created to provide models of devices of the technological processes. The library is not static, but based on the module JavaLikeCalc, allowing to create calculations on the Java-like language.

To address the library functions you can use the static call address DAQ.JavaLikeCalc.lib_techApp.{Func}() or dynamic SYS.DAQ.JavaLikeCalc["lib_techApp"]["{Func}"].call(), SYS.DAQ.JavaLikeCalc["lib_techApp"].{Func}()". Where {Func} — function identifier in the library.

Author: Roman Savochenko <roman@oscada.org>, Maxim Lysenko (2007,2010), Ksenia Yashina (2007)
Founded: October 2005
Sponsored by: DIYA Ltd
Version: 2.0.1
License: GPLv2
DOC: Libs_Technological_apparatuses|Libs/Technological_apparatuses','techApp','Технологічні апарати','Бібліотека моделей технологічних апаратів для створення комплексних моделей технологічних процесів, блокових схем модуля DAQ.BlockCalc.

Бібліотека створюється для надання моделей апаратів технологічних процесів. Бібліотека не є статичною, а будується на основі модуля JavaLikeCalc, який дозволяє створювати обчислення на мові схожій на Java.

Для адресації до функцій цієї бібліотеки можна використати статичну адресу виклику DAQ.JavaLikeCalc.lib_techApp.{Func}() або динамічну SYS.DAQ.JavaLikeCalc["lib_techApp"]["{Func}"].call(), SYS.DAQ.JavaLikeCalc["lib_techApp"].{Func}(). Де {Func} — ідентифікатор функції у бібліотеці.

Автор: Роман Савоченко <roman@oscada.org>, Максим Лисенко (2007,2010), Ксенія Яшина (2007)
Засновано: Жовтень 2005
Спонсоровано: ТОВ "ДІЯ"
Версія: 2.0.1
Ліцензія: GPLv2
DOC: Libs_Technological_apparatuses|Libs/Technological_apparatuses','Технологические аппараты','Библиотека моделей технологических аппаратов для создания комплексных моделей технологических процессов, блочных схем модуля DAQ.BlockCalc.

Библиотека создаётся для предоставления моделей аппаратов технологических процессов. Библиотека не является статической, а строится на основе модуля JavaLikeCalc, позволяющего создавать вычисления на Java-подобном языке.

Для адресации к функции этой библиотеки можно использовать статический адрес вызова DAQ.JavaLikeCalc.lib_techApp.{Func}() или динамический SYS.DAQ.JavaLikeCalc["lib_techApp"]["{Func}"].call(), SYS.DAQ.JavaLikeCalc["lib_techApp"].{Func}(). Где {Func} — идентификатор функции в библиотеке.

Автор: Роман Савоченко <roman@oscada.org>, Максим Лысенко (2007,2010), Ксения Яшина (2007)
Основано: Октябрь 2005
Спонсировано: ООО "ДІЯ"
Версия: 2.0.1
Лицензия: GPLv2
DOC: Libs_Technological_apparatuses|Libs/Technological_apparatuses',0);
INSERT INTO UserFuncLibs VALUES('servProc','Service procedures','Library of the service procedures of different application.

The library is created to provide service functions of performing service tasks about the OpenSCADA environment. The library is not static, but based on the module JavaLikeCalc, allowing to create calculations on the Java-like language. The functions'' names and their parameters are available in languages: English, Ukrainian and Russian.

To addressing the library functions you can use the static call address DAQ.JavaLikeCalc.lib_servProc.{Func}() or dynamic SYS.DAQ.JavaLikeCalc["lib_servProc"]["{Func}"].call(), SYS.DAQ.JavaLikeCalc["lib_servProc"].{Func}()". Where {Func} — function identifier in the library.

Author: Roman Savochenko <roman@oscada.org>
Founded: November 2017
Version: 1.2.1
License: GPLv2
DOC: Libs_Service_procedures|Libs/Service_procedures','lib_servProc','Сервісні процедури','Бібліотека сервісних процедур різноманітного застосування.

Бібліотека створюється для надання сервісних функцій виконання службових завдань довкола оточення OpenSCADA. Бібліотека не є статичною, а будується на основі модуля JavaLikeCalc, який дозволяє створювати обчислення на мові схожій на Java. Назви функцій та їх параметрів доступні на мовах: Англійська, Українська та Російська.

Для адресації до функцій цієї бібліотеки можна використати статичну адресу виклику DAQ.JavaLikeCalc.lib_servProc.{Func}() або динамічну SYS.DAQ.JavaLikeCalc["lib_servProc"]["{Func}"].call(), SYS.DAQ.JavaLikeCalc["lib_servProc"].{Func}(). Де {Func} — ідентифікатор функції у бібліотеці.

Автор: Роман Савоченко <roman@oscada.org>
Засновано: Листопад 2017
Версія: 1.2.1
Ліцензія: GPLv2
DOC: Libs_Service_procedures|Libs/Service_procedures','Сервисные процедуры','Библиотека сервисных процедур различного применения.

Библиотека создаётся для предоставления сервисных функций исполнения служебных задач около окружения OpenSCADA. Библиотека не является статической, а строится на основе модуля JavaLikeCalc, позволяющего создавать вычисления на Java-подобном языке. Названия функций и их параметров доступны на языках: Английский, Украинский и Российский.

Для адресации к функции этой библиотеки можно использовать статический адрес вызова DAQ.JavaLikeCalc.lib_servProc.{Func}() или динамический SYS.DAQ.JavaLikeCalc["lib_servProc"]["{Func}"].call(), SYS.DAQ.JavaLikeCalc["lib_servProc"].{Func}(). Где {Func} — идентификатор функции в библиотеке.

Автор: Роман Савоченко <roman@oscada.org>
Основано: Ноябрь 2017
Версия: 1.2.1
Лицензия: GPLv2
DOC: Libs_Service_procedures|Libs/Service_procedures',1);
INSERT INTO UserFuncLibs VALUES('doc','Reports'' and documents'' library','Provides the combined library of the reports, documents and related functions of common and the industrial automation.

The service functions of the frames of reports and documents are intended to perform the characteristic tasks of obtaining reporting data, for which you need to perform some verification or counting, for example, to check the validity and to summarize. Forming complex data, such as charts embedded in a document, can also be a service function.

To address the library functions you can use the static call address DAQ.JavaLikeCalc.lib_doc.{Func}() or dynamic SYS.DAQ.JavaLikeCalc["lib_doc"]["{Func}"].call(), SYS.DAQ.JavaLikeCalc["lib_doc"].{Func}()". Where {Func} — function identifier in the library.

The element''s names and their parameters are available in languages: English, Ukrainian and Russian. Their source code wrote in human-language independent mode with calls for the translations by the function tr() and the message''s translation also allowed for English, Ukrainian and Russian.

Founded: January 2008
Author: Roman Savochenko <roman@oscada.org>
Version: 2.0.3
License: GPLv2
DOC: Libs_Documents|Libs/Documents','flb_doc','Бібліотека звітів та документів','Надає комбіновану бібліотеку звітів, документів та пов''язаних функцій загальної та промислової автоматизації.

Сервісні функції кадрів звітів та документів призначено для виконання характерних задач отримання звітних даних, для чого треба здійснювати якусь перевірку або підрахунок, наприклад, перевіряти достовірність та підсумовувати. Сервісною функцією може бути й формування складних даних, як то вбудованих у документ діаграм.

Для адресації до функцій цієї бібліотеки можна використати статичну адресу виклику DAQ.JavaLikeCalc.lib_doc.{Func}() або динамічну SYS.DAQ.JavaLikeCalc["lib_doc"]["{Func}"].call(), SYS.DAQ.JavaLikeCalc["lib_doc"].{Func}(). Де {Func} — ідентифікатор функції у бібліотеці.

Назви елементів та їх параметрів доступні на мовах: Англійська, Українська та Російська. Їх вихідний код написано у мово(людська)-незалежному режимі з викликом функції перекладу tr() та переклад цих повідомлень також доступний Англійською, Українською та Російською.

Засновано: січень 2008р
Автор: Роман Савоченко <roman@oscada.org>
Версія: 2.0.3
Ліцензія: GPLv2
DOC: Libs_Documents|Libs/Documents','Библиотека отчётов и документов','Предоставляет комбинированную библиотеку отчётов, документов и связанных функций общей и промышленной автоматизации.

Сервисные функции кадров отчётов и документов предназначены для выполнения характерных задач получения отчётных данных, для чего нужно осуществлять какую-то проверку или подсчёт, например, проверять достоверность и суммировать. Сервисной функцией может быть и формирование сложных данных, вроде встраиваемых в документ диаграмм.

Для адресации к функции этой библиотеки можно использовать статический адрес вызова DAQ.JavaLikeCalc.lib_doc.{Func}() или динамический SYS.DAQ.JavaLikeCalc["lib_doc"]["{Func}"].call(), SYS.DAQ.JavaLikeCalc["lib_doc"].{Func}(). Где {Func} — идентификатор функции в библиотеке.

Названия элементов и их параметров доступны на языках: Английский, Украинский и Российский. Их исходный код написан в языко(человеческий)-независимом режиме с вызовом функции перевода tr() и перевод этих сообщений также доступен Английским, Украинским и Российским.

Основано: январь 2008г
Автор: Роман Савоченко <roman@oscada.org>
Версия: 2.0.3
Лицензия: GPLv2
DOC: Libs_Documents|Libs/Documents',1);
INSERT INTO UserFuncLibs VALUES('regEl','Regulation elements','Regulation elements library of block-schemes of the module DAQ.BlockCalc.

The library is created to provide functions of different control algorithms and it can be used in programmable logic controllers (PLC) based on OpenSCADA for construction control schemes in the module DAQ.BlockCalc. The library is not static, but based on the module JavaLikeCalc, allowing to create calculations on the Java-like language. The functions'' names and its parameters are available in languages: English, Ukrainian and Russian.

To addressing the library functions you can use the static call address DAQ.JavaLikeCalc.lib_regEl.{Func}() or dynamic SYS.DAQ.JavaLikeCalc["lib_regEl"]["{Func}"].call(), SYS.DAQ.JavaLikeCalc["lib_regEl"].{Func}()". Where {Func} — function identifier in the library.

Founded: January 2010
Author: Roman Savochenko <roman@oscada.org>
Sponsored by: DIYA Ltd
Version: 1.0.0
License: GPLv2
DOC: Libs_Regulation_elements|Libs/Regulation_elements','flb_regEl','Елементи регулювання','Бібліотека елементів регулювання блокових схем модуля DAQ.BlockCalc.

Бібліотека створюється для надання функцій різних алгоритмів контролю та вона може бути використана у програмованих логічних контролерах (PLC) заснованих на OpenSCADA для побудови схем контролю у модулі DAQ.BlockCalc. Бібліотека не є статичною, а будується на основі модуля JavaLikeCalc, який дозволяє створювати обчислення на мові схожій на Java. Назви функцій та їх параметрів доступні на мовах: Англійська, Українська та Російська.

Для адресації до функцій цієї бібліотеки можна використати статичну адресу виклику DAQ.JavaLikeCalc.lib_regEl.{Func}() або динамічну SYS.DAQ.JavaLikeCalc["lib_regEl"]["{Func}"].call(), SYS.DAQ.JavaLikeCalc["lib_regEl"].{Func}(). Де {Func} — ідентифікатор функції у бібліотеці.

Засновано: Січень 2010
Автор: Роман Савоченко <roman@oscada.org>
Спонсоровано: ТОВ "ДІЯ"
Версія: 1.0.0
Ліцензія: GPLv2
DOC: Libs_Regulation_elements|Libs/Regulation_elements','Элементы регулирования','Библиотека элементов регулирования блочных схем модуля DAQ.BlockCalc.

Библиотека создаётся для предоставления функций различных алгоритмов контроля и она может быть использована в программированных логических контроллерах (PLC) основанных на OpenSCADA для построения схем контроля в модуле DAQ.BlockCalc. Библиотека не является статической, а строится на основе модуля JavaLikeCalc, позволяющего создавать вычисления на Java-подобном языке. Названия функций и их параметров доступны на языках: Английский, Украинский и Российский.

Для адресации к функции этой библиотеки можно использовать статический адрес вызова DAQ.JavaLikeCalc.lib_regEl.{Func}() или динамический SYS.DAQ.JavaLikeCalc["lib_regEl"]["{Func}"].call(), SYS.DAQ.JavaLikeCalc["lib_regEl"].{Func}(). Где {Func} — идентификатор функции в библиотеке.

Основано: Январь 2010
Автор: Роман Савоченко <roman@oscada.org>
Спонсировано: ООО "ДІЯ"
Версия: 1.0.0
Лицензия: GPLv2
DOC: Libs_Regulation_elements|Libs/Regulation_elements',0);
INSERT INTO UserFuncLibs VALUES('Controller','Controllers','Library of programs of controllers based on JavaLikeCalc.

Author: Roman Savochenko <roman@oscada.org>
Version: 1.1.1
License: GPLv2','lib_Controllers','Контролери','Програми контролерів базованих на JavaLikeCalc.

Автор: Роман Савоченко <roman@oscada.org>
Версія: 1.1.1
Ліцензія: GPLv2','Контроллеры','Программы контроллеров основанных на JavaLikeCalc.

Автор: Роман Савоченко <roman@oscada.org>
Версия: 1.1.1
Лицензия: GPLv2',0);
INSERT INTO UserFuncLibs VALUES('web','XHTML-template','Pages processing functions library for XHTML-template user''s Web-interface.

Author: Roman Savochenko
Version: 0.2.0
License: GPL','flb_web','XHTML-шаблон','Бібліотека функцій обробки сторінок XHTML-шаблонів користувацткого Web-інтерфейсу.

Автор: Роман Савоченко <roman@oscada.org>
Версія: 0.2.0
Ліцензія: GPL','XHTML-шаблон','Библиотека функций обработки страниц XHTML-шаблонов пользовательского Web-интерфейса.

Автор: Роман Савоченко <roman@oscada.org>
Версия: 0.2.0
Лицензия: GPL',0);
CREATE TABLE IF NOT EXISTS 'lib_servProc_io' ("F_ID" TEXT DEFAULT '' ,"ID" TEXT DEFAULT '' ,"NAME" TEXT DEFAULT '' ,"TYPE" INTEGER DEFAULT '' ,"MODE" INTEGER DEFAULT '' ,"DEF" TEXT DEFAULT '' ,"HIDE" INTEGER DEFAULT '' ,"POS" INTEGER DEFAULT '' ,"uk#NAME" TEXT DEFAULT '' ,"uk#DEF" TEXT DEFAULT '' ,"ru#NAME" TEXT DEFAULT '' ,"ru#DEF" TEXT DEFAULT '' , PRIMARY KEY ("F_ID","ID"));
INSERT INTO lib_servProc_io VALUES('procArh','fromarch','From archive',0,0,'Archive.va_ai3_code',0,0,'З архіву','','Из архива','');
INSERT INTO lib_servProc_io VALUES('procArh','toarch','To archive',0,0,'Archive.va_ai3_dP',0,1,'У архів','','В архив','');
INSERT INTO lib_servProc_io VALUES('procArh','begs','Begin time',0,0,'2006-08-08 9:21:56',0,2,'Час початку','','Время начала','');
INSERT INTO lib_servProc_io VALUES('procArh','ends','End time',0,0,'2006-08-08 11:21:55',0,3,'Час закінчення','','Время конца','');
INSERT INTO lib_servProc_io VALUES('crc16','in','Input',0,0,'',0,0,'Вхід','','Вход','');
INSERT INTO lib_servProc_io VALUES('crc16','out','Result',1,1,'',0,1,'Результат','','Результат','');
INSERT INTO lib_servProc_io VALUES('releaseTests','sub','Subsystem (-1:All;0:SYS;1:Security;2:Transport;...',1,0,'-1',0,0,'Підсистема (-1:Всі;0:SYS;1:Безпека;2:Транспорти;...)','','Подсистема (-1:Все;0:SYS;1:Безопасность;2:Транспорты;...)','');
INSERT INTO lib_servProc_io VALUES('releaseTests','rez','Result',0,5,'',0,1,'Результат','','Результат','');
INSERT INTO lib_servProc_io VALUES('crc16','poly','Polynomial (reversion)',1,0,'40961',0,2,'','','','');
INSERT INTO lib_servProc_io VALUES('archPackFStests','tm','Start time (14.03.2015 21:37)',1,0,'1426361839',0,0,'','','','');
INSERT INTO lib_servProc_io VALUES('archPackFStests','per','Period, seconds',1,0,'60',0,1,'','','','');
INSERT INTO lib_servProc_io VALUES('archPackFStests','addr','Archive address',0,0,'test123',0,2,'','','','');
INSERT INTO lib_servProc_io VALUES('archPackFStests','step','Step (0-8), -1 for all',1,0,'-1',0,4,'','','','');
INSERT INTO lib_servProc_io VALUES('archPackFStests','archiver','Archiver address',0,0,'FSArch.1m',0,3,'','','','');
INSERT INTO lib_servProc_io VALUES('wacko2media','ndb','MySQL DB id',0,0,'wiki',0,0,'','','','');
INSERT INTO lib_servProc_io VALUES('wacko2media','wpg','Wiki page',0,0,'HomePageUk/Doc/SQLite',0,1,'','','','');
INSERT INTO lib_servProc_io VALUES('wacko2media','rez','Result',0,6,'',0,2,'Результат','','Результат','');
INSERT INTO lib_servProc_io VALUES('docOffLine','resDir','Results directory',0,0,'/home/roman/ext/work/Projects/OpenSCADA/doc/',0,3,'','','','');
INSERT INTO lib_servProc_io VALUES('docOffLine','pages','Pages list of "{wiki}:{langs}:{dest}"',0,4,'Documents:en,uk,ru:index.html
Documents/Release_0.9:en,uk,ru:Release_0.9.html
Works/Technical_Support/Agreement:en,uk,ru:Technical_Support_Agreement.html
About:en,uk,ru:About.html
Documents/Terms:en,uk,ru:Terms.html
Functions_and_demands:en,uk,ru:Functions_and_demands.html
Works/To_do:en:To_do.html
Documents/FAQ:en,uk,ru:FAQ.html
Documents/Quick_start:en,uk,ru:Quick_start.html
Documents/Program_manual:en,uk,ru:Program_manual.html
Documents/How_to:en,uk,ru:How_to.html
Documents/How_to/Install:en,uk,ru:How_to_Install.html
Documents/How_to/Live_disk:en,uk,ru:How_to_Live_disk.html
Documents/How_to/Violations,_alarms_and_notifications:en,uk,ru:How_to_Violations.html
Documents/How_to/Cyclic_programming:en,uk,ru:How_to_Cyclic_programming.html
Documents/How_to/Debug:en,uk,ru:How_to_Debug.html
Documents/How_to/Transferring_project_configuration:en,uk,ru:How_to_Transferring_project_configuration.html
Documents/How_to/Build_from_source:en,uk,ru:How_to_Build_from_source.html
Documents/How_to/Crash_report:en,uk,ru:How_to_Crash_report.html
Documents/How_to/Create_module:en,uk,ru:How_to_Create_module.html
Documents/DAQ:en,uk,ru:DAQ.html
Documents/User_API:en,uk,ru:User_API.html
Documents/API:en:API.html
Modules/SQLite:en,uk,ru:Modules/SQLite.html
Modules/MySQL:en,uk,ru:Modules/MySQL.html
Modules/FireBird:en,uk,ru:Modules/FireBird.html
Modules/PostgreSQL:en,uk,ru:Modules/PostgreSQL.html
Modules/DBGate:en,uk:Modules/DBGate.html
Modules/DBF:en,uk,ru:Modules/DBF.html
Modules/LDAP:en,uk,ru:Modules/LDAP.html
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
Modules/SoundCard:en,uk,ru:Modules/SoundCard.html
Modules/BFN:en,uk,ru:Modules/BFN.html
Modules/SMH2Gi:en,uk,ru:Modules/SMH2Gi.html
Modules/GPIO:en,uk,ru:Modules/GPIO.html
Modules/FSArch:en,uk,ru:Modules/FSArch.html
Modules/DBArch:en,uk,ru:Modules/DBArch.html
Modules/VCAEngine:en,uk,ru:Modules/VCAEngine.html
Modules/QTStarter:en,uk,ru:Modules/QTStarter.html
Modules/QTCfg:en,uk,ru:Modules/QTCfg.html
Modules/Vision:en,uk,ru:Modules/Vision.html
Modules/WebCfgD:en,uk,ru:Modules/WebCfgD.html
Modules/WebCfg:en,uk,ru:Modules/WebCfg.html
Modules/WebVision:en,uk,ru:Modules/WebVision.html
Modules/WebUser:en,uk,ru:Modules/WebUser.html
Modules/FLibSYS:en,uk,ru:Modules/FLibSYS.html
Modules/SystemTests:en,uk,ru:Modules/SystemTests.html
Modules/FLibMath:en,uk,ru:Modules/FLibMath.html
Modules/FLibComplex1:en,uk,ru:Modules/FLibComplex1.html
Libs/Main:en,uk,ru:Libs_Main.html
Libs/Devices:en,uk,ru:Libs_Devices.html
Libs/LowLevelDevices:en,uk,ru:Libs_LowLevelDevices.html
Libs/Service_procedures:en,uk,ru:Libs_Service_procedures.html
Libs/Regulation_elements:en,uk,ru:Libs_Regulation_elements.html
Libs/Technological_apparatuses:en,uk,ru:Libs_Technological_apparatuses.html
Libs/Main_graphical_elements:en,uk,ru:Libs_Main_graphical_elements.html
Libs/Mnemo_elements:en,uk,ru:Libs_Mnemo_elements.html
Libs/Electrical_elements:en,uk,ru:Libs_Electrical_elements.html
Libs/Documents:en,uk,ru:Libs_Documents.html
Libs/Prescriptions:en,uk,ru:Libs_Prescriptions.html',0,4,'','','','');
INSERT INTO lib_servProc_io VALUES('docOffLine','pagesCur','Current pages list of "{wiki}:{langs}:{dest}", for empty here used "pages"',0,5,'',0,5,'','','','');
INSERT INTO lib_servProc_io VALUES('docOffLine','res','Result',0,1,'',0,0,'Результат','','Результат','');
INSERT INTO lib_servProc_io VALUES('docOffLine','reqTr','Tries of the requests',1,1,'3',0,2,'','','','');
INSERT INTO lib_servProc_io VALUES('releaseTests','rezF','Result for HTML file',0,5,'',1,2,'','','','');
INSERT INTO lib_servProc_io VALUES('docOffLine','prgVer','Program version',0,0,'',0,1,'','','','');
CREATE TABLE IF NOT EXISTS 'tmplib_PrescrTempl_io' ("TMPL_ID" TEXT DEFAULT '' ,"ID" TEXT DEFAULT '' ,"NAME" TEXT DEFAULT '' ,"TYPE" INTEGER DEFAULT '' ,"FLAGS" INTEGER DEFAULT '' ,"VALUE" TEXT DEFAULT '' ,"POS" INTEGER DEFAULT '' ,"ru#NAME" TEXT DEFAULT '' ,"ru#VALUE" TEXT DEFAULT '' ,"uk#NAME" TEXT DEFAULT '' ,"uk#VALUE" TEXT DEFAULT '' , PRIMARY KEY ("TMPL_ID","ID"));
INSERT INTO tmplib_PrescrTempl_io VALUES('timer','run','Command: run',3,32,'0',4,'Команда: исполнение','','Команда: виконання','');
INSERT INTO tmplib_PrescrTempl_io VALUES('timer','pause','Command: pause',3,32,'0',5,'Команда: пауза','','Команда: пауза','');
INSERT INTO tmplib_PrescrTempl_io VALUES('timer','error','Prescription: error',3,32,'0',2,'Рецепт: ошибка','','Рецепт: помилка','');
INSERT INTO tmplib_PrescrTempl_io VALUES('timer','abort','Prescription: abort',3,32,'0',3,'Рецепт: прерывание','','Рецепт: перервано','');
INSERT INTO tmplib_PrescrTempl_io VALUES('timer','rez','Command: result',0,32,'1',6,'Команда: результат','','Команда: результат','');
INSERT INTO tmplib_PrescrTempl_io VALUES('timer','arg1','Time, seconds',2,32,'',7,'Время, секунд','','Час, секунд','');
INSERT INTO tmplib_PrescrTempl_io VALUES('timer','tmp1','Temporary 1',2,1,'0',8,'Временный 1','','Тимчасово 1','');
INSERT INTO tmplib_PrescrTempl_io VALUES('backTimer','run','Command: run',3,32,'0',4,'Команда: исполнение','','Команда: виконання','');
INSERT INTO tmplib_PrescrTempl_io VALUES('backTimer','pause','Command: pause',3,32,'0',5,'Команда: пауза','','Команда: пауза','');
INSERT INTO tmplib_PrescrTempl_io VALUES('backTimer','error','Prescription: error',3,32,'0',2,'Рецепт: ошибка','','Рецепт: помилка','');
INSERT INTO tmplib_PrescrTempl_io VALUES('backTimer','abort','Prescription: abort',3,32,'0',3,'Рецепт: прерывание','','Рецепт: перервано','');
INSERT INTO tmplib_PrescrTempl_io VALUES('backTimer','rez','Command: result',0,32,'1',6,'Команда: результат','','Команда: результат','');
INSERT INTO tmplib_PrescrTempl_io VALUES('backTimer','arg1','Time, seconds',2,32,'',7,'Время, секунд','','Час, секунд','');
INSERT INTO tmplib_PrescrTempl_io VALUES('backTimer','tmp1','Temporary 1',2,1,'0',8,'Временный 1','','Тимчасово 1','');
INSERT INTO tmplib_PrescrTempl_io VALUES('backTimer','start','Prescription: start',3,32,'0',0,'Рецепт: старт','','Рецепт: старт','');
INSERT INTO tmplib_PrescrTempl_io VALUES('backTimer','stop','Prescription: stop',3,32,'0',1,'Рецепт: стоп','','Рецепт: стоп','');
INSERT INTO tmplib_PrescrTempl_io VALUES('timer','start','Prescription: start',3,32,'0',0,'Рецепт: старт','','Рецепт: старт','');
INSERT INTO tmplib_PrescrTempl_io VALUES('timer','stop','Prescription: stop',3,32,'0',1,'Рецепт: стоп','','Рецепт: стоп','');
INSERT INTO tmplib_PrescrTempl_io VALUES('manager','dbDB','DB: DB name with the tables',0,80,'',0,'БД: Имя БД с таблицами','','БД: Ім''я БД з таблицями','');
INSERT INTO tmplib_PrescrTempl_io VALUES('manager','dbComs','DB: Table with commands',0,64,'PrescrComs',1,'БД: Таблица с командами','','БД: Таблиця з командами','');
INSERT INTO tmplib_PrescrTempl_io VALUES('manager','dbProgs','DB: Table with programs',0,80,'PrescrProgs',2,'БД: Таблица с программами','','БД: Таблиця з програмами','');
INSERT INTO tmplib_PrescrTempl_io VALUES('manager','comsCntr','Commands controller,
<empty> - for commands into the table,
''*'' - this parameter''s controller',0,64,'*',3,'Контроллер команд,
<пусто> - для команд в таблице,
''*'' - этот параметр контроллер','','Контролер команд,
<порожньо> - для команд у таблиці,
''*'' - цей параметр контролеру','');
INSERT INTO tmplib_PrescrTempl_io VALUES('manager','mode','Mode',1,40,'-2
-2;-1;0;1;2;3
Finish;Error;Stop;Run;Pause;Pass command',4,'Режим','-2
-2;-1;0;1;2;3
Завершение;Ошибка;Стоп;Исполнение;Пауза;Пропустить команду','Режим','-2
-2;-1;0;1;2;3
Завершення;Помилка;Стоп;Виконання;Пауза;Пропустити команду');
INSERT INTO tmplib_PrescrTempl_io VALUES('manager','curMode','Current mode',1,24,'-2
-2;-1;0;1;2;3
Finish;Error;Stop;Run;Pause;Pass command',5,'Текущий режим','-2
-2;-1;0;1;2;3
Завершение;Ошибка;Стоп;Исполнение;Пауза;Пропустить команду','Поточний режим','-2
-2;-1;0;1;2;3
Завершення;Помилка;Стоп;Виконання;Пауза;Пропустити команду');
INSERT INTO tmplib_PrescrTempl_io VALUES('manager','prog','Name of the selected program',0,32,'',6,'Имя выбранной программы','','Ім''я обраної програми','');
INSERT INTO tmplib_PrescrTempl_io VALUES('manager','startTm','Start time, seconds',1,16,'',7,'Время запуска, секунды','','Час запуску, секунди','');
INSERT INTO tmplib_PrescrTempl_io VALUES('manager','curCom','Current command-step',0,16,'',8,'Текущая комманда-шаг','','Поточна команда-крок','');
INSERT INTO tmplib_PrescrTempl_io VALUES('manager','work','Work program',4,16,'',9,'Рабочая программа','','Робоча програма','');
INSERT INTO tmplib_PrescrTempl_io VALUES('manager','comLs','Commands list',4,16,'',10,'Список команд','','Перелік команд','');
INSERT INTO tmplib_PrescrTempl_io VALUES('manager','clcCnt','Cycles counter',1,0,'',11,'Счётчик циклов','','Лічильник циклів','');
INSERT INTO tmplib_PrescrTempl_io VALUES('manager','this','The object',4,0,'',12,'Объект','','Об''єкт','');
CREATE TABLE IF NOT EXISTS 'DAQ_JavaLikeCalc' ("ID" TEXT DEFAULT '' ,"NAME" TEXT DEFAULT '' ,"ru#NAME" TEXT DEFAULT '' ,"uk#NAME" TEXT DEFAULT '' ,"DESCR" TEXT DEFAULT '' ,"ru#DESCR" TEXT DEFAULT '' ,"uk#DESCR" TEXT DEFAULT '' ,"ENABLE" INTEGER DEFAULT '0' ,"START" INTEGER DEFAULT '0' ,"MESS_LEV" INTEGER DEFAULT '3' ,"REDNT" INTEGER DEFAULT '0' ,"REDNT_RUN" TEXT DEFAULT '<high>' ,"PRM_BD" TEXT DEFAULT 'system' ,"FUNC" TEXT DEFAULT '' ,"SCHEDULE" TEXT DEFAULT '1' ,"PRIOR" INTEGER DEFAULT '0' ,"ITER" INTEGER DEFAULT '1' , PRIMARY KEY ("ID"));
CREATE TABLE IF NOT EXISTS 'DAQ_LogicLev' ("ID" TEXT DEFAULT '' ,"NAME" TEXT DEFAULT '' ,"ru#NAME" TEXT DEFAULT '' ,"uk#NAME" TEXT DEFAULT '' ,"DESCR" TEXT DEFAULT '' ,"ru#DESCR" TEXT DEFAULT '' ,"uk#DESCR" TEXT DEFAULT '' ,"ENABLE" INTEGER DEFAULT '0' ,"START" INTEGER DEFAULT '0' ,"MESS_LEV" INTEGER DEFAULT '3' ,"REDNT" INTEGER DEFAULT '0' ,"REDNT_RUN" TEXT DEFAULT '<high>' ,"PRM_BD" TEXT DEFAULT '' ,"PRM_BD_REFL" TEXT DEFAULT '' ,"PERIOD" INTEGER DEFAULT '0' ,"SCHEDULE" TEXT DEFAULT '1' ,"PRIOR" INTEGER DEFAULT '0' , PRIMARY KEY ("ID"));
CREATE TABLE IF NOT EXISTS 'WebUser_uPg' ("ID" TEXT DEFAULT '' ,"NAME" TEXT DEFAULT '' ,"uk#NAME" TEXT DEFAULT '' ,"DESCR" TEXT DEFAULT '' ,"uk#DESCR" TEXT DEFAULT '' ,"EN" INTEGER DEFAULT '0' ,"PROG" TEXT DEFAULT '' ,"uk#PROG" TEXT DEFAULT '' ,"TIMESTAMP" INTEGER DEFAULT '' ,"ru#DESCR" TEXT DEFAULT '' ,"ru#NAME" TEXT DEFAULT '' , PRIMARY KEY ("ID"));
INSERT INTO WebUser_uPg VALUES('tmpl','XHTML-template','XHTML-шаблон','Site from XHTML template.','Сайт із XHTML шаблону',1,'JavaLikeCalc.JavaScript
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
CREATE TABLE IF NOT EXISTS 'tmplib_DevLib' ("ID" TEXT DEFAULT '' ,"NAME" TEXT DEFAULT '' ,"uk#NAME" TEXT DEFAULT '' ,"ru#NAME" TEXT DEFAULT '' ,"DESCR" TEXT DEFAULT '' ,"uk#DESCR" TEXT DEFAULT '' ,"ru#DESCR" TEXT DEFAULT '' ,"MAXCALCTM" INTEGER DEFAULT '10' ,"PR_TR" INTEGER DEFAULT '1' ,"PROGRAM" TEXT DEFAULT '' ,"uk#PROGRAM" TEXT DEFAULT '' ,"ru#PROGRAM" TEXT DEFAULT '' ,"TIMESTAMP" INTEGER DEFAULT '' , PRIMARY KEY ("ID"));
INSERT INTO tmplib_DevLib VALUES('SCU750','EDWARDS TURBOMOLECULAR PUMPS','','','Typical EDWARDS TURBOMOLECULAR PUMPS (http://edwardsvacuum.com) data acquisition by SCU750 Cotrol Unit protocol.

Author: Roman Savochenko <roman@oscada.org>
Sponsored: Vasiliy Grigoriev from "Vacuum technologies laboratory (http://e-beam.ru)".
Version: 1.0.0
License: GPLv2','','',10,0,'JavaLikeCalc.JavaScript
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

f_err = t_err;','','',1580047532);
INSERT INTO tmplib_DevLib VALUES('TMH','Power supply for turbomolecular pumps','','','Power supply for turbomolecular pumps of firm SHIMADZU (http://www.shimadzu.com), model EI-R04M.

Author: Roman Savochenko <roman@oscada.org>
Sponsored: Vasiliy Grigoriev from "Vacuum technologies laboratory (http://e-beam.ru)".
Version: 1.0.0
License: GPLv2','','',10,0,'JavaLikeCalc.JavaScript
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

f_err = t_err;','','',1580047532);
INSERT INTO tmplib_DevLib VALUES('TM510x','Elemer TM510x','','','Multi-channels thermometer Elemer TM5102 and TM5103 of the firm Elemer (http://www.elemer.ru).

Author: Roman Savochenko <roman@oscada.org>
Version: 1.0.0
License: GPLv2','','',10,0,'JavaLikeCalc.JavaScript
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

f_err = t_err;','','',1580047532);
INSERT INTO tmplib_DevLib VALUES('UPS','UPS (moved)','','','!!!!: Moved to the template library Main. Will be removed soon
Uninterruptible power supply unifying data for provide all the data into single attribute of object type "All items" for next the control as the object with the data provide as table, alarming and allowing set writable attributes. The template aimed for using with module "System" data source as "UPS" and widget "Main.objProps" as the data presenter. The template also you can use as example for create like other data unification as complex object with properties, alarming and writing.

Author: Roman Savochenko <roman@oscada.org>
Version: 1.2.1','','',10,0,'JavaLikeCalc.JavaScript
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
if(tErr.toInt() && tErr.toInt() != f_err.toInt())	this.alarmSet(DESCR+": "+tErr.parse(1,":"), -(2+alLev));
else if(f_err.toInt() && !tErr.toInt())			this.alarmSet(DESCR+": "+tr("NORM"), 1);
f_err = tErr;
conDelay_ = 0;','','',1561317269);
INSERT INTO tmplib_DevLib VALUES('VKT7','VKT-7','','','Firm "Teplocom" (http://www.teplocom.spb.ru) computer "VKT-7" for complex heat measurement and counting. The device complex enough to provide more parameters, more history and accessed by a nonlinear Serial-based protocol at low speed. The template implements acquisition for all significant parameters, gets for their history by hours, days and result months. Also you can to append easily enough for processing of the remained parameters.

Author: Roman Savochenko <roman@oscada.org>
Sponsored: Vladislav Chubuk
Version: 1.0.0
License: GPLv2','','',60,0,'JavaLikeCalc.JavaScript
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
else f_err = errAttrs.length ? "11:"+tr("Quality errors")+": "+errAttrs : "0";','','',1580047532);
INSERT INTO tmplib_DevLib VALUES('SMDP','Sycon Multi Drop Protocol','','','STM devices for resonant frequency measurement for mass of deposited films attached to their surface by Sycon Instruments, Inc. (http://www.sycon.com).

Author: Roman Savochenko <roman@oscada.org>
Sponsored: Vasiliy Grigoriev from "Vacuum technologies laboratory (http://e-beam.ru)".
Version: 1.0.0
License: GPLv2','','',10,0,'JavaLikeCalc.JavaScript
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

f_err = t_err.length ? t_err : "0";','','',1580047251);
INSERT INTO tmplib_DevLib VALUES('IT3','Temperature measurement IT-3','','','Temperature measurement IT-3 from OmskEtalon (http://www.omsketalon.ru).

Author: Roman Savochenko <roman@oscada.org>
Sponsored: Vasiliy Grigoriev from "Vacuum technologies laboratory (http://e-beam.ru)".
Version: 1.2.0
License: GPLv2','','',10,0,'JavaLikeCalc.JavaScript
if(mdPass)	{
	if(f_start) { f_err = EVAL; this.cnt.set(0, 0, 0, true); cnt_ = 0; tDl = 5; }
	if((cnt=this.cnt.get()) != cnt_)	{ tDl = 5; cnt_ = cnt; }
	else if(tDl > 0 && (tDl=tDl-1/f_frq) <= 0) {
		this.err.set("1:"+tr("No data"), 0, 0, true);
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
else f_err = "0";','','',1580046256);
INSERT INTO tmplib_DevLib VALUES('IVE_452HS_02','IVE-452HS-02','','','Power supply of beam-electrons evaporator of "Plasma Tech" Ltd, from Moskov.

Author: Roman Savochenko <roman@oscada.org>
Sponsored: Vasiliy Grigoriev from "Vacuum technologies laboratory (http://e-beam.ru)".
Version: 1.0.0
License: GPLv2','','',10,0,'JavaLikeCalc.JavaScript
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
else f_err = "0";','','',1580046355);
INSERT INTO tmplib_DevLib VALUES('OPTRIS','OPTRIS CT/CTL','','','OPTRIS CT/CTL communication interface of "Optris" GmbH, from Berlin.

Author: Roman Savochenko <roman@oscada.org>
Sponsored: Vasiliy Grigoriev from "Vacuum technologies laboratory (http://e-beam.ru)".
Version: 1.0.0
License: GPLv2','','',10,0,'JavaLikeCalc.JavaScript
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
else f_err = "0";','','',1580047532);
INSERT INTO tmplib_DevLib VALUES('CTR','CTR 100, 101','','','The RS232C Serial Interface permits the communication between the digital Oerlikon Leybold Vacuum CERAVAC, from Köln.

Author: Roman Savochenko <roman@oscada.org>
Sponsored: Vasiliy Grigoriev from "Vacuum technologies laboratory (http://e-beam.ru)".
Version: 1.0.0
License: GPLv2','','',10,0,'JavaLikeCalc.JavaScript
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
else f_err = "0: " + u_err;','','',1580045150);
INSERT INTO tmplib_DevLib VALUES('IEC60870','IEC-60870','','','IEC 60870 part 5 is one from the IEC 60870 set of standards which defines systems used for telecontrol (supervisory control and data acquisition) in electrical engineering and power system automation applications. Part 5 provides a communication profile for sending basic telecontrol messages between two systems, which uses permanent directly connected data circuits between the systems.

The template currently implements the part 104 (Ethernet transport) for the client and followed services: STARTDT, STOPDT, TESTFR, Ack, C_IC_NA_1 (100), C_CI_NA_1 (101), C_SC_NA_1 (45), M_SP_NA_1 (1), M_SP_TA_1 (2), M_DP_NA_1 (3), M_DP_TA_1 (4), M_ME_NB_1 (11), M_ME_NC_1 (13), M_ME_TF_1 (36), C_CS_NA_1 (103). For information ones, the sequential (SQ) mode enabling and disabling are supported.

For the direct definition of the requested data, the "itemsSet" attribute is provided, where they can be defined individually, with a semantic identifier and name, or in a range named by a template. Often, remote systems may not need to determine the data to transmit and send existing or generic ones on their own initiative, and if they do not, have and need to ask for something additional ones, then you need to get a map of the available parameters with the addresses these you enter in this attribute in the format ai|di|do:{IOA}[-{EndIOA}][:a[:{NameBase}]], where:
- ai|di|do — data type, where:
  - "ai" — analog inputs, transmitted by the message M_ME_NB_1, M_ME_NC_1, M_ME_TF_1 and requested by C_SC_NA_1;
  - "di" — discrete inputs, transmitted by the message M_SP_NA_1, M_SP_TA_1, M_DP_NA_1, M_DP_TA_1 and requested by C_SC_NA_1;
  - "do" — discrete outputs, transmitted by C_SC_NA_1.
- {IOA} — parameter address of the range begin;
- {EndIOA} — the range end (the last item address);
- a — sign of creating the representative attribute for the parameter or the parameters group;
- {NameBase} — naming base of the attributes.

Examples of the parameters definition, rows of the attribute "itemsSet":
- di:100-108:a — discrete inputs in the address range [100...108] with forming the attributes;
- ai:873-880:a — analog inputs in the address range [873...880] with forming the attributes;
- do:1-10:a — discrete outputs in the address range [1...10] with forming the attributes.

An object into the attribute "items" is used to accumulate the gathered and controlling data, to next control as an object with the data represent in a table of the widget "Main.objProps", alarming and allowing to set the writable attributes.

The module provides integrated support for time synchronization of PLC by sending the C_CS_NA_1 service command.

In the template for the first time used the not requesting mode of an output transport and the attributes creation in free while performing.

Author: Roman Savochenko <roman@oscada.org>
Total complexity: 3.8 HD
Sponsored by: Ustijancev Michael for 3.0 HD, SVItoVYR Ltd for 0.8 HD
Version: 1.4.0
License: GPLv2','IEC 60870 у частині 5 є одним з набору стандартів IEC 60870 який визначає системи, що використовуються у віддаленому контролі (телемеханіці — диспетчерському контролі та зборі даних) у інженерній електриці та у застосунках автоматизації енергетичних систем. Частина 5 надає комунікаційний профіль для надсилання базових повідомлень віддаленого контролю між двома системами, який використовує постійні прямі підключення даних між системами.

Шаблон наразі реалізує частину 104 (Ethernet транспорт) для клієнту та наступних сервісів: STARTDT, STOPDT, TESTFR, Ack, C_IC_NA_1 (100), C_CI_NA_1 (101), C_SC_NA_1 (45), M_SP_NA_1 (1), M_ME_NB_1 (11), M_ME_NC_1 (13), M_ME_TF_1 (36), C_CS_NA_1 (103). Для інформаційних сервісів підтримується ввімкнення та вимкнення послідовного (SQ) режиму.

Для прямого визначення запитуваних даних передбачено атрибут "itemsSet", де вони можуть визначатися як окремо, зі смисловим ідентифікатором та назвою, або за діапазоном із найменуванням по шаблону. Часто віддалені системи можуть не потребувати визначення даних для передачі та надсилають наявні або основні зі своєї ініціативи та якщо вони цього не роблять, мають та потрібно запитати щось додаткове то вам треба отримати мапу наявних параметрів з адресами які й ввести у цьому атрибуті записами у форматі ai|di|do:{IOA}[-{EndIOA}][:a[:{NameBase}]], де:
- ai|di|do — тип даних, де:
  - "ai" — аналогові входи, передані повідомленням M_ME_NB_1, M_ME_NC_1, M_ME_TF_1 та запитані C_SC_NA_1;
  - "di" — дискретні входи, передані повідомленням M_SP_NA_1 та запитані C_SC_NA_1;
  - "do" — дискретні виходи, передавані командою C_SC_NA_1.
- {IOA} — адреса параметру або початок діапазону;
- {EndIOA} — кінець (адреса останнього елементу) діапазону;
- a — ознака створення репрезентативних атрибутів для параметру або групи параметрів;
- {NameBase} — база назви атрибуту(ів).

Приклади визначення параметрів, рядки атрибуту "itemsSet":
- di:100-108:a — дискретні входи за діапазоном адрес [100...108] із формуванням атрибутів;
- ai:873-880:a — аналогові входи за діапазоном адрес [873...880] із формуванням атрибутів;
- do:1-10:a — дискретні виходи за діапазоном адрес [1...10] із формуванням атрибутів.

Об''єкт у атрибуті "items" використовується для акумулювання зібраних та контрольованих даних задля подальшого контрою як об''єкт із представленням даних у таблиці віджету "Main.objProps", сигналізуючи та дозволяючи встановлювати записувані атрибути.

Модулем надається вбудована підтримка синхронізації часу ПЛК, надсиланням сервісної команди C_CS_NA_1.

У шаблоні вперше використано не запитувальний режим вихідного транспорту та вільне створення атрибутів під час виконання.

Автор: Роман Савоченко <roman@oscada.org>
Загальна працемісткість: 3.8 ЛД
Спонсорування: Устьянцев Михайло на 3.0 ЛД, ТОВ "СВІТоВИР" на 0.8 ЛД
Версія: 1.3.1
Ліцензія: GPLv2','',10,0,'JavaLikeCalc.JavaScript
function getTm(iSq) {
	ttm_ms = iSq.charCodeAt(1)*256 + iSq.charCodeAt(0);
	ttm = SYS.mktime(ttm_ms/1000, iSq.charCodeAt(2)&0x3F, iSq.charCodeAt(3)&0x1F, iSq.charCodeAt(4)&0x1F, (iSq.charCodeAt(5)&0x0F)-1, 2000+(iSq.charCodeAt(6)&0x7F));
	return ttm + ttm_ms*1e-3;
}

function inVal(aId, aVal, SIQ, wr, itm) {
	if(!SIQ.isEVal() && (SIQ&0xF0)) aVal = EVAL;	else SIQ = 0;
	if(wr.isEVal())	wr = false;
	if(itm.isEVal())	itm = 0;
	if(items[aId].isEVal()) {
		items[aId] = itW = new Object();
		itW.descr = aId;
		itW.wr = wr; itW.alarm = 0;
	} else itW = items[aId];
	itW.val = aVal;
	itW.SIQ = SIQ;	//For a specific quality processing
	if((aO=this[aId])) {
		aO.set(aVal, 0, 0, true);
		if(itm && aO.arch())	aO.arch().setVal(1e6*itm, aVal);
		if(itW.wr) oAVals[aId] = aVal;
	}
}

if(f_start)	{
	transport_ = "", tr = false;
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
	return;
}
if(f_stop && !STARTDT_con)	return;	//But at stop we must yet send STOPDT

//Items set changing process
if(itemsSet != itemsSet_) {
	for(off = 0; (iIt=itemsSet.parse(0,"\n",off)).length; ) {
		iIt_tp = iIt.parse(0, ":");
		iIt_IOA = iIt.parse(1, ":");
		iIt_flgs = iIt.parse(2, ":");
		iIt_nmBase = iIt.parse(3, ":");
		iIt_sDscr = false;
		if((iIt_EndIOA=iIt_IOA.indexOf("-")) >= 0) {
			iIt_EndIOA = iIt_IOA.slice(iIt_EndIOA+1).toInt(0);
			iIt_IOA = iIt_IOA.slice(0,iIt_EndIOA).toInt(0);
		}
		else { iIt_IOA = iIt_EndIOA = iIt_IOA.toInt(0); iIt_sDscr = iIt_nmBase.length; }
		if(iIt_tp == "ai")			{ iIt_nmBase = iIt_nmBase.length ? iIt_nmBase : "AI"; iIt_vtp = "real,ro"; }
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
		if(bufIn.length < 6 || (bufIn.length-2) < seqSz)	break;		//Not full, wait

		SYS.messDebug("/IEC60870",tr("Good input sequence")+": "+SYS.strDecode(bufIn.slice(0,seqSz+2),"Bin"," "));

		cntRx_ = -1;
		sB1 = bufIn.charCodeAt(2);
		if((sB1&0x3) == 0x3 && (sB1&0x40))			TESTFR_Act = true;	//U (TESTFR, Act)
		else if((sB1&0x3) == 0x3 && (sB1&0x80))	t1__ = 0;				//U (TESTFR, Con)
		else if((sB1&0x3) == 0x3 && (sB1&0x08)) {								//U (STARTDT, Con)
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
			ASDU_reas = bufIn.charCodeAt(8);			//7=TEST, 6=P/N, 0...5=number
			ASDU_addr = bufIn.charCodeAt(9);
			ASDU_OA = bufIn.charCodeAt(11)*256 + bufIn.charCodeAt(10);
			//destAddr = ASDU_addr, destOA = ASDU_OA;
			isSQ = ASDU_els&0x80;
			ASDU_els = ASDU_els&0x7F;

			cntRx = (cntRx+1)&0x7FFF;
			t2_ = 1/f_frq;	
			w_++;

			if(ASDU_id == 100) {
				if(ASDU_reas == 7)			C_IC_NA_1con = true;	//ActCon
				else if(ASDU_reas == 10)	C_IC_NA_1 = tmPoolAll;//-1;// 1;	//ActTerm, repeate after {tmPoolAll} second
			}
			else if(ASDU_id == 101) {
				if(ASDU_reas == 7)			C_CI_NA_1con = true;	//ActCon
				//else if(ASDU_reas == 10)	C_CI_NA_1 = false;	//ActTerm, repeate after some timeout
			}
			else if(ASDU_id == 103 && ASDU_reas == 7)	C_CS_NA_1con = true;
			else if(ASDU_id == 1) {	//M_SP_NA_1, Single-point information
				if((10+4+(ASDU_els-1)*(isSQ?1:4)) != seqSz)	SYS.messDebug("/IEC60870","M_SP_NA_1: "+tr("Items number is discrepancy to the package size"));
				else for(iEl = 0, iOff = 12; iEl < ASDU_els; iEl++, iOff += 1) {
					if(!isSQ || !iEl)	{ IOA = (bufIn.charCodeAt(iOff+2)<<16) + (bufIn.charCodeAt(iOff+1)<<8) + bufIn.charCodeAt(iOff); iOff += 3; }
					else IOA++;
					inVal("di"+IOA, (SIQ=bufIn.charCodeAt(iOff))&0x01, SIQ);
				}
			}
			else if(ASDU_id == 2) {	//M_SP_TA_1, Single-point information with timestamp
				if((10+4+(ASDU_els-1)*(isSQ?8:11)) != seqSz)	SYS.messDebug("/IEC60870","M_SP_TA_1: "+tr("Items number is discrepancy to the package size"));
				else for(iEl = 0, iOff = 12; iEl < ASDU_els; iEl++, iOff += 8) {
					if(!isSQ || !iEl)	{ IOA = (bufIn.charCodeAt(iOff+2)<<16) + (bufIn.charCodeAt(iOff+1)<<8) + bufIn.charCodeAt(iOff); iOff += 3; }
					else IOA++;
					inVal("di"+IOA, (SIQ=bufIn.charCodeAt(iOff))&0x01, SIQ, false, getTm(bufIn.slice(iOff+1,iOff+8)));
				}
			}
			else if(ASDU_id == 3) {	//M_DP_NA_1, Two-element information
				if((10+4+(ASDU_els-1)*(isSQ?1:4)) != seqSz)	SYS.messDebug("/IEC60870","M_DP_NA_1: "+tr("Items number is discrepancy to the package size"));
				else for(iEl = 0, iOff = 12; iEl < ASDU_els; iEl++, iOff += 1) {
					if(!isSQ || !iEl)	{ IOA = (bufIn.charCodeAt(iOff+2)<<16) + (bufIn.charCodeAt(iOff+1)<<8) + bufIn.charCodeAt(iOff); iOff += 3; }
					else IOA++;
					inVal("di"+IOA, (((SIQ=bufIn.charCodeAt(iOff))&0x3)==0x3||(SIQ&0x3)==0)?EVAL:(SIQ&0x2)?true:false, SIQ);
				}
			}
			else if(ASDU_id == 4) {	//M_DP_TA_1, Two-element information with timestamp
				if((10+4+(ASDU_els-1)*(isSQ?8:11)) != seqSz)	SYS.messDebug("/IEC60870","M_DP_TA_1: "+tr("Items number is discrepancy to the package size"));
				else for(iEl = 0, iOff = 12; iEl < ASDU_els; iEl++, iOff += 8) {
					if(!isSQ || !iEl)	{ IOA = (bufIn.charCodeAt(iOff+2)<<16) + (bufIn.charCodeAt(iOff+1)<<8) + bufIn.charCodeAt(iOff); iOff += 3; }
					else IOA++;
					inVal("di"+IOA, (((SIQ=bufIn.charCodeAt(iOff))&0x3)==0x3||(SIQ&0x3)==0)?EVAL:(SIQ&0x2)?true:false, SIQ, false, getTm(bufIn.slice(iOff+1,iOff+8)));
				}
			}
			else if(ASDU_id == 11) {	//M_ME_NB_1, Measured value, scaled value
				if((10+6+(ASDU_els-1)*(isSQ?3:6)) != seqSz)	SYS.messDebug("/IEC60870","M_ME_NB_1: "+tr("Items number is discrepancy to the package size"));
				else for(iEl = 0, iOff = 12; iEl < ASDU_els; iEl++, iOff += 3) {
					if(!isSQ || !iEl)	{ IOA = (bufIn.charCodeAt(iOff+2)<<16) + (bufIn.charCodeAt(iOff+1)<<8) + bufIn.charCodeAt(iOff); iOff += 3; }
					else IOA++;
					val = (bufIn.charCodeAt(iOff+1)<<8) + bufIn.charCodeAt(iOff);
					if(val > 32767) val -= 65536;
					inVal("ai"+IOA, val, bufIn.charCodeAt(iOff+2));
				}
			}
			else if(ASDU_id == 13) {	//M_ME_NC_1, Measured value, short real value
				if((10+8+(ASDU_els-1)*(isSQ?5:8)) != seqSz)	SYS.messDebug("/IEC60870","M_ME_NC_1: "+tr("Items number is discrepancy to the package size"));
				else for(iEl = 0, iOff = 12; iEl < ASDU_els; iEl++, iOff += 5) {
					if(!isSQ || !iEl)	{ IOA = (bufIn.charCodeAt(iOff+2)<<16) + (bufIn.charCodeAt(iOff+1)<<8) + bufIn.charCodeAt(iOff); iOff += 3; }
					else IOA++;
					inVal("ai"+IOA, Special.FLibSYS.floatMergeWord(bufIn.charCodeAt(iOff+1)*256+bufIn.charCodeAt(iOff),bufIn.charCodeAt(iOff+3)*256+bufIn.charCodeAt(iOff+2)), bufIn.charCodeAt(iOff+4));
				}
			}
			else if(ASDU_id == 36) {	//M_ME_TF_1, Measured value, short real value with timestamp
				if((10+15+(ASDU_els-1)*(isSQ?12:15)) != seqSz)	SYS.messDebug("/IEC60870","M_ME_TF_1: "+tr("Items number is discrepancy to the package size"));
				else for(iEl = 0, iOff = 12; iEl < ASDU_els; iEl++, iOff += 12) {
					if(!isSQ || !iEl)	{ IOA = (bufIn.charCodeAt(iOff+2)<<16) + (bufIn.charCodeAt(iOff+1)<<8) + bufIn.charCodeAt(iOff); iOff += 3; }
					else IOA++;
					inVal("ai"+IOA, Special.FLibSYS.floatMergeWord(bufIn.charCodeAt(iOff+1)*256+bufIn.charCodeAt(iOff),bufIn.charCodeAt(iOff+3)*256+bufIn.charCodeAt(iOff+2)), bufIn.charCodeAt(iOff+4),
						false, getTm(bufIn.slice(iOff+5,iOff+12)));
				}
			}
			else if(ASDU_id == 45) {	//C_SC_NA_1, Single-point confirm
				if((10+4+(ASDU_els-1)*(isSQ?1:4)) != seqSz)	SYS.messDebug("/IEC60870", "C_SC_NA_1: "+tr("Items number is discrepancy to the package size"));
				else for(iEl = 0, iOff = 12; iEl < ASDU_els; iEl++, iOff += 1) {
					if(!isSQ || !iEl)	{ IOA = (bufIn.charCodeAt(iOff+2)<<16) + (bufIn.charCodeAt(iOff+1)<<8) + bufIn.charCodeAt(iOff); iOff += 3; }
					else IOA++;
					inVal("do"+IOA, (SIQ=bufIn.charCodeAt(iOff))&0x01, SIQ);
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
			else if(syncTimePLC && !C_CS_NA_1) {
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

	if(t1_)	t1_ += 1/f_frq;
	if(t1__)	t1__ += 1/f_frq;
	if(t2_)	t2_ += 1/f_frq;
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
else f_err = "0";','','',1592158535);
INSERT INTO tmplib_DevLib VALUES('SSCP','Shark Slave Communication Protocol','','','Shark Slave Communication Protocol from EnergoCentrum PLUS, s.r.o.

Author: Roman Savochenko <roman@oscada.org>
Sponsored: Costumer Faster CZ (http://faster.cz)
Version: 0.6.2
License: GPLv2','','',30,0,'JavaLikeCalc.JavaScript
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

f_err = t_err;','','',1580047300);
INSERT INTO tmplib_DevLib VALUES('m200','Mercury 200','Меркурій 200','Меркурий 200','One phase counter of electricity Mercury 200, 203.2Т, 206 from the firm "Incotex" (http://www.incotexcom.ru)

Author: Arsen Zakojan <godzilla919@gmail.com>
Version: 1.0.0
License: GPLv2','Однофазні лічильники електроенергії Меркурій 200,  203.2Т,  206 компанії Інкотекс (http://www.incotexcom.ru).
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
P =Special.FLibSYS.str2real( req.text().slice(27,29)+req.text().slice(30,32)+req.text().slice(33,35));}','','',1580047532);
INSERT INTO tmplib_DevLib VALUES('m230','Mercury 230','Меркурій 230','Меркурий 230','Three phase counter of electricity Mercury 230, 231, 232, 233, 234, 236 from the firm "Incotex" (http://www.incotexcom.ru).

Author: Arsen Zakojan <godzilla919@gmail.com>
Version: 1.0.0
License: GPLv2','Трифазні лічильники електроенергії Меркурій 230, 231, 232, 233,  234,  236  компанії Інкотекс (http://www.incotexcom.ru).
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
','','',1580047532);
INSERT INTO tmplib_DevLib VALUES('Nik2303I','Nik2303I','','','Three phase counter of electricity NIK 2303 from firm NIK LLC (http://www.nik.net.ua).

Author: Ruslan Yarmoliuk <rylio74@gmail.com>
Version: 1.0.0
License: GPLv2','','',120,0,'JavaLikeCalc.JavaScript
using Special.FLibSYS;

if(f_start) {
	transport_ = transport;
	tr = SYS.Transport.nodeAt(transport,".");
	LLC = SYS.strFromCharCode(0xe6, 0xe6, 0x00);
	F_id = SYS.strFromCharCode(0xC0, 0x01, 0x81, 0x00, 0x03);
	end = SYS.strFromCharCode(0x02, 0x00);
	Ae=Ae_=Re=Re_=Ae1=Ae_1=Re1=Re_1=Ae2=Ae_2=Re2=Re_2=Ae3=Ae_3=Re3=Re_3=EVAL_REAL;
	U1=U2=U3=I1=I2=I3=Psum=P1=P2=P3=Qsum=Q1=Q2=Q3=kP1=kP2=kP3=EVAL_REAL;
	V12=V13=EVAL_INT;

}
if(f_stop) {
	return;
}
 
tErr="0";
//Check for the transport change and connect
if(!tr || transport != transport_)	{
	tr = SYS.Transport.nodeAt(transport,".");
	transport_ = transport;
}
tr.start(true, 0);
/* timeout=SYS.time();
	while(!tr.start()){
		if(SYS.time()>timeout+5)break;
	}	
*/	
if(!tr.start()){ tErr = "1:"+tr("Output transport ''%1'' error.").replace("%1",transport);
			Ae=Ae_=Re=Re_=Ae1=Ae_1=Re1=Re_1=U1=U2=U3=I1=I2=I3=Psum=P1=P2=P3=Qsum=Q1=Q2=Q3=kP1=kP2=kP3=EVAL_REAL;
			V12=V13=EVAL_INT;
}else{
	req = SYS.XMLNode("mess").setAttr("ProtIt","NIK2303").setAttr("SN",serial.toInt(10));
	//COMAND_SNRM
	req.setAttr("cntr",0x93).setText("");
	tr.messIO(req, "UserProtocol");
	if (req.attr("err").toInt()){SYS.messDebug("/NIK2303I/TMPL",tr("Error response")+": "+req.attr("err"));
		Ae=Ae_=Re=Re_=Ae1=Ae_1=Re1=Re_1=U1=U2=U3=I1=I2=I3=Psum=P1=P2=P3=Qsum=Q1=Q2=Q3=kP1=kP2=kP3=EVAL_REAL;
		V12=V13=EVAL_INT;
		return;}
	//COMAND_AARQ (передача пароля)	
	mess = SYS.strFromCharCode(0xa1, 0x09, 0x06, 0x07, 0x60,
		0x85, 0x74, 0x05, 0x08, 0x01, 0x01, 0x8a, 0x02,
		0x07, 0x80, 0x8b, 0x07, 0x60, 0x85, 0x74, 0x05,
		0x08, 0x02, 0x01, 0xac, 0x12, 0x80, 0x11, 0x00)
		+ passw + SYS.strFromCharCode(0xbe, 0x10, 0x04,	
		0x0e, 0x01, 0x00, 0x00, 0x00, 0x06, 0x5f, 0x1f,
		0x04, 0x00, 0x00, 0x08, 0xcb, 0x00, 0x80 );
	req.setAttr("cntr",0x10).setText(LLC + SYS.strFromCharCode(0x60, mess.length) + mess);
	tr.messIO(req, "UserProtocol");
	if (req.attr("err").toInt()){SYS.messDebug("/NIK2303I/TMPL",tr("Error response")+": "+req.attr("err"));
		Ae=Ae_=Re=Re_=Ae1=Ae_1=Re1=Re_1=Ua=Ub=Uc=Ia=Ib=Ic=Pa=Pb=Pc=Qa=Qb=Qc=kPa=kPb=kPc=EVAL_REAL;
		Vab=Vac=EVAL_INT;
		return;}
//ЗАПИТИ ПОТОЧНИХ ПОКАЗНИКІВ ЛІЧИЛЬНИКА T0
	if (T0)	{
		//Запит показник Активна енергія	(1.8.0)
		req.setAttr("cntr",0x10).setText(LLC+F_id+SYS.strFromCharCode( 1, 0, 1, 8, 0, 255)+end);
		tr.messIO(req, "UserProtocol");
		if (req.attr("err").toInt()){Ae = EVAL_REAL; tErr = "2:"+req.attr("err");}
		else	Ae = kT*((req.text().charCodeAt(9)<<40)+(req.text().charCodeAt(10)<<32)+(req.text().charCodeAt(11)<<24)+(req.text().charCodeAt(12)<<16)+(req.text().charCodeAt(13)<<8)+req.text().charCodeAt(14).toInt())/100000;
	//Запит показник Активна енергія генерація	(2.8.0)
		req.setAttr("cntr",0x10).setText(LLC+F_id+SYS.strFromCharCode( 1, 0, 2, 8, 0, 255)+end);
		tr.messIO(req, "UserProtocol");
		if (req.attr("err").toInt()){Ae_ = EVAL_REAL; tErr = "2:"+req.attr("err");}
		else	Ae_ = kT*((req.text().charCodeAt(9)<<40)+(req.text().charCodeAt(10)<<32)+(req.text().charCodeAt(11)<<24)+(req.text().charCodeAt(12)<<16)+(req.text().charCodeAt(13)<<8)+req.text().charCodeAt(14).toInt())/100000;
	//Запит показник реактивна енергія	 (3.8.0)
		req.setAttr("cntr",0x10).setText(LLC+F_id+SYS.strFromCharCode( 1, 0, 3, 8, 0, 255)+end);
		tr.messIO(req, "UserProtocol");
		if (req.attr("err").toInt()){Re = EVAL_REAL; tErr = "2:"+req.attr("err");}
		else	Re = kT*((req.text().charCodeAt(9)<<40)+(req.text().charCodeAt(10)<<32)+(req.text().charCodeAt(11)<<24)+(req.text().charCodeAt(12)<<16)+(req.text().charCodeAt(13)<<8)+req.text().charCodeAt(14).toInt())/100000;
	//Запит показник реактивна енергія генерація (4.8.0)	
		req.setAttr("cntr",0x10).setText(LLC+F_id+SYS.strFromCharCode( 1, 0, 4, 8, 0, 255)+end);
		tr.messIO(req, "UserProtocol");
		if (req.attr("err").toInt()){Re_ = EVAL_REAL; tErr = "2:"+req.attr("err");}
		else	Re_ = kT*((req.text().charCodeAt(9)<<40)+(req.text().charCodeAt(10)<<32)+(req.text().charCodeAt(11)<<24)+(req.text().charCodeAt(12)<<16)+(req.text().charCodeAt(13)<<8)+req.text().charCodeAt(14).toInt())/100000;
	}
	//ЗАПИТИ ПОТОЧНИХ ПОКАЗНИКІВ ЛІЧИЛЬНИКА T1
	if (T1)	{
		//Запит показник Активна енергія	(1.8.1)
		req.setAttr("cntr",0x10).setText(LLC+F_id+SYS.strFromCharCode( 1, 0, 1, 8, 1, 255)+end);
		tr.messIO(req, "UserProtocol");
		if (req.attr("err").toInt()){Ae1 = EVAL_REAL; tErr = "2:"+req.attr("err");}
		else	Ae1 = kT*((req.text().charCodeAt(9)<<40)+(req.text().charCodeAt(10)<<32)+(req.text().charCodeAt(11)<<24)+(req.text().charCodeAt(12)<<16)+(req.text().charCodeAt(13)<<8)+req.text().charCodeAt(14).toInt())/100000;
	//Запит показник Активна енергія генерація	(2.8.1)
		req.setAttr("cntr",0x10).setText(LLC+F_id+SYS.strFromCharCode( 1, 0, 2, 8, 1, 255)+end);
		tr.messIO(req, "UserProtocol");
		if (req.attr("err").toInt()){Ae_1 = EVAL_REAL; tErr = "2:"+req.attr("err");}
		else	Ae_1 = kT*((req.text().charCodeAt(9)<<40)+(req.text().charCodeAt(10)<<32)+(req.text().charCodeAt(11)<<24)+(req.text().charCodeAt(12)<<16)+(req.text().charCodeAt(13)<<8)+req.text().charCodeAt(14).toInt())/100000;
	//Запит показник реактивна енергія	 (3.8.1)
		req.setAttr("cntr",0x10).setText(LLC+F_id+SYS.strFromCharCode( 1, 0, 3, 8, 1, 255)+end);
		tr.messIO(req, "UserProtocol");
		if (req.attr("err").toInt()){Re1 = EVAL_REAL; tErr = "2:"+req.attr("err");}
		else	Re1 = kT*((req.text().charCodeAt(9)<<40)+(req.text().charCodeAt(10)<<32)+(req.text().charCodeAt(11)<<24)+(req.text().charCodeAt(12)<<16)+(req.text().charCodeAt(13)<<8)+req.text().charCodeAt(14).toInt())/100000;
	//Запит показник реактивна енергія генерація (4.8.1)	
		req.setAttr("cntr",0x10).setText(LLC+F_id+SYS.strFromCharCode( 1, 0, 4, 8, 1, 255)+end);
		tr.messIO(req, "UserProtocol");
		if (req.attr("err").toInt()){Re_1 = EVAL_REAL; tErr = "2:"+req.attr("err");}
		else	Re_1 = kT*((req.text().charCodeAt(9)<<40)+(req.text().charCodeAt(10)<<32)+(req.text().charCodeAt(11)<<24)+(req.text().charCodeAt(12)<<16)+(req.text().charCodeAt(13)<<8)+req.text().charCodeAt(14).toInt())/100000;
	}
	//ЗАПИТИ ПОТОЧНИХ ПОКАЗНИКІВ ЛІЧИЛЬНИКА T2
	if (T2)	{
		//Запит показник Активна енергія	(1.8.2)
		req.setAttr("cntr",0x10).setText(LLC+F_id+SYS.strFromCharCode( 1, 0, 1, 8, 2, 255)+end);
		tr.messIO(req, "UserProtocol");
		if (req.attr("err").toInt()){Ae2 = EVAL_REAL; tErr = "2:"+req.attr("err");}
		else	Ae2 = kT*((req.text().charCodeAt(9)<<40)+(req.text().charCodeAt(10)<<32)+(req.text().charCodeAt(11)<<24)+(req.text().charCodeAt(12)<<16)+(req.text().charCodeAt(13)<<8)+req.text().charCodeAt(14).toInt())/100000;
	//Запит показник Активна енергія генерація	(2.8.2)
		req.setAttr("cntr",0x10).setText(LLC+F_id+SYS.strFromCharCode( 1, 0, 2, 8, 2, 255)+end);
		tr.messIO(req, "UserProtocol");
		if (req.attr("err").toInt()){Ae_2 = EVAL_REAL; tErr = "2:"+req.attr("err");}
		else	Ae_2 = kT*((req.text().charCodeAt(9)<<40)+(req.text().charCodeAt(10)<<32)+(req.text().charCodeAt(11)<<24)+(req.text().charCodeAt(12)<<16)+(req.text().charCodeAt(13)<<8)+req.text().charCodeAt(14).toInt())/100000;
	//Запит показник реактивна енергія	 (3.8.2)
		req.setAttr("cntr",0x10).setText(LLC+F_id+SYS.strFromCharCode( 1, 0, 3, 8, 2, 255)+end);
		tr.messIO(req, "UserProtocol");
		if (req.attr("err").toInt()){Re2 = EVAL_REAL; tErr = "2:"+req.attr("err");}
		else	Re2 = kT*((req.text().charCodeAt(9)<<40)+(req.text().charCodeAt(10)<<32)+(req.text().charCodeAt(11)<<24)+(req.text().charCodeAt(12)<<16)+(req.text().charCodeAt(13)<<8)+req.text().charCodeAt(14).toInt())/100000;
	//Запит показник реактивна енергія генерація (4.8.2)	
		req.setAttr("cntr",0x10).setText(LLC+F_id+SYS.strFromCharCode( 1, 0, 4, 8, 2, 255)+end);
		tr.messIO(req, "UserProtocol");
		if (req.attr("err").toInt()){Re_2 = EVAL_REAL; tErr = "2:"+req.attr("err");}
		else	Re_2 = kT*((req.text().charCodeAt(9)<<40)+(req.text().charCodeAt(10)<<32)+(req.text().charCodeAt(11)<<24)+(req.text().charCodeAt(12)<<16)+(req.text().charCodeAt(13)<<8)+req.text().charCodeAt(14).toInt())/100000;
	}
	//ЗАПИТИ ПОТОЧНИХ ПОКАЗНИКІВ ЛІЧИЛЬНИКА T3
	if (T3)	{
		//Запит показник Активна енергія	(1.8.3)
		req.setAttr("cntr",0x10).setText(LLC+F_id+SYS.strFromCharCode( 1, 0, 1, 8, 3, 255)+end);
		tr.messIO(req, "UserProtocol");
		if (req.attr("err").toInt()){Ae3 = EVAL_REAL; tErr = "2:"+req.attr("err");}
		else	Ae3 = kT*((req.text().charCodeAt(9)<<40)+(req.text().charCodeAt(10)<<32)+(req.text().charCodeAt(11)<<24)+(req.text().charCodeAt(12)<<16)+(req.text().charCodeAt(13)<<8)+req.text().charCodeAt(14).toInt())/100000;
	//Запит показник Активна енергія генерація	(2.8.3)
		req.setAttr("cntr",0x10).setText(LLC+F_id+SYS.strFromCharCode( 1, 0, 2, 8, 3, 255)+end);
		tr.messIO(req, "UserProtocol");
		if (req.attr("err").toInt()){Ae_3 = EVAL_REAL; tErr = "2:"+req.attr("err");}
		else	Ae_3 = kT*((req.text().charCodeAt(9)<<40)+(req.text().charCodeAt(10)<<32)+(req.text().charCodeAt(11)<<24)+(req.text().charCodeAt(12)<<16)+(req.text().charCodeAt(13)<<8)+req.text().charCodeAt(14).toInt())/100000;
	//Запит показник реактивна енергія	 (3.8.3)
		req.setAttr("cntr",0x10).setText(LLC+F_id+SYS.strFromCharCode( 1, 0, 3, 8, 3, 255)+end);
		tr.messIO(req, "UserProtocol");
		if (req.attr("err").toInt()){Re3 = EVAL_REAL; tErr = "2:"+req.attr("err");}
		else	Re3 = kT*((req.text().charCodeAt(9)<<40)+(req.text().charCodeAt(10)<<32)+(req.text().charCodeAt(11)<<24)+(req.text().charCodeAt(12)<<16)+(req.text().charCodeAt(13)<<8)+req.text().charCodeAt(14).toInt())/100000;
	//Запит показник реактивна енергія генерація (4.8.3)	
		req.setAttr("cntr",0x10).setText(LLC+F_id+SYS.strFromCharCode( 1, 0, 4, 8, 3, 255)+end);
		tr.messIO(req, "UserProtocol");
		if (req.attr("err").toInt()){Re_3 = EVAL_REAL; tErr = "2:"+req.attr("err");}
		else	Re_3 = kT*((req.text().charCodeAt(9)<<40)+(req.text().charCodeAt(10)<<32)+(req.text().charCodeAt(11)<<24)+(req.text().charCodeAt(12)<<16)+(req.text().charCodeAt(13)<<8)+req.text().charCodeAt(14).toInt())/100000;
	}
//ЗАПИТИ МИТТЄВИХ ЗНАЧЕНЬ	
	if (U )	{ 		//Напруга
	//Запит напруга фаза 1	
		req.setAttr("cntr",0x10).setText(LLC+F_id+SYS.strFromCharCode( 1, 0, 32, 7, 0, 255)+end);
		tr.messIO(req, "UserProtocol");
		if (req.attr("err").toInt()){U1 = EVAL_REAL; tErr = "2:"+req.attr("err");}
		else	U1 = (req.text().slice(9, 17).replace(",",".").toReal());
	//Запит напруга фаза  2	
		req.setAttr("cntr",0x10).setText(LLC+F_id+SYS.strFromCharCode( 1, 0, 52, 7, 0, 255)+end);
		tr.messIO(req, "UserProtocol");
		if (req.attr("err").toInt()){U2 = EVAL_REAL; tErr = "2:"+req.attr("err");}
		else	U2 = (req.text().slice(9, 17).replace(",",".").toReal());
	//Запит напруга фаза  3	
		req.setAttr("cntr",0x10).setText(LLC+F_id+SYS.strFromCharCode( 1, 0, 72, 7, 0, 255)+end);
		tr.messIO(req, "UserProtocol");
		if (req.attr("err").toInt()){U3 = EVAL_REAL; tErr = "2:"+req.attr("err");}
		else	U3 = (req.text().slice(9, 17).replace(",",".").toReal());
	}
	if(I){ 		//СТРУМ	
	//Запит струм фаза 1	
		req.setAttr("cntr",0x10).setText(LLC+F_id+SYS.strFromCharCode( 1, 0, 31, 7, 0, 255)+end);
		tr.messIO(req, "UserProtocol");
		if (req.attr("err").toInt()){I1 = EVAL_REAL; tErr = "2:"+req.attr("err");}
		else	I1 = kT*(req.text().slice(9, 17).replace(",",".").toReal());
	//Запит струм фаза 2	
		req.setAttr("cntr",0x10).setText(LLC+F_id+SYS.strFromCharCode( 1, 0, 51, 7, 0, 255)+end);
		tr.messIO(req, "UserProtocol");
		if (req.attr("err").toInt()){I2 = EVAL_REAL; tErr = "2:"+req.attr("err");}
		else	I2 = kT*(req.text().slice(9, 17).replace(",",".").toReal());
	//Запит струм фаза 3	
		req.setAttr("cntr",0x10).setText(LLC+F_id+SYS.strFromCharCode( 1, 0, 71, 7, 0, 255)+end);
		tr.messIO(req, "UserProtocol");
		if (req.attr("err").toInt()){I3 = EVAL_REAL; tErr = "2:"+req.attr("err");}
		else	I3 = kT*(req.text().slice(9, 17).replace(",",".").toReal());
		}
	if(P){		//Потужність
	//Запит потужність сумарна
		req.setAttr("cntr",0x10).setText(LLC+F_id+SYS.strFromCharCode( 1, 0, 1, 7, 0, 254)+end);
		tr.messIO(req, "UserProtocol");
		if (req.attr("err").toInt()){Psum = EVAL_REAL; tErr = "2:"+req.attr("err");}
		else	Psum = kT*(req.text().slice(9, 17).replace(",",".").toReal());
	//Запит потужність фаза 1
		req.setAttr("cntr",0x10).setText(LLC+F_id+SYS.strFromCharCode( 1, 128, 21, 7, 0, 0)+end);
		tr.messIO(req, "UserProtocol");
		if (req.attr("err").toInt()){P1 = EVAL_REAL; tErr = "2:"+req.attr("err");}
		else	P1 = kT*(req.text().slice(9, 17).replace(",",".").toReal());
	//Запит потужність фаза 2	
		req.setAttr("cntr",0x10).setText(LLC+F_id+SYS.strFromCharCode( 1, 128, 41, 7, 0, 0)+end);
		tr.messIO(req, "UserProtocol");
		if (req.attr("err").toInt()){P2 = EVAL_REAL; tErr = "2:"+req.attr("err");}
		else	P2 = kT*(req.text().slice(9, 17).replace(",",".").toReal());
	//Запит потужність фаза 3	
		req.setAttr("cntr",0x10).setText(LLC+F_id+SYS.strFromCharCode( 1, 128, 61, 7, 0, 0)+end);
		tr.messIO(req, "UserProtocol");
		if (req.attr("err").toInt()){P3 = EVAL_REAL; tErr = "2:"+req.attr("err");}
		else	P3 = kT*(req.text().slice(9, 17).replace(",",".").toReal());
	}
	if(kP){		//Коефіцієнт потужності
	//Запит коефіцієнт потужності фаза 1	
		req.setAttr("cntr",0x10).setText(LLC+F_id+SYS.strFromCharCode( 1, 0, 33, 7, 0, 255)+end);
		tr.messIO(req, "UserProtocol");
		if (req.attr("err").toInt()){kP1 = EVAL_REAL; tErr = "2:"+req.attr("err");}
		else	kP1 = (req.text().slice(9, 17).replace(",",".").toReal());		
	//Запит коефіцієнт потужності фаза 2	
		req.setAttr("cntr",0x10).setText(LLC+F_id+SYS.strFromCharCode( 1, 0, 53, 7, 0, 255)+end);
		tr.messIO(req, "UserProtocol");
		if (req.attr("err").toInt()){kP2 = EVAL_REAL; tErr = "2:"+req.attr("err");}
		else	kP2 = (req.text().slice(9, 17).replace(",",".").toReal());		
	//Запит коефіцієнт потужності фаза 3	
		req.setAttr("cntr",0x10).setText(LLC+F_id+SYS.strFromCharCode( 1, 0, 73, 7, 0, 255)+end);
		tr.messIO(req, "UserProtocol");
		if (req.attr("err").toInt()){kP3 = EVAL_REAL; tErr = "2:"+req.attr("err");}
		else	kP3 = (req.text().slice(9, 17).replace(",",".").toReal());
	}
	if(Q){	//Реактивна потужність
	//Запит реактивна потужність сумарна	
		req.setAttr("cntr",0x10).setText(LLC+F_id+SYS.strFromCharCode( 1, 0, 3, 7, 0, 254)+end);
		tr.messIO(req, "UserProtocol");
		if (req.attr("err").toInt()){Qsum = EVAL_REAL; tErr = "2:"+req.attr("err");}
		else	Qsum = kT*(req.text().slice(9, 17).replace(",",".").toReal());
	//Запит реактивна потужність фаза 1	
		req.setAttr("cntr",0x10).setText(LLC+F_id+SYS.strFromCharCode( 1, 128, 23, 7, 0, 0)+end);
		tr.messIO(req, "UserProtocol");
		if (req.attr("err").toInt()){Q1 = EVAL_REAL; tErr = "2:"+req.attr("err");}
		else	Q1 = kT*(req.text().slice(9, 17).replace(",",".").toReal());
	//Запит реактивна потужність фаза B	
		req.setAttr("cntr",0x10).setText(LLC+F_id+SYS.strFromCharCode( 1, 128, 43, 7, 0, 0)+end);
		tr.messIO(req, "UserProtocol");
		if (req.attr("err").toInt()){Q2 = EVAL_REAL; tErr = "2:"+req.attr("err");}
		else	Q2 = kT*(req.text().slice(9, 17).replace(",",".").toReal());
	//Запит реактивна потужність фаза 3	
		req.setAttr("cntr",0x10).setText(LLC+F_id+SYS.strFromCharCode( 1, 128, 63, 7, 0, 0)+end);
		tr.messIO(req, "UserProtocol");
		if (req.attr("err").toInt()){Q3 = EVAL_REAL; tErr = "2:"+req.attr("err");}
		else	Q3 = kT*(req.text().slice(9, 17).replace(",",".").toReal());
	}
	if(K){
	//Запит кута між векторами U1_U2	
		req.setAttr("cntr",0x10).setText(LLC+F_id+SYS.strFromCharCode( 1, 128, 33, 7, 0, 0)+end);
		tr.messIO(req, "UserProtocol");
		if (req.attr("err").toInt()){V12 = EVAL_INT; tErr = "2:"+req.attr("err");}
		else	V12 = req.text().charCodeAt(8);
	//Запит кута між векторами U1_U3	
		req.setAttr("cntr",0x10).setText(LLC+F_id+SYS.strFromCharCode( 1, 128, 73, 7, 0, 0)+end);
		tr.messIO(req, "UserProtocol");
		if (req.attr("err").toInt()){V13 = EVAL_INT; tErr = "2:"+req.attr("err");}
		else	V13 = req.text().charCodeAt(8);
		tErr = req.attr("err");
	}
	//Закриття зєднання
	//COMAND_SNRM 
	req.setAttr("cntr",0x53).setText("");
	tr.messIO(req, "UserProtocol");
	tr.start(false, 0);
	
}
//Error set
if(tErr.length) {
	SYS.messDebug("/NIK2303I/TMPL",tr("Error response")+": "+tErr);
	f_err = tErr;
}
else f_err = "0";','','',1580046416);
INSERT INTO tmplib_DevLib VALUES('ModBusScan','ModBus scanner','Сканер ModBus','Сканер ModBus','ModBus scanner for Coils (1), Input Coils (2), Registers (3), Input Registers (4); in the range [0...65535]. The template useful to examine unknown ModBus devices, scanning their Coils'' and Registers'' map for detecting the available data addresses. The scanning performs per one item, whether it is a coil or a register, to prevent the data unavailability for strict data accessing devices. The protocol ModBus itself implemented by the OpenSCADA module DAQ.ModBus, together which the template designed to use.

Author: Roman Savochenko <roman@oscada.org>
Total complexity: 0.2 HD
Sponsored by: SVItoVYR Ltd for whole complexity
Version: 1.0.2
License: GPLv2','Сканер ModBus щодо Кілець (1), Вхідних Кілець (2), Регістрів (3), Вхідних Регістрів (4); у діапазоні [0...65535]. Шаблон корисний для вивчення невідомих пристроїв ModBus, скануванням їх мапи Кілець та Регістрів для виявлення наявних адрес даних. Сканування відбувається по одному елементу, чи то кільцю чи регістру, для попередження недоступності даних на пристроях суворого доступу даних. Сам протокол ModBus реалізовано модулем OpenSCADA DAQ.ModBus, разом з яким шаблон і передбачено до використання.

Автор: Роман Савоченко <roman@oscada.org>
Загальна працемісткість: 0.2 ЛД
Спонсорування: ТОВ "СВІТоВИР" на загальну працемісткість
Версія: 1.0.2
Ліцензія: GPLv2','',240,0,'JavaLikeCalc.JavaScript
if(f_start) {
	res = "";
	begin = begin_ = end = 0;
	type_ = type;
	//this.attrAdd("type", "", "integer|sel", "0;1;2;3\nCoils (1);Input Coils (2);Registers (3);Input Registers (4)");
	return;
}
if(f_stop) return;

if(type_ != type || begin < begin_)
	begin = max(0,min(65535,begin)), res = "";

for(stTm = SYS.time(); begin < end && begin < 65536 && (SYS.time()-stTm) < lim; begin++) {
	if(type == 0 || type == 1) {
		pdu = SYS.strFromCharCode((type==0)?1:2, begin/256, begin%256, 0, 1);
		rez = this.cntr().messIO(pdu);
		res += begin.toString() + ":\t" + (rez.length?rez:pdu.charCodeAt(2)&0x01) + "\n";
	}
	else {
		pdu = SYS.strFromCharCode((type==2)?3:4, begin/256, begin%256, 0, 1);
		rez = this.cntr().messIO(pdu);
		res += begin.toString() + ":\t" + (rez.length?rez:pdu.charCodeAt(2)*256+pdu.charCodeAt(3))  + "\n";
	}
}
type_ = type, begin_ = begin;','','',1592158322);
INSERT INTO tmplib_DevLib VALUES('pulsarM','Pulsar-M','Пульсар-М','Пульсар-М','Pulsar-M counters protocol.

Author: Roman Savochenko <roman@oscada.org>
Sponsored: Zubarev Dmitry (IP INTEGRAL<http://kip-i-a.pro>)
Version: 1.0.0
License: GPLv2','','',10,0,'JavaLikeCalc.JavaScript
//Same request to the device
function req(tr, iaddr, func, data) {
	addr_ = iaddr.toString(10).toInt(16);
	reqId = rand(65535);
	req = Special.FLibSYS.IO("", "", "l");
	req.wr(addr_, "uint32", "b").wr(func, "uint8").wr(data.length+10, "uint8").wr(data.string, "char").wr(reqId, "uint16");
	req.wr(Special.FLibSYS.CRC(req.string), "uint16");
	if(func == 3)	SYS.messInfo("/Pulsar",tr("Request")+": "+SYS.strDecode(req.string,"Bin"," "));
	resp = Special.FLibSYS.IO("", "", "l");
	resp.wr(tr.messIO(req.string),"char");
	while(resp.length && (resp.length < 10 || resp.length < resp.string.charCodeAt(5)) &&
		(tresp=tr.messIO("")).length) resp.wr(tresp,"char");
	//SYS.messInfo("/Pulsar",tr("Response")+": "+SYS.strDecode(resp.string,"Bin"," "));
	//data = resp;
	if(resp.length < 10)							return tr("3:No response or the response is short.");
	resp.pos = 0;
	if(resp.read("uint32",1,"b") != addr_)	return tr("4:Disparity of the request address and the response one.");
	if(resp.read("uint8",1) != func)			return tr("4:Disparity of the request function and the response one.");
	if(resp.read("uint8",1) > resp.length)	return tr("4:Incomplete response.");
	data.string = resp.read("char", resp.length-10);
	if(resp.read("uint16",1) != reqId.toInt())
		return tr("4:Disparity of the request ID and the response one.");
	if(resp.read("uint16",1) != Special.FLibSYS.CRC(resp.string.slice(0,-2)))
		return tr("4:Error CRC.");
	return "0";
}

//Set transport and init
if(f_start) {
	transport_ = transport;
	tr = SYS.Transport.nodeAt(transport,".");
	nChnl_ = 0;
	wrBuf = new Array();
	wrBufW = new Array();
}
if(f_stop) return;

t_err = "0";

if(nChnl != nChnl_) {
	nChnl = max(0, min(32,nChnl));
	for( ; nChnl_ < nChnl; nChnl_++) {
		this.attrAdd("chnl"+nChnl_, tr("Channel")+" "+(nChnl_+1)+": "+tr("value"), "real");
		this.attrAdd("w"+nChnl_, tr("Channel")+" "+(nChnl_+1)+": "+tr("weight"), "real");
		this.attrAdd("avF"+nChnl_, tr("Channel")+" "+(nChnl_+1)+": "+tr("average flow"), "real|ro");
		this.attrAdd("in"+nChnl_, tr("Channel")+" "+(nChnl_+1)+": "+tr("input"), "boolean|ro");
	}
	for( ; nChnl_ > nChnl; nChnl_--) {
		this.attrDel("chnl"+(nChnl_-1));
		wrBuf[nChnl_-1] = EVAL;
		this.attrDel("w"+(nChnl_-1));
		wrBufW[nChnl_-1] = EVAL;
		this.attrDel("avF"+(nChnl_-1));
		this.attrDel("in"+(nChnl_-1));
	}
}
//Check for the transport change and connect
if(!tr || transport != transport_)	{
	transport_ = transport;
	tr = SYS.Transport.nodeAt(transport,".");
}
if(!tr)	t_err = "1:"+tr("Error the output transport ''%1''.").replace("%1",transport);
else if(addr < 0 || addr > 99999999)
	t_err = "2:"+tr("The address ''%1'' is out of the range [0...99999999].").replace("%1",addr.toString());
else {
	//Processing for writing channels and their weights
	for(iCh = 0; iCh < nChnl && !t_err.toInt(); iCh++) {
		//Checking channels to write
		if(!wrBuf[iCh].isEVal() && !(aVl=this["chnl"+iCh].get()).isEVal() && aVl != wrBuf[iCh]) {
			io = Special.FLibSYS.IO("", "", "l");
			io.wr((0x01<<iCh), "uint32");
			io.wr(aVl, "real*8");
			if(!(t_err=req(tr,addr,3,io)).toInt())	wrBuf[iCh] = aVl;
		}
		//Checking weights to write
		if(!wrBufW[iCh].isEVal() && !(aVl=this["w"+iCh].get()).isEVal() && aVl != wrBufW[iCh]) {
			io = Special.FLibSYS.IO("", "", "l");
			io.wr((0x01<<iCh), "uint32");
			io.wr(aVl, "real*4");
			if(!(t_err=req(tr,addr,8,io)).toInt())	wrBufW[iCh] = aVl;
		}
		//Reading channels
		/*io = Special.FLibSYS.IO("", "", "l");
		io.wr((0x01<<iCh), "uint32");
		if(!(t_err=req(tr,addr,1,io)).toInt()) {
			wrBuf[iCh] = io.read("real*8",1);
			this["chnl"+iCh].set(wrBuf[iCh], 0, 0, true);	
		}
		//Reading weights
		io = Special.FLibSYS.IO("", "", "l");
		io.wr((0x01<<iCh), "uint32");
		if(!(t_err=req(tr,addr,7,io)).toInt()) {
			wrBufW[iCh] = io.read("real*4",1);
			this["w"+iCh].set(wrBufW[iCh], 0, 0, true);	
		}
		//Reading average floats
		io = Special.FLibSYS.IO("", "", "l");
		io.wr((0x01<<iCh), "uint32");
		if(!(t_err=req(tr,addr,0x3E,io)).toInt())
			this["avF"+iCh].set(io.read("real*8",1), 0, 0, true);*/
	}

	//Reading for channels, their weights, average floats and inputs
	chMask = 0;
	for(iCh = 0; iCh < nChnl; iCh++)	chMask = chMask | (1<<iCh);

	// Reading channels
	if(!t_err.toInt()) {
		io = Special.FLibSYS.IO("", "", "l");
		io.wr(chMask, "uint32");
		if(!(t_err=req(tr,addr,1,io)).toInt())
			for(iCh = 0; iCh < nChnl; iCh++) {
				wrBuf[iCh] = io.read("real*8",1);
				this["chnl"+iCh].set(wrBuf[iCh], 0, 0, true);	
			}
	}
	// Reading weights
	if(!t_err.toInt()) {
		io = Special.FLibSYS.IO("", "", "l");
		io.wr(chMask, "uint32");
		if(!(t_err=req(tr,addr,7,io)).toInt())
			for(iCh = 0; iCh < nChnl; iCh++) {
				wrBufW[iCh] = io.read("real*4",1);
				this["w"+iCh].set(wrBufW[iCh], 0, 0, true);	
			}
	}
	// Reading average floats
	if(!t_err.toInt()) {
		io = Special.FLibSYS.IO("", "", "l");
		io.wr(chMask, "uint32");
		if(!(t_err=req(tr,addr,0x3E,io)).toInt())
			for(iCh = 0; iCh < nChnl; iCh++)
				this["avF"+iCh].set(io.read("real*8",1), 0, 0, true);
	}
	// Reading for inputs
	if(!t_err.toInt()) {
		io = Special.FLibSYS.IO("", "", "l");
		io.wr(chMask, "uint32");
		if(!(t_err=req(tr,addr,0x19,io)).toInt()) {
			vMask = io.read("uint32", 1);
			for(iCh = 0; iCh < nChnl; iCh++)
				this["in"+iCh].set((vMask&(1<<iCh))?true:false, 0, 0, true);
		}
	}
}

f_err = t_err;','','',1580047532);
INSERT INTO tmplib_DevLib VALUES('OWEN','OWEN','','','OWEN data sources implementation in the OWEN protocol. Implemented wholly in the template for the protocol requesting and for dynamic data model of the OWEN data sources'' data with support of the writing and original names of the parameters.

The field "addr" of the items can be wrote positive, for the 8 bit address [0...255], and negative, for the 11 bit address[-2047...0].

Author: Roman Savochenko <roman@oscada.org>, Constantine (IrmIngeneer)
State of the protocol implementing: Client of the protocol for reading and writing the specified PLC properties.
Total complexity: 0.5 HD
Actual issues: The undocumented behaviour:
  - МВ110-8А responds error 0x31 for reading "in-t": >1003023331932DCB43 <1010932D4791;
  - and error 0x34 for it writing: >1003023334932D2A42 <1001932D0BEDBC.
Version: 0.8.1
License: GPLv2','','',30,0,'JavaLikeCalc.JavaScript
function hash(data, isName) {
	for(CRC = 0, i = 0; i < data.length; i++) {
		b = data.charCodeAt(i);
		for(j = 0; j < ((isName==true)?7:8); j++, b = b << 1)
			CRC = ((b^(CRC>>8)&0x80)?(CRC<<1)^0x8F57:CRC<<1) & 0xFFFF;
	}
	return CRC;
}

function nmHash(name) {
	for(srcF = name + "    ", hashF = "", iV = 0; iV < srcF.length; iV++) {
		tVl = srcF[iV]; tVl2 = srcF.charCodeAt(iV);
		if(tVl == "." && iV > 0)	{ hashF = hashF.slice(0,-1)+SYS.strFromCharCode(hashF.charCodeAt(hashF.length-1)+1); continue; }
		if(tVl2 >= 0x30 && tVl2 <= 0x39)		tVl = tVl2-0x30;
		else if(tVl2 >= 0x41 && tVl2 <= 0x5A)	tVl = tVl2-0x41+10;
		else if(tVl2 >= 0x61 && tVl2 <= 0x7A)	tVl = tVl2-0x61+10;
		else if(tVl == "-")	tVl = 10+26;
		else if(tVl == "_")	tVl = 10+26+1;
		else if(tVl == "/")	tVl = 10+26+2;
		else if(tVl == " ")	tVl = 10+26+3;
		else break;
		hashF += SYS.strFromCharCode(tVl*2);
	}
	if(iV < name.length || hashF.length < 4)	return -1;
	for(rez = "", iV = 0; iV < 4; iV++)
		rez += SYS.strFromCharCode(hashF.charCodeAt(iV)*2);

	//SYS.messDebug("/OWEN", "func="+name+"; code="+SYS.strDecode(rez));

	return hash(rez, true);
}

//Same request to the device
//  addr - negative for 11 bit type
function req(tr, addr, data) {
	if(addr < -2047 || addr > 255)	return "1:"+tr("Address out of the range [-2047...255].");
	req = SYS.strFromCharCode((addr>=0)?abs(addr):abs(addr)>>3, ((addr>=0)?0:abs(addr<<5))|((data.length>2)?data.length-2:0x10)) + data;

	// Appending CRC
	CRC = hash(req);
	req += SYS.strFromCharCode(CRC>>8, CRC&255);

	SYS.messDebug("/OWEN", "req="+SYS.strDecode(req));

	// Converting to ASCII
	for(reqASCII = "", i = 0; i < req.length; i++)
		reqASCII += SYS.strFromCharCode((req.charCodeAt(i)>>4)+71, (req.charCodeAt(i)&0x0F)+71);
	req = "#" + reqASCII + "\r";

	// Sending the request and getting response
	for(resp = tr.messIO(req); resp[resp.length-1] != "\r" && (respTail=tr.messIO("")).length; )
		resp += respTail;

	// Processing the response
	if(resp.length < req.length || resp[resp.length-1] != "\r" || resp[0] != "#" || (resp.length%2) != 0)
		return "10:"+tr("Wrong or no response.");

	//SYS.messDebug("/OWEN", "resp="+resp);

	resp = resp.slice(1, resp.length-1);

	// Converting to binary
	for(respBin = "", b = 0, i = 0, fullB = false; i < resp.length; i++, fullB = !fullB) {
		b += resp.charCodeAt(i) - 71;
		if(fullB) { respBin += SYS.strFromCharCode(b); b = 0; } else b *= 16;
	}
	resp = respBin;

	SYS.messDebug("/OWEN", "resp="+SYS.strDecode(resp));

	// Checking for CRC
	CRC = hash(resp.slice(0,-2));
	if(CRC != ((resp.charCodeAt(resp.length-2)<<8)+resp.charCodeAt(resp.length-1)))
		return "11:"+tr("Error CRC.");

	if(resp.charCodeAt(1) != (resp.length-6))
		return "12:"+tr("The data size is not equal to pointed one.");
	else if(resp.slice(2,4) != data.slice(0,2) && resp.charCodeAt(2,"UTF-16BE") != nmHash("n.Err"))
		return "21:Unpaired function in the response.";

	data = resp.slice(2, resp.length-2);
	//if(data.length < 3)	return "49:"+tr("Data size error.");

	return "0";
}

//Set transport and init
if(f_start) {
	dt = new Object();
	items_ = "";
	transport_ = transport;
	tr = SYS.Transport.nodeAt(transport,".");

	t_infW = ""; infWHoldTm = 0;
}
if(f_stop) return;

//Parsing the items to set and to create the user attributes
if(items != items_) {
	items_ = items;
	// Mark of checking to deletion needs
	for(var iDt in dt)	dt[iDt].mark = false;
	// Appending/Updating present ones
	for(itOff = 0; (sIt=items.parseLine(0,itOff)).length; ) {
		if(sIt[0] == "#")	continue;
		off1 = 0;
		itO = new Object();
		itO.addr = sIt.parse(0, ":", off1).toInt();
		itO.func = sIt.parse(0, ":", off1);
		itO.tp = sIt.parse(0, ":", off1);
		itO.md = sIt.parse(0, ":", off1);
		itO.id = sIt.parse(0, ":", off1);
		itO.nm = sIt.slice(off1);
		if(!itO.nm.length)	itO.nm = itO.id.length ? itO.id : itO.func;
		if(!itO.id.length)	itO.id = itO.func.replace(new RegExp("[.]"), "_");
		SYS.messDebug("/OWEN", "id="+itO.id);
		if(itO.tp == "u" || itO.tp == "i") {
			wTp = "integer";
			itO.sz = itO.tp.slice(1).toInt();
			itO.sz = (itO.sz?itO.sz:16)/8;
			if(!(itO.sz==1||itO.sz==2||itO.sz==4))	continue; 
		}
		else if(itO.tp == "s")	wTp = "string";
		else if(itO.tp == "f")	wTp = "real";
		else continue;
		itO.rd = (itO.md.indexOf("r") >= 0); itO.wr = (itO.md.indexOf("w") >= 0);
		if(!itO.wr)	wTp += "|ro";
		if((itO.funcH=nmHash(itO.func)) < 0)	continue;
		// Apending
		dt[itO.id] = itO;
		if(itO.rd || itO.wr) {
			this.attrAdd(itO.id, itO.nm, wTp);
			if(itO.wr)	itO.val = this[itO.id].get();
			itO.mark = true;
			//SYS.messDebug("/OWEN", "itO="+itO.id+"; tmpAddr="+tmpAddr+"; addr="+itO.addr);
		}
	}
	// Check, remove item and set to EVAL the attribute
	for(var iDt in dt) {
		if(dt[iDt].mark)	continue;
		this[dt[iDt].id].set(EVAL, 0, 0, true);	//If the attribute locked
		this.attrDel(dt[iDt].id);
		delete dt[iDt];
	}
}

t_err = "0";

//Check for the transport change and connect
if(!tr || transport != transport_)	{
	transport_ = transport;
	tr = SYS.Transport.nodeAt(transport,".");
}
if(!tr)	t_err = "1:"+tr("Error the output transport ''%1''.").replace("%1",transport);
else {
	t_inf = "";
	if(infWHoldTm && SYS.time() > infWHoldTm)	t_infW = "", infWHoldTm = 0;
	
	//The parameters processing
	for(var iDt in dt) {
		itO = dt[iDt];
		if(!t_err.toInt()) {
			SYS.messDebug("/OWEN", "iDt="+iDt);

			// Writing
			if(itO.wr && !(tVl=this[itO.id].get()).isEVal() && tVl != itO.val) {
				data = SYS.strFromCharCode(itO.funcH>>8, itO.funcH&0xFF);
				if(itO.tp == "f") {
					tVl2 = tVl3 = 0;
					Special.FLibSYS.floatSplitWord(tVl, tVl2, tVl3);
					data += SYS.strFromCharCode(tVl3>>8, tVl3&0xFF, tVl2>>8) + ((tVl2&0xFF)?SYS.strFromCharCode(tVl2&0xFF):"");
				}
				else if(itO.tp == "u" || itO.tp == "i") {
					for(tVl2 = SYS.strFromCharCode(tVl&0xFF), tVl = tVl>>8; tVl; tVl = tVl>>8)
						tVl2 = SYS.strFromCharCode(tVl&0xFF) + tVl2;
					data += tVl2;
				}
				else if(itO.tp == "s")
					for(iV = tVl.length-1; iV >= 0; iV--)	data += tVl[iV];

				if(!(t_err=req(tr,itO.addr,data)).toInt()) {
					if(data.charCodeAt(0,"UTF-16BE") == nmHash("n.Err")) {
						t_infW += (t_infW.length?"; ":": ")+itO.nm+":nErr=0x"+data.charCodeAt(2).toString(16);
						infWHoldTm = SYS.time()+10;
					} else itO.val = tVl;
				}
			}

			// Reading
			data = SYS.strFromCharCode(itO.funcH>>8, itO.funcH&0xFF);
			if(itO.rd && !(t_err=req(tr,itO.addr,data)).toInt()) {
				if(data.charCodeAt(0,"UTF-16BE") == nmHash("n.Err") && itO.funcH != nmHash("n.Err")) {
					t_inf += (t_inf.length?"; ":": ")+itO.nm+":nErr=0x"+data.charCodeAt(2).toString(16);
					this[itO.id].set(EVAL, 0, 0, true); itO.val = EVAL;
				}
				else {
					data = data.slice(2);
					if(itO.tp == "f") {
						if(data.length == 1)
							tVl = EVAL, t_inf += (t_inf.length?"; ":": ")+itO.nm+":err=0x"+data.charCodeAt(0).toString(16);
						else if(data.length >= 3)
							tVl = Special.FLibSYS.floatMergeWord(
								data.charCodeAt(2)*256+((data.length < 4)?0:data.charCodeAt(3)),
								data.charCodeAt(0)*256+data.charCodeAt(1));
						else tVl = EVAL;
						this[itO.id].set(tVl, 0, 0, true); itO.val = tVl;
					}
					else if(itO.tp == "u" || itO.tp == "i") {
						for(tVl = 0, iV = 0; iV < data.length; iV++)
							tVl = (tVl<<8) | data.charCodeAt(iV);
						this[itO.id].set(tVl, 0, 0, true); itO.val = tVl;
						//SYS.messDebug("/OWEN", "Integer data="+SYS.strDecode(data)+"; Sz="+itO.sz);
					}
					else if(itO.tp == "s")	{
						for(tVl = "", iV = 0; iV < data.length; iV++)	tVl = data[iV] + tVl;
						this[itO.id].set(tVl, 0, 0, true);	 itO.val = tVl;
					}
					continue;
				}
			}
		}
		this[itO.id].set(EVAL, 0, 0, true); itO.val = EVAL;
	}
}

f_err = t_err;
if(!f_err.toInt())	f_err += t_inf + (t_infW.length?" "+tr("Write")+t_infW:"");','','',1592158749);
INSERT INTO tmplib_DevLib VALUES('DCON_Prt','DCON Protocol + Input AI','','','Input and output parts of the protocol DCON implementation with the input part designed for typical AI modules at up to 20 channels. The protocol DCON client part also implemented by the OpenSCADA module DAQ.DCON.

The DCON commands are supported: #AA

Author: Roman Savochenko <roman@oscada.org>
Version: 1.0.1
License: GPLv2','','',10,0,'JavaLikeCalc.JavaScript
if(f_start)	io = tr = EVAL;

function CRC( inS ) {
	sumV = 0;
	for(i = 0; i < inS.length; i++) sumV += inS.charCodeAt(i);

	return sumV&0xFF;
}

//---------------------------------------
//Output request form of the UserProtocol
//---------------------------------------
//<ReqSymb addr="{toAddr}" err="{err}" CRC="{CRC}">{req}</ReqSymb> - tag of the output message
//  ReqSymb - symbol of the request type into the tag name, like to: ''#'', ''@'', etc;
//  toAddr - remote host address [0...240];
//  CRC - sign [0,1] of using CRC;
//  req - data of the request and response;
//  err - sets for the request result.
if(!tr.isEVal() && !io.isEVal() && io.attr("ProtIt").length) {
	io.setAttr("err", "0");

	req = io.name().slice(0,1) + io.attr("addr").toInt().toString(16,2) + io.text();
	if(io.attr("CRC").toInt())
		req += (CRC(req)&0xFF).toString(16,2);
	req += "\r";

	//Sending the request
	resp = tr.messIO(req);
	while(resp.length && resp[resp.length-1] != "\r")
		if((tresp=tr.messIO("")).length) resp += tresp;
		else break;

	//Analysis of the response
	if(io.attr("CRC").toInt()) {
		if(resp.length < 4 || resp[resp.length-1] != "\r")
		{ io.setAttr("err","10:"+tr("Error or no response.")); return; }
		//Checking the response to the integrity (CRC)
		if(CRC(resp.slice(0,-3)) != resp.slice(-3,-1).toInt(16))
		{ io.setAttr("err","11:"+tr("CRC error.")); return; }
	}
	else if(resp.length < 2 || resp[resp.length-1] != "\r")
	{ io.setAttr("err","10:"+tr("Error or no response.")); return; }
	if(resp[0] != ">") { io.setAttr("err","12:"+resp[0]+":"+tr("DCON error.")); return; }

	//Returning the result
	io.setText(resp.slice(1,(io.attr("CRC").toInt()?-3:-1)));

	return;
}

//---------------------------------------------
//Input requests processing of the UserProtocol
//---------------------------------------------
//Testing the request for completity
if(request.length < 4 || request[request.length-1] != "\r") {
	if(request.length > 10) request = "";
	return true;
}
addr = max(0, min(240,addr));
n = max(1, min(20,n));
req = request; request = "";

//SYS.messInfo("DCON/in", "TEST 01: "+req);

//Checking the request for the integrity (CRC) and the address
if((enCRC && CRC(req.slice(0,-3)) != req.slice(-3,-1).toInt(16)) || !(addr == 0 || req.slice(1,3).toInt(16) == addr))
	return true;

//Analysis the request and the response preparing
if(req[0] == "#") {	//Commands #AA
	answer = ">";
	for(iA = 0; iA < n; iA++)
		answer += ((tVl=arguments["ai"+iA]).isEVal()?0:tVl).toFixed(3,7,true);
} else answer = "?";	//return true;

//Finishing the response
if(enCRC) answer += (CRC(answer)&0xFF).toString(16,2);
answer += "\r";

return true;','','',1580046089);
INSERT INTO tmplib_DevLib VALUES('DCON_Prt_DI','DCON Protocol + Input DI','','','Input and output parts of the protocol DCON implementation with the input part designed for typical DI modules at up to 32 channels. The protocol DCON client part also implemented by the OpenSCADA module DAQ.DCON.

The DCON commands are supported: @AA, $AA6

Author: Roman Savochenko <roman@oscada.org>
Version: 1.0.1
License: GPLv2','','',10,0,'JavaLikeCalc.JavaScript
if(f_start)	io = tr = EVAL;

function CRC( inS ) {
	sumV = 0;
	for(i = 0; i < inS.length; i++) sumV += inS.charCodeAt(i);

	return sumV&0xFF;
}

//---------------------------------------
//Output request form of the UserProtocol
//---------------------------------------
//<ReqSymb addr="{toAddr}" err="{err}" CRC="{CRC}">{req}</ReqSymb> - tag of the output message
//  ReqSymb - symbol of the request type into the tag name, like to: ''#'', ''@'', etc;
//  toAddr - remote host address [0...240];
//  CRC - sign [0,1] of using CRC;
//  req - data of the request and response;
//  err - sets for the request result.
if(!tr.isEVal() && !io.isEVal() && io.attr("ProtIt").length) {
	io.setAttr("err", "0");

	req = io.name().slice(0,1) + io.attr("addr").toInt().toString(16,2) + io.text();
	if(io.attr("CRC").toInt())
		req += (CRC(req)&0xFF).toString(16,2);
	req += "\r";

	//Sending the request
	resp = tr.messIO(req);
	while(resp.length && resp[resp.length-1] != "\r")
		if((tresp=tr.messIO("")).length) resp += tresp;
		else break;

	//Analysis of the response
	if(io.attr("CRC").toInt()) {
		if(resp.length < 4 || resp[resp.length-1] != "\r")
		{ io.setAttr("err","10:"+tr("Error or no response.")); return; }
		//Checking the response to the integrity (CRC)
		if(CRC(resp.slice(0,-3)) != resp.slice(-3,-1).toInt(16))
		{ io.setAttr("err","11:"+tr("CRC error.")); return; }
	}
	else if(resp.length < 2 || resp[resp.length-1] != "\r")
	{ io.setAttr("err","10:"+tr("Error or no response.")); return; }
	if(resp[0] != ">") { io.setAttr("err","12:"+resp[0]+":"+tr("DCON error.")); return; }

	//Returning the result
	io.setText(resp.slice(1,(io.attr("CRC").toInt()?-3:-1)));

	return;
}

//---------------------------------------------
//Input requests processing of the UserProtocol
//---------------------------------------------
//Testing the request for completity
if(request.length < 4 || request[request.length-1] != "\r") {
	if(request.length > 10) request = "";
	return true;
}
addr = max(0, min(240,addr));
n = max(1, min(32,n));
req = request; request = "";

//SYS.messInfo("DCON/in", "TEST 01: "+req);

//Checking the request for the integrity (CRC) and the address
if((enCRC && CRC(req.slice(0,-3)) != req.slice(-3,-1).toInt(16)) || !(addr == 0 || req.slice(1,3).toInt(16) == addr))
	return true;

//Analysis the request and the response preparing
if(req[0] == "@" || (req[0] == "$" && req[3] == "6")) {	//Commands @AA and $AA6
	for(ib = 0; ib < n; ib++) {
		if(ib%8 == 0)	tVl = 0;
		tVl += ((arguments["di"+ib] == true)?1:0) << (ib%8);
		if(ib%8 == 7 || (ib+1) == n)	answer = tVl.toString(16,2) + answer;
	}
	if(req[0] == "$")	answer += "00";
	answer = "!" + answer;
} else answer = "?";	//return true;


//Finishing the response
if(enCRC) answer += (CRC(answer)&0xFF).toString(16,2);
answer += "\r";

return true;','','',1580047532);
INSERT INTO tmplib_DevLib VALUES('DCON_Prt_AO','DCON Protocol + Input AO','','','Input and output parts of the protocol DCON implementation with the input part designed for typical AO modules at up to 8 channels. The protocol DCON client part also implemented by the OpenSCADA module DAQ.DCON.

The DCON commands are supported: $AA8N, #AAN{Data}

Author: Roman Savochenko <roman@oscada.org>
Version: 1.0.1
License: GPLv2','','',10,0,'JavaLikeCalc.JavaScript
if(f_start)	io = tr = EVAL;

function CRC( inS ) {
	sumV = 0;
	for(i = 0; i < inS.length; i++) sumV += inS.charCodeAt(i);

	return sumV&0xFF;
}

//---------------------------------------
//Output request form of the UserProtocol
//---------------------------------------
//<ReqSymb addr="{toAddr}" err="{err}" CRC="{CRC}">{req}</ReqSymb> - tag of the output message
//  ReqSymb - symbol of the request type into the tag name, like to: ''#'', ''@'', etc;
//  toAddr - remote host address [0...240];
//  CRC - sign [0,1] of using CRC;
//  req - data of the request and response;
//  err - sets for the request result.
if(!tr.isEVal() && !io.isEVal() && io.attr("ProtIt").length) {
	io.setAttr("err", "0");

	req = io.name().slice(0,1) + io.attr("addr").toInt().toString(16,2) + io.text();
	if(io.attr("CRC").toInt())
		req += (CRC(req)&0xFF).toString(16,2);
	req += "\r";

	//Sending the request
	resp = tr.messIO(req);
	while(resp.length && resp[resp.length-1] != "\r")
		if((tresp=tr.messIO("")).length) resp += tresp;
		else break;

	//Analysis of the response
	if(io.attr("CRC").toInt()) {
		if(resp.length < 4 || resp[resp.length-1] != "\r")
		{ io.setAttr("err","10:"+tr("Error or no response.")); return; }
		//Checking the response to the integrity (CRC)
		if(CRC(resp.slice(0,-3)) != resp.slice(-3,-1).toInt(16))
		{ io.setAttr("err","11:"+tr("CRC error.")); return; }
	}
	else if(resp.length < 2 || resp[resp.length-1] != "\r")
	{ io.setAttr("err","10:"+tr("Error or no response.")); return; }
	if(resp[0] != ">") { io.setAttr("err","12:"+resp[0]+":"+tr("DCON error.")); return; }

	//Returning the result
	io.setText(resp.slice(1,(io.attr("CRC").toInt()?-3:-1)));

	return;
}

//---------------------------------------------
//Input requests processing of the UserProtocol
//---------------------------------------------
//Testing the request for completity
if(request.length < 4 || request[request.length-1] != "\r") {
	if(request.length > 10) request = "";
	return true;
}
addr = max(0, min(240,addr));
req = request; request = "";

//SYS.messInfo("DCON/in", "TEST 01: "+req);

//Checking the request for the integrity (CRC) and the address
if((enCRC && CRC(req.slice(0,-3)) != req.slice(-3,-1).toInt(16)) || !(addr == 0 || req.slice(1,3).toInt(16) == addr))
	return true;

//Analysis the request and the response preparing
if(req[0] == "$" && req[3] == "8") {	//Command $AA8N to read back the output value
	answer = "!";
	n = req.slice(4,5).toInt(16);
	answer += req.slice(1,3) + ((tVl=arguments["ao"+n]).isEVal()?0:tVl).toFixed(3,7,true);
}
else if(req[0] == "#" && req.length >= 10) {	//Command #AAN{Data} to write the output
	n = req.slice(3,4).toInt(16);
	arguments["ao"+n] = req.slice(4,(enCRC?-3:-1)).toReal();
	answer = ">";
}
else answer = "?";	//return true;

//Finishing the response
if(enCRC) answer += (CRC(answer)&0xFF).toString(16,2);
answer += "\r";

return true;','','',1580047532);
INSERT INTO tmplib_DevLib VALUES('DCON_Prt_DO','DCON Protocol + Input DO','','','Input and output parts of the protocol DCON implementation with the input part designed for typical DO modules at up to 32 channels. The protocol DCON client part also implemented by the OpenSCADA module DAQ.DCON.

The DCON commands are supported: @AA, @AA{Data}

Author: Roman Savochenko <roman@oscada.org>
Version: 1.0.1
License: GPLv2','','',10,0,'JavaLikeCalc.JavaScript
if(f_start)	io = tr = EVAL;

function CRC( inS ) {
	sumV = 0;
	for(i = 0; i < inS.length; i++) sumV += inS.charCodeAt(i);

	return sumV&0xFF;
}

//---------------------------------------
//Output request form of the UserProtocol
//---------------------------------------
//<ReqSymb addr="{toAddr}" err="{err}" CRC="{CRC}">{req}</ReqSymb> - tag of the output message
//  ReqSymb - symbol of the request type into the tag name, like to: ''#'', ''@'', etc;
//  toAddr - remote host address [0...240];
//  CRC - sign [0,1] of using CRC;
//  req - data of the request and response;
//  err - sets for the request result.
if(!tr.isEVal() && !io.isEVal() && io.attr("ProtIt").length) {
	io.setAttr("err", "0");

	req = io.name().slice(0,1) + io.attr("addr").toInt().toString(16,2) + io.text();
	if(io.attr("CRC").toInt())
		req += (CRC(req)&0xFF).toString(16,2);
	req += "\r";

	//Sending the request
	resp = tr.messIO(req);
	while(resp.length && resp[resp.length-1] != "\r")
		if((tresp=tr.messIO("")).length) resp += tresp;
		else break;

	//Analysis of the response
	if(io.attr("CRC").toInt()) {
		if(resp.length < 4 || resp[resp.length-1] != "\r")
		{ io.setAttr("err","10:"+tr("Error or no response.")); return; }
		//Checking the response to the integrity (CRC)
		if(CRC(resp.slice(0,-3)) != resp.slice(-3,-1).toInt(16))
		{ io.setAttr("err","11:"+tr("CRC error.")); return; }
	}
	else if(resp.length < 2 || resp[resp.length-1] != "\r")
	{ io.setAttr("err","10:"+tr("Error or no response.")); return; }
	if(resp[0] != ">") { io.setAttr("err","12:"+resp[0]+":"+tr("DCON error.")); return; }

	//Returning the result
	io.setText(resp.slice(1,(io.attr("CRC").toInt()?-3:-1)));

	return;
}

//---------------------------------------------
//Input requests processing of the UserProtocol
//---------------------------------------------
//Testing the request for completity
if(request.length < 4 || request[request.length-1] != "\r") {
	if(request.length > 10) request = "";
	return true;
}
addr = max(0, min(240,addr));
n = max(1, min(32,n));
req = request; request = "";

//SYS.messInfo("DCON/in", "TEST 01: "+req);

//Checking the request for the integrity (CRC) and the address
if((enCRC && CRC(req.slice(0,-3)) != req.slice(-3,-1).toInt(16)) || !(addr == 0 || req.slice(1,3).toInt(16) == addr))
	return true;
req = req.slice(0, (enCRC?-3:-1));

//Analysis the request and the response preparing
if(req[0] == "@" && req.length == 3) {	//Commands @AA
	for(ib = 0; ib < n; ib++) {
		if(ib%8 == 0)	tVl = 0;
		tVl += ((arguments["do"+ib] == true)?1:0) << (ib%8);
		if(ib%8 == 7 || (ib+1) == n)	answer = tVl.toString(16,2) + answer;
	}
	answer = ">" + answer;
}
else if(req[0] == "@" && req.length > 3) {	//Commands @AA{Data}
	for(iB = req.length-2, iC = 0; iB >= 3; iB -= 2) {
		tVl = req.slice(iB, iB+2).toInt(16);
		for(ib = 0; ib < 8; ib++, iC++)
			arguments["do"+iC] = tVl&(1<<ib);
	}
	answer = ">";
}
else answer = "?";	//return true;

//Finishing the response
if(enCRC) answer += (CRC(answer)&0xFF).toString(16,2);
answer += "\r";

return true;','','',1580047532);
INSERT INTO tmplib_DevLib VALUES('DNP3','Distributed Network Protocol 3','','','Distributed Network Protocol 3 (DNP3) is a set of communications protocols used between components in process automation systems. Its main use is in utilities such as electric and water companies. Usage in other industries is not common. It was developed for communications between various types of data acquisition and control equipment. Competing standards include the older Modbus protocol and the newer IEC 61850 protocol and IEC 61870.

The template implements currently for the client, which uses the followed functions: Confirm (0x00), Read (0x01), Write (0x02), Enable (0x14) and Disable (0x15) Spontaneous Messages.

And which implements and tested for the followed point-oriented objects (data types):
- 1.[2] — Binary Input [with status]
- 2.[1] — Binary Input Changed [w/o time]
- 3.[2] — Double-bit Input [with status]
- 4.[1] — Double-bit Input Changed [w/o time]
- 10.[2] — Binary Output (reading back and used to write also) [with status]
- 20,21.[1] — Binary Counter (Typical,Frozen) [32-Bit]
- 22,23.[1] — Binary Counter Changed (Typical,Frozen) [32-Bit w/o time]
- 30.[1,5] — Analog Input [32-bit integer value with flag, 32-bit floating point value with flag]
- 32.[7] — Analog Input Changed [32-bit floating point value with flag and event time]
- 40.[1] — Analog Output (reading back and used to write also) [32-bit integer value with flag]
- 50 — Time and Date (just passed and not read) [indexed absolute time and long interval]
- 60.[1,2,3,4] — Classes [0,1,2,3]
- 80.[1] — Device [Restart 7=Off(0x01)]
- 110.[1] — Octet String (just passed and not read) [Sized by variation]

Considering the purpose and the application area, the protocol does not provide the direct determination of the requested data and the device, that implements it, provides all available ones upon request for the Class 0 data, after which only changes are requested for the Classes 1, 2, 3.

All received and controlled data is accumulated in an object of the attribute "items" for further control as an object in a table of the widget "Main.objProps", according to the complex object definition.

For standard control over parameter attributes, including archiving, it is possible to define the necessary data to create a separate representative attribute for them, which is described in the attribute "itemsSet", where they can be defined individually, with a semantic identifier and name, or in a range named by a template. The data in the attribute "itemsSet" is defined by records in the format ai|ao|cnt|di|do:{pnt}[-{endPnt}][:a[:{NameBase}]], where:
- ai|ao|cnt|di|do — data type, where:
  - "ai" — analog inputs, transmitted by the functions 30 and 32;
  - "ao" — analog outputs, transmitted by the function 40;
  - "cnt" — counters, transmitted by the functions 20, 21, 22, 23; so Typical and Frozen counters is in one space;
  - "di" — discrete inputs, transmitted by the functions 1, 2, 3, 4; so Binary Input and Double-bit Input is in one space;
  - "do" — discrete outputs, transmitted by the function 10.
- {pnt} — point number of the range begin;
- {endPnt} — the range end (the last point number);
- a — sign of creating the representative attribute for the parameter or the parameters group;
- {NameBase} — naming base of the attributes.

Examples of the parameters definition, rows of the attribute "itemsSet":
- ai:0-9:a — analog inputs in the points range [0...9] with forming the attributes;
- ao:0-9:a — analog outputs in the points range [0...9] with forming the attributes;
- di:0-9:a — discrete inputs in the points range [0...9] with forming the attributes;
- do:0-9:a — discrete outputs in the points range [0...9] with forming the attributes;
- cnt:0-9:a — counters in the points range [0...9] with forming the attributes.

Author: Roman Savochenko <roman@oscada.org>
Total complexity: 2.0 HD
Sponsored by: Hadi Ramezani and Hossein Malakooti for whole complexity
Version: 0.8.1
License: GPLv2','','',10,0,'JavaLikeCalc.JavaScript
function CRC( inSeq ) { return Special.FLibSYS.CRC(inSeq, 16, 0x3D65, 0, true, true, -1); }

function inVal(aId, aVal, QLT, wr) {
	if(!QLT.isEVal() && (!(QLT&0x01) || (QLT&0x1E)))	aVal = EVAL;
	if(wr.isEVal())	wr = false;
	if(items[aId].isEVal()) {
		items[aId] = itW = new Object();
		itW.descr = aId;
		itW.wr = wr; itW.alarm = 0;
	} else itW = items[aId];
	itW.val = aVal;
	if((aO=this[aId])) {
		aO.set(aVal, 0, 0, true);
		if(itW.wr) oAVals[aId] = aVal;
	}
}

//Forming of the message and placing it to the output buffer
function mess( func, objs, ac ) {
	if(ac.isEVal())	{
		ac = 0x80 | 0x40 | (ctx.appSeq&0xF);	//FIR, FIN, Sequence
		ctx.reqToResp = ctx.appSeq.toString()+":"+func;
		if(ctx.appSeq >= 15)	ctx.appSeq = 0;	else ctx.appSeq++;
	} 
	aMess = SYS.strFromCharCode(ac, func) + objs;
	//Splitting the application message to the transport messages,
	//		limited in the length in 255 symbols without CRC
	for(off = 0; off < aMess.length; ) {
		trMess = aMess.slice(off, off+min(255-6,aMess.length-off));
		trMess = SYS.strFromCharCode(((off==0)?0x80:0)|((off+trMess.length)>=aMess.length?0x40:0)|(ctx.trSeq&0x3F)) +		//FIR, FIN, Sequence
			trMess;
		if(ctx.trSeq >= 63)	ctx.trSeq = 0;	else ctx.trSeq++;
		off += trMess.length;

		//Splitting the transport message to the data chunks,
		//		limited in 16 symbols and appending CRC
		for(mess = "", chOff = 0; chOff < trMess.length; chOff += 16) {
			tVl = trMess.slice(chOff, chOff+min(16,trMess.length-chOff));
			mess += tVl + SYS.strFromCharUTF("UTF-16",CRC(tVl));
		}

		//Appending the transport message header and it CRC
		tVl = SYS.strFromCharCode(0x05, 0x64, (trMess.length+5), 0x80|0x40|0x4) +	//DIR, PRM, Unconfirmed User Data (4) ????
				SYS.strFromCharUTF("UTF-16", dest, src);
		mess = tVl + SYS.strFromCharUTF("UTF-16",CRC(tVl)) + mess;
		ctx.out += mess;				//Buffering
		//tr.messIO(mess, 0, 0);	//Just direct writing per one message
	}
}

function processIn( ) {
	t_err = "";
	//Parse for packages in general - the transport layer
	for(waitSz = 0; ctx.in.length; ctx.in = ctx.in.slice(waitSz)) {
		// Checking the header entirety
		if((ctx.in.length >= 3 && (ctx.in.charCodeAt(0,"UTF-16") != 0x6405 || (dataSz=ctx.in.charCodeAt(2)) < 8)) ||
				(ctx.in.length >= 10 && CRC(ctx.in.slice(0,8)) != ctx.in.charCodeAt(8,"UTF-16"))) {
			SYS.messDebug("/DNP3",tr("Wrong or CRC-error sequence, rejected")+": "+SYS.strDecode(ctx.in,"Bin"," "));
			ctx.in = "";
			break;
		}
		waitSz = 10 + floor((dataSz-5)/16)*18 + ((tVl=(dataSz-5)%16)?tVl+2:0);
		if(ctx.in.length < 13 || ctx.in.length < waitSz)	break;	//Not full, waiting

		SYS.messDebug("/DNP3",tr("Good input sequence")+": "+SYS.strDecode(ctx.in.slice(0,waitSz),"Bin"," "));

		if(ctx.in.charCodeAt(4,"UTF-16") != src || ctx.in.charCodeAt(6,"UTF-16") != dest) {
			SYS.messDebug("/DNP3",tr("Foreign package, rejected")+": "+SYS.strDecode(ctx.in.slice(waitSz),"Bin"," "));
			continue;
		}

		// Getting the data chunks
		for(data = "", off = 10, iCh = 0; off < waitSz; off += 18, iCh++) {
			chSz = min(waitSz, off+18) - 2;
			if(CRC((tVl=ctx.in.slice(off,chSz))) != ctx.in.charCodeAt(chSz,"UTF-16"))	break;
			data += tVl;
		}
		if(off < waitSz) {
			SYS.messDebug("/DNP3",tr("CRC error in chunk %1, rejected").replace("%1",iCh.toString()));
			continue;
		}

		// Control of the transport packages sequence
		//  First
		if((tc=data.charCodeAt(0))&0x40) {
			ctx.inAMess = data;
			if(!(tc&0x80)) {							//Not Final
				if(ctx.firSeq >= 0 && (((wSeq=ctx.firSeq+1)>=64)?0:wSeq) != (tc&0x3F))	SYS.messDebug("/DNP3",tr("Broken sequence, rejected"));
				ctx.firSeq = tc&0x3F;
			}
		}
		//  Final or Pass
		else if(ctx.firSeq < 0 || (((wSeq=ctx.firSeq+1)>=64)?0:wSeq) != (tc&0x3F)) {	//Broken sequence
			SYS.messDebug("/DNP3",tr("Broken sequence, rejected"));
			ctx.firSeq = -1;
		}
		else if(ctx.firSeq >= 0) {																		//Good sequence
			ctx.inAMess += data.slice(1);
			ctx.firSeq = (tc&0x80) ? -1 : (tc&0x3F);	//Not Final
		}

		// Process of the reassigned application message
		if(ctx.inAMess.length && (tc&0x80)) {
			am = ctx.inAMess.slice(1), ctx.inAMess = "";	//Get the application message
			ac = am.charCodeAt(0);
			SYS.messDebug("/DNP3", "Input application message: "+SYS.strDecode(am,"Bin"," "));

			//  Checking and disabling the Spontaneous Messages
			if(am.charCodeAt(1) == 0x82 && !ctx.spontEn)	mess(0x15, SYS.strFromCharCode(0x3C,0x02,0x06,0x3C,0x03,0x06,0x3C,0x04,0x06));
			//  Confirmation
			if(ac&0x20)	mess(0x00, "", 0x80|0x40|(ac&0x1F));
			//  Processing the response package
			if((am.charCodeAt(1) == 0x81 && ctx.reqToResp.length && ctx.reqToResp.toInt() == (ac&0x0F)) || am.charCodeAt(1) == 0x82) {
				intInd = am.charCodeAt(2,"UTF-16BE");
				if(intInd&0x8000) t_err += tr("Device Restart")+"; ";
				if(intInd&0x4000) t_err += tr("Device Trouble")+"; ";
				if(intInd&0x2000) t_err += tr("Digital Outputs in Local")+"; ";
				if(intInd&0x1000) t_err += tr("Time Sync Required")+"; ";
				if(intInd&0x0800) t_err += tr("Class 3 Data Available")+"; ";
				if(intInd&0x0400) t_err += tr("Class 2 Data Available")+"; ";
				if(intInd&0x0200) t_err += tr("Class 1 Data Available")+"; ";
				if(intInd&0x0020) t_err += tr("Configuration Corrupt")+"; ";
				if(intInd&0x0010) t_err += tr("Operation Already Executing")+"; ";
				if(intInd&0x0008) t_err += tr("Event Buffer Overflow")+"; ";
				if(intInd&0x0004) t_err += tr("Parameters Invalid or Out of Range")+"; ";
				if(intInd&0x0002) t_err += tr("Requested Objects Unknown")+"; ";
				if(intInd&0x0001) t_err += tr("Function Code not implemented")+"; ";
				//respToF = ctx.reqToResp.parse(1,":").toInt();
				ctx.reqToResp = "";	//The propper response came
				if(intInd&0x8000)	mess(0x2, SYS.strFromCharCode(0x50,0x01,0x00,0x07,0x07,0x00));	//Restarting the server: Write 0x5001, 07, 0 (false)
				//Process the function result
				else //if(respToF == 0x01)	//Read
					for(off = 4; off < am.length; ) {
						if((off+3) >= am.length)	{ SYS.messDebug("/DNP3",tr("Not enough of the data in %1, the processing terminated").replace("%1",off.toString())); break; }
						dtObj = am.charCodeAt(off), dtObjV = am.charCodeAt(off+1), dtQf = am.charCodeAt(off+2), off += 3;
						dtRcode = (dtQf&0xF);	//Range code
						pntStart = pntStop = 0;
						if(dtRcode == 0)				//8-bit start and stop
							pntStart = am.charCodeAt(off), pntStop = am.charCodeAt(off+1), off += 2;
						else if(dtRcode == 1)		//16-bit start and stop
							pntStart = am.charCodeAt(off, "UTF-16"), pntStop = am.charCodeAt(off+2, "UTF-16"), off += 4;
						else if(dtRcode == 7)		//8-bit Single Field Quantity
							pntStop = am.charCodeAt(off)-1, off += 1;
						else if(dtRcode == 8)		//16-bit Single Field Quantity
							pntStop = am.charCodeAt(off, "UTF-16")-1, off += 2;
						else { SYS.messWarning("/DNP3",tr("Unsupported data range code %1 in %2, the processing terminated").replace("%1",dtRcode.toString()).replace("%2",off.toString())); break; }
						if(pntStart > pntStop)	{ SYS.messDebug("/DNP3",tr("Wrong points range [%1...%2] in %3, the processing terminated").replace("%1",pntStart.toString()).replace("%2",pntStop.toString()).replace("%3",off.toString())); break; }
						SYS.messDebug("/DNP3","pntStart="+pntStart+"; pntStop="+pntStop+"; off="+off);
						
						if(dtObj == 1) {					//Binary Input
							if(dtObjV == 2) {			//.with status
								if((off+(pntStop-pntStart+1)) > am.length)	{ SYS.messDebug("/DNP3",tr("Not enough of the data in %1, the processing terminated").replace("%1",off.toString())); break; }
								for( ; pntStart <= pntStop; pntStart++, off++)
									inVal("di"+pntStart, ((QLT=am.charCodeAt(off))&0x80)?true:false, QLT);
							} else { SYS.messWarning("/DNP3",tr("Binary Input (1): unsupported variant ''%1'', the processing terminated").replace("%1",dtObjV.toString())); 	break; }
						}
						else if(dtObj == 2) {			//Binary Input Changed
							if(dtObjV == 1) {			//.with status
								if((off+3*(pntStop-pntStart+1)) > am.length)	{ SYS.messDebug("/DNP3",tr("Not enough of the data in %1, the processing terminated").replace("%1",off.toString())); break; }
								for( ; pntStart <= pntStop; pntStart++, off += 3)
									inVal("di"+am.charCodeAt(off,"UTF-16"), ((QLT=am.charCodeAt(off+2))&0x80)?true:false, QLT);
							} else { SYS.messWarning("/DNP3",tr("Binary Input Changed (2): unsupported variant ''%1'', the processing terminated").replace("%1",dtObjV.toString())); 	break; }
						}
						else if(dtObj == 3) {			//Double-bit Input
							if(dtObjV == 2) {			//.with status
								if((off+(pntStop-pntStart+1)) > am.length)	{ SYS.messDebug("/DNP3",tr("Not enough of the data in %1, the processing terminated").replace("%1",off.toString())); break; }
								for( ; pntStart <= pntStop; pntStart++, off++)
									inVal("di"+pntStart, (((QLT=am.charCodeAt(off))&0xC0)==0xC0||(QLT&0xC0)==0)?EVAL:(QLT&0x40)?true:false, QLT);
							} else { SYS.messWarning("/DNP3",tr("Double-bit Input (3): unsupported variant ''%1'', the processing terminated").replace("%1",dtObjV.toString())); 	break; }
						}
						else if(dtObj == 4) {			//Double-bit Input Changed
							if(dtObjV == 1) {			//.with status
								if((off+3*(pntStop-pntStart+1)) > am.length)	{ SYS.messDebug("/DNP3",tr("Not enough of the data in %1, the processing terminated").replace("%1",off.toString())); break; }
								for( ; pntStart <= pntStop; pntStart++, off += 3)
									inVal("di"+am.charCodeAt(off,"UTF-16"), (((QLT=am.charCodeAt(off+2))&0xC0)==0xC0||(QLT&0xC0)==0)?EVAL:(QLT&0x40)?true:false, QLT);
							} else { SYS.messWarning("/DNP3",tr("Double-bit Input Changed (4): unsupported variant ''%1'', the processing terminated").replace("%1",dtObjV.toString())); 	break; }
						}
						else if(dtObj == 10) {			//Binary Output
							if(dtObjV == 2) {			//.with status
								if((off+(pntStop-pntStart+1)) > am.length)	{ SYS.messDebug("/DNP3",tr("Not enough of the data in %1, the processing terminated").replace("%1",off.toString())); break; }
								for( ; pntStart <= pntStop; pntStart++, off++)
									inVal("do"+pntStart, ((QLT=am.charCodeAt(off))&0x80)?true:false, QLT, true);
							} else { SYS.messWarning("/DNP3",tr("Binary Output (10): unsupported variant ''%1'', the processing terminated").replace("%1",dtObjV.toString())); 	break; }
						}
						else if(dtObj == 20 || dtObj == 21) {	//Binary Counter and Frozen Binary Counter
							if(dtObjV == 1) {							//.default
								if((off+5*(pntStop-pntStart+1)) > am.length)	{ SYS.messDebug("/DNP3",tr("Not enough of the data in %1, the processing terminated").replace("%1",off.toString())); break; }
								for( ; pntStart <= pntStop; pntStart++, off += 5)
									inVal("cnt"+pntStart, am.charCodeAt(off+1,"UTF-32"), am.charCodeAt(off));
							} else { SYS.messWarning("/DNP3",tr("Binary Counter (20) or Frozen Binary Counter (21): unsupported variant ''%1'', the processing terminated").replace("%1",dtObjV.toString())); 	break; }
						}
						else if(dtObj == 22 || dtObj == 23) {	//Binary Counter Changed and Frozen Binary Counter Changed
							if(dtObjV == 1) {							//.default
								if((off+7*(pntStop-pntStart+1)) > am.length)	{ SYS.messDebug("/DNP3",tr("Not enough of the data in %1, the processing terminated").replace("%1",off.toString())); break; }
								for( ; pntStart <= pntStop; pntStart++, off += 7)
									inVal("cnt"+am.charCodeAt(off,"UTF-16"), am.charCodeAt(off+3,"UTF-32"), am.charCodeAt(off+2));
							} else { SYS.messWarning("/DNP3",tr("Binary Counter Changed (22) or Frozen Binary Counter Changed (23): unsupported variant ''%1'', the processing terminated").replace("%1",dtObjV.toString())); 	break; }
						}
						else if(dtObj == 30) {			//Analog Input
							if(dtObjV == 1) {			//.32-bit integer value with flag
								if((off+5*(pntStop-pntStart+1)) > am.length)	{ SYS.messDebug("/DNP3",tr("Not enough of the data in %1, the processing terminated").replace("%1",off.toString())); break; }
								for( ; pntStart <= pntStop; pntStart++, off += 5)
									inVal("ai"+pntStart, am.charCodeAt(off+1,"UTF-32"), am.charCodeAt(off));
							} else if(dtObjV == 5) {	//.32-bit floating point value with flag
								if((off+5*(pntStop-pntStart+1)) > am.length)	{ SYS.messDebug("/DNP3",tr("Not enough of the data in %1, the processing terminated").replace("%1",off.toString())); break; }
								for( ; pntStart <= pntStop; pntStart++, off += 5)
									inVal("ai"+pntStart, Special.FLibSYS.floatMergeWord(am.charCodeAt(off+1,"UTF-16"),am.charCodeAt(off+3,"UTF-16")), am.charCodeAt(off));
							} else { SYS.messWarning("/DNP3",tr("Analog Input (30): unsupported variant ''%1'', the processing terminated").replace("%1",dtObjV.toString())); 	break; }
						}
						else if(dtObj == 32) {			//Analog Input Changed
							if(dtObjV == 7) {			//.32-bit floating point value with flag and event time
								if((off+13*(pntStop-pntStart+1)) > am.length)	{ SYS.messDebug("/DNP3",tr("Not enough of the data in %1, the processing terminated").replace("%1",off.toString())); break; }
								for( ; pntStart <= pntStop; pntStart++, off += 13)
									inVal("ai"+am.charCodeAt(off,"UTF-16"), Special.FLibSYS.floatMergeWord(am.charCodeAt(off+3,"UTF-16"),am.charCodeAt(off+5,"UTF-16")), am.charCodeAt(off+2));
							} else { SYS.messWarning("/DNP3",tr("Analog Input Changed (32): unsupported variant ''%1'', the processing terminated").replace("%1",dtObjV.toString())); 	break; }
						}
						else if(dtObj == 40) {			//Analog Output
							if(dtObjV == 1) {			//.32-bit integer value with flag
								if((off+5*(pntStop-pntStart+1)) > am.length)	{ SYS.messDebug("/DNP3",tr("Not enough of the data in %1, the processing terminated").replace("%1",off.toString())); break; }
								for( ; pntStart <= pntStop; pntStart++, off += 5)
									inVal("ao"+pntStart, am.charCodeAt(off+1,"UTF-32"), am.charCodeAt(off), true);
							} else { SYS.messWarning("/DNP3",tr("Analog Output (40): unsupported variant ''%1'', the processing terminated").replace("%1",dtObjV.toString())); 	break; }
						}
						else if(dtObj == 50) {			//Time and Date
							if(dtObjV == 4) {			//.Indexed absolute time and long interval
								if((off+5*(pntStop-pntStart+1)) > am.length)	{ SYS.messDebug("/DNP3",tr("Not enough of the data in %1, the processing terminated").replace("%1",off.toString())); break; }
								for( ; pntStart <= pntStop; pntStart++, off += 11) ;
									//inVal("ao"+pntStart, am.charCodeAt(off+1,"UTF-32"), am.charCodeAt(off));
							} else { SYS.messWarning("/DNP3",tr("Time and Date (4): unsupported variant ''%1'', the processing terminated").replace("%1",dtObjV.toString())); 	break; }
						}
						else if(dtObj == 110) {			//Time and Date
							if((off+dtObjV*(pntStop-pntStart+1)) > am.length)	{ SYS.messDebug("/DNP3",tr("Not enough of the data in %1, the processing terminated").replace("%1",off.toString())); break; }
							for( ; pntStart <= pntStop; pntStart++, off += dtObjV) ;
						}
						else { SYS.messWarning("/DNP3",tr("Unsupported data object ''%1'', the processing terminated").replace("%1",dtObj.toString())); break; }
					}
			}
		}
	}
	return "0"+(t_err.length?":"+t_err:"");
}

if(f_start) {
	itemsSet_ = "";
	items = new Object();
	oAVals = new Object();
	transport_ = "", tr = EVAL;
	tmRetr_ = tmResp_ = tmRetr;
	tmPoolEv_ = 0;
	tmPoolAll_ = tmPoolAll;
	ctx = new Object();

	return;
}
if(f_stop) {
	if(!tr.isEVal()) tr.start(false);
	return;
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
			iIt_EndIOA = iIt_IOA.slice(iIt_EndIOA+1).toInt(0);
			iIt_IOA = iIt_IOA.slice(0,iIt_EndIOA).toInt(0);
		}
		else { iIt_IOA = iIt_EndIOA = iIt_IOA.toInt(0); iIt_sDscr = iIt_nmBase.length; }
		if(iIt_tp == "ai")			{ iIt_nmBase = iIt_nmBase.length ? iIt_nmBase : "AI"; iIt_vtp = "real,ro"; }
		else if(iIt_tp == "ao")	{ iIt_nmBase = iIt_nmBase.length ? iIt_nmBase : "AO"; iIt_vtp = "real"; }
		else if(iIt_tp == "cnt")	{ iIt_nmBase = iIt_nmBase.length ? iIt_nmBase : "CNTR"; iIt_vtp = "int,ro"; }
		else if(iIt_tp == "di")	{ iIt_nmBase = iIt_nmBase.length ? iIt_nmBase : "DI"; iIt_vtp = "boolean,ro"; }
		else if(iIt_tp == "do")	{ iIt_nmBase = iIt_nmBase.length ? iIt_nmBase : "DO"; iIt_vtp = "boolean"; }
		//else if(iIt_tp == "s")		{ iIt_nmBase = iIt_nmBase.length ? iIt_nmBase : "STR"; iIt_vtp = "string,ro"; }
		else continue;
		while(iIt_IOA <= iIt_EndIOA) {
			aId = iIt_tp+iIt_IOA; aDscr = iIt_nmBase+(iIt_sDscr?"":"["+iIt_IOA+"]"); aWr = (iIt_tp == "ao" || iIt_tp == "do");
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

	tmPoolAll_ = tmPoolAll;	//and reread
}

//Check for the transport change and connect
t_err = "0";
if(tr.isEVal() || transport != transport_) {
	itemsSet_ = "";
	items = new Object();
	oAVals = new Object();
	tr = (tr_=SYS.Transport.nodeAt(transport, ".")) ? tr_ : EVAL;
	transport_ = transport;
	if(!tr.isEVal()) tr.start(false);
	tmRetr_ = tmResp_ = tmRetr;
	tmPoolEv_ = 0;
	tmPoolAll_ = tmPoolAll;
}
if(tr.isEVal())	t_err = "1:"+tr("Output transport ''%1'' error.").replace("%1",transport);
else if(dest < 0 || dest > 65520)
	t_err = "2:"+tr("Destination address ''%1'' out of range [0...65520].").replace("%1",dest.toString());
else if(src < 0 || src > 65520)
	t_err = "2:"+tr("Source address ''%1'' out of range [0...65520].").replace("%1",src.toString());
else if(!tr.start()) {
	if(tmRetr_ >= tmRetr) {
		tr.start(true);
		itemsSet_ = "";
		tmRetr_ = tmResp_ = 0;
		tmPoolEv_ = 0;
		tmPoolAll_ = tmPoolAll;
		ctx = new Object();
	} else tmRetr_ += 1/f_frq;
	if(!tr.start())	t_err = "1:"+tr("Output transport ''%1'' stoped.").replace("%1",transport);
}
else {
	//Init the context
	if(ctx.in.isEVal()) {
		ctx.in = ctx.out = ctx.inAMess = "";
		ctx.trSeq = ctx.appSeq = 0;
		ctx.firSeq = -1;
		ctx.spontEn = false;		//Spontanous messages disabled
		ctx.readAll = false;		//All values readed, stage
		ctx.reqToResp = "";
	}

	//Send output buffer and read the transport for an input data
	if(ctx.out.length) SYS.messDebug("/DNP3",tr("Good output sequence")+": "+SYS.strDecode(ctx.out,"Bin"," "));
	ctx.in += tr.messIO(ctx.out, -0.001);
	ctx.out = "";

	//Input processing
	t_err = processIn();

	//Response timeout
	if(ctx.reqToResp.length) {
		if(tmResp_ > tmRetr)	tr.start(false);	//No response long time
		tmResp_ += 1/f_frq;
	}
	else {
		//Writing
		// By attributes
		for(iIt in oAVals)
			if((aO=this[iIt]) && !(aOval=aO.get()).isEVal() && aOval != oAVals[iIt]) {
				if(iIt.slice(0,2) == "do")
					mess(0x02, SYS.strFromCharCode(10,2,0,iIt.slice(2).toInt(),iIt.slice(2).toInt(),aOval?1:0));
				else if(iIt.slice(0,2) == "ao")
					mess(0x02, SYS.strFromCharCode(40,1,0,iIt.slice(2).toInt(),iIt.slice(2).toInt(),0)+SYS.strFromCharUTF("UTF-32",aOval));
				oAVals[iIt] = aOval;
			}
		// By items object
		for(iIt in items)
			if((aO=items[iIt]) && !(aOval=aO.set).isEVal()) {
				if(iIt.slice(0,2) == "do")
					mess(0x02, SYS.strFromCharCode(10,2,0,iIt.slice(2).toInt(),iIt.slice(2).toInt(),aOval?1:0));
				else if(iIt.slice(0,2) == "ao")
					mess(0x02, SYS.strFromCharCode(40,1,0,iIt.slice(2).toInt(),iIt.slice(2).toInt(),0)+SYS.strFromCharUTF("UTF-32",aOval));
				aO.set = EVAL_BOOL;
			}

		//Polling
		// Read all classes
		if(tmPoolAll_ > tmPoolAll)		{ mess(0x1, SYS.strFromCharCode(0x3C,0x02,0x06,0x3C,0x03,0x06,0x3C,0x04,0x06,0x3C,0x01,0x06));	tmPoolAll_ = 0; ctx.readAll = true; }
		//Read periodically the class 1
		else if(tmPoolEv_ > tmPoolEv) {
			// Enable the spontanous messages
			if(!ctx.spontEn)	{ mess(0x14, SYS.strFromCharCode(0x3C,0x02,0x06,0x3C,0x03,0x06,0x3C,0x04,0x06)); ctx.spontEn = true; }
			else mess(0x1, SYS.strFromCharCode(0x3C,0x02,0x06));
			tmPoolEv_ = 0;
		}

		if(ctx.readAll)	tmPoolEv_ += 1/f_frq;
		tmPoolAll_ += 1/f_frq;
		tmResp_ = 0;
	}
}

//Error set
if(t_err.toInt()) {
	if(!tr.isEVal() && tr.start()) tr.start(false);
	if(f_err != t_err) {
		items = new Object();
		oAVals = new Object();
		aLs = this.nodeList("a_");
		for(iA = 0; iA < aLs.length; iA++)
			if((aPref=aLs[iA].slice(2,4)) == "di" || aPref == "do" || aPref == "ai" || aPref == "ao")
				this[aLs[iA].slice(2)].set(EVAL_INT, 0, 0, true);
		SYS.messDebug("/DNP3", tr("Error")+": "+t_err);
	}
}
f_err = t_err;
','','',1592158403);
INSERT INTO tmplib_DevLib VALUES('IEC62056','IEC-62056','','','IEC 62056 part 46 is one from the IEC 62056 set of standards which defines systems used for telecontrol (supervisory control and data acquisition) in electrical engineering and power system automation applications. Part 46 defines Data link layer using HDLC protocol, which uses mostly on serial interfaces.

Author: Roman Savochenko <roman@oscada.org>
State of the protocol implementing: Client of the specification part 46, reading of the directly specified OBIS
Total complexity: 3.2 HD
Sponsored by: SVItoVYR Ltd for whole complexity
Version: 1.1.2
License: GPLv2','IEC 62056 у частині 46 є одним з набору стандартів IEC 62056 який визначає системи, що використовуються у віддаленому контролі (телемеханіці — диспетчерському контролі та зборі даних) у інженерній електриці та у застосунках автоматизації енергетичних систем. Частина 46 надає шар підключення Даних з використанням протоколу HDLC, який переважно використовується на послідовних інтерфейсах.

Автор: Роман Савоченко <roman@oscada.org>
Стан реалізації протоколу: Клієнт частини 46 специфікації, читання прямо визначених OBIS
Загальна працемісткість: 3.2 ЛД
Спонсорування: ТОВ "СВІТоВИР АВТоМАТИК" на загальну працемісткість
Версія: 1.1.2
Ліцензія: GPLv2','',10,0,'JavaLikeCalc.JavaScript
function CRC( inSeq ) { return Special.FLibSYS.CRC(inSeq, 16, 0x1021, -1, true, true, 0xFFFF); }

function ASN1_Data( inSeq, offIn ) {
	val = EVAL;
	offIn += 1;
	if((vTp=inSeq.charCodeAt(offIn-1)) == 0)	//NULL
		val = 0;
	else if(vTp == 1 || vTp == 2) {		//Array, Structure
		itSz = inSeq.charCodeAt(offIn);
		val = new Array(itSz);
		offIn += 1;
		for(itI = 0; itI < itSz; itI++)
			val[itI] = ASN1_Data(inSeq, offIn);
	}
	else if(vTp == 4 || vTp == 18) {		//BIT STRING, Unsigned16
		val = inSeq.charCodeAt(offIn, "UTF-16BE");
		offIn += 2;
	}
	else if(vTp == 5) {							//Integer32
		val = inSeq.charCodeAt(offIn, "UTF-32BE");
		if(val > 2147483647) val -= 4294967296;
		offIn += 4;
	}
	else if(vTp == 6) {							//Unsigned32
		val = inSeq.charCodeAt(offIn, "UTF-32BE");
		offIn += 4;
	}
	else if(vTp == 9 || vTp == 10) {		//OCTET STRING, VisibleString
		itSz = inSeq.charCodeAt(offIn);
		val = inSeq.slice(offIn+1, offIn+1+itSz);
		offIn += 1+itSz;
	}
	else if(vTp == 13 || vTp == 15) {		//Integer8
		val = inSeq.charCodeAt(offIn);
		if(val > 127) val -= 256;
		offIn += 1;
	}
	else if(vTp == 16) {						//Integer16
		val = inSeq.charCodeAt(offIn, "UTF-16BE");
		if(val > 32767) val -= 65536;
		offIn += 2;
	}
	else if(vTp == 17 || vTp == 22) {		//Unsigned8, ENUMERATED
		val = inSeq.charCodeAt(offIn);
		offIn += 1;
	}
	else if(vTp == 18) {						//Unsigned16
		val = inSeq.charCodeAt(offIn, "UTF-16BE");
		offIn += 2;
	}
	else {
		SYS.messDebug("/IEC62056", tr("Unknown data type %1, termination").replace("%1",vTp.toString()));
		offIn = inSeq.length;
	}

	return val;
}

function inVal( aId, aVal ) {
	aId_ = aId.replace(".", "_"); 
	if(items[aId_].isEVal()) {
		items[aId_] = itW = new Object();
		itW.descr = aId;
		itW.wr = false; itW.alarm = 0;
	} else itW = items[aId_];
	itW.val = aVal;
	if((aO=this[aId_]).isEVal()) {
		if((aTp=typeof(aVal)) == "array")	aTp = "object";
		else if(aTp == "real") ;
		else if(aTp == "int")	aTp = "integer";
		else aTp = "string";
		this.attrAdd(aId_, aId, aTp+",ro");
		aO = this[aId_];
	}
	aO.set(aVal, 0, 0, true);
}

function processIn( isSync ) {
	t_err = "";
	//Parse for packages in general - the transport layer
	for(waitSz = 0; ctx.in.length; ctx.in = ctx.in.slice(waitSz)) {
		// Checking for very short
		if(ctx.in.length < 9)	{ if(isSync == true) return "10:"+tr("Too short"); break; }
		// Checking the header entirety
		if(ctx.in[0] != "~" || ((form=ctx.in.charCodeAt(1))&0xF0) != 0xA0 || ((dataSz=ctx.in.charCodeAt(1,"UTF-16BE")&0x07FF) < 7) ||
				//  Checking the source address
				!((src_=ctx.in.charCodeAt(3))&1) || (src_>>1) != src) {
			SYS.messDebug("/IEC62056",tr("Wrong header or source address, rejected")+": "+SYS.strDecode(ctx.in,"Bin"," "));
			if(isSync == true) return "11:"+tr("Wrong header or source address, rejected");
			ctx.in = "";
			break;
		}
		//  Checking the destination address
		if(((dst_=ctx.in.charCodeAt(4))&1) && (dst_>>1) == ctx.destUp && ctx.destLow == 0)
			dst_sz = 1;
		else if(!((dst_=ctx.in.charCodeAt(4))&1) && ((dst_2=ctx.in.charCodeAt(5))&1) && (dst_>>1) == ctx.destUp && (dst_2>>1) == ctx.destLow)
			dst_sz = 2;
		else if(!((dst_=ctx.in.charCodeAt(4))&1) && !(ctx.in.charCodeAt(5)&1) && !((dst_2=ctx.in.charCodeAt(6))&1) && (ctx.in.charCodeAt(7)&1) &&
				(((dst_>>1)<<7)|(ctx.in.charCodeAt(5)>>1)) == ctx.destUp && (((dst_2>>1)<<7)|(ctx.in.charCodeAt(7)>>1)) == ctx.destLow)
			dst_sz = 4;
		else {
			SYS.messDebug("/IEC62056",tr("Wrong destination address, rejected")+": "+SYS.strDecode(ctx.in,"Bin"," "));
			if(isSync == true) return "11:"+tr("Wrong destination address, rejected");
			ctx.in = "";
			break;
		}
		// Short yet, waiting
		if(ctx.in.length < (7+dst_sz))	{ if(isSync == true) return "10:"+tr("Too short"); break; }
		//  Checking the header CRC
		if(CRC(ctx.in.slice(1,5+dst_sz)) != ctx.in.charCodeAt(5+dst_sz,"UTF-16LE")) {
			SYS.messDebug("/IEC62056",tr("CRC-error, rejected")+": "+SYS.strDecode(ctx.in,"Bin"," "));
			if(isSync == true) return "11:"+tr("CRC-error, rejected");
			ctx.in = "";
			break;
		}
		// Checking the frame entirety
		//  Not full yet, waiting
		if(ctx.in.length < (waitSz=2+dataSz)) { if(isSync == true) return "10:"+tr("Too short"); break; }
		if(ctx.in[waitSz-1] != "~") {
			SYS.messDebug("/IEC62056",tr("The end symbol is wrong")+": "+SYS.strDecode(ctx.in,"Bin"," "));
			if(isSync == true) return "11:"+tr("The end symbol is wrong");
			ctx.in = "";
			break;
		}
		//  Checking the frame CRC with the data
		if(dataSz > (6+dst_sz) && CRC(ctx.in.slice(1,1+dataSz-2)) != ctx.in.charCodeAt(1+dataSz-2,"UTF-16LE")) {
			SYS.messDebug("/IEC62056",tr("CRC-error, rejected")+": "+SYS.strDecode(ctx.in,"Bin"," "));
			if(isSync == true) return "11:"+tr("CRC-error, rejected");
			ctx.in = "";
			break;
		}

		SYS.messDebug("/IEC62056",tr("Good input sequence")+": "+SYS.strDecode(ctx.in.slice(0,waitSz),"Bin"," "));

		cntr = ctx.in.charCodeAt(4+dst_sz);
		// Checking the control byte, sequinces and the frame segments
		if((cntr&0x7) == 1) {	//RR
			SYS.messDebug("/IEC62056", "RR"+(cntr>>5)+((cntr&0x10)?"F":""));
			if(ctx.reqToResp == "RR")	ctx.reqToResp = "";
			continue;
		}
		else if(!(cntr&1)) {	//I
			SYS.messDebug("/IEC62056", "I"+((cntr>>1)&0x7)+","+(cntr>>5)+((cntr&0x10)?"F":""));
			if(ctx.sendSeq != (cntr>>5)) {
				SYS.messDebug("/IEC62056", tr("Broken sequence %1, rejected").replace("%1",ctx.sendSeq.toString()));
				continue;
			}
			if(ctx.readSeq >= 7) ctx.readSeq = 0; else ctx.readSeq++;
			if(cntr&0x10)	ctx.sendRR = true; //mess("RR");		//F - final
		}

		ctx.inAMess += ctx.in.slice(7+dst_sz, 1+dataSz-2);
		if(form&0x8)	continue;	//Waiting the next segment

		// Getting the data
		if(cntr == 0x73)	{	//UA
			SYS.messDebug("/IEC62056", "UA");
			if(ctx.toInit == 1 && ctx.reqToResp == "SNRM")	{
				ctx.toInit++;
				//!!!! Parsing the SNRM result
			}
			ctx.reqToResp = "";
		}
		else if((cntr&0x11) == 0x10) {	//I,F
			if(ctx.inAMess.slice(0,4) == SYS.strFromCharCode(0xE6,0xE7,0x00,0x61)) {	//LPDU, APPLICATION 1, Initial parameter acquiring
				if(ctx.inAMess.indexOf(SYS.strFromCharCode(0xA2,0x03,0x02,0x01,0x00)) >= 0) ctx.toInit++;
				else { t_err += tr("Error the authentication")+"; "; tr.start(false); }
				//!!!! Deep parsing the initial parameter acquiring
			}
			else if(ctx.inAMess.slice(0,5) == SYS.strFromCharCode(0xE6,0xE7,0x00,0xC4,0x01)) {	//LPDU, GET-Response-Normal
				tVl = ctx.inAMess.charCodeAt(5);		//Invoke-Id-And-Priority
				// Get-Data-Result
				if((tVl=ctx.inAMess.charCodeAt(6)) == 0) {	// Data
					val = ASN1_Data(ctx.inAMess, 7);
					//Processing the result
					// Array of the variables with OBIS and values
					if(typeof(val) == "array" && val.length == 1 && typeof(val[0]) == "array" && val[0].length && typeof(val[0][0]) == "array" && val[0][0].length == 3)
						for(iIt = 0; iIt < val[0].length; iIt++) {
							wIt = val[0][iIt];
							tVl = val[0][iIt][0];
							for(iIt_OBIS = "", iN = 0; iN < tVl.length; iN++)
								iIt_OBIS += (iIt_OBIS.length?".":"") + tVl.charCodeAt(iN).toString();
							inVal(iIt_OBIS, val[0][iIt][1]*pow(10,val[0][iIt][2][0]));
						}
					// Single variable
					else inVal(ctx.reqOBIS, val);
					SYS.messDebug("/IEC62056", "Data value: "+typeof(val));
				}
				else if(tVl == 1)										// Error
					t_err += tr("Error the data %1").replace("%1",ctx.inAMess.charCodeAt(7).toString())+"; ";
			}
		}
		ctx.inAMess = "";
	}
	return "0"+(t_err.length?":"+t_err:"");
}

//Forming of the message and placing it to the output buffer
//  Commands: "SNRM", "RR", "I", "DISC"
function mess( com, data ) {
	if(data.isEVal())	data = "";
	mLen = 10 + (data.length ? data.length+2 : 0);
	 //SYS.strFromCharCode(ac, func) + objs;
	aMess = SYS.strFromCharCode(0xA0|(mLen>>8), mLen&0xFF);
	if(!ctx.destLow && ctx.destUp < 128)					aMess += SYS.strFromCharCode(((ctx.destUp&0x7F)<<1)|1);
	else if(ctx.destUp < 128 && ctx.destLow < 128)	aMess += SYS.strFromCharCode((ctx.destUp&0x7F)<<1, ((ctx.destLow&0x7F)<<1)|1);
	else aMess += SYS.strFromCharCode((ctx.destUp>>7)<<1, (ctx.destUp&0x7F)<<1, (ctx.destLow>>7)<<1, ((ctx.destLow&0x7F)<<1)|1);
	aMess += SYS.strFromCharCode((src<<1)|1);
	ctx.reqToResp = com;
	if(com == "SNRM")		aMess += SYS.strFromCharCode(0x93);
	else if(com == "RR")	aMess += SYS.strFromCharCode((ctx.readSeq<<5)|0x11);
	else if(com == "DISC")	aMess += SYS.strFromCharCode(0x53);
	else if(com == "I") {
		aMess += SYS.strFromCharCode(0x10|(ctx.readSeq<<5)|(ctx.sendSeq<<1));
		if(ctx.sendSeq >= 7) ctx.sendSeq = 0; else ctx.sendSeq++;
	} else return;	
	aMess += SYS.strFromCharUTF("UTF-16LE",CRC(aMess)) + data;
	if(data.length)	aMess += SYS.strFromCharUTF("UTF-16LE",CRC(aMess));
	aMess = SYS.strFromCharCode(0x7E) + aMess + SYS.strFromCharCode(0x7E);
	ctx.out += aMess;
	//Just direct writing per one message
	//SYS.messDebug("/IEC62056",tr("Good output sequence")+": "+SYS.strDecode(aMess,"Bin"," "));
	//tr.messIO(aMess, 0, 0);

	//Synchronous request-response
	SYS.messDebug("/IEC62056",tr("Good output sequence")+": "+SYS.strDecode(ctx.out,"Bin"," "));
	t_err = "0";
	ctx.in = tr.messIO(ctx.out); ctx.out = "";
	if(!ctx.in.length)	{ t_err = "10:"+tr("No response"); tr.start(false); }
	else while((t_err=processIn(true)).toInt() == 10 && (respTail=tr.messIO("")).length) ctx.in += respTail;

	return t_err;
}

if(f_start) {
	itemsSet_ = "";
	items = new Object();
	//oAVals = new Object();
	transport_ = "", tr = EVAL;
	tmRetr_ = tmResp_ = tmRetr;
	tmPollAll_ = tmPollAll; offPoll = 0;
	ctx = new Object();

	return;
}

t_err = "0";

if(f_stop)	t_err = mess("DISC");

//Items set changing process
if(itemsSet != itemsSet_) {
	itemsSet_ = itemsSet;
	tmPollAll_ = tmPollAll; offPoll = 0;		//and reread
}

//Check for the transport change and connect
destUp = dest.parse(0,":").toInt(); destLow = dest.parse(1,":").toInt();
if(tr.isEVal() || transport != transport_)	{
	itemsSet_ = "";
	items = new Object();
	//oAVals = new Object();
	tr = SYS.Transport.nodeAt(transport, ".");
	transport_ = transport;
	if(!tr.isEVal()) tr.start(false);
	tmRetr_ = tmResp_ = tmRetr;
	tmPollAll_ = tmPollAll; offPoll = 0;
}
if(tr.isEVal())	t_err = "1:"+tr("Output transport ''%1'' error.").replace("%1",transport);
else if(destUp < 0 || destUp > 16383)
	t_err = "2:"+tr("Upper HDLC destination address ''%1'' out of range [0...16383].").replace("%1",destUp.toString());
else if(destLow < 0 || destLow > 16383)
	t_err = "2:"+tr("Lower HDLC destination address ''%1'' out of range [0...16383].").replace("%1",destUp.toString());
else if(src < 0 || src > 127)
	t_err = "2:"+tr("Source address ''%1'' out of range [0...127].").replace("%1",src.toString());
else if(!tr.start()) {
	if(tmRetr_ >= tmRetr) {
		tr.start(true);
		itemsSet_ = "";
		tmRetr_ = tmResp_ = 0;
		tmPollAll_ = tmPollAll; offPoll = 0;
		ctx = new Object();
		ctx.destUp = destUp; ctx.destLow = destLow;
		pass_ = pass;
	} else tmRetr_ += 1/f_frq;
	if(!tr.start())	t_err = "1:"+tr("Output transport ''%1'' stoped.").replace("%1",transport);
}
else if(ctx.destUp != destUp || ctx.destLow != destLow || pass != pass_)	tr.start(false);
else {
	//Init the context
	if(ctx.in.isEVal()) {
		ctx.in = ctx.out = ctx.inAMess = "";
		ctx.sendSeq = ctx.readSeq = 0;
		ctx.toInit = 1;
		ctx.firSeq = -1;
		ctx.sendRR = false;
		ctx.reqToResp = "";
	}

	//Send output buffer and read the transport for an input data
	// Asynchronous mode
	//if(ctx.out.length) SYS.messDebug("/IEC62056",tr("Good output sequence")+": "+SYS.strDecode(ctx.out,"Bin"," "));
	//ctx.in += tr.messIO(ctx.out, -0.001);
	//ctx.out = "";

	//Input processing
	//t_err = processIn();

	//Response timeout
	/*if(ctx.reqToResp.length) {
		if(tmResp_ > tmRetr)	tr.start(false);	//No response long time
		tmResp_ += 1/f_frq;
	}
	// Initial parameters setting
	else*/
	if(!t_err.toInt() && ctx.toInit == 1)
		t_err = mess("SNRM", SYS.strFromCharCode(
			0x81,0x80,0x12,		//format and group identifier, length 18 bytes
				0x05,0x01,0x80,	//the parameter "Maximum information field length - transmit" = 128
				0x06,0x01,0x80,	//the parameter "Maximum information field length - receive" = 128
				0x07,0x04,0x00,0x00,0x00,0x01,	//the parameter "Window size - transmit" = 1
				0x08,0x04,0x00,0x00,0x00,0x07	//the parameter "Window size - receive" = 7
			));
	// Initial parameter acquiring
	if(!t_err.toInt() && ctx.toInit == 2)
		t_err = mess("I", SYS.strFromCharCode(
			0xE6,								//Destination (remote) LSAP, fixed in COSEM
			0xE6,								//Source (local) LSAP, fixed in COSEM E6 for command E7 for response
			0x00,								//Quality, Control byte, Reserved and 00 always
			0x60, 46+pass.length,		//APPLICATION 0, Length 54
			0xA1, 0x09, 0x06, 0x07,	//Application-context-name [1] (9 byte), OBJECT IDENTIFIER (7 byte)
				0x60, 0x85, 0x74, 0x05, 0x08, 0x01, 0x01,
			0x8A, 0x02, 0x07, 0x80,	//ACSE-requirements [10], BIT STRING (7) = 0x80
			0x8B, 0x07,						//Security Mechanism-name [11]
				0x60, 0x85, 0x74, 0x05, 0x08, 0x02, 0x01,
			0xAC, pass.length+2, 0x80, pass.length) + //Authentication-value [12], GraphicString (0x80), Key value
				pass + SYS.strFromCharCode(
			0xBE, 0x10, 0x04, 0x0E,	//Association-information [30], Client information, OCTET STRING (4)
				0x01,							// Data tag for the request
				0x00, 0x00, 0x00,		// Flags field
				0x06, 0x5F, 0x1F,		// Application tag, 31
				0x04, 0x00, 0x00, 0x18, 0x1D,	// Client side services
				0x00, 0x00					// PDU maximum size
			));
	if(!t_err.toInt() && ctx.toInit > 2) {
		if(ctx.sendRR)	{ t_err = mess("RR"); ctx.sendRR = false; }
		//Polling
		// Read all items
		if(!t_err.toInt() && tmPollAll_ > tmPollAll) {
			if((iIt=itemsSet.parse(0,"\n",offPoll)).length) {
				iIt_ClassId = iIt.parse(0, "-");
				iIt_OBIS = iIt.parse(1, "-");
				iIt_a = iIt.parse(2, "-");
				ctx.reqOBIS = iIt_OBIS;
				t_err = mess("I", SYS.strFromCharCode(0xE6, 0xE6, 0x00,
					0xC0, 0x01, 0x81,	//Get-Request-Normal, Invoke-Id-And-Priority
					0x00, iIt_ClassId.toInt(),	//Cosem-Class-Id, ?
					iIt_OBIS.parse(0,".").toInt(), iIt_OBIS.parse(1,".").toInt(), iIt_OBIS.parse(2,".").toInt(), iIt_OBIS.parse(3,".").toInt(), iIt_OBIS.parse(4,".").toInt(), iIt_OBIS.parse(5,".").toInt(),
					iIt_a.toInt(),		//Cosem-Object-Attribute-Id
					0x00						//Selective-Access-Descriptor, 0
				));
			}
			if(offPoll >= itemsSet.length) { tmPollAll_ = 0; offPoll = 0; }
		}

		tmPollAll_ += 1/f_frq;
		tmResp_ = 0;
	}
}

//Error set
if(t_err.toInt()) {
	if(!tr.isEVal() && tr.start()) tr.start(false);
	if(f_err != t_err) {
		for(var iIt in items)
			this[iIt].set(EVAL_REAL, 0, 0, true);
		items = new Object();
		SYS.messDebug("/IEC62056", tr("Error")+": "+t_err);
	}
}
f_err = t_err;

if(f_stop && !tr.isEVal()) tr.start(false);','','',1614260893);
INSERT INTO tmplib_DevLib VALUES('MTP4D','MTP 4D','','','The connection template of a simple vaccuumeter MTP 4D of the firm "Erstevak Ltd (http://www.erstvak.com/)".

Author: Roman Savochenko <roman@oscada.org>
Total complexity: 0.3 HD
Sponsored by, for whole complexity: Magomed, SAVTECH
Version: 1.1.0
License: GPLv2','','',10,0,'JavaLikeCalc.JavaScript
//Same request to the device
function req(data) {
	data = addr.toString(10,3) + data;

	//CRC calculation
	for(CRC = 0, iS = 0; iS < data.length; iS++) CRC += data.charCodeAt(iS);
	data += SYS.strFromCharCode((CRC%64)+64, 0x0D);

	for(resp = tr.messIO(data); resp[resp.length-1] != "\r" && (respTail=tr.messIO("")).length; )
		resp += respTail;

	if(resp.length < 6 || resp[resp.length-1] != "\r" || resp.slice(0,4) != data.slice(0,4))
		return "10:"+tr("Wrong or no response.");
	resp = resp.slice(0,-1);
	for(CRC = 0, iS = 0; iS < resp.length-1; iS++) CRC += resp.charCodeAt(iS);
	if(((CRC%64)+64) != resp.charCodeAt(resp.length-1))
		return "11:"+tr("Error CRC.");

	data = resp.slice(3,-1);

	return "0";
}

if(f_start) {
	f_err = "0";
	transport_ = transport;
	type = P = EVAL;
	zeroP = zeroAP = false;
	tr = EVAL;
}

if(f_start || f_stop) return;

t_err = "";

//Check for the transport change and connect
if(tr.isEVal() || transport != transport_)	{
	tr = SYS.Transport[transport.parse(0)]["out_"+transport.parse(1)];
	transport_ = transport;
}
if(tr.isEVal())	t_err = "1:"+tr("Output transport ''%1'' error.").replace("%1",transport);
else if(addr < 1 || addr > 999)
	t_err = "2:"+tr("Address ''%1'' out of range [1...999].").replace("%1",addr.toString());
else {
	//Setting
	if(zeroAP && !t_err.toInt()) {
		data = "j1";
		if(!(t_err=req(data)).toInt()) {
			data = "j100023";
			if(!(t_err=req(data)).toInt())	zeroAP = false;
		}
	}
	if(zeroP && !t_err.toInt()) {
		data = "j0";
		if(!(t_err=req(data)).toInt()) {
			data = "j000000";
			if(!(t_err=req(data)).toInt())	zeroP = false;
		}
	}

	//Reading
	data = "T";
	if(!t_err.toInt() && !(t_err=req(data)).toInt())	type = data.slice(1);
	else type = EVAL;

	data = "M";
	if(!t_err.toInt() && !(t_err=req(data)).toInt())
		P = data.slice(1,5).toReal()*1e-3*pow(10,data.slice(5,7).toReal()-20);
	else P = EVAL;
}

if(t_err.toInt()) { f_err = t_err; type = P = EVAL; }
else f_err = "0";','','',1603038629);
INSERT INTO tmplib_DevLib VALUES('mbBase','ModBus base','','','ModBus base template of the protocol implementing on the Logical Level. Designed mostly to use in the base of creation specific ModBus-based protocols for the standard-part registers'' access (function 3 for reading and function 16 for writing) and appending the new protocol specific part. The template was designed to work both under control of the controller objects of the module ModBus and LogicLev and can be managed from the template of the initiative connections processing for assigned output transports.

Author: Roman Savochenko <roman@oscada.org>
Total complexity: 0.5 HD
Sponsored by, for whole complexity: Elyor Turaboev, BLUE STAR GROUP Ltd
Version: 1.0.0
License: GPLv2','','',30,0,'JavaLikeCalc.JavaScript
//Same request to the device
function req(PDU) {
	// For placing into the module ModBus
	if(!transport.length)	return this.cntr().messIO(PDU);

	// For other logical level
	reqO = SYS.XMLNode(mbType).setAttr("id","Goboy").setAttr("node",addr).setText(PDU);
	if((rez=tr.messIO(reqO,"ModBus")).length)	return "10:"+rez;
	PDU = reqO.text();

	return reqO.attr("err");
}

if(f_start) {
	transport_ = transport;
	tr = EVAL;
	dt = new Object();
	items_ = "";
}

//Parse the items set and create user attributes
if(items != items_) {
	items_ = items;
	// Mark for check to deletion needs
	for(var iDt in dt)
		if(iDt != "10000") dt[iDt].mark = false;
	// Append/Update present ones
	for(off = 0; (sIt=items.parseLine(0,off)).length; ) {
		if(sIt[0] == "#")	continue;
		off1 = 0;
		itO = new Object();
		itO.tp = sIt.parse(0, ":", off1);
		tmpAddr = sIt.parse(0, ":", off1);
		itO.addr = tmpAddr.toInt();
		itO.md = sIt.parse(0, ":", off1);
		itO.id = sIt.parse(0, ":", off1);
		itO.nm = sIt.slice(off1);
		dt[itO.addr.toString(16,5)] = itO;
		if(itO.tp == "u" || itO.tp == "i" || itO.tp == "u2" || itO.tp == "i2")	{ wTp = "integer"; itO.sz = 2; }
		else if(itO.tp == "u4" || itO.tp == "i4")	{ wTp = "integer"; itO.sz = 4; }
		else if(itO.tp == "s")	{ wTp = "string"; itO.sz = 16; }
		else { wTp = "real"; itO.sz = 4; }
		itO.rd = (itO.md.indexOf("r") >= 0);
		itO.wr = (itO.md.indexOf("w") >= 0);
		itO.rev = (itO.md.indexOf("~") >= 0);
		if(!itO.wr)	wTp += "|ro";
		if(itO.rd || itO.wr) {
			this.attrAdd(itO.id, itO.nm, wTp);
			if(itO.wr)	itO.val = this[itO.id].get();
			itO.mark = true;
			//SYS.messInfo("/ED","itO="+itO.id+"; tmpAddr="+tmpAddr+"; addr="+itO.addr);
		}
	}
	dt["10000"] = EVAL;
	// Check, remove item and set to EVAL the attribute
	for(var iDt in dt) {
		if(iDt == "10000" || dt[iDt].mark)	continue;
		this[dt[iDt].id].set(EVAL, 0, 0, true);
		delete dt[iDt];
	}
}

if(f_start || f_stop) return;

tErr = "";

//Checking for the transport change and connect
if(tr.isEVal() || transport != transport_)	{
	if(!transport.length) {
		if(!((tVl=this.cntr().cfg("PROT")) == "RTU" || tVl == "ASCII" || tVl == "TCP")) {
			tr = EVAL;
			tErr = "1:"+tr("Output transport is empty and the controller object is not ModBus.");
		}
		else {
			tr = new Object();
			maxBlkSz = this.cntr().cfg("MAX_BLKSZ");
			if(maxBlkSz.isEVal())	maxBlkSz = 12;
			fragMerge = this.cntr().cfg("FRAG_MERGE");
			if(fragMerge.isEVal())	fragMerge = false;
		}
	} else tr = SYS.Transport[transport.parse(0)]["out_"+transport.parse(1)];
	maxBlkSz = max(10, min(200,maxBlkSz));
	transport_ = transport;
}
if(tErr.toInt()) ;
else if(tr.isEVal())
	tErr = "1:"+tr("Output transport ''%1'' error.").replace("%1",transport);
else if(addr < 0 || addr > 247)
	tErr = "2:"+tr("Address ''%1'' out of range [0...247].").replace("%1",addr.toString());
else {
	//Check for changed attributes and perform writing
	for(var iDt in dt) {
		isEOL = (iDt == "10000");
		itO = dt[iDt];
		tVl = this[itO.id].get();
		if(isEOL || !itO.wr || tVl.isEVal() || itO.val == tVl)	continue;
		PDU = Special.FLibSYS.IO("", "", "b");
		PDU.wr(16,"uint8").wr(itO.addr,"uint16").wr(floor(itO.sz/2),"uint16").wr(itO.sz,"uint8");
		if(itO.tp == "u" || itO.tp == "u2")			PDU.wr(tVl,"uint16");
		else if(itO.tp == "i" || itO.tp == "i2")	PDU.wr(tVl,"int16");
		else if(itO.tp == "u4")	{
			if(itO.rev)	PDU.wr(tVl>>16, "uint16").wr(tVl&0xFFFF, "uint16");
			else	PDU.wr(tVl&0xFFFF, "uint16").wr(tVl>>16, "uint16");
		}
		else if(itO.tp == "i4") {
			if(itO.rev)	PDU.wr(tVl>>16, "int16").wr(tVl&0xFFFF, "uint16");
			else	PDU.wr(tVl&0xFFFF, "uint16").wr(tVl>>16, "int16");
		}
		else if(itO.tp == "s")		PDU.wr(tVl.slice(0,16),"char").wr(" ","char",16-min(16,tVl.length));
		else {
			w1 = w2 = 0;
			if(itO.rev)	Special.FLibSYS.floatSplitWord(tVl, w2, w1);
			else	Special.FLibSYS.floatSplitWord(tVl, w1, w2);
			PDU.wr(w1, "uint16").wr(w2, "uint16");
		}
		//SYS.messInfo("/ED","reqPDU="+SYS.strDecode(PDU.string,"Bin"," "));
		if(!req(PDU.string).toInt())	itO.val = tVl;
		//SYS.messInfo("/ED","respPDU="+SYS.strDecode(PDU.string,"Bin"," "));
	}

	//Same requests for the data
	blk = new Array();
	for(var iDt in dt) {
		isEOL = (iDt == "10000");
		itO = dt[iDt];
		//SYS.messInfo("/ED","iDt="+iDt+"; isEOL="+isEOL);
		if(!isEOL && (!blk.length || (
				(itO.addr-blk[0].addr+1+floor((itO.sz-2)/2)) <= floor(maxBlkSz/2) && (fragMerge || (itO.addr-blk[blk.length-1].addr-floor((blk[blk.length-1].sz-2)/2)) <= 1) ))) {
			if(itO.rd) blk.push(itO);
			continue;
		}
		//Send request for this block
		if(blk.length) {
			regN = (blk[blk.length-1].addr - blk[0].addr) + 1 + floor((blk[blk.length-1].sz-2)/2);
			PDU = SYS.strFromCharCode(3, (blk[0].addr>>8)&0xFF, blk[0].addr&0xFF, 0, regN);
			//SYS.messInfo("/ED","reqPDU="+SYS.strDecode(PDU,"Bin"," "));
			if((tErr=req(PDU)).toInt()) break;
			//SYS.messInfo("/ED","respPDU="+SYS.strDecode(PDU,"Bin"," "));
			io = Special.FLibSYS.IO(PDU, "", "b");
			rF = io.read("uint8", 1); rN = io.read("uint8", 1);
			if(rF != 3 || (io.length-2) != rN) { tErr = "10:"+tr("Inconsistent respond''s length."); break; }
			for(iB = 0; iB < blk.length; iB++) {
				itO1 = blk[iB];
				io.pos = 2 + (itO1.addr-blk[0].addr)*2;
				if(itO1.tp == "u" || itO1.tp == "u2")			tVl = io.read("uint16", 1);
				else if(itO1.tp == "i" || itO1.tp == "i2")	tVl = io.read("int16", 1);
				else if(itO1.tp == "u4")
					tVl = itO1.rev ? io.read("uint16", 1)*65536 + io.read("uint16", 1) :
											io.read("uint16", 1) + io.read("uint16", 1)*65536;
				else if(itO1.tp == "i4")
					tVl = itO1.rev ? io.read("int16", 1)*65536 + io.read("uint16", 1) :
											io.read("uint16", 1) + io.read("int16", 1)*65536;
				else if(itO1.tp == "s")	tVl = io.read("char", blk[0].sz);
				else {
					w1 = io.read("uint16", 1); w2 = io.read("uint16", 1);
					tVl = itO1.rev ? Special.FLibSYS.floatMergeWord(w2, w1) :
											Special.FLibSYS.floatMergeWord(w1, w2);
					//tVl = io.read("float", 1, "l");
				}
				if(itO1.wr && itO1.val != this[itO1.id].get())	{ itO1.val = tVl; continue; }
				this[itO1.id].set(tVl, 0, 0, true);
				itO1.val = tVl;
			}
			blk = new Array(); blk.push(itO);
		}
	}
}

if(!tErr.length)	tErr = "0";
if(tErr.toInt()) {
	if(!tr.isEVal() && tr.start()) tr.start(false);
	if(f_err != tErr)
		for(var iDt in dt) {
			itO = dt[iDt];
			if(iDt == "10000")	continue;
			this[itO.id].set(EVAL, 0, 0, true);
		}
}
f_err = tErr;','','',1601827422);
CREATE TABLE IF NOT EXISTS 'tmplib_PrescrTempl' ("ID" TEXT DEFAULT '' ,"NAME" TEXT DEFAULT '' ,"uk#NAME" TEXT DEFAULT '' ,"ru#NAME" TEXT DEFAULT '' ,"DESCR" TEXT DEFAULT '' ,"uk#DESCR" TEXT DEFAULT '' ,"ru#DESCR" TEXT DEFAULT '' ,"MAXCALCTM" INTEGER DEFAULT '10' ,"PR_TR" INTEGER DEFAULT '1' ,"PROGRAM" TEXT DEFAULT '' ,"uk#PROGRAM" TEXT DEFAULT '' ,"ru#PROGRAM" TEXT DEFAULT '' ,"TIMESTAMP" INTEGER DEFAULT '' , PRIMARY KEY ("ID"));
INSERT INTO tmplib_PrescrTempl VALUES('timer','Command — Timer','Команда — Таймер','Команда — Таймер','Template of a command of the prescription typical timer. The timer is only designed to hold time between other action steps and for example, so it only has one attribute, "Time" in seconds.

Author: Roman Savochenko <roman@oscada.org>
Version: 2.0.0
License: GPLv2','Шаблон команди типового таймеру рецепту. Таймер розроблено лише для витримки часу між іншими дієвими кроками та для прикладу, відтак він містить лише один атрибут, це "Час" у секундах.

Автор: Роман Савоченко <roman@oscada.org>
Версія: 2.0.0
Ліцензія: GPLv2','Шаблон команды типового таймера рецепта. Таймер разработан только для удержания времени между другими действенными шагами и для примера, т.е. он содержит только один атрибут, это "Время" в секундах.

Автор: Роман Савоченко <roman@oscada.org>
Версия: 2.0.0
Лицензия: GPLv2',10,0,'JavaLikeCalc.JavaScript
//Reset to default
if(f_start || (!run && tmp1)) { run = pause = error = abort = false; tmp1 = 0; rez = 1; }
//Run
if(run && !pause) {
	if(tmp1 == 0) tmp1 = arg1+1/f_frq;
	tmp1 = max(0, tmp1 - 1/f_frq);
	if(tmp1 <= 0) { rez = "1:"+tr("Waiting %1s expired").replace("%1",arg1.toPrecision(3)); run = false; }
	else rez = "0:"+tr("Waiting %1s").replace("%1",tmp1.toPrecision(3));
}','','',1584814597);
INSERT INTO tmplib_PrescrTempl VALUES('backTimer','Command — Background timer','Команда — Фоновий таймер','Команда — Фоновый таймер','Template of a command of the prescription background timer. The timer designed only for hold a time in the background to an example, then it contains only one attribute, it is "Time" in seconds.

Author: Roman Savochenko <roman@oscada.org>
Version: 2.0.0
License: GPLv2','Шаблон команди фонового таймеру рецепту. Таймер розроблено лише для витримки часу у фоні для прикладу, відтак він містить лише один атрибут, це "Час" у секундах.

Автор: Роман Савоченко <roman@oscada.org>
Версія: 2.0.0
Ліцензія: GPLv2','Шаблон команды фонового таймера рецепта. Таймер разработан только для удержания времени в фоне для примера, т.е. он содержит только один атрибут, это "Время" в секундах.

Автор: Роман Савоченко <roman@oscada.org>
Версия: 2.0.0
Лицензия: GPLv2',10,0,'JavaLikeCalc.JavaScript
//Reset to default
if(f_start || (!run && tmp1)) { run = pause = error = abort = false; tmp1 = 0; rez = 1; }
//Run
if(run && !pause) {
	if(tmp1 == 0) tmp1 = arg1+1/f_frq;
	tmp1 = max(0, tmp1 - 1/f_frq);
	if(tmp1 <= 0) { rez = "1:"+tr("Waiting %1s expired").replace("%1",arg1.toPrecision(3)); run = false; }
	//if(tmp1 <= 10)	rez = "-1:Waiting "+arg1.toPrecision(3)+"s expired";
	else rez = "10:"+tr("Background waiting %1s").replace("%1",tmp1.toPrecision(3));
}','','',1584814597);
INSERT INTO tmplib_PrescrTempl VALUES('manager','Prescriptions manager','Менеджер рецептів','Менеджер рецептов','Basic, representative and unified template of a manager and a controller of the prescriptions, of their processing and direct execution in the "command-macro" mode. The template forms a structure of the prescription manager parameter which can be easily connected to all frames of this library.

The representative structure of the prescription manager is:
- "DB: DB name with the tables (dbDB)" — DB, like to "SQLite.vcaBase", where stored or must be stored the prescription-program tables.
- "DB: Table with programs (dbProgs)" — name of the prescriptions-programs table, typically "PrescrProgs".
- "Commands list (comLs)" — the allowed to use commands list.
- "Name of the selected program (prog)" — state and setting of the selected program name in processing by the manager.
- "Mode (mode)", "Current mode (curMode)" — the setting and the current manager mode of the selected prescription-program execution: Finish(-2), Error(-1), Stop(0), Run(1), Pause(2), Pass command(3).
- "Start time (startTm)" — the start time in seconds.
- "Current command-step (curCom)" — the currently processing command.
- "Work program (work)" — the actual and processing now program in an object.

Functions:
- Forms the "Commands list (comLs)", allowed to use in the user prescriptions-programs.
- Process the "Name of the selected program (prog)", modifying it will either load its command sequence to the "Work program (work)" or create a new program.
- Execute the program in the "Name of the selected program (prog)" after receiving the command "Run(1)" in the "Mode (mode)" and indicate this process in: the "Current mode (curMode)", the "Start time (startTm)", the "Current command-step (curCom)" and the "Work program (work)".
- Generate the action messages during the prescription execution for:
  CATEGORY: defines the user prescription-program ID ProgNM in the form "uprg{ProgNM}", where:
    - "uprg*" — the typical template-sign of a user prescription-program, that can be directly used in the category filter to determine in the messages only the user prescriptions-programs;
    - ProgNM — the prescription-program name.
  TEXT: the action description in the form "{ActDescr} "{ProgNM}" : {StartTm} : {ActTm}", where:
    - ActDescr — the action description:
      - "No current node present";
      - "Terminated program session by the user";
      - "Terminated program session by the error";
      - "Successful session of the program".
    - ProgNM — the prescription-program name;
    - StartTm — start time of the prescription-program, in the form "2020-03-14 16:05:01";
    - ActTm — action time of the prescription-program, in the form "2020-03-14 16:05:52".

Author: Roman Savochenko <roman@oscada.org>
Version: 2.2.0
License: GPLv2','Основний, представницький та уніфікований шаблон менеджеру та контролеру рецептів, їх опрацювання та прямого виконання у режимі "команда-макрос". Шаблон формує структуру параметру менеджеру рецептів, який може бути легко підключений до всіх кадрів цієї бібліотеки.

Представницькою структурою менеджеру рецептів є:
- "БД: Ім''я БД з таблицями (dbDB)" — БД, на кшталт "SQLite.vcaBase", де зберігаються або мають зберігатися таблиці рецептів-програм.
- "БД: Таблиця з програмами (dbProgs)" — назва таблиці рецептів-програм, типово "PrescrProgs".
- "Перелік команд (comLs)" — перелік команд доступних до використання.
- "Ім''я обраної програми (prog)" — стан та встановлення назви обраної програми у обробці менеджером.
- "Режим (mode)", "Поточний режим (curMode)" — встановлення та поточний режим менеджеру, виконання обраного рецепту-програми: Завершення(-2); Помилка(-1); Стоп(0); Виконання(1); Пауза(2); Пропустити команду(3).
- "Час запуску (startTm)" — час запуску у секундах.
- "Поточна команда-крок (curCom)" — поточна опрацьовувана команда.
- "Робоча програма (work)" — актуальна та опрацьовувана наразі програма у об''єкті.

Функції:
- Формує "Перелік команд (comLs)", доступних до використання у користувацьких рецептах-програмах.
- Опрацьовує "Ім''я обраної програми (prog)", зміна якої призводить до завантаження її послідовності команд до "Робоча програма (work)" або створення нової програми.
- Виконує програму у "Ім''я обраної програми (prog)" після отримання команди "Виконання(1)" у "Режим (mode)" та відзначає цей процес у: "Поточний режим (curMode)", "Час запуску (startTm)", "Поточна команда-крок (curCom)" та "Робоча програма (work)".
- Генерує повідомлення дій протягом виконання рецепту щодо:
  КАТЕГОРІЯ: визначає ідентифікатор користувацького рецепту-програми ProgNM у форматі "uprg{ProgNM}", де:
    - "uprg*" — типовий шаблон-ознака користувацького рецепту-програми, який може бути безпосередньо використано у фільтрі категорії для визначення у повідомленнях суто користувацьких рецептів-програм;
    - ProgNM — ім''я рецепту-програми.
  ТЕКСТ: опис дії у форматі "{ActDescr} "{ProgNM}" : {StartTm} : {ActTm}", де:
    - ActDescr — опис дії:
      - "Поточний вузол відсутній";
      - "Перерваний користувачем сеанс програми";
      - "Перерваний помилкою сеанс програми";
      - "Вдалий сеанс програми".
    - ProgNM — ім''я рецепту-програми;
    - StartTm — час запуску рецепту-програми, у форматі "2020-03-14 16:05:01";
    - ActTm — час дії рецепту-програми, у форматі "2020-03-14 16:05:52".

Автор: Роман Савоченко <roman@oscada.org>
Версія: 2.2.0
Ліцензія: GPLv2','Основной, представительский и унифицированный шаблон менеджера и контроллера рецептов, их обработки и прямого исполнения в режиме "команда-макрос". Шаблон формирует структуру параметра менеджера рецептов, который может быть легко подключен ко всем кадрам этой библиотеки.

Представительской структурой менеджера рецептов является:
- "БД: Имя БД с таблицами" — БД, вроде "SQLite.vcaBase", где хранятся или должны храниться таблицы рецептов-программ.
- "БД: Таблица с программами" — название таблицы рецептов-программ, типично "PrescrProgs".
- "Список команд (comLs)" — перечень команд доступных для использования.
- "Имя выбранной программы (prog)" — состояние и установка названия выбранной программы в обработке менеджером.
- "Режим (mode)", "Текущий режим (curMode)" — установка и текущий режим менеджера, исполнения выбранного рецепта-программы: Завершение(-2); Ошибка(-1); Стоп(0); Исполнение(1); Пауза(2); Пропустить команду(3);
- "Время запуска (startTm)" — время запуска в секундах.
- "Текущая команда-шаг (curCom)" — текущая обрабатываемая команда.
- "Рабочая программа (work)" — актуальная и обрабатываемая на данный момент программа в объекте.

Функции:
- Формирует "Список команд (comLs)", доступных для использования в пользовательских рецептах-программах.
- Обрабатывает "Имя выбранной программы (prog)", смена которой приводит к загрузке её последовательности команд в Рабочая программа (work)" или созданию новой программы.
- Исполняет программу в "Имя выбранной программы (prog)" после получения команды "Исполнение(1)" в "Режим (mode)" и отмечает этот процесс в: "Текущий режим (curMode)", "Время запуска (startTm)", "Текущая команда-шаг (curCom)" и "Рабочая программа (work)".
- Генерирует сообщения действий в течении исполнения рецепта на предмет:
  КАТЕГОРИЯ: определяет идентификатор пользовательского рецепта-программы ProgNM в формате "uprg{ProgNM}", где:
    - "uprg*" — типовой шаблон-признак пользовательского рецепта-программы, который может быть непосредственно использован в фильтре категории для определения чисто пользовательских рецептов-программ;
    - ProgNM — имя рецепта-программы.
  ТЕКСТ: описание действия в формате "{ActDescr} "{ProgNM}" : {StartTm} : {ActTm}", где:
    - ActDescr — описание действия:
      - "Текущий узел отсутствует";
      - "Прерванный пользователем сеанс программы";
      - "Прерванный ошибкой сеанс программы";
      - "Успешный сеанс программы".
    - ProgNM — имя рецепта-программы;
    - StartTm — время запуска рецепта-программы, в формате "2020-03-14 16:05:01";
    - ActTm — время действия рецепта-программы, в формате "2020-03-14 16:05:52".

Автор: Роман Савоченко <roman@oscada.org>
Версия: 2.2.0
Лицензия: GPLv2',10,0,'JavaLikeCalc.JavaScript
clcCnt++;

if(f_start) {
	dbDB_ = ""; dbO = false;
	work = SYS.XMLNode("prg");
}

//Checking for the commands controller presence
comCntrO = false;
if(comsCntr.length)	comCntrO = (comsCntr=="*") ? this.nodePrev() : SYS.DAQ.nodeAt(comsCntr,".");

t_err = "0";

//Check for the transport change and connect
if(!dbO || dbDB != dbDB_) {
	dbDB_ = dbDB;
	dbO = SYS.BD.nodeAt(dbDB_, ".");
	if(dbO) {
		dbO.SQLReq("CREATE TABLE IF NOT EXISTS PrescrProgs (name TEXT, prgTxt TEXT, PRIMARY KEY (name));");
		if(!comCntrO)
			dbO.SQLReq("CREATE TABLE IF NOT EXISTS PrescrComs (name TEXT, descr TEXT, proc TEXT, arg1 TEXT, arg2 TEXT, arg3 TEXT, arg4 TEXT, arg5 TEXT, PRIMARY KEY (name));");
	}
}
if(!dbO)	t_err = "1:"+tr("Error the DB ''%1''.").replace("%1", dbDB);
else {

//Commands list update
if(f_start || !(clcCnt%(60*f_frq))) {
	if(!comCntrO) {
		rez = dbO.SQLReq("SELECT * FROM "+dbComs+";");
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
			if(!comO.nodeAt("start") || !comO.nodeAt("stop") || !comO.nodeAt("run"))	continue;
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
	rez = dbO.SQLReq("SELECT prgTxt FROM "+dbProgs+" WHERE name=''"+prog+"'';");
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
		for(iC = 0; iC < cL.length; iC++) {
			var cLi = comCntrO[cL[iC]];
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
		for(iC = 0; iC < cL.length; iC++)
			comCntrO[cL[iC]].pause.set(mode == 2);
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
		//????
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
				SYS.messInfo("uprg"+prog,tr("Terminated program session by the user")+" \""+prog+"\" : "+SYS.strftime(startTm)+" : "+SYS.strftime(SYS.time()));
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
					SYS.messInfo("uprg"+prog,tr("Terminated program session by the error")+" \""+prog+"\" : "+SYS.strftime(startTm)+" : "+SYS.strftime(SYS.time()));
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
			rez = dbO.SQLReq("SELECT prgTxt FROM "+dbProgs+" WHERE name=''"+prog+"'';");
			if(rez.length > 1) {
				//Parse programm and insert procedure text and argument''s labels to here
				comTree = SYS.XMLNode("prg");
				comTree.load(rez[1][0]);
				comTree.setAttr("wtm",SYS.time()-startTm);
				dbO.SQLReq("UPDATE "+dbProgs+" SET prgTxt=''"+comTree.save().replace("''","''''")+"'' WHERE name=''"+prog+"'';");
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
				for(iC = 0; iC < cL.length; iC++) {
					var cLi = comCntrO[cL[iC]];
					cLi.run.set(false);
					cLi.start.set(false);
					cLi.abort.set(true);
				}
				SYS.messInfo("uprg"+prog, tr("Terminated program session by the user")+" \""+prog+"\" : "+SYS.strftime(startTm)+" : "+SYS.strftime(SYS.time()));
				curMode = mode = -3;
			}
			//Commands process
			else {
				// Start command
				if(!comEl.attr("tm").length) {
					curComPrm.run.set(false);	//Stop for possible background call
					comEl.setAttr("tm", SYS.time());
					for(i_a = 1; i_a <= 5; i_a++)
						if(!(comA=curComPrm["arg"+i_a]).isEVal())
							comA.set(comEl.attr("arg"+i_a));
					curComPrm.rez.set(0);
					curComPrm.run.set(true);
					if(mode == 2) curComPrm.pause.set(true);
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
					for(iC = 0; iC < cL.length; iC++) {
						var cLi = comCntrO[cL[iC]];
						cLi.run.set(false);
						cLi.start.set(false);
						cLi.error.set(true);
					}
					SYS.messInfo("uprg"+prog,tr("Terminated program session by the error")+" \""+prog+"\" : "+SYS.strftime(startTm)+" : "+SYS.strftime(SYS.time()));
					curMode = mode = -1;
				}
				if(!actBackGrnd && isBackgrnd && rez.toInt() == 0) actBackGrnd = true;
				if(isCurCmd && (rez.toInt() > 0 || isBackgrnd) && !toStop)	toNext = true;
				comElI.setAttr("rez", rez);
				for(i_a = 1; i_a <= 5; i_a++)
					if(!(comA=curComPI["arg"+i_a]).isEVal())
						comElI.setAttr("arg"+i_a, comA.get());
			}
			if(isCurCmd) {
				if(curComPI.pause.get() == true)	mode = 2;
				continue;
			}
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
			for(iC = 0, curComN = ""; iC < (curComLev-1); iC++) curComN += curCom.parse(iC,":");
			curCom = curComN.length ? curComN+":"+curComPos : curComPos;
		}
		
		//End call
		if(curComLev <= 0) {
			// Stop all typical and call "stop" command
			var cL = comCntrO.nodeList("prm_");
			for(iC = 0; iC < cL.length; iC++) {
				var cLi = comCntrO[cL[iC]];
				cLi.run.set(false);
				cLi.start.set(false);
				cLi.stop.set(true);
			}

			curMode = mode = -2;
			SYS.messInfo("uprg"+prog,tr("Successful session of the program")+" \""+prog+"\" : "+SYS.strftime(startTm)+" : "+SYS.strftime(SYS.time()));

			//Place to program last execution time
			rez = dbO.SQLReq("SELECT prgTxt FROM "+dbProgs+" WHERE name=''"+prog+"'';");
			if(rez.length > 1) {
				//Parse program and insert procedure text and argument''s labels to here
				comTree = SYS.XMLNode("prg");
				comTree.load(rez[1][0]);
				comTree.setAttr("wtm",SYS.time()-startTm);
				dbO.SQLReq("UPDATE "+dbProgs+" SET prgTxt=''"+comTree.save().replace("''","''''")+"'' WHERE name=''"+prog+"'';");
			}
		}
	}
}
curMode = mode;
//mode = curMode;

}

f_err = t_err;','','',1616351798);
CREATE TABLE IF NOT EXISTS 'tmplib_base' ("ID" TEXT DEFAULT '' ,"NAME" TEXT DEFAULT '' ,"uk#NAME" TEXT DEFAULT '' ,"ru#NAME" TEXT DEFAULT '' ,"DESCR" TEXT DEFAULT '' ,"uk#DESCR" TEXT DEFAULT '' ,"ru#DESCR" TEXT DEFAULT '' ,"MAXCALCTM" INTEGER DEFAULT '10' ,"PR_TR" INTEGER DEFAULT '1' ,"PROGRAM" TEXT DEFAULT '' ,"uk#PROGRAM" TEXT DEFAULT '' ,"ru#PROGRAM" TEXT DEFAULT '' ,"TIMESTAMP" INTEGER DEFAULT '' , PRIMARY KEY ("ID"));
INSERT INTO tmplib_base VALUES('digAlarm','Alarm discrete','Сигналізація дискретна','Сигнализация дискретная','Common, representative and unified template of separate discrete parameter (complex tag) processing with formation of signaling by the defined criterion. The template forms a structure of discrete parameter (complex tag) which can be easily connected to most widgets and cadres of the main elements library of the user interface just pointing the parameter object.

The template provides some fields of the representative structure of the discrete parameters and adds the error field:
- "State "Opened" (st_open)" — the state "Opened".
- "State "Closed" (st_close)" — the state "Closed".
- "Error (err)" — obligatory attribute of any parameter, consist of the combination of the error code and short text description, like to "2:No data or connection with source". The template reserves next error codes:
  - 1 — hardware errors, when the data mostly wrong;
  - 3 — violation of the specified condition.

Functions:
- Direct obtaining, by link, of the initial state and formation of two representative states with the possibility of their inversion stInv.
- Complex preprocessing and complete generation-imitation of the discrete input on the internal programming language of OpenSCADA into the configuration IO inProc, where you can operate next service parameters:
  - in [IN|OUT] — the same raw discrete input IO;
  - levErr, tErr [IN|OUT] — alarm level [-7...0] and text of the error (the attribute "err" format);
  - f_frq [IN] — frequency of calculation, the template IO;
  - this [IN] — reference to the parameter object, what executing the template;
  - ctx [IN] — object of the user context, what saved between the execution cycles.
- Processing of the discrete input in for the purpose of creating a violation and correspondingly setting the attribute err, at the condition "{st}:{lev}:{mess}", that is generation violation of the level lev and the text mess for the input status st (0|1).

Author: Roman Savochenko <roman@oscada.org>
Version: 1.1.0
License: GPLv2','Загальний, представницький шаблон обробки окремого дискретного параметру (складного тегу) із формування сигналізації за визначеним критерієм. Шаблон формує структуру дискретного параметру (складного тегу) який може бути легко підключений до більшості віджетів та кадрів бібліотеки основних елементів інтерфейсу користувача просто вказавши об''єкт параметру.

Шаблон надає окремі поля представницької структури дискретного параметру та додає поле помилки:
- "Стан "Відкрито" (st_open)" — стан "Відкрито".
- "Стан "Закрито" (st_close)" — стан "Відкрито".
- "Помилка (err)" — обов''язковий атрибут будь якого параметру, що містить комбінацію коду помилки та короткого опису, на кшталт "1:Немає даних або підключення до джерела". Шаблон резервує наступні коди помилок:
  - 1 — помилки обладнання, коли дані переважно помилкові;
  - 3 — порушення за визначеної умови.

Функції
- Пряме отримання, за посиланням, первинного стану та формування двох репрезентативних станів з можливістю їх інверсії stInv.
- Складна передобробка та повна генерація-імітація дискретного входу на внутрішній мові програмування OpenSCADA у конфігураційному ВВ inProc, де Ви можете оперувати наступними сервісними параметрами:
  - in [IN|OUT] — той самий сирий дискретний ВВ;
  - levErr, tErr [IN|OUT] — рівень аварії [-7...0] та текст помилки (формат атрибуту "err");
  - f_frq [IN] — частота обчислення, шаблонний ВВ;
  - this [IN] — посилання на об''єкт параметру, що виконує шаблон;
  - ctx [IN] — об''єкт користувацького контексту, що зберігається між циклами виклику.
- Опрацювання дискретного входу in на предмет формування порушення та відповідного встановлення атрибуту err, за умови "{st}:{lev}:{mess}", тобто генерація порушення з рівнем lev та текстом mess за стану входу st (0|1).

Автор: Роман Савоченко <roman@oscada.org>
Версія: 1.1.0
Ліцензія: GPLv2','',10,0,'JavaLikeCalc.JavaScript
if(f_start) {
	f_err = "0";
	alDelay_ = 0;
	//Prepare data for preprocessing
	inPrcId = this.nodePath("_");
	inPrcLng = "JavaLikeCalc.JavaScript";
	inPrcArgs = new Object();
	inPrcArgs.this = this;
	inPrcArgs.ctx = new Object();
	return;
}
if(f_stop) return;

tErr = "0", levErr = 0;

//Call a specific preprocessing procedure
if(inProc.length)	{
	inPrcArgs.f_frq = f_frq;
	inPrcArgs.in = in;
	inPrcArgs.levErr = levErr; inPrcArgs.tErr = tErr;
	SYS.DAQ.funcCall(inPrcLng, inPrcArgs, inProc, inPrcId);
	levErr = inPrcArgs.levErr; tErr = inPrcArgs.tErr;
	in = inPrcArgs.in;
}

//State set
if(in.isEVal()) {
	tErr = "1:"+tr("No data or connection with source");
	levErr = -5;
	st_open = st_close = in;
}
else {
	if(alrm.length && in == alrm.toInt())
		tErr = "3:"+alrm.parse(2,":"), levErr = -alrm.parse(1,":").toInt();
	st_open = stInv ? !in : in;
	st_close = !st_open;
}

//Alarms forming
if(alSup) {
	if(f_err.toInt())	this.alarmSet(DESCR+": "+tr("SUPPRESSED"), 1);
	f_err = "0";
}
else {
	if(tErr.toInt() && tErr.toInt() != f_err.toInt() && this.cntr().status().toInt())	return;
	if(alDelay > 0 && alDelay_ <= 0 && tErr.toInt() != f_err.toInt())	tErr1 = tErr.toInt();
	if(alDelay > 0 && alDelay_ > 0 && tErr.toInt() != tErr1)	{ alDelay_ = 0; tErr1 = tErr.toInt(); }
	if(alDelay > 0 && alDelay_ < alDelay){ alDelay_ += 1/f_frq; return; }
	if(tErr.toInt())	this.alarmSet(DESCR+": "+tErr.parse(1,":"), levErr);
	else	this.alarmSet(DESCR+": "+tr("NORM"), 1);
	f_err = tErr;
	alDelay_ = 0;
}','','',1572284562);
INSERT INTO tmplib_base VALUES('simleBoard','Analog alarm by borders (obsolete)','Сигнал аналоговий за границями (застаріле)','Сигнал аналоговый по границам (устаревшее)','The template of simple parameter included borders and dimension variable.

Author: Roman Savochenko <roman@oscada.org>
Version: 1.0.1','Шаблон простого параметру з перевіркою границь та одиницею виміру.

Автор: Роман Савоченко <roman@oscada.org>
Версія: 1.0.1','Шаблон простого параметра с проверкой границ и единицей измерения.

Автор: Роман Савоченко <roman@oscada.org>
Версия: 1.0.1',10,1,'JavaLikeCalc.JavaScript
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
else f_err="0";',1627833238);
INSERT INTO tmplib_base VALUES('digitBlock','Discrete parameters block (obsolete)','Блок дискр. параметрів (застаріле)','Блок дискр. параметров (устарелое)','The block for union of Discrete parameters for one device control.

Author: Roman Savochenko <roman@oscada.org>
Version: 1.0.0','Блок для збору дискретних параметрів, керуючих одним апаратом.

Автор: Роман Савоченко <roman@oscada.org>
Версія: 1.0.0','Блок для сборки дискретных параметров управляющих одним аппаратом.

Автор: Роман Савоченко <roman@oscada.org>
Версия: 1.0.0',10,0,'JavaLikeCalc.JavaScript
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
INSERT INTO tmplib_base VALUES('manInUnif','Manual input, unified','Ручний ввід, уніфікований','Ручной ввод, унифицированный','Common, representative and unified template of a manual input and an analog output signal. The template forms a structure of parameter (complex tag) of the manual input what can be easily connected to most widgets and cadres of the main elements library of the user interface just pointing the parameter object, the control mostly can be performed through the Element cadre control panel.

The representative structure of the parameter (complex tag) of the manual input is:
- "Variable (var)" — end variable of the parameter — engineeric real value, allowed and suitable for archiving.
- "Variable input (varIn)" — attribute for entering a new engineering value, which in the next falls into the variable.
- "Error (err)" — obligatory attribute of any parameter, consist of the combination of the error code and short text description, like to "2:No data or connection with source". The template reserves next error codes:
  - 1, 2 — hardware errors, when the data mostly wrong;
  - 3, 4 — alarm border errors;
  - 5, 6 — warning border errors;
  - 7 — the speed error of the variable changing.
- "Violations suppress (alSup) and delay (alDelay)" — violations generic control for suppressing and delaying.
- "Scale: minimum (min)" and "Scale: maximum (max)" — main scale of the end variable, the output of which causes a hardware border error. The scale is also used as a scale of the variable trend display on the graphs.
- "Logarithmic scale (log)" — the scale sign of the variable trend display on the graphs in the logarithmic scale.
- "Dimension (ed)" — dimension (unit of measurement) of the variable.
- "Precision (prec)" — number signs after the dot, for positive values, and precision (signs in whole), for negative ones, to display the real end variable on the screen.

Functions:
- Generic processing of the raw analog input for:
  - simple correction-calibration at the formula "in = iMult*(in+iAdd)";
  - scaling from the hardware border [plcMin...plcMax] to the main border [min...max], applying the square scale scSqr;
￼ !!!! The hardware borders can be inverted ones and disabled in whole for equal values of the maximum and minimum, when the main border only used.
  - permission to exceed the border for the input value on the value of plcExcess percent;
  - filtering the input value changing in the time Tf.
- Processing the result variable var for violation some borders and criteria, generating the alarms and proper setting the attribute err. The alarm borders can have some hysteresis HystBnd, can be delayed on the time alDelay and can be suppressed setting alSup. Next borders and criteria are provided:
  - missing for data or connection with source at the EVAL input value — error code 1 and alarm level 5;
  - exceeding the input value of the hardware border — error code 1,2 and alarm level 5;
  - exceeding the end variable of the alarm border [aMin...aMax] — error code 3,4 and alarm level 4;
￼ !!!! Disabled at setting for equal or inverted values of the border
  - exceeding the end variable of the warning border [wMin...wMax] — error code 5,6 and alarm level 2;
￼ !!!! Disabled at setting for equal or inverted values of the border
  - big motion speed of the end variable — error code 7 and alarm level 2.
- Reverse conversion of the engineered variable value and writing to the output.
￼ !!!! In the absence of the "Input/Output" connection, the writing is not performed and there is the stand-alone operation.

Author: Roman Savochenko <roman@oscada.org>
Version: 1.2.1
License: GPLv2','Загальний, представницький та уніфікований шаблон ручного вводу та сигналу аналогового виходу. Шаблон формує структуру параметру (складного тегу) ручного вводу, який може бути легко підключений до більшості віджетів та кадрів бібліотеки основних елементів інтерфейсу користувача просто вказавши об''єкт параметру, контроль переважно можна здійснювати через панель контролю Елемент кадр.

Представницькою структурою параметру (складного тегу) ручного вводу є:
- "Змінна (var)" — кінцева змінна параметру — інженерне реальне значення, доступне та придатне до архівації.
- "Введення змінної (varIn)" — атрибут введення нового інженерного значення, який надалі потрапляє до змінної.
- "Помилка (err)" — обов''язковий атрибут будь якого параметру, що містить комбінацію коду помилки та короткого опису, на кшталт "2:Немає даних або підключення до джерела". Шаблон резервує наступні коди помилок:
  - 1, 2 — помилки обладнання, коли дані переважно помилкові;
  - 3, 4 — помилки аварійної границі;
  - 5, 6 — помилки попереджувальної границі;
  - 7 — помилка швидкості зміни змінної.
- "Придушення (alSup) та затримка (alDelay) порушень" — загальний контроль порушень щодо придушення та затримки.
- "Шкала: мінімум (min)" та "Шкала: максимум (max)" — головна шкала кінцевої змінної, вихід за яку призводить до помилки апаратної границі. Шкала також використовується як шкала відображення тренду змінної на графіках.
- "Логарифмічна шкала (log)" — ознака відображення тренду змінної на графіках у логарифмічній шкалі.
- "Одиниця виміру (ed)" — одиниця виміру змінної.
- "Точність (prec)" — кількість знаків після крапки, для позитивних значень, та точність (знаків загалом), для негативних, для відображення реальної кінцевої змінної на екрані.

Функції:
- Загальне опрацювання аналогового входу щодо:
  - проста корекція-калібрування за формулою "in = iMult*(in+iAdd)";
  - масштабування від апаратної границі [plcMin...plcMax] у головну границю [min...max], враховуючи квадратичну шкалу scSqr;
￼ !!!! Апаратні границі може бути інвертовано та взагалі вимкнено однаковими значеннями максимуму та мінімуму, коли використовується тільки головна шкала.
  - дозвіл на перевищення границі вхідним значенням на величину plcExcess відсотків;
  - фільтрування зміни вхідного значення на час Tf.
- Опрацювання кінцевої змінної var на предмет порушення деяких границь та критеріїв, генеруючи аварії та відповідно встановлюючи атрибут err. Границі аварії можуть мати деякий гістерезис HystBnd, можуть бути затримані на час alDelay та можуть бути придушені встановленням alSup. Надаються наступні границі та критерії:
  - відсутність даних або підключення до джерела за вхідним значенням EVAL — код помилки 1 та рівень аварії 5;
  - перевищення апаратної границі вхідним значенням — код помилки 1,2 та рівень аварії 5;
  - перевищення аварійної границі кінцевою змінною [aMin...aMax] — код помилки 3,4 та рівень аварії 4;
￼ !!!! Вимикається встановленням однакових або інверсних значень границі
  - перевищення попереджувальної границі кінцевою змінною [wMin...wMax] — код помилки 5,6 та рівень аварії 2;
￼ !!!! Вимикається встановленням однакових або інверсних значень границі
  - велика швидкість зміни speed кінцевої змінної — код помилки 7 та рівень аварії 2.
- Зворотне перетворення введеного інженерного значення змінної та запис у вихід.
￼ !!!! За відсутності підключення "Входу/Виходу" запису не здійснюється та відбувається автономна робота.

Автор: Роман Савоченко <roman@oscada.org>
Версія: 1.2.1
Ліцензія: GPLv2','',10,0,'JavaLikeCalc.JavaScript
if(f_start)	{ f_err = "0"; alDelay_ = 0; }

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
if(alSup) {
	if(f_err.toInt())	this.alarmSet(DESCR+": "+tr("SUPPRESSED"), 1);
	f_err = "0";
}
else {
	if(tErr.toInt() && tErr.toInt() != f_err.toInt() && this.cntr().status().toInt())	return;
	if(alDelay > 0 && alDelay_ <= 0 && tErr.toInt() != f_err.toInt())	tErr1 = tErr.toInt();
	if(alDelay > 0 && alDelay_ > 0 && tErr.toInt() != tErr1)	{ alDelay_ = 0; tErr1 = tErr.toInt(); }
	if(alDelay > 0 && alDelay_ < alDelay){ alDelay_ += 1/f_frq; return; }
	if(tErr.toInt())	this.alarmSet(DESCR+": "+tErr.parse(1,":"), levErr);
	else	this.alarmSet(DESCR+": "+tr("NORM"), 1);
	f_err = tErr;
	alDelay_ = 0;
}','','',1627832983);
INSERT INTO tmplib_base VALUES('anUnif','Analog signal, unified','Аналоговий сигнал, уніфікований','Аналоговый сигнал, унифицированный','Common, representative and unified template of analog input signals processing. The template forms a structure of complex analog parameter (tag) which can be easily connected to most widgets and cadres of the main elements library of the user interface just pointing the parameter object.

Functions:
- Generic processing of the raw analog input for:
  - simple correction-calibration at the formula "in = iMult*(in+iAdd)";
  - scaling from the hardware border [plcMin...plcMax] to the main border [min...max], applying the square scale scSqr;
     !!! The hardware borders can be inverted ones and disabled in whole for equal values of the maximum and minimum, when the main border only used.
  - permission to exceed the border for the input value on the value of plcExcess percent;
  - filtering the input value changing in the time Tf.
- Simple imitation of the analog input, setting plcImit and linking the imitation-simulation source in plcImitIn.
     !!! Missing the link plcImitIn (EVAL) causes to generating random analog input
- Complex preprocessing and complete generation-imitation of the analog input on the internal programming language of OpenSCADA into the configuration IO inProc, where you can operate next service parameters:
  - f_frq [IN] — frequency of calculation, the template IO;
  - in [IN|OUT] — the same raw analog input IO;
  - var [IN] — the end variable IO;
  - min, max [IN] — the main border IOs;
  - plcMin, plcMax [IN] — the hardware border IOs;
  - plcImit, plcImitIn [IN] — IOs of the simple imitation;
  - levErr, tErr [IN|OUT] — alarm level [-7...0] and text of the error (the attribute "err" format).
- Simple substitution for the variable subVar at the error (not valid) input variable (EVAL) in the modes: "no", "last" and "substitute".
- Processing the result variable var for violation some borders and criteria, generating the alarms and proper setting the attribute err. The alarm borders can have some hysteresis HystBnd, can be delayed for the time alDelay and can be worked in the modes alSup: "init NORM", "suppress", "save on change". Next borders and criteria are provided:
  - missing for data or connection with source at the EVAL input value — error code 1 and alarm level 5;
  - exceeding the input value of the hardware border — error code 1,2 and alarm level 5;
  - exceeding the end variable of the alarm border [aMin...aMax] — error code 3,4 and alarm level 4;
     !!! Disabled at setting for equal or inverted values of the border
  - exceeding the end variable of the warning border [wMin...wMax] — error code 5,6 and alarm level 2;
     !!! Disabled at setting for equal or inverted values of the border
  - big motion speed of the end variable — error code 7 and alarm level 2.

Author: Roman Savochenko <roman@oscada.org>
Version: 2.1.3
License: GPLv2','Загальний, представницький та уніфікований шаблон обробки аналогових вхідних сигналів. Шаблон формує структуру складного аналогового параметру (тегу) який може бути легко підключений до більшості віджетів та кадрів бібліотеки основних елементів інтерфейсу користувача просто вказавши об''єкт параметру.

Функції:
- Загальне опрацювання аналогового входу щодо:
  - проста корекція-калібрування за формулою "in = iMult*(in+iAdd)";
  - масштабування від апаратної границі [plcMin...plcMax] у головну границю [min...max], враховуючи квадратичну шкалу scSqr;
     !!! Апаратні границі може бути інвертовано та взагалі вимкнено однаковими значеннями максимуму та мінімуму, коли використовується тільки головна шкала.
  - дозвіл на перевищення границі вхідним значенням на величину plcExcess відсотків;
  - фільтрування зміни вхідного значення на час Tf.
- Проста імітація аналогового входу, встановленням plcImit та підключенням джерела імітації-симуляції до plcImitIn.
     !!! Відсутність зв''язка plcImitIn (EVAL) призводить до генерації випадкового аналогового входу
- Складна передобробка та повна генерація-імітація аналогового входу на внутрішній мові програмування OpenSCADA у конфігураційному ВВ inProc, де Ви можете оперувати наступними сервісними параметрами:
  - f_frq [IN] — частота обчислення, шаблонний ВВ;
  - in [IN|OUT] — той самий сирий аналоговий ВВ;
  - var [IN] — ВВ кінцевої змінної;
  - min, max [IN] — ВВ головної шкали;
  - plcMin, plcMax [IN] — ВВ апаратної шкали;
  - plcImit, plcImitIn [IN] — ВВ простої імітації;
  - levErr, tErr [IN|OUT] — рівень аварії [-7...0] та текст помилки (формат атрибуту "err").
- Проста підстановка значення subVar за помилкою (недостовірністю) вхідної змінної (EVAL) у режимах: "немає", "останнє" та "підстановка".
- Опрацювання кінцевої змінної var на предмет порушення деяких границь та критеріїв, генеруючи аварії та відповідно встановлюючи атрибут err. Границі аварії можуть мати деякий гістерезис HystBnd, можуть бути затримані на час alDelay та можуть працювати у режимах alSup: "ініціювати НОРМА", "придушувати", "зберігати при зміні". Надаються наступні границі та критерії:
  - відсутність даних або підключення до джерела за вхідним значенням EVAL — код помилки 1 та рівень аварії 5;
  - перевищення апаратної границі вхідним значенням — код помилки 1,2 та рівень аварії 5;
  - перевищення аварійної границі кінцевою змінною [aMin...aMax] — код помилки 3,4 та рівень аварії 4;
     !!! Вимикається встановленням однакових або інверсних значень границі
  - перевищення попереджувальної границі кінцевою змінною [wMin...wMax] — код помилки 5,6 та рівень аварії 2;
     !!! Вимикається встановленням однакових або інверсних значень границі
  - велика швидкість зміни speed кінцевої змінної — код помилки 7 та рівень аварії 2.

Автор: Роман Савоченко <roman@oscada.org>
Версія: 2.1.3
Ліцензія: GPLv2','',10,0,'JavaLikeCalc.JavaScript
if(f_start) {
	prevVar = EVAL_REAL;
	alDelay_ = 0; firstNorm = alNormForceStart;
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
		var plcOff;
		plcDif = abs(pMax-pMin);
		if(plcOff.isEVal())		plcOff = rand(plcDif/2);
		in = pMin + plcDif/4 + plcOff + rand(plcDif/10)-plcDif/20;
  }
}

levErr = 0;
tErr = "0";

//Call specific preprocessing procedure
if(inProc.length)	{
	inPrcArgs.f_frq = f_frq;
	inPrcArgs.in = in; inPrcArgs.var = var; inPrcArgs.min = min; inPrcArgs.max = max;
	inPrcArgs.plcMin = pMin; inPrcArgs.plcMax = pMax;
	inPrcArgs.plcImit = plcImit; inPrcArgs.plcImitIn = plcImitIn;
	inPrcArgs.levErr = levErr; inPrcArgs.tErr = tErr;
	SYS.DAQ.funcCall(inPrcLng, inPrcArgs, inProc, inPrcId);
	in = inPrcArgs.in;
	levErr = inPrcArgs.levErr; tErr = inPrcArgs.tErr;
}

//Input data check and postprocess
if(in.isEVal()) {
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

	if(!tErr) {
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
}

//Alarms forming
if(alSup) {
	if(f_err.toInt())	this.alarmSet(DESCR+": "+tr("SUPPRESSED"), 1);
	f_err = "0";
}
else {
	if(!firstNorm && tErr.toInt() && tErr.toInt() != f_err.toInt() && this.cntr().status().toInt())	return;
	if(alDelay > 0 && alDelay_ <= 0 && tErr.toInt() != f_err.toInt())	tErr1 = tErr.toInt();
	if(alDelay > 0 && alDelay_ > 0 && tErr.toInt() != tErr1)	{ alDelay_ = 0; tErr1 = tErr.toInt(); }
	if(alDelay > 0 && alDelay_ < alDelay){ alDelay_ += 1/f_frq; return; }
	if(tErr.toInt())	this.alarmSet(DESCR+": "+tErr.parse(1,":"), levErr);
	else	this.alarmSet(DESCR+": "+tr("NORM"), 1, firstNorm);
	f_err = tErr;
	alDelay_ = 0; firstNorm = false;
}','','',1627832581);
INSERT INTO tmplib_base VALUES('digitBlockUnif','Discrete block, unified','Блок дискретних, уніфікований','Блок дискретных, унифицированный','Common, representative and unified template of the block for union of Discrete parameters for the common control device. The template forms a structure of discrete parameter-block (complex tag) which can be easily connected to most widgets and cadres of the main elements library of the user interface just pointing the parameter object.

The representative structure of discrete parameters (complex tags) is a latch object with two characteristic states and three commands, which in the final representation may have a different meaning and name:
- "State "Opened" (st_open)" — the state "Opened" of the latch. Is the primary status signal that can be used without the next one, when 1 (true) is "Opened", and 0 (false) is ''Closed''.
- "State "Closed" (st_close)" — the state "Closed".
- "Command "Open" (com)" — the command "Open" of the latch. Is the primary command signal that can be used without the next one, when 1 (true) is "Open", and 0 (false) is ''Close''.
- "Command "Close" (close)" — the command "Close" of the latch.
- "Command "Stop" (stop)" — the command "Stop" of the latch.

Functions:
- Direct obtaining, by link, of the two states (read only) and three commands (reading and writing).
- Possibility of holding the commands for a certain time and removing them.
- Ability to redefine the states and commands by the appropriate settings of the interface elements, such as digStts, for statuses, and digComs, for commands.

Author: Roman Savochenko <roman@oscada.org>
Version: 1.0.0
License: GPLv2','Загальний, представницький та уніфікований шаблон блоку поєднання дискретних параметрів у пристрій загального контролю. Шаблон формує структуру дискретного параметру-блоку (складного тегу) який може бути легко підключений до більшості віджетів та кадрів бібліотеки основних елементів інтерфейсу користувача просто вказавши об''єкт параметру.

Представницькою структурою дискретного параметру (складного тегу) є об''єкт засувки із характерними двома станами та трьома командами, які у кінцевому представленні можуть мати інше значення та назву:
- "Стан "Відкрито" (st_open)" — стан засувки "Відкрито". Є первинним сигналом стану, який може використовуватися без наступного, коли 1 (true) — "Відкрито", а 0 (false) — "Закрито".
- "Стан "Закрито" (st_close)" — стан засувки "Відкрито".
- "Команда "Відкрити" (com)" — команда засувки "Відкрити". Є первинним сигналом команди, який може використовуватися без наступного, коли 1 (true) — "Відкрити", а 0 (false) — "Закрити".
- "Команда "Закрити" (close)" — команда засувки "Закрити".
- "Команда "Зупинити" (stop)" — команда засувки "Зупинити".

Функції:
- Пряме отримання, за посиланням, двох станів (тільки читання) та трьох команд (читання та запис).
- Можливість утримання команди протягом визначеного часу та зняття їх.
- Можливість перевизначення станів та команд відповідним налаштуванням елементів інтерфейсу, як то digStts, для статусів, та digComs, для команд.

Автор: Роман Савоченко <roman@oscada.org>
Версія: 1.0.0
Ліцензія: GPLv2','',10,0,'JavaLikeCalc.JavaScript
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
INSERT INTO tmplib_base VALUES('pidUnifImp','Impulse PID signal, unified, violation statuses','Імпульсний ПІД сигнал, уніфікований, стани порушень','ПИД импульсный сигнал, унифицированный, состояния нарушений','Common, representative and unified template of an interface to the Proportional Integral Differential (PID) regulator based on the common analog input signal but with the PWM-output, extended by binary states of indication the violations and mostly like to the template "PID signal, unified, violation statuses". The template forms a structure of the impulse PID-regulator which can be easily connected to some cadres of the main elements library of the user interface just pointing the parameter object.

The representative structure of the impulse PID-regulators is mostly equal to the template "PID signal, unified, violation statuses" and contains a number of attributes specific to the pulse output and the attribute of determining the operation of this regulator in the analog mode, that is, this template is even more generalizable:
- "Impulse output up (impQup)" and "Impulse output down (impQdwn)" — signals of the PWM-output.
- "Mode: analog (analog)" — a sign of working the regulator in the analog output mode.

Functions:
Mostly the same as the basic template "PID signal, unified, violation statuses" and the impulse-specific attributes are additionally provided: "Impulse output up", "Impulse output down", "Mode: analog", "T impulses period", "T minimal impulse", "Impulse rate factor".

Author: Roman Savochenko <roman@oscada.org>
Sponsored by: DIYA LTD
Version: 1.1.1
License: GPLv2','Загальний, представницький та уніфікований шаблон інтерфейсу до Пропорційного Інтегрального Диференційного (ПІД) регулятору заснованому на аналоговому вхідному сигналі але із ШІМ-виходом, розширеному бінарними станами індикації порушень та переважно схоже на шаблон "ПІД сигнал, уніфікований, стани порушень". Шаблон формує структуру імпульсного ПІД-регулятору, який може бути легко підключений до деяких кадрів та віджетів бібліотеки основних елементів інтерфейсу користувача, просто вказавши об''єкт параметру.

Представницька структура імпульсних ПІД-регуляторів переважно еквівалентна шаблону "ПІД сигнал, уніфікований, стани порушень" та містить низку атрибутів, специфічних до імпульсного виходу та атрибут визначення роботи цього регулятору у аналоговому режимі, тобто цей шаблон є навіть більш узагальнювальним:
- "Імпульсний вихід у гору (impQup)" та "Імпульсний вихід до низу (impQdwn)" — сигнали ШІМ-виходу.
- "Режим: аналоговий (analog)" — ознака роботи регулятору у режимі аналогового виходу.

Функції:
Переважно те саме що у базового шаблону "ПІД сигнал, уніфікований, стани порушень" та додатково надаються імпульсно-специфічні атрибути: "Імпульсний вихід у гору", "Імпульсний вихід до низу", "Режим: аналоговий", "T період імпульсів", "Мінімальна довжина імпульсу", "Рейтинг фактор імпульсів".

Автор: Роман Савоченко <roman@oscada.org>
Спонсоровано: ТОВ "ДІЯ"
Версія: 1.1.1
Ліцензія: GPLv2','',10,0,'JavaLikeCalc.JavaScript
if(f_start) {
	f_err = "0";
	alDelay_ = 0;
}

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
if(tErr)	EVAL = true;
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

//Alarms forming
if(alSup) {
	if(f_err.toInt())	this.alarmSet(DESCR+": "+tr("SUPPRESSED"), 1);
	f_err = "0";
}
else {
	if(tErr.toInt() && tErr.toInt() != f_err.toInt() && this.cntr().status().toInt())	return;
	if(alDelay > 0 && alDelay_ <= 0 && tErr.toInt() != f_err.toInt())	tErr1 = tErr.toInt();
	if(alDelay > 0 && alDelay_ > 0 && tErr.toInt() != tErr1)	{ alDelay_ = 0; tErr1 = tErr.toInt(); }
	if(alDelay > 0 && alDelay_ < alDelay){ alDelay_ += 1/f_frq; return; }
	if(tErr.toInt())	this.alarmSet(DESCR+": "+tErr.parse(1,":"), -4);
	else	this.alarmSet(DESCR+": "+tr("NORM"), 1);
	f_err = tErr;
	alDelay_ = 0;
}','','',1627833248);
INSERT INTO tmplib_base VALUES('anUnifSt','Analog signal, unified, violation statuses','Аналоговий сигнал, уніфікований, стани порушень','Аналоговый сигнал, унифицированный, состояния нарушений','Common, representative and unified template of analog input signals processing, extended by binary states of indication the violations. The template grounded on the base template "Analog signal, unified" with its representative structure and some differences characteristic to independent development and expansion.

Binary states were provided for the convenient use of algorithms of operation at the borders of violations of the parameter object, preferably in the block diagrams of general control algorithms.

Functions:
Mostly the same as the basic "Analog signal, unified" and the borderline signs are additionally provided: "H", "L", "HH", "LL", "EVAL".

Author: Roman Savochenko <roman@oscada.org>
Version: 1.2.1
License: GPLv2','Загальний, представницький та уніфікований шаблон обробки аналогових вхідних сигналів, розширений бінарними станами індикації порушень. Шаблон засновано на базовому шаблоні "Аналоговий сигнал, уніфікований" з відповідною до нього представницькою структурою та деякими відмінностями, характерними до незалежного розвитку та розширення.

Бінарні стани передбачалося для зручного використання у алгоритмах роботи за границями порушення об''єкту параметру, переважно у блокових схемах загальних алгоритмів керування.

Функції:
Переважно відповідають базовому шаблону "Аналоговий сигнал, уніфікований" та додатково надаються ознаки порушень границь: "H", "L", "HH", "LL", "EVAL".

Автор: Роман Савоченко <roman@oscada.org>
Версія: 1.2.1
Ліцензія: GPLv2','',10,0,'JavaLikeCalc.JavaScript
if(f_start) {
	prevVar = EVAL_REAL;
	alDelay_ = 0;
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
		var plcOff;
		plcDif = abs(pMax-pMin);
		if(plcOff.isEVal())	plcOff = rand(plcDif/2);
		in = pMin + plcDif/4 + plcOff + rand(plcDif/10)-plcDif/20;
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
if(alSup) {
	if(f_err.toInt())	this.alarmSet(DESCR+": "+tr("SUPPRESSED"), 1);
	f_err = "0";
}
else {
	if(tErr.toInt() && tErr.toInt() != f_err.toInt() && this.cntr().status().toInt())	return;
	if(alDelay > 0 && alDelay_ <= 0 && tErr.toInt() != f_err.toInt())	tErr1 = tErr.toInt();
	if(alDelay > 0 && alDelay_ > 0 && tErr.toInt() != tErr1)	{ alDelay_ = 0; tErr1 = tErr.toInt(); }
	if(alDelay > 0 && alDelay_ < alDelay){ alDelay_ += 1/f_frq; return; }
	if(tErr.toInt())	this.alarmSet(DESCR+": "+tErr.parse(1,":"), levErr);
	else	this.alarmSet(DESCR+": "+tr("NORM"), 1);
	f_err = tErr;
	alDelay_ = 0;
}','','',1627832578);
INSERT INTO tmplib_base VALUES('pidUnif','PID signal, unified, violation statuses','ПІД сигнал, уніфікований, стани порушень','ПИД сигнал, унифицированный, состояния нарушений','Common, representative and unified template of an interface to the Proportional Integral Differential (PID) regulator based on the common analog input signal, extended by binary states of indication the violations. The template forms a structure of the analog PID-regulator which can be easily connected to some cadres and widgets of the main elements library of the user interface just pointing the parameter object.

The representative structure of the analog PID-regulators is:
- "Variable (var)" — end variable of the parameter — engineeric real value, allowed and suitable for archiving.
- "Error (err)" — obligatory attribute of any parameter, consist of the combination of the error code and short text description, like to "2:No data or connection with source". The template reserves next error codes:
  - 1, 2 — hardware errors, when the data mostly wrong;
  - 3, 4 — alarm border errors;
  - 5, 6 — warning border errors;
  - 7 — the speed error of the variable changing.
- "Violations suppress (alSup) and delay (alDelay)" — violations generic control for suppressing and delaying.
- "Setpoint (sp)" — setpoint for the analog signal value achievement in the automatic mode.
- "Manual input (manIn)" — attribute for entering a new output value, which in the next falls into the output at the manual mode.
- "Output (out)" — output (control signal) of the PID-regulator.
- "Mode: automate (auto)" — the PID-regulator mode "Automatic".
- "Mode: cascade (casc)" — the PID-regulator mode "Cascade".
- "Scale: minimum (min)" and "Scale: maximum (max)" — main scale of the end variable, the output of which causes a hardware border error. The scale is also used as a scale of the variable trend display on the graphs.
- "Logarithmic scale (log)" — the scale sign of the variable trend display on the graphs in the logarithmic scale.
- "Dimension (ed)" — dimension (unit of measurement) of the variable.
- "Precision (prec)" — number signs after the dot, for positive values, and precision (signs in whole), for negative ones, to display the real end variable on the screen.

Functions:
- Generic processing of the raw analog input for:
  - simple correction-calibration at the formula "in = iMult*(in+iAdd)";
  - scaling from the hardware border [plcMin...plcMax] to the main border [min...max], applying the square scale scSqr;
  - permission to exceed the border for the input value on the value of plcExcess percent;
  - filtering the input value changing in the time Tf.
- Processing the result variable var for violation some borders and criteria, generating the alarms and proper setting the attribute err. The alarm borders can have some hysteresis HystBnd and can be suppressed setting alSup. Next borders and criteria are provided:
- missing for data or connection with source at the EVAL input value — error code 1 and alarm level 5;
- exceeding the input value of the hardware border — error code 1,2 and alarm level 5;
- exceeding the end variable of the alarm border [aMin...aMax] — error code 3,4 and alarm level 4;
  !!! Disabled at setting for equal or inverted values of the border
- exceeding the end variable of the warning border [wMin...wMax] — error code 5,6 and alarm level 2;
  !!! Disabled at setting for equal or inverted values of the border
big motion speed of the end variable — error code 7 and alarm level 2.
- Transmission of the PID-specific information, control and coefficient attributes to and from the PLC or other data source with the regulation.

Author: Roman Savochenko <roman@oscada.org>
Sponsored by: DIYA LTD
Version: 1.2.1
License: GPLv2','Загальний, представницький та уніфікований шаблон інтерфейсу до Пропорційного Інтегрального Диференційного (ПІД) регулятору заснованому на аналоговому вхідному сигналі, розширеному бінарними станами індикації порушень. Шаблон формує структуру аналогового ПІД-регулятору, який може бути легко підключений до деяких кадрів та віджетів бібліотеки основних елементів інтерфейсу користувача, просто вказавши об''єкт параметру.

Представницькою структурою аналогового ПІД-регулятору є:
- "Змінна (var)" — кінцева змінна параметру — інженерне реальне значення, доступне та придатне до архівації.
- "Помилка (err)" — обов''язковий атрибут будь якого параметру, що містить комбінацію коду помилки та короткого опису, на кшталт "2:Немає даних або підключення до джерела". Шаблон резервує наступні коди помилок:
  - 1, 2 — помилки обладнання, коли дані переважно помилкові;
  - 3, 4 — помилки аварійної границі;
  - 5, 6 — помилки попереджувальної границі;
  - 7 — помилка швидкості зміни змінної.
- "Придушення (alSup) та затримка (alDelay) порушень" — загальний контроль порушень щодо придушення та затримки.
- "Завдання (sp)" — завдання для досягнення значенням аналоговим сигналом у автоматичному режимі.
- "Ручний ввід (manIn)" — атрибут введення нового значення виходу, яке надалі потрапляє до виходу у ручному режимі.
- "Вихід (out)" — вихід (сигнал контролю) ПІД-регулятору.
- "Режим: автомат (auto)" — режим "Автомат" ПІД-регулятору.
- "Режим: каскад (casc)" — режим "Каскад" ПІД-регулятору.
- "Шкала: мінімум (min)" та "Шкала: максимум (max)" — головна шкала кінцевої змінної, вихід за яку призводить до помилки апаратної границі. Шкала також використовується як шкала відображення тренду змінної на графіках.
- "Логарифмічна шкала (log)" — ознака відображення тренду змінної на графіках у логарифмічній шкалі.
- "Одиниця виміру (ed)" — одиниця виміру змінної.
- "Точність (prec)" — кількість знаків після крапки, для позитивних значень, та точність (знаків загалом), для негативних, для відображення реальної кінцевої змінної на екрані.

Функції:
- Загальне опрацювання аналогового входу щодо:
  - проста корекція-калібрування за формулою "in = iMult*(in+iAdd)";
  - масштабування від апаратної границі [plcMin...plcMax] у головну границю [min...max], враховуючи квадратичну шкалу scSqr;
  - дозвіл на перевищення границі вхідним значенням на величину plcExcess відсотків;
  - фільтрування зміни вхідного значення на час Tf.
- Проста імітація аналогового входу, встановленням plcImit та підключенням джерела імітації-симуляції до plcImitIn.
- Опрацювання кінцевої змінної var на предмет порушення деяких границь та критеріїв, генеруючи аварії та відповідно встановлюючи атрибут err. Границі аварії можуть мати деякий гістерезис HystBnd, можуть бути затримані на час alDelay та можуть бути придушені встановленням alSup. Надаються наступні границі та критерії:
  - відсутність даних або підключення до джерела за вхідним значенням EVAL — код помилки 1 та рівень аварії 5;
  - перевищення апаратної границі вхідним значенням — код помилки 1,2 та рівень аварії 5;
  - перевищення аварійної границі кінцевою змінною [aMin...aMax] — код помилки 3,4 та рівень аварії 4;
￼ !!! Вимикається встановленням однакових або інверсних значень границі
  - перевищення попереджувальної границі кінцевою змінною [wMin...wMax] — код помилки 5,6 та рівень аварії 2;
￼ !!! Вимикається встановленням однакових або інверсних значень границі
  - велика швидкість зміни speed кінцевої змінної — код помилки 7 та рівень аварії 2.
- Передавання специфічної ПІД інформації, контролю та атрибутів коефіцієнтів до та з ПЛК або іншого джерела даних із регулюванням.

Автор: Роман Савоченко <roman@oscada.org>
Спонсоровано: ТОВ "ДІЯ"
Версія: 1.2.0
Ліцензія: GPLv2','',10,0,'JavaLikeCalc.JavaScript
if(f_start) {
	f_err = "0";
	alDelay_ = 0;
}

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
if(tErr)	EVAL = true;
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

//Alarms forming
if(alSup) {
	if(f_err.toInt())	this.alarmSet(DESCR+": "+tr("SUPPRESSED"), 1);
	f_err = "0";
}
else {
	if(tErr.toInt() && tErr.toInt() != f_err.toInt() && this.cntr().status().toInt())	return;
	if(alDelay > 0 && alDelay_ <= 0 && tErr.toInt() != f_err.toInt())	tErr1 = tErr.toInt();
	if(alDelay > 0 && alDelay_ > 0 && tErr.toInt() != tErr1)	{ alDelay_ = 0; tErr1 = tErr.toInt(); }
	if(alDelay > 0 && alDelay_ < alDelay){ alDelay_ += 1/f_frq; return; }
	if(tErr.toInt())	this.alarmSet(DESCR+": "+tErr.parse(1,":"), -4);
	else	this.alarmSet(DESCR+": "+tr("NORM"), 1);
	f_err = tErr;
	alDelay_ = 0;
}','','',1627833164);
INSERT INTO tmplib_base VALUES('SNMP','Network devices by SNMP','Мережеві пристрої за SNMP','Сетевые устройства по SNMP','Data combination of a device, accessible by the protocol SNMP, in a complex object of the attribute "All items". The template designed to use together with the DAQ-module "SNMP" and to represent this data by the frame "Object properties" as a complex object with properties, signalling at single borders and writing. Considering the great specificity and variety of SNMP-accessible devices, this template is actually a billet and an example of creating a template based on it to a corresponding device with a specific to it processing of input properties, forming of violations and write capability.

Author: Roman Savochenko <roman@oscada.org>
Sponsored by: PAC HARTRON
Version: 1.1.0
License: GPLv2','Об''єднання даних пристрою, доступного за протоколом SNMP, у комплексному об''єкті атрибуту "Всі елементи". Шаблон розроблено для використання із модулем джерела даних "SNMP" та представлення цих даних кадром "Властивості об''єкту" як комплексний об''єкт із властивостями, сигналізацією за окремими границями та записом. Враховуючи велику специфіку та розмаїття пристроїв доступних за протоколом SNMP, цей шаблон фактично є заготовкою та прикладом створення на його основі шаблону до відповідного пристрою із специфічною до нього обробкою вхідних властивостей, формування порушень та можливістю запису.

Автор: Роман Савоченко <roman@oscada.org>
Спонсорування: ПАС ХАРТРОН
Версія: 1.1.0
Ліцензія: GPLv2','',10,0,'JavaLikeCalc.JavaScript
if(f_start)	{ srcPrm = false; items = new Object(); alDelay_ = 0; }

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
if(alSup) {
	if(f_err.toInt())	this.alarmSet(DESCR+": "+tr("SUPPRESSED"), 1);
	f_err = "0";
}
else {
	if(tErr.toInt() && tErr.toInt() != f_err.toInt() && this.cntr().status().toInt())	return;
	if(alDelay > 0 && alDelay_ <= 0 && tErr.toInt() != f_err.toInt())	tErr1 = tErr.toInt();
	if(alDelay > 0 && alDelay_ > 0 && tErr.toInt() != tErr1)	{ alDelay_ = 0; tErr1 = tErr.toInt(); }
	if(alDelay > 0 && alDelay_ < alDelay){ alDelay_ += 1/f_frq; return; }
	if(tErr.toInt())	this.alarmSet(DESCR+": "+tErr.parse(1,":"), -(2+alLev));
	else	this.alarmSet(DESCR+": "+tr("NORM"), 1);
	f_err = tErr;
	alDelay_ = 0;
}','','',1572284562);
INSERT INTO tmplib_base VALUES('codeState','Code state','Стан за кодом','Состояние по коду','A variant of the common, representative and unified template of the block for union of Discrete parameters, expanded by more than two states and three commands that can be encoded by an integer value. The template forms a structure of parameter (complex tag) which can be easily connected to most widgets and cadres of the main elements library of the user interface just pointing the parameter object.

The representative structure of the parameter (complex tag) of the code state is:
- "State "Text" (st_text)" — text description of the state.
- "Command "Text" (com_text)" — text of the command.
- "Commands (digComs)" — common list of the allowed commands, to form the selecting.
- "Error (err)" — obligatory attribute of any parameter, consist of the combination of the error code and short text description, like to "2:No data or connection with source". The template reserves next error codes:
  - 1 — hardware errors, when the data mostly wrong.
- "Violations suppress (alSup) and delay (alDelay)" — violations generic control for suppressing and delaying.

Functions:
- Direct receiving, by link, the status code and the command (read only), as well as setting the status text according to the list of states stats.
- Working out the text of the command com_text to find it in the general list coms of commands and issuing the received command code to the output.
- Generating from the general list coms of commands the list of commands to be selected in digComs.
- Complex processing and complete generation-imitation of the code state, as well as processing the code or the command text for targeted actions, on the internal programming language of OpenSCADA into the configuration IO inProc, where you can operate next service parameters:
  - in [IN|OUT] — IO of the code state;
  - text [OUT] — direct text status setting;
  - coms [IN] — list of available commands, of the corresponded IO;
  - com_text [IN|OUT] — text of the received command;
  - levErr, tErr [IN|OUT] — alarm level [-7...0] and text of the error (the attribute "err" format);
  - f_frq [IN] — frequency of the calculation, the template IO;
  - this [IN] — reference to the parameter object, what executing the template;
  - ctx [IN] — object of the user context, what saved between the execution cycles.
- Formation of the violation and the corresponding setting of the attribute err, provided at setting in the processing procedure or lack of the communication (in = EVAL). What can be delayed on the time alDelay and can be suppressed setting alSup.

Author: Roman Savochenko <roman@oscada.org>
Version: 1.2.0
License: GPLv2','Варіант загального, представницького та уніфікованого шаблону блоку поєднання дискретних параметрів, розширений більш ніж двома станами та трьома командами які можна закодувати цілим значенням. Шаблон формує структуру параметру (складного тегу) стану за кодом який може бути легко підключений до більшості віджетів та кадрів бібліотеки основних елементів інтерфейсу користувача просто вказавши об''єкт параметру.

Представницькою структурою параметру (складного тегу) стану за кодом є:
- "Стан "Текст" (st_text)" — текстовий опис стану.
- "Команда "Текст" (com_text)" — текст команди.
- "Команди (digComs)" — загальний перелік можливих команд, для формування обрання.
- "Помилка (err)" — обов''язковий атрибут будь якого параметру, що містить комбінацію коду помилки та короткого опису, на кшталт "1:Немає даних або підключення до джерела". Шаблон резервує наступні коди помилок:
  - 1 — помилки обладнання, коли дані переважно помилкові.
- "Придушення (alSup) та затримка (alDelay) порушень" — загальний контроль порушень щодо придушення та затримки.

Функції:
- Пряме отримання, за посиланням, коду стану та команди (тільки читання), а також встановлення тексту стану згідно до переліку станів stats.
- Опрацювання тексту команди com_text на предмет пошуку її у загальному переліку команд coms та видача отриманого коду команди на вихід.
- Формування із загальному переліку команд coms переліку команд для обрання у digComs.
- Складна обробка та повна генерація-імітація коду стану, а також обробка коду або тексту команди для цільових дій, на внутрішній мові програмування OpenSCADA у конфігураційному ВВ inProc, де Ви можете оперувати наступними сервісними параметрами:
  - in [IN|OUT] — ВВ коду статусу;
  - text [OUT] — пряме встановлення тексту статусу;
  - coms [IN] — перелік наявних команд, відповідного ВВ;
  - com_text [IN|OUT] — текст отриманої команди;
  - levErr, tErr [IN|OUT] — рівень аварії [-7...0] та текст помилки (формат атрибуту "err");
  - f_frq [IN] — частота обчислення, шаблонний ВВ;
  - this [IN] — посилання на об''єкт параметру, що виконує шаблон;
  - ctx [IN] — об''єкт користувацького контексту, що зберігається між циклами виклику.
- Формування порушення та відповідне встановлення атрибуту err, за умови встановлення у процедурі обробки або відсутності зв''язку (in = EVAL). Що може бути затримано на час alDelay та придушено встановленням alSup.

Автор: Роман Савоченко <roman@oscada.org>
Версія: 1.2.0
Ліцензія: GPLv2','',10,0,'JavaLikeCalc.JavaScript
if(f_start) {
	f_err = "0";
	alDelay_ = 0;
	//Prepare data for preprocessing
	inPrcId = this.nodePath("_");
	inPrcLng = "JavaLikeCalc.JavaScript";
	inPrcArgs = new Object();
	inPrcArgs.this = this;
	inPrcArgs.ctx = new Object();
	return;
}
if(f_stop) return;

tErr = "0", levErr = 0;

//Call a specific preprocessing procedure
st_text = "";
if(inProc.length)	{
	inPrcArgs.f_frq = f_frq;
	inPrcArgs.in = in;
	inPrcArgs.text = "";
	inPrcArgs.levErr = levErr; inPrcArgs.tErr = tErr;
	inPrcArgs.coms = coms; inPrcArgs.com_text = com_text;
	SYS.DAQ.funcCall(inPrcLng, inPrcArgs, inProc, inPrcId);
	in = inPrcArgs.in;
	levErr = inPrcArgs.levErr; tErr = inPrcArgs.tErr;
	st_text = inPrcArgs.text;
	com_text = inPrcArgs.com_text;
}

//Commands processing
digComs = "";
if(!coms.length)	com_text = EVAL;
else if(com_text.isEVal())	com_text = "";
for(off = 0; (tVl=coms.parseLine(0,off)).length; ) {
	off1 = 0;
	code = tVl.parse(0,":",off1).toInt();
	st = tVl.slice(off1);
	digComs += (digComs.length?"\n":"") + st;
	if(com_text.length && com_text == st)	out = code;
}
if(!com_text.isEVal()) com_text = "";

//State set
if(st_text.length) ;
else if(in.isEVal()) {
	tErr = "1:"+tr("No data or connection with source");
	levErr = -5;
	st_text = tr("No data");
}
else {
	st_text = tr("Unknown")+" "+in;
	for(off = 0; (tVl=stats.parseLine(0,off)).length; ) {
		off1 = 0;
		if(in == tVl.parse(0,":",off1).toInt())	{ st_text = tVl.slice(off1); break; }
	}
}

//Alarms forming
if(alSup) {
	if(f_err.toInt())	this.alarmSet(DESCR+": "+tr("SUPPRESSED"), 1);
	f_err = "0";
}
else {
	if(tErr.toInt() && tErr.toInt() != f_err.toInt() && this.cntr().status().toInt())	return;
	if(alDelay > 0 && alDelay_ <= 0 && tErr.toInt() != f_err.toInt())	tErr1 = tErr.toInt();
	if(alDelay > 0 && alDelay_ > 0 && tErr.toInt() != tErr1)	{ alDelay_ = 0; tErr1 = tErr.toInt(); }
	if(alDelay > 0 && alDelay_ < alDelay){ alDelay_ += 1/f_frq; return; }
	if(tErr.toInt())	this.alarmSet(DESCR+": "+tErr.parse(1,":"), levErr);
	else	this.alarmSet(DESCR+": "+tr("NORM"), 1);
	f_err = tErr;
	alDelay_ = 0;
}','','',1572284562);
INSERT INTO tmplib_base VALUES('ntf','Notificator by SMS, EMail-SMTP','Повідомлювач за SMS, EMail-SMTP','Уведомитель посредством SMS, EMail-SMTP','The complex template of the notification contains of parts of the dispatcher and the output user protocol of notification by EMail(SMTP) and SMS.

THE DISPATCHER can be performed for pointed messages of the message buffer of OpenSCADA and applied on the Logical level or the controller object of the module JavaLikeCalc.

THE OUTPUT USER PROTOCOL PART "SMS" provides operations with SMS by a GSM-modem connected as serial device. For now, there supported only sending SMS messages to a number of remote cell phone or GSM-modem. For sending there are supported: ECHO disable, PIN check and set, sending in PDU and the Text mode.

THE OUTPUT USER PROTOCOL PART "SMTP" provides operations with a SMTP-server for delivering EMails by their sending through TCP-Socket or SSL. For now, there are supported: TCP, SSL, AUTH, topic encoding for UTF-8.

Author: Roman Savochenko <roman@oscada.org>
Sponsored by: SVItoVYR LTD
Version: 1.0.3
License: GPLv2','Комплексний шаблон повідомлення містить частини диспетчеру та вихідний користувацький протокол повідомлення за EMail(SMTP) та SMS.

ДИСПЕТЧЕР може виконуватися для вказаних повідомлень буферу повідомлень OpenSCADA та застосовується на Логічному рівні або об''єкті контролеру модуля JavaLikeCalc.

"SMS" ЧАСТИНА ВИХІДНОГО КОРИСТУВАЦЬКОГО ПРОТОКОЛУ надає операції із SMS за допомогою GSM-модему, підключеного як послідовний пристрій. Наразі підтримується лише відправка SMS повідомлень на номер віддаленого мобільного телефону або GSM-модему. Для надсилання підтримується: вимкнення ЛУНИ, перевірка та встановлення PIN, надсилання у PDU та текстовому режимі.

"SMTP" ЧАСТИНА ВИХІДНОГО КОРИСТУВАЦЬКОГО ПРОТОКОЛУ надає операції із SMTP-сервером щодо доставки електронної пошти відправленням через TCP-Сокет або SSL. Наразі підтримуються: TCP, SSL, AUTH, кодування теми у UTF-8.

Автор: Роман Савоченко <roman@oscada.org>
Спонсоровано: ТОВ "СВІТоВИР АВТоМАТИК"
Версія: 1.0.3
Ліцензія: GPLv2','',10,0,'JavaLikeCalc.JavaScript
if(f_start)	io = tr = EVAL;

//-----------------------------------------------
//Request form of the UserProtocol ProtIt="SMTP":
//-----------------------------------------------
//<send auth="{user}:{pass}" from="{FromAddr}" to="{ToAddr}" topic="{My Topic}" err="1:Error">{MessageText}</send>
//  topic - topic of the message;
//  from - sender address;
//  to - destination address of the receiver;
//  auth - authenticate user and password;
//  MessageText - text of the message;
//  err - sets for the request result.
function SMTP( ) {
	//Transport''s timings check
	if(tr.timings() != "5:1")	tr.timings("5:1");
	io.setAttr("err", "");
	if(!tr.start(true))	{ io.setAttr("err", "10:"+tr("Impossible connect to the SMTP-server.")); return; }

	//Wait for a greeting after the connection
	rez = "";
	for(iTr = 0; iTr < 5 && !rez.length; iTr++) rez += tr.messIO("");
	while(rez.length && rez.slice(-2) != "\x0D\x0A" && (trez=tr.messIO("")).length) rez += trez;

	auth = io.attr("auth");

	//Send HELLO
	rez = tr.messIO("EHLO "+SYS.system("hostname -f").parse(0,"\n")+"\x0D\x0A");
	while(rez.length && rez.slice(-2) != "\x0D\x0A" && (trez=tr.messIO("")).length) rez += trez;
	if(!rez.length)	{ io.setAttr("err", "100:"+tr("No response.")); return; }
	for(off = 0; (sit=rez.parse(0,"\x0D\x0A",off)).length; )
		if(sit.toInt() != 250)	{ io.setAttr("err", "101:"+tr("Error HELLO: %1.").replace("%1",sit)); return; }
		else if(sit.indexOf("AUTH LOGIN") >= 0) {
			if(sit.indexOf("PLAIN") < 0)	{ io.setAttr("err", "101:"+tr("Unsupported auth methods: %1.").replace("%1",sit)); return; }
			else if(!auth.length)			{ io.setAttr("err", "101:"+tr("Auth required: %1.").replace("%1",sit)); return; }
		}

	//Send AUTH
	if(auth.length) {
		rez = tr.messIO("AUTH LOGIN\x0D\x0A");
		while(rez.length && rez.slice(-2) != "\x0D\x0A" && (trez=tr.messIO("")).length) rez += trez;
		if(!rez.length)	{ io.setAttr("err", "100:"+tr("No response.")); return; }
		if(rez.toInt() != 334)	{ io.setAttr("err", "102:"+tr("AUTH error: %1.").replace("%1",rez)); return; }

		rez = tr.messIO(SYS.strEncode(io.attr("auth").parse(0,":"),"Base64")+"\x0D\x0A");
		while(rez.length && rez.slice(-2) != "\x0D\x0A" && (trez=tr.messIO("")).length) rez += trez;
		if(!rez.length)	{ io.setAttr("err", "100:"+tr("No response.")); return; }
		if(rez.toInt() != 334)	{ io.setAttr("err", "102:"+tr("AUTH USER error: %1.").replace("%1",rez)); return; }

		rez = tr.messIO(SYS.strEncode(io.attr("auth").parse(1,":"),"Base64")+"\x0D\x0A");
		while(rez.length && rez.slice(-2) != "\x0D\x0A" && (trez=tr.messIO("")).length) rez += trez;
		if(!rez.length)	{ io.setAttr("err", "100:"+tr("No response.")); return; }
		if(rez.toInt() != 235)	{ io.setAttr("err", "102:"+tr("AUTH PASS error: %1.").replace("%1",rez)); return; }
	}

	//Send FROM
	rez = tr.messIO("MAIL FROM:<"+io.attr("from")+">\x0D\x0A");
	while(rez.length && rez.slice(-2) != "\x0D\x0A" && (trez=tr.messIO("")).length) rez += trez;
	if(!rez.length)	{ io.setAttr("err", "100:"+tr("No response.")); return; }
	if(rez.toInt() != 250)	{ io.setAttr("err", "103:"+tr("MAIL FROM error: %1.").replace("%1",rez)); return; }

	//Send TO
	rez = tr.messIO("RCPT TO:<"+io.attr("to")+">\x0D\x0A");
	while(rez.length && rez.slice(-2) != "\x0D\x0A" && (trez=tr.messIO("")).length) rez += trez;
	if(!rez.length)	{ io.setAttr("err", "100:"+tr("No response.")); return; }
	if(rez.toInt() != 250)	{ io.setAttr("err", "104:"+tr("RCPT TO error: %1.").replace("%1",rez)); return; }

	//Prepare and send DATA
	rez = tr.messIO("DATA\x0D\x0A");
	while(rez.length && rez.slice(-2) != "\x0D\x0A" && (trez=tr.messIO("")).length) rez += trez;
	if(!rez.length)	{ io.setAttr("err", "100:"+tr("No response.")); return; }
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
	if(!rez.length)	{ io.setAttr("err", "100:"+tr("No response.")); return; }
	if(rez.toInt() != 250)	{ io.setAttr("err", "105:"+tr("DATA send error: %1.").replace("%1",rez)); return; }

	//Send QUIT
	rez = tr.messIO("QUIT\x0D\x0A");
	while(rez.length && rez.slice(-2) != "\x0D\x0A" && (trez=tr.messIO("")).length) rez += trez;

	//SYS.messInfo("SMTP", "Response: "+rez);

	//Disconnect from the SMTP-server
	tr.start(false);
}

if(!tr.isEVal() && !io.isEVal() && io.attr("ProtIt") == "SMTP") { SMTP(); return; }

//--------------------------------------------
//Request form of the UserProtocol ProtIt="SMS":
//--------------------------------------------
//<cmd pin="1111" tel="+380XXXXXXXXX" text="{0|1}" err="1:Error">{SMSText}</cmd>
//  cmd - command, for now only "send" allowed;
//  pin - PIN code to the SIM card access;
//  tel - telephone number of the receiver (remote cell phone or GSM modem);
//  text{0,1} - SMS in the text mode, only ASCII;
//  SMSText - the message text;
//  err - sets for the request result.
function SMS( ) {
	//Transport''s timings check
	if(tr.timings() != "5000:100")	tr.timings("5000:100");
	io.setAttr("err", "");
	if(io.name() == "send") {
		tel = io.attr("tel");
		if(!tel.length || tel[0] != "+") { io.setAttr("err", "100:"+tr("Telephone number error.")); return; }

		//Disable ECHO
		rez = tr.messIO("ATE0\r");
		while(rez.length && (trez=tr.messIO("")).length) rez += trez;
		if(rez.indexOf("OK\r") < 0) {
			io.setAttr("err", "101:"+tr("Error disabling ECHO: %1").replace("%1",rez));
			return;
		}

		//Set PIN
		if(io.attr("pin").length) {
			//Check for PIN
			rez = tr.messIO("AT+CPIN?\r");
			while(rez.length && (trez=tr.messIO("")).length) rez += trez;
			if(rez.indexOf("OK\r") < 0)	{ io.setAttr("err", "102:"+tr("Error checking for PIN-code: %1").replace("%1",rez)); return; }
			if(rez.indexOf("+CPIN: READY\r") < 0) {	//PIN is needed
				rez = tr.messIO("AT+CPIN="+io.attr("pin")+"\r");
				while(rez.length && (trez=tr.messIO("")).length) rez += trez;
				if(rez.indexOf("OK\r") < 0)	{ io.setAttr("err", "102:"+tr("Error setting PIN: %1").replace("%1",rez)); return; }
			}
		}

		if(io.attr("text").toInt()) {	//SMS in the text mode
			//Switch to Text SMS mode
			rez = tr.messIO("AT+CMGF=1\r");
			while(rez.length && (trez=tr.messIO("")).length) rez += trez;
			if(rez.indexOf("OK\r") < 0)	{ io.setAttr("err", "103:"+tr("Error setting the SMS Text mode: %1").replace("%1",rez)); return; }

			//Send the Telephone number
			rez = tr.messIO("AT+CMGS=\""+tel+"\"\r");
			while(rez.length && (trez=tr.messIO("")).length) rez += trez;
			if(rez.indexOf(">") < 0)	{ io.setAttr("err", "104:"+tr("Error sending SMS: %1").replace("%1",rez)); return; }
			rez = tr.messIO(io.text()+"\x1A");
			while(rez.length && (trez=tr.messIO("")).length) rez += trez;
			//for(var iTr = 0; iTr < 100 && rez.indexOf("OK\r") < 0; iTr++) rez += tr.messIO("");	//Up to 10 seconds wait for reply
			if(rez.indexOf("OK\r") < 0)	{ io.setAttr("err", "104:"+tr("Error sending SMS PDU: %1").replace("%1",rez)); return; }
		}
		else {	//SMS in the PDU mode
			// Switch to PDU SMS mode
			rez = tr.messIO("AT+CMGF=0\r");
			while(rez.length && (trez=tr.messIO("")).length) rez += trez;
			if(rez.indexOf("OK\r") < 0)	{ io.setAttr("err", "103:"+tr("Error setting the SMS PDU mode: %1").replace("%1",rez)); return; }

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
			if(rez.indexOf(">") < 0)	{ io.setAttr("err", "104:"+tr("Error sending SMS: %1").replace("%1",rez)); return; }
			rez = tr.messIO(pdu+"\x1A");
			while(rez.length && (trez=tr.messIO("")).length) rez += trez;
			//for(var iTr = 0; iTr < 100 && rez.indexOf("OK\r") < 0; iTr++) rez += tr.messIO("");	//Up to 10 seconds wait for reply
			if(rez.indexOf("OK\r") < 0)	{ io.setAttr("err", "104:"+tr("Error sending SMS PDU: %1").replace("%1",rez)); return; }
			//SYS.messDebug("TEST SMS","PDU REZ :"+rez);
		}
		io.setAttr("err", "0");
	}
}

if(!tr.isEVal() && !io.isEVal() && io.attr("ProtIt") == "SMS")	{ SMS(); return; }

//---------------------------
//The notification dispatcher
//---------------------------
if(f_start) {
	queue = new Object();
	queueSMS = new Object();
	prTm = SYS.time();
	prTmU = 0;
	emailSentN = SMSSentN = 0;
	emailTrAddr_ = emailTrAddr;
	SMSTrAddr_ = SMSTrAddr;
	SMTPTr = emailTrAddr.length ? SYS.Transport.nodeAt(emailTrAddr, ".") : false;
	SMSTr = SMSTrAddr.length ? SYS.Transport.nodeAt(SMSTrAddr, ".") : false;
	return;	//Don''t real process at start
}
if(f_stop)	{ emailState = SMSState = ""; return; }
if(!SMTPTr || SMTPTr.isEVal() || emailTrAddr_ != emailTrAddr) {
	SMTPTr = emailTrAddr.length ? SYS.Transport.nodeAt(emailTrAddr, ".") : false;
	emailTrAddr_ = emailTrAddr;
}
if(!SMSTr || SMSTr.isEVal() || SMSTrAddr_ != SMSTrAddr)	{
	SMSTr = SMSTrAddr.length ? SYS.Transport.nodeAt(SMSTrAddr, ".") : false;
	SMSTrAddr_ = SMSTrAddr;
}

//Current alarms reading, and the queue of dispatching update
mess = SYS.Archive.messGet(prTm, SYS.time(), messCat, abs(messLev), "<buffer>");
for(iM = 0; iM < mess.length; iM++) {
	oM = mess[iM];
	if((oM.tm == prTm && oM.utm <= prTmU) || (messLev < 0 && oM.level >= 0))	continue;

	if(emailTrAddr.length)	queue[oM.level.toString()+oM.tm.toString()+oM.categ] = oM;
	if(SMSTr)	queueSMS[oM.level.toString()+oM.tm.toString()+oM.categ] = oM;

	if((oM.tm*1e6+oM.utm) > (prTm*1e6+prTmU))	prTm = oM.tm, prTmU = oM.utm;
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

	tr = SMTPTr, io = SYS.XMLNode("send");
	io.setAttr("ProtIt", "SMTP").setAttr("auth", emailAuth).setAttr("from", emailSender).setAttr("to", emailReceiver).setAttr("topic", topic).setText(mess);
	SMTP();

	//Remove the message from the queue anytime
	delete queue[nM]; emailSentN++;
	// and just notify the SMTP errors
	if((tVl=io.attr("err").toInt())) {
		if(tVl == 10)	SMTPTr = false;	//Reconnect the transport
		SYS.messWarning("/service/NTF", "EMail(SMTP) sending error: "+io.attr("err"));
	}
	io = tr = EVAL;

	//SYS.messInfo("NTF", "Send error: "+io.attr("err"));
	/*if(!(tVl=io.attr("err").toInt())) { delete queue[nM]; emailSentN++; }
	else {
		if(tVl == 10)	SMTPTr = false;	//Reconnect the transport
		emailQueueN++;
		if(!emailErr.length)	emailErr = io.attr("err");
	}
	io = tr = EVAL;

	/*req = "sendemail -q -f " + emailSender + " -t " + emailReceiver +
			" -u \"=?utf8?B?" + SYS.strEncode(topic,"Base64") + "?=\" -m \"" + mess + "\" -s " + emailTrAddr;
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
	tr = SMSTr, io = SYS.XMLNode("send");
	io.setAttr("ProtIt", "SMS").setAttr("pin", SMSPin).setAttr("text",SMSTextMd).setAttr("tel", SMSTel).setText(mess);
	SMS();
	if(!io.attr("err").toInt()) { delete queueSMS[nM]; SMSSentN++; }
	else {
		SMSQueueN++;
		if(!SMSErr.length)	SMSErr = io.attr("err");
	}
	io = tr = EVAL;
}
if(!SMSTr)	SMSState = "Disabled!";
else {
	SMSState = tr("Sent %1. In queue %2.").replace("%1",SMSSentN.toString()).replace("%2",SMSQueueN.toString());
	if(SMSErr.length) SMSState += " "+tr("Error: %1.").replace("%1",SMSErr);
}','','',1627833128);
INSERT INTO tmplib_base VALUES('UPS','Uninterruptible Power Supply','Джерело Безперебійного Живлення','Источник Бесперебойного Питания','Data combination of the Uninterruptible Power Supply in a complex object of the attribute "All items". The template designed to use together with the DAQ-module "System" in the part "UPS" and to represent this data by the frame "Object properties" as a complex object with properties, signalling at single borders and writing.

Author: Roman Savochenko <roman@oscada.org>
Sponsored by: DIYA LTD
Version: 1.3.0
License: GPLv2','Об''єднання даних Джерела Безперебійного Живлення у комплексному об''єкті атрибуту "Всі елементи". Шаблон розроблено для використання із модулем джерела даних "Система" у частині "ДБЖ" та представлення цих даних кадром "Властивості об''єкту" як комплексний об''єкт із властивостями, сигналізацією за окремими границями та записом.

Автор: Роман Савоченко <roman@oscada.org>
Спонсоровано: ТОВ "ДІЯ"
Версія: 1.3.0
Ліцензія: GPLv2','',10,0,'JavaLikeCalc.JavaScript
if(f_start)	{ srcPrm = false; items = new Object(); alDelay_ = 0; }

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
if(alSup) {
	if(f_err.toInt())	this.alarmSet(DESCR+": "+tr("SUPPRESSED"), 1);
	f_err = "0";
}
else {
	if(tErr.toInt() && tErr.toInt() != f_err.toInt() && this.cntr().status().toInt())	return;
	if(alDelay > 0 && alDelay_ <= 0 && tErr.toInt() != f_err.toInt())	tErr1 = tErr.toInt();
	if(alDelay > 0 && alDelay_ > 0 && tErr.toInt() != tErr1)	{ alDelay_ = 0; tErr1 = tErr.toInt(); }
	if(alDelay > 0 && alDelay_ < alDelay){ alDelay_ += 1/f_frq; return; }
	if(tErr.toInt())	this.alarmSet(DESCR+": "+tErr.parse(1,":"), -(2+alLev));
	else	this.alarmSet(DESCR+": "+tr("NORM"), 1);
	f_err = tErr;
	alDelay_ = 0;
}','','',1572284562);
INSERT INTO tmplib_base VALUES('initConsAssignTrs','Initiative connections processing for assigned output transports','Опрацювання ініціативних підключень щодо асоціативних вихідних транспортів','Обработка инициативных подключений на предмет ассоциативных выходных транспортов','The template of processing input initiative connections was created to provide the latest chain of implementation and support for conception of data acquisition in the passive mode and the initiative connection, that is — the control of associative connections, created by the input transport at each connection.

The function of creation associative output transports at connections is currently only supported by the module "Sockets" and for TCP-sockets.

The control of associative transport means:
  - obtaining a list of the associative transports, by the function assTrsList() of the input transports;
  - reading from new transports (opened) of identification information, which currently represents only the source object identifier;
    - search, by the received identifier, the source object and assigning to it of this transport;
    - registering in the attribute "prcTr" of the object type.
  - checking of existing transports for their closure (detachment), for which performed the deregistration in the attribute "prcTr" of the object type and the establishment of the source object in the empty value; disconnected output associated transports are reused by the input one upon receiving a new connection then it is switched on.

The template can control the containers of data source objects, constructed both as DAQ-objects of the controllers and logical level parameters, what representing the data source and having a transport address definition field, and what generally defined by the configuration field "srcObjPath" of the format {CntrAddr}:{TrCntrFldAddr}, where:
  - CntrAddr — container address, for example: "ModBus" — for DAQ-objects of the controllers, "LogicLev.CntrPLC" — for logical level parameters;
  - TrCntrFldAddr — address of the transport control field in the source object, for example — "%2fcntr%2fcfg%2fADDR" for "ModBus"; what can be obtained in a configurator at the history of pointing on the control field in the status line.

Author: Roman Savochenko <roman@oscada.org>
Sponsored by: Elyor Turaboev, BLUE STAR GROUP Ltd
Version: 1.1.2
License: GPLv2','Шаблон служби опрацювання вхідних ініціативних підключень створено для надання останньої ланки реалізації та підтримки концепції збору даних у пасивному режимі та ініціативного підключення, а саме — контролю асоціативних транспортів, створюваних вхідним транспортом за кожним підключенням.

Функцію створення вихідних асоціативних транспортів за підключенням на вхідний наразі підтримує лише модуль транспортів "Сокети" та для TCP-сокетів.

Контроль асоціативних транспортів передбачає:
  - отримання переліку асоціативних транспортів, функцією вхідного транспорту assTrsList();
  - читання із нових транспортів (відкритих) ідентифікаційної інформації, яка наразі репрезентує тільки ідентифікатор об''єкту джерела;
    - пошук, за отриманим ідентифікатором, об''єкта джерела та призначення йому адреси цього транспорту;
    - реєстрацію у атрибуті об''єктного типу "prcTr".
  - перевірка діючих транспортів на предмет їх закриття (від''єднання), щодо чого здійснюється дереєстрація у атрибуті об''єктного типу "prcTr" та встановлення об''єкту джерела порожнього значення; вимкнені вихідні асоціативні транспорти повторно використовуються вхідним за отриманням нового підключення, після чого він вмикається.

Шаблон може керувати контейнерами об''єктів джерел даних, побудованих як у вигляді DAQ-об''єктів контролерів, так і параметрів логічного рівня, що репрезентують джерело даних та мають поле визначення адреси транспорту та що загалом визначається конфігураційним полем "srcObjPath" із форматом {CntrAddr}:{TrCntrFldAddr}, де:
  - CntrAddr — адреса контейнеру, наприклад: "ModBus" — для DAQ-об''єктів контролерів, "LogicLev.CntrPLC" — для параметрів логічного рівня;
  - TrCntrFldAddr — адреса поля контролю транспорту у об''єкті джерела, наприклад — "%2fcntr%2fcfg%2fADDR" для "ModBus"; що може бути отримано у конфігураторі за історією наведення на поля контролю у рядку статусу.

Автор: Роман Савоченко <roman@oscada.org>
Спонсорування: Еліор Турабоєв, ТОВ BLUE STAR GROUP
Версія: 1.1.2
Ліцензія: GPLv2','',10,0,'JavaLikeCalc.JavaScript
if(f_start) {
	inTransport_ = "", inTr = EVAL;
	prcTr = new Object();
}

//Check for the transport change and connect
tErr = "";
if(inTr.isEVal() || inTransport != inTransport_)	{
	inTr = SYS.Transport.Sockets["in_"+inTransport];
	inTransport_ = inTransport;
}

if(inTr.isEVal())	tErr = "1:"+tr("Input transport ''%1'' error.").replace("%1",inTransport);
else if(!(CNTR_O=SYS.DAQ.nodeAt(srcObjPath.parse(0,":"),".")))
	tErr = "2:"+tr("Error the source object path ''%1''.").replace("%1", srcObjPath.parse(0,":"));
else if(!(CntrFldPath=srcObjPath.parse(1,":")).length)
	tErr = "3:"+tr("The control field path is empty.");
else {
	isLogPrm = srcObjPath.parse(0,":").parse(1).length;

	outTrs = inTr.assTrsList();
	for(iTr = 0; iTr < outTrs.length; iTr++) {
		oTrNm = outTrs[iTr];
		// The transport is present and active
		if(!(oTrO=SYS.Transport.Sockets["out_"+oTrNm]).isEVal() && oTrO.start()) {
			//  and not registered
			if(prcTr[oTrNm].isEVal()) {
				//   reading the source identifier
				idSeq = oTrO.messIO("").parse(0, ":");
				if(idSeq.length > 20)	idSeq = idSeq.slice(0, 20);
				//   stop for empty-missed identification or missed PLC object
				if(!idSeq.length || (PLC_O=CNTR_O[(isLogPrm?"prm_":"")+idSeq]).isEVal())	{
					oTrO.start(false);
					if(idSeq.length)
						SYS.messNote("initConsAssignTrs", tr("The requested identificator ''%1'' is mіssing in the container ''%2'' of the data source objects!").replace("%1",idSeq).replace("%2",srcObjPath.parse(0,":")));
					continue;
				}

				//   A duple connection is detected - stopping that
				req = SYS.XMLNode("get").setAttr("path",PLC_O.nodePath()+"/"+CntrFldPath);
				SYS.cntrReq(req);
				if((tVl=req.text()).length && tVl != oTrNm && !outTrs[tVl].isEVal() && SYS.Transport.Sockets["out_"+tVl].start()) {
				    if(prcTr[tVl] == idSeq) { oTrO.start(false); continue; }
				    else if(prcTr[tVl].isEVal())	SYS.Transport.Sockets["out_"+tVl].start(false);
				}

				//   connect the transport to a PLC object
				if(outTrTm.length)	oTrO.timings(outTrTm);
				prcTr[oTrNm] = idSeq;
				req.setName("set").setText("Sockets."+oTrNm);
				SYS.cntrReq(req);
			}
		}
		// is not present or active
		else if(!prcTr[oTrNm].isEVal() && !(PLC_O=CNTR_O[(isLogPrm?"prm_":"")+prcTr[oTrNm]]).isEVal()) {
			delete prcTr[oTrNm];
			SYS.cntrReq(SYS.XMLNode("set").setAttr("path",PLC_O.nodePath()+"/"+CntrFldPath).setText(""));
		}
	}
	delete oTrO;
}

//Error set
if(tErr.length)	f_err = tErr;
else f_err = "0";','','',1571249552);
CREATE TABLE IF NOT EXISTS 'lib_Controllers' ("ID" TEXT DEFAULT '' ,"NAME" TEXT DEFAULT '' ,"ru#NAME" TEXT DEFAULT '' ,"uk#NAME" TEXT DEFAULT '' ,"DESCR" TEXT DEFAULT '' ,"ru#DESCR" TEXT DEFAULT '' ,"uk#DESCR" TEXT DEFAULT '' ,"START" INTEGER DEFAULT '1' ,"MAXCALCTM" INTEGER DEFAULT '10' ,"PR_TR" INTEGER DEFAULT '1' ,"FORMULA" TEXT DEFAULT '' ,"ru#FORMULA" TEXT DEFAULT '' ,"uk#FORMULA" TEXT DEFAULT '' ,"TIMESTAMP" INTEGER DEFAULT '' , PRIMARY KEY ("ID"));
INSERT INTO lib_Controllers VALUES('prescr','Prescriptions manager (moved)','','','!!!!: Moved and replaced by the template PrescrTempl.manager. Will be removed soon
Prescriptions manager and controller. Used in addition with user interface''s cadre "Prescription: editing" and "Prescription: runtime" for which into a parameter of the controller you must pass that parameters: "mode", "prog", "startTm", "curCom", "comLs", "work".

Author: Roman Savochenko <roman@oscada.org>
Sponsor: Vasiliy Grigoriev from "Vacuum technologies laboratory (http://e-beam.ru)".
Version: 1.1.1','','',1,10,0,'clcCnt++;

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
		//????
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
				SYS.messInfo("uprg"+prog,tr("Terminated program session by the user")+" \""+prog+"\" : "+SYS.strftime(startTm)+" : "+SYS.strftime(SYS.time()));
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
					SYS.messInfo("uprg"+prog,tr("Terminated program session by the error")+" \""+prog+"\" : "+SYS.strftime(startTm)+" : "+SYS.strftime(SYS.time()));
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
				SYS.messInfo("uprg"+prog, tr("Terminated program session by the user")+" \""+prog+"\" : "+SYS.strftime(startTm)+" : "+SYS.strftime(SYS.time()));
				curMode = mode = -3;
			}
			//Commands process
			else {
				// Start command
				if(!comEl.attr("tm").length) {
					curComPrm.run.set(false);	//Stop for possible background call
					comEl.setAttr("tm", SYS.time());
					for(i_a = 1; i_a <= 5; i_a++)
						if(!(comA=curComPrm["arg"+i_a]).isEVal())
							comA.set(comEl.attr("arg"+i_a));
					curComPrm.rez.set(0);
					curComPrm.run.set(true);
					if(mode == 2) curComPrm.pause.set(true);
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
					SYS.messInfo("uprg"+prog,tr("Terminated program session by the error")+" \""+prog+"\" : "+SYS.strftime(startTm)+" : "+SYS.strftime(SYS.time()));
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
//mode = curMode;','','',1559660641);
INSERT INTO lib_Controllers VALUES('test','test','','','Different tests of the JavaLikeCalc language for execution into the controller mode.

Author: Roman Savochenko
Version: 1.2.0
','','',1,10,0,'// Testing the string parsing functions
// Testing of the string parsing at a separator
/*str = "1;;2;;3;;4;5;;6;";
for(off = 0; (sIt=str.parse(0,";",off,true)).length || off < str.length; )
	SYS.messInfo("TEST string", "Item ("+off+") = "+sIt);
SYS.messInfo("TEST string (direct)", "Item "+str.parse(4,";",0,true));*/

// Testing of the string parsing at a separator from the end
/*str = "1;;2;;3;;4;5;;6;";
for(off = str.length; (sIt=str.parseEnd(0,";",off,true)).length || off >= 0; )
	SYS.messInfo("TEST string", "Item ("+off+") = "+sIt);
SYS.messInfo("TEST string (direct)", "Item "+str.parseEnd(4,";",str.length,true));*/

// Testing of the string parsing as the path
/*str = "/1//2//3//4/5/6/";
for(off = 0; (sIt=str.parsePath(0,0,off)).length; )
	SYS.messInfo("TEST string", "Item ("+off+") = "+sIt);
SYS.messInfo("TEST string (direct)", "Item "+str.parsePath(4));*/

// Testing of the string parsing as the path from the end
/*str = "/1//2//3//4/5/6/";
for(off = str.length; (sIt=str.parsePathEnd(0,off)).length || off >= 0; )
	SYS.messInfo("TEST string", "Item ("+off+") = "+sIt);
SYS.messInfo("TEST string (direct)", "Item "+str.parsePathEnd(4));*/

// Testing of the string-text parsing as the lines
/*str = "1\n2\n\n3\x0A4\x0D5\x0D\x0A6";
for(off = 0; (sIt=str.parseLine(0,off)).length || off < str.length; )
	SYS.messInfo("TEST string", "Item ("+off+") = "+sIt);
SYS.messInfo("TEST string (direct)", "Item "+str.parseLine(4));*/


/*using Special.FLibSYS;
out+=10;
if(out>100) out=0;

test = (10).toString(16,2,true);
test = (-10.34).toFixed(4,8);*/


//==================================================
//Testing common operation of the internal functions
//--------------------------------------------------
function f1( ) {
	aOut = 3.14;
	a = 20*2;
	b = 20*3;
	c = a + b;
	SYS.messInfo("InF","F1: a="+a+"; b="+b+"; c="+c+"; inFarg="+inFarg);
	return c;
}

function f2( ) {
	a = 30*2;
	b = 30*3;
	c = a + b + f1();
	SYS.messInfo("InF","F2: a="+a+"; b="+b+"; c="+c);
	return c;
}

a = 10*2;
b = 10*3;
res = c = a + b + f2();

function fR( a1, a2 ) {
	SYS.messInfo("InF","a2="+a2);
	d = 10;
	if(a1 < 5) fR(a1+1, a2);
	d++; a2++;
	SYS.messInfo("InF","d="+d+"; a1="+a1+"; a2="+a2);
}

//aOut = 0;
fR(0, aOut=0);

SYS.messInfo("InF","a="+a+"; b="+b+"; c="+c+"; f2="+f2()+"; aOut="+aOut);

//===========================================
//Testing recursion of the internal functions
//-------------------------------------------
/*function ASN1_Data( inSeq, offIn ) {
	val = EVAL;
	offIn += 1;
	if((vTp=inSeq.charCodeAt(offIn-1)) == 0)	//NULL
		val = 0;
	else if(vTp == 1 || vTp == 2) {		//Array, Structure
		itSz = inSeq.charCodeAt(offIn);
		val = new Array(itSz);
		offIn += 1;
		for(itI = 0; itI < itSz; itI++)
			val[itI] = ASN1_Data(inSeq, offIn);
	}
	else if(vTp == 4 || vTp == 18) {		//BIT STRING, Unsigned16
		val = inSeq.charCodeAt(offIn, "UTF-16BE");
		offIn += 2;
	}
	else if(vTp == 5) {							//Integer32
		val = inSeq.charCodeAt(offIn, "UTF-32BE");
		if(val > 2147483647) val -= 4294967296;
		offIn += 4;
	}
	else if(vTp == 6) {							//Unsigned32
		val = inSeq.charCodeAt(offIn, "UTF-32BE");
		offIn += 4;
	}
	else if(vTp == 9 || vTp == 10) {		//OCTET STRING, VisibleString
		itSz = inSeq.charCodeAt(offIn);
		val = inSeq.slice(offIn+1, offIn+1+itSz);
		offIn += 1+itSz;
	}
	else if(vTp == 13 || vTp == 15) {		//Integer8
		val = inSeq.charCodeAt(offIn);
		if(val > 127) val -= 256;
		offIn += 1;
	}
	else if(vTp == 16) {						//Integer16
		val = inSeq.charCodeAt(offIn, "UTF-16BE");
		if(val > 32767) val -= 65536;
		offIn += 2;
	}
	else if(vTp == 17 || vTp == 22) {		//Unsigned8, ENUMERATED
		val = inSeq.charCodeAt(offIn);
		offIn += 1;
	}
	else if(vTp == 18) {						//Unsigned16
		val = inSeq.charCodeAt(offIn, "UTF-16BE");
		offIn += 2;
	}
	else {
		SYS.messInfo("/IEC62056", tr("Unknown data type %1, termination").replace("%1",vTp.toString()));
		offIn = inSeq.length;
	}
	//SYS.messInfo("/IEC62056", tr("Unknown data type %1, termination").replace("%1",vTp.toString()));

	return val;
}

inAMess = SYS.strFromCharCode(0x02, 0x02, 0x11, 0x07, 0x11, 0x1F);
val = ASN1_Data(inAMess, 0);
SYS.messInfo("/IEC62056", "Data value "+typeof(val)+": "+val);*/


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
}*/','','',1629440988);
INSERT INTO lib_Controllers VALUES('test1','test1','','','Different tests of the JavaLikeCalc language for execution into the controller mode.

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
//p_tm=c_tm;','','',1560101785);
INSERT INTO lib_Controllers VALUES('ntfDispatch','Notifications dispatcher (replaced)','','','!!!!: Replaced by the complex template Main.ntf. Will be removed soon
Notifications dispatcher by EMail and SMS for pointed messages of OpenSCADA messages buffer.

Author: Roman Savochenko <roman@oscada.org>
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

	if((oM.tm*1e6+oM.utm) > (prTm*1e6+prTmU))	prTm = oM.tm, prTmU = oM.utm;
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
}','','',1560101769);
CREATE TABLE IF NOT EXISTS 'lib_servProc' ("ID" TEXT DEFAULT '' ,"NAME" TEXT DEFAULT '' ,"uk#NAME" TEXT DEFAULT '' ,"ru#NAME" TEXT DEFAULT '' ,"DESCR" TEXT DEFAULT '' ,"uk#DESCR" TEXT DEFAULT '' ,"ru#DESCR" TEXT DEFAULT '' ,"START" INTEGER DEFAULT '1' ,"MAXCALCTM" INTEGER DEFAULT '10' ,"PR_TR" INTEGER DEFAULT '0' ,"FORMULA" TEXT DEFAULT '' ,"uk#FORMULA" TEXT DEFAULT '' ,"ru#FORMULA" TEXT DEFAULT '' ,"TIMESTAMP" INTEGER DEFAULT '' , PRIMARY KEY ("ID"));
INSERT INTO lib_servProc VALUES('procArh','Archives recalculation','Перерахунок архівів','Пересчёт архивов','Value archives recalculation.

Author: Roman Savochenko <roman@oscada.org>
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
}','','',1560097197);
INSERT INTO lib_servProc VALUES('crc16','CRC 16','','','Standard Cyclic Redundancy Check (CRC) with free setting the polynomial, by default 0x8005. It isn''t actual more by the common CRC implementing.

Author: Roman Savochenko <roman@oscada.org>
Version: 1.0.0','','',1,10,0,'out = 0xFFFF;
for(i = 0; i < in.length; i++) {
  out = out^in.charCodeAt(i);
  for(j = 0; j < 8; j++) out = (out&1) ? ((out>>1)^poly) : out >> 1;
}','','',1560097197);
INSERT INTO lib_servProc VALUES('releaseTests','TEST: Release','Тести випуску','Тесты выпуска','Set of formal tests of the OpenSCADA releases:
  - 0:System
  - 1:Security User/Group tests
  - 2:Transport subsystem tests
	- Data transferring through output and input transports by the test "TrOut":
		means presence of the input transports with the name "testRelease[{N}]" and
			the connected output transport adress into description or address field (if equal one).
  - 3:Protocol subsystem tests
	- Execution of the test controller object DAQ.ModBUS.testTCP:
		means presence of the controller object DAQ.ModBUS.testTCP.
	- Execution of the test controller object DAQ.OPC_UA.test:
		means presence of the  controller object DAQ.OPC_UA.test.
	- Send requests to a test-echo protocol from a transport:
		means presence of the user protocol object Protocol.UserProtocol.up_test.
  - 4:Archive subsystem tests
	- Using the test "Mess" of the module "SystemTests" for archiving messages:
		means presence the message archivers "test";
	- Assign/Clear archiving from the parameter attribute:
		means presence "/DAQ/LogicLev/experiment/F3" with the attribute "ed", and
		the value archivers "1s"
	- The tests "Archive" of the value archiver to verify the correctness of the functioning
			of the sequential mechanism for packaging by "SystemTests":
		means presence the archive "testArch_cntr" with the data period 1s and
		processed by the archiver FSArch.1s.
  - 5:DB subsystem tests
	- The tests "BD" of the module "SystemTests":
		means presence of the test DB with the name "testRelease".
  - 6:DAQ subsystem tests
	- Test "Param" from the module "SystemTests":
		presence of the DAQ parameter LogicLev.experiment.F3.
	- Test "Val" of the module "SystemTests" for the parameter attribute:
		presence of the DAQ parameter LogicLev.experiment.F3.var.
	- Blocks create/remove, change, load, save and load from config and copy, set link:
		presence the DAQ controller object "KM102cntr" for copy the block scheme.
		presence the DAQ parameter LogicLev.experiment.F3.var.
  - 7:UI subsystem tests
  - 8:Special subsystem tests

Author: Roman Savochenko <roman@oscada.org>
Version: 2.0.0','','',1,600,0,'nNm = "testNode";
node  = false;		//Process node
testREZ = "";		//Test rezult
rez = "";
rezF = "";

function result(name, val) {
	rez += name + "\n> " + val + "\n\n";
	rezF += "<tr><td>"+name+"</td><td>"+val+"</td></tr>\n";
}

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
	stTm = SYS.utime();
	testREZ = SYS.Special.SystemTests.XML(configPath);
	result("Control interface: Test parsing the XML file", testREZ);
	testREZ = SYS.Special.SystemTests.SysContrLang("/Archive/FSArch/mess_StatErrors/%2fprm%2fst");
	result("Control interface: Test of the control interface", testREZ);
	testREZ = SYS.Special.SystemTests.Base64Code();
	result("SYS: Tests for encoding of the Mime Base64 algorithm", testREZ);
	result("SYS: Time of the execution, seconds", (SYS.utime()-stTm)*1e-6);
}

//**********************************
//Security User/Group tests
if(sub < 0 || sub == 1) {
	stTm = SYS.utime();
	testREZ = "PASSED";
	// User
	nAddr = "/Security/usr_"+nNm;
	if(SYS.cntrReq(SYS.XMLNode("add").setAttr("path","/Security/%2fbr%2fusr_").setText(nNm)) != 0 || !(node=SYS.nodeAt(nAddr))) testREZ = "FAILED: user add";
	else if(SYS.cntrReq(SYS.XMLNode("set").setAttr("path",nAddr+"/%2fprm%2fDESCR").setText("Test")) != 0 || node.cfg("DESCR") != "Test") testREZ = "FAILED: change user";
	else if(SYS.cntrReq(SYS.XMLNode("save").setAttr("path",nAddr+"/%2fobj")) != 0) testREZ = "FAILED: save user";
	else if(SYS.cntrReq(SYS.XMLNode("set").setAttr("path",nAddr+"/%2fprm%2fDESCR").setText("Test 1")) != 0 || 
		SYS.cntrReq(SYS.XMLNode("load").setAttr("path",nAddr+"/%2fobj")) != 0 || node.cfg("DESCR") != "Test") testREZ = "FAILED: load user";
	else {
		node = false;
		if(SYS.cntrReq(SYS.XMLNode("del").setAttr("path","/Security/%2fbr%2fusr_").setText(nNm)) != 0 || SYS.nodeAt(nAddr)) testREZ = "FAILED: user delete";
		else {
			if(!cfgTree.getElementBy("Security_user"))	cfgTree.getElementBy("sub_Security").childAdd("tbl").setAttr("id","Security_user");
			cfgTree.getElementBy("Security_user").childAdd("fld").setAttr("NAME",nNm+"_cfg");
			cfgTree.save(0x20, configPath);
			SYS.cntrReq(SYS.XMLNode("scan").setAttr("path","/%2fgen%2fconfig"));		//Reload config command
			if(!SYS.nodeAt(nAddr+"_cfg")) testREZ = "FAILED: load user from config";
		}
	}
	//Group
	nAddr = "/Security/grp_"+nNm;
	if(testREZ == "PASSED") {
		if((SYS.cntrReq(SYS.XMLNode("add").setAttr("path","/Security/%2fbr%2fgrp_").setText(nNm)) != 0 || !(node=SYS.nodeAt(nAddr)))) testREZ = "FAILED: group add";
		else if(SYS.cntrReq(SYS.XMLNode("set").setAttr("path",nAddr+"/%2fprm%2fDESCR").setText("Test")) != 0 || node.cfg("DESCR") != "Test") testREZ = "FAILED: change group";
		else if(SYS.cntrReq(SYS.XMLNode("save").setAttr("path",nAddr+"/%2fobj")) != 0) testREZ = "FAILED: save group";
		else if(SYS.cntrReq(SYS.XMLNode("set").setAttr("path",nAddr+"/%2fprm%2fDESCR").setText("Test 1")) != 0 || 
			SYS.cntrReq(SYS.XMLNode("load").setAttr("path",nAddr+"/%2fobj")) != 0 || node.cfg("DESCR") != "Test") testREZ = "FAILED: load group";
		else {
			node = false;
			if(SYS.cntrReq(SYS.XMLNode("del").setAttr("path","/Security/%2fbr%2fgrp_").setText(nNm)) != 0 || SYS.nodeAt(nAddr)) testREZ = "FAILED: group delete";
			else {
				if(!cfgTree.getElementBy("Security_grp"))	cfgTree.getElementBy("sub_Security").childAdd("tbl").setAttr("id","Security_grp");
				cfgTree.getElementBy("Security_grp").childAdd("fld").setAttr("NAME",nNm+"_cfg").setAttr("USERS",nNm+"_cfg");
				cfgTree.save(0x20, configPath);
				SYS.cntrReq(SYS.XMLNode("scan").setAttr("path","/%2fgen%2fconfig"));		//Reload config command
				if(!SYS.nodeAt(nAddr+"_cfg")) testREZ = "FAILED: load group from config";
			}
		}
	}
	result("Security: User/Group create/remove, change, load, save and load from the config", testREZ);
	result("Security: Time of the execution, seconds", (SYS.utime()-stTm)*1e-6);
}

//**********************************
//Transport subsystems tests
if(sub < 0 || sub == 2) {
	stTm = SYS.utime();
	nList = SYS.Transport.nodeList("mod_");
	if(!cfgTree.getElementBy("Transport_in"))	cfgTree.getElementBy("sub_Transport").childAdd("tbl").setAttr("id","Transport_in");
	if(!cfgTree.getElementBy("Transport_out"))	cfgTree.getElementBy("sub_Transport").childAdd("tbl").setAttr("id","Transport_out");
	for(iN = 0; iN < nList.length; iN++) {
		//Disconnect/Connect module
		req = SYS.XMLNode("get").setAttr("path","/Transport/"+nList[iN]+"/%2fmodule%2fm_inf%2fSource");
		SYS.cntrReq(req);
		if(req.text()[0] != "*") {
			testREZ = SYS.Special.SystemTests.SOAttach(req.text(),-1);
			testREZ += "/"+SYS.Special.SystemTests.SOAttach(req.text(),1);
			result("Transport."+nList[iN].slice(4)+": Disconnect/Connect", testREZ);
		} else result("Transport."+nList[iN].slice(4)+": Disconnect/Connect", "Built-in");

		//Input transport
		testREZ = "PASSED";
		nAddr = "/Transport/"+nList[iN]+"/in_"+nNm;
		if(SYS.cntrReq(SYS.XMLNode("add").setAttr("path","/Transport/"+nList[iN]+"/%2fbr%2fin_").setAttr("id",nNm)) != 0 || !(node=SYS.nodeAt(nAddr))) testREZ = "FAILED: input add";
		else if(SYS.cntrReq(SYS.XMLNode("set").setAttr("path",nAddr+"/%2fprm%2fcfg%2fDESCRIPT").setText("Test")) != 0 || node.cfg("DESCRIPT") != "Test") testREZ = "FAILED: change input";
		else if(SYS.cntrReq(SYS.XMLNode("save").setAttr("path",nAddr+"/%2fobj")) != 0) testREZ = "FAILED: save input";
		else if(SYS.cntrReq(SYS.XMLNode("set").setAttr("path",nAddr+"/%2fprm%2fcfg%2fDESCRIPT").setText("Test 1")) != 0 || 
			SYS.cntrReq(SYS.XMLNode("load").setAttr("path",nAddr+"/%2fobj")) != 0 || node.cfg("DESCRIPT") != "Test") testREZ = "FAILED: load input";
		else {
			node = false;
			if(SYS.cntrReq(SYS.XMLNode("del").setAttr("path","/Transport/"+nList[iN]+"/%2fbr%2fin_").setAttr("id",nNm)) != 0 || SYS.nodeAt(nAddr)) testREZ = "FAILED: input delete";
			else {
				cfgTree.getElementBy("Transport_in").childAdd("fld").setAttr("ID",nNm+"_cfg").setAttr("MODULE",nList[iN].slice(4));
				cfgTree.save(0x20, configPath);
				SYS.cntrReq(SYS.XMLNode("scan").setAttr("path","/%2fgen%2fconfig"));		//Reload config command
				if(!SYS.nodeAt(nAddr+"_cfg")) testREZ = "FAILED: load input from config";
			}
		}
		//Output transport
		nAddr = "/Transport/"+nList[iN]+"/out_"+nNm;
		if(testREZ == "PASSED") { 
			if((SYS.cntrReq(SYS.XMLNode("add").setAttr("path","/Transport/"+nList[iN]+"/%2fbr%2fout_").setAttr("id",nNm)) != 0 || !(node=SYS.nodeAt(nAddr)))) testREZ = "FAILED: output add";
			else if(SYS.cntrReq(SYS.XMLNode("set").setAttr("path",nAddr+"/%2fprm%2fcfg%2fDESCRIPT").setText("Test")) != 0 || node.cfg("DESCRIPT") != "Test") testREZ = "FAILED: change output";
			else if(SYS.cntrReq(SYS.XMLNode("save").setAttr("path",nAddr+"/%2fobj")) != 0) testREZ = "FAILED: save output";
			else if(SYS.cntrReq(SYS.XMLNode("set").setAttr("path",nAddr+"/%2fprm%2fcfg%2fDESCRIPT").setText("Test 1")) != 0 || 
				SYS.cntrReq(SYS.XMLNode("load").setAttr("path",nAddr+"/%2fobj")) != 0 || node.cfg("DESCRIPT") != "Test") testREZ = "FAILED: load output";
			else {
				node = false;
				if(SYS.cntrReq(SYS.XMLNode("del").setAttr("path","/Transport/"+nList[iN]+"/%2fbr%2fout_").setAttr("id",nNm)) != 0 || SYS.nodeAt(nAddr)) testREZ = "FAILED: output delete";
				else {
					cfgTree.getElementBy("Transport_out").childAdd("fld").setAttr("ID",nNm+"_cfg").setAttr("MODULE",nList[iN].slice(4));
					cfgTree.save(0x20, configPath);
					SYS.cntrReq(SYS.XMLNode("scan").setAttr("path","/%2fgen%2fconfig"));		//Reload config command
					if(!SYS.nodeAt(nAddr+"_cfg")) testREZ = "FAILED: load output from config";
				}
			}
		}
		result("Transport."+nList[iN].slice(4)+": Input/Output create/remove, change, load, save and load from config", testREZ);

		//Transmission
		iTrList = SYS.Transport[nList[iN]].nodeList("in_");
		for(testREZ = "", iTr = 0; iTr < iTrList.length; iTr++) {
			iTrO = SYS.Transport[nList[iN]][iTrList[iTr]];
			if(!(iTrOutAddr=iTrO.cfg("DESCRIPT")).length) iTrOutAddr = iTrO.cfg("ADDR");
			if(iTrList[iTr].slice(0,14) != "in_testRelease")	continue;
			testREZ += (testREZ.length?"; ":"") + iTrOutAddr + ": " +
				SYS.Special.SystemTests.TrOut(iTrOutAddr,nList[iN].slice(4),"1234567890");
		}
		iTrO = 0;
		result("Transport."+nList[iN].slice(4)+": Data transferring through output and input transports by the test \"TrOut\"", testREZ.length?testREZ:"No test input transports");
	}
	result("Transport: Time of the execution, seconds", (SYS.utime()-stTm)*1e-6);
}

//**********************************
//Protocol subsystems tests
if(sub < 0 || sub == 3) {
	stTm = SYS.utime();
	nList = SYS.Protocol.nodeList("mod_");
	for(iN = 0; iN < nList.length; iN++) {
		if(nList[iN].slice(4) == "ModBus")
			result("Protocol."+nList[iN].slice(4)+": Execution of the test controller object DAQ.ModBUS.testTCP",
				(prmO=SYS.nodeAt("/DAQ/ModBus/testTCP"))?prmO.status():"missed");
		else if(nList[iN].slice(4) == "OPC_UA")
			result("Protocol."+nList[iN].slice(4)+": Execution of the test controller object DAQ.OPC_UA.test",
				(prmO=SYS.nodeAt("/DAQ/OPC_UA/test"))?prmO.status():"missed");
		else if(nList[iN].slice(4) == "UserProtocol") {
			SYS.cntrReq((req=SYS.XMLNode("get").setAttr("path","/Protocol/UserProtocol/up_test/%2fup%2fst%2fstatus")));
			result("Protocol."+nList[iN].slice(4)+": Send requests to a test-echo protocol from a transport", req.text());
		}

		//Disconnect/Connect module
		req = SYS.XMLNode("get").setAttr("path","/Protocol/"+nList[iN]+"/%2fmodule%2fm_inf%2fSource");
		SYS.cntrReq(req);
		if(req.text()[0] != "*") {
			testREZ = SYS.Special.SystemTests.SOAttach(req.text(),-1);
			testREZ += "/"+SYS.Special.SystemTests.SOAttach(req.text(),1);
			result("Protocol."+nList[iN].slice(4)+": Disconnect/Connect", testREZ);
		} else result("Protocol."+nList[iN].slice(4)+": Disconnect/Connect", "Built-in");
	}
	result("Protocol: Time of the execution, seconds", (SYS.utime()-stTm)*1e-6);
}

//**********************************
//Archive subsystem tests
if(sub < 0 || sub == 4) {
	stTm = SYS.utime();
	testREZ = SYS.Special.SystemTests.ValBuf();
	result("Archive: Tests for the value buffer.", testREZ);

	// Value archive
	testREZ = "PASSED";
	nAddr = "/Archive/va_"+nNm;
	if(SYS.cntrReq(SYS.XMLNode("add").setAttr("path","/Archive/%2fbr%2fva_").setAttr("id",nNm)) != 0 || !(node=SYS.nodeAt(nAddr))) testREZ = "FAILED: add";
	else if(SYS.cntrReq(SYS.XMLNode("set").setAttr("path",nAddr+"/%2fprm%2fcfg%2fDESCR").setText("Test")) != 0 || node.cfg("DESCR") != "Test") testREZ = "FAILED: change";
	else if(SYS.cntrReq(SYS.XMLNode("save").setAttr("path",nAddr+"/%2fobj")) != 0) testREZ = "FAILED: save";
	else if(SYS.cntrReq(SYS.XMLNode("set").setAttr("path",nAddr+"/%2fprm%2fcfg%2fDESCR").setText("Test 1")) != 0 || 
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
	result("Archive: Value archive create/remove, change, load, save and load from config", testREZ);

	// Archivers
	nList = SYS.Archive.nodeList("mod_");
	cfgTree.getElementBy("sub_Archive").childAdd("tbl").setAttr("id","Archive_mess_proc");
	cfgTree.getElementBy("sub_Archive").childAdd("tbl").setAttr("id","Archive_val_proc");
	for(iN = 0; iN < nList.length; iN++) {
		testREZ = "PASSED";
		//Messages archiver
		nAddr = "/Archive/"+nList[iN]+"/mess_"+nNm;
		if(SYS.cntrReq(SYS.XMLNode("add").setAttr("path","/Archive/"+nList[iN]+"/%2fbr%2fmess_").setAttr("id",nNm)) != 0 || !(node=SYS.nodeAt(nAddr))) testREZ = "FAILED: messages add";
		else if(SYS.cntrReq(SYS.XMLNode("set").setAttr("path",nAddr+"/%2fprm%2fcfg%2fDESCR").setText("Test")) != 0 || node.cfg("DESCR") != "Test") testREZ = "FAILED: change messages";
		else if(SYS.cntrReq(SYS.XMLNode("save").setAttr("path",nAddr+"/%2fobj")) != 0) testREZ = "FAILED: save messages";
		else if(SYS.cntrReq(SYS.XMLNode("set").setAttr("path",nAddr+"/%2fprm%2fcfg%2fDESCR").setText("Test 1")) != 0 || 
			SYS.cntrReq(SYS.XMLNode("load").setAttr("path",nAddr+"/%2fobj")) != 0 || node.cfg("DESCR") != "Test") testREZ = "FAILED: load messages";
		else {
			node = false;
			if(SYS.cntrReq(SYS.XMLNode("del").setAttr("path","/Archive/"+nList[iN]+"/%2fbr%2fmess_").setAttr("id",nNm)) != 0 || SYS.nodeAt(nAddr)) testREZ = "FAILED: messages delete";
			else {
				cfgTree.getElementBy("Archive_mess_proc").childAdd("fld").setAttr("ID",nNm+"_cfg").setAttr("MODUL",nList[iN].slice(4));
				cfgTree.save(0x20, configPath);
				SYS.cntrReq(SYS.XMLNode("scan").setAttr("path","/%2fgen%2fconfig"));		//Reload config command
				if(!SYS.nodeAt(nAddr+"_cfg")) testREZ = "FAILED: load messages from config";
			}
		}
		//Value archiver
		nAddr = "/Archive/"+nList[iN]+"/val_"+nNm;
		if(testREZ == "PASSED") {
			if((SYS.cntrReq(SYS.XMLNode("add").setAttr("path","/Archive/"+nList[iN]+"/%2fbr%2fval_").setAttr("id",nNm)) != 0 || !(node=SYS.nodeAt(nAddr)))) testREZ = "FAILED: values add";
			else if(SYS.cntrReq(SYS.XMLNode("set").setAttr("path",nAddr+"/%2fprm%2fcfg%2fDESCR").setText("Test")) != 0 || node.cfg("DESCR") != "Test") testREZ = "FAILED: change values";
			else if(SYS.cntrReq(SYS.XMLNode("save").setAttr("path",nAddr+"/%2fobj")) != 0) testREZ = "FAILED: save values";
			else if(SYS.cntrReq(SYS.XMLNode("set").setAttr("path",nAddr+"/%2fprm%2fcfg%2fDESCR").setText("Test 1")) != 0 || 
				SYS.cntrReq(SYS.XMLNode("load").setAttr("path",nAddr+"/%2fobj")) != 0 || node.cfg("DESCR") != "Test") testREZ = "FAILED: load values";
			else {
				node = false;
				if(SYS.cntrReq(SYS.XMLNode("del").setAttr("path","/Archive/"+nList[iN]+"/%2fbr%2fval_").setAttr("id",nNm)) != 0 || SYS.nodeAt(nAddr)) testREZ = "FAILED: values delete";
				else {
					cfgTree.getElementBy("Archive_val_proc").childAdd("fld").setAttr("ID",nNm+"_cfg").setAttr("MODUL",nList[iN].slice(4));
					cfgTree.save(0x20, configPath);
					SYS.cntrReq(SYS.XMLNode("scan").setAttr("path","/%2fgen%2fconfig"));		//Reload config command
					if(!SYS.nodeAt(nAddr+"_cfg")) testREZ = "FAILED: load values from config";
				}
			}
		}
		result("Archive."+nList[iN].slice(4)+": Message/Value archiver create/remove, change, load, save and load from config", testREZ);

		//Using the test "Mess" of the module "SystemTests" for archiving messages
		nNm1 = "test";
		result("Archive."+nList[iN].slice(4)+": The test \"Mess\" of the module \"SystemTests\" for archiving messages",
			SYS.Archive[nList[iN]]["mess_"+nNm1] ? SYS.Special.SystemTests.Mess(nList[iN].slice(4)+"."+nNm1,"",600) : "missed message archiver ''"+nNm1+"''");

		//Assign/Clear archiving from the parameter attribute
		nNm1 = "/DAQ/LogicLev/experiment/F3";
		testREZ = "missed DAQ parameter ''"+nNm1+"''";
		if(SYS.nodeAt(nNm1)) {
			testREZ = "PASSED";
			SYS.cntrReq(SYS.XMLNode("set").setAttr("path",nNm1+"/%2farch%2farch").setAttr("key_atr","ed").setAttr("col",nList[iN].slice(4)+".1s").setText("1"));
			if(!SYS.nodeAt("/Archive/va_F3_ed"))	testREZ = "error assigning";
			else {
				SYS.cntrReq(SYS.XMLNode("set").setAttr("path",nNm1+"/%2farch%2farch").setAttr("key_atr","ed").setAttr("col","prc").setText("0"));
				if(SYS.nodeAt("/Archive/va_F3_ed"))	testREZ = "error clearing";
			}
		}
		result("Archive."+nList[iN].slice(4)+": Assign/Clear archiving from the parameter attribute", testREZ);

		//The tests "Archive" of the value archiver to verify the correctness of the functioning of the sequential mechanism for packaging by "SystemTests".
		if(nList[iN].slice(4) == "FSArch") {
			nNm1 = "testArch_cntr";
			result("Archive."+nList[iN].slice(4)+": The tests \"Archive\" of the module \"SystemTests\" for the value archiver to verify the correctness of the functioning of the sequential packaging mechanism",
				SYS.nodeAt("/Archive/va_"+nNm1) ? SYS.Special.SystemTests.Archive(nNm1,1e6,nList[iN].slice(4)+".1s") : "missed archive "+nNm1);
		}

		//Disconnect/Connect module
		req = SYS.XMLNode("get").setAttr("path","/Archive/"+nList[iN]+"/%2fmodule%2fm_inf%2fSource");
		SYS.cntrReq(req);
		if(req.text()[0] != "*") {
			testREZ = SYS.Special.SystemTests.SOAttach(req.text(),-1);
			testREZ += "/"+SYS.Special.SystemTests.SOAttach(req.text(),1);
			result("Archive."+nList[iN].slice(4)+": Disconnect/Connect", testREZ);
		} else result("Archive."+nList[iN].slice(4)+": Disconnect/Connect", "Built-in");
	}
	result("Archive: Time of the execution, seconds", (SYS.utime()-stTm)*1e-6);
}

//**********************************
//DB subsystem tests
if(sub < 0 || sub == 5) {
	stTm = SYS.utime();
	nList = SYS.BD.nodeList("mod_");
	for(iN = 0; iN < nList.length; iN++) {
		//Disconnect/Connect module
		req = SYS.XMLNode("get").setAttr("path","/BD/"+nList[iN]+"/%2fmodule%2fm_inf%2fSource");
		SYS.cntrReq(req);
		if(req.text()[0] != "*") {
			testREZ = SYS.Special.SystemTests.SOAttach(req.text(),-1);
			testREZ += "/"+SYS.Special.SystemTests.SOAttach(req.text(),1);
			result("DB."+nList[iN].slice(4)+": Disconnect/Connect", testREZ);
		} else result("DB."+nList[iN].slice(4)+": Disconnect/Connect", "Built-in");

		//DB object
		testREZ = "PASSED";
		nAddr = "/BD/"+nList[iN]+"/db_"+nNm;
		if(SYS.cntrReq(SYS.XMLNode("add").setAttr("path","/BD/"+nList[iN]+"/%2fbr%2fdb_").setAttr("id",nNm)) != 0 || !(node=SYS.nodeAt(nAddr))) testREZ = "FAILED: add";
		else if(SYS.cntrReq(SYS.XMLNode("set").setAttr("path",nAddr+"/%2fprm%2fcfg%2fDESCR").setText("Test")) != 0 || node.cfg("DESCR") != "Test") testREZ = "FAILED: change";
		else if(SYS.cntrReq(SYS.XMLNode("save").setAttr("path",nAddr+"/%2fobj")) != 0) testREZ = "FAILED: save";
		else if(SYS.cntrReq(SYS.XMLNode("set").setAttr("path",nAddr+"/%2fprm%2fcfg%2fDESCR").setText("Test 1")) != 0 || 
			SYS.cntrReq(SYS.XMLNode("load").setAttr("path",nAddr+"/%2fobj")) != 0 || node.cfg("DESCR") != "Test") testREZ = "FAILED: load";
		else {
			node = false;
			if(SYS.cntrReq(SYS.XMLNode("del").setAttr("path","/BD/"+nList[iN]+"/%2fbr%2fdb_").setAttr("id",nNm)) != 0 || SYS.nodeAt(nAddr)) testREZ = "FAILED: delete";
			else {
				cfgTree.getElementBy("DB").childAdd("fld").setAttr("ID",nNm+"_cfg").setAttr("TYPE",nList[iN].slice(4)).setAttr("EN",0);
				cfgTree.save(0x20, configPath);
				SYS.cntrReq(SYS.XMLNode("scan").setAttr("path","/%2fgen%2fconfig"));		//Reload config command
				if(!SYS.nodeAt(nAddr+"_cfg")) testREZ = "FAILED: load from config";
			}
		}
		result("DB."+nList[iN].slice(4)+": create/remove, change, load, save and load from config", testREZ);

		//The tests "BD" of the module "SystemTests"
		nNm1 = "/BD/"+nList[iN]+"/testRelease";
		testREZ = "missed DB ''"+nNm1+"''";
		if((dbO=SYS.nodeAt(nNm1))) {
			testREZ = "inaccessible DB ''"+nNm1+"''";
			if(SYS.cntrReq(SYS.XMLNode("set").setAttr("path",nNm1+"/%2fprm%2fst%2fst").setText("1")) == 0) {
				SYS.cntrReq(SYS.XMLNode("set").setAttr("path",nNm1+"/%2fprm%2fst%2fst").setText("0"));
				testREZ = SYS.Special.SystemTests.DB(nList[iN].slice(4), SYS.nodeAt(nNm1).cfg("ADDR"), "test", 1000);
			}
		}
		result("DB."+nList[iN].slice(4)+": The tests \"BD\" of the module \"SystemTests\"", testREZ);
	}
	result("DB: Time of the execution, seconds", (SYS.utime()-stTm)*1e-6);
}

//**********************************
//DAQ subsystem tests
if(sub < 0 || sub == 6) {
	stTm = SYS.utime();
	// Templates
	testREZ = "PASSED";
	nAddr = "/DAQ/tmplb_"+nNm;
	if(SYS.cntrReq(SYS.XMLNode("add").setAttr("path","/DAQ/%2fbr%2ftmplb_").setAttr("id",nNm)) != 0 || !(node=SYS.nodeAt(nAddr))) testREZ = "FAILED: template library add";
	else if(SYS.cntrReq(SYS.XMLNode("set").setAttr("path",nAddr+"/%2flib%2fcfg%2fDESCR").setText("Test")) != 0 || node.cfg("DESCR") != "Test") testREZ = "FAILED: change template library";
	else if(SYS.cntrReq(SYS.XMLNode("save").setAttr("path",nAddr+"/%2fobj")) != 0) testREZ = "FAILED: save template library";
	else if(SYS.cntrReq(SYS.XMLNode("set").setAttr("path",nAddr+"/%2flib%2fcfg%2fDESCR").setText("Test 1")) != 0 || 
		SYS.cntrReq(SYS.XMLNode("load").setAttr("path",nAddr+"/%2fobj")) != 0 || node.cfg("DESCR") != "Test") testREZ = "FAILED: load template library";
	else {
		node = false;

		// Template
		nAddrLb = nAddr+"/tmpl_"+nNm;
		if(SYS.cntrReq(SYS.XMLNode("add").setAttr("path",nAddr+"/%2fbr%2ftmpl_").setAttr("id",nNm)) != 0 || !(node=SYS.nodeAt(nAddrLb))) testREZ = "FAILED: template add";
		else if(SYS.cntrReq(SYS.XMLNode("set").setAttr("path",nAddrLb+"/%2ftmpl%2fcfg%2fDESCR").setText("Test")) != 0 || node.cfg("DESCR") != "Test") testREZ = "FAILED: change template";
		else if(SYS.cntrReq(SYS.XMLNode("save").setAttr("path",nAddrLb+"/%2fobj")) != 0) testREZ = "FAILED: save template";
		else if(SYS.cntrReq(SYS.XMLNode("set").setAttr("path",nAddrLb+"/%2ftmpl%2fcfg%2fDESCR").setText("Test 1")) != 0 || 
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
	result("DAQ: Templates library create/remove, change, load, save and load from config", testREZ);

	// Modules
	nList = SYS.DAQ.nodeList("mod_");
	for(iN = 0; iN < nList.length; iN++) {
		testREZ = "PASSED";
		testREZ_ = "PASSED";
		//  Controller
		nAddr = "/DAQ/"+nList[iN]+"/cntr_"+nNm;
		if(SYS.cntrReq(SYS.XMLNode("add").setAttr("path","/DAQ/"+nList[iN]+"/%2fbr%2fcntr_").setAttr("id",nNm)) != 0 || !(node=SYS.nodeAt(nAddr))) testREZ = "FAILED: controller add";
		else if(SYS.cntrReq(SYS.XMLNode("set").setAttr("path",nAddr+"/%2fcntr%2fcfg%2fDESCR").setText("Test")) != 0 || node.cfg("DESCR") != "Test") testREZ = "FAILED: controller change";
		else if(SYS.cntrReq(SYS.XMLNode("save").setAttr("path",nAddr+"/%2fobj")) != 0) testREZ = "FAILED: controller save";
		else if(SYS.cntrReq(SYS.XMLNode("set").setAttr("path",nAddr+"/%2fcntr%2fcfg%2fDESCR").setText("Test 1")) != 0 || 
			SYS.cntrReq(SYS.XMLNode("load").setAttr("path",nAddr+"/%2fobj")) != 0 || node.cfg("DESCR") != "Test") testREZ = "FAILED: controller load";
		else {
			node = false;

			// Parameter
			if(nList[iN] != "mod_DAQGate") {
				nAddr1 = nAddr+"/prm_"+nNm;
				if(SYS.cntrReq(SYS.XMLNode("add").setAttr("path",nAddr+"/%2fbr%2fprm_").setAttr("id",nNm)) != 0 || !(node=SYS.nodeAt(nAddr1))) testREZ = "FAILED: parameter add";
				else if(SYS.cntrReq(SYS.XMLNode("set").setAttr("path",nAddr1+"/%2fprm%2fcfg%2fDESCR").setText("Test")) != 0 || node.cfg("DESCR") != "Test") testREZ = "FAILED: parameter change";
				else if(SYS.cntrReq(SYS.XMLNode("save").setAttr("path",nAddr1+"/%2fobj")) != 0) testREZ = "FAILED: parameter save";
				else if(SYS.cntrReq(SYS.XMLNode("set").setAttr("path",nAddr1+"/%2fprm%2fcfg%2fDESCR").setText("Test 1")) != 0 || 
					SYS.cntrReq(SYS.XMLNode("load").setAttr("path",nAddr1+"/%2fobj")) != 0 || node.cfg("DESCR") != "Test") testREZ = "FAILED: parameter load";
				node = false;
			}
			if(nList[iN] == "mod_BlockCalc") {
				nAddr1 = nAddr+"/blk_"+nNm;
				if(SYS.cntrReq(SYS.XMLNode("add").setAttr("path",nAddr+"/%2fbr%2fblk_").setAttr("id",nNm)) != 0 || !(node=SYS.nodeAt(nAddr1))) testREZ_ = "FAILED: block add";
				else if(SYS.cntrReq(SYS.XMLNode("set").setAttr("path",nAddr1+"/%2fblck%2fcfg%2fDESCR").setText("Test")) != 0 || node.cfg("DESCR") != "Test") testREZ_ = "FAILED: block change";
				else if(SYS.cntrReq(SYS.XMLNode("save").setAttr("path",nAddr1+"/%2fobj")) != 0) testREZ_ = "FAILED: block save";
				else if(SYS.cntrReq(SYS.XMLNode("set").setAttr("path",nAddr1+"/%2fblck%2fcfg%2fDESCR").setText("Test 1")) != 0 || 
					SYS.cntrReq(SYS.XMLNode("load").setAttr("path",nAddr1+"/%2fobj")) != 0 || node.cfg("DESCR") != "Test") testREZ_ = "FAILED: block load";
				else if(SYS.cntrReq((req=SYS.XMLNode("copy").setAttr("path","/%2fobj").setAttr("src","/DAQ/"+nList[iN]+"/cntr_KM102cntr").setAttr("dst","/DAQ/"+nList[iN]+"/cntr_KM102cntr_copy"))) != 0 || req.attr("rez").toInt())
					testREZ_ = "FAILED: block scheme copy: "+req.text();
				else if(SYS.cntrReq((req=SYS.XMLNode("set").setAttr("path","/DAQ/BlockCalc/KM102cntr_copy/blk_КШ111/%2flnk%2fio%2f1|t_full").setText("3"))) != 0 || req.attr("rez").toInt())
					testREZ_ = "FAILED: block scheme link type: "+req.text();
				else if(SYS.cntrReq((req=SYS.XMLNode("set").setAttr("path","/DAQ/BlockCalc/KM102cntr_copy/blk_КШ111/%2flnk%2fio%2f2|t_full").setText("LogicLev.experiment.F3.var"))) != 0 || req.attr("rez").toInt())
					testREZ_ = "FAILED: block scheme link value: "+req.text();
				node = false;
			}
			else if(nList[iN] == "mod_JavaLikeCalc") {
				nAddr_ = "/DAQ/"+nList[iN]+"/lib_"+nNm;
				if(SYS.cntrReq(SYS.XMLNode("add").setAttr("path","/DAQ/"+nList[iN]+"/%2fbr%2flib_").setAttr("id",nNm)) != 0 || !(node=SYS.nodeAt(nAddr_))) testREZ_ = "FAILED: library add";
				else if(SYS.cntrReq(SYS.XMLNode("set").setAttr("path",nAddr_+"/%2flib%2fcfg%2fDESCR").setText("Test")) != 0 || node.cfg("DESCR") != "Test") testREZ_ = "FAILED: library change";
				else if(SYS.cntrReq(SYS.XMLNode("save").setAttr("path",nAddr_+"/%2fobj")) != 0) testREZ_ = "FAILED: library save";
				else if(SYS.cntrReq(SYS.XMLNode("set").setAttr("path",nAddr_+"/%2flib%2fcfg%2fDESCR").setText("Test 1")) != 0 || 
					SYS.cntrReq(SYS.XMLNode("load").setAttr("path",nAddr_+"/%2fobj")) != 0 || node.cfg("DESCR") != "Test") testREZ_ = "FAILED: library load";
				else {
					node = false;

					// Function
					nAddr_1 = nAddr_+"/fnc_"+nNm;
					if(SYS.cntrReq(SYS.XMLNode("add").setAttr("path",nAddr_+"/%2fbr%2ffnc_").setAttr("id",nNm)) != 0 || !(node=SYS.nodeAt(nAddr_1))) testREZ_ = "FAILED: function add";
					else if(SYS.cntrReq(SYS.XMLNode("set").setAttr("path",nAddr_1+"/%2ffunc%2fcfg%2fDESCR").setText("Test")) != 0 || node.cfg("DESCR") != "Test") testREZ_ = "FAILED: function change";
					else if(SYS.cntrReq(SYS.XMLNode("save").setAttr("path",nAddr_1+"/%2fobj")) != 0) testREZ_ = "FAILED: function save";
					else if(SYS.cntrReq(SYS.XMLNode("set").setAttr("path",nAddr_1+"/%2ffunc%2fcfg%2fDESCR").setText("Test 1")) != 0 || 
						SYS.cntrReq(SYS.XMLNode("load").setAttr("path",nAddr_1+"/%2fobj")) != 0 || node.cfg("DESCR") != "Test") testREZ_ = "FAILED: function load";
					else if(SYS.cntrReq((req=SYS.XMLNode("copy").setAttr("path","/%2fobj").setAttr("src",nAddr_1).setAttr("dst",nAddr_1+"_copy"))) != 0 || req.attr("rez").toInt())
						testREZ_ = "FAILED: function copy: "+req.text();
					node = false;
					if(SYS.cntrReq(SYS.XMLNode("del").setAttr("path","/DAQ/"+nList[iN]+"/%2fbr%2flib_").setAttr("id",nNm)) != 0 || SYS.nodeAt(nAddr_)) testREZ_ = "FAILED: function delete";
				}			
			}

			if(SYS.cntrReq(SYS.XMLNode("del").setAttr("path","/DAQ/"+nList[iN]+"/%2fbr%2fcntr_").setAttr("id",nNm)) != 0 || SYS.nodeAt(nAddr)) testREZ = "FAILED: controller delete";
			else {
				treeNd = cfgTree.getElementBy("sub_DAQ");
				if(!treeNd.getElementBy(nList[iN])) treeNd.childAdd("node").setAttr("id",nList[iN]);
				treeNd = treeNd.getElementBy(nList[iN]);

				treeNd_t = treeNd.childAdd("tbl").setAttr("id","DAQ").childAdd("fld").setAttr("ID",nNm+"_cfg").setAttr("PRM_BD",nNm+"_cfg_prm").setAttr("ENABLE",1);
				if(nList[iN] == "mod_JavaLikeCalc") treeNd_t.setAttr("FUNC","servProc.fnc_crc16");
				treeNd.childAdd("tbl").setAttr("id",nNm+"_cfg_prm").childAdd("fld").setAttr("SHIFR",nNm+"_cfg");
				if(nList[iN] == "mod_BlockCalc") {
					treeNd_t.setAttr("BLOCK_SH",nNm+"_cfg_blcks");
					treeNd.childAdd("tbl").setAttr("id",nNm+"_cfg_blcks").childAdd("fld").setAttr("ID",nNm+"_cfg");
				}
				if(nList[iN] == "mod_JavaLikeCalc") {
					treeNd.childAdd("tbl").setAttr("id","lib").childAdd("fld").setAttr("ID",nNm+"_cfg").setAttr("DB","lib_"+nNm+"_cfg");
					treeNd.childAdd("tbl").setAttr("id","lib_"+nNm+"_cfg").childAdd("fld").setAttr("ID",nNm+"_cfg");
				}

				cfgTree.save(0x20, configPath);
				SYS.cntrReq(SYS.XMLNode("scan").setAttr("path","/%2fgen%2fconfig"));		//Reload config command
				SYS.nodeAt(nAddr+"_cfg").enable(true);
				if(!SYS.nodeAt(nAddr+"_cfg")) testREZ = "FAILED: load controller from config";
				else {
					if(!SYS.nodeAt(nAddr+"_cfg/prm_"+nNm+"_cfg")) testREZ = "FAILED: load parameter from config. The field of the parameters table seems is not PRM_BD.";
					if(nList[iN] == "mod_BlockCalc" && !SYS.nodeAt(nAddr+"_cfg/blk_"+nNm+"_cfg")) testREZ_ = "FAILED: load block from config";
					if(nList[iN] == "mod_JavaLikeCalc") {
						if(!SYS.nodeAt(nAddr_+"_cfg")) testREZ_ = "FAILED: load library from config";
						else if(!SYS.nodeAt(nAddr_+"_cfg/fnc_"+nNm+"_cfg")) testREZ_ = "FAILED: load function from config";
					}
				}
			}
		}
		result("DAQ."+nList[iN].slice(4)+": Controller object, parameter and other main objects create/remove, change, load, save and load from config", testREZ);
		if(nList[iN] == "mod_BlockCalc")
			result("DAQ."+nList[iN].slice(4)+": Blocks create/remove, change, load, save and load from config and copy, set link", testREZ_);
		else if(nList[iN] == "mod_JavaLikeCalc")
			result("DAQ."+nList[iN].slice(4)+": Functions and its libraries create/remove, change, load, save and load from config and function copy", testREZ_);
		else if(nList[iN] == "mod_LogicLev") {
			result("DAQ."+nList[iN].slice(4)+": Test \"Param\" from the module \"SystemTests\"",
				SYS.Special.SystemTests.Param("LogicLev.experiment.F3"));
			result("DAQ."+nList[iN].slice(4)+": Test \"Val\" of the module \"SystemTests\" for the parameter attribute",
				SYS.Special.SystemTests.Val("LogicLev.experiment.F3.var",100,1e6));
		}

		//Disconnect/Connect module
		if(nList[iN] != "mod_JavaLikeCalc") {
			req = SYS.XMLNode("get").setAttr("path","/DAQ/"+nList[iN]+"/%2fmodule%2fm_inf%2fSource");
			SYS.cntrReq(req);
			if(req.text()[0] != "*") {
				testREZ = SYS.Special.SystemTests.SOAttach(req.text(),-1);
				testREZ += "/"+SYS.Special.SystemTests.SOAttach(req.text(),1);
				result("DAQ."+nList[iN].slice(4)+": Disconnect/Connect", testREZ);
			} else result("DAQ."+nList[iN].slice(4)+": Disconnect/Connect", "Built-in");
		}
	}
	result("DAQ: Time of the execution, seconds", (SYS.utime()-stTm)*1e-6);
}

//**********************************
//UI subsystem tests
if(sub < 0 || sub == 7) {
	stTm = SYS.utime();
	nList = SYS.UI.nodeList("mod_");
	for(iN = 0; iN < nList.length; iN++) {
		if(nList[iN] == "mod_QTStarter" || nList[iN] == "mod_QTCfg") continue;

		//UI.VCAEngine tests
		if(nList[iN] == "mod_VCAEngine") {
			// Widgets library
			testREZ = "PASSED";
			nAddr = "/UI/"+nList[iN]+"/wlb_"+nNm;
			if(SYS.cntrReq(SYS.XMLNode("add").setAttr("path","/UI/"+nList[iN]+"/%2fbr%2fwlb_").setAttr("id",nNm)) || !(node=SYS.nodeAt(nAddr))) testREZ = "FAILED: widgets library add";
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

				if(SYS.cntrReq(SYS.XMLNode("del").setAttr("path","/UI/"+nList[iN]+"/%2fbr%2fwlb_").setAttr("id",nNm)) != 0 || SYS.nodeAt(nAddr)) testREZ = "FAILED: widgets library delete";
				else {
					cfgTree.getElementBy("mod_VCAEngine").childAdd("tbl").setAttr("id","LIB").childAdd("fld").setAttr("ID",nNm+"_cfg").setAttr("DB_TBL","wlib_"+nNm+"_cfg");
					cfgTree.getElementBy("mod_VCAEngine").childAdd("tbl").setAttr("id","wlib_"+nNm+"_cfg").childAdd("fld").setAttr("ID",nNm+"_cfg").setAttr("PARENT","/wlb_originals/wdg_Box");
					cfgTree.save(0x20, configPath);
					SYS.cntrReq(SYS.XMLNode("scan").setAttr("path","/%2fgen%2fconfig"));		//Reload config command
					if(!SYS.nodeAt(nAddr+"_cfg") || !SYS.nodeAt(nAddr+"_cfg/wdg_"+nNm+"_cfg")) testREZ = "FAILED: load widgets library from config";
				}
			}
			result("UI.VCAEngine: Widgets and widgets'' library create/remove, change, load, save and load from config", testREZ);

			// Project
			testREZ = "PASSED";
			nAddr = "/UI/"+nList[iN]+"/prj_"+nNm;
			if(SYS.cntrReq(SYS.XMLNode("add").setAttr("path","/UI/"+nList[iN]+"/%2fbr%2fprj_").setAttr("id",nNm)) || !(node=SYS.nodeAt(nAddr))) testREZ = "FAILED: project add";
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

				if(SYS.cntrReq(SYS.XMLNode("del").setAttr("path","/UI/"+nList[iN]+"/%2fbr%2fprj_").setAttr("id",nNm)) != 0 || SYS.nodeAt(nAddr)) testREZ = "FAILED: project delete";
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
			result("UI.VCAEngine: Project, pages and widgets create/remove, change, load, save and load from config", testREZ);

			//Session
			testREZ = "PASSED";
			nAddr = "/UI/"+nList[iN]+"/ses_"+nNm;
			req = SYS.XMLNode("CntrReqs").setAttr("path","/UI/"+nList[iN]);
			req.childAdd("add").setAttr("path","/%2fbr%2fses_").setText(nNm);
			req.childAdd("set").setAttr("path","/ses_"+nNm+"/%2fobj%2fst%2fprj").setText("tmplSO");
			req.childAdd("set").setAttr("path","/ses_"+nNm+"/%2fobj%2fst%2fstart").setText(1);
			if(SYS.cntrReq(req) != 0 || !SYS.nodeAt(nAddr)) testREZ = "FAILED: session add/start";
			else if(SYS.cntrReq(req=SYS.XMLNode("del").setAttr("path","/UI/"+nList[iN]+"/%2fbr%2fses_").setText(nNm)) || SYS.nodeAt(nAddr)) testREZ = "FAILED: session delete";
			result("UI.VCAEngine: Session create, start and remove", testREZ);
		}

		//Disconnect/Connect module
		req = SYS.XMLNode("get").setAttr("path","/UI/"+nList[iN]+"/%2fmodule%2fm_inf%2fSource");
		SYS.cntrReq(req);
		if(req.text()[0] != "*") {
			testREZ = SYS.Special.SystemTests.SOAttach(req.text(),-1);
			testREZ += "/"+SYS.Special.SystemTests.SOAttach(req.text(),1);
			result("UI."+nList[iN].slice(4)+": Disconnect/Connect", testREZ);
		} else result("UI."+nList[iN].slice(4)+": Disconnect/Connect", "Built-in");
	}
	result("UI: Time of the execution, seconds", (SYS.utime()-stTm)*1e-6);
}

//**********************************
//Special subsystem tests
if(sub < 0 || sub == 8) {
	stTm = SYS.utime();
	nList = SYS.Special.nodeList("mod_");
	for(iN = 0; iN < nList.length; iN++) {
		if(nList[iN] == "mod_SystemTests") continue;
		//Disconnect/Connect module
		req = SYS.XMLNode("get").setAttr("path","/Special/"+nList[iN]+"/%2fmodule%2fm_inf%2fSource");
		SYS.cntrReq(req);
		if(req.text()[0] != "*") {
			testREZ = SYS.Special.SystemTests.SOAttach(req.text(),-1);
			testREZ += "/"+SYS.Special.SystemTests.SOAttach(req.text(),1);
			result("Special."+nList[iN].slice(4)+": Disconnect/Connect", testREZ);
		} else result("Special."+nList[iN].slice(4)+": Disconnect/Connect", "Built-in");
	}
	result("Special: Time of the execution, seconds", (SYS.utime()-stTm)*1e-6);
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
	rezF+
	"</TABLE>\n"
	"</body>"
	"</html>");','','',1560097197);
INSERT INTO lib_servProc VALUES('archPackFStests','TEST: Packing of the archiver on FS','','','Tests set for packing of the value archives to File System, to check the optimization algorithm. The stages are provided:
  - Stage 0: Filling the main values: "1,1,10,10,100,100,1000,1000,10000,10000,EVAL"
  - Stage 1: Changing to different value 20: "1,1,20,10,100,100,1000,1000,10000,10000,EVAL"
  - Stage 2: Setting a value for merging (equal) to the up (10): "1,1,20,10,10,100,1000,1000,10000,10000,EVAL"
  - Stage 3: Setting a value for merging (equal) to the down (1000): "1,1,20,10,10,1000,1000,1000,10000,10000,EVAL"
  - Stage 4: Setting a different value to the end 20000: "1,1,20,10,10,1000,1000,1000,10000,10000,20000,EVAL"
  - Stage 5: Changing the end value for merging to the up (10000): "1,1,20,10,10,1000,1000,1000,10000,10000,10000,EVAL"
  - Stage 6: Setting a value to the end for merging to the up (10000): "1,1,20,10,10,1000,1000,1000,10000,10000,10000,10000,EVAL"
  - Stage 7: Setting the end value to EVAL: "1,1,20,10,10,1000,1000,1000,10000,10000,10000,EVAL"
  - Stage 8: Setting an equal value for inserting 10000: "1,1,20,10,10,1000,1000,1000,10000,10000,10000,EVAL"

Author: Roman Savochenko <roman@oscada.org>
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
}','','',1560097170);
INSERT INTO lib_servProc VALUES('wacko2media','EXT: WackoWiki to MediaWiki','','','Procedure of conversion Wiki-dialect from WackoWiki to MediaWiki. Used to move OpenSCADA Wiki to MediaWiki.

Author: Roman Savochenko <roman@oscada.org>
Version: 1.1.0','','',1,10,0,'dbO = SYS.BD.MySQL[ndb];
if(!dbO)	return "Error: DB ''"+ndb+"'' missed!";
DBTbl = dbO.SQLReq("SELECT body FROM `wk_pages` WHERE `tag`=\""+wpg+"\";");
if(DBTbl.err.length) return "Error: "+DBTbl.err;
else if(DBTbl.length < 2 || !DBTbl[1].length)	return "Error: Page ''"+wpg+"'' is not found!"; 
ibuf = DBTbl[1][0];
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
		else {
			tblInRow = false;
			//  Checking for the table header
			if((stLineOut=obuf.lastIndexOf("\n")) < 0)	stLineOut = 0;
			obuf = obuf.slice(0, stLineOut) + obuf.slice(stLineOut).replace(new RegExp("\\|\\s*''''''(.+?)''''''\\s*\\|","g"),"! $1 !").replace(new RegExp("\\|\\s*''''''(.+?)''''''\\s*$","m"),"! $1");
		}
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

return obuf;','','',1560098890);
INSERT INTO lib_servProc VALUES('docOffLine','EXT: Off-line documentation','','','Procedure of forming the offline documentation of OpenSCADA from Wiki by reading and processing the specified Wiki pages on different languages, currently it is: English, Ukrainian, Russian.

Author: Roman Savochenko <roman@oscada.org>
Version: 1.2.0','','',1,360,0,'trNm = "offLine";
docHost = "oscada.org:80";
docHost_ = "http://" + docHost.parse(0,":");
defLang = "en";

SYS.cntrReq((req=SYS.XMLNode("get").setAttr("path","/%2fgen%2fprog")));
prgNm = req.text();
if(!prgVer.length) {
	SYS.cntrReq((req=SYS.XMLNode("get").setAttr("path","/%2fgen%2fver")));
	prgVer = req.text();
}
docData = SYS.strftime(SYS.time(), "%B %Y");

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
				SYS.system("if test ! -d "+resDir+tVl2+"; then install -d "+resDir+tVl2+"/files; fi", true);
				//SYS.system("if test ! -d "+resDir+tVl2+"; then install -d "+resDir+tVl2+"/files; cp "+resDir+"en/files/doc.css "+resDir+tVl2+"/files; fi", true);
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
					SYS.system("if test ! -d "+resDir+tVl2+"; then install -d "+resDir+tVl2+"/files; fi", true);
					//SYS.system("if test ! -d "+resDir+tVl2+"; then install -d "+resDir+tVl2+"/files; cp "+resDir+"en/files/doc.css "+resDir+tVl2+"/files; fi", true);
				dirsO[tVl2] = true; lngCnt++;
			}
			pgsOprc[pWiki+"/"+tVl2] = pOffLn;
		}
	}
} else pgsOprc = pgsO;

//Pages fetching and processing
isError = false;
for(var ip in pgsOprc) {
	pLang = ip.lastIndexOf("/"); pWiki = ip.slice(0, pLang); pLang = ip.slice(pLang+1);
	pOffLn = pgsO[ip];
	if(isError)	{ pagesCur += pWiki+":"+pLang+":"+pOffLn+"\n"; continue; }

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
	req = SYS.XMLNode("GET").setAttr("Host",docHost).setAttr("URI","/wiki/index.php?title="+ip.replace("/"+defLang,"")+"&printable=yes");
	// Load the documentation page to a XML tree
	stTm = SYS.mtime();
	for(iTr = 0; iTr < reqTr; iTr++)
		if(!(rez=tr.messIO(req,"HTTP")).length && req.text().length) break;
		else tr.start(false);
	//if(req.text().length < 10)
	//	SYS.messInfo("OffLine", "TEST 00: rez="+rez+"; length="+req.text().length);
	if(iTr == reqTr)	{
		res = "1: Page "+ip+" load error in "+reqTr+" tries and "+1e-3*(SYS.mtime()-stTm)+" seconds";
		isError = true; pagesCur = pWiki+":"+pLang+":"+pOffLn+"\n";
		continue;
	}

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
				(chO.name() == "meta" && chO.attr("name") != "generator" && !chO.attr("charset").length) ||
				(chO.name() == "link" && chO.attr("rel") != "copyright"))	{
			head.childDel(iEl);
			iEl--;
		}
		else passPrev = true;
	}
	head.childAdd("link").setAttr("rel","stylesheet").setAttr("href",topDir+((pLang!=defLang)?"../"+defLang+"/":"")+"files/doc.css");

	// BODY processing
	body = docTree.childGet("body");
	header = body.getElementBy("mw_header");
	cnt = body.getElementBy("mw-content-text");
	body.clear(true);
	body.childAdd(header); body.childAdd(cnt);

	// Languages area processing
	if((lngSelAr=body.getElementBy("mw-pt-languages noprint","class"))) {
		lngSelAr.childDel(0);
		lngSelAr.setAttr("class", "mw-pt-languages");
		anchs = lngSelAr.getElementsBy("a");
		for(iEl = 0; iEl < anchs.length; iEl++) {
			href = anchs[iEl].attr("href");
			tVl = href.slice(href.lastIndexOf("/")+1);
			anchs[iEl].setAttr("href", topDir+"../"+(dirsO[tVl] == true?tVl:defLang)+"/"+pOffLn);
		}
	}

	// Images processing
	imgs = body.getElementsBy("img");
	for(iEl = 0; iEl < imgs.length && !isError; iEl++)
		if((tVl=imgs[iEl].attr("src").match("^/.*/(.*)$")).length) {
			isDef = false;
			if(imgsO[pLang+":"+tVl[1]] != true && (pLang == defLang || (isDef=imgsO[defLang+":"+tVl[1]]) != true)) {
				req = SYS.XMLNode("GET").setAttr("Host",docHost).setAttr("URI",imgs[iEl].attr("src"));

				stTm = SYS.mtime();
				for(iTr = 0; iTr < reqTr; iTr++)
					if(!(rez=tr.messIO(req,"HTTP")).length && req.text().length) break;
					else tr.start(false);
				//if(req.text().length < 10)
				//	SYS.messInfo("OffLine", "TEST 01: rez="+rez+"; length="+req.text().length);
				if(iTr == reqTr)	{
					res = "1: Image ''"+req.attr("URI")+"'' load error in "+reqTr+" tries and "+1e-3*(SYS.mtime()-stTm)+" seconds";
					isError = true; pagesCur = pWiki+":"+pLang+":"+pOffLn+"\n";
					break;
				}
				SYS.fileWrite(resDir+pLang+"/files/"+tVl[1], req.text());
				imgsO[pLang+":"+tVl[1]] = true;
				imgCnt++;
			}
			//SYS.messInfo("DOC", "pLang="+pLang+"; file="+tVl[1]+"; isP="+isP);
			imgs[iEl].setAttr("src", topDir+((isDef==true)?"../"+defLang+"/":"")+"files/"+tVl[1]);
		}
	if(isError)	continue;

	// Links processing
	anchs = body.getElementsBy("a");
	for(iEl = 0; iEl < anchs.length; iEl++) {
		href = anchs[iEl].attr("href");
		if(href[0] == "/") {
			if((tVl=href.lastIndexOf("#")) >= 0)	hTag = href.slice(tVl), href = href.slice(0, tVl);
			else hTag = "";

			dynLngPg = (href.indexOf("/wiki/Special:MyLanguage/") == 0) ? href.slice(25) : "";
			stLngPg = (!dynLngPg.length && href.indexOf("/wiki/") == 0) ? href.slice(6) : "";
			if(dynLngPg.length && (!(tDst=pgsO[dynLngPg+"/"+pLang]).isEVal() || !(tDst=pgsO[stLngPg]).isEVal()))
					anchs[iEl].setAttr("href", topDir+tDst+hTag);
			else if(dynLngPg.length && !(tDst=pgsO[dynLngPg+"/"+defLang]).isEVal())
					anchs[iEl].setAttr("href", topDir+"../"+defLang+"/"+tDst+hTag);
			else {
				anchs[iEl].setAttr("href", docHost_+href+hTag);
				if(!anchs[iEl].attr("class").length)	anchs[iEl].setAttr("class", "external");
			}
			lnkCnt++;
		}
	}

	// Appending the custom header and footer
	body.childIns(0, "div").load("<div class=''floatright''><a href=''"+docHost_+"''><img alt=''"+prgNm+"'' src=''"+topDir+"../en/files/Logo-big.png'' /></a></div>");
	body.childAdd("table").load("<table style=''border-top: dotted 2px #999999; margin-top: 20pt; color: gray;'' width=''100%''><tr>"
		"<td style=''text-align: left;'' width=''40%''><a href=''"+docHost_+"/wiki/"+pWiki+"/"+pLang+"''>"+pWiki+"/"+pLang+"</a> - <a href=''"+docHost_+"/en/main/about-the-project/licenses/''>GFDL</a></td>"
		"<td style=''text-align: center;''>"+docData+"</td>"
		"<td style=''text-align: right;'' width=''40%''>"+prgNm+" "+prgVer+"</td></tr></table>", false, 0x01);

	docTree.save(0x3C0, resDir+pLang+"/"+pOffLn);
	pCnt++;
	//SYS.messInfo("OffLine", "TEST 00: pLang="+pLang);
}

if(!isError)	pagesCur = "", res = "0: Fetched and processed pages="+pCnt+"; images="+imgCnt+"; links="+lnkCnt+"; languages="+lngCnt;','','',1579619800);
CREATE TABLE IF NOT EXISTS 'flb_regEl' ("ID" TEXT DEFAULT '' ,"NAME" TEXT DEFAULT '' ,"uk#NAME" TEXT DEFAULT '' ,"ru#NAME" TEXT DEFAULT '' ,"DESCR" TEXT DEFAULT '' ,"uk#DESCR" TEXT DEFAULT '' ,"ru#DESCR" TEXT DEFAULT '' ,"START" INTEGER DEFAULT '1' ,"MAXCALCTM" INTEGER DEFAULT '10' ,"PR_TR" INTEGER DEFAULT '0' ,"FORMULA" TEXT DEFAULT '' ,"uk#FORMULA" TEXT DEFAULT '' ,"ru#FORMULA" TEXT DEFAULT '' ,"TIMESTAMP" INTEGER DEFAULT '' , PRIMARY KEY ("ID"));
INSERT INTO flb_regEl VALUES('pidUnif','PID unified','ПІД уніфікований','ПИД унифицированный','Composite-unified analog and pulse PID. At the heart of the regulator is core a standard analog PID controller from the library FLibComplex1 and the implementation of the PWM for the pulse part.

Author: Roman Savochenko <roman@oscada.org>
Version: 1.0.0
License: GPLv2','Суміщений-уніфікований аналоговий та імпульсний ПІД-регулятор. У основі регулятора лежить ядро стандартного аналогового ПІД-регулятора з бібліотеки "FLibComplex1" та реалізація ШІМ для імпульсної частини.

Автор: Роман Савоченко <roman@oscada.org>
Версія: 1.0.0
Ліцензія: GPLv2','Совмещённый-унифицированный аналоговый и импульсный ПИД-регулятор. В основе регулятора лежит ядро стандартного аналогового ПИД-регулятора из библиотеки "FLibComplex1" и реализация ШИМ для импульсной части.

Автор: Роман Савоченко <roman@oscada.org>
Версия: 1.0.0
Лицензия: GPLv2',1,10,0,'//Call standard analog PID
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
return impAnOut;','','',1584883408);
INSERT INTO flb_regEl VALUES('pidUnifD','PID dynamic','ПІД динамічний','ПИД динамический','Completely identical to the unified PID-regulator, but it is implemented dynamically on JavaLikeCalc. The dynamic implementation allows you to easily adapt the regulator to the desired requirements, simply editing it.

Author: Roman Savochenko <roman@oscada.org>
Version: 1.0.0
License: GPLv2','Цілковито ідентичний уніфікованому ПІД-регулятору, але реалізований динамічно на JavaLikeCalc. Динамічна реалізація дозволяє легко адаптувати регулятор під потрібні вимоги, просто відредагувавши його.

Автор: Роман Савоченко <roman@oscada.org>
Версія: 1.0.0
Ліцензія: GPLv2','Полностью идентичный унифицированному ПИД-регулятору, но реализованный динамически на JavaLikeCalc. Динамическая реализация позволяет легко адаптировать регулятор под нужные требования, просто отредактировав его.

Автор: Роман Савоченко <roman@oscada.org>
Версия: 1.0.0
Лицензия: GPLv2',1,10,0,'if(f_start) {
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
return max(0, min(100,impAnOut));','','',1584883408);
INSERT INTO flb_regEl VALUES('pidImp','PID pulse','ПІД імпульсний','ПИД импульсный','Specialized pulse PID regulator is implemented on a special algorithm with compensation of double integration.

Author: Roman Savochenko <roman@oscada.org>
Version: 1.0.0
License: GPLv2','Спеціалізований імпульсний ПІД-регулятор реалізований за особливим алгоритмом із компенсацією подвійного інтегрування.

Автор: Роман Савоченко <roman@oscada.org>
Версія: 1.0.0
Ліцензія: GPLv2','Специализированный импульсный ПИД регулятор реализованный по особому алгоритму с компенсацией двойного интегрирования.

Автор: Роман Савоченко <roman@oscada.org>
Версия: 1.0.0
Лицензия: GPLv2',1,10,0,'//Call standard analog PID
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
return impAnOut;','','',1584883408);
CREATE TABLE IF NOT EXISTS 'flb_doc' ("ID" TEXT DEFAULT '' ,"NAME" TEXT DEFAULT '' ,"uk#NAME" TEXT DEFAULT '' ,"ru#NAME" TEXT DEFAULT '' ,"DESCR" TEXT DEFAULT '' ,"uk#DESCR" TEXT DEFAULT '' ,"ru#DESCR" TEXT DEFAULT '' ,"START" INTEGER DEFAULT '1' ,"MAXCALCTM" INTEGER DEFAULT '10' ,"PR_TR" INTEGER DEFAULT '0' ,"FORMULA" TEXT DEFAULT '' ,"uk#FORMULA" TEXT DEFAULT '' ,"ru#FORMULA" TEXT DEFAULT '' ,"TIMESTAMP" INTEGER DEFAULT '0' , PRIMARY KEY ("ID"));
INSERT INTO flb_doc VALUES('getVal','Getting value from the value archive','Отримання значення з архіву значень','Получение значения из архива значений','Query the value for the specified time (time:uTime) from the assigned archiver archtor (FSArch.1m) and issuing results with the specified number of digits prec after the dot.

The function works with the DAQ-parameter attribute objects to which the archive is attached and the address addr is written to it in full path, like "/DAQ/LogicLev/gen/prm_F3/a_var". For the empty archtor, the request will be made generally to everyone with the archive buffer periodicity!

In the process of obtaining a value, the time of this value may be different from the requested one, if the archive is not currently present and returns the closest value, preventing why you may by setting strong. When you need to get the closest valid value in a controlled way, for counters to example, you have the "tryTo" time argument to which to look for the closest valid value. The time in tryTo may be both lesser and bigger than the request time.

Initially, the result is presented as a string of real value with a certain number of digits prec after the dot, and with the value "Empty" for not valid values, allowing it to be inserted into the document without further processing. However, at requiring of additional processing and specific formatting, the result can be obtained with an unformatted real value, for prec less than 0.

Author: Roman Savochenko <roman@oscada.org>
Version: 1.1.2
License: GPLv2','Запит значення за визначений час (time:uTime) із пов''язаного архіватору archtor (FSArch.1m) та формування результату із визначеною кількістю знаків prec після коми.

Функція працює із об''єктами атрибутів DAQ-параметрів до яких підключено архів, та адреса addr до нього записується повним шляхом на кшталт "/DAQ/LogicLev/gen/prm_F3/a_var". Для порожнього archtor запит буде здійснюватися загально до усіх із періодичністю буферу архіву.

У процесі отримання значення, час цього значення може відрізнятися від запитаного, якщо архів на цей час відсутній та повертається найближче, запобігти чому можна встановивши strong. Коли-ж таки потрібно контрольовано отримати найближче достовірне значення, наприклад для лічильників, то передбачено аргумент часу tryTo до якого шукати найближче достовірне значення. Час у tryTo може бути як більше, так і менше за час запиту.

Первинно, результат надається у вигляді рядка реального значення із визначеною кількістю знаків prec після коми, та із значенням "Порожньо" для недостовірних значень, що дозволяє вставляти його до документу без додаткової обробки. Однак, за потреби додаткової обробки та специфічного форматування, результат можна отримати неформатованим реальним значенням, для prec менше 0.

Автор: Роман Савоченко <roman@oscada.org>
Версія: 1.1.2
Ліцензія: GPLv2','Запрос значения за определённое время (time:uTime) из связанного архиватора archtor (FSArch.1m) и формирование результата с определённым количеством знаков prec после запятой.

Функция работает с объектами атрибутов DAQ-параметров к которым подключён архив, и адрес addr к нему записывается полным путём вроде "/DAQ/LogicLev/gen/prm_F3/a_var". Для пустого archtor запрос будет осуществляться в целом ко всем с периодичностью буфера архива.

В процессе получения значения, время этого значения может отличаться от запрошенного, если архив на это время отсутствует и возвращается ближайшее, предотвратить что можно установив strong. Когда таки необходимо контролировано получить ближайшее достоверное значение, например для счётчиков, то предусмотрен аргумент времени tryTo до которого искать ближайшее достоверное значение. Время в tryTo может быть как больше, так и меньше времени запроса.

Первично, результат предоставляется в виде строки вещественного значения с определённым количеством знаков prec после запятой, и с значением "Пусто" для недостоверных значений, что позволяет вставлять его в документ без дополнительной обработки. Однако, при необходимости дополнительной обработки и специфического форматирования, результат можно получить неформатированным вещественным значением, для prec менше 0.

Автор: Роман Савоченко <roman@oscada.org>
Версия: 1.1.2
Лицензия: GPLv2',1,10,0,'srcTime = time*1e6 + uTime;
aO = SYS.nodeAt(addr).arch();
if(aO) {
	if(!tryTo || tryTo == srcTime)	val = aO.getVal(srcTime, false, archtor);
	else {
		if((aPer=aO.period(archtor)) == 0)	val = EVAL;
		else if(srcTime < tryTo)
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
return val;','','',1579609648);
INSERT INTO flb_doc VALUES('sumVal','Sum values of the value archive','Підсумок значень архіву значень','Сумма значений архива значений','Calculate the summary value over a specified period of the time [bTime...eTime], for the archiver archtor (FSArch.1m) data and issuing results with the specified number of digits prec after the dot.

The function works with the DAQ-parameter attribute objects to which the archive is attached and the address addr is written to it in full path, like "/DAQ/LogicLev/gen/prm_F3/a_var". For the empty archtor, the request will be made generally to everyone with the archive buffer periodicity!

In the process of calculation, the data in the archive is checked for validity.

Initially, the result is presented as a string of real value with a certain number of digits prec after the dot, and with the value "Empty" for not valid values, allowing it to be inserted into the document without further processing. However, at requiring of additional processing and specific formatting, the result can be obtained with an unformatted real value, for prec less than 0.

Author: Roman Savochenko <roman@oscada.org>
Version: 1.1.1
License: GPLv2','Обчислення суми значень за вказаний період часу [bTime...eTime], для даних архіватору archtor (FSArch.1m) та формування результату із визначеною кількістю знаків prec після коми.

Функція працює із об''єктами атрибутів DAQ-параметрів до яких підключено архів, та адреса addr до нього записується повним шляхом на кшталт "/DAQ/LogicLev/gen/prm_F3/a_var". Для порожнього archtor запит буде здійснюватися загально до усіх із періодичністю буферу архіву.

У процесі підрахунку здійснюється перевірка даних у архіві на предмет достовірності.

Первинно, результат надається у вигляді рядка реального значення із визначеною кількістю знаків prec після коми, та із значенням "Порожньо" для недостовірних значень, що дозволяє вставляти його до документу без додаткової обробки. Однак, за потреби додаткової обробки та специфічного форматування, результат можна отримати неформатованим реальним значенням, для prec менше 0.

Автор: Роман Савоченко <roman@oscada.org>
Версія: 1.1.1
Ліцензія: GPLv2','Вычисление суммы значений за указанный период времени [bTime...eTime], для данных архиватора archtor (FSArch.1m) и формирование результата с определённым количеством знаков prec после запятой.

Функция работает с объектами атрибутов DAQ-параметров к которым подключён архив, и адрес addr к нему записывается полным путём вроде "/DAQ/LogicLev/gen/prm_F3/a_var". Для пустого archtor запрос будет осуществляться в целом ко всем с периодичностью буфера архива.

В процессе расчёта осуществляется проверка данных в архиве на предмет достоверности.

Первично, результат предоставляется в виде строки вещественного значения с определённым количеством знаков prec после запятой, и с значением "Пусто" для недостоверных значений, что позволяет вставлять его в документ без дополнительной обработки. Однако, при необходимости дополнительной обработки и специфического форматирования, результат можно получить неформатированным вещественным значением, для prec менше 0.

Автор: Роман Савоченко <roman@oscada.org>
Версия: 1.1.1
Лицензия: GPLv2',1,10,0,'val = 0;
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
return val;','','',1579609896);
INSERT INTO flb_doc VALUES('averVal','Average value of the value archive','Середнє значення архіву значень','Среднее значение архива значений','Calculate the average value over a specified period of the time [bTime...eTime], for the archiver archtor (FSArch.1m) data and issuing results with the specified number of digits prec after the dot.

The function works with the DAQ-parameter attribute objects to which the archive is attached and the address addr is written to it in full path, like "/DAQ/LogicLev/gen/prm_F3/a_var". For the empty archtor, the request will be made generally to everyone with the archive buffer periodicity!

In the process of calculation, the data in the archive is checked for validity and the value of the percentage of reliable data actPerc that participated in the result is formed. In general, you should use intermediate archivers — medium quality archivers, so that there is not too much data in a given interval to reduce performance, and that it does not have one value at all.

Initially, the result is presented as a string of real value with a certain number of digits prec after the dot, and with the value "Empty" for not valid values, allowing it to be inserted into the document without further processing. However, at requiring of additional processing and specific formatting, the result can be obtained with an unformatted real value, for prec less than 0.

Author: Roman Savochenko <roman@oscada.org>
Version: 1.1.1
License: GPLv2','Обчислення середнього значення за вказаний період часу [bTime...eTime], для даних архіватору archtor (FSArch.1m) та формування результату із визначеною кількістю знаків prec після коми.

Функція працює із об''єктами атрибутів DAQ-параметрів до яких підключено архів, та адреса addr до нього записується повним шляхом на кшталт "/DAQ/LogicLev/gen/prm_F3/a_var". Для порожнього archtor запит буде здійснюватися загально до усіх із періодичністю буферу архіву.

У процесі підрахунку здійснюється перевірка даних у архіві на предмет достовірності та формується значення відсотку достовірних даних actPerc, що прийняли участь у отримані результату. Загалом, варто використовувати проміжні архіватори — архіватори середньої якості, щоб даних у ньому, за визначений інтервал, не було забагато для зниження продуктивності, та щоб це не було взагалі одне значення.

Первинно, результат надається у вигляді рядка реального значення із визначеною кількістю знаків prec після коми, та із значенням "Порожньо" для недостовірних значень, що дозволяє вставляти його до документу без додаткової обробки. Однак, за потреби додаткової обробки та специфічного форматування, результат можна отримати неформатованим реальним значенням, для prec менше 0.

Автор: Роман Савоченко <roman@oscada.org>
Версія: 1.1.1
Ліцензія: GPLv2','Вычисление среднего значения за указанный период времени [bTime...eTime], для данных архиватора archtor (FSArch.1m) и формирование результата с определённым количеством знаков prec после запятой.

Функция работает с объектами атрибутов DAQ-параметров к которым подключён архив, и адрес addr к нему записывается полным путём вроде "/DAQ/LogicLev/gen/prm_F3/a_var". Для пустого archtor запрос будет осуществляться в целом ко всем с периодичностью буфера архива.

В процессе расчёта осуществляется проверка данных в архиве на предмет достоверности и формируется значение процента достоверных данных actPerc, которые приняли участие в получении результата. В целом, стоит использовать промежуточные архиваторы — архиваторы среднего качества, чтобы данных в нём, за указанный интервал, не было очень много для понижения производительности, и чтобы это не было вообще одне значение.

Первично, результат предоставляется в виде строки вещественного значения с определённым количеством знаков prec после запятой, и с значением "Пусто" для недостоверных значений, что позволяет вставлять его в документ без дополнительной обработки. Однако, при необходимости дополнительной обработки и специфического форматирования, результат можно получить неформатированным вещественным значением, для prec менше 0.

Автор: Роман Савоченко <roman@oscada.org>
Версия: 1.1.1
Лицензия: GPLv2',1,10,0,'val = EVAL;
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
return val;','','',1579609632);
INSERT INTO flb_doc VALUES('diagram','Diagram','Діаграма','Диаграмма','Building trends diagram to SVG, which may be builtin next to the XHTML-document, for specified period of the time [(end-size)...end] and from the sources srcs.

The diagram forming code is based on code of the primitive "Diagram" of the visualisers, and which was quite simply migrated from C++ to the JavaLikeCalc embedded language. Currently, only trends have been moved-implemented!

The appearance of the diagram is determined by its dimensions hsz*vsz and the source SVG-template tmpl, a number attributes of the root tag of which has special meaning and corresponding to the primitive "Diagram":
- Scale: Markers: font (sclMarkFont), by default "Arial 10".
- Scale: horizontal (sclHor), by default "Markers (2)" and generally provided: "No draw (0)", "Grid (1)", "Markers (2)", "Grid and markers (3)".
- Scale: vertical (sclVer), by default "Grid and markers (3)" and generally provided: "No draw (0)", "Grid (1)", "Markers (2)", "Grid and markers (3)", "Grid (log) (5)", "Markers (log) (6)", "Grid and markers (log) (7)".
- Scale: color (sclColor), by default "gray".
- Scale: Markers: color (sclMarkColor), by default "gray".

Currently, only value archives can be used as a data source, either directly to them or through the DAQ-parameter attribute with the archive. The number of data sources is not directly limited and is written in separate lines of the argument srcs in the format "{addr}:{min}:{max}:{color}[:{scale}[:{width}]]", where:
- addr — address to the value archive or attributes of the DAQ-parameter, for example: "/Archive/va_CPULoad_load", "/DAQ/LogicLev/gen/F3/var";
- min, max — minimum and maximum of the scale, in the scale absence (min >= max), it will be calculated from the real data;
- color — colour of the trend curve;
- scale — separate definition of the trend scale, by default "Global (0)" and generally provided: "Global (0)", "Markers (2)", "Grid and markers (3)", "Markers (log) (6)", "Grid and markers (log) (7)".
- width — width of the trend curve, by default 2.

Author: Roman Savochenko <roman@oscada.org>
Sponsored by: Magomed, SAVTECH
Version: 1.4.0
License: GPLv2','Побудова діаграми трендів у SVG, яка може надалі вбудовуватися у XHTML-документу, для даних за вказаний період часу [(end-size)...end] та із джерел srcs.

Код формування діаграми засновано на коді примітиву "Діаграма" візуалізаторів, та який було доволі просто перенесено із мови C++ на вбудовану мову JavaLikeCalc. Наразі перенесено-реалізовано лише тренди!

Вигляд діаграми визначається її розмірами hsz*vsz та початковим SVG-шаблоном tmpl, низка атрибутів кореневого тегу якого мають особливе значення та які відповідають примітиву "Діаграма":
- Шкала: Маркери: шрифт (sclMarkFont), по замовченню "Arial 10".
- Шкала: горизонтальна (sclHor), по замовченню "Маркери (2)" та загалом передбачено: "Не зображувати (0)", "Ґратка (1)", "Маркери (2)", "Ґратка та маркери (3)".
- Шкала: вертикальна (sclVer), по замовченню "Ґратка та маркери (3)" та загалом передбачено: "Не зображувати (0)", "Ґратка (1)", "Маркери (2)", "Ґратка та маркери (3)", "Ґратка (лог) (5)", "Маркери (лог) (6)", "Ґратка та маркери (лог) (7)".
- Шкала: колір (sclColor), по замовченню "gray".
- Шкала: Маркери: колір (sclMarkColor), по замовченню "gray".

У якості джерела даних наразі можуть використовуватися лише архіви значень, прямою адресою до них або через атрибут DAQ-параметру з архівом. Кількість джерел даних прямо не обмежене та вони записуються окремими рядками аргументу srcs із форматом "{addr}:{min}:{max}:{color}[:{scale}[:{width}]]", де:
- addr — адреса до архіву значень або атрибуту DAQ-параметру, наприклад: "/Archive/va_CPULoad_load", "/DAQ/LogicLev/gen/F3/var";
- min, max — мінімум та максимум шкали, за відсутності шкали (min >= max) вона буде вираховуватися із реальних даних;
- color — колір кривої тренду;
- scale — окреме визначення шкали тренду, по замовченню "Глобально (0)" та загалом передбачено: "Глобально (0)", "Маркери (2)", "Ґратка та маркери (3)", "Маркери (лог) (6)", "Ґратка та маркери (лог) (7)".
- width — ширина кривої тренду, по замовченню 2.

Автор: Роман Савоченко <roman@oscada.org>
Спонсоровано: Магомед, SAVTECH
Версія: 1.4.0
Ліцензія: GPLv2','Построение диаграммы трендов в SVG, которая может далее встраиваться в XHTML-документ, для данных за указанный период времени [(end-size)...end] и из источников srcs.

Код формирования диаграммы основан на коде примитива "Диаграмма" визуализаторов, и который был довольно просто перенесен с языка C++ на встроенный язык JavaLikeCalc. Сейчас перенесено-реализовано только тренды!

Вид диаграммы определяется её размерами hsz*vsz и исходным SVG-шаблоном tmpl, ряд атрибутов кореневого тега которого имеют особое значение и которые соответствуют примитиву "Диаграмма":
- Шкала: Маркеры: шрифт (sclMarkFont), по умолчанию "Arial 10".
- Шкала: горизонтальная (sclHor), по умолчанию "Маркеры (2)" и в целом предусмотрено: "Не рисовать (0)", "Решетка (1)", "Маркеры (2)", "Решетка и маркеры (3)".
- Шкала: вертикальная (sclVer), по умолчанию "Решетка и маркеры (3)" и в целом предусмотрено: "Не рисовать (0)", "Решетка (1)", "Маркеры (2)", "Решетка и маркеры (3)", "Решетка (лог) (5)", "Маркеры (лог) (6)", "Решетка и маркеры (лог) (7)".
- Шкала: цвет (sclColor), по умолчанию "gray".
- Шкала: Маркеры: цвет (sclMarkColor), по умолчанию "gray".

В качестве источника данных на данный момент могут использоваться только архивы значений, прямым адресом к ним или через атрибут DAQ-параметра с архивом. Количество источников данных прямо не ограничено и они записываются отдельными строками аргумента srcs с форматом "{addr}:{min}:{max}:{color}[:{scale}[:{width}]]", где:
- addr — адрес к архиву значений или атрибуту DAQ-параметра, например: "/Archive/va_CPULoad_load", "/DAQ/LogicLev/gen/F3/var";
- min, max — минимум и максимум шкалы, при отсутствии шкалы (min >= max) она будет вычисляться из реальных данных;
- color — цвет кривой тренда;
- scale — отдельное определение шкалы тренда, по умолчанию "Глобально (0)" и в целом предусмотрено: "Глобально (0)", "Маркеры (2)", "Решетка и маркеры (3)", "Маркеры (лог) (6)", "Решетка и маркеры (лог) (7)".
- width — ширина кривой тренда, по умолчанию 2.

Автор: Роман Савоченко <roman@oscada.org>
Спонсировано: Магомед, SAVTECH
Версия: 1.4.0
Лицензия: GPLv2',1,10,0,'function strChars(inS) {
	for(inSz = 0, off = 0; off < inS.length; inSz++)
		inS.charAt(off, "UTF-8");
	return inSz;
}

im = SYS.XMLNode();
im.load(tmpl, false, 3);
if(!(drawArea=im.getElementBy("drawArea")) || !(scVer=im.getElementBy("scVer")) || !(scHor=im.getElementBy("scHor")) ||
		(styles=im.getElementsBy("style")[0]).isEVal())
	return "";

//Constant
FD_GRD = 0x1;
FD_MARKS = 0x2;
FD_LOG = 0x4;

if(!end)	end = SYS.time();
begin = end - size;
reqPer = size/vsz;

//Getting the trends data
trends = new Array();
for(off = 0; (tEl=srcs.parseLine(0,off)).length; ) {
	elO = new Object();
	elO.addr = tEl.parse(0, ":");
	elO.min = tEl.parse(1, ":"); elO.max = tEl.parse(2, ":");
	elO.color = tEl.parse(3, ":");
	elO.scale = tEl.parse(4, ":").toInt();
	elO.width = max(1, tEl.parse(5,":").toInt());
	elO.beg = begin*1e6; elO.per = reqPer*1e6;
	if((elO.srcO=SYS.nodeAt(elO.addr))) {
		if(typeof(elO.srcO) == "TCntrNode:TVal")	elO.srcO = elO.srcO.arch();
		if(typeof(elO.srcO) != "TCntrNode:TVArchive")	continue;
		elO.val = elO.srcO.getVals(elO.beg, end*1e6, elO.per, arch);
	}
	else if(!(dDt=SYS.XMLNode()).load(elO.addr).toInt() && dDt.name() == "d") {
		elO.val = new Array();
		isSec = dDt.attr("s").toInt();
		isAprox = dDt.attr("aprox").toInt();
		elO.per = dDt.attr("per").toInt() * (isSec?1e6:1);
		for(prevPos = 0, prevVal = EVAL, maxPos = floor(1e6*(end-begin)/elO.per), off1 = 0; true; ) {
			if((svl=dDt.text().parse(0,",",off1)).length)
				curPos = svl.parse(0,"=").toInt(), curVal = ((curVal=svl.parse(1,"="))==EVAL)?EVAL:curVal.toReal();
			else curPos = maxPos+1;
			for(stPos = prevPos; prevPos < curPos; prevPos++)
				if(isAprox && !prevVal.isEVal() && !curVal.isEVal())
					elO.val.push(prevVal+(curVal-prevVal)*(prevPos-stPos)/(curPos-stPos));
				else elO.val.push(prevVal);
			if(prevPos > maxPos)	break;
			prevVal = curVal;
		}
	}
	else continue;

	trends.push(elO);
}

im.setAttr("width", hsz).setAttr("height", vsz).setAttr("viewBox", "0 0 "+hsz+" "+vsz);

//Getting the labels font
if(!(tVl=im.attr("sclMarkFont")).length)	tVl = "Arial "+max(5,floor(hsz/50));
off = 0;
styles.setText("\ntext { font-family: "+tVl.parse(0," ",off)+"; font-size: "+(mrkFontSize=tVl.parse(0," ",off).toInt())+"px; "
		"font-weight: "+(tVl.parse(0," ",off).toInt()?"bold":"normal")+"; font-style: "+(tVl.parse(0," ",off).toInt()?"italic":"normal")+"; }" + styles.text());
sclHor = (tVl=im.attr("sclHor")).length ? tVl.toInt() : FD_MARKS;
sclVer = (tVl=im.attr("sclVer")).length ? tVl.toInt() : FD_MARKS|FD_GRD;
if(!(sclColor=im.attr("sclColor")).length)			sclColor = "gray";
if(!(sclMarkColor=im.attr("sclMarkColor")).length)	sclMarkColor = "gray";

mrkFontWRange = 0.7;
mrkWidth = 5*mrkFontSize*mrkFontWRange;
mrkHeight = mrkFontSize;

tArMrg = 2;
tArX	= tArMrg;
tArW	= hsz - tArX - tArMrg;
tArY	= tArMrg;
tArH	= vsz - tArY - tArMrg;
if(sclHor&FD_MARKS) {
	if(tArH < 100) sclHor = sclHor & ~(FD_MARKS);
	else tArH -= mrkHeight*2;
}

//Calculating the vertical scale
vsPerc = true;			//Vertical scale percent mode
isLog = sclVer&FD_LOG;		//Logarithmic scale
vsMax = -3e300, vsMin = 3e300;	//Trend''s vertical scale border

// Get main scale for non individual parameters
prmInGrp = 0, prmGrpLast = -1;
for(iP = 0, mainPerc = false; iP < trends.length; iP++) {
	cP = trends[iP];
	if(!cP.val.length || !cP.color.length) continue;
	cP.adjU = -3e300, cP.adjL = 3e300;
	if(cP.max > cP.min)	cP.adjL = cP.min, cP.adjU = cP.max;
	else {
		for(iDt = 0; iDt < cP.val.length; iDt++)
			if(!cP.val[iDt].isEVal()) { cP.adjL = min(cP.adjL, cP.val[iDt]); cP.adjU = max(cP.adjU, cP.val[iDt]); }
		// Value range expanding
		//  No value
		if(cP.adjU == -3e300)	cP.adjU = 1, cP.adjL = 0;
		// Range and absolute value are very small
		else if((cP.adjU-cP.adjL) < 1e-30 && abs(cP.adjU) < 1e-30)	cP.adjU += 0.5, cP.adjL -= 0.5;
		// Only range is very small
		else if((cP.adjU-cP.adjL) < 1e-30)
			cP.adjL -= (iP/trends.length)*abs(cP.adjU), cP.adjU += (1-iP/trends.length)*abs(cP.adjU);
		// Range smaller relatively to absolute value
		else if((cP.adjU-cP.adjL) / abs(cP.adjL+(cP.adjU-cP.adjL)/2) < 0.001) {
			wnt_dp = 0.001*abs(cP.adjL+(cP.adjU-cP.adjL)/2)-(cP.adjU-cP.adjL);
			cP.adjL -= (iP/trends.length)*wnt_dp, cP.adjU += (1-iP/trends.length)*wnt_dp;
		}
	}

	cP.wScale = cP.scale&(sclVer|FD_LOG);
	if(cP.wScale&(FD_MARKS|FD_GRD))	continue;

	//  Check for value border allow
	if(!mainPerc && (vsMin > vsMax || max(abs((vsMax-cP.adjL)/(vsMax-vsMin)-1),abs((cP.adjU-vsMin)/(vsMax-vsMin)-1)) < 0.2))
	{ vsMin = min(vsMin, cP.adjL); vsMax = max(vsMax, cP.adjU); }
	else { vsMax = -3e300; vsMin = 3e300; mainPerc = true; }

	prmInGrp++; prmGrpLast = iP;
}

// Checking for individual parameters and for the possibility to merge their to groups or create a new one for no group
prmIndiv = 0, prmIndivSc = -1, prmsInd = new Array();
for(iP = 0; iP < trends.length; iP++) {
	cP = trends[iP];
	cP.isIndiv = false;
	if(!cP.val.length || !cP.color.length || !(cP.wScale&(FD_MARKS|FD_GRD))) continue;
	// Checking for include to the present one or create a new group and exclude from individual ones
	if((!prmInGrp || (vsMin < vsMax && max(abs((vsMax-cP.adjL)/(vsMax-vsMin)-1),abs((cP.adjU-vsMin)/(vsMax-vsMin)-1)) < 0.2)) &&
			(cP.scale&FD_LOG) == (sclVer&FD_LOG))
	{
		vsMin = min(vsMin, cP.adjL); vsMax = max(vsMax, cP.adjU);
		prmInGrp++; prmGrpLast = iP;
		continue;
	}
	cP.isIndiv = true;
	prmIndiv++;
	if(prmIndivSc < 0 && (cP.scale&FD_GRD)) prmIndivSc = iP;
	else prmsInd.push(iP);
	if(cP.scale&FD_LOG) {
		cP.adjU = lg(max(1e-100,cP.adjU)); cP.adjL = lg(max(1e-100,cP.adjL));
		if((cP.adjU-cP.adjL) / abs(cP.adjL+(cP.adjU-cP.adjL)/2) < 0.0001) {
			wnt_dp = 0.0001*abs(cP.adjL+(cP.adjU-cP.adjL)/2)-(cP.adjU-cP.adjL);
			cP.adjL -= wnt_dp/2; cP.adjU += wnt_dp/2;
		}
	}
}
if(prmInGrp) prmsInd.push(-1);
if(prmIndivSc >= 0) prmsInd.push(prmIndivSc);

// Final main scale adapting
if(vsMin > vsMax) { vsPerc = true; vsMax = 100; vsMin = isLog ? pow(10,min(0,2-(tArH/150))) : 0; }
else vsPerc = false;
if(isLog) {
	vsMax = lg(max(1e-100,vsMax)); vsMin = lg(max(1e-100,vsMin));
	if((vsMax-vsMin) / abs(vsMin+(vsMax-vsMin)/2) < 0.0001) {
		wnt_dp = 0.0001*abs(vsMin+(vsMax-vsMin)/2)-(vsMax-vsMin);
		vsMin -= wnt_dp/2; vsMax += wnt_dp/2;
	}
}

//Draw main and individual vertical scales
vmax_ln = tArH / ((sclVer&FD_MARKS)?(2*mrkHeight):15);
for(iP = 0; vmax_ln >= 2 && iP < prmsInd.length; iP++) {	//prmsInd[i]=-1 - for main scale
	vDiv = 1;
	if(prmsInd[iP] < 0) {	//Main scale process
		// Draw the environment
		vsPercT = vsPerc;
		isLogT = isLog;
		sclVerT = sclVer;
		clrGridT = sclColor;
		clrMrk = sclMarkColor;
		if(prmInGrp == 1 && prmGrpLast >= 0)	//Set color for single parameter in main group
			clrGridT = clrMrk = trends[prmGrpLast].color;
		// Rounding
		v_len = vsMax - vsMin;
		while(v_len > vmax_ln)						{ vDiv *= 10; v_len /= 10; }
		while(v_len && v_len < vmax_ln/10)	{ vDiv /= 10; v_len *= 10; }
		vsMin = floor(vsMin/vDiv)*vDiv; vsMax = ceil(vsMax/vDiv)*vDiv;
		while(!isLogT && ((vsMax-vsMin)/vDiv) < vmax_ln/2) vDiv /= 2;
		vsMinT = vsMin; vsMaxT = vsMax;
	}
	else {	//Individual scale process
		cP = trends[prmsInd[iP]];
		// Draw environment
		vsPercT = false;
		isLogT = cP.scale&FD_LOG;
		sclVerT = cP.wScale;
		clrGridT = clrMrk = cP.color;
		// Rounding
		v_len = cP.adjU - cP.adjL;
		while(v_len > vmax_ln)						{ vDiv *= 10; v_len /= 10; }
		while(v_len && v_len < vmax_ln/10)	{ vDiv /= 10; v_len *= 10; }
		cP.adjL = floor(cP.adjL/vDiv)*vDiv; cP.adjU = ceil(cP.adjU/vDiv)*vDiv;
		while(!isLogT && ((cP.adjU-cP.adjL)/vDiv) < vmax_ln/2) vDiv /= 2;
		vsMinT = cP.adjL; vsMaxT = cP.adjU;
	}
	if(iP < (prmsInd.length-1))	sclVerT = sclVerT&(~FD_GRD);	//Hide grid for not last scale

	// Draw vertical grid and markers
	markWdth = 0;
	if(sclVerT&(FD_MARKS|FD_GRD)) {
		scVer.childAdd("rect").setAttr("fill",clrGridT).setAttr("x",tArX-1).setAttr("y",tArY).setAttr("width",1).setAttr("height",tArH);
		for(iV = floor((vsMinT/vDiv)+0.5)*vDiv; (vsMaxT-iV)/vDiv > -0.1; iV += vDiv) {
			//  Draw grid
			v_pos = tArY + tArH - tArH*(iV-vsMinT)/(vsMaxT-vsMinT);
			if(sclVerT&FD_GRD)	scVer.childAdd("rect").setAttr("fill",sclColor).setAttr("x",tArX).setAttr("y",floor(v_pos+0.5)).setAttr("width",tArW).setAttr("height",1);
			else	scVer.childAdd("rect").setAttr("fill",clrGridT).setAttr("x",tArX-3).setAttr("y",floor(v_pos+0.5)).setAttr("width",6).setAttr("height",1);
			//  Draw markers
			if(sclVerT&FD_MARKS) {
				isPerc = vsPercT && ((vsMaxT-iV-vDiv)/vDiv <= -0.1);
				isMax = (v_pos-1-mrkHeight) < tArY;
				labVal = (isLogT?pow(10,iV):iV).toPrecision(5) + (isPerc?" %":"");
				scVer.childAdd("text").setAttr("text-anchor","start").setAttr("stroke","none")
						.setAttr("fill", clrMrk)
						.setAttr("x",tArX+2).setAttr("y",floor(v_pos-1+(isMax?mrkHeight:0)+0.5)).setText(labVal);
				markWdth = max(markWdth, labVal.length*mrkFontSize*mrkFontWRange);
			}
		}
	}
	if(iP < (prmsInd.length-1)) { tArX += markWdth?(markWdth+5):0; tArW -= markWdth?(markWdth+5):0; }
}

//Calc and draw the horizontal scale
hDiv = 1;			//Horisontal scale divisor
hmax_ln = tArW / ((sclHor&FD_MARKS)?mrkWidth:15);
if(hmax_ln >= 2) {
	hvLev = 0;
	hLen = end - begin;

	if(hLen/2635200 >= 5)		{ hvLev = 7; hDiv = 2635200; }	//Month a unstrict interval !!!! to implement !!!!
	else if(hLen/86400 >= 5)	{ hvLev = 6; hDiv =   86400; }	//More days and no time in the scale
	else if(hLen/86400 >= 2)	{ hvLev = 5; hDiv =   86400; }	//Days
	else if(hLen/3600 >= 2)	{ hvLev = 4; hDiv =    3600; }	//Hours
	else if(hLen/60 >= 2)		{ hvLev = 3; hDiv =      60; }	//Minutes
	else if(hLen/1 >= 2)		{ hvLev = 2; hDiv =       1; }	//Seconds
	else if(hLen/0.001 >= 2)	{ hvLev = 1; hDiv =   0.001; }	//Milliseconds

	// Draw the horisontal grid and markers
	if(sclHor&(FD_MARKS|FD_GRD)) {
		tm_t = 0;
		lab_tm = lab_dt = "";

		//  Draw generic grid line
		scHor.childAdd("rect").setAttr("fill",sclColor).setAttr("x",floor(tArX+0.5)).setAttr("y",floor(tArY+tArH+0.5)).setAttr("width",floor(tArW+0.5)).setAttr("height",1);

		//  Draw full trend''s data and time to the trend end position
		begMarkBrd = -5;
		endMarkBrd = tArX + tArW;
		if((sclHor&FD_MARKS) && mrkHeight) {
			lab_dt = SYS.strftime(end, "%d-%m-%Y");
			if((end%60) == 0 && (end-floor(end)) == 0)
				lab_tm = SYS.strftime(end, "%H:%M");
			else if((end-floor(end)) == 0)
				lab_tm = SYS.strftime(end, "%H:%M:%S");
			else lab_tm = SYS.strftime(end, "%H:%M:%S")+"."+(end-floor(end));

			markY = tArY + tArH + mrkHeight;
			if(hvLev < 6) {
				scHor.childAdd("text").setAttr("text-anchor","end").setAttr("stroke","none")
					.setAttr("fill",sclMarkColor)
					.setAttr("x",tArX+tArW).setAttr("y",markY).setText(lab_tm);
				endMarkBrd = min(endMarkBrd, tArX + tArW - mrkFontSize*lab_tm.length*mrkFontWRange);
				markY += mrkHeight;
			}
			scHor.childAdd("text").setAttr("text-anchor","end").setAttr("stroke","none")
				.setAttr("fill",sclMarkColor)
				.setAttr("x",tArX+tArW).setAttr("y",markY).setText(lab_dt);
			endMarkBrd = min(endMarkBrd, tArX + tArW - mrkFontSize*lab_dt.length*mrkFontWRange);
		}


		hDiv_ = hDiv;
		while(hLen/hDiv_ > hmax_ln)	hDiv_ *= 10;
		while(hLen/hDiv_ < hmax_ln/5 && (1e6*hDiv_/5)%(1e6*hDiv) == 0) hDiv_ /= 5;
		while(hLen/hDiv_ < hmax_ln/2 && (hvLev < 6 || (1e6*hDiv_/2)%(1e6*hDiv) == 0)) hDiv_ /= 2;
		hDiv = hDiv_;

		UTChourDt = SYS.strptimegm(SYS.strftime(end)) - end;

		//  Draw grid and/or markers
		first_m = true;
		ttm1 = new Object();
		for(iH = begin; true; ) {
			//   Draw grid
			h_pos = tArX + tArW*(iH-begin)/(end-begin);
			if(sclHor&FD_GRD)
				scHor.childAdd("rect").setAttr("fill",sclColor).setAttr("x",floor(h_pos+0.5)).setAttr("y",floor(tArY+0.5)).setAttr("width",1).setAttr("height",tArH);
			else scHor.childAdd("rect").setAttr("fill",sclColor).setAttr("x",floor(h_pos+0.5)).setAttr("y",floor(tArY+tArH-3+0.5)).setAttr("width",1).setAttr("height",6);

			//   Draw markers
			if((sclHor&FD_MARKS) && (!(1e6*(iH+UTChourDt)%(1e6*hDiv)) || hvLev >= 7) && iH != end) {
				if(first_m)	SYS.localtime(begin-(end-begin), ttm1.sec, ttm1.min, ttm1.hour, ttm1.mday, ttm1.mon);
				ttm = new Object();
				SYS.localtime(iH, ttm.sec, ttm.min, ttm.hour, ttm.mday, ttm.mon);

				chLev = 0;
				if(ttm.year-ttm1.year)			chLev = 6;
				else if(ttm.mon-ttm1.mon) 	chLev = 5;
				else if(ttm.mday-ttm1.mday)	chLev = 4;
				else if(ttm.hour-ttm1.hour)	chLev = 3;
				else if(ttm.min-ttm1.min)		chLev = 2;
				else if(ttm.sec-ttm1.sec)		chLev = 1;

				//Checking for the data presence
				lab_dt = lab_tm = "";
				//Date
				if(hvLev == 7)
					lab_dt = SYS.strftime(iH, (chLev>=6)?"%B %Y":"%B");
				else if(/*hvLev == 5 ||*/ chLev >= 4)
					lab_dt = SYS.strftime(iH, (chLev>=5)?"%d-%m-%Y":"%d");
				//Hours and minuts
				if((hvLev == 4 || hvLev == 3 || ttm.hour || ttm.min) && !ttm.sec)
					lab_tm = SYS.strftime(iH, "%H:%M");
				//Seconds
				else if((hvLev == 2 || ttm.sec) && !((iH-floor(iH))*1e6))
					lab_tm = (chLev >= 2) ? SYS.strftime(iH, "%H:%M:%S") : (iH%60).toPrecision(6)+tr("s");
				//Milliseconds
				else if(hvLev <= 1 || (iH-floor(iH))*1e6)
					lab_tm = (chLev >= 2) ? SYS.strftime(iH, "%H:%M:%S")+(iH-floor(iH)).toPrecision(6).slice(1) :
								(chLev >= 1) ? ((iH%60)+(iH-floor(iH))).toPrecision(6)+tr("s") :
													(1e3*iH).toPrecision(6)+tr("ms");

				//SYS.messInfo("DG", "lab_tm="+lab_tm+"; hvLev="+hvLev+"; chLev="+chLev+"; iH="+iH);

				endPosTm = endPosDt = begMarkBrd, markY = tArY + tArH + mrkHeight;
				if(hvLev < 6) {
					if(lab_tm.length) {
						wdth = mrkFontSize*strChars(lab_tm)*mrkFontWRange;
						tpos = max(h_pos-wdth/2, 0);
						if(first_m || (tpos+wdth) < endMarkBrd && tpos > begMarkBrd) {
							if(first_m)	tpos = max(begMarkBrd, tpos);
							scHor.childAdd("text").setAttr("text-anchor",(first_m?"start":"middle")).setAttr("stroke","none")
								.setAttr("fill",sclMarkColor)
								.setAttr("x",floor(tpos+(first_m?0:wdth/2)+0.5)).setAttr("y",floor(markY+0.5)).setText(lab_tm);
							endPosTm = tpos + wdth;
						}
					}
					markY += mrkHeight;
				}
				if(lab_dt.length) {
					wdth = mrkFontSize*strChars(lab_dt)*mrkFontWRange;
					tpos = max(h_pos-wdth/2, 0);
					if(first_m || (tpos+wdth) < endMarkBrd && tpos > begMarkBrd) {
						if(first_m)	tpos = max(begMarkBrd, tpos);
						scHor.childAdd("text").setAttr("text-anchor",(first_m?"start":"middle")).setAttr("stroke","none")
							.setAttr("fill",sclMarkColor)
							.setAttr("x",floor(tpos+(first_m?0:wdth/2)+0.5)).setAttr("y",floor(markY+0.5)).setText(lab_dt);
						endPosDt = tpos + wdth;
					}
				}
				begMarkBrd = max(begMarkBrd, max(endPosTm,endPosDt));
				ttm1 = ttm;
				first_m = false;
			}

			//   Next
			if(iH >= end) break;
			if(hvLev >= 7) {	//Per month
				curYear = curMonth = 0;
				SYS.localtime(iH, 0, 0, 0, 0, curMonth, curYear);
				iH = SYS.mktime(0, 0, 0, 1, curMonth+1, curYear);
			} else iH = floor((iH+UTChourDt)/hDiv)*hDiv + hDiv - UTChourDt;
			iH = min(end, iH);
		}
	}
}

//Drawing the trends
for(iTr = 0; iTr < trends.length; iTr++) {
	cP = trends[iTr];

	vsPercT = cP.isIndiv ? false : vsPerc;
	isLogT = cP.isIndiv ? (cP.wScale&FD_LOG) : isLog;
	vsMaxT = cP.isIndiv ? cP.adjU : vsMax;
	vsMinT = cP.isIndiv ? cP.adjL : vsMin;

	// Prepare border for percent trend
	bordL = cP.min;
	bordU = cP.max;
	if(vsPercT && bordL >= bordU) { bordL = cP.adjL; bordU = cP.adjU; }
		/*bordU = -3e300, bordL = 3e300;
		for(iDt = 0; iDt < cP.val.length; iDt++) {
			c_val = cP.val[iDt];
			if(!c_val.isEVal()) { bordL = min(bordL, c_val); bordU = max(bordU, c_val); }
		}
		if((vMarg=(bordU-bordL)/10) == 0) vMarg = 0.5;
		bordL -= vMarg;
		bordU += vMarg;
	}*/

	trPath = 0; prev_vl = EVAL; prev_pos = 0;
	for(iDt = 0; iDt < cP.val.length; iDt++) {
		c_val = cP.val[iDt];
		if(vsPercT && !c_val.isEVal()) c_val = 100*(c_val-bordL)/(bordU-bordL);
		c_tm = 1e-6*(cP.beg + iDt*cP.per);
		if(c_tm < begin)	continue;
		c_pos = floor(tArX + tArW*(c_tm-begin)/(end-begin));
		if(!c_val.isEVal()) {
			c_vpos = max(tArY,min(tArY+tArH,floor(tArY + tArH - tArH*((isLogT?lg(max(1e-100,c_val)):c_val)-vsMinT)/(vsMaxT-vsMinT))));
			if(!trPath) trPath = drawArea.childAdd("path").setAttr("stroke", cP.color).setAttr("stroke-width",cP.width);
			if(prev_vl.isEVal()) trPath.setAttr("d", trPath.attr("d")+"M"+c_pos+","+c_vpos);
			else trPath.setAttr("d",trPath.attr("d")+"L"+c_pos+","+c_vpos);
		}
		else if(!prev_vl.isEVal()) {
			c_vpos = max(tArY,min(tArY+tArH,floor(tArY + tArH - tArH*((isLogT?lg(max(1e-100,prev_vl)):prev_vl)-vsMinT)/(vsMaxT-vsMinT))));
			trPath.setAttr("d", trPath.attr("d")+"L"+prev_pos+".1,"+c_vpos+".1");
		}
		prev_vl = c_val; prev_pos = c_pos;
	}
}

return im.save();','','',1581943347);
CREATE TABLE IF NOT EXISTS 'tmplib_LowDevLib' ("ID" TEXT DEFAULT '' ,"NAME" TEXT DEFAULT '' ,"DESCR" TEXT DEFAULT '' ,"MAXCALCTM" INTEGER DEFAULT '10' ,"PR_TR" INTEGER DEFAULT '0' ,"PROGRAM" TEXT DEFAULT '' ,"TIMESTAMP" INTEGER DEFAULT '0' , PRIMARY KEY ("ID"));
INSERT INTO tmplib_LowDevLib VALUES('1602A','GPIO|I2C: 1602A(HD44780)','LCD Module 1602A, STN, BLUB, 16 Character x 2 Line,  5 x 8 Dots, by the direct (Raspberry PI BCM2835 GPIO) or I2C (PCF8574) wiring.
Conditions: Default planing policy but realtime one preferred.
Author: Roman Savochenko <roman@oscada.org>
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
INSERT INTO tmplib_LowDevLib VALUES('1W_DS9097','One Wire by DS9097','One Wire sensors bus implementing by 1Wire-adapter DS9097. Supported direct and parasite powering for the temperature sensors.
Supported 1Wire-devices: DS1820, DS1820/DS18S20/DS1920 (not tested), DS1822 (not tested), DS2413, DS2408, DS2450, DS2438.
Author: Roman Savochenko <roman@oscada.org>
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
					devFamily = devID.charCodeAt(0)&0x7F; devNm = tr("1W device"); dO.isT = false;
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
			devFamily = devID.charCodeAt(0)&0x7F;
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
INSERT INTO tmplib_LowDevLib VALUES('1W_DS9097U','One Wire by DS9097U','One Wire sensors bus implementing by 1Wire-adapter DS9097U. Supported direct and parasite powering for the temperature sensors.
Supported 1Wire-devices: DS1820, DS1820/DS18S20/DS1920 (not tested), DS1822 (not tested), DS2413, DS2408, DS2450, DS2438.
Author: Roman Savochenko <roman@oscada.org>
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
					devFamily = devID.charCodeAt(0)&0x7F; devNm = tr("1W device"); dO.isT = false;
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
			devFamily = devID.charCodeAt(0)&0x7F;
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
INSERT INTO tmplib_LowDevLib VALUES('BMP180','I2C: BMP180','I2C Pressure and Temperature sensor. Connect through a Serial output transport into the I2C mode.
Author: Roman Savochenko <roman@oscada.org>
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
INSERT INTO tmplib_LowDevLib VALUES('DHT','GPIO: DHT11,22 (AM23XX)','Digital Temperature and Humidity Sensor for models: DHT11, DHT12, AM2302, AM2320, ...
The module designed for the sensors connect through GPIO, mostly it''s Raspberry PI BCM2835 GPIO.
Conditions: Exclusively realtime planing in the priority 199 (FIFO-99).
Author: Roman Savochenko <roman@oscada.org>
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
INSERT INTO tmplib_LowDevLib VALUES('DS3231','I2C: DS1307,DS3231','I2C RTC chips DS1307,DS3231 with Temperature sensor and calibration for DS3231. Connects through a Serial output transport into the I2C mode.
Author: Roman Savochenko <roman@oscada.org>
Version: 1.1.0',10,0,'JavaLikeCalc.JavaScript
//Set transport
if(f_start) {
	f_err = "0";
	transport_ = transport;
	tr = SYS.Transport.Serial.nodeAt("out_"+transport);
	mode_ = -1;
	tm_ = tm = "";
	agOff_ = 0;
	p32k_ = false;
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
	if(mode != mode_) {
		if(mode == 0) {
			this.attrAdd("pSQWf", "", "integer|sel", "0;1;2;3\n1Hz;1.024kHz;4.096kHz;8.192kHz");
			this.attrAdd("agOff", "Aging offset, [-128...127]", "integer");
			this.attrAdd("t", "T, °С", "real|ro");
			this.attrAdd("p32k", "Enable 32768Hz", "boolean");
		}
		else {
			this.attrAdd("pSQWf", "", "integer|sel", "0;1;2;3\n1Hz;4.096kHz;8.192kHz;32.768kHz");
			this.attrDel("agOff");
			this.attrDel("t");
			this.attrDel("p32k");
		}
		mode_ = mode;
	}
	if(tm != tm_) {
		off = 0; sdt = tm.parse(0,"T",off); stm = tm.parse(0,"T",off);
		off = 0; year = sdt.parse(0,"-",off).toInt(16); month = sdt.parse(0,"-",off).toInt(16); day = sdt.parse(0,"-",off).toInt(16);
		off = 0; hour = stm.parse(0,":",off).toInt(16); min = stm.parse(0,":",off).toInt(16); sec = stm.parse(0,":",off).toInt(16);
		tr.messIO(SYS.strFromCharCode(addr,0,sec,min,hour,0,day,month+((year&0xF00)?0x80:0),year&0xFF), 0, 0);
		tm_ = tm;
	}
	if(mode == 0 && !(tVl=this.agOff.get()).isEVal() && tVl != agOff_) {
		tr.messIO(SYS.strFromCharCode(addr,16,tVl), 0, 0);
		agOff_ = tVl;
	}
	if(mode == 0 && !(tVl=this.p32k.get()).isEVal() && tVl != p32k_) {
		resp = tr.messIO(SYS.strFromCharCode(addr,15), 0, 1);
		if(resp.length != 1) t_err = "3:"+tr("Wrong or empty response.");
		else {
			cntrB = resp.charCodeAt(0);
			cntrB = tVl ?	cntrB|0x08 : cntrB&(~0x08);
			tr.messIO(SYS.strFromCharCode(addr,15,cntrB), 0, 0);
		}
		p32k_ = tVl;
	}
	if(pSQW != pSQW_) {
		if(mode == 0) {
			resp = tr.messIO(SYS.strFromCharCode(addr,14), 0, 1);
			if(resp.length != 1) t_err = "3:"+tr("Wrong or empty response.");
			else {
				cntrB = resp.charCodeAt(0);
				cntrB = pSQW ? cntrB&(~0x04) : cntrB|0x04;
				tr.messIO(SYS.strFromCharCode(addr,14,cntrB), 0, 0);
			}
		}
		else {
			resp = tr.messIO(SYS.strFromCharCode(addr,7), 0, 1);
			if(resp.length != 1) t_err = "3:"+tr("Wrong or empty response.");
			else {
				cntrB = resp.charCodeAt(0);
				cntrB = pSQW ? cntrB|0x10 : cntrB&(~0x10);
				tr.messIO(SYS.strFromCharCode(addr,7,cntrB), 0, 0);
			}
		}
		pSQW_ = pSQW;
	}
	if(pSQWf != pSQWf_) {
		pSQWf = max(0, min(3,pSQWf));
		if(mode == 0) {
			resp = tr.messIO(SYS.strFromCharCode(addr,14), 0, 1);
			if(resp.length != 1) t_err = "3:"+tr("Wrong or empty response.");
			else {
				cntrB = (resp.charCodeAt(0)&(~0x18)) + (pSQWf<<3);
				tr.messIO(SYS.strFromCharCode(addr,14,cntrB), 0, 0);
			}
		}
		else {
			resp = tr.messIO(SYS.strFromCharCode(addr,7), 0, 1);
			if(resp.length != 1) t_err = "3:"+tr("Wrong or empty response.");
			else {
				cntrB = (resp.charCodeAt(0)&(~0x3)) + pSQWf;
				tr.messIO(SYS.strFromCharCode(addr,7,cntrB), 0, 0);
			}
		}
		pSQWf_ = pSQWf;
	}

	//Get current
	rSz = (mode == 0) ? 19 : 8;
	resp = tr.messIO(SYS.strFromCharCode(addr,0), 0, rSz);
	if(resp.length != rSz) t_err = "3:"+tr("Wrong or empty response.");
	else {
		// Get current time
		tm_ = tm = (20+(resp.charCodeAt(5)>>7)).toString(10,2)+resp.charCodeAt(6).toString(16,2)+"-" +
				(resp.charCodeAt(5)&0x7F).toString(16,2)+"-" + 
				resp.charCodeAt(4).toString(16,2)+"T" +
				(resp.charCodeAt(2)&0x3F).toString(16,2)+":" +
				resp.charCodeAt(1).toString(16,2)+":" + 
				resp.charCodeAt(0).toString(16,2);

		if(mode == 0) {
			// Aging offset
			if((tVl=resp.charCodeAt(16)) >= 128)	tVl = tVl-256;
			this.agOff.set(agOff_=tVl, 0, 0, true);

			// Miscellaneous
			this.p32k.set((p32k_=resp.charCodeAt(15)&0x08), 0, 0, true);

			pSQW = pSQW_ = !(resp.charCodeAt(14)&0x04);
			pSQWf = pSQWf_ = (resp.charCodeAt(14)>>3)&0x3;

			// Get temperature
			tVl = (resp.charCodeAt(17)<<2)+(resp.charCodeAt(18)>>6);
			if(tVl >= 128) tVl = tVl-256;
			this.t.set(tVl/4, 0, 0, true);
		}
		else {
			pSQW = pSQW_ = resp.charCodeAt(7)&0x10;
			pSQWf = pSQWf_ = resp.charCodeAt(7)&0x3;
		}
	}
}

if(t_err.toInt() && !f_err.toInt()) t = p = EVAL;
f_err = t_err;',1541249292);
INSERT INTO tmplib_LowDevLib VALUES('PCF8574','I2C: PCF8574','I2C 8-bit 8DIO. Connect through a Serial output transport into the I2C mode.
Author: Roman Savochenko <roman@oscada.org>
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
INSERT INTO tmplib_LowDevLib VALUES('PCF8591','I2C: PCF8591','I2C 8-bit 4xA/D and D/A converter. Connect through a Serial output transport into the I2C mode.
Author: Roman Savochenko <roman@oscada.org>
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
INSERT INTO tmplib_LowDevLib VALUES('BME280','I2C: BME280','I2C Pressure, Temperature and Humidity sensor. Connect through a Serial output transport into the I2C mode.
Author: Arcadiy Kisel, Roman Savochenko <roman@oscada.org>
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
INSERT INTO tmplib_LowDevLib VALUES('SHT3x','I2C: SHT3x','Digital Temperature and Humidity Sensor for the models: SHT30
Author: Roman Savochenko <roman@oscada.org>
Version: 1.0.0',10,0,'JavaLikeCalc.JavaScript
//Set transport
if(f_start) {
	f_err = "0";
	transport_ = transport;
	tr = SYS.Transport.Serial.nodeAt("out_"+transport);
	H = T = EVAL;
	return;
}
if(f_stop)	return;

t_err = "0";

//Check for the transport change and connect
if(!tr || transport != transport_)	{
	tr = SYS.Transport.Serial.nodeAt("out_"+transport);
	transport_ = transport;
}
if(!tr)	t_err = "1:"+tr("Output transport ''%1'' error.").replace("%1", transport);
else if(addr < 0 || addr > 119)	t_err = "2:"+tr("Device address ''%1'' out of range [0...119].").replace("%1",addr);
else {
	if(H.isEVal()) {
		if(f_frq < 0.5)		spdCfg = 0x2032;	//0.5mps
		else if(f_frq < 1)	spdCfg = 0x2130;	//1.0mps
		else if(f_frq < 2)	spdCfg = 0x2236;	//2.0mps
		else if(f_frq < 4)	spdCfg = 0x2334;	//4.0mps
		else spdCfg = 0x2737;						//10.0mps
		//SYS.messInfo("SMH", "spdCfg="+spdCfg.toString(16)+"; frq="+f_frq+"; tm="+(1/max(0.5,f_frq)));
		tr.messIO(SYS.strFromCharCode(addr,(spdCfg>>8)&0xFF,spdCfg&0xFF), 0, 0);	//10 mps
		SYS.sleep(1/max(0.5,f_frq));
	}

	rez = tr.messIO(SYS.strFromCharCode(addr), 0, 6);
	if(rez.length == 6) {
		T = 175*(((rez.charCodeAt(0)<<8)+rez.charCodeAt(1))/65535) - 45;
		H = 100*(((rez.charCodeAt(3)<<8)+rez.charCodeAt(4))/65535);
	}
	else { t_err = "3:"+tr("No data."); H = T = EVAL; }
}

f_err = t_err;',1542469153);
INSERT INTO tmplib_LowDevLib VALUES('ADS111x','I2C: ADS101x, ADS111x','I2C 12/16-bit 4xA/D converter. Connect through a Serial output transport into the I2C mode.
Author: Roman Savochenko <roman@oscada.org>
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
		//Selecting the channel and setting for needed options 
		sw = 0xC000 + (i<<12) + (range<<9) + 0x100 + 0x83;
		// Writing for the configuration
		tr.messIO(SYS.strFromCharCode(addr,1,sw>>8,sw&0xFF), 0, 0);
		// Waiting the conversion
		for(iTr = 0; iTr < 10 && (rez=tr.messIO(SYS.strFromCharCode(addr,1),0,2)).length && !(rez.charCodeAt(0)&0x80); iTr++)
			SYS.sleep(1e-3);
		// Reading for the value
		rez = tr.messIO(SYS.strFromCharCode(addr,0), 0, 2);
		if(rez.length) {
			tVl = (rez.charCodeAt(0)<<8) + rez.charCodeAt(1);
			if(tVl&0x8000) tVl -= 65536;
			range_ = (range == 0) ? 6.144 : 4.096/pow(2,range-1);
			arguments["ai"+i] = range_*tVl/32768;
		}
		else t_err = "3:"+tr("No read result.");
	}
}

if(t_err.toInt() && !f_err.toInt())
	for(i = 0; i < 4; i++)
		arguments["ai"+i] = EVAL;

f_err = t_err;',1549528595);
INSERT INTO tmplib_LowDevLib VALUES('MCP4725','I2C: MCP4725','I2C 12-bit D/A converter. Connect through a Serial output transport into the I2C mode.
Author: Roman Savochenko <roman@oscada.org>
Version: 1.0.0',10,0,'JavaLikeCalc.JavaScript
//Set transport
if(f_start) {
	f_err = "0";
	transport_ = transport;
	tr = SYS.Transport.Serial.nodeAt("out_"+transport);
	ao = 0;
	ao_ = EVAL;
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
	//Writing the changes
	if(!ao_.isEVal() && ao != ao_) {
		code = max(0,min(1,ao/vRef)) * 4096;
		tr.messIO(SYS.strFromCharCode(addr,code/256,code%256), 0, 0);
	}

	//Reading the current value	
	rez = tr.messIO(SYS.strFromCharCode(addr), 0, 3);
	if(rez.length)	ao = ao_ = vRef*(rez.charCodeAt(1)*255+rez.charCodeAt(2)>>4)/4096;
	else t_err = "3:"+tr("No read result.");
}

if(t_err.toInt() && !f_err.toInt())	ao_ = EVAL;

f_err = t_err;',1549488136);
INSERT INTO tmplib_LowDevLib VALUES('MAX6675','GPIO: MAX6675','Cold-Junction-Compensated K-Thermocouple-to-Digital Converter (0°C to +1024°C). The module designed for the sensors connect through softSPI by GPIO, mostly it''s Raspberry PI BCM2835 GPIO.
Conditions: Exclusively realtime planing in the priority 199 (FIFO-99).
Author: Arcadiy Kisel <arcsin@online.ua>
Version: 0.1.0',10,0,'JavaLikeCalc.JavaScript
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

if(f_start){
	mode(pin_cs, 4); put(pin_cs, true); 	//Set pin to output mode and next to true for 500us
	mode(pin_sclk, 4); put(pin_sclk, false); //Set pin to output mode and next to true for 500ms
	mode(pin_miso, 1); 
	tmSleep(1e-3);
}

//Read sensor''s data function declaration
function spiread() {
	d = 0;
	for(i = 7; i >= 0; i--) {
		put(pin_sclk, false); tmSleep(1e-3);							//Set output to false for 20ms
		if(get(pin_miso))	d = d | (1 << i);
  		put(pin_sclk, true); tmSleep(1e-3);
	}
	return d;
}

//Read
put(pin_cs, false); tmSleep(1e-3);		
v = spiread();
v = v << 8;
v = v | spiread();

put(pin_cs, true); 		
if(v&0x4)	t_err = "1:"+tr("uh oh, no thermocouple attached!"); 
else	t = (v>>3)*0.25;

if(t_err.toInt() && !f_err.toInt()) t = EVAL;
f_err = t_err;',1550995458);
CREATE TABLE IF NOT EXISTS 'tmplib_tests' ("ID" TEXT DEFAULT '' ,"NAME" TEXT DEFAULT '' ,"uk#NAME" TEXT DEFAULT '' ,"ru#NAME" TEXT DEFAULT '' ,"DESCR" TEXT DEFAULT '' ,"uk#DESCR" TEXT DEFAULT '' ,"ru#DESCR" TEXT DEFAULT '' ,"MAXCALCTM" INTEGER DEFAULT '10' ,"PR_TR" INTEGER DEFAULT '0' ,"PROGRAM" TEXT DEFAULT '' ,"TIMESTAMP" INTEGER DEFAULT '0' , PRIMARY KEY ("ID"));
INSERT INTO tmplib_tests VALUES('ai_simple','Simple AI','Простий AI','Простой AI','Simple analog parameter.','Простий аналоговий параметр.','Простой аналоговый параметр.',10,0,'JavaLikeCalc.JavaScript
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
}',1627833315);
INSERT INTO tmplib_tests VALUES('ai_man','Manual input of AI','Ручний ввід AI','Ручной ввод AI','Manual input of a standard analog parameter.','Стандартний аналоговий параметр ручного вводу.','Стандартный аналоговый параметр ручного ввода.',10,0,'JavaLikeCalc.JavaScript
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
}',1627833315);
INSERT INTO tmplib_tests VALUES('test','Test','Тест','Тест','S7 controller testing','Тестування контролера S7','Тестирование контроллера S7',10,0,'
',1539451468);
INSERT INTO tmplib_tests VALUES('gasPoint','Flow control point','','','','','',10,0,'JavaLikeCalc.JavaScript
if(f_start)	Q = 0;
//F=200+(rand(5)-2.5);
Q += F.isEVal() ? 0 : F/3600;
//P=9+(rand(2)-1);
//T=15+(rand(2)-1);
dP = F.isEVal() ? 0 : F/33;
DS = 1+(rand(0.2)-0.1);
',1561831602);
CREATE TABLE IF NOT EXISTS 'UserProtocol_uPrt' ("ID" TEXT DEFAULT '' ,"NAME" TEXT DEFAULT '' ,"uk#NAME" TEXT DEFAULT '' ,"ru#NAME" TEXT DEFAULT '' ,"DESCR" TEXT DEFAULT '' ,"uk#DESCR" TEXT DEFAULT '' ,"ru#DESCR" TEXT DEFAULT '' ,"EN" INTEGER DEFAULT '0' ,"DAQTmpl" TEXT DEFAULT '' ,"WaitReqTm" INTEGER DEFAULT '0' ,"InPROG" TEXT DEFAULT '' ,"OutPROG" TEXT DEFAULT '' ,"PR_TR" INTEGER DEFAULT '0' ,"TIMESTAMP" INTEGER DEFAULT '0' , PRIMARY KEY ("ID"));
INSERT INTO UserProtocol_uPrt VALUES('SMS','SMS (replaced)','','','!!!!: Replaced by the complex template Main.ntf. Will be removed soon
Provides operations with SMS by GSM-modem connected as serial device. For now supported only sending SMS messages to a number of remote cell phone or GSM modem.
Author: Roman Savochenko <roman@oscada.org>
Version: 2.1.1','','',1,'',0,'','JavaLikeCalc.JavaScript
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
	if(rez.indexOf("OK\r") < 0)	{ io.setAttr("err", "101:"+tr("Error disabling ECHO: %1").replace("%1",rez)); return; }

	//Set PIN
	if(io.attr("pin").length) {
		//Check for PIN
		rez = tr.messIO("AT+CPIN?\r");
		while(rez.length && (trez=tr.messIO("")).length) rez += trez;
		if(rez.indexOf("OK\r") < 0)	{ io.setAttr("err", "102:"+tr("Error checking for PIN-code: %1").replace("%1",rez)); return; }
		if(rez.indexOf("+CPIN: READY\r") < 0) {	//PIN is needed
			rez = tr.messIO("AT+CPIN="+io.attr("pin")+"\r");
			while(rez.length && (trez=tr.messIO("")).length) rez += trez;
			if(rez.indexOf("OK\r") < 0)	{ io.setAttr("err", "102:"+tr("Error setting PIN: %1").replace("%1",rez)); return; }
		}
	}

	if(io.attr("text").toInt()) {	//SMS in the text mode
		//Switch to Text SMS mode
		rez = tr.messIO("AT+CMGF=1\r");
		while(rez.length && (trez=tr.messIO("")).length) rez += trez;
		if(rez.indexOf("OK\r") < 0)	{ io.setAttr("err", "103:"+tr("Error setting the SMS Text mode: %1").replace("%1",rez)); return; }

		//Send the Telephone number
		rez = tr.messIO("AT+CMGS=\""+tel+"\"\r");
		while(rez.length && (trez=tr.messIO("")).length) rez += trez;
		if(rez.indexOf(">") < 0)	{ io.setAttr("err", "104:"+tr("Error sending SMS: %1").replace("%1",rez)); return; }
		rez = tr.messIO(io.text()+"\x1A");
		while(rez.length && (trez=tr.messIO("")).length) rez += trez;
		//for(var iTr = 0; iTr < 100 && rez.indexOf("OK\r") < 0; iTr++) rez += tr.messIO("");	//Up to 10 seconds wait for reply
		if(rez.indexOf("OK\r") < 0)	{ io.setAttr("err", "104:"+tr("Error sending SMS PDU: %1").replace("%1",rez)); return; }
	}
	else {	//SMS in the PDU mode
		// Switch to PDU SMS mode
		rez = tr.messIO("AT+CMGF=0\r");
		while(rez.length && (trez=tr.messIO("")).length) rez += trez;
		if(rez.indexOf("OK\r") < 0)	{ io.setAttr("err", "103:"+tr("Error setting the SMS PDU mode: %1").replace("%1",rez)); return; }

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
		if(rez.indexOf(">") < 0)	{ io.setAttr("err", "104:"+tr("Error sending SMS: %1").replace("%1",rez)); return; }
		rez = tr.messIO(pdu+"\x1A");
		while(rez.length && (trez=tr.messIO("")).length) rez += trez;
		//for(var iTr = 0; iTr < 100 && rez.indexOf("OK\r") < 0; iTr++) rez += tr.messIO("");	//Up to 10 seconds wait for reply
		if(rez.indexOf("OK\r") < 0)	{ io.setAttr("err", "104:"+tr("Error sending SMS PDU: %1").replace("%1",rez)); return; }
		//SYS.messDebug("TEST SMS","PDU REZ :"+rez);
	}
	io.setAttr("err", "0");
}',0,1550073193);
INSERT INTO UserProtocol_uPrt VALUES('SCU750','EDWARDS TURBOMOLECULAR PUMPS','','','Protocol level of typical EDWARDS TURBOMOLECULAR PUMPS (http://edwardsvacuum.com) data request by SCU750 Cotrol Unit protocol.
Author: Roman Savochenko <roman@oscada.org>
Sponsored: Vasiliy Grigoriev from "Vacuum technologies laboratory (http://e-beam.ru)".
Version: 1.0.0','','',1,'',0,'','JavaLikeCalc.JavaScript
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
}',0,1509283349);
INSERT INTO UserProtocol_uPrt VALUES('TMH','Power supply for turbomolecular pumps','','','Protocol level of power supply for turbomolecular pumps of firm SHIMADZU (http://www.shimadzu.com), model EI-R04M.
Author: Roman Savochenko <roman@oscada.org>
Sponsored: Vasiliy Grigoriev from "Vacuum technologies laboratory (http://e-beam.ru)".
Version: 1.0.0','','',1,'',0,'','JavaLikeCalc.JavaScript
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
io.setText(resp.slice(4,resp.length-3));',0,1509283419);
INSERT INTO UserProtocol_uPrt VALUES('VKT7','VKT-7','','','Protocol level of firm "Teplocom" (http://www.teplocom.spb.ru) computer "VKT-7" for complex heat measurement and counting. The device complex enough to provide more parameters, more history and accessed by a nonlinear Serial-based protocol at low speed. The template implements acquisition for all significant parameters, gets for their history by hours, days and result months. Also you can to append easily enough for processing of the remained parameters.
Author: Roman Savochenko <roman@oscada.org>
Sponsored: Vladislav Chubuk
Version: 1.0.0','','',1,'',0,'','JavaLikeCalc.JavaScript
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
io.setText(resp.slice(1,-2));',0,1509283495);
INSERT INTO UserProtocol_uPrt VALUES('TM510x','Elemer TM510x','','','Protocol level of multichannels thermometer Elemer TM5102 and TM5103 of firm Elemer (http://www.elemer.ru).
Author: Roman Savochenko <roman@oscada.org>
Version: 1.0.0','','',1,'',0,'','JavaLikeCalc.JavaScript
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
io.setText(resp.slice(resp.indexOf(";")+1,KSpos));',0,1509283413);
INSERT INTO UserProtocol_uPrt VALUES('SMDP','Sycon Multi Drop Protocol','','','Protocol level of STM devices for resonant frequency measurement for mass of deposited films attached to its surface by Sycon Instruments, Inc. (http://www.sycon.com)
Author: Roman Savochenko <roman@oscada.org>
Sponsored: Vasiliy Grigoriev from "Vacuum technologies laboratory (http://e-beam.ru)".
Version: 1.0.0','','',1,'',0,'','JavaLikeCalc.JavaScript
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
else io.setAttr("err",""+(10+errCd)+":"+tr("Unknown error."));',0,1509283368);
INSERT INTO UserProtocol_uPrt VALUES('IT3','Temperature measurement IT-3','','','Protocol level of temperature measurement IT-3 from OmskEtalon (http://www.omsketalon.ru).
Author: Roman Savochenko <roman@oscada.org>
Sponsored: Vasiliy Grigoriev from "Vacuum technologies laboratory (http://e-beam.ru)".
Version: 1.2.0','','',1,'',0,'JavaLikeCalc.JavaScript
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

return true;','JavaLikeCalc.JavaScript
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
}',0,1472721683);
INSERT INTO UserProtocol_uPrt VALUES('IVE_452HS_02','IVE-452HS-02','','','Protocol level of power supply of beam-electrons evaporator of "Plasma Tech" Ltd, from Moskov.
Author: Roman Savochenko <roman@oscada.org>
Sponsored: Vasiliy Grigoriev from "Vacuum technologies laboratory (http://e-beam.ru)".
Version: 1.0.0','','',1,'',0,'','JavaLikeCalc.JavaScript
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
io.setText(resp.slice(1,resp.length-1));',0,1509285266);
INSERT INTO UserProtocol_uPrt VALUES('OPTRIS','OPTRIS CT/CTL','','','Protocol level of OPTRIS CT/CTL communication interface of "Optris" GmbH, from Berlin.
Author: Roman Savochenko <roman@oscada.org>
Sponsored: Vasiliy Grigoriev from "Vacuum technologies laboratory (http://e-beam.ru)".
Version: 1.0.0','','',1,'',0,'','JavaLikeCalc.JavaScript
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
io.setText(resp);',0,1509286719);
INSERT INTO UserProtocol_uPrt VALUES('CTR','CTR 100, 101','','','Protocol part of the RS232C Serial Interface permits the communication between the digital Oerlikon Leybold Vacuum CERAVAC, from Köln.
Author: Roman Savochenko <roman@oscada.org>
Sponsored: Vasiliy Grigoriev from "Vacuum technologies laboratory (http://e-beam.ru)".
Version: 1.0.0','Protocol part of the RS232C Serial Interface permits the communication between the digital Oerlikon Leybold Vacuum CERAVAC, from Köln.
Author: Roman Savochenko <roman@oscada.org>
Sponsored: Vasiliy Grigoriev from "Vacuum technologies laboratory (http://e-beam.ru)".','',1,'',0,'','JavaLikeCalc.JavaScript
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
io.setText(resp.slice(1,resp.length-1));',0,1509287419);
INSERT INTO UserProtocol_uPrt VALUES('SMTP','SMTP (replaced)','SMTP','','!!!!: Replaced by the complex template Main.ntf. Will be removed soon
Provides EMAIL sending by the protocol SMTP.
Author: Roman Savochenko <roman@oscada.org>
Version: 1.0.0','','',1,'',0,'','JavaLikeCalc.JavaScript
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
if(!rez.length)	{ io.setAttr("err", "100:"+tr("No response.")); return; }
for(off = 0; (sit=rez.parse(0,"\x0D\x0A",off)).length; )
	if(sit.toInt() != 250)	{ io.setAttr("err", "101:"+tr("Error HELLO: %1.").replace("%1",sit)); return; }
	else if(sit.indexOf("AUTH LOGIN") >= 0) {
		if(sit.indexOf("PLAIN") < 0)	{ io.setAttr("err", "101:"+tr("Unsupported auth methods: %1.").replace("%1",sit)); return; }
		else if(!auth.length)				{ io.setAttr("err", "101:"+tr("Auth required: %1.").replace("%1",sit)); return; }
	}

//Send AUTH
if(auth.length) {
	rez = tr.messIO("AUTH LOGIN\x0D\x0A");
	while(rez.length && rez.slice(-2) != "\x0D\x0A" && (trez=tr.messIO("")).length) rez += trez;
	if(!rez.length)	{ io.setAttr("err", "100:"+tr("No response.")); return; }
	if(rez.toInt() != 334)	{ io.setAttr("err", "102:"+tr("AUTH error: %1.").replace("%1",rez)); return; }

	rez = tr.messIO(SYS.strEncode(io.attr("auth").parse(0,":"),"Base64")+"\x0D\x0A");
	while(rez.length && rez.slice(-2) != "\x0D\x0A" && (trez=tr.messIO("")).length) rez += trez;
	if(!rez.length)	{ io.setAttr("err", "100:"+tr("No response.")); return; }
	if(rez.toInt() != 334)	{ io.setAttr("err", "102:"+tr("AUTH USER error: %1.").replace("%1",rez)); return; }

	rez = tr.messIO(SYS.strEncode(io.attr("auth").parse(1,":"),"Base64")+"\x0D\x0A");
	while(rez.length && rez.slice(-2) != "\x0D\x0A" && (trez=tr.messIO("")).length) rez += trez;
	if(!rez.length)	{ io.setAttr("err", "100:"+tr("No response.")); return; }
	if(rez.toInt() != 235)	{ io.setAttr("err", "102:"+tr("AUTH PASS error: %1.").replace("%1",rez)); return; }
}

//Send FROM
rez = tr.messIO("MAIL FROM:<"+io.attr("from")+">\x0D\x0A");
while(rez.length && rez.slice(-2) != "\x0D\x0A" && (trez=tr.messIO("")).length) rez += trez;
if(!rez.length)	{ io.setAttr("err", "100:"+tr("No response.")); return; }
if(rez.toInt() != 250)	{ io.setAttr("err", "103:"+tr("MAIL FROM error: %1.").replace("%1",rez)); return; }

//Send TO
rez = tr.messIO("RCPT TO:<"+io.attr("to")+">\x0D\x0A");
while(rez.length && rez.slice(-2) != "\x0D\x0A" && (trez=tr.messIO("")).length) rez += trez;
if(!rez.length)	{ io.setAttr("err", "100:"+tr("No response.")); return; }
if(rez.toInt() != 250)	{ io.setAttr("err", "104:"+tr("RCPT TO error: %1.").replace("%1",rez)); return; }

//Prepare and send DATA
rez = tr.messIO("DATA\x0D\x0A");
while(rez.length && rez.slice(-2) != "\x0D\x0A" && (trez=tr.messIO("")).length) rez += trez;
if(!rez.length)	{ io.setAttr("err", "100:"+tr("No response.")); return; }
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
if(!rez.length)	{ io.setAttr("err", "100:"+tr("No response.")); return; }
if(rez.toInt() != 250)	{ io.setAttr("err", "105:"+tr("DATA send error: %1.").replace("%1",rez)); return; }

//Send QUIT
rez = tr.messIO("QUIT\x0D\x0A");
while(rez.length && rez.slice(-2) != "\x0D\x0A" && (trez=tr.messIO("")).length) rez += trez;

//SYS.messInfo("SMTP", "Response: "+rez);

//Disconnect from the SMTP-server
tr.start(false);',0,1550073190);
INSERT INTO UserProtocol_uPrt VALUES('AT24CXX','I2C: AT24C{32|64}','','','Provides operations with EEPROM memory based on I2C chips AT24C32 (4KB) and AT24C64 (8KB). Supported random reading and writing.
Author: Roman Savochenko <roman@oscada.org>
Version: 1.0.0','','',1,'',0,'','JavaLikeCalc.JavaScript
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
}',0,1479654824);
INSERT INTO UserProtocol_uPrt VALUES('m200','Mercury 200','Меркурій 200','Меркурий 200','Protocol level of one phase counter of electricity Mercury 200, 203.2Т, 206 from firm Incotex (http://www.incotexcom.ru).
Author: Arsen Zakojan <godzilla919@gmail.com>
Version: 1.0.0','','Протокольный уровень однофазных счетчиков электроэнергии Меркурий 200,  203.2Т,  206 компании Инкотекс (http://www.incotexcom.ru).
Автор: Арсен Закоян <godzilla919@gmail.com>
Версия 1.0',1,'',0,'','JavaLikeCalc.JavaScript
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
',1,1509290174);
INSERT INTO UserProtocol_uPrt VALUES('m230','Mercury 230','Меркурій 230','Меркурий 230','Protocol level of three phase counter of electricity Mercury 230, 231, 232, 233, 234, 236 from firm Incotex (http://www.incotexcom.ru).
Author: Arsen Zakojan <godzilla919@gmail.com>
Version: 1.0.0','','Протокольный уровень трехфазных счетчиков электроэнергии Меркурий 230, 231, 232, 233,  234,  236  компании Инкотекс (http://www.incotexcom.ru).
Автор: Арсен Закоян <godzilla919@gmail.com>
Версия 1.0',1,'',0,'','JavaLikeCalc.JavaScript
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
;}',1,1509290179);
INSERT INTO UserProtocol_uPrt VALUES('NIK2303','NIK2303','','','Protocol level of three phase counter of electricity NIK 2303 from firm NIK LLC (http://www.nik.net.ua).
Author: Ruslan Yarmoliuk <rylio74@gmail.com>
Version: 1.0.1','','',1,'',0,'','JavaLikeCalc.JavaScript
//Protocol NIK 2303
io.setAttr("err", "0");

SN = io.attr("SN").toInt();
cli = 0x21;
trm1 = (SN>>20)&0xFE;
trm2 = (SN>>13)&0xFE;
dnum1 = (SN>>6)&0xFE;;
dnum2 = ((SN&0x3FFF)<<1)-(dnum1<<7)+1;
//Message creation
if(!io.text().length) request = SYS.strFromCharCode(0xA0, 10, trm1, trm2, dnum1, dnum2, cli, io.attr("cntr"));
else {
	title = SYS.strFromCharCode(0xA0, io.text().length +12, trm1, trm2, dnum1, dnum2, cli, io.attr("cntr"));
	// Check for HCS
	for(HCS = 0xFFFF, i = 0; i < title.length; i++) {
		b = 0;
		for (j = 0; j<8; j++){				//byte reversion
			bit = (title.charCodeAt(i)&(1<<j))>>j;
			b = b |( bit << (8 - j - 1));
		} 
		for(j = 0; j < 8; j++, b = b << 1)		//HCS calculation
			HCS = ((b^(HCS>>8)&0x80)?(HCS<<1)^0x1021:HCS<<1) & 0xFFFF;
	}
	HCS = HCS ^ 0xFFFF; // HCS*HOR
	rev = 0;					// HCS reversion
	for (i = 0; i<16; i++){
		bit = (HCS&(1<<i))>>i;
		rev = rev |( bit << (16 - i - 1));
	} 
	HCS = rev;
	request = title + SYS.strFromCharCode(HCS&0xFF, (HCS>>8)&0xFF)+ io.text();
}
// Check for FCS
for(FCS = 0xFFFF, i = 0; i < request.length; i++) {
	b = 0;
	for (j = 0; j<8; j++){				//byte reversion
		bit = (request.charCodeAt(i)&(1<<j))>>j;
		b = b |( bit << (8 - j - 1));
	} 
	for(j = 0; j < 8; j++, b = b << 1)		//CRC calculation
		FCS = ((b^(FCS>>8)&0x80)?(FCS<<1)^0x1021:FCS<<1) & 0xFFFF;
}
FCS = FCS ^ 0xFFFF; // FCS*HOR
rev = 0;			// FCS reversion
for (i = 0; i<16; i++){
	bit = (FCS&(1<<i))>>i;
	rev = rev |( bit << (16 - i - 1));
} 
FCS = rev;
request = SYS.strFromCharCode(0x7E) + request + SYS.strFromCharCode(FCS&0xFF, (FCS>>8)&0xFF, 0x7E);
//SYS.messDebug("/NIK2303I/PRT", "Request: "+SYS.strDecode(request, "Bin","|"));


//message sending
resp = tr.messIO(request);
while(resp.length && resp.charCodeAt(resp.length-1) != 0x7E)
	if((tresp=tr.messIO("")).length) resp += tresp;
	else break;
if(resp.length == 0) {io.setText(""); io.setAttr("err","1:"+tr("No a respond")); return; }
//SYS.messDebug("/NIK2303I/PRT", "Respond: "+SYS.strDecode(resp, "Bin","|"));
if(resp.length <= 3 || resp.charCodeAt(2) != resp.length-2)
{ io.setAttr("err","3:"+tr("Respond too short or wrong")); return; }
// Remove markers
resp = resp.slice(1, resp.length-1);
// Check for FCS
for(FCS = 0xFFFF, i = 0; i < resp.length-2; i++) {
	b = 0;
	for (j = 0; j<8; j++){				//byte reversion
		bit = (resp.charCodeAt(i)&(1<<j))>>j;
		b = b |( bit << (8 - j - 1));
	} 
	for(j = 0; j < 8; j++, b = b << 1)		//FCS calculation
		FCS = ((b^(FCS>>8)&0x80)?(FCS<<1)^0x1021:FCS<<1) & 0xFFFF;
}

FCS = FCS ^ 0xFFFF; // FCS*HOR
rev = 0;			// FCS reversion
for (i = 0; i<16; i++){
	bit = (FCS&(1<<i))>>i;
	rev = rev |( bit << (16 - i - 1));
} 
FCS = rev;
if(FCS != ((resp.charCodeAt(resp.length-1)<<8)+ resp.charCodeAt(resp.length-2))) {
	io.setAttr("err", "11:"+tr("Error FCS."));
	return;
}
io.setText(resp.slice(10,resp.length-2));
io.setAttr("cntr", resp[7]);
io.setAttr("err", "0");',1,1512240522);
CREATE TABLE IF NOT EXISTS 'techApp' ("ID" TEXT DEFAULT '' ,"NAME" TEXT DEFAULT '' ,"uk#NAME" TEXT DEFAULT '' ,"ru#NAME" TEXT DEFAULT '' ,"DESCR" TEXT DEFAULT '' ,"uk#DESCR" TEXT DEFAULT '' ,"ru#DESCR" TEXT DEFAULT '' ,"START" INTEGER DEFAULT '1' ,"MAXCALCTM" INTEGER DEFAULT '10' ,"PR_TR" INTEGER DEFAULT '0' ,"FORMULA" TEXT DEFAULT '' ,"TIMESTAMP" INTEGER DEFAULT '0' , PRIMARY KEY ("ID"));
INSERT INTO techApp VALUES('lag','Lag','Затримка','Запаздывание','Lag model. Can be used for lag imitation of the sensor variables.','Модель затримки. Може використовуватися для імітації запізнення значень давачів.','Модель задержки. Может использоваться для имитации запаздывания значений датчиков.',1,10,0,'out -= (out-in)/(t_lg*f_frq);',1556609878);
INSERT INTO techApp VALUES('noise','Noise: 2 harmonic + rand','Шум: 2 гармоніки + випадковий.','Шум: 2 гармоники + случайное','Noise model. Contains three parts:
- first harmonic part;
- second harmonic part;
- noise based on generator of the randomize numbers.','Модель шуму. Містить три складові:
- перша гармоніка;
- друга гармоніка;
- шум на основі генератору випадкових чисел.','Модель шума. Содержит три составляющие:
- первая гармоника;
- вторая гармоника;
- шум на основе генератора случайных чисел.',1,10,0,'tmp_g1 = (tmp_g1 > 6.28) ? 0 : tmp_g1+6.28/(per_g1*f_frq);
tmp_g2 = (tmp_g2 > 6.28) ? 0 : tmp_g2+6.28/(per_g2*f_frq);
out = off + a_g1*sin(tmp_g1) + a_g2*sin(tmp_g2) + a_rnd*(rand(2)-1);',1556609878);
INSERT INTO techApp VALUES('ballCrane','Ball crane','Кульовий кран','Шаровый кран','Model of the ball crane. Includes for the going and estrangement time.','Модель кульового крану. Включає час ходу та час відриву.','Модель шарового крана. Включает время хода и время отрыва.',1,10,0,'if(!(st_close && !com) && !(st_open && com)) {
  tmp_up = (pos > 0 && pos < 100) ? 0 : (tmp_up>0&&lst_com==com)?tmp_up-1/f_frq:t_up;
  pos += (tmp_up > 0) ? 0 : (100*(com?1:-1))/(t_full*f_frq);
  pos = (pos > 100) ? 100 : (pos<0)?0:pos;
  st_open = (pos >= 100) ? true : false;
  st_close = (pos <= 0) ? true : false;
  lst_com = com;
}',1556609878);
INSERT INTO techApp VALUES('separator','Separator','Сепаратор','Сепаратор','Separator model with two phases, liquid and gas.','Модель сепаратору з двома фазами, рідинною та газовою.','Модель сепаратора с двумя фазами, жидкой и газовой.',1,10,0,'Flq = max(0, Fi*PercLq);
DAQ.JavaLikeCalc.lib_techApp.pipeBase(Fi, Pi, 293, Si, Fo+Flq, Po, 293, So, lo, Q0, 0.95, 0.01, f_frq);
Llq = max(0, min(100,Llq+0.27*(Flq-Fo_lq)/(Vap*Qlq*f_frq)));
Po_lq = Po + Llq*Vap/Qlq;',1556609878);
INSERT INTO techApp VALUES('valve','Valve','Клапан','Клапан','Valve model, includes:
- two valves in the one;
- super-critical speed;
- temperature changing at the throttling;
- work in one direction, back valve;
- speed control of the position changing;
- nonlinearity of the intersection from the position.','Модель клапану яка враховує:
- два клапани в одному;
- надкритичне витікання;
- зміна температури при дроселюванні;
- робота тільки у одному напрямку, зворотний клапан;
- керування швидкісттю зміни положення;
- нелінійність прохідного перетину від положення.','Модель клапана, учитывающая:
- два клапана в одном;
- сверхкритическое истечение;
- изменение температуры при дросселировании;
- работа только в одном направлении, обратный клапан;
- управляемая скорость изменения положения;
- нелинейность проходного сечения от положения.',1,10,0,'Qr = Q0+Q0*Kpr*(Pi-1);
tmp_l1 += (abs(l_v1-tmp_l1) > 5) ? 100*sign(l_v1-tmp_l1)/(t_v1*f_frq) : (l_v1-tmp_l1)/(t_v1*f_frq);
tmp_l2 += (abs(l_v2-tmp_l2) > 5) ? 100*sign(l_v2-tmp_l2)/(t_v2*f_frq) : (l_v2-tmp_l2)/(t_v2*f_frq);
Sr = (S_v1*pow(tmp_l1,Kln)+S_v2*pow(tmp_l2,Kln))/pow(100,Kln);

DAQ.JavaLikeCalc.lib_techApp.pipeBase(Fi, Pi, Ti, Sr, EVAL_REAL, Po, 293, So, lo, Q0, Kpr, 0.01, f_frq);
if(noBack) Fi = max(0, Fi);
Po = max(0, min(100,Po+0.27*(Fi-Fo)/(Q0*Kpr*So*lo*f_frq)));

To = max(0, min(2e3,To+(abs(Fi)*(Ti*pow(Po/Pi,0.02)-To)+(Fwind+1)*(Twind-To)/Riz)/(Ct*So*lo*Qr*f_frq)));',1556609878);
INSERT INTO techApp VALUES('lagClean','Lag: clean','Затримка: чиста','Задержка: чистая','Model of clean(transportable) lag. Realizes by incorporating several links of a simple delay. Appointed for lags into long pipes.','Модель чистої(транспортної) затримки. Реалізується шляхом включення декількох ланок простої затримки. Призначено для імітації затримок у довгих трубопроводах.','Модель чистого(транспортного) запаздывания. Реализуется путём включения нескольких простых звеньев запаздывания. Предназначен для имитации запаздывания в длинных трубопроводах.',1,10,0,'cl1 -= (cl1-in)/(t_lg*f_frq/4);
cl2 -= (cl2-cl1)/(t_lg*f_frq/4);
cl3 -= (cl3-cl2)/(t_lg*f_frq/4);
out -= (out-cl3)/(t_lg*f_frq/4);',1556609878);
INSERT INTO techApp VALUES('net','Network: load','Мережа: навантаження','Сеть: нагрузка','Loading with constant pressure on the network. Contains a parameter for connection the noise.','Навантаження з фіксованим тиском мережі. Містить параметр для підключення шуму.','Нагрузка с фиксированным давлением сети. Содержит параметр для подключения шума.',1,10,0,'DAQ.JavaLikeCalc.lib_techApp.pipeBase(Fi, Pi, 293, So, EVAL_REAL, Po, 293, So, 10, Q0, Kpr, 0.01, f_frq);',1556609878);
INSERT INTO techApp VALUES('src_press','Source: pressure','Джерело: тиск','Источник: давление','Source of the constant pressure. Contains a parameter for connection the noise.','Джерело з фіксованим тиском. Містить параметр для підключення шуму.','Источник с фиксированным давлением. Содержит параметр для подключения шума.',1,10,0,'DAQ.JavaLikeCalc.lib_techApp.pipeBase(Fit, Pi*Noise, 293, So, Fo, Po, 293, So, lo, Q0, Kpr, 0.01, f_frq);',1556609878);
INSERT INTO techApp VALUES('cooler','Air cooler','Повітряний холодильник','Воздушный холодильник','Model of the air cooler for gas flow.','Модель повітряного охолоджувача газового потоку.','Модель воздушного охладителя газового потока.',1,10,0,'DAQ.JavaLikeCalc.lib_techApp.pipeBase(Fi, Pi, 293, Si, Fo, Po, 293, So, lo, Q0, 0.95, 0.01, f_frq);
Qr = Q0 + Q0*0.95*(Pi-1);
To += (Fi*(Ti-To)+Wc*(Tair-To)/Rt)/(Ct*(Si*li+So*lo)*Qr*f_frq);',1556609878);
INSERT INTO techApp VALUES('compressor','Gas compressor','Компресор газовий','Компрессор газовый','Model of the gas compressor. Implements the surge effect.
The surge counts from the dynamic-gas curve, and next there counts the surge margin coefficient.','Модель газового компресора. Враховує ефект помпажу.
Помпаж розраховується за газово-динамічною кривою, виходячи з якої розраховується коефіцієнт запасу за помпажем.','Модель газового компрессора. Учитывает эффект помпажа. Помпаж вычисляется по газо-динамической кривой, исходя из которой получается коэффициент запаса по помпажу.',1,10,0,'Pmax = max(Pi, Po);
Pmin = min(Pi, Po);
Qr = Q0 + Q0*Kpr*(Pi-1);
Qrf = Q0 + Q0*Kpr*(Pmax-1);
Ftmp = (N > 0.1) ? (1-10*(Po-Pi)/(Qr*(pow(N,3)+0.1)*Kpmp)) : 1;
Kmrg = 1-Ftmp;  //The margin coefficient
Fi = V*N*Qr*sign(Ftmp)*pow(abs(Ftmp),Kslp)+
     0.3*(4*So*Qrf/(0.01*lo*1.7724+4*Qrf))*sign(Pi-Po)*pow(Qrf*(Pmax-max(Pmax*0.528,Pmin)),0.5);
Fit -= (Fit-Fi)/max(1,(lo*f_frq)/max(1e-4,abs(Fi/(Qrf*So))));
Po = max(0, min(100,Po+0.27*(Fi-Fo)/(Q0*Kpr*So*lo*f_frq)));

To += (abs(Fi)*(Ti*pow(Po/Pi,0.3)-To)+(Fwind+1)*(Twind-To)/Riz)/(Ct*(V+So*lo)*Qr*f_frq);',1556609878);
INSERT INTO techApp VALUES('src_flow','Source: flow','Джерело: витрати','Источник: расход','Source of the constant flow. Contains a parameter for connection the noise.','Джерело з фіксованими витратами. Містить параметр для підключення шуму.','Источник с фиксированным расходом. Содержит параметр для подключения шума.',1,10,0,'Po = max(0, min(100,Po+0.27*(Noise*Fi-Fo)/(Q0*Kpr*So*lo*f_frq)));',1556609878);
INSERT INTO techApp VALUES('pipe1_1','Pipe 1->1','Труба 1->1','Труба 1->1','Model of the pipe by the scheme "1 -> 1".','Модель вузла труб за схемою "1 -> 1".','Модель узла труб по схеме "1 -> 1".',1,10,0,'DAQ.JavaLikeCalc.lib_techApp.pipeBase(Fi, Pi, 293, So, Ft1, Pti, 293, So, 0.33*lo, Q0, Kpr, 0.01, f_frq);
DAQ.JavaLikeCalc.lib_techApp.pipeBase(Ft1, Pti, 293, So, Fto, Pt1, 293, So, 0.33*lo, Q0, Kpr, 0.01, f_frq);
DAQ.JavaLikeCalc.lib_techApp.pipeBase(Fto, Pt1, 293, So, Fo, Po, 293, So, 0.33*lo, Q0, Kpr, 0.01, f_frq);',1556609878);
INSERT INTO techApp VALUES('pipe1_2','Pipe 1->2','Труба 1->2','Труба 1->2','Model of the pipe by the scheme "1 -> 2".','Модель вузла труб за схемою "1 -> 2".','Модель узла труб по схеме "1 -> 2".',1,10,0,'DAQ.JavaLikeCalc.lib_techApp.pipeBase(F1tmp, Pi, 293, So1, Fo1, Po1, 293, So1, lo1, Q0, Kpr, 0.01, f_frq);
DAQ.JavaLikeCalc.lib_techApp.pipeBase(F2tmp, Pi, 293, So2, Fo2, Po2, 293, So2, lo2, Q0, Kpr, 0.01, f_frq);
Fi = F1tmp + F2tmp;',1556609878);
INSERT INTO techApp VALUES('pipe1_3','Pipe 1->3','Труба 1->3','Труба 1->3','Model of the pipe by the scheme "1 -> 3".','Модель вузла труб за схемою "1 -> 3".','Модель узла труб по схеме "1 -> 3".',1,10,0,'DAQ.JavaLikeCalc.lib_techApp.pipeBase(F1tmp, Pi, 293, So1, Fo1, Po1, 293, So1, lo1, Q0, Kpr, 0.01, f_frq);
DAQ.JavaLikeCalc.lib_techApp.pipeBase(F2tmp, Pi, 293, So2, Fo2, Po2, 293, So2, lo2, Q0, Kpr, 0.01, f_frq);
DAQ.JavaLikeCalc.lib_techApp.pipeBase(F3tmp, Pi, 293, So3, Fo3, Po3, 293, So3, lo3, Q0, Kpr, 0.01, f_frq);
Fi = F1tmp + F2tmp + F3tmp;',1556609878);
INSERT INTO techApp VALUES('pipe1_4','Pipe 1->4','Труба 1->4','Труба 1->4','Model of the pipe by the scheme "1 -> 4".','Модель вузла труб за схемою "1 -> 4".','Модель узла труб по схеме "1 -> 4".',1,10,0,'DAQ.JavaLikeCalc.lib_techApp.pipeBase(F1tmp, Pi, 293, So1, Fo1, Po1, 293, So1, lo1, Q0, Kpr, 0.01, f_frq);
DAQ.JavaLikeCalc.lib_techApp.pipeBase(F2tmp, Pi, 293, So2, Fo2, Po2, 293, So2, lo2, Q0, Kpr, 0.01, f_frq);
DAQ.JavaLikeCalc.lib_techApp.pipeBase(F3tmp, Pi, 293, So3, Fo3, Po3, 293, So3, lo3, Q0, Kpr, 0.01, f_frq);
DAQ.JavaLikeCalc.lib_techApp.pipeBase(F4tmp, Pi, 293, So4, Fo4, Po4, 293, So4, lo4, Q0, Kpr, 0.01, f_frq);
Fi = F1tmp + F2tmp + F3tmp + F4tmp;',1556609878);
INSERT INTO techApp VALUES('valveMech','Valve: processing mechanism','Клапан: виконавчий механізм','Клапан: исполнительный механизм','Model of the valve processing mechanism. Includes the going and estrangement time.','Модель виконавчого механізму клапана. Включає час ходу та час відриву.','Модель исполнительного механизма клапана. Включает время хода и время отрыва.',1,10,0,'if((pos >= 99 && com >= 99) || (pos <= 1 && com <= 1)) {
  tmp_up = t_up;
  if( pos >= 99) { pos = 100; st_open = true; }
  else { pos = 0; st_close = true; }
}
else if(tmp_up > 0) tmp_up -= 1/f_frq;
else {
  st_open = st_close = false;
  lst_com += (com-lst_com)/(0.5*t_full*f_frq);
  pos += (lst_com-pos)/(0.5*t_full*f_frq);
}
pos_sensor += (pos-pos_sensor)/(t_sensor*f_frq);',1556609878);
INSERT INTO techApp VALUES('diaphragm','Diaphragm','Діафрагма','Диафрагма','Diaphragm model.','Модель діафрагми.','Модель диафрагмы.',1,10,0,'DAQ.JavaLikeCalc.lib_techApp.pipeBase(Fi, Pi, 293, Sdf, Fo, Po, 293, So, lo, Q0, Kpr, 0.01, f_frq);
dP -= (dP-100*(Pi-Po))/f_frq;',1556609878);
INSERT INTO techApp VALUES('pipe3_1','Pipe 3->1','Труба 3->1','Труба 3->1','Model of the pipe by the scheme "3 -> 1".','Модель вузла труб за схемою "3 -> 1".','Модель узла труб по схеме "3 -> 1".',1,10,0,'DAQ.JavaLikeCalc.lib_techApp.pipeBase(Fi1, Pi1, 293, Si1, EVAL_REAL, Po, 293, So, lo, Q0, Kpr, 0.01, f_frq);
DAQ.JavaLikeCalc.lib_techApp.pipeBase(Fi2, Pi2, 293, Si2, EVAL_REAL, Po, 293, So, lo, Q0, Kpr, 0.01, f_frq);
DAQ.JavaLikeCalc.lib_techApp.pipeBase(Fi3, Pi3, 293, Si3, EVAL_REAL, Po, 293, So, lo, Q0, Kpr, 0.01, f_frq);
Po = max(0, min(100,Po+0.27*(Fi1+Fi2+Fi3-Fo)/(Q0*Kpr*So*lo*f_frq)));
To = max(0, To+(Fi1*(Ti1-To)+Fi2*(Ti2-To)+Fi3*(Ti3-To)+(Fwind+1)*(Twind-To)/Riz)/(Ct*So*lo*Q0*f_frq));',1556609878);
INSERT INTO techApp VALUES('pipeBase','Pipe-base','Труба-база','Труба-база','Implementation of the basic foundations of the pipe model:
- flow in the pipe, taking into account: the speed, pressure difference, resistance due to friction and the critical flow;
- calculation of the pressure;
- accounting for the environment density and degree of the compressibility for both gases and liquids.','Реалізація базових основ моделі труби:
- витрати у трубі з урахуванням: швидкості руху, різниці тиску, опору за рахунок тертя та критичної течі;
- розрахунок тиску;
- урахування щільності середовища та ступеня його стиснення як для газів, так і рідин.','Реализация базовых основ модели трубы:
- расход в трубе с учётом: скорости движения, перепада давления, сопротивления за счёт трения и критического истечения;
- расчёт давления;
- учёт плотности среды и степени её сжимаемости как для газов, так и для жидкостей.',1,10,0,'Pmax = max(Pi,Po);
Pmin = min(Pi, Po);
Qr = Q0+Q0*Kpr*(Pmax-1);
Fit = 630*(4*Si*So*Qr/(Ktr*lo*1.7724*Si+4*So*Qr))*sign(Pi-Po)*pow(Qr*(Pmax-max(Pmax*0.528,Pmin)),0.5);
Fi -= (Fi-Fit)/max(1,(lo*f_frq)/max(1,abs(Fit/(Qr*So))));
if(!Fo.isEVal()) Po = max(0, min(100,Po+0.27*(Fi-Fo)/(Q0*Kpr*So*lo*f_frq)));',1556609878);
INSERT INTO techApp VALUES('heatExch','Heat exchanger','Теплообмінник','Теплообменник','Model of the heat exchanger, calculating the heat exchange of the two streams.','Модель теплообміннику, що розраховує теплообмін двох потоків.','Модель теплообменника, рассчитывающая теплообмен двух потоков.',1,10,0,'DAQ.JavaLikeCalc.lib_techApp.pipeBase(Fi1, Pi1, Ti1, Si1, Fo1, Po1, 293, So1, lo1, Q0i1, Kpr1, 0.01, f_frq);
DAQ.JavaLikeCalc.lib_techApp.pipeBase(Fi2, Pi2, Ti2, Si2, Fo2, Po2, 293, So2, lo2, Q0i2, Kpr2, 0.01, f_frq);

To1 = max(0, min(1e4,(Fi1*Ti1*Ci1+ki*Fi2*Ti2*Ci2)/(Fi1*Ci1+ki*Fi2*Ci2)));
To2 = max(0, min(1e4,(ki*Fi1*Ti1*Ci1+Fi2*Ti2*Ci2)/(ki*Fi1*Ci1+Fi2*Ci2)));',1556609878);
INSERT INTO techApp VALUES('boilerBarrel','Boiler: barrel','Котел: барабан','Котёл: барабан','The model of the boiler''s barrel. ','Модель барабану котлоагрегату.','Модель барабана котлоагрегата.',1,10,0,'// Water
DAQ.JavaLikeCalc.lib_techApp.pipeBase(Fi1, Pi1, 293, Si1, EVAL_REAL, Po1, 293, So1, lo1, 1e3, 0.001, 0.01, f_frq);
Fi1 = max(0, Fi1);

// Steam
Lo = max(0, min(100,Lo+(Fi1-Fstm)*100/(Vi1*1000*f_frq)));
To1 = (100*pow(Po1,0.241)+5) + 273;

if(Tv < To1) {
  Tv += (k*S*(Ti2-Tv)-Fi1*0.00418*(Tv-Ti1))/f_frq;
  Fstm = 0;
}
if(Tv >= To1) {
  Tv = To1;
  Lambda = 2750-0.00418*(Tv-273);
  Fstm = (5*S*Fi2*(Ti2-Tv)-Fi1*0.00418*(Tv-Ti1))/(Po1*Lambda);
}

To2 = Ti2-Tv/k;
Po1 = max(0, min(100,Po1+0.27*(Fstm-Fo)/(1.2*0.98*((1-Lo/100)*Vi1+So1*lo1)*f_frq)));

// Smoke gas
DAQ.JavaLikeCalc.lib_techApp.pipeBase(Fi2, Pi2, 293, Si2, Fo2, Po2, 293, Si2, 30, 1.2, 0.98, 0.01, f_frq);',1556609878);
INSERT INTO techApp VALUES('boilerBurner','Boiler: burner','Котел: топка','Котёл: топка','The fire-chamber model of the boiler unit, which operates on three types of fuel, initially is: blast-furnace, coke and natural gases.','Модель топки котлоагрегату, який працює на трьох видах палива, початково це: доменний, коксовий та природний гази.','Модель топки котлоагрегата, работающего на трех видах топлива, исходно это: доменный, коксовый и природный газы.',1,10,0,'using DAQ.JavaLikeCalc.lib_techApp;
pipeBase(Fi1, Pi1, Ti1, Si1, EVAL_REAL, Po, 293, So, lo, 1.2, 0.95, 0.01, f_frq); Fi1 = max(0, Fi1);
pipeBase(Fi2, Pi2, Ti2, Si2, EVAL_REAL, Po, 293, So, lo, 0.7, 0.95, 0.01, f_frq); Fi2 = max(0, Fi2);
pipeBase(Fi3, Pi3, Ti3, Si3, EVAL_REAL, Po, 293, So, lo, 1.33, 0.95, 0.01, f_frq); Fi3 = max(0, Fi3);
pipeBase(Fi4, Pi4, Ti4, Si4, EVAL_REAL, Po, 293, So, lo, 1.293, 0.95, 0.01, f_frq); Fi4 = max(0, Fi4);

Neobhod_vzd = Fi1 + 10*Fi2 + 4*Fi3;
F_DG = Fi1 + Fi2 + Fi3 + Fi4;
O2 = max(0, min(100,(Fi4-Neobhod_vzd)*100/F_DG));
CO = min(100, (O2<1) ? (1.2*abs(O2)) : 0);
koef = min(1, Fi4/Neobhod_vzd);
Q = koef*(8050*Fi2+3900*Fi3+930*Fi1);
delta_t = Q/(F_DG*1.047);
To = max(0, min(2000,(delta_t+(Ti4-273)+(Ti3-273)*(Fi3/Fi1)+(Ti2-273)*(Fi2/Fi1)+(Ti1-273)*(Fi1/Fi4))+273));

Po = max(0, min(10,Po+0.27*(F_DG-Fo)/(1.2*0.95*(So*lo+V)*f_frq)));',1556609878);
INSERT INTO techApp VALUES('pipe2_1','Pipe 2->1','Труба 2->1','Труба 2->1','Model of the pipe by the scheme "2 -> 1".','Модель вузла труб за схемою "2 -> 1".','Модель узла труб по схеме "2 -> 1".',1,10,0,'DAQ.JavaLikeCalc.lib_techApp.pipeBase(Fi1, Pi1, 293, Si1, EVAL_REAL, Po, 293, So, lo, Q0, Kpr, 0.01, f_frq);
DAQ.JavaLikeCalc.lib_techApp.pipeBase(Fi2, Pi2, 293, Si2, EVAL_REAL, Po, 293, So, lo, Q0, Kpr, 0.01, f_frq);
Po = max(0, min(100,Po+0.27*(Fi1+Fi2-Fo)/(Q0*Kpr*So*lo*f_frq)));
To = max(0, To+(Fi1*(Ti1-To)+Fi2*(Ti2-To)+(Fwind+1)*(Twind-To)/Riz)/(Ct*So*lo*Q0*f_frq));',1556609878);
CREATE TABLE IF NOT EXISTS 'flb_web' ("ID" TEXT DEFAULT '' ,"NAME" TEXT DEFAULT '' ,"ru#NAME" TEXT DEFAULT '' ,"uk#NAME" TEXT DEFAULT '' ,"DESCR" TEXT DEFAULT '' ,"START" INTEGER DEFAULT '1' ,"MAXCALCTM" INTEGER DEFAULT '10' ,"PR_TR" INTEGER DEFAULT '0' ,"FORMULA" TEXT DEFAULT '' ,"TIMESTAMP" INTEGER DEFAULT '0' , PRIMARY KEY ("ID"));
INSERT INTO flb_web VALUES('alarms','Alarms','Нарушения','Порушення','',1,10,0,'//> Load rules
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
rules = alarmsTree.save();',1377261539);
INSERT INTO flb_web VALUES('devMon','Devices monitor','Монитор устройств','Монітор пристроїв','',1,10,0,'var treeEl = false;

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
//SYS.messDebug("TEST Dev","TEST 00: "+typeof(devLs[formEl["cat"]+"."+itId]));',1377260842);
INSERT INTO flb_web VALUES('devices','Devices dispatcher','Диспетчер устройств','Диспетчер пристроїв','',1,10,0,'var treeEl = false;

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
//SYS.messDebug("TEST Dev","TEST 00: "+typeof(devLs[formEl["cat"]+"."+itId]));',1377260944);
INSERT INTO flb_web VALUES('mess','Alarm messages','Сообщения сигнализаций','Повідомлення сигналізацій','',1,10,0,'var treeEl = false;

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
//SYS.messDebug("TEST Dev","TEST 00: "+typeof(devLs[formEl["cat"]+"."+itId]));',1377261138);
INSERT INTO flb_web VALUES('report','Reports','Отчёты','Звіти','',1,10,0,'var treeEl = false;
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
//SYS.messDebug("TEST Dev","TEST 00: "+typeof(devLs[formEl["cat"]+"."+itId]));',1377261196);
INSERT INTO flb_web VALUES('test','Test','Тест','Тест','',1,10,0,'if(f_start)
{
	var req = SYS.XMLNode("send").setAttr("ProtIt","SMS").setAttr("tel","+380679859815").setText("Тестовое сообщение");
	SYS.Transport.Serial.out_GSM.messIO(req,"UserProtocol");
}',1377261398);
INSERT INTO flb_web VALUES('timerWrite','Write by the timer','Запись по таймеру','Запис за таймером','',1,10,0,'//> Load rules
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
rules = timersTree.save();',1377261462);
INSERT INTO flb_web VALUES('user','Users dispatcher','Диспетчер пользователей','Диспетчер користувачів','',1,10,0,'var treeEl = false;

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
//SYS.messDebug("TEST Dev","TEST 00: "+typeof(devLs[formEl["cat"]+"."+itId]));',1377261537);
CREATE TABLE IF NOT EXISTS 'flb_web_io' ("F_ID" TEXT DEFAULT '' ,"ID" TEXT DEFAULT '' ,"NAME" TEXT DEFAULT '' ,"TYPE" INTEGER DEFAULT '' ,"MODE" INTEGER DEFAULT '' ,"DEF" TEXT DEFAULT '' ,"HIDE" INTEGER DEFAULT '' ,"POS" INTEGER DEFAULT '' ,"ru#NAME" TEXT DEFAULT '' ,"uk#NAME" TEXT DEFAULT '' ,"uk#DEF" TEXT DEFAULT '' ,"ru#DEF" TEXT DEFAULT '' , PRIMARY KEY ("F_ID","ID"));
INSERT INTO flb_web_io VALUES('alarms','rules','Rules',0,1,'\n',0,0,'Правила','Правила','','');
INSERT INTO flb_web_io VALUES('devMon','rez','Result',0,2,'',0,0,'Результат','Результат','','');
INSERT INTO flb_web_io VALUES('devMon','urlPrms','Address parameters',4,0,'',0,1,'Адрес параметров','Адреса параметрів','','');
INSERT INTO flb_web_io VALUES('devMon','tmpl','Template',0,0,'',0,2,'Шаблон','Шаблон','','');
INSERT INTO flb_web_io VALUES('devMon','cnts','POST request''s context',4,0,'',0,3,'Контекст POST-запроса','Контекст POST-запиту','','');
INSERT INTO flb_web_io VALUES('devMon','user','User',0,0,'',0,4,'Пользователь','Користувач','','');
INSERT INTO flb_web_io VALUES('devices','rez','Result',0,2,'',0,0,'Результат','Результат','','');
INSERT INTO flb_web_io VALUES('devices','urlPrms','Address parameters',4,0,'',0,1,'Адрес параметров','Адреса параметрів','','');
INSERT INTO flb_web_io VALUES('devices','tmpl','Template',0,0,'',0,2,'Шаблон','Шаблон','','');
INSERT INTO flb_web_io VALUES('devices','cnts','POST request''s context',4,0,'',0,3,'Контекст POST-запроса','Контекст POST-запиту','','');
INSERT INTO flb_web_io VALUES('devices','user','User',0,0,'',0,4,'Пользователь','Користувач','','');
INSERT INTO flb_web_io VALUES('mess','rez','Result',0,2,'',0,0,'Результат','Результат','','');
INSERT INTO flb_web_io VALUES('mess','urlPrms','Address parameters',4,0,'',0,1,'Адрес параметров','Адреса параметрів','','');
INSERT INTO flb_web_io VALUES('mess','tmpl','Template',0,0,'',0,2,'Шаблон','Шаблон','','');
INSERT INTO flb_web_io VALUES('mess','cnts','POST request''s context',4,0,'',0,3,'Контекст POST-запроса','Контекст POST-запиту','','');
INSERT INTO flb_web_io VALUES('mess','user','User',0,0,'',0,4,'Пользователь','Користувач','','');
INSERT INTO flb_web_io VALUES('report','rez','Result',0,2,'',0,0,'Результат','Результат','','');
INSERT INTO flb_web_io VALUES('report','urlPrms','Address parameters',4,0,'',0,1,'Адрес параметров','Адреса параметрів','','');
INSERT INTO flb_web_io VALUES('report','tmpl','Template',0,0,'',0,2,'Шаблон','Шаблон','','');
INSERT INTO flb_web_io VALUES('report','cnts','POST request''s context',4,0,'',0,3,'Контекст POST-запроса','Контекст POST-запиту','','');
INSERT INTO flb_web_io VALUES('report','user','User',0,0,'',0,4,'Пользователь','Користувач','','');
INSERT INTO flb_web_io VALUES('test','f_start','Start the function flag',3,0,'0',0,0,'Флаг запуска функции','Прапорець запуску функції','','');
INSERT INTO flb_web_io VALUES('test','test','Test',0,1,'',0,1,'Тест','Тест','','');
INSERT INTO flb_web_io VALUES('timerWrite','rules','Rules',0,1,'\n',0,0,'Правила','Правила','','');
INSERT INTO flb_web_io VALUES('user','rez','Result',0,2,'',0,0,'Результат','Результат','','');
INSERT INTO flb_web_io VALUES('user','urlPrms','Address parameters',4,0,'',0,1,'Адрес параметров','Адреса параметрів','','');
INSERT INTO flb_web_io VALUES('user','tmpl','Template',0,0,'',0,2,'Шаблон','Шаблон','','');
INSERT INTO flb_web_io VALUES('user','cnts','POST request''s context',4,0,'',0,3,'Контекст POST-запроса','Контекст POST-запиту','','');
INSERT INTO flb_web_io VALUES('user','user','User',0,0,'',0,4,'Пользователь','Користувач','','');
CREATE TABLE IF NOT EXISTS 'flb_doc_io' ("F_ID" TEXT DEFAULT '' ,"ID" TEXT DEFAULT '' ,"NAME" TEXT DEFAULT '' ,"TYPE" INTEGER DEFAULT '' ,"MODE" INTEGER DEFAULT '' ,"DEF" TEXT DEFAULT '' ,"HIDE" INTEGER DEFAULT '' ,"POS" INTEGER DEFAULT '' ,"uk#NAME" TEXT DEFAULT '' ,"uk#DEF" TEXT DEFAULT '' ,"ru#NAME" TEXT DEFAULT '' ,"ru#DEF" TEXT DEFAULT '' ,"sr#NAME" TEXT DEFAULT '' , PRIMARY KEY ("F_ID","ID"));
INSERT INTO flb_doc_io VALUES('getVal','rez','Result',0,2,'',0,0,'Результат','','Результат','','');
INSERT INTO flb_doc_io VALUES('getVal','addr','Address',0,0,'',0,1,'Адреса','','Адрес','','Адреса');
INSERT INTO flb_doc_io VALUES('getVal','time','Time, seconds',1,0,'',0,2,'Час, секунд','','Время, секунд','','');
INSERT INTO flb_doc_io VALUES('getVal','uTime','Time, microseconds',1,0,'',0,3,'Час, мікросекунди','','Время, микросекунды','','');
INSERT INTO flb_doc_io VALUES('getVal','prec','Signs after dot',1,0,'3',0,4,'Знаків після коми','','Знаков после запятой','','');
INSERT INTO flb_doc_io VALUES('getVal','archtor','Archiver',0,0,'',0,5,'Архіватор','','Архиватор','','');
INSERT INTO flb_doc_io VALUES('sumVal','rez','Result',0,2,'',0,0,'Результат','','Результат','','');
INSERT INTO flb_doc_io VALUES('sumVal','addr','Address',0,0,'',0,1,'Адреса','','Адрес','','Адреса');
INSERT INTO flb_doc_io VALUES('averVal','addr','Address',0,0,'',0,1,'Адреса','','Адрес','','Адреса');
INSERT INTO flb_doc_io VALUES('averVal','rez','Result',0,2,'',0,0,'Результат','','Результат','','');
INSERT INTO flb_doc_io VALUES('sumVal','prec','Signs after dot',1,0,'3',0,4,'Знаків після коми','','Знаков после запятой','','');
INSERT INTO flb_doc_io VALUES('sumVal','archtor','Archiver',0,0,'',0,5,'Архіватор','','Архиватор','','');
INSERT INTO flb_doc_io VALUES('sumVal','bTime','Begin time, seconds',1,0,'',0,2,'Час початку, секунд','','Время начала, секунд','','');
INSERT INTO flb_doc_io VALUES('sumVal','eTime','End time, seconds',1,0,'',0,3,'Час кінця, секунд','','Время конца, секунд','','');
INSERT INTO flb_doc_io VALUES('averVal','bTime','Begin time, seconds',1,0,'',0,2,'Час початку, секунд','','Время начала, секунд','','');
INSERT INTO flb_doc_io VALUES('averVal','eTime','End time, seconds',1,0,'',0,3,'Час кінця, секунд','','Время конца, секунд','','');
INSERT INTO flb_doc_io VALUES('averVal','prec','Signs after dot',1,0,'3',0,4,'Знаків після коми','','Знаков после запятой','','');
INSERT INTO flb_doc_io VALUES('averVal','archtor','Archiver',0,0,'',0,5,'Архіватор','','Архиватор','','');
INSERT INTO flb_doc_io VALUES('getVal','strong','Precisely',3,0,'0',0,6,'Точно','','Точно','','');
INSERT INTO flb_doc_io VALUES('getVal','tryTo','Try value up to, microseconds',1,0,'0',0,7,'Пробувати значення до, мікросекунди','','Пробовать значение до, микросекунды','','');
INSERT INTO flb_doc_io VALUES('averVal','actPerc','Percent of the actual values',2,1,'',0,6,'Відсоток актуальних значень','','Процент актуальных значений','','');
INSERT INTO flb_doc_io VALUES('diagram','res','Result',0,6,'',0,0,'Результат','','Результат','','');
INSERT INTO flb_doc_io VALUES('diagram','end','End time, seconds',2,0,'0',0,1,'Час кінця, секунд','','Время конца, секунд','','');
INSERT INTO flb_doc_io VALUES('diagram','size','Size, seconds',2,0,'3600',0,2,'Розмір, секунд','','Размер, секунд','','');
INSERT INTO flb_doc_io VALUES('diagram','srcs','Sources',0,4,'/DAQ/LogicLev/gen/F3/var:0:100:#ae77c3:2
/Archive/va_CPULoad_load:0:0:darkcyan:2',0,3,'Джерела','','Источники','','');
INSERT INTO flb_doc_io VALUES('diagram','hsz','Horizontal size, pixels',1,0,'600',0,4,'Горизонтальний розмір, пікселів','','Горизонтальный размер, пикселей','','');
INSERT INTO flb_doc_io VALUES('diagram','vsz','Vertical size, pixels',1,0,'400',0,5,'Вертикальний розмір, пікселів','','Вертикальный размер, пикселей','','');
INSERT INTO flb_doc_io VALUES('diagram','tmpl','SVG-template',0,4,'<svg xmlns="http://www.w3.org/2000/svg" version="1.1" sclMarkFont="Arial 10" sclHor="2" sclVer="3" sclColor="gray" sclMarkColor="gray">
    <style type="text/css">
	    #scVer rect { stroke: none; }
	    #scVer text { stroke: none; }
	    #scHor rect { stroke: none; }
	    #scHor text { stroke: none; }
	    #drawArea path { fill: none; }
    </style>
    <!--Draw grid and markers-->
    <g id="scVer" />
    <g id="scHor" />
    <!--Curves area -->
    <g id="drawArea" />
</svg>',0,7,'Шаблон','','Шаблон','','');
INSERT INTO flb_doc_io VALUES('diagram','arch','Archiver',0,0,'',0,6,'Архіватор','','Архиватор','','');
CREATE TABLE IF NOT EXISTS 'Trs' ("base" TEXT DEFAULT '' ,"uk#base" TEXT DEFAULT '' ,"ru#base" TEXT DEFAULT '' ,"sr#base" TEXT DEFAULT '' , PRIMARY KEY ("base"));
INSERT INTO Trs VALUES('No data or connection with source','Немає даних або зв''язку із джерелом','Нет данных или связи с источником','');
INSERT INTO Trs VALUES('The signal exceed to upper hardware border','Сигнал перевищив верхню апаратну границю','Выход сигнала за верхнюю аппаратную границу','');
INSERT INTO Trs VALUES('The signal exceed to bottom hardware border','Сигнал перевищив нижню апаратну границю','Выход сигнала за нижнюю аппаратную границу','');
INSERT INTO Trs VALUES('Upper alarm border error','Помилка верхньої аварійної границі','Нарушение верхней аварийной границы','');
INSERT INTO Trs VALUES('Lower alarm border error','Помилка нижньої аварійної границі','Нарушение нижней аварийной границы','');
INSERT INTO Trs VALUES('Upper warning border error','Помилка верхньої поперджув. границі','Нарушение верхней предупредительной границы','');
INSERT INTO Trs VALUES('Lower warning border error','Помилка нижньої поперджув. границі','Нарушение нижней предупредительной границы','');
INSERT INTO Trs VALUES('Too big parameter''s motion speed','Дуже велика швидкість зміни параметру','Очень большая скорость изменения параметра','');
INSERT INTO Trs VALUES('NORM','НОРМА','НОРМА','');
INSERT INTO Trs VALUES('Step missed','Крок пропущено','Шаг пропущен','');
INSERT INTO Trs VALUES('Program terminated','Програму перервано','Программа прервана','');
INSERT INTO Trs VALUES('Terminated program session by the user','Перерваний користувачем сеанс програми','Прерванный пользователем сеанс программы','');
INSERT INTO Trs VALUES('Terminated program session by the error','Перерваний помилкою сеанс програми','Прерванный ошибкой сеанс программы','');
INSERT INTO Trs VALUES('Successful session of the program','Вдалий сеанс програми','Успешный сеанс программы','');
INSERT INTO Trs VALUES('Command miss: ','Відсутня команда: ','Отсутствует команда: ','');
INSERT INTO Trs VALUES('Waiting %1s expired','Очікування %1с вичерпано','Ожидание %1c исчерпано','');
INSERT INTO Trs VALUES('Waiting %1s','Очікування %1с','Ожидание %1c','');
INSERT INTO Trs VALUES('Background waiting %1s','Фонове очікування %1с','Фоновое ожидание %1c','');
INSERT INTO Trs VALUES('No current node present','Поточний вузол відсутній','Текущий узел отсутствует','');
INSERT INTO Trs VALUES('No connection to source object','Немає з''єднання із об''єктом джерела','Нет подключения с объектом источника','');
INSERT INTO Trs VALUES('Source error','Помилка джерела','Ошибка источника','');
INSERT INTO Trs VALUES('Status','Статус','Статус','Статус');
INSERT INTO Trs VALUES('On battery','Від батареї','От батареи','');
INSERT INTO Trs VALUES('Low battery','Батарею розряджено','Батарея разряжена','');
INSERT INTO Trs VALUES('Shutdown load','Скид навантаження','Сброс нагрузки','');
INSERT INTO Trs VALUES('ALARM','АВАРІЯ','АВАРИЯ','');
INSERT INTO Trs VALUES('None good battery present','Відсутні хорощі батареї','Отсутствуют хорошие батареи','');
INSERT INTO Trs VALUES('Battery charge low','Низький заряд батареї','Низкий заряд батареи','');
INSERT INTO Trs VALUES('Battery charge critical','Критичний заряд батареї','Критический заряд батареи','');
INSERT INTO Trs VALUES('Bad %1 batteries present','Зіпсованих батарей %1','Испорченных батарей %1','');
INSERT INTO Trs VALUES('Input voltage low','Низька вхідна напруга','Низкое входное напряжение','');
INSERT INTO Trs VALUES('Input voltage high','Висока вхідна напруга','Высокое входное напряжение','');
INSERT INTO Trs VALUES('Input frequency too low','Дуже низька вхідна частота','Очень низкая входная частота','');
INSERT INTO Trs VALUES('Input frequency too high','Дуже висока вхідна частота','Очень высокая входная частота','');
INSERT INTO Trs VALUES('Input frequency low','Низька вхідна частота','Низкая входная частота','');
INSERT INTO Trs VALUES('Input frequency high','Висока вхідна частота','Высокая входная частота','');
INSERT INTO Trs VALUES('UPS overloaded','ДБЖ перевантажено','ИБП перегружено','');
INSERT INTO Trs VALUES('UPS load high','Високе навантаження ДБЖ','Высокая нагрузка ИБП','');
INSERT INTO Trs VALUES('UPS overheated','ДБЖ перегрітий','ИБП перегретый','');
INSERT INTO Trs VALUES('Temperature high','Висока температура','Высокая температура','');
INSERT INTO Trs VALUES('Levitation','Левітація','Левитация','');
INSERT INTO Trs VALUES('No Levitation','Не левітація','Не левитация','');
INSERT INTO Trs VALUES('Acceleration','Розгін','Разгон','');
INSERT INTO Trs VALUES('Normal','Нормальний','Нормальный','');
INSERT INTO Trs VALUES('Deceleration (Brake)','Гальмування','Торможение','');
INSERT INTO Trs VALUES('Autotest','Автотест','Автотест','');
INSERT INTO Trs VALUES('Tuning','Підлаштування','Подстройка','');
INSERT INTO Trs VALUES('Tuning Complete','Підлаштування завершене','Подстройка завершена','');
INSERT INTO Trs VALUES('Local','Локально','Локально','');
INSERT INTO Trs VALUES('Remote','Віддалено','Удалённо','');
INSERT INTO Trs VALUES('Start session.','Запуск сеансу.','Запуск сеанса.','');
INSERT INTO Trs VALUES('Request for values present list and size.','Запит наявних значень та розмірів','Запрос доступных значений и размеров','');
INSERT INTO Trs VALUES('Readed items list write.','Запис переліку элементів для читання.','Запись списка элементов для чтения.','');
INSERT INTO Trs VALUES('Write to hour archive for time','Запис до архіву годин на час','Запись в архив часов за время','');
INSERT INTO Trs VALUES('Write to day archive for time','Запис до архіву днів на час','Запись в архив дней за время','');
INSERT INTO Trs VALUES('Write to result month archive for time','Запис до результуючого архіву місяців на час','Запись в результирующий архив месяцев за время','');
INSERT INTO Trs VALUES('Error response','Помилка відповіді','Ошибка ответа','');
INSERT INTO Trs VALUES('Quality errors','Помилки якості','Ошибки качетсва','');
INSERT INTO Trs VALUES('No a connection','Нема підключення','Нет подключения','');
INSERT INTO Trs VALUES('Data list for request empty.','Перелік даних запиту порожній.','Список данных запроса пустой.','');
INSERT INTO Trs VALUES('Unknown','Невідомий','Неизвестный','');
INSERT INTO Trs VALUES('Deceleration','Гальмування','Торможение','');
INSERT INTO Trs VALUES('Stop operation failure','Невдача операції зупинки','Неудача операции останова','');
INSERT INTO Trs VALUES('Idling failure','Невдача простою','Неудача простоя','');
INSERT INTO Trs VALUES('Regeneration breaking failure','Збій регенерації гальмування','Сбой регенерации торможения','');
INSERT INTO Trs VALUES('Deceleration failure','Помилка гальмування','Ошибка торможения','');
INSERT INTO Trs VALUES('No connect','Не підключено','Не подключено','');
INSERT INTO Trs VALUES('Error or no response.','Помилка або немає відповіді','Ошибка или нет ответа','');
INSERT INTO Trs VALUES('CRC error.','Помилка CRC.','Ошибка CRC.','');
INSERT INTO Trs VALUES('DCON error.','Помилка DCON.','Ошибка DCON.','');
INSERT INTO Trs VALUES('No respond','Немає відповіді','Нет ответа','');
INSERT INTO Trs VALUES('No acknowledgment','Немає підтвердження','Нет подтверждения','');
INSERT INTO Trs VALUES('Data block short or error','Блок даних короткий або помилка','Блок данных короткий или ошибка','');
INSERT INTO Trs VALUES('Block sequence.','Блокувальна послідовність.','Блокирующая последовательность.','');
INSERT INTO Trs VALUES('Unknown block end.','Кінець невідомого блоку.','Конец неизвестного блока.','');
INSERT INTO Trs VALUES('Not full respond','Неповна відповідь','Неполный ответ','');
INSERT INTO Trs VALUES('Data range error.','Помилка діапазону даних','Ошибка диапазона данных','');
INSERT INTO Trs VALUES('Unknown error.','Невідома помилка.','Неизвестная ошибка.','');
INSERT INTO Trs VALUES('Error set PIN-code.','Помилка встановлення PIN-коду.','Ошибка установки PIN-кода.','');
INSERT INTO Trs VALUES('Error set PDU mode.','Помилка встановлення режиму PDU.','Ошибка установки режима PDU.','');
INSERT INTO Trs VALUES('Error sent SMS.','Помилка надсилання SMS.','Ошибка отправки SMS.','');
INSERT INTO Trs VALUES('Error sent SMS PDU','Помилка надсилання SMS PDU','Ошибка отправки SMS PDU','');
INSERT INTO Trs VALUES('Device address out of range 1...254','Адреса пристрою за межою 1...254','Адрес устройства за границей 1...254','');
INSERT INTO Trs VALUES('No or error respond','Немає або помилка відповіді','Нет или ошибка ответа','');
INSERT INTO Trs VALUES('Device address out of range 1...32','Адреса пристрою за межою 1...32','Адрес устройства за границей 1...32','');
INSERT INTO Trs VALUES('Device address out of range 0...254','Адреса пристрою за межою 0...254','Адрес устройства за границей 0...254','');
INSERT INTO Trs VALUES('Device address out of range 0...2047','Адреса пристрою за межою 0...2047','Адрес устройства за границей 0...2047','');
INSERT INTO Trs VALUES('No or short respond','Немає або коротка відповідь','Нет или короткий ответ','');
INSERT INTO Trs VALUES('Device address out of range 0...255','Адреса пристрою за межою 0...255','Адрес устройства за границей 0...255','');
INSERT INTO Trs VALUES('No a respond','Немає відповіді','Нет ответа','');
INSERT INTO Trs VALUES('CRC error','Помилка CRC','Ошибка CRC','');
INSERT INTO Trs VALUES('Signal exceed to upper hardware border','Вихід сигналу за нижню апаратну границю','Выход сигнала за нижнюю аппаратную границу','');
INSERT INTO Trs VALUES('Signal exceed to bottom hardware border','Вихід сигналу за верхню апаратну границю','Выход сигнала за верхнюю аппаратную границу','');
INSERT INTO Trs VALUES('Request','Запит','Запрос','');
INSERT INTO Trs VALUES('Header error','Помилка заголовку','Ошибка заголовка','');
INSERT INTO Trs VALUES('Header CRC error','Помилка контрольної суми заголовку','Ошибка контрольной суммы заголовка','');
INSERT INTO Trs VALUES('Info block size error','Помилка розміру інформаційного блоку','Ошибка размера информационного блока','');
INSERT INTO Trs VALUES('Info block CRC error.','Помилка контрольної суми інформаційного блоку','Ошибка контрольной суммы информационного блока','');
INSERT INTO Trs VALUES('Request error','Помилка запиту','Ошибка запроса','');
INSERT INTO Trs VALUES('Respond too short or wrong','Відповідь дуже коротка або помилкова','Ответ очень короткий или ошибочный','');
INSERT INTO Trs VALUES('Wrong or no a respond.','Помилка або немає відповіді.','Ошибка или нет ответа.','');
INSERT INTO Trs VALUES('Message''s length more 255*255','Розмір повідомлення більш за 255*255','Размер сообщения больше чем 255*255','');
INSERT INTO Trs VALUES('No data block get','Не отримано блоку даних','Не получено блока данных','');
INSERT INTO Trs VALUES('LRC error.','Помилка LRC.','Ошибка LRC.','');
INSERT INTO Trs VALUES('Message empty','Повідомлення порожне','Ответ пуст','');
INSERT INTO Trs VALUES('Request:','Запит:','Запрос:','');
INSERT INTO Trs VALUES('Wrong respond','Помилкова відповідь','Ошибочный ответ','');
INSERT INTO Trs VALUES('Respond:','Відповідь:','Ответ:','');
INSERT INTO Trs VALUES('Illegal command (CMD code not valid).','','','');
INSERT INTO Trs VALUES('Syntax error. (Too many bytes in data field, not enough bytes, etc).','','','');
INSERT INTO Trs VALUES('Inhibited.','','','');
INSERT INTO Trs VALUES('Obsolete command. No action taken, but not really an error.','','','');
INSERT INTO Trs VALUES('Telephone number error.','','','');
INSERT INTO Trs VALUES('Long length (%1) of the message.','','','');
INSERT INTO Trs VALUES('KS error.','','','');
INSERT INTO Trs VALUES('KS error','','','');
INSERT INTO Trs VALUES('Request error.','','','');
INSERT INTO Trs VALUES('Serial output transport ''%1'' error.','','','');
INSERT INTO Trs VALUES('Request: TRANSL_TEMP.','','','');
INSERT INTO Trs VALUES('No connection','','','');
INSERT INTO Trs VALUES('Output transport ''%1'' error.','','','');
INSERT INTO Trs VALUES('Read all data.','','','');
INSERT INTO Trs VALUES('Reply is not full','Відповідь не повна','Ответ не полный','');
INSERT INTO Trs VALUES('No Problems. The rest below are prioritized.','','','');
INSERT INTO Trs VALUES('Frequency of xtal is > Fq or < Fm. Halts all other calculations.','','','');
INSERT INTO Trs VALUES('Frequency was bad (previous XtalStat was XPROB_FREQ) and frequency is now in range, but life is less than 3%.','','','');
INSERT INTO Trs VALUES('Stability level XtalStab of xtal >= SlvlTrip.','','','');
INSERT INTO Trs VALUES('Unable to determine rate because of a computation error, most likely caused by a parameter (density, z-ratio, etc) having a zero or otherwise invalid (non numeric or infinity) value.','','','');
INSERT INTO Trs VALUES('XtalQual of xtal >= QlvlTrip.','','','');
INSERT INTO Trs VALUES('Transport ''%1'' error.','','','');
INSERT INTO Trs VALUES('Request for service info.','','','');
INSERT INTO Trs VALUES('Request for counter time and hour archive begin.','','','');
INSERT INTO Trs VALUES('Request for dimensions and precisions.','','','');
INSERT INTO Trs VALUES('Request for curent values.','','','');
INSERT INTO Trs VALUES('Request for hour archive values.','','','');
INSERT INTO Trs VALUES('Request for set archive data.','','','');
INSERT INTO Trs VALUES('Request for day archive values.','','','');
INSERT INTO Trs VALUES('Request for results month archive values.','','','');
INSERT INTO Trs VALUES('Request for data read.','','','');
INSERT INTO Trs VALUES('Set current','','','');
INSERT INTO Trs VALUES('Device address out of range 0...15','','','');
INSERT INTO Trs VALUES('Respond too short','','','');
INSERT INTO Trs VALUES('Respond too long, possible continuous mode','','','');
INSERT INTO Trs VALUES('RS232 synchronization error. ','','','');
INSERT INTO Trs VALUES('Incorrect command, e.g. inadmissible address (syntax error). ','','','');
INSERT INTO Trs VALUES('Inadmissible read command. ','','','');
INSERT INTO Trs VALUES('SP1 status. ','','','');
INSERT INTO Trs VALUES('SP2 status. ','','','');
INSERT INTO Trs VALUES('Atm. pressure out of range. ','','','');
INSERT INTO Trs VALUES('Temperature out of range. ','','','');
INSERT INTO Trs VALUES('Cal. mode wrong. ','','','');
INSERT INTO Trs VALUES('Pressure underflow. ','','','');
INSERT INTO Trs VALUES('Pressure overflow. ','','','');
INSERT INTO Trs VALUES('Zero adjust warning. ','','','');
INSERT INTO Trs VALUES('PT1000 fault (CTR 101 only). ','','','');
INSERT INTO Trs VALUES('Heaterblock overtemp. ','','','');
INSERT INTO Trs VALUES('Electronic overtemp. ','','','');
INSERT INTO Trs VALUES('Zero adjust error. ','','','');
INSERT INTO Trs VALUES('Output transport ''%1'' stoped.','','','');
INSERT INTO Trs VALUES('Wrong sequence, rejected','Помилкова послідовність, відкинуто','Ошибочная последовательность, отброшено','');
INSERT INTO Trs VALUES('Connection terminated by the input sequence broken, lost input packages, S(R)=%1, S(T)=%2','','','');
INSERT INTO Trs VALUES('Connection terminated by not acknowledge transmited package ''%1'' by timeout t1 (%2).','','','');
INSERT INTO Trs VALUES('Connection terminated by not confirmed TEST package by timeout t1 (%1).','','','');
INSERT INTO Trs VALUES('Error','Помилка','Ошибка','Грешка');
INSERT INTO Trs VALUES('Good output sequence','','','');
INSERT INTO Trs VALUES('Good input sequence','','','');
INSERT INTO Trs VALUES('M_SP_NA_1: No the SQ mode implemented yet','','','');
INSERT INTO Trs VALUES('M_SP_NA_1: Items number discrepancy to the package size','','','');
INSERT INTO Trs VALUES('M_ME_NB_1: No the SQ mode implemented yet','','','');
INSERT INTO Trs VALUES('M_ME_NB_1: Items number discrepancy to the package size','','','');
INSERT INTO Trs VALUES('C_SC_NA_1: No the SQ mode implemented yet','','','');
INSERT INTO Trs VALUES('C_SC_NA_1: Items number discrepancy to the package size','','','');
INSERT INTO Trs VALUES('Device address ''%1'' out of range [0...119].','','','');
INSERT INTO Trs VALUES('No read result.','','','');
INSERT INTO Trs VALUES('No a device present on the bus ''%1''.','','','');
INSERT INTO Trs VALUES('1W device','','','');
INSERT INTO Trs VALUES('Temperature','Температура','Температура','Температура');
INSERT INTO Trs VALUES('User cell [0...65535]','','','');
INSERT INTO Trs VALUES('Resolution [9...12]','','','');
INSERT INTO Trs VALUES('1:Device missed','','','');
INSERT INTO Trs VALUES('3:No a response or the response short.','','','');
INSERT INTO Trs VALUES('3:Incomplete response.','','','');
INSERT INTO Trs VALUES('4:Request error.','','','');
INSERT INTO Trs VALUES('Address ''%1'' out of range [0...255].','','','');
INSERT INTO Trs VALUES('Response','','','');
INSERT INTO Trs VALUES('Appending variable','Додання змінної','Добавление переменной','');
INSERT INTO Trs VALUES('4:Unknown function.','','','');
INSERT INTO Trs VALUES('4:Insufficient rights.','','','');
INSERT INTO Trs VALUES('4:Invalid function received.','','','');
INSERT INTO Trs VALUES('5:Data error...','','','');
INSERT INTO Trs VALUES('4:Invalid host''s address received.','','','');
INSERT INTO Trs VALUES('5:Operation error.','','','');
INSERT INTO Trs VALUES('Wrong login.','','','');
INSERT INTO Trs VALUES('No such variable.','','','');
INSERT INTO Trs VALUES('Unknown error %1.','','','');
INSERT INTO Trs VALUES('Destination:','Мета:','Цель:','');
INSERT INTO Trs VALUES('DI0','','','');
INSERT INTO Trs VALUES('DI1','','','');
INSERT INTO Trs VALUES('DO0','','','');
INSERT INTO Trs VALUES('DO1','','','');
INSERT INTO Trs VALUES('DS2480 is not detected.','','','');
INSERT INTO Trs VALUES('Size mismatch.','','','');
INSERT INTO Trs VALUES('day','день','день','');
INSERT INTO Trs VALUES('hour','год','час','');
INSERT INTO Trs VALUES('min','хвил','мин','');
INSERT INTO Trs VALUES('s','с','с','');
INSERT INTO Trs VALUES('ms','мс','мс','');
INSERT INTO Trs VALUES('us','мкс','мкс','');
INSERT INTO Trs VALUES('ns','нс','нс','');
INSERT INTO Trs VALUES('Allowed variables','Наявно змінних','Доступно переменных','');
INSERT INTO Trs VALUES('No data','Немає даних','Нет данных','Нема података');
INSERT INTO Trs VALUES('Error disabling ECHO: %1','Помилка вимкнення ЛУНИ: %1','Ошибка отключения ЕХО: %1','');
INSERT INTO Trs VALUES('Error checking for PIN-code: %1','Помилка перевірки PIN-коду: %1','Ошибка проверки PIN-кода: %1','');
INSERT INTO Trs VALUES('Error setting PIN: %1','Помилка встановлення PIN: %1','Ошибка установки PIN: %1','');
INSERT INTO Trs VALUES('Error setting the SMS Text mode: %1','Помилка встановлення текстового режиму SMS: %1','Ошибка установки текстового режима SMS: %1','');
INSERT INTO Trs VALUES('Error sending SMS: %1','Помилка надсилання SMS: %1','Ошибка отправки SMS: %1','');
INSERT INTO Trs VALUES('Error sending SMS PDU: %1','Помилка надсилання SMS PDU: %1','Ошибка отправки SMS PDU: %1','');
INSERT INTO Trs VALUES('Error setting the SMS PDU mode: %1','Помилка встановлення SMS PDU режиму: %1','Ошибка установки SMS PDU режима: %1','');
INSERT INTO Trs VALUES('Impossible connect to the SMTP-server.','Неможливо під''єднатися до SMTP-серверу.','Невозможно подключиться к SMTP-серверу.','');
INSERT INTO Trs VALUES('No response.','Немає відповіді.','Нет ответа.','');
INSERT INTO Trs VALUES('Error HELLO: %1.','Помилка HELLO: %1.','Ошибка HELLO: %1.','');
INSERT INTO Trs VALUES('Unsupported auth methods: %1.','Непідтримуваний метод автентифікації: %1.','Неподдерживаемый метод аутентификации: %1.','');
INSERT INTO Trs VALUES('Auth required: %1.','Потрібна автентифікація: %1.','Необходима аутентификация: %1.','');
INSERT INTO Trs VALUES('AUTH error: %1.','Помилка автентифікації: %1.','Ошибка аутентификации: %1.','');
INSERT INTO Trs VALUES('AUTH USER error: %1.','Помилка користувача автентифікації: %1.','Ошибка пользователя аутентификации: %1.','');
INSERT INTO Trs VALUES('AUTH PASS error: %1.','Помилка пароля автентифікації: %1.','Ошибка пароля аутентификации: %1.','');
INSERT INTO Trs VALUES('MAIL FROM error: %1.','Помилка MAIL FROM: %1.','Ошибка MAIL FROM: %1.','');
INSERT INTO Trs VALUES('RCPT TO error: %1.','Помилка RCPT TO: %1.','Ошибка RCPT TO: %1.','');
INSERT INTO Trs VALUES('DATA error: %1.','Помилка даних: %1.','Ошибка данных: %1.','');
INSERT INTO Trs VALUES('DATA send error: %1.','Помилка надсилання даних: %1.','Ошибка отправки данных: %1.','');
INSERT INTO Trs VALUES('Alarm','Сигнал','Сигнал','Аларм');
INSERT INTO Trs VALUES('Norm','Норма','Норма','Норма');
INSERT INTO Trs VALUES('Sent %1. In queue %2.','Надіслано %1. У черзі %2.','Отправлено %1. В очереди %2.','');
INSERT INTO Trs VALUES('Error: %1.','Помилка: %1.','Ошибка: %1.','');
INSERT INTO Trs VALUES('Wrong or empty respond to the calibration request.','','','');
INSERT INTO Trs VALUES('Wrong or empty respond to the temperature data.','','','');
INSERT INTO Trs VALUES('Wrong or empty read respond.','','','');
INSERT INTO Trs VALUES('Device address out of range 0...119.','','','');
INSERT INTO Trs VALUES('Memory offset out of range 0...8191.','','','');
INSERT INTO Trs VALUES('Readed memory size out of range 0...8191.','','','');
INSERT INTO Trs VALUES('Not or short respond %1 from %2.','','','');
INSERT INTO Trs VALUES('Too long write sequence.','','','');
INSERT INTO Trs VALUES('DI','','','');
INSERT INTO Trs VALUES('DO','','','');
INSERT INTO Trs VALUES('Resolution','','','');
INSERT INTO Trs VALUES('AI','','','');
INSERT INTO Trs VALUES('Mode','Режим','Режим','');
INSERT INTO Trs VALUES('Voltage','','','');
INSERT INTO Trs VALUES('Current','Поточний','Текущее','');
INSERT INTO Trs VALUES('Temperature, °С','Температура, °С','Температура, °С','Температура, °С');
INSERT INTO Trs VALUES('AI, V','','','');
INSERT INTO Trs VALUES('Voltage, V','','','');
INSERT INTO Trs VALUES('Current, A','','','');
INSERT INTO Trs VALUES('Current A/D','','','');
INSERT INTO Trs VALUES('Current Accumulator','','','');
INSERT INTO Trs VALUES('Current Accumulator Shadow','','','');
INSERT INTO Trs VALUES('Voltage A/D (0-VAD,1-VDD)','','','');
INSERT INTO Trs VALUES('Accumulation threshold','','','');
INSERT INTO Trs VALUES('Elapsed time meter','','','');
INSERT INTO Trs VALUES('Current Offset','','','');
INSERT INTO Trs VALUES('Enable Current A/D','','','');
INSERT INTO Trs VALUES('Enable Current Accumulator','','','');
INSERT INTO Trs VALUES('Enable Current Accumulator Shadow','','','');
INSERT INTO Trs VALUES('Disconnect time','','','');
INSERT INTO Trs VALUES('End of charge time','','','');
INSERT INTO Trs VALUES('No link to external functions on ''%1''.','','','');
INSERT INTO Trs VALUES('Unknown device %1 [0-DHT11, 1-DHT22].','','','');
INSERT INTO Trs VALUES('Tries number %1 out of range [1...5].','','','');
INSERT INTO Trs VALUES('Get data after %1 tries error.','','','');
INSERT INTO Trs VALUES('None of good battery present','','','');
INSERT INTO Trs VALUES('Error for ''%1'' as an output transport of I2C or a link to external functions of GPIO.','','','');
INSERT INTO Trs VALUES('Empty','Пусто','Пусто','');
INSERT INTO Trs VALUES('Wrong or empty respond to the calibration T1-3 or P1-9 request.','','','');
INSERT INTO Trs VALUES('Wrong or empty respond to the calibration H1 request.','','','');
INSERT INTO Trs VALUES('Wrong or empty respond to the calibration H2-H6 request.','','','');
INSERT INTO Trs VALUES('Wrong or empty respond to the pressure data.','','','');
INSERT INTO Trs VALUES('Wrong or empty respond to the humidity data.','','','');
INSERT INTO Trs VALUES('Error FCS.','Помилка FCS','Ошибка FCS','');
INSERT INTO Trs VALUES('Wrong or empty response.','Помилкова або порожня відповідь','Ошибочный или пустой ответ.','');
INSERT INTO Trs VALUES('3:No response or the response is short.','3:Немає відповіді або вона закоротка.','3:Нет ответа или он короткий.','');
INSERT INTO Trs VALUES('4:Disparity of the request address and the response one.','4:Невідповідність адреси запиту та відповіді.','4:Несоответствие адреса запроса и ответа.','');
INSERT INTO Trs VALUES('4:Disparity of the request function and the response one.','4:Невідповідність функції запиту та відповіді.','4:Несоответствие функции запроса и ответа.','');
INSERT INTO Trs VALUES('4:Incomplete response.','4:Неповна відповідь.','4:Неполный ответ.','');
INSERT INTO Trs VALUES('4:Disparity of the request ID and the response one.','4:Невідповідність ID запиту та відповіді.','4:Несоответствие ID запроса и ответа.','');
INSERT INTO Trs VALUES('4:Error CRC.','4:Помилка CRC.','4:Ошибка CRC.','');
INSERT INTO Trs VALUES('Channel','Канал','Канал','');
INSERT INTO Trs VALUES('value','значення','значение','вредност');
INSERT INTO Trs VALUES('weight','вага','масса','');
INSERT INTO Trs VALUES('average flow','середній поток','средний поток','');
INSERT INTO Trs VALUES('input','Вхід','Вход','');
INSERT INTO Trs VALUES('Error the output transport ''%1''.','Помилка вихідного транспорту ''%1''','Ошибка выходного транспорта ''%1''.','');
INSERT INTO Trs VALUES('The address ''%1'' is out of the range [0...99999999].','Адреса ''%1'' поза діапазоном [0...99999999].','Адрес ''%1'' за диапазоном [0...99999999].','');
INSERT INTO Trs VALUES('No data.','Немає даних.','Нет данных','');
INSERT INTO Trs VALUES('Address out of the range [-2047...255].','Адреса поза діапазону [-2047...255].','Адрес вне диапазона [-2047...255].','');
INSERT INTO Trs VALUES('Wrong or no response.','Помилкове або немає відповіді.','Ошибочное или нет ответа.','');
INSERT INTO Trs VALUES('Error CRC.','Помилка CRC.','Ошибка CRC.','');
INSERT INTO Trs VALUES('Data size error.','Помилка розміру даних.','Ошибка размера данных.','');
INSERT INTO Trs VALUES('The data size is not equal to pointed one.','Розмір даних не відповідає вказаному.','Размер данных не соответствует указанному.','');
INSERT INTO Trs VALUES('Write','Запис','Запись','');
INSERT INTO Trs VALUES('uh oh, no thermocouple attached!','ух ох, не підключено термопари','ух ох, не подключена термопара','');
INSERT INTO Trs VALUES('Items number is discrepancy to the package size','Кількість елементів не відповідає розміру пакета','Количество элементов не соответствует размеру пакета','');
INSERT INTO Trs VALUES('Input transport ''%1'' error.','Помилка вхідного транспорту ''%1''.','Ошибка входного транспорта ''%1''.','');
INSERT INTO Trs VALUES('Error the source object path ''%1''.','Помилковий шлях ''%1'' об''єкту джерела.','Ошибка пути ''%1'' объекта источника.','');
INSERT INTO Trs VALUES('The control field path is empty.','Шлях поля контролю порожній.','Путь контрольного поля пуст.','');
INSERT INTO Trs VALUES('The requested identificator ''%1'' is mіssing in the container ''%2'' of the data source objects!','Запитаний ідентифікатор ''%1'' відсутній у контейнері ''%2'' об''єктів джерел!','Запрошенный идентификатор ''%1'' отсутствует в контейнере ''%2'' объектов источников!','');
INSERT INTO Trs VALUES('SUPPRESSED','ПРИДУШЕНО','ПОДАВЛЕНО','');
INSERT INTO Trs VALUES('Wrong or CRC-error sequence, rejected','','','');
INSERT INTO Trs VALUES('Foreign package, rejected','','','');
INSERT INTO Trs VALUES('CRC error in chunk %1, rejected','','','');
INSERT INTO Trs VALUES('First without closed final and a package seems lost, rejected','','','');
INSERT INTO Trs VALUES('Broken sequence, rejected','','','');
INSERT INTO Trs VALUES('Destination address ''%1'' out of range [0...65520].','','','');
INSERT INTO Trs VALUES('Source address ''%1'' out of range [0...65520].','','','');
INSERT INTO Trs VALUES('Unsupported data object ''%1'', the processing terminated','','','');
INSERT INTO Trs VALUES('Binary Input: unsupported variant ''%1'', the processing terminated','','','');
INSERT INTO Trs VALUES('Binary Input (1): unsupported variant ''%1'', the processing terminated','','','');
INSERT INTO Trs VALUES('Not completed object''s data in the offset %1, the processing terminated','','','');
INSERT INTO Trs VALUES('Not enough of the data in %1, the processing terminated','','','');
INSERT INTO Trs VALUES('Unsupported data range code %1, the processing terminated','','','');
INSERT INTO Trs VALUES('Wrong points range [%1...%2], the processing terminated','','','');
INSERT INTO Trs VALUES('Wrong points range [%1...%2] in %1, the processing terminated','','','');
INSERT INTO Trs VALUES('Unsupported data range code %1 in %2, the processing terminated','','','');
INSERT INTO Trs VALUES('Wrong points range [%1...%2] in %3, the processing terminated','','','');
INSERT INTO Trs VALUES('Double-bit Input (3): unsupported variant ''%1'', the processing terminated','','','');
INSERT INTO Trs VALUES('Binary Counter (20) or Frozen Binary Counter (21): unsupported variant ''%1'', the processing terminated','','','');
INSERT INTO Trs VALUES('Analog Input (30): unsupported variant ''%1'', the processing terminated','','','');
INSERT INTO Trs VALUES('Binary Output (10): unsupported variant ''%1'', the processing terminated','','','');
INSERT INTO Trs VALUES('Analog Output (40): unsupported variant ''%1'', the processing terminated','','','');
INSERT INTO Trs VALUES('Time and Date (4): unsupported variant ''%1'', the processing terminated','','','');
INSERT INTO Trs VALUES('Changed Analog Input (32): unsupported variant ''%1'', the processing terminated','','','');
INSERT INTO Trs VALUES('Analog Input Changed (32): unsupported variant ''%1'', the processing terminated','','','');
INSERT INTO Trs VALUES('Binary Input Changed (2): unsupported variant ''%1'', the processing terminated','','','');
INSERT INTO Trs VALUES('Double-bit Input Changed (4): unsupported variant ''%1'', the processing terminated','','','');
INSERT INTO Trs VALUES('Binary Counter Changed (22) or Frozen Binary Counter Changed (23): unsupported variant ''%1'', the processing terminated','','','');
INSERT INTO Trs VALUES('Device Restart','','','');
INSERT INTO Trs VALUES('Device Trouble','','','');
INSERT INTO Trs VALUES('Digital Outputs in Local','','','');
INSERT INTO Trs VALUES('Time Sync Required','','','');
INSERT INTO Trs VALUES('Class 3 Data Available','','','');
INSERT INTO Trs VALUES('Class 2 Data Available','','','');
INSERT INTO Trs VALUES('Class 1 Data Available','','','');
INSERT INTO Trs VALUES('Configuration Corrupt','','','');
INSERT INTO Trs VALUES('Operation Already Executing','','','');
INSERT INTO Trs VALUES('Event Buffer Overflow','','','');
INSERT INTO Trs VALUES('Parameters Invalid or Out of Range','','','');
INSERT INTO Trs VALUES('Requested Objects Unknown','','','');
INSERT INTO Trs VALUES('Function Code not implemented','','','');
INSERT INTO Trs VALUES('Error the DB ''%1''.','Помилка БД ''%1''.','Ошибка БД ''%1''.','');
INSERT INTO Trs VALUES('Unknown data type %1, termination','Невідомий тип даних %1, переривання','Неизвестный тип данных %1, прерывание','');
INSERT INTO Trs VALUES('Wrong header or source address, rejected','Помилка заголовку або адреси джерела, відкинуто','Ошибка заголовка или адреса источника, отброшено','');
INSERT INTO Trs VALUES('Wrong destination address, rejected','Помилка адреси призначення, відкинуто','Ошибка адреса назначения, отброшено','');
INSERT INTO Trs VALUES('CRC-error, rejected','Помилка CRC, відкинуто','Ошибка CRC, отброшено','');
INSERT INTO Trs VALUES('The end symbol is wrong','Помилковий символ завершення','Ошибочный символ завершения','');
INSERT INTO Trs VALUES('Broken sequence %1, rejected','Помилкова послідовність %1, відкинуто','Ошибочная последовательность %1, отброшено','');
INSERT INTO Trs VALUES('Error the authentication','Помилка автентифікації','Ошибка аутентификации','');
INSERT INTO Trs VALUES('Error the data %1','Помилка даних %1','Ошибка данных %1','');
INSERT INTO Trs VALUES('Upper HDLC destination address ''%1'' out of range [0...16383].','Верхня адреса призначення HDLC ''%1'' поза діапазоном [0...16383].','Верхний адрес назначения HDLC ''%1'' за диапазоном [0...16383].','');
INSERT INTO Trs VALUES('Lower HDLC destination address ''%1'' out of range [0...16383].','Нижня адреса призначення HDLC ''%1'' поза діапазоном [0...16383].','Нижний адрес назначения HDLC ''%1'' за диапазоном [0...16383].','');
INSERT INTO Trs VALUES('Source address ''%1'' out of range [0...127].','Адреса джерела ''%1'' поза діапазоном [0...127].','Адрес источника ''%1'' за диапазоном [0...127].','');
INSERT INTO Trs VALUES('Address ''%1'' out of range [1...999].','Адреса ''%1'' поза межою [1...999].','Адрес ''%1'' за границей [1...999].','');
INSERT INTO Trs VALUES('Output transport is empty and the controller object is not ModBus.','','','');
INSERT INTO Trs VALUES('Address ''%1'' out of range [0...247].','','','');
INSERT INTO Trs VALUES('Inconsistent respond''s length.','','','');
INSERT INTO Trs VALUES('Too short','','','');
INSERT INTO Trs VALUES('No response','','','');
CREATE TABLE IF NOT EXISTS 'tmplib_base_io' ("TMPL_ID" TEXT DEFAULT '' ,"ID" TEXT DEFAULT '' ,"NAME" TEXT DEFAULT '' ,"TYPE" INTEGER DEFAULT '' ,"FLAGS" INTEGER DEFAULT '' ,"VALUE" TEXT DEFAULT '' ,"POS" INTEGER DEFAULT '' ,"uk#NAME" TEXT DEFAULT '' ,"uk#VALUE" TEXT DEFAULT '' ,"ru#NAME" TEXT DEFAULT '' ,"ru#VALUE" TEXT DEFAULT '' ,"sr#NAME" TEXT DEFAULT '' , PRIMARY KEY ("TMPL_ID","ID"));
INSERT INTO tmplib_base_io VALUES('digAlarm','in','Input',3,144,'Input|in',2,'Вхід','','Вход','','');
INSERT INTO tmplib_base_io VALUES('simleBoard','in','Input',2,128,'Parameter|var',0,'Вхід','','Вход','','');
INSERT INTO tmplib_base_io VALUES('simleBoard','var','Variable',2,32,'0',1,'Змінна','','Переменная','','Променљива');
INSERT INTO tmplib_base_io VALUES('simleBoard','ed','Dimension',0,4128,'',2,'Одиниця виміру','','Единица измерения','','');
INSERT INTO tmplib_base_io VALUES('simleBoard','min','Lower work border',2,32,'0',3,'Нижня робоча границя','','Нижн. раб. граница','','');
INSERT INTO tmplib_base_io VALUES('simleBoard','max','Upper work border',2,32,'100',4,'Верхня робоча границя','','Верхн. раб. граница','','');
INSERT INTO tmplib_base_io VALUES('simleBoard','aMin','Lower alarm border',2,32,'0',5,'Нижня аварійна границя','','Нижняя аварийная граница','','Доњи услови укључивања аларма');
INSERT INTO tmplib_base_io VALUES('simleBoard','aMax','Upper alarm border',2,32,'0',6,'Верхня аварійна границя','','Верхняя аварийная граница','','Горњи услови укључивања аларма');
INSERT INTO tmplib_base_io VALUES('simleBoard','wMin','Lower warning border',2,32,'0',7,'Нижня попереджувальна границя','','Нижн. предупредительная граница','','');
INSERT INTO tmplib_base_io VALUES('simleBoard','wMax','Upper warning border',2,32,'0',8,'Верхня попереджувальна границя','','Верхн. предупредительная граница','','Горња граница упозорења');
INSERT INTO tmplib_base_io VALUES('simleBoard','iAdd','Addon to input',2,64,'0',9,'Додаток до входу','','Добавка ко входу','','');
INSERT INTO tmplib_base_io VALUES('simleBoard','iMult','Input multiplication',2,64,'1',10,'Множник до входу','','Множитель входа','','');
INSERT INTO tmplib_base_io VALUES('digitBlock','cmdOpen','Command "Open"',3,161,'Crane|com',0,'Команда "Відкрити"','Кран|com','Команда "Открыть"','Кран|com','');
INSERT INTO tmplib_base_io VALUES('digitBlock','cmdClose','Command "Close"',3,161,'Crane|close',1,'Команда "Закрити"','Кран|close','Команда "Закрыть"','Кран|close','');
INSERT INTO tmplib_base_io VALUES('digitBlock','cmdStop','Command "Stop"',3,161,'Crane|stop',2,'Команда "Стоп"','Кран|stop','Команда "Стоп"','Кран|stop','');
INSERT INTO tmplib_base_io VALUES('digitBlock','stOpen','State "Opened"',3,144,'Crane|st_open',3,'Стан "Відкрито"','Кран|st_open','Состояние "Открыто"','Кран|st_open','');
INSERT INTO tmplib_base_io VALUES('digitBlock','stClose','State "Closed"',3,144,'Crane|st_close',4,'Стан "Закрито"','Кран|st_close','Состояние "Закрыто"','Кран|st_close','');
INSERT INTO tmplib_base_io VALUES('digitBlock','tCmd','Time for hold the command, seconds',1,32,'5',5,'Час утримання команди, секунди','','Время удерж. команды, секунды','','');
INSERT INTO tmplib_base_io VALUES('digitBlock','last_cmd','Last command',1,1,'0',6,'Остання команда','','Последняя команда','','');
INSERT INTO tmplib_base_io VALUES('digitBlock','w_tm','Process command counter',2,1,'0',7,'Лічильник обробки команди','','Счётчик отраб. команды','','');
INSERT INTO tmplib_base_io VALUES('manInUnif','inout','Input/Output',2,129,'Parameter|var',0,'Вхід/Вихід','','Вход/Выход','','');
INSERT INTO tmplib_base_io VALUES('manInUnif','var','Variable',2,16,'0',1,'Змінна','','Переменная','','Променљива');
INSERT INTO tmplib_base_io VALUES('manInUnif','ed','Dimension',0,4128,'',3,'Одиниця виміру','','Единица измерения','','');
INSERT INTO tmplib_base_io VALUES('manInUnif','min','Scale: minimum',2,32,'0',4,'Шкала: мінімум','','Шкала: минимум','','');
INSERT INTO tmplib_base_io VALUES('manInUnif','max','Scale: maximum',2,32,'100',5,'Шкала: максимум','','Шкала: максимум','','');
INSERT INTO tmplib_base_io VALUES('manInUnif','scSqr','Scale: square',3,0,'0',6,'Шкала: квадратична','','Шкала: квадратичная','','');
INSERT INTO tmplib_base_io VALUES('manInUnif','aMin','Border down alarm',2,32,'0',9,'Границя нижня аварійна','','Граница нижняя аварийная','','');
INSERT INTO tmplib_base_io VALUES('manInUnif','aMax','Border up alarm',2,32,'0',10,'Границя верхня аварійна','','Граница верхняя аварийная','','');
INSERT INTO tmplib_base_io VALUES('manInUnif','wMin','Border down warning',2,32,'0',11,'Границя нижня попереджув.','','Граница нижняя предупр.','','');
INSERT INTO tmplib_base_io VALUES('manInUnif','wMax','Border up warning',2,32,'0',12,'Границя верхня попереджув.','','Граница верхняя предупр.','','');
INSERT INTO tmplib_base_io VALUES('manInUnif','speed','Motion speed, %/cycle',2,32,'0',14,'Швидкість зміни, %/цикл','','Скорость изменения, %/цикл','','');
INSERT INTO tmplib_base_io VALUES('manInUnif','Tf','Filter time, seconds',2,32,'0',17,'Час фільтру, секунд','','Время фильтрации, секунд','','');
INSERT INTO tmplib_base_io VALUES('manInUnif','iAdd','Input addition',2,64,'0',18,'Додаток до входу','','Дополнение ко входу','','');
INSERT INTO tmplib_base_io VALUES('manInUnif','iMult','Input multiplication',2,64,'1',19,'Множник до входу','','Множитель входа','','');
INSERT INTO tmplib_base_io VALUES('manInUnif','plcMin','Minimum module scale',2,64,'0',20,'Мінімум шкали модуля','','Минимум шкалы модуля','','');
INSERT INTO tmplib_base_io VALUES('manInUnif','plcMax','Maximum module scale',2,64,'0',21,'Максимум шкали модуля','','Максимум шкалы модуля','','');
INSERT INTO tmplib_base_io VALUES('manInUnif','plcExcess','Allowed scale exceeding, %',2,64,'0',22,'Дозволене перевищення шкали, %','','Допустимое превышение шкалы модуля, %','','');
INSERT INTO tmplib_base_io VALUES('anUnif','in','Input',2,128,'Input|var',0,'Вхід','Вхід|var','Вход','Вход|var','');
INSERT INTO tmplib_base_io VALUES('anUnif','var','Variable',2,16,'0',2,'Змінна','','Переменная','','Променљива');
INSERT INTO tmplib_base_io VALUES('anUnif','ed','Dimension',0,4128,'',3,'Одиниця виміру','','Единица измерения','','');
INSERT INTO tmplib_base_io VALUES('anUnif','min','Scale: minimum',2,32,'0',4,'Шкала: мінімум','','Шкала: минимум','','');
INSERT INTO tmplib_base_io VALUES('anUnif','max','Scale: maximum',2,32,'100',5,'Шкала: максимум','','Шкала: максимум','','');
INSERT INTO tmplib_base_io VALUES('anUnif','scSqr','Scale: square',3,32,'0',6,'Шкала: квадратична','','Шкала: квадратичная','','');
INSERT INTO tmplib_base_io VALUES('anUnif','aMin','Border down alarm',2,32,'0',12,'Границя нижня аварійна','','Граница нижняя аварийная','','');
INSERT INTO tmplib_base_io VALUES('anUnif','aMax','Border up alarm',2,32,'0',13,'Границя верхня аварійна','','Граница верхняя аварийная','','');
INSERT INTO tmplib_base_io VALUES('anUnif','wMin','Border down warning',2,32,'0',14,'Границя нижня попереджув.','','Граница нижняя предупр.','','');
INSERT INTO tmplib_base_io VALUES('anUnif','wMax','Border up warning',2,32,'0',15,'Границя верхня попереджув.','','Граница верхняя предупр.','','');
INSERT INTO tmplib_base_io VALUES('anUnif','speed','Motion speed, %/cycle',2,32,'0',17,'Швидкість зміни, %/цикл','','Скорость изменения, %/цикл','','');
INSERT INTO tmplib_base_io VALUES('anUnif','prec','Precision, signs',1,32,'2',18,'Точність, знаків','','Точность, знаков','','');
INSERT INTO tmplib_base_io VALUES('anUnif','Tf','Filter time, seconds',2,32,'0',20,'Час фільтру, секунд','','Время фильтрации, секунд','','');
INSERT INTO tmplib_base_io VALUES('anUnif','iAdd','Input addition',2,64,'0',21,'Додаток до входу','','Дополнение ко входу','','');
INSERT INTO tmplib_base_io VALUES('anUnif','iMult','Input multiplication',2,64,'1',22,'Множник до входу','','Множитель входа','','');
INSERT INTO tmplib_base_io VALUES('anUnif','plcMin','Minimum module scale',2,64,'0',23,'Мінімум шкали модуля','','Минимум шкалы модуля','','');
INSERT INTO tmplib_base_io VALUES('anUnif','plcMax','Maximum module scale',2,64,'0',24,'Максимум шкали модуля','','Максимум шкалы модуля','','');
INSERT INTO tmplib_base_io VALUES('anUnif','plcExcess','Allowed scale exceeding, %',2,64,'0',25,'Дозволене перевищення шкали, %','','Допустимое превышение шкалы модуля, %','','');
INSERT INTO tmplib_base_io VALUES('anUnif','plcImit','PLC imitation',3,64,'0',26,'Імітація ПЛК','','Имитация ПЛК','','');
INSERT INTO tmplib_base_io VALUES('anUnif','plcImitIn','PLC imitation input',2,128,'Imitation|var',27,'Вхід імітації ПЛК','Імітація|var','Вход имитации ПЛК','Имитация|var','');
INSERT INTO tmplib_base_io VALUES('digitBlockUnif','com','Command "Open"',3,161,'Crane|com',0,'Команда "Відкрити"','Кран|com','Команда "Открыть"','Кран|com','');
INSERT INTO tmplib_base_io VALUES('digitBlockUnif','close','Command "Close"',3,161,'Crane|close',1,'Команда "Закрити"','Кран|close','Команда "Закрыть"','Кран|close','');
INSERT INTO tmplib_base_io VALUES('digitBlockUnif','stop','Command "Stop"',3,161,'Crane|stop',2,'Команда "Стоп"','Кран|stop','Команда "Стоп"','Кран|stop','');
INSERT INTO tmplib_base_io VALUES('digitBlockUnif','st_open','State "Opened"',3,144,'Crane|st_open',3,'Стан "Відкрито"','Кран|st_open','Состояние "Открыто"','Кран|st_open','');
INSERT INTO tmplib_base_io VALUES('digitBlockUnif','st_close','State "Closed"',3,144,'Crane|st_close',4,'Стан "Закрито"','Кран|st_close','Состояние "Закрыто"','Кран|st_close','');
INSERT INTO tmplib_base_io VALUES('digitBlockUnif','tCmd','Time to hold the command, seconds',1,64,'5',5,'Час утримання команди, секунди','','Время удерж. команды, секунды','','');
INSERT INTO tmplib_base_io VALUES('digitBlockUnif','last_cmd','Last command',1,1,'0',6,'Остання команда','','Последняя команда','','');
INSERT INTO tmplib_base_io VALUES('digitBlockUnif','w_tm','Process command counter',2,1,'0',7,'Лічильник обробки команди','','Счётчик отраб. команды','','');
INSERT INTO tmplib_base_io VALUES('pidUnifImp','in','Input',2,128,'Signal|in',0,'Вхід','','Вход','','');
INSERT INTO tmplib_base_io VALUES('pidUnifImp','sp','Setpoint',2,160,'PID|sp',1,'Завдання','ПІД|sp','Задание','ПИД|sp','');
INSERT INTO tmplib_base_io VALUES('pidUnifImp','var','Variable',2,16,'0',2,'Змінна','','Переменная','','Променљива');
INSERT INTO tmplib_base_io VALUES('pidUnifImp','manIn','Manual input, %',2,160,'PID|manIn',3,'Ручний ввід, %','ПІД|manIn','Ручной ввод, %','ПИД|manIn','');
INSERT INTO tmplib_base_io VALUES('pidUnifImp','out','Output, %',2,144,'PID|out',4,'Вихід, %','ПІД|out','Выход, %','ПИД|out','');
INSERT INTO tmplib_base_io VALUES('pidUnifImp','impQup','Impulse output up',3,144,'PID|impQupTm',5,'Імпульсний вихід у гору','ПІД|impQupTm','Импульсный выход вверх','ПИД|impQupTm','');
INSERT INTO tmplib_base_io VALUES('pidUnifImp','impQdwn','Impulse output down',3,144,'PID|impQdwnTm',6,'Імпульсний вихід до низу','ПІД|impQdwnTm','Имп. выход вниз','ПИД|impQdwnTm','');
INSERT INTO tmplib_base_io VALUES('pidUnifImp','auto','Mode: automate',3,160,'PID|auto',7,'Режим: автомат','ПІД|auto','Режим: автомат','ПИД|auto','');
INSERT INTO tmplib_base_io VALUES('pidUnifImp','analog','Mode: analog',3,144,'PID|analog',8,'Режим: аналоговий','ПІД|analog','Режим: аналоговый','ПИД|analog','');
INSERT INTO tmplib_base_io VALUES('pidUnifImp','ed','Dimension',0,4128,'',9,'Одиниця виміру','','Единица измерения','','');
INSERT INTO tmplib_base_io VALUES('pidUnifImp','min','Scale: minimum',2,160,'PID|min',10,'Шкала: мінімум','ПІД|min','Шкала: минимум','ПИД|min','');
INSERT INTO tmplib_base_io VALUES('pidUnifImp','max','Scale: maximum',2,160,'PID|max',11,'Шкала: максимум','ПІД|max','Шкала: максимум','ПИД|max','');
INSERT INTO tmplib_base_io VALUES('pidUnifImp','scSqr','Scale: square',3,32,'0',12,'Шкала: квадратична','','Шкала: квадратичная','','');
INSERT INTO tmplib_base_io VALUES('pidUnifImp','aMin','Border down alarm',2,32,'0',15,'Границя нижня аварійна','','Граница нижняя аварийная','','');
INSERT INTO tmplib_base_io VALUES('pidUnifImp','aMax','Border up alarm',2,32,'0',16,'Границя верхня аварійна','','Граница верхняя аварийная','','');
INSERT INTO tmplib_base_io VALUES('pidUnifImp','wMin','Border down warning',2,32,'0',17,'Границя нижня попереджув.','','Граница нижняя предупр.','','');
INSERT INTO tmplib_base_io VALUES('pidUnifImp','wMax','Border up warning',2,32,'0',18,'Границя верхня попереджув.','','Граница верхняя предупр.','','');
INSERT INTO tmplib_base_io VALUES('pidUnifImp','HystBnd','Hysteresis of borders violation',2,32,'0',19,'Гістерезис порушень границь','','Гистерезиз наруш. границ','','');
INSERT INTO tmplib_base_io VALUES('pidUnifImp','speed','Motion speed, %/cycle',2,32,'0',20,'Швидкість зміни, %/цикл','','Скорость изменения, %/цикл','','');
INSERT INTO tmplib_base_io VALUES('pidUnifImp','prec','Precision, signs',1,32,'2',21,'Точність, знаків','','Точность, знаков','','');
INSERT INTO tmplib_base_io VALUES('pidUnifImp','H','Border up warning violation',3,17,'0',23,'Порушення верхньої попереджув. границі','','Нарушение верхней предупредительной границы','','');
INSERT INTO tmplib_base_io VALUES('pidUnifImp','L','Border down warning violation',3,17,'0',24,'Порушення нижньої попереджув. границі','','Нарушение нижней предупредительной границы','','');
INSERT INTO tmplib_base_io VALUES('pidUnifImp','HH','Border up alarm violation',3,17,'0',25,'Порушення верхньої аварійної границі','','Нарушение верхней аварийной границы','','');
INSERT INTO tmplib_base_io VALUES('pidUnifImp','LL','Border down alarm violation',3,17,'0',26,'Порушення нижньої аварійної границі','','Нарушение нижней аварийной границы','','');
INSERT INTO tmplib_base_io VALUES('pidUnifImp','EVAL','Value is error',3,17,'0',27,'Значення помилкове','','Значение ошибочное','','');
INSERT INTO tmplib_base_io VALUES('pidUnifImp','Tf','Filter time, milliseconds',2,32,'0',28,'Час фільтру, мілісекунд','','Время фильтра, миллисекунд','','');
INSERT INTO tmplib_base_io VALUES('pidUnifImp','Kp','Gain',2,160,'PID|Kp',29,'Kп','ПІД|Kp','Kп','ПИД|Kp','');
INSERT INTO tmplib_base_io VALUES('pidUnifImp','Ki','Gain integration',2,160,'PID|Ki',30,'Кп інтегралу','ПІД|Ki','Кп интеграла','ПИД|Ki','');
INSERT INTO tmplib_base_io VALUES('pidUnifImp','Ti','Ti, milliseconds',1,160,'PID|Ti',31,'Tі, мілісекунд','ПІД|Ti','Tи, миллисекунд','ПИД|Ti','');
INSERT INTO tmplib_base_io VALUES('pidUnifImp','Kd','Gain differential',2,160,'PID|Kd',32,'Кп диференціалу','ПІД|Kd','Кп производной','ПИД|Kd','');
INSERT INTO tmplib_base_io VALUES('pidUnifImp','Td','Td, milliseconds',1,160,'PID|Td',33,'Tд, мілісекунд','ПІД|Td','Tд, миллисекунд','ПИД|Td','');
INSERT INTO tmplib_base_io VALUES('pidUnifImp','Tzd','T differential lag, milliseconds',1,160,'PID|Tzd',34,'T фільтру диференціалу, мілісекунд','ПІД|Tzd','T задержки дифференциала, миллисекунд','ПИД|Tzd','');
INSERT INTO tmplib_base_io VALUES('pidUnifImp','TImpPer','T impulses period, milliseconds',1,160,'PID|TImpPer',35,'T період імпульсів, мілісекунд','ПІД|TImpPer','T период импульса, миллисекунд','ПИД|TImpPer','');
INSERT INTO tmplib_base_io VALUES('pidUnifImp','TImpMin','T minimal impulse, milliseconds',1,160,'PID|TImpMin',36,'Мінімальна довжина імпульсу, мілісекунд','ПІД|TImpMin','T минимальная длина импульса, миллисекунд','ПИД|TImpMin','');
INSERT INTO tmplib_base_io VALUES('pidUnifImp','KImpRfact','Impulse rate factor',2,160,'PID|KImpRfact',37,'Рейтинг фактор імпульсів','ПІД|KImpRfact','Рейтинг фактор импульса','ПИД|KImpRfact','');
INSERT INTO tmplib_base_io VALUES('pidUnifImp','Zi','Insensitivity, %',2,160,'PID|Zi',38,'Нечутливість, %','ПІД|Zi','Нечувствительность, %','ПИД|Zi','');
INSERT INTO tmplib_base_io VALUES('pidUnifImp','Hup','Upper output border, %',2,160,'PID|Hup',39,'Верхня границя виходу, %','ПІД|Hup','Верхняя граница выхода, %','ПИД|Hup','');
INSERT INTO tmplib_base_io VALUES('pidUnifImp','Hdwn','Lower output border, %',2,160,'PID|Hdwn',40,'Нижня границя виходу, %','ПІД|Hdwn','Нижняя граница выхода, %','ПИД|Hdwn','');
INSERT INTO tmplib_base_io VALUES('pidUnifImp','K1','Coefficient of input 1',2,160,'PID|K1',41,'Коефіцієнт входу 1','ПІД|K1','Коэффициент входа 1','ПИД|K1','');
INSERT INTO tmplib_base_io VALUES('pidUnifImp','K2','Coefficient of input 2',2,160,'PID|K2',42,'Коефіцієнт входу 2','ПІД|K2','Коэффициент входа 2','ПИД|K2','');
INSERT INTO tmplib_base_io VALUES('pidUnifImp','K3','Coefficient of input 3',2,160,'PID|K3',43,'Коефіцієнт входу 3','ПІД|K3','Коэффициент входа 3','ПИД|K3','');
INSERT INTO tmplib_base_io VALUES('pidUnifImp','K4','Coefficient of input 4',2,160,'PID|K4',44,'Коефіцієнт входу 4','ПІД|K4','Коэффициент входа 4','ПИД|K4','');
INSERT INTO tmplib_base_io VALUES('pidUnifImp','iAdd','Input addition',2,64,'0',45,'Додаток до входу','','Дополнение ко входу','','');
INSERT INTO tmplib_base_io VALUES('pidUnifImp','iMult','Input multiplication',2,64,'1',46,'Множник до входу','','Множитель входа','','');
INSERT INTO tmplib_base_io VALUES('pidUnifImp','plcMin','Minimum module scale',2,64,'1',47,'Мінімум шкали модуля','','Минимум шкалы модуля','','');
INSERT INTO tmplib_base_io VALUES('pidUnifImp','plcMax','Maximum module scale',2,64,'5',48,'Максимум шкали модуля','','Максимум шкалы модуля','','');
INSERT INTO tmplib_base_io VALUES('pidUnifImp','plcExcess','Allowed scale exceeding, %',2,64,'0',49,'Дозволене перевищення шкали, %','','Допустимое превышение шкалы модуля, %','','');
INSERT INTO tmplib_base_io VALUES('pidUnifImp','plcImit','PLC imitation',3,64,'0',50,'Імітація ПЛК','','Имитация ПЛК','','');
INSERT INTO tmplib_base_io VALUES('pidUnifImp','plcImitIn','PLC imitation input',2,128,'Imitation|var',51,'Вхід імітації ПЛК','Імітація|var','Вход имитации ПЛК','Имитация|var','');
INSERT INTO tmplib_base_io VALUES('anUnifSt','in','Input',2,128,'Input|var',0,'Вхід','Вхід|var','Вход','Вход|var','');
INSERT INTO tmplib_base_io VALUES('anUnifSt','var','Variable',2,16,'0',2,'Змінна','','Переменная','','Променљива');
INSERT INTO tmplib_base_io VALUES('anUnifSt','H','Border up warning violation',3,17,'',3,'Порушення верхньої попереджув. границі','','Нарушение верхней предупредительной границы','','');
INSERT INTO tmplib_base_io VALUES('anUnifSt','L','Border down warning violation',3,17,'',4,'Порушення нижньої попереджув. границі','','Нарушение нижней предупредительной границы','','');
INSERT INTO tmplib_base_io VALUES('anUnifSt','HH','Border up alarm violation',3,17,'',5,'Порушення верхньої аварійної границі','','Нарушение верхней аварийной границы','','');
INSERT INTO tmplib_base_io VALUES('anUnifSt','LL','Border down alarm violation',3,17,'',6,'Порушення нижньої аварійної границі','','Нарушение нижней аварийной границы','','');
INSERT INTO tmplib_base_io VALUES('anUnifSt','EVAL','Error value',3,17,'',35,'Помилка значення','','Ошибка значения','','');
INSERT INTO tmplib_base_io VALUES('anUnifSt','ed','Dimension',0,4128,'',8,'Одиниця виміру','','Единица измерения','','');
INSERT INTO tmplib_base_io VALUES('anUnifSt','min','Scale: minimum',2,32,'0',9,'Шкала: мінімум','','Шкала: минимум','','');
INSERT INTO tmplib_base_io VALUES('anUnifSt','max','Scale: maximum',2,32,'100',10,'Шкала: максимум','','Шкала: максимум','','');
INSERT INTO tmplib_base_io VALUES('anUnifSt','scSqr','Scale: square',3,32,'0',11,'Шкала: квадратична','','Шкала: квадратичная','','');
INSERT INTO tmplib_base_io VALUES('anUnifSt','aMin','Border down alarm',2,32,'0',15,'Границя нижня аварійна','','Граница нижняя аварийная','','');
INSERT INTO tmplib_base_io VALUES('anUnifSt','aMax','Border up alarm',2,32,'0',16,'Границя верхня аварійна','','Граница верхняя аварийная','','');
INSERT INTO tmplib_base_io VALUES('anUnifSt','wMin','Border down warning',2,32,'0',17,'Границя нижня попереджув.','','Граница нижняя предупр.','','');
INSERT INTO tmplib_base_io VALUES('anUnifSt','wMax','Border up warning',2,32,'0',18,'Границя верхня попереджув.','','Граница верхняя предупр.','','');
INSERT INTO tmplib_base_io VALUES('anUnifSt','HystBnd','Hysteresis of boders violation',2,32,'0',19,'Гистерезис порушення границь','','Гистерезиз нарушения границ','','');
INSERT INTO tmplib_base_io VALUES('anUnifSt','speed','Motion speed, %/cycle',2,32,'0',20,'Швидкість зміни, %/цикл','','Скорость изменения, %/цикл','','');
INSERT INTO tmplib_base_io VALUES('anUnifSt','prec','Precision, signs',1,32,'2',21,'Точність, знаків','','Точность, знаков','','');
INSERT INTO tmplib_base_io VALUES('anUnifSt','Tf','Filter time, seconds',2,32,'0',23,'Час фільтру, секунд','','Время фильтрации, секунд','','');
INSERT INTO tmplib_base_io VALUES('anUnifSt','iAdd','Input addition',2,64,'0',24,'Додаток до входу','','Дополнение ко входу','','');
INSERT INTO tmplib_base_io VALUES('anUnifSt','iMult','Input multiplication',2,64,'1',25,'Множник до входу','','Множитель входа','','');
INSERT INTO tmplib_base_io VALUES('anUnifSt','plcMin','Minimum module scale',2,64,'1',26,'Мінімум шкали модуля','','Минимум шкалы модуля','','');
INSERT INTO tmplib_base_io VALUES('anUnifSt','plcMax','Maximum module scale',2,64,'5',27,'Максимум шкали модуля','','Максимум шкалы модуля','','');
INSERT INTO tmplib_base_io VALUES('anUnifSt','plcExcess','Allowed scale exceeding, %',2,64,'0',28,'Дозволене перевищення шкали, %','','Допустимое превышение шкалы модуля, %','','');
INSERT INTO tmplib_base_io VALUES('anUnifSt','plcImit','PLC imitation',3,64,'0',29,'Імітація ПЛК','','Имитация ПЛК','','');
INSERT INTO tmplib_base_io VALUES('anUnifSt','plcImitIn','PLC imitation input',2,128,'Imitation|var',30,'Вхід імітації ПЛК','Імітація|var','Вход имитации ПЛК','Имитация|var','');
INSERT INTO tmplib_base_io VALUES('pidUnif','in','Input',2,128,'Signal|in',0,'Вхід','','Вход','','');
INSERT INTO tmplib_base_io VALUES('pidUnif','sp','Setpoint',2,160,'PID|sp',1,'Завдання','ПІД|sp','Задание','ПИД|sp','');
INSERT INTO tmplib_base_io VALUES('pidUnif','var','Variable',2,16,'0',2,'Змінна','','Переменная','','Променљива');
INSERT INTO tmplib_base_io VALUES('pidUnif','manIn','Manual input, %',2,160,'PID|manIn',3,'Ручний ввід, %','ПІД|manIn','Ручной ввод, %','ПИД|manIn','');
INSERT INTO tmplib_base_io VALUES('pidUnif','out','Output, %',2,160,'PID|out',4,'Вихід, %','ПІД|out','Выход, %','ПИД|out','');
INSERT INTO tmplib_base_io VALUES('pidUnif','auto','Mode: automate',3,160,'PID|auto',5,'Режим: автомат','ПІД|auto','Режим: автомат','ПИД|auto','');
INSERT INTO tmplib_base_io VALUES('pidUnif','casc','Mode: cascade',3,160,'PID|casc',6,'Режим: каскад','ПІД|casc','Режим: каскад','ПИД|casc','');
INSERT INTO tmplib_base_io VALUES('pidUnif','ed','Dimension',0,4128,'',7,'Одиниця виміру','','Единица измерения','','');
INSERT INTO tmplib_base_io VALUES('pidUnif','min','Scale: minimum',2,160,'PID|min',8,'Шкала: мінімум','ПІД|min','Шкала: минимум','ПИД|min','');
INSERT INTO tmplib_base_io VALUES('pidUnif','max','Scale: maximum',2,160,'PID|max',9,'Шкала: максимум','ПІД|max','Шкала: максимум','ПИД|max','');
INSERT INTO tmplib_base_io VALUES('pidUnif','scSqr','Scale: square',3,32,'0',10,'Шкала: квадратична','','Шкала: квадратичная','','');
INSERT INTO tmplib_base_io VALUES('pidUnif','aMin','Border down alarm',2,32,'0',13,'Границя нижня аварійна','','Граница нижняя аварийная','','');
INSERT INTO tmplib_base_io VALUES('pidUnif','aMax','Border up alarm',2,32,'0',14,'Границя верхня аварійна','','Граница верхняя аварийная','','');
INSERT INTO tmplib_base_io VALUES('pidUnif','wMin','Border down warning',2,32,'0',15,'Границя нижня попереджув.','','Граница нижняя предупр.','','');
INSERT INTO tmplib_base_io VALUES('pidUnif','wMax','Border up warning',2,32,'0',16,'Границя верхня попереджув.','','Граница верхняя предупр.','','');
INSERT INTO tmplib_base_io VALUES('pidUnif','HystBnd','Hysteresis of boders violation',2,32,'0',17,'Гистерезис порушення границь','','Гистерезиз нарушения границ','','');
INSERT INTO tmplib_base_io VALUES('pidUnif','speed','Motion speed, %/cycle',2,32,'0',18,'Швидкість зміни, %/цикл','','Скорость изменения, %/цикл','','');
INSERT INTO tmplib_base_io VALUES('pidUnif','prec','Precision, signs',1,32,'2',19,'Точність, знаків','','Точность, знаков','','');
INSERT INTO tmplib_base_io VALUES('pidUnif','H','Border up warning violation',3,17,'0',21,'Порушення верхньої попереджув. границі','','Нарушение верхней предупредительной границы','','');
INSERT INTO tmplib_base_io VALUES('pidUnif','L','Border down warning violation',3,17,'0',22,'Порушення нижньої попереджув. границі','','Нарушение нижней предупредительной границы','','');
INSERT INTO tmplib_base_io VALUES('pidUnif','HH','Border up alarm violation',3,17,'0',23,'Порушення верхньої аварійної границі','','Нарушение верхней аварийной границы','','');
INSERT INTO tmplib_base_io VALUES('pidUnif','LL','Border down alarm violation',3,17,'0',24,'Порушення нижньої аварійної границі','','Нарушение нижней аварийной границы','','');
INSERT INTO tmplib_base_io VALUES('pidUnif','EVAL','Value is eval',3,17,'0',25,'Значення невірогідне','','Значение недостоверно','','');
INSERT INTO tmplib_base_io VALUES('pidUnif','Tf','Filter time, milliseconds',2,32,'0',26,'Час фільтру, мілісекунд','','Время фильтра, миллисекунд','','');
INSERT INTO tmplib_base_io VALUES('pidUnif','Kp','Gain',2,160,'PID|Kp',27,'Kп','ПІД|Kp','Kп','ПИД|Kp','');
INSERT INTO tmplib_base_io VALUES('pidUnif','Ti','Ti, milliseconds',1,160,'PID|Ti',28,'Tі, мілісекунд','ПІД|Ti','Tи, миллисекунд','ПИД|Ti','');
INSERT INTO tmplib_base_io VALUES('pidUnif','Kd','Gain differential',2,160,'PID|Kd',29,'Кп диференціалу','ПІД|Kd','Кп производной','ПИД|Kd','');
INSERT INTO tmplib_base_io VALUES('pidUnif','Td','Td, milliseconds',1,160,'PID|Td',30,'Tд, мілісекунд','ПІД|Td','Tд, миллисекунд','ПИД|Td','');
INSERT INTO tmplib_base_io VALUES('pidUnif','Tzd','T differential lag, milliseconds',1,160,'PID|Tzd',31,'T фільтру диференціалу, мілісекунд','ПІД|Tzd','T задержки дифференциала, миллисекунд','ПИД|Tzd','');
INSERT INTO tmplib_base_io VALUES('pidUnif','Zi','Insensitivity, %',2,160,'PID|Zi',32,'Нечутливість, %','ПІД|Zi','Нечувствительность, %','ПИД|Zi','');
INSERT INTO tmplib_base_io VALUES('pidUnif','Hup','Upper output border, %',2,160,'PID|Hup',33,'Верхня границя виходу, %','ПІД|Hup','Верхняя граница выхода, %','ПИД|Hup','');
INSERT INTO tmplib_base_io VALUES('pidUnif','Hdwn','Lower output border, %',2,160,'PID|Hdwn',34,'Нижня границя виходу, %','ПІД|Hdwn','Нижняя граница выхода, %','ПИД|Hdwn','');
INSERT INTO tmplib_base_io VALUES('pidUnif','K1','Coefficient of input 1',2,160,'PID|K1',35,'Коефіцієнт входу 1','ПІД|K1','Коэффициент входа 1','ПИД|K1','');
INSERT INTO tmplib_base_io VALUES('pidUnif','K2','Coefficient of input 2',2,160,'PID|K2',36,'Коефіцієнт входу 2','ПІД|K2','Коэффициент входа 2','ПИД|K2','');
INSERT INTO tmplib_base_io VALUES('pidUnif','K3','Coefficient of input 3',2,160,'PID|K3',37,'Коефіцієнт входу 3','ПІД|K3','Коэффициент входа 3','ПИД|K3','');
INSERT INTO tmplib_base_io VALUES('pidUnif','K4','Coefficient of input 4',2,160,'PID|K4',38,'Коефіцієнт входу 4','ПІД|K4','Коэффициент входа 4','ПИД|K4','');
INSERT INTO tmplib_base_io VALUES('pidUnif','iAdd','Input addition',2,64,'0',39,'Додаток до входу','','Дополнение ко входу','','');
INSERT INTO tmplib_base_io VALUES('pidUnif','iMult','Input multiplication',2,64,'1',40,'Множник до входу','','Множитель входа','','');
INSERT INTO tmplib_base_io VALUES('pidUnif','plcMin','Minimum module scale',2,64,'1',41,'Мінімум шкали модуля','','Минимум шкалы модуля','','');
INSERT INTO tmplib_base_io VALUES('pidUnif','plcMax','Maximum module scale',2,64,'5',42,'Максимум шкали модуля','','Максимум шкалы модуля','','');
INSERT INTO tmplib_base_io VALUES('pidUnif','plcExcess','Allowed scale exceeding, %',2,64,'0',43,'Дозволене перевищення шкали, %','','Допустимое превышение шкалы модуля, %','','');
INSERT INTO tmplib_base_io VALUES('pidUnif','plcImit','PLC imitation',3,64,'0',44,'Імітація ПЛК','','Имитация ПЛК','','');
INSERT INTO tmplib_base_io VALUES('pidUnif','plcImitIn','PLC imitation input',2,128,'Imitation|var',45,'Вхід імітації ПЛК','Імітація|var','Вход имитации ПЛК','Имитация|var','');
INSERT INTO tmplib_base_io VALUES('manInUnif','varIn','Variable input',2,32,'0',2,'Введення змінної','','Ввод переменной','','');
INSERT INTO tmplib_base_io VALUES('pidUnif','SHIFR','Code',0,0,'',47,'Шифр','','Шифр','','');
INSERT INTO tmplib_base_io VALUES('anUnifSt','this','Object',4,1,'',31,'Об''єкт','','Объект','','');
INSERT INTO tmplib_base_io VALUES('anUnifSt','SHIFR','Code',0,0,'',32,'Шифр','','Шифр','','');
INSERT INTO tmplib_base_io VALUES('anUnifSt','NAME','Name',0,0,'',33,'Ім''я','','Имя','','Име');
INSERT INTO tmplib_base_io VALUES('anUnifSt','DESCR','Description',0,0,'',34,'Опис','','Описание','','Опис');
INSERT INTO tmplib_base_io VALUES('anUnif','this','Object',4,1,'',28,'Об''єкт','','Объект','','');
INSERT INTO tmplib_base_io VALUES('pidUnif','NAME','Name',0,0,'',48,'Ім''я','','Имя','','Име');
INSERT INTO tmplib_base_io VALUES('pidUnif','this','Object',4,1,'',46,'Об''єкт','','Объект','','');
INSERT INTO tmplib_base_io VALUES('anUnif','SHIFR','Code',0,0,'',29,'Шифр','','Шифр','','');
INSERT INTO tmplib_base_io VALUES('anUnif','NAME','Name',0,0,'',30,'Ім''я','','Имя','','Име');
INSERT INTO tmplib_base_io VALUES('anUnif','DESCR','Description',0,0,'',31,'Опис','','Описание','','Опис');
INSERT INTO tmplib_base_io VALUES('pidUnif','DESCR','Description',0,0,'',49,'Опис','','Описание','','Опис');
INSERT INTO tmplib_base_io VALUES('pidUnifImp','this','Object',4,1,'',52,'Об''єкт','','Объект','','');
INSERT INTO tmplib_base_io VALUES('pidUnifImp','SHIFR','Code',0,0,'',53,'Шифр','','Шифр','','');
INSERT INTO tmplib_base_io VALUES('pidUnifImp','NAME','Name',0,0,'',54,'Ім''я','','Имя','','Име');
INSERT INTO tmplib_base_io VALUES('pidUnifImp','DESCR','Description',0,0,'',55,'Опис','','Описание','','Опис');
INSERT INTO tmplib_base_io VALUES('anUnif','HystBnd','Hysteresis of boders violation',2,32,'1',16,'Гистерезис порушення границь','','Гистерезиз нарушения границ','','');
INSERT INTO tmplib_base_io VALUES('anUnif','inProc','Input processing procedure',0,68,'',1,'Вхідна процедура обробки','','Входная процедура обработки','','');
INSERT INTO tmplib_base_io VALUES('anUnif','subMode','Substitute: mode',1,40,'0
0;1;2
no;last;substitute',7,'Заміна: режим','0
0;1;2
немає;останнє;підстановка','Замена: режим','0
0;1;2
нет;последнее;подстановка','');
INSERT INTO tmplib_base_io VALUES('anUnif','subVar','Substitute: variable',2,32,'0',8,'Заміна: змінна','','Замена: переменная','','');
INSERT INTO tmplib_base_io VALUES('anUnif','alSup','Violations suppress',3,32,'',9,'Придушення порушень','','Подавление нарушений','','');
INSERT INTO tmplib_base_io VALUES('anUnifSt','inProc','Input processing procedure',0,68,'',1,'Вхідна процедура обробки','','Входная процедура обработки','','');
INSERT INTO tmplib_base_io VALUES('anUnifSt','subMode','Substitute: mode',1,40,'0
0;1;2
no;last;substitute',7,'Заміна: режим','0
0;1;2
немає;останнє;підстановка','Замена: режим','0
0;1;2
нет;последнее;подстановка','');
INSERT INTO tmplib_base_io VALUES('anUnifSt','subVar','Substitute: variable',2,32,'0',12,'Заміна: змінна','','Замена: переменная','','');
INSERT INTO tmplib_base_io VALUES('anUnifSt','alSup','Violations suppress',3,32,'',13,'Придушення порушень','','Подавление нарушений','','');
INSERT INTO tmplib_base_io VALUES('manInUnif','alSup','Violations suppress',3,32,'0',7,'Придушення порушень','','Подавление нарушений','','');
INSERT INTO tmplib_base_io VALUES('manInUnif','HystBnd','Hysteresis of boders violation',2,32,'1',13,'Гистерезис порушення границь','','Гистерезиз нарушения границ','','');
INSERT INTO tmplib_base_io VALUES('manInUnif','prec','Precision, signs',1,32,'2',15,'Точність, знаків','','Точность, знаков','','');
INSERT INTO tmplib_base_io VALUES('manInUnif','this','Object',4,1,'',23,'Об''єкт','','Объект','','');
INSERT INTO tmplib_base_io VALUES('manInUnif','SHIFR','Code',0,0,'',24,'Шифр','','Шифр','','');
INSERT INTO tmplib_base_io VALUES('manInUnif','NAME','Name',0,0,'',25,'Ім''я','','Имя','','Име');
INSERT INTO tmplib_base_io VALUES('manInUnif','DESCR','Description',0,0,'',26,'Опис','','Описание','','Опис');
INSERT INTO tmplib_base_io VALUES('SNMP','srcAddr','Source object''s address',0,64,'',0,'Адреса вихідного об''єкту','','Адрес исходного объекта','','');
INSERT INTO tmplib_base_io VALUES('SNMP','manTables','Manual tables',0,64,'',1,'Ручні таблиці','','Ручные таблицы','','');
INSERT INTO tmplib_base_io VALUES('SNMP','items','All items',4,33,'',4,'Всі елементи','','Все элементы','','');
INSERT INTO tmplib_base_io VALUES('SNMP','this','Object',4,0,'',5,'Об''єкт','','Объект','','');
INSERT INTO tmplib_base_io VALUES('SNMP','SHIFR','Code',0,0,'',6,'Шифр','','Шифр','','');
INSERT INTO tmplib_base_io VALUES('SNMP','NAME','Name',0,0,'',7,'Ім''я','','Имя','','Име');
INSERT INTO tmplib_base_io VALUES('SNMP','DESCR','Description',0,0,'',8,'Опис','','Описание','','Опис');
INSERT INTO tmplib_base_io VALUES('digAlarm','alrm','Alarm "{st}:{lev}:{mess}"',0,64,'',0,'Сигнал "{st}:{lev}:{mess}"','','Сигнал "{st}:{lev}:{mess}"','','');
INSERT INTO tmplib_base_io VALUES('digAlarm','SHIFR','Code',0,0,'',8,'Шифр','','Шифр','','');
INSERT INTO tmplib_base_io VALUES('digAlarm','NAME','Name',0,0,'',9,'Ім''я','','Имя','','Име');
INSERT INTO tmplib_base_io VALUES('digAlarm','DESCR','Description',0,0,'',10,'Опис','','Описание','','Опис');
INSERT INTO tmplib_base_io VALUES('digAlarm','this','Object',4,0,'',11,'Об''єкт','','Объект','','');
INSERT INTO tmplib_base_io VALUES('anUnif','log','Logarithmic scale',3,32,'0',19,'Логарифмічна шкала','','Логарифмическая шкала','','Логаритамска скала');
INSERT INTO tmplib_base_io VALUES('manInUnif','log','Logarithmic scale',3,32,'0',16,'Логарифмічна шкала','','Логарифмическая шкала','','Логаритамска скала');
INSERT INTO tmplib_base_io VALUES('digAlarm','stInv','State inversion',3,64,'',1,'Інверсія стану','','Инверсия состояния','','');
INSERT INTO tmplib_base_io VALUES('digAlarm','st_open','State "Opened"',3,16,'',6,'Стан "Відкрито"','','Состояние "Открыто"','','');
INSERT INTO tmplib_base_io VALUES('digAlarm','st_close','State "Closed"',3,16,'',7,'Стан "Закрито"','','Состояние "Закрыто"','','');
INSERT INTO tmplib_base_io VALUES('anUnifSt','log','Logarithmic scale',3,32,'0',22,'Логарифмічна шкала','','Логарифмическая шкала','','Логаритамска скала');
INSERT INTO tmplib_base_io VALUES('digAlarm','inProc','Input processing procedure',0,68,'',3,'Вхідна процедура обробки','','Входная процедура обработки','','');
INSERT INTO tmplib_base_io VALUES('anUnif','alDelay','Violations delay, seconds',2,32,'0',10,'Затримка порушень, секунди','','Задержка нарушений, секунды','','');
INSERT INTO tmplib_base_io VALUES('anUnifSt','alDelay','Violations delay, seconds',2,32,'',14,'Затримка порушень, секунди','','Задержка нарушений, секунды','','');
INSERT INTO tmplib_base_io VALUES('codeState','in','Input',1,144,'Signal|in',0,'Вхід','','Вход','','');
INSERT INTO tmplib_base_io VALUES('codeState','inProc','Input processing procedure',0,68,'',2,'Вхідна процедура обробки','','Входная процедура обработки','','');
INSERT INTO tmplib_base_io VALUES('codeState','st_text','State "Text"',0,16,'',5,'Стан "Текст"','','Состояние "Текст"','','');
INSERT INTO tmplib_base_io VALUES('codeState','stats','States, rows "{code}:{State}"',0,36,'',6,'Стани, рядки "{code}:{State}"','','Состояния, строки "{code}:{State}"','','');
INSERT INTO tmplib_base_io VALUES('codeState','SHIFR','Code',0,0,'',10,'Шифр','','Шифр','','');
INSERT INTO tmplib_base_io VALUES('codeState','NAME','Name',0,0,'',11,'Ім''я','','Имя','','Име');
INSERT INTO tmplib_base_io VALUES('codeState','DESCR','Description',0,0,'',12,'Опис','','Описание','','Опис');
INSERT INTO tmplib_base_io VALUES('codeState','this','Object',4,0,'',13,'Об''єкт','','Объект','','');
INSERT INTO tmplib_base_io VALUES('ntf','tmOut','Maximum notification timeout, seconds',1,64,'5',2,'Максимальний таймаут повідомлення, секунд','','Максимальный таймаут сообщения, секунд','','');
INSERT INTO tmplib_base_io VALUES('ntf','messLev','Messages level, negative for alarms',1,64,'1',3,'Рівень повідомлень, негативний для порушень','','Уровень сообщений, негативный для нарушений','','');
INSERT INTO tmplib_base_io VALUES('ntf','messCat','Messages category, template or regular expression',0,64,'al*:*',4,'Категорія повідомлень, шаблон або регулярний вираз','','Категория сообщений, шаблон или регулярное выражение','','');
INSERT INTO tmplib_base_io VALUES('ntf','emailState','Email notification current state',0,17,'',5,'Поточний стан повідомлення Ел.поштою','','Текущее состояние уведомления Эл.почтой','','');
INSERT INTO tmplib_base_io VALUES('ntf','emailAuth','Auth, empty for disable',0,64,'user:pass',7,'Автентифікація, порожньо для вимкнення','','Аутентификация, пусто для выключения','','');
INSERT INTO tmplib_base_io VALUES('ntf','emailSender','Sender',0,64,'noreply@oscada.org',8,'Відправник','','Отправитель','','');
INSERT INTO tmplib_base_io VALUES('ntf','emailReceiver','Destination receiver address',0,64,'test@oscada.org',9,'Адреса призначення одержувача','','Адрес назначения получателя','','');
INSERT INTO tmplib_base_io VALUES('ntf','emailTopic','EMail topic',0,4160,'Notification',10,'Тема Ел.пошти','Повідомлення','Тема Эл.почты','Уведомление','');
INSERT INTO tmplib_base_io VALUES('ntf','emailMess','EMail message',0,4160,'',11,'Повідомлення Ел.пошти','','Сообщение Эл.почты','','');
INSERT INTO tmplib_base_io VALUES('ntf','SMSState','SMS notification current state',0,17,'',12,'Поточний стан повідомлення SMS','','Текущее состояние уведомления SMS','','');
INSERT INTO tmplib_base_io VALUES('ntf','SMSPin','SMS-pin, empty for disable',0,64,'1111',14,'Пін-код SMS, порожньо для вимкнення','','Пин-код SMS, пусто для выключения','','');
INSERT INTO tmplib_base_io VALUES('ntf','SMSTel','SMS destination receiver, tel. number',0,64,'+380XXXXXXXXX',15,'Адреса одержувача SMS, номер телефону','','Адрес получателя SMS, номер телефона','','');
INSERT INTO tmplib_base_io VALUES('ntf','SMSTextMd','SMS in the text mode, else PDU',3,64,'0',16,'SMS у текстовому режимі, інакше PDU','','SMS в текстовом режиме, иначе PDU','','');
INSERT INTO tmplib_base_io VALUES('ntf','io','UserPrt: Output IO',4,0,'',0,'UserPrt: Вихідний ВВ','','UserPrt: Выходной ВВ','','');
INSERT INTO tmplib_base_io VALUES('ntf','tr','UserPrt: Transport',4,0,'',1,'UserPrt: Транспорт','','UserPrt: Транспорт','','');
INSERT INTO tmplib_base_io VALUES('ntf','emailTrAddr','Output transport for SMTP connection, empty for disable',0,64,'Sockets.out_SMTP',6,'Вихідний транспорт SMTP підключень, порожньо для вимкнення','','Выходной транспорт SMTP подключений, пусто для выключения','','');
INSERT INTO tmplib_base_io VALUES('ntf','SMSTrAddr','SMS serial transport, empty for disable',0,64,'Serial.out_SMS',13,'Послідовний транспорт SMS, порожньо для вимкнення','','Последовательный транспорт SMS, пусто для выключения','','');
INSERT INTO tmplib_base_io VALUES('codeState','out','Output',1,145,'Signal|out',1,'Вихід','','Выход','','');
INSERT INTO tmplib_base_io VALUES('codeState','com_text','Command "Text"',0,32,'',7,'Команда "Текст"','','Команда "Текст"','','');
INSERT INTO tmplib_base_io VALUES('codeState','coms','Commands-states, rows "{code}:{State}"',0,36,'',8,'Команди-стани, рядки "{code}:{State}"','','Команды-состояния, строки "{code}:{State}"','','');
INSERT INTO tmplib_base_io VALUES('codeState','digComs','Commands',0,20,'',9,'Команди','','Команды','','Команде');
INSERT INTO tmplib_base_io VALUES('UPS','srcAddr','Source object''s address',0,64,'',0,'Адреса вихідного об''єкту','','Адрес исходного объекта','','');
INSERT INTO tmplib_base_io VALUES('UPS','items','All items',4,33,'',3,'Всі елементи','','Все элементы','','');
INSERT INTO tmplib_base_io VALUES('UPS','alDelay','Violations delay, seconds',2,32,'0',2,'Затримка порушень, секунди','','Задержка нарушений, секунды','','');
INSERT INTO tmplib_base_io VALUES('UPS','bChL','Battery charge low',2,32,'20',4,'Низький заряд батареї','','Низкий заряд батареи','','');
INSERT INTO tmplib_base_io VALUES('UPS','bChLL','Battery charge critical',2,32,'5',5,'Критичний заряд батареї','','Критический заряд батареи','','');
INSERT INTO tmplib_base_io VALUES('UPS','inVL','Input voltage low',2,32,'210',6,'Низька вхідна напруга','','Низкое входное напряжение','','');
INSERT INTO tmplib_base_io VALUES('UPS','inVH','Input voltage high',2,32,'250',7,'Висока вхідна напруга','','Высокое входное напряжение','','');
INSERT INTO tmplib_base_io VALUES('UPS','inFLL','Input frequency too low',2,32,'40',8,'Дуже низька вхідна частота','','Очень низкая входная частота','','');
INSERT INTO tmplib_base_io VALUES('UPS','inFL','Input frequency low',2,32,'45',9,'Низька вхідна частота','','Низкая входная частота','','');
INSERT INTO tmplib_base_io VALUES('UPS','inFH','Input frequency high',2,32,'55',10,'Висока вхідна частота','','Высокая входная частота','','');
INSERT INTO tmplib_base_io VALUES('UPS','inFHH','Input frequency too high',2,32,'60',11,'Дуже висока вхідна частота','','Очень высокая входная частота','','');
INSERT INTO tmplib_base_io VALUES('UPS','loadH','Load high',2,32,'80',12,'Навантаження високе','','Нагрузка высокая','','');
INSERT INTO tmplib_base_io VALUES('UPS','loadHH','Load too high',2,32,'100',13,'Навантаження дуже високе','','Нагрузка очень высокая','','');
INSERT INTO tmplib_base_io VALUES('UPS','tH','Temperature high',2,32,'50',14,'Висока температура','','Высокая температура','','');
INSERT INTO tmplib_base_io VALUES('UPS','tHH','Temperature too high',2,32,'70',15,'Температура дуже висока','','Температура очень высокая','','');
INSERT INTO tmplib_base_io VALUES('UPS','this','Object',4,0,'',16,'Об''єкт','','Объект','','');
INSERT INTO tmplib_base_io VALUES('UPS','SHIFR','Code',0,0,'',17,'Шифр','','Шифр','','');
INSERT INTO tmplib_base_io VALUES('UPS','NAME','Name',0,0,'',18,'Ім''я','','Имя','','Име');
INSERT INTO tmplib_base_io VALUES('UPS','DESCR','Description',0,0,'',19,'Опис','','Описание','','Опис');
INSERT INTO tmplib_base_io VALUES('initConsAssignTrs','inTransport','Input transport',0,64,'InitiateCons',0,'Вхідний транспорт','','Входной транспорт','','');
INSERT INTO tmplib_base_io VALUES('initConsAssignTrs','outTrTm','Output transport timeouts',0,64,'10:0.1',1,'Таймаути вихідних транспортів','','Таймауты выходных транспортов','','');
INSERT INTO tmplib_base_io VALUES('initConsAssignTrs','prcTr','Processed transports',4,17,'',3,'Опрацьовані транспорти','','Обработанные транспорты','','');
INSERT INTO tmplib_base_io VALUES('initConsAssignTrs','srcObjPath','Source object path',0,64,'ModBus:%2fcntr%2fcfg%2fADDR',2,'Шлях об''єкту джерела','','Путь объекта источника','','');
INSERT INTO tmplib_base_io VALUES('SNMP','alSup','Violations suppress',3,32,'',2,'Придушення порушень','','Подавление нарушений','','');
INSERT INTO tmplib_base_io VALUES('SNMP','alDelay','Violations delay, seconds',2,32,'0',3,'Затримка порушень, секунди','','Задержка нарушений, секунды','','');
INSERT INTO tmplib_base_io VALUES('UPS','alSup','Violations suppress',3,32,'',1,'Придушення порушень','','Подавление нарушений','','');
INSERT INTO tmplib_base_io VALUES('codeState','alSup','Violations suppress',3,32,'',3,'Придушення порушень','','Подавление нарушений','','');
INSERT INTO tmplib_base_io VALUES('codeState','alDelay','Violations delay, seconds',2,32,'0',4,'Затримка порушень, секунди','','Задержка нарушений, секунды','','');
INSERT INTO tmplib_base_io VALUES('digAlarm','alSup','Violations suppress',3,32,'',4,'Придушення порушень','','Подавление нарушений','','');
INSERT INTO tmplib_base_io VALUES('digAlarm','alDelay','Violations delay, seconds',2,32,'0',5,'Затримка порушень, секунди','','Задержка нарушений, секунды','','');
INSERT INTO tmplib_base_io VALUES('manInUnif','alDelay','Violations delay, seconds',3,32,'',8,'Затримка порушень, секунди','','Задержка нарушений, секунды','','');
INSERT INTO tmplib_base_io VALUES('pidUnif','alSup','Violations suppress',3,32,'0',11,'Придушення порушень','','Подавление нарушений','','');
INSERT INTO tmplib_base_io VALUES('pidUnif','alDelay','Violations delay, seconds',2,32,'0',12,'Затримка порушень, секунди','','Задержка нарушений, секунды','','');
INSERT INTO tmplib_base_io VALUES('pidUnif','log','Logarithmic scale',3,32,'0',20,'Логарифмічна шкала','','Логарифмическая шкала','','Логаритамска скала');
INSERT INTO tmplib_base_io VALUES('pidUnifImp','alSup','Violations suppress',3,32,'0',13,'Придушення порушень','','Подавление нарушений','','');
INSERT INTO tmplib_base_io VALUES('pidUnifImp','alDelay','Violations delay, seconds',2,32,'0',14,'Затримка порушень, секунди','','Задержка нарушений, секунды','','');
INSERT INTO tmplib_base_io VALUES('pidUnifImp','log','Logarithmic scale',3,32,'0',22,'Логарифмічна шкала','','Логарифмическая шкала','','Логаритамска скала');
INSERT INTO tmplib_base_io VALUES('anUnif','alNormForceStart','Force NORM violation at the start',3,32,'0',11,'Примусове порушення НОРМА при запуску','','Принудительное нарушение НОРМА при запуске','','');
CREATE TABLE IF NOT EXISTS 'tmplib_DevLib_io' ("TMPL_ID" TEXT DEFAULT '' ,"ID" TEXT DEFAULT '' ,"NAME" TEXT DEFAULT '' ,"TYPE" INTEGER DEFAULT '' ,"FLAGS" INTEGER DEFAULT '' ,"VALUE" TEXT DEFAULT '' ,"POS" INTEGER DEFAULT '' ,"ru#NAME" TEXT DEFAULT '' ,"ru#VALUE" TEXT DEFAULT '' ,"uk#NAME" TEXT DEFAULT '' ,"uk#VALUE" TEXT DEFAULT '' ,"sr#NAME" TEXT DEFAULT '' , PRIMARY KEY ("TMPL_ID","ID"));
INSERT INTO tmplib_DevLib_io VALUES('SCU750','transport','Transport',0,64,'SCU750',0,'Транспорт','','Транспорт','','');
INSERT INTO tmplib_DevLib_io VALUES('SCU750','addr','Device address (-1...255)',1,64,'1',1,'Адрес устройства (-1...255)','','Адреса пристрою (-1...255)','','');
INSERT INTO tmplib_DevLib_io VALUES('SCU750','perGet','Period data getting (s)',2,64,'10',2,'Период получения данных (с)','','Період отримання даних (с)','','');
INSERT INTO tmplib_DevLib_io VALUES('SCU750','version','Version',0,16,'',3,'Версия','','Версія','','');
INSERT INTO tmplib_DevLib_io VALUES('SCU750','snCntrUnit','Serial number: Control Unit',0,16,'',4,'Серийный номер: Блок Управления','','Серійний номер: Блок Керування','','');
INSERT INTO tmplib_DevLib_io VALUES('SCU750','snPump','Serial number: Pump',0,16,'',5,'Серийный номер: Насос','','Серійний номер: Насос','','');
INSERT INTO tmplib_DevLib_io VALUES('SCU750','cntPumpHour','Pump hour counter (minutes)',2,16,'',6,'Счётчик часов насоса (минуты)','','Лічильник годин насосу (хвилини)','','');
INSERT INTO tmplib_DevLib_io VALUES('SCU750','cntCntrUnitHour','Control unit hour counter (minutes)',2,16,'',7,'Счётчик часов объекта контроля (минуты)','','Лічильник годин об''єкта контролю (хвилини)','','');
INSERT INTO tmplib_DevLib_io VALUES('SCU750','cntStart','Start counter',2,16,'',8,'Запуск счётчика','','Пуск лічильника','','');
INSERT INTO tmplib_DevLib_io VALUES('SCU750','operMode','Pump Operation Mode',0,16,'',9,'Режим работы насоса','','Режим роботи насосу','','');
INSERT INTO tmplib_DevLib_io VALUES('SCU750','errors','Errors',0,16,'',10,'Ошибки','','Помилки','','');
INSERT INTO tmplib_DevLib_io VALUES('SCU750','events','Events',0,16,'',11,'События','','Події','','');
INSERT INTO tmplib_DevLib_io VALUES('SCU750','tTMS','TMS temperature (°С)',1,16,'',12,'TMS температура (°С)','','TMS температура (°С)','','');
INSERT INTO tmplib_DevLib_io VALUES('SCU750','tMotor','Motor temperature (°С)',1,16,'',13,'Температура мотора (°С)','','Температура мотору (°С)','','');
INSERT INTO tmplib_DevLib_io VALUES('SCU750','rotSpeed','Rotational speed (HZ)',1,16,'',14,'Скорость вращения (Гц)','','Швидкість обертання (Гц)','','');
INSERT INTO tmplib_DevLib_io VALUES('SCU750','comStart','Command: START',3,32,'',15,'Команда: ЗАПУСК','','Команда: ЗАПУСК','','');
INSERT INTO tmplib_DevLib_io VALUES('SCU750','comStop','Command: STOP',3,32,'',16,'Команда: СТОП','','Команда: СТОП','','');
INSERT INTO tmplib_DevLib_io VALUES('SCU750','comReset','Command: RESET',3,32,'',17,'Команда: СБРОС','','Команда: СКИД','','');
INSERT INTO tmplib_DevLib_io VALUES('SCU750','spSpeed','Set point: Speed',1,32,'',18,'Задание: Скорость','','Завдання: Швидкість','','');
INSERT INTO tmplib_DevLib_io VALUES('SCU750','spTMSt','Set point: TMS temperature',1,16,'',19,'Задание: температура TMS','','Завдання: температура TMS','','');
INSERT INTO tmplib_DevLib_io VALUES('SCU750','spSpeedPrev','Set point: Speed (previous)',1,1,'',20,'Задание: Скорость (предыдущая)','','Завдання: Швидкість (попередня)','','');
INSERT INTO tmplib_DevLib_io VALUES('SCU750','cnt','Counter',1,1,'',21,'Счётчик','','Лічильник','','');
INSERT INTO tmplib_DevLib_io VALUES('SCU750','operModes','Operation modes',4,1,'',22,'Режимы работы','','Режими роботи','','');
INSERT INTO tmplib_DevLib_io VALUES('TMH','transport','Transport',0,64,'TMH',0,'Транспорт','','Транспорт','','');
INSERT INTO tmplib_DevLib_io VALUES('TMH','addr','Device address (-1...255)',1,64,'1',1,'Адрес устройства (-1...255)','','Адреса пристрою (-1...255)','','');
INSERT INTO tmplib_DevLib_io VALUES('TMH','perGet','Period data getting (s)',2,64,'10',2,'Период получения данных (с)','','Період отримання даних (с)','','');
INSERT INTO tmplib_DevLib_io VALUES('TMH','modelID','Model ID',0,16,'',3,'ID модели','','ID моделі','','');
INSERT INTO tmplib_DevLib_io VALUES('TMH','errors','Errors',0,16,'',4,'Ошибки','','Помилки','','');
INSERT INTO tmplib_DevLib_io VALUES('TMH','rotSpeed','Rotational speed (HZ)',1,16,'',5,'Скорость вращения (Гц)','','Швидкість обертання (Гц)','','');
INSERT INTO tmplib_DevLib_io VALUES('TMH','I','Current (A)',2,16,'',6,'Ток (А)','','Струм (А)','','');
INSERT INTO tmplib_DevLib_io VALUES('TMH','axle1disb','Axle 1 disbalance',1,16,'',7,'Нарушение границы 1','','Порушення границі 1','','');
INSERT INTO tmplib_DevLib_io VALUES('TMH','axle2disb','Axle 2 disbalance',1,16,'',8,'Нарушение границы 2','','Порушення границі 2','','');
INSERT INTO tmplib_DevLib_io VALUES('TMH','MP_X1','MP X1',1,16,'',9,'','','','','');
INSERT INTO tmplib_DevLib_io VALUES('TMH','MP_Y1','MP Y1',1,16,'',10,'','','','','');
INSERT INTO tmplib_DevLib_io VALUES('TMH','MP_X2','MP X2',1,16,'',11,'','','','','');
INSERT INTO tmplib_DevLib_io VALUES('TMH','MP_Y2','MP Y2',1,16,'',12,'','','','','');
INSERT INTO tmplib_DevLib_io VALUES('TMH','MP_Z','MP Z',1,16,'',13,'','','','','');
INSERT INTO tmplib_DevLib_io VALUES('TMH','operMode','Operation Mode',0,16,'',14,'Режим работы','','Режим роботи','','');
INSERT INTO tmplib_DevLib_io VALUES('TMH','comStart','Command: START',3,32,'',15,'Команда: ЗАПУСК','','Команда: ЗАПУСК','','');
INSERT INTO tmplib_DevLib_io VALUES('TMH','comStop','Command: STOP',3,32,'',16,'Команда: СТОП','','Команда: СТОП','','');
INSERT INTO tmplib_DevLib_io VALUES('TMH','comReset','Command: RESET',3,32,'',17,'Команда: СБРОС','','Команда: СКИД','','');
INSERT INTO tmplib_DevLib_io VALUES('TMH','operCntr','Operation Control Mode',0,16,'',18,'Режим работы и контроля','','Режим роботи та контролю','','');
INSERT INTO tmplib_DevLib_io VALUES('TMH','comInteract','Interactive mode',3,32,'',19,'Интерактивный режим','','Інтерактивний режим','','');
INSERT INTO tmplib_DevLib_io VALUES('TMH','comAutonom','Autonomous mode',3,32,'',20,'Автономный режим','','Автономний режим','','');
INSERT INTO tmplib_DevLib_io VALUES('TMH','cnt','Counter',1,1,'',21,'Счётчик','','Лічильник','','');
INSERT INTO tmplib_DevLib_io VALUES('TMH','operModes','Operation modes',4,1,'',22,'Режимы работы','','Режими роботи','','');
INSERT INTO tmplib_DevLib_io VALUES('TM510x','transport','Transport',0,64,'tm510x',0,'Транспорт','','Транспорт','','');
INSERT INTO tmplib_DevLib_io VALUES('TM510x','addr','Device address (1...254)',1,64,'1',1,'Адрес устройства (1...254)','','Адреса пристрою (1...254)','','');
INSERT INTO tmplib_DevLib_io VALUES('TM510x','devTp','Device type',1,16,'',2,'Тип устройства','','Тип пристрою','','');
INSERT INTO tmplib_DevLib_io VALUES('TM510x','errors','Errors',0,16,'',3,'Ошибки','','Помилки','','');
INSERT INTO tmplib_DevLib_io VALUES('TM510x','in1','Input 1',2,16,'',4,'Вход 1','','Вхід 1','','');
INSERT INTO tmplib_DevLib_io VALUES('TM510x','in2','Input 2',2,16,'',5,'Вход 2','','Вхід 2','','');
INSERT INTO tmplib_DevLib_io VALUES('TM510x','in3','Input 3',2,16,'',6,'Вход 3','','Вхід 3','','');
INSERT INTO tmplib_DevLib_io VALUES('TM510x','in4','Input 4',2,16,'',7,'Вход 4','','Вхід 4','','');
INSERT INTO tmplib_DevLib_io VALUES('TM510x','in5','Input 5',2,16,'',8,'Вход 5','','Вхід 5','','');
INSERT INTO tmplib_DevLib_io VALUES('TM510x','in6','Input 6',2,16,'',9,'Вход 6','','Вхід 6','','');
INSERT INTO tmplib_DevLib_io VALUES('TM510x','in7','Input 7',2,16,'',10,'Вход 7','','Вхід 7','','');
INSERT INTO tmplib_DevLib_io VALUES('TM510x','in8','Input 8',2,16,'',11,'Вход 8','','Вхід 8','','');
INSERT INTO tmplib_DevLib_io VALUES('UPS','srcAddr','Source object''s address',0,64,'',0,'Адрес исходного объекта','','Адреса вихідного об''єкту','','');
INSERT INTO tmplib_DevLib_io VALUES('UPS','items','All items',4,33,'',1,'Все элементы','','Всі елементи','','');
INSERT INTO tmplib_DevLib_io VALUES('UPS','this','Object',4,0,'',15,'Объект','','Об''єкт','','');
INSERT INTO tmplib_DevLib_io VALUES('UPS','SHIFR','Code',0,0,'',16,'Шифр','','Шифр','','');
INSERT INTO tmplib_DevLib_io VALUES('UPS','NAME','Name',0,0,'',17,'Имя','','Ім''я','','Име');
INSERT INTO tmplib_DevLib_io VALUES('UPS','DESCR','Description',0,0,'',18,'Описание','','Опис','','Опис');
INSERT INTO tmplib_DevLib_io VALUES('VKT7','imit','Imitation drift % (0-disable)',2,64,'0',0,'Дрейф имитации % (0-отключен)','','Дрейф імітації % (0-відключено)','','');
INSERT INTO tmplib_DevLib_io VALUES('VKT7','trAddr','Transport',0,64,'Transport.Serial.out_VKT7',1,'Транспорт','','Транспорт','','');
INSERT INTO tmplib_DevLib_io VALUES('VKT7','arhH','Archiver: hours',0,64,'',2,'Архиватор: часы','','Архіватор: години','','');
INSERT INTO tmplib_DevLib_io VALUES('VKT7','arhD','Archiver: days',0,64,'',3,'Архиватор: дни','','Архіватор: дні','','');
INSERT INTO tmplib_DevLib_io VALUES('VKT7','arhRes','Archiver: results-month',0,64,'',4,'Архиватор: результаты-месяцы','','Архіватор: результати-місяці','','');
INSERT INTO tmplib_DevLib_io VALUES('VKT7','maxDayDepth','Archiver: maximum depth of reading for not hours archivers, days',1,64,'366',5,'Архиватор: максимум читаемой глубины для нечасового архиватора, дней','','Архіватор: максимум глибини читання для негодинного архіватора, днів','','');
INSERT INTO tmplib_DevLib_io VALUES('VKT7','lastResTm','Last result months read time (s)',1,33,'0',6,'Время последнего чтения результирующих месяцев (с)','','Час останього читання результуючих місяців (с)','','');
INSERT INTO tmplib_DevLib_io VALUES('VKT7','leftResTm','Left result months for read from archive',1,17,'',7,'Осталось результ. месяцев для чтения из архива','','Залишилося результ. місяців для читання із архіву','','');
INSERT INTO tmplib_DevLib_io VALUES('VKT7','lastDTm','Last days read time (s)',1,33,'0',8,'Время последнего чтения дней (с)','','Час останього читання днів (с)','','');
INSERT INTO tmplib_DevLib_io VALUES('VKT7','leftDTm','Left days for read from archive',1,17,'',9,'Осталось дней для чтения из архива','','Залишилося днів для читання із архіву','','');
INSERT INTO tmplib_DevLib_io VALUES('VKT7','lastHTm','Last hours read time (s)',1,33,'',10,'Время последнего чтения часов (с)','','Час останього читання годин (с)','','');
INSERT INTO tmplib_DevLib_io VALUES('VKT7','leftHTm','Left hours for read from archive',1,17,'',11,'Осталось часов для чтения из архива','','Залишилося годин для читання із архіву','','');
INSERT INTO tmplib_DevLib_io VALUES('VKT7','diffTm','Difference time (server-counter), hours',1,16,'',12,'Разница времени (сервер-счётчик), часы','','Різниця часу (сервер-лічильник), години','','');
INSERT INTO tmplib_DevLib_io VALUES('VKT7','firmVer','Firmware version',2,16,'',13,'Версия прошивки','','Версія прошивки','','');
INSERT INTO tmplib_DevLib_io VALUES('VKT7','abonId','Subscriber ID',0,16,'',14,'ID абонента','','ID абоненту','','');
INSERT INTO tmplib_DevLib_io VALUES('VKT7','repDay','Report day',1,16,'',15,'День отчёта','','День звіту','','');
INSERT INTO tmplib_DevLib_io VALUES('VKT7','netNum','Network number',1,16,'',16,'Сетевой номер','','Мережевий номер','','');
INSERT INTO tmplib_DevLib_io VALUES('VKT7','modelImpl','Model implementation',1,16,'',17,'Реализация модели','','Реалізація моделі','','');
INSERT INTO tmplib_DevLib_io VALUES('VKT7','tTypeM','t: dimension',0,16,'',18,'t: ед. измерения','','t: од. виміру','','');
INSERT INTO tmplib_DevLib_io VALUES('VKT7','GTypeM','G: dimension',0,16,'',19,'G: ед. измерения','','G: од. виміру','','');
INSERT INTO tmplib_DevLib_io VALUES('VKT7','VTypeM','V: dimension',0,16,'',20,'V: ед. измерения','','V: од. виміру','','');
INSERT INTO tmplib_DevLib_io VALUES('VKT7','MTypeM','M: dimension',0,16,'',21,'M: ед.измерения','','M: од.вимірювання','','');
INSERT INTO tmplib_DevLib_io VALUES('VKT7','PTypeM','P: dimension',0,16,'',22,'P: ед. измерения','','P: од.виміру','','');
INSERT INTO tmplib_DevLib_io VALUES('VKT7','QoTypeM','Qo: dimension',0,16,'',23,'Qo: ед. измерения','','Qo: од. виміру','','');
INSERT INTO tmplib_DevLib_io VALUES('VKT7','QntTypeHIM','ВНР: dimension',0,16,'',24,'ВНР: ед.измерения','','ВНР: од.виміру','','');
INSERT INTO tmplib_DevLib_io VALUES('VKT7','QntTypeM','ВОК: dimension',0,16,'',25,'ВОК: ед.измерения','','ВОК: од.виміру','','');
INSERT INTO tmplib_DevLib_io VALUES('VKT7','t1_1','t1 (Tв1)',2,32,'',26,'','','','','');
INSERT INTO tmplib_DevLib_io VALUES('VKT7','t2_1','t2 (Tв1)',2,32,'',27,'','','','','');
INSERT INTO tmplib_DevLib_io VALUES('VKT7','t3_1','t3 (Tв1)',2,32,'',28,'','','','','');
INSERT INTO tmplib_DevLib_io VALUES('VKT7','V1_1','V1 (Tв1)',2,32,'',29,'','','','','');
INSERT INTO tmplib_DevLib_io VALUES('VKT7','V2_1','V2 (Tв1)',2,32,'',30,'','','','','');
INSERT INTO tmplib_DevLib_io VALUES('VKT7','V3_1','V3 (Tв1)',2,32,'',31,'','','','','');
INSERT INTO tmplib_DevLib_io VALUES('VKT7','M1_1','M1 (Tв1)',2,32,'',32,'','','','','');
INSERT INTO tmplib_DevLib_io VALUES('VKT7','M2_1','M2 (Tв1)',2,32,'',33,'','','','','');
INSERT INTO tmplib_DevLib_io VALUES('VKT7','M3_1','M3 (Tв1)',2,32,'',34,'','','','','');
INSERT INTO tmplib_DevLib_io VALUES('VKT7','P1_1','P1 (Tв1)',2,32,'',35,'','','','','');
INSERT INTO tmplib_DevLib_io VALUES('VKT7','P2_1','P2 (Tв1)',2,32,'',36,'','','','','');
INSERT INTO tmplib_DevLib_io VALUES('VKT7','Mg_1','Mg (Tв1)',2,32,'',37,'','','','','');
INSERT INTO tmplib_DevLib_io VALUES('VKT7','Qo_1','Qo (Tв1)',2,32,'',38,'','','','','');
INSERT INTO tmplib_DevLib_io VALUES('VKT7','Qg_1','Qg (Tв1)',2,32,'',39,'','','','','');
INSERT INTO tmplib_DevLib_io VALUES('VKT7','dt_1','dt (Tв1)',2,32,'',40,'','','','','');
INSERT INTO tmplib_DevLib_io VALUES('VKT7','BNR_1','ВНР (Tв1)',2,32,'',41,'','','','','');
INSERT INTO tmplib_DevLib_io VALUES('VKT7','BOC_1','ВОС (Tв1)',2,32,'',42,'','','','','');
INSERT INTO tmplib_DevLib_io VALUES('VKT7','G1_1','G1 (Tв1)',2,32,'',43,'','','','','');
INSERT INTO tmplib_DevLib_io VALUES('VKT7','G2_1','G2 (Tв1)',2,32,'',44,'','','','','');
INSERT INTO tmplib_DevLib_io VALUES('VKT7','t1_2','t1 (Tв2)',2,32,'',45,'','','','','');
INSERT INTO tmplib_DevLib_io VALUES('VKT7','t2_2','t2 (Tв2)',2,32,'',46,'','','','','');
INSERT INTO tmplib_DevLib_io VALUES('VKT7','t3_2','t3 (Tв2)',2,32,'',47,'','','','','');
INSERT INTO tmplib_DevLib_io VALUES('VKT7','V1_2','V1 (Tв2)',2,32,'',48,'','','','','');
INSERT INTO tmplib_DevLib_io VALUES('VKT7','V2_2','V2 (Tв2)',2,32,'',49,'','','','','');
INSERT INTO tmplib_DevLib_io VALUES('VKT7','V3_2','V3 (Tв2)',2,32,'',50,'','','','','');
INSERT INTO tmplib_DevLib_io VALUES('VKT7','M1_2','M1 (Tв2)',2,32,'',51,'','','','','');
INSERT INTO tmplib_DevLib_io VALUES('VKT7','M2_2','M2 (Tв2)',2,32,'',52,'','','','','');
INSERT INTO tmplib_DevLib_io VALUES('VKT7','M3_2','M3 (Tв2)',2,32,'',53,'','','','','');
INSERT INTO tmplib_DevLib_io VALUES('VKT7','P1_2','P1 (Tв2)',2,32,'',54,'','','','','');
INSERT INTO tmplib_DevLib_io VALUES('VKT7','P2_2','P2 (Tв2)',2,32,'',55,'','','','','');
INSERT INTO tmplib_DevLib_io VALUES('VKT7','Mg_2','Mg (Tв2)',2,32,'',56,'','','','','');
INSERT INTO tmplib_DevLib_io VALUES('VKT7','Qo_2','Qo (Tв2)',2,32,'',57,'','','','','');
INSERT INTO tmplib_DevLib_io VALUES('VKT7','Qg_2','Qg (Tв2)',2,32,'',58,'','','','','');
INSERT INTO tmplib_DevLib_io VALUES('VKT7','dt_2','dt (Tв2)',2,32,'',59,'','','','','');
INSERT INTO tmplib_DevLib_io VALUES('VKT7','BNR_2','ВНР (Tв2)',2,32,'',60,'','','','','');
INSERT INTO tmplib_DevLib_io VALUES('VKT7','BOC_2','ВОС (Tв2)',2,32,'',61,'','','','','');
INSERT INTO tmplib_DevLib_io VALUES('VKT7','G1_2','G1 (Tв2)',2,32,'',62,'','','','','');
INSERT INTO tmplib_DevLib_io VALUES('VKT7','G2_2','G2 (Tв2)',2,32,'',63,'','','','','');
INSERT INTO tmplib_DevLib_io VALUES('VKT7','this','This parameter object',4,0,'',64,'Объект параметра','','Об''єкт параметру','','');
INSERT INTO tmplib_DevLib_io VALUES('SMDP','transport','Transport',0,64,'SMDP',0,'Транспорт','','Транспорт','','');
INSERT INTO tmplib_DevLib_io VALUES('SMDP','addr','Device address (16...254)',1,64,'16',1,'Адрес устройства (16...254)','','Адреса пристрою (16...254)','','');
INSERT INTO tmplib_DevLib_io VALUES('SMDP','perGet','Period of the data getting, s',2,64,'10',2,'Период сбора данных, с','','Період збору даних, с','','');
INSERT INTO tmplib_DevLib_io VALUES('SMDP','version','Firmware version',0,16,'',3,'Версия прошивки','','Версія прошивки','','');
INSERT INTO tmplib_DevLib_io VALUES('SMDP','CfgPrmSSID','Configuration parameter session ID',1,16,'',4,'Параметр конфигурации ID сеанса','','Параметр конфігурації ID сеансу','','');
INSERT INTO tmplib_DevLib_io VALUES('SMDP','Srlno','Measurement serial number',1,16,'',5,'Измерянный номер серии','','Виміряний номер серії','','');
INSERT INTO tmplib_DevLib_io VALUES('SMDP','RawFreq','Channel freq.',2,16,'',6,'Частота канала.','','Частота каналу.','','');
INSERT INTO tmplib_DevLib_io VALUES('SMDP','GoodFreq','Last used, good channel freq., Hz',2,16,'',7,'Последне использ.: частота хорошего канала, Гц','','Останнє використ.: частота хорошого каналу, Гц','','');
INSERT INTO tmplib_DevLib_io VALUES('SMDP','RawThick','Computed raw thickness, from frequency. (A)',2,16,'',8,'Вычисленная сырая толщина, с частоты. (А)','','Обчислена сира товщина, з частоти. (A)','','');
INSERT INTO tmplib_DevLib_io VALUES('SMDP','XtalThick','Computed material related thickness, can be zeroed. (A)',2,16,'',9,'Вычисленная относительная толщина материала, может быть нулевой. (A)','','Обчислена відносна товщина матеріалу, може бути нульовою. (A)','','');
INSERT INTO tmplib_DevLib_io VALUES('SMDP','XtalThick_F','Filtered computed material related thickness, can be zeroed. (A)',2,16,'',10,'Фильтрованная вычисленная относительная толщина материала, может быть нулевой. (A)','','Фільтрована обчислена відносна товщина матеріалу, може бути нульовою. (A)','','');
INSERT INTO tmplib_DevLib_io VALUES('SMDP','XtalRate','Rate, angstroms per second.',2,16,'',11,'','','','','');
INSERT INTO tmplib_DevLib_io VALUES('SMDP','XtalRate_F','Rate, angstroms per second, filtered.',2,16,'',12,'','','','','');
INSERT INTO tmplib_DevLib_io VALUES('SMDP','XtalLife','XtalLife (%)',2,16,'',13,'','','','','');
INSERT INTO tmplib_DevLib_io VALUES('SMDP','XtalQual','Quality level (0-9).',1,16,'',14,'','','','','');
INSERT INTO tmplib_DevLib_io VALUES('SMDP','XtalQualPeak','Highest quality level seen (0-9).',1,16,'',15,'','','','','');
INSERT INTO tmplib_DevLib_io VALUES('SMDP','XtalStab','Stability level (0-9).',1,16,'',16,'','','','','');
INSERT INTO tmplib_DevLib_io VALUES('SMDP','XtalStabPeak','Highest stability level seen (0-9).',1,16,'',17,'','','','','');
INSERT INTO tmplib_DevLib_io VALUES('SMDP','XtalStat','Channel status.',0,16,'',18,'Статус канала.','','Статус каналу.','','');
INSERT INTO tmplib_DevLib_io VALUES('SMDP','XtalLife_C','XtalLife (%).',1,16,'',19,'','','','','');
INSERT INTO tmplib_DevLib_io VALUES('SMDP','SessId','Session ID',1,32,'',20,'ID сеанса','','ID сеансу','','');
INSERT INTO tmplib_DevLib_io VALUES('SMDP','Fq','Xtal start freq. (Hz)',2,32,'',21,'','','','','');
INSERT INTO tmplib_DevLib_io VALUES('SMDP','Fm','Xtal min freq. (Hz)',2,32,'',22,'','','','','');
INSERT INTO tmplib_DevLib_io VALUES('SMDP','Density','Material density (gm/cc).',2,32,'',23,'','','','','');
INSERT INTO tmplib_DevLib_io VALUES('SMDP','Zratio','Material Z ratio. Not scaled or unitized.',2,32,'',24,'','','','','');
INSERT INTO tmplib_DevLib_io VALUES('SMDP','Tooling','1.000 is 100 % tooling (unity).',2,32,'',25,'1.000 есть 100 % настройка (объединение).','','1.000 є 100 % налаштування (об''єднання).','','');
INSERT INTO tmplib_DevLib_io VALUES('SMDP','RateReq','Requested rate (A/S).',2,32,'',26,'','','','','');
INSERT INTO tmplib_DevLib_io VALUES('SMDP','QlvlTrip','Quality threshold, if non 0 and exceeded, xtal fail occurs.',1,32,'',27,'','','','','');
INSERT INTO tmplib_DevLib_io VALUES('SMDP','SlvlTrip','Stability threshold, if non 0 and exceeded, xtal fail occurs.',1,32,'',28,'','','','','');
INSERT INTO tmplib_DevLib_io VALUES('IT3','transport','Transport',0,64,'IT3',0,'Транспорт','','Транспорт','','');
INSERT INTO tmplib_DevLib_io VALUES('IT3','addr','Device address',1,64,'1',1,'Адрес устройства','','Адреса пристрою','','');
INSERT INTO tmplib_DevLib_io VALUES('IT3','T','Temperature',2,16,'',3,'Температура','','Температура','','Температура');
INSERT INTO tmplib_DevLib_io VALUES('IT3','H','Upper border',3,16,'',4,'Верхняя граница','','Верхня границя','','');
INSERT INTO tmplib_DevLib_io VALUES('IT3','L','Lower border',3,16,'',5,'Нижняя граница','','Нижня границя','','');
INSERT INTO tmplib_DevLib_io VALUES('IT3','relSt','Relay state',3,16,'',6,'Состояние реле','','Стан реле','','');
INSERT INTO tmplib_DevLib_io VALUES('IVE_452HS_02','transport','Transport',0,64,'Sockets.out_IVE',0,'Транспорт','','Транспорт','','');
INSERT INTO tmplib_DevLib_io VALUES('IVE_452HS_02','addr','Device address',1,64,'255',1,'Адрес устройства','','Адреса пристрою','','');
INSERT INTO tmplib_DevLib_io VALUES('IVE_452HS_02','COIA','Output current level set-point',1,33,'',2,'Задание уровня выходного тока','','Завдання рівня вихідного струму','','');
INSERT INTO tmplib_DevLib_io VALUES('IVE_452HS_02','COUA','Output voltage level set-point',1,33,'',3,'Задание уровня выходного напряжения','','Завдання рівня вихідної напруги','','');
INSERT INTO tmplib_DevLib_io VALUES('IVE_452HS_02','COPA','Output power level set-point',1,33,'',4,'Задание уровня выходного мощности','','Завдання рівня вихідної потужності','','');
INSERT INTO tmplib_DevLib_io VALUES('IVE_452HS_02','DIA','Output current',1,16,'',5,'Выходной ток','','Вихідний струм','','');
INSERT INTO tmplib_DevLib_io VALUES('IVE_452HS_02','DUA','Output voltage',1,16,'',6,'Выходное напряжение','','Вихідна напруга','','');
INSERT INTO tmplib_DevLib_io VALUES('IVE_452HS_02','DPA','Output power',1,16,'',7,'Выходная мощность','','Вихідна потужність','','');
INSERT INTO tmplib_DevLib_io VALUES('IVE_452HS_02','DFA','Electric arcs frequency',1,16,'',8,'Частота появления "микродуг"','','Частота виникнення "мікродуг"','','');
INSERT INTO tmplib_DevLib_io VALUES('IVE_452HS_02','DAC','Electric arcs counter',1,16,'',9,'Счетчик возникновения дуги','','Лічильник виникнення дуги','','');
INSERT INTO tmplib_DevLib_io VALUES('IVE_452HS_02','COM_DEW','Command: Blocking by SC and XX',3,32,'',10,'Команда: Блокировка отключения выхода по "КЗ" и "ХХ"','','Команда: Блокування відключеня виходу по "КЗ" та "ХХ"','','');
INSERT INTO tmplib_DevLib_io VALUES('IVE_452HS_02','COM_OUT','Command: Display on UI out block''s params',3,32,'',11,'Команда: Отображение на «УИ» выходных/опорных параметров каналов блока','','Команда: Відображення на «УІ» вихідних/опорних параметрів каналів блоку','','');
INSERT INTO tmplib_DevLib_io VALUES('IVE_452HS_02','COM_DEP','Command: Enable MK',3,32,'',12,'Команда: Включение "МК"','','Команда: Включення "МК"','','');
INSERT INTO tmplib_DevLib_io VALUES('IVE_452HS_02','COM_DEL','Command: Blocks #1,2 to line',3,32,'',13,'Команда: Включение блока "по сети" - канал№1,2','','Команда: Включення блоку "по мережі" - канал№1,2','','');
INSERT INTO tmplib_DevLib_io VALUES('IVE_452HS_02','COM_UF','Command: Display on UI frequency and current',3,32,'',14,'Команда: Отображение на «УИ» напряжения и тока/частоты и мощности блока','','Команда: Відображення на «УІ» напруги та струму/частоти та потужності блоку','','');
INSERT INTO tmplib_DevLib_io VALUES('IVE_452HS_02','DKW','MUBR of the block overheated',3,16,'',15,'Перегрев MUBR блока','','Перегрів MUBR блоку','','');
INSERT INTO tmplib_DevLib_io VALUES('IVE_452HS_02','DKZ','Short circuits on the block out',3,16,'',16,'Короткое замыкание выходных цепей блока','','Коротке замикання вихідних ланцюгів блоку','','');
INSERT INTO tmplib_DevLib_io VALUES('IVE_452HS_02','DK','MK of the block overheated',3,16,'',17,'Перегрев MK блока','','Перегрів MK блоку','','');
INSERT INTO tmplib_DevLib_io VALUES('IVE_452HS_02','DE','Power and voltage on out present',3,16,'',18,'Мощность и напряжение есть на выходе','','Потужність та напруга є на виході','','');
INSERT INTO tmplib_DevLib_io VALUES('OPTRIS','transport','Transport',0,64,'Serial.out_OPTRIS',0,'Транспорт','','Транспорт','','');
INSERT INTO tmplib_DevLib_io VALUES('OPTRIS','addr','Device address',3,64,'1',1,'Адрес устройства','','Адреса пристрою','','');
INSERT INTO tmplib_DevLib_io VALUES('OPTRIS','Tproc','T process',2,16,'',2,'Т обработки','','Т обробки','','');
INSERT INTO tmplib_DevLib_io VALUES('OPTRIS','Thead','T head',2,16,'',3,'Т головы','','Т голови','','');
INSERT INTO tmplib_DevLib_io VALUES('OPTRIS','Tbox','T box',2,16,'',4,'Т корпуса','','Т корпусу','','');
INSERT INTO tmplib_DevLib_io VALUES('OPTRIS','Tact','T act.',2,16,'',5,'Т акт.','','Т акт.','','');
INSERT INTO tmplib_DevLib_io VALUES('OPTRIS','eps','IR epsilon',2,32,'',6,'IR эпсилон','','IR епсилон','','');
INSERT INTO tmplib_DevLib_io VALUES('OPTRIS','trans','IR transmission',2,32,'',7,'IR передачи','','IR передавання','','');
INSERT INTO tmplib_DevLib_io VALUES('OPTRIS','spIll','Spot illumination',3,32,'',8,'Точечное освещение','','Точкове освітлення','','');
INSERT INTO tmplib_DevLib_io VALUES('CTR','transport','Transport',0,64,'Serial.out_CTR',0,'Транспорт','','Транспорт','','');
INSERT INTO tmplib_DevLib_io VALUES('CTR','press','Pressure, Tor',2,17,'',1,'Давление, Tor','','Тиск, Tor','','');
INSERT INTO tmplib_DevLib_io VALUES('CTR','zeroSet','Zero set',3,32,'',2,'Установка нуля','','Встановлення нуля','','');
INSERT INTO tmplib_DevLib_io VALUES('IEC60870','transport','Transport',0,64,'Sockets.out_IEC60870',0,'Транспорт','','Транспорт','','');
INSERT INTO tmplib_DevLib_io VALUES('IEC60870','addr','Address, {addr}.{OA}',0,64,'0.5',1,'Адрес, {addr}.{OA}','','Адреса, {addr}.{OA}','','');
INSERT INTO tmplib_DevLib_io VALUES('IEC60870','tmRetr','Retry connection time, seconds',2,64,'10',2,'Время восстановления подключения, секунд','','Час відновлення підключення, секунд','','');
INSERT INTO tmplib_DevLib_io VALUES('IEC60870','t1','Acknowledge lack timeout, seconds',2,64,'1.5',4,'Таймаут подтверждения отсутствия активности, секунд','','Таймаут підтвердження відсутності активності, секунд','','');
INSERT INTO tmplib_DevLib_io VALUES('IEC60870','t2','Acknowledge lack activity timeout, seconds',2,64,'1',5,'Таймаут подтверждения, секунд','','Таймаут підтвердження, секунд','','');
INSERT INTO tmplib_DevLib_io VALUES('IEC60870','t3','Test timeout, seconds',2,64,'2',6,'Таймаут проверки, секунд','','Таймаут перевірки, секунд','','');
INSERT INTO tmplib_DevLib_io VALUES('IEC60870','k','Maximum unconfirmed',1,64,'12',7,'Максимально неподтверждённых','','Максимально непідтверджених','','');
INSERT INTO tmplib_DevLib_io VALUES('IEC60870','w','Maximum no ack',1,64,'8',8,'Максимальное отсутсвие "ack"','','Максимальна відсутність "ack"','','');
INSERT INTO tmplib_DevLib_io VALUES('IEC60870','itemsSet','Items set by: "ai|di|do:{IOA}[-{EndIOA}][:a[:{NameBase}]]"',0,36,'',9,'Набор элементов по: "ai|di|do:{IOA}[-{EndIOA}][:a[:{NameBase}]]"','','Набір елементів за: "ai|di|do:{IOA}[-{EndIOA}][:a[:{NameBase}]]"','','');
INSERT INTO tmplib_DevLib_io VALUES('IEC60870','items','All items',4,33,'',10,'Все элементы','','Всі елементи','','');
INSERT INTO tmplib_DevLib_io VALUES('IEC60870','this','Object',4,0,'',13,'Объект','','Об''єкт','','');
INSERT INTO tmplib_DevLib_io VALUES('SSCP','transport','Transport of the Ethernet network, Sockets',0,64,'SSCP',0,'Транспорт сети Ethernet, Сокеты','','Транспорт мережі Ethernet, Сокети','','');
INSERT INTO tmplib_DevLib_io VALUES('SSCP','addr','Address [0...255]',1,64,'1',1,'Адрес [0...255]','','Адреса [0...255]','','');
INSERT INTO tmplib_DevLib_io VALUES('SSCP','user','User',0,64,'admin',2,'Пользователь','','Користувач','','');
INSERT INTO tmplib_DevLib_io VALUES('SSCP','pass','Password',0,64,'rw',3,'Пароль','','Пароль','','');
INSERT INTO tmplib_DevLib_io VALUES('SSCP','list','Selected to process variables',0,36,'',8,'Переменные, выбранные для обработки','','Змінні, обрані для обробки','','');
INSERT INTO tmplib_DevLib_io VALUES('SSCP','listsDir','List files'' folder',0,64,'SSCP',5,'Директория файлов-списков','','Тека файлів-переліків','','');
INSERT INTO tmplib_DevLib_io VALUES('SSCP','this','Object',4,0,'',9,'Объект','','Об''єкт','','');
INSERT INTO tmplib_DevLib_io VALUES('SSCP','maxDtFrm','Maximum data frame size',1,64,'2048',4,'Максимальный размер фрейма данных','','Максимальний розмір фрейму даних','','');
INSERT INTO tmplib_DevLib_io VALUES('SSCP','verPrt','Protocol version',1,16,'',6,'Версия протокола','','Версія протоколу','','');
INSERT INTO tmplib_DevLib_io VALUES('SSCP','maxDtFrmServ','Server''s maximum data frame size',1,16,'',7,'Максимальный размер фрейма данных сервера','','Максимальний розмір фрейму даних серверу','','');
INSERT INTO tmplib_DevLib_io VALUES('IT3','mdPass','Passive mode, writing by an input protocol''s part',3,64,'0',2,'Пассивный режим, запись входной частью протокола','','Пасивний режим, запис вхідною частиною протоколу','','');
INSERT INTO tmplib_DevLib_io VALUES('IT3','this','Object',4,0,'',7,'Объект','','Об''єкт','','');
INSERT INTO tmplib_DevLib_io VALUES('UPS','bChL','Battery charge low',2,32,'20',3,'Низкий заряд батареи','','Низький заряд батареї','','');
INSERT INTO tmplib_DevLib_io VALUES('UPS','bChLL','Battery charge critical',2,32,'5',4,'Критический заряд батареи','','Критичний заряд батареї','','');
INSERT INTO tmplib_DevLib_io VALUES('UPS','inVL','Input voltage low',2,32,'210',5,'Низкое входное напряжение','','Низька вхідна напруга','','');
INSERT INTO tmplib_DevLib_io VALUES('UPS','inVH','Input voltage high',2,32,'250',6,'Высокое входное напряжение','','Висока вхідна напруга','','');
INSERT INTO tmplib_DevLib_io VALUES('UPS','inFLL','Input frequency too low',2,32,'40',7,'Очень низкая входная частота','','Дуже низька вхідна частота','','');
INSERT INTO tmplib_DevLib_io VALUES('UPS','inFL','Input frequency low',2,32,'45',8,'Низкая входная частота','','Низька вхідна частота','','');
INSERT INTO tmplib_DevLib_io VALUES('UPS','inFH','Input frequency high',2,32,'55',9,'Высокая входная частота','','Висока вхідна частота','','');
INSERT INTO tmplib_DevLib_io VALUES('UPS','inFHH','Input frequency too high',2,32,'60',10,'Очень высокая входная частота','','Дуже висока вхідна частота','','');
INSERT INTO tmplib_DevLib_io VALUES('UPS','loadH','Load high',2,32,'80',11,'Нагрузка высокая','','Навантаження високе','','');
INSERT INTO tmplib_DevLib_io VALUES('UPS','loadHH','Load too high',2,32,'100',12,'Нагрузка очень высокая','','Навантаження дуже високе','','');
INSERT INTO tmplib_DevLib_io VALUES('UPS','tH','Temperature high',2,32,'50',13,'Высокая температура','','Висока температура','','');
INSERT INTO tmplib_DevLib_io VALUES('UPS','tHH','Temperature too high',2,32,'70',14,'Температура очень высокая','','Температура дуже висока','','');
INSERT INTO tmplib_DevLib_io VALUES('UPS','alDelay','Violations delay, seconds',2,32,'0',2,'Задержка нарушений, секунды','','Затримка порушень, секунди','','');
INSERT INTO tmplib_DevLib_io VALUES('m200','s','Energy from reset, tariffs sum',0,16,'',0,'Энергия от сброса сумма Тарифов','','Енергія від скидання сума Тарифів','','');
INSERT INTO tmplib_DevLib_io VALUES('m200','t1','Energy from reset, tariff 1',2,16,'',1,'Энергия от сброса Тариф1','','Енергія від скидання Тариф1','','');
INSERT INTO tmplib_DevLib_io VALUES('m200','t2','Energy from reset, tariff 2',2,16,'',2,'Энергия от сброса Тариф2','','Енергія від скидання Тариф2','','');
INSERT INTO tmplib_DevLib_io VALUES('m200','t3','Energy from reset, tariff 3',2,16,'',3,'Энергия от сброса Тариф3','','Енергія від скидання Тариф3','','');
INSERT INTO tmplib_DevLib_io VALUES('m200','t4','Energy from reset, tariff 4',2,16,'',4,'Энергия от сброса Тариф4','','Енергія від скидання Тариф4','','');
INSERT INTO tmplib_DevLib_io VALUES('m200','U','Voltage U, V',2,16,'',5,'Напряжение U, В','','Напруга U, В','','');
INSERT INTO tmplib_DevLib_io VALUES('m200','I','Current I, A',2,16,'',6,'Ток I, А','','Струм I, А','','');
INSERT INTO tmplib_DevLib_io VALUES('m200','P','Power P, W',2,16,'',7,'Мощность P, Вт','','Потужність P, Вт','','');
INSERT INTO tmplib_DevLib_io VALUES('m200','transport','Transport',0,64,'merc200',8,'Транспорт','','Транспорт','','');
INSERT INTO tmplib_DevLib_io VALUES('m200','netaddr','Network address',2,64,'1',9,'Сетевой адрес','','Мережева адреса','','');
INSERT INTO tmplib_DevLib_io VALUES('m200','naladchik','Coded address by the program Fixer+',3,64,'0',10,'Адрес закодирован программой Наладчик+','','Адреса закодована програмою Налаштувач+','','');
INSERT INTO tmplib_DevLib_io VALUES('m200','tarif','Read energy from reset',3,64,'1',11,'Считывать энергию от сброса','','Вичитувати енергію від скидання','','');
INSERT INTO tmplib_DevLib_io VALUES('m200','UIP','Read current values',3,64,'1',12,'Считывать мгновенные значения','','Вичитувати миттеві значення','','');
INSERT INTO tmplib_DevLib_io VALUES('m200','first','First byte of the address',1,0,'',13,'Первый байт адреса','','Перший байт адреси','','');
INSERT INTO tmplib_DevLib_io VALUES('m200','second','Last 3 bytes of the address',1,0,'',14,'Последние 3 байта адреса','','Останні 3 байти адреси','','');
INSERT INTO tmplib_DevLib_io VALUES('m230','answer','Answer to the password request',0,16,'',0,'Ответ на запрос пароля','','Відповідь на запит пароля','','');
INSERT INTO tmplib_DevLib_io VALUES('m230','TS','Read energy of tariffs sum',3,64,'0',1,'Считывать энергию суммы тарифов','','Вичитувати енергію суми тарифів','','');
INSERT INTO tmplib_DevLib_io VALUES('m230','T1','Read energy of tariff 1',3,64,'0',2,'Считывать энергию тарифа 1','','Вичитувати енергію тарифу 1','','');
INSERT INTO tmplib_DevLib_io VALUES('m230','T2','Read energy of tariff 2',3,64,'0',3,'Считывать энергию тарифа 2','','Вичитувати енергію тарифу 2','','');
INSERT INTO tmplib_DevLib_io VALUES('m230','T3','Read energy of tariff 3',3,64,'0',4,'Считывать энергию тарифа 3','','Вичитувати енергію тарифу 3','','');
INSERT INTO tmplib_DevLib_io VALUES('m230','T4','Read energy of tariff 4',3,64,'0',5,'Считывать энергию тарифа 4','','Вичитувати енергію тарифу 4','','');
INSERT INTO tmplib_DevLib_io VALUES('m230','P','Read power P',3,64,'0',6,'Считывать мощность P','','Вичитувати потужність P','','');
INSERT INTO tmplib_DevLib_io VALUES('m230','Q','Read power Q',3,64,'0',7,'Считывать мощность Q','','Вичитувати потужність Q','','');
INSERT INTO tmplib_DevLib_io VALUES('m230','S','Read power S',3,64,'0',8,'Считывать мощность S','','Вичитувати потужність S','','');
INSERT INTO tmplib_DevLib_io VALUES('m230','U','Read voltage U',3,64,'0',9,'Считывать напряжение U','','Вичитувати напругу U','','');
INSERT INTO tmplib_DevLib_io VALUES('m230','I','Read current I',3,64,'0',10,'Считывать ток I','','Вичитувати струм I','','');
INSERT INTO tmplib_DevLib_io VALUES('m230','K','Read power coefficient',3,64,'0',11,'Считывать коэф. мощности','','Вичитувати коеф. потужності','','');
INSERT INTO tmplib_DevLib_io VALUES('m230','F','Read frequency F',3,64,'0',12,'Считывать частоту F','','Вичитувати частоту F','','');
INSERT INTO tmplib_DevLib_io VALUES('m230','N','Read serial number',3,64,'0',13,'Считывать серийный номер','','Вичитувати серійний номер','','');
INSERT INTO tmplib_DevLib_io VALUES('m230','TSAp','Energy from resetting, tariffs sum A+',2,16,'',14,'Энергия от сброса сумма тарифов A+','','Енергія від скидання сума тарифів A+','','');
INSERT INTO tmplib_DevLib_io VALUES('m230','TSAm','Energy from resetting, tariffs sum A-',2,16,'',15,'Энергия от сброса сумма тарифов A-','','Енергія від скидання сума тарифів A-','','');
INSERT INTO tmplib_DevLib_io VALUES('m230','TSRp','Energy from resetting, tariffs sum R+',2,16,'',16,'Энергия от сброса сумма тарифов R+','','Енергія від скидання сума тарифів R+','','');
INSERT INTO tmplib_DevLib_io VALUES('m230','TSRm','Energy from resetting, tariffs sum R-',2,16,'',17,'Энергия от сброса сумма тарифов R-','','Енергія від скидання сума тарифів R-','','');
INSERT INTO tmplib_DevLib_io VALUES('m230','T1Ap','Energy from resetting, tariff 1 A+',2,16,'',18,'Энергия от сброса тариф 1 A+','','Енергія від скидання тариф 1 A+','','');
INSERT INTO tmplib_DevLib_io VALUES('m230','T1Am','Energy from resetting, tariff 1 A-',2,16,'',19,'Энергия от сброса тариф 1 A-','','Енергія від скидання тариф 1 A-','','');
INSERT INTO tmplib_DevLib_io VALUES('m230','T1Rp','Energy from resetting, tariff 1 R+',2,16,'',20,'Энергия от сброса тариф 1 R+','','Енергія від скидання тариф 1 R+','','');
INSERT INTO tmplib_DevLib_io VALUES('m230','T1Rm','Energy from resetting, tariff 1 R-',2,16,'',21,'Энергия от сброса тариф 1 R-','','Енергія від скидання тариф 1 R-','','');
INSERT INTO tmplib_DevLib_io VALUES('m230','T2Ap','Energy from resetting, tariff 2 A+',2,16,'',22,'Энергия от сброса тариф 2 A+','','Енергія від скидання тариф 2 A+','','');
INSERT INTO tmplib_DevLib_io VALUES('m230','T2Am','Energy from resetting, tariff 2 A-',2,16,'',23,'Энергия от сброса тариф 2 A-','','Енергія від скидання тариф 2 A-','','');
INSERT INTO tmplib_DevLib_io VALUES('m230','T2Rp','Energy from resetting, tariff 2 R+',2,16,'',24,'Энергия от сброса тариф 2 R+','','Енергія від скидання тариф 2 R+','','');
INSERT INTO tmplib_DevLib_io VALUES('m230','T2Rm','Energy from resetting, tariff 2 R-',2,16,'',25,'Энергия от сброса тариф 2 R-','','Енергія від скидання тариф 2 R-','','');
INSERT INTO tmplib_DevLib_io VALUES('m230','T3Ap','Energy from resetting, tariff 3 A+',2,16,'',26,'Энергия от сброса тариф 3 A+','','Енергія від скидання тариф 3 A+','','');
INSERT INTO tmplib_DevLib_io VALUES('m230','T3Am','Energy from resetting, tariff 3 A-',2,16,'',27,'Энергия от сброса тариф 3 A-','','Енергія від скидання тариф 3 A-','','');
INSERT INTO tmplib_DevLib_io VALUES('m230','T3Rp','Energy from resetting, tariff 3 R+',2,16,'',28,'Энергия от сброса тариф 3 R+','','Енергія від скидання тариф 3 R+','','');
INSERT INTO tmplib_DevLib_io VALUES('m230','T3Rm','Energy from resetting, tariff 3 R-',2,16,'',29,'Энергия от сброса тариф 3 R-','','Енергія від скидання тариф 3 R-','','');
INSERT INTO tmplib_DevLib_io VALUES('m230','T4Ap','Energy from resetting, tariff 4 A+',2,16,'',30,'Энергия от сброса тариф 4 A+','','Енергія від скидання тариф 4 A+','','');
INSERT INTO tmplib_DevLib_io VALUES('m230','T4Am','Energy from resetting, tariff 4 A-',2,16,'',31,'Энергия от сброса тариф 4 A-','','Енергія від скидання тариф 4 A-','','');
INSERT INTO tmplib_DevLib_io VALUES('m230','T4Rp','Energy from resetting, tariff 4 R+',2,16,'',32,'Энергия от сброса тариф 4 R+','','Енергія від скидання тариф 4 R+','','');
INSERT INTO tmplib_DevLib_io VALUES('m230','T4Rm','Energy from resetting, tariff 4 R-',2,16,'',33,'Энергия от сброса тариф 4 R-','','Енергія від скидання тариф 4 R-','','');
INSERT INTO tmplib_DevLib_io VALUES('m230','PS','Power P(W) by the phases sum',2,16,'',34,'Мощность P(Вт) по сумме фаз','','Потужніст P(Вт) по сумі фаз','','');
INSERT INTO tmplib_DevLib_io VALUES('m230','P1','Power P(W) by the phase 1',2,16,'',35,'Мощность P(Вт) по фазе 1','','Потужніст P(Вт) по фазі 1','','');
INSERT INTO tmplib_DevLib_io VALUES('m230','P2','Power P(W) by the phase 2',2,16,'',36,'Мощность P(Вт) по фазе 2','','Потужніст P(Вт) по фазі 2','','');
INSERT INTO tmplib_DevLib_io VALUES('m230','P3','Power P(W) by the phase 3',2,16,'',37,'Мощность P(Вт) по фазе 3','','Потужніст P(Вт) по фазі 3','','');
INSERT INTO tmplib_DevLib_io VALUES('m230','QS','Power Q(var) by the phases sum',2,16,'',38,'Мощность Q(вар) по сумме фаз','','Потужніст Q(вар) по сумі фаз','','');
INSERT INTO tmplib_DevLib_io VALUES('m230','Q1','Power Q(var) by the phase 1',2,16,'',39,'Мощность Q(вар) по фазе 1','','Потужніст Q(вар) по фазі 1','','');
INSERT INTO tmplib_DevLib_io VALUES('m230','Q2','Power Q(var) by the phase 2',2,16,'',40,'Мощность Q(вар) по фазе 2','','Потужніст Q(вар) по фазі 2','','');
INSERT INTO tmplib_DevLib_io VALUES('m230','Q3','Power Q(var) by the phase 3',2,16,'',41,'Мощность Q(вар) по фазе 3','','Потужніст Q(вар) по фазі 3','','');
INSERT INTO tmplib_DevLib_io VALUES('m230','SS','Power S(VA) by the phases sum',2,16,'',42,'Мощность S(ВА) по сумме фаз','','Потужніст S(ВА) по сумі фаз','','');
INSERT INTO tmplib_DevLib_io VALUES('m230','S1','Power S(VA) by the phase 1',2,16,'',43,'Мощность S(ВА) по фазе 1','','Потужніст S(ВА) по фазі 1','','');
INSERT INTO tmplib_DevLib_io VALUES('m230','S2','Power S(VA) by the phase 2',2,16,'',44,'Мощность S(ВА) по фазе 2','','Потужніст S(ВА) по фазі 2','','');
INSERT INTO tmplib_DevLib_io VALUES('m230','S3','Power S(VA) by the phase 3',2,16,'',45,'Мощность S(ВА) по фазе 3','','Потужніст S(ВА) по фазі 3','','');
INSERT INTO tmplib_DevLib_io VALUES('m230','U1','Voltage U(V) by the phase 1',2,16,'',46,'Напряжение U(В) по фазе 1','','Напруга U(В) по фазі 1','','');
INSERT INTO tmplib_DevLib_io VALUES('m230','U2','Voltage U(V) by the phase 2',2,16,'',47,'Напряжение U(В) по фазе 2','','Напруга U(В) по фазі 2','','');
INSERT INTO tmplib_DevLib_io VALUES('m230','U3','Voltage U(V) by the phase 3',2,16,'',48,'Напряжение U(В) по фазе 3','','Напруга U(В) по фазі 3','','');
INSERT INTO tmplib_DevLib_io VALUES('m230','I1','Current I(A) by the phase 1',2,16,'',49,'Ток I(А) по фазе 1','','Струм I(А) по фазі 1','','');
INSERT INTO tmplib_DevLib_io VALUES('m230','I2','Current I(A) by the phase 2',2,16,'',50,'Ток I(А) по фазе 2','','Струм I(А) по фазі 2','','');
INSERT INTO tmplib_DevLib_io VALUES('m230','I3','Current I(A) by the phase 3',2,16,'',51,'Ток I(А) по фазе 3','','Струм I(А) по фазі 3','','');
INSERT INTO tmplib_DevLib_io VALUES('m230','KS','Power coefficient by the phases sum',2,16,'',52,'Коэф.мощности по сумме фаз','','Коеф.потужності по сумі фаз','','');
INSERT INTO tmplib_DevLib_io VALUES('m230','K1','Power coefficient by the phase 1',2,16,'',53,'Коэф.мощности по фазе 1','','Коеф.потужності по фазі 1','','');
INSERT INTO tmplib_DevLib_io VALUES('m230','K2','Power coefficient by the phase 2',2,16,'',54,'Коэф.мощности по фазе 2','','Коеф.потужності по фазі 2','','');
INSERT INTO tmplib_DevLib_io VALUES('m230','K3','Power coefficient by the phase 3',2,16,'',55,'Коэф.мощности по фазе 3','','Коеф.потужності по фазі 3','','');
INSERT INTO tmplib_DevLib_io VALUES('m230','F1','Frequency F(Hz)',2,16,'',56,'Частота F(Гц)','','Частота F(Гц)','','');
INSERT INTO tmplib_DevLib_io VALUES('m230','N1','Serial number',0,16,'',57,'Серийный номер','','Серійний номер','','');
INSERT INTO tmplib_DevLib_io VALUES('m230','netaddr','Network address (0...240)',1,64,'1',58,'Сетевой адрес (0...240)','','Мережева адреса (0...240)','','');
INSERT INTO tmplib_DevLib_io VALUES('m230','password','Password',0,64,'111111',59,'Пароль','','Пароль','','');
INSERT INTO tmplib_DevLib_io VALUES('m230','transport','Transport',0,64,'merc230',60,'Транспорт','','Транспорт','','');
INSERT INTO tmplib_DevLib_io VALUES('Nik2303I','transport','Transport',0,64,'Sockets.out_',0,'Транспорт','','Транспорт','','');
INSERT INTO tmplib_DevLib_io VALUES('Nik2303I','serial','Factory number of the counter',0,64,'',1,'Заводской номер счётчика','','Заводський номер лічильника','','');
INSERT INTO tmplib_DevLib_io VALUES('Nik2303I','passw','Password',0,64,'1111111111111111',2,'Пароль','','Пароль','','');
INSERT INTO tmplib_DevLib_io VALUES('Nik2303I','kT','Coefficient of transformation',1,64,'1',3,'Коэффициент трансформации','','Коефіцієнт трансформації','','');
INSERT INTO tmplib_DevLib_io VALUES('Nik2303I','T0','Current values',3,64,'0',4,'Текущие показатели','','Поточні показники','','');
INSERT INTO tmplib_DevLib_io VALUES('Nik2303I','T1','Current values tariff T1',3,64,'0',5,'Текущие показатели тариф T1','','Поточні показники тариф T1','','');
INSERT INTO tmplib_DevLib_io VALUES('Nik2303I','T2','Current values tariff T2',3,64,'0',6,'Текущие показатели тариф T2','','Поточні показники тариф T2','','');
INSERT INTO tmplib_DevLib_io VALUES('Nik2303I','T3','Current values tariff T3',3,64,'0',7,'Текущие показатели тариф T3','','Поточні показники тариф T3','','');
INSERT INTO tmplib_DevLib_io VALUES('Nik2303I','U','Instantaneous value "Voltage"',3,64,'0',8,'Мгновенные значения "Напряжение"','','Миттєві значення "Напруга"','','');
INSERT INTO tmplib_DevLib_io VALUES('Nik2303I','I','Instantaneous value "Current"',3,64,'0',9,'Мгновенные значения "Ток"','','Миттєві значення "Струм"','','');
INSERT INTO tmplib_DevLib_io VALUES('Nik2303I','P','Instantaneous value "Power"',3,64,'0',10,'Мгновенные значения "Мощность"','','Миттєві значення "Потужність"','','');
INSERT INTO tmplib_DevLib_io VALUES('Nik2303I','kP','Instantaneous value "Power coefficient"',3,64,'0',11,'Мгновенные значения "коэффициент Мощности"','','Миттєві значення "коефіцієнт Потужності"','','');
INSERT INTO tmplib_DevLib_io VALUES('Nik2303I','Q','Instantaneous value "Power reactive"',3,64,'0',12,'Мгновенные значения "реактивная мощность"','','Миттєві значення "Реактивна потужність"','','');
INSERT INTO tmplib_DevLib_io VALUES('Nik2303I','K','Instantaneous value "Vectors angle"',3,64,'0',13,'Мгновенные значения "Угол между векторами"','','Миттєві значення "Кут між векторами"','','');
INSERT INTO tmplib_DevLib_io VALUES('Nik2303I','Ae','А+(kW*h)',2,17,'',14,'А+(кВт*ч)','','А+(кВт*ч)','','');
INSERT INTO tmplib_DevLib_io VALUES('Nik2303I','Ae_','А-(kW*h)',2,17,'',15,'А-(кВт*ч)','','А-(кВт*ч)','','');
INSERT INTO tmplib_DevLib_io VALUES('Nik2303I','Re','R+(1+2quadrant)(kVar*h)',2,17,'',16,'R+(1+2квадрант)(кВар*ч)','','R+(1+2квадрант)(кВар*ч)','','');
INSERT INTO tmplib_DevLib_io VALUES('Nik2303I','Re_','R-(3+4quadrant)(kVar*h)',2,17,'',17,'R-(3+4квадрант)(кВар*ч)','','R-(3+4квадрант)(кВар*ч)','','');
INSERT INTO tmplib_DevLib_io VALUES('Nik2303I','Ae1','А1+(kW*h)',2,17,'',18,'А1+(кВт*ч)','','А1+(кВт*ч)','','');
INSERT INTO tmplib_DevLib_io VALUES('Nik2303I','Ae_1','А1-(kW*h)',2,17,'',19,'А1-(кВт*ч)','','А1-(кВт*ч)','','');
INSERT INTO tmplib_DevLib_io VALUES('Nik2303I','Re1','R1+(1+2quadrant)(kVar*h)',2,17,'',20,'R1+(1+2квадрант)(кВар*ч)','','R1+(1+2квадрант)(кВар*ч)','','');
INSERT INTO tmplib_DevLib_io VALUES('Nik2303I','Re_1','R1-(3+4quadrant)(kVar*h)',2,17,'',21,'R1-(3+4квадрант)(кВар*ч)','','R1-(3+4квадрант)(кВар*ч)','','');
INSERT INTO tmplib_DevLib_io VALUES('Nik2303I','Ae2','А2+(kW*h)',2,17,'',22,'А2+(кВт*ч)','','А2+(кВт*ч)','','');
INSERT INTO tmplib_DevLib_io VALUES('Nik2303I','Ae_2','А2-(kW*h)',2,17,'',23,'А2-(кВт*ч)','','А2-(кВт*ч)','','');
INSERT INTO tmplib_DevLib_io VALUES('Nik2303I','Re2','R2+(1+2quadrant)(kVar*h)',2,17,'',24,'R2+(1+2квадрант)(кВар*ч)','','R2+(1+2квадрант)(кВар*ч)','','');
INSERT INTO tmplib_DevLib_io VALUES('Nik2303I','Re_2','R2-(3+4quadrant)(kVar*h)',2,17,'',25,'R2-(3+4квадрант)(кВар*ч)','','R2-(3+4квадрант)(кВар*ч)','','');
INSERT INTO tmplib_DevLib_io VALUES('Nik2303I','Ae3','А3+(kW*h)',2,17,'',26,'А3+(кВт*ч)','','А3+(кВт*ч)','','');
INSERT INTO tmplib_DevLib_io VALUES('Nik2303I','Ae_3','А3-(kW*h)',2,17,'',27,'А3-(кВт*ч)','','А3-(кВт*ч)','','');
INSERT INTO tmplib_DevLib_io VALUES('Nik2303I','Re3','R3+(1+2quadrant)(kVar*h)',2,17,'',28,'R3+(1+2квадрант)(кВар*ч)','','R3+(1+2квадрант)(кВар*ч)','','');
INSERT INTO tmplib_DevLib_io VALUES('Nik2303I','Re_3','R3-(3+4quadrant)(kVar*h)',2,17,'',29,'R3-(3+4квадрант)(кВар*ч)','','R3-(3+4квадрант)(кВар*ч)','','');
INSERT INTO tmplib_DevLib_io VALUES('Nik2303I','U1','Phase voltage 1(V)',2,17,'',30,'Напряжение фаза 1 (В)','','Напруга фаза 1 (В)','','');
INSERT INTO tmplib_DevLib_io VALUES('Nik2303I','U2','Phase voltage 2(V)',2,17,'',31,'Напряжение фаза 2 (В)','','Напруга фаза 2 (В)','','');
INSERT INTO tmplib_DevLib_io VALUES('Nik2303I','U3','Phase voltage 3(V)',2,17,'',32,'Напряжение фаза 3 (В)','','Напруга фаза 3 (В)','','');
INSERT INTO tmplib_DevLib_io VALUES('Nik2303I','I1','Phase current 1(A)',2,17,'',33,'Ток фаза 1 (А)','','Струм фаза 1 (А)','','');
INSERT INTO tmplib_DevLib_io VALUES('Nik2303I','I2','Phase current 2(A)',2,17,'',34,'Ток фаза 2 (А)','','Струм фаза 2 (А)','','');
INSERT INTO tmplib_DevLib_io VALUES('Nik2303I','I3','Phase current 3(A)',2,17,'',35,'Ток фаза 3 (А)','','Струм фаза 3 (А)','','');
INSERT INTO tmplib_DevLib_io VALUES('Nik2303I','kP1','Power coeff. cos φ phase 1',2,17,'',36,'Коеф. мощности. cos φ фаза 1','','Коеф. потужн. cos φ фаза 1','','');
INSERT INTO tmplib_DevLib_io VALUES('Nik2303I','kP2','Power coeff. cos φ phase 2',2,17,'',37,'Коеф. мощности. cos φ фаза 2','','Коеф. потужн. cos φ фаза 2','','');
INSERT INTO tmplib_DevLib_io VALUES('Nik2303I','kP3','Power coeff. cos φ phase 3',2,17,'',38,'Коеф. мощности. cos φ фаза 3','','Коеф. потужн. cos φ фаза 3','','');
INSERT INTO tmplib_DevLib_io VALUES('Nik2303I','Psum','Summary active power (kW)',2,17,'',39,'Мощность  акт. суммарная (кВт)','','Потужн.  акт. сумарна (кВт)','','');
INSERT INTO tmplib_DevLib_io VALUES('Nik2303I','P1','Active power phase 1 (kW)',2,17,'',40,'Мощность  акт. фаза 1 (кВт)','','Потужн.  акт. фаза 1 (кВт)','','');
INSERT INTO tmplib_DevLib_io VALUES('Nik2303I','P2','Active power phase 2 (kW)',2,17,'',41,'Мощность  акт. фаза 2 (кВт)','','Потужн.  акт. фаза 2 (кВт)','','');
INSERT INTO tmplib_DevLib_io VALUES('Nik2303I','P3','Active power phase 3 (kW)',2,17,'',42,'Мощность  акт. фаза 3 (кВт)','','Потужн.  акт. фаза 3 (кВт)','','');
INSERT INTO tmplib_DevLib_io VALUES('Nik2303I','Qsum','Summary reactive power (kVar)',2,17,'',43,'Мощность реакт. сумарная (кВар)','','Потужн. реакт. сумарна (кВар)','','');
INSERT INTO tmplib_DevLib_io VALUES('Nik2303I','Q1','Reactive power phase 1 (kVar)',2,17,'',44,'Мощность реакт. фаза 1 (кВар)','','Потужн. реакт. фаза 1 (кВар)','','');
INSERT INTO tmplib_DevLib_io VALUES('Nik2303I','Q2','Reactive power phase 2 (kVar)',2,17,'',45,'Мощность реакт. фаза 2 (кВар)','','Потужн. реакт. фаза 2 (кВар)','','');
INSERT INTO tmplib_DevLib_io VALUES('Nik2303I','Q3','Reactive power phase 3 (kVar)',2,17,'',46,'Мощность реакт. фаза 3 (кВар)','','Потужн. реакт. фаза 3 (кВар)','','');
INSERT INTO tmplib_DevLib_io VALUES('Nik2303I','V12','Vector angle U1_U2(degr.)',1,17,'',47,'Угол вектора U1_U2 (град.)','','Кут вектора U1_U2 (град.)','','');
INSERT INTO tmplib_DevLib_io VALUES('Nik2303I','V13','Vector angle U1_U3(degr.)',1,17,'',48,'Угол вектора U1_U3 (град.)','','Кут вектора U1_U3 (град.)','','');
INSERT INTO tmplib_DevLib_io VALUES('Nik2303I','this','Object',4,0,'',49,'Объект','','Об''єкт','','');
INSERT INTO tmplib_DevLib_io VALUES('Nik2303I','NAME','Name',0,16,'',50,'Имя','','Ім''я','','Име');
INSERT INTO tmplib_DevLib_io VALUES('Nik2303I','SHIFR','Shifr',0,16,'',51,'Шифр','','Шифр','','');
INSERT INTO tmplib_DevLib_io VALUES('Nik2303I','DESCR','Description',0,16,'',52,'Описание','','Опис','','Опис');
INSERT INTO tmplib_DevLib_io VALUES('ModBusScan','lim','Limit time, seconds',1,64,'10',0,'Время ограничения, секунд','','Час обмеження, секунд','','');
INSERT INTO tmplib_DevLib_io VALUES('ModBusScan','type','Type',1,40,'2
0;1;2;3
Coils (1);Input Coils (2);Registers (3);Input Registers (4)',1,'Тип','2
0;1;2;3
Кольца (1);Входные Кольца (2);Регистры (3);Входные Регистры (4)','Тип','2
0;1;2;3
Кільця (1);Вхідні Кільця (2);Регістри (3);Вхідні Регістри (4)','');
INSERT INTO tmplib_DevLib_io VALUES('ModBusScan','begin','Begin (0...65535)',1,33,'',2,'Начало (0...65536)','','Початок (0...65536)','','');
INSERT INTO tmplib_DevLib_io VALUES('ModBusScan','end','End (0...65536)',1,33,'',3,'Конец (0...65536)','','Кінець (0...65536)','','');
INSERT INTO tmplib_DevLib_io VALUES('ModBusScan','res','Result',0,21,'',4,'Результат','','Результат','','');
INSERT INTO tmplib_DevLib_io VALUES('ModBusScan','this','Object',4,0,'',5,'Объект','','Об''єкт','','');
INSERT INTO tmplib_DevLib_io VALUES('pulsarM','transport','Transport',0,64,'Serial.out_pulsar',0,'Транспорт','','Транспорт','','');
INSERT INTO tmplib_DevLib_io VALUES('pulsarM','addr','Address [0...99999999]',1,64,'12345678',1,'Адрес [0...99999999]','','Адреса [0...99999999]','','');
INSERT INTO tmplib_DevLib_io VALUES('pulsarM','this','Object',4,0,'',3,'Объект','','Об''єкт','','');
INSERT INTO tmplib_DevLib_io VALUES('pulsarM','nChnl','Channels number',1,32,'0',2,'Количество каналов','','Кількість каналів','','');
INSERT INTO tmplib_DevLib_io VALUES('OWEN','transport','Transport',0,64,'Serial.out_owen',0,'Транспорт','','Транспорт','','');
INSERT INTO tmplib_DevLib_io VALUES('OWEN','items','Items set "{addr}:{func}:{f|i|u|s}:{r|w}[:{id}[:{nm}]]"',0,36,'',1,'','','','','');
INSERT INTO tmplib_DevLib_io VALUES('OWEN','this','Object',4,0,'',2,'Объект','','Об''єкт','','');
INSERT INTO tmplib_DevLib_io VALUES('DCON_Prt','io','Output: IO',4,0,'',1,'','','','','');
INSERT INTO tmplib_DevLib_io VALUES('DCON_Prt','tr','Transport',4,0,'',0,'Транспорт','','Транспорт','','');
INSERT INTO tmplib_DevLib_io VALUES('DCON_Prt','rez','Input: result',3,1,'0',2,'','','','','');
INSERT INTO tmplib_DevLib_io VALUES('DCON_Prt','request','Input: request',0,0,'',3,'','','','','');
INSERT INTO tmplib_DevLib_io VALUES('DCON_Prt','answer','Input: answer',0,1,'',4,'','','','','');
INSERT INTO tmplib_DevLib_io VALUES('DCON_Prt','sender','Input: sender',0,0,'',5,'','','','','');
INSERT INTO tmplib_DevLib_io VALUES('DCON_Prt','enCRC','Enable CRC',3,64,'0',27,'','','','','');
INSERT INTO tmplib_DevLib_io VALUES('DCON_Prt','ai0','Input: AI0',2,128,'',7,'','','','','');
INSERT INTO tmplib_DevLib_io VALUES('DCON_Prt','ai1','Input: AI1',2,128,'',8,'','','','','');
INSERT INTO tmplib_DevLib_io VALUES('DCON_Prt','ai2','Input: AI2',2,128,'',9,'','','','','');
INSERT INTO tmplib_DevLib_io VALUES('DCON_Prt','ai3','Input: AI3',2,128,'',10,'','','','','');
INSERT INTO tmplib_DevLib_io VALUES('DCON_Prt','ai4','Input: AI4',2,128,'',11,'','','','','');
INSERT INTO tmplib_DevLib_io VALUES('DCON_Prt','ai5','Input: AI5',2,128,'',12,'','','','','');
INSERT INTO tmplib_DevLib_io VALUES('DCON_Prt','ai6','Input: AI6',2,128,'',13,'','','','','');
INSERT INTO tmplib_DevLib_io VALUES('DCON_Prt','ai7','Input: AI7',2,128,'',14,'','','','','');
INSERT INTO tmplib_DevLib_io VALUES('DCON_Prt','ai8','Input: AI8',2,128,'',15,'','','','','');
INSERT INTO tmplib_DevLib_io VALUES('DCON_Prt','ai9','Input: AI9',2,128,'',16,'','','','','');
INSERT INTO tmplib_DevLib_io VALUES('DCON_Prt','addr','Address [0...240]',1,64,'0',28,'','','','','');
INSERT INTO tmplib_DevLib_io VALUES('DCON_Prt','n','Input: number of AI',1,64,'8',6,'','','','','');
INSERT INTO tmplib_DevLib_io VALUES('DCON_Prt','ai10','Input: AI10',2,128,'',17,'','','','','');
INSERT INTO tmplib_DevLib_io VALUES('DCON_Prt','ai11','Input: AI11',2,128,'',18,'','','','','');
INSERT INTO tmplib_DevLib_io VALUES('DCON_Prt','ai12','Input: AI12',2,128,'',19,'','','','','');
INSERT INTO tmplib_DevLib_io VALUES('DCON_Prt','ai13','Input: AI13',2,128,'',20,'','','','','');
INSERT INTO tmplib_DevLib_io VALUES('DCON_Prt','ai14','Input: AI14',2,128,'',21,'','','','','');
INSERT INTO tmplib_DevLib_io VALUES('DCON_Prt','ai15','Input: AI15',2,128,'',22,'','','','','');
INSERT INTO tmplib_DevLib_io VALUES('DCON_Prt','ai16','Input: AI16',2,128,'',23,'','','','','');
INSERT INTO tmplib_DevLib_io VALUES('DCON_Prt','ai17','Input: AI17',2,128,'',24,'','','','','');
INSERT INTO tmplib_DevLib_io VALUES('DCON_Prt','ai18','Input: AI18',2,128,'',25,'','','','','');
INSERT INTO tmplib_DevLib_io VALUES('DCON_Prt','ai19','Input: AI19',2,128,'',26,'','','','','');
INSERT INTO tmplib_DevLib_io VALUES('DCON_Prt_DI','tr','Transport',4,0,'',0,'Транспорт','','Транспорт','','');
INSERT INTO tmplib_DevLib_io VALUES('DCON_Prt_DI','io','Output: IO',4,0,'',1,'','','','','');
INSERT INTO tmplib_DevLib_io VALUES('DCON_Prt_DI','rez','Input: result',3,1,'0',2,'','','','','');
INSERT INTO tmplib_DevLib_io VALUES('DCON_Prt_DI','request','Input: request',0,0,'',3,'','','','','');
INSERT INTO tmplib_DevLib_io VALUES('DCON_Prt_DI','answer','Input: answer',0,1,'',4,'','','','','');
INSERT INTO tmplib_DevLib_io VALUES('DCON_Prt_DI','sender','Input: sender',0,0,'',5,'','','','','');
INSERT INTO tmplib_DevLib_io VALUES('DCON_Prt_DI','n','Input: number of DI',1,64,'8',6,'','','','','');
INSERT INTO tmplib_DevLib_io VALUES('DCON_Prt_DI','enCRC','Enable CRC',3,64,'0',39,'','','','','');
INSERT INTO tmplib_DevLib_io VALUES('DCON_Prt_DI','addr','Address [0...240]',1,64,'0',40,'','','','','');
INSERT INTO tmplib_DevLib_io VALUES('DCON_Prt_DI','di0','Input: DI0',3,128,'',7,'','','','','');
INSERT INTO tmplib_DevLib_io VALUES('DCON_Prt_DI','di1','Input: DI1',3,128,'',8,'','','','','');
INSERT INTO tmplib_DevLib_io VALUES('DCON_Prt_DI','di2','Input: DI2',3,128,'',9,'','','','','');
INSERT INTO tmplib_DevLib_io VALUES('DCON_Prt_DI','di3','Input: DI3',3,128,'',10,'','','','','');
INSERT INTO tmplib_DevLib_io VALUES('DCON_Prt_DI','di4','Input: DI4',3,128,'',11,'','','','','');
INSERT INTO tmplib_DevLib_io VALUES('DCON_Prt_DI','di5','Input: DI5',3,128,'',12,'','','','','');
INSERT INTO tmplib_DevLib_io VALUES('DCON_Prt_DI','di6','Input: DI6',3,128,'',13,'','','','','');
INSERT INTO tmplib_DevLib_io VALUES('DCON_Prt_DI','di7','Input: DI7',3,128,'',14,'','','','','');
INSERT INTO tmplib_DevLib_io VALUES('DCON_Prt_DI','di8','Input: DI8',3,128,'',15,'','','','','');
INSERT INTO tmplib_DevLib_io VALUES('DCON_Prt_DI','di9','Input: DI9',3,128,'',16,'','','','','');
INSERT INTO tmplib_DevLib_io VALUES('DCON_Prt_DI','di10','Input: DI10',3,128,'',17,'','','','','');
INSERT INTO tmplib_DevLib_io VALUES('DCON_Prt_DI','di11','Input: DI11',3,128,'',18,'','','','','');
INSERT INTO tmplib_DevLib_io VALUES('DCON_Prt_DI','di12','Input: DI12',3,128,'',19,'','','','','');
INSERT INTO tmplib_DevLib_io VALUES('DCON_Prt_DI','di13','Input: DI13',3,128,'',20,'','','','','');
INSERT INTO tmplib_DevLib_io VALUES('DCON_Prt_DI','di14','Input: DI14',3,128,'',21,'','','','','');
INSERT INTO tmplib_DevLib_io VALUES('DCON_Prt_DI','di15','Input: DI15',3,128,'',22,'','','','','');
INSERT INTO tmplib_DevLib_io VALUES('DCON_Prt_DI','di16','Input: DI16',3,128,'',23,'','','','','');
INSERT INTO tmplib_DevLib_io VALUES('DCON_Prt_DI','di17','Input: DI17',3,128,'',24,'','','','','');
INSERT INTO tmplib_DevLib_io VALUES('DCON_Prt_DI','di18','Input: DI18',3,128,'',25,'','','','','');
INSERT INTO tmplib_DevLib_io VALUES('DCON_Prt_DI','di19','Input: DI19',3,128,'',26,'','','','','');
INSERT INTO tmplib_DevLib_io VALUES('DCON_Prt_DI','di20','Input: DI20',3,128,'',27,'','','','','');
INSERT INTO tmplib_DevLib_io VALUES('DCON_Prt_DI','di21','Input: DI21',3,128,'',28,'','','','','');
INSERT INTO tmplib_DevLib_io VALUES('DCON_Prt_DI','di22','Input: DI22',3,128,'',29,'','','','','');
INSERT INTO tmplib_DevLib_io VALUES('DCON_Prt_DI','di23','Input: DI23',3,128,'',30,'','','','','');
INSERT INTO tmplib_DevLib_io VALUES('DCON_Prt_DI','di24','Input: DI24',3,128,'',31,'','','','','');
INSERT INTO tmplib_DevLib_io VALUES('DCON_Prt_DI','di25','Input: DI25',3,128,'',32,'','','','','');
INSERT INTO tmplib_DevLib_io VALUES('DCON_Prt_DI','di26','Input: DI26',3,128,'',33,'','','','','');
INSERT INTO tmplib_DevLib_io VALUES('DCON_Prt_DI','di27','Input: DI27',3,128,'',34,'','','','','');
INSERT INTO tmplib_DevLib_io VALUES('DCON_Prt_DI','di28','Input: DI28',3,128,'',35,'','','','','');
INSERT INTO tmplib_DevLib_io VALUES('DCON_Prt_DI','di29','Input: DI29',3,128,'',36,'','','','','');
INSERT INTO tmplib_DevLib_io VALUES('DCON_Prt_DI','di30','Input: DI30',3,128,'',37,'','','','','');
INSERT INTO tmplib_DevLib_io VALUES('DCON_Prt_DI','di31','Input: DI31',3,128,'',38,'','','','','');
INSERT INTO tmplib_DevLib_io VALUES('DCON_Prt_AO','tr','Transport',4,0,'',0,'Транспорт','','Транспорт','','');
INSERT INTO tmplib_DevLib_io VALUES('DCON_Prt_AO','io','Output: IO',4,0,'',1,'','','','','');
INSERT INTO tmplib_DevLib_io VALUES('DCON_Prt_AO','rez','Input: result',3,1,'0',2,'','','','','');
INSERT INTO tmplib_DevLib_io VALUES('DCON_Prt_AO','request','Input: request',0,0,'',3,'','','','','');
INSERT INTO tmplib_DevLib_io VALUES('DCON_Prt_AO','answer','Input: answer',0,1,'',4,'','','','','');
INSERT INTO tmplib_DevLib_io VALUES('DCON_Prt_AO','sender','Input: sender',0,0,'',5,'','','','','');
INSERT INTO tmplib_DevLib_io VALUES('DCON_Prt_AO','ao0','Input: AO0',2,129,'',6,'','','','','');
INSERT INTO tmplib_DevLib_io VALUES('DCON_Prt_AO','ao1','Input: AO1',2,129,'',7,'','','','','');
INSERT INTO tmplib_DevLib_io VALUES('DCON_Prt_AO','ao2','Input: AO2',2,129,'',8,'','','','','');
INSERT INTO tmplib_DevLib_io VALUES('DCON_Prt_AO','ao3','Input: AO3',2,129,'',9,'','','','','');
INSERT INTO tmplib_DevLib_io VALUES('DCON_Prt_AO','ao4','Input: AO4',2,129,'',10,'','','','','');
INSERT INTO tmplib_DevLib_io VALUES('DCON_Prt_AO','ao5','Input: AO5',2,129,'',11,'','','','','');
INSERT INTO tmplib_DevLib_io VALUES('DCON_Prt_AO','ao6','Input: AO6',2,129,'',12,'','','','','');
INSERT INTO tmplib_DevLib_io VALUES('DCON_Prt_AO','ao7','Input: AO7',2,129,'',13,'','','','','');
INSERT INTO tmplib_DevLib_io VALUES('DCON_Prt_AO','enCRC','Enable CRC',3,64,'0',14,'','','','','');
INSERT INTO tmplib_DevLib_io VALUES('DCON_Prt_AO','addr','Address [0...240]',1,64,'0',15,'','','','','');
INSERT INTO tmplib_DevLib_io VALUES('DCON_Prt_DO','tr','Transport',4,0,'',0,'Транспорт','','Транспорт','','');
INSERT INTO tmplib_DevLib_io VALUES('DCON_Prt_DO','io','Output: IO',4,0,'',1,'','','','','');
INSERT INTO tmplib_DevLib_io VALUES('DCON_Prt_DO','rez','Input: result',3,1,'0',2,'','','','','');
INSERT INTO tmplib_DevLib_io VALUES('DCON_Prt_DO','request','Input: request',0,0,'',3,'','','','','');
INSERT INTO tmplib_DevLib_io VALUES('DCON_Prt_DO','answer','Input: answer',0,1,'',4,'','','','','');
INSERT INTO tmplib_DevLib_io VALUES('DCON_Prt_DO','sender','Input: sender',0,0,'',5,'','','','','');
INSERT INTO tmplib_DevLib_io VALUES('DCON_Prt_DO','n','Input: number of DO, to read back',1,64,'8',6,'','','','','');
INSERT INTO tmplib_DevLib_io VALUES('DCON_Prt_DO','enCRC','Enable CRC',3,64,'0',39,'','','','','');
INSERT INTO tmplib_DevLib_io VALUES('DCON_Prt_DO','addr','Address [0...240]',1,64,'0',40,'','','','','');
INSERT INTO tmplib_DevLib_io VALUES('DCON_Prt_DO','do0','Input: DO0',3,129,'',7,'','','','','');
INSERT INTO tmplib_DevLib_io VALUES('DCON_Prt_DO','do1','Input: DO1',3,129,'',8,'','','','','');
INSERT INTO tmplib_DevLib_io VALUES('DCON_Prt_DO','do2','Input: DO2',3,129,'',9,'','','','','');
INSERT INTO tmplib_DevLib_io VALUES('DCON_Prt_DO','do3','Input: DO3',3,129,'',10,'','','','','');
INSERT INTO tmplib_DevLib_io VALUES('DCON_Prt_DO','do4','Input: DO4',3,129,'',11,'','','','','');
INSERT INTO tmplib_DevLib_io VALUES('DCON_Prt_DO','do5','Input: DO5',3,129,'',12,'','','','','');
INSERT INTO tmplib_DevLib_io VALUES('DCON_Prt_DO','do6','Input: DO6',3,129,'',13,'','','','','');
INSERT INTO tmplib_DevLib_io VALUES('DCON_Prt_DO','do7','Input: DO7',3,129,'',14,'','','','','');
INSERT INTO tmplib_DevLib_io VALUES('DCON_Prt_DO','do8','Input: DO8',3,129,'',15,'','','','','');
INSERT INTO tmplib_DevLib_io VALUES('DCON_Prt_DO','do9','Input: DO9',3,129,'',16,'','','','','');
INSERT INTO tmplib_DevLib_io VALUES('DCON_Prt_DO','do10','Input: DO10',3,129,'',17,'','','','','');
INSERT INTO tmplib_DevLib_io VALUES('DCON_Prt_DO','do11','Input: DO11',3,129,'',18,'','','','','');
INSERT INTO tmplib_DevLib_io VALUES('DCON_Prt_DO','do12','Input: DO12',3,129,'',19,'','','','','');
INSERT INTO tmplib_DevLib_io VALUES('DCON_Prt_DO','do13','Input: DO13',3,129,'',20,'','','','','');
INSERT INTO tmplib_DevLib_io VALUES('DCON_Prt_DO','do14','Input: DO14',3,129,'',21,'','','','','');
INSERT INTO tmplib_DevLib_io VALUES('DCON_Prt_DO','do15','Input: DO15',3,129,'',22,'','','','','');
INSERT INTO tmplib_DevLib_io VALUES('DCON_Prt_DO','do16','Input: DO16',3,129,'',23,'','','','','');
INSERT INTO tmplib_DevLib_io VALUES('DCON_Prt_DO','do17','Input: DO17',3,129,'',24,'','','','','');
INSERT INTO tmplib_DevLib_io VALUES('DCON_Prt_DO','do18','Input: DO18',3,129,'',25,'','','','','');
INSERT INTO tmplib_DevLib_io VALUES('DCON_Prt_DO','do19','Input: DO19',3,129,'',26,'','','','','');
INSERT INTO tmplib_DevLib_io VALUES('DCON_Prt_DO','do20','Input: DO20',3,129,'',27,'','','','','');
INSERT INTO tmplib_DevLib_io VALUES('DCON_Prt_DO','do21','Input: DO21',3,129,'',28,'','','','','');
INSERT INTO tmplib_DevLib_io VALUES('DCON_Prt_DO','do22','Input: DO22',3,129,'',29,'','','','','');
INSERT INTO tmplib_DevLib_io VALUES('DCON_Prt_DO','do23','Input: DO23',3,129,'',30,'','','','','');
INSERT INTO tmplib_DevLib_io VALUES('DCON_Prt_DO','do24','Input: DO24',3,129,'',31,'','','','','');
INSERT INTO tmplib_DevLib_io VALUES('DCON_Prt_DO','do25','Input: DO25',3,129,'',32,'','','','','');
INSERT INTO tmplib_DevLib_io VALUES('DCON_Prt_DO','do26','Input: DO26',3,129,'',33,'','','','','');
INSERT INTO tmplib_DevLib_io VALUES('DCON_Prt_DO','do27','Input: DO27',3,129,'',34,'','','','','');
INSERT INTO tmplib_DevLib_io VALUES('DCON_Prt_DO','do28','Input: DO28',3,129,'',35,'','','','','');
INSERT INTO tmplib_DevLib_io VALUES('DCON_Prt_DO','do29','Input: DO29',3,129,'',36,'','','','','');
INSERT INTO tmplib_DevLib_io VALUES('DCON_Prt_DO','do30','Input: DO30',3,129,'',37,'','','','','');
INSERT INTO tmplib_DevLib_io VALUES('DCON_Prt_DO','do31','Input: DO31',3,129,'',38,'','','','','');
INSERT INTO tmplib_DevLib_io VALUES('IEC60870','syncTimePLC','Sync PLC time',3,32,'',11,'Синхронизация времени ПЛК','','Синхронізація часу ПЛК','','');
INSERT INTO tmplib_DevLib_io VALUES('DNP3','transport','Transport',0,64,'Sockets.out_DNP3',0,'Транспорт','','Транспорт','','');
INSERT INTO tmplib_DevLib_io VALUES('DNP3','tmRetr','Retry connection time, seconds',2,64,'10',3,'Время восстановления подключения, секунд','','Час відновлення підключення, секунд','','');
INSERT INTO tmplib_DevLib_io VALUES('DNP3','itemsSet','Items set by: "ai|ao|cnt|di|do:{pnt}[-{endPnt}][:a[:{NameBase}]]"',0,36,'',6,'Набор элементов по: "ai|ao|cnt|di|do:{pnt}[-{endPnt}][:a[:{NameBase}]]"','','Набір елементів за: "ai|ao|cnt|di|do:{pnt}[-{endPnt}][:a[:{NameBase}]]"','','');
INSERT INTO tmplib_DevLib_io VALUES('DNP3','items','All items',4,33,'',7,'Все элементы','','Всі елементи','','');
INSERT INTO tmplib_DevLib_io VALUES('DNP3','this','Object',4,0,'',11,'Объект','','Об''єкт','','');
INSERT INTO tmplib_DevLib_io VALUES('DNP3','ctx','IO context',4,0,'',9,'Контекст ВВ','','Контекст ВВ','','');
INSERT INTO tmplib_DevLib_io VALUES('DNP3','dest','Destination [0...65520]',1,64,'10',1,'Назначение [0...65520]','','Призначення [0...65520]','','');
INSERT INTO tmplib_DevLib_io VALUES('DNP3','src','Source [0...65520]',1,64,'1',2,'Источник [0...65520]','','Джерело [0...65520]','','');
INSERT INTO tmplib_DevLib_io VALUES('DNP3','tr','Output transport',4,0,'',8,'Выходной транспорт','','Вихідний транспорт','','');
INSERT INTO tmplib_DevLib_io VALUES('DNP3','tmPoolEv','Poll events time, seconds',2,64,'1',4,'Время опроса событий, секунд','','Час опитування повідомлень, секунд','','');
INSERT INTO tmplib_DevLib_io VALUES('DNP3','tmPoolAll','Poll all time, seconds',2,64,'60',5,'Время опроса всего, секунд','','Час опитування всього, секунд','','');
INSERT INTO tmplib_DevLib_io VALUES('DNP3','oAVals','Output values',4,1,'',10,'Выходные значения','','Вихідні значення','','');
INSERT INTO tmplib_DevLib_io VALUES('IEC60870','tmPoolAll','Poll all time, seconds',2,64,'60',3,'Время опроса всего, секунд','','Час опитування всього, секунд','','');
INSERT INTO tmplib_DevLib_io VALUES('IEC60870','oAVals','Output values',4,0,'',12,'Выходные значения','','Вихідні значення','','');
INSERT INTO tmplib_DevLib_io VALUES('IEC62056','transport','Transport',0,64,'Serial.out_IEC62056',0,'Транспорт','','Транспорт','','');
INSERT INTO tmplib_DevLib_io VALUES('IEC62056','dest','Destination "{UHDLC}[:{LHDLC}]", [0...16383]',0,64,'17:6168',1,'Назначение "{UHDLC}[:{LHDLC}]", [0...16383]','','Призначення "{UHDLC}[:{LHDLC}]", [0...16383]','','');
INSERT INTO tmplib_DevLib_io VALUES('IEC62056','src','Source [0...127]',1,64,'1',2,'Источник [0...127]','','Джерело [0...127]','','');
INSERT INTO tmplib_DevLib_io VALUES('IEC62056','tmRetr','Retry connection time, seconds',2,64,'10',4,'Время восстановления подключения, секунд','','Час відновлення підключення, секунд','','');
INSERT INTO tmplib_DevLib_io VALUES('IEC62056','itemsSet','Items set by: "{ClassId}-{OBIS}-{attr}"',0,36,'',6,'Набор элементов по: "{OBIS}-{attr}"','','Набір елементів за: "{OBIS}-{attr}"','','');
INSERT INTO tmplib_DevLib_io VALUES('IEC62056','items','All items',4,33,'',7,'Все элементы','','Всі елементи','','');
INSERT INTO tmplib_DevLib_io VALUES('IEC62056','tr','Output transport',4,0,'',8,'Выходной транспорт','','Вихідний транспорт','','');
INSERT INTO tmplib_DevLib_io VALUES('IEC62056','ctx','IO context',4,0,'',9,'Контекст ВВ','','Контекст ВВ','','');
INSERT INTO tmplib_DevLib_io VALUES('IEC62056','this','Object',4,0,'',10,'Объект','','Об''єкт','','');
INSERT INTO tmplib_DevLib_io VALUES('IEC62056','pass','Password the device',0,64,'ABCDEFGH',3,'Пароль устройства','','Пароль пристрою','','');
INSERT INTO tmplib_DevLib_io VALUES('IEC62056','tmPollAll','Poll all time, seconds',2,64,'10',5,'Время опроса всего, секунд','','Час опитування всього, секунд','','');
INSERT INTO tmplib_DevLib_io VALUES('MTP4D','transport','Transport',0,64,'Serial.MTP',0,'Транспорт','','Транспорт','','');
INSERT INTO tmplib_DevLib_io VALUES('MTP4D','addr','Device address, [1...999]',1,64,'1',1,'Адрес устройства, [1...999]','','Адреса пристрою, [1...999]','','');
INSERT INTO tmplib_DevLib_io VALUES('MTP4D','type','Device type',0,16,'',2,'Тип устройства','','Тип пристрою','','');
INSERT INTO tmplib_DevLib_io VALUES('MTP4D','P','Pressure',2,16,'',3,'Давление','','Тиск','','');
INSERT INTO tmplib_DevLib_io VALUES('MTP4D','tr','Output transport',4,0,'',6,'Выходной транспорт','','Вихідний транспорт','','');
INSERT INTO tmplib_DevLib_io VALUES('MTP4D','this','Object',4,0,'',7,'Объект','','Об''єкт','','');
INSERT INTO tmplib_DevLib_io VALUES('mbBase','transport','Transport',0,64,'Sockets.mbbase',0,'Транспорт','','Транспорт','','');
INSERT INTO tmplib_DevLib_io VALUES('mbBase','addr','Device address, [0...247]',1,64,'1',1,'Адрес устройства, [0...247]','','Адреса пристрою, [0...247]','','');
INSERT INTO tmplib_DevLib_io VALUES('mbBase','mbType','ModBus type, [RTU|ASCII|TCP]',0,64,'RTU',2,'Тип ModBus, [RTU|ASCII|TCP]','','Тип ModBus, [RTU|ASCII|TCP]','','');
INSERT INTO tmplib_DevLib_io VALUES('mbBase','maxBlkSz','ModBus maximum block size, [10...200]',1,64,'200',3,'Максимальный размер блока ModBus, [10...200]','','Максимальний розмір блоку ModBus, [10...200]','','');
INSERT INTO tmplib_DevLib_io VALUES('mbBase','fragMerge','ModBus blocks merging',3,64,'1',4,'Объединение блоков ModBus','','Поєднання блоків ModBus','','');
INSERT INTO tmplib_DevLib_io VALUES('mbBase','items','Items set "[u|i|u2|i2|u4|i4|r|s]:{addr}:{w|r|~}:{id}:{nm}"',0,36,'',5,'Набор элементов "[u|i|u2|i2|u4|i4|r|s]:{addr}:{w|r|~}:{id}:{nm}"','','Набір елементів "[u|i|u2|i2|u4|i4|r|s]:{addr}:{w|r|~}:{id}:{nm}"','','');
INSERT INTO tmplib_DevLib_io VALUES('mbBase','tr','Output transport',4,0,'',6,'Выходной транспорт','','Вихідний транспорт','','');
INSERT INTO tmplib_DevLib_io VALUES('mbBase','this','Object',4,0,'',7,'Объект','','Об''єкт','','');
INSERT INTO tmplib_DevLib_io VALUES('MTP4D','zeroP','Set zero',3,32,'',4,'Установить ноль','','Встановити нуль','','');
INSERT INTO tmplib_DevLib_io VALUES('MTP4D','zeroAP','Set atmosphere',3,32,'',5,'Установить атмосферу','','Встановити атмосферу','','');
CREATE TABLE IF NOT EXISTS 'tmplib_LowDevLib_io' ("TMPL_ID" TEXT DEFAULT '' ,"ID" TEXT DEFAULT '' ,"NAME" TEXT DEFAULT '' ,"TYPE" INTEGER DEFAULT '0' ,"FLAGS" INTEGER DEFAULT '0' ,"VALUE" TEXT DEFAULT '' ,"POS" INTEGER DEFAULT '0' ,"uk#NAME" TEXT DEFAULT '' ,"ru#NAME" TEXT DEFAULT '' ,"uk#VALUE" TEXT DEFAULT '' ,"ru#VALUE" TEXT DEFAULT '' ,"sr#NAME" TEXT DEFAULT '' , PRIMARY KEY ("TMPL_ID","ID"));
INSERT INTO tmplib_LowDevLib_io VALUES('1602A','transport','Transport of the I2C, Serial (i2c) or
GPIO address with function put(), mostly it''s BCM2835 (DAQ.BCM2835.pi.pi)',0,64,'i2c',0,'','','','','');
INSERT INTO tmplib_LowDevLib_io VALUES('1602A','addr','I2C device address [0...119]',1,64,'39',1,'','','','','');
INSERT INTO tmplib_LowDevLib_io VALUES('1602A','RS','GPIO Pin: Reset',1,64,'7',2,'','','','','');
INSERT INTO tmplib_LowDevLib_io VALUES('1602A','E','GPIO Pin: Enable',1,64,'8',3,'','','','','');
INSERT INTO tmplib_LowDevLib_io VALUES('1602A','D4','GPIO Pin: Data4',1,64,'25',4,'','','','','');
INSERT INTO tmplib_LowDevLib_io VALUES('1602A','D5','GPIO Pin: Data5',1,64,'24',5,'','','','','');
INSERT INTO tmplib_LowDevLib_io VALUES('1602A','D6','GPIO Pin: Data6',1,64,'23',6,'','','','','');
INSERT INTO tmplib_LowDevLib_io VALUES('1602A','D7','GPIO Pin: Data7',1,64,'18',7,'','','','','');
INSERT INTO tmplib_LowDevLib_io VALUES('1602A','ln1','Line 1',0,32,'',8,'','','','','');
INSERT INTO tmplib_LowDevLib_io VALUES('1602A','ln2','Line 2',0,32,'',9,'','','','','');
INSERT INTO tmplib_LowDevLib_io VALUES('1W_DS9097','transport','Transport of the One Wire bus, Serial',0,64,'oneWire',0,'','','','','');
INSERT INTO tmplib_LowDevLib_io VALUES('1W_DS9097','tmResc','Rescan period, s',2,64,'60',1,'','','','','');
INSERT INTO tmplib_LowDevLib_io VALUES('1W_DS9097','tryEVAL','Tries after which set value to EVAL',1,64,'3',2,'','','','','');
INSERT INTO tmplib_LowDevLib_io VALUES('1W_DS9097','power','Power, for temperature',3,16,'',3,'','','','','');
INSERT INTO tmplib_LowDevLib_io VALUES('1W_DS9097','this','Object',4,0,'',4,'Об''єкт','Объект','','','');
INSERT INTO tmplib_LowDevLib_io VALUES('1W_DS9097U','transport','Transport of the One Wire bus, Serial',0,64,'oneWire',0,'','','','','');
INSERT INTO tmplib_LowDevLib_io VALUES('1W_DS9097U','tmResc','Rescan period, s',2,64,'60',1,'','','','','');
INSERT INTO tmplib_LowDevLib_io VALUES('1W_DS9097U','tryEVAL','Tries after which set value to EVAL',1,64,'3',2,'','','','','');
INSERT INTO tmplib_LowDevLib_io VALUES('1W_DS9097U','power','Power, for temperature',3,16,'',3,'','','','','');
INSERT INTO tmplib_LowDevLib_io VALUES('1W_DS9097U','isData','In data mode',3,0,'0',4,'','','','','');
INSERT INTO tmplib_LowDevLib_io VALUES('1W_DS9097U','this','Object',4,0,'',5,'Об''єкт','Объект','','','');
INSERT INTO tmplib_LowDevLib_io VALUES('BMP180','transport','Transport of the I2C, Serial',0,64,'i2c',0,'','','','','');
INSERT INTO tmplib_LowDevLib_io VALUES('BMP180','addr','Device address [0...119]',1,64,'119',1,'','','','','');
INSERT INTO tmplib_LowDevLib_io VALUES('BMP180','oss','Oversampling setting (0...3)',1,64,'0',2,'','','','','');
INSERT INTO tmplib_LowDevLib_io VALUES('BMP180','t','T, °С',2,16,'',3,'','','','','');
INSERT INTO tmplib_LowDevLib_io VALUES('BMP180','p','P, Pa',2,16,'',4,'','','','','');
INSERT INTO tmplib_LowDevLib_io VALUES('DHT','addr','GPIO address with functions mode(), get() and put(), mostly it''s BCM2835',0,64,'DAQ.GPIO.io.pi',0,'','','','','');
INSERT INTO tmplib_LowDevLib_io VALUES('DHT','pin','IO pin number of the GPIO',1,64,'17',1,'','','','','');
INSERT INTO tmplib_LowDevLib_io VALUES('DHT','tries','Tries [1...5]',1,64,'2',2,'','','','','');
INSERT INTO tmplib_LowDevLib_io VALUES('DHT','dev','Device (0-DHT11, 1-DHT22)',1,64,'1',3,'','','','','');
INSERT INTO tmplib_LowDevLib_io VALUES('DHT','t','T, °С',2,16,'',4,'','','','','');
INSERT INTO tmplib_LowDevLib_io VALUES('DHT','h','H, %',2,16,'',5,'','','','','');
INSERT INTO tmplib_LowDevLib_io VALUES('DS3231','transport','Transport of the I2C, Serial',0,64,'i2c',0,'','','','','');
INSERT INTO tmplib_LowDevLib_io VALUES('DS3231','addr','Device address [0...119]',1,64,'104',1,'','','','','');
INSERT INTO tmplib_LowDevLib_io VALUES('DS3231','tm','Date and time, YYYY-MM-DDTHH:mm:SS',0,32,'',3,'','','','','');
INSERT INTO tmplib_LowDevLib_io VALUES('DS3231','pSQW','Enable SQUARE-WAVE OUTPUT',3,32,'',4,'','','','','');
INSERT INTO tmplib_LowDevLib_io VALUES('PCF8574','transport','Transport of the I2C, Serial',0,64,'i2c',0,'','','','','');
INSERT INTO tmplib_LowDevLib_io VALUES('PCF8574','addr','Device address [0...119]',1,64,'39',1,'','','','','');
INSERT INTO tmplib_LowDevLib_io VALUES('PCF8574','di0','DI0',3,16,'',2,'','','','','');
INSERT INTO tmplib_LowDevLib_io VALUES('PCF8574','di1','DI1',3,16,'',3,'','','','','');
INSERT INTO tmplib_LowDevLib_io VALUES('PCF8574','di2','DI2',3,16,'',4,'','','','','');
INSERT INTO tmplib_LowDevLib_io VALUES('PCF8574','di3','DI3',3,16,'',5,'','','','','');
INSERT INTO tmplib_LowDevLib_io VALUES('PCF8574','di4','DI4',3,16,'',6,'','','','','');
INSERT INTO tmplib_LowDevLib_io VALUES('PCF8574','di5','DI5',3,16,'',7,'','','','','');
INSERT INTO tmplib_LowDevLib_io VALUES('PCF8574','di6','DI6',3,16,'',8,'','','','','');
INSERT INTO tmplib_LowDevLib_io VALUES('PCF8574','di7','DI7',3,16,'',9,'','','','','');
INSERT INTO tmplib_LowDevLib_io VALUES('PCF8574','do0','DO0',3,33,'',10,'','','','','');
INSERT INTO tmplib_LowDevLib_io VALUES('PCF8574','do1','DO1',3,33,'',11,'','','','','');
INSERT INTO tmplib_LowDevLib_io VALUES('PCF8574','do2','DO2',3,33,'',12,'','','','','');
INSERT INTO tmplib_LowDevLib_io VALUES('PCF8574','do3','DO3',3,33,'',13,'','','','','');
INSERT INTO tmplib_LowDevLib_io VALUES('PCF8574','do4','DO4',3,33,'',14,'','','','','');
INSERT INTO tmplib_LowDevLib_io VALUES('PCF8574','do5','DO5',3,33,'',15,'','','','','');
INSERT INTO tmplib_LowDevLib_io VALUES('PCF8574','do6','DO6',3,33,'',16,'','','','','');
INSERT INTO tmplib_LowDevLib_io VALUES('PCF8574','do7','DO7',3,33,'',17,'','','','','');
INSERT INTO tmplib_LowDevLib_io VALUES('PCF8591','transport','Transport of the I2C, Serial',0,64,'i2c',0,'','','','','');
INSERT INTO tmplib_LowDevLib_io VALUES('PCF8591','addr','Device address [0...119]',1,64,'72',1,'','','','','');
INSERT INTO tmplib_LowDevLib_io VALUES('PCF8591','vRef','Reference voltage, V',2,64,'3.2',2,'','','','','');
INSERT INTO tmplib_LowDevLib_io VALUES('PCF8591','ai0','AI0',2,16,'',3,'','','','','');
INSERT INTO tmplib_LowDevLib_io VALUES('PCF8591','ai1','AI1',2,16,'',4,'','','','','');
INSERT INTO tmplib_LowDevLib_io VALUES('PCF8591','ai2','AI2',2,16,'',5,'','','','','');
INSERT INTO tmplib_LowDevLib_io VALUES('PCF8591','ai3','AI3',2,16,'',6,'','','','','');
INSERT INTO tmplib_LowDevLib_io VALUES('PCF8591','ao','AO',2,32,'',7,'','','','','');
INSERT INTO tmplib_LowDevLib_io VALUES('BME280','transport','Transport of the I2C, Serial',0,64,'i2c',0,'','','','','');
INSERT INTO tmplib_LowDevLib_io VALUES('BME280','addr','Device address [0...119]',1,64,'118',1,'','','','','');
INSERT INTO tmplib_LowDevLib_io VALUES('BME280','oss','Oversampling setting (0...7)',1,64,'3',2,'','','','','');
INSERT INTO tmplib_LowDevLib_io VALUES('BME280','t','T, °С',2,16,'',3,'','','','','');
INSERT INTO tmplib_LowDevLib_io VALUES('BME280','p','P, Pa',2,16,'',4,'','','','','');
INSERT INTO tmplib_LowDevLib_io VALUES('BME280','h','H, %',2,16,'',5,'','','','','');
INSERT INTO tmplib_LowDevLib_io VALUES('DS3231','mode','Mode',1,40,'0
0;1
DS3231;DS1307',2,'Режим','Режим','','','');
INSERT INTO tmplib_LowDevLib_io VALUES('DS3231','this','Parameter',4,0,'',6,'Параметр','Параметр','','','');
INSERT INTO tmplib_LowDevLib_io VALUES('DS3231','pSQWf','SQUARE-WAVE OUTPUT frequency',1,32,'',5,'','','','','');
INSERT INTO tmplib_LowDevLib_io VALUES('SHT3x','transport','Transport of the I2C, Serial',0,64,'i2c',0,'','','','','');
INSERT INTO tmplib_LowDevLib_io VALUES('SHT3x','addr','Device address [0...119]',1,64,'68',1,'','','','','');
INSERT INTO tmplib_LowDevLib_io VALUES('SHT3x','H','Humidity',2,16,'',2,'Вологість','Влажность','','','Влажност');
INSERT INTO tmplib_LowDevLib_io VALUES('SHT3x','T','Temperature',2,16,'',3,'Температура','Температура','','','Температура');
INSERT INTO tmplib_LowDevLib_io VALUES('ADS111x','transport','Transport of the I2C, Serial',0,64,'i2c',0,'','','','','');
INSERT INTO tmplib_LowDevLib_io VALUES('ADS111x','addr','Device address [0...119]',1,64,'72',1,'','','','','');
INSERT INTO tmplib_LowDevLib_io VALUES('ADS111x','range','Range, ±V',1,40,'2
0;1;2;3;4;5
6.144;4.096;2.048;1.024;0.512;0.256',2,'','','','','');
INSERT INTO tmplib_LowDevLib_io VALUES('ADS111x','ai0','AI0',2,16,'',3,'','','','','');
INSERT INTO tmplib_LowDevLib_io VALUES('ADS111x','ai1','AI1',2,16,'',4,'','','','','');
INSERT INTO tmplib_LowDevLib_io VALUES('ADS111x','ai2','AI2',2,16,'',5,'','','','','');
INSERT INTO tmplib_LowDevLib_io VALUES('ADS111x','ai3','AI3',2,16,'',6,'','','','','');
INSERT INTO tmplib_LowDevLib_io VALUES('MCP4725','transport','Transport of the I2C, Serial',0,64,'i2c',0,'','','','','');
INSERT INTO tmplib_LowDevLib_io VALUES('MCP4725','addr','Device address [0...119]',1,64,'96',1,'','','','','');
INSERT INTO tmplib_LowDevLib_io VALUES('MCP4725','vRef','Reference voltage, V',2,64,'3.2',2,'','','','','');
INSERT INTO tmplib_LowDevLib_io VALUES('MCP4725','ao','AO',2,32,'',3,'','','','','');
INSERT INTO tmplib_LowDevLib_io VALUES('MAX6675','addr','GPIO address with functions mode(), get() and put(), mostly it''s BCM2835',0,64,'DAQ.GPIO.io.pi',0,'','','','','');
INSERT INTO tmplib_LowDevLib_io VALUES('MAX6675','pin_cs','CS pin number of the GPIO',1,64,'8',1,'','','','','');
INSERT INTO tmplib_LowDevLib_io VALUES('MAX6675','pin_sclk','SCLK pin number of the GPIO',1,64,'11',2,'','','','','');
INSERT INTO tmplib_LowDevLib_io VALUES('MAX6675','pin_miso','MISO pin number of the GPIO',1,64,'9',3,'','','','','');
INSERT INTO tmplib_LowDevLib_io VALUES('MAX6675','t','T, °С',2,17,'',4,'','','','','');
CREATE TABLE IF NOT EXISTS 'tmplib_tests_io' ("TMPL_ID" TEXT DEFAULT '' ,"ID" TEXT DEFAULT '' ,"NAME" TEXT DEFAULT '' ,"TYPE" INTEGER DEFAULT '' ,"FLAGS" INTEGER DEFAULT '' ,"VALUE" TEXT DEFAULT '' ,"POS" INTEGER DEFAULT '' ,"uk#NAME" TEXT DEFAULT '' ,"uk#VALUE" TEXT DEFAULT '' ,"ru#NAME" TEXT DEFAULT '' ,"ru#VALUE" TEXT DEFAULT '' ,"sr#NAME" TEXT DEFAULT '' , PRIMARY KEY ("TMPL_ID","ID"));
INSERT INTO tmplib_tests_io VALUES('ai_simple','val_cod','Value''s source code',1,128,'',0,'Вихідний код значення','','Исходный код значения','','');
INSERT INTO tmplib_tests_io VALUES('ai_simple','val','Value',2,16,'0',1,'Значення','','Значение','','Вредност');
INSERT INTO tmplib_tests_io VALUES('ai_simple','val_tp','Dimension',0,4128,'',2,'Одиниця виміру','','Единица измерения','','');
INSERT INTO tmplib_tests_io VALUES('ai_simple','bDwn','Lower value border',2,32,'0',3,'Нижня границя значення','','Нижняя граница значения','','');
INSERT INTO tmplib_tests_io VALUES('ai_simple','bUp','Upper value border',2,32,'100',4,'Верхня границя значення','','Верхняя граница значения','','');
INSERT INTO tmplib_tests_io VALUES('ai_simple','bWDwn','Lower technical border',2,32,'20',5,'Нижня технолог. границя','','Нижняя технологич. граница','','');
INSERT INTO tmplib_tests_io VALUES('ai_simple','bWUp','Upper technical border',2,32,'80',6,'Верхня технолог. границя','','Верхняя технологич. граница','','');
INSERT INTO tmplib_tests_io VALUES('ai_simple','bErrDwn','Lower alarm border',2,32,'10',7,'Нижня аварійна границя','','Нижняя аварийная граница','','Доњи услови укључивања аларма');
INSERT INTO tmplib_tests_io VALUES('ai_simple','bErrUp','Upper alarm border',2,32,'90',8,'Верхня аварійна границя','','Верхняя аварийная граница','','Горњи услови укључивања аларма');
INSERT INTO tmplib_tests_io VALUES('ai_man','val_cod','Value''s source code',1,129,'',0,'Вихідний код значення','','Исходный код значения','','');
INSERT INTO tmplib_tests_io VALUES('ai_man','val','Value',2,32,'0',1,'Значення','','Значение','','Вредност');
INSERT INTO tmplib_tests_io VALUES('ai_man','val_o','Previous value',2,0,'0',2,'Попередне значення','','Предыдущее значение','','');
INSERT INTO tmplib_tests_io VALUES('ai_man','val_tp','Dimension',0,4128,'',3,'Одиниця виміру','','Единица измерения','','');
INSERT INTO tmplib_tests_io VALUES('ai_man','bDwn','Lower value border',2,32,'0',4,'Нижня границя значення','','Нижняя граница значения','','');
INSERT INTO tmplib_tests_io VALUES('ai_man','bUp','Upper value border',2,32,'100',5,'Верхня границя значення','','Верхняя граница значения','','');
INSERT INTO tmplib_tests_io VALUES('ai_man','bWDwn','Lower technical border',2,32,'20',6,'Нижня технолог. границя','','Нижняя технологич. граница','','');
INSERT INTO tmplib_tests_io VALUES('ai_man','bWUp','Upper technical border',2,32,'80',7,'Верхня технолог. границя','','Верхняя технологич. граница','','');
INSERT INTO tmplib_tests_io VALUES('ai_man','bErrDwn','Lower alarm border',2,32,'10',8,'Нижня аварійна границя','','Нижняя аварийная граница','','Доњи услови укључивања аларма');
INSERT INTO tmplib_tests_io VALUES('ai_man','bErrUp','Upper alarm border',2,32,'90',9,'Верхня аварійна границя','','Верхняя аварийная граница','','Горњи услови укључивања аларма');
INSERT INTO tmplib_tests_io VALUES('test','in50','Input50',1,160,'DB Test|108',51,'Вхід50','DB Тест|108','Вход50','DB Тест|108','');
INSERT INTO tmplib_tests_io VALUES('test','in49','Input49',1,160,'DB Test|106',50,'Вхід49','DB Тест|106','Вход49','DB Тест|106','');
INSERT INTO tmplib_tests_io VALUES('test','in48','Input48',1,160,'DB Test|104',49,'Вхід48','DB Тест|104','Вход48','DB Тест|104','');
INSERT INTO tmplib_tests_io VALUES('test','in47','Input47',1,160,'DB Test|102',48,'Вхід47','DB Тест|102','Вход47','DB Тест|102','');
INSERT INTO tmplib_tests_io VALUES('test','in46','Input46',1,160,'DB Test|100',47,'Вхід46','DB Тест|100','Вход46','DB Тест|100','');
INSERT INTO tmplib_tests_io VALUES('test','in45','Input45',1,160,'DB Test|98',46,'Вхід45','DB Тест|98','Вход45','DB Тест|98','');
INSERT INTO tmplib_tests_io VALUES('test','in44','Input44',1,160,'DB Test|96',45,'Вхід44','DB Тест|96','Вход44','DB Тест|96','');
INSERT INTO tmplib_tests_io VALUES('test','in43','Input43',1,160,'DB Test|94',44,'Вхід43','DB Тест|94','Вход43','DB Тест|94','');
INSERT INTO tmplib_tests_io VALUES('test','in42','Input42',1,160,'DB Test|92',43,'Вхід42','DB Тест|92','Вход42','DB Тест|92','');
INSERT INTO tmplib_tests_io VALUES('test','in41','Input41',1,160,'DB Test|90',42,'Вхід41','DB Тест|90','Вход41','DB Тест|90','');
INSERT INTO tmplib_tests_io VALUES('test','in40','Input40',1,160,'DB Test|86',41,'Вхід40','DB Тест|86','Вход40','DB Тест|86','');
INSERT INTO tmplib_tests_io VALUES('test','in39','Input39',1,160,'DB Test|84',40,'Вхід39','DB Тест|84','Вход39','DB Тест|84','');
INSERT INTO tmplib_tests_io VALUES('test','in38','Input38',1,160,'DB Test|82',39,'Вхід38','DB Тест|82','Вход38','DB Тест|82','');
INSERT INTO tmplib_tests_io VALUES('test','in37','Input37',1,160,'DB Test|80',38,'Вхід37','DB Тест|80','Вход37','DB Тест|80','');
INSERT INTO tmplib_tests_io VALUES('test','in36','Input36',1,160,'DB Test|78',37,'Вхід36','DB Тест|78','Вход36','DB Тест|78','');
INSERT INTO tmplib_tests_io VALUES('test','in35','Input35',1,160,'DB Test|76',36,'Вхід35','DB Тест|76','Вход35','DB Тест|76','');
INSERT INTO tmplib_tests_io VALUES('test','in34','Input34',1,160,'DB Test|74',35,'Вхід34','DB Тест|74','Вход34','DB Тест|74','');
INSERT INTO tmplib_tests_io VALUES('test','in33','Input33',1,160,'DB Test|72',34,'Вхід33','DB Тест|72','Вход33','DB Тест|72','');
INSERT INTO tmplib_tests_io VALUES('test','in32','Input32',1,160,'DB Test|70',33,'Вхід32','DB Тест|70','Вход32','DB Тест|70','');
INSERT INTO tmplib_tests_io VALUES('test','in31','Input31',1,160,'DB Test|68',32,'Вхід31','DB Тест|68','Вход31','DB Тест|68','');
INSERT INTO tmplib_tests_io VALUES('test','in30','Input30',1,160,'DB Test|64',31,'Вхід30','DB Тест|64','Вход30','DB Тест|64','');
INSERT INTO tmplib_tests_io VALUES('test','in29','Input29',1,160,'DB Test|62',30,'Вхід29','DB Тест|62','Вход29','DB Тест|62','');
INSERT INTO tmplib_tests_io VALUES('test','in28','Input28',1,160,'DB Test|60',29,'Вхід28','DB Тест|60','Вход28','DB Тест|60','');
INSERT INTO tmplib_tests_io VALUES('test','in27','Input27',1,160,'DB Test|58',28,'Вхід27','DB Тест|58','Вход27','DB Тест|58','');
INSERT INTO tmplib_tests_io VALUES('test','in26','Input26',1,160,'DB Test|56',27,'Вхід26','DB Тест|56','Вход26','DB Тест|56','');
INSERT INTO tmplib_tests_io VALUES('test','in25','Input25',1,160,'DB Test|54',26,'Вхід25','DB Тест|54','Вход25','DB Тест|54','');
INSERT INTO tmplib_tests_io VALUES('test','in24','Input24',1,160,'DB Test|52',25,'Вхід24','DB Тест|52','Вход24','DB Тест|52','');
INSERT INTO tmplib_tests_io VALUES('test','in23','Input23',1,160,'DB Test|50',24,'Вхід23','DB Тест|50','Вход23','DB Тест|50','');
INSERT INTO tmplib_tests_io VALUES('test','in22','Input22',1,160,'DB Test|48',23,'Вхід22','DB Тест|48','Вход22','DB Тест|48','');
INSERT INTO tmplib_tests_io VALUES('test','in21','Input21',1,160,'DB Test|46',22,'Вхід21','DB Тест|46','Вход21','DB Тест|46','');
INSERT INTO tmplib_tests_io VALUES('test','in20','Input20',1,160,'DB Test|44',21,'Вхід20','DB Тест|44','Вход20','DB Тест|44','');
INSERT INTO tmplib_tests_io VALUES('test','in19','Input19',1,160,'DB Test|42',20,'Вхід19','DB Тест|42','Вход19','DB Тест|42','');
INSERT INTO tmplib_tests_io VALUES('test','in18','Input18',1,160,'DB Test|40',19,'Вхід18','DB Тест|40','Вход18','DB Тест|40','');
INSERT INTO tmplib_tests_io VALUES('test','in17','Input17',1,160,'DB Test|38',18,'Вхід17','DB Тест|38','Вход17','DB Тест|38','');
INSERT INTO tmplib_tests_io VALUES('test','in16','Input16',1,160,'DB Test|36',17,'Вхід16','DB Тест|36','Вход16','DB Тест|36','');
INSERT INTO tmplib_tests_io VALUES('test','in15','Input15',1,160,'DB Test|34',16,'Вхід15','DB Тест|34','Вход15','DB Тест|34','');
INSERT INTO tmplib_tests_io VALUES('test','in14','Input14',1,160,'DB Test|32',15,'Вхід14','DB Тест|32','Вход14','DB Тест|32','');
INSERT INTO tmplib_tests_io VALUES('test','in13','Input13',1,160,'DB Test|30',14,'Вхід13','DB Тест|30','Вход13','DB Тест|30','');
INSERT INTO tmplib_tests_io VALUES('test','in12','Input12',1,160,'DB Test|28',13,'Вхід12','DB Тест|28','Вход12','DB Тест|28','');
INSERT INTO tmplib_tests_io VALUES('test','in11','Input11',1,160,'DB Test|26',12,'Вхід11','DB Тест|26','Вход11','DB Тест|26','');
INSERT INTO tmplib_tests_io VALUES('test','in10','Input10',1,160,'DB Test|24',11,'Вхід10','DB Тест|24','Вход10','DB Тест|24','');
INSERT INTO tmplib_tests_io VALUES('test','in9','Input9',1,160,'DB Test|22',10,'Вхід9','DB Тест|22','Вход9','DB Тест|22','');
INSERT INTO tmplib_tests_io VALUES('test','in8','Input8',1,160,'DB Test|20',9,'Вхід8','DB Тест|20','Вход8','DB Тест|20','');
INSERT INTO tmplib_tests_io VALUES('test','in7','Input7',1,160,'DB Test|18',8,'Вхід7','DB Тест|18','Вход7','DB Тест|18','');
INSERT INTO tmplib_tests_io VALUES('test','in6','Input6',1,160,'DB Test|16',7,'Вхід6','DB Тест|16','Вход6','DB Тест|16','');
INSERT INTO tmplib_tests_io VALUES('test','in5','Input5',1,160,'DB Test|14',6,'Вхід5','DB Тест|14','Вход5','DB Тест|14','');
INSERT INTO tmplib_tests_io VALUES('test','in4','Input4',1,160,'DB Test|12',5,'Вхід4','DB Тест|12','Вход4','DB Тест|12','');
INSERT INTO tmplib_tests_io VALUES('test','in3','Input3',1,160,'DB Test|10',4,'Вхід3','DB Тест|10','Вход3','DB Тест|10','');
INSERT INTO tmplib_tests_io VALUES('test','in2','Input2',1,160,'DB Test|8|r8',3,'Вхід2','DB Тест|8','Вход2','DB Тест|8','');
INSERT INTO tmplib_tests_io VALUES('test','in1','Input1',1,160,'DB Test|6|i4',2,'Вхід1','DB Тест|6','Вход1','DB Тест|6','');
INSERT INTO tmplib_tests_io VALUES('test','in','Input',1,160,'DB Test|4',1,'Вхід','DB Тест|4','Вход','DB Тест|4','');
INSERT INTO tmplib_tests_io VALUES('test','di','DI',3,160,'DB Test|2.1',0,'','DB Тест|2.1','','DB Тест|2.1','');
INSERT INTO tmplib_tests_io VALUES('gasPoint','F','Flow, m3/h',2,144,'F|var',0,'Витрати, м3/год','','Расход, м3/ч','','');
INSERT INTO tmplib_tests_io VALUES('gasPoint','Q','Volume, m3',2,16,'0',1,'Об''єм, м3','','Объём, м3','','');
INSERT INTO tmplib_tests_io VALUES('gasPoint','P','Pressure, kgH/sm2',2,144,'P|var',2,'Тиск, кГс/см2','','Давление, кГс/см2','','');
INSERT INTO tmplib_tests_io VALUES('gasPoint','T','Temperature, °C',2,144,'T|var',3,'Температура, °С','','Температура, °С','','');
INSERT INTO tmplib_tests_io VALUES('gasPoint','dP','Differential pressure, kgH/m2',2,16,'0',4,'Перепад, кГс/м2','','Перепад, кГс/м2','','');
INSERT INTO tmplib_tests_io VALUES('gasPoint','DS','Density, kg/m3',2,16,'0',5,'Щильність, кг/м3','','Плотность, кг/м3','','');
CREATE TABLE IF NOT EXISTS 'techApp_io' ("F_ID" TEXT DEFAULT '' ,"ID" TEXT DEFAULT '' ,"NAME" TEXT DEFAULT '' ,"TYPE" INTEGER DEFAULT '' ,"MODE" INTEGER DEFAULT '' ,"DEF" TEXT DEFAULT '' ,"HIDE" INTEGER DEFAULT '' ,"POS" INTEGER DEFAULT '' ,"uk#NAME" TEXT DEFAULT '' ,"ru#NAME" TEXT DEFAULT '' ,"ru#DEF" TEXT DEFAULT '' ,"uk#DEF" TEXT DEFAULT '' ,"sr#NAME" TEXT DEFAULT '' , PRIMARY KEY ("F_ID","ID"));
INSERT INTO techApp_io VALUES('lag','out','Output',2,2,'0',0,0,'Вихід','Выход','','','');
INSERT INTO techApp_io VALUES('lag','in','Input',2,0,'0',0,1,'Вхід','Вход','','','');
INSERT INTO techApp_io VALUES('lag','t_lg','Lag time, seconds',2,0,'10',0,2,'Час затримки, секунд','Время запаздывания, секунд','','','');
INSERT INTO techApp_io VALUES('lag','f_frq','Calculation frequency, Hz',2,0,'100',1,3,'Частота обчислення, Гц','Частота вычисления, Гц','','','');
INSERT INTO techApp_io VALUES('noise','out','Output',2,2,'0',0,0,'Вихід','Выход','','','');
INSERT INTO techApp_io VALUES('noise','off','Main offset',2,0,'1',0,1,'Загальний зсув','Общее смещение','','','');
INSERT INTO techApp_io VALUES('noise','a_g1','Amplitude of the harmonic 1',2,0,'10',0,2,'Амплітуда гармоніки 1','Амплитуда гармоники 1','','','');
INSERT INTO techApp_io VALUES('noise','per_g1','Period of the harmonic part 1, seconds',2,0,'10',0,3,'Період гармоніки 1, секунд','Период гармоники 1, секунд','','','');
INSERT INTO techApp_io VALUES('noise','a_g2','Amplitude of the harmonic 2',2,0,'5',0,4,'Амплітуда гармоніки 2','Амплитуда гармоники 2','','','');
INSERT INTO techApp_io VALUES('noise','per_g2','Period of the harmonic part 2, seconds',2,0,'0.1',0,5,'Період гармоніки 2, секунд','Период гармоники 2, секунд','','','');
INSERT INTO techApp_io VALUES('noise','a_rnd','Amplitude of the random numbers',2,0,'1',0,6,'Амплітуда випадкових значень','Амплитуда случайных значений','','','');
INSERT INTO techApp_io VALUES('noise','f_frq','Calculation frequency, Hz',2,0,'100',1,7,'Частота обчислення, Гц','Частота вычисления, Гц','','','');
INSERT INTO techApp_io VALUES('noise','tmp_g1','Counter of the harmonic 1',2,0,'0',1,8,'Лічильник гармоніки 1','Счётчик гармоники 1','','','');
INSERT INTO techApp_io VALUES('noise','tmp_g2','Counter of the harmonic 2',2,0,'0',1,9,'Лічильник гармоніки 2','Счётчик гармоники 2','','','');
INSERT INTO techApp_io VALUES('ballCrane','pos','Position, %',2,1,'0',0,0,'Положення, %','Положение, %','','','Положај, %');
INSERT INTO techApp_io VALUES('ballCrane','com','Command',3,0,'0',0,1,'Команда','Команда','','','Команда');
INSERT INTO techApp_io VALUES('ballCrane','st_open','State "Opened"',3,1,'0',0,2,'Стан "Відкрито"','Состояние "Открыто"','','','');
INSERT INTO techApp_io VALUES('ballCrane','st_close','State "Closed"',3,1,'1',0,3,'Стан "Закрито"','Состояние "Закрыто"','','','');
INSERT INTO techApp_io VALUES('ballCrane','t_full','Going time, seconds',2,0,'5',0,4,'Час ходу, секунд','Время хода, секунд','','','');
INSERT INTO techApp_io VALUES('ballCrane','t_up','Estrangement time, seconds',2,0,'0.5',0,5,'Час відриву, секунд','Время отрыва, секунд','','','');
INSERT INTO techApp_io VALUES('ballCrane','f_frq','Calculation frequency, Hz',2,0,'100',1,6,'Частота обчислення, Гц','Частота вычисления, Гц','','','');
INSERT INTO techApp_io VALUES('ballCrane','tmp_up','Estrangement counter',2,0,'0',1,7,'Лічильник відриву','Счётчик отрыва','','','');
INSERT INTO techApp_io VALUES('ballCrane','lst_com','Last command',3,0,'0',1,8,'Остання команда','Последняя команда','','','');
INSERT INTO techApp_io VALUES('separator','Fi','Input flow, ton/h',2,1,'0',0,0,'Вхідні витрати, т/год','Входной расход, т/ч','','','');
INSERT INTO techApp_io VALUES('separator','Pi','Input pressure, at',2,0,'1',0,1,'Вхідний тиск, ата','Входное давление, ата','','','');
INSERT INTO techApp_io VALUES('separator','Si','Input cutset, m2',2,0,'0.2',0,2,'Вхідний перетин, м2','Входное сечение, м2','','','');
INSERT INTO techApp_io VALUES('separator','Fo','Output flow, ton/h',2,0,'0',0,3,'Вихідні витрати, т/год','Выходной расход, т/ч','','','');
INSERT INTO techApp_io VALUES('separator','Po','Output pressure, at',2,1,'1',0,4,'Вихідний тиск, ата','Выходное давление, ата','','','');
INSERT INTO techApp_io VALUES('separator','So','Output cutset, m2',2,0,'0.2',0,5,'Вихідний перетин, м2','Выходное сечение, м2','','','');
INSERT INTO techApp_io VALUES('separator','lo','Output length, m',2,0,'10',0,6,'Вихідна довжина, м','Выходная длина, м','','','');
INSERT INTO techApp_io VALUES('separator','Fo_lq','Output liquid flow, ton/h',2,0,'0',0,7,'Вихідні витрати рідини, т/год','Выходной расход жидкости, т/ч','','','');
INSERT INTO techApp_io VALUES('separator','Po_lq','Output liquid pressure, at',2,1,'1',0,8,'Вихідний тиск рідини, ата','Выходное давление жидкости, ата','','','');
INSERT INTO techApp_io VALUES('separator','Llq','Liquid level, %',2,1,'0',0,9,'Рівень рідини, %','Уровень жидкости, %','','','');
INSERT INTO techApp_io VALUES('separator','PercLq','% liquid',2,0,'0.01',0,10,'% рідини','% жидкости','','','');
INSERT INTO techApp_io VALUES('separator','Vap','Device capacity, m3',2,0,'10',0,11,'Об''єм апарату, м3','Объём аппарата, м3','','','');
INSERT INTO techApp_io VALUES('separator','Q0','Norm density of the environment, kg/m3',2,0,'1',0,12,'Нормальна щільність середовища, кг/м3','Нормальная плотность среды, кг/м3','','','');
INSERT INTO techApp_io VALUES('separator','Qlq','Liquid density, kg/m3',2,0,'1000',0,13,'Щільність рідини, кг/м3','Плотность жидкости, кг/м3','','','');
INSERT INTO techApp_io VALUES('separator','f_frq','Calculation frequency, Hz',2,0,'200',1,14,'Частота обчислення, Гц','Частота вычисления, Гц','','','');
INSERT INTO techApp_io VALUES('valve','t_v1','Valve 1 opening time, seconds',2,0,'10',0,10,'Час відкриття клапану 1, секунди','Время хода клапана 1, секунд','','','');
INSERT INTO techApp_io VALUES('valve','t_v2','Valve 2 opening time, seconds',2,0,'5',0,13,'Час відкриття клапану 2, секунди','Время хода клапана 2, секунд','','','');
INSERT INTO techApp_io VALUES('valve','tmp_l1','Lag of the position 1',2,1,'0',1,23,'Затримка положення 1','Задержка положения 1','','','');
INSERT INTO techApp_io VALUES('valve','tmp_l2','Lag of the position 2',2,1,'0',1,24,'Затримка положення 2','Задержка положения 2','','','');
INSERT INTO techApp_io VALUES('valve','Fi','Input flow, ton/h',2,1,'0',0,0,'Вхідні витрати, т/год','Входной расход, т/ч','','','');
INSERT INTO techApp_io VALUES('valve','Pi','Input pressure, at',2,0,'1',0,1,'Вхідний тиск, ата','Входное давление, ата','','','');
INSERT INTO techApp_io VALUES('valve','Ti','Input temperature, K',2,0,'273',0,2,'Вхідна температура, К','Входная температура, К','','','');
INSERT INTO techApp_io VALUES('valve','Fo','Output flow, ton/h',2,0,'0',0,3,'Вихідні витрати, т/год','Выходной расход, т/ч','','','');
INSERT INTO techApp_io VALUES('valve','Po','Output pressure, at',2,1,'1',0,4,'Вихідний тиск, ата','Выходное давление, ата','','','');
INSERT INTO techApp_io VALUES('valve','To','Output temperature, K',2,1,'273',0,5,'Вихідна температура, К','Выходная температура, К','','','');
INSERT INTO techApp_io VALUES('valve','So','Output pipe cutset, m2',2,0,'.2',0,6,'Вихідний перетин труби, м2','Выходное сечение трубы, м2','','','');
INSERT INTO techApp_io VALUES('valve','lo','Output pipe length, m',2,0,'10',0,7,'Вихідна довжина труби, м','Выходная длина трубы, м','','','');
INSERT INTO techApp_io VALUES('valve','S_v1','Valve 1 cutset, m2',2,0,'.1',0,8,'Перетин клапану 1, м2','Сечение клапана 1, м2','','','');
INSERT INTO techApp_io VALUES('valve','S_v2','Valve 2 cutset, m2',2,0,'.05',0,11,'Перетин клапану 2, м2','Сечение клапана 2, м2','','','');
INSERT INTO techApp_io VALUES('valve','l_v1','Valve 1 position, %',2,0,'0',0,9,'Положення клапану 1, %','Положение клапана 1, %','','','');
INSERT INTO techApp_io VALUES('valve','l_v2','Valve 2 position, %',2,0,'0',0,12,'Положення клапану 2, %','Положение клапана 2, %','','','');
INSERT INTO techApp_io VALUES('valve','Q0','Norm density of the environment, kg/m3',2,0,'1',0,14,'Нормальна щільність середовища, кг/м3','Нормальная плотность среды, кг/м3','','','');
INSERT INTO techApp_io VALUES('valve','Kln','Coefficient of the linearity',2,0,'1',0,15,'Коефіцієнт нелінійності','Коэффициент нелинейности','','','');
INSERT INTO techApp_io VALUES('valve','Kpr','Coefficient of the compressibility [0...1]',2,0,'0.95',0,16,'Коефіцієнт стискання середовища [0...1]','Коэффициент сжимаемости среды [0...1]','','','');
INSERT INTO techApp_io VALUES('valve','Ct','Heat capacity of the environment',2,0,'20',0,17,'Теплоємність середовища','Теплоёмкость среды','','','');
INSERT INTO techApp_io VALUES('valve','Riz','Heat resistance of the isolation',2,0,'20',0,18,'Тепловий опір ізоляції','Тепловое сопротивление изоляции','','','');
INSERT INTO techApp_io VALUES('valve','noBack','Back valve',3,0,'0',0,19,'Зворотній клапан','Обратный клапан','','','');
INSERT INTO techApp_io VALUES('valve','Fwind','Air speed',2,0,'1',0,20,'Швидкість повітря','Скорость воздуха','','','');
INSERT INTO techApp_io VALUES('valve','Twind','Air temperature, K',2,0,'273',0,21,'Температура повітря, К','Температура воздуха, К','','','');
INSERT INTO techApp_io VALUES('valve','f_frq','Calculation frequency, Hz',2,0,'200',1,22,'Частота обчислення, Гц','Частота вычисления, Гц','','','');
INSERT INTO techApp_io VALUES('src_press','Kpr','Coefficient of the compressibility [0...1]',2,0,'0.95',0,7,'Коефіцієнт стискання середовища [0...1]','Коэффициент сжимаемости среды [0...1]','','','');
INSERT INTO techApp_io VALUES('lagClean','out','Output',2,2,'0',0,0,'Вихід','Выход','','','');
INSERT INTO techApp_io VALUES('lagClean','in','Input',2,0,'0',0,1,'Вхід','Вход','','','');
INSERT INTO techApp_io VALUES('lagClean','t_lg','Lag time, seconds',2,0,'10',0,2,'Час затримки, секунд','Время запаздывания, секунд','','','');
INSERT INTO techApp_io VALUES('lagClean','f_frq','Calculation frequency, Hz',2,0,'100',1,3,'Частота обчислення, Гц','Частота вычисления, Гц','','','');
INSERT INTO techApp_io VALUES('lagClean','cl1','Link 1',2,0,'0',1,4,'Ланка 1','Звено 1','','','');
INSERT INTO techApp_io VALUES('lagClean','cl2','Link 2',2,0,'0',1,5,'Ланка 2','Звено 2','','','');
INSERT INTO techApp_io VALUES('lagClean','cl3','Link 3',2,0,'0',1,6,'Ланка 3','Звено 3','','','');
INSERT INTO techApp_io VALUES('net','Fi','Input flow, ton/h',2,1,'10',0,0,'Вхідні витрати, т/год','Входной расход, т/ч','','','');
INSERT INTO techApp_io VALUES('net','Pi','Input pressure, at',2,0,'1',0,1,'Вхідний тиск, ата','Входное давление, ата','','','');
INSERT INTO techApp_io VALUES('net','Po','Output pressure setpoint, at',2,0,'1',0,2,'Вихідне завдання тиску, ата','Выходное задание давления, ата','','','');
INSERT INTO techApp_io VALUES('net','So','Output pipe cutset, m2',2,0,'0.1',0,3,'Вихідний перетин труби, м2','Выходное сечение трубы, м2','','','');
INSERT INTO techApp_io VALUES('net','Kpr','Coefficient of the compressibility [0...1]',2,0,'0.95',0,4,'Коефіцієнт стискання середовища [0...1]','Коэффициент сжимаемости среды [0...1]','','','');
INSERT INTO techApp_io VALUES('net','Noise','Input flow''s noise',2,0,'1',0,5,'Шум вхідних витрат','Шум входного расхода','','','');
INSERT INTO techApp_io VALUES('net','Q0','Norm density of the environment, kg/m3',2,0,'1',0,6,'Нормальна щільність середовища, кг/м3','Нормальная плотность среды, кг/м3','','','');
INSERT INTO techApp_io VALUES('net','f_frq','Calculation frequency, Hz',2,0,'200',1,7,'Частота обчислення, Гц','Частота вычисления, Гц','','','');
INSERT INTO techApp_io VALUES('src_press','Pi','Input pressure setpoint, at',2,0,'10',0,0,'Завдання вхідного тиску, ата','Задание входного давления, ата','','','');
INSERT INTO techApp_io VALUES('src_press','Fo','Output flow, ton/h',2,0,'0',0,1,'Вихідні витрати, т/год','Выходной расход, т/ч','','','');
INSERT INTO techApp_io VALUES('src_press','Po','Output pressure, at',2,1,'1',0,2,'Вихідний тиск, ата','Выходное давление, ата','','','');
INSERT INTO techApp_io VALUES('src_press','So','Output pipe cutset, m2',2,0,'0.1',0,3,'Вихідний перетин труби, м2','Выходное сечение трубы, м2','','','');
INSERT INTO techApp_io VALUES('src_press','lo','Output pipe length, m',2,0,'100',0,4,'Вихідна довжина труби, м','Выходная длина трубы, м','','','');
INSERT INTO techApp_io VALUES('src_press','Noise','Input flow''s noise',2,0,'1',0,5,'Шум вхідних витрат','Шум входного расхода','','','');
INSERT INTO techApp_io VALUES('src_press','Q0','Norm density of the environment, kg/m3',2,0,'1',0,6,'Нормальна щільність середовища, кг/м3','Нормальная плотность среды, кг/м3','','','');
INSERT INTO techApp_io VALUES('src_press','f_frq','Calculation frequency, Hz',2,0,'200',1,8,'Частота обчислення, Гц','Частота вычисления, Гц','','','');
INSERT INTO techApp_io VALUES('src_press','Fit','Input flow, lagged',2,1,'0',1,9,'Вхідні витрати, утримані','Входной расход, удержанный','','','');
INSERT INTO techApp_io VALUES('cooler','Fi','Input flow, ton/h',2,1,'0',0,0,'Вхідні витрати, т/год','Входной расход, т/ч','','','');
INSERT INTO techApp_io VALUES('cooler','Pi','Input pressure, at',2,0,'1',0,1,'Вхідний тиск, ата','Входное давление, ата','','','');
INSERT INTO techApp_io VALUES('cooler','Ti','Input temperature, K',2,0,'273',0,2,'Вхідна температура, К','Входная температура, К','','','');
INSERT INTO techApp_io VALUES('cooler','Si','Cooler''s pipes cutset, m2',2,0,'0.05',0,3,'Перетин трубок, м2','Сечение трубок, м2','','','');
INSERT INTO techApp_io VALUES('cooler','li','Full cooler''s pipes length, m',2,0,'10',0,4,'Загальна довжина трубок, м','Общая длина трубок, м','','','');
INSERT INTO techApp_io VALUES('cooler','Fo','Output flow, ton/h',2,0,'0',0,5,'Вихідні витрати, т/год','Выходной расход, т/ч','','','');
INSERT INTO techApp_io VALUES('cooler','Po','Output pressure, at',2,1,'1',0,6,'Вихідний тиск, ата','Выходное давление, ата','','','');
INSERT INTO techApp_io VALUES('cooler','To','Output temperature, K',2,1,'273',0,7,'Вихідна температура, К','Выходная температура, К','','','');
INSERT INTO techApp_io VALUES('cooler','So','Output pipe cutset, m2',2,0,'.2',0,8,'Вихідний перетин труби, м2','Выходное сечение трубы, м2','','','');
INSERT INTO techApp_io VALUES('cooler','lo','Output pipe length, m',2,0,'10',0,9,'Вихідна довжина труби, м','Выходная длина трубы, м','','','');
INSERT INTO techApp_io VALUES('cooler','Tair','Cooling air temperature, К',2,0,'283',0,10,'Температура охолодження повітря, К','Температура охлаждения воздуха, К','','','');
INSERT INTO techApp_io VALUES('cooler','Wc','Cooler performance',2,0,'200',0,11,'Продуктивність холодильника','Производительность холодильника','','','');
INSERT INTO techApp_io VALUES('cooler','Q0','Norm density of the environment, kg/m3',2,0,'1',0,12,'Нормальна щільність середовища, кг/м3','Нормальная плотность среды, кг/м3','','','');
INSERT INTO techApp_io VALUES('cooler','Ct','Heat capacity of the environment',2,0,'100',0,13,'Теплоємність середовища','Теплоёмкость среды','','','');
INSERT INTO techApp_io VALUES('cooler','Rt','Heat resistance',2,0,'1',0,14,'Тепловий опір','Тепловое сопротивление','','','');
INSERT INTO techApp_io VALUES('cooler','f_frq','Calculation frequency, Hz',2,0,'200',1,15,'Частота обчислення, Гц','Частота вычисления, Гц','','','');
INSERT INTO techApp_io VALUES('src_flow','Kpr','Coefficient of the compressibility [0...1]',2,0,'0.95',0,7,'Коефіцієнт стискання середовища [0...1]','Коэффициент сжимаемости среды [0...1]','','','');
INSERT INTO techApp_io VALUES('pipe1_1','Kpr','Coefficient of the compressibility [0...1]',2,0,'0.95',0,7,'Коефіцієнт стискання середовища [0...1]','Коэффициент сжимаемости среды [0...1]','','','');
INSERT INTO techApp_io VALUES('compressor','Fi','Input flow, ton/h',2,1,'0',0,0,'Вхідні витрати, т/год','Входной расход, т/ч','','','');
INSERT INTO techApp_io VALUES('compressor','Pi','Input pressure, at',2,0,'1',0,1,'Вхідний тиск, ата','Входное давление, ата','','','');
INSERT INTO techApp_io VALUES('compressor','Ti','Input temperature, K',2,0,'273',0,2,'Вхідна температура, К','Входная температура, К','','','');
INSERT INTO techApp_io VALUES('compressor','Fo','Output flow, ton/h',2,0,'0',0,3,'Вихідні витрати, т/год','Выходной расход, т/ч','','','');
INSERT INTO techApp_io VALUES('compressor','Po','Output pressure, at',2,1,'1',0,4,'Вихідний тиск, ата','Выходное давление, ата','','','');
INSERT INTO techApp_io VALUES('compressor','To','Output temperature, K',2,1,'273',0,5,'Вихідна температура, К','Выходная температура, К','','','');
INSERT INTO techApp_io VALUES('compressor','So','Output pipe cutset, m2',2,0,'0.2',0,6,'Вихідний перетин труби, м2','Выходное сечение трубы, м2','','','');
INSERT INTO techApp_io VALUES('compressor','lo','Output pipe length, m',2,0,'2',0,7,'Вихідна довжина труби, м','Выходная длина трубы, м','','','');
INSERT INTO techApp_io VALUES('compressor','Kmrg','Surge protect margin coefficient',2,1,'0.1',0,8,'Коефіцієнт запасу за помпажем','Коэффициент запаса по помпажу','','','');
INSERT INTO techApp_io VALUES('compressor','N','Turnovers, 1000 x turn/min',2,0,'0',0,9,'Оберти компресору, тис. об./хвил','Обороты компрессора, тыс. об./мин','','','');
INSERT INTO techApp_io VALUES('compressor','V','Compressor capacity, m3',2,0,'7',0,10,'Об''єм компресору, м3','Объём компрессора, м3','','','');
INSERT INTO techApp_io VALUES('compressor','Kpmp','Surge coefficient, surge point',2,0,'0.066',0,11,'Коефіцієнт помпажу, точка помпажу','Коэффициент помпажа, точка помпажа','','','');
INSERT INTO techApp_io VALUES('compressor','Kslp','Slope coefficient of the surge curve',2,0,'0.08',0,12,'Коефіцієнт нахилу помпажної кривої','Коэффициент наклона помпажной кривой','','','');
INSERT INTO techApp_io VALUES('compressor','Q0','Norm density of the environment, kg/m3',2,0,'1',0,13,'Нормальна щільність середовища, кг/м3','Нормальная плотность среды, кг/м3','','','');
INSERT INTO techApp_io VALUES('compressor','Ct','Heat capacity of the environment',2,0,'100',0,15,'Теплоємність середовища','Теплоёмкость среды','','','');
INSERT INTO techApp_io VALUES('compressor','Riz','Heat resistance of the isolation',2,0,'100',0,16,'Тепловий опір ізоляції','Тепловое сопротивление изоляции','','','');
INSERT INTO techApp_io VALUES('compressor','Fwind','Air speed',2,0,'1',0,17,'Швидкість повітря','Скорость воздуха','','','');
INSERT INTO techApp_io VALUES('compressor','Twind','Air temperature, K',2,0,'273',0,18,'Температура повітря, К','Температура воздуха, К','','','');
INSERT INTO techApp_io VALUES('compressor','f_frq','Calculation frequency, Hz',2,0,'200',1,19,'Частота обчислення, Гц','Частота вычисления, Гц','','','');
INSERT INTO techApp_io VALUES('pipe1_2','Kpr','Coefficient of the compressibility [0...1]',2,0,'0.95',0,11,'Коефіцієнт стискання середовища [0...1]','Коэффициент сжимаемости среды [0...1]','','','');
INSERT INTO techApp_io VALUES('compressor','Fit','Input flow, lagged',2,1,'0',1,20,'Вхідні витрати, утримані','Входной расход, удержанный','','','');
INSERT INTO techApp_io VALUES('src_flow','Fi','Input flow setpoint, ton/h',2,0,'10',0,0,'Завдання вхідних витрат, т/год','Задание входного расхода, т/ч','','','');
INSERT INTO techApp_io VALUES('src_flow','Fo','Output flow, ton/h',2,0,'10',0,1,'Вихідні витрати, т/год','Выходной расход, т/ч','','','');
INSERT INTO techApp_io VALUES('src_flow','Po','Output pressure, at',2,1,'1',0,2,'Вихідний тиск, ата','Выходное давление, ата','','','');
INSERT INTO techApp_io VALUES('src_flow','So','Output pipe cutset, m2',2,0,'0.1',0,3,'Вихідний перетин труби, м2','Выходное сечение трубы, м2','','','');
INSERT INTO techApp_io VALUES('src_flow','lo','Output pipe length, m',2,0,'100',0,4,'Вихідна довжина труби, м','Выходная длина трубы, м','','','');
INSERT INTO techApp_io VALUES('src_flow','Noise','Input flow''s noise',2,0,'1',0,5,'Шум вхідних витрат','Шум входного расхода','','','');
INSERT INTO techApp_io VALUES('src_flow','Q0','Norm density of the environment, kg/m3',2,0,'1',0,6,'Нормальна щільність середовища, кг/м3','Нормальная плотность среды, кг/м3','','','');
INSERT INTO techApp_io VALUES('src_flow','f_frq','Calculation frequency, Hz',2,0,'100',1,8,'Частота обчислення, Гц','Частота вычисления, Гц','','','');
INSERT INTO techApp_io VALUES('pipe1_1','Fi','Input flow, ton/h',2,1,'0',0,0,'Вхідні витрати, т/год','Входной расход, т/ч','','','');
INSERT INTO techApp_io VALUES('pipe1_1','Pi','Input pressure, at',2,0,'1',0,1,'Вхідний тиск, ата','Входное давление, ата','','','');
INSERT INTO techApp_io VALUES('pipe1_1','Fo','Output flow, ton/h',2,0,'0',0,2,'Вихідні витрати, т/год','Выходной расход, т/ч','','','');
INSERT INTO techApp_io VALUES('pipe1_1','Po','Output pressure, at',2,1,'1',0,3,'Вихідний тиск, ата','Выходное давление, ата','','','');
INSERT INTO techApp_io VALUES('pipe1_1','So','Output cutset, m2',2,0,'.2',0,4,'Вихідний перетин, м2','Выходное сечение, м2','','','');
INSERT INTO techApp_io VALUES('pipe1_1','lo','Output length, m',2,0,'10',0,5,'Вихідна довжина, м','Выходная длина, м','','','');
INSERT INTO techApp_io VALUES('pipe1_1','Q0','Norm density of the environment, kg/m3',2,0,'1',0,6,'Нормальна щільність середовища, кг/м3','Нормальная плотность среды, кг/м3','','','');
INSERT INTO techApp_io VALUES('pipe1_1','f_frq','Calculation frequency, Hz',2,0,'200',1,8,'Частота обчислення, Гц','Частота вычисления, Гц','','','');
INSERT INTO techApp_io VALUES('pipe1_1','Pti','Pti',2,1,'1',1,9,'','','','','');
INSERT INTO techApp_io VALUES('pipe1_1','Fto','Fto',2,1,'0',1,10,'','','','','');
INSERT INTO techApp_io VALUES('pipe1_1','Pt1','Pt1',2,1,'1',1,11,'','','','','');
INSERT INTO techApp_io VALUES('pipe1_1','Ft1','Ft1',2,1,'0',1,12,'','','','','');
INSERT INTO techApp_io VALUES('pipe1_2','Fi','Input flow, ton/h',2,1,'0',0,0,'Вхідні витрати, т/год','Входной расход, т/ч','','','');
INSERT INTO techApp_io VALUES('pipe1_2','Pi','Input pressure, at',2,0,'1',0,1,'Вхідний тиск, ата','Входное давление, ата','','','');
INSERT INTO techApp_io VALUES('pipe1_2','Fo1','Output 1 flow, ton/h',2,0,'0',0,2,'Вихідні витрати 1, т/год','Выходной расход 1, т/ч','','','');
INSERT INTO techApp_io VALUES('pipe1_2','Po1','Output 1 pressure, at',2,1,'1',0,3,'Вихідний тиск 1, ата','Выходное давление 1, ата','','','');
INSERT INTO techApp_io VALUES('pipe1_2','So1','Output 1 cutset, m2',2,0,'.2',0,4,'Вихідний перетин труби 1, м2','Выходное сечение 1, м2','','','');
INSERT INTO techApp_io VALUES('pipe1_2','lo1','Output 1 length, m',2,0,'10',0,5,'Вихідна довжина труби 1, м','Выходная длина 1, м','','','');
INSERT INTO techApp_io VALUES('pipe1_2','Fo2','Output 2 flow, ton/h',2,0,'0',0,6,'Вихідні витрати 2, т/год','Выходной расход 2, т/ч','','','');
INSERT INTO techApp_io VALUES('pipe1_2','Po2','Output 2 pressure, at',2,1,'1',0,7,'Вихідний тиск 2, ата','Выходное давление 2, ата','','','');
INSERT INTO techApp_io VALUES('pipe1_2','So2','Output 2 cutset, m2',2,0,'.2',0,8,'Вихідний перетин труби 2, м2','Выходное сечение 2, м2','','','');
INSERT INTO techApp_io VALUES('pipe1_2','lo2','Output 2 length, m',2,0,'10',0,9,'Вихідна довжина труби 2, м','Выходная длина 2, м','','','');
INSERT INTO techApp_io VALUES('pipe1_2','Q0','Norm density of the environment, kg/m3',2,0,'1',0,10,'Нормальна щільність середовища, кг/м3','Нормальная плотность среды, кг/м3','','','');
INSERT INTO techApp_io VALUES('pipe1_2','f_frq','Calculation frequency, Hz',2,0,'100',1,12,'Частота обчислення, Гц','Частота вычисления, Гц','','','');
INSERT INTO techApp_io VALUES('pipe1_2','F1tmp','Temporary flow 1',2,1,'0',1,13,'Тимчасові витрати 1','Временный расход 1','','','');
INSERT INTO techApp_io VALUES('pipe1_2','F2tmp','Temporary flow 2',2,1,'0',1,14,'Тимчасові витрати 2','Временный расход 2','','','');
INSERT INTO techApp_io VALUES('pipe1_2','Pot1','Temporary pressure 1',2,1,'1',1,15,'Тимчасовий тиск 1','Временное давление 1','','','');
INSERT INTO techApp_io VALUES('pipe1_2','Pot2','Temporary pressure 2',2,1,'1',1,16,'Тимчасовий тиск 2','Временное давление 2','','','');
INSERT INTO techApp_io VALUES('pipe1_3','Fi','Input flow, ton/h',2,1,'0',0,0,'Вхідні витрати, т/год','Входной расход, т/ч','','','');
INSERT INTO techApp_io VALUES('pipe1_3','Pi','Input pressure, at',2,0,'1',0,1,'Вхідний тиск, ата','Входное давление, ата','','','');
INSERT INTO techApp_io VALUES('pipe1_3','Fo1','Output 1 flow, ton/h',2,0,'0',0,2,'Вихідні витрати 1, т/год','Выходной расход 1, т/ч','','','');
INSERT INTO techApp_io VALUES('pipe1_3','Po1','Output 1 pressure, at',2,1,'1',0,3,'Вихідний тиск 1, ата','Выходное давление 1, ата','','','');
INSERT INTO techApp_io VALUES('pipe1_3','So1','Output 1 cutset, m2',2,0,'0.2',0,4,'Вихідний перетин труби 1, м2','Выходное сечение 1, м2','','','');
INSERT INTO techApp_io VALUES('pipe1_3','lo1','Output 1 length, m',2,0,'10',0,5,'Вихідна довжина труби 1, м','Выходная длина 1, м','','','');
INSERT INTO techApp_io VALUES('pipe1_3','Fo2','Output 2 flow, ton/h',2,0,'0',0,6,'Вихідні витрати 2, т/год','Выходной расход 2, т/ч','','','');
INSERT INTO techApp_io VALUES('pipe1_3','Po2','Output 2 pressure, at',2,1,'1',0,7,'Вихідний тиск 2, ата','Выходное давление 2, ата','','','');
INSERT INTO techApp_io VALUES('pipe1_3','So2','Output 2 cutset, m2',2,0,'0.2',0,8,'Вихідний перетин труби 2, м2','Выходное сечение 2, м2','','','');
INSERT INTO techApp_io VALUES('pipe1_3','lo2','Output 2 length, m',2,0,'10',0,9,'Вихідна довжина труби 2, м','Выходная длина 2, м','','','');
INSERT INTO techApp_io VALUES('pipe1_3','Fo3','Output 3 flow, ton/h',2,0,'0',0,10,'Вихідні витрати 3, т/год','Выходной расход 3, т/ч','','','');
INSERT INTO techApp_io VALUES('pipe1_3','Po3','Output 3 pressure, at',2,1,'1',0,11,'Вихідний тиск 3, ата','Выходное давление 3, ата','','','');
INSERT INTO techApp_io VALUES('pipe1_3','So3','Output 3 cutset, m2',2,0,'0.2',0,12,'Вихідний перетин труби 3, м2','Выходное сечение 3, м2','','','');
INSERT INTO techApp_io VALUES('pipe1_3','lo3','Output 3 length, m',2,0,'10',0,13,'Вихідна довжина труби 3, м','Выходная длина 3, м','','','');
INSERT INTO techApp_io VALUES('pipe1_3','Q0','Norm density of the environment, kg/m3',2,0,'1',0,14,'Нормальна щільність середовища, кг/м3','Нормальная плотность среды, кг/м3','','','');
INSERT INTO techApp_io VALUES('pipe1_3','f_frq','Calculation frequency, Hz',2,0,'200',1,16,'Частота обчислення, Гц','Частота вычисления, Гц','','','');
INSERT INTO techApp_io VALUES('pipe1_3','F1tmp','Temporary flow 1',2,1,'0',0,17,'Тимчасові витрати 1','Временный расход 1','','','');
INSERT INTO techApp_io VALUES('pipe1_3','F2tmp','Temporary flow 2',2,1,'0',0,18,'Тимчасові витрати 2','Временный расход 2','','','');
INSERT INTO techApp_io VALUES('pipe1_3','F3tmp','Temporary flow 3',2,1,'0',0,19,'Тимчасові витрати 3','Временный расход 3','','','');
INSERT INTO techApp_io VALUES('pipe1_3','Pot1','Temporary pressure 1',2,1,'1',1,20,'Тимчасовий тиск 1','Временное давление 1','','','');
INSERT INTO techApp_io VALUES('pipe1_3','Pot2','Temporary pressure 2',2,1,'1',1,21,'Тимчасовий тиск 2','Временное давление 2','','','');
INSERT INTO techApp_io VALUES('pipe1_3','Pot3','Temporary pressure 3',2,1,'1',1,22,'Тимчасовий тиск 3','Временное давление 3','','','');
INSERT INTO techApp_io VALUES('pipe1_4','Fi','Input flow, ton/h',2,1,'0',0,0,'Вхідні витрати, т/год','Входной расход, т/ч','','','');
INSERT INTO techApp_io VALUES('pipe1_4','Pi','Input pressure, at',2,0,'1',0,1,'Вхідний тиск, ата','Входное давление, ата','','','');
INSERT INTO techApp_io VALUES('pipe1_4','Fo1','Output 1 flow, ton/h',2,0,'0',0,2,'Вихідні витрати 1, т/год','Выходной расход 1, т/ч','','','');
INSERT INTO techApp_io VALUES('pipe1_4','Po1','Output 1 pressure, at',2,1,'1',0,3,'Вихідний тиск 1, ата','Выходное давление 1, ата','','','');
INSERT INTO techApp_io VALUES('pipe1_4','So1','Output 1 cutset, m2',2,0,'0.2',0,4,'Вихідний перетин труби 1, м2','Выходное сечение 1, м2','','','');
INSERT INTO techApp_io VALUES('pipe1_4','lo1','Output 1 length, m',2,0,'10',0,5,'Вихідна довжина труби 1, м','Выходная длина 1, м','','','');
INSERT INTO techApp_io VALUES('pipe1_4','Fo2','Output 2 flow, ton/h',2,0,'0',0,6,'Вихідні витрати 2, т/год','Выходной расход 2, т/ч','','','');
INSERT INTO techApp_io VALUES('pipe1_4','Po2','Output 2 pressure, at',2,1,'1',0,7,'Вихідний тиск 2, ата','Выходное давление 2, ата','','','');
INSERT INTO techApp_io VALUES('pipe1_4','So2','Output 2 cutset, m2',2,0,'0.2',0,8,'Вихідний перетин труби 2, м2','Выходное сечение 2, м2','','','');
INSERT INTO techApp_io VALUES('pipe1_4','lo2','Output 2 length, m',2,0,'10',0,9,'Вихідна довжина труби 2, м','Выходная длина 2, м','','','');
INSERT INTO techApp_io VALUES('pipe1_4','Fo3','Output 3 flow, ton/h',2,0,'0',0,10,'Вихідні витрати 3, т/год','Выходной расход 3, т/ч','','','');
INSERT INTO techApp_io VALUES('pipe1_4','Po3','Output 3 pressure, at',2,1,'1',0,11,'Вихідний тиск 3, ата','Выходное давление 3, ата','','','');
INSERT INTO techApp_io VALUES('pipe1_4','So3','Output 3 cutset, m2',2,0,'0.2',0,12,'Вихідний перетин труби 3, м2','Выходное сечение 3, м2','','','');
INSERT INTO techApp_io VALUES('pipe1_4','lo3','Output 3 length, m',2,0,'10',0,13,'Вихідна довжина труби 3, м','Выходная длина 3, м','','','');
INSERT INTO techApp_io VALUES('pipe1_4','Fo4','Output 4 flow, ton/h',2,0,'0',0,14,'Вихідні витрати 4, т/год','Выходной расход 4, т/ч','','','');
INSERT INTO techApp_io VALUES('pipe1_4','Po4','Output 4 pressure, at',2,1,'1',0,15,'Вихідний тиск 4, ата','Выходное давление 4, ата','','','');
INSERT INTO techApp_io VALUES('pipe1_4','So4','Output 4 cutset, m2',2,0,'0.2',0,16,'Вихідний перетин труби 4, м2','Выходное сечение 4, м2','','','');
INSERT INTO techApp_io VALUES('pipe1_4','lo4','Output 4 length, m',2,0,'10',0,17,'Вихідна довжина труби 4, м','Выходная длина 4, м','','','');
INSERT INTO techApp_io VALUES('pipe1_4','Q0','Norm density of the environment, kg/m3',2,0,'1',0,18,'Нормальна щільність середовища, кг/м3','Нормальная плотность среды, кг/м3','','','');
INSERT INTO techApp_io VALUES('pipe1_4','f_frq','Calculation frequency, Hz',2,0,'200',1,20,'Частота обчислення, Гц','Частота вычисления, Гц','','','');
INSERT INTO techApp_io VALUES('pipe1_4','F1tmp','Temporary flow 1',2,1,'0',1,21,'Тимчасові витрати 1','Временный расход 1','','','');
INSERT INTO techApp_io VALUES('pipe1_4','F2tmp','Temporary flow 2',2,1,'0',1,22,'Тимчасові витрати 2','Временный расход 2','','','');
INSERT INTO techApp_io VALUES('pipe1_4','F3tmp','Temporary flow 3',2,1,'0',1,23,'Тимчасові витрати 3','Временный расход 3','','','');
INSERT INTO techApp_io VALUES('pipe1_4','F4tmp','Temporary flow 4',2,1,'0',1,24,'Тимчасові витрати 4','Временный расход 4','','','');
INSERT INTO techApp_io VALUES('pipe1_4','Pot1','Temporary pressure 1',2,1,'1',1,25,'Тимчасовий тиск 1','Временное давление 1','','','');
INSERT INTO techApp_io VALUES('pipe1_4','Pot2','Temporary pressure 2',2,1,'1',1,26,'Тимчасовий тиск 2','Временное давление 2','','','');
INSERT INTO techApp_io VALUES('pipe1_4','Pot3','Temporary pressure 3',2,1,'1',1,27,'Тимчасовий тиск 3','Временное давление 3','','','');
INSERT INTO techApp_io VALUES('pipe1_4','Pot4','Temporary pressure 4',2,1,'1',1,28,'Тимчасовий тиск 4','Временное давление 4','','','');
INSERT INTO techApp_io VALUES('valveMech','pos','Position, %',2,1,'0',0,0,'Положення, %','Положение, %','','','Положај, %');
INSERT INTO techApp_io VALUES('valveMech','pos_sensor','Position by the sensor, %',2,1,'0',0,1,'Положення за давачем, %','Положение по датчику, %','','','');
INSERT INTO techApp_io VALUES('valveMech','com','Command',2,0,'0',0,2,'Команда','Команда','','','Команда');
INSERT INTO techApp_io VALUES('valveMech','st_open','State "Opened"',3,1,'0',0,3,'Стан "Відкрито"','Состояние "Открыто"','','','');
INSERT INTO techApp_io VALUES('valveMech','st_close','State "Closed"',3,1,'1',0,4,'Стан "Закрито"','Состояние "Закрыто"','','','');
INSERT INTO techApp_io VALUES('valveMech','t_full','Going time, seconds',2,0,'3',0,5,'Час ходу, секунд','Время хода, секунд','','','');
INSERT INTO techApp_io VALUES('valveMech','t_up','Estrangement time, seconds',2,0,'1',0,6,'Час відриву, секунд','Время отрыва, секунд','','','');
INSERT INTO techApp_io VALUES('valveMech','t_sensor','Sensor lag time, seconds',2,0,'1',0,7,'Час затримки сенсору, секунд','Время задержки сенсора, секунд','','','');
INSERT INTO techApp_io VALUES('valveMech','f_frq','Calculation frequency, Hz',2,0,'100',1,8,'Частота обчислення, Гц','Частота вычисления, Гц','','','');
INSERT INTO techApp_io VALUES('valveMech','tmp_up','Estrangement counter',2,1,'0',0,9,'Лічильник відриву','Счётчик отрыва','','','');
INSERT INTO techApp_io VALUES('valveMech','lst_com','Last command',2,1,'0',0,10,'Остання команда','Последняя команда','','','');
INSERT INTO techApp_io VALUES('diaphragm','Fi','Input flow, ton/h',2,1,'0',0,0,'Вхідні витрати, т/год','Входной расход, т/ч','','','');
INSERT INTO techApp_io VALUES('diaphragm','Pi','Input pressure, at',2,0,'1',0,1,'Вхідний тиск, ата','Входное давление, ата','','','');
INSERT INTO techApp_io VALUES('diaphragm','Fo','Output flow, ton/h',2,0,'0',0,2,'Вихідні витрати, т/год','Выходной расход, т/ч','','','');
INSERT INTO techApp_io VALUES('diaphragm','Po','Output pressure, at',2,1,'1',0,3,'Вихідний тиск, ата','Выходное давление, ата','','','');
INSERT INTO techApp_io VALUES('diaphragm','dP','Pressure difference, kPa',2,1,'0',0,4,'Перепад тиску, кПа','Перепад давления, кПа','','','');
INSERT INTO techApp_io VALUES('diaphragm','Sdf','Diaphragm cutset, m2',2,0,'0.1',0,5,'Перетин діафрагми, м2','Сечение диафрагмы, м2','','','');
INSERT INTO techApp_io VALUES('diaphragm','So','Output pipe cutset, m2',2,0,'0.2',0,6,'Вихідний перетин труби, м2','Выходное сечение трубы, м2','','','');
INSERT INTO techApp_io VALUES('diaphragm','lo','Output pipe length, m',2,0,'10',0,7,'Вихідна довжина труби, м','Выходная длина трубы, м','','','');
INSERT INTO techApp_io VALUES('diaphragm','Q0','Norm density of the environment, kg/m3',2,0,'1',0,8,'Нормальна щільність середовища, кг/м3','Нормальная плотность среды, кг/м3','','','');
INSERT INTO techApp_io VALUES('diaphragm','f_frq','Calculation frequency, Hz',2,0,'100',1,10,'Частота обчислення, Гц','Частота вычисления, Гц','','','');
INSERT INTO techApp_io VALUES('pipe3_1','Kpr','Coefficient of the compressibility [0...1]',2,0,'0.95',0,18,'Коефіцієнт стискання середовища [0...1]','Коэффициент сжимаемости среды [0...1]','','','');
INSERT INTO techApp_io VALUES('pipe3_1','Fi1','Input 1 flow, ton/h',2,1,'0',0,0,'Вхідні витрати 1, т/год','Входной расход 1, т/ч','','','');
INSERT INTO techApp_io VALUES('pipe3_1','Pi1','Input 1 pressure, at',2,0,'1',0,1,'Вхідний тиск 1, ата','Входное давление 1, ата','','','');
INSERT INTO techApp_io VALUES('pipe3_1','Ti1','Input 1 temperature, K',2,0,'273',0,2,'Вхідна температура 1, К','Входная температура 1, К','','','');
INSERT INTO techApp_io VALUES('pipe3_1','Si1','Input 1 cutset, m2',2,0,'0.2',0,3,'Вхідний перетин 1, м2','Входное сечение 1, м2','','','');
INSERT INTO techApp_io VALUES('pipe3_1','Fi2','Input 2 flow, ton/h',2,1,'0',0,4,'Вхідні витрати 2, т/год','Входной расход 2, т/ч','','','');
INSERT INTO techApp_io VALUES('pipe3_1','Pi2','Input 2 pressure, at',2,0,'1',0,5,'Вхідний тиск 2, ата','Входное давление 2, ата','','','');
INSERT INTO techApp_io VALUES('pipe3_1','Ti2','Input 2 temperature, K',2,0,'273',0,6,'Вхідна температура 2, К','Входная температура 2, К','','','');
INSERT INTO techApp_io VALUES('pipe3_1','Si2','Input 2 cutset, m2',2,0,'0.2',0,7,'Вхідний перетин 2, м2','Входное сечение 2, м2','','','');
INSERT INTO techApp_io VALUES('pipe3_1','Fi3','Input 3 flow, ton/h',2,1,'0',0,8,'Вхідні витрати 3, т/год','Входной расход 3, т/ч','','','');
INSERT INTO techApp_io VALUES('pipe3_1','Pi3','Input 3 pressure, at',2,0,'1',0,9,'Вхідний тиск 3, ата','Входное давление 3, ата','','','');
INSERT INTO techApp_io VALUES('pipe3_1','Ti3','Input 3 temperature, K',2,0,'273',0,10,'Вхідна температура 3, К','Входная температура 3, К','','','');
INSERT INTO techApp_io VALUES('pipe3_1','Si3','Input 3 cutset, m2',2,0,'0.2',0,11,'Вхідний перетин 3, м2','Входное сечение 3, м2','','','');
INSERT INTO techApp_io VALUES('pipe3_1','Fo','Output flow, ton/h',2,0,'0',0,12,'Вихідні витрати, т/год','Выходной расход, т/ч','','','');
INSERT INTO techApp_io VALUES('pipe3_1','Po','Output pressure, at',2,1,'1',0,13,'Вихідний тиск, ата','Выходное давление, ата','','','');
INSERT INTO techApp_io VALUES('pipe3_1','To','Output temperature, K',2,1,'273',0,14,'Вихідна температура, К','Выходная температура, К','','','');
INSERT INTO techApp_io VALUES('pipe3_1','So','Output cutset, m2',2,0,'.2',0,15,'Вихідний перетин, м2','Выходное сечение, м2','','','');
INSERT INTO techApp_io VALUES('pipe3_1','lo','Output length, m',2,0,'10',0,16,'Вихідна довжина, м','Выходная длина, м','','','');
INSERT INTO techApp_io VALUES('pipe3_1','Q0','Norm density of the environment, kg/m3',2,0,'1',0,17,'Нормальна щільність середовища, кг/м3','Нормальная плотность среды, кг/м3','','','');
INSERT INTO techApp_io VALUES('pipe3_1','Ct','Heat capacity of the environment',2,0,'20',0,19,'Теплоємність середовища','Теплоёмкость среды','','','');
INSERT INTO techApp_io VALUES('pipe3_1','Riz','Heat resistance of the isolation',2,0,'20',0,20,'Тепловий опір ізоляції','Тепловое сопротивление изоляции','','','');
INSERT INTO techApp_io VALUES('pipe3_1','Fwind','Air speed',2,0,'1',0,21,'Швидкість повітря','Скорость воздуха','','','');
INSERT INTO techApp_io VALUES('pipe3_1','Twind','Air temperature, К',2,0,'273',0,22,'Температура повітря, К','Температура воздуха, К','','','');
INSERT INTO techApp_io VALUES('pipe3_1','f_frq','Calculation frequency, Hz',2,0,'100',1,23,'Частота обчислення, Гц','Частота вычисления, Гц','','','');
INSERT INTO techApp_io VALUES('compressor','Kpr','Coefficient of the compressibility [0...1]',2,0,'0.95',0,14,'Коефіцієнт стискання середовища [0...1]','Коэффициент сжимаемости среды [0...1]','','','');
INSERT INTO techApp_io VALUES('pipeBase','Fi','Input flow, ton/h',2,1,'0',0,0,'Вхідні витрати, т/год','Входной расход, т/ч','','','');
INSERT INTO techApp_io VALUES('pipeBase','Pi','Input pressure, at',2,0,'1',0,1,'Вхідний тиск, ата','Входное давление, ата','','','');
INSERT INTO techApp_io VALUES('pipeBase','Ti','Input temperature, K',2,0,'293',0,2,'Вхідна температура, К','Входная температура, К','','','');
INSERT INTO techApp_io VALUES('pipeBase','Si','Input cutset, m2',2,0,'.2',0,3,'Вхідний перетин, м2','Входное сечение, м2','','','');
INSERT INTO techApp_io VALUES('pipeBase','Fo','Output flow, ton/h',2,0,'0',0,4,'Вихідні витрати, т/год','Выходной расход, т/ч','','','');
INSERT INTO techApp_io VALUES('pipeBase','Po','Output pressure, at',2,1,'1',0,5,'Вихідний тиск, ата','Выходное давление, ата','','','');
INSERT INTO techApp_io VALUES('pipeBase','To','Output temperature, K',2,1,'293',0,6,'Вихідна температура, К','Выходная температура, К','','','');
INSERT INTO techApp_io VALUES('pipeBase','So','Output cutset, m2',2,0,'.2',0,7,'Вихідний перетин, м2','Выходное сечение, м2','','','');
INSERT INTO techApp_io VALUES('pipeBase','lo','Output length, m',2,0,'10',0,8,'Вихідна довжина, м','Выходная длина, м','','','');
INSERT INTO techApp_io VALUES('pipeBase','Q0','Norm density of the environment, kg/m3',2,0,'1',0,9,'Нормальна щільність середовища, кг/м3','Нормальная плотность среды, кг/м3','','','');
INSERT INTO techApp_io VALUES('pipeBase','Kpr','Coefficient of the compressibility [0...1]',2,0,'0.98',0,10,'Коефіцієнт стискання середовища [0...1]','Коэффициент сжимаемости среды [0...1]','','','');
INSERT INTO techApp_io VALUES('pipeBase','Ktr','Coefficient of friction',2,0,'0.01',0,11,'Коефіцієнт тертя','Коэффициент трения','','','');
INSERT INTO techApp_io VALUES('pipeBase','f_frq','Calculation frequency, Hz',2,0,'100',0,12,'Частота обчислення, Гц','Частота вычисления, Гц','','','');
INSERT INTO techApp_io VALUES('heatExch','Fi1','Input 1 flow, ton/h',2,0,'20',0,0,'Вхідні витрати 1, т/год','Входной расход 1, т/ч','','','');
INSERT INTO techApp_io VALUES('heatExch','Pi1','Input 1 pressure, at',2,0,'1',0,1,'Вхідний тиск 1, ата','Входное давление 1, ата','','','');
INSERT INTO techApp_io VALUES('heatExch','Ti1','Input 1 temperature, K',2,0,'20',0,2,'Вхідна температура 1, К','Входная температура 1, К','','','');
INSERT INTO techApp_io VALUES('heatExch','Si1','Input 1 cutset, m2',2,0,'1',0,3,'Вхідний перетин 1, м2','Входное сечение 1, м2','','','');
INSERT INTO techApp_io VALUES('heatExch','li1','Input 1 length, m',2,0,'10',0,4,'Вхідна довжина 1, м','Входная длина 1, м','','','');
INSERT INTO techApp_io VALUES('heatExch','Q0i1','Input 1 norm density, kg/m3',2,0,'1',0,5,'Вхідна нормальна щільність 1, кг/м3','Входная нормальная плотность 1, кг/м3','','','');
INSERT INTO techApp_io VALUES('heatExch','Kpr1','Input 1 coefficient of the compressibility [0...1]',2,0,'0.9',0,6,'Вхідний коефіцієнт стискання середовища 1 [0...1]','Входной коэффициент сжимаемости среды 1 [0...1]','','','');
INSERT INTO techApp_io VALUES('heatExch','Ci1','Input 1 heat capacity',2,0,'1',0,7,'Вхідна теплоємність 1','Входная теплоёмкость 1','','','');
INSERT INTO techApp_io VALUES('heatExch','Fi2','Input 2 flow, ton/h',2,0,'20',0,8,'Вхідні витрати 2, т/год','Входной расход 2, т/ч','','','');
INSERT INTO techApp_io VALUES('heatExch','Pi2','Input 2 pressure, at',2,0,'1',0,9,'Вхідний тиск 2, ата','Входное давление 2, ата','','','');
INSERT INTO techApp_io VALUES('heatExch','Ti2','Input 2 temperature, K',2,0,'40',0,10,'Вхідна температура 2, К','Входная температура 2, К','','','');
INSERT INTO techApp_io VALUES('heatExch','Si2','Input 2 cutset, m2',2,0,'1',0,11,'Вхідний перетин 2, м2','Входное сечение 2, м2','','','');
INSERT INTO techApp_io VALUES('heatExch','li2','Input 2 length, m',2,0,'10',0,12,'Вхідна довжина 2, м','Входная длина 2, м','','','');
INSERT INTO techApp_io VALUES('heatExch','Q0i2','Input 2 norm density, kg/m3',2,0,'1',0,13,'Вхідна нормальна щільність 2, кг/м3','Входная нормальная плотность 2, кг/м3','','','');
INSERT INTO techApp_io VALUES('heatExch','Kpr2','Input 2 coefficient of the compressibility [0...1]',2,0,'0.9',0,14,'Вхідний коефіцієнт стискання середовища 2 [0...1]','Входной коэффициент сжимаемости среды 2 [0...1]','','','');
INSERT INTO techApp_io VALUES('heatExch','Ci2','Input 2 heat capacity',2,0,'1',0,15,'Вхідна теплоємність 2','Входная теплоёмкость 2','','','');
INSERT INTO techApp_io VALUES('heatExch','ki','Heat transfer coefficient',2,0,'0.9',0,16,'Коефіцієнт тепловіддачі','Коэффициент теплоотдачи ','','','');
INSERT INTO techApp_io VALUES('heatExch','Fo1','Output 1 flow, ton/h',2,0,'0',0,17,'Вихідні витрати 1, т/год','Выходной расход 1, т/ч','','','');
INSERT INTO techApp_io VALUES('heatExch','Po1','Output 1 pressure, at',2,1,'1',0,18,'Вихідний тиск 1, ата','Выходное давление 1, ата','','','');
INSERT INTO techApp_io VALUES('heatExch','To1','Output 1 temperature, K',2,1,'273',0,19,'Вихідна температура 1, К','Выходная температура 1, K','','','');
INSERT INTO techApp_io VALUES('heatExch','So1','Output 1 cutset, m2',2,1,'1',0,20,'Вихідний перетин труби 1, м2','Выходное сечение 1, м2','','','');
INSERT INTO techApp_io VALUES('heatExch','lo1','Output 1 length, m',2,1,'10',0,21,'Вихідна довжина труби 1, м','Выходная длина 1, м','','','');
INSERT INTO techApp_io VALUES('heatExch','Fo2','Output 2 flow, ton/h',2,0,'0',0,22,'Вихідні витрати 2, т/год','Выходной расход 2, т/ч','','','');
INSERT INTO techApp_io VALUES('heatExch','Po2','Output 2 pressure, at',2,1,'1',0,23,'Вихідний тиск 2, ата','Выходное давление 2, ата','','','');
INSERT INTO techApp_io VALUES('heatExch','To2','Output 2 temperature, K',2,1,'273',0,24,'Вихідна температура 2, К','Выходная температура 2, K','','','');
INSERT INTO techApp_io VALUES('heatExch','So2','Output 2 cutset, m2',2,1,'1',0,25,'Вихідний перетин труби 2, м2','Выходное сечение 2, м2','','','');
INSERT INTO techApp_io VALUES('heatExch','lo2','Output 2 length, m',2,1,'10',0,26,'Вихідна довжина труби 2, м','Выходная длина 2, м','','','');
INSERT INTO techApp_io VALUES('heatExch','f_frq','Calculation frequency, Hz',2,0,'200',0,27,'Частота обчислення, Гц','Частота вычисления, Гц','','','');
INSERT INTO techApp_io VALUES('boilerBarrel','Fi1','Input water flow, ton/h',2,1,'22',0,0,'Вхідні витрати води, т/год','Входной расход воды, т/ч','','','');
INSERT INTO techApp_io VALUES('boilerBarrel','Pi1','Input water pressure, at',2,0,'43',0,1,'Вхідний тиск води, ата','Входное давление воды, ата','','','');
INSERT INTO techApp_io VALUES('boilerBarrel','Ti1','Input water temperature, K',2,0,'523',0,2,'Вхідна температура води, К','Входная температура воды, K','','','');
INSERT INTO techApp_io VALUES('boilerBarrel','Si1','Input water pipes cutset, m2',2,0,'0.6',0,3,'Вхідний перетин труб з водою, м2','Входное сечение труб с водой, м2','','','');
INSERT INTO techApp_io VALUES('boilerBarrel','Fi2','Input smoke gas flow, ton/h',2,1,'',0,4,'Вхідні витрати димових газів, т/год','Входной расход дымовых газов, т/ч','','','');
INSERT INTO techApp_io VALUES('boilerBarrel','Pi2','Input smoke gas pressure, at',2,0,'1.3',0,5,'Вхідний тиск димових газів, ата','Входное давление дымовых газов, ата','','','');
INSERT INTO techApp_io VALUES('boilerBarrel','Ti2','Input smoke gas temperature, K',2,0,'1700',0,6,'Вхідна температура димових газів, К','Входная температура дымовых газов, K','','','');
INSERT INTO techApp_io VALUES('boilerBarrel','Si2','Input smoke gas pipes cutset, m2',2,0,'10',0,7,'Вхідний перетин труби димових газів, м2','Входное сечение трубы дымовых газов, м2','','','');
INSERT INTO techApp_io VALUES('boilerBarrel','Vi1','Barrel volume, m3',2,0,'3',0,8,'Об''єм барабану, м3','Объем барабана, м3','','','');
INSERT INTO techApp_io VALUES('boilerBarrel','Lo','Barrel level, %',2,1,'10',0,9,'Рівень у барабані, %','Уровень в барабане, %','','','');
INSERT INTO techApp_io VALUES('boilerBarrel','S','Heating surface, m2',2,0,'15',0,10,'Поверхня нагріву, м2','Поверхность нагрева, м2','','','');
INSERT INTO techApp_io VALUES('boilerBarrel','k','Heat transfer coefficient',2,0,'0.8',0,11,'Коефіцієнт тепловіддачі','Коэффициент теплоотдачи ','','','');
INSERT INTO techApp_io VALUES('boilerBarrel','Fo','Output steam flow, ton/h',2,0,'20',0,12,'Вихідні витрати пару, т/год','Выходной расход пара, т/ч','','','');
INSERT INTO techApp_io VALUES('boilerBarrel','Po1','Output steam pressure, at',2,1,'41.68',0,13,'Вихідний тиск пари, ата','Выходное давление пара, ата','','','');
INSERT INTO techApp_io VALUES('boilerBarrel','To1','Output steam temperature, K',2,1,'10',0,14,'Вихідна температура пари, К','Выходная температура пара, K','','','');
INSERT INTO techApp_io VALUES('boilerBarrel','So1','Output steam pipe cutset, m2',2,0,'0.5',0,15,'Вихідний перетин труби пару, м2','Выходное сечение трубы по пару, м2','','','');
INSERT INTO techApp_io VALUES('boilerBarrel','lo1','Output steam pipe length, m',2,0,'5',0,16,'Вихідна довжина труби пару, м','Выходная длина трубы пара, м','','','');
INSERT INTO techApp_io VALUES('boilerBarrel','Fo2','Output smoke gas flow, ton/h',2,0,'180',0,17,'Вихідні витрати димових газів, т/год','Выходной расход дымовых газов, т/ч','','','');
INSERT INTO techApp_io VALUES('boilerBarrel','Po2','Output smoke gas pressure, at',2,1,'1',0,18,'Вихідний тиск димових газів, ата','Выходное давление дымовых газов, ата','','','');
INSERT INTO techApp_io VALUES('boilerBarrel','To2','Output smoke gas temperature, K',2,0,'0',0,19,'Вихідна температура димових газів, К','Выходная температура дымовых газов, K','','','');
INSERT INTO techApp_io VALUES('boilerBarrel','Fstm','Inner barrel steam flow, ton/h',2,1,'0',0,20,'Витрати пару у барабані, т/год','Расход пара в барабане, т/ч','','','');
INSERT INTO techApp_io VALUES('boilerBarrel','Tv','Inner water temperature, K',2,1,'0',0,21,'Температура води у барабані, K','Температура воды в барабане, K','','','');
INSERT INTO techApp_io VALUES('boilerBarrel','f_frq','Calculation frequency, Hz',2,0,'200',0,22,'Частота обчислення, Гц','Частота вычисления, Гц','','','');
INSERT INTO techApp_io VALUES('boilerBurner','Fi1','Input blast furnace gas flow, ton/h',2,1,'',0,0,'Вхідні витрати доменного газу, т/год','Входной расход доменного газа, т/ч','','','');
INSERT INTO techApp_io VALUES('boilerBurner','Pi1','Input blast furnace gas pressure, at',2,0,'',0,1,'Вхідний тиск доменного газу, ата','Входное давление доменного газа, ата','','','');
INSERT INTO techApp_io VALUES('boilerBurner','Ti1','Input blast furnace gas temperature, K',2,0,'40',0,2,'Вхідна температура димових газів, К','Входная температура доменного газа, K','','','');
INSERT INTO techApp_io VALUES('boilerBurner','Si1','Input blast furnace gas pipe cutset, m2',2,0,'',0,3,'Вхідний перетин труби димових газів, м2','Входное сечение трубы доменного газа, м2','','','');
INSERT INTO techApp_io VALUES('boilerBurner','Fi2','Input natural gas flow, ton/h',2,1,'',0,4,'Вхідні витрати природного газу, т/год','Входной расход природного газа, т/ч','','','');
INSERT INTO techApp_io VALUES('boilerBurner','Pi2','Input natural gas pressure, at',2,0,'',0,5,'Вхідний тиск природного газу, ата','Входное давление природного газа, ата','','','');
INSERT INTO techApp_io VALUES('boilerBurner','Ti2','Input natural gas temperature, K',2,0,'20',0,6,'Вхідна температура природного газу, К','Входная температура природного газа, K','','','');
INSERT INTO techApp_io VALUES('boilerBurner','Si2','Input natural gas pipe cutset, m2',2,0,'',0,7,'Вхідний перетин труби природного газу, м2','Входное сечение трубы природного газа, м2','','','');
INSERT INTO techApp_io VALUES('boilerBurner','Fi3','Input coke oven gas flow, ton/h',2,1,'',0,8,'Вхідні витрати коксового газу, т/год','Входной расход коксового газа, т/ч','','','');
INSERT INTO techApp_io VALUES('boilerBurner','Pi3','Input coke oven gas pressure, at',2,0,'',0,9,'Вхідний тиск коксового газу, ата','Входное давление коксового газа, ата','','','');
INSERT INTO techApp_io VALUES('boilerBurner','Ti3','Input coke oven gas temperature, K',2,0,'0',0,10,'Вхідна температура коксового газу, К','Входная температура коксового газа, K','','','');
INSERT INTO techApp_io VALUES('boilerBurner','Si3','Input coke oven gas pipe cutset, m2',2,0,'',0,11,'Вхідний перетин труби коксового газу, м2','Входное сечение трубы коксового газа, м2','','','');
INSERT INTO techApp_io VALUES('boilerBurner','Fi4','Input air flow, ton/h',2,1,'',0,12,'Вхідні витрати повітря, т/год','Входной расход воздуха, т/ч','','','');
INSERT INTO techApp_io VALUES('boilerBurner','Pi4','Input air pressure, at',2,0,'',0,13,'Вхідний тиск повітря, ата','Входное давление воздуха, ата','','','');
INSERT INTO techApp_io VALUES('boilerBurner','Ti4','Input air temperature, K',2,0,'20',0,14,'Вхідна температура повітря, К','Входная температура воздуха, K','','','');
INSERT INTO techApp_io VALUES('boilerBurner','Si4','Input air pipe cutset, m2',2,0,'',0,15,'Вхідний перетин труби повітря, м2','Входное сечение трубы воздуха, м2','','','');
INSERT INTO techApp_io VALUES('boilerBurner','Fo','Output smoke gas flow, ton/h',2,0,'',0,16,'Вихідні витрати димових газів, т/год','Выходной расход дымовых газов, т/ч','','','');
INSERT INTO techApp_io VALUES('boilerBurner','Po','Output smoke gas pressure, at',2,1,'',0,17,'Вихідний тиск димових газів, ата','Выходное давление дымовых газов, ата','','','');
INSERT INTO techApp_io VALUES('boilerBurner','To','Output smoke gas temperature, K',2,1,'',0,18,'Вихідна температура димових газів, К','Выходная температура дымовых газов, K','','','');
INSERT INTO techApp_io VALUES('boilerBurner','So','Output smoke gas pipe cutset, m2',2,0,'90',0,19,'Вихідний перетин труби димових газів, м2','Выходное сечение трубы дымовых газов, м2','','','');
INSERT INTO techApp_io VALUES('boilerBurner','lo','Output smoke gas pipe length, m',2,0,'',0,20,'Вихідна довжина труби димових газів, м','Выходная длина трубы дымовых газов, м','','','');
INSERT INTO techApp_io VALUES('boilerBurner','V','Burner volume, m3',2,0,'830',0,21,'Об''єм топки, м3','Объём топки, м3','','','');
INSERT INTO techApp_io VALUES('boilerBurner','CO','Percentage of CO in the flue stack gases, %',2,1,'',0,22,'Відсоток вмісту CO у димових газах, %','Процент содержания CO в дымовых газах, %','','','');
INSERT INTO techApp_io VALUES('boilerBurner','O2','Percentage of O2 in the flue stack gases, %',2,1,'',0,23,'Відсоток вмісту O2 у димових газах, %','Процент содержания O2 в дымовых газах, %','','','');
INSERT INTO techApp_io VALUES('boilerBurner','f_frq','Calculation frequency, Hz',2,0,'200',0,24,'Частота обчислення, Гц','Частота вычисления, Гц','','','');
INSERT INTO techApp_io VALUES('pipe2_1','Fi1','Input 1 flow, ton/h',2,1,'0',0,0,'Вхідні витрати 1, т/год','Входной расход 1, т/ч','','','');
INSERT INTO techApp_io VALUES('pipe2_1','Pi1','Input 1 pressure, at',2,0,'1',0,1,'Вхідний тиск 1, ата','Входное давление 1, ата','','','');
INSERT INTO techApp_io VALUES('pipe2_1','Ti1','Input 1 temperature, K',2,0,'273',0,2,'Вхідна температура 1, К','Входная температура 1, К','','','');
INSERT INTO techApp_io VALUES('pipe2_1','Si1','Input 1 cutset, m2',2,0,'0.2',0,3,'Вхідний перетин 1, м2','Входное сечение 1, м2','','','');
INSERT INTO techApp_io VALUES('pipe2_1','Fi2','Input 2 flow, ton/h',2,1,'0',0,4,'Вхідні витрати 2, т/год','Входной расход 2, т/ч','','','');
INSERT INTO techApp_io VALUES('pipe2_1','Pi2','Input 2 pressure, at',2,0,'1',0,5,'Вхідний тиск 2, ата','Входное давление 2, ата','','','');
INSERT INTO techApp_io VALUES('pipe2_1','Ti2','Input 2 temperature, K',2,0,'273',0,6,'Вхідна температура 2, К','Входная температура 2, К','','','');
INSERT INTO techApp_io VALUES('pipe2_1','Si2','Input 2 cutset, m2',2,0,'0.2',0,7,'Вхідний перетин 2, м2','Входное сечение 2, м2','','','');
INSERT INTO techApp_io VALUES('pipe2_1','Fo','Output flow, ton/h',2,0,'0',0,8,'Вихідні витрати, т/год','Выходной расход, т/ч','','','');
INSERT INTO techApp_io VALUES('pipe2_1','Po','Output pressure, at',2,1,'1',0,9,'Вихідний тиск, ата','Выходное давление, ата','','','');
INSERT INTO techApp_io VALUES('pipe2_1','To','Output temperature, K',2,1,'273',0,10,'Вихідна температура, К','Выходная температура, К','','','');
INSERT INTO techApp_io VALUES('pipe2_1','So','Output cutset, m2',2,0,'.2',0,11,'Вихідний перетин, м2','Выходное сечение, м2','','','');
INSERT INTO techApp_io VALUES('pipe2_1','lo','Output length, m',2,0,'10',0,12,'Вихідна довжина, м','Выходная длина, м','','','');
INSERT INTO techApp_io VALUES('pipe2_1','Q0','Norm density of the environment, kg/m3',2,0,'1',0,13,'Нормальна щільність середовища, кг/м3','Нормальная плотность среды, кг/м3','','','');
INSERT INTO techApp_io VALUES('pipe2_1','Kpr','Coefficient of the compressibility [0...1]',2,0,'0.95',0,14,'Коефіцієнт стискання середовища [0...1]','Коэффициент сжимаемости среды [0...1]','','','');
INSERT INTO techApp_io VALUES('pipe2_1','Ct','Heat capacity of the environment',2,0,'20',0,15,'Теплоємність середовища','Теплоёмкость среды','','','');
INSERT INTO techApp_io VALUES('pipe2_1','Riz','Heat resistance of the isolation',2,0,'20',0,16,'Тепловий опір ізоляції','Тепловое сопротивление изоляции','','','');
INSERT INTO techApp_io VALUES('pipe2_1','Fwind','Air speed',2,0,'1',0,17,'Швидкість повітря','Скорость воздуха','','','');
INSERT INTO techApp_io VALUES('pipe2_1','Twind','Air temperature, К',2,0,'273',0,18,'Температура повітря, К','Температура воздуха, К','','','');
INSERT INTO techApp_io VALUES('pipe2_1','f_frq','Calculation frequency, Hz',2,0,'100',1,19,'Частота обчислення, Гц','Частота вычисления, Гц','','','');
INSERT INTO techApp_io VALUES('pipe1_3','Kpr','Coefficient of the compressibility [0...1]',2,0,'0.95',0,15,'Коефіцієнт стискання середовища [0...1]','Коэффициент сжимаемости среды [0...1]','','','');
INSERT INTO techApp_io VALUES('pipe1_4','Kpr','Coefficient of the compressibility [0...1]',2,0,'0.95',0,19,'Коефіцієнт стискання середовища [0...1]','Коэффициент сжимаемости среды [0...1]','','','');
INSERT INTO techApp_io VALUES('diaphragm','Kpr','Coefficient of the compressibility [0...1]',2,0,'0.95',0,9,'Коефіцієнт стискання середовища [0...1]','Коэффициент сжимаемости среды [0...1]','','','');
CREATE TABLE IF NOT EXISTS 'lib_Controllers_io' ("F_ID" TEXT DEFAULT '' ,"ID" TEXT DEFAULT '' ,"NAME" TEXT DEFAULT '' ,"TYPE" INTEGER DEFAULT '' ,"MODE" INTEGER DEFAULT '' ,"DEF" TEXT DEFAULT '' ,"HIDE" INTEGER DEFAULT '' ,"POS" INTEGER DEFAULT '' ,"ru#NAME" TEXT DEFAULT '' ,"ru#DEF" TEXT DEFAULT '' ,"uk#NAME" TEXT DEFAULT '' ,"uk#DEF" TEXT DEFAULT '' ,"sr#NAME" TEXT DEFAULT '' , PRIMARY KEY ("F_ID","ID"));
INSERT INTO lib_Controllers_io VALUES('prescr','dbDB','DB: DB with tables name',0,1,'',0,4,'БД: Имя БД с таблицами','','БД: Ім''я БД з таблицями','','');
INSERT INTO lib_Controllers_io VALUES('prescr','dbComs','DB: Table with commands',0,1,'',0,5,'БД: Таблица с командами','','БД: Таблиця з командами','','');
INSERT INTO lib_Controllers_io VALUES('prescr','dbProgs','DB: Table with programs',0,1,'',0,6,'БД: Таблица с программами','','БД: Таблиця з програмами','','');
INSERT INTO lib_Controllers_io VALUES('prescr','mode','Mode: Finish(-2); Error(-1); Stop(0); Run(1); Pause(2); Pass command(3)',1,1,'',0,8,'Режим: Завершение(-2); Ошибка(-1); Стоп(0); Исполнение(1); Пауза(2); Пропустить команду(3)','','Режим: Завершення(-2); Помилка(-1); Стоп(0); Виконання(1); Пауза(2); Пропустити команду(3)','','');
INSERT INTO lib_Controllers_io VALUES('prescr','curMode','Current mode',1,0,'',0,9,'Текущий режим','','Поточний режим','','');
INSERT INTO lib_Controllers_io VALUES('prescr','prog','Name of the selected program',0,1,'',0,10,'Имя выбранной программы','','Ім''я обраної програми','','');
INSERT INTO lib_Controllers_io VALUES('prescr','startTm','Start time, seconds',1,0,'',0,11,'Время запуска, секунды','','Час запуску, секунди','','');
INSERT INTO lib_Controllers_io VALUES('prescr','curCom','Current command-step',0,0,'',0,12,'Текущая комманда-шаг','','Поточна команда-крок','','');
INSERT INTO lib_Controllers_io VALUES('prescr','work','Work program',4,0,'',0,13,'Рабочая программа','','Робоча програма','','');
INSERT INTO lib_Controllers_io VALUES('prescr','comsCntr','Commands controller',0,0,'',0,7,'Контроллер команд','','Контролер команд','','');
INSERT INTO lib_Controllers_io VALUES('prescr','comLs','Commands list',4,0,'',0,14,'Список команд','','Перелік команд','','');
INSERT INTO lib_Controllers_io VALUES('prescr','clcCnt','Cycles counter',1,0,'',0,15,'Счётчик циклов','','Лічильник циклів','','');
INSERT INTO lib_Controllers_io VALUES('test','offset','Offset',1,0,'100',0,4,'Смещение','','Зміщення','','');
INSERT INTO lib_Controllers_io VALUES('test','out','Out',2,1,'',0,5,'Вых.','','Вих.','','');
INSERT INTO lib_Controllers_io VALUES('test','test','Test',0,1,'',0,6,'Тест','','Тест','','');
INSERT INTO lib_Controllers_io VALUES('test','rez','Result',0,1,'',0,7,'Результат','','Результат','','');
INSERT INTO lib_Controllers_io VALUES('test1','clc','Clock',2,0,'',0,0,'Часы','','Годинник','','');
INSERT INTO lib_Controllers_io VALUES('test1','p_tm','Previous time (s)',1,1,'0',0,1,'Предыдущее время (с)','','Попередній час (с)','','');
INSERT INTO lib_Controllers_io VALUES('test1','rez','Result',2,1,'0',0,2,'Результат','','Результат','','');
INSERT INTO lib_Controllers_io VALUES('prescr','f_frq','Function calculate frequency (Hz)',2,0,'1000',0,0,'','','','','');
INSERT INTO lib_Controllers_io VALUES('prescr','f_start','Function start flag',3,0,'0',0,1,'Флаг запуска функции','','Прапорець запуску функції','','Ознака за покретања функције');
INSERT INTO lib_Controllers_io VALUES('prescr','f_stop','Function stop flag',3,0,'0',0,2,'','','','','');
INSERT INTO lib_Controllers_io VALUES('prescr','this','This controller object link',4,0,'0',0,3,'','','','','');
INSERT INTO lib_Controllers_io VALUES('ntfDispatch','f_start','Function start flag',3,0,'',0,15,'Флаг запуска функции','','Прапорець запуску функції','','Ознака за покретања функције');
INSERT INTO lib_Controllers_io VALUES('ntfDispatch','emailTopic','EMail topic',0,0,'Notification',0,8,'Тема Эл.почты','Уведомление','Тема Ел.пошти','Повідомлення','');
INSERT INTO lib_Controllers_io VALUES('ntfDispatch','emailMess','EMail message',0,0,'',0,9,'Сообщение Эл.почты','','Повідомлення Ел.пошти','','');
INSERT INTO lib_Controllers_io VALUES('ntfDispatch','f_frq','Function calculate frequency (Hz)',2,0,'1000',0,17,'','','','','');
INSERT INTO lib_Controllers_io VALUES('ntfDispatch','f_stop','Function stop flag',3,0,'0',0,16,'','','','','');
INSERT INTO lib_Controllers_io VALUES('ntfDispatch','this','This controller object link',4,0,'0',0,18,'','','','','');
INSERT INTO lib_Controllers_io VALUES('ntfDispatch','tmOut','Maximum notification timeout, seconds',1,0,'5',0,0,'Максимальный таймаут сообщения, секунд','','Максимальний таймаут повідомлення, секунд','','');
INSERT INTO lib_Controllers_io VALUES('ntfDispatch','SMSTrId','SMS serial transport, empty for disable',0,0,'SMS',0,11,'Последовательный транспорт SMS, пусто для выключения','','Послідовний транспорт SMS, порожньо для вимкнення','','');
INSERT INTO lib_Controllers_io VALUES('ntfDispatch','SMSPin','SMS pin, empty for disable',0,0,'1111',0,12,'','','','','');
INSERT INTO lib_Controllers_io VALUES('test','f_frq','Function calculate frequency (Hz)',2,0,'1000',0,0,'','','','','');
INSERT INTO lib_Controllers_io VALUES('test','f_start','Function start flag',3,0,'0',0,1,'Флаг запуска функции','','Прапорець запуску функції','','Ознака за покретања функције');
INSERT INTO lib_Controllers_io VALUES('test','f_stop','Function stop flag',3,0,'0',0,2,'','','','','');
INSERT INTO lib_Controllers_io VALUES('test','this','This controller object link',4,0,'0',0,3,'','','','','');
INSERT INTO lib_Controllers_io VALUES('ntfDispatch','emailServer','Output transport for SMTP connection, empty for disable',0,0,'Sockets.out_SMTP',0,4,'Выходной транспорт SMTP подключений, пусто для выключения','','Вихідний транспорт SMTP підключень, порожньо для вимкнення','','');
INSERT INTO lib_Controllers_io VALUES('ntfDispatch','emailAuth','Auth, empty for disable',0,0,'user:pass',0,5,'Аутентификация, пусто для выключения','','Автентифікація, порожньо для вимкнення','','');
INSERT INTO lib_Controllers_io VALUES('ntfDispatch','emailSender','Sender',0,0,'noreply@oscada.org',0,6,'Отправитель','','Відправник','','');
INSERT INTO lib_Controllers_io VALUES('ntfDispatch','emailReceiver','Destination receiver address',0,0,'test@oscada.org',0,7,'Адрес назначения получателя','','Адреса призначення одержувача','','');
INSERT INTO lib_Controllers_io VALUES('ntfDispatch','SMSTel','SMS destination receiver, tel. number',0,0,'+380XXXXXXXXX',0,13,'Адрес получателя SMS, номер телефона','','Адреса одержувача SMS, номер телефону','','');
INSERT INTO lib_Controllers_io VALUES('ntfDispatch','emailState','Email notification current state',0,1,'',0,3,'Текущее состояние уведомления Эл.почтой','','Поточний стан повідомлення Ел.поштою','','');
INSERT INTO lib_Controllers_io VALUES('ntfDispatch','SMSState','SMS notification current state',0,1,'',0,10,'Текущее состояние уведомления SMS','','Поточний стан повідомлення SMS','','');
INSERT INTO lib_Controllers_io VALUES('ntfDispatch','SMSTextMd','SMS in the text mode, else PDU',3,0,'0',0,14,'SMS в текстовом режиме, иначе PDU','','SMS у текстовому режимі, інакше PDU','','');
INSERT INTO lib_Controllers_io VALUES('ntfDispatch','messLev','Messages level, negative for alarms',1,0,'1',0,1,'Уровень сообщений, негативный для нарушений','','Рівень повідомлень, негативний для порушень','','');
INSERT INTO lib_Controllers_io VALUES('ntfDispatch','messCat','Messages category, template or regular expression',0,0,'al*:*',0,2,'Категория сообщений, шаблон или регулярное выражение','','Категорія повідомлень, шаблон або регулярний вираз','','');
INSERT INTO lib_Controllers_io VALUES('test','inFarg','inFarg',1,1,'3.14',0,8,'','','','','');
CREATE TABLE IF NOT EXISTS 'flb_regEl_io' ("F_ID" TEXT DEFAULT '' ,"ID" TEXT DEFAULT '' ,"NAME" TEXT DEFAULT '' ,"uk#NAME" TEXT DEFAULT '' ,"TYPE" INTEGER DEFAULT '' ,"MODE" INTEGER DEFAULT '' ,"DEF" TEXT DEFAULT '' ,"uk#DEF" TEXT DEFAULT '' ,"HIDE" INTEGER DEFAULT '' ,"POS" INTEGER DEFAULT '' ,"ru#NAME" TEXT DEFAULT '' ,"ru#DEF" TEXT DEFAULT '' ,"sr#NAME" TEXT DEFAULT '' , PRIMARY KEY ("F_ID","ID"));
INSERT INTO flb_regEl_io VALUES('pidUnif','var','Variable','Змінна',2,0,'0','',0,0,'Переменная','','Променљива');
INSERT INTO flb_regEl_io VALUES('pidUnif','sp','Setpoint','Завдання',2,0,'0','',0,1,'Задание','','');
INSERT INTO flb_regEl_io VALUES('pidUnif','max','Scale: maximum','Шкала: максимум',2,0,'100','',0,2,'Шкала: максимум','','');
INSERT INTO flb_regEl_io VALUES('pidUnif','min','Scale: minimum','Шкала: мінімум',2,0,'0','',0,3,'Шкала: минимум','','');
INSERT INTO flb_regEl_io VALUES('pidUnif','manIn','Manual input','Ручний ввід',2,0,'0','',0,4,'Ручной ввод','','');
INSERT INTO flb_regEl_io VALUES('pidUnif','out','Output, %','Вихід, %',2,2,'0','',0,5,'Выход, %','','');
INSERT INTO flb_regEl_io VALUES('pidUnif','impQup','Impulse output up','Імпульсний вихід у гору',3,1,'0','',0,6,'Импульсный выход вверх','','');
INSERT INTO flb_regEl_io VALUES('pidUnif','impQdwn','Impulse output down','Імпульсний вихід до низу',3,1,'0','',0,7,'Имп. выход вниз','','');
INSERT INTO flb_regEl_io VALUES('pidUnif','analog','Analog','Аналоговий',3,0,'1','',0,8,'Аналоговый','','');
INSERT INTO flb_regEl_io VALUES('pidUnif','auto','Automate','Автомат',3,0,'0','',0,9,'Автомат','','');
INSERT INTO flb_regEl_io VALUES('pidUnif','casc','Cascade','Каскад',3,0,'0','',0,10,'Каскад','','');
INSERT INTO flb_regEl_io VALUES('pidUnif','Kp','Gain','Kп',2,0,'1','',0,11,'Kп','','');
INSERT INTO flb_regEl_io VALUES('pidUnif','Ti','Ti, milliseconds','Tі, мілісекунд',1,0,'1000','',0,12,'Tи, миллисекунд','','');
INSERT INTO flb_regEl_io VALUES('pidUnif','Td','Td, milliseconds','Tд, мілісекунд',1,0,'0','',0,14,'Tд, миллисекунд','','');
INSERT INTO flb_regEl_io VALUES('pidUnif','TImpPer','T impulses period, milliseconds','T період імпульсів, мілісекунд',1,0,'5000','',0,16,'T период импульса, миллисекунд','','');
INSERT INTO flb_regEl_io VALUES('pidUnif','TImpMin','T minimal impulse, milliseconds','Мінімальна довжина імпульсу, мілісекунд',1,0,'500','',0,17,'T минимальная длина импульса, миллисекунд','','');
INSERT INTO flb_regEl_io VALUES('pidUnif','KImpRfact','Impulse rate factor','Рейтинг фактор імпульсів',2,0,'1','',0,18,'Рейтинг фактор импульса','','');
INSERT INTO flb_regEl_io VALUES('pidUnif','Hup','Upper output border, %','Верхня границя виходу, %',2,0,'100','',0,19,'Верхняя граница выхода, %','','');
INSERT INTO flb_regEl_io VALUES('pidUnif','Hdwn','Lower output border, %','Нижня границя виходу, %',2,0,'0','',0,20,'Нижняя граница выхода, %','','');
INSERT INTO flb_regEl_io VALUES('pidUnif','Zi','Insensitivity, %','Нечутливість, %',2,0,'0','',0,21,'Нечувствительность, %','','');
INSERT INTO flb_regEl_io VALUES('pidUnif','K1','Coefficient of input 1','Коефіцієнт входу 1',2,0,'0','',0,23,'Коэффициент входа 1','','');
INSERT INTO flb_regEl_io VALUES('pidUnif','in1','Input 1','Вхід 1',2,0,'0','',0,24,'Вход 1','','');
INSERT INTO flb_regEl_io VALUES('pidUnif','K2','Coefficient of input 2','Коефіцієнт входу 2',2,0,'0','',0,25,'Коэффициент входа 2','','');
INSERT INTO flb_regEl_io VALUES('pidUnif','in2','Input 2','Вхід 2',2,0,'0','',0,26,'Вход 2','','');
INSERT INTO flb_regEl_io VALUES('pidUnif','K3','Coefficient of input 3','Коефіцієнт входу 3',2,0,'0','',0,27,'Коэффициент входа 3','','');
INSERT INTO flb_regEl_io VALUES('pidUnif','in3','Input 3','Вхід 3',2,0,'0','',0,28,'Вход 3','','');
INSERT INTO flb_regEl_io VALUES('pidUnif','K4','Coefficient of input 4','Коефіцієнт входу 4',2,0,'0','',0,29,'Коэффициент входа 4','','');
INSERT INTO flb_regEl_io VALUES('pidUnif','in4','Input 4','Вхід 4',2,0,'0','',0,30,'Вход 4','','');
INSERT INTO flb_regEl_io VALUES('pidUnif','impAnOut','Analog position of impulse regulator','Аналогове положення імпульсного регулятора',2,0,'0','',0,31,'Аналоговое положение импульсного регулятора','','');
INSERT INTO flb_regEl_io VALUES('pidUnif','int','Integral accumulation','Накопичення інтегралу',2,1,'0','',1,38,'Накопление интеграла','','');
INSERT INTO flb_regEl_io VALUES('pidUnif','dif','Differential accumulation','Накопич. диференціалу',2,1,'0','',1,39,'Накопление производной','','');
INSERT INTO flb_regEl_io VALUES('pidUnif','lag','Lag accumulation','Накопичення затримки',2,1,'0','',1,40,'Накопление задержки','','');
INSERT INTO flb_regEl_io VALUES('pidUnif','impLag','Impulse lag time','Час утримання імпульсу',2,1,'0','',1,41,'Время удержания импульса','','');
INSERT INTO flb_regEl_io VALUES('pidUnif','perLag','New impulse generation time','Час генерації нового імпульсу',2,1,'0','',1,42,'Время генерации нового импульса','','');
INSERT INTO flb_regEl_io VALUES('pidUnifD','var','Variable','Змінна',2,0,'0','',0,0,'Переменная','','Променљива');
INSERT INTO flb_regEl_io VALUES('pidUnifD','sp','Setpoint','Завдання',2,0,'0','',0,1,'Задание','','');
INSERT INTO flb_regEl_io VALUES('pidUnifD','max','Scale: maximum','Шкала: максимум',2,0,'100','',0,2,'Шкала: максимум','','');
INSERT INTO flb_regEl_io VALUES('pidUnifD','min','Scale: minimum','Шкала: мінімум',2,0,'0','',0,3,'Шкала: минимум','','');
INSERT INTO flb_regEl_io VALUES('pidUnifD','manIn','Manual input','Ручний ввід',2,0,'0','',0,4,'Ручной ввод','','');
INSERT INTO flb_regEl_io VALUES('pidUnifD','out','Output, %','Вихід, %',2,2,'0','',0,5,'Выход, %','','');
INSERT INTO flb_regEl_io VALUES('pidUnifD','impQup','Impulse output up','Імпульсний вихід у гору',3,1,'0','',0,6,'Импульсный выход вверх','','');
INSERT INTO flb_regEl_io VALUES('pidUnifD','impQdwn','Impulse output down','Імпульсний вихід до низу',3,1,'0','',0,7,'Имп. выход вниз','','');
INSERT INTO flb_regEl_io VALUES('pidUnifD','analog','Analog','Аналоговий',3,0,'1','',0,8,'Аналоговый','','');
INSERT INTO flb_regEl_io VALUES('pidUnifD','auto','Automat','Автомат',3,0,'0','',0,9,'Автомат','','');
INSERT INTO flb_regEl_io VALUES('pidUnifD','casc','Cascade','Каскад',3,0,'0','',0,10,'Каскад','','');
INSERT INTO flb_regEl_io VALUES('pidUnifD','Kp','Gain','Kп',2,0,'1','',0,11,'Kп','','');
INSERT INTO flb_regEl_io VALUES('pidUnifD','Ti','Ti, milliseconds','Tі, мілісекунд',1,0,'1000','',0,12,'Tи, миллисекунд','','');
INSERT INTO flb_regEl_io VALUES('pidUnifD','Kd','Gain differential','Кп диференціалу',2,0,'1','',0,13,'Кп производной','','');
INSERT INTO flb_regEl_io VALUES('pidUnifD','Td','Td, milliseconds','Tд, мілісекунд',1,0,'0','',0,14,'Tд, миллисекунд','','');
INSERT INTO flb_regEl_io VALUES('pidUnifD','Tzd','T differential lag, milliseconds','T фільтру диференціалу, мілісекунд',1,0,'0','',0,15,'T задержки дифференциала, миллисекунд','','');
INSERT INTO flb_regEl_io VALUES('pidUnifD','TImpPer','T impulses period, milliseconds','T період імпульсів, мілісекунд',1,0,'5000','',0,16,'T период импульса, миллисекунд','','');
INSERT INTO flb_regEl_io VALUES('pidUnifD','TImpMin','T minimal impulse, milliseconds','Мінімальна довжина імпульсу, мілісекунд',1,0,'500','',0,17,'T минимальная длина импульса, миллисекунд','','');
INSERT INTO flb_regEl_io VALUES('pidUnifD','KImpRfact','Impulse rate factor','Рейтинг фактор імпульсів',2,0,'1','',0,18,'Рейтинг фактор импульса','','');
INSERT INTO flb_regEl_io VALUES('pidUnifD','Hup','Upper output border, %','Верхня границя виходу, %',2,0,'100','',0,19,'Верхняя граница выхода, %','','');
INSERT INTO flb_regEl_io VALUES('pidUnifD','Hdwn','Lower output border, %','Нижня границя виходу, %',2,0,'0','',0,20,'Нижняя граница выхода, %','','');
INSERT INTO flb_regEl_io VALUES('pidUnifD','Zi','Insensitivity, %','Нечутливість, %',2,0,'0','',0,21,'Нечувствительность, %','','');
INSERT INTO flb_regEl_io VALUES('pidUnifD','followSp','Follow to variable by setpoint into manual','Відслідковувати завданням за змінною у ручному',3,0,'1','',0,22,'Следить заданием за переменной в ручном','','');
INSERT INTO flb_regEl_io VALUES('pidUnifD','K1','Coefficient of input 1','Коефіцієнт входу 1',2,0,'0','',0,23,'Коэффициент входа 1','','');
INSERT INTO flb_regEl_io VALUES('pidUnifD','in1','Input 1','Вхід 1',2,0,'0','',0,24,'Вход 1','','');
INSERT INTO flb_regEl_io VALUES('pidUnifD','K2','Coefficient of input 2','Коефіцієнт входу 2',2,0,'0','',0,25,'Коэффициент входа 2','','');
INSERT INTO flb_regEl_io VALUES('pidUnifD','in2','Input 2','Вхід 2',2,0,'0','',0,26,'Вход 2','','');
INSERT INTO flb_regEl_io VALUES('pidUnifD','K3','Coefficient of input 3','Коефіцієнт входу 3',2,0,'0','',0,27,'Коэффициент входа 3','','');
INSERT INTO flb_regEl_io VALUES('pidUnifD','in3','Input 3','Вхід 3',2,0,'0','',0,28,'Вход 3','','');
INSERT INTO flb_regEl_io VALUES('pidUnifD','K4','Coefficient of input 4','Коефіцієнт входу 4',2,0,'0','',0,29,'Коэффициент входа 4','','');
INSERT INTO flb_regEl_io VALUES('pidUnifD','in4','Input 4','Вхід 4',2,0,'0','',0,30,'Вход 4','','');
INSERT INTO flb_regEl_io VALUES('pidUnifD','impAnOut','Real analog position of impulse regulator, [0...100], -1 - disable','Аналогове положення імпульсного регулятора',2,0,'-1','',0,33,'Аналоговое положение импульсного регулятора','','');
INSERT INTO flb_regEl_io VALUES('pidUnif','Kd','Gain differential','Кп диференціалу',2,0,'1','',0,13,'Кп производной','','');
INSERT INTO flb_regEl_io VALUES('pidUnifD','impAnImit','Full stroke time for analog position imitation, s','Імітація аналогового положення',2,0,'-1','',0,34,'Имитация аналогового положения','','');
INSERT INTO flb_regEl_io VALUES('pidUnifD','impQupTm','Imp. output up lag for GUI','Імп. вихід вгору утримано для GUI',3,1,'0','',0,35,'Импульсный выход вверх удерж. для GUI','','');
INSERT INTO flb_regEl_io VALUES('pidUnifD','impQdwnTm','Imp. output down lag for GUI','Імп. вихід додолу утримано для GUI',3,1,'0','',0,36,'Имп. выход вниз удерж. для GUI','','');
INSERT INTO flb_regEl_io VALUES('pidUnifD','f_frq','Calculation frequency, Hz','Частота обчислення, Гц',2,0,'1','',1,37,'Частота вычисления, Гц','','');
INSERT INTO flb_regEl_io VALUES('pidUnifD','f_start','First start','Перший запуск',3,0,'0','',1,38,'Первый запуск','','');
INSERT INTO flb_regEl_io VALUES('pidUnifD','f_stop','Last start','Останній запуск',3,0,'0','',1,39,'Последний запуск','','');
INSERT INTO flb_regEl_io VALUES('pidUnifD','impLag','Impulse lag time','Час утримання імпульсу',2,1,'0','',1,40,'Время удержания импульса','','');
INSERT INTO flb_regEl_io VALUES('pidUnifD','perLag','New impulse generation time','Час генерації нового імпульсу',2,1,'0','',1,41,'Время генерации нового импульса','','');
INSERT INTO flb_regEl_io VALUES('pidUnifD','integ','Integral accumulation','Накопичення інтегралу',2,1,'0','',1,42,'Накопление интеграла','','');
INSERT INTO flb_regEl_io VALUES('pidUnifD','difer','Differential accumulation','Накопич. диференціалу',2,1,'0','',1,43,'Накопление производной','','');
INSERT INTO flb_regEl_io VALUES('pidUnifD','dlag','Differential lag accumulation','Фільтр диференціалу',2,1,'0','',1,44,'Накопление задержки производной','','');
INSERT INTO flb_regEl_io VALUES('pidUnif','Tzd','T differential lag, milliseconds','T фільтру диференціалу, мілісекунд',1,0,'0','',0,15,'T задержки дифференциала, миллисекунд','','');
INSERT INTO flb_regEl_io VALUES('pidUnif','followSp','Follow to variable by setpoint into manual','Відслідковувати завданням за змінною у ручному',3,0,'1','',0,22,'Следить заданием за переменной в ручном','','');
INSERT INTO flb_regEl_io VALUES('pidUnif','impAnImit','Analog position imitation','Імітація аналогового положення',3,0,'0','',0,32,'Имитация аналогового положения','','');
INSERT INTO flb_regEl_io VALUES('pidUnif','impQupTm','Imp. output up lag for GUI','Імп. вихід вгору утримано для GUI',1,1,'0','',0,33,'Импульсный выход вверх удерж. для GUI','','');
INSERT INTO flb_regEl_io VALUES('pidUnif','impQdwnTm','Imp. output down lag for GUI','Імп. вихід додолу утримано для GUI',1,1,'0','',0,34,'Имп. выход вниз удерж. для GUI','','');
INSERT INTO flb_regEl_io VALUES('pidUnif','f_frq','Calculation frequency, Hz','Частота обчислення, Гц',1,0,'1','',1,35,'Частота вычисления, Гц','','');
INSERT INTO flb_regEl_io VALUES('pidUnif','f_start','First start','Перший запуск',3,0,'0','',1,36,'Первый запуск','','');
INSERT INTO flb_regEl_io VALUES('pidUnif','f_stop','Last start','Останній запуск',3,0,'0','',1,37,'Последний запуск','','');
INSERT INTO flb_regEl_io VALUES('pidImp','var','Variable','Змінна',2,0,'0','',0,0,'Переменная','','Променљива');
INSERT INTO flb_regEl_io VALUES('pidImp','sp','Setpoint','Завдання',2,0,'0','',0,1,'Задание','','');
INSERT INTO flb_regEl_io VALUES('pidImp','max','Scale: maximum','Шкала: максимум',2,0,'100','',0,2,'Шкала: максимум','','');
INSERT INTO flb_regEl_io VALUES('pidImp','min','Scale: minimum','Шкала: мінімум',2,0,'0','',0,3,'Шкала: минимум','','');
INSERT INTO flb_regEl_io VALUES('pidImp','manIn','Manual input','Ручний ввід',2,0,'0','',0,4,'Ручной ввод','','');
INSERT INTO flb_regEl_io VALUES('pidImp','out','Output, %','Вихід, %',2,2,'0','',0,5,'Выход, %','','');
INSERT INTO flb_regEl_io VALUES('pidImp','outPID','PID output','Вихід ПІД',2,1,'0','',0,6,'Выход ПИД','','');
INSERT INTO flb_regEl_io VALUES('pidImp','impQup','Impulse output up','Імпульсний вихід у гору',3,1,'0','',0,7,'Импульсный выход вверх','','');
INSERT INTO flb_regEl_io VALUES('pidImp','impQdwn','Impulse output down','Імпульсний вихід до низу',3,1,'0','',0,8,'Имп. выход вниз','','');
INSERT INTO flb_regEl_io VALUES('pidImp','auto','Automate','Автомат',3,0,'0','',0,9,'Автомат','','');
INSERT INTO flb_regEl_io VALUES('pidImp','casc','Cascade','Каскад',3,0,'0','',0,10,'Каскад','','');
INSERT INTO flb_regEl_io VALUES('pidImp','Kp','Gain','Kп',2,0,'1','',0,11,'Kп','','');
INSERT INTO flb_regEl_io VALUES('pidImp','Ki','Gain integration','Кп інтегралу',2,0,'1','',0,12,'Кп интеграла','','');
INSERT INTO flb_regEl_io VALUES('pidImp','Ti','Ti, milliseconds','Tі, мілісекунд',1,0,'1000','',0,13,'Tи, миллисекунд','','');
INSERT INTO flb_regEl_io VALUES('pidImp','Kd','Gain differential','Кп диференціалу',2,0,'1','',0,14,'Кп производной','','');
INSERT INTO flb_regEl_io VALUES('pidImp','Td','Td, milliseconds','Tд, мілісекунд',1,0,'0','',0,15,'Tд, миллисекунд','','');
INSERT INTO flb_regEl_io VALUES('pidImp','Tzd','T differential lag, milliseconds','T фільтру диференціалу, мілісекунд',1,0,'0','',0,16,'T задержки дифференциала, миллисекунд','','');
INSERT INTO flb_regEl_io VALUES('pidImp','TImpPer','T impulses period, milliseconds','T період імпульсів, мілісекунд',1,0,'5000','',0,17,'T период импульса, миллисекунд','','');
INSERT INTO flb_regEl_io VALUES('pidImp','TImpMin','T minimal impulse, milliseconds','Мінімальна довжина імпульсу, мілісекунд',1,0,'500','',0,18,'T минимальная длина импульса, миллисекунд','','');
INSERT INTO flb_regEl_io VALUES('pidImp','KImpRfact','Impulse rate factor','Рейтинг фактор імпульсів',2,0,'1','',0,19,'Рейтинг фактор импульса','','');
INSERT INTO flb_regEl_io VALUES('pidImp','Hup','Upper output border, %','Верхня границя виходу, %',2,0,'100','',0,20,'Верхняя граница выхода, %','','');
INSERT INTO flb_regEl_io VALUES('pidImp','Hdwn','Lower output border, %','Нижня границя виходу, %',2,0,'0','',0,21,'Нижняя граница выхода, %','','');
INSERT INTO flb_regEl_io VALUES('pidImp','Zi','Insensitivity, %','Нечутливість, %',2,0,'0','',0,22,'Нечувствительность, %','','');
INSERT INTO flb_regEl_io VALUES('pidImp','followSp','Follow to variable by setpoint into manual','Відслідковувати завданням за змінною у ручному',3,0,'1','',0,23,'Следить заданием за переменной в ручном','','');
INSERT INTO flb_regEl_io VALUES('pidImp','K1','Coefficient of input 1','Коефіцієнт входу 1',2,0,'0','',0,24,'Коэффициент входа 1','','');
INSERT INTO flb_regEl_io VALUES('pidImp','in1','Input 1','Вхід 1',2,0,'0','',0,25,'Вход 1','','');
INSERT INTO flb_regEl_io VALUES('pidImp','K2','Coefficient of input 2','Коефіцієнт входу 2',2,0,'0','',0,26,'Коэффициент входа 2','','');
INSERT INTO flb_regEl_io VALUES('pidImp','in2','Input 2','Вхід 2',2,0,'0','',0,27,'Вход 2','','');
INSERT INTO flb_regEl_io VALUES('pidImp','K3','Coefficient of input 3','Коефіцієнт входу 3',2,0,'0','',0,28,'Коэффициент входа 3','','');
INSERT INTO flb_regEl_io VALUES('pidImp','in3','Input 3','Вхід 3',2,0,'0','',0,29,'Вход 3','','');
INSERT INTO flb_regEl_io VALUES('pidImp','K4','Coefficient of input 4','Коефіцієнт входу 4',2,0,'0','',0,30,'Коэффициент входа 4','','');
INSERT INTO flb_regEl_io VALUES('pidImp','in4','Input 4','Вхід 4',2,0,'0','',0,31,'Вход 4','','');
INSERT INTO flb_regEl_io VALUES('pidImp','impAnOut','Analog position of impulse regulator','Аналогове положення імпульсного регулятора',2,0,'0','',0,32,'Аналоговое положение импульсного регулятора','','');
INSERT INTO flb_regEl_io VALUES('pidImp','impAnImit','Analog position imitation','Імітація аналогового положення',3,0,'0','',0,33,'Имитация аналогового положения','','');
INSERT INTO flb_regEl_io VALUES('pidImp','impQupTm','Imp. output up lag for GUI','Імп. вихід вгору утримано для GUI',1,1,'0','',0,34,'Импульсный выход вверх удерж. для GUI','','');
INSERT INTO flb_regEl_io VALUES('pidImp','impQdwnTm','Imp. output down lag for GUI','Імп. вихід додолу утримано для GUI',1,1,'0','',0,35,'Имп. выход вниз удерж. для GUI','','');
INSERT INTO flb_regEl_io VALUES('pidImp','f_frq','Calculation frequency, Hz','Частота обчислення, Гц',2,0,'1','',1,36,'Частота вычисления, Гц','','');
INSERT INTO flb_regEl_io VALUES('pidImp','f_start','First start','Перший запуск',3,0,'0','',1,37,'Первый запуск','','');
INSERT INTO flb_regEl_io VALUES('pidImp','f_stop','Last start','Останній запуск',3,0,'0','',1,38,'Последний запуск','','');
INSERT INTO flb_regEl_io VALUES('pidImp','impLag','Impulse lag time','Час утримання імпульсу',2,1,'0','',1,39,'Время удержания импульса','','');
INSERT INTO flb_regEl_io VALUES('pidImp','perLag','New impulse generation time','Час генерації нового імпульсу',2,1,'0','',1,40,'Время генерации нового импульса','','');
INSERT INTO flb_regEl_io VALUES('pidImp','integ','Integral accumulation','Накопичення інтегралу',2,1,'0','',1,41,'Накопление интеграла','','');
INSERT INTO flb_regEl_io VALUES('pidImp','difer','Differential accumulation','Накопич. диференціалу',2,1,'0','',1,42,'Накопление производной','','');
INSERT INTO flb_regEl_io VALUES('pidImp','dlag','Differential lag accumulation','Фільтр диференціалу',2,1,'0','',1,43,'Накопление задержки производной','','');
INSERT INTO flb_regEl_io VALUES('pidImp','err1','Error step back','Помилка крок назад',2,1,'0','',1,44,'Ошибка шаг назад','','');
INSERT INTO flb_regEl_io VALUES('pidImp','err2','Error two step back','Помилка два кроки назад',2,1,'0','',1,45,'Ошибка два шага назад','','');
INSERT INTO flb_regEl_io VALUES('pidUnifD','QO','Opened state of the control mechanism','Відкритий стан виконавчого механізму',3,0,'0','',0,31,'Открытое состояние исполнительного механизма','','');
INSERT INTO flb_regEl_io VALUES('pidUnifD','QZ','Closed state of the control mechanism','Закритий стан виконавчого механізму',3,0,'0','',0,32,'Закрытое состояние исполнительного механизма','','');
COMMIT;
