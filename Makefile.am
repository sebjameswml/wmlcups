
AUTOMAKE_OPTIONS = check-news dist-bzip2

SUBDIRS = wmlcups doc

bin_PROGRAMS = test teststatus testJobList testCupsdCtrl testppd testjobcommand testmemberuris rp lpd lpdstat qlpd wcupsprint wcupsaccept wcupsenable

test_SOURCES = test.cpp
test_LDADD = wmlcups/libwmlcups.la -lcups -lfutil -luuid -lmagic $(GLIBMM_LIBS)

wcupsenable_SOURCES = wcupsenable.cpp
wcupsenable_LDADD = wmlcups/libwmlcups.la -lcups -lfutil -luuid -lmagic $(GLIBMM_LIBS)

wcupsaccept_SOURCES = wcupsaccept.cpp
wcupsaccept_LDADD = wmlcups/libwmlcups.la -lcups -lfutil -luuid -lmagic $(GLIBMM_LIBS)

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

wcupsprint_SOURCES = wcupsprint.cpp
wcupsprint_LDADD = wmlcups/libwmlcups.la -lcups -lfutil -luuid -lmagic $(GLIBMM_LIBS)

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

