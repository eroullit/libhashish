
ifeq ($(shell [ ! -r ../Make.Rules ] && echo 1),)
    include ../Make.Rules
endif

CIN      := $(wildcard *.c)

OBJ      := $(CIN:%.c=%.o)
CFLAGS   += -I../include
OUT_SLIB := $(PACKAGE_NAME).a

export

.PHONY: all clean install $(OUT_SLIB)


all: $(CIN)
	@echo "### make $(OUT_SLIB)"; \
	$(MAKE) $(OUT_SLIB)

$(OUT_SLIB): ../Make.Rules $(OBJ)
	$(AR) cr $(OUT_SLIB) $(OBJ)
	$(RANLIB) $(OUT_SLIB)

clean:
	@echo "### clean lib"; \
	$(RM) -f $(OBJ) $(OUT_SLIB) \

distclean:
	@echo "### distclean lib"
	@true

install:
	$(MKDIR) -p $(DESTDIR)$(libdir)/
	$(INSTALL) -m 0644 $(OUT_SLIB) $(DESTDIR)$(libdir)


