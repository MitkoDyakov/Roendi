################################################################################
# Automatically-generated file. Do not edit!
# Toolchain: GNU Tools for STM32 (9-2020-q2-update)
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
S_SRCS += \
../Core/Startup/startup_stm32l433cbux.s 

S_DEPS += \
./Core/Startup/startup_stm32l433cbux.d 

OBJS += \
./Core/Startup/startup_stm32l433cbux.o 


# Each subdirectory must supply rules for building sources it contributes
Core/Startup/%.o: ../Core/Startup/%.s Core/Startup/subdir.mk
	arm-none-eabi-gcc -mcpu=cortex-m4 -g3 -DDEBUG -c -I/RotaryEncoderWithDisplay/Middlewares/ST/STM32_USB_Device_Library/Class/Composit -I"C:/Users/Dimitar/STM32CubeIDE/workspace_1.6.1/RotaryEncoderWithDisplay/Middlewares/ST/STM32_USB_Device_Library/Class/CDC" -I/RotaryEncoderWithDisplay/Middlewares/ST/STM32_USB_Device_Library/Class/CustomHID -I/RotaryEncoderWithDisplay/Middlewares/ST/STM32_USB_Device_Library/Class/CDC/Inc -I"/RotaryEncoderWithDisplay/Middlewares/ST/STM32_USB_Device_Library/Class/CDC/Inc" -I"/RotaryEncoderWithDisplay/Middlewares/ST/STM32_USB_Device_Library/Class/Composit/Inc" -I../Middlewares/ST/touchgfx/framework/include -I../TouchGFX/generated/fonts/include -I../TouchGFX/generated/gui_generated/include -I../TouchGFX/generated/images/include -I../TouchGFX/generated/texts/include -I../TouchGFX/gui/include -I../Core/Inc -x assembler-with-cpp -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfpu=fpv4-sp-d16 -mfloat-abi=hard -mthumb -o "$@" "$<"

