################################################################################
# Automatically-generated file. Do not edit!
################################################################################

SHELL = cmd.exe

CG_TOOL_ROOT := C:/ti/ccsv7/tools/compiler/ti-cgt-c2000_16.9.1.LTS

GEN_OPTS__FLAG := 
GEN_CMDS__FLAG := 

ORDERED_OBJS += \
"./F2806x_CodeStartBranch_ebss_init.obj" \
"./F2806x_usDelay.obj" \
"./bl_entrytable.obj" \
"./lib/F2806x_Adc.obj" \
"./lib/F2806x_CpuTimers.obj" \
"./lib/F2806x_DefaultIsr.obj" \
"./lib/F2806x_GlobalVariableDefs.obj" \
"./lib/F2806x_PieCtrl.obj" \
"./lib/F2806x_PieVect.obj" \
"./lib/sysctl.obj" \
"./lib/uartstdio.obj" \
"./lib/usb.obj" \
"./lib/usb_hal.obj" \
"./src/Adc.obj" \
"./src/CAN29.obj" \
"./src/CmdHandler.obj" \
"./src/DescriptorTask.obj" \
"./src/Device_Init.obj" \
"./src/ECAN.obj" \
"./src/EPWM.obj" \
"./src/EQEP.obj" \
"./src/I2C.obj" \
"./src/LightManager.obj" \
"./src/MainControl.obj" \
"./src/Parcenter.obj" \
"./src/SCI.obj" \
"./src/SPI.obj" \
"./src/TLShutter.obj" \
"./src/TaskMgr.obj" \
"./src/Timer.obj" \
"./src/USB.obj" \
"./src/XINT.obj" \
"./src/app.obj" \
"./src/eeprom.obj" \
"./src/eeprom_mmap.obj" \
"./src/lightcontrol.obj" \
"./src/monitoring.obj" \
"./src/nosepiece.obj" \
"./src/reflector.obj" \
"./src/rltl.obj" \
"./src/usb_bulk_structs.obj" \
"../F28069_MCB_app.cmd" \
"../F2806x_Headers_nonBIOS.cmd" \
"C:/ti/controlSUITE/device_support/f2806x/v151/MWare/driverlib/Debug/driverlib.lib" \
"C:/ti/controlSUITE/device_support/f2806x/v151/lib/usblib.lib" \
$(GEN_CMDS__FLAG) \
-lrts2800_fpu32.lib \

-include ../makefile.init

RM := DEL /F
RMDIR := RMDIR /S/Q

# All of the sources participating in the build are defined here
-include sources.mk
-include subdir_vars.mk
-include lib/subdir_vars.mk
-include src/subdir_vars.mk
-include subdir_rules.mk
-include lib/subdir_rules.mk
-include src/subdir_rules.mk
-include objects.mk

ifneq ($(MAKECMDGOALS),clean)
ifneq ($(strip $(C55_DEPS)),)
-include $(C55_DEPS)
endif
ifneq ($(strip $(C_UPPER_DEPS)),)
-include $(C_UPPER_DEPS)
endif
ifneq ($(strip $(S67_DEPS)),)
-include $(S67_DEPS)
endif
ifneq ($(strip $(S62_DEPS)),)
-include $(S62_DEPS)
endif
ifneq ($(strip $(S_DEPS)),)
-include $(S_DEPS)
endif
ifneq ($(strip $(OPT_DEPS)),)
-include $(OPT_DEPS)
endif
ifneq ($(strip $(C??_DEPS)),)
-include $(C??_DEPS)
endif
ifneq ($(strip $(ASM_UPPER_DEPS)),)
-include $(ASM_UPPER_DEPS)
endif
ifneq ($(strip $(S??_DEPS)),)
-include $(S??_DEPS)
endif
ifneq ($(strip $(C64_DEPS)),)
-include $(C64_DEPS)
endif
ifneq ($(strip $(CXX_DEPS)),)
-include $(CXX_DEPS)
endif
ifneq ($(strip $(S64_DEPS)),)
-include $(S64_DEPS)
endif
ifneq ($(strip $(INO_DEPS)),)
-include $(INO_DEPS)
endif
ifneq ($(strip $(CLA_DEPS)),)
-include $(CLA_DEPS)
endif
ifneq ($(strip $(S55_DEPS)),)
-include $(S55_DEPS)
endif
ifneq ($(strip $(SV7A_DEPS)),)
-include $(SV7A_DEPS)
endif
ifneq ($(strip $(C62_DEPS)),)
-include $(C62_DEPS)
endif
ifneq ($(strip $(C67_DEPS)),)
-include $(C67_DEPS)
endif
ifneq ($(strip $(PDE_DEPS)),)
-include $(PDE_DEPS)
endif
ifneq ($(strip $(K_DEPS)),)
-include $(K_DEPS)
endif
ifneq ($(strip $(C_DEPS)),)
-include $(C_DEPS)
endif
ifneq ($(strip $(CC_DEPS)),)
-include $(CC_DEPS)
endif
ifneq ($(strip $(C++_DEPS)),)
-include $(C++_DEPS)
endif
ifneq ($(strip $(C43_DEPS)),)
-include $(C43_DEPS)
endif
ifneq ($(strip $(S43_DEPS)),)
-include $(S43_DEPS)
endif
ifneq ($(strip $(ASM_DEPS)),)
-include $(ASM_DEPS)
endif
ifneq ($(strip $(S_UPPER_DEPS)),)
-include $(S_UPPER_DEPS)
endif
ifneq ($(strip $(CPP_DEPS)),)
-include $(CPP_DEPS)
endif
ifneq ($(strip $(SA_DEPS)),)
-include $(SA_DEPS)
endif
endif

