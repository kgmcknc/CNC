CURDIR = $(dir $(realpath $(firstword $(MAKEFILE_LIST))))
INCDIR = $(CURDIR)include
SRCDIR = $(CURDIR)src
OUTDIR = $(CURDIR)output
PRJNAME = $(shell basename $(CURDIR))

CC=gcc
CFLAGS=-I$(INCDIR)

DEPS = $(wildcard $(INCDIR)/*.h)
OBJ = $(patsubst $(SRCDIR)/%.c,$(OUTDIR)/%.o,$(wildcard $(SRCDIR)/*.c))
LIBS = wiringPi

$(OUTDIR)/%.o: $(SRCDIR)/%.c $(DEPS)
	$(CC) -c -o $@ $< $(CFLAGS) -lwiringPi

$(PRJNAME): $(OBJ)
	gcc -o $@ $^ $(CFLAGS)

.PHONY: clean

clean:
	rm -f $(OUTDIR)/*.o *~ core $(INCDIR)/*~
