# Makefile for Services tools directory.
#
# IRC Services is copyright (c) 1996-2009 Andrew Church.
#     E-mail: <achurch@achurch.org>
# Parts written by Andrew Kempe and others.
# This program is free but copyrighted software; see the file GPL.txt for
# details.

include ../Makefile.inc
TOPDIR = ..
CFLAGS_CONVERT_DB = $(CFLAGS) -I$(TOPDIR) -DCONVERT_DB

###########################################################################

ifneq ($(VSNPRINTF_O),)
CONVERT_DB_VSNPRINTF_O = $(TOPDIR)/vsnprintf.o
endif

# These aren't "modules" in the real sense; this is just a convenient way
# to list the object files that handle each database format.
CONVERT_DB_MODULES = \
	convert-cygnus.o \
	convert-epona.o \
	convert-hybserv.o \
	convert-magick.o \
	convert-ptlink.o \
	convert-sirv.o \
	convert-trircd.o \
	convert-ver8.o

CONVERT_DB_OBJS = convert-db.o $(CONVERT_DB_MODULES) \
	fileutil-x.o misc-x.o xml-export-x.o $(TOPDIR)/compat.o \
	$(CONVERT_DB_VSNPRINTF_O)

.c.o:
	$(CC) $(CFLAGS_CONVERT_DB) -c $< -o $@

###########################################################################

.PHONY: all install clean spotless

all: convert-db$(EXE_SUFFIX) $(PROGRAM)-chk

install:
	$(INSTALL_EXE) convert-db$(EXE_SUFFIX) "$(INSTALL_PREFIX)$(DATDEST)/convert-db$(EXE_SUFFIX)"
	$(INSTALL_EXE) $(PROGRAM)-chk "$(INSTALL_PREFIX)$(BINDEST)/$(PROGRAM)-chk"

clean:
	rm -f *.o convert-db$(EXE_SUFFIX) $(PROGRAM)-chk

spotless: clean

###########################################################################

convert-db$(EXE_SUFFIX): $(CONVERT_DB_OBJS)
	$(CC) $(LFLAGS) $(CONVERT_DB_OBJS) $(LIBS) -o $@

$(PROGRAM)-chk: ircservices-chk.in Makefile $(TOPDIR)/Makefile.inc
	PROGRAM=`echo "$(PROGRAM)" | sed 's,/,\\\\/,g'` ; \
	BINDEST=`echo "$(BINDEST)" | sed 's,/,\\\\/,g'` ; \
	DATDEST=`echo "$(DATDEST)" | sed 's,/,\\\\/,g'` ; \
	sed -e "s/@BINDEST@/$$BINDEST/g" -e "s/@DATDEST@/$$DATDEST/g" -e "s/@PROGRAM@/$$PROGRAM/g" <ircservices-chk.in >$(PROGRAM)-chk
	chmod a+x $(PROGRAM)-chk


$(CONVERT_DB_OBJS): Makefile $(TOPDIR)/Makefile.inc convert-db.h \
                    $(TOPDIR)/services.h $(TOPDIR)/encrypt.h \
                    $(TOPDIR)/modules/database/fileutil.h \
                    $(TOPDIR)/modules/nickserv/nickserv.h \
                    $(TOPDIR)/modules/chanserv/chanserv.h \
                    $(TOPDIR)/modules/memoserv/memoserv.h \
                    $(TOPDIR)/modules/operserv/operserv.h \
                    $(TOPDIR)/modules/operserv/maskdata.h \
                    $(TOPDIR)/modules/operserv/news.h \
                    $(TOPDIR)/modules/statserv/statserv.h

convert-db.o: convert-db.c $(TOPDIR)/language.h $(TOPDIR)/modules/misc/xml.h \
	$(TOPDIR)/modules/nickserv/util.c $(TOPDIR)/modules/chanserv/util.c

convert-cygnus.o:   convert-cygnus.c
convert-epona.o:    convert-epona.c
convert-magick.o:   convert-magick.c
convert-ptlink.o:   convert-ptlink.c $(TOPDIR)/language.h
convert-sirv.o:     convert-sirv.c
convert-trircd.o:   convert-trircd.c
convert-ver8.o:     convert-ver8.c

fileutil-x.o: $(TOPDIR)/modules/database/fileutil.c $(TOPDIR)/services.h \
              $(TOPDIR)/modules/database/fileutil.h
	$(CC) $(CFLAGS_CONVERT_DB) -c $< -o $@
misc-x.o: $(TOPDIR)/misc.c $(TOPDIR)/services.h
	$(CC) $(CFLAGS_CONVERT_DB) -c $< -o $@
xml-export-x.o: $(TOPDIR)/modules/misc/xml-export.c $(TOPDIR)/services.h \
                $(TOPDIR)/language.h \
                $(TOPDIR)/modules/nickserv/nickserv.h \
                $(TOPDIR)/modules/chanserv/chanserv.h \
                $(TOPDIR)/modules/memoserv/memoserv.h \
                $(TOPDIR)/modules/operserv/operserv.h \
                $(TOPDIR)/modules/operserv/maskdata.h \
                $(TOPDIR)/modules/operserv/news.h \
                $(TOPDIR)/modules/statserv/statserv.h
	$(CC) $(CFLAGS_CONVERT_DB) -c $< -o $@

$(TOPDIR)/compat.o $(TOPDIR)/misc.o $(TOPDIR)/vsnprintf.o:
	$(MAKE) -C $(TOPDIR) $(notdir $@)

###########################################################################
