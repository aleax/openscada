#===== Generic Info ======
Summary: Open SCADA system
Summary(ru_RU.UTF8): Открытая SCADA система
Summary(uk_UA.UTF8): Відкрита SCADA система
Summary(de_DE.UTF8): Open SCADA-System
Name: openscada
Version: 1+r0000
Release: 1
Source: openscada-%version.tar.xz
Source1: openscada-res-%version.tar.xz
License: GPLv2
Group: Applications/Engineering
Packager: Roman Savochenko <roman@oscada.org>
URL: http://oscada.org

%define srcname openscada-%version

#= Individual distributives seting =
%if %_vendor == "alt"
%set_verify_elf_method no
BuildRequires: glibc-devel gcc-c++ libpcre-devel libgd2-devel sqlite3
BuildRequires: libMySQL-devel libsqlite3-devel firebird-devel postgresql-devel
BuildRequires: libsensors3-devel libnet-snmp-devel libportaudio2-devel libcomedi-devel libqt4-devel libfftw3-devel
%else
%define _initdir /etc/init.d
%define _desktopdir %_datadir/applications
%define _iconsdir /usr/share/icons
BuildRoot: %_tmppath/%name-%version-root
%endif

Requires: %name-Archive.DBArch %name-Archive.FSArch
Requires: %name-DAQ.BlockCalc %name-DAQ.DAQGate %name-DAQ.DCON %name-DAQ.JavaLikeCalc %name-DAQ.LogicLev %name-DAQ.ModBus
Requires: %name-DAQ.OPC_UA %name-DAQ.SNMP %name-DAQ.Siemens %name-DAQ.SoundCard %name-DAQ.System
Requires: %name-DB.SQLite %name-DB.MySQL
Requires: %name-Transport.Sockets %name-Transport.SSL %name-Transport.Serial %name-Protocol.SelfSystem %name-Protocol.HTTP %name-Protocol.UserProtocol
Requires: %name-UI.QTStarter %name-UI.QTCfg %name-UI.VCAEngine %name-UI.Vision %name-UI.WebVision %name-UI.WebCfgD %name-UI.WebUser
Requires: %name-Special.FLibComplex1 %name-Special.FLibSYS %name-Special.FLibMath

%description
Open SCADA system. Typical installation.
%description -l ru_RU.UTF8
Открытая SCADA система. Типичная установка.
%description -l uk_UA.UTF8
Відкрита SCADA система. Типове встановлення.
%description -l de_DE.UTF8
Das offene SCADA System. Typische Installation.

#===== DB subsystem modules ======
%def_enable DBF
%def_enable SQLite
%def_enable MySQL
%def_enable FireBird
%def_enable PostgreSQL

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
%def_enable OPC_UA
%def_enable BFN
%ifnarch %ix86
%force_disable DiamondBoards
%force_disable ICP_DAS
%else
%def_enable ICP_DAS
%def_enable DiamondBoards
%endif
%def_enable Comedi
%def_enable AMRDevs
%def_disable MMS

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
%def_enable UserProtocol

#===== UI modules =====
%def_enable VCAEngine
%def_enable Vision
%def_enable QTStarter
%def_enable QTCfg
%def_enable WebCfg
%def_enable WebCfgD
%def_enable WebVision
%def_enable WebUser

#===== Special modules =====
%def_enable SystemTests
%def_enable FLibComplex1
%def_enable FLibMath
%def_enable FLibSYS

%package core
Summary: Open SCADA system core
Summary(ru_RU.UTF8): Ядро открытой SCADA системы
Summary(uk_UA.UTF8): Ядро відкритої SCADA системи
Summary(de_DE.UTF8): Open SCADA-System Kern
%if %_vendor == "alt"
AutoReq: noshell
%endif
Group: Graphics
%description core
Open SCADA system core.
%description core -l ru_RU.UTF8
Ядро открытой SCADA системы.
%description core -l uk_UA.UTF8
Ядро відкритої SCADA системи.
%description core -l de_DE.UTF8
Das offene SCADA System Core.

%package docEN
Summary: Open SCADA documents (EN)
Summary(ru_RU.UTF8): Документация открытой SCADA (EN)
Summary(uk_UA.UTF8): Документація відкритої SCADA (EN)
Summary(de_DE.UTF8): Open SCADA Dokumente (EN)
Group: Documentation
BuildArch: noarch
%description docEN
The %name-docEN package include documents files (English).
%description docEN -l ru_RU.UTF8
Пакет %name-docEN включает файлы документации (Английский).
%description docEN -l uk_UA.UTF8
Пакет %name-docEN включає файли документації (Англійська).
%description docEN -l de_DE.UTF8
Das Paket %name-docEN enthaelt Dokumentationsdateien (Englisch).

%package docRU
Summary: Open SCADA documents (RU)
Summary(ru_RU.UTF8): Документация открытой SCADA (RU)
Summary(uk_UA.UTF8): Документація відкритої SCADA (RU)
Summary(de_DE.UTF8): Open SCADA Dokumente (RU)
Group: Documentation
BuildArch: noarch
Requires: %name-docEN
%description docRU
The %name-docRU package include documents files (Russian).
%description docRU -l ru_RU.UTF8
Пакет %name-docRU включает файлы документации (Русский).
%description docRU -l uk_UA.UTF8
Пакет %name-docRU включає файли документації (Російська).
%description docRU -l de_DE.UTF8
Das Paket %name-docRU enthaelt Dokumentationsdateien (Russisch).

%package docUK
Summary: Open SCADA documents (UK)
Summary(ru_RU.UTF8): Документация открытой SCADA (UK)
Summary(uk_UA.UTF8): Документація відкритої SCADA (UK)
Summary(de_DE.UTF8): Open SCADA Dokumente (UK)
Group: Documentation
BuildArch: noarch
Requires: %name-docEN
%description docUK
The %name-docUK package include documents files (Ukraine).
%description docUK -l ru_RU.UTF8
Пакет %name-docUK включает файлы документации (Украинский).
%description docUK -l uk_UA.UTF8
Пакет %name-docUK включає файли документації (Українська).
%description docUK -l de_DE.UTF8
Das Paket %name-docUK enthaelt Dokumentationsdateien (Ukrainisch).

%package devel
Summary: Open SCADA development
Summary(ru_RU.UTF8): Разработка открытой SCADA
Summary(uk_UA.UTF8): Розробка відкритої SCADA
Summary(de_DE.UTF8): Open SCADA Entwicklung
Group: Development/C++
Requires: %name-core = %version-%release
%description devel
The %name-devel package - includes library archives and include files.
%description devel -l ru_RU.UTF8
Пакет %name-devel - включает архив библиотек и включаемые файлы.
%description devel -l uk_UA.UTF8
Пакет %name-devel - включає архів бібліотек та включаємі файли.
%description devel -l de_DE.UTF8
Das Paket %name-devel enthaelt die Bibliotheken und Archive Include-Dateien.

%package LibDB.Main
Summary: Main OpenSCADA libraries for DAQ and other into SQLite DB
Summary(ru_RU.UTF8): Основные библиотеки OpenSCADA для сбора данных и другого в БД SQLite
Summary(uk_UA.UTF8): Основні бібліотеки OpenSCADA для збору даних та іншого у БД SQLite
Summary(de_DE.UTF8): Hauptbibliothek OpenSCADA für die Datenerhebung und die anderen in der Datenbank SQLite
Group: Graphics
Requires: %name-DB.SQLite %name-DAQ.JavaLikeCalc %name-DAQ.LogicLev %name-Special.FLibComplex1 %name-Special.FLibSYS
BuildArch: noarch
%description LibDB.Main
The %{name}-LibDB.Main package includes main OpenSCADA libraries into SQLite DB.
For use connect SQLite DB file LibsDB/OscadaLibs.db.
%description LibDB.Main -l ru_RU.UTF8
Пакет %{name}-LibDB.Main включает основные библиотеки OpenSCADA в БД SQLite.
Для использования подключите файл БД SQLite LibsDB/OscadaLibs.db.
%description LibDB.Main -l uk_UA.UTF8
Пакет %{name}-LibDB.Main включає основні бібліотеки OpenSCADA у БД SQLite.
Для використання підключіть файл БД SQLite LibsDB/OscadaLibs.db.
%description LibDB.Main -l de_DE.UTF8
Das Paket %{name}-LibDB.Main enthaelt die Hauptbibliothek OpenSCADA die Datenbank SQLite.
So verwenden, verbinden Sie die Datei Datenbank SQLite LibsDB/OscadaLibs.db.

%package LibDB.VCA
Summary: Visual components libraries into SQLite DB
Summary(ru_RU.UTF8): Библиотеки визуальных компонетов в БД SQLite
Summary(uk_UA.UTF8): Бібліотеки візуальних компонентів у БД SQLite
Summary(de_DE.UTF8): Visuelle Komponente in einer Bibliothek Datenbank SQLite
Group: Graphics
Requires: %name-DB.SQLite %name-DAQ.JavaLikeCalc %name-Special.FLibSYS %name-UI.VCAEngine
BuildArch: noarch
%description LibDB.VCA
The %{name}-LibDB.VCA package includes visual components libraries into SQLite DB.
For use connect SQLite DB file LibsDB/vcaBase.db and LibsDB/vcaTest.db.
%description LibDB.VCA -l ru_RU.UTF8
Пакет %{name}-LibDB.VCA включает библиотеки визуальных компонетов в БД SQLite.
Для использования подключите файл БД SQLite LibsDB/vcaBase.db и LibsDB/vcaTest.db.
%description LibDB.VCA -l uk_UA.UTF8
Пакет %{name}-LibDB.VCA включає бібліотеки візуальних компонентів у БД SQLite.
Для використання підключіть файл БД SQLite LibsDB/vcaBase.db та LibsDB/vcaTest.db.
%description LibDB.VCA -l de_DE.UTF8
Das Paket %{name}-LibDB.VCA enthaelt Visuelle Komponente in einer Bibliothek Datenbank SQLite.
So verwenden, verbinden Sie die Datei Datenbank SQLite LibsDB/vcaBase.db und LibsDB/vcaTest.db.

