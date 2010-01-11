#===== Generic Info ======
Summary: Open SCADA system project
Name: openscada
Version: 0.6.4.1
Release: 1
Source: openscada-%version.tar.gz
License: GPLv2
Group: Applications/Engineering
Packager: Roman Savochenko <rom_as@oscada.org, rom_as@fromru.com>
URL: http://oscada.org.ua

%define srcname openscada-%version

#===== DB subsystem modules ======
%def_enable DBF
%def_enable SQLite
%def_enable MySQL
%def_enable FireBird

#===== DAQ modules =====
%def_enable System
%def_enable BlockCalc
%def_enable JavaLikeCalc
%def_enable LogicLev
%def_enable SNMP
%def_enable Siemens
%def_enable ModBus
%def_enable DCON
%def_enable DAQGate
%def_enable SoundCard
%ifnarch %ix86
%force_disable DiamondBoards
%force_disable ICP_DAS
%else
%def_enable ICP_DAS
%def_enable DiamondBoards
%endif

#=====  Archive modules =====
%def_enable FSArch
%def_enable DBArch

#===== Transport modules =====
%def_enable Sockets
%def_enable SSL
%def_enable Serial

#===== Transport protocol modules =====
%def_enable HTTP
%def_enable SelfSystem

#===== UI modules =====
%def_enable VCAEngine
%def_enable Vision
%def_enable QTStarter
%def_enable QTCfg
%def_enable WebCfg
%def_enable WebCfgD
%def_enable WebVision

#===== Special modules =====
%def_enable SystemTests
%def_enable FLibComplex1
%def_enable FLibMath
%def_enable FLibSYS


#= Individual distributives seting =
%if %_vendor == "alt"
%set_verify_elf_method no
BuildRequires: glibc-devel gcc4.4-c++ libgd2-devel libexpat-devel libMySQL-devel libsqlite3-devel libsensors-devel
BuildRequires: libnet-snmp-devel libqt4-devel firebird-devel libportaudio2-devel libfftw3-devel
%else
%define _initdir /etc/init.d
%define _desktopdir %_datadir/applications
%define _iconsdir /usr/share/icons
BuildRoot: %_tmppath/%name-%version-root
%endif

%description
Open SCADA system. For access use account "root" and password "openscada".
%description -l ru_RU.UTF8
Открытая SCADA система. Для доступа используйте запись "root" и пароль "openscada".
%description -l uk_UA.UTF8
Відкрита SCADA система. Для доступу використовуйте запис "root" та пароль "openscada".
%description -l de_DE.UTF8
Das offene SCADA System. Fuer den Zugang die Aufzeichnung "root" und das Kennwort "openscada" benutzen.

%package doc
Summary: Open SCADA documents.
Group: Documentation
%description doc
The %name-doc package include documents files.
%description doc -l ru_RU.UTF8
Пакет %name-doc включает файлы документации.
%description doc -l uk_UA.UTF8
Пакет %name-doc включає файли документації.
%description doc -l de_DE.UTF8
Das Paket %name-doc enthaelt Dokumentationsdateien.

%package devel
Summary: Open SCADA development.
Group: Development/C++
Requires: %name = %version-%release
%description devel
The %name-devel package includes library archives and include files.
%description devel -l ru_RU.UTF8
Пакет %name-devel включает архив библиотек и включаемые файлы.
%description devel -l uk_UA.UTF8
Пакет %name-devel включає архів бібліотек та включаємі файли.

%package demo
Summary: Open SCADA demo data bases and config.
Group: Graphics
Requires: %name = %version-%release %name-DAQ.BlockCalc %name-Archive.FSArch
Requires: %name-DAQ.JavaLikeCalc %name-DAQ.LogicLev %name-DAQ.ModBus %name-DAQ.System
Requires: %name-DB.SQLite, %name-Protocol.HTTP %name-Protocol.SelfSystem
Requires: %name-Special.FLibComplex1, %name-Special.FLibMath, %name-Special.FLibSYS, %name-Transport.Sockets,
Requires: %name-UI.QTCfg, %name-UI.QTStarter, %name-UI.VCAEngine, %name-UI.Vision, %name-UI.WebCfgD, %name-UI.WebVision
%description demo
The %{name}-demo package includes demo data bases and configs. For start use command <openscada_demo_en>.
For access use account "root" and password "openscada" or account "user" without password.
%description demo -l ru_RU.UTF8
Пакет %{name}-demo включает демонстрационные базы данных и конфигурации.
Для старта используйте команду <openscada_demo_ru>.
Для доступа используйте запись "root" и пароль "openscada" или запись "user" без пароля.
%description demo -l uk_UA.UTF8
Пакет %{name}-demo включає демонстраційні бази даних та конфігурації.
Для старту використовуйте команду <openscada_demo_ru>.
Для доступу використовуйте запис "root" та пароль "openscada" або запис "user" без пароля.
%description demo -l de_DE.UTF8
Das Paket %{name}-demo enthaelt Demodatenbanken und Konfigurationen.
Fuers Starten wird Kommando <openscada_demo_en> benutzt.
Fuer den Zugang die Aufzeichnung "root" und das Kennwort "openscada" benutzen.

%package plc
Summary: OpenSCADA PLC.
Group: Graphics
%if %_vendor == "alt"
BuildArch: noarch
%endif
Requires: %name = %version-%release %name-Archive.FSArch %name-DAQ.BlockCalc %name-DAQ.JavaLikeCalc %name-DAQ.LogicLev
Requires: %name-DAQ.ModBus %name-DAQ.System %name-DB.SQLite %name-Protocol.HTTP %name-Protocol.SelfSystem %name-Special.FLibComplex1
Requires: %name-Special.FLibMath %name-Special.FLibSYS %name-Transport.SSL %name-Transport.Serial %name-Transport.Sockets %name-UI.VCAEngine
Requires: %name-UI.WebCfgD %name-UI.WebVision
%description plc
The %name-plc package is virtual package for PLC.
%description plc -l ru_RU.UTF8
Пакет %name-plc это виртуальный пакет для ПЛК.
%description plc -l uk_UA.UTF8
Пакет %name-plc це віртуальний пакет для ПЛК.

