################################################################################
# Automatically-generated file. Do not edit!
################################################################################

-include ../makefile.init

RM := rm -rf

# All of the sources participating in the build are defined here
-include sources.mk
-include src/subdir.mk
-include emlib/subdir.mk
-include Drivers/subdir.mk
-include CMSIS/EFM32PG12B/subdir.mk
-include subdir.mk
-include objects.mk

ifneq ($(MAKECMDGOALS),clean)
ifneq ($(strip $(CC_DEPS)),)
-include $(CC_DEPS)
endif
ifneq ($(strip $(C++_DEPS)),)
-include $(C++_DEPS)
endif
ifneq ($(strip $(C_UPPER_DEPS)),)
-include $(C_UPPER_DEPS)
endif
ifneq ($(strip $(CXX_DEPS)),)
-include $(CXX_DEPS)
endif
ifneq ($(strip $(C_DEPS)),)
-include $(C_DEPS)
endif
ifneq ($(strip $(CPP_DEPS)),)
-include $(CPP_DEPS)
endif
endif

-include ../makefile.defs

# Add inputs and outputs from these tool invocations to the build variables 

# All Target
all: machine.axf

# Tool invocations
machine.axf: $(OBJS) $(USER_OBJS)
	@echo 'Building target: $@'
	@echo 'Invoking: GNU ARM C++ Linker'
	arm-none-eabi-g++ -g3 -gdwarf-2 -mcpu=cortex-m4 -mthumb -T "machine.ld" -L"C:/SiliconLabs/SimplicityStudio/v4/developer/sdks/gecko_sdk_suite/v2.3//platform/emdrv/" -L"C:/SiliconLabs/SimplicityStudio/v4/developer/sdks/gecko_sdk_suite/v2.3//platform/emdrv/nvm3/lib/" -u _printf_float --specs=nosys.specs -Xlinker --gc-sections -Xlinker -Map="machine.map" -mfpu=fpv4-sp-d16 -mfloat-abi=softfp --specs=nano.specs -u _printf_float -u _scanf_float -o machine.axf "./src/cnc_adc.o" "./src/cnc_functions.o" "./src/cnc_gpio.o" "./src/cnc_heaters.o" "./src/cnc_instructions.o" "./src/cnc_motors.o" "./src/cnc_pid.o" "./src/cnc_spi.o" "./src/cnc_timers.o" "./src/main.o" "./src/util.o" "./emlib/em_system.o" "./Drivers/dmadrv.o" "./Drivers/gpiointerrupt.o" "./Drivers/nvm.o" "./Drivers/nvm3_default.o" "./Drivers/nvm3_hal_flash.o" "./Drivers/nvm3_lock.o" "./Drivers/nvm_hal.o" "./Drivers/rtcdriver.o" "./Drivers/sleep.o" "./Drivers/spidrv.o" "./Drivers/tempdrv.o" "./Drivers/uartdrv.o" "./Drivers/ustimer.o" "./CMSIS/EFM32PG12B/startup_gcc_efm32pg12b.o" "./CMSIS/EFM32PG12B/system_efm32pg12b.o" -lnvm3_CM4_gcc "./Drivers/em_gpio.o" "./emlib/em_adc.o" "./emlib/em_cmu.o" "./emlib/em_core.o" "./emlib/em_emu.o" "./emlib/em_ldma.o" "./emlib/em_letimer.o" "./emlib/em_msc.o" "./emlib/em_rtc.o" "./emlib/em_rtcc.o" "./emlib/em_timer.o" "./emlib/em_usart.o" -Wl,--start-group -lgcc -lc -lnosys -Wl,--end-group
	@echo 'Finished building target: $@'
	@echo ' '

	@echo 'Building hex file: machine.hex'
	arm-none-eabi-objcopy -O ihex "machine.axf" "machine.hex"
	@echo ' '

	@echo 'Building bin file: machine.bin'
	arm-none-eabi-objcopy -O binary "machine.axf" "machine.bin"
	@echo ' '

	@echo 'Building s37 file: machine.s37'
	arm-none-eabi-objcopy -O srec "machine.axf" "machine.s37"
	@echo ' '

	@echo 'Running size tool'
	arm-none-eabi-size "machine.axf"
	@echo ' '

# Other Targets
clean:
	-$(RM) $(CC_DEPS)$(C++_DEPS)$(EXECUTABLES)$(OBJS)$(C_UPPER_DEPS)$(CXX_DEPS)$(C_DEPS)$(CPP_DEPS) machine.axf
	-@echo ' '

.PHONY: all clean dependents
.SECONDARY:

-include ../makefile.targets
