CFLAGS = -g -mglibc
LDFLAGS = 
.PHONY: all

DEPS = tab_settings.o 

all: detab entab

detab : detab.o $(DEPS)
	$(CC) $(CFLAGS) -o detab $< $(DEPS) $(LDFLAGS) 

detab.c: tab_consts.h tab_settings.h makefile
	touch $@

entab : entab.o $(DEPS)
	$(CC) $(CFLAGS) -o entab $< $(DEPS) $(LDFLAGS) 

entab.c: tab_consts.h tab_settings.h makefile
	touch $@

tab_settings.o: tab_consts.h tab_settings.h 

install:
	strip detab
	cp detab $(BINDIR)