%package server
Summary: OpenSCADA server.
Group: Graphics
%if %_vendor == "alt"
BuildArch: noarch
%endif
Requires: %name = %version-%release %name-DB.SQLite %name-DB.MySQL %name-DB.FireBird %name-DAQ.System %name-DAQ.BlockCalc %name-DAQ.JavaLikeCalc
Requires: %name-DAQ.LogicLev %name-DAQ.SNMP %name-DAQ.Siemens %name-DAQ.ModBus %name-DAQ.DCON %name-DAQ.DAQGate %name-DAQ.SoundCard
Requires: %name-Archive.FSArch %name-Archive.DBArch %name-Transport.Sockets %name-Transport.SSL %name-Transport.Serial %name-Protocol.HTTP
Requires: %name-Protocol.SelfSystem %name-UI.VCAEngine %name-UI.WebCfg %name-UI.WebCfgD %name-UI.WebVision %name-Special.FLibComplex1 %name-Special.FLibMath
Requires: %name-Special.FLibSYS
%description server
The %name-server package is virtual package for SCADA-server.
%description server -l ru_RU.UTF8
Пакет %name-server это виртуальный пакет для сервера SCADA.
%description server -l uk_UA.UTF8
Пакет %name-server це віртуальний пакет для сервера SCADA.

%package visStation
Summary: OpenSCADA visual station.
Group: Graphics
%if %_vendor == "alt"
BuildArch: noarch
%endif
Requires: %name = %version-%release %name-DB.SQLite %name-DB.MySQL %name-DAQ.System %name-DAQ.BlockCalc %name-DAQ.JavaLikeCalc %name-DAQ.LogicLev
Requires: %name-DAQ.SNMP %name-DAQ.Siemens %name-DAQ.ModBus %name-DAQ.DCON %name-DAQ.DAQGate %name-DAQ.SoundCard %name-Archive.FSArch %name-Archive.DBArch
Requires: %name-Transport.Sockets %name-Transport.SSL %name-Transport.Serial %name-Protocol.SelfSystem %name-UI.VCAEngine %name-UI.Vision %name-UI.QTStarter
Requires: %name-UI.QTCfg %name-Special.FLibComplex1 %name-Special.FLibMath %name-Special.FLibSYS
%description visStation
The %name-visStation package is virtual package for visual station (SCADA).
%description visStation -l ru_RU.UTF8
Пакет %name-visStation это виртуальный пакет для визуальной станции (SCADA).
%description visStation -l uk_UA.UTF8
Пакет %name-visStation це віртуальний пакет для сервера візуальної станції (SCADA).

#===== DB subsystem modules ======
%if_enabled DBF
%package DB.DBF
Summary: DB DBF support.
Group: Graphics
Requires: %name = %version-%release
%description DB.DBF
The %{name}-DB.DBF package allow support of the *.dbf files, version 3.0.
%description DB.DBF -l ru_RU.UTF8
Пакет %{name}-DB.DBF предоставляет поддержку *.dbf файлов, версии 3.0.
%description DB.DBF -l uk_UA.UTF8
Пакет %{name}-DB.DBF надає підтримку *.dbf файлів, версії 3.0.
%description DB.DBF -l de_DE.UTF8
Das Paket %{name}-DB.DBF unterstützt die *.dbf Datenbank Version 3.0.
%endif

%if_enabled SQLite
%package DB.SQLite
Summary: DB SQLite support.
Group: Graphics
Requires: %name = %version-%release
%description DB.SQLite
The %{name}-DB.SQLite package allow support of the BD SQLite.
%description DB.SQLite -l ru_RU.UTF8
Пакет %{name}-DB.SQLite предоставляет поддержку БД SQLite.
%description DB.SQLite -l uk_UA.UTF8
Пакет %{name}-DB.SQLite надає підтримку БД SQLite.
%description DB.SQLite -l de_DE.UTF8
Das Paket %{name}-DB.SQLite unterstützt die SQLite Datenbank.
%endif

%if_enabled MySQL
%package DB.MySQL
Summary: DB MySQL support.
Group: Graphics
Requires: %name = %version-%release
%description DB.MySQL
The %{name}-DB.MySQL package allow support of the BD MySQL.
%description DB.MySQL -l ru_RU.UTF8
Пакет %{name}-DB.MySQL предоставляет поддержку БД MySQL.
%description DB.MySQL -l uk_UA.UTF8
Пакет %{name}-DB.MySQL надає підтримку БД MySQL.
%description DB.MySQL -l de_DE.UTF8
Das Paket %{name}-DB.MySQL unterstützt die MySQL Datenbank.
%endif

%if_enabled FireBird
%package DB.FireBird
Summary: DB FireBird support.
Group: Graphics
Requires: %name = %version-%release
%description DB.FireBird
The %{name}-DB.FireBird package allow support of the DB FireBird.
%description DB.FireBird -l ru_RU.UTF8
Пакет %{name}-DB.FireBird предоставляет поддержку БД FireBird.
%description DB.FireBird -l uk_UA.UTF8
Пакет %{name}-DB.FireBird надає підтримку БД FireBird.
%description DB.FireBird -l de_DE.UTF8
Das Paket %{name}-DB.FireBird unterstützt die FireBird Datenbank.
%endif

#===== DAQ modules =====
%if_enabled System
%package DAQ.System
Summary: System DA.
Group: Graphics
Requires: %name = %version-%release
%description DAQ.System
The %{name}-DAQ.System package allow operation system data acquisition.
Support OS Linux data sources: HDDTemp, Sensors, Uptime, Memory, CPU and other.
%description DAQ.System -l ru_RU.UTF8
Пакет %{name}-DAQ.System предоставляет сбор данных операционной системы.
Поддерживаются источники данных ОС Linux: HDDTemp, Sensors, Uptime, Memory, CPU и другие.
%description DAQ.System -l uk_UA.UTF8
Пакет %{name}-DAQ.System надає збір даних операційної системи.
Підтримуються джерела даних ОС Linux: HDDTemp, Sensors, Uptime, Memory, CPU та інше.
%description DAQ.System -l de_DE.UTF8
Das Paket %{name}-DAQ.System emöglicht die Datenerfassung des Betriebssystems.
Unterstützt werden die ОС Linux Datenquellen: HDDTemp, Sensors, Uptime, Memory, CPU und andere.
%endif

%if_enabled BlockCalc
%package DAQ.BlockCalc
Summary: Block based calculator.
Group: Graphics
Requires: %name = %version-%release
%description DAQ.BlockCalc
The %{name}-DB.DAQ.BlockCalc package allow block based calculator.
%description DAQ.BlockCalc -l ru_RU.UTF8
Пакет %{name}-DAQ.BlockCalc предоставляет блочный вычислитель.
%description DAQ.BlockCalc -l uk_UA.UTF8
Пакет %{name}-DAQ.BlockCalc надає блоковий обчислювач.
%description DAQ.BlockCalc -l de_DE.UTF8
Das Paket %{name}-DAQ.BlockCalc erlaubt Berechnungen mit dem Blockrechner.
%endif

