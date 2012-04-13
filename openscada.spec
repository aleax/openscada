#===== Generic Info ======
Summary: Open SCADA system.
Summary(ru_RU.UTF8): Открытая SCADA система.
Summary(uk_UA.UTF8): Відкрита SCADA система.
Summary(de_DE.UTF8): Open SCADA-System.
Name: openscada
Version: 0.8.0
Release: 1
Source: openscada-%version.tar.lzma
Source1: openscada-res-%version.tar.lzma
License: GPLv2
Group: Applications/Engineering
Packager: Roman Savochenko <rom_as@oscada.org, rom_as@fromru.com>
URL: http://oscada.org

%define srcname openscada-%version

#= Individual distributives seting =
%if %_vendor == "alt"
%set_verify_elf_method no
BuildRequires: glibc-devel gcc-c++-common libgd2-devel libMySQL-devel libsqlite3-devel libsensors3-devel
BuildRequires: libnet-snmp-devel libqt4-devel firebird-devel postgresql9.0-devel libportaudio2-devel libfftw3-devel libpcre-devel
%else
%define _initdir /etc/init.d
%define _desktopdir %_datadir/applications
%define _iconsdir /usr/share/icons
%define _pkgconfigdir %_libdir/pkgconfig
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

%post
/sbin/chkconfig --add oscadad

%preun
/sbin/chkconfig --del oscadad

%package docEN
Summary: Open SCADA documents (EN).
Summary(ru_RU.UTF8): Документация открытой SCADA (EN).
Summary(uk_UA.UTF8): Документація відкритої SCADA (EN).
Summary(de_DE.UTF8): Open SCADA Dokumente (EN).
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
Summary: Open SCADA documents (RU).
Summary(ru_RU.UTF8): Документация открытой SCADA (RU).
Summary(uk_UA.UTF8): Документація відкритої SCADA (RU).
Summary(de_DE.UTF8): Open SCADA Dokumente (RU).
Group: Documentation
BuildArch: noarch
%description docRU
The %name-docRU package include documents files (Russian).
%description docRU -l ru_RU.UTF8
Пакет %name-docRU включает файлы документации (Русский).
%description docRU -l uk_UA.UTF8
Пакет %name-docRU включає файли документації (Російська).
%description docRU -l de_DE.UTF8
Das Paket %name-docRU enthaelt Dokumentationsdateien (Russisch).

%package docUK
Summary: Open SCADA documents (UK).
Summary(ru_RU.UTF8): Документация открытой SCADA (UK).
Summary(uk_UA.UTF8): Документація відкритої SCADA (UK).
Summary(de_DE.UTF8): Open SCADA Dokumente (UK).
Group: Documentation
BuildArch: noarch
%description docUK
The %name-docUK package include documents files (Ukraine).
%description docUK -l ru_RU.UTF8
Пакет %name-docUK включает файлы документации (Украинский).
%description docUK -l uk_UA.UTF8
Пакет %name-docUK включає файли документації (Українська).
%description docUK -l de_DE.UTF8
Das Paket %name-docUK enthaelt Dokumentationsdateien (Ukrainisch).

%package devel
Summary: Open SCADA development.
Summary(ru_RU.UTF8): Разработка открытой SCADA.
Summary(uk_UA.UTF8): Розробка відкритої SCADA.
Summary(de_DE.UTF8): Open SCADA Entwicklung.
Group: Development/C++
Requires: %name = %version-%release
%description devel
The %name-devel package includes library archives and include files.
%description devel -l ru_RU.UTF8
Пакет %name-devel включает архив библиотек и включаемые файлы.
%description devel -l uk_UA.UTF8
Пакет %name-devel включає архів бібліотек та включаємі файли.
%description devel -l de_DE.UTF8
Das Paket %name-devel enthaelt die Bibliotheken und Archive Include-Dateien.

