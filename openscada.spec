Summary: Open SCADA system project
Name: openscada
Version: 0.3.0
Release: 1
Source: %{name}-%{version}.tar.gz
License: GPL
Group: Applications/SCADA
Packager: Roman Savochenko <rom_as@fromru.com>
# URL: 
BuildRoot: %{_tmppath}/%{name}-%{version}-root

%description
Open SCADA system.
%description -l ru
Открытая SCADA система.
%description -l uk
В╕дкрита SCADA система.

%package devel
Summary: Open SCADA development.
Group: Development/Libraries
Requires: %{name} >= %{version}-%{release}
Conflicts: %{name} < %{version}
%description devel
The %{name}-devel package includes library archives and include files.
%description devel -l ru
Пакет %{name}-devel включает архив библиотек и включаемые файлы.
%description devel -l uk
Пакет %{name}-devel включа╓ арх╕в б╕бл╕отек та включа╓м╕ файли.


%package testdata
Summary: Open SCADA test data bases and configs.
Group: Applications/SCADA
Requires: %{name} >= %{version}-%{release}
Conflicts: %{name} < %{version}
%description testdata
The %{name}-testdata package includes test config and BD.
%description testdata -l ru
Пакет %{name}-testdata включает тестовую конфигурацию и БД.
%description testdata -l uk
Пакет %{name}-testdata включа╓ тестову конф╕гурац╕ю та БД.


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
mkdir -p $RPM_BUILD_ROOT/etc
install -m 644 test/oscada.xml $RPM_BUILD_ROOT/etc
install -m 755 test/OScadaTest $RPM_BUILD_ROOT/%{_bindir}
install -m 755 -d $RPM_BUILD_ROOT/%{_datadir}/%{name}/DATA
install -m 755 -d $RPM_BUILD_ROOT/%{_datadir}/%{name}/CFG
install -m 777 -d $RPM_BUILD_ROOT/var/spool/%{name}/ARHIVE/MESS
install -m 644 test/DATA/*.dbf $RPM_BUILD_ROOT/%{_datadir}/%{name}/DATA
install -m 644 test/CFG/*.cfg $RPM_BUILD_ROOT/%{_datadir}/%{name}/CFG

%clean
rm -rf $RPM_BUILD_ROOT $RPM_BUILD_DIR/%{name}-%{version}

%files
%defattr(-,root,root)
%doc README COPYING INSTALL TODO ChangeLog doc/OpenScadaUMLdescr.sxw
%{_bindir}/%{name}
%{_libdir}/*.so*
%{_libdir}/%{name}/*.so
%{_datadir}/locale/*/LC_MESSAGES/*

%files devel
%defattr(-,root,root)
%{_libdir}/*.*
#%{_libdir}/*.la
#%{_libdir}/*.so*
%{_includedir}/%{name}/*

%files testdata
%defattr(-,root,root)
%config /etc/oscada.xml
%{_bindir}/OScadaTest
%{_datadir}/%{name}/
/var/spool/%{name}/

%changelog
* Wed Nov 03 2004 Roman Savochenko <rom_as@fromru.com>
- move the message arhives data to /var/spool/%{name}/ARHIVE/MESS

* Tue Apr 06 2004 Roman Savochenko <rom_as@fromru.com>
- make 3 packages: OpenScada, OpenScada-devel, OpenScada-testdata
- add languages: ru, uk
- make packages from 'make -dist' package;

* Thu Oct 15 2003 Roman Savochenko <rom_as@fromru.com>
- Starting


