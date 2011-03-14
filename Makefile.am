SUBDIRS = src src_call

EXTRA_DIST = *.spec README* TODO* debian/*
AUTOMAKE_OPTIONS = foreign 1.6

dist-res:
	tar --exclude=".svn" -cvzf $(PACKAGE)-res-$(PACKAGE_VERSION).tar.gz doc data

dist-reslzma:
	tar --exclude=".svn" --lzma -cvf $(PACKAGE)-res-$(PACKAGE_VERSION).tar.lzma doc data