%package Model.AGLKS
Summary: Model "AGLKS" data bases and config (Demo: EN,UK,RU)
Summary(ru_RU.UTF8): БД и конфигурация модели "АГЛКС" (Демо: EN,UK,RU)
Summary(uk_UA.UTF8): БД та конфігурація моделі "АГЛКС" (Демо: EN,UK,RU)
Summary(de_DE.UTF8): Datenbanken und Konfigurationsdateien Modell "AGLKS" (Demo: EN,UK,RU)
Group: Graphics
%if %_vendor == "alt"
AutoReq: noshell
%endif
BuildArch: noarch
Requires: %name-LibDB.Main %name-LibDB.VCA
Requires: %name-Transport.Sockets %name-Transport.SSL %name-Transport.Serial %name-Protocol.HTTP %name-Protocol.SelfSystem %name-Protocol.UserProtocol
Requires: %name-DAQ.BlockCalc %name-DAQ.ModBus %name-DAQ.System %name-Archive.FSArch
Requires: %name-UI.QTStarter %name-UI.QTCfg %name-UI.Vision %name-UI.WebCfgD %name-UI.WebVision
Requires: %name-Special.FLibMath
%description Model.AGLKS
The %{name}-Model.AGLKS package includes model "AGLKS" data bases and config.
The Model is used for OpenSCADA demo and allowed for English, Ukrainian and Russian languages.
For start use command <openscada_AGLKS>.
%description Model.AGLKS -l ru_RU.UTF8
Пакет %{name}-Model.AGLKS включает БД и конфигурацию модели "АГЛКС".
Модель используется для демонстрации OpenSCADA и доступна на Английском, Украинском и Российском языках.
Для старта используйте команду <openscada_AGLKS>.
%description Model.AGLKS -l uk_UA.UTF8
Пакет %{name}-Model.AGLKS включає БД та конфігурацію моделі "АГЛКС".
Модель використано для демонстрації OpenSCADA та доступно на Англійській, Українській та Російській мовах.
Для старту використовуйте команду <openscada_AGLKS>.
%description Model.AGLKS -l de_DE.UTF8
Das Paket %{name}-Model.AGLKS enthaelt Datenbanken und Konfigurationsdateien Modell "AGLKS".
Das Modell wird verwendet, um OpenSCADA demonstrieren und ist verfügbar in Englisch, Ukrainisch und Russisch.
Fuers Starten wird Kommando <openscada_AGLKS> benutzt.

%package Model.Boiler
Summary: Model "Boiler" data bases and config (EN,UK,RU)
Summary(ru_RU.UTF8): БД и конфигурация модели "Котёл" (EN,UK,RU)
Summary(uk_UA.UTF8): БД та конфігурація моделі "Котел" (EN,UK,RU)
Summary(de_DE.UTF8): Datenbanken und Konfigurationsdateien Modell "Kessel" (EN,UK,RU)
Group: Graphics
%if %_vendor == "alt"
AutoReq: noshell
%endif
BuildArch: noarch
Requires: %name-LibDB.Main %name-LibDB.VCA
Requires: %name-DAQ.BlockCalc %name-DAQ.System %name-Archive.FSArch
Requires: %name-UI.QTCfg %name-UI.QTStarter %name-UI.Vision %name-UI.WebCfgD %name-UI.WebVision
Requires: %name-Special.FLibMath
%description Model.Boiler
The %{name}-Model.Boiler package model "Boiler" data bases and config.
The Model is allowed for English, Ukrainian and Russian languages.
For start use command <openscada_Boiler>.
%description Model.Boiler -l ru_RU.UTF8
Пакет %{name}-Model.Boiler включает БД и конфигурацию модели "Котёл".
Модель доступна доступна на Английском, Украинском и Российском языках.
Для старта используйте команду <openscada_Boiler>.
%description Model.Boiler -l uk_UA.UTF8
Пакет %{name}-Model.Boiler включає БД та конфігурацію моделі "Котел".
Модель доступно на Англійській, Українській та Російській мовах.
Для старту використовуйте команду <openscada_Boiler>.
%description Model.Boiler -l de_DE.UTF8
Das Paket %{name}-Model.Boiler enthaelt Datenbanken und Konfigurationsdateien Modell "Kessel".
Das Modell ist verfügbar in Englisch, Ukrainisch und Russisch.
Fuers Starten wird Kommando <openscada_Boiler> benutzt.

%package plc
Summary: OpenSCADA - PLC virtual package
Summary(ru_RU.UTF8): Виртуальный пакет OpenSCADA - ПЛК
Summary(uk_UA.UTF8): Віртуальний пакет OpenSCADA - ПЛК
Summary(de_DE.UTF8): OpenSCADA - SPS virtuelles Paket
Group: Graphics
BuildArch: noarch
Requires: %name-DB.SQLite
Requires: %name-Transport.Serial %name-Transport.Sockets %name-Transport.SSL %name-Protocol.HTTP %name-Protocol.SelfSystem %name-Protocol.UserProtocol
Requires: %name-DAQ.JavaLikeCalc %name-DAQ.BlockCalc %name-DAQ.LogicLev %name-DAQ.ModBus %name-DAQ.System %name-DAQ.DCON %name-Archive.FSArch
Requires: %name-UI.VCAEngine %name-UI.WebCfgD %name-UI.WebVision %name-Special.FLibComplex1 %name-Special.FLibMath %name-Special.FLibSYS
%description plc
The %name-plc is virtual package for PLC.
%description plc -l ru_RU.UTF8
Пакет %name-plc это виртуальный пакет для ПЛК.
%description plc -l uk_UA.UTF8
Пакет %name-plc це віртуальний пакет для ПЛК.
%description plc -l de_DE.UTF8
Das Paket %name-plc ist ein virtuelles Paket für SPS.

%package server
Summary: OpenSCADA - server virtual package
Summary(ru_RU.UTF8): Виртуальный пакет OpenSCADA - сервер
Summary(uk_UA.UTF8): Віртуальний пакет OpenSCADA - сервер
Summary(de_DE.UTF8): OpenSCADA - Server virtuelles Paket
Group: Graphics
BuildArch: noarch
Requires: %name-DB.SQLite %name-DB.MySQL %name-DB.FireBird %name-DB.PostgreSQL
Requires: %name-DAQ.System %name-DAQ.BlockCalc %name-DAQ.JavaLikeCalc
Requires: %name-DAQ.LogicLev %name-DAQ.SNMP %name-DAQ.Siemens %name-DAQ.ModBus %name-DAQ.DCON %name-DAQ.DAQGate %name-DAQ.SoundCard
Requires: %name-Archive.FSArch %name-Archive.DBArch
Requires: %name-Transport.Sockets %name-Transport.SSL %name-Transport.Serial %name-Protocol.HTTP %name-Protocol.SelfSystem
Requires: %name-UI.VCAEngine %name-UI.WebCfg %name-UI.WebCfgD %name-UI.WebVision
Requires: %name-Special.FLibComplex1 %name-Special.FLibMath %name-Special.FLibSYS
%description server
The %name-server is virtual package for SCADA-server.
%description server -l ru_RU.UTF8
Пакет %name-server это виртуальный пакет для сервера SCADA.
%description server -l uk_UA.UTF8
Пакет %name-server це віртуальний пакет для сервера SCADA.
%description server -l de_DE.UTF8
Das Paket %name-server ist ein virtuelles Paket für die Server-SCADA.

%package visStation
Summary: OpenSCADA - visual station virtual package
Summary(ru_RU.UTF8): Виртуальный пакет OpenSCADA - визуальная станция
Summary(uk_UA.UTF8): Віртуальний пакет OpenSCADA - візуальна станція
Summary(de_DE.UTF8): OpenSCADA - visuelle Station virtuelles Paket
Group: Graphics
BuildArch: noarch
Requires: %name-DB.SQLite %name-DB.MySQL
Requires: %name-DAQ.System %name-DAQ.BlockCalc %name-DAQ.JavaLikeCalc %name-DAQ.LogicLev %name-DAQ.SNMP %name-DAQ.Siemens
Requires: %name-DAQ.ModBus %name-DAQ.DCON %name-DAQ.DAQGate %name-DAQ.SoundCard
Requires: %name-Archive.FSArch %name-Archive.DBArch
Requires: %name-Transport.Sockets %name-Transport.SSL %name-Transport.Serial %name-Protocol.SelfSystem
Requires: %name-UI.VCAEngine %name-UI.Vision %name-UI.QTStarter %name-UI.QTCfg
Requires: %name-Special.FLibComplex1 %name-Special.FLibMath %name-Special.FLibSYS
%description visStation
The %name-visStation is virtual package for visual station (SCADA).
%description visStation -l ru_RU.UTF8
Пакет %name-visStation это виртуальный пакет для визуальной станции (SCADA).
%description visStation -l uk_UA.UTF8
Пакет %name-visStation це віртуальний пакет для сервера візуальної станції (SCADA).
%description visStation -l de_DE.UTF8
Das Paket %name-visStation ist ein virtuelles Paket für visuelle Station (SCADA).

