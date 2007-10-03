# Copyright (C) 2007 Bart Massey
# All rights reserved.
#
# Please see the file COPYING in this distribution for usage
# terms.
#
DESTDIR = /local
CC = gcc
CFLAGS = -g -Wall -O4
LIBS = -lm
TABS = normal_tab.c exponential_tab.c polynomial_tab.c
DOBJS = normal.o exponential.o polynomial.o \
        normal_tab.o exponential_tab.o polynomial_tab.o
OBJS = random.o random_compat.o $(DOBJS)

librandom.a: $(OBJS)
	$(AR) crv librandom.a $(OBJS)
	ranlib librandom.a

$(TABS): mktables
	./mktables

mktables: mktables.c zigconsts.h
	$(CC) $(CFLAGS) -o mktables mktables.c $(LIBS)

$(DOBJS): zigconsts.h

$(OBJS): random.h

check: test.dat
	gnuplot test.gnuplot

test.dat: test
	time ./test > test.dat

test: test.c random.h librandom.a
	$(CC) $(CFLAGS) -o test test.c librandom.a $(LIBS)

install: librandom.a
	-mkdir $(DESTDIR)/include/ziggurat
	cp random.h $(DESTDIR)/include/ziggurat/
	-mkdir $(DESTDIR)/lib/ziggurat
	cp librandom.a $(DESTDIR)/lib/ziggurat/

clean:
	-rm -f $(OBJS) librandom.a $(TABS) mktables test.dat test
