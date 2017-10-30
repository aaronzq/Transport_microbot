################################################################################
# Automatically-generated file. Do not edit!
################################################################################

SHELL = cmd.exe

CG_TOOL_ROOT := E:/ti/ccsv6/tools/compiler/arm_15.12.3.LTS

GEN_OPTS__FLAG := --cmd_file="configPkg/compiler.opt"
GEN_CMDS__FLAG := -l"configPkg/linker.cmd"

ORDERED_OBJS += \
"./linemouse.obj" \
"./linemouse_chassis.obj" \
"./Flash/Flash.obj" \
"./Kalman/Kalman1Var.obj" \
"./Pid/Pid.obj" \
"./Queue/Queue.obj" \
"./TskAction/TskAction.obj" \
"./TskIr/TskIr.obj" \
"./TskMotor/Imu.obj" \
"./TskMotor/MotorPwm.obj" \
"./TskMotor/TskMotor.obj" \
"./TskMotor/WheelEnc.obj" \
"./TskSolver/TskSolver.obj" \
"./TskTop/DbgUart.obj" \
"./TskTop/TskTop.obj" \
"../linemouse_chassis.cmd" \
$(GEN_CMDS__FLAG) \
-l"C:/ti/tirex-content/tirtos_tivac_2_16_00_08/products/TivaWare_C_Series-2.1.1.71b/grlib/ccs/Debug/grlib.lib" \
-l"C:/ti/tirex-content/tirtos_tivac_2_16_00_08/products/TivaWare_C_Series-2.1.1.71b/usblib/ccs/Debug/usblib.lib" \
-l"C:/ti/tirex-content/tirtos_tivac_2_16_00_08/products/TivaWare_C_Series-2.1.1.71b/driverlib/ccs/Debug/driverlib.lib" \
-llibc.a \

-include ../makefile.init

RM := DEL /F
RMDIR := RMDIR /S/Q

# All of the sources participating in the build are defined here
-include sources.mk
-include subdir_vars.mk
-include Flash/subdir_vars.mk
-include Kalman/subdir_vars.mk
-include Pid/subdir_vars.mk
-include Queue/subdir_vars.mk
-include TskAction/subdir_vars.mk
-include TskIr/subdir_vars.mk
-include TskMotor/subdir_vars.mk
-include TskSolver/subdir_vars.mk
-include TskTop/subdir_vars.mk
-include subdir_rules.mk
-include Flash/subdir_rules.mk
-include Kalman/subdir_rules.mk
-include Pid/subdir_rules.mk
-include Queue/subdir_rules.mk
-include TskAction/subdir_rules.mk
-include TskIr/subdir_rules.mk
-include TskMotor/subdir_rules.mk
-include TskSolver/subdir_rules.mk
-include TskTop/subdir_rules.mk
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
ifneq ($(strip $(SV7A_DEPS)),)
-include $(SV7A_DEPS)
endif
ifneq ($(strip $(S55_DEPS)),)
-include $(S55_DEPS)
endif
ifneq ($(strip $(C67_DEPS)),)
-include $(C67_DEPS)
endif
ifneq ($(strip $(CLA_DEPS)),)
-include $(CLA_DEPS)
endif
ifneq ($(strip $(C??_DEPS)),)
-include $(C??_DEPS)
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
ifneq ($(strip $(INO_DEPS)),)
-include $(INO_DEPS)
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
ifneq ($(strip $(PDE_DEPS)),)
-include $(PDE_DEPS)
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
LineMouse_KS.out \

EXE_OUTPUTS__QUOTED += \
"LineMouse_KS.out" \

BIN_OUTPUTS += \
LineMouse_KS.bin \

BIN_OUTPUTS__QUOTED += \
"LineMouse_KS.bin" \


# All Target
all: LineMouse_KS.out

# Tool invocations
LineMouse_KS.out: $(OBJS) $(CMD_SRCS) $(GEN_CMDS)
	@echo 'Building target: $@'
	@echo 'Invoking: ARM Linker'
	"E:/ti/ccsv6/tools/compiler/arm_15.12.3.LTS/bin/armcl" -mv7M4 --code_state=16 --float_support=FPv4SPD16 -me -g --gcc --define=ccs="ccs" --define=PART_TM4C123GH6PM --define=ccs --define=TIVAWARE --diag_wrap=off --diag_warning=225 --diag_warning=255 --display_error_number --gen_func_subsections=on --abi=eabi -z -m"LineMouse_KS.map" --stack_size=512 --heap_size=0 -i"E:/ti/ccsv6/tools/compiler/arm_15.12.3.LTS/lib" -i"E:/ti/ccsv6/tools/compiler/arm_15.12.3.LTS/include" --reread_libs --diag_wrap=off --warn_sections --display_error_number --xml_link_info="LineMouse_KS_linkInfo.xml" --rom_model -o "LineMouse_KS.out" $(ORDERED_OBJS)
	@echo 'Finished building target: $@'
	@echo ' '
	@$(MAKE) --no-print-directory post-build

LineMouse_KS.bin: $(EXE_OUTPUTS)
	@echo 'Invoking: ARM Hex Utility'
	"E:/ti/ccsv6/tools/compiler/arm_15.12.3.LTS/bin/armhex"  -o "LineMouse_KS.bin" $(EXE_OUTPUTS__QUOTED)
	@echo 'Finished building: $@'
	@echo ' '
	@$(MAKE) --no-print-directory post-build

# Other Targets
clean:
	-$(RM) $(GEN_MISC_FILES__QUOTED)$(EXE_OUTPUTS__QUOTED)$(GEN_FILES__QUOTED)$(BIN_OUTPUTS__QUOTED)$(GEN_OPTS__QUOTED)$(GEN_CMDS__QUOTED)
	-$(RM) "Kalman\Kalman1Var.d" 
	-$(RMDIR) $(GEN_MISC_DIRS__QUOTED)
	-$(RM) "linemouse_chassis.d" 
	-$(RM) "linemouse.obj" "linemouse_chassis.obj" "Flash\Flash.obj" "Kalman\Kalman1Var.obj" "Pid\Pid.obj" "Queue\Queue.obj" "TskAction\TskAction.obj" "TskIr\TskIr.obj" "TskMotor\Imu.obj" "TskMotor\MotorPwm.obj" "TskMotor\TskMotor.obj" "TskMotor\WheelEnc.obj" "TskSolver\TskSolver.obj" "TskTop\DbgUart.obj" "TskTop\TskTop.obj" 
	-$(RM) "linemouse.d" "Flash\Flash.d" "Pid\Pid.d" "Queue\Queue.d" "TskAction\TskAction.d" "TskIr\TskIr.d" "TskMotor\Imu.d" "TskMotor\MotorPwm.d" "TskMotor\TskMotor.d" "TskMotor\WheelEnc.d" "TskSolver\TskSolver.d" "TskTop\DbgUart.d" "TskTop\TskTop.d" 
	-@echo 'Finished clean'
	-@echo ' '

post-build:
	-"E:/ti/ccsv6/utils/tiobj2bin/tiobj2bin" "LineMouse_KS.out" "LineMouse_KS.bin" "E:/ti/ccsv6/tools/compiler/arm_15.12.3.LTS/bin/armofd" "E:/ti/ccsv6/tools/compiler/arm_15.12.3.LTS/bin/armhex" "E:/ti/ccsv6/utils/tiobj2bin/mkhex4bin"
	-@echo ' '

.PHONY: all clean dependents
.SECONDARY: post-build

-include ../makefile.targets

