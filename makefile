CFLAGS = -g -mglibc
LDFLAGS = -lm
.PHONY: all

DEPS = getop2.o getch.o stack.o 

all: revpol 

revpol : revpol.o $(DEPS)
	$(CC) $(CFLAGS) -o revpol revpol.o $(DEPS) $(LDFLAGS) 
	$@

revpol.c: calc.h makefile
	touch $@

install:
	strip revpol
	cp revpol ~/bin/rp
