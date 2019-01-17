CURDIR = $(dir $(realpath $(firstword $(MAKEFILE_LIST))))
INCDIR = $(CURDIR)include
SRCDIR = $(CURDIR)src
COMDIR = $(CURDIR)../common/src
OUTDIR = $(CURDIR)obj
PRJNAME = $(shell basename $(CURDIR))
LIBRARIES = -lwiringPi

CC=gcc
CFLAGS=

DEPS = $(wildcard $(SRCDIR)/*.h) $(wildcard $(COMDIR)/*.h)
OBJ = $(patsubst $(SRCDIR)/%.c,$(OUTDIR)/%.o,$(wildcard $(SRCDIR)/*.c)) $(patsubst $(COMDIR)/%.c,$(OUTDIR)/%.o,$(wildcard $(COMDIR)/*.c))

$(OUTDIR)/%.o: $(SRCDIR)/%.c $(DEPS)
	$(CC) -I $(COMDIR) -c -o $@ $< $(CFLAGS)

cnc_interface: $(OBJ)
	gcc -o $@ $^ $(CFLAGS) $(LIBRARIES)

.PHONY: clean

clean:
	rm -f $(OUTDIR)/*.o *~ core
