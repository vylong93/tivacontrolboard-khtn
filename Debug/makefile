################################################################################
# Automatically-generated file. Do not edit!
################################################################################

SHELL = cmd.exe

CG_TOOL_ROOT := D:/TI/ccsv6/tools/compiler/arm_5.1.6

ORDERED_OBJS += \
$(GEN_CMDS__FLAG) \
"./ControlBoard.obj" \
"./main.obj" \
"./tm4c123gh6pm_startup_ccs.obj" \
"./usb_swarm_control_structs.obj" \
"./libnrf24l01/src/TM4C123_nRF24L01.obj" \
"./libnrf24l01/src/nRF24L01.obj" \
"../tm4c123gh6pm.cmd" \
-l"libc.a" \
-l"D:\Program Files\TI\TivaWare_C_Series-2.1.0.12573\usblib\ccs\Debug\usblib.lib" \
-l"D:\Program Files\TI\TivaWare_C_Series-2.1.0.12573\driverlib\ccs\Debug\driverlib.lib" \

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
TivaControlBoard.out \

EXE_OUTPUTS__QUOTED += \
"TivaControlBoard.out" \

BIN_OUTPUTS += \
TivaControlBoard.hex \

BIN_OUTPUTS__QUOTED += \
"TivaControlBoard.hex" \


# All Target
all: TivaControlBoard.out

# Tool invocations
TivaControlBoard.out: $(OBJS) $(CMD_SRCS) $(GEN_CMDS)
	@echo 'Building target: $@'
	@echo 'Invoking: ARM Linker'
	"D:/TI/ccsv6/tools/compiler/arm_5.1.6/bin/armcl" -mv7M4 --code_state=16 --float_support=FPv4SPD16 --abi=eabi -me -g --gcc --define=ccs="ccs" --define=TARGET_IS_BLIZZARD_RB1 --define=CLASS_IS_TM4C123GH6PM --define=PART_TM4C123GH6PM --display_error_number --diag_warning=225 --diag_wrap=off -z -m"TivaControlBoard.map" --heap_size=0 --stack_size=512 -i"D:/TI/ccsv6/tools/compiler/arm_5.1.6/lib" -i"D:/TI/ccsv6/tools/compiler/arm_5.1.6/include" --reread_libs --warn_sections --display_error_number --diag_wrap=off --xml_link_info="TivaControlBoard_linkInfo.xml" --rom_model -o "TivaControlBoard.out" $(ORDERED_OBJS)
	@echo 'Finished building target: $@'
	@echo ' '

TivaControlBoard.hex: $(EXE_OUTPUTS)
	@echo 'Invoking: ARM Hex Utility'
	"D:/TI/ccsv6/tools/compiler/arm_5.1.6/bin/armhex"  -o "TivaControlBoard.hex" $(EXE_OUTPUTS__QUOTED)
	@echo 'Finished building: $@'
	@echo ' '

# Other Targets
clean:
	-$(RM) $(EXE_OUTPUTS__QUOTED)$(BIN_OUTPUTS__QUOTED)
	-$(RM) "ControlBoard.pp" "main.pp" "tm4c123gh6pm_startup_ccs.pp" "usb_swarm_control_structs.pp" "libnrf24l01\src\TM4C123_nRF24L01.pp" "libnrf24l01\src\nRF24L01.pp" 
	-$(RM) "ControlBoard.obj" "main.obj" "tm4c123gh6pm_startup_ccs.obj" "usb_swarm_control_structs.obj" "libnrf24l01\src\TM4C123_nRF24L01.obj" "libnrf24l01\src\nRF24L01.obj" 
	-@echo 'Finished clean'
	-@echo ' '

.PHONY: all clean dependents
.SECONDARY:

-include ../makefile.targets

