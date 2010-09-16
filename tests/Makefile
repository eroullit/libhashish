# $Id$

ifeq ($(shell [ ! -r ../Make.Rules ] && echo 1),)
    include ../Make.Rules
endif

CIN      := $(wildcard *.c)

INCLUDEDIR += -I../include
LIBFLAGS := -lm 
LIBFLAGS += -L../lib -L../localhash -lhashish_s -llocalhash
GRAPHFALGS :=

THREADFLAGSX = -lpthread

# maybe not a nifty way to check for libgd support but, ... ;-)
ifneq (,$(findstring gd,$(LIBGDFLAGS)))
	GRAPHFALGS += $(LIBGDFLAGS)
endif

export

.PHONY: all clean install unit_test string_distribution memory_leak_inspector \
concurrent_test bloom_filter_test

all: unit_test memory_leak_inspector iterator_test concurrent_test bloom_filter_test # string_distribution concurrent_test


unit_test: unit_test.c
	$(CC) $(INCLUDEDIR) $(CFLAGS) $< $(LIBFLAGS) -o $@

bloom_filter_test: bloom_filter_test.c
	$(CC) $(INCLUDEDIR) $(CFLAGS) $< $(LIBFLAGS) -o $@

concurrent_test: concurrent_test.c
	$(CC) $(INCLUDEDIR) $(CFLAGS) $< $(LIBFLAGS) $(THREADFLAGSX) -o $@

iterator_test: iterator_test.c
	$(CC) $(INCLUDEDIR) $(CFLAGS) $< $(LIBFLAGS) $(THREADFLAGSX) -o $@

string_distribution: string_distribution.c
	$(CC) $(INCLUDEDIR) $(CFLAGS) $< $(LIBFLAGS) $(THREADFLAGSX) -o $@

memory_leak_inspector: memory_leak_inspector.c
	$(CC) $(INCLUDEDIR) $(CFLAGS) $< $(LIBFLAGS) $(THREADFLAGSX) -o $@

test: 
	@echo $(shell ./unit_test 1>/dev/null 2>&1; \
					if [ $$? -ne 0 ] ; then  \
					echo "./unit_test failed" ;\
					else  echo "./unit test passed" ; \
					fi ;\
					)
	@echo $(shell ./bloom_filter_test 1>/dev/null 2>&1; \
					if [ $$? -ne 0 ] ; then  \
					echo "./bloom_filter_test failed" ; \
					else  echo "./bloom_filter_test passed" ; \
					fi ; \
					)
	@echo $(shell ./iterator_test 1>/dev/null 2>&1; \
					if [ $$? -ne 0 ] ; then  \
					echo "./iterator_test failed" ; else \
					echo "./iterator_test passed" ; \
					fi ; \
					)
ifneq (,$(findstring lpthread,$(THREADFLAGS)))
	@echo $(shell ./concurrent_test 1>/dev/null 2>&1;\
					if [ $$? -ne 0 ] ; then \
					echo "./concurrent_test failed" ; \
					else  echo "./concurrent_test passed" ; \
					fi ; \
					)
endif

clean:
	@echo "### cleaning"; \
	$(RM) -f $(OBJ) unit_test string_distribution
	$(RM) -f concurrent_test memory_leak_inspector *.png core*
	$(RM) -f iterator_test bloom_filter_test

distclean:
	@echo "### distclean testdir"
	@true

install:
	@true

