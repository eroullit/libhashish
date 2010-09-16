
ifeq ($(shell [ ! -r ../Make.Rules ] && echo 1),)
    include ../Make.Rules
endif

CIN      := $(wildcard *.c)

OBJ      := $(CIN:%.c=%.o)
INCLUDEDIR += -I../include
OUT_SLIB := $(addsuffix .a,$(PACKAGE_NAME))

LDFLAGS  = --warn-common

# static library name
LIBHASHSTATIC = liblocalhash.a

all: $(LIBHASHSTATIC) $(CIN)

%.o: %.c
	$(CC) $(INCLUDEDIR) $(CFLAGS) $< -c -o $@

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
	@true

 export
 
.PHONY: all clean install

