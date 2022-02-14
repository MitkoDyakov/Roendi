################################################################################
# Automatically-generated file. Do not edit!
# Toolchain: GNU Tools for STM32 (9-2020-q2-update)
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
CPP_SRCS += \
../TouchGFX/generated/images/src/BitmapDatabase.cpp \
../TouchGFX/generated/images/src/image_background.cpp \
../TouchGFX/generated/images/src/image_color-palette.cpp \
../TouchGFX/generated/images/src/image_color-palette_grey.cpp \
../TouchGFX/generated/images/src/image_cube.cpp \
../TouchGFX/generated/images/src/image_cube_gray.cpp \
../TouchGFX/generated/images/src/image_eraser.cpp \
../TouchGFX/generated/images/src/image_eraser_gray.cpp \
../TouchGFX/generated/images/src/image_eye-dropper.cpp \
../TouchGFX/generated/images/src/image_eye-dropper_gray.cpp \
../TouchGFX/generated/images/src/image_knife-tool.cpp \
../TouchGFX/generated/images/src/image_knife-tool_gray.cpp \
../TouchGFX/generated/images/src/image_logo-design.cpp \
../TouchGFX/generated/images/src/image_logo-design_gray.cpp \
../TouchGFX/generated/images/src/image_pen-tool.cpp \
../TouchGFX/generated/images/src/image_pen-tool_gray.cpp \
../TouchGFX/generated/images/src/image_printer.cpp \
../TouchGFX/generated/images/src/image_printer_gray.cpp \
../TouchGFX/generated/images/src/image_ruler.cpp \
../TouchGFX/generated/images/src/image_ruler_gray.cpp \
../TouchGFX/generated/images/src/image_shapes-gray.cpp \
../TouchGFX/generated/images/src/image_shapes.cpp \
../TouchGFX/generated/images/src/image_spray-paint.cpp \
../TouchGFX/generated/images/src/image_spray-paint_gray.cpp \
../TouchGFX/generated/images/src/image_triangle-ruler.cpp \
../TouchGFX/generated/images/src/image_triangle-ruler_gray.cpp 

OBJS += \
./TouchGFX/generated/images/src/BitmapDatabase.o \
./TouchGFX/generated/images/src/image_background.o \
./TouchGFX/generated/images/src/image_color-palette.o \
./TouchGFX/generated/images/src/image_color-palette_grey.o \
./TouchGFX/generated/images/src/image_cube.o \
./TouchGFX/generated/images/src/image_cube_gray.o \
./TouchGFX/generated/images/src/image_eraser.o \
./TouchGFX/generated/images/src/image_eraser_gray.o \
./TouchGFX/generated/images/src/image_eye-dropper.o \
./TouchGFX/generated/images/src/image_eye-dropper_gray.o \
./TouchGFX/generated/images/src/image_knife-tool.o \
./TouchGFX/generated/images/src/image_knife-tool_gray.o \
./TouchGFX/generated/images/src/image_logo-design.o \
./TouchGFX/generated/images/src/image_logo-design_gray.o \
./TouchGFX/generated/images/src/image_pen-tool.o \
./TouchGFX/generated/images/src/image_pen-tool_gray.o \
./TouchGFX/generated/images/src/image_printer.o \
./TouchGFX/generated/images/src/image_printer_gray.o \
./TouchGFX/generated/images/src/image_ruler.o \
./TouchGFX/generated/images/src/image_ruler_gray.o \
./TouchGFX/generated/images/src/image_shapes-gray.o \
./TouchGFX/generated/images/src/image_shapes.o \
./TouchGFX/generated/images/src/image_spray-paint.o \
./TouchGFX/generated/images/src/image_spray-paint_gray.o \
./TouchGFX/generated/images/src/image_triangle-ruler.o \
./TouchGFX/generated/images/src/image_triangle-ruler_gray.o 

CPP_DEPS += \
./TouchGFX/generated/images/src/BitmapDatabase.d \
./TouchGFX/generated/images/src/image_background.d \
./TouchGFX/generated/images/src/image_color-palette.d \
./TouchGFX/generated/images/src/image_color-palette_grey.d \
./TouchGFX/generated/images/src/image_cube.d \
./TouchGFX/generated/images/src/image_cube_gray.d \
./TouchGFX/generated/images/src/image_eraser.d \
./TouchGFX/generated/images/src/image_eraser_gray.d \
./TouchGFX/generated/images/src/image_eye-dropper.d \
./TouchGFX/generated/images/src/image_eye-dropper_gray.d \
./TouchGFX/generated/images/src/image_knife-tool.d \
./TouchGFX/generated/images/src/image_knife-tool_gray.d \
./TouchGFX/generated/images/src/image_logo-design.d \
./TouchGFX/generated/images/src/image_logo-design_gray.d \
./TouchGFX/generated/images/src/image_pen-tool.d \
./TouchGFX/generated/images/src/image_pen-tool_gray.d \
./TouchGFX/generated/images/src/image_printer.d \
./TouchGFX/generated/images/src/image_printer_gray.d \
./TouchGFX/generated/images/src/image_ruler.d \
./TouchGFX/generated/images/src/image_ruler_gray.d \
./TouchGFX/generated/images/src/image_shapes-gray.d \
./TouchGFX/generated/images/src/image_shapes.d \
./TouchGFX/generated/images/src/image_spray-paint.d \
./TouchGFX/generated/images/src/image_spray-paint_gray.d \
./TouchGFX/generated/images/src/image_triangle-ruler.d \
./TouchGFX/generated/images/src/image_triangle-ruler_gray.d 