%package LibDB.Main
Summary: Main OpenSCADA libraries for DAQ and other into SQLite DB.
Summary(ru_RU.UTF8): Основные библиотеки OpenSCADA для сбора данных и другого в БД SQLite.
Summary(uk_UA.UTF8): Основні бібліотеки OpenSCADA для збору даних та іншого у БД SQLite.
Summary(de_DE.UTF8): Hauptbibliothek OpenSCADA für die Datenerhebung und die anderen in der Datenbank SQLite.
Group: Graphics
BuildArch: noarch
Requires: %name
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
Summary: Visual components libraries into SQLite DB.
Summary(ru_RU.UTF8): Библиотеки визуальных компонетов в БД SQLite.
Summary(uk_UA.UTF8): Бібліотеки візуальних компонентів у БД SQLite.
Summary(de_DE.UTF8): Visuelle Komponente in einer Bibliothek Datenbank SQLite.
Group: Graphics
BuildArch: noarch
Requires: %name
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
Summary: Model "AGLKS" data bases and config (Demo: EN,RU,UK).
Summary(ru_RU.UTF8): БД и конфигурация модели "АГЛКС" (Демо: EN,RU,UK).
Summary(uk_UA.UTF8): БД та конфігурація моделі "АГЛКС" (Демо: EN,RU,UK).
Summary(de_DE.UTF8): Datenbanken und Konfigurationsdateien Modell "AGLKS" (Demo: EN,RU,UK).
Group: Graphics
Requires: %name %name-LibDB.Main %name-LibDB.VCA
%description Model.AGLKS
The %{name}-Model.AGLKS package includes model "AGLKS" data bases and config.
The Model is used for OpenSCADA demo and allowed for English, Russian and Ukrainian languages.
For start use command <openscada_demo> or <openscada_AGLKS>.
For access use account "root" and password "openscada" or account "user" and password "user".
%description Model.AGLKS -l ru_RU.UTF8
Пакет %{name}-Model.AGLKS включает БД и конфигурацию модели "АГЛКС".
Модель используется для демонстрации OpenSCADA и доступна на Английском, Русском и Украинском языках.
Для старта используйте команду <openscada_demo> мли <openscada_AGLKS>.
Для доступа используйте запись "root" и пароль "openscada" или запись "user" и пароль "user".
%description Model.AGLKS -l uk_UA.UTF8
Пакет %{name}-Model.AGLKS включає БД та конфігурацію моделі "АГЛКС".
Модель використано для демонстрації OpenSCADA та доступно на Англійській, Російській та Українській мовах.
Для старту використовуйте команду <openscada_demo> та <openscada_AGLKS>.
Для доступу використовуйте запис "root" та пароль "openscada" або запис "user" та пароль "user".
%description Model.AGLKS -l de_DE.UTF8
Das Paket %{name}-Model.AGLKS enthaelt Datenbanken und Konfigurationsdateien Modell "AGLKS".
Das Modell wird verwendet, um OpenSCADA demonstrieren und ist verfügbar in Englisch, Russisch und Ukrainisch.
Fuers Starten wird Kommando <openscada_demo> oder <openscada_AGLKS> benutzt.
Fuer den Zugang die Aufzeichnung "root" und das Kennwort "openscada" benutzen.

%package Model.Boiler
Summary: Model "Boiler" data bases and config (EN,RU,UK).
Summary(ru_RU.UTF8): БД и конфигурация модели "Котёл" (EN,RU,UK).
Summary(uk_UA.UTF8): БД та конфігурація моделі "Котел" (EN,RU,UK).
Summary(de_DE.UTF8): Datenbanken und Konfigurationsdateien Modell "Kessel" (EN,RU,UK).
Group: Graphics
Requires: %name %name-LibDB.Main %name-LibDB.VCA
%description Model.Boiler
The %{name}-Model.Boiler package model "Boiler" data bases and config.
The Model is allowed for English, Russian and Ukrainian languages.
For start use command <openscada_Boiler>.
For access use account "root" and password "openscada" or account "user" and password "user".
%description Model.Boiler -l ru_RU.UTF8
Пакет %{name}-Model.Boiler включает БД и конфигурацию модели "Котёл".
Модель доступна на Английском, Русском и Украинском языках.
Для старта используйте команду <openscada_Boiler>.
Для доступа используйте запись "root" и пароль "openscada" или запись "user" и пароль "user".
%description Model.Boiler -l uk_UA.UTF8
Пакет %{name}-Model.Boiler включає БД та конфігурацію моделі "Котел".
Модель доступно на Англійській, Російській та Українській мовах.
Для старту використовуйте команду <openscada_Boiler>.
Для доступу використовуйте запис "root" та пароль "openscada" або запис "user" та пароль "user".
%description Model.Boiler -l de_DE.UTF8
Das Paket %{name}-Model.Boiler enthaelt Datenbanken und Konfigurationsdateien Modell "Kessel".
Das Modell ist verfügbar in Englisch, Russisch und Ukrainisch.
Fuers Starten wird Kommando <openscada_Boiler> benutzt.
Fuer den Zugang die Aufzeichnung "root" und das Kennwort "openscada" benutzen.


