# Makefile for data directory.
#
# IRC Services is copyright (c) 1996-2009 Andrew Church.
#     E-mail: <achurch@achurch.org>
# Parts written by Andrew Kempe and others.
# This program is free but copyrighted software; see the file GPL.txt for
# details.

include ../Makefile.inc
TOPDIR=..

all: example-ircservices.conf example-modules.conf

%.conf: %.conf.in Makefile ../Makefile.inc
	sed 's/@PROGRAM@/$(PROGRAM)/g' <$< >$@

install: all $(DATDEST)/helpfiles
	$(INSTALL_DAT) example-ircservices.conf "$(INSTALL_PREFIX)$(DATDEST)/example-$(PROGRAM).conf"
	$(INSTALL_DAT) example-modules.conf "$(INSTALL_PREFIX)$(DATDEST)/example-modules.conf"

$(DATDEST)/helpfiles:
	$(CP_ALL) helpfiles "$(DATDEST)"
