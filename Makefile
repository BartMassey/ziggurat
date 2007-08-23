#   Copyright (C) 2007 Bart Massey
#   All rights reserved.
#
#   Redistribution and use in source and binary forms, with or without
#   modification, are permitted provided that the following conditions
#   are met:
#   
#     1. Redistributions of source code must retain the above copyright
#        notice, this list of conditions and the following disclaimer.
#
#     2. Redistributions in binary form must reproduce the above copyright
#        notice, this list of conditions and the following disclaimer in the
#        documentation and/or other materials provided with the distribution.
#
#     3. The names of its contributors may not be used to endorse or promote 
#        products derived from this software without specific prior written 
#        permission.
#
#   THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
#   "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
#   LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
#   A PARTICULAR PURPOSE ARE DISCLAIMED.  IN NO EVENT SHALL THE COPYRIGHT OWNER 
#   OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
#   EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
#   PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR
#   PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF
#   LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING
#   NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
#   SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
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

test: test.c random.h librandom.a
	$(CC) $(CFLAGS) -o test test.c librandom.a -lm

install: librandom.a
	-mkdir $(DESTDIR)/include/ziggurat
	cp random.h $(DESTDIR)/include/ziggurat/
	-mkdir $(DESTDIR)/lib/ziggurat
	cp librandom.a $(DESTDIR)/lib/ziggurat/

clean:
	-rm -f *.o librandom.a mktables random_tables.c
