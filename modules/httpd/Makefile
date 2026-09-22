# Makefile for httpd modules.
#
# IRC Services is copyright (c) 1996-2009 Andrew Church.
#     E-mail: <achurch@achurch.org>
# Parts written by Andrew Kempe and others.
# This program is free but copyrighted software; see the file GPL.txt for
# details.

###########################################################################

include ../../Makefile.inc

MODULES = main.so auth-ip.so auth-password.so dbaccess.so debug.so \
	  redirect.so top-page.so
OBJECTS-main.so = util.o

INCLUDES = http.h
INCLUDES-dbaccess.o = $(TOPDIR)/modules/operserv/operserv.h \
	$(TOPDIR)/modules/operserv/maskdata.h \
	$(TOPDIR)/modules/operserv/akill.h \
	$(TOPDIR)/modules/operserv/news.h \
	$(TOPDIR)/modules/operserv/sline.h \
	$(TOPDIR)/modules/nickserv/nickserv.h \
	$(TOPDIR)/modules/chanserv/chanserv.h \
	$(TOPDIR)/modules/chanserv/access.h \
	$(TOPDIR)/modules/statserv/statserv.h \
	$(TOPDIR)/modules/misc/xml.h
INCLUDES-main.o = $(TOPDIR)/timeout.h
INCLUDES-redirect.o = $(TOPDIR)/modules/nickserv/nickserv.h \
	$(TOPDIR)/modules/chanserv/chanserv.h

include ../Makerules

###########################################################################

# Note that while http.h technically depends on $(TOPDIR)/timeout.h, the
# only thing it uses from that file is "Timeout *", which is unlikely to
# change, so we don't bother depending on it here.  Source files which call
# timeout routines should #include "timeout.h" (and include it in their
# dependency lists) directly.
