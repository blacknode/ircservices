# Makefile for NickServ modules.
#
# IRC Services is copyright (c) 1996-2009 Andrew Church.
#     E-mail: <achurch@achurch.org>
# Parts written by Andrew Kempe and others.
# This program is free but copyrighted software; see the file GPL.txt for
# details.

###########################################################################

include ../../Makefile.inc

MODULES = main.so access.so autojoin.so link.so mail-auth.so
OBJECTS-main.so = collide.o set.o util.o

INCLUDES = nickserv.h ns-local.h $(TOPDIR)/language.h $(TOPDIR)/encrypt.h \
	$(TOPDIR)/modules/memoserv/memoserv.h
INCLUDES-collide.o = $(TOPDIR)/timeout.h
INCLUDES-set.o = $(TOPDIR)/encrypt.h $(TOPDIR)/modules/operserv/operserv.h

INCLUDES-main.o = $(TOPDIR)/commands.h $(TOPDIR)/databases.h \
	$(TOPDIR)/encrypt.h $(TOPDIR)/modules/operserv/operserv.h
INCLUDES-access.o = $(TOPDIR)/commands.h $(TOPDIR)/databases.h \
	$(TOPDIR)/modules/operserv/operserv.h
INCLUDES-autojoin.o = $(TOPDIR)/commands.h $(TOPDIR)/databases.h \
	$(TOPDIR)/modules/operserv/operserv.h \
	$(TOPDIR)/modules/chanserv/chanserv.h
INCLUDES-link.o = $(TOPDIR)/commands.h \
	$(TOPDIR)/modules/operserv/operserv.h
INCLUDES-mail-auth.o = $(TOPDIR)/commands.h $(TOPDIR)/modules/mail/mail.h \
	$(TOPDIR)/modules/operserv/operserv.h

include ../Makerules

###########################################################################
