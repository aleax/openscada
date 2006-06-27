#Relaxed mode for diamond board liraries build
%set_verify_elf_method textrel=relaxed
#define _initdir /etc/init.d

#===== Generic Info ======
Summary: Open SCADA system project
Name: openscada
Version: 0.5.0
Release: 1
Source: %{name}-%{version}.tar.gz
License: GPL
Group: Applications/SCADA
Packager: Roman Savochenko <rom_as@fromru.com>
URL: http://diyaorg.dp.ua/oscada
#BuildRoot: %{_tmppath}/%{name}-%{version}-root
PreReq: chkconfig
Conflicts: %{name}-athena

%description
Open SCADA system. For access use account "root" and password "openscada".
%description -l ru_RU.KOI8-R
Открытая SCADA система. Для доступа используйте запись "root" и пароль "openscada".
%description -l uk_UA.KOI8-U
В╕дкрита SCADA система. Для доступу використовуйте запис "root" та пароль "openscada".

%post
if [ $1 -ge 2 ]; then
    /sbin/service oscadad condreload ||:
else
    /sbin/chkconfig --add oscadad ||:
fi
	
%preun
if [ $1 = 0 ]; then
    /sbin/chkconfig --del oscadad ||:
fi


%package doc
Summary: Open SCADA documents.
Group: Applications/SCADA
#Requires: %{name}
%description doc
The %{name}-doc package include documents files.
%description doc -l ru_RU.KOI8-R
Пакет %{name}-doc включает файлы документации.
%description doc -l uk_UA.KOI8-U
Пакет %{name}-doc включа╓ файли документац╕╖.


%package devel
Summary: Open SCADA development.
Group: Development/Libraries
#Requires: %{name}
%description devel
The %{name}-devel package includes library archives and include files.
%description devel -l ru_RU.KOI8-R
Пакет %{name}-devel включает архив библиотек и включаемые файлы.
%description devel -l uk_UA.KOI8-U
Пакет %{name}-devel включа╓ арх╕в б╕бл╕отек та включа╓м╕ файли.


%package demo
Summary: Open SCADA demo data bases and config.
Group: Applications/SCADA
Requires: %{name}
%description demo
The %{name}-demo package includes demo data bases and configs. For start use command <openscada_demo>. For access use account "root" and password "openscada" or account "user" without password.
%description demo -l ru_RU.KOI8-R
Пакет %{name}-demo включает демонстрационные базы данных и конфигурации. Для старта используйте команду <openscada_demo>. Для доступа используйте запись "root" и пароль "openscada" или запись "user" без пароля.
%description demo -l uk_UA.KOI8-U
Пакет %{name}-demo включа╓ демонстрац╕йн╕ бази даних та конф╕гурац╕╖. Для старту використовуйте команду <openscada_demo>. Для доступу використовуйте запис "root" та пароль "openscada" або запис "user" без пароля.


%package athena
Summary: Open SCADA athena board build.
Group: Applications/SCADA
PreReq: chkconfig
Conflicts: %{name}
%description athena
Build for PC104 board ATH400-128 from Diamond Systems.
%description athena -l ru_RU.KOI8-R
Сборка для PC104 платы ATH400-128 от Diamond Systems.
%description athena -l uk_UA.KOI8-U
Зб╕рка для PC104 плати ATH400-128 в╕д Diamond Systems.

%post athena
if [ $1 -ge 2 ]; then
    /sbin/service oscadad condreload ||:
else
    /sbin/chkconfig --add oscadad ||:
fi
	
%preun athena
if [ $1 = 0 ]; then
    /sbin/chkconfig --del oscadad ||:
fi


%prep
#rm -rf $RPM_BUILD_ROOT
%setup

%build
export CFLAGS=$RPM_OPT_FLAGS
export CXXFLAGS=$RPM_OPT_FLAGS
./configure --prefix=%{_prefix}
%{__make}

