# $Id: Makefile 20 2008-01-23 23:46:02Z hgndgtl $

ifeq ($(shell [ ! -r ../../Make.Rules ] && echo 1),)
	include ../../Make.Rules
endif

CIN      := $(wildcard *.c)
OBJ      := $(CIN:%.c=%.o)
INCLUDEDIR += -I.. -I../../include
LIBFLAGS := -lm -L../../lib -L../../localhash -lhashish_s -llocalhash

# maybe not a nifty way to check for libgd support but, ... ;-)
ifneq (,$(findstring gd,$(LIBGDFLAGS)))
	LIBCANDYFLAGS += $(LIBGDFLAGS)
endif

export

.PHONY: all clean bloom_distribution false_positiv_probability_table analysis

all: bloom_distribution false_positiv_probability_table

false_positiv_probability_table: false_positiv_probability_table.c
	$(CC) $(INCLUDEDIR) $(CFLAGS) $< $(LIBFLAGS) -o $@

bloom_distribution: bloom_distribution.c
	$(CC) $(INCLUDEDIR) $(CFLAGS) $< $(LIBFLAGS) $(LIBCANDYFLAGS) -o $@

analysis: false_positiv_probability_table
	./false_positiv_probability_table > false_positiv_probability_table.data
	gnuplot false_positiv_probability_table.gpi

clean:
	@echo "### cleaning"; \
	$(RM) -f $(OBJ) bloom_distribution false_positiv_probability_table \
	*.png core* *.pdf *.data

distclean:
	@echo "### distclean"
	@true

install:
	@true