%if_enabled JavaLikeCalc
%package DAQ.JavaLikeCalc
Summary: Java-like based calculator.
Group: Graphics
Requires: %name = %version-%release
%description DAQ.JavaLikeCalc
The %{name}-DAQ.JavaLikeCalc package allow java-like based calculator and function's libraries engine.
User can create and modify function and libraries.
%description DAQ.JavaLikeCalc -l ru_RU.UTF8
Пакет %{name}-DAQ.JavaLikeCalc предоставляет основанные на java подобном языке вычислитель и движок библиотек.
Пользователь может создавать и модифицировать функции и библиотеки.
%description DAQ.JavaLikeCalc -l uk_UA.UTF8
Пакет %{name}-DAQ.JavaLikeCalc надає базовані на мові схожій на Java обчислювач та движок бібліотек функцї.
Користувач може створювати та модифікувати функції та бібліотеки.
%description DAQ.JavaLikeCalc -l de_DE.UTF8
Das Paket %{name}-DAQ.JavaLikeCalc java-ähnlicher Rechner und Bibliotheken.
Der Benutzer kann Funktionen und Bibliotheken erstellen und modifizieren.
%endif

%if_enabled DiamondBoards
%package DAQ.DiamondBoards
Summary: Diamond DA boards.
Group: Graphics
Requires: %name = %version-%release
%description DAQ.DiamondBoards
The %{name}-DAQ.DiamondBoards package allow access to Diamond systems DA boards.
Include support of Athena board.
%description DAQ.DiamondBoards -l ru_RU.UTF8
Пакет %{name}-DAQ.DiamondBoards предоставляет доступ к платам сбора данных фирмы Diamond systems.
Включает поддержку системной платы Athena.
%description DAQ.DiamondBoards -l uk_UA.UTF8
Пакет %{name}-DAQ.DiamondBoards надає доступ до плат збору даних фірми Diamond systems.
Включає підтримку системної плати Athena.
%description DAQ.DiamondBoards -l de_DE.UTF8
Das Paket %{name}-DAQ.DiamondBoards Zugriff auf Datenerfassungskarten von Diamond Systems (und Systemplatform Athena).
%endif

%if_enabled LogicLev
%package DAQ.LogicLev
Summary: Logic level.
Group: Graphics
Requires: %name = %version-%release
%description DAQ.LogicLev
The %{name}-DAQ.LogicLev package allow logic level parameters.
%description DAQ.LogicLev -l ru_RU.UTF8
Пакет %{name}-DAQ.LogicLev предоставляет логический уровень параметров.
%description DAQ.LogicLev -l uk_UA.UTF8
Пакет %{name}-DAQ.LogicLev надає логічний рівень параметрів.
%description DAQ.LogicLev -l de_DE.UTF8
Das Paket %{name}-DAQ.LogicLev ermöglicht Parameter der logischen Stufe.
%endif

%if_enabled SNMP
%package DAQ.SNMP
Summary: SNMP client.
Group: Graphics
Requires: %name = %version-%release
%description DAQ.SNMP
The %{name}-DAQ.SNMP package allow realising of SNMP client service.
%description DAQ.SNMP -l ru_RU.UTF8
Пакет %{name}-DAQ.SNMP предоставляет реализацию клиентского сервиса протокола SNMP.
%description DAQ.SNMP -l uk_UA.UTF8
Пакет %{name}-DAQ.SNMP надає реалізацію клієнтського SNMP сервісу.
%description DAQ.SNMP -l de_DE.UTF8
Das Paket %{name}-DAQ.SNMP anbindung eines Clients für das SNMP-Protokoll.
%endif

%if_enabled Siemens
%package DAQ.Siemens
Summary: Siemens DAQ.
Group: Graphics
Requires: %name = %version-%release
%description DAQ.Siemens
The %{name}-DAQ.Siemens package allow data source Siemens PLC by CP of Hilscher CIF cards use
MPI protocol and library Libnodave for other.
%description DAQ.Siemens -l ru_RU.UTF8
Пакет %{name}-DAQ.Siemens предоставляет источник данных ПЛК Siemens посредством
карт Hilscher CIF с использованием протокола MPI и библиотеки Libnodave для остального.
%description DAQ.Siemens -l uk_UA.UTF8
Пакет %{name}-DAQ.Siemens надає джерело даних ПЛК Siemens за допомогою
карт Hilscher CIF з використанням протоколу MPI та бібліотеки Libnodave для іншого.
%description DAQ.Siemens -l de_DE.UTF8
Das Paket %{name}-DAQ.Siemens ermöglicht Datenquelle Siemens PLC-Karten mit Hilscher CIF.
Für Andere werden MPI-Protokoll und Bibliothek Libnodave verwendet.
%endif

%if_enabled ModBus
%package DAQ.ModBus
Summary: ModBus.
Group: Graphics
Requires: %name = %version-%release
%description DAQ.ModBus
The %{name}-DAQ.ModBus package allow realisation of ModBus client service. Supported Modbus/TCP, Modbus/RTU and Modbus/ASCII protocols.
%description DAQ.ModBus -l ru_RU.UTF8
Пакет %{name}-DAQ.ModBus предоставляет реализацию клиентского сервиса протокола ModBus. Поддерживаются Modbus/TCP, Modbus/RTU и Modbus/ASCII протоколы.
%description DAQ.ModBus -l uk_UA.UTF8
Пакет %{name}-DAQ.ModBus надає реалізацію клієнтського ModBus сервісу. Підтримуються Modbus/TCP, Modbus/RTU та Modbus/ASCII протоколи.
%description DAQ.ModBus -l de_DE.UTF8
Das Paket %{name}-DAQ.ModBus ermöglicht die Realisierung des Klientservices des ModBus-Protokolls. Unterstützt werden Modbus/TCP, Modbus/RTU und Modbus/ASCII Protokolle.
%endif

%if_enabled DCON
%package DAQ.DCON
Summary: DCON client.
Group: Graphics
Requires: %name = %version-%release
%description DAQ.DCON
The %{name}-DAQ.DCON package allow realisation of DCON client service. Supported I-7000 DCON protocol.
%description DAQ.DCON -l ru_RU.UTF8
Пакет %{name}-DAQ.DCON предоставляет реализацию клиентского сервиса протокола DCON. Поддерживается протокол I-7000 DCON.
%description DAQ.DCON -l uk_UA.UTF8
Пакет %{name}-DAQ.DCON надає реалізацію клієнтського сервісу DCON. Підтримується I-7000 DCON протокол.
%description DAQ.DCON -l de_DE.UTF8
Das Paket %{name}-DAQ.DCON Realisierung des I-7000 DCON Client-Protokolls.
%endif

