# Master makefile for modules.
#
# IRC Services is copyright (c) 1996-2009 Andrew Church.
#     E-mail: <achurch@achurch.org>
# Parts written by Andrew Kempe and others.
# This program is free but copyrighted software; see the file GPL.txt for
# details.

TOPDIR=../
include $(TOPDIR)/Makefile.inc

###########################################################################

SUBDIRS = $(shell for i in *; do test -f "$$i/Makefile" && echo "$$i"; done)
.PHONY: all-dynamic all-static install clean spotless

###########################################################################

all-dynamic:
	@set -e ; for i in $(SUBDIRS) ; do \
		$(MAKE) -C $$i $@ DIRNAME="$$i" CFLAGS="$(CFLAGS)" ; \
		if $(TEST_NT) ! -f .stamp -o "$$i/.stamp" -nt .stamp ; then \
			echo "touch .stamp" ; \
			touch .stamp ; \
		fi ; \
	done

all-static:
	@rm -f modext.h modsyms.c modlist.c
	@echo '#include "../config.h"' >>modlist.c
	@echo '#include "../defs.h"' >>modlist.c
	@echo '#include "modext.h"' >>modlist.c
	@echo '#include "modsyms.c"' >>modlist.c
	@echo 'struct {const char *name; void *symlist;} modlist[] = {' >>modlist.c
	@set -e ; for i in $(SUBDIRS) ; do \
		$(MAKE) -C $$i $@ DIRNAME="$$i" CFLAGS="$(CFLAGS)" ; \
		cat $$i/.modext-*.h >>modext.h ; \
		cat $$i/.modsyms-*.c >>modsyms.c ; \
		cat $$i/.modlist-*.c >>modlist.c ; \
		if $(TEST_NT) "$$i/.stamp" -nt .stamp ; then \
			echo "touch .stamp" ; \
			touch .stamp ; \
		fi ; \
	done
	@echo '{0}};' >>modlist.c
	$(CC) -c modlist.c -o modlist.o
	ar -r modules.a modlist.o
	$(RANLIB) modules.a

install:
	@set -e ; for i in $(SUBDIRS) ; do \
		$(MAKE) -C $$i $@ DIRNAME="$$i" ; \
	done

clean spotless:
	@set -e ; for i in $(SUBDIRS) ; do \
		$(MAKE) -C $$i $@ DIRNAME="$$i" ; \
	done
	rm -f modext.h modsyms.c modlist.c *.[oa] .stamp */.stamp

# Remove modules.a (for recompilation) if the top-level Makefile changed
modules.a: ../Makefile ../Makefile.inc
	rm -f $@

$(DATDEST)/modules:
	$(MKDIR) $@
