################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Each subdirectory must supply rules for building sources it contributes
adc_task.obj: ../adc_task.c $(GEN_OPTS) $(GEN_HDRS)
	@echo 'Building file: $<'
	@echo 'Invoking: ARM Compiler'
	"C:/ti/ccsv6/tools/compiler/ti-cgt-arm_15.12.1.LTS/bin/armcl" -mv7M4 --code_state=16 --float_support=FPv4SPD16 --abi=eabi -me -O2 --include_path="C:/ti/ccsv6/tools/compiler/ti-cgt-arm_15.12.1.LTS/include" --include_path="P:/UNI/3rd Pro/ENCE464/group7/HelicopterControllerfin/HelicopterController/HelicopterController" --include_path="c:/ti/TivaWare_C_Series-2.1.3.156/examples/boards/ek-tm4c123gxl" --include_path="c:/ti/TivaWare_C_Series-2.1.3.156" --include_path="c:/ti/TivaWare_C_Series-2.1.3.156/third_party" --include_path="c:/ti/TivaWare_C_Series-2.1.3.156/third_party/FreeRTOS/Source/include" --include_path="c:/ti/TivaWare_C_Series-2.1.3.156/third_party/FreeRTOS" --include_path="c:/ti/TivaWare_C_Series-2.1.3.156/third_party/FreeRTOS/Source/portable/CCS/ARM_CM4F" --advice:power=all -g --gcc --define=ccs="ccs" --define=PART_TM4C123GH6PM --define=TARGET_IS_TM4C123_RB1 --diag_wrap=off --diag_warning=225 --display_error_number --gen_func_subsections=on --printf_support=full --ual --preproc_with_compile --preproc_dependency="adc_task.d" $(GEN_OPTS__FLAG) "$<"
	@echo 'Finished building: $<'
	@echo ' '

display_task.obj: ../display_task.c $(GEN_OPTS) $(GEN_HDRS)
	@echo 'Building file: $<'
	@echo 'Invoking: ARM Compiler'
	"C:/ti/ccsv6/tools/compiler/ti-cgt-arm_15.12.1.LTS/bin/armcl" -mv7M4 --code_state=16 --float_support=FPv4SPD16 --abi=eabi -me -O2 --include_path="C:/ti/ccsv6/tools/compiler/ti-cgt-arm_15.12.1.LTS/include" --include_path="P:/UNI/3rd Pro/ENCE464/group7/HelicopterControllerfin/HelicopterController/HelicopterController" --include_path="c:/ti/TivaWare_C_Series-2.1.3.156/examples/boards/ek-tm4c123gxl" --include_path="c:/ti/TivaWare_C_Series-2.1.3.156" --include_path="c:/ti/TivaWare_C_Series-2.1.3.156/third_party" --include_path="c:/ti/TivaWare_C_Series-2.1.3.156/third_party/FreeRTOS/Source/include" --include_path="c:/ti/TivaWare_C_Series-2.1.3.156/third_party/FreeRTOS" --include_path="c:/ti/TivaWare_C_Series-2.1.3.156/third_party/FreeRTOS/Source/portable/CCS/ARM_CM4F" --advice:power=all -g --gcc --define=ccs="ccs" --define=PART_TM4C123GH6PM --define=TARGET_IS_TM4C123_RB1 --diag_wrap=off --diag_warning=225 --display_error_number --gen_func_subsections=on --printf_support=full --ual --preproc_with_compile --preproc_dependency="display_task.d" $(GEN_OPTS__FLAG) "$<"
	@echo 'Finished building: $<'
	@echo ' '

