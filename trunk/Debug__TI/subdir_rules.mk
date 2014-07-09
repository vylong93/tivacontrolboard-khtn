################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Each subdirectory must supply rules for building sources it contributes
CustomTivaDrivers.obj: ../CustomTivaDrivers.c $(GEN_OPTS) $(GEN_HDRS)
	@echo 'Building file: $<'
	@echo 'Invoking: ARM Compiler'
	"c:/ti/ccsv6/tools/compiler/arm_5.1.6/bin/armcl" -mv7M4 --code_state=16 --float_support=FPv4SPD16 --abi=eabi -me -Ooff --include_path="D:/Portable/Programs/TivaDriver" --include_path="D:/Software Projects/TortoiseSVN/usbControlBoard" --include_path="c:/ti/ccsv6/tools/compiler/arm_5.1.6/include" -g --gcc --define=ccs="ccs" --define=PART_TM4C123GH6PM --define=CLASS_IS_TM4C123GH6PM --define=TARGET_IS_BLIZZARD_RB1 --display_error_number --diag_warning=225 --diag_wrap=off --preproc_with_compile --preproc_dependency="CustomTivaDrivers.pp" $(GEN_OPTS__FLAG) "$<"
	@echo 'Finished building: $<'
	@echo ' '

distanceSensing.obj: ../distanceSensing.c $(GEN_OPTS) $(GEN_HDRS)
	@echo 'Building file: $<'
	@echo 'Invoking: ARM Compiler'
	"c:/ti/ccsv6/tools/compiler/arm_5.1.6/bin/armcl" -mv7M4 --code_state=16 --float_support=FPv4SPD16 --abi=eabi -me -Ooff --include_path="D:/Portable/Programs/TivaDriver" --include_path="D:/Software Projects/TortoiseSVN/usbControlBoard" --include_path="c:/ti/ccsv6/tools/compiler/arm_5.1.6/include" -g --gcc --define=ccs="ccs" --define=PART_TM4C123GH6PM --define=CLASS_IS_TM4C123GH6PM --define=TARGET_IS_BLIZZARD_RB1 --display_error_number --diag_warning=225 --diag_wrap=off --preproc_with_compile --preproc_dependency="distanceSensing.pp" $(GEN_OPTS__FLAG) "$<"
	@echo 'Finished building: $<'
	@echo ' '

main.obj: ../main.c $(GEN_OPTS) $(GEN_HDRS)
	@echo 'Building file: $<'
	@echo 'Invoking: ARM Compiler'
	"c:/ti/ccsv6/tools/compiler/arm_5.1.6/bin/armcl" -mv7M4 --code_state=16 --float_support=FPv4SPD16 --abi=eabi -me -Ooff --include_path="D:/Portable/Programs/TivaDriver" --include_path="D:/Software Projects/TortoiseSVN/usbControlBoard" --include_path="c:/ti/ccsv6/tools/compiler/arm_5.1.6/include" -g --gcc --define=ccs="ccs" --define=PART_TM4C123GH6PM --define=CLASS_IS_TM4C123GH6PM --define=TARGET_IS_BLIZZARD_RB1 --display_error_number --diag_warning=225 --diag_wrap=off --preproc_with_compile --preproc_dependency="main.pp" $(GEN_OPTS__FLAG) "$<"
	@echo 'Finished building: $<'
	@echo ' '

tm4c123gh6pm_startup_ccs.obj: ../tm4c123gh6pm_startup_ccs.c $(GEN_OPTS) $(GEN_HDRS)
	@echo 'Building file: $<'
	@echo 'Invoking: ARM Compiler'
	"c:/ti/ccsv6/tools/compiler/arm_5.1.6/bin/armcl" -mv7M4 --code_state=16 --float_support=FPv4SPD16 --abi=eabi -me -Ooff --include_path="D:/Portable/Programs/TivaDriver" --include_path="D:/Software Projects/TortoiseSVN/usbControlBoard" --include_path="c:/ti/ccsv6/tools/compiler/arm_5.1.6/include" -g --gcc --define=ccs="ccs" --define=PART_TM4C123GH6PM --define=CLASS_IS_TM4C123GH6PM --define=TARGET_IS_BLIZZARD_RB1 --display_error_number --diag_warning=225 --diag_wrap=off --preproc_with_compile --preproc_dependency="tm4c123gh6pm_startup_ccs.pp" $(GEN_OPTS__FLAG) "$<"
	@echo 'Finished building: $<'
	@echo ' '

usb_swarm_control_structs.obj: ../usb_swarm_control_structs.c $(GEN_OPTS) $(GEN_HDRS)
	@echo 'Building file: $<'
	@echo 'Invoking: ARM Compiler'
	"c:/ti/ccsv6/tools/compiler/arm_5.1.6/bin/armcl" -mv7M4 --code_state=16 --float_support=FPv4SPD16 --abi=eabi -me -Ooff --include_path="D:/Portable/Programs/TivaDriver" --include_path="D:/Software Projects/TortoiseSVN/usbControlBoard" --include_path="c:/ti/ccsv6/tools/compiler/arm_5.1.6/include" -g --gcc --define=ccs="ccs" --define=PART_TM4C123GH6PM --define=CLASS_IS_TM4C123GH6PM --define=TARGET_IS_BLIZZARD_RB1 --display_error_number --diag_warning=225 --diag_wrap=off --preproc_with_compile --preproc_dependency="usb_swarm_control_structs.pp" $(GEN_OPTS__FLAG) "$<"
	@echo 'Finished building: $<'
	@echo ' '


