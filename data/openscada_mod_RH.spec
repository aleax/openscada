%define srcname openscada-%version
%define langmess %_datadir/locale
%define _desktopdir applications
%define _iconsdir icons
%define flag_el 1
# bcond_with "--disable compiling"
# bcond_without "--enable compiling"
# ======== DAQ =========
%bcond_with diamondboards
%bcond_without dcon
%bcond_without modbus
%bcond_without soundcard
%bcond_without snmp
%bcond_without siemens
%bcond_without system
%bcond_without blockcalc
%bcond_without javalikecalc
%bcond_without logiclevel
%bcond_without daqgate
%bcond_without opcua
%bcond_without bfn

%bcond_with icpdas
%bcond_with amrdevs
# ======== Protocol ========
%bcond_without selfsystem
%bcond_without userprotocol
# ========= DATA BASES =====
%bcond_with firebird
%bcond_without mysql
%bcond_without dbf
%bcond_without sqlite
%bcond_without postgresql
# =========== ARH ===========
%bcond_without dbarch
%bcond_without fsarch
# ========== Web Interfaces ======
%bcond_without webcfg
%bcond_without webcfgd
%bcond_without webvision
%bcond_without webuser
# ========== HTTP Protocol =========
%bcond_without http
# ========== QT Interfaces ===== ==
# QT4 devel old in to CentOs
%if "%{?dist}" == ".el5"
%bcond_with qtstarter
%bcond_with qtcfg
%bcond_with uivision
%bcond_with uivcaengine
%define flag_el 0
%else
%bcond_without qtstarter
%bcond_without qtcfg
%bcond_without uivision
%bcond_without uivcaengine
%endif
#%define _desktopdir %_datadir/applications

# ========== Transports ==========
%bcond_without ssl
%bcond_without sockets
%bcond_without serial
# ========== Special =============
%bcond_without flibcomplex
%bcond_without flibmath
%bcond_without flibsys
%bcond_without systemtests

# Only for x86_32
%ifarch x86_64
  %if 0%{?with_diamondboards}
  %{error: DIAMONDBOARDS support available only for %{ix86} target }
%endif
  %if 0%{?with_icpdas}
  %{error: ICP_DAS support available only for %{ix86} target }
  %endif
%endif

Summary: Open SCADA system
Name: openscada
Version: 0.9
Release: r2237%{?dist}
Source0: ftp://oscada.org/OpenSCADA/Work/openscada-%version.tar.lzma
Source1: ftp://oscada.org/OpenSCADA/Work/openscada-res-%version.tar.lzma

License: GPLv2
Group: Applications/Engineering
URL: http://oscada.org
BuildRoot: %(mktemp -ud %{_tmppath}/%{name}-%{version}-%{release}-XXXXXX)
# has some x86-assembly includes
ExclusiveArch:  %{ix86} x86_64

BuildRequires: gettext
BuildRequires: gd-devel
BuildRequires: byacc
BuildRequires: bison
BuildRequires: qt4-devel
BuildRequires: qt-devel
BuildRequires: openssl-devel
BuildRequires: fftw-devel
BuildRequires: autoconf
BuildRequires: automake
BuildRequires: libtool
BuildRequires: desktop-file-utils
BuildRequires: sed
BuildRequires: chrpath
BuildRequires: net-snmp-devel
BuildRequires: glibc-devel
BuildRequires: gcc-c++
BuildRequires: pcre-devel
BuildRequires: lzma
%if "%{?dist}" == ".el5"
BuildRequires: buildsys-macros
%endif
Requires(post): chkconfig
Requires(preun): chkconfig
Requires(preun): initscripts

Obsoletes: %{name}-Special-FlibComplex1
Obsoletes: %{name}-Special-FlibMath
Obsoletes: %{name}-Special-FlibSys

Obsoletes: %{name}-demo

%description
Open SCADA system. For access use account "root" and password "openscada".
%description -l ru_RU.UTF8
Открытая SCADA система. Для доступа используйте запись "root" и пароль
"openscada".
%description -l uk_UA.UTF8
Відкрита SCADA система. Для доступу використовуйте запис "root" та пароль
"openscada".
%description -l de_DE.UTF8
Das offene SCADA System. Für den Zugang die Aufzeichnung "root" und das
Kennwort "openscada" benutzen.

%post
/sbin/ldconfig
/sbin/chkconfig --add openscadad

%postun -p /sbin/ldconfig

%preun
if [ $1 = 0 ]; then
 /sbin/service openscadad stop > /dev/null 2>&1
 /sbin/chkconfig --del openscadad
fi

################## DAQ-System ###########################
%if 0%{?with_diamondboards}
%package DAQ-DiamondBoards
Summary: Diamond DA boards
Group: Applications/Engineering
Requires: %{name} = %{version}-%{release}
%description DAQ-DiamondBoards
The %name-DAQ-DiamondBoards package, provides data acquisition from the OS.
Supported OS Linux data sources: HDDTemp, Sensors, Uptime, Memory, CPU, UPS etc.
%description DAQ-DiamondBoards -l ru_RU.UTF8
Пакет %name-DAQ-DiamondBoards, предоставляет доступ к платам сбора данных фирмы
Diamond systems. Включает общую поддержку всех основных плат.
%description DAQ-DiamondBoards -l uk_UA.UTF8
Пакет %name-DAQ-DiamondBoards, надає доступ до плат збору даних фірми
Diamond systems. Включає загальну підтримку всіх основних плат.
%description DAQ-DiamondBoards -l de_DE.UTF8
Das Paket %name-DAQ-DiamondBoards, bietet Zugriff auf Diamant Systeme DA-Boards.
Inklusive Haupt Unterstützung für alle generischen Platten.
%endif

%if 0%{?with_dcon}
%package DAQ-DCON
Summary: DCON client
Group: Applications/Engineering
Requires: %{name} = %{version}-%{release}
%description DAQ-DCON
The %name-DAQ-DCON package, provides an implementation of DCON-client protocol.
Supports I-7000 DCON protocol.
%description DAQ-DCON -l ru_RU.UTF8
Пакет %name-DAQ-DCON, предоставляет реализацию клиентского сервиса
протокола DCON. Поддерживается протокол I-7000 DCON.
%description DAQ-DCON -l uk_UA.UTF8
Пакет %name-DAQ-DCON, надає реалізацію клієнтського сервісу DCON.
Підтримується I-7000 DCON протокол.
%description DAQ-DCON -l de_DE.UTF8
Das Paket %name-DAQ-DCON, bietet Realisierung DCON Kundenservice.
Unterstützte I-7000 DCON-Protokoll.
%endif

%if 0%{?with_modbus}
%package DAQ-ModBus
Summary: ModBus protocol and DAQ
Group: Applications/Engineering
Requires: %{name} = %{version}-%{release}
%description DAQ-ModBus
The %name-DAQ-ModBus package, provides realization of ModBus client service.
Supported Modbus/TCP, Modbus/RTU and Modbus/ASCII protocols.
%description DAQ-ModBus -l ru_RU.UTF8
Пакет %name-DAQ-ModBus, предоставляет реализацию клиентского сервиса
протокола ModBus. Поддерживаются Modbus/TCP, Modbus/RTU и
Modbus/ASCII протоколы.
%description DAQ-ModBus -l uk_UA.UTF8
Пакет %name-DAQ-ModBus, надає реалізацію клієнтського
ModBus сервісу.
Підтримуються Modbus/TCP, Modbus/RTU та Modbus/ASCII протоколи.
%description DAQ-ModBus -l de_DE.UTF8
Das Paket %name-DAQ-ModBus, emöglicht die Implementierung des
Kundenservices des ModBus-Protokolls. Unterstützt werden die
Protokolle Modbus/TCP, Modbus/RTU и Modbus/ASCII.
%endif

%if 0%{?with_soundcard}
%package DAQ-Soundcard
Summary: Sound card
Group: Applications/Engineering
BuildRequires: portaudio-devel
Requires: %{name} = %{version}-%{release}
%description DAQ-Soundcard
The %name-DAQ-Soundcard package, provides an access to the sound card.
%description DAQ-Soundcard -l ru_RU.UTF8
Пакет %name-DAQ-Soundcard, предоставляет доступ к звуковой карте.
%description DAQ-Soundcard -l uk_UA.UTF8
Пакет %name-DAQ-Soundcard, надає доступ до даних звукової карти.
%description DAQ-Soundcard -l de_DE.UTF8
Das Paket %name-DAQ-Soundcard, gewährt den Zugang zur Schallkarte.
%endif

%if 0%{?with_snmp}
%package DAQ-SNMP
Summary: SNMP client
Group: Applications/Engineering
Requires: %{name} = %{version}-%{release}
%description DAQ-SNMP
The %name-DAQ-SNMP package, provides an implementation of the client of SNMP-service.
%description DAQ-SNMP -l ru_RU.UTF8
Пакет %name-DAQ-SNMP, предоставляет реализацию клиентского
сервиса протокола SNMP.
%description DAQ-SNMP -l uk_UA.UTF8
Пакет %name-DAQ-SNMP, надає реалізацію клієнтського SNMP сервісу.
%description DAQ-SNMP -l de_DE.UTF8
Das Paket %name-DAQ-SNMP, emöglicht die Implementierung des
Kundenservices des SNMP-Protokolls.
%endif

%if 0%{?with_siemens}
%package DAQ-Siemens
Summary: Siemens DAQ
Group: Applications/Engineering
Requires: %{name} = %{version}-%{release}
%description DAQ-Siemens
The %name-DAQ-Siemens package, provides a data source PLC Siemens
by means of Hilscher CIF cards, by using the MPI protocol,
and Libnodave library for the rest.
%description DAQ-Siemens -l ru_RU.UTF8
Пакет %name-DAQ-Siemens, предоставляет источник данных ПЛК Siemens
посредством карт Hilscher CIF с использованием протокола MPI и
библиотеки Libnodave для остального.
%description DAQ-Siemens -l uk_UA.UTF8
Пакет %name-DAQ-Siemens, надає джерело даних ПЛК Siemens за
допомогою карт Hilscher CIF з використанням протоколу MPI та
бібліотеки Libnodave для іншого.
%description DAQ-Siemens -l de_DE.UTF8
Das Paket %name-DAQ-Siemens, enthält die Datenquelle PLC Siemens
mittels der Karten Hilscher CIF durch Anwendung des MPI -
Protokolls und der Bibliothek Libnodave für Anderes.
%endif

%if 0%{?with_system}
%package DAQ-System
Summary: System DA
Group: Applications/Engineering
BuildRequires: lm_sensors-devel
Requires: %{name} = %{version}-%{release}
%description DAQ-System
The %name-DAQ-System package, allow operation system data acquisition.
Support OS Linux data sources: HDDTemp, Sensors, Uptime, Memory,
CPU, UPS and other.
%description DAQ-System -l ru_RU.UTF8
Пакет %name-DAQ-System, предоставляет сбор данных операционной
системы. Поддерживаются источники данных ОС Linux: HDDTemp, Sensors,
Uptime, Memory, CPU, ИБП и другие.
%description DAQ-System -l uk_UA.UTF8
Пакет %name-DAQ-System, надає збір даних операційної системи.
Підтримуються джерела даних ОС Linux: HDDTemp, Sensors, Uptime,
Memory, CPU, ДБЖ та інше.
%description DAQ-System -l de_DE.UTF8
Das Paket %name-DAQ-System, ermöglicht die Datenerfassung des
Operationssystems. Es werden die Datenquellen ОС Linux: HDDTemp,
Sensors, Uptime, Memory, CPU, UPS und andere unterstützt.
%endif

%if 0%{?with_blockcalc}
%package DAQ-BlockCalc
Summary: Block based calculator
Group: Applications/Engineering
Requires: %{name} = %{version}-%{release}
%description DAQ-BlockCalc
The %name-DAQ-BlockCalc package, provides block based calculator.
%description DAQ-BlockCalc -l ru_RU.UTF8
Пакет %name-DAQ-BlockCalc, предоставляет блочный вычислитель.
%description DAQ-BlockCalc -l uk_UA.UTF8
Пакет %name-DAQ-BlockCalc, надає блоковий обчислювач.
%description DAQ-BlockCalc -l de_DE.UTF8
Das Paket %name-DAQ-BlockCalc, gewährt den Blockrechner
%endif

