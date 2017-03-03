PWD = $(shell pwd)
CFLAGS = -Wall  -I$(PWD)/../apue.3e/include
CFLAGS += -I$(PWD)/
LDFLAGS = -L$(PWD)/../apue.3e/lib
LIBS = -lapue
SUBDIR = fileio

all:
	@list=$(SUBDIR); for subdir in $$list; do \
		(cd $$subdir ; $(MAKE)) \
	done

.PHONY: fileio

fileio: fileio/rwseek
	@(cd $@ ; $(MAKE))

clean:
	@list=$(SUBDIR) ; for subdir in $$list ; do \
	(cd $$subdir; $(MAKE) clean); \
	done

export CFLAGS LDFLAGS LIBS 
