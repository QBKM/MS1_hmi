################################################################################
# Automatically-generated file. Do not edit!
# Toolchain: GNU Tools for STM32 (9-2020-q2-update)
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../Components/HMI/fonts.c \
../Components/HMI/leds.c \
../Components/HMI/oled.c \
../Components/HMI/ssd1351.c 

OBJS += \
./Components/HMI/fonts.o \
./Components/HMI/leds.o \
./Components/HMI/oled.o \
./Components/HMI/ssd1351.o 

C_DEPS += \
./Components/HMI/fonts.d \
./Components/HMI/leds.d \
./Components/HMI/oled.d \
./Components/HMI/ssd1351.d 


# Each subdirectory must supply rules for building sources it contributes
Components/HMI/%.o: ../Components/HMI/%.c Components/HMI/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m4 -std=gnu11 -g3 -DDEBUG -DUSE_HAL_DRIVER -DSTM32F446xx -c -I../Core/Inc -I"C:/Users/Quent/Desktop/STM_Workspace/MS1_Workspace/MS1_hmi_446/Components/HMI/inc" -I"C:/Users/Quent/Desktop/STM_Workspace/MS1_Workspace/MS1_hmi_446/Components/Configuration" -I"C:/Users/Quent/Desktop/STM_Workspace/MS1_Workspace/MS1_hmi_446/ThirdParty/FreeRTOS/portable/GCC/ARM_CM4F" -I"C:/Users/Quent/Desktop/STM_Workspace/MS1_Workspace/MS1_hmi_446/ThirdParty/FreeRTOS/portable/MemMang" -I"C:/Users/Quent/Desktop/STM_Workspace/MS1_Workspace/MS1_hmi_446/ThirdParty/FreeRTOS/include" -I../Drivers/STM32F4xx_HAL_Driver/Inc -I../Drivers/STM32F4xx_HAL_Driver/Inc/Legacy -I../Drivers/CMSIS/Device/ST/STM32F4xx/Include -I../Drivers/CMSIS/Include -O0 -ffunction-sections -fdata-sections -Wall -fstack-usage -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfpu=fpv4-sp-d16 -mfloat-abi=hard -mthumb -o "$@"

clean: clean-Components-2f-HMI

clean-Components-2f-HMI:
	-$(RM) ./Components/HMI/fonts.d ./Components/HMI/fonts.o ./Components/HMI/leds.d ./Components/HMI/leds.o ./Components/HMI/oled.d ./Components/HMI/oled.o ./Components/HMI/ssd1351.d ./Components/HMI/ssd1351.o

.PHONY: clean-Components-2f-HMI

