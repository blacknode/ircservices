# Makefile for OperServ modules.
#
# IRC Services is copyright (c) 1996-2009 Andrew Church.
#     E-mail: <achurch@achurch.org>
# Parts written by Andrew Kempe and others.
# This program is free but copyrighted software; see the file GPL.txt for
# details.

###########################################################################

include ../../Makefile.inc

MODULES = main.so akill.so news.so sessions.so sline.so

OBJECTS-main.so = maskdata.o

INCLUDES = operserv.h $(TOPDIR)/commands.h $(TOPDIR)/databases.h \
	$(TOPDIR)/language.h
INCLUDES-main.o = maskdata.h akill.h news.h \
	$(TOPDIR)/timeout.h $(TOPDIR)/encrypt.h \
	$(TOPDIR)/modules/nickserv/nickserv.h \
	$(TOPDIR)/modules/chanserv/chanserv.h
INCLUDES-maskdata.o = maskdata.h
INCLUDES-akill.o = maskdata.h akill.h
INCLUDES-news.o = news.h
INCLUDES-sessions.o = maskdata.h akill.h
INCLUDES-sline.o = maskdata.h sline.h

include ../Makerules

###########################################################################
