PWD = $(shell pwd)
CFLAGS = -Wall  -I$(PWD)/../apue.3e/include
LDFLAGS = -L$(PWD)/../apue.3e/lib
LIBS = -lapue
SUBDIR = fileio

EXEC = a 
all: $(EXEC)

a: a.c 
	gcc $(CFLAGS) $<  $(LDFLAGS) $(LIBS)  -o $@

.PHONY: fileio

fileio: fileio/rwseek
	(cd $@ ; $(MAKE))

clean:
	rm -f *.o *.ko a
	list=$(SUBDIR) ; for subdir in $$list ; do \
	(cd $$subdir; $(MAKE) clean); \
	done

export CFLAGS LDFLAGS LIBS 