%prep
%setup -q -n %srcname
%setup -T -D -a 1

%build
%if %_vendor == "Mandriva"
autoreconf -ivf
%endif

%configure

%if %_vendor == "suse"
%__make
%else
%make
%endif

%install
%makeinstall
rm -f %buildroot/%_libdir/openscada/*.la
install -m 755 -d %buildroot/var/spool/openscada/{DATA,icons,LibsDB,AGLKS,Boiler}
install -m 755 -d %buildroot/var/spool/openscada/ARCHIVES/{MESS,VAL}
install -m 644 -pD data/oscada.xml %buildroot/%_sysconfdir/oscada.xml
install -m 644 -pD data/oscada_start.xml %buildroot/%_sysconfdir/oscada_start.xml
install -m 755 -pD data/openscada_start %buildroot/%_bindir/openscada_start
install -m 644 -pD data/openscada.desktop %buildroot/%_desktopdir/openscada.desktop
install -m 644 -pD data/openscada.png %buildroot/%_iconsdir/openscada.png
install -m 755 -pD data/oscada_ALT.init %buildroot/%_initdir/oscadad
echo "OpenSCADA data dir" > %buildroot/var/spool/openscada/DATA/.info
install -m 644 data/icons/* %buildroot/var/spool/openscada/icons
echo "OpenSCADA messages archive dir" > %buildroot/var/spool/openscada/ARCHIVES/MESS/.info
echo "OpenSCADA values archive dir" > %buildroot/var/spool/openscada/ARCHIVES/VAL/.info

install -m 644 data/LibsDB/*.db %buildroot/var/spool/openscada/LibsDB

install -m 644 data/ModelsDB/AGLKS/*.db %buildroot/var/spool/openscada/AGLKS
install -m 644 -pD data/ModelsDB/AGLKS/oscada_AGLKS.xml %buildroot/%_sysconfdir/oscada_AGLKS.xml
install -m 755 -pD data/ModelsDB/AGLKS/openscada_AGLKS %buildroot/%_bindir/openscada_AGLKS
install -m 755 -pD data/ModelsDB/AGLKS/openscada_demo %buildroot/%_bindir/openscada_demo
install -m 644 -pD data/ModelsDB/AGLKS/openscada_AGLKS.desktop %buildroot/%_desktopdir/openscada_AGLKS.desktop
install -m 644 -pD data/ModelsDB/AGLKS/openscada_AGLKS.png %buildroot/%_iconsdir/openscada_AGLKS.png
install -m 644 -pD data/ModelsDB/AGLKS/openscada_AGLKS.png %buildroot/var/spool/openscada/icons/AGLKS.png

install -m 644 data/ModelsDB/Boiler/*.db %buildroot/var/spool/openscada/Boiler
install -m 644 -pD data/ModelsDB/Boiler/oscada_Boiler.xml %buildroot/%_sysconfdir/oscada_Boiler.xml
install -m 755 -pD data/ModelsDB/Boiler/openscada_Boiler %buildroot/%_bindir/openscada_Boiler
install -m 644 -pD data/ModelsDB/Boiler/openscada_Boiler.desktop %buildroot/%_desktopdir/openscada_Boiler.desktop
install -m 644 -pD data/ModelsDB/Boiler/openscada_Boiler.png %buildroot/%_iconsdir/openscada_Boiler.png
install -m 644 -pD data/ModelsDB/Boiler/openscada_Boiler.png %buildroot/var/spool/openscada/icons/Boiler.png

sed -i 's|/usr/lib|%_libdir|' %buildroot/%_sysconfdir/oscada*.xml

%clean
#rm -rf %buildroot %buildroot/%name-%version

%files
%defattr(-,root,root)
%config(noreplace) %_sysconfdir/oscada.xml
%config(noreplace) %_sysconfdir/oscada_start.xml
%config %_initdir/oscadad
%_bindir/openscada
%_bindir/openscada_start
%doc README README_ru README_uk COPYING ChangeLog INSTALL TODO TODO_ru TODO_uk
%_desktopdir/openscada.desktop
%_iconsdir/openscada.png
%_libdir/*.so.*
%_libdir/openscada/*.so
#exclude %_libdir/openscada/*.a
#exclude %_libdir/openscada/*.la
%_datadir/locale/*/LC_MESSAGES/*
/var/spool/openscada/DATA/.info
/var/spool/openscada/icons/*
%exclude /var/spool/openscada/icons/AGLKS.png
%exclude /var/spool/openscada/icons/Boiler.png
/var/spool/openscada/ARCHIVES/MESS/.info
/var/spool/openscada/ARCHIVES/VAL/.info

%files docEN
%defattr(-,root,root)
%doc doc/en/*

%files docRU
%defattr(-,root,root)
%doc doc/ru/*

%files docUK
%defattr(-,root,root)
%doc doc/uk/*

%files devel
%defattr(-,root,root)
%_libdir/*.so
%_libdir/*.*a
%_includedir/openscada/*
%_pkgconfigdir/openscada.pc

%files LibDB.Main
%defattr(-,root,root)
/var/spool/openscada/LibsDB/OscadaLibs.db

%files LibDB.VCA
%defattr(-,root,root)
/var/spool/openscada/LibsDB/vca*.db

%files Model.AGLKS
%defattr(-,root,root)
%config(noreplace) %_sysconfdir/oscada_AGLKS.xml
%_bindir/openscada_AGLKS
%_bindir/openscada_demo
%_desktopdir/openscada_AGLKS.desktop
%_iconsdir/openscada_AGLKS.png
/var/spool/openscada/icons/AGLKS.png
/var/spool/openscada/AGLKS/*.db

%files Model.Boiler
%defattr(-,root,root)
%config(noreplace) %_sysconfdir/oscada_Boiler.xml
%_bindir/openscada_Boiler
%_desktopdir/openscada_Boiler.desktop
%_iconsdir/openscada_Boiler.png
/var/spool/openscada/icons/Boiler.png
/var/spool/openscada/Boiler/*.db

%changelog
* Fri Apr 06 2012 Roman Savochenko <rom_as@oscada.org>
- Build 0.8.0 release.

* Mon Dec 05 2011 Roman Savochenko <rom_as@oscada.org>
- Build 0.7.2 release.

* Mon Apr 18 2011 Roman Savochenko <rom_as@oscada.org>
- Updated for: noarch packages enable to all distribution, SuSE and Mandriva adaptation by default.

* Mon Mar 14 2011 Roman Savochenko <rom_as@oscada.org>
- New source packages implementation is released.
- Documentation separated by languages.
- Libraries DB packages is added for *-LibDB.Main and *-LibDB.VCA.
- Instead DemoDB package added Model's packages for "AGLKS" and "Boiler".

* Sun Oct 24 2010 Roman Savochenko <rom_as@oscada.org>
- Build 0.7.0 production release.

* Fri May 28 2010 Roman Savochenko <rom_as@oscada.org>
- Build 0.6.4.2 release.

* Mon Jan 25 2010 Roman Savochenko <rom_as@oscada.org>
- Build 0.6.4.1 release.

* Mon Oct 12 2009 Roman Savochenko <rom_as@oscada.org>
- Build 0.6.4 release.

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
- move the message arhives data to /var/spool/%{name}/ARHIVE/MESS

* Tue Apr 06 2004 Roman Savochenko <rom_as@fromru.com>
- make 3 packages: OpenScada, OpenScada-devel, OpenScada-testdata
- add languages: ru, uk
- make packages from 'make -dist' package;

* Thu Oct 15 2003 Roman Savochenko <rom_as@fromru.com>
- Starting
