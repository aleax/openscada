#===== Generic Info ======
Summary: Open SCADA system
Summary(uk_UA.UTF8): Відкрита SCADA система
Summary(ru_RU.UTF8): Открытая SCADA система
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
AutoReq: noshell
%set_verify_elf_method no
BuildRequires: glibc-devel gcc-c++ libpcre-devel libgd2-devel sqlite3
BuildRequires: libMySQL-devel libsqlite3-devel firebird-devel postgresql-devel
BuildRequires: libsensors3-devel libnet-snmp-devel libportaudio2-devel libcomedi-devel libqt4-devel libfftw3-devel
%else
%define _initdir /etc/init.d
%define _desktopdir %_datadir/applications
%define _iconsdir /usr/share/icons
%define _pkgconfigdir %_libdir/pkgconfig
BuildRoot: %_tmppath/%name-%version-root
%endif

%description
Open SCADA system.
%description -l uk_UA.UTF8
Відкрита SCADA система.
%description -l ru_RU.UTF8
Открытая SCADA система.
%description -l de_DE.UTF8
Das offene SCADA System.

%package plc
Summary: Open SCADA PLC - daemon mode
Summary(uk_UA.UTF8): ПЛК на основі відкритої SCADA - режим демону
Summary(ru_RU.UTF8): ПЛК на основе открытой SCADA - режим демона
Summary(de_DE.UTF8): PLC basierend auf offenem SCADA - Dämonmodus
Group: Applications/Engineering
BuildArch: noarch
Requires: %name
%description plc
The %name-plc package is virtual package for PLC.
%description plc -l uk_UA.UTF8
Пакет %name-plc це віртуальний пакет для ПЛК.
%description plc -l ru_RU.UTF8
Пакет %name-plc это виртуальный пакет для ПЛК.
%description plc -l de_RU.UTF8
Пакет %name-plc ist das Virtualpaket für PLC.

%package server
Summary: OpenSCADA server - daemon mode
Summary(uk_UA.UTF8): Сервер на основі відкритої SCADA - режим демону
Summary(ru_RU.UTF8): Сервер на основе открытой SCADA - режим демона
Summary(de_DE.UTF8): Server basierend auf offenem SCADA - Dämonmodus
Group: Applications/Engineering
BuildArch: noarch
Requires: %name
%description server
The %name-server package is virtual package for OpenSCADA-server.
%description server -l uk_UA.UTF8
Пакет %name-server це віртуальний пакет для сервера OpenSCADA.
%description server -l ru_RU.UTF8
Пакет %name-server это виртуальный пакет для сервера OpenSCADA.
%description server -l de_RU.UTF8
Пакет %name-server это виртуальный пакет для сервера OpenSCADA.

%package docEN
Summary: Open SCADA documents (EN)
Summary(uk_UA.UTF8): Документація відкритої SCADA (EN)
Summary(ru_RU.UTF8): Документация открытой SCADA (EN)
Summary(de_DE.UTF8): Open SCADA Dokumente (EN)
Group: Documentation
BuildArch: noarch
%description docEN
The %name-docEN package include documents files (English).
%description docEN -l uk_UA.UTF8
Пакет %name-docEN включає файли документації (Англійська).
%description docEN -l ru_RU.UTF8
Пакет %name-docEN включает файлы документации (Английский).
%description docEN -l de_DE.UTF8
Das Paket %name-docEN enthaelt Dokumentationsdateien (Englisch).

%package docRU
Summary: Open SCADA documents (RU)
Summary(uk_UA.UTF8): Документація відкритої SCADA (RU)
Summary(ru_RU.UTF8): Документация открытой SCADA (RU)
Summary(de_DE.UTF8): Open SCADA Dokumente (RU)
Group: Documentation
BuildArch: noarch
Requires: %name-docEN
%description docRU
The %name-docRU package include documents files (Russian).
%description docRU -l uk_UA.UTF8
Пакет %name-docRU включає файли документації (Російська).
%description docRU -l ru_RU.UTF8
Пакет %name-docRU включает файлы документации (Русский).
%description docRU -l de_DE.UTF8
Das Paket %name-docRU enthaelt Dokumentationsdateien (Russisch).