%install
#rm -rf $RPM_BUILD_ROOT
%makeinstall
install -m 755 -d $RPM_BUILD_ROOT/%{_includedir}/%{name}/
install -m 644 *.h $RPM_BUILD_ROOT/%{_includedir}/%{name}
install -m 644 src/*.h $RPM_BUILD_ROOT/%{_includedir}/%{name}
install -m 644 -pD data/oscada.xml $RPM_BUILD_ROOT/%{_sysconfdir}/oscada.xml
install -m 755 -pD data/oscada.init $RPM_BUILD_ROOT/%{_initdir}/oscadad
install -m 644 demo/oscada_demo.xml $RPM_BUILD_ROOT/%{_sysconfdir}
install -m 755 demo/openscada_demo $RPM_BUILD_ROOT/%{_bindir}
install -m 777 -d $RPM_BUILD_ROOT/var/spool/%{name}/DATA
install -m 755 -d $RPM_BUILD_ROOT/var/spool/%{name}/icons
echo "Open SCADA data dir" > $RPM_BUILD_ROOT/var/spool/%{name}/DATA/.data
install -m 666 demo/*.db $RPM_BUILD_ROOT/var/spool/%{name}/DATA
install -m 644 data/icons/* $RPM_BUILD_ROOT/var/spool/%{name}/icons
install -m 777 -d $RPM_BUILD_ROOT/var/spool/%{name}/ARCHIVES/MESS
install -m 777 -d $RPM_BUILD_ROOT/var/spool/%{name}/ARCHIVES/VAL
install -m 777 -d $RPM_BUILD_ROOT/var/spool/%{name}/VISION
install -m 666 demo/VISION/* $RPM_BUILD_ROOT/var/spool/%{name}/VISION

%clean
#rm -rf $RPM_BUILD_ROOT $RPM_BUILD_DIR/%{name}-%{version}

%files
%defattr(-,root,root)
%config(noreplace) %{_sysconfdir}/oscada.xml
%config %{_initdir}/oscadad
%{_bindir}/%{name}
%{_libdir}/*.so*
%{_libdir}/%{name}/*.so
%{_datadir}/locale/*/LC_MESSAGES/*
/var/spool/%{name}/DATA/.data
/var/spool/%{name}/icons/*
/var/spool/%{name}/ARCHIVES/

%files doc
%defattr(-,root,root)
%{_datadir}/doc
#doc README README_ru COPYING INSTALL TODO ChangeLog doc/*

%files devel
%defattr(-,root,root)
%{_libdir}/*.*
#%{_libdir}/*.la
#%{_libdir}/*.so*
%{_includedir}/%{name}/*

%files athena
%defattr(-,root,root)
%config(noreplace) %{_sysconfdir}/oscada.xml
%config %{_initdir}/oscadad
%{_bindir}/%{name}
%{_libdir}/*.so*
%{_libdir}/%{name}/arh_BaseArh.so
%{_libdir}/%{name}/bd_DBF.so
%{_libdir}/%{name}/bd_SQLite.so
%{_libdir}/%{name}/daq_OperationSystem.so
%{_libdir}/%{name}/daq_BlockCalc.so
%{_libdir}/%{name}/daq_DiamondBoards.so
%{_libdir}/%{name}/daq_JavaLikeCalc.so
%{_libdir}/%{name}/prot_HTTP.so
%{_libdir}/%{name}/spec_FLibComplex1.so
%{_libdir}/%{name}/spec_FLibMath.so
%{_libdir}/%{name}/spec_FLibTime.so
%{_libdir}/%{name}/spec_SystemTests.so
%{_libdir}/%{name}/tr_Sockets.so
%{_libdir}/%{name}/ui_WebCfg.so

/var/spool/%{name}/DATA/.data
/var/spool/%{name}/ARCHIVES/
%{_datadir}/locale/*/LC_MESSAGES/*

%files demo
%defattr(-,root,root)
%config(noreplace) %{_sysconfdir}/oscada_demo.xml
%{_bindir}/openscada_demo
/var/spool/%{name}/
#/var/spool/%{name}/VISION

%changelog
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


