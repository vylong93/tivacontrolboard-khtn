################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Each subdirectory must supply rules for building sources it contributes
libnrf24l01/src/TM4C123_nRF24L01.obj: ../libnrf24l01/src/TM4C123_nRF24L01.c $(GEN_OPTS) $(GEN_HDRS)
	@echo 'Building file: $<'
	@echo 'Invoking: ARM Compiler'
	"D:/ti/ccsv6/tools/compiler/arm_5.1.6/bin/armcl" -mv7M4 --code_state=16 --float_support=FPv4SPD16 --abi=eabi -me --include_path="D:/ti/ccsv6/tools/compiler/arm_5.1.6/include" --include_path="E:/00 SwarmRobot/Workspace/SVN/ControlBoard/branches/ControlBoard_nRF24L01" --include_path="D:/Program Files/TI/TivaWare_C_Series-2.1.0.12573" -g --gcc --define=ccs="ccs" --define=TARGET_IS_BLIZZARD_RB1 --define=CLASS_IS_TM4C123GH6PM --define=PART_TM4C123GH6PM --display_error_number --diag_warning=225 --diag_wrap=off --preproc_with_compile --preproc_dependency="libnrf24l01/src/TM4C123_nRF24L01.pp" --obj_directory="libnrf24l01/src" $(GEN_OPTS__FLAG) "$<"
	@echo 'Finished building: $<'
	@echo ' '

libnrf24l01/src/nRF24L01.obj: ../libnrf24l01/src/nRF24L01.c $(GEN_OPTS) $(GEN_HDRS)
	@echo 'Building file: $<'
	@echo 'Invoking: ARM Compiler'
	"D:/ti/ccsv6/tools/compiler/arm_5.1.6/bin/armcl" -mv7M4 --code_state=16 --float_support=FPv4SPD16 --abi=eabi -me --include_path="D:/ti/ccsv6/tools/compiler/arm_5.1.6/include" --include_path="E:/00 SwarmRobot/Workspace/SVN/ControlBoard/branches/ControlBoard_nRF24L01" --include_path="D:/Program Files/TI/TivaWare_C_Series-2.1.0.12573" -g --gcc --define=ccs="ccs" --define=TARGET_IS_BLIZZARD_RB1 --define=CLASS_IS_TM4C123GH6PM --define=PART_TM4C123GH6PM --display_error_number --diag_warning=225 --diag_wrap=off --preproc_with_compile --preproc_dependency="libnrf24l01/src/nRF24L01.pp" --obj_directory="libnrf24l01/src" $(GEN_OPTS__FLAG) "$<"
	@echo 'Finished building: $<'
	@echo ' '