%package docUK
Summary: Open SCADA documents (UK)
Summary(uk_UA.UTF8): Документація відкритої SCADA (UK)
Summary(ru_RU.UTF8): Документация открытой SCADA (UK)
Summary(de_DE.UTF8): Open SCADA Dokumente (UK)
Group: Documentation
BuildArch: noarch
Requires: %name-docEN
%description docUK
The %name-docUK package include documents files (Ukraine).
%description docUK -l uk_UA.UTF8
Пакет %name-docUK включає файли документації (Українська).
%description docUK -l ru_RU.UTF8
Пакет %name-docUK включает файлы документации (Украинский).
%description docUK -l de_DE.UTF8
Das Paket %name-docUK enthaelt Dokumentationsdateien (Ukrainisch).

%package devel
Summary: Open SCADA development
Summary(uk_UA.UTF8): Розробка відкритої SCADA
Summary(ru_RU.UTF8): Разработка открытой SCADA
Summary(de_DE.UTF8): Open SCADA Entwicklung
Group: Development/C++
Requires: %name = %version-%release
%description devel
The %name-devel package includes library archives and include files.
%description devel -l uk_UA.UTF8
Пакет %name-devel включає архів бібліотек та включаємі файли.
%description devel -l ru_RU.UTF8
Пакет %name-devel включает архив библиотек и включаемые файлы.
%description devel -l de_DE.UTF8
Das Paket %name-devel enthaelt die Bibliotheken und Archive Include-Dateien.

%package LibDB.Main
Summary: Main OpenSCADA libraries for DAQ and other into SQLite DB
Summary(uk_UA.UTF8): Основні бібліотеки OpenSCADA для збору даних та іншого у БД SQLite
Summary(ru_RU.UTF8): Основные библиотеки OpenSCADA для сбора данных и другого в БД SQLite
Summary(de_DE.UTF8): Hauptbibliothek OpenSCADA für die Datenerhebung und die anderen in der Datenbank SQLite
Group: Graphics
BuildArch: noarch
Requires: %name
%description LibDB.Main
The %{name}-LibDB.Main package includes main OpenSCADA libraries into SQLite DB.
For use connect SQLite DB file LibsDB/OscadaLibs.db.
%description LibDB.Main -l uk_UA.UTF8
Пакет %{name}-LibDB.Main включає основні бібліотеки OpenSCADA у БД SQLite.
Для використання підключіть файл БД SQLite LibsDB/OscadaLibs.db.
%description LibDB.Main -l ru_RU.UTF8
Пакет %{name}-LibDB.Main включает основные библиотеки OpenSCADA в БД SQLite.
Для использования подключите файл БД SQLite LibsDB/OscadaLibs.db.
%description LibDB.Main -l de_DE.UTF8
Das Paket %{name}-LibDB.Main enthaelt die Hauptbibliothek OpenSCADA die Datenbank SQLite.
So verwenden, verbinden Sie die Datei Datenbank SQLite LibsDB/OscadaLibs.db.

%package LibDB.VCA
Summary: Visual components libraries into SQLite DB
Summary(uk_UA.UTF8): Бібліотеки візуальних компонентів у БД SQLite
Summary(ru_RU.UTF8): Библиотеки визуальных компонетов в БД SQLite
Summary(de_DE.UTF8): Visuelle Komponente in einer Bibliothek Datenbank SQLite
Group: Graphics
BuildArch: noarch
Requires: %name
%description LibDB.VCA
The %{name}-LibDB.VCA package includes visual components libraries into SQLite DB.
For use connect SQLite DB file LibsDB/vcaBase.db and LibsDB/vcaTest.db.
%description LibDB.VCA -l uk_UA.UTF8
Пакет %{name}-LibDB.VCA включає бібліотеки візуальних компонентів у БД SQLite.
Для використання підключіть файл БД SQLite LibsDB/vcaBase.db та LibsDB/vcaTest.db.
%description LibDB.VCA -l ru_RU.UTF8
Пакет %{name}-LibDB.VCA включает библиотеки визуальных компонетов в БД SQLite.
Для использования подключите файл БД SQLite LibsDB/vcaBase.db и LibsDB/vcaTest.db.
%description LibDB.VCA -l de_DE.UTF8
Das Paket %{name}-LibDB.VCA enthaelt Visuelle Komponente in einer Bibliothek Datenbank SQLite.
So verwenden, verbinden Sie die Datei Datenbank SQLite LibsDB/vcaBase.db und LibsDB/vcaTest.db.