%if_enabled DAQGate
%package DAQ.DAQGate
Summary: Data sources gate.
Group: Graphics
Requires: %name = %version-%release
%description DAQ.DAQGate
The %{name}-DAQ.DAQGate package allow to make gate data sources of remote OpenSCADA station to local OpenSCADA station.
%description DAQ.DAQGate -l ru_RU.UTF8
Пакет %{name}-DAQ.DAQGate позволяет выполнять шлюзование источников данных удалённых OpenSCADA станций в локальные.
%description DAQ.DAQGate -l uk_UA.UTF8
Пакет %{name}-DAQ.DAQGate дозволяє шлюзувати джерела даних віддалених OpenSCADA станцій до локальних.
%description DAQ.DAQGate -l de_DE.UTF8
Das Paket %{name}-DAQ.DAQGate verbindung mit Datenquellen von entfernten OpenSCADA-Stationen.
%endif

%if_enabled SoundCard
%package DAQ.SoundCard
Summary: Sound card.
Group: Graphics
Requires: %name = %version-%release
%description DAQ.SoundCard
The %{name}-DAQ.SoundCard package allow access to sound card data.
%description DAQ.SoundCard -l ru_RU.UTF8
Пакет %{name}-DAQ.SoundCard предоставляет доступ к звуковой карте.
%description DAQ.SoundCard -l uk_UA.UTF8
Пакет %{name}-DAQ.SoundCard надає доступ до даних звукової карти.
%description DAQ.SoundCard -l de_DE.UTF8
Das Paket %{name}-DAQ.SoundCard zugriff auf Soundkartendata.
%endif

%if_enabled ICP_DAS
%package DAQ.ICP_DAS
Summary: ICP DAS hardware.
Group: Graphics
Requires: %name = %version-%release
%description DAQ.ICP_DAS
The %{name}-DAQ.ICP_DAS package allow realisation of ICP DAS hardware support. Include I87000 and I-7000 DCON modules and I-8000 fast modules.
%description DAQ.ICP_DAS -l ru_RU.UTF8
Пакет %{name}-DAQ.ICP_DAS предоставляет реализацию поддержки оборудования ICP DAS. Включена поддержка I-87000 и I-7000 DCON модулей и I-8000 быстрых модулей.
%description DAQ.ICP_DAS -l uk_UA.UTF8
Пакет %{name}-DAQ.ICP_DAS надає реалізацію підтримки обладнання ICP DAS. Включаючи I-87000 та I-7000 DCON модулі та I-8000 швидкі модулі.
%endif

#=====  Archive modules =====
%if_enabled FSArch
%package Archive.FSArch
Summary: File system archivator.
Group: Graphics
Requires: %name = %version-%release
%description Archive.FSArch
The %{name}-Archive.FSArch package allow functions for messages and values arhiving to file system.
%description Archive.FSArch -l ru_RU.UTF8
Пакет %{name}-Archive.FSArch предоставляет функции архивирования сообщений и значений на файловую систему.
%description Archive.FSArch -l uk_UA.UTF8
Пакет %{name}-Archive.FSArch надає функції архівації повідомлень та значень на файлову систему.
%description Archive.FSArch -l de_DE.UTF8
Das Paket %{name}-Archive.FSArch funktionen zum Archivieren von Nachrichten und Werten in dasDateisystem.
%endif

%if_enabled DBArch
%package Archive.DBArch
Summary: To DB archivator.
Group: Graphics
Requires: %name = %version-%release
%description Archive.DBArch
The %{name}-Archive.DBArch package allow functions for messages and values arhiving to DB.
%description Archive.DBArch -l ru_RU.UTF8
Пакет %{name}-Archive.DBArch предоставляет функции архивирования сообщений и значений на БД.
%description Archive.DBArch -l uk_UA.UTF8
Пакет %{name}-Archive.DBArch надає функції архівації повідомлень та значень на БД.
%description Archive.DBArch -l de_DE.UTF8
Das Paket %{name}-Archive.DBArch funktionen zum Archivieren von Nachrichten und Werten in die Datenbank.
%endif

#===== Transport modules =====
%if_enabled Sockets
%package Transport.Sockets
Summary: Transport sockets.
Group: Graphics
Requires: %name = %version-%release
%description Transport.Sockets
The %{name}-Transport.Sockets package allow sockets based transport. Support inet and unix sockets. Inet socket use TCP and UDP protocols.
%description Transport.Sockets -l ru_RU.UTF8
Пакет %{name}-Transport.Sockets предоставляет транспорт основанный на сокетах. Поддерживаются интернет и UNIX сокеты. Интернет сокет использует TCP и UDP протоколы.
%description Transport.Sockets -l uk_UA.UTF8
Пакет %{name}-Transport.Sockets надає транспорт базований на сокетах. Підтримуються інтернет та UNIX сокети. Інтернет сокет використовує TCP та UDP протоколи.
%description Transport.Sockets -l de_DE.UTF8
Das Paket %{name}-Transport.Sockets gewährt den auf Sockets gebauten Transport. Internet und UNIX Sockets werden unterstützt. Internet Socket benutzt die TCP und UDP Protokolle.
%endif

%if_enabled SSL
%package Transport.SSL
Summary: Transport SSL.
Group: Graphics
Requires: %name = %version-%release
%description Transport.SSL
The %{name}-Transport.SSL package allow security socket layer based transport. Used OpenSSL and supported SSLv2, SSLv3 and TLSv1.
%description Transport.SSL -l ru_RU.UTF8
Пакет %{name}-Transport.SSL предоставляет транспорт основанный на слое безопасных сокетов. Используется OpenSSL и поддерживаютя SSLv2, SSLv3 and TLSv1.
%description Transport.SSL -l uk_UA.UTF8
Пакет %{name}-Transport.SSL надає транспорт базований на безпечному шарі сокетів. Використано OpenSSL та підтримуються SSLv2, SSLv3 and TLSv1.
%description Transport.SSL -l de_DE.UTF8
Das Paket %{name}-Transport.SSL gewährt den auf der Schicht von sicheren Sockets begründeten Transport. Es werden benutzt OpenSSL und unterstützt SSLv2, SSLv3 und TLSv1.
%endif