%if 0%{?with_javalikecalc}
%package DAQ-JavaLikeCalc
Summary: Java-like based calculator
Group: Applications/Engineering
Requires: %{name} = %{version}-%{release}
%description DAQ-JavaLikeCalc
The %name-DAQ-JavaLikeCalc package, provides based on java like
language calculator and engine of libraries.
The user can create and modify functions and libraries.
%description DAQ-JavaLikeCalc -l ru_RU.UTF8
Пакет %name-DAQ-JavaLikeCalc, предоставляет основанные на java
подобном языке вычислитель и движок библиотек. Пользователь может
создавать и модифицировать функции и библиотеки.
%description DAQ-JavaLikeCalc -l uk_UA.UTF8
Пакет %name-DAQ-JavaLikeCalc, надає базовані на мові схожій на Java
обчислювач та движок бібліотек функцї. Користувач може створювати та
модифікувати функції та бібліотеки.
%description DAQ-JavaLikeCalc -l de_DE.UTF8
Das %name-DAQ-JavaLikeCalc, entält die auf der Java - ähnlicher
Sprache begründeten Bibliothekenrechner und -läufer.
Der Nutzer kann Funktionen und Bibliotheken schaffen und
modifizieren.
%endif

%if 0%{?with_logiclevel}
%package DAQ-LogicLevel
Summary: Logic level
Group: Applications/Engineering
Requires: %{name} = %{version}-%{release}
%description DAQ-LogicLevel
The %name-DAQ-LogicLevel package, provides the logical level of parameters.
%description DAQ-LogicLevel -l ru_RU.UTF8
Пакет %name-DAQ-LogicLevel, предоставляет логический уровень
параметров.
%description DAQ-LogicLevel -l uk_UA.UTF8
Пакет %name-DAQ-LogicLevel, надає логічний рівень параметрів.
%description DAQ-LogicLevel -l de_DE.UTF8
Das Paket %name-DAQ-LogicLevel, enthält das logische
Parameterlevel.
%endif

%if 0%{?with_daqgate}
%package DAQ-Gate
Summary: Data sources gate
Group: Applications/Engineering
Requires: %{name} = %{version}-%{release}
%description DAQ-Gate
The %name-DAQ-Gate package, allows you to perform the locking of the data sources
of the remote OpenSCADA stations in the local ones.
%description DAQ-Gate -l ru_RU.UTF8
Пакет %name-DAQ-Gate, позволяет выполнять шлюзование источников
данных удалённых OpenSCADA станций в локальные.
%description DAQ-Gate -l uk_UA.UTF8
Пакет %name-DAQ-Gate, дозволяє шлюзувати джерела даних віддалених
OpenSCADA станцій до локальних.
%description DAQ-Gate -l de_DE.UTF8
Das Paket %name-DAQ-Gate, ermöglicht das Einschleusen der
Datenquellen der entfernten OpenSCADA Stationen in die lokale.
%endif

%if 0%{?with_icpdas}
%package DAQ-IcpDas
Summary: ICP DAS hardware
Group: Applications/Engineering
Requires: %{name} = %{version}-%{release}
%description DAQ-IcpDas
The %name-DAQ-IcpDas package, provides implementation for "ICP DAS" hardware support.
Includes main I-87xxx DCON modules, I-8xxx fast modules and boards on ISA bus.
%description DAQ-IcpDas -l ru_RU.UTF8
Пакет %name-DAQ-IcpDas, предоставляет реализацию поддержки оборудования ICP DAS.
Включает поддержку основных I-87xxx DCON модулей, I-8xxx быстрых модулей и плат на ISA шине.
%description DAQ-IcpDas -l uk_UA.UTF8
Пакет %name-DAQ-IcpDas, надає реалізацію підтримки обладнання ICP DAS.
Включає підтримку основних I-87xxx DCON модулів, I-8xxx швидких модулів та плат на ISA шині.
%description DAQ-IcpDas -l de_DE.UTF8
Das Paket %name-DAQ-IcpDas, bietet Implementierung für Hardware-Unterstützung "ICP DAS".
Inklusive Haupt I-87xxx DCON Module I-8xxx schnelle Module und Boards auf ISA-Bus.
%endif

