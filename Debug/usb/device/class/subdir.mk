################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../usb/device/class/usb_device_class.c 

C_DEPS += \
./usb/device/class/usb_device_class.d 

OBJS += \
./usb/device/class/usb_device_class.o 


# Each subdirectory must supply rules for building sources it contributes
usb/device/class/%.o: ../usb/device/class/%.c usb/device/class/subdir.mk
	@echo 'Building file: $<'
	@echo 'Invoking: MCU C Compiler'
	arm-none-eabi-gcc -DCPU_MK64FN1M0VLL12 -DCPU_MK64FN1M0VLL12_cm4 -DSDK_OS_BAREMETAL -DSDK_DEBUGCONSOLE=1 -DCR_INTEGER_PRINTF -DPRINTF_FLOAT_ENABLE=0 -DSERIAL_PORT_TYPE_UART=1 -D__MCUXPRESSO -D__USE_CMSIS -DDEBUG -D__REDLIB__ -I"C:\Users\wen_c\Documents\MCUXpressoIDE_11.6.1_8255\workspace\new\AGV-MICRO\drivers" -I"C:\Users\wen_c\Documents\MCUXpressoIDE_11.6.1_8255\workspace\new\AGV-MICRO\device" -I"C:\Users\wen_c\Documents\MCUXpressoIDE_11.6.1_8255\workspace\new\AGV-MICRO\CMSIS" -I"C:\Users\wen_c\Documents\MCUXpressoIDE_11.6.1_8255\workspace\new\AGV-MICRO\utilities" -I"C:\Users\wen_c\Documents\MCUXpressoIDE_11.6.1_8255\workspace\new\AGV-MICRO\component\serial_manager" -I"C:\Users\wen_c\Documents\MCUXpressoIDE_11.6.1_8255\workspace\new\AGV-MICRO\component\uart" -I"C:\Users\wen_c\Documents\MCUXpressoIDE_11.6.1_8255\workspace\new\AGV-MICRO\usb\include" -I"C:\Users\wen_c\Documents\MCUXpressoIDE_11.6.1_8255\workspace\new\AGV-MICRO\component\lists" -I"C:\Users\wen_c\Documents\MCUXpressoIDE_11.6.1_8255\workspace\new\AGV-MICRO\component\osa" -I"C:\Users\wen_c\Documents\MCUXpressoIDE_11.6.1_8255\workspace\new\AGV-MICRO\board" -I"C:\Users\wen_c\Documents\MCUXpressoIDE_11.6.1_8255\workspace\new\AGV-MICRO\source\generated" -I"C:\Users\wen_c\Documents\MCUXpressoIDE_11.6.1_8255\workspace\new\AGV-MICRO\usb\device\class\cdc" -I"C:\Users\wen_c\Documents\MCUXpressoIDE_11.6.1_8255\workspace\new\AGV-MICRO\usb\device\include" -I"C:\Users\wen_c\Documents\MCUXpressoIDE_11.6.1_8255\workspace\new\AGV-MICRO\usb\device\source" -I"C:\Users\wen_c\Documents\MCUXpressoIDE_11.6.1_8255\workspace\new\AGV-MICRO\usb\device\source\khci" -I"C:\Users\wen_c\Documents\MCUXpressoIDE_11.6.1_8255\workspace\new\AGV-MICRO\usb\device\class" -I"C:\Users\wen_c\Documents\MCUXpressoIDE_11.6.1_8255\workspace\new\AGV-MICRO\drivers" -I"C:\Users\wen_c\Documents\MCUXpressoIDE_11.6.1_8255\workspace\new\AGV-MICRO\device" -I"C:\Users\wen_c\Documents\MCUXpressoIDE_11.6.1_8255\workspace\new\AGV-MICRO\CMSIS" -I"C:\Users\wen_c\Documents\MCUXpressoIDE_11.6.1_8255\workspace\new\AGV-MICRO\utilities" -I"C:\Users\wen_c\Documents\MCUXpressoIDE_11.6.1_8255\workspace\new\AGV-MICRO\component\serial_manager" -I"C:\Users\wen_c\Documents\MCUXpressoIDE_11.6.1_8255\workspace\new\AGV-MICRO\component\uart" -I"C:\Users\wen_c\Documents\MCUXpressoIDE_11.6.1_8255\workspace\new\AGV-MICRO\usb\include" -I"C:\Users\wen_c\Documents\MCUXpressoIDE_11.6.1_8255\workspace\new\AGV-MICRO\component\lists" -I"C:\Users\wen_c\Documents\MCUXpressoIDE_11.6.1_8255\workspace\new\AGV-MICRO\component\osa" -I"C:\Users\wen_c\Documents\MCUXpressoIDE_11.6.1_8255\workspace\new\AGV-MICRO\board" -I"C:\Users\wen_c\Documents\MCUXpressoIDE_11.6.1_8255\workspace\new\AGV-MICRO\source\generated" -I"C:\Users\wen_c\Documents\MCUXpressoIDE_11.6.1_8255\workspace\new\AGV-MICRO\usb\device\class\cdc" -I"C:\Users\wen_c\Documents\MCUXpressoIDE_11.6.1_8255\workspace\new\AGV-MICRO\usb\device\include" -I"C:\Users\wen_c\Documents\MCUXpressoIDE_11.6.1_8255\workspace\new\AGV-MICRO\usb\device\source" -I"C:\Users\wen_c\Documents\MCUXpressoIDE_11.6.1_8255\workspace\new\AGV-MICRO\usb\device\source\khci" -I"C:\Users\wen_c\Documents\MCUXpressoIDE_11.6.1_8255\workspace\new\AGV-MICRO\usb\device\class" -I"C:\Users\wen_c\Documents\MCUXpressoIDE_11.6.1_8255\workspace\new\AGV-MICRO\source" -O0 -fno-common -g3 -Wall -c -ffunction-sections -fdata-sections -ffreestanding -fno-builtin -fmerge-constants -fmacro-prefix-map="$(<D)/"= -mcpu=cortex-m4 -mfpu=fpv4-sp-d16 -mfloat-abi=hard -mthumb -D__REDLIB__ -fstack-usage -specs=redlib.specs -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.o)" -MT"$(@:%.o=%.d)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


clean: clean-usb-2f-device-2f-class

clean-usb-2f-device-2f-class:
	-$(RM) ./usb/device/class/usb_device_class.d ./usb/device/class/usb_device_class.o

.PHONY: clean-usb-2f-device-2f-class