%if_enabled Serial
%package Transport.Serial
Summary: Transport serial interfaces.
Group: Graphics
Requires: %name = %version-%release
%description Transport.Serial
The %{name}-Transport.Serial package allow serial based interfaces. Used for data exchanging through serial interfaces like RS232, RS485, GSM and other.
%description Transport.Serial -l ru_RU.UTF8
Пакет %{name}-Transport.Serial предоставляет последовательный интерфейс. Используется для обмена данными через последовательные интерфейсы типа RS232, RS485, GSM и другое.
%description Transport.Serial -l uk_UA.UTF8
Пакет %{name}-Transport.Serial надає послідовні інтерфейси. Використовується для обміну даними через послідовні інтерфейсти типу RS232, RS485, GSM та інше.
%description Transport.Serial -l de_DE.UTF8
Das Paket %{name}-Transport.Serial ermöglicht die Verwendung der Seriellen Schnittstellen für den Datenaustausch mit RS232, RS485, GSM und anderen.
%endif

#===== Transport protocol modules =====
%if_enabled HTTP
%package Protocol.HTTP
Summary: Protocol HTTP-realisation.
Group: Graphics
Requires: %name = %version-%release
%description Protocol.HTTP
The %{name}-Protocol.HTTP package allow support HTTP for WWW based UIs.
%description Protocol.HTTP -l ru_RU.UTF8
Пакет %{name}-Protocol.HTTP предоставляет поддержку HTTP для WWW основанных пользовательских интерфейсов.
%description Protocol.HTTP -l uk_UA.UTF8
Пакет %{name}-Protocol.HTTP надає підтримку HTTP для WWW базозованих користувальницьких інтерфейсів.
%description Protocol.HTTP -l de_DE.UTF8
Das Paket %{name}-Protocol.HTTP HTTP-Unterstützung für eine WWW basierte Benutzerschnittstelle.
%endif

%if_enabled SelfSystem
%package Protocol.SelfSystem
Summary: Self system OpenSCADA protocol.
Group: Graphics
Requires: %name = %version-%release
%description Protocol.SelfSystem
The %{name}-Protocol.SelfSystem package - self OpenSCADA protocol, support generic functions.
%description Protocol.SelfSystem -l ru_RU.UTF8
Пакет %{name}-Protocol.SelfSystem - cобственный протокол OpenSCADA, поддерживает основные функции.
%description Protocol.SelfSystem -l uk_UA.UTF8
Пакет %{name}-Protocol.SelfSystem - власний протокол OpenSCADA, підтримує основні функції.
%description Protocol.SelfSystem -l de_DE.UTF8
Das Paket %{name}-Protocol.SelfSystem - allgemeine Hauptfunktionen für das Systemprotokoll OpenSCADA.
%endif

#===== UI modules =====
%if_enabled VCAEngine
%package UI.VCAEngine
Summary: Visual control area engine.
Group: Graphics
Requires: %name = %version-%release
%description UI.VCAEngine
The %{name}-UI.VCAEngine package - generic visual control area engine.
%description UI.VCAEngine -l ru_RU.UTF8
Пакет %{name}-UI.VCAEngine - общий движок среды визуализации и управления.
%description UI.VCAEngine -l uk_UA.UTF8
Пакет %{name}-UI.VCAEngine - загальний рущій середовища візуалізації та керування.
%description UI.VCAEngine -l de_DE.UTF8
Das Paket %{name}-UI.VCAEngine - allgemeine Visualisierungssteuerung.
%endif

%if_enabled Vision
%package UI.Vision
Summary: Operation user interface (QT).
Group: Graphics
Requires: %name = %version-%release %name-UI.QTStarter
%description UI.Vision
The %{name}-UI.Vision package - visual operation user interface.
%description UI.Vision -l ru_RU.UTF8
Пакет %{name}-UI.Vision - рабочий пользовательский интерфейс.
%description UI.Vision -l uk_UA.UTF8
Пакет %{name}-UI.Vision - робочий інтерфейс користувача.
%description UI.Vision -l de_DE.UTF8
Das Paket %{name}-UI.Vision - benutzerschnittstelle.
%endif

%if_enabled QTStarter
%package UI.QTStarter
Summary: QT GUI starter.
Group: Graphics
Requires: %name = %version-%release
%description UI.QTStarter
The %{name}-UI.QTStarter package QT GUI starter. It is single for all QT GUI modules!
%description UI.QTStarter -l ru_RU.UTF8
Пакет %{name}-UI.QTStarter предоставляет QT GUI пускатель. Он является единственным для всех QT GUI модулей!
%description UI.QTStarter -l uk_UA.UTF8
Пакет %{name}-UI.QTStarter надає QT GUI пускач. Він є один для усіх QT GUI модулів!
%description UI.QTStarter -l de_DE.UTF8
Das Paket %{name}-UI.QTStarter - QT GUI-Starter für die QT GUI Module!
%endif

%if_enabled QTCfg
%package UI.QTCfg
Summary: System configurator (QT).
Group: Graphics
Requires: %name = %version-%release %name-UI.QTStarter
%description UI.QTCfg
The %{name}-UI.QTCfg package allow the QT based OpenSCADA system configurator.
%description UI.QTCfg -l ru_RU.UTF8
Пакет %{name}-UI.QTCfg предоставляет QT основанный конфигуратор системы OpenSCADA.
%description UI.QTCfg -l uk_UA.UTF8
Пакет %{name}-UI.QTCfg надає базований на QT конфігуратор системи OpenSCADA.
%description UI.QTCfg -l de_DE.UTF8
Das Paket %{name}-UI.QTCfg ermöglicht die QT-basierte Systemeinstellung von OpenSCADA.
%endif

%if_enabled WebCfg
%package UI.WebCfg
Summary: System configurator (WEB).
Group: Graphics
Requires: %name = %version-%release %name-Protocol.HTTP
%description UI.WebCfg
The %{name}-UI.WebCfg package allow the WEB based OpenSCADA system configurator.
%description UI.WebCfg -l ru_RU.UTF8
Пакет %{name}-UI.WebCfg предоставляет WEB основанный конфигуратор системы OpenSCADA.
%description UI.WebCfg -l uk_UA.UTF8
Пакет %{name}-UI.WebCfg надає WEB базований конфігуратор системи OpenSCADA.
%description UI.WebCfg -l de_DE.UTF8
Das Paket %{name}-UI.WebCfg ermöglicht die WEB-basierten Systemeinstellungen OpenSCADA.
%endif

