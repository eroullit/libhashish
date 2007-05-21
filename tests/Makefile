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

.PHONY: all clean install unit_test string_distribution

all: unit_test string_distribution

unit_test: $(OBJ)
	$(CC) $(CFLAGS) unit_test.o xfuncs.o $(LIBFLAGS) -o $@

string_distribution: $(OBJ)
	$(CC) $(CFLAGS) string_distribution.o xfuncs.o $(LIBFLAGS) $(GRAPHFALGS) -o $@

clean:
	@echo "### cleaning"; \
	$(RM) -f $(OBJ) unit_test string_distribution *.png core

distclean:
	@echo "### distclean testdir"
	@true

install:
	@true