project.obj: ../project.c $(GEN_OPTS) $(GEN_HDRS)
	@echo 'Building file: $<'
	@echo 'Invoking: ARM Compiler'
	"C:/ti/ccsv6/tools/compiler/ti-cgt-arm_15.12.1.LTS/bin/armcl" -mv7M4 --code_state=16 --float_support=FPv4SPD16 --abi=eabi -me -O2 --include_path="C:/ti/ccsv6/tools/compiler/ti-cgt-arm_15.12.1.LTS/include" --include_path="P:/UNI/3rd Pro/ENCE464/group7/HelicopterControllerfin/HelicopterController/HelicopterController" --include_path="c:/ti/TivaWare_C_Series-2.1.3.156/examples/boards/ek-tm4c123gxl" --include_path="c:/ti/TivaWare_C_Series-2.1.3.156" --include_path="c:/ti/TivaWare_C_Series-2.1.3.156/third_party" --include_path="c:/ti/TivaWare_C_Series-2.1.3.156/third_party/FreeRTOS/Source/include" --include_path="c:/ti/TivaWare_C_Series-2.1.3.156/third_party/FreeRTOS" --include_path="c:/ti/TivaWare_C_Series-2.1.3.156/third_party/FreeRTOS/Source/portable/CCS/ARM_CM4F" --advice:power=all -g --gcc --define=ccs="ccs" --define=PART_TM4C123GH6PM --define=TARGET_IS_TM4C123_RB1 --diag_wrap=off --diag_warning=225 --display_error_number --gen_func_subsections=on --printf_support=full --ual --preproc_with_compile --preproc_dependency="project.d" $(GEN_OPTS__FLAG) "$<"
	@echo 'Finished building: $<'
	@echo ' '

pwm_task.obj: ../pwm_task.c $(GEN_OPTS) $(GEN_HDRS)
	@echo 'Building file: $<'
	@echo 'Invoking: ARM Compiler'
	"C:/ti/ccsv6/tools/compiler/ti-cgt-arm_15.12.1.LTS/bin/armcl" -mv7M4 --code_state=16 --float_support=FPv4SPD16 --abi=eabi -me -O2 --include_path="C:/ti/ccsv6/tools/compiler/ti-cgt-arm_15.12.1.LTS/include" --include_path="P:/UNI/3rd Pro/ENCE464/group7/HelicopterControllerfin/HelicopterController/HelicopterController" --include_path="c:/ti/TivaWare_C_Series-2.1.3.156/examples/boards/ek-tm4c123gxl" --include_path="c:/ti/TivaWare_C_Series-2.1.3.156" --include_path="c:/ti/TivaWare_C_Series-2.1.3.156/third_party" --include_path="c:/ti/TivaWare_C_Series-2.1.3.156/third_party/FreeRTOS/Source/include" --include_path="c:/ti/TivaWare_C_Series-2.1.3.156/third_party/FreeRTOS" --include_path="c:/ti/TivaWare_C_Series-2.1.3.156/third_party/FreeRTOS/Source/portable/CCS/ARM_CM4F" --advice:power=all -g --gcc --define=ccs="ccs" --define=PART_TM4C123GH6PM --define=TARGET_IS_TM4C123_RB1 --diag_wrap=off --diag_warning=225 --display_error_number --gen_func_subsections=on --printf_support=full --ual --preproc_with_compile --preproc_dependency="pwm_task.d" $(GEN_OPTS__FLAG) "$<"
	@echo 'Finished building: $<'
	@echo ' '

startup_ccs.obj: ../startup_ccs.c $(GEN_OPTS) $(GEN_HDRS)
	@echo 'Building file: $<'
	@echo 'Invoking: ARM Compiler'
	"C:/ti/ccsv6/tools/compiler/ti-cgt-arm_15.12.1.LTS/bin/armcl" -mv7M4 --code_state=16 --float_support=FPv4SPD16 --abi=eabi -me -O2 --include_path="C:/ti/ccsv6/tools/compiler/ti-cgt-arm_15.12.1.LTS/include" --include_path="P:/UNI/3rd Pro/ENCE464/group7/HelicopterControllerfin/HelicopterController/HelicopterController" --include_path="c:/ti/TivaWare_C_Series-2.1.3.156/examples/boards/ek-tm4c123gxl" --include_path="c:/ti/TivaWare_C_Series-2.1.3.156" --include_path="c:/ti/TivaWare_C_Series-2.1.3.156/third_party" --include_path="c:/ti/TivaWare_C_Series-2.1.3.156/third_party/FreeRTOS/Source/include" --include_path="c:/ti/TivaWare_C_Series-2.1.3.156/third_party/FreeRTOS" --include_path="c:/ti/TivaWare_C_Series-2.1.3.156/third_party/FreeRTOS/Source/portable/CCS/ARM_CM4F" --advice:power=all -g --gcc --define=ccs="ccs" --define=PART_TM4C123GH6PM --define=TARGET_IS_TM4C123_RB1 --diag_wrap=off --diag_warning=225 --display_error_number --gen_func_subsections=on --printf_support=full --ual --preproc_with_compile --preproc_dependency="startup_ccs.d" $(GEN_OPTS__FLAG) "$<"
	@echo 'Finished building: $<'
	@echo ' '

