################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Each subdirectory must supply rules for building sources it contributes
ControlBoard.obj: ../ControlBoard.c $(GEN_OPTS) $(GEN_HDRS)
	@echo 'Building file: $<'
	@echo 'Invoking: ARM Compiler'
	"D:/TI/ccsv6/tools/compiler/arm_5.1.6/bin/armcl" -mv7M4 --code_state=16 --float_support=FPv4SPD16 --abi=eabi -me --include_path="D:/TI/ccsv6/tools/compiler/arm_5.1.6/include" --include_path="E:/00 SwarmRobot/Workspace/TivaControlBoard" --include_path="D:/Program Files/TI/TivaWare_C_Series-2.1.0.12573" -g --gcc --define=ccs="ccs" --define=TARGET_IS_BLIZZARD_RB1 --define=CLASS_IS_TM4C123GH6PM --define=PART_TM4C123GH6PM --display_error_number --diag_warning=225 --diag_wrap=off --preproc_with_compile --preproc_dependency="ControlBoard.pp" $(GEN_OPTS__FLAG) "$<"
	@echo 'Finished building: $<'
	@echo ' '

main.obj: ../main.c $(GEN_OPTS) $(GEN_HDRS)
	@echo 'Building file: $<'
	@echo 'Invoking: ARM Compiler'
	"D:/TI/ccsv6/tools/compiler/arm_5.1.6/bin/armcl" -mv7M4 --code_state=16 --float_support=FPv4SPD16 --abi=eabi -me --include_path="D:/TI/ccsv6/tools/compiler/arm_5.1.6/include" --include_path="E:/00 SwarmRobot/Workspace/TivaControlBoard" --include_path="D:/Program Files/TI/TivaWare_C_Series-2.1.0.12573" -g --gcc --define=ccs="ccs" --define=TARGET_IS_BLIZZARD_RB1 --define=CLASS_IS_TM4C123GH6PM --define=PART_TM4C123GH6PM --display_error_number --diag_warning=225 --diag_wrap=off --preproc_with_compile --preproc_dependency="main.pp" $(GEN_OPTS__FLAG) "$<"
	@echo 'Finished building: $<'
	@echo ' '

tm4c123gh6pm_startup_ccs.obj: ../tm4c123gh6pm_startup_ccs.c $(GEN_OPTS) $(GEN_HDRS)
	@echo 'Building file: $<'
	@echo 'Invoking: ARM Compiler'
	"D:/TI/ccsv6/tools/compiler/arm_5.1.6/bin/armcl" -mv7M4 --code_state=16 --float_support=FPv4SPD16 --abi=eabi -me --include_path="D:/TI/ccsv6/tools/compiler/arm_5.1.6/include" --include_path="E:/00 SwarmRobot/Workspace/TivaControlBoard" --include_path="D:/Program Files/TI/TivaWare_C_Series-2.1.0.12573" -g --gcc --define=ccs="ccs" --define=TARGET_IS_BLIZZARD_RB1 --define=CLASS_IS_TM4C123GH6PM --define=PART_TM4C123GH6PM --display_error_number --diag_warning=225 --diag_wrap=off --preproc_with_compile --preproc_dependency="tm4c123gh6pm_startup_ccs.pp" $(GEN_OPTS__FLAG) "$<"
	@echo 'Finished building: $<'
	@echo ' '

usb_swarm_control_structs.obj: ../usb_swarm_control_structs.c $(GEN_OPTS) $(GEN_HDRS)
	@echo 'Building file: $<'
	@echo 'Invoking: ARM Compiler'
	"D:/TI/ccsv6/tools/compiler/arm_5.1.6/bin/armcl" -mv7M4 --code_state=16 --float_support=FPv4SPD16 --abi=eabi -me --include_path="D:/TI/ccsv6/tools/compiler/arm_5.1.6/include" --include_path="E:/00 SwarmRobot/Workspace/TivaControlBoard" --include_path="D:/Program Files/TI/TivaWare_C_Series-2.1.0.12573" -g --gcc --define=ccs="ccs" --define=TARGET_IS_BLIZZARD_RB1 --define=CLASS_IS_TM4C123GH6PM --define=PART_TM4C123GH6PM --display_error_number --diag_warning=225 --diag_wrap=off --preproc_with_compile --preproc_dependency="usb_swarm_control_structs.pp" $(GEN_OPTS__FLAG) "$<"
	@echo 'Finished building: $<'
	@echo ' '


