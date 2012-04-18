SUBDIRS = src src_call

EXTRA_DIST = I18N.mk openscada.pc.in *.spec README* TODO* debian/*

pkgconfigdir = $(libdir)/pkgconfig
pkgconfig_DATA = openscada.pc

%.pc: %.pc.in
	@$(AM_V_GEN)sed -e 's,@prefix\@,$(prefix),g' \
	    -e 's,@libdir\@,$(libdir),g' \
	    -e 's,@HOWL_COMPAT_VERSION\@,$(HOWL_COMPAT_VERSION),g' \
	    -e 's,@PACKAGE_VERSION\@,$(PACKAGE_VERSION),g' $< > $@

CLEANFILES = openscada.pc

dist-res:
	tar --exclude=".svn" -cvzf $(PACKAGE)-res-$(PACKAGE_VERSION).tar.gz doc data

dist-reslzma:
	tar --exclude=".svn" --lzma -cvf $(PACKAGE)-res-$(PACKAGE_VERSION).tar.lzma doc data
