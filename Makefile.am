SUBDIRS = src

EXTRA_DIST = openscada.spec README_ru doc/*.pdf \
	data/oscada.init data/oscada.xml data/icons/*.png \
	demo/openscada_demo demo/oscada_demo.xml demo/*.db demo/VISION/*
AUTOMAKE_OPTIONS = foreign 1.6

install-data-local:
	install -m 755 -d $(oscd_docdir)
	install -m 644 README README_ru COPYING INSTALL TODO ChangeLog $(oscd_docdir)
	install -m 644 doc/*.pdf $(oscd_docdir)