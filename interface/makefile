CURDIR = $(dir $(realpath $(firstword $(MAKEFILE_LIST))))
SRCDIR = $(CURDIR)src
COMDIR = $(CURDIR)../common/src
OUTDIR = $(CURDIR)obj
PRJNAME = $(shell basename $(CURDIR))
LIBRARIES = -lwiringPi

CC=g++
CFLAGS=-I$(COMDIR) -I$(SRCDIR) -DSPI_MASTER -DINTERFACE

SRC = $(wildcard $(SRCDIR)/*.cpp) $(wildcard $(COMDIR)/*.cpp)
DEPS = $(wildcard $(SRCDIR)/*.h) $(wildcard $(COMDIR)/*.h)
OBJ = $(patsubst $(SRCDIR)/%.cpp,$(OUTDIR)/%.o,$(wildcard $(SRCDIR)/*.cpp)) $(patsubst $(COMDIR)/%.cpp,$(OUTDIR)/%.o,$(wildcard $(COMDIR)/*.cpp))

$(OUTDIR)/%.o: $(SRCDIR)/%.cpp $(DEPS)
	$(CC) -c -o $@ $< $(CFLAGS)
	
$(OUTDIR)/%.o: $(COMDIR)/%.cpp $(DEPS)
	$(CC) -c -o $@ $< $(CFLAGS)

cnc_interface: $(OBJ)
	$(CC) -o $@ $^ $(CFLAGS) $(LIBRARIES)

objects:
	echo $(OBJ)
	
sources:
	echo $(SRC)

dependencies:
	echo $(DEPS)
	
includes:
	echo $(wildcard $(DEPS)/*)

.PHONY: clean

clean:
	rm -f $(OUTDIR)/*.o *~ core $(COMDIR)/*~
