################################################################################
# Automatically-generated file. Do not edit!
# Toolchain: GNU Tools for STM32 (10.3-2021.10)
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../Core/Src/Peripherals/i2c.c \
../Core/Src/Peripherals/usart.c 

OBJS += \
./Core/Src/Peripherals/i2c.o \
./Core/Src/Peripherals/usart.o 

C_DEPS += \
./Core/Src/Peripherals/i2c.d \
./Core/Src/Peripherals/usart.d 


# Each subdirectory must supply rules for building sources it contributes
Core/Src/Peripherals/%.o Core/Src/Peripherals/%.su: ../Core/Src/Peripherals/%.c Core/Src/Peripherals/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m7 -std=gnu11 -g3 -DDEBUG -DUSE_HAL_DRIVER -DSTM32F767xx -c -I../Core/Inc -I../Drivers/STM32F7xx_HAL_Driver/Inc -I../Drivers/STM32F7xx_HAL_Driver/Inc/Legacy -I../Drivers/CMSIS/Device/ST/STM32F7xx/Include -I../Drivers/CMSIS/Include -I../Middlewares/Third_Party/FreeRTOS/Source/include -I../Middlewares/Third_Party/FreeRTOS/Source/CMSIS_RTOS -I../Middlewares/Third_Party/FreeRTOS/Source/portable/GCC/ARM_CM7/r0p1 -O0 -ffunction-sections -fdata-sections -Wall -fstack-usage -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfpu=fpv5-d16 -mfloat-abi=hard -mthumb -o "$@"

clean: clean-Core-2f-Src-2f-Peripherals

clean-Core-2f-Src-2f-Peripherals:
	-$(RM) ./Core/Src/Peripherals/i2c.d ./Core/Src/Peripherals/i2c.o ./Core/Src/Peripherals/i2c.su ./Core/Src/Peripherals/usart.d ./Core/Src/Peripherals/usart.o ./Core/Src/Peripherals/usart.su

.PHONY: clean-Core-2f-Src-2f-Peripherals

