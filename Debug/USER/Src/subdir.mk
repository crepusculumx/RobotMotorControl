################################################################################
# Automatically-generated file. Do not edit!
# Toolchain: GNU Tools for STM32 (9-2020-q2-update)
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../USER/Src/ComProtocol.c \
../USER/Src/CrcCheck.c \
../USER/Src/Deque.c \
../USER/Src/GpioDriver.c \
../USER/Src/MotorController.c \
../USER/Src/MotorDriver.c \
../USER/Src/PwmDriver.c \
../USER/Src/TypeChanger.c 

OBJS += \
./USER/Src/ComProtocol.o \
./USER/Src/CrcCheck.o \
./USER/Src/Deque.o \
./USER/Src/GpioDriver.o \
./USER/Src/MotorController.o \
./USER/Src/MotorDriver.o \
./USER/Src/PwmDriver.o \
./USER/Src/TypeChanger.o 

C_DEPS += \
./USER/Src/ComProtocol.d \
./USER/Src/CrcCheck.d \
./USER/Src/Deque.d \
./USER/Src/GpioDriver.d \
./USER/Src/MotorController.d \
./USER/Src/MotorDriver.d \
./USER/Src/PwmDriver.d \
./USER/Src/TypeChanger.d 


# Each subdirectory must supply rules for building sources it contributes
USER/Src/%.o: ../USER/Src/%.c USER/Src/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m3 -std=gnu11 -g3 -DDEBUG -DUSE_HAL_DRIVER -DSTM32F103xB -c -I../Core/Inc -I../Drivers/STM32F1xx_HAL_Driver/Inc -I../Drivers/STM32F1xx_HAL_Driver/Inc/Legacy -I../Drivers/CMSIS/Device/ST/STM32F1xx/Include -I../Drivers/CMSIS/Include -I"/home/crepusculumx/cubeMX-ws/RobotMotorControl/USER/Inc" -O0 -ffunction-sections -fdata-sections -Wall -fstack-usage -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfloat-abi=soft -mthumb -o "$@"

