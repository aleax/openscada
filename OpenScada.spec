Summary: Open SCADA system project
Name: OpenScada
Version: 0.0.1
Release: 1
Source: %{name}-%{version}.tgz
License: GPL
Group: Applications/SCADA
Packager: Roman Savochenko <rom_as@fromru.com>
# URL: 
BuildRoot: %{_tmppath}/%{name}-%{version}-root

%description
SCADA system

%prep
rm -rf $RPM_BUILD_ROOT
%setup

%build
export CFLAGS=$RPM_OPT_FLAGS
export CXXFLAGS=$RPM_OPT_FLAGS
./configure --prefix=%{_prefix} --bindir=%{_bindir} --datadir=%{_datadir}
%{__make}

%install
rm -rf $RPM_BUILD_ROOT
%makeinstall 

%clean
rm -rf $RPM_BUILD_ROOT $RPM_BUILD_DIR/%{name}-%{version}

%files
%defattr(-,root,root)
%doc README COPYING INSTALL TODO ChangeLog doc/OpenScadaUMLdescr.sxw
%{_bindir}/*
%{_libdir}/*

%changelog
* Thu Oct 15 2003 Roman Savochenko <rom_as@fromru.com>
- 0.0.1
- Starting


