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

clean:
	-rm -f *.o librandom.a mktables random_tables.c
