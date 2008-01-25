# $Id$

ifeq ($(shell [ ! -r ../Make.Rules ] && echo 1),)
    include ../Make.Rules
endif

CIN      := $(wildcard *.c)

OBJ      := unit_test.o xfuncs.o memory_leak_inspector.o concurrent_test.o #$(CIN:%.c=%.o)
CFLAGS   += -I../include
LIBFLAGS := -lm 
LIBFLAGS += -L../lib -lhashish_s
GRAPHFALGS :=

THREADFLAGSX = -lpthread

# maybe not a nifty way to check for libgd support but, ... ;-)
ifneq (,$(findstring gd,$(LIBGDFLAGS)))
	GRAPHFALGS += $(LIBGDFLAGS)
endif

export

.PHONY: all clean install unit_test string_distribution memory_leak_inspector concurrent_test

all: unit_test memory_leak_inspector concurrent_test # string_distribution concurrent_test

unit_test: $(OBJ)
	$(CC) $(CFLAGS) unit_test.o xfuncs.o $(LIBFLAGS) -o $@

concurrent_test: $(OBJ)
	$(CC) $(CFLAGS) concurrent_test.o xfuncs.o $(LIBFLAGS) $(THREADFLAGSX) -o $@

string_distribution: $(OBJ)
	$(CC) $(CFLAGS) string_distribution.o xfuncs.o $(LIBFLAGS) $(GRAPHFALGS) -o $@

memory_leak_inspector: $(OBJ)
	$(CC) $(CFLAGS) memory_leak_inspector.o xfuncs.o $(LIBFLAGS) -o $@

clean:
	@echo "### cleaning"; \
	$(RM) -f $(OBJ) unit_test string_distribution concurrent_test memory_leak_inspector *.png core*

distclean:
	@echo "### distclean testdir"
	@true

install:
	@true

