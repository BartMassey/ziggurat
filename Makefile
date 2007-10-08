# Copyright (C) 2007 Bart Massey
# All rights reserved.
#
# Please see the file COPYING in this distribution for usage
# terms.
#
DESTDIR = /local
INCDIR = $(DESTDIR)/include/ziggurat
LIBDIR = $(DESTDIR)/lib/ziggurat
CC = gcc
CFLAGS = -g -Wall -O4
LIBS = -lm
TABS = normal_tab.c exponential_tab.c polynomial_tab.c
DOBJS = normal.o exponential.o polynomial.o \
        normal_tab.o exponential_tab.o polynomial_tab.o
OBJS = random.o isaac.o $(DOBJS)

librandom.a: $(OBJS)
	$(AR) crv librandom.a $(OBJS)
	ranlib librandom.a

$(TABS): mktables
	./mktables

mktables: mktables.c zigconsts.h
	$(CC) $(CFLAGS) -o mktables mktables.c $(LIBS)

$(DOBJS): zigconsts.h

$(OBJS): random.h

check: normaltest.dat polytest.dat test.gnuplot
	gnuplot test.gnuplot

normaltest.dat: normaltest
	time ./normaltest > normaltest.dat

normaltest: normaltest.c random.h librandom.a
	$(CC) $(CFLAGS) -o normaltest normaltest.c librandom.a $(LIBS)

polytest.dat: polytest
	time ./polytest > polytest.dat

polytest: polytest.c random.h librandom.a
	$(CC) $(CFLAGS) -o polytest polytest.c librandom.a $(LIBS)

polyzig: polyzig.c
	$(CC) $(CFLAGS) -o polyzig polyzig.c $(LIBS)

install: librandom.a
	-[ -d $(INCDIR) ] || mkdir $(INCDIR)
	cp random.h $(INCDIR)
	-[ -d $(LIBDIR) ] || mkdir $(LIBDIR)
	cp librandom.a $(LIBDIR)

clean:
	-rm -f $(OBJS) librandom.a $(TABS) mktables normaltest.dat normaltest
