################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
S_SRCS += \
../src/lab0.s 

C_SRCS += \
../src/syscalls.c \
../src/system_stm32f0xx.c 

O_SRCS += \
../src/autotest.o 

OBJS += \
./src/lab0.o \
./src/syscalls.o \
./src/system_stm32f0xx.o 

C_DEPS += \
./src/syscalls.d \
./src/system_stm32f0xx.d 


# Each subdirectory must supply rules for building sources it contributes
src/%.o: ../src/%.s
	@echo 'Building file: $<'
	@echo 'Invoking: MCU GCC Assembler'
	@echo $(PWD)
	arm-none-eabi-as -mcpu=cortex-m0 -mthumb -mfloat-abi=soft -I"/Users/raghuramselvaraj/Current Classes/ECE362/lab0/StdPeriph_Driver/inc" -I"/Users/raghuramselvaraj/Current Classes/ECE362/lab0/inc" -I"/Users/raghuramselvaraj/Current Classes/ECE362/lab0/CMSIS/device" -I"/Users/raghuramselvaraj/Current Classes/ECE362/lab0/CMSIS/core" -g -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '

src/%.o: ../src/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: MCU GCC Compiler'
	@echo $(PWD)
	arm-none-eabi-gcc -mcpu=cortex-m0 -mthumb -mfloat-abi=soft -DSTM32 -DSTM32F0 -DSTM32F091RCTx -DDEBUG -DSTM32F091 -DUSE_STDPERIPH_DRIVER -I"/Users/raghuramselvaraj/Current Classes/ECE362/lab0/StdPeriph_Driver/inc" -I"/Users/raghuramselvaraj/Current Classes/ECE362/lab0/inc" -I"/Users/raghuramselvaraj/Current Classes/ECE362/lab0/CMSIS/device" -I"/Users/raghuramselvaraj/Current Classes/ECE362/lab0/CMSIS/core" -O0 -g3 -Wall -fmessage-length=0 -ffunction-sections -c -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