%if_enabled WebCfgD
%package UI.WebCfgD
Summary: Dynamic WEB configurator.
Group: Graphics
Requires: %name = %version-%release %name-Protocol.HTTP
%description UI.WebCfgD
The %{name}-UI.WebCfgD package allow the dynamic WEB based OpenSCADA system configurator. Use XHTML, CSS and JavaScript technologies.
%description UI.WebCfgD -l ru_RU.UTF8
Пакет %{name}-UI.WebCfgD предоставляет динамический WEB основанный конфигуратор. Использует XHTML, CSS и JavaScript технологии.
%description UI.WebCfgD -l uk_UA.UTF8
Пакет %{name}-UI.WebCfgD надає динамічний WEB базований конфігуратор. Використано XHTML, CSS та JavaScript технології.
%description UI.WebCfgD -l de_DE.UTF8
Das Paket %{name}-UI.WebCfgD gewährt den dynamischen WEB begründeten Configurator. Benutzt XHTML, CSS und JavaScript Technologien.
%endif

%if_enabled WebVision
%package UI.WebVision
Summary: Operation user interface (WEB).
Group: Graphics
Requires: %name = %version-%release %name-Protocol.HTTP
%description UI.WebVision
The %{name}-UI.WebVision package - web operation user interface for visual control area (VCA) projects playing.
%description UI.WebVision -l ru_RU.UTF8
Пакет %{name}-UI.WebVision - web рабочий пользовательский интерфейс для исполнения визуальных сред управления (СВУ).
%description UI.WebVision -l uk_UA.UTF8
Пакет %{name}-UI.WebVision - web робочий інтерфейс користувача для виконання візуального середовища керування (СВК).
%description UI.WebVision -l de_DE.UTF8
Das Paket %{name}-UI.WebVision - web Benutzerschnittstelle für die Steuerung der sichtbaren Bereiche (VCA).
%endif

#===== Special modules =====
%if_enabled SystemTests
%package Special.SystemTests
Summary: OpenSCADA system's tests.
Group: Graphics
Requires: %name = %version-%release
%description Special.SystemTests
The %{name}-Special.SystemTests package allow the group tests for OpenSCADA system.
%description Special.SystemTests -l ru_RU.UTF8
Пакет %{name}-Special.SystemTests предоставляет группу тестов для системы OpenSCADA.
%description Special.SystemTests -l uk_UA.UTF8
Пакет %{name}-Special.SystemTests надає групу тестів для системи OpenSCADA.
%description Special.SystemTests -l de_DE.UTF8
Das Paket %{name}-Special.SystemTests ermöglicht Gruppentests für das OpenSCADA-System.
%endif

%if_enabled FLibComplex1
%package Special.FLibComplex1
Summary: Complex1 function's lib.
Group: Graphics
Requires: %name = %version-%release
%description Special.FLibComplex1
The %{name}-Special.FLibComplex1 package allow static function library Complex1 (SCADA Complex1 functions).
%description Special.FLibComplex1 -l ru_RU.UTF8
Пакет %{name}-Special.FLibComplex1 предоставляет статическую библиотеку функций Complex1 (функции SCADA Complex1).
%description Special.FLibComplex1 -l uk_UA.UTF8
Пакет %{name}-Special.FLibComplex1 надає статичну бібліотеку функцій Complex1 (функції SCADA Complex1).
%description Special.FLibComplex1 -l de_DE.UTF8
Das Paket %{name}-Special.FLibComplex1 zugriff auf die statische Bibliothek Complex1 (Funktionen SCADA Complex1).
%endif

%if_enabled FLibMath
%package Special.FLibMath
Summary: Math function's lib.
Group: Graphics
Requires: %name = %version-%release
%description Special.FLibMath
The %{name}-Special.FLibMath package allow mathematic static function library.
%description Special.FLibMath -l ru_RU.UTF8
Пакет %{name}-Special.FLibMath предоставляет библиотеку стандартных математические функций.
%description Special.FLibMath -l uk_UA.UTF8
Пакет %{name}-Special.FLibMath надає статичну бібліотеку математичних функцій.
%description Special.FLibMath -l de_DE.UTF8
Das Paket %{name}-Special.FLibMath - bibliothek mit mathematischen Funktionen.
%endif

%if_enabled FLibSYS
%package Special.FLibSYS
Summary: System API functions.
Group: Graphics
Requires: %name = %version-%release
%description Special.FLibSYS
The %{name}-Special.FLibSYS package allow system API functions library of the user programming area.
%description Special.FLibSYS -l ru_RU.UTF8
Пакет %{name}-Special.FLibSYS предоставляет в систему библиотеку системного API среды пользовательского программирования.
%description Special.FLibSYS -l uk_UA.UTF8
Пакет %{name}-Special.FLibSYS надає в систему бібліотеку системного API середовища програмування користувача.
%description Special.FLibSYS -l de_DE.UTF8
Das Paket %{name}-Special.FLibSYS - bibliothek mit System-API für spezifische Programmierung.
%endif

%prep
%setup -q -n %srcname

%build
%configure CFLAGS="-O2" CXXFLAGS="-O2 -Wno-deprecated" \
    %{subst_enable DBF} %{subst_enable SQLite} %{subst_enable MySQL} %{subst_enable FireBird} \
    %{subst_enable System} %{subst_enable BlockCalc} %{subst_enable JavaLikeCalc} %{subst_enable DiamondBoards} \
    %{subst_enable LogicLev} %{subst_enable SNMP} %{subst_enable Siemens} %{subst_enable ModBus} %{subst_enable DCON} \
    %{subst_enable DAQGate} %{subst_enable SoundCard} %{subst_enable ICP_DAS} \
    %{subst_enable FSArch} %{subst_enable DBArch} \
    %{subst_enable Sockets} %{subst_enable SSL} %{subst_enable Serial} \
    %{subst_enable HTTP} %{subst_enable SelfSystem} \
    %{subst_enable VCAEngine} %{subst_enable Vision} %{subst_enable QTStarter} %{subst_enable QTCfg} \
    %{subst_enable WebCfg} %{subst_enable WebCfgD} %{subst_enable WebVision} \
    %{subst_enable SystemTests} %{subst_enable FLibComplex1} %{subst_enable FLibMath} %{subst_enable FLibSYS}
%make

