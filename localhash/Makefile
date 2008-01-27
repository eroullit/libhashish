
ifeq ($(shell [ ! -r ../Make.Rules ] && echo 1),)
    include ../Make.Rules
endif

CIN      := $(wildcard *.c)

OBJ      := $(CIN:%.c=%.o)
CFLAGS   += -I../include
OUT_SLIB := $(addsuffix .a,$(PACKAGE_NAME))

LDFLAGS  = --warn-common

# static library name
LIBHASHSTATIC = liblocalhash.a

all: $(LIBHASHSTATIC) $(CIN)


$(LIBHASHSTATIC): $(OBJ) ../Make.Rules
	@echo "### build static archiv"
	$(AR) cr $@ $(OBJ) 
	$(RANLIB) $@

clean:
	@echo "### clean-up library directory"; \
	$(RM) -f $(OBJ) $(OUT_SLIB) $(OUT_DLIB) $(LIBHASHSTATIC)
 
distclean:
	@echo "### distclean library directory"
	@true
 
install:
	$(MKDIR) -p $(DESTDIR)$(libdir)/
	$(INSTALL) -m 0644 $(OUT_SLIB) $(DESTDIR)$(libdir)
	$(INSTALL) -m 0644 $(OUT_DLIB) $(DESTDIR)$(libdir)
	$(LDCONFIG)
	( cd $(LIBDIR) ; ln -sf $(LIBHASHNAME) $(LIBHASH) )

 export
 
.PHONY: all clean install