switch_task.obj: ../switch_task.c $(GEN_OPTS) $(GEN_HDRS)
	@echo 'Building file: $<'
	@echo 'Invoking: ARM Compiler'
	"C:/ti/ccsv6/tools/compiler/ti-cgt-arm_15.12.1.LTS/bin/armcl" -mv7M4 --code_state=16 --float_support=FPv4SPD16 --abi=eabi -me -O2 --include_path="C:/ti/ccsv6/tools/compiler/ti-cgt-arm_15.12.1.LTS/include" --include_path="P:/UNI/3rd Pro/ENCE464/group7/HelicopterControllerfin/HelicopterController/HelicopterController" --include_path="c:/ti/TivaWare_C_Series-2.1.3.156/examples/boards/ek-tm4c123gxl" --include_path="c:/ti/TivaWare_C_Series-2.1.3.156" --include_path="c:/ti/TivaWare_C_Series-2.1.3.156/third_party" --include_path="c:/ti/TivaWare_C_Series-2.1.3.156/third_party/FreeRTOS/Source/include" --include_path="c:/ti/TivaWare_C_Series-2.1.3.156/third_party/FreeRTOS" --include_path="c:/ti/TivaWare_C_Series-2.1.3.156/third_party/FreeRTOS/Source/portable/CCS/ARM_CM4F" --advice:power=all -g --gcc --define=ccs="ccs" --define=PART_TM4C123GH6PM --define=TARGET_IS_TM4C123_RB1 --diag_wrap=off --diag_warning=225 --display_error_number --gen_func_subsections=on --printf_support=full --ual --preproc_with_compile --preproc_dependency="switch_task.d" $(GEN_OPTS__FLAG) "$<"
	@echo 'Finished building: $<'
	@echo ' '

yaw_task.obj: ../yaw_task.c $(GEN_OPTS) $(GEN_HDRS)
	@echo 'Building file: $<'
	@echo 'Invoking: ARM Compiler'
	"C:/ti/ccsv6/tools/compiler/ti-cgt-arm_15.12.1.LTS/bin/armcl" -mv7M4 --code_state=16 --float_support=FPv4SPD16 --abi=eabi -me -O2 --include_path="C:/ti/ccsv6/tools/compiler/ti-cgt-arm_15.12.1.LTS/include" --include_path="P:/UNI/3rd Pro/ENCE464/group7/HelicopterControllerfin/HelicopterController/HelicopterController" --include_path="c:/ti/TivaWare_C_Series-2.1.3.156/examples/boards/ek-tm4c123gxl" --include_path="c:/ti/TivaWare_C_Series-2.1.3.156" --include_path="c:/ti/TivaWare_C_Series-2.1.3.156/third_party" --include_path="c:/ti/TivaWare_C_Series-2.1.3.156/third_party/FreeRTOS/Source/include" --include_path="c:/ti/TivaWare_C_Series-2.1.3.156/third_party/FreeRTOS" --include_path="c:/ti/TivaWare_C_Series-2.1.3.156/third_party/FreeRTOS/Source/portable/CCS/ARM_CM4F" --advice:power=all -g --gcc --define=ccs="ccs" --define=PART_TM4C123GH6PM --define=TARGET_IS_TM4C123_RB1 --diag_wrap=off --diag_warning=225 --display_error_number --gen_func_subsections=on --printf_support=full --ual --preproc_with_compile --preproc_dependency="yaw_task.d" $(GEN_OPTS__FLAG) "$<"
	@echo 'Finished building: $<'
	@echo ' '


