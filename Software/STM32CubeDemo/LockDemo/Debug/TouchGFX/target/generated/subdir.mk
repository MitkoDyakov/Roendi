################################################################################
# Automatically-generated file. Do not edit!
# Toolchain: GNU Tools for STM32 (9-2020-q2-update)
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
CPP_SRCS += \
../TouchGFX/target/generated/OSWrappers.cpp \
../TouchGFX/target/generated/STM32DMA.cpp \
../TouchGFX/target/generated/TouchGFXConfiguration.cpp \
../TouchGFX/target/generated/TouchGFXGeneratedHAL.cpp 

OBJS += \
./TouchGFX/target/generated/OSWrappers.o \
./TouchGFX/target/generated/STM32DMA.o \
./TouchGFX/target/generated/TouchGFXConfiguration.o \
./TouchGFX/target/generated/TouchGFXGeneratedHAL.o 

CPP_DEPS += \
./TouchGFX/target/generated/OSWrappers.d \
./TouchGFX/target/generated/STM32DMA.d \
./TouchGFX/target/generated/TouchGFXConfiguration.d \
./TouchGFX/target/generated/TouchGFXGeneratedHAL.d 


# Each subdirectory must supply rules for building sources it contributes
TouchGFX/target/generated/%.o: ../TouchGFX/target/generated/%.cpp TouchGFX/target/generated/subdir.mk
	arm-none-eabi-g++ "$<" -mcpu=cortex-m4 -std=gnu++14 -g3 -DDEBUG -DUSE_HAL_DRIVER -DSTM32L433xx -c -I../Core/Inc -I../Drivers/STM32L4xx_HAL_Driver/Inc -I../Drivers/STM32L4xx_HAL_Driver/Inc/Legacy -I../Drivers/CMSIS/Device/ST/STM32L4xx/Include -I../Drivers/CMSIS/Include -I../TouchGFX/App -I../TouchGFX/target/generated -I../TouchGFX/target -I../Middlewares/ST/touchgfx/framework/include -I../TouchGFX/generated/fonts/include -I../TouchGFX/generated/gui_generated/include -I../TouchGFX/generated/images/include -I../TouchGFX/generated/texts/include -I../TouchGFX/generated/videos/include -I../TouchGFX/gui/include -O0 -ffunction-sections -fdata-sections -fno-exceptions -fno-rtti -fno-use-cxa-atexit -Wall -femit-class-debug-always -fstack-usage -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfpu=fpv4-sp-d16 -mfloat-abi=hard -mthumb -o "$@"

clean: clean-TouchGFX-2f-target-2f-generated

clean-TouchGFX-2f-target-2f-generated:
	-$(RM) ./TouchGFX/target/generated/OSWrappers.d ./TouchGFX/target/generated/OSWrappers.o ./TouchGFX/target/generated/STM32DMA.d ./TouchGFX/target/generated/STM32DMA.o ./TouchGFX/target/generated/TouchGFXConfiguration.d ./TouchGFX/target/generated/TouchGFXConfiguration.o ./TouchGFX/target/generated/TouchGFXGeneratedHAL.d ./TouchGFX/target/generated/TouchGFXGeneratedHAL.o

.PHONY: clean-TouchGFX-2f-target-2f-generated

