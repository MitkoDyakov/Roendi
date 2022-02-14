################################################################################
# Automatically-generated file. Do not edit!
# Toolchain: GNU Tools for STM32 (9-2020-q2-update)
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
CPP_SRCS += \
../TouchGFX/generated/fonts/src/ApplicationFontProvider.cpp \
../TouchGFX/generated/fonts/src/CachedFont.cpp \
../TouchGFX/generated/fonts/src/FontCache.cpp \
../TouchGFX/generated/fonts/src/Font_verdanab_25_4bpp_0.cpp \
../TouchGFX/generated/fonts/src/GeneratedFont.cpp \
../TouchGFX/generated/fonts/src/Kerning_verdanab_25_4bpp.cpp \
../TouchGFX/generated/fonts/src/Table_verdanab_25_4bpp.cpp \
../TouchGFX/generated/fonts/src/UnmappedDataFont.cpp 

OBJS += \
./TouchGFX/generated/fonts/src/ApplicationFontProvider.o \
./TouchGFX/generated/fonts/src/CachedFont.o \
./TouchGFX/generated/fonts/src/FontCache.o \
./TouchGFX/generated/fonts/src/Font_verdanab_25_4bpp_0.o \
./TouchGFX/generated/fonts/src/GeneratedFont.o \
./TouchGFX/generated/fonts/src/Kerning_verdanab_25_4bpp.o \
./TouchGFX/generated/fonts/src/Table_verdanab_25_4bpp.o \
./TouchGFX/generated/fonts/src/UnmappedDataFont.o 

CPP_DEPS += \
./TouchGFX/generated/fonts/src/ApplicationFontProvider.d \
./TouchGFX/generated/fonts/src/CachedFont.d \
./TouchGFX/generated/fonts/src/FontCache.d \
./TouchGFX/generated/fonts/src/Font_verdanab_25_4bpp_0.d \
./TouchGFX/generated/fonts/src/GeneratedFont.d \
./TouchGFX/generated/fonts/src/Kerning_verdanab_25_4bpp.d \
./TouchGFX/generated/fonts/src/Table_verdanab_25_4bpp.d \
./TouchGFX/generated/fonts/src/UnmappedDataFont.d 


# Each subdirectory must supply rules for building sources it contributes
TouchGFX/generated/fonts/src/%.o: ../TouchGFX/generated/fonts/src/%.cpp TouchGFX/generated/fonts/src/subdir.mk
	arm-none-eabi-g++ "$<" -mcpu=cortex-m4 -std=gnu++14 -g3 -DDEBUG -DUSE_HAL_DRIVER -DSTM32L433xx -c -I../Core/Inc -I../TouchGFX/App -I../TouchGFX/target/generated -I../TouchGFX/target -I../Drivers/STM32L4xx_HAL_Driver/Inc -I../Drivers/STM32L4xx_HAL_Driver/Inc/Legacy -I../Drivers/CMSIS/Device/ST/STM32L4xx/Include -I../Drivers/CMSIS/Include -I../Middlewares/ST/touchgfx/framework/include -I../TouchGFX/generated/fonts/include -I../TouchGFX/generated/gui_generated/include -I../TouchGFX/generated/images/include -I../TouchGFX/generated/texts/include -I../TouchGFX/generated/videos/include -I../TouchGFX/gui/include -O0 -ffunction-sections -fdata-sections -fno-exceptions -fno-rtti -fno-use-cxa-atexit -Wall -femit-class-debug-always -fstack-usage -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfpu=fpv4-sp-d16 -mfloat-abi=hard -mthumb -o "$@"

clean: clean-TouchGFX-2f-generated-2f-fonts-2f-src

clean-TouchGFX-2f-generated-2f-fonts-2f-src:
	-$(RM) ./TouchGFX/generated/fonts/src/ApplicationFontProvider.d ./TouchGFX/generated/fonts/src/ApplicationFontProvider.o ./TouchGFX/generated/fonts/src/CachedFont.d ./TouchGFX/generated/fonts/src/CachedFont.o ./TouchGFX/generated/fonts/src/FontCache.d ./TouchGFX/generated/fonts/src/FontCache.o ./TouchGFX/generated/fonts/src/Font_verdanab_25_4bpp_0.d ./TouchGFX/generated/fonts/src/Font_verdanab_25_4bpp_0.o ./TouchGFX/generated/fonts/src/GeneratedFont.d ./TouchGFX/generated/fonts/src/GeneratedFont.o ./TouchGFX/generated/fonts/src/Kerning_verdanab_25_4bpp.d ./TouchGFX/generated/fonts/src/Kerning_verdanab_25_4bpp.o ./TouchGFX/generated/fonts/src/Table_verdanab_25_4bpp.d ./TouchGFX/generated/fonts/src/Table_verdanab_25_4bpp.o ./TouchGFX/generated/fonts/src/UnmappedDataFont.d ./TouchGFX/generated/fonts/src/UnmappedDataFont.o

.PHONY: clean-TouchGFX-2f-generated-2f-fonts-2f-src

