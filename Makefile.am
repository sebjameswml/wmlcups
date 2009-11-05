
AUTOMAKE_OPTIONS = check-news dist-bzip2

SUBDIRS = wmlcups doc

bin_PROGRAMS = test testenable testaccept teststatus testJobList testCupsdCtrl

test_SOURCES = test.cpp
test_LDADD = wmlcups/libwmlcups.la -lcups -lfutil -luuid

testenable_SOURCES = testenable.cpp
testenable_LDADD = wmlcups/libwmlcups.la -lcups -lfutil -luuid

testaccept_SOURCES = testaccept.cpp
testaccept_LDADD = wmlcups/libwmlcups.la -lcups -lfutil -luuid

teststatus_SOURCES = teststatus.cpp
teststatus_LDADD = wmlcups/libwmlcups.la -lcups -lfutil -luuid

testCupsdCtrl_SOURCES = testCupsdCtrl.cpp
testCupsdCtrl_LDADD = wmlcups/libwmlcups.la -lcups -lfutil -luuid

testJobList_SOURCES = testJobList.cpp
testJobList_LDADD = wmlcups/libwmlcups.la -lcups -lfutil -luuid

INCLUDES = -I./wmlcups

CLEANFILES = *~

MAINTAINERCLEANFILES = aclocal.m4 wmlcups-*.tar.gz wmlcups-*.tar.bz2 wmlcups-*.diff

