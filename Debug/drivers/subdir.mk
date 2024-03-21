################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../drivers/fsl_adc16.c \
../drivers/fsl_clock.c \
../drivers/fsl_common.c \
../drivers/fsl_common_arm.c \
../drivers/fsl_flexcan.c \
../drivers/fsl_gpio.c \
../drivers/fsl_i2c.c \
../drivers/fsl_pit.c \
../drivers/fsl_smc.c \
../drivers/fsl_sysmpu.c \
../drivers/fsl_uart.c 

C_DEPS += \
./drivers/fsl_adc16.d \
./drivers/fsl_clock.d \
./drivers/fsl_common.d \
./drivers/fsl_common_arm.d \
./drivers/fsl_flexcan.d \
./drivers/fsl_gpio.d \
./drivers/fsl_i2c.d \
./drivers/fsl_pit.d \
./drivers/fsl_smc.d \
./drivers/fsl_sysmpu.d \
./drivers/fsl_uart.d 

OBJS += \
./drivers/fsl_adc16.o \
./drivers/fsl_clock.o \
./drivers/fsl_common.o \
./drivers/fsl_common_arm.o \
./drivers/fsl_flexcan.o \
./drivers/fsl_gpio.o \
./drivers/fsl_i2c.o \
./drivers/fsl_pit.o \
./drivers/fsl_smc.o \
./drivers/fsl_sysmpu.o \
./drivers/fsl_uart.o 


