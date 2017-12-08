#!/usr/bin/make -f

.PHONY:	dummy


include defs.mk
include src/files.mk


TARGETS=initc initc-static initc-debug


INCLUDES=-I$(shell pwd)/lib

objlist:=$(shell for f in $(OBJS); do fl="$$fl src/$$f"; done; echo "$$fl") 
LDFLAGS+=-Wl,--as-needed
LDFLAGS_STATIC=$(LDFLAGS) -Wl,--gc-sections -flto
LIBS=lib/libinitc.a -lpopt


all:	$(TARGETS)


libinitc:
	$(MAKE) -C lib clean all

libinitc-stamp:	libinitc
	touch $@


initc:	libinitc-stamp
	$(MAKE) -C src clean all
	$(CC) -o $@ $(objlist) $(LDFLAGS) $(LIBS)
#	$(STRIP) $@


initc-static:	libinitc-stamp
	$(MAKE) -C src clean all
	$(CC) -static -o $@ $(objlist) $(LDFLAGS_STATIC) $(LIBS)
	$(STRIP) $@

initc-debug:	libinitc-stamp
	$(MAKE) -C src clean all CFLAGS="$(CFLAGS) -DDEBUG"
	$(CC) -static -o $@ $(objlist) $(LDFLAGS_STATIC) $(LIBS)




tidy:
	$(RM) *~

clean:	tidy
	$(RM) libinitc-stamp
	$(MAKE) -C lib clean
	$(MAKE) -C src clean
	$(RM) $(TARGETS)


#############################################################################
.c.o:
	$(CC) -o $@ -c $(CFLAGS) $(INCLUDES) $<