%install
%makeinstall
install -m 755 -d %buildroot/%_includedir/openscada/
install -m 755 -d %buildroot/var/spool/openscada/{DATA,icons,DEMO}
install -m 755 -d %buildroot/var/spool/openscada/ARCHIVES/{MESS,VAL}
install -m 644 *.h %buildroot/%_includedir/openscada
install -m 644 src/*.h %buildroot/%_includedir/openscada
install -m 644 -pD data/oscada.xml %buildroot/%_sysconfdir/oscada.xml
install -m 644 -pD data/oscada_start.xml %buildroot/%_sysconfdir/oscada_start.xml
install -m 755 -pD data/openscada_start %buildroot/%_bindir/openscada_start
install -m 644 -pD data/openscada.desktop %buildroot/%_desktopdir/openscada.desktop
install -m 644 -pD data/openscada.png %buildroot/%_iconsdir/openscada.png
install -m 755 -pD data/oscada.init %buildroot/%_initdir/oscadad
echo "OpenSCADA data dir" > %buildroot/var/spool/openscada/DATA/.info
install -m 644 data/icons/* %buildroot/var/spool/openscada/icons
echo "OpenSCADA messages archive dir" > %buildroot/var/spool/openscada/ARCHIVES/MESS/.info
echo "OpenSCADA values archive dir" > %buildroot/var/spool/openscada/ARCHIVES/VAL/.info
install -m 644 -pD demo/oscada_demo.xml %buildroot/%_sysconfdir/oscada_demo.xml
install -m 755 -pD demo/openscada_demo %buildroot/%_bindir/openscada_demo
install -m 644 -pD demo/openscada_demo.desktop %buildroot/%_desktopdir/openscada_demo.desktop
install -m 644 -pD demo/openscada_demo.png %buildroot/%_iconsdir/openscada_demo.png
install -m 644 demo/*.db %buildroot/var/spool/openscada/DEMO
sed -i 's|/usr/lib|%_libdir|' %buildroot/%_sysconfdir/oscada*.xml

%post
%if %_vendor == "alt"
%post_service oscadad
%else
/sbin/chkconfig --add oscadad
%endif

%preun
%if %_vendor == "alt"
%preun_service oscadad
%else
/sbin/chkconfig --del oscadad
%endif

%clean
#rm -rf %buildroot %buildroot/%name-%version

%files
%defattr(-,root,root)
%config(noreplace) %_sysconfdir/oscada.xml
%config(noreplace) %_sysconfdir/oscada_start.xml
%config %_initdir/oscadad
%_bindir/openscada
%_bindir/openscada_start
%doc README README_ru COPYING ChangeLog INSTALL TODO TODO_ru TODO_uk
%_desktopdir/openscada.desktop
%_iconsdir/openscada.png
%_libdir/*.so.*
#_libdir/openscada/*.so
%exclude %_libdir/openscada/*.a
%exclude %_libdir/openscada/*.la
%_datadir/locale/*/LC_MESSAGES/openscada.mo
/var/spool/openscada/DATA/.info
/var/spool/openscada/icons/*
/var/spool/openscada/ARCHIVES/MESS/.info
/var/spool/openscada/ARCHIVES/VAL/.info

%files doc
%defattr(-,root,root)
%doc doc/*.pdf doc/Modules
#_datadir/doc/*

%files devel
%defattr(-,root,root)
%_libdir/*.so
%_libdir/*.*a
%_includedir/openscada/*

%files demo
%defattr(-,root,root)
%config(noreplace) %_sysconfdir/oscada_demo.xml
%_bindir/openscada_demo
%_desktopdir/openscada_demo.desktop
%_iconsdir/openscada_demo.png
/var/spool/openscada/DEMO/*.db

%files plc

%files server

%files visStation

#===== DB subsystem modules ======
%if_enabled DBF
%files DB.DBF
%_libdir/openscada/bd_DBF.so
%_datadir/locale/*/LC_MESSAGES/oscd_DBF.mo
%endif

%if_enabled SQLite
%files DB.SQLite
%_libdir/openscada/bd_SQLite.so
%_datadir/locale/*/LC_MESSAGES/oscd_SQLite.mo
%endif

%if_enabled MySQL
%files DB.MySQL
%_libdir/openscada/bd_MySQL.so
%_datadir/locale/*/LC_MESSAGES/oscd_MySQL.mo
%endif

%if_enabled FireBird
%files DB.FireBird
%_libdir/openscada/bd_FireBird.so
%_datadir/locale/*/LC_MESSAGES/oscd_FireBird.mo
%endif

#===== DAQ modules =====
%if_enabled System
%files DAQ.System
%_libdir/openscada/daq_System.so
%_datadir/locale/*/LC_MESSAGES/oscd_System.mo
%endif

%if_enabled BlockCalc
%files DAQ.BlockCalc
%_libdir/openscada/daq_BlockCalc.so
%_datadir/locale/*/LC_MESSAGES/oscd_BlockCalc.mo
%endif

%if_enabled JavaLikeCalc
%files DAQ.JavaLikeCalc
%_libdir/openscada/daq_JavaLikeCalc.so
%_datadir/locale/*/LC_MESSAGES/oscd_JavaLikeCalc.mo
%endif

%if_enabled DiamondBoards
%files DAQ.DiamondBoards
%_libdir/openscada/daq_DiamondBoards.so
%_datadir/locale/*/LC_MESSAGES/oscd_DiamondBoards.mo
%endif

%if_enabled LogicLev
%files DAQ.LogicLev
%_libdir/openscada/daq_LogicLev.so
%_datadir/locale/*/LC_MESSAGES/oscd_LogicLev.mo
%endif

%if_enabled SNMP
%files DAQ.SNMP
%_libdir/openscada/daq_SNMP.so
%_datadir/locale/*/LC_MESSAGES/oscd_SNMP.mo
%endif

%if_enabled Siemens
%files DAQ.Siemens
%_libdir/openscada/daq_Siemens.so
%_datadir/locale/*/LC_MESSAGES/oscd_Siemens.mo
%endif

%if_enabled ModBus
%files DAQ.ModBus
%_libdir/openscada/daq_ModBus.so
%_datadir/locale/*/LC_MESSAGES/oscd_ModBus.mo
%endif

%if_enabled DCON
%files DAQ.DCON
%_libdir/openscada/daq_DCON.so
%_datadir/locale/*/LC_MESSAGES/oscd_DCON.mo
%endif

%if_enabled DAQGate
%files DAQ.DAQGate
%_libdir/openscada/daq_DAQGate.so
%_datadir/locale/*/LC_MESSAGES/oscd_DAQGate.mo
%endif

%if_enabled SoundCard
%files DAQ.SoundCard
%_libdir/openscada/daq_SoundCard.so
%_datadir/locale/*/LC_MESSAGES/oscd_SoundCard.mo
%endif

