################################################################################
# Automatically-generated file. Do not edit!
################################################################################

SHELL = cmd.exe

CG_TOOL_ROOT := c:/ti/ccsv6/tools/compiler/gcc-arm-none-eabi-4_7-2013q3

ORDERED_OBJS += \
"./libnrf24l01/src/nRF24L01.o" \
"./libnrf24l01/src/TM4C123_nRF24L01.o" \
"./usb_swarm_control_structs.o" \
"./tm4c123gh6pm_startup_ccs_gcc.o" \
"./main.o" \
"./distanceSensing.o" \
"./CustomTivaDrivers.o" \
-Wl,-T"../tm4c123ge6pm.lds" \
$(GEN_CMDS__FLAG) \
-Wl,--start-group \
-l"c" \
-lusb \
-l"gcc" \
-l"nosys" \
-l"driver" \
-Wl,--end-group \

-include ../makefile.init

RM := DEL /F
RMDIR := RMDIR /S/Q

# All of the sources participating in the build are defined here
-include sources.mk
-include subdir_vars.mk
-include libnrf24l01/src/subdir_vars.mk
-include subdir_rules.mk
-include libnrf24l01/src/subdir_rules.mk
-include objects.mk

ifneq ($(MAKECMDGOALS),clean)
ifneq ($(strip $(S_DEPS)),)
-include $(S_DEPS)
endif
ifneq ($(strip $(S_UPPER_DEPS)),)
-include $(S_UPPER_DEPS)
endif
ifneq ($(strip $(S62_DEPS)),)
-include $(S62_DEPS)
endif
ifneq ($(strip $(C64_DEPS)),)
-include $(C64_DEPS)
endif
ifneq ($(strip $(ASM_DEPS)),)
-include $(ASM_DEPS)
endif
ifneq ($(strip $(CC_DEPS)),)
-include $(CC_DEPS)
endif
ifneq ($(strip $(S55_DEPS)),)
-include $(S55_DEPS)
endif
ifneq ($(strip $(C67_DEPS)),)
-include $(C67_DEPS)
endif
ifneq ($(strip $(C??_DEPS)),)
-include $(C??_DEPS)
endif
ifneq ($(strip $(CLA_DEPS)),)
-include $(CLA_DEPS)
endif
ifneq ($(strip $(CPP_DEPS)),)
-include $(CPP_DEPS)
endif
ifneq ($(strip $(S??_DEPS)),)
-include $(S??_DEPS)
endif
ifneq ($(strip $(C_DEPS)),)
-include $(C_DEPS)
endif
ifneq ($(strip $(C62_DEPS)),)
-include $(C62_DEPS)
endif
ifneq ($(strip $(CXX_DEPS)),)
-include $(CXX_DEPS)
endif
ifneq ($(strip $(C++_DEPS)),)
-include $(C++_DEPS)
endif
ifneq ($(strip $(ASM_UPPER_DEPS)),)
-include $(ASM_UPPER_DEPS)
endif
ifneq ($(strip $(K_DEPS)),)
-include $(K_DEPS)
endif
ifneq ($(strip $(C43_DEPS)),)
-include $(C43_DEPS)
endif
ifneq ($(strip $(S67_DEPS)),)
-include $(S67_DEPS)
endif
ifneq ($(strip $(SA_DEPS)),)
-include $(SA_DEPS)
endif
ifneq ($(strip $(S43_DEPS)),)
-include $(S43_DEPS)
endif
ifneq ($(strip $(OPT_DEPS)),)
-include $(OPT_DEPS)
endif
ifneq ($(strip $(S64_DEPS)),)
-include $(S64_DEPS)
endif
ifneq ($(strip $(C_UPPER_DEPS)),)
-include $(C_UPPER_DEPS)
endif
ifneq ($(strip $(C55_DEPS)),)
-include $(C55_DEPS)
endif
endif

-include ../makefile.defs

# Add inputs and outputs from these tool invocations to the build variables 
EXE_OUTPUTS += \
usbSwarmControl.out \

EXE_OUTPUTS__QUOTED += \
"usbSwarmControl.out" \

BIN_OUTPUTS += \
usbSwarmControl.hex \

BIN_OUTPUTS__QUOTED += \
"usbSwarmControl.hex" \


# All Target
all: usbSwarmControl.out

dependents:
	-cd D:/Software\ Projects/ArmTI/usblib/Debug && $(MAKE) all
	-cd D:/Software\ Projects/ArmTI/driverlib/Debug && $(MAKE) all

# Tool invocations
usbSwarmControl.out: $(OBJS) $(LDS_SRCS) $(CMD_SRCS) $(GEN_CMDS) D:/Software\ Projects/ArmTI/usblib/Debug/usblib.lib D:/Software\ Projects/ArmTI/driverlib/Debug/driverlib.lib
	@echo 'Building target: $@'
	@echo 'Invoking: GNU Linker'
	"c:/ti/ccsv6/tools/compiler/gcc-arm-none-eabi-4_7-2013q3/bin/arm-none-eabi-gcc.exe" -march=armv7e-m -mthumb -mfloat-abi=softfp -mfpu=fpv4-sp-d16 -DPART_TM4C123GH6PM -DTARGET_IS_BLIZZARD_RB1 -Dgcc -O0 -ffunction-sections -fdata-sections -g -Wall -Wl,-Map,"usbSwarmControl.map" -L"D:/Portable/Programs/TivaDriver/driverlib/gcc" -L"D:/Portable/Programs/TivaDriver/usblib/gcc" -o"usbSwarmControl.out" $(ORDERED_OBJS)
	@echo 'Finished building target: $@'
	@echo ' '
	@$(MAKE) --no-print-directory post-build

usbSwarmControl.hex: $(EXE_OUTPUTS)
	@echo 'Invoking: GNU Objcopy Utility'
	"c:/ti/ccsv6/tools/compiler/gcc-arm-none-eabi-4_7-2013q3/bin/arm-none-eabi-objcopy.exe"  $(EXE_OUTPUTS__QUOTED) "usbSwarmControl.hex"
	@echo 'Finished building: $@'
	@echo ' '

# Other Targets
clean:
	-$(RM) $(EXE_OUTPUTS__QUOTED)$(BIN_OUTPUTS__QUOTED)
	-$(RM) "CustomTivaDrivers.d" "distanceSensing.d" "main.d" "tm4c123gh6pm_startup_ccs_gcc.d" "usb_swarm_control_structs.d" "libnrf24l01\src\TM4C123_nRF24L01.d" "libnrf24l01\src\nRF24L01.d" 
	-$(RM) "CustomTivaDrivers.o" "distanceSensing.o" "main.o" "tm4c123gh6pm_startup_ccs_gcc.o" "usb_swarm_control_structs.o" "libnrf24l01\src\TM4C123_nRF24L01.o" "libnrf24l01\src\nRF24L01.o" 
	-@echo 'Finished clean'
	-@echo ' '

post-build:
	-@echo 'Memory Allocation'
	-arm-none-eabi-size usbSwarmControl.out
	-@echo ' '

.PHONY: all clean dependents
.SECONDARY: post-build
D:/Software\ Projects/ArmTI/usblib/Debug/usblib.lib:
D:/Software\ Projects/ArmTI/driverlib/Debug/driverlib.lib:

-include ../makefile.targets

