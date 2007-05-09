# $Id$

ifeq ($(shell [ ! -r ../Make.Rules ] && echo 1),)
	include ../Make.Rules
endif

.PHONY: all clean install uninstall

all:
	@true

clean:
	@true
	rm -rf $(DESTDIR)$(includedir)/libhashish

distclean: clean
	@true

install:
	mkdir -p $(DESTDIR)$(includedir)/libhashish
	install -m 0644 libhashish.h $(DESTDIR)$(includedir)/libhashish/

uninstall:
	@true
	rm -rf $(DESTDIR)$(includedir)/libhashish
