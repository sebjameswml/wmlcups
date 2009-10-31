
AUTOMAKE_OPTIONS = check-news dist-bzip2

SUBDIRS = wmlcups doc

bin_PROGRAMS = test testenable testaccept

test_SOURCES = test.cpp
test_LDADD = wmlcups/libwmlcups.la -lcups

testenable_SOURCES = testenable.cpp
testenable_LDADD = wmlcups/libwmlcups.la -lcups

testaccept_SOURCES = testaccept.cpp
testaccept_LDADD = wmlcups/libwmlcups.la -lcups

INCLUDES = -I./wmlcups

CLEANFILES = *~

MAINTAINERCLEANFILES = aclocal.m4 wmlcups-*.tar.gz wmlcups-*.tar.bz2 wmlcups-*.diff

