# $Id$

ifeq ($(shell [ ! -r ../Make.Rules ] && echo 1),)
	include ../Make.Rules
endif


all: libhashish.pod
	pod2man --section=7 --center libhashish --date 0.1 libhashish.pod > libhashish.7

display: all
	groff -man -Tascii libhashish.7

ps: all
	groff -man -Tps libhashish.7 > libhashish.ps

pdf: ps
	ps2pdf libhashish.ps

clean:
	rm -rf libhashish.7 *.pdf *.ps

distclean: clean
	@true

install:
	@true

uninstall:
	@true

.PHONY: all clean install uninstall