#===== DB subsystem modules ======
%if_enabled DBF
%package DB.DBF
Summary: DB DBF support
Summary(ru_RU.UTF8): Поддержка БД DBF
Summary(uk_UA.UTF8): Підтримка БД DBF
Summary(de_DE.UTF8): DB DBF unterstützen
Group: Graphics
Requires: %name-core = %version-%release
%description DB.DBF
The %{name}-DB.DBF package - provides support of the *.dbf files, version 3.0.
%description DB.DBF -l ru_RU.UTF8
Пакет %{name}-DB.DBF - предоставляет поддержку *.dbf файлов, версии 3.0.
%description DB.DBF -l uk_UA.UTF8
Пакет %{name}-DB.DBF - надає підтримку *.dbf файлів, версії 3.0.
%description DB.DBF -l de_DE.UTF8
Das Paket %{name}-DB.DBF - unterstützt die *.dbf Datenbank Version 3.0.
%endif

%if_enabled SQLite
%package DB.SQLite
Summary: DB SQLite support
Summary(ru_RU.UTF8): Поддержка БД SQLite
Summary(uk_UA.UTF8): Підтримка БД SQLite
Summary(de_DE.UTF8): DB SQLite unterstützen
Group: Graphics
Requires: %name-core = %version-%release
%description DB.SQLite
The %{name}-DB.SQLite package - provides support of the BD SQLite.
%description DB.SQLite -l ru_RU.UTF8
Пакет %{name}-DB.SQLite - предоставляет поддержку БД SQLite.
%description DB.SQLite -l uk_UA.UTF8
Пакет %{name}-DB.SQLite - надає підтримку БД SQLite.
%description DB.SQLite -l de_DE.UTF8
Das Paket %{name}-DB.SQLite - unterstützt die SQLite Datenbank.
%endif

%if_enabled MySQL
%package DB.MySQL
Summary: DB MySQL support
Summary(ru_RU.UTF8): Поддержка БД MySQL
Summary(uk_UA.UTF8): Підтримка БД MySQL
Summary(de_DE.UTF8): DB MySQL unterstützen
Group: Graphics
Requires: %name-core = %version-%release
%description DB.MySQL
The %{name}-DB.MySQL package - provides support of the BD MySQL.
%description DB.MySQL -l ru_RU.UTF8
Пакет %{name}-DB.MySQL - предоставляет поддержку БД MySQL.
%description DB.MySQL -l uk_UA.UTF8
Пакет %{name}-DB.MySQL - надає підтримку БД MySQL.
%description DB.MySQL -l de_DE.UTF8
Das Paket %{name}-DB.MySQL - unterstützt die MySQL Datenbank.
%endif

%if_enabled FireBird
%package DB.FireBird
Summary: DB FireBird support
Summary(ru_RU.UTF8): Поддержка БД FireBird
Summary(uk_UA.UTF8): Підтримка БД FireBird
Summary(de_DE.UTF8): DB FireBird unterstützen
Group: Graphics
Requires: %name-core = %version-%release
%description DB.FireBird
The %{name}-DB.FireBird package - provides support of the DB FireBird.
%description DB.FireBird -l ru_RU.UTF8
Пакет %{name}-DB.FireBird - предоставляет поддержку БД FireBird.
%description DB.FireBird -l uk_UA.UTF8
Пакет %{name}-DB.FireBird - надає підтримку БД FireBird.
%description DB.FireBird -l de_DE.UTF8
Das Paket %{name}-DB.FireBird - unterstützt die FireBird Datenbank.
%endif

%if_enabled PostgreSQL
%package DB.PostgreSQL
Summary: DB PostgreSQL support
Summary(ru_RU.UTF8): Поддержка БД PostgreSQL
Summary(uk_UA.UTF8): Підтримка БД PostgreSQL
Summary(de_DE.UTF8): DB PostgreSQL unterstützen
Group: Graphics
Requires: %name-core = %version-%release
%description DB.PostgreSQL
The %{name}-DB.PostgreSQL package - provides support of the DB PostgreSQL.
%description DB.PostgreSQL -l ru_RU.UTF8
Пакет %{name}-DB.PostgreSQL - предоставляет поддержку БД PostgreSQL.
%description DB.PostgreSQL -l uk_UA.UTF8
Пакет %{name}-DB.PostgreSQL - надає підтримку БД PostgreSQL.
%description DB.PostgreSQL -l de_DE.UTF8
Das Paket %{name}-DB.PostgreSQL - unterstützt die PostgreSQL Datenbank.
%endif

#===== DAQ subsystem modules =====
%if_enabled System
%package DAQ.System
Summary: System DA
Summary(ru_RU.UTF8): Источник данных "Система"
Summary(uk_UA.UTF8): Джерело даних "Система"
Summary(de_DE.UTF8): Source Data "Systems"
Group: Graphics
Requires: %name-core = %version-%release
%description DAQ.System
The %{name}-DAQ.System package - provides data acquisition from the OS.
Supported OS Linux data sources: HDDTemp, Sensors, Uptime, Memory, CPU, UPS etc.
%description DAQ.System -l ru_RU.UTF8
Пакет %{name}-DAQ.System - предоставляет сбор данных операционной системы.
Поддерживаются источники данных ОС Linux: HDDTemp, Sensors, Uptime, Memory, CPU, ИБП и другие.
%description DAQ.System -l uk_UA.UTF8
Пакет %{name}-DAQ.System - надає збір даних операційної системи.
Підтримуються джерела даних ОС Linux: HDDTemp, Sensors, Uptime, Memory, CPU, ДБЖ та інше.
%description DAQ.System -l de_DE.UTF8
Das Paket %{name}-DAQ.System - emöglicht die Datenerfassung des Betriebssystems.
Unterstützt werden die ОС Linux Datenquellen: HDDTemp, Sensors, Uptime, Memory, CPU, UPS und andere.
%endif

%if_enabled BlockCalc
%package DAQ.BlockCalc
Summary: Block based calculator
Summary(ru_RU.UTF8): Блочный вычислитель
Summary(uk_UA.UTF8): Блоковий обчислювач
Summary(de_DE.UTF8): Block-Rechner
Group: Graphics
Requires: %name-core = %version-%release
%description DAQ.BlockCalc
The %{name}-DB.DAQ.BlockCalc package - provides block based calculator.
%description DAQ.BlockCalc -l ru_RU.UTF8
Пакет %{name}-DAQ.BlockCalc - предоставляет блочный вычислитель.
%description DAQ.BlockCalc -l uk_UA.UTF8
Пакет %{name}-DAQ.BlockCalc - надає блоковий обчислювач.
%description DAQ.BlockCalc -l de_DE.UTF8
Das Paket %{name}-DAQ.BlockCalc - erlaubt Berechnungen mit dem Blockrechner.
%endif

%if_enabled JavaLikeCalc
%package DAQ.JavaLikeCalc
Summary: Java-like based calculator
Summary(ru_RU.UTF8): Вычислитель, основанный на Java-подобном языке
Summary(uk_UA.UTF8): Обчислювач, який засновано на подібній до Java мові
Summary(de_DE.UTF8): Java-basierter Rechner wie
Group: Graphics
Requires: %name-core = %version-%release
%description DAQ.JavaLikeCalc
The %{name}-DAQ.JavaLikeCalc package - provides based on java like language calculator and engine of libraries.
The user can create and modify functions and libraries.
%description DAQ.JavaLikeCalc -l ru_RU.UTF8
Пакет %{name}-DAQ.JavaLikeCalc - предоставляет основанные на java подобном языке вычислитель и движок библиотек.
Пользователь может создавать и модифицировать функции и библиотеки.
%description DAQ.JavaLikeCalc -l uk_UA.UTF8
Пакет %{name}-DAQ.JavaLikeCalc - надає базовані на мові схожій на Java обчислювач та движок бібліотек функцї.
Користувач може створювати та модифікувати функції та бібліотеки.
%description DAQ.JavaLikeCalc -l de_DE.UTF8
Das Paket %{name}-DAQ.JavaLikeCalc - java-ähnlicher Rechner und Bibliotheken.
Der Benutzer kann Funktionen und Bibliotheken erstellen und modifizieren.
%endif

%if_enabled DiamondBoards
%package DAQ.DiamondBoards
Summary: Diamond DA boards
Summary(ru_RU.UTF8): Платы сбора данных Diamond
Summary(uk_UA.UTF8): Плати збору даних Diamond
Summary(de_DE.UTF8): Diamond DA-Boards
Group: Graphics
Requires: %name-core = %version-%release
%description DAQ.DiamondBoards
The %{name}-DAQ.DiamondBoards package - provides an access to "Diamond Systems" DAQ boards.
Includes main support for all generic boards.
%description DAQ.DiamondBoards -l ru_RU.UTF8
Пакет %{name}-DAQ.DiamondBoards - предоставляет доступ к платам сбора данных фирмы Diamond systems.
Включает общую поддержку всех основных плат.
%description DAQ.DiamondBoards -l uk_UA.UTF8
Пакет %{name}-DAQ.DiamondBoards - надає доступ до плат збору даних фірми Diamond systems.
Включає загальну підтримку всіх основних плат.
%description DAQ.DiamondBoards -l de_DE.UTF8
Das Paket %{name}-DAQ.DiamondBoards - bietet Zugriff auf Diamant Systeme DA-Boards.
Inklusive Haupt Unterstützung für alle generischen Platten.
%endif

%if_enabled LogicLev
%package DAQ.LogicLev
Summary: Logic level
Summary(ru_RU.UTF8): Логический уровень
Summary(uk_UA.UTF8): Логічний рівень
Summary(de_DE.UTF8): Logikpegel
Group: Graphics
Requires: %name-core = %version-%release
%description DAQ.LogicLev
The %{name}-DAQ.LogicLev package - provides the logical level of parameters.
%description DAQ.LogicLev -l ru_RU.UTF8
Пакет %{name}-DAQ.LogicLev - предоставляет логический уровень параметров.
%description DAQ.LogicLev -l uk_UA.UTF8
Пакет %{name}-DAQ.LogicLev - надає логічний рівень параметрів.
%description DAQ.LogicLev -l de_DE.UTF8
Das Paket %{name}-DAQ.LogicLev - ermöglicht Parameter der logischen Stufe.
%endif