-include ../makefile.defs

# Add inputs and outputs from these tool invocations to the build variables 
EXE_OUTPUTS += \
Axiolab_MCB_FL.out \

EXE_OUTPUTS__QUOTED += \
"Axiolab_MCB_FL.out" \

BIN_OUTPUTS += \
Axiolab_MCB_FL.hex \

BIN_OUTPUTS__QUOTED += \
"Axiolab_MCB_FL.hex" \


# All Target
all: Axiolab_MCB_FL.out

# Tool invocations
Axiolab_MCB_FL.out: $(OBJS) $(CMD_SRCS) $(LIB_SRCS) $(GEN_CMDS)
	@echo 'Building target: $@'
	@echo 'Invoking: C2000 Linker'
	"C:/ti/ccsv7/tools/compiler/ti-cgt-c2000_16.9.1.LTS/bin/cl2000" -v28 -ml -mt --cla_support=cla0 --float_support=fpu32 --vcu_support=vcu0 --opt_for_speed=3 --advice:performance=all --define=DEBUG --define=ccs_c2k -g --diag_warning=225 --diag_wrap=off --display_error_number -z -m"Axiolab_MCB_FL.map" --heap_size=0x1000 --stack_size=0x500 --warn_sections -i"C:/ti/ccsv7/tools/compiler/ti-cgt-c2000_16.9.1.LTS/lib" -i"C:/ti/ccsv7/tools/compiler/ti-cgt-c2000_16.9.1.LTS/include" --reread_libs --diag_wrap=off --display_error_number --xml_link_info="Axiolab_MCB_FL_linkInfo.xml" --rom_model -o "Axiolab_MCB_FL.out" $(ORDERED_OBJS)
	@echo 'Finished building target: $@'
	@echo ' '
	@$(MAKE) --no-print-directory post-build

Axiolab_MCB_FL.hex: $(EXE_OUTPUTS)
	@echo 'Invoking: C2000 Hex Utility'
	"C:/ti/ccsv7/tools/compiler/ti-cgt-c2000_16.9.1.LTS/bin/hex2000"  -o "Axiolab_MCB_FL.hex" $(EXE_OUTPUTS__QUOTED)
	@echo 'Finished building: $@'
	@echo ' '
	@$(MAKE) --no-print-directory post-build

# Other Targets
clean:
	-$(RM) $(BIN_OUTPUTS__QUOTED)$(EXE_OUTPUTS__QUOTED)
	-$(RM) "F2806x_CodeStartBranch_ebss_init.obj" "F2806x_usDelay.obj" "bl_entrytable.obj" "lib\F2806x_Adc.obj" "lib\F2806x_CpuTimers.obj" "lib\F2806x_DefaultIsr.obj" "lib\F2806x_GlobalVariableDefs.obj" "lib\F2806x_PieCtrl.obj" "lib\F2806x_PieVect.obj" "lib\sysctl.obj" "lib\uartstdio.obj" "lib\usb.obj" "lib\usb_hal.obj" "src\Adc.obj" "src\CAN29.obj" "src\CmdHandler.obj" "src\DescriptorTask.obj" "src\Device_Init.obj" "src\ECAN.obj" "src\EPWM.obj" "src\EQEP.obj" "src\I2C.obj" "src\LightManager.obj" "src\MainControl.obj" "src\Parcenter.obj" "src\SCI.obj" "src\SPI.obj" "src\TLShutter.obj" "src\TaskMgr.obj" "src\Timer.obj" "src\USB.obj" "src\XINT.obj" "src\app.obj" "src\eeprom.obj" "src\eeprom_mmap.obj" "src\lightcontrol.obj" "src\monitoring.obj" "src\nosepiece.obj" "src\reflector.obj" "src\rltl.obj" "src\usb_bulk_structs.obj" 
	-$(RM) "lib\F2806x_Adc.d" "lib\F2806x_CpuTimers.d" "lib\F2806x_DefaultIsr.d" "lib\F2806x_GlobalVariableDefs.d" "lib\F2806x_PieCtrl.d" "lib\F2806x_PieVect.d" "lib\sysctl.d" "lib\uartstdio.d" "lib\usb.d" "lib\usb_hal.d" "src\Adc.d" "src\CAN29.d" "src\CmdHandler.d" "src\DescriptorTask.d" "src\Device_Init.d" "src\ECAN.d" "src\EPWM.d" "src\EQEP.d" "src\I2C.d" "src\LightManager.d" "src\MainControl.d" "src\Parcenter.d" "src\SCI.d" "src\SPI.d" "src\TLShutter.d" "src\TaskMgr.d" "src\Timer.d" "src\USB.d" "src\XINT.d" "src\app.d" "src\eeprom.d" "src\eeprom_mmap.d" "src\lightcontrol.d" "src\monitoring.d" "src\nosepiece.d" "src\reflector.d" "src\rltl.d" "src\usb_bulk_structs.d" 
	-$(RM) "F2806x_CodeStartBranch_ebss_init.d" "F2806x_usDelay.d" "bl_entrytable.d" 
	-@echo 'Finished clean'
	-@echo ' '

post-build:
	-@echo 'INTEL -HEX'
	-"C:/ti/ccsv7/tools/compiler/ti-cgt-c2000_16.9.1.LTS/bin/hex2000" -i "Axiolab_MCB_FL.out" -o "Axiolab_MCB_FL.hex" -order MS -romwidth 16
	-@echo ' '

.PHONY: all clean dependents
.SECONDARY:

-include ../makefile.targets

