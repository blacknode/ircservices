# Makefile for MemoServ modules.
#
# IRC Services is copyright (c) 1996-2009 Andrew Church.
#     E-mail: <achurch@achurch.org>
# Parts written by Andrew Kempe and others.
# This program is free but copyrighted software; see the file GPL.txt for
# details.

###########################################################################

include ../../Makefile.inc

MODULES = main.so forward.so ignore.so

INCLUDES = memoserv.h $(TOPDIR)/commands.h $(TOPDIR)/language.h
INCLUDES-forward.o = $(TOPDIR)/modules/mail/mail.h \
	$(TOPDIR)/modules/nickserv/nickserv.h
INCLUDES-ignore.o = $(TOPDIR)/databases.h \
	$(TOPDIR)/modules/operserv/operserv.h \
	$(TOPDIR)/modules/nickserv/nickserv.h
INCLUDES-main.o = $(TOPDIR)/databases.h \
	$(TOPDIR)/modules/operserv/operserv.h \
	$(TOPDIR)/modules/nickserv/nickserv.h \
	$(TOPDIR)/modules/chanserv/chanserv.h

include ../Makerules

###########################################################################

