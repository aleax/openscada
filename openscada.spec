#Relaxed mode for diamond board liraries build
%set_verify_elf_method textrel=relaxed

#===== Generic Info ======
Summary: Open SCADA system project
Name: openscada
Version: 0.4.0
Release: 1
Source: %{name}-%{version}.tar.gz
License: GPL
Group: Applications/SCADA
Packager: Roman Savochenko <rom_as@fromru.com>
# URL: 
BuildRoot: %{_tmppath}/%{name}-%{version}-root
PreReq: chkconfig

%description
Open SCADA system.
%description -l ru
Открытая SCADA система.
%description -l uk
В╕дкрита SCADA система.

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
Requires: %{name} = %{version}-%{release}
%description doc
The %{name}-doc package include documents files.
%description doc -l ru
Пакет %{name}-doc включает файлы документации.
%description doc -l uk
Пакет %{name}-doc включа╓ файли документац╕╖.


%package devel
Summary: Open SCADA development.
Group: Development/Libraries
Requires: %{name} = %{version}-%{release}
%description devel
The %{name}-devel package includes library archives and include files.
%description devel -l ru
Пакет %{name}-devel включает архив библиотек и включаемые файлы.
%description devel -l uk
Пакет %{name}-devel включа╓ арх╕в б╕бл╕отек та включа╓м╕ файли.


%package demo
Summary: Open SCADA demo data bases and config.
Group: Applications/SCADA
Requires: %{name} = %{version}-%{release}
%description demo
The %{name}-demo package includes demo data bases and configs. For start use command <openscada_demo>.
%description demo -l ru
Пакет %{name}-demo включает демонстрационные базы данных и конфигурации. Для старта используйте команду <openscada_demo>.
%description demo -l uk
Пакет %{name}-demo включа╓ демонстрац╕йн╕ бази даних та конф╕гурац╕╖. Для старту використовуйте команду <openscada_demo>.


%package athena
Summary: Open SCADA athena board build.
Group: Applications/SCADA
PreReq: chkconfig
%description athena
Build for PC104 board ATH400-128 from Diamond Systems.
%description athena -l ru
Сборка для PC104 платы ATH400-128 от Diamond Systems.
%description athena -l uk
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
rm -rf $RPM_BUILD_ROOT
%setup

%build
export CFLAGS=$RPM_OPT_FLAGS
export CXXFLAGS=$RPM_OPT_FLAGS
./configure --prefix=%{_prefix}
%{__make}

%install
rm -rf $RPM_BUILD_ROOT
%makeinstall
install -m 755 -d $RPM_BUILD_ROOT/%{_includedir}/%{name}/
install -m 644 *.h $RPM_BUILD_ROOT/%{_includedir}/%{name}
install -m 644 src/*.h $RPM_BUILD_ROOT/%{_includedir}/%{name}
install -m 644 -pD oscada.xml $RPM_BUILD_ROOT/%{_sysconfdir}/oscada.xml
install -m 755 -pD oscada.init $RPM_BUILD_ROOT/%{_initdir}/oscadad
install -m 644 demo/oscada_demo.xml $RPM_BUILD_ROOT/%{_sysconfdir}
install -m 755 demo/openscada_demo $RPM_BUILD_ROOT/%{_bindir}
install -m 755 -d $RPM_BUILD_ROOT/%{_datadir}/%{name}/DATA
echo "Open SCADA data dir" > $RPM_BUILD_ROOT/%{_datadir}/%{name}/DATA/.data
install -m 644 demo/*.db $RPM_BUILD_ROOT/%{_datadir}/%{name}/DATA
install -m 777 -d $RPM_BUILD_ROOT/var/spool/%{name}/ARCHIVES/MESS

%clean
rm -rf $RPM_BUILD_ROOT $RPM_BUILD_DIR/%{name}-%{version}

%files
%defattr(-,root,root)
%config(noreplace) %{_sysconfdir}/oscada.xml
%config %{_initdir}/oscadad
%{_bindir}/%{name}
%{_libdir}/*.so*
%{_libdir}/%{name}/*.so
%{_datadir}/locale/*/LC_MESSAGES/*

%files doc
%defattr(-,root,root)
%doc README COPYING INSTALL TODO ChangeLog doc/*

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
%{_libdir}/%{name}/cntr_OperationSystem.so
%{_libdir}/%{name}/cntr_BlockCalc.so
%{_libdir}/%{name}/cntr_DiamondBoards.so
%{_libdir}/%{name}/prot_HTTP.so
%{_libdir}/%{name}/spec_FreeFuncLibs.so
%{_libdir}/%{name}/spec_StaticFuncLibs.so
%{_libdir}/%{name}/spec_SystemTests.so
%{_libdir}/%{name}/tr_Sockets.so
%{_libdir}/%{name}/ui_WebCfg.so

%{_datadir}/%{name}/DATA/.data
%{_datadir}/locale/*/LC_MESSAGES/*
/var/spool/%{name}/

%files demo
%defattr(-,root,root)
%config(noreplace) %{_sysconfdir}/oscada_demo.xml
%{_bindir}/openscada_demo
%{_datadir}/%{name}/
/var/spool/%{name}/

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


