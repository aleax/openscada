SUBDIRS = src src_call

EXTRA_DIST = I18N.mk openscada.pc.in *.spec README* TODO* debian/*

pkgconfigdir = $(libdir)/pkgconfig
pkgconfig_DATA = openscada.pc

if ResourcesAllow
bin_SCRIPTS = data/openscada_start
sysconf_DATA = data/oscada.xml data/oscada_start.xml data/ModelsDB/AGLKS/oscada_AGLKS.xml data/ModelsDB/Boiler/oscada_Boiler.xml

sysicodir = $(datadir)/icons
sysico_DATA = data/openscada.png data/ModelsDB/AGLKS/openscada_AGLKS.png data/ModelsDB/Boiler/openscada_Boiler.png

appsdir = $(datadir)/applications
apps_DATA = data/openscada.desktop data/ModelsDB/AGLKS/openscada_AGLKS.desktop data/ModelsDB/Boiler/openscada_Boiler.desktop

icodir = $(oscd_datadir)/icons
ico_DATA = data/icons/* data/openscada.png data/ModelsDB/AGLKS/AGLKS.png data/ModelsDB/Boiler/Boiler.png

libdbdir = $(oscd_datadir)/LibsDB
libdb_DATA = data/LibsDB/*.db

AGLKSdbdir = $(oscd_datadir)/AGLKS
AGLKSdb_DATA = data/ModelsDB/AGLKS/*.db

Boilerdbdir = $(oscd_datadir)/Boiler
Boilerdb_DATA = data/ModelsDB/Boiler/*.db
endif

distSrc: distdir
	rm -rf $(distdir)/data $(distdir)/doc
	tar --exclude=".svn" -cvzf $(PACKAGE)-$(PACKAGE_VERSION).tar.gz $(distdir)
	$(am__remove_distdir)

distSrc-lzma: distdir
	rm -rf $(distdir)/data $(distdir)/doc
	tar --exclude=".svn" --lzma -cvf $(PACKAGE)-$(PACKAGE_VERSION).tar.lzma $(distdir)
	$(am__remove_distdir)

distRes:
	tar --exclude=".svn" -cvzf $(PACKAGE)-res-$(PACKAGE_VERSION).tar.gz doc data

distRes-lzma:
	tar --exclude=".svn" --lzma -cvf $(PACKAGE)-res-$(PACKAGE_VERSION).tar.lzma doc data
