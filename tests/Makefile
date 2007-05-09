# $Id$

ifeq ($(shell [ ! -r ../Make.Rules ] && echo 1),)
    include ../Make.Rules
endif

CIN      := $(wildcard *.c)

OBJ      := $(CIN:%.c=%.o)
CFLAGS   += -I../include
LIBFLAGS := -lm 
LIBFLAGS += -L../lib -lhashish
GRAPHFALGS :=

# maybe not a nifty way to check for libgd support but, ... ;-)
ifneq (,$(findstring gd,$(LIBGDFLAGS)))
	GRAPHFALGS += $(LIBGDFLAGS)
endif

export

.PHONY: all clean install hashtest_simple string_distribution

all: hashtest_simple string_distribution

hashtest_simple: $(OBJ)
	$(CC) $(CFLAGS) hashtest_simple.o $(LIBFLAGS) -o $@

string_distribution: $(OBJ)
	$(CC) $(CFLAGS) string_distribution.o $(LIBFLAGS) $(GRAPHFALGS) -o $@

clean:
	@echo "### cleaning"; \
	$(RM) -f $(OBJ) hashtest_simple string_distribution *.png core

distclean:
	@echo "### distclean testdir"
	@true

install:
	@true

