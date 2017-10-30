################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Each subdirectory must supply rules for building sources it contributes
build-1440418911: ../empty.cfg
	@echo 'Building file: $<'
	@echo 'Invoking: XDCtools'
	"C:/ti/tirex-content/xdctools_3_32_00_06_core/xs" --xdcpath="C:/ti/tirex-content/tirtos_tivac_2_16_00_08/packages;C:/ti/tirex-content/tirtos_tivac_2_16_00_08/products/tidrivers_tivac_2_16_00_08/packages;C:/ti/tirex-content/tirtos_tivac_2_16_00_08/products/bios_6_45_01_29/packages;C:/ti/tirex-content/tirtos_tivac_2_16_00_08/products/ndk_2_25_00_09/packages;C:/ti/tirex-content/tirtos_tivac_2_16_00_08/products/uia_2_00_05_50/packages;C:/ti/tirex-content/tirtos_tivac_2_16_00_08/products/ns_1_11_00_10/packages;E:/ti/ccsv6/ccs_base;" xdc.tools.configuro -o configPkg -t ti.targets.arm.elf.M4F -p ti.platforms.tiva:TM4C123GH6PM -r release -c "E:/ti/ccsv6/tools/compiler/arm_15.12.3.LTS" --compileOptions "-mv7M4 --code_state=16 --float_support=FPv4SPD16 -me --include_path=\"C:/Users/AaronW/workspace_v6_2/LineMouse_KS\" --include_path=\"C:/ti/tirex-content/tirtos_tivac_2_16_00_08/products/TivaWare_C_Series-2.1.1.71b\" --include_path=\"C:/ti/tirex-content/tirtos_tivac_2_16_00_08/products/bios_6_45_02_31/packages/ti/sysbios/posix\" --include_path=\"E:/ti/ccsv6/tools/compiler/arm_15.12.3.LTS/include\" -g --gcc --define=ccs=\"ccs\" --define=PART_TM4C123GH6PM --define=ccs --define=TIVAWARE --diag_wrap=off --diag_warning=225 --diag_warning=255 --display_error_number --gen_func_subsections=on --abi=eabi  " "$<"
	@echo 'Finished building: $<'
	@echo ' '

configPkg/linker.cmd: build-1440418911
configPkg/compiler.opt: build-1440418911
configPkg/: build-1440418911

linemouse.obj: ../linemouse.cc $(GEN_OPTS) | $(GEN_HDRS)
	@echo 'Building file: $<'
	@echo 'Invoking: ARM Compiler'
	"E:/ti/ccsv6/tools/compiler/arm_15.12.3.LTS/bin/armcl" -mv7M4 --code_state=16 --float_support=FPv4SPD16 -me --include_path="C:/Users/AaronW/workspace_v6_2/LineMouse_KS" --include_path="C:/ti/tirex-content/tirtos_tivac_2_16_00_08/products/TivaWare_C_Series-2.1.1.71b" --include_path="C:/ti/tirex-content/tirtos_tivac_2_16_00_08/products/bios_6_45_02_31/packages/ti/sysbios/posix" --include_path="E:/ti/ccsv6/tools/compiler/arm_15.12.3.LTS/include" -g --gcc --define=ccs="ccs" --define=PART_TM4C123GH6PM --define=ccs --define=TIVAWARE --diag_wrap=off --diag_warning=225 --diag_warning=255 --display_error_number --gen_func_subsections=on --abi=eabi --preproc_with_compile --preproc_dependency="linemouse.d" $(GEN_OPTS__FLAG) "$<"
	@echo 'Finished building: $<'
	@echo ' '

linemouse_chassis.obj: ../linemouse_chassis.c $(GEN_OPTS) | $(GEN_HDRS)
	@echo 'Building file: $<'
	@echo 'Invoking: ARM Compiler'
	"E:/ti/ccsv6/tools/compiler/arm_15.12.3.LTS/bin/armcl" -mv7M4 --code_state=16 --float_support=FPv4SPD16 -me --include_path="C:/Users/AaronW/workspace_v6_2/LineMouse_KS" --include_path="C:/ti/tirex-content/tirtos_tivac_2_16_00_08/products/TivaWare_C_Series-2.1.1.71b" --include_path="C:/ti/tirex-content/tirtos_tivac_2_16_00_08/products/bios_6_45_02_31/packages/ti/sysbios/posix" --include_path="E:/ti/ccsv6/tools/compiler/arm_15.12.3.LTS/include" -g --gcc --define=ccs="ccs" --define=PART_TM4C123GH6PM --define=ccs --define=TIVAWARE --diag_wrap=off --diag_warning=225 --diag_warning=255 --display_error_number --gen_func_subsections=on --abi=eabi --preproc_with_compile --preproc_dependency="linemouse_chassis.d" $(GEN_OPTS__FLAG) "$<"
	@echo 'Finished building: $<'
	@echo ' '


