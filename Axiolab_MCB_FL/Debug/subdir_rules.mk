################################################################################
# Automatically-generated file. Do not edit!
################################################################################

SHELL = cmd.exe

# Each subdirectory must supply rules for building sources it contributes
F2806x_CodeStartBranch_ebss_init.obj: ../F2806x_CodeStartBranch_ebss_init.asm $(GEN_OPTS) | $(GEN_HDRS)
	@echo 'Building file: $<'
	@echo 'Invoking: C2000 Compiler'
	"C:/ti/ccsv7/tools/compiler/ti-cgt-c2000_16.9.1.LTS/bin/cl2000" -v28 -ml -mt --cla_support=cla0 --float_support=fpu32 --vcu_support=vcu0 --opt_for_speed=3 --include_path="C:/ti/ccsv7/tools/compiler/ti-cgt-c2000_16.9.1.LTS/include" --include_path="C:/ti/controlSUITE/device_support/f2806x/v151/MWare" --include_path="C:/03_workspace/mcb_app/Axiolab_MCB_FL/lib" --include_path="C:/03_workspace/mcb_app/Axiolab_MCB_FL/include" --advice:performance=all --define=DEBUG --define=ccs_c2k -g --diag_warning=225 --diag_wrap=off --display_error_number --preproc_with_compile --preproc_dependency="F2806x_CodeStartBranch_ebss_init.d" $(GEN_OPTS__FLAG) "$<"
	@echo 'Finished building: $<'
	@echo ' '

F2806x_usDelay.obj: ../F2806x_usDelay.asm $(GEN_OPTS) | $(GEN_HDRS)
	@echo 'Building file: $<'
	@echo 'Invoking: C2000 Compiler'
	"C:/ti/ccsv7/tools/compiler/ti-cgt-c2000_16.9.1.LTS/bin/cl2000" -v28 -ml -mt --cla_support=cla0 --float_support=fpu32 --vcu_support=vcu0 --opt_for_speed=3 --include_path="C:/ti/ccsv7/tools/compiler/ti-cgt-c2000_16.9.1.LTS/include" --include_path="C:/ti/controlSUITE/device_support/f2806x/v151/MWare" --include_path="C:/03_workspace/mcb_app/Axiolab_MCB_FL/lib" --include_path="C:/03_workspace/mcb_app/Axiolab_MCB_FL/include" --advice:performance=all --define=DEBUG --define=ccs_c2k -g --diag_warning=225 --diag_wrap=off --display_error_number --preproc_with_compile --preproc_dependency="F2806x_usDelay.d" $(GEN_OPTS__FLAG) "$<"
	@echo 'Finished building: $<'
	@echo ' '

bl_entrytable.obj: ../bl_entrytable.asm $(GEN_OPTS) | $(GEN_HDRS)
	@echo 'Building file: $<'
	@echo 'Invoking: C2000 Compiler'
	"C:/ti/ccsv7/tools/compiler/ti-cgt-c2000_16.9.1.LTS/bin/cl2000" -v28 -ml -mt --cla_support=cla0 --float_support=fpu32 --vcu_support=vcu0 --opt_for_speed=3 --include_path="C:/ti/ccsv7/tools/compiler/ti-cgt-c2000_16.9.1.LTS/include" --include_path="C:/ti/controlSUITE/device_support/f2806x/v151/MWare" --include_path="C:/03_workspace/mcb_app/Axiolab_MCB_FL/lib" --include_path="C:/03_workspace/mcb_app/Axiolab_MCB_FL/include" --advice:performance=all --define=DEBUG --define=ccs_c2k -g --diag_warning=225 --diag_wrap=off --display_error_number --preproc_with_compile --preproc_dependency="bl_entrytable.d" $(GEN_OPTS__FLAG) "$<"
	@echo 'Finished building: $<'
	@echo ' '


