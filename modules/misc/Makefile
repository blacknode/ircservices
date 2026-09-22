# Makefile for HelpServ modules.
#
# IRC Services is copyright (c) 1996-2009 Andrew Church.
#     E-mail: <achurch@achurch.org>
# Parts written by Andrew Kempe and others.
# This program is free but copyrighted software; see the file GPL.txt for
# details.

###########################################################################

include ../../Makefile.inc

MODULES = devnull.so helpserv.so xml-export.so xml-import.so

INCLUDES-helpserv.o = $(TOPDIR)/language.h
INCLUDES-xml-export.o = $(XML_INCLUDES)
INCLUDES-xml-import.o = $(XML_INCLUDES) \
	$(TOPDIR)/modules/nickserv/util.c \
	$(TOPDIR)/modules/chanserv/util.c

include ../Makerules

###########################################################################

XML_INCLUDES = xml.h $(TOPDIR)/language.h \
	$(TOPDIR)/modules/nickserv/nickserv.h \
	$(TOPDIR)/modules/chanserv/chanserv.h \
	$(TOPDIR)/modules/memoserv/memoserv.h \
	$(TOPDIR)/modules/operserv/operserv.h \
	$(TOPDIR)/modules/operserv/maskdata.h \
	$(TOPDIR)/modules/operserv/news.h \
	$(TOPDIR)/modules/statserv/statserv.h