%if_enabled ICP_DAS
%files DAQ.ICP_DAS
%_libdir/openscada/daq_ICP_DAS.so
%_datadir/locale/*/LC_MESSAGES/oscd_ICP_DAS.mo
%endif

#=====  Archive modules =====
%if_enabled FSArch
%files Archive.FSArch
%_libdir/openscada/arh_FSArch.so
%_datadir/locale/*/LC_MESSAGES/oscd_FSArch.mo
%endif

%if_enabled DBArch
%files Archive.DBArch
%_libdir/openscada/arh_DBArch.so
%_datadir/locale/*/LC_MESSAGES/oscd_DBArch.mo
%endif

#===== Transport modules =====
%if_enabled Sockets
%files Transport.Sockets
%_libdir/openscada/tr_Sockets.so
%_datadir/locale/*/LC_MESSAGES/oscd_Sockets.mo
%endif

%if_enabled SSL
%files Transport.SSL
%_libdir/openscada/tr_SSL.so
%_datadir/locale/*/LC_MESSAGES/oscd_SSL.mo
%endif

%if_enabled Serial
%files Transport.Serial
%_libdir/openscada/tr_Serial.so
%_datadir/locale/*/LC_MESSAGES/oscd_Serial.mo
%endif

#===== Transport protocol modules =====
%if_enabled HTTP
%files Protocol.HTTP
%_libdir/openscada/prot_HTTP.so
%_datadir/locale/*/LC_MESSAGES/oscd_HTTP.mo
%endif

%if_enabled SelfSystem
%files Protocol.SelfSystem
%_libdir/openscada/prot_SelfSystem.so
%_datadir/locale/*/LC_MESSAGES/oscd_SelfSystem.mo
%endif

#===== UI modules =====
%if_enabled VCAEngine
%files UI.VCAEngine
%_libdir/openscada/ui_VCAEngine.so
%_datadir/locale/*/LC_MESSAGES/oscd_VCAEngine.mo
%endif

%if_enabled Vision
%files UI.Vision
%_libdir/openscada/ui_Vision.so
%_datadir/locale/*/LC_MESSAGES/oscd_Vision.mo
%endif

%if_enabled QTStarter
%files UI.QTStarter
%_libdir/openscada/ui_QTStarter.so
%_datadir/locale/*/LC_MESSAGES/oscd_QTStarter.mo
%endif

%if_enabled QTCfg
%files UI.QTCfg
%_libdir/openscada/ui_QTCfg.so
%_datadir/locale/*/LC_MESSAGES/oscd_QTCfg.mo
%endif

%if_enabled WebCfg
%files UI.WebCfg
%_libdir/openscada/ui_WebCfg.so
%_datadir/locale/*/LC_MESSAGES/oscd_WebCfg.mo
%endif

%if_enabled WebCfgD
%files UI.WebCfgD
%_libdir/openscada/ui_WebCfgD.so
%_datadir/locale/*/LC_MESSAGES/oscd_WebCfgD.mo
%endif

%if_enabled WebVision
%files UI.WebVision
%_libdir/openscada/ui_WebVision.so
%_datadir/locale/*/LC_MESSAGES/oscd_WebVision.mo
%endif

#===== Special modules =====
%if_enabled SystemTests
%files Special.SystemTests
%_libdir/openscada/spec_SystemTests.so
%_datadir/locale/*/LC_MESSAGES/oscd_SystemTests.mo
%endif

%if_enabled FLibComplex1
%files Special.FLibComplex1
%_libdir/openscada/spec_FLibComplex1.so
%_datadir/locale/*/LC_MESSAGES/oscd_FLibComplex1.mo
%endif

%if_enabled FLibMath
%files Special.FLibMath
%_libdir/openscada/spec_FLibMath.so
%_datadir/locale/*/LC_MESSAGES/oscd_FLibMath.mo
%endif

%if_enabled FLibSYS
%files Special.FLibSYS
%_libdir/openscada/spec_FLibSYS.so
%_datadir/locale/*/LC_MESSAGES/oscd_FLibSYS.mo
%endif


%changelog
* Sun Oct 04 2009 Roman Savochenko <rom_as@oscada.org>
- Move modules to separated packages

* Mon Jun 08 2009 Roman Savochenko <rom_as@oscada.org.ua>
- Build 0.6.3.3 release.

* Mon May 25 2009 Roman Savochenko <rom_as@diyaorg.dp.ua>
- Merge demo DB from different languages to one multilanguage (Russian,English and Ukrainian) DB.

* Wed Mar 18 2009 Roman Savochenko <rom_as@diyaorg.dp.ua>
- OpenSCADA update 0.6.3.2 release build.

* Mon Feb 2 2009 Roman Savochenko <rom_as@diyaorg.dp.ua>
- OpenSCADA update 0.6.3.1 release build.

* Mon Dec 22 2008 Roman Savochenko <rom_as@diyaorg.dp.ua>
- Documentation pack is unified and separated to project info files and documentation.
- Dynamic librarie's links packing into main package and development is fixed.

* Thu Oct 02 2008 Roman Savochenko <rom_as@fromru.com>
- Package name simple changing allow is added.

* Thu Sep 18 2008 Roman Savochenko <rom_as@diyaorg.dp.ua>
- Update spec to build for ALTLinux Sisyphus.

* Wed Mar 26 2008 Roman Savochenko <rom_as@diyaorg.dp.ua>
- Rebuilded for support x86_64 several distributives and some build system bugs is fixed.

* Sat Mar 23 2008 Roman Savochenko <rom_as@diyaorg.dp.ua>
- menu files included

* Fri Sep 02 2005 Roman Savochenko <rom_as@fromru.com>
- replace testdate whith demo package
- rename xinetd script from openscada to oscadad
- add xinetd script to generic package

* Wed Mar 16 2005 Roman Savochenko <rom_as@fromru.com>
- add Athena board specific build

* Wed Nov 03 2004 Roman Savochenko <rom_as@fromru.com>
- move the message arhives data to /var/spool/openscada/ARHIVE/MESS

* Tue Apr 06 2004 Roman Savochenko <rom_as@fromru.com>
- make 3 packages: OpenScada, OpenScada-devel, OpenScada-testdata
- add languages: ru, uk
- make packages from 'make -dist' package;

* Thu Oct 15 2003 Roman Savochenko <rom_as@fromru.com>
- Starting
