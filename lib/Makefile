#
.PHONY:	dummy


include ../defs.mk
include files.mk

TARGET=libinitc.a




all:	$(TARGET)




tidy:	dummy
	$(RM) *~


libinitc.a:	$(OBJS)
	$(AR) cr $@ $^




kversion_test:	kversion_test.c kversion.o
	$(CC) -o $@ -static $^




clean:
	$(RM) $(TARGET)
	$(RM) *.o


#############################################################################
.c.o:
	$(CC) -o $@ -c $(CFLAGS) $(INCLUDES) $<

