################################################################################
# Automatically-generated file. Do not edit!
# Toolchain: GNU Tools for STM32 (9-2020-q2-update)
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../Core/Src/adc.c \
../Core/Src/battery.c \
../Core/Src/communicate.c \
../Core/Src/control.c \
../Core/Src/dma.c \
../Core/Src/encoder.c \
../Core/Src/gpio.c \
../Core/Src/i2c.c \
../Core/Src/imu_bmx055.c \
../Core/Src/linetracer.c \
../Core/Src/main.c \
../Core/Src/module_test.c \
../Core/Src/motor.c \
../Core/Src/robotracer.c \
../Core/Src/sensor_adc.c \
../Core/Src/stm32f3xx_hal_msp.c \
../Core/Src/stm32f3xx_it.c \
../Core/Src/syscalls.c \
../Core/Src/sysmem.c \
../Core/Src/system_stm32f3xx.c \
../Core/Src/tim.c \
../Core/Src/usart.c 

OBJS += \
./Core/Src/adc.o \
./Core/Src/battery.o \
./Core/Src/communicate.o \
./Core/Src/control.o \
./Core/Src/dma.o \
./Core/Src/encoder.o \
./Core/Src/gpio.o \
./Core/Src/i2c.o \
./Core/Src/imu_bmx055.o \
./Core/Src/linetracer.o \
./Core/Src/main.o \
./Core/Src/module_test.o \
./Core/Src/motor.o \
./Core/Src/robotracer.o \
./Core/Src/sensor_adc.o \
./Core/Src/stm32f3xx_hal_msp.o \
./Core/Src/stm32f3xx_it.o \
./Core/Src/syscalls.o \
./Core/Src/sysmem.o \
./Core/Src/system_stm32f3xx.o \
./Core/Src/tim.o \
./Core/Src/usart.o 

C_DEPS += \
./Core/Src/adc.d \
./Core/Src/battery.d \
./Core/Src/communicate.d \
./Core/Src/control.d \
./Core/Src/dma.d \
./Core/Src/encoder.d \
./Core/Src/gpio.d \
./Core/Src/i2c.d \
./Core/Src/imu_bmx055.d \
./Core/Src/linetracer.d \
./Core/Src/main.d \
./Core/Src/module_test.d \
./Core/Src/motor.d \
./Core/Src/robotracer.d \
./Core/Src/sensor_adc.d \
./Core/Src/stm32f3xx_hal_msp.d \
./Core/Src/stm32f3xx_it.d \
./Core/Src/syscalls.d \
./Core/Src/sysmem.d \
./Core/Src/system_stm32f3xx.d \
./Core/Src/tim.d \
./Core/Src/usart.d 


# Each subdirectory must supply rules for building sources it contributes
Core/Src/%.o: ../Core/Src/%.c Core/Src/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m4 -std=gnu11 -g3 -DUSE_HAL_DRIVER -DSTM32F303x8 -DDEBUG -c -I../Drivers/CMSIS/Include -I../Drivers/STM32F3xx_HAL_Driver/Inc -I../Core/Inc -I../Drivers/STM32F3xx_HAL_Driver/Inc/Legacy -I../Drivers/CMSIS/Device/ST/STM32F3xx/Include -O0 -ffunction-sections -fdata-sections -Wall -fstack-usage -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfpu=fpv4-sp-d16 -mfloat-abi=hard -mthumb -o "$@"

