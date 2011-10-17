CLEANFILES = *~

INCLUDES = -I$(top_srcdir) -I$(top_builddir) $(GLIBMM_CFLAGS)

lib_LTLIBRARIES = libwmlcups.la

libwmlcups_la_SOURCES = CupsCtrl.cpp IppAttr.cpp PpdOption.cpp CupsJob.cpp CupsdCtrl.cpp CupsCtrlClasses.cpp CupsCtrlJobs.cpp CupsCtrlError.cpp CupsCtrlPpd.cpp CupsCtrlQueue.cpp CupsCtrlPrinter.cpp Ppd.cpp

libwmlcups_la_LDFLAGS = -version-info 2:0:0

pkginclude_HEADERS = CupsCtrl.h IppAttr.h PpdOption.h CupsJob.h QueueCupsStatus.h CupsdCtrl.h Ppd.h


