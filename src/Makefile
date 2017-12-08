#
.PHONY:	dummy


include ../defs.mk
include files.mk
INCLUDES=-I../lib -I.

tidy:	dummy
	$(RM) *~


all:	$(OBJS)







#############################################################################
.c.o:
	$(CC) -o $@ -c $(CFLAGS) $(INCLUDES) $<



clean:
	$(RM) $(OBJS)
