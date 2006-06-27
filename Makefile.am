SUBDIRS = src

EXTRA_DIST = openscada.spec README_ru doc/release_0.5.0_ru.html doc/release_0.5.0_ru_files/* \
	doc/AboutOpenSCADA_ru.html doc/AboutOpenSCADA_ru_files/* \
	doc/APIOpenSCADA_ru.html doc/APIOpenSCADA_ru_files/* doc/FAQ_ru.html doc/FAQ_ru_files/* \
	data/oscada.init data/oscada.xml data/icons/*.png \
	demo/openscada_demo demo/oscada_demo.xml demo/*.db demo/VISION/*
AUTOMAKE_OPTIONS = foreign 1.6

install-data-local:
	install -m 755 -d $(oscd_docdir)
	cp README README_ru COPYING INSTALL TODO ChangeLog $(oscd_docdir)
	cp -r doc/* $(oscd_docdir)