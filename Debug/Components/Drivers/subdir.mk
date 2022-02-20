################################################################################
# Automatically-generated file. Do not edit!
# Toolchain: GNU Tools for STM32 (9-2020-q2-update)
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../Components/Drivers/oled_ascii.c \
../Components/Drivers/oled_driver.c \
../Components/Drivers/oled_gfx.c 

OBJS += \
./Components/Drivers/oled_ascii.o \
./Components/Drivers/oled_driver.o \
./Components/Drivers/oled_gfx.o 

C_DEPS += \
./Components/Drivers/oled_ascii.d \
./Components/Drivers/oled_driver.d \
./Components/Drivers/oled_gfx.d 


# Each subdirectory must supply rules for building sources it contributes
Components/Drivers/%.o: ../Components/Drivers/%.c Components/Drivers/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m4 -std=gnu11 -g3 -DDEBUG -DUSE_HAL_DRIVER -DSTM32F446xx -c -I../Core/Inc -I"C:/Users/Quent/Desktop/STM_Workspace/MS1_Workspace/MS1_hmi_446/Components/Drivers/inc" -I"C:/Users/Quent/Desktop/STM_Workspace/MS1_Workspace/MS1_hmi_446/Components/Utils/inc" -I"C:/Users/Quent/Desktop/STM_Workspace/MS1_Workspace/MS1_hmi_446/Components/HMI/inc" -I"C:/Users/Quent/Desktop/STM_Workspace/MS1_Workspace/MS1_hmi_446/Components/Configuration" -I"C:/Users/Quent/Desktop/STM_Workspace/MS1_Workspace/MS1_hmi_446/ThirdParty/FreeRTOS/portable/GCC/ARM_CM4F" -I"C:/Users/Quent/Desktop/STM_Workspace/MS1_Workspace/MS1_hmi_446/ThirdParty/FreeRTOS/portable/MemMang" -I"C:/Users/Quent/Desktop/STM_Workspace/MS1_Workspace/MS1_hmi_446/ThirdParty/FreeRTOS/include" -I../Drivers/STM32F4xx_HAL_Driver/Inc -I../Drivers/STM32F4xx_HAL_Driver/Inc/Legacy -I../Drivers/CMSIS/Device/ST/STM32F4xx/Include -I../Drivers/CMSIS/Include -O0 -ffunction-sections -fdata-sections -Wall -fstack-usage -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfpu=fpv4-sp-d16 -mfloat-abi=hard -mthumb -o "$@"

clean: clean-Components-2f-Drivers

clean-Components-2f-Drivers:
	-$(RM) ./Components/Drivers/oled_ascii.d ./Components/Drivers/oled_ascii.o ./Components/Drivers/oled_driver.d ./Components/Drivers/oled_driver.o ./Components/Drivers/oled_gfx.d ./Components/Drivers/oled_gfx.o

.PHONY: clean-Components-2f-Drivers

