# Makefile for ChanServ modules.
#
# IRC Services is copyright (c) 1996-2009 Andrew Church.
#     E-mail: <achurch@achurch.org>
# Parts written by Andrew Kempe and others.
# This program is free but copyrighted software; see the file GPL.txt for
# details.

###########################################################################

include ../../Makefile.inc

MODULES = main.so access-levels.so access-xop.so
OBJECTS-main.so = access.o check.o set.o util.o

INCLUDES = chanserv.h cs-local.h $(TOPDIR)/language.h $(TOPDIR)/encrypt.h \
	   $(TOPDIR)/modules/nickserv/nickserv.h \
	   $(TOPDIR)/modules/operserv/operserv.h

INCLUDES-access.o = access.h
INCLUDES-check.o = $(TOPDIR)/timeout.h
INCLUDES-main.o = $(TOPDIR)/commands.h $(TOPDIR)/databases.h \
	$(TOPDIR)/encrypt.h
INCLUDES-set.o = $(TOPDIR)/encrypt.h

INCLUDES-access-levels.o = $(TOPDIR)/commands.h access.h
INCLUDES-access-xop.o = $(TOPDIR)/commands.h

include ../Makerules

###########################################################################
