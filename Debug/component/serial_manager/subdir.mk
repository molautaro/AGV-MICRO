################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../component/serial_manager/fsl_component_serial_manager.c \
../component/serial_manager/fsl_component_serial_port_uart.c 

C_DEPS += \
./component/serial_manager/fsl_component_serial_manager.d \
./component/serial_manager/fsl_component_serial_port_uart.d 

OBJS += \
./component/serial_manager/fsl_component_serial_manager.o \
./component/serial_manager/fsl_component_serial_port_uart.o 


# Each subdirectory must supply rules for building sources it contributes
component/serial_manager/%.o: ../component/serial_manager/%.c component/serial_manager/subdir.mk
	@echo 'Building file: $<'
	@echo 'Invoking: MCU C Compiler'
	arm-none-eabi-gcc -D__REDLIB__ -DCPU_MK64FN1M0VLL12 -DCPU_MK64FN1M0VLL12_cm4 -DSDK_OS_BAREMETAL -DSDK_DEBUGCONSOLE=1 -DCR_INTEGER_PRINTF -DPRINTF_FLOAT_ENABLE=0 -DSERIAL_PORT_TYPE_UART=1 -D__MCUXPRESSO -D__USE_CMSIS -DDEBUG -I"C:\Users\wen_c\Documents\MCUXpressoIDE_11.6.1_8255\workspace\test_usb\drivers" -I"C:\Users\wen_c\Documents\MCUXpressoIDE_11.6.1_8255\workspace\test_usb\device" -I"C:\Users\wen_c\Documents\MCUXpressoIDE_11.6.1_8255\workspace\test_usb\CMSIS" -I"C:\Users\wen_c\Documents\MCUXpressoIDE_11.6.1_8255\workspace\test_usb\utilities" -I"C:\Users\wen_c\Documents\MCUXpressoIDE_11.6.1_8255\workspace\test_usb\component\serial_manager" -I"C:\Users\wen_c\Documents\MCUXpressoIDE_11.6.1_8255\workspace\test_usb\component\uart" -I"C:\Users\wen_c\Documents\MCUXpressoIDE_11.6.1_8255\workspace\test_usb\usb\device\source\khci" -I"C:\Users\wen_c\Documents\MCUXpressoIDE_11.6.1_8255\workspace\test_usb\usb\include" -I"C:\Users\wen_c\Documents\MCUXpressoIDE_11.6.1_8255\workspace\test_usb\usb\device\include" -I"C:\Users\wen_c\Documents\MCUXpressoIDE_11.6.1_8255\workspace\test_usb\usb\device\source" -I"C:\Users\wen_c\Documents\MCUXpressoIDE_11.6.1_8255\workspace\test_usb\usb\device\class\cdc" -I"C:\Users\wen_c\Documents\MCUXpressoIDE_11.6.1_8255\workspace\test_usb\usb\device\class" -I"C:\Users\wen_c\Documents\MCUXpressoIDE_11.6.1_8255\workspace\test_usb\component\lists" -I"C:\Users\wen_c\Documents\MCUXpressoIDE_11.6.1_8255\workspace\test_usb\component\osa" -I"C:\Users\wen_c\Documents\MCUXpressoIDE_11.6.1_8255\workspace\test_usb\board" -I"C:\Users\wen_c\Documents\MCUXpressoIDE_11.6.1_8255\workspace\test_usb\source\generated" -I"C:\Users\wen_c\Documents\MCUXpressoIDE_11.6.1_8255\workspace\test_usb\drivers" -I"C:\Users\wen_c\Documents\MCUXpressoIDE_11.6.1_8255\workspace\test_usb\device" -I"C:\Users\wen_c\Documents\MCUXpressoIDE_11.6.1_8255\workspace\test_usb\CMSIS" -I"C:\Users\wen_c\Documents\MCUXpressoIDE_11.6.1_8255\workspace\test_usb\utilities" -I"C:\Users\wen_c\Documents\MCUXpressoIDE_11.6.1_8255\workspace\test_usb\component\serial_manager" -I"C:\Users\wen_c\Documents\MCUXpressoIDE_11.6.1_8255\workspace\test_usb\component\uart" -I"C:\Users\wen_c\Documents\MCUXpressoIDE_11.6.1_8255\workspace\test_usb\usb\device\source\khci" -I"C:\Users\wen_c\Documents\MCUXpressoIDE_11.6.1_8255\workspace\test_usb\usb\include" -I"C:\Users\wen_c\Documents\MCUXpressoIDE_11.6.1_8255\workspace\test_usb\usb\device\include" -I"C:\Users\wen_c\Documents\MCUXpressoIDE_11.6.1_8255\workspace\test_usb\usb\device\source" -I"C:\Users\wen_c\Documents\MCUXpressoIDE_11.6.1_8255\workspace\test_usb\usb\device\class\cdc" -I"C:\Users\wen_c\Documents\MCUXpressoIDE_11.6.1_8255\workspace\test_usb\usb\device\class" -I"C:\Users\wen_c\Documents\MCUXpressoIDE_11.6.1_8255\workspace\test_usb\component\lists" -I"C:\Users\wen_c\Documents\MCUXpressoIDE_11.6.1_8255\workspace\test_usb\component\osa" -I"C:\Users\wen_c\Documents\MCUXpressoIDE_11.6.1_8255\workspace\test_usb\board" -I"C:\Users\wen_c\Documents\MCUXpressoIDE_11.6.1_8255\workspace\test_usb\source\generated" -I"C:\Users\wen_c\Documents\MCUXpressoIDE_11.6.1_8255\workspace\test_usb\source" -O0 -fno-common -g3 -Wall -c -ffunction-sections -fdata-sections -ffreestanding -fno-builtin -fmerge-constants -fmacro-prefix-map="$(<D)/"= -mcpu=cortex-m4 -mfpu=fpv4-sp-d16 -mfloat-abi=hard -mthumb -D__REDLIB__ -fstack-usage -specs=redlib.specs -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.o)" -MT"$(@:%.o=%.d)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


clean: clean-component-2f-serial_manager

clean-component-2f-serial_manager:
	-$(RM) ./component/serial_manager/fsl_component_serial_manager.d ./component/serial_manager/fsl_component_serial_manager.o ./component/serial_manager/fsl_component_serial_port_uart.d ./component/serial_manager/fsl_component_serial_port_uart.o

.PHONY: clean-component-2f-serial_manager

