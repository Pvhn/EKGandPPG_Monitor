################################################################################
# Automatically-generated file. Do not edit!
# Toolchain: GNU Tools for STM32 (12.3.rel1)
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../Lib/ili9341/fonts.c \
../Lib/ili9341/ili9341.c 

OBJS += \
./Lib/ili9341/fonts.o \
./Lib/ili9341/ili9341.o 

C_DEPS += \
./Lib/ili9341/fonts.d \
./Lib/ili9341/ili9341.d 


# Each subdirectory must supply rules for building sources it contributes
Lib/ili9341/%.o Lib/ili9341/%.su Lib/ili9341/%.cyclo: ../Lib/ili9341/%.c Lib/ili9341/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m4 -std=gnu11 -g3 -DDEBUG -DUSE_HAL_DRIVER -DSTM32F407xx -c -I../Core/Inc -I../Drivers/STM32F4xx_HAL_Driver/Inc -I../Drivers/STM32F4xx_HAL_Driver/Inc/Legacy -I../Drivers/CMSIS/Device/ST/STM32F4xx/Include -I../Drivers/CMSIS/Include -I../Lib/ekgSensor -I../Lib/ppgSensor -I../Lib/max030102 -I../Lib/ili9341 -O0 -ffunction-sections -fdata-sections -Wall -fstack-usage -fcyclomatic-complexity -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfpu=fpv4-sp-d16 -mfloat-abi=hard -mthumb -o "$@"

clean: clean-Lib-2f-ili9341

clean-Lib-2f-ili9341:
	-$(RM) ./Lib/ili9341/fonts.cyclo ./Lib/ili9341/fonts.d ./Lib/ili9341/fonts.o ./Lib/ili9341/fonts.su ./Lib/ili9341/ili9341.cyclo ./Lib/ili9341/ili9341.d ./Lib/ili9341/ili9341.o ./Lib/ili9341/ili9341.su

.PHONY: clean-Lib-2f-ili9341