# Each subdirectory must supply rules for building sources it contributes
TouchGFX/generated/images/src/%.o: ../TouchGFX/generated/images/src/%.cpp TouchGFX/generated/images/src/subdir.mk
	arm-none-eabi-g++ "$<" -mcpu=cortex-m4 -std=gnu++14 -g3 -DDEBUG -DUSE_HAL_DRIVER -DSTM32L433xx -c -I../Core/Inc -I../TouchGFX/App -I../TouchGFX/target/generated -I../TouchGFX/target -I../Drivers/STM32L4xx_HAL_Driver/Inc -I../Drivers/STM32L4xx_HAL_Driver/Inc/Legacy -I../Drivers/CMSIS/Device/ST/STM32L4xx/Include -I../Drivers/CMSIS/Include -I../Middlewares/ST/touchgfx/framework/include -I../TouchGFX/generated/fonts/include -I../TouchGFX/generated/gui_generated/include -I../TouchGFX/generated/images/include -I../TouchGFX/generated/texts/include -I../TouchGFX/generated/videos/include -I../TouchGFX/gui/include -O0 -ffunction-sections -fdata-sections -fno-exceptions -fno-rtti -fno-use-cxa-atexit -Wall -femit-class-debug-always -fstack-usage -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfpu=fpv4-sp-d16 -mfloat-abi=hard -mthumb -o "$@"

clean: clean-TouchGFX-2f-generated-2f-images-2f-src

clean-TouchGFX-2f-generated-2f-images-2f-src:
	-$(RM) ./TouchGFX/generated/images/src/BitmapDatabase.d ./TouchGFX/generated/images/src/BitmapDatabase.o ./TouchGFX/generated/images/src/image_background.d ./TouchGFX/generated/images/src/image_background.o ./TouchGFX/generated/images/src/image_color-palette.d ./TouchGFX/generated/images/src/image_color-palette.o ./TouchGFX/generated/images/src/image_color-palette_grey.d ./TouchGFX/generated/images/src/image_color-palette_grey.o ./TouchGFX/generated/images/src/image_cube.d ./TouchGFX/generated/images/src/image_cube.o ./TouchGFX/generated/images/src/image_cube_gray.d ./TouchGFX/generated/images/src/image_cube_gray.o ./TouchGFX/generated/images/src/image_eraser.d ./TouchGFX/generated/images/src/image_eraser.o ./TouchGFX/generated/images/src/image_eraser_gray.d ./TouchGFX/generated/images/src/image_eraser_gray.o ./TouchGFX/generated/images/src/image_eye-dropper.d ./TouchGFX/generated/images/src/image_eye-dropper.o ./TouchGFX/generated/images/src/image_eye-dropper_gray.d ./TouchGFX/generated/images/src/image_eye-dropper_gray.o ./TouchGFX/generated/images/src/image_knife-tool.d ./TouchGFX/generated/images/src/image_knife-tool.o ./TouchGFX/generated/images/src/image_knife-tool_gray.d ./TouchGFX/generated/images/src/image_knife-tool_gray.o ./TouchGFX/generated/images/src/image_logo-design.d ./TouchGFX/generated/images/src/image_logo-design.o ./TouchGFX/generated/images/src/image_logo-design_gray.d ./TouchGFX/generated/images/src/image_logo-design_gray.o ./TouchGFX/generated/images/src/image_pen-tool.d ./TouchGFX/generated/images/src/image_pen-tool.o ./TouchGFX/generated/images/src/image_pen-tool_gray.d ./TouchGFX/generated/images/src/image_pen-tool_gray.o ./TouchGFX/generated/images/src/image_printer.d ./TouchGFX/generated/images/src/image_printer.o ./TouchGFX/generated/images/src/image_printer_gray.d ./TouchGFX/generated/images/src/image_printer_gray.o ./TouchGFX/generated/images/src/image_ruler.d ./TouchGFX/generated/images/src/image_ruler.o ./TouchGFX/generated/images/src/image_ruler_gray.d ./TouchGFX/generated/images/src/image_ruler_gray.o ./TouchGFX/generated/images/src/image_shapes-gray.d ./TouchGFX/generated/images/src/image_shapes-gray.o ./TouchGFX/generated/images/src/image_shapes.d ./TouchGFX/generated/images/src/image_shapes.o ./TouchGFX/generated/images/src/image_spray-paint.d ./TouchGFX/generated/images/src/image_spray-paint.o ./TouchGFX/generated/images/src/image_spray-paint_gray.d ./TouchGFX/generated/images/src/image_spray-paint_gray.o ./TouchGFX/generated/images/src/image_triangle-ruler.d ./TouchGFX/generated/images/src/image_triangle-ruler.o ./TouchGFX/generated/images/src/image_triangle-ruler_gray.d ./TouchGFX/generated/images/src/image_triangle-ruler_gray.o

.PHONY: clean-TouchGFX-2f-generated-2f-images-2f-src

