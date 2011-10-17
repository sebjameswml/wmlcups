
AUTOMAKE_OPTIONS = check-news dist-bzip2

SUBDIRS = wmlcups doc

bin_PROGRAMS = test testenable testaccept teststatus testJobList testCupsdCtrl testppd testjobcommand testmemberuris testPrint rp lpd lpdstat qlpd

test_SOURCES = test.cpp
test_LDADD = wmlcups/libwmlcups.la -lcups -lfutil -luuid -lmagic $(GLIBMM_LIBS)

testenable_SOURCES = testenable.cpp
testenable_LDADD = wmlcups/libwmlcups.la -lcups -lfutil -luuid -lmagic $(GLIBMM_LIBS)

testaccept_SOURCES = testaccept.cpp
testaccept_LDADD = wmlcups/libwmlcups.la -lcups -lfutil -luuid -lmagic $(GLIBMM_LIBS)

teststatus_SOURCES = teststatus.cpp
teststatus_LDADD = wmlcups/libwmlcups.la -lcups -lfutil -luuid -lmagic $(GLIBMM_LIBS)

testppd_SOURCES = testppd.cpp
testppd_LDADD = wmlcups/libwmlcups.la -lcups -lfutil -luuid -lmagic $(GLIBMM_LIBS)

testCupsdCtrl_SOURCES = testCupsdCtrl.cpp
testCupsdCtrl_LDADD = wmlcups/libwmlcups.la -lcups -lfutil -luuid -lmagic $(GLIBMM_LIBS)

testJobList_SOURCES = testJobList.cpp
testJobList_LDADD = wmlcups/libwmlcups.la -lcups -lfutil -luuid -lmagic $(GLIBMM_LIBS)

testjobcommand_SOURCES = testjobcommand.cpp
testjobcommand_LDADD = wmlcups/libwmlcups.la -lcups -lfutil -luuid -lmagic $(GLIBMM_LIBS)

testmemberuris_SOURCES = testmemberuris.cpp
testmemberuris_LDADD = wmlcups/libwmlcups.la -lcups -lfutil -luuid -lmagic $(GLIBMM_LIBS)

testPrint_SOURCES = testPrint.cpp
testPrint_LDADD = wmlcups/libwmlcups.la -lcups -lfutil -luuid -lmagic $(GLIBMM_LIBS)

qlpd_SOURCES = qlpd.cpp
qlpd_LDADD = -lfutil -luuid -lmagic $(GLIBMM_LIBS)

lpd_SOURCES = lpd.c
lpd_LDADD = -lcups -lnsl -lcrypt

lpdstat_SOURCES = lpdstat.c
lpdstat_LDADD = -lcups -lnsl -lcrypt

rp_SOURCES = rp.c

INCLUDES = -I./wmlcups $(GLIBMM_CFLAGS)

CLEANFILES = *~

MAINTAINERCLEANFILES = aclocal.m4 wmlcups-*.tar.gz wmlcups-*.tar.bz2 wmlcups-*.diff

