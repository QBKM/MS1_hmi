################################################################################
# Automatically-generated file. Do not edit!
# Toolchain: GNU Tools for STM32 (9-2020-q2-update)
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../ThirdParty/FreeRTOS/portable/MemMang/heap_4.c 

OBJS += \
./ThirdParty/FreeRTOS/portable/MemMang/heap_4.o 

C_DEPS += \
./ThirdParty/FreeRTOS/portable/MemMang/heap_4.d 


# Each subdirectory must supply rules for building sources it contributes
ThirdParty/FreeRTOS/portable/MemMang/%.o: ../ThirdParty/FreeRTOS/portable/MemMang/%.c ThirdParty/FreeRTOS/portable/MemMang/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m4 -std=gnu11 -g3 -DDEBUG -DUSE_HAL_DRIVER -DSTM32F446xx -c -I../Core/Inc -I"C:/Users/Quent/Desktop/STM_Workspace/MS1_Workspace/MS1_hmi_446/Components/Drivers/inc" -I"C:/Users/Quent/Desktop/STM_Workspace/MS1_Workspace/MS1_hmi_446/Components/Utils/inc" -I"C:/Users/Quent/Desktop/STM_Workspace/MS1_Workspace/MS1_hmi_446/Components/HMI/inc" -I"C:/Users/Quent/Desktop/STM_Workspace/MS1_Workspace/MS1_hmi_446/Components/Configuration" -I"C:/Users/Quent/Desktop/STM_Workspace/MS1_Workspace/MS1_hmi_446/ThirdParty/FreeRTOS/portable/GCC/ARM_CM4F" -I"C:/Users/Quent/Desktop/STM_Workspace/MS1_Workspace/MS1_hmi_446/ThirdParty/FreeRTOS/portable/MemMang" -I"C:/Users/Quent/Desktop/STM_Workspace/MS1_Workspace/MS1_hmi_446/ThirdParty/FreeRTOS/include" -I../Drivers/STM32F4xx_HAL_Driver/Inc -I../Drivers/STM32F4xx_HAL_Driver/Inc/Legacy -I../Drivers/CMSIS/Device/ST/STM32F4xx/Include -I../Drivers/CMSIS/Include -O0 -ffunction-sections -fdata-sections -Wall -fstack-usage -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfpu=fpv4-sp-d16 -mfloat-abi=hard -mthumb -o "$@"

clean: clean-ThirdParty-2f-FreeRTOS-2f-portable-2f-MemMang

clean-ThirdParty-2f-FreeRTOS-2f-portable-2f-MemMang:
	-$(RM) ./ThirdParty/FreeRTOS/portable/MemMang/heap_4.d ./ThirdParty/FreeRTOS/portable/MemMang/heap_4.o

.PHONY: clean-ThirdParty-2f-FreeRTOS-2f-portable-2f-MemMang

