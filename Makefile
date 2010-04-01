# Copyright (C) 2007 Bart Massey
# All rights reserved.
#
# Please see the file COPYING in this distribution for usage
# terms.
#
DESTDIR = /usr/local
INCDIR = $(DESTDIR)/include
LIBDIR = $(DESTDIR)/lib
CC = gcc
CFLAGS = -g -O4 -Werror -Wall -Wextra -Wmissing-prototypes -Wwrite-strings
LIBS = -lm
TABS = normal_tab.c exponential_tab.c polynomial_tab.c
DOBJS = normal.o exponential.o polynomial.o \
        normal_tab.o exponential_tab.o polynomial_tab.o
OBJS = random.o isaac.o $(DOBJS)

libzrandom.a: $(OBJS)
	$(AR) crv libzrandom.a $(OBJS)
	ranlib libzrandom.a

$(TABS): mktables
	./mktables

mktables: mktables.c zigconsts.h
	$(CC) $(CFLAGS) -o mktables mktables.c $(LIBS)

$(DOBJS): zigconsts.h

$(OBJS): zrandom.h

check: normaltest.dat polytest.dat test.gnuplot
	gnuplot test.gnuplot

normaltest.dat: normaltest
	time ./normaltest > normaltest.dat

normaltest: normaltest.c zrandom.h libzrandom.a
	$(CC) $(CFLAGS) -o normaltest normaltest.c libzrandom.a $(LIBS)

polytest.dat: polytest
	time ./polytest > polytest.dat

polytest: polytest.c zrandom.h libzrandom.a
	$(CC) $(CFLAGS) -o polytest polytest.c libzrandom.a $(LIBS)

polyzig: polyzig.c
	$(CC) $(CFLAGS) -o polyzig polyzig.c $(LIBS)

install: libzrandom.a
	-[ -d $(INCDIR) ] || mkdir $(INCDIR)
	cp zrandom.h $(INCDIR)/
	-[ -d $(LIBDIR) ] || mkdir $(LIBDIR)
	cp libzrandom.a $(LIBDIR)/

clean:
	-rm -f $(OBJS) libzrandom.a $(TABS) mktables \
               normaltest.dat normaltest \
               polytest.dat polytest \
               polyzig
