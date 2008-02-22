# $Id: Makefile 20 2008-01-23 23:46:02Z hgndgtl $


SUBDIRS = avalanche bloom_distribution hash-time hashstr
  
all clean:
	@for dir in $(SUBDIRS); do \
	(cd $$dir && $(MAKE) $@); \
	done