%package Model.AGLKS
Summary: Model "AGLKS" data bases and config (Demo: EN,UK,RU)
Summary(uk_UA.UTF8): БД та конфігурація моделі "АГЛКС" (Демо: EN,UK,RU)
Summary(ru_RU.UTF8): БД и конфигурация модели "АГЛКС" (Демо: EN,UK,RU)
Summary(de_DE.UTF8): Datenbanken und Konfigurationsdateien Modell "AGLKS" (Demo: EN,UK,RU)
Group: Graphics
BuildArch: noarch
Requires: %name %name-LibDB.Main %name-LibDB.VCA
%description Model.AGLKS
The %{name}-Model.AGLKS package includes model "AGLKS" data bases and config.
The Model is used for OpenSCADA demo and allowed for English, Ukrainian and Russian languages.
For start use command <openscada_AGLKS>.
%description Model.AGLKS -l uk_UA.UTF8
Пакет %{name}-Model.AGLKS включає БД та конфігурацію моделі "АГЛКС".
Модель використано для демонстрації OpenSCADA та доступно на Англійській, Українській та Російській мовах.
Для старту використовуйте команду <openscada_AGLKS>.
%description Model.AGLKS -l ru_RU.UTF8
Пакет %{name}-Model.AGLKS включает БД и конфигурацию модели "АГЛКС".
Модель используется для демонстрации OpenSCADA и доступна на Английском, Украинском и Российском языках.
Для старта используйте команду <openscada_AGLKS>.
%description Model.AGLKS -l de_DE.UTF8
Das Paket %{name}-Model.AGLKS enthaelt Datenbanken und Konfigurationsdateien Modell "AGLKS".
Das Modell wird verwendet, um OpenSCADA demonstrieren und ist verfügbar in Englisch, Ukrainisch und Russisch.
Fuers Starten wird Kommando <openscada_AGLKS> benutzt.

%package Model.Boiler
Summary: Model "Boiler" data bases and config (EN,UK,RU)
Summary(uk_UA.UTF8): БД та конфігурація моделі "Котел" (EN,UK,RU)
Summary(ru_RU.UTF8): БД и конфигурация модели "Котёл" (EN,UK,RU)
Summary(de_DE.UTF8): Datenbanken und Konfigurationsdateien Modell "Kessel" (EN,UK,RU)
Group: Graphics
BuildArch: noarch
Requires: %name %name-LibDB.Main %name-LibDB.VCA
%description Model.Boiler
The %{name}-Model.Boiler package model "Boiler" data bases and config.
The Model is allowed for English, Ukrainian and Russian languages.
For start use command <openscada_Boiler>.
%description Model.Boiler -l uk_UA.UTF8
Пакет %{name}-Model.Boiler включає БД та конфігурацію моделі "Котел".
Модель доступно на Англійській, Українській та Російській мовах.
Для старту використовуйте команду <openscada_Boiler>.
%description Model.Boiler -l ru_RU.UTF8
Пакет %{name}-Model.Boiler включает БД и конфигурацию модели "Котёл".
Модель доступна на Английском, Украинском и Российском языках.
Для старта используйте команду <openscada_Boiler>.
%description Model.Boiler -l de_DE.UTF8
Das Paket %{name}-Model.Boiler enthaelt Datenbanken und Konfigurationsdateien Modell "Kessel".
Das Modell ist verfügbar in Englisch, Ukrainisch und Russisch.
Fuers Starten wird Kommando <openscada_Boiler> benutzt.

%prep
%setup -q -n %srcname
%setup -T -D -a 1 -n %srcname

%build
%if %_vendor == "redhat" || %_vendor == "Mandriva" || %_vendor == "suse"
autoreconf -ivf
%endif

%configure

%if %_vendor == "redhat" || %_vendor == "suse"
%__make
%else
%make
%endif

