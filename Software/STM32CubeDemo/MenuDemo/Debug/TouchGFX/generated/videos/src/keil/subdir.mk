################################################################################
# Automatically-generated file. Do not edit!
# Toolchain: GNU Tools for STM32 (9-2020-q2-update)
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
S_SRCS += \
../TouchGFX/generated/videos/src/keil/Videos.s 

S_DEPS += \
./TouchGFX/generated/videos/src/keil/Videos.d 

OBJS += \
./TouchGFX/generated/videos/src/keil/Videos.o 


# Each subdirectory must supply rules for building sources it contributes
TouchGFX/generated/videos/src/keil/%.o: ../TouchGFX/generated/videos/src/keil/%.s TouchGFX/generated/videos/src/keil/subdir.mk
	arm-none-eabi-gcc -mcpu=cortex-m4 -g3 -DDEBUG -c -x assembler-with-cpp -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfpu=fpv4-sp-d16 -mfloat-abi=hard -mthumb -o "$@" "$<"

clean: clean-TouchGFX-2f-generated-2f-videos-2f-src-2f-keil

clean-TouchGFX-2f-generated-2f-videos-2f-src-2f-keil:
	-$(RM) ./TouchGFX/generated/videos/src/keil/Videos.d ./TouchGFX/generated/videos/src/keil/Videos.o

.PHONY: clean-TouchGFX-2f-generated-2f-videos-2f-src-2f-keil

