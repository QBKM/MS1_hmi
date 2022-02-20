################################################################################
# Automatically-generated file. Do not edit!
# Toolchain: GNU Tools for STM32 (9-2020-q2-update)
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../Components/HMI/buttons.c \
../Components/HMI/fonts.c \
../Components/HMI/hmi.c \
../Components/HMI/leds.c \
../Components/HMI/oled_ascii.c \
../Components/HMI/oled_driver.c \
../Components/HMI/oled_gfx.c \
../Components/HMI/uart_parser.c 

OBJS += \
./Components/HMI/buttons.o \
./Components/HMI/fonts.o \
./Components/HMI/hmi.o \
./Components/HMI/leds.o \
./Components/HMI/oled_ascii.o \
./Components/HMI/oled_driver.o \
./Components/HMI/oled_gfx.o \
./Components/HMI/uart_parser.o 

C_DEPS += \
./Components/HMI/buttons.d \
./Components/HMI/fonts.d \
./Components/HMI/hmi.d \
./Components/HMI/leds.d \
./Components/HMI/oled_ascii.d \
./Components/HMI/oled_driver.d \
./Components/HMI/oled_gfx.d \
./Components/HMI/uart_parser.d 


# Each subdirectory must supply rules for building sources it contributes
Components/HMI/%.o: ../Components/HMI/%.c Components/HMI/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m4 -std=gnu11 -g3 -DDEBUG -DUSE_HAL_DRIVER -DSTM32F446xx -c -I../Core/Inc -I"C:/Users/Quent/Desktop/STM_Workspace/MS1_Workspace/MS1_hmi_446/Components/HMI/inc" -I"C:/Users/Quent/Desktop/STM_Workspace/MS1_Workspace/MS1_hmi_446/Components/Configuration" -I"C:/Users/Quent/Desktop/STM_Workspace/MS1_Workspace/MS1_hmi_446/ThirdParty/FreeRTOS/portable/GCC/ARM_CM4F" -I"C:/Users/Quent/Desktop/STM_Workspace/MS1_Workspace/MS1_hmi_446/ThirdParty/FreeRTOS/portable/MemMang" -I"C:/Users/Quent/Desktop/STM_Workspace/MS1_Workspace/MS1_hmi_446/ThirdParty/FreeRTOS/include" -I../Drivers/STM32F4xx_HAL_Driver/Inc -I../Drivers/STM32F4xx_HAL_Driver/Inc/Legacy -I../Drivers/CMSIS/Device/ST/STM32F4xx/Include -I../Drivers/CMSIS/Include -O0 -ffunction-sections -fdata-sections -Wall -fstack-usage -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfpu=fpv4-sp-d16 -mfloat-abi=hard -mthumb -o "$@"

clean: clean-Components-2f-HMI

clean-Components-2f-HMI:
	-$(RM) ./Components/HMI/buttons.d ./Components/HMI/buttons.o ./Components/HMI/fonts.d ./Components/HMI/fonts.o ./Components/HMI/hmi.d ./Components/HMI/hmi.o ./Components/HMI/leds.d ./Components/HMI/leds.o ./Components/HMI/oled_ascii.d ./Components/HMI/oled_ascii.o ./Components/HMI/oled_driver.d ./Components/HMI/oled_driver.o ./Components/HMI/oled_gfx.d ./Components/HMI/oled_gfx.o ./Components/HMI/uart_parser.d ./Components/HMI/uart_parser.o

.PHONY: clean-Components-2f-HMI

