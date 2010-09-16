ifeq ($(shell [ ! -r ../../Make.Rules ] && echo 1),)
	include ../../Make.Rules
endif

CIN      := $(wildcard *.c)
OBJ      := $(CIN:%.c=%.o)
INCLUDEDIR += -I.. -I../../include
LIBFLAGS := -lm -L../../lib -L../../localhash -lhashish_s -llocalhash

export

.PHONY: all clean bench

all: benchtool

benchtool: benchtool.c
	$(CC) $(INCLUDEDIR) $(CFLAGS) $< $(LIBFLAGS) -o $@

clean:
	@echo "### cleaning"; \
	$(RM) -f $(OBJ) benchtool bench-*.data bench.pdf

distclean:
	@echo "### distclean"
	@true

install:
	@true

