#===== Generic Info ======
Summary: Open SCADA system project
Name: openscada
Version: 0.6.1
Release: 1
Source: %name-%version.tar.gz
License: GPL
Group: Applications/SCADA
Packager: Roman Savochenko <rom_as@fromru.com>
URL: http://oscada.diyaorg.dp.ua

#= Individual distributives seting =
%if %_vendor == "alt"
%set_verify_elf_method no
%else
%define _initdir /etc/init.d
%define _desktopdir %_datadir/applications
%endif
%if %_vendor == "suse" || %_vendor == "redhat"
BuildRoot: %_tmppath/%name-%version-root
%endif

%description
Open SCADA system. For access use account "root" and password "openscada".
%description -l ru_RU.UTF8
Открытая SCADA система. Для доступа используйте запись "root" и пароль "openscada".
%description -l uk_UA.UTF8
Відкрита SCADA система. Для доступу використовуйте запис "root" та пароль "openscada".

%post
test -e /usr/lib/%name || ln -s %_libdir/%name /usr/lib/%name
/sbin/chkconfig --add oscadad
/sbin/chkconfig oscadad off

%preun
test -h /usr/lib/%name && rm -f /usr/lib/%name 
/sbin/chkconfig --del oscadad

%package doc
Summary: Open SCADA documents.
Group: Applications/SCADA
%description doc
The %name-doc package include documents files.
%description doc -l ru_RU.UTF8
Пакет %name-doc включает файлы документации.
%description doc -l uk_UA.UTF8
Пакет %name-doc включає файли документації.


%package devel
Summary: Open SCADA development.
Group: Development/Libraries
#Requires: %name
%description devel
The %name-devel package includes library archives and include files.
%description devel -l ru_RU.UTF8
Пакет %name-devel включает архив библиотек и включаемые файлы.
%description devel -l uk_UA.UTF8
Пакет %name-devel включає архів бібліотек та включаємі файли.


%package demo
Summary: Open SCADA demo data bases and config.
Group: Applications/SCADA
Requires: %name
%description demo
The %{name}-demo package includes demo data bases and configs. For start use command <openscada_demo>. For access use account "root" and password "openscada" or account "user" without password.
%description demo -l ru_RU.UTF8
Пакет %{name}-demo включает демонстрационные базы данных и конфигурации. Для старта используйте команду <openscada_demo>. Для доступа используйте запись "root" и пароль "openscada" или запись "user" без пароля.
%description demo -l uk_UA.UTF8
Пакет %{name}-demo включає демонстраційні бази даних та конфігурації. Для старту використовуйте команду <openscada_demo>. Для доступу використовуйте запис "root" та пароль "openscada" або запис "user" без пароля.


%prep
%setup

%build
%configure
%__make

%install
%makeinstall
install -m 755 -d %buildroot/%_includedir/%name/
install -m 644 *.h %buildroot/%_includedir/%name
install -m 644 src/*.h %buildroot/%_includedir/%name
install -m 644 -pD data/oscada.xml %buildroot/%_sysconfdir/oscada.xml
install -m 644 -pD data/openscada.desktop %buildroot/%_desktopdir/openscada.desktop
install -m 644 -pD data/openscada.png %buildroot/%_iconsdir/openscada.png
install -m 755 -pD data/oscada.init %buildroot/%_initdir/oscadad
install -m 644 -pD demo/oscada_demo.xml %buildroot/%_sysconfdir/oscada_demo.xml
install -m 755 -pD demo/openscada_demo %buildroot/%_bindir/openscada_demo
install -m 644 -pD demo/openscada_demo.desktop %buildroot/%_desktopdir/openscada_demo.desktop
install -m 644 -pD demo/openscada_demo.png %buildroot/%_iconsdir/openscada_demo.png
install -m 777 -d %buildroot/var/spool/%name/DATA
install -m 755 -d %buildroot/var/spool/%name/icons
echo "Open SCADA data dir" > %buildroot/var/spool/%name/DATA/.data
install -m 666 demo/*.db %buildroot/var/spool/%name/DATA
install -m 644 data/icons/* %buildroot/var/spool/%name/icons
install -m 777 -d %buildroot/var/spool/%name/ARCHIVES/MESS
install -m 777 -d %buildroot/var/spool/%name/ARCHIVES/VAL

%clean
#rm -rf %buildroot %buildroot/%name-%version

%files
%defattr(-,root,root)
%config(noreplace) %_sysconfdir/oscada.xml
%config %_initdir/oscadad
%_bindir/%name
%_desktopdir/openscada.desktop
%_iconsdir/openscada.png
%_libdir/*.so*
%_libdir/%name/*.so
%_datadir/locale/*/LC_MESSAGES/*
/var/spool/%name/DATA/.data
/var/spool/%name/icons/*
/var/spool/%name/ARCHIVES/

%files doc
%defattr(-,root,root)
%_datadir/doc
#doc README README_ru COPYING INSTALL TODO TODO_ru TODO_uk ChangeLog doc/*

%files devel
%defattr(-,root,root)
%_libdir/*.*
%_includedir/%name/*

%files demo
%defattr(-,root,root)
%config(noreplace) %_sysconfdir/oscada_demo.xml
%_bindir/openscada_demo
%_desktopdir/openscada_demo.desktop
%_iconsdir/openscada_demo.png
/var/spool/%name/DATA/

%changelog
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