%install
%makeinstall
rm -f %buildroot/%_libdir/openscada/*.la
install -m 755 -d %buildroot/%_datadir/openscada/{DATA,icons,docs,LibsDB,AGLKS,Boiler}
%if %_vendor == "alt"
install -m 755 -pD data/oscada_ALT.init %buildroot/%_initdir/openscada-server
install -m 755 -pD data/oscada_ALT.init %buildroot/%_initdir/openscada-plc
%else
install -m 755 -pD data/oscada_RH.init %buildroot/%_initdir/openscada-server
install -m 755 -pD data/oscada_RH.init %buildroot/%_initdir/openscada-plc
%endif
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

ln -s %_defaultdocdir/%name-docEN-%version %buildroot/%_datadir/openscada/docs/en
ln -s %_defaultdocdir/%name-docRU-%version %buildroot/%_datadir/openscada/docs/ru
ln -s %_defaultdocdir/%name-docUK-%version %buildroot/%_datadir/openscada/docs/uk

%clean
#rm -rf %buildroot %buildroot/%name-%version

%files
%defattr(-,root,root)
%config(noreplace) %_sysconfdir/oscada.xml
%config(noreplace) %_sysconfdir/oscada_start.xml
%_bindir/openscada
%_bindir/openscada_start
%_bindir/openscada-proj
%doc README README_ru README_uk COPYING ChangeLog INSTALL TODO TODO_ru TODO_uk
%_mandir/man1/*
%_mandir/*/man1/*
%_desktopdir/openscada.desktop
%_iconsdir/openscada.png
%_libdir/*.so.*
%_libdir/openscada/*.so
#exclude %_libdir/openscada/*.a
#exclude %_libdir/openscada/*.la
%_datadir/locale/*/LC_MESSAGES/*
%_datadir/openscada/icons/*
%exclude %_datadir/openscada/icons/AGLKS.png
%exclude %_datadir/openscada/icons/Boiler.png

%post plc
/sbin/chkconfig --add openscada-plc
/sbin/chkconfig openscada-plc on

%preun plc
/sbin/chkconfig --del openscada-plc

%files plc
%defattr(-,root,root)
%config(noreplace) %_sysconfdir/oscada_plc.xml
%config %_initdir/openscada-plc

%post server
/sbin/chkconfig --add openscada-server
/sbin/chkconfig openscada-server on

%preun server
/sbin/chkconfig --del openscada-server

%files server
%defattr(-,root,root)
%config(noreplace) %_sysconfdir/oscada_server.xml
%config %_initdir/openscada-server

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

%changelog
* Sun Jun 17 2018 Roman Savochenko <roman@oscada.org>
- The work branch switched to the version 1+r0000.

* Sat Mar 24 2018 Roman Savochenko <roman@oscada.org>
- The daemon mode init script "oscadad" renamed to "openscada-server" and separated to "openscada-plc".

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
- Updated for: noarch packages enable to all distribution, SuSE and Mandriva adaptation by default.

* Mon Mar 14 2011 Roman Savochenko <roman@oscada.org>
- New source packages implementation is released.
- Documentation separated by languages.
- Libraries DB packages is added for *-LibDB.Main and *-LibDB.VCA.
- Instead DemoDB package added Model's packages for "AGLKS" and "Boiler".

* Sun Oct 24 2010 Roman Savochenko <roman@oscada.org>
- Build 0.7.0 production release.

* Fri May 28 2010 Roman Savochenko <roman@oscada.org>
- Build 0.6.4.2 release.

* Mon Jan 25 2010 Roman Savochenko <roman@oscada.org>
- Build 0.6.4.1 release.

* Mon Oct 12 2009 Roman Savochenko <roman@oscada.org>
- Build 0.6.4 release.

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
- move the message arhives data to /var/spool/%{name}/ARHIVE/MESS

* Tue Apr 06 2004 Roman Savochenko <roman@oscada.org>
- make 3 packages: OpenScada, OpenScada-devel, OpenScada-testdata
- add languages: ru, uk
- make packages from 'make -dist' package;

* Wed Oct 15 2003 Roman Savochenko <roman@oscada.org>
- Starting
