CURDIR = $(dir $(realpath $(firstword $(MAKEFILE_LIST))))
SRCDIR = $(CURDIR)src
COMDIR = $(CURDIR)../common/src
OUTDIR = $(CURDIR)obj
PRJNAME = $(shell basename $(CURDIR))
LIBRARIES = -lwiringPi

CC=gcc
CFLAGS=-I$(COMDIR)

DEPS = $(wildcard $(SRCDIR)/*.h) $(wildcard $(COMDIR)/*.h)
OBJ = $(patsubst $(SRCDIR)/%.c,$(OUTDIR)/%.o,$(wildcard $(SRCDIR)/*.c))

$(OUTDIR)/%.o: $(SRCDIR)/%.c $(DEPS)
	$(CC) -c -o $@ $< $(CFLAGS)

cnc_interface: $(OBJ)
	gcc -o $@ $^ $(CFLAGS) $(LIBRARIES)

objects:
	echo $(wildcard $(OUTDIR)/*)
	
includes:
	echo $(wildcard $(DEPS)/*)

.PHONY: clean

clean:
	rm -f $(OUTDIR)/*.o *~ core $(COMDIR)/*~
