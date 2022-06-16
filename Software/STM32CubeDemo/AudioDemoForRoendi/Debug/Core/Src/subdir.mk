################################################################################
# Automatically-generated file. Do not edit!
# Toolchain: GNU Tools for STM32 (10.3-2021.10)
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../Core/Src/GC9A01.c \
../Core/Src/WavePlayer.c \
../Core/Src/crc.c \
../Core/Src/dac.c \
../Core/Src/default_data.c \
../Core/Src/dma.c \
../Core/Src/gpio.c \
../Core/Src/lptim.c \
../Core/Src/main.c \
../Core/Src/quadspi.c \
../Core/Src/spi.c \
../Core/Src/stm32l4xx_hal_msp.c \
../Core/Src/stm32l4xx_it.c \
../Core/Src/syscalls.c \
../Core/Src/sysmem.c \
../Core/Src/system_stm32l4xx.c \
../Core/Src/tim.c \
../Core/Src/wrapper_tgfx.c 

C_DEPS += \
./Core/Src/GC9A01.d \
./Core/Src/WavePlayer.d \
./Core/Src/crc.d \
./Core/Src/dac.d \
./Core/Src/default_data.d \
./Core/Src/dma.d \
./Core/Src/gpio.d \
./Core/Src/lptim.d \
./Core/Src/main.d \
./Core/Src/quadspi.d \
./Core/Src/spi.d \
./Core/Src/stm32l4xx_hal_msp.d \
./Core/Src/stm32l4xx_it.d \
./Core/Src/syscalls.d \
./Core/Src/sysmem.d \
./Core/Src/system_stm32l4xx.d \
./Core/Src/tim.d \
./Core/Src/wrapper_tgfx.d 

OBJS += \
./Core/Src/GC9A01.o \
./Core/Src/WavePlayer.o \
./Core/Src/crc.o \
./Core/Src/dac.o \
./Core/Src/default_data.o \
./Core/Src/dma.o \
./Core/Src/gpio.o \
./Core/Src/lptim.o \
./Core/Src/main.o \
./Core/Src/quadspi.o \
./Core/Src/spi.o \
./Core/Src/stm32l4xx_hal_msp.o \
./Core/Src/stm32l4xx_it.o \
./Core/Src/syscalls.o \
./Core/Src/sysmem.o \
./Core/Src/system_stm32l4xx.o \
./Core/Src/tim.o \
./Core/Src/wrapper_tgfx.o 


# Each subdirectory must supply rules for building sources it contributes
Core/Src/%.o Core/Src/%.su: ../Core/Src/%.c Core/Src/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m4 -std=gnu11 -g3 -DDEBUG -DUSE_HAL_DRIVER -DSTM32L433xx -c -I../Core/Inc -I../TouchGFX/App -I../TouchGFX/target/generated -I../TouchGFX/target -I../Drivers/STM32L4xx_HAL_Driver/Inc -I../Drivers/STM32L4xx_HAL_Driver/Inc/Legacy -I../Drivers/CMSIS/Device/ST/STM32L4xx/Include -I../Drivers/CMSIS/Include -I../Middlewares/ST/touchgfx/framework/include -I../TouchGFX/generated/fonts/include -I../TouchGFX/generated/gui_generated/include -I../TouchGFX/generated/images/include -I../TouchGFX/generated/texts/include -I../TouchGFX/generated/videos/include -I../TouchGFX/gui/include -O0 -ffunction-sections -fdata-sections -Wall -fstack-usage -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfpu=fpv4-sp-d16 -mfloat-abi=hard -mthumb -o "$@"

clean: clean-Core-2f-Src

clean-Core-2f-Src:
	-$(RM) ./Core/Src/GC9A01.d ./Core/Src/GC9A01.o ./Core/Src/GC9A01.su ./Core/Src/WavePlayer.d ./Core/Src/WavePlayer.o ./Core/Src/WavePlayer.su ./Core/Src/crc.d ./Core/Src/crc.o ./Core/Src/crc.su ./Core/Src/dac.d ./Core/Src/dac.o ./Core/Src/dac.su ./Core/Src/default_data.d ./Core/Src/default_data.o ./Core/Src/default_data.su ./Core/Src/dma.d ./Core/Src/dma.o ./Core/Src/dma.su ./Core/Src/gpio.d ./Core/Src/gpio.o ./Core/Src/gpio.su ./Core/Src/lptim.d ./Core/Src/lptim.o ./Core/Src/lptim.su ./Core/Src/main.d ./Core/Src/main.o ./Core/Src/main.su ./Core/Src/quadspi.d ./Core/Src/quadspi.o ./Core/Src/quadspi.su ./Core/Src/spi.d ./Core/Src/spi.o ./Core/Src/spi.su ./Core/Src/stm32l4xx_hal_msp.d ./Core/Src/stm32l4xx_hal_msp.o ./Core/Src/stm32l4xx_hal_msp.su ./Core/Src/stm32l4xx_it.d ./Core/Src/stm32l4xx_it.o ./Core/Src/stm32l4xx_it.su ./Core/Src/syscalls.d ./Core/Src/syscalls.o ./Core/Src/syscalls.su ./Core/Src/sysmem.d ./Core/Src/sysmem.o ./Core/Src/sysmem.su ./Core/Src/system_stm32l4xx.d ./Core/Src/system_stm32l4xx.o ./Core/Src/system_stm32l4xx.su ./Core/Src/tim.d ./Core/Src/tim.o ./Core/Src/tim.su ./Core/Src/wrapper_tgfx.d ./Core/Src/wrapper_tgfx.o ./Core/Src/wrapper_tgfx.su

.PHONY: clean-Core-2f-Src

