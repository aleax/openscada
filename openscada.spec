#===== Generic Info ======
Summary: Open SCADA system project
Name: openscada
Version: 0.6.3.2
Release: 1
Source: openscada-%version.tar.gz
License: GPL
Group: Graphics
Packager: Roman Savochenko <rom_as@fromru.com>
URL: http://oscada.org.ua

%define srcname openscada-%version

#= Individual distributives seting =
%if %_vendor == "alt"
%set_verify_elf_method no
BuildRequires: glibc-devel gcc4.3-c++ libgd2-devel libexpat-devel libMySQL-devel libsqlite3-devel libsensors-devel libnet-snmp-devel libqt4-devel firebird-devel libportaudio2-devel libfftw3-devel
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

%post
test -e /usr/lib/openscada || ln -s %_libdir/openscada /usr/lib/openscada
/sbin/chkconfig --add oscadad
/sbin/chkconfig oscadad off

%preun
test -h /usr/lib/openscada && rm -f /usr/lib/openscada || :;
/sbin/chkconfig --del oscadad

%package doc
Summary: Open SCADA documents.
Group: Documentation
%description doc
The %name-doc package include documents files.
%description doc -l ru_RU.UTF8
Пакет %name-doc включает файлы документации.
%description doc -l uk_UA.UTF8
Пакет %name-doc включає файли документації.


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
%if %_vendor == "alt"
BuildArch: noarch
%endif
Requires: %name
%description demo
The %{name}-demo package includes demo data bases and configs. For start use command <openscada_demo_en>. For access use account "root" and password "openscada" or account "user" without password.
%description demo -l ru_RU.UTF8
Пакет %{name}-demo включает демонстрационные базы данных и конфигурации. Для старта используйте команду <openscada_demo_ru>. Для доступа используйте запись "root" и пароль "openscada" или запись "user" без пароля.
%description demo -l uk_UA.UTF8
Пакет %{name}-demo включає демонстраційні бази даних та конфігурації. Для старту використовуйте команду <openscada_demo_ru>. Для доступу використовуйте запис "root" та пароль "openscada" або запис "user" без пароля.


%prep
%setup -q -n %srcname

%build
%configure CFLAGS="-O2" CXXFLAGS="-O2 -Wno-deprecated"
%__make

%install
%makeinstall
install -m 755 -d %buildroot/%_includedir/openscada/
install -m 644 *.h %buildroot/%_includedir/openscada
install -m 644 src/*.h %buildroot/%_includedir/openscada
install -m 644 -pD data/oscada.xml %buildroot/%_sysconfdir/oscada.xml
install -m 644 -pD data/oscada_start.xml %buildroot/%_sysconfdir/oscada_start.xml
install -m 755 -pD data/openscada_start %buildroot/%_bindir/openscada_start
install -m 644 -pD data/openscada.desktop %buildroot/%_desktopdir/openscada.desktop
install -m 644 -pD data/openscada.png %buildroot/%_iconsdir/openscada.png
install -m 755 -pD data/oscada.init %buildroot/%_initdir/oscadad
install -m 755 -d %buildroot/var/spool/openscada/DATA
install -m 755 -d %buildroot/var/spool/openscada/icons
echo "OpenSCADA data dir" > %buildroot/var/spool/openscada/DATA/.info
install -m 644 data/icons/* %buildroot/var/spool/openscada/icons
install -m 755 -d %buildroot/var/spool/openscada/ARCHIVES/MESS
install -m 755 -d %buildroot/var/spool/openscada/ARCHIVES/VAL
echo "OpenSCADA messages archive dir" > %buildroot/var/spool/openscada/ARCHIVES/MESS/.info
echo "OpenSCADA values archive dir" > %buildroot/var/spool/openscada/ARCHIVES/VAL/.info
install -m 644 -pD demo/en/oscada_demo_en.xml %buildroot/%_sysconfdir/oscada_demo_en.xml
install -m 755 -pD demo/en/openscada_demo_en %buildroot/%_bindir/openscada_demo_en
install -m 644 -pD demo/en/openscada_demo_en.desktop %buildroot/%_desktopdir/openscada_demo_en.desktop
install -m 644 -pD demo/en/openscada_demo_en.png %buildroot/%_iconsdir/openscada_demo_en.png
install -m 755 -d %buildroot/var/spool/openscada/DEMO_EN
install -m 644 demo/en/*.db %buildroot/var/spool/openscada/DEMO_EN
install -m 644 -pD demo/ru/oscada_demo_ru.xml %buildroot/%_sysconfdir/oscada_demo_ru.xml
install -m 755 -pD demo/ru/openscada_demo_ru %buildroot/%_bindir/openscada_demo_ru
install -m 644 -pD demo/ru/openscada_demo_ru.desktop %buildroot/%_desktopdir/openscada_demo_ru.desktop
install -m 644 -pD demo/ru/openscada_demo_ru.png %buildroot/%_iconsdir/openscada_demo_ru.png
install -m 755 -d %buildroot/var/spool/openscada/DEMO_RU
install -m 644 demo/ru/*.db %buildroot/var/spool/openscada/DEMO_RU

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
%_libdir/openscada/*.so
%_datadir/locale/*/LC_MESSAGES/*
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
%config(noreplace) %_sysconfdir/oscada_demo_en.xml
%_bindir/openscada_demo_en
%_desktopdir/openscada_demo_en.desktop
%_iconsdir/openscada_demo_en.png
/var/spool/openscada/DEMO_EN/*.db
%config(noreplace) %_sysconfdir/oscada_demo_ru.xml
%_bindir/openscada_demo_ru
%_desktopdir/openscada_demo_ru.desktop
%_iconsdir/openscada_demo_ru.png
/var/spool/openscada/DEMO_RU/*.db

%changelog
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