# Each subdirectory must supply rules for building sources it contributes
drivers/%.o: ../drivers/%.c drivers/subdir.mk
	@echo 'Building file: $<'
	@echo 'Invoking: MCU C Compiler'
	arm-none-eabi-gcc -D__REDLIB__ -DCPU_MK64FN1M0VLL12 -DCPU_MK64FN1M0VLL12_cm4 -DSDK_OS_BAREMETAL -DSDK_DEBUGCONSOLE=1 -DCR_INTEGER_PRINTF -DPRINTF_FLOAT_ENABLE=0 -DSERIAL_PORT_TYPE_UART=1 -D__MCUXPRESSO -D__USE_CMSIS -DDEBUG -I"C:\Users\wen_c\Documents\MCUXpressoIDE_11.6.1_8255\workspace\test_usb\drivers" -I"C:\Users\wen_c\Documents\MCUXpressoIDE_11.6.1_8255\workspace\test_usb\device" -I"C:\Users\wen_c\Documents\MCUXpressoIDE_11.6.1_8255\workspace\test_usb\CMSIS" -I"C:\Users\wen_c\Documents\MCUXpressoIDE_11.6.1_8255\workspace\test_usb\utilities" -I"C:\Users\wen_c\Documents\MCUXpressoIDE_11.6.1_8255\workspace\test_usb\component\serial_manager" -I"C:\Users\wen_c\Documents\MCUXpressoIDE_11.6.1_8255\workspace\test_usb\component\uart" -I"C:\Users\wen_c\Documents\MCUXpressoIDE_11.6.1_8255\workspace\test_usb\usb\device\source\khci" -I"C:\Users\wen_c\Documents\MCUXpressoIDE_11.6.1_8255\workspace\test_usb\usb\include" -I"C:\Users\wen_c\Documents\MCUXpressoIDE_11.6.1_8255\workspace\test_usb\usb\device\include" -I"C:\Users\wen_c\Documents\MCUXpressoIDE_11.6.1_8255\workspace\test_usb\usb\device\source" -I"C:\Users\wen_c\Documents\MCUXpressoIDE_11.6.1_8255\workspace\test_usb\usb\device\class\cdc" -I"C:\Users\wen_c\Documents\MCUXpressoIDE_11.6.1_8255\workspace\test_usb\usb\device\class" -I"C:\Users\wen_c\Documents\MCUXpressoIDE_11.6.1_8255\workspace\test_usb\component\lists" -I"C:\Users\wen_c\Documents\MCUXpressoIDE_11.6.1_8255\workspace\test_usb\component\osa" -I"C:\Users\wen_c\Documents\MCUXpressoIDE_11.6.1_8255\workspace\test_usb\board" -I"C:\Users\wen_c\Documents\MCUXpressoIDE_11.6.1_8255\workspace\test_usb\source\generated" -I"C:\Users\wen_c\Documents\MCUXpressoIDE_11.6.1_8255\workspace\test_usb\drivers" -I"C:\Users\wen_c\Documents\MCUXpressoIDE_11.6.1_8255\workspace\test_usb\device" -I"C:\Users\wen_c\Documents\MCUXpressoIDE_11.6.1_8255\workspace\test_usb\CMSIS" -I"C:\Users\wen_c\Documents\MCUXpressoIDE_11.6.1_8255\workspace\test_usb\utilities" -I"C:\Users\wen_c\Documents\MCUXpressoIDE_11.6.1_8255\workspace\test_usb\component\serial_manager" -I"C:\Users\wen_c\Documents\MCUXpressoIDE_11.6.1_8255\workspace\test_usb\component\uart" -I"C:\Users\wen_c\Documents\MCUXpressoIDE_11.6.1_8255\workspace\test_usb\usb\device\source\khci" -I"C:\Users\wen_c\Documents\MCUXpressoIDE_11.6.1_8255\workspace\test_usb\usb\include" -I"C:\Users\wen_c\Documents\MCUXpressoIDE_11.6.1_8255\workspace\test_usb\usb\device\include" -I"C:\Users\wen_c\Documents\MCUXpressoIDE_11.6.1_8255\workspace\test_usb\usb\device\source" -I"C:\Users\wen_c\Documents\MCUXpressoIDE_11.6.1_8255\workspace\test_usb\usb\device\class\cdc" -I"C:\Users\wen_c\Documents\MCUXpressoIDE_11.6.1_8255\workspace\test_usb\usb\device\class" -I"C:\Users\wen_c\Documents\MCUXpressoIDE_11.6.1_8255\workspace\test_usb\component\lists" -I"C:\Users\wen_c\Documents\MCUXpressoIDE_11.6.1_8255\workspace\test_usb\component\osa" -I"C:\Users\wen_c\Documents\MCUXpressoIDE_11.6.1_8255\workspace\test_usb\board" -I"C:\Users\wen_c\Documents\MCUXpressoIDE_11.6.1_8255\workspace\test_usb\source\generated" -I"C:\Users\wen_c\Documents\MCUXpressoIDE_11.6.1_8255\workspace\test_usb\source" -O0 -fno-common -g3 -Wall -c -ffunction-sections -fdata-sections -ffreestanding -fno-builtin -fmerge-constants -fmacro-prefix-map="$(<D)/"= -mcpu=cortex-m4 -mfpu=fpv4-sp-d16 -mfloat-abi=hard -mthumb -D__REDLIB__ -fstack-usage -specs=redlib.specs -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.o)" -MT"$(@:%.o=%.d)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


clean: clean-drivers

clean-drivers:
	-$(RM) ./drivers/fsl_adc16.d ./drivers/fsl_adc16.o ./drivers/fsl_clock.d ./drivers/fsl_clock.o ./drivers/fsl_common.d ./drivers/fsl_common.o ./drivers/fsl_common_arm.d ./drivers/fsl_common_arm.o ./drivers/fsl_flexcan.d ./drivers/fsl_flexcan.o ./drivers/fsl_gpio.d ./drivers/fsl_gpio.o ./drivers/fsl_i2c.d ./drivers/fsl_i2c.o ./drivers/fsl_pit.d ./drivers/fsl_pit.o ./drivers/fsl_smc.d ./drivers/fsl_smc.o ./drivers/fsl_sysmpu.d ./drivers/fsl_sysmpu.o ./drivers/fsl_uart.d ./drivers/fsl_uart.o

.PHONY: clean-drivers

