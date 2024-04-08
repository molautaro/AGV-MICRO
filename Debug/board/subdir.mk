################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../board/board.c \
../board/clock_config.c \
../board/peripherals.c \
../board/pin_mux.c 

C_DEPS += \
./board/board.d \
./board/clock_config.d \
./board/peripherals.d \
./board/pin_mux.d 

OBJS += \
./board/board.o \
./board/clock_config.o \
./board/peripherals.o \
./board/pin_mux.o 


# Each subdirectory must supply rules for building sources it contributes
board/%.o: ../board/%.c board/subdir.mk
	@echo 'Building file: $<'
	@echo 'Invoking: MCU C Compiler'
	arm-none-eabi-gcc -DCPU_MK64FN1M0VLL12 -DCPU_MK64FN1M0VLL12_cm4 -DSDK_OS_BAREMETAL -DSDK_DEBUGCONSOLE=1 -DCR_INTEGER_PRINTF -DPRINTF_FLOAT_ENABLE=0 -DSERIAL_PORT_TYPE_UART=1 -D__MCUXPRESSO -D__USE_CMSIS -DDEBUG -D__REDLIB__ -I"C:\Users\Lauta\Documents\MCUXpressoIDE_11.6.1_8255\workspace\usb_config\drivers" -I"C:\Users\Lauta\Documents\MCUXpressoIDE_11.6.1_8255\workspace\usb_config\device" -I"C:\Users\Lauta\Documents\MCUXpressoIDE_11.6.1_8255\workspace\usb_config\CMSIS" -I"C:\Users\Lauta\Documents\MCUXpressoIDE_11.6.1_8255\workspace\usb_config\utilities" -I"C:\Users\Lauta\Documents\MCUXpressoIDE_11.6.1_8255\workspace\usb_config\component\serial_manager" -I"C:\Users\Lauta\Documents\MCUXpressoIDE_11.6.1_8255\workspace\usb_config\component\uart" -I"C:\Users\Lauta\Documents\MCUXpressoIDE_11.6.1_8255\workspace\usb_config\usb\include" -I"C:\Users\Lauta\Documents\MCUXpressoIDE_11.6.1_8255\workspace\usb_config\component\lists" -I"C:\Users\Lauta\Documents\MCUXpressoIDE_11.6.1_8255\workspace\usb_config\component\osa" -I"C:\Users\Lauta\Documents\MCUXpressoIDE_11.6.1_8255\workspace\usb_config\board" -I"C:\Users\Lauta\Documents\MCUXpressoIDE_11.6.1_8255\workspace\usb_config\source\generated" -I"C:\Users\Lauta\Documents\MCUXpressoIDE_11.6.1_8255\workspace\usb_config\usb\device\class\cdc" -I"C:\Users\Lauta\Documents\MCUXpressoIDE_11.6.1_8255\workspace\usb_config\usb\output\source\device\class" -I"C:\Users\Lauta\Documents\MCUXpressoIDE_11.6.1_8255\workspace\usb_config\usb\device\include" -I"C:\Users\Lauta\Documents\MCUXpressoIDE_11.6.1_8255\workspace\usb_config\usb\device\source" -I"C:\Users\Lauta\Documents\MCUXpressoIDE_11.6.1_8255\workspace\usb_config\usb\device\source\khci" -I"C:\Users\Lauta\Documents\MCUXpressoIDE_11.6.1_8255\workspace\usb_config\usb\device\class" -I"C:\Users\Lauta\Documents\MCUXpressoIDE_11.6.1_8255\workspace\usb_config\usb\output\source\device" -I"C:\Users\Lauta\Documents\MCUXpressoIDE_11.6.1_8255\workspace\usb_config\drivers" -I"C:\Users\Lauta\Documents\MCUXpressoIDE_11.6.1_8255\workspace\usb_config\device" -I"C:\Users\Lauta\Documents\MCUXpressoIDE_11.6.1_8255\workspace\usb_config\CMSIS" -I"C:\Users\Lauta\Documents\MCUXpressoIDE_11.6.1_8255\workspace\usb_config\utilities" -I"C:\Users\Lauta\Documents\MCUXpressoIDE_11.6.1_8255\workspace\usb_config\component\serial_manager" -I"C:\Users\Lauta\Documents\MCUXpressoIDE_11.6.1_8255\workspace\usb_config\component\uart" -I"C:\Users\Lauta\Documents\MCUXpressoIDE_11.6.1_8255\workspace\usb_config\usb\include" -I"C:\Users\Lauta\Documents\MCUXpressoIDE_11.6.1_8255\workspace\usb_config\component\lists" -I"C:\Users\Lauta\Documents\MCUXpressoIDE_11.6.1_8255\workspace\usb_config\component\osa" -I"C:\Users\Lauta\Documents\MCUXpressoIDE_11.6.1_8255\workspace\usb_config\board" -I"C:\Users\Lauta\Documents\MCUXpressoIDE_11.6.1_8255\workspace\usb_config\source\generated" -I"C:\Users\Lauta\Documents\MCUXpressoIDE_11.6.1_8255\workspace\usb_config\usb\device\class\cdc" -I"C:\Users\Lauta\Documents\MCUXpressoIDE_11.6.1_8255\workspace\usb_config\usb\output\source\device\class" -I"C:\Users\Lauta\Documents\MCUXpressoIDE_11.6.1_8255\workspace\usb_config\usb\device\include" -I"C:\Users\Lauta\Documents\MCUXpressoIDE_11.6.1_8255\workspace\usb_config\usb\device\source" -I"C:\Users\Lauta\Documents\MCUXpressoIDE_11.6.1_8255\workspace\usb_config\usb\device\source\khci" -I"C:\Users\Lauta\Documents\MCUXpressoIDE_11.6.1_8255\workspace\usb_config\usb\device\class" -I"C:\Users\Lauta\Documents\MCUXpressoIDE_11.6.1_8255\workspace\usb_config\usb\output\source\device" -I"C:\Users\Lauta\Documents\MCUXpressoIDE_11.6.1_8255\workspace\usb_config\source" -O0 -fno-common -g3 -Wall -c -ffunction-sections -fdata-sections -ffreestanding -fno-builtin -fmerge-constants -fmacro-prefix-map="$(<D)/"= -mcpu=cortex-m4 -mfpu=fpv4-sp-d16 -mfloat-abi=hard -mthumb -D__REDLIB__ -fstack-usage -specs=redlib.specs -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.o)" -MT"$(@:%.o=%.d)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


clean: clean-board

clean-board:
	-$(RM) ./board/board.d ./board/board.o ./board/clock_config.d ./board/clock_config.o ./board/peripherals.d ./board/peripherals.o ./board/pin_mux.d ./board/pin_mux.o

.PHONY: clean-board

