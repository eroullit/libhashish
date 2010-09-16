ifeq ($(shell [ ! -r ../../Make.Rules ] && echo 1),)
	include ../../Make.Rules
endif

CIN      := $(wildcard *.c)
OBJ      := $(CIN:%.c=%.o)
INCLUDEDIR += -I.. -I../../include
LIBFLAGS := -lm -L../../lib -L../../localhash -lhashish_s -llocalhash

export

.PHONY: all clean avalanche

all: avalanche

analysis: avalanche
	./avalanche >avalanche.data
	gnuplot avalanche.gpi

avalanche: avalanche.c
	$(CC) $(INCLUDEDIR) $(CFLAGS) $< $(LIBFLAGS) -o $@

clean:
	@echo "### cleaning"; \
	$(RM) -f $(OBJ) avalanche *.png *.pdf *.data core*

distclean:
	@echo "### distclean"
	@true

install:
	@true