%if_enabled SNMP
%package DAQ.SNMP
Summary: SNMP client
Summary(ru_RU.UTF8): Клиент SNMP
Summary(uk_UA.UTF8): Клієнт SNMP
Summary(de_DE.UTF8): SNMP-Client
Group: Graphics
Requires: %name-core = %version-%release
%description DAQ.SNMP
The %{name}-DAQ.SNMP package - provides an implementation of the client of SNMP-service.
%description DAQ.SNMP -l ru_RU.UTF8
Пакет %{name}-DAQ.SNMP - предоставляет реализацию клиентского сервиса протокола SNMP.
%description DAQ.SNMP -l uk_UA.UTF8
Пакет %{name}-DAQ.SNMP - надає реалізацію клієнтського SNMP сервісу.
%description DAQ.SNMP -l de_DE.UTF8
Das Paket %{name}-DAQ.SNMP - anbindung eines Clients für das SNMP-Protokoll.
%endif

%if_enabled Siemens
%package DAQ.Siemens
Summary: Siemens DAQ
Summary(ru_RU.UTF8): Источник данных Siemens
Summary(uk_UA.UTF8): Джерело даних Siemens
Summary(de_DE.UTF8): Siemens DAQ
Group: Graphics
Requires: %name-core = %version-%release
%description DAQ.Siemens
The %{name}-DAQ.Siemens package - provides a data source PLC Siemens by means of Hilscher CIF cards,
by using the MPI protocol, and Libnodave library for the rest.
%description DAQ.Siemens -l ru_RU.UTF8
Пакет %{name}-DAQ.Siemens - предоставляет источник данных ПЛК Siemens посредством
карт Hilscher CIF с использованием протокола MPI и библиотеки Libnodave для остального.
%description DAQ.Siemens -l uk_UA.UTF8
Пакет %{name}-DAQ.Siemens - надає джерело даних ПЛК Siemens за допомогою
карт Hilscher CIF з використанням протоколу MPI та бібліотеки Libnodave для іншого.
%description DAQ.Siemens -l de_DE.UTF8
Das Paket %{name}-DAQ.Siemens - ermöglicht Datenquelle Siemens PLC-Karten mit Hilscher CIF.
Für Andere werden MPI-Protokoll und Bibliothek Libnodave verwendet.
%endif

%if_enabled ModBus
%package DAQ.ModBus
Summary: ModBus protocol and DAQ
Summary(ru_RU.UTF8): ModBus протокол и сбор данных
Summary(uk_UA.UTF8): ModBus протокол та збір даних
Summary(de_DE.UTF8): MODBUS-Protokoll und DAQ
Group: Graphics
Requires: %name-core = %version-%release
%description DAQ.ModBus
The %{name}-DAQ.ModBus package - provides realisation of ModBus client service.
Supported Modbus/TCP, Modbus/RTU and Modbus/ASCII protocols.
%description DAQ.ModBus -l ru_RU.UTF8
Пакет %{name}-DAQ.ModBus - предоставляет реализацию клиентского сервиса протокола ModBus.
Поддерживаются Modbus/TCP, Modbus/RTU и Modbus/ASCII протоколы.
%description DAQ.ModBus -l uk_UA.UTF8
Пакет %{name}-DAQ.ModBus - надає реалізацію клієнтського ModBus сервісу.
Підтримуються Modbus/TCP, Modbus/RTU та Modbus/ASCII протоколи.
%description DAQ.ModBus -l de_DE.UTF8
Das Paket %{name}-DAQ.ModBus - ermöglicht die Realisierung des Klientservices des ModBus-Protokolls.
Unterstützt werden Modbus/TCP, Modbus/RTU und Modbus/ASCII Protokolle.
%endif

%if_enabled DCON
%package DAQ.DCON
Summary: DCON client
Summary(ru_RU.UTF8): Клиент DCON
Summary(uk_UA.UTF8): Клієнт DCON
Summary(de_DE.UTF8): DCON Client
Group: Graphics
Requires: %name-core = %version-%release
%description DAQ.DCON
The %{name}-DAQ.DCON package - provides an implementation of DCON-client protocol.
Supports I-7000 DCON protocol.
%description DAQ.DCON -l ru_RU.UTF8
Пакет %{name}-DAQ.DCON - предоставляет реализацию клиентского сервиса протокола DCON.
Поддерживается протокол I-7000 DCON.
%description DAQ.DCON -l uk_UA.UTF8
Пакет %{name}-DAQ.DCON - надає реалізацію клієнтського сервісу DCON.
Підтримується I-7000 DCON протокол.
%description DAQ.DCON -l de_DE.UTF8
Das Paket %{name}-DAQ.DCON - bietet Realisierung DCON Kundenservice.
Unterstützte I-7000 DCON-Protokoll.
%endif

%if_enabled DAQGate
%package DAQ.DAQGate
Summary: Data sources gate
Summary(ru_RU.UTF8): Шлюз источников данных
Summary(uk_UA.UTF8): Шлюз джерел даних
Summary(de_DE.UTF8): Datenquellen Tor
Group: Graphics
Requires: %name-core = %version-%release
%description DAQ.DAQGate
The %{name}-DAQ.DAQGate package - allows you to perform the locking of the data sources
of the remote OpenSCADA stations in the local ones.
%description DAQ.DAQGate -l ru_RU.UTF8
Пакет %{name}-DAQ.DAQGate - позволяет выполнять шлюзование источников данных
удалённых OpenSCADA станций в локальные.
%description DAQ.DAQGate -l uk_UA.UTF8
Пакет %{name}-DAQ.DAQGate - дозволяє шлюзувати джерела даних віддалених OpenSCADA
станцій до локальних.
%description DAQ.DAQGate -l de_DE.UTF8
Das Paket %{name}-DAQ.DAQGate - verbindung mit Datenquellen von entfernten
OpenSCADA-Stationen.
%endif

%if_enabled SoundCard
%package DAQ.SoundCard
Summary: Sound card
Summary(ru_RU.UTF8): Звуковая карта
Summary(uk_UA.UTF8): Звукова карта
Summary(de_DE.UTF8): Soundkarte
Group: Graphics
Requires: %name-core = %version-%release
%description DAQ.SoundCard
The %{name}-DAQ.SoundCard package - provides an access to the sound card.
%description DAQ.SoundCard -l ru_RU.UTF8
Пакет %{name}-DAQ.SoundCard - предоставляет доступ к звуковой карте.
%description DAQ.SoundCard -l uk_UA.UTF8
Пакет %{name}-DAQ.SoundCard - надає доступ до даних звукової карти.
%description DAQ.SoundCard -l de_DE.UTF8
Das Paket %{name}-DAQ.SoundCard - zugriff auf Soundkartendata.
%endif

%if_enabled ICP_DAS
%package DAQ.ICP_DAS
Summary: ICP DAS hardware
Summary(ru_RU.UTF8): Оборудование ICP DAS
Summary(uk_UA.UTF8): Обладнання ICP DAS
Summary(de_DE.UTF8): ICP DAS Hardware
Group: Graphics
Requires: %name-core = %version-%release
%description DAQ.ICP_DAS
The %{name}-DAQ.ICP_DAS package - provides implementation for "ICP DAS" hardware support.
Includes main I-87xxx DCON modules, I-8xxx fast modules and boards on ISA bus.
%description DAQ.ICP_DAS -l ru_RU.UTF8
Пакет %{name}-DAQ.ICP_DAS - предоставляет реализацию поддержки оборудования ICP DAS.
Включает поддержку основных I-87xxx DCON модулей, I-8xxx быстрых модулей и плат на ISA шине.
%description DAQ.ICP_DAS -l uk_UA.UTF8
Пакет %{name}-DAQ.ICP_DAS - надає реалізацію підтримки обладнання ICP DAS.
Включає підтримку основних I-87xxx DCON модулів, I-8xxx швидких модулів та плат на ISA шині.
%description DAQ.ICP_DAS -l de_DE.UTF8
Das Paket %{name}-DAQ.ICP_DAS - bietet Implementierung für Hardware-Unterstützung "ICP DAS".
Inklusive Haupt I-87xxx DCON Module I-8xxx schnelle Module und Boards auf ISA-Bus.
%endif

%if_enabled OPC_UA
%package DAQ.OPC_UA
Summary: OPC UA protocol and DAQ
Summary(ru_RU.UTF8): OPC UA протокол и сбор данных
Summary(uk_UA.UTF8): OPC UA протокол та збір даних
Summary(de_DE.UTF8): OPC UA-Protokoll und DAQ
Group: Graphics
Requires: %name-core = %version-%release
%description DAQ.OPC_UA
The %{name}-DAQ.OPC_UA package - provides realisation of OPC UA protocol.
%description DAQ.OPC_UA -l ru_RU.UTF8
Пакет %{name}-DAQ.OPC_UA - предоставляет реализацию OPC UA протокола.
%description DAQ.OPC_UA -l uk_UA.UTF8
Пакет %{name}-DAQ.OPC_UA - надає реалізацію OPC UA протокола.
%description DAQ.OPC_UA -l de_DE.UTF8
Das Paket %{name}-DAQ.OPC_UA - erlauben Realisierung von OPC UA-Protokoll.
%endif

