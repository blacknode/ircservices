# Makefile for protocol modules.
#
# IRC Services is copyright (c) 1996-2009 Andrew Church.
#     E-mail: <achurch@achurch.org>
# Parts written by Andrew Kempe and others.
# This program is free but copyrighted software; see the file GPL.txt for
# details.

###########################################################################

include ../../Makefile.inc

MODULES = bahamut.so dalnet.so dreamforge.so hybrid.so inspircd.so \
          monkey.so ptlink.so ratbox.so rfc1459.so solidircd.so trircd.so \
          ts8.so undernet-p9.so unreal.so

INCLUDES-bahamut.o = $(BANEXCEPT_C) $(INVITEMASK_C) $(SJOIN_C) $(SVSNICK_C) \
	$(TOPDIR)/messages.h $(TOPDIR)/language.h \
	$(TOPDIR)/modules/operserv/operserv.h \
	$(TOPDIR)/modules/operserv/maskdata.h \
	$(TOPDIR)/modules/nickserv/nickserv.h \
	$(TOPDIR)/modules/chanserv/chanserv.h
INCLUDES-dalnet.o = $(TOPDIR)/messages.h $(TOPDIR)/language.h
INCLUDES-dreamforge.o = $(SVSNICK_C) \
	$(TOPDIR)/messages.h $(TOPDIR)/language.h \
	$(TOPDIR)/modules/operserv/operserv.h \
	$(TOPDIR)/modules/nickserv/nickserv.h
INCLUDES-hybrid.o = $(BANEXCEPT_C) $(INVITEMASK_C) $(SJOIN_C) $(SVSNICK_C) \
	$(TOPDIR)/messages.h
INCLUDES-inspircd.o = $(BANEXCEPT_C) $(CHANPROT_C) $(HALFOP_C) \
	$(INVITEMASK_C) $(SVSNICK_C) \
	$(TOPDIR)/messages.h $(TOPDIR)/language.h $(TOPDIR)/timeout.h \
        $(TOPDIR)/version.h \
	$(TOPDIR)/modules/operserv/operserv.h \
	$(TOPDIR)/modules/operserv/maskdata.h \
	$(TOPDIR)/modules/nickserv/nickserv.h \
	$(TOPDIR)/modules/chanserv/chanserv.h
INCLUDES-monkey.o = $(HALFOP_C) $(SJOIN_C) \
	$(TOPDIR)/messages.h $(TOPDIR)/language.h \
	$(TOPDIR)/modules/operserv/operserv.h \
	$(TOPDIR)/modules/nickserv/nickserv.h
INCLUDES-ptlink.o = $(BANEXCEPT_C) $(SJOIN_C) $(SVSNICK_C) \
	$(TOPDIR)/messages.h $(TOPDIR)/language.h $(TOPDIR)/version.h \
	$(TOPDIR)/modules/operserv/operserv.h \
	$(TOPDIR)/modules/operserv/maskdata.h \
	$(TOPDIR)/modules/nickserv/nickserv.h \
	$(TOPDIR)/modules/chanserv/chanserv.h
INCLUDES-ratbox.o = $(BANEXCEPT_C) $(INVITEMASK_C) $(SJOIN_C) \
	$(TOPDIR)/messages.h
INCLUDES-solidircd.o = $(BANEXCEPT_C) $(HALFOP_C) $(INVITEMASK_C) \
	$(SJOIN_C) $(SVSNICK_C) $(TOPDIR)/messages.h $(TOPDIR)/language.h \
	$(TOPDIR)/modules/operserv/operserv.h \
	$(TOPDIR)/modules/operserv/maskdata.h \
	$(TOPDIR)/modules/nickserv/nickserv.h \
	$(TOPDIR)/modules/chanserv/chanserv.h
INCLUDES-trircd.o = $(BANEXCEPT_C) $(CHANPROT_C) $(HALFOP_C) \
	$(INVITEMASK_C) $(SJOIN_C) $(SVSNICK_C) $(TOKEN_C) \
	$(TOPDIR)/messages.h $(TOPDIR)/language.h \
	$(TOPDIR)/modules/operserv/operserv.h \
	$(TOPDIR)/modules/nickserv/nickserv.h
INCLUDES-undernet-p9.o = $(TOPDIR)/messages.h $(TOPDIR)/language.h
INCLUDES-unreal.o = $(BANEXCEPT_C) $(CHANPROT_C) $(HALFOP_C) \
	$(INVITEMASK_C) $(SJOIN_C) $(SVSNICK_C) $(TOKEN_C) \
	$(TOPDIR)/messages.h $(TOPDIR)/language.h $(TOPDIR)/timeout.h \
	$(TOPDIR)/modules/operserv/operserv.h \
	$(TOPDIR)/modules/operserv/maskdata.h \
	$(TOPDIR)/modules/nickserv/nickserv.h \
	$(TOPDIR)/modules/chanserv/chanserv.h

BANEXCEPT_C = banexcept.c banexcept.h $(TOPDIR)/language.h \
	$(TOPDIR)/modules/chanserv/chanserv.h
CHANPROT_C = chanprot.c chanprot.h $(TOPDIR)/language.h
HALFOP_C = halfop.c halfop.h $(TOPDIR)/language.h \
	$(TOPDIR)/modules/chanserv/chanserv.h
INVITEMASK_C = invitemask.c invitemask.h $(TOPDIR)/language.h \
	$(TOPDIR)/modules/chanserv/chanserv.h
SJOIN_C = sjoin.c sjoin.h $(TOPDIR)/modules/chanserv/chanserv.h
SVSNICK_C = svsnick.c svsnick.h
TOKEN_C = token.c token.h $(TOPDIR)/messages.h

include ../Makerules

###########################################################################
