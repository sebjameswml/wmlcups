
AUTOMAKE_OPTIONS = check-news dist-bzip2

SUBDIRS = wmlcups doc

bin_PROGRAMS = test testenable testaccept teststatus testJobList testCupsdCtrl testppd testjobcommand testmemberuris testPrint

test_SOURCES = test.cpp
test_LDADD = wmlcups/libwmlcups.la -lcups -lfutil -luuid -lmagic

testenable_SOURCES = testenable.cpp
testenable_LDADD = wmlcups/libwmlcups.la -lcups -lfutil -luuid -lmagic

testaccept_SOURCES = testaccept.cpp
testaccept_LDADD = wmlcups/libwmlcups.la -lcups -lfutil -luuid -lmagic

teststatus_SOURCES = teststatus.cpp
teststatus_LDADD = wmlcups/libwmlcups.la -lcups -lfutil -luuid -lmagic

testppd_SOURCES = testppd.cpp
testppd_LDADD = wmlcups/libwmlcups.la -lcups -lfutil -luuid -lmagic

testCupsdCtrl_SOURCES = testCupsdCtrl.cpp
testCupsdCtrl_LDADD = wmlcups/libwmlcups.la -lcups -lfutil -luuid -lmagic

testJobList_SOURCES = testJobList.cpp
testJobList_LDADD = wmlcups/libwmlcups.la -lcups -lfutil -luuid -lmagic

testjobcommand_SOURCES = testjobcommand.cpp
testjobcommand_LDADD = wmlcups/libwmlcups.la -lcups -lfutil -luuid -lmagic

testmemberuris_SOURCES = testmemberuris.cpp
testmemberuris_LDADD = wmlcups/libwmlcups.la -lcups -lfutil -luuid -lmagic

testPrint_SOURCES = testPrint.cpp
testPrint_LDADD = wmlcups/libwmlcups.la -lcups -lfutil -luuid -lmagic

INCLUDES = -I./wmlcups

CLEANFILES = *~

MAINTAINERCLEANFILES = aclocal.m4 wmlcups-*.tar.gz wmlcups-*.tar.bz2 wmlcups-*.diff