%if_enabled BFN
%package DAQ.BFN
Summary: Big Farm Net
Group: Graphics
Requires: %name-core = %version-%release
%description DAQ.BFN
The %{name}-DAQ.BFN package - provides Big Farm Net (BFN) modules support for
 Viper CT/BAS and other from "Big Dutchman" (http://www.bigdutchman.com).
%description DAQ.BFN -l ru_RU.UTF8
Пакет %{name}-DAQ.BFN - предоставляет поддержку модулей Большой Сети Фермы (BFN)
для Viper CT/BAS и других от "Big Dutchman" (http://www.bigdutchman.com).
%description DAQ.BFN -l uk_UA.UTF8
Пакет %{name}-DAQ.BFN - надає підтримку модулів Великої Мережі Ферми (BFN)
для Viper CT/BAS та інших від "Big Dutchman" (http://www.bigdutchman.com).
%description DAQ.BFN -l de_DE.UTF8
Das Paket %{name}-DAQ.BFN - bietet Big Farm Net (BFN) Module Unterstützung für
Viper CT/BAS und andere von "Big Dutchman" (http://www.bigdutchman.com).
%endif

%if_enabled Comedi
%package DAQ.Comedi
Summary: DAQ boards by Comedi
Summary(ru_RU.UTF8): DAQ платы от Comedi
Summary(uk_UA.UTF8): DAQ плати від Comedi
Summary(de_DE.UTF8): DAQ-Karte aus Comedi
Group: Graphics
Requires: %name-core = %version-%release
%description DAQ.Comedi
The %{name}-DAQ.Comedi package - provides ISA, PCI, PCMCIA, USB DAQ boards collection by Comedi(http://www.comedi.org).
%description DAQ.Comedi -l ru_RU.UTF8
Пакет %{name}-DAQ.Comedi - предоставляет коллекцию ISA, PCI, PCMCIA, USB DAQ плат от Comedi(http://www.comedi.org).
%description DAQ.Comedi -l uk_UA.UTF8
Пакет %{name}-DAQ.Comedi - надає колекцію ISA, PCI, PCMCIA, USB DAQ плат від Comedi(http://www.comedi.org).
%description DAQ.Comedi -l de_DE.UTF8
Das Paket %{name}-DAQ.Comedi - erlauben die Sammlung von ISA, PCI, PCMCIA, USB DAQ-Karten von Comedi (http://www.comedi.org).
%endif

%if_enabled AMRDevs
%package DAQ.AMRDevs
Summary: AMR devices
Summary(ru_RU.UTF8): Устройства АСКУ
Summary(uk_UA.UTF8): Пристрої АСКО
Summary(de_DE.UTF8): Die AMR-Devices
Group: Graphics
Requires: %name-core = %version-%release
%description DAQ.AMRDevs
The %{name}-DAQ.AMRDevs package - provides access to automatic meter reading devices.
Supported devices: Kontar (http://www.mzta.ru).
%description DAQ.AMRDevs -l ru_RU.UTF8
Пакет %{name}-DAQ.AMRDevs - предоставляет доступ к устройствам счётчиков коммерческого учёта.
Поддерживаются устройства: Kontar (http://www.mzta.ru).
%description DAQ.AMRDevs -l uk_UA.UTF8
Пакет %{name}-DAQ.AMRDevs - надає доступ до пристроїв лічильників комерцевого обліку.
Підтримуються пристрої: Kontar (http://www.mzta.ru).
%description DAQ.AMRDevs -l de_DE.UTF8
Das Paket %{name}-DAQ.AMRDevs - ermöglicht den Zugang zu den Zählerwerken der kommerziellen Inventur.
Es werden die Anlagen unterstützt: Kontar (http://www.mzta.ru).
%endif

%if_enabled MMS
%package DAQ.MMS
Summary: MMS(IEC-9506)
Group: Graphics
Requires: %name-core = %version-%release
%description DAQ.MMS
The %{name}-DAQ.MMS package - provides MMS(IEC-9506) client implementation.
%description DAQ.MMS -l ru_RU.UTF8
Пакет %{name}-DAQ.MMS - предоставляет реализацию клиента MMS(IEC-9506).
%description DAQ.MMS -l uk_UA.UTF8
Пакет %{name}-DAQ.MMS - надає реалізацію клієнта MMS(IEC-9506).
%description DAQ.MMS -l de_DE.UTF8
Das Paket %{name}-DAQ.MMS - ermöglicht MMS(IEC-9506) Client-Implementierung.
%endif


#===== Archiver subsystem modules =====
%if_enabled FSArch
%package Archive.FSArch
Summary: To file system archiver
Summary(ru_RU.UTF8): Архиватор на файловую систему
Summary(uk_UA.UTF8): Архіватор на файлову систему
Summary(de_DE.UTF8): File System archivator
Group: Graphics
Requires: %name-core = %version-%release
%description Archive.FSArch
The %{name}-Archive.FSArch package - provides functions for messages and values archiving to file system.
%description Archive.FSArch -l ru_RU.UTF8
Пакет %{name}-Archive.FSArch - предоставляет функции архивирования сообщений и значений на файловую систему.
%description Archive.FSArch -l uk_UA.UTF8
Пакет %{name}-Archive.FSArch - надає функції архівації повідомлень та значень на файлову систему.
%description Archive.FSArch -l de_DE.UTF8
Das Paket %{name}-Archive.FSArch - funktionen zum Archivieren von Nachrichten und Werten in dasDateisystem.
%endif

%if_enabled DBArch
%package Archive.DBArch
Summary: To DB archiver
Summary(ru_RU.UTF8): Архиватор на БД
Summary(uk_UA.UTF8): Архіватор на БД
Summary(de_DE.UTF8): Um DB archivator
Group: Graphics
Requires: %name-core = %version-%release
%description Archive.DBArch
The %{name}-Archive.DBArch package - provides functions for messages and values archiving to DB.
%description Archive.DBArch -l ru_RU.UTF8
Пакет %{name}-Archive.DBArch - предоставляет функции архивирования сообщений и значений на БД.
%description Archive.DBArch -l uk_UA.UTF8
Пакет %{name}-Archive.DBArch - надає функції архівації повідомлень та значень на БД.
%description Archive.DBArch -l de_DE.UTF8
Das Paket %{name}-Archive.DBArch - funktionen zum Archivieren von Nachrichten und Werten in die Datenbank.
%endif

#===== Transport subsystem modules =====
%if_enabled Sockets
%package Transport.Sockets
Summary: Transport: Sockets
Summary(ru_RU.UTF8): Транспорт: Сокеты
Summary(uk_UA.UTF8): Транспорт: Сокети
Summary(de_DE.UTF8): Verkehr: Sockets
Group: Graphics
Requires: %name-core = %version-%release
%description Transport.Sockets
The %{name}-Transport.Sockets package - provides sockets based transport.
Supports inet and unix sockets. Inet socket uses TCP, UDP and RAWCAN protocols.
%description Transport.Sockets -l ru_RU.UTF8
Пакет %{name}-Transport.Sockets - предоставляет транспорт основанный на сокетах.
Поддерживаются интернет и UNIX сокеты. Интернет сокет использует TCP, UDP и RAWCAN протоколы.
%description Transport.Sockets -l uk_UA.UTF8
Пакет %{name}-Transport.Sockets - надає транспорт базований на сокетах.
Підтримуються інтернет та UNIX сокети. Інтернет сокет використовує TCP, UDP та RAWCAN протоколи.
%description Transport.Sockets -l de_DE.UTF8
Das Paket %{name}-Transport.Sockets - gewährt den auf Sockets gebauten Transport.
Internet und UNIX Sockets werden unterstützt. Internet Socket benutzt die TCP, UDP und RAWCAN Protokolle.
%endif

%if_enabled SSL
%package Transport.SSL
Summary: Transport: SSL
Summary(ru_RU.UTF8): Транспорт: SSL
Summary(uk_UA.UTF8): Транспорт: SSL
Summary(de_DE.UTF8): Verkehr: SSL
Group: Graphics
Requires: %name-core = %version-%release
%description Transport.SSL
The %{name}-Transport.SSL package - provides transport based on the secure sockets' layer.
OpenSSL is used and SSLv2, SSLv3 and TLSv1 are supported.
%description Transport.SSL -l ru_RU.UTF8
Пакет %{name}-Transport.SSL - предоставляет транспорт основанный на слое безопасных сокетов.
Используется OpenSSL и поддерживаютя SSLv2, SSLv3 and TLSv1.
%description Transport.SSL -l uk_UA.UTF8
Пакет %{name}-Transport.SSL - надає транспорт базований на безпечному шарі сокетів.
Використано OpenSSL та підтримуються SSLv2, SSLv3 and TLSv1.
%description Transport.SSL -l de_DE.UTF8
Das Paket %{name}-Transport.SSL - gewährt den auf der Schicht von sicheren Sockets begründeten Transport.
Es werden benutzt OpenSSL und unterstützt SSLv2, SSLv3 und TLSv1.
%endif

%if_enabled Serial
%package Transport.Serial
Summary: Transport: Serial interfaces
Summary(ru_RU.UTF8): Транспорт: Последовательные интерфейсы
Summary(uk_UA.UTF8): Транспорт: Послідовні інтерфейси
Summary(de_DE.UTF8): Verkehr: Serielle Schnittstellen
Group: Graphics
Requires: %name-core = %version-%release
%description Transport.Serial
The %{name}-Transport.Serial package - Provides a serial interface.
It is used to data exchange via the serial interfaces of type RS232, RS485, GSM and more.
%description Transport.Serial -l ru_RU.UTF8
Пакет %{name}-Transport.Serial - предоставляет последовательный интерфейс.
Используется для обмена данными через последовательные интерфейсы типа RS232, RS485, GSM и другое.
%description Transport.Serial -l uk_UA.UTF8
Пакет %{name}-Transport.Serial - надає послідовні інтерфейси.
Використовується для обміну даними через послідовні інтерфейсти типу RS232, RS485, GSM та інше.
%description Transport.Serial -l de_DE.UTF8
Das Paket %{name}-Transport.Serial - ermöglicht die Verwendung der Seriellen Schnittstellen.
Für den Datenaustausch mit RS232, RS485, GSM und anderen.
%endif

#===== Transport protocol subsystem modules =====
%if_enabled HTTP
%package Protocol.HTTP
Summary: Protocol: HTTP-realisation
Summary(ru_RU.UTF8): Протокол: реалізация HTTP
Summary(uk_UA.UTF8): Протокол: реалізація HTTP
Summary(de_DE.UTF8): Protokoll: HTTP-Realisierung
Group: Graphics
Requires: %name-core = %version-%release
%description Protocol.HTTP
The %{name}-Protocol.HTTP package - provides support for the HTTP protocol for WWW-based user interfaces.
%description Protocol.HTTP -l ru_RU.UTF8
Пакет %{name}-Protocol.HTTP - предоставляет поддержку HTTP для WWW основанных пользовательских интерфейсов.
%description Protocol.HTTP -l uk_UA.UTF8
Пакет %{name}-Protocol.HTTP - надає підтримку HTTP для WWW базозованих користувальницьких інтерфейсів.
%description Protocol.HTTP -l de_DE.UTF8
Das Paket %{name}-Protocol.HTTP - HTTP-Unterstützung für eine WWW basierte Benutzerschnittstelle.
%endif

%if_enabled SelfSystem
%package Protocol.SelfSystem
Summary: Self system OpenSCADA protocol
Summary(ru_RU.UTF8): Протокол: собственный системы OpenSCADA
Summary(uk_UA.UTF8): Протокол: власний системи OpenSCADA
Summary(de_DE.UTF8): Protokoll: Eigener System OpenSCADA
Group: Graphics
Requires: %name-core = %version-%release
%description Protocol.SelfSystem
The %{name}-Protocol.SelfSystem package - provides own OpenSCADA protocol based at XML and one's control interface.
%description Protocol.SelfSystem -l ru_RU.UTF8
Пакет %{name}-Protocol.SelfSystem - предоставляет cобственный протокол основанный на XML и интерфейсе управления OpenSCADA.
%description Protocol.SelfSystem -l uk_UA.UTF8
Пакет %{name}-Protocol.SelfSystem - надає власний протокол заснований на XML та інтерфейсі керування OpenSCADA.
%description Protocol.SelfSystem -l de_DE.UTF8
Das Paket %{name}-Protocol.SelfSystem - bietet Selbst OpenSCADA Protokoll Basis auf XML und OpenSCADA Steuerschnittstelle.
%endif

%if_enabled UserProtocol
%package Protocol.UserProtocol
Summary: User protocol
Summary(ru_RU.UTF8): Протокол пользователя
Summary(uk_UA.UTF8): Протокол користувача
Summary(de_DE.UTF8): User-Protokoll
Group: Graphics
Requires: %name-core = %version-%release
%description Protocol.UserProtocol
The %{name}-Protocol.UserProtocol package - allows you to create your own user protocols on any OpenSCADA's language.
%description Protocol.UserProtocol -l ru_RU.UTF8
Пакет %{name}-Protocol.UserProtocol - позволяет создавать собственные пользовательские протоколы на внутреннем языке OpenSCADA.
%description Protocol.UserProtocol -l uk_UA.UTF8
Пакет %{name}-Protocol.UserProtocol - дозволяє створювати власні протоколи користувача на внутрішній мові OpenSCADA.
%description Protocol.UserProtocol -l de_DE.UTF8
Das Paket %{name}-Protocol.UserProtocol - ermöglichen die Erstellung selbst Benutzer Protokolle über interne OpenSCADA Sprache.
%endif

#===== UI subsystem modules =====
%if_enabled VCAEngine
%package UI.VCAEngine
Summary: Visual Control Area (VCA) engine
Summary(ru_RU.UTF8): Движок визуальной среды управления
Summary(uk_UA.UTF8): Рущій візуального середовища керування
Summary(de_DE.UTF8): Visuelle Kontrolle Bereich Engine
Group: Graphics
Requires: %name-core = %version-%release
%description UI.VCAEngine
The %{name}-UI.VCAEngine package - the main visual control area engine.
%description UI.VCAEngine -l ru_RU.UTF8
Пакет %{name}-UI.VCAEngine - общий движок среды визуализации и управления.
%description UI.VCAEngine -l uk_UA.UTF8
Пакет %{name}-UI.VCAEngine - загальний рущій середовища візуалізації та керування.
%description UI.VCAEngine -l de_DE.UTF8
Das Paket %{name}-UI.VCAEngine - allgemeine Visualisierungssteuerung.
%endif

%if_enabled Vision
%package UI.Vision
Summary: Operation user interface (Qt)
Summary(ru_RU.UTF8): Рабочий интерфейс пользователя (Qt)
Summary(uk_UA.UTF8): Робочий інтерфейс користувача (Qt)
Summary(de_DE.UTF8): Operation Benutzeroberfläche (Qt)
Group: Graphics
Requires: %name-core = %version-%release %name-UI.QTStarter
%description UI.Vision
The %{name}-UI.Vision package - visual operation user interface,
based on Qt library - front-end to VCA engine.
%description UI.Vision -l ru_RU.UTF8
Пакет %{name}-UI.Vision - рабочий пользовательский интерфейс,
основанный на библиотеке Qt - внешний интерфейс к движку визуализации.
%description UI.Vision -l uk_UA.UTF8
Пакет %{name}-UI.Vision - робочий інтерфейс користувача,
заснований на бібліотеці Qt - зовнішній інтерфейс до рущія візуалізації.
%description UI.Vision -l de_DE.UTF8
Das Paket %{name}-UI.Vision - visuelle Betrieb Benutzeroberfläche
basierend auf Qt-Bibliothek - Front-End für VCA-Motor.
%endif

%if_enabled QTStarter
%package UI.QTStarter
Summary: Qt-based GUI starter
Summary(ru_RU.UTF8): Пускатель графических основанных на Qt интерфейсов
Summary(uk_UA.UTF8): Пускач графічних інтерфейсів заснованих на Qt
Summary(de_DE.UTF8): Qt-basierte GUI-Starter
Group: Graphics
Requires: %name-core = %version-%release
%description UI.QTStarter
The %{name}-UI.QTStarter package - provides the Qt GUI starter.
Qt-starter is the only and compulsory component for all GUI modules based on the Qt library.
%description UI.QTStarter -l ru_RU.UTF8
Пакет %{name}-UI.QTStarter - предоставляет Qt GUI пускатель.
Qt-starter единственный и обязательный компонент для всех GUI модулей основанных на Qt.
%description UI.QTStarter -l uk_UA.UTF8
Пакет %{name}-UI.QTStarter - надає Qt GUI пускач.
Qt-starter єдиний та обов'язковий компонент для всіх GUI модулів заснованих на Qt.
%description UI.QTStarter -l de_DE.UTF8
Das Paket %{name}-UI.QTStarter - bietet die Qt GUI Starter.
Qt-Starter ist die einzige und verpflichtender Bestandteil für alle GUI-Module
basierend auf der Qt-Bibliothek.
%endif

%if_enabled QTCfg
%package UI.QTCfg
Summary: OpenSCADA system configurator (Qt)
Summary(ru_RU.UTF8): Системный OpenSCADA конфигуратор (Qt)
Summary(uk_UA.UTF8): Системний OpenSCADA конфігуратор (Qt)
Summary(de_DE.UTF8): System-Konfigurator OpenSCADA (Qt)
Group: Graphics
Requires: %name-core = %version-%release %name-UI.QTStarter
%description UI.QTCfg
The %{name}-UI.QTCfg package - provides the Qt-based configurator of the OpenSCADA system.
%description UI.QTCfg -l ru_RU.UTF8
Пакет %{name}-UI.QTCfg - предоставляет Qt основанный конфигуратор системы OpenSCADA.
%description UI.QTCfg -l uk_UA.UTF8
Пакет %{name}-UI.QTCfg - надає базований на Qt конфігуратор системи OpenSCADA.
%description UI.QTCfg -l de_DE.UTF8
Das Paket %{name}-UI.QTCfg - ermöglicht die Qt-basierte Systemeinstellung von OpenSCADA.
%endif

%if_enabled WebCfg
%package UI.WebCfg
Summary: System configurator (WEB)
Summary(ru_RU.UTF8): Системный конфигуратор (WEB)
Summary(uk_UA.UTF8): Системний конфігуратор (WEB)
Summary(de_DE.UTF8): System-Konfigurator (WEB)
Group: Graphics
Requires: %name-core = %version-%release %name-Protocol.HTTP
%description UI.WebCfg
The %{name}-UI.WebCfg package - provides the WEB-based configurator of the OpenSCADA system.
%description UI.WebCfg -l ru_RU.UTF8
Пакет %{name}-UI.WebCfg - предоставляет WEB основанный конфигуратор системы OpenSCADA.
%description UI.WebCfg -l uk_UA.UTF8
Пакет %{name}-UI.WebCfg - надає WEB базований конфігуратор системи OpenSCADA.
%description UI.WebCfg -l de_DE.UTF8
Das Paket %{name}-UI.WebCfg - ermöglicht die WEB-basierten Systemeinstellungen OpenSCADA.
%endif

%if_enabled WebCfgD
%package UI.WebCfgD
Summary: Dynamic WEB configurator
Summary(ru_RU.UTF8): Динамичный WEB конфигуратор
Summary(uk_UA.UTF8): Динамічний WEB конфігуратор
Summary(de_DE.UTF8): Dynamische Web-Konfigurator
Group: Graphics
Requires: %name-core = %version-%release %name-Protocol.HTTP
%description UI.WebCfgD
The %{name}-UI.WebCfgD package - provides dynamic WEB based configurator.
Uses XHTML, CSS and JavaScript technology.
%description UI.WebCfgD -l ru_RU.UTF8
Пакет %{name}-UI.WebCfgD - предоставляет динамический WEB основанный конфигуратор.
Использует XHTML, CSS и JavaScript технологии.
%description UI.WebCfgD -l uk_UA.UTF8
Пакет %{name}-UI.WebCfgD - надає динамічний WEB базований конфігуратор.
Використано XHTML, CSS та JavaScript технології.
%description UI.WebCfgD -l de_DE.UTF8
Das Paket %{name}-UI.WebCfgD - gewährt den dynamischen WEB begründeten Configurator.
Benutzt XHTML, CSS und JavaScript Technologien.
%endif

%if_enabled WebVision
%package UI.WebVision
Summary: Operation user interface (WEB)
Summary(ru_RU.UTF8): Рабочий интерфейс пользователя (WEB)
Summary(uk_UA.UTF8): Робочий інтерфейс користувача (WEB)
Summary(de_DE.UTF8): Operation Benutzeroberfläche (WEB)
Group: Graphics
Requires: %name-core = %version-%release %name-Protocol.HTTP
%description UI.WebVision
The %{name}-UI.WebVision package - visual operation user interface,
based on WEB - front-end to VCA engine.
%description UI.WebVision -l ru_RU.UTF8
Пакет %{name}-UI.WebVision - рабочий пользовательский интерфейс,
основанный на WEB - внешний интерфейс к движку визуализации.
%description UI.WebVision -l uk_UA.UTF8
Пакет %{name}-UI.WebVision - робочий інтерфейс користувача,
заснований на WEB - зовнішній інтерфейс до рущія візуалізації.
%description UI.WebVision -l de_DE.UTF8
Das Paket %{name}-UI.WebVision - visuelle Betrieb Benutzeroberfläche
basierend auf WEB - Front-End für VCA-Motor.
%endif

%if_enabled WebUser
%package UI.WebUser
Summary: Web interface from user
Summary(ru_RU.UTF8): Web интерфейс от пользователя
Summary(uk_UA.UTF8): Web інтерфейс від користувача
Summary(de_DE.UTF8): Web-Oberfläche vom Benutzer
Group: Graphics
Requires: %name-core = %version-%release %name-Protocol.HTTP
%description UI.WebUser
The %{name}-UI.WebUser package - allows you to create your own user web-interfaces in any language of OpenSCADA.
%description UI.WebUser -l ru_RU.UTF8
Пакет %{name}-UI.WebUser - позволяет создавать собственные пользовательские web-интерфейсы на любом языке OpenSCADA.
%description UI.WebUser -l uk_UA.UTF8
Пакет %{name}-UI.WebUser - дозволяє створювати користувачу власні web-інтерфейси на будьякій мові OpenSCADA.
%description UI.WebUser -l de_DE.UTF8
Das Paket %{name}-UI.WebUser - ermöglichen die Erstellung selbst Benutzer Web-Interfaces auf jedem OpenSCADA Sprache.
%endif

#===== Special subsystem modules =====
%if_enabled SystemTests
%package Special.SystemTests
Summary: OpenSCADA system's tests
Summary(ru_RU.UTF8): Системные тесты OpenSCADA
Summary(uk_UA.UTF8): Системні тести OpenSCADA
Summary(de_DE.UTF8): OpenSCADA System-Tests
Group: Graphics
Requires: %name-core = %version-%release
%description Special.SystemTests
The %{name}-Special.SystemTests package - provides the group of tests to the OpenSCADA system.
%description Special.SystemTests -l ru_RU.UTF8
Пакет %{name}-Special.SystemTests - предоставляет группу тестов для системы OpenSCADA.
%description Special.SystemTests -l uk_UA.UTF8
Пакет %{name}-Special.SystemTests - надає групу тестів для системи OpenSCADA.
%description Special.SystemTests -l de_DE.UTF8
Das Paket %{name}-Special.SystemTests - ermöglicht Gruppentests für das OpenSCADA-System.
%endif

%if_enabled FLibComplex1
%package Special.FLibComplex1
Summary: Complex1 function's library
Summary(ru_RU.UTF8): Библиотека функций Complex1
Summary(uk_UA.UTF8): Бібліотека функцій Complex1
Summary(de_DE.UTF8): Complex1 Funktion's library
Group: Graphics
Requires: %name-core = %version-%release
%description Special.FLibComplex1
The %{name}-Special.FLibComplex1 package - provides the library of functions compatible with SCADA Complex1 of the firm SIC "DIYA".
%description Special.FLibComplex1 -l ru_RU.UTF8
Пакет %{name}-Special.FLibComplex1 - предоставляет библиотеку функций совместимых с SCADA Complex1 фирмы ООО НИП "ДІЯ".
%description Special.FLibComplex1 -l uk_UA.UTF8
Пакет %{name}-Special.FLibComplex1 - надає бібліотеку функцій сумісних із SCADA Complex1 фірми ТОВ НІП "ДІЯ".
%description Special.FLibComplex1 -l de_DE.UTF8
Das Paket %{name}-Special.FLibComplex1 - bietet die Bibliothek von Funktionen mit SCADA Complex1 der Firma SIC "DIYA" kompatibel.
%endif

%if_enabled FLibMath
%package Special.FLibMath
Summary: Math function's library
Summary(ru_RU.UTF8): Библиотека математических функций
Summary(uk_UA.UTF8): Бібліотека математичних функцій
Summary(de_DE.UTF8): Math-Funktion ist library
Group: Graphics
Requires: %name-core = %version-%release
%description Special.FLibMath
The %{name}-Special.FLibMath package - provides the library of standard mathematical functions.
%description Special.FLibMath -l ru_RU.UTF8
Пакет %{name}-Special.FLibMath - предоставляет библиотеку стандартных математические функций.
%description Special.FLibMath -l uk_UA.UTF8
Пакет %{name}-Special.FLibMath - надає статичну бібліотеку математичних функцій.
%description Special.FLibMath -l de_DE.UTF8
Das Paket %{name}-Special.FLibMath - bibliothek mit mathematischen Funktionen.
%endif

%if_enabled FLibSYS
%package Special.FLibSYS
Summary: System API functions
Summary(ru_RU.UTF8): Функции системного API
Summary(uk_UA.UTF8): Функції системного API
Summary(de_DE.UTF8): System-API-Funktionen
Group: Graphics
Requires: %name-core = %version-%release
%description Special.FLibSYS
The %{name}-Special.FLibSYS package - provides the library of system API of user programming area.
%description Special.FLibSYS -l ru_RU.UTF8
Пакет %{name}-Special.FLibSYS - предоставляет в систему библиотеку системного API среды пользовательского программирования.
%description Special.FLibSYS -l uk_UA.UTF8
Пакет %{name}-Special.FLibSYS - надає в систему бібліотеку системного API середовища програмування користувача.
%description Special.FLibSYS -l de_DE.UTF8
Das Paket %{name}-Special.FLibSYS - bibliothek mit System-API für spezifische Programmierung.
%endif

%prep
%setup -q -n %srcname
%setup -T -D -a 1 -n %srcname

%build
%configure %{subst_enable DBF} %{subst_enable SQLite} %{subst_enable MySQL} %{subst_enable FireBird} %{subst_enable PostgreSQL} \
    %{subst_enable System} %{subst_enable BlockCalc} %{subst_enable JavaLikeCalc} %{subst_enable DiamondBoards} \
    %{subst_enable LogicLev} %{subst_enable SNMP} %{subst_enable Siemens} %{subst_enable ModBus} %{subst_enable DCON} \
    %{subst_enable DAQGate} %{subst_enable SoundCard} %{subst_enable ICP_DAS} %{subst_enable OPC_UA} %{subst_enable BFN} %{subst_enable Comedi} \
    %{subst_enable AMRDevs} %{subst_enable MMS} \
    %{subst_enable FSArch} %{subst_enable DBArch} \
    %{subst_enable Sockets} %{subst_enable SSL} %{subst_enable Serial} \
    %{subst_enable HTTP} %{subst_enable SelfSystem} %{subst_enable UserProtocol} \
    %{subst_enable VCAEngine} %{subst_enable Vision} %{subst_enable QTStarter} %{subst_enable QTCfg} \
    %{subst_enable WebCfg} %{subst_enable WebCfgD} %{subst_enable WebVision} %{subst_enable WebUser} \
    %{subst_enable SystemTests} %{subst_enable FLibComplex1} %{subst_enable FLibMath} %{subst_enable FLibSYS}
%make

%install
%makeinstall
rm -f %buildroot/%_libdir/openscada/*.la
install -m 755 -d %buildroot/%_datadir/openscada/{DATA,icons,docs,LibsDB,AGLKS,Boiler}
install -m 755 -pD data/oscada_ALT.init %buildroot/%_initdir/openscada-server
install -m 755 -pD data/oscada_ALT.init %buildroot/%_initdir/openscada-plc
sed -i "s/--projName=server/--projName=plc/" %buildroot/%_initdir/openscada-plc

install -m 755 -d %buildroot/%_mandir/man1
install -m 755 -d %buildroot/%_mandir/{uk,ru}/man1
install -m 644 doc/{openscada.1,openscada_start.1} %buildroot/%_mandir/man1/
install -m 644 doc/openscada.uk.1 %buildroot/%_mandir/uk/man1/openscada.1
install -m 644 doc/openscada_start.uk.1 %buildroot/%_mandir/uk/man1/openscada_start.1
install -m 644 doc/openscada.ru.1 %buildroot/%_mandir/ru/man1/openscada.1
install -m 644 doc/openscada_start.ru.1 %buildroot/%_mandir/ru/man1/openscada_start.1

ln -s openscada %buildroot/%_bindir/openscada_AGLKS
ln -s openscada %buildroot/%_bindir/openscada_Boiler

ln -s `relative %_defaultdocdir/%name-docEN-%version %_datadir/openscada/docs/en` %buildroot/%_datadir/openscada/docs/en
ln -s `relative %_defaultdocdir/%name-docRU-%version %_datadir/openscada/docs/ru` %buildroot/%_datadir/openscada/docs/ru
ln -s `relative %_defaultdocdir/%name-docUK-%version %_datadir/openscada/docs/uk` %buildroot/%_datadir/openscada/docs/uk

%files

%clean
#rm -rf %buildroot %buildroot/%name-%version


%files core
%defattr(-,root,root)
%config(noreplace) %_sysconfdir/oscada.xml
%config(noreplace) %_sysconfdir/oscada_start.xml

%_bindir/openscada
%_bindir/openscada_start
%_bindir/openscada-proj
%doc README README_ru README_uk COPYING ChangeLog INSTALL TODO TODO_ru TODO_uk
%_mandir/man1/*
%_mandir/*/man1/*
%_libdir/*.so.*
#_libdir/openscada/*.so
#exclude %_libdir/openscada/*.a
#exclude %_libdir/openscada/*.la
%_datadir/locale/*/LC_MESSAGES/openscada.mo
##%_datadir/openscada/DATA/.info
%_datadir/openscada/icons/*
%exclude %_datadir/openscada/icons/AGLKS.png
%exclude %_datadir/openscada/icons/Boiler.png
##%_datadir/openscada/ARCHIVES/MESS/.info
##%_datadir/openscada/ARCHIVES/VAL/.info

%files docEN
%defattr(-,root,root)
%doc doc/en/*
%_datadir/openscada/docs/en

%files docRU
%defattr(-,root,root)
%doc doc/ru/*
%_datadir/openscada/docs/ru

%files docUK
%defattr(-,root,root)
%doc doc/uk/*
%_datadir/openscada/docs/uk

%files devel
%defattr(-,root,root)
%_libdir/*.so
%_libdir/*.*a
%_includedir/openscada/*
%_pkgconfigdir/openscada.pc

%files LibDB.Main
%defattr(-,root,root)
%_datadir/openscada/LibsDB/OscadaLibs.db

%files LibDB.VCA
%defattr(-,root,root)
%_datadir/openscada/LibsDB/vca*.db

%files Model.AGLKS
%defattr(-,root,root)
%config(noreplace) %_sysconfdir/oscada_AGLKS.xml
%_bindir/openscada_AGLKS
%_desktopdir/openscada_AGLKS.desktop
%_iconsdir/openscada_AGLKS.png
%_datadir/openscada/icons/AGLKS.png
%_datadir/openscada/AGLKS/*.db

%files Model.Boiler
%defattr(-,root,root)
%config(noreplace) %_sysconfdir/oscada_Boiler.xml
%_bindir/openscada_Boiler
%_desktopdir/openscada_Boiler.desktop
%_iconsdir/openscada_Boiler.png
%_datadir/openscada/icons/Boiler.png
%_datadir/openscada/Boiler/*.db 

%post plc
%if %_vendor == "alt"
%post_service openscada-plc
%else
/sbin/chkconfig --add openscada-plc
%endif

%preun plc
%if %_vendor == "alt"
%preun_service openscada-plc
%else
/sbin/chkconfig --del openscada-plc
%endif

%files plc
%defattr(-,root,root)
%config(noreplace) %_sysconfdir/oscada_plc.xml
%config %_initdir/openscada-plc

%post server
%if %_vendor == "alt"
%post_service openscada-server
%else
/sbin/chkconfig --add openscada-server
%endif

%preun server
%if %_vendor == "alt"
%preun_service openscada-server
%else
/sbin/chkconfig --del openscada-server
%endif

%files server
%defattr(-,root,root)
%config(noreplace) %_sysconfdir/oscada_server.xml
%config %_initdir/openscada-server

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

%if_enabled PostgreSQL
%files DB.PostgreSQL
%_libdir/openscada/bd_PostgreSQL.so
%_datadir/locale/*/LC_MESSAGES/oscd_PostgreSQL.mo
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

%if_enabled OPC_UA
%files DAQ.OPC_UA
%_libdir/openscada/daq_OPC_UA.so
%_datadir/locale/*/LC_MESSAGES/oscd_OPC_UA.mo
%endif

%if_enabled BFN
%files DAQ.BFN
%_libdir/openscada/daq_BFN.so
%_datadir/locale/*/LC_MESSAGES/oscd_BFN.mo
%_datadir/openscada/BFN/*.db
%endif

%if_enabled Comedi
%files DAQ.Comedi
%_libdir/openscada/daq_Comedi.so
%_datadir/locale/*/LC_MESSAGES/oscd_Comedi.mo
%endif

%if_enabled AMRDevs
%files DAQ.AMRDevs
%_libdir/openscada/daq_AMRDevs.so
%_datadir/locale/*/LC_MESSAGES/oscd_AMRDevs.mo
%endif

%if_enabled MMS
%files DAQ.MMS
%_libdir/openscada/daq_MMS.so
%_datadir/locale/*/LC_MESSAGES/oscd_MMS.mo
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

%if_enabled UserProtocol
%files Protocol.UserProtocol
%_libdir/openscada/prot_UserProtocol.so
%_datadir/locale/*/LC_MESSAGES/oscd_UserProtocol.mo
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
%_desktopdir/openscada.desktop
%_iconsdir/openscada.png
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

%if_enabled WebUser
%files UI.WebUser
%_libdir/openscada/ui_WebUser.so
%_datadir/locale/*/LC_MESSAGES/oscd_WebUser.mo
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
* Sun Jun 17 2018 Roman Savochenko <roman@oscada.org>
- The work branch switched to the version 1+r0000.

* Sat Mar 24 2018 Roman Savochenko <roman@oscada.org>
- The daemon mode init script "oscadad" renamed to "openscada-server", separated to "openscada-plc" and moved to the coresponding package.

* Fri May 22 2015 Roman Savochenko <roman@oscada.org>
- Move to new Work version scheme naming 0.9+rNNNN, sets by the AutoBuilder or manual.
- Next LTS version will 0.8.N.

* Sat Feb 22 2014 Roman Savochenko <roman@oscada.org>
- Move to new Work version scheme naming 0.9-rNNNN.
- Next LTS version will 0.8.N.

* Fri Apr 06 2012 Roman Savochenko <roman@oscada.org>
- Build 0.8.0 release.

* Mon Dec 05 2011 Roman Savochenko <roman@oscada.org>
- Build 0.7.2 release.

* Mon Apr 18 2011 Roman Savochenko <roman@oscada.org>
- Model's RPM-packages dependency to main package is fixed for modules spec-file.

* Mon Mar 14 2011 Roman Savochenko <roman@oscada.org>
- New source packages implementation is released.
- Documentation separated by languages.
- Libraries DB packages is added for *-LibDB.Main and *-LibDB.VCA.
- Instead DemoDB package added Model's packages for "AGLKS" and "Boiler".

* Wed Jan 26 2011 Roman Savochenko <roman@oscada.org>
- "openscada-core" package is created for core library and core content. Main package "openscada" is used for typical installation.
- German messages is supplemented.

* Sun Oct 24 2010 Roman Savochenko <roman@oscada.org>
- Build 0.7.0 production release.

* Fri May 28 2010 Roman Savochenko <roman@oscada.org>
- Build 0.6.4.2 release.

* Mon Jan 25 2010 Roman Savochenko <roman@oscada.org>
- Build 0.6.4.1 release.

* Sun Oct 04 2009 Roman Savochenko <roman@oscada.org>
- Move modules to separated packages

* Mon Jun 08 2009 Roman Savochenko <roman@oscada.org>
- Build 0.6.3.3 release.

* Mon May 25 2009 Roman Savochenko <roman@oscada.org>
- Merge demo DB from different languages to one multilanguage (Russian,English and Ukrainian) DB.

* Wed Mar 18 2009 Roman Savochenko <roman@oscada.org>
- OpenSCADA update 0.6.3.2 release build.

* Mon Feb 2 2009 Roman Savochenko <roman@oscada.org>
- OpenSCADA update 0.6.3.1 release build.

* Mon Dec 22 2008 Roman Savochenko <roman@oscada.org>
- Documentation pack is unified and separated to project info files and documentation.
- Dynamic librarie's links packing into main package and development is fixed.

* Thu Oct 02 2008 Roman Savochenko <roman@oscada.org>
- Package name simple changing allow is added.

* Thu Sep 18 2008 Roman Savochenko <roman@oscada.org>
- Update spec to build for ALTLinux Sisyphus.

* Wed Mar 26 2008 Roman Savochenko <roman@oscada.org>
- Rebuilded for support x86_64 several distributives and some build system bugs is fixed.

* Sun Mar 23 2008 Roman Savochenko <roman@oscada.org>
- menu files included

* Fri Sep 02 2005 Roman Savochenko <roman@oscada.org>
- replace testdate whith demo package
- rename xinetd script from openscada to oscadad
- add xinetd script to generic package

* Wed Mar 16 2005 Roman Savochenko <roman@oscada.org>
- add Athena board specific build

* Wed Nov 03 2004 Roman Savochenko <roman@oscada.org>
- move the message arhives data to /var/spool/openscada/ARHIVE/MESS

* Tue Apr 06 2004 Roman Savochenko <roman@oscada.org>
- make 3 packages: OpenScada, OpenScada-devel, OpenScada-testdata
- add languages: ru, uk
- make packages from 'make -dist' package;

* Wed Oct 15 2003 Roman Savochenko <roman@oscada.org>
- Starting
