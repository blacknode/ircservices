# Makefile for language module

include ../Makefile.inc


LANGOBJS = de en_us es fr hu ja_euc ja_sjis nl ru tr
LANGSRCS = $(LANGOBJS:%=%.l)

LANGCOMP = ./langcomp
#LANGCOMP = ./langcomp -w


all: $(LANGOBJS)


install: all
	mkdir -p "$(INSTALL_PREFIX)$(DATDEST)/languages"
	cp $(LANGOBJS) "$(INSTALL_PREFIX)$(DATDEST)/languages"


clean:
	rm -f $(LANGOBJS) langcomp

spotless: clean
	rm -f langstrs.h


$(LANGOBJS): %: %.l langcomp index
	$(LANGCOMP) $<

ja_jis.l: ja_euc.l jconv.pl
	perl jconv.pl -jis <ja_euc.l >$@
ja_sjis.l: ja_euc.l jconv.pl
	perl jconv.pl -sjis <ja_euc.l >$@

langcomp: langcomp.c
	$(CC) $(CFLAGS) langcomp.c -o $@


langstrs.h: index make-langstrs-h.pl
	perl make-langstrs-h.pl >$@.new
	@if cmp $@ $@.new >/dev/null 2>&1 ; then \
		echo "$@ unchanged" ; \
		rm -f $@.new ; \
	else \
		mv -f $@.new $@ ; \
	fi

index: en_us.l
	grep '^[A-Z]' <en_us.l >$@
