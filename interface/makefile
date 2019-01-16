CURDIR = $(dir $(realpath $(firstword $(MAKEFILE_LIST))))
INCDIR = $(CURDIR)include
SRCDIR = $(CURDIR)src
OUTDIR = $(CURDIR)obj
PRJNAME = $(shell basename $(CURDIR))
LIBRARIES = -lwiringPi

CC=gcc
CFLAGS=

DEPS = $(wildcard $(SRCDIR)/*.h)
OBJ = $(patsubst $(SRCDIR)/%.c,$(OUTDIR)/%.o,$(wildcard $(SRCDIR)/*.c))

$(OUTDIR)/%.o: $(SRCDIR)/%.c $(DEPS)
	$(CC) -c -o $@ $< $(CFLAGS)

cnc_interface: $(OBJ)
	gcc -o $@ $^ $(CFLAGS) $(LIBRARIES)

.PHONY: clean

clean:
	rm -f $(OUTDIR)/*.o *~ core
