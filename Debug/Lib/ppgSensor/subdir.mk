################################################################################
# Automatically-generated file. Do not edit!
# Toolchain: GNU Tools for STM32 (12.3.rel1)
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../Lib/ppgSensor/ppgSensor.c \
../Lib/ppgSensor/spO2_algorithim.c 

OBJS += \
./Lib/ppgSensor/ppgSensor.o \
./Lib/ppgSensor/spO2_algorithim.o 

C_DEPS += \
./Lib/ppgSensor/ppgSensor.d \
./Lib/ppgSensor/spO2_algorithim.d 


# Each subdirectory must supply rules for building sources it contributes
Lib/ppgSensor/%.o Lib/ppgSensor/%.su Lib/ppgSensor/%.cyclo: ../Lib/ppgSensor/%.c Lib/ppgSensor/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m4 -std=gnu11 -g3 -DDEBUG -DUSE_HAL_DRIVER -DSTM32F407xx -c -I../Core/Inc -I../Drivers/STM32F4xx_HAL_Driver/Inc -I../Drivers/STM32F4xx_HAL_Driver/Inc/Legacy -I../Drivers/CMSIS/Device/ST/STM32F4xx/Include -I../Drivers/CMSIS/Include -I../Lib/ekgSensor -I../Lib/ppgSensor -I../Lib/max030102 -I../Lib/ili9341 -O0 -ffunction-sections -fdata-sections -Wall -fstack-usage -fcyclomatic-complexity -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfpu=fpv4-sp-d16 -mfloat-abi=hard -mthumb -o "$@"

clean: clean-Lib-2f-ppgSensor

clean-Lib-2f-ppgSensor:
	-$(RM) ./Lib/ppgSensor/ppgSensor.cyclo ./Lib/ppgSensor/ppgSensor.d ./Lib/ppgSensor/ppgSensor.o ./Lib/ppgSensor/ppgSensor.su ./Lib/ppgSensor/spO2_algorithim.cyclo ./Lib/ppgSensor/spO2_algorithim.d ./Lib/ppgSensor/spO2_algorithim.o ./Lib/ppgSensor/spO2_algorithim.su

.PHONY: clean-Lib-2f-ppgSensor

