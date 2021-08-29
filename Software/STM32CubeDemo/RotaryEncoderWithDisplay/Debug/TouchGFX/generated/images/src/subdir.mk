################################################################################
# Automatically-generated file. Do not edit!
# Toolchain: GNU Tools for STM32 (9-2020-q2-update)
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
CPP_SRCS += \
../TouchGFX/generated/images/src/BitmapDatabase.cpp \
../TouchGFX/generated/images/src/image_adasda.cpp \
../TouchGFX/generated/images/src/image_bac.cpp \
../TouchGFX/generated/images/src/image_background.cpp \
../TouchGFX/generated/images/src/image_backgroundHome.cpp \
../TouchGFX/generated/images/src/image_banna.cpp \
../TouchGFX/generated/images/src/image_blue.cpp \
../TouchGFX/generated/images/src/image_dogo.cpp \
../TouchGFX/generated/images/src/image_homer.cpp \
../TouchGFX/generated/images/src/image_idea.cpp \
../TouchGFX/generated/images/src/image_padlock.cpp \
../TouchGFX/generated/images/src/image_planet.cpp \
../TouchGFX/generated/images/src/image_small_speaker.cpp \
../TouchGFX/generated/images/src/image_temp.cpp \
../TouchGFX/generated/images/src/image_thermometer.cpp \
../TouchGFX/generated/images/src/image_tumbnail.cpp \
../TouchGFX/generated/images/src/image_vinyl.cpp 

OBJS += \
./TouchGFX/generated/images/src/BitmapDatabase.o \
./TouchGFX/generated/images/src/image_adasda.o \
./TouchGFX/generated/images/src/image_bac.o \
./TouchGFX/generated/images/src/image_background.o \
./TouchGFX/generated/images/src/image_backgroundHome.o \
./TouchGFX/generated/images/src/image_banna.o \
./TouchGFX/generated/images/src/image_blue.o \
./TouchGFX/generated/images/src/image_dogo.o \
./TouchGFX/generated/images/src/image_homer.o \
./TouchGFX/generated/images/src/image_idea.o \
./TouchGFX/generated/images/src/image_padlock.o \
./TouchGFX/generated/images/src/image_planet.o \
./TouchGFX/generated/images/src/image_small_speaker.o \
./TouchGFX/generated/images/src/image_temp.o \
./TouchGFX/generated/images/src/image_thermometer.o \
./TouchGFX/generated/images/src/image_tumbnail.o \
./TouchGFX/generated/images/src/image_vinyl.o 

CPP_DEPS += \
./TouchGFX/generated/images/src/BitmapDatabase.d \
./TouchGFX/generated/images/src/image_adasda.d \
./TouchGFX/generated/images/src/image_bac.d \
./TouchGFX/generated/images/src/image_background.d \
./TouchGFX/generated/images/src/image_backgroundHome.d \
./TouchGFX/generated/images/src/image_banna.d \
./TouchGFX/generated/images/src/image_blue.d \
./TouchGFX/generated/images/src/image_dogo.d \
./TouchGFX/generated/images/src/image_homer.d \
./TouchGFX/generated/images/src/image_idea.d \
./TouchGFX/generated/images/src/image_padlock.d \
./TouchGFX/generated/images/src/image_planet.d \
./TouchGFX/generated/images/src/image_small_speaker.d \
./TouchGFX/generated/images/src/image_temp.d \
./TouchGFX/generated/images/src/image_thermometer.d \
./TouchGFX/generated/images/src/image_tumbnail.d \
./TouchGFX/generated/images/src/image_vinyl.d 


# Each subdirectory must supply rules for building sources it contributes
TouchGFX/generated/images/src/%.o: ../TouchGFX/generated/images/src/%.cpp TouchGFX/generated/images/src/subdir.mk
	arm-none-eabi-g++ "$<" -mcpu=cortex-m4 -std=gnu++14 -g3 -DDEBUG -DUSE_HAL_DRIVER -DSTM32L433xx -c -I../Core/Inc -I../Drivers/STM32L4xx_HAL_Driver/Inc -I../Drivers/STM32L4xx_HAL_Driver/Inc/Legacy -I../Drivers/CMSIS/Device/ST/STM32L4xx/Include -I../Drivers/CMSIS/Include -I../USB_DEVICE/App -I../USB_DEVICE/Target -I../Middlewares/ST/STM32_USB_Device_Library/Core/Inc -I../Middlewares/ST/STM32_USB_Device_Library/Class/CustomHID/Inc -I/RotaryEncoderWithDisplay/Middlewares/ST/STM32_USB_Device_Library/Class/Composit -I"C:/Users/Dimitar/STM32CubeIDE/workspace_1.6.1/RotaryEncoderWithDisplay/Middlewares/ST/STM32_USB_Device_Library/Class/CDC" -I/RotaryEncoderWithDisplay/Middlewares/ST/STM32_USB_Device_Library/Class/CustomHID -I/RotaryEncoderWithDisplay/Middlewares/ST/STM32_USB_Device_Library/Class/CDC/Inc -I"/RotaryEncoderWithDisplay/Middlewares/ST/STM32_USB_Device_Library/Class/CDC/Inc" -I"/RotaryEncoderWithDisplay/Middlewares/ST/STM32_USB_Device_Library/Class/Composit/Inc" -I../TouchGFX/App -I../TouchGFX/target/generated -I../TouchGFX/target -I../Middlewares/ST/touchgfx/framework/include -I../TouchGFX/generated/fonts/include -I../TouchGFX/generated/gui_generated/include -I../TouchGFX/generated/images/include -I../TouchGFX/generated/texts/include -I../TouchGFX/gui/include -O0 -ffunction-sections -fdata-sections -fno-exceptions -fno-rtti -fno-use-cxa-atexit -Wall -femit-class-debug-always -fstack-usage -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfpu=fpv4-sp-d16 -mfloat-abi=hard -mthumb -o "$@"

