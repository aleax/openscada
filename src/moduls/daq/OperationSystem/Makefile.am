EXTRA_DIST = *.h po/* doc/about_ru.html doc/about_ru_files/*

AM_CXXFLAGS= -I$(top_srcdir)/src -fPIC -DPIC
AM_LDFLAGS= -shared -module

oscd_modul_PROGRAMS = daq_OperationSystem.so
daq_OperationSystem_so_SOURCES = os_contr.cpp da_cpu.cpp da_mem.cpp da_uptime.cpp da_hddtemp.cpp da_sensors.cpp da_smart.cpp da_hddstat.cpp da_netstat.cpp


#PO files process and doc install
fm_name = Mod_DAQ_OperationSystem
m_name = $(oscd_modulpref)OperationSystem

messages:
	@catalogs=`find ./po -name \*.po` ; \
	name=po/$(m_name).pot ; \
	xgettext -o $$name -C -a *.cpp *.h &> /dev/null ; \
	for cat in $$catalogs; do \
	msgmerge -U $$cat $$name ; \
	done

install-data-local:
	@catalogs=`find ./po -name \*.po` ; \
	name=$(m_name).mo ; \
	for cat in $$catalogs; do \
	mkdir -p $(oscd_localedir)/`basename $$cat .po`/LC_MESSAGES ; \
	msgfmt -vo $$name $$cat ; \
	cp $$name $(oscd_localedir)/`basename $$cat .po`/LC_MESSAGES/$$name ; \
	rm $$name ; \
	done
	install -m 755 -d $(oscd_docdir)/$(fm_name)
	cp -r doc/* $(oscd_docdir)/$(fm_name)

uninstall-local:
	@catalogs=`find ./po -name \*.po` ; \
	name=$(m_name).mo ; \
	for cat in $$catalogs; do \
	rm -f $$name $(oscd_localedir)/`basename $$cat .po`/LC_MESSAGES/$$name ; \
	done

clean-local:
	rm -f po/$(m_name).pot ; \
	rm -f po/*~ ;