################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
S_SRCS += \
../startup/startup_stm32.s 

OBJS += \
./startup/startup_stm32.o 


# Each subdirectory must supply rules for building sources it contributes
startup/%.o: ../startup/%.s
	@echo 'Building file: $<'
	@echo 'Invoking: MCU GCC Assembler'
	@echo $(PWD)
	arm-none-eabi-as -mcpu=cortex-m0 -mthumb -mfloat-abi=soft -I"/Users/raghuramselvaraj/Current Classes/ECE362/lab3/StdPeriph_Driver/inc" -I"/Users/raghuramselvaraj/Current Classes/ECE362/lab3/inc" -I"/Users/raghuramselvaraj/Current Classes/ECE362/lab3/CMSIS/device" -I"/Users/raghuramselvaraj/Current Classes/ECE362/lab3/CMSIS/core" -g -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


