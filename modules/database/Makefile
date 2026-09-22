# Makefile for database modules.
#
# IRC Services is copyright (c) 1996-2009 Andrew Church.
#     E-mail: <achurch@achurch.org>
# Parts written by Andrew Kempe and others.
# This program is free but copyrighted software; see the file GPL.txt for
# details.

###########################################################################

include ../../Makefile.inc

MODULES = standard.so version4.so
OBJECTS-standard.so = fileutil.o
OBJECTS-version4.so = extsyms.o  # fileutil.c is #included

SERVDEPS = $(TOPDIR)/modules/nickserv/nickserv.h \
	$(TOPDIR)/modules/chanserv/chanserv.h \
	$(TOPDIR)/modules/memoserv/memoserv.h \
	$(TOPDIR)/modules/operserv/operserv.h \
	$(TOPDIR)/modules/operserv/akill.h \
	$(TOPDIR)/modules/operserv/maskdata.h \
	$(TOPDIR)/modules/operserv/news.h \
	$(TOPDIR)/modules/operserv/sline.h \
	$(TOPDIR)/modules/statserv/statserv.h

INCLUDES-extsyms.o = extsyms.h
INCLUDES-fileutil.o = fileutil.h
INCLUDES-standard.o = fileutil.h $(TOPDIR)/databases.h $(TOPDIR)/encrypt.h
# NOTE: fileutil.c is *not* a typo here!
INCLUDES-version4.o = extsyms.h fileutil.c fileutil.h $(TOPDIR)/databases.h \
	$(TOPDIR)/language.h $(TOPDIR)/encrypt.h $(SERVDEPS) \
	$(TOPDIR)/modules/nickserv/util.c \
	$(TOPDIR)/modules/chanserv/util.c

include ../Makerules

###########################################################################

extsyms.h: $(TOPDIR)/modules/nickserv/nickserv.h \
	   $(TOPDIR)/modules/chanserv/chanserv.h \
	   $(TOPDIR)/modules/memoserv/memoserv.h \
	   $(TOPDIR)/modules/statserv/statserv.h
	@touch $@
