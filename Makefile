# Makefile for Services.
#
# IRC Services is copyright (c) 1996-2009 Andrew Church.
#     E-mail: <achurch@achurch.org>
# Parts written by Andrew Kempe and others.
# This program is free but copyrighted software; see the file GPL.txt for
# details.


include Makefile.inc
TOPDIR=.

default: all

###########################################################################
########################## Configuration section ##########################

# NOTE: Compilation options (CLEAN_COMPILE, etc.) are now all controlled by
#       the configure script.

# Add any extra flags you want here.  The default line below enables
# warnings and debugging symbols on GCC.  If you have a non-GCC compiler,
# you may want to comment it out or change it.  ("icc" below is the Intel
# C compiler for Linux, which doesn't understand GCC's -W/-f options.)

ifeq ($(CC),icc)
MORE_CFLAGS = -g
else
MORE_CFLAGS = -g -Wall -Wmissing-prototypes
endif

######################## End configuration section ########################
###########################################################################


ifneq ($(STATIC_MODULES),)
CDEFS += -DSTATIC_MODULES
MODLIB = modules/modules.a
endif

CFLAGS = $(CDEFS) $(BASE_CFLAGS) $(MORE_CFLAGS)


OBJS =	actions.o channels.o commands.o compat.o conffile.o databases.o \
	encrypt.o ignore.o init.o language.o log.o main.o memory.o \
	messages.o misc.o modes.o modules.o process.o send.o servers.o \
	signals.o sockets.o timeout.o users.o $(VSNPRINTF_O)

.c.o:
	$(CC) $(CFLAGS) -c $< -o $@

###########################################################################

.PHONY: default config-check all myall myclean clean spotless distclean \
	install instbin instmod modules languages tools

###########################################################################

config-check:
	@if ./configure -check ; then : ; else \
		echo 'Either you have not yet run the "configure" script, or the script has been' ; \
		echo 'updated since you last ran it.  Please run "./configure" before running' ; \
		echo '"$(MAKE)".' ; \
		exit 1 ; \
	fi

all: config-check myall
	@echo Now run \"$(MAKE) install\" to install Services.

myall: $(PROGRAM) languages tools

myclean:
	rm -f *.o $(PROGRAM) version.c.old

clean: myclean
	$(MAKE) -C lang clean
	$(MAKE) -C modules clean
	$(MAKE) -C tools clean

spotless distclean: myclean
	$(MAKE) -C lang spotless
	$(MAKE) -C modules spotless
	$(MAKE) -C tools spotless
	rm -rf config.cache config.h* configure.log conf-tmp Makefile.inc* \
		langstrs.h version.c

install: config-check myall myinstall
	@echo ""
	@echo "Don't forget to create/update your ircservices.conf and modules.conf files!"
	@echo "See the README for details."
	@echo ""

myinstall: instdirs instbin instmod
	$(MAKE) -C lang install
	$(MAKE) -C tools install
	$(MAKE) -C data install

instdirs:
	mkdir -p "$(INSTALL_PREFIX)$(BINDEST)"
	mkdir -p "$(INSTALL_PREFIX)$(DATDEST)"

instbin:
	$(INSTALL_EXE) $(PROGRAM)$(EXE_SUFFIX) "$(INSTALL_PREFIX)$(BINDEST)/$(PROGRAM)$(EXE_SUFFIX)"

ifneq ($(STATIC_MODULES),)
instmod:
else
instmod:
	$(MAKE) -C modules install
endif

###########################################################################

$(PROGRAM): $(OBJS) modules version.o
	$(CC) $(LFLAGS) $(OBJS) version.o $(MODLIB) $(LIBS) -o $@

ifneq ($(STATIC_MODULES),)
modules:
	@$(MAKE) -C modules all-static CFLAGS="$(CFLAGS)"
else
modules:
	@$(MAKE) -C modules all-dynamic CFLAGS="$(CFLAGS)"
endif

languages:
	@$(MAKE) -C lang CFLAGS="$(CFLAGS)"

tools: services.h
	@$(MAKE) -C tools CFLAGS="$(CFLAGS)"


# Catch any changes in compilation options at the top of this file or the
# configure output
$(OBJS): Makefile Makefile.inc

$(OBJS): services.h

actions.o:	actions.c	language.h modules.h timeout.h
channels.o:	channels.c	modules.h hash.h
commands.o:	commands.c	modules.h commands.h language.h
compat.o:	compat.c
conffile.o:	conffile.c	conffile.h
databases.o:	databases.c	databases.h encrypt.h modules.h
encrypt.o:	encrypt.c	encrypt.h
ignore.o:	ignore.c
init.o:		init.c		conffile.h databases.h messages.h modules.h \
				language.h version.h
language.o:	language.c	language.h modules/nickserv/nickserv.h
log.o:		log.c
main.o:		main.c		databases.h modules.h timeout.h
memory.o:	memory.c
messages.o:	messages.c	messages.h language.h modules.h version.h \
				modules/operserv/operserv.h
misc.o:		misc.c
modes.o:	modes.c
modules.o:	modules.c	modules.h conffile.h
process.o:	process.c	modules.h messages.h
send.o:		send.c		modules.h language.h
servers.o:	servers.c	modules.h hash.h
signals.o:	signals.c
sockets.o:	sockets.c
timeout.o:	timeout.c	timeout.h
users.o:	users.c		modules.h hash.h
vsnprintf.o:	vsnprintf.c


services.h: config.h defs.h memory.h list-array.h log.h sockets.h \
            send.h modes.h users.h channels.h servers.h extern.h
	-touch $@

version.c: $(OBJS) modules/.stamp
	sh version.sh
modules/.stamp: modules

databases.h: modules.h
	-touch $@

language.h: langstrs.h
	-touch $@

langstrs.h: lang/langstrs.h
	cp -p lang/langstrs.h .

.PHONY: lang/langstrs.h
lang/langstrs.h:
	$(MAKE) -C lang langstrs.h

###########################################################################
