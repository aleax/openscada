EXTRA_DIST = *.h test_Makefile test.cpp *.pem

noinst_LTLIBRARIES = libOPC_UA.la
libOPC_UA_la_SOURCES = libOPC_UA.cpp
libOPC_UA_la_CXXFLAGS = -fpic
libOPC_UA_la_LIBADD  = $(LIB_OpenSSL)

# noinst_PROGRAMS = testOPC_UA
# testOPC_UA_SOURCES = test.cpp
# testOPC_UA_LDADD = -lOPC_UA
