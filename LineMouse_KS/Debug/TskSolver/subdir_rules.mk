################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Each subdirectory must supply rules for building sources it contributes
TskSolver/TskSolver.obj: ../TskSolver/TskSolver.cc $(GEN_OPTS) | $(GEN_HDRS)
	@echo 'Building file: $<'
	@echo 'Invoking: ARM Compiler'
	"E:/ti/ccsv6/tools/compiler/arm_15.12.3.LTS/bin/armcl" -mv7M4 --code_state=16 --float_support=FPv4SPD16 -me --include_path="C:/Users/AaronW/workspace_v6_2/LineMouse_KS" --include_path="C:/ti/tirex-content/tirtos_tivac_2_16_00_08/products/TivaWare_C_Series-2.1.1.71b" --include_path="C:/ti/tirex-content/tirtos_tivac_2_16_00_08/products/bios_6_45_02_31/packages/ti/sysbios/posix" --include_path="E:/ti/ccsv6/tools/compiler/arm_15.12.3.LTS/include" -g --gcc --define=ccs="ccs" --define=PART_TM4C123GH6PM --define=ccs --define=TIVAWARE --diag_wrap=off --diag_warning=225 --diag_warning=255 --display_error_number --gen_func_subsections=on --abi=eabi --preproc_with_compile --preproc_dependency="TskSolver/TskSolver.d" --obj_directory="TskSolver" $(GEN_OPTS__FLAG) "$<"
	@echo 'Finished building: $<'
	@echo ' '


