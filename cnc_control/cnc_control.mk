# This makefile should be invoked from the root of the project.  That is, not in the Debug/Release
# directory.  The Precision32 will invoke it from the Debug/Release directory, but by specifying
# -C .., make will change directories before proceding to build anything.
# options:
#   CONFIGNAME=(Debug|Release) 
#       Specifies whether to do a Debug or Release build, and which directory output should go to.
#       Debug includes debugging symbols in the output, and no optimization.  Powermode 8 is disabled, no ihex file is produced.
#       Release has no debugging symbols in the output, and tells the compiler to optimize.  Powermode 8 is enabled. A .hex file is produced
#   UPGONLY=1
#       If this variable is 1, the fw upgrade app will be compiled with debug symbols
#       and only the fw upg app will be flashed into the MCU.  
RM := rm -rf
CC = arm-none-eabi-gcc

ifeq ($(CONFIGNAME),debug)
	CCFLAGS := -DNDEBUG -DDEBUG -DNO_PM8 -O0 -gdwarf-2
	NO_BOOTLOADER = 1
else
# C standard mentions NDEBUG, but never DEBUG.  NDEBUG removes assert() from binary.
	CCFLAGS := -DNDEBUG -Os -gdwarf-2
	CCFLAGS += -DRELEASE_BUILD
#	CCFLAGS += -Werror=implicit-function-declaration
endif

LOCALDIRS := .
LOCALDIRS += cnc_control
LOCALDIRS += efm32pg_driver
LOCALDIRS += hal
#LOCALDIRS += spi_slave_driver
HALDIRS := efm32pg_driver/efm32pg_hal/cpu efm32pg_driver/efm32pg_hal/efm32pg
DIRS := $(LOCALDIRS) $(HALDIRS)
OUTDIR := $(CONFIGNAME)

LDSCRIPT := cnc_control.ld
LDLIBS := -Wl,--start-group -lgcc -lc -lnosys -Wl,--end-group

INCLUDES := $(addprefix -I,$(DIRS)) -Ifw_upg_app_src
CCFLAGS += -DEFM32PG12B500F1024GL125

CCFLAGS += -Wall -fmessage-length=0
CCFLAGS += -fno-builtin
CCFLAGS += -ffunction-sections 
CCFLAGS += -fdata-sections 

CCFLAGS += -std=c99 

CCFLAGS += -mcpu=cortex-m3
CCFLAGS += -mthumb
CCFLAGS += --specs=nano.specs

CCFLAGS += -Werror=return-type

CCFLAGS += -D__NO_SYSTEM_INIT
  
# the combination of -ffunction-sections and --gc-sections allows uncalled functions to be removed
LDFLAGS := -Map="$(OUTDIR)/cnc_control.map" --gc-sections
 
CFILES := $(foreach dir,$(DIRS),$(wildcard $(dir)/*.c)) 

DEPS := $(patsubst %.c,%.d,$(CFILES))
DEPS := $(addprefix $(OUTDIR)/,$(DEPS))

SFILES := $(foreach dir,$(DIRS),$(wildcard $(dir)/*.s)) 

OBJS := $(patsubst %.c,%.o,$(CFILES)) $(patsubst %.s,%.o,$(SFILES)) 
OBJS := $(addprefix $(OUTDIR)/,$(OBJS))

CCLDFLAGS := $(addprefix -Xlinker ,$(LDFLAGS))

# These allow fw_upg_app_makefile.mk to have access to these values.
export UPGONLY 
export CC
export RM
export CONFIGNAME
export INCLUDES
export OUTDIR
export HALDIRS
export NO_BOOTLOADER

all: $(OUTDIR)/cnc_control.bin $(OUTDIR)/cnc_control.hex 
	@echo
#	@echo `arm-none-eabi-gcc --version`
#	@echo `arm-none-eabi-size.exe -t $(OUTDIR)/*/*.o`
	@echo `arm-none-eabi-size.exe $(OUTDIR)/cnc_control.axf`	
	@echo "---" $(CONFIGNAME) "build complete ---"

# load the dependencies we found during previous build to see if .h changes will cause a rebuild
-include $(OBJS:.o=.d)
	
$(OUTDIR)/cnc_control.hex: $(OUTDIR)/cnc_control.axf
	arm-none-eabi-objcopy -O ihex $(OUTDIR)/cnc_control.axf $(OUTDIR)/cnc_control.hex

$(OUTDIR)/cnc_control.bin: $(OUTDIR)/cnc_control.axf
	arm-none-eabi-objcopy -O binary $(OUTDIR)/cnc_control.axf $(OUTDIR)/cnc_control.bin

$(OUTDIR)/cnc_control.txt: $(OUTDIR)/cnc_control.bin
	perl tools/bin_to_cmds.pl $(OUTDIR)/cnc_control.bin > $(OUTDIR)/cnc_control.txt	

$(OUTDIR)/cnc_control.axf $(OUTDIR)/cnc_control.map: $(OBJS)
	$(CC) $(CCLDFLAGS) $(CCFLAGS) -T "$(LDSCRIPT)" -o"$(OUTDIR)/cnc_control.axf" $(OBJS) $(LDLIBS)
	@echo

$(OUTDIR)/%.o: %.c
	@-mkdir -p $(OUTDIR)/$(dir $<)
	@echo "$*.c -> $(OUTDIR)/$*.o"
	@$(CC) -c $(CCFLAGS) $(INCLUDES) -MMD -MP -MF"$(OUTDIR)/$*.d" -MT"$(OUTDIR)/$*.d" -o "$(OUTDIR)/$*.o" "$*.c" 	
	
$(OUTDIR)/%.o: %.s
	@-mkdir -p $(OUTDIR)/$(dir $<)
	@echo "$*.s -> $(OUTDIR)/$*.o"
	@$(CC) -c -o "$(OUTDIR)/$*.o" "$*.s" 	
	@echo	

clean:
	-$(RM) $(DEPS) $(OBJS) "$(OUTDIR)/cnc_control.axf" "$(OUTDIR)/cnc_control.bin" "$(OUTDIR)/cnc_control.hex" "$(OUTDIR)/cnc_control.map" "$(OUTDIR)/cnc_control.txt"
	@echo

distclean: clean
	-$(RM) $(DEPS)
	@echo	
	
post-build:

.PHONY: all clean distclean 
.SECONDARY: post-build
