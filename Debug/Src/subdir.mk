################################################################################
# Automatically-generated file. Do not edit!
# Toolchain: GNU Tools for STM32 (13.3.rel1)
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../Src/encoder.c \
../Src/main.c \
../Src/protocol.c \
../Src/pwm.c \
../Src/syscalls.c \
../Src/sysmem.c \
../Src/systick.c \
../Src/tim.c \
../Src/uart.c \
../Src/uart6.c \
../Src/x_pid.c \
../Src/y_pid.c 

OBJS += \
./Src/encoder.o \
./Src/main.o \
./Src/protocol.o \
./Src/pwm.o \
./Src/syscalls.o \
./Src/sysmem.o \
./Src/systick.o \
./Src/tim.o \
./Src/uart.o \
./Src/uart6.o \
./Src/x_pid.o \
./Src/y_pid.o 

C_DEPS += \
./Src/encoder.d \
./Src/main.d \
./Src/protocol.d \
./Src/pwm.d \
./Src/syscalls.d \
./Src/sysmem.d \
./Src/systick.d \
./Src/tim.d \
./Src/uart.d \
./Src/uart6.d \
./Src/x_pid.d \
./Src/y_pid.d 


# Each subdirectory must supply rules for building sources it contributes
Src/%.o Src/%.su Src/%.cyclo: ../Src/%.c Src/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m4 -std=gnu11 -g3 -DDEBUG -DSTM32 -DSTM32F4 -DSTM32F446RETx -DNUCLEO_F446RE -DSTM32F446xx -c -I../Inc -I/home/kridega/stm/f4x/STM32NUCLEOF446RE/chip_headers/CMSIS/Include -I/home/kridega/stm/f4x/STM32NUCLEOF446RE/chip_headers/CMSIS/Device/ST/STM32F4xx/Include -O0 -ffunction-sections -fdata-sections -Wall -fstack-usage -fcyclomatic-complexity -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfpu=fpv4-sp-d16 -mfloat-abi=hard -mthumb -o "$@"

clean: clean-Src

clean-Src:
	-$(RM) ./Src/encoder.cyclo ./Src/encoder.d ./Src/encoder.o ./Src/encoder.su ./Src/main.cyclo ./Src/main.d ./Src/main.o ./Src/main.su ./Src/protocol.cyclo ./Src/protocol.d ./Src/protocol.o ./Src/protocol.su ./Src/pwm.cyclo ./Src/pwm.d ./Src/pwm.o ./Src/pwm.su ./Src/syscalls.cyclo ./Src/syscalls.d ./Src/syscalls.o ./Src/syscalls.su ./Src/sysmem.cyclo ./Src/sysmem.d ./Src/sysmem.o ./Src/sysmem.su ./Src/systick.cyclo ./Src/systick.d ./Src/systick.o ./Src/systick.su ./Src/tim.cyclo ./Src/tim.d ./Src/tim.o ./Src/tim.su ./Src/uart.cyclo ./Src/uart.d ./Src/uart.o ./Src/uart.su ./Src/uart6.cyclo ./Src/uart6.d ./Src/uart6.o ./Src/uart6.su ./Src/x_pid.cyclo ./Src/x_pid.d ./Src/x_pid.o ./Src/x_pid.su ./Src/y_pid.cyclo ./Src/y_pid.d ./Src/y_pid.o ./Src/y_pid.su

.PHONY: clean-Src

