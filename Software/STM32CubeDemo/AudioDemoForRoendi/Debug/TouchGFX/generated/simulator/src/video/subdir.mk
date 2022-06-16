################################################################################
# Automatically-generated file. Do not edit!
# Toolchain: GNU Tools for STM32 (10.3-2021.10)
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
CPP_SRCS += \
../TouchGFX/generated/simulator/src/video/SoftwareMJPEGDecoder.cpp 

OBJS += \
./TouchGFX/generated/simulator/src/video/SoftwareMJPEGDecoder.o 

CPP_DEPS += \
./TouchGFX/generated/simulator/src/video/SoftwareMJPEGDecoder.d 


# Each subdirectory must supply rules for building sources it contributes
TouchGFX/generated/simulator/src/video/%.o TouchGFX/generated/simulator/src/video/%.su: ../TouchGFX/generated/simulator/src/video/%.cpp TouchGFX/generated/simulator/src/video/subdir.mk
	arm-none-eabi-g++ "$<" -mcpu=cortex-m4 -std=gnu++14 -g3 -DDEBUG -DUSE_HAL_DRIVER -DSTM32L433xx -c -I../Core/Inc -I../TouchGFX/App -I../TouchGFX/target/generated -I../TouchGFX/target -I../Drivers/STM32L4xx_HAL_Driver/Inc -I../Drivers/STM32L4xx_HAL_Driver/Inc/Legacy -I../Drivers/CMSIS/Device/ST/STM32L4xx/Include -I../Drivers/CMSIS/Include -O0 -ffunction-sections -fdata-sections -fno-exceptions -fno-rtti -fno-use-cxa-atexit -Wall -fstack-usage -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfpu=fpv4-sp-d16 -mfloat-abi=hard -mthumb -o "$@"

clean: clean-TouchGFX-2f-generated-2f-simulator-2f-src-2f-video

clean-TouchGFX-2f-generated-2f-simulator-2f-src-2f-video:
	-$(RM) ./TouchGFX/generated/simulator/src/video/SoftwareMJPEGDecoder.d ./TouchGFX/generated/simulator/src/video/SoftwareMJPEGDecoder.o ./TouchGFX/generated/simulator/src/video/SoftwareMJPEGDecoder.su

.PHONY: clean-TouchGFX-2f-generated-2f-simulator-2f-src-2f-video

