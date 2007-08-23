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

librandom.a: random_tables.o random.o
	$(AR) crv librandom.a random_tables.o random.o
	ranlib librandom.a

random_tables.c: mktables
	./mktables > random_tables.c

mktables: mktables.c zigconsts.h
	$(CC) $(CFLAGS) -o mktables mktables.c $(LIBS)

random_tables.o random.o: zigconsts.h random.h

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
	-rm -f *.o librandom.a random_tables.c mktables test.dat test