%if 0%{?with_bfn}
%package DAQ-Bfn
Summary: Big Farm Net
Group: Applications/Engineering
Requires: %{name} = %{version}-%{release}
%description DAQ-Bfn
The %{name}-DAQ-Bfn package, provides Big Farm Net (BFN) modules support for
 Viper CT/BAS and other from "Big Dutchman" (http://www.bigdutchman.com).
%description DAQ-Bfn -l ru_RU.UTF8
Пакет %{name}-DAQ-Bfn, предоставляет поддержку модулей Большой Сети Фермы (BFN)
для Viper CT/BAS и других от "Big Dutchman" (http://www.bigdutchman.com).
%description DAQ-Bfn -l uk_UA.UTF8
Пакет %{name}-DAQ-Bfn, надає підтримку модулів Великої Мережі Ферми (BFN)
для Viper CT/BAS та інших від "Big Dutchman" (http://www.bigdutchman.com).
%description DAQ-Bfn -l de_DE.UTF8
Das Packet %{name}-DAQ-Bfn, bietet Big Farm Net (BFN) Module Unterstützung für
Viper CT/BAS und andere von "Big Dutchman" (http://www.bigdutchman.com).
%endif

%if 0%{?with_selfsystem}
%package Protocol-SelfSystem
Summary: Self system OpenSCADA protocol
Group: Applications/Engineering
Requires: %{name} = %{version}-%{release}
%description Protocol-SelfSystem
The %name-Protocol-SelfSystem package, provides own OpenSCADA protocol based at XML
and one's control interface.
%description Protocol-SelfSystem -l ru_RU.UTF8
Пакет %name-Protocol-SelfSystem, предоставляет cобственный протокол основанный
на XML и интерфейсе управления OpenSCADA.
%description Protocol-SelfSystem -l uk_UA.UTF8
Пакет %name-Protocol-SelfSystem, надає власний протокол заснований на XML
та інтерфейсі керування OpenSCADA.
%description Protocol-SelfSystem -l de_DE.UTF8
Das Paket %name-Protocol-SelfSystem, bietet Selbst OpenSCADA Protokoll Basis
auf XML und OpenSCADA Steuerschnittstelle.
%endif

%if 0%{?with_userprotocol}
%package Protocol-UserProtocol
Summary: User protocol
Group: Applications/Engineering
Requires: %{name} = %{version}-%{release}
%description Protocol-UserProtocol
The %{name}-Protocol-UserProtocol package, allows you to create your own user
protocols on any OpenSCADA's language.
%description Protocol-UserProtocol -l ru_RU.UTF8
Пакет %{name}-Protocol-UserProtocol, позволяет создавать
собственные пользовательские протоколы на любом OpenSCADA языке.
%description Protocol-UserProtocol -l uk_UA.UTF8
Пакет %{name}-Protocol-UserProtocol, дозволяє створювати власні
протоколи користувача на будьякій мові OpenSCADA.
%description Protocol-UserProtocol -l de_DE.UTF8
Das Paket %{name}-Protocol-UserProtocol, ermöglicht das Schaffen der
eigenen Usersprotokolle in jeder OpenSCADA - Sprache.
%endif

%if 0%{?with_opcua}
%package DAQ-OpcUa
Summary: OPC UA protocol and DAQ
Group: Applications/Engineering
Requires: %{name} = %{version}-%{release}
%description DAQ-OpcUa
The %name-DAQ-OpcUa package, allow realization of OPC UA protocol.
%description DAQ-OpcUa -l ru_RU.UTF8
Пакет %name-DAQ-OpcUa, предоставляет реализацию OPC UA протокола.
%description DAQ-OpcUa -l uk_UA.UTF8
Пакет %name-DAQ-OpcUa, надає реалізацію OPC UA протокола.
%description DAQ-OpcUa -l de_DE.UTF8
Das Paket %name-DAQ-OpcUa, gewährt den Einsatz des OPC UA -Protokolls
%endif

%if 0%{?with_amrdevs}
%package DAQ-AMRDevs
Summary: AMR devices
Group: Applications/Engineering
Requires: %{name} = %{version}-%{release}
%description DAQ-AMRDevs
The %name-DAQ-AMRDevs.
%description DAQ-AMRDevs -l ru_RU.UTF8
Пакет %name-DAQ-AMRDevs.
%description DAQ-AMRDevs -l uk_UA.UTF8
Пакет %name-DAQ-AMRDevs.
%description DAQ-AMRDevs -l de_DE.UTF8
Das Paket %name-DAQ-AMRDevs.
%endif

########################### BD-System ############################
%if 0%{?with_firebird}
%package DB-FireBird
Summary: DB FireBird support
Group: Applications/Engineering
BuildRequires: firebird-devel
Requires: %{name} = %{version}-%{release}
%description DB-FireBird
The %name-DB-FireBird package, provides support of the DB FireBird.
%description DB-FireBird -l ru_RU.UTF8
Пакет %name-DB-FireBird, предоставляет поддержку БД FireBird.
%description DB-FireBird -l uk_UA.UTF8
Пакет %name-DB-FireBird, модуль БД. Надає підтримку БД FireBird.
%description DB-FireBird -l de_DE.UTF8
Das Paket %name-DB-FireBird Gewährt die
FireBird-Dateibasenunterstützung.
%endif

%if 0%{?with_mysql}
%package DB-MySQL
Summary: DB MySQL support
Group: Applications/Engineering
BuildRequires: mysql-devel
Requires: %{name} = %{version}-%{release}
%description DB-MySQL
The %name-DB-MySQL package, provides support of the BD MySQL.
%description DB-MySQL -l ru_RU.UTF8
Пакет %name-DB-MySQL, предоставляет поддержку БД MySQL.
%description DB-MySQL -l uk_UA.UTF8
Пакет %name-DB-MySQL, Надає підтримку БД MySQL.
%description DB-MySQL -l de_DE.UTF8
Das Paket %name-DB-MySQL, gewährt die MySQL-Dateibasenunterstützung.
%endif

%if 0%{?with_dbf}
%package DB-DBF
Summary: DB DBF support
Group: Applications/Engineering
Requires: %{name} = %{version}-%{release}
%description DB-DBF
The %name-DB-DBF package, provides support of the *.dbf files, version 3.0.
version 3.0.
%description DB-DBF -l ru_RU.UTF8
Пакет %name-DB-DBF, предоставляет поддержку *.dbf файлов,
версии 3.0.
%description DB-DBF -l uk_UA.UTF8
Пакет %name-DB-DBF, надає підтримку *.dbf файлів, версії 3.0.
%description DB-DBF -l de_DE.UTF8
Das Paket %name-DB-DBF, gewährt die *.dbf Dateiunterstützung,
Versionen 3.0.
%endif

%if 0%{?with_sqlite}
%package DB-SQLite
Summary: DB SQLite support
Group: Applications/Engineering
BuildRequires: sqlite-devel
Requires: %{name} = %{version}-%{release}
%description DB-SQLite
The %name-DB-SQLite package, provides support of the BD SQLite.
%description DB-SQLite -l ru_RU.UTF8
Пакет %name-DB-SQLite, предоставляет поддержку БД SQLite.
%description DB-SQLite -l uk_UA.UTF8
Пакет %name-DB-SQLite, надає підтримку БД SQLite.
%description DB-SQLite -l de_DE.UTF8
Das Paket %name-DB-SQLite gewährt die DB SQLite - Unterstützung.
%endif

%if 0%{?with_postgresql}
%package DB-PostgreSQL
BuildRequires: postgresql-devel
Summary: DB PostgreSQL support
Group: Applications/Engineering
Requires: %{name} = %{version}-%{release}
%description DB-PostgreSQL
The %name-DB-PostgreSQL package, provides support of the DB PostgreSQL.
%description DB-PostgreSQL -l ru_RU.UTF8
Пакет %name-DB-PostgreSQL, предоставляет поддержку БД PostgreSQL.
%description DB-PostgreSQL -l uk_UA.UTF8
Пакет %name-DB-PostgreSQL, надає підтримку БД PostgreSQL.
%description DB-PostgreSQL -l de_DE.UTF8
Das Paket %name-DB-PostgreSQL gewährt die DB PostgreSQL - Unterstützung.
%endif

############################# ARH-System ############################
%if 0%{?with_dbarch}
%package ARH-DBArch
Summary: To DB archiver
Group: Applications/Engineering
Requires: %{name} = %{version}-%{release}
%description ARH-DBArch
The %name-ARH-DBArch package, provides functions for messages
and values archiving to DB.
%description ARH-DBArch -l ru_RU.UTF8
Пакет %name-ARH-DBArch, предоставляет функции архивирования
сообщений и значений на БД.
%description ARH-DBArch -l uk_UA.UTF8
Пакет %name-ARH-DBArch, надає функції архівації повідомлень
та значень на БД.
%description ARH-DBArch -l de_DE.UTF8
Das Paket %name-ARH-DBArch, gewährt Archivierungsfunktionen
der Nachrichten und Bedeutungen für DB.
%endif

%if 0%{?with_fsarch}
%package ARH-FSArch
Summary: To file system archiver
Group: Applications/Engineering
Requires: %{name} = %{version}-%{release}
%description ARH-FSArch
The %name-ARH-FSArch package, provides functions for messages
and values archiving to file system.
%description ARH-FSArch -l ru_RU.UTF8
Пакет %name-ARH-FSArch, предоставляет функции архивирования
сообщений и значений на файловую систему.
%description ARH-FSArch -l uk_UA.UTF8
Пакет %name-ARH-FSArch, надає функції архівації повідомлень
та значень на файлову систему.
%description ARH-FSArch -l de_DE.UTF8
Das Paket %name-ARH-FSArch, gewährt Archivierungsfunktionen
für Nachrichte und Bedeutungen für Dateisystem.
%endif

############################# UI-System ##############################
%if 0%{?with_webcfg}
%package UI-WebCfg
Summary: System configurator (WEB)
Group: Applications/Engineering
Requires: %{name} = %{version}-%{release}
# ############### Transport ########################
%if 0%{?with_sockets}
Requires:%{name}-Transport-Sockets = %{version}-%{release}
%endif
%if 0%{?with_ssl}
Requires:%{name}-Transport-SSL = %{version}-%{release}
%endif
# ##################################################
%description UI-WebCfg
The %name-UI-WebCfg package, provides the WEB-based configurator
of the OpenSCADA system.
%description UI-WebCfg -l ru_RU.UTF8
Пакет %name-UI-WebCfg, предоставляет WEB основанный конфигуратор
системы OpenSCADA.
%description UI-WebCfg -l uk_UA.UTF8
Пакет %name-UI-WebCfg, надає WEB базований конфігуратор системи
OpenSCADA.
%description UI-WebCfg -l de_DE.UTF8
Das Paket %name-UI-WebCfg, gewährt den WEB-begründeten
OpenSCADA-Konfigurator.
%endif

%if 0%{?with_webcfgd}
%package UI-WebCfgd
Summary: Dynamic WEB configurator
Group: Applications/Engineering
Requires: %{name} = %{version}-%{release}
# ############### Transport ########################
%if 0%{?with_sockets}
Requires:%{name}-Transport-Sockets = %{version}-%{release}
%endif
%if 0%{?with_ssl}
Requires:%{name}-Transport-SSL = %{version}-%{release}
%endif
# ##################################################
%description UI-WebCfgd
The %name-UI-WebCfgd package, provides dynamic WEB based configurator.
Uses XHTML, CSS and JavaScript technology.
%description UI-WebCfgd -l ru_RU.UTF8
Пакет %name-UI-WebCfgd, предоставляет динамический WEB основанный конфигуратор.
Использует XHTML, CSS и JavaScript технологии.
%description UI-WebCfgd -l uk_UA.UTF8
Пакет %name-UI-WebCfgd, надає динамічний WEB базований конфігуратор.
Використано XHTML, CSS та JavaScript технології.
%description UI-WebCfgd -l de_DE.UTF8
Das Paket %name-UI-WebCfgd, gewährt den dynamischen WEB-begründeten
Konfigurator. Nutzt die XHTML, CSS und JavaScript-Technologien aus.
%endif

%if 0%{?with_webvision}
%package UI-WebVision
Summary: Operation user interface (WEB)
Group: Applications/Engineering
Requires: %{name} = %{version}-%{release}
# ############### Transport ########################
%if 0%{?with_sockets}
Requires:%{name}-Transport-Sockets = %{version}-%{release}
%endif
%if 0%{?with_ssl}
Requires:%{name}-Transport-SSL = %{version}-%{release}
%endif
# ##################################################
%description UI-WebVision
The %name-UI-WebVision package, visual operation user interface,
based on WEB - front-end to VCA engine.
%description UI-WebVision -l ru_UA.UTF8
Пакет %name-UI-WebVision, рабочий пользовательский интерфейс,
основанный на WEB - внешний интерфейс к движку визуализации.
%description UI-WebVision -l uk_RU.UTF8
Пакет %name-UI-WebVision, робочий інтерфейс користувача,
заснований на WEB - зовнішній інтерфейс до рущія візуалізації.
%description UI-WebVision -l de_DE.UTF8
Das Paket %name-UI-WebVision, visuelle Betrieb Benutzeroberfläche
basierend auf WEB - Front-End für VCA-Motor.
%endif

%if 0%{?with_webuser}
%package UI-WebUser
Summary: Web interface from user
Group: Applications/Engineering
Requires: %{name} = %{version}-%{release}
%description UI-WebUser
The %name-UI-WebUser package, allows you to create your own user
web-interfaces in any language of OpenSCADA.
%description UI-WebUser -l ru_UA.UTF8
Пакет %name-UI-WebUser, позволяет создавать собственные пользовательские
web-интерфейсы на любом языке OpenSCADA.
%description UI-WebUser -l uk_RU.UTF8
Пакет %name-UI-WebUser, дозволяє створювати користувачу власні
web-інтерфейси на будьякій мові OpenSCADA.
%description UI-WebUser -l de_DE.UTF8
Das Paket %name-UI-WebUser, ermöglichen die Erstellung selbst Benutzer
Web-Interfaces auf jedem OpenSCADA Sprache.
%endif

%if 0%{?with_http}
%package Protocol-HTTP
Summary: Protocol: HTTP-realisation
Group: Applications/Engineering
Requires: %{name} = %{version}-%{release}
# ############### Transport ########################
%if 0%{?with_sockets}
Requires:%{name}-Transport-Sockets = %{version}-%{release}
%endif
%if 0%{?with_ssl}
Requires:%{name}-Transport-SSL = %{version}-%{release}
%endif
# ##################################################
%description Protocol-HTTP
The %name-Protocol-HTTP package, provides support for the HTTP protocol
for WWW-based user interfaces.
%description Protocol-HTTP -l ru_RU.UTF8
Пакет %name-Protocol-HTTP, предоставляет поддержку HTTP для WWW
основанных пользовательских интерфейсов.
%description Protocol-HTTP -l uk_UA.UTF8
Пакет %name-Protocol-HTTP, надає підтримку HTTP для WWW базозованих
користувальницьких інтерфейсів.
%description Protocol-HTTP -l de_DE.UTF8
Das Paket %name-Protocol-HTTP, gewährt die HTTP-Unterstützung
für die WWW-basierenden Nutzersinterfaces.
%endif

############################### GUI-System ##################################
%if 0%{?with_qtstarter}
%package UI-QTStarter
Summary: Qt-based GUI starter
Group: Applications/Engineering
Requires: %{name} = %{version}-%{release}
%description UI-QTStarter
The %name-UI-QTStarter package, provides the Qt GUI starter.
Qt-starter is the only and compulsory component for all GUI modules based on the Qt library.
%description UI-QTStarter -l ru_RU.UTF8
Пакет %name-UI-QTStarter, предоставляет Qt GUI пускатель.
Qt-starter единственный и обязательный компонент для всех GUI модулей основанных на Qt.
%description UI-QTStarter -l uk_UA.UTF8
Пакет %name-UI-QTStarter, надає Qt GUI пускач.
Qt-starter єдиний та обов'язковий компонент для всіх GUI модулів заснованих на Qt.
%description UI-QTStarter -l de_DE.UTF8
Das Paket %name-UI-QTStarter, bietet die Qt GUI Starter.
Qt-Starter ist die einzige und verpflichtender Bestandteil für alle GUI-Module
basierend auf der Qt-Bibliothek.
%endif

%if 0%{?with_qtcfg}
%package UI-QTCfg
Summary: OpenSCADA system configurator (Qt)
Group: Applications/Engineering
Requires: %{name} = %{version}-%{release}
%description UI-QTCfg
The %name-UI-QTCfg package, provides the Qt—based configurator of the OpenSCADA system.
%description UI-QTCfg -l ru_RU.UTF8
Пакет %name-UI-QTCfg, предоставляет Qt основанный конфигуратор
системы OpenSCADA.
%description UI-QTCfg -l uk_UA.UTF8
Пакет %name-UI-QTCfg, надає заснований на Qt конфігуратор.
%description UI-QTCfg -l de_DE.UTF8
Das Paket %name-UI-QTCfg, gewährt den Qt-begründeten
OpenSCADA-Systemkonfigurator.
%endif

############################### UI-System ##################################
%if 0%{?with_uivision}
%package UI-Vision
Summary: Operation user interface (Qt)
Group: Applications/Engineering
Requires: %{name} = %{version}-%{release}
%description UI-Vision
The %name-UI-Vision package, visual operation user interface,
based on Qt library - front-end to VCA engine.
%description UI-Vision -l ru_RU.UTF8
Пакет %name-UI-Vision, рабочий пользовательский интерфейс,
основанный на библиотеке Qt - внешний интерфейс к движку визуализации.
%description UI-Vision -l uk_UA.UTF8
Пакет %name-UI-Vision, робочий інтерфейс користувача,
заснований на бібліотеці Qt - зовнішній інтерфейс до рущія візуалізації.
%description UI-Vision -l de_DE.UTF8
Das Paket %name-UI-Vision, visuelle Betrieb Benutzeroberfläche
basierend auf Qt-Bibliothek - Front-End für VCA-Motor.
%endif

%if 0%{?with_uivcaengine}
%package UI-VCAEngine
Summary: Visual Control Area (VCA) engine
Group: Applications/Engineering
Requires: %{name} = %{version}-%{release}
%description UI-VCAEngine
The %{name}-UI-VCAEngine package, the main visual control area engine.
%description UI-VCAEngine -l ru_RU.UTF8
Пакет %{name}-UI-VCAEngine, общий движок среды визуализации и управления.
%description UI-VCAEngine -l uk_UA.UTF8
Пакет %{name}-UI-VCAEngine, загальний рущій середовища візуалізації та керування.
%description UI-VCAEngine -l de_DE.UTF8
Das Paket %{name}-UI-VCAEngine, allgemeine Visualisierungssteuerung.
%endif

############################# Transport-System ##############################
%if 0%{?with_ssl}
%package Transport-SSL
Summary: Transport: SSL
Group: Applications/Engineering
Requires: %{name} = %{version}-%{release}
%description Transport-SSL
The %name-Transport-SSL package, provides transport based on the secure sockets' layer.
OpenSSL is used and SSLv2, SSLv3 and TLSv1 are supported.
%description Transport-SSL -l ru_RU.UTF8
Пакет %name-Transport-SSL, предоставляет транспорт основанный
на слое безопасных сокетов. Используется OpenSSL и поддерживаются SSLv2, SSLv3
and TLSv1.
%description Transport-SSL -l uk_UA.UTF8
Пакет %name-Transport-SSL, надає транспорт базований на
безпечному шарі сокетів.
Використано OpenSSL та підтримуються SSLv2, SSLv3 and TLSv1.
%description Transport-SSL -l de_DE.UTF8
Das Paket %name-Transport-SSL, enthält den auf der Schicht der unfallfesten.
Sockets begründeten Transport. Es werden OpenSSL und SSLv2, SSLv3
und TLSv1 benutzt und unterstützt.
%endif

%if 0%{?with_sockets}
%package Transport-Sockets
Summary: Transport: Sockets
Group: Applications/Engineering
Requires: %{name} = %{version}-%{release}
%description Transport-Sockets
The %name-Transport-Sockets package, provides sockets based transport.
Supports inet and unix sockets. Inet socket uses TCP, UDP and RAWCAN protocols.
%description Transport-Sockets -l ru_RU.UTF8
Пакет %name-Transport-Sockets, предоставляет транспорт основанный
на сокетах. Поддерживаются интернет и UNIX сокеты.
Интернет сокет использует TCP, UDP и RAWCAN протоколы.
%description Transport-Sockets -l uk_UA.UTF8
Пакет %name-Transport-Sockets, надає транспорт базований на сокетах.
Підтримуються інтернет та UNIX сокети.
Інтернет сокет використовує TCP, UDP та RAWCAN протоколи.
%description Transport-Sockets -l de_DE.UTF8
Das Paket %name-Transport-Sockets, gewährt den auf Sockets begründeten Transport.
Unterstützt werden die Internet- und UNIX-Sockets.
Das Internetsocket benutzt die TCP, UDP and RAWCAN Protokolle.
%endif

%if 0%{?with_serial}
%package Transport-Serial
Summary: Transport: Serial interfaces
Group: Applications/Engineering
Requires: %{name} = %{version}-%{release}
%description Transport-Serial
The %name-Transport-Serial package, provides a serial interface.
It is used to data exchange via the serial interfaces of type RS232, RS485, GSM and more.
%description Transport-Serial -l ru_RU.UTF8
Пакет %name-Transport-Serial, предоставляет последовательный
интерфейс. Используется для обмена данными через последовательные интерфейсы
типа RS232, RS485, GSM и другое.
%description Transport-Serial -l uk_UA.UTF8
Пакет %name-Transport-Serial, надає послідовні інтерфейси.
Використовується для обміну даними через послідовні інтерфейсти
типу RS232, RS485, GSM та інше.
%description Transport-Serial -l de_DE.UTF8
Das Paket %name-Transport-Serial, gewährt das konsequente
Nutzersinterface. Wird für das Umtauschen von Daten durch konsequente
Interfaces wie RS232, RS485, GSM und andere benutzt.
%endif

############################# Functions-System #########################
%if 0%{?with_flibcomplex}
%package Special-FLibComplex1
Summary: Complex1 function's library
Group: Applications/Engineering
Requires: %{name} = %{version}-%{release}
%description Special-FLibComplex1
The %name-Special-FLibComplex1 package, provides the library of functions compatible
with SCADA Complex1 of the firm SIC "DIYA".
%description Special-FLibComplex1 -l ru_RU.UTF8
Пакет %name-Special-FLibComplex1-Sockets, предоставляет библиотеку функций совместимых
с SCADA Complex1 фирмы ООО НИП "ДІЯ".
%description Special-FLibComplex1 -l uk_UA.UTF8
Пакет %name-Special-FLibComplex1, надає бібліотеку функцій сумісних із SCADA Complex1
фірми ТОВ НІП "ДІЯ".
%description Special-FLibComplex1 -l de_DE.UTF8
Das Paket %name-Special-FLibComplex1s, bietet die Bibliothek von Funktionen mit SCADA
Complex1 der Firma SIC "DIYA" kompatibel.
%endif

%if 0%{?with_flibmath}
%package Special-FLibMath
Summary: Math function's library
Group: Applications/Engineering
Requires: %{name} = %{version}-%{release}
%description Special-FLibMath
The %name-Special-FLibMath package, provides the library of standard
mathematical functions.
%description Special-FLibMath -l ru_RU.UTF8
Пакет %name-Special-FLibMath, предоставляет библиотеку стандартных
математических функций.
%description Special-FLibMath -l uk_UA.UTF8
Пакет %name-Special-FLibMath, надає статичну бібліотеку
математичних функцій.
%description Special-FLibMath -l de_DE.UTF8
Das Paket %name-Special-FLibMath, enthält die Standardbibliothek
der mathematischen Funktionen.
%endif

%if 0%{?with_flibsys}
%package Special-FLibSYS
Summary: System API functions
Group: Applications/Engineering
Requires: %{name} = %{version}-%{release}
%description Special-FLibSYS
The %name-Special-FLibSYS package, provides the library of system API
of user programming area.
%description Special-FLibSYS -l ru_RU.UTF8
Пакет %name-Special-FLibSYS, предоставляет в систему библиотеку
системного API среды пользовательского программирования.
%description Special-FLibSYS -l uk_UA.UTF8
Пакет %name-Special-FLibSYS, надає в систему бібліотеку
системного API середовища програмування користувача.
%description Special-FLibSYS -l de_DE.UTF8
Das Paket %name-Special-FLibSYS, gewährt in das System die
Bibliothek der API-Systemprogrammierung des Nutzersbereiches.
%endif

######################### Tests-System ##############################
%if 0%{?with_systemtests}
%package Special-SystemTests
Summary: OpenSCADA system's tests
Group: Applications/Engineering
Requires: %{name} = %{version}-%{release}
%description Special-SystemTests
The %name-Special-SystemTests package, provides the group of tests
to the OpenSCADA system.
%description Special-SystemTests -l ru_RU.UTF8
Пакет %name-Special-SystemTests, предоставляет группу тестов
для системы OpenSCADA.
%description Special-SystemTests -l uk_UA.UTF8
Пакет %name-Special-SystemTests, надає групу тестів
для системи OpenSCADA.
%description Special-SystemTests -l de_DE.UTF8
Das Paket %name-Special-SystemTests, enthält die Testgruppe für das
OpenSCADA-System
%endif
######################### Documents-System ##############################

%package docEN
Summary: Open SCADA documents (EN)
Group: Documentation
%if 0%{?flag_el}
BuildArch: noarch
%endif
%description docEN
The %name-docEN package include documents files (English).
%description docEN -l ru_RU.UTF8
Пакет %name-docEN включает файлы документации (Английский).
%description docEN -l uk_UA.UTF8
Пакет %name-docEN включає файли документації (Англійська).
%description docEN -l de_DE.UTF8
Das Paket %name-docEN enthält die Dokumentationsdateien (Englisch).

%package docRU
Summary: Open SCADA documents (RU)
Group: Documentation
%if 0%{?flag_el}
BuildArch: noarch
%endif
%description docRU
The %name-docRU package include documents files (Russian).
%description docRU -l ru_RU.UTF8
Пакет %name-docRU включает файлы документации (Русский).
%description docRU -l uk_UA.UTF8
Пакет %name-docRU включає файли документації (Російська).
%description docRU -l de_DE.UTF8
Das Paket %name-docRU enthält die Dokumentationsdateien (Russisch).

%package docUK
Summary: Open SCADA documents (UK)
Group: Documentation
%if 0%{?flag_el}
BuildArch: noarch
%endif
%description docUK
The %name-docUK package include documents files (Ukraine).
%description docUK -l ru_RU.UTF8
Пакет %name-docUK включает файлы документации (Украинский).
%description docUK -l uk_UA.UTF8
Пакет %name-docUK включає файли документації (Українська).
%description docUK -l de_DE.UTF8
Das Paket %name-docUK enthält die Dokumentationsdateien (Ukrainisch).

######################### Devel-System ##############################
%package devel
Summary: Open SCADA development
Group: Development/Libraries
Requires: %{name} = %{version}-%{release}
%description devel
The %name-devel package includes library archives and include files.
%description devel -l ru_RU.UTF8
Пакет %name-devel включает архив библиотек и включаемые файлы.
%description devel -l uk_UA.UTF8
Пакет %name-devel включає архів бібліотек та включаємі файли.
%description devel -l de_DE.UTF8
Das Paket %name-devel enthält das Bibliothekenarchiv und die
eingeschlossenen
Dateien.

######################### DataBases-System ##############################
%package LibDB-Main
Summary: Main OpenSCADA libraries for DAQ and other into SQLite DB
Group: Applications/Engineering
%if 0%{?flag_el}
BuildArch: noarch
%endif
Requires:%{name} = %{version}-%{release}
%description LibDB-Main
The %{name}-LibDB-Main package, includes main OpenSCADA libraries
into SQLite DB.
For use connect SQLite DB file LibsDB/OscadaLibs.db.
%description LibDB-Main -l ru_RU.UTF8
Пакет %{name}-LibDB-Main, включает основные библиотеки OpenSCADA
в БД SQLite.
Для использования подключите файл БД SQLite LibsDB/OscadaLibs.db.
%description LibDB-Main -l uk_UA.UTF8
Пакет %{name}-LibDB-Main, включає основні бібліотеки OpenSCADA
у БД SQLite.
Для використання підключіть файл БД SQLite LibsDB/OscadaLibs.db.
%description LibDB-Main -l de_DE.UTF8
Das Paket %{name}-LibDB-Main, enthaelt die Hauptbibliothek
OpenSCADA die Datenbank SQLite.
So verwenden, verbinden Sie die Datei Datenbank SQLite
LibsDB/OscadaLibs.db.

%package LibDB-VCA
Summary: Main OpenSCADA libraries for DAQ and other into SQLite DB
Group: Applications/Engineering
%if 0%{?flag_el}
BuildArch: noarch
%endif
Requires:%{name} = %{version}-%{release}
%description LibDB-VCA
The %{name}-LibDB-VCA package, includes visual components libraries
into SQLite DB.
For use connect SQLite DB file LibsDB/vcaBase.db and LibsDB/vcaTest.db.
%description LibDB-VCA -l ru_RU.UTF8
Пакет %{name}-LibDB-VCA, включает библиотеки визуальных компонетов
в БД SQLite.
Для использования подключите файл БД SQLite LibsDB/vcaBase.db и
LibsDB/vcaTest.db.
%description LibDB-VCA -l uk_UA.UTF8
Пакет %{name}-LibDB-VCA, включає бібліотеки візуальних компонентів
у БД SQLite.
Для використання підключіть файл БД SQLite LibsDB/vcaBase.db
та LibsDB/vcaTest.db.
%description LibDB-VCA -l de_DE.UTF8
Das Paket %{name}-LibDB-VCA, enthaelt Visuelle Komponente in einer
Bibliothek Datenbank SQLite.
So verwenden, verbinden Sie die Datei Datenbank SQLite
LibsDB/vcaBase.db und LibsDB/vcaTest.db.

# ############################### Modeles ############################################
%package Model-AGLKS
Summary: Model "AGLKS" data bases and config (Demo: EN,RU,UK)
Group: Applications/Engineering
Requires: %{name} = %{version}-%{release}
Requires: %{name}-LibDB-Main = %{version}-%{release}
Requires: %{name}-LibDB-VCA = %{version}-%{release}
Requires: %{name}-DAQ-BlockCalc = %{version}-%{release}
Requires: %{name}-ARH-FSArch = %{version}-%{release}
Requires: %{name}-DAQ-JavaLikeCalc = %{version}-%{release}
Requires: %{name}-DAQ-LogicLevel = %{version}-%{release}
Requires: %{name}-DAQ-ModBus = %{version}-%{release}
Requires: %{name}-DAQ-System = %{version}-%{release}
Requires: %{name}-DB-SQLite = %{version}-%{release}
Requires: %{name}-Protocol-HTTP = %{version}-%{release}
Requires: %{name}-Protocol-SelfSystem = %{version}-%{release}
Requires: %{name}-Special-FLibComplex1 = %{version}-%{release}
Requires: %{name}-Special-FLibMath = %{version}-%{release}
Requires: %{name}-Special-FLibSYS = %{version}-%{release}
Requires: %{name}-Transport-SSL = %{version}-%{release}
Requires: %{name}-Transport-Serial = %{version}-%{release}
%if 0%{?with_qtstarter}
Requires: %{name}-UI-QTStarter = %{version}-%{release}
%endif
%if 0%{?with_qtcfg}
Requires: %{name}-UI-QTCfg = %{version}-%{release}
%endif
%if 0%{?with_uivision}
Requires: %{name}-UI-Vision = %{version}-%{release}
%endif
%if 0%{?with_uivcaengine}
Requires: %{name}-UI-VCAEngine = %{version}-%{release}
%endif
Requires: %{name}-UI-WebCfg = %{version}-%{release}
Requires: %{name}-UI-WebVision = %{version}-%{release}


%description Model-AGLKS
The %{name}-Model-AGLKS package includes model "AGLKS" data
bases and config.
The Model is used for OpenSCADA demo and allowed for English, Russian
and Ukrainian languages.
For start use command <openscada_demo> or <openscada_AGLKS>.
For access use account "root" and password "openscada" or account "user" and
password "user".
%description Model-AGLKS -l ru_RU.UTF8
Пакет %{name}-Model-AGLKS включает БД и конфигурацию модели "АГЛКС".
Модель используется для демонстрации OpenSCADA и доступна на Английском,
Русском и Украинском языках.
Для старта используйте команду <openscada_demo> мли <openscada_AGLKS>.
Для доступа используйте запись "root" и пароль "openscada" или запись
"user" и пароль "user".
%description Model-AGLKS -l uk_UA.UTF8
Пакет %{name}-Model-AGLKS включає БД та конфігурацію моделі "АГЛКС".
Модель використано для демонстрації OpenSCADA та доступно на Англійській,
Російській та Українській мовах.
Для старту використовуйте команду <openscada_demo> та <openscada_AGLKS>.
Для доступу використовуйте запис "root" та пароль "openscada" або запис
"user" та пароль "user".
%description Model-AGLKS -l de_DE.UTF8
Das Paket %{name}-Model-AGLKS enthaelt Datenbanken und
Konfigurationsdateien Modell "AGLKS".
Das Modell wird verwendet, um OpenSCADA demonstrieren und ist verfügbar
in Englisch, Russisch und Ukrainisch.
Fuers Starten wird Kommando <openscada_demo> oder <openscada_AGLKS> benutzt.
Fuer den Zugang die Aufzeichnung "root" und das Kennwort "openscada" benutzen.

%package Model-Boiler
Summary: Model "Boiler" data bases and config (only Russian)
Group: Applications/Engineering
Requires: %{name} = %{version}-%{release}
Requires: %{name}-LibDB-Main = %{version}-%{release}
Requires: %{name}-LibDB-VCA = %{version}-%{release}
Requires: %{name}-DAQ-BlockCalc = %{version}-%{release}
Requires: %{name}-ARH-FSArch = %{version}-%{release}
Requires: %{name}-DAQ-JavaLikeCalc = %{version}-%{release}
Requires: %{name}-DAQ-LogicLevel = %{version}-%{release}
Requires: %{name}-DAQ-System = %{version}-%{release}
Requires: %{name}-Special-FLibComplex1 = %{version}-%{release}
Requires: %{name}-Special-FLibMath = %{version}-%{release}
Requires: %{name}-Special-FLibSYS = %{version}-%{release}
Requires: %{name}-DB-SQLite = %{version}-%{release}
%if 0%{?with_qtstarter}
Requires: %{name}-UI-QTStarter = %{version}-%{release}
%endif
%if 0%{?with_qtcfg}
Requires: %{name}-UI-QTCfg = %{version}-%{release}
%endif
%if 0%{?with_uivision}
Requires: %{name}-UI-Vision = %{version}-%{release}
%endif
%if 0%{?with_uivcaengine}
Requires: %{name}-UI-VCAEngine = %{version}-%{release}
%endif
Requires: %{name}-UI-WebCfg = %{version}-%{release}
Requires: %{name}-UI-WebVision = %{version}-%{release}

%description Model-Boiler
The %{name}-Model-Boiler package model "Boiler" data bases and config.
The Model is allowed only for Russian language.
For start use command <openscada_Boiler>.
For access use account "root" and password "openscada" or account
"user" and password "user".
%description Model-Boiler -l ru_RU.UTF8
Пакет %{name}-Model-Boiler включает БД и конфигурацию модели "Котёл".
Модель доступна только на Русском языке.
Для старта используйте команду <openscada_Boiler>.
Для доступа используйте запись "root" и пароль "openscada" или запись
"user" и пароль "user".
%description Model-Boiler -l uk_UA.UTF8
Пакет %{name}-Model-Boiler включає БД та конфігурацію моделі "Котел".
Модель доступно тільки на Російській мові.
Для старту використовуйте команду <openscada_Boiler>.
Для доступу використовуйте запис "root" та пароль "openscada" або запис
"user" та пароль "user".
%description Model-Boiler -l de_DE.UTF8
Das Paket %{name}-Model-Boiler enthaelt Datenbanken und
Konfigurationsdateien Modell "Kessel".
Das Modell ist nur in Russisch verfügbar.
Fuers Starten wird Kommando <openscada_Boiler> benutzt.
Fuer den Zugang die Aufzeichnung "root" und das Kennwort "openscada" benutzen. 

# ############################### Virtual Packages ###################################
%package plc
Summary: OpenSCADA PLC
Group: Applications/Engineering
%if 0%{?flag_el}
BuildArch: noarch
%endif
Requires: %{name} = %{version}-%{release}
Requires: %{name}-ARH-FSArch
Requires: %{name}-DAQ-BlockCalc
%if 0%{?with_icpdas}
Requires: %{name}-DAQ-IcpDas
%endif
Requires: %{name}-DAQ-JavaLikeCalc
Requires: %{name}-DAQ-LogicLevel
Requires: %{name}-DAQ-ModBus
Requires: %{name}-DAQ-System
Requires: %{name}-DB-SQLite
Requires: %{name}-Protocol-HTTP
Requires: %{name}-Protocol-SelfSystem
Requires: %{name}-Special-FLibComplex1
Requires: %{name}-Special-FLibMath
Requires: %{name}-Special-FLibSYS
Requires: %{name}-Transport-SSL
Requires: %{name}-Transport-Serial
Requires: %{name}-Transport-Sockets
%if 0%{?with_uivcaengine}
Requires: %{name}-UI-VCAEngine
%endif
Requires: %{name}-UI-WebCfgd
Requires: %{name}-UI-WebVision
%description plc
The %{name}-plc package is virtual package for PLC.
%description plc -l ru_RU.UTF8
Пакет %{name}-plc это виртуальный пакет для ПЛК.
%description plc -l uk_UA.UTF8
Пакет %{name}-plc це віртуальний пакет для ПЛК.
%description plc -l de_RU.UTF8
Пакет %{name}-plc ist das Virtualpaket für PLC.

%package server
Summary: OpenSCADA server
Group: Applications/Engineering
%if 0%{?flag_el}
BuildArch: noarch
%endif
Requires: %{name} = %{version}-%{release}
Requires: %{name}-DB-SQLite
Requires: %{name}-DB-MySQL
Requires: %{name}-DB-PostgreSQL
%if 0%{?with_firebird}
Requires: %{name}-DB-FireBird
%endif
Requires: %{name}-DAQ-System
Requires: %{name}-DAQ-BlockCalc
Requires: %{name}-DAQ-JavaLikeCalc
%if 0%{?with_diamondboards}
Requires: %{name}-DAQ-DiamondBoards
%endif
Requires: %{name}-DAQ-LogicLevel
Requires: %{name}-DAQ-SNMP
Requires: %{name}-DAQ-Siemens
Requires: %{name}-DAQ-ModBus
Requires: %{name}-DAQ-DCON
Requires: %{name}-DAQ-Gate
Requires: %{name}-DAQ-Soundcard
%if 0%{?with_icpdas}
Requires: %{name}-DAQ-IcpDas
%endif
Requires: %{name}-ARH-FSArch
Requires: %{name}-ARH-DBArch
Requires: %{name}-Transport-Sockets
Requires: %{name}-Transport-SSL
Requires: %{name}-Transport-Serial
Requires: %{name}-Protocol-HTTP
Requires: %{name}-Protocol-SelfSystem
%if 0%{?with_uivision}
Requires: %{name}-UI-VCAEngine
%endif
Requires: %{name}-UI-WebCfg
Requires: %{name}-UI-WebVision
Requires: %{name}-UI-WebCfgd
Requires: %{name}-Special-FLibComplex1
Requires: %{name}-Special-FLibMath
Requires: %{name}-Special-FLibSYS
%description server
The %name-server package is virtual package for OpenSCADA-server.
%description server -l ru_RU.UTF8
Пакет %name-server это виртуальный пакет для сервера OpenSCADA.
%description server -l uk_UA.UTF8
Пакет %name-server це віртуальний пакет для сервера OpenSCADA.
%description server -l de_RU.UTF8
Пакет %name-server это виртуальный пакет для сервера OpenSCADA.

%package visStation
Summary: OpenSCADA visual station
Group: Applications/Engineering
%if 0%{?flag_el}
BuildArch: noarch
%endif
Requires: %name = %version-%release
Requires: %name-DB-SQLite
Requires: %name-DB-MySQL
Requires: %name-DB-PostgreSQL
Requires: %name-DAQ-System
Requires: %name-DAQ-BlockCalc
Requires: %name-DAQ-JavaLikeCalc
Requires: %name-DAQ-LogicLevel
Requires: %name-DAQ-SNMP
Requires: %name-DAQ-Siemens
Requires: %name-DAQ-ModBus
Requires: %name-DAQ-DCON
Requires: %name-DAQ-Gate
Requires: %name-DAQ-Soundcard
Requires: %name-ARH-FSArch
Requires: %name-ARH-DBArch
Requires: %name-Transport-Sockets
Requires: %name-Transport-SSL
Requires: %name-Transport-Serial
Requires: %name-Protocol-SelfSystem
%if 0%{?with_uivcaengine}
Requires: %name-UI-VCAEngine
%endif
%if 0%{?with_uivision}
Requires: %name-UI-Vision
%endif
%if 0%{?with_qtstarter}
Requires: %name-UI-QTStarter
%endif
%if 0%{?with_qtcfg}
Requires: %name-UI-QTCfg
%endif
Requires: %name-Special-FLibComplex1
Requires: %name-Special-FLibMath
Requires: %name-Special-FLibSYS
%description visStation
The %name-viStation package is virtual package for visual
station (OpenSCADA).
%description visStation -l ru_RU.UTF8
Пакет %name-visStation это виртуальный пакет для визуальной
станции (OpenSCADA).
%description visStation -l uk_UA.UTF8
Пакет %name-visStation це віртуальний пакет для сервера
візуальної станції (OpenSCADA).
%description visStation -l de_RU.UTF8
Пакет %name-visStation это виртуальный пакет для визуальной
станции (OpenSCADA).

%prep
%setup -q -n %srcname
%setup -T -D -a 1

%build
autoreconf -ifv
CFLAGS="%optflags" CXXFLAGS="%optflags" %configure --disable-static \
%{!?with_dcon:--disable-DCON} \
%{!?with_diamondboards:--disable-DiamondBoards} \
%{!?with_mysql:--disable-MySQL} \
%{!?with_firebird:--disable-FireBird} \
%{?with_firebird:--with-firebird=%{_libdir}/firebird} \
%{!?with_dbf:--disable-DBF} \
%{!?with_sqlite:--disable-SQLite} \
%{!?with_postgresql:--disable-PostgreSQL} \
%{!?with_webcfg:--disable-WebCfg} \
%{!?with_webcfgd:--disable-WebCfgD} \
%{!?with_webvision:--disable-WebVision} \
%{!?with_webuser:--disable-WebUser} \
%{!?with_http:--disable-HTTP} \
%{!?with_modbus:--disable-ModBus} \
%{!?with_soundcard:--disable-SoundCard} \
%{!?with_qtcfg:--disable-QTCfg} \
%{!?with_uivision:--disable-Vision} \
%{!?with_uivcaengine:--disable-VCAEngine} \
%{!?with_ssl:--disable-SSL} \
%{!?with_serial:--disable-Serial} \
%{!?with_sockets:--disable-Sockets} \
%{!?with_snmp:--disable-SNMP} \
%{!?with_siemens:--disable-Siemens} \
%{!?with_dbarch:--disable-DBArch} \
%{!?with_fsarch:--disable-FSArch} \
%{!?with_system:--disable-System} \
%{!?with_blockcalc:--disable-BlockCalc} \
%{!?with_javalikecalc:--disable-JavaLikeCalc} \
%{!?with_logiclevel:--disable-LogicLev} \
%{!?with_daqgate:--disable-DAQGate} \
%{!?with_icpdas:--disable-ICP_DAS} \
%{!?with_opcua:--disable-OPC_UA} \
%{!?with_amrdevs:--disable-AMRDevs} \
%{!?with_bfn:--disabled-BFN} \
%{!?without_bfn:--enable-BFN} \
%{!?with_selfsystem:--disable-SelfSystem} \
%{!?with_userprotocol:--disable-UserProtocol} \
%{!?with_flibcomplex:--disable-FLibComplex1} \
%{!?with_flibmath:--disable-FLibMath} \
%{!?with_flibsys:-disable-FlibSYS} \
%{!?with_systemtests:--disable-SelfSystem} \
%{!?with_qtstarter:--disable-QTStarter}

make %{?_smp_mflags}

%install
%__rm -rf %buildroot
%__make DESTDIR=%buildroot install

# let's try to get rid of rpath
chrpath --delete %buildroot/%_bindir/openscada

# remove static libs and libtool archives
%__rm -f %buildroot/%_libdir/*.*a
%__rm -f %buildroot/%_libdir/openscada/*.*a

%__install -m 755 -d %buildroot/%_includedir/openscada/
%__install -m 644 src/*.h %buildroot/%_includedir/openscada
%__install -m 755 -pD data/oscada_RH.init %buildroot/%_initrddir/openscadad
%__install -m 755 -d %buildroot/%_datadir/openscada/{DATA,icons,LibsDB,AGLKS,Boiler}

ln -s openscada_start %buildroot/%_bindir/openscada_AGLKS
ln -s openscada_start %buildroot/%_bindir/openscada_Boiler

%find_lang o.* %name.lang

%clean
%__rm -rf %buildroot

%files -f %{name}.lang

%defattr(-,root,root)
%config(noreplace) %_sysconfdir/oscada.xml
%config(noreplace) %_sysconfdir/oscada_start.xml
%dir %_libdir/openscada
%dir %_datadir/openscada
%_initrddir/openscadad
%_bindir/openscada
%_bindir/openscada_start
%doc README README_ru README_uk COPYING ChangeLog INSTALL TODO TODO_ru TODO_uk
%_libdir/*.so.*
%_libdir/openscada/*.so
%exclude %langmess/de/LC_MESSAGES/oscd_*
%exclude %langmess/ru/LC_MESSAGES/oscd_*
%exclude %langmess/uk/LC_MESSAGES/oscd_*

%{?with_diamondboards: %exclude %_libdir/openscada/daq_DiamondBoards.so}
%{?with_dcon: %exclude %_libdir/openscada/daq_DCON.so}
%{?with_modbus: %exclude %{_libdir}/openscada/daq_ModBus.so}
%{?with_soundcard: %exclude %{_libdir}/openscada/daq_SoundCard.so}
%{?with_snmp: %exclude %{_libdir}/openscada/daq_SNMP.so}
%{?with_siemens: %exclude %{_libdir}/openscada/daq_Siemens.so}
%{?with_system: %exclude %{_libdir}/openscada/daq_System.so}
%{?with_blockcalc: %exclude %{_libdir}/openscada/daq_BlockCalc.so}
%{?with_javalikecalc: %exclude %{_libdir}/openscada/daq_JavaLikeCalc.so}
%{?with_logiclevel: %exclude %{_libdir}/openscada/daq_LogicLev.so}
%{?with_daqgate: %exclude %{_libdir}/openscada/daq_DAQGate.so}
%{?with_icpdas: %exclude %{_libdir}/openscada/daq_ICP_DAS.so}
%{?with_opcua: %exclude %{_libdir}/openscada/daq_OPC_UA.so}
%{?with_amrdevs: %exclude %{_libdir}/openscada/daq_AMRDevs.so}
%{?with_bfn: %exclude %{_libdir}/openscada/daq_BFN.so}
%{?with_selfsystem: %exclude %{_libdir}/openscada/prot_SelfSystem.so}
%{?with_userprotocol: %exclude %{_libdir}/openscada/prot_UserProtocol.so}
%{?with_firebird: %exclude %{_libdir}/openscada/bd_FireBird.so}
%{?with_mysql: %exclude %{_libdir}/openscada/bd_MySQL.so}
%{?with_dbf: %exclude %{_libdir}/openscada/bd_DBF.so}
%{?with_sqlite: %exclude %{_libdir}/openscada/bd_SQLite.so}
%{?with_postgresql: %exclude %{_libdir}/openscada/bd_PostgreSQL.so}
%{?with_dbarch: %exclude %{_libdir}/openscada/arh_DBArch.so}
%{?with_fsarch: %exclude %{_libdir}/openscada/arh_FSArch.so}
%{?with_webcfg: %exclude %{_libdir}/openscada/ui_WebCfg.so}
%{?with_webcfgd: %exclude %{_libdir}/openscada/ui_WebCfgD.so}
%{?with_webvision: %exclude %{_libdir}/openscada/ui_WebVision.so}
%{?with_webuser: %exclude %{_libdir}/openscada/ui_WebUser.so}
%{?with_http: %exclude %{_libdir}/openscada/prot_HTTP.so}
%{?with_qtstarter: %exclude %{_libdir}/openscada/ui_QTStarter.so}
%{?with_qtcfg: %exclude %{_libdir}/openscada/ui_QTCfg.so}
%{?with_uivision: %exclude %{_libdir}/openscada/ui_Vision.so}
%{?with_uivcaengine: %exclude %{_libdir}/openscada/ui_VCAEngine.so}
%{?with_ssl: %exclude %{_libdir}/openscada/tr_SSL.so}
%{?with_sockets: %exclude %{_libdir}/openscada/tr_Sockets.so}
%{?with_ssl: %exclude %{_libdir}/openscada/tr_Serial.so}
%{?with_flibcomplex: %exclude %{_libdir}/openscada/spec_FLibComplex1.so}
%{?with_flibmath: %exclude %{_libdir}/openscada/spec_FLibMath.so}
%{?with_flibsys: %exclude %{_libdir}/openscada/spec_FLibSYS.so}
%{?with_systemtests: %exclude %{_libdir}/openscada/spec_SystemTests.so}

%{_localstatedir}/spool/openscada/DATA/info
%{_localstatedir}/spool/openscada/icons/*
%{_localstatedir}/spool/openscada/ARCHIVES/MESS/info
%{_localstatedir}/spool/openscada/ARCHIVES/VAL/info

%files plc
%defattr(-,root,root)

%files server
%defattr(-,root,root)

%files visStation
%defattr(-,root,root)

%files docEN
%defattr(-,root,root)
#%doc doc/*.pdf doc/Modules
%doc doc/en/*

%files docRU
%defattr(-,root,root)
#%doc doc/*.pdf doc/Modules
%doc doc/ru/*

%files docUK
%defattr(-,root,root)
#%doc doc/*.pdf doc/Modules
%doc doc/uk/*

%if 0%{?with_diamondboards}
%files DAQ-DiamondBoards
%defattr(-,root,root)
%{_libdir}/openscada/daq_DiamondBoards.so
%endif

%if 0%{?with_dcon}
%files DAQ-DCON
%defattr(-,root,root)
%{_libdir}/openscada/daq_DCON.so
%lang(de) %{langmess}/de/LC_MESSAGES/oscd_DCON.mo
%lang(ru) %{langmess}/ru/LC_MESSAGES/oscd_DCON.mo
%lang(uk) %{langmess}/uk/LC_MESSAGES/oscd_DCON.mo
%endif

%if 0%{?with_modbus}
%files DAQ-ModBus
%defattr(-,root,root)
%{_libdir}/openscada/daq_ModBus.so
%lang(de) %{langmess}/de/LC_MESSAGES/oscd_ModBus.mo
%lang(ru) %{langmess}/ru/LC_MESSAGES/oscd_ModBus.mo
%lang(uk) %{langmess}/uk/LC_MESSAGES/oscd_ModBus.mo
%endif

%if 0%{?with_soundcard}
%files DAQ-Soundcard
%defattr(-,root,root)
%{_libdir}/openscada/daq_SoundCard.so
%lang(de) %{langmess}/de/LC_MESSAGES/oscd_SoundCard.mo
%lang(ru) %{langmess}/ru/LC_MESSAGES/oscd_SoundCard.mo
%lang(uk) %{langmess}/uk/LC_MESSAGES/oscd_SoundCard.mo
%endif

%if 0%{?with_snmp}
%files DAQ-SNMP
%defattr(-,root,root)
%{_libdir}/openscada/daq_SNMP.so
%lang(de) %{langmess}/de/LC_MESSAGES/oscd_SNMP.mo
%lang(ru) %{langmess}/ru/LC_MESSAGES/oscd_SNMP.mo
%lang(uk) %{langmess}/uk/LC_MESSAGES/oscd_SNMP.mo
%endif

%if 0%{?with_siemens}
%files DAQ-Siemens
%defattr(-,root,root)
%{_libdir}/openscada/daq_Siemens.so
%lang(de) %{langmess}/de/LC_MESSAGES/oscd_Siemens.mo
%lang(ru) %{langmess}/ru/LC_MESSAGES/oscd_Siemens.mo
%lang(uk) %{langmess}/uk/LC_MESSAGES/oscd_Siemens.mo
%endif

%if 0%{?with_system}
%files DAQ-System
%defattr(-,root,root)
%{_libdir}/openscada/daq_System.so
%lang(de) %{langmess}/de/LC_MESSAGES/oscd_System.mo
%lang(ru) %{langmess}/ru/LC_MESSAGES/oscd_System.mo
%lang(uk) %{langmess}/uk/LC_MESSAGES/oscd_System.mo
%endif

%if 0%{?with_blockcalc}
%files DAQ-BlockCalc
%defattr(-,root,root)
%{_libdir}/openscada/daq_BlockCalc.so
%lang(de) %{langmess}/de/LC_MESSAGES/oscd_BlockCalc.mo
%lang(ru) %{langmess}/ru/LC_MESSAGES/oscd_BlockCalc.mo
%lang(uk) %{langmess}/uk/LC_MESSAGES/oscd_BlockCalc.mo
%endif

%if 0%{?with_javalikecalc}
%files DAQ-JavaLikeCalc
%defattr(-,root,root)
%{_libdir}/openscada/daq_JavaLikeCalc.so
%lang(de) %{langmess}/de/LC_MESSAGES/oscd_JavaLikeCalc.mo
%lang(ru) %{langmess}/ru/LC_MESSAGES/oscd_JavaLikeCalc.mo
%lang(uk) %{langmess}/uk/LC_MESSAGES/oscd_JavaLikeCalc.mo
%endif

%if 0%{?with_logiclevel}
%files DAQ-LogicLevel
%defattr(-,root,root)
%{_libdir}/openscada/daq_LogicLev.so
%lang(de) %{langmess}/de/LC_MESSAGES/oscd_LogicLev.mo
%lang(ru) %{langmess}/ru/LC_MESSAGES/oscd_LogicLev.mo
%lang(uk) %{langmess}/uk/LC_MESSAGES/oscd_LogicLev.mo
%endif

%if 0%{?with_daqgate}
%files DAQ-Gate
%defattr(-,root,root)
%{_libdir}/openscada/daq_DAQGate.so
%lang(de) %{langmess}/de/LC_MESSAGES/oscd_DAQGate.mo
%lang(ru) %{langmess}/ru/LC_MESSAGES/oscd_DAQGate.mo
%lang(uk) %{langmess}/uk/LC_MESSAGES/oscd_DAQGate.mo
%endif

%if 0%{?with_icpdas}
%files DAQ-IcpDas
%defattr(-,root,root)
%{_libdir}/openscada/daq_ICP_DAS.so
%lang(de) %{langmess}/de/LC_MESSAGES/oscd_ICP_DAS.mo
%lang(ru) %{langmess}/ru/LC_MESSAGES/oscd_ICP_DAS.mo
%lang(uk) %{langmess}/uk/LC_MESSAGES/oscd_ICP_DAS.mo
%endif

%if 0%{?with_opcua}
%files DAQ-OpcUa
%defattr(-,root,root)
%{_libdir}/openscada/daq_OPC_UA.so
%lang(de) %{langmess}/de/LC_MESSAGES/oscd_OPC_UA.mo
%lang(ru) %{langmess}/ru/LC_MESSAGES/oscd_OPC_UA.mo
%lang(uk) %{langmess}/uk/LC_MESSAGES/oscd_OPC_UA.mo
%endif

%if 0%{?with_amrdevs}
%files DAQ-AMRDevs
%defattr(-,root,root)
%{_libdir}/openscada/daq_AMRDevs.so
%lang(de) %{langmess}/de/LC_MESSAGES/oscd_AMRDevs.mo
%lang(ru) %{langmess}/ru/LC_MESSAGES/oscd_AMRDevs.mo
%lang(uk) %{langmess}/uk/LC_MESSAGES/oscd_AMRDevs.mo
%endif

%if 0%{?with_bfn}
%files DAQ-Bfn
%defattr(-,root,root)
%{_libdir}/openscada/daq_BFN.so
%lang(de) %{langmess}/de/LC_MESSAGES/oscd_BFN.mo
%lang(ru) %{langmess}/ru/LC_MESSAGES/oscd_BFN.mo
%lang(uk) %{langmess}/uk/LC_MESSAGES/oscd_BFN.mo
%endif

%if 0%{?with_selfsystem}
%files Protocol-SelfSystem
%defattr(-,root,root)
%{_libdir}/openscada/prot_SelfSystem.so
%lang(de) %{langmess}/de/LC_MESSAGES/oscd_SelfSystem.mo
%lang(ru) %{langmess}/ru/LC_MESSAGES/oscd_SelfSystem.mo
%lang(uk) %{langmess}/uk/LC_MESSAGES/oscd_SelfSystem.mo
%endif

%if 0%{?with_userprotocol}
%files Protocol-UserProtocol
%defattr(-,root,root)
%{_libdir}/openscada/prot_UserProtocol.so
%lang(de) %{langmess}/de/LC_MESSAGES/oscd_UserProtocol.mo
%lang(ru) %{langmess}/ru/LC_MESSAGES/oscd_UserProtocol.mo
%lang(uk) %{langmess}/uk/LC_MESSAGES/oscd_UserProtocol.mo
%endif

%if 0%{?with_firebird}
%files DB-FireBird
%defattr(-,root,root)
%{_libdir}/openscada/bd_FireBird.so
%lang(de) %{langmess}/de/LC_MESSAGES/oscd_FireBird.mo
%lang(ru) %{langmess}/ru/LC_MESSAGES/oscd_FireBird.mo
%lang(uk) %{langmess}/uk/LC_MESSAGES/oscd_FireBird.mo
%endif

%if 0%{?with_mysql}
%files DB-MySQL
%defattr(-,root,root)
%{_libdir}/openscada/bd_MySQL.so
%lang(de) %{langmess}/de/LC_MESSAGES/oscd_MySQL.mo
%lang(ru) %{langmess}/ru/LC_MESSAGES/oscd_MySQL.mo
%lang(uk) %{langmess}/uk/LC_MESSAGES/oscd_MySQL.mo
%endif

%if 0%{?with_dbf}
%files DB-DBF
%defattr(-,root,root)
%{_libdir}/openscada/bd_DBF.so
%lang(de) %{langmess}/de/LC_MESSAGES/oscd_DBF.mo
%lang(ru) %{langmess}/ru/LC_MESSAGES/oscd_DBF.mo
%lang(uk) %{langmess}/uk/LC_MESSAGES/oscd_DBF.mo
%endif

%if 0%{?with_sqlite}
%files DB-SQLite
%defattr(-,root,root)
%{_libdir}/openscada/bd_SQLite.so
%lang(de) %{langmess}/de/LC_MESSAGES/oscd_SQLite.mo
%lang(ru) %{langmess}/ru/LC_MESSAGES/oscd_SQLite.mo
%lang(uk) %{langmess}/uk/LC_MESSAGES/oscd_SQLite.mo
%endif

%if 0%{?with_postgresql}
%files DB-PostgreSQL
%defattr(-,root,root)
%{_libdir}/openscada/bd_PostgreSQL.so
%lang(de) %{langmess}/de/LC_MESSAGES/oscd_PostgreSQL.mo
%lang(ru) %{langmess}/ru/LC_MESSAGES/oscd_PostgreSQL.mo
%lang(uk) %{langmess}/uk/LC_MESSAGES/oscd_PostgreSQL.mo
%endif

%if 0%{?with_dbarch}
%files ARH-DBArch
%defattr(-,root,root)
%{_libdir}/openscada/arh_DBArch.so
%lang(de) %{langmess}/de/LC_MESSAGES/oscd_DBArch.mo
%lang(ru) %{langmess}/ru/LC_MESSAGES/oscd_DBArch.mo
%lang(uk) %{langmess}/uk/LC_MESSAGES/oscd_DBArch.mo
%endif

%if 0%{?with_fsarch}
%files ARH-FSArch
%defattr(-,root,root)
%{_libdir}/openscada/arh_FSArch.so
%lang(de) %{langmess}/de/LC_MESSAGES/oscd_FSArch.mo
%lang(ru) %{langmess}/ru/LC_MESSAGES/oscd_FSArch.mo
%lang(uk) %{langmess}/uk/LC_MESSAGES/oscd_FSArch.mo
%endif

%if 0%{?with_webcfg}
%files UI-WebCfg
%defattr(-,root,root)
%{_libdir}/openscada/ui_WebCfg.so
%lang(de) %{langmess}/de/LC_MESSAGES/oscd_WebCfg.mo
%lang(ru) %{langmess}/ru/LC_MESSAGES/oscd_WebCfg.mo
%lang(uk) %{langmess}/uk/LC_MESSAGES/oscd_WebCfg.mo
%endif

%if 0%{?with_webcfgd}
%files UI-WebCfgd
%defattr(-,root,root)
%{_libdir}/openscada/ui_WebCfgD.so
%lang(de) %{langmess}/de/LC_MESSAGES/oscd_WebCfgD.mo
%lang(ru) %{langmess}/ru/LC_MESSAGES/oscd_WebCfgD.mo
%lang(uk) %{langmess}/uk/LC_MESSAGES/oscd_WebCfgD.mo
%endif

%if 0%{?with_webvision}
%files UI-WebVision
%defattr(-,root,root)
%{_libdir}/openscada/ui_WebVision.so
%lang(de) %{langmess}/de/LC_MESSAGES/oscd_WebVision.mo
%lang(ru) %{langmess}/ru/LC_MESSAGES/oscd_WebVision.mo
%lang(uk) %{langmess}/uk/LC_MESSAGES/oscd_WebVision.mo
%endif

%if 0%{?with_webuser}
%files UI-WebUser
%defattr(-,root,root)
%{_libdir}/openscada/ui_WebUser.so
%lang(de) %{langmess}/de/LC_MESSAGES/oscd_WebUser.mo
%lang(ru) %{langmess}/ru/LC_MESSAGES/oscd_WebUser.mo
%lang(uk) %{langmess}/uk/LC_MESSAGES/oscd_WebUser.mo
%endif

%if 0%{?with_http}
%files Protocol-HTTP
%defattr(-,root,root)
%{_libdir}/openscada/prot_HTTP.so
%lang(de) %{langmess}/de/LC_MESSAGES/oscd_HTTP.mo
%lang(ru) %{langmess}/ru/LC_MESSAGES/oscd_HTTP.mo
%lang(uk) %{langmess}/uk/LC_MESSAGES/oscd_HTTP.mo
%endif

%if 0%{?with_qtstarter}
%files UI-QTStarter
%defattr(-,root,root)
%{_libdir}/openscada/ui_QTStarter.so
#%_desktopdir/openscada.desktop
#%_desktopdir/openscada_demo.desktop
#%_iconsdir/openscada.png
%lang(de) %{langmess}/de/LC_MESSAGES/oscd_QTStarter.mo
%lang(ru) %{langmess}/ru/LC_MESSAGES/oscd_QTStarter.mo
%lang(uk) %{langmess}/uk/LC_MESSAGES/oscd_QTStarter.mo
%endif

%if 0%{?with_qtcfg}
%files UI-QTCfg
%defattr(-,root,root)
%{_libdir}/openscada/ui_QTCfg.so
%lang(de) %{langmess}/de/LC_MESSAGES/oscd_QTCfg.mo
%lang(ru) %{langmess}/ru/LC_MESSAGES/oscd_QTCfg.mo
%lang(uk) %{langmess}/uk/LC_MESSAGES/oscd_QTCfg.mo
%endif

%if 0%{?with_uivision}
%files UI-Vision
%defattr(-,root,root)
%{_libdir}/openscada/ui_Vision.so
%lang(de) %{langmess}/de/LC_MESSAGES/oscd_Vision.mo
%lang(ru) %{langmess}/ru/LC_MESSAGES/oscd_Vision.mo
%lang(uk) %{langmess}/uk/LC_MESSAGES/oscd_Vision.mo
%endif

%if 0%{?with_uivcaengine}
%files UI-VCAEngine
%defattr(-,root,root)
%{_libdir}/openscada/ui_VCAEngine.so
%lang(de) %{langmess}/de/LC_MESSAGES/oscd_VCAEngine.mo
%lang(ru) %{langmess}/ru/LC_MESSAGES/oscd_VCAEngine.mo
%lang(uk) %{langmess}/uk/LC_MESSAGES/oscd_VCAEngine.mo
%endif

%if 0%{?with_ssl}
%files Transport-SSL
%defattr(-,root,root)
%{_libdir}/openscada/tr_SSL.so
%lang(de) %{langmess}/de/LC_MESSAGES/oscd_SSL.mo
%lang(ru) %{langmess}/ru/LC_MESSAGES/oscd_SSL.mo
%lang(uk) %{langmess}/uk/LC_MESSAGES/oscd_SSL.mo
%endif

%if 0%{?with_sockets}
%files Transport-Sockets
%defattr(-,root,root)
%{_libdir}/openscada/tr_Sockets.so
%lang(de) %{langmess}/de/LC_MESSAGES/oscd_Sockets.mo
%lang(ru) %{langmess}/ru/LC_MESSAGES/oscd_Sockets.mo
%lang(uk) %{langmess}/uk/LC_MESSAGES/oscd_Sockets.mo
%endif

%if 0%{?with_serial}
%files Transport-Serial
%defattr(-,root,root)
%{_libdir}/openscada/tr_Serial.so
%lang(de) %{langmess}/de/LC_MESSAGES/oscd_Serial.mo
%lang(ru) %{langmess}/ru/LC_MESSAGES/oscd_Serial.mo
%lang(uk) %{langmess}/uk/LC_MESSAGES/oscd_Serial.mo
%endif

%if 0%{?with_flibcomplex}
%files Special-FLibComplex1
%defattr(-,root,root)
%{_libdir}/openscada/spec_FLibComplex1.so
%lang(de) %{langmess}/de/LC_MESSAGES/oscd_FLibComplex1.mo
%lang(ru) %{langmess}/ru/LC_MESSAGES/oscd_FLibComplex1.mo
%lang(uk) %{langmess}/uk/LC_MESSAGES/oscd_FLibComplex1.mo
%endif

%if 0%{?with_flibmath}
%files Special-FLibMath
%defattr(-,root,root)
%{_libdir}/openscada/spec_FLibMath.so
%lang(de) %{langmess}/de/LC_MESSAGES/oscd_FLibMath.mo
%lang(ru) %{langmess}/ru/LC_MESSAGES/oscd_FLibMath.mo
%lang(uk) %{langmess}/uk/LC_MESSAGES/oscd_FLibMath.mo
%endif

%if 0%{?with_flibsys}
%files Special-FLibSYS
%defattr(-,root,root)
%{_libdir}/openscada/spec_FLibSYS.so
%lang(de) %{langmess}/de/LC_MESSAGES/oscd_FLibSYS.mo
%lang(ru) %{langmess}/ru/LC_MESSAGES/oscd_FLibSYS.mo
%lang(uk) %{langmess}/uk/LC_MESSAGES/oscd_FLibSYS.mo
%endif

%if 0%{?with_systemtests}
%files Special-SystemTests
%defattr(-,root,root)
%{_libdir}/openscada/spec_SystemTests.so
%lang(de) %{langmess}/de/LC_MESSAGES/oscd_SystemTests.mo
%lang(ru) %{langmess}/ru/LC_MESSAGES/oscd_SystemTests.mo
%lang(uk) %{langmess}/uk/LC_MESSAGES/oscd_SystemTests.mo
%endif

%files devel
%defattr(-,root,root)
%dir %{_includedir}/openscada
%{_libdir}/*.so
%{_includedir}/openscada/*
%{_libdir}/pkgconfig/openscada.pc

%files LibDB-Main
%defattr(-,root,root)
%{_localstatedir}/spool/openscada/LibsDB/OscadaLibs.db

%files LibDB-VCA
%defattr(-,root,root)
%{_localstatedir}/spool/openscada/LibsDB/vca*.db

%files Model-AGLKS
%defattr(-,root,root)
%config(noreplace) %{_sysconfdir}/oscada_AGLKS.xml
%{_bindir}/openscada_AGLKS
%{_localstatedir}/spool/openscada/AGLKS/*.db
%if 0%{?flag_el}
%{_datadir}/%_desktopdir/openscada_AGLKS.desktop
%{_datadir}/%_iconsdir/openscada_AGLKS.png
%{_localstatedir}/spool/openscada/icons/AGLKS.png
%endif

%files Model-Boiler
%defattr(-,root,root)
%config(noreplace) %{_sysconfdir}/oscada_Boiler.xml
%{_bindir}/openscada_Boiler
%{_localstatedir}/spool/openscada/Boiler/*.db 
%if 0%{?flag_el}
%{_datadir}/%_desktopdir/openscada_Boiler.desktop
%{_datadir}/%_iconsdir/openscada_Boiler.png
%{_localstatedir}/spool/openscada/icons/Boiler.png
%endif

%changelog
* Wed May 14 2014 Aleksey Popkov <aleksey@oscada.org> - 0.8.0.10
- Build the 0.8.0.10, LTS version.

* Mon Dec 23 2013 Aleksey Popkov <aleksey@oscada.org> - 0.8.0.9-1
- Build the 0.8.0.9-1, LTS version.

* Thu Sep 19 2013 Aleksey Popkov <aleksey@oscada.org> - 0.8.0.8-1
- Build the 0.8.0.8-1, LTS version.

* Fri Jul 12 2013 Aleksey Popkov <aleksey@oscada.org> - 0.8.0.7-1
- Build the 0.8.0.7-1, LTS version.

* Mon May 13 2013 Aleksey Popkov <aleksey@oscada.org> - 0.8.0.6-1
- Build the 0.8.0.6-1, LTS version.

* Thu Nov 15 2012 Aleksey Popkov <aleksey@oscada.org> - 0.8.0.4-1
- Build the 0.8.0.4-1, LTS version.

* Mon Sep 10 2012 Aleksey Popkov <aleksey@oscada.org> - 0.8.0.3-1
- Build the 0.8.0.3-1, LTS version.

* Wed Jul 11 2012 Aleksey Popkov <aleksey@oscada.org> - 0.8.0.2-1
- Build the 0.8.0.2-1, LTS version.

* Thu Jun 14 2012 Aleksey Popkov <aleksey@oscada.org> - 0.8.0.1-1
- Build the 0.8.0.1-1, LTS version.

* Fri Apr 13 2012 Aleksey Popkov <aleksey@oscada.org> - 0.8.0-3
- Adding patch for phonon.
- Adding some of the comments.
- Some cosmetics and fixed.
- Some commands are renamed in the macros.
- Building on the CentOs5.x NOT fixed.

* Tue Apr 10 2012 Aleksey Popkov <aleksey@oscada.org> - 0.8.0-2
- Some cosmetics.

* Mon Apr 09 2012 Aleksey Popkov <aleksey@oscada.org> - 0.8.0-1
- Build 0.8.0 release.

* Thu Dec 8 2011 Aleksey Popkov <aleksey@oscada.org> - 0.7.2-3
- Fixed of source code for build on the el5.
- Fixed of Source0 and Source1 directives.
- Some cosmetics.

* Thu Dec 8 2011 Aleksey Popkov <aleksey@oscada.org> - 0.7.2-2
- Some cosmetics.

* Wed Dec 7 2011 Aleksey Popkov <aleksey@oscada.org> - 0.7.2-1
- Build reliase 0.7.2.
- Removed to patch for init script.

* Wed Sep 14 2011 Aleksey Popkov <aleksey@oscada.org> - 0.7.1-5
- Adding of directive Obsoletes for demo package.
- Removed to some directives dir by Roman Savochenko (rom_as@oscada.org).

* Tue May 17 2011 Aleksey Popkov <aleksey@oscada.org> - 0.7.1-4
- Adding of autoreconf command.

* Mon Apr 18 2011 Aleksey Popkov <aleksey@oscada.org> - 0.7.1-3
- Updated for noarch packages enable.

* Fri Apr 15 2011 Aleksey Popkov <aleksey@oscada.org> - 0.7.1-2
- Cosmetics of file name.

* Thu Apr 14 2011 Aleksey Popkov <aleksey@oscada.org> - 0.7.1-1
- New source packages implementation is released.
- Documentation separated by languages.
- Libraries DB packages is added for *-LibDB-Main and *-LibDB-VCA.
- Instead DemoDB package added Model's packages for "AGLKS" and "Boiler".
- Adding the BFN module to self package.

* Fri Mar 4 2011 Aleksey Popkov <aleksey@oscada.org> - 0.7.0.2-2
- Rebuild of 0.7.0.2 version.
- Fixed UI.WebVision: Function VCAElFigure::dashDotFigureBorders() realisation wrong comment is fixed.

* Wed Mar 2 2011 Aleksey Popkov <aleksey@oscada.org> - 0.7.0.2-1
- Build of 0.7.0.2 version.

* Tue Jan 11 2011 Aleksey Popkov <aleksey@oscada.org> - 0.7.0.1-5
- Moved files of messages from main package to the self package.
- Fixed macros errors.
- Fixed of error in oscada.init.patch file.
- Fixed somes of spelling-error.

* Tue Jan 4 2011 Aleksey Popkov <aleksey@oscada.org> - 0.7.0.1-4
- My mistake fixing. Sorry!

* Tue Dec 21 2010 Aleksey Popkov <aleksey@oscada.org> - 0.7.0.1-3
- Fixed:UI.VCAEngine: A session deadlock is fixed for dynamic-active projects, for attributes access.

* Mon Dec 20 2010 Aleksey Popkov <aleksey@oscada.org> - 0.7.0.1-2
- Fixed BuildRequires.

* Mon Dec 20 2010 Aleksey Popkov <aleksey@oscada.org> - 0.7.0.1-1
- Fixed Source0 patch.
- Build 0.7.0.1 update to production release.

* Fri Nov 26 2010 Aleksey Popkov <aleksey@oscada.org> - 0.7.0-3
- Fixed unresolved deps.

* Tue Oct 26 2010 Aleksey Popkov <aleksey@oscada.org> - 0.7.0-2
- Build the 0.7.0 version.

* Wed Oct 13 2010 Aleksey Popkov <aleksey@oscada.org> - 0.7.0-2
- Adding the module to self package of PostgreSQL servers.
- Build the 0.7.0 version.

* Mon May 17 2010 Aleksey Popkov <aleksey@oscada.org> - 0.6.4.2-1
- RPM-build speck files is changed for build version 0.6.4.2 packages.
- Adding the module to self package of OPC_UA.
- Adding the module to self package of WebUser.
- Adding the module to self package of UserProtocol.
- Disabled the module AMRDevs (not tested).

* Wed Feb 17 2010 Aleksey Popkov <aleksey@oscada.org.ua> - 0.6.4.1-9
- Fixed of dependencies in Obsoletes directives.
- Change of Source0 url path.

* Sat Jan 30 2010 Aleksey Popkov <aleksey@oscada.org.ua> - 0.6.4.1-8
- Fixed of dependencies.

* Fri Jan 29 2010 Aleksey Popkov <aleksey@oscada.org.ua> - 0.6.4.1-7
- Added of obsoletes and provides directives.

* Fri Jan 29 2010 Aleksey Popkov <aleksey@oscada.org.ua> - 0.6.4.1-6
- Fixed of dependencies.

* Tue Jan 26 2010 Aleksey Popkov <aleksey@oscada.org.ua> - 0.6.4.1-5
- Fixed of dependencies.

* Tue Jan 26 2010 Aleksey Popkov <aleksey@oscada.org.ua> - 0.6.4.1-4
- The macros doc is edited.

* Sat Jan 9 2010 Aleksey Popkov <aleksey@oscada.org.ua> - 0.6.4-3
- Fixed of libpath in the oscada_demo.xml.

* Fri Oct 16 2009 Aleksey Popkov <aleksey@oscada.org.ua> - 0.6.4-2
- Added of Obsoletes directive by Peter Lemenkov <lemenkov@gmail.com>.

* Sun Oct 11 2009 Aleksey Popkov <aleksey@oscada.org.ua> - 0.6.4-1
- The change version for release 0.6.4.
- Moved Ui-VCAEngine module to the self package.
- Removed QTStarter module from the main package.
- Added the virtual plc, server, visStation packages.
- Some cosmetics.
- Fixed somes bugs Peter Lemenkov <lemenkov@gmail.com>.

* Sun Oct 4 2009 Aleksey Popkov <aleksey@oscada.org.ua> - 0.6.3.4-1
- Adding self module ICP_DAS.
- Fixed Germany Language translations by Popkova Irina.
- Delete openscada-0.6.3.3-openssl.patch from previouns version.
- Adding the next version of the package.

* Tue Sep 1 2009 Aleksey Popkov <aleksey@oscada.org.ua> - 0.6.3.3-13
- Adding Requires for webcfg, webcfgd, webvision, http and snmp.
- Some cosmetics.

* Tue Aug 25 2009 Tomas Mraz <tmraz@redhat.com> - 0.6.3.3-12
- rebuilt with new openssl.

* Mon Jul 27 2009 Popkov Aleksey <aleksey@oscada.org.ua> - 0.6.3.3-11
- Fixed of macros find_lang for epel-5 by Peter Lemenkov <lemenkov@gmail.com>.

* Sat Jul 25 2009 Fedora Release Engineering <rel-eng@lists.fedoraproject.org> - 0.6.3.3-10
- Rebuilt for https://fedoraproject.org/wiki/Fedora_12_Mass_Rebuild.

* Tue Jul 14 2009 Popkov Aleksey <aleksey@oscada.org.ua> 0.6.3.3-9
- Adding find_lang macros by Peter Lemenkov <lemenkov@gmail.com>.
- Somes cosmetics.

* Tue Jun 30 2009 Popkov Aleksey <aleksey@oscada.org.ua> 0.6.3.3-8
- Added of dependences in to self package demo.
- Fixed preun section by Peter Lemenkov <lemenkov@gmail.com>.
- Somes cosmetics.

* Fri Jun 19 2009 Popkov Aleksey <aleksey@oscada.org.ua> 0.6.3.3-7
- Fixed bugs maked by me.

* Thu Jun 18 2009 Popkov Aleksey <aleksey@oscada.org.ua> 0.6.3.3-6
- Workarounds for some bugs in rpmbuild by Peter Lemenkov <lemenkov@gmail.com>.

* Wed Jun 17 2009 Popkov Aleksey <aleksey@oscada.org.ua> 0.6.3.3-5
- Fixed critical bugs maked by me.

* Tue Jun 16 2009 Popkov Aleksey <aleksey@oscada.org.ua> 0.6.3.3-4
- Enabled Portaudio-devel library by Popkov Aleksey.

* Tue Jun 16 2009 Popkov Aleksey <aleksey@oscada.org.ua> 0.6.3.3-3
- Adapted spec file for dist 5E-epel by Popkov Aleksey.
- Adapted spec file for dist 4E-epel by Popkov Aleksey (Not tested).
- Fixed oscada.init.patch for cases messages.

* Thu Jun 11 2009 Popkov Aleksey <aleksey@oscada.org.ua> 0.6.3.3-2
- Fixed incoherent-init-script-name warning of rename init scripts from oscadad to openscadad by Popkov Aleksey.
- Fixed incoherent-subsys error of rename into init scripts from lockfile=/var/lock/subsys/oscadad to lockfile=/var/lock/subsys/openscadad by Popkov Aleksey.

* Wed Jun 10 2009 Popkov Aleksey <aleksey@oscada.org.ua> 0.6.3.3-1
- Release OpenSCADA 0.6.3.3.
- Added self modules of daq_DAQGate and tr_Serial.
- Adapted init script for fedora. oscada.init.patch.
- Translated description to Germany language by Popkova Irina.

* Mon Jun 8 2009 Popkov Aleksey <aleksey@oscada.org.ua> 0.6.3.3-0.1.svn902
- Prerelease OpenSCADA 0.6.3.3 (svn ver. 902)
- Fixed issue with find_lang
- Removal, of some unneded files by Peter Lemenkov <lemenkov@gmail.com>
- Translated description to German language by Popkova Irina.

* Thu Jun 4 2009 Popkov Aleksey <aleksey@oscada.org.ua> 0.6.3.3-0.1.svn901
- Prerelease OpenSCADA 0.6.3.3 (svn ver. 901)
- Translated description to German language by Popkova Irina.

* Wed Jun 3 2009 Popkov Aleksey <aleksey@oscada.org.ua> 0.6.3.3-0.1.svn899
- Adaptated for release OpenSCADA 0.6.3.3.

* Tue May 26 2009 Popkov Aleksey <aleksey@oscada.org.ua> 0.6.3.2-2
- OpenSCADA build for Fedora.
- Translated description to German language by Popkova Irina.

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

* Sun Mar 23 2008 Roman Savochenko <rom_as@diyaorg.dp.ua>
- menu files included.

* Fri Sep 02 2005 Roman Savochenko <rom_as@fromru.com>
- replace testdate whith demo package.
- rename xinetd script from openscada to oscadad.
- add xinetd script to generic package.

* Wed Mar 16 2005 Roman Savochenko <rom_as@fromru.com>
- add Athena board specific build.

* Wed Nov 03 2004 Roman Savochenko <rom_as@fromru.com>
- move the message arhives data to /var/spool/{name}/ARHIVE/MESS.

* Tue Apr 06 2004 Roman Savochenko <rom_as@fromru.com>
- make 3 packages: OpenScada, OpenScada-devel, OpenScada-testdata.
- add languages: ru, uk.
- make packages from 'make -dist' package;

* Wed Oct 15 2003 Roman Savochenko <rom_as@fromru.com>
- Starting.
