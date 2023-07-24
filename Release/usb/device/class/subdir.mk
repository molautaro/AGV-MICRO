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
	arm-none-eabi-gcc -DCPU_MK64FN1M0VLL12 -DCPU_MK64FN1M0VLL12_cm4 -DSDK_OS_BAREMETAL -DSDK_DEBUGCONSOLE=1 -DCR_INTEGER_PRINTF -DPRINTF_FLOAT_ENABLE=0 -DSERIAL_PORT_TYPE_UART=1 -D__MCUXPRESSO -D__USE_CMSIS -DNDEBUG -D__REDLIB__ -I"C:\Users\Lauta\Documents\MCUXpressoIDE_11.6.1_8255\workspace\test_usb\drivers" -I"C:\Users\Lauta\Documents\MCUXpressoIDE_11.6.1_8255\workspace\test_usb\device" -I"C:\Users\Lauta\Documents\MCUXpressoIDE_11.6.1_8255\workspace\test_usb\CMSIS" -I"C:\Users\Lauta\Documents\MCUXpressoIDE_11.6.1_8255\workspace\test_usb\utilities" -I"C:\Users\Lauta\Documents\MCUXpressoIDE_11.6.1_8255\workspace\test_usb\component\serial_manager" -I"C:\Users\Lauta\Documents\MCUXpressoIDE_11.6.1_8255\workspace\test_usb\component\uart" -I"C:\Users\Lauta\Documents\MCUXpressoIDE_11.6.1_8255\workspace\test_usb\usb\device\source\khci" -I"C:\Users\Lauta\Documents\MCUXpressoIDE_11.6.1_8255\workspace\test_usb\usb\include" -I"C:\Users\Lauta\Documents\MCUXpressoIDE_11.6.1_8255\workspace\test_usb\usb\device\include" -I"C:\Users\Lauta\Documents\MCUXpressoIDE_11.6.1_8255\workspace\test_usb\usb\device\source" -I"C:\Users\Lauta\Documents\MCUXpressoIDE_11.6.1_8255\workspace\test_usb\usb\device\class\cdc" -I"C:\Users\Lauta\Documents\MCUXpressoIDE_11.6.1_8255\workspace\test_usb\usb\output\source\device\class" -I"C:\Users\Lauta\Documents\MCUXpressoIDE_11.6.1_8255\workspace\test_usb\usb\device\class" -I"C:\Users\Lauta\Documents\MCUXpressoIDE_11.6.1_8255\workspace\test_usb\usb\output\source\device" -I"C:\Users\Lauta\Documents\MCUXpressoIDE_11.6.1_8255\workspace\test_usb\component\lists" -I"C:\Users\Lauta\Documents\MCUXpressoIDE_11.6.1_8255\workspace\test_usb\component\osa" -I"C:\Users\Lauta\Documents\MCUXpressoIDE_11.6.1_8255\workspace\test_usb\board" -I"C:\Users\Lauta\Documents\MCUXpressoIDE_11.6.1_8255\workspace\test_usb\source\generated" -I"C:\Users\Lauta\Documents\MCUXpressoIDE_11.6.1_8255\workspace\test_usb\drivers" -I"C:\Users\Lauta\Documents\MCUXpressoIDE_11.6.1_8255\workspace\test_usb\device" -I"C:\Users\Lauta\Documents\MCUXpressoIDE_11.6.1_8255\workspace\test_usb\CMSIS" -I"C:\Users\Lauta\Documents\MCUXpressoIDE_11.6.1_8255\workspace\test_usb\utilities" -I"C:\Users\Lauta\Documents\MCUXpressoIDE_11.6.1_8255\workspace\test_usb\component\serial_manager" -I"C:\Users\Lauta\Documents\MCUXpressoIDE_11.6.1_8255\workspace\test_usb\component\uart" -I"C:\Users\Lauta\Documents\MCUXpressoIDE_11.6.1_8255\workspace\test_usb\usb\device\source\khci" -I"C:\Users\Lauta\Documents\MCUXpressoIDE_11.6.1_8255\workspace\test_usb\usb\include" -I"C:\Users\Lauta\Documents\MCUXpressoIDE_11.6.1_8255\workspace\test_usb\usb\device\include" -I"C:\Users\Lauta\Documents\MCUXpressoIDE_11.6.1_8255\workspace\test_usb\usb\device\source" -I"C:\Users\Lauta\Documents\MCUXpressoIDE_11.6.1_8255\workspace\test_usb\usb\device\class\cdc" -I"C:\Users\Lauta\Documents\MCUXpressoIDE_11.6.1_8255\workspace\test_usb\usb\output\source\device\class" -I"C:\Users\Lauta\Documents\MCUXpressoIDE_11.6.1_8255\workspace\test_usb\usb\device\class" -I"C:\Users\Lauta\Documents\MCUXpressoIDE_11.6.1_8255\workspace\test_usb\usb\output\source\device" -I"C:\Users\Lauta\Documents\MCUXpressoIDE_11.6.1_8255\workspace\test_usb\component\lists" -I"C:\Users\Lauta\Documents\MCUXpressoIDE_11.6.1_8255\workspace\test_usb\component\osa" -I"C:\Users\Lauta\Documents\MCUXpressoIDE_11.6.1_8255\workspace\test_usb\board" -I"C:\Users\Lauta\Documents\MCUXpressoIDE_11.6.1_8255\workspace\test_usb\source\generated" -I"C:\Users\Lauta\Documents\MCUXpressoIDE_11.6.1_8255\workspace\test_usb\source" -Os -fno-common -g -Wall -c -ffunction-sections -fdata-sections -ffreestanding -fno-builtin -fmacro-prefix-map="$(<D)/"= -mcpu=cortex-m4 -mfpu=fpv4-sp-d16 -mfloat-abi=hard -mthumb -D__REDLIB__ -fstack-usage -specs=redlib.specs -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.o)" -MT"$(@:%.o=%.d)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


clean: clean-usb-2f-device-2f-class

clean-usb-2f-device-2f-class:
	-$(RM) ./usb/device/class/usb_device_class.d ./usb/device/class/usb_device_class.o

.PHONY: clean-usb-2f-device-2f-class

